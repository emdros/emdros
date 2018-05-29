/*
 * emdros_environment.cpp
 *
 * Emdros environment
 *
 * Ulrik Petersen
 * Created: 3/14-2003
 * Last update: 5/29-2018
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2003-2018  Ulrik Sandborg-Petersen
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

/**@file emdros_environment.cpp
 *@brief Implementation of EmdrosEnv (MQL layer)
 */


#include <emdros-lconfig.h>
#include <emdf_output.h>
#include <mql_result.h>
#include <mql_types.h>
#include <mql_error.h>
#include <mql_execution_environment.h>
#include <environment_emdros.h>
#include <emdf_output.h>
#include <mql_execute.h>
#include <mql_sheaf.h>
#include <string>
#include <iostream>
#include <debug.h>

#if USE_POSTGRESQL
#include <pgemdfdb.h>
#endif
#if USE_MYSQL
#include <mysqlemdfdb.h>
#endif
#if USE_SQLITE3
#include <sqlite3emdfdb.h>
#endif
#include <bptemdfdb.h>
#include <bpt2emdfdb.h>

// Only used by constructors
/** Initialize EmdrosEnv
 *\internal
 *
 * This is used by the constructors to initialize.
 *
 * @param output_stream The std::ostream on which to print output.  
 E.g., std::cout.
 * @param output_kind The kind of output 
 * @param charset The character set to use 
 * @param hostname The hostname to connect to (e.g., "localhost"; 
 *        ignored for SQLite and SQLite3)
 * @param user The database username to connect as (e.g., "emdf"; 
 *        ignored for SQLite and SQLite3)
 * @param password The database password to connect with (e.g.,
 * "changeme").  On SQLite and SQLite3, the password is ignored, 
 * except if SQLite/SQLite3 has been enhanced with encryption-facilities 
 * (e.g., those of Dr. Hipp), in which case it is passed raw to 
 * sqlite_open_encrypted as the key, with the string length as nKey.
 * @param initial_db The initial database to connect to (e.g., "emdf")
 */
void EmdrosEnv::init(std::ostream* output_stream, 
		     eOutputKind output_kind, eCharsets charset, 
		     std::string hostname, 
		     std::string user, std::string password, 
		     std::string initial_db,
		     eBackendKind backend_kind)
{
	EMdFDB *pDB = 0;
	if (backend_kind == kPostgreSQL) {
#if USE_POSTGRESQL
		pDB = new PgEMdFDB(hostname, user, password, initial_db);
#else
		throw EmdrosException(std::string("PostgreSQL support not compiled in. Cannot make a PostgreSQL connection."));
#endif
	} else if (backend_kind == kMySQL) {
#if USE_MYSQL
		pDB = new MySQLEMdFDB(hostname, user, password, initial_db);
#else
		throw EmdrosException(std::string("MySQL support not compiled in. Cannot make a MySQL connection."));
#endif
	} else if (backend_kind == kSQLite3) {
#if USE_SQLITE3
		pDB = new SQLite3EMdFDB(initial_db, password);
#else
		throw EmdrosException(std::string("SQLite 3.X.X support not compiled in. Cannot make a SQLite 3.X.X connection."));
#endif
	} else if (backend_kind == kBPT) {
		pDB = new BPTEMdFDB(initial_db, password);
	} else if (backend_kind == kBPT2) {
		pDB = new BPT2EMdFDB(initial_db, password);
	} else {
		ASSERT_THROW(false, "Unknown backend");
	}

	EMdFOutput *pOut = new EMdFOutput(charset, output_stream, output_kind, 3);
  
	m_pEE = new MQLExecEnv(pDB, pOut);
}

/**The full constructor.
 *
 * Use this if you want to have full control.  With this
 * constructor, you can specify even the output stream to use when
 * printing output from the MQL engine.
 *
 * @param output_stream The std::ostream on which to print output.
 * E.g., std::cout.
 *
 * @param output_kind The kind of output 
 *
 * @param charset The character set to use 
 *
 * @param hostname The hostname to connect to (e.g., "localhost"; 
 *        ignored for SQLite)
 *
 * @param user The database username to connect as (e.g., "emdf"; 
 *        ignored for SQLite)
 *
 * @param password The database password to connect with (e.g.,
 * "changeme").  On SQLite, the password is ignored, except if SQLite
 * has been enhanced with encryption-facilities (e.g., those of
 * Dr. Hipp, the author of SQLite), in which case it is passed raw to
 * sqlite_open_encrypted as the key, with the string length as nKey.
 * @param initial_db The initial database to connect to (e.g., "emdf")
 */
EmdrosEnv::EmdrosEnv(std::ostream* output_stream, 
		     eOutputKind output_kind, eCharsets charset, 
		     std::string hostname, 
		     std::string user, std::string password, 
		     std::string initial_db,
		     eBackendKind backend)
{
	init(output_stream, output_kind, charset, 
	     hostname, user, password, initial_db, backend);
}


