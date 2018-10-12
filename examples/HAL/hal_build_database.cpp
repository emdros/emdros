/*
 * hal_build_database.cpp
 *
 * Functions for building HAL db
 *
 * Created: Sometime in the first half of 2003
 * Last update: 3/1-2017
 *
 *******************************************************************/

/*
 * Copyright (C) 2001-2018     Ulrik Sandborg-Petersen
 * Copyright (C) 2018-present  Sandborg-Petersen Holding ApS, Denmark
 *
 * Licensed under the MIT License.
 *
 * Please see the file COPYING in the root of the sources for more details.
 *
 */



#include <emdf_output.h>
#include <monads.h>
#include <emdfdb.h>
#include <emdros_environment.h>
#include <hal_execution_environment.h>
#include <hal_defines.h>
#include <hal_schema.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <emdros_environment.h>
#include <hal_string_func.h>
#include <string_func.h>
#include <monads.h>
#include <debug.h>

// Global variables
HAL_Execution_environment *pHEE = 0;
EmdrosEnv *pEEE = 0;

bool maybeIssueQuery(std::string& query, const std::string& object_type_name, bool bDoItRegardless)
{
	if (bDoItRegardless
	    || query.size() >= 1024*1024) {
		bool bDBResult;
		bool bCompilerResult = true;
		bDBResult = pEEE->executeString("CREATE OBJECTS WITH OBJECT TYPE [" + object_type_name + "]" + query + " GO ", bCompilerResult, false, true);
		if (!bDBResult) {
			std::cerr << "DB ERROR: Could not execute query for object type" << object_type_name << std::endl;
		} else {
			if (!bCompilerResult) {
				std::cerr << "COMPILER ERROR: Could not execute query for object type" << object_type_name << std::endl;
			}
		}
		query = "";
		return bDBResult && bCompilerResult;
	} else {
		return true;
	}
}


bool UseDB(EmdrosEnv *pEEE, std::string dbname)
{
	bool bResult = true;
	bool bCompilerResult = true;

	// Use DB
	bool bDBResult;
	bDBResult = pEEE->executeString("USE DATABASE \"" + encodeSTRINGstring(dbname) + "\" GO", bCompilerResult, false, true);
	if (!bDBResult) {
		std::cerr << "DB ERROR: Could not use DB " << dbname << std::endl;
	} else {
		if (!bCompilerResult) {
			std::cerr << "COMPILER ERROR: Could not use DB " << dbname << std::endl;
		}
	}

	bResult = bDBResult && bCompilerResult;
	return bResult;
}

bool CreateDB(EmdrosEnv *pEEE, std::string dbname)
{
	bool bResult = true;
	bool bCompilerResult = true;

	// Drop DB
	std::cerr << "Dropping DB " << dbname << std::endl;
	pEEE->executeString("DROP DATABASE " + dbname + " GO", bCompilerResult, false, false);


	// Create DB
	std::cerr << "Creating DB " << dbname << std::endl;
	pEEE->executeString("CREATE DATABASE " + dbname + " GO", bCompilerResult, false, true);

	// Use DB
	std::cerr << "Using DB " << dbname << std::endl;
	if (!UseDB(pEEE, dbname)) {
		return false;
	}

	// Open file
	// Create DB
	bool bDBResult;
	std::cerr << "Making meta-data in DB " << dbname << std::endl;
	bDBResult = pEEE->executeString(strHALschema, bCompilerResult, false, true);
	if (!bDBResult) {
		std::cerr << "DB ERROR: Could not create DB" << std::endl;
	} else {
		if (!bCompilerResult) {
			std::cerr << "COMPILER ERROR: Could not create DB" << std::endl;
		}
	}

	bResult = bDBResult && bCompilerResult;
	if (bResult) {
		std::cerr << "SUCCESS in dropping and creating DB." << std::endl;
	} else {
		std::cerr << "FAILURE in dropping and creating DB." << std::endl;
	}
	return bResult;
}

