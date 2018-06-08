/*
 * fts_indexer.cpp
 *
 * A front-end to the BookcaseIndexer class in the libharvest library.
 * Created: 5/1-2001 (1st of May, 2001)
 * Last update: 6/8-2018
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
#include <encryption.h>
#include <opt.h>
#include <messages.h>

#include <harvest_fts.h>
#include <harvest_fts2.h>


// TODO: Add filters
int exec_indexer(EmdrosEnv *pSourceEE, EmdrosEnv *pTargetEE, const std::string& output_filename, const std::string& bookcase_otn, const std::string& indexed_otn, const std::string& indexed_feature, const std::string& stylesheet_name, const std::string& stylesheet_file_name, const StringList& filter_string_list, bool bEmitVacuum, int fts_engine_version)
{ 
	bool bResult = true;

	std::ofstream *pFOUT = 0;
	if (output_filename.empty()) {
		pFOUT = 0;
	} else {
		pFOUT = new std::ofstream(output_filename.c_str());
	}

	if (pFOUT != 0 && !(*pFOUT)) {
		delete pFOUT;
		std::cerr << "ERROR: Could not open file '" << output_filename << "' for writing." << std::endl;
		return 4;
	}

	// from harvest/harvest_fts.cpp.
	EmdrosFTSStringFilter *pFilterChain = 0;

	if (stylesheet_name == "") {
		pFilterChain = stringFilterFactory(filter_string_list); 
	} else {
		std::string filter_error_message;
		pFilterChain = stringFilterFactoryJSONFileName(stylesheet_file_name, bResult, filter_error_message, stylesheet_name);
		if (!bResult) {
			std::cerr << "ERROR doing parsing the filters:\n" 
				  << filter_error_message
				  << std::endl;
		}
	}

	if (bResult) {
		BookcaseIndexer *pIndexer = 0;
		if (fts_engine_version == 1) {
			pIndexer = new BookcaseIndexer(pSourceEE, pTargetEE, pFOUT, bookcase_otn, indexed_otn, indexed_feature, pFilterChain);
		} else if (fts_engine_version == 2) {
			pIndexer = new BookcaseIndexer2(pSourceEE, pTargetEE, pFOUT, bookcase_otn, indexed_otn, indexed_feature, pFilterChain, 50000);
		} else {
			std::cerr << "ERROR: Unknown FTS engine version: " << fts_engine_version << ". Aborting." << std::endl;
			bResult = false;
		}

		if (bResult) {
			std::cout << "About to start indexing..." << std::endl;
			// TODO: Add --fm and --lm options, using them here!
			// Right now, the empty set means 'everything'.
			if (!pIndexer->process(SetOfMonads(), bEmitVacuum)) {
				std::cerr << "ERROR doing the indexing:\n" 
					  << pIndexer->errorMessage()
					  << std::endl;
				bResult = false;
			} else {
				bResult = true;
			}
		}
		delete pIndexer;
	}

	delete pFOUT;
	delete pFilterChain;

	if (bResult) {
		std::cout << "...Finished indexing!" << std::endl;
		return 0;
	} else {
		std::cerr << "...Indexing finished with error!" << std::endl;
		return 5;
	}
}

void show_version(std::ostream& ostr)
{
	ostr << "fts_indexer from Emdros version " << EMDROS_VERSION << std::endl;
}

void print_usage(std::ostream& ostr)
{
	show_version(ostr);
	ostr << "Usage:" << std::endl;
	ostr << "   fts_indexer [options]" << std::endl;
	ostr << "   fts_indexer (--help|-V|--version)" << std::endl;
	ostr << "OPTIONS:" << std::endl;
	printUsageStandardArguments(ostr);
	ostr << "   -fe , --fts-engine (1|2)       Use the given FTS engine version" << std::endl;
	ostr << "   -d , --dbname <dbname>  Set db name to read from, and also to write to,\n"
	     << "                           unless -td is used to indicate where to write." << std::endl;
	ostr << "   -td , --target-dbname <dbname>  Set target database name" << std::endl;
	ostr << "   -th , --target-host <hostname> Set target database host name" << std::endl;
	ostr << "   -tu , --target-user <user> Set target password user name" << std::endl;
	ostr << "   -tp , --target-password <password> Set target database password" << std::endl;
	ostr << "   -tb , --target-backend <backend> Set target database backend." << std::endl;
	ostr << "                        <backend> must be drawn from the same values" << std::endl;
	ostr << "                        as for the -b switch." << std::endl;
	ostr << "   -sf , --stylesheet-filename <stylesheet-filename> The name of the file" << std::endl;
	ostr << "                        from which to read the JSON stylesheet." << std::endl;
	ostr << "   -s , --stylesheet <stylesheet-name> The name of the stylesheet to use" << std::endl;
	ostr << "                        from the stylesheet file." << std::endl;
	ostr << "   --bookcase-otn <otn> Use otn as the object type for the bookcase" << std::endl;
	ostr << "   --indexed-otn <otn>  Use otn as the object type to be indexed" << std::endl;
	ostr << "   --indexed-feature <fn>  Use comma-separated list fn as the feature name(s)\n"
	     << "                           to index on the indexed object type to index." << std::endl;
	ostr << "   --nodb               Do not alter target database" << std::endl;
	ostr << "   --no-vacuum          Do not emit 'VACUUM DATABASE ANALYZE GO' at the end." << std::endl;
	ostr << "   -o , --output <filename>  Output to filename" << std::endl;
	ostr << "DEFAULTS:" << std::endl;
	ostr << "   -fe 1" << std::endl;
	printUsageDefaultsOfStandardArguments(ostr);

}

int main(int argc, char* argv[])
{
	// Set defaults
	bool bDontUseTargetDB = false;
	bool bEmitVacuum = true;
	eOutputKind output_kind = kOKConsole;
	eCharsets charset = kCSUTF8;
	eBackendKind source_backend_kind;
	eBackendKind target_backend_kind;
	std::string output_filename;
	std::string source_db_name;
	std::string target_db_name;
	std::string source_hostname;
	std::string target_hostname;
	std::string source_user;
	std::string target_user;
	std::string source_password;
	std::string target_password;
	std::string bookcase_otn;
	std::string indexed_otn;
	std::string indexed_feature;
	std::string stylesheet_file_name;
	std::string stylesheet_name;
	StringList filter_string_list;
	int fts_engine_version;

	addStandardArguments(false // Do add -e | --encoding
			     );

	addOption("-s", "--stylesheet", true,
		  "",
		  "ERROR: -s and --stylesheet must have a stylesheet name as their argument.\n"
		  "       example: -s 'englishstylesheet'\n");

	addOption("-sf", "--stylesheet-filename", true,
		  "",
		  "ERROR: -sf and --stylesheet-filename must have a filename as their argument.\n"
		  "       example: -sf /home/myuser/stylesheet.json\n");

	addOption("-td", "--target-dbname", true,
		  "",
		  "ERROR: -td and --target-dbname must have a database-name as their argument.\n"
		  "       example: -td 'mydb'\n");

	addOption("-tp", "--target-password", true,
		  "",
		  "ERROR: -tp and --target-password must have a password as their argument.\n"
		  "       example: -tp 'sicret'\n");
	addOption("-th", "--target-host", true,
		  "",
		  "ERROR: -th and --target-host must have a database hostname as their argument.\n"
		  "       example: -tp 'sicret'\n");
	addOption("-tu", "--target-user", true,
		  "",
		  "ERROR: -tu and --target-user must have a database user as their argument.\n"
		  "       example: -tp 'sicret'\n");
	addOption("--bookcase-otn", "--bookcase-otn", true,
		  "",
		  "ERROR: --bookcase-otn must have an object type name as its argument.\n"
		  "       example: --bookcase-otn Document\n");
	addOption("--indexed-otn", "--indexed-otn", true,
		  "",
		  "ERROR: --indexed-otn must have an object type name as its argument.\n"
		  "       example: --indexed-otn Word\n");
	addOption("--indexed-feature", "--indexed-feature", true,
		  "",
		  "ERROR: --indexed-feature must have a feature name as its argument.\n"
		  "       example: --indexed-feature surface\n");
	addOption("--nodb", "--nodb", false);
	addOption("--no-vacuum", "--no-vacuum", false);

	addOption("-d", "--dbname", true, 
		  "emdf",
		  "ERROR: -d and --dbname must have a database-name as their argument.\n"
		  "       example: -d book1\n");
	addOption("-o", "--output", true, 
		  "",
		  "ERROR: -o and --output must have a filename as their argument.\n"
		  "       example: -o indexcreation.mql\n");
	addOption("-fe", "--fts-engine", true, 
		  "1",
		  "ERROR: -fe and --fts-engine must have a number as their argument.\n"
		  "       example: -fe 2\n");

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
		if (!getStandardArguments(bShowVersion, bShowHelp,
					  source_hostname,
					  source_user,
					  source_password,
					  source_backend_kind,
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

		// stylesheet name
		if (getArgumentPresent("-s")) {
			getArgumentValue("-s", stylesheet_name);
		} else {
			stylesheet_name = "";
		}

		// stylesheet filename
		if (getArgumentPresent("-sf")) {
			getArgumentValue("-sf", stylesheet_file_name);
		} else {
			stylesheet_file_name = "";
		}

		if ((stylesheet_name != "" &&
		     stylesheet_file_name == "")
		    || (stylesheet_name == "" &&
			stylesheet_file_name != "")) {
			print_usage(std::cerr);
			std::cerr << "The -s <stylesheet> and -sf <stylesheet-filename> options\nmust be given together!" << std::endl;
			return 1;
		}

		// source database name
		if (getArgumentPresent("-d")) {
			getArgumentValue("-d", source_db_name);
		} else {
			print_usage(std::cerr);
			std::cerr << "The -d <database-name> option is required!" << std::endl;
			return 1;
		}

		// target database name
		if (getArgumentPresent("-td")) {
			getArgumentValue("-td", target_db_name);
		} else {
			target_db_name = source_db_name;
		}

		// target database hostname
		if (getArgumentPresent("-th")) {
			getArgumentValue("-th", target_hostname);
		} else {
			target_hostname = source_hostname;
		}

		// target database user
		if (getArgumentPresent("-tu")) {
			getArgumentValue("-tu", target_user);
		} else {
			target_user = source_user;
		}

		// target database password
		if (getArgumentPresent("-tp")) {
			getArgumentValue("-tp", target_password);
		} else {
			target_password = source_password;
		}

		// target database backend
		if (getArgumentPresent("-tb")) {
			std::string target_backend_name;
			if (!string2backend_kind(target_backend_name, target_backend_kind)) {
				print_usage(std::cerr);
				std::cerr << "Unknown target backend kind '" << target_backend_name << "'" << std::endl;
				return 1;
			}
		} else {
			target_backend_kind = source_backend_kind;
		}

		// bookcase-otn
		if (getArgumentPresent("--bookcase-otn")) {
			getArgumentValue("--bookcase-otn", bookcase_otn);
		} else {
			print_usage(std::cerr);
			std::cerr << "The --bookcase-otn <bookcase-otn> option is required!" << std::endl;
			return 1;
		}

		// indexed-otn
		if (getArgumentPresent("--indexed-otn")) {
			getArgumentValue("--indexed-otn", indexed_otn);
		} else {
			print_usage(std::cerr);
			std::cerr << "The --indexed-otn <indexed-otn> option is required!" << std::endl;
			return 1;
		}

		// indexed-feature
		if (getArgumentPresent("--indexed-feature")) {
			getArgumentValue("--indexed-feature", indexed_feature);
		} else {
			print_usage(std::cerr);
			std::cerr << "The --indexed-feature <indexed-feature> option is required!" << std::endl;
			return 1;
		}

		// output_filename
		if (getArgumentPresent("-o")) {
			getArgumentValue("-o", output_filename);
		} else {
			output_filename = "";
		}

		// fts engine version
		if (getArgumentPresent("-fe")) {
			std::string fts_engine_str;
			getArgumentValue("-fe", fts_engine_str);
			fts_engine_version = (int) string2long(fts_engine_str);
		} else {
			fts_engine_version = 1;
		}

		if (getArgumentPresent("--nodb")) {
			bDontUseTargetDB = true;
		}  else {
			bDontUseTargetDB = false;
		}

		if (getArgumentPresent("--no-vacuum")) {
			bEmitVacuum = false;
		} else {
			bEmitVacuum = true;
		}

		if (surplus_arguments.size() > 0) {
			if (stylesheet_name != "") {
				print_usage(std::cerr);
				std::cerr << "You cannot both give the -s and -sf options, and have filters on the\ncommandline." << std::endl;
				return 1;
			} else {
				std::list<std::string>::const_iterator ci = 
					surplus_arguments.begin();
				while (ci != surplus_arguments.end()) {
					filter_string_list.addStringBack(*ci);
					++ci;
				}
			}
		} 
	}

	if (!(fts_engine_version == 1 
	      || fts_engine_version == 2)) {
		error_message = "ERROR: Requested FTS engine version " + long2string(fts_engine_version) + " is not available.\n"
			+ "Please use either 1 or 2.";
		print_usage(std::cerr);
		std::cerr << error_message << std::endl;
		return 1;
	} 

	// Make connection

	EmdrosEnv *pSourceEE = new EmdrosEnv(output_kind, 
					     charset,
					     source_hostname,
					     source_user, source_password,
					     source_db_name,
					     source_backend_kind);

	EmdrosEnv *pTargetEE = 0;
	if (bDontUseTargetDB) {
		pTargetEE = 0;
	} else {
		if (source_db_name != target_db_name
		    || source_hostname != target_hostname
		    || source_user != target_user
		    || source_password != target_password
		    || source_backend_kind != target_backend_kind) {
			pTargetEE = new EmdrosEnv(output_kind, 
						  charset,
						  target_hostname,
						  target_user, target_password,
						  target_db_name,
						  target_backend_kind);
		} else {
			pTargetEE = pSourceEE;
		}
	}

	// Zero-fill passwords
	zeroFillString(source_password);
	zeroFillString(target_password);

	// Check that we connected to source
	if (!(pSourceEE->connectionOk())) {
		emdrosMessageConnectionNotOK(&std::cerr, source_backend_kind);
		if (pSourceEE == pTargetEE) {
			delete pSourceEE;
		} else {
			delete pSourceEE;
			delete pTargetEE;
		}
		return 2;
	}

	// Check that we connected to target
	if (pTargetEE && !pTargetEE->connectionOk()) {
		emdrosMessageConnectionNotOK(&std::cerr, target_backend_kind);
		if (pSourceEE == pTargetEE) {
			delete pSourceEE;
		} else {
			delete pSourceEE;
			delete pTargetEE;
		}
		return 2;
	}

	int nResult = 3;
	try {
		nResult = exec_indexer(pSourceEE, pTargetEE, output_filename, bookcase_otn, indexed_otn, indexed_feature, stylesheet_name, stylesheet_file_name, filter_string_list, bEmitVacuum, fts_engine_version);
	} catch (EMdFNULLValueException& e) {
		std::cerr << "ERROR: EMdFNULLValueException (Database error)..." << std::endl;
		std::cerr << pSourceEE->getDBError() << std::endl; 
		std::cerr << pSourceEE->getCompilerError() << std::endl;
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (EMdFDBDBError& e) {
		std::cerr << "ERROR: EMdFDBDBError (Database error)..." << std::endl;
		std::cerr << pSourceEE->getDBError() << std::endl; 
		std::cerr << pSourceEE->getCompilerError() << std::endl;
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (EMdFDBException& e) {
		std::cerr << "ERROR: EMdFDBException (Database error)..." << std::endl;
		std::cerr << pSourceEE->getDBError() << std::endl; 
		std::cerr << pSourceEE->getCompilerError() << std::endl;
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
	if (pSourceEE == pTargetEE) {
		delete pSourceEE;
	} else {
		delete pSourceEE;
		delete pTargetEE;
	}
	return nResult;
}