/**The easier constructor.
 *
 * The constructor of EmdrosEnv which is easier to use, especially
 * from SWIG interface.  Uses std::cout as the output stream.
 *
 * @param output_kind The kind of output 
 *
 * @param charset The character set to use 
 *
 * @param hostname The hostname to connect to (e.g., "localhost"; 
 *        ignored for SQLite)
 *
 * @param user The database username to connect as (e.g., "emdf"; 
 *        ignored for SQLite)
 *
 * @param password The database password to connect with (e.g.,
 * "changeme").  On SQLite, the password is ignored, except if SQLite
 * has been enhanced with encryption-facilities (e.g., those of
 * Dr. Hipp, the author of SQLite), in which case it is passed raw to
 * sqlite_open_encrypted as the key, with the string length as nKey.
 *
 * @param initial_db The initial database to connect to (e.g., "emdf")
 */
EmdrosEnv::EmdrosEnv(eOutputKind output_kind, eCharsets charset, 
		     std::string hostname, 
		     std::string user, std::string password, 
		     std::string initial_db,
		     eBackendKind backend) 
{
	init(&std::cout, output_kind, charset, 
	     hostname, user, password, initial_db, backend);
}


/** The destructor
 *
 */
EmdrosEnv::~EmdrosEnv()
{
	delete m_pEE->pOut;
	delete m_pEE->pDB;
	delete m_pEE;
}



/** Execute MQL statement(s) from a string
 *
 * Execute the MQL statement(s) in parameter input. Do not use an
 * MQLResultCallback.
 *
 * @param input The MQL statement(s) to execute.
 *
 * @param bResult Used for returning compiler success (true) or
 * failure (false).
 *
 * @param bPrintResult True means that result should be printed on the
 * output stream (see EmdrosEnv::EmdrosEnv()). False means that no
 * output is printed.
 *
 * @param bReportError True means that errors should be reported on
 * the output stream (see EmdrosEnv::EmdrosEnv()). False means that
 * errors are not printed.
 *
 * @return True on no database error, false if a database error occurred.
 */
bool EmdrosEnv::executeString(const std::string& input, 
			      bool& bResult, 
			      bool bPrintResult, 
			      bool bReportError)
{
	return mqlExecuteString(m_pEE, input, bResult, bPrintResult, bReportError, 0);
}


/** Execute MQL statement(s) from a string
 *
 * Execute the MQL statement(s) in parameter input.
 *
 * @param input The MQL statement(s) to execute.
 *
 * @param bResult Used for returning compiler success (true) or
 * failure (false).
 *
 * @param bPrintResult True means that result should be printed on the
 * output stream (see EmdrosEnv::EmdrosEnv()). False means that no
 * output is printed.
 *
 * @param bReportError True means that errors should be reported on
 * the output stream (see EmdrosEnv::EmdrosEnv()). False means that
 * errors are not printed.
 *
 * @param pCallback If nil (0), don't use a callback. If non-nil, use
 * the callback when processing MQL results.  This is useful, e.g., if
 * you want to consume the output one section at a time, instead of
 * storing it all in-memory and then consuming it after this function
 * returns.
 *
 * @return True on no database error, false if a database error occurred.
 */
bool EmdrosEnv::executeString(const std::string& input, 
			      bool& bResult, 
			      bool bPrintResult, 
			      bool bReportError,
			      MQLResultCallback *pCallback)
{
	return mqlExecuteString(m_pEE, input, bResult, bPrintResult, bReportError, pCallback);
}


/** Execute MQL statement(s) from a file
 *
 * Execute the MQL statement(s) found in the file with the given filename.
 *
 * @param filename Name of the file containing the MQL statement(s) 
 *        to execute.
 * @param bResult Used for returning compiler success (true) or 
 *        failure (false).
 * @param bPrintResult True means that result should be printed on 
 *        the output stream (see EmdrosEnv::EmdrosEnv()). False means 
 *        that no output is printed.
 * @param bReportError True means that errors should be reported on 
 *        the output stream (see EmdrosEnv::EmdrosEnv()). False means 
 *        that errors are not printed.
 * @param pCallback If nil (0), don't use a callback. If non-nil, use
 * the callback when processing MQL results.  This is useful, e.g., if
 * you want to consume the output one section at a time, instead of
 * storing it all in-memory and then consuming it after this function
 * returns.
 *
 * @return True on no database error, false if a database error occurred.
 */
bool EmdrosEnv::executeFile(std::string filename, 
			    bool& bResult, 
			    bool bPrintResult, 
			    bool bReportError,
			    MQLResultCallback *pCallback)
{
	return mqlExecuteFile(m_pEE, filename, bResult, bPrintResult, bReportError, pCallback);
}

