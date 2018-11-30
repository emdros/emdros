/*
 * mql_gq.cpp
 *
 * MQL "Get Query from stream" scanner
 *
 * Ulrik Petersen
 * Created: 6/23-2007
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
