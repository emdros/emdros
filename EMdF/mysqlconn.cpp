/*
 * mysqlconn.cpp
 *
 * MySQL Database connection 
 *
 * Ulrik Petersen
 * Created: 1/27-2001
 * Last update: 4/17-2018
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


/**@file mysqlconn.cpp
 *@brief Implementation of MySQL connection (EMdF layer)
 */


#ifdef _WIN32
#include <windows.h>
#endif

#include <mysqlconn.h>
#include <debug.h>
#include <string_func.h>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <logging.h>
#include <encryption.h>


MySQLEMdFConnection::MySQLEMdFConnection(const std::string& host, const std::string& user, const std::string& passwd, const std::string& db)
{
#if USE_MYSQL
	// We aren't doing any transaction
	m_bTransactionInProgress = false;

	// We don't have any result
	pResultSet = NULL;
	m_Row = NULL;

	// We dont' have any connection
	pConn = NULL;

	// Get db
	if (db.length() == 0) {
		m_db = "emdf";
	} else {
		convertDBName(db, m_db);
	}

	// Set m_host, m_user, m_passwd
	m_host = host;
	m_user = user;
	m_passwd = passwd;

	// Encrypt password
	encryptString(m_passwd);

	// connect using the member variables just set
	connect();
#endif
}

MySQLEMdFConnection::~MySQLEMdFConnection()
{
#if USE_MYSQL
	shutdownConnection();
#endif
}

void MySQLEMdFConnection::shutdownConnection()
{
#if USE_MYSQL
	if (pConn != NULL) {
		// Commit transaction if in progress
		if (m_bTransactionInProgress)
			commitTransaction();
    
		// Do away with result set
		freeResult();
    
		// Close the connection and deallocate the MYSQL structure
		mysql_close(pConn);

		// Just to be safe...
		pConn = NULL;
	}
#endif
}


// Status of connection
// Returns true on connection good, false on connection bad
// Should be called right after constructor to see if we
// succeeded in setting up the connection.
bool MySQLEMdFConnection::connectionOk(void)
{
#if USE_MYSQL
	if (pConn == NULL)
		return false;
	else {
		return mysql_ping(pConn) == 0;
	}
#else
	return false;
#endif
}


// Executing SELECT
bool MySQLEMdFConnection::execSelect(const std::string& query)
{
#if USE_MYSQL
	if (pConn == 0) {
		return false;
	} else {
		// Do away with previous pResultSet
		freeResult();
    
		// Write to log
		LOG_WRITE_TIME("MySQLEMdFConnection::execSelect", std::string("Starting query.\n") + query);
    
		// Execute query
		if (mysql_query(pConn, query.c_str()) != 0) {
			// Write to log
			LOG_WRITE_TIME("MySQLEMdFConnection::execSelect", "Query finished with failure.");

			// Error: mysql_query did not return 0.
			return false;
		} else {
			// No error

			// Write to log
			LOG_WRITE_TIME("MySQLEMdFConnection::execSelect", "Query finished with success.");

			// Use result
			LOG_WRITE_TIME("MySQLEMdFConnection::execSelect", "Using result...");
			pResultSet = mysql_use_result(pConn);
			LOG_WRITE_TIME("MySQLEMdFConnection::execSelect", "Using result finished.");
			if (mysql_errno(pConn) != 0) {
				LOG_WRITE_TIME("MySQLEMdFConnection::execSelect", "Using result failed.");
				// An error occurred
				return false;
			} else {
				// No error occurred

				// Get first row
				bool bMoreRows;
				if (!getNextTuple(bMoreRows)) {
					// An error occurred
					return false;
				}

				// Get number of fields
				m_num_fields = mysql_field_count(pConn);

				// Return success
				return true;
			} 
		}
	}
#else
	return false;
#endif
}

// Executing all others but SELECT
bool MySQLEMdFConnection::execCommand(const std::string& query)
{
#if USE_MYSQL
	if (pConn == 0) {
		return false;
	} else {
		// Do away with previous pResultSet
		freeResult();
    
		// Write to log
		LOG_WRITE_TIME("MySQLEMdFConnection::execCommand", std::string("Starting query.\n") + query);
    
		// Execute query
		if (mysql_query(pConn, query.c_str()) != 0) {
			// Write to log
			LOG_WRITE_TIME("MySQLEMdFConnection::execCommand", "Query finished with failure.");

			// Error: mysql_query did not return 0.
			return false;
		} else {
			// No error

			// "Store result" to make sure we've finished
			pResultSet = mysql_store_result(pConn);

			// This should be 0; it's a command...
			if (pResultSet != NULL) {
				// It wasn't 0, it is an error

				// Write to log
				LOG_WRITE_TIME("MySQLEMdFConnection::execCommand", "Query finished with success, BUT there were results.  This should not be, since this is a command.");

				// Append local error
				appendLocalError("MySQLEMdFConnection::execCommand: Query finished with success, BUT there were results.  This should not be, since this is a command.");

				// Free result
				mysql_free_result(pResultSet);
				pResultSet = NULL;

				// Return error
				return false;
			} else {
				// It was 0, there was no error

				// Write to log
				LOG_WRITE_TIME("MySQLEMdFConnection::execCommand", "Query finished with success.");
	
				// Get number of affected rows
				m_command_rows = mysql_affected_rows(pConn);
	
				// Return success
				return true;
			}
		} 
	}
#else
	return false;
#endif
}


