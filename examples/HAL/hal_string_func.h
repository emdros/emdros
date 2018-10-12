/*
 * hal_string_func.h
 *
 * String helper functions
 *
 * Ulrik Petersen
 * Created: 3/1-2001
 * Last update: 7/17-2003
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





#ifndef __HAL_STRING_FUNC__H__
#define __HAL_STRING_FUNC__H__

#include <string>

extern bool hal_is_other_than_whitespace(const std::string& str);
extern bool hal_has_double_quote(const std::string& str);
extern std::string hal_strip_string(const std::string& input, const std::string& chars_to_strip);

#endif // __HAL_STRING_FUNC__H__
