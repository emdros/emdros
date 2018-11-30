/*
 * pgconn.h
 *
 * PostgreSQL Database connection 
 *
 * Ulrik Petersen
 * Created: 1/27-2001
 * Last update: 11/30-2018
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


/**@file pgconn.h
 *@brief Header file for PostgreSQL connection (EMdF layer)
 */



#ifndef EMDF_PGCONN__H__
#define EMDF_PGCONN__H__

#include "emdros-lconfig.h"

#ifndef USE_POSTGRESQL
#define USE_POSTGRESQL (0)
#endif

#include "conn.h"

#if USE_POSTGRESQL
extern "C" {
#include <libpq-fe.h>
}
#endif


class PgEMdFConnection : public EMdFConnection {
private:
  void *m_pConn;
  std::string m_current_db_name;
  std::string m_host;
  std::string m_user;
  std::string m_password;
  void *m_pResult;
  bool m_bTransactionInProgress;
  long m_current_tuple;
  long m_current_last_tuple;
public:
  // Constructor and destructor
  PgEMdFConnection(const std::string& host, const std::string& user, const std::string& passwd, const std::string& database_name);
  virtual ~PgEMdFConnection();

  // Connect and disconnect
  void connect();
  void disconnect();



  // Status of connection
  // Returns true on connection good, false on connection bad
  virtual bool connectionOk(void);

  // Executing
  virtual bool execSelect(const std::string& query);
  virtual bool execCommand(const std::string& query);

  // Get last error message
  // Returns true on error message available 
  // (even if it is "OK"), false on error 
  // message not available.
  virtual bool errorMessage(std::string& out);

  // Transactions
  virtual bool beginTransaction(void);
  virtual bool commitTransaction(void);
  virtual bool abortTransaction(void);


  // Tuple-counts
  // Both return -1 on not appliccable or error
  virtual long noOfResultTuples(void) {   // Only for SELECT
#if USE_POSTGRESQL
    return PQntuples((PGresult*) m_pResult); 
#else
    return -1;
#endif
  }


  // Field count
  // Returns -1 on no fields.
  virtual int getNoOfFields(void) {
#if USE_POSTGRESQL
    if (m_pConn == 0)
      return -1;
    else {
      return PQnfields((PGresult*) m_pResult); 
    }
#else
    return -1;
#endif
  }


  // Getting next tuple
  // Returns true on success, false on failure
  // On success, bMoreTuples tells us whether there are any more tuples.
  virtual bool getNextTuple(bool& bMoreTuples) {
#if USE_POSTGRESQL
    bool bResult;

    if (m_current_tuple < m_current_last_tuple) {
      // This is less than the last
      bMoreTuples = true;
      bResult = true;
    } else if (m_current_tuple == m_current_last_tuple) {
      // This is the last
      bMoreTuples = false;
      bResult = true;
    } else {
      // We are past the last -- this is an error in the logic
      bMoreTuples = false;
      bResult = false;
    }

    m_current_tuple++;
    return bResult;
#else
    UNUSED(bMoreTuples);
    return false;
#endif
  };

  // Is there a row in the local cache?
  // Must be called right after the query is done.
  virtual bool hasRow(void) const {
#if USE_POSTGRESQL
    return m_pResult != 0 && m_current_last_tuple >= 0;
#else
    return false;
#endif
  };


  // Tuple-access by field number
  // First tuple is 0
  // First field is 0.
  virtual bool accessTuple(int field_no, 
			   bool& result) {
#if USE_POSTGRESQL
    const char *szTemp = PQgetvalue((PGresult*) m_pResult, m_current_tuple, field_no); 
    if (szTemp == NULL) {
      throw EMdFNULLValueException();
    } else {
      // Convert result
      result = char2bool(*szTemp);
	
      // Return success
      return true;
    }
#else
    UNUSED(field_no);
    UNUSED(result);
    return false;
#endif
  };

  virtual bool accessTuple(int field_no, 
			   emdf_ivalue& result) {
#if USE_POSTGRESQL
    const char *szTemp = PQgetvalue((PGresult*) m_pResult, m_current_tuple, field_no); 

    // Convert result
    // NOTE: The following is MUCH slower than strtol or atol.
    // std::string strtmp = szTemp;
    // std::istringstream mystringstream(strtmp);
    // mystringstream >> result;
      
    if (szTemp == NULL
	|| szTemp[0] == '\0') {
      throw EMdFNULLValueException();
    } else {
      // Convert result
      // result = strtol(szTemp, (char **)NULL, 10);
      result = atoi(szTemp);

      // Return success
      return true;
    }
#else
    UNUSED(field_no);
    UNUSED(result);
    return false;
#endif
  };

  virtual bool accessTuple(int field_no, 
			   std::string& result) {
#if USE_POSTGRESQL
    const char *szTemp = PQgetvalue((PGresult*) m_pResult, m_current_tuple, field_no); 

    // Test for result availability      
    if (szTemp == NULL) {
      throw EMdFNULLValueException();
    } else {
      // Convert result
      result = szTemp;

      // Return success
      return true;
    }
#else
    UNUSED(field_no);
    UNUSED(result);
    return false;
#endif
  };

  // Release results so they don't take up memory
  virtual void finalize(void) {
#if USE_POSTGRESQL
    // Remove result
    if (m_pResult != 0) 
      PQclear((PGresult*) m_pResult);
    m_pResult=NULL;
#endif
  };



  // Vacuum
  virtual bool vacuum(bool bAnalyze);

  bool execCOPY(std::string& copy_data);
  bool mayCOPY(void);

private:
  // Make the assignment operator and copy constructor private
  // because they are not supported.
  PgEMdFConnection(const PgEMdFConnection&);
  PgEMdFConnection& operator=(const PgEMdFConnection&);
};


#endif // EMDF_PGCONN__H__
