/*
 * mql_yylex.h
 *
 * MQL yylex
 *
 * Ulrik Petersen
 * Created: 5/1 (1st of May, 2001)
 * Last update: 6/8-2018
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2018  Ulrik Sandborg-Petersen
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

/**@file mql_yylex.h
 *@brief Header file for yylex() function (MQL layer)
 */

#ifndef MQL_YYLEX__H__
#define MQL_YYLEX__H__

#include <string>
#include "string_func.h"
#include "arena.h"

extern int yylex(void *lvalp, void *parm);

#define STRING_MAGIC "*&$STRING$&*"
#define INTEGER_MAGIC "*&$INTEGER$&*"
#define CHAR_MAGIC "*&$CHAR$&*"

class Token{
public:
        Token() { init(); };
	~Token() { clear(); };
 private:
	// Leave unimplemented, so as to prevent compiler from
	// generating crashing ones...
	Token(const Token& other);
	Token& operator=(const Token& other);
 public:
	void init() { pString = 0; pBigstring = 0; integer = 0; szTokenName = 0; cChar = '\0'; };
	void clear() {
		delete pString;
		pString = 0;
		delete pBigstring;
		pBigstring = 0;
		szTokenName = 0;
	};
	void setChar(char c) { cChar = c; };
	void setString(std::string* pS) { pString = pS; };
	void setInteger(emdros_int64 i) { integer = i; };
	void setName(const char *szName) { szTokenName = szName; };
	std::string *extractString(void) { std::string *pS = pString; pString = 0; return pS; }
	std::string getTokenName(void) const { 
		if (szTokenName == 0) {
			return std::string("<<EOF>>"); // End of file
		} else if (strcmp(szTokenName, STRING_MAGIC) == 0) { 
		  return "the string '" + *pString + "'"; 
		} else if (strcmp(szTokenName, INTEGER_MAGIC) == 0) { 
			return std::string("the integer ") + emdros_int64ToString(integer); 
		} else if (strcmp(szTokenName, CHAR_MAGIC) == 0) { 
			return std::string("the character 0x") + char2hex(cChar); 
		} else { 
			return std::string("the token ") + szTokenName; 
		} 
	};
	std::string *pString;
	Bigstring *pBigstring;
	emdros_int64 integer;
	const char *szTokenName;
	char cChar;
};

inline void deleteToken(Token* pT)
{
	if (pT!=0) {
		// pT->clear();
		delete pT;
	}
}

inline Token* newToken()
{
	Token *pResult = new Token();
	pResult->init();
	return pResult;
}




#endif // MQL_YYLEX__H__
