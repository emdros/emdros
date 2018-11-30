/*
 * conf_ini.cpp
 *
 * A class to parse and hold an INI-file-like configuration file.
 *
 * Ulrik Petersen
 * Created: 4/9-2005
 * Last update: 11/30-2018
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


#include <conf_ini.h>
#include <cstring>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <memory>

#include <string_func.h>

#define MAX_LINE (1024)

ConfigurationINI::ConfigurationINI(std::istream *cf)
{
	// Any keys before the first section go in the section "".
	m_cur_section = "";
	
	const char *error;
	int erroffset;

	// Assignment pattern section
	pcre *assignmentpattern_section = 
		pcre_compile("\\[([^\\]]+)\\]",  /* the pattern */
			     0,                /* default options */
			     &error,           /* for error message */
			     &erroffset,       /* for error offset */
			     NULL);            /* use default character tables */
	pcre_extra *assignpat_section_extra = NULL;
	assignpat_section_extra = pcre_study(assignmentpattern_section, 0, &error);

	// Assignment pattern
	pcre *assignmentpattern = 
		pcre_compile("([^ \\t\\n=]*)\\s*=\\s*([^\\r\\n]+)",  /* the pattern */
			     0,                /* default options */
			     &error,           /* for error message */
			     &erroffset,       /* for error offset */
			     NULL);            /* use default character tables */
	pcre_extra *assignpat_extra = NULL;
	assignpat_extra = pcre_study(assignmentpattern, 0, &error);

	// Comment pattern
	pcre *commentpattern = 
		pcre_compile("([^#]*)#.*",  /* the pattern */
			     0,                /* default options */
			     &error,           /* for error message */
			     &erroffset,       /* for error offset */
			     NULL);            /* use default character tables */
	pcre_extra *commentpat_extra = NULL;
	commentpat_extra = pcre_study(commentpattern, 0, &error);
	while (!cf->eof()) {
		char szLine[MAX_LINE];
		cf->getline(szLine, MAX_LINE);
		int ovector[30];
		int rc = 
			pcre_exec(commentpattern,  
				  commentpat_extra,    
				  szLine,  /* the subject string */
				  (int) strlen(szLine),  /* the length of the subject string */
				  0,       /* start at offset 0 in the subject */
				  0,       /* default options */
				  ovector, /* vector of integers for substring information */
				  30);     /* number of elements in the vector (NOT size in bytes) */
		// Remove comment
		char szLineWOutComment[MAX_LINE];
		if (rc >= 0) {
			pcre_copy_substring(szLine, ovector, (rc == 0) ? 10 : rc, 
					    1, /* String number. */
					    szLineWOutComment, MAX_LINE);
		} else {
			strcpy(szLineWOutComment, szLine);
		}

		rc = pcre_exec(assignmentpattern_section,  
			       assignpat_section_extra,    
			       szLineWOutComment,  /* the subject string */
			       (int) strlen(szLineWOutComment),  /* the length of the subject string */
			       0,       /* start at offset 0 in the subject */
			       0,       /* default options */
			       ovector, /* vector of integers for substring information */
			       30);     /* number of elements in the vector (NOT size in bytes) */
    
		if (rc >= 0) {
			char szSection[MAX_LINE];
			pcre_copy_substring(szLineWOutComment, ovector, (rc == 0) ? 10 : rc, 
					    1, /* String number. */
					    szSection, MAX_LINE);
      
			m_cur_section = std::string(szSection);
		} else {
			rc = pcre_exec(assignmentpattern,  
				       assignpat_extra,    
				       szLineWOutComment,  /* the subject string */
				       (int) strlen(szLineWOutComment),  /* the length of the subject string */
				       0,       /* start at offset 0 in the subject */
				       0,       /* default options */
				       ovector, /* vector of integers for substring information */
				       30);     /* number of elements in the vector (NOT size in bytes) */
			
			if (rc >= 0) {
				char szKey[MAX_LINE];
				char szValue[MAX_LINE];
				pcre_copy_substring(szLineWOutComment, ovector, (rc == 0) ? 10 : rc, 
						    1, /* String number. */
						    szKey, MAX_LINE);
				pcre_copy_substring(szLineWOutComment, ovector, (rc == 0) ? 10 : rc, 
						    2, /* String number. */
						    szValue, MAX_LINE);
				
                std::string sValue = szValue;
                sValue.erase(sValue.find_last_not_of(" \n\r\t")+1); // Remove trailing whitespace

				addKeyValuePair(std::string(szKey), sValue);
			}
		}
	}
	pcre_free(assignmentpattern_section);
	pcre_free(assignpat_section_extra);
	pcre_free(assignmentpattern);
	pcre_free(assignpat_extra);
	pcre_free(commentpattern);
	pcre_free(commentpat_extra);
}

