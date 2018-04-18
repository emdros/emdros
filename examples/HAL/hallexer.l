%{

/*
 * hallexer.l
 *
 * Created: Sometime in the first half of 2003
 * Last update: 7/25-2003
 *
 *******************************************************************/
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2003  Ulrik Petersen
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
#include <hal_execution_environment.h>
#include <hal_defines.h>


%}

%option c++ 
%option prefix="hh"
%option yylineno
/* %option case-insensitive */
%option noyywrap
%option 8bit

/*
%x STRINGDQ
%x STRINGSQ
%x COMMENT_SLASH_STAR
%x COMMENT_SLASH_SLASH

*/


%%

[\(\"\'\)]+                          /* remove */
[A-Za-z\xa6\xa8\xb4\xb8\xbc-\xbe\xc0-\xd6\xd8-\xf6\xf8-\xff\(\"\'][A-Za-z\xa6\xa8\xb4\xb8\xbc-\xbe\xc0-\xd6\xd8-\xf6\xf8-\xff\-]*-[0-9]+[\)\"\']?         { pHEE->set_current_word(yytext); return TT_WORD; }
[A-Za-z\xa6\xa8\xb4\xb8\xbc-\xbe\xc0-\xd6\xd8-\xf6\xf8-\xff\(\"\'][A-Za-z\xa6\xa8\xb4\xb8\xbc-\xbe\xc0-\xd6\xd8-\xf6\xf8-\xff\-\(\)\"\']*         { pHEE->set_current_word(yytext); return TT_WORD; }
\-                          /* remove */
[\.\!\?]                { pHEE->set_current_punctuation(yytext); return TT_END_OF_SENTENCE_PUNCTUATION; }
[0-9]                       { pHEE->set_current_word(yytext); return TT_NUMBER; }
[0-9][0-9\.\,]*[0-9]        { /* NOTE: When changing this definition, 
                                 also change HAL_Execution_environment::is_number(). */
                                  pHEE->set_current_word(yytext); return TT_NUMBER; }
[\(\)\'\"\,\;\:\/\&\%\*\[\]~\+_§\#\$]                /* remove */
[ \t]+                      /* remove */
[\x80-\xa5\xa6\xa9-\xb3\xb5-\xb7\xb9-\xbb\xbf\xd7\xf7]+               /* remove */
\n                          /* remove */




%%

