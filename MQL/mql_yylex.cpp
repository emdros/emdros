/*
 * mql_yylex.cpp
 *
 * MQL yylex
 *
 * Ulrik Petersen
 * Created: 5/1-2001 (1st of May, 2001)
 * Last update: 7/17-2004
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


/**@file mql_yylex.cpp
 *@brief Implementation of yylex() function (MQL layer)
 */

#include <mql_yylex.h>
#include <mql_execution_environment.h>


/** MQL yylex function
 *
 * The yylex function as it is called from yyparse (the Bison parser).
 *
 * @param lvalp The Token* left value pointer from the Bison parser.
 *
 * @param parm The MQLExecEnv* parameter passed to the Bison parser.
 *
 * @return The \#define of the token read, or 0 if end-of-file.
 */
int yylex(void *lvalp, void *parm)
{
	return ((MQLExecEnv*) parm)->pLexer->scan((Token*) lvalp);
}
