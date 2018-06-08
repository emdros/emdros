/*
 * qdxml.h
 *
 * Quick'n Dirty XML SAX-like parser
 *
 * Ulrik Petersen
 * Created: 4/4-2007
 * Last update: 6/9-2018
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2007-2018  Ulrik Sandborg-Petersen
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

/**@file qdxml.h
 *@brief Header file for Quick'n Dirty XML SAX-like parser
 */

#ifndef QDXML__H__
#define QDXML__H__


#include "emdros-lconfig.h"
#include <iostream>
#include <istream>
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
	int m_quote_char;
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

	bool myisspace(int nc)
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
