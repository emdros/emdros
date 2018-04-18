/*
 * emdfdb.h
 *
 * EMdF database base class
 *
 * Ulrik Petersen
 * Created: 1/27-2001
 * Last update: 3/1-2017
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2017 Ulrik Sandborg-Petersen
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

/**@file emdfdb.h
 *@brief Header file for EMdFDB (EMdF layer)
 */

#ifndef EMDFDB__H__
#define EMDFDB__H__

#ifndef SWIG
#include "emdf.h"
#include "infos.h"
#include <string>
#include <list>
#include <set>
#include <vector>
#include <sstream>
#include <map>
#include "emdf_enums.h"
#include "emdf_exception.h"
#include "string_func.h"
#endif // !defined SWIG


#ifndef SWIG
/** Object type information for the object type cache.
 *\ingroup EMdF
 **/
class ObjectTypeCacheInfo {
 public:
	id_d_t m_object_type_id;
	long m_object_type_flags;
	monad_m m_largest_object_length;
        ObjectTypeCacheInfo(id_d_t object_type_id, long object_type_flags, monad_m largest_object_length) : m_object_type_id(object_type_id), m_object_type_flags(object_type_flags), m_largest_object_length(largest_object_length) {};
};

/** A std::map mapping an id_d_t to a std::pair of std::string and long.
 *@internal
 */
typedef std::map<std::string, ObjectTypeCacheInfo> String2OTCacheInfoMap;

/** A std::map mapping a std::pair of id_d_t and std::string to a
 * std::map mapping an id_d_t to a std::string.
 *@internal
 */
typedef std::map<std::pair<id_d_t, std::string>, std::map<id_d_t, std::string> > IDDStringPair2IDDStringMapMap;

/** A std::map mapping a std::pair of id_d_t and std::string to a
 * std::map mapping a std::string to an id_d_t.
 *@internal
 */
typedef std::map<std::pair<id_d_t, std::string>, std::map<std::string, id_d_t> > IDDStringPair2StringIDDMapMap; 
#endif 

class IntegerList; // forward declaration
class SetOfMonads; // forward declaration
class FastSetOfMonads; // forward declaration
class EMdFValue; // forward declaration
class EMdFComparison; // forward declaration
class EMdFFFactor; // Forward declaration
class EMdFFTerm; // Forward declaration
class EMdFFFeatures; // Forward declaration
class Inst; // Forward declaration
class IDDList; // Forward declaration
class Table; // Forward declaration
class EMdFConnection; // Forward declaration
class EnumConstCache; // Forward declaration
class StringSetsCache; // Forward declaration
class StringSetCache; // Forward declaration
class InstObject; // Forward declaration

/**The main implementation of the EMdF layer.
 *\ingroup EMdF
 * \section Overview
 *
 * EMdFDB is the interface to the EMdF layer.  You must not
 * instantiate an object of class EMdFDB.  Instead, either use
 * EmdrosEnv or one of the backend-specific subclasses (PgEMdFDB,
 * MySQLEMdFDB or SQLiteEMdFDB).
 *
 * The EMdF database classes are structured in the following
 * hierarchy:
 *
 * - EMdFDB
 *   - PgEMdFDB
 *   - MySQLEMdFDB
 *   - SQLiteEMdFDB 
 *
 * \section Boolean return value
 *
 * For the functions which return a boolean, the return value
 * generally means the following:
 *
 * - true: No error.
 * - false: An error occurred. Use the error interface to get to know
 *          what the error was. The values of any reference parameters which
 *          should have returned something are undefined.
 *
 * The only exceptions to this rule in the user-interface
 * (non-internal) are the transaction interface methods and the
 * ConnectionOk() method.
 *
 * \section Special 'emdf' database name
 *
 * All of the backends recognize the special database name 'emdf'
 * (without the quotes). This is a valid database name, but will not
 * associate the connection with any database.
 *
 * Hence, you can use 'emdf' as a default database name used until you
 * issue a 'USE DATABASE' MQL statement or call the
 * EMdFDB::useDatabase() method.
 *
 * - On PostgreSQL, the user is actually connected to the 'template1' database.
 * - On MySQL, the user is connected with no default associated database.
 * - On SQLite, the user is not connected to any database.
 */
class EMdFDB {
 protected:
	EMdFConnection *pConn;
	std::string m_local_errormessage;
	Table *m_enum_cache;
	EnumConstCache *m_enum_const_cache;
	Table *m_feature_cache;
	String2OTCacheInfoMap m_object_type_cache_map;
	StringSetsCache *m_string_sets_cache;
	long m_schema_version;
	friend class EMdFComparison;
	eBackendKind m_backend_kind;
	int m_max_table_name_length;
 public:
	//
	// Construction and destruction
	EMdFDB(eBackendKind backend_kind);
 private:
	// Leave unimplemented, so as to prevent the compiler from
	// generating one, which would crash the system.
	EMdFDB(const EMdFDB& other);
	EMdFDB& operator=(const EMdFDB& other);
 public:
	virtual ~EMdFDB();        

