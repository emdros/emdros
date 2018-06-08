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
#line 1 "./json.yxx"


/*
 * json.yxx
 *
 * JSON parser
 *
 * Ulrik Sandborg-Petersen
 * Created: 7/28-2008
 * Last update: 5/11-2018
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

#include <cassert>
#include <json_classes.h>
#include <mql_yylex.h>
#include <cstdlib>
#include <string>

#undef TOKEN

#undef JSONYYCODETYPE
#undef JSONYYNOCODE
#undef JSONYYACTIONTYPE
#undef JSONYYNSTATE
#undef JSONYYNRULE
#undef JSONYY_ACTTAB_COUNT
#undef JSONYY_SHIFT_USE_DFLT
#undef JSONYY_SHIFT_COUNT
#undef JSONYY_SHIFT_MIN
#undef JSONYY_SHIFT_MAX
#undef JSONYY_REDUCE_USE_DFLT
#undef JSONYY_REDUCE_COUNT
#undef JSONYY_REDUCE_MIN
#undef JSONYY_REDUCE_MAX

#line 139 "./json.c"
/**************** End of %include directives **********************************/
/* These constants specify the various numeric values for terminal symbols
** in a format understandable to "makeheaders".  This section is blank unless
** "lemon" is run with the "-m" command-line option.
***************** Begin makeheaders token definitions *************************/
/**************** End makeheaders token definitions ***************************/

/* The next sections is a series of control #defines.
** various aspects of the generated parser.
**    JSONYYCODETYPE         is the data type used to store the integer codes
**                       that represent terminal and non-terminal symbols.
**                       "unsigned char" is used if there are fewer than
**                       256 symbols.  Larger types otherwise.
**    JSONYYNOCODE           is a number of type JSONYYCODETYPE that is not used for
**                       any terminal or nonterminal symbol.
**    JSONYYFALLBACK         If defined, this indicates that one or more tokens
**                       (also known as: "terminal symbols") have fall-back
**                       values which should be used if the original symbol
**                       would not parse.  This permits keywords to sometimes
**                       be used as identifiers, for example.
**    JSONYYACTIONTYPE       is the data type used for "action codes" - numbers
**                       that indicate what to do in response to the next
**                       token.
**    JSONParserTOKENTYPE     is the data type used for minor type for terminal
**                       symbols.  Background: A "minor type" is a semantic
**                       value associated with a terminal or non-terminal
**                       symbols.  For example, for an "ID" terminal symbol,
**                       the minor type might be the name of the identifier.
**                       Each non-terminal can have a different minor type.
**                       Terminal symbols all have the same minor type, though.
**                       This macros defines the minor type for terminal 
**                       symbols.
**    JSONYYMINORTYPE        is the data type used for all minor types.
**                       This is typically a union of many types, one of
**                       which is JSONParserTOKENTYPE.  The entry in the union
**                       for terminal symbols is called "jsonyy0".
**    JSONYYSTACKDEPTH       is the maximum depth of the parser's stack.  If
**                       zero the stack is dynamically sized using realloc()
**    JSONParserARG_SDECL     A static variable declaration for the %extra_argument
**    JSONParserARG_PDECL     A parameter declaration for the %extra_argument
**    JSONParserARG_STORE     Code to store %extra_argument into jsonyypParser
**    JSONParserARG_FETCH     Code to extract %extra_argument from jsonyypParser
**    JSONYYERRORSYMBOL      is the code number of the error symbol.  If not
**                       defined, then do no error processing.
**    JSONYYNSTATE           the combined number of states.
**    JSONYYNRULE            the number of rules in the grammar
**    JSONYY_MAX_SHIFT       Maximum value for shift actions
**    JSONYY_MIN_SHIFTREDUCE Minimum value for shift-reduce actions
**    JSONYY_MAX_SHIFTREDUCE Maximum value for shift-reduce actions
**    JSONYY_MIN_REDUCE      Maximum value for reduce actions
**    JSONYY_ERROR_ACTION    The jsonyy_action[] code for syntax error
**    JSONYY_ACCEPT_ACTION   The jsonyy_action[] code for accept
**    JSONYY_NO_ACTION       The jsonyy_action[] code for no-op
*/
#ifndef INTERFACE
# define INTERFACE 1
#endif
/************* Begin control #defines *****************************************/
#define JSONYYCODETYPE unsigned char
#define JSONYYNOCODE 26
#define JSONYYACTIONTYPE unsigned char
#define JSONParserTOKENTYPE Token*
typedef union {
  int jsonyyinit;
  JSONParserTOKENTYPE jsonyy0;
  JSONValue* jsonyy22;
  JSONKeyValuePair* jsonyy27;
  int jsonyy42;
  JSONListElement* jsonyy47;
} JSONYYMINORTYPE;
#ifndef JSONYYSTACKDEPTH
#define JSONYYSTACKDEPTH 100
#endif
#define JSONParserARG_SDECL JSONExecEnv *pEE;
#define JSONParserARG_PDECL ,JSONExecEnv *pEE
#define JSONParserARG_FETCH JSONExecEnv *pEE = jsonyypParser->pEE
#define JSONParserARG_STORE jsonyypParser->pEE = pEE
#define JSONYYNSTATE             12
#define JSONYYNRULE              23
#define JSONYY_MAX_SHIFT         11
#define JSONYY_MIN_SHIFTREDUCE   32
#define JSONYY_MAX_SHIFTREDUCE   54
#define JSONYY_MIN_REDUCE        55
#define JSONYY_MAX_REDUCE        77
#define JSONYY_ERROR_ACTION      78
#define JSONYY_ACCEPT_ACTION     79
#define JSONYY_NO_ACTION         80
/************* End control #defines *******************************************/

/* Define the jsonyytestcase() macro to be a no-op if is not already defined
** otherwise.
**
** Applications can choose to define jsonyytestcase() in the %include section
** to a macro that can assist in verifying code coverage.  For production
** code the jsonyytestcase() macro should be turned off.  But it is useful
** for testing.
*/
#ifndef jsonyytestcase
# define jsonyytestcase(X)
#endif


