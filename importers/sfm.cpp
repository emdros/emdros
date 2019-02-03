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

/*
 * Copyright (C) 2001-2018     Ulrik Sandborg-Petersen
 * Copyright (C) 2018-present  Sandborg-Petersen Holding ApS, Denmark
 *
 * Licensed under the MIT License.
 *
 * Please see the file COPYING in the root of the sources for more details.
 *
 */



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