	// Get backend-name
	std::string getBackendName(void);
	eBackendKind getBackendKind(void) { return m_backend_kind; };


#ifndef SWIG
	//
	// Sequence manipulation
	virtual bool getNextObjectID_D(/* out */ id_d_t& id_d);
	virtual bool getNextTypeID(/* out */ id_d_t& id_d);
	virtual bool getNextOtherID(/* out */ id_d_t& id_d);
	virtual bool setNextObjectID_DIfNotHigher(id_d_t next_id_d);
 protected:
	/** Get next ID from one of the two sequences.
	 *\internal
	 *
	 * Returns in \p out the id_d_t that is next in the sequence \p
	 * sequence.
	 *
	 * Must be overridden in a descendant class.
	 *
	 * @param sequence the sequence to use. Can either be
	 * SEQUENCE_OBJECT_ID_DS or SEQUENCE_OTHER_IDS.
	 * @param out the value to be returned.
	 * @return true on no DB error, false on DB error.
	 */
	virtual bool getNextID(int sequence, id_d_t& out) { UNUSED(sequence); UNUSED(out); return false; };
 public:
#endif // !defined SWIG
  

	//
	// Database manipulation
#ifndef SWIG
	virtual bool createDatabase(const std::string& db_name, eCharsets charset, const std::string& key);
	// Uses default charset.
	virtual bool createDatabase(const std::string& db_name, const std::string& key);
	virtual bool initializeDatabase(const std::string& db_name, const std::string& key);
	virtual bool dropDatabase(const std::string& db_name);
	virtual void databaseNameIsOK(const std::string& db_name, bool& bIsOK);
#endif
	virtual bool useDatabase(const std::string& db_name, const std::string& key, bool bIsInitializingOrCreating);
	virtual bool useDatabase(const std::string& db_name, const std::string& key);
	virtual bool dbIsInitialized(bool& bIsInitialized);

#ifndef SWIG
	virtual bool getSchemaVersion(/* out */ long& schema_version);
 protected:
	virtual bool setSchemaVersion(long new_schema_version);
	virtual bool createSchemaVersionTable(long initial_schema_version);
 public:
#endif


	//
	// Object type manipulation
#ifndef SWIG
	virtual bool createObjectType(const std::string& object_type_name,
				      const std::list<FeatureInfo>& FeatureInfos,
				      eObjectRangeType objectRangeType,
				      eMonadUniquenessType monadUniquenessType,
				      /* out */ id_d_t& object_type_id);
 protected:
	virtual bool createObjectTypeOT_objects(const std::string& object_type_name,
						eObjectRangeType objectRangeType,
						eMonadUniquenessType monadUniquenessType,
						const std::list<FeatureInfo>& FeatureInfos);

  
 public:
	virtual bool dropObjectType(const std::string& object_type_name, id_d_t object_type_id);
	virtual bool objectTypeExists(const std::string& object_type_name, 
				      /* out */ bool& result, 
				      /* out */ id_d_t& object_type_id,
				      /* out */ eObjectRangeType& objectRangeType,
				      /* out */ eMonadUniquenessType& monadUniquenessType); 
	virtual bool objectTypeExists(id_d_t object_type_id,
				      /* out */ bool& result, 
				      /* out */ std::string& object_type_name,
				      /* out */ eObjectRangeType& objectRangeType,
				      /* out */ eMonadUniquenessType& monadUniquenessType); 
	virtual bool getLargestObjectLength(const std::string& object_type_name,
					    eObjectRangeType objectRangeType,
					    /* out */ monad_m& largest_object_length);
	virtual bool getLargestObjectLengthFromOT_objects(const std::string& object_type_name,
							  eObjectRangeType objectRangeType,
							  /* out */ monad_m& largest_object_length);						    
 protected:
	virtual bool setLargestObjectLengthIfNecessary(const std::string& object_type_name,
						       monad_m new_largest_object_length,
						       eObjectRangeType objectRangeType,
						       bool bSetUnconditionally);
 public:

	//
	// Enumeration manipulation
	virtual bool createEnum(const std::string& enum_name, 
				/* out */ id_d_t& enum_id);
	virtual bool dropEnum(id_d_t enum_id);
	virtual bool enumExists(const std::string& enum_name, 
				/* out */ bool& result,
				/* out */ id_d_t& enum_id);
	virtual bool enumExists(id_d_t enum_id,
				/* out */ bool& result,
				/* out */ std::string& enum_name);
	virtual bool getDefault(id_d_t enum_id,
				/* out */ bool& result,
				/* out */ std::string& default_constant_name);

	//
	// Enumeration constant manipulation
	virtual bool createEnumConstant(id_d_t enum_id,
					const std::string& enum_name,
					const EnumConstInfo& ec_info);
	virtual bool enumConstExists(const std::string& enum_const_name, 
				     id_d_t enum_id, 
				     /* out */ bool& result,
				     /* out */ long& value,
				     /* out */ bool& is_default);
	virtual bool enumConstExists(long value,
				     id_d_t enum_id,
				     bool& bExists,
				     /* out */ std::string& enum_const_name,
				     /* out */ bool& is_default);
	// Returns the string-representation of an enumeration constant in 
	// enum enum_name with the value value.
	virtual bool getEnumConstNameFromValue(long value,
					       const std::string& enum_name, 
					       /* out */ std::string& enum_const_name);
	virtual bool dropEnumConst(id_d_t enum_id, const std::string& enum_const_name, long value);
	virtual bool updateEnumConst(const std::string& enum_const_name, 
				     id_d_t enum_id,
				     long old_value,
				     long new_value);
	virtual bool setDefaultEnumConst(id_d_t enum_id,
					 const std::string& enum_value_name);

