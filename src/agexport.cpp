/*
 * agexport.cpp
 *
 * A front-end to the Annotation Graph export
 * Created: 5/1-2001 (1st of May, 2001)
 * Last update: 5/11-2018
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


#include <agxml.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <emdfdb.h>
#include <table.h>
#include <emdros-lconfig.h>
#include <emdf_output.h>
#include <emdros_environment.h>
#include <encryption.h>
#include <opt.h>
#include <messages.h>



const eCharsets DEFAULT_ENCODING = kCSUTF8;


int exec_stream(EmdrosEnv *pEE, std::ostream* pOutStream, eCharsets charset)
{
	int result = 0;
	EMdFOutput *pOutput = new EMdFOutput(charset,
					     pOutStream,
					     kOKCompactXML);
	AGXMLExporter *pExporter = new AGXMLExporter(pEE, pOutput, 1, MAX_MONAD);
	std::string message;
	if (!pExporter->doIt(message)) {
		std::cerr << "ERROR: Either a compiler error or a database error\n"
			  << "occurred while exporting.\n\n"
			  << message << std::endl;
		result = 5;
	}
	delete pExporter;
	delete pOutput;
	return result;
}

int exec_cin(EmdrosEnv *pEE, eCharsets charset)
{
	return exec_stream(pEE, &std::cout, charset);
}

int exec_file(EmdrosEnv *pEE, const std::string& output_filename, eCharsets charset)
{
	std::ofstream fout;
	int nResult = 0;
	fout.open(output_filename.c_str());
	if (!fout) {
		std::cerr << "FAILURE: Could not open file " << output_filename << " for writing." << std::endl;
		std::cerr << "Action aborted." << std::endl;
		nResult = 4;
	} else {
		nResult = exec_stream(pEE, &fout, charset);
	}

	return nResult;
}


void show_version(std::ostream& ostr)
{
	ostr << "agexport from Emdros version " << EMDROS_VERSION << std::endl;
}

void print_usage(std::ostream& ostr)
{
	show_version(ostr);
	ostr << "Usage:" << std::endl;
	ostr << "   agexport [options]" << std::endl;
	ostr << "   agexport (--help|-V|--version)" << std::endl;
	ostr << "OPTIONS:" << std::endl;
	printUsageStandardArguments(ostr);
	ostr << "   -d , --dbname db     Dump database db (required)" << std::endl;
	ostr << "   -o , --output file   Dump database to filename file (\"-\" means stdout)" << std::endl;
	ostr << "   -e , --encoding enc  Can either be iso_8859-1 to iso_8859-15 or UTF8" << std::endl;     
	ostr << "DEFAULTS:" << std::endl;
	ostr << "   -o -                 (Use stdout)" << std::endl;
	ostr << "   -e UTF8" << std::endl;
	printUsageDefaultsOfStandardArguments(ostr);
	
}

int main(int argc, char* argv[])
{
	// Set defaults
	eOutputKind output_kind = kOKConsole;
	std::string db;
	std::string output_filename;
	std::string hostname;
	std::string user;				
	eCharsets charset;
	std::string password;
	eBackendKind backend_kind;

	addStandardArguments(true // Do add -e | --encoding
			     );

	addOption("-d", "--dbname", true, 
		  "", // Default value
		  "ERROR: -d and --dbname must have a database-name as their argument.\n"
		  "       example: -d book1\n");

	addOption("-o", "--output", true, 
		  "-", // Default value
		  "ERROR: -o and --output must have a filename (or \"-\" for stdout)\n"
		  "       as their argument.\n"
		  "       example: -o output.xml\n");


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

		if (bShowVersion) {
			show_version(std::cout);
			return 0;
		}

		// --help
		if (bShowHelp) {
			print_usage(std::cout);
			return 0;
		}

		// There must be no surplus arguments.
		if (surplus_arguments.size() != 0) {
			print_usage(std::cerr);
			std::cerr << "Error: unrecognized command-line options: " << joinList(" ", surplus_arguments) << std::endl;
			return 1;
		}

		// --dbname | -d
		getArgumentValue("-d", db);
	}

	if (db == "") {
		print_usage(std::cerr);
		std::cerr << "ERROR: You must give a -d option or a --dbname option." << std::endl;
		return 1;
	}

	// Make connection
	EmdrosEnv *pEE = new EmdrosEnv(output_kind, kCSISO_8859_1,
				       hostname,
				       user, password,
				       db, 
				       backend_kind);
	// Zero-fill password
	zeroFillString(password);

	// Check that we connected
	if (!pEE->connectionOk()) {
		emdrosMessageConnectionNotOK(&std::cerr, backend_kind);
		delete pEE;
		return 2;
	}

	int nResult = 3;
	try {
		if (output_filename == "-") {
			nResult = exec_cin(pEE, charset);
		} else {
			nResult = exec_file(pEE, output_filename, charset);
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


