/*
 * libemdfphp.i: SWIG interface to libemdf.la
 * Created: 2/8-2003 (February 8, 2003)
 * Last update: 12/20-2022
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
#include "../../include/llist.h"
#include "../../include/emdf_output.h"
#include "../../include/emdf_value.h"
#include "../../include/emdfdb.h"
#include "../../include/monads.h"
#include "../../include/table.h"
#include "../../include/string_list.h"
#include "../../include/emdf_enums.h"
#include "../../include/json_classes.h"
%}


%include <typemaps.i>
%include "../../include/emdf.h"
%include "../../include/llist.h"
%include "../../include/emdf_output.h"
%include "../../include/emdf_value.h"
%include "../../include/emdfdb.h"
%include "../../include/monads.h"
%include "../../include/table.h"
%include "../../include/string_list.h"
%include "../../include/emdf_enums.h"
%include "../../include/json_classes.h"




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