/** Execute MQL statement(s) from a stream
 *
 * Execute the MQL statement(s) which can be read from the std::istream
 * given.
 *
 * @param strin The std::istream to process MQL statement(s) from. 
 * @param bResult Used for returning compiler success (true) or 
 *        failure (false).
 * @param bPrintResult True means that result should be printed on 
 *        the output stream (see EmdrosEnv::EmdrosEnv()). False means 
 *        that no output is printed.
 * @param bReportError True means that errors should be reported on 
 *        the output stream (see EmdrosEnv::EmdrosEnv()). False means 
 *        that errors are not printed.
 * @param pCallback If nil (0), don't use a callback. If non-nil, use
 * the callback when processing MQL results.  This is useful, e.g., if
 * you want to consume the output one section at a time, instead of
 * storing it all in-memory and then consuming it after this function
 * returns.
 * @return True on no database error, false if a database error occurred.
 */
bool EmdrosEnv::executeStream(std::istream& strin, 
			      bool& bResult, 
			      bool bPrintResult, 
			      bool bReportError,
			      MQLResultCallback *pCallback)
{
	return mqlExecuteStream(m_pEE, strin, bResult, bPrintResult, bReportError, pCallback);
}


/** Check if connection to DB backend is OK.
 *
 * Just calls EMdFDB::connectionOk() (or its polymorphic instance).
 *
 * @see EMdFDB::connectionOk()
 * 
 * @return True if connection is alive, false if not.
 */
bool EmdrosEnv::connectionOk(void)
{
	return m_pEE->pDB->connectionOk();
}

/** Vacuum the database.
 *
 * Just calls EMdFDB::vacuum() (or its polymorphic instance).
 *
 * @see EMdFDB::vacuum()
 *
 * @param bAnalyze Must be true if the user wants the ANALYZE option
 * to occur.  Exactly what the ANALYZE option does is dependent on the
 * implementation in the subclass.
 *
 * @return True on no database error, false if a database error occurred.
 */
bool EmdrosEnv::vacuum(bool bAnalyze)
{
	return m_pEE->pDB->vacuum(bAnalyze);
}


/** Gets the min_m monad of the database.
 *
 * The min_m monad is the least monad in the database.  
 * Defaults to MAX_MONAD if there are no objects in the database.
 *
 * Just calls EMdFDB::getMin_m() (or its polymorphic instance).
 *
 * @see EMdFDB::getMin_m()
 * 
 * @param min_m Used for returning the min_m monad.
 * @return True on no database error, false if a database error occurred.
 */
bool EmdrosEnv::getMin_m(monad_m& /* out */ min_m)
{
	return m_pEE->pDB->getMin_m(min_m);
}


/** Gets the max_m monad of the database.
 *
 * The max_m monad is the least monad in the database.  
 * Defaults to 0 if there are no objects in the database.
 *
 * Just calls EMdFDB::getMax_m() (or its polymorphic instance).
 *
 * @see EMdFDB::getMax_m()
 * 
 * @param max_m Used for returning the max_m monad.
 * @return True on no database error, false if a database error occurred.
 */
bool EmdrosEnv::getMax_m(monad_m& /* out */ max_m)
{
	return m_pEE->pDB->getMax_m(max_m);
}


/** Gets the all_m-1 object of the database.
 *
 * The all_m-1 object is the unique set of monads that
 * contains all monads between min_m and max_m.
 *
 * Just calls EMdFDB::getAll_m_1() (or its polymorphic instance).
 *
 * @see EMdFDB::getMin_m(), EMdFDB::getMax_m(), 
 *      EmdrosEnv::getMin_m(), EmdrosEnv::getMax_m().
 * 
 * @param all_m_1 Used for returning the all_m-1 object.
 * @return True on no database error, false if a database error occurred.
 */
bool EmdrosEnv::getAll_m_1(SetOfMonads& /* out */ all_m_1)
{
	return m_pEE->pDB->getAll_m_1(all_m_1);
}


// Transactions
/** Begin a transaction if possible.
 *
 * Begins a transaction if possible.  It is possible only if the backend
 * supports transactions, and no transaction is in progress in the current
 * EMdFDB instance (i.e., a descendant: PgEMdFBD, MySQLEMdFDB, or 
 * SQLiteEMdFDB).
 *
 * Just calls EMdFDB::beginTransaction() (or its polymorphic instance).
 *
 * @see EMdFDB::beginTransaction().
 * 
 * @return True on transaction begun, false on transaction not started.
 */
bool EmdrosEnv::beginTransaction(void)
{
	return m_pEE->pDB->beginTransaction();
}


/** Commit a transaction.
 *
 * Attempts to commit a transaction.  Should only be called if
 * beginTransaction previously returned true.
 *
 * Just calls EMdFDB::commitTransaction() (or its polymorphic instance).
 *
 * @see EMdFDB::commitTransaction().
 * 
 * @return True on no database error, false if a database error occurred.
 */
