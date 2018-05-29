/*
 * mql_database_statements
 *
 * MQL DATABASE statements
 *
 * Ulrik Petersen
 * Created: 3/1-2001 (1st of March, 2001)
 * Last update: 5/29-2018
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

/**@file mql_database_statements.cpp
 *@brief Implementation of MQL DATABASE statements (MQL layer)
 */



#include <emdfdb.h>
#include <debug.h>
#include <mql_database_statements.h>
#include <mql_error.h>
#include <mql_execution_environment.h>


//////////////////////////////////////////////////////////////
//
// DatabaseStatement 
// base class for DATABASE statements
//
//////////////////////////////////////////////////////////////
DatabaseStatement::~DatabaseStatement()
{
  delete m_database_name;
}

bool DatabaseStatement::symbol(bool& bResult)
{
	// Ask database
	m_pEE->pDB->databaseNameIsOK(*m_database_name, bResult);

	// decide
	if (!bResult) {
		m_pEE->pError->appendError("The database name is not a valid database name.\n");
		switch (m_pEE->pDB->getBackendKind()) {
		case kPostgreSQL:
			m_pEE->pError->appendError("For PostgreSQL, the database name must be an identifier.\nSee the MQL User's Guide for what this means.\n");
			break;
		case kMySQL:
			m_pEE->pError->appendError("For MySQL, the database name must be an identifier.\nSee the MQL User's Guide for what this means.\n");
			break;
		case kSQLite3:
			; // Do nothing
			break;
		case kBPT:
			; // Do nothing
			break;
		case kBPT2:
			; // Do nothing
			break;
		default:
			ASSERT_THROW(false, "Unknown backend");
			break;
		}
	}
	return true;
}


////////////////////////////////////////////////////////////////
//
// CREATE DATABASE
//
// CreateDatabaseStatement 
//
//////////////////////////////////////////////////////////////
bool CreateDatabaseStatement::exec()
{
	eCharsets charset;
	if (m_encoding_name->empty()) {
		charset = m_pEE->pDB->getDefaultEncoding();
	} else {
		if (!string2charset(*m_encoding_name, charset)) {
			m_pEE->pError->appendError("The encoding name '" + *m_encoding_name + "' is unknown.\n");
			return false;
		}
		if (!m_pEE->pDB->canDeclareEncoding(charset)) {
			m_pEE->pError->appendError("The encoding name '" + *m_encoding_name + "' cannot be used with this backend.\n");
			return false;
		}
	}

	// Create database
	return m_pEE->pDB->createDatabase(*m_database_name, charset, *m_key);
}


////////////////////////////////////////////////////////////////
//
// INITIALIZE DATABASE
//
// InitializeDatabaseStatement 
//
//////////////////////////////////////////////////////////////
bool InitializeDatabaseStatement::exec()
{
  // Initialize database
  return m_pEE->pDB->initializeDatabase(*m_database_name, 
					*m_key);
}


////////////////////////////////////////////////////////////////
//
// USE DATABASE
//
// UseStatement 
//
//////////////////////////////////////////////////////////////

bool UseStatement::exec()
{
  // Use database
  bool bResult = m_pEE->pDB->useDatabase(*m_database_name, *m_key);

  // Return result
  return bResult;
}


////////////////////////////////////////////////////////////////
//
// DROP DATABASE
//
// DropDatabaseStatement 
//
//////////////////////////////////////////////////////////////

bool DropDatabaseStatement::exec()
{
  // Drop database
  return m_pEE->pDB->dropDatabase(*m_database_name);
}


////////////////////////////////////////////////////////////////
//
// VACUUM DATABASE
//
// VacuumDatabaseStatement 
//
//////////////////////////////////////////////////////////////

bool VacuumDatabaseStatement::exec()
{
  // Drop database
  return m_pEE->pDB->vacuum(m_bAnalyze);
}


////////////////////////////////////////////////////////////////
//
// BEGIN TRANSACTION
//
// BeginTransactionStatement
//
//////////////////////////////////////////////////////////////

bool BeginTransactionStatement::exec()
{
  // Begin transaction
  bool bResult = m_pEE->pDB->beginTransaction();

  // Create new MQLResult
  m_result = new MQLResult();
  m_result->appendHeader("transaction_started", kTCBool);

  // Start new row
  m_result->startNewRow();

  // Append result
  m_result->append(bool_alpha2string(bResult));

  // Return true; this statement cannot fail with a DB error!
  return true;
}




////////////////////////////////////////////////////////////////
//
// COMMIT TRANSACTION
//
// CommitTransactionStatement
//
//////////////////////////////////////////////////////////////

bool CommitTransactionStatement::exec()
{
  // Commit transaction
  bool bResult = m_pEE->pDB->commitTransaction();

  // Create new MQLResult
  m_result = new MQLResult();
  m_result->appendHeader("transaction_committed", kTCBool);

  // Start new row
  m_result->startNewRow();

  // Append result
  m_result->append(bool_alpha2string(bResult));

  // Return true; this statement cannot fail with a DB error!
  return true;
}




////////////////////////////////////////////////////////////////
//
// ABORT TRANSACTION
//
// AbortTransactionStatement
//
//////////////////////////////////////////////////////////////

bool AbortTransactionStatement::exec()
{
  // Abort transaction
  bool bResult = m_pEE->pDB->abortTransaction();

  // Create new MQLResult
  m_result = new MQLResult();
  m_result->appendHeader("transaction_aborted", kTCBool);

  // Start new row
  m_result->startNewRow();

  // Append result
  m_result->append(bool_alpha2string(bResult));

  // Return true; this statement cannot fail with a DB error!
  return true;
}



////////////////////////////////////////////////////////////////
//
// DROP INDEXES STATEMENT
//
// DropIndexesStatement
//
//////////////////////////////////////////////////////////////
bool DropIndexesStatement::execOneType(const ObjectTypeInfo& oti)
{
  // Get object type and id_d
  std::string object_type_name = oti.getObjectTypeName();

  // Drop indexes
  if (!m_pEE->pDB->dropIndicesOnObjectType(object_type_name)) {
    return false;
  }

  // If we got this far, there was no DB error
  return true;
}



////////////////////////////////////////////////////////////////
//
// CREATE INDEXES STATEMENT
//
// CreateIndexesStatement
//
//////////////////////////////////////////////////////////////
bool CreateIndexesStatement::execOneType(const ObjectTypeInfo& oti)
{
  // Get object type and id_d
  std::string object_type_name = oti.getObjectTypeName();

  // Create indexes
  if (!m_pEE->pDB->createIndicesOnObjectType(object_type_name)) {
    return false;
  }

  // If we got this far, there was no DB error
  return true;
}

