/*
 * libemdfperl.i: SWIG interface to libemdf.la from Perl
 * Created: 2/8-2003 (February 8, 2003)
 * Last update: 4/18-2018
 *
 * Recreated on 7/9-2004 from Python SWIG bindings and new typemaps,
 * thereby completely replacing what Chris Wilson had done.  UP.
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
#include "../../include/emdf_output.h"
#include "../../include/emdf_value.h"
#include "../../include/emdfdb.h"
#include "../../include/monads.h"
#include "../../include/table.h"
#include "../../include/string_list.h"
#include "../../include/emdf_enums.h"
%}


%typemap(out) TableRowIterator {
  TableRowIterator * resultobj = new TableRowIterator((TableRowIterator)$1);
  ST(argvi) = sv_newmortal();
  SWIG_MakePtr(ST(argvi), (void *) resultobj, SWIGTYPE_p_TableRowIterator, SWIG_SHADOW|SWIG_OWNER);
  sv_bless(ST(argvi), gv_stashpv("EmdrosPerl::TableRowIterator", 0));
  ++argvi;
}

%typemap(out) TableIterator {
  TableIterator * resultobj = new TableIterator((TableIterator)$1);
  ST(argvi) = sv_newmortal();
  SWIG_MakePtr(ST(argvi), (void *) resultobj, SWIGTYPE_p_TableIterator, SWIG_SHADOW|SWIG_OWNER);
  sv_bless(ST(argvi), gv_stashpv("EmdrosPerl::TableIterator", 0));
  ++argvi;
}


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

