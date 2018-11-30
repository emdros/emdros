/*
 * sqlite3conn.cpp
 *
 * SQLite3 Database connection 
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


/**@file sqlite3conn.cpp
 *@brief Implementation of SQLite3 connection (EMdF layer)
 */



#include <utils.h>
#include <debug.h>
#include <cstdlib>
#include <cassert>
#include <sqlite3conn.h>
#include <string_func.h>
#include <logging.h>
#include <iostream>
#include <cstdlib>
#include <sstream>
#ifndef WIN32
#include <unistd.h>
#endif



#if USE_SQLITE3
/*
 * Implemented in libsqlite3
 */
#if !USE_SYSTEM_SQLITE3
extern "C" int sqlite3_open_encrypted(
				      const char *zFilename,   /* Name of the encrypted database */
				      const void *pKey,        /* Pointer to the key */
				      int nKey,                /* Number of bytes in the key */
				      sqlite3 **ppDB
				      );
extern "C" int sqlite3_key(sqlite3 *pDB,
			   const void *pKey,        /* Pointer to the key */
			   int nKey                 /* Number of bytes in the key */
			   );
#endif /* !USE_SYSTEM_SQLITE3 */
#endif /* USE_SQLITE3 */


#if USE_SQLITE3
extern "C" {
#ifdef WIN32
extern int sqlite3OsAccess(sqlite3_vfs *pVfs, const char *zFile, int flags, int *res);
int mySQLite3OsFileExists(const char *zFile) {
  int res = 0;
  int rc = SQLITE_OK;
  sqlite3_vfs *pVfs = sqlite3_vfs_find(NULL); // Find first VFS on linked list inside of SQLite 3
  if (pVfs != NULL) {
	  rc = pVfs->xAccess(pVfs, zFile, SQLITE_ACCESS_EXISTS, &res);
  } else {
	  rc = SQLITE_ERROR;
  }
  return (res && rc==SQLITE_OK);
}
#else
	int mySQLite3OsFileExists(const char *zFilename) {
		return access(zFilename, 0)==0;
	}
#endif // defined(WIN32)
} // extern "C"
#endif /* USE_SQLITE3 */




SQLite3EMdFConnection::SQLite3EMdFConnection(const std::string& database_name, const std::string& key)
	: m_bTransactionInProgress(false),
#if USE_SQLITE3
	  m_pDB(0), 
	  m_pVM(0),
#endif
	  m_nNoOfFields(0),
	  m_bHasRow(false)
{
#if USE_SQLITE3
	// Use database emdf if database name not given
	std::string db;
	if (database_name.length() == 0) {
		db = "emdf";
	} else {
		db = database_name;
	}

	useDatabaseSQLite3(db, key, false);
#endif
}

SQLite3EMdFConnection::~SQLite3EMdFConnection()
{
#if USE_SQLITE3
	deleteDatabase();
#endif
}


// Status of connection
// Returns true on connection good, false on connection bad
// Should be called right after constructor to see if we
// succeeded in setting up the connection.
bool SQLite3EMdFConnection::connectionOk(void)
{
#if USE_SQLITE3
	if (m_pDB == 0)
		// This means that the DB is "emdf"
		return true;
	else {
		// This is the best we can do: The SQLite API does not offer
		// a way of checking this.
		return true;
	}
#else
	return false;
#endif /* USE_SQLITE3 */
}


// Executing
bool SQLite3EMdFConnection::execSelect(const std::string& query)
{
#if USE_SQLITE3
	if (m_pDB == 0)
		return false;
	else {
		// Write to log
#if EMDROS_LOGGING
		std::string log = "Starting query.\n" + query;
		LOG_WRITE_TIME("SQLite3EMdFConnection::execSelect", log);
#endif

		// Clear the Result Stucture if needed
		finalize();

		// Compile
		// char *szError = NULL; // TODO: Should it be there?
		const char *szTail = NULL;
		int nResult;
		if ((nResult = sqlite3_prepare(m_pDB, query.c_str(), -1, &m_pVM, &szTail))
		    != SQLITE_OK) {
			// We did not succeed
			LOG_WRITE_TIME("SQLite3EMdFConnection::execSelect", "query compilation failed.");

			// Was there an error message?
			if (nResult != SQLITE_OK) {
				// Append error
				appendLocalError(std::string(sqlite3_errmsg(m_pDB)));
			}

			// Return failure
			return false;
		}

		// Get first row
		bool bMoreTuples;
		if (!getNextTuple(bMoreTuples))
			return false;

		// Write to log
		LOG_WRITE_TIME("SQLite3EMdFConnection::execSelect", "Query finished.");

		// Return success
		return true;
	}
#else
	return false;
#endif /* USE_SQLITE3 */
}


