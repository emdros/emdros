/*
 * templatelang.re
 *
 * Template language lexer based on re2c
 *
 * Ulrik Sandborg-Petersen
 * Created: 7/28-2008
 * Last update: 4/10-2019
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

#include <templatelang_lexer.h>
#include "templatelangpars.h"
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


TemplateLanguageScanner::TemplateLanguageScanner(const std::string& strIn)
{
	sz = strIn.c_str();
	lim = sz + strIn.length();
	bot = tok = ptr = pos = top = eof = 0;
	cur = sz;
}

TemplateLanguageScanner::~TemplateLanguageScanner()
{
	if (bot) {
		delete[] bot;
	}
}

/*
char *TemplateLanguageScanner::fill(char *cursor)
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

#define TEMPLATELANG_TOKEN_RETURN(X,Y) { yylval->setName(X); return Y; }

int TemplateLanguageScanner::scan(Token *pYYLVAL)
{
	yylval = pYYLVAL;

/* scan: */
	tok = cur;

/*!re2c
" }}"                  { TEMPLATELANG_TOKEN_RETURN("}}", T_TEMPLATELANG_KEY_BB_CLOSE); }
"{{ else }}"              { TEMPLATELANG_TOKEN_RETURN("else", T_TEMPLATELANG_KEY_ELSE); }
"{{ endif }}"             { TEMPLATELANG_TOKEN_RETURN("endif", T_TEMPLATELANG_KEY_ENDIF); }
"{{ id_d }}"              { TEMPLATELANG_TOKEN_RETURN("id_d", T_TEMPLATELANG_KEY_ID_D); }
"{{ firstmonad }}"        { TEMPLATELANG_TOKEN_RETURN("firstmonad", T_TEMPLATELANG_KEY_FIRSTMONAD); }
"{{ lastmonad }}"        { TEMPLATELANG_TOKEN_RETURN("lastmonad", T_TEMPLATELANG_KEY_LASTMONAD); }
"{{ dbname }}"            { TEMPLATELANG_TOKEN_RETURN("dbname", T_TEMPLATELANG_KEY_DBNAME); }
"{{ dictlookup"           { TEMPLATELANG_TOKEN_RETURN("dictlookup", T_TEMPLATELANG_KEY_DICTLOOKUP); }
"{{ counter"           { TEMPLATELANG_TOKEN_RETURN("counter", T_TEMPLATELANG_KEY_COUNTER); }
"{{ listappendbegin"   { TEMPLATELANG_TOKEN_RETURN("listappendbegin", T_TEMPLATELANG_KEY_LISTAPPENDBEGIN); }
"{{ listappendend }}"  { TEMPLATELANG_TOKEN_RETURN("listappendend", T_TEMPLATELANG_KEY_LISTAPPENDEND); }
"{{ listemit"          { TEMPLATELANG_TOKEN_RETURN("listemit", T_TEMPLATELANG_KEY_LISTEMIT); }
"{{ listclear"         { TEMPLATELANG_TOKEN_RETURN("listclear", T_TEMPLATELANG_KEY_LISTCLEAR); }
"{{ namedsetadd"   { TEMPLATELANG_TOKEN_RETURN("namedsetadd", T_TEMPLATELANG_KEY_NAMEDSETADD); }
"{{ namedsetaddend }}"  { TEMPLATELANG_TOKEN_RETURN("namedsetaddend", T_TEMPLATELANG_KEY_NAMEDSETADDEND); }
"{{ namedsetremove"   { TEMPLATELANG_TOKEN_RETURN("namedsetremove", T_TEMPLATELANG_KEY_NAMEDSETREMOVE); }
"{{ namedsetremoveend }}"  { TEMPLATELANG_TOKEN_RETURN("namedsetremoveend", T_TEMPLATELANG_KEY_NAMEDSETREMOVEEND); }
"{{ namedsetemit"          { TEMPLATELANG_TOKEN_RETURN("namedsetemit", T_TEMPLATELANG_KEY_NAMEDSETEMIT); }
"{{ namedsetclear"         { TEMPLATELANG_TOKEN_RETURN("namedsetclear", T_TEMPLATELANG_KEY_NAMEDSETCLEAR); }
"{{ setvarend }}"      { TEMPLATELANG_TOKEN_RETURN("setvarend", T_TEMPLATELANG_KEY_SETVAREND); }
"{{ setvar"            { TEMPLATELANG_TOKEN_RETURN("setvar", T_TEMPLATELANG_KEY_SETVAR); }
"{{ setnixing all }}"  { TEMPLATELANG_TOKEN_RETURN("setnixing all", T_TEMPLATELANG_KEY_SETNIXING_ALL); }
"{{ setnixing doc }}"  { TEMPLATELANG_TOKEN_RETURN("setnixing doc", T_TEMPLATELANG_KEY_SETNIXING_DOC); }
"{{ setnixing off }}"  { TEMPLATELANG_TOKEN_RETURN("setnixing off", T_TEMPLATELANG_KEY_SETNIXING_OFF); }
"{{ emitvar"           { TEMPLATELANG_TOKEN_RETURN("emitvar", T_TEMPLATELANG_KEY_EMITVAR); }
"{{ setcounter"        { TEMPLATELANG_TOKEN_RETURN("setcounter", T_TEMPLATELANG_KEY_SETCOUNTER); }
"{{ inccounter"        { TEMPLATELANG_TOKEN_RETURN("inccounter", T_TEMPLATELANG_KEY_INCCOUNTER); }
"{{ deccounter"        { TEMPLATELANG_TOKEN_RETURN("deccounter", T_TEMPLATELANG_KEY_DECCOUNTER); }
"{{ feature"           { TEMPLATELANG_TOKEN_RETURN("feature", T_TEMPLATELANG_KEY_FEATURE); }
"counter"              { TEMPLATELANG_TOKEN_RETURN("bare-counter", T_TEMPLATELANG_KEY_BARE_COUNTER); }
"feature"              { TEMPLATELANG_TOKEN_RETURN("bare-feature", T_TEMPLATELANG_KEY_BARE_FEATURE); }
"format"               { TEMPLATELANG_TOKEN_RETURN("bare-format", T_TEMPLATELANG_KEY_BARE_FORMAT); }
"from"                 { TEMPLATELANG_TOKEN_RETURN("bare-from", T_TEMPLATELANG_KEY_BARE_FROM); }
"maxlength"            { TEMPLATELANG_TOKEN_RETURN("bare-maxlength", T_TEMPLATELANG_KEY_BARE_MAXLENGTH); }
"substring"            { TEMPLATELANG_TOKEN_RETURN("bare-substring", T_TEMPLATELANG_KEY_BARE_SUBSTRING); }
"var"                  { TEMPLATELANG_TOKEN_RETURN("bare-var", T_TEMPLATELANG_KEY_BARE_VAR); }
"varnomangle"          { TEMPLATELANG_TOKEN_RETURN("bare-varnomangle", T_TEMPLATELANG_KEY_BARE_VARNOMANGLE); }
"{{ featurenomangle"   { TEMPLATELANG_TOKEN_RETURN("featurenomangle", T_TEMPLATELANG_KEY_FEATURENOMANGLE); }
"{{ featurejsonmangle"   { TEMPLATELANG_TOKEN_RETURN("featurejsonmangle", T_TEMPLATELANG_KEY_FEATUREJSONMANGLE); }
"featurenomangle"      { TEMPLATELANG_TOKEN_RETURN("bare-featurenomangle", T_TEMPLATELANG_KEY_BARE_FEATURENOMANGLE); }
"featurejsonmangle"      { TEMPLATELANG_TOKEN_RETURN("bare-featurejsonmangle", T_TEMPLATELANG_KEY_BARE_FEATUREJSONMANGLE); }
"{{ attrib"            { TEMPLATELANG_TOKEN_RETURN("attrib", T_TEMPLATELANG_KEY_ATTRIB); }
"{{ copyattribs"       { TEMPLATELANG_TOKEN_RETURN("copyattribs", T_TEMPLATELANG_KEY_COPYATTRIBS); }
"{{ attribnomangle"    { TEMPLATELANG_TOKEN_RETURN("attribnomangle", T_TEMPLATELANG_KEY_ATTRIB_NOMANGLE); }
"{{ if featureequal"   { TEMPLATELANG_TOKEN_RETURN("featureequal", T_TEMPLATELANG_KEY_IF_FEATUREEQUAL); }
"{{ if varequal"       { TEMPLATELANG_TOKEN_RETURN("varequal", T_TEMPLATELANG_KEY_IF_VAREQUAL); }
"{{ if listempty"      { TEMPLATELANG_TOKEN_RETURN("listempty", T_TEMPLATELANG_KEY_IF_LISTEMPTY); }
"{{ if namedsetempty"      { TEMPLATELANG_TOKEN_RETURN("namedsetempty", T_TEMPLATELANG_KEY_IF_NAMEDSETEMPTY); }
"{{ if hasattrib"      { TEMPLATELANG_TOKEN_RETURN("hasattrib", T_TEMPLATELANG_KEY_IF_HASATTRIB); }
"{{ if attribequal"    { TEMPLATELANG_TOKEN_RETURN("attribequal", T_TEMPLATELANG_KEY_IF_ATTRIBEQUAL); }
[A-Za-z_][A-Za-z0-9_]* { addToken(); TEMPLATELANG_TOKEN_RETURN("identifier", T_TEMPLATELANG_IDENTIFIER);}
[0-9][0-9]*            { addInteger();
                         TEMPLATELANG_TOKEN_RETURN(INTEGER_MAGIC, T_TEMPLATELANG_INTEGER); }
"'"                    { yylval->setString(new std::string);
		         goto stringsq; 
                       }
" "                    { TEMPLATELANG_TOKEN_RETURN(" ", T_TEMPLATELANG_KEY_SPACE); }
"\t"                   { yylval->setChar('\t'); TEMPLATELANG_TOKEN_RETURN(CHAR_MAGIC, T_TEMPLATELANG_CHAR); }
"\r"                   { yylval->setChar('\r'); TEMPLATELANG_TOKEN_RETURN(CHAR_MAGIC, T_TEMPLATELANG_CHAR); }
"\n"                   { TEMPLATELANG_TOKEN_RETURN("\\n", T_TEMPLATELANG_KEY_NEWLINE); }
"-"                    { TEMPLATELANG_TOKEN_RETURN("-", T_TEMPLATELANG_KEY_DASH); }
zero                   { if (cur >= lim) { goto end; } }
SLASH SLASH            { yylval->setChar('\\'); TEMPLATELANG_TOKEN_RETURN(CHAR_MAGIC, T_TEMPLATELANG_CHAR); }
SLASH "'"              { yylval->setChar('\''); TEMPLATELANG_TOKEN_RETURN(CHAR_MAGIC, T_TEMPLATELANG_CHAR); }
dot                    { yylval->setChar(*tok); TEMPLATELANG_TOKEN_RETURN(CHAR_MAGIC, T_TEMPLATELANG_CHAR);}
 */


