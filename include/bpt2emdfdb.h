/*
 * bpt2emdfdb.h
 *
 * EMdF database class for Bit Packed Tables
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

/**@file bpt2emdfdb.h
 *@brief Header file for BPT2EMdFDB (EMdF layer)
 */

#ifndef BPT2EMDFDB__H__
#define BPT2EMDFDB__H__

#ifndef SWIG
#include "emdfdb.h"
#endif // !defined SWIG

class BPT2EMdFComparison;
class BPT2EMdFConnection;

/**The Bit Packed Table implementation of the EMdF layer.
 *\ingroup EMdF
 * \section Overview
 *
 * BPT2EMdFDB is the interface to the EMdF layer.
 */
class BPT2EMdFDB : public EMdFDB {
	friend class BPT2EMdFComparison;
 public:
	//
	// Construction and destruction
	BPT2EMdFDB();
	BPT2EMdFDB(std::string database_name, std::string key);
 	virtual ~BPT2EMdFDB();        

#ifndef SWIG
	//
	// Sequence manipulation
	virtual bool getNextObjectID_D(/* out */ id_d_t& id_d) { UNUSED(id_d); return false; };
	virtual bool getNextTypeID(/* out */ id_d_t& id_d) { UNUSED(id_d); return false; };
	virtual bool getNextOtherID(/* out */ id_d_t& id_d) { UNUSED(id_d); return false; };
	virtual bool setNextObjectID_DIfNotHigher(id_d_t next_id_d) { UNUSED(next_id_d); return false; };
 protected:
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
				 const std::list<FeatureInfo>& FeatureInfos,
				 id_d_t object_id_d,
				 /* out */ std::list<EMdFValue*>& results); 
	virtual bool getFeatures(const std::string& object_type_name,
				 id_d_t object_type_id,
				 const std::list<FeatureInfo>& FeatureInfos,
				 const SetOfMonads& object_id_ds_set,
				 /* out */ std::list<std::list<std::string> >& results);


 protected:
	virtual bool loadStringSetIntoCacheBPT2(const std::string& object_type_name, 
					       id_d_t object_type_id,
					       const std::string& feature_name,
					       /* may be 0 */ Table *pTable);
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
	bool getStringOffset(const std::string& object_type_name,
			     const std::string& feature_name,
			     const std::string& instr,
			     long& out_offset);
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
	bool tableExists(std::string table_name, bool& /* out */ bTableExists);
	bool objectTypeHasObjects(const std::string& object_type_name,
				  bool& /* out */ bHasObjects);
#endif  // !defined SWIG
 public:

	// min_m and max_m
 protected:  
	bool createMin_max_m_table(void);
	bool setMin_max_m_fromObjectTypes(bool bBeVerbose);
	bool setMin_max_m_fromObjectType(const std::string& object_type_name, bool bBeVerbose);
	bool setMax_m(monad_m max_m_candidate, bool bSetUnconditionally = false);
	bool setMin_m(monad_m min_m_candidate, bool bSetUnconditionally = false);
 public:

	virtual bool getMin_m(monad_m& /* out */ min_m);
	virtual bool getMax_m(monad_m& /* out */ max_m);



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
					      const std::string& pre_query_string,
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
	virtual bool getInstLocalBPT2(const std::string& object_type_name,		
				     id_d_t object_type_id,
				     const SetOfMonads& Su,
				     const SetOfMonads& all_m_1,
				     EMdFFFeatures *pre_query_constraints,
				     const std::list<FeatureInfo>& features_to_get,
				     eObjectRangeType objectRangeType,
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

#endif // !defined SWIG
	// DB maintenance
	// We always succeed, and do nothing, since we are a static database
	virtual bool vacuum(bool bAnalyze) { UNUSED(bAnalyze); return true; };

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

 	virtual bool canDoComparisonOp(eComparisonOp e, eEVkind kind);
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
	// Transaction
	// We always succeed, since we are a static database.
	virtual bool beginTransaction(void) { return true; }
	virtual bool commitTransaction(void) { return true; }
	virtual bool abortTransaction(void) { return true; }

	//
	// Error messages
	virtual bool connectionOk(void);

	// Encodings
#ifndef SWIG
	virtual eCharsets getDefaultEncoding(void) const { return kCSISO_8859_1; };
	virtual bool canDeclareEncoding(eCharsets charset) const;
 protected:
	BPT2EMdFConnection *getConnection(void);
	// virtual std::string getEncodingClause(eCharsets charset) const;
 public:
#endif // !defined SWIG

#ifndef SWIG
	virtual std::string escapeStringForSQL(const std::string& str);
 protected:
	virtual unsigned int getMaxNoOfJoins(void) const { return 0; };
	virtual std::string FeatureInfo2SQLvalue(const std::string& normalized_object_type_name,
						 id_d_t object_type_id,
						 bool bCreateStringIDDIfNotThere,
						 const FeatureInfo& fi);

	// and we might as well report success, since nothing was done
	virtual bool createIndex(const std::string& index_name, const std::string& table_name, const std::list<std::string>& columns);
	virtual bool createIndex(const std::string& index_name, const std::string& table_name, const std::list<std::pair<std::string, unsigned int> >& columns);
	virtual bool createIndicesOnOTObjects(const std::string& OTN);
	virtual void dropIndex(const std::string& index_name, const std::string& table_name);
	virtual bool dropIndicesOnOTObjects(const std::string& object_type_name);
	/** Create sequence tables
	 *\internal 
	 *
	 * Does nothing, since we are a static database.  Just reports
	 * success.
	 *
	 * @return true on no DB error, false otherwise.
	 */
	virtual bool createSequenceTables() { return true; };
#endif // !defined SWIG
};



#endif // BPT2EMDFDB__H__
