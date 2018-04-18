/*
 * sqlite3emdfdb.h
 *
 * SQLite3 EMdF database base class
 *
 * Ulrik Petersen
 * Created: 1/27-2001
 * Last update: 10/3-2011
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2011  Ulrik Sandborg-Petersen
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
 *
 *   (or with modified versions of these), and to distribute linked
 *   combinations including both Emdros, in whole or in part, and one
 *   or more of the libraries normally distributed with (a)-(e) above.
 *
 *   Please note: This gives you special rights concerning the
 *   libraries which normally accompany the above pieces of software.
 *   It gives you no special rights concerning software that you write
 *   yourself.  You must obey the GNU General Public License in all
 *   respects for all of the code used other than the libraries
 *   normally distributed with (a)-(e) above.
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
				   /* out */ long& object_count);

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