/* Next are the tables used to determine what action to take based on the
** current state and lookahead token.  These tables are used to implement
** functions that take a state number and lookahead value and return an
** action integer.  
**
** Suppose the action integer is N.  Then the action is determined as
** follows
**
**   0 <= N <= JSONYY_MAX_SHIFT             Shift N.  That is, push the lookahead
**                                      token onto the stack and goto state N.
**
**   N between JSONYY_MIN_SHIFTREDUCE       Shift to an arbitrary state then
**     and JSONYY_MAX_SHIFTREDUCE           reduce by rule N-JSONYY_MIN_SHIFTREDUCE.
**
**   N between JSONYY_MIN_REDUCE            Reduce by rule N-JSONYY_MIN_REDUCE
**     and JSONYY_MAX_REDUCE
**
**   N == JSONYY_ERROR_ACTION               A syntax error has occurred.
**
**   N == JSONYY_ACCEPT_ACTION              The parser accepts its input.
**
**   N == JSONYY_NO_ACTION                  No such action.  Denotes unused
**                                      slots in the jsonyy_action[] table.
**
** The action table is constructed as a single large table named jsonyy_action[].
** Given state S and lookahead X, the action is computed as either:
**
**    (A)   N = jsonyy_action[ jsonyy_shift_ofst[S] + X ]
**    (B)   N = jsonyy_default[S]
**
** The (A) formula is preferred.  The B formula is used instead if:
**    (1)  The jsonyy_shift_ofst[S]+X value is out of range, or
**    (2)  jsonyy_lookahead[jsonyy_shift_ofst[S]+X] is not equal to X, or
**    (3)  jsonyy_shift_ofst[S] equal JSONYY_SHIFT_USE_DFLT.
** (Implementation note: JSONYY_SHIFT_USE_DFLT is chosen so that
** JSONYY_SHIFT_USE_DFLT+X will be out of range for all possible lookaheads X.
** Hence only tests (1) and (2) need to be evaluated.)
**
** The formulas above are for computing the action when the lookahead is
** a terminal symbol.  If the lookahead is a non-terminal (as occurs after
** a reduce action) then the jsonyy_reduce_ofst[] array is used in place of
** the jsonyy_shift_ofst[] array and JSONYY_REDUCE_USE_DFLT is used in place of
** JSONYY_SHIFT_USE_DFLT.
**
** The following are the tables generated in this section:
**
**  jsonyy_action[]        A single table containing all actions.
**  jsonyy_lookahead[]     A table containing the lookahead for each entry in
**                     jsonyy_action.  Used to detect hash collisions.
**  jsonyy_shift_ofst[]    For each state, the offset into jsonyy_action for
**                     shifting terminals.
**  jsonyy_reduce_ofst[]   For each state, the offset into jsonyy_action for
**                     shifting non-terminals after a reduce.
**  jsonyy_default[]       Default action for each state.
**
*********** Begin parsing tables **********************************************/
#define JSONYY_ACTTAB_COUNT (59)
static const JSONYYACTIONTYPE jsonyy_action[] = {
 /*     0 */    79,   11,   33,   34,   35,   36,   37,   38,    4,    2,
 /*    10 */    40,   50,    9,    1,   46,   51,   52,   53,   54,   48,
 /*    20 */    33,   34,   35,   36,   37,   38,    8,    4,    6,    5,
 /*    30 */    50,    9,    1,   44,   51,   52,   53,   54,   49,   33,
 /*    40 */    34,   35,   36,   37,   38,   45,   33,   34,   35,   36,
 /*    50 */    37,   38,   10,   47,    7,    3,   43,   39,   55,
};
static const JSONYYCODETYPE jsonyy_lookahead[] = {
 /*     0 */    13,   14,   15,   16,   17,   18,   19,   20,    1,    3,
 /*    10 */     2,    4,    4,    6,    7,    8,    9,   10,   11,   14,
 /*    20 */    15,   16,   17,   18,   19,   20,   22,    1,    3,   24,
 /*    30 */     4,    4,    6,   23,    8,    9,   10,   11,   14,   15,
 /*    40 */    16,   17,   18,   19,   20,   14,   15,   16,   17,   18,
 /*    50 */    19,   20,   22,    7,   21,    5,   23,    2,    0,
};
#define JSONYY_SHIFT_USE_DFLT (59)
#define JSONYY_SHIFT_COUNT    (11)
#define JSONYY_SHIFT_MIN      (0)
#define JSONYY_SHIFT_MAX      (58)
static const unsigned char jsonyy_shift_ofst[] = {
 /*     0 */    26,    7,   26,   26,    8,    6,   27,   25,   46,   50,
 /*    10 */    55,   58,
};
#define JSONYY_REDUCE_USE_DFLT (-14)
#define JSONYY_REDUCE_COUNT (7)
#define JSONYY_REDUCE_MIN   (-13)
#define JSONYY_REDUCE_MAX   (33)
static const signed char jsonyy_reduce_ofst[] = {
 /*     0 */   -13,    5,   24,   31,   33,    4,   10,   30,
};
static const JSONYYACTIONTYPE jsonyy_default[] = {
 /*     0 */    78,   78,   64,   78,   78,   65,   64,   65,   78,   78,
 /*    10 */    78,   78,
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
#ifdef JSONYYFALLBACK
static const JSONYYCODETYPE jsonyyFallback[] = {
};
#endif /* JSONYYFALLBACK */

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
struct jsonyyStackEntry {
  JSONYYACTIONTYPE stateno;  /* The state-number, or reduce action in SHIFTREDUCE */
  JSONYYCODETYPE major;      /* The major token value.  This is the code
                         ** number for the token at this stack level */
  JSONYYMINORTYPE minor;     /* The user-supplied minor token value.  This
                         ** is the value of the token  */
};
typedef struct jsonyyStackEntry jsonyyStackEntry;

/* The state of the parser is completely contained in an instance of
** the following structure */
struct jsonyyParser {
  jsonyyStackEntry *jsonyytos;          /* Pointer to top element of the stack */
#ifdef JSONYYTRACKMAXSTACKDEPTH
  int jsonyyhwm;                    /* High-water mark of the stack */
#endif
#ifndef JSONYYNOERRORRECOVERY
  int jsonyyerrcnt;                 /* Shifts left before out of the error */
#endif
  JSONParserARG_SDECL                /* A place to hold %extra_argument */
#if JSONYYSTACKDEPTH<=0
  int jsonyystksz;                  /* Current side of the stack */
  jsonyyStackEntry *jsonyystack;        /* The parser's stack */
  jsonyyStackEntry jsonyystk0;          /* First stack entry */
#else
  jsonyyStackEntry jsonyystack[JSONYYSTACKDEPTH];  /* The parser's stack */
#endif
};
typedef struct jsonyyParser jsonyyParser;

#ifndef NDEBUG
#include <stdio.h>
static FILE *jsonyyTraceFILE = 0;
static char *jsonyyTracePrompt = 0;
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
void JSONParserTrace(FILE *TraceFILE, char *zTracePrompt){
  jsonyyTraceFILE = TraceFILE;
  jsonyyTracePrompt = zTracePrompt;
  if( jsonyyTraceFILE==0 ) jsonyyTracePrompt = 0;
  else if( jsonyyTracePrompt==0 ) jsonyyTraceFILE = 0;
}
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing shifts, the names of all terminals and nonterminals
** are required.  The following table supplies these names */
static const char *const jsonyyTokenName[] = { 
  "$",             "KEY_BRACE_OPEN",  "KEY_BRACE_CLOSE",  "KEY_COMMA",   
  "STRING",        "KEY_COLON",     "KEY_BRACKET_OPEN",  "KEY_BRACKET_CLOSE",
  "KEY_TRUE",      "KEY_FALSE",     "KEY_NULL",      "INTEGER",     
  "error",         "top_value",     "json_value",    "json_object", 
  "json_list",     "json_boolean",  "json_string",   "json_integer",
  "json_null",     "json_key_value_list",  "opt_comma",     "json_key_value_list_element",
  "json_value_list",
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *const jsonyyRuleName[] = {
 /*   0 */ "top_value ::= json_value",
 /*   1 */ "json_value ::= json_object",
 /*   2 */ "json_value ::= json_list",
 /*   3 */ "json_value ::= json_boolean",
 /*   4 */ "json_value ::= json_string",
 /*   5 */ "json_value ::= json_integer",
 /*   6 */ "json_value ::= json_null",
 /*   7 */ "json_object ::= KEY_BRACE_OPEN json_key_value_list opt_comma KEY_BRACE_CLOSE",
 /*   8 */ "json_object ::= KEY_BRACE_OPEN KEY_BRACE_CLOSE",
 /*   9 */ "opt_comma ::= KEY_COMMA",
 /*  10 */ "opt_comma ::=",
 /*  11 */ "json_key_value_list ::= json_key_value_list_element",
 /*  12 */ "json_key_value_list ::= json_key_value_list KEY_COMMA json_key_value_list_element",
 /*  13 */ "json_key_value_list_element ::= STRING KEY_COLON json_value",
 /*  14 */ "json_list ::= KEY_BRACKET_OPEN KEY_BRACKET_CLOSE",
 /*  15 */ "json_list ::= KEY_BRACKET_OPEN json_value_list opt_comma KEY_BRACKET_CLOSE",
 /*  16 */ "json_value_list ::= json_value",
 /*  17 */ "json_value_list ::= json_value_list KEY_COMMA json_value",
 /*  18 */ "json_string ::= STRING",
 /*  19 */ "json_boolean ::= KEY_TRUE",
 /*  20 */ "json_boolean ::= KEY_FALSE",
 /*  21 */ "json_null ::= KEY_NULL",
 /*  22 */ "json_integer ::= INTEGER",
};
#endif /* NDEBUG */


#if JSONYYSTACKDEPTH<=0
/*
** Try to increase the size of the parser stack.  Return the number
** of errors.  Return 0 on success.
*/
static int jsonyyGrowStack(jsonyyParser *p){
  int newSize;
  int idx;
  jsonyyStackEntry *pNew;

  newSize = p->jsonyystksz*2 + 100;
  idx = p->jsonyytos ? (int)(p->jsonyytos - p->jsonyystack) : 0;
  if( p->jsonyystack==&p->jsonyystk0 ){
    pNew = malloc(newSize*sizeof(pNew[0]));
    if( pNew ) pNew[0] = p->jsonyystk0;
  }else{
    pNew = realloc(p->jsonyystack, newSize*sizeof(pNew[0]));
  }
  if( pNew ){
    p->jsonyystack = pNew;
    p->jsonyytos = &p->jsonyystack[idx];
#ifndef NDEBUG
    if( jsonyyTraceFILE ){
      fprintf(jsonyyTraceFILE,"%sStack grows from %d to %d entries.\n",
              jsonyyTracePrompt, p->jsonyystksz, newSize);
    }
#endif
    p->jsonyystksz = newSize;
  }
  return pNew==0; 
}
#endif

/* Datatype of the argument to the memory allocated passed as the
** second argument to JSONParserAlloc() below.  This can be changed by
** putting an appropriate #define in the %include section of the input
** grammar.
*/
#ifndef JSONYYMALLOCARGTYPE
# define JSONYYMALLOCARGTYPE size_t
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
** to JSONParser and JSONParserFree.
*/
void *JSONParserAlloc(void *(*mallocProc)(JSONYYMALLOCARGTYPE)){
  jsonyyParser *pParser;
  pParser = (jsonyyParser*)(*mallocProc)( (JSONYYMALLOCARGTYPE)sizeof(jsonyyParser) );
  if( pParser ){
#ifdef JSONYYTRACKMAXSTACKDEPTH
    pParser->jsonyyhwm = 0;
#endif
#if JSONYYSTACKDEPTH<=0
    pParser->jsonyytos = NULL;
    pParser->jsonyystack = NULL;
    pParser->jsonyystksz = 0;
    if( jsonyyGrowStack(pParser) ){
      pParser->jsonyystack = &pParser->jsonyystk0;
      pParser->jsonyystksz = 1;
    }
#endif
#ifndef JSONYYNOERRORRECOVERY
    pParser->jsonyyerrcnt = -1;
#endif
    pParser->jsonyytos = pParser->jsonyystack;
    pParser->jsonyystack[0].stateno = 0;
    pParser->jsonyystack[0].major = 0;
  }
  return pParser;
}

/* The following function deletes the "minor type" or semantic value
** associated with a symbol.  The symbol can be either a terminal
** or nonterminal. "jsonyymajor" is the symbol code, and "jsonyypminor" is
** a pointer to the value to be deleted.  The code used to do the 
** deletions is derived from the %destructor and/or %token_destructor
** directives of the input grammar.
*/
static void jsonyy_destructor(
  jsonyyParser *jsonyypParser,    /* The parser */
  JSONYYCODETYPE jsonyymajor,     /* Type code for object to destroy */
  JSONYYMINORTYPE *jsonyypminor   /* The object to be destroyed */
){
  JSONParserARG_FETCH;
  switch( jsonyymajor ){
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
    case 1: /* KEY_BRACE_OPEN */
    case 2: /* KEY_BRACE_CLOSE */
    case 3: /* KEY_COMMA */
    case 4: /* STRING */
    case 5: /* KEY_COLON */
    case 6: /* KEY_BRACKET_OPEN */
    case 7: /* KEY_BRACKET_CLOSE */
    case 8: /* KEY_TRUE */
    case 9: /* KEY_FALSE */
    case 10: /* KEY_NULL */
    case 11: /* INTEGER */
{
#line 114 "./json.yxx"
 
	if (!pEE->pLexer->isLocallyAllocatedToken((jsonyypminor->jsonyy0))) {
		deleteToken((jsonyypminor->jsonyy0)); 
	}

#line 602 "./json.c"
}
      break;
    case 13: /* top_value */
    case 14: /* json_value */
{
#line 141 "./json.yxx"
 delete((jsonyypminor->jsonyy22)); 
#line 610 "./json.c"
}
      break;
    case 15: /* json_object */
    case 16: /* json_list */
{
#line 154 "./json.yxx"
delete((jsonyypminor->jsonyy22)); 
#line 618 "./json.c"
}
      break;
    case 17: /* json_boolean */
    case 18: /* json_string */
    case 19: /* json_integer */
    case 20: /* json_null */
{
#line 211 "./json.yxx"
delete((jsonyypminor->jsonyy22));
#line 628 "./json.c"
}
      break;
    case 21: /* json_key_value_list */
    case 23: /* json_key_value_list_element */
{
#line 167 "./json.yxx"
delete((jsonyypminor->jsonyy27));
#line 636 "./json.c"
}
      break;
    case 22: /* opt_comma */
{
#line 161 "./json.yxx"
;
#line 643 "./json.c"
}
      break;
    case 24: /* json_value_list */
{
#line 191 "./json.yxx"
delete((jsonyypminor->jsonyy47)); 
#line 650 "./json.c"
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
static void jsonyy_pop_parser_stack(jsonyyParser *pParser){
  jsonyyStackEntry *jsonyytos;
  assert( pParser->jsonyytos!=0 );
  assert( pParser->jsonyytos > pParser->jsonyystack );
  jsonyytos = pParser->jsonyytos--;
#ifndef NDEBUG
  if( jsonyyTraceFILE ){
    fprintf(jsonyyTraceFILE,"%sPopping %s\n",
      jsonyyTracePrompt,
      jsonyyTokenName[jsonyytos->major]);
  }
#endif
  jsonyy_destructor(pParser, jsonyytos->major, &jsonyytos->minor);
}

/* 
** Deallocate and destroy a parser.  Destructors are called for
** all stack elements before shutting the parser down.
**
** If the JSONYYPARSEFREENEVERNULL macro exists (for example because it
** is defined in a %include section of the input grammar) then it is
** assumed that the input pointer is never NULL.
*/
void JSONParserFree(
  void *p,                    /* The parser to be deleted */
  void (*freeProc)(void*)     /* Function used to reclaim memory */
){
  jsonyyParser *pParser = (jsonyyParser*)p;
#ifndef JSONYYPARSEFREENEVERNULL
  if( pParser==0 ) return;
#endif
  while( pParser->jsonyytos>pParser->jsonyystack ) jsonyy_pop_parser_stack(pParser);
#if JSONYYSTACKDEPTH<=0
  if( pParser->jsonyystack!=&pParser->jsonyystk0 ) free(pParser->jsonyystack);
#endif
  (*freeProc)((void*)pParser);
}

/*
** Return the peak depth of the stack for a parser.
*/
#ifdef JSONYYTRACKMAXSTACKDEPTH
int JSONParserStackPeak(void *p){
  jsonyyParser *pParser = (jsonyyParser*)p;
  return pParser->jsonyyhwm;
}
#endif

/*
** Find the appropriate action for a parser given the terminal
** look-ahead token iLookAhead.
*/
static unsigned int jsonyy_find_shift_action(
  jsonyyParser *pParser,        /* The parser */
  JSONYYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
  int stateno = pParser->jsonyytos->stateno;
 
  if( stateno>=JSONYY_MIN_REDUCE ) return stateno;
  assert( stateno <= JSONYY_SHIFT_COUNT );
  do{
    i = jsonyy_shift_ofst[stateno];
    assert( iLookAhead!=JSONYYNOCODE );
    i += iLookAhead;
    if( i<0 || i>=JSONYY_ACTTAB_COUNT || jsonyy_lookahead[i]!=iLookAhead ){
#ifdef JSONYYFALLBACK
      JSONYYCODETYPE iFallback;            /* Fallback token */
      if( iLookAhead<sizeof(jsonyyFallback)/sizeof(jsonyyFallback[0])
             && (iFallback = jsonyyFallback[iLookAhead])!=0 ){
#ifndef NDEBUG
        if( jsonyyTraceFILE ){
          fprintf(jsonyyTraceFILE, "%sFALLBACK %s => %s\n",
             jsonyyTracePrompt, jsonyyTokenName[iLookAhead], jsonyyTokenName[iFallback]);
        }
#endif
        assert( jsonyyFallback[iFallback]==0 ); /* Fallback loop must terminate */
        iLookAhead = iFallback;
        continue;
      }
#endif
#ifdef JSONYYWILDCARD
      {
        int j = i - iLookAhead + JSONYYWILDCARD;
        if( 
#if JSONYY_SHIFT_MIN+JSONYYWILDCARD<0
          j>=0 &&
#endif
#if JSONYY_SHIFT_MAX+JSONYYWILDCARD>=JSONYY_ACTTAB_COUNT
          j<JSONYY_ACTTAB_COUNT &&
#endif
          jsonyy_lookahead[j]==JSONYYWILDCARD && iLookAhead>0
        ){
#ifndef NDEBUG
          if( jsonyyTraceFILE ){
            fprintf(jsonyyTraceFILE, "%sWILDCARD %s => %s\n",
               jsonyyTracePrompt, jsonyyTokenName[iLookAhead],
               jsonyyTokenName[JSONYYWILDCARD]);
          }
#endif /* NDEBUG */
          return jsonyy_action[j];
        }
      }
#endif /* JSONYYWILDCARD */
      return jsonyy_default[stateno];
    }else{
      return jsonyy_action[i];
    }
  }while(1);
}

/*
** Find the appropriate action for a parser given the non-terminal
** look-ahead token iLookAhead.
*/
static int jsonyy_find_reduce_action(
  int stateno,              /* Current state number */
  JSONYYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
#ifdef JSONYYERRORSYMBOL
  if( stateno>JSONYY_REDUCE_COUNT ){
    return jsonyy_default[stateno];
  }
#else
  assert( stateno<=JSONYY_REDUCE_COUNT );
#endif
  i = jsonyy_reduce_ofst[stateno];
  assert( i!=JSONYY_REDUCE_USE_DFLT );
  assert( iLookAhead!=JSONYYNOCODE );
  i += iLookAhead;
#ifdef JSONYYERRORSYMBOL
  if( i<0 || i>=JSONYY_ACTTAB_COUNT || jsonyy_lookahead[i]!=iLookAhead ){
    return jsonyy_default[stateno];
  }
#else
  assert( i>=0 && i<JSONYY_ACTTAB_COUNT );
  assert( jsonyy_lookahead[i]==iLookAhead );
#endif
  return jsonyy_action[i];
}

/*
** The following routine is called if the stack overflows.
*/
static void jsonyyStackOverflow(jsonyyParser *jsonyypParser){
   JSONParserARG_FETCH;
#ifndef NDEBUG
   if( jsonyyTraceFILE ){
     fprintf(jsonyyTraceFILE,"%sStack Overflow!\n",jsonyyTracePrompt);
   }
#endif
   while( jsonyypParser->jsonyytos>jsonyypParser->jsonyystack ) jsonyy_pop_parser_stack(jsonyypParser);
   /* Here code is inserted which will execute if the parser
   ** stack every overflows */
/******** Begin %stack_overflow code ******************************************/
/******** End %stack_overflow code ********************************************/
   JSONParserARG_STORE; /* Suppress warning about unused %extra_argument var */
}

/*
** Print tracing information for a SHIFT action
*/
#ifndef NDEBUG
static void jsonyyTraceShift(jsonyyParser *jsonyypParser, int jsonyyNewState){
  if( jsonyyTraceFILE ){
    if( jsonyyNewState<JSONYYNSTATE ){
      fprintf(jsonyyTraceFILE,"%sShift '%s', go to state %d\n",
         jsonyyTracePrompt,jsonyyTokenName[jsonyypParser->jsonyytos->major],
         jsonyyNewState);
    }else{
      fprintf(jsonyyTraceFILE,"%sShift '%s'\n",
         jsonyyTracePrompt,jsonyyTokenName[jsonyypParser->jsonyytos->major]);
    }
  }
}
#else
# define jsonyyTraceShift(X,Y)
#endif

/*
** Perform a shift action.
*/
static void jsonyy_shift(
  jsonyyParser *jsonyypParser,          /* The parser to be shifted */
  int jsonyyNewState,               /* The new state to shift in */
  int jsonyyMajor,                  /* The major token to shift in */
  JSONParserTOKENTYPE jsonyyMinor        /* The minor token to shift in */
){
  jsonyyStackEntry *jsonyytos;
  jsonyypParser->jsonyytos++;
#ifdef JSONYYTRACKMAXSTACKDEPTH
  if( (int)(jsonyypParser->jsonyytos - jsonyypParser->jsonyystack)>jsonyypParser->jsonyyhwm ){
    jsonyypParser->jsonyyhwm++;
    assert( jsonyypParser->jsonyyhwm == (int)(jsonyypParser->jsonyytos - jsonyypParser->jsonyystack) );
  }
#endif
#if JSONYYSTACKDEPTH>0 
  if( jsonyypParser->jsonyytos>=&jsonyypParser->jsonyystack[JSONYYSTACKDEPTH] ){
    jsonyypParser->jsonyytos--;
    jsonyyStackOverflow(jsonyypParser);
    return;
  }
#else
  if( jsonyypParser->jsonyytos>=&jsonyypParser->jsonyystack[jsonyypParser->jsonyystksz] ){
    if( jsonyyGrowStack(jsonyypParser) ){
      jsonyypParser->jsonyytos--;
      jsonyyStackOverflow(jsonyypParser);
      return;
    }
  }
#endif
  if( jsonyyNewState > JSONYY_MAX_SHIFT ){
    jsonyyNewState += JSONYY_MIN_REDUCE - JSONYY_MIN_SHIFTREDUCE;
  }
  jsonyytos = jsonyypParser->jsonyytos;
  jsonyytos->stateno = (JSONYYACTIONTYPE)jsonyyNewState;
  jsonyytos->major = (JSONYYCODETYPE)jsonyyMajor;
  jsonyytos->minor.jsonyy0 = jsonyyMinor;
  jsonyyTraceShift(jsonyypParser, jsonyyNewState);
}

/* The following table contains information about every rule that
** is used during the reduce.
*/
static const struct {
  JSONYYCODETYPE lhs;         /* Symbol on the left-hand side of the rule */
  unsigned char nrhs;     /* Number of right-hand side symbols in the rule */
} jsonyyRuleInfo[] = {
  { 13, 1 },
  { 14, 1 },
  { 14, 1 },
  { 14, 1 },
  { 14, 1 },
  { 14, 1 },
  { 14, 1 },
  { 15, 4 },
  { 15, 2 },
  { 22, 1 },
  { 22, 0 },
  { 21, 1 },
  { 21, 3 },
  { 23, 3 },
  { 16, 2 },
  { 16, 4 },
  { 24, 1 },
  { 24, 3 },
  { 18, 1 },
  { 17, 1 },
  { 17, 1 },
  { 20, 1 },
  { 19, 1 },
};

static void jsonyy_accept(jsonyyParser*);  /* Forward Declaration */

/*
** Perform a reduce action and the shift that must immediately
** follow the reduce.
*/
static void jsonyy_reduce(
  jsonyyParser *jsonyypParser,         /* The parser */
  unsigned int jsonyyruleno        /* Number of the rule by which to reduce */
){
  int jsonyygoto;                     /* The next state */
  int jsonyyact;                      /* The next action */
  jsonyyStackEntry *jsonyymsp;            /* The top of the parser's stack */
  int jsonyysize;                     /* Amount to pop the stack */
  JSONParserARG_FETCH;
  jsonyymsp = jsonyypParser->jsonyytos;
#ifndef NDEBUG
  if( jsonyyTraceFILE && jsonyyruleno<(int)(sizeof(jsonyyRuleName)/sizeof(jsonyyRuleName[0])) ){
    jsonyysize = jsonyyRuleInfo[jsonyyruleno].nrhs;
    fprintf(jsonyyTraceFILE, "%sReduce [%s], go to state %d.\n", jsonyyTracePrompt,
      jsonyyRuleName[jsonyyruleno], jsonyymsp[-jsonyysize].stateno);
  }
#endif /* NDEBUG */

  /* Check that the stack is large enough to grow by a single entry
  ** if the RHS of the rule is empty.  This ensures that there is room
  ** enough on the stack to push the LHS value */
  if( jsonyyRuleInfo[jsonyyruleno].nrhs==0 ){
#ifdef JSONYYTRACKMAXSTACKDEPTH
    if( (int)(jsonyypParser->jsonyytos - jsonyypParser->jsonyystack)>jsonyypParser->jsonyyhwm ){
      jsonyypParser->jsonyyhwm++;
      assert( jsonyypParser->jsonyyhwm == (int)(jsonyypParser->jsonyytos - jsonyypParser->jsonyystack));
    }
#endif
#if JSONYYSTACKDEPTH>0 
    if( jsonyypParser->jsonyytos>=&jsonyypParser->jsonyystack[JSONYYSTACKDEPTH-1] ){
      jsonyyStackOverflow(jsonyypParser);
      return;
    }
#else
    if( jsonyypParser->jsonyytos>=&jsonyypParser->jsonyystack[jsonyypParser->jsonyystksz-1] ){
      if( jsonyyGrowStack(jsonyypParser) ){
        jsonyyStackOverflow(jsonyypParser);
        return;
      }
      jsonyymsp = jsonyypParser->jsonyytos;
    }
#endif
  }

  switch( jsonyyruleno ){
  /* Beginning here are the reduction cases.  A typical example
  ** follows:
  **   case 0:
  **  #line <lineno> <grammarfile>
  **     { ... }           // User supplied code
  **  #line <lineno> <thisfile>
  **     break;
  */
/********** Begin reduce actions **********************************************/
        JSONYYMINORTYPE jsonyylhsminor;
      case 0: /* top_value ::= json_value */
#line 142 "./json.yxx"
{ pEE->setTopValue(jsonyymsp[0].minor.jsonyy22); }
#line 982 "./json.c"
        break;
      case 1: /* json_value ::= json_object */
      case 2: /* json_value ::= json_list */ jsonyytestcase(jsonyyruleno==2);
      case 3: /* json_value ::= json_boolean */ jsonyytestcase(jsonyyruleno==3);
      case 4: /* json_value ::= json_string */ jsonyytestcase(jsonyyruleno==4);
      case 5: /* json_value ::= json_integer */ jsonyytestcase(jsonyyruleno==5);
      case 6: /* json_value ::= json_null */ jsonyytestcase(jsonyyruleno==6);
#line 146 "./json.yxx"
{ jsonyylhsminor.jsonyy22 = jsonyymsp[0].minor.jsonyy22; }
#line 992 "./json.c"
  jsonyymsp[0].minor.jsonyy22 = jsonyylhsminor.jsonyy22;
        break;
      case 7: /* json_object ::= KEY_BRACE_OPEN json_key_value_list opt_comma KEY_BRACE_CLOSE */
#line 156 "./json.yxx"
{ jsonyylhsminor.jsonyy22 = new JSONValue(jsonyymsp[-2].minor.jsonyy27); jsonyymsp[-3].minor.jsonyy0 = 0; jsonyymsp[0].minor.jsonyy0 = 0;}
#line 998 "./json.c"
  jsonyy_destructor(jsonyypParser,22,&jsonyymsp[-1].minor);
  jsonyymsp[-3].minor.jsonyy22 = jsonyylhsminor.jsonyy22;
        break;
      case 8: /* json_object ::= KEY_BRACE_OPEN KEY_BRACE_CLOSE */
#line 158 "./json.yxx"
{ jsonyylhsminor.jsonyy22 = new JSONValue((JSONKeyValuePair*) 0); jsonyymsp[-1].minor.jsonyy0 = 0; jsonyymsp[0].minor.jsonyy0 = 0;}
#line 1005 "./json.c"
  jsonyymsp[-1].minor.jsonyy22 = jsonyylhsminor.jsonyy22;
        break;
      case 9: /* opt_comma ::= KEY_COMMA */
#line 162 "./json.yxx"
{ jsonyylhsminor.jsonyy42 = 0; jsonyymsp[0].minor.jsonyy0 = 0;}
#line 1011 "./json.c"
  jsonyymsp[0].minor.jsonyy42 = jsonyylhsminor.jsonyy42;
        break;
      case 10: /* opt_comma ::= */
#line 163 "./json.yxx"
{ jsonyymsp[1].minor.jsonyy42 = 0; }
#line 1017 "./json.c"
        break;
      case 11: /* json_key_value_list ::= json_key_value_list_element */
#line 169 "./json.yxx"
{ jsonyylhsminor.jsonyy27 = jsonyymsp[0].minor.jsonyy27; }
#line 1022 "./json.c"
  jsonyymsp[0].minor.jsonyy27 = jsonyylhsminor.jsonyy27;
        break;
      case 12: /* json_key_value_list ::= json_key_value_list KEY_COMMA json_key_value_list_element */
#line 173 "./json.yxx"
{ jsonyylhsminor.jsonyy27 = jsonyymsp[0].minor.jsonyy27; jsonyylhsminor.jsonyy27->setNext(jsonyymsp[-2].minor.jsonyy27); jsonyymsp[-1].minor.jsonyy0 = 0; }
#line 1028 "./json.c"
  jsonyymsp[-2].minor.jsonyy27 = jsonyylhsminor.jsonyy27;
        break;
      case 13: /* json_key_value_list_element ::= STRING KEY_COLON json_value */
#line 179 "./json.yxx"
{ jsonyylhsminor.jsonyy27 = new JSONKeyValuePair(jsonyymsp[-2].minor.jsonyy0->extractString(),jsonyymsp[0].minor.jsonyy22,0); delete jsonyymsp[-2].minor.jsonyy0; jsonyymsp[-1].minor.jsonyy0 = 0; }
#line 1034 "./json.c"
  jsonyymsp[-2].minor.jsonyy27 = jsonyylhsminor.jsonyy27;
        break;
      case 14: /* json_list ::= KEY_BRACKET_OPEN KEY_BRACKET_CLOSE */
#line 185 "./json.yxx"
{ jsonyylhsminor.jsonyy22 = new JSONValue((JSONListElement*) 0); jsonyymsp[-1].minor.jsonyy0 = 0; jsonyymsp[0].minor.jsonyy0 = 0; }
#line 1040 "./json.c"
  jsonyymsp[-1].minor.jsonyy22 = jsonyylhsminor.jsonyy22;
        break;
      case 15: /* json_list ::= KEY_BRACKET_OPEN json_value_list opt_comma KEY_BRACKET_CLOSE */
#line 187 "./json.yxx"
{ jsonyylhsminor.jsonyy22 = new JSONValue(jsonyymsp[-2].minor.jsonyy47); jsonyymsp[-3].minor.jsonyy0 = 0; jsonyymsp[0].minor.jsonyy0 = 0; }
#line 1046 "./json.c"
  jsonyy_destructor(jsonyypParser,22,&jsonyymsp[-1].minor);
  jsonyymsp[-3].minor.jsonyy22 = jsonyylhsminor.jsonyy22;
        break;
      case 16: /* json_value_list ::= json_value */
#line 193 "./json.yxx"
{ jsonyylhsminor.jsonyy47 = new JSONListElement(jsonyymsp[0].minor.jsonyy22); }
#line 1053 "./json.c"
  jsonyymsp[0].minor.jsonyy47 = jsonyylhsminor.jsonyy47;
        break;
      case 17: /* json_value_list ::= json_value_list KEY_COMMA json_value */
#line 197 "./json.yxx"
{ jsonyylhsminor.jsonyy47 = new JSONListElement(jsonyymsp[0].minor.jsonyy22); jsonyylhsminor.jsonyy47->setNext(jsonyymsp[-2].minor.jsonyy47); jsonyymsp[-1].minor.jsonyy0 = 0; }
#line 1059 "./json.c"
  jsonyymsp[-2].minor.jsonyy47 = jsonyylhsminor.jsonyy47;
        break;
      case 18: /* json_string ::= STRING */
#line 207 "./json.yxx"
{ jsonyylhsminor.jsonyy22 = new JSONValue(jsonyymsp[0].minor.jsonyy0->extractString()); jsonyymsp[0].minor.jsonyy0->clear(); delete jsonyymsp[0].minor.jsonyy0; }
#line 1065 "./json.c"
  jsonyymsp[0].minor.jsonyy22 = jsonyylhsminor.jsonyy22;
        break;
      case 19: /* json_boolean ::= KEY_TRUE */
#line 213 "./json.yxx"
{ jsonyylhsminor.jsonyy22 = new JSONValue(true); jsonyymsp[0].minor.jsonyy0 = 0;}
#line 1071 "./json.c"
  jsonyymsp[0].minor.jsonyy22 = jsonyylhsminor.jsonyy22;
        break;
      case 20: /* json_boolean ::= KEY_FALSE */
#line 215 "./json.yxx"
{ jsonyylhsminor.jsonyy22 = new JSONValue(false); jsonyymsp[0].minor.jsonyy0 = 0;}
#line 1077 "./json.c"
  jsonyymsp[0].minor.jsonyy22 = jsonyylhsminor.jsonyy22;
        break;
      case 21: /* json_null ::= KEY_NULL */
#line 221 "./json.yxx"
{ jsonyylhsminor.jsonyy22 = new JSONValue(); jsonyymsp[0].minor.jsonyy0 = 0; }
#line 1083 "./json.c"
  jsonyymsp[0].minor.jsonyy22 = jsonyylhsminor.jsonyy22;
        break;
      case 22: /* json_integer ::= INTEGER */
#line 227 "./json.yxx"
{ jsonyylhsminor.jsonyy22 = new JSONValue(jsonyymsp[0].minor.jsonyy0->integer); delete jsonyymsp[0].minor.jsonyy0; }
#line 1089 "./json.c"
  jsonyymsp[0].minor.jsonyy22 = jsonyylhsminor.jsonyy22;
        break;
      default:
        break;
/********** End reduce actions ************************************************/
  };
  assert( jsonyyruleno<sizeof(jsonyyRuleInfo)/sizeof(jsonyyRuleInfo[0]) );
  jsonyygoto = jsonyyRuleInfo[jsonyyruleno].lhs;
  jsonyysize = jsonyyRuleInfo[jsonyyruleno].nrhs;
  jsonyyact = jsonyy_find_reduce_action(jsonyymsp[-jsonyysize].stateno,(JSONYYCODETYPE)jsonyygoto);
  if( jsonyyact <= JSONYY_MAX_SHIFTREDUCE ){
    if( jsonyyact>JSONYY_MAX_SHIFT ){
      jsonyyact += JSONYY_MIN_REDUCE - JSONYY_MIN_SHIFTREDUCE;
    }
    jsonyymsp -= jsonyysize-1;
    jsonyypParser->jsonyytos = jsonyymsp;
    jsonyymsp->stateno = (JSONYYACTIONTYPE)jsonyyact;
    jsonyymsp->major = (JSONYYCODETYPE)jsonyygoto;
    jsonyyTraceShift(jsonyypParser, jsonyyact);
  }else{
    assert( jsonyyact == JSONYY_ACCEPT_ACTION );
    jsonyypParser->jsonyytos -= jsonyysize;
    jsonyy_accept(jsonyypParser);
  }
}

/*
** The following code executes when the parse fails
*/
#ifndef JSONYYNOERRORRECOVERY
static void jsonyy_parse_failed(
  jsonyyParser *jsonyypParser           /* The parser */
){
  JSONParserARG_FETCH;
#ifndef NDEBUG
  if( jsonyyTraceFILE ){
    fprintf(jsonyyTraceFILE,"%sFail!\n",jsonyyTracePrompt);
  }
#endif
  while( jsonyypParser->jsonyytos>jsonyypParser->jsonyystack ) jsonyy_pop_parser_stack(jsonyypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser fails */
/************ Begin %parse_failure code ***************************************/
#line 136 "./json.yxx"

  pEE->bSyntaxError = true;
#line 1136 "./json.c"
/************ End %parse_failure code *****************************************/
  JSONParserARG_STORE; /* Suppress warning about unused %extra_argument variable */
}
#endif /* JSONYYNOERRORRECOVERY */

/*
** The following code executes when a syntax error first occurs.
*/
static void jsonyy_syntax_error(
  jsonyyParser *jsonyypParser,           /* The parser */
  int jsonyymajor,                   /* The major type of the error token */
  JSONParserTOKENTYPE jsonyyminor         /* The minor type of the error token */
){
  JSONParserARG_FETCH;
#define TOKEN jsonyyminor
/************ Begin %syntax_error code ****************************************/
#line 122 "./json.yxx"

  UNUSED(jsonyyminor);
  UNUSED(jsonyymajor);
  pEE->bSyntaxError = true;
  std::string errMsg;
  if (TOKEN == 0) {
     errMsg = "syntax error near <<EOF>>";
  } else {
     errMsg = "syntax error near " + TOKEN->getTokenName();
  }
  pEE->addToError(errMsg);
#line 1165 "./json.c"
/************ End %syntax_error code ******************************************/
  JSONParserARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following is executed when the parser accepts
*/
static void jsonyy_accept(
  jsonyyParser *jsonyypParser           /* The parser */
){
  JSONParserARG_FETCH;
#ifndef NDEBUG
  if( jsonyyTraceFILE ){
    fprintf(jsonyyTraceFILE,"%sAccept!\n",jsonyyTracePrompt);
  }
#endif
#ifndef JSONYYNOERRORRECOVERY
  jsonyypParser->jsonyyerrcnt = -1;
#endif
  assert( jsonyypParser->jsonyytos==jsonyypParser->jsonyystack );
  /* Here code is inserted which will be executed whenever the
  ** parser accepts */
/*********** Begin %parse_accept code *****************************************/
/*********** End %parse_accept code *******************************************/
  JSONParserARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/* The main parser program.
** The first argument is a pointer to a structure obtained from
** "JSONParserAlloc" which describes the current state of the parser.
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
void JSONParser(
  void *jsonyyp,                   /* The parser */
  int jsonyymajor,                 /* The major token code number */
  JSONParserTOKENTYPE jsonyyminor       /* The value for the token */
  JSONParserARG_PDECL               /* Optional %extra_argument parameter */
){
  JSONYYMINORTYPE jsonyyminorunion;
  unsigned int jsonyyact;   /* The parser action. */
#if !defined(JSONYYERRORSYMBOL) && !defined(JSONYYNOERRORRECOVERY)
  int jsonyyendofinput;     /* True if we are at the end of input */
#endif
#ifdef JSONYYERRORSYMBOL
  int jsonyyerrorhit = 0;   /* True if jsonyymajor has invoked an error */
#endif
  jsonyyParser *jsonyypParser;  /* The parser */

  jsonyypParser = (jsonyyParser*)jsonyyp;
  assert( jsonyypParser->jsonyytos!=0 );
#if !defined(JSONYYERRORSYMBOL) && !defined(JSONYYNOERRORRECOVERY)
  jsonyyendofinput = (jsonyymajor==0);
#endif
  JSONParserARG_STORE;

#ifndef NDEBUG
  if( jsonyyTraceFILE ){
    fprintf(jsonyyTraceFILE,"%sInput '%s'\n",jsonyyTracePrompt,jsonyyTokenName[jsonyymajor]);
  }
#endif

  do{
    jsonyyact = jsonyy_find_shift_action(jsonyypParser,(JSONYYCODETYPE)jsonyymajor);
    if( jsonyyact <= JSONYY_MAX_SHIFTREDUCE ){
      jsonyy_shift(jsonyypParser,jsonyyact,jsonyymajor,jsonyyminor);
#ifndef JSONYYNOERRORRECOVERY
      jsonyypParser->jsonyyerrcnt--;
#endif
      jsonyymajor = JSONYYNOCODE;
    }else if( jsonyyact <= JSONYY_MAX_REDUCE ){
      jsonyy_reduce(jsonyypParser,jsonyyact-JSONYY_MIN_REDUCE);
    }else{
      assert( jsonyyact == JSONYY_ERROR_ACTION );
      jsonyyminorunion.jsonyy0 = jsonyyminor;
#ifdef JSONYYERRORSYMBOL
      int jsonyymx;
#endif
#ifndef NDEBUG
      if( jsonyyTraceFILE ){
        fprintf(jsonyyTraceFILE,"%sSyntax Error!\n",jsonyyTracePrompt);
      }
#endif
#ifdef JSONYYERRORSYMBOL
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
      if( jsonyypParser->jsonyyerrcnt<0 ){
        jsonyy_syntax_error(jsonyypParser,jsonyymajor,jsonyyminor);
      }
      jsonyymx = jsonyypParser->jsonyytos->major;
      if( jsonyymx==JSONYYERRORSYMBOL || jsonyyerrorhit ){
#ifndef NDEBUG
        if( jsonyyTraceFILE ){
          fprintf(jsonyyTraceFILE,"%sDiscard input token %s\n",
             jsonyyTracePrompt,jsonyyTokenName[jsonyymajor]);
        }
#endif
        jsonyy_destructor(jsonyypParser, (JSONYYCODETYPE)jsonyymajor, &jsonyyminorunion);
        jsonyymajor = JSONYYNOCODE;
      }else{
        while( jsonyypParser->jsonyytos >= jsonyypParser->jsonyystack
            && jsonyymx != JSONYYERRORSYMBOL
            && (jsonyyact = jsonyy_find_reduce_action(
                        jsonyypParser->jsonyytos->stateno,
                        JSONYYERRORSYMBOL)) >= JSONYY_MIN_REDUCE
        ){
          jsonyy_pop_parser_stack(jsonyypParser);
        }
        if( jsonyypParser->jsonyytos < jsonyypParser->jsonyystack || jsonyymajor==0 ){
          jsonyy_destructor(jsonyypParser,(JSONYYCODETYPE)jsonyymajor,&jsonyyminorunion);
          jsonyy_parse_failed(jsonyypParser);
#ifndef JSONYYNOERRORRECOVERY
          jsonyypParser->jsonyyerrcnt = -1;
#endif
          jsonyymajor = JSONYYNOCODE;
        }else if( jsonyymx!=JSONYYERRORSYMBOL ){
          jsonyy_shift(jsonyypParser,jsonyyact,JSONYYERRORSYMBOL,jsonyyminor);
        }
      }
      jsonyypParser->jsonyyerrcnt = 3;
      jsonyyerrorhit = 1;
#elif defined(JSONYYNOERRORRECOVERY)
      /* If the JSONYYNOERRORRECOVERY macro is defined, then do not attempt to
      ** do any kind of error recovery.  Instead, simply invoke the syntax
      ** error routine and continue going as if nothing had happened.
      **
      ** Applications can set this macro (for example inside %include) if
      ** they intend to abandon the parse upon the first syntax error seen.
      */
      jsonyy_syntax_error(jsonyypParser,jsonyymajor, jsonyyminor);
      jsonyy_destructor(jsonyypParser,(JSONYYCODETYPE)jsonyymajor,&jsonyyminorunion);
      jsonyymajor = JSONYYNOCODE;
      
#else  /* JSONYYERRORSYMBOL is not defined */
      /* This is what we do if the grammar does not define ERROR:
      **
      **  * Report an error message, and throw away the input token.
      **
      **  * If the input token is $, then fail the parse.
      **
      ** As before, subsequent error messages are suppressed until
      ** three input tokens have been successfully shifted.
      */
      if( jsonyypParser->jsonyyerrcnt<=0 ){
        jsonyy_syntax_error(jsonyypParser,jsonyymajor, jsonyyminor);
      }
      jsonyypParser->jsonyyerrcnt = 3;
      jsonyy_destructor(jsonyypParser,(JSONYYCODETYPE)jsonyymajor,&jsonyyminorunion);
      if( jsonyyendofinput ){
        jsonyy_parse_failed(jsonyypParser);
#ifndef JSONYYNOERRORRECOVERY
        jsonyypParser->jsonyyerrcnt = -1;
#endif
      }
      jsonyymajor = JSONYYNOCODE;
#endif
    }
  }while( jsonyymajor!=JSONYYNOCODE && jsonyypParser->jsonyytos>jsonyypParser->jsonyystack );
#ifndef NDEBUG
  if( jsonyyTraceFILE ){
    jsonyyStackEntry *i;
    char cDiv = '[';
    fprintf(jsonyyTraceFILE,"%sReturn. Stack=",jsonyyTracePrompt);
    for(i=&jsonyypParser->jsonyystack[1]; i<=jsonyypParser->jsonyytos; i++){
      fprintf(jsonyyTraceFILE,"%c%s", cDiv, jsonyyTokenName[i->major]);
      cDiv = ' ';
    }
    fprintf(jsonyyTraceFILE,"]\n");
  }
#endif
  return;
}
