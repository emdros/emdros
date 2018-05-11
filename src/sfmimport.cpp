/*
 * sfmimport.cpp
 *
 * A front-end to Emdros for importing slashed text
 * Created: 8/8-2006 (August 8, 2006)
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
 *   Copyright (C) 2006-2018  Ulrik Petersen
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

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <string_func.h>
#include <emdros-lconfig.h>
#include <opt.h>
#include <sfmimporter.h>




int exec_list(const std::list<std::string>& input_filename_list, std::ostream* pOutStream, const std::string& dbname, monad_m start_monad, id_d_t start_id_d, const std::string& schema_filename, bool bPrintSchema)
{
	int result = 0;

	try {
		SFMImporter *pImporter = new SFMImporter(start_monad, start_id_d, schema_filename);
		if (bPrintSchema) {
			if (dbname != "") {
				std::cout << "CREATE DATABASE '" << dbname << "' GO\n\n"
					  << "USE DATABASE '" << dbname << "' GO\n\n";
			}
			
			pImporter->putSchema(&std::cout);
			
			delete pImporter;
			
			return 0;
		}

		bool bResult = false;
		if (input_filename_list.empty()) {
			bResult = pImporter->readDocumentFromStream(&std::cin);
		} else {
			bResult = pImporter->readDocumentsFromFiles(input_filename_list);
		}
		
		if (!bResult) {
			std::cerr << "FAILURE: Error occurred reading documents." << std::endl;
			result = 5;
		} else {
			if (dbname != "") {
				(*pOutStream) << "USE DATABASE '" << dbname << "' GO\n\n";
			}

			pImporter->putCorpusAsMQL(pOutStream);
			
			std::cerr << "SUCCESS: next_monad is " << pImporter->getNextMonad() << "   next_id_d is " << pImporter->getNextID_D() << std::endl;
		}
		
		delete pImporter;
		
		return result;
	} catch (EmdrosException& e) {
		std::cerr << e.what();
		return 3;
	}

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
	ostr << "sfmimport from Emdros version " << EMDROS_VERSION << std::endl;
}

void print_usage(std::ostream& ostr)
{
	show_version(ostr);
	ostr << "Usage:" << std::endl;
	ostr << "   sfmimport -i <schema-filename> [options] [inputfile ...]" << std::endl;
	ostr << "   sfmimport (--help|-V|--version)" << std::endl;
	ostr << "OPTIONS:" << std::endl;
	ostr << "   --help               Show this help" << std::endl;
	ostr << "   -V , --version       Show version" << std::endl;
	ostr << "   -i , --schema-filename filename     Use this SFM schema" << std::endl;
	ostr << "   -s , --schema        Print schema and quit." << std::endl;
	ostr << "   -d , --dbname db     database name (to use in MQL output)" << std::endl;
	ostr << "   -o , --output file   Dump database to filename file (\"-\" means stdout)" << std::endl;
	ostr << "   --start-monad monad  Start monad (default 1)" << std::endl;
	ostr << "   --start-id_d id_d    Start id_d (default 1)" << std::endl;
	ostr << "DEFAULTS:" << std::endl;
	ostr << "   -o -                 (Use stdout)" << std::endl;
	ostr << "   --start-monad 1" << std::endl;
	ostr << "   --start-id_d  1" << std::endl;
	ostr << "   If no filenames are given, a document is read from stdin." << std::endl;
	ostr << "   By default, no -d switch is assumed." << std::endl;
}

int main(int argc, char* argv[])
{
	// Set defaults
	std::string db;
	std::string output_filename("-");
	std::list<std::string> input_filename_list;
	bool bPrintSchema = false;
	monad_m start_monad = 1;
	id_d_t start_id_d = 1;
	std::string dummy_hostname, dummy_user, dummy_password;
	eCharsets dummy_charset;
	eBackendKind dummy_backend_kind;
	std::string sfm_schema_filename;

	addStandardArguments(false // Do not add -e | --encoding
			     );

	addOption("-s", "--schema");

	addOption("-d", "--dbname", true, 
		  "", // Default value
		  "ERROR: -d and --dbname must have a database-name as their argument.\n"
		  "       example: -d book1\n");

	addOption("-o", "--output", true, 
		  "-", // Default value
		  "ERROR: -o and --output must have a filename (or \"-\" for stdout)\n"
		  "       as their argument.\n"
		  "       example: -o output.xml\n");


	addOption("--start-monad", "--start-monad", true, 
		  "1", // Default value
		  "ERROR: --start-monad must have a monad as its argument\n"
		  "       example: --start-monad 1\n");

	addOption("-i", "--schema-filename", true, 
		  "", // Default value
		  "ERROR: --schema-filename must have a string as its argument\n"
		  "       example: --schema-filename /home/joe/myschema.sfm\n");

	addOption("--start-id_d", "--start-id_d", true, 
		  "1", // Default value
		  "ERROR: --start-id_d must have an id_d as its argument\n"
		  "       example: --start-id_d 1\n");


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

		if (bShowVersion) {
			show_version(std::cout);
			return 0;
		}

		// --help
		if (bShowHelp) {
			print_usage(std::cout);
			return 0;
		}

		// --start-monad
		std::string start_string;
		if (getArgumentValue("--start-monad", start_string)) {
			if (string_is_number(start_string)) {
				start_monad = string2long(start_string);
			} else {
				print_usage(std::cerr);
				std::cerr << "ERROR: The argument of --start-monad must be a number, and not '" << start_string << "'." << std::endl;
				return 1;
			}
		}
		if (start_monad < 1) {
			print_usage(std::cerr);
			std::cerr << "ERROR: The argument of --start-monad must be >= 1." << std::endl;
			return 1;
		}

		// --start-id_d
		if (getArgumentValue("--start-id_d", start_string)) {
			if (string_is_number(start_string)) {
				start_id_d = string2long(start_string);
			} else {
				print_usage(std::cerr);
				std::cerr << "ERROR: The argument of --start-id_d must be a number, and not '" << start_string << "'." << std::endl;
				return 1;
			}
		}
		if (start_id_d < 1) {
			print_usage(std::cerr);
			std::cerr << "ERROR: The argument of --start-id_d must be >= 1." << std::endl;
			return 1;
		}

		// --dbname | -d
		getArgumentValue("-d", db);

		// Output
		getArgumentValue("-o", output_filename);

		// --schema-filename|-i
		getArgumentValue("-i", sfm_schema_filename);

		// -s | --schema
		if (getArgumentPresent("-s")) {
			bPrintSchema = true;
		}

		// Any and all surplus arguments are input filenames...
		input_filename_list = surplus_arguments;
	}

	if (sfm_schema_filename == "") {
		print_usage(std::cerr);
		std::cerr << "Error: You must supply a -i or --schema-filename switch." << std::endl;
		return 1;
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

		if (bContinue) {
			nResult = exec_list(input_filename_list, pOut, db, start_monad, start_id_d, sfm_schema_filename, bPrintSchema);
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


