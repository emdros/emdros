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

/*
 * Copyright (C) 2001-2018     Ulrik Sandborg-Petersen
 * Copyright (C) 2018-present  Sandborg-Petersen Holding ApS, Denmark
 *
 * Licensed under the MIT License.
 *
 * Please see the file COPYING in the root of the sources for more details.
 *
 */


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