// Executing
bool SQLite3EMdFConnection::execCommand(const std::string& query)
{
#if USE_SQLITE3
	if (m_pDB == 0)
		return false;
	else {
		// Write to log
#if EMDROS_LOGGING
		std::string log = "Starting query.\n" + query;
		LOG_WRITE_TIME("SQLite3EMdFConnection::execCommand", log);
#endif

		// Clear the result stucture if needed
		finalize();

		// Compile
		int result;
		char *szError;
		while ((result = sqlite3_exec(m_pDB, query.c_str(), NULL, NULL, &szError))
		       == SQLITE_BUSY) {
			// Busy-waiting
			for (int i = 0; i < 10000; i++) {
				;;
			}
		}

		if (result != SQLITE_OK) {
			// We did not succeed
			LOG_WRITE_TIME("SQLite3EMdFConnection::execCommand", "query failed.");

			// Was there an error message?
			if (szError != NULL) {
				// Append error
				appendLocalError(std::string(szError));

				// Free error
				sqlite3_free(szError);
			}

			// Return failure
			return false;
		}

		// Write to log
		LOG_WRITE_TIME("SQLite3EMdFConnection::execCommand", "Query finished.");

		// Return success
		return true;
	}
#else
	return false;
#endif /* USE_SQLITE3 */
}


// Get last error message
// Returns true on error message available 
// (even if it is "OK"), false on error 
// message not available.
bool SQLite3EMdFConnection::errorMessage(std::string& out)
{
	UNUSED(out); // Silence a warning
#if USE_SQLITE3
	if (m_pDB == 0)
		return false;
	else {
		return false;
	}
#else
	return false;
#endif /* USE_SQLITE3 */
}


// Transactions
bool SQLite3EMdFConnection::beginTransaction(void)
{
#if USE_SQLITE3
	if (m_pDB == 0)
		return false;
	else {
		if (m_bTransactionInProgress) {
			return false;
		} else {
			m_bTransactionInProgress = true;
			return execCommand("BEGIN");
		}
	}
#else
	return false;
#endif /* USE_SQLITE3 */
}

bool SQLite3EMdFConnection::commitTransaction(void)
{
#if USE_SQLITE3
	if (m_pDB == 0)
		return false;
	else {
		if (m_bTransactionInProgress) {
			m_bTransactionInProgress = false;
			return execCommand("COMMIT");
		} else {
			appendLocalError("commitTransaction: No transaction in progress.");
			return false;
		}
	}
#else
	return false;
#endif /* USE_SQLITE3 */
}

bool SQLite3EMdFConnection::abortTransaction(void)
{
#if USE_SQLITE3
	if (m_pDB == 0)
		return false;
	else {
		if (m_bTransactionInProgress) {
			m_bTransactionInProgress = false;
			return execCommand("ROLLBACK");
		} else {
			appendLocalError("abortTransaction: No transaction in progress.");
			return false;
		}
	}
#else
	return false;
#endif /* USE_SQLITE3 */
}


// Field count
// Returns -1 on no fields.
int SQLite3EMdFConnection::getNoOfFields(void)
{
#if USE_SQLITE3
	if (m_pDB == 0)
		return -1;
	else {
		assert(false);
		return m_nNoOfFields;
	}
#else
	return -1;
#endif
}


