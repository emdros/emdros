/*
 * json_classes.cpp
 *
 * JSON classes
 *
 * Ulrik Sandborg-Petersen
 * Created: 7/28-2008
 * Last update: 6/22-2017
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2008-2017  Ulrik Sandborg-Petersen
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
#include <sstream>

#include "json_classes.h"
#include "string_func.h"
#include "jsonpars.h"

//////////////////////////////////////////////////////////////////
//
// JSONKeyValuePair
//
//////////////////////////////////////////////////////////////////

JSONKeyValuePair::JSONKeyValuePair(std::string *pKey, JSONValue *pValue, JSONKeyValuePair *pNext)
{
	m_pKey = pKey;
	m_pValue = pValue;
	m_pNext = pNext;
}


JSONKeyValuePair::~JSONKeyValuePair()
{
	delete m_pKey;
	delete m_pValue;
	delete m_pNext;
}






//////////////////////////////////////////////////////////////////
//
// JSONKeyValuePair
//
//////////////////////////////////////////////////////////////////

JSONListElement::JSONListElement(JSONValue *pValue)
{
	m_pValue = pValue;
	m_pNext = 0;
}


JSONListElement::~JSONListElement()
{
	delete m_pValue;
	delete m_pNext;
}

void JSONListElement::destroyList(JSONListElement *pList)
{
	JSONListElement *pCur = pList;
	while (pCur != 0) {
		JSONListElement *pNext = pCur->m_pNext;
		pCur->m_pNext = 0;
		delete pCur;
		pCur = pNext;
	}
}






//////////////////////////////////////////////////////////////////
//
// JSONValue
//
//////////////////////////////////////////////////////////////////

JSONValue::JSONValue()
{
	m_kind = kJSONNull;
}


JSONValue::JSONValue(bool bValue)
{
	m_kind = kJSONBoolean;
	m_value.m_boolean = bValue;
}

JSONValue::JSONValue(long long integer)
{
	m_kind = kJSONInteger;
	m_value.m_integer = integer;
}

JSONValue::JSONValue(std::string *pString)
{
	m_kind = kJSONString;
	m_value.m_pString = pString;
}


JSONValue::JSONValue(JSONListElement *pTail)
{
	m_kind = kJSONList;
	m_value.m_pList = new std::list<JSONValue*>;
	JSONListElement *pCur = pTail;
	while (pCur) {
		JSONValue *pCurValue = pCur->extractValue();
		m_value.m_pList->push_front(pCurValue);
		pCur = pCur->getNext();
	}
	JSONListElement::destroyList(pTail);
}


JSONValue::JSONValue(JSONKeyValuePair *pTail)
{
	m_kind = kJSONObject;
	m_value.m_pObject = new std::map<std::string, JSONValue*>;
	if (pTail == 0) {

	} else {
		JSONKeyValuePair *pCur = pTail;
		while (pCur) {
			JSONValue *pCurValue = pCur->extractValue();
			m_value.m_pObject->insert(std::make_pair(pCur->getKey(), pCurValue));
			pCur = pCur->getNext();
		}
		delete pTail;
	}
}

JSONValue::JSONValue(eJSONValueKind kind)
{
	m_kind = kind;
	switch (m_kind) {
	case kJSONNull:
		// Nothing to do
		break;
	case kJSONBoolean:
		m_value.m_boolean = false;
		break;
	case kJSONInteger:
		m_value.m_integer = 0L;
		break;
	case kJSONString:
		m_value.m_pString = new std::string();
		break;
	case kJSONList:
		m_value.m_pList = new std::list<JSONValue*>;
		break;
	case kJSONObject:
		m_value.m_pObject = new std::map<std::string, JSONValue *>;
		break;
	}
}

JSONValue::JSONValue(const JSONValue& other)
{
	copyOther(other);
}

JSONValue& JSONValue::operator=(const JSONValue& other)
{
	copyOther(other);
	return *this;
}

void JSONValue::copyOther(const JSONValue& other)
{
	m_kind = other.m_kind;
	switch (m_kind) {
	case kJSONNull:
		break;
		
	case kJSONBoolean:
		m_value.m_boolean = other.m_value.m_boolean;
		break;

	case kJSONInteger:
		m_value.m_integer = other.m_value.m_integer;
		break;
	case kJSONString:
		m_value.m_pString = new std::string(*(other.m_value.m_pString));
		break;
	case kJSONList:
		{
			m_value.m_pList = new std::list<JSONValue*>();
		
			std::list<JSONValue*>::const_iterator ci = other.m_value.m_pList->begin();
			std::list<JSONValue*>::const_iterator cend = other.m_value.m_pList->end();
			while (ci != cend) {
				m_value.m_pList->push_back(new JSONValue(*ci));
				++ci;
			}
		}
		break;
	case kJSONObject:
		{
			m_value.m_pObject = new std::map<std::string, JSONValue*>();
		
			std::map<std::string, JSONValue*>::const_iterator ci = other.m_value.m_pObject->begin();
			std::map<std::string, JSONValue*>::const_iterator cend = other.m_value.m_pObject->end();
			while (ci != cend) {
				m_value.m_pObject->insert(std::make_pair(ci->first, new JSONValue(*(ci->second))));
				++ci;
			}
		}
		break;
	}
}


 // object
JSONValue::~JSONValue()
{
	switch (m_kind) {
	case kJSONNull:
	case kJSONBoolean:
	case kJSONInteger:
		// Nothing to do
		break;
	case kJSONString:
		delete m_value.m_pString;
		break;
	case kJSONList:
		deleteList();
		break;
	case kJSONObject:
		deleteObject();
		break;
	}
}

void JSONValue::deleteList(void)
{
	if (m_kind == kJSONList) {
		std::list<JSONValue*>::iterator it = m_value.m_pList->begin();
		while (it != m_value.m_pList->end()) {
			delete *it;
			++it;
		}
		m_value.m_pList->clear();
	}
	delete m_value.m_pList;
	m_kind = kJSONNull;
}

void JSONValue::deleteObject(void)
{
	if (m_kind == kJSONObject) {
		std::map<std::string, JSONValue*>::iterator it = m_value.m_pObject->begin();
		while (it != m_value.m_pObject->end()) {
			delete it->second;
			++it;
		}
		m_value.m_pObject->clear();
	}
	delete m_value.m_pObject;
	m_kind = kJSONNull;
}



std::string JSONValue::getString(void) const
{
	if (m_kind == kJSONString) {
		return *(m_value.m_pString);
	} else {
		return "";
	}
}


bool JSONValue::getBoolean(void) const
{
	if (m_kind == kJSONBoolean) {
		return m_value.m_boolean;
	} else {
		return false;
	}
}


long long JSONValue::getInteger(void) const
{
	if (m_kind == kJSONInteger) {
		return m_value.m_integer;
	} else {
		return 0;
	}
}


const std::list<JSONValue*>& JSONValue::getList(void) const
{
	static const std::list<JSONValue*> emptyList;

	if (m_kind == kJSONList) {
		return *m_value.m_pList;
	} else {
		return emptyList;
	}
}

void JSONValue::append(JSONValue* pValue)
{
	if (m_kind == kJSONList) {
		m_value.m_pList->push_back(pValue);
	} else {
		// Not a list, can't append!
	}
}

void JSONValue::addKeyValue(const std::string& key, JSONValue *pValue)
{
	if (m_kind == kJSONObject) {
		std::map<std::string, JSONValue*>::iterator 
			it = m_value.m_pObject->find(key);
		if (it != m_value.m_pObject->end()) {
			// Don't add; key exists already
		} else {
			m_value.m_pObject->insert(std::make_pair(key, pValue));
		}
	} else {
		// Not an object, can't add
	}
}


std::list<std::string> JSONValue::getObjectKeys(void) const
{
	std::list<std::string> result;
	if (m_kind == kJSONObject) {
		std::map<std::string, JSONValue*>::const_iterator 
			ci = m_value.m_pObject->begin();
		while (ci != m_value.m_pObject->end()) {
			result.push_back(ci->first);
			++ci;
		}
		return result;
	} else {
		return result; // empty result
	}
}


const JSONValue* JSONValue::getObjectValue(const std::string& key) const
{
	if (m_kind == kJSONObject) {
		std::map<std::string, JSONValue*>::const_iterator 
			ci = m_value.m_pObject->find(key);
		if (ci != m_value.m_pObject->end()) {
			return ci->second;
		} else {
			return 0;
		}
	} else {
		return 0;
	}
	
}

JSONValue* JSONValue::getObjectValue(const std::string& key)
{
	if (m_kind == kJSONObject) {
		std::map<std::string, JSONValue*>::iterator 
			ci = m_value.m_pObject->find(key);
		if (ci != m_value.m_pObject->end()) {
			return ci->second;
		} else {
			return 0;
		}
	} else {
		return 0;
	}
}

void JSONValue::printCompact(std::ostream *pOut, bool bEscapeAsUnicode) const
{
	switch (m_kind) {
	case kJSONNull:
		(*pOut) << "null";
		break;
	case kJSONBoolean:
		if (getBoolean()) {
			(*pOut) << "true";
		} else {
			(*pOut) << "false";
		}
		break;
	case kJSONInteger:
		(*pOut) << longlong2string(getInteger());
		break;
	case kJSONString:
		(*pOut) << '\"' << escapeJSONChars(getString(), bEscapeAsUnicode) << '\"';
		break;
	case kJSONList: {
		(*pOut) << '[';
		std::list<JSONValue*>::const_iterator ci = m_value.m_pList->begin();
		if (ci != m_value.m_pList->end()) {
			(*ci)->printCompact(pOut, bEscapeAsUnicode);
			++ci;
		}
		while (ci != m_value.m_pList->end()) {
			(*pOut) << ',';
			(*ci)->printCompact(pOut, bEscapeAsUnicode);
			++ci;
		}
		(*pOut) << ']';
			}
		break;
	case kJSONObject: {
		(*pOut) << '{';
		std::map<std::string, JSONValue*>::const_iterator ci;
		ci = m_value.m_pObject->begin();
		if (ci != m_value.m_pObject->end()) {
			(*pOut) << '\"' << escapeJSONChars(ci->first, bEscapeAsUnicode) << "\":";
			ci->second->printCompact(pOut, bEscapeAsUnicode);
			++ci;
		}
		while (ci != m_value.m_pObject->end()) {
			(*pOut) << ',';
			(*pOut) << '\"' << escapeJSONChars(ci->first, bEscapeAsUnicode) << "\":";
			ci->second->printCompact(pOut, bEscapeAsUnicode);
			++ci;
		}
		(*pOut) << '}';
	}
		break;
	}
}

bool JSONValue::castToBool() const
{
	return !(this->isFalse());
}

long long JSONValue::castToInteger() const
{
	switch (m_kind) {
	case kJSONNull:
		return 0;
		break;
	case kJSONBoolean:
		if (m_value.m_boolean) {
			return 1L;
		} else {
			return 0L;
		}
		break;
	case kJSONInteger:
		return m_value.m_integer;
		break;
	case kJSONString:
		return string2longlong(*m_value.m_pString);
		break;
	case kJSONList:
		return m_value.m_pList->size();
		break;
	case kJSONObject:
		return m_value.m_pObject->size();
		break;
	}
	return 0;
}

std::string JSONValue::castToString() const
{
	std::stringstream sout;
	bool bEscapeAsUnicode = false;
	this->pretty(&sout, 0, bEscapeAsUnicode);
	return sout.str();
}


bool JSONValue::isFalse() const
{
	switch (m_kind) {
	case kJSONNull:
		return true;
		break;
	case kJSONBoolean:
		return !(m_value.m_boolean);
		break;
	case kJSONInteger:
		return (m_value.m_integer == 0L);
		break;
	case kJSONString:
		return m_value.m_pString->empty();
		break;
	case kJSONList:
		return m_value.m_pList->empty();
		break;
	case kJSONObject: 
		return m_value.m_pObject->empty();
		break;
	}
	return true; // Fool some compilers into giving no warning
}

void JSONValue::pretty(std::ostream *pOut, int indent_level, bool bEscapeAsUnicode) const
{
	switch (m_kind) {
	case kJSONNull:
		(*pOut) << "null";
		break;
	case kJSONBoolean:
		if (getBoolean()) {
			(*pOut) << "true";
		} else {
			(*pOut) << "false";
		}
		break;
	case kJSONInteger:
		(*pOut) << longlong2string(getInteger());
		break;
	case kJSONString:
		(*pOut) << '\"' << escapeJSONChars(getString(), bEscapeAsUnicode) << '\"';
		break;
	case kJSONList: {
		(*pOut) << "[\n";
		std::list<JSONValue*>::const_iterator ci = m_value.m_pList->begin();
		if (ci != m_value.m_pList->end()) {
			indent(pOut, indent_level+1);
			(*ci)->pretty(pOut, indent_level+1, bEscapeAsUnicode);
			++ci;
		}
		while (ci != m_value.m_pList->end()) {
			(*pOut) << ",\n";
			indent(pOut, indent_level+1);
			(*ci)->pretty(pOut, indent_level+1, bEscapeAsUnicode);
			++ci;
		}
		(*pOut) << '\n';
		indent(pOut, indent_level);
		(*pOut) << "]";
			}
		break;
	case kJSONObject: {
		(*pOut) << "{\n";
		std::map<std::string, JSONValue*>::const_iterator ci;
		ci = m_value.m_pObject->begin();
		if (ci != m_value.m_pObject->end()) {
			indent(pOut, indent_level+1);
			(*pOut) << '\"' << escapeJSONChars(ci->first, bEscapeAsUnicode) << "\" : ";
			ci->second->pretty(pOut, indent_level+1, bEscapeAsUnicode);
			++ci;
		}
		while (ci != m_value.m_pObject->end()) {
			(*pOut) << ",\n";
			indent(pOut, indent_level+1);
			(*pOut) << '\"' << escapeJSONChars(ci->first, bEscapeAsUnicode) << "\" : ";
			
			ci->second->pretty(pOut, indent_level+2, bEscapeAsUnicode);
			++ci;
		}
		(*pOut) << '\n';
		indent(pOut, indent_level);
		(*pOut) << "}";
	}
		break;
	}
	if (indent_level == 0) {
		(*pOut) << '\n';
	}
}

void JSONValue::indent(std::ostream *pOut, int indent_level) const
{
	for (int i = 0; i < indent_level; ++i) {
		(*pOut) << "   ";
	}
}


bool JSONValue::hasObjectKey(const std::string& key) const
{
	if (m_kind == kJSONObject) {
		std::map<std::string, JSONValue*>::const_iterator 
			ci = m_value.m_pObject->find(key);
		if (ci != m_value.m_pObject->end()) {
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}





//////////////////////////////////////////////////////////////////
//
// JSONExecEnv
//
//////////////////////////////////////////////////////////////////




JSONExecEnv::JSONExecEnv()
{
	m_pError = new std::string;
	bSyntaxError = false;
	m_pTopValue = 0;
	pLexer = 0;
}


JSONExecEnv::~JSONExecEnv()
{
	delete m_pError;
	delete m_pTopValue;
	delete pLexer;
}

void JSONExecEnv::clean()
{
	delete pLexer;
	pLexer = 0;
}

void JSONExecEnv::setTopValue(JSONValue *pValue)
{
	delete m_pTopValue;
	m_pTopValue = pValue;
}

JSONValue *JSONExecEnv::extractTopValue(void)
{
	JSONValue *pResult = m_pTopValue;
	m_pTopValue = 0;
	return pResult;
}

std::string JSONExecEnv::getInputScannedSoFar()
{
	if (pLexer == 0) {
		return "";
	} else {
		return pLexer->getInputScannedSoFar();
	}
}

void JSONExecEnv::clearError(void)
{
	*m_pError = "";
}


std::string JSONExecEnv::getError()
{
	return *m_pError;
}

void JSONExecEnv::prependError(const std::string& addition)
{
	*m_pError = addition + "\n" + *m_pError;
}

void JSONExecEnv::addToError(const std::string& addition)
{

	*m_pError += addition + "\n";
}

Token *JSONnewToken()
{
	Token *pToken = newToken();
	//pToken->init();
	return pToken;
}

int jjlex(void *lvalp, void *parm)
{
	return ((JSONExecEnv*) parm)->pLexer->scan((Token**) lvalp);
}


// Prototypes
extern void *JSONParserAlloc(void *(*mallocProc)(size_t));
extern void JSONParserFree(void *p, void (*freeProc)(void*));
extern void JSONParser(
		      void *yyp,                       /* The parser */
		      int yymajor,                     /* The major token code number */
		      Token *yyminor,                  /* The value for the token */
		      JSONExecEnv *pArg /* optional argument */
		      );


