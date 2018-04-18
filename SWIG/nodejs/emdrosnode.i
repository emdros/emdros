/*
 * emdrosnode.i: Node.JS bindings for Emdros using SWIG
 * Created: 2/8-2003 (February 8, 2003)
 * Last update: 2/12-2016
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2003-2016  Ulrik Sandborg-Petersen
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


%module emdrosnode

%begin %{

#include <node.h>

#ifndef SWIG_V8_VERSION
#define SWIG_V8_VERSION ((V8_MAJOR_VERSION << 16) | (V8_MINOR_VERSION << 12) | (V8_UILD_NUMBER))
#endif

%}

%{
#include <iostream>
#include <exception>

%}

/* don't tell us about changes in the first letter of constants */
/* #pragma SWIG nowarn=801 */

/* include common typemaps. */
%include "typemaps.i" 
%include "std_common.i"
%include "std_string.i"
%include "std_vector.i"

 /*
%include "exception.i"
 */

%template(StringVector) std::vector<std::string>;
%template(IntVector) std::vector<long>;


/*
%exception {
  try {
    $action
  } catch (...) {
    SWIG_exception(SWIG_RuntimeError, "Unknown exception");
  }
}
*/




%apply long &INOUT { long & };
%apply bool &INOUT { bool & };

/* This assumes that all "long &" parameters are INOUT,
 * which seems safe, if somewhat inefficient.
 */

%apply const std::string & {std::string &};
%apply std::string & {string &};

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

 /*
%{
#include "../../amalgamation/emdros-lconfig.h"
#include "../../amalgamation/emdros-config.h"
#include "../../amalgamation/emdros.h"
%}

%include "../../amalgamation/emdros-lconfig.h"
%include "../../amalgamation/emdros-config.h"
%include "../../amalgamation/emdros.h"
 */

%include "libemdfnodejs.i"
%include "libmqlnodejs.i"

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
