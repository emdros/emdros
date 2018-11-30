/*
 * sqlite3emdfdb.h
 *
 * SQLite3 EMdF database base class
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


/**@file sqlite3emdfdb.h
 *@brief Header file for SQLite3 EMdF database (EMdF layer)
 */


#ifndef SQLITE3EMDFDB__H__
#define SQLITE3EMDFDB__H__

#include "emdros-lconfig.h"
#ifndef SWIG
#include "emdf.h"
#include "emdfdb.h"
#include <list>
#include <map>
#endif // !defined SWIG
#include <string>



/** SQLite3 implementation of EMdFDB.
 */
class SQLite3EMdFDB : public EMdFDB {
 private:
 public:
	SQLite3EMdFDB(std::string database_name, std::string key);
	virtual ~SQLite3EMdFDB();
#if defined(USE_SQLITE3) && USE_SQLITE3
#ifndef SWIG
	virtual bool useDatabase(const std::string& db_name, const std::string& key) { return this->useDatabase(db_name, key, false); };

	// DB maintenance
	virtual bool vacuum(bool bAnalyze);

	// Create/Drop database
	virtual bool createDatabase(const std::string& db_name, eCharsets charset, const std::string& key);
	virtual bool dropDatabase(const std::string& db_name);
	virtual void databaseNameIsOK(const std::string& db_name, bool& bIsOK);


 protected:
	virtual unsigned int getMaxNoOfJoins(void) const { return 0; }; // Disallow joins in SQLite 3. The reason is that SQLite 3.4.0 doesn't handle the type of joins I need very well (cross joins).
	virtual bool useDatabase(const std::string& db_name, const std::string& key, bool bCreate);

	// Feature manipulation
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
#ifndef SWIG
	bool getIndices(const std::string& object_type_name,
			std::map<std::string, std::pair<bool, std::string> >& result);
	bool createIndicesFromIndexMap(const std::string& object_type_name,
				       std::map<std::string, std::pair<bool, std::string> >& mymap);
	void removeIndicesWithFeature(const std::string feature_name,
				      std::map<std::string, std::pair<bool, std::string> >& result);
	virtual bool createMonadIndexOnOTObjects(const std::string& OTN,
						 eObjectRangeType objectRangeType);
	virtual bool dropMonadIndexOnOTObjects(const std::string& object_type_name);

#endif // !defined(SWIG)
 public:
	virtual bool createObjects(const std::string& object_type_name,
				   id_d_t object_type_id,
				   const std::list<FeatureInfo>& object_type_features,
				   std::list<InstObject*>& object_list,
				   eObjectRangeType objectRangeType,
				   /* out */ emdf_ivalue& object_count);

 protected:
	// Sequence manipulation
	virtual bool getNextID(int sequence, id_d_t& out);
	virtual bool createSequenceTables();
 public:
	virtual bool setNextObjectID_DIfNotHigher(id_d_t next_id_d);
	virtual bool addStringToStringSetTable(const std::string& normalized_object_type_name,
					       const std::string& encoded_feature_name,
					       const std::string& encoded_string_to_create, // Must be encoded with escapeStringForSQL
					       /* out */ id_d_t& result);


 protected:


	virtual std::string escapeStringForSQL(const std::string& str);

 protected:
	virtual bool createObjectsOT_objects(const std::string& object_type_name, 
					     id_d_t object_type_id,
					     const InstObject *pObject,
					     const SetOfMonads& som,
					     eObjectRangeType objectRangeType,
					     const std::list<FeatureInfo>& object_type_features);
 public:
	virtual eCharsets getDefaultEncoding(void) const;
	virtual bool canDeclareEncoding(eCharsets charset) const;
 protected:
	virtual std::string getEncodingClause(eCharsets charset) const;


 private:
	// Make the assignment operator and copy constructor private
	// because they are not supported.
	SQLite3EMdFDB(const SQLite3EMdFDB&);
	SQLite3EMdFDB& operator=(const SQLite3EMdFDB&);
#endif // !defined SWIG
#endif // USE_SQLITE3
};






#endif // SQLITE3EMDFDB__H__
