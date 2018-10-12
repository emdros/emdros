/*
 * qdxml.h
 *
 * Quick'n Dirty XML SAX-like parser
 *
 * Ulrik Petersen
 * Created: 4/4-2007
 * Last update: 3/3-2017
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


/**@file qdxml.h
 *@brief Header file for Quick'n Dirty XML SAX-like parser
 */

#ifndef QDXML__H__
#define QDXML__H__


#include "emdros-lconfig.h"
#include <iostream>


#if HAVE_ISTREAM
#include <istream>
#else
#include <istream.h>
#endif

#include <string>
#include <map>
#include <stack>
#include <sstream>

#include "exception_emdros.h"
#include "string_func.h"
#include "llist.h"

class QDException : public EmdrosException {
public:
	QDException(const std::string& msg) : EmdrosException(msg) {};
	~QDException() {};
};


typedef std::map<std::string, std::string> AttributeMap;


extern std::string getAttribute(const AttributeMap& attrs, const std::string& key);



class QDDocHandler {
 public:
	QDDocHandler() {};
	virtual ~QDDocHandler() {};
	virtual void startElement(const std::string& tag, const AttributeMap& attrs) { UNUSED(tag); UNUSED(attrs); };
	virtual void endElement(const std::string& tag) { UNUSED(tag); };
	virtual void startDocument(void) {};
	virtual void endDocument(void) {};
	virtual void text(const std::string& str) { UNUSED(str); };
	virtual void comment(const std::string& str) { UNUSED(str); };
};



class QDParser {
 protected:
	const static std::string NOTAG;
	eCharsets m_encoding;
	int m_line;
	int m_column;
	int m_tag_depth;
	AttributeMap m_attributes;
	std::istream *m_pInStream;
	QDDocHandler *m_pDH;
	std::string m_tag_name;
	std::string m_lvalue;
	std::string m_rvalue;
	bool m_end_of_line;
	char m_quote_char;
	std::string m_entity;
	std::string m_text;
	

	typedef enum {
		TEXT,
		ENTITY,
		OPEN_TAG,
		CLOSE_TAG,
		START_TAG,
		ATTRIBUTE_LVALUE,
		ATTRIBUTE_EQUAL,
		ATTRIBUTE_RVALUE,
		QUOTE,
		IN_TAG,
		SINGLE_TAG,
		COMMENT,
		DONE,
		DOCTYPE,
		IN_XMLDECLARATION,
		BEFORE_XMLDECLARATION,
		OPEN_XMLDECLARATION,
		CDATA
	} eQDStates;


	typedef std::stack<eQDStates> StateStack;
	StateStack m_stack;
 public:
	QDParser();
	~QDParser();
	void parse(QDDocHandler *pDH, std::istream *pStream);

 protected:
	void cleanUp() 
	{
		if (bot) {
			delete[] bot;
			bot = 0;
		}
	}
	void eraseAttributes()
	{
		m_attributes.clear();
	}

	bool myisspace(char nc)
	{
		return nc == (int) ' ' 
			|| nc == (int) '\n' 
			// || nc == (int) '\r'  // '\r' is converted to '\n' by getNextChar().
			|| nc == (int) '\t' 
			|| nc == (int) '\x0b' // Vertical space
			|| nc == (int) '\f';
	}

	void pushState(eQDStates state) {
		m_stack.push(state);
	}
		

	eQDStates popState(void)
	{
		if (!m_stack.empty()) {
			eQDStates result = m_stack.top();
			m_stack.pop();
			return result;
		} else {
			except("QDParser::popState: Attempted to pop off of an empty StateStack.");
			return TEXT; // Just to fool the compiler into giving now warning!
		}
	}

	void handle_entity(eQDStates mode);

	std::string state2string(eQDStates e);

	void except(const std::string& s);

	int getNextChar(void);
	int handleEOL(int c);

	void fillBuffer(void);
 protected:
	void readIntoBuffer();
	bool hasMoreInput() { return eof == 0 || cur != eof; };

 private:
	char *bot, *cur, *lim, *top, *eof;
};



#endif
