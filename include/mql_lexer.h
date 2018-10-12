/*
 * mql_lexer.h
 *
 * MQL Lexer based on re2c
 *
 * Ulrik Petersen
 * Created: 6/23-2007
 * Last update: 6/24-2007
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


/**@file mql_lexer.h
 *@brief Header file for MQL Lexer (MQL layer)
 */


#ifndef MQL_LEXER__H__
#define MQL_LEXER__H__

#include <string>
#include "mql_yylex.h"

class MQLScanner {
protected:
	const char *sz;
        Token *yylval;
	const char *bot, *tok, *ptr, *pos, *top, *eof;
	const char *lim, *cur;
public:
	MQLScanner(const std::string& strIn);
	virtual ~MQLScanner();
	int scan(Token *pYYLVAL);
	bool noMoreInput() { return cur != lim; };
protected:
	//char *fill(char* cursor);
	void addToken(void);
	void addInteger(void);
	void getString(std::string& str);
private:
	MQLScanner(const MQLScanner&); // Copy constructor unimplemented
	MQLScanner& operator=(const MQLScanner&); // Assignment operator unimplemented
};



#endif // MQL_LEXER__H__
