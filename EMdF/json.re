/*
 * json.re
 *
 * JSON Lexer based on re2c
 *
 * Ulrik Sandborg-Petersen
 * Created: 7/28-2008
 * Last update: 7/30-2016
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

#include <json_lexer.h>
#include "jsonpars.h"
#include <iostream>
#include <string_func.h>

#define YYCTYPE         unsigned char
#define YYCURSOR        cur
#define YYLIMIT         lim
#define YYMARKER        ptr
/*!re2c
re2c:yyfill:enable = 0;
re2c:labelprefix = jj;
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


JSONScanner::JSONScanner(const std::string& strIn)
{
	m_szIn = strIn;
	sz = m_szIn.c_str();
	lim = sz + strIn.length();
	bot = tok = ptr = pos = top = eof = 0;
	cur = sz;
	m_pOpenBrace = newToken();
	m_pCloseBrace = newToken();
	m_pOpenBracket = newToken();
	m_pCloseBracket = newToken();
	m_pTrue = newToken();
	m_pFalse = newToken();
	m_pNull = newToken();
	m_pComma = newToken();
	m_pColon = newToken();
	m_pOpenBrace->setName("{");
	m_pCloseBrace->setName("}");
	m_pOpenBracket->setName("[");
	m_pCloseBracket->setName("]");
	m_pTrue->setName("true");
	m_pFalse->setName("false");
	m_pNull->setName("null");
	m_pComma->setName(",");
	m_pColon->setName(":");
}

JSONScanner::~JSONScanner()
{
	if (bot) {
		delete[] bot;
	}
	delete(m_pOpenBrace);
	delete(m_pCloseBrace);
	delete(m_pOpenBracket);
	delete(m_pCloseBracket);
	delete(m_pTrue);
	delete(m_pFalse);
	delete(m_pNull);
	delete(m_pComma);
	delete(m_pColon);

}
bool JSONScanner::isLocallyAllocatedToken(Token *pToken)
{
	return pToken == m_pOpenBrace
	|| pToken == m_pCloseBrace
	|| pToken == m_pOpenBracket
	|| pToken == m_pCloseBracket
	|| pToken == m_pTrue
	|| pToken == m_pFalse
	|| pToken == m_pNull
	|| pToken == m_pComma
	|| pToken == m_pColon;
}


#define TOKEN_RETURN(X,Y) { (*yylval)->setName(X); return Y; }
#define LOCAL_TOKEN_RETURN(L,Y) { *yylval = L; return Y; }
#define NEW_TOKEN() { (*yylval) = newToken(); }
#define NEW_TOKEN_RETURN(X,Y) { NEW_TOKEN() TOKEN_RETURN(X,Y) }



int JSONScanner::scan(Token **ppYYLVAL)
{
	yylval = ppYYLVAL;

 scan:	
	tok = cur;

/*!re2c
F A L S E                   { LOCAL_TOKEN_RETURN(m_pFalse, T_JSON_KEY_FALSE); }
T R U E                     { LOCAL_TOKEN_RETURN(m_pTrue, T_JSON_KEY_TRUE); }
N U L L                     { LOCAL_TOKEN_RETURN(m_pNull, T_JSON_KEY_NULL); }
"{"                         { LOCAL_TOKEN_RETURN(m_pOpenBrace, T_JSON_KEY_BRACE_OPEN); }
"}"                         { LOCAL_TOKEN_RETURN(m_pCloseBrace,T_JSON_KEY_BRACE_CLOSE); }
"\["                        { LOCAL_TOKEN_RETURN(m_pOpenBracket, T_JSON_KEY_BRACKET_OPEN); }
"\]"                        { LOCAL_TOKEN_RETURN(m_pCloseBracket, T_JSON_KEY_BRACKET_CLOSE); }
":"                         { LOCAL_TOKEN_RETURN(m_pColon, T_JSON_KEY_COLON);  }
","                         { LOCAL_TOKEN_RETURN(m_pComma, T_JSON_KEY_COMMA); }
"-"?[0-9]{1,10}                 { NEW_TOKEN(); 
                                  addInteger();
                                  TOKEN_RETURN(INTEGER_MAGIC, T_JSON_INTEGER); }
'"'                          { 
                              NEW_TOKEN(); 
                              (*yylval)->setString(new std::string);
			      (*yylval)->pString->reserve(32);
			      goto stringdq; 
                            }
" "                         { goto scan; }
"\t"                        { goto scan; }
"\r"                        { goto scan; }
"\n"                        { goto scan; }
zero      { if (cur >= lim) { goto end; } }
dot       { goto end; /* anything else is an error, so return EOF. */ }
 */

