/*
 * sqlite3conn.h
 *
 * SQLite3 Database connection 
 *
 * Ulrik Petersen
 * Created: 1/27-2001
 * Last update: 5/27-2024
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


/**@file sqlite3conn.h
 *@brief Header file for SQLite3 connection (EMdF layer)
 */



#ifndef EMDF_SQLITE3CONN__H__
#define EMDF_SQLITE3CONN__H__

// For USE_SQLITE3 and USE_SYSTEM_SQLITE3
#include "emdros-lconfig.h"


#include <cstdlib>
#include "conn.h"

#if USE_SQLITE3
extern "C" {
#include "sqlite3_emdros.h"
}
#endif

#include "debug.h"

class SQLite3EMdFConnection : public EMdFConnection {
 private:
	std::string m_db_name;
	bool m_bTransactionInProgress;
#if USE_SQLITE3
	sqlite3 *m_pDB;
	sqlite3_stmt *m_pVM;
#endif
	int m_nNoOfFields;
	bool m_bHasRow;
 public:
	// Constructor and destructor
	SQLite3EMdFConnection(const std::string& db, const std::string& key);
	virtual ~SQLite3EMdFConnection();

	// Status of connection
	// Returns true on connection good, false on connection bad
	virtual bool connectionOk(void);
  
	// Executing SELECT
	virtual bool execSelect(const std::string& query);

	// Executing all others
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

	/*
	long getLastInsertRowID(void) {
#if USE_SQLITE3
		if (m_pDB == 0) {
			return 0;
		} else {
			return sqlite3_last_insert_rowid(m_pDB);
		}
#else
		return 0;
#endif
	}
	*/

	// Getting next tuple
	// Returns true on success, false on failure
	// On success, bMoreTuples tells us whether there are any more tuples.
	virtual bool getNextTuple(bool& bMoreTuples) {
#if USE_SQLITE3
		if (m_pDB == 0 || m_pVM == 0) {
			bMoreTuples = m_bHasRow = false;
			return false;
		} else {
			// Try until we don't get a BUSY result
			int result;
			while ((result = sqlite3_step(m_pVM))
			       == SQLITE_BUSY) {
				// Busy-waiting
				for (int i = 0; i < 10000; i++) {
					;;
				}
			}
      
			if (result == SQLITE_ROW) {
				bMoreTuples = m_bHasRow = true;
				return true;
			} else if (result == SQLITE_DONE) {
				bMoreTuples = m_bHasRow = false;
				return true;
			} else if (result == SQLITE_ERROR) {
				bMoreTuples = m_bHasRow = false;
				return false;
			} else if (result == SQLITE_MISUSE) {
				appendLocalError("GetNetxTuple: result == SQLITE_MISUSE.");
				bMoreTuples =  m_bHasRow = false;
				return false;
			} else {
				// We should not get any other return values than the above
				ASSERT_THROW(false, "We should not get any other return values than the above");
				bMoreTuples =  m_bHasRow = false;
				// Just to fool the compiler into not giving a warning...
				return false;
			}
		}
#else
		bMoreTuples = m_bHasRow = false;
		
		return false;
#endif
	};

	// Is there a row in the local cache?
	// Must be called right after the query is done.
	virtual bool hasRow(void) const {
#if USE_SQLITE3
		return m_bHasRow;
#else
		return false;
#endif
	};

	// Tuple-access
	// First tuple is 0
	// First field is 0.
	virtual bool accessTuple(int field_no, 
				 bool& result) {
#if USE_SQLITE3
		if (!m_bHasRow) {
			return false; 
		} else {
			// Declare field
			const unsigned char *pField = sqlite3_column_text(m_pVM, field_no);
			if (*pField == '\0') {
				throw EMdFNULLValueException();
			} else {
				result = char2bool(*pField);
	  
				// Return success      
				return true;
			}
		}
#else
		// Avoid compiler warning when not building with SQLite3 support 
		UNUSED(field_no);

		// Result is false by definition: We don't use SQLITE3.
		result = false;

		// Return DB error
		return false;
#endif
	};

	virtual bool accessTuple(int field_no, 
				 emdf_ivalue& result) {
#if USE_SQLITE3
		if (!m_bHasRow) {
			// Return failure
			return false;
		} else {
			result = sqlite3_column_int(m_pVM, field_no);
			if (result == 0) {
				if (sqlite3_column_type(m_pVM, field_no) == SQLITE_NULL) {
					throw EMdFNULLValueException();
				} else {
					return true;
				}
			} else {
				// Return success      
				return true;
			}
		}
#else
		// Avoid compiler warnings when not building with SQLite3 support 
		UNUSED(field_no);
		UNUSED(result);

		// Return DB error
		return false;
#endif
	};

	virtual bool accessTuple(int field_no, 
				 std::string& result) {
#if USE_SQLITE3
		if (!m_bHasRow) {
			return false;
		} else {
			int no_of_bytes = sqlite3_column_bytes(m_pVM, field_no);
			if (no_of_bytes == 0) {
				if (sqlite3_column_type(m_pVM, field_no) == SQLITE_NULL) {
					throw EMdFNULLValueException();
				} else {
					result = std::string("");
	  
					// Return success
					return true;
				}
			} else {
				if (sqlite3_column_type(m_pVM, field_no) == SQLITE_BLOB) {
					const char *pField = (const char*) sqlite3_column_blob(m_pVM, field_no);
					result.clear();
					for (int count = 0;
					     count < no_of_bytes;
					     ++count) {
						result += pField[count];
					}
					
				} else {
					result = std::string((const char*)sqlite3_column_text(m_pVM, field_no));
				}

				// Return success
				return true;
			}
		}
#else
		// Avoid compiler warnings when not building with SQLite3 support 
		UNUSED(field_no);
		result = "";

		// Return DB error
		return false;
#endif
	};

	// Be done with a result set
	virtual void finalize(void);

	// UseDatabase
	virtual bool useDatabaseSQLite3(const std::string& database_name, const std::string& key, bool bIsCreating);

	void get_last_insert_rowid(emdf_ivalue& result /* out */);

	// Vacuum
	bool vacuum(bool bAnalyze);

 protected:

 private:
	// Make the assignment operator and copy constructor private
	// because they are not supported.
	SQLite3EMdFConnection(const SQLite3EMdFConnection&);
	SQLite3EMdFConnection& operator=(const SQLite3EMdFConnection&);

	// Clean up
	void deleteDatabase(void);
};


#endif // EMDF_SQLITE3CONN__H__
