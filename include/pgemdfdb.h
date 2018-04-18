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
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2014  Ulrik Sandborg-Petersen
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
