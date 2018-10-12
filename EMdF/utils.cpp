/*
 * utils.cpp
 *
 * Utility functions for EMdF
 * Created: 1/30-2001
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
		strOut += tolower(*p);
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

