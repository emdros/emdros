/*
 * libmqlphp.i: SWIG interface to libmql.la from PHP
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
#include "../../include/mql_error.h"
#include "../../include/mql_execute.h"
#include "../../include/mql_types.h"
#include "../../include/mql_sheaf.h"
#include "../../include/mql_result.h"
#include "../../include/mql_execution_environment.h"
#include "../../include/emdros_environment.h"
#include "../../include/mql_enums.h"
%}

%include "../../include/llist.h"
%include "../../include/mql_execution_environment.h"
%include "../../include/mql_error.h"
%include "../../include/mql_execute.h"
%include "../../include/mql_types.h"
%include "../../include/mql_sheaf.h"
%include "../../include/mql_result.h"
%include "../../include/environment_emdros.h"
%include "../../include/mql_enums.h"
