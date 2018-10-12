/*
 * exec.h
 *
 * Functions to execute stuff
 *
 * Ulrik Petersen
 * Created: 4/9-2005
 * Last update: 5/31-2005
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


#ifndef EXEC__H__
#define EXEC__H__

#include "emdros_wrapper.h"
#include "conf.h"

extern int exec_stream(std::istream *pStrin, EmdrosWrapper *pEW);
extern int exec_file(std::string filename, EmdrosWrapper *pEW);
extern int exec_cin(EmdrosWrapper *pEW);
extern bool exec_check_configuration(EmdrosWrapper *pEW);


#endif
