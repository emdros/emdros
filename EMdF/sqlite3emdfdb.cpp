/*
 * sqlite3emdfdb.cpp
 *
 * SQLite EMdF database base class
 *
 * Ulrik Petersen
 * Created: 1/27-2001
 * Last update: 11/15-2018
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



#include <string>
#include <cstring>
#include <utils.h>
#include <string_func.h>
#include <string_list.h>
#include <sqlite3emdfdb.h>
#include <inst_object.h>
#include <debug.h>
#include <cstdio>
#include <functional>
#include <algorithm>

#if defined(USE_SQLITE3) && USE_SQLITE3
#include <sqlite3conn.h>
#endif

/**@file sqlite3emdfdb.cpp
 *@brief Implementation of SQLite EMdF database (EMdF layer)
 */


#if defined(USE_SQLITE3) && USE_SQLITE3
extern "C" {
	extern int mySQLite3OsFileExists(const char*); // Defined in sqlite3conn.cpp
}
#endif


/** Constructor.
 *
 * @internal
 *
 * @param database_name The name of the database to open. "emdf" is
 * treated specially.
 *
 * @param key The encryption key.  Leave blank for no encryption.
 * Note that encryption is not available in the Open Source version of
 * Emdros.
 *
 */
SQLite3EMdFDB::SQLite3EMdFDB(std::string database_name, std::string key) 
	: EMdFDB(kSQLite3)
{
	// This is not documented in the SQLite documentation, but we
	// set a limit high enough that we won't use it most of the
	// time when used in EMdFDB::normalizeTableName().
	m_max_table_name_length = 1024;
#if defined(USE_SQLITE3) && USE_SQLITE3
	pConn = new SQLite3EMdFConnection(database_name, key);
	useDatabase(database_name, key, false);
#else
	; // FIXME: What to do if not implemented?
#endif
}

/** Destructor.
 *
 */
SQLite3EMdFDB::~SQLite3EMdFDB()
{
	// pConn is deleted in the base class, EMdFDB.
}

#if defined(USE_SQLITE3) && USE_SQLITE3
/** Use database.
 *
 * Connect to a database.  The user can call this method, or use the
 * MQL statement USE DATABASE.
 *
 * Note that the "key" parameter is only used if you have an SQLite
 * that supports encryption.
 *
 * @param db_name the database name to be used.
 *
 * @param key the encryption key to use.
 *
 * @param bIsCreating Must be true if we are not only using but also
 * creating the database.
 *
 * @return True on no database error, false if a database error occurred.
 */
bool SQLite3EMdFDB::useDatabase(const std::string& db_name, const std::string& key, bool bIsCreating)
{
	if (pConn == 0)
		return false;
	else {
		// Make new connection, first deleting the old one
		if (!((SQLite3EMdFConnection*)pConn)->useDatabaseSQLite3(db_name, key, bIsCreating)) {
			DEBUG_X_IS_WRONG("SQLite3EMdFDB::useDatabase", "Either an error or occurred, or database does not exist.  If the latter, use CREATE DATABASE first.")
				return false;
		}

		// Set pragma synchronous if database is not emdf.
		// If it is emdf, then there is no database to perform stuff on.
		if (strcmp_nocase(db_name, "emdf") != 0) {
			char szQuery[100];
#ifdef SQLITE3_EMDFDB_NO_FSYNC
			strcpy(szQuery,"PRAGMA synchronous = OFF");
#else
			strcpy(szQuery,"PRAGMA synchronous = NORMAL");
#endif 
			if (!pConn->execCommand(szQuery)) {
				DEBUG_COMMAND_QUERY_FAILED("SQLite3EMdFDB::useDatabase", szQuery);
				return false;
			}
		}

		// Call base class
		if (!EMdFDB::useDatabase(db_name, key, bIsCreating)) {
			return false;
		}

		return pConn->connectionOk();
	}
}

/** Vacuum database.
 *
 * Vacuum database and optionally analyze it.
 *
 * Deletes redundant sequence info, then passes the buck to
 * SQLite3EMdFConnection::vacuum().
 *
 * @see SQLite3EMdFConnection::vacuum().
 *
 * @param bAnalyze Must be true if the user wants the ANALYZE option
 * to occur.  Exactly what the ANALYZE option does is dependent on the
 * implementation in the subclass.  On SQLite, it does nothing.
 *
 * @return True on no database error, false if a database error occurred.
 */
bool SQLite3EMdFDB::vacuum(bool bAnalyze)
{
	if (pConn == 0)
		return false;
	else {
		// Ask the connection to vacuum
		return ((SQLite3EMdFConnection*)pConn)->vacuum(bAnalyze);
	}
}

//
// Sequence manipulation

/** Get next id_d from a given sequence.
 *@internal
 *
 * There are three sequences: SEQUENCE_OBJECT_ID_DS,
 * SEQUENCE_TYPE_IDS, and SEQUENCE_OTHER_IDS (defined in emdf.h).
 * This method gets the next id_d from the given sequence.
 *
 * @return True on no database error, false if a database error occurred.
 */
bool SQLite3EMdFDB::getNextID(int sequence, id_d_t& out)
{
	if (pConn == 0)
		return false;
	else {
		// Declare szQuery
		char szQuery[512];

		// Begin transaction
		bool bDoCommit = pConn->beginTransaction();

		try {

			// Update value
			sprintf(szQuery, 
				"UPDATE sequence_%d SET sequence_value = sequence_value + 1;",
				sequence);
			if (!pConn->execCommand(szQuery)) {
				DEBUG_COMMAND_QUERY_FAILED("SQLite3EMdFDB::getNextID", szQuery);
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}

			// Clean up result
			pConn->finalize();

			// Ask what the current value is
			sprintf(szQuery, "SELECT sequence_value FROM sequence_%d;", sequence);
			if (!pConn->execSelect(szQuery)) {
				DEBUG_SELECT_QUERY_FAILED("SQLite3EMdFDB::getNextID", szQuery);
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}

			// Access result
			if (!pConn->accessTuple(0, out)) {
				DEBUG_ACCESS_TUPLE_FAILED("SQLite3EMdFDB::getNextID");
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}

			// Clean up result
			pConn->finalize();

			// Commit transaction
			if (bDoCommit) {
				if (!pConn->commitTransaction()) {
					DEBUG_COMMITTRANSACTION_FAILED("SQLite3EMdFDB::getNextID");
					return false;
				}
			}
		} catch (EMdFNULLValueException& e) {
			DEBUG_NULL_VALUE_EXCEPTION("SQLite3EMdFDB::getNextID");

			// Abort transaction if in progress
			if (bDoCommit) {
				pConn->abortTransaction();
			}

			// There was a NULL value exception, hence we return an error
			return false;
		}

		// If we got this far, the command succeeded.
		return true;
	}
}

/** set next object id_d sequence number
 *@internal
 *
 * Sets the next ID_D sequence number to \p next_id_d if current ID_D
 * sequence number is not already greater.
 *
 * @param next_id_d the id_d to set the sequence to.
 * @return True on no database error, false if a database error occurred.
 */
