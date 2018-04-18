/*
 * pgconn.h
 *
 * PostgreSQL Database connection 
 *
 * Ulrik Petersen
 * Created: 1/27-2001
 * Last update: 6/22-2015
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2015  Ulrik Sandborg-Petersen
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License as
 *   published by the Free Software Foundation, license version 2.  
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *   02111-1307 USA
 *
 *
 *   Special exception
 *   =================
 * 
 *   In addition, as a special exception, Ulrik Petersen, the
 *   copyright holder of Emdros, gives permission to link Emdros, in
 *   whole or in part, with the libraries which are normally
 *   distributed with:
 *   
 *   a) Sun's Java platform,
 *   b) Python, 
 *   c) Jython,
 *   d) Ruby, and/or 
 *   e) Perl 
 *   f) PostgreSQL
 *   g) OpenSSL
 *
 *   (or with modified versions of these), and to distribute linked
 *   combinations including both Emdros, in whole or in part, and one
 *   or more of the libraries normally distributed with (a)-(g) above.
 *
 *   Please note: This gives you special rights concerning the
 *   libraries which normally accompany the above pieces of software.
 *   It gives you no special rights concerning software that you write
 *   yourself.  You must obey the GNU General Public License in all
 *   respects for all of the code used other than the libraries
 *   normally distributed with (a)-(g) above.
 *
 *   If you modify this file, you may extend this exception to your
 *   version of the file, but you are not obligated to do so. If you
 *   do not wish to do so, delete this exception statement from your
 *   version.
 *
 *
 *   Other licensing forms
 *   =====================
 *
 *   If you wish to negotiate commercial licensing, please contact
 *   Ulrik Petersen at ulrikp[at]users.sourceforge.net.
 *
 *   Licensing can also be negotiated if your organization is an
 *   educational, non-profit, charity, missionary or similar
 *   organization.
 *
 *
 *   Website
 *   =======
 *
 *   Emdros has a website here:
 *
 *   http://emdros.org
 *
 *
 *
 **************************************************************************/

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
    return false;
#endif
  };

  virtual bool accessTuple(int field_no, 
			   long& result) {
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
      result = atol(szTemp);

      // Return success
      return true;
    }
#else
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
