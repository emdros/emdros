/*
 * emdroslang.re
 *
 * Emdros language lexer based on re2c
 *
 * Ulrik Sandborg-Petersen
 * Created: 11/11-2016
 * Last update: 5/22-2017
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

#include <emdroslang_lexer.h>
#include "emdroslangpars.h"
#include <iostream>
#include <string_func.h>

#define YYCTYPE         unsigned char
#define YYCURSOR        cur
#define YYLIMIT         lim
#define YYMARKER        ptr
/*!re2c
re2c:yyfill:enable = 0;
re2c:labelprefix = tt;
*/


/*!re2c
zero     = "\000";
any      = [\001-\377];
dot      = any\[\n];
BACKPING = [\x60]; /* Backping "`" */
SLASH    = [\\];   /* slash "\\" */
A        = [Aa];
B        = [Bb];
C        = [Cc];
D        = [Dd];
E        = [Ee];
F        = [Ff];
G        = [Gg];
H        = [Hh];
I        = [Ii];
J        = [Jj];
K        = [Kk];
L        = [Ll];
M        = [Mm];
N        = [Nn];
O        = [Oo];
P        = [Pp];
Q        = [Qq];
R        = [Rr];
S        = [Ss];
T        = [Tt];
U        = [Uu];
V        = [Vv];
W        = [Ww];
X        = [Xx];
Y        = [Yy];
Z        = [Zz];
*/


EmdroslangScanner::EmdroslangScanner(const std::string& strIn)
{
	sz = strIn.c_str();
	lim = sz + strIn.length();
	bot = tok = ptr = pos = top = eof = 0;
	cur = sz;
}

EmdroslangScanner::~EmdroslangScanner()
{
	if (bot) {
		delete[] bot;
	}
}

/*
char *EmdroslangScanner::fill(char *cursor)
{
        if(!eof)
        {
                unsigned int cnt = tok - bot;
                if(cnt)
                {
                        memcpy(bot, tok, lim - tok);
                        tok = bot;
                        ptr -= cnt;
                        cursor -= cnt;
                        pos -= cnt;
                        lim -= cnt;
                }
                if((top - lim) < BSIZE)
                {
                        char *buf = new char[(lim - bot) + BSIZE];
                        memcpy(buf, tok, lim - tok);
                        tok = buf;
                        ptr = &buf[ptr - bot];
                        cursor = &buf[cursor - bot];
                        pos = &buf[pos - bot];
                        lim = &buf[lim - bot];
                        top = &lim[BSIZE];
                        delete [] bot;
                        bot = buf;
                }
                pIn->read(lim, BSIZE);
                if ((cnt = pIn->gcount()) != BSIZE )
                {
                        eof = &lim[cnt]; *eof++ = '\0';
                }
                lim += cnt;
        }
        return cursor;
}
*/

#define EMDROSLANG_TOKEN_RETURN(X,Y) { yylval->setName(X); return Y; }