bool SQLite3EMdFDB::setNextObjectID_DIfNotHigher(id_d_t next_id_d)
{
	if (pConn == 0)
		return false;
	else {
		// Declare szQuery
		char szQuery[512];

		// Begin transaction
		bool bDoCommit = pConn->beginTransaction();

		try {
			// Ask what the current value is
			id_d_t current_id_d;
			sprintf(szQuery, "SELECT sequence_value FROM sequence_%d;", SEQUENCE_OBJECT_ID_DS);
			if (!pConn->execSelect(szQuery)) {
				DEBUG_SELECT_QUERY_FAILED("SQLite3EMdFDB::setNextObjectID_DIfNotHigher", szQuery);
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}

			if (!pConn->hasRow()) {
				DEBUG_X_IS_WRONG("SQLite3EMdFDB::setNextObjectID_DIfNotHigher", "there were no results.");
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}

			// Access result
			if (!pConn->accessTuple(0, current_id_d)) {
				DEBUG_ACCESS_TUPLE_FAILED("SQLite3EMdFDB::setNextObjectID_DIfNotHigher");
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}

			// Clean up result
			pConn->finalize();

			// Only update if current is less than next minus 1
			if (current_id_d < next_id_d-1) {
				// Update value
				sprintf(szQuery, 
					"UPDATE sequence_%d SET sequence_value = %ld;", 
					SEQUENCE_OBJECT_ID_DS, next_id_d-1);
				if (!pConn->execCommand(szQuery)) {
					DEBUG_COMMAND_QUERY_FAILED("SQLite3EMdFDB::getNextID", szQuery);
					if (bDoCommit)
						pConn->abortTransaction();
					return false;
				}
			}

			// Commit transaction
			if (bDoCommit) {
				if (!pConn->commitTransaction()) {
					DEBUG_COMMITTRANSACTION_FAILED("SQLite3EMdFDB::getNextID");
					return false;
				}
			}
		} catch (EMdFNULLValueException& e) {
			DEBUG_NULL_VALUE_EXCEPTION("SQLite3EMdFDB::getNextID");

			// Abort transaction if in progress
			if (bDoCommit) {
				pConn->abortTransaction();
			}

			// There was a NULL value exception, hence we return an error
			return false;
		}

		// If we got this far, the command succeeded.
		return true;
	}
}


/** Drop database.
 *@internal
 *
 * Drops (deletes) a database.
 *
 * Does not call EMdFDB::dropDatabase(), but does its own stuff.
 *
 * @param db_name the database name to be used.
 * @return True on no database error, false if a database error occurred.
 */
bool SQLite3EMdFDB::dropDatabase(const std::string& db_name)
{
	if (pConn == 0)
		return false;
	else {
		// First check that the file exists...
		// FIXME: This is a potential security risk, since now any file
		// can be deleted which the using process has permissions
		// to write to...
		if (mySQLite3OsFileExists(db_name.c_str()) == 0) {
			DEBUG_X_IS_WRONG("SQLite3EMdFDB::dropDatabase", "database name did not exist.")
				appendLocalError("Could not switch database context to '" + db_name + "'\n");
			return false;
		}

		// Switch database context to "emdf"
		// So that we can drop.
		if (!useDatabase("emdf", "", false)) {
			DEBUG_X_FAILED("SQLite3EMdFDB::dropDatabase", "useDatabase 'emdf'");
			return false;
		}

		if (remove(db_name.c_str()) != 0) {
			appendLocalError(std::string("Could not remove file '") + db_name + "'.");
			return false;
		}

		// Remove journal, if there.  We don't care about the 
		// return value if it isn't there.
		std::string journal_filename = db_name + "-journal";
		remove(journal_filename.c_str());

		/* Everything was OK. */
		return true;
	}
}

/** Create sequence tables.
 *@internal
 *
 * Create the three sequence tables.
 *
 * @see SQLite3EMdFDB::getNextID() for an introduction to the two
 * sequences.
 *
 * @return True on no database error, false if a database error occurred.
 */
bool SQLite3EMdFDB::createSequenceTables()
{
	if (pConn == 0)
		return false;
	else {
		// Declare szQuery
		char szQuery[512];

		// Begin transaction
		bool bDoCommit = pConn->beginTransaction();

		// Create table/sequence 'sequence_0'
		sprintf(szQuery, "CREATE TABLE sequence_%d (\n"
			"   sequence_value INTEGER PRIMARY KEY\n"
			")", SEQUENCE_OBJECT_ID_DS);
		if (!pConn->execCommand(szQuery)) {
			DEBUG_COMMAND_QUERY_FAILED("SQLite3EMdFDB::createSequenceTables118", szQuery);
			return false;
		}

		// Create table/sequence 'sequence_1'
		sprintf(szQuery, "CREATE TABLE sequence_%d (\n"
			"   sequence_value INTEGER PRIMARY KEY\n"
			")", SEQUENCE_TYPE_IDS);
		if (!pConn->execCommand(szQuery)) {
			DEBUG_COMMAND_QUERY_FAILED("SQLite3EMdFDB::createSequenceTables118", szQuery);
			return false;
		}

		// Create table/sequence 'sequence_2'
		sprintf(szQuery, "CREATE TABLE sequence_%d (\n"
			"   sequence_value INTEGER PRIMARY KEY\n"
			")", SEQUENCE_OTHER_IDS);
		if (!pConn->execCommand(szQuery)) {
			DEBUG_COMMAND_QUERY_FAILED("SQLite3EMdFDB::createSequenceTables118", szQuery);
			return false;
		}


		// Seed sequence table with start value for SEQUENCE_OBJECT_ID_DS
		sprintf(szQuery,
			"INSERT INTO sequence_%d (sequence_value)\n"
			"VALUES ( %d )", 
			SEQUENCE_OBJECT_ID_DS, 
			SEQUENCE_START-1);
		if (!pConn->execCommand(szQuery)) {
			DEBUG_COMMAND_QUERY_FAILED("SQLite3EMdFDB::createSequenceTables118", szQuery);
			return false;
		}

		// Seed sequence table with start value for SEQUENCE_TYPE_ID_DS
		sprintf(szQuery,
			"INSERT INTO sequence_%d (sequence_value)\n"
			"VALUES ( %d )", 
			SEQUENCE_TYPE_IDS, 
			SEQUENCE_START-1);
		if (!pConn->execCommand(szQuery)) {
			DEBUG_COMMAND_QUERY_FAILED("SQLite3EMdFDB::CreateSequenceTables", szQuery);
			return false;
		}

		// Seed sequence table with start value for SEQUENCE_OTHER_ID_DS
		sprintf(szQuery,
			"INSERT INTO sequence_%d (sequence_value)\n"
			"VALUES ( %d )", 
			SEQUENCE_OTHER_IDS, 
			SEQUENCE_START-1);
		if (!pConn->execCommand(szQuery)) {
			DEBUG_COMMAND_QUERY_FAILED("SQLite3EMdFDB::CreateSequenceTables", szQuery);
			return false;
		}

		// Commit transaction if necessary
		if (bDoCommit)
			pConn->commitTransaction();

		return true;
	}
}

