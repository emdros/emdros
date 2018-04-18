/*
 * opt.cpp
 *
 * Classes and functions to support getting arguments from argc and argv
 *
 * Ulrik Petersen
 * Created: 7/19-2006
 * Last update: 4/18-2018
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2006-2018  Ulrik Sandborg-Petersen
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

#include <opt.h>
#include <debug.h>
#include <string_func.h>

// global variable
OptionMap_t theOptionMap;

bool getHasEncodingOption(void)
{
	OptionMap_t::const_iterator ci = theOptionMap.find("-e");
	if (ci == theOptionMap.end()) {
		return false;
	} else {
		return ci->second.long_version == "--encoding";
	}
}

std::string reduceOption(const std::string& in)
{
	std::string::size_type length = in.length();
	if (length == 0) {
		ASSERT_THROW(false, "opt.cpp: reduce_option: Cannot add empty option.");
		return std::string(""); // Just to fool the compiler into giving no warning...
	} else if (length == 1) {
		if (in[0] == '-') {
			return std::string("");
		} else {
			return in;
		}
	} else {
		if (in[0] == '-') {
			if (in[1] == '-') {
				return in.substr(2);
			} else {
				return in.substr(1);
			}
		} else {
			return in;
		}
	}
}

void addOption(const char *short_version, /* with or without the - . */
	       const char *long_version,  /* with or without the -- . */
	       bool takes_parameter,
	       const char *default_value,
	       const char *help_message_when_no_param)
{
	std::string sv = short_version;
	std::string lv = long_version;
	if (sv == lv
	    && sv.length() >= 2
	    && sv[0] == '-'
	    && sv[1] == '-') {
		sv = std::string("-") + reduceOption(sv); // Add "-" back on.
		lv = reduceOption(lv);
	} else {
		sv = reduceOption(sv);
		lv = reduceOption(lv);
	}
	option_t opt(sv.c_str(),
		     lv.c_str(),
		     takes_parameter,
		     default_value,
		     help_message_when_no_param);
	theOptionMap.insert(std::make_pair(opt.short_version,opt));
}

option_t& isOption(const std::string& argument, bool& bIsOption)
{
	//
	// It doesn't matter if the user fiddles with this one;
	// It is only returned when bIsOption is false.
	//
	static option_t dummy_opt("","");

	//
	if (argument.length() > 1
	    && argument[1] == '-') {
		// Find via a linear search
		OptionMap_t::iterator it, end;
		it = theOptionMap.begin();
		end = theOptionMap.end();
		while (it != end) {
			if (it->second.long_version == argument) {
				bIsOption = true;
				return it->second;
			} 
			++it;
		} 
	} else {
		// Find from the second character out.
		OptionMap_t::iterator it = theOptionMap.find(argument);
		if (it == theOptionMap.end()) {
			bIsOption = false;
			return dummy_opt;
		} else {
			bIsOption = true;
			return it->second;
		}
	}
	bIsOption = false;
	return dummy_opt;
}


bool parseArguments(int argc, char *argv[], std::string& error_message, std::list<std::string>& surplus_arguments)
{
	for (int i = 1; i < argc; ++i) {
		if (argv[i][0] != '-') {
			surplus_arguments.push_back(argv[i]);
		} else {
			// Starts with '-'
			std::string argument(argv[i]);
			bool bIsOption;
			option_t& opt = isOption(argument, bIsOption);
			if (!bIsOption) {
				error_message = "Unknown option: " + argument;
				return false;
			} else {
				// It is a known option
				if (opt.bTakesParameter) {
					++i;
					if (i >= argc) {
						std::string err = opt.help_string_when_no_param;
						if (err == "") {
							error_message = "Error: There must be a value after the option " + argument;
						} else {
							error_message = err;
						}
						return false;
					} else {
						opt.bPresent = true;
						opt.value = std::string(argv[i]);
					}
				} else {
					opt.bPresent = true;
				}
			}
		}
	}
	return true;
}

bool getArgumentValue(const char *short_version, std::string& value)
{
	OptionMap_t::const_iterator ci = theOptionMap.find(std::string(short_version));
	if (ci == theOptionMap.end()) {
		return false;
	} else {
		if (ci->second.bPresent) {
			value = ci->second.value;
			return true;
		} else {
			value = ci->second.default_value;
			return false;
		}
	}
}


bool getArgumentPresent(const char *short_version)
{
	OptionMap_t::const_iterator ci = theOptionMap.find(std::string(short_version));
	if (ci == theOptionMap.end()) {
		return false;
	} else {
		return ci->second.bPresent;
	}
}


