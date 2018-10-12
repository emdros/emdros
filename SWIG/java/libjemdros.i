/*
 * libjemdros.i: Java bindings for Emdros using SWIG
 * Created: 2/8-2003 (February 8, 2003)
 * Last update: 6/29-2011
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

%include "libemdfjava.i"
%include "libmqljava.i"

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
