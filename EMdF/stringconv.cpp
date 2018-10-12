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

/*
 * Copyright (C) 2001-2018     Ulrik Sandborg-Petersen
 * Copyright (C) 2018-present  Sandborg-Petersen Holding ApS, Denmark
 *
 * Licensed under the MIT License.
 *
 * Please see the file COPYING in the root of the sources for more details.
 *
 */


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


