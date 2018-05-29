/*
 * mqlqt.cpp
 *
 * A tool to query Emdros databases.
 *
 * Created: 5/1-2001 (1st of May, 2001)
 * Last update: 11/10-2017
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
 *   Copyright (C) 2001-2017  Ulrik Sandborg-Petersen
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


#include "emdros-config.h"
// Do this because emdros-config might #define malloc,
// in which case some systems will fail to use the real 
// malloc, thus causing build errors.
#undef malloc

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include <emdros-lconfig.h>
#include <emdros.h>

#include <emdros_wrapper.h>
#include <exec.h>
#include <consout.h>



void show_version(std::ostream& ostr)
{
	ostr << "eqtc from Emdros version " << EMDROS_VERSION << std::endl;
}

void print_usage(std::ostream& ostr)
{
	show_version(ostr);
	ostr << "Usage:\n";
	ostr << "  eqtc [options] [filename] \n";
	ostr << "       If no filename is given, use stdin.\n";
	ostr << "   eqtc (--help|-V|--version)\n";
	ostr << "OPTIONS:\n";
	printUsageStandardArguments(ostr);
	ostr << "   -c , --config file   Use this config file\n";
	ostr << "   -d , --db database   Use this database\n";
	ostr << "DEFAULTS:\n";
	printUsageDefaultsOfStandardArguments(ostr);
}

int main(int argc, char* argv[])
{
	// Set defaults
	eOutputKind output_kind = kOKConsole;
	std::string initial_db("");
	std::string filename;
	std::string hostname("localhost");
	std::string user("emdf");
	std::string configfile;
	Configuration *pConf;
	std::string password = "";
	eBackendKind backend_kind;
	eCharsets dummy_charset;

	addStandardArguments(false // Do not add -e | --encoding
			     );

	addOption("-c", "--config", true, 
		  "",
		  "ERROR: -c and --config must have a filename as their argument.\n"
		  "       example: -c /home/joe/mygreatdb.cfg\n"
		  "       example: -c penn.cfg\n");

	addOption("-d", "--dbname", true, 
		  "",
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

		// -d | --dbname
		getArgumentValue("-d", initial_db);

		// -c | --config
		getArgumentValue("-c", configfile);

		if (surplus_arguments.size() == 0) {
			filename = "";
		} else if (surplus_arguments.size() == 1) {
			filename = *surplus_arguments.begin();
		} else {
			std::cerr << "Error: You can only specify one filename. You specified: '" << joinList("' and '", surplus_arguments) << "'." << std::endl;
			return 1;
		}
	}
  
	std::cerr << "Using config file " << configfile << std::endl;
	ConsoleQTOutput consoutput(&std::cout, false);
	pConf = parse_config_file(configfile, "",
				  &std::cout);
	if (pConf == 0) {
		std::cerr << "Error: Could not parse config file '" << configfile << "'." << std::endl;
		return 1;
	} 


	std::string error_msg;
	if (!app_checkConfiguration(pConf, error_msg)) {
		delete pConf;
		std::cerr << error_msg 
			  << "\nERROR: Configuration file inconsistent." << std::endl;
		return 1;
	}

	if (initial_db == "") {
		if (pConf->has_key("database")) {
			initial_db = pConf->getValues("database")[0];
		} else {
			initial_db = "emdf";
		}
	}

	// Make connection
	EmdrosEnv *pEE = new EmdrosEnv(output_kind, kCSISO_8859_1,
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
		delete pConf;
		return 2;
	}

	int nResult = 0;
	EmdrosWrapper *pEW = new EmdrosWrapper(pEE, pConf, &consoutput);
	std::string message;
	if (!pEW->configurePOut(message)) {
		std::cerr << "Something went wrong in configuring output." << std::endl;
		std::cerr << "Here's what:" << std::endl;
		std::cerr << "--------------------------------------" << std::endl;
		if (message.length() == 0) {
			std::cerr << "No message." << std::endl;
		} else {
			std::cerr << message << std::endl;
		}
		std::cerr << "--------------------------------------" << std::endl;
		std::cerr << "Aborting." << std::endl;
		delete pEE;
		delete pConf;
		return 3;
	}

	if (filename == "") {
		nResult = exec_cin(pEW);
	} else {
		nResult = exec_file(filename, pEW);
	}

	// Clean up
	delete pEW;
	delete pEE;
	delete pConf;


	return nResult;
}