	//
	// Feature manipulation
	virtual bool createFeature(const std::string& feature_name,
				   const std::string& object_type_name,
				   id_d_t object_type_id, 
				   id_d_t feature_type_id, 
				   const std::string& default_value, 
				   bool computed);  // Assumes it does not
	// exist
	virtual bool addFeature(const std::string& object_type_name,
				id_d_t object_type_id,
				const FeatureInfo& fi);
	virtual bool dropFeature(const std::string& object_type_name,
				 const std::string& feature_name,
				 id_d_t feature_type_id,
				 id_d_t object_type_id);
 protected:
	virtual bool addFeatureToOT_objects(const std::string& object_type_name,
					    id_d_t object_type_id,
					    eObjectRangeType objectRangeType,
					    eMonadUniquenessType monadUniquenessType,
					    const FeatureInfo& fi);
	virtual bool dropFeatureFromOT_objects(const std::string& object_type_name,
					       const std::string& feature_name,
					       eObjectRangeType objectRangeType,
					       eMonadUniquenessType monadUniquenessType,
					       id_d_t object_type_id);
 public:
	virtual bool featureExists(const std::string& feature_name,
				   id_d_t object_type_id,
				   /* out */ bool& result,
				   /* out */ id_d_t& feature_type_id,
				   /* out */ std::string& default_value,
				   /* out */ bool& is_computed);
	// Assumes everything exists 

  
	virtual bool getFeatures(const std::string& object_type_name,
				 id_d_t object_type_id,
				 eObjectRangeType objectRangeType, 
				 const std::list<FeatureInfo>& FeatureInfos,
				 id_d_t object_id_d,
				 /* out */ std::list<EMdFValue*>& results); 
	virtual bool getFeatures(const std::string& object_type_name,
				 id_d_t object_type_id,
				 eObjectRangeType objectRangeType, 
				 const std::list<FeatureInfo>& FeatureInfos,
				 const SetOfMonads& object_id_ds_set,
				 /* out */ std::list<std::list<std::string> >& results);


 protected:
	virtual bool getFeaturesByQuery(const std::string& object_type_name,
					id_d_t object_type_id,
					eObjectRangeType objectRangeType,
					const std::list<FeatureInfo>& FeatureInfos,
					const SetOfMonads& object_id_ds_set,
					/* out */ std::list<std::list<std::string> >& results);
	virtual bool getFeaturesByQueryExec(const std::string query, 
					    const std::string& normalized_object_type_name,
					    id_d_t object_type_id,
					    eObjectRangeType objectRangeType,
					    const std::list<FeatureInfo>& FeatureInfos,
					    const std::vector<id_d_t>& feature_types_vec,
					    const std::vector<std::string>& feature_names_vec,
					    const std::vector<StringSetCache*>& string_set_caches_vec,
					    /* out */ std::list<std::list<std::string> >& results);
	virtual bool getComputedFeatures(const std::string& object_type_name,
					 const std::list<FeatureInfo>& FeatureInfos,
					 const SetOfMonads& object_id_ds_set,
					 /* out */ std::list<std::list<std::string> >& results);
	virtual bool getComputedFeature(const std::string& object_type_name, 
					const FeatureInfo& fi, 
					id_d_t object_id_d, 
					/* out */ std::string& comp_result);
  
	virtual void addStringToSetCacheIfNotAlreadyThere(id_d_t object_type_id,
							  const std::string& encoded_feature_name,
							  id_d_t set_id_d,
							  const std::string& string_value);
  
	virtual bool findStringInSetCacheFromID_D(id_d_t object_type_id,
						  const std::string& encoded_feature_name,
						  id_d_t set_id_d,
						  /* out */ std::string& result);
	virtual bool findID_DInSetCacheFromString(id_d_t object_type_id,
						  const std::string& encoded_feature_name,
						  const std::string& string_value,
						  /* out */ id_d_t& set_id_d);
	virtual bool loadStringSetIntoCache(const std::string& object_type_name, 
					    id_d_t object_type_id,
					    const std::string& feature_name);
	virtual void getStringSetID_DsInCacheAsSOM(const std::string& normalized_object_type_name,
						   id_d_t object_type_id,
						   const std::string& encoded_feature_name,
						   SetOfMonads& id_d_set /* result */);
	virtual bool getID_DFromStringSet(const std::string& normalized_object_type_name,
					  id_d_t object_type_id,
					  const std::string& encoded_feature_name,
					  const std::string& encoded_string_to_find, // Must be encoded with string2dbstring if it is a STRING feature
					  const std::string& original_string,
					  bool bCreateIfNotThere,
					  /* out */ std::string& result); // Even though it is an id_d, we get is as a string.
	virtual bool createStringSetTable(const std::string& object_type_name, 
					  const std::string& feature_name);
	virtual bool dropStringSetTable(const std::string& object_type_name, 
					const std::string& feature_name);
	virtual bool addStringToStringSetTable(const std::string& normalized_object_type_name,
					       const std::string& encoded_feature_name,
					       const std::string& encoded_string_to_find, // Must be encoded with escapeStringForSQL
					       /* out */ id_d_t& result);
	virtual bool createIndicesOnStringSetTable(const std::string& object_type_name,
						   const std::string& feature_name);
	virtual bool dropIndicesOnStringSetTable(const std::string& object_type_name,
						 const std::string& feature_name);
 public:
	virtual bool getStringSetStrings(const std::string& object_type_name, 
					 id_d_t object_type_id,
					 const std::string& feature_name,
					 /* out */ Table& result);
 public:

