/*
 * harvest.h
 *
 * A class to "harvest" a sheaf.
 *
 * Ulrik Petersen
 * Created: 4/9-2005
 * Last update: 6/8-2018
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2005-2018  Ulrik Sandborg-Petersen
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

#ifndef HARVEST__H__
#define HARVEST__H__

#include "emdf.h"
#include "monads.h"
#include "mql_sheaf.h"

#include <list>
#include <set>

#include "mytable.h"
#include "solution.h"


class EmdrosWrapper; // Forward declaration

typedef Llist<Solution> SolutionList;
typedef LlistConstIterator<Solution> SolutionListConstIterator;

typedef LlistConstIterator<SetOfMonads> SetOfMonadsListConstIterator;
typedef LlistIterator<SetOfMonads> SetOfMonadsListIterator;
typedef Llist<SetOfMonads> SetOfMonadsList;

typedef enum {
	kHitTypeFocus,
	kHitTypeInnermost,
	kHitTypeInnermostFocus,
	kHitTypeOutermost
} eHarvestHitType;

class Harvest {
 private:
	/*
	  # A Harvest is a list of Solution objects.
	  # accessed like: for solution in harvest: do_my_thing(solution)
   
	  # A Harvest is constructed from a EmdrosEnv, a Sheaf and a configuration
	*/
	SolutionList m_solutions;
	std::set<id_d_t> m_raster_id_ds;
	EmdrosWrapper *m_pEW;
	Sheaf *m_pSheaf;
	Configuration *m_pConf;
	eHarvestHitType m_hit_type;
 public:
	Harvest(EmdrosWrapper *pEnv, Sheaf *pSheaf);
	~Harvest();
	SolutionListConstIterator const_iterator() { return SolutionListConstIterator(&m_solutions); };
	unsigned int size() const { return m_solutions.size(); };
 private:
	void sheaf_monads_separated(const Sheaf *pSheaf, eHarvestHitType hit_type, bool bIsOutermost, SetOfMonadsList *pSom_List);
	void straw_monads_separated(const Straw *pStraw, eHarvestHitType hit_type, SetOfMonadsList *pSom_List);
	SetOfMonads maximal_set(MyTable *pTable, SetOfMonads mset);
	SetOfMonads overlap_set(MyTable *pTable, SetOfMonads mset);
	SetOfMonads sheaf_focus(const Sheaf *pSheaf);
	SetOfMonads straw_focus(const Straw *pStraw);

	MyTable *getTable(const SetOfMonads& mset, 
			  const std::string& unit, 
			  const StringVec& features);

	MyTable *getRasterTableWithContext(SetOfMonadsList *pSeparated_solution_monad_list, 
					   monad_m nRasterContextBefore, 
					   monad_m nRasterContextAfter);
};

#endif
