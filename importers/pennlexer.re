/*
 * pennlexer.lxx
 *
 * Penn Treebank lexer
 *
 * Ulrik Petersen
 * Created: 2/17-2006
 * Last update: 6/8-2018
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2006-2018  Ulrik Sandborg-Petersen
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
                std::streamsize cnt = tok - bot;
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
