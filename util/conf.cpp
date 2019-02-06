/*
 * conf.cpp
 *
 * A class to parse and hold a configuration file.
 *
 * Ulrik Petersen
 * Created: 4/9-2005
 * Last update: 2/6-2019
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


#include <conf.h>
#include <cstring>
#include <fstream>
#include <algorithm>
#include <iostream>
//#include "myos.h"


#define MAX_LINE (1024)

Configuration::Configuration(std::istream *cf)
{
  const char *error;
  int erroffset;

  // Assignment pattern
  pcre *assignmentpattern = 
    pcre_compile("([^ \\t\\n=]*)\\s*=\\s*([0-9A-Za-z_\\.\\-]*(\"[^\"\\n]*\")?)",  /* the pattern */
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
      
      addKeyValuePair(std::string(szKey), std::string(szValue));
    }
  }
  pcre_free(assignmentpattern);
  pcre_free(assignpat_extra);
  pcre_free(commentpattern);
  pcre_free(commentpat_extra);
}

void Configuration::addKeyValuePair(const std::string& key, const std::string& value)
{
  if (m_conf.find(key) == m_conf.end()) {
    StringVec svec;
    svec.push_back(do_constants(remove_slashr(value)));
    m_conf[key] = svec;
  } else {
    m_conf[key].push_back(do_constants(remove_slashr(value)));
  }
}

bool Configuration::has_key(const std::string& key)
{
  return m_conf.find(key) != m_conf.end();
}

bool Configuration::has_subkey(const std::string& key, const std::string& subkey)
{
  StringVec strSubkeyVec = getSubkeyValues(key, subkey);
  return strSubkeyVec.size() != 0;
}

bool Configuration::has_subsubkey(const std::string& key, const std::string& subkey, const std::string& subsubkey)
{
  StringVec strSubSubkeyVec = getSubSubkeyValues(key, subkey, subsubkey);
  return strSubSubkeyVec.size() != 0;
}



bool Configuration::has(const std::string& key, const std::string& value)
{
  if (m_conf.find(key) != m_conf.end()) {
    return std::find(m_conf[key].begin(), m_conf[key].end(), value) != m_conf[key].end();
  } else {
    return false;
  }
}

std::string Configuration::toString(void) const
{
  std::string result = "{\n";
  for (StringStringVecMap::const_iterator mapci = m_conf.begin();
       mapci != m_conf.end();
       ++mapci) {
    result += "  " + mapci->first + " : ";
    for (StringVec::const_iterator setci = mapci->second.begin();
	 setci != mapci->second.end();
	 ++setci) {
      result += *setci + " ";
    }
    result += "\n";
  }
  result += "}\n";
  return result;
}

static void myreplace(std::string& token, std::string toreplace, std::string replacement)
{
  std::string::size_type found_pos;
  while((found_pos = token.find(toreplace)) != std::string::npos) {
    token.replace(found_pos, toreplace.length(), replacement);
  }
}

std::string Configuration::do_constants(const std::string& token)
{
  std::string result = token;
  myreplace(result, "COMMA_SPACE", ", ");
  myreplace(result, "COMMA", ",");
  myreplace(result, "COLON_SPACE", ": ");
  myreplace(result, "COLON", ":");
  myreplace(result, "SPACE", " ");
  myreplace(result, "NIL", "");
  myreplace(result, "OPEN_BRACKET", "[");
  myreplace(result, "CLOSE_BRACKET", "]");
  myreplace(result, "OPEN_BRACE", "{");
  myreplace(result, "CLOSE_BRACE", "}");
  myreplace(result, "OPEN_PAREN", "(");
  myreplace(result, "CLOSE_PAREN", ")");
  myreplace(result, "NEWLINE", "\n");
  return result;
}

std::string Configuration::remove_slashr(const std::string& token)
{
  return remove_char(token, '\r');
}

std::string Configuration::remove_quote(const std::string& token)
{
  return remove_char(token, '\"');
}

std::string Configuration::remove_char(const std::string& token, char c)
{
  if (token.find_first_of(std::string(1,c)) == std::string::npos) {
    return token;
  } else {
    std::string result;
    for (unsigned int i = 0; i < token.length(); ++i) {
      if (token[i] != c) {
	result += token[i];
      }
    }
    return result;
  }
}

// Assumes that key exists
StringVec Configuration::getValues(const std::string& key)
{
  if (m_conf.find(key) == m_conf.end()) {
    return StringVec();
  } else {
    StringVec result;
    for (StringVec::const_iterator ci = m_conf[key].begin();
	 ci != m_conf[key].end();
	 ++ci) {
      result.push_back(remove_quote(*ci));
    }
    return result;
  }
}

StringVec Configuration::getSubkeyValues(const std::string& key, const std::string& subkey)
{
  StringVec result;
  if (m_conf.find(key) != m_conf.end()) {
    std::string subkey_plus_dot = subkey;
    subkey_plus_dot += ".";
    for (StringVec::const_iterator ci = m_conf[key].begin();
	 ci != m_conf[key].end();
	 ++ci) {
      std::string subkey_plus_value = *ci;
      std::string subkey = subkey_plus_value.substr(0, subkey_plus_dot.length());
      if (subkey == subkey_plus_dot) {
	result.push_back(remove_quote(subkey_plus_value.substr(subkey_plus_dot.length())));
      }
    }
  }
  return result;
}


StringVec Configuration::getSubSubkeyValues(const std::string& key, const std::string& subkey, const std::string& subsubkey)
{
  StringVec result;
  if (m_conf.find(key) != m_conf.end()) {
    std::string subsubkey_plus_dot = subkey + "." + subsubkey + ".";
    for (StringVec::const_iterator ci = m_conf[key].begin();
	 ci != m_conf[key].end();
	 ++ci) {
      std::string subsubkey_plus_value = *ci;
      std::string subsubkey_value = subsubkey_plus_value.substr(0, subsubkey_plus_dot.length());
      if (subsubkey_value == subsubkey_plus_dot) {
	result.push_back(remove_quote(subsubkey_plus_value.substr(subsubkey_plus_dot.length())));
      }
    }
  }
  return result;
}


bool Configuration::check(const StringVec& toCheck, std::ostream *pErrorStream)
{
  bool bResult = true;
  for (StringVec::const_iterator ci = toCheck.begin();
       ci != toCheck.end();
       ++ci) {
    std::string key = *ci;
    if (!has_key(key)) {
      bResult = false;
      (*pErrorStream) << ("Error: missing key '" + key + "' in configuration file.");
    }
  }
  return bResult;
}

///////////////////////////////////////////////
//
// Utility functions
//
///////////////////////////////////////////////

Configuration *parse_config_file(const std::string& filename, 
				 const std::string& prefix, 
				 std::ostream *pOut)
{
  std::string strError;
  std::ifstream *pFin;
  pFin = new std::ifstream();
  pFin->open(filename.c_str());
  if (!(*pFin)) {
    delete pFin;
    std::string prefixed_filename = prefix + filename;
    pFin = new std::ifstream();
    pFin->open(prefixed_filename.c_str());
    if (!(*pFin)) {
      delete pFin;
      if (pOut != 0) {
	(*pOut) << "FAILURE: Could not open file " << prefixed_filename;
	(*pOut) << "\nfor reading. Action aborted.\n";
      }
      return 0;
    }
  }
  Configuration *pConf = new Configuration(pFin);
  pFin->close();
  delete pFin;

  return pConf;
}

