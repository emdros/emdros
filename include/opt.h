/*
 * opt.h
 *
 * Classes and functions to support getting arguments from argc and argv
 *
 * Ulrik Sandborg-Petersen
 * Created: 7/19-2006
 * Last update: 1/16-2019
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


#ifndef OPTIONS__H__
#define OPTIONS__H__

#include <string>
#include <map>
#include <list>
#include "emdf_enums.h"

#include "emdros-lconfig.h"

#include <ostream>
#include <iostream>

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
