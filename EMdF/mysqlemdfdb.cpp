/*
 * mysqlemdfdb.cpp
 *
 * MySQL EMdF database base class
 *
 * Ulrik Petersen
 * Created: 1/27-2001
 * Last update: 11/2-2018
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


/**@file mysqlemdfdb.cpp
 *@brief Implementation of MySQL EMdF database (EMdF layer)
 */


#ifdef _WIN32
#include <windows.h>
#endif

#include <mysqlemdfdb.h>
#include <debug.h>
#include <inst_object.h>
#include <string_list.h>


#include <mysqlconn.h>


/** Construct a MySQLEMdFDB object
 *
 * @param host The TCP/IP hostname of the host to connect to.  May be
 * "localhost".
 *
 * @param user The MySQL user to connect as.
 *
 * @param passwd The MySQL password of the user \p user.
 *
 * @param database_name The MySQL database to connect to initially.
 * If this is "emdf", then the connection is deferred until the next
 * MySQLEMdFDB::useDatabase().
 */
MySQLEMdFDB::MySQLEMdFDB(std::string host, 
			 std::string user, 
			 std::string passwd, 
			 std::string database_name)
	: EMdFDB(kMySQL)
{
	m_max_table_name_length = 64;
#if defined(USE_MYSQL) && USE_MYSQL
	pConn = new MySQLEMdFConnection(host, user, passwd, database_name);
	useDatabase(database_name, std::string(""), false);
#else
	UNUSED(host);
	UNUSED(user);
	UNUSED(passwd);
	UNUSED(database_name);
	; // FIXME: What to do if not implemented?
#endif
}


/** Destructor
 */
MySQLEMdFDB::~MySQLEMdFDB()
{
}

#if defined(USE_MYSQL) && USE_MYSQL
/** Use database (i.e., connect to a new database)
 *
 * Connect to a database.  The user can call this method, or use the
 * MQL statement USE DATABASE.
 *
 * Just calls MySQLEMdFDB::useDatabase(db_name, key, false).
 *
 * Note that the encryption key is only used for SQLite, and only if
 * you have an SQLite that supports encryption.  Thus it should be ""
 * for this method.
 *
 * @param db_name the database name to be used.
 *
 * @param key is ignored.
 *
 * @return True on no database error, false if a database error occurred.
 */
bool MySQLEMdFDB::useDatabase(const std::string& db_name, const std::string& key)
{
	return useDatabase(db_name, key, false);
}


/** Use database (i.e., connect to a new database)
 *
 * Connect to a database.  The user can call this method, or use the
 * MQL statement USE DATABASE.
 *
 * First calls useDatabase on both connections, then calls
 * EMdFDB::useDatabase().
 *
 * Note that the encryption key is only used for SQLite, and only if
 * you have an SQLite that supports encryption.  Thus it should be ""
 * for this method.
 *
 * @param db_name the database name to be used.
 *
 * @param key is ignored.
 *
 * @param bIsInitializingOrCreating Must be false if we are not
 * creating or initializing the database, true otherwise.
 *
 * @return True on no database error, false if a database error occurred.
 */
bool MySQLEMdFDB::useDatabase(const std::string& db_name, const std::string& key, bool bIsInitializingOrCreating)
{
	if (pConn == 0)
		return false;
	else {
		// Convert name
		std::string NewDbName;
		convertDBName(db_name, NewDbName);
    
		// Ask the connection to do it.
		bool pConnResult = pConn->useDatabase(NewDbName, key);

		// Call base class
		if (!EMdFDB::useDatabase(db_name, key, bIsInitializingOrCreating)) {
			return false;
		}

		return pConnResult;
	}
}

/** Vacuum database
 *
 * For all object types, call MySQLEMdFDB::vacuumObjectType().
 *
 * @see MySQLEMdFDB::vacuumObjectType()
 *
 * @param bAnalyze Passed on to MySQLEMdFDB::vacuumObjectType().
 *
 * @return True on no DB error, false otherwise.
 */
