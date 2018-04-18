/*
 * mql_exporter.h
 *
 * A class to dump Emdros databases to MQL
 *
 * Ulrik Petersen
 * Created: 12/16-2005
 * Last update: 10/9-2007
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2005-2007  Ulrik Petersen
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

#ifndef MQL_EXPORTER__H__
#define MQL_EXPORTER__H__


#include "emdros-config.h"
// Do this because emdros-config might #define malloc,
// in which case some systems will fail to use the real 
// malloc, thus causing build errors.
#undef malloc

#include "emdros-lconfig.h"
#include <iostream>
#include <string>
#include "environment_emdros.h"
#include <list>
#include "infos.h"


#define DEFAULT_BATCH_SIZE (50000)
#define START_MONAD_NO_VALUE (MAX_MONAD)
#define END_MONAD_NO_VALUE (0)

typedef struct MQLExportEnv {
public:
	std::string filename;
	std::string password;
	std::string hostname;
	std::string user;
	std::string encoding;
	bool bCreateAndUseDB;
	bool bDumpEncoding;
	bool bCreateObjectTypes;
	bool bCreateEnums;
	bool bCreateArbitraryMonadSets;
	bool bCreateObjectData;
	bool bUseTransactions;
	bool bVerbose;
	bool bBatchCreateObjects;
	long nBatchSize;
	std::string db;
	std::list<std::string> object_types;
	monad_m m_start;
	monad_m m_end;
	MQLExportEnv() {
		hostname = "localhost";
		user = "emdf";
		encoding = "";
		bCreateAndUseDB = true;
		bDumpEncoding = false;
		bCreateObjectTypes = true;
		bCreateEnums = true;
		bCreateArbitraryMonadSets = true;
		bCreateObjectData = true;
		bVerbose = false;
		bBatchCreateObjects = false;
		nBatchSize = DEFAULT_BATCH_SIZE;
		m_start = START_MONAD_NO_VALUE;
		m_end = END_MONAD_NO_VALUE;
		password = "";
	};
	~MQLExportEnv() {};
} MQLExportEnv;


class MQLExporter {
protected:
	MQLExportEnv m_de;
	EmdrosEnv *m_pEE;
	std::ostream *m_ostr;
	std::string m_filename;
public:
	MQLExporter(const MQLExportEnv& de, EmdrosEnv *pEE, std::ostream* pOutstr); // Dump to outstr
	MQLExporter(const MQLExportEnv& de, EmdrosEnv *pEE, std::string filename); // Dump to file
	MQLExporter(const MQLExportEnv& de, EmdrosEnv *pEE); // Dump to stdout
	~MQLExporter();
	/**
	 *
	 * @param message an error message if return value is non-zero.
	 *
	 * @return
	 * 
	 * 0 : Success
	 * 1 : Wrong usage
	 * 2 : Connection to backend server could not be established
	 * 3 : An exception occurred
	 * 4 : Could not open file
	 * 5 : Database error
	 * 6 : Compiler error
	 */
	 int doIt(std::string& message);
protected:
	bool ExecuteQuery(std::string query, bool& bCompilerResult); 
	bool GetObjectTypeId(std::string object_type_name, id_d_t& object_type_id, eObjectRangeType& objectRangeType, eMonadUniquenessType& monadUniquenessType);
	bool GetEnumConst(long value, const std::string& enum_name, std::string& outstr);
	bool getObjectTypes(bool& bCompilerResult);
	void DumpCreateAndUseDB();
	bool DumpArbitraryMonadSet(const std::string& monad_set_name, const SetOfMonads& som, bool& bCompilerResult);
	bool DumpArbitraryMonadSets(bool& bCompilerResult); 
	bool DumpEnum(std::string enum_name, bool& bCompilerResult); 
	bool DumpEnums(bool& bCompilerResult); 
	bool DumpObjectType(std::string object_type_name, bool& bCompilerResult); 
	bool DumpObjectTypes(bool& bCompilerResult); 
	void DumpBeginTransaction();
	void DumpCommitTransaction();
	bool DumpObjectDataSingleObject(std::string object_type_name, const std::list<FeatureInfo>& FeatureInfos, const InstObject *pObject, bool& bCompilerResult);
	void DumpDropIndexesOnObjectType(const std::string& object_type_name);
	void DumpCreateIndexesOnObjectType(const std::string& object_type_name);
	void DumpBatchCreateHeader(const std::string& object_type_name);
	void DumpCommitBeginTransaction(bool& bTransactionInProgress);
	bool DumpObjectDataForObjectType(std::string object_type_name, bool& bCompilerResult);
	bool DumpObjectData(bool& bCompilerResult); 
	void VacuumDatabase();
	bool do_dump(bool& bCompilerResult); 
};


#endif /* MQL_ERROR__H__ */
