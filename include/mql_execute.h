/*
 * mql_execute.h
 *
 * Functions for executing MQL queries
 *
 * Ulrik Petersen
 * Created: 3/28-2001
 * Last update: 5/28-2018
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2018  Ulrik Sandborg-Petersen
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

/**@file mql_execute.h
 *@brief Header file for MQL execution functions (MQL layer)
 */



#ifndef MQL_EXECUTE__H__
#define MQL_EXECUTE__H__

#include <istream>
#include <string>

#include "emdros-lconfig.h"
#include "mql_execution_environment.h"


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
