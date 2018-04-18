/*
 * mql_gq.cpp
 *
 * MQL "Get Query from stream" scanner
 *
 * Ulrik Petersen
 * Created: 6/23-2007
 * Last update: 4/29-2013
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2013  Ulrik Sandborg-Petersen
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

/**@file mql_gq.re
 *@brief Implementation of MQLGQScanner (re-based) (MQL layer)
 */


#include <mql_gq.h>

#ifdef BSIZE
#undef BSIZE
#endif

#define BSIZE (1024*16)

#undef YYCTYPE
#undef YYCURSOR
#undef YYLIMIT
#undef YYMARKER


#define YYCTYPE         unsigned char
#define YYCURSOR        cursor
#define YYLIMIT         lim
#define YYMARKER        ptr
#define YYFILL(n)       {cursor = fill(cursor);}

/*!re2c
zero     = "\000";
any      = [\001-\377];
dot      = any\[\n];
Q        = [Qq];
U        = [Uu];
I        = [Ii];
T        = [Tt];
G        = [Gg];
O        = [Oo];
*/


MQLGQScanner::MQLGQScanner(std::istream *pIstr)
{
	pIn = pIstr;
	pBigstring = 0;
	bot = tok = ptr = cur = pos = lim = top = eof = 0;
	bufbase1 = new char[BSIZE*2];
	bufbase2 = new char[BSIZE*2];
	bCurIsBufbase1 = false;
}

MQLGQScanner::~MQLGQScanner()
{
	delete pBigstring;
	delete [] bufbase1;
	delete [] bufbase2;
}

void MQLGQScanner::addToken(void)
{
        /* Don't add "+ 1" because cur points 
           one beyond the end of the token. */
	pBigstring->addChars((const char*) tok, cur - tok);
/*
	char *p;
	for (p = tok;
	     p != cur;
	     ++p) {
		pBigstring->addChar(*p);
	}
*/
}

char *MQLGQScanner::fill(char *cursor)
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
                        char *buf = (bCurIsBufbase1) ? (bufbase2) : (bufbase1);
			bCurIsBufbase1 = !bCurIsBufbase1;

			if (tok != 0) {
                           	memcpy(buf, tok, lim - tok);
                        }
                        tok = buf;
                        ptr = &buf[ptr - bot];
                        cursor = &buf[cursor - bot];
                        pos = &buf[pos - bot];
                        lim = &buf[lim - bot];
                        top = &lim[BSIZE];
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


Bigstring *MQLGQScanner::scan(void)
{
	pBigstring = new Bigstring();
	char *cursor = cur;

 scan:	
	tok = cursor;

/*!re2c
Q U I T   { cur = cursor; addToken(); pBigstring->addChars(" GO", 3); tok = cur; goto end; }
G O       { cur = cursor; /* addToken(); */ tok = cur; goto end; }
[a-zA-Z_][a-zA-Z0-9_]*    { cur = cursor; addToken(); goto scan; }
"/*"      { goto comment; }
"//"      { goto cppcomment; } 
'"'       { pBigstring->addChar('\"');
	    cur = cursor;
	    goto stringdq;
          }
"'"       { pBigstring->addChar('\'');
	    cur = cursor;
	    goto stringsq;
          }
zero      { if (cursor == eof) { cur = cursor; goto end; } }
"\n"      { pBigstring->addChar('\n'); goto scan; }
dot       { pBigstring->addChar(*tok); goto scan; }
 */

 comment:
        tok = cursor;
/*!re2c
"*/"          {  goto scan; }
"\n"          { pBigstring->addChar('\n'); goto comment; }
zero          { if (cursor == eof) { goto end; } }
dot           { tok = cur; goto comment; }
 */

 cppcomment:
        tok = cursor;
/*!re2c
"\n"            { pBigstring->addChar('\n'); goto scan; }	  
zero            { if (cursor == eof) { goto end; } }
dot             { tok = cur; goto cppcomment; }
 */

stringdq:
	tok = cursor;
/*!re2c
'"'           { pBigstring->addChar(*tok);
  	        cur = cursor;
	        goto scan;
              }
[\\][ntvbarf\\'\"\?]  { cur = cursor; 
		        pBigstring->addChars(tok, 2);
		      goto stringdq;
                    }
[\\][0-3][0-7][0-7] { cur = cursor; 
		      pBigstring->addChars(tok, 4);
		    goto stringdq; 
                  }
[\\]"x"[A-Fa-f0-9][A-Fa-f0-9]    { cur = cursor;
		      		   pBigstring->addChars(tok, 4);
			       goto stringdq;
                             }
"\n"            { pBigstring->addChar('\n'); cur = cursor; goto stringdq; }	  
zero            { if (cursor == eof) { goto end; } }
dot             { pBigstring->addChar(*tok); cur = cursor; goto stringdq; }
*/

stringsq:
	tok = cursor;
/*!re2c
"'"           { pBigstring->addChar('\'');
  	        cur = cursor;
	        goto scan;
              }
"\n"            { pBigstring->addChar('\n'); cur = cursor; goto stringsq; }	  
zero            { if (cursor == eof) { goto end; } }
dot             { pBigstring->addChar(*tok); cur = cursor; goto stringsq; }
*/


	
 end:
        Bigstring *result = pBigstring;
        pBigstring = 0;
	return result;
}



/*
int main(int argc, char *argv[])
{
      MQLGQScanner sc(&std::cin);
      while (!sc.noMoreInput()) {
           Bigstring *pBigstring = sc.scan();
           std::string result;
           pBigstring->toString(result);
           std::cerr << "UP200: '" << result << "'" << std::endl;
           delete pBigstring;
      }
}

*/
