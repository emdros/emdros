/*
 * hallexer.re
 *
 * Created: 6/23-2007
 * Last update: 6/23-2007
 *
 *******************************************************************/
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2007  Ulrik Petersen
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, license version 2.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
 **************************************************************************/


#define HAL_LEXER


#include <string>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "hal_execution_environment.h"
#include "hal_defines.h"
#include "hallexer.h"

#define BSIZE 8192

#define YYCTYPE         unsigned char
#define YYCURSOR        cur
#define YYLIMIT         lim
#define YYMARKER        ptr
#define YYFILL(n)       {cur = fill(cur);}

/*!re2c
zero     = "\000";
any      = [\001-\377];
*/

HALScanner::HALScanner(std::istream *pIstr)
{
	pIn = pIstr;
	bot = tok = ptr = cur = pos = lim = top = eof = 0;
}

HALScanner::~HALScanner()
{
	if (bot) {
		delete[] bot;
	}
}

void HALScanner::addWord(void)
{
	std::string result;
	const char *p;
	for (p = tok;
	     p != cur;
	     ++p) {
	     result.append(1, *p);
	}
	pHEE->set_current_word(result);
}

char *HALScanner::fill(char *cursor)
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


int HALScanner::scan(HAL_Execution_environment *pHee)
{
	pHEE = pHee;
	

 scan:	
	tok = cur;

/*!re2c
[("']      { goto scan; }
[A-Za-z\xa6\xa8\xb4\xb8\xbc-\xbe\xc0-\xd6\xd8-\xf6\xf8-\xff\(\"\'][A-Za-z\xa6\xa8\xb4\xb8\xbc-\xbe\xc0-\xd6\xd8-\xf6\xf8-\xff\-]*[-}[0-9]+[\)\"\']?         { addWord(); return TT_WORD; }
[A-Za-z\xa6\xa8\xb4\xb8\xbc-\xbe\xc0-\xd6\xd8-\xf6\xf8-\xff\(\"\'][A-Za-z\xa6\xa8\xb4\xb8\xbc-\xbe\xc0-\xd6\xd8-\xf6\xf8-\xff\-\(\)\"\']*         { addWord(); return TT_WORD; }
[-]                     { goto scan; }
[\.\!\?]                { std::string punct;
			  punct.append(1, *cur);
			  pHEE->set_current_punctuation(punct); 
			  return TT_END_OF_SENTENCE_PUNCTUATION; 
			}
[0-9]                   { addWord(); return TT_NUMBER; }
[0-9][0-9\.\,]*[0-9]        { /* NOTE: When changing this definition, 
                                 also change HAL_Execution_environment::is_number(). */
                                  addWord(); return TT_NUMBER; }
[\(\)\'\"\,\;\:\/\&\%\*\[\]~\+_§\#\$]     { goto scan; }
[ \t\n]             { goto scan; }
[\x80-\xa5\xa6\xa9-\xb3\xb5-\xb7\xb9-\xbb\xbf\xd7\xf7] { goto scan; }
zero      { if (cur == eof) { goto end; } }
any       { goto scan; }
 */

 end:
 	return 0;
}
