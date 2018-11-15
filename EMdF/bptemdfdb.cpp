/*
 * bptemdfdb.cpp
 *
 * EMdF database class for Bit Packed Tables
 *
 * This is not the real thing. The methods here are just stubs, mostly
 * reporting failure.
 *
 * The real thing is closed-source. Please contact Ulrik
 * Sandborg-Petersen at ulrikp-at-scripturesys*dot*com for more
 * information on the BPT engine.
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


/**@file bptemdfdb.cpp
 *@brief Implementation of BPTEMdFDB (EMdF layer)
 */

#include <emdros-config.h>

#include <emdf.h>
#include <cstdio>
#include <map>
#include <cstring>
#include <emdf_enums.h>
#include <new>
#include <exception_emdros.h>
#include <emdf_exception.h>
#include <emdf_value.h>
#include <json_classes.h>
#include <monads.h>
#include <enum_const_cache.h>
#include <infos.h>
#include <conn.h>
#include <emdf_ffeatures.h>
#include <table.h>
#include <conn.h>
#include <inst.h>
#include <monads.h>
#include <bptemdfdb.h>
#include <emdf_value.h>
#include <emdf_ffeatures.h>
#include <enum_const_cache.h>
#include <string_func.h>
#include <string_list.h>
#include <string_set_cache.h>
#include <utils.h>
#include <debug.h>
#include <iostream>
#include <sstream>
#include <set>
#include <logging.h>
#if defined(HAVE_UNISTD_H) && HAVE_UNISTD_H
#include <unistd.h>
#endif





/////////////////////////////////////////////////////////
//
// BPTEMdFDB class implementation
//
/////////////////////////////////////////////////////////



/** Constructor.
 *
 */
BPTEMdFDB::BPTEMdFDB() 
	: EMdFDB(kBPT)
{
}

BPTEMdFDB::BPTEMdFDB(std::string database_name, std::string key) 
	: EMdFDB(kBPT)
{
	UNUSED(database_name);
	UNUSED(key);
}

/** Destructor.
 * 
 */
BPTEMdFDB::~BPTEMdFDB()        
{
}



/** database name is OK.
 *@internal
 *
 * @param db_name the database name to be checked
 * @param bIsOK set to true if db_name is OK, false otherwise.
 */
void BPTEMdFDB::databaseNameIsOK(const std::string& db_name, bool& bIsOK)
{
	UNUSED(db_name);
	
	bIsOK = false;
}

//
// Database manipulation
/** Create database.
 *@internal
 *
 * Must be overridden in a subclass.
 *
 * Note that the encryption key is only used for SQLite, and only if
 * you have an SQLite that supports encryption.
 *
 * @param db_name the database name to be created
 * @param key the encryption key to be used.
 * @return True on no database error, false if a database error occurred.
 */
bool BPTEMdFDB::createDatabase(const std::string& db_name, const std::string& key)
{
	UNUSED(db_name);
	UNUSED(key);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
}

/** Create database.
 *@internal
 *
 * Must be overridden in a subclass.
 *
 * Note that the encryption key is only used for SQLite, and only if
 * you have an SQLite that supports encryption.
 *
 * @param db_name the database name to be created
 * @param key the encryption key to be used.
 * @return True on no database error, false if a database error occurred.
 */
