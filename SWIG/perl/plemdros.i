/*
 * plemdros.i: Perl bindings for Emdros using SWIG
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



%module EmdrosPerl

%{
#include <iostream>
/* These two, which are defined in Perl's embed.h, 
 * conflict with the Linux STL iostream library.
 */
#undef do_open
#undef do_close
%}



%include "typemaps.i"
%include <std_string.i>
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





#pragma SWIG nowarn=801 


%include "../../include/emdf.h"

%apply long &INOUT { long & };
%apply bool &INOUT { bool & };

%constant std::ostream * kSTDOUT = &std::cout;
%constant std::ostream * kSTDERR = &std::cerr;

%include "libemdfperl.i"
%include "libmqlperl.i"

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
