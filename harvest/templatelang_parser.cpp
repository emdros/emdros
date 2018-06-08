/*
** 2000-05-29
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** Driver template for the LEMON parser generator.
**
** The "lemon" program processes an LALR(1) input grammar file, then uses
** this template to construct a parser.  The "lemon" program inserts text
** at each "%%" line.  Also, any "P-a-r-s-e" identifer prefix (without the
** interstitial "-" characters) contained in this template is changed into
** the value of the %name directive from the grammar.  Otherwise, the content
** of this template is copied straight through into the generate parser
** source file.
**
** The following is the concatenation of all %include directives from the
** input grammar file:
*/
#include <stdio.h>
/************ Begin %include sections from the grammar ************************/
#line 1 "./templatelang.yxx"


/*
 * templatelang.yxx
 *
 * Template language parser
 *
 * Ulrik Sandborg-Petersen
 * Created: 7/28-2008
 * Last update: 6/8-2018
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2008-2018  Ulrik Sandborg-Petersen
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


#include <templatelang_classes.h>
#include <mql_yylex.h>
#include <cstdlib>
#include <string>
#include <cassert>

#undef TOKEN

#line 125 "./templatelang.c"
/**************** End of %include directives **********************************/
/* These constants specify the various numeric values for terminal symbols
** in a format understandable to "makeheaders".  This section is blank unless
** "lemon" is run with the "-m" command-line option.
***************** Begin makeheaders token definitions *************************/
/**************** End makeheaders token definitions ***************************/

/* The next sections is a series of control #defines.
** various aspects of the generated parser.
**    TMPLGYYCODETYPE         is the data type used to store the integer codes
**                       that represent terminal and non-terminal symbols.
**                       "unsigned char" is used if there are fewer than
**                       256 symbols.  Larger types otherwise.
**    TMPLGYYNOCODE           is a number of type TMPLGYYCODETYPE that is not used for
**                       any terminal or nonterminal symbol.
**    TMPLGYYFALLBACK         If defined, this indicates that one or more tokens
**                       (also known as: "terminal symbols") have fall-back
**                       values which should be used if the original symbol
**                       would not parse.  This permits keywords to sometimes
**                       be used as identifiers, for example.
**    TMPLGYYACTIONTYPE       is the data type used for "action codes" - numbers
**                       that indicate what to do in response to the next
**                       token.
**    TemplateLangParserTOKENTYPE     is the data type used for minor type for terminal
**                       symbols.  Background: A "minor type" is a semantic
**                       value associated with a terminal or non-terminal
**                       symbols.  For example, for an "ID" terminal symbol,
**                       the minor type might be the name of the identifier.
**                       Each non-terminal can have a different minor type.
**                       Terminal symbols all have the same minor type, though.
**                       This macros defines the minor type for terminal 
**                       symbols.
**    TMPLGYYMINORTYPE        is the data type used for all minor types.
**                       This is typically a union of many types, one of
**                       which is TemplateLangParserTOKENTYPE.  The entry in the union
**                       for terminal symbols is called "tmplgyy0".
**    TMPLGYYSTACKDEPTH       is the maximum depth of the parser's stack.  If
**                       zero the stack is dynamically sized using realloc()
**    TemplateLangParserARG_SDECL     A static variable declaration for the %extra_argument
**    TemplateLangParserARG_PDECL     A parameter declaration for the %extra_argument
**    TemplateLangParserARG_STORE     Code to store %extra_argument into tmplgyypParser
**    TemplateLangParserARG_FETCH     Code to extract %extra_argument from tmplgyypParser
**    TMPLGYYERRORSYMBOL      is the code number of the error symbol.  If not
**                       defined, then do no error processing.
**    TMPLGYYNSTATE           the combined number of states.
**    TMPLGYYNRULE            the number of rules in the grammar
**    TMPLGYY_MAX_SHIFT       Maximum value for shift actions
**    TMPLGYY_MIN_SHIFTREDUCE Minimum value for shift-reduce actions
**    TMPLGYY_MAX_SHIFTREDUCE Maximum value for shift-reduce actions
**    TMPLGYY_MIN_REDUCE      Maximum value for reduce actions
**    TMPLGYY_ERROR_ACTION    The tmplgyy_action[] code for syntax error
**    TMPLGYY_ACCEPT_ACTION   The tmplgyy_action[] code for accept
**    TMPLGYY_NO_ACTION       The tmplgyy_action[] code for no-op
*/
#ifndef INTERFACE
# define INTERFACE 1
#endif
/************* Begin control #defines *****************************************/
#define TMPLGYYCODETYPE unsigned char
#define TMPLGYYNOCODE 80
#define TMPLGYYACTIONTYPE unsigned short int
#define TemplateLangParserTOKENTYPE Token*
typedef union {
  int tmplgyyinit;
  TemplateLangParserTOKENTYPE tmplgyy0;
  TemplateSetVarNow* tmplgyy2;
  TemplateIfFeatureEqual* tmplgyy8;
  TemplateString* tmplgyy14;
  TemplateNamedSetRemoveBegin* tmplgyy15;
  TemplateIf* tmplgyy24;
  TemplateNamedSetAddBegin* tmplgyy30;
  TemplateIfAttribEqual* tmplgyy32;
  TemplateDictlookupFeature* tmplgyy33;
  TemplateIfHasAttrib* tmplgyy52;
  TemplateNamedSetAddEnd* tmplgyy56;
  TemplateIfVarEqual* tmplgyy67;
  TemplateListAppendBegin* tmplgyy72;
  TemplateEmitVar* tmplgyy75;
  TemplateDictlookupCounter* tmplgyy79;
  TemplateIfListEmpty* tmplgyy81;
  TemplateSetVarBegin* tmplgyy83;
  TemplateSetVarEnd* tmplgyy84;
  TemplateListAppendEnd* tmplgyy102;
  TemplateIfNamedSetEmpty* tmplgyy120;
  TemplateNamedSetRemoveEnd* tmplgyy123;
  TemplateASTNode* tmplgyy127;
  TemplateDictlookupVar* tmplgyy130;
  TemplateNixing* tmplgyy136;
} TMPLGYYMINORTYPE;
#ifndef TMPLGYYSTACKDEPTH
#define TMPLGYYSTACKDEPTH 100
#endif
#define TemplateLangParserARG_SDECL TemplateLangExecEnv *pEE;
#define TemplateLangParserARG_PDECL ,TemplateLangExecEnv *pEE
#define TemplateLangParserARG_FETCH TemplateLangExecEnv *pEE = tmplgyypParser->pEE
#define TemplateLangParserARG_STORE tmplgyypParser->pEE = pEE
#define TMPLGYYNSTATE             150
#define TMPLGYYNRULE              89
#define TMPLGYY_MAX_SHIFT         149
#define TMPLGYY_MIN_SHIFTREDUCE   238
#define TMPLGYY_MAX_SHIFTREDUCE   326
#define TMPLGYY_MIN_REDUCE        327
#define TMPLGYY_MAX_REDUCE        415
#define TMPLGYY_ERROR_ACTION      416
#define TMPLGYY_ACCEPT_ACTION     417
#define TMPLGYY_NO_ACTION         418
/************* End control #defines *******************************************/

/* Define the tmplgyytestcase() macro to be a no-op if is not already defined
** otherwise.
**
** Applications can choose to define tmplgyytestcase() in the %include section
** to a macro that can assist in verifying code coverage.  For production
** code the tmplgyytestcase() macro should be turned off.  But it is useful
** for testing.
*/
#ifndef tmplgyytestcase
# define tmplgyytestcase(X)
#endif


/* Next are the tables used to determine what action to take based on the
** current state and lookahead token.  These tables are used to implement
** functions that take a state number and lookahead value and return an
** action integer.  
**
** Suppose the action integer is N.  Then the action is determined as
** follows
**
**   0 <= N <= TMPLGYY_MAX_SHIFT             Shift N.  That is, push the lookahead
**                                      token onto the stack and goto state N.
**
**   N between TMPLGYY_MIN_SHIFTREDUCE       Shift to an arbitrary state then
**     and TMPLGYY_MAX_SHIFTREDUCE           reduce by rule N-TMPLGYY_MIN_SHIFTREDUCE.
**
**   N between TMPLGYY_MIN_REDUCE            Reduce by rule N-TMPLGYY_MIN_REDUCE
**     and TMPLGYY_MAX_REDUCE
**
**   N == TMPLGYY_ERROR_ACTION               A syntax error has occurred.
**
**   N == TMPLGYY_ACCEPT_ACTION              The parser accepts its input.
**
**   N == TMPLGYY_NO_ACTION                  No such action.  Denotes unused
**                                      slots in the tmplgyy_action[] table.
**
** The action table is constructed as a single large table named tmplgyy_action[].
** Given state S and lookahead X, the action is computed as either:
**
**    (A)   N = tmplgyy_action[ tmplgyy_shift_ofst[S] + X ]
**    (B)   N = tmplgyy_default[S]
**
** The (A) formula is preferred.  The B formula is used instead if:
**    (1)  The tmplgyy_shift_ofst[S]+X value is out of range, or
**    (2)  tmplgyy_lookahead[tmplgyy_shift_ofst[S]+X] is not equal to X, or
**    (3)  tmplgyy_shift_ofst[S] equal TMPLGYY_SHIFT_USE_DFLT.
** (Implementation note: TMPLGYY_SHIFT_USE_DFLT is chosen so that
** TMPLGYY_SHIFT_USE_DFLT+X will be out of range for all possible lookaheads X.
** Hence only tests (1) and (2) need to be evaluated.)
**
** The formulas above are for computing the action when the lookahead is
** a terminal symbol.  If the lookahead is a non-terminal (as occurs after
** a reduce action) then the tmplgyy_reduce_ofst[] array is used in place of
** the tmplgyy_shift_ofst[] array and TMPLGYY_REDUCE_USE_DFLT is used in place of
** TMPLGYY_SHIFT_USE_DFLT.
**
** The following are the tables generated in this section:
**
**  tmplgyy_action[]        A single table containing all actions.
**  tmplgyy_lookahead[]     A table containing the lookahead for each entry in
**                     tmplgyy_action.  Used to detect hash collisions.
**  tmplgyy_shift_ofst[]    For each state, the offset into tmplgyy_action for
**                     shifting terminals.
**  tmplgyy_reduce_ofst[]   For each state, the offset into tmplgyy_action for
**                     shifting non-terminals after a reduce.
**  tmplgyy_default[]       Default action for each state.
**
*********** Begin parsing tables **********************************************/
#define TMPLGYY_ACTTAB_COUNT (585)
static const TMPLGYYACTIONTYPE tmplgyy_action[] = {
 /*     0 */   243,  244,  149,  325,  323,   18,  146,  143,  248,  140,
 /*    10 */   316,  137,  134,  133,  324,  320,  127,  321,  120,  114,
 /*    20 */   108,  103,  100,   95,   92,  318,  319,  322,  317,  285,
 /*    30 */   286,  287,   64,   59,   52,   49,   46,   43,  301,    3,
 /*    40 */    38,  306,   34,  308,   29,  310,   24,  313,   20,  315,
 /*    50 */   326,  243,  244,  149,  325,  323,   19,  146,  143,  248,
 /*    60 */   140,  316,  137,  134,  133,  324,  320,  127,  321,  120,
 /*    70 */   114,  108,  103,  100,   95,   92,  318,  319,  322,  317,
 /*    80 */   285,  286,  287,   64,   59,   52,   49,   46,   43,  314,
 /*    90 */     2,   38,  306,   34,  308,   29,  310,   24,  313,   20,
 /*   100 */   315,  326,  243,  244,  149,  325,  323,   21,  146,  143,
 /*   110 */   248,  140,  316,  137,  134,  133,  324,  320,  127,  321,
 /*   120 */   120,  114,  108,  103,  100,   95,   92,  318,  319,  322,
 /*   130 */   317,  285,  286,  287,   64,   59,   52,   49,   46,   43,
 /*   140 */   303,  311,   38,  306,   34,  308,   29,  310,   24,  313,
 /*   150 */    20,  315,  326,  243,  244,  149,  325,  323,    9,  146,
 /*   160 */   143,  248,  140,  316,  137,  134,  133,  324,  320,  127,
 /*   170 */   321,  120,  114,  108,  103,  100,   95,   92,  318,  319,
 /*   180 */   322,  317,  285,  286,  287,   64,   59,   52,   49,   46,
 /*   190 */    43,  302,   23,   38,  306,   34,  308,   29,  310,   24,
 /*   200 */   313,   20,  315,  326,  327,  243,  244,  149,  325,  323,
 /*   210 */    25,  146,  143,  248,  140,  316,  137,  134,  133,  324,
 /*   220 */   320,  127,  321,  120,  114,  108,  103,  100,   95,   92,
 /*   230 */   318,  319,  322,  317,  285,  286,  287,   64,   59,   52,
 /*   240 */    49,   46,   43,  309,   26,   38,  306,   34,  308,   29,
 /*   250 */   310,   24,  313,   20,  315,  326,  243,  244,  149,  325,
 /*   260 */   323,   27,  146,  143,  248,  140,  316,  137,  134,  133,
 /*   270 */   324,  320,  127,  321,  120,  114,  108,  103,  100,   95,
 /*   280 */    92,  318,  319,  322,  317,  285,  286,  287,   64,   59,
 /*   290 */    52,   49,   46,   43,   28,  307,   38,  306,   34,  308,
 /*   300 */    29,  310,   24,  313,   20,  315,  326,  417,    7,  239,
 /*   310 */   241,  242,  266,  267,  268,  269,  270,  271,  272,  273,
 /*   320 */   274,  275,  276,  277,  278,  279,  295,  296,  297,  298,
 /*   330 */   300,  299,    1,    4,  239,  241,  242,  266,  267,  268,
 /*   340 */   269,  270,  271,  272,  273,  274,  275,  276,  277,  278,
 /*   350 */   279,  295,  296,  297,  298,  300,  299,    1,    5,  239,
 /*   360 */   241,  242,  266,  267,  268,  269,  270,  271,  272,  273,
 /*   370 */   274,  275,  276,  277,  278,  279,  295,  296,  297,  298,
 /*   380 */   300,  299,    1,    6,  239,  241,  242,  266,  267,  268,
 /*   390 */   269,  270,  271,  272,  273,  274,  275,  276,  277,  278,
 /*   400 */   279,  295,  296,  297,  298,  300,  299,    1,  240,  241,
 /*   410 */   242,  266,  267,  268,  269,  270,  271,  272,  273,  274,
 /*   420 */   275,  276,  277,  278,  279,  295,  296,  297,  298,  300,
 /*   430 */   299,    1,   79,   22,   36,  312,  304,  112,  118,   30,
 /*   440 */    89,   84,   74,   69,  124,   12,   56,  259,   31,   55,
 /*   450 */   111,  117,   14,   32,  256,   33,  131,  123,  252,  305,
 /*   460 */    35,   10,   37,  294,   39,   40,   41,   42,  293,   45,
 /*   470 */    44,   47,  292,   48,  291,   50,   51,  290,   54,   53,
 /*   480 */    11,  289,   57,   58,  288,   60,   61,  284,   62,   63,
 /*   490 */    66,   68,   65,   71,  283,   67,   73,  282,   76,   70,
 /*   500 */    78,  329,  329,  281,   81,   72,   82,  329,   75,   83,
 /*   510 */    77,   86,  280,   80,   85,   87,   88,    8,   91,   90,
 /*   520 */    94,  265,   97,  264,   93,   99,   96,  102,  263,  329,
 /*   530 */   262,  105,  107,  329,   98,  101,  110,  261,  329,  260,
 /*   540 */   104,  106,  113,  258,  109,  116,  257,  119,  329,  122,
 /*   550 */    13,  115,  255,  254,   16,  126,  329,   15,  253,  121,
 /*   560 */   129,  251,  329,  125,  132,  250,  136,  329,  128,  329,
 /*   570 */   130,  249,  135,   17,  141,  138,  139,  247,  142,  246,
 /*   580 */   144,  145,  245,  147,  148,
};
static const TMPLGYYCODETYPE tmplgyy_lookahead[] = {
 /*     0 */     1,    2,    3,    4,    5,   15,    7,    8,    9,   10,
 /*    10 */    11,   12,   13,   14,   15,   16,   17,   18,   19,   20,
 /*    20 */    21,   22,   23,   24,   25,   26,   27,   28,   29,   30,
 /*    30 */    31,   32,   33,   34,   35,   36,   37,   38,   39,   40,
 /*    40 */    41,   42,   43,   44,   45,   46,   47,   48,   49,   50,
 /*    50 */    51,    1,    2,    3,    4,    5,    4,    7,    8,    9,
 /*    60 */    10,   11,   12,   13,   14,   15,   16,   17,   18,   19,
 /*    70 */    20,   21,   22,   23,   24,   25,   26,   27,   28,   29,
 /*    80 */    30,   31,   32,   33,   34,   35,   36,   37,   38,    6,
 /*    90 */    40,   41,   42,   43,   44,   45,   46,   47,   48,   49,
 /*   100 */    50,   51,    1,    2,    3,    4,    5,   15,    7,    8,
 /*   110 */     9,   10,   11,   12,   13,   14,   15,   16,   17,   18,
 /*   120 */    19,   20,   21,   22,   23,   24,   25,   26,   27,   28,
 /*   130 */    29,   30,   31,   32,   33,   34,   35,   36,   37,   38,
 /*   140 */    39,    6,   41,   42,   43,   44,   45,   46,   47,   48,
 /*   150 */    49,   50,   51,    1,    2,    3,    4,    5,   15,    7,
 /*   160 */     8,    9,   10,   11,   12,   13,   14,   15,   16,   17,
 /*   170 */    18,   19,   20,   21,   22,   23,   24,   25,   26,   27,
 /*   180 */    28,   29,   30,   31,   32,   33,   34,   35,   36,   37,
 /*   190 */    38,   39,    4,   41,   42,   43,   44,   45,   46,   47,
 /*   200 */    48,   49,   50,   51,    0,    1,    2,    3,    4,    5,
 /*   210 */    15,    7,    8,    9,   10,   11,   12,   13,   14,   15,
 /*   220 */    16,   17,   18,   19,   20,   21,   22,   23,   24,   25,
 /*   230 */    26,   27,   28,   29,   30,   31,   32,   33,   34,   35,
 /*   240 */    36,   37,   38,    6,    4,   41,   42,   43,   44,   45,
 /*   250 */    46,   47,   48,   49,   50,   51,    1,    2,    3,    4,
 /*   260 */     5,   15,    7,    8,    9,   10,   11,   12,   13,   14,
 /*   270 */    15,   16,   17,   18,   19,   20,   21,   22,   23,   24,
 /*   280 */    25,   26,   27,   28,   29,   30,   31,   32,   33,   34,
 /*   290 */    35,   36,   37,   38,    4,    6,   41,   42,   43,   44,
 /*   300 */    45,   46,   47,   48,   49,   50,   51,   53,   54,   55,
 /*   310 */    56,   57,   58,   59,   60,   61,   62,   63,   64,   65,
 /*   320 */    66,   67,   68,   69,   70,   71,   72,   73,   74,   75,
 /*   330 */    76,   77,   78,   54,   55,   56,   57,   58,   59,   60,
 /*   340 */    61,   62,   63,   64,   65,   66,   67,   68,   69,   70,
 /*   350 */    71,   72,   73,   74,   75,   76,   77,   78,   54,   55,
 /*   360 */    56,   57,   58,   59,   60,   61,   62,   63,   64,   65,
 /*   370 */    66,   67,   68,   69,   70,   71,   72,   73,   74,   75,
 /*   380 */    76,   77,   78,   54,   55,   56,   57,   58,   59,   60,
 /*   390 */    61,   62,   63,   64,   65,   66,   67,   68,   69,   70,
 /*   400 */    71,   72,   73,   74,   75,   76,   77,   78,   55,   56,
 /*   410 */    57,   58,   59,   60,   61,   62,   63,   64,   65,   66,
 /*   420 */    67,   68,   69,   70,   71,   72,   73,   74,   75,   76,
 /*   430 */    77,   78,   18,    4,    4,    6,    6,    5,    5,   15,
 /*   440 */    26,   27,   28,   29,    5,    4,   15,    6,    4,   18,
 /*   450 */    18,   18,    4,   15,    6,    4,    4,   18,    6,    6,
 /*   460 */    15,   15,    4,    6,   15,    4,   11,    4,    6,    4,
 /*   470 */    11,   15,    6,    4,    6,   15,    4,    6,    4,   15,
 /*   480 */     4,    6,   15,    4,    6,   15,    4,    6,    5,    4,
 /*   490 */     4,    4,   15,    4,    6,   15,    4,    6,    4,   15,
 /*   500 */     4,   79,   79,    6,    4,   15,    5,   79,   15,    4,
 /*   510 */    15,    4,    6,   15,   15,    5,    4,    4,    4,   15,
 /*   520 */     4,    6,    4,    6,   15,    4,   15,    4,    6,   79,
 /*   530 */     6,    4,    4,   79,   15,   15,    4,    6,   79,    6,
 /*   540 */    15,   15,    4,    6,   15,    4,    6,    4,   79,    4,
 /*   550 */    15,   15,    6,    6,    4,    4,   79,   15,    6,   15,
 /*   560 */     4,    6,   79,   15,    4,    6,    4,   79,   15,   79,
 /*   570 */    16,    6,   11,   15,    5,   11,    4,    6,    4,    6,
 /*   580 */     5,    4,    6,    5,    4,
};
#define TMPLGYY_SHIFT_USE_DFLT (585)
#define TMPLGYY_SHIFT_COUNT    (149)
#define TMPLGYY_SHIFT_MIN      (-10)
#define TMPLGYY_SHIFT_MAX      (580)
static const short tmplgyy_shift_ofst[] = {
 /*     0 */   255,   50,  255,  255,   -1,  101,  152,  204,  414,  429,
 /*    10 */   430,  431,  432,  441,  433,  448,  439,  452,   83,  -10,
 /*    20 */    52,  135,   92,  143,  188,  237,  195,  240,  246,  290,
 /*    30 */   289,  424,  444,  438,  451,  453,  445,  446,  458,  457,
 /*    40 */   449,  461,  455,  463,  462,  459,  465,  466,  456,  469,
 /*    50 */   468,  460,  472,  471,  464,  474,  475,  476,  467,  479,
 /*    60 */   478,  470,  482,  483,  485,  481,  477,  486,  480,  487,
 /*    70 */   488,  484,  489,  490,  492,  491,  493,  494,  495,  496,
 /*    80 */   497,  498,  500,  501,  505,  506,  499,  507,  510,  512,
 /*    90 */   513,  504,  514,  515,  509,  516,  517,  511,  518,  519,
 /*   100 */   521,  522,  520,  523,  524,  525,  527,  526,  528,  531,
 /*   110 */   529,  532,  533,  535,  538,  537,  536,  541,  540,  542,
 /*   120 */   543,  546,  544,  545,  547,  550,  548,  551,  552,  553,
 /*   130 */   556,  554,  558,  560,  555,  559,  561,  562,  565,  564,
 /*   140 */   572,  571,  569,  574,  573,  575,  577,  576,  578,  580,
};
#define TMPLGYY_REDUCE_USE_DFLT (-1)
#define TMPLGYY_REDUCE_COUNT (7)
#define TMPLGYY_REDUCE_MIN   (0)
#define TMPLGYY_REDUCE_MAX   (353)
static const short tmplgyy_reduce_ofst[] = {
 /*     0 */   254,  279,  304,  329,  353,  353,  353,  353,
};
static const TMPLGYYACTIONTYPE tmplgyy_default[] = {
 /*     0 */   416,  416,  416,  416,  416,  416,  416,  416,  416,  416,
 /*    10 */   416,  416,  416,  416,  416,  416,  416,  416,  416,  416,
 /*    20 */   416,  416,  416,  416,  416,  416,  416,  416,  416,  416,
 /*    30 */   416,  416,  416,  416,  416,  416,  416,  416,  416,  416,
 /*    40 */   416,  416,  416,  416,  416,  416,  416,  416,  416,  416,
 /*    50 */   416,  416,  416,  416,  416,  416,  416,  416,  416,  416,
 /*    60 */   416,  416,  416,  416,  416,  416,  416,  416,  416,  416,
 /*    70 */   416,  416,  416,  416,  416,  416,  416,  416,  416,  416,
 /*    80 */   416,  416,  416,  416,  416,  416,  416,  416,  416,  416,
 /*    90 */   416,  416,  416,  416,  416,  416,  416,  416,  416,  416,
 /*   100 */   416,  416,  416,  416,  416,  416,  416,  416,  416,  416,
 /*   110 */   416,  416,  416,  416,  416,  416,  416,  416,  416,  416,
 /*   120 */   416,  416,  416,  416,  416,  416,  416,  416,  416,  416,
 /*   130 */   416,  416,  416,  416,  416,  416,  416,  416,  416,  416,
 /*   140 */   416,  416,  416,  416,  416,  416,  416,  416,  416,  416,
};
/********** End of lemon-generated parsing tables *****************************/

