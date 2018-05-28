/*
 * mql_utility.cpp
 *
 * MQL utility functions
 *
 * Ulrik Petersen
 * Created: 3/16-2001
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

