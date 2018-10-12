/*
 * mql_statement.cpp
 *
 * MQL Statement
 *
 * Ulrik Petersen
 * Created: 4/1-2001 (April 1st, 2001)
 * Last update: 11/10-2005
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


/**@file mql_statement.cpp
 *@brief Implementation of Statement (MQL layer)
 */

#include <mql_types.h>
#include <mql_execution_environment.h>

////////////////////////////////////////////////////////////
//
// Statement
//
// Base class for all statements
//
////////////////////////////////////////////////////////////

/** Constructor
 *@internal
 */
Statement::Statement(MQLExecEnv *pEE) 
  : m_bDeleteResult(true), m_result(0), m_pEE(pEE)
{
}

/** Destructor
 *@internal
 *
 * Only deletes result if m_bDeleteResult has not been set to false
 * with Statement::setDeleteResult(false).
 */
Statement::~Statement() 
{ 
  if (m_bDeleteResult)
    delete m_result; 
}


/** Set the flag saying whether we should delete result in destructor.
 *@internal
 *
 * Used by other methods to take over result
 */
void Statement::setDeleteResult(bool bDoDelete)
{
  m_bDeleteResult = bDoDelete;
}

/** Weed stage of the compiler
 * @internal
 *
 * @param bResult Used to return compiler success (true) or compiler
 * failure (false).
 */
void Statement::weed(bool& bResult) 
{ 
  bResult = true; 
  return; 
}

/** Symbol checking stage of the compiler
 * @internal
 *
 * @param bResult Used to return compiler success (true) or compiler
 * failure (false).
 *
 * @return True on no DB error, false otherwise.
 */
bool Statement::symbol(bool& bResult) 
{ 
  bResult = true; 
  return true; 
}

/** Type checking stage of the compiler
 * @internal
 *
 * @param bResult Used to return compiler success (true) or compiler
 * failure (false).
 *
 * @return True on no DB error, false otherwise.
 */
bool Statement::type(bool& bResult) 
{ 
  bResult = true; 
  return true; 
}

/** Monads stage of the compiler
 * @internal
 *
 * @param bResult Used to return compiler success (true) or compiler
 * failure (false).
 *
 * @return True on no DB error, false otherwise.
 */
bool Statement::monads(bool& bResult) 
{ 
  bResult = true; 
  return true; 
}

/** Execute stage of the compiler
 * @internal
 *
 * @return True on no DB error, false otherwise.
 */
bool Statement::exec() 
{ 
  return true; 
}

/** Get result from statement.
 *
 * Gets the MQLResult from the Statement.  The result is left in the
 * care of the Statement: You should NOT delete the result yourself.
 *
 * \see EmdrosEnv::getResult(), EmdrosEnv::takeOverResult().
 *
 * @return a pointer to the MQLResult inside the statement.
 */
MQLResult* Statement::getResult() 
{ 
  return m_result; 
}