/** Create database.
 *@internal
 *
 * All tables with meta-data are created, along with the database
 * itself.
 *
 * Calls EMdFDB::createDatabase() as well as doing its own thing.
 *
 * Note that the "key" parameter is only used if you have an SQLite
 * that supports encryption.
 *
 * @param db_name the database name to be created
 * @param charset the default database-wide charset to use.
 * @param key the encryption key to be used.
 * @return True on no database error, false if a database error occurred.
 */
bool SQLite3EMdFDB::createDatabase(const std::string& db_name, eCharsets charset, const std::string& key)
{
	if (!canDeclareEncoding(charset)) {
		DEBUG_X_IS_WRONG("SQLite3EMdFDB::createDatabase", "Cannot use this charset with this backend.");
		return false;
	}

	// Attach database
	if (!useDatabase(db_name, key, true)) {
		return false;
	}

	bool bIsInitialized;
	if (!dbIsInitialized(bIsInitialized)) {
		// There was a DB error checking initialization
		return false;
	} else {
		// There was no DB error checking initialization

		// So was it initialized?
		if (bIsInitialized) {
			// Yes. Return error
			DEBUG_X_IS_WRONG("SQLite3EMdFDB::createDatabase", "Database was initialized already");
			return false;
		} else {
			// No, it wasn't initialized

			// Call base class
			return EMdFDB::initializeDatabase(db_name, key);
		}
	}
}


// See p. 515 in Stroustrup, "TC++PL", 3rd edition.
class SQLite3_Feature_info_nocase_equals_or_is_computed_predicate : public std::unary_function<FeatureInfo, bool> {
private:
	std::string m_other;
public:
	SQLite3_Feature_info_nocase_equals_or_is_computed_predicate(const std::string& str) : m_other(str) {};
	bool operator()(const FeatureInfo& fi) { return fi.getIsComputed() || strcmp_nocase(m_other, fi.getRetrievedFeatureName()) == 0; };
};


/** My own implementation of the std::remove_if algorithm.
 *
 * @internal
 *
 * This is implemented because the std::remove_if from <algorithm>
 * doesn't work on Visual C++ 6.0, nor does FeatureInfos.remove_if
 * work.
 *
 * @param FeatureInfos The list of FeatureInfo to remove something from
 * (input/output).
 *
 * @param name_to_remove The condition upon which entries in the list
 * should be removed.
 *
 */
void my_remove_if_sqlite3(std::list<FeatureInfo>& FeatureInfos, const std::string& name_to_remove)
{
	// Generate predicate
	SQLite3_Feature_info_nocase_equals_or_is_computed_predicate Pred = 
		SQLite3_Feature_info_nocase_equals_or_is_computed_predicate(name_to_remove);

	// Traverse list
	std::list<FeatureInfo>::iterator i(FeatureInfos.begin());
	while (i != FeatureInfos.end()) {
		if (Pred(*i)) {
			i = FeatureInfos.erase(i);
		} else {
			++i;
		}
	}
}

/** Remove indices which have a certain feature from the index cache.
 *@internal
 *
 * Removes all indices from the cache \p result which refer to the feature
 * by name \p feature_name.
 *
 * Is called by SQLite3EMdFDB::dropFeatureFromOT_objects().
 *
 * @see SQLite3EMdFDB::dropFeatureFromOT_objects().
 *
 * @param feature_name The feature which should not be referenced by
 * any index.
 * @param result The index cache to alter.
 */
void SQLite3EMdFDB::removeIndicesWithFeature(const std::string feature_name,
					    std::map<std::string, std::pair<bool, std::string> >& result)
{
	// Get lowercase version of feature name
	std::string feature_name_SQL;
	str_tolower(encodeFeatureName(feature_name), feature_name_SQL);
	std::map<std::string, std::pair<bool, std::string> >::iterator it, it2;
	it = result.begin();
	while (it != result.end()) {
		// Get lowercase version of index creation string
		std::string index_creation_string;
		str_tolower(it->second.second, index_creation_string);

		// Can we find the feature_name_SQL inside the 
		// index creation string?
		if (index_creation_string.find(feature_name_SQL) != std::string::npos) {
			// Yes, which means that we should not recreate this index
			// Erase at position it, but advance it before we erase
			it2 = it;
			++it;
			result.erase(it2);
		} else {
			// No, so we can create it -- leave it alone
			++it;
		}
	}
}

/** Parse SQL which creates an index.
 *@internal
 *
 * Parse an SQL string which had previously been used to create an index
 * in an SQLite database.
 *
 * The cache works as follows: It is a map from "index name" to a
 * pair<bool, string>, where the bool tells us whether the index was
 * created with the UNIQUE modifier, and the string is the complete
 * string used to identify the columns, including the parentheses (and
 * possibly a conflict resolution strategy).
 *
 * @param index_creation_string The SQL string to parse.
 * @param result The index cache to add to.
 * @return True if parsed correctly, false if string was not a valid
 * index creation string.  Note that we assume it is valid because it
 * comes from sqlite_master.
 */
static bool parseIndexCreationStringSQLite3(const std::string& index_creation_string, 
				     std::map<std::string, std::pair<bool, std::string> >& result)
{
	// Tokenize
	std::list<std::string> tokens;
	// It is necessary to strip so as to eliminate whitespace
	// before and after. If we don't we might end of up with the
	// last token being added twice to 'tokens', due to
	// peculiarities with C++ 'istr >> token'
	std::istringstream istr(strip(index_creation_string));
	std::string token;
	istr >> token;
	tokens.push_back(token);
	while (!istr.eof()) {
		// istr reads up to next whitespace
		istr >> token;
		tokens.push_back(token);
	}

	std::list<std::string>::const_iterator ci = tokens.begin();

	// Eat "CREATE"
	if (strcmp_nocase(*ci, "CREATE") != 0) {
		return false;
	}
	++ci;

	// Is it UNIQUE? If so, set boolean to true and skip it
	bool bIsUnique = false;;
	if (strcmp_nocase(*ci, "UNIQUE") == 0) {
		bIsUnique = true;
		++ci;
	}

	// Eat "INDEX"
	if (strcmp_nocase(*ci, "INDEX") != 0) {
		return false;
	}
	++ci;

	// Get index name
	std::string index_name = *ci;
	++ci;

	// Eat "ON"
	if (strcmp_nocase(*ci, "ON") != 0) {
		return false;
	}
	++ci;

	// Get table name
	std::string table_name = *ci;
	++ci;

	// Get rest of string
	std::string index_creation;
	for (; ci != tokens.end(); ++ci) {
		index_creation += *ci + " ";
	}

	// Place inside the map
	result[index_name] = std::pair<bool, std::string>(bIsUnique, index_creation);

	// If we came this far, there were no errors
	return true;
}

/** Get a cache of indexes on a given OT_object table.
 *@internal
 *
 * Queries sqlite_master to get the indices on OT_objects.  Returns
 * them as a map from index name (string) to the parenthesized list
 * (still string) of columns which make up the index, paired with a
 * boolean telling whether it is a UNIQUE index.
 *
 * @see parseIndexCreationStringSQLite3() for further details on the cache
 * structure.
 *
 * @param object_type_name The name of the object type.
 * @param result Used to return the index cache.
 * @return True on no database error, false if a database error occurred.
 */
