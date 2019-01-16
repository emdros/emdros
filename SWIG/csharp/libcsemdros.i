/*
 * libemdros.i: C# bindings for Emdros using SWIG
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



%module csemdros

/* include common typemaps. */
%include "typemaps.i" 
%include "std_string.i"
%include "std_vector.i"
%include "std_except.i"

%include "exception.i"

%rename(csout) out; // Rename all out functions to csout


%{
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
%}



%template(StringVector) std::vector<std::string>;
%template(IntVector) std::vector<emdros_i32>;


/* make sure that STL exceptions get caught and rethrown. */
%exception {
  try {
    $action
  } catch (const std::exception& e) {
    SWIG_exception(SWIG_RuntimeError, e.what());
  }
}


%include "../../include/emdros-lconfig.h"
%include "../../include/emdf.h"


%apply emdros_i32 &INOUT { emdros_i32 & };
%apply emdros_u32 &INOUT { emdros_u32 & };
%apply bool &INOUT { bool & };
%apply const std::string & {std::string &};
%apply std::string & {std::string &};

%constant std::ostream * KSTDOUT = &std::cout;
%constant std::ostream * KSTDERR = &std::cerr;


/* base + EMdF library. */
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





 /* MQL library */
%include "../../include/mql_execution_environment.h"
%include "../../include/mql_error.h"
%include "../../include/mql_execute.h"
%include "../../include/mql_types.h"
%include "../../include/mql_sheaf.h"
%include "../../include/mql_result.h"
%include "../../include/environment_emdros.h"
%include "../../include/mql_enums.h"

 /* harvest library */
%include "../../include/renderobjects.h"
%include "../../include/renderxml.h"
%include "../../include/harvest_fts.h"




%exception %{
try {
    $action
} catch (EMdFDBDBError& e) {
    SWIG_CSharpSetPendingException(SWIG_CSharpApplicationException, (std::string("EMdFDBDBError: ") + e.what()).c_str());
} catch (EMdFNULLValueException& e) {
    SWIG_CSharpSetPendingException(SWIG_CSharpApplicationException, (std::string("EMdFNULLValueException: ") + e.what()).c_str());
} catch (EMdFDBException& e) {
    SWIG_CSharpSetPendingException(SWIG_CSharpApplicationException, (std::string("EMdFDBException: ") + e.what()).c_str());
} catch (BadMonadsException& e) {
    SWIG_CSharpSetPendingException(SWIG_CSharpApplicationException, (std::string("BadMonadsException: ") + e.what()).c_str());
} catch (WrongCharacterSetException& e) {
    SWIG_CSharpSetPendingException(SWIG_CSharpApplicationException, (std::string("WrongCharacterSetException: ") + e.what()).c_str());
} catch (EMdFOutputException& e) {
    SWIG_CSharpSetPendingException(SWIG_CSharpApplicationException, (std::string("EMdFOutputException: ") + e.what()).c_str());
} catch (TableColumnException& e) {
    SWIG_CSharpSetPendingException(SWIG_CSharpApplicationException, (std::string("TableColumnException: ") + e.what()).c_str());
} catch (TableRowException& e) {
    SWIG_CSharpSetPendingException(SWIG_CSharpApplicationException, (std::string("TableRowException: ") + e.what()).c_str());
} catch (TableException& e) {
    SWIG_CSharpSetPendingException(SWIG_CSharpApplicationException, (std::string("TableException: ") + e.what()).c_str());
} catch (EmdrosException& e) {
    SWIG_CSharpSetPendingException(SWIG_CSharpApplicationException, (std::string("EmdrosException: ") + e.what()).c_str());
}
%}
