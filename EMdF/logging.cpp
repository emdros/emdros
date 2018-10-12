/*
 * logging.cpp
 *
 * Functions for logging
 *
 * Ulrik Petersen
 * Created: 11/21-2002
 * Last update: 10/4-2018
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


/**@file logging.cpp
 *@brief Implementation of EMdF logging (EMdF layer)
 */


#include <emdf.h>
#include <logging.h>

#include <time.h>
#include <fstream>
#include <cstdlib>
#include <cstring>
#ifdef __DJGPP__
#include <elconf.h>
#else
#include <emdros-lconfig.h>
#endif


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
#elif SUN && defined(_POSIX_PTHREAD_SEMANTICS)
	// This used to be ctime_r(pTime, szTime, BUFFER_LENGTH); but
	// when _POSIX_PTHREAD_SEMANTICS is defined, the C library
	// becomes POSIX-compliant on this front, so the last
	// parameter is removed.
	ctime_r(pTime, szTime);
#elif SUN && !defined(_POSIX_PTHREAD_SEMANTICS)
	ctime_r(pTime, szTime, BUFFER_LENGTH);
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



