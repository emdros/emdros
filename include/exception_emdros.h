/*
 * exception_emdros.h
 *
 * Base class for all Emdros exceptions.
 *
 * Ulrik Petersen
 * Created: 1/21-2005
 * Last update: 11/27-2005
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

/**@file exception_emdros.h
 *@brief Header file for base class for all Emdros exceptions.
 */

#ifndef EMDROS_EXCEPTION__H__
#define EMDROS_EXCEPTION__H__

#include <string>


/** Base class for all Emdros exceptions.
 */
class EmdrosException {
private:
  std::string m_message; /**< The exception message. */
public:
  /** Constructor: Give a default exception message. 
   */
  EmdrosException() : m_message("An Emdros exception occurred.") {};

  /** Constructor: Set the exception message.
   *
   * @param message the message to use.
   */
  EmdrosException(const std::string& message) : m_message(message) {};

  /** Destructor. */
  virtual ~EmdrosException() {};

  /** Return the message.
   *
   * @return the message in the exception.
   */
  std::string what() { return m_message; };
};



#endif // EMDROS_EXCEPTION__H__
