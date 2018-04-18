/*
 * sfm.cpp
 *
 * supporting functions and types for SFM input
 *
 * Ulrik Petersen
 * Created: 8/25-1999
 * Last update: 7/15-2008
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 1999-2008  Ulrik Sandborg-Petersen
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License as
 *   published by the Free Software Foundation, license version 2.  
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *   02111-1307 USA
 *
 *
 *   Special exception
 *   =================
 * 
 *   In addition, as a special exception, Ulrik Petersen, the
 *   copyright holder of Emdros, gives permission to link Emdros, in
 *   whole or in part, with the libraries which are normally
 *   distributed with:
 *   
 *   a) Sun's Java platform,
 *   b) Python, 
 *   c) Jython,
 *   d) Ruby, and/or 
 *   e) Perl 
 *   f) PostgreSQL
 *   g) OpenSSL
 *
 *   (or with modified versions of these), and to distribute linked
 *   combinations including both Emdros, in whole or in part, and one
 *   or more of the libraries normally distributed with (a)-(g) above.
 *
 *   Please note: This gives you special rights concerning the
 *   libraries which normally accompany the above pieces of software.
 *   It gives you no special rights concerning software that you write
 *   yourself.  You must obey the GNU General Public License in all
 *   respects for all of the code used other than the libraries
 *   normally distributed with (a)-(g) above.
 *
 *   If you modify this file, you may extend this exception to your
 *   version of the file, but you are not obligated to do so. If you
 *   do not wish to do so, delete this exception statement from your
 *   version.
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
 *
 *   Website
 *   =======
 *
 *   Emdros has a website here:
 *
 *   http://emdros.org
 *
 *
 *
 **************************************************************************/


#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "sfm.h"
#endif

#include <sfm.h>
#include <sstream>
#include <cstdlib>
#include <string_func.h>


#define MAX_LINE_LENGTH (1024)



void sfm_extract_id_and_value(const std::string& line, std::string& field_id, std::string& field_value)
{
	std::string::size_type i = line.find_first_of(" ");
	if (i == std::string::npos) {
		field_id = line;
		field_value = "";
	} else {
		field_id = line.substr(0, i);
		field_value = line.substr(i+1);
	}
}

void sfm_read_line(std::istream& fin, std::string& field_id, std::string& field_value)
{
	char buf[MAX_LINE_LENGTH];
	std::string line;

	// Read line
	fin.getline(buf, MAX_LINE_LENGTH, '\n');

	// Make string from line
	line = buf;

	// Remove carriage return
	line = remove_char(line, '\r');

	// Extract id and value
	sfm_extract_id_and_value(line, field_id, field_value);
}


void sfm_parse_line(std::istream& fin, const std::string field_to_look_for, std::string& field_value)
{
	std::string field_id;
	sfm_read_line(fin, field_id, field_value);

	if (field_id != field_to_look_for) {
		std::ostringstream ostr;
		ostr << "ERROR while reading some file:  Did not find " 
			  << field_to_look_for << " field.";
		throw SFM_Exception(ostr.str());
	}
}


void sfm_eat_white_after_record(std::istream& fin)
{
	while (!fin.eof()) {
		char c;
		fin.get(c);
		if (c == '\\') {
			fin.putback(c);
			break;
		}
	}
	return;
}

bool sfm_is_number_field(const std::string& field_id, std::string& number_field_id, int& field_number)
{
	std::string::size_type index = field_id.find_first_of("123456789");
	if (index == std::string::npos) {
		return false;
	} else {
		number_field_id = field_id.substr(0, index);
		field_number = atoi(field_id.substr(index).c_str());
		return true;
	}
}

