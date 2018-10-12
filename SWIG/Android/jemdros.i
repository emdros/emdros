/*
 * jemdros.i: Java-Android bindings for Emdros using SWIG
 * Created: 2/8-2003 (February 8, 2003)
 * Last update: 4/18-2018
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
%}


/* don't tell us about changes in the first letter of constants */
#pragma SWIG nowarn=801 

/* include common typemaps. */
%include "typemaps.i" 
%include "various.i"
%include "std_string.i"
%include "std_vector.i"

%include "exception.i"

%template(StringVector) std::vector<std::string>;
%template(IntVector) std::vector<long>;


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



%apply long &INOUT { long & };
%apply bool &INOUT { bool & };
%apply const std::string & {std::string &};
%apply std::string & {string &};

/* This assumes that all "long &" parameters are INOUT,
 * which seems safe, if somewhat inefficient.
 */

%constant std::ostream * KSTDOUT = &std::cout;
%constant std::ostream * KSTDERR = &std::cerr;




/* To get monad_m and id_d_t typedefs. */
%include "../../include/emdf.h"

/*
%{
typedef long monad_m;
typedef long id_d_t;
%}
typedef long monad_m;
typedef long id_d_t;

*/

 /* EMdF library */

%{
#include "../../include/emdf.h"
#include "../../include/exception_emdros.h"
#include "../../include/emdf_exception.h"
#include "../../include/emdf_output.h"
#include "../../include/emdf_value.h"
#include "../../include/emdfdb.h"
#include "../../include/monads.h"
#include "../../include/table.h"
#include "../../include/string_list.h"
#include "../../include/emdf_enums.h"
#include "../../include/json_classes.h"
%}

%typemap(javabase) EmdrosException "java.lang.Exception";
%typemap(javacode) EmdrosException %{
  public String getMessage() {
    return what();
  }
%}

#define emdf_throws(EXCEPT) \
%typemap(throws, throws="EXCEPT") EXCEPT { \
  jclass excep = jenv->FindClass("com.scripturesys.emdros/EXCEPT"); \
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



%include "../../include/exception_emdros.h"
%include "../../include/emdf_exception.h"
%include "../../include/llist.h"
%include "../../include/emdf.h"
%include "../../include/emdf_output.h"
%include "../../include/emdf_value.h"
%include "../../include/emdfdb.h"
%include "../../include/monads.h"
%include "../../include/table.h"
%include "../../include/string_list.h"
%include "../../include/emdf_enums.h"
%include "../../include/json_classes.h"


 // Not necessary -- we don't want to instantiate these!
 /***
%{
#include "../../include/pgemdfdb.h"
%}
%include "../../include/pgemdfdb.h"

%{
#include "../../include/mysqlemdfdb.h"
%}
%include "../../include/mysqlemdfdb.h"

%{
#include "../../include/sqlite3emdfdb.h"
%}
%include "../../include/sqlite3emdfdb.h"
**/

 // Not necessary -- we don't want to instantiate these!
 /***
%{
#include "../../include/bptemdfdb.h"
%}
%include "../../include/bptemdfdb.h"
 **/


 /* MQL library */


%{
#include "../../include/mql_error.h"
#include "../../include/mql_execute.h"
#include "../../include/mql_types.h"
#include "../../include/mql_sheaf.h"
#include "../../include/string2som_map.h"
#include "../../include/mql_result.h"
#include "../../include/mql_execution_environment.h"
#include "../../include/emdros_environment.h"
#include "../../include/mql_enums.h"
%}

#define mql_throws(EXCEPT) \
%typemap(throws, throws="EXCEPT") EXCEPT { \
  jclass excep = jenv->FindClass("com.scripturesys.emdros/EXCEPT"); \
  if (excep) \
    jenv->ThrowNew(excep, $1.what().c_str()); \
  return $null; \
}




%include "../../include/llist.h"
%include "../../include/mql_execution_environment.h"
%include "../../include/mql_error.h"
%include "../../include/mql_execute.h"
%include "../../include/mql_types.h"
%include "../../include/mql_sheaf.h"
%include "../../include/string2som_map.h"
%include "../../include/mql_result.h"
%include "../../include/environment_emdros.h"
%include "../../include/mql_enums.h"



 /* harvest library */
%{
#include "../../include/renderobjects.h"
#include "../../include/renderxml.h"
#include "../../include/harvest_fts.h"
%}

 /* harvest library */
%include "../../include/renderobjects.h"
%include "../../include/renderxml.h"
%include "../../include/harvest_fts.h"