bool EmdrosEnv::commitTransaction(void)
{
	return m_pEE->pDB->commitTransaction();
}


/** Abort a transaction.
 *
 * Attempts to abort a transaction.  Should only be called if
 * beginTransaction previously returned true.
 *
 * Just calls EMdFDB::abortTransaction() (or its polymorphic instance).
 *
 * @see EMdFDB::abortTransaction().
 * 
 * @return True on no database error, false if a database error occurred.
 */
bool EmdrosEnv::abortTransaction(void)
{
	return m_pEE->pDB->abortTransaction();
}


/** Get result from statement.
 *
 * Gets the MQLResult from the Statement in the MQLExecEnv.  Return
 * nil (0) if there is no statement.  Calls Statement::getResult().
 * The result is left in the care of the Statement: You should NOT
 * delete the result yourself.  If you wish to take over
 * responsibility for the Sheaf, use EmdrosEnv::takeOverResult().
 *
 * @see EmdrosEnv::takeOverResult(), Statement::getResult().
 *
 * @return a pointer to the MQLResult inside the current statement.
 *
 */
MQLResult* EmdrosEnv::getResult(void)
{
	Statement *pStatement = m_pEE->pStatement;
	if (pStatement == 0) {
		return 0;
	} else {
		return pStatement->getResult();
	}
}

/** Get sheaf from result.
 *
 * Gets the MQLResult from the Statement in the MQLExecEnv (using
 * EmdrosEnv::getResult().  Returns nil (0) if there is no result.
 * Calls MQLResult::getSheaf().  The sheaf is left in the care of the
 * MQLResult: You should NOT delete the sheaf yourself.  If you wish
 * to take over responsibility for the Sheaf, use
 * EmdrosEnv::takeOverSheaf().
 *
 * @see EmdrosEnv::takeOverSheaf(), EmdrosEnv::getResult(), 
 *      MQLResult::getSheaf().
 *
 * @return a pointer to the Sheaf inside the current MQLResult.
 *
 */
Sheaf* EmdrosEnv::getSheaf(void)
{
	MQLResult *pResult = getResult();
	if (pResult == 0) {
		return 0;
	} else {
		return pResult->getSheaf();
	}
}

/** Get flat sheaf from result.
 *
 * Gets the MQLResult from the Statement in the MQLExecEnv (using
 * EmdrosEnv::getResult().  Returns nil (0) if there is no result.
 * Calls MQLResult::getFlatSheaf().  The sheaf is left in the care of
 * the MQLResult: You should NOT delete the sheaf yourself.  If you
 * wish to take over responsibility for the Sheaf, use
 * EmdrosEnv::takeOverFlatSheaf().
 *
 * @see EmdrosEnv::takeOverFlatSheaf(), EmdrosEnv::getResult(),
 * MQLResult::getFlatSheaf().
 *
 * @return a pointer to the FlatSheaf inside the current MQLResult,
 * nil (0) if not found.
 *
 */
FlatSheaf* EmdrosEnv::getFlatSheaf(void)
{
	MQLResult *pResult = getResult();
	if (pResult == 0) {
		return 0;
	} else {
		return pResult->getFlatSheaf();
	}
}

/** Get table from result.
 *
 * Gets the Table from the Statement in the MQLExecEnv (using
 * EmdrosEnv::getResult().  Returns nil (0) if there is no result.
 * The Table is left in the care of the Statement: You should NOT
 * delete the table yourself.  If you wish to take over responsibility
 * for the Table, use EmdrosEnv::takeOverTable().
 *
 * @see EmdrosEnv::takeOverTable(), EmdrosEnv::getResult(), MQLResult.
 *
 * @return a pointer to the Table which is the current MQLResult.
 */
Table* EmdrosEnv::getTable(void)
{
	MQLResult *pResult = getResult();
	if (pResult == 0) {
		return 0;
	} else {
		return (Table*) pResult;
	}
}


/** Get current statement
 * 
 * Get the current statement from the MQLExecEnv.  The Statement is
 * left in the care of the MQLExecEnv: You should NOT delete it
 * yourself!  If you wish to take over the statement, use
 * EmdrosEnv::takeOverStatement().
 *
 * @see EmdrosEnv::takeOverStatement().
 *
 * @return The current statement.
 */
Statement *EmdrosEnv::getStatement(void)
{
	return m_pEE->pStatement;
}


/** Get current statement and take over responsibility for it.
 * 
 * Get the current statement from the MQLExecEnv.  The Statement is
 * now YOUR responsibility: YOU must delete it when you are done with
 * it.  If you wish to leave it in the care of the MQLExecEnv, use
 * EmdrosEnv::getStatement().
 *
 * @see EmdrosEnv::getStatement().
 *
 * @return The Statement to take over.
 */