bool SQLite3EMdFDB::getIndices(const std::string& object_type_name,
			      std::map<std::string, std::pair<bool, std::string> >& result)
{
	// Normalize object type name
	std::string OTN = normalizeOTName(object_type_name);

	// Build query. Don't use sprintf, as object_type_name comes from
	// an untrusted user.
	std::ostringstream query_stream;
	query_stream << "SELECT sql\n"
		     << "FROM sqlite_master\n"
		     << "WHERE type='index' AND tbl_name!=name\n"
		     << "      AND tbl_name='"  << normalizeTableName(OTN + "_objects", true) << "'";

	// Execute select
	if (!pConn->execSelect(query_stream.str())) {
		DEBUG_SELECT_QUERY_FAILED("SQLite3EMdFDB::getIndices", query_stream.str());
		return false;
	}

	// Get it from the backend
	std::string index_creation_string;
	bool bMoreRows = pConn->hasRow();
	while (bMoreRows) {
		try {	
			// Access result
			if (!pConn->accessTuple(0, index_creation_string)) {
				DEBUG_ACCESS_TUPLE_FAILED("SQLite3EMdFDB::getIndices");
				pConn->finalize();
				return false;
			}
			// OK, so we have result. Now add it to the map
			if (!parseIndexCreationStringSQLite3(index_creation_string, result)) {
				// We could not parse one of the indexes. Finalize and return error.
				pConn->finalize();
				return false;
			}
		} catch (EMdFNULLValueException& e) {
			// There was a NULL value exception, but this is not an error:
			// The primary key will return NULL.
			// Do one more round
		}

		// See if there are any more rows
		pConn->getNextTuple(bMoreRows);
	}    

	// Release results
	pConn->finalize();

	// If we got this far, there were no DB errors
	return true;
}

/** (Re)create indexes on OT_objects from index cache.
 *@internal
 *
 * Called by SQLite3EMdFDB::dropFeatureFromOT_objects() and
 * SQLite3EMdFDB::addFeatureToOT_objects().
 *
 * @see SQLite3EMdFDB::dropFeatureFromOT_objects() and
 * SQLite3EMdFDB::addFeatureToOT_objects().
 *
 * @param object_type_name The name of the object type.
 * @param mymap The index cache.
 * @return True on no database error, false if a database error occurred.
 */
bool SQLite3EMdFDB::createIndicesFromIndexMap(const std::string& object_type_name,
					     std::map<std::string, std::pair<bool, std::string> >& mymap)
{
	// Get table name ("OT_objects")
	std::string table_name = normalizeOTName(object_type_name) + "_objects";

	// Iterate over mape
	std::map<std::string, std::pair<bool, std::string> >::const_iterator ci;
	for (ci = mymap.begin(); ci != mymap.end(); ++ci) {
		//
		// Decode iterator
		//

		// Get index name
		std::string index_name = ci->first;

		// Get whether the index is UNIQUE
		std::string UNIQUE;
		if (ci->second.first) {
			UNIQUE = " UNIQUE ";
		} else {
			UNIQUE = " ";
		}

		// Get index creation string
		std::string index_creation_string = ci->second.second;

		//
		// Build query string. Don't use sprintf, since we don't
		// trust the user's object_type_name (present in table_name).
		// 
		std::ostringstream query_stream;
		query_stream 
			<< "CREATE" << UNIQUE << "INDEX " << normalizeIndexName(index_name) << '\n'
			<< "ON " << normalizeTableName(table_name, true) << '\n'
			<< index_creation_string;

		//
		// Execute query
		//
		if (!pConn->execCommand(query_stream.str().c_str())) {
			DEBUG_COMMAND_QUERY_FAILED("SQLite3EMdFDB::createIndicesFromIndexMap", query_stream.str().c_str());
			return false;
		}
	}

	// If we got this far, there were no errors
	return true;
}

/** Remove feature from OT_objects table.
 *@internal
 *
 * Implements an ALTER TABLE that drops a column.
 *
 * Makes sure that indexes which did not contain the feature to be
 * removed are preserved/recreated.
 *
 * @param object_type_name The name of the object type to alter.
 *
 * @param feature_name The name of the feature to drop.
 *
 * @param objectRangeType The eObjectRangeType of the object type.
 *
 * @param object_type_id The id_d of the object type.  Must be the same object
 *        type as object_type_name.
 *
 * @return True on no database error, false if a database error occurred.
 */
