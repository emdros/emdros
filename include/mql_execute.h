/*
 * mql_execute.h
 *
 * Functions for executing MQL queries
 *
 * Ulrik Petersen
 * Created: 3/28-2001
 * Last update: 4/11-2017
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


/**@file mql_execute.h
 *@brief Header file for MQL execution functions (MQL layer)
 */



#ifndef MQL_EXECUTE__H__
#define MQL_EXECUTE__H__

#ifdef __DJGPP__
#include "elconf.h"
#else
#include "emdros-lconfig.h"
#endif

#if HAVE_ISTREAM
#include <istream>
#else
#include <istream.h>
#endif

#include <string>
#ifdef __DJGPP__
#include "mql_ex~1.h"
#include "mql_ex~2.h"
#include "mql_ex~3.h"
#else
#include "mql_execution_environment.h"
#endif

/**
 * \defgroup MQLExecute MQL Execution functions
 *\ingroup MQL
 *
 * These functions form low-level entry points into MQL execution.  To
 * use them, you must have an MQLExecEnv object.  However, it is
 * probably easier to use an EmdrosEnv and its
 * EmdrosEnv::executeString(), EmdrosEnv::executeFile(), and
 * EmdrosEnv::executeStream() methods.
 */
/*@{*/
extern bool mqlExecuteFile(MQLExecEnv *pEE, 
			   std::string filename, 
			   bool& bResult, 
			   bool bPrintResult, 
			   bool bReportError,
			   MQLResultCallback *pCallback = 0);

#ifndef SWIG
extern bool mqlExecuteStream(MQLExecEnv *pEE, 
			     std::istream& strin, 
			     bool& bResult, 
			     bool bPrintResult, 
			     bool bReportError,
			     MQLResultCallback *pCallback = 0);

#endif


extern bool mqlExecuteString(MQLExecEnv *pEE, 
			     const std::string& input, 
			     bool& bResult, 
			     bool bPrintResult, 
			     bool bReportError,
			     MQLResultCallback *pCallback = 0);

#ifndef SWIG
extern bool mqlGetNextQuery(std::istream *pStrin,
			    std::string& result);

#endif
/*@}*/



#endif /* MQL_EXECUTE__H__ */