	//
	// Segment manipulation
	virtual bool createSegment(const std::string& segment_name, monad_m
				   first, monad_m last);

	//
	// Upgrading to 1.1.0 and beyond
	bool upgradeDatabase(const std::string& database_name, int nCurrentVersion);
	bool getCurrentDatabaseVersion(const std::string& database_name, 
				       const std::string& key,
				       std::string& /* out */ strCurrentVersion,
				       int& /* out */ nCurrentVersion,
				       int& /* out */ nVersionWhichWeCanUpgradeTo,
				       std::string& /* out */ strVersionWhichWeCanUpgradeTo,
				       bool& /* out */ bNeedsUpgrading,
				       bool& /* out */ bCanUpgrade);
 protected:
	// utility functions
	std::string normalizeTableName(const std::string& table_name, bool bCheckIfTableExists);
	std::string normalizeIndexName(const std::string& table_name);
	virtual bool tableExists(std::string table_name, bool& /* out */ bTableExists);
	bool objectTypeHasObjects(const std::string& object_type_name,
				  bool& /* out */ bHasObjects);
#endif  // !defined SWIG
 public:

	// min_m and max_m
 protected:  
	virtual bool createMin_max_m_table(void);
	virtual bool setMin_max_m_fromObjectTypes(bool bBeVerbose);
	virtual bool setMin_max_m_fromObjectType(const std::string& object_type_name, bool bBeVerbose);
	virtual bool setMax_m(monad_m max_m_candidate, bool bSetUnconditionally = false);
	virtual bool setMin_m(monad_m min_m_candidate, bool bSetUnconditionally = false);
 public:

	virtual bool getMin_m(monad_m& /* out */ min_m);
	virtual bool getMax_m(monad_m& /* out */ max_m);
	virtual bool getAll_m_1(SetOfMonads& /* out */ all_m_1);


	// Single range objects
#ifndef SWIG 
	// Not swigwrapped because it resulted in a SWIGTYPE_p_eObjectRangeType on Java.
	bool getOTObjectRangeType(const std::string& object_type_name,
				  /* out */ eObjectRangeType& objectRangeType);
	bool getOTMonadUniquenessType(const std::string& object_type_name,
				      /* out */ eMonadUniquenessType& monadUniquenessType);
#endif

	// Indices
	bool createIndicesOnDatabase(const std::string& database_name, const std::string& key);
	bool dropIndicesOnDatabase(const std::string& database_name, const std::string& key);
	bool createIndicesOnObjectType(const std::string& object_type_name);
	bool dropIndicesOnObjectType(const std::string& object_type_name);
	bool createIndicesOnObjectType(const std::string& database_name, const std::string& key, const std::string& object_type_name);
	bool dropIndicesOnObjectType(const std::string& database_name, const std::string& key, const std::string& object_type_name);
    
	//
	// Querying
#ifndef SWIG
 protected:
	
	virtual bool getObjectsHavingMonadsInMonadSet(const std::string object_type_name,
						      const SetOfMonads& monad_ms,
						      std::set<id_d_t>& objects_already_seen,					      
						      eObjectRangeType objectRangeType,

						      /* out */ Table& result);
	bool getObjectsHavingMonadsInFromSingleUniqueMonadOT(const std::string object_type_name,
							     id_d_t object_type_id,
							     const SetOfMonads& monad_ms,
							     const SetOfMonads& all_m_1,
							     const std::list<FeatureInfo>& features_to_get,
							     /* Out */ Inst *pInst);

