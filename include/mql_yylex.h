/*
 * mql_yylex.h
 *
 * MQL yylex
 *
 * Ulrik Petersen
 * Created: 5/1 (1st of May, 2001)
 * Last update: 4/12-2019
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
	void setInteger(long long i) { integer = i; };
	void setName(const char *szName) { szTokenName = szName; };
	std::string *extractString(void) { std::string *pS = pString; pString = 0; return pS; }
	std::string getTokenName(void) const { 
		if (szTokenName == 0) {
			return std::string("<<EOF>>"); // End of file
		} else if (strcmp(szTokenName, STRING_MAGIC) == 0) { 
		  return "the string '" + *pString + "'"; 
		} else if (strcmp(szTokenName, INTEGER_MAGIC) == 0) { 
			return std::string("the integer ") + longlong2string(integer); 
		} else if (strcmp(szTokenName, CHAR_MAGIC) == 0) { 
			return std::string("the character 0x") + char2hex(cChar); 
		} else { 
			std::string result = std::string("the token '") + szTokenName + "'";
			if (pString != 0) {
				result += " with string '" + *pString + "'";
			}
			return result;
		} 
	};
	std::string *pString;
	Bigstring *pBigstring;
	long long integer;
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