/* The next table maps tokens (terminal symbols) into fallback tokens.  
** If a construct like the following:
** 
**      %fallback ID X Y Z.
**
** appears in the grammar, then ID becomes a fallback token for X, Y,
** and Z.  Whenever one of the tokens X, Y, or Z is input to the parser
** but it does not parse, the type of the token is changed to ID and
** the parse is retried before an error is thrown.
**
** This feature can be used, for example, to cause some keywords in a language
** to revert to identifiers if they keyword does not apply in the context where
** it appears.
*/
#ifdef TMPLGYYFALLBACK
static const TMPLGYYCODETYPE tmplgyyFallback[] = {
};
#endif /* TMPLGYYFALLBACK */

/* The following structure represents a single element of the
** parser's stack.  Information stored includes:
**
**   +  The state number for the parser at this level of the stack.
**
**   +  The value of the token stored at this level of the stack.
**      (In other words, the "major" token.)
**
**   +  The semantic value stored at this level of the stack.  This is
**      the information used by the action routines in the grammar.
**      It is sometimes called the "minor" token.
**
** After the "shift" half of a SHIFTREDUCE action, the stateno field
** actually contains the reduce action for the second half of the
** SHIFTREDUCE.
*/
struct tmplgyyStackEntry {
  TMPLGYYACTIONTYPE stateno;  /* The state-number, or reduce action in SHIFTREDUCE */
  TMPLGYYCODETYPE major;      /* The major token value.  This is the code
                         ** number for the token at this stack level */
  TMPLGYYMINORTYPE minor;     /* The user-supplied minor token value.  This
                         ** is the value of the token  */
};
typedef struct tmplgyyStackEntry tmplgyyStackEntry;

/* The state of the parser is completely contained in an instance of
** the following structure */
struct tmplgyyParser {
  tmplgyyStackEntry *tmplgyytos;          /* Pointer to top element of the stack */
#ifdef TMPLGYYTRACKMAXSTACKDEPTH
  int tmplgyyhwm;                    /* High-water mark of the stack */
#endif
#ifndef TMPLGYYNOERRORRECOVERY
  int tmplgyyerrcnt;                 /* Shifts left before out of the error */
#endif
  TemplateLangParserARG_SDECL                /* A place to hold %extra_argument */
#if TMPLGYYSTACKDEPTH<=0
  int tmplgyystksz;                  /* Current side of the stack */
  tmplgyyStackEntry *tmplgyystack;        /* The parser's stack */
  tmplgyyStackEntry tmplgyystk0;          /* First stack entry */
#else
  tmplgyyStackEntry tmplgyystack[TMPLGYYSTACKDEPTH];  /* The parser's stack */
#endif
};
typedef struct tmplgyyParser tmplgyyParser;

#ifndef NDEBUG
#include <stdio.h>
static FILE *tmplgyyTraceFILE = 0;
static char *tmplgyyTracePrompt = 0;
#endif /* NDEBUG */

