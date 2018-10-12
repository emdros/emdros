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

/*
 * Copyright (C) 2001-2018     Ulrik Sandborg-Petersen
 * Copyright (C) 2018-present  Sandborg-Petersen Holding ApS, Denmark
 *
 * Licensed under the MIT License.
 *
 * Please see the file COPYING in the root of the sources for more details.
 *
 */


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



