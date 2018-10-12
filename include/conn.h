/*
 * conn.h
 *
 * Database connection abstract base class.
 *
 * Ulrik Petersen
 * Created: 1/27-2001
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
			   long& result) = 0;
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