#ifndef NDEBUG
/* 
** Turn parser tracing on by giving a stream to which to write the trace
** and a prompt to preface each trace message.  Tracing is turned off
** by making either argument NULL 
**
** Inputs:
** <ul>
** <li> A FILE* to which trace output should be written.
**      If NULL, then tracing is turned off.
** <li> A prefix string written at the beginning of every
**      line of trace output.  If NULL, then tracing is
**      turned off.
** </ul>
**
** Outputs:
** None.
*/
void TemplateLangParserTrace(FILE *TraceFILE, char *zTracePrompt){
  tmplgyyTraceFILE = TraceFILE;
  tmplgyyTracePrompt = zTracePrompt;
  if( tmplgyyTraceFILE==0 ) tmplgyyTracePrompt = 0;
  else if( tmplgyyTracePrompt==0 ) tmplgyyTraceFILE = 0;
}
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing shifts, the names of all terminals and nonterminals
** are required.  The following table supplies these names */
static const char *const tmplgyyTokenName[] = { 
  "$",             "KEY_ID_D",      "KEY_FIRSTMONAD",  "KEY_FEATURE", 
  "KEY_SPACE",     "INTEGER",       "KEY_BB_CLOSE",  "KEY_FEATUREJSONMANGLE",
  "KEY_FEATURENOMANGLE",  "KEY_DBNAME",    "KEY_ATTRIB",    "IDENTIFIER",  
  "KEY_ATTRIB_NOMANGLE",  "KEY_COPYATTRIBS",  "KEY_COUNTER",   "STRING",      
  "KEY_BARE_FORMAT",  "KEY_SETCOUNTER",  "KEY_BARE_VAR",  "KEY_INCCOUNTER",
  "KEY_DECCOUNTER",  "KEY_LISTEMIT",  "KEY_LISTCLEAR",  "KEY_NAMEDSETEMIT",
  "KEY_NAMEDSETCLEAR",  "KEY_DICTLOOKUP",  "KEY_BARE_FEATURE",  "KEY_BARE_FEATURENOMANGLE",
  "KEY_BARE_VARNOMANGLE",  "KEY_BARE_COUNTER",  "KEY_SETNIXING_ALL",  "KEY_SETNIXING_DOC",
  "KEY_SETNIXING_OFF",  "KEY_IF_FEATUREEQUAL",  "KEY_IF_VAREQUAL",  "KEY_IF_LISTEMPTY",
  "KEY_IF_NAMEDSETEMPTY",  "KEY_IF_HASATTRIB",  "KEY_IF_ATTRIBEQUAL",  "KEY_ENDIF",   
  "KEY_ELSE",      "KEY_LISTAPPENDBEGIN",  "KEY_LISTAPPENDEND",  "KEY_NAMEDSETADD",
  "KEY_NAMEDSETADDEND",  "KEY_NAMEDSETREMOVE",  "KEY_NAMEDSETREMOVEEND",  "KEY_SETVAR",  
  "KEY_SETVAREND",  "KEY_EMITVAR",   "CHAR",          "KEY_NEWLINE", 
  "error",         "top_value",     "template_list",  "template_list_element",
  "template_string",  "template_if",   "template_listappend",  "template_listappend_end",
  "template_namedsetadd",  "template_namedsetadd_end",  "template_namedsetremove",  "template_namedsetremove_end",
  "template_setvarnow",  "template_setvar",  "template_setvar_end",  "template_emitvar",
  "template_setnixing",  "template_dictlookup_feature",  "template_dictlookup_var",  "template_dictlookup_counter",
  "template_if_featureequal",  "template_if_varequal",  "template_if_listempty",  "template_if_namedsetempty",
  "template_if_hasattrib",  "template_if_attribequal",  "template_some_if",
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *const tmplgyyRuleName[] = {
 /*   0 */ "top_value ::= template_list",
 /*   1 */ "template_list ::= template_list_element",
 /*   2 */ "template_list ::= template_list template_list_element",
 /*   3 */ "template_list_element ::= template_string",
 /*   4 */ "template_list_element ::= template_if",
 /*   5 */ "template_list_element ::= KEY_ID_D",
 /*   6 */ "template_list_element ::= KEY_FIRSTMONAD",
 /*   7 */ "template_list_element ::= KEY_FEATURE KEY_SPACE INTEGER KEY_BB_CLOSE",
 /*   8 */ "template_list_element ::= KEY_FEATUREJSONMANGLE KEY_SPACE INTEGER KEY_BB_CLOSE",
 /*   9 */ "template_list_element ::= KEY_FEATURENOMANGLE KEY_SPACE INTEGER KEY_BB_CLOSE",
 /*  10 */ "template_list_element ::= KEY_DBNAME",
 /*  11 */ "template_list_element ::= KEY_ATTRIB KEY_SPACE IDENTIFIER KEY_BB_CLOSE",
 /*  12 */ "template_list_element ::= KEY_ATTRIB_NOMANGLE KEY_SPACE IDENTIFIER KEY_BB_CLOSE",
 /*  13 */ "template_list_element ::= KEY_COPYATTRIBS KEY_BB_CLOSE",
 /*  14 */ "template_list_element ::= KEY_COUNTER KEY_SPACE STRING KEY_BB_CLOSE",
 /*  15 */ "template_list_element ::= KEY_COUNTER KEY_SPACE STRING KEY_SPACE KEY_BARE_FORMAT KEY_SPACE STRING KEY_BB_CLOSE",
 /*  16 */ "template_list_element ::= KEY_SETCOUNTER KEY_SPACE STRING KEY_SPACE INTEGER KEY_BB_CLOSE",
 /*  17 */ "template_list_element ::= KEY_SETCOUNTER KEY_SPACE STRING KEY_SPACE KEY_BARE_VAR KEY_SPACE STRING KEY_BB_CLOSE",
 /*  18 */ "template_list_element ::= KEY_INCCOUNTER KEY_SPACE STRING KEY_BB_CLOSE",
 /*  19 */ "template_list_element ::= KEY_INCCOUNTER KEY_SPACE STRING KEY_SPACE INTEGER KEY_BB_CLOSE",
 /*  20 */ "template_list_element ::= KEY_INCCOUNTER KEY_SPACE STRING KEY_SPACE KEY_BARE_VAR KEY_SPACE STRING KEY_BB_CLOSE",
 /*  21 */ "template_list_element ::= KEY_DECCOUNTER KEY_SPACE STRING KEY_BB_CLOSE",
 /*  22 */ "template_list_element ::= KEY_DECCOUNTER KEY_SPACE STRING KEY_SPACE INTEGER KEY_BB_CLOSE",
 /*  23 */ "template_list_element ::= KEY_DECCOUNTER KEY_SPACE STRING KEY_SPACE KEY_BARE_VAR KEY_SPACE STRING KEY_BB_CLOSE",
 /*  24 */ "template_list_element ::= KEY_LISTEMIT KEY_SPACE STRING KEY_SPACE STRING KEY_BB_CLOSE",
 /*  25 */ "template_list_element ::= KEY_LISTCLEAR KEY_SPACE STRING KEY_BB_CLOSE",
 /*  26 */ "template_list_element ::= KEY_NAMEDSETEMIT KEY_SPACE STRING KEY_SPACE STRING KEY_BB_CLOSE",
 /*  27 */ "template_list_element ::= KEY_NAMEDSETCLEAR KEY_SPACE STRING KEY_BB_CLOSE",
 /*  28 */ "template_list_element ::= template_listappend",
 /*  29 */ "template_list_element ::= template_listappend_end",
 /*  30 */ "template_list_element ::= template_namedsetadd",
 /*  31 */ "template_list_element ::= template_namedsetadd_end",
 /*  32 */ "template_list_element ::= template_namedsetremove",
 /*  33 */ "template_list_element ::= template_namedsetremove_end",
 /*  34 */ "template_list_element ::= template_setvarnow",
 /*  35 */ "template_list_element ::= template_setvar",
 /*  36 */ "template_list_element ::= template_setvar_end",
 /*  37 */ "template_list_element ::= template_emitvar",
 /*  38 */ "template_list_element ::= template_setnixing",
 /*  39 */ "template_list_element ::= template_dictlookup_feature",
 /*  40 */ "template_list_element ::= template_dictlookup_var",
 /*  41 */ "template_list_element ::= template_dictlookup_counter",
 /*  42 */ "template_dictlookup_feature ::= KEY_DICTLOOKUP KEY_SPACE STRING KEY_SPACE KEY_BARE_FEATURE KEY_SPACE INTEGER KEY_SPACE STRING KEY_BB_CLOSE",
 /*  43 */ "template_dictlookup_feature ::= KEY_DICTLOOKUP KEY_SPACE STRING KEY_SPACE KEY_BARE_FEATURENOMANGLE KEY_SPACE INTEGER KEY_SPACE STRING KEY_BB_CLOSE",
 /*  44 */ "template_dictlookup_var ::= KEY_DICTLOOKUP KEY_SPACE STRING KEY_SPACE KEY_BARE_VAR KEY_SPACE STRING KEY_SPACE STRING KEY_BB_CLOSE",
 /*  45 */ "template_dictlookup_var ::= KEY_DICTLOOKUP KEY_SPACE STRING KEY_SPACE KEY_BARE_VARNOMANGLE KEY_SPACE STRING KEY_SPACE STRING KEY_BB_CLOSE",
 /*  46 */ "template_dictlookup_counter ::= KEY_DICTLOOKUP KEY_SPACE STRING KEY_SPACE KEY_BARE_COUNTER KEY_SPACE STRING KEY_SPACE STRING KEY_BB_CLOSE",
 /*  47 */ "template_setnixing ::= KEY_SETNIXING_ALL",
 /*  48 */ "template_setnixing ::= KEY_SETNIXING_DOC",
 /*  49 */ "template_setnixing ::= KEY_SETNIXING_OFF",
 /*  50 */ "template_if_featureequal ::= KEY_IF_FEATUREEQUAL KEY_SPACE INTEGER KEY_SPACE STRING KEY_BB_CLOSE",
 /*  51 */ "template_if_varequal ::= KEY_IF_VAREQUAL KEY_SPACE STRING KEY_SPACE STRING KEY_BB_CLOSE",
 /*  52 */ "template_if_varequal ::= KEY_IF_VAREQUAL KEY_SPACE STRING KEY_SPACE KEY_BARE_VAR KEY_SPACE STRING KEY_BB_CLOSE",
 /*  53 */ "template_if_listempty ::= KEY_IF_LISTEMPTY KEY_SPACE STRING KEY_BB_CLOSE",
 /*  54 */ "template_if_namedsetempty ::= KEY_IF_NAMEDSETEMPTY KEY_SPACE STRING KEY_BB_CLOSE",
 /*  55 */ "template_if_hasattrib ::= KEY_IF_HASATTRIB KEY_SPACE IDENTIFIER KEY_BB_CLOSE",
 /*  56 */ "template_if_attribequal ::= KEY_IF_ATTRIBEQUAL KEY_SPACE IDENTIFIER KEY_SPACE STRING KEY_BB_CLOSE",
 /*  57 */ "template_some_if ::= template_if_featureequal",
 /*  58 */ "template_some_if ::= template_if_varequal",
 /*  59 */ "template_some_if ::= template_if_listempty",
 /*  60 */ "template_some_if ::= template_if_namedsetempty",
 /*  61 */ "template_some_if ::= template_if_attribequal",
 /*  62 */ "template_some_if ::= template_if_hasattrib",
 /*  63 */ "template_if ::= template_some_if template_list KEY_ENDIF",
 /*  64 */ "template_if ::= template_some_if template_list KEY_ELSE template_list KEY_ENDIF",
 /*  65 */ "template_if ::= template_some_if KEY_ELSE template_list KEY_ENDIF",
 /*  66 */ "template_listappend ::= KEY_LISTAPPENDBEGIN KEY_SPACE STRING KEY_BB_CLOSE",
 /*  67 */ "template_listappend ::= KEY_LISTAPPENDBEGIN KEY_SPACE STRING KEY_SPACE STRING KEY_BB_CLOSE",
 /*  68 */ "template_listappend_end ::= KEY_LISTAPPENDEND",
 /*  69 */ "template_namedsetadd ::= KEY_NAMEDSETADD KEY_SPACE STRING KEY_SPACE STRING KEY_BB_CLOSE",
 /*  70 */ "template_namedsetadd_end ::= KEY_NAMEDSETADDEND",
 /*  71 */ "template_namedsetremove ::= KEY_NAMEDSETREMOVE KEY_SPACE STRING KEY_SPACE STRING KEY_BB_CLOSE",
 /*  72 */ "template_namedsetremove_end ::= KEY_NAMEDSETREMOVEEND",
 /*  73 */ "template_setvarnow ::= KEY_SETVAR KEY_SPACE STRING KEY_SPACE STRING KEY_BB_CLOSE",
 /*  74 */ "template_setvar ::= KEY_SETVAR KEY_SPACE STRING KEY_BB_CLOSE",
 /*  75 */ "template_setvar_end ::= KEY_SETVAREND",
 /*  76 */ "template_emitvar ::= KEY_EMITVAR KEY_SPACE STRING KEY_BB_CLOSE",
 /*  77 */ "template_string ::= CHAR",
 /*  78 */ "template_string ::= IDENTIFIER",
 /*  79 */ "template_string ::= KEY_BARE_COUNTER",
 /*  80 */ "template_string ::= KEY_BARE_FEATURE",
 /*  81 */ "template_string ::= KEY_BARE_FEATURENOMANGLE",
 /*  82 */ "template_string ::= KEY_BARE_FORMAT",
 /*  83 */ "template_string ::= KEY_BARE_VAR",
 /*  84 */ "template_string ::= KEY_BARE_VARNOMANGLE",
 /*  85 */ "template_string ::= INTEGER",
 /*  86 */ "template_string ::= STRING",
 /*  87 */ "template_string ::= KEY_SPACE",
 /*  88 */ "template_string ::= KEY_NEWLINE",
};
#endif /* NDEBUG */


#if TMPLGYYSTACKDEPTH<=0
/*
** Try to increase the size of the parser stack.  Return the number
** of errors.  Return 0 on success.
*/
static int tmplgyyGrowStack(tmplgyyParser *p){
  int newSize;
  int idx;
  tmplgyyStackEntry *pNew;

  newSize = p->tmplgyystksz*2 + 100;
  idx = p->tmplgyytos ? (int)(p->tmplgyytos - p->tmplgyystack) : 0;
  if( p->tmplgyystack==&p->tmplgyystk0 ){
    pNew = malloc(newSize*sizeof(pNew[0]));
    if( pNew ) pNew[0] = p->tmplgyystk0;
  }else{
    pNew = realloc(p->tmplgyystack, newSize*sizeof(pNew[0]));
  }
  if( pNew ){
    p->tmplgyystack = pNew;
    p->tmplgyytos = &p->tmplgyystack[idx];
#ifndef NDEBUG
    if( tmplgyyTraceFILE ){
      fprintf(tmplgyyTraceFILE,"%sStack grows from %d to %d entries.\n",
              tmplgyyTracePrompt, p->tmplgyystksz, newSize);
    }
#endif
    p->tmplgyystksz = newSize;
  }
  return pNew==0; 
}
#endif

/* Datatype of the argument to the memory allocated passed as the
** second argument to TemplateLangParserAlloc() below.  This can be changed by
** putting an appropriate #define in the %include section of the input
** grammar.
*/
#ifndef TMPLGYYMALLOCARGTYPE
# define TMPLGYYMALLOCARGTYPE size_t
#endif

/* 
** This function allocates a new parser.
** The only argument is a pointer to a function which works like
** malloc.
**
** Inputs:
** A pointer to the function used to allocate memory.
**
** Outputs:
** A pointer to a parser.  This pointer is used in subsequent calls
** to TemplateLangParser and TemplateLangParserFree.
*/
void *TemplateLangParserAlloc(void *(*mallocProc)(TMPLGYYMALLOCARGTYPE)){
  tmplgyyParser *pParser;
  pParser = (tmplgyyParser*)(*mallocProc)( (TMPLGYYMALLOCARGTYPE)sizeof(tmplgyyParser) );
  if( pParser ){
#ifdef TMPLGYYTRACKMAXSTACKDEPTH
    pParser->tmplgyyhwm = 0;
#endif
#if TMPLGYYSTACKDEPTH<=0
    pParser->tmplgyytos = NULL;
    pParser->tmplgyystack = NULL;
    pParser->tmplgyystksz = 0;
    if( tmplgyyGrowStack(pParser) ){
      pParser->tmplgyystack = &pParser->tmplgyystk0;
      pParser->tmplgyystksz = 1;
    }
#endif
#ifndef TMPLGYYNOERRORRECOVERY
    pParser->tmplgyyerrcnt = -1;
#endif
    pParser->tmplgyytos = pParser->tmplgyystack;
    pParser->tmplgyystack[0].stateno = 0;
    pParser->tmplgyystack[0].major = 0;
  }
  return pParser;
}

/* The following function deletes the "minor type" or semantic value
** associated with a symbol.  The symbol can be either a terminal
** or nonterminal. "tmplgyymajor" is the symbol code, and "tmplgyypminor" is
** a pointer to the value to be deleted.  The code used to do the 
** deletions is derived from the %destructor and/or %token_destructor
** directives of the input grammar.
*/
static void tmplgyy_destructor(
  tmplgyyParser *tmplgyypParser,    /* The parser */
  TMPLGYYCODETYPE tmplgyymajor,     /* Type code for object to destroy */
  TMPLGYYMINORTYPE *tmplgyypminor   /* The object to be destroyed */
){
  TemplateLangParserARG_FETCH;
  switch( tmplgyymajor ){
    /* Here is inserted the actions which take place when a
    ** terminal or non-terminal is destroyed.  This can happen
    ** when the symbol is popped from the stack during a
    ** reduce or during error processing or when a parser is 
    ** being destroyed before it is finished parsing.
    **
    ** Note: during a reduce, the only symbols destroyed are those
    ** which appear on the RHS of the rule, but which are *not* used
    ** inside the C code.
    */
/********* Begin destructor definitions ***************************************/
      /* TERMINAL Destructor */
    case 1: /* KEY_ID_D */
    case 2: /* KEY_FIRSTMONAD */
    case 3: /* KEY_FEATURE */
    case 4: /* KEY_SPACE */
    case 5: /* INTEGER */
    case 6: /* KEY_BB_CLOSE */
    case 7: /* KEY_FEATUREJSONMANGLE */
    case 8: /* KEY_FEATURENOMANGLE */
    case 9: /* KEY_DBNAME */
    case 10: /* KEY_ATTRIB */
    case 11: /* IDENTIFIER */
    case 12: /* KEY_ATTRIB_NOMANGLE */
    case 13: /* KEY_COPYATTRIBS */
    case 14: /* KEY_COUNTER */
    case 15: /* STRING */
    case 16: /* KEY_BARE_FORMAT */
    case 17: /* KEY_SETCOUNTER */
    case 18: /* KEY_BARE_VAR */
    case 19: /* KEY_INCCOUNTER */
    case 20: /* KEY_DECCOUNTER */
    case 21: /* KEY_LISTEMIT */
    case 22: /* KEY_LISTCLEAR */
    case 23: /* KEY_NAMEDSETEMIT */
    case 24: /* KEY_NAMEDSETCLEAR */
    case 25: /* KEY_DICTLOOKUP */
    case 26: /* KEY_BARE_FEATURE */
    case 27: /* KEY_BARE_FEATURENOMANGLE */
    case 28: /* KEY_BARE_VARNOMANGLE */
    case 29: /* KEY_BARE_COUNTER */
    case 30: /* KEY_SETNIXING_ALL */
    case 31: /* KEY_SETNIXING_DOC */
    case 32: /* KEY_SETNIXING_OFF */
    case 33: /* KEY_IF_FEATUREEQUAL */
    case 34: /* KEY_IF_VAREQUAL */
    case 35: /* KEY_IF_LISTEMPTY */
    case 36: /* KEY_IF_NAMEDSETEMPTY */
    case 37: /* KEY_IF_HASATTRIB */
    case 38: /* KEY_IF_ATTRIBEQUAL */
    case 39: /* KEY_ENDIF */
    case 40: /* KEY_ELSE */
    case 41: /* KEY_LISTAPPENDBEGIN */
    case 42: /* KEY_LISTAPPENDEND */
    case 43: /* KEY_NAMEDSETADD */
    case 44: /* KEY_NAMEDSETADDEND */
    case 45: /* KEY_NAMEDSETREMOVE */
    case 46: /* KEY_NAMEDSETREMOVEEND */
    case 47: /* KEY_SETVAR */
    case 48: /* KEY_SETVAREND */
    case 49: /* KEY_EMITVAR */
    case 50: /* CHAR */
    case 51: /* KEY_NEWLINE */
{
#line 100 "./templatelang.yxx"
 (tmplgyypminor->tmplgyy0)->clear(); delete (tmplgyypminor->tmplgyy0);
                    UNUSED(pEE); // Just to silence a warning
		  
#line 856 "./templatelang.c"
}
      break;
    case 53: /* top_value */
{
#line 120 "./templatelang.yxx"
 delete((tmplgyypminor->tmplgyy127)); 
#line 863 "./templatelang.c"
}
      break;
    case 54: /* template_list */
    case 55: /* template_list_element */
    case 78: /* template_some_if */
{
#line 124 "./templatelang.yxx"
delete((tmplgyypminor->tmplgyy127));
#line 872 "./templatelang.c"
}
      break;
    case 56: /* template_string */
{
#line 390 "./templatelang.yxx"
delete((tmplgyypminor->tmplgyy14));
#line 879 "./templatelang.c"
}
      break;
    case 57: /* template_if */
{
#line 322 "./templatelang.yxx"
delete((tmplgyypminor->tmplgyy24));
#line 886 "./templatelang.c"
}
      break;
    case 58: /* template_listappend */
{
#line 332 "./templatelang.yxx"
delete((tmplgyypminor->tmplgyy72));
#line 893 "./templatelang.c"
}
      break;
    case 59: /* template_listappend_end */
{
#line 339 "./templatelang.yxx"
delete((tmplgyypminor->tmplgyy102));
#line 900 "./templatelang.c"
}
      break;
    case 60: /* template_namedsetadd */
{
#line 344 "./templatelang.yxx"
delete((tmplgyypminor->tmplgyy30));
#line 907 "./templatelang.c"
}
      break;
    case 61: /* template_namedsetadd_end */
{
#line 349 "./templatelang.yxx"
delete((tmplgyypminor->tmplgyy56));
#line 914 "./templatelang.c"
}
      break;
    case 62: /* template_namedsetremove */
{
#line 354 "./templatelang.yxx"
delete((tmplgyypminor->tmplgyy15));
#line 921 "./templatelang.c"
}
      break;
    case 63: /* template_namedsetremove_end */
{
#line 359 "./templatelang.yxx"
delete((tmplgyypminor->tmplgyy123));
#line 928 "./templatelang.c"
}
      break;
    case 64: /* template_setvarnow */
{
#line 365 "./templatelang.yxx"
delete((tmplgyypminor->tmplgyy2));
#line 935 "./templatelang.c"
}
      break;
    case 65: /* template_setvar */
{
#line 370 "./templatelang.yxx"
delete((tmplgyypminor->tmplgyy83));
#line 942 "./templatelang.c"
}
      break;
    case 66: /* template_setvar_end */
{
#line 376 "./templatelang.yxx"
delete((tmplgyypminor->tmplgyy84));
#line 949 "./templatelang.c"
}
      break;
    case 67: /* template_emitvar */
{
#line 382 "./templatelang.yxx"
delete((tmplgyypminor->tmplgyy75));
#line 956 "./templatelang.c"
}
      break;
    case 68: /* template_setnixing */
{
#line 244 "./templatelang.yxx"
delete ((tmplgyypminor->tmplgyy136));
#line 963 "./templatelang.c"
}
      break;
    case 69: /* template_dictlookup_feature */
{
#line 213 "./templatelang.yxx"
delete ((tmplgyypminor->tmplgyy33));
#line 970 "./templatelang.c"
}
      break;
    case 70: /* template_dictlookup_var */
{
#line 224 "./templatelang.yxx"
delete ((tmplgyypminor->tmplgyy130));
#line 977 "./templatelang.c"
}
      break;
    case 71: /* template_dictlookup_counter */
{
#line 236 "./templatelang.yxx"
delete ((tmplgyypminor->tmplgyy79));
#line 984 "./templatelang.c"
}
      break;
    case 72: /* template_if_featureequal */
{
#line 261 "./templatelang.yxx"
delete ((tmplgyypminor->tmplgyy8));
#line 991 "./templatelang.c"
}
      break;
    case 73: /* template_if_varequal */
{
#line 268 "./templatelang.yxx"
delete ((tmplgyypminor->tmplgyy67));
#line 998 "./templatelang.c"
}
      break;
    case 74: /* template_if_listempty */
{
#line 279 "./templatelang.yxx"
delete ((tmplgyypminor->tmplgyy81));
#line 1005 "./templatelang.c"
}
      break;
    case 75: /* template_if_namedsetempty */
{
#line 286 "./templatelang.yxx"
delete ((tmplgyypminor->tmplgyy120));
#line 1012 "./templatelang.c"
}
      break;
    case 76: /* template_if_hasattrib */
{
#line 293 "./templatelang.yxx"
delete ((tmplgyypminor->tmplgyy52));
#line 1019 "./templatelang.c"
}
      break;
    case 77: /* template_if_attribequal */
{
#line 300 "./templatelang.yxx"
delete ((tmplgyypminor->tmplgyy32));
#line 1026 "./templatelang.c"
}
      break;
/********* End destructor definitions *****************************************/
    default:  break;   /* If no destructor action specified: do nothing */
  }
}

/*
** Pop the parser's stack once.
**
** If there is a destructor routine associated with the token which
** is popped from the stack, then call it.
*/
static void tmplgyy_pop_parser_stack(tmplgyyParser *pParser){
  tmplgyyStackEntry *tmplgyytos;
  assert( pParser->tmplgyytos!=0 );
  assert( pParser->tmplgyytos > pParser->tmplgyystack );
  tmplgyytos = pParser->tmplgyytos--;
#ifndef NDEBUG
  if( tmplgyyTraceFILE ){
    fprintf(tmplgyyTraceFILE,"%sPopping %s\n",
      tmplgyyTracePrompt,
      tmplgyyTokenName[tmplgyytos->major]);
  }
#endif
  tmplgyy_destructor(pParser, tmplgyytos->major, &tmplgyytos->minor);
}

/* 
** Deallocate and destroy a parser.  Destructors are called for
** all stack elements before shutting the parser down.
**
** If the TMPLGYYPARSEFREENEVERNULL macro exists (for example because it
** is defined in a %include section of the input grammar) then it is
** assumed that the input pointer is never NULL.
*/
void TemplateLangParserFree(
  void *p,                    /* The parser to be deleted */
  void (*freeProc)(void*)     /* Function used to reclaim memory */
){
  tmplgyyParser *pParser = (tmplgyyParser*)p;
#ifndef TMPLGYYPARSEFREENEVERNULL
  if( pParser==0 ) return;
#endif
  while( pParser->tmplgyytos>pParser->tmplgyystack ) tmplgyy_pop_parser_stack(pParser);
#if TMPLGYYSTACKDEPTH<=0
  if( pParser->tmplgyystack!=&pParser->tmplgyystk0 ) free(pParser->tmplgyystack);
#endif
  (*freeProc)((void*)pParser);
}

/*
** Return the peak depth of the stack for a parser.
*/
#ifdef TMPLGYYTRACKMAXSTACKDEPTH
int TemplateLangParserStackPeak(void *p){
  tmplgyyParser *pParser = (tmplgyyParser*)p;
  return pParser->tmplgyyhwm;
}
#endif

/*
** Find the appropriate action for a parser given the terminal
** look-ahead token iLookAhead.
*/
static unsigned int tmplgyy_find_shift_action(
  tmplgyyParser *pParser,        /* The parser */
  TMPLGYYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
  int stateno = pParser->tmplgyytos->stateno;
 
  if( stateno>=TMPLGYY_MIN_REDUCE ) return stateno;
  assert( stateno <= TMPLGYY_SHIFT_COUNT );
  do{
    i = tmplgyy_shift_ofst[stateno];
    assert( iLookAhead!=TMPLGYYNOCODE );
    i += iLookAhead;
    if( i<0 || i>=TMPLGYY_ACTTAB_COUNT || tmplgyy_lookahead[i]!=iLookAhead ){
#ifdef TMPLGYYFALLBACK
      TMPLGYYCODETYPE iFallback;            /* Fallback token */
      if( iLookAhead<sizeof(tmplgyyFallback)/sizeof(tmplgyyFallback[0])
             && (iFallback = tmplgyyFallback[iLookAhead])!=0 ){
#ifndef NDEBUG
        if( tmplgyyTraceFILE ){
          fprintf(tmplgyyTraceFILE, "%sFALLBACK %s => %s\n",
             tmplgyyTracePrompt, tmplgyyTokenName[iLookAhead], tmplgyyTokenName[iFallback]);
        }
#endif
        assert( tmplgyyFallback[iFallback]==0 ); /* Fallback loop must terminate */
        iLookAhead = iFallback;
        continue;
      }
#endif
#ifdef TMPLGYYWILDCARD
      {
        int j = i - iLookAhead + TMPLGYYWILDCARD;
        if( 
#if TMPLGYY_SHIFT_MIN+TMPLGYYWILDCARD<0
          j>=0 &&
#endif
#if TMPLGYY_SHIFT_MAX+TMPLGYYWILDCARD>=TMPLGYY_ACTTAB_COUNT
          j<TMPLGYY_ACTTAB_COUNT &&
#endif
          tmplgyy_lookahead[j]==TMPLGYYWILDCARD && iLookAhead>0
        ){
#ifndef NDEBUG
          if( tmplgyyTraceFILE ){
            fprintf(tmplgyyTraceFILE, "%sWILDCARD %s => %s\n",
               tmplgyyTracePrompt, tmplgyyTokenName[iLookAhead],
               tmplgyyTokenName[TMPLGYYWILDCARD]);
          }
#endif /* NDEBUG */
          return tmplgyy_action[j];
        }
      }
#endif /* TMPLGYYWILDCARD */
      return tmplgyy_default[stateno];
    }else{
      return tmplgyy_action[i];
    }
  }while(1);
}

/*
** Find the appropriate action for a parser given the non-terminal
** look-ahead token iLookAhead.
*/
static int tmplgyy_find_reduce_action(
  int stateno,              /* Current state number */
  TMPLGYYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
#ifdef TMPLGYYERRORSYMBOL
  if( stateno>TMPLGYY_REDUCE_COUNT ){
    return tmplgyy_default[stateno];
  }
#else
  assert( stateno<=TMPLGYY_REDUCE_COUNT );
#endif
  i = tmplgyy_reduce_ofst[stateno];
  assert( i!=TMPLGYY_REDUCE_USE_DFLT );
  assert( iLookAhead!=TMPLGYYNOCODE );
  i += iLookAhead;
#ifdef TMPLGYYERRORSYMBOL
  if( i<0 || i>=TMPLGYY_ACTTAB_COUNT || tmplgyy_lookahead[i]!=iLookAhead ){
    return tmplgyy_default[stateno];
  }
#else
  assert( i>=0 && i<TMPLGYY_ACTTAB_COUNT );
  assert( tmplgyy_lookahead[i]==iLookAhead );
#endif
  return tmplgyy_action[i];
}

/*
** The following routine is called if the stack overflows.
*/
static void tmplgyyStackOverflow(tmplgyyParser *tmplgyypParser){
   TemplateLangParserARG_FETCH;
#ifndef NDEBUG
   if( tmplgyyTraceFILE ){
     fprintf(tmplgyyTraceFILE,"%sStack Overflow!\n",tmplgyyTracePrompt);
   }
#endif
   while( tmplgyypParser->tmplgyytos>tmplgyypParser->tmplgyystack ) tmplgyy_pop_parser_stack(tmplgyypParser);
   /* Here code is inserted which will execute if the parser
   ** stack every overflows */
/******** Begin %stack_overflow code ******************************************/
/******** End %stack_overflow code ********************************************/
   TemplateLangParserARG_STORE; /* Suppress warning about unused %extra_argument var */
}

/*
** Print tracing information for a SHIFT action
*/
#ifndef NDEBUG
static void tmplgyyTraceShift(tmplgyyParser *tmplgyypParser, int tmplgyyNewState){
  if( tmplgyyTraceFILE ){
    if( tmplgyyNewState<TMPLGYYNSTATE ){
      fprintf(tmplgyyTraceFILE,"%sShift '%s', go to state %d\n",
         tmplgyyTracePrompt,tmplgyyTokenName[tmplgyypParser->tmplgyytos->major],
         tmplgyyNewState);
    }else{
      fprintf(tmplgyyTraceFILE,"%sShift '%s'\n",
         tmplgyyTracePrompt,tmplgyyTokenName[tmplgyypParser->tmplgyytos->major]);
    }
  }
}
#else
# define tmplgyyTraceShift(X,Y)
#endif

/*
** Perform a shift action.
*/
static void tmplgyy_shift(
  tmplgyyParser *tmplgyypParser,          /* The parser to be shifted */
  int tmplgyyNewState,               /* The new state to shift in */
  int tmplgyyMajor,                  /* The major token to shift in */
  TemplateLangParserTOKENTYPE tmplgyyMinor        /* The minor token to shift in */
){
  tmplgyyStackEntry *tmplgyytos;
  tmplgyypParser->tmplgyytos++;
#ifdef TMPLGYYTRACKMAXSTACKDEPTH
  if( (int)(tmplgyypParser->tmplgyytos - tmplgyypParser->tmplgyystack)>tmplgyypParser->tmplgyyhwm ){
    tmplgyypParser->tmplgyyhwm++;
    assert( tmplgyypParser->tmplgyyhwm == (int)(tmplgyypParser->tmplgyytos - tmplgyypParser->tmplgyystack) );
  }
#endif
#if TMPLGYYSTACKDEPTH>0 
  if( tmplgyypParser->tmplgyytos>=&tmplgyypParser->tmplgyystack[TMPLGYYSTACKDEPTH] ){
    tmplgyypParser->tmplgyytos--;
    tmplgyyStackOverflow(tmplgyypParser);
    return;
  }
#else
  if( tmplgyypParser->tmplgyytos>=&tmplgyypParser->tmplgyystack[tmplgyypParser->tmplgyystksz] ){
    if( tmplgyyGrowStack(tmplgyypParser) ){
      tmplgyypParser->tmplgyytos--;
      tmplgyyStackOverflow(tmplgyypParser);
      return;
    }
  }
#endif
  if( tmplgyyNewState > TMPLGYY_MAX_SHIFT ){
    tmplgyyNewState += TMPLGYY_MIN_REDUCE - TMPLGYY_MIN_SHIFTREDUCE;
  }
  tmplgyytos = tmplgyypParser->tmplgyytos;
  tmplgyytos->stateno = (TMPLGYYACTIONTYPE)tmplgyyNewState;
  tmplgyytos->major = (TMPLGYYCODETYPE)tmplgyyMajor;
  tmplgyytos->minor.tmplgyy0 = tmplgyyMinor;
  tmplgyyTraceShift(tmplgyypParser, tmplgyyNewState);
}

/* The following table contains information about every rule that
** is used during the reduce.
*/
static const struct {
  TMPLGYYCODETYPE lhs;         /* Symbol on the left-hand side of the rule */
  unsigned char nrhs;     /* Number of right-hand side symbols in the rule */
} tmplgyyRuleInfo[] = {
  { 53, 1 },
  { 54, 1 },
  { 54, 2 },
  { 55, 1 },
  { 55, 1 },
  { 55, 1 },
  { 55, 1 },
  { 55, 4 },
  { 55, 4 },
  { 55, 4 },
  { 55, 1 },
  { 55, 4 },
  { 55, 4 },
  { 55, 2 },
  { 55, 4 },
  { 55, 8 },
  { 55, 6 },
  { 55, 8 },
  { 55, 4 },
  { 55, 6 },
  { 55, 8 },
  { 55, 4 },
  { 55, 6 },
  { 55, 8 },
  { 55, 6 },
  { 55, 4 },
  { 55, 6 },
  { 55, 4 },
  { 55, 1 },
  { 55, 1 },
  { 55, 1 },
  { 55, 1 },
  { 55, 1 },
  { 55, 1 },
  { 55, 1 },
  { 55, 1 },
  { 55, 1 },
  { 55, 1 },
  { 55, 1 },
  { 55, 1 },
  { 55, 1 },
  { 55, 1 },
  { 69, 10 },
  { 69, 10 },
  { 70, 10 },
  { 70, 10 },
  { 71, 10 },
  { 68, 1 },
  { 68, 1 },
  { 68, 1 },
  { 72, 6 },
  { 73, 6 },
  { 73, 8 },
  { 74, 4 },
  { 75, 4 },
  { 76, 4 },
  { 77, 6 },
  { 78, 1 },
  { 78, 1 },
  { 78, 1 },
  { 78, 1 },
  { 78, 1 },
  { 78, 1 },
  { 57, 3 },
  { 57, 5 },
  { 57, 4 },
  { 58, 4 },
  { 58, 6 },
  { 59, 1 },
  { 60, 6 },
  { 61, 1 },
  { 62, 6 },
  { 63, 1 },
  { 64, 6 },
  { 65, 4 },
  { 66, 1 },
  { 67, 4 },
  { 56, 1 },
  { 56, 1 },
  { 56, 1 },
  { 56, 1 },
  { 56, 1 },
  { 56, 1 },
  { 56, 1 },
  { 56, 1 },
  { 56, 1 },
  { 56, 1 },
  { 56, 1 },
  { 56, 1 },
};

static void tmplgyy_accept(tmplgyyParser*);  /* Forward Declaration */

/*
** Perform a reduce action and the shift that must immediately
** follow the reduce.
*/
static void tmplgyy_reduce(
  tmplgyyParser *tmplgyypParser,         /* The parser */
  unsigned int tmplgyyruleno        /* Number of the rule by which to reduce */
){
  int tmplgyygoto;                     /* The next state */
  int tmplgyyact;                      /* The next action */
  tmplgyyStackEntry *tmplgyymsp;            /* The top of the parser's stack */
  int tmplgyysize;                     /* Amount to pop the stack */
  TemplateLangParserARG_FETCH;
  tmplgyymsp = tmplgyypParser->tmplgyytos;
#ifndef NDEBUG
  if( tmplgyyTraceFILE && tmplgyyruleno<(int)(sizeof(tmplgyyRuleName)/sizeof(tmplgyyRuleName[0])) ){
    tmplgyysize = tmplgyyRuleInfo[tmplgyyruleno].nrhs;
    fprintf(tmplgyyTraceFILE, "%sReduce [%s], go to state %d.\n", tmplgyyTracePrompt,
      tmplgyyRuleName[tmplgyyruleno], tmplgyymsp[-tmplgyysize].stateno);
  }
#endif /* NDEBUG */

  /* Check that the stack is large enough to grow by a single entry
  ** if the RHS of the rule is empty.  This ensures that there is room
  ** enough on the stack to push the LHS value */
  if( tmplgyyRuleInfo[tmplgyyruleno].nrhs==0 ){
#ifdef TMPLGYYTRACKMAXSTACKDEPTH
    if( (int)(tmplgyypParser->tmplgyytos - tmplgyypParser->tmplgyystack)>tmplgyypParser->tmplgyyhwm ){
      tmplgyypParser->tmplgyyhwm++;
      assert( tmplgyypParser->tmplgyyhwm == (int)(tmplgyypParser->tmplgyytos - tmplgyypParser->tmplgyystack));
    }
#endif
#if TMPLGYYSTACKDEPTH>0 
    if( tmplgyypParser->tmplgyytos>=&tmplgyypParser->tmplgyystack[TMPLGYYSTACKDEPTH-1] ){
      tmplgyyStackOverflow(tmplgyypParser);
      return;
    }
#else
    if( tmplgyypParser->tmplgyytos>=&tmplgyypParser->tmplgyystack[tmplgyypParser->tmplgyystksz-1] ){
      if( tmplgyyGrowStack(tmplgyypParser) ){
        tmplgyyStackOverflow(tmplgyypParser);
        return;
      }
      tmplgyymsp = tmplgyypParser->tmplgyytos;
    }
#endif
  }

  switch( tmplgyyruleno ){
  /* Beginning here are the reduction cases.  A typical example
  ** follows:
  **   case 0:
  **  #line <lineno> <grammarfile>
  **     { ... }           // User supplied code
  **  #line <lineno> <thisfile>
  **     break;
  */
/********** Begin reduce actions **********************************************/
        TMPLGYYMINORTYPE tmplgyylhsminor;
      case 0: /* top_value ::= template_list */
#line 121 "./templatelang.yxx"
{ pEE->setTopNode(tmplgyymsp[0].minor.tmplgyy127); }
#line 1424 "./templatelang.c"
        break;
      case 1: /* template_list ::= template_list_element */
#line 126 "./templatelang.yxx"
{ tmplgyylhsminor.tmplgyy127 = tmplgyymsp[0].minor.tmplgyy127; }
#line 1429 "./templatelang.c"
  tmplgyymsp[0].minor.tmplgyy127 = tmplgyylhsminor.tmplgyy127;
        break;
      case 2: /* template_list ::= template_list template_list_element */
#line 128 "./templatelang.yxx"
{ tmplgyylhsminor.tmplgyy127 = tmplgyymsp[0].minor.tmplgyy127; tmplgyylhsminor.tmplgyy127->setNext(tmplgyymsp[-1].minor.tmplgyy127); }
#line 1435 "./templatelang.c"
  tmplgyymsp[-1].minor.tmplgyy127 = tmplgyylhsminor.tmplgyy127;
        break;
      case 3: /* template_list_element ::= template_string */
#line 133 "./templatelang.yxx"
{ tmplgyylhsminor.tmplgyy127 = tmplgyymsp[0].minor.tmplgyy14; }
#line 1441 "./templatelang.c"
  tmplgyymsp[0].minor.tmplgyy127 = tmplgyylhsminor.tmplgyy127;
        break;
      case 4: /* template_list_element ::= template_if */
#line 135 "./templatelang.yxx"
{ tmplgyylhsminor.tmplgyy127 = tmplgyymsp[0].minor.tmplgyy24; }
#line 1447 "./templatelang.c"
  tmplgyymsp[0].minor.tmplgyy127 = tmplgyylhsminor.tmplgyy127;
        break;
      case 5: /* template_list_element ::= KEY_ID_D */
{  tmplgyy_destructor(tmplgyypParser,1,&tmplgyymsp[0].minor);
#line 137 "./templatelang.yxx"
{ tmplgyymsp[0].minor.tmplgyy127 = new TemplateSimpleNode(kTemplateID_D); }
#line 1454 "./templatelang.c"
}
        break;
      case 6: /* template_list_element ::= KEY_FIRSTMONAD */
{  tmplgyy_destructor(tmplgyypParser,2,&tmplgyymsp[0].minor);
#line 139 "./templatelang.yxx"
{ tmplgyymsp[0].minor.tmplgyy127 = new TemplateSimpleNode(kTemplateFirstMonad); }
#line 1461 "./templatelang.c"
}
        break;
      case 7: /* template_list_element ::= KEY_FEATURE KEY_SPACE INTEGER KEY_BB_CLOSE */
{  tmplgyy_destructor(tmplgyypParser,3,&tmplgyymsp[-3].minor);
#line 141 "./templatelang.yxx"
{ tmplgyymsp[-3].minor.tmplgyy127 = new TemplateFeature((unsigned int) tmplgyymsp[-1].minor.tmplgyy0->integer, kMKXML); delete tmplgyymsp[-1].minor.tmplgyy0; }
#line 1468 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,6,&tmplgyymsp[0].minor);
}
        break;
      case 8: /* template_list_element ::= KEY_FEATUREJSONMANGLE KEY_SPACE INTEGER KEY_BB_CLOSE */
{  tmplgyy_destructor(tmplgyypParser,7,&tmplgyymsp[-3].minor);
#line 143 "./templatelang.yxx"
{ tmplgyymsp[-3].minor.tmplgyy127 = new TemplateFeature((unsigned int) tmplgyymsp[-1].minor.tmplgyy0->integer, kMKJSON); delete tmplgyymsp[-1].minor.tmplgyy0; }
#line 1477 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,6,&tmplgyymsp[0].minor);
}
        break;
      case 9: /* template_list_element ::= KEY_FEATURENOMANGLE KEY_SPACE INTEGER KEY_BB_CLOSE */
{  tmplgyy_destructor(tmplgyypParser,8,&tmplgyymsp[-3].minor);
#line 145 "./templatelang.yxx"
{ tmplgyymsp[-3].minor.tmplgyy127 = new TemplateFeature((unsigned int) tmplgyymsp[-1].minor.tmplgyy0->integer, kMKNone); delete tmplgyymsp[-1].minor.tmplgyy0; }
#line 1486 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,6,&tmplgyymsp[0].minor);
}
        break;
      case 10: /* template_list_element ::= KEY_DBNAME */
{  tmplgyy_destructor(tmplgyypParser,9,&tmplgyymsp[0].minor);
#line 147 "./templatelang.yxx"
{ tmplgyymsp[0].minor.tmplgyy127 = new TemplateSimpleNode(kTemplateDBName); }
#line 1495 "./templatelang.c"
}
        break;
      case 11: /* template_list_element ::= KEY_ATTRIB KEY_SPACE IDENTIFIER KEY_BB_CLOSE */
{  tmplgyy_destructor(tmplgyypParser,10,&tmplgyymsp[-3].minor);
#line 149 "./templatelang.yxx"
{ tmplgyymsp[-3].minor.tmplgyy127 = new TemplateAttrib(tmplgyymsp[-1].minor.tmplgyy0->extractString()); delete tmplgyymsp[-1].minor.tmplgyy0; }
#line 1502 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,6,&tmplgyymsp[0].minor);
}
        break;
      case 12: /* template_list_element ::= KEY_ATTRIB_NOMANGLE KEY_SPACE IDENTIFIER KEY_BB_CLOSE */
{  tmplgyy_destructor(tmplgyypParser,12,&tmplgyymsp[-3].minor);
#line 151 "./templatelang.yxx"
{ tmplgyymsp[-3].minor.tmplgyy127 = new TemplateAttribNoMangle(tmplgyymsp[-1].minor.tmplgyy0->extractString()); delete tmplgyymsp[-1].minor.tmplgyy0; }
#line 1511 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,6,&tmplgyymsp[0].minor);
}
        break;
      case 13: /* template_list_element ::= KEY_COPYATTRIBS KEY_BB_CLOSE */
{  tmplgyy_destructor(tmplgyypParser,13,&tmplgyymsp[-1].minor);
#line 153 "./templatelang.yxx"
{ tmplgyymsp[-1].minor.tmplgyy127 = new TemplateCopyAttribs(); }
#line 1520 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,6,&tmplgyymsp[0].minor);
}
        break;
      case 14: /* template_list_element ::= KEY_COUNTER KEY_SPACE STRING KEY_BB_CLOSE */
{  tmplgyy_destructor(tmplgyypParser,14,&tmplgyymsp[-3].minor);
#line 155 "./templatelang.yxx"
{ tmplgyymsp[-3].minor.tmplgyy127 = new TemplateCounter(tmplgyymsp[-1].minor.tmplgyy0->extractString()); delete tmplgyymsp[-1].minor.tmplgyy0; }
#line 1528 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,6,&tmplgyymsp[0].minor);
}
        break;
      case 15: /* template_list_element ::= KEY_COUNTER KEY_SPACE STRING KEY_SPACE KEY_BARE_FORMAT KEY_SPACE STRING KEY_BB_CLOSE */
{  tmplgyy_destructor(tmplgyypParser,14,&tmplgyymsp[-7].minor);
#line 157 "./templatelang.yxx"
{ tmplgyymsp[-7].minor.tmplgyy127 = new TemplateCounterFormat(tmplgyymsp[-5].minor.tmplgyy0->extractString(), tmplgyymsp[-1].minor.tmplgyy0->extractString()); delete tmplgyymsp[-5].minor.tmplgyy0; delete tmplgyymsp[-1].minor.tmplgyy0;}
#line 1537 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-6].minor);
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-4].minor);
  tmplgyy_destructor(tmplgyypParser,16,&tmplgyymsp[-3].minor);
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,6,&tmplgyymsp[0].minor);
}
        break;
      case 16: /* template_list_element ::= KEY_SETCOUNTER KEY_SPACE STRING KEY_SPACE INTEGER KEY_BB_CLOSE */
{  tmplgyy_destructor(tmplgyypParser,17,&tmplgyymsp[-5].minor);
#line 159 "./templatelang.yxx"
{ tmplgyymsp[-5].minor.tmplgyy127 = new TemplateSetCounter(tmplgyymsp[-3].minor.tmplgyy0->extractString(), (unsigned int) tmplgyymsp[-1].minor.tmplgyy0->integer); delete tmplgyymsp[-3].minor.tmplgyy0; delete tmplgyymsp[-1].minor.tmplgyy0; }
#line 1549 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-4].minor);
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,6,&tmplgyymsp[0].minor);
}
        break;
      case 17: /* template_list_element ::= KEY_SETCOUNTER KEY_SPACE STRING KEY_SPACE KEY_BARE_VAR KEY_SPACE STRING KEY_BB_CLOSE */
{  tmplgyy_destructor(tmplgyypParser,17,&tmplgyymsp[-7].minor);
#line 161 "./templatelang.yxx"
{ tmplgyymsp[-7].minor.tmplgyy127 = new TemplateSetCounterVar(tmplgyymsp[-5].minor.tmplgyy0->extractString(), tmplgyymsp[-1].minor.tmplgyy0->extractString()); delete tmplgyymsp[-5].minor.tmplgyy0; delete tmplgyymsp[-1].minor.tmplgyy0; }
#line 1559 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-6].minor);
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-4].minor);
  tmplgyy_destructor(tmplgyypParser,18,&tmplgyymsp[-3].minor);
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,6,&tmplgyymsp[0].minor);
}
        break;
      case 18: /* template_list_element ::= KEY_INCCOUNTER KEY_SPACE STRING KEY_BB_CLOSE */
{  tmplgyy_destructor(tmplgyypParser,19,&tmplgyymsp[-3].minor);
#line 163 "./templatelang.yxx"
{ tmplgyymsp[-3].minor.tmplgyy127 = new TemplateIncCounter(tmplgyymsp[-1].minor.tmplgyy0->extractString(), 1, 0); delete tmplgyymsp[-1].minor.tmplgyy0; }
#line 1571 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,6,&tmplgyymsp[0].minor);
}
        break;
      case 19: /* template_list_element ::= KEY_INCCOUNTER KEY_SPACE STRING KEY_SPACE INTEGER KEY_BB_CLOSE */
{  tmplgyy_destructor(tmplgyypParser,19,&tmplgyymsp[-5].minor);
#line 165 "./templatelang.yxx"
{ tmplgyymsp[-5].minor.tmplgyy127 = new TemplateIncCounter(tmplgyymsp[-3].minor.tmplgyy0->extractString(), (unsigned int) tmplgyymsp[-1].minor.tmplgyy0->integer, 0); delete tmplgyymsp[-3].minor.tmplgyy0; delete tmplgyymsp[-1].minor.tmplgyy0; }
#line 1580 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-4].minor);
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,6,&tmplgyymsp[0].minor);
}
        break;
      case 20: /* template_list_element ::= KEY_INCCOUNTER KEY_SPACE STRING KEY_SPACE KEY_BARE_VAR KEY_SPACE STRING KEY_BB_CLOSE */
{  tmplgyy_destructor(tmplgyypParser,19,&tmplgyymsp[-7].minor);
#line 167 "./templatelang.yxx"
{ tmplgyymsp[-7].minor.tmplgyy127 = new TemplateIncCounter(tmplgyymsp[-5].minor.tmplgyy0->extractString(), 0, tmplgyymsp[-1].minor.tmplgyy0->extractString()); delete tmplgyymsp[-5].minor.tmplgyy0; delete tmplgyymsp[-1].minor.tmplgyy0; }
#line 1590 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-6].minor);
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-4].minor);
  tmplgyy_destructor(tmplgyypParser,18,&tmplgyymsp[-3].minor);
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,6,&tmplgyymsp[0].minor);
}
        break;
      case 21: /* template_list_element ::= KEY_DECCOUNTER KEY_SPACE STRING KEY_BB_CLOSE */
{  tmplgyy_destructor(tmplgyypParser,20,&tmplgyymsp[-3].minor);
#line 169 "./templatelang.yxx"
{ tmplgyymsp[-3].minor.tmplgyy127 = new TemplateDecCounter(tmplgyymsp[-1].minor.tmplgyy0->extractString(), 1, 0); delete tmplgyymsp[-1].minor.tmplgyy0; }
#line 1602 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,6,&tmplgyymsp[0].minor);
}
        break;
      case 22: /* template_list_element ::= KEY_DECCOUNTER KEY_SPACE STRING KEY_SPACE INTEGER KEY_BB_CLOSE */
{  tmplgyy_destructor(tmplgyypParser,20,&tmplgyymsp[-5].minor);
#line 171 "./templatelang.yxx"
{ tmplgyymsp[-5].minor.tmplgyy127 = new TemplateDecCounter(tmplgyymsp[-3].minor.tmplgyy0->extractString(), (unsigned int) tmplgyymsp[-1].minor.tmplgyy0->integer, 0); delete tmplgyymsp[-3].minor.tmplgyy0; delete tmplgyymsp[-1].minor.tmplgyy0; }
#line 1611 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-4].minor);
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,6,&tmplgyymsp[0].minor);
}
        break;
      case 23: /* template_list_element ::= KEY_DECCOUNTER KEY_SPACE STRING KEY_SPACE KEY_BARE_VAR KEY_SPACE STRING KEY_BB_CLOSE */
{  tmplgyy_destructor(tmplgyypParser,20,&tmplgyymsp[-7].minor);
#line 173 "./templatelang.yxx"
{ tmplgyymsp[-7].minor.tmplgyy127 = new TemplateDecCounter(tmplgyymsp[-5].minor.tmplgyy0->extractString(), 0, tmplgyymsp[-1].minor.tmplgyy0->extractString()); delete tmplgyymsp[-5].minor.tmplgyy0; delete tmplgyymsp[-1].minor.tmplgyy0; }
#line 1621 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-6].minor);
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-4].minor);
  tmplgyy_destructor(tmplgyypParser,18,&tmplgyymsp[-3].minor);
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,6,&tmplgyymsp[0].minor);
}
        break;
      case 24: /* template_list_element ::= KEY_LISTEMIT KEY_SPACE STRING KEY_SPACE STRING KEY_BB_CLOSE */
{  tmplgyy_destructor(tmplgyypParser,21,&tmplgyymsp[-5].minor);
#line 175 "./templatelang.yxx"
{ tmplgyymsp[-5].minor.tmplgyy127 = new TemplateListEmit(tmplgyymsp[-3].minor.tmplgyy0->extractString(), tmplgyymsp[-1].minor.tmplgyy0->extractString()); delete tmplgyymsp[-3].minor.tmplgyy0; delete tmplgyymsp[-1].minor.tmplgyy0; }
#line 1633 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-4].minor);
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,6,&tmplgyymsp[0].minor);
}
        break;
      case 25: /* template_list_element ::= KEY_LISTCLEAR KEY_SPACE STRING KEY_BB_CLOSE */
{  tmplgyy_destructor(tmplgyypParser,22,&tmplgyymsp[-3].minor);
#line 177 "./templatelang.yxx"
{ tmplgyymsp[-3].minor.tmplgyy127 = new TemplateListClear(tmplgyymsp[-1].minor.tmplgyy0->extractString()); delete tmplgyymsp[-1].minor.tmplgyy0; }
#line 1643 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,6,&tmplgyymsp[0].minor);
}
        break;
      case 26: /* template_list_element ::= KEY_NAMEDSETEMIT KEY_SPACE STRING KEY_SPACE STRING KEY_BB_CLOSE */
{  tmplgyy_destructor(tmplgyypParser,23,&tmplgyymsp[-5].minor);
#line 179 "./templatelang.yxx"
{ tmplgyymsp[-5].minor.tmplgyy127 = new TemplateNamedSetEmit(tmplgyymsp[-3].minor.tmplgyy0->extractString(), tmplgyymsp[-1].minor.tmplgyy0->extractString()); delete tmplgyymsp[-3].minor.tmplgyy0; delete tmplgyymsp[-1].minor.tmplgyy0; }
#line 1652 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-4].minor);
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,6,&tmplgyymsp[0].minor);
}
        break;
      case 27: /* template_list_element ::= KEY_NAMEDSETCLEAR KEY_SPACE STRING KEY_BB_CLOSE */
{  tmplgyy_destructor(tmplgyypParser,24,&tmplgyymsp[-3].minor);
#line 181 "./templatelang.yxx"
{ tmplgyymsp[-3].minor.tmplgyy127 = new TemplateNamedSetClear(tmplgyymsp[-1].minor.tmplgyy0->extractString()); delete tmplgyymsp[-1].minor.tmplgyy0; }
#line 1662 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,6,&tmplgyymsp[0].minor);
}
        break;
      case 28: /* template_list_element ::= template_listappend */