void ConfigurationINI::addKeyValuePair(const std::string& key, const std::string& value)
{
	if (m_conf.find(m_cur_section) == m_conf.end()) {
		m_conf[m_cur_section] = StringMap();
	}

	m_conf[m_cur_section][key] = value;
}

bool ConfigurationINI::hasSection(const std::string& section) const
{
	return m_conf.find(section) != m_conf.end();
}

bool ConfigurationINI::hasKeyInSection(const std::string& section, const std::string& key) const
{
	if (m_conf.find(section) == m_conf.end()) {
		return false;
	} else {
		return m_conf.find(section)->second.find(key) != m_conf.find(section)->second.end();
	}
}


bool ConfigurationINI::hasValueInSection(const std::string& section, const std::string& key, const std::string& value) const
{
	if (!hasSection(section)) {
		return false;
	}

	if (!hasKeyInSection(section, key)) {
		return false;
	}

	std::string myvalue = m_conf.find(section)->second.find(key)->second;
	return myvalue == value;
}


bool ConfigurationINI::getValue(const std::string& section, const std::string& key, std::string& value) const
{
	if (!hasSection(section)) {
		return false;
	}

	if (!hasKeyInSection(section, key)) {
		return false;
	}

	value = m_conf.find(section)->second.find(key)->second;
	return true;
}

StringVec ConfigurationINI::getSections() const
{
	StringVec ovec;
	StringStringMapMap::const_iterator ci = m_conf.begin();
	while (ci != m_conf.end()) {
		ovec.push_back(ci->first);
		++ci;
	}
	return ovec;
}

void sectionToStringVec(const ConfigurationINI *pConf, const std::string& section, StringVec& ovec)
{
	if (pConf->hasSection(section)) {
		if (section != "") {
			ovec.push_back("[" + section + "]");
		}

		StringVec keys = pConf->getKeysInSection(section);
		for (StringVec::size_type i = 0;
		     i < keys.size();
		     ++i) {
			std::string key = keys[i];

			std::string value;
			pConf->getValue(section, key, value);

			std::string result_line = key + " = " + value;
			ovec.push_back(result_line);
		}
	}
}


std::string ConfigurationINI::toString() const
{
	StringVec ovec;

	sectionToStringVec(this, "", ovec);
	
	StringVec sections = getSections();

	for (StringVec::size_type i = 0;
	     i < sections.size();
	     ++i) {
		std::string section = sections[i];
		if (section == "") {
			; // Already done above as the first one.
		} else {
			// Two empty lines precede each section
			ovec.push_back("");
			ovec.push_back("");

			// Now emit this section
			sectionToStringVec(this, section, ovec);
		}
	}

	return joinVector("\n", ovec) + "\n";
}

StringVec ConfigurationINI::getKeysInSection(const std::string& section) const
{
	if (!hasSection(section)) {
		StringVec empty_ovec;
		return empty_ovec;
	} else {
		StringVec ovec;
		StringStringMapMap::const_iterator section_ci = m_conf.find(section);
		StringMap::const_iterator ci = section_ci->second.begin();
		while (ci != section_ci->second.end()) {
			ovec.push_back(ci->first);
			++ci;
		}
		return ovec;
	}
}


///////////////////////////////////////////////
//
// Utility functions
//
///////////////////////////////////////////////

ConfigurationINI *parse_configINI_file(const std::string& filename, 
				       std::ostream *pOut)
{
	std::ifstream fin(filename.c_str());

	if (!fin) {
		if (pOut != 0) {
			(*pOut) << "FAILURE: Could not open file " << filename;
			(*pOut) << "\nfor reading. Action aborted.\n";
		}
		return 0;
	} else {
		return new ConfigurationINI(&fin);
	}
}

