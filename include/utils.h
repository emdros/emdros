/*
 * utils.h
 *
 * Utility functions for EMdF
 *
 * Ulrik Petersen
 * Created: 1/27-2001
 * Last update: 4/29-2006
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


/**@file utils.h
 *@brief Database utilities (EMdF layer)
 */



#ifndef UTILS__H__
#define UTILS__H__

#include <string>

void convertDBName(const std::string& strIn, std::string& strOut);
char bool2char(bool in);
bool char2bool(char in);


#endif /* UTILS__H__ */
