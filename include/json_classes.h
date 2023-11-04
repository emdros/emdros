/*
 * json_classes.h
 *
 * JSON classes
 *
 * Ulrik Sandborg-Petersen
 * Created: 7/28-2008
 * Last update: 11/4-2023
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
        // null value
	JSONValue();
        // boolean value
	JSONValue(bool bValue);
        // integer
	JSONValue(long long integer);
#ifndef SWIG
        // string
	JSONValue(std::string *pString);    // string
        // list
	JSONValue(JSONListElement *pTail);  // list
        // object
	JSONValue(JSONKeyValuePair *pTail); //
#endif
        // any of the above, making an empty (or null, false, or 0) JSONValue
	JSONValue(eJSONValueKind kind);
        // Copy constructor
	JSONValue(const JSONValue& other);
        // Destructor
	~JSONValue();

        // get the kind
	eJSONValueKind getKind(void) const { return m_kind; };

        // If the value is a string, return that value.
        // Otherwise, return the empty string "".
	std::string getString(void) const;

        // If the value is a boolean, return that value.
        // Otherwise, return false.
	bool getBoolean(void) const;

        // If the value is a JSON integer, return that value.
        // Otherwise, return 0
	long long getInteger(void) const;

        // If the value is a JSON list, construct and return a
        // std::list<JSONValue*> with the contents of the string.
        // Otherwise, return the empty list.
	const std::list<JSONValue*>& getList(void) const;

        // If the value is a JSON list, append the value.
        // Otherwise, do nothing.
	void append(JSONValue* pValue);

        // If the value is a JSON object, add the key and the value.
        // Otherwise, do nothing.
	void addKeyValue(const std::string& key, JSONValue *pValue);

        // If the value is a JSON object, look for the key. If the key
        // is found, return the corresponding JSONValue* value. If the
        // key is not found, return 0 (nil). If the value is not a
        // JSON object, return 0 (nil),
	const JSONValue* getObjectValue(const std::string& key) const;

        // non-const variant. Does the exact same thing as the const
        // version.
	JSONValue* getObjectValue(const std::string& key);
#ifndef SWIG

        // Calls getObjectValue() const version.
	const JSONValue* operator[](const std::string& key) const { return getObjectValue(key); };

        // Calls getObjectValue() non-const version.
	JSONValue* operator[](const std::string& key) { return getObjectValue(key); };
#endif

        // If the value is a JSON object, return a list of the keys in it.
        // Otherwise, return the empty list.
	std::list<std::string> getObjectKeys(void) const;

        // Return true if one of the following is true:
        // - kind is kJSONNull.
        // - kind is kJSONBoolean, and the value is false.
        // - kind is kJSONInteger, and the value is 0L.
        // - kind is kJSONString, and the string is empty.
        // - kind is kJSONList, and the list is empty.
        // - kind is kJSONObject, and the object has no key/value pairs.
        // Otherwise, returns false.
	bool isFalse() const;

        // If the value is a kJSONObject, returns true if the object
        // has the key provided, and false if the object does not have
        // the key.
        // If the value is not a kJSONObject, return false.
	bool hasObjectKey(const std::string& key) const;
#ifndef SWIG

        // Pretty-print to pOut, with indentation and newlines.  Use
        // the initial indentation level provided in the indent_level
        // parameter.  Also, if the parameter bEscapeAsUnicode is
        // true, UTF-8-encoded code points are escaped as \u rather
        // than being copied as the raw UTF-8 characters.
        void pretty(std::ostream *pOut, int indent_level = 0, bool bEscapeAsUnicode = false) const;

        // Pretty-print as a compact string to pOut, with as little
        // whitespace as possible.  If the parameter bEscapeAsUnicode
        // is true, UTF-8-encoded code points are escaped as \u rather
        // than being copied as the raw UTF-8 characters.
	void printCompact(std::ostream *pOut, bool bEscapeAsUnicode = false) const;
#endif
        // Return !(this->isFalse()).
	bool castToBool() const;

        // - If kind is kJSONNull, return 0.
        // - If kind is kSONBooelan, return 1L if the value is true, or 0
        //   if the value is false.
        // - If the kind is kJSONInteger, return the value.
        // - If the kind is kJSONString, attempt to convert the
        //   contents of the string to to longlong, and return the
        //   value.
        // - If the kind is kJSONList, return the size of the list.
        // - If the kind is kJSONObject, return the number of key/value pairs.
	long long castToInteger() const;

        // Use pretty() and return the string.
	std::string castToString(bool bEscapeAsUnicode = false) const;

        // Use printCompact() and return the string.
	std::string castToCompactString(bool bEscapeAsUnicode = false) const;
 private:
#ifndef SWIG
        // Assignment operator
	JSONValue& operator=(const JSONValue& other);

        // copying Mechanism, used in the copy constructor and the
        // assignment operator.
	void copyOther(const JSONValue& other);
#endif
        // If it is a list, deletes the list items, then deletes the
        // list, and sets the m_kind to kJSONNull.
        // If it is not a list, do nothing.
	void deleteList(void);

        // If it is a kJSONObject, delete all key/value pairs. Then
        // deletes the object, then sets m_kind to kJSONNull. If it is
        // not a kJSONObject, do nothing.
	void deleteObject(void);
#ifndef SWIG
        // Emit some 3 spaces indent_level times.  This is used in
        // pretty(), and hence also in castToString().
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
// Parse the json_string parameter and return it as a JSONValue* object.
// 0 (nil) is returned if this is unsuccessful.
// Any error message is returned in the error_message out parameter.
extern JSONValue *readAndParseJSONFromString(const std::string& json_string, std::string& error_message);

#ifdef SWIG
%apply std::string& OUTPUT { std::string& error_message }
#endif
// Parse the contents of the file with the given filename as JSON, and
// return it as a JSONValue* object.
// 0 (nil) is returned if this is unsuccessful.
// Any error message is returned in the error_message out parameter.
extern JSONValue *readAndParseJSONFromFile(const std::string& filename, std::string& error_message);

#endif
