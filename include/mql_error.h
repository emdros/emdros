/*
 * mql_error.h
 *
 * Definitions and functions for MQL error handling
 *
 * Ulrik Petersen
 * Created: 3/1-2001 (1st of March, 2001)
 * Last update: 3/9-2006
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2006  Ulrik Petersen
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

/**@file mql_error.h
 *@brief Header file for MQLError (MQL layer)
 */



#ifndef MQL_ERROR__H__
#define MQL_ERROR__H__

#include <string>

/** MQL error string encapsulation.
 *@ingroup MQL
 *
 * A class to hold the latest error messages from the MQL compiler.
 * Any database error can be gotten from the EmdrosEnv::getDBError(),
 * or if using an EMdFDB-derivative, then EMdFDB::errorMessage()
 * and/or EMdFDB::getLocalError().
 *
 * The MQLExecEnv object has an MQLError object which it manages.
 * MQLExecEnv, in turn, is encapsulated by EmdrosEnv, which you should
 * probably use rather than an MQLExecEnv directly.
 */
class MQLError {
 private:
	std::string m_mql_error;
 public:
	MQLError();
	~MQLError();
	void clearError(void);
	std::string getError(void);
#ifndef SWIG
	void appendError(std::string error);
	void prependError(std::string error);
	void setError(std::string error);
#endif // !defined SWIG
};


#endif /* MQL_ERROR__H__ */
