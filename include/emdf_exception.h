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

/*
 * Copyright (C) 2001-2018     Ulrik Sandborg-Petersen
 * Copyright (C) 2018-present  Sandborg-Petersen Holding ApS, Denmark
 *
 * Licensed under the MIT License.
 *
 * Please see the file COPYING in the root of the sources for more details.
 *
 */

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
