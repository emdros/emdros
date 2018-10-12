/*
 * hal_build_database.h
 *
 * Functions for building a HAL db
 *
 * Created: Sometime in the first half of 2003
 * Last update: 11/3-2015
 *
 *******************************************************************/

/*
 * Copyright (C) 2001-2018     Ulrik Sandborg-Petersen
 * Copyright (C) 2018-present  Sandborg-Petersen Holding ApS, Denmark
 *
 * Licensed under the MIT License.
 *
 * Please see the file COPYING in the root of the sources for more details.
 *
 */




#ifndef __HAL_BUILD_DB__H__
#define __HAL_BUILD_DB__H__

#include <string>

class EmdrosEnv; // Forward declaration

extern bool CreateDB(EmdrosEnv *pEEE, std::string dbname);
extern bool build_db(std::string inputfilename, std::string dbname, std::string wordlist_filename, bool bAddNumbers);

#endif // __HAL_BUILD_DB__H__