// Get last error message
// Returns true on error message available 
// (even if it is "OK"), false on error message not available.
bool MySQLEMdFConnection::errorMessage(std::string& out)
{
#if USE_MYSQL
	if (pConn == 0) {
		out = "not connected.";
		return true;
	} else {
		out = mysql_error(pConn);

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
bool MySQLEMdFConnection::beginTransaction(void)
{
#if USE_MYSQL
	if (pConn == 0)
		return false;
	else {
		if (m_bTransactionInProgress) {
			return false;
		} else {
			m_bTransactionInProgress = execCommand("BEGIN");
			return m_bTransactionInProgress;
		}
	}
#else
	return false;
#endif
}

bool MySQLEMdFConnection::commitTransaction(void)
{
#if USE_MYSQL
	if (pConn == 0)
		return false;
	else {
		if (m_bTransactionInProgress) {
			m_bTransactionInProgress = false;
			return execCommand("COMMIT");
		} else {
			//      std::cerr << "Warning: MySQLEMdFConnection::commitTransaction: No transaction in progress." << std::endl << "... Something is wrong in the logic! ..." << std::endl;
			return false;
		}
	}
#else
	return false;
#endif
}

bool MySQLEMdFConnection::abortTransaction(void)
{
#if USE_MYSQL
	if (pConn == 0)
		return false;
	else {
		if (m_bTransactionInProgress) {
			m_bTransactionInProgress = false;
			return execCommand("ROLLBACK");
		} else {
			std::cerr << "Warning: MySQLEMdFConnection::abortTransaction: No transaction in progress." << std::endl << "... Something is wrong in the logic! ..." << std::endl;
			return false;
		}
	}
#else
	return false;
#endif
}


// Field count
// Returns -1 on no fields.
int MySQLEMdFConnection::getNoOfFields(void)
{
#if USE_MYSQL
	if (pConn == 0)
		return -1;
	else {
		if (pResultSet == NULL)
			return -1;
		else {
			return (int) mysql_num_fields(pResultSet);
		}
	}
#else
	return -1;
#endif
}


// key is ignored
bool MySQLEMdFConnection::useDatabase(const std::string& db_name, const std::string& key)
{
	UNUSED(key); // Silence a warning
	
#if USE_MYSQL
	if (pConn == 0)
		return false;
	else {
		if (strcmp_nocase(db_name, "emdf") == 0) {
			shutdownConnection();
			m_db = "emdf";
			return connect();
		} else {
			int result = mysql_select_db(pConn, db_name.c_str());
			freeResult();
			return result == 0;
		}
	}
#else
	UNUSED(db_name); // Silence a warning
	return false;
#endif
}

void MySQLEMdFConnection::freeResult(void)
{
#if USE_MYSQL
	if (pResultSet != NULL) {
		// Fetch all remaining rows (if any)
		while (mysql_fetch_row(pResultSet) != NULL) ;

		// Free result
		mysql_free_result(pResultSet);

		// Clean up
		pResultSet = NULL;
		m_Row = NULL;
	}
#endif
}

std::string MySQLEMdFConnection::escapeStringForSQL(const std::string& str)
{
#if USE_MYSQL
	// Malloc a piece of memory that is twice the input length + 1
	// as the MySQL manual demands.
	char *strTo = (char*) malloc(str.length()*2 + 1);

	// Call MySQL's API function.
	// This fills the buffer strTo with the scaped version of 
	// the third argument, and processes up to the number of 
	// bytes in the fourth argument.
	// This can even encode '\0'.
	// The return value is the number of bytes in the strTo buffer,
	// excluding the extra zero byte which the buffer is padded with.
	unsigned long newlength;
	newlength = mysql_real_escape_string(pConn, 
					     strTo, str.c_str(), str.length());

	// This std::string constructor is described on page 585 in
	// Stroustrup, TCPPPL, 3rd ed.
	std::string result = "'" + std::string(strTo, newlength) + "'";

	// Free the memory, since it has been copied to the result
	free(strTo);

	return result;
#else
	return "";
#endif
}

bool MySQLEMdFConnection::connect()
{
#if USE_MYSQL
	// Make sure there isn't anything in progress
	shutdownConnection();

	// Make MYSQL structure
	pConn = mysql_init(NULL);

	// Read default configuration information
	mysql_options(pConn, MYSQL_READ_DEFAULT_GROUP, "client");

	// Decide DB name
	const char *pDBName = NULL;
	if (strcmp_nocase(m_db, "emdf") == 0) {
		// If the db is "emdf", don't set the default database when connecting
		pDBName = NULL;
	} else {
		pDBName = m_db.c_str();
	}
  
	// Get unencrypted form of password
	std::string unencrypted_password = m_passwd;
	unencryptString(unencrypted_password);

	// Connect
	MYSQL *res;
	if ((res = mysql_real_connect(pConn, m_host.c_str(), m_user.c_str(), unencrypted_password.c_str(), pDBName, 0, NULL, 0)) != pConn) {
		std::cerr << "MySQLEMdFDBConnection::connect():\nError: could not connect.  Return value from mysql_real_connect() was " << res << "." << std::endl;
		std::cerr << "mysql_error says: '" << mysql_error(pConn) << "'" << std::endl;
	}

	// Guard against an attack of looking at the memory or core-dumping
	zeroFillString(unencrypted_password);


#ifdef EMDF_DEBUG
	if (!connectionOk()) {
		std::cerr << "mysqlconn.cpp: Connection not OK connecting to database '"
			  << db << "'." << std::endl;
		std::string error_message;
		bool res_err;
		res_err = errorMessage(error_message);
		std::cout << "Error message = '" << error_message << "'\n";
		std::cout << "res_err = " << res_err << "\n";
	}
#endif // EMDF_DEBUG

	// Make sure transactions actually do something...
	execCommand("SET autocommit = 0");

	// Return connection status
	return connectionOk();
#else
	// !(USE_MYSQL)
	return false;
#endif
}
