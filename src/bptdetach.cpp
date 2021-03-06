/*
 * bptdetach.cpp
 *
 * A front-end for detaching an Emdros BPT database payload from
 * another BPT database.
 *
 * Created: 5/1-2001 (1st of May, 2001)
 * Last update: 5/11-2018
 *
 * Return codes:
 * 
 * 0 : Success
 * 1 : Wrong usage
 * 2 : Connection to backend server could not be established
 * 3 : An exception occurred
 * 4 : Something else went wrong
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
#include <emdros_environment.h>
#include <mql_execution_environment.h>
#include <encryption.h>
#include <opt.h>
#include <messages.h>

#include <bptemdfdb.h>


bool exec_dumpPayload(EmdrosEnv *pSourceEE, const std::string& output_bptfilename, const std::string& payload_bptkey)
{
	bool bResult = false;

	std::string error_message;
	std::string backend_name = pSourceEE->getBackendName();
	if (backend_name == "bpt") {
		BPTEMdFDB *pDB = static_cast<BPTEMdFDB*>(pSourceEE->getMQLEE()->pDB);
		bResult = pDB->dumpPayload(output_bptfilename, payload_bptkey);
		if (!bResult) {
			error_message = pDB->errorMessage() + "\n\n";
		}
	} else {
		bResult = false;
		error_message = "Unknown BPT backend.\n\n";
	}
	if (bResult) {
		std::cout << "SUCCESS!\nThe database was successfully dumped as:\n" << output_bptfilename << std::endl;
	} else {
		std::cerr << "FAILURE!\nSomething went wrong while dumping BPT payload:\n\n"
			  << "---------------------------------------------------------------\n"
			  << error_message 
			  << "\n---------------------------------------------------------------\n"
			  << "FAILURE!" << std::endl;
			
	}

	// Return result
	return bResult;
}



void show_version(std::ostream& ostr)
{
	ostr << "bptdetach from Emdros version " << EMDROS_VERSION << std::endl;
}

void print_usage(std::ostream& ostr)
{
	show_version(ostr);
	ostr << "Usage:" << std::endl;
	ostr << "   bptdetach [options]" << std::endl;
	ostr << "   bptdumper (--help|-V|--version)" << std::endl;
	ostr << "OPTIONS:" << std::endl;
	ostr << "   -d, --dbname db        Dump this BPT database (required)" << std::endl;
	ostr << "   -o, --payload-db filename    Use filename as output filename (required)" << std::endl;
	ostr << "   -pk, --payload-key key  Use this key to encrypt the payload (default none)" << std::endl;
	ostr << "   --key key               Decrypt BPT db with this key (defaults to none)" << std::endl;
	ostr << "DEFAULTS:" << std::endl;
	ostr << "   --payload-key ''   (none)" << std::endl;
	ostr << "   --key ''           (none)" << std::endl;
}

int main(int argc, char* argv[])
{
	// Set defaults
	eOutputKind output_kind = kOKConsole;
	eCharsets charset = kCSUTF8;
	std::string hostname;
	std::string user;
	std::string bpt_dbname;
	std::string bpt_inkey;
	std::string payload_outfilename;
	std::string payload_outkey;
	eBackendKind bpt_backend_kind = kBPT;


	addOption("-d", "--dbname", true, 
		  "",
		  "ERROR: -d and --dbname must have a database-name as their argument.\n"
		  "       example: -d book1.bpt\n");

	addOption("-V", "--version", false,
		  "",
		  "");

	addOption("--help", "--help", false,
		  "",
		  "");

	addOption("--key", "--key", true, 
		  "",
		  "ERROR: --key must have a key as its argument.");

	addOption("-o", "--payload-db", true, 
		  "",
		  "ERROR: -o and --payload-db must have a BPT database-name as their argument.\n"
		  "       example: -o book1.payload.bpt\n");

	addOption("-pk", "--payload-key", true, 
		  "",
		  "ERROR: --pk and --payload-key must have a key as its argument.");

	// Parse arguments
	std::list<std::string> surplus_arguments;

	std::string error_message;
	if (!parseArguments(argc, argv, error_message, surplus_arguments)) {
		print_usage(std::cerr);
		std::cerr << error_message << std::endl;
		return 1;
	} else {
		bool bShowVersion = getArgumentPresent("-V");
		bool bShowHelp = getArgumentPresent("--help");
		std::string error_message;

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

		// Database name
		getArgumentValue("-d", bpt_dbname);

		// Key
		getArgumentValue("--key", bpt_inkey);

		// Payload db
		getArgumentValue("-o", payload_outfilename);

		// Payload key
		getArgumentValue("-pk", payload_outkey);

		if (surplus_arguments.size() != 0) {
			std::cerr << "Error: bptdetach takes no non-option arguments. You specified: '" << joinList("' and '", surplus_arguments) << "'." << std::endl;
			return 1;
		}
	}

	// Make connection
	EmdrosEnv *pEE = new EmdrosEnv(output_kind, 
				       charset,
				       hostname,
				       user, bpt_inkey,
				       bpt_dbname,
				       bpt_backend_kind);

	// Check that we connected
	if (!pEE->connectionOk()) {
		emdrosMessageConnectionNotOK(&std::cerr, bpt_backend_kind);
		delete pEE;
		return 2;
	}

	int nResult = 3;
	try {
		if (exec_dumpPayload(pEE, payload_outfilename, payload_outkey)) {
			nResult = 0;
		} else {
			nResult = 4;
		}
	} catch (EMdFNULLValueException& e) {
		std::cerr << "ERROR: EMdFNULLValueException (Database error)..." << std::endl;
		std::cerr << pEE->getDBError() << std::endl; 
		std::cerr << pEE->getCompilerError() << std::endl;
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (EMdFDBDBError& e) {
		std::cerr << "ERROR: EMdFDBDBError (Database error)..." << std::endl;
		std::cerr << pEE->getDBError() << std::endl; 
		std::cerr << pEE->getCompilerError() << std::endl;
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (EMdFDBException& e) {
		std::cerr << "ERROR: EMdFDBException (Database error)..." << std::endl;
		std::cerr << pEE->getDBError() << std::endl; 
		std::cerr << pEE->getCompilerError() << std::endl;
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (TableColumnException& e) {
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (TableRowException& e) {
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (TableException& e) {
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (BadMonadsException& e) {
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (WrongCharacterSetException& e) {
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (EMdFOutputException& e) {
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (EmdrosException& e) {
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (...) {
		std::cerr << "Unknown exception occurred.  Program aborted." << std::endl;
	} 

	// Clean up
	delete pEE;
	return nResult;
}
