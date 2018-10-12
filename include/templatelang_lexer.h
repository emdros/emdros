/*
 * templatelang_lexer.h
 *
 * TEMPLATELANG Lexer based on re2c
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


#ifndef TEMPLATELANG_LEXER__H__
#define TEMPLATELANG_LEXER__H__

#include <string>
#include "mql_yylex.h"

class TemplateLanguageScanner {
protected:
	const char *sz;
        Token *yylval;
	const char *bot, *tok, *ptr, *pos, *top, *eof;
	const char *lim, *cur;
public:
	TemplateLanguageScanner(const std::string& strIn);
	virtual ~TemplateLanguageScanner();
	int scan(Token *pYYLVAL);
	bool noMoreInput() { return cur != lim; };
protected:
	//char *fill(char* cursor);
	void addToken(void);
	void addInteger(void);
	void getString(std::string& str);
private:
	TemplateLanguageScanner(const TemplateLanguageScanner&); // Copy constructor unimplemented
	TemplateLanguageScanner& operator=(const TemplateLanguageScanner&); // Assignment operator unimplemented
};



#endif // TemplateLang_LEXER__H__
