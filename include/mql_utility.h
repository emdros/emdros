/*
 * mql_utility.h
 *
 * MQL utility functions
 *
 * Ulrik Petersen
 * Created: 3/16-2001
 * Last update: 1/16-2019
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


/**@file mql_utility.h
 *@brief Header file for MQL utilities (MQL layer)
 */



#ifndef MQL_UTILITY__H__
#define MQL_UTILITY__H__

#include "emdf.h"
#include "monads.h"

class Straw; // Forward declaration

extern void R_restrict(const SetOfMonads& SOM, monad_m monad, SetOfMonads& result);
extern void join(Straw* pStraw, ListOfStraws* pLS);
// extern void hat(const SetOfMonads& O, SetOfMonads& out); // Has been obsolete...
#endif /* MQL_UTILITY__H__ */
