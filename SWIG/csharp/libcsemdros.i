/*
 * libemdros.i: C# bindings for Emdros using SWIG
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



%module csemdros

/* include common typemaps. */
%include "typemaps.i" 
%include "std_string.i"
%include "std_vector.i"
%include "std_except.i"

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





%apply long &INOUT { long & };
%apply bool &INOUT { bool & };
%apply const std::string & {std::string &};
%apply std::string & {string &};

%constant std::ostream * KSTDOUT = &std::cout;
%constant std::ostream * KSTDERR = &std::cerr;




/* To get monad_m and id_d_t typedefs. */
%include "../../include/emdf.h"


%include "libemdf.i"
%include "libmql.i"

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



%exception %{
try {
    $action
} catch (EMdFDBDBError e) {
    SWIG_CSharpSetPendingException(SWIG_CSharpApplicationException, (std::string("EMdFDBDBError: ") + e.what()).c_str());
} catch (EMdFNULLValueException e) {
    SWIG_CSharpSetPendingException(SWIG_CSharpApplicationException, (std::string("EMdFNULLValueException: ") + e.what()).c_str());
} catch (EMdFDBException e) {
    SWIG_CSharpSetPendingException(SWIG_CSharpApplicationException, (std::string("EMdFDBException: ") + e.what()).c_str());
} catch (BadMonadsException e) {
    SWIG_CSharpSetPendingException(SWIG_CSharpApplicationException, (std::string("BadMonadsException: ") + e.what()).c_str());
} catch (WrongCharacterSetException e) {
    SWIG_CSharpSetPendingException(SWIG_CSharpApplicationException, (std::string("WrongCharacterSetException: ") + e.what()).c_str());
} catch (EMdFOutputException e) {
    SWIG_CSharpSetPendingException(SWIG_CSharpApplicationException, (std::string("EMdFOutputException: ") + e.what()).c_str());
} catch (TableColumnException e) {
    SWIG_CSharpSetPendingException(SWIG_CSharpApplicationException, (std::string("TableColumnException: ") + e.what()).c_str());
} catch (TableRowException e) {
    SWIG_CSharpSetPendingException(SWIG_CSharpApplicationException, (std::string("TableRowException: ") + e.what()).c_str());
} catch (TableException e) {
    SWIG_CSharpSetPendingException(SWIG_CSharpApplicationException, (std::string("TableException: ") + e.what()).c_str());
} catch (EmdrosException e) {
    SWIG_CSharpSetPendingException(SWIG_CSharpApplicationException, (std::string("EmdrosException: ") + e.what()).c_str());
}
%}
