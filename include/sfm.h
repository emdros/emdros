/*
 * sfm.h
 *
 * supporting functions and types for SFM input
 *
 * Ulrik Petersen
 * Created: 8/25-1999
 * Last update: 10/3-2011
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





/*
 * NOTES:
 * - One assumption is that each field occupies only one line.
 * - Another assumption is that the caller knows when the record is finished.
 * - Another assumption is that sfm_eat_white_after_record is called between records.
 * 
 */

#ifndef _SFM__H__
#define _SFM__H__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "sfm.cpp"
#endif

#include <iostream>
#include "exception_emdros.h"

class SFM_Exception : public EmdrosException { 
public:
	SFM_Exception() : EmdrosException(std::string("An Emdros SFM_Exception occurred.")) {
	};
	SFM_Exception(const std::string& message) : EmdrosException("An Emdros SFM_Exception occurred.\n" + message) {
	};
	       
};


extern void sfm_extract_id_and_value(const std::string& line, std::string& field_id, 
				     std::string& field_value);

extern void sfm_read_line(std::istream& fin, std::string& field_id, 
			  std::string& field_value);

extern void sfm_parse_line(std::istream& fin, const std::string field_to_look_for, 
			   std::string& field_value);

extern void sfm_eat_white_after_record(std::istream& fin);

extern bool sfm_is_number_field(const std::string& field_id, std::string& number_field_id, int& field_number);


#endif // _SFM__H__