bool MySQLEMdFDB::vacuum(bool bAnalyze)
{
	if (pConn == 0)
		return false;
	else {
		std::list<std::string> object_type_names;
		if (!getObjectTypes(object_type_names)) {
			return false;
		}

		std::list<std::string>::const_iterator ci;
		ci = object_type_names.begin();
		while (ci != object_type_names.end()) {
			if (!vacuumObjectType(*ci, bAnalyze)) {
				return false;
			}
			++ci;
		}
		return true;
	}
}


/** Vacuum object type
 *@internal
 * 
 * On the OT_objects table of the object type with name \p
 * object_type_name, run an OPTIMIZE TABLE command.  If in addition \p
 * bAnalyze is true, run an ANALYZE TABLE command on the table.
 *
 * @param object_type_name The name of the object type on which we
 * must run the command(s).
 *
 * @param bAnalyze If false, only an OPTIMIZE TABLE is issed.  If
 * true, an ANALYZE TABLE is issued as well.
 */
bool MySQLEMdFDB::vacuumObjectType(const std::string& object_type_name, bool bAnalyze)
{
	// OPTIMIZE TABLE OT_objects
	{
		std::ostringstream query_stream;
		query_stream << "OPTIMIZE TABLE " << normalizeTableName(object_type_name + "_objects", true) << ";";

		// Yeah yeah, I know it's a command, but it returns a table row,
		// so we must treat it as a SELECT.
		if (!pConn->execSelect(query_stream.str())) {
			DEBUG_SELECT_QUERY_FAILED("MySQLEMdFDB::vacuumObjectType", query_stream.str().c_str());
			return false;
		}

		// Free result
		pConn->finalize();
	}

	// ANALYZE TABLE
	if (bAnalyze) {
		// ANALYZE TABLE OT_objects
		{
			std::ostringstream query_stream;
			query_stream << "ANALYZE TABLE " << normalizeTableName(object_type_name + "_objects", true) << ";";

			// Yeah yeah, I know it's a command, but it returns a table row,
			// so we must treat it as a SELECT.
			if (!pConn->execSelect(query_stream.str())) {
				DEBUG_SELECT_QUERY_FAILED("MySQLEMdFDB::vacuumObjectType", query_stream.str().c_str());
				return false;
			}

			// Free result
			pConn->finalize();
		}
	}

	// If we got this far, there were no DB errors
	return true;
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
bool MySQLEMdFDB::tableExists(std::string table_name, bool&  /* out */ bTableExists)
{
	if (pConn == 0) {
		return false;
	} else {
		// Test that connection is OK
		if (!pConn->connectionOk()) {
			DEBUG_X_FAILED("MySQLEMdFDB::tableExists", "connectionOk()");
			return false;
		}

		// SELECT
		{
			std::ostringstream query_stream;
			query_stream 
				<< "SHOW TABLES LIKE '" << table_name << "'";
			if (!pConn->execSelect(query_stream.str().c_str())) {
				bTableExists = false;
			} else {
				// Assume it isn't there, until we retrieve one row.
				bTableExists = false;

				// Read row
				if (pConn->hasRow()) {
					bTableExists = true;
					bool bMoreTuples = true;
					while (bMoreTuples) {
						if (!pConn->getNextTuple(bMoreTuples)) {
							std::cerr << "MySQLEMdFDB::tableExists: getNextTuple returned error." << std::endl;
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
bool MySQLEMdFDB::getNextID(int sequence, id_d_t& out)
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
				"UPDATE sequence_%d\n"
				"SET sequence_value = LAST_INSERT_ID(sequence_value + 1);",
				sequence);
			if (!pConn->execCommand(szQuery)) {
				DEBUG_COMMAND_QUERY_FAILED("MySQLEMdFDB::getNextID", szQuery);
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}

			// Ask what the current value is
			sprintf(szQuery, "SELECT LAST_INSERT_ID();");
			if (!pConn->execSelect(szQuery)) {
				DEBUG_SELECT_QUERY_FAILED("MySQLEMdFDB::getNextID", szQuery);
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}

			// Access result
			if (!pConn->accessTuple(0, out)) {
				DEBUG_ACCESS_TUPLE_FAILED("MySQLEMdFDB::getNextID");
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}

			// Clean up result
			pConn->finalize();

			// Commit transaction
			if (bDoCommit) {
				if (!pConn->commitTransaction()) {
					DEBUG_COMMITTRANSACTION_FAILED("MySQLEMdFDB::getNextID");
					return false;
				}
			}
		} catch (EMdFNULLValueException& e) {
			// Abort transaction if in progress
			if (bDoCommit) {
				pConn->finalize();
				pConn->abortTransaction();
			}

			DEBUG_NULL_VALUE_EXCEPTION("MySQLEMdFDB::getNextID");

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
bool MySQLEMdFDB::setNextObjectID_DIfNotHigher(id_d_t next_id_d)
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
				DEBUG_SELECT_QUERY_FAILED("MySQLEMdFDB::setNextObjectID_DIfNotHigher", szQuery);
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}

			if (!pConn->hasRow()) {
				DEBUG_X_IS_WRONG("MySQLEMdFDB::setNextObjectID_DIfNotHigher", "there were no results.");
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}

			// Access result
			if (!pConn->accessTuple(0, current_id_d)) {
				DEBUG_ACCESS_TUPLE_FAILED("MySQLEMdFDB::setNextObjectID_DIfNotHigher");
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
					"UPDATE sequence_%d\n"
					"SET sequence_value = LAST_INSERT_ID(%ld);",
					SEQUENCE_OBJECT_ID_DS, next_id_d-1);
				if (!pConn->execCommand(szQuery)) {
					DEBUG_COMMAND_QUERY_FAILED("MySQLEMdFDB::setNextObjectID_DIfNotHigher", szQuery);
					if (bDoCommit)
						pConn->abortTransaction();
					return false;
				}
			}

			// Commit transaction
			if (bDoCommit) {
				if (!pConn->commitTransaction()) {
					DEBUG_COMMITTRANSACTION_FAILED("MySQLEMdFDB::setNextObjectID_DIfNotHigher");
					return false;
				}
			}
		} catch (EMdFNULLValueException& e) {
			// Abort transaction if in progress
			if (bDoCommit)
				pConn->abortTransaction();

			DEBUG_NULL_VALUE_EXCEPTION("MySQLEMdFDB::setNextObjectID_DIfNotHigher");

			// There was a NULL value exception, hence we return an error
			return false;
		}

		// If we got this far, the command succeeded.
		return true;
	}
}

/** Create database.
 *@internal
 *
 * Note that the encryption key is only used for BPT and SQLite,
 * provided you have an SQLite with encryption enabled (available from
 * Emdros's author).  Thus it should be "" for this method.
 *
 * @param db_name the database name to be created
 * @param key the encryption key to be used.
 * @return True on no database error, false if a database error occurred.
 */
bool MySQLEMdFDB::createDatabase(const std::string& db_name, const std::string& key)
{
	return this->createDatabase(db_name, getDefaultEncoding(), key);
}



/** Create database
 *
 * Creates the database and calls EMdFDB::initializeDatabase().
 *
 * Note that the encryption key is only used for SQLite, and only if
 * you have an SQLite that supports encryption.  Thus it should be ""
 * for this method.
 *
 * @param db_name the database name to be created.
 * @param charset the default database-wide charset to use.
 * @param key the encryption key (passed directly to EMdFDB)
 * @return True on no database error, false if a database error occurred.
 */
bool MySQLEMdFDB::createDatabase(const std::string& db_name, eCharsets charset, const std::string& key)
{
	// Get a DBname which we can use
	std::string NewDbName;
	convertDBName(db_name, NewDbName);

	if (!canDeclareEncoding(charset)) {
		DEBUG_X_IS_WRONG("MySQLEMdFDB::createDatabase", "Cannot use this charset with this backend.");
		return false;
	}


	// Create database
	std::ostringstream query_stream;
	query_stream << "CREATE DATABASE " << NewDbName
		     << ' ' << getEncodingClause(charset);
	if (!pConn->execCommand(query_stream.str())) {
		DEBUG_COMMAND_QUERY_FAILED("MySQLEMdFDB::createDatabase", query_stream.str().c_str());
		return false;
	}
	
	return EMdFDB::initializeDatabase(db_name, key);
}




/** Create sequence tables.
 *@internal
 *
 * Create the three sequence tables.
 *
 * @see MySQLEMdFDB::getNextID() for an introduction to the two
 * sequences.
 *
 * @return True on no database error, false if a database error occurred.
 */
bool MySQLEMdFDB::createSequenceTables()
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
			"   sequence_id INT NOT NULL,\n"
			"   sequence_value INT NOT NULL,\n"
			"   PRIMARY KEY (sequence_id)\n"
			")", SEQUENCE_OBJECT_ID_DS);
		if (!pConn->execCommand(szQuery)) {
			DEBUG_COMMAND_QUERY_FAILED("MySQLEMdFDB::createSequenceTables118", szQuery);
			return false;
		}

		// Create table/sequence 'sequence_1'
		sprintf(szQuery, "CREATE TABLE sequence_%d (\n"
			"   sequence_id INT NOT NULL,\n"
			"   sequence_value INT NOT NULL,\n"
			"   PRIMARY KEY (sequence_id)\n"
			")", SEQUENCE_TYPE_IDS);
		if (!pConn->execCommand(szQuery)) {
			DEBUG_COMMAND_QUERY_FAILED("MySQLEMdFDB::createSequenceTables118", szQuery);
			return false;
		}

		// Create table/sequence 'sequence_2'
		sprintf(szQuery, "CREATE TABLE sequence_%d (\n"
			"   sequence_id INT NOT NULL,\n"
			"   sequence_value INT NOT NULL,\n"
			"   PRIMARY KEY (sequence_id)\n"
			")", SEQUENCE_OTHER_IDS);
		if (!pConn->execCommand(szQuery)) {
			DEBUG_COMMAND_QUERY_FAILED("MySQLEMdFDB::createSequenceTables118", szQuery);
			return false;
		}



		// Seed sequence table with start value for SEQUENCE_OBJECT_ID_DS
		sprintf(szQuery,
			"INSERT INTO sequence_%d (sequence_id, sequence_value)\n"
			"VALUES ( %d, %d )", 
			SEQUENCE_OBJECT_ID_DS, 
			SEQUENCE_OBJECT_ID_DS, 
			SEQUENCE_START-1);
		if (!pConn->execCommand(szQuery)) {
			DEBUG_COMMAND_QUERY_FAILED("MySQLEMdFDB::createSequenceTables118", szQuery);
			return false;
		}

		// Seed sequence table with start value for SEQUENCE_TYPE_ID_DS
		sprintf(szQuery,
			"INSERT INTO sequence_%d (sequence_id, sequence_value)\n"
			"VALUES ( %d, %d )", 
			SEQUENCE_TYPE_IDS, 
			SEQUENCE_TYPE_IDS, 
			SEQUENCE_START-1);
		if (!pConn->execCommand(szQuery)) {
			DEBUG_COMMAND_QUERY_FAILED("MySQLEMdFDB::CreateSequenceTables", szQuery);
			return false;
		}

		// Seed sequence table with start value for SEQUENCE_OTHER_ID_DS
		sprintf(szQuery,
			"INSERT INTO sequence_%d (sequence_id, sequence_value)\n"
			"VALUES ( %d, %d )", 
			SEQUENCE_OTHER_IDS, 
			SEQUENCE_OTHER_IDS, 
			SEQUENCE_START-1);
		if (!pConn->execCommand(szQuery)) {
			DEBUG_COMMAND_QUERY_FAILED("MySQLEMdFDB::CreateSequenceTables", szQuery);
			return false;
		}

		// Commit transaction if necessary
		if (bDoCommit)
			pConn->commitTransaction();

		return true;
	}
}

