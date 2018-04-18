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
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 1999-2011  Ulrik Sandborg-Petersen
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