Statement *EmdrosEnv::takeOverStatement(void)
{
	Statement *pStatement = m_pEE->pStatement;
	m_pEE->pStatement = 0;
	return pStatement;
}

/** Get current result and take over responsibility for it.
 * 
 * Calls EmdrosEnv::takeOverStatement().  Gets the MQLResult from this
 * statement, and deletes the Statement.  Note: The MQLResult is now
 * YOUR responsibility: YOU must delete it when you are done with it.
 * If you wish to leave it in the care of the Statement, use
 * EmdrosEnv::getResult().
 *
 * @see EmdrosEnv::getResult().
 *
 * @return The MQLResult to take over, or 0 if there is no statement.
 */
MQLResult* EmdrosEnv::takeOverResult(void)
{
	Statement *pStatement = takeOverStatement();
	if (!pStatement) {
		return 0;
	} else {
		MQLResult *pResult = pStatement->getResult();
		pStatement->setDeleteResult(false);
		delete pStatement;
		return pResult;
	}
}

/** Get current sheaf and take over responsibility for it.
 * 
 * Calls EmdrosEnv::takeOverResult().  If the result is not a sheaf,
 * delete the result and return nil (0).  Otherwise, if it is a sheaf,
 * get the sheaf from the MQLResult and delete the MQLResult.  Note:
 * The Sheaf is now YOUR responsibility: YOU must delete it when you
 * are done with it.  If you wish to leave it in the care of the
 * MQLResult, use EmdrosEnv::getSheaf().
 *
 * @see EmdrosEnv::getSheaf(), EmdrosEnv::takeOverResult(),
 * MQLResult::isSheaf(), MQLResult::getSheaf().
 *
 * @return The sheaf to be taken over, or 0 if result is not a sheaf
 * or if there is no result.
 */
Sheaf* EmdrosEnv::takeOverSheaf(void)
{
	MQLResult *pMQLResult = takeOverResult();
	if (pMQLResult == 0) {
		return 0;
	} else {
		Sheaf *pResult;
		if (!pMQLResult->isSheaf()) {
			pResult = 0;
			delete pMQLResult;
		} else {
			pResult = pMQLResult->getSheaf();
			pMQLResult->setDeleteFullSheaf(false);
			delete pMQLResult;
		}
		return pResult;
	}
}

/** Get current flat sheaf and take over responsibility for it.
 * 
 * Calls EmdrosEnv::takeOverResult().  If the result is not a flat
 * sheaf, delete the result and return nil (0).  Otherwise, if it is a
 * flat sheaf, get the flat sheaf from the MQLResult and delete the
 * MQLResult.  Note: The flat sheaf is now YOUR responsibility: YOU
 * must delete it when you are done with it.  If you wish to leave it
 * in the care of the MQLResult, use EmdrosEnv::getFlatSheaf().
 *
 * @see EmdrosEnv::getFlatSheaf(), EmdrosEnv::takeOverResult(),
 * MQLResult::isFlatSheaf(), MQLResult::getFlatSheaf().
 *
 * @return The flat sheaf to be taken over, or 0 if result is not a
 * flat sheaf or if there is no result.
 */
FlatSheaf* EmdrosEnv::takeOverFlatSheaf(void)
{
	MQLResult *pMQLResult = takeOverResult();
	if (pMQLResult == 0) {
		return 0;
	} else {
		FlatSheaf *pResult;
		if (!pMQLResult->isFlatSheaf()) {
			pResult = 0;
			delete pMQLResult;
		} else {
			pResult = pMQLResult->getFlatSheaf();
			pMQLResult->setDeleteFlatSheaf(false);
			delete pMQLResult;
		}
		return pResult;
	}
}

/** Get current table and take over responsibility for it.
 * 
 * Calls EmdrosEnv::takeOverResult().  If the result is not a table,
 * delete the result and return nil (0).  Otherwise, if it is a table,
 * get the table from the MQLResult (i.e., downcase it to Table).
 * Note: The Table is now YOUR responsibility: YOU must delete it when
 * you are done with it.  If you wish to leave it in the care of the
 * MQLResult, use EmdrosEnv::getTable().
 *
 * @see EmdrosEnv::getTable(), EmdrosEnv::takeOverResult(),
 * MQLResult::isTable(), MQLResult::getTable().
 *
 * @return The table to be taken over, or 0 if result is not a table
 * or if there is no result.
 */
Table* EmdrosEnv::takeOverTable(void)
{
	MQLResult *pMQLResult = takeOverResult();
	if (pMQLResult == 0) {
		return 0;
	} else {
		Table *pResult;
		if (!pMQLResult->isTable()) {
			pResult = 0;
			delete pMQLResult;
		} else {
			pResult = pMQLResult->getTable();
		}
		return pResult;
	}
}