/** Escape a string so that it can be used in an SQL 'string'.
 *@internal
 *
 * @param str The string to escape.
 * @return The escaped string.
 */
std::string MySQLEMdFDB::escapeStringForSQL(const std::string& str)
{
	ASSERT_THROW(pConn != 0, "pConn mustn't be nil (0).");
	// We can't use dynamic_cast, since we don't support/include RTTI
	// on Visual C++.
	return ((MySQLEMdFConnection*)(pConn))->escapeStringForSQL(str);
}


bool MySQLEMdFDB::lockTablesForCreateObjects(const std::string& object_type_name, const std::list<FeatureInfo>& object_type_features)
{
	if (pConn == 0) {
		return false;
	}

	std::string query;
	query = "LOCK TABLES " + normalizeTableName(object_type_name + "_objects", true) + " WRITE, min_m WRITE, max_m WRITE, object_types WRITE";
	query += ", sequence_" + int2string(SEQUENCE_OBJECT_ID_DS) + " WRITE";
	query += ", sequence_" + int2string(SEQUENCE_TYPE_IDS) + " WRITE";
	query += ", sequence_" + int2string(SEQUENCE_OTHER_IDS) + " WRITE";
	if (!object_type_features.empty()) {
		std::list<FeatureInfo>::const_iterator ci = object_type_features.begin();
		std::list<FeatureInfo>::const_iterator cend = object_type_features.end();
		while (ci != cend) {
			// Get feature's type
			id_d_t feature_type_id = ci->getType();
			switch (feature_type_id & FEATURE_TYPE_TYPE_MASK) {
			case FEATURE_TYPE_ASCII:
			case FEATURE_TYPE_STRING:
				if (featureTypeIdIsFromSet(feature_type_id)) {
					std::string feature_name = ci->getName();
					std::string encoded_feature_name = encodeFeatureName(feature_name);
					std::string table_name = object_type_name 
						+ "_" + encoded_feature_name + "_set";
					query += ", " + normalizeTableName(table_name, true) + " WRITE";
				}
				break;
			default:
				// Only treat ASCII and STRING.
				break;
			}
			
			++ci;
		}
	}
	if (!pConn->execCommand(query)) {
		DEBUG_COMMAND_QUERY_FAILED("MySQLEMdFDB::unlockTablesForCreateObjects", query);
		return false;
	}

	return true;
}

