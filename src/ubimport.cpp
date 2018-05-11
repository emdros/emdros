/*
 * ubimport.cpp
 *
 * A front-end to the Unbound Bible import
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

#include <unboundbibleimporter.h>

#include <opt.h>


int exec_stream(std::istream *pInStream, std::ostream* pOutStream, const std::string& dbname, eCharsets charset)
{
	UNUSED(charset); // FIXME: Use it, or remove it from parameters list.
	
	int result = 0;

	UnboundBibleImporter *pImporter = new UnboundBibleImporter();
	if (!pImporter->readDocumentFromStream(pInStream)) {
		std::cerr << "FAILURE: Error occurred reading Bible." << std::endl;
		result = 5;
	} else {
		if (dbname != "") {
			(*pOutStream) << "CREATE DATABASE '" << dbname << "' GO\n"
				      << "USE DATABASE '" << dbname << "' GO\n\n";

		}
		pImporter->putSchema(pOutStream);
		pImporter->putCorpusAsMQL(pOutStream);
	}

	delete pImporter;

	return result;
}

bool open_file_for_writing(std::ofstream& fout, const std::string& filename)
{
	bool bResult = false;
	fout.open(filename.c_str());
	if (!fout) {
		std::cerr << "FAILURE: Could not open file " << filename << " for writing." << std::endl;
		std::cerr << "Action aborted." << std::endl;
		bResult = false;
	} else {
		bResult = true;
	}

	return bResult;
}

bool open_file_for_reading(std::ifstream& fin, const std::string& filename)
{
	bool bResult = false;
	fin.open(filename.c_str());
	if (!fin) {
		std::cerr << "FAILURE: Could not open file " << filename << " for reading." << std::endl;
		std::cerr << "Action aborted." << std::endl;
		bResult = false;
	} else {
		bResult = true;
	}

	return bResult;
}


void show_version(std::ostream& ostr)
{
	ostr << "ubimport from Emdros version " << EMDROS_VERSION << std::endl;
}

void print_usage(std::ostream& ostr)
{
	show_version(ostr);
	ostr << "Usage:" << std::endl;
	ostr << "   ubimport [options]" << std::endl;
	ostr << "   ubimport (--help|-V|--version)" << std::endl;
	ostr << "OPTIONS:" << std::endl;
	ostr << "   --help               Show this help" << std::endl;
	ostr << "   -V , --version       Show version" << std::endl;
	ostr << "   -d , --dbname db     database name (to use in MQL output)" << std::endl;
	ostr << "   -i , --input  file   Read Bible from filename file (\"-\" means stdin)" << std::endl;
	ostr << "   -o , --output file   Dump database to filename file (\"-\" means stdout)" << std::endl;
	ostr << "DEFAULTS:" << std::endl;
	ostr << "   -i -                 (Use stdin)" << std::endl;
	ostr << "   -o -                 (Use stdout)" << std::endl;
	ostr << "   -d \"\"                (Empty means: Don't emit CREATE/USE DATABASE)" << std::endl;
}


int main(int argc, char* argv[])
{
	// Set defaults
	std::string db("");
	std::string output_filename("-");
	std::string input_filename("-");
	std::string dummy_hostname;
	std::string dummy_user;
	eCharsets dummy_charset = kCSUTF8;
	std::string dummy_password;
	eBackendKind dummy_backend_kind;


	addStandardArguments(false // Do not add -e | --encoding
			     );

	addOption("-d", "--dbname", true, 
		  "", // Default value
		  "ERROR: -d and --dbname must have a database-name as their argument.\n"
		  "       example: -d book1\n");

	addOption("-i", "--input", true, 
		  "-", // Default value
		  "ERROR: -i and --input must have a filename (or \"-\" for stdin)\n"
		  "       as their argument.\n"
		  "       example: -i danish1933.txt\n");

	addOption("-o", "--output", true, 
		  "-", // Default value
		  "ERROR: -o and --output must have a filename (or \"-\" for stdout)\n"
		  "       as their argument.\n"
		  "       example: -o output.mql\n");


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
					  dummy_hostname,
					  dummy_user,
					  dummy_password,
					  dummy_backend_kind,
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

		// --dbname | -d
		getArgumentValue("-d", db);

		// Input
		getArgumentValue("-i", input_filename);

		// Output
		getArgumentValue("-o", output_filename);

		if (surplus_arguments.size() != 0) {
			print_usage(std::cerr);
			std::cerr << "ERROR: Extraneous argument(s) not parsed: '" << joinList("' and '", surplus_arguments) << "'." << std::endl;
			return 1;
		}
	}

	int nResult = 3;
	bool bContinue = true;
	try {
		std::ostream *pOut = 0;
		std::ofstream fout;
		if (output_filename == "-") {
			pOut = &std::cout;
		} else {
			if (!open_file_for_writing(fout, output_filename)) {
				nResult = 4;
				bContinue = false;
			} else {
				pOut = &fout;
			}
		}

		std::istream *pIn = 0;
		std::ifstream fin;
		if (bContinue) {
			if (input_filename == "-") {
				pIn = &std::cin;
			} else {
				if (!open_file_for_reading(fin, input_filename)) {
					nResult = 4;
					bContinue = false;
				} else {
					pIn = &fin;
				}
				
			}
		}

		if (bContinue) {
			nResult = exec_stream(pIn, pOut, db, dummy_charset);
		}

	} catch (EMdFNULLValueException& e) {
		std::cerr << "ERROR: EMdFNULLValueException (Database error)..." << std::endl;
		/*
		std::cerr << pEE->getDBError() << std::endl; 
		std::cerr << pEE->getCompilerError() << std::endl;
		*/
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (EMdFDBDBError& e) {
		std::cerr << "ERROR: EMdFDBDBError (Database error)..." << std::endl;
		/*
		std::cerr << pEE->getDBError() << std::endl; 
		std::cerr << pEE->getCompilerError() << std::endl;
		*/
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (EMdFDBException& e) {
		std::cerr << "ERROR: EMdFDBException (Database error)..." << std::endl;
		/*
		std::cerr << pEE->getDBError() << std::endl; 
		std::cerr << pEE->getCompilerError() << std::endl;
		*/
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
	//	delete pEE;
	return nResult;
}


