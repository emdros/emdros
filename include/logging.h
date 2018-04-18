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
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2002-2008  Ulrik Sandborg-Petersen
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
