/*
 * mytable.cpp
 *
 * A vector-representation of an Emdros Table.
 *
 * Ulrik Petersen
 * Created: 4/9-2005
 * Last update: 11/30-2006
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


#include "mytable.h"
#include <string_func.h>
#include <algorithm>

MyTable::MyTable(const StringVec& feature_names)
{
	m_cur = m_end = m_rows.end();
	m_feature_names = feature_names;
	m_largest_object_length = 0;
}

MyTable::MyTable(MyTable *pOtherTable, const SetOfMonads& restriction_set)
{
	m_largest_object_length = 0;

	// Iterate over the restriction set.
	SOMConstIterator somci = restriction_set.const_iterator();
	while (somci.hasNext()) {
		MonadSetElement sommse = somci.next();

		pOtherTable->set_universe(sommse.first(), sommse.last());

		while (pOtherTable->hasNext()) {
			const MyRow& row = pOtherTable->next();

			// Check if there is overlap
			MonadSetElement rowmse(row.getFirstMonad(), row.getLastMonad());
			if (rowmse.overlap(sommse)) {
				// Yes, so add it.
				addRow(row);
			}
		}
	}
	m_feature_names = pOtherTable->m_feature_names;
}

std::string MyTable::toString() const
{
	std::string result;
	for (MonadMMyRowMap::const_iterator ci = m_rows.begin();
	     ci != m_rows.end();
	     ++ci) {
		result += ci->second.toString() + "\n";
	}
	return result;
}

unsigned int MyTable::length(void) const
{
	return m_rows.size();
}

void MyTable::set_universe(monad_m start_monad, monad_m end_monad)
{
	// We can't set m_cur (first one) to m_rows.lower_bound(start_monad)
	// right now, since what we are always looking for when calling this 
	// method is those rows that OVERLAP with this range.
	// That may entail rows that have a start monad that is below
	// start_monad.  

	// However, since we know the length of the largest object,
	// we can look for 
	//
	//      m_rows.lower_bound(start_monad - m_largest_object_length - 1).
	//
	// and be sure to get all we want.

	// It cannot entail monads that have a start monad
	// after end_monad, however, and upper_bound returns the first one
	// after its parameter, or end() if it does not exist, so that is
	// what we want.
	monad_m lowest_possible_monad;
	if (start_monad < m_largest_object_length-1) {
		lowest_possible_monad = 0;
	} else {
		lowest_possible_monad = start_monad - m_largest_object_length - 1;
	}
	m_cur = m_rows.lower_bound(MyTriple(lowest_possible_monad,
					    start_monad, // Is ignored in MyTriple::operator<() because the next parameter is false.
					    false));
	m_end = m_rows.upper_bound(MyTriple(end_monad+1,MAX_MONAD,true));
}

bool MyTable::hasNext(void) const
{
	if (m_rows.empty()) {
		return false; 
	} else {
		return m_cur != m_end;
	}
}

const MyRow& MyTable::next(void)
{
	const MyRow& result = m_cur->second;
	++m_cur;
	return result;
}


void clear_OTTableMap(OTTableMap& mymap)
{
	OTTableMap::iterator it = mymap.begin();
	while (it != mymap.end()) {
		delete it->second;
		++it;
	}
	mymap.clear();
}    
