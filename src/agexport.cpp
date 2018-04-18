/*
 * agexport.cpp
 *
 * A front-end to the Annotation Graph export
 * Created: 5/1-2001 (1st of May, 2001)
 * Last update: 1/7-2009
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
 *   Copyright (C) 2001-2009  Ulrik Sandborg-Petersen
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
	return nResult;
}