//extern void JSONParserTrace(FILE *TraceFILE, char *zTracePrompt);

int jjparse(JSONExecEnv *pEE)
{
	void *pParser;
	Token *pToken;
	int hTokenId;

	pEE->bSyntaxError = false;
	pParser = JSONParserAlloc( malloc );
	//	JSONParserTrace(stderr, "JSONParser: ");
	//pToken = newToken();
	pToken = 0;
	while((hTokenId = jjlex(&pToken, pEE))
	      && !pEE->bSyntaxError) {
		JSONParser(pParser, hTokenId, pToken, pEE);
		pToken = 0;
		//pToken = newToken();
	}
	JSONParser(pParser, 0, pToken, pEE);
	if (!pEE->pLexer->isLocallyAllocatedToken(pToken)) {
		// pToken->clear();
		delete pToken;
	}
//delete pToken;

	JSONParserFree(pParser, free );


	if (pEE->bSyntaxError) {
		return 1;
	} else {
		return 0;
	}
}




JSONValue *parseJSON(JSONExecEnv* pEE, const std::string& json_string, bool& bResult)
{
	pEE->clean();

	pEE->pLexer = new JSONScanner(json_string);

	// Clear error
	pEE->clearError();

	// Declare return_value
	JSONValue *pJSONResult = 0;
  
	// Initialize bResult
	bResult = true;

	// Parser
	int iResult = jjparse(pEE);

	// Did parsing fail?
	if (iResult == 1) {
		bResult = false;
		pEE->prependError("Parsing failed\n");
	} else if (iResult == 2) {
		bResult = false;
		pEE->prependError("Parsing stack overflow. Parsing failed.\n");
	}

	// Was the parsing a success?
	if (bResult) {
		// If so, extract top value
		pJSONResult = pEE->extractTopValue();
	} else {
		pJSONResult = 0;
	}

	// Return
	return pJSONResult;
}