bool MySQLEMdFDB::unlockTablesForCreateObjects(const std::string& object_type_name)
{
	UNUSED(object_type_name);
	
	if (pConn == 0) {
		return false;
	}

	std::string query;
	query = "UNLOCK TABLES";
	if (!pConn->execCommand(query)) {
		DEBUG_COMMAND_QUERY_FAILED("MySQLEMdFDB::unlockTablesForCreateObjects", query);
		return false;
	}

	return true;
}



/** Batch-create objects, create a line of OT_objects data in a string.
 *@internal
 *
 * This is part of the machinery for the MQL "CREATE OBJECTS WITH
 * OBJECT TYPE.".  It is used by EMdFDB::createObjects() to create one
 * object's worth of output data in a std::string, to be sent to the
 * backend later by MySQLEMdFDB::createObjectsOT_objects_DB().
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
void MySQLEMdFDB::createObjectsOT_objects_data(const std::string object_type_name, 
					       id_d_t object_type_id,
					       const InstObject *pObject,
					       const SetOfMonads& som,
					       const std::list<FeatureInfo>& object_type_features,
					       eObjectRangeType objectRangeType,
					       /* out */ std::string& OT_objects_data)
{
	OT_objects_data += '(';

	// object id_d
	OT_objects_data += id_d2number_string(pObject->getID_D());
	OT_objects_data += ',';

	// First monad
	OT_objects_data += monad_m2string(som.first());

	// Last monad
	if (objectRangeType != kORTSingleMonad) {
		OT_objects_data += "," + monad_m2string(som.last());
	}

	// Monads
	if (objectRangeType == kORTMultipleRange) {
		OT_objects_data += "," + escapeStringForSQL(som.toCompactString());
	}


	std::list<FeatureInfo>::const_iterator ci = object_type_features.begin();
	std::list<FeatureInfo>::const_iterator cend = object_type_features.end();
	int index = 0;
	while (ci != cend) {
		// Emit comma after previous column
		OT_objects_data += ',';

		// Access object's feature at the given index 
		const EMdFValue *pValue = pObject->getFeature(index);

		// Get feature's type
		id_d_t feature_type_id = ci->getType();
		IntegerList *pIntegerList = 0;
		switch (feature_type_id & FEATURE_TYPE_TYPE_MASK) {
		case FEATURE_TYPE_ASCII:
			if (featureTypeIdIsFromSet(feature_type_id)) {
				FeatureInfo myfi(ci->getName(), ci->getType(), pValue->getString(), ci->getIsComputed());
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
				FeatureInfo myfi(ci->getName(), ci->getType(), pValue->getString(), ci->getIsComputed());
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
				// Emit tab after previous column
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
				OT_objects_data += '\'' + pIntegerList->getDelimitedString(DEFAULT_LIST_DELIMITER) + '\'';
			} else {
				OT_objects_data += "''";
			}
			break;
		default:
			ASSERT_THROW(false, "Unknown FEATURE_TYPE");
			break;
		}

		// Advance index and iterator
		++index;
		++ci;
	}

	OT_objects_data += ")";
}


