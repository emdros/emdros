/*
 * pennlexer.lxx
 *
 * Penn Treebank lexer
 *
 * Ulrik Petersen
 * Created: 2/17-2006
 * Last update: 11/30-2018
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




#include <string>
#include <sstream>
#include <pennimporter.h>
#include "pennparser.h"

#define TOKEN_RETURN(X,Y) { yylval->setName(X); /* std::cerr << "UP200: TOKEN_NAME = " << #X << ", token = " << Y << std::endl; */ return Y; }

/*!max:re2c */


#define BSIZE 8192
#if YYMAXFILL > BSIZE
#error YYMAXFILL must be less than or equal to BSIZE!
#endif

#define YYCTYPE         unsigned char
#define YYCURSOR        cur
#define YYLIMIT         lim
#define YYMARKER        ptr
#define YYFILL(n)       {cur = fill(cur);}

/*!re2c
zero       = "\000";
any        = [\001-\377];
dot        = any\[\n];
pos        =  ([^ \t\n\r()\000]+)|([$][(])|([$][(][-][^ \t\n\r()\000]+);
LSQBRACKET = "[";
RSQBRACKET = "]";
DIGITORDOT = [0-9\.];
whitespace =  [ \t\n\r\v];
*/

PennScanner::PennScanner(std::istream *pIstr)
{
	pIn = pIstr;
	bot = tok = ptr = cur = pos = lim = top = eof = 0;
}

PennScanner::~PennScanner()
{
	if (bot) {
		delete[] bot;
	}
}

void PennScanner::addToken(void)
{
	yylval->setString(new std::string);
	const unsigned char *p;
	for (p = tok;
	     p != cur;
	     ++p) {
	     yylval->pString->append(1, *p);
	}
	// std::cerr << "UP202: addToken: '" << *(yylval->pString) << "'" << std::endl;
}

unsigned char *PennScanner::fill(unsigned char *cursor)
{
        if(!eof)
        {
                size_t cnt = tok - bot;
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
                        unsigned char *buf = new unsigned char[(lim - bot) + BSIZE];
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
                pIn->read((char *) lim, BSIZE);
                if ((cnt = pIn->gcount()) != BSIZE )
                {
                        eof = &lim[cnt]; *eof++ = '\0';
                }
                lim += cnt;
        }
        return cursor;
}


int PennScanner::scan(PennToken *pYYLVAL)
{
	yylval = pYYLVAL;
	

 scan:	
	tok = cur;

/*!re2c
"%%"      { goto comment; }
whitespace { goto scan; }
LSQBRACKET DIGITORDOT+ RSQBRACKET { goto scan; }
pos       { addToken();
            TOKEN_RETURN("word/function", T_NAME);
          }
"("       { TOKEN_RETURN("LPAREN", T_KEY_LPAREN); }
")"       { TOKEN_RETURN("RPAREN", T_KEY_RPAREN); }
zero      { if (cur >= eof) { goto end; } }
any       { TOKEN_RETURN("character", *cur); }
 */

 comment:
/*!re2c
"\n"          { goto scan; }
zero          { if (cur >= eof) { goto end; } }
any\[\n]      { tok = cur; goto comment; }
 */

 end:
 	return 0;
}