stringdq:
	tok = cur;
/*!re2c
'"'                   { TOKEN_RETURN(STRING_MAGIC, T_JSON_STRING); }
SLASH "n"             { (*yylval)->pString->push_back('\n'); goto stringdq; }
SLASH "t"             { (*yylval)->pString->push_back('\t'); goto stringdq; }
SLASH "b"             { (*yylval)->pString->push_back('\b'); goto stringdq; }
SLASH "r"             { (*yylval)->pString->push_back('\r'); goto stringdq; }
SLASH "f"             { (*yylval)->pString->push_back('\f'); goto stringdq; }
SLASH SLASH           { (*yylval)->pString->push_back('\\'); goto stringdq; }
SLASH "?"             { (*yylval)->pString->push_back('\?'); goto stringdq; }
SLASH "'"             { (*yylval)->pString->push_back('\''); goto stringdq; }
SLASH '"'             { (*yylval)->pString->push_back('\"'); goto stringdq; }
SLASH "\n"            { /* eaten; it is a LineContinuation */ goto stringdq; }
SLASH "\r\n"          { /* eaten; it is a LineContinuation */ goto stringdq; }
SLASH "\r"            { /* eaten; it is a LineContinuation */ goto stringdq; }
SLASH "\xe2\x80\xa8"  { /* \\ + \u2028 (<LS>) is eaten; it is a LineContinuation */ goto stringdq; }
SLASH "\xe2\x80\xa9"  { /* \\ + \u2029 (<PS>) is eaten; it is a LineContinuation */ goto stringdq; }
SLASH "x" [A-Fa-f0-9][A-Fa-f0-9]    { tok += 2; // Go past SLASH x
			       std::string mystr;
			       getString(mystr);
			       (*yylval)->pString->push_back(hex2char(mystr.c_str()));
			       goto stringdq; 			     
                             }
SLASH "u" [A-Fa-f0-9][A-Fa-f0-9][A-Fa-f0-9][A-Fa-f0-9]    { tok += 2; // Go past SLASH x
			       std::string mystr;
			       getString(mystr);
			       long hex_long = hex2long(mystr);
			       std::string utf8_output;
			       long2utf8(hex_long, utf8_output);
			       (*(*yylval)->pString) += utf8_output;
			       goto stringdq; 			     
                             }
zero                         { if (cur >= lim) { goto end; } /* We might encounter a string which does not end before EOF. */ }
dot                          { (*yylval)->pString->push_back(*tok);
			       goto stringdq; }
"\n"                         { (*yylval)->pString->push_back('\n');
			       goto stringdq; }
*/

 end:
	return 0;
}


void JSONScanner::addToken(void)
{
	(*yylval)->setString(new std::string);
	std::string::size_type length = cur - tok;
	(*yylval)->pString->reserve(length);
	(*yylval)->pString->append(tok, length);
/*
	const char *p;
	for (p = tok;
	     p != cur;
	     ++p) {
	     (*yylval)->pString->push_back(*p);
	}
*/
}

void JSONScanner::addInteger(void)
{
	const unsigned int MAX_INTEGER_CHARS = 12;
	unsigned int cnt = cur - tok;
	if (cnt > MAX_INTEGER_CHARS) {
	   	std::string mystring;
		const char *p;
		for (p = tok;
		     p != cur;
	     	     ++p) {
		     mystring.append(1, *p);
		}
		(*yylval)->setInteger(sz2longlong(mystring.c_str()));
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
		(*yylval)->setInteger(sz2longlong(szInt));
	}
}

void JSONScanner::getString(std::string& str)
{
	std::string::size_type length = cur - tok;
	str.reserve(length);
	str.append(tok, length);
/*
	const char *p;
	for (p = tok;
	     p != cur;
	     ++p) {
	     str.append(1, *p);
	}
*/
}

std::string JSONScanner::getInputScannedSoFar()
{
	if (cur == 0 || sz == 0) {
	        return "";
        } else {
                long length = cur - sz;
	        if (length < 0) {
	              return "";
            	} else {
	              std::string result;
		      for (const char *p = sz;
		           p != cur;
		           ++p) {
		              result += *p;
		      } 
		      return result;
                }
        }
}
