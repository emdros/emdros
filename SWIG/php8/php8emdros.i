/*
 * phpemdros.i: PHP bindings for Emdros using SWIG
 * Created: 2/8-2003 (February 8, 2003)
 * Last update: 11/4-2023
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



%module(directors="1") EmdrosPHP8


/* From the SWIG 4.1 docs:
    
   If the interface file uses %pragma(php) include=... or
   %pragma(php) code=... then SWIG will also generate a third file,
   example.php to contain what these specify. In SWIG < 4.1.0, this
   third file was always generated as it defined the PHP classes, etc
   (but this is now done via C code in example_wrap.c) and also
   contained code to dynamically load the extension (but this used
   the PHP dl() function, which isn't recommended nowadays).
   

   So, since PHP8 support was only added in 4.1.0, and since it does
   nothing nowadays, it is better to not generate it, and leave it out
   of the distributed files altogether. Its absence will trigger
   developers to change their code.

*/
  

/* As of SWIG 4.1.0 and PHP8, the documentation says:

   "If you're writing a new set of bindings and only targeting PHP8
   or newer then enabling type declarations everywhere probably makes
   sense. It will only actually make a difference if you enable
   directors and are wrapping C++ classes with virtual methods, but
   doing it anyway means you won't forget to if the code you are
   wrapping later evolves to have such classes and methods."

   So, turn them on here, since this directory is not for PHP < 8."

 */
%feature("php:type", "1");  

%feature("director") MQLResultCallback;





/* don't tell us about changes in the first letter of constants */
/* #pragma SWIG nowarn=801  */

/* include common typemaps. */
%include <typemaps.i>
%include <utils.i>
%include <stl.i>
%include <std_common.i>
%include <std_string.i>
%include <std_vector.i>
%include <std_map.i>
%include <exception.i>

/* This assumes that all "long &" parameters are INOUT,
 * which seems safe, if somewhat inefficient.
 */
%define %standard_byref_params(TYPE)
       %apply TYPE& INOUT { TYPE& };
       %apply TYPE& OUTPUT { TYPE &result, TYPE &bResult };
%enddef

 

%standard_byref_params(bool)
%standard_byref_params(long)



/*
%apply const std::string & {std::string &};
%apply std::string & {string &};
*/

/*
// Doesn't work with PHP
%constant std::ostream * KSTDOUT = &std::cout;
%constant std::ostream * KSTDERR = &std::cerr;
*/




%template(StringVector) std::vector<std::string>;
%template(IntVector) std::vector<long>;

%{
#include <iostream>
%}


/* make sure that STL exceptions get caught and rethrown. */
%exception {
  try {
    $action
  } catch (const std::exception& e) {
    SWIG_exception(SWIG_RuntimeError, e.what());
  }
}






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

%include "libemdfphp.i"
%include "libmqlphp.i"

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