#line 183 "./templatelang.yxx"
{ tmplgyylhsminor.tmplgyy127 = tmplgyymsp[0].minor.tmplgyy72; }
#line 1670 "./templatelang.c"
  tmplgyymsp[0].minor.tmplgyy127 = tmplgyylhsminor.tmplgyy127;
        break;
      case 29: /* template_list_element ::= template_listappend_end */
#line 185 "./templatelang.yxx"
{ tmplgyylhsminor.tmplgyy127 = tmplgyymsp[0].minor.tmplgyy102; }
#line 1676 "./templatelang.c"
  tmplgyymsp[0].minor.tmplgyy127 = tmplgyylhsminor.tmplgyy127;
        break;
      case 30: /* template_list_element ::= template_namedsetadd */
#line 187 "./templatelang.yxx"
{ tmplgyylhsminor.tmplgyy127 = tmplgyymsp[0].minor.tmplgyy30; }
#line 1682 "./templatelang.c"
  tmplgyymsp[0].minor.tmplgyy127 = tmplgyylhsminor.tmplgyy127;
        break;
      case 31: /* template_list_element ::= template_namedsetadd_end */
#line 189 "./templatelang.yxx"
{ tmplgyylhsminor.tmplgyy127 = tmplgyymsp[0].minor.tmplgyy56; }
#line 1688 "./templatelang.c"
  tmplgyymsp[0].minor.tmplgyy127 = tmplgyylhsminor.tmplgyy127;
        break;
      case 32: /* template_list_element ::= template_namedsetremove */
