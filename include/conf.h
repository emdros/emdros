/*
 * conf.h
 *
 * A class to parse and hold a configuration file.
 *
 * Ulrik Petersen
 * Created: 4/9-2005
 * Last update: 1/27-2007
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


#ifndef CONFIGURATION__H__
#define CONFIGURATION__H__

#include "pcre_emdros.h"
#include <string>
#include <map>
#include <set>
#include <vector>
#include "emdros-lconfig.h"
#if HAVE_ISTREAM
#include <istream>
#else
#include <istream.h>
#endif
#if HAVE_OSTREAM
#include <ostream>
#else
#include <ostream.h>
#endif


typedef std::vector<std::string> StringVec;
typedef std::map<std::string, StringVec> StringStringVecMap;


class Configuration {
private:
  StringStringVecMap m_conf;
public:
  Configuration(std::istream *cf);
  ~Configuration() {};
  void addKeyValuePair(const std::string& key, const std::string& value);
  bool has_key(const std::string& key);
  bool has_subkey(const std::string& key, const std::string& subkey);
  bool has_subsubkey(const std::string& key, const std::string& subkey, const std::string& subsubkey);
  bool has(const std::string& key, const std::string& value);
  std::string toString(void) const;
  StringVec getValues(const std::string& key);
  StringVec getSubkeyValues(const std::string& key, const std::string& subkey);
  StringVec getSubSubkeyValues(const std::string& key, const std::string& subkey, const std::string& subsubkey);
  bool check(const StringVec& toCheck, std::ostream *pErrorOut);
private:
  std::string do_constants(const std::string& token);
  std::string remove_slashr(const std::string& token);
  std::string remove_quote(const std::string& token);
  std::string remove_char(const std::string& token, char c);
};

extern Configuration *parse_config_file(const std::string& filename, const std::string& prefix, std::ostream *pOut);

// Returns true on pConf OK; false otherwise.
// If false, then error_msg is defined, and must contain
// an error message in UTF-8.
// Otherwise, error_msg is undefined.
//
// MUST BE IMPLEMENTED IN APP!
extern bool app_checkConfiguration(Configuration *pConf, std::string& error_msg);

typedef bool(*app_checkConfigurationFunc_t)(Configuration *pConf, std::string& error_msg);


#endif // CONFIGURATION__H__
