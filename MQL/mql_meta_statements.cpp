/*
 * mql_meta_statements.cpp
 *
 * MQL meta-statements
 *
 * Ulrik Petersen
 * Created: 1/19-2002
 * Last update: 11/10-2004
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


/**@file mql_meta_statements.cpp
 *@brief Implementation of MQL meta-statements (MQL layer)
 */



#include <mql_meta_statements.h>


/////////////////////////////////////////////////////////////
//
// QuitStatement
//
/////////////////////////////////////////////////////////////
QuitStatement::QuitStatement(MQLExecEnv *pEE) : Statement(pEE)
{
}

QuitStatement::~QuitStatement()
{
}

bool QuitStatement::exec()
{
  m_pEE->bQuit = true;
  return true;
}