bool SQLite3EMdFDB::dropFeatureFromOT_objects(const std::string& object_type_name,
					     const std::string& feature_name,
					     eObjectRangeType objectRangeType,
					     eMonadUniquenessType monadUniquenessType,
					     id_d_t object_type_id)
{
	// Get list of current features
	std::list<FeatureInfo> FeatureInfos;
	if (!getFeaturesForObjectType(object_type_id, FeatureInfos)) {
		DEBUG_X_FAILED("SQLite3EMdFDB::dropFeatureFromOT_objects", "Getting all features");
		return false;
	}

	// Remove "our" feature from the list, as well as 'self'
	my_remove_if_sqlite3(FeatureInfos, std::string(feature_name));
	my_remove_if_sqlite3(FeatureInfos, std::string("self"));
	my_remove_if_sqlite3(FeatureInfos, std::string("monads"));


	// Normalize object type name
	std::string OTN = normalizeOTName(object_type_name);


	//
	// Create temporary table
	// 

	// Build query for creating OT_objects_backup, with list of features
	std::ostringstream OT_Objects_stream;
	OT_Objects_stream << "CREATE TABLE " << normalizeTableName(OTN + "_objects_backup", false) << " (\n"
		"    object_id_d INTEGER PRIMARY KEY NOT NULL,\n"
		"    first_monad INT NOT NULL";
	if (objectRangeType != kORTSingleMonad) {
		OT_Objects_stream 
			<< ",\n"
			<< "    last_monad INT NOT NULL";
	}
	if (objectRangeType == kORTMultipleRange) {
		OT_Objects_stream 
			<< ",\n"
			<< "    monads " << getSQL_TEXT_TYPE() << " NOT NULL";
	}
	std::list<FeatureInfo>::const_iterator f_it(FeatureInfos.begin());
	std::list<FeatureInfo>::const_iterator f_iend(FeatureInfos.end());
	while (f_it != f_iend) {
		// Make comma and newline
		OT_Objects_stream << ",\n";

		// Make SQL type
		makeFeatureSQLType(OT_Objects_stream, *f_it, true);

		if (featureTypeIdIsSOM(f_it->getRetrievedType())) {
			OT_Objects_stream << ",\n    first_monad_" << encodeFeatureName(f_it->getRetrievedFeatureName()) << " INT NOT NULL";
		}

		// Advance iterator
		++f_it;
	}

	// End the CREATE TABLE statement
	OT_Objects_stream << "\n)\n";

	// Create OT_Objects
	if (!pConn->execCommand(OT_Objects_stream.str().c_str())) {
		DEBUG_COMMAND_QUERY_FAILED("SQLite3EMdFDB::dropFeatureFromOT_objects", OT_Objects_stream.str().c_str());
		return false;
	}

	// 
	// SELECT into this new backup table
	// 
	std::ostringstream INSERT_INTO_stream;
	INSERT_INTO_stream << "SELECT object_id_d, first_monad";
	if (objectRangeType != kORTSingleMonad) {
		INSERT_INTO_stream << ", last_monad";
	}
	if (objectRangeType == kORTMultipleRange) {
		INSERT_INTO_stream << ", monads";
	}
	INSERT_INTO_stream << " ";
	f_it = FeatureInfos.begin();
	f_iend = FeatureInfos.end();
	while (f_it != f_iend) {
		INSERT_INTO_stream << ", " << encodeFeatureName(f_it->getRetrievedFeatureName());

		if (featureTypeIdIsSOM(f_it->getRetrievedType())) {
			INSERT_INTO_stream << ", first_monad_" << encodeFeatureName(f_it->getRetrievedFeatureName());
		}
		
		// Advance iterator
		++f_it;
	}

	// Make query
	std::string INSERT_INTO_query = "INSERT INTO " 
		+ normalizeTableName(OTN + "_objects_backup", false) + " "
		+ INSERT_INTO_stream.str() + "\n   FROM " 
		+ normalizeTableName(OTN + "_objects", false);

	// Copy table OT_objects into OT_objects_backup
	if (!pConn->execCommand(INSERT_INTO_query.c_str())) {
		DEBUG_COMMAND_QUERY_FAILED("SQLite3EMdFDB::dropFeatureFromOT_objects", INSERT_INTO_query.c_str());
		return false;
	}

	//
	// Get indices
	// 
	std::map<std::string, std::pair<bool, std::string> > index_name_index_creation_map;
	if (!getIndices(object_type_name, index_name_index_creation_map)) {
		DEBUG_X_FAILED("SQLite3EMdFDB::dropFeatureFromOT_objects", "getting indices from object type name '" + object_type_name + "'");
		return false;
	}

	// Remove indices on feature_name from indices
	removeIndicesWithFeature(feature_name, index_name_index_creation_map);

	//
	// DROP TABLE OT_objects
	//
	std::string DROP_OT_objects = "DROP TABLE " + normalizeTableName(OTN + "_objects", false);
	if (!pConn->execCommand(DROP_OT_objects.c_str())) {
		DEBUG_COMMAND_QUERY_FAILED("SQLite3EMdFDB::dropFeatureFromOT_objects", DROP_OT_objects.c_str());
		return false;
	}

	//
	// Create OT_objects again, with the feature removed
	//
	if (!createObjectTypeOT_objects(object_type_name, 
					objectRangeType, 
					monadUniquenessType,
					FeatureInfos)) {
		DEBUG_X_FAILED("SQLite3EMdFDB::dropFeatureFromOT_objects", "createObjectTypeOT_objects");
		return false;
	}

	//
	// Copy back
	//

	// Make query
	INSERT_INTO_query = "INSERT INTO " 
		+ normalizeTableName(OTN + "_objects", false) + " "
		+ INSERT_INTO_stream.str() + "\n   FROM " 
		+ normalizeTableName(OTN + "_objects_backup", false);

	// Copy table OT_objects_backup into OT_objects
	if (!pConn->execCommand(INSERT_INTO_query.c_str())) {
		DEBUG_COMMAND_QUERY_FAILED("SQLite3EMdFDB::dropFeatureFromOT_objects", INSERT_INTO_query.c_str());
		return false;
	}

	// DROP TABLE OT_objects_backup
	std::string DROP_OT_objects_backup = "DROP TABLE " 
		+ normalizeTableName(OTN + "_objects_backup", false);
	if (!pConn->execCommand(DROP_OT_objects_backup.c_str())) {
		DEBUG_COMMAND_QUERY_FAILED("SQLite3EMdFDB::dropFeatureFromOT_objects", DROP_OT_objects_backup.c_str());
		return false;
	}

	// 
	// Create indices anew
	// 
	if (!createIndicesFromIndexMap(object_type_name, index_name_index_creation_map)) {
		DEBUG_X_FAILED("SQLite3EMdFDB::dropFeatureFromOT_objects", "creating indices anew");
		return false;
	}

	// If we got this far, there were no DB errors
	return true;
}

bool SQLite3EMdFDB::createMonadIndexOnOTObjects(const std::string& object_type_name,
						eObjectRangeType objectRangeType)
{
	UNUSED(objectRangeType);
	
	// Normalize object type name
	std::string OTN = normalizeOTName(object_type_name);

	std::string monad_index_table_name = OTN + "_monad_index";

	bool bMonadIndexExists = false;
	if (!tableExists(monad_index_table_name, bMonadIndexExists)) {
		return false;
	} 

	if (bMonadIndexExists) {
		// Drop the index if it exists, to avoid old data getting
		// reinserted
		if (!dropMonadIndexOnOTObjects(object_type_name)) {
			return false;
		}
	}

	// CREATE VIRTUAL TABLE 
	std::string CREATE_monad_index_query = "CREATE VIRTUAL TABLE " 
		+ normalizeTableName(monad_index_table_name, false) + " USING rtree(\n"
		"   id,\n"
		"   first_monad,\n"
                "   last_monad)";
		
	if (!pConn->execCommand(CREATE_monad_index_query.c_str())) {
		DEBUG_COMMAND_QUERY_FAILED("SQLite3EMdFDB::createMonadIndexOnOTObjects", CREATE_monad_index_query.c_str());
		return false;
	}

	// XXX: Came to here!

	if (!pConn->execCommand(CREATE_monad_index_query.c_str())) {
		DEBUG_COMMAND_QUERY_FAILED("SQLite3EMdFDB::createMonadIndexOnOTObjects", CREATE_monad_index_query.c_str());
		return false;
	}

	

	return true;
}

bool SQLite3EMdFDB::dropMonadIndexOnOTObjects(const std::string& object_type_name)
{
	// Normalize object type name
	std::string OTN = normalizeOTName(object_type_name);

	std::string monad_index_table_name = OTN + "_monad_index";

	// DROP query
	std::string DROP_monad_index_query = "DROP TABLE " 
		+ normalizeTableName(monad_index_table_name, false);
		
	if (!pConn->execCommand(DROP_monad_index_query.c_str())) {
		DEBUG_COMMAND_QUERY_FAILED("SQLite3EMdFDB::dropMonadIndexOnOTObjects", DROP_monad_index_query.c_str());
		return false;
	}

	return true;
}


/** Add feature to OT_objects table.
 *@internal
 *
 * Implements an ALTER TABLE that adds a column.
 *
 * Makes sure that indexes are preserved/recreated.
 *
 * @param object_type_name The name of the object type to alter.
 *
 * @param object_type_id The id_d of the object type.  Must be the
 * same object type as \p object_type_name.
 *
 * @param objectRangeType the eObjectRangeType of the object type.
 *
 * @param fi The FeatureInfo description of the feature to add.
 *
 * @return True on no database error, false if a database error occurred.
 */