int EmdroslangScanner::scan(Token *pYYLVAL)
{
	yylval = pYYLVAL;

scan:
	tok = cur;

/*!re2c
"{"                       { EMDROSLANG_TOKEN_RETURN("{", T_EMDROSLANG_KEY_LBRACE); }
"}"                       { EMDROSLANG_TOKEN_RETURN("}", T_EMDROSLANG_KEY_RBRACE); }

"("                       { EMDROSLANG_TOKEN_RETURN("(", T_EMDROSLANG_KEY_LPAREN); }
")"                       { EMDROSLANG_TOKEN_RETURN(")", T_EMDROSLANG_KEY_RPAREN); }

"["                       { EMDROSLANG_TOKEN_RETURN("[", T_EMDROSLANG_KEY_LSQUARE_BRACKET); }
"]"                       { EMDROSLANG_TOKEN_RETURN("]", T_EMDROSLANG_KEY_RSQUARE_BRACKET); }

","                       { EMDROSLANG_TOKEN_RETURN(",", T_EMDROSLANG_KEY_COMMA); }
";"                       { EMDROSLANG_TOKEN_RETURN(",", T_EMDROSLANG_KEY_SEMICOLON); }
"="                       { EMDROSLANG_TOKEN_RETURN("=", T_EMDROSLANG_KEY_ASSIGN); }

"=="                      { EMDROSLANG_TOKEN_RETURN("==", T_EMDROSLANG_KEY_EQEQ); }
"!="                      { EMDROSLANG_TOKEN_RETURN("!=", T_EMDROSLANG_KEY_NEQ); }
"<"                       { EMDROSLANG_TOKEN_RETURN("<", T_EMDROSLANG_KEY_LT); }
"<="                      { EMDROSLANG_TOKEN_RETURN("<=", T_EMDROSLANG_KEY_LEQ); }
">"                       { EMDROSLANG_TOKEN_RETURN(">", T_EMDROSLANG_KEY_GT); }
">="                      { EMDROSLANG_TOKEN_RETURN(">=", T_EMDROSLANG_KEY_GEQ); }

"::"                      { EMDROSLANG_TOKEN_RETURN("::", T_EMDROSLANG_KEY_COLON_COLON); }
":"                       { EMDROSLANG_TOKEN_RETURN(":", T_EMDROSLANG_KEY_COLON); }

"+"                       { EMDROSLANG_TOKEN_RETURN("+", T_EMDROSLANG_KEY_PLUS); }
"-"                       { EMDROSLANG_TOKEN_RETURN("-", T_EMDROSLANG_KEY_MINUS); }
"*"                       { EMDROSLANG_TOKEN_RETURN("*", T_EMDROSLANG_KEY_STAR); }
"/"                       { EMDROSLANG_TOKEN_RETURN("/", T_EMDROSLANG_KEY_SLASH); }
"%"                       { EMDROSLANG_TOKEN_RETURN("%", T_EMDROSLANG_KEY_PERCENT); }
"!"                       { EMDROSLANG_TOKEN_RETURN("!", T_EMDROSLANG_KEY_NOT); }

"&&"                      { EMDROSLANG_TOKEN_RETURN("&&", T_EMDROSLANG_KEY_AND_AND); }
"||"                      { EMDROSLANG_TOKEN_RETURN("&&", T_EMDROSLANG_KEY_PIPE_PIPE); }

"fetchinfo"               { EMDROSLANG_TOKEN_RETURN("fetchinfo", T_EMDROSLANG_KEY_FETCHINFO); }

"stylesheet"              { EMDROSLANG_TOKEN_RETURN("stylesheet", T_EMDROSLANG_KEY_STYLESHEET); }

"var"                     { EMDROSLANG_TOKEN_RETURN("var", T_EMDROSLANG_KEY_VAR); }
"false"                     { EMDROSLANG_TOKEN_RETURN("false", T_EMDROSLANG_KEY_FALSE); }
"true"                     { EMDROSLANG_TOKEN_RETURN("true", T_EMDROSLANG_KEY_TRUE); }
"null"                     { EMDROSLANG_TOKEN_RETURN("null", T_EMDROSLANG_KEY_NULL); }

"while"                     { EMDROSLANG_TOKEN_RETURN("while", T_EMDROSLANG_KEY_WHILE); }
"do"                     { EMDROSLANG_TOKEN_RETURN("do", T_EMDROSLANG_KEY_DO); }
"for"                     { EMDROSLANG_TOKEN_RETURN("for", T_EMDROSLANG_KEY_FOR); }
"in"                     { EMDROSLANG_TOKEN_RETURN("in", T_EMDROSLANG_KEY_IN); }
"if"                     { EMDROSLANG_TOKEN_RETURN("if", T_EMDROSLANG_KEY_IF); }
"elif"                     { EMDROSLANG_TOKEN_RETURN("elif", T_EMDROSLANG_KEY_ELIF); }
"else"                     { EMDROSLANG_TOKEN_RETURN("else", T_EMDROSLANG_KEY_ELSE); }

"function"                { EMDROSLANG_TOKEN_RETURN("function", T_EMDROSLANG_KEY_FUNCTION); }

"xmlrenderinfo"           { EMDROSLANG_TOKEN_RETURN("xmlrenderinfo", T_EMDROSLANG_KEY_XMLRENDERINFO); }


[A-Za-z_][A-Za-z0-9_]* { addToken(); EMDROSLANG_TOKEN_RETURN("identifier", T_EMDROSLANG_IDENTIFIER);}
[0-9][0-9]*            { addInteger();
                         EMDROSLANG_TOKEN_RETURN(INTEGER_MAGIC, T_EMDROSLANG_INTEGER); }
"'"                    { yylval->setString(new std::string);
		         goto stringsq; 
                       }
"/*"                    { goto comment_slash_star; }
"//"                    { goto comment_slash_slash; }
"\""                    { yylval->setString(new std::string);
		         goto stringdq; 
                       }
zero                   { if (cur >= lim) { goto end; } }
 */


stringsq:
	tok = cur;
/*!re2c
"'"                   { EMDROSLANG_TOKEN_RETURN(STRING_MAGIC, T_EMDROSLANG_STRING); }
SLASH "n"             { yylval->pString->push_back('\n'); goto stringsq; }
SLASH "t"             { yylval->pString->push_back('\t'); goto stringsq; }
SLASH "v"             { yylval->pString->push_back('\v'); goto stringsq; }
SLASH "b"             { yylval->pString->push_back('\b'); goto stringsq; }
SLASH "a"             { yylval->pString->push_back('\a'); goto stringsq; }
SLASH "r"             { yylval->pString->push_back('\r'); goto stringsq; }
SLASH "f"             { yylval->pString->push_back('\f'); goto stringsq; }
SLASH SLASH           { yylval->pString->push_back('\\'); goto stringsq; }
SLASH "?"             { yylval->pString->push_back('?'); goto stringsq; }
SLASH "'"             { yylval->pString->push_back('\''); goto stringsq; }
SLASH '"'             { yylval->pString->push_back('"'); goto stringsq; }
SLASH [0-9][0-9]?     { std::string mystr;
			getString(mystr);
			yylval->pString->append(mystr);
		      }
SLASH "u" [1]?[A-Fa-f0-9][A-Fa-f0-9][A-Fa-f0-9][A-Fa-f0-9]    { tok += 2; // Go past SLASH u
			       std::string mystr;
			       getString(mystr);
			       long utf8 = hex2long(mystr);
			       std::string mystr2;
			       long2utf8(utf8, mystr2);
			       yylval->pString->append(mystr2);
			       goto stringsq; 			     
                             }
zero                         { if (cur >= lim) { goto end; } }
dot                          { yylval->pString->push_back(*tok);
			       goto stringsq; }
"\n"                         { yylval->pString->push_back('\n');
			       goto stringsq; }
*/

comment_slash_star:
	tok = cur;
/*!re2c
"*/"                  { goto scan; }
zero                         { if (cur >= lim) { goto end; } }
dot                          { goto comment_slash_star; }
"\n"                         { goto comment_slash_star; }
*/

comment_slash_slash:
	tok = cur;
/*!re2c
zero                         { if (cur >= lim) { goto end; } }
dot                          { goto comment_slash_slash; }
"\n"                         { goto scan; }
*/

stringdq:
	tok = cur;
/*!re2c
"\""                  { EMDROSLANG_TOKEN_RETURN(STRING_MAGIC, T_EMDROSLANG_STRING); }
SLASH "n"             { yylval->pString->push_back('\n'); goto stringsq; }
SLASH "t"             { yylval->pString->push_back('\t'); goto stringsq; }
SLASH "v"             { yylval->pString->push_back('\v'); goto stringsq; }
SLASH "b"             { yylval->pString->push_back('\b'); goto stringsq; }
SLASH "a"             { yylval->pString->push_back('\a'); goto stringsq; }
SLASH "r"             { yylval->pString->push_back('\r'); goto stringsq; }
SLASH "f"             { yylval->pString->push_back('\f'); goto stringsq; }
SLASH SLASH           { yylval->pString->push_back('\\'); goto stringsq; }
SLASH "?"             { yylval->pString->push_back('\?'); goto stringsq; }
SLASH "'"             { yylval->pString->push_back('\''); goto stringsq; }
SLASH '"'             { yylval->pString->push_back('\"'); goto stringsq; }
SLASH [0-9][0-9]      { yylval->pString->push_back(*tok); goto stringsq; }
SLASH [0-9]           { yylval->pString->push_back(*tok); goto stringsq; }
SLASH "u" [1]?[A-Fa-f0-9][A-Fa-f0-9][A-Fa-f0-9][A-Fa-f0-9]    { tok += 2; // Go past SLASH u
			       std::string mystr;
			       getString(mystr);
			       long utf8 = hex2long(mystr);
			       std::string mystr2;
			       long2utf8(utf8, mystr2);
			       yylval->pString->append(mystr2);
			       goto stringsq; 			     
                             }
zero                         { if (cur >= lim) { goto end; } }
dot                          { yylval->pString->append(*tok, 1);
			       goto stringsq; }
"\n"                         { yylval->pString->append('\n', 1);
			       goto stringsq; }
*/


 end:
	return 0;
}


void EmdroslangScanner::addToken(void)
{
	yylval->setString(new std::string);
	const char *p;
	for (p = tok;
	     p != cur;
	     ++p) {
	     yylval->pString->push_back(*p);
	}
}

void EmdroslangScanner::addInteger(void)
{
	const unsigned int MAX_INTEGER_CHARS = 12;
	unsigned int cnt = cur - tok;
	if (cnt > MAX_INTEGER_CHARS) {
	   	std::string mystring;
		const char *p;
		for (p = tok;
		     p != cur;
	     	     ++p) {
		     mystring.push_back(*p);
		}
		yylval->setInteger(sz2longlong(mystring.c_str()));
		yylval->setString(new std::string(mystring));
        } else {
		char szInt[MAX_INTEGER_CHARS+1];

		const char *p;
		char *q = szInt;
		for (p = tok;
		     p != cur;
	     	     ++p) {
		     *q++ = *p;
		}
		*q = '\0';
		yylval->setInteger(sz2longlong(szInt));
		yylval->setString(new std::string(szInt));
	}
}

void EmdroslangScanner::getString(std::string& str)
{
	const char *p;
	for (p = tok;
	     p != cur;
	     ++p) {
	     str.push_back(*p);
	}
}