void CreateHALWord(const std::string stripped_surface, int nHALMonad, std::string& query)
{
	// Create som
	SetOfMonads som;
	som.add(nHALMonad);

	// Create MQL query
	std::string mql;
	mql = "CREATE OBJECT\n";
	mql += "FROM MONADS = " + som.toString() + "\n"
		+ "[\n"
		+ "  stripped_surface := \"" + encodeSTRINGstring(stripped_surface) + "\";\n"
		+ "]\n";

	query += mql;
}

void CreateWord(HAL_Execution_environment *pHEE, int nTextMonad, SetOfMonads& somText, int nTokenType, std::string& query)
{
	// Create som
	SetOfMonads som;
	som.add(nTextMonad);

	// Add monad to text
	somText.add(nTextMonad);

	// Create MQL query
	std::string mql;
	mql = "CREATE OBJECT\n";
	mql += "FROM MONADS = " + som.toString() + "\n"
		+ "[\n"
		+ "  surface := \"" + encodeSTRINGstring(pHEE->get_current_word()) + "\";\n"
		+ "  stripped_surface := \"" + encodeSTRINGstring(pHEE->get_current_stripped_surface()) + "\";\n"
		+ "  is_number := " + ((nTokenType == TT_NUMBER) ? "true" : "false") + ";\n"
		+ "]\n";

	query += mql;
}


bool CreateSentence(HAL_Execution_environment *pHEE, const SetOfMonads& som)
{
	std::string mql;
	mql = "CREATE OBJECT\n";
	mql += "FROM MONADS = " + som.toString() + "\n"
		+ "[Sentence\n"
		+ "  end_of_sentence_punctuation := '" + pHEE->get_current_punctuation() + "';\n"
		+ "]\nGO";

	bool bCompilerResult = true;
	bool bDBResult = pEEE->executeString(mql, bCompilerResult, false, true);
	if (!bDBResult) {
		std::cerr << "DB ERROR: Could not create sentence with som " << som.toString() << std::endl;
	} else {
		if (!bCompilerResult) {
			std::cerr << "COMPILER ERROR: Could not create sentence with som " << som.toString() << std::endl;

		}
	}

	return bCompilerResult && bDBResult;
}

bool CreateTextObject(HAL_Execution_environment *pHEE, const SetOfMonads& som, bool bAddNumbers)
{
	UNUSED(pHEE);
	
	std::string mql;
	mql = "CREATE OBJECT\n";
	mql += "FROM MONADS = " + som.toString() + "\n"
		+ "[Text "
		+ "  add_numbers := " + ((bAddNumbers) ? "true" : "false") + ";\n"
		+ "] GO";

	bool bCompilerResult = true;
	bool bDBResult = pEEE->executeString(mql, bCompilerResult, false, true);
	if (!bDBResult) {
		std::cerr << "DB ERROR: Could not create text with som " << som.toString() << std::endl;
	} else {
		if (!bCompilerResult) {
			std::cerr << "COMPILER ERROR: Could not create text with som " << som.toString() << std::endl;

		}
	}

	return bCompilerResult && bDBResult;
}

bool build_text(HAL_Execution_environment *pHEE, bool bAddNumbers)
{
	bool bResult = true;
	int nTextMonad = 1;

	// Read file
	int nLexResult;
	SetOfMonads somSentence;
	somSentence.add(nTextMonad);
	SetOfMonads somText;
	std::string wordQuery, halWordQuery, sentenceQuery;
	while (bResult && (nLexResult = pHEE->yylex())) {
		if ((nTextMonad % 100) == 1) {
			std::cerr << nTextMonad << " " << std::flush;
		}
		switch (nLexResult) {
		case TT_WORD:
			pHEE->add_word(pHEE->get_current_stripped_surface());
			somSentence.add(nTextMonad);
			CreateWord(pHEE, nTextMonad, somText, TT_WORD, wordQuery);
			nTextMonad++;
			break;
		case TT_NUMBER:
			if (bAddNumbers) {
				pHEE->add_word(pHEE->get_current_stripped_surface());
			}
			somSentence.add(nTextMonad);
			CreateWord(pHEE, nTextMonad, somText, TT_NUMBER, wordQuery);
			nTextMonad++;
			break;
		case TT_END_OF_SENTENCE_PUNCTUATION:
			if (!somSentence.isEmpty()) {
				if (!CreateSentence(pHEE, somSentence)) {
					bResult = false;
				}
				somSentence.clear();
			}
			break;
		default:
			ASSERT_THROW(false, "Unknown token");
			break;
		}

		if (!maybeIssueQuery(wordQuery, "Word", false)) {
			bResult = false;
		}
	}

	if (!wordQuery.empty()) {
		if (!maybeIssueQuery(wordQuery, "Word", true)) {
			bResult = false;
		}
	}

	// Create trailing sentence if not already done.
	// (I.e., if file does not end with eos-punctuation.)
	if (!somSentence.isEmpty()) {
		CreateSentence(pHEE, somSentence);
		somSentence.clear();
	}

	// Create text object
	if (!CreateTextObject(pHEE, somText, bAddNumbers)) {
		bResult = false;
	}

	std::cerr << "!" << std::endl;

	// Return result
	return bResult;
}

