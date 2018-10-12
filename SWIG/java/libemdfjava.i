/*
 * libemdfjava.i: Java SWIG interface to libemdf.la
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
%}

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