/** Batch-create objects, send batch to the backend.
 *@internal
 *
 * This is part of the machinery for the MQL "CREATE OBJECTS WITH
 * OBJECT TYPE.".  It is used by EMdFDB::createObjects() to send the
 * current batch of objects to the backend.
 *
 * @see EMdFDB::createObjects(),
 * MySQLEMdFDB::::createObjectsOT_objects_data().
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
bool MySQLEMdFDB::createObjectsOT_objects_DB(const std::string& object_type_name,
					     const std::string& OT_objects_data,
					     eObjectRangeType objectRangeType,
					     const std::list<FeatureInfo>& object_type_features)
{
	std::string query;
	query += "INSERT INTO ";
	query += normalizeTableName(object_type_name + "_objects", true);
	query += "(object_id_d,first_monad";
	if (objectRangeType != kORTSingleMonad) {
		query += ",last_monad";
	}
	if (objectRangeType == kORTMultipleRange) {
		query += ",monads";
	}
	std::list<FeatureInfo>::const_iterator ci = object_type_features.begin();
	std::list<FeatureInfo>::const_iterator cend = object_type_features.end();
	while (ci != cend) {
		query += ',';
		query += encodeFeatureName(ci->getName());
		if (featureTypeIdIsSOM(ci->getType())) {
			query += ",first_monad_" + encodeFeatureName(ci->getName());
		}
		++ci;
	}
	query += ")VALUES";
	query += OT_objects_data;
	query += ';';

	if (!pConn->execCommand(query)) {
		DEBUG_COMMAND_QUERY_FAILED("MySQLEMdFDB::createObjectsOT_objects_DB", query);
		return false;
	}
	return true;
}

/** Batch-create objects, create what is necessary in between each
 *  object record
 *@internal
 *
 * This is part of the machinery for the MQL "CREATE OBJECTS WITH
 * OBJECT TYPE.".  It is used by EMdFDB::createObjects() and others to
 * create the in-between-records marker for the std::string containing
 * the batch.  For MySQL, this is a comma.
 *
 * @see EMdFDB::createObjects()
 *
 * @param OT_data The std::string containing the batch which we've
 * built so far.  Is also used to return the data.
 */
