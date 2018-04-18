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
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2005  Ulrik Petersen
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
