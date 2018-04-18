/*
 * mql_execute.cpp
 *
 * Functions for executing MQL queries
 * Created: 3/23-2001
 * Last update: 4/1-2016
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2016   Ulrik Sandborg-Petersen
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

/**@file mql_execute.cpp
 *@brief Implementation of MQL execution functions (MQL layer)
 */


#include <emdfdb.h>
#include <arena.h>
#include <string_list.h>
#include <mql_execute.h>
#include <mql_error.h>
#include <mql_result.h>
#include <mql_extern.h>
#include <mql_execution_environment.h>
#include <string_func.h>
#include <string_list.h>
#include <mql_sheaf.h>
#include <logging.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <debug.h>
#include <mql_lexer.h>
#include <mql_gq.h>

// Prototypes
extern void *MQLParserAlloc(void *(*mallocProc)(size_t));
extern void MQLParserFree(void *p, void (*freeProc)(void*));
extern void MQLParser(
		      void *yyp,                       /* The parser */
		      int yymajor,                     /* The major token code number */
		      Token *yyminor,                  /* The value for the token */
		      MQLExecEnv *pArg /* optional argument */
		      );

#ifndef NDEBUG
extern void MQLParserTrace(FILE *TraceFILE, char *zTracePrompt);
#endif

int yyparse(MQLExecEnv *pEE)
{
	void *pParser;
	int hTokenId;

#ifndef NDEBUG
	// MQLParserTrace(stdout, "TRACE: ");
#endif
  
	LOG_WRITE_TIME("yyparse()", "Parsing query.");

	pEE->bSyntaxError = false,
		pParser = MQLParserAlloc( malloc );
	Token *token = newToken();
	while((hTokenId = yylex(token, pEE))
	      && !pEE->bSyntaxError) {
		MQLParser(pParser, hTokenId, token, pEE);
		token = newToken();
	}
	MQLParser(pParser, 0, token, pEE);
	MQLParserFree(pParser, free );
	deleteToken(token);

	LOG_WRITE_TIME("yyparse()", "... Done parsing query.");
	if (pEE->bSyntaxError) {
		// rdeleteToken(token);
		return 1;
	} else {
		return 0;
	}
}






bool execute_statement(MQLExecEnv *pEE, bool& bResult)
{
	// Declare local variables
	bool return_value = true;

	// Initialize bResult
	bResult = true;

	// Weed
	if (return_value && bResult) {
		LOG_WRITE_TIME("execute_statement()", "Weeding...");
		pEE->nCompilerStage = COMPILER_STAGE_WEED;
		ASSERT_THROW(pEE->pStatement != 0, "pStatement was 0");
		pEE->pStatement->weed(bResult);
		if (!bResult) {
			pEE->pError->prependError("Weeding failed with compiler error\n");
			return_value =  true;
		} 
	}

	// Symbol
	if (return_value && bResult) {
		LOG_WRITE_TIME("execute_statement()", "Symbol-checking...");
		pEE->nCompilerStage = COMPILER_STAGE_SYMBOL;
		if (!(pEE->pStatement->symbol(bResult))) {
			pEE->pError->prependError("Symbolchecking failed with database error\n");
			return_value =  false;
		}
		if (return_value && !bResult) {
			pEE->pError->prependError("Symbolchecking failed with compiler error\n");
			return_value =  true;
		}
	}

	// Type
	if (return_value && bResult) {
		LOG_WRITE_TIME("execute_statement()", "Type-checking...");
		pEE->nCompilerStage = COMPILER_STAGE_TYPE;
		if (!(pEE->pStatement->type(bResult))) {
			pEE->pError->prependError("Typechecking failed with database error\n");
			return_value =  false;
		}
		if (return_value && !bResult) {
			pEE->pError->prependError("Typechecking failed with compiler error\n");
			return_value =  true;
		}
	}

	// Monads
	if (return_value && bResult) {
		LOG_WRITE_TIME("execute_statement()", "Monads-checking...");
		pEE->nCompilerStage = COMPILER_STAGE_MONADS;
		if (!(pEE->pStatement->monads(bResult))) {
			pEE->pError->prependError("Monads-checking failed with database error\n");
			return_value =  false;
		}
		if (return_value && !bResult) {
			pEE->pError->prependError("Monads-checking failed with compiler error\n");
			return_value =  true;
		}
	}

	// Exec
	if (return_value && bResult) {
		LOG_WRITE_TIME("execute_statement()", "Executing query!...");
		pEE->nCompilerStage = COMPILER_STAGE_EXEC;
		// mql_progress is not shown in JSON result
		if (pEE->pOut->isXML()) {
			pEE->pOut->startTag("mql_progress", true);
			pEE->pOut->newline();
			pEE->pOut->flush();
		}
		if (pEE->pOut->isXML()) {
			pEE->pOut->endTag("mql_progress", true);
			pEE->pOut->flush();
		}
		if (!(pEE->pStatement->exec())) {
			pEE->pError->prependError("Executing failed with database error\n");
			return_value =  false;
		} else {
			return_value = true;
		}
		LOG_WRITE_TIME("execute_statement()", "... Done executing query!...");
	}

	// Return
	return return_value;
}

