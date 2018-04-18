/*
 * debug.h
 *
 * Debug defines for EMdF
 *
 * Ulrik Petersen
 * Created: 1/30-2001
 * Last update: 2/11-2010
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2010  Ulrik Sandborg-Petersen
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


/**@file debug.h
 *@brief Header file for EMdF debugging (EMdF layer)
 *
 */


#ifndef DEBUG__H__
#define DEBUG__H__

#include "exception_emdros.h"
#include "string_func.h"
#include <iostream>
#include <sstream>

#ifndef EMDF_DEBUG
#define DEBUG_PRINT_ERROR_MESSAGE(err_msg)
#else // EMDF_DEBUG
#define DEBUG_PRINT_ERROR_MESSAGE(err_msg) std::cerr << err_msg;
#endif // EMDF_DEBUG

#define DEBUG_HANDLE_ERROR_MESSAGE(szError) {\
    std::string err_msg = szError; \
    appendLocalError(err_msg); \
    DEBUG_PRINT_ERROR_MESSAGE(err_msg); \
  }

#define DEBUG_SELECT_QUERY_FAILED(szMethod, szQ) { \
    std::ostringstream strerr_msg; \
    strerr_msg << szMethod << ": Query '" << std::endl \
         << szQ << std::endl \
         << "' failed." << std::endl; \
    DEBUG_HANDLE_ERROR_MESSAGE(strerr_msg.str()); \
    pConn->finalize(); \
}

#define DEBUG_BPT_QUERY_FAILED(szMethod, OTN, monad_ms_string, columns_to_get_string, column_restriction_string) { \
    std::ostringstream strerr_msg; \
    strerr_msg << szMethod << ": Query with:\nOTN = '" << OTN << "'\nMonads = '" << monad_ms_string << '\n' \
    << "Columns_to_get: " << columns_to_get_string << '\n' \
         << "column_restriction: '" << column_restriction_string << "'\n...failed!" << std::endl; \
    DEBUG_HANDLE_ERROR_MESSAGE(strerr_msg.str()); \
    pConn->finalize(); \
}

#define DEBUG_GET_NEXT_TUPLE_FAILED(szMethod) { \
    std::ostringstream strerr_msg; \
    strerr_msg << szMethod << ": getNextTuple() failed." << std::endl; \
    DEBUG_HANDLE_ERROR_MESSAGE(strerr_msg.str()); \
    pConn->finalize(); \
}

#define DEBUG_COMMAND_QUERY_FAILED(szMethod, szQ) { \
    std::ostringstream strerr_msg; \
    strerr_msg << szMethod << ": Query '" << std::endl \
         << szQ << std::endl \
         << "' failed." << std::endl; \
    DEBUG_HANDLE_ERROR_MESSAGE(strerr_msg.str()); \
    pConn->finalize(); \
}

#define DEBUG_X_FAILED(szMethod, szWhatFailed) { \
    std::ostringstream strerr_msg; \
    strerr_msg << szMethod << ": " << szWhatFailed << " failed." << std::endl;\
    DEBUG_HANDLE_ERROR_MESSAGE(strerr_msg.str()); \
}
#define DEBUG_NULL_VALUE_EXCEPTION(szMethod) { \
    std::ostringstream strerr_msg; \
    strerr_msg << szMethod << ": NULL VALUE EXCEPTION." << std::endl; \
    DEBUG_HANDLE_ERROR_MESSAGE(strerr_msg.str()); \
    pConn->finalize(); \
}
#define DEBUG_BEGINTRANSACTION_FAILED(szMethod) DEBUG_X_FAILED(szMethod, "beginTransaction")
#define DEBUG_COMMITTRANSACTION_FAILED(szMethod) DEBUG_X_FAILED(szMethod, "commitTransaction")
#define DEBUG_ACCESS_TUPLE_FAILED(szMethod) { \
    std::ostringstream strerr_msg; \
    strerr_msg << szMethod << ": accessTuple() failed." << std::endl; \
    DEBUG_HANDLE_ERROR_MESSAGE(strerr_msg.str()); \
    pConn->finalize(); \
}

#define DEBUG_X_IS_WRONG(szMethod, szWhatIsWrong) { \
  std::ostringstream strerr_msg; \
  strerr_msg << szMethod << ": " << szWhatIsWrong << std::endl; \
  DEBUG_HANDLE_ERROR_MESSAGE(strerr_msg.str()); \
}

#define MY_ASSERT_THROW_X(COND, EXCEP, STR, MYLINE) { if (!(COND)) { std::string mymessage = std::string(#EXCEP ":" __FILE__ ":" ) + long2string(MYLINE) + ":" + STR; throw EXCEP(mymessage); } }


#define ASSERT_THROW_X(COND, EXCEP, STR)  MY_ASSERT_THROW_X(COND, EXCEP, STR, __LINE__)

#define ASSERT_THROW(COND, STR)  ASSERT_THROW_X(COND, EmdrosException, STR)





#endif /* DEBUG__H__ */