bool SQLite3EMdFDB::addFeatureToOT_objects(const std::string& object_type_name,
					  id_d_t object_type_id,
					  eObjectRangeType objectRangeType,
					  eMonadUniquenessType monadUniquenessType,
					  const FeatureInfo& fi)
{
	UNUSED(object_type_id);
	UNUSED(objectRangeType);
	UNUSED(monadUniquenessType);

	std::string OTN = normalizeOTName(object_type_name);

	// Make query
	std::ostringstream ALTER_TABLE_query_str;
	ALTER_TABLE_query_str << "ALTER TABLE "
			      << normalizeTableName(OTN + "_objects", false)
			      << " ADD COLUMN ";
	std::string secondary_ALTER_TABLE_query = ALTER_TABLE_query_str.str();
	bool bUseSecondary_ALTER_TABLE_query = false;
	makeFeatureSQLType(ALTER_TABLE_query_str, fi, true);
	ALTER_TABLE_query_str << " DEFAULT ";
	id_d_t feature_type_id = fi.getRetrievedType();
	if (featureTypeIdIsListOf(feature_type_id)) {
		// We cannot at present have default values for lists, so
		// we just add '', the empty list
		ALTER_TABLE_query_str << "''";
	} else {
		if (featureTypeIdIsSTRING(feature_type_id)
		    || featureTypeIdIsASCII(feature_type_id)) {
			ALTER_TABLE_query_str << escapeStringForSQL(fi.getDefaultValue());
		} else if (featureTypeIdIsSOM(feature_type_id)) {
			ALTER_TABLE_query_str << escapeStringForSQL(fi.getDefaultValue());
			secondary_ALTER_TABLE_query += " first_monad_" + encodeFeatureName(fi.getRetrievedFeatureName()) + " INT DEFAULT ";
			SetOfMonads som;
			som.fromString(fi.getDefaultValue());
			if (som.isEmpty()) {
				secondary_ALTER_TABLE_query += monad_m2string(MAX_MONAD); // first_monad_mdf_FEATURE_NAME
			} else {
				secondary_ALTER_TABLE_query += monad_m2string(som.first()); // first_monad_mdf_FEATURE_NAME
			}
			bUseSecondary_ALTER_TABLE_query = true;
		} else if (featureTypeIdIsENUM(feature_type_id)) {
			// Find the value of the enumeration constant.
			bool bExists;
			bool bDummyIsDefault;
			long enum_value;
			if (!enumConstExists(fi.getDefaultValue(),
					     feature_type_id,
					     bExists,
					     enum_value,
					     bDummyIsDefault)) {
				DEBUG_X_IS_WRONG("SQLite3EMdFDB::addFeatureToOT_objects", 
						 "Database error: Could not find enumeration constant with name '" + fi.getDefaultValue() + "'.");
				return false;
			}
			if (!bExists) {
				DEBUG_X_IS_WRONG("SQLite3EMdFDB::addFeatureToOT_objects", 
						 "Enum constant with name '" + fi.getDefaultValue() + "' did not exist.");
				return false;
			} 
			ALTER_TABLE_query_str << enum_value;
		} else {
			ALTER_TABLE_query_str << fi.getDefaultValue();
		}
	}

	// Begin transaction
	bool bDoCommit = pConn->beginTransaction();

	// Copy table OT_objects_backup into OT_objects
	if (!pConn->execCommand(ALTER_TABLE_query_str.str().c_str())) {
		DEBUG_COMMAND_QUERY_FAILED("SQLite3EMdFDB::addFeatureToOT_objects", ALTER_TABLE_query_str.str().c_str());
		if (bDoCommit) {
			pConn->abortTransaction();
		}
		return false;
	}

	// Copy table OT_objects_backup into OT_objects
	if (bUseSecondary_ALTER_TABLE_query) {
		if (!pConn->execCommand(secondary_ALTER_TABLE_query.c_str())) {
			DEBUG_COMMAND_QUERY_FAILED("SQLite3EMdFDB::addFeatureToOT_objects", secondary_ALTER_TABLE_query.c_str());
			if (bDoCommit) {
				pConn->abortTransaction();
			}
			return false;
		}
	}

	// Make query for updating with default value
	std::ostringstream UPDATE_query_str;
	UPDATE_query_str << "UPDATE "
			 << normalizeTableName(OTN + "_objects", false)
			 << " SET "
			 << encodeFeatureName(fi.getRetrievedFeatureName())
			 << " = ";
	if (featureTypeIdIsListOf(feature_type_id)) {
		// We cannot at present have default values for lists, so
		// we just add '', the empty list
		UPDATE_query_str << "''";
	} else if (featureTypeIdIsSTRING(feature_type_id)
		   || featureTypeIdIsASCII(feature_type_id)) {
		UPDATE_query_str << escapeStringForSQL(fi.getDefaultValue());
	} else if (featureTypeIdIsSOM(feature_type_id)) {
		UPDATE_query_str << escapeStringForSQL(fi.getDefaultValue())
				 << ", first_monad_" << encodeFeatureName(fi.getRetrievedFeatureName())
				 << " = ";
		SetOfMonads som;
		som.fromString(fi.getDefaultValue());
		if (som.isEmpty()) {
			UPDATE_query_str << monad_m2string(MAX_MONAD); // first_monad_mdf_FEATURE_NAME
		} else {
			UPDATE_query_str << monad_m2string(som.first()); // first_monad_mdf_FEATURE_NAME
		}
	} else if (featureTypeIdIsENUM(feature_type_id)) {
		// Find the value of the enumeration constant.
		bool bExists;
		bool bDummyIsDefault;
		long enum_value;
		if (!enumConstExists(fi.getDefaultValue(),
				     feature_type_id,
				     bExists,
				     enum_value,
				     bDummyIsDefault)) {
			DEBUG_X_IS_WRONG("SQLite3EMdFDB::addFeatureToOT_objects", 
					 "Database error: Could not find enumeration constant with name '" + fi.getDefaultValue() + "'.");
			return false;
		}
		if (!bExists) {
			DEBUG_X_IS_WRONG("SQLite3EMdFDB::addFeatureToOT_objects", 
					 "Enum constant with name '" + fi.getDefaultValue() + "' did not exist.");
			return false;
		} 
		UPDATE_query_str << enum_value;
	} else {
		UPDATE_query_str << fi.getDefaultValue();
	}

	// Copy table OT_objects_backup into OT_objects
	if (!pConn->execCommand(UPDATE_query_str.str().c_str())) {
		DEBUG_COMMAND_QUERY_FAILED("SQLite3EMdFDB::addFeatureToOT_objects", UPDATE_query_str.str().c_str());
		if (bDoCommit) {
			pConn->abortTransaction();
		}
		return false;
	}

	// Commit transaction
	if (bDoCommit) {
		if (!pConn->commitTransaction()) {
			DEBUG_COMMITTRANSACTION_FAILED("SQLite3EMdFDB::addFeatureToOT_objects");
			return false;
		}
	}

	// If we got this far, there were no DB errors
	return true;
}

/** Checks whether database name is OK.
 *@internal
 *
 * This is overridden from EMdFDB.  We just assume it is OK, since
 * checking filename syntax and semantics is hairy.
 * 
 * @param db_name the database name to be checked
 * @param bIsOK set to true if db_name is OK, false otherwise.
 */
void SQLite3EMdFDB::databaseNameIsOK(const std::string& db_name, bool& bIsOK)
{
	UNUSED(db_name);
	
	// FIXME: This should probably check that the name is a legal filename.
	// For now, just assume it is OK.
	bIsOK = true;
}