bool execute(MQLExecEnv* pEE, bool& bResult)
{
	// Clear errors
	pEE->pError->clearError();
	pEE->pDB->clearLocalError();

	// Declare return_value
	bool return_value = true;
  
	// Initialize bResult
	bResult = true;

	// Initialize pEE->nCompilerStage
	pEE->nCompilerStage = COMPILER_STAGE_PARSE;

	// Parser
	int iResult = yyparse(pEE);
	// Did parsing fail?
	if (iResult == 1) {
		bResult = false;
		pEE->pError->prependError("Parsing failed\n");
		return_value = true;
	} else if (iResult == 2) {
		bResult = false;
		pEE->pError->prependError("Parsing stack overflow. Parsing failed. Try simplifying your MQL statement.\n");
		return_value = true;
	}

	// Was the parsing a success?
	if (bResult) {
		// If so, execute statements
		return_value = execute_statement(pEE, bResult);
	}

	// Return
	return return_value;
}

void execute_query(MQLExecEnv *pEE, std::string query, bool& bDBResult, bool& bCompileResult, bool bPrintResult, bool bReportError)
{
	UNUSED(bPrintResult);
	
	// Clean from previous round (if any)
	pEE->clean();

	// Lexer
	pEE->pLexer = new MQLScanner(query);

	// Write log
	LOG_WRITE_TIME("execute_query()", "Executing query:");
	LOG_WRITE("execute_query()", query.c_str());

	//std::cerr << "UP200: query = \n" << query << std::endl;

	// Execute
	bDBResult = execute(pEE, bCompileResult);

	// Write log
	LOG_WRITE_TIME("execute_query()", "Execution finished.");


	// Report error if necessary
	std::string strError;
	if (!bDBResult 
	    && bReportError 
	    && pEE->pOut->getOutputKind() == kOKConsole) {
		std::cerr << "ERROR: Database error while executing query:" << std::endl;
		std::cerr << "--------------------------------------------" << std::endl;
		std::cerr << query << std::endl;
		std::cerr << "--------------------------------------------" << std::endl;
		std::cerr << pEE->pDB->errorMessage() << std::endl; 
		std::cerr << pEE->pError->getError() << std::endl;
	} else {
		if (!bCompileResult 
		    && bReportError
		    && pEE->pOut->getOutputKind() == kOKConsole) {
			std::cerr << "ERROR: Compile error while executing query:" << std::endl;
			std::cerr << "--------------------------------------------" << std::endl;
			std::cerr << query << std::endl;
			std::cerr << "--------------------------------------------" << std::endl;
			std::cerr << pEE->pError->getError() << std::endl;
		} 
	}
}

/** Execute an MQL stream.
 *
 * Read MQL off of a std::istream stream and execute each of the
 * statements in the stream.
 *
 * You should probably not use this directly, but instead use
 * EmdrosEnv::executeStream(), which is much easier.
 *
 * @param pEE The MQLExecEnv environment on which to operate.
 * @param strin The input stream.
 * @param bResult Used for returning compiler success (true) or
 *        failure (false).
 * @param bPrintResult True means that result should be printed on the
 *        output stream (see EmdrosEnv::EmdrosEnv() or EMdFOutput). False
 *        means that no output is to be printed.
 * @param bReportError True means that errors should be reported on 
 *        the output stream (see EmdrosEnv::EmdrosEnv() or EMdFOutput). 
 *        False means that errors are not to be printed.
 * @return True on no database error, false if a database error occurred.
 */
