/*
 * hal_string_func.cpp
 *
 * String helper functions
 *
 * Ulrik Petersen
 * Created: 3/1-2001
 * Last update: 3/1-2017
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



#include <hal_string_func.h>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cstring>
#include <sstream>
#include <iostream>
#include <string_func.h>

bool hal_is_other_than_whitespace(const std::string& str)
{
  return strcspn(str.c_str(), " \t\n\r\v") != 0;
}

bool hal_has_double_quote(const std::string& str)
{
  return str.find_first_of('"') != std::string::npos;
}

std::string hal_strip_string(const std::string& input, const std::string& chars_to_strip)
{
	std::string result = "";
	for (std::string::size_type i = 0; i < input.length(); ++i) {
		// See if input[i] can not be found in chars_to_strip
		if (chars_to_strip.find_first_of(input[i], 0) == std::string::npos) {
			result += input[i];
		} else {
			; // Don't add it.
		}
	}
	return result;
}
