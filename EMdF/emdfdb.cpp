/*
 * emdfdb.cpp
 *
 * EMdF database base class
 *
 * Ulrik Petersen
 * Created: 1/27-2001
 * Last update: 5/10-2018
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2018  Ulrik Sandborg-Petersen
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

/**@file emdfdb.cpp
 *@brief Implementation of EMdFDB (EMdF layer)
 */

#include <emdros-config.h>
// Do this because emdros-config might #define malloc,
// in which case some systems will fail to use the real 
// malloc, thus causing build errors.
#undef malloc


#include <table.h>
#include <conn.h>
#include <inst.h>
#include <monads.h>
#include <emdfdb.h>
#include <emdf_value.h>
#include <emdf_ffeatures.h>
#include <enum_const_cache.h>
#include <string_func.h>
#include <string_list.h>
#include <string_set_cache.h>
#include <crc32_emdros.h>
#include <utils.h>
#include <debug.h>
#include <iostream>
#include <sstream>
#include <set>
#include <logging.h>
#if defined(HAVE_UNISTD_H) && HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef WIN32
#include <windows.h>
typedef unsigned int uint32_t;
#else
extern "C" {
  /* For uint32_t. */
#include <stdint.h>
}
#endif


#ifdef WIN32
static void sleep(DWORD milliseconds)
{
	SleepEx(milliseconds, FALSE);
}
#endif


#ifdef __DJGPP__
static void sleep(short int milliseconds)
{
	// Note: This method isn't really necessary, so
	// this hack is OK.
	volatile double mydouble = 1.34;
	for (int i = 0; i < milliseconds*10000; ++i) {
		mydouble = (mydouble * 1.2);
	}
}
#endif

/** Decode object range type from Object Type flags.
 *
 *@internal
 *
 *@param object_type_flags The Object type flags to decode.
 *
 *@return The right eObjectRangeType enumeration constant.
 */
inline eObjectRangeType get_object_range_type_from_OT_flags(long object_type_flags) {
	long masked_OT_flags = object_type_flags & OT_RANGE_MASK;
	switch (masked_OT_flags) {
	case OT_WITH_MULTIPLE_RANGE_OBJECTS:
		return kORTMultipleRange;
		break;
	case OT_WITH_SINGLE_RANGE_OBJECTS:
		return kORTSingleRange;
		break;
	case OT_WITH_SINGLE_MONAD_OBJECTS:
		return kORTSingleMonad;
		break;
	default:
		ASSERT_THROW(false, "Unknown OT range");
		return kORTMultipleRange; // Just to fool the compiler.
	}
	return kORTMultipleRange; // Just to fool the compiler.
}

/** Decode monad uniqueness type from Object Type flags.
 *
 *@internal
 *
 *@param object_type_flags The Object type flags to decode.
 *
 *@return The right eMonadUniquenessType enumeration constant.
 *
 */
inline eMonadUniquenessType get_monad_uniqueness_type_from_OT_flags(long object_type_flags) {
	long masked_OT_flags = object_type_flags & OT_MONAD_UNIQUENESS_MASK;
	switch (masked_OT_flags) {
	case OT_WITHOUT_UNIQUE_MONADS:
		return kMUTNonUniqueMonads;
		break;
	case OT_HAVING_UNIQUE_FIRST_MONADS:
		return kMUTUniqueFirstMonads;
		break;
	case OT_HAVING_UNIQUE_FIRST_AND_LAST_MONADS:
		return kMUTUniqueFirstAndLastMonads;
		break;
	default:
		ASSERT_THROW(false, "Unknown monad uniqueness");
		return kMUTNonUniqueMonads;  // Just to fool the compiler.
	}
	return kMUTNonUniqueMonads;  // Just to fool the compiler.
}

/** Decode objectRangeType to the names for first and last monad.
 *
 *@internal
 *
 *If this is "single monad", then both firstm and lastm will be
 *"first_monad". Otherwise, "firstm" will be "first_monad" and lastm
 *will be "last_monad."
 *
 *@param objectRangeType The ORT to decode.
 *
 *@param firstm The name of the "first monad" SQL column (output).
 *
 *@param lastm The name of the "last monad" SQL column (output).
 */
inline void bool2firstm_lastm(eObjectRangeType objectRangeType, 
			      std::string& firstm, 
			      std::string& lastm)
{
	if (objectRangeType == kORTSingleMonad) {
		firstm = "first_monad";
		lastm = "first_monad";
	} else {
		firstm = "first_monad";
		lastm = "last_monad";
	}
}



/** Get index and table name for feature
 *@internal
 *
 * Utility function to construct, from an object type name and an
 * uncoded feature name, the table name OT_objects and the name of the
 * index on the feature (OT_mdf_<feature-name>_i).
 *
 * @param object_type_name The name of the object type 
 *
 * @param unencoded_feature_name The unencoded name of the feature
 * (i.e., it is what the user would see, not the mdf_<feature-name>
 * encoded form.
 *
 * @param table_name The output result for the table name
 *
 * @param index_name The output result for the index name.
 */
void EMdFDB::getIndexAndTableNameForFeature(const std::string& object_type_name, 
					    const std::string& unencoded_feature_name, 
					    /* out */ std::string& table_name, 
					    /* out */ std::string& index_name)
{
	// Normalize object type name
	std::string OTN = normalizeOTName(object_type_name);

	// Get table name
	table_name = OTN + "_objects";

	// Get index name
	index_name = OTN + "_" + encodeFeatureName(unencoded_feature_name) + "_i";
}

/** Constructor.
 *
 * Should NOT be used directly.  Instead, use a descendant, like
 * PgEMdFDB::PgEMdFDB() or MySQLEMdFDB::MySQLEMdFDB().  Or better yet,
 * use EmdrosEnv::EmdrosEnv().
 */
EMdFDB::EMdFDB(eBackendKind backend_kind) 
	: pConn(0), m_backend_kind(backend_kind)
{
	m_enum_const_cache = new EnumConstCache;
	m_enum_cache = new Table;
	m_feature_cache = new Table;
	m_string_sets_cache = new StringSetsCache;

	m_schema_version = EMDROS_VERSION_CURRENT;
	m_max_table_name_length = 63;
}

/** Destructor.
 * 
 * Deletes the connection objects, as well as anything allocated by the
 * EMdFDB::EMdFDB() constructor.
 */
EMdFDB::~EMdFDB()        
{
	if (pConn != 0)
		delete pConn;
	delete m_enum_cache;
	delete m_enum_const_cache;
	delete m_feature_cache;
	delete m_string_sets_cache;
}



//
// Sequence manipulation
/** set next object id_d sequence number.
 *@internal
 *
 * Sets the next ID_D sequence number to \p next_id_d if current ID_D
 * sequence number is not already greater.
 *
 * Must be overridden in descendant.
 * 
 * @param next_id_d the id_d to set the sequence to.
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::setNextObjectID_DIfNotHigher(id_d_t next_id_d)
{
	UNUSED(next_id_d);
	
	return false; // Must be overridden
}

/** get next object id_d sequence number.
 *@internal
 *
 * Calls EMdFDB::getNext().
 *
 * @see EMdFDB::getNext().
 * 
 * @param id_d Used to return the id_d
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::getNextObjectID_D(/* out */ id_d_t& id_d)
{
	if (pConn == 0)
		return false;
	else {
		return getNextID(SEQUENCE_OBJECT_ID_DS, id_d);
	}
}

/** get next type id_d sequence number.
 *@internal
 *
 * Calls EMdFDB::getNext().
 *
 * @see EMdFDB::getNext().
 * 
 * @param id_d Used to return the id_d
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::getNextTypeID(/* out */ id_d_t& id_d)
{
	if (pConn == 0)
		return false;
	else {
		bool result = getNextID(SEQUENCE_TYPE_IDS, id_d);

		// We shift-left a number of bits (see emdf.h).
		// This is so as to leave room for bit-fields below.
		id_d <<= SEQUENCE_TYPE_IDS_FREE_LOWER_BITS;
		return result;
	}
}

/** get next other id_d (not object id_d, not type id) sequence number.
 *@internal
 *
 * Calls EMdFDB::getNext().
 *
 * @see EMdFDB::getNext().
 * 
 * @param id_d Used to return the id_d
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::getNextOtherID(/* out */ id_d_t& id_d)
{
	if (pConn == 0)
		return false;
	else {
		bool result = getNextID(SEQUENCE_OTHER_IDS, id_d);
		return result;
	}
}

/** database name is OK.
 *@internal
 *
 * Sets bIsOK to true if db_name is a valid database name, i.e., if it
 * is an identifier.
 *
 * @see is_identifier().
 * 
 * @param db_name the database name to be checked
 * @param bIsOK set to true if db_name is OK, false otherwise.
 */
void EMdFDB::databaseNameIsOK(const std::string& db_name, bool& bIsOK)
{
	bIsOK = is_identifier(db_name);
}

//
// Database manipulation
/** Create database.
 *@internal
 *
 * Must be overridden in a subclass.
 *
 * Note that the encryption key is only used for BPT and SQLite 3,
 * provided you have an SQLite 3 with encryption enabled (available
 * from Emdros's author).
 *
 * @param db_name the database name to be created
 * @param key the encryption key to be used.
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::createDatabase(const std::string& db_name, const std::string& key)
{
	return createDatabase(db_name, getDefaultEncoding(), key);
}

/** Create database.
 *@internal
 *
 * Must be overridden in a subclass.
 *
 * Note that the encryption key is only used for SQLite 3, and only if
 * you have an SQLite 3 that supports encryption.
 *
 * @param db_name the database name to be created
 * @param key the encryption key to be used.
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::createDatabase(const std::string& db_name, eCharsets charset, const std::string& key)
{
	UNUSED(db_name);
	UNUSED(charset);
	UNUSED(key);
	
	return false; // You must override this in the descendant class
}

//
/** Initialize database.
 *@internal
 *
 * All tables with meta-data are initialized.
 *
 * May be overridden in a subclass, but the subclass should probably
 * call this method.
 *
 * Note that the encryption key is only used for SQLite 3, and only if
 * you have an SQLite 3 that supports encryption.
 *
 * @param db_name the database name to be initialized.
 * @param key the encryption key to be used.
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::initializeDatabase(const std::string& db_name, const std::string& key)
{
	if (pConn == 0)
		return false;
	else {
		// Get a DBname which we can use
		std::string NewDbName;
		convertDBName(db_name, NewDbName);

		// Switch to new database
		if (!useDatabase(db_name, key, true)) {
			DEBUG_X_FAILED("EMdFDB::initializeDatabase", "useDatabase");
			return false;
		}

		// Check whether it is initialized already
		bool bIsInitialized;
		if (!dbIsInitialized(bIsInitialized)) {
			// There was a DB error checking initialization
			return false;
		} else {
			// There was no DB error checking initialization
			
			// So was it initialized?
			if (bIsInitialized) {
				// Yes. Return error
				DEBUG_X_IS_WRONG("EMdFDB::initializeDatabase", "Database was initialized already");
				return false;
			} 
		}
		
		// No, it wasn't initialized

		// Begin transaction
		bool bDoCommit;
		bDoCommit = pConn->beginTransaction();

		const char *WITHOUT_OIDS = "";
		if (m_backend_kind == kPostgreSQL) {
			WITHOUT_OIDS = "\nWITHOUT OIDS";
		}

		if (!createSchemaVersionTable(EMDROS_VERSION_CURRENT)) {
			DEBUG_X_FAILED("EMdFDB::initializeDatabase", "createSchemaVersionTable");
			if (bDoCommit)
				pConn->abortTransaction();
			return false;
		}

		if (!createSequenceTables()) {
			DEBUG_X_FAILED("EMdFDB::initializeDatabase", "createSequenceTables");
			if (bDoCommit)
				pConn->abortTransaction();
			return false;
		}


		// Create table 'enumerations'
		{
			std::ostringstream query_stream;
			query_stream 
				<< "CREATE TABLE enumerations (\n"
				<< "    enum_id INTEGER PRIMARY KEY NOT NULL,\n"
				<< "    enum_name VARCHAR("
				<< int2string(MAX_IDENTIFIER_CHARS)
				<< ") " << getMYSQL_BINARY() << " NOT NULL"
				<< "\n)" << WITHOUT_OIDS;
			if (!pConn->execCommand(query_stream.str())) {
				DEBUG_COMMAND_QUERY_FAILED("EMdFDB::initializeDatabase", query_stream.str());
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}
		}

		// Create table 'enumeration_constants'
		{
			std::ostringstream query_stream;
			query_stream 
				<< "CREATE TABLE enumeration_constants (\n"
				<< "    enum_id INT NOT NULL,\n"
				<< "    enum_value_name VARCHAR("
				<< int2string(MAX_IDENTIFIER_CHARS)
				<< ") " 
				<< getMYSQL_BINARY()
				<< " NOT NULL,\n"
				<< "    value INT NOT NULL,\n"
				<< "    is_default CHAR(1) NOT NULL,\n"
				<< "    PRIMARY KEY (enum_id, enum_value_name)\n"
				<< ")" << WITHOUT_OIDS;
			if (!pConn->execCommand(query_stream.str())) {
				DEBUG_COMMAND_QUERY_FAILED("EMdFDB::initializeDatabase", query_stream.str());
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}
		}

		// Create table 'object_types'
		{ 
			std::ostringstream query_stream;
			query_stream
				<< 
				"CREATE TABLE object_types (\n"
				<< "    object_type_id INTEGER PRIMARY KEY NOT NULL,\n"
				<<"    object_type_name VARCHAR("
				<< int2string(MAX_IDENTIFIER_CHARS)
				<< ") " 
				<< getMYSQL_BINARY()
				<< " NOT NULL,\n"
				<< "    object_type_flags INT NOT NULL DEFAULT 0,\n"
				<< "    largest_object_length INT NOT NULL DEFAULT 0"
				<< "\n)" 
				<< WITHOUT_OIDS;
			if (!pConn->execCommand(query_stream.str())) {
				DEBUG_COMMAND_QUERY_FAILED("EMdFDB::initializeDatabase", query_stream.str());
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}
		}

		// Create table 'features'
		{
			std::ostringstream query_stream;
			query_stream 
				<< "CREATE TABLE features (\n"
				<< "    object_type_id INT NOT NULL,\n"
				<< "    feature_name VARCHAR("
				<< int2string(MAX_IDENTIFIER_CHARS)
				<< ") " 
				<< getMYSQL_BINARY()
				<< " NOT NULL,\n"
				<< "    feature_type_id INT NOT NULL,\n"
				<< "    default_value "
				<< getSQL_TEXT_TYPE() 
				<< " NOT NULL,\n"
				<< "    computed CHAR(1) NOT NULL DEFAULT 'N',\n"
				<< "    PRIMARY KEY (object_type_id, feature_name)\n"
				<< ")"
				<< WITHOUT_OIDS;
			if (!pConn->execCommand(query_stream.str())) {
				DEBUG_COMMAND_QUERY_FAILED("EMdFDB::initializeDatabase", query_stream.str());
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}
		}

		// Create min_m/max_m table
		if (!createMin_max_m_table()) {
			DEBUG_X_FAILED("EMdFDB::initializeDatabase", "Creating min_m/max_m table");
			if (bDoCommit)
				pConn->abortTransaction();
			return false;
		}

		// Create table 'monad_sets'
		{
			std::ostringstream query_stream;
			query_stream 
				<< "CREATE TABLE monad_sets (\n"
				"   monad_set_id INTEGER PRIMARY KEY NOT NULL,\n"
				"   monad_set_name VARCHAR("
				<< int2string(MAX_IDENTIFIER_CHARS)
				<< ") " 
				<< getMYSQL_BINARY()
				<< " NOT NULL"
				<<"\n)"
				<< WITHOUT_OIDS;
			if (!pConn->execCommand(query_stream.str())) {
				DEBUG_COMMAND_QUERY_FAILED("EMdFDB::initializeDatabase", query_stream.str());
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}
		}

		// Create table 'monad_sets_monads'
		{
			std::ostringstream query_stream;
			query_stream
				<< "CREATE TABLE monad_sets_monads (\n"
				<< "   monad_set_id INT NOT NULL,\n"
				<< "   mse_first INT NOT NULL,\n"
				<< "   mse_last INT NOT NULL,\n"
				<< "   PRIMARY KEY (monad_set_id, mse_first)\n"
				<< ")" 
				<< WITHOUT_OIDS;
			if (!pConn->execCommand(query_stream.str())) {
				DEBUG_COMMAND_QUERY_FAILED("EMdFDB::initializeDatabase", query_stream.str());
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}
		}


		// Commit transaction if necessary
		if (bDoCommit) {
			if (!pConn->commitTransaction()) {
				DEBUG_COMMITTRANSACTION_FAILED("EMdFDB::initializeDatabase");
				return false;
			}
		}

		return true;
	}
}

// Must be called from subclass
/** Use database.
 *
 * Connect to a database.  The user can call this method, or use the
 * MQL statement USE DATABASE.
 *
 * Should be overridden in a subclass, since this doesn't actually
 * connect -- it just initializes the caches.  However, the subclass
 * should remember to call this method.
 *
 * Note that the encryption key is only used for SQLite 3, and only if
 * you have an SQLite 3 that supports encryption.
 *
 * @param db_name the database name to be used.
 * @param key is ignored in EMdFDB.  May be used by derived subclass.
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::useDatabase(const std::string& db_name, const std::string& key)
{
	return this->useDatabase(db_name, key, false);
}

// Must be called from subclass
/** Use database.
 *
 * Connect to a database.  The user can call this method, or use the
 * MQL statement USE DATABASE.
 *
 * Should be overridden in a subclass, since this doesn't actually
 * connect -- it just initializes the caches.  However, the subclass
 * should remember to call this method.
 *
 * Note that the encryption key is only used for SQLite 3, and only if
 * you have an SQLite 3 that supports encryption.
 *
 * @param db_name the database name to be used.
 *
 * @param key is ignored in EMdFDB.  May be used by derived subclass.
 *
 * @param bIsInitializingOrCreating Must be false if we are not
 * creating or initializing the database, true otherwise.
 *
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::useDatabase(const std::string& db_name, const std::string& key, bool bIsInitializingOrCreating)
{
	UNUSED(key);
	
	// Clean out our tables
	m_enum_cache->clear();
	m_enum_const_cache->clear();
	m_feature_cache->clear();
	m_object_type_cache_map.clear();
	m_string_sets_cache->clear();

	// Only get schema version if 
	if (strcmp_nocase(db_name, "emdf") == 0
	    || bIsInitializingOrCreating) {
		// If it is the dummy-database 'emdf', we just use the
		// current version as the default.
		m_schema_version = EMDROS_VERSION_CURRENT;
	} else {
		// Otherwise, we ask the database.
		if (!getSchemaVersion(m_schema_version)) {
			return false;
		}
	}

	return true;
}

/** Drop database.
 *@internal
 *
 * Drops (deletes) a database.
 *
 * @param db_name the database name to be used.
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::dropDatabase(const std::string& db_name)
{
	if (pConn == 0)
		return false;
	else {
		// Switch database context to "emdf"
		// So that we can drop.
		if (!useDatabase("emdf", "")) {
			DEBUG_X_FAILED("EMdFDB::dropDatabase", "useDatabase 'emdf'");
			return false;
		}
    
		// We need to give the server some time to delete the database
		// sleep(2);

		// Get a DBname which we can use
		std::string NewDbName;
		convertDBName(db_name, NewDbName);

		std::ostringstream query_stream;
		query_stream << "DROP DATABASE " << NewDbName;
		if (!pConn->execCommand(query_stream.str())) {
			DEBUG_COMMAND_QUERY_FAILED("EMdFDB::dropDatabase", query_stream.str());
			return false;
		}


		/* Everything was OK. */
		return true;
	}
}

/** Checks whether current DB is initialized.
 *@internal
 *
 * Checks whether current DB has the meta-data necessary for being an
 * Emdros database.
 *
 * @param bIsInitialized Used to return true if it is initialized, 
 *        false otherwise.
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::dbIsInitialized(bool& bIsInitialized)
{
	// Set it to false initially, so this is returned even if 
	// we return false as the return value
	bIsInitialized = false;

	// If pConn isn't initialized, return false
	if (pConn == 0) {
		return false;
	} else {
		// pConn is initialized.

		// Check that table object_types exists
		bool bTableExists;
		if (!tableExists("object_types", bTableExists)) {
			return false;
		}

		// Assume that DB is initialized (with CreateDatbase())
		// if object_types exists
		bIsInitialized = bTableExists;

		// If we got this far, there were no errors
		return true;
	}
}

/** Get current schema version
 *@internal
 *
 * The schema version numbers are defined in emdf.h.
 *
 * @param schema_version Used to return the schema version.
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::getSchemaVersion(/* out */ long& schema_version)
{
	// If pConn isn't initialized, return false
	if (pConn == 0) {
		return false;
	} else {
		// pConn is initialized.

		// SELECT
		{
			std::ostringstream query_stream;
			query_stream 
				<< "SELECT schema_version\n"
				<< "FROM schema_version\n"
				<< "WHERE dummy_id = 0";
			if (!pConn->execSelect(query_stream.str())) {
				DEBUG_SELECT_QUERY_FAILED("EMdFDB::getSchemaVersion", query_stream.str());
				return false;
			} 
		}

		// Access result
		try {
			if (!pConn->accessTuple(0, schema_version)) {
				DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getSchemaVersion");
				// Return failure  
				return false;
			}
		} catch (EMdFNULLValueException e) {
			DEBUG_NULL_VALUE_EXCEPTION("EMdFDB::getSchemaVersion");
			// There was a NULL value exception, hence we return an error
			return false;
		}

		// Release results
		pConn->finalize();

		// If we got this far, there were no errors
		return true;
	}
}


/** Set schema version
 *@internal
 *
 * Set the schema version in the schema_version table. The schema
 * version numbers are defined in emdf.h.
 *
 * @param new_schema_version The schema version to set to.
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::setSchemaVersion(long new_schema_version)
{
	// If pConn isn't initialized, return false
	if (pConn == 0) {
		return false;
	} else {
		// pConn is initialized.

		// UPDATE schema_version table
		std::ostringstream query_stream;
		query_stream 
			<< "UPDATE schema_version\n"
			<< "SET schema_version = " << long2string(new_schema_version) << '\n'
			<< "WHERE dummy_id = 0";
		if (!pConn->execCommand(query_stream.str())) {
			DEBUG_COMMAND_QUERY_FAILED("EMdFDB::setSchemaVersion", query_stream.str());
			return false;
		} 

		// If we got this far, there were no errors
		return true;
	}
}


/** Create schema version table
 *@internal
 *
 * @param initial_schema_version The initial schema version to use.
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::createSchemaVersionTable(long initial_schema_version)
{
	// If pConn isn't initialized, return false
	if (pConn == 0) {
		return false;
	} else {
		// pConn is initialized.

		const char *WITHOUT_OIDS = "";
		if (m_backend_kind == kPostgreSQL) {
			WITHOUT_OIDS = "\nWITHOUT OIDS";
		}

		// Create table 'schema_version'
		{
			std::ostringstream query_stream;
			query_stream
				<< "CREATE TABLE schema_version (\n"
				<< "    dummy_id INTEGER PRIMARY KEY NOT NULL,\n"
				<< "    schema_version INT NOT NULL"
				<< "\n)" 
				<< WITHOUT_OIDS;
			if (!pConn->execCommand(query_stream.str())) {
				DEBUG_COMMAND_QUERY_FAILED("EMdFDB::createSchemaVersionTable", query_stream.str());
				return false;
			}
		}

		// Seed table 'schema_version'
		{
			std::ostringstream query_stream;
			query_stream
				<< "INSERT INTO schema_version (dummy_id, schema_version)\n"
				<< "VALUES ( 0, "
				<< long2string(initial_schema_version)
				<< " )";
			if (!pConn->execCommand(query_stream.str())) {
				DEBUG_COMMAND_QUERY_FAILED("EMdFDB::createSchemaVersionTable", query_stream.str());
				return false;
			}
		}

		// If we got this far, there were no errors
		return true;
	}
}





/** Construct an SQL type from a FeatureInfo.
 *@internal
 *
 * @param OT_Objects_stream The stream to write the SQL type to.
 * @param fi The FeatureInfo to write
 * @param add_not_null Should be true if NOT NULL should be added, 
 *        false otherwise.
 */
void EMdFDB::makeFeatureSQLType(std::ostringstream& OT_Objects_stream, 
				const FeatureInfo& fi,
				bool add_not_null)
{
	if (!fi.getIsComputed()) {
		// Output feature name
		OT_Objects_stream << "    " << encodeFeatureName(fi.getName()) << " ";
    
		// Output feature type
		id_d_t feature_type = fi.getType();
		switch(feature_type & FEATURE_TYPE_TYPE_MASK) {
		case FEATURE_TYPE_INTEGER:
		case FEATURE_TYPE_ID_D:
		case FEATURE_TYPE_ENUM:
			OT_Objects_stream << "INT";
			break;
		case FEATURE_TYPE_LIST_OF_INTEGER:
		case FEATURE_TYPE_LIST_OF_ID_D:
		case FEATURE_TYPE_LIST_OF_ENUM:
			OT_Objects_stream << getSQL_TEXT_TYPE();
			break;
		case FEATURE_TYPE_STRING:
		case FEATURE_TYPE_ASCII:
			if (featureTypeIdIsFromSet(feature_type)) {
				OT_Objects_stream << "INT";
			} else {
				OT_Objects_stream << getSQL_TEXT_TYPE();
			}
			break;
		case FEATURE_TYPE_SET_OF_MONADS:
			OT_Objects_stream << getSQL_TEXT_TYPE();
			break;
		default:
			ASSERT_THROW(false, "Unknown FEATURE_TYPE");
			break;
		}
    
		// Output NOT NULL
		if (add_not_null) {
			OT_Objects_stream << " NOT NULL ";
		}
	}
}

//
/** Make SQL WHERE condition from monad set.
 *@internal
 *
 * Build an SQL string which can be used to constrain a search on
 * OT_objects to a particular set of monads.
 *
 * The string has the form
 * (
 *  (first_monad >= mse1.first() AND last_monad <= mse1.last())
 *   OR 
 *  (first_monad >= mse2.first() AND last_monad <= mse2.last())
 *   OR
 *   ...
 *   OR
 *  (first_monad >= msen.first() AND last_monad <= msen.last())
 * )
 *
 *
 * @param monads the set of monads to use.
 *
 * @param objectRangeType The eObjectRangeType of the object type for
 * whose OT_objects table we are constructing this SQL fragment.
 *
 * @return the string containing the WHERE condition.
 */
std::string EMdFDB::makeMonadConstraintsFromSOM(const SetOfMonads& monads, 
						eObjectRangeType objectRangeType)
{
	std::ostringstream monad_constraints_stream;
	monad_constraints_stream << "(";
	SOMConstIterator ci = monads.const_iterator();
	while (ci.hasNext()) {
		MonadSetElement mse = ci.current();
		if (objectRangeType == kORTSingleMonad) {
			monad_constraints_stream 
				<< "(first_monad>=" 
				<< monad_m2string(mse.first())
				<< " AND first_monad<="
				<< monad_m2string(mse.last()) << ")";
		} else if (objectRangeType == kORTSingleRange) {
			monad_constraints_stream 
				<< "(first_monad>=" 
				<< monad_m2string(mse.first())
				<< " AND last_monad<="
				<< monad_m2string(mse.last()) << ")";
		} else {
			monad_constraints_stream 
				<< "(first_monad>="
				<< monad_m2string(mse.first())
				<< " AND first_monad<="
				<< monad_m2string(mse.last())
				<< " AND "
				<< "last_monad<=" << monad_m2string(mse.last()) << ")";
		}
		ci.next();
		if (ci.hasNext()) {
			monad_constraints_stream 
				<< " OR ";
		}
	}
	monad_constraints_stream << ")";
	return monad_constraints_stream.str();
}

//
// Object type manipulation


/** Create OT_objects table.
 *@internal
 *
 * @param object_type_name The object type name to create.
 * @param objectRangeType Tells whether this is a "multiple range",
 *        "single range", or "single monad" object type.
 * @param FeatureInfos The list of feature infos describing the 
 *        object type's features.
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::createObjectTypeOT_objects(const std::string& object_type_name,
					eObjectRangeType objectRangeType,
					eMonadUniquenessType monadUniquenessType,
					const std::list<FeatureInfo>& FeatureInfos)
{
	const char *WITHOUT_OIDS = "";
	if (m_backend_kind == kPostgreSQL) {
		WITHOUT_OIDS = "\nWITHOUT OIDS";
	}

	// WITH UNIQUE FIRST AND LAST MONADS is equivalent to 
	// WITH UNIQUE FIRST MONADS if we are in a 
	// WITH SINGLE MONAD OBJECTS situation.
	// Since WITH UNIQUE FIRST MONADS is better,
	// we "upgrade" here...
	if (monadUniquenessType == kMUTUniqueFirstAndLastMonads
	    && objectRangeType == kORTSingleMonad) {
		monadUniquenessType = kMUTUniqueFirstMonads;
	}

	bool bPRIMARYKEY_on_object_id_d = true;
	bool bPRIMARYKEY_on_first_monad = false;
	bool bFollowing_PRIMARYKEY_on_fm_lm = false;
	switch (monadUniquenessType) {
	case kMUTNonUniqueMonads:
		bPRIMARYKEY_on_object_id_d = true;
		bPRIMARYKEY_on_first_monad = false;
		bFollowing_PRIMARYKEY_on_fm_lm = false;
		break;
	case kMUTUniqueFirstMonads:
		bPRIMARYKEY_on_object_id_d = false;
		bPRIMARYKEY_on_first_monad = true;
		bFollowing_PRIMARYKEY_on_fm_lm = false;
		break;
	case kMUTUniqueFirstAndLastMonads:
		bPRIMARYKEY_on_object_id_d = false;
		bPRIMARYKEY_on_first_monad = false;
		bFollowing_PRIMARYKEY_on_fm_lm = true;
		break;
	default:
		ASSERT_THROW(false, "Unknown monad uniqueness type");
		break;
	}

	// Normalize object type name
	std::string OTN = normalizeOTName(object_type_name);
	
	// Build query for creating OT_objects, with list of features
	std::ostringstream OT_Objects_stream;

	OT_Objects_stream << "CREATE TABLE " << normalizeTableName(OTN + "_objects", false) << " (\n";
	if (bPRIMARYKEY_on_object_id_d) {
		OT_Objects_stream << "    object_id_d INTEGER PRIMARY KEY NOT NULL,\n";
	} else {
		OT_Objects_stream << "    object_id_d INT NOT NULL,\n";
	}
	if (bPRIMARYKEY_on_first_monad) {
		OT_Objects_stream << "    first_monad INTEGER PRIMARY KEY NOT NULL";
	} else {
		OT_Objects_stream << "    first_monad INT NOT NULL";
	}
	if (objectRangeType != kORTSingleMonad) {
		OT_Objects_stream << ",\n"
				  << "    last_monad INT NOT NULL";
	}
	if (objectRangeType == kORTMultipleRange) {
		OT_Objects_stream << ",\n"
				  << "    monads " << getSQL_TEXT_TYPE() << " NOT NULL";
	}
	std::list<FeatureInfo>::const_iterator f_it(FeatureInfos.begin());
	std::list<FeatureInfo>::const_iterator f_iend(FeatureInfos.end());
	while (f_it != f_iend) {
		// Make newline
		OT_Objects_stream << ",\n";
		
		// Make SQL type
		makeFeatureSQLType(OT_Objects_stream, *f_it, true);

		if (featureTypeIdIsSOM(f_it->getType())) {
			OT_Objects_stream << ",\n    first_monad_" << encodeFeatureName(f_it->getName()) << " INT NOT NULL";
		}
		
		// Advance iterator
		++f_it;
	}
	if (bFollowing_PRIMARYKEY_on_fm_lm) {
		OT_Objects_stream << ",\n    PRIMARY KEY (first_monad, last_monad)";    
		
	}
	OT_Objects_stream << "\n)" << WITHOUT_OIDS;

	// Create OT_Objects
	if (!pConn->execCommand(OT_Objects_stream.str())) {
		DEBUG_COMMAND_QUERY_FAILED("EMdFDB::createObjectTypeOT_objects", OT_Objects_stream.str());
		return false;
	}
	
	// If we got this far, there were no DB errors
	return true;
}

/** Create object type.
 *@internal
 *
 * Create object type in meta-data, as tables, and with indexes.
 *
 * @param object_type_name The object type name to create.
 * @param FeatureInfos The list of feature infos describing the 
 *        object type's features.

 * @param objectRangeType Used to tell whether the objects of this
 * type should be "multiple range objects", "single range objects", or
 * "single monad objects".  This will have an effect on how the
 * OT_objects table is generated, and also on whether the OT_monad_ms
 * table is generated.
 * @param object_type_id Used to return the object type id of the newly 
 *        created object type.
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::createObjectType(const std::string& object_type_name,
			      const std::list<FeatureInfo>& FeatureInfos,
			      eObjectRangeType objectRangeType,
			      eMonadUniquenessType monadUniquenessType,
                              /* out */ id_d_t& object_type_id)
{
	if (pConn == 0)
		return false;
	else {
		// Begin transaction
		bool bDoCommit;
		bDoCommit = pConn->beginTransaction();

		// Generate auto-generated ID
		if (!getNextTypeID(object_type_id)) {
			DEBUG_X_FAILED("EMdFDB::createObjectType", "getNextTypeID");
			if (bDoCommit)
				pConn->abortTransaction();
			return false;
		}

		// Normalize object type name
		std::string OTN = normalizeOTName(object_type_name);

		// Create object_type_flags
		long object_type_flags = 0;
		switch (objectRangeType) {
		case kORTMultipleRange:
			// object_type_flags |= OT_WITH_MULTIPLE_RANGE_OBJECTS;
			break;
		case kORTSingleRange:
			object_type_flags |= OT_WITH_SINGLE_RANGE_OBJECTS;
			break;
		case kORTSingleMonad:
			object_type_flags |= OT_WITH_SINGLE_MONAD_OBJECTS;
			break;
		default:
			ASSERT_THROW(false, "Unknown object range type");
			break;
		}

		long monad_uniqueness_flags = 0;
		switch (monadUniquenessType) {
		case kMUTNonUniqueMonads:
			monad_uniqueness_flags = OT_WITHOUT_UNIQUE_MONADS;
			break;
		case kMUTUniqueFirstMonads:
			monad_uniqueness_flags = OT_HAVING_UNIQUE_FIRST_MONADS;
			break;
		case kMUTUniqueFirstAndLastMonads:
			if (objectRangeType == kORTSingleMonad) {
				// For WITH SINGLE MONAD OBJECTS types,
				// UniqueFirstAndLastMonads is equivalent to
				// UniqueFirstMonads.
				monad_uniqueness_flags = OT_HAVING_UNIQUE_FIRST_MONADS;
			} else {
				monad_uniqueness_flags = OT_HAVING_UNIQUE_FIRST_AND_LAST_MONADS;
			}
			break;
		default:
			ASSERT_THROW(false, "Unknown monad uniqueness type");
			break;
		}

		object_type_flags |= monad_uniqueness_flags;

		// Insert new values 
		{
			std::ostringstream query_stream;
			query_stream 
				<< "INSERT INTO object_types "
				<< "(object_type_id, object_type_name, object_type_flags, largest_object_length)\n"
				<< "VALUES ( " << long2string(object_type_id) << ", "
				<< "'" << OTN  << "', " 
				<< long2string(object_type_flags) << ", "
				<< "0" 
				<< ")\n";
			if (!pConn->execCommand(query_stream.str())) {
				DEBUG_COMMAND_QUERY_FAILED("EMdFDB::createObjectType", query_stream.str());
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}
		}

		// Create OT_Objects
		if (!createObjectTypeOT_objects(object_type_name, 
						objectRangeType, 
						monadUniquenessType,
						FeatureInfos)) {
			DEBUG_X_FAILED("EMdFDB::createObjectType", "EMdFDB::createObjectTypeOT_objects");
			if (bDoCommit)
				pConn->abortTransaction();
			return false;
		}

		// Create features
		std::list<FeatureInfo>::const_iterator f_it(FeatureInfos.begin());
		std::list<FeatureInfo>::const_iterator f_iend(FeatureInfos.end());
		while (f_it != f_iend) {
			if (!createFeature(f_it->getName(),
					   object_type_name,
					   object_type_id,
					   f_it->getType(),
					   f_it->getDefaultValue(),
					   f_it->getIsComputed() != 0)) {
				DEBUG_X_FAILED("EMdFDB::createObjectType", "Creating feature");
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}


			// Advance iterator
			++f_it;
		}

		// Create "self" feature
		if (!createFeature("self",
				   object_type_name,
				   object_type_id,
				   FEATURE_TYPE_ID_D,
				   NIL_AS_STRING,
				   true)) {
			DEBUG_X_FAILED("EMdFDB::createObjectType", "Creating feature 'self'");
			if (bDoCommit)
				pConn->abortTransaction();
			return false;
		}
    
		// Create indices on OT_objects    
		if (!createIndicesOnOTObjects(OTN)) {
			if (bDoCommit)
				pConn->abortTransaction();
			return false;
		}

		if (bDoCommit) {
			if (!pConn->commitTransaction()) {
				DEBUG_COMMITTRANSACTION_FAILED("EMdFDB::createObjectType");
				return false;
			}
		}

		// Add to cache 
		addObjectTypeToCache(object_type_id, OTN, object_type_flags, 0);

		// The command was a success
		return true;
	}
}

/** Add object type to object type cache
 *@internal
 *
 * @param object_type_id the object type id of the object type
 * @param object_type_name the object type name
 * @param object_type_flags the object type flags
 * @param largest_object_length the length (in monads) of the longest object
 */
void EMdFDB::addObjectTypeToCache(id_d_t object_type_id, const std::string& object_type_name, long object_type_flags, monad_m largest_object_length)
{
	// Normalize object type name
	std::string OTN = normalizeOTName(object_type_name);

	// Add to map
	ObjectTypeCacheInfo otci(object_type_id, object_type_flags, largest_object_length);
	m_object_type_cache_map.insert(String2OTCacheInfoMap::value_type(OTN, otci));
}

/** Drop object type.
 *@internal
 *
 * Drop object type from meta-data, as tables, and with indexes.
 *
 * @param object_type_name The object type name to drop.
 * @param object_type_id The object type id of the object type (should 
 *        be the same object type as object_type_name)
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::dropObjectType(const std::string& object_type_name, id_d_t object_type_id)
{
	if (pConn == 0)
		return false;
	else {
		// Begin transaction
		bool bDoCommit;
		bDoCommit = pConn->beginTransaction();

		// Normalize object type name
		std::string OTN = normalizeOTName(object_type_name);

		// Get object range type
		eObjectRangeType objectRangeType;
		if (!getOTObjectRangeType(object_type_name, objectRangeType)) {
			DEBUG_X_FAILED("EMdFDB::dropObjectType", "getting range type of object type " + object_type_name);
			return false;
		}

		// Drop table OT_objects
		{
			std::ostringstream query_stream;
			query_stream << "DROP TABLE " << normalizeTableName(OTN + "_objects", true);
			if (!pConn->execCommand(query_stream.str())) {
				DEBUG_COMMAND_QUERY_FAILED("EMdFDB::dropObjectType", query_stream.str());
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}
		}

		// Delete all features
		{
			std::ostringstream query_stream;
			query_stream
				<< "DELETE FROM features\n"
				<< "WHERE object_type_id = " 
				<< long2string(object_type_id);
			if (!pConn->execCommand(query_stream.str())) {
				DEBUG_COMMAND_QUERY_FAILED("EMdFDB::dropObjectType", query_stream.str());
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}
		}

		// Delete from table 'object_types'
		{
			std::ostringstream query_stream;
			query_stream
				<< "DELETE FROM object_types\n"
				<< "WHERE object_type_id = " 
				<< long2string(object_type_id);
			if (!pConn->execCommand(query_stream.str())) {
				DEBUG_COMMAND_QUERY_FAILED("EMdFDB::dropObjectType", query_stream.str());
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}
		}

		// Commit transaction
		if (bDoCommit) {
			if (!pConn->commitTransaction()) {
				DEBUG_COMMITTRANSACTION_FAILED("EMdFDB::dropObjectType");
				return false;
			}
		}

		// Delete from cache
		deleteObjectTypeFromCache(OTN);

		// If we got this far, the command was a success
		return true;
	}
}

/** Delete object type from cache
 *@internal
 *
 * @param object_type_id The object type id of the object type
 */
void EMdFDB::deleteObjectTypeFromCache(const std::string& object_type_name)
{
	std::string OTN = normalizeOTName(object_type_name);

	// Delete from the map cache
	String2OTCacheInfoMap::iterator mi;
	mi = m_object_type_cache_map.find(OTN);
	if (mi != m_object_type_cache_map.end()) {
		m_object_type_cache_map.erase(mi);
	}
}

/** Get the length of the largest object in object_types for a given OT, from OT_objects.
 *@internal
 *
 * Gets the largest_object_length entry in table object_types, from
 * OT_objects.
 *
 * @param object_type_name The name of the object type to query.
 *
 * @param bGetFromOT_objects true: Get value from OT_objects rather
 * than object_types table.
 *
 * @param largest_object_length The output, in monads.
 *
 * @return True on no DB error, false otherwise.
 */
bool EMdFDB::getLargestObjectLengthFromOT_objects(const std::string& object_type_name,
						  eObjectRangeType objectRangeType,
						  /* out */ monad_m& largest_object_length)
{
	if (pConn == 0)
		return false;
	else {
		// Normalize object type name
		std::string OTN = normalizeOTName(object_type_name);

		// Find out if object type has objects
		bool bHasObjects = false;
		if (!objectTypeHasObjects(object_type_name, bHasObjects)) {
			DEBUG_X_FAILED("EMdFDB::getLargestObjectLengthFromOT_objects", "finding out whether object type had objects");
			return false;
		}

		// If there are none, then there is no point in going further
		if (!bHasObjects) {
			largest_object_length = 0;
			return true;
		}

		// SELECT
		{
			if (objectRangeType == kORTSingleMonad) {
				largest_object_length = 1;
			} else {
				std::ostringstream query_stream;
				query_stream 
					<< "SELECT MAX(last_monad - first_monad + 1)\n"
					<< "FROM " << normalizeTableName(OTN + "_objects", true);
				if (!pConn->execSelect(query_stream.str())) {
					DEBUG_SELECT_QUERY_FAILED("EMdFDB::getLargestObjectLengthFromOT_objects", query_stream.str());
					return false;
				} 

				// Access result
				try {
					if (!pConn->accessTuple(0, largest_object_length)) {
						DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getLargestObjectLengthFromOT_objects");
						// Return failure  
						return false;
					}
				} catch (EMdFNULLValueException e) {
					DEBUG_NULL_VALUE_EXCEPTION("EMdFDB::getLargestObjectLengthFromOT_objects");
					// There was a NULL value exception, hence we return an error
					return false;
				}
					
				// Release results
				pConn->finalize();
			}
		}
		
		// If we got this far, there were no errors
		return true;
	}
}

/** Get the length of the largest object in object_types for a given OT.
 *@internal
 *
 * Gets the largest_object_length entry in table object_types.
 *
 * @param object_type_name The name of the object type to query.
 *
 * @param bGetFromOT_objects true: Get value from OT_objects rather
 * than object_types table.
 *
 * @param largest_object_length The output, in monads.
 *
 * @return True on no DB error, false otherwise.
 */
bool EMdFDB::getLargestObjectLength(const std::string& object_type_name,
				    eObjectRangeType objectRangeType,
				    /* out */ monad_m& largest_object_length)
{
	if (pConn == 0)
		return false;
	else {
		// Normalize object type name
		std::string OTN = normalizeOTName(object_type_name);

		// Try the cache first
		String2OTCacheInfoMap::const_iterator ci;
		ci = m_object_type_cache_map.find(OTN);
		if (ci != m_object_type_cache_map.end()) {
			largest_object_length = ci->second.m_largest_object_length;
			return true;
		}

		// Assume it was OK
		bool bFirstSelectWasOK = true;
		if (m_schema_version < EMDROS_VERSION_1_2_0_PRE_208) {
			bFirstSelectWasOK = false;
		} else {
			std::ostringstream query_stream;
			query_stream 
				<< "SELECT largest_object_length\n"
				<< "FROM object_types\n"
				<< "WHERE object_type_name = '" << OTN << "'";
			if (!pConn->execSelect(query_stream.str())) {
				DEBUG_SELECT_QUERY_FAILED("EMdFDB::getLargestObjectLength", query_stream.str());
				bFirstSelectWasOK = false;
			} 
		}
		
		if (bFirstSelectWasOK) {
			// Access result
			try {
				if (!pConn->accessTuple(0, largest_object_length)) {
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getLargestObjectLength");
					// Return failure  
					return false;
				}
			} catch (EMdFNULLValueException e) {
				DEBUG_NULL_VALUE_EXCEPTION("EMdFDB::getLargestObjectLength");
				// There was a NULL value exception, hence we return an error
				return false;
			}
			
			// Release results
			pConn->finalize();
		} else {
			// Release results
			pConn->finalize();

			if (!getLargestObjectLengthFromOT_objects(OTN,
								  objectRangeType,
								  largest_object_length)) {
				return false;
			}
		}
		
		// If we got this far, there were no errors
		return true;
	}
}

/** Set the length of the largest object in object_types for a given OT.
 *@internal
 *
 * Sets the largest_object_length entry in table object_types.
 *
 * Only sets it if the largest object is less...
 *
 * @param object_type_name The name of the object type to update.
 *
 * @param new_largest_object_length.
 *
 * @return True on no DB error, false otherwise.
 */
bool EMdFDB::setLargestObjectLengthIfNecessary(const std::string& object_type_name,
					       monad_m new_largest_object_length,
					       eObjectRangeType objectRangeType,
					       bool bSetUnconditionally)
{
	if (pConn == 0)
		return false;
	else {
		// Begin transaction
		bool bDoCommit;
		bDoCommit = pConn->beginTransaction();

		monad_m current_largest = 0;
		if (!bSetUnconditionally) {
			if (!getLargestObjectLength(object_type_name, 
						    objectRangeType,
						    current_largest)) {
				
				DEBUG_X_FAILED("EMdFDB::setLargestObjectLength", "getting largest object length");
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}
			
			// Perhaps don't need to do anything...
			if (current_largest >= new_largest_object_length) {
				if (bDoCommit)
					pConn->commitTransaction();
				return true;
			}
		}


		// Normalize object type name
		std::string OTN = normalizeOTName(object_type_name);

		if (m_schema_version >= EMDROS_VERSION_1_2_0_PRE_208) {
			std::ostringstream query_stream;
			query_stream << "UPDATE object_types\n"
				     << "SET largest_object_length = " 
				     << long2string(new_largest_object_length) << '\n'
				     << "WHERE object_type_name = '" << OTN << "'";
			if (!pConn->execCommand(query_stream.str())) {
				DEBUG_COMMAND_QUERY_FAILED("EMdFDB::setLargestObjectLength", query_stream.str());
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}
		}
			
		// Commit transaction if necessary
		if (bDoCommit) {
			if (!pConn->commitTransaction()) {
				DEBUG_COMMITTRANSACTION_FAILED("EMdFDB::setLargestObjectLength");
				return false;
			}
		}

		// Update cache if OT in the cache...
		String2OTCacheInfoMap::iterator i;
		i = m_object_type_cache_map.find(OTN);
		if (i != m_object_type_cache_map.end()) {
			i->second.m_largest_object_length = new_largest_object_length;
		}

		
		// No DB error...
		return true;
	}
}





/** Check whether object type exists, using object type id
 *@internal
 *
 * @param object_type_id The object type id of the object type
 * @param result Used to return true if it is there, false if it is not.
 * @param object_type_name Used to return the name of the object type.
 *        Only valid if \p result is true.
 * @param objectRangeType Tells whether this is a "multiple range",
 *        "single range", or "single monad" object type.
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::objectTypeExists(id_d_t object_type_id,
			      /* out */ bool& result, 
			      /* out */ std::string& object_type_name,
			      /* out */ eObjectRangeType& objectRangeType,
			      /* out */ eMonadUniquenessType& monadUniquenessType)
{
	if (pConn == 0)
		return false;
	else {
		long object_type_flags;

		// First try the map cache
		String2OTCacheInfoMap::const_iterator mi, mend;
		mi = m_object_type_cache_map.begin();
		mend = m_object_type_cache_map.end();
		while (mi != mend) {
			id_d_t OT_id = mi->second.m_object_type_id;
			if (OT_id == object_type_id) {
				object_type_name = mi->first;
				object_type_flags = mi->second.m_object_type_flags;
				objectRangeType = get_object_range_type_from_OT_flags(object_type_flags);
				monadUniquenessType = get_monad_uniqueness_type_from_OT_flags(object_type_flags);
				result = true;
				return true;
			}
			++mi;
		}

		// The "largest_object_length" column was added in 
		// 1.2.0.pre208.
		bool bGetLargestObjectLengthFromDB = 
			m_schema_version >= EMDROS_VERSION_1_2_0_PRE_208;

		// Build and execute query
		{
			std::ostringstream query_stream;
			query_stream
				<< "SELECT object_type_name, object_type_flags"
				<< (bGetLargestObjectLengthFromDB ? ", largest_object_length" : "")
				<< "\n"
				<< "FROM object_types\n"
				<< "WHERE object_type_id = "
				<< long2string(object_type_id);
			if (!pConn->execSelect(query_stream.str())) {
				DEBUG_SELECT_QUERY_FAILED("EMdFDB::objectTypeExists", query_stream.str());
				return false;
			}
		}

		// Is there at least one tuple? (There should be either 1 or none)
		monad_m largest_object_length = 0;
		if (pConn->hasRow()) {
			try {
				// Access object type name result
				if (!pConn->accessTuple(0, object_type_name)) {
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::objectTypeExists");
					return false;
				}
				// Access object type name result
				if (!pConn->accessTuple(1, object_type_flags)) {
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::objectTypeExists");
					return false;
				}

				// Access largest object length
				if (bGetLargestObjectLengthFromDB) {
					if (!pConn->accessTuple(2, largest_object_length)) {
						DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::objectTypeExists");
						return false;
					}
				}
			} catch (EMdFNULLValueException e) {
				DEBUG_NULL_VALUE_EXCEPTION("EMdFDB::objectTypeExists");
				// There was a NULL value exception, hence we return an error
				return false;
			}
      
			// We did find the object type
			result = true;
		} else { 

			// Release results
			pConn->finalize();

			// We did not find the object type
			result = false;
		}

		// Release results
		pConn->finalize();

		// Add to cache if we found it
		if (result) {
			// Set output
			objectRangeType = get_object_range_type_from_OT_flags(object_type_flags);
			monadUniquenessType = get_monad_uniqueness_type_from_OT_flags(object_type_flags);
			bool bAddToCache = true;
			if (bGetLargestObjectLengthFromDB) {
				; // We have already gotten largest_object_length
				// from DB. No need to get it again.
			} else {
				bAddToCache = getLargestObjectLength(object_type_name, 
								     objectRangeType,
								     largest_object_length);
			}
			if (bAddToCache) {
				addObjectTypeToCache(object_type_id, object_type_name, object_type_flags, largest_object_length);
			}
		}

		// If we got this far, then the command as a whole was a success, 
		// no matter whether we found the object type or not.
		return true;
	}
}

/** Check whether object type exists, using object type name
 *@internal
 *
 * @param object_type_name The object type name of the object type to find.
 * @param result Used to return true if it is there, false if it is not.
 * @param object_type_id Used to return the object type id.  Only valid
 *        if \p result is true.
 * @param objectRangeType Tells whether this is a "multiple range",
 *        "single range", or "single monad" object type.
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::objectTypeExists(const std::string& object_type_name, 
                              /* out */ bool& result, 
                              /* out */ id_d_t& object_type_id,
			      /* out */ eObjectRangeType& objectRangeType,
			      /* out */ eMonadUniquenessType& monadUniquenessType) 
{
	if (pConn == 0)
		return false;
	else {
		long object_type_flags;

		// Normalize object type name
		std::string OTN = normalizeOTName(object_type_name);

		// First try the cache
		String2OTCacheInfoMap::const_iterator ci;
		ci = m_object_type_cache_map.find(OTN);
		if (ci != m_object_type_cache_map.end()) {
			object_type_id = ci->second.m_object_type_id;
			object_type_flags = ci->second.m_object_type_flags;
			objectRangeType = get_object_range_type_from_OT_flags(object_type_flags);
			monadUniquenessType = get_monad_uniqueness_type_from_OT_flags(object_type_flags);
			result = true;
			return true;
		}

		// The "largest_object_length" column was added in 
		// 1.2.0.pre208.
		bool bGetLargestObjectLengthFromDB = 
			m_schema_version >= EMDROS_VERSION_1_2_0_PRE_208;

		// Build and execute query
		std::ostringstream query_stream;
		query_stream << "SELECT object_type_id, object_type_flags";
		if (bGetLargestObjectLengthFromDB) {
			query_stream << ", largest_object_length";
		}
		query_stream << "\n"
			     << "FROM object_types\n"
			     << "WHERE object_type_name = '" << OTN << "'";
		if (!pConn->execSelect(query_stream.str())) {
			DEBUG_SELECT_QUERY_FAILED("EMdFDB::objectTypeExists", query_stream.str());
			return false;
		}

		monad_m largest_object_length = 0;

		// Is there at least one tuple? (There should be either 1 or none)
		if (pConn->hasRow()) {
			// Access result
			try {
				if (!pConn->accessTuple(0, object_type_id)) {
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::objectTypeExists");
					return false;
				}
				if (!pConn->accessTuple(1, object_type_flags)) {
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::objectTypeExists");
					return false;
				}

				if (bGetLargestObjectLengthFromDB) {
					if (!pConn->accessTuple(2, largest_object_length)) {
						DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::objectTypeExists");
						return false;
					}
				}

			} catch (EMdFNULLValueException e) {
				DEBUG_NULL_VALUE_EXCEPTION("EMdFDB::objectTypeExists");

				// There was a NULL value exception, hence we return an error
				return false;
			}
 
			// We did find the object type
			result = true;
		} else { 
			// We did not find the object type
			result = false;
		}

		// Release results
		pConn->finalize();

		// Add results to cache if found
		if (result) {
			// Set output
			objectRangeType = get_object_range_type_from_OT_flags(object_type_flags);
			monadUniquenessType = get_monad_uniqueness_type_from_OT_flags(object_type_flags);

			bool bAddToCache = true;
			if (bGetLargestObjectLengthFromDB) {
				; // We have already gotten largest_object_length
				// from DB. No need to get it again.
			} else {
				bAddToCache = getLargestObjectLength(OTN, 
								     objectRangeType,
								     largest_object_length);
			}
			if (bAddToCache) {
				addObjectTypeToCache(object_type_id, OTN, object_type_flags, largest_object_length);
			}
		}

		// If we got this far, then the command as a whole was a success, 
		// no matter whether we found the object type or not.
		return true;
	}
}


/** Create enumeration.
 *@internal
 *
 * Used to create an empty enumeration with no enum constants.
 *
 * @param enum_name The name of the enumeration to create.
 * @param enum_id Used to return the id_d of the newly created enumeration.
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::createEnum(const std::string& enum_name, 
                        /* out */ id_d_t& enum_id)
{
	if (pConn == 0)
		return false;
	else {
		// Begin transaction
		bool bDoCommit;
		bDoCommit = pConn->beginTransaction();

		// Get auto-generated ID
		if (!getNextTypeID(enum_id)) {
			DEBUG_X_FAILED("EMdFDB::createEnum", "getNextTypeID");
			if (bDoCommit)
				pConn->abortTransaction();
			return false;
		}

		// Build and execute query
		std::ostringstream query_stream;
		query_stream 
			<< "INSERT INTO enumerations (enum_id, enum_name)\n"
			<< "VALUES ( " 
			<< long2string(enum_id) 
			<< ", '" 
			<< enum_name
			<< "' )";
		if (!pConn->execCommand(query_stream.str())) {
			DEBUG_COMMAND_QUERY_FAILED("EMdFDB::createEnum", query_stream.str());
			if (bDoCommit)
				pConn->abortTransaction();
			return false;
		}

		// Commit transaction
		if (bDoCommit) {
			if (!pConn->commitTransaction()) {
				DEBUG_COMMITTRANSACTION_FAILED("EMdFDB::createEnum");
				return false;
			}
		}

		// Add to cache
		addEnumToCache(enum_id, std::string(enum_name));

		// If we got this far, the command succeeded.
		return true;
	}
}

/** Add enumeration to cache
 *@internal
 *
 * @param enum_id The enumeration id_d.
 * @param enum_name The name of the enumeration.
 */
void EMdFDB::addEnumToCache(id_d_t enum_id, const std::string& enum_name)
{
	m_enum_cache->startNewRow();
	m_enum_cache->append(id_d2string(enum_id));
	m_enum_cache->append(enum_name);
}

/** Drop enumeration.
 *@internal
 *
 * Drop an enumeration and all its constants, and remove them from
 * local caches.
 *
 * @param enum_id The id_d of the enum to drop.
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::dropEnum(id_d_t enum_id)
{
	if (pConn == 0)
		return false;
	else {
		// Strip the lower bits off that aren't part of the enum_id
		enum_id = STRIP_ENUM_ID_OF_LOWER_BITS(enum_id);

		// Begin transaction
		bool bDoCommit;
		bDoCommit = pConn->beginTransaction();

		// Build and execute query
		std::ostringstream query_stream1;
		query_stream1
			<< "DELETE\n"
			<< "FROM enumeration_constants\n"
			<< "WHERE enum_id = " << long2string(enum_id);
		if (!pConn->execCommand(query_stream1.str())) {
			DEBUG_COMMAND_QUERY_FAILED("EMdFDB::dropEnum", query_stream1.str());
			if (bDoCommit)
				pConn->abortTransaction();
			return false;
		}

		// Build and execute query
		std::ostringstream query_stream2;
		query_stream2 
			<< "DELETE\n"
			<< "FROM enumerations\n"
			<< "WHERE enum_id = " << long2string(enum_id);
		if (!pConn->execCommand(query_stream2.str())) {
			DEBUG_COMMAND_QUERY_FAILED("EMdFDB::dropEnum", query_stream2.str());
			if (bDoCommit)
				pConn->abortTransaction();
			return false;
		}

		// Commit transaction
		if (bDoCommit) {
			if (!pConn->commitTransaction()) {
				DEBUG_COMMITTRANSACTION_FAILED("EMdFDB::dropEnum");
				return false;
			}
		}

		// Delete from enum cache
		TableIterator i;
		i = m_enum_cache->find(id_d2string(enum_id), 1);
		if (i.hasNext()) {
			m_enum_cache->erase(i);
		}

		// Delete from enum const cache
		m_enum_const_cache->dropEnum(enum_id);

		// If we got this far, the command succeeded.
		return true;
	}
}

/** Check whether enumeration exists, based on id_d
 *@internal
 *
 * @param enum_id The id_d of the enum to find.
 * @param result Used to return true if it is there, false if it is not.  
 * @param enum_name Used to return the name of the enumeration.  Only valid
 *        if \p result is true.
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::enumExists(id_d_t enum_id,
			/* out */ bool& result,
			/* out */ std::string& enum_name)
{
	if (pConn == 0)
		return false;
	else {
		// Strip the lower bits off that aren't part of the enum_id
		enum_id = STRIP_ENUM_ID_OF_LOWER_BITS(enum_id);

		// First try the cache
		TableIterator i;
		i = m_enum_cache->find(id_d2string(enum_id), 1);
		if (i.hasNext()) {
			enum_name = m_enum_cache->getColumn(i, 2);
			result = true;
			return true;
		}

		// Build and execute query
		std::ostringstream query_stream;
		query_stream 
			<< "SELECT enum_name\n"
			<< "FROM enumerations\n"
			<< "WHERE enum_id = " << long2string(enum_id);
		if (!pConn->execSelect(query_stream.str())) {
			DEBUG_SELECT_QUERY_FAILED("EMdFDB::enumExists", query_stream.str());
			return false;
		}

		// Is there at least one tuple? (There should be either 1 or none)
		if (pConn->hasRow()) {
			// Access result
			try {
				if (!pConn->accessTuple(0, enum_name)) {
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::enumExists");
					return false;
				}
			} catch (EMdFNULLValueException e) {
				DEBUG_NULL_VALUE_EXCEPTION("EMdFDB::enumExists");
				// There was a NULL value exception, hence we return an error
				return false;
			}

      
			// We did find the enumeration
			result = true;
		} else { 
			// We did not find the enumeration
			result = false;
		}

		// Add results to cache if found
		if (result) {
			addEnumToCache(enum_id, std::string(enum_name));
		}

		// Release results
		pConn->finalize();

		// If we got this far, then the command as a whole was a success, 
		// no matter whether we found the enum or not.
		return true;
	}
}

/** Check whether enumeration exists, based on enumeration name
 *@internal
 *
 * @param enum_name The name of the enum to find.
 * @param result Used to return true if it is there, false if it is not.
 * @param enum_id Used to return the id_d of the enumeration.  Only valid 
 *        if \p result is true.
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::enumExists(const std::string& enum_name, 
                        /* out */ bool& result,
                        /* out */ id_d_t& enum_id)
{
	if (pConn == 0)
		return false;
	else {
		// First try the cache
		TableIterator i;
		i = m_enum_cache->find(std::string(enum_name), 2);
		if (i.hasNext()) {
			enum_id = string2id_d(m_enum_cache->getColumn(i, 1));
			result = true;
			return true;
		}

		// Build and execute query
		std::ostringstream query_stream;
		query_stream << "SELECT enum_id\n"
			     << "FROM enumerations\n"
			     << "WHERE enum_name = '" << enum_name << "'";
		if (!pConn->execSelect(query_stream.str())) {
			DEBUG_SELECT_QUERY_FAILED("EMdFDB::enumExists", query_stream.str());
			return false;
		}

		// Is there at least one tuple? (There should be either 1 or none)
		if (pConn->hasRow()) {
			// Access result
			try {
				if (!pConn->accessTuple(0, enum_id)) {
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::enumExists");
					return false;
				}
			} catch (EMdFNULLValueException e) {
				DEBUG_NULL_VALUE_EXCEPTION("EMdFDB::enumExists");
				// There was a NULL value exception, hence we return an error
				return false;
			}

      
			// We did find the enumeration
			result = true;
		} else { 
			// We did not find the enumeration
			result = false;
		}

		// Add results to cache if found
		if (result) {
			addEnumToCache(enum_id, std::string(enum_name));
		}

		// Release results
		pConn->finalize();

		// If we got this far, then the command as a whole was a success, 
		// no matter whether we found the enum or not.
		return true;
	}
}

/** Get the default constant in an enumeration.
 *@internal
 *
 * @param enum_id The id_d of the enumeration.
 * @param result Used to return true if it is there, false if it is not.
 * @param default_constant_name Used to return the name of the default
 *        constant.  Only valid if \p result is true.
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::getDefault(id_d_t enum_id, 
                        /* out */ bool& result,
			/* out */ std::string& default_constant_name)
{
	if (pConn == 0)
		return false;
	else {
		// Strip the lower bits off that aren't part of the enum_id
		enum_id = STRIP_ENUM_ID_OF_LOWER_BITS(enum_id);

		// First try cache
		const EnumConstInfo *pECI = m_enum_const_cache->getDefault(enum_id);
		if (pECI != 0) {
			default_constant_name = pECI->getName();
			result = true;
			return true;
		}

		// Build and execute query
		std::ostringstream query_stream;
		query_stream
			<< "SELECT enum_value_name\n"
			<< "FROM enumeration_constants\n"
			<< "WHERE enum_id = " << long2string(enum_id) << "\n"
			<< "      AND is_default = 'Y'";
		if (!pConn->execSelect(query_stream.str())) {
			DEBUG_SELECT_QUERY_FAILED("EMdFDB::getDefault", query_stream.str());
			return false;
		}

		// Is there at least one tuple? (There should be either 1 or none)
		if (pConn->hasRow()) {
			// Access result
			try {
				if (!pConn->accessTuple(0, default_constant_name)) {
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getDefault");
					return false;
				}
			} catch (EMdFNULLValueException e) {
				DEBUG_NULL_VALUE_EXCEPTION("EMdFDB::getDefault");
				// There was a NULL value exception, hence we return an error
				return false;
			}

      
			// We did find the default
			result = true;
		} else { 
			// We did not find the default
			result = false;
		}

		// Release results
		pConn->finalize();

		// If we got this far, then the command as a whole was a success, 
		// no matter whether we found the default or not.
		return true;
	}
}


/** Create an enum constant.
 *@internal
 *
 * Create an enumeration constant and add it to the local cache.
 *
 * @param enum_id The id_d of the enumeration.
 * @param enum_name The name of the enumeration.  Must be the same 
 *        enumeration as enum_id.
 * @param ec_info an EnumConstInfo describing the new constant.
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::createEnumConstant(id_d_t enum_id,
				const std::string& enum_name,
				const EnumConstInfo& ec_info)
{
	if (pConn == 0)
		return false;
	else {
		// Strip the lower bits off that aren't part of the enum_id
		enum_id = STRIP_ENUM_ID_OF_LOWER_BITS(enum_id);

		// Build and execute query
		std::ostringstream query_stream;
		query_stream
			<< "INSERT INTO enumeration_constants (\n"
			<< "    enum_id,\n"
			<< "    enum_value_name,\n"
			<< "    value,\n"
			<< "    is_default\n"
			<< ")\n"
			<< "VALUES (\n"
			<< "    "  << long2string(enum_id)
			<< ", '" << ec_info.getName()
			<< "', " << long2string(ec_info.getValue())
			<< ", '" << bool2char(ec_info.getIsDefault()) << "'\n"
			<< ")";
		if (!pConn->execCommand(query_stream.str())) {
			DEBUG_COMMAND_QUERY_FAILED("EMdFDB::CreateEnumConst", query_stream.str());
			return false;
		}

		// Add to cache
		if (!addEnumConstToCache(enum_id, enum_name, ec_info.getName(), ec_info.getValue(), ec_info.getIsDefault()))
			return false;

		// If we got this far, the command succeeded.
		return true;
	}
}


/** Add enum constant to cache.
 *@internal
 *
 * @param enum_id The id_d of the enumeration.
 * @param enum_name The name of the enumeration.  Must be the same 
 *        enumeration as enum_id.
 * @param enum_const_name The name of the enumeration constant.
 * @param value The integer value of the constant.
 * @param is_default Must be true if the constant is to be the default 
 *        constant.
 * @return True on constant added, false on not added.
 *         Note that false does not mean error.  It may have been there 
 *         already.
 */
bool EMdFDB::addEnumConstToCache(id_d_t enum_id, const std::string& enum_name, const std::string& enum_const_name, long value, bool is_default)
{
	// Strip the lower bits off that aren't part of the enum_id
	enum_id = STRIP_ENUM_ID_OF_LOWER_BITS(enum_id);

	EnumConstInfo eci(enum_const_name, value, is_default);
	return m_enum_const_cache->addEci(enum_id, enum_name, eci);
}

/** Check whether enum const exists, by enum const name
 *@internal
 *
 * @param enum_const_name The name of the enum const to find.
 * @param enum_id The id_d of the enumeration.
 * @param result Used to return true if it is there, false if it is not.
 * @param value Used to return the value of the enum const if there.
 *        Only valid if \p result is true.
 * @param is_default Used to return whether the enum const is the default
 *         (true means it is). Only valid if \p result is true.
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::enumConstExists(const std::string& enum_const_name, 
			     id_d_t enum_id, 
                             /* out */ bool& result,
                             /* out */ long& value,
                             /* out */ bool& is_default)
{
	if (pConn == 0)
		return false;
	else {
		// Strip the lower bits off that aren't part of the enum_id
		enum_id = STRIP_ENUM_ID_OF_LOWER_BITS(enum_id);

		result = false; // Assume it isn't there
		// First try the cache
		const EnumConstInfo *pECI = m_enum_const_cache->find(enum_id, enum_const_name);
		if (pECI != 0) {
			value = pECI->getValue();
			is_default = pECI->getIsDefault();
			result = true;
			return true;
		}

		// Build and execute query
		std::ostringstream query_stream;
		query_stream << "SELECT enum_value_name, value, is_default\n"
			     << "FROM enumeration_constants\n"
			     << "WHERE enum_id = " << long2string(enum_id)
			     << " AND enum_value_name = " 
			     << getMYSQL_BINARY() << "'" << enum_const_name << "'\n";
		if (!pConn->execSelect(query_stream.str())) {
			DEBUG_SELECT_QUERY_FAILED("EMdFDB::enumConstExists", query_stream.str());
			return false;
		}

		std::string enum_const_name_from_db;
		// If there are no rows, we did not find it.
		// However, if there is at least one row, we need to iterate
		// over them.  This is because MySQL will return all that have 
		// the same enum_value_name case-insensitively.
		bool bMoreRows = pConn->hasRow();
		if (!bMoreRows) {
			// We did not find the enumeration constant
			result = false;
		} else {
			while (bMoreRows && !result) {
				try {
					// Get constant name
					if (!pConn->accessTuple(0, enum_const_name_from_db)) {
						DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::enumConstExists");
						return false;
					}

					// Get value
					if (!pConn->accessTuple(1, value)) {
						DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::enumConstExists");
						return false;
					}
	
					// Get is_default
					if (!pConn->accessTuple(2, is_default)) {
						DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::enumConstExists");
						return false;
					}
				} catch (EMdFNULLValueException e) {
					DEBUG_NULL_VALUE_EXCEPTION("EMdFDB::enumConstExists");
					// There was a NULL value exception, hence we return an error
					return false;
				}

				// Check whether we have really found it.
				// Do this case-sensitively.
				if (strcmp_case(enum_const_name_from_db, enum_const_name) == 0) {
					// We did find the enumeration constant
					result = true;
				}

				// Get next tuple
				if (!pConn->getNextTuple(bMoreRows)) {
					DEBUG_GET_NEXT_TUPLE_FAILED("EMdFDB::enumConstExists");
					return false;
				}
			}
		} 

		// Release results
		pConn->finalize();

		// Add to cache if found
		if (result) {
			// Get the enum_name
			std::string enum_name;
			bool bEnumExists;
			if (!enumExists(enum_id, bEnumExists, enum_name)) {
				DEBUG_X_FAILED("EMdFDB::enumConstExists", "Getting name of enum using enumExists");
				return false;
			}
			if (!bEnumExists) {
				DEBUG_X_IS_WRONG("EMdFDB::enumConstExists", "Enum with id_d " + id_d2string(enum_id) + " did not exist.");
				return false;
			}

			// Add it to the enum_const_cache
			if (!addEnumConstToCache(enum_id, enum_name, enum_const_name, value, is_default))
				return false;
		}

		// If we got this far, then the command as a whole was a success, 
		// no matter whether we found the enumeration constant or not.
		return true;
	}
}



// Returns the string-representation of an enumeration constant in 
// enum enum_name with the value value.  If more than one enum constant
// has the same value, it is undefined which of them is returned.
/** Gets enum const name from value.
 *
 * Gets the enumeration constant corresponding to a given value
 * within a specific enumeration.
 *
 * @param value the value whose enum constant we will find.
 * @param enum_name the name of the enumeration within which we wish to 
 *        find the enumeration constant.
 * @param enum_const_name Used to return the name of the enumeration
 *        constant if found.
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::getEnumConstNameFromValue(long value,
				       const std::string& enum_name,
				       /* out */ std::string& enum_const_name)
  
{
	if (pConn == 0)
		return false;
	else {
		bool bExists = false;
      
		// First try the cache
		const EnumConstInfo* pECI = m_enum_const_cache->find(enum_name, value);
		if (pECI != 0) {
			enum_const_name = pECI->getName();
			return true;
		}

		// Get enum_id
		id_d_t enum_id;
		bool bEnumExists;
		if (!enumExists(enum_name, bEnumExists, enum_id)) {
			DEBUG_X_IS_WRONG("EMdFDB::getEnumConstNameFromValue", "enumExists returned with DB error.");
			return false;
		}
		if (!bEnumExists) {
			DEBUG_X_IS_WRONG("EMdFDB::getEnumConstNameFromValue", "enumeration did not exist");
			return false;
		}
      
		// Build and execute query
		std::ostringstream query_stream;
		query_stream << "SELECT enum_value_name, is_default\n"
			     << "FROM enumeration_constants\n"
			     << "WHERE enum_id = " << long2string(enum_id)
			     << " AND value = " << value << "\n";
		if (!pConn->execSelect(query_stream.str())) {
			DEBUG_SELECT_QUERY_FAILED("EMdFDB::getEnumConstNameFromValue", query_stream.str());
			return false;
		}
      
		// Is there at least one tuple? (There may be none, 1, or more)
		bool is_default;
		if (pConn->hasRow()) {
			try {
				// Get value
				if (!pConn->accessTuple(0, enum_const_name)) {
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getEnumConstNameFromValue");
					return false;
				}
	
				// Get is_default
				if (!pConn->accessTuple(1, is_default)) {
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::enumConstExists");
					return false;
				}
			} catch (EMdFNULLValueException e) {
				DEBUG_NULL_VALUE_EXCEPTION("EMdFDB::enumConstExists");
				// There was a NULL value exception, hence we return an error
				return false;
			}
	
			// So it existed
			bExists = true;
		} else { 
			bExists = false;
		}
      
		// Release results
		pConn->finalize();

		if (bExists) {
			if (!addEnumConstToCache(enum_id, enum_name, enum_const_name, value, is_default))

				return false;
		} else {
			DEBUG_X_IS_WRONG("EMdFDB::getEnumConstNameFromValue", "enumeration constant with value " + long2string(value) + " did not exist");
			return false;
		}
      
		// If we got this far, then the command as a whole was a success, 
		// no matter whether we found the enumeration constant or not.
		return true;
	}
}


// Only gets one of them if more than one
// TODO: Fix this
/** Check whether enum const exists, by value
 *@internal
 *
 * @param value The value of the enum const to find.
 * @param enum_id The id_d of the enumeration.
 * @param bExists Used to return true if it is there, false if it is not.
 * @param enum_const_name Used to return the name of the enum const if there.
 *        Only valid if \p result is true.
 * @param is_default Used to return whether the enum const is the default
 *         (true means it is). Only valid if \p result is true.
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::enumConstExists(long value,
			     id_d_t enum_id,
			     bool& bExists,
			     /* out */ std::string& enum_const_name,
			     /* out */ bool& is_default)

{
	if (pConn == 0)
		return false;
	else {
		// Strip the lower bits off that aren't part of the enum_id
		enum_id = STRIP_ENUM_ID_OF_LOWER_BITS(enum_id);

		// Assume it isn't there
		bExists = false;

		// First try the cache
		const EnumConstInfo* pECI = m_enum_const_cache->find(enum_id, value);
		if (pECI != 0) {
			enum_const_name = pECI->getName();
			bExists = true;
			return true;
		}

		// Build and execute query
		std::ostringstream query_stream;
		query_stream << "SELECT enum_value_name, is_default\n"
			     << "FROM enumeration_constants\n"
			     << "WHERE enum_id = " << long2string(enum_id)
			     << " AND value = " << long2string(value) << "\n";
		if (!pConn->execSelect(query_stream.str())) {
			DEBUG_SELECT_QUERY_FAILED("EMdFDB::enumConstExists", query_stream.str());
			return false;
		}

		// Is there at least one tuple? (There may be none, 1, or more)
		if (pConn->hasRow()) {
			try {
				// Get value
				if (!pConn->accessTuple(0, enum_const_name)) {
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::enumConstExists");
					return false;
				}
	
				// Get is_default
				if (!pConn->accessTuple(1, is_default)) {
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::enumConstExists");
					return false;
				}

			} catch (EMdFNULLValueException e) {
				DEBUG_NULL_VALUE_EXCEPTION("EMdFDB::enumConstExists");
				// There was a NULL value exception, hence we return an error
				return false;
			}

			// We did find the enumeration constant
			bExists = true;
		} else { 
			// We did not find the enumeration constant
			bExists = false;
		}

		// Release results
		pConn->finalize();

		// Add to cache if found
		if (bExists) {
			// Get the enum_name
			std::string enum_name;
			bool bEnumExists;
			if (!enumExists(enum_id, bEnumExists, enum_name)) {
				DEBUG_X_FAILED("EMdFDB::enumConstExists", "Getting name of enum using enumExists");
				return false;
			}
			if (!bEnumExists) {
				DEBUG_X_IS_WRONG("EMdFDB::enumConstExists", "Enum with id_d " + id_d2string(enum_id) + " did not exist.");
				return false;
			}

			// Add it to the cache
			if (!addEnumConstToCache(enum_id, enum_name, enum_const_name, value, is_default))
				return false;
		}

		// If we got this far, then the command as a whole was a success, 
		// no matter whether we found the enumeration constant or not.
		return true;
	}
}

/** Drop enumeration constant.
 *@internal
 *
 * Drops it both from the database and from the local cache.
 *
 * @param enum_id The id_d of the enumeration.
 * @param enum_const_name The name of the enum const to drop.
 * @param value The value of the enum const to drop.
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::dropEnumConst(id_d_t enum_id, 
			   const std::string& enum_const_name, 
			   long value)
{
	if (pConn == 0)
		return false;
	else {
		// Strip the lower bits off that aren't part of the enum_id
		enum_id = STRIP_ENUM_ID_OF_LOWER_BITS(enum_id);

		// Build and execute query
		std::ostringstream query_stream;
		query_stream << "DELETE\n"
			     << "FROM enumeration_constants\n"
			     << "WHERE enum_id = " << long2string(enum_id)
			     << " AND enum_value_name = "
			     << getMYSQL_BINARY() << "'" << enum_const_name << "'"
			     << " AND value = " << long2string(value);
		if (!pConn->execCommand(query_stream.str())) {
			DEBUG_COMMAND_QUERY_FAILED("EMdFDB::dropEnumConst", query_stream.str());
			return false;
		}

		// Delete from cache
		m_enum_const_cache->drop(enum_id, enum_const_name);

		// If we got this far, the command succeeded.
		return true;
	}
}

/** Update enumeration constant.
 *@internal
 *
 * Updates it both in the database and in the local cache.
 *
 * @param enum_const_name The name of the enum const to update.
 * @param enum_id The id_d of the enumeration.
 * @param old_value The current value of the enum const.
 * @param new_value The new value of the enum const.
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::updateEnumConst(const std::string& enum_const_name, 
			     id_d_t enum_id,
			     long old_value,
			     long new_value)
{
	if (pConn == 0)
		return false;
	else {
		// Strip the lower bits off that aren't part of the enum_id
		enum_id = STRIP_ENUM_ID_OF_LOWER_BITS(enum_id);

		//
		// Build and execute query
		//
		std::ostringstream query_stream;
		query_stream 
			<< "UPDATE enumeration_constants\n"
			<< "SET value = " << long2string(new_value) << "\n"
			<< "WHERE enum_id = " << long2string(enum_id) << "\n"
			<< "      AND enum_value_name = "
			<< getMYSQL_BINARY() << "'" << enum_const_name << "'\n"
			<< "      AND value = " << long2string(old_value);
		if (!pConn->execCommand(query_stream.str())) {
			DEBUG_COMMAND_QUERY_FAILED("EMdFDB::updateEnumConst", query_stream.str());
			return false;
		}

		//
		// Update in cache, if there
		//
		m_enum_const_cache->updateValue(enum_id, enum_const_name, new_value);

		// If we got this far, the command succeeded.
		return true;
	}
}

/** Set the default enumeration constant in an enum.
 *@internal
 *
 * Sets it both in the database and in the local cache.
 *
 * @param enum_id The id_d of the enumeration.
 * @param enum_const_name The name of the enum constant to make 
 *        the new default.
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::setDefaultEnumConst(id_d_t enum_id,
				 const std::string& enum_const_name)
{
	if (pConn == 0)
		return false;
	else {
		// Strip the lower bits off that aren't part of the enum_id
		enum_id = STRIP_ENUM_ID_OF_LOWER_BITS(enum_id);

		bool bDoCommit;

		// Start transaction
		bDoCommit = pConn->beginTransaction();

		// Remove current default
		std::ostringstream query_stream1;
		query_stream1
			<< "UPDATE enumeration_constants\n"
			<< "SET is_default = 'N'\n"
			<< "WHERE enum_id = " << long2string(enum_id);
		if (!pConn->execCommand(query_stream1.str())) {
			DEBUG_COMMAND_QUERY_FAILED("EMdFDB::setDefaultEnumConst", query_stream1.str());
			if (bDoCommit)
				pConn->abortTransaction();
			return false;
		}

		// Set new default
		std::ostringstream query_stream2;
		query_stream2
			<< "UPDATE enumeration_constants\n"
			<< "SET is_default = 'Y'\n"
			<< "WHERE enum_id = " << long2string(enum_id) << "\n"
			<< "      AND enum_value_name = "
			<< getMYSQL_BINARY() << "'" << enum_const_name << "'";
		if (!pConn->execCommand(query_stream2.str())) {
			DEBUG_COMMAND_QUERY_FAILED("EMdFDB::setDefaultEnumConst", query_stream2.str());
			if (bDoCommit)
				pConn->abortTransaction();
			return false;
		}

		// Commit transaction
		if (bDoCommit) {
			if (!pConn->commitTransaction()) {
				DEBUG_COMMITTRANSACTION_FAILED("EMdFDB::setDefaultEnumConst");
				return false;
			}
		}

		// Update cache
		m_enum_const_cache->setNewDefault(enum_id, enum_const_name);

		// If we got this far, the command succeeded.
		return true;
	}
}

//
// Feature manipulation
/** Create feature on object type.
 *@internal
 *
 * @param feature_name The name of the new feature.
 *
 * @param object_type_name The name of the object type on which to
 * create the feature.
 *
 * @param object_type_id The id_d of the object type on which to
 * create the feature.
 *
 * @param feature_type_id The id_d of the new feature (if it is an
 * enum, it must be allocated by caller with
 * EMdFDB::getNextTypeID()).
 *
 * @param default_value The default value of the new feature.
 *
 * @param computed Whether the feature is computed (true) or not (false).
 *
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::createFeature(const std::string& feature_name,
			   const std::string& object_type_name,
                           id_d_t object_type_id, 
                           id_d_t feature_type_id, 
                           const std::string& default_value, 
                           bool computed)  // Assumes it does not
// exist
{
	if (pConn == 0)
		return false;
	else {
		// Make sure it is lower-case
		std::string lowercase_feature_name;
		str_tolower(feature_name, lowercase_feature_name);

		// Build FeatureInfo
		FeatureInfo fi(lowercase_feature_name, feature_type_id, 
			       default_value, computed);

		// Build and execute query
		std::ostringstream query_stream;
		query_stream
			<< "INSERT INTO features (\n"
			<< "    object_type_id,\n"
			<< "    feature_name,\n"
			<< "    feature_type_id,\n"
			<< "    default_value,\n"
			<< "    computed\n"
			<< ")\n"
			<< "VALUES (\n"
			<< "    " << long2string(object_type_id)
			<< ", '" << encodeFeatureName(lowercase_feature_name)
			<< "', " << long2string(feature_type_id)
			<< ", " << escapeStringForSQL(default_value)
			<< ", '" << bool2char(computed)
			<< "'\n"
			<< ")";
		if (!pConn->execCommand(query_stream.str())) {
			DEBUG_COMMAND_QUERY_FAILED("EMdFDB::createFeature", query_stream.str());
			return false;
		}

		// Create OT_mdf_FEATURE_NAME_table if necessary
		if (featureTypeIdIsFromSet(feature_type_id)) {
			if (!createStringSetTable(object_type_name, lowercase_feature_name)) {
				DEBUG_X_FAILED("EMdFDB::createFeature", "Creating OT_mdf_FEATURE_NAME_set table for object type " + object_type_name + " and feature " + lowercase_feature_name);
				return false;
			}
		}

		// Make sure we have all other features in cache
		std::list<FeatureInfo> dummy;
		if (!getFeaturesForObjectType(object_type_id, dummy))
			return false;

		// Add to cache
		addFeatureToCacheIfNotAlreadyThere(object_type_id, lowercase_feature_name, feature_type_id, default_value, computed);

		// If we got this far, the command succeeded.
		return true;
	}
}


/** Add feature to local cache if not already there.
 *@internal
 *
 * @param object_type_id The id_d of the object type which contains the feature. 
 * @param feature_name The name of the feature.
 * @param feature_type_id The id_d of the feature.
 * @param default_value The default value of the feature.
 * @param is_computed Whether the feature is computed (true) or not (false).
 */
void EMdFDB::addFeatureToCacheIfNotAlreadyThere(id_d_t object_type_id, 
						const std::string& feature_name, 
						id_d_t feature_type_id, 
						const std::string& default_value, 
						bool is_computed)
{
	// Add it to the feature cache if not already there.
	TableIterator i;
	i = m_feature_cache->find(feature_name, 2, id_d2string(object_type_id), 1);
	if (!i.hasNext()) { // We didn't find it, so add it.
		addFeatureToCache(object_type_id, 
				  feature_name,
				  feature_type_id, 
				  default_value, 
				  is_computed);
	}
}

/** Add feature to local cache unconditionally
 *@internal
 *
 * @param object_type_id The id_d of the object type which contains the feature. * @param feature_name The name of the feature.
 * @param feature_type_id The id_d of the feature.
 * @param default_value The default value of the feature.
 * @param is_computed Whether the feature is computed (true) or not (false).
 */
void EMdFDB::addFeatureToCache(id_d_t object_type_id, 
			       const std::string& feature_name, 
			       id_d_t feature_type_id, 
			       const std::string& default_value, 
			       bool is_computed)
{
	m_feature_cache->startNewRow();
	m_feature_cache->append(id_d2string(object_type_id));
	m_feature_cache->append(std::string(feature_name));
	m_feature_cache->append(id_d2string(feature_type_id));
	m_feature_cache->append(default_value);
	m_feature_cache->append(bool2string(is_computed));
}

/** Add feature to OT_objects table.
 *@internal
 *
 * @param object_type_name The object type name of the OT to contain 
 *        the feature.
 *
 * @param object_type_id The id_d of the object type. Must be the same 
 *        object type as \p object_type_name.
 *
 * @param objectRangeType The object range type of the object type
 * being used.
 *
 * @param fi The FeatureInfo describing the feature.
 *
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::addFeatureToOT_objects(const std::string& object_type_name,
				    id_d_t object_type_id,
				    eObjectRangeType objectRangeType,
				    eMonadUniquenessType monadUniquenessType,
				    const FeatureInfo& fi)
{
	UNUSED(objectRangeType);
	UNUSED(monadUniquenessType);
	
	long enum_value;
  
	// Normalize object type name
	std::string OTN = normalizeOTName(object_type_name);

	// Add feature to "OT_objects" table
	try {
		{
			std::ostringstream query_stream;
			query_stream
				<< "ALTER TABLE " << normalizeTableName(OTN + "_objects", false) << " ADD ";
			std::string secondary_query = query_stream.str();
			bool bUseSecondaryQuery = false;

			// Last arg is false because PostgreSQL does not support NOT NULL
			// constraint on ALTER TABLE ADD COLUMN.
			makeFeatureSQLType(query_stream, fi, false);
      
			bool bAddDefault = false;
			std::string default_value;
      
			// PostgreSQL doesn't support adding defaults 
			// when ALTERing TABLE.
			if (m_backend_kind != kPostgreSQL) {
				bAddDefault = true;
			}

			id_d_t feature_type = fi.getType();
			if (featureTypeIdIsListOf(feature_type)) {
				bAddDefault = false;
			} else if (featureTypeIdIsASCII(feature_type)
				   || featureTypeIdIsSTRING(feature_type)) {
				// Don't add default when it is ASCII or STRING.
				// ASCII and STRING type (LONGTEXT) can't be given a default
				// value by ALTER TABLE in MySQL.
				bAddDefault = false;
			} else if (featureTypeIdIsENUM(feature_type)) {
				// Find the value of the enumeration constant.
				// This sets enum_value, which is also used later when doing
				// UPDATE OT_objects SET <column_name> = <default_value>
				bool bExists;
				bool bDummyIsDefault;
				if (!enumConstExists(fi.getDefaultValue(),
						     fi.getType(),
						     bExists,
						     enum_value,
						     bDummyIsDefault)) {
					DEBUG_X_IS_WRONG("EMdFDB::addFeatureToOT_objects", 
							 "Database error: Could not find enumeration constant with name '" + fi.getDefaultValue() + "'.");
					return false;
				}
				if (!bExists) {
					DEBUG_X_IS_WRONG("EMdFDB::addFeatureToOT_objects", 
							 "Enum constant with name '" + fi.getDefaultValue() + "' did not exist.");
					return false;
				} else {
					default_value = long2string(enum_value);
				}
			} else if (featureTypeIdIsINTEGER(feature_type)
				   || featureTypeIdIsID_D(feature_type)) {
				// The penultimate "false" on FeatureInfo2SQLvalue means that 
				// we must not create any IDD-String association in the 
				// OT_mdf_FEATURE_NAME_set table, but since this feature
				// is not a STRING or ASCII feature, it doesn't matter:
				// We won't call getID_DFromStringSet().
				default_value = FeatureInfo2SQLvalue(OTN, object_type_id, false, fi);
			} else if (featureTypeIdIsSOM(feature_type)) {
				// Don't add default when it is
				// SetOfMonads SetOfMonads SQL type
				// (LONGTEXT) can't be given a default
				// value by ALTER TABLE in MySQL.
				bAddDefault = false;
				bUseSecondaryQuery = true;
				secondary_query += " first_monad_" + encodeFeatureName(fi.getName()) + " INT NOT NULL";
			} else {
				DEBUG_X_IS_WRONG("EMdFDB::addFeatureToOT_objects", 
						 "Feature type with number " + long2string(fi.getType())
						 + " is unknown. Please report this bug\nto Ulrik Petersen via <http://emdros.org/contact.html> and mention this error message.");
				return false;
			}
      
			if (bAddDefault) {
				// Give it a default value.
				query_stream << " DEFAULT " << default_value;
			}
      
			if (!pConn->execCommand(query_stream.str())) {
				DEBUG_COMMAND_QUERY_FAILED("EMdFDB::addFeatureToOT_objects", query_stream.str());
				return false;
			}

			if (bUseSecondaryQuery) {
				if (!pConn->execCommand(secondary_query)) {
					DEBUG_COMMAND_QUERY_FAILED("EMdFDB::addFeatureToOT_objects", secondary_query);
					return false;
				}
			}
      
			// Clean up
			pConn->finalize();
		}
    
		// Set default value
		{
			std::ostringstream query_stream;
			query_stream
				<< "UPDATE " << normalizeTableName(OTN + "_objects", false) << " SET "
				<< encodeFeatureName(fi.getName()) << " = ";
			if (featureTypeIdIsENUM(fi.getType())) {
				query_stream << long2string(enum_value);
			} else {
				// The penultimate "true" on FeatureInfo2SQLvalue means that 
				// we must create any IDD-String association if it is not 
				// there in the OT_mdf_FEATURE_NAME_set table.
				query_stream << FeatureInfo2SQLvalue(OTN, object_type_id, true, fi);
			}
      
			if (!pConn->execCommand(query_stream.str())) {
				DEBUG_COMMAND_QUERY_FAILED("EMdFDB::addFeatureToOT_objects", query_stream.str());
				return false;
			}

			// Clean up
			pConn->finalize();
		}

		// Create index if necessary
		if (featureTypeIdIsWithIndex(fi.getType())) {
			std::string table_name;
			std::string index_name;
			std::list<std::pair<std::string, unsigned int> > column_list2;
			column_list2.push_back(std::pair<std::string, unsigned int>(encodeFeatureName(fi.getName()), MAX_TEXT_INDEX_CHARS));
			getIndexAndTableNameForFeature(object_type_name, 
						       fi.getName(),
						       table_name, 
						       index_name);
			createIndex(index_name, table_name, column_list2);
			column_list2.pop_back();
		}
	} catch (EMdFDBDBError e) {
		DEBUG_X_FAILED("EMdFDB::addFeatureToOT_objects", e.what());
		return false;
	}

	// If we got this far, there were no DB errors
	return true;
}

/** Add feature to database.
 *@internal
 *
 * This is the main entry point in the suite of methods that deal with
 * creation of features.
 *
 * @param object_type_name The object type name of the OT to contain 
 *        the feature.
 * @param object_type_id The id_d of the object type. Must be the same 
 *        object type as \p object_type_name.
 * @param fi The FeatureInfo describing the feature.
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::addFeature(const std::string& object_type_name,
			id_d_t object_type_id,
			const FeatureInfo& fi)
{
	if (pConn == 0)
		return false;
	else {
		bool bDoCommit;

		// Begin transaction
		bDoCommit = pConn->beginTransaction();

		// Normalize object type name
		std::string OTN = normalizeOTName(object_type_name);

		// Get object range type and
		// monad uniqueness type
		bool bExists;
		id_d_t dummy_id;
		eObjectRangeType objectRangeType;
		eMonadUniquenessType monadUniquenessType;
		if (!objectTypeExists(object_type_name, 
				      bExists, 
				      dummy_id, 
				      objectRangeType,
				      monadUniquenessType)) {
			DEBUG_X_FAILED("EMdFDB::addFeature", "checking whether " + object_type_name + " existed");
			return false;
		}
    
		// If it didn't exist, we return DB error
		if (!bExists) {
			DEBUG_X_FAILED("EMdFDB::addFeature", ": object type " + object_type_name + " did not exist");
			return false;
		} 


		// Add feature to "features" table
		if (m_backend_kind == kPostgreSQL 
		    || m_backend_kind == kMySQL) {
			//
			// PostgreSQL and MySQL need it this way around,
			// since otherwise, addFeatureToOT_objects
			// fail if the feature is a STRING|ASCII FROM SET.
			//
			if (!createFeature(fi.getName(),
					   object_type_name,
					   object_type_id,
					   fi.getType(),
					   fi.getDefaultValue(),
					   fi.getIsComputed() != 0)) {
				DEBUG_X_FAILED("EMdFDB::addFeature", "Creating feature");
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}
			
			// Add to OT_objects
			if (!addFeatureToOT_objects(object_type_name, 
						    object_type_id, 
						    objectRangeType, 
						    monadUniquenessType,
						    fi)) {
				DEBUG_X_FAILED("EMdFDB::addFeature", "addFeatureToOT_objects");
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}
		} else if (m_backend_kind == kSQLite3) {

			// SQLite 3 needs it this way around, since
			// otherwise, their special implementation of
			// addFeatureToOT_objects fails.
			//
			if (!addFeatureToOT_objects(object_type_name, 
						    object_type_id, 
						    objectRangeType, 
						    monadUniquenessType,
						    fi)) {
				DEBUG_X_FAILED("EMdFDB::addFeature", "addFeatureToOT_objects");
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}

			if (!createFeature(fi.getName(),
					   object_type_name,
					   object_type_id,
					   fi.getType(),
					   fi.getDefaultValue(),
					   fi.getIsComputed() != 0)) {
				DEBUG_X_FAILED("EMdFDB::addFeature", "Creating feature");
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}
			
		} else {
			ASSERT_THROW(false, "Unknown m_backend_kind.");
		}

		// Commit transaction
		if (bDoCommit) {
			if (!pConn->commitTransaction()) {
				DEBUG_COMMITTRANSACTION_FAILED("EMdFDB::addFeature");
				return false;
			}
		}

		// Make sure we have all other features in cache
		std::list<FeatureInfo> dummy;
		if (!getFeaturesForObjectType(object_type_id, dummy))
			return false;

		// Add to cache
		addFeatureToCacheIfNotAlreadyThere(object_type_id, fi.getName(), fi.getType(), fi.getDefaultValue(), fi.getIsComputed());

		// If we got this far, the command succeeded.
		return true;
	}

}

/** Drop feature from OT_objects table.
 *@internal
 *
 * @param object_type_name The object type name of the OT containing
 *        the feature.
 *
 * @param feature_name the name of the feature to drop.  This must be
 *        the unencoded feature name (i.e., what the user would see).
 *
 * @param objectRangeType The eObjectRangeType of the object type.
 *
 * @param object_type_id The id_d of the object type. Must be the same 
 *        object type as \p object_type_name.
 *
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::dropFeatureFromOT_objects(const std::string& object_type_name,
				       const std::string& feature_name,
				       eObjectRangeType objectRangeType,
				       eMonadUniquenessType monadUniquenessType,
				       id_d_t object_type_id)
{
	UNUSED(objectRangeType);
	UNUSED(monadUniquenessType);
	
	// Normalize object type name
	std::string OTN = normalizeOTName(object_type_name);

	bool bResult = false;
	id_d_t feature_type_id;
	std::string dummy_default_value;
	bool dummy_is_computed;
	if (!featureExists(feature_name,
                           object_type_id,
                           bResult,
                           feature_type_id,
                           dummy_default_value,
                           dummy_is_computed)) {
		return false;
	}

	if (!bResult) {
		return false;
	}

  
	// Remove feature from "OT_objects" table
	std::ostringstream query_stream;
	query_stream
		<< "ALTER TABLE " << normalizeTableName(OTN + "_objects", false) << " DROP COLUMN "
		<< encodeFeatureName(feature_name);
	if (!pConn->execCommand(query_stream.str())) {
		DEBUG_COMMAND_QUERY_FAILED("EMdFDB::dropFeatureFromOT_objects", query_stream.str());
		return false;
	}

	if (featureTypeIdIsSOM(feature_type_id)) {
		std::ostringstream query_stream;
		query_stream
			<< "ALTER TABLE " << normalizeTableName(OTN + "_objects", false) << " DROP COLUMN "
			<< "first_monad_" << encodeFeatureName(feature_name);
		if (!pConn->execCommand(query_stream.str())) {
			DEBUG_COMMAND_QUERY_FAILED("EMdFDB::dropFeatureFromOT_objects", query_stream.str());
			return false;
		}
	}

	return true;
}

/** Drop feature from database.
 *@internal
 *
 * This is the main entry point into the DB for dropping features.
 *
 * @param object_type_name The object type name of the OT containing
 *        the feature.
 * @param feature_name the name of the feature to drop.  This must be
 *        the unencoded feature name (i.e., what the user would see).
 * @param feature_type_id the type id of the feature to drop.
 * @param object_type_id The id_d of the object type. Must be the same 
 *        object type as \p object_type_name.
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::dropFeature(const std::string& object_type_name,
                         const std::string& feature_name,
			 id_d_t feature_type_id,
                         id_d_t object_type_id)
{
	if (pConn == 0)
		return false;
	else {
		bool bDoCommit;

		// Begin transaction
		bDoCommit = pConn->beginTransaction();

		// Normalize object type name
		std::string OTN = normalizeOTName(object_type_name);

		// Get object range type and
		// monad uniqueness type
		bool bExists;
		id_d_t dummy_object_type_id;
		eObjectRangeType objectRangeType;
		eMonadUniquenessType monadUniquenessType;
		if (!objectTypeExists(object_type_name, 
				      bExists, 
				      dummy_object_type_id, 
				      objectRangeType,
				      monadUniquenessType)) {
			DEBUG_X_FAILED("EMdFDB::dropFeature", "checking whether " + object_type_name + " existed");
			return false;
		}
    
		// Remove feature from "OT_objects" table
		if (!dropFeatureFromOT_objects(object_type_name, 
					       feature_name, 
					       objectRangeType,
					       monadUniquenessType,
					       object_type_id)) {
			DEBUG_X_FAILED("EMdFDB::dropFeature", "dropFeatureFromOT_objects");
			if (bDoCommit) {
				pConn->abortTransaction();
			}
			return false;
		}

		// Drop OT_mdf_FEATURE_NAME_set table if necessary
		if (featureTypeIdIsFromSet(feature_type_id)) {
			if (!dropStringSetTable(object_type_name, feature_name)) {
				DEBUG_X_FAILED("EMdFDB::dropFeature", "dropping OT_mdf_FEATURE_NAME_set table for object type '" + object_type_name + "' and feature '" + feature_name + "'");
				return false;
			}
		}

		// Remove feature from "features" table
		{
			// Build and execute query
			std::ostringstream query_stream;
			query_stream
				<< "DELETE FROM features\n"
				<< "WHERE object_type_id = " 
				<< long2string(object_type_id) << "\n"
				<< "      AND feature_name = '" << encodeFeatureName(feature_name) << "'";
			if (!pConn->execCommand(query_stream.str())) {
				DEBUG_COMMAND_QUERY_FAILED("EMdFDB::dropFeature", query_stream.str());
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}
		}

		// Commit transaction
		if (bDoCommit) {
			if (!pConn->commitTransaction()) {
				DEBUG_COMMITTRANSACTION_FAILED("EMdFDB::dropFeature");
				return false;
			}
		}

		// Delete from cache
		TableIterator i;
		i = m_feature_cache->find(std::string(feature_name), 2, id_d2string(object_type_id), 1);
		if (i.hasNext()) {
			m_feature_cache->erase(i);
		}

		// If we got this far, the command succeeded.
		return true;
	}
}


/** Check whether feature exists (by feature name).
 *@internal
 *
 * @param feature_name The name of the feature to find.  This must be
 *        the unencoded feature name (i.e., what the user would see).
 * @param object_type_id The id_d of the object type.
 * @param result Used to return true if it is there, false if it is not.
 * @param feature_type_id Used to return the id_d if it is there.  
 *        Only valid if \p result is true.
 * @param default_value Used to return the default value if it is there.  
 *        Only valid if \p result is true.
 * @param is_computed Used to return whether feature is computed (if it 
 *        is there).  Only valid if \p result is true.
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::featureExists(const std::string& feature_name,
                           id_d_t object_type_id,
                           /* out */ bool& result,
                           /* out */ id_d_t& feature_type_id,
                           /* out */ std::string& default_value,
                           /* out */ bool& is_computed)
{
	if (pConn == 0)
		return false;
	else {
		// First test whether we are doing 'monads'
		if (strcmp_nocase(feature_name, "monads") == 0) {
			feature_type_id = FEATURE_TYPE_SET_OF_MONADS;
			default_value = " { 1 } ";
			is_computed = false;
			result = true;
			return true;
		} 

		// Then try the cache
		TableIterator i;
		i = m_feature_cache->find(std::string(feature_name), 2, id_d2string(object_type_id), 1);
		if (i.hasNext()) {
			feature_type_id = string2id_d(m_feature_cache->getColumn(i, 3));
			default_value = m_feature_cache->getColumn(i, 4);
			is_computed = string2bool(m_feature_cache->getColumn(i, 5));
			result = true;
			return true;
		}

		// If this fails, build and execute query
		std::ostringstream query_stream;
		query_stream
			<< "SELECT feature_type_id, default_value, computed\n"
			<< "FROM features\n"
			<< "WHERE object_type_id = " 
			<< long2string(object_type_id) << "\n"
			<< "      AND feature_name = '" << encodeFeatureName(feature_name) << "'";
		if (!pConn->execSelect(query_stream.str())) {
			DEBUG_SELECT_QUERY_FAILED("EMdFDB::featureExists", query_stream.str());
			return false;
		}

		// Is there at least one tuple? (There should be either 1 or none)
		if (pConn->hasRow()) {
			try {
				// Get feature_type_id
				if (!pConn->accessTuple(0, feature_type_id)) {
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::featureExists");	
					return false;
				}

				// Get default_value
				std::string defval;
				if (!pConn->accessTuple(1, default_value)) {
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::featureExists");
					return false;
				}

				// Get is_computed
				if (!pConn->accessTuple(2, is_computed)) {
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::featureExists");
					return false;
				}
			} catch (EMdFNULLValueException e) {
				DEBUG_NULL_VALUE_EXCEPTION("EMdFDB::featureExists");
				// There was a NULL value exception, hence we return an error
				return false;
			}
      
			// We did find the feature
			result = true;
		} else { 
			// We did not find the feature
			result = false;
		}

		// Release results
		pConn->finalize();

		// Get all features into cache for next call to ourselves.
		std::list<FeatureInfo> dummy;
		if (!getFeaturesForObjectType(object_type_id, dummy))
			return false;

		// If we got this far, then the command as a whole was a success, 
		// no matter whether we found the feature or not.
		return true;
	}
}


// Assumes everything exists 
// TODO: Make this work with computed features
/** Get feature values from a single object.
 *@internal
 *
 * This is the incarnation used in MQLObject::retrieveFeatureValues().
 *
 * @see MQLObject::retrieveFeatureValues().
 *
 * @param object_type_name The name of the object type of the object to query.
 * @param object_type_id The id_d of the object type.
 * @param FeatureInfos List of FeatureInfo objects telling us which
 *        features to get.
 * @param object_id_d The id_d of the object to query.
 * @param results List of EMdFValue containing the values of the features, 
 *        in the same order as in FeatureInfos.
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::getFeatures(const std::string& object_type_name,
			 id_d_t object_type_id,
			 eObjectRangeType objectRangeType, 
			 const std::list<FeatureInfo>& FeatureInfos,
			 id_d_t object_id_d,
			 /* out */ std::list<EMdFValue*>& results)
{
	if (pConn == 0)
		return false;
	else {
		// Normalize object type name
		std::string OTN = normalizeOTName(object_type_name);  

		//
		// Build query
		//
		std::string str_features_to_get;
		std::vector<id_d_t> feature_types_vec;
		std::vector<std::string> feature_names_vec;
		std::vector<StringSetCache*> string_set_caches_vec;
		std::string join_string;
		std::string from_string;
	
		if (!getFeatureVectors(FeatureInfos,
				       OTN,
				       object_type_id,
				       objectRangeType, 
				       str_features_to_get,
				       feature_types_vec,
				       feature_names_vec,
				       string_set_caches_vec,
				       join_string,
				       from_string)) {
			DEBUG_X_FAILED("getFeatures", "getting feature vectors");
			return false;
		}

		int no_of_features_to_get = feature_names_vec.size();

		std::ostringstream query_stream;
		query_stream 
			<< "SELECT object_id_d"
			<< str_features_to_get
			<< "\n"
			<< "FROM " << from_string;

  
		// Add the WHERE clauses
		query_stream << "\nWHERE ";
		if (!join_string.empty()) {
			query_stream << join_string << " AND ";
		}
		query_stream << "OS.object_id_d = " << long2string(object_id_d)
			     << "\n";

		// Execute query
		if (!pConn->execSelect(query_stream.str())) {
			DEBUG_SELECT_QUERY_FAILED("EMdFDB::getFeatures", query_stream.str());
			return false;
		}
  
		// Check that there were results
		if (!pConn->hasRow()) {
			DEBUG_X_IS_WRONG("EMdFDB::getFeatures", "No results from SQL-query");
			return false;
		}

		// Due to the first selected column being 'object_id_d'
		int first_feature_column_index = 1;
  
		//
		// Get results
		//
		EMdFValue *pFeature_values;

		// This uses the () constructor, which gives us a kEVInt with 
		// value 0.
		pFeature_values = new EMdFValue[no_of_features_to_get]();

		if (!getFeatureValues(no_of_features_to_get,
				      objectRangeType,
				      first_feature_column_index,
				      feature_types_vec,
				      feature_names_vec,
				      string_set_caches_vec,
				      pFeature_values)) {
			DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getFeatures");
			pConn->finalize();
			return false;
		}
    
		// Iterate through features
		std::list<FeatureInfo>::const_iterator fi_i = FeatureInfos.begin();
		std::list<FeatureInfo>::const_iterator fi_iend = FeatureInfos.end();
		int feature_name_index = 0;
		while (fi_i != fi_iend) {
			const EMdFValue *pValue = &(pFeature_values[feature_name_index]);

			// Push back the value
			results.push_back(new EMdFValue(*pValue));

			// Advance iterators
			++fi_i;
			++feature_name_index;
		}

		// Release results
		pConn->finalize();

		delete[] pFeature_values;

		// If we got this far, there were no DB errors
		return true;
	}
}

/** Convert a std::list<id_d_t> to a list of list of std::string
 *
 * The list of id_d_t's is converted to a list of list of std::string,
 * where each id_d is converted to its string equivalent (with
 * id_d2string()), and made into a list of string with only that
 * string in it.  Each element in the list of id_d_ts thus becomes a
 * list of string with only one element.  The net effect is to create
 * a "table"-like structure.
 *
 * @see id_d2string().
 *
 * @param object_id_ds The object id_ds to convert.
 *
 * @param results The output.
 */
void local_make_id_d_list(const std::list<id_d_t> object_id_ds, std::list<std::list<std::string> >& results)
{
	std::list<id_d_t>::const_iterator ci(object_id_ds.begin());
	std::list<id_d_t>::const_iterator cend(object_id_ds.end());
	while (ci != cend) {
		// Add list so we have something to fill
		results.push_back(std::list<std::string>());

		// Add id_d
		results.rbegin()->push_back(id_d2string(*ci));

		// Advance iterator
		++ci;
	}
}


// Assumes everything exists 
// TODO: Make this work with computed features
/** Get feature values from a set of objects.
 *@internal
 *
 * This is the incarnation used in the MQL statement "GET FEATURES".
 *
 * Uses EMdFDB::getFeaturesByQuery().
 * 
 * @see EMdFDB::getFeaturesByQuery(), GetFeaturesStatement::exec().
 *
 * @param object_type_name The name of the object type of the object to query.
 *
 * @param object_type_id The id_d of the object type.
 *
 * @param FeatureInfos List of FeatureInfo objects telling us which
 *        features to get.
 *
 * @param object_id_ds_set The set of id_ds to query.  Even though
 * this is a SetOfMonads, it is really id_ds that are in there.
 *
 * @param results List of List of string containing the values of the
 * features, in the same order as in FeatureInfos.
 *
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::getFeatures(const std::string& object_type_name,
			 id_d_t object_type_id,
			 eObjectRangeType objectRangeType, 
			 const std::list<FeatureInfo>& FeatureInfos,
			 const SetOfMonads& object_id_ds_set,
			 /* out */ std::list<std::list<std::string> >& results)
{
	if (pConn == 0)
		return false;
	else {
		// Make sure the results are pristine
		results.clear();
    
		// Return now if the list of features is empty
		// Make results into a list of the object id_ds in the 
		// set of object id_ds.
		if (FeatureInfos.empty()) {
			std::list<id_d_t> object_id_ds_list;
			object_id_ds_set.getMonad_mList(object_id_ds_list);
			local_make_id_d_list(object_id_ds_list, results);
			return true;
		}

		// Make list of enumeration id_ds
		std::list<id_d_t> enumeration_id_ds;
		std::list<FeatureInfo>::const_iterator fii(FeatureInfos.begin());
		std::list<FeatureInfo>::const_iterator fiiend(FeatureInfos.end());
		while (fii != fiiend) {
			// Get enumeration names
			id_d_t feature_type_id = fii->getType();
			if (featureTypeIdIsENUM(feature_type_id)
			    || featureTypeIdIsListOfENUM(feature_type_id)) {
				enumeration_id_ds.push_back(feature_type_id);
			}

			// Advance iterator
			++fii;
		}

		// Load enumerations into cache if there are any enumerations
		if (!enumeration_id_ds.empty()) {
			std::list<id_d_t>::const_iterator ci = enumeration_id_ds.begin();
			std::list<id_d_t>::const_iterator cend = enumeration_id_ds.end();
			for (; ci != cend; ++ci) {
				if (!loadEnumConstantsIntoCache(*ci)) {
					DEBUG_X_FAILED("getFeatures", "Loading enumeration constants into cache");
					return false;
				}
			}
		}

		if (!getFeaturesByQuery(object_type_name, object_type_id,
					objectRangeType,
					FeatureInfos, object_id_ds_set, results)) {
			return false;
		} else {
			return true;
		}
	}
}

/** Get features from the backend
 *@internal
 *
 * Is used by EMdFDB::getFeatures().  Uses getFeaturesByQueryExec().
 *
 * @see EMdFDB::getFeatures(), EMdFDB::getFeaturesByQueryExec().
 *
 * @param object_type_name The name of the object type of the object to query.
 *
 * @param object_type_id The id_d of the object type.
 *
 * @param FeatureInfos List of FeatureInfo objects telling us which
 *        features to get.
 *
 * @param object_id_ds The set of id_ds to query.  Even though this is
 * a SetOfMonads, it is really id_ds that are in there.
 *
 * @param results List of List of string containing the values of the
 * features, in the same order as in FeatureInfos.
 *
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::getFeaturesByQuery(const std::string& object_type_name,
				id_d_t object_type_id,
				eObjectRangeType objectRangeType, 
				const std::list<FeatureInfo>& FeatureInfos,
				const SetOfMonads& object_id_ds,
				/* out */ std::list<std::list<std::string> >& results)
{
	// Normalize object type name
	std::string OTN = normalizeOTName(object_type_name);  

	//
	// Build query
	//
	std::string str_features_to_get;
	std::vector<id_d_t> feature_types_vec;
	std::vector<std::string> feature_names_vec;
	std::vector<StringSetCache*> string_set_caches_vec;
	std::string join_string;
	std::string from_string;
	
	if (!getFeatureVectors(FeatureInfos,
			       OTN,
			       object_type_id,
			       objectRangeType, 
			       str_features_to_get,
			       feature_types_vec,
			       feature_names_vec,
			       string_set_caches_vec,
			       join_string,
			       from_string)) {
		DEBUG_X_FAILED("EMdFDB::getFeaturesByQuery", "getting feature vectors");
		return false;
	}

	std::ostringstream query_stream;
	query_stream 
		<< "SELECT OS.object_id_d"
		<< str_features_to_get
		<< "\n"
		<< "FROM " << from_string;
		query_stream << "\nWHERE ";
		if (!join_string.empty()) {
			query_stream << join_string << " AND ";
		}

  
	// Add the FROM and WHERE clauses
	query_stream << "\n";
	if (!join_string.empty()) {
		query_stream << "(\n";
	}
	SOMConstIterator id_dci = object_id_ds.const_iterator();
	while (id_dci.hasNext()) {
		MonadSetElement mse = id_dci.next();
		if (mse.first() == mse.last()) {
			query_stream << "OS.object_id_d = " << long2string(mse.first());
		} else {
			query_stream << "(OS.object_id_d>=" 
				     << long2string(mse.first())
				     << " AND OS.object_id_d<=" 
				     << long2string(mse.last())
				     << ")";
      
		}
		if (id_dci.hasNext()) {
			query_stream << "\n OR ";
		} else {
			query_stream << '\n';
		}
	}
	if (!join_string.empty()) {
		query_stream << ")";
	}

	// Actually execute query
	return getFeaturesByQueryExec(query_stream.str(), 
				      OTN, object_type_id,
				      objectRangeType,
				      FeatureInfos, 
				      feature_types_vec,
				      feature_names_vec,
				      string_set_caches_vec,
				      results);
}

/** Get features from the backend
 *@internal
 *
 * Is used by getFeaturesByQuery().
 *
 * @see EMdFDB::getFeaturesByQuery().
 *
 * @param query The query to execute
 *
 * @param normalized_object_type_name The name of the object type to
 * query.  It must have been run through normalizeOTName().
 *
 * @param object_type_id The id_d of the object type.  Must be for the
 * same object type as normalized_object_type_name.
 *
 * @param FeatureInfos List of FeatureInfo objects telling us which
 *        features to get.
 *
 * @param results List of List of string containing the values of the
 * features, in the same order as in FeatureInfos.
 *
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::getFeaturesByQueryExec(const std::string query,
				    const std::string& normalized_object_type_name,
				    id_d_t object_type_id,
				    eObjectRangeType objectRangeType,
				    const std::list<FeatureInfo>& FeatureInfos,
				    const std::vector<id_d_t>& feature_types_vec,
				    const std::vector<std::string>& feature_names_vec,
				    const std::vector<StringSetCache*>& string_set_caches_vec,
				    /* out */ std::list<std::list<std::string> >& results)
{
	UNUSED(normalized_object_type_name);
	UNUSED(object_type_id);
	
	int no_of_features_to_get = feature_names_vec.size();

	unsigned int feature_name_index = 0;

	// Execute query
	if (!pConn->execSelect(query)) {
		DEBUG_SELECT_QUERY_FAILED("EMdFDB::getFeaturesByQueryExec", query);
		return false;
	}
  
  
	try {  
		// Get results
		bool bMoreRows = pConn->hasRow();
		int first_feature_column_index = 1;

		// This uses the () constructor, which gives us a kEVInt with 
		// value 0.
		EMdFValue *pFeature_values = new EMdFValue[no_of_features_to_get]();

		int emdf_value_index;

		while (bMoreRows) {
			// Add list so that we have something to fill
			results.push_back(std::list<std::string>());
      
			// Get object id_d from DB
			long field_no = 0;
			std::string object_id_d;
			if (!pConn->accessTuple(field_no, object_id_d)) {
				DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getFeaturesByQueryExec");
				return false;
			}
      
			results.rbegin()->push_back(object_id_d);
			++field_no;

			// Iterate through features

			if (!getFeatureValues(no_of_features_to_get,
					      objectRangeType,
					      first_feature_column_index,
					      feature_types_vec,
					      feature_names_vec,
					      string_set_caches_vec,
					      pFeature_values)) {
				DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getFeatures");
				pConn->finalize();
				return false;
			}
    
			// Iterate through features
			std::list<FeatureInfo>::const_iterator fi_i = FeatureInfos.begin();
			std::list<FeatureInfo>::const_iterator fi_iend = FeatureInfos.end();
			feature_name_index = 0;
			while (fi_i != fi_iend) {
				const EMdFValue *pValue = &(pFeature_values[feature_name_index]);

				// Push back the real value
				id_d_t feature_type = fi_i->getType();
				if (featureTypeIdIsListOf(feature_type)) {
					if (!featureTypeIdIsListOfENUM(feature_type)) {
						// List of integer or list of id_d
						results.rbegin()->push_back(pValue->toString());
					} else {
						IntegerList *pMyIntegerList = pValue->getIntegerList();
						IntegerListConstIterator ci = pMyIntegerList->const_iterator();
						// Is it empty?
						if (!ci.hasNext()) {
							// Yes, it's empty.  So push the empty string.
							results.rbegin()->push_back("");
						} else {
							// No, the list is not empty
							std::string mylistresult;
							while (ci.hasNext()) {
								long enum_const_value = ci.next();
								std::string enum_const_name;
								// First try the cache
								const EnumConstInfo *pECI = m_enum_const_cache->find(feature_type, enum_const_value);
								if (pECI != 0) {
									enum_const_name = pECI->getName();
								} else {
									bool bEnumConstExists;
									bool bDummyIsDefault;
									
									if (!enumConstExists(enum_const_value,
											     feature_type,
											     bEnumConstExists,
											     enum_const_name,
											     bDummyIsDefault)) {
										DEBUG_X_FAILED("EMdFDB::getFeaturesByQueryExec", 
											       "Getting enum const name");
										return false;
									}
									if (!bEnumConstExists) {
										DEBUG_X_IS_WRONG("EMdFDB::getFeaturesByQueryExec", 
												 "Enum const with value " + pValue->toString() + " did not exist.");
										return false;
									}
								}
								mylistresult += DEFAULT_LIST_DELIMITER + enum_const_name;
							}
							mylistresult += DEFAULT_LIST_DELIMITER;
							results.rbegin()->push_back(mylistresult);
						}
					}
				} else if (featureTypeIdIsENUM(feature_type)) {
					long enum_const_value = pValue->getEnum();
					std::string enum_const_name;
					// First try the cache
					const EnumConstInfo *pECI = m_enum_const_cache->find(feature_type, enum_const_value);
					if (pECI != 0) {
						enum_const_name = pECI->getName();
					} else {
						bool bEnumConstExists;
						bool bDummyIsDefault;
						
						if (!enumConstExists(enum_const_value,
								     feature_type,
								     bEnumConstExists,
								     enum_const_name,
								     bDummyIsDefault)) {
							DEBUG_X_FAILED("EMdFDB::getFeaturesByQueryExec", 
								       "Getting enum const name");
							return false;
						}
						if (!bEnumConstExists) {
							DEBUG_X_IS_WRONG("EMdFDB::getFeaturesByQueryExec", 
									 "Enum const with value " + pValue->toString() + " did not exist, 2.");
							return false;
						}
					}
					results.rbegin()->push_back(enum_const_name);
				} else {
					// A catch-all
					results.rbegin()->push_back(pValue->toString());
				}

				// Advance iterators
				++fi_i;
				++feature_name_index;
			}

			// Get next tuple
			if (!pConn->getNextTuple(bMoreRows)) {
				DEBUG_GET_NEXT_TUPLE_FAILED("EMdFDB::getFeaturesByQueryExec");
				return false;
			}

			if (bMoreRows) {
				// If we have more rows, it is
				// important to call the d'tor of all
				// EMdFValues; otherwise, we will get
				// memory leaks for those EMdFValues
				// which allocate memory, such as
				// EMdFValue(SetOfMonads).
				for (emdf_value_index = 0;
				     emdf_value_index < no_of_features_to_get;
				     ++emdf_value_index) {
					pFeature_values[emdf_value_index].~EMdFValue();
				}
			}
		}

		// Release results
		pConn->finalize();

		delete[] pFeature_values;

	} catch (EMdFNULLValueException e) {
		DEBUG_NULL_VALUE_EXCEPTION("EMdFDB::getFeaturesByQueryExec");
		// There was a NULL value exception, hence we return an error
		return false;
	}

	// If we got this far, there were no DB errors
	return true;
}


//
//
/** Get computed features
 *@internal
 *
 * Is used by getFeatures() when there are no backend-features to get.
 *
 * @see EMdFDB::getFeatures(), EMdFDB::getComputedFeature().
 *
 * @param object_type_name The name of the object type to query.
 *
 * @param FeatureInfos List of FeatureInfo objects telling us which
 *        features to get.
 *
 * @param object_id_ds_set The set of id_ds to query.  Even though
 * this is a SetOfMonads, it is really id_ds that are in there.
 *
 * @param results List of List of string containing the values of the
 * features, in the same order as in FeatureInfos.
 *
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::getComputedFeatures(const std::string& object_type_name,
				 const std::list<FeatureInfo>& FeatureInfos,
				 const SetOfMonads& object_id_ds_set,
				 /* out */ std::list<std::list<std::string> >& results)
{
	// Get list of object id_ds
	// (Yes, I know, I know, but id_d_t is typedef'd the same way
	// as monad_m, and so it doesn't matter.  For now, at least.)
	std::list<id_d_t> object_id_ds;
	object_id_ds_set.getMonad_mList(object_id_ds);

	// Get object id_d from list
	std::list<id_d_t>::const_iterator id_di(object_id_ds.begin());
	std::list<id_d_t>::const_iterator id_diend(object_id_ds.end());
	while (id_di != id_diend) {
		// Add list so we have something to fill
		results.push_back(std::list<std::string>());

		// Add id_d
		results.rbegin()->push_back(id_d2string(*id_di));

		// Iterate through features
		std::list<FeatureInfo>::const_iterator fii(FeatureInfos.begin());
		std::list<FeatureInfo>::const_iterator fiiend(FeatureInfos.end());
		while (fii != fiiend) {
			// Get result
			std::string comp_result;
			if (!getComputedFeature(object_type_name, *fii, *id_di, comp_result)) {
				DEBUG_X_FAILED("EMdFDB::getComputedFeatures", "");
				return false;
			}

			// Add result
			results.rbegin()->push_back(comp_result);

			// Advance iterator
			++fii;
		}

		// Advance iterator
		++id_di;
	}
  
	// If we got this far, there were no DB errors
	return true;
}

/** Get computed feature.
 *@internal
 *
 * Is used by getComputedFeatures().
 *
 * @see EMdFDB::getComputedFeatures().
 *
 * @param object_type_name The name of the object type to query.
 *
 * @param fi The feature_info of the feature to get.
 *
 * @param object_id_d The object id_d of the object to query.
 *
 * @param comp_result The string representation of the computed
 * result.
 *
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::getComputedFeature(const std::string& object_type_name, 
				const FeatureInfo& fi, 
				id_d_t object_id_d, 
				/* out */ std::string& comp_result)
{
	UNUSED(object_type_name);
	
	// We must be computed
	if (!fi.getIsComputed()) {
		DEBUG_X_IS_WRONG("EMdFDB::getComputedFeature", 
				 "Function called with a non-computed feature.\n"
				 "This is an internal error.  Please contact Ulrik Petersen\n"
				 "at <ulrikp{a-t}users.sourceforge.net> with a description\n"
				 "of the problem.");
		return false;
	}

	if (strcmp_nocase(fi.getName(), "self") == 0) {
		// Feature is "self"
		// Push back id_d
		comp_result = id_d2string(object_id_d);

		// If we got this far, there were no DB errors
		return true;
	} else {
		// We don't yet support computed features other than "self"
		DEBUG_X_IS_WRONG("EMdFDB::getComputedFeature", 
				 "Computed features other than 'self' not yet supported.");
		return false;
	}
}


/** Add string set value to cache if not already there.
 *@internal
 *
 * Add a string set value to the string set caches.  Only adds it if it
 * is not there already.
 *
 * @param object_type_id the object type id of the object type which
 * holds the feature.
 *
 * @param encoded_feature_name The "mdf_<feature-name>"-encoded
 * version of the feature name.
 *
 * @param set_id_d The id_d of the set value.
 *
 * @param string_value The string to be associated with this set value.
 *
 */
void EMdFDB::addStringToSetCacheIfNotAlreadyThere(id_d_t object_type_id,
						  const std::string& encoded_feature_name,
						  id_d_t set_id_d,
						  const std::string& string_value)
{
	m_string_sets_cache->addPair(object_type_id,
				     encoded_feature_name,
				     set_id_d,
				     string_value);
}

/** Find string set value in cache from set value id
 *@internal
 *
 * Find a string set value in the string set caches, from set value ID.
 *
 * @param object_type_id the object type id of the object type which
 * holds the feature.
 *
 * @param encoded_feature_name The "mdf_<feature-name>"-encoded
 * version of the feature name.
 *
 * @param set_id_d The id_d of the set value.
 *
 * @param result The output string value
 *
 * @return true on found, false on not found.
 */
bool EMdFDB::findStringInSetCacheFromID_D(id_d_t object_type_id,
					  const std::string& encoded_feature_name,
					  id_d_t set_id_d,
					  /* out */ std::string& result)
{
	return m_string_sets_cache->getStringFromID_D(object_type_id,
						      encoded_feature_name,
						      set_id_d,
						      result);
}
					  

/** Find string set ID in cache from string value
 *@internal
 *
 * Find a string set value ID in the string set caches, from string
 * value.
 *
 * @param object_type_id the object type id of the object type which
 * holds the feature.
 *
 * @param encoded_feature_name The "mdf_<feature-name>"-encoded
 * version of the feature name.
 *
 * @param string_value The string value to find.
 *
 * @param set_id_d The output id_d of the set value.
 *
 * @return true on found, false on not found.
 */
bool EMdFDB::findID_DInSetCacheFromString(id_d_t object_type_id,
					  const std::string& encoded_feature_name,
					  const std::string& string_value,
					  /* out */ id_d_t& set_id_d)

{
	return m_string_sets_cache->getID_DFromString(object_type_id,
						      encoded_feature_name,
						      string_value,
						      set_id_d);
}
					  


/** Load all strings from a string set into string set cache 
 *@internal
 *
 * Only queries the DB if not already done.
 *
 * @param object_type_name the name of the object type which holds the
 * feature.  
 *
 * @param object_type_id the object type id of the object type which
 * holds the feature.
 *
 * @param feature_name The feature-name.  This must be the
 * non-"mdf_"-encoded version of the feature name.
 *
 * @return true on no DB error, false on DB error.
 */
bool EMdFDB::loadStringSetIntoCache(const std::string& object_type_name, 
				    id_d_t object_type_id,
				    const std::string& feature_name)
{
	// Encode feature name
	std::string encoded_feature_name = encodeFeatureName(feature_name);

	StringSetCache *pSSC = m_string_sets_cache->getSSC(object_type_id,
							   encoded_feature_name);
	if (pSSC->getIsComplete()) {
		return true;
	}

	// Normalize object type name
	std::string OTN = normalizeOTName(object_type_name);
  
	// Calculate name of table
	std::string table_name = OTN 
		+ "_" + encoded_feature_name + "_set";

	std::string query = "SELECT id_d, string_value FROM " + normalizeTableName(table_name, true);

	if (!pConn->execSelect(query)) {
		DEBUG_SELECT_QUERY_FAILED("EMdFDB::loadStringSetIntoCache", query);
		return false;
	} 
  
	bool bMoreRows = pConn->hasRow();
	while (bMoreRows) {
		try {
			// Get id_d
			id_d_t id_d;
			if (!pConn->accessTuple(0, id_d)) {
				DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::loadStringSetIntoCache");
				return false;
			}
      
			// Get string value
			std::string string_value;
			if (!pConn->accessTuple(1, string_value)) {
				DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::loadStringSetIntoCache");
				return false;
			}
      
			// Add it to the cache
			pSSC->addPair(id_d, string_value);
		} catch (EMdFNULLValueException e) {
			DEBUG_NULL_VALUE_EXCEPTION("EMdFDB::loadStringSetIntoCache");
			// There was a NULL value exception, hence we return an error
			return false;
		}
    
		// Get next tuple
		if (!pConn->getNextTuple(bMoreRows)) {
			DEBUG_GET_NEXT_TUPLE_FAILED("EMdFDB::loadStringSetIntoCache");
			return false;
		}
	}
  
	// Release results
	pConn->finalize();

	pSSC->setIsComplete(true);

	// If we got this far, there were no errors
	return true;
}

/** Get all strings into cache, then get the strings.
 *@internal
 *
 * Only queries the DB if not already done.
 *
 * @param object_type_name the name of the object type which holds the
 * feature.  
 *
 * @param object_type_id the object type id of the object type which
 * holds the feature.
 *
 * @param feature_name The feature-name.  This must be the
 * non-"mdf_"-encoded version of the feature name.
 *
 * @return true on no DB error, false on DB error.
 */
bool EMdFDB::getStringSetStrings(const std::string& object_type_name, 
				 id_d_t object_type_id,
				 const std::string& feature_name,
				 /* out */ Table& result)
{
	if (!loadStringSetIntoCache(object_type_name,
				    object_type_id,
				    feature_name)) {
		DEBUG_X_FAILED("EMdFDB::getStringSetStrings", "loading all strings into cache.");
		return false;
	}

	// Encode feature name
	std::string encoded_feature_name = encodeFeatureName(feature_name);

	StringSetCache *pSSC = m_string_sets_cache->getSSC(object_type_id,
							   encoded_feature_name);
	pSSC->getStringsAsTable(result);
	return true;	
}

/** Get the id_ds in a string set cache as a SetOfMonads.
 *@internal
 *
 * @param normalized_object_type_name the name of the object type
 * which holds the feature.  Must have been normalized with
 * NormalizeOTN().
 *
 * @param object_type_id the object type id of the object type which
 * holds the feature.
 *
 * @param encoded_feature_name The "mdf_<feature-name>"-encoded
 * version of the feature name.
 *
 * @param result a set of "monads" (id_d_t's really) which holds the
 * id_ds in the cache after the call.
 */
void EMdFDB::getStringSetID_DsInCacheAsSOM(const std::string& normalized_object_type_name,
					   id_d_t object_type_id,
					   const std::string& encoded_feature_name,
					   SetOfMonads& id_d_set /* result */)
{
	UNUSED(normalized_object_type_name);
	
	StringSetCache *pSSC = m_string_sets_cache->getSSC(object_type_id,
							   encoded_feature_name);
	pSSC->getID_DsAsSOM(id_d_set);
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
bool EMdFDB::addStringToStringSetTable(const std::string& normalized_object_type_name,
				       const std::string& encoded_feature_name,
				       const std::string& encoded_string_to_create, // Must be encoded with escapeStringForSQL
				       /* out */ id_d_t& result)
{
	// Calculate name of table
	std::string table_name = normalized_object_type_name 
		+ "_" + encoded_feature_name + "_set";

	// Get id_d
	if (!getNextOtherID(result)) {
		DEBUG_X_FAILED("EMdFDB::addStringToStringSetTable", "getting id_d for string in set.");
		return false;
	}
	
	// Get query to add it
	std::string query = "INSERT INTO " + normalizeTableName(table_name, true) + " VALUES(" + id_d2string(result) + "," + encoded_string_to_create + ")";
      
	// Execute query
	if (!pConn->execCommand(query)) {
		DEBUG_COMMAND_QUERY_FAILED("EMdFDB::addStringToStringSetTable", query);
		return false;
	}

	// If we came this far, there were no DB errors, so we return SUCCESS.
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
 * @param encoded_string_to_find The string to be found. Must be
 * encoded with EMdFDB::escapeStringForSQL().
 *
 * @param original_string The string to be found. Must be the original
 * string, before encoding with EMdFDB::escapeStringForSQL().
 *
 * @param bCreateIfNotThere Must be true if the caller wants the id_d
 * to be created if it is not present in the table.
 *
 * @param result The set id_d value to find (encoded as a std::string).
 *
 * @return true on found, false on not found or DB error.
 */
bool EMdFDB::getID_DFromStringSet(const std::string& normalized_object_type_name,
				  id_d_t object_type_id,
				  const std::string& encoded_feature_name,
				  const std::string& encoded_string_to_find, // Must be encoded with escapeStringForSQL
				  const std::string& original_string,
				  bool bCreateIfNotThere,
				  /* out */ std::string& result) // Even though it is an id_d, we get is as a string.
{
	id_d_t id_d;
	if (findID_DInSetCacheFromString(object_type_id,
					 encoded_feature_name,
					 original_string,
					 id_d)) {
		result = id_d2number_string(id_d);
		return true;
	}

	// Calculate name of table
	std::string table_name = normalized_object_type_name 
		+ "_" + encoded_feature_name + "_set";

	// Form query 
	std::string query = "SELECT id_d FROM " + normalizeTableName(table_name, false) + " WHERE string_value = " + getMYSQL_BINARY() + encoded_string_to_find;

	if (!pConn->execSelect(query)) {
		DEBUG_SELECT_QUERY_FAILED("EMdFDB::getID_DFromStringSet", query);
		return false;
	} 

	// Assume we must add it to the cache
	bool bAddToCacheIfNotThere = true;

	// Access result
	if (pConn->hasRow()) {
		try {
			if (!pConn->accessTuple(0, result)) {
				DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getID_DFromStringSet");
				// Return failure  
				return false;
			}
		} catch (EMdFNULLValueException e) {
			DEBUG_NULL_VALUE_EXCEPTION("EMdFDB::getID_DFromStringSet");
			// There was a NULL value exception, hence we return an error
			return false;
		}

		// Release results
		pConn->finalize();
	} else {
		// It was not there! Should we add it?
		if (bCreateIfNotThere) {
			// Release results
			pConn->finalize();

			id_d_t id_d;
			if (!addStringToStringSetTable(normalized_object_type_name,
						       encoded_feature_name,
						       encoded_string_to_find,
						       id_d)) {
				DEBUG_X_FAILED("EMdFDB::getID_DFromStringSet", std::string("Adding string to StringSet table ") + table_name);
				return false;
			}
      
			// Set result
			result = id_d2number_string(id_d);
		} else {
			// It was not there, but we must not add it. Return NIL
			result = id_d2number_string(NIL);
      
			// We don't want to add NIL to the cache.  That would
			// defeat the purpose of going here: NIL would be associated
			// with whatever string was given, and this string
			// would be returned first thing above, from the cache.
			bAddToCacheIfNotThere = false;
		}
	}


	// Add it to the cache
	if (bAddToCacheIfNotThere) {
		addStringToSetCacheIfNotAlreadyThere(object_type_id,
						     encoded_feature_name,
						     string2long(result),
						     original_string);
	}

	// If we got this far, there were no errors
	return true;
}


/** Create a string set table
 *@internal
 *
 * Creates the string set table for a given object type and feature
 * (OT_<feature-name>_set.
 *
 * @param object_type_name The object type name.
 * @param feature_name The name of the feature to get the set table.
 * @return true on no DB error, false on DB error.
 */
bool EMdFDB::createStringSetTable(const std::string& object_type_name, 
				  const std::string& feature_name)
{
	// Normalize object type name
	std::string OTN = normalizeOTName(object_type_name);

	// Encode feature name
	std::string encoded_feature_name = encodeFeatureName(feature_name);

	// Calculate name of table
	std::string table_name =
		normalizeTableName(OTN + "_" + encoded_feature_name + "_set", false);

	// Get query for creating table
	std::string query = "CREATE TABLE " + table_name + " (\n"
		+ "  id_d INTEGER PRIMARY KEY NOT NULL,\n"
		+ "  string_value " + getSQL_TEXT_TYPE() + " NOT NULL\n"
		+ ")";

	// Execute query for creating table
	if (!pConn->execCommand(query)) {
		DEBUG_COMMAND_QUERY_FAILED("EMdFDB::createStringSetTable", query);
		return false;
	}

	// Execute query for creating table
	if (!createIndicesOnStringSetTable(object_type_name, feature_name)) {
		DEBUG_X_FAILED("EMdFDB::createStringSetTable", "creating indices on " + table_name);
		return false;
	}


	// If we got this far, there were no DB errors
	return true;
}

/** Create index(es) on string set table
 *@internal
 *
 * Creates the index OT_<feature_name>_set_i for a given object type
 * and feature OT_<feature-name>_set.
 *
 * @param object_type_name The object type name.
 * @param feature_name The name of the feature to get the set table.
 * @return true on no DB error, false on DB error.
 */
bool EMdFDB::createIndicesOnStringSetTable(const std::string& object_type_name,
					   const std::string& feature_name)
{
	// Normalize object type name
	std::string OTN = normalizeOTName(object_type_name);

	// Encode feature name
	std::string encoded_feature_name = encodeFeatureName(feature_name);

	// Calculate name of table
	std::string table_name = OTN 
		+ "_" + encoded_feature_name + "_set";

	// Drop index on table
	std::list<std::pair<std::string, unsigned int> > column_list;
	column_list.push_back(std::pair<std::string, unsigned int>("string_value", MAX_TEXT_INDEX_CHARS));
	std::string index_name = table_name + "_i";
	if (!createIndex(index_name, table_name, column_list)) {
		DEBUG_X_FAILED("EMdFDB::createIndicesOnStringSetTable",
			       "creating index on " + table_name);
		return false;
	}

	// If we got this far, there were no DB errors
	return true;
}


/** Drop index(es) on string set table
 *@internal
 *
 * Drops the index OT_<feature_name>_set_i for a given object type and
 * feature OT_<feature-name>_set.
 *
 * @param object_type_name The object type name.
 * @param feature_name The name of the feature to get the set table.
 * @return true on no DB error, false on DB error.
 */
bool EMdFDB::dropIndicesOnStringSetTable(const std::string& object_type_name,
					 const std::string& feature_name)
{
	// Normalize object type name
	std::string OTN = normalizeOTName(object_type_name);

	// Encode feature name
	std::string encoded_feature_name = encodeFeatureName(feature_name);

	// Calculate name of table
	std::string table_name = OTN 
		+ "_" + encoded_feature_name + "_set";

	// Create index on table
	std::string index_name = table_name + "_i";
	dropIndex(index_name, table_name);

	// If we got this far, there were no DB errors
	return true;
}



/** Drop a string set table
 *@internal
 *
 * Drops the string set table for a given object type and feature
 * (OT_<feature-name>_set.
 *
 * @param object_type_name The object type name.
 * @param feature_name The name of the feature to get the set table.
 * @return true on no DB error, false on DB error.
 */
bool EMdFDB::dropStringSetTable(const std::string& object_type_name, 
				const std::string& feature_name)
{
	// Normalize object type name
	std::string OTN = normalizeOTName(object_type_name);

	// Encode feature name
	std::string encoded_feature_name = encodeFeatureName(feature_name);

	// Calculate name of table
	std::string table_name =
		normalizeTableName(OTN 
				   + "_" + encoded_feature_name + "_set", false);

	// Get query for creating table
	std::string query = "DROP TABLE " + table_name;

	// Execute query for creating table
	if (!pConn->execCommand(query)) {
		DEBUG_COMMAND_QUERY_FAILED("EMdFDB::dropStringSetTable", query);
		return false;
	}

	// If we got this far, there were no DB errors
	return true;
}


//
// Segment manipulation
// Assumes that segment does not exist
/** Create a segment.
 *@internal
 *
 * This just calls EMdFDB::createMonadSet with the segment name and a
 * SetOfMonads which is the set from first to last.
 *
 * @param segment_name The name of the "segment" (actually monad set)
 * to create.
 *
 * @param first the first monad of the "segment"
 *
 * @param last the last monad of the "segment"
 *
 * @return true on no DB error, false otherwise.
 */
bool EMdFDB::createSegment(const std::string& segment_name, 
			   monad_m first, monad_m last)
{
	// Just create a monad set
	SetOfMonads som(first, last);
	return createMonadSet(segment_name, som);
}

bool EMdFDB::insertMonadRange(monad_m first, monad_m last)
{
	if (pConn == 0) {
		return false;
	} else {
		if (last < first) {
			DEBUG_X_FAILED("EMdFDB::insertMonadRange", "last monad given was smaller than first");
			return false;
		}

		std::list<std::string> object_type_names;
		if (!getObjectTypes(object_type_names)) {
			DEBUG_X_FAILED("EMdFDB::insertMonadRange", "Could not get object types");
			return false;
		}

		std::list<std::string>::const_iterator ci;
		ci = object_type_names.begin();
		while (ci != object_type_names.end()) {
			if (!insertMonadRangeOT(*ci, first, last)) {
				// Here we have already reported error
				return false;
			}
			++ci;
		}

		// If we got this far, there were no errors
		return true;
	}
}

bool EMdFDB::insertMonadRangeOT(const std::string& object_type_name, monad_m first, monad_m last)
{
	if (pConn == 0) {
		return false;
	} else {
		if (last < first) {
			DEBUG_X_FAILED("EMdFDB::insertMonadRange", "last monad given was smaller than first");
			return false;
		}

		// Normalize object type name
		std::string OTN = normalizeOTName(object_type_name);
		
		// Get object range type
		eObjectRangeType objectRangeType;
		if (!getOTObjectRangeType(object_type_name, objectRangeType)) {
			DEBUG_X_FAILED("EMdFDB::insertMonads", "getting range type of object type " + object_type_name);
			return false;
		}

		// Get table name
		std::string table_name = OTN + "_objects";

		monad_m monads_to_insert = last - first + 1;
		std::string str_monads_to_insert = monad_m2string(monads_to_insert);

		/*
		  if (objectRangeType == kORTSingleMonad) {
		  query = "UPDATE " + table_name + "\n" 
		  + "SET first_monad = first_monad + " + str_monads_to_insert + "\n"
		  } else if (objectRangeType == kORTSingleRange) {
		  query = "UPDATE " + table_name + "\n"
		  + "SET first_monad = first_monad + " + monad_m2string(monads_to_insert)
		  + ", last_monad = last_monad + " 
		  }
		*/


		// If we got this far, there were no errors
		return true;
	}
}

/** Select the monad sets available
 *@internal
 *
 * Retrieves a list of the arbitrary monad sets available in the database.
 *
 * @param monad_set_names The output list
 *
 * @return true on no DB error, false otherwise.
 */
bool EMdFDB::selectMonadSets(/* out */ std::list<std::string>& monad_set_names)
{
	if (pConn == 0) {
		return false;
	} else {
		// Make sure the results are pristine
		monad_set_names.clear();
    
		// Make SQL statement
		std::string query = "SELECT monad_set_name FROM monad_sets";
		if (!pConn->execSelect(query)) {
			DEBUG_SELECT_QUERY_FAILED("EMdFDB::selectMonadSets", query);
			return false;
		}

		try {
			bool bMoreRows = pConn->hasRow();
			while (bMoreRows) {
				// Get data from tuple
				std::string monad_set_name;
				if (!pConn->accessTuple(0, monad_set_name)) {
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::selectMonadSets");
					return false;
				}

				// Add to list
				monad_set_names.push_back(monad_set_name);

				// Get next tuple
				if (!pConn->getNextTuple(bMoreRows)) {
					DEBUG_GET_NEXT_TUPLE_FAILED("EMdFDB::selectMonadSets");
					return false;
				}
			}
		} catch (EMdFNULLValueException e) {
			DEBUG_NULL_VALUE_EXCEPTION("EMdFDB::selectMonadSets");
			// There was a NULL value exception, hence we return an error
			return false;
		}
    
		// Release results
		pConn->finalize();

		// If we got this far, there were no DB errors
		return true;
	}
}

/** Checks for existence of monad set
 *@internal
 *
 * Checks for the existence of an arbitrary monad set with name \p
 * monad_set_name , and also retrieves the SetOfMonads if it exists.
 *
 * @param monad_set_name the name of the monad set to find.
 *
 * @param bExists returns true if monad set exists, false otherwise
 *
 * @param som the SetOfMonads from the monad set if it exists. Is
 * undefined if bExists is false.
 *
 * @return true on no DB error, false otherwise.
 */
bool EMdFDB::monadSetExists(const std::string& monad_set_name, 
			    /* out */ bool& bExists,
			    /* out */ SetOfMonads& som)
{
	if (pConn == 0)
		return false;
	else {
		// Make sure the results are pristine
		som.clear();
    
		// Make SQL statement
		std::ostringstream query_stream;
		std::string lowercase_name;
		str_tolower(monad_set_name, lowercase_name);
		query_stream << "SELECT monad_set_id FROM monad_sets WHERE monad_set_name = '" << lowercase_name << "'";
		if (!pConn->execSelect(query_stream.str())) {
			DEBUG_SELECT_QUERY_FAILED("EMdFDB::monadSetExists", query_stream.str());
			return false;
		}

		try {
			// Are there any rows?
			bool bMoreRows = pConn->hasRow();
			if (!bMoreRows) {
				// No: It doesn't exist
				bExists = false;
			} else {
				// Yes, it exists
				bExists = true;

				// Get id_d from tuple
				id_d_t monad_set_id;
				if (!pConn->accessTuple(0, monad_set_id)) {
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::monadSetExists");
					return false;
				}

				// Clean up before we go any further
				pConn->finalize();

				//
				// Get monad set
				//

				// Make SQL statement
				std::ostringstream query_stream2;
				query_stream2 << "SELECT mse_first, mse_last FROM monad_sets_monads WHERE monad_set_id = " << long2string(monad_set_id);
				if (!pConn->execSelect(query_stream2.str())) {
					DEBUG_SELECT_QUERY_FAILED("EMdFDB::monadSetExists", query_stream2.str());
					return false;
				}

				bMoreRows = pConn->hasRow();
				while (bMoreRows) {
					// Get mse_first from tuple
					monad_m mse_first;
					if (!pConn->accessTuple(0, mse_first)) {
						DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::monadSetExists");
						return false;
					}

					// Get mse_last from tuple
					monad_m mse_last;
					if (!pConn->accessTuple(1, mse_last)) {
						DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::monadSetExists");
						return false;
					}

					// Add to set
					som.add(mse_first, mse_last);

					// Get next tuple
					if (!pConn->getNextTuple(bMoreRows)) {
						DEBUG_GET_NEXT_TUPLE_FAILED("EMdFDB::monadSetExists");
						return false;
					}
				}
			}
		} catch (EMdFNULLValueException e) {
			DEBUG_NULL_VALUE_EXCEPTION("EMdFDB::monadSetExists");
			// There was a NULL value exception, hence we return an error
			return false;
		}
    
		// Release results
		pConn->finalize();

		// If we got this far, there were no DB errors
		return true;
	}
}


/** Create an arbitrary monad set
 *@internal
 *
 * @param monad_set_name The name of the set.
 *
 * @param som The SetOfMonads for the monad set.
 *
 * @return true on no DB error, false otherwise.
 */
bool EMdFDB::createMonadSet(const std::string& monad_set_name, const SetOfMonads& som)
{
	if (pConn == 0)
		return false;
	else {
		// Start transaction
		bool bDoCommit = pConn->beginTransaction();

		// Generate auto-generated ID
		id_d_t monad_set_id;
		if (!getNextOtherID(monad_set_id)) {
			DEBUG_X_FAILED("EMdFDB::createMonadSet", "getNextOtherID");
			if (bDoCommit)
				pConn->abortTransaction();
			return false;
		}

		// Create in monad_sets table
		{
			std::ostringstream query_stream;
			std::string lowercase_name;
			str_tolower(monad_set_name, lowercase_name);
			query_stream << "INSERT INTO monad_sets "
				     << "(monad_set_id, monad_set_name) "
				     << "VALUES (" 
				     << long2string(monad_set_id) << ","
				     << "'" << lowercase_name << "'"
				     << ")";
			if (!pConn->execCommand(query_stream.str())) {
				DEBUG_COMMAND_QUERY_FAILED("EMdFDB::createMonadSet", query_stream.str());
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}
		}

		// Create in monad_sets_monads table
		SOMConstIterator ci = som.const_iterator();
		while (ci.hasNext()) {
			// get next mse
			MonadSetElement mse = ci.next();

			// Add it
			std::ostringstream query_stream;
			query_stream << "INSERT INTO monad_sets_monads " 
				     << "(monad_set_id, mse_first, mse_last) "
				     << "VALUES (" 
				     << long2string(monad_set_id) << ","
				     << long2string(mse.first()) << ", "
				     << long2string(mse.last())
				     << ")";
			if (!pConn->execCommand(query_stream.str())) {
				DEBUG_COMMAND_QUERY_FAILED("EMdFDB::createMonadSet", query_stream.str());
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}
		}
    
		// Release results
		pConn->finalize();

		// Commit transaction
		if (bDoCommit) {
			if (!pConn->commitTransaction()) {
				DEBUG_COMMITTRANSACTION_FAILED("EMdFDB::createMonadSet");
				return false;
			}
		}

		// If we got this far, there were no DB errors
		return true;
	}
}


/** Drop a monad set
 *@internal
 *
 * @param monad_set_name The name of the arbitrary monad set to drop.
 *
 * @return true on no DB error, false otherwise.
 */
bool EMdFDB::dropMonadSet(const std::string& monad_set_name)
{
	if (pConn == 0)
		return false;
	else {
		// Get id_d of monad set
		id_d_t monad_set_id;
		{
			std::ostringstream query_stream;
			std::string lowercase_name;
			str_tolower(monad_set_name, lowercase_name);
			query_stream << "SELECT monad_set_id FROM monad_sets WHERE monad_set_name = '" << lowercase_name << "'";
			if (!pConn->execSelect(query_stream.str())) {
				DEBUG_SELECT_QUERY_FAILED("EMdFDB::dropMonadSet", query_stream.str());
				return false;
			}
		}

		try {
			// Does it exist?
			bool bMoreRows = pConn->hasRow();
			if (!bMoreRows) {
				// No: It doesn't exist

				// Simply return
				pConn->finalize();
				return true;
			} else {
				// Yes, it exists

				// Get id_d from tuple
				if (!pConn->accessTuple(0, monad_set_id)) {
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::dropMonadSet");
					return false;
				}

				// Clean up before we go any further
				pConn->finalize();
			}
		} catch (EMdFNULLValueException e) {
			DEBUG_NULL_VALUE_EXCEPTION("EMdFDB::monadSetExists");
			// There was a NULL value exception, hence we return an error
			return false;
		}

		// Start transaction
		bool bDoCommit = pConn->beginTransaction();

		// Drop from monad_sets_monads
		{
			std::ostringstream query_stream;
			query_stream << "DELETE\n"
				     << "FROM monad_sets_monads\n"
				     << "WHERE monad_set_id = " << long2string(monad_set_id);
			if (!pConn->execCommand(query_stream.str())) {
				DEBUG_COMMAND_QUERY_FAILED("EMdFDB::dropMonadSet", query_stream.str());
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}
		}
    
		// Drop from monad_sets
		{
			std::ostringstream query_stream;
			query_stream << "DELETE\n"
				     << "FROM monad_sets\n"
				     << "WHERE monad_set_id = " << long2string(monad_set_id);
			if (!pConn->execCommand(query_stream.str())) {
				DEBUG_COMMAND_QUERY_FAILED("EMdFDB::dropMonadSet", query_stream.str());
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}
		}

		// Commit transaction
		if (bDoCommit) {
			if (!pConn->commitTransaction()) {
				DEBUG_COMMITTRANSACTION_FAILED("EMdFDB::dropMonadSet");
				return false;
			}
		}

		// If we got this far, there were no DB errors
		return true;
	}
}


/** Get current error message.
 *
 * Get current local and remote error messages.
 *
 * @return The error messages combined.
 */
std::string EMdFDB::errorMessage(void)
{
	if (pConn == 0) {
		return ""; 
	} else {
		std::string remote_error;
		std::string local_error;
		std::string out;
		pConn->errorMessage(remote_error);
		local_error = getLocalError();
		if ((remote_error.length() + local_error.length()) > 0) {
			out = "Remote error message:\n"
				+ remote_error + ((remote_error.length() > 0) ? "\n" : "")
				+ "---------------------\n"
				+ "Local error message:\n"
				+ local_error + ((local_error.length() > 0) ? "\n" : "")
				+ "---------------------\n";
		}
		return out;
	}
}

/** Check if connection to DB backend is OK.
 *
 * Just calls EMdFConnection::connectionOk() (or its polymorphic
 * instance).
 *
 * @see EMdFConnection::connectionOk()
 * 
 * @return True if connection is alive, false if not.
 */
bool EMdFDB::connectionOk(void)
{
	if (pConn == 0)
		return false;
	else {
		return pConn->connectionOk();
	}
}

/** Append an error message to the local error string
 *@internal
 *
 * @param error The string to append.
 */
void EMdFDB::appendLocalError(std::string error)
{
	m_local_errormessage += error;
}

/** Clear local error messages.
 *
 * Clear local error messages, both in EMdFDB and in EMdFConnection.
 */
void EMdFDB::clearLocalError(void)
{
	m_local_errormessage = "";
	if (pConn) {
		pConn->clearLocalError();
	}
}

/** Get local error message.
 *
 * Get conglomerate of local error message in EMdFDB and in
 * EMdFConnection.
 *
 * @return The combined error message.
 */
std::string EMdFDB::getLocalError(void) 
{ 
	std::string result;
	result = m_local_errormessage + "\n";
	if (pConn) {
		result += pConn->getLocalError();
	}
	return result; 
}

/** Get objects having monads in a monad set
 *@internal
 *
 * Returns all the objects that have monads overlapping with a
 * specified monad set.  This is used to implement the MQL statement
 * "SELECT OBJECTS HAVING MONADS IN."  The monad set can be anything;
 * it need not be contiguous, but it must be non-empty.
 *
 * Note that this is not the same as objects that fit completely
 * inside the set of monads... it is objects that overlap with it.
 *
 * This is not the public method; it is protected, and is used by the
 * other version of this method.
 *
 * @param object_type_name the name of the object type that holds the
 * objects.
 *
 * @param monad_ms the monad set to use in the search
 *
 * @param objects_already_seen A set of object id_ds of objects which
 * we have already seen.
 *
 * @param objectRangeType the eObjectRangeType of the object type
 *
 * @param result the resulting Table.  Has the same signature as the
 * SELECT OBJECTS HAVING MONADS IN statement (see the MQL User's
 * Guide).
 *
 * @return true on no DB error, false otherwise.
 */
bool EMdFDB::getObjectsHavingMonadsInMonadSet(const std::string object_type_name,
					      const SetOfMonads& monad_ms,
					      std::set<id_d_t>& objects_already_seen,					      
					      eObjectRangeType objectRangeType,
					      /* out */ Table& result)
{
	// Normalize object type name
	std::string OTN = normalizeOTName(object_type_name);

	// Decide mse_first/mse_last or first_monad/last_monad
	std::string firstm, lastm;
	bool2firstm_lastm(objectRangeType, firstm, lastm);

	//
	// Build WHERE clause
	//
	std::ostringstream where_clause;
	// Only do the long thing if there is more than one MSE
	bool bHasOnlyOneMSE = monad_ms.hasOnlyOneMSE();
	if (bHasOnlyOneMSE) {
		where_clause << firstm << " <= " << long2string(monad_ms.last())
			     << " AND " << lastm << " >= " << long2string(monad_ms.first());
	} else {
		where_clause << firstm << " <= " << long2string(monad_ms.last())
			     << " AND " << lastm << " >= " << long2string(monad_ms.first())
			     << " AND (" << '\n';

		SOMConstIterator monad_ci = monad_ms.const_iterator();
		while (monad_ci.hasNext()) {
			MonadSetElement mse = monad_ci.next();
			where_clause 
				<< "("
				<< firstm << " <= "
				<< long2string(mse.last())
				<< " AND "
				<< lastm << " >= " 
				<< long2string(mse.first())
				<< ")"
				;
			if (monad_ci.hasNext()) {
				where_clause 
					<< " OR ";
			}
		}

		where_clause << ")";
	}



	// Build and execute query
	std::ostringstream query_stream;
	query_stream
		<< "SELECT object_id_d, " << firstm;
	if (objectRangeType != kORTSingleMonad) {
		query_stream << ", " << lastm;
	}
	if (objectRangeType == kORTMultipleRange) {
		query_stream << ", monads";
	}
	query_stream << "\n"
		     << "FROM " << normalizeTableName(OTN + "_objects", true) + "\n"
		     << "WHERE " << where_clause.str()
		     << "\nORDER BY object_id_d";
	if (!pConn->execSelect(query_stream.str())) {
		DEBUG_SELECT_QUERY_FAILED("EMdFDB::getObjectsHavingMonadsInMonadSet", query_stream.str());
		return false;
	}

    
	try {
		id_d_t oid;  
		monad_m mse_first;
		monad_m mse_last;
		bool bMoreRows = pConn->hasRow();
		while (bMoreRows) {
			SetOfMonads som;
			// Get data from tuple
			if (!pConn->accessTuple(0, oid)) {
				DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getObjectsHavingMonadsInMonadSet");
				return false;
			}
			if (!pConn->accessTuple(1, mse_first)) {
				DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getObjectsHavingMonadsInMonadSet");
				return false;
			}
			if (objectRangeType == kORTSingleMonad) {
				mse_last = mse_first;
			} else {
				if (!pConn->accessTuple(2, mse_last)) {
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getObjectsHavingMonadsInMonadSet");
					return false;
				}
			}
			if (objectRangeType == kORTMultipleRange) {
				std::string monads_string;
				if (!pConn->accessTuple(3, monads_string)) {
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getObjectsHavingMonadsInMonadSet");
					return false;
				}
				som.fromCompactString(monads_string);
			} else {
				som.add(mse_first, mse_last);
			}

			// Construct data
			// Only add if not already seen
			if (objects_already_seen.find(oid) == objects_already_seen.end()) {
				// Only add if it actually meets the criteria
				SetOfMonads intersection = SetOfMonads::intersect(som, monad_ms);
				if (!intersection.isEmpty()) {
					// Add to set of already-seen objects
					objects_already_seen.insert(oid);
	  
					// Find out which monad to add
					monad_m monad_to_add = intersection.first();
	  
					// Add to result
					result.startNewRow();
					result.append(object_type_name);
					result.append(monad_m2string(monad_to_add));
					result.append(id_d2string(oid));
				}
			}

			// Get next tuple
			if (!pConn->getNextTuple(bMoreRows)) {
				DEBUG_GET_NEXT_TUPLE_FAILED("EMdFDB::getObjectsHavingMonadsInMonadSet");
				return false;
			}
		}
	} catch (EMdFNULLValueException e) {
		DEBUG_NULL_VALUE_EXCEPTION("EMdFDB::getObjectsHavingMonadsInMonadSet");
		// There was a NULL value exception, hence we return an error
		return false;
	}

    
	// Release results
	pConn->finalize();

	// If we got this far, the command was a success.
	return true;
}


/** The maximum number of MonadSetElements to have in a WHERE clause.
 */
#define MAX_MSES (200) 

/** Get objects having monads in monad set.
 *@internal
 *
 * Returns all the objects that have monads overlapping with a
 * specified monad set.  This is used to implement the MQL statement
 * "SELECT OBJECTS HAVING MONADS IN."  The monad set can be anything;
 * it need not be contiguous, but it must be non-empty.
 *
 * Note that this is not the same as objects that fit completely
 * inside the set of monads... it is objects that overlap with it.
 *
 * This is actually what is called from the MQL engine; the other
 * version of this method is used by this method to do the grunt work.
 *
 * @param object_type_name the name of the object type that holds the
 * objects.
 *
 * @param monad_ms the monad set to use in the search
 *
 * @param objectRangeType the eObjectRangeType of the object type
 *
 * @param result the resulting Table.  Has the same signature as the
 * SELECT OBJECTS HAVING MONADS IN statement (see the MQL User's
 * Guide).
 *
 * @return true on no DB error, false otherwise.
 */
bool EMdFDB::getObjectsHavingMonadsInMonadSet(const std::string object_type_name,
					      const SetOfMonads& monad_ms,
					      eObjectRangeType objectRangeType,
					      /* out */ Table& result)
{
	if (pConn == 0)
		return false;
	else {
	  
		// Declare set of objects already seen.  This is initially empty,
		// but is (possibly) added to every time we call the other
		// member function.
		std::set<id_d_t> objects_already_seen;
		
		// Get const_iterator from monad_ms
		SOMConstIterator ci = monad_ms.const_iterator();
		
		// Declare temporary set of monads which we will use for calling
		// the other function
		SetOfMonads temp_som;
		
		// Start with a count of 0
		int mse_count = 0;
		
		// Iterate though monad_ms.
		while (ci.hasNext()) {
			// Add current MSE to temp_som and advance iterator
			temp_som.addMSE(ci.next());
			
			// Increment count of mses
			mse_count++;
			
			// If we have reached the maximum count of MSEs, 
			// or ci has now advanced past the end,
			// then run query.
			if ((mse_count == MAX_MSES) || (!ci.hasNext())) {
				// We have reached maximal mse count, 
				// or there are no more mses left in monad_ms.
				
				// Run query
				if (!getObjectsHavingMonadsInMonadSet(object_type_name, 
								      temp_som, 
								      objects_already_seen, 
								      objectRangeType,
								      result)) {
					// Something went wrong
					
					// We have already reported this error,
					// so no need to do it again.
					// DEBUG_X_FAILED("EMdFDB::getObjectsHavingMonadsInMonadSet", "");
					
					// Clear table 
					// (because we must return an empty table if an error occurred).
					result.clear();
					
					// Return error
					return false;
				} else {
					// Everything went fine
					
					// Now we must prepare for another iteration
					
					// Clear temp_som
					temp_som.clear();
					
					// Set mse_count to 0
					mse_count = 0;
				}
			}
		}
		// If we got here, there were no errors.
		return true;
	}
}

bool EMdFDB::getObjectsHavingMonadsInExec(const std::string& OTN,
					  id_d_t object_type_id,
					  const std::string& str_monad_constraint1,
					  const std::string& query_prefix,
					  const FastSetOfMonads& original_som,
					  eObjectRangeType objectRangeType,

					  const std::string& str_features_to_get,
					  const std::vector<id_d_t>& feature_types_vec,
					  const std::vector<std::string>& feature_names_vec,
					  const std::vector<StringSetCache*>& string_set_caches_vec,
					  const std::string& from_string,
					  const std::string& join_string,
					  const std::string& monad_set_name,
					  /* out */ Inst *pInst)
{
	UNUSED(OTN);
	UNUSED(object_type_id);
	UNUSED(str_features_to_get);
	UNUSED(from_string);
	UNUSED(join_string);
	
	LOG_WRITE_TIME("EMdFDB::getObjectsHavingMonadsInExec", "Query starting.");

	bool bMonadSetIsFeature = strcmp_nocase(monad_set_name, "monads") != 0;

	std::string query = query_prefix + str_monad_constraint1;

	if (!pConn->execSelect(query)) {
		DEBUG_SELECT_QUERY_FAILED("EMdFDB::getObjectsHavingMonadsInExec", query);
		return false;
	}
	LOG_WRITE_TIME("EMdFDB::getObjectsHavingMonadsInExec", "Query finished.");
	int no_of_features_to_get = feature_names_vec.size();

	LOG_WRITE_TIME("EMdFDB::getObjectsHavingMonadsInExec", "Retrieval starting.");
	InstObject *pObj = 0;
	try {
		bool bMoreRows = pConn->hasRow();

		/* first_feature_column_index tells us at which column index
		 * to start looking for feature-values.  This is
		 * normally 3, since object_type, first_monad, and
		 * last_monad are normally retrieved, but if the
		 * object range type is "WITH SINGLE MONAD OBJECTS",
		 * then we don't retrieve the last_monad, so we start
		 * looking for features at index 2.
		 */
		int first_feature_column_index;
		if (bMonadSetIsFeature) {
			first_feature_column_index = 2;
		} else if (objectRangeType == kORTSingleMonad) {
			first_feature_column_index = 2;
		} else if (objectRangeType == kORTSingleRange) {
			first_feature_column_index = 3;
		} else {
			first_feature_column_index = 2;
		}
		while (bMoreRows) {
			// Declare this pFeature_values array of dummy EMdFValue objects.
			EMdFValue *pFeature_values;
			if (no_of_features_to_get > 0) {
				// This uses the () constructor, which gives us a kEVInt with 
				// value 0.
				pFeature_values = (EMdFValue *) pInst->allocate(sizeof(EMdFValue)*no_of_features_to_get);
				/*
				  for (int i = 0; i < no_of_features_to_get; ++i) {
				  // Use placement new operator (p. 255 in TC++PL 3rd ed.)
				  new (&pFeature_values[i]) EMdFValue();
				  }
				*/
			} else {
				pFeature_values = 0;
			}

			// Get object id_d
			id_d_t oid;
			if (!pConn->accessTuple(0, oid)) {
				DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getObjectsHavingMonadsInExec");
				return false;
			}

			bool bGetObject = false;
			SetOfMonads *pSom = 0; 
			monad_m mse_first, mse_last;
			bool bTreatAsFullSOM = false;

			// Get monads 
			if (bMonadSetIsFeature
			    || objectRangeType == kORTMultipleRange) {
				bTreatAsFullSOM = true;

				std::string monads_string;
				if (!pConn->accessTuple(1, monads_string)) {
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getObjectsHavingMonadsInExec");
					return false;
				}
				pSom = new SetOfMonads(monads_string);
				if (FastSetOfMonads::overlap(*pSom, original_som)) {
					mse_first = pSom->first();
					mse_last = pSom->last();
					bGetObject = true;
				} else {
					delete pSom;
				}
			} else {
				// Get mse_first
				if (!pConn->accessTuple(1, mse_first)) {
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getObjectsHavingMonadsInExec");
					return false;
				}
				
				// Get mse_last
				if (objectRangeType == kORTSingleMonad) {
					/* If this is WITH SINGLE MONAD OBJECTS, then we don't 
					 * retrive mse_last.  Instead, we just get it
					 * from mse_first.
					 */
					mse_last = mse_first;
					if (original_som.isMemberOf(mse_first)) {
						bGetObject = true;
					}
				} else {
					if (!pConn->accessTuple(2, mse_last)) {
						DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getObjectsHavingMonadsInExec");
						return false;
					}
					if (original_som.hasMonadsInRange(mse_first, mse_last)) {
						bGetObject = true;
					}
				}
			}

			if (bGetObject) {
				if (pInst->hasObject(mse_first, oid)) {
					bGetObject = false;
					delete pSom;
				}
			}
			if (bGetObject) {
				if (no_of_features_to_get > 0) {
					if (!getFeatureValues(no_of_features_to_get,
							      objectRangeType,
							      first_feature_column_index,
							      feature_types_vec,
							      feature_names_vec,
							      string_set_caches_vec,
							      pFeature_values)) {
						DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getObjectsHavingMonadsInExec");
						pConn->finalize();
						delete pSom;
						return false;
					}
				}

				// Make new object
				// First allocate using Result Inst's allocator
				pObj = (InstObject*) pInst->allocate(sizeof(InstObject));
				// We need to do it this way both if
				// we are MultipleRange and if the
				// monad set is a feature.
				if (bTreatAsFullSOM) {
					if (pSom->hasOnlyOneMSE()) {
						// Then use placement new operator (p. 255 in TC++PL 3rd ed.)
						pObj = new(pObj) InstObject(oid, 
									    mse_first, mse_last,
									    pFeature_values, 
									    no_of_features_to_get); 
						delete pSom;
					} else {
						// Then use placement new operator (p. 255 in TC++PL 3rd ed.)
						pObj = new(pObj) InstObject(oid, 
									    pSom, 
									    pFeature_values, 
									    no_of_features_to_get,
									    false); 
					} 
				} else {
					// Then use placement new operator (p. 255 in TC++PL 3rd ed.)
					pObj = new(pObj) InstObject(oid, 
								    mse_first, mse_last,
								    pFeature_values, 
								    no_of_features_to_get); 
				}

				// Insert object in inst
				pInst->addObject(pObj, mse_first, mse_last);
			}

			// Get next tuple
			if (!pConn->getNextTuple(bMoreRows)) {
				DEBUG_GET_NEXT_TUPLE_FAILED("EMdFDB::getObjectsHavingMonadsInExec");
				return false;
			}
		} // end of iteration over query-results
	} catch (EMdFNULLValueException e) {
		DEBUG_NULL_VALUE_EXCEPTION("EMdFDB::getObjectsHavingMonadsInExec");
		// There was a NULL value exception, hence we return an error
		return false;
	} catch (BadMonadsException e) {
		DEBUG_X_FAILED("EMdFDB::getObjectsHavingMonadsInExec", "Bad monads exception: " + e.what());
		// There was a NULL value exception, hence we return an error
		return false;
	}

	// Write "retrieval finished" to log
	LOG_WRITE_TIME("EMdFDB::getObjectsHavingMonadsInExec", "Retrieval finished.");

	// Release results
	pConn->finalize();

	// If we got this far, the command was a success.
	return true;
}




/** Get SQL query for getObjectsHavingMonadsIn.
 *@internal
 *
 * @param objectRangeType the eObjectRangeType of the object type
 *
 * @param str_features_to_get a list of the features to get.
 *
 * @param feature_types_vec A vector of the feature type ids of the
 * features to get.
 *
 * @param feature_names_vec A vector of the feature names to get (must
 * be in the same order as feature_types_vec).
 *
 * @param from_string A string to be used in the FROM clause of the SQL
 * SELECT statement.
 *
 * @param join_string A string to be used in the WHERE clause of the SQL
 * SELECT statement.
 *
 * @param bHasMonadConstraints A bool to say whether we have monad constraints.
 *
 * @param result The result is returned here.
 *
 */
void EMdFDB::local_get_getObjectsHavingMonadsInSQLQuery(eObjectRangeType objectRangeType,
						
							const std::string& str_features_to_get,
							const std::vector<id_d_t>& feature_types_vec,
							const std::vector<std::string>& feature_names_vec,
							const std::string& pre_query_string,
							const std::string& from_string,
							const std::string& join_string,
							bool bHasMonadConstraints,
							const std::string& monad_set_name,
							/* out */ std::string& result)
{
	UNUSED(feature_types_vec);
	UNUSED(feature_names_vec);

	bool bUsePreQueryString = !pre_query_string.empty();
	bool bUseJoinString = !join_string.empty();
	bool bMonadSetIsFeature = strcmp_nocase(monad_set_name, "monads") != 0;

	std::ostringstream query_stream;
	if (bMonadSetIsFeature) {
		// The monad set is a feature, not the constituting
		// set of monads
		query_stream
			<< "SELECT "
			<< "OS.object_id_d, OS." << encodeFeatureName(monad_set_name) << str_features_to_get << "\n"
			<< "FROM " << from_string << "\n";
	} else if (objectRangeType == kORTSingleRange
	    || objectRangeType == kORTSingleMonad) {
		// It WAS a WITH SINGLE RANGE OBJECTS object type
		// or a WITH SINGLE MONAD OBJECTS object type
		query_stream
			<< "SELECT "
			<< "OS.object_id_d, OS.first_monad";
		if (objectRangeType == kORTSingleRange) {
			query_stream << ", OS.last_monad";
		} 
		query_stream << str_features_to_get 
			     << "\n"
			     << "FROM " << from_string << "\n";
	} else {
		// It was a WITH MULTIPLE RANGE OBJECTS object type
		query_stream
			<< "SELECT "
			<< "OS.object_id_d, OS.monads" << str_features_to_get << "\n"
			<< "FROM " << from_string << "\n";
	}

	if (bUseJoinString
	    || bHasMonadConstraints
	    || bUsePreQueryString) {
		query_stream << "WHERE ";
	}
	
	if (bUseJoinString) {
		query_stream << join_string;
	}
	if (bUsePreQueryString) {
		if (bUseJoinString) {
			query_stream << " AND (" << pre_query_string << ")";
		} else {
			query_stream << "(" << pre_query_string << ")";
		}
	}
	if ((bUseJoinString
	     || bUsePreQueryString)
	    && bHasMonadConstraints) {
		query_stream << " AND ";
	}
	
	result = query_stream.str();
}


/** Get objects having monads in a monad set
 *@internal
 *
 * Returns all the objects that have monads overlapping with a
 * specified monad set.  This is used to implement the MQL statement
 * "GET OBJECTS HAVING MONADS IN."  The monad set can be anything;
 * it need not be contiguous, but it must be non-empty.
 *
 * Note that this is not the same as objects that fit completely
 * inside the set of monads... it is objects that overlap with it.
 *
 * This is not the public method; it is protected, and is used by the
 * other version of this method.
 *
 * @param OTN the name of the object type that holds the
 * objects.
 *
 * @param object_type_id object type id of the object type that holds
 * the objects.
 *
 * @param objectRangeType the eObjectRangeType of the object type
 *
 * @param monad_ms the monad set to use in the search
 *
 * @param original_som the original, complete set of monads (of which
 * the monad_ms parameter is a subset).
 *
 * @param str_features_to_get a list of the features to get.
 *
 * @param feature_types_vec A vector of the feature type ids of the
 * features to get.
 *
 * @param feature_names_vec A vector of the feature names to get (must
 * be in the same order as feature_types_vec).
 *
 * @param from_string A string to be used in the FROM clause of the SQL
 * SELECT statement.
 *
 * @param join_string A string to be used in the WHERE clause of the SQL
 * SELECT statement.

 * @param monad_set_name The name of the monad set. A value of
 * "monads" means that the monad set constituting the object must be
 * used. Otherwise, the feature named by this parameter must exist and
 * must be a set of monads.
 *
 *
 * @param pInst
 *
 * @return true on no DB error, false otherwise.
 */
bool EMdFDB::getObjectsHavingMonadsIn(const std::string OTN,
				      id_d_t object_type_id,
				      eObjectRangeType objectRangeType,
				      monad_m mse_first_monad,
				      monad_m mse_last_monad,
				      const FastSetOfMonads& original_som,
				      const SetOfMonads& all_m_1,
				      const std::string& pre_query_string,
				      const std::string& str_features_to_get,
				      const std::vector<id_d_t>& feature_types_vec,
				      const std::vector<std::string>& feature_names_vec,
				      const std::vector<StringSetCache*>& string_set_caches_vec,
				      const std::string& from_string,
				      const std::string& join_string,
				      monad_m largest_object_length,
				      const std::string& monad_set_name,
				      /* out */ Inst *pInst)
{
	bool bMonadSetIsFeature = strcmp_nocase(monad_set_name, "monads") != 0;

	// Get min_m / max_m
	monad_m min_m = all_m_1.first();
	monad_m max_m = all_m_1.last();

        // Should we make use of monad constraints?
        bool bUseMonadConstraints;
	if (bMonadSetIsFeature) {
                bUseMonadConstraints = true;
	} else if (mse_first_monad == min_m 
            && mse_last_monad == max_m) {
                bUseMonadConstraints = false;
        } else {
                bUseMonadConstraints = true;
        }
        

	// Calculate SELECT...FROM...WHERE part beforehand.
	std::string query_prefix;
	local_get_getObjectsHavingMonadsInSQLQuery(objectRangeType,
						   str_features_to_get,
						   feature_types_vec,
						   feature_names_vec,
						   pre_query_string,
						   from_string,
						   join_string,
						   bUseMonadConstraints, // There is a monad constraint!
						   monad_set_name,
						   query_prefix);
	
	//
	// Build monad constraint
	//
	std::ostringstream str_monad_constraint1;

	if (bMonadSetIsFeature) {
		// We do not have largest_object_length at our
		// disposal, so we cannot rely on that to give us
		// correct answers, only having first_monad at out
		// disposal.  Had we also had a last_monad at our
		// disposal, we would have been able to do this
		// exactly.
		str_monad_constraint1 
			<< "(first_monad_" << encodeFeatureName(monad_set_name) << "<="
				<< long2string(mse_last_monad)
			<< ')';
	} else if (bUseMonadConstraints) {
		// + 1 because, if we want all objects which can
		// potentially end on or after mse.first(), then we
		// want to add 1 so as not to get the ones that can
		// potentially end at mse.first() - 1.
		//
		// To see, this, consider an object which starts at
		// monad 100, in an object type whose largest object
		// length is 2. Say this object is maximal (length 2),
		// then it ends at monad 101.  If we want to find
		// objects which can potentially end at monad 102,
		// then we need to find those which begin at 102 - 2 +
		// 1 = 101, but we do not want to find the object
		// which starts at monad 100.
		//
		// Note that this, coupled with the fact that we fill
		// the monad set such that the longest gap is
		// largest_object_length, is also the reason why we
		// can get away with not doing pInst->hasObject().
		// 
                monad_m first_monad = mse_first_monad - largest_object_length + 1;
                if (first_monad < min_m) {
                        first_monad = min_m;
                }
                monad_m last_monad = mse_last_monad;

		str_monad_constraint1 
			<< "(first_monad<="
			<< long2string(last_monad)
			<< " AND first_monad>=" << long2string(first_monad)
			<< ")"
			;
        }

	if (!getObjectsHavingMonadsInExec(OTN,
					  object_type_id,
					  str_monad_constraint1.str(),
					  query_prefix,
					  original_som,
					  objectRangeType,
					  
					  str_features_to_get,
					  feature_types_vec,
					  feature_names_vec,
					  string_set_caches_vec,
					  from_string,
					  join_string,
					  monad_set_name,
					  pInst)) {
		DEBUG_X_FAILED("EMdFDB::getObjectsHavingMonadsIn", "EMdFDB::getObjectsHavingMonadsInExec()");
		return false;
	}

	return true;
}

bool EMdFDB::getObjectsHavingMonadsInFromSingleUniqueMonadExec(const std::string& OTN,
							       id_d_t object_type_id,
							       const std::string& str_monad_constraint1,
							       const std::string& query_prefix,
							       const std::string& str_features_to_get,
							       const std::vector<id_d_t>& feature_types_vec,
							       const std::vector<std::string>& feature_names_vec,
							       const std::vector<StringSetCache*>& string_set_caches_vec,
							       /* out */ Inst *pInst)
{
	UNUSED(OTN);
	UNUSED(object_type_id);
	UNUSED(str_features_to_get);
	
	LOG_WRITE_TIME("EMdFDB::getObjectsHavingMonadsInFromSingleUniqueMonadExec", "Query starting.");

	std::string query = query_prefix + str_monad_constraint1;

	if (!pConn->execSelect(query)) {
		DEBUG_SELECT_QUERY_FAILED("EMdFDB::getObjectsHavingMonadsInFromSingleUniqueMonadExec", query);
		return false;
	}
	LOG_WRITE_TIME("EMdFDB::getObjectsHavingMonadsInFromSingleUniqueMonadExec", "Query finished.");
	int no_of_features_to_get = feature_names_vec.size();

	LOG_WRITE_TIME("EMdFDB::getObjectsHavingMonadsInFromSingleUniqueMonadExec", "Retrieval starting.");
	InstObject *pObj = 0;
	try {
		bool bMoreRows = pConn->hasRow();

		/* feature_column_index tells us at which column index to start
		 * looking for feature-values.  This is normally 3, since
		 * object_type, first_monad, and last_monad are normally
		 * retrieved, but if the object range type is "WITH SINGLE MONAD
		 * OBJECTS", then we don't retrieve the last_monad, so we
		 * start looking for features at index 2.  In this method,
		 * the object range type is ALWAYS WITH SINGLE MONAD OBJECTS,
		 * and so it is always 2.
		 */
		int first_feature_column_index = 2;

		while (bMoreRows) {
			// Declare this pFeature_values array of dummy EMdFValue objects.
			EMdFValue *pFeature_values;
			if (no_of_features_to_get > 0) {
				// This uses the () constructor, which gives us a kEVInt with 
				// value 0.
				pFeature_values = (EMdFValue *) pInst->allocate(sizeof(EMdFValue)*no_of_features_to_get);
				/*
				  for (int i = 0; i < no_of_features_to_get; ++i) {
				  // Use placement new operator (p. 255 in TC++PL 3rd ed.)
				  new (&pFeature_values[i]) EMdFValue();
				  }
				*/
			} else {
				pFeature_values = 0;
			}

			// Get object id_d
			id_d_t oid;
			if (!pConn->accessTuple(0, oid)) {
				DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getObjectsHavingMonadsInFromSingleUniqueMonadExec");
				return false;
			}


			// Get monads 

			// Get mse_first
			monad_m mse_first;
			if (!pConn->accessTuple(1, mse_first)) {
				DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getObjectsHavingMonadsInFromSingleUniqueMonadExec");
				return false;
			}
				

			// Get feature values
			if (no_of_features_to_get > 0) {
				if (!getFeatureValues(no_of_features_to_get,
						      kORTSingleMonad,
						      first_feature_column_index,
						      feature_types_vec,
						      feature_names_vec,
						      string_set_caches_vec,
						      pFeature_values)) {
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getObjectsHavingMonadsInFromSingleUniqueMonadExec");
					pConn->finalize();
					return false;
				}
			}
			
			// Make new object
			// First allocate using Result Inst's allocator
			pObj = (InstObject*) pInst->allocate(sizeof(InstObject));
			
			// Then use placement new operator (p. 255 in TC++PL 3rd ed.)
			pObj = new(pObj) InstObject(oid, 
						    mse_first, mse_first,
						    pFeature_values, 
						    no_of_features_to_get); 
			// Insert object in inst
			pInst->addObject(pObj, mse_first, mse_first);

			// Get next tuple
			if (!pConn->getNextTuple(bMoreRows)) {
				DEBUG_GET_NEXT_TUPLE_FAILED("EMdFDB::getObjectsHavingMonadsInFromSingleUniqueMonadExec");
				return false;
			}
		} // end of iteration over query-results
	} catch (EMdFNULLValueException e) {
		DEBUG_NULL_VALUE_EXCEPTION("EMdFDB::getObjectsHavingMonadsInFromSingleUniqueMonadExec");
		// There was a NULL value exception, hence we return an error
		return false;
	}

	// Write "retrieval finished" to log
	LOG_WRITE_TIME("EMdFDB::getObjectsHavingMonadsInFromSingleUniqueMonadExec", "Retrieval finished.");

	// Release results
	pConn->finalize();

	// If we got this far, the command was a success.
	return true;
}

bool EMdFDB::getObjectsHavingMonadsInFromSingleUniqueMonadOT(const std::string object_type_name,
							     id_d_t object_type_id,
							     const SetOfMonads& monad_ms,
							     const SetOfMonads& all_m_1,
							     const std::string& pre_query_string,
							     const std::list<FeatureInfo>& features_to_get,
							     /* Out */ Inst *pInst)
{
	UNUSED(all_m_1);
	
	// Normalize object type name
	std::string OTN = normalizeOTName(object_type_name);
	

	// ****************************************************
	//
	// Preliminaries
	//
	// ****************************************************
	// Make string of features to get
	std::string str_features_to_get;
	std::vector<id_d_t> feature_types_vec;
	std::vector<std::string> feature_names_vec;
	std::vector<StringSetCache*> string_set_caches_vec;
	std::string join_string;
	std::string from_string;
	
	if (!getFeatureVectors(features_to_get,
			       OTN,
			       object_type_id,
			       kORTSingleMonad, 
			       str_features_to_get,
			       feature_types_vec,
			       feature_names_vec,
			       string_set_caches_vec,
			       join_string,
			       from_string)) {
		DEBUG_X_FAILED("getObjectsHavingMonadsInFromSingleUniqueMonadOT", "getting feature vectors");
		return false;
	}

	SOMConstIterator ci = monad_ms.const_iterator();

	std::string query_prefix;
	local_get_getObjectsHavingMonadsInSQLQuery(kORTSingleMonad,
						   str_features_to_get,
						   feature_types_vec,
						   feature_names_vec,
						   pre_query_string,
						   from_string,
						   join_string,
						   true, //bUseMonadConstraints,
						   "monads",
						   query_prefix);
	
        

	while (ci.hasNext()) {
		MonadSetElement mse = ci.next();
                
		std::ostringstream str_monad_constraint1;
		monad_m first_monad = mse.first();
		monad_m last_monad = mse.last();
		if (first_monad == last_monad) {
			str_monad_constraint1 << "(first_monad=" 
					      << long2string(first_monad)
					      << ')';
		} else {
			str_monad_constraint1 << "(first_monad>=" 
					      << long2string(first_monad)
					      << " AND first_monad<=" 
					      << long2string(last_monad)
					      << ')';
		}
                
                
		if (!getObjectsHavingMonadsInFromSingleUniqueMonadExec(OTN,
								       object_type_id,
								       str_monad_constraint1.str(),
								       query_prefix,
								       str_features_to_get,
								       feature_types_vec,
								       feature_names_vec,
								       string_set_caches_vec,
								       pInst)) {
			
			return false;
                }
	}

	// finalize Result
	pInst->fillingIsDone();

	// If we got here, there were no errors.
	return true;
}
	
/** Get objects having monads in monad set.
 *@internal
 *
 * Returns all the objects that have monads overlapping with a
 * specified monad set.  This is used to implement the MQL statement
 * "SELECT OBJECTS HAVING MONADS IN."  The monad set can be anything;
 * it need not be contiguous, but it must be non-empty.
 *
 * Note that this is not the same as objects that fit completely
 * inside the set of monads... it is objects that overlap with it.
 *
 * This is actually what is called from the MQL engine; the other
 * version of this method is used by this method to do the grunt work.
 *
 * @param object_type_name the name of the object type that holds the
 * objects.
 *
 * @param object_type_id the id_d of the object type
 *
 * @param objectRangeType the eObjectRangeType of the object type
 *
 * @param monad_ms the monad set to use in the search
 *
 * @param features_to_get a list of the features to get.
 *
 * @param monad_set_name the name of the monad set to use. If
 * "monads", then the monad set of which the object consists is
 * used. Otherwise, the string must refer to a feature on the object
 * type which is a set of monads.
 *
 * @param pInst Result as an Inst
 *
 * @return true on no DB error, false otherwise.
 */
bool EMdFDB::getObjectsHavingMonadsIn(const std::string object_type_name,
				      id_d_t object_type_id,
				      eObjectRangeType objectRangeType,	
				      const SetOfMonads& monad_ms,
				      const SetOfMonads& all_m_1,
				      const std::string& pre_query_string,
				      const std::list<FeatureInfo>& features_to_get,
				      const std::string& monad_set_name,
				      /* Out */ Inst *pInst)
{
	if (pConn == 0)
		return false;
	else {
		// Normalize object type name
		std::string OTN = normalizeOTName(object_type_name);

		// Get monad uniqueness type
		id_d_t dummy_object_type_id;
		bool dummy_bExists;
		eObjectRangeType dummyRangeType;
		eMonadUniquenessType monadUniquenessType;
		if (!objectTypeExists(object_type_name, 
				      dummy_bExists,
				      dummy_object_type_id,
				      dummyRangeType,
				      monadUniquenessType)) {
			return false;
		}

		bool bMonadSetIsFeature = strcmp_nocase(monad_set_name, "monads") != 0;

		if (!bMonadSetIsFeature
		    && objectRangeType == kORTSingleMonad
		    && monadUniquenessType == kMUTUniqueFirstMonads) {
			return getObjectsHavingMonadsInFromSingleUniqueMonadOT(object_type_name,
									       object_type_id,
									       monad_ms,
									       all_m_1,
									       pre_query_string,
									       
									       features_to_get,
									       /* Out */ pInst);
			
		}

		
  
		// Largest object length
		monad_m largest_object_length;
		if (bMonadSetIsFeature) {
			largest_object_length = 1;
		} else {
			if (!getLargestObjectLength(OTN, 
						    objectRangeType,
						    largest_object_length)) {
				DEBUG_X_FAILED("EMdFDB::getObjectsHavingMonadsIn", "getting largest object length");
				return false;
			}
		}

		// ****************************************************
		//
		// Preliminaries
		//
		// ****************************************************
		// Make string of features to get
		std::string str_features_to_get;
		std::vector<id_d_t> feature_types_vec;
		std::vector<std::string> feature_names_vec;
		std::vector<StringSetCache*> string_set_caches_vec;
		std::string join_string;
		std::string from_string;
		
		if (!getFeatureVectors(features_to_get,
				       OTN,
				       object_type_id,
				       objectRangeType, 
				       str_features_to_get,
				       feature_types_vec,
				       feature_names_vec,
				       string_set_caches_vec,
				       join_string,
				       from_string)) {
			DEBUG_X_FAILED("getObjectsHavingMonadsInFromSingleUniqueMonadOT", "getting feature vectors");
			return false;
		}

		/** The minimum gap (of monads) to leave in the set of monads to use 
		 * as a basis for querying. 
		 *@internal
		 */
#define MIN_GAP (1600) 

		// Note that fact, that we fill the monad set such
		// that the longest gap is largest_object_length,
		// coupled with the fact that we look behind no
		// further than is necessary (largest_object_length +
		// 1) is the reason why we can get away with not doing
		// pInst->hasObject().
		// 
		monad_m min_gap;
		if (bMonadSetIsFeature) {
			// If we do a set of monads which is not
			// "monads", we need to do it all in one go.
			// Otherwise, we will most likely do too many
			// sweeps of the database.
			min_gap = monad_ms.last() - monad_ms.first() + 1;
		} else {
			min_gap = (largest_object_length > MIN_GAP) ? largest_object_length : MIN_GAP;
		}
		
		
		
		FastSetOfMonads fast_monad_ms(monad_ms);
		
		// Declare temporary set of monads which we will use for calling
		// the other function
		FastSetOfMonads temp_som(fast_monad_ms.fillGaps(min_gap));
		FastSOMConstIterator ci = temp_som.const_iterator();
		
		// Iterate though monad_ms.
		while (ci.hasNext()) {
			MonadSetElement mse = ci.next();
			
			// Run query
			if (!getObjectsHavingMonadsIn(OTN,
						      object_type_id,
						      objectRangeType,
						      mse.first(),
						      mse.last(),
						      fast_monad_ms,
						      all_m_1,
						      pre_query_string,
						      str_features_to_get,
						      feature_types_vec,
						      feature_names_vec,
						      string_set_caches_vec,
						      from_string,
						      join_string,
						      largest_object_length,
						      monad_set_name,
						      pInst)) {
				// Something went wrong
				
				// We have already reported this error,
				// so no need to do it again.
				// DEBUG_X_FAILED("EMdFDB::getObjectsHavingMonadsIn", "");
				
				// finalize Result
				pInst->fillingIsDone();
				
				// Return error
				return false;
			} else {
				// Everything went fine
			}
		}

		// finalize Result
		pInst->fillingIsDone();

		// If we got here, there were no errors.
		return true;
	}
}

bool EMdFDB::getFeatureVectors(const std::list<FeatureInfo>& features_to_get,
			       const std::string& normalized_OTN,
			       id_d_t object_type_id,
			       eObjectRangeType objectRangeType, 
			       std::string& str_features_to_get,
			       std::vector<id_d_t>& feature_types_vec,
			       std::vector<std::string>& feature_names_vec,
			       std::vector<StringSetCache*>& string_set_caches_vec,
			       std::string& join_string,
			       std::string& from_string) 
{
	// Make string of features to get
	feature_names_vec.resize(features_to_get.size(), "");
	string_set_caches_vec.resize(features_to_get.size(), 0);
	std::list<FeatureInfo>::const_iterator ci(features_to_get.begin());
	std::list<FeatureInfo>::const_iterator cend(features_to_get.end());
	unsigned int feature_names_index = 0;
	unsigned int feature_is_from_set_table_alias_number = 0;
	from_string = normalizeTableName(normalized_OTN + "_objects", true) + " OS";
	while (ci != cend) {
		// Get lowercase name
		std::string locase_name;
		str_tolower(ci->getName(), locase_name);

		if (locase_name == "self") {
			// Is it self?
			// TODO: We should check for computed status when we add
			// ability to do computed features.
			str_features_to_get += ", OS.object_id_d";
			feature_names_vec[feature_names_index] = "object_id_d";
		} else if (locase_name == "monads") {
			// Is it 'monads'?
			if (objectRangeType == kORTSingleRange) {
				str_features_to_get += ", OS.first_monad, OS.last_monad";
			} else if (objectRangeType == kORTSingleMonad) {
				str_features_to_get += ", OS.first_monad";
			} else if (objectRangeType == kORTMultipleRange) {
				str_features_to_get += ", OS.monads";
			} else {
				ASSERT_THROW(false, "Logic error: Emdros has been extended with objectRangeType " + long2string((long)objectRangeType) + " without changing this part of the code.");
			}
			feature_names_vec[feature_names_index] = "monads";
		} else {
			if (featureTypeIdIsFromSet(ci->getType())) {
				if (feature_is_from_set_table_alias_number < getMaxNoOfJoins()) {
					std::string alias = std::string("FIFS") + long2string(feature_is_from_set_table_alias_number);
					++feature_is_from_set_table_alias_number;

					str_features_to_get += ", " + alias + ".string_value";
					if (join_string.length() > 0) {
						join_string += " AND ";
					}
					join_string += "OS." + encodeFeatureName(locase_name) + " = " + alias + ".id_d";
					from_string += ", " + normalizeTableName(normalized_OTN + "_" + encodeFeatureName(locase_name) + "_set", true) + " " + alias;
						
				} else {
					str_features_to_get += ", OS." + encodeFeatureName(locase_name);
					
					if (!loadStringSetIntoCache(normalized_OTN,
								    object_type_id,
								    locase_name)) {
						DEBUG_X_FAILED("EMdFDB::getFeatureVectors", ("getting string set cache for " + normalized_OTN + "." + locase_name));
						return false;
					}
						
					StringSetCache *pSSC = m_string_sets_cache->getSSC(object_type_id, encodeFeatureName(locase_name));
					string_set_caches_vec[feature_names_index] = pSSC;
				}
			} else {
				str_features_to_get += ", OS." + encodeFeatureName(locase_name);
			}
			feature_names_vec[feature_names_index] = locase_name;
		}

		// Add type to feature_types_vec
		feature_types_vec.push_back(ci->getType());

		// Advance iterators
		++ci;
		++feature_names_index;
	}

	// Return 'no DB error'
	return true;
}

bool EMdFDB::getFeatureValues(int no_of_features_to_get,
			      eObjectRangeType objectRangeType,	
			      int first_feature_column_index,
			      const std::vector<id_d_t>& feature_types_vec,
			      const std::vector<std::string>& feature_names_vec,
			      const std::vector<StringSetCache*>& string_set_caches_vec,
			      EMdFValue *pFeature_values)
{
	int feature_index, column_index;
	std::string value_str;
	long value_long;
	for (feature_index = 0, column_index=0; feature_index < no_of_features_to_get; ++feature_index, ++column_index) {
		id_d_t feature_type = feature_types_vec[feature_index];
		if (featureTypeIdIsListOf(feature_type)) {
			if (!pConn->accessTuple(first_feature_column_index + column_index, 
						value_str)) {
				DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getFeatureValues");
				return false;
			}
			IntegerList *pIntegerList = new IntegerList(value_str, DEFAULT_LIST_DELIMITER);
			switch (feature_type & FEATURE_TYPE_TYPE_MASK) {
			case FEATURE_TYPE_LIST_OF_INTEGER:
				new (&pFeature_values[feature_index]) EMdFValue(kEVListOfInteger, pIntegerList);
				break;
			case FEATURE_TYPE_LIST_OF_ID_D:
				new (&pFeature_values[feature_index]) EMdFValue(kEVListOfID_D, pIntegerList);
				break;
			case FEATURE_TYPE_LIST_OF_ENUM:
				// We do not translate to enum constants, since this is done in the MQL layer.
				new (&pFeature_values[feature_index]) EMdFValue(kEVListOfInteger, pIntegerList);
				break;
			default:
				ASSERT_THROW(false, "Unknown FEATURE_TYPE");
				break;
			}
		} else {
			switch (feature_type & FEATURE_TYPE_TYPE_MASK) {
			case FEATURE_TYPE_ASCII:
			case FEATURE_TYPE_STRING:
				if (!pConn->accessTuple(first_feature_column_index + column_index, 
							value_str)) {
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getFeatureValues");
					return false;
				}
				if (featureTypeIdIsFromSet(feature_type)
				    && string_set_caches_vec[feature_index] != 0) {
					std::string newValue;
					id_d_t id_d = string2long(value_str);
					if (!string_set_caches_vec[feature_index]->getStringFromID_D(id_d, newValue)) {
						DEBUG_X_FAILED("EMdFDB::getFeatureValues", "Getting string from set.");
						return false;
					}
					new (&pFeature_values[feature_index]) EMdFValue(newValue);
				} else {
					new (&pFeature_values[feature_index]) EMdFValue(value_str);
				}
				break;
			case FEATURE_TYPE_INTEGER:
				if (!pConn->accessTuple(first_feature_column_index + column_index, 
							value_long)) {
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getFeatureValues");
					return false;
				}
				new (&pFeature_values[feature_index]) EMdFValue(kEVInt, value_long);
				break;
			case FEATURE_TYPE_ID_D:
				if (!pConn->accessTuple(first_feature_column_index + column_index, 
							value_long)) {
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getFeatureValues");
					return false;
				}
				new (&pFeature_values[feature_index]) EMdFValue(kEVID_D, value_long);
				break;
			case FEATURE_TYPE_ENUM:
				if (!pConn->accessTuple(first_feature_column_index + column_index, 
							value_long)) {
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getFeatureValues");
					return false;
				}
				new (&pFeature_values[feature_index]) EMdFValue(kEVEnum, value_long);
				break;
			case FEATURE_TYPE_SET_OF_MONADS:
				if (feature_names_vec[feature_index] == "monads") {
					// Get monads 
					SetOfMonads somFeature;
					monad_m feature_mse_first, feature_mse_last;
					if (objectRangeType == kORTMultipleRange) {
						std::string monads_string;
						if (!pConn->accessTuple(first_feature_column_index + column_index, monads_string)) {
							DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getFeatureValues");
							return false;
						}
						somFeature = SetOfMonads(monads_string);
						feature_mse_first = somFeature.first();
						feature_mse_last = somFeature.last();
					} else {
						// Get feature_mse_first
						if (!pConn->accessTuple(first_feature_column_index + column_index, feature_mse_first)) {
							DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getFeatureValues");
							return false;
						}
						
						// Get feature_mse_last
						if (objectRangeType == kORTSingleMonad) {
							/* If this iw WITH SINGLE MONAD OBJECTS, then we don't 
							 * retrive feature_mse_last.  Instead, we just get it
							 * from feature_mse_first.
							 */
							feature_mse_last = feature_mse_first;
						} else {
							++column_index;
							if (!pConn->accessTuple(first_feature_column_index + column_index, feature_mse_last)) {
								DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getFeatureValues");
								return false;
							}
						}
						somFeature.add(feature_mse_first, feature_mse_last);
					}
					new (&pFeature_values[feature_index]) EMdFValue(somFeature);
				} else {
					if (!pConn->accessTuple(first_feature_column_index + column_index, 
								value_str)) {
						DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getFeatureValues");
						return false;
					}
					{
						SetOfMonads som;
						som.fromCompactString(value_str);
						new (&pFeature_values[feature_index]) EMdFValue(som);
					}
				}
				break;
			default:
				ASSERT_THROW(false, "Unknown FEATURE_TYPE.");
				break;
			}
		}
	}
	
	// Return "no DB error"
	return true;
}


/** The maximum number of conditions to have in a WHERE clause.
 */
#define MAX_CONDITIONS (2*200)

/** get Inst(T,Su).
 *
 *@internal
 *
 * Get an Inst(object type, Substrate).
 *
 * This is the entry point into EMdFDB; the other getInst() is used by
 * this method.
 *
 * Actually, the substrate is not queried monad set element-for-monad
 * set element: Su.first()..Su.last() is queried instead, thus leaving
 * out any gaps.  The MQL engine will filter them out.
 *
 * @param object_type_name the name of the object type
 * 
 * @param object_type_id the id_d_t of the object type; must be for
 * the same object type as \p object_type_name .
 *
 * @param Su the substrate within which to search.
 *
 * @param all_m_1 the all_m-1 object for the database.
 *
 * @param pre_query_string an SQL string, suitable for putting in a
 * WHERE condition, that puts constraints on the objects queries.
 * Must use "OS.mdf_<feature-name>" in comparisons.  Can only use the
 * OT_objects table for the object type specified with
 * object_type_name.
 *
 * @param features_to_get a list of the features to get.
 *
 * @param monad_set_name the name of the monad set to use. If
 * "monads", then the monad set of which the object consists is
 * used. Otherwise, the string must refer to a feature on the object
 * type which is a set of monads.
 *
 * @param Result the Inst to get back.
 *
 * @return true on no DB error, false otherwise.
 */
bool EMdFDB::getInst(const std::string& object_type_name,
		     id_d_t object_type_id,
		     const SetOfMonads& Su,
		     const SetOfMonads& all_m_1,
		     const std::string& pre_query_string,
		     EMdFFFeatures *pre_query_constraints, /* ignored in EMdFDB */
		     const std::list<FeatureInfo>& features_to_get,
		     const std::string& monad_set_name,
		     eMonadSetRelationOperation ms_operation,
		     /* out */ Inst& Result)
{
	UNUSED(pre_query_constraints); // Ignored in EMdFDB
	
	if (pConn == 0)
		return false;
	else {
		// Get range type
		eObjectRangeType objectRangeType;
		if (!getOTObjectRangeType(object_type_name, objectRangeType)) {
			DEBUG_X_FAILED("EMdFDB::getInst", "getting range type of object type " + object_type_name);
			return false;
		}

		// Get Inst
		if (!getInst(object_type_name,
			     object_type_id,
			     Su.first(), Su.last(),
			     all_m_1,
			     pre_query_string,
			     features_to_get,
			     monad_set_name,
			     objectRangeType,
			     ms_operation,
			     Result)) {
			return false;
		}
    
		// If we got this far, the command was as success.
		return true;
	}
}


/** get Inst(T,Su)
 *@internal
 *
 * Get an Inst(object type, Substrate), within a range
 * first_monad..last_monad.
 *
 * This is not the public entry point into EMdFDB; the other getInst()
 * uses this method.
 *
 * @param object_type_name the name of the object type
 * 
 * @param object_type_id the id_d_t of the object type; must be for
 * the same object type as \p object_type_name .
 *
 * @param first_monad the first monad to query
 *
 * @param last_monad the last monad to query (everything in between
 * first_monad and last_monad will be queried)
 *
 * @param all_m_1 the all_m-1 object for the database.
 *
 * @param pre_query_string an SQL string, suitable for putting in a
 * WHERE condition, that puts constraints on the objects queries.
 * Must use "OS.mdf_<feature-name>" in comparisons.  Can only use the
 * OT_objects table for the object type specified with
 * object_type_name.
 *
 * @param features_to_get a list of the features to get.
 *
 * @param monad_set_name the name of the monad set to use. If
 * "monads", then the monad set of which the object consists is
 * used. Otherwise, the string must refer to a feature on the object
 * type which is a set of monads.
 *
 * @param objectRangeType the eObjectRangeType of the object type
 *
 * @param largest_object_length the length (in monads) of the longest
 * object in the object type which we are querying.
 *
 * @param Result the Inst to get back.
 *
 * @return true on no DB error, false otherwise.
 */
bool EMdFDB::getInst(const std::string& object_type_name,
		     id_d_t object_type_id,
		     monad_m first_monad, monad_m last_monad,
		     const SetOfMonads& all_m_1,
		     const std::string& pre_query_string,
		     const std::list<FeatureInfo>& features_to_get,
		     const std::string& monad_set_name,
		     eObjectRangeType objectRangeType,
		     eMonadSetRelationOperation ms_operation,
		     /* out */ Inst& Result)
{
	if (pConn == 0)
		return false;
	else {
		// ****************************************************
		//
		// Preliminaries
		//
		// ****************************************************

		// Are we doing a normal "monads" Inst or a "monad-set-feature" Inst
		bool bMonadSetIsFeature = strcmp_nocase(monad_set_name, "monads") != 0;
		
		// Normalize object type name
		std::string OTN = normalizeOTName(object_type_name);

		// Make string of features to get
		std::string str_features_to_get;
		std::vector<id_d_t> feature_types_vec;
		std::vector<std::string> feature_names_vec;
		std::vector<StringSetCache*> string_set_caches_vec;
		std::string join_string;
		std::string from_string;
		
		if (!getFeatureVectors(features_to_get,
				       OTN,
				       object_type_id,
				       objectRangeType, 
				       str_features_to_get,
				       feature_types_vec,
				       feature_names_vec,
				       string_set_caches_vec,
				       join_string,
				       from_string)) {
			DEBUG_X_FAILED("EMdFDB::getInst", "getting feature vectors");
			return false;
		}
		    

		/*
		std::string str_features_to_get;
		std::vector<id_d_t> feature_types_vec;
		std::vector<std::string> feature_names_vec(features_to_get.size());
		std::vector<StringSetCache*> string_set_caches_vec(features_to_get.size(), 0);
		std::list<FeatureInfo>::const_iterator ci(features_to_get.begin());
		std::list<FeatureInfo>::const_iterator cend(features_to_get.end());
		unsigned int feature_names_index = 0;
		unsigned int feature_is_from_set_table_alias_number = 0;
		std::string join_string;
		std::string from_string = OTN + "_objects OS";
		while (ci != cend) {
			// Get lowercase name
			std::string locase_name;
			str_tolower(ci->getName(), locase_name);

			if (locase_name == "self") {
				// Is it self?
				// TODO: We should check for computed status when we add
				// ability to do computed features.
				str_features_to_get += ", OS.object_id_d";
				feature_names_vec[feature_names_index] = "object_id_d";
			} else if (locase_name == "monads") {
				// Is it 'monads'?
				if (objectRangeType == kORTSingleRange) {
					str_features_to_get += ", OS.first_monad, OS.last_monad";
				} else if (objectRangeType == kORTSingleMonad) {
					str_features_to_get += ", OS.first_monad";
				} else if (objectRangeType == kORTMultipleRange) {
					str_features_to_get += ", OS.monads";
				} else {
					ASSERT_THROW(false, "Logic error: Emdros has been extended with objectRangeType " + long2string((long)objectRangeType) + " without changing this part of the code.");
				}
				feature_names_vec[feature_names_index] = "monads";
			} else {
				if (featureTypeIdIsFromSet(ci->getType())) {
					if (feature_is_from_set_table_alias_number < getMaxNoOfJoins()) {
						std::string alias = std::string("FIFS") + long2string(feature_is_from_set_table_alias_number);
						++feature_is_from_set_table_alias_number;

						str_features_to_get += ", " + alias + ".string_value";
						//feature_names_vec[feature_names_index] = encodeFeatureName(locase_name);
						if (join_string.length() > 0) {
							join_string += " AND ";
						}
						join_string += "OS." + encodeFeatureName(locase_name) + " = " + alias + ".id_d";
						from_string += ", " + OTN + "_" + encodeFeatureName(locase_name) + "_set" + " " + alias;
						
					} else {
						str_features_to_get += ", OS." + encodeFeatureName(locase_name);
						//feature_names_vec[feature_names_index] = encodeFeatureName(locase_name);
					
						if (!loadStringSetIntoCache(OTN,
									    object_type_id,
									    locase_name)) {
							DEBUG_X_FAILED("EMdFDB::getInst", ("getting string set cache for " + OTN + "." + locase_name));
							return false;
						}
						
						StringSetCache *pSSC = m_string_sets_cache->getSSC(object_type_id, encodeFeatureName(locase_name));
						string_set_caches_vec[feature_names_index] = pSSC;
					}
				} else {
					str_features_to_get += ", OS." + encodeFeatureName(locase_name);
				}
				feature_names_vec[feature_names_index] = locase_name;
			}

			// Add type to feature_types_vec
			feature_types_vec.push_back(ci->getType());

			// Advance iterators
			++ci;
			++feature_names_index;
		}
		*/
		

		
		from_string += "\n";

		// Make monad_constraint1
		std::ostringstream monad_constraint_stream1;
		if (bMonadSetIsFeature) {
			// NOTE: We do not need to consider 
			// largest_object_length in getInst().
			//
			// For getInst() we want all those o that, for a
			// given Substrate Su, conform to this formula:
			//
			// o.first() >= Su.first() and o.last() <= Su.last().
			//
			// But since forall o: o.first() <= o.last()
			//
			// this can be rewritten to:
			//
			// o.first() >= Su.first() and o.first() <= Su.last().
			//
			// Hence, we do not need to care about 
			// largest_object_length.
			std::string first_monad_column_name = "first_monad_" + encodeFeatureName(monad_set_name);
			monad_constraint_stream1 << "OS." << first_monad_column_name << ">=" 
						 << long2string(first_monad) 
						 << " AND OS." << first_monad_column_name << "<=" 
						 << long2string(last_monad);
		} else {
			if (objectRangeType == kORTSingleMonad) {
				monad_constraint_stream1  << "OS.first_monad>=" 
							  << long2string(first_monad)
							  << " AND OS.first_monad<=" 
							  << long2string(last_monad);
			} else {
				// NOTE: It is NOT a good idea to not to use BETWEEN here...
				// For some reason, BETWEEN is faster on SQLite2,
				// even though BETWEEN makes it impossible to use the
				// index.
				//monad_constraint_stream1  << "OS.first_monad BETWEEN " << first_monad
				//			  << " AND " << last_monad);
				
				// NOTE: We do not need to consider 
				// largest_object_length in getInst().
				//
				// For getInst() we want all those o that, for a
				// given Substrate Su, conform to this formula:
				//
				// o.first() >= Su.first() and o.last() <= Su.last().
				//
				// But since forall o: o.first() <= o.last()
				//
				// this can be rewritten to:
				//
				// o.first() >= Su.first() and o.first() <= Su.last().
				//
				// Hence, we do not need to care about 
				// largest_object_length.
				
				monad_constraint_stream1 << "OS.first_monad>=" 
							 << long2string(first_monad) 
							 << " AND OS.first_monad<=" 
							 << long2string(last_monad);
			}
		}

		std::string str_monad_constraint1;
		str_monad_constraint1 = monad_constraint_stream1.str();
			
			// Get min_m and max_m
		monad_m min_m = all_m_1.first();
		monad_m max_m = all_m_1.last();

		// Get boundaries for 20% from either end of the monads
		monad_m monads_in_database = max_m - min_m;
		monad_m monad_fraction = monads_in_database / 5;

		// Should we make use of monad constraints?
		// Only if both first_monad and last_monad are within
		// 20% of min_m and max_m respectively.
		// This is because the extra monad constraints clause
		// slows us down (and is unnecessary) if the boundaries
		// are within 20% of the end points.
		bool bUseMonadConstraints;
		bool bFirstLastMonadAreMinMMaxM;
		if (first_monad == min_m 
		    && last_monad == max_m) {
			bUseMonadConstraints = false;
			bFirstLastMonadAreMinMMaxM = true;
		} else  if (first_monad <= (min_m + monad_fraction)
			    && last_monad >= (max_m - monad_fraction)) {
			bUseMonadConstraints = false;
			bFirstLastMonadAreMinMMaxM = false;
		} else {
			bUseMonadConstraints = true;
			bFirstLastMonadAreMinMMaxM = false;
		}

		// Should we make use of pre_query_string?
		bool bUsePreQueryString;
		if (pre_query_string.length() > 0) {
			bUsePreQueryString = true;
		} else {
			bUsePreQueryString = false;
		}

		bool bUseJoinString;
		if (join_string.length() > 0) {
			bUseJoinString = true;
		} else {
			bUseJoinString = false;
		}
   
		// Make str_pre_query_string
		std::string str_pre_query_string;
		if (bUsePreQueryString) {
			if (bUseMonadConstraints) {
				str_pre_query_string = " (" + pre_query_string + ") AND ";
			} else {
				str_pre_query_string = " (" + pre_query_string + ")";
			}
		} 
		
		// Should we make use of str_features_to_get
		bool bUseFeaturesToGet;
		if (str_features_to_get.length() > 0) {
			bUseFeaturesToGet = true;
		} else {
			bUseFeaturesToGet = false;
		}
    
		// Get whether it is a WITH SINGLE RANGE OBJECTS or not
		eObjectRangeType objectRangeType;
		if (!getOTObjectRangeType(object_type_name, objectRangeType)) {
			DEBUG_X_FAILED("EMdFDB::getInst", "getting range type of object type " + object_type_name);
			return false;
		}


		// ****************************************************
		//
		// Build and execute first query
		//
		// ****************************************************
		//LOG_WRITE_TIME("EMdFDB::getInst", "Query starting.");
		std::ostringstream query_stream;
		if (bMonadSetIsFeature) {
			if (bUsePreQueryString) {
				query_stream
					<< "SELECT " 
					<< "OS.object_id_d, OS." << encodeFeatureName(monad_set_name) << str_features_to_get << "\n"
					<< "FROM " << from_string
					<< "WHERE ";
				if (bUseJoinString) {
					query_stream << join_string << " AND ";
				}
				query_stream << str_pre_query_string;
				if (bUseMonadConstraints) {
					query_stream << '\n' << str_monad_constraint1;
				}
			} else { // pre_query_string is empty
				query_stream
					<< "SELECT "
					<< "OS.object_id_d, OS." << encodeFeatureName(monad_set_name) << str_features_to_get << "\n"
					<< "FROM " << from_string;
				if (bUseMonadConstraints || bUseJoinString) {
					query_stream << "WHERE ";
					if (bUseJoinString) {
						query_stream << join_string;
						if (bUseMonadConstraints) {
							query_stream << " AND ";
						}
					}
					if (bUseMonadConstraints) {
						query_stream << str_monad_constraint1;
					}
				}
			}
		} else {
			// monad set is "monads", not a feature
			if (objectRangeType == kORTSingleRange
			    || objectRangeType == kORTSingleMonad) {
				// It WAS a WITH SINGLE RANGE OBJECTS object type
				// or a WITH SINGLE MONAD OBJECTS object type
				if (bUsePreQueryString) {
					query_stream
						<< "SELECT " 
						<< "OS.object_id_d, OS.first_monad";
					if (objectRangeType == kORTSingleRange) {
						query_stream << ", OS.last_monad";
					} 
					query_stream << str_features_to_get 
						     << "\n"
						     << "FROM " << from_string
						     << "WHERE ";
					if (bUseJoinString) {
						query_stream << join_string << " AND ";
					}
					query_stream << str_pre_query_string;
					if (bUseMonadConstraints) {
						query_stream << '\n' << str_monad_constraint1;
					}
				} else { // pre_query_string is empty
					if (bUseMonadConstraints || bUseFeaturesToGet) {
						query_stream
							<< "SELECT "
							<< "OS.object_id_d, OS.first_monad";
						if (objectRangeType == kORTSingleRange) {
							query_stream << ", OS.last_monad";
						} 
						query_stream << str_features_to_get 
							     << "\n"
							     << "FROM " << from_string;
						if (bUseMonadConstraints || bUseJoinString) {
							query_stream << "WHERE ";
							if (bUseJoinString) {
								query_stream << join_string;
								if (bUseMonadConstraints)
									query_stream << " AND ";
							}
							if (bUseMonadConstraints) 
								query_stream << str_monad_constraint1;
						}
					} else {
						query_stream
							<< "SELECT "
							<< "OS.object_id_d, OS.first_monad";
						if (objectRangeType == kORTSingleRange) {
							query_stream << ", OS.last_monad";
						} 
						query_stream << "\n"
							     << "FROM " << from_string;
						if (bUseJoinString) {
							query_stream << "WHERE " << join_string;
						}
					}
				}
			} else {
				// It was a WITH MULTIPLE RANGE OBJECTS object type
				if (bUsePreQueryString) {
					query_stream
						<< "SELECT " 
						<< "OS.object_id_d, OS.monads" << str_features_to_get << "\n"
						<< "FROM " << from_string
						<< "WHERE ";
					if (bUseJoinString) {
						query_stream << join_string << " AND ";
					}
					query_stream << str_pre_query_string;
					if (bUseMonadConstraints) {
						query_stream << '\n' << str_monad_constraint1;
					}
				} else { // pre_query_string is empty
					query_stream
						<< "SELECT "
						<< "OS.object_id_d, OS.monads" << str_features_to_get << "\n"
						<< "FROM " << from_string;
					if (bUseMonadConstraints || bUseJoinString) {
						query_stream << "WHERE ";
						if (bUseJoinString) {
							query_stream << join_string;
							if (bUseMonadConstraints) {
								query_stream << " AND ";
							}
						}
						if (bUseMonadConstraints) {
							query_stream << str_monad_constraint1;
						}
					}
				}
			}
		}
		if (!pConn->execSelect(query_stream.str())) {
			DEBUG_SELECT_QUERY_FAILED("EMdFDB::getInst", query_stream.str());
			return false;
		}
		
		//LOG_WRITE_TIME("EMdFDB::getInst", "Query finished.");
			int no_of_features_to_get = features_to_get.size();


		LOG_WRITE_TIME("EMdFDB::getInst", "Retrieval starting.");
		InstObject *pObj = 0;
		try {
			bool bMoreRows = pConn->hasRow();
			
			/* first_feature_column_index tells us at which
			 * column index to start looking for
			 * feature-values.  This is normally 3, since
			 * object_type, first_monad, and last_monad
			 * are normally retrieved, but if the object
			 * range type is "WITH SINGLE MONAD OBJECTS",
			 * then we don't retrieve the last_monad, so
			 * we start looking for features at index 2.
			 * Similarly, if the object type range is
			 * "WITH MULTIPLE RANGE OBJECTS", then we
			 * retrieve object_id_d, monads, so we start
			 * looking for features at index 2.
			 */
			int first_feature_column_index;
			if (bMonadSetIsFeature) {
				first_feature_column_index = 2;
			} else if (objectRangeType == kORTSingleMonad) {
				first_feature_column_index = 2;
			} else if (objectRangeType == kORTSingleRange) {
				first_feature_column_index = 3;
			} else {
				first_feature_column_index = 2;
			}
			while (bMoreRows) {
				// Declare this pFeature_values array of dummy EMdFValue objects.
				EMdFValue *pFeature_values;
				if (no_of_features_to_get > 0) {
					// This uses the () constructor, which gives us a kEVInt with 
					// value 0.
					pFeature_values = (EMdFValue *) Result.allocate(sizeof(EMdFValue)*no_of_features_to_get);
				} else {
					pFeature_values = 0;
				}

				// Get object id_d
				id_d_t oid;
				if (!pConn->accessTuple(0, oid)) {
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getInst");
					return false;
				}

				// Get monads 
				SetOfMonads *pSom = 0; 
				monad_m mse_first, mse_last;
				if (bMonadSetIsFeature
				    || objectRangeType == kORTMultipleRange) {
					std::string monads_string;
					if (!pConn->accessTuple(1, monads_string)) {
						DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getInst");
						return false;
					}
					pSom = new SetOfMonads(monads_string);
					mse_first = pSom->first();
					mse_last = pSom->last();
				} else {
					// Get mse_first
					if (!pConn->accessTuple(1, mse_first)) {
						DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getInst");
						return false;
					}

					// Get mse_last
					if (objectRangeType == kORTSingleMonad) {
						/* If this iw WITH SINGLE MONAD OBJECTS, then we don't 
						 * retrive mse_last.  Instead, we just get it
						 * from mse_first.
						 */
						mse_last = mse_first;
					} else {
						if (!pConn->accessTuple(2, mse_last)) {
							DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getInst");
							return false;
						}
					}
				}

				// Get feature values
				if (no_of_features_to_get > 0) {
					if (!getFeatureValues(no_of_features_to_get,
							      objectRangeType,
							      first_feature_column_index,
							      feature_types_vec,
							      feature_names_vec,
							      string_set_caches_vec,
							      pFeature_values)) {
						DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getInst");
						pConn->finalize();
						delete pSom;
						return false;
					}
				}
				
				// Add if we are using monad constraints
				// or if first_monad / last_monad describe
				// all_m-1.
				// 
				// If we are not using monad constraints,
				// but this is because the monad constraints
				// would coincide with all_m-1, then we add regardless,
				// since we know that all objects are going to be
				// within the boundaries.
				bool bDoAdd = bUseMonadConstraints || bFirstLastMonadAreMinMMaxM;
				if (!bDoAdd) {
					// If we are not using monad constraints,
					// and if boundaries are not equal to all_m-1,
					// only add if object's boundaries are within
					// the Su boundaries which we got in our
					// first_monad and last_monad method parameters.
					//
					// This check is carried out because
					// if we are not using monad contraints,
					// we might get extraneous objects which would be
					// wasteful to process in mql_R (the part_of Su constraint
					// would not hold.) Hence it is easier to discard them here.
				    
					switch (ms_operation) {
					  case kMSROPartOf:
						bDoAdd = mse_last <= last_monad 
							&& mse_first >= first_monad;
						break;

					  case kMSROStartsIn:
						bDoAdd = mse_first <= last_monad 
							&& mse_first >= first_monad;
						break;

					  default:
						ASSERT_THROW(false, "Unknown monad set operation in EMdFDB::getInst.");
						break;
					}
				}
				
				if (bDoAdd) {
					// Make new object
					// First allocate using Result Inst's allocator
					pObj = (InstObject*) Result.allocate(sizeof(InstObject));
					if (objectRangeType == kORTMultipleRange) {
						if (pSom->hasOnlyOneMSE()) {
							// Then use placement new operator (p. 255 in TC++PL 3rd ed.)
							pObj = new(pObj) InstObject(oid, 
										    mse_first, mse_last,
										    pFeature_values, 
										    no_of_features_to_get); 
							delete pSom;
						} else {
							// Then use placement new operator (p. 255 in TC++PL 3rd ed.)
							pObj = new(pObj) InstObject(oid, 
										    pSom, 
										    pFeature_values, 
										    no_of_features_to_get,
										    false); 
						} 
					} else {
						// Then use placement new operator (p. 255 in TC++PL 3rd ed.)
						pObj = new(pObj) InstObject(oid, 
									    mse_first, mse_last,
									    pFeature_values, 
									    no_of_features_to_get); 
					}
					
					// Insert object in inst
					Result.addObject(pObj, mse_first, mse_last);
				} else {
					delete pSom;
				}
				
				
				// Get next tuple
				if (!pConn->getNextTuple(bMoreRows)) {
					DEBUG_GET_NEXT_TUPLE_FAILED("EMdFDB::getInst");
					return false;
				}
			} // end of iteration over query-results
		} catch (EMdFNULLValueException e) {
			DEBUG_NULL_VALUE_EXCEPTION("EMdFDB::getInst");
			// There was a NULL value exception, hence we return an error
			return false;
		}
		
		// Write "retrieval finished" to log
		LOG_WRITE_TIME("EMdFDB::getInst", "Retrieval finished.");
		
		// Release results
		pConn->finalize();
		
		// finalize Result
		Result.fillingIsDone();
		
		// If we got this far, the command was as success.
		return true;
	}
}



/** Get the objects starting at a given monad Sm
 *@internal
 *
 * @param object_type_name The name of the object type which holds the
 * objects to query.
 *
 * @param Sm the monad at which to find the objects
 *
 * @param Result the IDDList which will contain the id_ds of the
 * objects at monad Sm.
 *
 * @return true on no DB error, false otherwise.
 */
bool EMdFDB::getObjectsStartingAtSm(const std::string& object_type_name,
				    monad_m Sm,
				    /* out */ IDDList& Result)
{
	if (pConn == 0)
		return false;
	else {
		// Normalize object type name
		std::string OTN = normalizeOTName(object_type_name);

		// Build query
		std::ostringstream query_stream;
		query_stream
			<< "SELECT object_id_d\n"
			<< "FROM " << normalizeTableName(OTN + "_objects", true) << "\n"
			<< "WHERE first_monad = " << long2string(Sm);

		// Execute query
		if (!pConn->execSelect(query_stream.str())) {
			DEBUG_SELECT_QUERY_FAILED("EMdFDB::getObjectsStartingAtSm", query_stream.str());
			return false;
		}
      
		try {
			bool bMoreRows = pConn->hasRow();
			while (bMoreRows) {
				id_d_t oid;
				if (!pConn->accessTuple(0, oid)) {
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getObjectsStartingAtSm");
					return false;
				} else {
					Result.addOid(oid);
				}

				// Get next tuple
				if (!pConn->getNextTuple(bMoreRows)) {
					DEBUG_GET_NEXT_TUPLE_FAILED("EMdFDB::getObjectsStartingAtSm");
					return false;
				}
			}
		} catch (EMdFNULLValueException e) {
			DEBUG_NULL_VALUE_EXCEPTION("EMdFDB::getObjectsStartingAtSm");
			// There was a NULL value exception, hence we return an error
			return false;
		}
    
		// Release results
		pConn->finalize();

		// If we got this far, the command was as success.
		return true;
	}
}

//
// Gets the set of monads making up the object 
// with id_d object_id_d
//
/** Get the monads of an object with a given id_d
 *@internal
 *
 * @param object_id_d the id_d of the object to query
 *
 * @param object_type_id the object type id of the object type holding
 * the object.
 *
 * @param monads The SetOfMonads which will hold the object's monads.
 *
 * @return true on no DB error, false otherwise.
 */
bool EMdFDB::getMonadsFromID_D(id_d_t object_id_d, 
			       id_d_t object_type_id,
			       /* out */ SetOfMonads& monads)
{
	// Get object type name
	std::string object_type_name;
	bool bObjectTypeExists;
	eObjectRangeType objectRangeType;
	eMonadUniquenessType monadUniquenessType;
	if (!objectTypeExists(object_type_id, 
			      bObjectTypeExists, 
			      object_type_name, 
			      objectRangeType,
			      monadUniquenessType)) {
		DEBUG_X_IS_WRONG("getMonadsFromID_D", 
				 "Could not get object type name for object id_d " 
				 + id_d2string(object_id_d) + ".\n");
		return false;
	}
	if (!bObjectTypeExists) {
		DEBUG_X_IS_WRONG("getMonadsFromID_D", 
				 "Object type for object with object id_d " 
				 + id_d2string(object_id_d) + " does not exist.\n");
		return true;
	}

	// Get set of monads
	if (!getSOMForObject(object_type_name, 
			     object_id_d, 
			     objectRangeType, 
			     monads)) {
		DEBUG_X_IS_WRONG("getMonadsFromID_D", 
				 "Could not getSOMForObject for object with object id_d " 
				 + id_d2string(object_id_d) + ".\n");
		return false;
	}

	// If we got this far, there were no errors
	return true;
}

/** Get the monads of objects with given id_ds.
 *
 * @internal
 *
 * Used to implement GET MONADS.
 *
 * @param id_ds_set a "SetOfMonads" actually holding id_ds of objects
 * from the same object type.
 *
 * @param object_type_name the name of the object type holding the
 * objects.
 *
 * @param objectRangeType the eObjectRangeType of the object type
 * specified in object_type_name.
 *
 * @param pOutTable the table to hold the monads.  This has the
 * signature documented in the MQL User's Guide for GET MONADS.
 *
 * @return true on no DB error, false otherwise.
 */
bool EMdFDB::getMonadsFromID_Ds(const SetOfMonads& id_ds_set,
				const std::string &object_type_name,
				eObjectRangeType objectRangeType,
				Table *pOutTable)
{
	if (pConn == 0)
		return false;
	else {
		// Normalize object type name
		std::string OTN = normalizeOTName(object_type_name);

		// Decide mse_first/mse_last or first_monad/last_monad
		std::string firstm, lastm;
		bool2firstm_lastm(objectRangeType, firstm, lastm);
    
		// Build first part of query
		std::ostringstream query_stream_select_from;
		query_stream_select_from << "SELECT object_id_d, ";
		if (objectRangeType == kORTMultipleRange) {
			query_stream_select_from << "monads";
		} else {
			query_stream_select_from << firstm;
			if (objectRangeType == kORTSingleRange) {
				query_stream_select_from << ", " << lastm;
			}
		}
		query_stream_select_from << "\n"
					 << "FROM " << normalizeTableName(OTN + "_objects", true) << "\n";

/** The max number of ID_Ds to have before issuing a query. 
 * @internal
 */
#define WHERE_MAX_ID_DS (200) 
		SOMConstIterator id_dci = id_ds_set.const_iterator();
		while (id_dci.hasNext()) {
			// Build where-part
			std::ostringstream query_stream_where;
			query_stream_where << "WHERE ";

			for (int i = 0; (i < WHERE_MAX_ID_DS) && (id_dci.hasNext()); ++i) {
				MonadSetElement mse = id_dci.next();
				if (mse.first() == mse.last()) {
					query_stream_where << "object_id_d = " 
							   << long2string(mse.first());
				} else {
					query_stream_where << "(object_id_d>= " 
							   << long2string(mse.first())
							   << " AND object_id_d<=" << long2string(mse.last())
							   << ")";
				}
				if (id_dci.hasNext() && i < (WHERE_MAX_ID_DS-1)) {
					query_stream_where << "\n OR ";
				} else {
					query_stream_where << '\n';
				}
			}
#undef WHERE_MAX_ID_DS

			// Build query-string
			std::string query(query_stream_select_from.str());
			query += query_stream_where.str();
			query += "\nORDER BY object_id_d";

			// Execute query
			if (!pConn->execSelect(query)) {
				DEBUG_SELECT_QUERY_FAILED("EMdFDB::getMonadsFromID_Ds", query);
				return false;
			}

			// Build table
			id_d_t oid;
			monad_m mse_first, mse_last;
			try {
				bool bMoreRows = pConn->hasRow();
				while (bMoreRows) {
					bool bAccessSuccess = pConn->accessTuple(0, oid);
					SetOfMonads som;
					if (bAccessSuccess) {
						if (objectRangeType == kORTMultipleRange) {
							std::string monads_string;
							bAccessSuccess = pConn->accessTuple(1, monads_string);
							if (!bAccessSuccess) {
								DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getMonadsFromID_Ds");
								return false;
							}
							som.fromCompactString(monads_string);
						} else {
							bAccessSuccess = pConn->accessTuple(1, mse_first);
							if (bAccessSuccess) {
								if (objectRangeType == kORTSingleMonad) {
									mse_last = mse_first;
								} else {
									bAccessSuccess = pConn->accessTuple(2, mse_last);
								}
							}
						}
					}
	  
					if (bAccessSuccess) {
						if (objectRangeType == kORTMultipleRange) {
							SOMConstIterator ci = som.const_iterator();
							while (ci.hasNext()) {
								MonadSetElement mse = ci.next();
								pOutTable->startNewRow();
								pOutTable->append(id_d2string(oid));
								pOutTable->append(monad_m2string(mse.first()));
								pOutTable->append(monad_m2string(mse.last()));
							}
						} else {
							pOutTable->startNewRow();
							pOutTable->append(id_d2string(oid));
							pOutTable->append(monad_m2string(mse_first));
							pOutTable->append(monad_m2string(mse_last));
						}
					} else {
						DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getMonadsFromID_Ds");
						// Return failure
						return false;
					}

					// Get next tuple
					if (!pConn->getNextTuple(bMoreRows)) {
						DEBUG_GET_NEXT_TUPLE_FAILED("EMdFDB::getMonadsFromID_Ds");
						return false;
					}
				}
			} catch (EMdFNULLValueException e) {
				DEBUG_NULL_VALUE_EXCEPTION("EMdFDB::getMonadsFromID_Ds");
				// There was a NULL value exception, hence we return an error
				return false;
			}

		}
    
		// Release results
		pConn->finalize();

		// If we got this far, the command was as success.
		return true;
	}
}


//
// Get set of monads from object_id_d belonging to object type
// with name object_type_name
//
/** Get monads from an object
 *@internal
 *
 * This is similar to EMdFDB::getMonadsFromID_D(), but that method has
 * fewer parameters, and actulaly calls this method.
 *
 * The rationale for having both is that EMdFDB::getMonadsFromID_D()
 * is useful in situations where we do not know all the information
 * that this method needs.
 *
 * @param object_type_name the name of the object type holding the
 * object.
 *
 * @param object_id_d the id_d of the object to query
 *
 * @param objectRangeType the eObjectRangeType of the object type
 * specified with \p object_type_name.
 * 
 * @param Result The SetOfMonads which will hold the object's monads.
 *
 * @return true on no DB error, false otherwise.
 */
bool EMdFDB::getSOMForObject(const std::string& object_type_name,
			     id_d_t object_id_d,
			     eObjectRangeType objectRangeType,
			     /* out */ SetOfMonads& Result)
{
	if (pConn == 0)
		return false;
	else {
		// Normalize object type name
		std::string OTN = normalizeOTName(object_type_name);

		// Decide mse_first/mse_last or first_monad/last_monad
		std::string firstm, lastm;
		bool2firstm_lastm(objectRangeType, firstm, lastm);
    
		// Build first part of query
		std::ostringstream query_stream;
		query_stream << "SELECT object_id_d, ";
		if (objectRangeType == kORTMultipleRange) {
			query_stream << "monads";
		} else {
			query_stream << firstm;
			if (objectRangeType == kORTSingleRange) {
				query_stream << ", " << lastm;
			}
		}
		query_stream
			<< "\n"
			<< "FROM " << normalizeTableName(OTN + "_objects", true) << "\n"
			<< "WHERE object_id_d = " << long2string(object_id_d);
		if (!pConn->execSelect(query_stream.str())) {
			// Possibly emit debug message
			DEBUG_SELECT_QUERY_FAILED("EMdFDB::getSOMForObject", query_stream.str());
			// Return failure
			return false;
		}

		// Build table
		id_d_t oid;
		monad_m mse_first, mse_last;
		try {
			bool bMoreRows = pConn->hasRow();
			if (bMoreRows) {
				bool bAccessSuccess = pConn->accessTuple(0, oid);
				if (bAccessSuccess) {
					if (objectRangeType == kORTMultipleRange) {
						std::string monads_string;
						bAccessSuccess = pConn->accessTuple(1, monads_string);
						if (!bAccessSuccess) {
							DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getSOMForObject");
							return false;
						}
						Result.fromCompactString(monads_string);
					} else {
						bAccessSuccess = pConn->accessTuple(1, mse_first);
						if (bAccessSuccess) {
							if (objectRangeType == kORTSingleMonad) {
								mse_last = mse_first;
							} else {
								bAccessSuccess = pConn->accessTuple(2, mse_last);
							}
						}
					}
				}
	
				if (bAccessSuccess) {
					if (objectRangeType == kORTMultipleRange) {
						; // Do nothing; it has already been done
					} else {
						Result.add(mse_first, mse_last);
					}
				} else {
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getSOMForObject");
					// Return failure
					return false;
				}
			}
		} catch (EMdFNULLValueException e) {
			DEBUG_NULL_VALUE_EXCEPTION("EMdFDB::getSOMForObject");
			// There was a NULL value exception, hence we return an error
			return false;
		}
     
		// Release results
		pConn->finalize();

		// If we got this far, the command was as success.
		return true;
	}
}

/** Get a list of the object types in the database
 *@internal
 *
 * @param Result the list of the object type names.  The object type
 * names are as the user would see them.
 *
 * @return true on no DB error, false otherwise.
 */
bool EMdFDB::getObjectTypes(/* out */ std::list<std::string>& Result)
{
	if (pConn == 0)
		return false;
	else {
		// Build and execute query
		std::ostringstream query_stream;
		query_stream 
			<< "SELECT object_type_name\n"
			<< "FROM object_types";
		if (!pConn->execSelect(query_stream.str())) {
			DEBUG_SELECT_QUERY_FAILED("EMdFDB::getObjectTypes", query_stream.str());
			return false;
		}

		try {
			bool bMoreRows = pConn->hasRow();
			while (bMoreRows) {
				std::string str;
				if (pConn->accessTuple(0, str)) {
					Result.push_back(str);
				} else {
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getObjectTypes");
					// Return failure
					return false;
				}

				// Get next tuple
				if (!pConn->getNextTuple(bMoreRows)) {
					DEBUG_GET_NEXT_TUPLE_FAILED("EMdFDB::getObjectTypes");
					return false;
				}
			}
		} catch (EMdFNULLValueException e) {
			DEBUG_NULL_VALUE_EXCEPTION("EMdFDB::getObjectTypes");
			// There was a NULL value exception, hence we return an error
			return false;
		}
    
		// Release results
		pConn->finalize();

		// If we got this far, the command was as success.
		return true;
	}
}


/** Get features for an object type.
 *@internal
 *
 * Returns a list of the features of an object type.  Also adds them
 * to the cache if not already there.
 *
 * This function is part of an overall invariant which says that if
 * there is one feature for a given object type id in the feature
 * cache, then they are all there.  This must be maintained by calling
 * this function before adding a feature to the feature cache.
 *
 * @param object_type_id the id_d of the object type to query
 *
 * @param Result the list of FeatureInfo objects describing the
 * features of the object type.
 *
 * @return true on no DB error, false otherwise.
 */
bool EMdFDB::getFeaturesForObjectType(id_d_t object_type_id,
				      /* out */ std::list<FeatureInfo>& Result)
{
	if (pConn == 0)
		return false;
	else {
		//
		// First try the cache
		//

		// Make object_type_id to string for later comparison
		std::string str_oid = id_d2string(object_type_id);

		// Assume we didn't find any.
		bool bReturnSuccess = false;

		// Go through feature cache
		TableIterator i = m_feature_cache->iterator();
		while (i.hasNext()) {
			// Does this feature in the feature cache
			// have the same object type id as the one
			// we are looking for?
			if (str_oid == m_feature_cache->getColumn(i, 1)) {
				// Get info
				std::string feature_name = m_feature_cache->getColumn(i, 2);
				id_d_t feature_type_id = string2id_d(m_feature_cache->getColumn(i, 3));
				std::string default_value = m_feature_cache->getColumn(i, 4);
				bool is_computed = string2bool(m_feature_cache->getColumn(i, 5));
	
				// Make FeatureInfo
				FeatureInfo f_info(feature_name, feature_type_id, 
						   default_value, is_computed);

				// Include it in result
				Result.push_back(f_info);

				// At least we found one that belonged to the object type.
				// Se let's assume we have found them all.
				bReturnSuccess = true;
			}

			// Advance iterator
			i.next();
		}

		// If we found at least one, return success.
		if (bReturnSuccess) 
			return true;

		//
		// Otherwise, look in database
		//

		// Build and execute query
		std::ostringstream query_stream;
		query_stream
			<< "SELECT feature_name, feature_type_id, default_value, computed\n"
			<< "FROM features\n"
			<< "WHERE object_type_id = " << long2string(object_type_id)
			<< "\nORDER BY feature_type_id";
		if (!pConn->execSelect(query_stream.str())) {
			DEBUG_SELECT_QUERY_FAILED("EMdFDB::getFeaturesForObjectType", query_stream.str());
			return false;
		}

		try {
			bool bMoreRows = pConn->hasRow();
			while (bMoreRows) {
				std::string name, default_value;
				id_d_t feature_type;
				bool is_computed;

				/* Get name */
				std::string temp_name;
				if (!pConn->accessTuple(0, temp_name)) { 
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getFeaturesForObjectType");
					// Return failure
					return false;
				}
				name = decodeFeatureName(temp_name);

				/* Get type */
				if (!pConn->accessTuple(1, feature_type)) { 
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getFeaturesForObjectType");
					// Return failure
					return false;
				}

				/* Get default value */
				std::string defval;
				if (!pConn->accessTuple(2, default_value)) { 
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getFeaturesForObjectType");
					// Return failure
					return false;
				}

				/* Get is_computed */
				if (!pConn->accessTuple(3, is_computed)) { 
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getFeaturesForObjectType");
					// Return failure
					return false;
				}

				/* Construct FeatureInfo and push on the back. */
				FeatureInfo f_info(name, feature_type, default_value, is_computed);
				Result.push_back(f_info);

				// Add it to the feature cache if not already there.
				addFeatureToCacheIfNotAlreadyThere(object_type_id, 
								   name, 
								   feature_type, 
								   default_value, 
								   is_computed);

				// Get next tuple
				if (!pConn->getNextTuple(bMoreRows)) {
					DEBUG_GET_NEXT_TUPLE_FAILED("EMdFDB::getFeaturesForObjectType");
					return false;
				}
			}
		} catch (EMdFNULLValueException e) {
			DEBUG_NULL_VALUE_EXCEPTION("EMdFDB::getFeaturesForObjectType");
			// There was a NULL value exception, hence we return an error
			return false;
		}
    
		// If we got this far, the command was as success.
		return true;
	}
}

/** Get enumerations in the database
 *@internal
 *
 * Returns a list of the enumerations in the database.
 *
 * @param Result the list to contain enumeration names in the
 * database.
 *
 * @return true on no DB error, false otherwise.
 */
bool EMdFDB::getEnumerations(/* out */ std::list<std::string>& Result)
{
	if (pConn == 0)
		return false;
	else {
		// Build and execute query
		std::string query = 
			"SELECT enum_name\n"
			"FROM enumerations";

		if (!pConn->execSelect(query)) {
			// Possibly emit debug message
			DEBUG_SELECT_QUERY_FAILED("EMdFDB::getEnumerations", query);
			// Return failure
			return false;
		}

		try {
			bool bMoreRows = pConn->hasRow();
			while (bMoreRows) {
				std::string enum_name;

				/* Get enumeration name. */
				if (!pConn->accessTuple(0, enum_name)) { 
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getEnumerations");
					// Return failure
					return false;
				}

				/* Put it at the back of the list. */
				Result.push_back(enum_name);

				// Get next tuple
				if (!pConn->getNextTuple(bMoreRows)) {
					DEBUG_GET_NEXT_TUPLE_FAILED("EMdFDB::getEnumerations");
					return false;
				}
			}
		} catch (EMdFNULLValueException e) {
			DEBUG_NULL_VALUE_EXCEPTION("EMdFDB::getEnumerations");
			// There was a NULL value exception, hence we return an error
			return false;
		}
    
		// Release results
		pConn->finalize();
    
		// If we got this far, the command was as success.
		return true;
	}
}

/** Load enumeration constants into cache
 *
 * Loads enumeration constants for a given enumeration into the cache.
 *
 * @param enum_id the id_d of the enumeration to query.
 *
 * @return true on no DB error, false otherwise.
 */
bool EMdFDB::loadEnumConstantsIntoCache(id_d_t enum_id)
{
	if (pConn == 0)
		return false;
	else {
		// Strip the lower bits off that aren't part of the enum_id
		enum_id = STRIP_ENUM_ID_OF_LOWER_BITS(enum_id);

		// Get enum name
		std::string enum_name;
		bool bEnumExists;
		if (!enumExists(enum_id, bEnumExists, enum_name)) {
			DEBUG_X_FAILED("loadEnumConstantsIntoCache", "Getting enumeration name.");
			return false;
		}
		if (!bEnumExists) {
			DEBUG_X_IS_WRONG("loadEnumConstantsIntoCache", "Enum did not exist.");
			return false;
		}

		// Build and execute query
		std::ostringstream query_stream;
		query_stream
			<< "SELECT enum_value_name, value, is_default\n"
			<< "FROM enumeration_constants\n"
			<< "WHERE enum_id = " << long2string(enum_id);
		if (!pConn->execSelect(query_stream.str())) {
			// Possibly emit debug message
			DEBUG_SELECT_QUERY_FAILED("EMdFDB::loadEnumConstantsIntoCache", query_stream.str());
			// Return failue
			return false;
		}
    
		try {
			bool bMoreRows = pConn->hasRow();
			while (bMoreRows) {
				std::string name;
				long value;
				bool is_default;
      
				/* Get name. */
				if (!pConn->accessTuple(0, name)) { 
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::loadEnumConstantsIntoCache");
					// Return failue
					return false;
				}

				/* Get name. */
				if (!pConn->accessTuple(1, value)) { 
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::loadEnumConstantsIntoCache");
					// Return failue
					return false;
				}

				/* Get name. */
				if (!pConn->accessTuple(2, is_default)) { 
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::loadEnumConstantsIntoCache");
					// Return failue
					return false;
				}

				// Add to cache
				addEnumConstToCache(enum_id, enum_name, name, value, is_default);

				// Get next tuple
				if (!pConn->getNextTuple(bMoreRows)) {
					DEBUG_GET_NEXT_TUPLE_FAILED("EMdFDB::loadEnumConstantsIntoCache");
					return false;
				}
			}
		} catch (EMdFNULLValueException e) {
			DEBUG_NULL_VALUE_EXCEPTION("EMdFDB::loadEnumConstantsIntoCache");
			// There was a NULL value exception, hence we return an error
			return false;
		}
    
		// Release results
		pConn->finalize();
	  
		// If we got this far, the command was as success.
		return true;
	}
}

/** Get enumeration constants from a given enumeration
 *@internal
 *
 * @param enum_id the id_d of the enumeration to query.
 *
 * @param Result the list to contain the enumeration constant infos
 * (EnumConstInfo) for the enum.
 *
 * @return true on no DB error, false otherwise.
 */
bool EMdFDB::getEnumConstants(id_d_t enum_id,
			      /* out */ std::list<EnumConstInfo>& Result)
{
	if (pConn == 0)
		return false;
	else {
		// Strip the lower bits off that aren't part of the enum_id
		enum_id = STRIP_ENUM_ID_OF_LOWER_BITS(enum_id);

		// Get enum name
		std::string enum_name;
		bool bEnumExists;
		if (!enumExists(enum_id, bEnumExists, enum_name)) {
			DEBUG_X_FAILED("getEnumConstants", "Getting enumeration name.");
			return false;
		}
		if (!bEnumExists) {
			DEBUG_X_IS_WRONG("getEnumConstants", "Enum did not exist.");
			return false;
		}

		// Build and execute query
		std::ostringstream query_stream;
		query_stream
			<< "SELECT enum_value_name, value, is_default\n"
			<< "FROM enumeration_constants\n"
			<< "WHERE enum_id = " << long2string(enum_id)
			<< "\nORDER BY enum_value_name";
		if (!pConn->execSelect(query_stream.str())) {
			// Possibly emit debug message
			DEBUG_SELECT_QUERY_FAILED("EMdFDB::getEnumConstants", query_stream.str());
			// Return failue
			return false;
		}
    
		try {
			bool bMoreRows = pConn->hasRow();
			while (bMoreRows) {
				std::string name;
				long value;
				bool is_default;
      
				/* Get name. */
				if (!pConn->accessTuple(0, name)) { 
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getEnumConstants");
					// Return failue
					return false;
				}

				/* Get name. */
				if (!pConn->accessTuple(1, value)) { 
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getEnumConstants");
					// Return failue
					return false;
				}

				/* Get name. */
				if (!pConn->accessTuple(2, is_default)) { 
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getEnumConstants");
					// Return failue
					return false;
				}

				// Put it at the back of the list. 
				EnumConstInfo ec_info(name, value, is_default);
				Result.push_back(ec_info);

				// Add to cache
				addEnumConstToCache(enum_id, enum_name, name, value, is_default);

				// Get next tuple
				if (!pConn->getNextTuple(bMoreRows)) {
					DEBUG_GET_NEXT_TUPLE_FAILED("EMdFDB::getEnumConstants");
					return false;
				}
			}
		} catch (EMdFNULLValueException e) {
			DEBUG_NULL_VALUE_EXCEPTION("EMdFDB::getEnumConstants");
			// There was a NULL value exception, hence we return an error
			return false;
		}
    
		// Release results
		pConn->finalize();
	  
		// If we got this far, the command was as success.
		return true;
	}
}


/** Get object types which use an enumeration
 *@internal
 *
 * Gets a list of the names of the object types that use a given
 * enumeration for some feature.
 *
 * @param enum_id the id_d of the enumeration to query.
 *
 * @param Result the list to contain the names of the object types
 * that use the enumeration.
 *
 * @return true on no DB error, false otherwise.
 */
bool EMdFDB::getObjectTypesUsingEnumeration(id_d_t enum_id,
					    /* out */ std::list<std::string>& Result)
{
	if (pConn == 0)
		return false;
	else {
		// Strip the lower bits off that aren't part of the enum_id
		id_d_t enum_feature_id = STRIP_ENUM_ID_OF_LOWER_BITS(enum_id)
			| FEATURE_TYPE_ENUM;
		id_d_t list_of_enum_feature_id = STRIP_ENUM_ID_OF_LOWER_BITS(enum_id)
			| FEATURE_TYPE_LIST_OF_ENUM;


		std::ostringstream query_stream;

		// Build and execute query
		query_stream
			<< "SELECT DISTINCT OT.object_type_name\n"
			<< "FROM object_types OT, features F\n"
			<< "WHERE OT.object_type_id = F.object_type_id \n"
			<< "      AND (F.feature_type_id = " << long2string(enum_feature_id) << "\n"
			<< "           OR F.feature_type_id = " << long2string(list_of_enum_feature_id) << ")";
		if (!pConn->execSelect(query_stream.str())) {
			DEBUG_SELECT_QUERY_FAILED("EMdFDB::getObjectTypesUsingEnumeration", query_stream.str());
			return false;
		}

		try {
			bool bMoreRows = pConn->hasRow();
			while (bMoreRows) {
				std::string name;

				/* Get name. */
				if (!pConn->accessTuple(0, name)) { 
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getObjectTypesUsingEnumeration");
					// Return failue
					return false;
				}

				/* Push it at the back. */
				Result.push_back(name);

				// Get next tuple
				if (!pConn->getNextTuple(bMoreRows)) {
					DEBUG_GET_NEXT_TUPLE_FAILED("EMdFDB::getObjectTypesUsingEnumeration");
					return false;
				}
			}
		} catch (EMdFNULLValueException e) {
			DEBUG_NULL_VALUE_EXCEPTION("EMdFDB::getObjectTypesUsingEnumeration");
			// There was a NULL value exception, hence we return an error
			return false;
		}
    
		// Release results
		pConn->finalize();

		// If we got this far, the command was as success.
		return true;
	}
}

/** Get objects that are wholly contained within a set of monads.
 *@internal
 *
 * This is different from EMdFDB::getObjectsHavingMonadsInMonadSet()
 * in that for this method, the objects have to be wholly contained in
 * the monad set and not just overlap with it.
 *
 * @param monads the set of monads within which to search.
 *
 * @param object_type_name the name of the object type holding the
 * objects to query.
 *
 * @param objectRangeType the eObjectRangeType of the object type
 * specified with object_type_name.
 *
 * @param result the list to hold the id_ds of the objects wholly
 * cntained within \p monads.
 *
 * @return true on no DB error, false otherwise.
 */
bool EMdFDB::getObjectsWithinMonads(const SetOfMonads& monads, 
				    const std::string& object_type_name,
				    eObjectRangeType objectRangeType,
				    std::list<id_d_t>& result)
{
	if (pConn == 0)
		return false;
	else {
		// Make sure we are pristine
		result.clear();
    
		// Return if monads are empty.
		if (monads.isEmpty())
			return true;

		// Normalize object type name
		std::string OTN = normalizeOTName(object_type_name);

		// Make monad constraints
		std::string monad_constraints = makeMonadConstraintsFromSOM(monads, objectRangeType);

		// Make query for selecting object ids 
		std::string query =
			std::string(" SELECT object_id_d\n") 
			+ "FROM " + normalizeTableName(OTN + "_objects", true) + "\n"
			+ "WHERE (" + monad_constraints + ")";
		if (!pConn->execSelect(query)) {
			DEBUG_SELECT_QUERY_FAILED("EMdFDB::getObjectsWithinMonads", query);
			return false;
		}

		try {
			bool bMoreRows = pConn->hasRow();
			while (bMoreRows) {
				id_d_t oid;
				if (!pConn->accessTuple(0, oid)) { 
					DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getObjectsWithinMonads");

					// Make sure we don't return any results
					result.clear();

					// Return failure
					return false;
				}
				result.push_back(oid);

				// Get next tuple
				if (!pConn->getNextTuple(bMoreRows)) {
					DEBUG_GET_NEXT_TUPLE_FAILED("EMdFDB::getObjectsWithinMonads");
					return false;
				}
			}
		} catch (EMdFNULLValueException e) {
			DEBUG_NULL_VALUE_EXCEPTION("EMdFDB::getObjectsWithinMonads");
			// There was a NULL value exception, hence we return an error
			return false;
		}

		// Release results
		pConn->finalize();

		// If we got this far, the command was as success.
		return true;
	}
}


/** Checks whether an object id_d exists within a given object type.
 *@internal
 *
 * @param object_id_d the id_d to check
 *
 * @param object_type_name the name of the object type to check
 *
 * @param Result returns false if not there, true if it exists in the
 * object type.  Note how this is different from the return value:
 * This value is only valid if the method returns true (no DB error),
 * whereas here true means "the object id existed in the object type".
 *
 * @return true on no DB error, false otherwise.
 */
bool EMdFDB::objectID_DExistsInType(id_d_t object_id_d, 
				    const std::string& object_type_name,
				    /* out */ bool& Result)
{
	if (pConn == 0)
		return false;
	else {
		// Normalize object type name
		std::string OTN = normalizeOTName(object_type_name);

		// Build and execute query
		std::ostringstream query_stream;
		query_stream
			<< "SELECT object_id_d\n" 
			<< "FROM " << normalizeTableName(OTN + "_objects", true) << "\n" //     
			<< "WHERE object_id_d = " << long2string(object_id_d);
		if (!pConn->execSelect(query_stream.str())) {
			DEBUG_SELECT_QUERY_FAILED("EMdFDB::objectID_DExistsInType", query_stream.str());
			return false;
		}
    
		/* If there was a non-zero number of tuples, the object
		 * exists. */
		Result = pConn->hasRow();

		// Clean up after the select
		pConn->finalize();

		// If we got this far, the command was as success.
		return true;
	}
}

/** Checks whether an object id_d exists within any object type.
 *@internal
 *
 * @param object_id_d the id_d to check
 *
 * @param Result returns false if not there, true if it exists in the
 * object type.  Note how this is different from the return value:
 * This value is only valid if the method returns true (no DB error),
 * whereas here true means "the object id existed in the object type".
 *
 * @param object_type_id if Result is true, then this will be used to
 * return the object type id of the object type in which the id_d
 * existed.
 *
 * @return true on no DB error, false otherwise.
 */
bool EMdFDB::objectID_DExists(id_d_t object_id_d, 
			      /* out */ bool& Result,
			      /* out */ id_d_t& object_type_id)
{
	if (pConn == 0)
		return false;
	else {
		// Assume we didn't find it
		Result = false;

		// Get object type names
		std::list<std::string> object_type_names;
		getObjectTypes(object_type_names);

		// Iterate through object type names
		std::list<std::string>::const_iterator ci(object_type_names.begin());
		std::list<std::string>::const_iterator cend(object_type_names.end());
		while (!Result && ci != cend) {
			// Find object type id and whether it has single range objects
			bool bObjectTypeExists;
			eObjectRangeType objectRangeType;
			eMonadUniquenessType monadUniquenessType;
			if (!objectTypeExists(*ci, 
					      bObjectTypeExists, object_type_id,
					      objectRangeType,
					      monadUniquenessType)) {
				DEBUG_X_FAILED("EMdFDB::objectID_DExists", "getting object type");
				return false;
			}

			// See if it exists in this type
			bool bObjectExistsInType;
			if (!objectID_DExistsInType(object_id_d, 
						    *ci,
						    bObjectExistsInType))
				return false;

			// We found it
			if (bObjectExistsInType) {
				// Tell caller we found it
				Result = true;

				// Since we found it, we can return now.
				return true;
			}

			// Advance iterator
			++ci;
		}
		// If we got this far, the command was as success.
		return true;
	}
}

/** Create an object
 *@internal
 *
 * @param object_id_d the id_d of the object to create.
 *
 * @param object_type_name the object type name of the OT to hold the
 * object.
 *
 * @param object_type_id The id_d of the object type. Must be the same 
 *        object type as \p object_type_name.
 *
 * @param monads the monads of the object
 *
 * @param objectRangeType the eObjectRangeType of the object type
 * indicated with \e object_type_name .
 *
 * @param features a list of FeatureInfo objects describing the
 * features of the object.  Must be the full set of features for the
 * object type.
 *
 * @return true on no DB error, false otherwise.
 */
bool EMdFDB::createObject(id_d_t object_id_d,
			  const std::string& object_type_name,
			  id_d_t object_type_id,
			  const SetOfMonads& monads,
			  eObjectRangeType objectRangeType,
			  const std::list<FeatureInfo>& features)
{
	if (pConn == 0)
		return false;
	else {
		// Begin transaction
		bool bDoCommit;
		bDoCommit = pConn->beginTransaction();

		// Normalize object type name
		std::string OTN = normalizeOTName(object_type_name);

		try {
			//
			// Insert in 'OT_objects' 

			// Build query
			std::ostringstream strQuery;
			strQuery << "INSERT INTO "
				 << normalizeTableName(OTN + "_objects", true) << " "
				 << "(object_id_d, first_monad";
			if (objectRangeType != kORTSingleMonad) {
				strQuery <<  ",last_monad";
			}
			if (objectRangeType == kORTMultipleRange) {
				strQuery << ",monads";
			}
    
			// Add names of features to be added
			std::list<FeatureInfo>::const_iterator fi(features.begin());
			std::list<FeatureInfo>::const_iterator fiend(features.end());
			while (fi != fiend) {
				if (!fi->getIsComputed()) {
					strQuery << "," << encodeFeatureName(fi->getName());
					if (featureTypeIdIsSOM(fi->getType())) {
						strQuery << ",first_monad_" + encodeFeatureName(fi->getName());
					}
				}
				++fi;
			}
			strQuery << ")\n";

			// Add values
			strQuery << "VALUES ( ";
			strQuery << long2string(object_id_d) << ",";
			strQuery << long2string(monads.first());
			if (objectRangeType != kORTSingleMonad) {
				strQuery << "," << long2string(monads.last());
			}
			if (objectRangeType == kORTMultipleRange) {
				strQuery << "," 
					 << escapeStringForSQL(monads.toCompactString());

			}
			fi = features.begin();
			while (fi != features.end()) {
				if (!fi->getIsComputed()) {
					// The penultimate "true" on FeatureInfo2SQLvalue means that 
					// we must create any IDD-String association if it is not 
					// there in the OT_mdf_FEATURE_NAME_set table.
					strQuery << ',' << FeatureInfo2SQLvalue(OTN, object_type_id, true, *fi);
					if (featureTypeIdIsSOM(fi->getType())) {
						SetOfMonads som;
						som.fromString(fi->getDefaultValue());
						if (som.isEmpty()) {
							strQuery << ',' << monad_m2string(MAX_MONAD);
						} else {
							strQuery << ',' << monad_m2string(som.first());
						}
					}
				}
				++fi;
			}
			strQuery << ")\n";

			// Execute query
			if (!pConn->execCommand(strQuery.str())) {
				DEBUG_COMMAND_QUERY_FAILED("EMdFDB::createObject", strQuery.str());
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}
 
			// Update min_m to monads.first() if necessary
			monad_m min_m_candidate = monads.first();
			if (!setMin_m(min_m_candidate)) {
				DEBUG_X_FAILED("EMdFDB::createObject", "Setting min_m");
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}

			// Update max_m to monads.last() if necessary
			monad_m max_m_candidate = monads.last();
			if (!setMax_m(max_m_candidate)) {
				DEBUG_X_FAILED("EMdFDB::createObject", "Setting max_m");
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}

			// Update OT's entry in object_types for largest_object_length
			monad_m object_length = monads.last() - monads.first() + 1;
			if (!setLargestObjectLengthIfNecessary(OTN, 
							       object_length, 
							       objectRangeType,
							       false)) {
				DEBUG_X_FAILED("EMdFDB::createObject", "Setting object type's largest object length");
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}

			// Commit transaction if necessary
			if (bDoCommit) {
				if (!pConn->commitTransaction()) {
					DEBUG_COMMITTRANSACTION_FAILED("EMdFDB::createObject");
					return false;
				}
			}
		} catch (EMdFDBDBError e) {
			DEBUG_X_FAILED("EMdFDB::createObject", e.what());
			return false;
		}
		// If we got this far, there were no errors
		return true;
	}
}

/** Update an existing object
 *@internal
 *
 * @param object_id_d the id_d of the object to create.
 *
 * @param object_type_name the object type name of the OT to hold the
 * object.
 *
 * @param object_type_id The id_d of the object type. Must be the same 
 *        object type as \p object_type_name.
 *
 * @param features a list of FeatureInfo objects describing the
 * features to be updayed.  Need not be all the features from the
 * object type.
 *
 * @return true on no DB error, false otherwise.
 */
bool EMdFDB::updateObject(id_d_t object_id_d,
			  const std::string& object_type_name,
			  id_d_t object_type_id,
			  const std::list<FeatureInfo>& features)
{
	if (pConn == 0)
		return false;
	else {
		try {
			//
			// Create query
			//
			std::ostringstream query_stream;

			// Normalize object type name
			std::string OTN = normalizeOTName(object_type_name);

			// Add UPDATE
			query_stream 
				<< "UPDATE " << normalizeTableName(OTN + "_objects", true) << "\n";

			// Add features
			query_stream << "SET ";
			std::list<FeatureInfo>::const_iterator ci(features.begin());
			std::list<FeatureInfo>::const_iterator cend(features.end());
			while (ci != cend) {
				// Add feature

				// The penultimate "true" on FeatureInfo2SQLvalue means that 
				// we must create any IDD-String association if it is not 
				// there in the OT_mdf_FEATURE_NAME_set table.
				query_stream << encodeFeatureName(ci->getName()) << " = " << FeatureInfo2SQLvalue(OTN, object_type_id, true, *ci);
				if (featureTypeIdIsSOM(ci->getType())) {
					SetOfMonads som;
					som.fromString(ci->getDefaultValue());
					if (som.isEmpty()) {
						query_stream << ",first_monad_" << encodeFeatureName(ci->getName()) << '=' << monad_m2string(MAX_MONAD);
					} else {
						query_stream << ",first_monad_" << encodeFeatureName(ci->getName()) << '=' << monad_m2string(som.first());
					}
				}
				

				// Advance iterator
				++ci;

				// Add comma or newline
				if (ci != cend)
					query_stream << ",\n";
				else 
					query_stream << "\n";
			}

			// Add WHERE
			query_stream << "WHERE object_id_d = " << long2string(object_id_d);

			if (!pConn->execCommand(query_stream.str())) {
				DEBUG_COMMAND_QUERY_FAILED("EMdFDB::updateObject", query_stream.str());
				return false;
			}
		} catch (EMdFDBDBError e) {
			DEBUG_X_FAILED("EMdFDB::updateObject", e.what());
			return false;
		}
		// If we got this far, there were no errors
		return true;
	}
}

/** Drop an object completely from the database
 *@internal
 *
 * @param object_id_d the id_d of the object to create.
 *
 * @param object_type_id The id_d of the object type holding the object.
 *
 * @param object_type_name the object type name of the OT holding the
 * object. Must be the same type as \p object_type_id.
 *
 * @param objectRangeType the eObjectRangeType of the object type
 * indicated with \e object_type_name .
 *
 * @return true on no DB error, false otherwise.
 */
bool EMdFDB::dropObject(id_d_t object_id_d, 
			id_d_t object_type_id, 
			const std::string& object_type_name,
			eObjectRangeType objectRangeType)
{
	UNUSED(object_type_id);

	if (pConn == 0)
		return false;
	else {
		// Begin transaction
		bool bDoCommit;
		bDoCommit = pConn->beginTransaction();

		// Normalize object type name
		std::string OTN = normalizeOTName(object_type_name);

		// Get monads (we need them later)
		SetOfMonads monads;
		getSOMForObject(OTN, object_id_d, objectRangeType, monads);

		// Delete object from OT_objects
		{
			std::ostringstream query_stream;
			query_stream 
				<< "DELETE\n"
				<< "FROM " << normalizeTableName(OTN + "_objects", true) << "\n"
				<< "WHERE object_id_d = " << long2string(object_id_d);
			if (!pConn->execCommand(query_stream.str())) {
				DEBUG_COMMAND_QUERY_FAILED("EMdFDB::dropObject", query_stream.str());
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}
		}

		//
		// Check to see whether we should update min_m/max_m
		//

		// Get current min_m and max_m
		monad_m current_min_m;
		if (!getMin_m(current_min_m)) {
			DEBUG_X_FAILED("EMdFDB::dropObject", "Getting current min_m");
			if (bDoCommit)
				pConn->abortTransaction();
			return false;
		}
 
		monad_m current_max_m;
		if (!getMax_m(current_max_m)) {
			DEBUG_X_FAILED("EMdFDB::dropObject", "Getting current max_m");
			if (bDoCommit)
				pConn->abortTransaction();
			return false;
		}

		// Should we update min_m and/or max_m?
		// That is, is there a danger that this object was the one
		// causing the min_m?
		// That is, is current_min_m == the last monad of the object?
		if (current_min_m == monads.first()
		    || current_max_m == monads.last()) {
			// Yes, we should update
			if (!setMin_max_m_fromObjectTypes(false)) {
				DEBUG_X_FAILED("EMdFDB::dropObject", "Setting min_m/max_m from all object types");
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}
		}

		monad_m largest_object_length;
		if (!getLargestObjectLength(OTN, 
					    objectRangeType,
					    largest_object_length)) {
			DEBUG_X_FAILED("EMdFDB::dropObject", "Getting largest object length");
			if (bDoCommit)
				pConn->abortTransaction();
			return false;
		}

		monad_m object_length = monads.last() - monads.first() + 1;
		if (object_length >= largest_object_length) {
			if (!setLargestObjectLengthIfNecessary(OTN, 
							       object_length, 
							       objectRangeType,
							       true)) {
				DEBUG_X_FAILED("EMdFDB::dropObject", "Setting largest object length unconditionally");
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}
		}
		

		// Commit transaction if necessary
		if (bDoCommit) {
			if (!pConn->commitTransaction()) {
				DEBUG_COMMITTRANSACTION_FAILED("EMdFDB::dropObject");
				return false;
			}
		}

		// If we got this far, there were no errors
		return true;
	}
}

/** Drop objects that are wholly contained in a set of monads
 *@internal
 *
 * Uses EMdFDB::getObjectsWithinMonads() to get the object id_ds to
 * drop.
 *
 * @param monads the monad set within which to drop objects.
 *
 * @param object_type_name the object type name of the OT to hold the
 * object.
 *
 * @param object_type_id The id_d of the object type. Must be the same 
 *        object type as \p object_type_name.
 *
 * @param objectRangeType the eObjectRangeType of the object type.
 *
 * @param result the list to contain the id_ds of the objects dropped.
 *
 * @return true on no DB error, false otherwise.
 */
bool EMdFDB::dropObjectsInMonads(const SetOfMonads& monads, 
				 const std::string& object_type_name, 
				 id_d_t object_type_id, 
				 eObjectRangeType objectRangeType,
				 /* out */ std::list<id_d_t>& result)
{
	UNUSED(object_type_id);
	
	if (pConn == 0) {
		return false;
	} else {
		// See if monads are empty.  Return with success if they are
		if (monads.isEmpty()) {
			return true;
		}

		// Find out if object type has objects
		bool bHasObjects = false;
		if (!objectTypeHasObjects(object_type_name, bHasObjects)) {
			DEBUG_X_FAILED("EMdFDB::dropObjectsInMonads", "finding out whether object type had objects");
			return false;
		}

		// If there are none, then there is no point in going further
		if (!bHasObjects) {
			return true;
		}

		// Get results
		if (!getObjectsWithinMonads(monads, 
					    object_type_name, 
					    objectRangeType,
					    result)) {
			result.clear();
			DEBUG_X_FAILED("EMdFDB::dropObjectsInMonads", "getObjectsWithinMonads");
			return false;
		}

		// If there are no objects to delete, return at this point with success
		if (result.empty()) {
			return true;
		}

		// Normalize object type name
		std::string OTN = normalizeOTName(object_type_name);

		// Make monad constraints
		std::string monad_constraints_first_last_monad = makeMonadConstraintsFromSOM(monads, objectRangeType);


		// Begin transaction
		bool bDoCommit;
		bDoCommit = pConn->beginTransaction();

		std::cerr << "OT_objects" << "..." << std::flush;
		// Delete object from OT_objects
		{
			std::ostringstream query_stream;
			query_stream 
				<< "DELETE\n"
				<< "FROM " << normalizeTableName(OTN + "_objects", true) << "\n"
				<< "WHERE " << monad_constraints_first_last_monad;
			if (!pConn->execCommand(query_stream.str())) {
				DEBUG_COMMAND_QUERY_FAILED("EMdFDB::dropObjectsInMonads", query_stream.str());
				if (bDoCommit)
					pConn->abortTransaction();
				result.clear();
				return false;
			}
		}

		// Update min_m/max_m if necessary
		if (!setMin_max_m_fromObjectTypes(true)) {
			DEBUG_X_FAILED("EMdFDB::dropObjectsInMonads", "Setting min_m/max_m from all object types");
			if (bDoCommit)
				pConn->abortTransaction();
			result.clear();
			return false;
		}

		monad_m largest_object_length;
		if (!getLargestObjectLengthFromOT_objects(OTN,
							  objectRangeType,
							  largest_object_length)) {
			DEBUG_X_FAILED("EMdFDB::dropObjectsInMonads", "Getting largest object length from OT_objects");
			if (bDoCommit)
				pConn->abortTransaction();
			result.clear();
			return false;
		}

		if (!setLargestObjectLengthIfNecessary(OTN,
						       largest_object_length,
						       objectRangeType,
						       true // Set unconditionally,
						       
						       )) {
			DEBUG_X_FAILED("EMdFDB::dropObjectsInMonads", "Setting largest object length from OT_objects");
			if (bDoCommit)
				pConn->abortTransaction();
			result.clear();
			return false;
		}


		// Commit transaction if necessary
		if (bDoCommit) {
			if (!pConn->commitTransaction()) {
				DEBUG_COMMITTRANSACTION_FAILED("EMdFDB::dropObjectsInMonads");
				result.clear();
				return false;
			}
		}
		std::cerr << "!" << std::endl << std::flush;

		// If we got this far, there were no errors
		return true;
	}
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
 * @param object_type_id The object type id of the object type to
 * contain the objects.
 *
 * @param object_type_features A list of the FeatureInfo's (minus
 * \p self) used in each InstObject in \p object_list.
 *
 * @param object_list A list of InstObject's.  Each InstObject must
 * have EMdFValue's in the same order as in \p object_type_features.
 *
 * @param objectRangeType Tells whether this is a "multiple range",
 *        "single range", or "single monad" object type.
 *
 * @param object_count Used to return the number of objects created.
 *
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::createObjects(const std::string& object_type_name,
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

		if (!lockTablesForCreateObjects(OTN, object_type_features)) {
			DEBUG_X_FAILED("EMdFDB::createObjects", "Locking tables");
			return false;
		}

		monad_m min_m_candidate = MAX_MONAD;
		monad_m max_m_candidate = 0;
		id_d_t max_id_d = 0;

		// We cannot set this to 1, since there may be 0 objects in object_list.
		monad_m largest_object_length_candidate = 0;

		int count_in_this_round = 0;
		std::list<InstObject*>::const_iterator ci = object_list.begin();
		std::list<InstObject*>::const_iterator cend = object_list.end();
		std::string OT_objects_data = "";
		while (ci != cend) {
			// Take object's monads.
			// We do this because we don't want to create the set of monads
			// on the stack twice. InstObject::getMonads returns a full
			// object, not a reference or a pointer.
			SetOfMonads som;
			((*ci)->getMonads(som));

			// Deal with largest object length
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

			// Create data in OT_objects string
			createObjectsOT_objects_data(OTN, 
						     object_type_id,
						     *ci, 
						     som, 
						     object_type_features, 
						     objectRangeType,
						     OT_objects_data);
			++count_in_this_round;
			++ci;

			// Actually create the data iff
			// either we have reached 10000 objects
			// to be created, or
			// we have reached the end,
			// or either string gets to be 1MB.
			// The 1MB limit is because on Windows's Visual C++ 6.0
			// implementation of std::string, things begin to slow
			// down after 1MB.
			if (count_in_this_round == 10000
			    || ci == cend
			    || OT_objects_data.length() > 1042441) {
				// Create OT_objects data
				if (!createObjectsOT_objects_DB(OTN, OT_objects_data, objectRangeType, object_type_features)) {
					DEBUG_X_FAILED("EMdFDB::createObjects", "creating data in OT_objects");
					return false;
				}
	  
				// Update object_count
				object_count += count_in_this_round;

				// Reset count
				count_in_this_round = 0;

				// Erase strings so we can start afresh
				OT_objects_data = "";
			} else { // We shouldn't create the data in this round
				// So instead we should separate each bit of data
				// from the next
				createObjectsInBetweenData(OT_objects_data);
			}
		}

		// Update min_m to min_m_candidate if necessary
		if (!setMin_m(min_m_candidate)) {
			DEBUG_X_FAILED("EMdFDB::createObjects", "Setting min_m");
			return false;
		}

		// Update max_m to max_m_candidate if necessary
		if (!setMax_m(max_m_candidate)) {
			DEBUG_X_FAILED("EMdFDB::createObjects", "Setting max_m");
			return false;
		}

		// Update largest object length if necessary
		if (!setLargestObjectLengthIfNecessary(OTN, 
						       largest_object_length_candidate, 	
						       objectRangeType,
						       false)) {
			DEBUG_X_FAILED("EMdFDB::createObjects", "Setting largest object length");
			return false;
		}

		// Update sequence 0 to max_id_d+1 if appropriate
		if (!setNextObjectID_DIfNotHigher(max_id_d+1)) {
			DEBUG_X_FAILED("EMdFDB::createObjects", "Setting object id_d sequence");
			return false;
		}

		if (!unlockTablesForCreateObjects(OTN)) {
			DEBUG_X_FAILED("EMdFDB::createObjects", "Unlocking tables");
			return false;
		}


		// If we came this far, there were no DB erros
		return true;
	}
}


/** Batch-create objects, create a line of OT_objects data in a string.
 *@internal
 *
 * This is part of the machinery for the MQL "CREATE OBJECTS WITH
 * OBJECT TYPE.".  It is used by EMdFDB::createObjects() to create one
 * object's worth of output data in a std::string, to be sent to the
 * backend later by EMdFDB::createObjectsOT_objects_DB() (or its
 * polymorphic descendant).
 *
 * @param object_type_name The name of the object type to contain the
 * objects.
 *
 * @param object_type_id The object type id of the object type
 * mentioned by \p object_type_name .
 *
 * @param pObject A pointer to the InstObject which currently holds
 * the object data.
 *
 * @param som The SetOfMonads of the object
 *
 * @param object_type_features A list of the FeatureInfo's (minus
 * \p self) used in each InstObject in \p object_list.
 *
 * @param objectRangeType Tells whether this is a "multiple range",
 *        "single range", or "single monad" object type.
 *
 * @param OT_objects_data The string holding the data to be issued
 * later.
 *
 * @return True on no database error, false if a database error occurred.
 */
void EMdFDB::createObjectsOT_objects_data(const std::string object_type_name, 
					  id_d_t object_type_id,
					  const InstObject *pObject,
					  const SetOfMonads& som,
					  const std::list<FeatureInfo>& object_type_features,
					  eObjectRangeType objectRangeType,
					  /* out */ std::string& OT_objects_data)
{
	UNUSED(object_type_name);
	UNUSED(object_type_id);
	UNUSED(pObject);
	UNUSED(som);
	UNUSED(object_type_features);
	UNUSED(objectRangeType);
	UNUSED(OT_objects_data);
	
	appendLocalError("Error: createObjects is not implemented for your backend.\n");
}

/** Batch-create objects, send batch to the backend.
 *@internal
 *
 * This is part of the machinery for the MQL "CREATE OBJECTS WITH
 * OBJECT TYPE.".  It is used by EMdFDB::createObjects() to send the
 * current batch of objects to the backend.
 *
 * @see EMdFDB::createObjects()
 *
 * @param object_type_name The name of the object type to contain the
 * objects.
 *
 * @param OT_objects_data The std::string containing the batch.
 *
 * @param objectRangeType Tells whether this is a "multiple range",
 *        "single range", or "single monad" object type.
 *
 * @param object_type_features A std::list of FeatureInfos.  The list
 * must be in the same order as the data is in in OT_objects_data.
 *
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::createObjectsOT_objects_DB(const std::string& object_type_name,
					const std::string& OT_objects_data,
					eObjectRangeType objectRangeType,
					const std::list<FeatureInfo>& object_type_features)
{
	UNUSED(object_type_name);
	UNUSED(OT_objects_data);
	UNUSED(objectRangeType);
	UNUSED(object_type_features);
	
	appendLocalError("Error: createObjects is not implemented for your backend.\n");
	return false;
}


/** Batch-create objects, create what is necessary in between each
 *  object record
 *@internal
 *
 * This is part of the machinery for the MQL "CREATE OBJECTS WITH
 * OBJECT TYPE.".  It is used by EMdFDB::createObjects() and others to
 * create the in-between-records marker for the std::string containing
 * the batch.
 *
 * @see EMdFDB::createObjects()
 *
 * @param OT_data The std::string containing the batch which we've
 * built so far.  Is also used to return the data.
 */
void EMdFDB::createObjectsInBetweenData(/* out */ std::string& OT_data)
{
	UNUSED(OT_data);
	
	appendLocalError("Error: createObjects is not implemented for your backend.\n");
}

					    

/** Get current database version
 *
 * This method retrieves information about the schema version of an
 * Emdros database.  The database is first USEd, so the current
 * database connection will be lost.
 *
 * Note that the encryption key is only used for BPT and SQLite 3, and
 * then only if you have an SQLite 3 that supports encryption.
 *
 * @param database_name The name/location of the database to query.
 *
 * @param key The encryption key to be used.
 *
 * @param strCurrentVersion Used to return a human-readable
 * representation of the version.
 *
 * @param nCurrentVersion Used to return the current version as
 * defined in emdf.h.
 *
 * @param nVersionWhichWeCanUpgradeTo Used to return the number of the
 * version which we can upgrade to.  Note that this may be lower than
 * the current version, as we can't always upgrade.
 *
 * @param strVersionWhichWeCanUpgradeTo Used to return a
 * human-readable version of \p nVersionWhichWeCanUpgradeTo .
 *
 * @param bNeedsUpgrading Used to say whether the database needs to be
 * upgraded to be able to run with the current version of Emdros.
 * True means yes, false means no.
 *
 * @param bCanUpgrade Used to say whether the
 * EMdFDB::ugpradeDatabase() method is able to upgrade the database
 * automatically.  If this is not the case, you can always dump the
 * database with the mqldump utility from the current version of the
 * database, then import the resulting MQL script with the mql(1)
 * program from the current version of Emdros.
 *
 * @return True on no DB error, false otherwise.
 */
bool EMdFDB::getCurrentDatabaseVersion(const std::string& database_name, 
				       const std::string& key,
				       std::string& /* out */ strCurrentVersion,
				       int& /* out */ nCurrentVersion,
				       int& /* out */ nVersionWhichWeCanUpgradeTo,
				       std::string& /* out */ strVersionWhichWeCanUpgradeTo,
				       bool& /* out */ bNeedsUpgrading,
				       bool& /* out */ bCanUpgrade)
{
	if (pConn == 0)
		return false;
	else {
		// Assume we can't upgrade
		bCanUpgrade = false;

		// Assume we need upgrading
		bNeedsUpgrading = true;

		// Which version can we upgrade to, if we can?
		nVersionWhichWeCanUpgradeTo = EMDROS_VERSION_1_1_8_TO_1_1_20;
		strVersionWhichWeCanUpgradeTo = "1.1.8-1.1.20";

		if (!useDatabase(database_name, key)) {
			std::cerr << "FAILURE: Could not USE DATABASE " << database_name << "!" << std::endl;
			bCanUpgrade = false;
			return false;
		}

		bool bSchema_version_exists;
		if (!tableExists("schema_version", bSchema_version_exists)) {
			bCanUpgrade = false;
			return false;
		} 

		if (bSchema_version_exists) {
			// So we can simply get the schema version from the schema_version
			// table.

			// Get version
			long schema_version;
			if (!getSchemaVersion(schema_version)) {
				bCanUpgrade = false;
				return false;
			}

			// We now know the current version
			nCurrentVersion = schema_version;

			if (schema_version == EMDROS_VERSION_1_2_0_PRE_59_TO_1_2_0_PRE_79) {
				strCurrentVersion = "1.2.0.pre59-1.2.0.pre79";
				bCanUpgrade = false;
				bNeedsUpgrading = true;
				return true;
			} else if (schema_version == EMDROS_VERSION_1_2_0_PRE_80_TO_1_2_0_PRE_85) {
				strCurrentVersion = "1.2.0.pre80-1.2.0.pre85";
				bCanUpgrade = false;
				bNeedsUpgrading = true;
				return true;
			} else if (schema_version == EMDROS_VERSION_1_2_0_PRE_86_TO_1_2_0_PRE_96) {
				strCurrentVersion = "1.2.0.pre86-1.2.0.pre96";
				bCanUpgrade = false;
				bNeedsUpgrading = true;
				return true;
			} else if (schema_version == EMDROS_VERSION_1_2_0_PRE_97_TO_1_2_0_PRE_151) {
				strCurrentVersion = "1.2.0.pre97-1.2.0.pre151";
				bCanUpgrade = false;
				bNeedsUpgrading = true;
				return true;
			} else if (schema_version == EMDROS_VERSION_1_2_0_PRE_152_TO_1_2_0_PRE_153) {
				strCurrentVersion = "1.2.0.pre152-1.2.0.pre153";
				bCanUpgrade = false;
				bNeedsUpgrading = true;
				return true;
			} else if (schema_version == EMDROS_VERSION_1_2_0_PRE_154_TO_1_2_0_PRE198) {
				strCurrentVersion = "1.2.0.pre154-1.2.0.pre198.";
				bCanUpgrade = false;
				bNeedsUpgrading = true;
				return true;
			} else if (schema_version == EMDROS_VERSION_1_2_0_PRE_199_TO_1_2_0_PRE207) {
				strCurrentVersion = "1.2.0.pre199-1.2.0.pre207.";
				bCanUpgrade = false;
				bNeedsUpgrading = true;
				return true;
			} else if (schema_version == EMDROS_VERSION_1_2_0_PRE_208) {
				strCurrentVersion = "1.2.0.pre208 or later.";
				bCanUpgrade = false;
				bNeedsUpgrading = false;
				return true;
			} else {
				std::cerr << "Error: Unknown Emdros version '" << schema_version << "' in the schema_version table." << std::endl;
				bCanUpgrade = false;
				bNeedsUpgrading = false;
				return false;
			}

			// Catch-all
			bCanUpgrade = false;
			bNeedsUpgrading = false;
			return false;
		}


		// Check if sequence_0 table exists
		bool bSequence_0TableExists;
		if (!tableExists("sequence_0", bSequence_0TableExists)) {
			bCanUpgrade = false;
			return false;
		}

		bool bSequence_118_0TableExists;
		if (!tableExists("sequence_118_0", bSequence_118_0TableExists)) {
			bCanUpgrade = false;
			return false;
		}

		bool bBuildableFromTableExists;
		if (!tableExists("buildable_from", bBuildableFromTableExists)) {
			bCanUpgrade = false;
			return false;
		}

		// Did buildable_from not exist?  (This was removed in 1.2.0)
		if (!bBuildableFromTableExists) {
			strCurrentVersion = "1.2.0.pre01 to 1.2.0.pre58";
			nCurrentVersion = EMDROS_VERSION_1_2_0_PRE_01_TO_1_2_0_PRE_58;

			// If we are here, we need to upgrade, but we can't
			bNeedsUpgrading = true;
			bCanUpgrade = false;
			return true;
		}

		// Did sequence_118_0 exist? (This was added in 1.1.8)
		if (bSequence_118_0TableExists) {
			strCurrentVersion = "1.1.8-1.1.20";
			nCurrentVersion = EMDROS_VERSION_1_1_8_TO_1_1_20;

			// If we are here, we must upgrade, but can't
			bNeedsUpgrading = true;
			bCanUpgrade = false;
			return true;
		}

		// If not, and if sequence_0 didn't exist either, 
		// then we are pre-1.1.0
		if (!bSequence_0TableExists) {
			strCurrentVersion = "pre-1.1.0";
			nCurrentVersion = EMDROS_VERSION_PRE_1_1_0;
			bNeedsUpgrading = true;
			bCanUpgrade = false;
			return true;
		}

		bool bMax_mTableExists;
		// Check to see whether max_m exists
		if (!tableExists("max_m", bMax_mTableExists)) {
			bCanUpgrade = false;
			return false;
		}

		// If not, then we are pre-1.1.5
		if (!bMax_mTableExists) {
			strCurrentVersion = "1.1.0 to 1.1.4";
			nCurrentVersion = EMDROS_VERSION_1_1_0_TO_1_1_4;
			bNeedsUpgrading = true;
			bCanUpgrade = false;
			return true;
		} else {
			// Current version is 1.1.5 to 1.1.7
			strCurrentVersion = "1.1.5 to 1.1.7";
			nCurrentVersion = EMDROS_VERSION_1_1_5_TO_1_1_7;
			bNeedsUpgrading = true;
			bCanUpgrade = false;
			return true;
		}

		// If we got this far, there were no errors
		return true;
	}
}

/** Upgrade the database automatically
 *
 * The database is first USE'd, so the current database connection
 * will be lost.
 *
 * Currently, this method does nothing beyond USE'ing the database; we
 * cannot upgrade any databases at this point.
 *
 * The recommended way of upgrading is to use mqldump from the version
 * of Emdros that was used to create the database to dump the database
 * to MQL.  Then the database can be imported with the current version
 * of the mql program.
 *
 * @param database_name The name of the database to upgrade.
 *
 * @param nCurrentVersion the version number (defined in emdf.h) of
 * the database.  Use EMdFDB::getCurrentDatabaseVersion() to get the
 * number.
 *
 * @return True on no DB error, false otherwise.
 */
bool EMdFDB::upgradeDatabase(const std::string& database_name, 
			     int nCurrentVersion)
{
	UNUSED(nCurrentVersion);
	
	if (pConn == 0)
		return false;
	else {
		/* So far, we only upgrade on PostgreSQL and MySQL, so we just pass
		 * an empty key. */
		if (!useDatabase(database_name, "")) {
			std::cerr << "FAILURE: Could not USE DATABASE " << database_name << "!" << std::endl;
			return false;
		}

		// We cannot upgrade
		return false;
	}
}

/** Checks whether a certain existing object type has objects
 *@internal
 *
 * @param object_type_name The name of the object type to test.
 *
 * @param bHasObjects Upon successful return (i.e., function returns
 * true), this value is true if there exists at least one object in
 * the object type, and false if there are no objects.  The value is
 * undefined if the method returns false.
 *
 * @return True on no DB error, false otherwise. Note that the
 * bHasObjects value is only valid upon return if the return value is
 * true.
 */
bool EMdFDB::objectTypeHasObjects(const std::string& object_type_name,
				  bool& /* out */ bHasObjects)
{
	if (pConn == 0) {
		return false;
	} else {
		// Test that connection is OK
		if (!pConn->connectionOk()) {
			DEBUG_X_FAILED("EMdFDB::objectTypeHasObjects", "connectionOk()");
			return false;
		}

		// Normalize object type name
		std::string OTN = normalizeOTName(object_type_name);
		

		// SELECT
		{
			std::ostringstream query_stream;
			query_stream 
				<< "SELECT object_id_d FROM " 
				<< normalizeTableName(OTN + "_objects", true) << " LIMIT 1";
			if (!pConn->execSelect(query_stream.str())) {
				DEBUG_X_FAILED("EMdFDB::objectTypeHasObjects", "executing LIMIT select.");
				return false;
			} else {
				bHasObjects = pConn->hasRow();
			}

			pConn->finalize();
		}

		// If we got this far, there were no errors
		return true;
	}
}


/**
 * Normalize a table name so as to fit into the maximum table name
 * length of the backend engine used.
 *
 * This is done by first checking the length of the table_name
 * parameter.  If it first inside the maximum length for the engine,
 * the table_name is returned as-is.
 *
 * If it does not fit, and the bCheckIfTableExists parameter is true,
 * the table name is checked for existence.  If it exists, the
 * table_name parameter is returned unchanged.
 *
 * Otherwise, a CRC32-based digest scheme is used to create a table
 * name which fits into the maximum table name length of the engine.
 *
 * The maximum table name length is taken from
 * EMdFDB::m_max_table_name_length.
 *
 * @param table_name The table name to normalize.
 *
 * @param bCheckIfTableExists If true, and the table_name is too long,
 * checks whether table exists. If it does, the table_name is returned
 * as-is.  If false, and the table_name is too long, the table_name is
 * normalized unconditionally.
 *
 * @return The normalized table name.
 */
std::string EMdFDB::normalizeTableName(const std::string& table_name, bool bCheckIfTableExists)
{
	int table_name_length = (int) table_name.length();

	if (table_name_length <= m_max_table_name_length) {
		return table_name;
	} else {
		if (bCheckIfTableExists) {
			bool bTableExists;
			if (!tableExists(table_name.substr(0, m_max_table_name_length), bTableExists)) {
				// Error. Return early.
				return table_name;
			} else {
				if (bTableExists) {
					// Table already exists; don't do CRC32-generated table name
					return table_name;
				} else {
					; // Fall through to the CRC32-generated table name...
				}
			}
		} else {
			; // Fall through to the CRC32-generated table name...
		}
	}
	std::string new_table_name;
	std::string salt1 = "ZCGwYkCLCl";
	std::string salt2 = "8Ktc9lTiU3";
	std::string salted_table_name = salt1 + table_name + salt2;
	unsigned long crc32 = emdros_crc32_calc((const unsigned char*) salted_table_name.c_str(), salted_table_name.length());
	std::string crc32_string = ulong2hex(crc32);
	
	new_table_name = table_name.substr(0, m_max_table_name_length - crc32_string.length() - 1) + "_" + crc32_string;
	
	return new_table_name;
}

std::string EMdFDB::normalizeIndexName(const std::string& index_name)
{
	int index_name_length = (int) index_name.length();

	if (index_name_length <= m_max_table_name_length) {
		return index_name;
	} else {
		return normalizeTableName(index_name, false);
	}
}

/** Checks whether a given table exists.
 *@internal
 *
 * @param table_name The name of the table to test for.
 *
 * @param bTableExists Upon successful return (i.e., function returns
 * true), this value is true if the table exists, and false if it does
 * not exist. The value is undefined if the method returns false.
 *
 * @return True on no DB error, false otherwise. Note that the
 * bTableExists value is only valid upon return if the return value is
 * true.
 */
bool EMdFDB::tableExists(std::string table_name, bool&  /* out */ bTableExists)
{
	if (pConn == 0) {
		return false;
	} else {
		// Test that connection is OK
		if (!pConn->connectionOk()) {
			DEBUG_X_FAILED("EMdFDB::tableExists", "connectionOk()");
			return false;
		}

		// SELECT
		{
			std::ostringstream query_stream;
			query_stream 
				<< "SELECT *" << '\n'
				<< "FROM " << table_name << '\n'
				<< "LIMIT 1";
			if (!pConn->execSelect(query_stream.str())) {
				bTableExists = false;
			} else {
				bTableExists = true;

				// Read row
				if (pConn->hasRow()) {
					bool bMoreTuples = true;
					while (bMoreTuples) {
						if (!pConn->getNextTuple(bMoreTuples)) {
							std::cerr << "EMdFDB::tableExists: getNextTuple returned error." << std::endl;
							return false;
						}
					}
				}
			}

			pConn->finalize();
		}

		// If we got this far, there were no errors
		return true;
	}
}


//
// min_m/max_m
// 

/** Create Min_m/Max_m tables
 *@internal
 *
 * @return True on no DB error, false otherwise.
 */
bool EMdFDB::createMin_max_m_table(void)
{
	if (pConn == 0)
		return false;
	else {
		const char *WITHOUT_OIDS = "";
		if (m_backend_kind == kPostgreSQL) {
			WITHOUT_OIDS = "\nWITHOUT OIDS";
		}

		// CREATE TABLE
		{
			std::ostringstream query_stream;
			query_stream 
				<< "CREATE TABLE max_m (\n" 
				<< "    dummy_id INTEGER PRIMARY KEY NOT NULL,\n"
				<< "    max_m    INT NOT NULL"
				<< "\n)"
				<< WITHOUT_OIDS;
			if (!pConn->execCommand(query_stream.str())) {
				DEBUG_COMMAND_QUERY_FAILED("EMdFDB::createMin_max_m_table", query_stream.str());
				return false;
			} 
		}

		// Fill table with initial value
		{
			std::ostringstream query_stream;
			query_stream 
				<< "INSERT INTO max_m (dummy_id, max_m)\n"
				<< "VALUES ( 0 , 0 )";
			if (!pConn->execCommand(query_stream.str())) {
				DEBUG_COMMAND_QUERY_FAILED("EMdFDB::createMin_max_m_table", query_stream.str());
				return false;
			} 
		}

		// CREATE TABLE
		{
			std::ostringstream query_stream;
			query_stream 
				<< "CREATE TABLE min_m (\n" 
				<< "    dummy_id INTEGER PRIMARY KEY NOT NULL,\n"
				<< "    min_m    INT NOT NULL"
				<< "\n)"
				<< WITHOUT_OIDS;
			if (!pConn->execCommand(query_stream.str())) {
				DEBUG_COMMAND_QUERY_FAILED("EMdFDB::createMin_max_m_table", query_stream.str());
				return false;
			} 
		}

		// Fill table with initial value
		{
			std::ostringstream query_stream;
			query_stream 
				<< "INSERT INTO min_m (dummy_id, min_m)\n"
				<< "VALUES ( 0 , " << long2string(MAX_MONAD) << " )";
			if (!pConn->execCommand(query_stream.str())) {
				DEBUG_COMMAND_QUERY_FAILED("EMdFDB::createMin_max_m_table", query_stream.str());
				return false;
			} 
		}

		// If we got this far, there were no errors
		return true;
	}
}

// Iterate over all object types, calling Set_min_max_m_from_object_type
// for each object type.
/** Set Min_M/Max_m from all object types.
 *@internal
 *
 * Iterates over all object types, calling
 * EMdFDB::setMin_max_m_fromObjectType() for each object type.
 *
 * @param bBeVerbose If this is true, we print a lot of progress info
 * on std::cerr.  This is mainly for debugging purposes.
 *
 * @return True on no DB error, false otherwise.
 */
bool EMdFDB::setMin_max_m_fromObjectTypes(bool bBeVerbose)
{
	if (pConn == 0)
		return false;
	else {
		// Get list of object types
		std::list<std::string> object_type_names;
		if (!getObjectTypes(object_type_names)) {
			std::cerr << "FAILURE: Could not get list of object types in database!" << std::endl;
			return false;
		}

		// Reset min_m 
		if (!setMin_m(MAX_MONAD, true)) {
			std::cerr << "FAILURE: Could not set min_m unconditionally!" << std::endl;
			return false;
		}

		// Reset max_m 
		if (!setMax_m(0, true)) {
			std::cerr << "FAILURE: Could not set max_m unconditionally!" << std::endl;
			return false;
		}

		// Output message if being verbose
		if (bBeVerbose) {
			std::cerr << std::endl << "Setting min_m/max_m from object types..." << std::endl << std::flush;
		}

		// Iterator over list
		std::list<std::string>::const_iterator ci;
		ci = object_type_names.begin();
		while (ci != object_type_names.end()) {
			// Output object type name if being verbose
			if (bBeVerbose) {
				std::cerr << *ci;
			}

			// Set min_m/max_m from object type
			if (!setMin_max_m_fromObjectType(*ci, bBeVerbose)) {
				return false;
			}

			// Advance iterator
			++ci;

			// Output ", " if not end and being verbose
			if (ci != object_type_names.end()) {
				if (bBeVerbose) {
					std::cerr << ", ";
				}
			}
		}

		// Output message if being verbose
		if (bBeVerbose) {
			std::cerr << "... Done!" << std::endl << std::flush;
		}

		// If we got this far, there were no errors
		return true;
	}
}

// 
/** Set min_m and max_m from a certain object type (if necessary)
 *@internal
 *
 * Gets the minimum MIN and maximum monad MAX from the object type
 * with the given name.  If min_m is currently greater than MIN, set
 * min_m to MIN.  If max_m is currently smaller than MAX, set max_m to
 * MAX.
 *
 * @param object_type_name The name of the object type to set min_m
 * and max_m from.
 *
 * @param bBeVerbose Currently unused.
 *
 * @return True on no DB error, false otherwise.
 */
bool EMdFDB::setMin_max_m_fromObjectType(const std::string& object_type_name, 
					 bool bBeVerbose)
{
	UNUSED(bBeVerbose);
	
	if (pConn == 0)
		return false;
	else {
		// Normalize object type name
		std::string OTN = normalizeOTName(object_type_name);

		// Get whether it is a WITH SINGLE RANGE OBJECTS or not
		eObjectRangeType objectRangeType;
		if (!getOTObjectRangeType(OTN, objectRangeType)) {
			DEBUG_X_FAILED("EMdFDB::setMin_max_m_fromObjectType", 
				       "getting range type of object type " + object_type_name);
			return false;
		}
    
		// Decide mse_first/mse_last or first_monad/last_monad
		std::string firstm, lastm;
		bool2firstm_lastm(objectRangeType, firstm, lastm);

		// Check the object type has rows
		bool bObjectTypeHasObjects = false;
		{
			std::ostringstream query_stream;
			query_stream 
				<< "SELECT object_id_d" << '\n'
				<< "FROM " << normalizeTableName(OTN + "_objects", true) + "\n"
				<< "LIMIT 1";
			if (!pConn->execSelect(query_stream.str())) {
				DEBUG_SELECT_QUERY_FAILED("EMdFDB::setMin_max_m_fromObjectType", query_stream.str());
				return false;
			}

			// Is there at least one tuple? (There should be either 1 or none)
			if (pConn->hasRow()) {
				// There are objects in the table
				bObjectTypeHasObjects = true;
			} else {
				// There are no objects in the table
				bObjectTypeHasObjects = false;
			}

			// Release results
			pConn->finalize();
		}

		// Does the object type not have objects?
		if (!bObjectTypeHasObjects) {
			// If the object type has no objects,
			// there is no point in going further.
			// Return with success.
			// Also, it is an error to proceed, because
			// MIN(mse_first) and MAX(mse_last) will
			// return 0 and 0 respectively, thus
			// setting min_m to 0, at least on PostgreSQL.
			return true;
		}

		//
		// Get min_m/max_m candidate
		//
		monad_m min_m_candidate;
		monad_m max_m_candidate;

		// SELECT
		{
			std::ostringstream query_stream;
			query_stream 
				<< "SELECT MIN(" << firstm << "), MAX(" << lastm << ")\n"
				<< "FROM " << normalizeTableName(OTN + "_objects", true);
			if (!pConn->execSelect(query_stream.str())) {
				DEBUG_SELECT_QUERY_FAILED("EMdFDB::setMin_max_m_fromObjectType", query_stream.str());
				return false;
			} 
		}

		// Access result
		bool bSetMinMax = true;
		try {
			if (!pConn->accessTuple(0, min_m_candidate)) {
				DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::setMin_max_m_fromObjectType");
				// Return failure  
				return false;
			}

			if (!pConn->accessTuple(1, max_m_candidate)) {
				DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::setMin_max_m_fromObjectType");
				// Return failure  
				return false;
			}
		} catch (EMdFNULLValueException e) {
			// There was a NULL Value exception, so the object type
			// was probably empty.
			// Do not set min/max
			bSetMinMax = false;
		}

		// Should we set min and max?
		if (bSetMinMax) {
			// Set min_m (if smaller)
			if (!setMin_m(min_m_candidate)) {
				DEBUG_X_FAILED("EMdFDB::setMin_max_m_fromObjectType", "Setting min_m");
				// Release results
				pConn->finalize();
	
				// Return failure  
				return false;
			}

			// Set max_m (if larger)
			if (!setMax_m(max_m_candidate)) {
				DEBUG_X_FAILED("EMdFDB::setMin_max_m_fromObjectType", "Setting max_m");
				// Release results
				pConn->finalize();
	
				// Return failure  
				return false;
			}
		}

		// Release results
		pConn->finalize();
	
		// If we got this far, there were no errors
		return true;
	}
}


/** Gets the max_m monad of the database.
 *
 * The max_m monad is the least monad in the database.  
 * Defaults to 0 if there are no objects in the database.
 *
 * @param max_m Used for returning the max_m monad.
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::getMax_m(monad_m& /* out */ max_m)
{
	if (pConn == 0)
		return false;
	else {
		//
		// Get max_m 
		//

		// SELECT
		{
			std::ostringstream query_stream;
			query_stream 
				<< "SELECT max_m\n"
				<< "FROM max_m\n"
				<< "WHERE dummy_id = 0";
			if (!pConn->execSelect(query_stream.str())) {
				DEBUG_SELECT_QUERY_FAILED("EMdFDB::getMax_m", query_stream.str());
				return false;
			} 
		}

		// Access result
		try {
			if (!pConn->accessTuple(0, max_m)) {
				DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getMax_m");
				// Return failure  
				return false;
			}
		} catch (EMdFNULLValueException e) {
			DEBUG_NULL_VALUE_EXCEPTION("EMdFDB::getMax_m");
			// There was a NULL value exception, hence we return an error
			return false;
		}


		// Release results
		pConn->finalize();
	
		// If we got this far, there were no errors
		return true;
	}
}


/** Set max_m if necessary
 *@internal
 *
 * Tests to see if max_m is currently smaller than max_m_candidate.
 * If it is, we set max_m to max_m_candiate.  Otherwise, we leave
 * max_m alone, unless bSetUnconditionally is true, in which case we
 * always set max_m to max_m_candiate.
 *
 * @see EMdFDB::setMin_m().
 *
 * @param max_m_candidate The monad to potentially set max_m to.
 *
 * @param bSetUnconditionally If true, set max_m to max_m_candidate,
 * even if max_m is currently greater than max_m_candidate.
 *
 * @return True on no DB error, false otherwise.
 */
bool EMdFDB::setMax_m(monad_m max_m_candidate, bool bSetUnconditionally)
{
	if (pConn == 0)
		return false;
	else {
		// Get current max_m for comparison
		monad_m current_max_m;
		if (!getMax_m(current_max_m)) {
			DEBUG_X_FAILED("EMdFDB::setMax_m", "Getting current max_m");
			return false;
		}

		// Check if we should update
		if (bSetUnconditionally || (max_m_candidate > current_max_m)) {
			// Yes, so UPDATE
			{
				std::ostringstream query_stream;
				query_stream 
					<< "UPDATE max_m\n"
					<< "SET max_m = " << long2string(max_m_candidate) << '\n'
					<< "WHERE dummy_id = 0";
				if (!pConn->execCommand(query_stream.str())) {
					DEBUG_COMMAND_QUERY_FAILED("EMdFDB::setMax_m", query_stream.str());
					return false;
				} 
			}
		}

		// If we got this far, there were no errors
		return true;
	}
}


/** Gets the min_m monad of the database.
 *
 * The min_m monad is the least monad in the database.  
 * Defaults to MAX_MONAD if there are no objects in the database.
 *
 * @param min_m Used for returning the min_m monad.
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::getMin_m(monad_m& /* out */ min_m)
{
	if (pConn == 0)
		return false;
	else {
		//
		// Get min_m 
		//

		// SELECT
		{
			std::ostringstream query_stream;
			query_stream 
				<< "SELECT min_m\n"
				<< "FROM min_m\n"
				<< "WHERE dummy_id = 0";
			if (!pConn->execSelect(query_stream.str())) {
				DEBUG_SELECT_QUERY_FAILED("EMdFDB::getMin_m", query_stream.str());
				return false;
			} 
		}

		// Access result
		try {
			if (!pConn->accessTuple(0, min_m)) {
				DEBUG_ACCESS_TUPLE_FAILED("EMdFDB::getMin_m");
				// Return failure  
				return false;
			}
		} catch (EMdFNULLValueException e) {
			DEBUG_NULL_VALUE_EXCEPTION("EMdFDB::getMin_m");
			// There was a NULL value exception, hence we return an error
			return false;
		}

		// Release results
		pConn->finalize();
	
		// If we got this far, there were no errors
		return true;
	}
}


/** Set min_m if necessary
 *@internal
 *
 * Tests to see if min_m is currently greater than min_m_candidate.
 * If it is, we set min_m to min_m_candiate.  Otherwise, we leave
 * min_m alone, unless bSetUnconditionally is true, in which case we
 * always set min_m to min_m_candiate.
 *
 * @see EMdFDB::setMax_m().
 *
 * @param min_m_candidate The monad to potentially set min_m to.
 *
 * @param bSetUnconditionally If true, set min_m to min_m_candidate,
 * even if min_m is currently greater than min_m_candidate.
 *
 * @return True on no DB error, false otherwise.
 */
bool EMdFDB::setMin_m(monad_m min_m_candidate, bool bSetUnconditionally) 
{
	if (pConn == 0)
		return false;
	else {
		// Get current min_m for comparison
		monad_m current_min_m;
		if (!getMin_m(current_min_m)) {
			DEBUG_X_FAILED("EMdFDB::setMin_m", "Getting current min_m");
			return false;
		}

		// Check if we should update
		if (bSetUnconditionally || (min_m_candidate < current_min_m)) {
			// Yes, so UPDATE
			{
				std::ostringstream query_stream;
				query_stream 
					<< "UPDATE min_m\n"
					<< "SET min_m = " << long2string(min_m_candidate) << '\n'
					<< "WHERE dummy_id = 0";
				if (!pConn->execCommand(query_stream.str())) {
					DEBUG_COMMAND_QUERY_FAILED("EMdFDB::setMin_m", query_stream.str());
					return false;
				} 
			}
		}

		// If we got this far, there were no errors
		return true;
	}
}


/** Gets the all_m-1 object of the database.
 *
 * The all_m-1 object is the unique set of monads that
 * contains all monads between min_m and max_m.
 *
 * @see EMdFDB::getMin_m(), EMdFDB::getMax_m(), 
 *      EmdrosEnv::getMin_m(), EmdrosEnv::getMax_m().
 * 
 * @param all_m_1 Used for returning the all_m-1 object.
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::getAll_m_1(SetOfMonads& /* out */ all_m_1)
{
	if (pConn == 0)
		return false;
	else {
		// Get min_m
		monad_m min_m;
		if (!getMin_m(min_m)) {
			DEBUG_X_FAILED("EMdFDB::getAll_m_1", "Getting min_m");
			return false;
		}

		// Get max_m
		monad_m max_m;
		if (!getMax_m(max_m)) {
			DEBUG_X_FAILED("EMdFDB::getAll_m_1", "Getting max_m");
			return false;
		}

		if (min_m == MAX_MONAD
		    && max_m == 0) {
			// Create new set of monads
			SetOfMonads empty;

			all_m_1 = empty;
		} else {
			// Create new set of monads
			SetOfMonads new_all_m_1(min_m, max_m);

			all_m_1 = new_all_m_1;
		}

		// If we got this far, there were no errors
		return true;
	}
}


/** Retrieve the object range type for a given object type
 *@internal
 *
 * Used to get the object range type of the object type.  The range
 * type tells us whether objects are "single monad", "single range",
 * or "multiple range".
 *
 * This is really a convenience function, since all it does is call
 * EMdFDB::objectTypeExists with some dummy parameters.
 *
 * @param object_type_name The name of the object type to query.
 *
 * @param objectRangeType Used to return the range type.
 *
 * @return True on no DB error, false otherwise.
 */
bool EMdFDB::getOTObjectRangeType(const std::string& object_type_name,
				  /* out */ eObjectRangeType& objectRangeType)
{
	bool bExists;
	id_d_t dummy_id;
	eMonadUniquenessType dummyMonadUniquenessType;
	if (!objectTypeExists(object_type_name, 
			      bExists, 
			      dummy_id, 
			      objectRangeType,
			      dummyMonadUniquenessType)) {
		DEBUG_X_FAILED("EMdFDB::getOTObjectRangeType", "checking whether " + object_type_name + " existed");
		return false;
	}

	// If it didn't exist, we return DB error
	if (!bExists) {
		return false;
	} else {
		// Otherwise, we just let objectRangeType be set above,
		// and return success.
		return true;
	}
}

bool EMdFDB::getOTMonadUniquenessType(const std::string& object_type_name,
				      /* out */ eMonadUniquenessType& monadUniquenessType)
{
	bool bExists;
	id_d_t dummy_id;
	eObjectRangeType dummyObjectRangeType;
	if (!objectTypeExists(object_type_name, 
			      bExists, 
			      dummy_id, 
			      dummyObjectRangeType,
			      monadUniquenessType)) {
		DEBUG_X_FAILED("EMdFDB::getOTMonadUniquenessType", "checking whether " + object_type_name + " existed");
		return false;
	}

	// If it didn't exist, we return DB error
	if (!bExists) {
		return false;
	} else {
		// Otherwise, we just let objectRangeType be set above,
		// and return success.
		return true;
	}
}


//
// Transactions
// 

// Begin transaction
/** Begin a transaction.
 *
 * Attempts to begin a transaction if possible.  It is possible only
 * if the backend supports transactions, and no transaction is in
 * progress in the current EMdFDB instance (i.e., a descendant, such
 * as PgEMdFDB, MySQLEMdFDB, SQLite3EMdFDB).
 *
 * Just calls EMdFConnection::beginTransaction() (or its polymorphic
 * instance).
 *
 * \section Example
 * \code
 *  bool bDoCommit;  // Should we commit later?
 *  bDoCommit = pDB->beginTransaction(); // Possibly begin transaction
 *
 *  // Process...
 *
 *  if (something went wrong) { 
 *      // Recover...
 *      if (bDoCommit) {
 *          if (!pDB->abortTransaction()) {
 *              // Abort failed
 *          }
 *      }
 *      // Exit from function
 *  }
 *
 *  // More processing...
 *
 *  // We're done!
 *
 *  if (bDoCommit) {
 *      if (!pDB->commitTransaction()) {
 *          // Commit failed.
 *      }
 *  }
 * \endcode
 *
 * @see EMdFConnection::beginTransaction().
 * 
 * @return True on transaction begun, false on transaction not
 * started. \b NOTE how this is different from most other EMdFDB
 * return values!
 */
bool EMdFDB::beginTransaction(void)
{
	if (pConn == 0)
		return false;
	else {
		return pConn->beginTransaction();
	}
}

// Commit transaction
/** Commit a transaction.
 *
 * Attempts to commit a transaction.  Should only be called if
 * EMdFDB::beginTransaction() previously returned true.
 *
 * Just calls EMdFConnection::commitTransaction() (or its polymorphic
 * instance).
 *
 * @see EMdFConnection::commitTransaction(), and
 * EMdFDB::beginTransaction() for an example of how to use.
 * 
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::commitTransaction(void)
{
	if (pConn == 0)
		return false;
	else {
		return pConn->commitTransaction();
	}
}

// Abort transaction
/** Abort a transaction.
 *
 * Attempts to abort a transaction.  Should only be called if
 * EMdFDB::beginTransaction() previously returned true.
 *
 * Just calls EMdFConnection::abortTransaction() (or its polymorphic
 * instance).
 *
 * @see EMdFConnection::abortTransaction(), and
 * EMdFDB::beginTransaction() for an example of how to use.
 * 
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::abortTransaction(void)
{
	if (pConn == 0)
		return false;
	else {
		return pConn->abortTransaction();
	}
}

/** Encodes a feature name for use as a column name.
 *@internal
 *
 * Encodes a feature name so that it can be used as an SQL column
 * name.  This is done by lower-casing the feature name and prepending
 * the string "mdf_" to the resulting string.
 *
 * Note that this ensures that all feature names are case-INsensitive
 * when used from within MQL.
 *
 * Note also that this is necessary so as to avoid name clash between
 * user-entered feature-names and other column-names such as
 * "object_id_d", "first_monad", etc.
 *
 * @param feature_name An unencoded feature name, perhaps straight
 * from the user.
 *
 * @return The encoded feature name.
 */
std::string EMdFDB::encodeFeatureName(const std::string feature_name)
{
	// Get lowercase name
	std::string locase_name;
	str_tolower(feature_name, locase_name);

	// Get encoded name
	return "mdf_" + locase_name;
}

/** Encodes a feature name for use as a column name in a PreQueryString.
 *@internal
 *
 * Encodes a feature name so that it can be used as an SQL column name
 * in a PreQueryString.  This is done by calling
 * EMdFDB::encodeFeatureName() and prepending the string "OS." to
 * the resulting string.  If the feature is "self", use OS.object_id_d
 * instead.
 *
 * @see EMdFDB::encodeFeatureName().
 *
 * @param feature_name An unencoded feature name, perhaps straight
 * from the user.
 *
 * @return The encoded feature name.
 */
std::string EMdFDB::encodeFeatureNameForPrequeryString(const std::string feature_name)
{
	std::string strResult;
	if (strcmp_nocase(feature_name, "self") == 0) {
		strResult = "OS.object_id_d";
	} else {
		strResult = std::string("OS.") + encodeFeatureName(feature_name);
	}
	return strResult;
}


/** Decode an encoded feature name
 *@internal
 *
 * Decodes a feature-name previously encoded with
 * EMdFDB::encodeFeatureName().
 *
 * @see EMdFDB::encodeFeatureName().
 *
 * @param coded_feature_name The encoded feature-name to decode.
 *
 * @return The decoded feature-name.
 */
std::string EMdFDB::decodeFeatureName(const std::string coded_feature_name)
{
	return coded_feature_name.substr(4);
}


/** Convert a type ID to a string based on MQL
 *@internal
 *
 * Converts a type ID to its MQL string equivalent.  For example, an
 * INTEGER feature type would get the string "integer", an ASCII
 * feature type would get the string "ascii", and an enum feature type
 * would get the name of the enum.  Similarly for id_d --> "id_d" and
 * string --> "string".
 *
 * Similar to EMdFDB::typeIdToTableColumnType().
 *
 * @see EMdFDB::typeIdToTableColumnType().
 *
 * @param type_id The feature ID of the feature.
 *
 * @param result The result string.
 *
 * @return True on no DB error, false otherwise.
 */
bool EMdFDB::typeIdToString(id_d_t type_id, std::string& result)
{
	bool bResult;
	bool bExists = false;

	// Get feature_type
	switch(type_id & FEATURE_TYPE_TYPE_MASK) {
	case FEATURE_TYPE_INTEGER:
		result = "integer";
		bResult = true;
		break;
	case FEATURE_TYPE_ID_D:
		result = "id_d";
		bResult = true;
		break;
	case FEATURE_TYPE_STRING:
		result = "string";
		bResult = true;
		break;
	case FEATURE_TYPE_ASCII:
		result = "ascii";
		bResult = true;
		break;
	case FEATURE_TYPE_ENUM:
		if (!enumExists(type_id, bExists, result)) {
			bResult = false;
			return false;
		} else {
			if (!bExists) {
				bResult = false;
			} else {
				bResult = true;
			}
		}
		break;
	case FEATURE_TYPE_LIST_OF_INTEGER:
		result = "list of integer";
		bResult = true;
		break;
	case FEATURE_TYPE_LIST_OF_ID_D:
		result = "list of id_d";
		bResult = true;
		break;
	case FEATURE_TYPE_LIST_OF_ENUM:
		if (!enumExists(type_id, bExists, result)) {
			bResult = false;
			return false;
		} else {
			if (!bExists) {
				bResult = false;
				return false;
			} else {
				result = "list of " + result;
				bResult = true;
			}
		}
		break;
	case FEATURE_TYPE_SET_OF_MONADS:
		result = "set of monads";
		bResult = true;
		break;
	default:
		ASSERT_THROW(false, "Unknown FEATURE_TYPE");
		break;
	}
  
	return bResult;
}

/** Convert a type ID to a TableColumnType.
 *@internal
 *
 * Converts a type ID to its TableColumnType equivalent.
 *
 * Similar to EMdFDB::typeIdToString().
 *
 * @see EMdFDB::typeIdToString().
 *
 * @param type_id The feature ID of the feature.
 *
 * @param tc_type The TableColumnType of the feature
 *
 * @param enum_type If tc_type is kTCEnum or kTCListOfEnum, then this
 * is used to return the enum name.  Otherwise, its value is
 * undefined.
 *
 * @return True on no DB error, false otherwise.
 */
bool EMdFDB::typeIdToTableColumnType(id_d_t type_id, TableColumnType& tc_type, std::string& enum_type)
{
	bool bResult = false;

	bool bExists = false;
	switch(type_id & FEATURE_TYPE_TYPE_MASK) {
	case FEATURE_TYPE_INTEGER:
		tc_type = kTCInteger;
		bResult = true;
		break;
	case FEATURE_TYPE_ID_D:
		tc_type = kTCID_D;
		bResult = true;
		break;
	case FEATURE_TYPE_STRING:
	case FEATURE_TYPE_ASCII:
		tc_type = kTCString;
		bResult = true;
		break;
	case FEATURE_TYPE_ENUM:
		tc_type = kTCEnum;
		if (!enumExists(type_id, bExists, enum_type)) {
			bResult = false;
		} else {
			if (!bExists) {
				bResult = false;
			} else {
				bResult = true;
			}
		}
		break;
	case FEATURE_TYPE_LIST_OF_INTEGER:
		tc_type = kTCListOfInteger;
		bResult = true;
		break;
	case FEATURE_TYPE_LIST_OF_ID_D:
		tc_type = kTCListOfID_D;
		bResult = true;
		break;
	case FEATURE_TYPE_LIST_OF_ENUM:    
		tc_type = kTCListOfEnum;
		if (!enumExists(type_id, bExists, enum_type)) {
			bResult = false;
		} else {
			if (!bExists) {
				bResult = false;
			} else {
				bResult = true;
			}
		}
		break;
	case FEATURE_TYPE_SET_OF_MONADS:
		tc_type = kTCSetOfMonads;
		bResult = true;
		break;
	default:
		ASSERT_THROW(false, "Unknown FEATURE_TYPE");
		break;
	}

	return bResult;
}

// With an extra argument saying the length of the index (for BLOB/TEXT)
/** Create an index on a table and given columns, with a length for
 *  BLOB/TEXT columns.
 *@internal
 *
 * Creates an index with the given name on the given table using the
 * given column names.  This version of the method takes, for the
 * columns, a list of pairs of string and int, where the int tells us
 * how long the index prefix used must be.  This is useful for MySQL,
 * which requires this for BLOB/TEXT.
 *
 * @param index_name The name of the new index.
 *
 * @param table_name The name of the table holding the columns to be
 * indexed.
 *
 * @param columns A list of pairs of string and int, where each string
 * is the name of a column to use in the index.  The index will be
 * created with the columns in the specified order.  The int is only
 * used on MySQL, and tells the backend how many bytes to use for the
 * index.  20 is a reasonable value.
 *
 * @return True on no DB error, false otherwise.
 */
bool EMdFDB::createIndex(const std::string& index_name, 
			 const std::string& table_name, 
			 const std::list<std::pair<std::string, unsigned int> >& columns)
{
	if (pConn == 0)
		return false;
	else {
		//
		// Create query
		//
		std::ostringstream query_stream;
		query_stream 
			<< "CREATE INDEX " << normalizeIndexName(index_name) << '\n'
			<< "ON " << normalizeTableName(table_name, false) << '\n'
			<< "(";
		std::list<std::pair<std::string, unsigned int> >::const_iterator ci;
		ci = columns.begin();
		while (ci != columns.end()) {
			query_stream << ci->first;

			if (m_backend_kind == kMySQL) {
				query_stream << '(' << ci->second << ')';
			}

			++ci;
			if (ci != columns.end()) {
				query_stream << ", ";
			}
		}
		query_stream << ")\n";


		//
		// Execute query
		//
		if (!pConn->execCommand(query_stream.str())) {
			DEBUG_COMMAND_QUERY_FAILED("EMdFDB::createIndex", query_stream.str());
			return false;
		}


		// If we got this far, there were no errors
		return true;
	}
}

/** Create an index on a table and given columns
 *@internal
 *
 * Creates an index with the given name on the given table using the
 * given column names.
 *
 * @param index_name The name of the new index.
 *
 * @param table_name The name of the table holding the columns to be
 * indexed.
 *
 * @param columns A list of string, where each string is the name of a
 * column to use in the index.  The index will be created with the
 * columns in the specified order.
 *
 * @return True on no DB error, false otherwise.
 */
bool EMdFDB::createIndex(const std::string& index_name, 
			 const std::string& table_name, 
			 const std::list<std::string>& columns)
{
	if (pConn == 0)
		return false;
	else {
		//
		// Create query
		//
		std::ostringstream query_stream;
		query_stream 
			<< "CREATE INDEX " << normalizeIndexName(index_name) << '\n'
			<< "ON " << normalizeTableName(table_name, false) << '\n'
			<< "(";
		std::list<std::string>::const_iterator ci;
		ci = columns.begin();
		while (ci != columns.end()) {
			query_stream << *ci;
			++ci;
			if (ci != columns.end()) {
				query_stream << ", ";
			}
		}
		query_stream << ")\n";


		//
		// Execute query
		//
		if (!pConn->execCommand(query_stream.str())) {
			DEBUG_COMMAND_QUERY_FAILED("EMdFDB::createIndex", query_stream.str());
			return false;
		}


		// If we got this far, there were no errors
		return true;
	}
}

/** Create standard indexes on OT_objects
 *
 * For a given object type, create the standard indexes on the
 * OT_objects table.
 *
 * @param object_type_name The name of the object type on which to
 * create standard indexes.
 *
 * @return True on no DB error, false otherwise.
 */
bool EMdFDB::createIndicesOnOTObjects(const std::string& object_type_name) 
{
	// Normalize object type name
	std::string OTN = normalizeOTName(object_type_name);

	// Get object type id
	id_d_t object_type_id;
	bool bExists;
	eObjectRangeType objectRangeType;
	eMonadUniquenessType monadUniquenessType;
	if (!objectTypeExists(object_type_name, 
			      bExists,
			      object_type_id,
			      objectRangeType,
			      monadUniquenessType)) {
		return false;
	}

	std::list<std::string> column_list;
	std::string index_name;

	switch (monadUniquenessType) {
	case kMUTNonUniqueMonads:
		column_list.push_back("first_monad");
		index_name = OTN + "o_fm_i";
		break;
	case kMUTUniqueFirstMonads:
		column_list.push_back("object_id_d");
		index_name = OTN + "o_id_d_i";
		break;
	case kMUTUniqueFirstAndLastMonads:
		column_list.push_back("object_id_d");
		index_name = OTN + "o_id_d_i";
		break;
	default:
		ASSERT_THROW(false, "Unknown monad uniqueness type");
		break;
	}
  
	// Create index on OT_objects
	if (!createIndex(index_name, OTN + "_objects", column_list)) {
		return false;
	}
	column_list.pop_back();

	// Get a list of features
	std::list<FeatureInfo> FeatureInfos;
	if (!getFeaturesForObjectType(object_type_id, FeatureInfos))
		return false;

	// Iterate over feature infos
	std::list<FeatureInfo>::const_iterator ci = FeatureInfos.begin();
	std::list<FeatureInfo>::const_iterator cend = FeatureInfos.end();
	while (ci != cend) {
		if (featureTypeIdIsWithIndex(ci->getType())) {
			std::string table_name;
			std::string index_name;
			std::list<std::pair<std::string, unsigned int> > column_list2;
			column_list2.push_back(std::pair<std::string, unsigned int>(encodeFeatureName(ci->getName()), MAX_TEXT_INDEX_CHARS));
			getIndexAndTableNameForFeature(object_type_name, 
						       ci->getName(),
						       table_name, 
						       index_name);
			createIndex(index_name, table_name, column_list2);
			column_list2.pop_back();

			/*
			if (featureTypeIdIsFromSet(ci->getType())) {
				if (!createIndicesOnStringSetTable(object_type_name,
								   ci->getName())) {
					DEBUG_X_FAILED("EMdFDB::createIndicesOnOTObjects",
						       "Creating indices on FROM SET table.");
				}
			}
			*/
		}
		++ci;
	}

	/*
	  XXX: Uncomment this to get it working, once the code has
	  been implemented in SQLite 3's EMdF backend.
	*/
	/*
	if (!createMonadIndexOnOTObjects(object_type_name, objectRangeType)) {
		DEBUG_X_FAILED("EMdFDB::createIndicesOnOTObjects",
			       "Creating monad index on OT_objects.");
	}
	*/
  
	// If we got this far, there were no errors
	return true;
}

bool EMdFDB::createMonadIndexOnOTObjects(const std::string& OTN,
					 eObjectRangeType objectRangeType)
{
	UNUSED(OTN);
	UNUSED(objectRangeType);
	
	// The default implementation just succeeds without doing
	// anything
	return true;
}


/** Create default indices on all object types.
 *
 * First uses the database by calling EMdFDB::useDatabase() (or its
 * polymorphic instance), then getting a list of object types in the
 * database, then calling EMdFDB::createIndicesOnObjectType() for each
 * of them.
 *
 * @see EMdFDB::useDatabase(), EMdFDB::getObjectTypes(),
 * EMdFDB::createIndicesOnObjectType().
 *
 * Note that the encryption key is only used for BPT and SQLite 3, and
 * then only if you have an SQLite 3 that supports encryption.
 *
 * @param database_name The database to use and create indexes on.
 * @param key Just passed to EMdFDB::useDatabase()
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::createIndicesOnDatabase(const std::string& database_name, const std::string& key)
{
	if (pConn == 0) {
		return false;
	} else {
		if (!useDatabase(database_name, key)) {
			std::cerr << "FAILURE: Could not USE DATABASE " << database_name << "!" << std::endl;
			return false;
		}

		std::list<std::string> object_type_names;
		if (!getObjectTypes(object_type_names)) {
			std::cerr << "FAILURE: Could not get list of object types in database " << database_name << "!" << std::endl;
			return false;
		}

		std::list<std::string>::const_iterator ci;
		ci = object_type_names.begin();
		while (ci != object_type_names.end()) {
			if (!createIndicesOnObjectType(*ci)) {
				return false;
			}
			++ci;
		}

		// If we got this far, there were no errors
		return true;
	}
}

/** Drop default indices on all object types.
 *
 * First uses the database by calling EMdFDB::useDatabase() (or its
 * polymorphic instance), then getting a list of object types in the
 * database, then calling EMdFDB::dropIndicesOnObjectType() for each
 * of them.
 *
 * Note that the encryption key is only used for SQLite 3, and only if
 * you have an SQLite 3 that supports encryption.
 *
 * @see EMdFDB::useDatabase(), EMdFDB::getObjectTypes(),
 * EMdFDB::dropIndicesOnObjectType().
 *
 * @param database_name The database to use and drop indexes on.
 * @param key Just passed to EMdFDB::useDatabase()
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::dropIndicesOnDatabase(const std::string& database_name, const std::string& key)
{
	if (pConn == 0)
		return false;
	else {
		if (!useDatabase(database_name, key)) {
			std::cerr << "FAILURE: Could not USE DATABASE " << database_name << "!" << std::endl;
			return false;
		}

		std::list<std::string> object_type_names;
		if (!getObjectTypes(object_type_names)) {
			std::cerr << "FAILURE: Could not get list of object types in database " << database_name << "!" << std::endl;
			return false;
		}

		std::list<std::string>::const_iterator ci;
		ci = object_type_names.begin();
		while (ci != object_type_names.end()) {
			if (!dropIndicesOnObjectType(*ci)) {
				return false;
			}
			++ci;
		}

		// If we got this far, there were no errors
		return true;
	}
}

/** Create default indices on specific object type in specific database.
 *
 * First uses the database by calling EMdFDB::useDatabase() (or its
 * polymorphic instance), then calling
 * EMdFDB::createIndicesOnObjectType(const std::string&).
 *
 * Note that the encryption key is only used for SQLite 3, and only if
 * you have an SQLite 3 that supports encryption.
 *
 * @see EMdFDB::useDatabase(), EMdFDB::createIndicesOnObjectType(const
 * std::string&).
 *
 * @param database_name The database to use and drop indexes on.
 * @param key Just passed to EMdFDB::useDatabase()
 * @param object_type_name Name of the object type on which to create default indexes.
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::createIndicesOnObjectType(const std::string& database_name, const std::string& key, const std::string& object_type_name)
{
	if (pConn == 0)
		return false;
	else {
		if (!useDatabase(database_name, key)) {
			std::cerr << "FAILURE: Could not USE DATABASE " << database_name << "!" << std::endl;
			return false;
		}

		if (!createIndicesOnObjectType(object_type_name))
			return false;

		// If we got this far, there were no errors
		return true;
	}
}

/** Drop default indices on specific object type in specific database.
 *
 * First uses the database by calling EMdFDB::useDatabase() (or its
 * polymorphic instance), then calling
 * EMdFDB::dropIndicesOnObjectType(const std::string&).
 *
 * Note that the encryption key is only used for SQLite 3, and only if
 * you have an SQLite 3 that supports encryption.
 *
 * @see EMdFDB::useDatabase(), EMdFDB::dropIndicesOnObjectType(const
 * std::string&).
 *
 * @param database_name The database to use and drop indexes on.
 * @param key Just passed to EMdFDB::useDatabase()
 * @param object_type_name Name of the object type on which to drop default indexes.
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::dropIndicesOnObjectType(const std::string& database_name, const std::string& key, const std::string& object_type_name)
{
	if (pConn == 0)
		return false;
	else {
		if (!useDatabase(database_name, key)) {
			std::cerr << "FAILURE: Could not USE DATABASE " << database_name << "!" << std::endl;
			return false;
		}

		if (!dropIndicesOnObjectType(object_type_name))
			return false;

		// If we got this far, there were no errors
		return true;
	}
}

/** Create default indices on specific object type in current database.
 *
 * Calls EMdFDB::createIndicesOnOTObjects() (or its polymorphic variant).
 *
 * @see EMdFDB::createIndicesOnOTObjects(),
 *
 * @param object_type_name Name of the object type on which to create default indexes.
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::createIndicesOnObjectType(const std::string& object_type_name)
{
	eObjectRangeType objectRangeType;
	if (!getOTObjectRangeType(object_type_name, objectRangeType)) {
		DEBUG_X_FAILED("EMdFDB::createIndicesOnObjectType", "getting range type of object type " + object_type_name);
		return false;
	}

	// Create indices on OT_objects
	std::cerr << "Creating indices on " << object_type_name << "_objects..." << std::flush;
	createIndicesOnOTObjects(object_type_name);
	std::cerr << "!" << std::endl;

	return true;
}

/** Drop default indices on specific object type in current database.
 *
 * Calls EMdFDB::dropIndicesOnOTObjects(), and, if necessary,
 * EMdFDB::dropIndicesOnOTMonadMs() (or their polymorphic variants).
 *
 * @see EMdFDB::dropIndicesOnOTObjects(),
 * EMdFDB::dropIndicesOnOTMonadMs().
 *
 * @param object_type_name Name of the object type on which to drop default indexes.
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::dropIndicesOnObjectType(const std::string& object_type_name)
{
	bool bResult = true;

	// Drop indices on OT_objects
	std::cerr << "Dropping indices on " << object_type_name << "_objects..." << std::flush;
	bResult = dropIndicesOnOTObjects(object_type_name);
	std::cerr << "!" << std::endl;
	
	return bResult;
}

/** Drop default indices on OT_objects.
 *
 * @param object_type_name Name of the object type on which to drop default indexes.
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::dropIndicesOnOTObjects(const std::string& object_type_name)
{
	// Normalize object type name
	std::string OTN = normalizeOTName(object_type_name);


	// Get object type id
	id_d_t object_type_id;
	bool bExists;
	eObjectRangeType dummyRangeType;
	eMonadUniquenessType monadUniquenessType;
	if (!objectTypeExists(object_type_name, 
			      bExists,
			      object_type_id,
			      dummyRangeType,
			      monadUniquenessType)) {
		return false;
	}

	std::string index_name;

	switch (monadUniquenessType) {
	case kMUTNonUniqueMonads:
		index_name = OTN + "o_fm_i";
		break;
	case kMUTUniqueFirstMonads:
		index_name = OTN + "o_id_d_i";
		break;
	case kMUTUniqueFirstAndLastMonads:
		index_name = OTN + "o_id_d_i";
		break;
	default:
		ASSERT_THROW(false, "Unknown monad uniqueness type");
		break;
	}
  
	dropIndex(index_name, OTN + "_objects");


	// Get a list of features
	std::list<FeatureInfo> FeatureInfos;
	if (!getFeaturesForObjectType(object_type_id, FeatureInfos))
		return false;
	
	// Iterate over feature infos
	std::list<FeatureInfo>::const_iterator ci = FeatureInfos.begin();
	std::list<FeatureInfo>::const_iterator cend = FeatureInfos.end();
	while (ci != cend) {
		if (featureTypeIdIsWithIndex(ci->getType())) {
			std::string table_name;
			std::string index_name;
			getIndexAndTableNameForFeature(object_type_name, 
						       ci->getName(),
						       table_name, 
						       index_name);
			dropIndex(index_name, table_name);
		}
		/*
		if (featureTypeIdIsFromSet(ci->getType())) {
			if (!dropIndicesOnStringSetTable(object_type_name,
							 ci->getName())) {
				DEBUG_X_FAILED("EMdFDB::dropIndicesOnOTObjects",
					       "Dropping indices on FROM SET table.");
			}
		}
		*/
		++ci;
	}

	if (!dropMonadIndexOnOTObjects(object_type_name)) {
		DEBUG_X_FAILED("EMdFDB::dropIndicesOnOTObjects",
			       "Dropping monad index on OT_objects.");
	}
	
	// If we got this far, there were no DB erros.
	return true;
}

bool EMdFDB::dropMonadIndexOnOTObjects(const std::string& OTN)
{
	UNUSED(OTN);
	
	// The default implementation just succeeds without doing
	// anything
	return true;
}


/** Drop index.
 *
 * @param index_name The name of the index to drop.
 * @param table_name The name of the table which has the index to drop.
 * @return True on no database error, false if a database error occurred.
 */
void EMdFDB::dropIndex(const std::string& index_name, const std::string& table_name)
{
	if (pConn == 0)
		return;
	else {
		//
		// Create query
		//
		std::ostringstream query_stream;
		query_stream 
			<< "DROP INDEX " << normalizeIndexName(index_name);
		if (m_backend_kind == kMySQL) {
			query_stream << " ON " << normalizeTableName(table_name, false);
		}
		
		query_stream << '\n';

		//
		// Execute query
		// Don't bother checking result
		//
		pConn->execCommand(query_stream.str());


		// If we got this far, there were no errors
		return;
	}
}

/** Vacuum database.
 *
 * Vacuum database and optionally analyze it.  It is backend-dependent
 * what this actully does.
 *
 * This must be overridden in a descendant class if it is to have
 * any effect at all.
 *
 * @param bAnalyze Must be true if the user wants the ANALYZE option
 * to occur.  Exactly what the ANALYZE option does is dependent on the
 * implementation in the subclass.
 *
 * @return True on no database error, false if a database error occurred.
 */
bool EMdFDB::vacuum(bool bAnalyze)
{
	UNUSED(bAnalyze);

	// You should override it.
	return false;
}


/** Get backend-name as a string.
 *
 * Gets the name of the backend as a string.
 *
 * @see backend_kind2string(eBackendKind)
 *
 * @return The name of the backend.
 */
std::string EMdFDB::getBackendName(void)
{
	return backend_kind2string(m_backend_kind);
}

/** Construct an EMdFComparison from a non-IN comparison
 *@internal
 *
 * Part of the emdf_ffeatures.cpp machinery, which is for creating
 * pre-query strings in MQL.
 *
 * @param left_feature_name The feature name on the left-hand-side of
 * the comparison.
 *
 * @param left_type_id_d The feature type id of the feature mentioned
 * by left_feature_name.
 *
 * @param object_type_name The name of the object type holding the
 * feature on the left-hand-side.
 *
 * @param object_type_id The object type id of the object type holding
 * the feature on the left-hand-side.
 *
 * @param e The eComparisonOp of the comparison to use.
 *
 * @param right_hand_side The EMdFValue which holds the
 * right-hand-side value to compare with.
 *
 * @return The newly created EMdFComparison.
 */
EMdFComparison* EMdFDB::getEMdFComparison(const std::string& left_feature_name, 
					  id_d_t left_type_id_d, 
					  const std::string& object_type_name, 
					  id_d_t object_type_id, 
					  eComparisonOp e, 
					  EMdFValue *right_hand_side)
{
	return new EMdFComparison(left_feature_name, left_type_id_d, 
				  object_type_name, object_type_id, 
				  e, 
				  right_hand_side);
}

/** Construct an EMdFComparison from an IN comparison on an IntegerList
 *@internal
 *
 * Part of the emdf_ffeatures.cpp machinery, which is for creating
 * pre-query strings in MQL.
 *
 * @param left_feature_name The feature name on the left-hand-side of
 * the comparison.
 *
 * @param left_type_id_d The feature type id of the feature mentioned
 * by left_feature_name.
 *
 * @param object_type_name The name of the object type holding the
 * feature on the left-hand-side.
 *
 * @param object_type_id The object type id of the object type holding
 * the feature on the left-hand-side.
 *
 * @param pIntegerList This should give integers to test against.
 *
 * @return The newly created EMdFComparison.
 */
EMdFComparison *EMdFDB::getEMdFComparison(const std::string& left_feature_name, 
					  id_d_t left_type_id_d, 
					  const std::string& object_type_name,
					  id_d_t object_type_id,
					  const IntegerList *pIntegerList)
{
	return new EMdFComparison(left_feature_name, left_type_id_d, 
				  object_type_name, object_type_id,
				  pIntegerList);
}

/** Construct an EMdFComparison from an IN comparison on a list of EnumConstInfo
 *@internal
 *
 * Part of the emdf_ffeatures.cpp machinery, which is for creating
 * pre-query strings in MQL.
 *
 * @param left_feature_name The feature name on the left-hand-side of
 * the comparison.
 *
 * @param left_type_id_d The feature type id of the feature mentioned
 * by left_feature_name.
 *
 * @param object_type_name The name of the object type holding the
 * feature on the left-hand-side.
 *
 * @param object_type_id The object type id of the object type holding
 * the feature on the left-hand-side.
 *
 * @param in_enum_list This should give the enumeration constants to
 * test against.
 *
 * @return The newly created EMdFComparison.
 */
EMdFComparison *EMdFDB::getEMdFComparison(const std::string& left_feature_name, 
					  id_d_t left_type_id_d, 
					  const std::string& object_type_name,
					  id_d_t object_type_id,
					  const std::list<EnumConstInfo>& in_enum_list)
{
	return new EMdFComparison(left_feature_name, left_type_id_d, 
				  object_type_name, object_type_id,
				  in_enum_list);
}

/** Create an EMdFFFactor from an EMdFFFactor
 *@internal
 * 
 * Part of the emdf_ffeatures.cpp machinery, which is for creating
 * pre-query strings in MQL.
 *
 * Create an EMdFFFactor from an EMdFFFactor.
 *
 * @param ffactor The EMdFFFactor to use.
 *
 * @return The newly created EMdFFFactor.
 */
EMdFFFactor* EMdFDB::getEMdFFFactor(EMdFFFactor* ffactor)
{
	return new EMdFFFactor(ffactor);
}


/** Create an EMdFFFactor from an EMdFFFeatures.
 *@internal
 *
 * Part of the emdf_ffeatures.cpp machinery, which is for creating
 * pre-query strings in MQL.
 *
 * Create a new EMdFFFactor from ffeatures.
 *
 * @param ffeatures the EMdFFFeatures to use.
 *
 * @return The newly created EMdFFFactor.
 */
EMdFFFactor* EMdFDB::getEMdFFFactor(EMdFFFeatures* ffeatures)
{
	return new EMdFFFactor(ffeatures);
}


/** Create an EMdFFFactor from an EMdFComparison
 *@internal
 *
 * Part of the emdf_ffeatures.cpp machinery, which is for creating
 * pre-query strings in MQL.
 *
 * Create a new EMdFFFactor from the feature comparison.
 *
 * @param feature_comparison The EMdFComparison to use.
 *
 * @return The newly created EMdFFFactor.
 */
EMdFFFactor* EMdFDB::getEMdFFFactor(EMdFComparison* feature_comparison)
{
	return new EMdFFFactor(feature_comparison);
}


/** Create an EMdFFTerm from an EMdFFTerm and an EMdFFFactor.
 *@internal
 *
 * Part of the emdf_ffeatures.cpp machinery, which is for creating
 * pre-query strings in MQL.
 *
 * If fterm is nil, create an EMdFFTerm from just the
 * ffactor. Otherwise, create an EMdFFTerm from both fterm and
 * ffactor.
 *
 * @param fterm The EMdFFTerm to use.
 *
 * @param ffactor the EMdFFFactor to use.
 *
 * @return The EMdFFTerm which is a combination of the two.
 */
EMdFFTerm* EMdFDB::getEMdFFTerm(EMdFFTerm* fterm, EMdFFFactor* ffactor)
{
	EMdFFTerm* pResult;
	if (fterm == 0) {
		pResult = new EMdFFTerm(ffactor);
	} else {
		pResult = new EMdFFTerm(fterm, ffactor);
	}
	return pResult;
}


/** Create an EMdFFFeatures from an EMdFFFeatures and an EMFFFterm.
 *@internal
 *
 * Part of the emdf_ffeatures.cpp machinery, which is for creating
 * pre-query strings in MQL.
 *
 * If ffeatures is nil, create an EMdFFFeatures from just the
 * fterm. Otherwise, create an EMdFFFeatures from both ffeatures and
 * fterm.
 *
 * \section EMdFFFeatures Grammar
 * This is according to a grammar which looks like this:
 * \code
 *   ffeatures ::= fterm | ffeatures "OR" fterm
 *   fterm ::= ffactor | fterm "AND" ffactor
 *   ffactor ::= feature_comparison | "NOT" ffactor | "(" ffeatures ")"
 *   ffeature_comparison ::= left_hand_side eComparisonOp right_hand_side
 *                         | left_hand_side IN (list...)
 * \endcode
 *
 * @param ffeatures The EMdFFFeatures to use.
 *
 * @param fterm The EMdFFTerm to use.
 *
 * @return The EMdFFFeatures which is a combination of the two.
 */
EMdFFFeatures* EMdFDB::getEMdFFFeatures(EMdFFFeatures* ffeatures, EMdFFTerm* fterm)
{
	EMdFFFeatures* pResult;
	if (ffeatures == 0) {
		pResult = new EMdFFFeatures(fterm);
	} else {
		pResult = new EMdFFFeatures(ffeatures, fterm);
	}
	return pResult;
}




/** Convert an eComparisonOp to the backend's equivalent comparison operator.
 *@internal
 *
 * @param e The eComparisonOp to convert
 *
 * @return The string representing the backend's comparison operator
 * which is equivalent to \p e .
 */
std::string EMdFDB::eComparisonOp2BackendComparisonOp(eComparisonOp e) 
{
	std::string strResult;
	switch (e) {
	case kEqual:
		strResult = "=";
		break;
	case kLessThan:
		strResult = "<";
		break;
	case kGreaterThan:
		strResult = ">";
		break;
	case kNotEqual:
		strResult = "<>";
		break;
	case kLessThanOrEqual:
		strResult = "<=";
		break;
	case kGreaterThanOrEqual:
		strResult = ">=";
		break;
	case kIn:
		strResult = "IN";
		break;
	case kHas:
		strResult = "LIKE";
		break;
	default:
		ASSERT_THROW(false, "Error: eComparisonOp2SQLString: eComparisonOp does not have a counterpart in SQL!");
		break;
	}
	return strResult;
}

/** Tells us whether the backend can do a certain eComparisonOp operator.
 *@internal
 *
 * Part of the emdf_ffeatures.cpp machinery, which is for creating
 * pre-query strings in MQL.
 *
 * @param e The comparison operator to test.
 *
 * @param kind eEVkind corresponding to the left-hand-side feature.
 *
 * @return True if the backend can do the comparison op; false otherwise.
 */
bool EMdFDB::canDoComparisonOp(eComparisonOp e, eEVkind kind)
{
	bool bResult;
	switch (e) {
	case kIn:
		if (kind == kEVEnum
		    || kind == kEVInt
		    || kind == kEVID_D) {
			bResult = true;
		} else {
			bResult = false;
		}
		break;
	case kEqual:
	case kLessThan:
	case kGreaterThan:
	case kNotEqual:
	case kLessThanOrEqual:
	case kGreaterThanOrEqual:
		bResult = true;
		break;
	case kTilde:
	case kNotTilde:
		bResult = false;
		break;
	case kHas:
		if (kind == kEVListOfInteger
		    || kind == kEVListOfID_D) {
			bResult = true;
		} else {
			bResult = false;
		}
		break;
	default:
		// If I ever add more comparison operators, this will catch them.
		ASSERT_THROW(false, "Unknown eComparisonOp");
	}
	return bResult;
}

/** Convert a feature type id to an eEVkind.
 *@internal
 *
 * Returns the expected eEVKind from a feature type ID.
 *
 * @param type_id_d the feature type id to convert
 *
 * @return the eEVkind that corresponds to the feature type id.
 */
eEVkind EMdFDB::getEVkindFromTypeID_D(id_d_t type_id_d)
{
	eEVkind result;

	// Act on type
	switch (type_id_d & FEATURE_TYPE_TYPE_MASK) {
	case FEATURE_TYPE_LIST_OF_INTEGER:
		result = kEVListOfInteger;
		break;
	case FEATURE_TYPE_LIST_OF_ID_D:
		result = kEVListOfID_D;
		break;
	case FEATURE_TYPE_LIST_OF_ENUM:
		result = kEVListOfInteger;
		break;
	case FEATURE_TYPE_INTEGER:
		result = kEVInt;
		break;
	case FEATURE_TYPE_ASCII:
	case FEATURE_TYPE_STRING:
		result = kEVString;
		break;
	case FEATURE_TYPE_ID_D:
		result = kEVID_D;
		break;
	case FEATURE_TYPE_ENUM:
		result = kEVEnum;
		break;
	case FEATURE_TYPE_SET_OF_MONADS:
		result = kEVSetOfMonads;
		break;
	default:
		ASSERT_THROW(false, "Unknown FEATURE_TYPE.");
		break;
	}

	// Return result
	return result;
}

/** Convert a FeatureInfo to an SQL value, suitable for an SQL query.
 *@internal
 *
 * @param OTN The name of the object type which the feature is a
 * feature of.
 *
 * @param object_type_id The id_d of the object type mentioned by OTN.
 *
 * @param bCreateStringIDDIfNotThere Used only if the feature is a
 * STRING or ASCII feature which has been declared FROM SET.  If true,
 * and the string does not exist in the set, it is created.  If false,
 * the string is not created in the set even if it is not there.  This
 * is useful in MQL, where we must be able to match against a value
 * which is not in the set without adding it to the set.
 *
 * @param fi The FeatureInfo describing the feature to convert to SQL.
 *
 * @return The string representing the FeatureInfo as an SQL value.
 */
std::string EMdFDB::FeatureInfo2SQLvalue(const std::string& OTN,
					 id_d_t object_type_id,
					 bool bCreateStringIDDIfNotThere,
					 const FeatureInfo& fi)
{
	std::string result;

	// Get feature type
	id_d_t ft = fi.getType();

	// Make string
	if (featureTypeIdIsASCIIorSTRING(ft)) {
		if (featureTypeIdIsFromSet(ft)) {
			std::string default_value = fi.getDefaultValue();
			if (!getID_DFromStringSet(OTN, 
						  object_type_id,
						  encodeFeatureName(fi.getName()),
						  escapeStringForSQL(default_value),
						  default_value,
						  bCreateStringIDDIfNotThere,
						  result)) {
				throw EMdFDBDBError("Could not get ID_D from set in object type '" + OTN + "' and from feature '" + fi.getName() + "'.");
			}
		} else {
			result = escapeStringForSQL(fi.getDefaultValue());
		}
	} else if (featureTypeIdIsListOf(ft)) {
		// We don't need to mess with getMYSQL_BINARY() because 
		// it is just spaces and numbers.
		//
		// Once we move to lists of strings, however, we will need
		// getMYSQL_BINARY().
		result = escapeStringForSQL(fi.getDefaultValue());
	} else if (featureTypeIdIsSOM(ft)) {
		SetOfMonads som;
		som.fromString(fi.getDefaultValue());
		result = escapeStringForSQL(som.toCompactString());
	} else {
		// enum, integer, id_d
		result = fi.getDefaultValue();
	}

	// Return result
	return result;
}

/** Escape a string so that it can be used in an SQL 'string'.
 *@internal
 *
 * MUST be overridden in descendant classes.
 *
 * @param str The string to escape.
 * @return The escaped string.
 */
std::string EMdFDB::escapeStringForSQL(const std::string& str)
{
	UNUSED(str);

	ASSERT_THROW(false, "EMdFDB::escapeStringForSQL() should be overridden in descendant class, and not called directly!");

	// Fool the Visual C++ compiler into giving no error
	return "";
}

/** Get the string " BINARY " iff we are on MySQL; else the empty
 * string.
 *
 * Rationale: On MySQL, string comparisons are case-insensitive unless
 * either a) the column has been declared BINARY (for CHAR and
 * VARCHAR), b) the column has been declared as a BLOB rather than a
 * TEXT (for BLOBs), or c) the "BINARY" keyword precedes the literal
 * string to compare with.
 *
 * @return The string " BINARY " if we are on MySQL; otherwise, the
 * string "".
 */
std::string EMdFDB::getMYSQL_BINARY(void)
{
	if (m_backend_kind == kMySQL) {
		return std::string(" BINARY ");
	} else {
		return std::string();
	}
}

/** get the SQL type of TEXT columns.
 *
 * @return "TEXT" if we are doing PostgreSQL or kSQLite3, or
 * "LONGBLOB" if we are doing MySQL, and "TEXT" otherwise.
 */
const char *EMdFDB::getSQL_TEXT_TYPE() const
{
	if (m_backend_kind == kPostgreSQL) {
		return "TEXT";
	} else if (m_backend_kind == kSQLite3) {
		return "TEXT";
	} else if (m_backend_kind == kMySQL) {
		return "LONGBLOB";
 	} else {
		// Use this as a fall-back
		return "TEXT";
	}
}


int hash_djb2(const std::string& instr, long hashtable_size)
{    
	//  djb2
	// From http://www.cse.yorku.ca/~oz/hash.html

        uint32_t hash = 5381;
        uint32_t c;
	
	std::string::size_type i = 0;
	std::string::size_type length = instr.length();
	while (i < length) {
		c = (uint32_t) instr[i++];
		hash = ((hash << 5) + hash) ^ c; /* hash * 33 ^ c */
	}
        return hash % hashtable_size;
}

std::string getStringFromeComparisonOp(eComparisonOp op)
{
	switch (op) {
	case kEqual:
                return "=";
		break;
	case kLessThan:
		return "<"; 
		break;
	case kGreaterThan:
		return ">";
		break;
	case kNotEqual:
		return "<>";
		break;
	case kLessThanOrEqual:
		return "<=";
		break;
	case kGreaterThanOrEqual:
		return ">=";
		break;
	case kTilde:
                return "~";
		break;
	case kNotTilde:
		return "!=";
		break;
	case kIn:
		return "IN";
		break;
	case kHas:
		return "HAS";
		break;
	default:
		ASSERT_THROW(false,
			     "Unknown eComparisonOp.");
		return ""; // Silence some compilers' warnings
		break;
	}
}

