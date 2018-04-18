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
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2002  Ulrik Petersen
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



#ifndef __HAL_STRING_FUNC__H__
#define __HAL_STRING_FUNC__H__

#include <string>

extern bool hal_is_other_than_whitespace(const std::string& str);
extern bool hal_has_double_quote(const std::string& str);
extern std::string hal_strip_string(const std::string& input, const std::string& chars_to_strip);

#endif // __HAL_STRING_FUNC__H__