	bool getObjectsHavingMonadsInFromSingleUniqueMonadExec(const std::string& OTN,
							       id_d_t object_type_id,
							       const std::string& str_monad_constraint1,
							       const std::string& query_prefix,
							       const std::string& str_features_to_get,
							       const std::vector<id_d_t>& feature_types_vec,
							       const std::vector<std::string>& feature_names_vec,
							       const std::vector<StringSetCache*>& string_set_caches_vec,
							       /* out */ Inst *pInst);

	
	bool getObjectsHavingMonadsInExec(const std::string& OTN,
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
					  /* out */ Inst *pInst);
	void local_get_getObjectsHavingMonadsInSQLQuery(eObjectRangeType objectRangeType,
							
							const std::string& str_features_to_get,
							const std::vector<id_d_t>& feature_types_vec,
							const std::vector<std::string>& feature_names_vec,
							const std::string& from_string,
							const std::string& join_string,
							bool bHasMonadConstraints,
							const std::string& monad_set_name,
							/* out */ std::string& result);
	bool getObjectsHavingMonadsIn(const std::string object_type_name,
				      id_d_t object_type_id,
				      eObjectRangeType objectRangeType,
				      monad_m first_monad,
				      monad_m last_monad,
				      const FastSetOfMonads& original_som,
				      const SetOfMonads& all_m_1,
				      const std::string& str_features_to_get,
				      const std::vector<id_d_t>& feature_types_vec,
				      const std::vector<std::string>& feature_names_vec,
				      const std::vector<StringSetCache*>& string_set_caches_vec,
				      const std::string& from_string,
				      const std::string& join_string,
				      monad_m largest_object_length,
				      const std::string& monad_set_name,
				      /* out */ Inst *pInst);
 public:
	virtual bool getObjectsHavingMonadsInMonadSet(const std::string object_type_name,
						      const SetOfMonads& monad_ms,
						      eObjectRangeType objectRangeType,
						      /* out */ Table& result);
	virtual bool getObjectsHavingMonadsIn(const std::string object_type_name,
					      id_d_t object_type_id,
					      eObjectRangeType objectRangeType,
					      const SetOfMonads& monad_ms,
					      const SetOfMonads& all_m_1,
					      const std::list<FeatureInfo>& features_to_get,
					      const std::string& monad_set_name,
					      /* out */ Inst *pInst);
	virtual bool getInst(const std::string& object_type_name,
			     id_d_t object_type_id,
			     const SetOfMonads& Su,
			     const SetOfMonads& all_m_1,
			     const std::string& pre_query_string,
			     EMdFFFeatures *pre_query_constraints,
			     const std::list<FeatureInfo>& features_to_get,
			     const std::string& monad_set_name,
			     eMonadSetRelationOperation ms_operation,
			     /* out */ Inst& Result);
 protected:
	virtual bool getFeatureVectors(const std::list<FeatureInfo>& features_to_get,
				       const std::string& OTN,
				       id_d_t object_type_id,
				       eObjectRangeType objectRangeType, 
				       std::string& str_features_to_get,
				       std::vector<id_d_t>& feature_types_vec,
				       std::vector<std::string>& feature_names_vec,
				       std::vector<StringSetCache*>& string_set_caches_vec,
				       std::string& join_string,
				       std::string& from_string);
	virtual bool getFeatureValues(int no_of_features_to_get,
				      eObjectRangeType objectRangeType,	
				      int feature_column_index,
				      const std::vector<id_d_t>& feature_types_vec,
				      const std::vector<std::string>& feature_names_vec,
				      const std::vector<StringSetCache*>& string_set_caches_vec,
				      EMdFValue *pFeature_values);
	virtual bool getInst(const std::string& object_type_name,		
			     id_d_t object_type_id,
			     monad_m first_monad, monad_m last_monad,
			     const SetOfMonads& all_m_1,
			     const std::string& pre_query_string,
			     const std::list<FeatureInfo>& features_to_get,
			     const std::string& monad_set_name,
			     eObjectRangeType objectRangeType,
			     eMonadSetRelationOperation ms_operation,
			     /* out */ Inst& Result);
 public:

	virtual bool getObjectsStartingAtSm(const std::string& object_type_name,
					    monad_m Sm,
					    /* out */ IDDList& Result);
	virtual bool getSOMForObject(const std::string& object_type_name,
				     id_d_t object_id_d,
				     eObjectRangeType objectRangeType,
				     /* out */ SetOfMonads& Result);
	virtual bool getMonadsFromID_D(id_d_t object_id_d, 
				       id_d_t object_type_id,
				       /* out */ SetOfMonads& monads);
	virtual bool getMonadsFromID_Ds(const SetOfMonads& id_ds_set,
					const std::string &object_type_name,
					eObjectRangeType objectRangeType,
					Table *pOutTable);
	virtual bool getObjectTypes(/* out */ std::list<std::string>& Result);
	virtual bool getFeaturesForObjectType(id_d_t object_type_id,
					      /* out */ std::list<FeatureInfo>& Result);
	virtual bool getEnumerations(/* out */ std::list<std::string>& Result);
	virtual bool getEnumConstants(id_d_t enum_id,
				      /* out */ std::list<EnumConstInfo>& Result);
	virtual bool loadEnumConstantsIntoCache(id_d_t enum_id);
	virtual bool getObjectTypesUsingEnumeration(id_d_t enum_id,
						    /* out */ std::list<std::string>& Result);
	virtual bool getObjectsWithinMonads(const SetOfMonads& monads, 
					    const std::string& object_type_name,
					    eObjectRangeType objectRangeType,
					    /* out */ std::list<id_d_t>& result);

