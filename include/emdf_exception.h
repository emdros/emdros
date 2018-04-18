/*
 * emdf_exception.h
 *
 * EMdF exceptions
 *
 * Ulrik Petersen
 * Created: 2/12-2005
 * Last update: 11/27-2012
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2012  Ulrik Sandborg-Petersen
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
/**@file emdf_exception.h
 *@brief Header file for EMdF exceptions
 */

#ifndef EMDF_EXCEPTION__H__
#define EMDF_EXCEPTION__H__

#include "exception_emdros.h"


/** EMdF general EMdFDB exception.
 */
class EMdFDBException : public EmdrosException{
public:
  /** Constructor: Give a default exception message. 
   */
  EMdFDBException() : EmdrosException("A EMdFDBException occurred.") {};

  /** Constructor: Set the exception message.
   *
   * @param message the message to use.
   */
  EMdFDBException(const std::string& message) : EmdrosException(message) {};

  /** Destructor. */
  virtual ~EMdFDBException() {};
};

/** EMdF EMdFDB DB error exception.
 */
class EMdFDBDBError : public EMdFDBException {
public:
  /** Constructor: Give a default exception message. 
   */
  EMdFDBDBError() : EMdFDBException("A EMdFDBDBError occurred.") {};

  /** Constructor: Set the exception message.
   *
   * @param message the message to use.
   */
  EMdFDBDBError(const std::string& message) : EMdFDBException(message) {};

  /** Destructor. */
  virtual ~EMdFDBDBError() {};
};


/** EMdF connection NULL value exception.
 */
class EMdFNULLValueException : public EmdrosException {
public:
  /** Constructor: Give a default exception message. 
   */
  EMdFNULLValueException() : EmdrosException("A EMdFNULLValueException occurred: A NULL value was received from the backend.") {};

  /** Constructor: Set the exception message.
   *
   * @param message the message to use.
   */
  EMdFNULLValueException(const std::string& message) : EmdrosException(message) {};

  /** Destructor. */
  virtual ~EMdFNULLValueException() {};
};


/** EMdF EMdFOutput exception.
 */
class EMdFOutputException : public EmdrosException {
public:
  /** Constructor: Give a default exception message. 
   */
  EMdFOutputException() : EmdrosException("A EMdFOutputException occurred.") {};

  /** Constructor: Set the exception message.
   *
   * @param message the message to use.
   */
  EMdFOutputException(const std::string& message) : EmdrosException(message) {};
  /** Destructor. */
  virtual ~EMdFOutputException() {};
};

class WrongCharacterSetException : public EMdFOutputException {
public:
  WrongCharacterSetException() : EMdFOutputException("A WrongCharacterSetException occurred.") {};
  WrongCharacterSetException(const std::string& message) : EMdFOutputException(message) {};
  virtual ~WrongCharacterSetException() {};
};

// General exceptions
/** EMdF general table exception.
 */
class TableException : public EmdrosException {
public:
  /** Constructor: Give a default exception message. 
   */
  TableException() : EmdrosException("A Table exception occurred.") {};

  /** Constructor: Set the exception message.
   *
   * @param message the message to use.
   */
  TableException(const std::string& message) : EmdrosException(message) {};

  /** Destructor. */
   virtual ~TableException() {};
}; 

// Row exceptions
/** EMdF table row exception.
 */
class TableRowException : public TableException {
public:
  /** Constructor: Give a default exception message. 
   */
  TableRowException() : TableException("A Table row exception occurred.") {};

  /** Constructor: Set the exception message.
   *
   * @param message the message to use.
   */
  TableRowException(const std::string& message) : TableException(message) {};

  /** Destructor. */
   virtual ~TableRowException() {};
}; 

// Column exceptions
/** EMdF table column exception.
 *
 * Is most likely "column out of range".
 */
class TableColumnException : public TableException {
public:
  /** Constructor: Give a default exception message. 
   */
  TableColumnException() : TableException("A Table column exception occurred.") {};

  /** Constructor: Set the exception message.
   *
   * @param message the message to use.
   */
  TableColumnException(const std::string& message) : TableException(message) {};

  /** Destructor. */
   virtual ~TableColumnException() {};
}; 


/** Bad monads exception.
 *\ingroup MonadSets
 *
 * Thrown by MonadSetElement::MonadSetElement(monad_m, monad_m) if the
 * monad-range is non-monotonic, i.e., if first > last.
 */
class BadMonadsException : public EmdrosException {
public:
  /** Constructor: Give a default exception message. 
   */
  BadMonadsException() : EmdrosException("A BadMonadsException occurred.") {};

  /** Constructor: Set the exception message.
   *
   * @param message the message to use.
   */
  BadMonadsException(const std::string& message) : EmdrosException(message) {};

  /** Destructor. */
  virtual ~BadMonadsException() {};
};





#endif // EMDF_EXCEPTION__H__
