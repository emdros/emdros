/*
 * pgconn.cpp
 *
 * PostgreSQL Database connection 
 *
 * Ulrik Petersen
 * Created: 1/27-2001
 * Last update: 4/13-2016
 *
 */

/*
 * Copyright (C) 2001-2018     Ulrik Sandborg-Petersen
 * Copyright (C) 2018-present  Sandborg-Petersen Holding ApS, Denmark
 *
 * Licensed under the MIT License.
 *
 * Please see the file COPYING in the root of the sources for more details.
 *
 */


/**@file pgconn.cpp
 *@brief Implementation of PostgreSQL connection (EMdF layer)
 */



#include <utils.h>
#include <debug.h>
#include <cstdlib>
#include <pgconn.h>
#include <logging.h>
#include <iostream>
#include <encryption.h>
#include <cstdlib>
#include <string>
#include <string_func.h>
#include <sstream>




PgEMdFConnection::PgEMdFConnection(const std::string& host, const std::string& user, const std::string& passwd, const std::string& database_name)
  : m_pConn(NULL), m_pResult(NULL), m_bTransactionInProgress(false)
{
  // Use database emdf if database name not given
  std::string db;
  if (database_name.length() == 0) {
    db = "emdf";
  } else {
    db = database_name;
  }
  m_current_db_name = db;

  // Store host
  m_host = host;

  // Store user
  m_user = user;

  // Store and encrypt password
  m_password = passwd;
  encryptString(m_password);
  


  // Connect if necessary?
  if (strcmp_nocase(db, "emdf") != 0) {
    connect();
  }
}

void PgEMdFConnection::connect()
{
#if USE_POSTGRESQL
  if (strcmp_nocase(m_current_db_name, "emdf") == 0) {
    disconnect();
  } else {
    std::ostringstream my_connection_str;
    
    // Make sure we are disconnected
    disconnect();
    
    // Make connection string
    my_connection_str 
      << "host='" << m_host 
      << "' user='" << m_user << "' ";
    unencryptString(m_password);
    if (m_password != "") {
      my_connection_str << "password='" << m_password << "' ";
    }
    encryptString(m_password);
    my_connection_str << "dbname='" << m_current_db_name << "'";
    
    // Connect
    m_pConn = PQconnectdb(my_connection_str.str().c_str());
  }
#endif
}
  
void PgEMdFConnection::disconnect()
{
#if USE_POSTGRESQL
  // Commit transaction if in progress
  if (m_bTransactionInProgress)
    commitTransaction();
  
  // Remove result
  finalize();
  
  // Close down connection
  if (m_pConn != 0) 
    PQfinish((PGconn*) m_pConn);
  m_pConn=NULL;
#endif
}

PgEMdFConnection::~PgEMdFConnection()
{
#if USE_POSTGRESQL
  disconnect();
#endif
}


// Status of connection
// Returns true on connection good, false on connection bad
// Should be called right after constructor to see if we
// succeeded in setting up the connection.
bool PgEMdFConnection::connectionOk(void)
{
#if USE_POSTGRESQL
  // If we are currently "connected" to the emdf database,
  // then the connection is, by definition, OK.
  if (strcmp_nocase(m_current_db_name, "emdf") == 0) {
    return true;
  } else if (m_pConn == 0) {
    return false;
  } else {
    return PQstatus((PGconn*) m_pConn) != CONNECTION_BAD;
  }
#else
  return false;
#endif
}


// Executing
bool PgEMdFConnection::execSelect(const std::string& query)
{
#if USE_POSTGRESQL
  if (m_pConn == 0)
    return false;
  else {
    // Check that connection is OK
    if (!connectionOk())
      return false;

    // Write to log
#if EMDROS_LOGGING
    std::string log = "Starting query.\n" + query;
    LOG_WRITE_TIME("PgEMdFConnection::execSelect", log);
#endif

    // Clear the Result Stucture if needed
    finalize();

    // Execute
    ExecStatusType status;
    m_pResult = PQexec((PGconn*) m_pConn, query.c_str());
  
    // Return the status
    if (m_pResult != NULL)
      status = PQresultStatus((PGresult*) m_pResult);
    else 
      status = PGRES_FATAL_ERROR;

    // Write to log
    LOG_WRITE_TIME("PgEMdFConnection::execSelect", "Query finished.");

    if ((status == PGRES_TUPLES_OK) || (status == PGRES_COMMAND_OK)) {
      // It was a success

      // Set last_tuple to show how many results, 
      // and set current tuple to the first tuple
      m_current_last_tuple = noOfResultTuples() - 1;
      m_current_tuple = 0;

      // Return success
      return true;
    } else {
      // It was a failure

      // Set values to show there are no results
      m_current_last_tuple = -1;
      m_current_tuple = 0;

      // Return failure
      return false;
    }
  }
#else
  return false;
#endif
}