#line 191 "./templatelang.yxx"
{ tmplgyylhsminor.tmplgyy127 = tmplgyymsp[0].minor.tmplgyy15; }
#line 1694 "./templatelang.c"
  tmplgyymsp[0].minor.tmplgyy127 = tmplgyylhsminor.tmplgyy127;
        break;
      case 33: /* template_list_element ::= template_namedsetremove_end */
#line 193 "./templatelang.yxx"
{ tmplgyylhsminor.tmplgyy127 = tmplgyymsp[0].minor.tmplgyy123; }
#line 1700 "./templatelang.c"
  tmplgyymsp[0].minor.tmplgyy127 = tmplgyylhsminor.tmplgyy127;
        break;
      case 34: /* template_list_element ::= template_setvarnow */
#line 195 "./templatelang.yxx"
{ tmplgyylhsminor.tmplgyy127 = tmplgyymsp[0].minor.tmplgyy2; }
#line 1706 "./templatelang.c"
  tmplgyymsp[0].minor.tmplgyy127 = tmplgyylhsminor.tmplgyy127;
        break;
      case 35: /* template_list_element ::= template_setvar */
#line 197 "./templatelang.yxx"
{ tmplgyylhsminor.tmplgyy127 = tmplgyymsp[0].minor.tmplgyy83; }
#line 1712 "./templatelang.c"
  tmplgyymsp[0].minor.tmplgyy127 = tmplgyylhsminor.tmplgyy127;
        break;
      case 36: /* template_list_element ::= template_setvar_end */
