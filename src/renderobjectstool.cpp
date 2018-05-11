/*
 * renderobjects.cpp
 *
 * A front-end to the harvest library's RenderObjects and RenderXML modules.
 *
 * Created: 9/16-2008
 * Last update: 5/11-2018
 *
 * Return codes:
 * 
 * 0 : Success
 * 1 : Wrong usage
 * 2 : Connection to backend server could not be established
 * 3 : An exception occurred
 * 4 : Could not open file
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2008-2018  Ulrik Sandborg-Petersen
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
#include <exception>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string_func.h>
#include <renderobjects.h>
#include <renderxml.h>
#include <emdros-lconfig.h>
#include <emdros_environment.h>
#include <opt.h>
#include <messages.h>
#include <encryption.h>

JSONValue *getStyleSheet(const std::string& stylesheet_filename)
{
	std::ifstream fin;
	fin.open(stylesheet_filename.c_str());
	if (!fin) {
		std::cerr << "FAILURE: Could not open stylesheet file " << stylesheet_filename << " for reading." << std::endl;
		std::cerr << "Action aborted." << std::endl;
		return 0;
	}
	std::string strResult;
	std::string line;
	while (!fin.eof() && !fin.fail() && !fin.bad()) {
		std::getline(fin, line);
		strResult += line;
		strResult += '\n';
	}
	
	if (!fin.eof()) {
		std::cerr << "FAILURE: Could not read contents of stylesheet file " << stylesheet_filename << "." << std::endl;
		std::cerr << "Action aborted." << std::endl;
		return 0;
	}

	JSONExecEnv *pJSONEE = new JSONExecEnv();
	bool bResult = false;
	JSONValue *pJSONValue = parseJSON(pJSONEE, strResult, bResult);
	if (!bResult) {
		std::cerr << "FAILURE while parsing stylesheet contents.  Here is any error message:\n"
			  << "----------------------------------------------------------------------\n"
			  << pJSONEE->getError() << '\n'
			  << "----------------------------------------------------------------------\n";
		delete pJSONEE;
		return 0;
	} else {
		delete pJSONEE;
		return pJSONValue;
	}
}


int exec_stream(EmdrosEnv *pEE,
		std::ostream& fout,
		const std::string& stylesheet_filename,
		const std::string& stylesheet_name,
		const std::string& dbname,
		bool bRenderXML,
		monad_m first_monad,
		monad_m last_monad)
{
	JSONValue *pStylesheet = getStyleSheet(stylesheet_filename);
	
	
	if (pStylesheet == 0) {
		// Error has already been reported
		return 3;
	}
	

	if (pStylesheet->getKind() != kJSONObject) {
		std::cerr << "FAILURE: stylsheet does not consist of a JSON object." << std::endl;
		delete pStylesheet;
		return 3;
	}
	
	if (!pStylesheet->hasObjectKey("fetchinfo")) {
		std::cerr << "FAILURE: stylsheet does not contain a 'fetchinfo' key." << std::endl;
		delete pStylesheet;
		return 3;
	}
	
	const JSONValue *pFetchInfo = pStylesheet->getObjectValue("fetchinfo");
	
	if (pFetchInfo->getKind() != kJSONObject) {
		std::cerr << "FAILURE: stylsheet's 'fetchinfo' value is not a JSON object." << std::endl;
		delete pStylesheet;
		return 3;
	}

	if (bRenderXML) {
		if (!pStylesheet->hasObjectKey("renderinfo")) {
			std::cerr << "FAILURE: stylsheet does not contain a 'renderinfo' key." << std::endl;
			delete pStylesheet;
			return 3;
		}
		
		const JSONValue *pRenderInfo = pStylesheet->getObjectValue("renderinfo");
	
		if (pRenderInfo->getKind() != kJSONObject) {
			std::cerr << "FAILURE: stylsheet's 'renderinfo' value is not a JSON object." << std::endl;
			delete pStylesheet;
			return 3;
		}
		
		if (!pRenderInfo->hasObjectKey(stylesheet_name)) {
			std::cerr << std::string("renderinfo does not have key '") + stylesheet_name + "'." << std::endl;
			delete pStylesheet;
			return 3;
		}
		
		const JSONValue* pRenderStylesheet = (*pRenderInfo)[stylesheet_name];

		if (pRenderStylesheet->getKind() != kJSONObject) {
			std::cerr << "FAILURE: stylsheet's 'renderinfo[" << stylesheet_name <<"]' value is not a JSON object." << std::endl;
			delete pStylesheet;
			return 3;
		}
		
		if (!pRenderStylesheet->hasObjectKey("fetch_stylesheet")) {
			std::cerr << std::string("renderinfo[") + stylesheet_name + "] does not have key 'fetch_stylesheet'" << std::endl;
			delete pStylesheet;
			return 3;
		}

		std::string fetch_stylesheet = (*pRenderStylesheet)["fetch_stylesheet"]->getString();
		
		RenderObjects *pRO = new RenderObjects(pEE, dbname, pStylesheet, fetch_stylesheet);
		if (!pRO->process(first_monad, last_monad)) {
			std::cerr << "FAILURE:\n" 
				  << pRO->getError()
				  << std::endl;
			delete pRO;
			delete pStylesheet;
			return 3;
		} else {
			std::string xml = pRO->getDocument();
			delete pRO;
			
			bool bProcessOK = false;
			std::string strError;
			fout << RenderXML(xml, pStylesheet, stylesheet_name, bProcessOK, strError);
			if (!bProcessOK) {
				std::cerr << strError << std::endl;
				delete pStylesheet;
				return 3;
			} else {	
				delete pStylesheet;
				return 0;
			}
		}
	} else {
		RenderObjects *pRO = new RenderObjects(pEE, dbname, pStylesheet, stylesheet_name);
		if (!pRO->process(first_monad, last_monad)) {
			std::cerr << "FAILURE:\n" 
				  << pRO->getError()
				  << std::endl;
			delete pRO;
			delete pStylesheet;
			return 3;
		} else {
			fout << pRO->getDocument();
			delete pRO;
			delete pStylesheet;
			return 0;
		}
	}
}

int exec_cout(EmdrosEnv *pEE,
	     const std::string& stylesheet_filename,
	     const std::string& stylesheet_name,
	     const std::string& dbname,
	     bool bRenderXML,
	     monad_m first_monad,
	     monad_m last_monad)
{
	return exec_stream(pEE, std::cout, stylesheet_filename, stylesheet_name, dbname, bRenderXML, first_monad, last_monad);
}

int exec_file(EmdrosEnv *pEE,
	      const std::string& output_filename,
	      const std::string& stylesheet_filename,
	      const std::string& stylesheet_name,
	      const std::string& dbname,
	      bool bRenderXML,
	      monad_m first_monad,
	      monad_m last_monad)
{
	std::ofstream fout;
	fout.open(output_filename.c_str());
	if (!fout) {
		std::cerr << "FAILURE: Could not open file " << stylesheet_filename << " for writing." << std::endl;
		std::cerr << "Action aborted." << std::endl;
		return 4;
	} else {
	        return exec_stream(pEE, fout, stylesheet_filename, stylesheet_name, dbname, bRenderXML, first_monad, last_monad);
	}
}


void show_version(std::ostream& ostr)
{
	ostr << "renderobjects from Emdros version " << EMDROS_VERSION << std::endl;
}

void print_usage(std::ostream& ostr)
{
	show_version(ostr);
	ostr << "Usage:" << std::endl;
	ostr << "   renderobjects [options] stylesheet-filename databasename" << std::endl;
	ostr << "   renderobjects (--help|-V|--version)" << std::endl;
	ostr << "OPTIONS:" << std::endl;
	ostr << "   -s, --stylesheet      RenderObjects stylesheet name" << std::endl;
	ostr << "   -o, --output filename Name of file to output to (stdout if absent or '-')" << std::endl;
	printUsageStandardArguments(ostr);
	ostr << "   --fm monad            First monad to harvest" << std::endl;
	ostr << "   --lm monad            Last  monad to harvest" << std::endl;
	ostr << "   --renderxml           Don't just render objects, render XML" << std::endl;
	ostr << "DEFAULTS:" << std::endl;
	ostr << "   -s base" << std::endl;
	ostr << "   -o -                  (stdout)" << std::endl;
	ostr << "   --fm 1" << std::endl;
	ostr << "   --lm " << MAX_MONAD << std::endl;
	printUsageDefaultsOfStandardArguments(ostr);
}

int main(int argc, char* argv[])
{
	// Set defaults
	eOutputKind output_kind = kOKConsole;
	std::string stylesheet_filename;
	std::string initial_db;
	std::string output_filename;
	std::string stylesheet_name;
	std::string hostname;
	std::string user;
	std::string password;
	bool bRenderXML;
	monad_m first_monad;
	monad_m last_monad;
	eBackendKind backend_kind;
	eCharsets charset;

	addStandardArguments(false // Do not add -e | --encoding
			     );

	addOption("--renderxml", "--renderxml",
		  false, "",
		  ""
	    );
	addOption("-s", "--stylesheet",
		  true, "base",
		  "ERROR: -s and --stylesheet must have a stylesheet name as their argument.\n"
		  "       example: -s base\n"
	    );
	addOption("-o", "--output",
		  true, "-",
		  "ERROR: -o and --output must have an output filename as their argument.\n"
		  "       example: -o out.xml\n");
	addOption("--fm", "--fm",
		  true, "1",
		  "ERROR: --fm must have a monad as its argument.\n"
		  "       example: --fm 2001\n"
	    );
	addOption("--lm", "--lm",
		  true, monad_m2string(MAX_MONAD).c_str(),
		  "ERROR: --lm must have a monad as its argument.\n"
		  "       example: --lm 4000\n");

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

		getArgumentValue("-s", stylesheet_name);

		getArgumentValue("-o", output_filename);

		std::string first_monad_str;
		getArgumentValue("--fm", first_monad_str);
		if (string_is_number(first_monad_str)) {
		    first_monad = string2monad_m(first_monad_str);
		} else {
			print_usage(std::cerr);
			std::cerr << "ERROR: --fm must take an integer (i.e., a monad) as its argument.\n"
				  << "You specified --fm '" << first_monad_str << "', which is not an integer." << std::endl;
			return 1;
		}

		std::string last_monad_str;
		getArgumentValue("--lm", last_monad_str);
		if (string_is_number(last_monad_str)) {
		    last_monad = string2monad_m(last_monad_str);
		} else {
			print_usage(std::cerr);
			std::cerr << "ERROR: --lm must take an integer (i.e., a monad) as its argument.\n"
				  << "You specified --lm '" << last_monad_str << "', which is not an integer." << std::endl;
			return 1;
		}


		bRenderXML = getArgumentPresent("--renderxml");

		if (surplus_arguments.size() == 2) {
		    std::list<std::string>::const_iterator ci = surplus_arguments.begin();
		    stylesheet_filename = *ci;
		    ++ci;
		    initial_db = *ci;
		} else {
			std::cerr << "Error: You must specify first a stylesheet filename, then a database name.\nYou specified:\n"
				  << "'" << joinList("' and '", surplus_arguments) << "'." << std::endl;
			return 1;
		}
	}

	// Make connection
	EmdrosEnv *pEE = new EmdrosEnv(output_kind, 
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

	int nResult = 3;
	try {
	        if (output_filename == "-") {
			nResult = exec_cout(pEE, stylesheet_filename, stylesheet_name, initial_db, bRenderXML, first_monad, last_monad);
		} else {
			nResult = exec_file(pEE, output_filename, stylesheet_filename, stylesheet_name, initial_db, bRenderXML, first_monad, last_monad);
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
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (...) {
		std::cerr << "Unknown exception occurred.  Program aborted." << std::endl;
	} 

	// Clean up
	delete pEE;
	return nResult;
}