	// 
	// Object manipulation
	virtual bool objectID_DExistsInType(id_d_t object_id_d, 
					    const std::string& object_type_name,
					    /* out */ bool& Result);
	virtual bool objectID_DExists(id_d_t object_id_d, 
				      /* out */ bool& Result,
				      /* out */ id_d_t& object_type_id);
	virtual bool createObject(id_d_t object_id_d,
				  const std::string& object_type_name,
				  id_d_t object_type_id,
				  const SetOfMonads& monads,
				  eObjectRangeType objectRangeType,
				  const std::list<FeatureInfo>& features);
	virtual bool updateObject(id_d_t object_id_d,
				  const std::string& object_type_name,
				  id_d_t object_type_id,
				  const std::list<FeatureInfo>& features);
	virtual bool dropObject(id_d_t object_id_d, 
				id_d_t object_type_id, 
				const std::string& object_type_name,
				eObjectRangeType objectRangeType);
	virtual bool dropObjectsInMonads(const SetOfMonads& monads, 
					 const std::string& object_type_name, 
					 id_d_t object_type_id, 
					 eObjectRangeType objectRangeType,
					 std::list<id_d_t>& result);
	virtual bool createObjects(const std::string& object_type_name,
				   id_d_t object_type_id,
				   const std::list<FeatureInfo>& object_type_features,
				   std::list<InstObject*>& object_list,
				   eObjectRangeType objectRangeType,
				   /* out */ long& object_count);
 protected:
	virtual bool lockTablesForCreateObjects(const std::string& object_type_name, const std::list<FeatureInfo>& object_type_features) { UNUSED(object_type_name); UNUSED(object_type_features); return true; };
	virtual bool unlockTablesForCreateObjects(const std::string& object_type_name) { UNUSED(object_type_name); return true; };

	virtual void createObjectsOT_objects_data(const std::string object_type_name, 
						  id_d_t object_type_id,
						  const InstObject *pObject,
						  const SetOfMonads& som,
						  const std::list<FeatureInfo>& object_type_features,
						  eObjectRangeType objectRangeType,
						  /* out */ std::string& OT_objects_data);
	virtual bool createObjectsOT_objects_DB(const std::string& object_type_name,
						const std::string& OT_objects_data,
						eObjectRangeType objectRangeType,
						const std::list<FeatureInfo>& object_type_features);
	virtual void createObjectsInBetweenData(/* out */ std::string& OT_data);

 public:

	//
	// Feature name encoding/decoding
	static std::string encodeFeatureName(const std::string feature_name);
	static std::string decodeFeatureName(const std::string coded_feature_name);
	static std::string encodeFeatureNameForPrequeryString(const std::string feature_name);

	//
	// SQL production
	void makeFeatureSQLType(std::ostringstream& OT_Objects_stream, 
				const FeatureInfo& fi,
				bool add_not_null = true);
	std::string makeMonadConstraintsFromSOM(const SetOfMonads& monads, 
						eObjectRangeType objectRangeType);

#endif // !defined SWIG
	// DB maintenance
	virtual bool vacuum(bool bAnalyze);

	// Comparisons
#ifndef SWIG
	virtual EMdFComparison *getEMdFComparison(const std::string& left_feature_name, 
						  id_d_t left_type_id_d, 
						  const std::string& object_type_name, 
						  id_d_t object_type_id, 
						  eComparisonOp e, 
						  EMdFValue *right_hand_side);
	virtual EMdFComparison *getEMdFComparison(const std::string& left_feature_name, 
						  id_d_t left_type_id_d, 
						  const std::string& object_type_name,
						  id_d_t object_type_id,
						  const std::list<EnumConstInfo>& in_enum_list);
	virtual EMdFComparison *getEMdFComparison(const std::string& left_feature_name, 
						  id_d_t left_type_id_d, 
						  const std::string& object_type_name,
						  id_d_t object_type_id,
						  const IntegerList *pIntegerList);
	virtual EMdFFFactor* getEMdFFFactor(EMdFFFactor* ffactor);
	virtual EMdFFFactor* getEMdFFFactor(EMdFFFeatures* ffeatures);
	virtual EMdFFFactor* getEMdFFFactor(EMdFComparison* feature_comparison);
	virtual EMdFFTerm* getEMdFFTerm(EMdFFTerm* fterm, EMdFFFactor* ffactor);
	virtual EMdFFFeatures* getEMdFFFeatures(EMdFFFeatures* ffeatures, EMdFFTerm* fterm);
	virtual std::string eComparisonOp2BackendComparisonOp(eComparisonOp e);
	virtual bool canDoComparisonOp(eComparisonOp e, eEVkind kind);
	virtual eEVkind getEVkindFromTypeID_D(id_d_t type_id_d);
#endif

	// 
	// Monad manipulation
	virtual bool insertMonadRange(monad_m first, monad_m last);
	virtual bool insertMonadRangeOT(const std::string& object_type_name, monad_m first, monad_m last);

	// Monad set manipulation
#ifndef SWIG
	virtual bool selectMonadSets(/* out */ std::list<std::string>& monad_set_names);
	virtual bool monadSetExists(const std::string& monad_set_name, 
				    /* out */ bool& bExists,
				    /* out */ SetOfMonads& som);
	virtual bool createMonadSet(const std::string& monad_set_name, const SetOfMonads& som);
	virtual bool dropMonadSet(const std::string& monad_set_name);
#endif

	//
	// Helper functions
#ifndef SWIG
	bool typeIdToString(id_d_t type_id, std::string& result);
	bool typeIdToTableColumnType(id_d_t type_id, TableColumnType& tc_type, std::string& enum_type);
#endif // !defined SWIG

	// Transactions
	virtual bool beginTransaction(void);
	virtual bool commitTransaction(void);
	virtual bool abortTransaction(void);

	//
	// Error messages
	virtual std::string errorMessage(void);
	virtual bool connectionOk(void);
	void clearLocalError(void);
	std::string getLocalError(void);