bool mqlExecuteStream(MQLExecEnv *pEE, 
		      std::istream& strin, 
		      bool& bResult, 
		      bool bPrintResult, 
		      bool bReportError,
		      MQLResultCallback *pCallback)
{
	// Set callback
	pEE->pCallback = pCallback;

	// Take care of XML header
	if (bPrintResult) {
		if (pEE->pOut->isXML()) {
			pEE->pOut->printXMLDecl();
			pEE->pOut->printDTDstart("mql_results");
			MQLResult::printDTD(pEE->pOut);
			Table::printDTD(pEE->pOut);
			Sheaf::printDTD(pEE->pOut);
			FlatSheaf::printDTD(pEE->pOut);
			pEE->pOut->printDTDend();
			pEE->pOut->startTag("mql_results", true);
			pEE->pOut->newline();
			pEE->pOut->flush();
		} else if (pEE->pOut->isJSON()) {
			pEE->pOut->jsonStartStruct();
			pEE->pOut->jsonLabel("mql_results");
			pEE->pOut->jsonStartArray();
		}
	}

	// Initialize bExecResult
	bool bExecDBResult = true;
	bool bExecCompileResult = true;

	// Initialize Query-splitting lexer
	MQLGQScanner *pQSL = 
		new MQLGQScanner(&strin);

	// Split into queries
	std::string query;
	do {
		// Get query out of stream
		LOG_WRITE_TIME("mqlExecuteStream()", "parsing the query the first time...");
		Bigstring *pBigstring = pQSL->scan();
		LOG_WRITE_TIME("mqlExecuteStream()", "... Done parsing the query the first time...");
		LOG_WRITE_TIME("mqlExecuteStream()", "Converting pBigstring to string");
		bool bRunQuery = true;
		LOG_WRITE_TIME("mqlExecuteStream()", "Seeing if this is other than whitespace!");
		bool bIsOtherThanWhitespace = pBigstring->is_other_than_whitespace();
		LOG_WRITE_TIME("mqlExecuteStream()", "... Done seeing if this is other than whitespace!");

		if (bIsOtherThanWhitespace) {
			bRunQuery = true;
			pBigstring->addChars(" GO", 3);
			pBigstring->toString(query);
		} else {
			bRunQuery = false;
			query = "";
		}
		LOG_WRITE_TIME("mqlExecuteStream()", "... Done converting pBigstring to string");

		// We're done with the big string!
		LOG_WRITE_TIME("mqlExecuteStream()", "Deleting pBigstring");
		delete pBigstring;
		LOG_WRITE_TIME("mqlExecuteStream()", "... Done deleting pBigstring");


    
		// Run query if necessary
		if (bRunQuery) {
			bool bCompileResult = true;
			bool bDBResult = true;

			// out pre-result material
			if (bPrintResult) {
				if (pEE->pOut->isXML()) {
					// Start mql_result
					pEE->pOut->startTag("mql_result", true);
					pEE->pOut->flush();
				} else if (pEE->pOut->isJSON()) {
					pEE->pOut->jsonStartStruct();
				}
			}

			// Execute
			execute_query(pEE, query, bDBResult, bCompileResult, bPrintResult, bReportError);
      
			// Make sure we report the right thing back
			bExecDBResult = bExecDBResult && bDBResult;
			bExecCompileResult = bExecCompileResult && bCompileResult;

			// Make success-result
			bool bSuccess = bDBResult && bCompileResult;

			// out pre-result material
			if (bPrintResult) {
				if (pEE->pOut->isXML()) {
					// Output "success" node
					AttributePairList status_attributes;
					status_attributes.push_back(AttributePair("success", bool_alpha2string(bSuccess)));
					pEE->pOut->startSingleTag("status", status_attributes, true);
					
					// Output error node?
					if (!bSuccess) {
						//
						// Start error node
						//
						pEE->pOut->startTag("error", true);
						
						//
						// error_source node
						// 
						
						// Figure out source
						bool bWasDB;
						//bool bWasCompiler;
						if (!bDBResult) {
							bWasDB = true;
							//bWasCompiler = false;
						} else if (!bCompileResult) {
							bWasDB = false;
							//bWasCompiler = true;
						} else {
							ASSERT_THROW(false,
								     "Source of error should be either DB or compiler...");
						}
						
						// Make source string
						std::string error_source_string;
						error_source_string = (bWasDB) ? "db" : "compiler";
						
						// Make error source attributes
						AttributePairList error_source_attributes;
						error_source_attributes.push_back(AttributePair("source", error_source_string));
						
						// Emit "error_source" node
						pEE->pOut->startSingleTag("error_source", error_source_attributes, true);
						
						//
						// error_stage node
						//
						
						// Get stage string
						std::string stage_string;
						switch (pEE->nCompilerStage) {
						case COMPILER_STAGE_NONE:	
							stage_string = "none";
							break;
						case COMPILER_STAGE_PARSE:	
							stage_string = "parse";
							break;
						case COMPILER_STAGE_WEED:
							stage_string = "weed";
							break;
						case COMPILER_STAGE_SYMBOL:
							stage_string = "symbol";
							break;
						case COMPILER_STAGE_TYPE:
							stage_string = "type";
							break;
						case COMPILER_STAGE_MONADS:
							stage_string = "monads";
							break;
						case COMPILER_STAGE_EXEC:
							stage_string = "exec";
							break;
						default:
							ASSERT_THROW(false,
								     "Unknown compiler stage");
							break;
						}
	  
						// Make stage attributes
						AttributePairList error_stage_attributes;
						error_stage_attributes.push_back(AttributePair("stage", stage_string));
	  
						// Emit error_stage node
						pEE->pOut->startSingleTag("error_stage", error_stage_attributes, true);

						//
						// error_message_db node
						//
						pEE->pOut->startTag("error_message_db", true);
						std::string strError = pEE->pDB->errorMessage();
						pEE->pOut->outCharData(strError);
						pEE->pOut->endTag("error_message_db", true);
	  
						//
						// error_message_compiler node
						//
						pEE->pOut->startTag("error_message_compiler", true);
						strError = pEE->pError->getError();
						pEE->pOut->outCharData(strError);
						pEE->pOut->endTag("error_message_compiler", true);

						// End "error" node
						pEE->pOut->endTag("error", true);
					}
					pEE->pOut->flush();
				} else if (pEE->pOut->isJSON()) {
					// Output "success" node
					pEE->pOut->jsonLabel("status_success");
					pEE->pOut->jsonOutValue(bSuccess);

					// Output error node?
					if (!bSuccess) {
						//
						// Start error node
						//
						pEE->pOut->jsonLabel("error");
						pEE->pOut->jsonStartStruct();
						
						//
						// error_source node
						// 
						
						// Figure out source
						bool bWasDB;
						//bool bWasCompiler;
						if (!bDBResult) {
							bWasDB = true;
							//bWasCompiler = false;
						} else if (!bCompileResult) {
							bWasDB = false;
							//bWasCompiler = true;
						} else {
							ASSERT_THROW(false,
								     "Source of error should be either DB or compiler...");
						}

						pEE->pOut->jsonLabel("error_source");
						pEE->pOut->jsonOutValue(bWasDB ? "db" : "compiler");

						
						//
						// error_stage node
						//
						
						// Get stage string
						std::string stage_string;
						switch (pEE->nCompilerStage) {
						case COMPILER_STAGE_NONE:	
							stage_string = "none";
							break;
						case COMPILER_STAGE_PARSE:	
							stage_string = "parse";
							break;
						case COMPILER_STAGE_WEED:
							stage_string = "weed";
							break;
						case COMPILER_STAGE_SYMBOL:
							stage_string = "symbol";
							break;
						case COMPILER_STAGE_TYPE:
							stage_string = "type";
							break;
						case COMPILER_STAGE_MONADS:
							stage_string = "monads";
							break;
						case COMPILER_STAGE_EXEC:
							stage_string = "exec";
							break;
						default:
							ASSERT_THROW(false,
								     "Unknown compiler stage");
							break;
						}
						
						pEE->pOut->jsonLabel("error_stage");
						pEE->pOut->jsonOutValue(stage_string);
                        
						
						//
						// error_message_db node
						//
						pEE->pOut->jsonLabel("error_message_db");
						pEE->pOut->jsonOutValue(pEE->pDB->errorMessage());
						

						//
						// error_message_compiler node
						//
						pEE->pOut->jsonLabel("error_message_compiler");
						pEE->pOut->jsonOutValue(pEE->pError->getError());

						// End "error" node
						pEE->pOut->jsonEndStruct();
					}
				}
			}

			// Write to log
			LOG_WRITE_TIME("mqlExecuteStream()", "Printing results.");


			bool bUseMQLResult = true;
			if (pEE->pCallback != 0) {
				bUseMQLResult = pEE->pCallback->useResultAfterwards();
			}


			// out result
			if (bSuccess && bUseMQLResult && bPrintResult && (pEE->pStatement != 0)) {
				MQLResult* pMQLResult = pEE->pStatement->getResult();
				if (pMQLResult != 0)
					pMQLResult->out(pEE->pOut);
				pEE->pOut->flush();
			}

			// Write to log
			LOG_WRITE_TIME("mqlExecuteStream()", "Printing finished.");

			// out post-result material
			if (bPrintResult) {
				if (pEE->pOut->isXML()) {
					pEE->pOut->endTag("mql_result", true);
					pEE->pOut->newline();
					pEE->pOut->flush();
				} else if (pEE->pOut->isJSON()) {
					pEE->pOut->jsonEndStruct();
				}
			}
		}
		
		// Clean up
		query = "";
	} while (!pQSL->noMoreInput() && !(pEE->bQuit));
  
	// Clean up
	delete pQSL;
	
	// Take care of XML footer
	if (bPrintResult) {
		if (pEE->pOut->isXML()) {
			pEE->pOut->endTag("mql_results", true);
			pEE->pOut->newline();
			pEE->pOut->flush();
		} else if (pEE->pOut->isJSON()) {
			pEE->pOut->jsonEndArray();
			pEE->pOut->jsonEndStruct();
			pEE->pOut->newline();
		}
	}
	
	// Unset pCallback 
	//
	// That way, we don't clean() it if it has gone away once we
	// call the MQLExecEnv d'tor.
	pEE->pCallback = 0;

	// Return execution result
	bResult = bExecCompileResult;
	return bExecDBResult;
}


