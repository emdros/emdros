/*
 * conn.h
 *
 * Database connection abstract base class.
 *
 * Ulrik Petersen
 * Created: 1/27-2001
 * Last update: 6/8-2018
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2018  Ulrik Sandborg-Petersen
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

/**@file conn.h
 *@brief Header file for EMdFConnection (EMdF layer)
 */



#ifndef EMDF_CONN__H__
#define EMDF_CONN__H__

#include <sstream>
#include "emdf.h"
#include "exception_emdros.h"
#include "emdf_exception.h"


class EMdFConnection {
public:
  // Constructor and destructor
  EMdFConnection();
  virtual ~EMdFConnection();
  
  // Status of connection
  // Returns true on connection good, false on connection bad
  virtual bool connectionOk(void) = 0;
  
  // Executing SELECT
  virtual bool execSelect(const std::string& query) = 0;

  // Executing all others
  virtual bool execCommand(const std::string& query) = 0;

  // Get last error message
  // Returns true on error message available 
  // (even if it is "OK"), false on error 
  // message not available.
  virtual bool errorMessage(std::string& out) = 0;

  // Transactions
  virtual bool beginTransaction(void) = 0;
  virtual bool commitTransaction(void) = 0;
  virtual bool abortTransaction(void) = 0;

  // Field count
  // Returns -1 on no fields.
  virtual int getNoOfFields(void) = 0;

  //*********************************************************
  //**
  //** Note to self...
  //**
  //** Ulrik, PLEASE NOTE
  //** 
  //** The benefit of moving the getNextTuple/accessTuple
  //** to a regime where the return value is actually returned
  //** (rather than being passed as a reference parameter)
  //** and errors taken care of by exceptions, is
  //** EXACTLY ZILCH!
  //**
  //** You have tried this twice now, both times unsuccessfully.
  //**
  //** Not even if optimization is turned on do you get any 
  //** benefit.
  //** 
  //** Please stop wasting your own time chasing rabbit trails.
  //**
  //*********************************************************


  // Getting next tuple
  // Returns true on success, false on failure
  // On success, bMoreTuples tells us whether there are any more tuples.
  virtual bool getNextTuple(bool& bMoreTuples) = 0;

  // Is there a row in the local cache?
  // Must be called right after the query is done.
  virtual bool hasRow(void) const = 0;

  // Tuple-access
  // First tuple is 0
  // First field is 0.
  virtual bool accessTuple(int field_no, 
			   bool& result) = 0;
  virtual bool accessTuple(int field_no, 
			   emdros_int64& result) = 0;
  virtual bool accessTuple(int field_no, 
			   std::string& result) = 0;

  // Be done with a result set
  virtual void finalize(void) = 0;

  // UseDatabase
  virtual bool useDatabase(const std::string& database_name, const std::string& key);

  // Local error messages
  void appendLocalError(std::string error);
  void clearLocalError(void);
  std::string getLocalError(void);

protected:

private:
  // Make the assignment operator and copy constructor private
  // because they are not supported.
  EMdFConnection(const EMdFConnection&);
  EMdFConnection& operator=(const EMdFConnection&);

  // Local error message
  std::string m_local_errormessage;
};


#endif // EMDF_CONN__H__
