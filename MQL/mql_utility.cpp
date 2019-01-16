/*
 * mql_utility.cpp
 *
 * MQL utility functions
 *
 * Ulrik Petersen
 * Created: 3/16-2001
 * Last update: 1/19-2019
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


/**@file mql_utility.cpp
 *@brief Implementation of MQL utilities (MQL layer)
 */


#include <mql_sheaf.h>
#include <mql_utility.h>



/** Remove from som SOM all the monads from SOM.first() to monad_m
 *  monad-1, and return the resulting SOM in result.
 *
 * @param SOM the input SetOfMnads.
 *
 * @param monad The first monad to stay in the set.
 *
 * @param result The resulting SetOfMonads.
 */
void R_restrict(const SetOfMonads& SOM, monad_m monad, SetOfMonads& result)
{
  result = SOM;
  monad_m SOM_first = SOM.first();
  monad_m mm1 = monad-1;
  if (SOM_first <= mm1) { 
    result.removeMSE(SOM_first, mm1);
  }
}

/** Call pLS->join(pStraw).
 * This was never used, so it was removed.
 * Probably it wasn't used because of the overhead in calling this
 * function, rather than just calling pLS->join(pStraw) directly.
 */
/*
void join(Straw* pStraw, ListOfStraws* pLS)
{
  pLS->join(pStraw);
}
*/

/*
 * This has been obsoleted.  See mql_R.cpp for what its replacement is.
 */
/*
void hat(const SetOfMonads& O, SetOfMonads& out)
{
  out.add(O.first(), O.last());
}
*/

