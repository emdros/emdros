/*
 * libemdros.i: C# bindings for Emdros using SWIG
 * Created: 2/8-2003 (February 8, 2003)
 * Last update: 5/29-2018
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2003-2018  Ulrik Sandborg-Petersen
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


%module csemdros

/* include common typemaps. */
%include "typemaps.i" 
%include "std_string.i"
%include "std_vector.i"
%include "std_except.i"

%include "exception.i"

%rename(csout) out; // Rename all out functions to csout


%{
#include "emdros-lconfig.h"

#if defined(USE_AMALGAMATION) && USE_AMALGAMATION

#include "emdros.h"

#else
	
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



%template(StringVector) std::vector<std::string>;
// %template(IntVector) std::vector<emdros_int64>;


/* make sure that STL exceptions get caught and rethrown. */
%exception {
  try {
    $action
  } catch (const std::exception& e) {
    SWIG_exception(SWIG_RuntimeError, e.what());
  }
}


%include "../../include/emdf.h"


%apply emdros_int64 &INOUT { emdros_int64 & };
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
