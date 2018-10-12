/*
 * conf_ini.h
 *
 * A class to parse and hold an INI-file-like configuration file.
 *
 * Ulrik Sandborg-Petersen and Claus Tøndering
 * Created: 4/9-2005
 * Last update: 4/10-2017
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


#ifndef CONFIGURATION2__H__
#define CONFIGURATION2__H__

#include <conf.h>
#include <memory>

typedef std::map<std::string, std::string> StringMap;
typedef std::map<std::string, StringMap> StringStringMapMap;


class ConfigurationINI {
 private:
	StringStringMapMap m_conf;
	std::string m_cur_section;
	void addKeyValuePair(const std::string& key, const std::string& value);
 public:
	ConfigurationINI(std::istream *cf);
	~ConfigurationINI() {}
	bool hasSection(const std::string& section) const;
	bool hasKeyInSection(const std::string& section, const std::string& key) const;
	bool hasValueInSection(const std::string& section, const std::string& key, const std::string& value) const;
	bool getValue(const std::string& section, const std::string& key, std::string& value) const;
	StringVec getSections() const;
	StringVec getKeysInSection(const std::string& section) const;
	std::string toString() const;
};

extern ConfigurationINI *parse_configINI_file(const std::string& filename, std::ostream *pOut);



#endif // CONFIGURATION2__H__
