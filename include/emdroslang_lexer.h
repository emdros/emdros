/*
 * emdroslang_lexer.h
 *
 * EmdrosLang Lexer based on re2c
 *
 * Ulrik Sandborg-Petersen
 * Created: 6/23-2007
 * Last update: 10/3-2011
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


#ifndef EMDROSLANG_LEXER__H__
#define EMDROSLANG_LEXER__H__

#include <string>
#include "mql_yylex.h"

class EmdroslangScanner {
protected:
	const char *sz;
        Token *yylval;
	const char *bot, *tok, *ptr, *pos, *top, *eof;
	const char *lim, *cur;
public:
	EmdroslangScanner(const std::string& strIn);
	virtual ~EmdroslangScanner();
	int scan(Token *pYYLVAL);
	bool noMoreInput() { return cur != lim; };
protected:
	//char *fill(char* cursor);
	void addToken(void);
	void addInteger(void);
	void getString(std::string& str);
private:
	EmdroslangScanner(const EmdroslangScanner&); // Copy constructor unimplemented
	EmdroslangScanner& operator=(const EmdroslangScanner&); // Assignment operator unimplemented
};



#endif // EMDROSLANG_LEXER__H__
