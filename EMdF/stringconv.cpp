/*
 * strinconv.cpp
 *
 * Classes and functions for conversion of strings to other strings by
 * means of a trie.
 *
 * Ulrik Sandborg-Petersen
 * Created: 1/28-2011
 * Last update: 1/28-2011
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2011  Ulrik Sandborg-Petersen
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

#include <fstream>
#include <json_classes.h>
#include <string_func.h>
#include <debug.h>

#include <map>

class StringConvTrie {
	JSONValue *m_pRoot;
protected:
	StringConvTrie();
	StringConvTrie();
	~StringConvTrie();


	bool loadStylesheet(const std::string& JSON_stylesheet, const std::string& stylesheet_name, std::string& error_message);
	void add(const std::string& key, const std::string value);
	std::string find(const std::string& key, bool& bFound);
	std::string findp(const std::string& key, bool& bFound, std::string& remainder);
	std::string convert(const std::string& keystring, bool& bFound, std::string& remainder);
protected:
	JSONValue *getEmptyNode();
};


StringConvTrie::StringConvTrie()
{
	m_pRoot = getEmptyNode();
}

JSONValue *StringConvTrie::getEmptyNode()
{
	std::string error_message;
	JSONValue *pEmptyNode = readAndParseJSONFromString("[null, {}]", error_message);
	ASSERT_THROW(pEmptyNode != 0,
		     "Could not parse [null, {}] as JSON.");
	return pEmptyNode;
}


StringConvTrie::~StringConvTrie()
{
	delete m_pRoot;
}

bool StringConvTrie::loadStylesheet(const JSONValue *pStringConversion, const std::string& stylesheet_name, std::string& error_message)
{
XXX	
}


bool StringConvTrie::loadStylesheet(const std::string& JSON_stylesheet, const std::string& stylesheet_name, std::string& error_message)
{
	const JSONValue *pStringConversion = readAndParseJSONFromString(JSON_stylesheet, error_message);
	if (pStringConversion == 0) {
		error_message = "Could not parse JSON stylesheet. Error message from JSON parser:\n" + error_message;
		return false;
	} else {
		bool bResult = loadStyleSheet(pStringConversion, stylesheet_name, error_message);
		delete pStringConversion;
		return bResult;
	}
}


void StringConvTrie::add(const std::string& key, const std::string value)
{
	JSONValue *pCurrentNode = m_pRoot;
	for (std::string::const_iterator ci = key.begin();
	     ci != key.end();
	     ++ci) {
		char c = *ci;
		std::string ch(1, c);

		JSONValue *pDict = pCurrentNode->getListElement(1);
		ASSERT_THROW(pDict->getKind() == kJSONObject,
			     "pDict isn't a kJSONObject");
		if (!pDict->hasObjectKey(ch)) {
			pDict->addKeyValue(ch, getEmptyNode());
		}
		pCurrentNode = pDict->getObjectValue(ch);
	}
	pCurrentNode->setListElement(new JSONValue(new std::string(value)), 0);
}


std::string StringConvTrie::find(const std::string& key, bool& bFound)
{
	bFound = false;
	JSONValue *pCurrentNode = m_pRoot;
	for (std::string::const_iterator ci = key.begin();
	     ci != key.end();
	     ++ci) {
		char c = *ci;
		std::string ch(1, c);

		JSONValue *pDict = pCurrentNode->getListElement(1);
		ASSERT_THROW(pDict->getKind() == kJSONObject,
			     "pDict isn't a kJSONObject");
		if (!pDict->hasObjectKey(ch)) {
			bFound = false;
			return "";
		}
		pCurrentNode = pDict->getObjectValue(ch);
	}
	ASSERT_THROW(pCurrentNode->getKind() == kJSONList,
		     "Current node isn't a JSONList");
	if (pCurrentNode->getListElement(0)->getKind() == kJSONString) {
		bFound = true;
		return pCurrentNode->getListElement(0)->getString();
	} else {
		ASSERT_THROW(pCurrentNode->getListElement(0)->getKind() == kJSONNull,
			     "Current node's list element 0 isn't JSON null.");
		bFound = false;
		return "";
	}
}


std::string StringConvTrie::findp(const std::string& key, bool& bFound, std::string& remainder)
{
	bFound = false;
	JSONValue *pCurrentNode = m_pRoot;
	remainder = key;
	for (std::string::const_iterator ci = key.begin();
	     ci != key.end();
	     ++ci) {
		char c = *ci;
		std::string ch(1, c);

		JSONValue *pDict = pCurrentNode->getListElement(1);
		ASSERT_THROW(pDict->getKind() == kJSONObject,
			     "pDict isn't a kJSONObject");
		if (!pDict->hasObjectKey(ch)) {
			bFound = false;

			ASSERT_THROW(pCurrentNode->getKind() == kJSONList,
				     "Current node isn't a JSONList");
			if (pCurrentNode->getListElement(0)->getKind() == kJSONString) {
				return pCurrentNode->getListElement(0)->getString();
			} else {
				ASSERT_THROW(pCurrentNode->getListElement(0)->getKind() == kJSONNull,
					     "Current node's list element 0 isn't JSON null.");
				return "";
			}
		}
		pCurrentNode = pDict->getObjectValue(ch);
		if (remainder.length() > 0) {
			remainder = remainder.substr(1, std::string::npos);
		}
	}
	ASSERT_THROW(pCurrentNode->getKind() == kJSONList,
		     "Current node isn't a JSONList");
	if (pCurrentNode->getListElement(0)->getKind() == kJSONString) {
		bFound = true;
		return pCurrentNode->getListElement(0)->getString();
	} else {
		ASSERT_THROW(pCurrentNode->getListElement(0)->getKind() == kJSONNull,
			     "Current node's list element 0 isn't JSON null.");
		bFound = false;
		return "";
	}
}


std::string StringConvTrie::convert(const std::string& keystring, bool& bFound, std::string& remainder)
{
	std::string valuestring;
	std::string key = keystring;
	bFound = true;
	while (bFound && !key.empty()) {
		std::string remnd;
		std::string value;
		value = findp(key, bFound, remnd);
		key = remnd;
		if (!bFound) {
			remainder = key;
			return valuestring;
		}
		valuestring += value;
	}
	remainder = key;
	return valuestring;
}


