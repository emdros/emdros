/*
 * fts_harvest.cpp
 *
 * A front-end to the BookcaseHarvester class in the libharvest library.
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
#include <harvest_fts3.h>


int exec_harvester(EmdrosEnv *pEnv, const std::string& output_filename, int fts_engine_version, const std::string& bookcase_otn, const std::string& indexed_otn, const std::string& indexed_feature, const std::string& stylesheet_name, const std::string& stylesheet_file_name, const StringList& query_string_list, long largest_proximity, bool bUseGoogleSyntax, bool bGetXHTML, bool bSingleBookcaseHits, const std::string& bookcase_title_otn, const std::string& bookcase_title_feature, int hits_per_page, int page_number)
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
		pFilterChain = 0;
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
		BookcaseHitList result;
		std::string error_message;
			
		std::string query;
		StringListConstIterator qlci = query_string_list.const_iterator();
		while (qlci.hasNext()) {
			std::string this_term = qlci.next();
			query += this_term;
			if (qlci.hasNext()) {
				query += " ";
			}
		}
			
		if (fts_engine_version == 3) {
			bResult = harvestFTS3(pEnv, bUseGoogleSyntax, bookcase_otn, indexed_otn, indexed_feature, SetOfMonads(1, MAX_MONAD), query, largest_proximity, bSingleBookcaseHits, error_message, result);
		} else {
			if (bUseGoogleSyntax) {
				bResult = harvestFTSGoogle_with_version(pEnv, fts_engine_version, bookcase_otn, indexed_otn, indexed_feature, SetOfMonads(1, MAX_MONAD), query, largest_proximity, pFilterChain, bSingleBookcaseHits, error_message, result);
			} else {
				bResult = harvestFTSNonGoogle_with_version(pEnv, fts_engine_version, bookcase_otn, indexed_otn, indexed_feature, SetOfMonads(1, MAX_MONAD), query, largest_proximity, pFilterChain, bSingleBookcaseHits, error_message, result);
			}
		}
			
		if (bResult) {
			std::ostream *pOut;
			if (pFOUT == 0) {
				pOut = &std::cout;
			} else {
				pOut = pFOUT;
			}
			if (bGetXHTML) {
				// FIXME: Replace indexed_otn with token_otn
				// FIXME: Replaced indexed_feature with token_feature_name
				// FIXME: Replace 8 and 12 with monads_before_hit_word and monads_after_hit_word respectively
				result.setMaxHitsPerPage(hits_per_page);
				bResult = result.harvestHits(pEnv, bookcase_otn, bookcase_title_otn, bookcase_title_feature, indexed_otn, indexed_feature, 8, 12, query, pFilterChain, page_number);
				if (bResult) {
					BookcaseHitListConstIterator my_ci = result.const_iterator(page_number);
					if (my_ci.hasNext()) {
						(*pOut) << "<dl>\n";
						while (my_ci.hasNext()) {
							BookcaseHit myhit(my_ci.next());
							std::string snippet;
							std::string snippet_title;
							result.getSnippet(myhit, "<b>", "</b>", " ", true, snippet, snippet_title);
							(*pOut) << "<dt>" << snippet_title << "</dt>\n";
							(*pOut) << "<dd>" << snippet << "</dd>\n";
						}
						(*pOut) << "</dl>\n";
					}
				}
				if (page_number > 0) {
					(*pOut) << "<p>" << "Showing page " << page_number << " of " << result.getPageCount() << " pages.</p>\n";
				} else {
					(*pOut) << "<p>" << "Showing all results, which could have been split onto " << result.getPageCount() << " pages.</p>\n";
				}
			} else {
				BookcaseHitListConstIterator hitci = result.const_iterator();
				while (hitci.hasNext()) {
					BookcaseHit hit = hitci.next();
					(*pOut) << hit.m_bookcase_fm << '\t' << hit.m_bookcase_lm << '\t' << hit.m_hit_fm << '\n';
				}
				(*pOut) << std::flush;
			}
		}
			
		if (!bResult) {
			std::cerr << "ERROR doing harvesting:\n" 
			  << error_message
			  << std::endl;
		
		}
	}

	delete pFOUT;
	delete pFilterChain;

	if (bResult) {
		return 0;
	} else {
		std::cerr << "...Harvesting finished with error!" << std::endl;
		return 5;
	}
}

void show_version(std::ostream& ostr)
{
	ostr << "fts_harvester from Emdros version " << EMDROS_VERSION << std::endl;
}

void print_usage(std::ostream& ostr)
{
	show_version(ostr);
	ostr << "Usage:" << std::endl;
	ostr << "   fts_harvester [options] <query-terms>" << std::endl;
	ostr << "   fts_harvester (--help|-V|--version)" << std::endl;
	ostr << "OPTIONS:" << std::endl;
	printUsageStandardArguments(ostr);
	ostr << "   -d                   Set the database name to query (required)" << std::endl;
	ostr << "   -fe , --fts-engine (1|2|3)    Use the given FTS engine version" << std::endl;
	ostr << "   -g , --google-syntax If given, Google syntax will be used." << std::endl;
	ostr << "   --xhtml              If given, an XHTML snippet will be output." << std::endl;
	ostr << "   --bookcase-title-otn <object-type-name> The name of the OT for bc titles" << std::endl;
	ostr << "   -btf, --bookcase-title-feature <feature-name> The name of the feature" << std::endl;
	ostr << "                        from which to read the bookcase title. Only used" << std::endl;
	ostr << "                        with --xhtml." << std::endl;
	ostr << "   -sf , --stylesheet-filename <stylesheet-filename> The name of the file" << std::endl;
	ostr << "                        from which to read the JSON stylesheet." << std::endl;
	ostr << "   -s , --stylesheet <stylesheet-name> The name of the stylesheet to use" << std::endl;
	ostr << "                        from the stylesheet file." << std::endl;
	ostr << "   --bookcase-otn <otn> Use otn as the object type for the bookcase" << std::endl;
	ostr << "   --indexed-otn <otn>  Use otn as the object type to be indexed" << std::endl;
	ostr << "   --indexed-feature <fn>  Use fn as the feature name to index on\n"
	     << "                           the indexed object type to index." << std::endl;
ostr << "   -lp, --largest-proximity <integer>  Use this number as the largest\n"
	     << "                           proximity." << std::endl;
	ostr << "   -o , --output <filename>  Output to filename" << std::endl;
	ostr << "   -page <integer>            Select page (-1 selects all)" << std::endl;
	ostr << "   -hpp <positive-integer>    Max. hits per page" << std::endl;
	ostr << "DEFAULTS:\n";
	ostr << "   -fe 1" << std::endl;
	ostr << "   -lp " << MAX_MONAD << std::endl;
	ostr << "   -page -1 " << std::endl;
	ostr << "   -hpp 10 " << std::endl;

	printUsageDefaultsOfStandardArguments(ostr);
}

int main(int argc, char* argv[])
{
	// Set defaults
	eOutputKind output_kind = kOKConsole;
	eCharsets charset = kCSUTF8;
	eBackendKind source_backend_kind;
	std::string output_filename;
	std::string source_db_name;
	std::string source_hostname;
	std::string source_user;
	std::string source_password;
	std::string bookcase_otn;
	std::string indexed_otn;
	std::string indexed_feature;
	std::string stylesheet_file_name;
	std::string stylesheet_name;
	std::string bookcase_title_feature;
	std::string bookcase_title_otn;
	bool bUseGoogleSyntax = false;
	bool bSingleBookcaseHits = false;
	bool bGetXHTML = false;
	long largest_proximity;
	StringList query_string_list;
	int fts_engine_version = 1;
	int hits_per_page = 10;
	int page_number = -1;

	addStandardArguments(false // Do add -e | --encoding
			     );

	addOption("-fe", "--fts-engine", true,
		  "1",
		  "ERROR: -fe and --fts-engine must have a number (engine version) as their argument.\n"
		  "       example: -fe 1\n");

	addOption("-btf", "--bookcase-title-feature", true,
		  "",
		  "ERROR: -btf and --bookcase-title-feature must have a feature-name as their argument.\n"
		  "       example: -btf title\n");

	addOption("-btotn", "--bookcase-title-otn", true,
		  "",
		  "ERROR: -btotn and --bookcase-title-otn must have an object type name\n"
		  "       as their argument.\n"
		  "       example: -btotn enclosing_object_type\n");

	addOption("-lp", "--largest-proximity", true,
		  "",
		  "ERROR: -lp and --largest-proximity must have an integer as their argument.\n"
		  "       example: -lp 100\n");

	addOption("-hpp", "--hits-per-page", true,
		  "10",
		  "ERROR: -hpp and --hits-per-page must have an integer as their argument.\n"
		  "       example: -hpp 10\n");

	addOption("-page", "--page", true,
		  "-1",
		  "ERROR: -page and --page must have an integer as their argument.\n"
		  "       example: -page 0\n");

	addOption("-s", "--stylesheet", true,
		  "",
		  "ERROR: -s and --stylesheet must have a stylesheet name as their argument.\n"
		  "       example: -s 'englishstylesheet'\n");

	addOption("-sf", "--stylesheet-filename", true,
		  "",
		  "ERROR: -sf and --stylesheet-filename must have a filename as their argument.\n"
		  "       example: -sf /home/myuser/stylesheet.json\n");

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
	addOption("-g", "--google-syntax", false);
	addOption("--single-bookcase-hits", "--single-bookcase-hits", false);
	addOption("--xhtml", "--xhtml", false);

	addOption("-d", "--dbname", true, 
		  "emdf",
		  "ERROR: -d and --dbname must have a database-name as their argument.\n"
		  "       example: -d book1\n");
	addOption("-o", "--output", true, 
		  "",
		  "ERROR: -o and --output must have a filename as their argument.\n"
		  "       example: -o indexcreation.mql\n");

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

		// largest proximity
		if (getArgumentPresent("-lp")) {
			std::string largest_proximity_str;
			getArgumentValue("-lp", largest_proximity_str);
			largest_proximity = string2long(largest_proximity_str);
		} else {
			largest_proximity = MAX_MONAD;
		}

		// hits per page
		if (getArgumentPresent("-hpp")) {
			std::string hits_per_page_str;
			getArgumentValue("-hpp", hits_per_page_str);
			hits_per_page = string2long(hits_per_page_str);
		} else {
			hits_per_page = 10;
		}

		// page number
		if (getArgumentPresent("-page")) {
			std::string page_number_str;
			getArgumentValue("-page", page_number_str);
			page_number = string2long(page_number_str);
		} else {
			page_number = -1;
		}


		// largest proximity
		if (getArgumentPresent("-lp")) {
			std::string largest_proximity_str;
			getArgumentValue("-lp", largest_proximity_str);
			largest_proximity = string2long(largest_proximity_str);
		} else {
			largest_proximity = MAX_MONAD;
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

		// bookcase title feature name
		if (getArgumentPresent("-btf")) {
			getArgumentValue("-btf", bookcase_title_feature);
		} else {
			bookcase_title_feature = "";
		}

		// bookcase title object type name
		if (getArgumentPresent("-btotn")) {
			getArgumentValue("-btotn", bookcase_title_otn);
		} else {
			bookcase_title_otn = "";
		}


		// source database name
		if (getArgumentPresent("-d")) {
			getArgumentValue("-d", source_db_name);
		} else {
			print_usage(std::cerr);
			std::cerr << "The -d <database-name> option is required!" << std::endl;
			return 1;
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

		// -g, --google-syntax
		if (getArgumentPresent("-g")) {
			bUseGoogleSyntax = true;
		}

		// --single-bookcase-hits
		if (getArgumentPresent("--single-bookcase-hits")) {
			bSingleBookcaseHits = true;
		}

		// --xhtlm
		if (getArgumentPresent("--xhtml")) {
			bGetXHTML = true;
		}


		if (surplus_arguments.size() > 0) {
			std::list<std::string>::const_iterator ci = 
				surplus_arguments.begin();
			while (ci != surplus_arguments.end()) {
				query_string_list.addStringBack(*ci);
				++ci;
			}
		} 
	}

	if (!(fts_engine_version == 1 
	      || fts_engine_version == 2
	      || fts_engine_version == 3)) {
		std::string error_message = "ERROR: Requested FTS engine version " + long2string(fts_engine_version) + " is not available.\n"
			+ "Please use either 1, 2, or 3.";
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

	// Zero-fill passwords
	zeroFillString(source_password);

	// Check that we connected to source
	if (!(pSourceEE->connectionOk())) {
		emdrosMessageConnectionNotOK(&std::cerr, source_backend_kind);
		delete pSourceEE;
		return 2;
	}

	int nResult = 3;
	try {
		nResult = exec_harvester(pSourceEE, output_filename, fts_engine_version, bookcase_otn, indexed_otn, indexed_feature, stylesheet_name, stylesheet_file_name, query_string_list, largest_proximity, bUseGoogleSyntax, bGetXHTML, bSingleBookcaseHits, bookcase_title_otn, bookcase_title_feature, hits_per_page, page_number);
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
	delete pSourceEE;

	return nResult;
}