// Executing
bool PgEMdFConnection::execCommand(const std::string& query)
{
#if USE_POSTGRESQL
  if (m_pConn == 0)
    return false;
  else {
    // Check that connection is OK
    if (!connectionOk())
      return false;

    // Write to log
#if EMDROS_LOGGING
    std::string log = "Starting query.\n" + query;
    LOG_WRITE_TIME("PgEMdFConnection::execCommand", log);
#endif

    // Clear the Result Stucture if needed
    finalize();

    // Execute
    ExecStatusType status;
    m_pResult = PQexec((PGconn*) m_pConn, query.c_str());
  
    // Return the status
    if (m_pResult != NULL)
      status = PQresultStatus((PGresult*) m_pResult);
    else 
      status = PGRES_FATAL_ERROR;

    // Write to log
    LOG_WRITE_TIME("PgEMdFConnection::execCommand", "Query finished.");

    // Set values to show there are no results
    m_current_last_tuple = -1;
    m_current_tuple = 0;

    // Return true or false based on status
    return (status == PGRES_COMMAND_OK) 
      || (status == PGRES_TUPLES_OK)
      || (status == PGRES_COPY_IN);
  }
#else
  return false;
#endif
}

bool PgEMdFConnection::mayCOPY(void)
{
#if USE_POSTGRESQL
  if (m_pConn == 0) {
    return false;
  } else {
    return PQresultStatus((PGresult*) m_pResult) == PGRES_COPY_IN;
  }
#else
  return false;
#endif
}

bool PgEMdFConnection::execCOPY(std::string& copy_data)
{
#if USE_POSTGRESQL
  if (m_pConn == 0)
    return false;
  else {
    // Check that we are in the state for copying.
    if (!mayCOPY()) {
      return false;
    }

    // Make sure that the calls below block!
    PQsetnonblocking((PGconn*) m_pConn, 0);

    // Write to log
    /*
#if EMDROS_LOGGING
    std::string log = "Starting COPY-query.\n" + copy_data;
    LOG_WRITE_TIME("PgEMdFConnection::execCOPY", log);
#endif
    */

    // Clear the Result Stucture if needed
    finalize();

    // Execute
#line 1 "EMdF/pgconn.cpp"
    std::string current_line;
    std::string::size_type pos = 0;
    std::string::size_type data_length = copy_data.length();

    const char *cstr_copy_data = copy_data.c_str();
    const std::string::size_type PG_EXEC_COPY_BUFFER_LENGTH = 16384;
    while (true) {
	    std::string::size_type buffer_length = PG_EXEC_COPY_BUFFER_LENGTH;
	    if ((pos + PG_EXEC_COPY_BUFFER_LENGTH) > data_length) {
		    buffer_length = data_length - pos;
	    }
	    int result = PQputCopyData((PGconn*) m_pConn, cstr_copy_data + pos, buffer_length);
	    pos += buffer_length;
	    
	    if (result != 1) {
		    // Make sure we clean up
		    PQendcopy((PGconn*) m_pConn);
		    
		    // An error occurred
		    return false;
	    } 

	    if (pos >= data_length) {
		    break;
	    }
    }
    
    // Write to log
    LOG_WRITE_TIME("PgEMdFConnection::execCOPY", "COPY query finished.");

    while (true) {
	    int result = PQputCopyEnd((PGconn*) m_pConn, NULL);

	    if (result == 1) {
		    // Data was sent OK
		    break;
	    } else {
		    // An error occurred
		    return false;
	    }
    }

    // Clear the Result Stucture if needed
    finalize();

    // Set values to show there are no results
    m_current_last_tuple = -1;
    m_current_tuple = 0;

    // Return no DB error
    return true;
  }
#else
  return false;
#endif
}


// Get last error message
// Returns true on error message available 
// (even if it is "OK"), false on error 
// message not available.
bool PgEMdFConnection::errorMessage(std::string& out)
{
#if USE_POSTGRESQL
  if (m_pConn == 0) {
    return false;
  } else {
    out = (const char*) PQerrorMessage((PGconn*) m_pConn); 

    if (out.length() > 0)
      return true;
    else
      return false;
  }
#else
  return false;
#endif
}


// Transactions
bool PgEMdFConnection::beginTransaction(void)
{
  if (m_pConn == 0)
    return false;
  else {
    if (m_bTransactionInProgress) {
      return false;
    } else {
      m_bTransactionInProgress = true;
      return execCommand("BEGIN");
    }
  }
}

bool PgEMdFConnection::commitTransaction(void)
{
#if USE_POSTGRESQL
  if (m_pConn == 0)
    return false;
  else {
    if (m_bTransactionInProgress) {
      m_bTransactionInProgress = false;
      return execCommand("END");
    } else {
      //      std::cerr << "Warning: PgEMdFConnection::commitTransaction: No transaction in progress." << std::endl << "... Something is wrong in the logic! ..." << std::endl;
      return false;
    }
  }
#else
  return false;
#endif
}

bool PgEMdFConnection::abortTransaction(void)
{
#if USE_POSTGRESQL
  if (m_pConn == 0)
    return false;
  else {
    if (m_bTransactionInProgress) {
      m_bTransactionInProgress = false;
      return execCommand("ROLLBACK");
    } else {
      std::cerr << "Warning: PgEMdFConnection::abortTransaction: No transaction in progress." << std::endl << "... Something is wrong in the logic! ..." << std::endl;
      return false;
    }
  }
#else
  return false;
#endif
}


bool PgEMdFConnection::vacuum(bool bAnalyze)
{
#if USE_POSTGRESQL
  if (m_pConn == 0)
    return false;
  else {
    std::string Query = "VACUUM";
    if (bAnalyze)
      Query += " ANALYZE";
    return execCommand(Query);
  }
#else
  return false;
#endif
}