/** Execute an MQL string.
 *
 * Read MQL off of a std::string and execute each of the statements in
 * the string.
 *
 * You should probably not use this directly, but instead use
 * EmdrosEnv::executeString(), which is much easier.
 *
 * @param pEE The MQLExecEnv environment on which to operate.
 * @param input The input string.
 * @param bResult Used for returning compiler success (true) or
 *        failure (false).
 * @param bPrintResult True means that result should be printed on the
 *        output stream (see EmdrosEnv::EmdrosEnv() or EMdFOutput). False
 *        means that no output is to be printed.
 * @param bReportError True means that errors should be reported on 
 *        the output stream (see EmdrosEnv::EmdrosEnv() or EMdFOutput). 
 *        False means that errors are not to be printed.
 * @return True on no database error, false if a database error occurred.
 */
bool mqlExecuteString(MQLExecEnv *pEE, 
		      const std::string& input, 
		      bool& bResult, 
		      bool bPrintResult, 
		      bool bReportError,
		      MQLResultCallback *pCallback)
{
	// Lexer
	std::istringstream strstream(input);

	// Execute
	bool bExecResult = mqlExecuteStream(pEE, strstream, bResult, bPrintResult, bReportError, pCallback);
  
	// Return execution result
	return bExecResult;
}


/** Execute an MQL file.
 *
 * Open the file with the given filename and read MQL off of it.
 * Execute each of the statements in the file.
 *
 * You should probably not use this directly, but instead use
 * EmdrosEnv::executeFile(), which is much easier.
 *
 * @param pEE The MQLExecEnv environment on which to operate.
 * @param filename The filename to open.  If it does not havean
 * absolute path, it is interpreted relative to the working directory
 * of the process in which Emdros resides.
 * @param bResult Used for returning compiler success (true) or
 *        failure (false).
 * @param bPrintResult True means that result should be printed on the
 *        output stream (see EmdrosEnv::EmdrosEnv() or EMdFOutput). False
 *        means that no output is to be printed.
 * @param bReportError True means that errors should be reported on 
 *        the output stream (see EmdrosEnv::EmdrosEnv() or EMdFOutput). 
 *        False means that errors are not to be printed.
 * @return True on no database error, false if a database error occurred.
 */
