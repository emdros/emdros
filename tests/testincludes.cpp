/*
 * testincludes.cpp
 *
 * Trial of includes
 * Created: 11/8-2002 (November 8, 2002)
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



#include <conn.h>
#include <debug.h>
#include <emdfdb.h>
#include <emdf.h>
#include <infos.h>
#include <inst.h>
#include <inst_object.h>
#include <monads.h>
#include <string_func.h>
#include <table.h>
#include <utils.h>
#include <emdf_output.h>
#include <mql_execute.h>
#include <mql_execution_environment.h>
#include <mql_types.h>
#include <mql_error.h>
#include <mql_result.h>
#include <mql_sheaf.h>
#include <mql_extern.h>
#include <emdros_environment.h>
#include <environment_emdros.h>

#if USE_POSTGRESQL
#include <pgemdfdb.h>
#endif
#if USE_MYSQL
#include <mysqlemdfdb.h>
#endif



int main(int argc, char *argv[])
{
  return 0;
}