	// Encodings
#ifndef SWIG
	virtual eCharsets getDefaultEncoding(void) const { return kCSISO_8859_1; };
	virtual bool canDeclareEncoding(eCharsets charset) const { UNUSED(charset); return true; };
 protected:
	virtual std::string getEncodingClause(eCharsets charset) const { UNUSED(charset); return ""; };
 public:
#endif // !defined SWIG

#ifndef SWIG
	void appendLocalError(std::string error);
	virtual std::string escapeStringForSQL(const std::string& str);
	std::string getMYSQL_BINARY(void);
 protected:
	virtual unsigned int getMaxNoOfJoins(void) const { return 6; };
	const char *getSQL_TEXT_TYPE() const;
	virtual std::string FeatureInfo2SQLvalue(const std::string& normalized_object_type_name,
						 id_d_t object_type_id,
						 bool bCreateStringIDDIfNotThere,
						 const FeatureInfo& fi);
	virtual bool createIndex(const std::string& index_name, const std::string& table_name, const std::list<std::string>& columns);
	virtual bool createIndex(const std::string& index_name, const std::string& table_name, const std::list<std::pair<std::string, unsigned int> >& columns);
	virtual bool createIndicesOnOTObjects(const std::string& OTN);
	virtual bool createMonadIndexOnOTObjects(const std::string& OTN,
						 eObjectRangeType objectRangeType);
	virtual void dropIndex(const std::string& index_name, const std::string& table_name);
	virtual bool dropIndicesOnOTObjects(const std::string& object_type_name);
	virtual bool dropMonadIndexOnOTObjects(const std::string& object_type_name);
	void getIndexAndTableNameForFeature(const std::string& object_type_name, 
					    const std::string& unencoded_feature_name, 
					    /* out */ std::string& table_name, 
					    /* out */ std::string& index_name);
	/** Create sequence tables
	 *\internal
	 * @return true on no DB error, false otherwise.
	 */
	virtual bool createSequenceTables() { return false; };
	void addEnumToCache(id_d_t enum_id, const std::string& enum_name);
	void addObjectTypeToCache(id_d_t object_type_id, const std::string& object_type_name, long object_type_flags, monad_m largest_object_length);
	bool addEnumConstToCache(id_d_t enum_id, const std::string& enum_name, const std::string& enum_const_name, long value, bool is_default);
	void addFeatureToCache(id_d_t object_type_id, const std::string& feature_name, id_d_t feature_type_id, const std::string& default_value, bool is_computed);
	void addFeatureToCacheIfNotAlreadyThere(id_d_t object_type_id, const std::string& feature_name, id_d_t feature_type_id, const std::string& default_value, bool is_computed);
	void deleteObjectTypeFromCache(const std::string& object_type_name);
#endif // !defined SWIG
};

#ifndef SWIG
/** Test whether a feature type ID is an INTEGER.
 *@internal
 *
 * @param feature_type_id The feature type id to test.
 *
 * @return True iff the feature type id is INTEGER.
 *
 */
inline bool featureTypeIdIsINTEGER(id_d_t feature_type_id) 
{ return (feature_type_id & FEATURE_TYPE_TYPE_MASK) == FEATURE_TYPE_INTEGER; }

/** Test whether a feature type ID is an ID_D
 *@internal
 *
 * @param feature_type_id The feature type id to test.
 *
 * @return True iff the feature type id is ID_D.
 *
 */
inline bool featureTypeIdIsID_D(id_d_t feature_type_id)
{ return (feature_type_id & FEATURE_TYPE_TYPE_MASK) == FEATURE_TYPE_ID_D; }

/** Test whether a feature type ID is an ASCII.
 *
 *@internal
 *
 * @param feature_type_id The feature type id to test.
 *
 * @return True iff the feature type id is ASCII.
 *
 */
inline bool featureTypeIdIsASCII(id_d_t feature_type_id)
{ return (feature_type_id & FEATURE_TYPE_TYPE_MASK) == FEATURE_TYPE_ASCII; }

/** Test whether a feature type ID is a STRING
 *@internal
 *
 * @param feature_type_id The feature type id to test.
 *
 * @return True iff the feature type id is STRING.
 *
 */
inline bool featureTypeIdIsSTRING(id_d_t feature_type_id)
{ return (feature_type_id & FEATURE_TYPE_TYPE_MASK) == FEATURE_TYPE_STRING; }

/** Test whether a feature type ID is an ASCII or a STRING
 *@internal
 *
 * @param feature_type_id The feature type id to test.
 *
 * @return True iff the feature type id is ASCII or STRING.
 *
 */
inline bool featureTypeIdIsASCIIorSTRING(id_d_t feature_type_id)
{ 
	id_d_t ftmasked = feature_type_id & FEATURE_TYPE_TYPE_MASK; 
	return ftmasked == FEATURE_TYPE_ASCII || ftmasked == FEATURE_TYPE_STRING; 
}

/** Test whether a feature type ID is an ENUM
 *@internal
 *
 * @param feature_type_id The feature type id to test.
 *
 * @return True iff the feature type id is an ENUM.
 *
 */
