/*
 * mql.re
 *
 * MQL Lexer based on re2c
 *
 * Ulrik Petersen
 * Created: 6/23-2007
 * Last update: 5/28-2018
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

/**@file mql_lexer.re
 *@brief Implementation file for MQL Lexer (MQL layer)
 */

#include <mql_lexer.h>
#include <mqlpars.h>
#include <iostream>
#include <string_func.h>

#undef YYCTYPE
#undef YYCURSOR
#undef YYLIMIT
#undef YYMARKER

#define YYCTYPE         unsigned char
#define YYCURSOR        cur
#define YYLIMIT         lim
#define YYMARKER        ptr
/*!re2c
re2c:yyfill:enable = 0;
re2c:labelprefix = xx;
*/


/*!re2c
zero     = "\000";
any      = [\001-\377];
dot      = any\[\n];
BACKPING = "\x60"; /* Backping "`" */
SLASH    = "\\";   /* slash "\\" */
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


MQLScanner::MQLScanner(const std::string& strIn)
{
	sz = strIn.c_str();
	lim = sz + strIn.length();
	bot = tok = ptr = pos = top = eof = 0;
	cur = sz;
}

MQLScanner::~MQLScanner()
{
	if (bot) {
		delete[] bot;
	}
}


#define MQL_TOKEN_RETURN(X,Y) { yylval->setName(X); return Y; }

