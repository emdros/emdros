/*
 * emdros_utiltry.cpp
 *
 * Testing the util library of Emdros
 * Created: 1/27-2001
 * Last update: 1/11-2016
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2016  Ulrik Sandborg-Petersen
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


#include <iostream>
#include <sstream>
#include <debug.h>
#include <string>

#include <opt.h>
#include <messages.h>
#include <conf_ini.h>



#define VALUE_ERROR {				\
		result = 2;			\
		bContinue = false;		\
		bAllOK = false;			\
	}

void show_version(std::ostream& ostr)
{
	ostr << "emdros_utiltry from Emdros version " << EMDROS_VERSION << std::endl;
}


void print_usage(std::ostream& ostr)
{
	show_version(ostr);
	ostr << "Usage:\n";
	ostr << "   emdros_utiltry [options] \n";
	ostr << "   --help               Show this help" << std::endl;
	ostr << "   -V , --version       Show version" << std::endl;
}

bool test_conf_ini()
{
	bool bContinue = true;
	
	const char szConfigINI[] =
"#\n"
"# Test config file for the util/conf_ini.cpp implementation\n"
"#\n"
"\n"
"global_key1 = blah/blah/blah\n"
"global_key2 = \"yes\"\n"
"\n"
"[blah]\n"
"hostname = blah.example.com\n"
"user=any\n"
"password=secret \n"     // Trailing space
"backend = SQLite3\t\n"  //Trailing tab
"\n"
"[blah/blah/]\n"
"hostname = blah.example.com\n"
"user=any\n"
"password=secret\n"
"backend = SQLite3\n"
"\n";

	std::istringstream istr(szConfigINI);

	ConfigurationINI *pConf = new ConfigurationINI(&istr);

	StringVec expectedSectionVec;
	expectedSectionVec.push_back("");
	expectedSectionVec.push_back("blah");
	expectedSectionVec.push_back("blah/blah/");

	std::string expected_sections_str = joinVector(";", expectedSectionVec, 1024);

	StringVec actualSectionVec = pConf->getSections();
	std::string actual_sections_str = joinVector(";", actualSectionVec, 1024);

	if (bContinue) {
		if (expected_sections_str == actual_sections_str) {
			std::cout << "SUCCESS: test_conf_ini: expected sections were found.\n";
		} else {
			std::cerr << "ERROR: test_conf_ini: expected sections were NOT found. Actual sections = '" << actual_sections_str << "'\n";
			bContinue = false;
		}
	}

	if (bContinue) {
		StringVec expectedGlobalKeysVec;
		expectedGlobalKeysVec.push_back("global_key1");
		expectedGlobalKeysVec.push_back("global_key2");

		std::string expected_global_keys_str = joinVector(";", expectedGlobalKeysVec, 1024);
		
		StringVec actualGlobalKeysVec = pConf->getKeysInSection("");
		
		std::string actual_global_keys_str = joinVector(";", actualGlobalKeysVec, 1024);

		if (expected_global_keys_str == actual_global_keys_str) {
			std::cout << "SUCCESS: test_conf_ini: expected global keys were found.\n";
		} else {
			std::cerr << "ERROR: test_conf_ini: expected global keys were NOT found. Actual global keys = '" << actual_global_keys_str << "'\n";
			bContinue = false;
		}
	}

	if (bContinue) {
		const char szConfigINIStringRepr[] =
"global_key1 = blah/blah/blah\n"
"global_key2 = \"yes\"\n"
"\n"
"\n"
"[blah]\n"
"backend = SQLite3\n"
"hostname = blah.example.com\n"
"password = secret\n"
"user = any\n"
"\n"
"\n"
"[blah/blah/]\n"
"backend = SQLite3\n"
"hostname = blah.example.com\n"
"password = secret\n"
"user = any\n";
		
		std::string actual_string_representation = pConf->toString();
		std::string expected_string_representation = szConfigINIStringRepr;


		if (expected_string_representation == actual_string_representation) {
			std::cout << "SUCCESS: test_conf_ini: expected string representation was found.\n";
		} else {
			std::cerr << "ERROR: test_conf_ini: expected string representation was NOT found. Actual string representation = \n" << actual_string_representation << "\n";
			bContinue = false;
		}
	}

	delete pConf;
	
	return bContinue;
}

int test_util_library()
{
	std::cout << "----------------------------------\n"
		  << "Testing util library\n"
		  << "----------------------------------" << std::endl;

	int result = 0;
	bool bContinue = true;
	bool bAllOK = true;

	// Test ConfigurationINI.
	if (bContinue) {
		bContinue = test_conf_ini();
		

		if (bContinue) {
			std::cout << "SUCCESS: All tested ConfigurationINI methods passed.\n";
		} else {
			bAllOK = false;
		}
	}
	

	if (bAllOK) {
		std::cout << "SUCCESS: All tests completed successfully on the Emdros util library." << std::endl;
	} else {
		std::cout << "FAILURE: At least one test failed on the Emdros util library." << std::endl;
		result = 1;
	}

	return result;
}

int main(int argc, char* argv[])
{
	int result = 0;

	addOption("--help", "--help");
	addOption("-V", "--version");


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
		bShowVersion = getArgumentPresent("-V");
		bShowHelp = getArgumentPresent("--help");
	
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

		if (surplus_arguments.size() != 0) {
			print_usage(std::cout);
			return 0;
		}
	}


	result = test_util_library();
	

	if (result == 0) {
		std::cout << "SUCCESS: All tests completed successfully.\n";
	} else {
		std::cerr << "FAILURE: At least one test failed." << std::endl;
	}

	return result;
}


