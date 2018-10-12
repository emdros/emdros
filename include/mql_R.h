/*
 * mql_R.h
 *
 * MQL retrieval functions
 *
 * Ulrik Petersen
 * Created: 3/16-2001
 * Last update: 12/1-2014
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



#ifndef MQL_R__H__
#define MQL_R__H__

#include "monads.h"

class MQLExecEnv; // Forward declaration
class Topograph; // Forward declaration
class Sheaf; // Forward declaration
class Inst; // Forward declaration
class ObjectBlock; // Forward declaration

extern Sheaf* R_topograph(MQLExecEnv *pEE, const SetOfMonads& U, const SetOfMonads& Su, monad_m max_range, Topograph* pTopograph);
extern Inst* R_inst(MQLExecEnv *pEE, const SetOfMonads& Su, ObjectBlock *object_block, eMonadSetRelationOperation *pMonadSetOperation);



#endif /* MQL_R__H__ */
