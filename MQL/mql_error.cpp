/*
 * mql_error.cpp
 *
 * Definitions and functions for MQL error handling
 *
 * Ulrik Petersen
 * Created: 3/1-2001 (1st of March, 2001)
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

/**@file mql_error.cpp
 *@brief Implementation of MQLError (MQL layer)
 */

#include <mql_error.h>

/** Constructor.
 *
 * Do not use directly: Instead, let the MQLExecEnv manage the
 * MQLError object for you.  In turn, MQLExecEnv is managed by
 * EmdrosEnv, so you should probably use that instead of EmdrosEnv.
 */
MQLError::MQLError()
{
}

/** Destructor.
 *
 * Do not call directly.  Instead, let the MQLExecEnv object manage
 * the MQLError object for you.
 */
MQLError::~MQLError()
{
}


/** Clear the error message (after you have read it off).
 *
 * Use this to clear any error message which you have read.
 */
void MQLError::clearError(void)
{
  m_mql_error = "";
}

/** Append an error message
 *\internal
 *
 * Used by the MQL compiler to append error messages.
 */
void MQLError::appendError(std::string error)
{
  m_mql_error += error;
}

/** Prepend an error message
 *\internal
 *
 * Used by the MQL compiler to prepend error messages.
 */
void MQLError::prependError(std::string error)
{
  m_mql_error = error + m_mql_error;
}

/** Set (clear + set) the error message
 *\internal
 *
 * Used by the MQL compiler to clear the error message and set it to
 * something else.
 */
void MQLError::setError(std::string error)
{
  m_mql_error = error;
}

/** Get error message
 *
 * You can use this to get the error message.  However, if you have an
 * EmdrosEnv object, it is much easier to use
 * EmdrosEnv::getCompilerError().
 */
std::string MQLError::getError(void)
{
  return m_mql_error;
}



