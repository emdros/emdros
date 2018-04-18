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
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2017  Ulrik Sandborg-Petersen
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, license version 2.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
 **************************************************************************/


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