bool build_HAL_space(HAL_Execution_environment *pHEE, bool bAddNumbers)
{
	UNUSED(bAddNumbers);
	
	bool bResult = true;

	// Treat set of words
	int nHALMonad = 1000000;
	HAL_Execution_environment::const_iterator ci;
	ci = pHEE->begin();
	int count = 0;
	std::cerr << "Emitting HAL-monads.";
	std::string HALWordQuery;
	while (bResult && (ci != pHEE->end())) {
		if ((count++ % 50) == 0) {
			std::cerr << count << " " << std::flush;
		}
    
		// Add to HAL-space in Emdros db
		CreateHALWord(ci->first, nHALMonad, HALWordQuery);
		ci++;
		nHALMonad++;
		if (!maybeIssueQuery(HALWordQuery, "HAL_word", false)) {
			bResult = false;
		}
	}

	if (!HALWordQuery.empty()) {
		if (!maybeIssueQuery(HALWordQuery, "HAL_word", true)) {
			bResult = false;
		}
	}

	std::cerr << "!" << std::endl;

	// Return result
	return bResult;
}

bool build_WordList(HAL_Execution_environment *pHEE, std::string wordlist_filename)
{
	bool bResult = true;

	// Print message
	std::cerr << "Writing word-list to file " << wordlist_filename << std::endl;

	// Open file
	std::ofstream fout;
	fout.open(wordlist_filename.c_str());
	if (!fout) {
		std::cerr << "ERROR: Cannot open " << wordlist_filename << " for writing." << std::endl;
		return false;
	}

	// Print word-list
	pHEE->Print(&fout);

	// Return result
	return bResult;
}

bool build_db(std::string inputfilename, std::string dbname, std::string wordlist_filename, bool bAddNumbers)
{
	bool bResult = true;

	// Use DB
	if (!UseDB(pEEE, dbname)) {
		std::cerr << "ERROR: Could not use DB." << std::endl;
		return false;
	}
  

	// Print message
	std::cerr << "Opening file " << inputfilename << std::endl;

	// Open file
	std::ifstream fin;
	fin.open(inputfilename.c_str());
	if (!fin) {
		std::cerr << "ERROR: Cannot open " << inputfilename << " for reading." << std::endl;
		return false;
	}

	// Set up execution environment
	pHEE = new HAL_Execution_environment(&fin, &std::cout);

	// Build text
	if (!build_text(pHEE, bAddNumbers)) {
		std::cerr << "ERROR building text in Emdros database." << std::endl;
		delete pHEE;
		fin.close();
		return false;
	}

	// Close file
	fin.close();

	// Build HAL-space
	if (!build_HAL_space(pHEE, bAddNumbers)) {
		std::cerr << "ERROR building HAL-space in Emdros database." << std::endl;
		delete pHEE;
		return false;
	}

	// Build list of words
	if (!build_WordList(pHEE, wordlist_filename)) {
		std::cerr << "ERROR building word-list." << std::endl;
		delete pHEE;
		return false;
	}

	// Clean pHEE
	delete pHEE;

	// Return result
	return bResult;
}


