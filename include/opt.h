/*
 * opt.h
 *
 * Classes and functions to support getting arguments from argc and argv
 *
 * Ulrik Sandborg-Petersen
 * Created: 7/19-2006
 * Last update: 5/28-2018
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

#ifndef OPTIONS__H__
#define OPTIONS__H__

#include <string>
#include <map>
#include <list>
#include <iostream>
#include <ostream>
#include "emdf_enums.h"

#include "emdros-lconfig.h"

typedef struct option_t {
	std::string short_version; /**< For options with one -. */
	std::string long_version; /**< For options with two -s. */
	std::string help_string_when_no_param;    /**< string to print when needs a parameter and not given a parameter. */
	bool bTakesParameter; /**< true iff this takes a parameter. */
	std::string value;
	std::string default_value;
	bool bPresent;
	option_t(const char *sv, /* without the - .*/
		 const char *lv, /* without the -- . */
		 bool tp=false, 
		 const char *dv="", /* default value. */
		 const char *hs="") 
	{ 
		short_version = std::string("-") + sv; 
		long_version= std::string("--") + lv; 
		help_string_when_no_param=hs; 
		bTakesParameter=tp; 
		default_value = dv,
		value=""; 
		bPresent=false;
	};
	// Use default copy-constructor: All strings must be constant.
} option_t;

typedef std::map<std::string, option_t> OptionMap_t;

extern void addOption(const char *short_version, /* with or without the - . */
		      const char *long_version,  /* with or without the -- . */
		      bool takes_parameter = false,
		      const char *default_value = "",
		      const char *help_message_when_no_param = "");
		      

extern bool parseArguments(int argc, char *argv[], std::string& error_message, std::list<std::string>& surplus_arguments);

extern bool getArgumentValue(const char *short_version, std::string& value);

extern bool getArgumentPresent(const char *short_version);

/* Must be declared in the program. */
extern OptionMap_t theOptionMap; 

/* Adds -h, -u, -p, -b, --version, --help, 
 * and -e if bAddEncoding is true. */
extern void addStandardArguments(bool bAddEncoding = false);

extern bool getStandardArguments(bool& bShowVersion, bool& bShowHelp, std::string& hostname, std::string& user, std::string& password, eBackendKind& backend_kind, eCharsets& charset, std::string& error_message);

extern std::string getBackendsAvailable();

extern void printBackendsAvailable(std::ostream& ostr);

extern void printUsageStandardArguments(std::ostream& ostr);

extern void printUsageDefaultsOfStandardArguments(std::ostream& ostr);

#endif // OPTIONS__H__
