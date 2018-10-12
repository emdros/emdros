/*
 * mysqlemdfdb.h
 *
 * MySQL EMdF database base class
 *
 * Ulrik Petersen
 * Created: 1/27-2001
 * Last update: 5/11-2014
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


/**@file mysqlemdfdb.h
 *@brief Header file for MySQL EMdF database (EMdF layer)
 */


#ifndef MYSQLEMDFDB__H__
#define MYSQLEMDFDB__H__

#include "emdros-lconfig.h"
#ifndef SWIG
#include "emdf.h"
#include "emdfdb.h"
#include <list>
#endif
#include <string>



/** MySQL implementation of EMdFDB.
 *@ingroup EMdF
 */
class MySQLEMdFDB : public EMdFDB {
 public:
	MySQLEMdFDB(std::string host, std::string user, std::string passwd, std::string database_name);
	virtual ~MySQLEMdFDB();
#if defined(USE_MYSQL) && USE_MYSQL
#ifndef SWIG
	virtual bool createDatabase(const std::string& db_name, eCharsets charset, const std::string& key);
	/* Uses default charset. */
	virtual bool createDatabase(const std::string& db_name, const std::string& key);
	virtual bool useDatabase(const std::string& db_name, const std::string& key, bool bIsInitializingOrCreating);
	virtual bool useDatabase(const std::string& db_name, const std::string& key);

	// DB maintenance
	virtual bool vacuum(bool bAnalyze);

	// Escaping of strings
	virtual std::string escapeStringForSQL(const std::string& str);

 protected:
	virtual bool lockTablesForCreateObjects(const std::string& object_type_name, const std::list<FeatureInfo>& object_type_features);
	virtual bool unlockTablesForCreateObjects(const std::string& object_type_name);

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
 private:
	// Make the assignment operator and copy constructor private
	// because they are not supported.
	MySQLEMdFDB(const MySQLEMdFDB&);
	MySQLEMdFDB& operator=(const MySQLEMdFDB&);

	// Vacuum
	bool vacuumObjectType(const std::string& object_type_name, bool bAnalyze);
 
 public:
	virtual eCharsets getDefaultEncoding(void) const;
	virtual bool canDeclareEncoding(eCharsets charset) const;
 protected:
	virtual unsigned int getMaxNoOfJoins(void) const { return 0; }; // Disallow joins in MySQL. The reason is that it is faster.
	virtual std::string getEncodingClause(eCharsets charset) const;

	virtual bool tableExists(std::string table_name, bool& /* out */ bTableExists);

 protected:
	// Sequence manipulation
	virtual bool getNextID(int sequence, id_d_t& out);
	virtual bool createSequenceTables();
 public:
	virtual bool setNextObjectID_DIfNotHigher(id_d_t next_id_d);

#endif // !defined SWIG
#endif // USE_MYSQL
};






#endif // MYSQLEMDFDB__H__