/** Reads and parses some JSON from a string.
 *
 * @param json_string The JSON to parse.
 *
 * @param error_message An error message indicating what went wrong,
 * if anything.
 *
 * @return 0 (nil) upon failure, otherwise, a JSONValue parsed from
 * the contents of the string.
 */
JSONValue *readAndParseJSONFromString(const std::string& json_string, std::string& error_message)
{
	JSONExecEnv *pJSONEE = new JSONExecEnv();
	bool bResult = false;
	JSONValue *pJSONValue = parseJSON(pJSONEE, json_string, bResult);
	if (!bResult) {
		error_message += "FAILURE while parsing JSON contents.  Here is any error message:\n"
			  "----------------------------------------------------------------------\n"
			  + pJSONEE->getError() 
			  + "\n----------------------------------------------------------------------\n";
		delete pJSONEE;
		return 0;
	} else {
		delete pJSONEE;
		return pJSONValue;
	}
}




/** Reads and parses some JSON from a file with the given filename
 *
 * @param filename The name of the file to parse.
 *
 * @param error_message An error message indicating what went wrong,
 * if anything.
 *
 * @return 0 (nil) upon failure, otherwise, a JSONValue parsed from
 * the contents of the file.
 */
JSONValue *readAndParseJSONFromFile(const std::string& filename, std::string& error_message)
{
	std::ifstream fin;
	fin.open(filename.c_str());
	if (!fin) {
		error_message += "FAILURE: Could not open file " + filename + " for reading.\nAction aborted.\n";

		return 0;
	}
	std::string strResult;
	std::string line;
	while (!fin.eof() && !fin.fail() && !fin.bad()) {
		std::getline(fin, line);
		strResult += line;
		strResult += '\n';
	}
	
	if (!fin.eof()) {
		error_message += "FAILURE: Could not read contents of file " + filename + ".\n"
			+ "Action aborted.\n";

		return 0;
	}

	return readAndParseJSONFromString(strResult, error_message);
}