stringsq:
	tok = cur;
/*!re2c
"'"                   { TEMPLATELANG_TOKEN_RETURN(STRING_MAGIC, T_TEMPLATELANG_STRING); }
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
SLASH [0-3][0-7][0-7] { tok += 1; // Go past SLASH 
		    std::string mystr;
		    getString(mystr);
		    yylval->pString->push_back(octal2char(mystr.c_str())); 
		    goto stringsq; }
SLASH "x" [A-Fa-f0-9][A-Fa-f0-9]    { tok += 2; // Go past SLASH x
			       std::string mystr;
			       getString(mystr);
			       yylval->pString->push_back(hex2char(mystr.c_str()));
			       goto stringsq; 			     
                             }
zero                         { if (cur >= lim) { goto end; } }
dot                          { yylval->pString->push_back(*tok);
			       goto stringsq; }
"\n"                         { yylval->pString->push_back('\n');
			       goto stringsq; }
*/


 end:
	return 0;
}


void TemplateLanguageScanner::addToken(void)
{
	yylval->setString(new std::string);
	const char *p;
	for (p = tok;
	     p != cur;
	     ++p) {
	     yylval->pString->push_back(*p);
	}
}

void TemplateLanguageScanner::addInteger(void)
{
	const size_t MAX_INTEGER_CHARS = 12;
	size_t cnt = cur - tok;
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

void TemplateLanguageScanner::getString(std::string& str)
{
	const char *p;
	for (p = tok;
	     p != cur;
	     ++p) {
	     str.push_back(*p);
	}
}

