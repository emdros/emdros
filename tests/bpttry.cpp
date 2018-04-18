/*
 * bpttry.cpp
 *
 * Trial of MQL and EMdF framework
 * Created: 3/6-2001 (March 6, 2001)
 * Last update: 4/3-2018
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


#include <emdros-lconfig.h>
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
#include <bptdumper.h>
#include <bpt2dumper.h>
#include <bptemdfdb.h>
#include <bpt2emdfdb.h>
#include "bptqa.h"


static bool bAllOK; // Global variable -- ugly, but it works
static bool bSomethingTested;

void show_version(std::ostream& ostr)
{
	ostr << "bpttry from Emdros version " << EMDROS_VERSION << std::endl;
}

void print_usage(std::ostream& ostr)
{
	ostr << "Usage:" << std::endl;
	ostr << "   bpttry [options] " << std::endl;
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

std::string getBPTDBName(long BPT_SCHEMA)
{
	return std::string("bptschema") + long2string(BPT_SCHEMA);
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
  bool bResult = true;
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


bool exec_newdb(EmdrosEnv *pEE)
{
	// Drop database emdf3
	std::string mql_query = "DROP DATABASE emdf3 GO";
	exec_string_ignore_error(pEE, mql_query, "dropping database emdf3");

	// Create database
	mql_query = "CREATE DATABASE emdf3 GO";
	if (!exec_string(pEE, mql_query, "creating database emdf3"))
		return false;

	// Use database
	mql_query = "USE DATABASE emdf3 GO";
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

bool exec_dumpDB(EmdrosEnv *pEE, eBackendKind BPT_backend_kind, const std::string& password_bpt, long BPT_SCHEMA, EmdrosEnv *pBPTEE)
{
	std::string db_name = getBPTDBName(BPT_SCHEMA);

	// Drop database bptschema.
	// Currently, this does nothing, since dropping a database is
	// not implemented in the BPTEMdF class.
	// Hence, the file is not deleted, and what is more, the BPT
	// EmdrosEnv still has a pointer to an open file.
	
	std::string mql_query = "DROP DATABASE " + db_name + " GO";
	exec_string_ignore_error(pBPTEE, mql_query, "dropping database " + db_name);

	// ... Therefore, we need to make sure we close the file
	// inside of the *pBPTEE EmdrosEnv.  This is done by
	// connecting to the pseudo-database, 'emdf'.
	mql_query = "USE DATABASE emdf GO";
	if (!exec_string(pBPTEE, mql_query, "using database emdf"))
		return false;
	

	std::string error_message;
	bool bResult = false;
	if (BPT_backend_kind == kBPT) {
		bResult = BPTdumpAsBPT(pEE, 
				       db_name, 
				       password_bpt,  // key
				       error_message, 
				       BPT_SCHEMA, 
				       "", // payload filename
				       "", // payload key
				       ""); // payload codec
	} else if (BPT_backend_kind == kBPT2) {
		bResult = BPT2dumpAsBPT2(pEE, 
					 db_name, 
					 password_bpt,  // key
					 error_message, 
					 BPT_SCHEMA, 
					 "", // payload filename
					 ""); // payload key
	} else {
		ASSERT_THROW(false,
			     "Unknown BPT_backend_kind.");
	}
	if (!bResult) {
		std::cerr << "ERROR dumping emdf3 as " << db_name << ". Error message:\n" << error_message << std::endl;
		return false;
	}

	if (password_bpt.empty()) {
		mql_query = "USE DATABASE " + db_name + " GO";
	} else {
		mql_query = "USE DATABASE " + db_name + " WITH KEY '" + password_bpt + "' GO";
	}
	if (!exec_string(pBPTEE, mql_query, "using database " + db_name))
		return false;

	return true;
}

bool exec_execQueryAnswerOnDB(EmdrosEnv *pEE,
			      const QueryAnswer& qa,
			      const std::string& db_name)
{
	std::string success = qa.m_query_name;
	std::string mql_query = qa.m_query;

	bool bResult = false;

	if (!pEE->executeString(mql_query, bResult, true, false)) {
		if (qa.m_bDBSuccessExpected) {
			DB_ERROR;
			std::cerr << "... on DB: " << db_name << std::endl;
			bResult= false;
		} else {
			std::cout << "SUCCESS: " << success << " on DB " << db_name << std::endl;
			bResult = true;
		}
	} else {
		if (!bResult) {
			if (qa.m_bCompilerSuccessExpected) {
				COMPILE_ERROR;
				std::cerr << "... on DB: " << db_name << std::endl;
				bResult= false;
			} else {
				if (pEE->getCompilerError() != qa.m_answer) {
					std::cerr << "FAILURE: " << success << " made a compiler failure, which it should." << " on DB " << db_name << std::endl;
					std::cerr << "         However, the expected output was:'" << qa.m_answer << "', but it was in fact '" << pEE->getCompilerError() << "'" <<  " on DB " << db_name << std::endl;
					bResult = false;
				} else {
					std::cout << "SUCCESS: " << success << " made a compiler failure, which it should!" <<  " on DB " << db_name << std::endl;
					std::cout << "SUCCESS: The error was: '" << pEE->getCompilerError() << "'" <<  " on DB " << db_name << std::endl;
					bResult = true;
				}
			}
		} else { 
			if (!qa.m_bCompilerSuccessExpected) {
				std::cerr << "FAILURE: " << success << " did not fail with a compiler error, as expected!" <<  " on DB " << db_name << std::endl;
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
					std::cout << "SUCCESS in " << success <<  " on DB " << db_name << std::endl;
					bResult= true;
				} else {	
					std::cerr << "ERROR: Expecting result to be '\n" << qa.m_answer << "' but it was '\n" << ostr.str() << "'." <<  " on DB " << db_name << std::endl;
					bResult = false;
					QUERY_WAS;
					bAllOK = false;
				}
			}
		}
	}

	return bResult;
}


bool exec_QueryAnswer(EmdrosEnv *pEE, eBackendKind BPT_backend_kind, const std::string& password_bpt, const std::vector<long>& BPT_SCHEMA_vec, std::vector<EmdrosEnv*>& BPTEE_vec, const QueryAnswer& qa)
{	
	unsigned int index;
	bool bResult = true;
	std::string success = qa.m_query_name;
	if (qa.m_bBPTDumpDB) {
		for (index = 0;
		     index < BPT_SCHEMA_vec.size();
		     ++index) {
			long BPT_SCHEMA = BPT_SCHEMA_vec[index];
			EmdrosEnv *pBPTEE = BPTEE_vec[index];
			bool bThisResult = exec_dumpDB(pEE, BPT_backend_kind, password_bpt, BPT_SCHEMA, pBPTEE);
			bResult = bResult && bThisResult;
			
			// Clean up
			pEE->clean();
			pBPTEE->clean();
			
		}

		// Return
		return bResult;
	}
	
	if (qa.m_bNewDB) {
		if (!exec_newdb(pEE)) {
			std::cerr << "ERROR: Could not create new DB for " << success << std::endl;
			return false;
		}
		
		// Clean up
		pEE->clean();
	}
	
	bResult = exec_execQueryAnswerOnDB(pEE, qa, "emdf3");
	if (!qa.m_bNoBPT) {
		for (index = 0;
		     index < BPT_SCHEMA_vec.size();
		     ++index) {
			long BPT_SCHEMA = BPT_SCHEMA_vec[index];
			EmdrosEnv *pBPTEE = BPTEE_vec[index];
			if (bResult) {
				bResult = exec_execQueryAnswerOnDB(pBPTEE, qa, getBPTDBName(BPT_SCHEMA));
			}
		}
	}
	
	// Clean up
	pEE->clean();
	for (index = 0;
	     index < BPTEE_vec.size();
	     ++index) {
		EmdrosEnv *pBPTEE = BPTEE_vec[index];
		pBPTEE->clean();
	}

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


int testBPTEngine(EmdrosEnv *pEE, int nBPTVersion, const std::string& password_bpt)
{
	std::list<QueryAnswer*>::iterator it = bptqa_list.begin();
	std::list<QueryAnswer*>::iterator itend = bptqa_list.end();

	std::string mql_query;
	std::string expected_sheaf;
	int result = 0;
	Table features_table;
	std::string AEOEAA_string;
	std::string my_string;
	std::string my_string2;
	std::string my_string3;
	SetOfMonads somSheaf;
	SheafConstIterator ci1;
	SheafIterator i1;
	unsigned int i;
	std::vector<long> BPT_SCHEMA_vec;
	eBackendKind BPT_backend_kind;

	if (nBPTVersion == 1) {
		BPT_SCHEMA_vec.push_back(BPT_SCHEMA_1);
		//BPT_SCHEMA_vec.push_back(BPT_SCHEMA_2);
		BPT_SCHEMA_vec.push_back(BPT_SCHEMA_3);
		BPT_SCHEMA_vec.push_back(BPT_SCHEMA_5);
		BPT_SCHEMA_vec.push_back(BPT_SCHEMA_7);
		BPT_backend_kind = kBPT;
	} else {
		BPT_SCHEMA_vec.push_back(BPT_SCHEMA_1);
		BPT_backend_kind = kBPT2;
	}

	bSomethingTested = true;
	
	std::vector<EmdrosEnv*> BPTEE_vec;
	for (i = 0;
	     i < BPT_SCHEMA_vec.size();
	     ++i) {
		EmdrosEnv *pBPTEESchema = new EmdrosEnv(kOKConsole, kCSISO_8859_1,
							"", // hostname
							"", // user
							password_bpt, // password
							"emdf",
							BPT_backend_kind);
		BPTEE_vec.push_back(pBPTEESchema);
	}

	// Drop database emdf4
	mql_query = "DROP DATABASE emdf4 GO";
	exec_string_ignore_error(pEE, mql_query, "dropping database emdf4");

	// Drop database emdf3
	mql_query = "DROP DATABASE emdf3 GO";
	exec_string_ignore_error(pEE, mql_query, "dropping database emdf3");

	bAllOK = true; // Global variable

	// Create database
	mql_query = "CREATE DATABASE emdf3 GO";
	if (!exec_string(pEE, mql_query, "creating database emdf3"))
		goto end2;

	// Create database
	mql_query = "CREATE DATABASE emdf4 GO";
	if (!exec_string(pEE, mql_query, "creating database emdf4"))
		goto end1;

	// Use database
	mql_query = "USE DATABASE emdf4 GO";
	if (!exec_string(pEE, mql_query, "using database emdf4"))
		goto end;

	// Use special emdf "database" (which does not need to exist)
	mql_query = "USE DATABASE emdf GO";
	if (!exec_string(pEE, mql_query, "using database emdf"))
		goto end;

	// Use database
	mql_query = "USE DATABASE emdf3 GO";
	if (!exec_string(pEE, mql_query, "using database emdf3"))
		goto end;

	// Test all in QueryAnswer array

	while (it != itend) {
		QueryAnswer *pQA = *it;

		if (strcmp_nocase(pQA->m_query_name, "STOP") == 0) {
			break;
		} else {

			if (!exec_QueryAnswer(pEE, BPT_backend_kind, password_bpt, BPT_SCHEMA_vec, BPTEE_vec, *pQA)) {
				bAllOK = false;
				goto end;
			}
		}

		++it;
	}

 end:
	// Drop database
	mql_query = "DROP DATABASE emdf4 GO";
	exec_string(pEE, mql_query, "dropping database emdf4");
 end1:
	// Drop database
	/*
	mql_query = "DROP DATABASE emdf3 GO";
	exec_string(pEE, mql_query, "dropping database emdf3");
	*/

 end2:
	/*
	// For now, we can't drop BPT databases...

	// Drop database
	mql_query = "DROP DATABASE bptschema1 GO";
	exec_string(pBPTEESchema1, mql_query, "dropping database bptschema1");

	// Drop database
	mql_query = "DROP DATABASE bptschema2 GO";
	exec_string(pBPTEESchema2, mql_query, "dropping database bptschema2");
	*/

	result = bAllOK ? 0 : 1;

	for (i = 0;
	     i < BPTEE_vec.size();
	     ++i) {
		EmdrosEnv *pBPTEESchema = BPTEE_vec[i];
		delete pBPTEESchema;
	}
	BPTEE_vec.clear();

	BPT_SCHEMA_vec.clear();

	return result;
}