bool mqlExecuteFile(MQLExecEnv *pEE, 
		    std::string filename, 
		    bool& bResult, 
		    bool bPrintResult, 
		    bool bReportError,
		    MQLResultCallback *pCallback)
{
	// Open file
	std::ifstream fin;
	fin.open(filename.c_str());
	if (!fin) {
		pEE->pError->appendError("Could not open " + filename + " for reading.");
		return false;
	}

	// Call other function
	bool bExecResult = mqlExecuteStream(pEE, fin, bResult, bPrintResult, bReportError, pCallback);

	// Return result
	return bExecResult;
}


/** Partition a stream into queries, getting the next query in the stream
 *
 * @param pStrin The std::istream to partition
 * @param result The contents of the next query.  Is empty if we 
 *        return false.
 * @return True if there was another query, false if we reached eof.
 */
bool mqlGetNextQuery(std::istream *pStrin, std::string& result)
{
	if (pStrin->eof()) {
		result = "";
		return false;
	} else {
		// Initialize Query-splitting lexer
		MQLGQScanner *pQSL = 
			new MQLGQScanner(pStrin);
    
		// Split into queries
		result = "";
		std::string query;
    
		// Get query out of stream
		Bigstring *pBigstring = pQSL->scan();
		pBigstring->toString(query);

		// We're done with the big string!
		delete pBigstring;

		bool bThereWasAQuery = true;
		if (is_other_than_whitespace(query)) {
			query += " GO";
			bThereWasAQuery = true;
		} else {
			query = "";
			bThereWasAQuery = false;
		}

		result = query;

		// Clean up
		delete pQSL;
    
		return bThereWasAQuery;
	}
}
  
