/*
 * json_lexer.h
 *
 * JSON Lexer based on re2c
 *
 * Ulrik Sandborg-Petersen
 * Created: 6/23-2007
 * Last update: 4/9-2014
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


#ifndef JSON_LEXER__H__
#define JSON_LEXER__H__

#include <string>
#include "mql_yylex.h" /* from Emdros */

class JSONScanner {
protected:
	std::string m_szIn;
	const char *sz;
        Token **yylval;
	const char *bot, *tok, *ptr, *pos, *top, *eof;
	const char *lim, *cur;
	Token *m_pOpenBrace, *m_pCloseBrace, 
		*m_pOpenBracket, *m_pCloseBracket, 
		*m_pTrue, *m_pFalse, *m_pNull,
		*m_pComma, *m_pColon;
public:
	JSONScanner(const std::string& strIn);
	virtual ~JSONScanner();
	int scan(Token **ppYYLVAL);
	bool noMoreInput() { return cur != lim; };
	std::string getInputScannedSoFar();
	bool isLocallyAllocatedToken(Token *pToken);
protected:
	//char *fill(char* cursor);
	void addToken(void);
	void addInteger(void);
	void getString(std::string& str);
private:
	JSONScanner(const JSONScanner&); // Copy constructor unimplemented
	JSONScanner& operator=(const JSONScanner&); // Assignment operator unimplemented
};



#endif // JSON_LEXER__H__
