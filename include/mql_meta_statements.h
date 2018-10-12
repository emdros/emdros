/*
 * mql_meta_statements.h
 *
 * Definitions of AST-classes for MQL
 *
 * Ulrik Petersen
 * Created: 11/22-2002
 * Last update: 11/10-2005
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


/**@file mql_meta_statements.h
 *@brief Header file for MQL meta-statements (MQL layer)
 */



#ifndef MQL_META_STATEMENTS__H__
#define MQL_META_STATEMENTS__H__

#include "mql_types.h"
#include "mql_execution_environment.h"


// QUIT
class QuitStatement : public Statement {
public:
  QuitStatement(MQLExecEnv *pEE);
  virtual ~QuitStatement();
  virtual bool exec();
};




#endif /* MQL_META_STATEMENTS__H__ */