int testall(EmdrosEnv *pEE)
{
	int nResult = 0;
	(void) pEE; // Silence a warning when we don't have BPT or BPT2.
	
#if defined(USE_BPT) && USE_BPT
	if (nResult == 0) {
		// Test BPT version 1
		nResult = testBPTEngine(pEE, 1, "");
	}
	if (nResult == 0) {
		// Test BPT version 1, with password
		std::string password_bpt_56_bit = "0x65b11951 0x9aa24adb";
		nResult = testBPTEngine(pEE, 1, password_bpt_56_bit);
	}
	if (nResult == 0) {
		// Test BPT version 1, with char-encoded password
		std::string password_bpt_56_bit = "\xc4\x08\xd8\x33\xb9\x5f\x70";
		nResult = testBPTEngine(pEE, 1, password_bpt_56_bit);
	}
#endif
#if defined(USE_BPT2) && USE_BPT2
	if (nResult == 0) {
		// Test BPT version 2
		nResult = testBPTEngine(pEE, 2, "");
	}
	if (nResult == 0) {
		// Test BPT version 2, with password
		std::string password_bpt_56_bit = "0xfe3a3b53 0x2344ceb5"
		nResult = testBPTEngine(pEE, 2, password_bpt_56_bit);
	}
	if (nResult == 0) {
		// Test BPT version 2, with char-encoded password
		std::string password_bpt_56_bit = "\x7d\x1f\xea\x92\xf3\xe5\x6a";
		nResult = testBPTEngine(pEE, 2, password_bpt_56_bit);
	}
#endif
	return nResult;
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


	bAllOK = true;

	bSomethingTested = false;
	
	try {
		result = testall(pEE);
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

	if (!bSomethingTested) {
		std::cout << "BPT not installed. Backed " << backend_kind2string(backend_kind) << " not tested." << std::endl;
		result = 0;
	} else {
		if (bAllOK) {
			std::cout << "SUCCESS: All tests completed successfully on backend " << backend_kind2string(backend_kind) << '.' << std::endl;
			result = 0;
		} else {
			std::cerr << "FAILURE: At least one test failed  on backend " << backend_kind2string(backend_kind) << '.' << std::endl;
			result = 1;
		}
	}

	// Clean up
	delete pEE;
	return result;

}

int main(int argc, char* argv[])
{
	init_bptqa_list();
	
	int result = 0;

	std::string hostname;
	std::string password;
	std::string user;
	std::string initial_db("emdf");
	eBackendKind backend_kind;
	eCharsets dummy_charset;
	bool bTestedSQLite3 = false;
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

	if (!bSomethingTested) {
		std::cout << "BPT not installed. Nothing tested." << std::endl;
	} else {
		std::cout << "Backends tested:" << std::endl;	
		if (bTestedSQLite3) {
			std::cout << "                 - SQLite 3.X.X" << std::endl;
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
	} 

	teardown_bptqa_list();
	
	return result;
}
