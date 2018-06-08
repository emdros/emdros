/*
 * utils.cpp
 *
 * Utility functions for EMdF
 * Created: 1/30-2001
 * Last update: 6/9-2018
 *
 */

/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2018  Ulrik Petersen
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

/**@file utils.cpp
 *@brief Database utilities (EMdF layer)
 */

 
#include <utils.h>
#include <emdf.h>
#include <ctype.h> // For tolower
 
/** Convert a database name to something that can be passed to the backends.
 *
 * In particular, strIn is lower-cased, and truncated to maximum
 * length MAX_DBNAME_CHARS.
 *
 * @param strIn The database name to change (probably from the user).
 *
 * @param strOut The resulting database name (to be passed to the backend).
 */
void convertDBName(const std::string& strIn, std::string& strOut)
{
	strOut = "";
	std::string::const_iterator p(strIn.begin());
	std::string::const_iterator cend(strIn.end());
	int count = 1;
	while (p != cend
	       && count < MAX_DBNAME_CHARS) {
		strOut += (char) tolower(*p);
		++count;
		++p;
	}
}


/** Converts a bool to either "Y" (true) or "N" (false).
 *
 * @param in The bool to convert.
 *
 * @return "Y" if input boolean is true, "N" otherwise.
 */
char bool2char(bool in)
{
	if (in)
		return 'Y';
	else
		return 'N';
}

/** Convert a char to a boolean: "Y" becomes "true", everything else
 *  becomes "false".
 *
 * @param in The char to convert.
 *
 * @return The boolean: true if input char is "Y", false otherwise.
 */
bool char2bool(char in) 
{
	return in == 'Y';
}