int MQLScanner::scan(Token *pYYLVAL)
{
	yylval = pYYLVAL;

 scan:	
	tok = cur;

/*!re2c
C R E A T E                 { MQL_TOKEN_RETURN("create", T_KEY_CREATE); }
U P D A T E                 { MQL_TOKEN_RETURN("update", T_KEY_UPDATE); }
U N O R D E R E D G R O U P { MQL_TOKEN_RETURN("unorderedgroup", T_KEY_UNORDERED_GROUP); }
S E L E C T                 { MQL_TOKEN_RETURN("select", T_KEY_SELECT);  }
D R O P                     { MQL_TOKEN_RETURN("drop", T_KEY_DROP);  }
G E T                       { MQL_TOKEN_RETURN("get", T_KEY_GET);  }
I N S E R T                 { MQL_TOKEN_RETURN("insert", T_KEY_INSERT);  }
D E L E T E                 { MQL_TOKEN_RETURN("delete", T_KEY_DELETE);  }
D A T A B A S E             { MQL_TOKEN_RETURN("database", T_KEY_DATABASE);  }
U S E                       { MQL_TOKEN_RETURN("use", T_KEY_USE);  }
I N I T I A L I Z E         { MQL_TOKEN_RETURN("initialize", T_KEY_INITIALIZE);  }
V A C U U M                 { MQL_TOKEN_RETURN("vacuum", T_KEY_VACUUM); }
A N A L Y Z E               { MQL_TOKEN_RETURN("analyze", T_KEY_ANALYZE); }
B E G I N                   { MQL_TOKEN_RETURN("begin", T_KEY_BEGIN); }
C O M M I T                 { MQL_TOKEN_RETURN("commit", T_KEY_COMMIT); }
A B O R T                   { MQL_TOKEN_RETURN("abort", T_KEY_ABORT); }
T R A N S A C T I O N       { MQL_TOKEN_RETURN("transaction", T_KEY_TRANSACTION); }
T Y P E                     { MQL_TOKEN_RETURN("type", T_KEY_TYPE);  }
T Y P E S                   { MQL_TOKEN_RETURN("types", T_KEY_TYPES);  }
O B J E C T                 { MQL_TOKEN_RETURN("object", T_KEY_OBJECT);  }
O B J E C T S               { MQL_TOKEN_RETURN("objects", T_KEY_OBJECTS);  }
L I S T                     { MQL_TOKEN_RETURN("list", T_KEY_LIST); }
O F                         { MQL_TOKEN_RETURN("of", T_KEY_OF); }
I N                         { MQL_TOKEN_RETURN("in", T_KEY_IN);  }
I N D E X E S               { MQL_TOKEN_RETURN("indexes", T_KEY_INDEXES); }
I N D E X                   { MQL_TOKEN_RETURN("index", T_KEY_INDEX); }
I N T E G E R               { MQL_TOKEN_RETURN("integer", T_KEY_INTEGER);  }
S T R I N G                 { MQL_TOKEN_RETURN("string", T_KEY_STRING);  }
A S C I I                   { MQL_TOKEN_RETURN("ascii", T_KEY_ASCII); }
I D "_" D                   { MQL_TOKEN_RETURN("id_d", T_KEY_ID_D);  }
I D "_" D S                 { MQL_TOKEN_RETURN("id_ds", T_KEY_ID_DS);  }
D E F A U L T               { MQL_TOKEN_RETURN("default", T_KEY_DEFAULT);  }
C O M P U T E D             { MQL_TOKEN_RETURN("computed", T_KEY_COMPUTED);  }
A D D                       { MQL_TOKEN_RETURN("add", T_KEY_ADD);  }
R E M O V E                 { MQL_TOKEN_RETURN("remove", T_KEY_REMOVE);  }
F R O M                     { MQL_TOKEN_RETURN("from", T_KEY_FROM);  }
B Y                         { MQL_TOKEN_RETURN("by", T_KEY_BY);  }
E N U M                     { MQL_TOKEN_RETURN("enum", T_KEY_ENUM);  }
E N U M E R A T I O N       { MQL_TOKEN_RETURN("enumeration", T_KEY_ENUMERATION);  }
E N U M E R A T I O N S     { MQL_TOKEN_RETURN("enumerations", T_KEY_ENUMERATIONS);  }
C O N S T A N T S           { MQL_TOKEN_RETURN("constants", T_KEY_CONSTANTS);  }
S E G M E N T               { MQL_TOKEN_RETURN("segment", T_KEY_SEGMENT);  }
R A N G E                   { MQL_TOKEN_RETURN("range", T_KEY_RANGE);  }
S I N G L E                 { MQL_TOKEN_RETURN("single", T_KEY_SINGLE); }
M U L T I P L E             { MQL_TOKEN_RETURN("multiple", T_KEY_MULTIPLE); }
F O C U S                   { MQL_TOKEN_RETURN("focus", T_KEY_FOCUS);  }
A L L                       { MQL_TOKEN_RETURN("all", T_KEY_ALL);  }
H A S                       { MQL_TOKEN_RETURN("has", T_KEY_HAS);  }
W H E R E                   { MQL_TOKEN_RETURN("where", T_KEY_WHERE);  }
H A V I N G                 { MQL_TOKEN_RETURN("having", T_KEY_HAVING); }
F E A T U R E               { MQL_TOKEN_RETURN("feature", T_KEY_FEATURE);  }
F E A T U R E S             { MQL_TOKEN_RETURN("features", T_KEY_FEATURES);  }
A G G R E G A T E           { MQL_TOKEN_RETURN("aggregate", T_KEY_AGGREGATE);  }
U N I V E R S E             { MQL_TOKEN_RETURN("universe", T_KEY_UNIVERSE);  }
S U B S T R A T E           { MQL_TOKEN_RETURN("substrate", T_KEY_SUBSTRATE);  }
M O N A D S                 { MQL_TOKEN_RETURN("monads", T_KEY_MONADS);  }
M O N A D                   { MQL_TOKEN_RETURN("monad", T_KEY_MONAD);  }
U N I Q U E                 { MQL_TOKEN_RETURN("unique", T_KEY_UNIQUE);  }
U S I N G                   { MQL_TOKEN_RETURN("using", T_KEY_USING);  }
W I T H                     { MQL_TOKEN_RETURN("with", T_KEY_WITH);  }
W I T H O U T               { MQL_TOKEN_RETURN("without", T_KEY_WITHOUT);  }
K E Y                       { MQL_TOKEN_RETURN("key", T_KEY_KEY); }
N O R E T R I E V E         { MQL_TOKEN_RETURN("noretrieve", T_KEY_NORETRIEVE);  }
R E T R I E V E             { MQL_TOKEN_RETURN("retrieve", T_KEY_RETRIEVE);  }
F I R S T                   { MQL_TOKEN_RETURN("first", T_KEY_FIRST);  }
L A S T                     { MQL_TOKEN_RETURN("last", T_KEY_LAST);  }
O R                         { MQL_TOKEN_RETURN("or", T_KEY_OR);  }
A N D                       { MQL_TOKEN_RETURN("and", T_KEY_AND);  }
N O T                       { MQL_TOKEN_RETURN("not", T_KEY_NOT);  }
E X I S T S                 { MQL_TOKEN_RETURN("exists", T_KEY_EXISTS);  }
I F                         { MQL_TOKEN_RETURN("if", T_KEY_IF);  }
N O T E X I S T             { MQL_TOKEN_RETURN("notexist", T_KEY_NOTEXIST);  }
N O T E X I S T S           { MQL_TOKEN_RETURN("notexists", T_KEY_NOTEXISTS);  }
G A P                       { MQL_TOKEN_RETURN("gap", T_KEY_GAP);  }
G A P "?"                   { MQL_TOKEN_RETURN("gap?", T_KEY_OPT_GAP);  }
"\.\."                      { MQL_TOKEN_RETURN("..", T_KEY_POWER);  }
B E T W E E N               { MQL_TOKEN_RETURN("between", T_KEY_BETWEEN); }
A S                         { MQL_TOKEN_RETURN("as", T_KEY_AS);  }
N I L                       { MQL_TOKEN_RETURN("nil", T_KEY_NIL);  }
G O                         { MQL_TOKEN_RETURN("go", T_KEY_GO); }
R E T U R N I N G           { MQL_TOKEN_RETURN("returning", T_KEY_RETURNING); }
C A L C U L A T I O N       { MQL_TOKEN_RETURN("calculation", T_KEY_CALCULATION); }
M I N "_" M                 { MQL_TOKEN_RETURN("min_m", T_KEY_MIN_M); }
M A X "_" M                 { MQL_TOKEN_RETURN("max_m", T_KEY_MAX_M); }
M I N                       { MQL_TOKEN_RETURN("min", T_KEY_MIN); }
M A X                       { MQL_TOKEN_RETURN("max", T_KEY_MAX); }
S U M                       { MQL_TOKEN_RETURN("sum", T_KEY_SUM); }
C O U N T                   { MQL_TOKEN_RETURN("count", T_KEY_COUNT); }
F L A T                     { MQL_TOKEN_RETURN("flat", T_KEY_FLAT); }
F U L L                     { MQL_TOKEN_RETURN("full", T_KEY_FULL); }
O N                         { MQL_TOKEN_RETURN("on", T_KEY_ON); }
S H E A F                   { MQL_TOKEN_RETURN("sheaf", T_KEY_SHEAF); }
A T                         { MQL_TOKEN_RETURN("at", T_KEY_AT); }
Q U I T                     { MQL_TOKEN_RETURN("quit", T_KEY_QUIT); }
S E T                       { MQL_TOKEN_RETURN("set", T_KEY_SET); }
S E T S                     { MQL_TOKEN_RETURN("sets", T_KEY_SETS); }
U N I O N                   { MQL_TOKEN_RETURN("union", T_KEY_UNION); }
O V E R L A P               { MQL_TOKEN_RETURN("overlap", T_KEY_OVERLAP); }
P A R T "_" O F             { MQL_TOKEN_RETURN("part_of", T_KEY_PART_OF); }
S T A R T S "_" I N         { MQL_TOKEN_RETURN("starts_in", T_KEY_STARTS_IN); }
I N T E R S E C T           { MQL_TOKEN_RETURN("intersect", T_KEY_INTERSECT); }
D I F F E R E N C E         { MQL_TOKEN_RETURN("difference", T_KEY_DIFFERENCE); }
R E P L A C E               { MQL_TOKEN_RETURN("replace", T_KEY_REPLACE); }
E N C O D I N G             { MQL_TOKEN_RETURN("encoding", T_KEY_ENCODING); }
"="                         { MQL_TOKEN_RETURN("=", T_KEY_EQUALS);  }
"<"                         { MQL_TOKEN_RETURN("<", T_KEY_LESS_THAN);  }
">"                         { MQL_TOKEN_RETURN(">", T_KEY_GREATER_THAN);  }
"<>"                        { MQL_TOKEN_RETURN("<>", T_KEY_NOT_EQUAL);  }
"<="                        { MQL_TOKEN_RETURN("<=", T_KEY_LESS_THAN_OR_EQUAL);  }
"=<"                        { MQL_TOKEN_RETURN("=<", T_KEY_LESS_THAN_OR_EQUAL);  }
">="                        { MQL_TOKEN_RETURN(">=", T_KEY_GREATER_THAN_OR_EQUAL);  }
"=>"                        { MQL_TOKEN_RETURN("=>", T_KEY_GREATER_THAN_OR_EQUAL);  }
"\~"                        { MQL_TOKEN_RETURN("~", T_KEY_TILDE); }
"!\~"                       { MQL_TOKEN_RETURN("!~", T_KEY_NOT_TILDE); }
"\["                        { MQL_TOKEN_RETURN("[", T_KEY_OPEN_SQUARE_BRACKET); }
"\]"                        { MQL_TOKEN_RETURN("]", T_KEY_CLOSE_SQUARE_BRACKET); }
"{"                         { MQL_TOKEN_RETURN("{", T_KEY_OPEN_BRACE);  }
"}"                         { MQL_TOKEN_RETURN("}", T_KEY_CLOSE_BRACE);  }
"("                         { MQL_TOKEN_RETURN("(", T_KEY_OPEN_BRACKET); }
")"                         { MQL_TOKEN_RETURN(")", T_KEY_CLOSE_BRACKET); }
":="                        { MQL_TOKEN_RETURN(":=", T_KEY_ASSIGN);  }
":"                         { MQL_TOKEN_RETURN(":", T_KEY_COLON); }
";"                         { MQL_TOKEN_RETURN(";", T_KEY_SEMICOLON); }
","                         { MQL_TOKEN_RETURN(",", T_KEY_COMMA); }
"."                         { MQL_TOKEN_RETURN(".", T_KEY_DOT); }
"\*"                        { MQL_TOKEN_RETURN("*", T_KEY_STAR); }
"-"                         { MQL_TOKEN_RETURN("-", T_KEY_DASH); }
"!"                         { MQL_TOKEN_RETURN("!", T_KEY_EXCLAMATION); }
[a-zA-Z_][a-zA-Z0-9_]*      { addToken();
			      MQL_TOKEN_RETURN(STRING_MAGIC, T_IDENTIFIER);
			    }
(BACKPING [a-zA-Z_][a-zA-Z0-9_]*)+       { addToken();
                                           MQL_TOKEN_RETURN("mark", T_MARK); }
[0-9]{1,20}                 { addInteger();
                              MQL_TOKEN_RETURN(INTEGER_MAGIC, T_INTEGER); }
'"'                         { yylval->setString(new std::string);
			      yylval->pString->reserve(32);
			      goto stringdq; 
                            }
"'"                         { yylval->setString(new std::string);
			      yylval->pString->reserve(32);
			      goto stringsq; 
			    }
" "                         { goto scan; }
"\t"                        { goto scan; }
"\r"                        { goto scan; }
"\n"                        { goto scan; }
zero      { if (cur >= lim) { goto end; } }
dot       { yylval->setChar(*tok);
	    MQL_TOKEN_RETURN(CHAR_MAGIC, *tok); }
 */

stringdq:
	tok = cur;
/*!re2c
'"'                   { MQL_TOKEN_RETURN(STRING_MAGIC, T_STRING); }
SLASH "n"             { yylval->pString->push_back('\n'); goto stringdq; }
SLASH "t"             { yylval->pString->push_back('\t'); goto stringdq; }
SLASH "v"             { yylval->pString->push_back('\v'); goto stringdq; }
SLASH "b"             { yylval->pString->push_back('\b'); goto stringdq; }
SLASH "a"             { yylval->pString->push_back('\a'); goto stringdq; }
SLASH "r"             { yylval->pString->push_back('\r'); goto stringdq; }
SLASH "f"             { yylval->pString->push_back('\f'); goto stringdq; }
SLASH SLASH           { yylval->pString->push_back('\\'); goto stringdq; }
SLASH "?"             { yylval->pString->push_back('\?'); goto stringdq; }
SLASH "'"             { yylval->pString->push_back('\''); goto stringdq; }
SLASH '"'             { yylval->pString->push_back('\"'); goto stringdq; }
SLASH [0-3][0-7][0-7] { tok += 1; // Go past SLASH 
      		        char szOctal[4];
			const char *p = tok;
			char *q = &(szOctal[0]);
			*q++ = *p++;
			*q++ = *p++;
			*q++ = *p++;
			*q = '\0';
		        yylval->pString->push_back(octal2char(szOctal)); 
		        goto stringdq; }
SLASH "x" [A-Fa-f0-9][A-Fa-f0-9]    { 
			       	      yylval->pString->push_back(hex2char(tok[2], tok[3]));
			       	      goto stringdq; 			     
                             }
zero                         { if (cur >= lim) { goto end; } }
dot                          { yylval->pString->push_back(*tok);
			       goto stringdq; }
"\n"                         { yylval->pString->push_back('\n');
			       goto stringdq; }
*/

stringsq:
	tok = cur;
/*!re2c
[']                          { MQL_TOKEN_RETURN(STRING_MAGIC, T_STRING); }
zero                         { if (cur >= lim) { goto end; } }
"\n"                         { yylval->pString->push_back('\n');
			       goto stringsq; }
dot\[']                      { yylval->pString->push_back(*tok);
			       goto stringsq; }
*/
	
 end:
	return 0;
}


void MQLScanner::addToken(void)
{
	yylval->setString(new std::string);
	std::string::size_type length = cur - tok;
	yylval->pString->reserve(length);
	yylval->pString->append(tok, length);
/*
	const char *p;
	for (p = tok;
	     p != cur;
	     ++p) {
	     yylval->pString->push_back(*p);
	}
*/
}

void MQLScanner::addInteger(void)
{
	const unsigned int MAX_INTEGER_CHARS = 20;
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
	}
}

void MQLScanner::getString(std::string& str)
{
	std::string::size_type length = cur - tok;
	str.reserve(length);
	str.append(tok, length);
	/*
	const char *p;
	for (p = tok;
	     p != cur;
	     ++p) {
	     str.push_back(*p);
	}
*/

}

