/*
 * harvest.h
 *
 * A class to "harvest" a sheaf.
 *
 * Ulrik Petersen
 * Created: 4/9-2005
 * Last update: 3/19-2008
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



#ifndef HARVEST__H__
#define HARVEST__H__

#include <emdros_environment.h>
#include <mql_sheaf.h>
#include <list>
#include <llist.h>
#include <set>
#include "mytable.h"
#include "solution.h"
#include "conf.h"

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
