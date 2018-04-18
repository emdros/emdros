/*
 * hal.cpp
 *
 * A tool to query Emdros databases.
 *
 * Created: 5/1-2001 (1st of May, 2001)
 * Last update: 9/17-2017
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
#include <encryption.h>
#include <conf.h>
#include <prefix_emdros.h>
#include "HALSpace.h"
#include <fstream>
#include <opt.h>
#include <messages.h>



#define FIRST_HAL_MONAD    (1000000)  

std::string app_prefix()
{
#ifdef WIN32
	return prefix() + "..\\etc\\hal\\";
#else
	return prefix() + "share/emdros/HAL/";
#endif
}

int exec(EmdrosEnv *pEE, Configuration *pConf, std::ostream *pOut)
{
	int n = string2long(pConf->getValues("n")[0]);
	HALSpace *pHALSpace = new HALSpace(n, FIRST_HAL_MONAD, pEE);
  
	pHALSpace->load();

	std::ofstream fcsvout;
	bool bDoCSV = pConf->has_key("csv");
	if (bDoCSV) {
		fcsvout.open(pConf->getValues("csv")[0].c_str());
		if (!fcsvout) {
			std::cerr << "Error: Could not open '" << pConf->getValues("csv")[0] << "' for writing.  Not writing CSV." << std::endl;
		} else {
			if (pConf->has_key("csv_word")) {
				pHALSpace->write_to_csv_file(&fcsvout, pConf->getValues("csv_word"));
			} else {
				StringVec empty_csv_words;
				pHALSpace->write_to_csv_file(&fcsvout, empty_csv_words);
			}
			fcsvout.close();
		}
	}

	std::ofstream fsimcsvout;
	bool bDoSimilarityCSV = pConf->has_key("similarity_csv");
	if (bDoSimilarityCSV) {
		std::string simcsv_filename = pConf->getValues("similarity_csv")[0];
		fsimcsvout.open(simcsv_filename.c_str());
		if (!fsimcsvout) {
			std::cerr << "Error: Could not open '" << simcsv_filename << "' for writing.  Not writing similarity." << std::endl;
		} else {
			pHALSpace->write_similarity_measures(&fsimcsvout);

			fsimcsvout.close();
		}
	}

	std::string output_filename = pConf->getValues("output")[0];
  
	std::ofstream fout;
	fout.open(output_filename.c_str());
	if (!fout) {
		std::cerr << "Error: Could not open '" << output_filename << "' for writing.\nNot outputting vectors." << std::endl;

	} else {
		(*pOut) << "Now writing output to '" << output_filename << "'";
		(*pOut) << '\n';
		pHALSpace->write_word_vectors(pConf->getValues("word"),
					      string2long(pConf->getValues("factor")[0]),
					      string2long(pConf->getValues("max_values")[0]),
					      &fout);
		fout.close();
	}


	// Write SQL
	std::ofstream foutSQL;
	std::string SQLoutput_filename = "sql.txt";
	foutSQL.open(SQLoutput_filename.c_str());
	if (!foutSQL) {
		std::cerr << "Error: Could not open '" << SQLoutput_filename << "' for writing.\nNot outputting SQL." << std::endl;

	} else {
		(*pOut) << "Now writing output to '" << SQLoutput_filename << "'";
		(*pOut) << '\n';
		pHALSpace->writeSQL(&foutSQL);
		foutSQL.close();
	}

	// Write CXT
	std::ofstream foutCXT;
	std::string CXToutput_filename = "HALSpace.cxt";
	foutCXT.open(CXToutput_filename.c_str());
	if (!foutCXT) {
		std::cerr << "Error: Could not open '" << CXToutput_filename << "' for writing.\nNot outputting CXT." << std::endl;

	} else {
		(*pOut) << "Now writing output to '" << CXToutput_filename << "'";
		(*pOut) << '\n';
		int threshold = 25;
		pHALSpace->writeCXT(&foutCXT, pConf->getValues("word"), threshold); 
		foutCXT.close();
	}





	delete pHALSpace;
	return 0;
}

void show_version(std::ostream& ostr)
{
	ostr << "mqlhal from Emdros version " << EMDROS_VERSION << std::endl;
}

void print_usage(std::ostream& ostr)
{
	show_version(ostr);
	ostr << "Usage:\n";
	ostr << "   hal [options] \n";
	ostr << "   hal (--help|-V|--version)\n";
	ostr << "OPTIONS:\n";
	printUsageStandardArguments(ostr);
	ostr << "   -c , --config file   Use this config file\n";
	ostr << "   -d , --db database   Use this database\n";
	ostr << "DEFAULTS:\n";
	ostr << "   -d emdf\n";
	ostr << "   -c " << app_prefix() << "default.cfg" << std::endl;
	printUsageDefaultsOfStandardArguments(ostr);
}

bool app_checkConfiguration(Configuration *pConf, std::string& error_msg)
{
	// Check for consistency
	StringVec keysToCheck;
	keysToCheck.push_back("factor");
	keysToCheck.push_back("n");
	keysToCheck.push_back("csv");
	keysToCheck.push_back("output");
	keysToCheck.push_back("max_values");
	keysToCheck.push_back("word");
	std::ostringstream ssout;
	if (!pConf->check(keysToCheck, &ssout)) {
		error_msg = ssout.str()
			+ "Error: at least one key which needed to be defined\n"
			+ "was not defined in the configuration file.\n";
		return false;
	}
	return true;
}



int main(int argc, char* argv[])
{
	// Set defaults
	eOutputKind output_kind = kOKConsole;
	std::string initial_db;
	std::string filename;
	std::string hostname;
	std::string user;
	std::string password;
	std::string configfile(app_prefix() + "default.cfg");
	Configuration *pConf;
	eBackendKind backend_kind;
	eCharsets dummy_charset;

	addStandardArguments(false // Do not add -e | --encoding
			     );

	addOption("-c", "--config", true, 
		  std::string(app_prefix() + "default.cfg").c_str(),
		  "ERROR: -c and --config must have a filename as their argument.\n"
		  "       example: -c /home/joe/mygreatdb.cfg\n"
		  "       example: -c hal.cfg\n");

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

		if (surplus_arguments.size() != 0) {
			print_usage(std::cerr);
			std::cerr << "ERROR: Extraneous argument(s) not parsed: '" << joinList("' and '", surplus_arguments) << "'." << std::endl;
			return 1;
		}
	}

	pConf = parse_config_file(configfile, app_prefix(), &std::cout);
	if (pConf == 0) {
		std::cerr << "Error: Could not parse config file '" << configfile << "'." << std::endl;
		return 1;
	}

	std::string error_msg;
	if (!app_checkConfiguration(pConf, error_msg)) {
		delete pConf;
		std::cerr << error_msg 
			  << "\nConfiguration file inconsistent." << std::endl;
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
	nResult = exec(pEE, pConf, &std::cout);

	// Clean up
	delete pEE;
	delete pConf;

	return nResult;
}


