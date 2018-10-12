/*
 * mql_database_statements.h
 *
 * Definitions of AST-classes for MQL
 *
 * Ulrik Petersen
 * Created: 11/22-2002
 * Last update: 10/8-2007
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


/**@file mql_database_statements.h
 *@brief Header file for MQL DATABASE statements (MQL layer)
 */



#ifndef MQL_DATABASE_STATEMENTS__H__
#define MQL_DATABASE_STATEMENTS__H__

#include "mql_types.h"
#include "mql_object_type_statements.h"
#include "mql_execution_environment.h"


class DatabaseStatement : public Statement {
 protected:
	std::string* m_database_name;
 public:
 DatabaseStatement(MQLExecEnv *pEE, std::string* database_name) : Statement(pEE), m_database_name(database_name)  {};
	virtual ~DatabaseStatement();
	virtual bool symbol(bool& bResult);
};

class WithKeyDatabaseStatement : public DatabaseStatement {
 protected:
	std::string* m_key;
 public:
 WithKeyDatabaseStatement(MQLExecEnv *pEE, std::string* database_name, std::string* key) : DatabaseStatement(pEE, database_name), m_key(key)  {};
	virtual ~WithKeyDatabaseStatement() { delete m_key; };
};

// CREATE DATABASE
class CreateDatabaseStatement : public WithKeyDatabaseStatement {
 protected:
	std::string *m_encoding_name;
 public:
 CreateDatabaseStatement(MQLExecEnv *pEE, std::string* database_name, std::string* key, std::string *encoding_name) : WithKeyDatabaseStatement(pEE, database_name, key), m_encoding_name(encoding_name) {};
	virtual ~CreateDatabaseStatement() { delete m_encoding_name; };
	virtual bool exec();
};

// INITIALIZE DATABASE
class InitializeDatabaseStatement : public WithKeyDatabaseStatement {
 public:
 InitializeDatabaseStatement(MQLExecEnv *pEE, std::string* database_name, std::string* key) : WithKeyDatabaseStatement(pEE, database_name, key) {};
	virtual ~InitializeDatabaseStatement() {};
	virtual bool exec();
};

// USE DATABASE
class UseStatement : public WithKeyDatabaseStatement {
 public:
 UseStatement(MQLExecEnv *pEE, std::string* database_name, std::string* key) : WithKeyDatabaseStatement(pEE, database_name, key) {};
	virtual ~UseStatement() {};
	virtual bool exec();
};

// DROP DATABASE
class DropDatabaseStatement : public DatabaseStatement {
 public:
 DropDatabaseStatement(MQLExecEnv *pEE, std::string* database_name) : DatabaseStatement(pEE, database_name) {};
	virtual ~DropDatabaseStatement() {};
	virtual bool exec();
};

// VACUUM DATABASE
class VacuumDatabaseStatement : public Statement {
 private:
	bool m_bAnalyze;
 public:
 VacuumDatabaseStatement(MQLExecEnv *pEE, bool bAnalyze) : Statement(pEE), m_bAnalyze(bAnalyze) {};
	virtual ~VacuumDatabaseStatement() {};
	virtual bool exec();
};

// BEGIN TRANSACTION
class BeginTransactionStatement : public Statement {
 public:
 BeginTransactionStatement(MQLExecEnv *pEE) : Statement(pEE) {};
	virtual ~BeginTransactionStatement() {};
	virtual bool exec();
};


// ABORT TRANSACTION
class AbortTransactionStatement : public Statement {
 public:
 AbortTransactionStatement(MQLExecEnv *pEE) : Statement(pEE) {};
	virtual ~AbortTransactionStatement() {};
	virtual bool exec();
};


// COMMIT TRANSACTION
class CommitTransactionStatement : public Statement {
 public:
 CommitTransactionStatement(MQLExecEnv *pEE) : Statement(pEE) {};
	virtual ~CommitTransactionStatement() {};
	virtual bool exec();
};


// DROP INDEXES STATEMENT
class DropIndexesStatement : public ObjectTypesStatement {
 public:
 DropIndexesStatement(MQLExecEnv *pEE, const std::string& object_type_name) : ObjectTypesStatement(pEE, object_type_name) {};
	virtual ~DropIndexesStatement() {};
 protected:
	virtual bool execOneType(const ObjectTypeInfo& oti);
};


// CREATE INDEXES STATEMENT
class CreateIndexesStatement : public ObjectTypesStatement {
 public:
 CreateIndexesStatement(MQLExecEnv *pEE, const std::string& object_type_name) : ObjectTypesStatement(pEE, object_type_name) {};
	virtual ~CreateIndexesStatement() {};
 protected:
	virtual bool execOneType(const ObjectTypeInfo& oti);
};



#endif /* MQL_DATABASE_STATEMENTS__H__ */
