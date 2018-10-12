/*
 * json_classes.h
 *
 * JSON classes
 *
 * Ulrik Sandborg-Petersen
 * Created: 7/28-2008
 * Last update: 5/23-2017
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


#ifndef JSON_HEADER_H_
#define JSON_HEADER_H_

#include <string>
#include <map>
#include <list>
#include <ostream>
#include "json_lexer.h"

typedef enum {
	kJSONNull,
	kJSONBoolean,
	kJSONInteger,
	kJSONString,
	kJSONList,
	kJSONObject
} eJSONValueKind;

class JSONValue; // Forward declaration

#ifndef SWIG
class JSONKeyValuePair {
 protected:
	std::string *m_pKey;
	JSONValue *m_pValue;
	JSONKeyValuePair *m_pNext;
 public:
	JSONKeyValuePair(std::string *pKey, JSONValue *pValue, JSONKeyValuePair *pNext);
	~JSONKeyValuePair();

	std::string getKey(void) const { return *m_pKey; };
	JSONValue *extractValue(void) { JSONValue *pResult = m_pValue; m_pValue = 0; return pResult; };

	void setNext(JSONKeyValuePair *pNext) { m_pNext = pNext; };
	JSONKeyValuePair *getNext(void) { return m_pNext; };
 private:
	JSONKeyValuePair(const JSONKeyValuePair& other);
	JSONKeyValuePair& operator=(const JSONKeyValuePair& other);
};

#endif // !defined(SWIG)

#ifndef SWIG

class JSONListElement {
 protected:
	JSONValue *m_pValue;
	JSONListElement *m_pNext;
 public:
	JSONListElement(JSONValue *pValue);
	~JSONListElement();
	JSONValue *extractValue(void) { JSONValue *pResult = m_pValue; m_pValue = 0; return pResult; };
	void setNext(JSONListElement *pNext) { m_pNext = pNext; };
	JSONListElement *getNext(void) { return m_pNext; };
	static void destroyList(JSONListElement *pList);
 private:
	JSONListElement(const JSONListElement& other);
	JSONListElement& operator=(const JSONListElement& other);
};

#endif // !defined(SWIG)

class JSONValue {
#ifndef SWIG
	eJSONValueKind m_kind;
	union {
		bool m_boolean;
		long long m_integer;
		const std::string* m_pString;
		std::list<JSONValue*> *m_pList;
		std::map<std::string, JSONValue*> *m_pObject;
	} m_value;
#endif
 public:
	JSONValue();                        // null
	JSONValue(bool bValue);             // boolean
	JSONValue(long long integer);       // integer
#ifndef SWIG
	JSONValue(std::string *pString);    // string
	JSONValue(JSONListElement *pTail);  // list
	JSONValue(JSONKeyValuePair *pTail); // object
#endif
	JSONValue(eJSONValueKind kind);     // any of the above, making an empty (or null, false, or 0) JSONValue
	JSONValue(const JSONValue& other);
	~JSONValue();
	eJSONValueKind getKind(void) const { return m_kind; };
	std::string getString(void) const;
	bool getBoolean(void) const;
	long long getInteger(void) const;
	const std::list<JSONValue*>& getList(void) const;
	void append(JSONValue* pValue);
	void addKeyValue(const std::string& key, JSONValue *pValue);
	const JSONValue* getObjectValue(const std::string& key) const;
	JSONValue* getObjectValue(const std::string& key);
#ifndef SWIG
	const JSONValue* operator[](const std::string& key) const { return getObjectValue(key); };
	JSONValue* operator[](const std::string& key) { return getObjectValue(key); };
#endif
	std::list<std::string> getObjectKeys(void) const;
	bool isFalse() const;
	bool hasObjectKey(const std::string& key) const;
#ifndef SWIG
	void pretty(std::ostream *pOut, int indent_level = 0, bool bEscapeAsUnicode = false) const;
	void printCompact(std::ostream *pOut, bool bEscapeAsUnicode = false) const;
	bool castToBool() const;
	long long castToInteger() const;
	std::string castToString() const;
#endif
 private:
#ifndef SWIG
	JSONValue& operator=(const JSONValue& other);
	void copyOther(const JSONValue& other);
#endif
	void deleteList(void);
	void deleteObject(void);
#ifndef SWIG
	void indent(std::ostream *pOut, int indent_level) const;
#endif
};

#ifndef SWIG
class JSONExecEnv {
 protected:
	std::string *m_pError;
	JSONValue *m_pTopValue;
 public:
	JSONScanner *pLexer;
	JSONExecEnv();
 private:
	// Leave unimplemented, so as to prevent the compiler from
	// creating crashing ones
	JSONExecEnv(const JSONExecEnv& other);
	JSONExecEnv& operator=(const JSONExecEnv& other);
 public:
	~JSONExecEnv();
	bool bSyntaxError;

	std::string getInputScannedSoFar();

	void setTopValue(JSONValue *pValue);
	JSONValue *extractTopValue(void);

	void clearError(void);
	std::string getError();
	void addToError(const std::string& addition);
	void prependError(const std::string& addition);

	void clean();
};
#endif

#ifndef SWIG
extern JSONValue *parseJSON(JSONExecEnv* pEE, const std::string& json_string, bool& bResult);
#endif

#ifdef SWIG
%apply std::string& OUTPUT { std::string& error_message }
#endif
extern JSONValue *readAndParseJSONFromString(const std::string& json_string, std::string& error_message);
extern JSONValue *readAndParseJSONFromFile(const std::string& filename, std::string& error_message);

#endif