inline bool featureTypeIdIsENUM(id_d_t feature_type_id)
{ return (feature_type_id & FEATURE_TYPE_TYPE_MASK) == FEATURE_TYPE_ENUM; }

/** Test whether a feature type ID is a SET OF MONADS
 *@internal
 *
 * @param feature_type_id The feature type id to test.
 *
 * @return True iff the feature type id is an SET OF MONADS.
 *
 */
inline bool featureTypeIdIsSOM(id_d_t feature_type_id)
{ return (feature_type_id & FEATURE_TYPE_TYPE_MASK) == FEATURE_TYPE_SET_OF_MONADS; }

/** Test whether a feature type ID is FROM SET
 *@internal
 *
 * @param feature_type_id The feature type id to test.
 *
 * @return True iff the feature type id is FROM SET.
 *
 */
inline bool featureTypeIdIsFromSet(id_d_t feature_type_id)
{
	return ((feature_type_id & FEATURE_TYPE_TYPE_MASK) == FEATURE_TYPE_ASCII
		|| (feature_type_id & FEATURE_TYPE_TYPE_MASK) == FEATURE_TYPE_STRING)
		&& (feature_type_id & FEATURE_TYPE_FROM_SET) != 0;
}


/** Test whether a feature type ID is WITH INDEX
 *@internal
 *
 * @param feature_type_id The feature type id to test.
 *
 * @return True iff the feature type id is WITH INDEX.
 *
 */
inline bool featureTypeIdIsWithIndex(id_d_t feature_type_id)
{
	return (feature_type_id < MIN_ENUM_TYPE_ID)
		&& (feature_type_id & FEATURE_TYPE_WITH_INDEX) != 0;
}

/** Test whether a feature type ID is LIST OF.
 *@internal
 *
 * @param feature_type_id The feature type id to test.
 *
 * @return True iff the feature type id is LIST OF something.
 *
 */
inline bool featureTypeIdIsListOf(id_d_t feature_type_id)
{
	id_d_t ftmasked = feature_type_id & FEATURE_TYPE_TYPE_MASK; 
	return ftmasked == FEATURE_TYPE_LIST_OF_INTEGER
		|| ftmasked == FEATURE_TYPE_LIST_OF_ID_D
		|| ftmasked == FEATURE_TYPE_LIST_OF_ENUM;
}


/** Test whether a feature type ID is LIST OF INTEGER.
 *@internal
 *
 * @param feature_type_id The feature type id to test.
 *
 * @return True iff the feature type id is LIST OF INTEGER.
 *
 */
inline bool featureTypeIdIsListOfINTEGER(id_d_t feature_type_id)
{
	return (feature_type_id & FEATURE_TYPE_TYPE_MASK) == FEATURE_TYPE_LIST_OF_INTEGER;
}


/** Test whether a feature type ID is LIST OF ID_D.
 *@internal
 *
 * @param feature_type_id The feature type id to test.
 *
 * @return True iff the feature type id is LIST OF ID_D.
 *
 */
inline bool featureTypeIdIsListOfID_D(id_d_t feature_type_id)
{
	return (feature_type_id & FEATURE_TYPE_TYPE_MASK) == FEATURE_TYPE_LIST_OF_ID_D;
}


/** Test whether a feature type ID is LIST OF ENUM.
 *@internal
 *
 * @param feature_type_id The feature type id to test.
 *
 * @return True iff the feature type id is LIST OF ENUM.
 *
 */
inline bool featureTypeIdIsListOfENUM(id_d_t feature_type_id)
{
	return (feature_type_id & FEATURE_TYPE_TYPE_MASK) == FEATURE_TYPE_LIST_OF_ENUM;
}


/** Normalize an Object Type Name
 *@internal
 *
 * Currently, this only converts it to lower-case.
 *
 * Note how this ensures that all database names are case-insensitive
 * as far as MQL is concerned.
 *
 * @param object_type_name The object type name to normalize.
 *
 * @return The normalized object type name.
 */
inline std::string normalizeOTName(const std::string& object_type_name)
{
	std::string result;
	str_tolower(object_type_name, result);
	return result;
}

/** Strips an enum id_d_t of its lower bits.
 */
#define STRIP_ENUM_ID_OF_LOWER_BITS(X) (X & (0x7FFFFFFF - ((1 << SEQUENCE_TYPE_IDS_FREE_LOWER_BITS) - 1)))

/** Convert a "list of enum feature" id to an enum feature id.
 */
#define GET_ENUM_FEATURE_ID_FROM_LIST_OF_ENUM_FEATURE_ID(X) (STRIP_ENUM_ID_OF_LOWER_BITS(X) | FEATURE_TYPE_ENUM)

extern void local_get_getObjectsHavingMonadsInSQLQuery(eObjectRangeType objectRangeType,
						       
						       const std::string& str_features_to_get,
						       const std::vector<id_d_t>& feature_types_vec,
						       const std::vector<std::string>& feature_names_vec,
						       const std::string& from_string,
						       const std::string& join_string,
						       bool bHasMonadConstraints,
						       /* out */ std::string& result);


#endif

extern int hash_djb2(const std::string& instr, long hashtable_size);

extern std::string getStringFromeComparisonOp(eComparisonOp op);


#endif // EMDFDB__H__
