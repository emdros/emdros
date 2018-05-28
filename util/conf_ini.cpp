/*
 * conf_ini.cpp
 *
 * A class to parse and hold an INI-file-like configuration file.
 *
 * Ulrik Sandborg-Petersen
 * Created: 4/9-2005
 * Last update: 11/4-2017
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2005-2017  Ulrik Sandborg-Petersen
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

#include <cstring>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <memory>

#include "conf_ini.h"
#include "string_func.h"
#include "pcre2_emdros.h"

#define MAX_LINE (1024)

ConfigurationINI::ConfigurationINI(std::istream *cf)
{
	// Any keys before the first section go in the section "".
	m_cur_section = "";
	
	int error;
	PCRE2_SIZE erroffset;

	// Section pattern
	#define CONF_INI_SECTION_PATTERN  "\\[([^\\]]+)\\]"
	pcre2_code *section_pattern =
		pcre2_compile((PCRE2_SPTR) CONF_INI_SECTION_PATTERN,  /* the pattern */
			      sizeof(CONF_INI_SECTION_PATTERN)-1,
			      0,                /* default options */
			      &error,           /* for error message */
			      &erroffset,       /* for error offset */
			      NULL);            /* use default character tables */
		
	pcre2_match_data *section_match_data = pcre2_match_data_create_from_pattern(section_pattern, NULL);

	// Assignment pattern
#define CONF_INI_ASSIGNMENT_PATTERN "([^ \\t\\n=]*)\\s*=\\s*([^\\r\\n]+)"
	pcre2_code *assignment_pattern =
		pcre2_compile((PCRE2_SPTR) CONF_INI_ASSIGNMENT_PATTERN,  /* the pattern */
			      sizeof(CONF_INI_ASSIGNMENT_PATTERN)-1,
			      0,                /* default options */
			      &error,           /* for error message */
			      &erroffset,       /* for error offset */
			      NULL);            /* use default character tables */

	pcre2_match_data *assignment_match_data = pcre2_match_data_create_from_pattern(assignment_pattern, NULL);
	
	// Comment pattern
#define CONF_INI_COMMENT_PATTERN "([^#]*)#.*"
	pcre2_code *comment_pattern = 
		pcre2_compile((PCRE2_SPTR) CONF_INI_COMMENT_PATTERN,  /* the pattern */
			      sizeof(CONF_INI_COMMENT_PATTERN)-1,
			      0,               /* default options */
			      &error,           /* for error message */
			      &erroffset,       /* for error offset */
			      NULL);            /* use default character tables */
	pcre2_match_data *comment_match_data = pcre2_match_data_create_from_pattern(comment_pattern, NULL);

	while (!cf->eof()) {
		char szLine[MAX_LINE];
		cf->getline(szLine, MAX_LINE);
		int rc = pcre2_match(comment_pattern,  
				     (PCRE2_SPTR) szLine,  /* the subject string */
				     (PCRE2_SIZE) strlen(szLine),  /* the length of the subject string */
				     0,       /* start at offset 0 in the subject */
				     0,       /* default options */
				     comment_match_data,
				     NULL);   /* use default match context. */
		// Remove comment
		char szLineWOutComment[MAX_LINE];
		if (rc >= 0) {
			PCRE2_SIZE nLineLength = MAX_LINE;
			pcre2_substring_copy_bynumber(comment_match_data,
						       1, /* String number. */
						       (PCRE2_UCHAR*) szLineWOutComment,
						       &nLineLength);
		} else {
			strcpy(szLineWOutComment, szLine);
		}
		
		rc = pcre2_match(section_pattern,  
				 (PCRE2_SPTR) szLineWOutComment,  /* the subject string */
				 strlen(szLineWOutComment),  /* the length of the subject string */
				 0,       /* start at offset 0 in the subject */
				 0,       /* default options */
				 section_match_data,
				 NULL);   /* use default match context. */
		
		if (rc >= 0) {
			PCRE2_UCHAR szSection[MAX_LINE];
			PCRE2_SIZE nSectionLength = MAX_LINE;
			
			pcre2_substring_copy_bynumber(section_match_data,
						      1, /* String number. */
						      (PCRE2_UCHAR*) szSection,
						      &nSectionLength);
			m_cur_section = std::string((const char*) szSection);
		} else {
			rc = pcre2_match(assignment_pattern,  
					 (PCRE2_SPTR) szLineWOutComment,  /* the subject string */
					 strlen(szLineWOutComment),  /* the length of the subject string */
					 0,       /* start at offset 0 in the subject */
					 0,       /* default options */
					 assignment_match_data,
					 NULL);   /* use default match context. */
		
			if (rc >= 0) {
				char szKey[MAX_LINE];
				PCRE2_SIZE nKeyLength = MAX_LINE;
				char szValue[MAX_LINE];
				PCRE2_SIZE nValueLength = MAX_LINE;
				pcre2_substring_copy_bynumber(assignment_match_data,
							      1, /* String number. */
							      (PCRE2_UCHAR*) szKey,
							      &nKeyLength);
				pcre2_substring_copy_bynumber(assignment_match_data,
							      2, /* String number. */
							      (PCRE2_UCHAR*) szValue,
							      &nValueLength);
				

				std::string sValue = szValue;
				sValue.erase(sValue.find_last_not_of(" \n\r\t")+1); // Remove trailing whitespace

				addKeyValuePair(std::string(szKey), sValue);
			}
		}
	}
	pcre2_code_free(section_pattern);
	pcre2_code_free(assignment_pattern);
	pcre2_code_free(comment_pattern);
	pcre2_match_data_free(section_match_data);
	pcre2_match_data_free(assignment_match_data);
	pcre2_match_data_free(comment_match_data);
}

#undef MAX_LINE

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

