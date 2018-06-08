/*
 * conf.h
 *
 * A class to parse and hold a configuration file.
 *
 * Ulrik Sandborg-Petersen
 * Created: 4/9-2005
 * Last update: 6/8-2018
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2005-2018  Ulrik Sandborg-Petersen
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

#ifndef CONFIGURATION__H__
#define CONFIGURATION__H__

#include <string>
#include <map>
#include <set>
#include <vector>
#include <istream>
#include <ostream>



#if defined(_WINDOWS)
extern "C++" {
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

Configuration *parse_config_file(const std::string& filename, const std::string& prefix, std::ostream *pOut);

// Returns true on pConf OK; false otherwise.
// If false, then error_msg is defined, and must contain
// an error message in UTF-8.
// Otherwise, error_msg is undefined.
//
// MUST BE IMPLEMENTED IN APP!
bool app_checkConfiguration(Configuration *pConf, std::string& error_msg);

typedef bool(*app_checkConfigurationFunc_t)(Configuration *pConf, std::string& error_msg);

#if defined(_WINDOWS)
} // extern "C++" 
#endif

	

#endif // CONFIGURATION__H__
