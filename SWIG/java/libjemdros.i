/*
 * libjemdros.i: Java bindings for Emdros using SWIG
 * Created: 2/8-2003 (February 8, 2003)
 * Last update: 1/16-2019
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



%module jemdros

%{
#include <iostream>
#include "emdros-lconfig.h"
%}


/* don't tell us about changes in the first letter of constants */
#pragma SWIG nowarn=801 

/* include common typemaps. */
%include "typemaps.i" 
%include "various.i"
%include "std_string.i"
%include "std_vector.i"

%include "exception.i"


%{
#if defined(USE_AMALGAMATION) && USE_AMALGAMATION
#include "emdros.h"
#else
#include "../../include/emdros-lconfig.h"
#include "../../include/emdf.h"
#include "../../include/exception_emdros.h"
#include "../../include/string_list.h"
#include "../../include/emdf_exception.h"
#include "../../include/emdf_output.h"
#include "../../include/emdf_value.h"
#include "../../include/emdfdb.h"
#include "../../include/monads.h"
#include "../../include/table.h"
#include "../../include/emdf_enums.h"
#include "../../include/pgemdfdb.h"
#include "../../include/mysqlemdfdb.h"
#include "../../include/sqlite3emdfdb.h"
#include "../../include/mql_error.h"
#include "../../include/mql_execute.h"
#include "../../include/mql_types.h"
#include "../../include/mql_sheaf.h"
#include "../../include/mql_result.h"
#include "../../include/mql_execution_environment.h"
#include "../../include/environment_emdros.h"
#include "../../include/mql_enums.h"
#include "../../include/renderobjects.h"
#include "../../include/renderxml.h"
#include "../../include/harvest_fts.h"
	
#endif
%}

// Include this before using emdros_int64, monad_m, id_d_t, etc.
%include "../../include/emdros-lconfig.h"
%include "../../include/emdf.h"


%template(StringVector) std::vector<std::string>;
%template(IntVector) std::vector<emdros_int64>;


/* make sure that STL exceptions get caught and rethrown. */
%exception {
  try {
    $action
  } catch (const std::exception& e) {
    SWIG_exception(SWIG_RuntimeError, e.what());
  }
}


%typemap(in) bool *INOUT, bool &INOUT (bool temp, jboolean *jbtemp) {
  if (!$input) {
    SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "array null");
    return $null;
  }
  if (JCALL1(GetArrayLength, jenv, $input) == 0) {
    SWIG_JavaThrowException(jenv, SWIG_JavaIndexOutOfBoundsException, "Array must contain at least 1 element");
    return $null;
  }
  jbtemp = JCALL2(GetBooleanArrayElements, jenv, $input, 0);
  temp = (*jbtemp) ? true : false;
  $1 = &temp;
}

%typemap(freearg) bool *INOUT, bool &INOUT ""

%typemap(argout) bool *INOUT, bool &INOUT { 
  *jbtemp$argnum = temp$argnum ? (jboolean) 1 : (jboolean) 0;
  JCALL3(ReleaseBooleanArrayElements, jenv, $input , (jboolean *)jbtemp$argnum, 0); 
}



%apply emdros_int64 &INOUT { emdros_int64 & };
%apply bool &INOUT { bool & };
%apply const std::string & {std::string &};
%apply std::string & {string &};

%constant std::ostream * KSTDOUT = &std::cout;
%constant std::ostream * KSTDERR = &std::cerr;

#define mql_throws(EXCEPT)		   \
%typemap(throws, throws="EXCEPT") EXCEPT { \
  jclass excep = jenv->FindClass("jemdros/EXCEPT"); \
  if (excep) \
    jenv->ThrowNew(excep, $1.what().c_str()); \
  return $null; \
}

%typemap(javabase) EmdrosException "java.lang.Exception";
%typemap(javacode) EmdrosException %{
  public String getMessage() {
    return what();
  }
%}

#define emdf_throws(EXCEPT) \
%typemap(throws, throws="EXCEPT") EXCEPT { \
  jclass excep = jenv->FindClass("jemdros/EXCEPT"); \
  if (excep) \
    jenv->ThrowNew(excep, $1.what().c_str()); \
  return $null; \
}

emdf_throws(EMdFDBDBError)
emdf_throws(EMdFNULLValueException)
emdf_throws(EMdFDBException)
emdf_throws(BadMonadsException)
emdf_throws(WrongCharacterSetException)
emdf_throws(EMdFOutputException)
emdf_throws(TableColumnException)
emdf_throws(TableRowException)
emdf_throws(TableException)
emdf_throws(EmdrosException)



// base and EMdF libraries
%include "../../include/emdf.h"
%include "../../include/exception_emdros.h"
%include "../../include/llist.h"
%include "../../include/string_list.h"
%include "../../include/emdf_exception.h"
%include "../../include/emdf_output.h"
%include "../../include/emdf_value.h"
%include "../../include/emdfdb.h"
%include "../../include/monads.h"
%include "../../include/table.h"
%include "../../include/emdf_enums.h"
%include "../../include/pgemdfdb.h"
%include "../../include/mysqlemdfdb.h"
%include "../../include/sqlite3emdfdb.h"

// MQL library
%include "../../include/mql_execution_environment.h"
%include "../../include/mql_error.h"
%include "../../include/mql_execute.h"
%include "../../include/mql_types.h"
%include "../../include/mql_sheaf.h"
%include "../../include/mql_result.h"
%include "../../include/environment_emdros.h"
%include "../../include/mql_enums.h"


%include "../../include/renderobjects.h"
%include "../../include/renderxml.h"
%include "../../include/harvest_fts.h"