// Getting error-messages and info
/** Get the current database error message.
 *
 * Calls EMdFDB::errorMessage() on the current database.
 *
 * @see EMdFDB::errorMessage().
 *
 * @return The current database error message (if any).
 */
std::string EmdrosEnv::getDBError(void)
{
	return m_pEE->pDB->errorMessage();
}


/** Get the current compile error message.
 *
 * Calls MQLError::getError().
 *
 * @see MQLError:getError().
 *
 * @return The current compiler error message (if any).
 */
std::string EmdrosEnv::getCompilerError(void)
{
	return m_pEE->pError->getError();
}


/** Get the compiler stage that was executed last.
 *
 * @return The compiler stage last executed (see \ref CompilerStages).
 */
int EmdrosEnv::getLastCompilerStage(void)
{
	return m_pEE->nCompilerStage;
}

/** Clear all error messages.
 *
 * Clears all compiler error messages and database error messages.
 */
void EmdrosEnv::clearErrors(void)
{
	m_pEE->pDB->clearLocalError();
	m_pEE->pError->clearError();
}


// Outputting
// These all output on the stream in local EMdFOutput,
// i.e., the stream you passed to the constructor of EmdrosEnv,
// or stdout if the other constructor was used
/** Output a string.
 *
 * Outputs a string on the stream in the local EMdFOutput.  That is,
 * the on the stream you passed to the EmdrosEnv::EmdrosEnv
 * constructor (which defaults to std::cout for the second
 * constructor).
 *
 * @param str The string to output.
 */
// out your own string
void EmdrosEnv::out(std::string str)
{
	m_pEE->pOut->out(str);
}


/** Output an MQLResult.
 *
 * Outputs an MQLResult on the stream in the local EMdFOutput.  That
 * is, the on the stream you passed to the EmdrosEnv::EmdrosEnv
 * constructor (which defaults to std::cout for the second
 * constructor).
 *
 * This will be done with the eOutputKind and eCharsets values from
 * the EmdrosEnv::EmdrosEnv constructor.
 *
 * @see MQLResult::out()
 *
 * @param pResult The MQLResult to output (sheaf or table).
 */
void EmdrosEnv::out(MQLResult *pResult)
{
	pResult->out(m_pEE->pOut);
}


/** Output a sheaf.
 *
 * Outputs a Sheaf on the stream in the local EMdFOutput.  That
 * is, the on the stream you passed to the EmdrosEnv::EmdrosEnv
 * constructor (which defaults to std::cout for the second
 * constructor).
 *
 * This will be done with the eOutputKind and eCharsets values from
 * the EmdrosEnv::EmdrosEnv constructor.
 *
 * @see Sheaf::printConsole(), Sheaf::printXML().
 *
 * @param pSheaf The Sheaf to output.
 */
void EmdrosEnv::out(Sheaf *pSheaf)
{
	switch (m_pEE->pOut->getOutputKind()) {
	case kOKXML:
	case kOKCompactXML:
		pSheaf->printXML(m_pEE->pOut);
		break;

	case kOKJSON:
	case kOKCompactJSON:
		pSheaf->printJSON(m_pEE->pOut);
		break;

	case kOKConsole:
		pSheaf->printConsole(m_pEE->pOut);
		break;

	default:
		ASSERT_THROW(false, "Unknown eOutputKind");
		break;
	}
}


/** Output a flat sheaf.
 *
 * Outputs a FlatSheaf on the stream in the local EMdFOutput.  That
 * is, the on the stream you passed to the EmdrosEnv::EmdrosEnv
 * constructor (which defaults to std::cout for the second
 * constructor).
 *
 * This will be done with the eOutputKind and eCharsets values from
 * the EmdrosEnv::EmdrosEnv constructor.
 *
 * @see FlatSheaf::printConsole(), FlatSheaf::printXML().
 *
 * @param pFlatSheaf The FlatSheaf to output.
 */
void EmdrosEnv::out(FlatSheaf *pFlatSheaf)
{
	switch (m_pEE->pOut->getOutputKind()) {
	case kOKXML:
	case kOKCompactXML:
		pFlatSheaf->printXML(m_pEE->pOut);
		break;

	case kOKJSON:
	case kOKCompactJSON:
		pFlatSheaf->printJSON(m_pEE->pOut);
		break;

	case kOKConsole:
		pFlatSheaf->printConsole(m_pEE->pOut);
		break;

	default:
		ASSERT_THROW(false, "Unknown eOutputKind");
		break;
	}
}


/** Output a table
 *
 * Outputs a Table on the stream in the local EMdFOutput.  That
 * is, the on the stream you passed to the EmdrosEnv::EmdrosEnv
 * constructor (which defaults to std::cout for the second
 * constructor).
 *
 * This will be done with the eOutputKind and eCharsets values from
 * the EmdrosEnv::EmdrosEnv constructor.
 *
 * @see Table::printConsole(), Table::printXML().
 *
 * @param pTable The Table to output.
 */