void MySQLEMdFDB::createObjectsInBetweenData(/* out */ std::string& OT_data)
{
	OT_data += ',';
}


eCharsets MySQLEMdFDB::getDefaultEncoding(void) const
{
	return kCSISO_8859_1;
}


bool MySQLEMdFDB::canDeclareEncoding(eCharsets charset) const
{
	return charset == kCSISO_8859_1 || charset == kCSUTF8;
}


std::string MySQLEMdFDB::getEncodingClause(eCharsets charset) const
{
	/* There is nothing to do. The reason is that we cannot change
           it at compile time, and even though the UTF8 is chosen
           instead of iso-8859-1, we don't use the DB in such a way
           that the DB will care.  NOTE: This may change if we decide
           to store anything which needs a LIKE comparison (such as a
           list of strings...).
	*/

	// The DEFAULT CHARACTER SET syntax was introduced in MySQL 4.1.1.
	// Hence, we need to put it inside a comment which is only
	// executed if the version is 4.1.1 or higher.
	// This is done by saying '/*!40101', and closing the comment
	// at the end.
	std::string result = "/*!40101 DEFAULT CHARACTER SET ";
	if (charset == kCSUTF8) {
		result += "'utf8'";
	} else if (charset == kCSISO_8859_1) {
		result += "'latin1'";
	} else {
		ASSERT_THROW(false, "Unknown charset in MySQLEMdFDB::getEncodingClause");
	}
	result += " */";

	return result;
}




#endif // USE_MYSQL
