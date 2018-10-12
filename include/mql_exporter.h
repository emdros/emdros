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

/*
 * Copyright (C) 2001-2018     Ulrik Sandborg-Petersen
 * Copyright (C) 2018-present  Sandborg-Petersen Holding ApS, Denmark
 *
 * Licensed under the MIT License.
 *
 * Please see the file COPYING in the root of the sources for more details.
 *
 */


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
