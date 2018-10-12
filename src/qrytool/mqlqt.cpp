/*
 * mqlqt.cpp
 *
 * A tool to query Emdros databases.
 *
 * Created: 5/1-2001 (1st of May, 2001)
 * Last update: 3/20-2010
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

#include <monads.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <emdfdb.h>
#include <table.h>
#include <emdros-lconfig.h>
#include <emdf_output.h>
#include <emdros_environment.h>
#include "emdros_wrapper.h"
#include <encryption.h>
#include <conf.h>
#include "exec.h"
#include <prefix_emdros.h>
#include "consout.h"
#include <opt.h>
#include <messages.h>



std::string app_prefix(void)
{
#ifdef WIN32
	return prefix();
#else
	std::string myprefix = prefix() + "share/emdros/qrytool/";
	return myprefix;
#endif
}


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
	std::string configfile(app_prefix() + "default.cfg");
	Configuration *pConf;
	std::string password = "";
	eBackendKind backend_kind;
	eCharsets dummy_charset;

	addStandardArguments(false // Do not add -e | --encoding
			     );

	addOption("-c", "--config", true, 
		  std::string(app_prefix() + "default.cfg").c_str(),
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
	pConf = parse_config_file(configfile, app_prefix(), 
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


