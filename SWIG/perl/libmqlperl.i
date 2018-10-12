/*
 * libmqlperl.i: SWIG interface to libmql.la from Perl
 * Created: 2/8-2003 (February 8, 2003)
 * Last update: 8/14-2008
 *
 * Recreated on 7/9-2004 from Python SWIG bindings and new typemaps,
 * thereby completely replacing what Chris Wilson had done.  UP.
 *
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
#include "../../include/mql_error.h"
#include "../../include/mql_execute.h"
#include "../../include/mql_types.h"
#include "../../include/mql_sheaf.h"
#include "../../include/mql_result.h"
#include "../../include/mql_execution_environment.h"
#include "../../include/emdros_environment.h"
#include "../../include/mql_enums.h"
%}

%typemap(out) SheafConstIterator {
  SheafConstIterator * resultobj = new SheafConstIterator((SheafConstIterator)$1);
  ST(argvi) = sv_newmortal();
#if SWIG_VERSION >= 0x010335
  SWIG_MakePtr(ST(argvi), (void *) resultobj, SWIGTYPE_p_LlistConstIteratorT_Straw_t, SWIG_SHADOW|SWIG_OWNER);
#else
  SWIG_MakePtr(ST(argvi), (void *) resultobj, SWIGTYPE_p_LlistConstIteratorTStraw_t, SWIG_SHADOW|SWIG_OWNER);
#endif
  sv_bless(ST(argvi), gv_stashpv("EmdrosPerl::SheafConstIterator", 0));
  ++argvi;
}

%typemap(out) StrawConstIterator {
  StrawConstIterator * resultobj = new StrawConstIterator((StrawConstIterator)$1);
  ST(argvi) = sv_newmortal();
#if SWIG_VERSION >= 0x010335
  SWIG_MakePtr(ST(argvi), (void *) resultobj, SWIGTYPE_p_LlistConstIteratorT_MatchedObject_t, SWIG_SHADOW|SWIG_OWNER);
#else
  SWIG_MakePtr(ST(argvi), (void *) resultobj, SWIGTYPE_p_LlistConstIteratorTMatchedObject_t, SWIG_SHADOW|SWIG_OWNER);
#endif
  sv_bless(ST(argvi), gv_stashpv("EmdrosPerl::StrawConstIterator", 0));
  ++argvi;
}


%typemap(out) Straw* {
  Straw * resultobj = (Straw *)$1;
  ST(argvi) = sv_newmortal();
  SWIG_MakePtr(ST(argvi), (void *) resultobj, SWIGTYPE_p_Straw, SWIG_SHADOW);
  ST(argvi) = sv_bless(ST(argvi), gv_stashpv("EmdrosPerl::Straw", 0));
  ++argvi;
}

%typemap(out) Sheaf* {
  Sheaf * resultobj;
  if ($1) {
    resultobj = (Sheaf *)$1;
  } else {
    resultobj = 0;
  }
  ST(argvi) = sv_newmortal();
  SWIG_MakePtr(ST(argvi), (void *) resultobj, SWIGTYPE_p_Sheaf, SWIG_SHADOW);
  if (resultobj != 0) {
    ST(argvi) = sv_bless(ST(argvi), gv_stashpv("EmdrosPerl::Sheaf", 0));
  }
  ++argvi;
}

%typemap(out) MatchedObject* {
  MatchedObject * resultobj = (MatchedObject *)$1;
  ST(argvi) = sv_newmortal();
  SWIG_MakePtr(ST(argvi), (void *) resultobj, SWIGTYPE_p_MatchedObject, SWIG_SHADOW);
  ST(argvi) = sv_bless(ST(argvi), gv_stashpv("EmdrosPerl::MatchedObject", 0));
  ++argvi;
}





%include "../../include/llist.h"
%include "../../include/mql_execution_environment.h"
%include "../../include/mql_error.h"
%include "../../include/mql_execute.h"
%include "../../include/mql_types.h"
%include "../../include/mql_sheaf.h"
%include "../../include/mql_result.h"
%include "../../include/environment_emdros.h"
%include "../../include/mql_enums.h"