bool SQLite3EMdFConnection::vacuum(bool bAnalyze)
{
#if USE_SQLITE3
	if (m_pDB == 0)
		return false;
	else {
		// Make sure we aren't doing a query at the moment
		finalize();

		// NOTE: In 2.8.6, VACUUM does not work with sqlite_compile; 
		// only with sqlite_exec.

		// Try until we don't get a BUSY result
		int result;
		char *szError;
		while ((result = sqlite3_exec(m_pDB, "VACUUM", NULL, NULL, &szError))
		       == SQLITE_BUSY) {
			// Busy-waiting
			for (int i = 0; i < 10000; i++) {
				;;
			}
		}

		if (result != SQLITE_OK) {
			// Note: We can't use DEBUG_COMMAND_QUERY_FAILED
			// because this macro calls pConn->finalize(),
			// which doesn't exist in this connection object.
			DEBUG_X_FAILED("SQLite3EMdFConnection::vacuum", "VACUUM");
			appendLocalError(szError);
			sqlite3_free(szError);
			return false;
		}

		if (bAnalyze) {

			while ((result = sqlite3_exec(m_pDB, "ANALYZE", NULL, NULL, &szError))
			       == SQLITE_BUSY) {
				// Busy-waiting
				for (int i = 0; i < 10000; i++) {
					;;
				}
			}

			if (result != SQLITE_OK) {
				// Note: We can't use DEBUG_COMMAND_QUERY_FAILED
				// because this macro calls pConn->finalize(),
				// which doesn't exist in this connection object.
				DEBUG_X_FAILED("SQLite3EMdFConnection::vacuum", "VACUUM");
				appendLocalError(szError);
				sqlite3_free(szError);
				return false;
			}
		}
		// If we got this far, there were no DB errors
		return true;
	}
#else
	return false;
#endif /* USE_SQLITE3 */
}

// Release results so they don't take up memory
void SQLite3EMdFConnection::finalize(void)
{
#if USE_SQLITE3
	if (m_pVM != 0) {
		sqlite3_finalize(m_pVM);  // FIXME: This should check for any errors...
		m_pVM = 0;
	}
	m_nNoOfFields = 0;
#endif /* USE_SQLITE3 */
}

void SQLite3EMdFConnection::deleteDatabase()
{
#if USE_SQLITE3
	// Commit transaction if any in progress
	if (m_bTransactionInProgress) {
		commitTransaction();
	}

	// Clean up VM
	finalize();

	// Close DB
	if (m_pDB != 0) {
		sqlite3_close(m_pDB); // FIXME: This should check for any errors...
		m_pDB = 0;
	}
#endif /* USE_SQLITE3 */
}

// UseDatabase
bool SQLite3EMdFConnection::useDatabaseSQLite3(const std::string& database_name, const std::string& key, bool bIsCreating)
{
#if USE_SQLITE3
	// First clean up after ourselves
	deleteDatabase();

	// Treat the "emdf" database specially
	if (strcmp_nocase(database_name, "emdf") == 0) {
		// The "emdf" database is empty.
		m_pDB = 0;
	} else {
		bool bDoIt = false;
		if (bIsCreating) {
			bDoIt = true;
		} else {
			bDoIt = mySQLite3OsFileExists(database_name.c_str()) != 0;
		}

		if (bDoIt) {
			int result = 0;
#if USE_SYSTEM_SQLITE3
			result = sqlite3_open(database_name.c_str(), &m_pDB);
#else
			if (key.length() == 0) { 
				result = sqlite3_open(database_name.c_str(), &m_pDB);
			} else {
				result = sqlite3_open_encrypted(database_name.c_str(), 
								(const void*) key.c_str(), (int) key.length(),
								&m_pDB);
				if (result != SQLITE_OK) {
					appendLocalError(std::string(sqlite3_errmsg(m_pDB)));
					
					// There was an error...
					return false;
				}

				result = sqlite3_key(m_pDB,
						     (const void*) key.c_str(),
						     (int) key.length());
			}
#endif
      
			// If there was an error message, append it and return "error" (false)
			if (m_pDB == NULL) {
				appendLocalError(std::string("SQLite3EMdFConnection::useDatabase: m_pDB was NULL."));
				return false;
			} else if (result != SQLITE_OK) {
				appendLocalError(std::string(sqlite3_errmsg(m_pDB)));

				// There was an error...
				return false;
			}
		} else {
			// file does not exist, and we are not creating.
			return false;
		}
	}

	// If we got this far, there were no DB errors
	return true;
#else
	return false;
#endif /* USE_SQLITE3 */
}

void SQLite3EMdFConnection::get_last_insert_rowid(emdf_ivalue& result /* out */)
{
#if USE_SQLITE3
	result = (emdf_ivalue) sqlite3_last_insert_rowid(m_pDB);
#else
	result = 0;
#endif
}


