/*
 * mql.cpp
 *
 * A front-end to the MQL parser
 * Created: 5/1-2001 (1st of May, 2001)
 * Last update: 11/23-2016
 *
 * Return codes:
 * 
 * 0 : Success
 * 1 : Wrong usage
 * 2 : Connection to backend server could not be established
 * 3 : An exception occurred
 * 4 : Could not open file
 * 5 : Database error
 * 6 : Compiler error
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2016  Ulrik Sandborg-Petersen
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


#include <emdros-config.h>
// Do this because emdros-config might #define malloc,
// in which case some systems will fail to use the real 
// malloc, thus causing build errors.
#undef malloc


#include <monads.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string_func.h>
#include <emdfdb.h>
#include <table.h>
#include <emdros-lconfig.h>
#include <emdf_output.h>
#include <mql_execute.h>
#include <mql_sheaf.h>
#include <emdros_environment.h>
#include <encryption.h>
#include <opt.h>
#include <messages.h>



int exec_file(EmdrosEnv *pEE, std::string filename, bool bPrintOutput, bool bCallerAcceptsIncompleteOutput, MQLResultCallback *pCallback)
{
	UNUSED(bCallerAcceptsIncompleteOutput);
	
	bool bResult;
	std::string strError;
	std::ifstream fin;
	int nResult;
	fin.open(filename.c_str());
	if (!fin) {
		std::cerr << "FAILURE: Could not open file " << filename << " for reading." << std::endl;
		std::cerr << "Action aborted." << std::endl;
		nResult = 4;
	}
	if (!pEE->executeStream(fin, bResult, bPrintOutput, true, pCallback)) {
		std::cerr << "FAILURE: Database error executing " << filename << std::endl;
		nResult = 5;
	} else {
		if (!bResult) {
			// std::cerr << "FAILURE: Compiler error executing " << filename << std::endl;
			nResult = 6;
		} else { 
			// std::cout << "SUCCESS executing file " << filename << std::endl;
			nResult = 0;
		}
	}

	// Return result
	return nResult;
}


int exec_cin(EmdrosEnv *pEE, bool bPrintOutput, bool bCallerAcceptsIncompleteOutput, MQLResultCallback *pCallback)
{
	UNUSED(bCallerAcceptsIncompleteOutput);
	
	bool bResult;
	std::string strError;
	int nResult;
	if (!pEE->executeStream(std::cin, bResult, bPrintOutput, true, pCallback)) {
		std::cerr << "FAILURE: Database error executing stdin." << std::endl;
		nResult = 5;
	} else {
		if (!bResult) {
			// std::cerr << "FAILURE: Compiler error executing stdin." << std::endl;
			nResult = 6;
		} else { 
			// std::cout << "SUCCESS executing stdin." << std::endl;
			nResult = 0;
		}
	}

	// Return result
	return nResult;
}

void show_version(std::ostream& ostr)
{
	ostr << "mql from Emdros version " << EMDROS_VERSION << std::endl;
	printBackendsAvailable(ostr);
}

void print_usage(std::ostream& ostr)
{
	show_version(ostr);
	ostr << "Usage:" << std::endl;
	ostr << "   mql [options] [filename] " << std::endl;
	ostr << "       If no filename is given, use stdin." << std::endl;
	ostr << "   mql (--help|-V|--version)" << std::endl;
	ostr << "OPTIONS:" << std::endl;
	ostr << "   --xml                Output XML format" << std::endl;
	ostr << "   --cxml               Output compact XML format" << std::endl;
	ostr << "   --json               Output JSON format" << std::endl;
	ostr << "   --cjson              Output compact JSON format" << std::endl;
	ostr << "   --console            Output console-friendly format" << std::endl;
	ostr << "   --incomplete-output  Caller accepts incomplete output while processing" << std::endl;
	printUsageStandardArguments(ostr);
	ostr << "   -d , --dbname db     Set initial database to db" << std::endl;
	ostr << "   -n , --nop           Do not print results (no output)" << std::endl;
	ostr << "DEFAULTS:" << std::endl;
	ostr << "   --console" << std::endl;
	ostr << "   -d emdf" << std::endl;
	printUsageDefaultsOfStandardArguments(ostr);
}

int main(int argc, char* argv[])
{
	// Set defaults
	eOutputKind output_kind = kOKConsole;
	bool bCallerAcceptsIncompleteOutput = false;
	std::string initial_db;
	std::string filename;
	std::string hostname;
	std::string user;
	bool bPrintOutput = true;
	std::string password;
	eBackendKind backend_kind;
	eCharsets charset;

	addStandardArguments(true // Do add -e | --encoding
			     );

	addOption("--xml", "--xml");
	addOption("--cxml", "--cxml");
	addOption("--json", "--json");
	addOption("--cjson", "--cjson");
	addOption("--console", "--console");
	addOption("--incomplete-output", "--incomplete-output");
	addOption("-n", "--nop");

	addOption("-d", "--dbname", true, 
		  "emdf",
		  "ERROR: -d and --dbname must have a database-name as their argument.\n"
		  "       example: -d book1\n");

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
					  charset,
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

		// Output kind
		if (getArgumentPresent("--xml")) {
			output_kind = kOKXML;
		}
		if (getArgumentPresent("--cxml")) {
			output_kind = kOKCompactXML;
		} 
		if (getArgumentPresent("--json")) {
			output_kind = kOKJSON;
		}
		if (getArgumentPresent("--cjson")) {
			output_kind = kOKCompactJSON;
		} 
		if (getArgumentPresent("--console")) {
			output_kind = kOKConsole;
		}
		if (getArgumentPresent("--incomplete-output")) {
			bCallerAcceptsIncompleteOutput = true;
		}

		// --nop | -n
		if (getArgumentPresent("-n")) {
			bPrintOutput = false;
		}

		getArgumentValue("-d", initial_db);

		if (surplus_arguments.size() == 0) {
			filename = "";
		} else if (surplus_arguments.size() == 1) {
			filename = *surplus_arguments.begin();
		} else {
			std::cerr << "Error: You can only specify one filename. You specified: '" << joinList("' and '", surplus_arguments) << "'." << std::endl;
			return 1;
		}
	}

	int nResult = 3;
	EmdrosEnv *pEE = 0;
	MQLResultCallbackOutput *pCallback = 0;
	
	try {
	
		// Make connection
		pEE = new EmdrosEnv(output_kind, 
				    charset,
				    hostname,
				    user, password,
				    initial_db,
				    backend_kind);
		// Zero-fill password
		zeroFillString(password);
		
		// Check that we connected
		if (!pEE->connectionOk()) {
			emdrosMessageConnectionNotOK(&std::cerr, backend_kind);
			delete pEE;
			return 2;
		}
		
		bool bFlushAfterEveryResult = !bCallerAcceptsIncompleteOutput;
		if (bPrintOutput) {
			pCallback = new MQLResultCallbackOutput(pEE->getMQLEE(), bFlushAfterEveryResult);
		} else {
			pCallback = 0;
		}
		
		if (filename == "") {
			nResult = exec_cin(pEE, bPrintOutput, bCallerAcceptsIncompleteOutput, pCallback);
		} else {
			nResult = exec_file(pEE, filename, bPrintOutput, bCallerAcceptsIncompleteOutput, pCallback);
		}
	} catch (EMdFNULLValueException e) {
		std::cerr << "ERROR: EMdFNULLValueException (Database error)..." << std::endl;
		std::cerr << pEE->getDBError() << std::endl; 
		std::cerr << pEE->getCompilerError() << std::endl;
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (EMdFDBDBError e) {
		std::cerr << "ERROR: EMdFDBDBError (Database error)..." << std::endl;
		std::cerr << pEE->getDBError() << std::endl; 
		std::cerr << pEE->getCompilerError() << std::endl;
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (EMdFDBException e) {
		std::cerr << "ERROR: EMdFDBException (Database error)..." << std::endl;
		std::cerr << pEE->getDBError() << std::endl; 
		std::cerr << pEE->getCompilerError() << std::endl;
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (TableColumnException e) {
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (TableRowException e) {
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (TableException e) {
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (BadMonadsException e) {
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (WrongCharacterSetException e) {
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (EMdFOutputException e) {
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (EmdrosException e) {
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (...) {
		std::cerr << "Unknown exception occurred.  Program aborted." << std::endl;
	} 

	// Clean up
	delete pEE;
	delete pCallback;

	return nResult;
}


