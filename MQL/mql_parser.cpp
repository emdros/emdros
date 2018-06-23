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
#line 1 "./mql.yxx"


/*
 * mql.yxx
 *
 * MQL parser
 *
 * Ulrik Petersen
 * Created: 2/27-2001
 * Last update: 6/8-2018
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



#include <cassert>
#include <string_list.h>
#include <cstdlib>
#include <string>
#include <mql_error.h>
#include <mql_types.h>
#include <mql_database_statements.h>
#include <mql_enumeration_statements.h>
#include <mql_features_statements.h>
#include <mql_meta_statements.h>
#include <mql_monads_statements.h>
#include <mql_object_statements.h>
#include <mql_object_type_statements.h>
#include <mql_segment_statements.h>
#include <mql_select_statements.h>
#include <mql_query.h>
#include <mql_extern.h>
#include <mql_execution_environment.h>
#include <mql_yylex.h>

#undef TOKEN

#undef MQLYYCODETYPE
#undef MQLYYNOCODE
#undef MQLYYACTIONTYPE
#undef MQLYYNSTATE
#undef MQLYYNRULE
#undef MQLYY_ACTTAB_COUNT
#undef MQLYY_SHIFT_COUNT
#undef MQLYY_SHIFT_MAX
#undef MQLYY_REDUCE_USE_DFLT
#undef MQLYY_REDUCE_COUNT
#undef MQLYY_REDUCE_MIN
#undef MQLYY_REDUCE_MAX
#undef MYMIN


#line 155 "./mql.c"
/**************** End of %include directives **********************************/
/* These constants specify the various numeric values for terminal symbols
** in a format understandable to "makeheaders".  This section is blank unless
** "lemon" is run with the "-m" command-line option.
***************** Begin makeheaders token definitions *************************/
/**************** End makeheaders token definitions ***************************/

/* The next sections is a series of control #defines.
** various aspects of the generated parser.
**    MQLYYCODETYPE         is the data type used to store the integer codes
**                       that represent terminal and non-terminal symbols.
**                       "unsigned char" is used if there are fewer than
**                       256 symbols.  Larger types otherwise.
**    MQLYYNOCODE           is a number of type MQLYYCODETYPE that is not used for
**                       any terminal or nonterminal symbol.
**    MQLYYFALLBACK         If defined, this indicates that one or more tokens
**                       (also known as: "terminal symbols") have fall-back
**                       values which should be used if the original symbol
**                       would not parse.  This permits keywords to sometimes
**                       be used as identifiers, for example.
**    MQLYYACTIONTYPE       is the data type used for "action codes" - numbers
**                       that indicate what to do in response to the next
**                       token.
**    MQLParserTOKENTYPE     is the data type used for minor type for terminal
**                       symbols.  Background: A "minor type" is a semantic
**                       value associated with a terminal or non-terminal
**                       symbols.  For example, for an "ID" terminal symbol,
**                       the minor type might be the name of the identifier.
**                       Each non-terminal can have a different minor type.
**                       Terminal symbols all have the same minor type, though.
**                       This macros defines the minor type for terminal 
**                       symbols.
**    MQLYYMINORTYPE        is the data type used for all minor types.
**                       This is typically a union of many types, one of
**                       which is MQLParserTOKENTYPE.  The entry in the union
**                       for terminal symbols is called "mqlyy0".
**    MQLYYSTACKDEPTH       is the maximum depth of the parser's stack.  If
**                       zero the stack is dynamically sized using realloc()
**    MQLParserARG_SDECL     A static variable declaration for the %extra_argument
**    MQLParserARG_PDECL     A parameter declaration for the %extra_argument
**    MQLParserARG_STORE     Code to store %extra_argument into mqlyypParser
**    MQLParserARG_FETCH     Code to extract %extra_argument from mqlyypParser
**    MQLYYERRORSYMBOL      is the code number of the error symbol.  If not
**                       defined, then do no error processing.
**    MQLYYNSTATE           the combined number of states.
**    MQLYYNRULE            the number of rules in the grammar
**    MQLYY_MAX_SHIFT       Maximum value for shift actions
**    MQLYY_MIN_SHIFTREDUCE Minimum value for shift-reduce actions
**    MQLYY_MAX_SHIFTREDUCE Maximum value for shift-reduce actions
**    MQLYY_MIN_REDUCE      Maximum value for reduce actions
**    MQLYY_ERROR_ACTION    The mqlyy_action[] code for syntax error
**    MQLYY_ACCEPT_ACTION   The mqlyy_action[] code for accept
**    MQLYY_NO_ACTION       The mqlyy_action[] code for no-op
*/
#ifndef INTERFACE
# define INTERFACE 1
#endif
/************* Begin control #defines *****************************************/
#define MQLYYCODETYPE unsigned short int
#define MQLYYNOCODE 312
#define MQLYYACTIONTYPE unsigned short int
#define MQLParserTOKENTYPE Token*
typedef union {
  int mqlyyinit;
  MQLParserTOKENTYPE mqlyy0;
  GapBlock* mqlyy6;
  IntegerListNode* mqlyy17;
  Blocks* mqlyy18;
  UnorderedGroup* mqlyy24;
  ID_D* mqlyy43;
  ECUpdate* mqlyy46;
  Statement* mqlyy50;
  MonadSetChainElement* mqlyy51;
  BlockString0* mqlyy63;
  FeatureComparison* mqlyy68;
  eRetrieval mqlyy88;
  ObjectBlockString* mqlyy98;
  ObjectBlock* mqlyy101;
  MQLMonadSetElement* mqlyy108;
  BlockString1* mqlyy116;
  eComparisonOp mqlyy130;
  BlockString2* mqlyy169;
  Block* mqlyy218;
  monad_m mqlyy244;
  bool mqlyy263;
  MQLType* mqlyy264;
  FeatureAssignment* mqlyy265;
  ObjectReferenceUsage* mqlyy266;
  Topograph* mqlyy269;
  FFactor* mqlyy272;
  eFocusSpec mqlyy273;
  ObjectSpec* mqlyy275;
  id_d_t mqlyy282;
  FeatureDeclaration* mqlyy289;
  eFirstLast mqlyy296;
  MonadSetRelationClause* mqlyy299;
  OptGapBlock* mqlyy303;
  StringListNode* mqlyy336;
  eObjectRangeType mqlyy342;
  AggregateFeature* mqlyy344;
  eSetOperator mqlyy346;
  eMonadSetRelationOperation mqlyy360;
  GrammarFeature* mqlyy366;
  FeatureUpdate* mqlyy380;
  Expression* mqlyy413;
  MaxRange* mqlyy448;
  ECDeclaration* mqlyy455;
  int mqlyy466;
  emdros_int64* mqlyy473;
  ObjectSpecNoOT* mqlyy481;
  BlockString* mqlyy501;
  emdros_int64 mqlyy503;
  FFeatures* mqlyy520;
  eMonadUniquenessType mqlyy527;
  long mqlyy529;
  sheaf_return_type_pair* mqlyy551;
  eUniverseOrSubstrate mqlyy569;
  Power* mqlyy594;
  UsingRange* mqlyy600;
  FTerm* mqlyy601;
  Value* mqlyy616;
} MQLYYMINORTYPE;
#ifndef MQLYYSTACKDEPTH
#define MQLYYSTACKDEPTH 2000
#endif
#define MQLParserARG_SDECL MQLExecEnv *pEE;
#define MQLParserARG_PDECL ,MQLExecEnv *pEE
#define MQLParserARG_FETCH MQLExecEnv *pEE = mqlyypParser->pEE
#define MQLParserARG_STORE mqlyypParser->pEE = pEE
#define MQLYYNSTATE             422
#define MQLYYNRULE              385
#define MQLYY_MAX_SHIFT         421
#define MQLYY_MIN_SHIFTREDUCE   748
#define MQLYY_MAX_SHIFTREDUCE   1132
#define MQLYY_MIN_REDUCE        1133
#define MQLYY_MAX_REDUCE        1517
#define MQLYY_ERROR_ACTION      1518
#define MQLYY_ACCEPT_ACTION     1519
#define MQLYY_NO_ACTION         1520
/************* End control #defines *******************************************/

/* Define the mqlyytestcase() macro to be a no-op if is not already defined
** otherwise.
**
** Applications can choose to define mqlyytestcase() in the %include section
** to a macro that can assist in verifying code coverage.  For production
** code the mqlyytestcase() macro should be turned off.  But it is useful
** for testing.
*/
#ifndef mqlyytestcase
# define mqlyytestcase(X)
#endif


