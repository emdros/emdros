/*
 * pgemdfdb.cpp
 *
 * PostgreSQL EMdF database base class
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


/**@file pgemdfdb.cpp
 *@brief Implementation of PostgreSQL EMdF database (EMdF layer)
 */


#include <pgemdfdb.h>
#include <debug.h>
#include <utils.h>
#include <encryption.h>
#include <inst_object.h>
#include <string_list.h>

#if defined(USE_POSTGRESQL) && USE_POSTGRESQL
#include <pgconn.h>
#endif

/** Construct a PgEMdFDB object
 *
 * @param host The TCP/IP hostname of the host to connect to.  May be
 * "localhost".
 *
 * @param user The PostgreSQL user to connect as.
 *
 * @param passwd The PostgreSQL password of the user \p user.
 *
 * @param database_name The PostgreSQL database to connect to
 * initially.  If this is "emdf", then the connection is deferred
 * until the next PgEMdFDB::useDatabase().
 */
PgEMdFDB::PgEMdFDB(std::string host, 
		   std::string user, 
		   std::string passwd, 
		   std::string database_name) 
	: EMdFDB(kPostgreSQL),
	  m_host(host), m_user(user), m_passwd(passwd)
{
	m_max_table_name_length = 63;
#if defined(USE_POSTGRESQL) && USE_POSTGRESQL
	pConn = 0;
	encryptString(m_passwd);

	useDatabase(database_name, 
		    std::string("") // empty key... we don't use keys on PostgreSQL
		    );
#else
	UNUSED(host);
	UNUSED(user);
	UNUSED(passwd);
	UNUSED(database_name);
	; // FIXME: What to do if it is not implemented
#endif
}

/** Destructor
 */
PgEMdFDB::~PgEMdFDB()
{
}


#if defined(USE_POSTGRESQL) && USE_POSTGRESQL
/** Connect to a PostgreSQL database
 *@internal
 *
 * @param database_name The name of the PostgreSQL database to connect
 * to.  May be "emdf".
 */