#line 199 "./templatelang.yxx"
{ tmplgyylhsminor.tmplgyy127 = tmplgyymsp[0].minor.tmplgyy84; }
#line 1718 "./templatelang.c"
  tmplgyymsp[0].minor.tmplgyy127 = tmplgyylhsminor.tmplgyy127;
        break;
      case 37: /* template_list_element ::= template_emitvar */
#line 201 "./templatelang.yxx"
{ tmplgyylhsminor.tmplgyy127 = tmplgyymsp[0].minor.tmplgyy75; }
#line 1724 "./templatelang.c"
  tmplgyymsp[0].minor.tmplgyy127 = tmplgyylhsminor.tmplgyy127;
        break;
      case 38: /* template_list_element ::= template_setnixing */
#line 203 "./templatelang.yxx"
{ tmplgyylhsminor.tmplgyy127 = tmplgyymsp[0].minor.tmplgyy136; }
#line 1730 "./templatelang.c"
  tmplgyymsp[0].minor.tmplgyy127 = tmplgyylhsminor.tmplgyy127;
        break;
      case 39: /* template_list_element ::= template_dictlookup_feature */
#line 205 "./templatelang.yxx"
{ tmplgyylhsminor.tmplgyy127 = tmplgyymsp[0].minor.tmplgyy33; }
#line 1736 "./templatelang.c"
  tmplgyymsp[0].minor.tmplgyy127 = tmplgyylhsminor.tmplgyy127;
        break;
      case 40: /* template_list_element ::= template_dictlookup_var */
#line 207 "./templatelang.yxx"
{ tmplgyylhsminor.tmplgyy127 = tmplgyymsp[0].minor.tmplgyy130; }
#line 1742 "./templatelang.c"
  tmplgyymsp[0].minor.tmplgyy127 = tmplgyylhsminor.tmplgyy127;
        break;
      case 41: /* template_list_element ::= template_dictlookup_counter */
#line 209 "./templatelang.yxx"
{ tmplgyylhsminor.tmplgyy127 = tmplgyymsp[0].minor.tmplgyy79; }
#line 1748 "./templatelang.c"
  tmplgyymsp[0].minor.tmplgyy127 = tmplgyylhsminor.tmplgyy127;
        break;
      case 42: /* template_dictlookup_feature ::= KEY_DICTLOOKUP KEY_SPACE STRING KEY_SPACE KEY_BARE_FEATURE KEY_SPACE INTEGER KEY_SPACE STRING KEY_BB_CLOSE */
{  tmplgyy_destructor(tmplgyypParser,25,&tmplgyymsp[-9].minor);
#line 215 "./templatelang.yxx"
{ tmplgyymsp[-9].minor.tmplgyy33 = new TemplateDictlookupFeature(tmplgyymsp[-7].minor.tmplgyy0->extractString(), (unsigned int) tmplgyymsp[-3].minor.tmplgyy0->integer, tmplgyymsp[-1].minor.tmplgyy0->extractString(), true); 
  delete tmplgyymsp[-7].minor.tmplgyy0; delete tmplgyymsp[-3].minor.tmplgyy0; delete tmplgyymsp[-1].minor.tmplgyy0;
}
#line 1757 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-8].minor);
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-6].minor);
  tmplgyy_destructor(tmplgyypParser,26,&tmplgyymsp[-5].minor);
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-4].minor);
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,6,&tmplgyymsp[0].minor);
}
        break;
      case 43: /* template_dictlookup_feature ::= KEY_DICTLOOKUP KEY_SPACE STRING KEY_SPACE KEY_BARE_FEATURENOMANGLE KEY_SPACE INTEGER KEY_SPACE STRING KEY_BB_CLOSE */
{  tmplgyy_destructor(tmplgyypParser,25,&tmplgyymsp[-9].minor);
#line 219 "./templatelang.yxx"
{ tmplgyymsp[-9].minor.tmplgyy33 = new TemplateDictlookupFeature(tmplgyymsp[-7].minor.tmplgyy0->extractString(), (unsigned int) tmplgyymsp[-3].minor.tmplgyy0->integer, tmplgyymsp[-1].minor.tmplgyy0->extractString(), false); 
  delete tmplgyymsp[-7].minor.tmplgyy0; delete tmplgyymsp[-3].minor.tmplgyy0; delete tmplgyymsp[-1].minor.tmplgyy0;
}
#line 1772 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-8].minor);
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-6].minor);
  tmplgyy_destructor(tmplgyypParser,27,&tmplgyymsp[-5].minor);
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-4].minor);
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,6,&tmplgyymsp[0].minor);
}
        break;
      case 44: /* template_dictlookup_var ::= KEY_DICTLOOKUP KEY_SPACE STRING KEY_SPACE KEY_BARE_VAR KEY_SPACE STRING KEY_SPACE STRING KEY_BB_CLOSE */
{  tmplgyy_destructor(tmplgyypParser,25,&tmplgyymsp[-9].minor);
#line 226 "./templatelang.yxx"
{ tmplgyymsp[-9].minor.tmplgyy130 = new TemplateDictlookupVar(tmplgyymsp[-7].minor.tmplgyy0->extractString(), tmplgyymsp[-3].minor.tmplgyy0->extractString(), tmplgyymsp[-1].minor.tmplgyy0->extractString(), true); 
  delete tmplgyymsp[-7].minor.tmplgyy0; delete tmplgyymsp[-3].minor.tmplgyy0; delete tmplgyymsp[-1].minor.tmplgyy0;
}
#line 1787 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-8].minor);
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-6].minor);
  tmplgyy_destructor(tmplgyypParser,18,&tmplgyymsp[-5].minor);
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-4].minor);
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,6,&tmplgyymsp[0].minor);
}
        break;
      case 45: /* template_dictlookup_var ::= KEY_DICTLOOKUP KEY_SPACE STRING KEY_SPACE KEY_BARE_VARNOMANGLE KEY_SPACE STRING KEY_SPACE STRING KEY_BB_CLOSE */
{  tmplgyy_destructor(tmplgyypParser,25,&tmplgyymsp[-9].minor);
#line 230 "./templatelang.yxx"
{ tmplgyymsp[-9].minor.tmplgyy130 = new TemplateDictlookupVar(tmplgyymsp[-7].minor.tmplgyy0->extractString(), tmplgyymsp[-3].minor.tmplgyy0->extractString(), tmplgyymsp[-1].minor.tmplgyy0->extractString(), false); 
  delete tmplgyymsp[-7].minor.tmplgyy0; delete tmplgyymsp[-3].minor.tmplgyy0; delete tmplgyymsp[-1].minor.tmplgyy0;
}
#line 1802 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-8].minor);
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-6].minor);
  tmplgyy_destructor(tmplgyypParser,28,&tmplgyymsp[-5].minor);
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-4].minor);
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,6,&tmplgyymsp[0].minor);
}
        break;
      case 46: /* template_dictlookup_counter ::= KEY_DICTLOOKUP KEY_SPACE STRING KEY_SPACE KEY_BARE_COUNTER KEY_SPACE STRING KEY_SPACE STRING KEY_BB_CLOSE */
{  tmplgyy_destructor(tmplgyypParser,25,&tmplgyymsp[-9].minor);
#line 238 "./templatelang.yxx"
{ tmplgyymsp[-9].minor.tmplgyy79 = new TemplateDictlookupCounter(tmplgyymsp[-7].minor.tmplgyy0->extractString(), tmplgyymsp[-3].minor.tmplgyy0->extractString(), tmplgyymsp[-1].minor.tmplgyy0->extractString(), true); 
  delete tmplgyymsp[-7].minor.tmplgyy0; delete tmplgyymsp[-3].minor.tmplgyy0; delete tmplgyymsp[-1].minor.tmplgyy0;
}
#line 1817 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-8].minor);
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-6].minor);
  tmplgyy_destructor(tmplgyypParser,29,&tmplgyymsp[-5].minor);
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-4].minor);
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,6,&tmplgyymsp[0].minor);
}
        break;
      case 47: /* template_setnixing ::= KEY_SETNIXING_ALL */
{  tmplgyy_destructor(tmplgyypParser,30,&tmplgyymsp[0].minor);
#line 246 "./templatelang.yxx"
{ tmplgyymsp[0].minor.tmplgyy136 = new TemplateNixing(kTNTAll); 
}
#line 1831 "./templatelang.c"
}
        break;
      case 48: /* template_setnixing ::= KEY_SETNIXING_DOC */
{  tmplgyy_destructor(tmplgyypParser,31,&tmplgyymsp[0].minor);
#line 249 "./templatelang.yxx"
{ tmplgyymsp[0].minor.tmplgyy136 = new TemplateNixing(kTNTDoc); 
}
#line 1839 "./templatelang.c"
}
        break;
      case 49: /* template_setnixing ::= KEY_SETNIXING_OFF */
{  tmplgyy_destructor(tmplgyypParser,32,&tmplgyymsp[0].minor);
#line 252 "./templatelang.yxx"
{ tmplgyymsp[0].minor.tmplgyy136 = new TemplateNixing(kTNTOff); 
}
#line 1847 "./templatelang.c"
}
        break;
      case 50: /* template_if_featureequal ::= KEY_IF_FEATUREEQUAL KEY_SPACE INTEGER KEY_SPACE STRING KEY_BB_CLOSE */
{  tmplgyy_destructor(tmplgyypParser,33,&tmplgyymsp[-5].minor);
#line 263 "./templatelang.yxx"
{ tmplgyymsp[-5].minor.tmplgyy8 = new TemplateIfFeatureEqual((unsigned int) tmplgyymsp[-3].minor.tmplgyy0->integer, tmplgyymsp[-1].minor.tmplgyy0->extractString()); 
  delete tmplgyymsp[-1].minor.tmplgyy0; delete tmplgyymsp[-3].minor.tmplgyy0;			     
}
#line 1856 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-4].minor);
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,6,&tmplgyymsp[0].minor);
}
        break;
      case 51: /* template_if_varequal ::= KEY_IF_VAREQUAL KEY_SPACE STRING KEY_SPACE STRING KEY_BB_CLOSE */
{  tmplgyy_destructor(tmplgyypParser,34,&tmplgyymsp[-5].minor);
#line 270 "./templatelang.yxx"
{ tmplgyymsp[-5].minor.tmplgyy67 = new TemplateIfVarEqual(tmplgyymsp[-3].minor.tmplgyy0->extractString(), tmplgyymsp[-1].minor.tmplgyy0->extractString(), false); 
  delete tmplgyymsp[-3].minor.tmplgyy0; delete tmplgyymsp[-1].minor.tmplgyy0;			     
}
#line 1868 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-4].minor);
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,6,&tmplgyymsp[0].minor);
}
        break;
      case 52: /* template_if_varequal ::= KEY_IF_VAREQUAL KEY_SPACE STRING KEY_SPACE KEY_BARE_VAR KEY_SPACE STRING KEY_BB_CLOSE */
{  tmplgyy_destructor(tmplgyypParser,34,&tmplgyymsp[-7].minor);
#line 274 "./templatelang.yxx"
{ tmplgyymsp[-7].minor.tmplgyy67 = new TemplateIfVarEqual(tmplgyymsp[-5].minor.tmplgyy0->extractString(), tmplgyymsp[-1].minor.tmplgyy0->extractString(), true); 
  delete tmplgyymsp[-5].minor.tmplgyy0; delete tmplgyymsp[-1].minor.tmplgyy0;			     
}
#line 1880 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-6].minor);
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-4].minor);
  tmplgyy_destructor(tmplgyypParser,18,&tmplgyymsp[-3].minor);
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,6,&tmplgyymsp[0].minor);
}
        break;
      case 53: /* template_if_listempty ::= KEY_IF_LISTEMPTY KEY_SPACE STRING KEY_BB_CLOSE */
{  tmplgyy_destructor(tmplgyypParser,35,&tmplgyymsp[-3].minor);
#line 281 "./templatelang.yxx"
{ tmplgyymsp[-3].minor.tmplgyy81 = new TemplateIfListEmpty(tmplgyymsp[-1].minor.tmplgyy0->extractString());
  delete tmplgyymsp[-1].minor.tmplgyy0; 
}
#line 1894 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,6,&tmplgyymsp[0].minor);
}
        break;
      case 54: /* template_if_namedsetempty ::= KEY_IF_NAMEDSETEMPTY KEY_SPACE STRING KEY_BB_CLOSE */
{  tmplgyy_destructor(tmplgyypParser,36,&tmplgyymsp[-3].minor);
#line 288 "./templatelang.yxx"
{ tmplgyymsp[-3].minor.tmplgyy120 = new TemplateIfNamedSetEmpty(tmplgyymsp[-1].minor.tmplgyy0->extractString());
  delete tmplgyymsp[-1].minor.tmplgyy0; 
}
#line 1905 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,6,&tmplgyymsp[0].minor);
}
        break;
      case 55: /* template_if_hasattrib ::= KEY_IF_HASATTRIB KEY_SPACE IDENTIFIER KEY_BB_CLOSE */
{  tmplgyy_destructor(tmplgyypParser,37,&tmplgyymsp[-3].minor);
#line 295 "./templatelang.yxx"
{ tmplgyymsp[-3].minor.tmplgyy52 = new TemplateIfHasAttrib(tmplgyymsp[-1].minor.tmplgyy0->extractString()); 
  delete tmplgyymsp[-1].minor.tmplgyy0; 
}
#line 1916 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,6,&tmplgyymsp[0].minor);
}
        break;
      case 56: /* template_if_attribequal ::= KEY_IF_ATTRIBEQUAL KEY_SPACE IDENTIFIER KEY_SPACE STRING KEY_BB_CLOSE */
{  tmplgyy_destructor(tmplgyypParser,38,&tmplgyymsp[-5].minor);
#line 302 "./templatelang.yxx"
{ tmplgyymsp[-5].minor.tmplgyy32 = new TemplateIfAttribEqual(tmplgyymsp[-3].minor.tmplgyy0->extractString(), tmplgyymsp[-1].minor.tmplgyy0->extractString()); 
  delete tmplgyymsp[-1].minor.tmplgyy0; delete tmplgyymsp[-3].minor.tmplgyy0; 
}
#line 1927 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-4].minor);
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,6,&tmplgyymsp[0].minor);
}
        break;
      case 57: /* template_some_if ::= template_if_featureequal */