/* Next are the tables used to determine what action to take based on the
** current state and lookahead token.  These tables are used to implement
** functions that take a state number and lookahead value and return an
** action integer.  
**
** Suppose the action integer is N.  Then the action is determined as
** follows
**
**   0 <= N <= MQLYY_MAX_SHIFT             Shift N.  That is, push the lookahead
**                                      token onto the stack and goto state N.
**
**   N between MQLYY_MIN_SHIFTREDUCE       Shift to an arbitrary state then
**     and MQLYY_MAX_SHIFTREDUCE           reduce by rule N-MQLYY_MIN_SHIFTREDUCE.
**
**   N between MQLYY_MIN_REDUCE            Reduce by rule N-MQLYY_MIN_REDUCE
**     and MQLYY_MAX_REDUCE
**
**   N == MQLYY_ERROR_ACTION               A syntax error has occurred.
**
**   N == MQLYY_ACCEPT_ACTION              The parser accepts its input.
**
**   N == MQLYY_NO_ACTION                  No such action.  Denotes unused
**                                      slots in the mqlyy_action[] table.
**
** The action table is constructed as a single large table named mqlyy_action[].
** Given state S and lookahead X, the action is computed as either:
**
**    (A)   N = mqlyy_action[ mqlyy_shift_ofst[S] + X ]
**    (B)   N = mqlyy_default[S]
**
** The (A) formula is preferred.  The B formula is used instead if:
**    (1)  The mqlyy_shift_ofst[S]+X value is out of range, or
**    (2)  mqlyy_lookahead[mqlyy_shift_ofst[S]+X] is not equal to X, or
**    (3)  mqlyy_shift_ofst[S] equal MQLYY_SHIFT_USE_DFLT.
** (Implementation note: MQLYY_SHIFT_USE_DFLT is chosen so that
** MQLYY_SHIFT_USE_DFLT+X will be out of range for all possible lookaheads X.
** Hence only tests (1) and (2) need to be evaluated.)
**
** The formulas above are for computing the action when the lookahead is
** a terminal symbol.  If the lookahead is a non-terminal (as occurs after
** a reduce action) then the mqlyy_reduce_ofst[] array is used in place of
** the mqlyy_shift_ofst[] array and MQLYY_REDUCE_USE_DFLT is used in place of
** MQLYY_SHIFT_USE_DFLT.
**
** The following are the tables generated in this section:
**
**  mqlyy_action[]        A single table containing all actions.
**  mqlyy_lookahead[]     A table containing the lookahead for each entry in
**                     mqlyy_action.  Used to detect hash collisions.
**  mqlyy_shift_ofst[]    For each state, the offset into mqlyy_action for
**                     shifting terminals.
**  mqlyy_reduce_ofst[]   For each state, the offset into mqlyy_action for
**                     shifting non-terminals after a reduce.
**  mqlyy_default[]       Default action for each state.
**
*********** Begin parsing tables **********************************************/
#define MQLYY_ACTTAB_COUNT (970)
static const MQLYYACTIONTYPE mqlyy_action[] = {
 /*     0 */  1519,  421,  749,  750,  751,  752,  753,  754,  755,  756,
 /*    10 */   757,  758,  759,  760,  761,  762,  763,  764,  765,  766,
 /*    20 */   767,  768,  769,  770,  771,  772,  773,  774,  775,  776,
 /*    30 */   777,  778,  779,  780,  781,  782,  783,  784,  785,  786,
 /*    40 */   787,  788,  789,  790,  791,  792,  793,  794,  795,  796,
 /*    50 */    41,  130,  863,  337,  346,  188,  410,   67,  834,  409,
 /*    60 */    42,  182, 1043, 1044,  267, 1045,    5,  949,  256,  266,
 /*    70 */   341,  369,  340,  339,  331,  174,  171,  868,  290, 1007,
 /*    80 */     3,  952,  178, 1002,  142, 1107, 1108, 1109, 1110, 1111,
 /*    90 */   829,  109,  868, 1053, 1004,  267, 1045,    5,   77,  256,
 /*   100 */   266,  287,  147,  288,  831,   21,  316, 1101,  167,   50,
 /*   110 */    80, 1041, 1042, 1003,  177,   13, 1107, 1108, 1109, 1110,
 /*   120 */  1111,  831,  267, 1045,    5,  956, 1050,  266,   25,  901,
 /*   130 */   957,   12,  233,  907,  268,    2,  267, 1045,    5,  831,
 /*   140 */   256,  266,  925, 1107, 1108, 1109, 1110, 1111,  895,  896,
 /*   150 */   222, 1040,  304,    2,  924,  913,  989, 1107, 1108, 1109,
 /*   160 */  1110, 1111, 1002, 1312,  891,  275,  114,  931, 1090,  958,
 /*   170 */   982,   76,  988,  267, 1045,    5,  303, 1051,  266,  407,
 /*   180 */   915, 1090,    2,  283,  877,  878,  879,  124,  388,   58,
 /*   190 */    62,  238, 1003,   78, 1107, 1108, 1109, 1110, 1111,   53,
 /*   200 */   864,   64,  403,   78,  118,  209,  207,  115,  396, 1006,
 /*   210 */  1005, 1013, 1011, 1091, 1092, 1093, 1094, 1095, 1096, 1097,
 /*   220 */  1098,   78,  121, 1054, 1055,   78, 1091, 1092, 1093, 1094,
 /*   230 */  1095, 1096, 1097, 1098,  216,   78,  372,  232,  239,  240,
 /*   240 */   960, 1054, 1055,   70,  132,  131,   86,  355,  302,  363,
 /*   250 */   107,  859,  931,  110,  328,    1,   76,   22,  913, 1007,
 /*   260 */   913, 1008,  132,  131,   86,  186,  297,  829,    6,    2,
 /*   270 */  1054, 1055, 1100,  128, 1004, 1075, 1076,  137,   76,  277,
 /*   280 */   342, 1100,  407,  915,  407,  915,  370,  320,  231,  317,
 /*   290 */  1081, 1085,  366,   86,  895,  896,  338,  967,  195,  318,
 /*   300 */  1081, 1085,  370, 1017, 1036,   78, 1086, 1028,  271,  334,
 /*   310 */    78,  323, 1037, 1013,  109,  868,  319,  317, 1081, 1085,
 /*   320 */   319,  317, 1081, 1085,  310,  895,  896,  154,  282,  315,
 /*   330 */   319,  317, 1081, 1085, 1011,  312,  308,  306,  315,    3,
 /*   340 */  1102, 1099,    3, 1054, 1055,  205, 1011,  201,  831, 1102,
 /*   350 */  1099,  347,   66,  999,  895,  896,  370, 1054, 1055, 1018,
 /*   360 */   227,  226,  913,  166,  127,  165,   86,  371,   66,  999,
 /*   370 */  1033, 1106,  229,  230, 1106,  957, 1002,  170, 1021,  168,
 /*   380 */    86,    3,    3,  364, 1019, 1002,  407,  915, 1064,  234,
 /*   390 */   898,  196,  262, 1027,  253, 1121, 1002,  254, 1072, 1073,
 /*   400 */  1074,  365, 1083, 1085, 1028,  412, 1003, 1082, 1085,  849,
 /*   410 */   366, 1029,  930, 1106, 1106, 1003, 1038, 1062, 1063,  365,
 /*   420 */   345,  913,   51,  999, 1119, 1120, 1003,  134,  245,  370,
 /*   430 */   133, 1490,  994,  365,  197, 1038,  255,  258,  928,  222,
 /*   440 */   136,  298,  877,  878,  879,  407,  915,  847,  177,  183,
 /*   450 */   848, 1017,    7,  263,  329,  872,  330,  995,  246,   92,
 /*   460 */     8,  263,  865,   64, 1038,  292,  260,  908, 1060,  930,
 /*   470 */   891,  315,  326,  324,  892,  224,  891,  305, 1021,  889,
 /*   480 */   950,  888, 1089,  887, 1020,  886,  866,  409,  835,  409,
 /*   490 */  1116,   76,  413, 1113,  259,  159, 1000,  957,   93,  900,
 /*   500 */   904,   88,  875,  192, 1023,  899,  196, 1018,  884,  241,
 /*   510 */   933,  127, 1112,  972,  265,  272,   24, 1067,  980,  365,
 /*   520 */   985,  984, 1027,  223, 1075, 1076,  365,  895,  896,  249,
 /*   530 */   883,  798,  799, 1028,  401,  882,  143,  384,  815,  816,
 /*   540 */   252,  100,  251,  380,  970,   76,  973,  826,  382,  969,
 /*   550 */   968, 1087,  350,  947, 1088, 1084,  406,   52, 1048,  322,
 /*   560 */   906,  102,   10,  400,   89,  102,   91,  404,  404, 1010,
 /*   570 */    75,   91,  376,  894, 1009,  375,  974,  378,   11,   81,
 /*   580 */   235,  391,  210,  212,  394,  932,  404,   48,   56,  961,
 /*   590 */    63,   91,   94, 1127,   96,  912,   32,  402,   33, 1012,
 /*   600 */   155,  244, 1114,  148,  261,   45,   14,   47,  274,  150,
 /*   610 */   293,  327,  295, 1039,  307,  309,  311,   60, 1104,  335,
 /*   620 */   808,  187,  189,  992,  411,  842,  841,  840,  839,  838,
 /*   630 */   414,  218,   18,  922,  220,  248,   54,  242,  122,  123,
 /*   640 */    31,  951,  130,  138,   83, 1059, 1132,  129, 1131,   84,
 /*   650 */    85, 1130, 1128, 1060, 1057,  257,   34,   87,  139,  916,
 /*   660 */   140,   44,  269,  221,  921,  144,  897,  279,  273,  145,
 /*   670 */   146,  983,  987,  280,  281,  284,  926,  923,  876,  314,
 /*   680 */   291,   59,  294,  225,  299,  891,    9,  893,  161,   36,
 /*   690 */   366,  871,  169,  321,  903,  911,  910,  325,  173,   37,
 /*   700 */   172,  176, 1034,  909,  175,   38,  811,  806,  809, 1031,
 /*   710 */   333,  343,  415,  181,  417,  801, 1030,  813,  111,  368,
 /*   720 */  1026,  800,  348,  357,  919,  190,   82, 1024,  203,  351,
 /*   730 */   917,  890,   16, 1015,  399,  885,  398,  236,  861,   17,
 /*   740 */    20,  374,  211,  200,  361,  990,  991,   74,  214,  362,
 /*   750 */   918,  359,  116,  206,  119,  408,  812,  419,  117,  797,
 /*   760 */   208,  217,  941,  125,  243,  940,  939,  219,  247,  126,
 /*   770 */    69, 1125, 1122,    4,  250,  943, 1117,  135, 1126,  102,
 /*   780 */  1070, 1069,   46,  934, 1068,   95,  264,  141,  948, 1046,
 /*   790 */    71,  979,   97,  270,  977,  149,   99,   98,  953,  276,
 /*   800 */   204,  151,  954,  955,  153,  278,  152,   35,  285,  289,
 /*   810 */  1035,  286,  880,   72,  156,   55,  978,  157,  296,  101,
 /*   820 */   971,  158,  300,  301,   23,  313,  106,  966, 1445,  959,
 /*   830 */   965,  160,  163,   49,  162,   57,  964,  103,  873,   26,
 /*   840 */   228,  963,  104,   14,  164,   61,  105,  962, 1032,  108,
 /*   850 */    43,  179,  180,  867,  819,  336,  818,  184,  870,  817,
 /*   860 */   185,  344,   27,  112,   28, 1025, 1412,   15,  191,  349,
 /*   870 */    73,  193,  354,   29,  332,  352,  353,  920,  198,   30,
 /*   880 */   996,  194, 1001,  358,  360,  356,  199,   65,   40,  367,
 /*   890 */    39,  824,  202,  113,  823,  853,  373,  822,  377,  828,
 /*   900 */   379,  827,  858,  385,  386,  387,  237,  383,  381, 1014,
 /*   910 */   846,  845,  802,  213,  844,  389,  820,  843,   68,  215,
 /*   920 */   814,  120,   19,  392,  416,  390,  395,  393,  837,  804,
 /*   930 */   418,   90,  855,  836,  851,  850,   79,  397, 1133, 1135,
 /*   940 */   420, 1135, 1135, 1135,  936, 1135,  914, 1135, 1135, 1135,
 /*   950 */  1135, 1135, 1135, 1135, 1135, 1135, 1135, 1135, 1135, 1135,
 /*   960 */  1135, 1135, 1135, 1135, 1135, 1135, 1135, 1135, 1135,  405,
};
static const MQLYYCODETYPE mqlyy_lookahead[] = {
 /*     0 */   132,  133,  134,  135,  136,  137,  138,  139,  140,  141,
 /*    10 */   142,  143,  144,  145,  146,  147,  148,  149,  150,  151,
 /*    20 */   152,  153,  154,  155,  156,  157,  158,  159,  160,  161,
 /*    30 */   162,  163,  164,  165,  166,  167,  168,  169,  170,  171,
 /*    40 */   172,  173,  174,  175,  176,  177,  178,  179,  180,  181,
 /*    50 */     2,  195,   19,  191,    6,    7,  196,  197,  198,  199,
 /*    60 */    12,   13,  278,  279,  280,  281,  282,   54,  284,  285,
 /*    70 */    22,  207,   24,   25,  212,   57,   58,   59,   30,  215,
 /*    80 */   245,  246,  220,    4,   71,  301,  302,  303,  304,  305,
 /*    90 */    17,   58,   59,  278,  230,  280,  281,  282,  199,  284,
 /*   100 */   285,   28,  234,   30,    4,   57,    4,    5,   60,   61,
 /*   110 */    62,  276,  277,   34,  210,   67,  301,  302,  303,  304,
 /*   120 */   305,    4,  280,  281,  282,  190,  284,  285,   49,   55,
 /*   130 */   195,   52,  228,  229,  278,   18,  280,  281,  282,    4,
 /*   140 */   284,  285,   69,  301,  302,  303,  304,  305,   75,   76,
 /*   150 */    27,  103,  253,   18,   81,   53,   83,  301,  302,  303,
 /*   160 */   304,  305,    4,   90,    4,   92,    4,   69,   72,   69,
 /*   170 */    97,   73,   99,  280,  281,  282,   80,  284,  285,   77,
 /*   180 */    78,   72,   18,  248,   64,   65,   66,    4,   26,   80,
 /*   190 */    70,   29,   34,  199,  301,  302,  303,  304,  305,  208,
 /*   200 */   209,  210,   40,  199,   42,   43,   44,   45,   46,    4,
 /*   210 */     5,    4,  230,  117,  118,  119,  120,  121,  122,  123,
 /*   220 */   124,  199,    3,  106,  107,  199,  117,  118,  119,  120,
 /*   230 */   121,  122,  123,  124,   15,  199,   17,  249,  256,  257,
 /*   240 */   252,  106,  107,  126,  127,  128,  129,   28,   28,   30,
 /*   250 */    30,  207,   69,    3,   34,   18,   73,   52,   53,  215,
 /*   260 */    53,   54,  127,  128,  129,   15,   46,   17,  104,   18,
 /*   270 */   106,  107,  230,  251,  230,  115,  116,  251,   73,  191,
 /*   280 */    30,  230,   77,   78,   77,   78,  199,  251,  294,  295,
 /*   290 */   296,  297,   34,  129,   75,   76,   17,  255,   79,  295,
 /*   300 */   296,  297,  199,   45,   84,  199,  255,   28,  220,   30,
 /*   310 */   199,   91,   92,    4,   58,   59,  294,  295,  296,  297,
 /*   320 */   294,  295,  296,  297,   82,   75,   76,  239,  240,  287,
 /*   330 */   294,  295,  296,  297,  230,   93,   94,   95,  287,  245,
 /*   340 */   298,  299,  245,  106,  107,  211,  230,  213,    4,  298,
 /*   350 */   299,  264,  265,  266,   75,   76,  199,  106,  107,  101,
 /*   360 */   256,  257,   53,  211,   18,  213,  129,  264,  265,  266,
 /*   370 */   190,  277,  256,  257,  277,  195,    4,  211,  263,  213,
 /*   380 */   129,  245,  245,  268,  269,    4,   77,   78,   81,  222,
 /*   390 */   223,  224,  219,   17,  300,   81,    4,  300,  112,  113,
 /*   400 */   114,  267,  296,  297,   28,  191,   34,  296,  297,    4,
 /*   410 */    34,   19,  215,  277,  277,   34,  199,  110,  111,  267,
 /*   420 */   191,   53,  265,  266,  110,  111,   34,  117,   53,  199,
 /*   430 */   120,   19,   53,  267,  220,  199,  300,  300,  241,   27,
 /*   440 */   130,   69,   64,   65,   66,   77,   78,   42,  210,  220,
 /*   450 */    45,   45,  291,  292,  274,  211,  212,   78,   83,   84,
 /*   460 */   291,  292,  209,  210,  199,  213,  293,  229,    4,  215,
 /*   470 */     4,  287,  213,  218,  219,  258,    4,   96,  263,  215,
 /*   480 */   195,  215,  298,  219,  269,  219,  198,  199,  198,  199,
 /*   490 */   215,   73,  190,  301,  258,  241,  266,  195,  306,  226,
 /*   500 */   227,  214,  215,  270,  271,  223,  224,  101,   46,  242,
 /*   510 */   243,   18,   19,   19,   33,   17,   52,   36,   20,  267,
 /*   520 */    75,   76,   17,  258,  115,  116,  267,   75,   76,  244,
 /*   530 */    68,    4,    5,   28,    8,   69,   52,   31,   20,   21,
 /*   540 */    86,   51,   88,   37,   19,   73,    4,   34,   35,   54,
 /*   550 */    54,   54,    2,   69,   54,   54,   53,   71,  283,   51,
 /*   560 */    74,   71,  254,   37,   72,   71,   71,   71,   71,   54,
 /*   570 */    71,   71,   27,   74,   54,   30,   34,   26,  254,   71,
 /*   580 */    29,   27,   71,  200,   30,   74,   71,   62,  275,  252,
 /*   590 */   216,   71,  289,  309,  289,  230,  307,  203,  307,  230,
 /*   600 */   216,  199,  301,  260,  293,  290,  105,  290,  259,  220,
 /*   610 */   212,  212,  199,  199,  199,  199,  199,  217,  199,  199,
 /*   620 */   182,  182,  182,  263,  194,  204,  204,  204,  204,  204,
 /*   630 */   189,  182,   89,  238,   85,    8,   62,  195,  237,  236,
 /*   640 */   308,  195,  195,  108,  286,  109,   53,  288,  310,  288,
 /*   650 */   288,  310,  310,    4,  287,  310,  308,  286,  288,   53,
 /*   660 */   195,   80,  231,  231,  238,  237,    4,   30,  195,  236,
 /*   670 */   235,  221,  221,  195,  247,  215,   28,  240,  215,   10,
 /*   680 */   195,   80,  195,  195,  250,    4,  195,  219,  235,   18,
 /*   690 */    34,  211,   18,  250,    4,  225,  227,  195,  224,  195,
 /*   700 */   225,  224,  273,  227,  225,  195,   14,    3,  187,  273,
 /*   710 */   221,  195,   16,  185,    8,  183,  272,  188,  185,    8,
 /*   720 */   272,  183,  261,   53,    4,  211,  219,  271,   18,  195,
 /*   730 */   221,  219,  195,  262,   51,  211,   52,    8,   56,  195,
 /*   740 */    55,   48,  201,  261,  221,  262,  215,  225,  192,  219,
 /*   750 */   233,  232,  206,  205,  193,  202,  188,   10,  206,  184,
 /*   760 */   205,  183,   19,   18,   51,   34,   34,   27,   82,   71,
 /*   770 */    16,   19,   19,  105,   87,   87,   19,   35,   19,   71,
 /*   780 */    54,   54,   52,  243,   36,   71,   35,   18,   54,   19,
 /*   790 */    96,   20,   51,   98,   19,   18,   10,   51,   19,   21,
 /*   800 */   261,   18,   53,   19,   80,   72,   90,   18,   34,   46,
 /*   810 */    19,   31,   68,   63,   18,    8,   19,   18,   51,   84,
 /*   820 */    19,   18,   34,   31,   52,   30,  125,   54,  125,   19,
 /*   830 */    54,   18,   71,   52,   28,   80,   54,   52,   19,   35,
 /*   840 */    84,   54,   52,  105,   18,    8,   52,   54,   19,   51,
 /*   850 */    92,   46,   18,   39,   23,   20,   23,   46,   19,   23,
 /*   860 */    18,   20,  102,    3,  102,   19,  102,   18,   51,   17,
 /*   870 */    19,   18,    8,   73,   72,   20,   17,   53,    8,   51,
 /*   880 */    19,   72,   39,   27,   72,   77,   46,   71,   73,   45,
 /*   890 */    72,   28,   72,   72,   28,   50,   49,   28,   27,   34,
 /*   900 */    32,   34,   54,   41,   46,   27,   33,   32,   36,    4,
 /*   910 */    34,   34,    5,   18,   34,   41,   19,   34,   18,   20,
 /*   920 */    19,   17,  100,   41,    9,   46,   41,   46,   39,    5,
 /*   930 */    11,   41,   46,   39,   47,   47,   38,   53,    0,  311,
 /*   940 */     1,  311,  311,  311,   53,  311,   53,  311,  311,  311,
 /*   950 */   311,  311,  311,  311,  311,  311,  311,  311,  311,  311,
 /*   960 */   311,  311,  311,  311,  311,  311,  311,  311,  311,   77,
};
#define MQLYY_SHIFT_USE_DFLT (970)
#define MQLYY_SHIFT_COUNT    (421)
#define MQLYY_SHIFT_MIN      (0)
#define MQLYY_SHIFT_MAX      (939)
static const unsigned short int mqlyy_shift_ofst[] = {
 /*     0 */    48,  117,  135,  237,  251,  164,  251,   79,   79,   79,
 /*    10 */   102,  102,   79,   73,   79,  158,  158,  158,  123,  205,
 /*    20 */   205,  279,  207,  309,  309,   79,   79,  258,  258,   18,
 /*    30 */   258,  412,  412,  412,  412,  100,  100,  158,  256,  379,
 /*    40 */    74,  219,  250,  242,  183,  286,  160,  286,  372,  381,
 /*    50 */   376,  392,   18,   33,  158,  406,  158,   98,  464,   98,
 /*    60 */   466,  406,  472,  472,  158,  379,  158,  158,  100,  344,
 /*    70 */   346,  418,  418,  550,  492,   74,  503,   96,  109,  162,
 /*    80 */   220,  242,  120,  307,  314,  314,  310,  307,  378,  368,
 /*    90 */   405,  368,  158,  493,  481,  409,  481,  445,  498,  452,
 /*   100 */   505,  158,  158,  158,  158,  158,  158,  462,  505,  158,
 /*   110 */   527,  527,  527,  379,  526,  526,  526,  526,  526,  506,
 /*   120 */   518,  527,  543,  549,  627,  344,  344,  344,  574,  535,
 /*   130 */   536,  536,  536,  593,  593,  593,  593,  574,  649,  535,
 /*   140 */   536,  344,  606,  606,  543,  549,  627,  581,  662,  344,
 /*   150 */   662,  344,  637,  418,  648,  418,  344,  344,  344,  669,
 /*   160 */   344,  669,  601,  681,  344,  671,  671,  656,  674,  344,
 /*   170 */   674,  690,  492,  690,   74,  492,  690,   74,  662,  681,
 /*   180 */   344,  692,  704,  662,  681,  344,  696,  706,  704,  706,
 /*   190 */   711,  656,  550,  344,  670,  720,  690,  662,  656,  681,
 /*   200 */   710,  711,  418,  344,  710,  711,  683,  684,  683,  684,
 /*   210 */   503,  682,  685,  344,  729,  693,  696,  747,  706,  375,
 /*   220 */   454,   13,  484,  490,  494,  525,  495,  496,  542,  497,
 /*   230 */   500,  501,  508,  486,  499,  545,  551,  513,  554,  515,
 /*   240 */   520,  511,  743,  745,  713,  731,  732,  740,  686,  698,
 /*   250 */   754,  687,  688,  752,  753,  757,  668,  742,  759,  708,
 /*   260 */   726,  727,  714,  730,  748,  751,  769,  694,  770,  734,
 /*   270 */   741,  695,  771,  775,  777,  746,  786,  778,  749,  733,
 /*   280 */   779,  783,  716,  784,  789,  724,  774,  780,  744,  750,
 /*   290 */   763,  791,  796,  807,  797,  799,  735,  767,  801,  803,
 /*   300 */   755,  788,  792,  772,  773,  776,  781,  782,  785,  787,
 /*   310 */   790,  793,  794,  756,  795,  701,  703,  804,  804,  738,
 /*   320 */   810,  813,  806,  758,  761,  819,  826,  837,  798,  829,
 /*   330 */   760,  762,  800,  802,  805,  814,  834,  835,  764,  831,
 /*   340 */   833,  836,  811,  839,  842,  841,  860,  846,  849,  817,
 /*   350 */   852,  851,  853,  855,  859,  864,  824,  808,  809,  856,
 /*   360 */   815,  812,  870,  840,  816,  818,  820,  821,  844,  843,
 /*   370 */   822,  861,  828,  845,  847,  863,  866,  869,  871,  865,
 /*   380 */   868,  867,  872,  873,  875,  876,  862,  858,  878,  877,
 /*   390 */   874,  879,  880,  882,  881,  883,  885,  848,  884,  886,
 /*   400 */   887,  888,  889,  890,  905,  891,  892,  893,  894,  898,
 /*   410 */   897,  895,  899,  901,  900,  904,  907,  915,  924,  919,
 /*   420 */   938,  939,
};
#define MQLYY_REDUCE_USE_DFLT (-217)
#define MQLYY_REDUCE_COUNT (218)
#define MQLYY_REDUCE_MIN   (-216)
#define MQLYY_REDUCE_MAX   (578)
static const short mqlyy_reduce_ofst[] = {
 /*     0 */  -132, -144, -144, -216, -185, -158, -107,   22,   26,   36,
 /*    10 */    42,   51,   -6,   88,    4,   87,  103, -140, -165, -136,
 /*    20 */    44, -138,  -18,  104,  116,  106,  111,  152,  166,  -96,
 /*    30 */   134,   94,   97,  136,  137,  -65,  180,  157,   -9,  115,
 /*    40 */   167,  214,  229,  -12,  197,  161,  173,  169,  217, -101,
 /*    50 */   244,  230,  238,  253,  236,  252,  265,  254,  184,  197,
 /*    60 */   255,  259,  264,  266,  288,  215,  230,  290,  302,  285,
 /*    70 */   192,  275,  287,  233,  273,  282,  267,  308,  324,  383,
 /*    80 */   313,  337,  374,  303,  289,  291,  284,  305,  384,  365,
 /*    90 */   394,  369,  402,  301,  315,  311,  317,  343,  349,  389,
 /*   100 */   398,  413,  414,  415,  416,  417,  419,  400,  399,  420,
 /*   110 */   438,  439,  440,  360,  421,  422,  423,  424,  425,  430,
 /*   120 */   441,  449,  395,  401,  403,  442,  446,  447,  332,  358,
 /*   130 */   359,  361,  362,  338,  341,  342,  345,  348,  367,  371,
 /*   140 */   370,  465,  431,  432,  426,  428,  433,  435,  450,  473,
 /*   150 */   451,  478,  427,  460,  437,  463,  485,  487,  488,  434,
 /*   160 */   491,  443,  453,  468,  502,  429,  436,  480,  444,  504,
 /*   170 */   448,  470,  469,  475,  474,  476,  479,  477,  489,  507,
 /*   180 */   510,  521,  528,  509,  512,  516,  529,  532,  533,  538,
 /*   190 */   461,  514,  456,  534,  517,  519,  522,  523,  524,  530,
 /*   200 */   471,  482,  531,  537,  483,  539,  546,  548,  552,  555,
 /*   210 */   540,  553,  541,  544,  561,  556,  568,  575,  578,
};
static const MQLYYACTIONTYPE mqlyy_default[] = {
 /*     0 */  1518, 1518, 1518, 1518, 1518, 1434, 1518, 1462, 1462, 1462,
 /*    10 */  1518, 1518, 1518, 1215, 1518, 1383, 1383, 1218, 1331, 1518,
 /*    20 */  1518, 1215, 1518, 1518, 1518, 1518, 1518, 1518, 1518, 1254,
 /*    30 */  1518, 1331, 1331, 1331, 1331, 1518, 1518, 1518, 1254, 1518,
 /*    40 */  1287, 1215, 1215, 1518, 1518, 1456, 1518, 1456, 1518, 1518,
 /*    50 */  1518, 1518, 1254, 1254, 1518, 1518, 1518, 1518, 1518, 1518,
 /*    60 */  1518, 1518, 1518, 1518, 1518, 1518, 1382, 1217, 1518, 1518,
 /*    70 */  1518, 1500, 1518, 1518, 1290, 1287, 1518, 1518, 1518, 1518,
 /*    80 */  1518, 1518, 1518, 1446, 1503, 1503, 1514, 1446, 1259, 1518,
 /*    90 */  1518, 1518, 1518, 1518, 1450, 1518, 1450, 1371, 1366, 1518,
 /*   100 */  1518, 1518, 1518, 1518, 1518, 1518, 1518, 1518, 1518, 1518,
 /*   110 */  1518, 1518, 1518, 1518, 1237, 1237, 1237, 1237, 1237, 1210,
 /*   120 */  1518, 1518, 1518, 1327, 1323, 1518, 1518, 1518, 1509, 1441,
 /*   130 */  1443, 1443, 1443, 1518, 1518, 1518, 1518, 1509, 1518, 1441,
 /*   140 */  1443, 1518, 1518, 1518, 1518, 1327, 1323, 1314, 1518, 1518,
 /*   150 */  1518, 1518, 1518, 1518, 1312, 1518, 1518, 1518, 1518, 1360,
 /*   160 */  1518, 1360, 1314, 1518, 1518, 1518, 1518, 1518, 1518, 1518,
 /*   170 */  1518, 1518, 1518, 1518, 1287, 1518, 1518, 1287, 1518, 1518,
 /*   180 */  1518, 1195, 1192, 1518, 1518, 1518, 1518, 1188, 1192, 1188,
 /*   190 */  1378, 1518, 1407, 1518, 1518, 1518, 1518, 1518, 1518, 1518,
 /*   200 */  1518, 1378, 1518, 1518, 1518, 1378, 1241, 1242, 1241, 1242,
 /*   210 */  1518, 1247, 1245, 1518, 1206, 1239, 1518, 1190, 1188, 1518,
 /*   220 */  1518, 1518, 1518, 1518, 1518, 1518, 1518, 1518, 1518, 1518,
 /*   230 */  1518, 1518, 1518, 1518, 1518, 1518, 1518, 1518, 1518, 1518,
 /*   240 */  1518, 1518, 1518, 1518, 1518, 1518, 1518, 1518, 1518, 1330,
 /*   250 */  1329, 1518, 1518, 1518, 1518, 1518, 1437, 1518, 1518, 1508,
 /*   260 */  1518, 1518, 1518, 1518, 1518, 1451, 1518, 1432, 1518, 1518,
 /*   270 */  1518, 1518, 1518, 1518, 1518, 1518, 1361, 1518, 1518, 1518,
 /*   280 */  1518, 1518, 1518, 1518, 1518, 1518, 1518, 1311, 1518, 1518,
 /*   290 */  1518, 1518, 1518, 1518, 1518, 1518, 1518, 1518, 1518, 1518,
 /*   300 */  1518, 1518, 1518, 1518, 1518, 1518, 1518, 1518, 1518, 1518,
 /*   310 */  1518, 1518, 1518, 1518, 1518, 1518, 1488, 1464, 1465, 1463,
 /*   320 */  1518, 1518, 1518, 1518, 1266, 1518, 1518, 1518, 1518, 1518,
 /*   330 */  1518, 1518, 1518, 1518, 1518, 1518, 1518, 1518, 1214, 1518,
 /*   340 */  1518, 1518, 1518, 1518, 1518, 1518, 1518, 1518, 1518, 1518,
 /*   350 */  1518, 1518, 1518, 1518, 1518, 1518, 1518, 1518, 1518, 1518,
 /*   360 */  1518, 1518, 1518, 1518, 1401, 1518, 1518, 1518, 1518, 1518,
 /*   370 */  1518, 1518, 1214, 1518, 1518, 1518, 1518, 1518, 1518, 1518,
 /*   380 */  1518, 1518, 1518, 1518, 1518, 1518, 1518, 1518, 1518, 1518,
 /*   390 */  1518, 1518, 1518, 1518, 1518, 1518, 1518, 1518, 1518, 1518,
 /*   400 */  1518, 1518, 1518, 1518, 1518, 1322, 1320, 1518, 1518, 1518,
 /*   410 */  1518, 1518, 1518, 1518, 1518, 1518, 1518, 1518, 1518, 1518,
 /*   420 */  1518, 1518,
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
#ifdef MQLYYFALLBACK
static const MQLYYCODETYPE mqlyyFallback[] = {
};
#endif /* MQLYYFALLBACK */

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
struct mqlyyStackEntry {
  MQLYYACTIONTYPE stateno;  /* The state-number, or reduce action in SHIFTREDUCE */
  MQLYYCODETYPE major;      /* The major token value.  This is the code
                         ** number for the token at this stack level */
  MQLYYMINORTYPE minor;     /* The user-supplied minor token value.  This
                         ** is the value of the token  */
};
typedef struct mqlyyStackEntry mqlyyStackEntry;

/* The state of the parser is completely contained in an instance of
** the following structure */
struct mqlyyParser {
  mqlyyStackEntry *mqlyytos;          /* Pointer to top element of the stack */
#ifdef MQLYYTRACKMAXSTACKDEPTH
  int mqlyyhwm;                    /* High-water mark of the stack */
#endif
#ifndef MQLYYNOERRORRECOVERY
  int mqlyyerrcnt;                 /* Shifts left before out of the error */
#endif
  MQLParserARG_SDECL                /* A place to hold %extra_argument */
#if MQLYYSTACKDEPTH<=0
  int mqlyystksz;                  /* Current side of the stack */
  mqlyyStackEntry *mqlyystack;        /* The parser's stack */
  mqlyyStackEntry mqlyystk0;          /* First stack entry */
#else
  mqlyyStackEntry mqlyystack[MQLYYSTACKDEPTH];  /* The parser's stack */
#endif
};
typedef struct mqlyyParser mqlyyParser;

#ifndef NDEBUG
#include <stdio.h>
static FILE *mqlyyTraceFILE = 0;
static char *mqlyyTracePrompt = 0;
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
void MQLParserTrace(FILE *TraceFILE, char *zTracePrompt){
  mqlyyTraceFILE = TraceFILE;
  mqlyyTracePrompt = zTracePrompt;
  if( mqlyyTraceFILE==0 ) mqlyyTracePrompt = 0;
  else if( mqlyyTracePrompt==0 ) mqlyyTraceFILE = 0;
}
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing shifts, the names of all terminals and nonterminals
** are required.  The following table supplies these names */
static const char *const mqlyyTokenName[] = { 
  "$",             "KEY_GO",        "KEY_CREATE",    "KEY_DATABASE",
  "IDENTIFIER",    "STRING",        "KEY_INITIALIZE",  "KEY_USE",     
  "KEY_WITH",      "KEY_KEY",       "KEY_USING",     "KEY_ENCODING",
  "KEY_DROP",      "KEY_VACUUM",    "KEY_ANALYZE",   "KEY_INDEXES", 
  "KEY_ON",        "KEY_OBJECT",    "KEY_OPEN_SQUARE_BRACKET",  "KEY_CLOSE_SQUARE_BRACKET",
  "KEY_TYPE",      "KEY_TYPES",     "KEY_BEGIN",     "KEY_TRANSACTION",
  "KEY_COMMIT",    "KEY_ABORT",     "KEY_MULTIPLE",  "KEY_RANGE",   
  "KEY_OBJECTS",   "KEY_SINGLE",    "KEY_MONAD",     "KEY_HAVING",  
  "KEY_UNIQUE",    "KEY_FIRST",     "KEY_MONADS",    "KEY_AND",     
  "KEY_LAST",      "KEY_WITHOUT",   "KEY_COLON",     "KEY_SEMICOLON",
  "KEY_LIST",      "KEY_OF",        "KEY_INTEGER",   "KEY_STRING",  
  "KEY_ASCII",     "KEY_ID_D",      "KEY_SET",       "KEY_INDEX",   
  "KEY_IF",        "KEY_NOT",       "KEY_EXISTS",    "KEY_FROM",    
  "KEY_OPEN_BRACKET",  "INTEGER",       "KEY_CLOSE_BRACKET",  "KEY_DEFAULT", 
  "KEY_COMPUTED",  "KEY_UPDATE",    "KEY_REMOVE",    "KEY_ADD",     
  "KEY_INSERT",    "KEY_DELETE",    "KEY_GET",       "KEY_CALCULATION",
  "KEY_UNION",     "KEY_DIFFERENCE",  "KEY_INTERSECT",  "KEY_SELECT",  
  "KEY_SETS",      "KEY_ALL",       "KEY_REPLACE",   "KEY_COMMA",   
  "KEY_EQUALS",    "KEY_OPEN_BRACE",  "KEY_CLOSE_BRACE",  "KEY_ENUMERATION",
  "KEY_ENUM",      "KEY_DASH",      "KEY_NIL",       "KEY_SEGMENT", 
  "KEY_IN",        "KEY_FOCUS",     "KEY_MAX",       "KEY_MAX_M",   
  "KEY_FEATURE",   "KEY_RETURNING",  "KEY_FULL",      "KEY_SHEAF",   
  "KEY_FLAT",      "KEY_WHERE",     "KEY_AT",        "KEY_AGGREGATE",
  "KEY_FEATURES",  "KEY_MIN",       "KEY_SUM",       "KEY_COUNT",   
  "KEY_STAR",      "KEY_ENUMERATIONS",  "KEY_CONSTANTS",  "KEY_MIN_M",   
  "KEY_ASSIGN",    "KEY_ID_DS",     "KEY_BY",        "KEY_QUIT",    
  "KEY_EXCLAMATION",  "KEY_OR",        "KEY_NOTEXIST",  "KEY_NOTEXISTS",
  "KEY_AS",        "MARK",          "KEY_NORETRIEVE",  "KEY_RETRIEVE",
  "KEY_PART_OF",   "KEY_STARTS_IN",  "KEY_OVERLAP",   "KEY_UNIVERSE",
  "KEY_SUBSTRATE",  "KEY_LESS_THAN",  "KEY_GREATER_THAN",  "KEY_NOT_EQUAL",
  "KEY_LESS_THAN_OR_EQUAL",  "KEY_GREATER_THAN_OR_EQUAL",  "KEY_TILDE",     "KEY_NOT_TILDE",
  "KEY_HAS",       "KEY_DOT",       "KEY_UNORDERED_GROUP",  "KEY_OPT_GAP", 
  "KEY_GAP",       "KEY_POWER",     "KEY_BETWEEN",   "error",       
  "statement",     "statement_by_itself",  "create_database_statement",  "initialize_database_statement",
  "use_statement",  "drop_database_statement",  "vacuum_database_statement",  "create_object_type_statement",
  "update_object_type_statement",  "drop_object_type_statement",  "insert_monads_statement",  "delete_monads_statement",
  "get_monads_statement",  "monad_set_calculation_statement",  "create_enumeration_statement",  "update_enumeration_statement",
  "drop_enumeration_statement",  "create_segment_statement",  "select_statement",  "select_objects_at_statement",
  "select_objects_having_monads_in_statement",  "get_aggregate_features_statement",  "get_objects_having_monads_in_statement",  "get_set_from_feature_statement",
  "select_object_types_statement",  "select_features_statement",  "select_enumerations_statement",  "select_enumeration_constants_statement",
  "select_object_types_which_use_enum_statement",  "select_min_m_statement",  "select_max_m_statement",  "create_object_from_monads_statement",
  "create_object_from_id_ds_statement",  "update_objects_by_monads_statement",  "update_objects_by_id_ds_statement",  "delete_objects_by_monads_statement",
  "delete_objects_by_id_ds_statement",  "get_features_statement",  "quit_statement",  "create_indexes_statement",
  "drop_indexes_statement",  "begin_transaction_statement",  "commit_transaction_statement",  "abort_transaction_statement",
  "select_monad_sets_statement",  "get_monad_sets_statement",  "create_monad_set_statement",  "update_monad_set_statement",
  "drop_monad_set_statement",  "create_objects_statement",  "database_name",  "opt_WITH_KEY",
  "opt_USING_ENCODING",  "opt_DATABASE",  "opt_WITH_ENCODING",  "opt_ANALYZE", 
  "on_object_type",  "choice_type_types",  "choice_object_type_or_all",  "opt_OBJECT",  
  "opt_if_not_exists",  "opt_range_type",  "opt_monad_uniqueness_type",  "object_type_name",
  "opt_feature_declaration_list",  "feature_declaration_list",  "feature_declaration",  "feature_name",
  "feature_type",  "default_specification",  "opt_computed",  "list_feature_type",
  "opt_with_index",  "opt_string_length",  "opt_from_set",  "expression",  
  "feature_update_list",  "feature_update",  "opt_ADD",       "monad_specification",
  "choice_number_OBJECTS",  "id_ds_specification",  "monad_set_chain",  "monad_set",   
  "monad_set_operator",  "choice_number_SET",  "monad_set_name_list",  "monad_set_name",
  "choice_ENUM_ERATION",  "enumeration_name",  "ec_declaration_list",  "ec_declaration",
  "opt_DEFAULT",   "ec_name",       "opt_ec_initialization",  "ec_initialization",
  "ec_update_list",  "ec_update",     "signed_integer",  "unsigned_integer",
  "segment_name",  "segment_range",  "select_clause",  "in_clause",   
  "with_max_range_clause",  "returning_clause",  "where_clause",  "focus_specification",
  "opt_OBJECTS",   "in_specification",  "monad_set_element_list",  "monad_set_element",
  "object_type_name_list",  "using_range_clause",  "mql_query",     "single_monad_specification",
  "object_type_to_find",  "aggregate_feature_list",  "using_monad_feature",  "feature_constraints",
  "aggregate_feature",  "aggregate_feature_comparison",  "comparison_operator",  "value",       
  "list_of_identifier",  "list_of_integer",  "feature_list",  "opt_OBJECTYPE",
  "opt_ENUM_ERATION",  "with_id_d_specification",  "object_creation_specification",  "id_d_const",  
  "opt_list_of_feature_assignments",  "list_of_feature_assignments",  "feature_assignment",  "choice_number_ID_DS",
  "id_d_list",     "id_d",          "object_creation_list",  "object_creation_no_object_type",
  "object_update_specification",  "object_deletion_specification",  "object_type_name_to_delete",  "choice_number_FEATURES",
  "topograph",     "blocks",        "block_string",  "unordered_group",
  "block_string0",  "block",         "block_string1",  "star_monad_set",
  "block_string2",  "notexist",      "object_reference_declaration",  "object_reference",
  "mark_declaration",  "retrieval",     "firstlast",     "monad_set_relation_clause",
  "monad_set_relation_operation",  "universe_or_substrate",  "ffeatures",     "fterm",       
  "ffactor",       "feature_comparison",  "object_reference_usage",  "enum_const",  
  "opt_blocks",    "object_block",  "power",         "opt_gap_block",
  "gap_block",     "notexist_object_block",  "object_block_string",  "gap_retrieval",
  "feature_retrieval",  "restrictor",    "limit",       
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *const mqlyyRuleName[] = {
 /*   0 */ "statement ::= statement_by_itself KEY_GO",
 /*   1 */ "statement_by_itself ::= create_database_statement",
 /*   2 */ "statement_by_itself ::= initialize_database_statement",
 /*   3 */ "statement_by_itself ::= use_statement",
 /*   4 */ "statement_by_itself ::= drop_database_statement",
 /*   5 */ "statement_by_itself ::= vacuum_database_statement",
 /*   6 */ "statement_by_itself ::= create_object_type_statement",
 /*   7 */ "statement_by_itself ::= update_object_type_statement",
 /*   8 */ "statement_by_itself ::= drop_object_type_statement",
 /*   9 */ "statement_by_itself ::= insert_monads_statement",
 /*  10 */ "statement_by_itself ::= delete_monads_statement",
 /*  11 */ "statement_by_itself ::= get_monads_statement",
 /*  12 */ "statement_by_itself ::= monad_set_calculation_statement",
 /*  13 */ "statement_by_itself ::= create_enumeration_statement",
 /*  14 */ "statement_by_itself ::= update_enumeration_statement",
 /*  15 */ "statement_by_itself ::= drop_enumeration_statement",
 /*  16 */ "statement_by_itself ::= create_segment_statement",
 /*  17 */ "statement_by_itself ::= select_statement",
 /*  18 */ "statement_by_itself ::= select_objects_at_statement",
 /*  19 */ "statement_by_itself ::= select_objects_having_monads_in_statement",
 /*  20 */ "statement_by_itself ::= get_aggregate_features_statement",
 /*  21 */ "statement_by_itself ::= get_objects_having_monads_in_statement",
 /*  22 */ "statement_by_itself ::= get_set_from_feature_statement",
 /*  23 */ "statement_by_itself ::= select_object_types_statement",
 /*  24 */ "statement_by_itself ::= select_features_statement",
 /*  25 */ "statement_by_itself ::= select_enumerations_statement",
 /*  26 */ "statement_by_itself ::= select_enumeration_constants_statement",
 /*  27 */ "statement_by_itself ::= select_object_types_which_use_enum_statement",
 /*  28 */ "statement_by_itself ::= select_min_m_statement",
 /*  29 */ "statement_by_itself ::= select_max_m_statement",
 /*  30 */ "statement_by_itself ::= create_object_from_monads_statement",
 /*  31 */ "statement_by_itself ::= create_object_from_id_ds_statement",
 /*  32 */ "statement_by_itself ::= update_objects_by_monads_statement",
 /*  33 */ "statement_by_itself ::= update_objects_by_id_ds_statement",
 /*  34 */ "statement_by_itself ::= delete_objects_by_monads_statement",
 /*  35 */ "statement_by_itself ::= delete_objects_by_id_ds_statement",
 /*  36 */ "statement_by_itself ::= get_features_statement",
 /*  37 */ "statement_by_itself ::= quit_statement",
 /*  38 */ "statement_by_itself ::= create_indexes_statement",
 /*  39 */ "statement_by_itself ::= drop_indexes_statement",
 /*  40 */ "statement_by_itself ::= begin_transaction_statement",
 /*  41 */ "statement_by_itself ::= commit_transaction_statement",
 /*  42 */ "statement_by_itself ::= abort_transaction_statement",
 /*  43 */ "statement_by_itself ::= select_monad_sets_statement",
 /*  44 */ "statement_by_itself ::= get_monad_sets_statement",
 /*  45 */ "statement_by_itself ::= create_monad_set_statement",
 /*  46 */ "statement_by_itself ::= update_monad_set_statement",
 /*  47 */ "statement_by_itself ::= drop_monad_set_statement",
 /*  48 */ "statement_by_itself ::= create_objects_statement",
 /*  49 */ "create_database_statement ::= KEY_CREATE KEY_DATABASE database_name opt_WITH_KEY opt_USING_ENCODING",
 /*  50 */ "database_name ::= IDENTIFIER",
 /*  51 */ "database_name ::= STRING",
 /*  52 */ "initialize_database_statement ::= KEY_INITIALIZE KEY_DATABASE database_name opt_WITH_KEY",
 /*  53 */ "use_statement ::= KEY_USE opt_DATABASE database_name opt_WITH_KEY",
 /*  54 */ "opt_WITH_KEY ::= KEY_WITH KEY_KEY STRING",
 /*  55 */ "opt_WITH_KEY ::=",
 /*  56 */ "opt_USING_ENCODING ::= KEY_USING KEY_ENCODING STRING",
 /*  57 */ "opt_USING_ENCODING ::=",
 /*  58 */ "opt_DATABASE ::= KEY_DATABASE",
 /*  59 */ "opt_DATABASE ::=",
 /*  60 */ "drop_database_statement ::= KEY_DROP KEY_DATABASE database_name",
 /*  61 */ "vacuum_database_statement ::= KEY_VACUUM opt_DATABASE opt_ANALYZE",
 /*  62 */ "opt_ANALYZE ::=",
 /*  63 */ "opt_ANALYZE ::= KEY_ANALYZE",
 /*  64 */ "create_indexes_statement ::= KEY_CREATE KEY_INDEXES on_object_type",
 /*  65 */ "drop_indexes_statement ::= KEY_DROP KEY_INDEXES on_object_type",
 /*  66 */ "on_object_type ::= KEY_ON KEY_OBJECT choice_type_types KEY_OPEN_SQUARE_BRACKET choice_object_type_or_all KEY_CLOSE_SQUARE_BRACKET",
 /*  67 */ "choice_type_types ::= KEY_TYPE",
 /*  68 */ "choice_type_types ::= KEY_TYPES",
 /*  69 */ "begin_transaction_statement ::= KEY_BEGIN KEY_TRANSACTION",
 /*  70 */ "commit_transaction_statement ::= KEY_COMMIT KEY_TRANSACTION",
 /*  71 */ "abort_transaction_statement ::= KEY_ABORT KEY_TRANSACTION",
 /*  72 */ "create_object_type_statement ::= KEY_CREATE opt_OBJECT KEY_TYPE opt_if_not_exists opt_range_type opt_monad_uniqueness_type KEY_OPEN_SQUARE_BRACKET object_type_name opt_feature_declaration_list KEY_CLOSE_SQUARE_BRACKET",
 /*  73 */ "opt_range_type ::=",
 /*  74 */ "opt_range_type ::= KEY_WITH KEY_MULTIPLE KEY_RANGE KEY_OBJECTS",
 /*  75 */ "opt_range_type ::= KEY_WITH KEY_SINGLE KEY_RANGE KEY_OBJECTS",
 /*  76 */ "opt_range_type ::= KEY_WITH KEY_SINGLE KEY_MONAD KEY_OBJECTS",
 /*  77 */ "opt_monad_uniqueness_type ::=",
 /*  78 */ "opt_monad_uniqueness_type ::= KEY_HAVING KEY_UNIQUE KEY_FIRST KEY_MONADS",
 /*  79 */ "opt_monad_uniqueness_type ::= KEY_HAVING KEY_UNIQUE KEY_FIRST KEY_AND KEY_LAST KEY_MONADS",
 /*  80 */ "opt_monad_uniqueness_type ::= KEY_WITHOUT KEY_UNIQUE KEY_MONADS",
 /*  81 */ "opt_OBJECT ::= KEY_OBJECT",
 /*  82 */ "opt_OBJECT ::=",
 /*  83 */ "object_type_name ::= IDENTIFIER",
 /*  84 */ "opt_feature_declaration_list ::= feature_declaration_list",
 /*  85 */ "opt_feature_declaration_list ::=",
 /*  86 */ "feature_declaration_list ::= feature_declaration",
 /*  87 */ "feature_declaration_list ::= feature_declaration_list feature_declaration",
 /*  88 */ "feature_declaration ::= feature_name KEY_COLON feature_type default_specification opt_computed KEY_SEMICOLON",
 /*  89 */ "feature_declaration ::= feature_name KEY_COLON KEY_LIST KEY_OF list_feature_type KEY_SEMICOLON",
 /*  90 */ "feature_type ::= KEY_INTEGER opt_with_index",
 /*  91 */ "feature_type ::= KEY_STRING opt_string_length opt_from_set opt_with_index",
 /*  92 */ "feature_type ::= KEY_ASCII opt_string_length opt_from_set opt_with_index",
 /*  93 */ "feature_type ::= KEY_ID_D opt_with_index",
 /*  94 */ "feature_type ::= IDENTIFIER opt_with_index",
 /*  95 */ "feature_type ::= KEY_SET KEY_OF KEY_MONADS",
 /*  96 */ "feature_type ::= KEY_SINGLE KEY_MONAD KEY_SET KEY_OF KEY_MONADS",
 /*  97 */ "feature_type ::= KEY_SINGLE KEY_RANGE KEY_SET KEY_OF KEY_MONADS",
 /*  98 */ "feature_type ::= KEY_MULTIPLE KEY_RANGE KEY_SET KEY_OF KEY_MONADS",
 /*  99 */ "list_feature_type ::= KEY_INTEGER",
 /* 100 */ "list_feature_type ::= KEY_ID_D",
 /* 101 */ "list_feature_type ::= IDENTIFIER",
 /* 102 */ "opt_with_index ::= KEY_WITH KEY_INDEX",
 /* 103 */ "opt_with_index ::= KEY_WITHOUT KEY_INDEX",
 /* 104 */ "opt_with_index ::=",
 /* 105 */ "opt_if_not_exists ::= KEY_IF KEY_NOT KEY_EXISTS",
 /* 106 */ "opt_if_not_exists ::=",
 /* 107 */ "opt_from_set ::= KEY_FROM KEY_SET",
 /* 108 */ "opt_from_set ::=",
 /* 109 */ "opt_string_length ::=",
 /* 110 */ "opt_string_length ::= KEY_OPEN_BRACKET INTEGER KEY_CLOSE_BRACKET",
 /* 111 */ "default_specification ::= KEY_DEFAULT expression",
 /* 112 */ "default_specification ::=",
 /* 113 */ "opt_computed ::= KEY_COMPUTED",
 /* 114 */ "opt_computed ::=",
 /* 115 */ "update_object_type_statement ::= KEY_UPDATE opt_OBJECT KEY_TYPE KEY_OPEN_SQUARE_BRACKET object_type_name feature_update_list KEY_CLOSE_SQUARE_BRACKET",
 /* 116 */ "feature_update_list ::= feature_update",
 /* 117 */ "feature_update_list ::= feature_update_list feature_update",
 /* 118 */ "feature_update ::= opt_ADD feature_declaration",
 /* 119 */ "feature_update ::= KEY_REMOVE feature_name KEY_SEMICOLON",
 /* 120 */ "opt_ADD ::= KEY_ADD",
 /* 121 */ "opt_ADD ::=",
 /* 122 */ "drop_object_type_statement ::= KEY_DROP opt_OBJECT KEY_TYPE KEY_OPEN_SQUARE_BRACKET object_type_name KEY_CLOSE_SQUARE_BRACKET",
 /* 123 */ "insert_monads_statement ::= KEY_INSERT monad_specification",
 /* 124 */ "delete_monads_statement ::= KEY_DELETE monad_specification",
 /* 125 */ "get_monads_statement ::= KEY_GET KEY_MONADS KEY_FROM choice_number_OBJECTS KEY_WITH id_ds_specification KEY_OPEN_SQUARE_BRACKET object_type_name KEY_CLOSE_SQUARE_BRACKET",
 /* 126 */ "monad_set_calculation_statement ::= KEY_MONAD KEY_SET KEY_CALCULATION monad_set_chain",
 /* 127 */ "monad_set_chain ::= monad_set",
 /* 128 */ "monad_set_chain ::= monad_set_chain monad_set_operator monad_set",
 /* 129 */ "monad_set_operator ::= KEY_UNION",
 /* 130 */ "monad_set_operator ::= KEY_DIFFERENCE",
 /* 131 */ "monad_set_operator ::= KEY_INTERSECT",
 /* 132 */ "select_monad_sets_statement ::= KEY_SELECT KEY_MONAD KEY_SETS",
 /* 133 */ "get_monad_sets_statement ::= KEY_GET KEY_MONAD choice_number_SET monad_set_name_list",
 /* 134 */ "get_monad_sets_statement ::= KEY_GET KEY_MONAD choice_number_SET KEY_ALL",
 /* 135 */ "choice_number_SET ::= KEY_SETS",
 /* 136 */ "choice_number_SET ::= KEY_SET",
 /* 137 */ "create_monad_set_statement ::= KEY_CREATE KEY_MONAD KEY_SET monad_set_name KEY_WITH monad_specification",
 /* 138 */ "update_monad_set_statement ::= KEY_UPDATE KEY_MONAD KEY_SET monad_set_name monad_set_operator monad_set_name",
 /* 139 */ "update_monad_set_statement ::= KEY_UPDATE KEY_MONAD KEY_SET monad_set_name KEY_REPLACE monad_set_name",
 /* 140 */ "update_monad_set_statement ::= KEY_UPDATE KEY_MONAD KEY_SET monad_set_name monad_set_operator monad_set",
 /* 141 */ "update_monad_set_statement ::= KEY_UPDATE KEY_MONAD KEY_SET monad_set_name KEY_REPLACE monad_set",
 /* 142 */ "drop_monad_set_statement ::= KEY_DROP KEY_MONAD KEY_SET monad_set_name",
 /* 143 */ "monad_set_name ::= IDENTIFIER",
 /* 144 */ "monad_set_name_list ::= monad_set_name",
 /* 145 */ "monad_set_name_list ::= monad_set_name_list KEY_COMMA monad_set_name",
 /* 146 */ "create_enumeration_statement ::= KEY_CREATE choice_ENUM_ERATION enumeration_name KEY_EQUALS KEY_OPEN_BRACE ec_declaration_list KEY_CLOSE_BRACE",
 /* 147 */ "choice_ENUM_ERATION ::= KEY_ENUMERATION",
 /* 148 */ "choice_ENUM_ERATION ::= KEY_ENUM",
 /* 149 */ "enumeration_name ::= IDENTIFIER",
 /* 150 */ "ec_declaration_list ::= ec_declaration",
 /* 151 */ "ec_declaration_list ::= ec_declaration_list KEY_COMMA ec_declaration",
 /* 152 */ "ec_declaration ::= opt_DEFAULT ec_name opt_ec_initialization",
 /* 153 */ "opt_DEFAULT ::= KEY_DEFAULT",
 /* 154 */ "opt_DEFAULT ::=",
 /* 155 */ "ec_name ::= IDENTIFIER",
 /* 156 */ "opt_ec_initialization ::= ec_initialization",
 /* 157 */ "opt_ec_initialization ::=",
 /* 158 */ "update_enumeration_statement ::= KEY_UPDATE choice_ENUM_ERATION enumeration_name KEY_EQUALS KEY_OPEN_BRACE ec_update_list KEY_CLOSE_BRACE",
 /* 159 */ "ec_update_list ::= ec_update",
 /* 160 */ "ec_update_list ::= ec_update_list KEY_COMMA ec_update",
 /* 161 */ "ec_update ::= opt_ADD opt_DEFAULT ec_name ec_initialization",
 /* 162 */ "ec_update ::= KEY_UPDATE opt_DEFAULT ec_name ec_initialization",
 /* 163 */ "ec_update ::= KEY_REMOVE ec_name",
 /* 164 */ "ec_initialization ::= KEY_EQUALS signed_integer",
 /* 165 */ "signed_integer ::= INTEGER",
 /* 166 */ "signed_integer ::= KEY_DASH INTEGER",
 /* 167 */ "signed_integer ::= KEY_NIL",
 /* 168 */ "unsigned_integer ::= INTEGER",
 /* 169 */ "drop_enumeration_statement ::= KEY_DROP choice_ENUM_ERATION enumeration_name",
 /* 170 */ "create_segment_statement ::= KEY_CREATE KEY_SEGMENT segment_name KEY_RANGE KEY_EQUALS segment_range",
 /* 171 */ "segment_name ::= IDENTIFIER",
 /* 172 */ "segment_range ::= INTEGER KEY_DASH INTEGER",
 /* 173 */ "select_statement ::= select_clause in_clause with_max_range_clause returning_clause where_clause",
 /* 174 */ "select_statement ::= select_clause KEY_IN IDENTIFIER with_max_range_clause returning_clause where_clause",
 /* 175 */ "select_clause ::= KEY_SELECT focus_specification opt_OBJECTS",
 /* 176 */ "focus_specification ::= KEY_FOCUS",
 /* 177 */ "focus_specification ::= KEY_ALL",
 /* 178 */ "opt_OBJECTS ::= KEY_OBJECTS",
 /* 179 */ "opt_OBJECTS ::=",
 /* 180 */ "in_clause ::= KEY_IN in_specification",
 /* 181 */ "in_clause ::=",
 /* 182 */ "in_specification ::= monad_set",
 /* 183 */ "in_specification ::= KEY_ALL",
 /* 184 */ "monad_set ::= KEY_OPEN_BRACE monad_set_element_list KEY_CLOSE_BRACE",
 /* 185 */ "monad_set_element_list ::= monad_set_element",
 /* 186 */ "monad_set_element_list ::= monad_set_element_list KEY_COMMA monad_set_element",
 /* 187 */ "monad_set_element ::= INTEGER",
 /* 188 */ "monad_set_element ::= INTEGER KEY_DASH INTEGER",
 /* 189 */ "monad_set_element ::= INTEGER KEY_DASH",
 /* 190 */ "with_max_range_clause ::=",
 /* 191 */ "with_max_range_clause ::= KEY_WITH KEY_MAX KEY_RANGE KEY_MAX_M KEY_MONADS",
 /* 192 */ "with_max_range_clause ::= KEY_WITH KEY_MAX KEY_RANGE INTEGER KEY_MONADS",
 /* 193 */ "with_max_range_clause ::= KEY_WITH KEY_MAX KEY_RANGE KEY_FEATURE feature_name KEY_FROM KEY_OPEN_SQUARE_BRACKET object_type_name KEY_CLOSE_SQUARE_BRACKET",
 /* 194 */ "returning_clause ::=",
 /* 195 */ "returning_clause ::= KEY_RETURNING KEY_FULL KEY_SHEAF",
 /* 196 */ "returning_clause ::= KEY_RETURNING KEY_FLAT KEY_SHEAF",
 /* 197 */ "returning_clause ::= KEY_RETURNING KEY_FLAT KEY_SHEAF KEY_ON object_type_name_list",
 /* 198 */ "using_range_clause ::=",
 /* 199 */ "using_range_clause ::= KEY_RANGE KEY_ALL",
 /* 200 */ "using_range_clause ::= KEY_RANGE KEY_OPEN_BRACKET unsigned_integer KEY_COMMA unsigned_integer KEY_CLOSE_BRACKET",
 /* 201 */ "using_range_clause ::= KEY_RANGE KEY_OPEN_BRACKET unsigned_integer KEY_CLOSE_BRACKET",
 /* 202 */ "object_type_name_list ::= object_type_name",
 /* 203 */ "object_type_name_list ::= object_type_name_list KEY_COMMA object_type_name",
 /* 204 */ "where_clause ::= KEY_WHERE mql_query",
 /* 205 */ "select_objects_at_statement ::= KEY_SELECT opt_OBJECTS KEY_AT single_monad_specification KEY_OPEN_SQUARE_BRACKET object_type_name KEY_CLOSE_SQUARE_BRACKET",
 /* 206 */ "single_monad_specification ::= KEY_MONAD KEY_EQUALS INTEGER",
 /* 207 */ "select_objects_having_monads_in_statement ::= KEY_SELECT KEY_OBJECTS KEY_HAVING KEY_MONADS KEY_IN monad_set KEY_OPEN_SQUARE_BRACKET object_type_to_find KEY_CLOSE_SQUARE_BRACKET",
 /* 208 */ "object_type_to_find ::= choice_object_type_or_all",
 /* 209 */ "choice_object_type_or_all ::= object_type_name",
 /* 210 */ "choice_object_type_or_all ::= KEY_ALL",
 /* 211 */ "get_aggregate_features_statement ::= KEY_GET KEY_AGGREGATE KEY_FEATURES aggregate_feature_list KEY_FROM KEY_OBJECTS in_clause using_monad_feature KEY_OPEN_SQUARE_BRACKET object_type_name feature_constraints KEY_CLOSE_SQUARE_BRACKET",
 /* 212 */ "aggregate_feature_list ::= aggregate_feature",
 /* 213 */ "aggregate_feature_list ::= aggregate_feature_list KEY_COMMA aggregate_feature",
 /* 214 */ "aggregate_feature ::= KEY_MIN KEY_OPEN_BRACKET feature_name KEY_CLOSE_BRACKET",
 /* 215 */ "aggregate_feature ::= KEY_MAX KEY_OPEN_BRACKET feature_name KEY_CLOSE_BRACKET",
 /* 216 */ "aggregate_feature ::= KEY_SUM KEY_OPEN_BRACKET feature_name KEY_CLOSE_BRACKET",
 /* 217 */ "aggregate_feature ::= KEY_COUNT KEY_OPEN_BRACKET KEY_STAR KEY_CLOSE_BRACKET",
 /* 218 */ "aggregate_feature ::= KEY_COUNT KEY_OPEN_BRACKET aggregate_feature_comparison KEY_CLOSE_BRACKET",
 /* 219 */ "aggregate_feature_comparison ::= feature_name comparison_operator value",
 /* 220 */ "aggregate_feature_comparison ::= feature_name KEY_IN KEY_OPEN_BRACKET list_of_identifier KEY_CLOSE_BRACKET",
 /* 221 */ "aggregate_feature_comparison ::= feature_name KEY_IN KEY_OPEN_BRACKET list_of_integer KEY_CLOSE_BRACKET",
 /* 222 */ "get_objects_having_monads_in_statement ::= KEY_GET KEY_OBJECTS KEY_HAVING KEY_MONADS KEY_IN in_specification using_monad_feature KEY_OPEN_SQUARE_BRACKET object_type_name KEY_CLOSE_SQUARE_BRACKET",
 /* 223 */ "get_objects_having_monads_in_statement ::= KEY_GET KEY_OBJECTS KEY_HAVING KEY_MONADS KEY_IN in_specification using_monad_feature KEY_OPEN_SQUARE_BRACKET object_type_name KEY_GET KEY_ALL KEY_CLOSE_SQUARE_BRACKET",
 /* 224 */ "get_objects_having_monads_in_statement ::= KEY_GET KEY_OBJECTS KEY_HAVING KEY_MONADS KEY_IN in_specification using_monad_feature KEY_OPEN_SQUARE_BRACKET object_type_name KEY_GET feature_list KEY_CLOSE_SQUARE_BRACKET",
 /* 225 */ "using_monad_feature ::= KEY_USING KEY_MONAD KEY_FEATURE IDENTIFIER",
 /* 226 */ "using_monad_feature ::= KEY_USING KEY_MONAD KEY_FEATURE KEY_MONADS",
 /* 227 */ "using_monad_feature ::=",
 /* 228 */ "select_object_types_statement ::= KEY_SELECT opt_OBJECT KEY_TYPES",
 /* 229 */ "select_features_statement ::= KEY_SELECT KEY_FEATURES KEY_FROM opt_OBJECTYPE KEY_OPEN_SQUARE_BRACKET object_type_name KEY_CLOSE_SQUARE_BRACKET",
 /* 230 */ "get_set_from_feature_statement ::= KEY_GET KEY_SET KEY_FROM KEY_FEATURE feature_name KEY_OPEN_SQUARE_BRACKET object_type_name KEY_CLOSE_SQUARE_BRACKET",
 /* 231 */ "opt_OBJECTYPE ::= KEY_OBJECT KEY_TYPE",
 /* 232 */ "opt_OBJECTYPE ::= KEY_TYPE",
 /* 233 */ "opt_OBJECTYPE ::=",
 /* 234 */ "select_enumerations_statement ::= KEY_SELECT KEY_ENUMERATIONS",
 /* 235 */ "select_enumeration_constants_statement ::= KEY_SELECT choice_ENUM_ERATION KEY_CONSTANTS KEY_FROM opt_ENUM_ERATION enumeration_name",
 /* 236 */ "opt_ENUM_ERATION ::= KEY_ENUM",
 /* 237 */ "opt_ENUM_ERATION ::= KEY_ENUMERATION",
 /* 238 */ "opt_ENUM_ERATION ::=",
 /* 239 */ "select_object_types_which_use_enum_statement ::= KEY_SELECT opt_OBJECT KEY_TYPES KEY_USING choice_ENUM_ERATION enumeration_name",
 /* 240 */ "select_min_m_statement ::= KEY_SELECT KEY_MIN_M",
 /* 241 */ "select_max_m_statement ::= KEY_SELECT KEY_MAX_M",
 /* 242 */ "create_object_from_monads_statement ::= KEY_CREATE KEY_OBJECT KEY_FROM monad_specification with_id_d_specification object_creation_specification",
 /* 243 */ "monad_specification ::= KEY_MONADS KEY_EQUALS monad_set",
 /* 244 */ "with_id_d_specification ::= KEY_WITH KEY_ID_D KEY_EQUALS id_d_const",
 /* 245 */ "with_id_d_specification ::=",
 /* 246 */ "id_d_const ::= INTEGER",
 /* 247 */ "id_d_const ::= KEY_NIL",
 /* 248 */ "object_creation_specification ::= KEY_OPEN_SQUARE_BRACKET object_type_name opt_list_of_feature_assignments KEY_CLOSE_SQUARE_BRACKET",
 /* 249 */ "opt_list_of_feature_assignments ::= list_of_feature_assignments",
 /* 250 */ "opt_list_of_feature_assignments ::=",
 /* 251 */ "list_of_feature_assignments ::= feature_assignment",
 /* 252 */ "list_of_feature_assignments ::= list_of_feature_assignments feature_assignment",
 /* 253 */ "feature_assignment ::= feature_name KEY_ASSIGN expression KEY_SEMICOLON",
 /* 254 */ "feature_name ::= IDENTIFIER",
 /* 255 */ "feature_name ::= KEY_MONADS",
 /* 256 */ "expression ::= signed_integer",
 /* 257 */ "expression ::= STRING",
 /* 258 */ "expression ::= IDENTIFIER",
 /* 259 */ "expression ::= monad_set",
 /* 260 */ "expression ::= KEY_OPEN_BRACKET KEY_CLOSE_BRACKET",
 /* 261 */ "expression ::= KEY_OPEN_BRACKET list_of_integer KEY_CLOSE_BRACKET",
 /* 262 */ "expression ::= KEY_OPEN_BRACKET list_of_identifier KEY_CLOSE_BRACKET",
 /* 263 */ "list_of_integer ::= signed_integer",
 /* 264 */ "list_of_integer ::= list_of_integer KEY_COMMA signed_integer",
 /* 265 */ "list_of_identifier ::= IDENTIFIER",
 /* 266 */ "list_of_identifier ::= list_of_identifier KEY_COMMA IDENTIFIER",
 /* 267 */ "create_object_from_id_ds_statement ::= KEY_CREATE KEY_OBJECT KEY_FROM id_ds_specification with_id_d_specification object_creation_specification",
 /* 268 */ "id_ds_specification ::= choice_number_ID_DS KEY_EQUALS id_d_list",
 /* 269 */ "choice_number_ID_DS ::= KEY_ID_D",
 /* 270 */ "choice_number_ID_DS ::= KEY_ID_DS",
 /* 271 */ "id_d_list ::= id_d",
 /* 272 */ "id_d_list ::= id_d_list KEY_COMMA id_d",
 /* 273 */ "id_d ::= id_d_const",
 /* 274 */ "create_objects_statement ::= KEY_CREATE KEY_OBJECTS KEY_WITH KEY_OBJECT KEY_TYPE KEY_OPEN_SQUARE_BRACKET object_type_name KEY_CLOSE_SQUARE_BRACKET object_creation_list",
 /* 275 */ "object_creation_list ::= object_creation_no_object_type",
 /* 276 */ "object_creation_list ::= object_creation_list object_creation_no_object_type",
 /* 277 */ "object_creation_no_object_type ::= KEY_CREATE KEY_OBJECT KEY_FROM monad_specification with_id_d_specification KEY_OPEN_SQUARE_BRACKET opt_list_of_feature_assignments KEY_CLOSE_SQUARE_BRACKET",
 /* 278 */ "update_objects_by_monads_statement ::= KEY_UPDATE choice_number_OBJECTS KEY_BY monad_specification object_update_specification",
 /* 279 */ "choice_number_OBJECTS ::= KEY_OBJECT",
 /* 280 */ "choice_number_OBJECTS ::= KEY_OBJECTS",
 /* 281 */ "object_update_specification ::= KEY_OPEN_SQUARE_BRACKET object_type_name list_of_feature_assignments KEY_CLOSE_SQUARE_BRACKET",
 /* 282 */ "update_objects_by_id_ds_statement ::= KEY_UPDATE choice_number_OBJECTS KEY_BY id_ds_specification object_update_specification",
 /* 283 */ "delete_objects_by_monads_statement ::= KEY_DELETE choice_number_OBJECTS KEY_BY monad_specification object_deletion_specification",
 /* 284 */ "object_deletion_specification ::= KEY_OPEN_SQUARE_BRACKET object_type_name_to_delete KEY_CLOSE_SQUARE_BRACKET",
 /* 285 */ "object_type_name_to_delete ::= choice_object_type_or_all",
 /* 286 */ "delete_objects_by_id_ds_statement ::= KEY_DELETE choice_number_OBJECTS KEY_BY id_ds_specification object_deletion_specification",
 /* 287 */ "get_features_statement ::= KEY_GET choice_number_FEATURES feature_list KEY_FROM choice_number_OBJECTS KEY_WITH id_ds_specification KEY_OPEN_SQUARE_BRACKET object_type_name KEY_CLOSE_SQUARE_BRACKET",
 /* 288 */ "choice_number_FEATURES ::= KEY_FEATURE",
 /* 289 */ "choice_number_FEATURES ::= KEY_FEATURES",
 /* 290 */ "feature_list ::= feature_name",
 /* 291 */ "feature_list ::= feature_list KEY_COMMA feature_name",
 /* 292 */ "quit_statement ::= KEY_QUIT",
 /* 293 */ "mql_query ::= topograph",
 /* 294 */ "topograph ::= blocks",
 /* 295 */ "blocks ::= using_range_clause block_string",
 /* 296 */ "blocks ::= using_range_clause unordered_group",
 /* 297 */ "block_string0 ::= block",
 /* 298 */ "block_string0 ::= KEY_OPEN_SQUARE_BRACKET block_string KEY_CLOSE_SQUARE_BRACKET",
 /* 299 */ "block_string1 ::= block_string0",
 /* 300 */ "block_string1 ::= block_string0 KEY_STAR star_monad_set",
 /* 301 */ "block_string2 ::= block_string1",
 /* 302 */ "block_string2 ::= block_string1 block_string2",
 /* 303 */ "block_string2 ::= block_string1 KEY_EXCLAMATION block_string2",
 /* 304 */ "block_string ::= block_string2",
 /* 305 */ "block_string ::= block_string2 KEY_OR block_string",
 /* 306 */ "notexist ::= KEY_NOTEXIST",
 /* 307 */ "notexist ::= KEY_NOTEXISTS",
 /* 308 */ "object_reference_declaration ::=",
 /* 309 */ "object_reference_declaration ::= KEY_AS object_reference",
 /* 310 */ "mark_declaration ::=",
 /* 311 */ "mark_declaration ::= MARK",
 /* 312 */ "object_reference ::= IDENTIFIER",
 /* 313 */ "retrieval ::=",
 /* 314 */ "retrieval ::= KEY_NORETRIEVE",
 /* 315 */ "retrieval ::= KEY_RETRIEVE",
 /* 316 */ "retrieval ::= KEY_FOCUS",
 /* 317 */ "firstlast ::=",
 /* 318 */ "firstlast ::= KEY_FIRST",
 /* 319 */ "firstlast ::= KEY_LAST",
 /* 320 */ "firstlast ::= KEY_FIRST KEY_AND KEY_LAST",
 /* 321 */ "monad_set_relation_clause ::= monad_set_relation_operation KEY_OPEN_BRACKET monad_set_name KEY_COMMA universe_or_substrate KEY_CLOSE_BRACKET",
 /* 322 */ "monad_set_relation_clause ::= monad_set_relation_operation KEY_OPEN_BRACKET universe_or_substrate KEY_CLOSE_BRACKET",
 /* 323 */ "monad_set_relation_clause ::=",
 /* 324 */ "monad_set_relation_operation ::= KEY_PART_OF",
 /* 325 */ "monad_set_relation_operation ::= KEY_STARTS_IN",
 /* 326 */ "monad_set_relation_operation ::= KEY_OVERLAP",
 /* 327 */ "universe_or_substrate ::= KEY_UNIVERSE",
 /* 328 */ "universe_or_substrate ::= KEY_SUBSTRATE",
 /* 329 */ "feature_constraints ::=",
 /* 330 */ "feature_constraints ::= ffeatures",
 /* 331 */ "ffeatures ::= fterm",
 /* 332 */ "ffeatures ::= ffeatures KEY_OR fterm",
 /* 333 */ "fterm ::= ffactor",
 /* 334 */ "fterm ::= fterm KEY_AND ffactor",
 /* 335 */ "ffactor ::= KEY_NOT ffactor",
 /* 336 */ "ffactor ::= KEY_OPEN_BRACKET ffeatures KEY_CLOSE_BRACKET",
 /* 337 */ "ffactor ::= feature_comparison",
 /* 338 */ "feature_comparison ::= feature_name comparison_operator value",
 /* 339 */ "feature_comparison ::= feature_name KEY_IN KEY_OPEN_BRACKET list_of_identifier KEY_CLOSE_BRACKET",
 /* 340 */ "feature_comparison ::= feature_name KEY_IN KEY_OPEN_BRACKET list_of_integer KEY_CLOSE_BRACKET",
 /* 341 */ "feature_comparison ::= feature_name KEY_IN object_reference_usage",
 /* 342 */ "comparison_operator ::= KEY_EQUALS",
 /* 343 */ "comparison_operator ::= KEY_LESS_THAN",
 /* 344 */ "comparison_operator ::= KEY_GREATER_THAN",
 /* 345 */ "comparison_operator ::= KEY_NOT_EQUAL",
 /* 346 */ "comparison_operator ::= KEY_LESS_THAN_OR_EQUAL",
 /* 347 */ "comparison_operator ::= KEY_GREATER_THAN_OR_EQUAL",
 /* 348 */ "comparison_operator ::= KEY_TILDE",
 /* 349 */ "comparison_operator ::= KEY_NOT_TILDE",
 /* 350 */ "comparison_operator ::= KEY_HAS",
 /* 351 */ "value ::= enum_const",
 /* 352 */ "value ::= signed_integer",
 /* 353 */ "value ::= STRING",
 /* 354 */ "value ::= object_reference_usage",
 /* 355 */ "enum_const ::= IDENTIFIER",
 /* 356 */ "object_reference_usage ::= object_reference KEY_DOT feature_name",
 /* 357 */ "opt_blocks ::=",
 /* 358 */ "opt_blocks ::= blocks",
 /* 359 */ "block ::= object_block",
 /* 360 */ "block ::= power",
 /* 361 */ "block ::= opt_gap_block",
 /* 362 */ "block ::= gap_block",
 /* 363 */ "block ::= notexist_object_block",
 /* 364 */ "unordered_group ::= KEY_OPEN_SQUARE_BRACKET KEY_UNORDERED_GROUP object_block_string KEY_CLOSE_SQUARE_BRACKET",
 /* 365 */ "object_block_string ::= object_block",
 /* 366 */ "object_block_string ::= object_block_string object_block",
 /* 367 */ "star_monad_set ::=",
 /* 368 */ "star_monad_set ::= monad_set",
 /* 369 */ "opt_gap_block ::= KEY_OPEN_SQUARE_BRACKET KEY_OPT_GAP mark_declaration gap_retrieval opt_blocks KEY_CLOSE_SQUARE_BRACKET",
 /* 370 */ "gap_retrieval ::=",
 /* 371 */ "gap_retrieval ::= KEY_NORETRIEVE",
 /* 372 */ "gap_retrieval ::= KEY_RETRIEVE",
 /* 373 */ "gap_retrieval ::= KEY_FOCUS",
 /* 374 */ "gap_block ::= KEY_OPEN_SQUARE_BRACKET KEY_GAP mark_declaration gap_retrieval opt_blocks KEY_CLOSE_SQUARE_BRACKET",
 /* 375 */ "feature_retrieval ::= KEY_GET feature_list",
 /* 376 */ "feature_retrieval ::=",
 /* 377 */ "object_block ::= KEY_OPEN_SQUARE_BRACKET object_type_name mark_declaration object_reference_declaration retrieval firstlast monad_set_relation_clause feature_constraints feature_retrieval opt_blocks KEY_CLOSE_SQUARE_BRACKET",
 /* 378 */ "notexist_object_block ::= notexist KEY_OPEN_SQUARE_BRACKET object_type_name mark_declaration object_reference_declaration retrieval firstlast monad_set_relation_clause feature_constraints feature_retrieval opt_blocks KEY_CLOSE_SQUARE_BRACKET",
 /* 379 */ "power ::= KEY_POWER restrictor",
 /* 380 */ "power ::= KEY_POWER KEY_BETWEEN limit KEY_AND limit",
 /* 381 */ "restrictor ::=",
 /* 382 */ "restrictor ::= KEY_LESS_THAN limit",
 /* 383 */ "restrictor ::= KEY_LESS_THAN_OR_EQUAL limit",
 /* 384 */ "limit ::= INTEGER",
};
#endif /* NDEBUG */


#if MQLYYSTACKDEPTH<=0
/*
** Try to increase the size of the parser stack.  Return the number
** of errors.  Return 0 on success.
*/
static int mqlyyGrowStack(mqlyyParser *p){
  int newSize;
  int idx;
  mqlyyStackEntry *pNew;

  newSize = p->mqlyystksz*2 + 100;
  idx = p->mqlyytos ? (int)(p->mqlyytos - p->mqlyystack) : 0;
  if( p->mqlyystack==&p->mqlyystk0 ){
    pNew = malloc(newSize*sizeof(pNew[0]));
    if( pNew ) pNew[0] = p->mqlyystk0;
  }else{
    pNew = realloc(p->mqlyystack, newSize*sizeof(pNew[0]));
  }
  if( pNew ){
    p->mqlyystack = pNew;
    p->mqlyytos = &p->mqlyystack[idx];
#ifndef NDEBUG
    if( mqlyyTraceFILE ){
      fprintf(mqlyyTraceFILE,"%sStack grows from %d to %d entries.\n",
              mqlyyTracePrompt, p->mqlyystksz, newSize);
    }
#endif
    p->mqlyystksz = newSize;
  }
  return pNew==0; 
}
#endif

/* Datatype of the argument to the memory allocated passed as the
** second argument to MQLParserAlloc() below.  This can be changed by
** putting an appropriate #define in the %include section of the input
** grammar.
*/
#ifndef MQLYYMALLOCARGTYPE
# define MQLYYMALLOCARGTYPE size_t
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
** to MQLParser and MQLParserFree.
*/
void *MQLParserAlloc(void *(*mallocProc)(MQLYYMALLOCARGTYPE)){
  mqlyyParser *pParser;
  pParser = (mqlyyParser*)(*mallocProc)( (MQLYYMALLOCARGTYPE)sizeof(mqlyyParser) );
  if( pParser ){
#ifdef MQLYYTRACKMAXSTACKDEPTH
    pParser->mqlyyhwm = 0;
#endif
#if MQLYYSTACKDEPTH<=0
    pParser->mqlyytos = NULL;
    pParser->mqlyystack = NULL;
    pParser->mqlyystksz = 0;
    if( mqlyyGrowStack(pParser) ){
      pParser->mqlyystack = &pParser->mqlyystk0;
      pParser->mqlyystksz = 1;
    }
#endif
#ifndef MQLYYNOERRORRECOVERY
    pParser->mqlyyerrcnt = -1;
#endif
    pParser->mqlyytos = pParser->mqlyystack;
    pParser->mqlyystack[0].stateno = 0;
    pParser->mqlyystack[0].major = 0;
  }
  return pParser;
}

/* The following function deletes the "minor type" or semantic value
** associated with a symbol.  The symbol can be either a terminal
** or nonterminal. "mqlyymajor" is the symbol code, and "mqlyypminor" is
** a pointer to the value to be deleted.  The code used to do the 
** deletions is derived from the %destructor and/or %token_destructor
** directives of the input grammar.
*/
static void mqlyy_destructor(
  mqlyyParser *mqlyypParser,    /* The parser */
  MQLYYCODETYPE mqlyymajor,     /* Type code for object to destroy */
  MQLYYMINORTYPE *mqlyypminor   /* The object to be destroyed */
){
  MQLParserARG_FETCH;
  switch( mqlyymajor ){
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
    case 1: /* KEY_GO */
    case 2: /* KEY_CREATE */
    case 3: /* KEY_DATABASE */
    case 4: /* IDENTIFIER */
    case 5: /* STRING */
    case 6: /* KEY_INITIALIZE */
    case 7: /* KEY_USE */
    case 8: /* KEY_WITH */
    case 9: /* KEY_KEY */
    case 10: /* KEY_USING */
    case 11: /* KEY_ENCODING */
    case 12: /* KEY_DROP */
    case 13: /* KEY_VACUUM */
    case 14: /* KEY_ANALYZE */
    case 15: /* KEY_INDEXES */
    case 16: /* KEY_ON */
    case 17: /* KEY_OBJECT */
    case 18: /* KEY_OPEN_SQUARE_BRACKET */
    case 19: /* KEY_CLOSE_SQUARE_BRACKET */
    case 20: /* KEY_TYPE */
    case 21: /* KEY_TYPES */
    case 22: /* KEY_BEGIN */
    case 23: /* KEY_TRANSACTION */
    case 24: /* KEY_COMMIT */
    case 25: /* KEY_ABORT */
    case 26: /* KEY_MULTIPLE */
    case 27: /* KEY_RANGE */
    case 28: /* KEY_OBJECTS */
    case 29: /* KEY_SINGLE */
    case 30: /* KEY_MONAD */
    case 31: /* KEY_HAVING */
    case 32: /* KEY_UNIQUE */
    case 33: /* KEY_FIRST */
    case 34: /* KEY_MONADS */
    case 35: /* KEY_AND */
    case 36: /* KEY_LAST */
    case 37: /* KEY_WITHOUT */
    case 38: /* KEY_COLON */
    case 39: /* KEY_SEMICOLON */
    case 40: /* KEY_LIST */
    case 41: /* KEY_OF */
    case 42: /* KEY_INTEGER */
    case 43: /* KEY_STRING */
    case 44: /* KEY_ASCII */
    case 45: /* KEY_ID_D */
    case 46: /* KEY_SET */
    case 47: /* KEY_INDEX */
    case 48: /* KEY_IF */
    case 49: /* KEY_NOT */
    case 50: /* KEY_EXISTS */
    case 51: /* KEY_FROM */
    case 52: /* KEY_OPEN_BRACKET */
    case 53: /* INTEGER */
    case 54: /* KEY_CLOSE_BRACKET */
    case 55: /* KEY_DEFAULT */
    case 56: /* KEY_COMPUTED */
    case 57: /* KEY_UPDATE */
    case 58: /* KEY_REMOVE */
    case 59: /* KEY_ADD */
    case 60: /* KEY_INSERT */
    case 61: /* KEY_DELETE */
    case 62: /* KEY_GET */
    case 63: /* KEY_CALCULATION */
    case 64: /* KEY_UNION */
    case 65: /* KEY_DIFFERENCE */
    case 66: /* KEY_INTERSECT */
    case 67: /* KEY_SELECT */
    case 68: /* KEY_SETS */
    case 69: /* KEY_ALL */
    case 70: /* KEY_REPLACE */
    case 71: /* KEY_COMMA */
    case 72: /* KEY_EQUALS */
    case 73: /* KEY_OPEN_BRACE */
    case 74: /* KEY_CLOSE_BRACE */
    case 75: /* KEY_ENUMERATION */
    case 76: /* KEY_ENUM */
    case 77: /* KEY_DASH */
    case 78: /* KEY_NIL */
    case 79: /* KEY_SEGMENT */
    case 80: /* KEY_IN */
    case 81: /* KEY_FOCUS */
    case 82: /* KEY_MAX */
    case 83: /* KEY_MAX_M */
    case 84: /* KEY_FEATURE */
    case 85: /* KEY_RETURNING */
    case 86: /* KEY_FULL */
    case 87: /* KEY_SHEAF */
    case 88: /* KEY_FLAT */
    case 89: /* KEY_WHERE */
    case 90: /* KEY_AT */
    case 91: /* KEY_AGGREGATE */
    case 92: /* KEY_FEATURES */
    case 93: /* KEY_MIN */
    case 94: /* KEY_SUM */
    case 95: /* KEY_COUNT */
    case 96: /* KEY_STAR */
    case 97: /* KEY_ENUMERATIONS */
    case 98: /* KEY_CONSTANTS */
    case 99: /* KEY_MIN_M */
    case 100: /* KEY_ASSIGN */
    case 101: /* KEY_ID_DS */
    case 102: /* KEY_BY */
    case 103: /* KEY_QUIT */
    case 104: /* KEY_EXCLAMATION */
    case 105: /* KEY_OR */
    case 106: /* KEY_NOTEXIST */
    case 107: /* KEY_NOTEXISTS */
    case 108: /* KEY_AS */
    case 109: /* MARK */
    case 110: /* KEY_NORETRIEVE */
    case 111: /* KEY_RETRIEVE */
    case 112: /* KEY_PART_OF */
    case 113: /* KEY_STARTS_IN */
    case 114: /* KEY_OVERLAP */
    case 115: /* KEY_UNIVERSE */
    case 116: /* KEY_SUBSTRATE */
    case 117: /* KEY_LESS_THAN */
    case 118: /* KEY_GREATER_THAN */
    case 119: /* KEY_NOT_EQUAL */
    case 120: /* KEY_LESS_THAN_OR_EQUAL */
    case 121: /* KEY_GREATER_THAN_OR_EQUAL */
    case 122: /* KEY_TILDE */
    case 123: /* KEY_NOT_TILDE */
    case 124: /* KEY_HAS */
    case 125: /* KEY_DOT */
    case 126: /* KEY_UNORDERED_GROUP */
    case 127: /* KEY_OPT_GAP */
    case 128: /* KEY_GAP */
    case 129: /* KEY_POWER */
    case 130: /* KEY_BETWEEN */
{
#line 130 "./mql.yxx"
 deleteToken((mqlyypminor->mqlyy0)); UNUSED(pEE); 
#line 1503 "./mql.c"
}
      break;
    case 132: /* statement */
    case 133: /* statement_by_itself */
    case 134: /* create_database_statement */
    case 135: /* initialize_database_statement */
    case 136: /* use_statement */
    case 137: /* drop_database_statement */
    case 138: /* vacuum_database_statement */
    case 139: /* create_object_type_statement */
    case 140: /* update_object_type_statement */
    case 141: /* drop_object_type_statement */
    case 142: /* insert_monads_statement */
    case 143: /* delete_monads_statement */
    case 144: /* get_monads_statement */
    case 145: /* monad_set_calculation_statement */
    case 146: /* create_enumeration_statement */
    case 147: /* update_enumeration_statement */
    case 148: /* drop_enumeration_statement */
    case 149: /* create_segment_statement */
    case 150: /* select_statement */
    case 151: /* select_objects_at_statement */
    case 152: /* select_objects_having_monads_in_statement */
    case 153: /* get_aggregate_features_statement */
    case 154: /* get_objects_having_monads_in_statement */
    case 156: /* select_object_types_statement */
    case 158: /* select_enumerations_statement */
    case 159: /* select_enumeration_constants_statement */
    case 160: /* select_object_types_which_use_enum_statement */
    case 161: /* select_min_m_statement */
    case 162: /* select_max_m_statement */
    case 163: /* create_object_from_monads_statement */
    case 164: /* create_object_from_id_ds_statement */
    case 165: /* update_objects_by_monads_statement */
    case 166: /* update_objects_by_id_ds_statement */
    case 167: /* delete_objects_by_monads_statement */
    case 168: /* delete_objects_by_id_ds_statement */
    case 169: /* get_features_statement */
    case 170: /* quit_statement */
    case 171: /* create_indexes_statement */
    case 172: /* drop_indexes_statement */
    case 173: /* begin_transaction_statement */
    case 174: /* commit_transaction_statement */
    case 175: /* abort_transaction_statement */
    case 176: /* select_monad_sets_statement */
    case 177: /* get_monad_sets_statement */
    case 178: /* create_monad_set_statement */
    case 179: /* update_monad_set_statement */
    case 180: /* drop_monad_set_statement */
    case 181: /* create_objects_statement */
{
#line 152 "./mql.yxx"
delete((mqlyypminor->mqlyy50));
#line 1557 "./mql.c"
}
      break;
    case 182: /* database_name */
    case 183: /* opt_WITH_KEY */
    case 186: /* opt_WITH_ENCODING */
    case 188: /* on_object_type */
    case 190: /* choice_object_type_or_all */
    case 195: /* object_type_name */
    case 199: /* feature_name */
    case 221: /* enumeration_name */
    case 232: /* segment_name */
    case 248: /* object_type_to_find */
    case 250: /* using_monad_feature */
    case 273: /* object_deletion_specification */
    case 274: /* object_type_name_to_delete */
    case 286: /* object_reference_declaration */
    case 287: /* object_reference */
    case 288: /* mark_declaration */
    case 299: /* enum_const */
{
#line 217 "./mql.yxx"
 deleteToken((mqlyypminor->mqlyy0)); 
#line 1580 "./mql.c"
}
      break;
    case 185: /* opt_DATABASE */
    case 189: /* choice_type_types */
    case 191: /* opt_OBJECT */
    case 210: /* opt_ADD */
    case 212: /* choice_number_OBJECTS */
    case 217: /* choice_number_SET */
    case 220: /* choice_ENUM_ERATION */
    case 240: /* opt_OBJECTS */
    case 259: /* opt_OBJECTYPE */
    case 260: /* opt_ENUM_ERATION */
    case 267: /* choice_number_ID_DS */
    case 275: /* choice_number_FEATURES */
    case 285: /* notexist */
{
#line 245 "./mql.yxx"
;
#line 1599 "./mql.c"
}
      break;
    case 187: /* opt_ANALYZE */
    case 192: /* opt_if_not_exists */
    case 202: /* opt_computed */
    case 204: /* opt_with_index */
    case 206: /* opt_from_set */
    case 224: /* opt_DEFAULT */
{
#line 263 "./mql.yxx"
;
#line 1611 "./mql.c"
}
      break;
    case 193: /* opt_range_type */
{
#line 328 "./mql.yxx"
;
#line 1618 "./mql.c"
}
      break;
    case 194: /* opt_monad_uniqueness_type */
{
#line 339 "./mql.yxx"
;
#line 1625 "./mql.c"
}
      break;
    case 196: /* opt_feature_declaration_list */
    case 197: /* feature_declaration_list */
    case 198: /* feature_declaration */
{
#line 361 "./mql.yxx"
delete((mqlyypminor->mqlyy289));
#line 1634 "./mql.c"
}
      break;
    case 200: /* feature_type */
    case 203: /* list_feature_type */
{
#line 383 "./mql.yxx"
delete((mqlyypminor->mqlyy264));
#line 1642 "./mql.c"
}
      break;
    case 201: /* default_specification */
    case 207: /* expression */
{
#line 441 "./mql.yxx"
delete((mqlyypminor->mqlyy413));
#line 1650 "./mql.c"
}
      break;
    case 205: /* opt_string_length */
{
#line 433 "./mql.yxx"
;
#line 1657 "./mql.c"
}
      break;
    case 208: /* feature_update_list */
    case 209: /* feature_update */
{
#line 466 "./mql.yxx"
delete((mqlyypminor->mqlyy380));
#line 1665 "./mql.c"
}
      break;
    case 211: /* monad_specification */
    case 215: /* monad_set */
    case 233: /* segment_range */
    case 235: /* in_clause */
    case 241: /* in_specification */
    case 242: /* monad_set_element_list */
    case 243: /* monad_set_element */
    case 283: /* star_monad_set */
{
#line 1081 "./mql.yxx"
delete((mqlyypminor->mqlyy108));
#line 1679 "./mql.c"
}
      break;
    case 213: /* id_ds_specification */
    case 268: /* id_d_list */
    case 269: /* id_d */
{
#line 1182 "./mql.yxx"
delete((mqlyypminor->mqlyy43));
#line 1688 "./mql.c"
}
      break;
    case 214: /* monad_set_chain */
{
#line 527 "./mql.yxx"
delete((mqlyypminor->mqlyy51));
#line 1695 "./mql.c"
}
      break;
    case 216: /* monad_set_operator */
{
#line 535 "./mql.yxx"
;
#line 1702 "./mql.c"
}
      break;
    case 218: /* monad_set_name_list */
    case 244: /* object_type_name_list */
    case 256: /* list_of_identifier */
{
#line 611 "./mql.yxx"
delete((mqlyypminor->mqlyy336));
#line 1711 "./mql.c"
}
      break;
    case 219: /* monad_set_name */
{
#line 606 "./mql.yxx"
deleteToken((mqlyypminor->mqlyy0));
#line 1718 "./mql.c"
}
      break;
    case 222: /* ec_declaration_list */
    case 223: /* ec_declaration */
{
#line 639 "./mql.yxx"
delete((mqlyypminor->mqlyy455));
#line 1726 "./mql.c"
}
      break;
    case 225: /* ec_name */
{
#line 662 "./mql.yxx"
 deleteToken((mqlyypminor->mqlyy0));  
#line 1733 "./mql.c"
}
      break;
    case 226: /* opt_ec_initialization */
{
#line 666 "./mql.yxx"
delete((mqlyypminor->mqlyy473));
#line 1740 "./mql.c"
}
      break;
    case 227: /* ec_initialization */
    case 230: /* signed_integer */
    case 231: /* unsigned_integer */
{
#line 701 "./mql.yxx"
;
#line 1749 "./mql.c"
}
      break;
    case 228: /* ec_update_list */
    case 229: /* ec_update */
{
#line 683 "./mql.yxx"
delete((mqlyypminor->mqlyy46));
#line 1757 "./mql.c"
}
      break;
    case 234: /* select_clause */
    case 239: /* focus_specification */
{
#line 761 "./mql.yxx"
;
#line 1765 "./mql.c"
}
      break;
    case 236: /* with_max_range_clause */
{
#line 820 "./mql.yxx"
delete((mqlyypminor->mqlyy448));
#line 1772 "./mql.c"
}
      break;
    case 237: /* returning_clause */
{
#line 832 "./mql.yxx"
delete((mqlyypminor->mqlyy551));
#line 1779 "./mql.c"
}
      break;
    case 238: /* where_clause */
    case 246: /* mql_query */
    case 276: /* topograph */
{
#line 873 "./mql.yxx"
delete((mqlyypminor->mqlyy269));
#line 1788 "./mql.c"
}
      break;
    case 245: /* using_range_clause */
{
#line 847 "./mql.yxx"
delete((mqlyypminor->mqlyy600));
#line 1795 "./mql.c"
}
      break;
    case 247: /* single_monad_specification */
    case 309: /* restrictor */
    case 310: /* limit */
{
#line 888 "./mql.yxx"
;
#line 1804 "./mql.c"
}
      break;
    case 249: /* aggregate_feature_list */
    case 252: /* aggregate_feature */
{
#line 930 "./mql.yxx"
delete((mqlyypminor->mqlyy344));
#line 1812 "./mql.c"
}
      break;
    case 251: /* feature_constraints */
    case 294: /* ffeatures */
{
#line 1457 "./mql.yxx"
delete((mqlyypminor->mqlyy520));
#line 1820 "./mql.c"
}
      break;
    case 253: /* aggregate_feature_comparison */
    case 297: /* feature_comparison */
{
#line 953 "./mql.yxx"
delete((mqlyypminor->mqlyy68));
#line 1828 "./mql.c"
}
      break;
    case 254: /* comparison_operator */
{
#line 1496 "./mql.yxx"
;
#line 1835 "./mql.c"
}
      break;
    case 255: /* value */
{
#line 1510 "./mql.yxx"
delete((mqlyypminor->mqlyy616));
#line 1842 "./mql.c"
}
      break;
    case 257: /* list_of_integer */
{
#line 1155 "./mql.yxx"
delete((mqlyypminor->mqlyy17));
#line 1849 "./mql.c"
}
      break;
    case 258: /* feature_list */
    case 308: /* feature_retrieval */
{
#line 1328 "./mql.yxx"
delete((mqlyypminor->mqlyy366));
#line 1857 "./mql.c"
}
      break;
    case 261: /* with_id_d_specification */
    case 263: /* id_d_const */
{
#line 1085 "./mql.yxx"
;
#line 1865 "./mql.c"
}
      break;
    case 262: /* object_creation_specification */
    case 272: /* object_update_specification */
{
#line 1100 "./mql.yxx"
delete((mqlyypminor->mqlyy275));
#line 1873 "./mql.c"
}
      break;
    case 264: /* opt_list_of_feature_assignments */
    case 265: /* list_of_feature_assignments */
    case 266: /* feature_assignment */
{
#line 1109 "./mql.yxx"
delete((mqlyypminor->mqlyy265));
#line 1882 "./mql.c"
}
      break;
    case 270: /* object_creation_list */
    case 271: /* object_creation_no_object_type */
{
#line 1219 "./mql.yxx"
delete((mqlyypminor->mqlyy481));
#line 1890 "./mql.c"
}
      break;
    case 277: /* blocks */
    case 300: /* opt_blocks */
{
#line 1354 "./mql.yxx"
delete((mqlyypminor->mqlyy18));
#line 1898 "./mql.c"
}
      break;
    case 278: /* block_string */
{
#line 1386 "./mql.yxx"
delete((mqlyypminor->mqlyy501));
#line 1905 "./mql.c"
}
      break;
    case 279: /* unordered_group */
{
#line 1547 "./mql.yxx"
delete((mqlyypminor->mqlyy24));
#line 1912 "./mql.c"
}
      break;
    case 280: /* block_string0 */
{
#line 1362 "./mql.yxx"
delete((mqlyypminor->mqlyy63));
#line 1919 "./mql.c"
}
      break;
    case 281: /* block */
{
#line 1538 "./mql.yxx"
delete((mqlyypminor->mqlyy218));
#line 1926 "./mql.c"
}
      break;
    case 282: /* block_string1 */
{
#line 1369 "./mql.yxx"
delete((mqlyypminor->mqlyy116));
#line 1933 "./mql.c"
}
      break;
    case 284: /* block_string2 */
{
#line 1376 "./mql.yxx"
delete((mqlyypminor->mqlyy169));
#line 1940 "./mql.c"
}
      break;
    case 289: /* retrieval */
    case 307: /* gap_retrieval */
{
#line 1416 "./mql.yxx"
;
#line 1948 "./mql.c"
}
      break;
    case 290: /* firstlast */
{
#line 1423 "./mql.yxx"
;
#line 1955 "./mql.c"
}
      break;
    case 291: /* monad_set_relation_clause */
{
#line 1430 "./mql.yxx"
delete((mqlyypminor->mqlyy299));
#line 1962 "./mql.c"
}
      break;
    case 292: /* monad_set_relation_operation */
{
#line 1439 "./mql.yxx"

#line 1969 "./mql.c"
}
      break;
    case 293: /* universe_or_substrate */
{
#line 1449 "./mql.yxx"
 
#line 1976 "./mql.c"
}
      break;
    case 295: /* fterm */
{
#line 1469 "./mql.yxx"
delete((mqlyypminor->mqlyy601));
#line 1983 "./mql.c"
}
      break;
    case 296: /* ffactor */
{
#line 1474 "./mql.yxx"
delete((mqlyypminor->mqlyy272));
#line 1990 "./mql.c"
}
      break;
    case 298: /* object_reference_usage */
{
#line 1526 "./mql.yxx"
delete((mqlyypminor->mqlyy266));
#line 1997 "./mql.c"
}
      break;
    case 301: /* object_block */
    case 305: /* notexist_object_block */
{
#line 1596 "./mql.yxx"
delete((mqlyypminor->mqlyy101));
#line 2005 "./mql.c"
}
      break;
    case 302: /* power */
{
#line 1625 "./mql.yxx"
delete((mqlyypminor->mqlyy594));
#line 2012 "./mql.c"
}
      break;
    case 303: /* opt_gap_block */
{
#line 1568 "./mql.yxx"
delete((mqlyypminor->mqlyy303));
#line 2019 "./mql.c"
}
      break;
    case 304: /* gap_block */
{
#line 1582 "./mql.yxx"
delete((mqlyypminor->mqlyy6));
#line 2026 "./mql.c"
}
      break;
    case 306: /* object_block_string */
{
#line 1555 "./mql.yxx"
delete((mqlyypminor->mqlyy98));
#line 2033 "./mql.c"
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
static void mqlyy_pop_parser_stack(mqlyyParser *pParser){
  mqlyyStackEntry *mqlyytos;
  assert( pParser->mqlyytos!=0 );
  assert( pParser->mqlyytos > pParser->mqlyystack );
  mqlyytos = pParser->mqlyytos--;
#ifndef NDEBUG
  if( mqlyyTraceFILE ){
    fprintf(mqlyyTraceFILE,"%sPopping %s\n",
      mqlyyTracePrompt,
      mqlyyTokenName[mqlyytos->major]);
  }
#endif
  mqlyy_destructor(pParser, mqlyytos->major, &mqlyytos->minor);
}

/* 
** Deallocate and destroy a parser.  Destructors are called for
** all stack elements before shutting the parser down.
**
** If the MQLYYPARSEFREENEVERNULL macro exists (for example because it
** is defined in a %include section of the input grammar) then it is
** assumed that the input pointer is never NULL.
*/
void MQLParserFree(
  void *p,                    /* The parser to be deleted */
  void (*freeProc)(void*)     /* Function used to reclaim memory */
){
  mqlyyParser *pParser = (mqlyyParser*)p;
#ifndef MQLYYPARSEFREENEVERNULL
  if( pParser==0 ) return;
#endif
  while( pParser->mqlyytos>pParser->mqlyystack ) mqlyy_pop_parser_stack(pParser);
#if MQLYYSTACKDEPTH<=0
  if( pParser->mqlyystack!=&pParser->mqlyystk0 ) free(pParser->mqlyystack);
#endif
  (*freeProc)((void*)pParser);
}

/*
** Return the peak depth of the stack for a parser.
*/
#ifdef MQLYYTRACKMAXSTACKDEPTH
int MQLParserStackPeak(void *p){
  mqlyyParser *pParser = (mqlyyParser*)p;
  return pParser->mqlyyhwm;
}
#endif

/*
** Find the appropriate action for a parser given the terminal
** look-ahead token iLookAhead.
*/
static unsigned int mqlyy_find_shift_action(
  mqlyyParser *pParser,        /* The parser */
  MQLYYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
  int stateno = pParser->mqlyytos->stateno;
 
  if( stateno>=MQLYY_MIN_REDUCE ) return stateno;
  assert( stateno <= MQLYY_SHIFT_COUNT );
  do{
    i = mqlyy_shift_ofst[stateno];
    assert( iLookAhead!=MQLYYNOCODE );
    i += iLookAhead;
    if( i<0 || i>=MQLYY_ACTTAB_COUNT || mqlyy_lookahead[i]!=iLookAhead ){
#ifdef MQLYYFALLBACK
      MQLYYCODETYPE iFallback;            /* Fallback token */
      if( iLookAhead<sizeof(mqlyyFallback)/sizeof(mqlyyFallback[0])
             && (iFallback = mqlyyFallback[iLookAhead])!=0 ){
#ifndef NDEBUG
        if( mqlyyTraceFILE ){
          fprintf(mqlyyTraceFILE, "%sFALLBACK %s => %s\n",
             mqlyyTracePrompt, mqlyyTokenName[iLookAhead], mqlyyTokenName[iFallback]);
        }
#endif
        assert( mqlyyFallback[iFallback]==0 ); /* Fallback loop must terminate */
        iLookAhead = iFallback;
        continue;
      }
#endif
#ifdef MQLYYWILDCARD
      {
        int j = i - iLookAhead + MQLYYWILDCARD;
        if( 
#if MQLYY_SHIFT_MIN+MQLYYWILDCARD<0
          j>=0 &&
#endif
#if MQLYY_SHIFT_MAX+MQLYYWILDCARD>=MQLYY_ACTTAB_COUNT
          j<MQLYY_ACTTAB_COUNT &&
#endif
          mqlyy_lookahead[j]==MQLYYWILDCARD && iLookAhead>0
        ){
#ifndef NDEBUG
          if( mqlyyTraceFILE ){
            fprintf(mqlyyTraceFILE, "%sWILDCARD %s => %s\n",
               mqlyyTracePrompt, mqlyyTokenName[iLookAhead],
               mqlyyTokenName[MQLYYWILDCARD]);
          }
#endif /* NDEBUG */
          return mqlyy_action[j];
        }
      }
#endif /* MQLYYWILDCARD */
      return mqlyy_default[stateno];
    }else{
      return mqlyy_action[i];
    }
  }while(1);
}

/*
** Find the appropriate action for a parser given the non-terminal
** look-ahead token iLookAhead.
*/
static int mqlyy_find_reduce_action(
  int stateno,              /* Current state number */
  MQLYYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
#ifdef MQLYYERRORSYMBOL
  if( stateno>MQLYY_REDUCE_COUNT ){
    return mqlyy_default[stateno];
  }
#else
  assert( stateno<=MQLYY_REDUCE_COUNT );
#endif
  i = mqlyy_reduce_ofst[stateno];
  assert( i!=MQLYY_REDUCE_USE_DFLT );
  assert( iLookAhead!=MQLYYNOCODE );
  i += iLookAhead;
#ifdef MQLYYERRORSYMBOL
  if( i<0 || i>=MQLYY_ACTTAB_COUNT || mqlyy_lookahead[i]!=iLookAhead ){
    return mqlyy_default[stateno];
  }
#else
  assert( i>=0 && i<MQLYY_ACTTAB_COUNT );
  assert( mqlyy_lookahead[i]==iLookAhead );
#endif
  return mqlyy_action[i];
}

/*
** The following routine is called if the stack overflows.
*/
static void mqlyyStackOverflow(mqlyyParser *mqlyypParser){
   MQLParserARG_FETCH;
#ifndef NDEBUG
   if( mqlyyTraceFILE ){
     fprintf(mqlyyTraceFILE,"%sStack Overflow!\n",mqlyyTracePrompt);
   }
#endif
   while( mqlyypParser->mqlyytos>mqlyypParser->mqlyystack ) mqlyy_pop_parser_stack(mqlyypParser);
   /* Here code is inserted which will execute if the parser
   ** stack every overflows */
/******** Begin %stack_overflow code ******************************************/
#line 134 "./mql.yxx"

  std::string errMsg = "stack overflow. Bailing out...\n";
  pEE->pError->appendError(errMsg);
   
#line 2208 "./mql.c"
/******** End %stack_overflow code ********************************************/
   MQLParserARG_STORE; /* Suppress warning about unused %extra_argument var */
}

/*
** Print tracing information for a SHIFT action
*/
#ifndef NDEBUG
static void mqlyyTraceShift(mqlyyParser *mqlyypParser, int mqlyyNewState){
  if( mqlyyTraceFILE ){
    if( mqlyyNewState<MQLYYNSTATE ){
      fprintf(mqlyyTraceFILE,"%sShift '%s', go to state %d\n",
         mqlyyTracePrompt,mqlyyTokenName[mqlyypParser->mqlyytos->major],
         mqlyyNewState);
    }else{
      fprintf(mqlyyTraceFILE,"%sShift '%s'\n",
         mqlyyTracePrompt,mqlyyTokenName[mqlyypParser->mqlyytos->major]);
    }
  }
}
#else
# define mqlyyTraceShift(X,Y)
#endif

/*
** Perform a shift action.
*/
static void mqlyy_shift(
  mqlyyParser *mqlyypParser,          /* The parser to be shifted */
  int mqlyyNewState,               /* The new state to shift in */
  int mqlyyMajor,                  /* The major token to shift in */
  MQLParserTOKENTYPE mqlyyMinor        /* The minor token to shift in */
){
  mqlyyStackEntry *mqlyytos;
  mqlyypParser->mqlyytos++;
#ifdef MQLYYTRACKMAXSTACKDEPTH
  if( (int)(mqlyypParser->mqlyytos - mqlyypParser->mqlyystack)>mqlyypParser->mqlyyhwm ){
    mqlyypParser->mqlyyhwm++;
    assert( mqlyypParser->mqlyyhwm == (int)(mqlyypParser->mqlyytos - mqlyypParser->mqlyystack) );
  }
#endif
#if MQLYYSTACKDEPTH>0 
  if( mqlyypParser->mqlyytos>=&mqlyypParser->mqlyystack[MQLYYSTACKDEPTH] ){
    mqlyypParser->mqlyytos--;
    mqlyyStackOverflow(mqlyypParser);
    return;
  }
#else
  if( mqlyypParser->mqlyytos>=&mqlyypParser->mqlyystack[mqlyypParser->mqlyystksz] ){
    if( mqlyyGrowStack(mqlyypParser) ){
      mqlyypParser->mqlyytos--;
      mqlyyStackOverflow(mqlyypParser);
      return;
    }
  }
#endif
  if( mqlyyNewState > MQLYY_MAX_SHIFT ){
    mqlyyNewState += MQLYY_MIN_REDUCE - MQLYY_MIN_SHIFTREDUCE;
  }
  mqlyytos = mqlyypParser->mqlyytos;
  mqlyytos->stateno = (MQLYYACTIONTYPE)mqlyyNewState;
  mqlyytos->major = (MQLYYCODETYPE)mqlyyMajor;
  mqlyytos->minor.mqlyy0 = mqlyyMinor;
  mqlyyTraceShift(mqlyypParser, mqlyyNewState);
}

/* The following table contains information about every rule that
** is used during the reduce.
*/
static const struct {
  MQLYYCODETYPE lhs;         /* Symbol on the left-hand side of the rule */
  unsigned char nrhs;     /* Number of right-hand side symbols in the rule */
} mqlyyRuleInfo[] = {
  { 132, 2 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 133, 1 },
  { 134, 5 },
  { 182, 1 },
  { 182, 1 },
  { 135, 4 },
  { 136, 4 },
  { 183, 3 },
  { 183, 0 },
  { 184, 3 },
  { 184, 0 },
  { 185, 1 },
  { 185, 0 },
  { 137, 3 },
  { 138, 3 },
  { 187, 0 },
  { 187, 1 },
  { 171, 3 },
  { 172, 3 },
  { 188, 6 },
  { 189, 1 },
  { 189, 1 },
  { 173, 2 },
  { 174, 2 },
  { 175, 2 },
  { 139, 10 },
  { 193, 0 },
  { 193, 4 },
  { 193, 4 },
  { 193, 4 },
  { 194, 0 },
  { 194, 4 },
  { 194, 6 },
  { 194, 3 },
  { 191, 1 },
  { 191, 0 },
  { 195, 1 },
  { 196, 1 },
  { 196, 0 },
  { 197, 1 },
  { 197, 2 },
  { 198, 6 },
  { 198, 6 },
  { 200, 2 },
  { 200, 4 },
  { 200, 4 },
  { 200, 2 },
  { 200, 2 },
  { 200, 3 },
  { 200, 5 },
  { 200, 5 },
  { 200, 5 },
  { 203, 1 },
  { 203, 1 },
  { 203, 1 },
  { 204, 2 },
  { 204, 2 },
  { 204, 0 },
  { 192, 3 },
  { 192, 0 },
  { 206, 2 },
  { 206, 0 },
  { 205, 0 },
  { 205, 3 },
  { 201, 2 },
  { 201, 0 },
  { 202, 1 },
  { 202, 0 },
  { 140, 7 },
  { 208, 1 },
  { 208, 2 },
  { 209, 2 },
  { 209, 3 },
  { 210, 1 },
  { 210, 0 },
  { 141, 6 },
  { 142, 2 },
  { 143, 2 },
  { 144, 9 },
  { 145, 4 },
  { 214, 1 },
  { 214, 3 },
  { 216, 1 },
  { 216, 1 },
  { 216, 1 },
  { 176, 3 },
  { 177, 4 },
  { 177, 4 },
  { 217, 1 },
  { 217, 1 },
  { 178, 6 },
  { 179, 6 },
  { 179, 6 },
  { 179, 6 },
  { 179, 6 },
  { 180, 4 },
  { 219, 1 },
  { 218, 1 },
  { 218, 3 },
  { 146, 7 },
  { 220, 1 },
  { 220, 1 },
  { 221, 1 },
  { 222, 1 },
  { 222, 3 },
  { 223, 3 },
  { 224, 1 },
  { 224, 0 },
  { 225, 1 },
  { 226, 1 },
  { 226, 0 },
  { 147, 7 },
  { 228, 1 },
  { 228, 3 },
  { 229, 4 },
  { 229, 4 },
  { 229, 2 },
  { 227, 2 },
  { 230, 1 },
  { 230, 2 },
  { 230, 1 },
  { 231, 1 },
  { 148, 3 },
  { 149, 6 },
  { 232, 1 },
  { 233, 3 },
  { 150, 5 },
  { 150, 6 },
  { 234, 3 },
  { 239, 1 },
  { 239, 1 },
  { 240, 1 },
  { 240, 0 },
  { 235, 2 },
  { 235, 0 },
  { 241, 1 },
  { 241, 1 },
  { 215, 3 },
  { 242, 1 },
  { 242, 3 },
  { 243, 1 },
  { 243, 3 },
  { 243, 2 },
  { 236, 0 },
  { 236, 5 },
  { 236, 5 },
  { 236, 9 },
  { 237, 0 },
  { 237, 3 },
  { 237, 3 },
  { 237, 5 },
  { 245, 0 },
  { 245, 2 },
  { 245, 6 },
  { 245, 4 },
  { 244, 1 },
  { 244, 3 },
  { 238, 2 },
  { 151, 7 },
  { 247, 3 },
  { 152, 9 },
  { 248, 1 },
  { 190, 1 },
  { 190, 1 },
  { 153, 12 },
  { 249, 1 },
  { 249, 3 },
  { 252, 4 },
  { 252, 4 },
  { 252, 4 },
  { 252, 4 },
  { 252, 4 },
  { 253, 3 },
  { 253, 5 },
  { 253, 5 },
  { 154, 10 },
  { 154, 12 },
  { 154, 12 },
  { 250, 4 },
  { 250, 4 },
  { 250, 0 },
  { 156, 3 },
  { 157, 7 },
  { 155, 8 },
  { 259, 2 },
  { 259, 1 },
  { 259, 0 },
  { 158, 2 },
  { 159, 6 },
  { 260, 1 },
  { 260, 1 },
  { 260, 0 },
  { 160, 6 },
  { 161, 2 },
  { 162, 2 },
  { 163, 6 },
  { 211, 3 },
  { 261, 4 },
  { 261, 0 },
  { 263, 1 },
  { 263, 1 },
  { 262, 4 },
  { 264, 1 },
  { 264, 0 },
  { 265, 1 },
  { 265, 2 },
  { 266, 4 },
  { 199, 1 },
  { 199, 1 },
  { 207, 1 },
  { 207, 1 },
  { 207, 1 },
  { 207, 1 },
  { 207, 2 },
  { 207, 3 },
  { 207, 3 },
  { 257, 1 },
  { 257, 3 },
  { 256, 1 },
  { 256, 3 },
  { 164, 6 },
  { 213, 3 },
  { 267, 1 },
  { 267, 1 },
  { 268, 1 },
  { 268, 3 },
  { 269, 1 },
  { 181, 9 },
  { 270, 1 },
  { 270, 2 },
  { 271, 8 },
  { 165, 5 },
  { 212, 1 },
  { 212, 1 },
  { 272, 4 },
  { 166, 5 },
  { 167, 5 },
  { 273, 3 },
  { 274, 1 },
  { 168, 5 },
  { 169, 10 },
  { 275, 1 },
  { 275, 1 },
  { 258, 1 },
  { 258, 3 },
  { 170, 1 },
  { 246, 1 },
  { 276, 1 },
  { 277, 2 },
  { 277, 2 },
  { 280, 1 },
  { 280, 3 },
  { 282, 1 },
  { 282, 3 },
  { 284, 1 },
  { 284, 2 },
  { 284, 3 },
  { 278, 1 },
  { 278, 3 },
  { 285, 1 },
  { 285, 1 },
  { 286, 0 },
  { 286, 2 },
  { 288, 0 },
  { 288, 1 },
  { 287, 1 },
  { 289, 0 },
  { 289, 1 },
  { 289, 1 },
  { 289, 1 },
  { 290, 0 },
  { 290, 1 },
  { 290, 1 },
  { 290, 3 },
  { 291, 6 },
  { 291, 4 },
  { 291, 0 },
  { 292, 1 },
  { 292, 1 },
  { 292, 1 },
  { 293, 1 },
  { 293, 1 },
  { 251, 0 },
  { 251, 1 },
  { 294, 1 },
  { 294, 3 },
  { 295, 1 },
  { 295, 3 },
  { 296, 2 },
  { 296, 3 },
  { 296, 1 },
  { 297, 3 },
  { 297, 5 },
  { 297, 5 },
  { 297, 3 },
  { 254, 1 },
  { 254, 1 },
  { 254, 1 },
  { 254, 1 },
  { 254, 1 },
  { 254, 1 },
  { 254, 1 },
  { 254, 1 },
  { 254, 1 },
  { 255, 1 },
  { 255, 1 },
  { 255, 1 },
  { 255, 1 },
  { 299, 1 },
  { 298, 3 },
  { 300, 0 },
  { 300, 1 },
  { 281, 1 },
  { 281, 1 },
  { 281, 1 },
  { 281, 1 },
  { 281, 1 },
  { 279, 4 },
  { 306, 1 },
  { 306, 2 },
  { 283, 0 },
  { 283, 1 },
  { 303, 6 },
  { 307, 0 },
  { 307, 1 },
  { 307, 1 },
  { 307, 1 },
  { 304, 6 },
  { 308, 2 },
  { 308, 0 },
  { 301, 11 },
  { 305, 12 },
  { 302, 2 },
  { 302, 5 },
  { 309, 0 },
  { 309, 2 },
  { 309, 2 },
  { 310, 1 },
};

static void mqlyy_accept(mqlyyParser*);  /* Forward Declaration */

/*
** Perform a reduce action and the shift that must immediately
** follow the reduce.
*/
static void mqlyy_reduce(
  mqlyyParser *mqlyypParser,         /* The parser */
  unsigned int mqlyyruleno        /* Number of the rule by which to reduce */
){
  int mqlyygoto;                     /* The next state */
  int mqlyyact;                      /* The next action */
  mqlyyStackEntry *mqlyymsp;            /* The top of the parser's stack */
  int mqlyysize;                     /* Amount to pop the stack */
  MQLParserARG_FETCH;
  mqlyymsp = mqlyypParser->mqlyytos;
#ifndef NDEBUG
  if( mqlyyTraceFILE && mqlyyruleno<(int)(sizeof(mqlyyRuleName)/sizeof(mqlyyRuleName[0])) ){
    mqlyysize = mqlyyRuleInfo[mqlyyruleno].nrhs;
    fprintf(mqlyyTraceFILE, "%sReduce [%s], go to state %d.\n", mqlyyTracePrompt,
      mqlyyRuleName[mqlyyruleno], mqlyymsp[-mqlyysize].stateno);
  }
#endif /* NDEBUG */

  /* Check that the stack is large enough to grow by a single entry
  ** if the RHS of the rule is empty.  This ensures that there is room
  ** enough on the stack to push the LHS value */
  if( mqlyyRuleInfo[mqlyyruleno].nrhs==0 ){
#ifdef MQLYYTRACKMAXSTACKDEPTH
    if( (int)(mqlyypParser->mqlyytos - mqlyypParser->mqlyystack)>mqlyypParser->mqlyyhwm ){
      mqlyypParser->mqlyyhwm++;
      assert( mqlyypParser->mqlyyhwm == (int)(mqlyypParser->mqlyytos - mqlyypParser->mqlyystack));
    }
#endif
#if MQLYYSTACKDEPTH>0 
    if( mqlyypParser->mqlyytos>=&mqlyypParser->mqlyystack[MQLYYSTACKDEPTH-1] ){
      mqlyyStackOverflow(mqlyypParser);
      return;
    }
#else
    if( mqlyypParser->mqlyytos>=&mqlyypParser->mqlyystack[mqlyypParser->mqlyystksz-1] ){
      if( mqlyyGrowStack(mqlyypParser) ){
        mqlyyStackOverflow(mqlyypParser);
        return;
      }
      mqlyymsp = mqlyypParser->mqlyytos;
    }
#endif
  }

  switch( mqlyyruleno ){
  /* Beginning here are the reduction cases.  A typical example
  ** follows:
  **   case 0:
  **  #line <lineno> <grammarfile>
  **     { ... }           // User supplied code
  **  #line <lineno> <thisfile>
  **     break;
  */
/********** Begin reduce actions **********************************************/
        MQLYYMINORTYPE mqlyylhsminor;
      case 0: /* statement ::= statement_by_itself KEY_GO */
#line 153 "./mql.yxx"
{ pEE->pStatement = mqlyymsp[-1].minor.mqlyy50; }
#line 2733 "./mql.c"
  mqlyy_destructor(mqlyypParser,1,&mqlyymsp[0].minor);
        break;
      case 1: /* statement_by_itself ::= create_database_statement */
      case 2: /* statement_by_itself ::= initialize_database_statement */ mqlyytestcase(mqlyyruleno==2);
      case 3: /* statement_by_itself ::= use_statement */ mqlyytestcase(mqlyyruleno==3);
      case 4: /* statement_by_itself ::= drop_database_statement */ mqlyytestcase(mqlyyruleno==4);
      case 5: /* statement_by_itself ::= vacuum_database_statement */ mqlyytestcase(mqlyyruleno==5);
      case 6: /* statement_by_itself ::= create_object_type_statement */ mqlyytestcase(mqlyyruleno==6);
      case 7: /* statement_by_itself ::= update_object_type_statement */ mqlyytestcase(mqlyyruleno==7);
      case 8: /* statement_by_itself ::= drop_object_type_statement */ mqlyytestcase(mqlyyruleno==8);
      case 9: /* statement_by_itself ::= insert_monads_statement */ mqlyytestcase(mqlyyruleno==9);
      case 10: /* statement_by_itself ::= delete_monads_statement */ mqlyytestcase(mqlyyruleno==10);
      case 11: /* statement_by_itself ::= get_monads_statement */ mqlyytestcase(mqlyyruleno==11);
      case 12: /* statement_by_itself ::= monad_set_calculation_statement */ mqlyytestcase(mqlyyruleno==12);
      case 13: /* statement_by_itself ::= create_enumeration_statement */ mqlyytestcase(mqlyyruleno==13);
      case 14: /* statement_by_itself ::= update_enumeration_statement */ mqlyytestcase(mqlyyruleno==14);
      case 15: /* statement_by_itself ::= drop_enumeration_statement */ mqlyytestcase(mqlyyruleno==15);
      case 16: /* statement_by_itself ::= create_segment_statement */ mqlyytestcase(mqlyyruleno==16);
      case 17: /* statement_by_itself ::= select_statement */ mqlyytestcase(mqlyyruleno==17);
      case 18: /* statement_by_itself ::= select_objects_at_statement */ mqlyytestcase(mqlyyruleno==18);
      case 19: /* statement_by_itself ::= select_objects_having_monads_in_statement */ mqlyytestcase(mqlyyruleno==19);
      case 20: /* statement_by_itself ::= get_aggregate_features_statement */ mqlyytestcase(mqlyyruleno==20);
      case 21: /* statement_by_itself ::= get_objects_having_monads_in_statement */ mqlyytestcase(mqlyyruleno==21);
      case 22: /* statement_by_itself ::= get_set_from_feature_statement */ mqlyytestcase(mqlyyruleno==22);
      case 23: /* statement_by_itself ::= select_object_types_statement */ mqlyytestcase(mqlyyruleno==23);
      case 24: /* statement_by_itself ::= select_features_statement */ mqlyytestcase(mqlyyruleno==24);
      case 25: /* statement_by_itself ::= select_enumerations_statement */ mqlyytestcase(mqlyyruleno==25);
      case 26: /* statement_by_itself ::= select_enumeration_constants_statement */ mqlyytestcase(mqlyyruleno==26);
      case 27: /* statement_by_itself ::= select_object_types_which_use_enum_statement */ mqlyytestcase(mqlyyruleno==27);
      case 28: /* statement_by_itself ::= select_min_m_statement */ mqlyytestcase(mqlyyruleno==28);
      case 29: /* statement_by_itself ::= select_max_m_statement */ mqlyytestcase(mqlyyruleno==29);
      case 30: /* statement_by_itself ::= create_object_from_monads_statement */ mqlyytestcase(mqlyyruleno==30);
      case 31: /* statement_by_itself ::= create_object_from_id_ds_statement */ mqlyytestcase(mqlyyruleno==31);
      case 32: /* statement_by_itself ::= update_objects_by_monads_statement */ mqlyytestcase(mqlyyruleno==32);
      case 33: /* statement_by_itself ::= update_objects_by_id_ds_statement */ mqlyytestcase(mqlyyruleno==33);
      case 34: /* statement_by_itself ::= delete_objects_by_monads_statement */ mqlyytestcase(mqlyyruleno==34);
      case 35: /* statement_by_itself ::= delete_objects_by_id_ds_statement */ mqlyytestcase(mqlyyruleno==35);
      case 36: /* statement_by_itself ::= get_features_statement */ mqlyytestcase(mqlyyruleno==36);
      case 37: /* statement_by_itself ::= quit_statement */ mqlyytestcase(mqlyyruleno==37);
      case 38: /* statement_by_itself ::= create_indexes_statement */ mqlyytestcase(mqlyyruleno==38);
      case 39: /* statement_by_itself ::= drop_indexes_statement */ mqlyytestcase(mqlyyruleno==39);
      case 40: /* statement_by_itself ::= begin_transaction_statement */ mqlyytestcase(mqlyyruleno==40);
      case 41: /* statement_by_itself ::= commit_transaction_statement */ mqlyytestcase(mqlyyruleno==41);
      case 42: /* statement_by_itself ::= abort_transaction_statement */ mqlyytestcase(mqlyyruleno==42);
      case 43: /* statement_by_itself ::= select_monad_sets_statement */ mqlyytestcase(mqlyyruleno==43);
      case 44: /* statement_by_itself ::= get_monad_sets_statement */ mqlyytestcase(mqlyyruleno==44);
      case 45: /* statement_by_itself ::= create_monad_set_statement */ mqlyytestcase(mqlyyruleno==45);
      case 46: /* statement_by_itself ::= update_monad_set_statement */ mqlyytestcase(mqlyyruleno==46);
      case 47: /* statement_by_itself ::= drop_monad_set_statement */ mqlyytestcase(mqlyyruleno==47);
      case 48: /* statement_by_itself ::= create_objects_statement */ mqlyytestcase(mqlyyruleno==48);
#line 157 "./mql.yxx"
{ mqlyylhsminor.mqlyy50 = mqlyymsp[0].minor.mqlyy50; }
#line 2786 "./mql.c"
  mqlyymsp[0].minor.mqlyy50 = mqlyylhsminor.mqlyy50;
        break;
      case 49: /* create_database_statement ::= KEY_CREATE KEY_DATABASE database_name opt_WITH_KEY opt_USING_ENCODING */
{  mqlyy_destructor(mqlyypParser,2,&mqlyymsp[-4].minor);
#line 213 "./mql.yxx"
{ mqlyymsp[-4].minor.mqlyy50 = new CreateDatabaseStatement((MQLExecEnv*) pEE, mqlyymsp[-2].minor.mqlyy0->extractString(), mqlyymsp[-1].minor.mqlyy0->extractString(), mqlyymsp[0].minor.mqlyy0->extractString()); deleteToken(mqlyymsp[-2].minor.mqlyy0); deleteToken(mqlyymsp[-1].minor.mqlyy0); deleteToken(mqlyymsp[0].minor.mqlyy0); }
#line 2793 "./mql.c"
  mqlyy_destructor(mqlyypParser,3,&mqlyymsp[-3].minor);
}
        break;
      case 50: /* database_name ::= IDENTIFIER */
      case 51: /* database_name ::= STRING */ mqlyytestcase(mqlyyruleno==51);
      case 143: /* monad_set_name ::= IDENTIFIER */ mqlyytestcase(mqlyyruleno==143);
      case 149: /* enumeration_name ::= IDENTIFIER */ mqlyytestcase(mqlyyruleno==149);
      case 155: /* ec_name ::= IDENTIFIER */ mqlyytestcase(mqlyyruleno==155);
      case 171: /* segment_name ::= IDENTIFIER */ mqlyytestcase(mqlyyruleno==171);
      case 208: /* object_type_to_find ::= choice_object_type_or_all */ mqlyytestcase(mqlyyruleno==208);
      case 209: /* choice_object_type_or_all ::= object_type_name */ mqlyytestcase(mqlyyruleno==209);
      case 254: /* feature_name ::= IDENTIFIER */ mqlyytestcase(mqlyyruleno==254);
      case 285: /* object_type_name_to_delete ::= choice_object_type_or_all */ mqlyytestcase(mqlyyruleno==285);
      case 311: /* mark_declaration ::= MARK */ mqlyytestcase(mqlyyruleno==311);
      case 312: /* object_reference ::= IDENTIFIER */ mqlyytestcase(mqlyyruleno==312);
      case 355: /* enum_const ::= IDENTIFIER */ mqlyytestcase(mqlyyruleno==355);
#line 218 "./mql.yxx"
{ mqlyylhsminor.mqlyy0 = mqlyymsp[0].minor.mqlyy0; }
#line 2812 "./mql.c"
  mqlyymsp[0].minor.mqlyy0 = mqlyylhsminor.mqlyy0;
        break;
      case 52: /* initialize_database_statement ::= KEY_INITIALIZE KEY_DATABASE database_name opt_WITH_KEY */
{  mqlyy_destructor(mqlyypParser,6,&mqlyymsp[-3].minor);
#line 225 "./mql.yxx"
{ mqlyymsp[-3].minor.mqlyy50 = new InitializeDatabaseStatement((MQLExecEnv*) pEE, mqlyymsp[-1].minor.mqlyy0->extractString(), mqlyymsp[0].minor.mqlyy0->extractString()); deleteToken(mqlyymsp[-1].minor.mqlyy0); deleteToken(mqlyymsp[0].minor.mqlyy0); }
#line 2819 "./mql.c"
  mqlyy_destructor(mqlyypParser,3,&mqlyymsp[-2].minor);
}
        break;
      case 53: /* use_statement ::= KEY_USE opt_DATABASE database_name opt_WITH_KEY */
{  mqlyy_destructor(mqlyypParser,7,&mqlyymsp[-3].minor);
#line 230 "./mql.yxx"
{ mqlyymsp[-3].minor.mqlyy50 = new UseStatement((MQLExecEnv*) pEE, mqlyymsp[-1].minor.mqlyy0->extractString(), mqlyymsp[0].minor.mqlyy0->extractString()); deleteToken(mqlyymsp[-1].minor.mqlyy0); deleteToken(mqlyymsp[0].minor.mqlyy0); }
#line 2827 "./mql.c"
  mqlyy_destructor(mqlyypParser,185,&mqlyymsp[-2].minor);
}
        break;
      case 54: /* opt_WITH_KEY ::= KEY_WITH KEY_KEY STRING */
{  mqlyy_destructor(mqlyypParser,8,&mqlyymsp[-2].minor);
#line 234 "./mql.yxx"
{ mqlyymsp[-2].minor.mqlyy0 = mqlyymsp[0].minor.mqlyy0; }
#line 2835 "./mql.c"
  mqlyy_destructor(mqlyypParser,9,&mqlyymsp[-1].minor);
}
        break;
      case 55: /* opt_WITH_KEY ::= */
      case 57: /* opt_USING_ENCODING ::= */ mqlyytestcase(mqlyyruleno==57);
#line 235 "./mql.yxx"
{ mqlyymsp[1].minor.mqlyy0 = newToken(); mqlyymsp[1].minor.mqlyy0->setString(new std::string("")); }
#line 2843 "./mql.c"
        break;
      case 56: /* opt_USING_ENCODING ::= KEY_USING KEY_ENCODING STRING */
{  mqlyy_destructor(mqlyypParser,10,&mqlyymsp[-2].minor);
#line 240 "./mql.yxx"
{ mqlyymsp[-2].minor.mqlyy0 = mqlyymsp[0].minor.mqlyy0; }
#line 2849 "./mql.c"
  mqlyy_destructor(mqlyypParser,11,&mqlyymsp[-1].minor);
}
        break;
      case 58: /* opt_DATABASE ::= KEY_DATABASE */
{  mqlyy_destructor(mqlyypParser,3,&mqlyymsp[0].minor);
#line 246 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy466=0;}
#line 2857 "./mql.c"
}
        break;
      case 59: /* opt_DATABASE ::= */
#line 247 "./mql.yxx"
{ mqlyymsp[1].minor.mqlyy466=0;}
#line 2863 "./mql.c"
        break;
      case 60: /* drop_database_statement ::= KEY_DROP KEY_DATABASE database_name */
{  mqlyy_destructor(mqlyypParser,12,&mqlyymsp[-2].minor);
#line 253 "./mql.yxx"
{ mqlyymsp[-2].minor.mqlyy50 = new DropDatabaseStatement((MQLExecEnv*) pEE, mqlyymsp[0].minor.mqlyy0->extractString()); deleteToken(mqlyymsp[0].minor.mqlyy0); }
#line 2869 "./mql.c"
  mqlyy_destructor(mqlyypParser,3,&mqlyymsp[-1].minor);
}
        break;
      case 61: /* vacuum_database_statement ::= KEY_VACUUM opt_DATABASE opt_ANALYZE */
{  mqlyy_destructor(mqlyypParser,13,&mqlyymsp[-2].minor);
#line 259 "./mql.yxx"
{ mqlyymsp[-2].minor.mqlyy50 = new VacuumDatabaseStatement((MQLExecEnv*) pEE, mqlyymsp[0].minor.mqlyy263); }
#line 2877 "./mql.c"
  mqlyy_destructor(mqlyypParser,185,&mqlyymsp[-1].minor);
}
        break;
      case 62: /* opt_ANALYZE ::= */
      case 104: /* opt_with_index ::= */ mqlyytestcase(mqlyyruleno==104);
      case 106: /* opt_if_not_exists ::= */ mqlyytestcase(mqlyyruleno==106);
      case 114: /* opt_computed ::= */ mqlyytestcase(mqlyyruleno==114);
      case 154: /* opt_DEFAULT ::= */ mqlyytestcase(mqlyyruleno==154);
#line 264 "./mql.yxx"
{ mqlyymsp[1].minor.mqlyy263 = false; }
#line 2888 "./mql.c"
        break;
      case 63: /* opt_ANALYZE ::= KEY_ANALYZE */
{  mqlyy_destructor(mqlyypParser,14,&mqlyymsp[0].minor);
#line 265 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy263 = true; }
#line 2894 "./mql.c"
}
        break;
      case 64: /* create_indexes_statement ::= KEY_CREATE KEY_INDEXES on_object_type */
{  mqlyy_destructor(mqlyypParser,2,&mqlyymsp[-2].minor);
#line 271 "./mql.yxx"
{ mqlyymsp[-2].minor.mqlyy50 = new CreateIndexesStatement((MQLExecEnv*) pEE, *(mqlyymsp[0].minor.mqlyy0->pString)); deleteToken(mqlyymsp[0].minor.mqlyy0); }
#line 2901 "./mql.c"
  mqlyy_destructor(mqlyypParser,15,&mqlyymsp[-1].minor);
}
        break;
      case 65: /* drop_indexes_statement ::= KEY_DROP KEY_INDEXES on_object_type */
{  mqlyy_destructor(mqlyypParser,12,&mqlyymsp[-2].minor);
#line 277 "./mql.yxx"
{ mqlyymsp[-2].minor.mqlyy50 = new DropIndexesStatement((MQLExecEnv*) pEE, *(mqlyymsp[0].minor.mqlyy0->pString)); deleteToken(mqlyymsp[0].minor.mqlyy0); }
#line 2909 "./mql.c"
  mqlyy_destructor(mqlyypParser,15,&mqlyymsp[-1].minor);
}
        break;
      case 66: /* on_object_type ::= KEY_ON KEY_OBJECT choice_type_types KEY_OPEN_SQUARE_BRACKET choice_object_type_or_all KEY_CLOSE_SQUARE_BRACKET */
{  mqlyy_destructor(mqlyypParser,16,&mqlyymsp[-5].minor);
#line 286 "./mql.yxx"
{ mqlyymsp[-5].minor.mqlyy0 = mqlyymsp[-1].minor.mqlyy0; }
#line 2917 "./mql.c"
  mqlyy_destructor(mqlyypParser,17,&mqlyymsp[-4].minor);
  mqlyy_destructor(mqlyypParser,189,&mqlyymsp[-3].minor);
  mqlyy_destructor(mqlyypParser,18,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,19,&mqlyymsp[0].minor);
}
        break;
      case 67: /* choice_type_types ::= KEY_TYPE */
{  mqlyy_destructor(mqlyypParser,20,&mqlyymsp[0].minor);
#line 292 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy466 = 0; }
#line 2928 "./mql.c"
}
        break;
      case 68: /* choice_type_types ::= KEY_TYPES */
{  mqlyy_destructor(mqlyypParser,21,&mqlyymsp[0].minor);
#line 293 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy466 = 0; }
#line 2935 "./mql.c"
}
        break;
      case 69: /* begin_transaction_statement ::= KEY_BEGIN KEY_TRANSACTION */
{  mqlyy_destructor(mqlyypParser,22,&mqlyymsp[-1].minor);
#line 299 "./mql.yxx"
{ mqlyymsp[-1].minor.mqlyy50 = new BeginTransactionStatement((MQLExecEnv*) pEE); }
#line 2942 "./mql.c"
  mqlyy_destructor(mqlyypParser,23,&mqlyymsp[0].minor);
}
        break;
      case 70: /* commit_transaction_statement ::= KEY_COMMIT KEY_TRANSACTION */
{  mqlyy_destructor(mqlyypParser,24,&mqlyymsp[-1].minor);
#line 305 "./mql.yxx"
{ mqlyymsp[-1].minor.mqlyy50 = new CommitTransactionStatement((MQLExecEnv*) pEE); }
#line 2950 "./mql.c"
  mqlyy_destructor(mqlyypParser,23,&mqlyymsp[0].minor);
}
        break;
      case 71: /* abort_transaction_statement ::= KEY_ABORT KEY_TRANSACTION */
{  mqlyy_destructor(mqlyypParser,25,&mqlyymsp[-1].minor);
#line 311 "./mql.yxx"
{ mqlyymsp[-1].minor.mqlyy50 = new AbortTransactionStatement((MQLExecEnv*) pEE); }
#line 2958 "./mql.c"
  mqlyy_destructor(mqlyypParser,23,&mqlyymsp[0].minor);
}
        break;
      case 72: /* create_object_type_statement ::= KEY_CREATE opt_OBJECT KEY_TYPE opt_if_not_exists opt_range_type opt_monad_uniqueness_type KEY_OPEN_SQUARE_BRACKET object_type_name opt_feature_declaration_list KEY_CLOSE_SQUARE_BRACKET */
{  mqlyy_destructor(mqlyypParser,2,&mqlyymsp[-9].minor);
#line 324 "./mql.yxx"
{ mqlyymsp[-9].minor.mqlyy50 = new CreateObjectTypeStatement((MQLExecEnv*) pEE, mqlyymsp[-2].minor.mqlyy0->extractString(), mqlyymsp[-5].minor.mqlyy342, mqlyymsp[-4].minor.mqlyy527, mqlyymsp[-1].minor.mqlyy289, mqlyymsp[-6].minor.mqlyy263); deleteToken(mqlyymsp[-2].minor.mqlyy0); }
#line 2966 "./mql.c"
  mqlyy_destructor(mqlyypParser,191,&mqlyymsp[-8].minor);
  mqlyy_destructor(mqlyypParser,20,&mqlyymsp[-7].minor);
  mqlyy_destructor(mqlyypParser,18,&mqlyymsp[-3].minor);
  mqlyy_destructor(mqlyypParser,19,&mqlyymsp[0].minor);
}
        break;
      case 73: /* opt_range_type ::= */
#line 330 "./mql.yxx"
{ mqlyymsp[1].minor.mqlyy342 = kORTMultipleRange; }
#line 2976 "./mql.c"
        break;
      case 74: /* opt_range_type ::= KEY_WITH KEY_MULTIPLE KEY_RANGE KEY_OBJECTS */
{  mqlyy_destructor(mqlyypParser,8,&mqlyymsp[-3].minor);
#line 332 "./mql.yxx"
{ mqlyymsp[-3].minor.mqlyy342 = kORTMultipleRange; }
#line 2982 "./mql.c"
  mqlyy_destructor(mqlyypParser,26,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,27,&mqlyymsp[-1].minor);
  mqlyy_destructor(mqlyypParser,28,&mqlyymsp[0].minor);
}
        break;
      case 75: /* opt_range_type ::= KEY_WITH KEY_SINGLE KEY_RANGE KEY_OBJECTS */
{  mqlyy_destructor(mqlyypParser,8,&mqlyymsp[-3].minor);
#line 334 "./mql.yxx"
{ mqlyymsp[-3].minor.mqlyy342 = kORTSingleRange; }
#line 2992 "./mql.c"
  mqlyy_destructor(mqlyypParser,29,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,27,&mqlyymsp[-1].minor);
  mqlyy_destructor(mqlyypParser,28,&mqlyymsp[0].minor);
}
        break;
      case 76: /* opt_range_type ::= KEY_WITH KEY_SINGLE KEY_MONAD KEY_OBJECTS */
{  mqlyy_destructor(mqlyypParser,8,&mqlyymsp[-3].minor);
#line 336 "./mql.yxx"
{ mqlyymsp[-3].minor.mqlyy342 = kORTSingleMonad; }
#line 3002 "./mql.c"
  mqlyy_destructor(mqlyypParser,29,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,30,&mqlyymsp[-1].minor);
  mqlyy_destructor(mqlyypParser,28,&mqlyymsp[0].minor);
}
        break;
      case 77: /* opt_monad_uniqueness_type ::= */
#line 341 "./mql.yxx"
{ mqlyymsp[1].minor.mqlyy527 = kMUTNonUniqueMonads; }
#line 3011 "./mql.c"
        break;
      case 78: /* opt_monad_uniqueness_type ::= KEY_HAVING KEY_UNIQUE KEY_FIRST KEY_MONADS */
{  mqlyy_destructor(mqlyypParser,31,&mqlyymsp[-3].minor);
#line 343 "./mql.yxx"
{ mqlyymsp[-3].minor.mqlyy527 = kMUTUniqueFirstMonads; }
#line 3017 "./mql.c"
  mqlyy_destructor(mqlyypParser,32,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,33,&mqlyymsp[-1].minor);
  mqlyy_destructor(mqlyypParser,34,&mqlyymsp[0].minor);
}
        break;
      case 79: /* opt_monad_uniqueness_type ::= KEY_HAVING KEY_UNIQUE KEY_FIRST KEY_AND KEY_LAST KEY_MONADS */
{  mqlyy_destructor(mqlyypParser,31,&mqlyymsp[-5].minor);
#line 345 "./mql.yxx"
{ mqlyymsp[-5].minor.mqlyy527 = kMUTUniqueFirstAndLastMonads; }
#line 3027 "./mql.c"
  mqlyy_destructor(mqlyypParser,32,&mqlyymsp[-4].minor);
  mqlyy_destructor(mqlyypParser,33,&mqlyymsp[-3].minor);
  mqlyy_destructor(mqlyypParser,35,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,36,&mqlyymsp[-1].minor);
  mqlyy_destructor(mqlyypParser,34,&mqlyymsp[0].minor);
}
        break;
      case 80: /* opt_monad_uniqueness_type ::= KEY_WITHOUT KEY_UNIQUE KEY_MONADS */
{  mqlyy_destructor(mqlyypParser,37,&mqlyymsp[-2].minor);
#line 347 "./mql.yxx"
{ mqlyymsp[-2].minor.mqlyy527 = kMUTNonUniqueMonads; }
#line 3039 "./mql.c"
  mqlyy_destructor(mqlyypParser,32,&mqlyymsp[-1].minor);
  mqlyy_destructor(mqlyypParser,34,&mqlyymsp[0].minor);
}
        break;
      case 81: /* opt_OBJECT ::= KEY_OBJECT */
{  mqlyy_destructor(mqlyypParser,17,&mqlyymsp[0].minor);
#line 352 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy466 = 0; }
#line 3048 "./mql.c"
}
        break;
      case 82: /* opt_OBJECT ::= */
      case 121: /* opt_ADD ::= */ mqlyytestcase(mqlyyruleno==121);
      case 179: /* opt_OBJECTS ::= */ mqlyytestcase(mqlyyruleno==179);
#line 353 "./mql.yxx"
{ mqlyymsp[1].minor.mqlyy466 = 0; }
#line 3056 "./mql.c"
        break;
      case 83: /* object_type_name ::= IDENTIFIER */
#line 358 "./mql.yxx"
{mqlyylhsminor.mqlyy0 = mqlyymsp[0].minor.mqlyy0; }
#line 3061 "./mql.c"
  mqlyymsp[0].minor.mqlyy0 = mqlyylhsminor.mqlyy0;
        break;
      case 84: /* opt_feature_declaration_list ::= feature_declaration_list */
      case 86: /* feature_declaration_list ::= feature_declaration */ mqlyytestcase(mqlyyruleno==86);
#line 362 "./mql.yxx"
{ mqlyylhsminor.mqlyy289 = mqlyymsp[0].minor.mqlyy289; }
#line 3068 "./mql.c"
  mqlyymsp[0].minor.mqlyy289 = mqlyylhsminor.mqlyy289;
        break;
      case 85: /* opt_feature_declaration_list ::= */
#line 363 "./mql.yxx"
{ mqlyymsp[1].minor.mqlyy289 = 0; }
#line 3074 "./mql.c"
        break;
      case 87: /* feature_declaration_list ::= feature_declaration_list feature_declaration */
#line 370 "./mql.yxx"
{ mqlyylhsminor.mqlyy289 = mqlyymsp[0].minor.mqlyy289; mqlyylhsminor.mqlyy289->setNext(mqlyymsp[-1].minor.mqlyy289); }
#line 3079 "./mql.c"
  mqlyymsp[-1].minor.mqlyy289 = mqlyylhsminor.mqlyy289;
        break;
      case 88: /* feature_declaration ::= feature_name KEY_COLON feature_type default_specification opt_computed KEY_SEMICOLON */
#line 376 "./mql.yxx"
{ mqlyylhsminor.mqlyy289 = new FeatureDeclaration(mqlyymsp[-5].minor.mqlyy0->extractString(), mqlyymsp[-3].minor.mqlyy264, mqlyymsp[-2].minor.mqlyy413, mqlyymsp[-1].minor.mqlyy263, 0); deleteToken(mqlyymsp[-5].minor.mqlyy0); }
#line 3085 "./mql.c"
  mqlyy_destructor(mqlyypParser,38,&mqlyymsp[-4].minor);
  mqlyy_destructor(mqlyypParser,39,&mqlyymsp[0].minor);
  mqlyymsp[-5].minor.mqlyy289 = mqlyylhsminor.mqlyy289;
        break;
      case 89: /* feature_declaration ::= feature_name KEY_COLON KEY_LIST KEY_OF list_feature_type KEY_SEMICOLON */
#line 379 "./mql.yxx"
{ mqlyylhsminor.mqlyy289 = new FeatureDeclaration(mqlyymsp[-5].minor.mqlyy0->extractString(), mqlyymsp[-1].minor.mqlyy264, new Expression(), false, 0); deleteToken(mqlyymsp[-5].minor.mqlyy0); }
#line 3093 "./mql.c"
  mqlyy_destructor(mqlyypParser,38,&mqlyymsp[-4].minor);
  mqlyy_destructor(mqlyypParser,40,&mqlyymsp[-3].minor);
  mqlyy_destructor(mqlyypParser,41,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,39,&mqlyymsp[0].minor);
  mqlyymsp[-5].minor.mqlyy289 = mqlyylhsminor.mqlyy289;
        break;
      case 90: /* feature_type ::= KEY_INTEGER opt_with_index */
{  mqlyy_destructor(mqlyypParser,42,&mqlyymsp[-1].minor);
#line 385 "./mql.yxx"
{ mqlyymsp[-1].minor.mqlyy264 = new MQLType(kInteger, mqlyymsp[0].minor.mqlyy263); }
#line 3104 "./mql.c"
}
        break;
      case 91: /* feature_type ::= KEY_STRING opt_string_length opt_from_set opt_with_index */
{  mqlyy_destructor(mqlyypParser,43,&mqlyymsp[-3].minor);
#line 387 "./mql.yxx"
{ mqlyymsp[-3].minor.mqlyy264 = new MQLType(kString, mqlyymsp[-1].minor.mqlyy263, mqlyymsp[0].minor.mqlyy263); }
#line 3111 "./mql.c"
  mqlyy_destructor(mqlyypParser,205,&mqlyymsp[-2].minor);
}
        break;
      case 92: /* feature_type ::= KEY_ASCII opt_string_length opt_from_set opt_with_index */
{  mqlyy_destructor(mqlyypParser,44,&mqlyymsp[-3].minor);
#line 389 "./mql.yxx"
{ mqlyymsp[-3].minor.mqlyy264 = new MQLType(kASCII, mqlyymsp[-1].minor.mqlyy263, mqlyymsp[0].minor.mqlyy263); }
#line 3119 "./mql.c"
  mqlyy_destructor(mqlyypParser,205,&mqlyymsp[-2].minor);
}
        break;
      case 93: /* feature_type ::= KEY_ID_D opt_with_index */
{  mqlyy_destructor(mqlyypParser,45,&mqlyymsp[-1].minor);
#line 391 "./mql.yxx"
{ mqlyymsp[-1].minor.mqlyy264 = new MQLType(kID_D, mqlyymsp[0].minor.mqlyy263); }
#line 3127 "./mql.c"
}
        break;
      case 94: /* feature_type ::= IDENTIFIER opt_with_index */
#line 393 "./mql.yxx"
{ mqlyylhsminor.mqlyy264 = new MQLType(mqlyymsp[-1].minor.mqlyy0->extractString(), false, mqlyymsp[0].minor.mqlyy263); deleteToken(mqlyymsp[-1].minor.mqlyy0); }
#line 3133 "./mql.c"
  mqlyymsp[-1].minor.mqlyy264 = mqlyylhsminor.mqlyy264;
        break;
      case 95: /* feature_type ::= KEY_SET KEY_OF KEY_MONADS */
{  mqlyy_destructor(mqlyypParser,46,&mqlyymsp[-2].minor);
#line 395 "./mql.yxx"
{ mqlyymsp[-2].minor.mqlyy264 = new MQLType(kORTMultipleRange); }
#line 3140 "./mql.c"
  mqlyy_destructor(mqlyypParser,41,&mqlyymsp[-1].minor);
  mqlyy_destructor(mqlyypParser,34,&mqlyymsp[0].minor);
}
        break;
      case 96: /* feature_type ::= KEY_SINGLE KEY_MONAD KEY_SET KEY_OF KEY_MONADS */
{  mqlyy_destructor(mqlyypParser,29,&mqlyymsp[-4].minor);
#line 397 "./mql.yxx"
{ mqlyymsp[-4].minor.mqlyy264 = new MQLType(kORTSingleMonad); }
#line 3149 "./mql.c"
  mqlyy_destructor(mqlyypParser,30,&mqlyymsp[-3].minor);
  mqlyy_destructor(mqlyypParser,46,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,41,&mqlyymsp[-1].minor);
  mqlyy_destructor(mqlyypParser,34,&mqlyymsp[0].minor);
}
        break;
      case 97: /* feature_type ::= KEY_SINGLE KEY_RANGE KEY_SET KEY_OF KEY_MONADS */
{  mqlyy_destructor(mqlyypParser,29,&mqlyymsp[-4].minor);
#line 399 "./mql.yxx"
{ mqlyymsp[-4].minor.mqlyy264 = new MQLType(kORTSingleRange); }
#line 3160 "./mql.c"
  mqlyy_destructor(mqlyypParser,27,&mqlyymsp[-3].minor);
  mqlyy_destructor(mqlyypParser,46,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,41,&mqlyymsp[-1].minor);
  mqlyy_destructor(mqlyypParser,34,&mqlyymsp[0].minor);
}
        break;
      case 98: /* feature_type ::= KEY_MULTIPLE KEY_RANGE KEY_SET KEY_OF KEY_MONADS */
{  mqlyy_destructor(mqlyypParser,26,&mqlyymsp[-4].minor);
#line 401 "./mql.yxx"
{ mqlyymsp[-4].minor.mqlyy264 = new MQLType(kORTMultipleRange); }
#line 3171 "./mql.c"
  mqlyy_destructor(mqlyypParser,27,&mqlyymsp[-3].minor);
  mqlyy_destructor(mqlyypParser,46,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,41,&mqlyymsp[-1].minor);
  mqlyy_destructor(mqlyypParser,34,&mqlyymsp[0].minor);
}
        break;
      case 99: /* list_feature_type ::= KEY_INTEGER */
{  mqlyy_destructor(mqlyypParser,42,&mqlyymsp[0].minor);
#line 408 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy264 = new MQLType(kListOfInteger); }
#line 3182 "./mql.c"
}
        break;
      case 100: /* list_feature_type ::= KEY_ID_D */
{  mqlyy_destructor(mqlyypParser,45,&mqlyymsp[0].minor);
#line 410 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy264 = new MQLType(kListOfID_D); }
#line 3189 "./mql.c"
}
        break;
      case 101: /* list_feature_type ::= IDENTIFIER */
#line 412 "./mql.yxx"
{ mqlyylhsminor.mqlyy264 = new MQLType(mqlyymsp[0].minor.mqlyy0->extractString(), true, false); deleteToken(mqlyymsp[0].minor.mqlyy0); }
#line 3195 "./mql.c"
  mqlyymsp[0].minor.mqlyy264 = mqlyylhsminor.mqlyy264;
        break;
      case 102: /* opt_with_index ::= KEY_WITH KEY_INDEX */
{  mqlyy_destructor(mqlyypParser,8,&mqlyymsp[-1].minor);
#line 417 "./mql.yxx"
{ mqlyymsp[-1].minor.mqlyy263 = true; }
#line 3202 "./mql.c"
  mqlyy_destructor(mqlyypParser,47,&mqlyymsp[0].minor);
}
        break;
      case 103: /* opt_with_index ::= KEY_WITHOUT KEY_INDEX */
{  mqlyy_destructor(mqlyypParser,37,&mqlyymsp[-1].minor);
#line 418 "./mql.yxx"
{ mqlyymsp[-1].minor.mqlyy263 = false; }
#line 3210 "./mql.c"
  mqlyy_destructor(mqlyypParser,47,&mqlyymsp[0].minor);
}
        break;
      case 105: /* opt_if_not_exists ::= KEY_IF KEY_NOT KEY_EXISTS */
{  mqlyy_destructor(mqlyypParser,48,&mqlyymsp[-2].minor);
#line 423 "./mql.yxx"
{ mqlyymsp[-2].minor.mqlyy263 = true; }
#line 3218 "./mql.c"
  mqlyy_destructor(mqlyypParser,49,&mqlyymsp[-1].minor);
  mqlyy_destructor(mqlyypParser,50,&mqlyymsp[0].minor);
}
        break;
      case 107: /* opt_from_set ::= KEY_FROM KEY_SET */
{  mqlyy_destructor(mqlyypParser,51,&mqlyymsp[-1].minor);
#line 428 "./mql.yxx"
{mqlyymsp[-1].minor.mqlyy263 = true;}
#line 3227 "./mql.c"
  mqlyy_destructor(mqlyypParser,46,&mqlyymsp[0].minor);
}
        break;
      case 108: /* opt_from_set ::= */
#line 429 "./mql.yxx"
{mqlyymsp[1].minor.mqlyy263 = false;}
#line 3234 "./mql.c"
        break;
      case 109: /* opt_string_length ::= */
#line 435 "./mql.yxx"
{ mqlyymsp[1].minor.mqlyy529 = 0; }
#line 3239 "./mql.c"
        break;
      case 110: /* opt_string_length ::= KEY_OPEN_BRACKET INTEGER KEY_CLOSE_BRACKET */
{  mqlyy_destructor(mqlyypParser,52,&mqlyymsp[-2].minor);
#line 437 "./mql.yxx"
{ mqlyymsp[-2].minor.mqlyy529 = 0; }
#line 3245 "./mql.c"
  mqlyy_destructor(mqlyypParser,53,&mqlyymsp[-1].minor);
  mqlyy_destructor(mqlyypParser,54,&mqlyymsp[0].minor);
}
        break;
      case 111: /* default_specification ::= KEY_DEFAULT expression */
{  mqlyy_destructor(mqlyypParser,55,&mqlyymsp[-1].minor);
#line 443 "./mql.yxx"
{ mqlyymsp[-1].minor.mqlyy413 = mqlyymsp[0].minor.mqlyy413; }
#line 3254 "./mql.c"
}
        break;
      case 112: /* default_specification ::= */
#line 445 "./mql.yxx"
{ mqlyymsp[1].minor.mqlyy413 = 0; }
#line 3260 "./mql.c"
        break;
      case 113: /* opt_computed ::= KEY_COMPUTED */
{  mqlyy_destructor(mqlyypParser,56,&mqlyymsp[0].minor);
#line 451 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy263 = true; }
#line 3266 "./mql.c"
}
        break;
      case 115: /* update_object_type_statement ::= KEY_UPDATE opt_OBJECT KEY_TYPE KEY_OPEN_SQUARE_BRACKET object_type_name feature_update_list KEY_CLOSE_SQUARE_BRACKET */
{  mqlyy_destructor(mqlyypParser,57,&mqlyymsp[-6].minor);
#line 462 "./mql.yxx"
{ mqlyymsp[-6].minor.mqlyy50 = new UpdateObjectTypeStatement((MQLExecEnv*) pEE, mqlyymsp[-2].minor.mqlyy0->extractString(), mqlyymsp[-1].minor.mqlyy380); deleteToken(mqlyymsp[-2].minor.mqlyy0); }
#line 3273 "./mql.c"
  mqlyy_destructor(mqlyypParser,191,&mqlyymsp[-5].minor);
  mqlyy_destructor(mqlyypParser,20,&mqlyymsp[-4].minor);
  mqlyy_destructor(mqlyypParser,18,&mqlyymsp[-3].minor);
  mqlyy_destructor(mqlyypParser,19,&mqlyymsp[0].minor);
}
        break;
      case 116: /* feature_update_list ::= feature_update */
#line 468 "./mql.yxx"
{ mqlyylhsminor.mqlyy380 = mqlyymsp[0].minor.mqlyy380; }
#line 3283 "./mql.c"
  mqlyymsp[0].minor.mqlyy380 = mqlyylhsminor.mqlyy380;
        break;
      case 117: /* feature_update_list ::= feature_update_list feature_update */
#line 470 "./mql.yxx"
{ mqlyylhsminor.mqlyy380 = mqlyymsp[0].minor.mqlyy380; mqlyylhsminor.mqlyy380->setNext(mqlyymsp[-1].minor.mqlyy380); }
#line 3289 "./mql.c"
  mqlyymsp[-1].minor.mqlyy380 = mqlyylhsminor.mqlyy380;
        break;
      case 118: /* feature_update ::= opt_ADD feature_declaration */
{  mqlyy_destructor(mqlyypParser,210,&mqlyymsp[-1].minor);
#line 476 "./mql.yxx"
{ mqlyymsp[-1].minor.mqlyy380 = new FeatureUpdate(mqlyymsp[0].minor.mqlyy289, 0); }
#line 3296 "./mql.c"
}
        break;
      case 119: /* feature_update ::= KEY_REMOVE feature_name KEY_SEMICOLON */
{  mqlyy_destructor(mqlyypParser,58,&mqlyymsp[-2].minor);
#line 478 "./mql.yxx"
{ mqlyymsp[-2].minor.mqlyy380 = new FeatureUpdate(mqlyymsp[-1].minor.mqlyy0->extractString(), 0); deleteToken(mqlyymsp[-1].minor.mqlyy0); }
#line 3303 "./mql.c"
  mqlyy_destructor(mqlyypParser,39,&mqlyymsp[0].minor);
}
        break;
      case 120: /* opt_ADD ::= KEY_ADD */
{  mqlyy_destructor(mqlyypParser,59,&mqlyymsp[0].minor);
#line 483 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy466 = 0; }
#line 3311 "./mql.c"
}
        break;
      case 122: /* drop_object_type_statement ::= KEY_DROP opt_OBJECT KEY_TYPE KEY_OPEN_SQUARE_BRACKET object_type_name KEY_CLOSE_SQUARE_BRACKET */
{  mqlyy_destructor(mqlyypParser,12,&mqlyymsp[-5].minor);
#line 493 "./mql.yxx"
{ mqlyymsp[-5].minor.mqlyy50 = new DropObjectTypeStatement((MQLExecEnv*) pEE, mqlyymsp[-1].minor.mqlyy0->extractString()); deleteToken(mqlyymsp[-1].minor.mqlyy0); }
#line 3318 "./mql.c"
  mqlyy_destructor(mqlyypParser,191,&mqlyymsp[-4].minor);
  mqlyy_destructor(mqlyypParser,20,&mqlyymsp[-3].minor);
  mqlyy_destructor(mqlyypParser,18,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,19,&mqlyymsp[0].minor);
}
        break;
      case 123: /* insert_monads_statement ::= KEY_INSERT monad_specification */
{  mqlyy_destructor(mqlyypParser,60,&mqlyymsp[-1].minor);
#line 499 "./mql.yxx"
{ mqlyymsp[-1].minor.mqlyy50 = new InsertMonadsStatement((MQLExecEnv*) pEE, mqlyymsp[0].minor.mqlyy108); }
#line 3329 "./mql.c"
}
        break;
      case 124: /* delete_monads_statement ::= KEY_DELETE monad_specification */
{  mqlyy_destructor(mqlyypParser,61,&mqlyymsp[-1].minor);
#line 505 "./mql.yxx"
{ mqlyymsp[-1].minor.mqlyy50 = new DeleteMonadsStatement((MQLExecEnv*) pEE, mqlyymsp[0].minor.mqlyy108); }
#line 3336 "./mql.c"
}
        break;
      case 125: /* get_monads_statement ::= KEY_GET KEY_MONADS KEY_FROM choice_number_OBJECTS KEY_WITH id_ds_specification KEY_OPEN_SQUARE_BRACKET object_type_name KEY_CLOSE_SQUARE_BRACKET */
{  mqlyy_destructor(mqlyypParser,62,&mqlyymsp[-8].minor);
#line 516 "./mql.yxx"
{ mqlyymsp[-8].minor.mqlyy50 = new GetMonadsStatement((MQLExecEnv*) pEE, mqlyymsp[-3].minor.mqlyy43, mqlyymsp[-1].minor.mqlyy0->extractString()); deleteToken(mqlyymsp[-1].minor.mqlyy0); }
#line 3343 "./mql.c"
  mqlyy_destructor(mqlyypParser,34,&mqlyymsp[-7].minor);
  mqlyy_destructor(mqlyypParser,51,&mqlyymsp[-6].minor);
  mqlyy_destructor(mqlyypParser,212,&mqlyymsp[-5].minor);
  mqlyy_destructor(mqlyypParser,8,&mqlyymsp[-4].minor);
  mqlyy_destructor(mqlyypParser,18,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,19,&mqlyymsp[0].minor);
}
        break;
      case 126: /* monad_set_calculation_statement ::= KEY_MONAD KEY_SET KEY_CALCULATION monad_set_chain */
{  mqlyy_destructor(mqlyypParser,30,&mqlyymsp[-3].minor);
#line 523 "./mql.yxx"
{ mqlyymsp[-3].minor.mqlyy50 = new MonadSetCalculationStatement((MQLExecEnv*) pEE, mqlyymsp[0].minor.mqlyy51); }
#line 3356 "./mql.c"
  mqlyy_destructor(mqlyypParser,46,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,63,&mqlyymsp[-1].minor);
}
        break;
      case 127: /* monad_set_chain ::= monad_set */
#line 529 "./mql.yxx"
{ mqlyylhsminor.mqlyy51 = new MonadSetChainElement(mqlyymsp[0].minor.mqlyy108, kSONone, 0); }
#line 3364 "./mql.c"
  mqlyymsp[0].minor.mqlyy51 = mqlyylhsminor.mqlyy51;
        break;
      case 128: /* monad_set_chain ::= monad_set_chain monad_set_operator monad_set */
#line 531 "./mql.yxx"
{ mqlyylhsminor.mqlyy51 = new MonadSetChainElement(mqlyymsp[0].minor.mqlyy108, mqlyymsp[-1].minor.mqlyy346, 0); mqlyylhsminor.mqlyy51->setNext(mqlyymsp[-2].minor.mqlyy51); }
#line 3370 "./mql.c"
  mqlyymsp[-2].minor.mqlyy51 = mqlyylhsminor.mqlyy51;
        break;
      case 129: /* monad_set_operator ::= KEY_UNION */
{  mqlyy_destructor(mqlyypParser,64,&mqlyymsp[0].minor);
#line 536 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy346 = kSOUnion; }
#line 3377 "./mql.c"
}
        break;
      case 130: /* monad_set_operator ::= KEY_DIFFERENCE */
{  mqlyy_destructor(mqlyypParser,65,&mqlyymsp[0].minor);
#line 537 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy346 = kSODifference; }
#line 3384 "./mql.c"
}
        break;
      case 131: /* monad_set_operator ::= KEY_INTERSECT */
{  mqlyy_destructor(mqlyypParser,66,&mqlyymsp[0].minor);
#line 538 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy346 = kSOIntersect; }
#line 3391 "./mql.c"
}
        break;
      case 132: /* select_monad_sets_statement ::= KEY_SELECT KEY_MONAD KEY_SETS */
{  mqlyy_destructor(mqlyypParser,67,&mqlyymsp[-2].minor);
#line 544 "./mql.yxx"
{ mqlyymsp[-2].minor.mqlyy50 = new SelectMonadSetsStatement((MQLExecEnv*) pEE); }
#line 3398 "./mql.c"
  mqlyy_destructor(mqlyypParser,30,&mqlyymsp[-1].minor);
  mqlyy_destructor(mqlyypParser,68,&mqlyymsp[0].minor);
}
        break;
      case 133: /* get_monad_sets_statement ::= KEY_GET KEY_MONAD choice_number_SET monad_set_name_list */
{  mqlyy_destructor(mqlyypParser,62,&mqlyymsp[-3].minor);
#line 551 "./mql.yxx"
{ mqlyymsp[-3].minor.mqlyy50 = new GetMonadSetsStatement((MQLExecEnv*) pEE, mqlyymsp[0].minor.mqlyy336); }
#line 3407 "./mql.c"
  mqlyy_destructor(mqlyypParser,30,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,217,&mqlyymsp[-1].minor);
}
        break;
      case 134: /* get_monad_sets_statement ::= KEY_GET KEY_MONAD choice_number_SET KEY_ALL */
{  mqlyy_destructor(mqlyypParser,62,&mqlyymsp[-3].minor);
#line 554 "./mql.yxx"
{ mqlyymsp[-3].minor.mqlyy50 = new GetMonadSetsStatement((MQLExecEnv*) pEE); }
#line 3416 "./mql.c"
  mqlyy_destructor(mqlyypParser,30,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,217,&mqlyymsp[-1].minor);
  mqlyy_destructor(mqlyypParser,69,&mqlyymsp[0].minor);
}
        break;
      case 135: /* choice_number_SET ::= KEY_SETS */
{  mqlyy_destructor(mqlyypParser,68,&mqlyymsp[0].minor);
#line 559 "./mql.yxx"
{mqlyymsp[0].minor.mqlyy466 = 0;}
#line 3426 "./mql.c"
}
        break;
      case 136: /* choice_number_SET ::= KEY_SET */
{  mqlyy_destructor(mqlyypParser,46,&mqlyymsp[0].minor);
#line 560 "./mql.yxx"
{mqlyymsp[0].minor.mqlyy466 = 0;}
#line 3433 "./mql.c"
}
        break;
      case 137: /* create_monad_set_statement ::= KEY_CREATE KEY_MONAD KEY_SET monad_set_name KEY_WITH monad_specification */
{  mqlyy_destructor(mqlyypParser,2,&mqlyymsp[-5].minor);
#line 567 "./mql.yxx"
{ mqlyymsp[-5].minor.mqlyy50 = new CreateMonadSetStatement((MQLExecEnv*) pEE, mqlyymsp[-2].minor.mqlyy0->extractString(), mqlyymsp[0].minor.mqlyy108); deleteToken(mqlyymsp[-2].minor.mqlyy0); }
#line 3440 "./mql.c"
  mqlyy_destructor(mqlyypParser,30,&mqlyymsp[-4].minor);
  mqlyy_destructor(mqlyypParser,46,&mqlyymsp[-3].minor);
  mqlyy_destructor(mqlyypParser,8,&mqlyymsp[-1].minor);
}
        break;
      case 138: /* update_monad_set_statement ::= KEY_UPDATE KEY_MONAD KEY_SET monad_set_name monad_set_operator monad_set_name */
{  mqlyy_destructor(mqlyypParser,57,&mqlyymsp[-5].minor);
#line 577 "./mql.yxx"
{ mqlyymsp[-5].minor.mqlyy50 = new UpdateMonadSetStatement((MQLExecEnv*) pEE, mqlyymsp[-2].minor.mqlyy0->extractString(), mqlyymsp[-1].minor.mqlyy346, mqlyymsp[0].minor.mqlyy0->extractString()); deleteToken(mqlyymsp[-2].minor.mqlyy0); deleteToken(mqlyymsp[0].minor.mqlyy0); }
#line 3450 "./mql.c"
  mqlyy_destructor(mqlyypParser,30,&mqlyymsp[-4].minor);
  mqlyy_destructor(mqlyypParser,46,&mqlyymsp[-3].minor);
}
        break;
      case 139: /* update_monad_set_statement ::= KEY_UPDATE KEY_MONAD KEY_SET monad_set_name KEY_REPLACE monad_set_name */
{  mqlyy_destructor(mqlyypParser,57,&mqlyymsp[-5].minor);
#line 583 "./mql.yxx"
{ mqlyymsp[-5].minor.mqlyy50 = new UpdateMonadSetStatement((MQLExecEnv*) pEE, mqlyymsp[-2].minor.mqlyy0->extractString(), kSOReplace, mqlyymsp[0].minor.mqlyy0->extractString()); deleteToken(mqlyymsp[-2].minor.mqlyy0); deleteToken(mqlyymsp[0].minor.mqlyy0); }
#line 3459 "./mql.c"
  mqlyy_destructor(mqlyypParser,30,&mqlyymsp[-4].minor);
  mqlyy_destructor(mqlyypParser,46,&mqlyymsp[-3].minor);
  mqlyy_destructor(mqlyypParser,70,&mqlyymsp[-1].minor);
}
        break;
      case 140: /* update_monad_set_statement ::= KEY_UPDATE KEY_MONAD KEY_SET monad_set_name monad_set_operator monad_set */
{  mqlyy_destructor(mqlyypParser,57,&mqlyymsp[-5].minor);
#line 589 "./mql.yxx"
{ mqlyymsp[-5].minor.mqlyy50 = new UpdateMonadSetStatement((MQLExecEnv*) pEE, mqlyymsp[-2].minor.mqlyy0->extractString(), mqlyymsp[-1].minor.mqlyy346, mqlyymsp[0].minor.mqlyy108); deleteToken(mqlyymsp[-2].minor.mqlyy0); }
#line 3469 "./mql.c"
  mqlyy_destructor(mqlyypParser,30,&mqlyymsp[-4].minor);
  mqlyy_destructor(mqlyypParser,46,&mqlyymsp[-3].minor);
}
        break;
      case 141: /* update_monad_set_statement ::= KEY_UPDATE KEY_MONAD KEY_SET monad_set_name KEY_REPLACE monad_set */
{  mqlyy_destructor(mqlyypParser,57,&mqlyymsp[-5].minor);
#line 595 "./mql.yxx"
{ mqlyymsp[-5].minor.mqlyy50 = new UpdateMonadSetStatement((MQLExecEnv*) pEE, mqlyymsp[-2].minor.mqlyy0->extractString(), kSOReplace, mqlyymsp[0].minor.mqlyy108); deleteToken(mqlyymsp[-2].minor.mqlyy0); }
#line 3478 "./mql.c"
  mqlyy_destructor(mqlyypParser,30,&mqlyymsp[-4].minor);
  mqlyy_destructor(mqlyypParser,46,&mqlyymsp[-3].minor);
  mqlyy_destructor(mqlyypParser,70,&mqlyymsp[-1].minor);
}
        break;
      case 142: /* drop_monad_set_statement ::= KEY_DROP KEY_MONAD KEY_SET monad_set_name */
{  mqlyy_destructor(mqlyypParser,12,&mqlyymsp[-3].minor);
#line 602 "./mql.yxx"
{ mqlyymsp[-3].minor.mqlyy50 = new DropMonadSetStatement((MQLExecEnv*) pEE, mqlyymsp[0].minor.mqlyy0->extractString()); deleteToken(mqlyymsp[0].minor.mqlyy0); }
#line 3488 "./mql.c"
  mqlyy_destructor(mqlyypParser,30,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,46,&mqlyymsp[-1].minor);
}
        break;
      case 144: /* monad_set_name_list ::= monad_set_name */
      case 202: /* object_type_name_list ::= object_type_name */ mqlyytestcase(mqlyyruleno==202);
      case 265: /* list_of_identifier ::= IDENTIFIER */ mqlyytestcase(mqlyyruleno==265);
#line 613 "./mql.yxx"
{ mqlyylhsminor.mqlyy336 = new StringListNode(*(mqlyymsp[0].minor.mqlyy0->pString), 0); deleteToken(mqlyymsp[0].minor.mqlyy0); }
#line 3498 "./mql.c"
  mqlyymsp[0].minor.mqlyy336 = mqlyylhsminor.mqlyy336;
        break;
      case 145: /* monad_set_name_list ::= monad_set_name_list KEY_COMMA monad_set_name */
      case 203: /* object_type_name_list ::= object_type_name_list KEY_COMMA object_type_name */ mqlyytestcase(mqlyyruleno==203);
      case 266: /* list_of_identifier ::= list_of_identifier KEY_COMMA IDENTIFIER */ mqlyytestcase(mqlyyruleno==266);
#line 616 "./mql.yxx"
{ mqlyylhsminor.mqlyy336 = new StringListNode(*(mqlyymsp[0].minor.mqlyy0->pString), mqlyymsp[-2].minor.mqlyy336); mqlyymsp[-2].minor.mqlyy336->setNext(mqlyylhsminor.mqlyy336); deleteToken(mqlyymsp[0].minor.mqlyy0); }
#line 3506 "./mql.c"
  mqlyy_destructor(mqlyypParser,71,&mqlyymsp[-1].minor);
  mqlyymsp[-2].minor.mqlyy336 = mqlyylhsminor.mqlyy336;
        break;
      case 146: /* create_enumeration_statement ::= KEY_CREATE choice_ENUM_ERATION enumeration_name KEY_EQUALS KEY_OPEN_BRACE ec_declaration_list KEY_CLOSE_BRACE */
{  mqlyy_destructor(mqlyypParser,2,&mqlyymsp[-6].minor);
#line 625 "./mql.yxx"
{ mqlyymsp[-6].minor.mqlyy50 = new CreateEnumerationStatement((MQLExecEnv*) pEE, mqlyymsp[-4].minor.mqlyy0->extractString(), mqlyymsp[-1].minor.mqlyy455); deleteToken(mqlyymsp[-4].minor.mqlyy0); }
#line 3514 "./mql.c"
  mqlyy_destructor(mqlyypParser,220,&mqlyymsp[-5].minor);
  mqlyy_destructor(mqlyypParser,72,&mqlyymsp[-3].minor);
  mqlyy_destructor(mqlyypParser,73,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,74,&mqlyymsp[0].minor);
}
        break;
      case 147: /* choice_ENUM_ERATION ::= KEY_ENUMERATION */
{  mqlyy_destructor(mqlyypParser,75,&mqlyymsp[0].minor);
#line 630 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy466 = 0; }
#line 3525 "./mql.c"
}
        break;
      case 148: /* choice_ENUM_ERATION ::= KEY_ENUM */
{  mqlyy_destructor(mqlyypParser,76,&mqlyymsp[0].minor);
#line 631 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy466 = 0; }
#line 3532 "./mql.c"
}
        break;
      case 150: /* ec_declaration_list ::= ec_declaration */
#line 641 "./mql.yxx"
{ mqlyylhsminor.mqlyy455 = mqlyymsp[0].minor.mqlyy455; }
#line 3538 "./mql.c"
  mqlyymsp[0].minor.mqlyy455 = mqlyylhsminor.mqlyy455;
        break;
      case 151: /* ec_declaration_list ::= ec_declaration_list KEY_COMMA ec_declaration */
#line 643 "./mql.yxx"
{ mqlyylhsminor.mqlyy455 = mqlyymsp[0].minor.mqlyy455; mqlyylhsminor.mqlyy455->setNext(mqlyymsp[-2].minor.mqlyy455); }
#line 3544 "./mql.c"
  mqlyy_destructor(mqlyypParser,71,&mqlyymsp[-1].minor);
  mqlyymsp[-2].minor.mqlyy455 = mqlyylhsminor.mqlyy455;
        break;
      case 152: /* ec_declaration ::= opt_DEFAULT ec_name opt_ec_initialization */
#line 648 "./mql.yxx"
{ mqlyylhsminor.mqlyy455 = new ECDeclaration(mqlyymsp[-2].minor.mqlyy263, mqlyymsp[-1].minor.mqlyy0->extractString(), mqlyymsp[0].minor.mqlyy473, 0); 
      deleteToken(mqlyymsp[-1].minor.mqlyy0); 
    }
#line 3553 "./mql.c"
  mqlyymsp[-2].minor.mqlyy455 = mqlyylhsminor.mqlyy455;
        break;
      case 153: /* opt_DEFAULT ::= KEY_DEFAULT */
{  mqlyy_destructor(mqlyypParser,55,&mqlyymsp[0].minor);
#line 656 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy263 = true; }
#line 3560 "./mql.c"
}
        break;
      case 156: /* opt_ec_initialization ::= ec_initialization */
#line 668 "./mql.yxx"
{ mqlyylhsminor.mqlyy473 = new emdros_int64(mqlyymsp[0].minor.mqlyy503); }
#line 3566 "./mql.c"
  mqlyymsp[0].minor.mqlyy473 = mqlyylhsminor.mqlyy473;
        break;
      case 157: /* opt_ec_initialization ::= */
#line 671 "./mql.yxx"
{ mqlyymsp[1].minor.mqlyy473 = 0; }
#line 3572 "./mql.c"
        break;
      case 158: /* update_enumeration_statement ::= KEY_UPDATE choice_ENUM_ERATION enumeration_name KEY_EQUALS KEY_OPEN_BRACE ec_update_list KEY_CLOSE_BRACE */
{  mqlyy_destructor(mqlyypParser,57,&mqlyymsp[-6].minor);
#line 679 "./mql.yxx"
{ mqlyymsp[-6].minor.mqlyy50 = new UpdateEnumerationStatement((MQLExecEnv*) pEE, mqlyymsp[-4].minor.mqlyy0->extractString(), mqlyymsp[-1].minor.mqlyy46); deleteToken(mqlyymsp[-4].minor.mqlyy0); }
#line 3578 "./mql.c"
  mqlyy_destructor(mqlyypParser,220,&mqlyymsp[-5].minor);
  mqlyy_destructor(mqlyypParser,72,&mqlyymsp[-3].minor);
  mqlyy_destructor(mqlyypParser,73,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,74,&mqlyymsp[0].minor);
}
        break;
      case 159: /* ec_update_list ::= ec_update */
#line 685 "./mql.yxx"
{ mqlyylhsminor.mqlyy46 = mqlyymsp[0].minor.mqlyy46; }
#line 3588 "./mql.c"
  mqlyymsp[0].minor.mqlyy46 = mqlyylhsminor.mqlyy46;
        break;
      case 160: /* ec_update_list ::= ec_update_list KEY_COMMA ec_update */
#line 687 "./mql.yxx"
{ mqlyylhsminor.mqlyy46 = mqlyymsp[0].minor.mqlyy46; mqlyylhsminor.mqlyy46->setNext(mqlyymsp[-2].minor.mqlyy46); }
#line 3594 "./mql.c"
  mqlyy_destructor(mqlyypParser,71,&mqlyymsp[-1].minor);
  mqlyymsp[-2].minor.mqlyy46 = mqlyylhsminor.mqlyy46;
        break;
      case 161: /* ec_update ::= opt_ADD opt_DEFAULT ec_name ec_initialization */
{  mqlyy_destructor(mqlyypParser,210,&mqlyymsp[-3].minor);
#line 693 "./mql.yxx"
{ mqlyymsp[-3].minor.mqlyy46 = new ECUpdate(mqlyymsp[-2].minor.mqlyy263, mqlyymsp[-1].minor.mqlyy0->extractString(), mqlyymsp[0].minor.mqlyy503, kAdd, 0); deleteToken(mqlyymsp[-1].minor.mqlyy0); }
#line 3602 "./mql.c"
}
        break;
      case 162: /* ec_update ::= KEY_UPDATE opt_DEFAULT ec_name ec_initialization */
{  mqlyy_destructor(mqlyypParser,57,&mqlyymsp[-3].minor);
#line 695 "./mql.yxx"
{ mqlyymsp[-3].minor.mqlyy46 = new ECUpdate(mqlyymsp[-2].minor.mqlyy263, mqlyymsp[-1].minor.mqlyy0->extractString(), mqlyymsp[0].minor.mqlyy503, kUpdate, 0); deleteToken(mqlyymsp[-1].minor.mqlyy0); }
#line 3609 "./mql.c"
}
        break;
      case 163: /* ec_update ::= KEY_REMOVE ec_name */
{  mqlyy_destructor(mqlyypParser,58,&mqlyymsp[-1].minor);
#line 697 "./mql.yxx"
{ mqlyymsp[-1].minor.mqlyy46 = new ECUpdate(false, mqlyymsp[0].minor.mqlyy0->extractString(), 0, kRemove, 0); deleteToken(mqlyymsp[0].minor.mqlyy0); }
#line 3616 "./mql.c"
}
        break;
      case 164: /* ec_initialization ::= KEY_EQUALS signed_integer */
{  mqlyy_destructor(mqlyypParser,72,&mqlyymsp[-1].minor);
#line 702 "./mql.yxx"
{ mqlyymsp[-1].minor.mqlyy503 = mqlyymsp[0].minor.mqlyy503; }
#line 3623 "./mql.c"
}
        break;
      case 165: /* signed_integer ::= INTEGER */
      case 168: /* unsigned_integer ::= INTEGER */ mqlyytestcase(mqlyyruleno==168);
#line 706 "./mql.yxx"
{ mqlyylhsminor.mqlyy503 = mqlyymsp[0].minor.mqlyy0->integer; deleteToken(mqlyymsp[0].minor.mqlyy0); }
#line 3630 "./mql.c"
  mqlyymsp[0].minor.mqlyy503 = mqlyylhsminor.mqlyy503;
        break;
      case 166: /* signed_integer ::= KEY_DASH INTEGER */
{  mqlyy_destructor(mqlyypParser,77,&mqlyymsp[-1].minor);
#line 707 "./mql.yxx"
{ mqlyymsp[-1].minor.mqlyy503 = -(mqlyymsp[0].minor.mqlyy0->integer); deleteToken(mqlyymsp[0].minor.mqlyy0); }
#line 3637 "./mql.c"
}
        break;
      case 167: /* signed_integer ::= KEY_NIL */
{  mqlyy_destructor(mqlyypParser,78,&mqlyymsp[0].minor);
#line 708 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy503 = NIL; }
#line 3644 "./mql.c"
}
        break;
      case 169: /* drop_enumeration_statement ::= KEY_DROP choice_ENUM_ERATION enumeration_name */
{  mqlyy_destructor(mqlyypParser,12,&mqlyymsp[-2].minor);
#line 721 "./mql.yxx"
{ mqlyymsp[-2].minor.mqlyy50 = new DropEnumerationStatement((MQLExecEnv*) pEE, mqlyymsp[0].minor.mqlyy0->extractString()); deleteToken(mqlyymsp[0].minor.mqlyy0); }
#line 3651 "./mql.c"
  mqlyy_destructor(mqlyypParser,220,&mqlyymsp[-1].minor);
}
        break;
      case 170: /* create_segment_statement ::= KEY_CREATE KEY_SEGMENT segment_name KEY_RANGE KEY_EQUALS segment_range */
{  mqlyy_destructor(mqlyypParser,2,&mqlyymsp[-5].minor);
#line 731 "./mql.yxx"
{ mqlyymsp[-5].minor.mqlyy50 = new CreateSegmentStatement((MQLExecEnv*) pEE, mqlyymsp[-3].minor.mqlyy0->extractString(), mqlyymsp[0].minor.mqlyy108); deleteToken(mqlyymsp[-3].minor.mqlyy0); }
#line 3659 "./mql.c"
  mqlyy_destructor(mqlyypParser,79,&mqlyymsp[-4].minor);
  mqlyy_destructor(mqlyypParser,27,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,72,&mqlyymsp[-1].minor);
}
        break;
      case 172: /* segment_range ::= INTEGER KEY_DASH INTEGER */
#line 741 "./mql.yxx"
{ mqlyylhsminor.mqlyy108 = new MQLMonadSetElement(mqlyymsp[-2].minor.mqlyy0->integer, mqlyymsp[0].minor.mqlyy0->integer, 0, true);  deleteToken(mqlyymsp[-2].minor.mqlyy0); deleteToken(mqlyymsp[0].minor.mqlyy0); }
#line 3668 "./mql.c"
  mqlyy_destructor(mqlyypParser,77,&mqlyymsp[-1].minor);
  mqlyymsp[-2].minor.mqlyy108 = mqlyylhsminor.mqlyy108;
        break;
      case 173: /* select_statement ::= select_clause in_clause with_max_range_clause returning_clause where_clause */
#line 751 "./mql.yxx"
{ mqlyylhsminor.mqlyy50 = new SelectStatement((MQLExecEnv*) pEE, mqlyymsp[-4].minor.mqlyy273, mqlyymsp[-3].minor.mqlyy108, mqlyymsp[-2].minor.mqlyy448, mqlyymsp[-1].minor.mqlyy551, mqlyymsp[0].minor.mqlyy269); }
#line 3675 "./mql.c"
  mqlyymsp[-4].minor.mqlyy50 = mqlyylhsminor.mqlyy50;
        break;
      case 174: /* select_statement ::= select_clause KEY_IN IDENTIFIER with_max_range_clause returning_clause where_clause */
#line 757 "./mql.yxx"
{ mqlyylhsminor.mqlyy50 = new SelectStatement((MQLExecEnv*) pEE, mqlyymsp[-5].minor.mqlyy273, mqlyymsp[-3].minor.mqlyy0->extractString(), mqlyymsp[-2].minor.mqlyy448, mqlyymsp[-1].minor.mqlyy551, mqlyymsp[0].minor.mqlyy269); deleteToken(mqlyymsp[-3].minor.mqlyy0); }
#line 3681 "./mql.c"
  mqlyy_destructor(mqlyypParser,80,&mqlyymsp[-4].minor);
  mqlyymsp[-5].minor.mqlyy50 = mqlyylhsminor.mqlyy50;
        break;
      case 175: /* select_clause ::= KEY_SELECT focus_specification opt_OBJECTS */
{  mqlyy_destructor(mqlyypParser,67,&mqlyymsp[-2].minor);
#line 763 "./mql.yxx"
{ mqlyymsp[-2].minor.mqlyy273 = mqlyymsp[-1].minor.mqlyy273; }
#line 3689 "./mql.c"
  mqlyy_destructor(mqlyypParser,240,&mqlyymsp[0].minor);
}
        break;
      case 176: /* focus_specification ::= KEY_FOCUS */
{  mqlyy_destructor(mqlyypParser,81,&mqlyymsp[0].minor);
#line 768 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy273 = kFocus; }
#line 3697 "./mql.c"
}
        break;
      case 177: /* focus_specification ::= KEY_ALL */
{  mqlyy_destructor(mqlyypParser,69,&mqlyymsp[0].minor);
#line 769 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy273 = kAll; }
#line 3704 "./mql.c"
}
        break;
      case 178: /* opt_OBJECTS ::= KEY_OBJECTS */
{  mqlyy_destructor(mqlyypParser,28,&mqlyymsp[0].minor);
#line 774 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy466 = 0; }
#line 3711 "./mql.c"
}
        break;
      case 180: /* in_clause ::= KEY_IN in_specification */
{  mqlyy_destructor(mqlyypParser,80,&mqlyymsp[-1].minor);
#line 781 "./mql.yxx"
{ mqlyymsp[-1].minor.mqlyy108 = mqlyymsp[0].minor.mqlyy108; mqlyymsp[-1].minor.mqlyy108->setIsFirst(true); }
#line 3718 "./mql.c"
}
        break;
      case 181: /* in_clause ::= */
#line 783 "./mql.yxx"
{ mqlyymsp[1].minor.mqlyy108 = new MQLMonadSetElement(1L, MAX_MONAD, 0, true); }
#line 3724 "./mql.c"
        break;
      case 182: /* in_specification ::= monad_set */
      case 368: /* star_monad_set ::= monad_set */ mqlyytestcase(mqlyyruleno==368);
#line 789 "./mql.yxx"
{ mqlyylhsminor.mqlyy108 = mqlyymsp[0].minor.mqlyy108; mqlyylhsminor.mqlyy108->setIsFirst(true); }
#line 3730 "./mql.c"
  mqlyymsp[0].minor.mqlyy108 = mqlyylhsminor.mqlyy108;
        break;
      case 183: /* in_specification ::= KEY_ALL */
{  mqlyy_destructor(mqlyypParser,69,&mqlyymsp[0].minor);
#line 791 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy108 = new MQLMonadSetElement(1L, MAX_MONAD, 0, true); }
#line 3737 "./mql.c"
}
        break;
      case 184: /* monad_set ::= KEY_OPEN_BRACE monad_set_element_list KEY_CLOSE_BRACE */
{  mqlyy_destructor(mqlyypParser,73,&mqlyymsp[-2].minor);
#line 797 "./mql.yxx"
{ mqlyymsp[-2].minor.mqlyy108 = mqlyymsp[-1].minor.mqlyy108; mqlyymsp[-2].minor.mqlyy108->setIsFirst(true); }
#line 3744 "./mql.c"
  mqlyy_destructor(mqlyypParser,74,&mqlyymsp[0].minor);
}
        break;
      case 185: /* monad_set_element_list ::= monad_set_element */
#line 803 "./mql.yxx"
{ mqlyylhsminor.mqlyy108 = mqlyymsp[0].minor.mqlyy108; }
#line 3751 "./mql.c"
  mqlyymsp[0].minor.mqlyy108 = mqlyylhsminor.mqlyy108;
        break;
      case 186: /* monad_set_element_list ::= monad_set_element_list KEY_COMMA monad_set_element */
#line 806 "./mql.yxx"
{ mqlyylhsminor.mqlyy108 = mqlyymsp[0].minor.mqlyy108; mqlyylhsminor.mqlyy108->setNext(mqlyymsp[-2].minor.mqlyy108); }
#line 3757 "./mql.c"
  mqlyy_destructor(mqlyypParser,71,&mqlyymsp[-1].minor);
  mqlyymsp[-2].minor.mqlyy108 = mqlyylhsminor.mqlyy108;
        break;
      case 187: /* monad_set_element ::= INTEGER */
#line 812 "./mql.yxx"
{ mqlyylhsminor.mqlyy108 = new MQLMonadSetElement(mqlyymsp[0].minor.mqlyy0->integer, 0, false); deleteToken(mqlyymsp[0].minor.mqlyy0); }
#line 3764 "./mql.c"
  mqlyymsp[0].minor.mqlyy108 = mqlyylhsminor.mqlyy108;
        break;
      case 188: /* monad_set_element ::= INTEGER KEY_DASH INTEGER */
#line 814 "./mql.yxx"
{ mqlyylhsminor.mqlyy108 = new MQLMonadSetElement(mqlyymsp[-2].minor.mqlyy0->integer, mqlyymsp[0].minor.mqlyy0->integer, 0, false); deleteToken(mqlyymsp[-2].minor.mqlyy0); deleteToken(mqlyymsp[0].minor.mqlyy0); }
#line 3770 "./mql.c"
  mqlyy_destructor(mqlyypParser,77,&mqlyymsp[-1].minor);
  mqlyymsp[-2].minor.mqlyy108 = mqlyylhsminor.mqlyy108;
        break;
      case 189: /* monad_set_element ::= INTEGER KEY_DASH */
#line 816 "./mql.yxx"
{ mqlyylhsminor.mqlyy108 = new MQLMonadSetElement(mqlyymsp[-1].minor.mqlyy0->integer, MAX_MONAD, 0, false); deleteToken(mqlyymsp[-1].minor.mqlyy0); }
#line 3777 "./mql.c"
  mqlyy_destructor(mqlyypParser,77,&mqlyymsp[0].minor);
  mqlyymsp[-1].minor.mqlyy108 = mqlyylhsminor.mqlyy108;
        break;
      case 190: /* with_max_range_clause ::= */
#line 822 "./mql.yxx"
{ mqlyymsp[1].minor.mqlyy448 = new MaxRange(MAX_MONAD); }
#line 3784 "./mql.c"
        break;
      case 191: /* with_max_range_clause ::= KEY_WITH KEY_MAX KEY_RANGE KEY_MAX_M KEY_MONADS */
{  mqlyy_destructor(mqlyypParser,8,&mqlyymsp[-4].minor);
#line 824 "./mql.yxx"
{ mqlyymsp[-4].minor.mqlyy448 = new MaxRange(MAX_MONAD); }
#line 3790 "./mql.c"
  mqlyy_destructor(mqlyypParser,82,&mqlyymsp[-3].minor);
  mqlyy_destructor(mqlyypParser,27,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,83,&mqlyymsp[-1].minor);
  mqlyy_destructor(mqlyypParser,34,&mqlyymsp[0].minor);
}
        break;
      case 192: /* with_max_range_clause ::= KEY_WITH KEY_MAX KEY_RANGE INTEGER KEY_MONADS */
{  mqlyy_destructor(mqlyypParser,8,&mqlyymsp[-4].minor);
#line 826 "./mql.yxx"
{ mqlyymsp[-4].minor.mqlyy448 = new MaxRange(mqlyymsp[-1].minor.mqlyy0->integer); deleteToken(mqlyymsp[-1].minor.mqlyy0); }
#line 3801 "./mql.c"
  mqlyy_destructor(mqlyypParser,82,&mqlyymsp[-3].minor);
  mqlyy_destructor(mqlyypParser,27,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,34,&mqlyymsp[0].minor);
}
        break;
      case 193: /* with_max_range_clause ::= KEY_WITH KEY_MAX KEY_RANGE KEY_FEATURE feature_name KEY_FROM KEY_OPEN_SQUARE_BRACKET object_type_name KEY_CLOSE_SQUARE_BRACKET */
{  mqlyy_destructor(mqlyypParser,8,&mqlyymsp[-8].minor);
#line 828 "./mql.yxx"
{ mqlyymsp[-8].minor.mqlyy448 = new MaxRange(mqlyymsp[-1].minor.mqlyy0->extractString(), mqlyymsp[-4].minor.mqlyy0->extractString()); deleteToken(mqlyymsp[-1].minor.mqlyy0); deleteToken(mqlyymsp[-4].minor.mqlyy0); }
#line 3811 "./mql.c"
  mqlyy_destructor(mqlyypParser,82,&mqlyymsp[-7].minor);
  mqlyy_destructor(mqlyypParser,27,&mqlyymsp[-6].minor);
  mqlyy_destructor(mqlyypParser,84,&mqlyymsp[-5].minor);
  mqlyy_destructor(mqlyypParser,51,&mqlyymsp[-3].minor);
  mqlyy_destructor(mqlyypParser,18,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,19,&mqlyymsp[0].minor);
}
        break;
      case 194: /* returning_clause ::= */
#line 834 "./mql.yxx"
{ mqlyymsp[1].minor.mqlyy551 = new sheaf_return_type_pair(kFullSheaf, ((StringList*) 0)); }
#line 3823 "./mql.c"
        break;
      case 195: /* returning_clause ::= KEY_RETURNING KEY_FULL KEY_SHEAF */
{  mqlyy_destructor(mqlyypParser,85,&mqlyymsp[-2].minor);
#line 836 "./mql.yxx"
{ mqlyymsp[-2].minor.mqlyy551 = new sheaf_return_type_pair(kFullSheaf, ((StringList*) 0)); }
#line 3829 "./mql.c"
  mqlyy_destructor(mqlyypParser,86,&mqlyymsp[-1].minor);
  mqlyy_destructor(mqlyypParser,87,&mqlyymsp[0].minor);
}
        break;
      case 196: /* returning_clause ::= KEY_RETURNING KEY_FLAT KEY_SHEAF */
{  mqlyy_destructor(mqlyypParser,85,&mqlyymsp[-2].minor);
#line 838 "./mql.yxx"
{ StringList *pSL = new StringList(); 
      mqlyymsp[-2].minor.mqlyy551 = new sheaf_return_type_pair(kFlatSheaf, pSL); }
#line 3839 "./mql.c"
  mqlyy_destructor(mqlyypParser,88,&mqlyymsp[-1].minor);
  mqlyy_destructor(mqlyypParser,87,&mqlyymsp[0].minor);
}
        break;
      case 197: /* returning_clause ::= KEY_RETURNING KEY_FLAT KEY_SHEAF KEY_ON object_type_name_list */
{  mqlyy_destructor(mqlyypParser,85,&mqlyymsp[-4].minor);
#line 842 "./mql.yxx"
{ StringList *pSL = new StringList(mqlyymsp[0].minor.mqlyy336); 
      delete mqlyymsp[0].minor.mqlyy336;
      mqlyymsp[-4].minor.mqlyy551 = new sheaf_return_type_pair(kFlatSheaf, pSL); }
#line 3850 "./mql.c"
  mqlyy_destructor(mqlyypParser,88,&mqlyymsp[-3].minor);
  mqlyy_destructor(mqlyypParser,87,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,16,&mqlyymsp[-1].minor);
}
        break;
      case 198: /* using_range_clause ::= */
#line 849 "./mql.yxx"
{ mqlyymsp[1].minor.mqlyy600 = 0; }
#line 3859 "./mql.c"
        break;
      case 199: /* using_range_clause ::= KEY_RANGE KEY_ALL */
{  mqlyy_destructor(mqlyypParser,27,&mqlyymsp[-1].minor);
#line 851 "./mql.yxx"
{ mqlyymsp[-1].minor.mqlyy600 = 0; }
#line 3865 "./mql.c"
  mqlyy_destructor(mqlyypParser,69,&mqlyymsp[0].minor);
}
        break;
      case 200: /* using_range_clause ::= KEY_RANGE KEY_OPEN_BRACKET unsigned_integer KEY_COMMA unsigned_integer KEY_CLOSE_BRACKET */
{  mqlyy_destructor(mqlyypParser,27,&mqlyymsp[-5].minor);
#line 856 "./mql.yxx"
{ mqlyymsp[-5].minor.mqlyy600 = new UsingRange(mqlyymsp[-3].minor.mqlyy503, mqlyymsp[-1].minor.mqlyy503); }
#line 3873 "./mql.c"
  mqlyy_destructor(mqlyypParser,52,&mqlyymsp[-4].minor);
  mqlyy_destructor(mqlyypParser,71,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,54,&mqlyymsp[0].minor);
}
        break;
      case 201: /* using_range_clause ::= KEY_RANGE KEY_OPEN_BRACKET unsigned_integer KEY_CLOSE_BRACKET */
{  mqlyy_destructor(mqlyypParser,27,&mqlyymsp[-3].minor);
#line 861 "./mql.yxx"
{ mqlyymsp[-3].minor.mqlyy600 = new UsingRange(0, mqlyymsp[-1].minor.mqlyy503); }
#line 3883 "./mql.c"
  mqlyy_destructor(mqlyypParser,52,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,54,&mqlyymsp[0].minor);
}
        break;
      case 204: /* where_clause ::= KEY_WHERE mql_query */
{  mqlyy_destructor(mqlyypParser,89,&mqlyymsp[-1].minor);
#line 874 "./mql.yxx"
{ mqlyymsp[-1].minor.mqlyy269 = mqlyymsp[0].minor.mqlyy269; }
#line 3892 "./mql.c"
}
        break;
      case 205: /* select_objects_at_statement ::= KEY_SELECT opt_OBJECTS KEY_AT single_monad_specification KEY_OPEN_SQUARE_BRACKET object_type_name KEY_CLOSE_SQUARE_BRACKET */
{  mqlyy_destructor(mqlyypParser,67,&mqlyymsp[-6].minor);
#line 884 "./mql.yxx"
{ mqlyymsp[-6].minor.mqlyy50 = new SelectObjectsAtStatement((MQLExecEnv*) pEE, mqlyymsp[-3].minor.mqlyy244, mqlyymsp[-1].minor.mqlyy0->extractString()); deleteToken(mqlyymsp[-1].minor.mqlyy0); }
#line 3899 "./mql.c"
  mqlyy_destructor(mqlyypParser,240,&mqlyymsp[-5].minor);
  mqlyy_destructor(mqlyypParser,90,&mqlyymsp[-4].minor);
  mqlyy_destructor(mqlyypParser,18,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,19,&mqlyymsp[0].minor);
}
        break;
      case 206: /* single_monad_specification ::= KEY_MONAD KEY_EQUALS INTEGER */
{  mqlyy_destructor(mqlyypParser,30,&mqlyymsp[-2].minor);
#line 890 "./mql.yxx"
{ mqlyymsp[-2].minor.mqlyy244 = mqlyymsp[0].minor.mqlyy0->integer; deleteToken(mqlyymsp[0].minor.mqlyy0); }
#line 3910 "./mql.c"
  mqlyy_destructor(mqlyypParser,72,&mqlyymsp[-1].minor);
}
        break;
      case 207: /* select_objects_having_monads_in_statement ::= KEY_SELECT KEY_OBJECTS KEY_HAVING KEY_MONADS KEY_IN monad_set KEY_OPEN_SQUARE_BRACKET object_type_to_find KEY_CLOSE_SQUARE_BRACKET */
{  mqlyy_destructor(mqlyypParser,67,&mqlyymsp[-8].minor);
#line 901 "./mql.yxx"
{ mqlyymsp[-8].minor.mqlyy50 = new SelectObjectsHavingMonadsInStatement((MQLExecEnv*) pEE, mqlyymsp[-3].minor.mqlyy108, *(mqlyymsp[-1].minor.mqlyy0->pString)); deleteToken(mqlyymsp[-1].minor.mqlyy0); }
#line 3918 "./mql.c"
  mqlyy_destructor(mqlyypParser,28,&mqlyymsp[-7].minor);
  mqlyy_destructor(mqlyypParser,31,&mqlyymsp[-6].minor);
  mqlyy_destructor(mqlyypParser,34,&mqlyymsp[-5].minor);
  mqlyy_destructor(mqlyypParser,80,&mqlyymsp[-4].minor);
  mqlyy_destructor(mqlyypParser,18,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,19,&mqlyymsp[0].minor);
}
        break;
      case 210: /* choice_object_type_or_all ::= KEY_ALL */
{  mqlyy_destructor(mqlyypParser,69,&mqlyymsp[0].minor);
#line 913 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy0 = newToken(); mqlyymsp[0].minor.mqlyy0->setString(new std::string("all")); }
#line 3931 "./mql.c"
}
        break;
      case 211: /* get_aggregate_features_statement ::= KEY_GET KEY_AGGREGATE KEY_FEATURES aggregate_feature_list KEY_FROM KEY_OBJECTS in_clause using_monad_feature KEY_OPEN_SQUARE_BRACKET object_type_name feature_constraints KEY_CLOSE_SQUARE_BRACKET */
{  mqlyy_destructor(mqlyypParser,62,&mqlyymsp[-11].minor);
#line 927 "./mql.yxx"
{ mqlyymsp[-11].minor.mqlyy50 = new GetAggregateFeaturesStatement((MQLExecEnv*) pEE, mqlyymsp[-8].minor.mqlyy344, mqlyymsp[-5].minor.mqlyy108, mqlyymsp[-4].minor.mqlyy0->extractString(), mqlyymsp[-2].minor.mqlyy0->extractString(), mqlyymsp[-1].minor.mqlyy520); deleteToken(mqlyymsp[-4].minor.mqlyy0); deleteToken(mqlyymsp[-2].minor.mqlyy0); }
#line 3938 "./mql.c"
  mqlyy_destructor(mqlyypParser,91,&mqlyymsp[-10].minor);
  mqlyy_destructor(mqlyypParser,92,&mqlyymsp[-9].minor);
  mqlyy_destructor(mqlyypParser,51,&mqlyymsp[-7].minor);
  mqlyy_destructor(mqlyypParser,28,&mqlyymsp[-6].minor);
  mqlyy_destructor(mqlyypParser,18,&mqlyymsp[-3].minor);
  mqlyy_destructor(mqlyypParser,19,&mqlyymsp[0].minor);
}
        break;
      case 212: /* aggregate_feature_list ::= aggregate_feature */
#line 932 "./mql.yxx"
{ mqlyylhsminor.mqlyy344 = mqlyymsp[0].minor.mqlyy344; }
#line 3950 "./mql.c"
  mqlyymsp[0].minor.mqlyy344 = mqlyylhsminor.mqlyy344;
        break;
      case 213: /* aggregate_feature_list ::= aggregate_feature_list KEY_COMMA aggregate_feature */
#line 936 "./mql.yxx"
{ mqlyylhsminor.mqlyy344 = mqlyymsp[0].minor.mqlyy344; mqlyylhsminor.mqlyy344->setNext(mqlyymsp[-2].minor.mqlyy344); }
#line 3956 "./mql.c"
  mqlyy_destructor(mqlyypParser,71,&mqlyymsp[-1].minor);
  mqlyymsp[-2].minor.mqlyy344 = mqlyylhsminor.mqlyy344;
        break;
      case 214: /* aggregate_feature ::= KEY_MIN KEY_OPEN_BRACKET feature_name KEY_CLOSE_BRACKET */
{  mqlyy_destructor(mqlyypParser,93,&mqlyymsp[-3].minor);
#line 941 "./mql.yxx"
{ mqlyymsp[-3].minor.mqlyy344 = new AggregateFeature(kAFMIN, mqlyymsp[-1].minor.mqlyy0->extractString()); deleteToken(mqlyymsp[-1].minor.mqlyy0); }
#line 3964 "./mql.c"
  mqlyy_destructor(mqlyypParser,52,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,54,&mqlyymsp[0].minor);
}
        break;
      case 215: /* aggregate_feature ::= KEY_MAX KEY_OPEN_BRACKET feature_name KEY_CLOSE_BRACKET */
{  mqlyy_destructor(mqlyypParser,82,&mqlyymsp[-3].minor);
#line 943 "./mql.yxx"
{ mqlyymsp[-3].minor.mqlyy344 = new AggregateFeature(kAFMAX, mqlyymsp[-1].minor.mqlyy0->extractString()); deleteToken(mqlyymsp[-1].minor.mqlyy0); }
#line 3973 "./mql.c"
  mqlyy_destructor(mqlyypParser,52,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,54,&mqlyymsp[0].minor);
}
        break;
      case 216: /* aggregate_feature ::= KEY_SUM KEY_OPEN_BRACKET feature_name KEY_CLOSE_BRACKET */
{  mqlyy_destructor(mqlyypParser,94,&mqlyymsp[-3].minor);
#line 945 "./mql.yxx"
{ mqlyymsp[-3].minor.mqlyy344 = new AggregateFeature(kAFSUM, mqlyymsp[-1].minor.mqlyy0->extractString()); deleteToken(mqlyymsp[-1].minor.mqlyy0); }
#line 3982 "./mql.c"
  mqlyy_destructor(mqlyypParser,52,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,54,&mqlyymsp[0].minor);
}
        break;
      case 217: /* aggregate_feature ::= KEY_COUNT KEY_OPEN_BRACKET KEY_STAR KEY_CLOSE_BRACKET */
{  mqlyy_destructor(mqlyypParser,95,&mqlyymsp[-3].minor);
#line 947 "./mql.yxx"
{ mqlyymsp[-3].minor.mqlyy344 = new AggregateFeature(kAFCOUNT_ALL); }
#line 3991 "./mql.c"
  mqlyy_destructor(mqlyypParser,52,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,96,&mqlyymsp[-1].minor);
  mqlyy_destructor(mqlyypParser,54,&mqlyymsp[0].minor);
}
        break;
      case 218: /* aggregate_feature ::= KEY_COUNT KEY_OPEN_BRACKET aggregate_feature_comparison KEY_CLOSE_BRACKET */
{  mqlyy_destructor(mqlyypParser,95,&mqlyymsp[-3].minor);
#line 950 "./mql.yxx"
{ mqlyymsp[-3].minor.mqlyy344 = new AggregateFeature(kAFCOUNT_FEATURE_COMPARISON, mqlyymsp[-1].minor.mqlyy68); }
#line 4001 "./mql.c"
  mqlyy_destructor(mqlyypParser,52,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,54,&mqlyymsp[0].minor);
}
        break;
      case 219: /* aggregate_feature_comparison ::= feature_name comparison_operator value */
      case 338: /* feature_comparison ::= feature_name comparison_operator value */ mqlyytestcase(mqlyyruleno==338);
#line 955 "./mql.yxx"
{ mqlyylhsminor.mqlyy68 = new FeatureComparison(mqlyymsp[-2].minor.mqlyy0->extractString(), mqlyymsp[-1].minor.mqlyy130, mqlyymsp[0].minor.mqlyy616); deleteToken(mqlyymsp[-2].minor.mqlyy0); }
#line 4010 "./mql.c"
  mqlyymsp[-2].minor.mqlyy68 = mqlyylhsminor.mqlyy68;
        break;
      case 220: /* aggregate_feature_comparison ::= feature_name KEY_IN KEY_OPEN_BRACKET list_of_identifier KEY_CLOSE_BRACKET */
      case 339: /* feature_comparison ::= feature_name KEY_IN KEY_OPEN_BRACKET list_of_identifier KEY_CLOSE_BRACKET */ mqlyytestcase(mqlyyruleno==339);
#line 957 "./mql.yxx"
{ mqlyylhsminor.mqlyy68 = new FeatureComparison(mqlyymsp[-4].minor.mqlyy0->extractString(), mqlyymsp[-1].minor.mqlyy336); deleteToken(mqlyymsp[-4].minor.mqlyy0); }
#line 4017 "./mql.c"
  mqlyy_destructor(mqlyypParser,80,&mqlyymsp[-3].minor);
  mqlyy_destructor(mqlyypParser,52,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,54,&mqlyymsp[0].minor);
  mqlyymsp[-4].minor.mqlyy68 = mqlyylhsminor.mqlyy68;
        break;
      case 221: /* aggregate_feature_comparison ::= feature_name KEY_IN KEY_OPEN_BRACKET list_of_integer KEY_CLOSE_BRACKET */
      case 340: /* feature_comparison ::= feature_name KEY_IN KEY_OPEN_BRACKET list_of_integer KEY_CLOSE_BRACKET */ mqlyytestcase(mqlyyruleno==340);
#line 959 "./mql.yxx"
{ mqlyylhsminor.mqlyy68 = new FeatureComparison(mqlyymsp[-4].minor.mqlyy0->extractString(), mqlyymsp[-1].minor.mqlyy17); deleteToken(mqlyymsp[-4].minor.mqlyy0); }
#line 4027 "./mql.c"
  mqlyy_destructor(mqlyypParser,80,&mqlyymsp[-3].minor);
  mqlyy_destructor(mqlyypParser,52,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,54,&mqlyymsp[0].minor);
  mqlyymsp[-4].minor.mqlyy68 = mqlyylhsminor.mqlyy68;
        break;
      case 222: /* get_objects_having_monads_in_statement ::= KEY_GET KEY_OBJECTS KEY_HAVING KEY_MONADS KEY_IN in_specification using_monad_feature KEY_OPEN_SQUARE_BRACKET object_type_name KEY_CLOSE_SQUARE_BRACKET */
{  mqlyy_destructor(mqlyypParser,62,&mqlyymsp[-9].minor);
#line 975 "./mql.yxx"
{ mqlyymsp[-9].minor.mqlyy50 = new GetObjectsHavingMonadsInStatement((MQLExecEnv*) pEE, mqlyymsp[-4].minor.mqlyy108, mqlyymsp[-1].minor.mqlyy0->extractString(), mqlyymsp[-3].minor.mqlyy0->extractString()); deleteToken(mqlyymsp[-1].minor.mqlyy0); deleteToken(mqlyymsp[-3].minor.mqlyy0); }
#line 4037 "./mql.c"
  mqlyy_destructor(mqlyypParser,28,&mqlyymsp[-8].minor);
  mqlyy_destructor(mqlyypParser,31,&mqlyymsp[-7].minor);
  mqlyy_destructor(mqlyypParser,34,&mqlyymsp[-6].minor);
  mqlyy_destructor(mqlyypParser,80,&mqlyymsp[-5].minor);
  mqlyy_destructor(mqlyypParser,18,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,19,&mqlyymsp[0].minor);
}
        break;
      case 223: /* get_objects_having_monads_in_statement ::= KEY_GET KEY_OBJECTS KEY_HAVING KEY_MONADS KEY_IN in_specification using_monad_feature KEY_OPEN_SQUARE_BRACKET object_type_name KEY_GET KEY_ALL KEY_CLOSE_SQUARE_BRACKET */
{  mqlyy_destructor(mqlyypParser,62,&mqlyymsp[-11].minor);
#line 983 "./mql.yxx"
{ mqlyymsp[-11].minor.mqlyy50 = new GetObjectsHavingMonadsInStatement((MQLExecEnv*) pEE, mqlyymsp[-6].minor.mqlyy108, mqlyymsp[-3].minor.mqlyy0->extractString(), mqlyymsp[-5].minor.mqlyy0->extractString(), true); deleteToken(mqlyymsp[-3].minor.mqlyy0); deleteToken(mqlyymsp[-5].minor.mqlyy0); }
#line 4050 "./mql.c"
  mqlyy_destructor(mqlyypParser,28,&mqlyymsp[-10].minor);
  mqlyy_destructor(mqlyypParser,31,&mqlyymsp[-9].minor);
  mqlyy_destructor(mqlyypParser,34,&mqlyymsp[-8].minor);
  mqlyy_destructor(mqlyypParser,80,&mqlyymsp[-7].minor);
  mqlyy_destructor(mqlyypParser,18,&mqlyymsp[-4].minor);
  mqlyy_destructor(mqlyypParser,62,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,69,&mqlyymsp[-1].minor);
  mqlyy_destructor(mqlyypParser,19,&mqlyymsp[0].minor);
}
        break;
      case 224: /* get_objects_having_monads_in_statement ::= KEY_GET KEY_OBJECTS KEY_HAVING KEY_MONADS KEY_IN in_specification using_monad_feature KEY_OPEN_SQUARE_BRACKET object_type_name KEY_GET feature_list KEY_CLOSE_SQUARE_BRACKET */
{  mqlyy_destructor(mqlyypParser,62,&mqlyymsp[-11].minor);
#line 991 "./mql.yxx"
{ mqlyymsp[-11].minor.mqlyy50 = new GetObjectsHavingMonadsInStatement((MQLExecEnv*) pEE, mqlyymsp[-6].minor.mqlyy108, mqlyymsp[-3].minor.mqlyy0->extractString(), mqlyymsp[-5].minor.mqlyy0->extractString(), mqlyymsp[-1].minor.mqlyy366); deleteToken(mqlyymsp[-3].minor.mqlyy0); deleteToken(mqlyymsp[-5].minor.mqlyy0); }
#line 4065 "./mql.c"
  mqlyy_destructor(mqlyypParser,28,&mqlyymsp[-10].minor);
  mqlyy_destructor(mqlyypParser,31,&mqlyymsp[-9].minor);
  mqlyy_destructor(mqlyypParser,34,&mqlyymsp[-8].minor);
  mqlyy_destructor(mqlyypParser,80,&mqlyymsp[-7].minor);
  mqlyy_destructor(mqlyypParser,18,&mqlyymsp[-4].minor);
  mqlyy_destructor(mqlyypParser,62,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,19,&mqlyymsp[0].minor);
}
        break;
      case 225: /* using_monad_feature ::= KEY_USING KEY_MONAD KEY_FEATURE IDENTIFIER */
{  mqlyy_destructor(mqlyypParser,10,&mqlyymsp[-3].minor);
#line 995 "./mql.yxx"
{ mqlyymsp[-3].minor.mqlyy0 = mqlyymsp[0].minor.mqlyy0; }
#line 4079 "./mql.c"
  mqlyy_destructor(mqlyypParser,30,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,84,&mqlyymsp[-1].minor);
}
        break;
      case 226: /* using_monad_feature ::= KEY_USING KEY_MONAD KEY_FEATURE KEY_MONADS */
{  mqlyy_destructor(mqlyypParser,10,&mqlyymsp[-3].minor);
#line 996 "./mql.yxx"
{ mqlyymsp[-3].minor.mqlyy0 = newToken(); mqlyymsp[-3].minor.mqlyy0->setString(new std::string("monads")); }
#line 4088 "./mql.c"
  mqlyy_destructor(mqlyypParser,30,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,84,&mqlyymsp[-1].minor);
  mqlyy_destructor(mqlyypParser,34,&mqlyymsp[0].minor);
}
        break;
      case 227: /* using_monad_feature ::= */
#line 997 "./mql.yxx"
{ mqlyymsp[1].minor.mqlyy0 = newToken(); mqlyymsp[1].minor.mqlyy0->setString(new std::string("monads")); }
#line 4097 "./mql.c"
        break;
      case 228: /* select_object_types_statement ::= KEY_SELECT opt_OBJECT KEY_TYPES */
{  mqlyy_destructor(mqlyypParser,67,&mqlyymsp[-2].minor);
#line 1004 "./mql.yxx"
{ mqlyymsp[-2].minor.mqlyy50 = new SelectObjectTypesStatement((MQLExecEnv*) pEE); }
#line 4103 "./mql.c"
  mqlyy_destructor(mqlyypParser,191,&mqlyymsp[-1].minor);
  mqlyy_destructor(mqlyypParser,21,&mqlyymsp[0].minor);
}
        break;
      case 229: /* select_features_statement ::= KEY_SELECT KEY_FEATURES KEY_FROM opt_OBJECTYPE KEY_OPEN_SQUARE_BRACKET object_type_name KEY_CLOSE_SQUARE_BRACKET */
{  mqlyy_destructor(mqlyypParser,67,&mqlyymsp[-6].minor);
#line 1011 "./mql.yxx"
{ mqlyymsp[-6].minor.mqlyy50 = new SelectFeaturesStatement((MQLExecEnv*) pEE, mqlyymsp[-1].minor.mqlyy0->extractString()); deleteToken(mqlyymsp[-1].minor.mqlyy0); }
#line 4112 "./mql.c"
  mqlyy_destructor(mqlyypParser,92,&mqlyymsp[-5].minor);
  mqlyy_destructor(mqlyypParser,51,&mqlyymsp[-4].minor);
  mqlyy_destructor(mqlyypParser,259,&mqlyymsp[-3].minor);
  mqlyy_destructor(mqlyypParser,18,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,19,&mqlyymsp[0].minor);
}
        break;
      case 230: /* get_set_from_feature_statement ::= KEY_GET KEY_SET KEY_FROM KEY_FEATURE feature_name KEY_OPEN_SQUARE_BRACKET object_type_name KEY_CLOSE_SQUARE_BRACKET */
{  mqlyy_destructor(mqlyypParser,62,&mqlyymsp[-7].minor);
#line 1018 "./mql.yxx"
{ mqlyymsp[-7].minor.mqlyy50 = new GetSetFromFeatureStatement((MQLExecEnv*) pEE, mqlyymsp[-1].minor.mqlyy0->extractString(), mqlyymsp[-3].minor.mqlyy0->extractString()); deleteToken(mqlyymsp[-1].minor.mqlyy0); deleteToken(mqlyymsp[-3].minor.mqlyy0); }
#line 4124 "./mql.c"
  mqlyy_destructor(mqlyypParser,46,&mqlyymsp[-6].minor);
  mqlyy_destructor(mqlyypParser,51,&mqlyymsp[-5].minor);
  mqlyy_destructor(mqlyypParser,84,&mqlyymsp[-4].minor);
  mqlyy_destructor(mqlyypParser,18,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,19,&mqlyymsp[0].minor);
}
        break;
      case 231: /* opt_OBJECTYPE ::= KEY_OBJECT KEY_TYPE */
{  mqlyy_destructor(mqlyypParser,17,&mqlyymsp[-1].minor);
#line 1023 "./mql.yxx"
{ mqlyymsp[-1].minor.mqlyy466=0; }
#line 4136 "./mql.c"
  mqlyy_destructor(mqlyypParser,20,&mqlyymsp[0].minor);
}
        break;
      case 232: /* opt_OBJECTYPE ::= KEY_TYPE */
{  mqlyy_destructor(mqlyypParser,20,&mqlyymsp[0].minor);
#line 1024 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy466=0; }
#line 4144 "./mql.c"
}
        break;
      case 233: /* opt_OBJECTYPE ::= */
#line 1025 "./mql.yxx"
{ mqlyymsp[1].minor.mqlyy466=0; }
#line 4150 "./mql.c"
        break;
      case 234: /* select_enumerations_statement ::= KEY_SELECT KEY_ENUMERATIONS */
{  mqlyy_destructor(mqlyypParser,67,&mqlyymsp[-1].minor);
#line 1031 "./mql.yxx"
{ mqlyymsp[-1].minor.mqlyy50 = new SelectEnumerationsStatement((MQLExecEnv*) pEE); }
#line 4156 "./mql.c"
  mqlyy_destructor(mqlyypParser,97,&mqlyymsp[0].minor);
}
        break;
      case 235: /* select_enumeration_constants_statement ::= KEY_SELECT choice_ENUM_ERATION KEY_CONSTANTS KEY_FROM opt_ENUM_ERATION enumeration_name */
{  mqlyy_destructor(mqlyypParser,67,&mqlyymsp[-5].minor);
#line 1040 "./mql.yxx"
{ mqlyymsp[-5].minor.mqlyy50 = new SelectEnumerationConstantsStatement((MQLExecEnv*) pEE, mqlyymsp[0].minor.mqlyy0->extractString()); deleteToken(mqlyymsp[0].minor.mqlyy0); }
#line 4164 "./mql.c"
  mqlyy_destructor(mqlyypParser,220,&mqlyymsp[-4].minor);
  mqlyy_destructor(mqlyypParser,98,&mqlyymsp[-3].minor);
  mqlyy_destructor(mqlyypParser,51,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,260,&mqlyymsp[-1].minor);
}
        break;
      case 236: /* opt_ENUM_ERATION ::= KEY_ENUM */
{  mqlyy_destructor(mqlyypParser,76,&mqlyymsp[0].minor);
#line 1045 "./mql.yxx"
{mqlyymsp[0].minor.mqlyy466=0;}
#line 4175 "./mql.c"
}
        break;
      case 237: /* opt_ENUM_ERATION ::= KEY_ENUMERATION */
{  mqlyy_destructor(mqlyypParser,75,&mqlyymsp[0].minor);
#line 1046 "./mql.yxx"
{mqlyymsp[0].minor.mqlyy466=0;}
#line 4182 "./mql.c"
}
        break;
      case 238: /* opt_ENUM_ERATION ::= */
#line 1047 "./mql.yxx"
{mqlyymsp[1].minor.mqlyy466=0;}
#line 4188 "./mql.c"
        break;
      case 239: /* select_object_types_which_use_enum_statement ::= KEY_SELECT opt_OBJECT KEY_TYPES KEY_USING choice_ENUM_ERATION enumeration_name */
{  mqlyy_destructor(mqlyypParser,67,&mqlyymsp[-5].minor);
#line 1056 "./mql.yxx"
{ mqlyymsp[-5].minor.mqlyy50 = new SelectObjectTypesUsingEnumerationStatement((MQLExecEnv*) pEE, mqlyymsp[0].minor.mqlyy0->extractString()); deleteToken(mqlyymsp[0].minor.mqlyy0); }
#line 4194 "./mql.c"
  mqlyy_destructor(mqlyypParser,191,&mqlyymsp[-4].minor);
  mqlyy_destructor(mqlyypParser,21,&mqlyymsp[-3].minor);
  mqlyy_destructor(mqlyypParser,10,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,220,&mqlyymsp[-1].minor);
}
        break;
      case 240: /* select_min_m_statement ::= KEY_SELECT KEY_MIN_M */
{  mqlyy_destructor(mqlyypParser,67,&mqlyymsp[-1].minor);
#line 1062 "./mql.yxx"
{ mqlyymsp[-1].minor.mqlyy50 = new SelectMinMStatement((MQLExecEnv*) pEE); }
#line 4205 "./mql.c"
  mqlyy_destructor(mqlyypParser,99,&mqlyymsp[0].minor);
}
        break;
      case 241: /* select_max_m_statement ::= KEY_SELECT KEY_MAX_M */
{  mqlyy_destructor(mqlyypParser,67,&mqlyymsp[-1].minor);
#line 1068 "./mql.yxx"
{ mqlyymsp[-1].minor.mqlyy50 = new SelectMaxMStatement((MQLExecEnv*) pEE); }
#line 4213 "./mql.c"
  mqlyy_destructor(mqlyypParser,83,&mqlyymsp[0].minor);
}
        break;
      case 242: /* create_object_from_monads_statement ::= KEY_CREATE KEY_OBJECT KEY_FROM monad_specification with_id_d_specification object_creation_specification */
{  mqlyy_destructor(mqlyypParser,2,&mqlyymsp[-5].minor);
#line 1077 "./mql.yxx"
{ mqlyymsp[-5].minor.mqlyy50 = new CreateObjectFromMonadsStatement((MQLExecEnv*) pEE, mqlyymsp[-2].minor.mqlyy108, mqlyymsp[-1].minor.mqlyy282, mqlyymsp[0].minor.mqlyy275); }
#line 4221 "./mql.c"
  mqlyy_destructor(mqlyypParser,17,&mqlyymsp[-4].minor);
  mqlyy_destructor(mqlyypParser,51,&mqlyymsp[-3].minor);
}
        break;
      case 243: /* monad_specification ::= KEY_MONADS KEY_EQUALS monad_set */
{  mqlyy_destructor(mqlyypParser,34,&mqlyymsp[-2].minor);
#line 1082 "./mql.yxx"
{ mqlyymsp[-2].minor.mqlyy108 = mqlyymsp[0].minor.mqlyy108; mqlyymsp[-2].minor.mqlyy108->setIsFirst(true); }
#line 4230 "./mql.c"
  mqlyy_destructor(mqlyypParser,72,&mqlyymsp[-1].minor);
}
        break;
      case 244: /* with_id_d_specification ::= KEY_WITH KEY_ID_D KEY_EQUALS id_d_const */
{  mqlyy_destructor(mqlyypParser,8,&mqlyymsp[-3].minor);
#line 1087 "./mql.yxx"
{ mqlyymsp[-3].minor.mqlyy282 = mqlyymsp[0].minor.mqlyy282; }
#line 4238 "./mql.c"
  mqlyy_destructor(mqlyypParser,45,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,72,&mqlyymsp[-1].minor);
}
        break;
      case 245: /* with_id_d_specification ::= */
#line 1089 "./mql.yxx"
{ mqlyymsp[1].minor.mqlyy282 = NIL; }
#line 4246 "./mql.c"
        break;
      case 246: /* id_d_const ::= INTEGER */
#line 1095 "./mql.yxx"
{ mqlyylhsminor.mqlyy282 = mqlyymsp[0].minor.mqlyy0->integer; deleteToken(mqlyymsp[0].minor.mqlyy0); }
#line 4251 "./mql.c"
  mqlyymsp[0].minor.mqlyy282 = mqlyylhsminor.mqlyy282;
        break;
      case 247: /* id_d_const ::= KEY_NIL */
{  mqlyy_destructor(mqlyypParser,78,&mqlyymsp[0].minor);
#line 1097 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy282 = NIL; }
#line 4258 "./mql.c"
}
        break;
      case 248: /* object_creation_specification ::= KEY_OPEN_SQUARE_BRACKET object_type_name opt_list_of_feature_assignments KEY_CLOSE_SQUARE_BRACKET */
      case 281: /* object_update_specification ::= KEY_OPEN_SQUARE_BRACKET object_type_name list_of_feature_assignments KEY_CLOSE_SQUARE_BRACKET */ mqlyytestcase(mqlyyruleno==281);
{  mqlyy_destructor(mqlyypParser,18,&mqlyymsp[-3].minor);
#line 1105 "./mql.yxx"
{ mqlyymsp[-3].minor.mqlyy275 = new ObjectSpec(mqlyymsp[-2].minor.mqlyy0->extractString(), mqlyymsp[-1].minor.mqlyy265); deleteToken(mqlyymsp[-2].minor.mqlyy0); }
#line 4266 "./mql.c"
  mqlyy_destructor(mqlyypParser,19,&mqlyymsp[0].minor);
}
        break;
      case 249: /* opt_list_of_feature_assignments ::= list_of_feature_assignments */
      case 251: /* list_of_feature_assignments ::= feature_assignment */ mqlyytestcase(mqlyyruleno==251);
#line 1111 "./mql.yxx"
{ mqlyylhsminor.mqlyy265 = mqlyymsp[0].minor.mqlyy265; }
#line 4274 "./mql.c"
  mqlyymsp[0].minor.mqlyy265 = mqlyylhsminor.mqlyy265;
        break;
      case 250: /* opt_list_of_feature_assignments ::= */
#line 1113 "./mql.yxx"
{ mqlyymsp[1].minor.mqlyy265 = 0; }
#line 4280 "./mql.c"
        break;
      case 252: /* list_of_feature_assignments ::= list_of_feature_assignments feature_assignment */
#line 1122 "./mql.yxx"
{ mqlyylhsminor.mqlyy265 = mqlyymsp[0].minor.mqlyy265; mqlyylhsminor.mqlyy265->setNext(mqlyymsp[-1].minor.mqlyy265); }
#line 4285 "./mql.c"
  mqlyymsp[-1].minor.mqlyy265 = mqlyylhsminor.mqlyy265;
        break;
      case 253: /* feature_assignment ::= feature_name KEY_ASSIGN expression KEY_SEMICOLON */
#line 1128 "./mql.yxx"
{ mqlyylhsminor.mqlyy265 = new FeatureAssignment(mqlyymsp[-3].minor.mqlyy0->extractString(), mqlyymsp[-1].minor.mqlyy413, 0); deleteToken(mqlyymsp[-3].minor.mqlyy0); }
#line 4291 "./mql.c"
  mqlyy_destructor(mqlyypParser,100,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,39,&mqlyymsp[0].minor);
  mqlyymsp[-3].minor.mqlyy265 = mqlyylhsminor.mqlyy265;
        break;
      case 255: /* feature_name ::= KEY_MONADS */
{  mqlyy_destructor(mqlyypParser,34,&mqlyymsp[0].minor);
#line 1134 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy0 = newToken(); mqlyymsp[0].minor.mqlyy0->setString(new std::string("monads")); }
#line 4300 "./mql.c"
}
        break;
      case 256: /* expression ::= signed_integer */
#line 1139 "./mql.yxx"
{ mqlyylhsminor.mqlyy413 = new Expression(mqlyymsp[0].minor.mqlyy503); }
#line 4306 "./mql.c"
  mqlyymsp[0].minor.mqlyy413 = mqlyylhsminor.mqlyy413;
        break;
      case 257: /* expression ::= STRING */
#line 1141 "./mql.yxx"
{ mqlyylhsminor.mqlyy413 = new Expression(mqlyymsp[0].minor.mqlyy0->extractString(), kExprString); deleteToken(mqlyymsp[0].minor.mqlyy0); }
#line 4312 "./mql.c"
  mqlyymsp[0].minor.mqlyy413 = mqlyylhsminor.mqlyy413;
        break;
      case 258: /* expression ::= IDENTIFIER */
#line 1143 "./mql.yxx"
{ mqlyylhsminor.mqlyy413 = new Expression(mqlyymsp[0].minor.mqlyy0->extractString(), kExprIdentifier); deleteToken(mqlyymsp[0].minor.mqlyy0); }
#line 4318 "./mql.c"
  mqlyymsp[0].minor.mqlyy413 = mqlyylhsminor.mqlyy413;
        break;
      case 259: /* expression ::= monad_set */
#line 1145 "./mql.yxx"
{ mqlyylhsminor.mqlyy413 = new Expression(mqlyymsp[0].minor.mqlyy108); }
#line 4324 "./mql.c"
  mqlyymsp[0].minor.mqlyy413 = mqlyylhsminor.mqlyy413;
        break;
      case 260: /* expression ::= KEY_OPEN_BRACKET KEY_CLOSE_BRACKET */
{  mqlyy_destructor(mqlyypParser,52,&mqlyymsp[-1].minor);
#line 1147 "./mql.yxx"
{ mqlyymsp[-1].minor.mqlyy413 = new Expression(); /* empty list */ }
#line 4331 "./mql.c"
  mqlyy_destructor(mqlyypParser,54,&mqlyymsp[0].minor);
}
        break;
      case 261: /* expression ::= KEY_OPEN_BRACKET list_of_integer KEY_CLOSE_BRACKET */
{  mqlyy_destructor(mqlyypParser,52,&mqlyymsp[-2].minor);
#line 1149 "./mql.yxx"
{ mqlyymsp[-2].minor.mqlyy413 = new Expression(new IntegerList(mqlyymsp[-1].minor.mqlyy17)); delete mqlyymsp[-1].minor.mqlyy17; }
#line 4339 "./mql.c"
  mqlyy_destructor(mqlyypParser,54,&mqlyymsp[0].minor);
}
        break;
      case 262: /* expression ::= KEY_OPEN_BRACKET list_of_identifier KEY_CLOSE_BRACKET */
{  mqlyy_destructor(mqlyypParser,52,&mqlyymsp[-2].minor);
#line 1151 "./mql.yxx"
{ mqlyymsp[-2].minor.mqlyy413 = new Expression(new StringList(mqlyymsp[-1].minor.mqlyy336), kExprListOfIdentifier); delete mqlyymsp[-1].minor.mqlyy336; }
#line 4347 "./mql.c"
  mqlyy_destructor(mqlyypParser,54,&mqlyymsp[0].minor);
}
        break;
      case 263: /* list_of_integer ::= signed_integer */
#line 1157 "./mql.yxx"
{ mqlyylhsminor.mqlyy17 = new IntegerListNode(mqlyymsp[0].minor.mqlyy503, 0); }
#line 4354 "./mql.c"
  mqlyymsp[0].minor.mqlyy17 = mqlyylhsminor.mqlyy17;
        break;
      case 264: /* list_of_integer ::= list_of_integer KEY_COMMA signed_integer */
#line 1160 "./mql.yxx"
{ mqlyylhsminor.mqlyy17 = new IntegerListNode(mqlyymsp[0].minor.mqlyy503, mqlyymsp[-2].minor.mqlyy17); mqlyymsp[-2].minor.mqlyy17->setNext(mqlyylhsminor.mqlyy17); }
#line 4360 "./mql.c"
  mqlyy_destructor(mqlyypParser,71,&mqlyymsp[-1].minor);
  mqlyymsp[-2].minor.mqlyy17 = mqlyylhsminor.mqlyy17;
        break;
      case 267: /* create_object_from_id_ds_statement ::= KEY_CREATE KEY_OBJECT KEY_FROM id_ds_specification with_id_d_specification object_creation_specification */
{  mqlyy_destructor(mqlyypParser,2,&mqlyymsp[-5].minor);
#line 1178 "./mql.yxx"
{ mqlyymsp[-5].minor.mqlyy50 = new CreateObjectFromID_DsStatement((MQLExecEnv*) pEE, mqlyymsp[-2].minor.mqlyy43, mqlyymsp[-1].minor.mqlyy282, mqlyymsp[0].minor.mqlyy275); }
#line 4368 "./mql.c"
  mqlyy_destructor(mqlyypParser,17,&mqlyymsp[-4].minor);
  mqlyy_destructor(mqlyypParser,51,&mqlyymsp[-3].minor);
}
        break;
      case 268: /* id_ds_specification ::= choice_number_ID_DS KEY_EQUALS id_d_list */
{  mqlyy_destructor(mqlyypParser,267,&mqlyymsp[-2].minor);
#line 1184 "./mql.yxx"
{ mqlyymsp[-2].minor.mqlyy43 = mqlyymsp[0].minor.mqlyy43; }
#line 4377 "./mql.c"
  mqlyy_destructor(mqlyypParser,72,&mqlyymsp[-1].minor);
}
        break;
      case 269: /* choice_number_ID_DS ::= KEY_ID_D */
{  mqlyy_destructor(mqlyypParser,45,&mqlyymsp[0].minor);
#line 1189 "./mql.yxx"
{mqlyymsp[0].minor.mqlyy466=0;}
#line 4385 "./mql.c"
}
        break;
      case 270: /* choice_number_ID_DS ::= KEY_ID_DS */
{  mqlyy_destructor(mqlyypParser,101,&mqlyymsp[0].minor);
#line 1190 "./mql.yxx"
{mqlyymsp[0].minor.mqlyy466=0;}
#line 4392 "./mql.c"
}
        break;
      case 271: /* id_d_list ::= id_d */
#line 1196 "./mql.yxx"
{ mqlyylhsminor.mqlyy43 = mqlyymsp[0].minor.mqlyy43; }
#line 4398 "./mql.c"
  mqlyymsp[0].minor.mqlyy43 = mqlyylhsminor.mqlyy43;
        break;
      case 272: /* id_d_list ::= id_d_list KEY_COMMA id_d */
#line 1198 "./mql.yxx"
{ mqlyylhsminor.mqlyy43 = mqlyymsp[0].minor.mqlyy43; mqlyylhsminor.mqlyy43->setNext(mqlyymsp[-2].minor.mqlyy43); }
#line 4404 "./mql.c"
  mqlyy_destructor(mqlyypParser,71,&mqlyymsp[-1].minor);
  mqlyymsp[-2].minor.mqlyy43 = mqlyylhsminor.mqlyy43;
        break;
      case 273: /* id_d ::= id_d_const */
#line 1203 "./mql.yxx"
{ mqlyylhsminor.mqlyy43 = new ID_D(mqlyymsp[0].minor.mqlyy282, 0); }
#line 4411 "./mql.c"
  mqlyymsp[0].minor.mqlyy43 = mqlyylhsminor.mqlyy43;
        break;
      case 274: /* create_objects_statement ::= KEY_CREATE KEY_OBJECTS KEY_WITH KEY_OBJECT KEY_TYPE KEY_OPEN_SQUARE_BRACKET object_type_name KEY_CLOSE_SQUARE_BRACKET object_creation_list */
{  mqlyy_destructor(mqlyypParser,2,&mqlyymsp[-8].minor);
#line 1215 "./mql.yxx"
{ mqlyymsp[-8].minor.mqlyy50 = new CreateObjectsStatement((MQLExecEnv*) pEE, mqlyymsp[-2].minor.mqlyy0->extractString(), mqlyymsp[0].minor.mqlyy481); deleteToken(mqlyymsp[-2].minor.mqlyy0); }
#line 4418 "./mql.c"
  mqlyy_destructor(mqlyypParser,28,&mqlyymsp[-7].minor);
  mqlyy_destructor(mqlyypParser,8,&mqlyymsp[-6].minor);
  mqlyy_destructor(mqlyypParser,17,&mqlyymsp[-5].minor);
  mqlyy_destructor(mqlyypParser,20,&mqlyymsp[-4].minor);
  mqlyy_destructor(mqlyypParser,18,&mqlyymsp[-3].minor);
  mqlyy_destructor(mqlyypParser,19,&mqlyymsp[-1].minor);
}
        break;
      case 275: /* object_creation_list ::= object_creation_no_object_type */
#line 1222 "./mql.yxx"
{ mqlyylhsminor.mqlyy481 = mqlyymsp[0].minor.mqlyy481; }
#line 4430 "./mql.c"
  mqlyymsp[0].minor.mqlyy481 = mqlyylhsminor.mqlyy481;
        break;
      case 276: /* object_creation_list ::= object_creation_list object_creation_no_object_type */
#line 1225 "./mql.yxx"
{ mqlyylhsminor.mqlyy481 = mqlyymsp[0].minor.mqlyy481; mqlyylhsminor.mqlyy481->setNext(mqlyymsp[-1].minor.mqlyy481); }
#line 4436 "./mql.c"
  mqlyymsp[-1].minor.mqlyy481 = mqlyylhsminor.mqlyy481;
        break;
      case 277: /* object_creation_no_object_type ::= KEY_CREATE KEY_OBJECT KEY_FROM monad_specification with_id_d_specification KEY_OPEN_SQUARE_BRACKET opt_list_of_feature_assignments KEY_CLOSE_SQUARE_BRACKET */
{  mqlyy_destructor(mqlyypParser,2,&mqlyymsp[-7].minor);
#line 1237 "./mql.yxx"
{ mqlyymsp[-7].minor.mqlyy481 = new ObjectSpecNoOT(mqlyymsp[-4].minor.mqlyy108, mqlyymsp[-3].minor.mqlyy282, mqlyymsp[-1].minor.mqlyy265, 0); }
#line 4443 "./mql.c"
  mqlyy_destructor(mqlyypParser,17,&mqlyymsp[-6].minor);
  mqlyy_destructor(mqlyypParser,51,&mqlyymsp[-5].minor);
  mqlyy_destructor(mqlyypParser,18,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,19,&mqlyymsp[0].minor);
}
        break;
      case 278: /* update_objects_by_monads_statement ::= KEY_UPDATE choice_number_OBJECTS KEY_BY monad_specification object_update_specification */
{  mqlyy_destructor(mqlyypParser,57,&mqlyymsp[-4].minor);
#line 1246 "./mql.yxx"
{ mqlyymsp[-4].minor.mqlyy50 = new UpdateObjectsByMonadsStatement((MQLExecEnv*) pEE, mqlyymsp[-1].minor.mqlyy108, mqlyymsp[0].minor.mqlyy275); }
#line 4454 "./mql.c"
  mqlyy_destructor(mqlyypParser,212,&mqlyymsp[-3].minor);
  mqlyy_destructor(mqlyypParser,102,&mqlyymsp[-2].minor);
}
        break;
      case 279: /* choice_number_OBJECTS ::= KEY_OBJECT */
{  mqlyy_destructor(mqlyypParser,17,&mqlyymsp[0].minor);
#line 1251 "./mql.yxx"
{mqlyymsp[0].minor.mqlyy466=0;}
#line 4463 "./mql.c"
}
        break;
      case 280: /* choice_number_OBJECTS ::= KEY_OBJECTS */
{  mqlyy_destructor(mqlyypParser,28,&mqlyymsp[0].minor);
#line 1252 "./mql.yxx"
{mqlyymsp[0].minor.mqlyy466=0;}
#line 4470 "./mql.c"
}
        break;
      case 282: /* update_objects_by_id_ds_statement ::= KEY_UPDATE choice_number_OBJECTS KEY_BY id_ds_specification object_update_specification */
{  mqlyy_destructor(mqlyypParser,57,&mqlyymsp[-4].minor);
#line 1272 "./mql.yxx"
{ mqlyymsp[-4].minor.mqlyy50 = new UpdateObjectsByID_DsStatement((MQLExecEnv*) pEE, mqlyymsp[-1].minor.mqlyy43, mqlyymsp[0].minor.mqlyy275); }
#line 4477 "./mql.c"
  mqlyy_destructor(mqlyypParser,212,&mqlyymsp[-3].minor);
  mqlyy_destructor(mqlyypParser,102,&mqlyymsp[-2].minor);
}
        break;
      case 283: /* delete_objects_by_monads_statement ::= KEY_DELETE choice_number_OBJECTS KEY_BY monad_specification object_deletion_specification */
{  mqlyy_destructor(mqlyypParser,61,&mqlyymsp[-4].minor);
#line 1282 "./mql.yxx"
{ mqlyymsp[-4].minor.mqlyy50 = new DeleteObjectsByMonadsStatement((MQLExecEnv*) pEE, mqlyymsp[-1].minor.mqlyy108, *(mqlyymsp[0].minor.mqlyy0->pString)); deleteToken(mqlyymsp[0].minor.mqlyy0); }
#line 4486 "./mql.c"
  mqlyy_destructor(mqlyypParser,212,&mqlyymsp[-3].minor);
  mqlyy_destructor(mqlyypParser,102,&mqlyymsp[-2].minor);
}
        break;
      case 284: /* object_deletion_specification ::= KEY_OPEN_SQUARE_BRACKET object_type_name_to_delete KEY_CLOSE_SQUARE_BRACKET */
{  mqlyy_destructor(mqlyypParser,18,&mqlyymsp[-2].minor);
#line 1291 "./mql.yxx"
{ mqlyymsp[-2].minor.mqlyy0 = mqlyymsp[-1].minor.mqlyy0; }
#line 4495 "./mql.c"
  mqlyy_destructor(mqlyypParser,19,&mqlyymsp[0].minor);
}
        break;
      case 286: /* delete_objects_by_id_ds_statement ::= KEY_DELETE choice_number_OBJECTS KEY_BY id_ds_specification object_deletion_specification */
{  mqlyy_destructor(mqlyypParser,61,&mqlyymsp[-4].minor);
#line 1307 "./mql.yxx"
{ mqlyymsp[-4].minor.mqlyy50 = new DeleteObjectsByID_DsStatement((MQLExecEnv*) pEE, mqlyymsp[-1].minor.mqlyy43, *(mqlyymsp[0].minor.mqlyy0->pString)); deleteToken(mqlyymsp[0].minor.mqlyy0); }
#line 4503 "./mql.c"
  mqlyy_destructor(mqlyypParser,212,&mqlyymsp[-3].minor);
  mqlyy_destructor(mqlyypParser,102,&mqlyymsp[-2].minor);
}
        break;
      case 287: /* get_features_statement ::= KEY_GET choice_number_FEATURES feature_list KEY_FROM choice_number_OBJECTS KEY_WITH id_ds_specification KEY_OPEN_SQUARE_BRACKET object_type_name KEY_CLOSE_SQUARE_BRACKET */
{  mqlyy_destructor(mqlyypParser,62,&mqlyymsp[-9].minor);
#line 1319 "./mql.yxx"
{ mqlyymsp[-9].minor.mqlyy50 = new GetFeaturesStatement((MQLExecEnv*) pEE, mqlyymsp[-7].minor.mqlyy366, mqlyymsp[-3].minor.mqlyy43, mqlyymsp[-1].minor.mqlyy0->extractString()); deleteToken(mqlyymsp[-1].minor.mqlyy0); }
#line 4512 "./mql.c"
  mqlyy_destructor(mqlyypParser,275,&mqlyymsp[-8].minor);
  mqlyy_destructor(mqlyypParser,51,&mqlyymsp[-6].minor);
  mqlyy_destructor(mqlyypParser,212,&mqlyymsp[-5].minor);
  mqlyy_destructor(mqlyypParser,8,&mqlyymsp[-4].minor);
  mqlyy_destructor(mqlyypParser,18,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,19,&mqlyymsp[0].minor);
}
        break;
      case 288: /* choice_number_FEATURES ::= KEY_FEATURE */
{  mqlyy_destructor(mqlyypParser,84,&mqlyymsp[0].minor);
#line 1324 "./mql.yxx"
{mqlyymsp[0].minor.mqlyy466=0;}
#line 4525 "./mql.c"
}
        break;
      case 289: /* choice_number_FEATURES ::= KEY_FEATURES */
{  mqlyy_destructor(mqlyypParser,92,&mqlyymsp[0].minor);
#line 1325 "./mql.yxx"
{mqlyymsp[0].minor.mqlyy466=0;}
#line 4532 "./mql.c"
}
        break;
      case 290: /* feature_list ::= feature_name */
#line 1330 "./mql.yxx"
{ mqlyylhsminor.mqlyy366 = new GrammarFeature(mqlyymsp[0].minor.mqlyy0->extractString(), 0); deleteToken(mqlyymsp[0].minor.mqlyy0); }
#line 4538 "./mql.c"
  mqlyymsp[0].minor.mqlyy366 = mqlyylhsminor.mqlyy366;
        break;
      case 291: /* feature_list ::= feature_list KEY_COMMA feature_name */
#line 1332 "./mql.yxx"
{ mqlyylhsminor.mqlyy366 = new GrammarFeature(mqlyymsp[0].minor.mqlyy0->extractString(), mqlyymsp[-2].minor.mqlyy366); deleteToken(mqlyymsp[0].minor.mqlyy0); }
#line 4544 "./mql.c"
  mqlyy_destructor(mqlyypParser,71,&mqlyymsp[-1].minor);
  mqlyymsp[-2].minor.mqlyy366 = mqlyylhsminor.mqlyy366;
        break;
      case 292: /* quit_statement ::= KEY_QUIT */
{  mqlyy_destructor(mqlyypParser,103,&mqlyymsp[0].minor);
#line 1338 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy50 = new QuitStatement((MQLExecEnv*) pEE); }
#line 4552 "./mql.c"
}
        break;
      case 293: /* mql_query ::= topograph */
#line 1344 "./mql.yxx"
{ mqlyylhsminor.mqlyy269 = mqlyymsp[0].minor.mqlyy269; }
#line 4558 "./mql.c"
  mqlyymsp[0].minor.mqlyy269 = mqlyylhsminor.mqlyy269;
        break;
      case 294: /* topograph ::= blocks */
#line 1350 "./mql.yxx"
{ mqlyylhsminor.mqlyy269 = new Topograph(mqlyymsp[0].minor.mqlyy18); }
#line 4564 "./mql.c"
  mqlyymsp[0].minor.mqlyy269 = mqlyylhsminor.mqlyy269;
        break;
      case 295: /* blocks ::= using_range_clause block_string */
#line 1356 "./mql.yxx"
{ mqlyylhsminor.mqlyy18 = new Blocks(mqlyymsp[0].minor.mqlyy501, mqlyymsp[-1].minor.mqlyy600); }
#line 4570 "./mql.c"
  mqlyymsp[-1].minor.mqlyy18 = mqlyylhsminor.mqlyy18;
        break;
      case 296: /* blocks ::= using_range_clause unordered_group */
#line 1358 "./mql.yxx"
{ mqlyylhsminor.mqlyy18 = new Blocks(mqlyymsp[0].minor.mqlyy24, mqlyymsp[-1].minor.mqlyy600); }
#line 4576 "./mql.c"
  mqlyymsp[-1].minor.mqlyy18 = mqlyylhsminor.mqlyy18;
        break;
      case 297: /* block_string0 ::= block */
#line 1364 "./mql.yxx"
{ mqlyylhsminor.mqlyy63 = new BlockString0(mqlyymsp[0].minor.mqlyy218); }
#line 4582 "./mql.c"
  mqlyymsp[0].minor.mqlyy63 = mqlyylhsminor.mqlyy63;
        break;
      case 298: /* block_string0 ::= KEY_OPEN_SQUARE_BRACKET block_string KEY_CLOSE_SQUARE_BRACKET */
{  mqlyy_destructor(mqlyypParser,18,&mqlyymsp[-2].minor);
#line 1366 "./mql.yxx"
{ mqlyymsp[-2].minor.mqlyy63 = new BlockString0(mqlyymsp[-1].minor.mqlyy501); }
#line 4589 "./mql.c"
  mqlyy_destructor(mqlyypParser,19,&mqlyymsp[0].minor);
}
        break;
      case 299: /* block_string1 ::= block_string0 */
#line 1371 "./mql.yxx"
{ mqlyylhsminor.mqlyy116 = new BlockString1(mqlyymsp[0].minor.mqlyy63); }
#line 4596 "./mql.c"
  mqlyymsp[0].minor.mqlyy116 = mqlyylhsminor.mqlyy116;
        break;
      case 300: /* block_string1 ::= block_string0 KEY_STAR star_monad_set */
#line 1373 "./mql.yxx"
{ mqlyylhsminor.mqlyy116 = new BlockString1(mqlyymsp[-2].minor.mqlyy63, mqlyymsp[0].minor.mqlyy108); }
#line 4602 "./mql.c"
  mqlyy_destructor(mqlyypParser,96,&mqlyymsp[-1].minor);
  mqlyymsp[-2].minor.mqlyy116 = mqlyylhsminor.mqlyy116;
        break;
      case 301: /* block_string2 ::= block_string1 */
#line 1378 "./mql.yxx"
{ mqlyylhsminor.mqlyy169 = new BlockString2(mqlyymsp[0].minor.mqlyy116); }
#line 4609 "./mql.c"
  mqlyymsp[0].minor.mqlyy169 = mqlyylhsminor.mqlyy169;
        break;
      case 302: /* block_string2 ::= block_string1 block_string2 */
#line 1380 "./mql.yxx"
{ mqlyylhsminor.mqlyy169 = new BlockString2(mqlyymsp[-1].minor.mqlyy116, mqlyymsp[0].minor.mqlyy169, false); }
#line 4615 "./mql.c"
  mqlyymsp[-1].minor.mqlyy169 = mqlyylhsminor.mqlyy169;
        break;
      case 303: /* block_string2 ::= block_string1 KEY_EXCLAMATION block_string2 */
#line 1383 "./mql.yxx"
{ mqlyylhsminor.mqlyy169 = new BlockString2(mqlyymsp[-2].minor.mqlyy116, mqlyymsp[0].minor.mqlyy169, true); }
#line 4621 "./mql.c"
  mqlyy_destructor(mqlyypParser,104,&mqlyymsp[-1].minor);
  mqlyymsp[-2].minor.mqlyy169 = mqlyylhsminor.mqlyy169;
        break;
      case 304: /* block_string ::= block_string2 */
#line 1388 "./mql.yxx"
{ mqlyylhsminor.mqlyy501 = new BlockString(mqlyymsp[0].minor.mqlyy169); }
#line 4628 "./mql.c"
  mqlyymsp[0].minor.mqlyy501 = mqlyylhsminor.mqlyy501;
        break;
      case 305: /* block_string ::= block_string2 KEY_OR block_string */
#line 1390 "./mql.yxx"
{ mqlyylhsminor.mqlyy501 = new BlockString(mqlyymsp[-2].minor.mqlyy169, mqlyymsp[0].minor.mqlyy501); }
#line 4634 "./mql.c"
  mqlyy_destructor(mqlyypParser,105,&mqlyymsp[-1].minor);
  mqlyymsp[-2].minor.mqlyy501 = mqlyylhsminor.mqlyy501;
        break;
      case 306: /* notexist ::= KEY_NOTEXIST */
{  mqlyy_destructor(mqlyypParser,106,&mqlyymsp[0].minor);
#line 1395 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy466 = 0; }
#line 4642 "./mql.c"
}
        break;
      case 307: /* notexist ::= KEY_NOTEXISTS */
{  mqlyy_destructor(mqlyypParser,107,&mqlyymsp[0].minor);
#line 1396 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy466 = 0; }
#line 4649 "./mql.c"
}
        break;
      case 308: /* object_reference_declaration ::= */
      case 310: /* mark_declaration ::= */ mqlyytestcase(mqlyyruleno==310);
#line 1401 "./mql.yxx"
{ mqlyymsp[1].minor.mqlyy0 = newToken(); mqlyymsp[1].minor.mqlyy0->setString(new std::string()); }
#line 4656 "./mql.c"
        break;
      case 309: /* object_reference_declaration ::= KEY_AS object_reference */
{  mqlyy_destructor(mqlyypParser,108,&mqlyymsp[-1].minor);
#line 1402 "./mql.yxx"
{ mqlyymsp[-1].minor.mqlyy0 = mqlyymsp[0].minor.mqlyy0; }
#line 4662 "./mql.c"
}
        break;
      case 313: /* retrieval ::= */
#line 1417 "./mql.yxx"
{ mqlyymsp[1].minor.mqlyy88 = kRetrieve; }
#line 4668 "./mql.c"
        break;
      case 314: /* retrieval ::= KEY_NORETRIEVE */
      case 371: /* gap_retrieval ::= KEY_NORETRIEVE */ mqlyytestcase(mqlyyruleno==371);
{  mqlyy_destructor(mqlyypParser,110,&mqlyymsp[0].minor);
#line 1418 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy88 = kNoRetrieve; }
#line 4675 "./mql.c"
}
        break;
      case 315: /* retrieval ::= KEY_RETRIEVE */
      case 372: /* gap_retrieval ::= KEY_RETRIEVE */ mqlyytestcase(mqlyyruleno==372);
{  mqlyy_destructor(mqlyypParser,111,&mqlyymsp[0].minor);
#line 1419 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy88 = kRetrieve; }
#line 4683 "./mql.c"
}
        break;
      case 316: /* retrieval ::= KEY_FOCUS */
      case 373: /* gap_retrieval ::= KEY_FOCUS */ mqlyytestcase(mqlyyruleno==373);
{  mqlyy_destructor(mqlyypParser,81,&mqlyymsp[0].minor);
#line 1420 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy88 = kRetrieveFocus; }
#line 4691 "./mql.c"
}
        break;
      case 317: /* firstlast ::= */
#line 1424 "./mql.yxx"
{ mqlyymsp[1].minor.mqlyy296 = kNoFirstLast; }
#line 4697 "./mql.c"
        break;
      case 318: /* firstlast ::= KEY_FIRST */
{  mqlyy_destructor(mqlyypParser,33,&mqlyymsp[0].minor);
#line 1425 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy296 = kFirst; }
#line 4703 "./mql.c"
}
        break;
      case 319: /* firstlast ::= KEY_LAST */
{  mqlyy_destructor(mqlyypParser,36,&mqlyymsp[0].minor);
#line 1426 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy296 = kLast; }
#line 4710 "./mql.c"
}
        break;
      case 320: /* firstlast ::= KEY_FIRST KEY_AND KEY_LAST */
{  mqlyy_destructor(mqlyypParser,33,&mqlyymsp[-2].minor);
#line 1427 "./mql.yxx"
{ mqlyymsp[-2].minor.mqlyy296 = kFirstAndLast; }
#line 4717 "./mql.c"
  mqlyy_destructor(mqlyypParser,35,&mqlyymsp[-1].minor);
  mqlyy_destructor(mqlyypParser,36,&mqlyymsp[0].minor);
}
        break;
      case 321: /* monad_set_relation_clause ::= monad_set_relation_operation KEY_OPEN_BRACKET monad_set_name KEY_COMMA universe_or_substrate KEY_CLOSE_BRACKET */
#line 1432 "./mql.yxx"
{ mqlyylhsminor.mqlyy299 = new MonadSetRelationClause(mqlyymsp[-5].minor.mqlyy360, mqlyymsp[-3].minor.mqlyy0->extractString(), mqlyymsp[-1].minor.mqlyy569); deleteToken(mqlyymsp[-3].minor.mqlyy0); }
#line 4725 "./mql.c"
  mqlyy_destructor(mqlyypParser,52,&mqlyymsp[-4].minor);
  mqlyy_destructor(mqlyypParser,71,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,54,&mqlyymsp[0].minor);
  mqlyymsp[-5].minor.mqlyy299 = mqlyylhsminor.mqlyy299;
        break;
      case 322: /* monad_set_relation_clause ::= monad_set_relation_operation KEY_OPEN_BRACKET universe_or_substrate KEY_CLOSE_BRACKET */
#line 1434 "./mql.yxx"
{ mqlyylhsminor.mqlyy299 = new MonadSetRelationClause(mqlyymsp[-3].minor.mqlyy360, new std::string("monads"), mqlyymsp[-1].minor.mqlyy569); }
#line 4734 "./mql.c"
  mqlyy_destructor(mqlyypParser,52,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,54,&mqlyymsp[0].minor);
  mqlyymsp[-3].minor.mqlyy299 = mqlyylhsminor.mqlyy299;
        break;
      case 323: /* monad_set_relation_clause ::= */
#line 1436 "./mql.yxx"
{ mqlyymsp[1].minor.mqlyy299 = new MonadSetRelationClause(kMSROPartOf, new std::string("monads"), kMSNSubstrate); }
#line 4742 "./mql.c"
        break;
      case 324: /* monad_set_relation_operation ::= KEY_PART_OF */
{  mqlyy_destructor(mqlyypParser,112,&mqlyymsp[0].minor);
#line 1441 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy360 = kMSROPartOf; }
#line 4748 "./mql.c"
}
        break;
      case 325: /* monad_set_relation_operation ::= KEY_STARTS_IN */
{  mqlyy_destructor(mqlyypParser,113,&mqlyymsp[0].minor);
#line 1443 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy360 = kMSROStartsIn; }
#line 4755 "./mql.c"
}
        break;
      case 326: /* monad_set_relation_operation ::= KEY_OVERLAP */
{  mqlyy_destructor(mqlyypParser,114,&mqlyymsp[0].minor);
#line 1445 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy360 = kMSROOverlap; }
#line 4762 "./mql.c"
}
        break;
      case 327: /* universe_or_substrate ::= KEY_UNIVERSE */
{  mqlyy_destructor(mqlyypParser,115,&mqlyymsp[0].minor);
#line 1451 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy569 = kMSNUniverse; }
#line 4769 "./mql.c"
}
        break;
      case 328: /* universe_or_substrate ::= KEY_SUBSTRATE */
{  mqlyy_destructor(mqlyypParser,116,&mqlyymsp[0].minor);
#line 1453 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy569 = kMSNSubstrate; }
#line 4776 "./mql.c"
}
        break;
      case 329: /* feature_constraints ::= */
#line 1458 "./mql.yxx"
{ mqlyymsp[1].minor.mqlyy520 = 0; }
#line 4782 "./mql.c"
        break;
      case 330: /* feature_constraints ::= ffeatures */
#line 1459 "./mql.yxx"
{ mqlyylhsminor.mqlyy520 = mqlyymsp[0].minor.mqlyy520; }
#line 4787 "./mql.c"
  mqlyymsp[0].minor.mqlyy520 = mqlyylhsminor.mqlyy520;
        break;
      case 331: /* ffeatures ::= fterm */
#line 1464 "./mql.yxx"
{ mqlyylhsminor.mqlyy520 = new FFeatures(mqlyymsp[0].minor.mqlyy601); }
#line 4793 "./mql.c"
  mqlyymsp[0].minor.mqlyy520 = mqlyylhsminor.mqlyy520;
        break;
      case 332: /* ffeatures ::= ffeatures KEY_OR fterm */
#line 1465 "./mql.yxx"
{ mqlyylhsminor.mqlyy520 = new FFeatures(mqlyymsp[-2].minor.mqlyy520, mqlyymsp[0].minor.mqlyy601); }
#line 4799 "./mql.c"
  mqlyy_destructor(mqlyypParser,105,&mqlyymsp[-1].minor);
  mqlyymsp[-2].minor.mqlyy520 = mqlyylhsminor.mqlyy520;
        break;
      case 333: /* fterm ::= ffactor */
#line 1470 "./mql.yxx"
{ mqlyylhsminor.mqlyy601 = new FTerm(mqlyymsp[0].minor.mqlyy272); }
#line 4806 "./mql.c"
  mqlyymsp[0].minor.mqlyy601 = mqlyylhsminor.mqlyy601;
        break;
      case 334: /* fterm ::= fterm KEY_AND ffactor */
#line 1471 "./mql.yxx"
{ mqlyylhsminor.mqlyy601 = new FTerm(mqlyymsp[-2].minor.mqlyy601, mqlyymsp[0].minor.mqlyy272); }
#line 4812 "./mql.c"
  mqlyy_destructor(mqlyypParser,35,&mqlyymsp[-1].minor);
  mqlyymsp[-2].minor.mqlyy601 = mqlyylhsminor.mqlyy601;
        break;
      case 335: /* ffactor ::= KEY_NOT ffactor */
{  mqlyy_destructor(mqlyypParser,49,&mqlyymsp[-1].minor);
#line 1476 "./mql.yxx"
{ mqlyymsp[-1].minor.mqlyy272 = new FFactor(mqlyymsp[0].minor.mqlyy272); }
#line 4820 "./mql.c"
}
        break;
      case 336: /* ffactor ::= KEY_OPEN_BRACKET ffeatures KEY_CLOSE_BRACKET */
{  mqlyy_destructor(mqlyypParser,52,&mqlyymsp[-2].minor);
#line 1478 "./mql.yxx"
{ mqlyymsp[-2].minor.mqlyy272 = new FFactor(mqlyymsp[-1].minor.mqlyy520); }
#line 4827 "./mql.c"
  mqlyy_destructor(mqlyypParser,54,&mqlyymsp[0].minor);
}
        break;
      case 337: /* ffactor ::= feature_comparison */
#line 1480 "./mql.yxx"
{ mqlyylhsminor.mqlyy272 = new FFactor(mqlyymsp[0].minor.mqlyy68); }
#line 4834 "./mql.c"
  mqlyymsp[0].minor.mqlyy272 = mqlyylhsminor.mqlyy272;
        break;
      case 341: /* feature_comparison ::= feature_name KEY_IN object_reference_usage */
#line 1492 "./mql.yxx"
{ mqlyylhsminor.mqlyy68 = new FeatureComparison(mqlyymsp[-2].minor.mqlyy0->extractString(), kIn, new Value(mqlyymsp[0].minor.mqlyy266)); deleteToken(mqlyymsp[-2].minor.mqlyy0); }
#line 4840 "./mql.c"
  mqlyy_destructor(mqlyypParser,80,&mqlyymsp[-1].minor);
  mqlyymsp[-2].minor.mqlyy68 = mqlyylhsminor.mqlyy68;
        break;
      case 342: /* comparison_operator ::= KEY_EQUALS */
{  mqlyy_destructor(mqlyypParser,72,&mqlyymsp[0].minor);
#line 1497 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy130 = kEqual; }
#line 4848 "./mql.c"
}
        break;
      case 343: /* comparison_operator ::= KEY_LESS_THAN */
{  mqlyy_destructor(mqlyypParser,117,&mqlyymsp[0].minor);
#line 1498 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy130 = kLessThan; }
#line 4855 "./mql.c"
}
        break;
      case 344: /* comparison_operator ::= KEY_GREATER_THAN */
{  mqlyy_destructor(mqlyypParser,118,&mqlyymsp[0].minor);
#line 1499 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy130 = kGreaterThan; }
#line 4862 "./mql.c"
}
        break;
      case 345: /* comparison_operator ::= KEY_NOT_EQUAL */
{  mqlyy_destructor(mqlyypParser,119,&mqlyymsp[0].minor);
#line 1500 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy130 = kNotEqual; }
#line 4869 "./mql.c"
}
        break;
      case 346: /* comparison_operator ::= KEY_LESS_THAN_OR_EQUAL */
{  mqlyy_destructor(mqlyypParser,120,&mqlyymsp[0].minor);
#line 1501 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy130 = kLessThanOrEqual; }
#line 4876 "./mql.c"
}
        break;
      case 347: /* comparison_operator ::= KEY_GREATER_THAN_OR_EQUAL */
{  mqlyy_destructor(mqlyypParser,121,&mqlyymsp[0].minor);
#line 1502 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy130 = kGreaterThanOrEqual; }
#line 4883 "./mql.c"
}
        break;
      case 348: /* comparison_operator ::= KEY_TILDE */
{  mqlyy_destructor(mqlyypParser,122,&mqlyymsp[0].minor);
#line 1503 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy130 = kTilde; }
#line 4890 "./mql.c"
}
        break;
      case 349: /* comparison_operator ::= KEY_NOT_TILDE */
{  mqlyy_destructor(mqlyypParser,123,&mqlyymsp[0].minor);
#line 1504 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy130 = kNotTilde; }
#line 4897 "./mql.c"
}
        break;
      case 350: /* comparison_operator ::= KEY_HAS */
{  mqlyy_destructor(mqlyypParser,124,&mqlyymsp[0].minor);
#line 1505 "./mql.yxx"
{ mqlyymsp[0].minor.mqlyy130 = kHas; }
#line 4904 "./mql.c"
}
        break;
      case 351: /* value ::= enum_const */
#line 1512 "./mql.yxx"
{ mqlyylhsminor.mqlyy616 = new Value(mqlyymsp[0].minor.mqlyy0->extractString(), kValEnumConst); deleteToken(mqlyymsp[0].minor.mqlyy0); }
#line 4910 "./mql.c"
  mqlyymsp[0].minor.mqlyy616 = mqlyylhsminor.mqlyy616;
        break;
      case 352: /* value ::= signed_integer */
#line 1514 "./mql.yxx"
{ mqlyylhsminor.mqlyy616 = new Value(mqlyymsp[0].minor.mqlyy503); }
#line 4916 "./mql.c"
  mqlyymsp[0].minor.mqlyy616 = mqlyylhsminor.mqlyy616;
        break;
      case 353: /* value ::= STRING */
#line 1516 "./mql.yxx"
{ mqlyylhsminor.mqlyy616 = new Value(mqlyymsp[0].minor.mqlyy0->extractString(), kValString); deleteToken(mqlyymsp[0].minor.mqlyy0); }
#line 4922 "./mql.c"
  mqlyymsp[0].minor.mqlyy616 = mqlyylhsminor.mqlyy616;
        break;
      case 354: /* value ::= object_reference_usage */
#line 1518 "./mql.yxx"
{ mqlyylhsminor.mqlyy616 = new Value(mqlyymsp[0].minor.mqlyy266); }
#line 4928 "./mql.c"
  mqlyymsp[0].minor.mqlyy616 = mqlyylhsminor.mqlyy616;
        break;
      case 356: /* object_reference_usage ::= object_reference KEY_DOT feature_name */
#line 1528 "./mql.yxx"
{ mqlyylhsminor.mqlyy266 = new ObjectReferenceUsage(mqlyymsp[-2].minor.mqlyy0->extractString(), mqlyymsp[0].minor.mqlyy0->extractString()); deleteToken(mqlyymsp[-2].minor.mqlyy0); deleteToken(mqlyymsp[0].minor.mqlyy0); }
#line 4934 "./mql.c"
  mqlyy_destructor(mqlyypParser,125,&mqlyymsp[-1].minor);
  mqlyymsp[-2].minor.mqlyy266 = mqlyylhsminor.mqlyy266;
        break;
      case 357: /* opt_blocks ::= */
#line 1533 "./mql.yxx"
{ mqlyymsp[1].minor.mqlyy18 = 0; }
#line 4941 "./mql.c"
        break;
      case 358: /* opt_blocks ::= blocks */
#line 1534 "./mql.yxx"
{ mqlyylhsminor.mqlyy18 = mqlyymsp[0].minor.mqlyy18; }
#line 4946 "./mql.c"
  mqlyymsp[0].minor.mqlyy18 = mqlyylhsminor.mqlyy18;
        break;
      case 359: /* block ::= object_block */
      case 363: /* block ::= notexist_object_block */ mqlyytestcase(mqlyyruleno==363);
#line 1539 "./mql.yxx"
{ mqlyylhsminor.mqlyy218 = new Block(mqlyymsp[0].minor.mqlyy101); }
#line 4953 "./mql.c"
  mqlyymsp[0].minor.mqlyy218 = mqlyylhsminor.mqlyy218;
        break;
      case 360: /* block ::= power */
#line 1540 "./mql.yxx"
{ mqlyylhsminor.mqlyy218 = new Block(mqlyymsp[0].minor.mqlyy594); }
#line 4959 "./mql.c"
  mqlyymsp[0].minor.mqlyy218 = mqlyylhsminor.mqlyy218;
        break;
      case 361: /* block ::= opt_gap_block */
#line 1541 "./mql.yxx"
{ mqlyylhsminor.mqlyy218 = new Block(mqlyymsp[0].minor.mqlyy303); }
#line 4965 "./mql.c"
  mqlyymsp[0].minor.mqlyy218 = mqlyylhsminor.mqlyy218;
        break;
      case 362: /* block ::= gap_block */
#line 1542 "./mql.yxx"
{ mqlyylhsminor.mqlyy218 = new Block(mqlyymsp[0].minor.mqlyy6); }
#line 4971 "./mql.c"
  mqlyymsp[0].minor.mqlyy218 = mqlyylhsminor.mqlyy218;
        break;
      case 364: /* unordered_group ::= KEY_OPEN_SQUARE_BRACKET KEY_UNORDERED_GROUP object_block_string KEY_CLOSE_SQUARE_BRACKET */
{  mqlyy_destructor(mqlyypParser,18,&mqlyymsp[-3].minor);
#line 1552 "./mql.yxx"
{ mqlyymsp[-3].minor.mqlyy24 = new UnorderedGroup(mqlyymsp[-1].minor.mqlyy98); }
#line 4978 "./mql.c"
  mqlyy_destructor(mqlyypParser,126,&mqlyymsp[-2].minor);
  mqlyy_destructor(mqlyypParser,19,&mqlyymsp[0].minor);
}
        break;
      case 365: /* object_block_string ::= object_block */
#line 1556 "./mql.yxx"
{ mqlyylhsminor.mqlyy98 = new ObjectBlockString(NULL, mqlyymsp[0].minor.mqlyy101); }
#line 4986 "./mql.c"
  mqlyymsp[0].minor.mqlyy98 = mqlyylhsminor.mqlyy98;
        break;
      case 366: /* object_block_string ::= object_block_string object_block */
#line 1557 "./mql.yxx"
{ mqlyylhsminor.mqlyy98 = new ObjectBlockString(mqlyymsp[-1].minor.mqlyy98, mqlyymsp[0].minor.mqlyy101); }
#line 4992 "./mql.c"
  mqlyymsp[-1].minor.mqlyy98 = mqlyylhsminor.mqlyy98;
        break;
      case 367: /* star_monad_set ::= */
#line 1564 "./mql.yxx"
{ mqlyymsp[1].minor.mqlyy108 = new MQLMonadSetElement(0, MAX_MONAD, 0, true); }
#line 4998 "./mql.c"
        break;
      case 369: /* opt_gap_block ::= KEY_OPEN_SQUARE_BRACKET KEY_OPT_GAP mark_declaration gap_retrieval opt_blocks KEY_CLOSE_SQUARE_BRACKET */
{  mqlyy_destructor(mqlyypParser,18,&mqlyymsp[-5].minor);
#line 1570 "./mql.yxx"
{ mqlyymsp[-5].minor.mqlyy303 = new OptGapBlock(mqlyymsp[-2].minor.mqlyy88, mqlyymsp[-3].minor.mqlyy0->extractString(), mqlyymsp[-1].minor.mqlyy18); deleteToken(mqlyymsp[-3].minor.mqlyy0); }
#line 5004 "./mql.c"
  mqlyy_destructor(mqlyypParser,127,&mqlyymsp[-4].minor);
  mqlyy_destructor(mqlyypParser,19,&mqlyymsp[0].minor);
}
        break;
      case 370: /* gap_retrieval ::= */
#line 1575 "./mql.yxx"
{ mqlyymsp[1].minor.mqlyy88 = kNoRetrieve; }
#line 5012 "./mql.c"
        break;
      case 374: /* gap_block ::= KEY_OPEN_SQUARE_BRACKET KEY_GAP mark_declaration gap_retrieval opt_blocks KEY_CLOSE_SQUARE_BRACKET */
{  mqlyy_destructor(mqlyypParser,18,&mqlyymsp[-5].minor);
#line 1584 "./mql.yxx"
{ mqlyymsp[-5].minor.mqlyy6 = new GapBlock(mqlyymsp[-2].minor.mqlyy88, mqlyymsp[-3].minor.mqlyy0->extractString(), mqlyymsp[-1].minor.mqlyy18); deleteToken(mqlyymsp[-3].minor.mqlyy0); }
#line 5018 "./mql.c"
  mqlyy_destructor(mqlyypParser,128,&mqlyymsp[-4].minor);
  mqlyy_destructor(mqlyypParser,19,&mqlyymsp[0].minor);
}
        break;
      case 375: /* feature_retrieval ::= KEY_GET feature_list */
{  mqlyy_destructor(mqlyypParser,62,&mqlyymsp[-1].minor);
#line 1591 "./mql.yxx"
{ mqlyymsp[-1].minor.mqlyy366 = mqlyymsp[0].minor.mqlyy366; }
#line 5027 "./mql.c"
}
        break;
      case 376: /* feature_retrieval ::= */
#line 1592 "./mql.yxx"
{ mqlyymsp[1].minor.mqlyy366 = 0; }
#line 5033 "./mql.c"
        break;
      case 377: /* object_block ::= KEY_OPEN_SQUARE_BRACKET object_type_name mark_declaration object_reference_declaration retrieval firstlast monad_set_relation_clause feature_constraints feature_retrieval opt_blocks KEY_CLOSE_SQUARE_BRACKET */
{  mqlyy_destructor(mqlyypParser,18,&mqlyymsp[-10].minor);
#line 1605 "./mql.yxx"
{ mqlyymsp[-10].minor.mqlyy101 = new ObjectBlock(mqlyymsp[-9].minor.mqlyy0->extractString(), mqlyymsp[-8].minor.mqlyy0->extractString(), mqlyymsp[-7].minor.mqlyy0->extractString(), mqlyymsp[-6].minor.mqlyy88, mqlyymsp[-5].minor.mqlyy296, mqlyymsp[-4].minor.mqlyy299, mqlyymsp[-3].minor.mqlyy520, mqlyymsp[-2].minor.mqlyy366, mqlyymsp[-1].minor.mqlyy18, false); 
      deleteToken(mqlyymsp[-9].minor.mqlyy0); deleteToken(mqlyymsp[-8].minor.mqlyy0); deleteToken(mqlyymsp[-7].minor.mqlyy0); 
    }
#line 5041 "./mql.c"
  mqlyy_destructor(mqlyypParser,19,&mqlyymsp[0].minor);
}
        break;
      case 378: /* notexist_object_block ::= notexist KEY_OPEN_SQUARE_BRACKET object_type_name mark_declaration object_reference_declaration retrieval firstlast monad_set_relation_clause feature_constraints feature_retrieval opt_blocks KEY_CLOSE_SQUARE_BRACKET */
{  mqlyy_destructor(mqlyypParser,285,&mqlyymsp[-11].minor);
#line 1619 "./mql.yxx"
{ mqlyymsp[-11].minor.mqlyy101 = new ObjectBlock(mqlyymsp[-9].minor.mqlyy0->extractString(), mqlyymsp[-8].minor.mqlyy0->extractString(), mqlyymsp[-7].minor.mqlyy0->extractString(), mqlyymsp[-6].minor.mqlyy88, mqlyymsp[-5].minor.mqlyy296, mqlyymsp[-4].minor.mqlyy299, mqlyymsp[-3].minor.mqlyy520, mqlyymsp[-2].minor.mqlyy366, mqlyymsp[-1].minor.mqlyy18, true); 
      deleteToken(mqlyymsp[-9].minor.mqlyy0); deleteToken(mqlyymsp[-8].minor.mqlyy0); deleteToken(mqlyymsp[-7].minor.mqlyy0); 
    }
#line 5051 "./mql.c"
  mqlyy_destructor(mqlyypParser,18,&mqlyymsp[-10].minor);
  mqlyy_destructor(mqlyypParser,19,&mqlyymsp[0].minor);
}
        break;
      case 379: /* power ::= KEY_POWER restrictor */
{  mqlyy_destructor(mqlyypParser,129,&mqlyymsp[-1].minor);
#line 1627 "./mql.yxx"
{ mqlyymsp[-1].minor.mqlyy594 = new Power(mqlyymsp[0].minor.mqlyy244); }
#line 5060 "./mql.c"
}
        break;
      case 380: /* power ::= KEY_POWER KEY_BETWEEN limit KEY_AND limit */
{  mqlyy_destructor(mqlyypParser,129,&mqlyymsp[-4].minor);
#line 1629 "./mql.yxx"
{ mqlyymsp[-4].minor.mqlyy594 = new Power(mqlyymsp[-2].minor.mqlyy244, mqlyymsp[0].minor.mqlyy244); }
#line 5067 "./mql.c"
  mqlyy_destructor(mqlyypParser,130,&mqlyymsp[-3].minor);
  mqlyy_destructor(mqlyypParser,35,&mqlyymsp[-1].minor);
}
        break;
      case 381: /* restrictor ::= */
#line 1633 "./mql.yxx"
{ mqlyymsp[1].minor.mqlyy244 = MAX_MONAD; }
#line 5075 "./mql.c"
        break;
      case 382: /* restrictor ::= KEY_LESS_THAN limit */
{  mqlyy_destructor(mqlyypParser,117,&mqlyymsp[-1].minor);
#line 1634 "./mql.yxx"
{ mqlyymsp[-1].minor.mqlyy244 = mqlyymsp[0].minor.mqlyy244-1; }
#line 5081 "./mql.c"
}
        break;
      case 383: /* restrictor ::= KEY_LESS_THAN_OR_EQUAL limit */
{  mqlyy_destructor(mqlyypParser,120,&mqlyymsp[-1].minor);
#line 1635 "./mql.yxx"
{ mqlyymsp[-1].minor.mqlyy244 = mqlyymsp[0].minor.mqlyy244; }
#line 5088 "./mql.c"
}
        break;
      case 384: /* limit ::= INTEGER */
#line 1641 "./mql.yxx"
{ mqlyylhsminor.mqlyy244 = mqlyymsp[0].minor.mqlyy0->integer; deleteToken(mqlyymsp[0].minor.mqlyy0); }
#line 5094 "./mql.c"
  mqlyymsp[0].minor.mqlyy244 = mqlyylhsminor.mqlyy244;
        break;
      default:
        break;
/********** End reduce actions ************************************************/
  };
  assert( mqlyyruleno<sizeof(mqlyyRuleInfo)/sizeof(mqlyyRuleInfo[0]) );
  mqlyygoto = mqlyyRuleInfo[mqlyyruleno].lhs;
  mqlyysize = mqlyyRuleInfo[mqlyyruleno].nrhs;
  mqlyyact = mqlyy_find_reduce_action(mqlyymsp[-mqlyysize].stateno,(MQLYYCODETYPE)mqlyygoto);
  if( mqlyyact <= MQLYY_MAX_SHIFTREDUCE ){
    if( mqlyyact>MQLYY_MAX_SHIFT ){
      mqlyyact += MQLYY_MIN_REDUCE - MQLYY_MIN_SHIFTREDUCE;
    }
    mqlyymsp -= mqlyysize-1;
    mqlyypParser->mqlyytos = mqlyymsp;
    mqlyymsp->stateno = (MQLYYACTIONTYPE)mqlyyact;
    mqlyymsp->major = (MQLYYCODETYPE)mqlyygoto;
    mqlyyTraceShift(mqlyypParser, mqlyyact);
  }else{
    assert( mqlyyact == MQLYY_ACCEPT_ACTION );
    mqlyypParser->mqlyytos -= mqlyysize;
    mqlyy_accept(mqlyypParser);
  }
}

/*
** The following code executes when the parse fails
*/
#ifndef MQLYYNOERRORRECOVERY
static void mqlyy_parse_failed(
  mqlyyParser *mqlyypParser           /* The parser */
){
  MQLParserARG_FETCH;
#ifndef NDEBUG
  if( mqlyyTraceFILE ){
    fprintf(mqlyyTraceFILE,"%sFail!\n",mqlyyTracePrompt);
  }
#endif
  while( mqlyypParser->mqlyytos>mqlyypParser->mqlyystack ) mqlyy_pop_parser_stack(mqlyypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser fails */
/************ Begin %parse_failure code ***************************************/
#line 140 "./mql.yxx"

  pEE->bSyntaxError = true;
#line 5141 "./mql.c"
/************ End %parse_failure code *****************************************/
  MQLParserARG_STORE; /* Suppress warning about unused %extra_argument variable */
}
#endif /* MQLYYNOERRORRECOVERY */

/*
** The following code executes when a syntax error first occurs.
*/
static void mqlyy_syntax_error(
  mqlyyParser *mqlyypParser,           /* The parser */
  int mqlyymajor,                   /* The major type of the error token */
  MQLParserTOKENTYPE mqlyyminor         /* The minor type of the error token */
){
  MQLParserARG_FETCH;
#define TOKEN mqlyyminor
/************ Begin %syntax_error code ****************************************/
#line 143 "./mql.yxx"

  UNUSED(mqlyymajor);
  UNUSED(mqlyyminor);
  pEE->bSyntaxError = true;
  std::string errMsg = "syntax error near " + TOKEN->getTokenName() + '\n';
  pEE->pError->appendError(errMsg);
#line 5165 "./mql.c"
/************ End %syntax_error code ******************************************/
  MQLParserARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following is executed when the parser accepts
*/
static void mqlyy_accept(
  mqlyyParser *mqlyypParser           /* The parser */
){
  MQLParserARG_FETCH;
#ifndef NDEBUG
  if( mqlyyTraceFILE ){
    fprintf(mqlyyTraceFILE,"%sAccept!\n",mqlyyTracePrompt);
  }
#endif
#ifndef MQLYYNOERRORRECOVERY
  mqlyypParser->mqlyyerrcnt = -1;
#endif
  assert( mqlyypParser->mqlyytos==mqlyypParser->mqlyystack );
  /* Here code is inserted which will be executed whenever the
  ** parser accepts */
/*********** Begin %parse_accept code *****************************************/
/*********** End %parse_accept code *******************************************/
  MQLParserARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/* The main parser program.
** The first argument is a pointer to a structure obtained from
** "MQLParserAlloc" which describes the current state of the parser.
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
void MQLParser(
  void *mqlyyp,                   /* The parser */
  int mqlyymajor,                 /* The major token code number */
  MQLParserTOKENTYPE mqlyyminor       /* The value for the token */
  MQLParserARG_PDECL               /* Optional %extra_argument parameter */
){
  MQLYYMINORTYPE mqlyyminorunion;
  unsigned int mqlyyact;   /* The parser action. */
#if !defined(MQLYYERRORSYMBOL) && !defined(MQLYYNOERRORRECOVERY)
  int mqlyyendofinput;     /* True if we are at the end of input */
#endif
#ifdef MQLYYERRORSYMBOL
  int mqlyyerrorhit = 0;   /* True if mqlyymajor has invoked an error */
#endif
  mqlyyParser *mqlyypParser;  /* The parser */

  mqlyypParser = (mqlyyParser*)mqlyyp;
  assert( mqlyypParser->mqlyytos!=0 );
#if !defined(MQLYYERRORSYMBOL) && !defined(MQLYYNOERRORRECOVERY)
  mqlyyendofinput = (mqlyymajor==0);
#endif
  MQLParserARG_STORE;

#ifndef NDEBUG
  if( mqlyyTraceFILE ){
    fprintf(mqlyyTraceFILE,"%sInput '%s'\n",mqlyyTracePrompt,mqlyyTokenName[mqlyymajor]);
  }
#endif

  do{
    mqlyyact = mqlyy_find_shift_action(mqlyypParser,(MQLYYCODETYPE)mqlyymajor);
    if( mqlyyact <= MQLYY_MAX_SHIFTREDUCE ){
      mqlyy_shift(mqlyypParser,mqlyyact,mqlyymajor,mqlyyminor);
#ifndef MQLYYNOERRORRECOVERY
      mqlyypParser->mqlyyerrcnt--;
#endif
      mqlyymajor = MQLYYNOCODE;
    }else if( mqlyyact <= MQLYY_MAX_REDUCE ){
      mqlyy_reduce(mqlyypParser,mqlyyact-MQLYY_MIN_REDUCE);
    }else{
      assert( mqlyyact == MQLYY_ERROR_ACTION );
      mqlyyminorunion.mqlyy0 = mqlyyminor;
#ifdef MQLYYERRORSYMBOL
      int mqlyymx;
#endif
#ifndef NDEBUG
      if( mqlyyTraceFILE ){
        fprintf(mqlyyTraceFILE,"%sSyntax Error!\n",mqlyyTracePrompt);
      }
#endif
#ifdef MQLYYERRORSYMBOL
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
      if( mqlyypParser->mqlyyerrcnt<0 ){
        mqlyy_syntax_error(mqlyypParser,mqlyymajor,mqlyyminor);
      }
      mqlyymx = mqlyypParser->mqlyytos->major;
      if( mqlyymx==MQLYYERRORSYMBOL || mqlyyerrorhit ){
#ifndef NDEBUG
        if( mqlyyTraceFILE ){
          fprintf(mqlyyTraceFILE,"%sDiscard input token %s\n",
             mqlyyTracePrompt,mqlyyTokenName[mqlyymajor]);
        }
#endif
        mqlyy_destructor(mqlyypParser, (MQLYYCODETYPE)mqlyymajor, &mqlyyminorunion);
        mqlyymajor = MQLYYNOCODE;
      }else{
        while( mqlyypParser->mqlyytos >= mqlyypParser->mqlyystack
            && mqlyymx != MQLYYERRORSYMBOL
            && (mqlyyact = mqlyy_find_reduce_action(
                        mqlyypParser->mqlyytos->stateno,
                        MQLYYERRORSYMBOL)) >= MQLYY_MIN_REDUCE
        ){
          mqlyy_pop_parser_stack(mqlyypParser);
        }
        if( mqlyypParser->mqlyytos < mqlyypParser->mqlyystack || mqlyymajor==0 ){
          mqlyy_destructor(mqlyypParser,(MQLYYCODETYPE)mqlyymajor,&mqlyyminorunion);
          mqlyy_parse_failed(mqlyypParser);
#ifndef MQLYYNOERRORRECOVERY
          mqlyypParser->mqlyyerrcnt = -1;
#endif
          mqlyymajor = MQLYYNOCODE;
        }else if( mqlyymx!=MQLYYERRORSYMBOL ){
          mqlyy_shift(mqlyypParser,mqlyyact,MQLYYERRORSYMBOL,mqlyyminor);
        }
      }
      mqlyypParser->mqlyyerrcnt = 3;
      mqlyyerrorhit = 1;
#elif defined(MQLYYNOERRORRECOVERY)
      /* If the MQLYYNOERRORRECOVERY macro is defined, then do not attempt to
      ** do any kind of error recovery.  Instead, simply invoke the syntax
      ** error routine and continue going as if nothing had happened.
      **
      ** Applications can set this macro (for example inside %include) if
      ** they intend to abandon the parse upon the first syntax error seen.
      */
      mqlyy_syntax_error(mqlyypParser,mqlyymajor, mqlyyminor);
      mqlyy_destructor(mqlyypParser,(MQLYYCODETYPE)mqlyymajor,&mqlyyminorunion);
      mqlyymajor = MQLYYNOCODE;
      
#else  /* MQLYYERRORSYMBOL is not defined */
      /* This is what we do if the grammar does not define ERROR:
      **
      **  * Report an error message, and throw away the input token.
      **
      **  * If the input token is $, then fail the parse.
      **
      ** As before, subsequent error messages are suppressed until
      ** three input tokens have been successfully shifted.
      */
      if( mqlyypParser->mqlyyerrcnt<=0 ){
        mqlyy_syntax_error(mqlyypParser,mqlyymajor, mqlyyminor);
      }
      mqlyypParser->mqlyyerrcnt = 3;
      mqlyy_destructor(mqlyypParser,(MQLYYCODETYPE)mqlyymajor,&mqlyyminorunion);
      if( mqlyyendofinput ){
        mqlyy_parse_failed(mqlyypParser);
#ifndef MQLYYNOERRORRECOVERY
        mqlyypParser->mqlyyerrcnt = -1;
#endif
      }
      mqlyymajor = MQLYYNOCODE;
#endif
    }
  }while( mqlyymajor!=MQLYYNOCODE && mqlyypParser->mqlyytos>mqlyypParser->mqlyystack );
#ifndef NDEBUG
  if( mqlyyTraceFILE ){
    mqlyyStackEntry *i;
    char cDiv = '[';
    fprintf(mqlyyTraceFILE,"%sReturn. Stack=",mqlyyTracePrompt);
    for(i=&mqlyypParser->mqlyystack[1]; i<=mqlyypParser->mqlyytos; i++){
      fprintf(mqlyyTraceFILE,"%c%s", cDiv, mqlyyTokenName[i->major]);
      cDiv = ' ';
    }
    fprintf(mqlyyTraceFILE,"]\n");
  }
#endif
  return;
}