/** Escape a string so that it can be used in an SQL 'string'.
 *@internal
 *
 * @param str The string to escape.
 * @return The escaped string.
 */
std::string SQLite3EMdFDB::escapeStringForSQL(const std::string& str)
{
	std::string result;
	result.reserve(str.length()*2 + 3);
	result = "'";
	std::string::size_type length = str.length();
	for (std::string::size_type i = 0; i < length; ++i) {
		unsigned char c = (unsigned char) str[i];
		if (c == '\'') {
			result += "''";
		} else {
			result += c;
		}
	}
	result += '\'';
	return result;
	/*
	static const char *upperhexchars = "0123456789ABCDEF";
	result = "X'";
	std::string::size_type length = str.length();
	for (std::string::size_type i = 0; i < length; ++i) {
		unsigned char c = (unsigned char) str[i];
		unsigned int c2;
		c2 = (c >> 4) & 0x0F;
		// result += (c2 >= 10) ? ('7' + c2) : ('0' + c2); // 'A' - 10 = '7'
		result += upperhexchars[c2];
		c2 = c & 0x0F;
		// result += (c2 >= 10) ? ('7' + c2) : ('0' + c2); // 'A' - 10 = '7'
		result += upperhexchars[c2];
		
	}
	result += '\'';
	return result;
	*/
}

/** Batch-create objects.
 *@internal
 *
 * This is the entry point for the EMdF-part of the implementation of
 * the MQL statement "CREATE OBJECTS WITH OBJECT TYPE.".
 *
 * Each object in \p object_list must have an array of EMdFValue with
 * the same order as \p object_type_features.  \p object_type_features
 * must be the full list of features in the object type, minus self.
 * The InstObject's in \p object_list must have an array of EMdFValue
 * with the same order as \p object_type_features.  Upon return, \p
 * object_count will contain the number of objects successfully
 * created.
 *
 * @param object_type_name The name of the object type to contain the
 * objects.
 *
 * @param object_type_id The id_d of the object type to contain the
 * objects.
 *
 * @param object_type_features A list of the FeatureInfo's (minus
 * \p self) used in each InstObject in \p object_list.
 *
 * @param object_list A list of InstObject's.  Each InstObject must
 * have EMdFValue's in the same order as in \p object_type_features.
 *
 * @param objectRangeType Tells us whether the object type is a "WITH
 * MULTIPLE RANGE OBJECTS", "WITH SINGLE RANGE OBJECTS", or "WITH
 * SINGLE MONAD OBJECTS".
 *
 * @param object_count Used to return the number of objects created.
 *
 * @return True on no database error, false if a database error occurred.
 */
bool SQLite3EMdFDB::createObjects(const std::string& object_type_name,
				 id_d_t object_type_id,
				 const std::list<FeatureInfo>& object_type_features,
				 std::list<InstObject*>& object_list,
				 eObjectRangeType objectRangeType,
				 /* out */ long& object_count)
{
	if (pConn == 0) {
		return false;
	} else {
		// Normalize object type name
		std::string OTN = normalizeOTName(object_type_name);

		// Declare
		monad_m min_m_candidate = MAX_MONAD;
		monad_m max_m_candidate = 0;
		id_d_t max_id_d = 0;

		
		// We cannot set this to 1, since there may be 0 objects in object_list.
		monad_m largest_object_length_candidate = 0;

		// Get whether this is 

		std::list<InstObject*>::const_iterator ci = object_list.begin();
		std::list<InstObject*>::const_iterator cend = object_list.end();
		while (ci != cend) {
			// Take object's monads.
			// We do this because we don't want to create the set of monads
			// on the stack twice. InstObject::getMonads returns a full
			// object, not a reference or a pointer.
			SetOfMonads som;
			(*ci)->getMonads(som);

			monad_m object_length = som.last() - som.first() + 1;
			if (object_length > largest_object_length_candidate) {
				largest_object_length_candidate = object_length;
			}

			// Deal with min_m/max_m
			monad_m first_monad = som.first();
			monad_m last_monad = som.last();
			if (first_monad < min_m_candidate) {
				min_m_candidate = first_monad;
			}
			if (last_monad > max_m_candidate) {
				max_m_candidate = last_monad;
			}

			// Deal with max_id_d
			id_d_t current_id_d = (*ci)->getID_D();
			if (current_id_d > max_id_d) {
				max_id_d = current_id_d;
			}

			// Create data in OT_objects
			if (!createObjectsOT_objects(OTN, 
						     object_type_id,
						     *ci, 
						     som, 
						     objectRangeType,
						     object_type_features)) {
				DEBUG_X_FAILED("SQLite3EMdFDB::createObjects", "creating data in OT_objects");
				return false;
			}
	
			++ci;
			++object_count;
		}

		// Update min_m to min_m_candidate if necessary
		if (!setMin_m(min_m_candidate)) {
			DEBUG_X_FAILED("SQLite3EMdFDB::createObjects", "Setting min_m");
			return false;
		}

		// Update max_m to max_m_candidate if necessary
		if (!setMax_m(max_m_candidate)) {
			DEBUG_X_FAILED("SQLite3EMdFDB::createObjects", "Setting max_m");
			return false;
		}

		// Update largest object length if necessary
		if (!setLargestObjectLengthIfNecessary(OTN, 
						       largest_object_length_candidate, 
						       objectRangeType,
						       false)) {
			DEBUG_X_FAILED("SQLite3EMdFDB::createObjects", "Setting largest object length");
			return false;
		}

		// Update sequence 0 to max_id_d+1 if appropriate
		if (!setNextObjectID_DIfNotHigher(max_id_d+1)) {
			DEBUG_X_FAILED("SQLite3EMdFDB::createObjects", "Setting object id_d sequence");
			return false;
		}

		// If we came this far, there were no DB errors
		return true;
	}
}

/** Batch-add an object to OT_objects
 *@internal
 *
 * Called from SQLite3EMdFDB::createObjects().
 *
 * @see SQLite3EMdFDB::createObjects().
 * 
 * @param object_type_name The name of the object type.
 *
 * @param object_type_id The id_d of the object type.
 *
 * @param pObject The object to add.  The InstObject must
 * have EMdFValue's in the same order as in \p object_type_features.
 *
 * @param som The new set of monads for the object.
 *
 * @param objectRangeType the eObjectRangeType of the object.
 *
 * @param object_type_features A list of the FeatureInfo's (minus
 * \p self) used in \p pObject.
 *
 * @return True on no database error, false if a database error occurred.
 */