#line 309 "./templatelang.yxx"
{ tmplgyylhsminor.tmplgyy127 = tmplgyymsp[0].minor.tmplgyy8; }
#line 1936 "./templatelang.c"
  tmplgyymsp[0].minor.tmplgyy127 = tmplgyylhsminor.tmplgyy127;
        break;
      case 58: /* template_some_if ::= template_if_varequal */
#line 311 "./templatelang.yxx"
{ tmplgyylhsminor.tmplgyy127 = tmplgyymsp[0].minor.tmplgyy67; }
#line 1942 "./templatelang.c"
  tmplgyymsp[0].minor.tmplgyy127 = tmplgyylhsminor.tmplgyy127;
        break;
      case 59: /* template_some_if ::= template_if_listempty */
#line 313 "./templatelang.yxx"
{ tmplgyylhsminor.tmplgyy127 = tmplgyymsp[0].minor.tmplgyy81; }
#line 1948 "./templatelang.c"
  tmplgyymsp[0].minor.tmplgyy127 = tmplgyylhsminor.tmplgyy127;
        break;
      case 60: /* template_some_if ::= template_if_namedsetempty */
#line 315 "./templatelang.yxx"
{ tmplgyylhsminor.tmplgyy127 = tmplgyymsp[0].minor.tmplgyy120; }
#line 1954 "./templatelang.c"
  tmplgyymsp[0].minor.tmplgyy127 = tmplgyylhsminor.tmplgyy127;
        break;
      case 61: /* template_some_if ::= template_if_attribequal */
#line 317 "./templatelang.yxx"
{ tmplgyylhsminor.tmplgyy127 = tmplgyymsp[0].minor.tmplgyy32; }
#line 1960 "./templatelang.c"
  tmplgyymsp[0].minor.tmplgyy127 = tmplgyylhsminor.tmplgyy127;
        break;
      case 62: /* template_some_if ::= template_if_hasattrib */
#line 319 "./templatelang.yxx"
{ tmplgyylhsminor.tmplgyy127 = tmplgyymsp[0].minor.tmplgyy52; }
#line 1966 "./templatelang.c"
  tmplgyymsp[0].minor.tmplgyy127 = tmplgyylhsminor.tmplgyy127;
        break;
      case 63: /* template_if ::= template_some_if template_list KEY_ENDIF */
#line 324 "./templatelang.yxx"
{ tmplgyylhsminor.tmplgyy24 = new TemplateIf(tmplgyymsp[-2].minor.tmplgyy127, tmplgyymsp[-1].minor.tmplgyy127, 0); }
#line 1972 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,39,&tmplgyymsp[0].minor);
  tmplgyymsp[-2].minor.tmplgyy24 = tmplgyylhsminor.tmplgyy24;
        break;
      case 64: /* template_if ::= template_some_if template_list KEY_ELSE template_list KEY_ENDIF */
#line 326 "./templatelang.yxx"
{ tmplgyylhsminor.tmplgyy24 = new TemplateIf(tmplgyymsp[-4].minor.tmplgyy127, tmplgyymsp[-3].minor.tmplgyy127, tmplgyymsp[-1].minor.tmplgyy127); }
#line 1979 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,40,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,39,&tmplgyymsp[0].minor);
  tmplgyymsp[-4].minor.tmplgyy24 = tmplgyylhsminor.tmplgyy24;
        break;
      case 65: /* template_if ::= template_some_if KEY_ELSE template_list KEY_ENDIF */
#line 328 "./templatelang.yxx"
{ tmplgyylhsminor.tmplgyy24 = new TemplateIf(tmplgyymsp[-3].minor.tmplgyy127, 0, tmplgyymsp[-1].minor.tmplgyy127); }
#line 1987 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,40,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,39,&tmplgyymsp[0].minor);
  tmplgyymsp[-3].minor.tmplgyy24 = tmplgyylhsminor.tmplgyy24;
        break;
      case 66: /* template_listappend ::= KEY_LISTAPPENDBEGIN KEY_SPACE STRING KEY_BB_CLOSE */
{  tmplgyy_destructor(tmplgyypParser,41,&tmplgyymsp[-3].minor);
#line 334 "./templatelang.yxx"
{ tmplgyymsp[-3].minor.tmplgyy72 = new TemplateListAppendBegin(tmplgyymsp[-1].minor.tmplgyy0->extractString(), 0); delete tmplgyymsp[-1].minor.tmplgyy0; }
#line 1996 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,6,&tmplgyymsp[0].minor);
}
        break;
      case 67: /* template_listappend ::= KEY_LISTAPPENDBEGIN KEY_SPACE STRING KEY_SPACE STRING KEY_BB_CLOSE */
{  tmplgyy_destructor(tmplgyypParser,41,&tmplgyymsp[-5].minor);
#line 336 "./templatelang.yxx"
{ tmplgyymsp[-5].minor.tmplgyy72 = new TemplateListAppendBegin(tmplgyymsp[-3].minor.tmplgyy0->extractString(), tmplgyymsp[-1].minor.tmplgyy0->extractString()); delete tmplgyymsp[-3].minor.tmplgyy0; delete tmplgyymsp[-1].minor.tmplgyy0; }
#line 2005 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-4].minor);
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,6,&tmplgyymsp[0].minor);
}
        break;
      case 68: /* template_listappend_end ::= KEY_LISTAPPENDEND */
{  tmplgyy_destructor(tmplgyypParser,42,&tmplgyymsp[0].minor);
#line 341 "./templatelang.yxx"
{ tmplgyymsp[0].minor.tmplgyy102 = new TemplateListAppendEnd(); }
#line 2015 "./templatelang.c"
}
        break;
      case 69: /* template_namedsetadd ::= KEY_NAMEDSETADD KEY_SPACE STRING KEY_SPACE STRING KEY_BB_CLOSE */
{  tmplgyy_destructor(tmplgyypParser,43,&tmplgyymsp[-5].minor);
#line 346 "./templatelang.yxx"
{ tmplgyymsp[-5].minor.tmplgyy30 = new TemplateNamedSetAddBegin(tmplgyymsp[-3].minor.tmplgyy0->extractString(), tmplgyymsp[-1].minor.tmplgyy0->extractString()); delete tmplgyymsp[-3].minor.tmplgyy0; delete tmplgyymsp[-1].minor.tmplgyy0; }
#line 2022 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-4].minor);
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,6,&tmplgyymsp[0].minor);
}
        break;
      case 70: /* template_namedsetadd_end ::= KEY_NAMEDSETADDEND */
{  tmplgyy_destructor(tmplgyypParser,44,&tmplgyymsp[0].minor);
#line 351 "./templatelang.yxx"
{ tmplgyymsp[0].minor.tmplgyy56 = new TemplateNamedSetAddEnd(); }
#line 2032 "./templatelang.c"
}
        break;
      case 71: /* template_namedsetremove ::= KEY_NAMEDSETREMOVE KEY_SPACE STRING KEY_SPACE STRING KEY_BB_CLOSE */
{  tmplgyy_destructor(tmplgyypParser,45,&tmplgyymsp[-5].minor);
#line 356 "./templatelang.yxx"
{ tmplgyymsp[-5].minor.tmplgyy15 = new TemplateNamedSetRemoveBegin(tmplgyymsp[-3].minor.tmplgyy0->extractString(), tmplgyymsp[-1].minor.tmplgyy0->extractString()); delete tmplgyymsp[-3].minor.tmplgyy0; delete tmplgyymsp[-1].minor.tmplgyy0; }
#line 2039 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-4].minor);
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,6,&tmplgyymsp[0].minor);
}
        break;
      case 72: /* template_namedsetremove_end ::= KEY_NAMEDSETREMOVEEND */
{  tmplgyy_destructor(tmplgyypParser,46,&tmplgyymsp[0].minor);
#line 361 "./templatelang.yxx"
{ tmplgyymsp[0].minor.tmplgyy123 = new TemplateNamedSetRemoveEnd(); }
#line 2049 "./templatelang.c"
}
        break;
      case 73: /* template_setvarnow ::= KEY_SETVAR KEY_SPACE STRING KEY_SPACE STRING KEY_BB_CLOSE */
{  tmplgyy_destructor(tmplgyypParser,47,&tmplgyymsp[-5].minor);
#line 367 "./templatelang.yxx"
{ tmplgyymsp[-5].minor.tmplgyy2 = new TemplateSetVarNow(tmplgyymsp[-3].minor.tmplgyy0->extractString(), tmplgyymsp[-1].minor.tmplgyy0->extractString()); delete tmplgyymsp[-3].minor.tmplgyy0; delete tmplgyymsp[-1].minor.tmplgyy0; }
#line 2056 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-4].minor);
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,6,&tmplgyymsp[0].minor);
}
        break;
      case 74: /* template_setvar ::= KEY_SETVAR KEY_SPACE STRING KEY_BB_CLOSE */
{  tmplgyy_destructor(tmplgyypParser,47,&tmplgyymsp[-3].minor);
#line 372 "./templatelang.yxx"
{ tmplgyymsp[-3].minor.tmplgyy83 = new TemplateSetVarBegin(tmplgyymsp[-1].minor.tmplgyy0->extractString()); delete tmplgyymsp[-1].minor.tmplgyy0; }
#line 2066 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,6,&tmplgyymsp[0].minor);
}
        break;
      case 75: /* template_setvar_end ::= KEY_SETVAREND */
{  tmplgyy_destructor(tmplgyypParser,48,&tmplgyymsp[0].minor);
#line 378 "./templatelang.yxx"
{ tmplgyymsp[0].minor.tmplgyy84 = new TemplateSetVarEnd(); }
#line 2075 "./templatelang.c"
}
        break;
      case 76: /* template_emitvar ::= KEY_EMITVAR KEY_SPACE STRING KEY_BB_CLOSE */
{  tmplgyy_destructor(tmplgyypParser,49,&tmplgyymsp[-3].minor);
#line 384 "./templatelang.yxx"
{ tmplgyymsp[-3].minor.tmplgyy75 = new TemplateEmitVar(tmplgyymsp[-1].minor.tmplgyy0->extractString()); delete tmplgyymsp[-1].minor.tmplgyy0; }
#line 2082 "./templatelang.c"
  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[-2].minor);
  tmplgyy_destructor(tmplgyypParser,6,&tmplgyymsp[0].minor);
}
        break;
      case 77: /* template_string ::= CHAR */
#line 392 "./templatelang.yxx"
{ tmplgyylhsminor.tmplgyy14 = new TemplateString(new std::string(1, tmplgyymsp[0].minor.tmplgyy0->cChar)); delete tmplgyymsp[0].minor.tmplgyy0; }
#line 2090 "./templatelang.c"
  tmplgyymsp[0].minor.tmplgyy14 = tmplgyylhsminor.tmplgyy14;
        break;
      case 78: /* template_string ::= IDENTIFIER */