void EmdrosEnv::out(Table *pTable)
{
	switch (m_pEE->pOut->getOutputKind()) {
	case kOKXML:
	case kOKCompactXML:
		pTable->printXML(m_pEE->pOut);
		break;

	case kOKJSON:
	case kOKCompactJSON:
		pTable->printJSON(m_pEE->pOut);
		break;

	case kOKConsole:
		pTable->printConsole(m_pEE->pOut);
		break;

	default:
		ASSERT_THROW(false, "Unknown eOutputKind");
		break;
	}
}


/** Output a set of monads
 *
 * Outputs a SetOfMonads on the stream in the local EMdFOutput.  That
 * is, the on the stream you passed to the EmdrosEnv::EmdrosEnv
 * constructor (which defaults to std::cout for the second
 * constructor).
 *
 * This will be done with the eOutputKind and eCharsets values from
 * the EmdrosEnv::EmdrosEnv constructor.
 *
 * @see SetOfMonads::printConsole(), SetOfMonads::printXML().
 *
 * @param pMonads The SetOfMonads to output.
 */
void EmdrosEnv::out(SetOfMonads *pMonads)
{
	switch (m_pEE->pOut->getOutputKind()) {
	case kOKXML:
	case kOKCompactXML:
		pMonads->printXML(m_pEE->pOut);
		break;

	case kOKJSON:
	case kOKCompactJSON:
		pMonads->printJSON(m_pEE->pOut);
		break;

	case kOKConsole:
		pMonads->printConsole(m_pEE->pOut);
		break;

	default:
		ASSERT_THROW(false, "Unknown eOutputKind");
		break;
	}
}



//
// XML outputting
//

// XML declaration.
// Calls EMdFOutput::printXMLDecl on the local EMdFOutput
/** Output the XML declaration
 *
 * Outputs the XML declaration on the stream in the local EMdFOutput.
 * That is, the on the stream you passed to the EmdrosEnv::EmdrosEnv
 * constructor (which defaults to std::cout for the second
 * constructor).
 *
 * This will be done with the eCharsets value from the
 * EmdrosEnv::EmdrosEnv constructor.
 *
 * Calls EMdFOutput::printXMLDecl on the local EMdFOutput
 *
 * @see EMdFOutput::printXMLDecl()
 */
void EmdrosEnv::printXMLDecl()
{
	m_pEE->pOut->printXMLDecl();
}

 

// DTDs
/** Output the start of the DTD.
 *
 * Outputs the start of the DTD on the stream in the local EMdFOutput.
 * That is, the on the stream you passed to the EmdrosEnv::EmdrosEnv
 * constructor (which defaults to std::cout for the second
 * constructor).
 *
 * Calls EMdFOutput::printDTDStart on the local EMdFOutput.
 *
 * @see EMdFOutput::printDTDStart()
 *
 * @param root_element The root element of the DTD.
 */
void EmdrosEnv::printDTDStart(std::string root_element)
{
	m_pEE->pOut->printDTDstart(root_element.c_str());
}


/** Output the end of the DTD.
 *
 * Outputs the end of the DTD on the stream in the local EMdFOutput.
 * That is, the on the stream you passed to the EmdrosEnv::EmdrosEnv
 * constructor (which defaults to std::cout for the second
 * constructor).
 *
 * Calls EMdFOutput::printDTDEnd on the local EMdFOutput.
 *
 * @see EMdFOutput::printDTDEnd()
 */
void EmdrosEnv::printDTDEnd()
{
	m_pEE->pOut->printDTDend();
}


/** Output the DTD of MQLResult.
 *
 * Outputs the DTD of MQLResult on the stream in the local EMdFOutput.
 * That is, the on the stream you passed to the EmdrosEnv::EmdrosEnv
 * constructor (which defaults to std::cout for the second
 * constructor).
 *
 * Calls MQLResult::printDTD on the local EMdFOutput.
 *
 * @see MQLResult::printDTD().
 */
void EmdrosEnv::printDTDMQLResult(void)
{
	MQLResult::printDTD(m_pEE->pOut);
}


/** Output the DTD of Table.
 *
 * Outputs the DTD of Table on the stream in the local EMdFOutput.
 * That is, the on the stream you passed to the EmdrosEnv::EmdrosEnv
 * constructor (which defaults to std::cout for the second
 * constructor).
 *
 * Calls Table::printDTD on the local EMdFOutput.
 *
 * @see Table::printDTD().
 */
void EmdrosEnv::printDTDTable(void)
{
	Table::printDTD(m_pEE->pOut);
}


