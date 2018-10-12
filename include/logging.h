/*
 * logging.h
 *
 * Crude functions for logging
 *
 * Ulrik Petersen
 * Created: 11/21-2002
 * Last update: 3/18-2008
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


/**@file logging.h
 *@brief Header file for EMdF logging (EMdF layer)
 */


#ifndef _LOGGING__H__
#define _LOGGING__H__

#include <string>

/*
 * Emdros logging
 */
#ifdef WIN32
#define EMDROS_LOGGING     (0)
#define EMDROS_LOGFILE     "c:\\emdros.log"
#else
#define EMDROS_LOGGING     (0)
#define EMDROS_LOGFILE     "/tmp/emdros.log"
#endif




extern void logWrite(std::string function_name, std::string message);
extern void logWriteTime(std::string function_name, std::string message);

#if (EMDROS_LOGGING)
/** Write MSG with function name FN to logfile (if logging is turned on).
 *
 * @param FN The name of the calling function.
 *
 * @param MSG The log message to write.
 *
 */
#define LOG_WRITE(FN,MSG) logWrite(FN,MSG)
/** Write MSG with function name FN and a TIMESTAMP to logfile (if
 * logging is turned on).
 *
 * @param FN The name of the calling function.
 *
 * @param MSG The log message to write.
 *
 */
#define LOG_WRITE_TIME(FN,MSG) logWriteTime(FN,MSG)
#else
/** Write MSG with function name FN to logfile (if logging is turned on).
 *
 * @param FN The name of the calling function.
 *
 * @param MSG The log message to write.
 *
 */
#define LOG_WRITE(FN,MSG) ;

/** Write MSG with function name FN and a TIMESTAMP to logfile (if
 * logging is turned on).
 *
 * @param FN The name of the calling function.
 *
 * @param MSG The log message to write.
 *
 */
#define LOG_WRITE_TIME(FN,MSG) ;
#endif




#endif // _LOGGING__H__
