/*
 * mqltry.cpp
 *
 * Trial of MQL and EMdF framework
 * Created: 3/6-2001 (March 6, 2001)
 * Last update: 4/18-2018
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


#include <monads.h>
#include <emdfdb.h>
#include <utils.h>
#include <emdf_output.h>
#include <mql_result.h>
#include <emdros_environment.h>
#include <mql_sheaf.h>
#include <string_func.h>
#include <encryption.h>
#include <iostream>
#include <sstream>
#include <string>
#include <opt.h>
#include <messages.h>
#include "mqlqa.h"


static bool bAllOK; // Global variable -- ugly, but it works

void show_version(std::ostream& ostr)
{
	ostr << "mqltry from Emdros version " << EMDROS_VERSION << std::endl;
}

void print_usage(std::ostream& ostr)
{
	ostr << "Usage:" << std::endl;
	ostr << "   mqltry [options] " << std::endl;
	ostr << "OPTIONS:" << std::endl;
	printUsageStandardArguments(ostr);
	ostr << "DEFAULTS:" << std::endl;
	printUsageDefaultsOfStandardArguments(ostr);
}

#define DB_ERROR {							\
		std::cerr << "ERROR: Database error in " << success << " while executing query " << std::endl; \
		std::cerr << "-------------------" << std::endl;	\
		std::cerr << mql_query << std::endl;			\
		std::cerr << "-------------------" << std::endl;	\
		std::cerr << pEE->getDBError() << std::endl;		\
		std::cerr << pEE->getCompilerError() << std::endl;	\
		bAllOK = false;						\
	}


#define QUERY_WAS {							\
		std::cerr << "The query was:" << std::endl;		\
		std::cerr << "------------------------------------------------" << std::endl; \
		std::cerr << mql_query << std::endl;			\
		std::cerr << "------------------------------------------------" << std::endl; \
		std::cerr << pEE->getCompilerError() << std::endl;	\
		std::cerr << "while attempting " << success << std::endl; \
	}

#define COMPILE_ERROR {							\
		std::cerr << "ERROR: Compile error in " << success << "." << std::endl; \
		QUERY_WAS;						\
		bAllOK = false;						\
	} 



#define ROW_ERROR {							\
		std::cerr << "mqltry ROW_ERROR: Expecting " << no_of_expected_rows << " rows, got " << no_of_rows << "." << std::endl; \
		QUERY_WAS;						\
		bAllOK = false;						\
	}

void exec_string_ignore_error(EmdrosEnv *pEE, std::string mql_query, std::string success)
{
	bool bResult;
	if (!pEE->executeString(mql_query, bResult, false, false)) {
		// Do nothing
		// ESPECIALLY don't call DB_ERROR.  This will set bAllOK to false,
		// which is not the intention!
	} else {
		std::cout << "SUCCESS in " << success << std::endl;
	}

	// Clean up
	pEE->clean();
}

bool exec_string(EmdrosEnv *pEE, std::string mql_query, std::string success, bool bExpectedResult = true)
{
	bool bResult;
	if (!pEE->executeString(mql_query, bResult, true, false)) {
		DB_ERROR;
		bResult= false;
	} else {
		if (bExpectedResult != bResult) {
			COMPILE_ERROR;
			bResult= false;
		} else { 
			std::cout << "SUCCESS in " << success << std::endl;
			bResult= true;
		}
	}

	// Clean up
	pEE->clean();

	// Return
	return bResult;
}

bool handle_with_key(eBackendKind backend_kind, const std::string& password)
{
	bool bResult = (backend_kind == kSQLite3) && (!password.empty());
	if (bResult) {
		std::cout << "Using password: " + password + "\n";
	}
	return bResult;
}


std::string get_db_name(eBackendKind backend_kind, const std::string& base_name)
{
	switch (backend_kind) {
	case kBackendNone:
	case kPostgreSQL:
	case kMySQL:
	case kBPT:
	case kBPT2:
		return base_name;
		break;
	case kSQLite2:
		return base_name + "_sqlite2";
		break;
	case kSQLite3:
		return base_name + "_sqlite3";
		break;
	}
	return base_name;
}


bool exec_newdb(EmdrosEnv *pEE, eBackendKind backend_kind, const std::string& password)
{
	// Drop database emdf3
	std::string mql_query = "DROP DATABASE " + get_db_name(backend_kind, "emdf3") + " GO";
	exec_string_ignore_error(pEE, mql_query, "dropping database emdf3");

	// Create database
	if (handle_with_key(backend_kind, password)) {
		mql_query = "CREATE DATABASE " + get_db_name(backend_kind, "emdf3") + " WITH KEY '" + password + "' GO";
	} else {
		mql_query = "CREATE DATABASE " + get_db_name(backend_kind, "emdf3") + " GO";
	}
	if (!exec_string(pEE, mql_query, "creating database emdf3"))
		return false;

	// Use database
	if (handle_with_key(backend_kind, password)) {
		mql_query = "USE DATABASE " + get_db_name(backend_kind, "emdf3") + " WITH KEY '" + password + "' GO";
	} else {
		mql_query = "USE DATABASE " + get_db_name(backend_kind, "emdf3") + " GO";
	}
	if (!exec_string(pEE, mql_query, "using database emdf3"))
		return false;

	return true;
}






bool exec_string_take_over_sheaf(EmdrosEnv *pEE, Sheaf **ppSheaf, std::string mql_query, std::string success)
{
	bool bResult;
	*ppSheaf = 0;
	if (!pEE->executeString(mql_query, bResult, true, false)) {
		DB_ERROR;
		bResult= false;
	} else {
		if (!bResult) {
			COMPILE_ERROR;
			bResult= false;
		} else { 
			*ppSheaf = pEE->takeOverSheaf();
			std::cout << "SUCCESS in " << success << std::endl;
			bResult= true;
		}
	}

	// Clean up
	pEE->clean();

	// Return
	return bResult;
}

bool exec_QueryAnswer(EmdrosEnv *pEE, const QueryAnswer& qa, eBackendKind backend_kind, const std::string& password)
{
	bool bResult = true;
	std::string success = qa.m_query_name;
	if (qa.m_bNewDB) {
		if (!exec_newdb(pEE, backend_kind, password)) {
			std::cerr << "ERROR: Could not create new DB for " << success << std::endl;
			return false;
		}
	}
	std::string mql_query = qa.m_query;
	if (!pEE->executeString(mql_query, bResult, true, false)) {
		if (qa.m_bDBSuccessExpected) {
			DB_ERROR;
			bResult= false;
		} else {
			std::cout << "SUCCESS: " << success << std::endl;
			bResult = true;
		}
	} else {
		if (!bResult) {
			if (qa.m_bCompilerSuccessExpected) {
				COMPILE_ERROR;
				bResult= false;
			} else {
				if (pEE->getCompilerError() != qa.m_answer) {
					std::cerr << "FAILURE: " << success << " made a compiler failure, which it should." << std::endl;
					std::cerr << "         However, the expected output was:'" << qa.m_answer << "', but it was in fact '" << pEE->getCompilerError() << "'" << std::endl;
					bResult = false;
				} else {
					std::cout << "SUCCESS: " << success << " made a compiler failure, which it should!" << std::endl;
					std::cout << "SUCCESS: The error was: '" << pEE->getCompilerError() << "'" << std::endl;
					bResult = true;
				}
			}
		} else { 
			if (!qa.m_bCompilerSuccessExpected) {
				std::cerr << "FAILURE: " << success << " did not fail with a compiler error, as expected!" << std::endl;
				bResult = false;
			} else {
				std::ostringstream ostr;
				EMdFOutput stringEMdFOutput(kCSISO_8859_1, &ostr, kOKConsole);
				if (pEE->isFlatSheaf()) {
					FlatSheaf *pFlatSheaf = pEE->takeOverFlatSheaf();
					pFlatSheaf->printConsole(&stringEMdFOutput);
					delete pFlatSheaf;
				} else if (pEE->isSheaf()) {
					Sheaf *pSheaf = pEE->takeOverSheaf();
					pSheaf->printConsole(&stringEMdFOutput);
					delete pSheaf;
				} else if (pEE->isTable()) {
					Table *pTable = pEE->takeOverTable();
					pTable->printConsole(&stringEMdFOutput);
					delete pTable;
				} else {
					; // Leave ostr blank.
				}
				
				if (ostr.str() == qa.m_answer) {
					std::cout << "SUCCESS in " << success << std::endl;
					bResult= true;
				} else {	
					std::cerr << "ERROR: Expecting result to be '\n" << qa.m_answer << "' but it was '\n" << ostr.str() << "'." << std::endl;
					bResult = false;
					QUERY_WAS;
					bAllOK = false;
				}
			}
		}
	}

	// Clean up
	pEE->clean();

	// Return
	return bResult;
}


bool exec_string_test_sheaf(EmdrosEnv *pEE, std::string mql_query, std::string expected_sheaf, std::string success)
{
	bool bResult;
	if (!pEE->executeString(mql_query, bResult, true, false)) {
		DB_ERROR;
		bResult= false;
	} else {
		if (!bResult) {
			COMPILE_ERROR;
			bResult= false;
		} else { 
			std::ostringstream ostr;
			EMdFOutput stringEMdFOutput(kCSISO_8859_1, &ostr, kOKConsole);
			if (pEE->isFlatSheaf()) {
				if (pEE->isTable()) {
					bResult = false;
					std::cerr << "FAILURE: isTable() returns true for flat sheaf." << std::endl;
				}
				if (pEE->isSheaf()) {
					bResult = false;
					std::cerr << "FAILURE: isSheaf() returns true for flat sheaf." << std::endl;
				}
				FlatSheaf *pFlatSheaf = pEE->takeOverFlatSheaf();
				pFlatSheaf->printConsole(&stringEMdFOutput);
				delete pFlatSheaf;
			} else if (pEE->isSheaf()) {
				if (pEE->isTable()) {
					bResult = false;
					std::cerr << "FAILURE: isTable() returns true for sheaf." << std::endl;
				}
				if (pEE->isFlatSheaf()) {
					bResult = false;
					std::cerr << "FAILURE: isFlatSheaf() returns true for sheaf." << std::endl;
				}
				Sheaf *pSheaf = pEE->takeOverSheaf();
				pSheaf->printConsole(&stringEMdFOutput);
				delete pSheaf;
			} else {
				std::cerr << "ERROR: Result is not a sheaf. Please contact the author through\n           http://emdros.org/contact.html\nand report this bug." << std::endl;
				bResult = false;
				bAllOK = false;
				pEE->clean();
				return bResult;
			}
      
			if (ostr.str() == expected_sheaf) {
				std::cout << "SUCCESS in " << success << std::endl;
				bResult= true;
			} else {	
				std::cerr << "ERROR: Expecting resulting sheaf to be '\n" << expected_sheaf << "' but it was '\n" << ostr.str() << "'." << std::endl;
				bResult = false;
				QUERY_WAS;
				bAllOK = false;
			}
		}
	}

	// Clean up
	pEE->clean();

	// Return
	return bResult;
}

bool exec_string_test_harvestMarks(EmdrosEnv *pEE, const std::string& mql_query, std::string& expected_harvest_marks, bool bUseSingleMarks, const std::string& success)
{
	bool bResult;
	std::string str_result;
	if (!pEE->executeString(mql_query, bResult, true, true)) {
		DB_ERROR;
		bResult= false;
	} else {
		if (!bResult) {
			COMPILE_ERROR;
			bResult= false;
		} else { 
			if (pEE->isSheaf()) {
				std::ostringstream ostr;

				String2SOMMap result;
				pEE->getSheaf()->harvestMarks(result, bUseSingleMarks);
				String2SOMMapConstIterator ci = result.const_iterator();
				while (ci.hasNext()) {
					std::string marks_key = ci.next();
					SetOfMonads som = result.getMonadSet(marks_key);
					ostr << '"' << marks_key << '"' << " -> " << som.toString() << '\n';
				}
				str_result = ostr.str();
				if (str_result == expected_harvest_marks) {
					bResult = true;
				} else {
					bResult = false;
				}
			}
		}
	}
	if (bResult) {
		std::cout << "SUCCESS: " << success;
	} else {
		std::cerr << "ERROR: Expecting resulting harvestMarks string to be '\n" << expected_harvest_marks << "' but it was '\n" << str_result << "'" << std::endl;
	}
	QUERY_WAS;

	// Clean up
	pEE->clean();

	// Return
	return bResult;
}


bool exec_string_check_table(EmdrosEnv *pEE, std::string mql_query, std::string success, Table& table_to_check_with)
{
	bool bResult;
	if (!pEE->executeString(mql_query, bResult, true, false)) {
		DB_ERROR;
		bResult = false;
	} else {
		if (!bResult) {
			COMPILE_ERROR;
		} else {
			std::cout << "SUCCESS in " << success << std::endl;
		}
	}

	if (pEE->isSheaf()) {
		std::cerr << "FAILURE: isSheaf() returns true for table." << std::endl;
		bResult = false;
	}
	if (pEE->isFlatSheaf()) {
		std::cerr << "FAILURE: isFlatSheaf() returns true for table." << std::endl;
		bResult = false;
	}
	if (!pEE->isTable()) {
		std::cerr << "FAILURE: isTable() returns false for table." << std::endl;
		bResult = false;
	}
	

	// Get resulting id_d
	if (bResult) {
		Table *pResult = pEE->getTable();
		bResult = pResult->equals(table_to_check_with);
	}

	// Clean up
	pEE->clean();

	// Return result
	return bResult;

}


bool exec_string_get_id_d(EmdrosEnv *pEE, std::string mql_query, std::string success, id_d_t& result_id_d)
{
	bool bResult;
	if (!pEE->executeString(mql_query, bResult, true, false)) {
		DB_ERROR;
		bResult = false;
	} else {
		if (!bResult) {
			COMPILE_ERROR;
		} else {
			std::cout << "SUCCESS in " << success << std::endl;
		}
	}

	// Get resulting id_d
	if (bResult) {
		Table *pResult = pEE->getTable();
		TableIterator ti = pResult->iterator();
		std::string strId_d = pResult->getColumn(ti, 1);
		result_id_d = string2id_d(strId_d);
	}

	// Clean up
	pEE->clean();

	// Return result
	return bResult;
}

bool exec_string_get_long(EmdrosEnv *pEE, std::string mql_query, std::string success, long& result_long)
{
	bool bResult;
	if (!pEE->executeString(mql_query, bResult, true, false)) {
		DB_ERROR;
		bResult = false;
	} else {
		if (!bResult) {
			COMPILE_ERROR;
		} else {
			std::cout << "SUCCESS in " << success << std::endl;
		}
	}

	// Get resulting long
	if (bResult) {
		Table *pResult = pEE->getTable();
		TableIterator ti = pResult->iterator();
		std::string strLong = pResult->getColumn(ti, 1);
		result_long = string2long(strLong);
	}

	// Clean up
	pEE->clean();

	// Return result
	return bResult;
}

bool exec_string_get_string(EmdrosEnv *pEE, std::string mql_query, std::string success, int column, std::string& result_string)
{
	bool bResult;
	if (!pEE->executeString(mql_query, bResult, true, false)) {
		DB_ERROR;
		bResult = false;
	} else {
		if (!bResult) {
			COMPILE_ERROR;
		} else {
			std::cout << "SUCCESS in " << success << std::endl;
		}
	}

	// Get resulting id_d
	if (bResult) {
		Table *pResult = pEE->getTable();
		TableIterator ti = pResult->iterator();
		result_string = pResult->getColumn(ti, column);
	}

	// Clean up
	pEE->clean();

	// Return result
	return bResult;
}

bool exec_string_check_no_of_rows(EmdrosEnv *pEE, std::string mql_query, std::string success, long no_of_expected_rows)
{
	bool bResult;
	if (!pEE->executeString(mql_query, bResult, true, false)) {
		DB_ERROR;
		bResult = false;
	} else {
		if (!bResult) {
			COMPILE_ERROR;
		} else {
			// Check no of expected rows
			long no_of_rows = pEE->getTable()->size();
			if (no_of_rows != no_of_expected_rows) {
				bResult = false;
				ROW_ERROR;
			} else {
				std::cout << "SUCCESS in " << success << std::endl;
			}
		}
	}

	// Clean up
	pEE->clean();

	// Return result
	return bResult;
}

bool exec_string_check_counts(EmdrosEnv *pEE, std::string mql_query, const std::string& object_type_name, bool bUseOnlyFocusObjects, long nExpectedObjectCount, long nExpectedObjectCountFromObjectType, long nExpectedStrawCount, const std::string& success)
{
	bool bResult;
	if (!pEE->executeString(mql_query, bResult, true, false)) {
		DB_ERROR;
		bResult = false;
	} else {
		if (!bResult) {
			COMPILE_ERROR;
		} else {
			long no_of_objects = pEE->getSheaf()->countObjects(bUseOnlyFocusObjects);
			long no_of_objects_from_object_type = pEE->getSheaf()->countObjectsFromObjectType(object_type_name, bUseOnlyFocusObjects);
			
			long no_of_straws = pEE->getSheaf()->countStraws();

			if (no_of_objects != nExpectedObjectCount
			    || no_of_objects_from_object_type != nExpectedObjectCountFromObjectType
			    || no_of_straws != nExpectedStrawCount) {
				bResult = false;
				std::cerr << "FAILURE in " << success << ": (objectCount, objectCountFromOjectType, strawCount) = (" << no_of_objects << ", " << no_of_objects_from_object_type << ", " << no_of_straws << ") which is != (" << nExpectedObjectCount << ", " << nExpectedObjectCountFromObjectType << ", " << nExpectedStrawCount << ")" << std::endl;
				
			} else {
				std::cout << "SUCCESS in " << success << std::endl;
			}
		}
	}

	// Clean up
	pEE->clean();

	// Return result
	return bResult;
}

bool exec_string_check_monad(EmdrosEnv *pEE, std::string mql_query, std::string success, monad_m expected_monad)
{
	bool bResult = false;
	if (!pEE->executeString(mql_query, bResult, true, false)) {
		DB_ERROR;
		bResult = false;
	} else {
		if (!bResult) {
			COMPILE_ERROR;
			bResult = false;
		} else {
			// Check monad
			long no_of_expected_rows = 1;
			long no_of_rows = pEE->getTable()->size();
			if (no_of_rows != no_of_expected_rows) {
				bResult = false;
				ROW_ERROR;
			} else {
				monad_m monad_m_result;
				MQLResult *pResult = pEE->getResult();
				TableIterator ti = pResult->iterator();
				std::string strMonad_m = pResult->getColumn(ti, 1);
				monad_m_result = string2monad_m(strMonad_m);
				if (monad_m_result != expected_monad) {
					std::cerr << "Error: Expecting resulting monad to be " << expected_monad << " but it was " << monad_m_result << "." << std::endl;
					bResult = false;
					QUERY_WAS;
					bAllOK = false;
				} else {
					std::cout << "SUCCESS in " << success << std::endl;
					bResult = true;
				}
			}
		}
	}

	// Clean up
	pEE->clean();

	// Return result
	return bResult;
}


bool exec_string_expect_compiler_error_but_no_DB_error(EmdrosEnv *pEE, std::string mql_query, std::string success)
{
	bool bResult;
	if (!pEE->executeString(mql_query, bResult, false, false)) {
		DB_ERROR;
		bResult = false;
	} else {
		bResult = !bResult;
		if (bResult) {
			std::cout << "SUCCESS in " << success << std::endl;
		} else {
			std::cerr << "FAILURE in " << success << std::endl;
			bAllOK = false;
		}
	}

	// Clean up
	pEE->clean();

	return bResult;
}

int testall(EmdrosEnv *pEE, eBackendKind backend_kind, const std::string& password)
{
	std::string mql_query;
	std::string expected_sheaf;
	int result = 0;
	Table features_table;
	Sheaf *pMySheaf = 0;
	std::string AEOEAA_string;
	std::string my_string;
	std::string my_string2;
	std::string my_string3;
	std::string expected_harvest_marks;
	SetOfMonads somSheaf;
	Sheaf *pSheaf1 = 0;
	bool bIsFail;
	bool bUseSingleMarks = true;
	Sheaf *pSheaf2;
	SheafConstIterator ci1;
	SheafIterator i1;

	// Drop database emdf4
	mql_query = "DROP DATABASE " + get_db_name(backend_kind, "emdf4") + " GO";
	exec_string_ignore_error(pEE, mql_query, "dropping database emdf4");

	// Drop database emdf3
	mql_query = "DROP DATABASE " + get_db_name(backend_kind, "emdf3") + " GO";
	exec_string_ignore_error(pEE, mql_query, "dropping database emdf3");

	bAllOK = true; // Global variable

	// Create database
	if (handle_with_key(backend_kind, password)) {
		mql_query = "CREATE DATABASE " + get_db_name(backend_kind, "emdf3") + " WITH KEY '" + password + "' GO";
	} else {
		mql_query = "CREATE DATABASE " + get_db_name(backend_kind, "emdf3") + " GO";
	}
	if (!exec_string(pEE, mql_query, "creating database emdf3"))
		goto end2;

	// Create database
	if (handle_with_key(backend_kind, password)) {
		mql_query = "CREATE DATABASE " + get_db_name(backend_kind, "emdf4") + " WITH KEY '" + password + "' GO";
	} else {
		mql_query = "CREATE DATABASE " + get_db_name(backend_kind, "emdf4") + " GO";
	}
	if (!exec_string(pEE, mql_query, "creating database emdf4"))
		goto end1;

	// Use database
	if (handle_with_key(backend_kind, password)) {
		mql_query = "USE DATABASE " + get_db_name(backend_kind, "emdf4") + " WITH KEY '" + password + "' GO";
	} else {
		mql_query = "USE DATABASE " + get_db_name(backend_kind, "emdf4") + " GO";
	}
	if (!exec_string(pEE, mql_query, "using database emdf4"))
		goto end;

	// Use special emdf "database" (which does not need to exist)
	if (handle_with_key(backend_kind, password)) {
		mql_query = "USE DATABASE emdf WITH KEY '" + password + "' GO";
	} else {
		mql_query = "USE DATABASE emdf GO";
	}
	if (!exec_string(pEE, mql_query, "using database emdf"))
		goto end;

	// Use database
	if (handle_with_key(backend_kind, password)) {
		mql_query = "USE DATABASE " + get_db_name(backend_kind, "emdf3") + " WITH KEY '" + password + "' GO";
	} else {
		mql_query = "USE DATABASE " + get_db_name(backend_kind, "emdf3") + " GO";
	}
	if (!exec_string(pEE, mql_query, "using database emdf3"))
		goto end;

	// Create enumeration
	mql_query = 
		"CREATE ENUMERATION "
		"psp_t = { "
		" default psp_None = -1, verb = 1, noun, proper_noun, adverb, preposition, \n"
		" conjunction = 7, foobar, pronoun "
		"} GO";
	if (!exec_string(pEE, mql_query, "creating enumeration psp_t"))
		goto end;

	// Create enumeration
	mql_query = 
		"CREATE ENUMERATION "
		"function_t = { "
		"  fn_Subject, fn_Object, fn_Predicate, fn_Adjunct, fn_Complement,\n"
		"  fn_Time, fn_Location"
		"} GO";
	if (!exec_string(pEE, mql_query, "creating enumeration function_t"))
		goto end;

	// This tests that the enumeration is not created.
	// The compiler should generate an error saying that
	// two labels cannot have the same value.
	mql_query = 
		"CREATE ENUMERATION "
		"tybalt_t = { "
		" foo, bar = 1 "
		"} GO";
	if (!exec_string(pEE, mql_query, "not creating enumeration with two labels with the same value", false))
		goto end;

	// This tests that the enumeration is not updated.
	// The compiler should generate an error saying that
	// two labels cannot have the same value.
	mql_query = 
		"UPDATE ENUMERATION "
		"psp_t = { "
		" ADD adjective = 2, UPDATE adverb = 1, REMOVE preposition "
		"} GO";
	if (!exec_string(pEE, mql_query, "not updating enumeration with two labels with the same value", false))
		goto end;

	// Update enumeration
	mql_query =  
		"UPDATE ENUMERATION "
		"psp_t = { "
		" remove foobar, add default negation = 11, add demonstrative = 8, update conjunction = 6 "
		"} GO";
	if (!exec_string(pEE, mql_query, "updating enumeration psp_t"))
		goto end;


	// Create enumeration
	mql_query = 
		"CREATE ENUMERATION "
		"sploinks_t = { "
		" habib, habab, default habub, WxQt = 142, WXQt = 132, xYqt = 121, XYqt = 111 "
		"} GO";
	if (!exec_string(pEE, mql_query, "creating enumeration sploinks_t"))
		goto end;

	// Selecting enumeration constants
	mql_query = 
		"SELECT ENUMERATION CONSTANTS FROM ENUMERATION sploinks_t GO";
	if (!exec_string_check_no_of_rows(pEE, mql_query, "selecting enumeration constants", 7))
		goto end;

	// Dropping enumeration constants
	mql_query = 
		"UPDATE ENUMERATION "
		"sploinks_t = { "
		"  remove xYqt"
		"} GO";
	if (!exec_string(pEE, mql_query, "dropping enumeration constant"))
		goto end;

	// Selecting enumeration constants
	mql_query = 
		"SELECT ENUMERATION CONSTANTS FROM ENUMERATION sploinks_t GO";
	if (!exec_string_check_no_of_rows(pEE, mql_query, "selecting enumeration constants", 6))
		goto end;

	// Updating enumeration constant with case-insensitive duplicate
	mql_query = 
		"UPDATE ENUMERATION "
		"sploinks_t = { "
		"  UPDATE WXQt = -2 "
		"} GO";
	if (!exec_string(pEE, mql_query, "updating enumeration constant"))
		goto end;

	// Drop enumeration
	mql_query = 
		"DROP ENUMERATION sploinks_t GO";
	if (!exec_string(pEE, mql_query, "dropping enumeration sploinks_t"))
		goto end;

	// Create monad set som1
	mql_query = 
		"CREATE MONAD SET SOM1 WITH MONADS = { 1-3, 10-15 } GO";
	if (!exec_string(pEE, mql_query, "creating monad set som1"))
		goto end;
  
	// Create monad set som2
	mql_query = 
		"CREATE MONAD SET SOM2 WITH MONADS = { 4-20 } GO";
	if (!exec_string(pEE, mql_query, "creating monad set som2"))
		goto end;

	// Update monad set som1 with union
	mql_query = 
		"UPDATE MONAD SET SOM1 UNION SOM2 GO";
	if (!exec_string(pEE, mql_query, "updating monad set with union"))
		goto end;

	// Update monad set som1 with intersect
	mql_query = 
		"UPDATE MONAD SET SOM1 INTERSECT { 3-15 } GO";
	if (!exec_string(pEE, mql_query, "updating monad set with intersect"))
		goto end;

	// Update monad set som1 with difference
	mql_query = 
		"UPDATE MONAD SET SOM1 DIFFERENCE { 5-8 } GO";
	if (!exec_string(pEE, mql_query, "updating monad set with difference"))
		goto end;

	// Update monad set som1 with replace
	mql_query = 
		"UPDATE MONAD SET SOM1 replace SOM2 GO";
	if (!exec_string(pEE, mql_query, "updating monad set with replace"))
		goto end;

	// Create a segment
	mql_query = "CREATE SEGMENT segment1 RANGE = 1 - 450000 GO";
	if (!exec_string(pEE, mql_query, "creating \"segment\""))
		goto end;

	// Get all monad sets
	mql_query = 
		"GET MONAD SETS ALL GO";
	if (!exec_string(pEE, mql_query, "getting all monad sets"))
		goto end;

	// Get all monad sets
	mql_query = 
		"GET MONAD SETS som2 GO";
	if (!exec_string(pEE, mql_query, "getting som2"))
		goto end;

	// Get monad set names
	mql_query = 
		"SELECT MONAD SETS GO";
	if (!exec_string(pEE, mql_query, "selecting monad sets"))
		goto end;

	// Dropping monad set
	mql_query = 
		"DROP MONAD SET SOM2 GO";
	if (!exec_string(pEE, mql_query, "dropping monad set som2"))
		goto end;

	// Create object type
	mql_query = 
		"CREATE OBJECT TYPE\n"
		"WITH MULTIPLE RANGE OBJECTS\n"
		"[Word\n"
		"  surface : string(34) FROM SET;\n"
		"  lemma : ascii(127) WITH INDEX;\n" // NOTE: This must be ASCII for the escaping-tests below to function
		"  half_verse : ascii(1) DEFAULT 'A';\n" 
		"  psp : psp_t;\n"
		"  psplist : LIST OF psp_t;\n"
		"  number : integer;\n"
		"  idUP : ID_D;\n"
		"] GO";
	if (!exec_string(pEE, mql_query, "creating object type Word"))
		goto end;

	// Create object type
	mql_query = 
		"CREATE OBJECT TYPE\n"
		"WITH SINGLE RANGE OBJECTS\n"
		"[Sploinks\n"
		"  lemma: STRING DEFAULT '\xc3\xa6';\n"
		"] GO";
	if (!exec_string(pEE, mql_query, "creating object type Sploinks"))
		goto end;

	// Create object type
	mql_query = 
		"CREATE OBJECT TYPE\n"
		"WITH SINGLE MONAD OBJECTS\n"
		"[SploinksSingleMonad\n"
		"  lemma: STRING DEFAULT '\xc3\xa7';\n"
		"] GO";
	if (!exec_string(pEE, mql_query, "creating object type SploinksSingleMonad"))
		goto end;

	// Create object type
	mql_query = 
		"CREATE OBJECT TYPE\n"
		"[Phalanges\n"
		"  fog : ASCII DEFAULT '?';\n"
		"  fog2 : ASCII(1) DEFAULT '?';\n"
		"  surface : STRING DEFAULT '?';\n"
		"  surface2 : STRING(30) DEFAULT '?';\n"
		"  crasis : INTEGER DEFAULT -1;\n"
		"  idUP : ID_D DEFAULT NIL;\n"
		"]\n"
		" GO";
	if (!exec_string(pEE, mql_query, "creating object type Phalanges"))
		goto end;

	// Create object type
	mql_query = 
		"CREATE OBJECT TYPE\n"
		"[Phrase\n"
		"  dummy : ASCII(1) default 'C';\n"
		"  parents : LIST OF id_d;\n"
		"  pages : LIST OF integer;\n"
		"  functions : LIST OF function_t;\n"
		"  function : function_t;\n"
		"  number : integer;\n"
		"] GO";
	if (!exec_string(pEE, mql_query, "creating object type Phrase"))
		goto end;

	// Select object types
	mql_query = 
		"SELECT OBJECT TYPES GO";
	if (!exec_string_check_no_of_rows(pEE, mql_query, "selecting object types",5))
		goto end;

	// Select features
	mql_query = 
		"SELECT FEATURES FROM OBJECT TYPE [Word] GO";
	if (!exec_string_check_no_of_rows(pEE, mql_query, "selecting features",8))
		goto end;

	// Select enumerations
	mql_query = 
		"SELECT ENUMERATIONS GO";
	if (!exec_string_check_no_of_rows(pEE, mql_query, "selecting enumerations",2))
		goto end;

	// Select enumeration constants
	mql_query = 
		"SELECT ENUMERATION CONSTANTS FROM ENUM psp_t GO";
	if (!exec_string_check_no_of_rows(pEE, mql_query, "selecting enumeration constants", 10))
		goto end;
	std::cout << "Check that conjunction has value 6 and that negation is the default." << std::endl;

	// Select object types using enumeration
	mql_query = 
		"SELECT OBJECT TYPES USING ENUMERATION psp_t GO";
	if (!exec_string_check_no_of_rows(pEE, mql_query, "selecting object types using enumeration",1))
		goto end;

	// Create object from monads
	mql_query = 
		"CREATE OBJECT\n"
		"FROM MONADS = { 1-2 }\n"
		"[Word\n"
		"  surface := 'KAI\\\xc3\xa6';\n"
		"  lemma := 'KAI/';\n"
		"  psp := conjunction;\n"
		"  psplist := ();\n"
		"  number := 1;\n"
		"] GO";
	id_d_t object1;
	if (!exec_string_get_id_d(pEE, mql_query, "creating object from monads", object1))
		goto end;

	// Create object from monads
	mql_query = 
		"CREATE OBJECT\n"
		"FROM MONADS = { 4-5 }\n"
		"[Word\n"
		"  surface := \"U(MEI=S\";\n"
		"  lemma := 'E)GW/';\n"
		"  psp := pronoun;\n"
		"  psplist := (pronoun,conjunction);\n"
		"  number := 2;\n"
		"] GO";
	id_d_t object2;
	if (!exec_string_get_id_d(pEE, mql_query, "creating object from monads", object2))
		goto end;

	// Create object from id_ds
	mql_query = 
		"CREATE OBJECT\n"
		"FROM ID_DS = " + id_d2string(object1) + ", " + id_d2string(object2) + "\n" +
		"[Phrase] GO\n";
	id_d_t object3;
	if (!exec_string_get_id_d(pEE, mql_query, "creating object from id_ds", object3))
		goto end;

	// Create object from monads
	mql_query = 
		"CREATE OBJECT\n"
		"FROM MONADS = { 3 }\n"
		"[Word\n"
		"  surface := \"DE\\\\\";\n"
		"  lemma := 'DE/';\n"
		"  psp := conjunction;\n"
		"  psplist := (conjunction);\n"
		"] GO";
	id_d_t object4;
	if (!exec_string_get_id_d(pEE, mql_query, "creating object from monads", object4))
		goto end;

	// Select objects at
	mql_query = "SELECT OBJECTS\n"
		"AT MONAD = 1\n"
		"[Word] GO";
	if (!exec_string_check_no_of_rows(pEE, mql_query, "selecting objects at monad",1))
		goto end;

	// Get features
	mql_query = "GET FEATURES surface, self, number\n"
		"FROM OBJECTS WITH ID_DS = " + id_d2string(object1) + ", " + id_d2string(object2) + "\n"
		"[Word] GO";
	if (!exec_string_check_no_of_rows(pEE, mql_query, "getting features",2))
		goto end;

	// Get features
	mql_query = "GET FEATURES self\n"
		"FROM OBJECT WITH ID_D = " + id_d2string(object3) + "\n"
		"[Phrase] GO";
	if (!exec_string_check_no_of_rows(pEE, mql_query, "getting features",1))
		goto end;

	// GET SET FROM FEATURE
	mql_query = "GET SET FROM FEATURE surface [Word] GO\n";
	if (!exec_string_check_no_of_rows(pEE, mql_query, "getting set from feature",3))
		goto end;

	// GET SET FROM FEATURE: Should fail
	mql_query = "GET SET FROM FEATURE surface [df1231223] GO\n";
	if (!exec_string_expect_compiler_error_but_no_DB_error(pEE, mql_query, "not getting set from nonexistent object type"))
		goto end;

	// GET SET FROM FEATURE: Should fail
	mql_query = "GET SET FROM FEATURE bl231232 [Word] GO\n";
	if (!exec_string_expect_compiler_error_but_no_DB_error(pEE, mql_query, "not getting set from nonexistent feature"))
		goto end;

	// GET SET FROM FEATURE: Should fail
	mql_query = "GET SET FROM FEATURE psp [Word] GO\n";
	if (!exec_string_expect_compiler_error_but_no_DB_error(pEE, mql_query, "not getting set from existing feature that is not a FROM SET feature"))
		goto end;

	// Update object
	mql_query = "UPDATE OBJECT\n"
		"BY MONADS = { 4-5 }\n"
		"[Word\n"
		"  surface := \"H(MEI=S\";\n"
		"  number := 1;\n"
		"  psp := proper_noun;\n"
		"  psplist := (proper_noun, conjunction);\n"
		"  idUP := NIL;\n"
		"] GO";
	if (!exec_string_check_no_of_rows(pEE, mql_query, "updating object by monads",1))
		goto end;

	// Update object
	mql_query = "UPDATE OBJECT\n"
		"BY ID_D = " + id_d2string(object3) + "\n"
		"[Phrase\n"
		"  dummy := 'D';\n"
		"] GO";
	if (!exec_string_check_no_of_rows(pEE, mql_query, "updating object by id_ds",1))
		goto end;

	// min_m
	mql_query = "SELECT MIN_M\n"
		"GO";
	if (!exec_string_check_monad(pEE, mql_query, "checking min_m", 1))
		goto end;
  
	// max_m
	mql_query = "SELECT MAX_M\n"
		"GO";
	if (!exec_string_check_monad(pEE, mql_query, "checking max_m", 5))
		goto end;

	// Query
	mql_query = "SELECT FOCUS OBJECTS\n"
		"WHERE\n"
		"[Phrase\n"
		"  [Word focus\n"
		"    psp = conjunction\n"
		"  ]\n"
		"  [gap retrieve\n"
		"     [Word focus\n"
		"        psp = conjunction\n"
		"     ]\n"
		"  ]\n"
		"  [Word\n"
		"      number = 1\n"
		"     GET surface, lemma, psp\n"
		"  ]\n"
		"] GO";
	expected_sheaf = 
		" //  <  < [ phrase 3 { 1-2, 4-5 } false  //  <  < [ word 1 { 1-2 } true  //  <  > \n"
		" ]\n"
		"[ pow_m { 3 } false  //  <  < [ word 4 { 3 } true  //  <  > \n"
		" ]\n"
		" > \n"
		" > \n"
		" ]\n"
		"[ word 2 { 4-5 } false (surface=\"H(MEI=S\",lemma=\"E)GW/\",psp=\"proper_noun\") //  <  > \n"
		" ]\n"
		" > \n"
		" > \n"
		" ]\n"
		" > \n"
		" > \n";
	if (!exec_string_test_sheaf(pEE, mql_query, expected_sheaf, "querying using MQL's query-subset, testing sheaf"))
		goto end;

	// Query
	mql_query = "SELECT FOCUS OBJECTS\n"
		"WHERE\n"
		"[Phrase\n"
		"  [Word focus\n"
		"     psp = conjunction\n"
		"  ]\n"
		"  [gap retrieve\n"
		"     [Word focus\n"
		"        psp = conjunction\n"
		"     ]\n"
		"  ]\n"
		"  [Word\n"
		"      number = 1\n"
		"     GET surface, lemma, psp\n"
		"  ]\n"
		"] GO";
	if (!exec_string_take_over_sheaf(pEE, &pMySheaf, mql_query, "querying using MQL's query-subset"))
		goto end;
	if (pMySheaf == 0) {
		std::cerr << "ERROR: returned sheaf was nil." << std::endl;
		bAllOK = false;
		goto end;
	}

	// Test getting SOM without focus booleans
	somSheaf = pMySheaf->getSOM(false);
	if (somSheaf.toString() == " { 1-5 } ") {
		std::cout << "SUCCESS: getting SOM from sheaf without focus boolean." << std::endl;
	} else {
		std::cerr << "ERROR: getting SOM from sheaf without focus boolean. Sheaf was '" << somSheaf.toString() << "'" << std::endl;
		bAllOK = false;
		delete pMySheaf;
		goto end;
	}

	somSheaf = pMySheaf->getSOM(true);
	if (somSheaf.toString() == " { 1-3 } ") {
		std::cout << "SUCCESS: getting SOM from sheaf with focus boolean." << std::endl;
	} else {
		std::cerr << "ERROR: getting SOM from sheaf with focus boolean. Sheaf was '" << somSheaf.toString() << "'" << std::endl;
		bAllOK = false;
		delete pMySheaf;
		goto end;
	}

	// Clean up after ourselves
	delete pMySheaf;

	// Query
	mql_query = "SELECT FOCUS OBJECTS\n"
		"RETURNING FLAT SHEAF\n"
		"WHERE\n"
		"[Phrase\n"
		"  [Word focus\n"
		"     psp = conjunction\n"
		"  ]\n"
		"  [gap retrieve\n"
		"     [Word focus\n"
		"        psp = conjunction\n"
		"     ]\n"
		"  ]\n"
		"  [Word\n"
		"      number = 1\n"
		"     GET surface, lemma, psp\n"
		"  ]\n"
		"] GO";
	expected_sheaf = " // <  < [ pow_m { 3 } false  //  <  > \n"
		" ]\n"
		" > \n"
		" < [ word 1 { 1-2 } true  //  <  > \n"
		" ]\n"
		"[ word 4 { 3 } true  //  <  > \n"
		" ]\n"
		"[ word 2 { 4-5 } false (surface=\"H(MEI=S\",lemma=\"E)GW/\",psp=\"proper_noun\") //  <  > \n"
		" ]\n"
		" > \n"
		" < [ phrase 3 { 1-2, 4-5 } false  //  <  > \n"
		" ]\n"
		" > \n"
		" >\n";
	if (!exec_string_test_sheaf(pEE, mql_query, expected_sheaf, "querying using MQL's query-subset and RETURNING FLAT SHEAF 1"))
		goto end;

	// Query
	mql_query = "SELECT FOCUS OBJECTS\n"
		"RETURNING FULL SHEAF\n"
		"WHERE\n"
		"[Phrase\n"
		"  [gap retrieve\n"
		"     [Word focus\n"
		"        psp = conjunction\n"
		"     ]\n"
		"  ]\n"
		"  [Word\n"
		"      number = 1\n"
		"     GET surface, lemma, psp\n"
		"  ]\n"
		"] GO";
	expected_sheaf = " //  <  < [ phrase 3 { 1-2, 4-5 } false  //  <  < [ pow_m { 3 } false  //  <  < [ word 4 { 3 } true  //  <  > \n"
		" ]\n"
		" > \n"
		" > \n"
		" ]\n"
		"[ word 2 { 4-5 } false (surface=\"H(MEI=S\",lemma=\"E)GW/\",psp=\"proper_noun\") //  <  > \n"
		" ]\n"
		" > \n"
		" > \n"
		" ]\n"
		" > \n"
		" > \n";
	if (!exec_string_test_sheaf(pEE, mql_query, expected_sheaf, "querying using MQL's query-subset and RETURNING FULL SHEAF 1"))
		goto end;


	// Query
	mql_query = "SELECT FOCUS OBJECTS\n"
		"RETURNING FULL SHEAF\n"
		"WHERE\n"
		"[Phrase\n"
		"  [gap? retrieve\n"
		"     [Word focus\n"
		"        psp = conjunction\n"
		"     ]\n"
		"  ]\n"
		"  [Word\n"
		"      number = 1\n"
		"     GET surface, lemma, psp\n"
		"  ]\n"
		"] GO";
	expected_sheaf = " //  <  < [ phrase 3 { 1-2, 4-5 } false  //  <  < [ word 1 { 1-2 } false (surface=\"KAI\\\\\\xc3\\xa6\",lemma=\"KAI/\",psp=\"conjunction\") //  <  > \n"
		" ]\n"
		" > \n"
		" < [ pow_m { 3 } false  //  <  < [ word 4 { 3 } true  //  <  > \n"
		" ]\n"
		" > \n"
		" > \n"
		" ]\n"
		"[ word 2 { 4-5 } false (surface=\"H(MEI=S\",lemma=\"E)GW/\",psp=\"proper_noun\") //  <  > \n"
		" ]\n"
		" > \n"
		" < [ word 2 { 4-5 } false (surface=\"H(MEI=S\",lemma=\"E)GW/\",psp=\"proper_noun\") //  <  > \n"
		" ]\n"
		" > \n"
		" > \n"
		" ]\n"
		" > \n"
		" > \n";
	if (!exec_string_test_sheaf(pEE, mql_query, expected_sheaf, "querying using MQL's query-subset and RETURNING FULL SHEAF 2"))
		goto end;

	mql_query = "SELECT ALL OBJECTS\n"
		"RETURNING FULL SHEAF\n"
		"WHERE\n"
		"[Phrase`Sort1\n"
		"  [gap?`Sort2`red retrieve\n"
		"     [Word`Sort3 focus\n"
		"        psp = conjunction\n"
		"     ]\n"
		"  ]\n"
		"  [Word`Sort4`blue\n"
		"      number = 1\n"
		"     GET surface, lemma, psp\n"
		"  ]\n"
		"] GO";
	expected_harvest_marks = "\"`Sort1\" ->  { 1-2, 4-5 } \n\"`Sort2\" ->  { 3 } \n\"`Sort3\" ->  { 3 } \n\"`Sort4\" ->  { 1-2, 4-5 } \n\"`blue\" ->  { 1-2, 4-5 } \n\"`red\" ->  { 3 } \n";
	bUseSingleMarks = true;
	if (!exec_string_test_harvestMarks(pEE, mql_query, expected_harvest_marks, bUseSingleMarks, "harvesting Marks 1"))
		goto end;

	expected_harvest_marks = "\"`Sort1\" ->  { 1-2, 4-5 } \n\"`Sort2`red\" ->  { 3 } \n\"`Sort3\" ->  { 3 } \n\"`Sort4`blue\" ->  { 1-2, 4-5 } \n";
	bUseSingleMarks = false;
	if (!exec_string_test_harvestMarks(pEE, mql_query, expected_harvest_marks, bUseSingleMarks, "harvesting Marks 2"))
		goto end;	
	
	// Query
	mql_query = "SELECT FOCUS OBJECTS\n"
		"RETURNING FLAT SHEAF ON Phrase\n"
		"WHERE\n"
		"[Phrase\n"
		"  [Word focus\n"
		"     psp = conjunction\n"
		"  ]\n"
		"  [gap retrieve\n"
		"     [Word focus\n"
		"        psp = conjunction\n"
		"     ]\n"
		"  ]\n"
		"  [Word\n"
		"      number = 1\n"
		"     GET surface, lemma, psp\n"
		"  ]\n"
		"] GO";
	expected_sheaf = " // <  < [ phrase 3 { 1-2, 4-5 } false  //  <  > \n"
		" ]\n"
		" > \n"
		" >\n";
	if (!exec_string_test_sheaf(pEE, mql_query, expected_sheaf, "querying using MQL's query-subset and RETURNING FLAT SHEAF, restricting it to Phrase"))
		goto end;

	// Query
	mql_query = "SELECT FOCUS OBJECTS\n"
		"RETURNING FULL SHEAF\n"
		"WHERE\n"
		"[Phrase\n"
		"  NOTEXIST [Word focus\n"
		"     psp = adverb\n"
		"  ]\n"
		"  [Word\n"
		"      number = 1\n"
		"     GET surface, lemma, psp\n"
		"  ]\n"
		"] GO";
	expected_sheaf = " //  <  < [ phrase 3 { 1-2, 4-5 } false  //  <  < [ word 1 { 1-2 } false (surface=\"KAI\\\\\\xc3\\xa6\",lemma=\"KAI/\",psp=\"conjunction\") //  <  > \n"
		" ]\n"
		" > \n"
		" < [ word 2 { 4-5 } false (surface=\"H(MEI=S\",lemma=\"E)GW/\",psp=\"proper_noun\") //  <  > \n"
		" ]\n"
		" > \n"
		" > \n"
		" ]\n"
		" > \n"
		" > \n";
	if (!exec_string_test_sheaf(pEE, mql_query, expected_sheaf, "querying using MQL's query-subset and RETURNING FULL SHEAF 3"))
		goto end;

	// Query
	mql_query = "SELECT OBJECTS\n"
		"HAVING MONADS IN { 1-2, 4-5 }\n"
		"[ALL] GO";
	if (!exec_string_check_no_of_rows(pEE, mql_query, "selecting objects having monads in monad set", 3))
		goto end;

	// Testing escaping of strings
	// Create object from monads
	my_string = "ah\\'m jus\\' 2\\\" to mah naim\\\\";
	my_string2 = "ah\'m jus\' 2\" to mah naim\\";
	mql_query = 
		"CREATE OBJECT\n"
		"FROM MONADS = { 1000-1100, 1102, 1104, 1106-1110 }\n"
		"[Word\n"
		"  lemma := \"" + my_string + "\";\n"
		"] GO";
	id_d_t object1000;
	if (!exec_string_get_id_d(pEE, mql_query, "creating object with escaped string", object1000))
		goto end;

	// min_m
	mql_query = "SELECT MIN_M\n"
		"GO";
	if (!exec_string_check_monad(pEE, mql_query, "checking min_m", 1))
		goto end;
  
	// max_m
	mql_query = "SELECT MAX_M\n"
		"GO";
	if (!exec_string_check_monad(pEE, mql_query, "checking max_m", 1110))
		goto end;

	// Test looking for a non-existent ID_D with GET FEATURES.
	// Should get an empty table back.
	mql_query = "GET FEATURES lemma\n"
		"FROM OBJECTS WITH ID_DS = 123456789\n"
		"[Word] GO";
	//features_table.startNewRow();
	if (!exec_string_check_table(pEE, mql_query, "checking that GET FEATURES with non-existent ID_D returns an empty table.",features_table))
		goto end;

	// Testing getting it back
	// Get features
	mql_query = "GET FEATURES lemma\n"
		"FROM OBJECTS WITH ID_DS = " + id_d2string(object1000) + "\n"
		"[Word] GO";
	features_table.startNewRow();
	features_table.append(id_d2string(object1000));
	features_table.append(my_string2);
	if (!exec_string_check_table(pEE, mql_query, "checking that escaped string came back alright",features_table))
		goto end;

	mql_query = "SELECT ALL OBJECTS WHERE [word self = 1000000000]GO";
	if (!exec_string_take_over_sheaf(pEE, &pMySheaf, mql_query, "querying to get a failed sheaf"))
		goto end;
	if (pMySheaf == 0) {
		std::cerr << "ERROR: returned sheaf was nil." << std::endl;
		bAllOK = false;
		goto end;
	} else {
		SheafConstIterator ci = pMySheaf->const_iterator();
		if (ci.hasNext()) {
			std::cerr << "ERROR: SheafConstiterator::hasNext() returned true, which it shouldn't." << std::endl;
			bAllOK = false;
			delete pMySheaf;
			goto end;
		}
		try {
			//const Straw *pStraw = 
			ci.next(); // This should throw an exception.

			std::cerr << "ERROR: SheafConstIterator::next() did not throw an exception, which it should!" << std::endl;
			bAllOK = false;
			delete pMySheaf;
			goto end;
		} catch (EmdrosException e) {
			// This is what it should do!
			std::cout << "SUCCESS: SheafConstIterator::next() threw an exception, which it should!" << std::endl;
		}
	}

	// Clean up after ourselves.
	delete pMySheaf;


	// NOTEXIST can be other than the only one in its blocks
	mql_query = "SELECT FOCUS OBJECTS WHERE [Phrase NOTEXIST [Word][Word]]GO";
	if (!exec_string(pEE, mql_query, "allowing a NOTEXIST to not be the only one in the blocks. Test #1."))
		goto end;

	mql_query = "SELECT FOCUS OBJECTS WHERE [Phrase NOTEXIST [Word][gap]]GO";
	if (!exec_string(pEE, mql_query, "allowing a NOTEXIST to not be the only one in the blocks. Test #2."))
		goto end;

	mql_query = "SELECT FOCUS OBJECTS WHERE [Phrase [Word]NOTEXIST[Word]]GO";
	if (!exec_string(pEE, mql_query, "allowing a NOTEXIST to not be the only one in the blocks. Test #3."))
		goto end;

	mql_query = "SELECT FOCUS OBJECTS WHERE [Phrase [Word]NOTEXIST[Word][Word]]GO";
	if (!exec_string(pEE, mql_query, "allowing a NOTEXIST to not be the only one in the blocks. Test #4."))
		goto end;

	// NOTEXIST may not have STAR
	mql_query = "SELECT FOCUS OBJECTS WHERE [Phrase NOTEXIST[Word]*]GO";
	if (!exec_string_expect_compiler_error_but_no_DB_error(pEE, mql_query, "not allowing a NOTEXIST to have Kleene Star. Test #1."))
		goto end;

	// We may not define object references across STAR...: SHOULD FAIL!
	mql_query = "SELECT ALL OBJECTS WHERE [Phrase [Word as w1]]*[Word surface=w1.surface]GO";
	if (!exec_string_expect_compiler_error_but_no_DB_error(pEE, mql_query, "not allowing object reference usage across STAR. Test #1"))
		goto end;

	// We may not define object references across STAR...: SHOULD FAIL!
	mql_query = "SELECT ALL OBJECTS WHERE [Phrase [Word as w1]]*[Word surface=w1.surface]GO";
	if (!exec_string_expect_compiler_error_but_no_DB_error(pEE, mql_query, "not allowing object reference usage across STAR. Test #2"))
		goto end;


	// We may not define object references across NOTEXIST...: SHOULD FAIL!
	mql_query = "SELECT ALL OBJECTS WHERE [Phrase NOTEXIST [Word as w1]][Word surface=w1.surface]GO";
	if (!exec_string_expect_compiler_error_but_no_DB_error(pEE, mql_query, "not allowing object reference usage across NOTEXIST. Test #1"))
		goto end;


	// NOTEXIST may not have STAR
	mql_query = "SELECT FOCUS OBJECTS WHERE [Phrase NOTEXIST[Word]*{1-3,5-7}]GO";
	if (!exec_string_expect_compiler_error_but_no_DB_error(pEE, mql_query, "not allowing a NOTEXIST to have Kleene Star. Test #1."))
		goto end;

	// There may only be an object block after a power block...
	mql_query = "SELECT FOCUS OBJECTS WHERE [Phrase [Word]..[Word]]GO";
	if (!exec_string(pEE, mql_query, "allowing an object block after a power block. Test #1."))
		goto end;

	// There may only be an object block after a power block...
	mql_query = "SELECT FOCUS OBJECTS WHERE [Phrase [Word]..[Word]*]GO";
	if (!exec_string(pEE, mql_query, "allowing an object block after a power block. Test #2."))
		goto end;

	// Using OR
	mql_query = "SELECT FOCUS OBJECTS WHERE [Phrase [Word]..[Word]or[phrase]]GO";
	if (!exec_string(pEE, mql_query, "using OR. Test #1"))
		goto end;

	// Using OR #2
	mql_query = "SELECT FOCUS OBJECTS WHERE [Phrase as p1 [Phrase number=p1.number]or[Word]]GO";
	if (!exec_string(pEE, mql_query, "using OR. Test #2"))
		goto end;

	// Using OR #3
	mql_query = "SELECT FOCUS OBJECTS WHERE [Phrase as p1 [Phrase number=p1.number]]or[Word]GO";
	if (!exec_string(pEE, mql_query, "using OR. Test #3"))
		goto end;

	// Using OR #4
	mql_query = "SELECT FOCUS OBJECTS WHERE [Phrase as p1 [Phrase [Phrase number=p1.number]]or[Word]]";
	if (!exec_string(pEE, mql_query, "using OR. Test #4"))
		goto end;

	// Using OR #5
	mql_query = "SELECT FOCUS OBJECTS WHERE [Phrase as p1 [Phrase]or[Phrase [Phrase number=p1.number]]]";
	if (!exec_string(pEE, mql_query, "using OR. Test #5"))
		goto end;

	// Using NOTEXIST with object references inside: This should BE ALLOWED!
	mql_query = "SELECT FOCUS OBJECTS WHERE NOTEXIST [Phrase [Phrase AS p1]..[Phrase number=p1.number]]";
	if (!exec_string(pEE, mql_query, "using NOTEXIST with object references inside."))
		goto end;

	if (!exec_string_check_counts(pEE, mql_query, "phrase", false, 0, 0, 0, "testing counts of objects and straws 2.1")) {
		bAllOK = false;
		goto end;
	}
	if (!exec_string_check_counts(pEE, mql_query, "phrase", true, 0, 0, 0, "testing counts of objects and straws 2.2")) {
		bAllOK = false;
		goto end;
	}
	if (!exec_string_check_counts(pEE, mql_query, "pow_m", false, 0, 0, 0, "testing counts of objects and straws 2.3")) {
		bAllOK = false;
		goto end;
	}
	if (!exec_string_check_counts(pEE, mql_query, "", false, 0, 0, 0, "testing counts of objects and straws 2.4")) {
		bAllOK = false;
		goto end;
	}
	if (!exec_string_check_counts(pEE, mql_query, "", true, 0, 0, 0, "testing counts of objects and straws 2.5")) {
		bAllOK = false;
		goto end;
	}
	

	// We may not define object references across OR...: SHOULD FAIL!
	mql_query = "SELECT ALL OBJECTS WHERE [Word AS w1]or[Word surface=w1.surface]GO";
	if (!exec_string_expect_compiler_error_but_no_DB_error(pEE, mql_query, "not allowing object reference usage across OR. Test #1"))
		goto end;

	// We may not define object references across OR...: SHOULD FAIL!
	mql_query = "SELECT ALL OBJECTS WHERE [Phrase [word as w1]]or[Word surface=w1.surface]GO";
	if (!exec_string_expect_compiler_error_but_no_DB_error(pEE, mql_query, "not allowing object reference usage across OR. Test #2"))
		goto end;

	// We may not define object references across OR...: SHOULD FAIL!
	mql_query = "SELECT ALL OBJECTS WHERE [Word as w1]or[word]or[Word surface=w1.surface]GO";
	if (!exec_string_expect_compiler_error_but_no_DB_error(pEE, mql_query, "not allowing object reference usage across OR. Test #3"))
		goto end;

	// We may not define object references across OR...: SHOULD FAIL!
	mql_query = "SELECT ALL OBJECTS WHERE [Phrase [Word as w1]]or[word]or[Word surface=w1.surface]GO";
	if (!exec_string_expect_compiler_error_but_no_DB_error(pEE, mql_query, "not allowing object reference usage across OR. Test #4"))
		goto end;

	// We may not define object references across OR...: SHOULD FAIL!
	mql_query = "SELECT ALL OBJECTS WHERE [Word]or[Phrase [Word as w1]]or[word]or[Word surface=w1.surface]GO";
	if (!exec_string_expect_compiler_error_but_no_DB_error(pEE, mql_query, "not allowing object reference usage across OR. Test #5"))
		goto end;

	// We may not define object references across OR...: SHOULD FAIL!
	mql_query = "SELECT ALL OBJECTS WHERE [Word]or[Phrase [Word as w1]]or[Phrase [Word surface=w1.surface]]GO";
	if (!exec_string_expect_compiler_error_but_no_DB_error(pEE, mql_query, "not allowing object reference usage across OR. Test #6"))
		goto end;


	// Delete object
	mql_query = "DELETE OBJECTS\n"
		"BY MONADS = { 1-2, 4-5 }\n"
		"[Word] GO";
	if (!exec_string_check_no_of_rows(pEE, mql_query, "deleting objects by monads from Word", 2))
		goto end;

	// min_m
	mql_query = "SELECT MIN_M\n"
		"GO";
	if (!exec_string_check_monad(pEE, mql_query, "checking min_m", 1))
		goto end;
  
	// max_m
	mql_query = "SELECT MAX_M\n"
		"GO";
	if (!exec_string_check_monad(pEE, mql_query, "checking max_m", 1110))
		goto end;

	// Delete object
	mql_query = "DELETE OBJECTS\n"
		"BY ID_D = " + id_d2string(object3) + "\n" 
		"[Phrase] GO";
	if (!exec_string_check_no_of_rows(pEE, mql_query, "deleting object by id_ds", 1))
		goto end;

	// Delete object
	mql_query = "DELETE OBJECTS\n"
		"BY MONADS = { 1-11 }\n"
		"[Word] GO";
	if (!exec_string_check_no_of_rows(pEE, mql_query, "deleting objects by monads from Word", 1))
		goto end;

	// min_m
	mql_query = "SELECT MIN_M\n"
		"GO";
	if (!exec_string_check_monad(pEE, mql_query, "checking min_m", 1000))
		goto end;
  
	// max_m
	mql_query = "SELECT MAX_M\n"
		"GO";
	if (!exec_string_check_monad(pEE, mql_query, "checking max_m", 1110))
		goto end;

	// Create objects
	mql_query = "CREATE OBJECTS WITH OBJECT TYPE [Word]\n"
		"CREATE OBJECT FROM MONADS = { 10-11 }\n"
		"[ surface := 'blah';\n"
		"  lemma := 'blat';\n"
		"  half_verse := 'A';\n" 
		"  psp := proper_noun;\n"
		"  psplist := ();\n"
		"  number := 1;\n"
		"]\n"
		"CREATE OBJECT FROM MONADS = { 14 }\n"
		"[ surface := 'blah';\n"
		"  lemma := 'blat';\n"
		"  half_verse := 'A';\n" 
		"  psp := proper_noun;\n"
		"  psplist := ();\n"
		"  number := 4;\n"
		"]\n"
		// OK, perhaps an unrealistic monad-set for a word, but
		// I need to exercise the machinery
		"CREATE OBJECT FROM MONADS = { 12-13, 15-80, 90-99 }\n"
		"[ number := 2; ]\n"
		"CREATE OBJECT FROM MONADS = { 10000 }\n"
		"[ surface := 'blah'; number := 3;\n"
		"]\n"
		"GO";
	long result_long;
	if (!exec_string_get_long(pEE, mql_query, "batch-creating objects", result_long))
		goto end;
	if (result_long == 4) {
		std::cout << "SUCCESS: Batch-creation created 4 objects, as it should." << std::endl;
	} else {
		std::cout << "ERROR: Batch-creation created " << result_long << " objects, but it should have created 4." << std::endl;
		bAllOK = false;
		goto end;
	}

	// min_m
	mql_query = "SELECT MIN_M\n"
		"GO";
	if (!exec_string_check_monad(pEE, mql_query, "checking min_m", 10))
		goto end;
  
	// max_m
	mql_query = "SELECT MAX_M\n"
		"GO";
	if (!exec_string_check_monad(pEE, mql_query, "checking max_m", 10000))
		goto end;


	// min_m
	mql_query = "SELECT ALL OBJECTS IN {10-99} WHERE [Word FOCUS number IN (1,2)] OR [Word number IN (3,4)]"
		"GO";
	if (!exec_string(pEE, mql_query, "SELECT ALL OBJECTS with IN on integer-list"))
		goto end;


	if (!exec_string_check_counts(pEE, mql_query, "word", false, 3, 3, 3, "testing counts of objects and straws 3.1")) {
		bAllOK = false;
		goto end;
	}
	if (!exec_string_check_counts(pEE, mql_query, "word", true, 2, 2, 3, "testing counts of objects and straws 3.2")) {
		bAllOK = false;
		goto end;
	}
	if (!exec_string_check_counts(pEE, mql_query, "pow_m", false, 3, 0, 3, "testing counts of objects and straws 3.3")) {
		bAllOK = false;
		goto end;
	}
	if (!exec_string_check_counts(pEE, mql_query, "", false, 3, 3, 3, "testing counts of objects and straws 3.4")) {
		bAllOK = false;
		goto end;
	}
	if (!exec_string_check_counts(pEE, mql_query, "", true, 2, 2, 3, "testing counts of objects and straws 3.5")) {
		bAllOK = false;
		goto end;
	}
	
	
  
	// Create object with id_d
	mql_query = "CREATE OBJECT\n"
		"FROM MONADS = { 1-10 }\n"
		"WITH ID_D = 12000\n"
		"[Phalanges]\n"
		"GO";
	id_d_t phalanges_object_id_d;
	if (!exec_string_get_id_d(pEE, mql_query, "creating object from monads with specific id_d", phalanges_object_id_d))
		goto end;

	if (phalanges_object_id_d == 12000) {
		std::cout << "SUCCESS: Phalanges object had the correct id_d." << std::endl;
	} else {
		std::cerr << "FAILURE: Phalanges object did not have the correct id_d." << std::endl;
		std::cerr << "         The id_d was: " << phalanges_object_id_d << std::endl;
		bAllOK = false;
		goto end;
	}

	// Select features
	mql_query = 
		"SELECT FEATURES FROM OBJECT TYPE [Phalanges] GO";
	if (!exec_string_check_no_of_rows(pEE, mql_query, "selecting features", 7))
		goto end;
	// Check that next auto-generated id_d is correct
	mql_query = "CREATE OBJECT\n"
		"FROM MONADS = { 1-11 }\n"
		"[Phalanges]\n"
		"GO";
	if (!exec_string_get_id_d(pEE, mql_query, "creating object from monads without specific id_d", phalanges_object_id_d))
		goto end;

	if (phalanges_object_id_d == 12001) {
		std::cout << "SUCCESS: Object had the correct auto-generated id_d." << std::endl;
	} else {
		std::cerr << "FAILURE: Object did not have the correct auto-generated id_d." << std::endl;
		std::cerr << "         The id_d was: " << phalanges_object_id_d << std::endl;
		bAllOK = false;
		goto end;
	}

	// Update object type
	mql_query = 
		"UPDATE OBJECT TYPE\n"
		"[Phalanges\n"
		"  ADD fax : ASCII(32) DEFAULT \"111-111-1111\";\n"
		"  ADD fax2 : STRING(32) DEFAULT \"222-222-2222\";\n"
		"  ADD foo : INTEGER DEFAULT 3333;\n"
		"  ADD foo2 : ID_D DEFAULT 10001;\n"
		"  ADD foo3 : psp_t DEFAULT proper_noun;\n"
		"  ADD foo4 : LIST OF integer;\n"
		"  ADD foo5 : LIST OF id_d;\n"
		"  ADD foo6 : LIST OF psp_t;\n"
		"  REMOVE fog;\n"
		"  REMOVE fog2;\n"
		"  REMOVE surface;\n"
		"  REMOVE crasis;\n"
		"  REMOVE idUP;\n"
		"]\n"
		" GO";
	if (!exec_string(pEE, mql_query, "updating object type Phalanges"))
		goto end;

	// Select features
	mql_query = 
		"SELECT FEATURES FROM OBJECT TYPE [Phalanges] GO";
	if (!exec_string_check_no_of_rows(pEE, mql_query, "selecting features", 10))
		goto end;

	// Test getting feature fax from 12001, which was added through UPDATE
	// OBJECT TYPE.  Before 1.2.0.pre70, this would give a database error
	// on MySQL because the DB engine would insert NULL.
	mql_query = "GET FEATURES fax\n"
		"FROM OBJECTS WITH ID_DS = 12001\n"
		"[Phalanges]\n"
		"GO";
	if (!exec_string_get_string(pEE, mql_query, "getting feature fax from Phalanges, which was added through UPDATE OBJECT TYPE", 2, my_string3))
		goto end;
	if (my_string3 == "111-111-1111") {
		std::cout << "SUCCESS: default value of fax was correct." << std::endl;
	} else {
		std::cerr << "FAILURE: Default value of fax was incorrect." << std::endl;
		std::cerr << "         The value was: '" << my_string3 << "'." << std::endl;
		bAllOK = false;
		goto end;
	}

	// Test getting feature fax2 from 12001, which was added through UPDATE
	// OBJECT TYPE.  Before 1.2.0.pre70, this would give a database error
	// on MySQL because the DB engine would insert NULL.
	mql_query = "GET FEATURES fax2\n"
		"FROM OBJECTS WITH ID_DS = 12001\n"
		"[Phalanges]\n"
		"GO";
	if (!exec_string_get_string(pEE, mql_query, "getting feature fax2 from Phalanges, which was added through UPDATE OBJECT TYPE", 2, my_string3))
		goto end;
	if (my_string3 == "222-222-2222") {
		std::cout << "SUCCESS: default value of fax2 was correct." << std::endl;
	} else {
		std::cerr << "FAILURE: Default value of fax2 was incorrect." << std::endl;
		std::cerr << "         The value was: '" << my_string3 << "'." << std::endl;
		bAllOK = false;
		goto end;
	}

	// Test getting feature foo from 12001, which was added through UPDATE
	// OBJECT TYPE.  Before 1.2.0.pre70, this would give a database error
	// on MySQL because the DB engine would insert NULL.
	mql_query = "GET FEATURES foo\n"
		"FROM OBJECTS WITH ID_DS = 12001\n"
		"[Phalanges]\n"
		"GO";
	if (!exec_string_get_string(pEE, mql_query, "getting feature foo from Phalanges, which was added through UPDATE OBJECT TYPE", 2, my_string3))
		goto end;
	if (my_string3 == "3333") {
		std::cout << "SUCCESS: default value of foo was correct." << std::endl;
	} else {
		std::cerr << "FAILURE: Default value of foo was incorrect." << std::endl;
		std::cerr << "         The value was: '" << my_string3 << "'." << std::endl;
		bAllOK = false;
		goto end;
	}

	// Test getting feature foo2 from 12001, which was added through UPDATE
	// OBJECT TYPE.  Before 1.2.0.pre70, this would give a database error
	// on MySQL because the DB engine would insert NULL.
	mql_query = "GET FEATURES foo2\n"
		"FROM OBJECTS WITH ID_DS = 12001\n"
		"[Phalanges]\n"
		"GO";
	if (!exec_string_get_string(pEE, mql_query, "getting feature foo2 from Phalanges, which was added through UPDATE OBJECT TYPE", 2, my_string3))
		goto end;
	if (my_string3 == "10001") {
		std::cout << "SUCCESS: default value of foo2 was correct." << std::endl;
	} else {
		std::cerr << "FAILURE: Default value of foo2 was incorrect." << std::endl;
		std::cerr << "         The value was: '" << my_string3 << "'." << std::endl;
		bAllOK = false;
		goto end;
	}

	// Test getting feature foo3 from 12001, which was added through UPDATE
	// OBJECT TYPE.  Before 1.2.0.pre70, this would give a database error
	// on MySQL because the DB engine would insert NULL.
	mql_query = "GET FEATURES foo3\n"
		"FROM OBJECTS WITH ID_DS = 12001\n"
		"[Phalanges]\n"
		"GO";
	if (!exec_string_get_string(pEE, mql_query, "getting feature foo3 from Phalanges, which was added through UPDATE OBJECT TYPE", 2, my_string3))
		goto end;
	if (my_string3 == "proper_noun") {
		std::cout << "SUCCESS: default value of foo3 was correct." << std::endl;
	} else {
		std::cerr << "FAILURE: Default value of foo3 was incorrect." << std::endl;
		std::cerr << "         The value was: '" << my_string3 << "'." << std::endl;
		std::cerr << "         The value should have been: '" << "proper_noun" << "'." << std::endl;
		bAllOK = false;
		goto end;
	}


	// Create objects
	mql_query = "CREATE OBJECTS WITH OBJECT TYPE [Phalanges]\n"
		"CREATE OBJECT FROM MONADS = { 10-11 }\n"
		"WITH ID_D = 13000\n"
		"[]\n"
		"CREATE OBJECT FROM MONADS = { 10-12 }\n"
		"WITH ID_D = 13002\n"
		"[]\n"
		"CREATE OBJECT FROM MONADS = { 10-12 }\n"
		"WITH ID_D = 13001\n"
		"[]\n"
		"GO";
	if (!exec_string_get_long(pEE, mql_query, "batch-creating objects", result_long))
		goto end;
	if (result_long == 3) {
		std::cout << "SUCCESS: Batch-creation created 3 objects, as it should." << std::endl;
	} else {
		std::cout << "ERROR: Batch-creation created " << result_long << " objects, but it should have created 3." << std::endl;
		bAllOK = false;
		goto end;
	}

	// Check that next auto-generated id_d is correct
	mql_query = "CREATE OBJECT\n"
		"FROM MONADS = { 1-14 }\n"
		"[Phalanges]\n"
		"GO";
	if (!exec_string_get_id_d(pEE, mql_query, "creating object from monads without specific id_d", phalanges_object_id_d))
		goto end;

	if (phalanges_object_id_d == 13003) {
		std::cout << "SUCCESS: Object had the correct auto-generated id_d." << std::endl;
	} else {
		std::cerr << "FAILURE: Object did not have the correct auto-generated id_d." << std::endl;
		std::cerr << "         The id_d was: " << phalanges_object_id_d << std::endl;
		bAllOK = false;
		goto end;
	}

	// Drop object type
	mql_query = 
		"DROP OBJECT TYPE\n"
		"[Word] GO";
	if (!exec_string(pEE, mql_query, "dropping object type Word"))
		goto end;

	mql_query = 
		"CREATE OBJECT\n"
		"FROM MONADS = {1-2,6-9}\n"
		"[Sploinks] GO";
	if (!exec_string_expect_compiler_error_but_no_DB_error(pEE, mql_query, "not creating Sploinks object with multiple range monad set."))
		goto end;

	mql_query = 
		"CREATE OBJECTS\n"
		"WITH OBJECT TYPE [Sploinks]"
		"CREATE OBJECT FROM MONADS = {6-7,16-19}\n"
		"[]\n"
		"CREATE OBJECT FROM MONADS = {6-7,10-13}\n"
		"[] GO";
	if (!exec_string_expect_compiler_error_but_no_DB_error(pEE, mql_query, "not creating Sploinks objects with multiple range monad sets."))
		goto end;

	mql_query = 
		"CREATE OBJECT\n"
		"FROM MONADS = {1-2,6-9}\n"
		"[SploinksSingleMonad] GO";
	if (!exec_string_expect_compiler_error_but_no_DB_error(pEE, mql_query, "not creating SploinksSingleMonad object with multiple range monad set."))
		goto end;

	mql_query = 
		"CREATE OBJECT\n"
		"FROM MONADS = {6-9}\n"
		"[SploinksSingleMonad] GO";
	if (!exec_string_expect_compiler_error_but_no_DB_error(pEE, mql_query, "not creating SploinksSingleMonad object with non-singleton single range monad set."))
		goto end;

	mql_query = 
		"CREATE OBJECT\n"
		"FROM MONADS = {1}\n"
		"[SploinksSingleMonad] GO";
	if (!exec_string(pEE, mql_query, "craeting single monad object SploinksSingleMonad."))
		goto end;

	mql_query = 
		"CREATE OBJECTS\n"
		"WITH OBJECT TYPE [Phrase]\n"
		"CREATE OBJECT FROM MONADS = {20000,20005-20007}\n"
		"[dummy := 'D';]\n"
		"CREATE OBJECT FROM MONADS = {20010,20014-20017}\n"
		"[dummy := 'D';] GO";
	if (!exec_string(pEE, mql_query, "creating phrase objects."))
		goto end;

	mql_query = 
		"CREATE OBJECTS\n"
		"WITH OBJECT TYPE [Phalanges]"
		"CREATE OBJECT FROM MONADS = {20000-20007}\n"
		"[]\n"
		"CREATE OBJECT FROM MONADS = {20010-20017}\n"
		"[] GO";
	if (!exec_string(pEE, mql_query, "creating phalanges objects."))
		goto end;

	mql_query = 
		"SELECT ALL OBJECTS\n"
		"WHERE\n"
		"[Phalanges\n"
		"  [Phrase dummy = 'D']\n"
		"  ..\n"
		"  [Phrase dummy = 'D']\n"
		"]GO";
	if (!exec_string_take_over_sheaf(pEE, &pSheaf2, mql_query, "executing topographic test query."))
		goto end;
	bIsFail = pSheaf2->isFail();
	delete pSheaf2;
	if (bIsFail) {
		std::cout << "SUCCESS: sheaf was a failed sheaf, as it should be." << std::endl;
	} else {
		std::cerr << "FAILURE: sheaf was not a failed sheaf, as it should be." << std::endl;
		bAllOK = false;
		goto end;
	}

	id_d_t AEOEAA_id_d;
	mql_query = 
		"CREATE OBJECT FROM MONADS={10000} [Sploinks lemma := '\xc3\xa6';]";
	if (!exec_string_get_id_d(pEE, mql_query, "creating Sploinks object with 'זרו'.", AEOEAA_id_d))
		goto end;

	mql_query =
		"GET FEATURES lemma FROM OBJECTS WITH ID_DS = " + id_d2number_string(AEOEAA_id_d)
		+ "[Sploinks]";
	if (!exec_string_get_string(pEE, mql_query, "Getting feature from Sploinks object with '\xc3\xa6'.", 2, AEOEAA_string))
		goto end;
	if (AEOEAA_string != "\xc3\xa6") {
		bAllOK = false;
		std::cerr << "ERROR: GET FEATURES did not return the correct STRING:\n"
			  << "       Expected '\xc3\xa6' but got '" << AEOEAA_string << "'" << std::endl;
		goto end;
	} else {
		std::cout << "SUCCESS: GET FEATURES did return the correct 'זרו' STRING." << std::endl;
	}

	// Create something with lists of ID_Ds and lists of integers
	mql_query = 
		"CREATE OBJECTS WITH OBJECT TYPE [Phrase]\n"
		"CREATE OBJECT FROM MONADS = {100000-100005}\n"
		"WITH ID_D = 13000001\n"
		"[\n"
		"  parents := ();\n"
		"  pages := ();\n"
		"  functions := ();\n"
		"  function := fn_Adjunct;\n"
		"  number := 1;\n"
		"]\n"
		"CREATE OBJECT FROM MONADS = {100001-100004}\n"
		"WITH ID_D = 13000002\n"
		"[\n"
		"  parents := (13000001);\n"
		"  pages := (3,2,4);\n"
		"  functions := (fn_Subject, fn_Object, fn_Predicate);\n"
		"  function := fn_Subject;\n"
		"  number := 2;\n"
		"]\n"
		"CREATE OBJECT FROM MONADS = {100002-100003}\n"
		"WITH ID_D = 13000003\n"
		"[\n"
		"  parents := (13000001, 13000002);\n"
		"  pages := (21);\n"
		"  functions := (fn_Predicate);\n"
		"  function := fn_Subject;\n"
		"  number := 3;\n"
		"]\n"
		"CREATE OBJECT FROM MONADS = {100003-100003}\n"
		"WITH ID_D = 13000004\n"
		"[\n"
		"  parents := (13000001, 13000002, 13000003);\n"
		"  pages := ();\n"
		"  functions := (fn_Predicate, fn_Object);\n"
		"  function := fn_Complement;\n"
		"  number := 4;\n"
		"]\n"
		"CREATE OBJECT FROM MONADS = {100005-100005}\n"
		"WITH ID_D = 13000005\n"
		"[\n"
		"  parents := (1,2,3,4,5,6,7,8,9,10);\n"
		"  pages := (21,-1,3,4,-2);\n"
		"  functions := (fn_Subject, fn_Object, fn_Predicate, fn_Adjunct, fn_Complement,\n"
		"  fn_Time, fn_Location);\n"
		"  function := fn_Predicate;\n"
		"  number := 5;\n"
		"]\n"
		"GO";
	if (!exec_string(pEE, mql_query, "creating phrases with lists of integers and id_ds"))
		goto end;

	mql_query =
		"GET OBJECTS HAVING MONADS IN {100000-100002}\n"
		"[Phrase GET parents, pages, functions]\n"
		"GO\n";
	if (!exec_string(pEE, mql_query, "GET OBJECTS HAVING MONADS IN with phrases with lists of integers and id_ds"))
		goto end;

	mql_query =
		"GET FEATURES parents, pages, functions FROM OBJECTS WITH ID_DS = 13000001, 13000002, 13000003, 13000004, 13000005\n"
		"[Phrase]\n"
		"GO\n";
	if (!exec_string(pEE, mql_query, "GET FEATURES with phrases with lists of integers and id_ds and enum constants"))
		goto end;

	mql_query =
		"SELECT ALL OBJECTS IN {100000-100005} WHERE\n"
		"[Phrase\n"
		"  self IN (13000002, 13000001)\n"
		"]\n" 
		"GO\n";
	expected_sheaf = 
		" //  <  < [ phrase 13000001 { 100000-100005 } false  //  <  > \n"
		" ]\n"
		" > \n"
		" < [ phrase 13000002 { 100001-100004 } false  //  <  > \n"
		" ]\n"
		" > \n"
		" > \n";
	if (!exec_string_test_sheaf(pEE, mql_query, expected_sheaf, "SELECT ALL OBJECTS with phrases with self IN explicit list of id_ds"))
		goto end;


	if (!exec_string_check_counts(pEE, mql_query, "phrase", false, 2, 2, 2, "testing counts of objects and straws 1.1")) {
		bAllOK = false;
		goto end;
	}
	if (!exec_string_check_counts(pEE, mql_query, "phrase", true, 0, 0, 2, "testing counts of objects and straws 1.2")) {
		bAllOK = false;
		goto end;
	}
	if (!exec_string_check_counts(pEE, mql_query, "pow_m", false, 2, 0, 2, "testing counts of objects and straws 1.3")) {
		bAllOK = false;
		goto end;
	}
	if (!exec_string_check_counts(pEE, mql_query, "", false, 2, 2, 2, "testing counts of objects and straws 1.4")) {
		bAllOK = false;
		goto end;
	}
	if (!exec_string_check_counts(pEE, mql_query, "", true, 0, 0, 2, "testing counts of objects and straws 1.5")) {
		bAllOK = false;
		goto end;
	}

	
	
	mql_query =
		"SELECT ALL OBJECTS IN {100000-100005} WHERE\n"
		"[Phrase\n"
		"  number IN (2,5,4)\n"
		"  GET number\n"
		"]\n" 
		"GO\n";
	expected_sheaf = 
		" //  <  < [ phrase 13000002 { 100001-100004 } false (number=\"2\") //  <  > \n"
		" ]\n"
		" > \n"
		" < [ phrase 13000004 { 100003 } false (number=\"4\") //  <  > \n"
		" ]\n"
		" > \n"
		" < [ phrase 13000005 { 100005 } false (number=\"5\") //  <  > \n"
		" ]\n"
		" > \n"
		" > \n";
	if (!exec_string_test_sheaf(pEE, mql_query, expected_sheaf,
				    "SELECT ALL OBJECTS with phrases with self IN explicit list of integer"))
		goto end;

	mql_query =
		"SELECT ALL OBJECTS IN {100000-100005} WHERE\n"
		"[Phrase\n"
		"  function IN (fn_Complement, fn_Predicate)\n"
		"  GET function\n"
		"]\n" 
		"GO\n";
	expected_sheaf = 
		" //  <  < [ phrase 13000004 { 100003 } false (function=\"fn_Complement\") //  <  > \n"
		" ]\n"
		" > \n"
		" < [ phrase 13000005 { 100005 } false (function=\"fn_Predicate\") //  <  > \n"
		" ]\n"
		" > \n"
		" > \n";
	if (!exec_string_test_sheaf(pEE, mql_query, expected_sheaf,
				    "SELECT ALL OBJECTS with phrases with self IN explicit list of enum const"))
		goto end;
	
	mql_query =
		"SELECT ALL OBJECTS IN {100000-100005} WHERE\n"
		"[Phrase AS p1\n"
		"  [Phrase parents HAS p1.self \n"
		"          GET parents\n"
		"  ]\n"
		"]\n" 
		"GO\n";
	expected_sheaf = 
		" //  <  < [ phrase 13000001 { 100000-100005 } false  //  <  < [ phrase 13000002 { 100001-100004 } false (parents=\" 13000001 \") //  <  > \n"
		" ]\n"
		" > \n"
		" < [ phrase 13000003 { 100002-100003 } false (parents=\" 13000001 13000002 \") //  <  > \n"
		" ]\n"
		" > \n"
		" < [ phrase 13000004 { 100003 } false (parents=\" 13000001 13000002 13000003 \") //  <  > \n"
		" ]\n"
		" > \n"
		" > \n"
		" ]\n"
		" > \n"
		" < [ phrase 13000002 { 100001-100004 } false  //  <  < [ phrase 13000003 { 100002-100003 } false (parents=\" 13000001 13000002 \") //  <  > \n"
		" ]\n"
		" > \n"
		" < [ phrase 13000004 { 100003 } false (parents=\" 13000001 13000002 13000003 \") //  <  > \n"
		" ]\n"
		" > \n"
		" > \n"
		" ]\n"
		" > \n"
		" < [ phrase 13000003 { 100002-100003 } false  //  <  < [ phrase 13000004 { 100003 } false (parents=\" 13000001 13000002 13000003 \") //  <  > \n"
		" ]\n"
		" > \n"
		" > \n"
		" ]\n"
		" > \n"
		" > \n";
	if (!exec_string_test_sheaf(pEE, mql_query, expected_sheaf,
				    "SELECT ALL OBJECTS with phrases with list of id_ds HAS atomic values from object reference"))
		goto end;

	mql_query =
		"SELECT ALL OBJECTS IN {100000-100005} WHERE\n"
		"[Phrase parents HAS 10\n"
		"        OR parents HAS 13000003\n"
		"        GET parents\n"
		"]\n" 
		"GO\n";
	expected_sheaf = 
		" //  <  < [ phrase 13000004 { 100003 } false (parents=\" 13000001 13000002 13000003 \") //  <  > \n"
		" ]\n"
		" > \n"
		" < [ phrase 13000005 { 100005 } false (parents=\" 1 2 3 4 5 6 7 8 9 10 \") //  <  > \n"
		" ]\n"
		" > \n"
		" > \n";
	if (!exec_string_test_sheaf(pEE, mql_query, expected_sheaf,
				    "SELECT ALL OBJECTS with phrases with list of id_ds HAS atomic values"))
		goto end;

	mql_query =
		"SELECT ALL OBJECTS IN {100000-100005} WHERE\n"
		"[Phrase pages HAS 2\n"
		"        OR pages HAS -1\n"
		"        GET pages\n"
		"]\n" 
		"GO\n";
	expected_sheaf = 
		" //  <  < [ phrase 13000002 { 100001-100004 } false (pages=\" 3 2 4 \") //  <  > \n"
		" ]\n"
		" > \n"
		" < [ phrase 13000005 { 100005 } false (pages=\" 21 -1 3 4 -2 \") //  <  > \n"
		" ]\n"
		" > \n"
		" > \n";
	if (!exec_string_test_sheaf(pEE, mql_query, expected_sheaf,
				    "SELECT ALL OBJECTS with phrases with list of integers HAS atomic values"))
		goto end;

	mql_query =
		"SELECT ALL OBJECTS IN {100000-100005} WHERE\n"
		"[Phrase functions HAS fn_Object\n"
		"        OR functions HAS fn_Subject\n"
		"        GET functions\n"
		"]\n" 
		"GO\n";
	expected_sheaf = 
		" //  <  < [ phrase 13000002 { 100001-100004 } false (functions=\" fn_Subject fn_Object fn_Predicate \") //  <  > \n"
		" ]\n"
		" > \n"
		" < [ phrase 13000004 { 100003 } false (functions=\" fn_Predicate fn_Object \") //  <  > \n"
		" ]\n"
		" > \n"
		" < [ phrase 13000005 { 100005 } false (functions=\" fn_Subject fn_Object fn_Predicate fn_Adjunct fn_Complement fn_Time fn_Location \") //  <  > \n"
		" ]\n"
		" > \n"
		" > \n";
	if (!exec_string_test_sheaf(pEE, mql_query, expected_sheaf,
				    "SELECT ALL OBJECTS with phrases with list of enum HAS atomic value of enum const"))
		goto end;

	mql_query =
		"SELECT ALL OBJECTS IN {100000-100005} WHERE\n"
		"[Phrase GET parents\n"
		"  [Phrase self IN (13000005)]\n"
		"]\n" 
		"GO\n";
	expected_sheaf = 
		" //  <  < [ phrase 13000001 { 100000-100005 } false (parents=\"\") //  <  < [ phrase 13000005 { 100005 } false  //  <  > \n"
		" ]\n"
		" > \n"
		" > \n"
		" ]\n"
		" > \n"
		" < [ phrase 13000005 { 100005 } false (parents=\" 1 2 3 4 5 6 7 8 9 10 \") //  <  < [ phrase 13000005 { 100005 } false  //  <  > \n"
		" ]\n"
		" > \n"
		" > \n"
		" ]\n"
		" > \n"
		" > \n";
	if (!exec_string_test_sheaf(pEE, mql_query, expected_sheaf,
				    "SELECT ALL OBJECTS with phrases with atomic values IN list of id_ds"))
		goto end;

	mql_query =
		"SELECT ALL OBJECTS IN {100000-100005}\n"
		"RETURNING FLAT SHEAF\n"
		"WHERE\n"
		"[Phrase AS p1 GET parents\n"
		"  [Phrase self IN (13000005)]\n"
		"]\n" 
		"GO\n";	
	expected_sheaf = 
		" // <  < [ phrase 13000001 { 100000-100005 } false (parents=\"\") //  <  > \n"
		" ]\n"
		"[ phrase 13000005 { 100005 } false  //  <  > \n"
		" ]\n"
		"[ phrase 13000005 { 100005 } false (parents=\" 1 2 3 4 5 6 7 8 9 10 \") //  <  > \n"
		" ]\n"
		"[ phrase 13000005 { 100005 } false  //  <  > \n"
		" ]\n"
		" > \n"
		" >\n";
	if (!exec_string_test_sheaf(pEE, mql_query, expected_sheaf,
				    "SELECT ALL OBJECTS with phrases with atomic values IN list of id_ds, RETURNING FLAT SHEAF."))
		goto end;

	// SELECT ALL OBJECTS with object reference usage on the same object block.
	mql_query = "SELECT ALL OBJECTS WHERE [Phrase AS p1 self = P1.number] GO";
	if (!exec_string_expect_compiler_error_but_no_DB_error(pEE, mql_query, "not allowing the same object reference usage on the same object block."))
		goto end;


	mql_query =
		"SELECT ALL OBJECTS IN {100000-100005} WHERE\n"
		"[Phrase functions HAS fn_Adjunct OR functions HAS fn_Object\n"
		"   GET functions\n"
		"]\n" 
		"GO\n";
	expected_sheaf = 
		" //  <  < [ phrase 13000002 { 100001-100004 } false (functions=\" fn_Subject fn_Object fn_Predicate \") //  <  > \n"
		" ]\n"
		" > \n"
		" < [ phrase 13000004 { 100003 } false (functions=\" fn_Predicate fn_Object \") //  <  > \n"
		" ]\n"
		" > \n"
		" < [ phrase 13000005 { 100005 } false (functions=\" fn_Subject fn_Object fn_Predicate fn_Adjunct fn_Complement fn_Time fn_Location \") //  <  > \n"
		" ]\n"
		" > \n"
		" > \n";
	if (!exec_string_test_sheaf(pEE, mql_query, expected_sheaf,
				    "SELECT ALL OBJECTS with phrases with lists of enums HAS enum const."))
		goto end;
  
	mql_query =
		"SELECT ALL OBJECTS IN {100000-100005}\n"
		"RETURNING FLAT SHEAF\n"
		"WHERE\n"
		"[Phrase functions HAS fn_Adjunct OR functions HAS fn_Object\n"
		"   GET functions\n"
		"]\n" 
		"GO\n";
	expected_sheaf = 
		" // <  < [ phrase 13000002 { 100001-100004 } false (functions=\" fn_Subject fn_Object fn_Predicate \") //  <  > \n"
		" ]\n"
		"[ phrase 13000004 { 100003 } false (functions=\" fn_Predicate fn_Object \") //  <  > \n"
		" ]\n"
		"[ phrase 13000005 { 100005 } false (functions=\" fn_Subject fn_Object fn_Predicate fn_Adjunct fn_Complement fn_Time fn_Location \") //  <  > \n"
		" ]\n"
		" > \n"
		" >\n";
	if (!exec_string_test_sheaf(pEE, mql_query, expected_sheaf,
				    "SELECT ALL OBJECTS with phrases with lists of enums HAS enum const, RETURNING FLAT SHEAF."))
		goto end;
  
	mql_query =
		"SELECT ALL OBJECTS IN {100000-100005} WHERE\n"
		"[Phrase AS p1 GET function\n"
		"  [Phrase functions HAS p1.function GET functions\n"
		"  ]\n"
		"]\n" 
		"GO\n";
	expected_sheaf = 
		" //  <  < [ phrase 13000001 { 100000-100005 } false (function=\"fn_Adjunct\") //  <  < [ phrase 13000005 { 100005 } false (functions=\" fn_Subject fn_Object fn_Predicate fn_Adjunct fn_Complement fn_Time fn_Location \") //  <  > \n"
		" ]\n"
		" > \n"
		" > \n"
		" ]\n"
		" > \n"
		" < [ phrase 13000002 { 100001-100004 } false (function=\"fn_Subject\") //  <  < [ phrase 13000002 { 100001-100004 } false (functions=\" fn_Subject fn_Object fn_Predicate \") //  <  > \n"
		" ]\n"
		" > \n"
		" > \n"
		" ]\n"
		" > \n"
		" < [ phrase 13000005 { 100005 } false (function=\"fn_Predicate\") //  <  < [ phrase 13000005 { 100005 } false (functions=\" fn_Subject fn_Object fn_Predicate fn_Adjunct fn_Complement fn_Time fn_Location \") //  <  > \n"
		" ]\n"
		" > \n"
		" > \n"
		" ]\n"
		" > \n"
		" > \n";
	if (!exec_string_test_sheaf(pEE, mql_query, expected_sheaf,
				    "SELECT ALL OBJECTS with phrases with lists of enums HAS object reference usage."))
		goto end;
  
	mql_query =
		"SELECT ALL OBJECTS IN {100000-100005}\n"
		"RETURNING FLAT SHEAF\n"
		"WHERE\n"
		"[Phrase AS p1 GET function\n"
		"  [Phrase functions HAS p1.function GET functions\n"
		"  ]\n"
		"]\n" 
		"GO\n";
	expected_sheaf = 
		" // <  < [ phrase 13000001 { 100000-100005 } false (function=\"fn_Adjunct\") //  <  > \n"
		" ]\n"
		"[ phrase 13000005 { 100005 } false (functions=\" fn_Subject fn_Object fn_Predicate fn_Adjunct fn_Complement fn_Time fn_Location \") //  <  > \n"
		" ]\n"
		"[ phrase 13000002 { 100001-100004 } false (function=\"fn_Subject\") //  <  > \n"
		" ]\n"
		"[ phrase 13000002 { 100001-100004 } false (functions=\" fn_Subject fn_Object fn_Predicate \") //  <  > \n"
		" ]\n"
		"[ phrase 13000005 { 100005 } false (function=\"fn_Predicate\") //  <  > \n"
		" ]\n"
		"[ phrase 13000005 { 100005 } false (functions=\" fn_Subject fn_Object fn_Predicate fn_Adjunct fn_Complement fn_Time fn_Location \") //  <  > \n"
		" ]\n"
		" > \n"
		" >\n";
	if (!exec_string_test_sheaf(pEE, mql_query, expected_sheaf,
				    "SELECT ALL OBJECTS with phrases with lists of enums HAS object reference usage, RETURNING FLAT SHEAF."))
		goto end;

	

	// Monad set calculation
	mql_query = 
		"MONAD SET CALCULATION\n"
		"{ 1-3, 6-8, 10-14 }\n"
		"UNION\n"
		"{ 4-7 }\n"
		"INTERSECT\n"
		"{ 2-13 }\n"
		"DIFFERENCE\n"
		"{ 2-3, 12 }\n"
		"GO";
	if (!exec_string(pEE, mql_query, "performing monad set calculation"))
		goto end;

	// Monad set calculation
	mql_query = 
		"MONAD SET CALCULATION\n"
		"{ 1-3, 6-8, 10-12 }\n"
		"UNION\n"
		"{ 4-7 }\n"
		"GO";
	if (!exec_string(pEE, mql_query, "performing monad set calculation"))
		goto end;

	// Vacuuming database
	mql_query = 
		"VACUUM DATABASE ANALYZE GO";
	if (!exec_string(pEE, mql_query, "vacuuming database"))
		goto end;

	// Const iterator on empty sheaf
	pSheaf1 = new Sheaf(false);
	ci1 = pSheaf1->const_iterator();
	if (ci1.hasNext() == false) {
		std::cout << "SUCCESS: const iterator on empty sheaf tested OK." << std::endl;
		delete pSheaf1;

	} else {
		std::cerr << "ERROR: const iterator on empty sheaf not right." << std::endl;
		delete pSheaf1;
		bAllOK = false;
		goto end;
	}

	// Const iterator on failed sheaf
	pSheaf1 = new Sheaf(true);
	ci1 = pSheaf1->const_iterator();
	if (ci1.hasNext() == false) {
		std::cout << "SUCCESS: const iterator on failed sheaf tested OK." << std::endl;
		delete pSheaf1;

	} else {
		std::cerr << "ERROR: const iterator on failed  sheaf not right." << std::endl;
		delete pSheaf1;
		bAllOK = false;
		goto end;
	}

	// Iterator on empty sheaf
	pSheaf1 = new Sheaf(false);
	i1 = pSheaf1->iterator();
	if (i1.hasNext() == false) {
		std::cout << "SUCCESS: iterator on empty sheaf tested OK." << std::endl;
		delete pSheaf1;

	} else {
		std::cerr << "ERROR: iterator on empty sheaf not right." << std::endl;
		delete pSheaf1;
		bAllOK = false;
		goto end;
	}

	// Iterator on failed sheaf
	pSheaf1 = new Sheaf(true);
	i1 = pSheaf1->iterator();
	if (i1.hasNext() == false) {
		std::cout << "SUCCESS: iterator on failed sheaf tested OK." << std::endl;
		delete pSheaf1;
	} else {
		std::cerr << "ERROR: iterator on failed  sheaf not right." << std::endl;
		delete pSheaf1;
		bAllOK = false;
		goto end;
	}

	// Test all in QueryAnswer array
	for (int qaindex = 0; strcmp_nocase(mqlqa[qaindex].m_query_name, "STOP") != 0; ++qaindex) {
		const QueryAnswer& qa = mqlqa[qaindex];
		if (!exec_QueryAnswer(pEE, qa, backend_kind, password)) {
			bAllOK = false;
			goto end;
		}
	}

 end:
	// Drop database
	mql_query = "DROP DATABASE " + get_db_name(backend_kind, "emdf4") + " GO";
	exec_string(pEE, mql_query, "dropping database emdf4");
 end1:
	// Drop database
	mql_query = "DROP DATABASE " + get_db_name(backend_kind, "emdf3") + " GO";
	exec_string(pEE, mql_query, "dropping database emdf3");

 end2:
	result = bAllOK ? 0 : 1;
	return result;
}

int test_backend(eBackendKind backend_kind, 
		 const std::string& hostname, 
		 const std::string& user, 
		 const std::string& password,
		 const std::string& initial_db)
{
	int result = 0;
	std::cout << "----------------------------------\n"
		  << "Testing backend " << backend_kind2string(backend_kind) << '\n'
		  << "----------------------------------" << std::endl;

	// Make execution environment
	EmdrosEnv *pEE;
	pEE = new EmdrosEnv(kOKConsole, kCSISO_8859_1,
			    hostname,
			    user, password,
			    initial_db,
			    backend_kind);

  
	// Test connection
	if (!pEE->connectionOk()) {
		std::cerr << "DB error message: '";
		std::cerr << pEE->getDBError() << "'" << std::endl;  

		emdrosMessageConnectionNotOK(&std::cerr, backend_kind);

		delete pEE;
		return 1;
	}


	try {
		result = testall(pEE, backend_kind, password);
	} catch (EMdFDBException e) {
		std::cerr << "ERROR: EMdFDBException (Database error)..." << std::endl;
		std::cerr << "message:\n"
			  << e.what()
			  << std::endl;
		std::cerr << pEE->getDBError() << std::endl; 
		std::cerr << pEE->getCompilerError() << std::endl;
		bAllOK = false;
	} catch (EmdrosException e) {
		std::cerr << "EmdrosException thrown:\n"
			  << e.what()
			  << std::endl;
		std::cerr << pEE->getDBError() << std::endl; 
		std::cerr << pEE->getCompilerError() << std::endl;
		bAllOK = false;
	} catch (...) {
		std::cerr << "ERROR: Unknown exception..." << std::endl;
		std::cerr << pEE->getDBError() << std::endl; 
		std::cerr << pEE->getCompilerError() << std::endl;
		bAllOK = false;
	}

	if (bAllOK) {
		std::cout << "SUCCESS: All tests completed successfully on backend " << backend_kind2string(backend_kind) << '.' << std::endl;
		result = 0;
	} else {
		std::cerr << "FAILURE: At least one test failed  on backend " << backend_kind2string(backend_kind) << '.' << std::endl;
		result = 1;
	}

	// Clean up
	delete pEE;
	return result;

}

int main(int argc, char* argv[])
{
	int result = 0;

	std::string hostname;
	std::string password;
	// We don't want to test a key with more bits, since that
	// would mean we could not export the Emdros tarball from the
	// US.
	std::string password_sqlite3_encryption_56_bit_sscanf = "0xe6963bce 0xba9829bb";
	std::string password_sqlite3_encryption_56_bit_char_based = "\x5c\x2c\x12\x26\xf0\x90\xd4";
	std::string user;
	std::string initial_db("emdf");
	eBackendKind backend_kind;
	eCharsets dummy_charset;
	bool bTestedSQLite3 = false;
	bool bTestedSQLite3EncryptionSScanf = false;
	bool bTestedSQLite3EncryptionCharBased = false;
	bool bTestedPostgreSQL = false;
	bool bTestedMySQL = false;

	addStandardArguments(false // Do not add -e | --encoding
			     );

	// Parse arguments
	std::list<std::string> surplus_arguments;

	std::string error_message;
	if (!parseArguments(argc, argv, error_message, surplus_arguments)) {
		print_usage(std::cerr);
		std::cerr << error_message << std::endl;
		return 1;
	} else {
		bool bShowVersion;
		bool bShowHelp;
		std::string error_message;
		if (!getStandardArguments(bShowVersion, bShowHelp,
					  hostname,
					  user,
					  password,
					  backend_kind,
					  dummy_charset,
					  error_message)) {
			print_usage(std::cerr);
			std::cerr << error_message << std::endl;
			return 1;
		}

		// -V | --version
		if (bShowVersion) {
			show_version(std::cout);
			return 0;
		}

		// --help
		if (bShowHelp) {
			print_usage(std::cout);
			return 0;
		}
		
		// Did the user specify a backend?
		if (!getArgumentPresent("-b")) {
			// No, so we say it's "kBackenNone" to indicate
			// further down that we want them all...
			backend_kind = kBackendNone;
		}

		if (surplus_arguments.size() != 0) {
			print_usage(std::cerr);
			std::cerr << "ERROR: Extraneous argument(s) not parsed: '" << joinList("' and '", surplus_arguments) << "'." << std::endl;
			return 1;
		}
	}



#if USE_SQLITE3
	if (result == 0
	    && (backend_kind == kBackendNone || backend_kind == kSQLite3)) {
		bTestedSQLite3 = true;
		result = test_backend(kSQLite3, hostname, user, password, initial_db);
		if (result != 0) {
			std::cerr << "FAILURE: At least one test failed." << std::endl;
		}
	}
	if (result == 0
	    && (backend_kind == kBackendNone || backend_kind == kSQLite3)) {
		bTestedSQLite3EncryptionCharBased = true;
		result = test_backend(kSQLite3, hostname, user, password_sqlite3_encryption_56_bit_char_based, initial_db);
		if (result != 0) {
			std::cerr << "FAILURE: At least one test failed." << std::endl;
		}
	}
	if (result == 0
	    && (backend_kind == kBackendNone || backend_kind == kSQLite3)) {
		bTestedSQLite3EncryptionSScanf = true;
		result = test_backend(kSQLite3, hostname, user, password_sqlite3_encryption_56_bit_sscanf, initial_db);
		if (result != 0) {
			std::cerr << "FAILURE: At least one test failed." << std::endl;
		}
	}
#endif
#if USE_POSTGRESQL
	if (result == 0
	    && (backend_kind == kBackendNone || backend_kind == kPostgreSQL)) {
		bTestedPostgreSQL = true;
		result = test_backend(kPostgreSQL, hostname, user, password, initial_db);
		if (result != 0) {
			std::cerr << "FAILURE: At least one test failed." << std::endl;
		}
	}
#endif
#if USE_MYSQL
	if (result == 0
	    && (backend_kind == kBackendNone || backend_kind == kMySQL)) {
		bTestedMySQL = true;
		result = test_backend(kMySQL, hostname, user, password, initial_db);
		if (result != 0) {
			std::cerr << "FAILURE: At least one test failed." << std::endl;
		}
	}
#endif

	std::cout << "Backends tested:" << std::endl;
	if (bTestedSQLite3) {
		std::cout << "                 - SQLite 3.X.X" << std::endl;
	}
	if (bTestedSQLite3EncryptionSScanf) {
		std::cout << "                 - SQLite 3.X.X with sscanf password" << std::endl;
	}
	if (bTestedSQLite3EncryptionCharBased) {
		std::cout << "                 - SQLite 3.X.X with char-based password" << std::endl;
	}
	if (bTestedPostgreSQL) {
		std::cout << "                 - PostgreSQL" << std::endl;
	}
	if (bTestedMySQL) {
		std::cout << "                 - MySQL" << std::endl;
	}

	if (result == 0) {
		std::cout << "SUCCESS: All tests completed successfully." << std::endl;
	} else {
		std::cerr << "FAILURE: At least one test failed." << std::endl;
	}

	return result;
}