void PgEMdFDB::connect(const std::string database_name)
{
	// Make sure we have a clean slate
	delete pConn;
    
	std::string NewDbName;
	convertDBName(database_name, NewDbName);

	// Decrypt password momentarily
	std::string unencrypted_password = m_passwd;
	unencryptString(unencrypted_password);

	std::string pConnDBName;
	if (strcmp_nocase(NewDbName, "emdf") == 0) {
		pConnDBName = "template1";
		pConn = new PgEMdFConnection(m_host, m_user, unencrypted_password, pConnDBName);
		if (pConn->connectionOk()) {
			;

		} else {
			delete pConn;
			pConnDBName = "postgres";			
			pConn = new PgEMdFConnection(m_host, m_user, unencrypted_password, pConnDBName);
		}
	} else {
		pConnDBName = NewDbName;

		pConn = new PgEMdFConnection(m_host, m_user, unencrypted_password, pConnDBName);
	}

	// Guard against an attack of looking at the memory or core-dumping
	zeroFillString(unencrypted_password);
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
bool PgEMdFDB::createDatabase(const std::string& db_name, const std::string& key)
{
	return this->createDatabase(db_name, getDefaultEncoding(), key);
}




/** Create database
 *
 * Creates the database and calls EMdFDB::initializeDatabase().
 *
 * Note that the encryption key is only used for BPT and SQLite,
 * provided you have an SQLite with encryption enabled (avaialble from
 * Emdros's author).  Thus it should be "" for this method.
 *
 * @param db_name the database name to be created.
 * @param charset the default database-wide charset to use.
 * @param key the encryption key (passed directly to EMdFDB)
 * @return True on no database error, false if a database error occurred.
 */
bool PgEMdFDB::createDatabase(const std::string& db_name, eCharsets charset, const std::string& key)
{
	// Get a DBname which we can use
	std::string NewDbName;
	convertDBName(db_name, NewDbName);

	if (!canDeclareEncoding(charset)) {
		DEBUG_X_IS_WRONG("PgEMdFDB::createDatabase", "Cannot use this charset with this backend.");
		return false;
	}

	// Create database
	std::ostringstream query_stream;
	query_stream << "CREATE DATABASE " << NewDbName;
	query_stream << ' ' << getEncodingClause(charset)
		     << " TEMPLATE template0";
	if (!pConn->execCommand(query_stream.str())) {
		DEBUG_COMMAND_QUERY_FAILED("PgEMdFDB::createDatabase", query_stream.str().c_str());
		return false;
	}
	return EMdFDB::initializeDatabase(db_name, key);
}


/** Use database (i.e., connect to a new database) without creating
 *
 * Just calls PgEMdFDB::useDatabase(db_name, key, false).
 *
 * @param db_name the database name to be used.
 * @param key the encryption key (passed directly to EMdFDB)
 * @return True on no database error, false if a database error occurred.
 */
bool PgEMdFDB::useDatabase(const std::string& db_name, const std::string& key)
{
	return useDatabase(db_name, key, false);
}

/** Use database.
 *
 * Connect to a database.  The user can call this method, or use the
 * MQL statement USE DATABASE.
 *
 * Should be overridden in a subclass, since this doesn't actually
 * connect -- it just initializes the caches.  However, the subclass
 * should remember to call this method.
 *
 * Note that the encryption key is only used for SQLite, and only if
 * you have an SQLite that supports encryption.
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
bool PgEMdFDB::useDatabase(const std::string& db_name, const std::string& key, bool bIsInitializingOrCreating)
{
	connect(db_name);

	// Call base class (this cleans up some structures like caches)
	if (!EMdFDB::useDatabase(db_name, key, bIsInitializingOrCreating)) {
		return false;
	}

	return pConn->connectionOk();
}


/** Vacuum database
 *
 * Defers the actual execution to PgConn::vacuum().
 *
 *
 * @param bAnalyze Tells the PgConn object whether we should analyze
 * as well as vacuum.  If true, the command will be "VACUUM ANALYZE"
 * rather than just "VACUUM".
 *
 * @return True on no DB error, false otherwise.
 */
bool PgEMdFDB::vacuum(bool bAnalyze)
{
	if (pConn == 0)
		return false;
	else {
		return ((PgEMdFConnection*)pConn)->vacuum(bAnalyze);
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
bool PgEMdFDB::tableExists(std::string table_name, bool&  /* out */ bTableExists)
{
	if (pConn == 0) {
		return false;
	} else {
		// Test that connection is OK
		if (!pConn->connectionOk()) {
			DEBUG_X_FAILED("PgEMdFDB::tableExists", "connectionOk()");
			return false;
		}

		// SELECT
		{
			std::ostringstream query_stream;
			query_stream 
				<< "SELECT EXISTS (\n"
				<< "   SELECT 1 \n"
				<< "   FROM   pg_catalog.pg_class c\n"
				<< "   JOIN   pg_catalog.pg_namespace n ON n.oid = c.relnamespace\n"
				<< "   WHERE  c.relname = '" << table_name << "'\n"
				<< "   AND    c.relkind = 'r'    -- only tables(?)\n"
				<< ")";
			if (!pConn->execSelect(query_stream.str().c_str())) {
				bTableExists = false;
				return false;
			} else {
				// Assume it isn't there, until we retrieve one row.
				bTableExists = false;

				// Read row
				if (pConn->hasRow()) {
					std::string result;
					if (!pConn->accessTuple(0, result)) {
						std::cerr << "PgEMdFDB::tableExists: accessTuple returned error." << std::endl;
						return false;
					}
					if (result == "t") {
						bTableExists = true;
					} else {
						bTableExists = false;
					}
				}
			}

			pConn->finalize();
		}

		// If we got this far, there were no errors
		return true;
	}
}




/** Get next id_d from a given sequence.
 *@internal
 *
 * There are three sequences: SEQUENCE_OBJECT_ID_DS,
 * SEQUENCE_TYPE_IDS, and SEQUENCE_OTHER_IDS (defined in emdf.h).
 * This method gets the next id_d from the given sequence.
 *
 * @return True on no database error, false if a database error occurred.
 */
bool PgEMdFDB::getNextID(int sequence, id_d_t& out)
{
	if (pConn == 0)
		return false;
	else {
		// Declare szQuery
		char szQuery[512];

		// Begin transaction
		bool bDoCommit = pConn->beginTransaction();

		try {
			// Ask what the next value is
			sprintf(szQuery, "SELECT nextval('sequence_%d');", sequence);
			if (!pConn->execSelect(szQuery)) {
				DEBUG_SELECT_QUERY_FAILED("PgEMdFDB::getNextID", szQuery);
				if (bDoCommit)
					pConn->abortTransaction();
				return false;
			}

			// Access result
			if (!pConn->accessTuple(0, out)) {
				DEBUG_ACCESS_TUPLE_FAILED("PgEMdFDB::getNextID");

				if (bDoCommit)
					pConn->abortTransaction();

				return false;
			}

			// Clean up result
			pConn->finalize();

			// Commit transaction
			if (bDoCommit) {
				if (!pConn->commitTransaction()) {
					DEBUG_COMMITTRANSACTION_FAILED("PgEMdFDB::getNextID");
					return false;
				}
			}
		} catch (EMdFNULLValueException& e) {
			// Abort transaction if in progress
			if (bDoCommit) {
				pConn->finalize();
				pConn->abortTransaction();
			}

			DEBUG_NULL_VALUE_EXCEPTION("PgEMdFDB::getNextID");

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
bool PgEMdFDB::setNextObjectID_DIfNotHigher(id_d_t next_id_d)
{
	if (pConn == 0)
		return false;
	else {
		// Declare szQuery
		char szQuery[512];

		// NOTE: As per the PostgreSQL docs, it doesn't make sense to
		// encase this in a transaction, so I don't.  See the PostgreSQL
		// docs:
		//
		// file:///usr/share/doc/postgresql/html/functions-sequence.html

		try {
			// Get current value, so that we know what it is right now.
			//
			// See file:///usr/share/doc/postgresql/html/sql-createsequence.html
			//
			// This may cause problems if other concurrent sessions are
			// calling nextval concurrently.
			//
			// NOTE: We cannot use currval, because currval assumes that nextval
			// has been called in this session.  See the PostgreSQL docs:
			//
			// file:///usr/share/doc/postgresql/html/functions-sequence.html
			//
			id_d_t current_id_d;
			sprintf(szQuery, "SELECT last_value FROM sequence_%d;", SEQUENCE_OBJECT_ID_DS);
			if (!pConn->execSelect(szQuery)) {
				DEBUG_SELECT_QUERY_FAILED("PgEMdFDB::setNextObjectID_DIfNotHigher", szQuery);
				return false;
			}

			if (!pConn->hasRow()) {
				DEBUG_X_IS_WRONG("PgEMdFDB::setNextObjectID_DIfNotHigher", "there were no results.");
				return false;
			}

			// Access result
			if (!pConn->accessTuple(0, current_id_d)) {
				DEBUG_ACCESS_TUPLE_FAILED("PgEMdFDB::setNextObjectID_DIfNotHigher");
				return false;
			}

			// Clean up result
			pConn->finalize();

			// Only update if current is less than next minus 1
			if (current_id_d < next_id_d-1) {
				// Set current value to one less than next_id_d, so that
				// next time, we will increment to next_id_d.
				sprintf(szQuery, "SELECT setval('sequence_%d', %ld);", SEQUENCE_OBJECT_ID_DS, next_id_d-1);
				if (!pConn->execSelect(szQuery)) {
					DEBUG_SELECT_QUERY_FAILED("PgEMdFDB::setNextObjectID_DIfNotHigher", szQuery);
					return false;
				}

				// Clean up
				pConn->finalize();
			}
		} catch (EMdFNULLValueException& e) {
			DEBUG_NULL_VALUE_EXCEPTION("PgEMdFDB::setNextObjectID_DIfNotHigher");

			// There was a NULL value exception, hence we return an error
			return false;
		}

		// If we got this far, the command succeeded.
		return true;
	}
}


/** Create sequence tables.
 *@internal
 *
 * Create the three sequence tables.
 *
 * @see PgEMdFDB::getNextID() for an introduction to the three
 * sequences.
 *
 * @return True on no database error, false if a database error occurred.
 */
bool PgEMdFDB::createSequenceTables()
{
	if (pConn == 0)
		return false;
	else {
		// Declare szQuery
		char szQuery[512];

		// Begin transaction
		bool bDoCommit = pConn->beginTransaction();

		// Create table/sequence 'sequence_0'
		sprintf(szQuery,
			"CREATE SEQUENCE sequence_%d\n"
			"START %d;", SEQUENCE_OBJECT_ID_DS, SEQUENCE_START);
		if (!pConn->execCommand(szQuery)) {
			DEBUG_COMMAND_QUERY_FAILED("PgEMdFDB::createSequenceTables", szQuery);
			if (bDoCommit)
				pConn->abortTransaction();
			return false;
		}

		// Create table/sequence 'sequence_1'
		sprintf(szQuery,
			"CREATE SEQUENCE sequence_%d\n"
			"START %d;", SEQUENCE_TYPE_IDS, SEQUENCE_START);
		if (!pConn->execCommand(szQuery)) {
			DEBUG_COMMAND_QUERY_FAILED("PgEMdFDB::createSequenceTables", szQuery);
			if (bDoCommit)
				pConn->abortTransaction();
			return false;
		}

		// Create table/sequence 'sequence_2'
		sprintf(szQuery,
			"CREATE SEQUENCE sequence_%d\n"
			"START %d;", SEQUENCE_OTHER_IDS, SEQUENCE_START);
		if (!pConn->execCommand(szQuery)) {
			DEBUG_COMMAND_QUERY_FAILED("PgEMdFDB::createSequenceTables", szQuery);
			if (bDoCommit)
				pConn->abortTransaction();
			return false;
		}

		// Commit transaction if necessary
		if (bDoCommit)
			pConn->commitTransaction();

		return true;
	}
}

/** Escape a string so that it can be used with the COPY command.
 *
 * This is used in the EMdFDB::createObjects() machinery.
 *
 * @param str The string to escape.
 *
 * @return The escaped string.
 */
std::string PgEMdFDB::escapeStringForCOPY(const std::string& str)
{
	std::string result;
	std::string::const_iterator ci = str.begin();
	std::string::const_iterator cend = str.end();
	for (; ci != cend; ++ci) {
		unsigned char c = *ci;
		if (c == '\b') {
			result += "\\b";
		} else if (c == '\f') {
			result += "\\f";
		} else if (c == '\n') {
			result += "\\n";
		} else if (c == '\r') {
			result += "\\r";
		} else if (c == '\t') {
			result += "\\t";
		} else if (c == '\v') {
			result += "\\v";
		} else if (c == '\\') {
			result += "\\\\";
		} else if (c > 127
			   || c < 32) {
			result += '\\';
			result += char2octal(c);
		} else {
			result += c;
		}
	}
	return result;
}

/** Escape a string so that it can be used in an SQL 'string'.
 *@internal
 *
 * @param str The string to escape.
 * @return The escaped string.
 */
std::string PgEMdFDB::escapeStringForSQL(const std::string& str)
{
	// Malloc a piece of memory that is twice the input length + 1
	// as the PostgreSQL manual demands.
	char *strTo = (char*) malloc(str.length()*2 + 1);

	// Call PostgreSQL libpq function to escape string.
	// This fills the buffer strTo with the escaped version of 
	// the second argument, and processes up to the number of 
	// bytes in the third argument.
	// The return value is the number of bytes in the strTo buffer,
	// excluding the extra zero byte which the buffer is padded with.
	size_t newlength = PQescapeString(strTo, str.c_str(), str.length());

	// This std::string constructor is described on page 585 in
	// Stroustrup, TCPPPL, 3rd ed.
	std::string result = "'" + std::string(strTo, newlength) + "'";

	// Free the memory, since it has been copied to the result
	free(strTo);

	return result;
}


/** Batch-create objects, create a line of OT_objects data in a string.
 *@internal
 *
 * This is part of the machinery for the MQL "CREATE OBJECTS WITH
 * OBJECT TYPE.".  It is used by EMdFDB::createObjects() to create one
 * object's worth of output data in a std::string, to be sent to the
 * backend later by PgEMdFDB::createObjectsOT_objects_DB().
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
void PgEMdFDB::createObjectsOT_objects_data(const std::string object_type_name,
					    id_d_t object_type_id,
					    const InstObject *pObject,
					    const SetOfMonads& som,
					    const std::list<FeatureInfo>& object_type_features,
					    eObjectRangeType objectRangeType,
					    /* out */ std::string& OT_objects_data)
{
	// object id_d
	OT_objects_data += id_d2number_string(pObject->getID_D());
	OT_objects_data += '\t';

	// First monad
	OT_objects_data += monad_m2string(som.first());

	// Last monad
	if (objectRangeType != kORTSingleMonad) {
		OT_objects_data += '\t';
		OT_objects_data += monad_m2string(som.last());
	}

	// Monads
	if (objectRangeType == kORTMultipleRange) {
		OT_objects_data += '\t';
		OT_objects_data += escapeStringForCOPY(som.toCompactString());
	}

	std::list<FeatureInfo>::const_iterator ci = object_type_features.begin();
	std::list<FeatureInfo>::const_iterator cend = object_type_features.end();
	int index = 0;
	while (ci != cend) {
		// Emit tab after previous column
		OT_objects_data += '\t';

		// Access object's feature at the given index 
		const EMdFValue *pValue = pObject->getFeature(index);

		// Get feature's type
		id_d_t feature_type_id = ci->getType();
		IntegerList *pIntegerList = 0;
		switch (feature_type_id & FEATURE_TYPE_TYPE_MASK) {
		case FEATURE_TYPE_ASCII:
			try {
				if (featureTypeIdIsFromSet(feature_type_id)) {
					FeatureInfo myfi(ci->getName(), ci->getType(), pValue->getString(), ci->getIsComputed());
					// The penultimate "true" on FeatureInfo2SQLvalue means that 
					// we must create any IDD-String association if it is not 
					// there in the OT_mdf_FEATURE_NAME_set table.
					OT_objects_data += escapeStringForCOPY(FeatureInfo2SQLvalue(object_type_name, object_type_id, true, myfi));
				} else {
					OT_objects_data += escapeStringForCOPY(pValue->getString());
				}
			} catch (EmdrosException& e) {
				DEBUG_X_IS_WRONG("PgEMdFDB::createObjectsOT_objects_data", "getting string-value from set for ASCII feature failed.\nException thrown.  Exception message:\n" + e.what());
				throw e;
			}
			break;
		case FEATURE_TYPE_STRING:
			try {
				if (featureTypeIdIsFromSet(feature_type_id)) {
					FeatureInfo myfi(ci->getName(), ci->getType(), pValue->getString(), ci->getIsComputed());
					// The penultimate "true" on FeatureInfo2SQLvalue means that 
					// we must create any IDD-String association if it is not 
					// there in the OT_mdf_FEATURE_NAME_set table.
					OT_objects_data += FeatureInfo2SQLvalue(object_type_name, object_type_id, true, myfi);
				} else {
					OT_objects_data += escapeStringForCOPY(pValue->getString());
				}
			} catch (EmdrosException& e) {
				DEBUG_X_IS_WRONG("PgEMdFDB::createObjectsOT_objects_data", "getting string-value from set for STRING feature failed.\nException thrown.  Exception message:\n" + e.what());
				throw e;
			}
			break;
		case FEATURE_TYPE_SET_OF_MONADS:
			{
				SetOfMonads som = pValue->getSOM();
				OT_objects_data += escapeStringForCOPY(som.toCompactString());
				// Emit tab after previous column
				OT_objects_data += '\t';
				if (som.isEmpty()) {
					OT_objects_data += monad_m2string(MAX_MONAD);
				} else {
					OT_objects_data += monad_m2string(som.first());
				}
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
		case FEATURE_TYPE_LIST_OF_INTEGER:
		case FEATURE_TYPE_LIST_OF_ID_D:
		case FEATURE_TYPE_LIST_OF_ENUM:
			ASSERT_THROW(pValue->getKind() == kEVListOfInteger
				     || pValue->getKind() == kEVListOfID_D, "value's kind is neither kEVListOfInteger nor kEVListOfID_D");
			pIntegerList = pValue->getIntegerList();
			if (pIntegerList != 0) {
				OT_objects_data += escapeStringForCOPY(pIntegerList->getDelimitedString(DEFAULT_LIST_DELIMITER));
			}
			break;
		}

		// Advance index and iterator
		++index;
		++ci;
	}
}


/** Batch-create objects, send batch to the backend.
 *@internal
 *
 * This is part of the machinery for the MQL "CREATE OBJECTS WITH
 * OBJECT TYPE.".  It is used by EMdFDB::createObjects() to send the
 * current batch of objects to the backend.
 *
 * @see EMdFDB::createObjects(),
 * PgEMdFDB::::createObjectsOT_objects_data().
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
bool PgEMdFDB::createObjectsOT_objects_DB(const std::string& object_type_name,
					  const std::string& OT_objects_data,
					  eObjectRangeType objectRangeType,
					  const std::list<FeatureInfo>& object_type_features)
{
	std::string query;
	query += "COPY ";
	query += normalizeTableName(object_type_name + "_objects", true);
	query += "(object_id_d, first_monad";
	if (objectRangeType != kORTSingleMonad) {
		query += ", last_monad";
	}
	if (objectRangeType == kORTMultipleRange) {
		query += ", monads";
	}
	std::list<FeatureInfo>::const_iterator ci = object_type_features.begin();
	std::list<FeatureInfo>::const_iterator cend = object_type_features.end();
	while (ci != cend) {
		query += ", ";
		query += encodeFeatureName(ci->getName());
		if (featureTypeIdIsSOM(ci->getType())) {
			query += ", first_monad_" + encodeFeatureName(ci->getName());
		}
		++ci;
	}
	query += ") FROM stdin;\n";

	if (!pConn->execCommand(query)) {
		DEBUG_COMMAND_QUERY_FAILED("PgEMdFDB::createObjectsOT_objects_DB", query);
		return false;
	}

	query = OT_objects_data;
	query += "\n\\.\n";

	if (!((PgEMdFConnection*)pConn)->execCOPY(query)) {
		std::string error;
		pConn->errorMessage(error);
		appendLocalError(error);
		DEBUG_X_FAILED("PgEMdFDB::createObjectsOT_objects_DB", query);
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
 * the batch.  For COPY, this is a newline.
 *
 * @see EMdFDB::createObjects()
 *
 * @param OT_data The std::string containing the batch which we've
 * built so far.  Is also used to return the data.
 */
void PgEMdFDB::createObjectsInBetweenData(/* out */ std::string& OT_data)
{
	OT_data += '\n';
}


eCharsets PgEMdFDB::getDefaultEncoding(void) const
{
	return kCSUTF8;
}


bool PgEMdFDB::canDeclareEncoding(eCharsets charset) const
{
	return charset == kCSISO_8859_1 || charset == kCSUTF8;
}


std::string PgEMdFDB::getEncodingClause(eCharsets charset) const
{
	/* There is nothing to do. The reason is that we cannot change
           it at compile time, and even though the UTF8 is chosen
           instead of iso-8859-1, we don't use the DB in such a way
           that the DB will care.  NOTE: This may change if we decide
           to store anything which needs a LIKE comparison (such as a
           list of strings...).
	*/

	std::string result = "ENCODING = ";
	if (charset == kCSUTF8) {
		result += "'UTF8'";
	} else if (charset == kCSISO_8859_1) {
		result += "'LATIN1'";
	} else {
		ASSERT_THROW(false, "Unknown charset in PgEMdFDB::getEncodingClause");
	}

	return result;
}



#endif // USE_POSTGRESQL
