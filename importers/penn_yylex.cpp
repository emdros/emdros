/*
 * penn_yylex.cpp
 *
 * Penn Treebank yylex
 *
 * Ulrik Petersen
 * Created: 5/1-2001 (1st of May, 2001)
 * Last update: 6/23-2007
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


/**@file penn_yylex.cpp
 *@brief Implementation of yylex() function (Penn Treebank Importer)
 */

#include <penn_yylex.h>
#include <pennimporter.h>



/** Penn Treebank yylex function
 *
 * @param lvalp The PennToken* left value pointer for the parser
 *
 * @param parm A pointer to the PennImporterEnv which is associated
 * with this lexer.
 *
 * @return The \#define of the token read, or 0 if end-of-file.
 */
int penn_yylex(void *lvalp, void *parm)
{
	return ((PennImporterEnv*) parm)->pLexer->scan((PennToken*) lvalp);
}
