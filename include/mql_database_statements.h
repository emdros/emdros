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
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2002-2007  Ulrik Petersen
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
