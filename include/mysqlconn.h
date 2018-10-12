/*
 * mysqlconn.h
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


/**@file mysqlconn.h
 *@brief Header file for MySQL connection (EMdF layer)
 */



#ifndef EMDF_MYSQLCONN__H__
#define EMDF_MYSQLCONN__H__

#include "emdros-lconfig.h"

#ifndef USE_MYSQL
#define USE_MYSQL 0
#endif


#include "conn.h"
#include <cstdlib>
#include "utils.h"

#if USE_MYSQL
#include <mysql.h>
#endif

class MySQLEMdFConnection : public EMdFConnection {
private:
#if USE_MYSQL
  MYSQL* pConn;
  MYSQL_RES *pResultSet;
  MYSQL_ROW m_Row;
#endif
  std::string m_host;
  std::string m_user;
  std::string m_passwd;
  std::string m_db;
  bool m_bTransactionInProgress;
  unsigned long m_command_rows;
  unsigned int m_num_fields;
  long m_current_tuple_no;
public:
  // Constructor and destructor
  MySQLEMdFConnection(const std::string& host, const std::string& user, const std::string& passwd, const std::string& db);
  virtual ~MySQLEMdFConnection();

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


  // Field count
  // Returns -1 on no fields.
  virtual int getNoOfFields(void);

  virtual bool hasRow(void) const {
#if USE_MYSQL
    return pResultSet != 0 && m_Row != 0;
#else
    return false;
#endif
  };
  
  // Getting next tuple
  // Returns true on success, false on failure
  // On success, bMoreTuples tells us whether there are any more tuples.
  virtual bool getNextTuple(bool& bMoreTuples) {
#if USE_MYSQL
    if (pConn == 0 || pResultSet == NULL)
      return false;
    else {
      m_Row = mysql_fetch_row(pResultSet);
    }
    
    // Check whether there were more tuples
    if (m_Row == NULL) {
      // There are no more tuples
      bMoreTuples = false;
    } else {
      // There are more tuples
      bMoreTuples = true;
    }
    
    // Return success      
    return true;
#else
    return false;
#endif
  };

  // Tuple-access by field number
  // First tuple is 0
  // First field is 0.
  virtual bool accessTuple(int field_no, 
			   bool& result) {
#if USE_MYSQL
    if (m_Row == NULL) {
      // Return failure
      return false;
    } else {
      // Declare field
      const char *pField = m_Row[field_no];

      // Check for NULL value
      if (pField == NULL) {
	// It was NULL.  Throw exception
	throw EMdFNULLValueException();
      } else {
	// It wasn't NULL.  Convert result
	result = char2bool(*pField);
      }
	
      // Return success      
      return true;
    }
#else
    return false;
#endif
  };

  virtual bool accessTuple(int field_no, 
			   long& result) {
#if USE_MYSQL
    if (m_Row == NULL) {
      // Return failure
      return false;
    } else {
      const char *pField = m_Row[field_no];
      // Convert result
      // NOTE: The following is MUCH slower than strtol or atol.
      //std::string strtmp = pField;
      //std::istringstream mystringstream(strtmp);
      //mystringstream >> result;
	
      // Check for NULL value
      if (pField == NULL) {
	// It was NULL.  Throw exception
	throw EMdFNULLValueException();
      } else {
	// It wasn't NULL.  Convert result
	result = atol(pField);
      }
	
      // Return success      
      return true;
    }
#else
    return false;
#endif
  };

  virtual bool accessTuple(int field_no, 
			   std::string& result) {
#if USE_MYSQL
    if (m_Row == 0) {
      return false;
    } else {
      const char *pField = m_Row[field_no];

      // Check for NULL value
      if (pField == NULL) {
	// It was NULL.  Throw exception
	throw EMdFNULLValueException();
      } else {
	// It wasn't NULL.  Convert result
	result = pField;
      }
      
      // Return success
      return true;
    }
#else
    return false;
#endif
  };

  // Be done with a result set
  virtual void finalize(void) {
#if USE_MYSQL
    freeResult();
#endif
  };

  // UseDatabase
  // Key is ignored.
  virtual bool useDatabase(const std::string& db_name, const std::string& key);

  // SQL escaping
  virtual std::string escapeStringForSQL(const std::string& str);
protected:
  void freeResult(void);
private:
  // Make the assignment operator and copy constructor private
  // because they are not supported.
  MySQLEMdFConnection(const MySQLEMdFConnection&);
  MySQLEMdFConnection& operator=(const MySQLEMdFConnection&);
  
  // connect using local m_user/m_password/m_db info
  bool connect();

  // Commit any transaction, free the result, and close the connection
  // Only done if there actually is a connection
  void shutdownConnection();
};


#endif // EMDF_MYSQLCONN__H__
