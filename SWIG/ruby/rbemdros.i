/*
 * rbemdros.i: Ruby bindings for Emdros using SWIG
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



%module EmdrosRuby

%{
#include <iostream>
%}

/* don't tell us about changes in the first letter of constants */
#pragma SWIG nowarn=801 

/* include common typemaps. */
%include "typemaps.i" 
%include "std_common.i"
%include "std_string.i"
%include "std_vector.i"

%include "exception.i"

%template(StringVector) std::vector<std::string>;
%template(IntVector) std::vector<long>;

/* This assumes that all "long &" parameters are INOUT,
 * which seems safe, if somewhat inefficient.
 */
%apply long &INOUT { long & };
%apply bool &INOUT { bool & };

%apply const std::string & {std::string &};
%apply std::string & {string &};

%constant std::ostream * KSTDOUT = &std::cout;
%constant std::ostream * KSTDERR = &std::cerr;



/* make sure that STL exceptions get caught and rethrown. */
%exception {
  try {
    $action
  } catch (const std::exception& e) {
    SWIG_exception(SWIG_RuntimeError, e.what());
  }
}



/*
%{
#include "../../include/emdf.h"
%}
*/

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

%include "libemdfruby.i"
%include "libmqlruby.i"

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