/* Adds -h, -u, -p, -b, --help, --version. */
void addStandardArguments(bool bAddEncoding)
{
	addOption("--help", "--help");
	addOption("-V", "--version");

	addOption("-h", "--host", true,
		  "localhost",
		  "ERROR: -h and --host must have a hostname as their argument.\n"
		  "       example: -h mql.example.com\n");
	addOption("-u", "--user", true,
		  "emdf",
		  "ERROR: -u and --user must have a database user name as their argument.\n"
		  "       example: -u emdf\n");
	addOption("-p", "--password", true,
		  "",
		  "ERROR: -p and --password must have a database password as their argument.\n"
		  "       example: -p secret\n");
	addOption("-b", "--backend", true,
		  backend_kind2string(DEFAULT_BACKEND_ENUM).c_str(),
		  "ERROR: -b and --backend must have a database backend name as their argument.\n"
		  "       Valid values are:\n\n"
		  "       For PostgreSQL: p, pg, postgres, postgresql\n"
		  "       For MySQL: m, my, mysql\n"
		  "       For SQLite 3.X.X: 3, lt3, sqlite3, s3\n\n"
		  "       ... all are case-IN-sensitive.\n"
		  );
	if (bAddEncoding) {
		addOption("-e", "--encoding", true,
			  "UTF8",
			  "ERROR: -e and --encoding must have an encoding as their argument.\n"
			  "       example: -e UTF8\n"
			  "       example: -e iso_8859-1, -e iso_8859-2, ... , -e iso_8859-15\n"
			  );
	}
}


bool getStandardArguments(bool& bShowVersion, bool& bShowHelp, std::string& hostname, std::string& user, std::string& password, eBackendKind& backend_kind, eCharsets& charset, std::string& error_message)
{
	bShowVersion = getArgumentPresent("-V");
	bShowHelp = getArgumentPresent("--help");
	
	getArgumentValue("-h", hostname);
	getArgumentValue("-u", user);
	getArgumentValue("-p", password);

	std::string backend_str;
	if (getArgumentValue("-b", backend_str)) {
		if (!string2backend_kind(backend_str, backend_kind)) {
			error_message = std::string("Unknown backend for -b or --backend switch: '") + backend_str + "'.\n";
			return false;
		} 
	} else {
		backend_kind = DEFAULT_BACKEND_ENUM;
	}

	// Is encoding there?
	if (getHasEncodingOption()) {
		std::string encoding_str;
		getArgumentValue("-e", encoding_str);
		if (!string2charset(encoding_str, charset)) {
			error_message = "ERROR: Unknown encoding '" + encoding_str + "'.\n"
				+ "       Valid encodings:\n"
				+ "       -e UTF8\n"
				+ "       -e iso_8859-1, -e iso_8859_2, ..., -e iso_8859-15\n";
			return false;
		}
	}

	return true;
}

std::string getBackendsAvailable()
{

	std::list<std::string> backend_list;
	if (USE_POSTGRESQL) {
		backend_list.push_back("PostgreSQL");
	}
	if (USE_MYSQL) {
		backend_list.push_back("MySQL");
	}
	if (USE_SQLITE3) {
		backend_list.push_back("SQLite3");
	}
	if (USE_BPT) {
		backend_list.push_back("BPT");
	}
	if (USE_BPT2) {
		backend_list.push_back("BPT2");
	}
	std::string backend_string = joinList(", ", backend_list, 256);
	return backend_string;
}

void printBackendsAvailable(std::ostream& ostr)
{
	ostr << "BACKENDS AVAILABLE IN THIS BUILD:\n";

	std::string backend_string = getBackendsAvailable();
	ostr << "   " << backend_string << std::endl;	
}

void printUsageStandardArguments(std::ostream& ostr)
{
	ostr << "   --help               Show this help" << std::endl;
	ostr << "   -V , --version       Show version" << std::endl;
	if (getHasEncodingOption()) {
		ostr << "   -e , --encoding enc  Can either be iso_8859-1 to iso_8859-15, or UTF8" << std::endl;     
	}
	ostr << "   -h , --host host     Set hostname to connect to" << std::endl;
	ostr << "   -u , --user user     Set database user to connect as (default: 'emdf')" << std::endl;
	ostr << "   -p , --password pwd  Set password to use for database user" << std::endl;
	ostr << "   -b , --backend bkend Use this backend. Valid values:" << std::endl;
	ostr << "                        For MySQL: m, my, mysql" << std::endl;
	ostr << "                        For PostgreSQL: p, pg, postgres, postgresql" << std::endl;
	ostr << "                        For SQLite 3.X.X: 3, lt3, sqlite3, s3" << std::endl;
	ostr << "                        For the BPT engine version 1: b, bpt" << std::endl;
	ostr << "                        For the BPT engine version 2: b2, bpt2" << std::endl;
	ostr << "                        ... all are case-IN-sensitive." << std::endl;
	ostr << "                        BACKENDS AVAILABLE IN THIS BUILD:\n"
	     << "                        " << getBackendsAvailable() << std::endl;
}

void printUsageDefaultsOfStandardArguments(std::ostream& ostr)
{
	ostr << "   -h localhost " << std::endl;
	ostr << "   -u emdf " << std::endl;
	ostr << "   -b " << backend_kind2string(DEFAULT_BACKEND_ENUM) << std::endl;
	if (getHasEncodingOption()) {
		ostr << "   -e UTF8 " << std::endl;
	}
}