#line 394 "./templatelang.yxx"
{ tmplgyylhsminor.tmplgyy14 = new TemplateString(tmplgyymsp[0].minor.tmplgyy0->extractString()); delete tmplgyymsp[0].minor.tmplgyy0; }
#line 2096 "./templatelang.c"
  tmplgyymsp[0].minor.tmplgyy14 = tmplgyylhsminor.tmplgyy14;
        break;
      case 79: /* template_string ::= KEY_BARE_COUNTER */
{  tmplgyy_destructor(tmplgyypParser,29,&tmplgyymsp[0].minor);
#line 396 "./templatelang.yxx"
{ tmplgyymsp[0].minor.tmplgyy14 = new TemplateString(new std::string("counter")); }
#line 2103 "./templatelang.c"
}
        break;
      case 80: /* template_string ::= KEY_BARE_FEATURE */
{  tmplgyy_destructor(tmplgyypParser,26,&tmplgyymsp[0].minor);
#line 398 "./templatelang.yxx"
{ tmplgyymsp[0].minor.tmplgyy14 = new TemplateString(new std::string("feature")); }
#line 2110 "./templatelang.c"
}
        break;
      case 81: /* template_string ::= KEY_BARE_FEATURENOMANGLE */
{  tmplgyy_destructor(tmplgyypParser,27,&tmplgyymsp[0].minor);
#line 400 "./templatelang.yxx"
{ tmplgyymsp[0].minor.tmplgyy14 = new TemplateString(new std::string("featurenomangle")); }
#line 2117 "./templatelang.c"
}
        break;
      case 82: /* template_string ::= KEY_BARE_FORMAT */
{  tmplgyy_destructor(tmplgyypParser,16,&tmplgyymsp[0].minor);
#line 402 "./templatelang.yxx"
{ tmplgyymsp[0].minor.tmplgyy14 = new TemplateString(new std::string("format")); }
#line 2124 "./templatelang.c"
}
        break;
      case 83: /* template_string ::= KEY_BARE_VAR */
{  tmplgyy_destructor(tmplgyypParser,18,&tmplgyymsp[0].minor);
#line 404 "./templatelang.yxx"
{ tmplgyymsp[0].minor.tmplgyy14 = new TemplateString(new std::string("var")); }
#line 2131 "./templatelang.c"
}
        break;
      case 84: /* template_string ::= KEY_BARE_VARNOMANGLE */
{  tmplgyy_destructor(tmplgyypParser,28,&tmplgyymsp[0].minor);
#line 406 "./templatelang.yxx"
{ tmplgyymsp[0].minor.tmplgyy14 = new TemplateString(new std::string("varnomangle")); }
#line 2138 "./templatelang.c"
}
        break;
      case 85: /* template_string ::= INTEGER */
#line 408 "./templatelang.yxx"
{ tmplgyylhsminor.tmplgyy14 = new TemplateString(new std::string(*tmplgyymsp[0].minor.tmplgyy0->pString)); delete tmplgyymsp[0].minor.tmplgyy0; }
#line 2144 "./templatelang.c"
  tmplgyymsp[0].minor.tmplgyy14 = tmplgyylhsminor.tmplgyy14;
        break;
      case 86: /* template_string ::= STRING */
#line 410 "./templatelang.yxx"
{ std::string *pString = new std::string("\""); *pString += *tmplgyymsp[0].minor.tmplgyy0->pString + "\""; tmplgyylhsminor.tmplgyy14 = new TemplateString(pString); delete tmplgyymsp[0].minor.tmplgyy0; }
#line 2150 "./templatelang.c"
  tmplgyymsp[0].minor.tmplgyy14 = tmplgyylhsminor.tmplgyy14;
        break;
      case 87: /* template_string ::= KEY_SPACE */
{  tmplgyy_destructor(tmplgyypParser,4,&tmplgyymsp[0].minor);
#line 412 "./templatelang.yxx"
{ tmplgyymsp[0].minor.tmplgyy14 = new TemplateString(new std::string(" ")); }
#line 2157 "./templatelang.c"
}
        break;
      case 88: /* template_string ::= KEY_NEWLINE */
{  tmplgyy_destructor(tmplgyypParser,51,&tmplgyymsp[0].minor);
#line 414 "./templatelang.yxx"
{ tmplgyymsp[0].minor.tmplgyy14 = new TemplateString(new std::string("\n")); }
#line 2164 "./templatelang.c"
}
        break;
      default:
        break;
/********** End reduce actions ************************************************/
  };
  assert( tmplgyyruleno<sizeof(tmplgyyRuleInfo)/sizeof(tmplgyyRuleInfo[0]) );
  tmplgyygoto = tmplgyyRuleInfo[tmplgyyruleno].lhs;
  tmplgyysize = tmplgyyRuleInfo[tmplgyyruleno].nrhs;
  tmplgyyact = tmplgyy_find_reduce_action(tmplgyymsp[-tmplgyysize].stateno,(TMPLGYYCODETYPE)tmplgyygoto);
  if( tmplgyyact <= TMPLGYY_MAX_SHIFTREDUCE ){
    if( tmplgyyact>TMPLGYY_MAX_SHIFT ){
      tmplgyyact += TMPLGYY_MIN_REDUCE - TMPLGYY_MIN_SHIFTREDUCE;
    }
    tmplgyymsp -= tmplgyysize-1;
    tmplgyypParser->tmplgyytos = tmplgyymsp;
    tmplgyymsp->stateno = (TMPLGYYACTIONTYPE)tmplgyyact;
    tmplgyymsp->major = (TMPLGYYCODETYPE)tmplgyygoto;
    tmplgyyTraceShift(tmplgyypParser, tmplgyyact);
  }else{
    assert( tmplgyyact == TMPLGYY_ACCEPT_ACTION );
    tmplgyypParser->tmplgyytos -= tmplgyysize;
    tmplgyy_accept(tmplgyypParser);
  }
}

/*
** The following code executes when the parse fails
*/
#ifndef TMPLGYYNOERRORRECOVERY
static void tmplgyy_parse_failed(
  tmplgyyParser *tmplgyypParser           /* The parser */
){
  TemplateLangParserARG_FETCH;
#ifndef NDEBUG
  if( tmplgyyTraceFILE ){
    fprintf(tmplgyyTraceFILE,"%sFail!\n",tmplgyyTracePrompt);
  }
#endif
  while( tmplgyypParser->tmplgyytos>tmplgyypParser->tmplgyystack ) tmplgyy_pop_parser_stack(tmplgyypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser fails */
/************ Begin %parse_failure code ***************************************/
#line 115 "./templatelang.yxx"

  pEE->bSyntaxError = true;
#line 2211 "./templatelang.c"
/************ End %parse_failure code *****************************************/
  TemplateLangParserARG_STORE; /* Suppress warning about unused %extra_argument variable */
}
#endif /* TMPLGYYNOERRORRECOVERY */

/*
** The following code executes when a syntax error first occurs.
*/
static void tmplgyy_syntax_error(
  tmplgyyParser *tmplgyypParser,           /* The parser */
  int tmplgyymajor,                   /* The major type of the error token */
  TemplateLangParserTOKENTYPE tmplgyyminor         /* The minor type of the error token */
){
  TemplateLangParserARG_FETCH;
#define TOKEN tmplgyyminor
/************ Begin %syntax_error code ****************************************/
#line 106 "./templatelang.yxx"

  UNUSED(tmplgyymajor);
  UNUSED(tmplgyyminor);
  pEE->bSyntaxError = true;
  std::string errMsg = "syntax error near " + TOKEN->getTokenName();
  pEE->addToError(errMsg);
#line 2235 "./templatelang.c"
/************ End %syntax_error code ******************************************/
  TemplateLangParserARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following is executed when the parser accepts
*/
static void tmplgyy_accept(
  tmplgyyParser *tmplgyypParser           /* The parser */
){
  TemplateLangParserARG_FETCH;
#ifndef NDEBUG
  if( tmplgyyTraceFILE ){
    fprintf(tmplgyyTraceFILE,"%sAccept!\n",tmplgyyTracePrompt);
  }
#endif
#ifndef TMPLGYYNOERRORRECOVERY
  tmplgyypParser->tmplgyyerrcnt = -1;
#endif
  assert( tmplgyypParser->tmplgyytos==tmplgyypParser->tmplgyystack );
  /* Here code is inserted which will be executed whenever the
  ** parser accepts */
/*********** Begin %parse_accept code *****************************************/
/*********** End %parse_accept code *******************************************/
  TemplateLangParserARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/* The main parser program.
** The first argument is a pointer to a structure obtained from
** "TemplateLangParserAlloc" which describes the current state of the parser.
** The second argument is the major token number.  The third is
** the minor token.  The fourth optional argument is whatever the
** user wants (and specified in the grammar) and is available for
** use by the action routines.
**
** Inputs:
** <ul>
** <li> A pointer to the parser (an opaque structure.)
** <li> The major token number.
** <li> The minor token number.
** <li> An option argument of a grammar-specified type.
** </ul>
**
** Outputs:
** None.
*/
void TemplateLangParser(
  void *tmplgyyp,                   /* The parser */
  int tmplgyymajor,                 /* The major token code number */
  TemplateLangParserTOKENTYPE tmplgyyminor       /* The value for the token */
  TemplateLangParserARG_PDECL               /* Optional %extra_argument parameter */
){
  TMPLGYYMINORTYPE tmplgyyminorunion;
  unsigned int tmplgyyact;   /* The parser action. */
#if !defined(TMPLGYYERRORSYMBOL) && !defined(TMPLGYYNOERRORRECOVERY)
  int tmplgyyendofinput;     /* True if we are at the end of input */
#endif
#ifdef TMPLGYYERRORSYMBOL
  int tmplgyyerrorhit = 0;   /* True if tmplgyymajor has invoked an error */
#endif
  tmplgyyParser *tmplgyypParser;  /* The parser */

  tmplgyypParser = (tmplgyyParser*)tmplgyyp;
  assert( tmplgyypParser->tmplgyytos!=0 );
#if !defined(TMPLGYYERRORSYMBOL) && !defined(TMPLGYYNOERRORRECOVERY)
  tmplgyyendofinput = (tmplgyymajor==0);
#endif
  TemplateLangParserARG_STORE;

#ifndef NDEBUG
  if( tmplgyyTraceFILE ){
    fprintf(tmplgyyTraceFILE,"%sInput '%s'\n",tmplgyyTracePrompt,tmplgyyTokenName[tmplgyymajor]);
  }
#endif

  do{
    tmplgyyact = tmplgyy_find_shift_action(tmplgyypParser,(TMPLGYYCODETYPE)tmplgyymajor);
    if( tmplgyyact <= TMPLGYY_MAX_SHIFTREDUCE ){
      tmplgyy_shift(tmplgyypParser,tmplgyyact,tmplgyymajor,tmplgyyminor);
#ifndef TMPLGYYNOERRORRECOVERY
      tmplgyypParser->tmplgyyerrcnt--;
#endif
      tmplgyymajor = TMPLGYYNOCODE;
    }else if( tmplgyyact <= TMPLGYY_MAX_REDUCE ){
      tmplgyy_reduce(tmplgyypParser,tmplgyyact-TMPLGYY_MIN_REDUCE);
    }else{
      assert( tmplgyyact == TMPLGYY_ERROR_ACTION );
      tmplgyyminorunion.tmplgyy0 = tmplgyyminor;
#ifdef TMPLGYYERRORSYMBOL
      int tmplgyymx;
#endif
#ifndef NDEBUG
      if( tmplgyyTraceFILE ){
        fprintf(tmplgyyTraceFILE,"%sSyntax Error!\n",tmplgyyTracePrompt);
      }
#endif
#ifdef TMPLGYYERRORSYMBOL
      /* A syntax error has occurred.
      ** The response to an error depends upon whether or not the
      ** grammar defines an error token "ERROR".  
      **
      ** This is what we do if the grammar does define ERROR:
      **
      **  * Call the %syntax_error function.
      **
      **  * Begin popping the stack until we enter a state where
      **    it is legal to shift the error symbol, then shift
      **    the error symbol.
      **
      **  * Set the error count to three.
      **
      **  * Begin accepting and shifting new tokens.  No new error
      **    processing will occur until three tokens have been
      **    shifted successfully.
      **
      */
      if( tmplgyypParser->tmplgyyerrcnt<0 ){
        tmplgyy_syntax_error(tmplgyypParser,tmplgyymajor,tmplgyyminor);
      }
      tmplgyymx = tmplgyypParser->tmplgyytos->major;
      if( tmplgyymx==TMPLGYYERRORSYMBOL || tmplgyyerrorhit ){
#ifndef NDEBUG
        if( tmplgyyTraceFILE ){
          fprintf(tmplgyyTraceFILE,"%sDiscard input token %s\n",
             tmplgyyTracePrompt,tmplgyyTokenName[tmplgyymajor]);
        }
#endif
        tmplgyy_destructor(tmplgyypParser, (TMPLGYYCODETYPE)tmplgyymajor, &tmplgyyminorunion);
        tmplgyymajor = TMPLGYYNOCODE;
      }else{
        while( tmplgyypParser->tmplgyytos >= tmplgyypParser->tmplgyystack
            && tmplgyymx != TMPLGYYERRORSYMBOL
            && (tmplgyyact = tmplgyy_find_reduce_action(
                        tmplgyypParser->tmplgyytos->stateno,
                        TMPLGYYERRORSYMBOL)) >= TMPLGYY_MIN_REDUCE
        ){
          tmplgyy_pop_parser_stack(tmplgyypParser);
        }
        if( tmplgyypParser->tmplgyytos < tmplgyypParser->tmplgyystack || tmplgyymajor==0 ){
          tmplgyy_destructor(tmplgyypParser,(TMPLGYYCODETYPE)tmplgyymajor,&tmplgyyminorunion);
          tmplgyy_parse_failed(tmplgyypParser);
#ifndef TMPLGYYNOERRORRECOVERY
          tmplgyypParser->tmplgyyerrcnt = -1;
#endif
          tmplgyymajor = TMPLGYYNOCODE;
        }else if( tmplgyymx!=TMPLGYYERRORSYMBOL ){
          tmplgyy_shift(tmplgyypParser,tmplgyyact,TMPLGYYERRORSYMBOL,tmplgyyminor);
        }
      }
      tmplgyypParser->tmplgyyerrcnt = 3;
      tmplgyyerrorhit = 1;
#elif defined(TMPLGYYNOERRORRECOVERY)
      /* If the TMPLGYYNOERRORRECOVERY macro is defined, then do not attempt to
      ** do any kind of error recovery.  Instead, simply invoke the syntax
      ** error routine and continue going as if nothing had happened.
      **
      ** Applications can set this macro (for example inside %include) if
      ** they intend to abandon the parse upon the first syntax error seen.
      */
      tmplgyy_syntax_error(tmplgyypParser,tmplgyymajor, tmplgyyminor);
      tmplgyy_destructor(tmplgyypParser,(TMPLGYYCODETYPE)tmplgyymajor,&tmplgyyminorunion);
      tmplgyymajor = TMPLGYYNOCODE;
      
#else  /* TMPLGYYERRORSYMBOL is not defined */
      /* This is what we do if the grammar does not define ERROR:
      **
      **  * Report an error message, and throw away the input token.
      **
      **  * If the input token is $, then fail the parse.
      **
      ** As before, subsequent error messages are suppressed until
      ** three input tokens have been successfully shifted.
      */
      if( tmplgyypParser->tmplgyyerrcnt<=0 ){
        tmplgyy_syntax_error(tmplgyypParser,tmplgyymajor, tmplgyyminor);
      }
      tmplgyypParser->tmplgyyerrcnt = 3;
      tmplgyy_destructor(tmplgyypParser,(TMPLGYYCODETYPE)tmplgyymajor,&tmplgyyminorunion);
      if( tmplgyyendofinput ){
        tmplgyy_parse_failed(tmplgyypParser);
#ifndef TMPLGYYNOERRORRECOVERY
        tmplgyypParser->tmplgyyerrcnt = -1;
#endif
      }
      tmplgyymajor = TMPLGYYNOCODE;
#endif
    }
  }while( tmplgyymajor!=TMPLGYYNOCODE && tmplgyypParser->tmplgyytos>tmplgyypParser->tmplgyystack );
#ifndef NDEBUG
  if( tmplgyyTraceFILE ){
    tmplgyyStackEntry *i;
    char cDiv = '[';
    fprintf(tmplgyyTraceFILE,"%sReturn. Stack=",tmplgyyTracePrompt);
    for(i=&tmplgyypParser->tmplgyystack[1]; i<=tmplgyypParser->tmplgyytos; i++){
      fprintf(tmplgyyTraceFILE,"%c%s", cDiv, tmplgyyTokenName[i->major]);
      cDiv = ' ';
    }
    fprintf(tmplgyyTraceFILE,"]\n");
  }
#endif
  return;
}