bool SQLite3EMdFDB::createObjectsOT_objects(const std::string& object_type_name, 
					   id_d_t object_type_id,
					   const InstObject *pObject,
					   const SetOfMonads& som,
					   eObjectRangeType objectRangeType,
					   const std::list<FeatureInfo>& object_type_features)
{
	std::string OT_objects_data;
	OT_objects_data = "INSERT INTO ";
	OT_objects_data += normalizeTableName(object_type_name + "_objects", false);
	OT_objects_data += "(object_id_d,first_monad";
	if (objectRangeType != kORTSingleMonad) {
		OT_objects_data += ",last_monad";
	}
	if (objectRangeType == kORTMultipleRange) {
		OT_objects_data += ",monads";
	}
	std::list<FeatureInfo>::const_iterator ci = object_type_features.begin();
	std::list<FeatureInfo>::const_iterator cend = object_type_features.end();
	while (ci != cend) {
		OT_objects_data += ',';
		OT_objects_data += encodeFeatureName(ci->getRetrievedFeatureName());
		if (featureTypeIdIsSOM(ci->getRetrievedType())) {
			OT_objects_data += ",first_monad_" + encodeFeatureName(ci->getRetrievedFeatureName());
		}
		++ci;
	}
	OT_objects_data += ")VALUES(";

	// object id_d
	OT_objects_data += id_d2number_string(pObject->getID_D());
	OT_objects_data += ',';

	// First monad
	OT_objects_data += monad_m2string(som.first());

	// Last monad
	if (objectRangeType != kORTSingleMonad) {
		OT_objects_data += ',';
		OT_objects_data += monad_m2string(som.last());
	}

	if (objectRangeType == kORTMultipleRange) {
		OT_objects_data += "," + escapeStringForSQL(som.toCompactString());
	}

	ci = object_type_features.begin();
	int index = 0;
	while (ci != cend) {
		// Emit comma after previous column
		OT_objects_data += ',';

		// Access object's feature at the given index 
		const EMdFValue *pValue = pObject->getFeature(index);

		// Get feature's type
		id_d_t feature_type_id = ci->getRetrievedType();
		IntegerList *pIntegerList = 0;
		switch (feature_type_id & FEATURE_TYPE_TYPE_MASK) {
		case FEATURE_TYPE_ASCII:
			if (featureTypeIdIsFromSet(feature_type_id)) {
				FeatureInfo myfi(ci->getFeatureName(),
						 "",
						 ci->getRetrievedType(), pValue->getString());
				// The penultimate "true" on FeatureInfo2SQLvalue means that 
				// we must create any IDD-String association if it is not 
				// there in the OT_mdf_FEATURE_NAME_set table.
				OT_objects_data += FeatureInfo2SQLvalue(object_type_name, object_type_id, true, myfi);
			} else {
				OT_objects_data += escapeStringForSQL(pValue->getString());
			}
			break;
		case FEATURE_TYPE_STRING:
			if (featureTypeIdIsFromSet(feature_type_id)) {
				FeatureInfo myfi(ci->getFeatureName(),
						 "",
						 ci->getRetrievedType(), pValue->getString());
				// The penultimate "true" on FeatureInfo2SQLvalue means that 
				// we must create any IDD-String association if it is not 
				// there in the OT_mdf_FEATURE_NAME_set table.
				OT_objects_data += FeatureInfo2SQLvalue(object_type_name, object_type_id, true, myfi);
			} else {
				OT_objects_data += escapeStringForSQL(pValue->getString());
			}
			break;
		case FEATURE_TYPE_INTEGER:
			OT_objects_data += long2string(pValue->getInt());
			break;
		case FEATURE_TYPE_ID_D:
			OT_objects_data += id_d2number_string(pValue->getID_D());
			break;
		case FEATURE_TYPE_ENUM:
			OT_objects_data += long2string(pValue->getEnum());
			break;
		case FEATURE_TYPE_SET_OF_MONADS:
			{
				SetOfMonads som = pValue->getSOM();
				OT_objects_data += escapeStringForSQL(som.toCompactString());
				// Emit comma after previous column
				OT_objects_data += ',';
				if (som.isEmpty()) {
					OT_objects_data += monad_m2string(MAX_MONAD);
				} else {
					OT_objects_data += monad_m2string(som.first());
				}
			}
			break;
		case FEATURE_TYPE_LIST_OF_INTEGER:
		case FEATURE_TYPE_LIST_OF_ID_D:
		case FEATURE_TYPE_LIST_OF_ENUM:
			ASSERT_THROW(pValue->getKind() == kEVListOfInteger
				     || pValue->getKind() == kEVListOfID_D,
				     "pValue's getKind() is neither kEVListOfInteger nor kEVListOfID_D");
			pIntegerList = pValue->getIntegerList();
			if (pIntegerList != 0) {
				OT_objects_data += escapeStringForSQL(pIntegerList->getDelimitedString(DEFAULT_LIST_DELIMITER));
			} else {
				OT_objects_data += "''";
			}
			break;
		}

		// Advance index and iterator
		++index;
		++ci;
	}

	OT_objects_data += ");";

	// Run it
	if (!pConn->execCommand(OT_objects_data)) {
		DEBUG_COMMAND_QUERY_FAILED("SQLite3EMdFDB::createObjectsOT_objects", OT_objects_data);
		return false;
	}

	// If we came this far, there were no DB errors
	return true;
}

/** get ID_D from string set
 *@internal
 *
 * Get a string value id_d from a string set.  First tries the cache,
 * but if it doesn't find it there, then queries the backend.
 *
 * @param normalized_object_type_name the name of the object type
 * which holds the feature.  Must have been normalized with
 * NormalizeOTN().  Adds the set id_d/string-value pair to the cache
 * if necessary.
 *
 * @param object_type_id the object type id of the object type which
 * holds the feature.
 *
 * @param encoded_feature_name The "mdf_<feature-name>"-encoded
 * version of the feature name.
 *
 * @param encoded_string_to_create The string to be created. Must be
 * encoded with EMdFDB::escapeStringForSQL().
 *
 * @param original_string The string to be created. Must be the original
 * string, before encoding with EMdFDB::escapeStringForSQL().
 *
 * @param result The set id_d value created
 *
 * @return true on found, false on not found or DB error.
 */
bool SQLite3EMdFDB::addStringToStringSetTable(const std::string& normalized_object_type_name,
					     const std::string& encoded_feature_name,
					     const std::string& encoded_string_to_create, // Must be encoded with escapeStringForSQL
					     /* out */ id_d_t& result)
{
	// Calculate name of table
	std::string table_name = normalized_object_type_name 
		+ "_" + encoded_feature_name + "_set";

	// Get query to add it
	std::string query = "INSERT INTO " + normalizeTableName(table_name, true) + " VALUES(NULL," + encoded_string_to_create + ")";
	
	// Execute query
	if (!pConn->execCommand(query)) {
		DEBUG_COMMAND_QUERY_FAILED("SQLite3EMdFDB::addStringToStringSetTable", query.c_str());
		return false;
	}

	// Get id_d
	((SQLite3EMdFConnection*)pConn)->get_last_insert_rowid(result);

	// If we got this far, there were no errors
	return true;
}


eCharsets SQLite3EMdFDB::getDefaultEncoding(void) const
{
	return kCSUTF8;
}


bool SQLite3EMdFDB::canDeclareEncoding(eCharsets charset) const
{
	return charset == kCSUTF8;
}


std::string SQLite3EMdFDB::getEncodingClause(eCharsets charset) const
{
	UNUSED(charset);

	/* There is nothing to do. The reason is that we cannot change
           it at compile time (it is built in into SQLite 3).
	*/
	return ""; 
}





#endif // USE_SQLITE3
