/*
 * logging.cpp
 *
 * Functions for logging
 *
 * Ulrik Petersen
 * Created: 11/21-2002
 * Last update: 5/28-2018
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2004-2018  Ulrik Sandborg-Petersen
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

/**@file logging.cpp
 *@brief Implementation of EMdF logging (EMdF layer)
 */


#include <emdf.h>
#include <logging.h>

#include <time.h>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <emdros-lconfig.h>


/** Maximum string length that a timestamp can be.
 *@internal
 */
#define BUFFER_LENGTH  (30)

/** Get the current time as a string.
 *
 * @internal
 *
 * @param mytime The current time as a string (output).
 *
 */
void get_time(std::string& mytime)
{
	// Allocate time structure
	time_t* pTime = (time_t*) malloc(sizeof(time_t));

	// Allocate string buffer
	// +1 because of the trailing zero.
	char* szTime = (char*) malloc(BUFFER_LENGTH+1);

	// Get localt time
	time(pTime);

	// Convert to string
#if LINUX
	ctime_r(pTime, szTime);
#elif SUN
	// This used to be ctime_r(pTime, szTime, BUFFER_LENGTH);
	// but somewhere in the Solaris 10 series (I think),
	// the last parameter was removed to be POSIX-compliant.		
	ctime_r(pTime, szTime);
#else // This should be Win32
	strncpy(szTime, ctime(pTime), BUFFER_LENGTH-1);
	szTime[BUFFER_LENGTH] = '\0';
#endif

	// Chop newline
	szTime[strlen(szTime)-1] = '\0';

	// Set mytime
	mytime = szTime;

	// Free memory
	free(szTime);
	free(pTime);
}

/** Open the log file.
 *@internal
 */
#define OPEN_LOG(FSTR) std::ofstream FSTR(EMDROS_LOGFILE, std::ios_base::app); \
	if (!FSTR) return; 

/** Write a simple log message.
 *
 * @internal
 *
 * @param function_name The name of the calling function.
 *
 * @param message The log message to log.
 */
void logWrite(std::string function_name, std::string message)
{
#if (EMDROS_LOGGING)
	OPEN_LOG(fout);
	fout << function_name << ":" << message << std::endl;
#else
	UNUSED(function_name);
	UNUSED(message);
#endif
}

/** Write a log message with a timestamp.
 *
 * @internal
 *
 * @param function_name The name of the calling function.
 *
 * @param message The message to log.
 *
 */
void logWriteTime(std::string function_name, std::string message)
{
#if (EMDROS_LOGGING)
	OPEN_LOG(fout);
	std::string mytime;
	get_time(mytime);
	fout << function_name << " " << mytime << ":" << message << std::endl;
#else
	UNUSED(function_name);
	UNUSED(message);
#endif
}