/** Output the DTD of Sheaf.
 *
 * Outputs the DTD of Sheaf on the stream in the local EMdFOutput.
 * That is, the on the stream you passed to the EmdrosEnv::EmdrosEnv
 * constructor (which defaults to std::cout for the second
 * constructor).
 *
 * Calls Sheaf::printDTD on the local EMdFOutput.
 *
 * @see Sheaf::printDTD().
 */
void EmdrosEnv::printDTDSheaf(void)
{
	Sheaf::printDTD(m_pEE->pOut);
}


/** Output the DTD of FlatSheaf.
 *
 * Outputs the DTD of FlatSheaf on the stream in the local EMdFOutput.
 * That is, the on the stream you passed to the EmdrosEnv::EmdrosEnv
 * constructor (which defaults to std::cout for the second
 * constructor).
 *
 * Calls FlatSheaf::printDTD on the local EMdFOutput.
 *
 * @see FlatSheaf::printDTD().
 */
void EmdrosEnv::printDTDFlatSheaf(void)
{
	FlatSheaf::printDTD(m_pEE->pOut);
}


/** Clean up.
 * 
 * clean() calls MQLExecEnv::clean().
 *
 * It is good to call if you are through with a query's results,
 * and there is a long time till the next executeXXX call.
 *
 * @see MQLExecEnv::clean()
 *
 */
void EmdrosEnv::clean()
{
	m_pEE->clean();
}


/** Get backend-name as a string.
 *
 * This gets the name of the backend (PostgreSQL, MySQL, or SQLite).
 * Calls EMdFDB::getBackendName() to get it.
 *
 * @see EMdFDB::getBackendName()
 *
 * @return The name of the backend.
 */
std::string EmdrosEnv::getBackendName(void)
{
	if (m_pEE == 0 || m_pEE->pDB == 0) {
		ASSERT_THROW(false, "EmdrosEnv::getBackendName(): Called before initializing EmdrosEnv.");
		return "";
	} else {
		return m_pEE->pDB->getBackendName();
	}
}

// Returns the string-representation of an enumeration constant in 
// enum enum_name with the value value.  If more than one enum constant
// has the same value, it is undefined which of them is returned.
/** Gets enum const name from value.
 *
 * Gets the enumeration constant corresponding to a given value
 * within a specific enumeration.
 *
 * Just calls EMdFDB::getEnumConstNameFromValue() (or its polymorphic 
 * instance).
 *
 * @see EMdFDB::getEnumConstNameFromValue()
 * 
 * @param value the value whose enum constant we will find.
 * @param enum_name the name of the enumeration within which we wish to 
 *        find the enumeration constant.
 * @param bDBOK Used to return DB error status: True if no error, false 
 *        if a DB error occurred.  This is what is normally returned as
 *        a boolean from most other EMdFDB methods.
 * @return The enumeration name.
 */
std::string EmdrosEnv::getEnumConstNameFromValue(long value,
						 const std::string& enum_name, 
						 /* out */ bool &bDBOK)
{
	std::string enum_const_name;
	bDBOK = m_pEE->pDB->getEnumConstNameFromValue(value, enum_name, enum_const_name);
	return enum_const_name;
}
 
/** Test whether result is a sheaf
 *
 * Calls EmdrosEnv::getResult() to see if there is a result.  If there
 * is no result, return false.  Otherwise, if there is a result, call
 * MQLResult::isSheaf().
 *
 * @see MQLResult::isSheaf(), EmdrosEnv::getResult().
 *
 * @return true if result is a sheaf. False if there is no result, or
 *         result is not a sheaf.
 */
bool EmdrosEnv::isSheaf(void)
{
	MQLResult *pResult = getResult();
	if (pResult == 0) {
		return false;
	} else {
		return pResult->isSheaf();
	}
}

/** Test whether result is a flat sheaf
 *
 * Calls EmdrosEnv::getResult() to see if there is a result.  If there
 * is no result, return false.  Otherwise, if there is a result, call
 * MQLResult::isFlatSheaf().
 *
 * @see MQLResult::isFlatSheaf(), EmdrosEnv::getResult().
 *
 * @return true if result is a flat sheaf. False if there is no
 * result, or result is not a flat sheaf.
 */
bool EmdrosEnv::isFlatSheaf(void)
{
	MQLResult *pResult = getResult();
	if (pResult == 0) {
		return false;
	} else {
		return pResult->isFlatSheaf();
	}
}

/** Test whether result is a table
 *
 * Calls EmdrosEnv::getResult() to see if there is a result.  If there
 * is no result, return false.  Otherwise, if there is a result, call
 * MQLResult::isTable().
 *
 * @see MQLResult::isTable(), EmdrosEnv::getResult().
 *
 * @return true if result is a table. False if there is no result, or
 *         result is not a table.
 */
bool EmdrosEnv::isTable(void) 
{
	MQLResult *pResult = getResult();
	if (pResult == 0) {
		return false;
	} else {
		return pResult->isTable();
	}
}
