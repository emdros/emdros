/*
 * pgemdfdb.h
 *
 * PostgreSQL EMdF database base class
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


/**@file pgemdfdb.h
 *@brief Header file for PostgreSQL EMdF database (EMdF layer)
 */


#ifndef PGEMDFDB__H__
#define PGEMDFDB__H__

#include "emdros-lconfig.h"
#ifndef SWIG
#include "emdf.h"
#include "emdfdb.h"
#include <list>
#endif // !defined SWIG
#include <string>



/** PostgreSQL implementation of EMdFDB.
 *@ingroup EMdF
 */
class PgEMdFDB : public EMdFDB {
 private:
	std::string m_host;
	std::string m_user;
	std::string m_passwd;
 public:
	PgEMdFDB(std::string host, std::string user, std::string passwd, std::string database_name);
	virtual ~PgEMdFDB();
#if defined(USE_POSTGRESQL) && USE_POSTGRESQL
#ifndef SWIG
	void connect(const std::string database_name);

	/* key is ignored on PostgreSQL. */
	virtual bool createDatabase(const std::string& db_name, eCharsets charset, const std::string& key);
	/* Uses default charset. */
	virtual bool createDatabase(const std::string& db_name, const std::string& key);
	virtual bool useDatabase(const std::string& db_name, const std::string& key);
	virtual bool useDatabase(const std::string& db_name, const std::string& key, bool bIsInitializingOrCreating);


	// DB maintenance
	virtual bool vacuum(bool bAnalyze);

	// Escaping of strings
	virtual std::string escapeStringForSQL(const std::string& str);
 protected:
	std::string escapeStringForCOPY(const std::string& str);

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

 protected:
	// Utility functions
	virtual bool tableExists(std::string table_name, bool& /* out */ bTableExists);
	
 protected:
	// Sequence manipulation
	virtual bool getNextID(int sequence, id_d_t& out);
	virtual bool createSequenceTables();
 public:
	virtual bool setNextObjectID_DIfNotHigher(id_d_t next_id_d);

 public:
	virtual eCharsets getDefaultEncoding(void) const;
	virtual bool canDeclareEncoding(eCharsets charset) const;
 protected:
	virtual std::string getEncodingClause(eCharsets charset) const;

 private:
	// Make the assignment operator and copy constructor private
	// because they are not supported.
	PgEMdFDB(const PgEMdFDB&);
	PgEMdFDB& operator=(const PgEMdFDB&);
#endif // !defined SWIG
#endif // USE_POSTGRESQL
};






#endif // PGEMDFDB__H__