bool BPTEMdFDB::createDatabase(const std::string& db_name, eCharsets charset, const std::string& key)
{
	UNUSED(db_name);
	UNUSED(charset);
	UNUSED(key);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
 * Note that the encryption key is only used for SQLite, and only if
 * you have an SQLite that supports encryption.
 *
 * @param db_name the database name to be initialized.
 * @param key the encryption key to be used.
 * @return True on no database error, false if a database error occurred.
 */
bool BPTEMdFDB::initializeDatabase(const std::string& db_name, const std::string& key)
{
	UNUSED(db_name);
	UNUSED(key);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
 * Note that the encryption key is only used for SQLite, and only if
 * you have an SQLite that supports encryption.
 *
 * @param db_name the database name to be used.
 * @param key is ignored in BPTEMdFDB.  May be used by derived subclass.
 * @return True on no database error, false if a database error occurred.
 */
bool BPTEMdFDB::useDatabase(const std::string& db_name, const std::string& key)
{
	UNUSED(db_name);
	UNUSED(key);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
 * Note that the encryption key is only used for SQLite, and only if
 * you have an SQLite that supports encryption.
 *
 * @param db_name the database name to be used.
 *
 * @param key is ignored in BPTEMdFDB.  May be used by derived subclass.
 *
 * @param bIsInitializingOrCreating Must be false if we are not
 * creating or initializing the database, true otherwise.
 *
 * @return True on no database error, false if a database error occurred.
 */
bool BPTEMdFDB::useDatabase(const std::string& db_name, const std::string& key, bool bIsInitializingOrCreating)
{
	UNUSED(db_name);
	UNUSED(key);
	UNUSED(bIsInitializingOrCreating);
	
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
}

/** Drop database.
 *@internal
 *
 * Drops (deletes) a database.
 *
 * @param db_name the database name to be used.
 * @return True on no database error, false if a database error occurred.
 */
bool BPTEMdFDB::dropDatabase(const std::string& db_name)
{
	UNUSED(db_name);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::dbIsInitialized(bool& bIsInitialized)
{
	bIsInitialized = false;
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::useDatabase", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
}

/** Get current schema version
 *@internal
 *
 * The schema version numbers are defined in emdf.h.
 *
 * @param schema_version Used to return the schema version.
 * @return True on no database error, false if a database error occurred.
 */
bool BPTEMdFDB::getSchemaVersion(/* out */ long& schema_version)
{
	UNUSED(schema_version);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::useDatabase", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
}


/** Set schema version
 *@internal
 *
 * @param new_schema_version The schema version to set to.
 * @return True on no database error, false if a database error occurred.
 */
bool BPTEMdFDB::setSchemaVersion(long new_schema_version)
{
	UNUSED(new_schema_version);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::useDatabase", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
}


/** Create schema version table
 *@internal
 *
 * @param initial_schema_version The initial schema version to use.
 * @return True on no database error, false if a database error occurred.
 */
bool BPTEMdFDB::createSchemaVersionTable(long initial_schema_version)
{
	UNUSED(initial_schema_version);

	DEBUG_X_IS_WRONG("BPTEMdFDB::useDatabase", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::createObjectTypeOT_objects(const std::string& object_type_name,
					eObjectRangeType objectRangeType,
					eMonadUniquenessType monadUniquenessType,
					const std::list<FeatureInfo>& FeatureInfos)
{
	UNUSED(object_type_name);
	UNUSED(objectRangeType);
	UNUSED(monadUniquenessType);
	UNUSED(FeatureInfos);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::useDatabase", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::createObjectType(const std::string& object_type_name,
			      const std::list<FeatureInfo>& FeatureInfos,
			      eObjectRangeType objectRangeType,
			      eMonadUniquenessType monadUniquenessType,
                              /* out */ id_d_t& object_type_id)
{
	UNUSED(object_type_name);
	UNUSED(FeatureInfos);
	UNUSED(objectRangeType);
	UNUSED(monadUniquenessType);
	UNUSED(object_type_id);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::useDatabase", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::dropObjectType(const std::string& object_type_name, id_d_t object_type_id)

{
	UNUSED(object_type_name);
	UNUSED(object_type_id);

	DEBUG_X_IS_WRONG("BPTEMdFDB::useDatabase", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::getLargestObjectLengthFromOT_objects(const std::string& object_type_name,
						     eObjectRangeType objectRangeType,
						     /* out */ monad_m& largest_object_length)
{
	UNUSED(object_type_name);
	UNUSED(objectRangeType);
	UNUSED(largest_object_length);

	DEBUG_X_IS_WRONG("BPTEMdFDB::useDatabase", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::getLargestObjectLength(const std::string& object_type_name,
				       eObjectRangeType objectRangeType,
				       /* out */ monad_m& largest_object_length)
{
	UNUSED(object_type_name);
	UNUSED(objectRangeType);
	UNUSED(largest_object_length);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::useDatabase", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::setLargestObjectLengthIfNecessary(const std::string& object_type_name,
					       monad_m new_largest_object_length,
					       eObjectRangeType objectRangeType,
					       bool bSetUnconditionally)
{
	UNUSED(object_type_name);
	UNUSED(new_largest_object_length);
	UNUSED(objectRangeType);
	UNUSED(bSetUnconditionally);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::useDatabase", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::objectTypeExists(id_d_t object_type_id,
				 /* out */ bool& result, 
				 /* out */ std::string& object_type_name,
				 /* out */ eObjectRangeType& objectRangeType,
				 /* out */ eMonadUniquenessType& monadUniquenessType)
{
	UNUSED(object_type_id);
	UNUSED(result);
	UNUSED(object_type_name);
	UNUSED(objectRangeType);
	UNUSED(monadUniquenessType);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::useDatabase", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::objectTypeExists(const std::string& object_type_name, 
				 /* out */ bool& result, 
				 /* out */ id_d_t& object_type_id,
				 /* out */ eObjectRangeType& objectRangeType,
				 /* out */ eMonadUniquenessType& monadUniquenessType) 
{
	UNUSED(object_type_name);
	UNUSED(result);
	UNUSED(object_type_id);
	UNUSED(objectRangeType);
	UNUSED(monadUniquenessType);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::useDatabase", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::createEnum(const std::string& enum_name, 
			   /* out */ id_d_t& enum_id)
{
	UNUSED(enum_name);
	UNUSED(enum_id);

	DEBUG_X_IS_WRONG("BPTEMdFDB::useDatabase", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::dropEnum(id_d_t enum_id)
{
	UNUSED(enum_id);

	DEBUG_X_IS_WRONG("BPTEMdFDB::useDatabase", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::enumExists(id_d_t enum_id,
			   /* out */ bool& result,
			   /* out */ std::string& enum_name)
{
	UNUSED(enum_id);
	UNUSED(result);
	UNUSED(enum_name);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::useDatabase", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::enumExists(const std::string& enum_name, 
			   /* out */ bool& result,
			   /* out */ id_d_t& enum_id)
{
	UNUSED(enum_name);
	UNUSED(result);
	UNUSED(enum_id);
	
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::useDatabase", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::getDefault(id_d_t enum_id, 
			   /* out */ bool& result,
			   /* out */ std::string& default_constant_name)
{
	UNUSED(enum_id);
	UNUSED(result);
	UNUSED(default_constant_name);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::createEnumConstant(id_d_t enum_id,
				const std::string& enum_name,
				const EnumConstInfo& ec_info)
{
	UNUSED(enum_id);
	UNUSED(enum_name);
	UNUSED(ec_info);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::enumConstExists(const std::string& enum_const_name, 
			     id_d_t enum_id, 
                             /* out */ bool& result,
                             /* out */ long& value,
                             /* out */ bool& is_default)
{
	UNUSED(enum_const_name);
	UNUSED(enum_id);
	UNUSED(result);
	UNUSED(value);
	UNUSED(is_default);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::getEnumConstNameFromValue(long value,
					  const std::string& enum_name,
					  /* out */ std::string& enum_const_name)
  
{
	UNUSED(value);
	UNUSED(enum_name);
	UNUSED(enum_const_name);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::enumConstExists(long value,
				id_d_t enum_id,
				bool& bExists,
				/* out */ std::string& enum_const_name,
				/* out */ bool& is_default)

{
	UNUSED(value);
	UNUSED(enum_id);
	UNUSED(bExists);
	UNUSED(enum_const_name);
	UNUSED(is_default);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::dropEnumConst(id_d_t enum_id, 
			      const std::string& enum_const_name, 
			      long value)
{
	UNUSED(enum_id);
	UNUSED(enum_const_name);
	UNUSED(value);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::updateEnumConst(const std::string& enum_const_name, 
			     id_d_t enum_id,
			     long old_value,
			     long new_value)
{
	UNUSED(enum_const_name);
	UNUSED(enum_id);
	UNUSED(old_value);
	UNUSED(new_value);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::setDefaultEnumConst(id_d_t enum_id,
				    const std::string& enum_const_name)
{
	UNUSED(enum_id);
	UNUSED(enum_const_name);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
 * BPTEMdFDB::getNextTypeID()).
 *
 * @param default_value The default value of the new feature.
 *
 * @param computed Whether the feature is computed (true) or not (false).
 *
 * @return True on no database error, false if a database error occurred.
 */
bool BPTEMdFDB::createFeature(const std::string& object_type_name,
			      id_d_t object_type_id,
			      const FeatureInfo& feature_info)
{
	UNUSED(object_type_name);
	UNUSED(object_type_id);
	UNUSED(feature_info);

	DEBUG_X_IS_WRONG("BPTEMdFDB::createFeature", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::addFeatureToOT_objects(const std::string& object_type_name,
				    id_d_t object_type_id,
				    eObjectRangeType objectRangeType,
				    eMonadUniquenessType monadUniquenessType,
				    const FeatureInfo& fi)
{
	UNUSED(object_type_name);
	UNUSED(object_type_id);
	UNUSED(objectRangeType);
	UNUSED(monadUniquenessType);
	UNUSED(fi);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::addFeature(const std::string& object_type_name,
			id_d_t object_type_id,
			const FeatureInfo& fi)
{
	UNUSED(object_type_name);
	UNUSED(object_type_id);
	UNUSED(fi);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::dropFeatureFromOT_objects(const std::string& object_type_name,
				       const std::string& feature_name,
				       eObjectRangeType objectRangeType,
				       eMonadUniquenessType monadUniquenessType,
				       id_d_t object_type_id)
{
	UNUSED(object_type_name);
	UNUSED(feature_name);
	UNUSED(objectRangeType);
	UNUSED(monadUniquenessType);
	UNUSED(object_type_id);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::dropFeature(const std::string& object_type_name,
                         const std::string& feature_name,
			 id_d_t feature_type_id,
                         id_d_t object_type_id)
{
	UNUSED(object_type_name);
	UNUSED(feature_name);
	UNUSED(feature_type_id);
	UNUSED(object_type_id);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::featureExists(const std::string& feature_name,
			      id_d_t object_type_id,
			      /* out */ bool& result,
			      /* out */ id_d_t& feature_type_id,
			      /* out */ std::string& default_value,
			      /* out */ bool& is_computed)
{
	UNUSED(feature_name);
	UNUSED(object_type_id);
	UNUSED(result);
	UNUSED(feature_type_id);
	UNUSED(default_value);
	UNUSED(is_computed);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
}



// Assumes everything exists 
// TODO: Make this work with computed features
/** Get feature values from a set of objects.
 *@internal
 *
 * This is the incarnation used in the MQL statement "GET FEATURES".
 *
 * Uses BPTEMdFDB::getFeaturesByQuery().
 * 
 * @see BPTEMdFDB::getFeaturesByQuery(), GetFeaturesStatement::exec().
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
bool BPTEMdFDB::getFeatures(const std::string& object_type_name,
			 id_d_t object_type_id,
			 const std::list<FeatureInfo>& FeatureInfos,
			 const SetOfMonads& object_id_ds_set,
			 /* out */ std::list<std::list<std::string> >& results)
{
	UNUSED(object_type_name);
	UNUSED(object_type_id);
	UNUSED(FeatureInfos);
	UNUSED(object_id_ds_set);
	UNUSED(results);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::loadStringSetIntoCacheBPT(const std::string& object_type_name, 
					  id_d_t object_type_id,
					  const std::string& feature_name,
					  Table *pTable /* may be 0 */)
{
	UNUSED(object_type_name);
	UNUSED(object_type_id);
	UNUSED(feature_name);
	UNUSED(pTable);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
 * encoded with BPTEMdFDB::escapeStringForSQL().
 *
 * @param original_string The string to be created. Must be the original
 * string, before encoding with BPTEMdFDB::escapeStringForSQL().
 *
 * @param result The set id_d value created
 *
 * @return true on found, false on not found or DB error.
 */
bool BPTEMdFDB::addStringToStringSetTable(const std::string& normalized_object_type_name,
					  const std::string& encoded_feature_name,
					  const std::string& encoded_string_to_create, // Must be encoded with escapeStringForSQL
					  /* out */ id_d_t& result)
{
	UNUSED(normalized_object_type_name);
	UNUSED(encoded_feature_name);
	UNUSED(encoded_string_to_create);
	UNUSED(result);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
 * encoded with BPTEMdFDB::escapeStringForSQL().
 *
 * @param original_string The string to be found. Must be the original
 * string, before encoding with BPTEMdFDB::escapeStringForSQL().
 *
 * @param bCreateIfNotThere Must be true if the caller wants the id_d
 * to be created if it is not present in the table.
 *
 * @param result The set id_d value to find (encoded as a std::string).
 *
 * @return true on found, false on not found or DB error.
 */
bool BPTEMdFDB::getID_DFromStringSet(const std::string& normalized_object_type_name,
				     id_d_t object_type_id,
				     const std::string& encoded_feature_name,
				     const std::string& encoded_string_to_find, // Must be encoded with escapeStringForSQL
				     const std::string& original_string,
				     bool bCreateIfNotThere,
				     /* out */ std::string& result) // Even though it is an id_d, we get is as a string.
{
	UNUSED(normalized_object_type_name);
	UNUSED(object_type_id);
	UNUSED(encoded_feature_name);
	UNUSED(encoded_string_to_find);
	UNUSED(original_string);
	UNUSED(bCreateIfNotThere);
	UNUSED(result);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::createStringSetTable(const std::string& object_type_name, 
				     const std::string& feature_name)
{
	UNUSED(object_type_name);
	UNUSED(feature_name);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::createIndicesOnStringSetTable(const std::string& object_type_name,
					      const std::string& feature_name)
{
	UNUSED(object_type_name);
	UNUSED(feature_name);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::dropIndicesOnStringSetTable(const std::string& object_type_name,
					    const std::string& feature_name)
{
	UNUSED(object_type_name);
	UNUSED(feature_name);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::dropStringSetTable(const std::string& object_type_name, 
				   const std::string& feature_name)
{
	UNUSED(object_type_name);
	UNUSED(feature_name);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
}


//
// Segment manipulation
// Assumes that segment does not exist
/** Create a segment.
 *@internal
 *
 * This just calls BPTEMdFDB::createMonadSet with the segment name and a
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
bool BPTEMdFDB::createSegment(const std::string& segment_name, 
			      monad_m first, monad_m last)
{
	UNUSED(segment_name);
	UNUSED(first);
	UNUSED(last);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
}

bool BPTEMdFDB::insertMonadRange(monad_m first, monad_m last)
{
	UNUSED(first);
	UNUSED(last);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
}

bool BPTEMdFDB::insertMonadRangeOT(const std::string& object_type_name, monad_m first, monad_m last)
{
	UNUSED(object_type_name);
	UNUSED(first);
	UNUSED(last);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::selectMonadSets(/* out */ std::list<std::string>& monad_set_names)
{
	UNUSED(monad_set_names);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::monadSetExists(const std::string& monad_set_name, 
			       /* out */ bool& bExists,
			       /* out */ SetOfMonads& som)

{
	UNUSED(monad_set_name);
	UNUSED(bExists);
	UNUSED(som);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::createMonadSet(const std::string& monad_set_name, const SetOfMonads& som)
{
	UNUSED(monad_set_name);
	UNUSED(som);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
}


/** Drop a monad set
 *@internal
 *
 * @param monad_set_name The name of the arbitrary monad set to drop.
 *
 * @return true on no DB error, false otherwise.
 */
bool BPTEMdFDB::dropMonadSet(const std::string& monad_set_name)
{
	UNUSED(monad_set_name);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::connectionOk(void)
{
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::getObjectsHavingMonadsInMonadSet(const std::string object_type_name,
						 const SetOfMonads& monad_ms,
						 std::set<id_d_t>& objects_already_seen,					      
						 eObjectRangeType objectRangeType,
						 /* out */ Table& result)
{
	UNUSED(object_type_name);
	UNUSED(monad_ms);
	UNUSED(objects_already_seen);
	UNUSED(objectRangeType);
	UNUSED(result);

	
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::getObjectsHavingMonadsInMonadSet(const std::string object_type_name,
						 const SetOfMonads& monad_ms,
						 eObjectRangeType objectRangeType,
						 /* out */ Table& result)
{
	UNUSED(object_type_name);
	UNUSED(monad_ms);
	UNUSED(objectRangeType);
	UNUSED(result);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
 * @param pInst Result as an Inst
 *
 * @return true on no DB error, false otherwise.
 */
bool BPTEMdFDB::getObjectsHavingMonadsIn(const std::string object_type_name,
					 id_d_t object_type_id,
					 eObjectRangeType objectRangeType,	
					 const SetOfMonads& monad_ms,
					 const SetOfMonads& all_m_1,
					 const std::string& pre_query_string,
					 const std::list<FeatureInfo>& features_to_get,
					 const std::string& monad_set_name,
					 /* Out */ Inst *pInst)
{
	UNUSED(object_type_name);
	UNUSED(object_type_id);
	UNUSED(objectRangeType);
	UNUSED(monad_ms);
	UNUSED(all_m_1);
	UNUSED(pre_query_string);
	UNUSED(features_to_get);
	UNUSED(monad_set_name);
	UNUSED(pInst);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
}

/** get Inst(T,Su).
 *
 *@internal
 *
 * Get an Inst(object type, Substrate).
 *
 * This is the entry point into BPTEMdFDB; the other getInst() is used by
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
 * @param Result the Inst to get back.
 *
 * @return true on no DB error, false otherwise.
 */
bool BPTEMdFDB::getInst(const std::string& object_type_name,
			id_d_t object_type_id,
			const SetOfMonads& Su,
			const SetOfMonads& all_m_1,	
			const std::string& pre_query_string,
			EMdFFFeatures *pre_query_constraints,
			const std::list<FeatureInfo>& features_to_get,
			const std::string& monad_set_name,
			eMonadSetRelationOperation ms_operation,
			/* out */ Inst& Result)
{
	UNUSED(object_type_name);
	UNUSED(object_type_id);
	UNUSED(Su);
	UNUSED(all_m_1);
	UNUSED(pre_query_string);
	UNUSED(pre_query_constraints);
	UNUSED(features_to_get);
	UNUSED(monad_set_name);
	UNUSED(ms_operation);
	UNUSED(Result);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
}


/** get Inst(T,Su)
 *@internal
 *
 * Get an Inst(object type, Substrate), within a range
 * first_monad..last_monad.
 *
 * This is not the public entry point into BPTEMdFDB; the other getInst()
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
 * @param objectRangeType the eObjectRangeType of the object type
 *
 * @param largest_object_length the length (in monads) of the longest
 * object in the object type which we are querying.
 *
 * @param Result the Inst to get back.
 *
 * @return true on no DB error, false otherwise.
 */
bool BPTEMdFDB::getInstLocalBPT(const std::string& object_type_name,
				id_d_t object_type_id,
				const SetOfMonads& Su,
				const SetOfMonads& all_m_1,
				EMdFFFeatures *pre_query_constraints,
				const std::list<FeatureInfo>& features_to_get,
				eObjectRangeType objectRangeType,
				/* out */ Inst& Result)
{
	UNUSED(object_type_name);
	UNUSED(object_type_id);
	UNUSED(Su);
	UNUSED(all_m_1);
	UNUSED(pre_query_constraints);
	UNUSED(features_to_get);
	UNUSED(objectRangeType);
	UNUSED(Result);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::getObjectsStartingAtSm(const std::string& object_type_name,
				       monad_m Sm,
				       /* out */ IDDList& Result)
{
	UNUSED(object_type_name);
	UNUSED(Sm);
	UNUSED(Result);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::getMonadsFromID_D(id_d_t object_id_d, 
				  id_d_t object_type_id,
				  /* out */ SetOfMonads& monads)
{
	UNUSED(object_id_d);
	UNUSED(object_type_id);
	UNUSED(monads);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::getMonadsFromID_Ds(const SetOfMonads& id_ds_set,
				const std::string &object_type_name,
				eObjectRangeType objectRangeType,
				Table *pOutTable)
{
	UNUSED(id_ds_set);
	UNUSED(object_type_name);
	UNUSED(objectRangeType);
	UNUSED(pOutTable);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
}


//
// Get set of monads from object_id_d belonging to object type
// with name object_type_name
//
/** Get monads from an object
 *@internal
 *
 * This is similar to BPTEMdFDB::getMonadsFromID_D(), but that method has
 * fewer parameters, and actulaly calls this method.
 *
 * The rationale for having both is that BPTEMdFDB::getMonadsFromID_D()
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
bool BPTEMdFDB::getSOMForObject(const std::string& object_type_name,
			     id_d_t object_id_d,
			     eObjectRangeType objectRangeType,
			     /* out */ SetOfMonads& Result)
{
	UNUSED(object_type_name);
	UNUSED(object_id_d);
	UNUSED(objectRangeType);
	UNUSED(Result);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
}

/** Get a list of the object types in the database
 *@internal
 *
 * @param Result the list of the object type names.  The object type
 * names are as the user would see them.
 *
 * @return true on no DB error, false otherwise.
 */
bool BPTEMdFDB::getObjectTypes(/* out */ std::list<std::string>& Result)
{
	UNUSED(Result);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::getFeaturesForObjectType(id_d_t object_type_id,
					 /* out */ std::list<FeatureInfo>& Result)
{
	UNUSED(object_type_id);
	UNUSED(Result);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::getEnumerations(/* out */ std::list<std::string>& Result)
{
	UNUSED(Result);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
}

/** Load enumeration constants into cache
 *
 * Loads enumeration constants for a given enumeration into the cache.
 *
 * @param enum_id the id_d of the enumeration to query.
 *
 * @return true on no DB error, false otherwise.
 */
bool BPTEMdFDB::loadEnumConstantsIntoCache(id_d_t enum_id)
{
	UNUSED(enum_id);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::getEnumConstants(id_d_t enum_id,
				 /* out */ std::list<EnumConstInfo>& Result)
{
	UNUSED(enum_id);
	UNUSED(Result);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::getObjectTypesUsingEnumeration(id_d_t enum_id,
					       /* out */ std::list<std::string>& Result)
{
	UNUSED(enum_id);
	UNUSED(Result);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
}

/** Get objects that are wholly contained within a set of monads.
 *@internal
 *
 * This is different from BPTEMdFDB::getObjectsHavingMonadsInMonadSet()
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
bool BPTEMdFDB::getObjectsWithinMonads(const SetOfMonads& monads, 
				       const std::string& object_type_name,
				       eObjectRangeType objectRangeType,
				       std::list<id_d_t>& result)
{
	UNUSED(monads);
	UNUSED(object_type_name);
	UNUSED(objectRangeType);
	UNUSED(result);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::objectID_DExistsInType(id_d_t object_id_d, 
				       const std::string& object_type_name,
				       /* out */ bool& Result)
{
	UNUSED(object_id_d);
	UNUSED(object_type_name);
	UNUSED(Result);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::objectID_DExists(id_d_t object_id_d, 
				 /* out */ bool& Result,
				 /* out */ id_d_t& object_type_id)
{
	UNUSED(object_id_d);
	UNUSED(Result);
	UNUSED(object_type_id);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::createObject(id_d_t object_id_d,
			  const std::string& object_type_name,
			  id_d_t object_type_id,
			  const SetOfMonads& monads,
			  eObjectRangeType objectRangeType,
			  const std::list<FeatureInfo>& features)
{
	UNUSED(object_id_d);
	UNUSED(object_type_name);
	UNUSED(object_type_id);
	UNUSED(monads);
	UNUSED(objectRangeType);
	UNUSED(features);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::updateObject(id_d_t object_id_d,
			     const std::string& object_type_name,
			     id_d_t object_type_id,
			     const std::list<FeatureInfo>& features)
{
	UNUSED(object_id_d);
	UNUSED(object_type_name);
	UNUSED(object_type_id);
	UNUSED(features);

	
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::dropObject(id_d_t object_id_d, 
			   id_d_t object_type_id, 
			   const std::string& object_type_name,
			   eObjectRangeType objectRangeType)
{
	UNUSED(object_id_d);
	UNUSED(object_type_id);
	UNUSED(object_type_name);
	UNUSED(objectRangeType);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
}

/** Drop objects that are wholly contained in a set of monads
 *@internal
 *
 * Uses BPTEMdFDB::getObjectsWithinMonads() to get the object id_ds to
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
bool BPTEMdFDB::dropObjectsInMonads(const SetOfMonads& monads, 
				    const std::string& object_type_name, 
				    id_d_t object_type_id, 
				    eObjectRangeType objectRangeType,
				    /* out */ std::list<id_d_t>& result)
{
	UNUSED(monads);
	UNUSED(object_type_name);
	UNUSED(object_type_id);
	UNUSED(objectRangeType);
	UNUSED(result);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::createObjects(const std::string& object_type_name,
			      id_d_t object_type_id,
			      const std::list<FeatureInfo>& object_type_features,
			      std::list<InstObject*>& object_list,
			      eObjectRangeType objectRangeType,
			      /* out */ long& object_count)
{
	UNUSED(object_type_name);
	UNUSED(object_type_id);
	UNUSED(object_type_features);
	UNUSED(object_list);
	UNUSED(objectRangeType);
	UNUSED(object_count);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
}


/** Batch-create objects, create a line of OT_objects data in a string.
 *@internal
 *
 * This is part of the machinery for the MQL "CREATE OBJECTS WITH
 * OBJECT TYPE.".  It is used by BPTEMdFDB::createObjects() to create one
 * object's worth of output data in a std::string, to be sent to the
 * backend later by BPTEMdFDB::createObjectsOT_objects_DB() (or its
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
void BPTEMdFDB::createObjectsOT_objects_data(const std::string object_type_name, 
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
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
}

/** Batch-create objects, send batch to the backend.
 *@internal
 *
 * This is part of the machinery for the MQL "CREATE OBJECTS WITH
 * OBJECT TYPE.".  It is used by BPTEMdFDB::createObjects() to send the
 * current batch of objects to the backend.
 *
 * @see BPTEMdFDB::createObjects()
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
bool BPTEMdFDB::createObjectsOT_objects_DB(const std::string& object_type_name,
					   const std::string& OT_objects_data,
					   eObjectRangeType objectRangeType,
					   const std::list<FeatureInfo>& object_type_features)
{
	UNUSED(object_type_name);
	UNUSED(OT_objects_data);
	UNUSED(objectRangeType);
	UNUSED(object_type_features);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
}


/** Batch-create objects, create what is necessary in between each
 *  object record
 *@internal
 *
 * This is part of the machinery for the MQL "CREATE OBJECTS WITH
 * OBJECT TYPE.".  It is used by BPTEMdFDB::createObjects() and others to
 * create the in-between-records marker for the std::string containing
 * the batch.
 *
 * @see BPTEMdFDB::createObjects()
 *
 * @param OT_data The std::string containing the batch which we've
 * built so far.  Is also used to return the data.
 */
void BPTEMdFDB::createObjectsInBetweenData(/* out */ std::string& OT_data)
{
	UNUSED(OT_data);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
}

					    

/** Get current database version
 *
 * This method retrieves information about the schema version of an
 * Emdros database.  The database is first USEd, so the current
 * database connection will be lost.
 *
 * Note that the encryption key is only used for SQLite, and only if
 * you have an SQLite that supports encryption.
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
 * BPTEMdFDB::ugpradeDatabase() method is able to upgrade the database
 * automatically.  If this is not the case, you can always dump the
 * database with the mqldump utility from the current version of the
 * database, then import the resulting MQL script with the mql(1)
 * program from the current version of Emdros.
 *
 * @return True on no DB error, false otherwise.
 */
bool BPTEMdFDB::getCurrentDatabaseVersion(const std::string& database_name, 
					  const std::string& key,
					  std::string& /* out */ strCurrentVersion,
					  int& /* out */ nCurrentVersion,
					  int& /* out */ nVersionWhichWeCanUpgradeTo,
					  std::string& /* out */ strVersionWhichWeCanUpgradeTo,
					  bool& /* out */ bNeedsUpgrading,
					  bool& /* out */ bCanUpgrade)
{
	UNUSED(database_name);
	UNUSED(key);
	UNUSED(strCurrentVersion);
	UNUSED(nCurrentVersion);
	UNUSED(nVersionWhichWeCanUpgradeTo);
	UNUSED(strVersionWhichWeCanUpgradeTo);
	UNUSED(bNeedsUpgrading);
	UNUSED(bCanUpgrade);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
 * the database.  Use BPTEMdFDB::getCurrentDatabaseVersion() to get the
 * number.
 *
 * @return True on no DB error, false otherwise.
 */
bool BPTEMdFDB::upgradeDatabase(const std::string& database_name, 
				int nCurrentVersion)
{
	UNUSED(database_name);
	UNUSED(nCurrentVersion);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::objectTypeHasObjects(const std::string& object_type_name,
				     bool& /* out */ bHasObjects)
{
	UNUSED(object_type_name);
	UNUSED(bHasObjects);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::tableExists(std::string table_name, bool&  /* out */ bTableExists)
{
	UNUSED(table_name);
	UNUSED(bTableExists);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
}


//
// min_m/max_m
// 

/** Create Min_m/Max_m tables
 *@internal
 *
 * @return True on no DB error, false otherwise.
 */
bool BPTEMdFDB::createMin_max_m_table(void)
{
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
}

// Iterate over all object types, calling Set_min_max_m_from_object_type
// for each object type.
/** Set Min_M/Max_m from all object types.
 *@internal
 *
 * Iterates over all object types, calling
 * BPTEMdFDB::setMin_max_m_fromObjectType() for each object type.
 *
 * @param bBeVerbose If this is true, we print a lot of progress info
 * on std::cerr.  This is mainly for debugging purposes.
 *
 * @return True on no DB error, false otherwise.
 */
bool BPTEMdFDB::setMin_max_m_fromObjectTypes(bool bBeVerbose)
{
	UNUSED(bBeVerbose);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::setMin_max_m_fromObjectType(const std::string& object_type_name, 
					    bool bBeVerbose)
{
	UNUSED(object_type_name);
	UNUSED(bBeVerbose);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
}


/** Gets the max_m monad of the database.
 *
 * The max_m monad is the least monad in the database.  
 * Defaults to 0 if there are no objects in the database.
 *
 * @param max_m Used for returning the max_m monad.
 * @return True on no database error, false if a database error occurred.
 */
bool BPTEMdFDB::getMax_m(monad_m& /* out */ max_m)
{
	UNUSED(max_m);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
}


/** Set max_m if necessary
 *@internal
 *
 * Tests to see if max_m is currently smaller than max_m_candidate.
 * If it is, we set max_m to max_m_candiate.  Otherwise, we leave
 * max_m alone, unless bSetUnconditionally is true, in which case we
 * always set max_m to max_m_candiate.
 *
 * @see BPTEMdFDB::setMin_m().
 *
 * @param max_m_candidate The monad to potentially set max_m to.
 *
 * @param bSetUnconditionally If true, set max_m to max_m_candidate,
 * even if max_m is currently greater than max_m_candidate.
 *
 * @return True on no DB error, false otherwise.
 */
bool BPTEMdFDB::setMax_m(monad_m max_m_candidate, bool bSetUnconditionally)
{
	UNUSED(max_m_candidate);
	UNUSED(bSetUnconditionally);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
}


/** Gets the min_m monad of the database.
 *
 * The min_m monad is the least monad in the database.  
 * Defaults to MAX_MONAD if there are no objects in the database.
 *
 * @param min_m Used for returning the min_m monad.
 * @return True on no database error, false if a database error occurred.
 */
bool BPTEMdFDB::getMin_m(monad_m& /* out */ min_m)
{
	UNUSED(min_m);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
}


/** Set min_m if necessary
 *@internal
 *
 * Tests to see if min_m is currently greater than min_m_candidate.
 * If it is, we set min_m to min_m_candiate.  Otherwise, we leave
 * min_m alone, unless bSetUnconditionally is true, in which case we
 * always set min_m to min_m_candiate.
 *
 * @see BPTEMdFDB::setMax_m().
 *
 * @param min_m_candidate The monad to potentially set min_m to.
 *
 * @param bSetUnconditionally If true, set min_m to min_m_candidate,
 * even if min_m is currently greater than min_m_candidate.
 *
 * @return True on no DB error, false otherwise.
 */
bool BPTEMdFDB::setMin_m(monad_m min_m_candidate, bool bSetUnconditionally) 
{
	UNUSED(min_m_candidate);
	UNUSED(bSetUnconditionally);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::createIndex(const std::string& index_name, 
			 const std::string& table_name, 
			 const std::list<std::pair<std::string, unsigned int> >& columns)
{
	UNUSED(index_name);
	UNUSED(table_name);
	UNUSED(columns);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::createIndex(const std::string& index_name, 
			    const std::string& table_name, 
			    const std::list<std::string>& columns)
{
	UNUSED(index_name);
	UNUSED(table_name);
	UNUSED(columns);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
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
bool BPTEMdFDB::createIndicesOnOTObjects(const std::string& object_type_name) 
{
	UNUSED(object_type_name);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
}

/** Create default indices on all object types.
 *
 * First uses the database by calling BPTEMdFDB::useDatabase() (or its
 * polymorphic instance), then getting a list of object types in the
 * database, then calling BPTEMdFDB::createIndicesOnObjectType() for each
 * of them.
 *
 * @see BPTEMdFDB::useDatabase(), BPTEMdFDB::getObjectTypes(),
 * BPTEMdFDB::createIndicesOnObjectType().
 *
 * Note that the encryption key is only used for SQLite, and only if
 * you have an SQLite that supports encryption.
 *
 * @param database_name The database to use and create indexes on.
 * @param key Just passed to BPTEMdFDB::useDatabase()
 * @return True on no database error, false if a database error occurred.
 */
bool BPTEMdFDB::createIndicesOnDatabase(const std::string& database_name, const std::string& key)
{
	UNUSED(database_name);
	UNUSED(key);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
}

/** Drop default indices on all object types.
 *
 * First uses the database by calling BPTEMdFDB::useDatabase() (or its
 * polymorphic instance), then getting a list of object types in the
 * database, then calling BPTEMdFDB::dropIndicesOnObjectType() for each
 * of them.
 *
 * Note that the encryption key is only used for SQLite, and only if
 * you have an SQLite that supports encryption.
 *
 * @see BPTEMdFDB::useDatabase(), BPTEMdFDB::getObjectTypes(),
 * BPTEMdFDB::dropIndicesOnObjectType().
 *
 * @param database_name The database to use and drop indexes on.
 * @param key Just passed to BPTEMdFDB::useDatabase()
 * @return True on no database error, false if a database error occurred.
 */
bool BPTEMdFDB::dropIndicesOnDatabase(const std::string& database_name, const std::string& key)
{
	UNUSED(database_name);
	UNUSED(key);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
}

/** Create default indices on specific object type in specific database.
 *
 * First uses the database by calling BPTEMdFDB::useDatabase() (or its
 * polymorphic instance), then calling
 * BPTEMdFDB::createIndicesOnObjectType(const std::string&).
 *
 * Note that the encryption key is only used for SQLite, and only if
 * you have an SQLite that supports encryption.
 *
 * @see BPTEMdFDB::useDatabase(), BPTEMdFDB::createIndicesOnObjectType(const
 * std::string&).
 *
 * @param database_name The database to use and drop indexes on.
 * @param key Just passed to BPTEMdFDB::useDatabase()
 * @param object_type_name Name of the object type on which to create default indexes.
 * @return True on no database error, false if a database error occurred.
 */
bool BPTEMdFDB::createIndicesOnObjectType(const std::string& database_name, const std::string& key, const std::string& object_type_name)
{
	UNUSED(database_name);
	UNUSED(key);
	UNUSED(object_type_name);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
}

/** Drop default indices on specific object type in specific database.
 *
 * First uses the database by calling BPTEMdFDB::useDatabase() (or its
 * polymorphic instance), then calling
 * BPTEMdFDB::dropIndicesOnObjectType(const std::string&).
 *
 * Note that the encryption key is only used for SQLite, and only if
 * you have an SQLite that supports encryption.
 *
 * @see BPTEMdFDB::useDatabase(), BPTEMdFDB::dropIndicesOnObjectType(const
 * std::string&).
 *
 * @param database_name The database to use and drop indexes on.
 * @param key Just passed to BPTEMdFDB::useDatabase()
 * @param object_type_name Name of the object type on which to drop default indexes.
 * @return True on no database error, false if a database error occurred.
 */
bool BPTEMdFDB::dropIndicesOnObjectType(const std::string& database_name, const std::string& key, const std::string& object_type_name)
{
	UNUSED(database_name);
	UNUSED(key);
	UNUSED(object_type_name);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
}

/** Create default indices on specific object type in current database.
 *
 * Calls BPTEMdFDB::createIndicesOnOTObjects() (or its polymorphic variant).
 *
 * @see BPTEMdFDB::createIndicesOnOTObjects(),
 *
 * @param object_type_name Name of the object type on which to create default indexes.
 * @return True on no database error, false if a database error occurred.
 */
bool BPTEMdFDB::createIndicesOnObjectType(const std::string& object_type_name)
{
	UNUSED(object_type_name);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
}

/** Drop default indices on specific object type in current database.
 *
 * Calls BPTEMdFDB::dropIndicesOnOTObjects(), and, if necessary,
 * BPTEMdFDB::dropIndicesOnOTMonadMs() (or their polymorphic variants).
 *
 * @see BPTEMdFDB::dropIndicesOnOTObjects(),
 * BPTEMdFDB::dropIndicesOnOTMonadMs().
 *
 * @param object_type_name Name of the object type on which to drop default indexes.
 * @return True on no database error, false if a database error occurred.
 */
bool BPTEMdFDB::dropIndicesOnObjectType(const std::string& object_type_name)
{
	UNUSED(object_type_name);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
}

/** Drop default indices on OT_objects.
 *
 * @param object_type_name Name of the object type on which to drop default indexes.
 * @return True on no database error, false if a database error occurred.
 */
bool BPTEMdFDB::dropIndicesOnOTObjects(const std::string& object_type_name)
{
	UNUSED(object_type_name);

	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
}

/** Drop index.
 *
 * @param index_name The name of the index to drop.
 * @param table_name The name of the table which has the index to drop.
 * @return True on no database error, false if a database error occurred.
 */
void BPTEMdFDB::dropIndex(const std::string& index_name, const std::string& table_name)
{
	UNUSED(index_name);
	UNUSED(table_name);
	
	// Just return ... it doesn't matter that
	// nothing happens.
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
EMdFComparison* BPTEMdFDB::getEMdFComparison(const std::string& left_feature_name, 
					     id_d_t left_type_id_d, 
					     const std::string& object_type_name, 
					     id_d_t object_type_id, 
					     eComparisonOp e, 
					     EMdFValue *right_hand_side)
{
	UNUSED(left_feature_name);
	UNUSED(left_type_id_d);
	UNUSED(object_type_name);
	UNUSED(object_type_id);
	UNUSED(e);
	UNUSED(right_hand_side);
	
	return 0;
}

EMdFComparison *BPTEMdFDB::getEMdFComparison(const std::string& left_feature_name, 
					     id_d_t left_type_id_d, 
					     const std::string& object_type_name,
					     id_d_t object_type_id,
					     const std::list<EnumConstInfo>& in_enum_list)
{
	UNUSED(left_feature_name);
	UNUSED(left_type_id_d);
	UNUSED(object_type_name);
	UNUSED(object_type_id);
	UNUSED(in_enum_list);
	
	return 0;
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
EMdFComparison *BPTEMdFDB::getEMdFComparison(const std::string& left_feature_name, 
					     id_d_t left_type_id_d, 
					     const std::string& object_type_name,
					     id_d_t object_type_id,
					     const IntegerList *pIntegerList)
{
	UNUSED(left_feature_name);
	UNUSED(left_type_id_d);
	UNUSED(object_type_name);
	UNUSED(object_type_id);
	UNUSED(pIntegerList);
	
	return 0;
}


EMdFFFactor* BPTEMdFDB::getEMdFFFactor(EMdFFFactor* ffactor)
{
	UNUSED(ffactor);
	
	return 0;
}


EMdFFFactor* BPTEMdFDB::getEMdFFFactor(EMdFFFeatures* ffeatures)
{
	UNUSED(ffeatures);
	
	return 0;
}


EMdFFFactor* BPTEMdFDB::getEMdFFFactor(EMdFComparison* feature_comparison)
{
	UNUSED(feature_comparison);
	
	return 0;
}


EMdFFTerm* BPTEMdFDB::getEMdFFTerm(EMdFFTerm* fterm, EMdFFFactor* ffactor)
{
	UNUSED(fterm);
	UNUSED(ffactor);
	
	return 0;
}


EMdFFFeatures* BPTEMdFDB::getEMdFFFeatures(EMdFFFeatures* ffeatures, EMdFFTerm* fterm)
{
	UNUSED(ffeatures);
	UNUSED(fterm);
	
	return 0;
}






/** Tells us whether the backend can do a certain eComparisonOp operator.
 *@internal
 *
 * Part of the emdf_ffeatures.cpp machinery, which is for creating
 * pre-query strings in MQL.
 *
 * We cannot do pre-querying yet, so we just return false.
 * That way, we won't be able to do any emdf_ffeatures!
 *
 * @param e The comparison operator to test.
 *
 * @param kind eEVkind corresponding to the left-hand-side feature.
 *
 * @return True if the backend can do the comparison op; false otherwise.
 */
bool BPTEMdFDB::canDoComparisonOp(eComparisonOp e, eEVkind kind)
{
	UNUSED(e);
	UNUSED(kind);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
}


/** Escape a string so that it can be used in an SQL 'string'.
 *@internal
 *
 * MUST be overridden in descendant classes.
 *
 * @param str The string to escape.
 * @return The escaped string.
 */
std::string BPTEMdFDB::escapeStringForSQL(const std::string& str)
{
	// Just return identity.
	return str;
}


bool BPTEMdFDB::canDeclareEncoding(eCharsets charset) const
{
	return charset == kCSUTF8;
}


BPTEMdFConnection *BPTEMdFDB::getConnection(void)
{
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return 0;
}

std::string BPTEMdFDB::FeatureInfo2SQLvalue(const std::string& OTN,
					    id_d_t object_type_id,
					    bool bCreateStringIDDIfNotThere,
					    const FeatureInfo& fi)
{
	UNUSED(OTN);
	UNUSED(object_type_id);
	UNUSED(bCreateStringIDDIfNotThere);
	UNUSED(fi);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return "";
}

bool BPTEMdFDB::getStringOffset(const std::string& object_type_name,
				const std::string& feature_name,
				const std::string& instr,
				long& out_offset)
{
	UNUSED(object_type_name);
	UNUSED(feature_name);
	UNUSED(instr);
	UNUSED(out_offset);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
}

bool BPTEMdFDB::getStringSetStrings(const std::string& object_type_name, 
				    id_d_t object_type_id,
				    const std::string& feature_name,
				    /* out */ Table& result)
{
	UNUSED(object_type_name);
	UNUSED(object_type_id);
	UNUSED(feature_name);
	UNUSED(result);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
}

bool BPTEMdFDB::dumpPayload(const std::string& new_database_name, const std::string& new_key)
{
	UNUSED(new_database_name);
	UNUSED(new_key);
	
	DEBUG_X_IS_WRONG("BPTEMdFDB::dumpPayload", "The BPT backend is not present in the Open Source version of Emdros.\nFor licensing the BPT backend, please contact Ulrik Sandborg-Petersen:\nulrikp-at-emdros{dot}org ... thanks!");
	return false;
}
