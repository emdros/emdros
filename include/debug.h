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

/*
 * Copyright (C) 2001-2018     Ulrik Sandborg-Petersen
 * Copyright (C) 2018-present  Sandborg-Petersen Holding ApS, Denmark
 *
 * Licensed under the MIT License.
 *
 * Please see the file COPYING in the root of the sources for more details.
 *
 */



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
