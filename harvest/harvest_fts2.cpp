/*
 * harvest_fts2.cpp
 *
 * Full Text Search engine version 2, part of the Emdros Harvest
 * library.
 *
 * Ulrik Sandborg-Petersen
 * Created: 8/25-2010
 * Last update: 5/11-2018
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2010-2018  Ulrik Sandborg-Petersen
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

/**@file harvest_fts.cpp
 *@brief Implementation file for harvest full text search
 */

#include <harvest_fts.h>
#include <harvest_fts2.h>
#include <map>

#include <emdf.h>
#include <mql_sheaf.h>
#include <emdf_value.h>
#include <mql_execution_environment.h>
#include <emdfdb.h>
#include <string_func.h> // String functions in libemdf.



#define MYABS(A) (((A) < 0) ? (-(A)) : (A))
#define MYMAX(A,B) ((A > B) ? (A) : (B))
#define MYMIN(A,B) ((A < B) ? (A) : (B))


// In harvest_fts.cpp
extern std::string make_index_name(const std::string& index_infix_suffix, const std::string& bookcase_OTN, const std::string& indexed_OTN, const std::list<std::string>& indexed_feature_name_list);


//////////////////////////////////////////////////////////////////////////
//
// BookcaseIndexer2
//
// The class which does the indexing.
//
//////////////////////////////////////////////////////////////////////////
BookcaseIndexer2::BookcaseIndexer2(EmdrosEnv *pSourceEnv, EmdrosEnv *pTargetEnv, std::ostream *pMQLOutStream, const std::string& bookcase_OTN, const std::string& indexed_OTN, const std::string& indexed_feature_name_list, EmdrosFTSStringFilter *pStringFilter, long max_bookcases_per_object)
	: BookcaseIndexer(pSourceEnv, pTargetEnv, pMQLOutStream, bookcase_OTN, indexed_OTN, indexed_feature_name_list, pStringFilter),
	  m_max_bookcases_per_object(max_bookcases_per_object),
	  m_bookcase_count(0),
	  m_object_count(0)
{
}

BookcaseIndexer2::~BookcaseIndexer2()
{
	clear_mymap();
	clear_query_cache();
}




bool BookcaseIndexer2::process(const SetOfMonads& substrate, bool bEmitVacuum)
{
	// Make sure we are pristine in the query cache.
	clear_query_cache();

	if (m_pSourceEnv == 0) {
		appendError("Source Environment's is nil. Cannot continue.");
		return false;
	}

	if (!(m_pSourceEnv->connectionOk())) {
		appendError("Source Environment's connection is not OK.");
		return false;
	}

	if (m_bookcase_OTN == "") {
		appendError("bookcase_OTN is empty. Cannot continue.");
		return false;
	}

	if (m_indexed_OTN == "") {
		appendError("indexed_OTN is empty. Cannot continue.");
		return false;
	}

	if (m_indexed_feature_name_list.empty()) {
		appendError("indexed_feature_name is empty. Cannot continue.");
		return false;
	}

	m_index_OTN = make_index_name("_index2", m_bookcase_OTN, m_indexed_OTN, m_indexed_feature_name_list);

	emit(std::string("DROP OBJECT TYPE [") + m_index_OTN + "]");
	go(); // Ignore the error if the object type did not exist.
	errorMessage(); // Clear error; ignore error message.

	eObjectRangeType objectRangeType;
	bool bObjectTypeExists;
	id_d_t object_type_id;
	eMonadUniquenessType monadUniquenessType;
	if (!m_pSourceEnv->getMQLEE()->pDB->objectTypeExists(m_bookcase_OTN,
							     bObjectTypeExists,
							     object_type_id,
							     objectRangeType,
							     monadUniquenessType)) {
		appendError("Bookcase object type '" + m_bookcase_OTN + "' does not exist!");
		return false;
	}

	std::string range_type_string;
	range_type_string = "WITH MULTIPLE RANGE OBJECTS";

	emit(std::string("CREATE OBJECT TYPE ") + range_type_string + " [" + m_index_OTN + " feature_value : STRING WITH INDEX; monad_set : SET OF MONADS;]");
	if (!go()) {
		appendError("Could not create object type: " + m_index_OTN);
		return false;
	}

	emit("DROP INDEXES ON OBJECT TYPE [" + m_index_OTN + "]");
	if (!go()) {
		appendError("Could not drop indexes on object type: " + m_index_OTN);
		return false;
	}

	m_object_count = 0;

	emit("BEGIN TRANSACTION");
	if (!go()) {
		appendError("Could not begin transaction");
		return false;
	}

	SetOfMonads substrate_som;
	if (substrate.isEmpty()) {
		substrate_som.add(1, MAX_MONAD);
	} else {
		substrate_som = substrate;
	}

	Sheaf *pBookcase_sheaf = 0;
	bool bEmitFinalGO = false;
	bool bEmitCreateObjectsWithObjectType = true;
	try {
		std::string query = "SELECT ALL OBJECTS IN " + substrate_som.toString() + " WHERE [" + m_bookcase_OTN + "]GO";

		if (!execQueryOnSource(query)) {
			appendError("Could not get all bookcase objects in substrate\nby executing the following query:\n" + query);
			return false;
		}
                
                pBookcase_sheaf = m_pSourceEnv->takeOverSheaf();

                SheafConstIterator bookcase_sheaf_ci = pBookcase_sheaf->const_iterator();
		while (bookcase_sheaf_ci.hasNext()) {
			const Straw *pBookcaseStraw = bookcase_sheaf_ci.next();

			if (bEmitCreateObjectsWithObjectType) {
				emit("CREATE OBJECTS WITH OBJECT TYPE [" + m_index_OTN + "]");
				bEmitCreateObjectsWithObjectType = false;
				print_mymap();
			}
			
			StrawConstIterator bookcase_straw_ci = pBookcaseStraw->const_iterator();
			while (bookcase_straw_ci.hasNext()) {
				const MatchedObject *pMO = bookcase_straw_ci.next();
				
				SetOfMonads mo_som = pMO->getMonads();

				if (!process_bookcase(mo_som.first(), mo_som.last())) {
					appendError("Could not process bookcase with monads:" + mo_som.toString());
					throw EmdrosException("Could not process bookcase with monads:" + mo_som.toString());
				} else {
					bEmitFinalGO = true;
				}

				
				if (m_object_count >= 50000) {
					emit("\nGO\n");
					m_object_count = 0;
					bEmitFinalGO = false;
					bEmitCreateObjectsWithObjectType = true;
				}

			}
		}
	} catch (EmdrosException& e) {
		std::cerr << e.what() << std::endl;
	} catch (...) {
		
	}

	delete pBookcase_sheaf;

	if (!m_mymap.empty()) {
		if (bEmitCreateObjectsWithObjectType) {
			emit("CREATE OBJECTS WITH OBJECT TYPE [" + m_index_OTN + "]");
			bEmitCreateObjectsWithObjectType = false; // Defensive programming. Not necessary. But keeps invariants intact.
		}
		print_mymap();
		bEmitFinalGO = true;
	}
	

	if (bEmitFinalGO) {
		if (!go()) {
			appendError("Could not emit final 'GO'");
			return false;
		}
	}

	emit("COMMIT TRANSACTION");
	if (!go()) {
		appendError("Could not commit transaction");
		return false;
	}

	emit("CREATE INDEXES ON OBJECT TYPE [" + m_index_OTN + "]");
	if (!go()) {
		appendError("Could not create indexes on object type: " + m_index_OTN);
		return false;
	}
	
	if (bEmitVacuum) {
		emit("VACUUM DATABASE ANALYZE");
		if (!go()) {
			appendError("Could not vacuum database at the end (though everything else went fine!)");
			return false;
		}
	}

	return true;
}


std::string BookcaseIndexer2::errorMessage(void)
{
	std::string result = m_error_message;
	m_error_message.clear();
	return result;
}


inline void token_decision(monad_m token_first_monad, monad_m token_last_monad, monad_m bookcase_first_monad, monad_m bookcase_last_monad, bool& bDoAdd, bool &bAdvance, bool &bContinue, monad_m& fm_add, monad_m& lm_add)
{
	bDoAdd = false;
	bAdvance = false;
	if (token_last_monad < bookcase_first_monad) {
		bAdvance = true;
		bDoAdd = false;
	} else if (token_first_monad > bookcase_last_monad) {
		bContinue = false;
		bDoAdd = false;
	} else if (token_first_monad < bookcase_first_monad
		   && token_last_monad >= bookcase_first_monad
		   && token_last_monad <= bookcase_last_monad) {
		fm_add = bookcase_first_monad;
		lm_add = token_last_monad;
		bDoAdd = true;
		bAdvance = true;
	} else if (token_first_monad < bookcase_first_monad
		   && token_last_monad > bookcase_last_monad) {
		fm_add = bookcase_first_monad;
		lm_add = bookcase_last_monad;
		bDoAdd = true;
		bContinue = false;
	} else if (token_first_monad >= bookcase_first_monad
		   && token_last_monad <= bookcase_last_monad) {
		fm_add = token_first_monad;
		lm_add = token_last_monad;
		bDoAdd = true;
		bAdvance = true;
	} else if (token_first_monad >= bookcase_first_monad
		   && token_first_monad <= bookcase_last_monad
		   && token_last_monad > bookcase_last_monad) {
		fm_add = token_first_monad;
		lm_add = bookcase_last_monad;
		bDoAdd = true;
		bContinue = false;
	} else {
		ASSERT_THROW(false,
			     "Monad interval overlap logic error! We should not be able to get here!");
		
	}

	

}


bool BookcaseIndexer2::process_bookcase(monad_m first_monad, monad_m last_monad)
{
	std::string query = "SELECT ALL OBJECTS IN {" + monad_m2string(first_monad) + "-" + monad_m2string(last_monad) + "} WHERE [" + m_indexed_OTN + " GET " + joinList(",", m_indexed_feature_name_list, 4096) + "]";

        Sheaf *pIndexedOTSheaf = 0;
	m_bookcase_count += 1;

	try {
		if (!execQueryOnSource(query)) {
			appendError("Could not get objects by executing query:\n" + query);
			return false;
		}

		pIndexedOTSheaf = m_pSourceEnv->takeOverSheaf();
		
		if (pIndexedOTSheaf == 0) {
			appendError("Sheaf was nil when getting objects by executing query:\n" + query);
			return false;
		}
		

		unsigned int no_of_indexed_feature_names = m_indexed_feature_name_list.size();
		unsigned int i;
		SheafConstIterator indexed_OT_sheaf_ci = pIndexedOTSheaf->const_iterator();

		while (indexed_OT_sheaf_ci.hasNext()) {
			const Straw *pIndexedOTStraw = indexed_OT_sheaf_ci.next();

			StrawConstIterator indexed_OT_straw_ci = pIndexedOTStraw->const_iterator();
			while (indexed_OT_straw_ci.hasNext()) {
				const MatchedObject *pMO = indexed_OT_straw_ci.next();

				monad_m fm = pMO->getFirst();

				std::set<std::string> feature_value_set;
				
				for (i = 0;
				     i < no_of_indexed_feature_names;
				     ++i) {
					std::string feature_value = pMO->getFeatureAsString(i);

					bool bUseValue = true;
					if (m_pStringFilter) {
						feature_value = m_pStringFilter->process(feature_value, bUseValue);
					}


					if (bUseValue) {
						feature_value_set.insert(feature_value);
					}

				}

				if (!feature_value_set.empty()) {
					std::set<std::string>::const_iterator set_ci = feature_value_set.begin();
					while (set_ci != feature_value_set.end()) {
						std::string feature_value = *set_ci++;

						if (m_mymap.find(feature_value) == m_mymap.end()) {
							m_mymap.insert(std::make_pair(feature_value, std::make_pair(new SetOfMonads(), new SetOfMonads())));
						}
						
						m_mymap[feature_value].second->add(fm);
						m_mymap[feature_value].first->add(first_monad, last_monad);
					}
				}
			}
		}

		delete pIndexedOTSheaf;
		pIndexedOTSheaf = 0;

		if (m_bookcase_count > m_max_bookcases_per_object) {
			print_mymap();
		}
	} catch (...) {
		; // Do nothing
	}

	// This is safe, even if we have deleted it already,
	// since we set it to nil when we do.
	delete pIndexedOTSheaf;

	return true;
}

bool BookcaseIndexer2::process_bookcase(const SetOfMonads& som)
{
	return process_bookcase(som.first(), som.last());
}

void BookcaseIndexer2::print_mymap()
{
	string2PSOMPairMap::const_iterator map_ci = m_mymap.begin();
	while (map_ci != m_mymap.end()) {
		std::string feature_value = map_ci->first;
		const SetOfMonads *pSOM0 = map_ci->second.first;
		const SetOfMonads *pSOM1 = map_ci->second.second;

		std::string object_query_prefix = "CREATE OBJECT FROM MONADS=" + pSOM0->toString() + "[feature_value:=\"";

		emit(object_query_prefix + encodeSTRINGstring(feature_value) + "\";monad_set:=" + pSOM1->toString() + ";]");

		++m_object_count;

		++map_ci;
	}

	clear_mymap();

	m_bookcase_count = 0;
}

void BookcaseIndexer2::clear_mymap()
{
	// Clear m_mymap, deleting all SetOfMonad objects.
	string2PSOMPairMap::iterator map_i = m_mymap.begin();
	while (map_i != m_mymap.end()) {
		SetOfMonads *pSOM0 = map_i->second.first;
		SetOfMonads *pSOM1 = map_i->second.second;
		
		delete pSOM0;
		delete pSOM1;
		
		++map_i;
	}
	m_mymap.clear();
}


///////////////////////////////////////////////////////////////////////
//
// BookcaseHarvester2
//
//////////////////////////////////////////////////////////////////////////
BookcaseHarvester2::BookcaseHarvester2(EmdrosEnv *pEnv, const std::string& bookcase_OTN, const std::string& indexed_OTN, const std::string& indexed_feature_name_list) 
	: BookcaseHarvester(pEnv, bookcase_OTN, indexed_OTN, indexed_feature_name_list)
{
}

BookcaseHarvester2::~BookcaseHarvester2()
{
	// Nothing to do. We don't own anything.
}


// bookcase-first-monad --> bookcase-last-monad
void BookcaseHarvester2::getLMDict(const SetOfMonads& substrate, monad2monadMap& lmdict)
{
	std::string query = "GET OBJECTS HAVING MONADS IN " + substrate.toString() + "[" + m_bookcase_OTN + "]";
	FlatSheaf *pIndex_sheaf = 0;
	bool bResult = true;
	if (!m_pEnv->executeString(query,
				   bResult,
				   false, // bPrintResult
				   true  // bReportError
				   )) {
		bResult = false;
		ASSERT_THROW(false, "Wasn't able to execute query for harvesting.");
	}
	if (!bResult) {
		ASSERT_THROW(false, "Wasn't able to execute query for harvesting.");
	}

	pIndex_sheaf = m_pEnv->takeOverFlatSheaf();

	FlatSheafConstIterator index_sheaf_ci = pIndex_sheaf->const_iterator();
	
	while (index_sheaf_ci.hasNext()) {
		const FlatStraw *pIndex_straw = index_sheaf_ci.next();
		FlatStrawConstIterator index_straw_ci = pIndex_straw->const_iterator();
		while (index_straw_ci.hasNext()) {
			const MatchedObject *pIndex_mo = index_straw_ci.next();
			monad_m first_monad = pIndex_mo->getFirst();
			monad_m last_monad = pIndex_mo->getLast();
			
			lmdict[first_monad] = last_monad;
		}
	}
	
	
	
	delete pIndex_sheaf;
}


bool BookcaseHarvester2::getBigSOMs(const StringList& token_list, unsigned int token_list_length, monad_m largest_proximity, const int2SOMPairMap& mydict2, const std::map<int, int>& tokenindex2mydict_map, FastSetOfMonads& big_bookcase_som, FastSetOfMonads& first_feature_token_som)
{
	SetOfMonads mybig_bookcase_som = SetOfMonads(1, MAX_MONAD);
	SetOfMonads myfirst_feature_token_som = SetOfMonads();
	bool bFound = false;

	SetOfMonads offset_token_som;

	StringListConstIterator token_list_ci = token_list.const_iterator();
	unsigned int token_list_index = 0;
	while (token_list_ci.hasNext()) {
		std::string feature_value = token_list_ci.next();
		std::map<int, int>::const_iterator tokenindex_ci = tokenindex2mydict_map.find(token_list_index);
		ASSERT_THROW(tokenindex_ci != tokenindex2mydict_map.end(),
			     "Unknown token_list_index in tokenindex2mydict_map.");
		int mydict_index = tokenindex_ci->second;
		int2SOMPairMap::const_iterator mydict2_ci = mydict2.find(mydict_index);
		ASSERT_THROW(mydict2_ci != mydict2.end(),
			     "Unknown mydict_index in mydict2.");
		const SetOfMonads& bookcase_som = mydict2_ci->second.first;
		const SetOfMonads& token_som = mydict2_ci->second.second;
		SetOfMonads new_som = SetOfMonads::intersect(mybig_bookcase_som, bookcase_som);
		if (new_som.isEmpty()) {
			// Don't do anything to mybig_bookcase_som, since we might
			// get hits later in the list.  In particular,
			// don't set it to the empty set.
		} else {
			mybig_bookcase_som = new_som;
			if (token_list_index == 0) {
				myfirst_feature_token_som = token_som;
				offset_token_som = token_som;
				offset_token_som.offset(token_list_length);
			} else {
				if (largest_proximity <= 1) {
					SetOfMonads tmp_token_som = token_som;
					monad_m myoffset = token_list_length - token_list_index;
					if (myoffset != 0) {
						tmp_token_som.offset(myoffset);
					}
					SetOfMonads new_tmp_token_som = SetOfMonads::intersect(offset_token_som, tmp_token_som);
					if (new_tmp_token_som.isEmpty()) {

					} else {
						offset_token_som = new_tmp_token_som;

						// Only add those to
						// myfirst_feature_token_som
						// that pass the test
						// of being adjacent
						// to the rest.

						// But first, bring it
						// back to the right
						// offset, unless
						// we are at the last index.
						myfirst_feature_token_som = offset_token_som;
						myfirst_feature_token_som.offset(token_list_length - myoffset);
					}
				} else {
					// Don't do anything for this
					// case.  We only want
					// myfirst_feature_token_som
					// to be set for the first
					// feature.
				}
			}
			bFound = true;

		}
		++token_list_index;
	}

	big_bookcase_som = FastSetOfMonads(mybig_bookcase_som);
	first_feature_token_som = FastSetOfMonads(myfirst_feature_token_som);

	return bFound;
}

bool BookcaseHarvester2::getBigSOM(const StringList& token_list, unsigned int token_list_length, const int2SOMPairMap& mydict2, const std::map<int, int>& tokenindex2mydict_map, FastSetOfMonads& big_bookcase_som)
{
	UNUSED(token_list_length);
	
	big_bookcase_som = SetOfMonads(1, MAX_MONAD);
	bool bFound = false;

	StringListConstIterator token_list_ci = token_list.const_iterator();
	unsigned int token_list_index = 0;
	while (token_list_ci.hasNext()) {
		std::string feature_value = token_list_ci.next();
		std::map<int, int>::const_iterator tokenindex_ci = tokenindex2mydict_map.find(token_list_index);
		ASSERT_THROW(tokenindex_ci != tokenindex2mydict_map.end(),
			     "Unknown token_list_index in tokenindex2mydict_map.");
		int mydict_index = tokenindex_ci->second;
		int2SOMPairMap::const_iterator mydict2_ci = mydict2.find(mydict_index);
		ASSERT_THROW(mydict2_ci != mydict2.end(),
			     "Unknown mydict_index in mydict2.");
		const SetOfMonads& bookcase_som = mydict2_ci->second.first;
		SetOfMonads new_som = SetOfMonads::intersect(big_bookcase_som, bookcase_som);
		if (new_som.isEmpty()) {
			// Don't do anything to mybig_bookcase_som, since we might
			// get hits later in the list.  In particular,
			// don't set it to the empty set.
		} else {
			big_bookcase_som = new_som;
			bFound = true;

		}
		++token_list_index;
	}
	return bFound;
}




void BookcaseHarvester2::process(const SetOfMonads& substrate, const StringList& token_list, monad_m largest_proximity, bool bReduceToSingleHitsWithinBookcases, BookcaseHitList& result)
{
	UNUSED(substrate); // FIXME: Use the substrate!
	
	if (token_list.isEmpty()) {
		return;
	}

	eObjectRangeType objectRangeType;
	bool bObjectTypeExists;
	id_d_t object_type_id;
	eMonadUniquenessType monadUniquenessType;
	if (!m_pEnv->getMQLEE()->pDB->objectTypeExists(m_bookcase_OTN,
						      bObjectTypeExists,
						      object_type_id,
						      objectRangeType,
						      monadUniquenessType)) {
		ASSERT_THROW(false,
			     "Bookcase object type '" + m_bookcase_OTN + "' does not exist!");
	}

	if (!bObjectTypeExists) {
		ASSERT_THROW(false,
			     "Bookcase object type '" + m_bookcase_OTN + "' does not exist!");
	}

	if (!m_pEnv->getMQLEE()->pDB->getLargestObjectLength(m_bookcase_OTN, 
							     objectRangeType,
							     m_bookcase_largest_object_length)) {
		ASSERT_THROW(false,
			     "Could not get largest object length for bookcase object type '" + m_bookcase_OTN + "'");
	}

	// Make sure we don't look too far!
	bool bUseLargestProximity = true;
	if (largest_proximity <= 1) {
		bUseLargestProximity = false;
	} else if (largest_proximity >= m_bookcase_largest_object_length) {
		bUseLargestProximity = false;
	}

		
	typedef std::map<monad_m, monad_m> monad2monadMap;

	std::string index_OTN = make_index_name("_index2", m_bookcase_OTN, m_indexed_OTN, m_indexed_feature_name_list);

	// feature_value --> { first_monad --> SetOfMonads }
	typedef std::map<int, std::map<monad_m, SetOfMonads> > mydict_t;

        mydict_t mydict;

	int2SOMPairMap mydict2;
        
	// Turn the token list into a token set,
	// so as not to query for the same token more than once
	// if it appears more than once in the token_list.
	StringListConstIterator token_ci = token_list.const_iterator();
	std::map<std::string, int> tokenstring2int_map;
	std::map<int, std::string> mydictindex2tokenstring_map;
	std::map<int, int> tokenindex2mydict_map;
	int token_list_index = 0;
	int mydict_index = 0;
	while (token_ci.hasNext()) {
		std::string feature_value = token_ci.next();
		std::map<std::string, int>::const_iterator ci = tokenstring2int_map.find(feature_value);
		if (ci == tokenstring2int_map.end()) {
			int index = mydict_index;
			tokenstring2int_map.insert(std::make_pair(feature_value, index));
			tokenindex2mydict_map.insert(std::make_pair(token_list_index, index));
			mydictindex2tokenstring_map.insert(std::make_pair(index, feature_value));
			++mydict_index;
		} else {
			int index = ci->second;
			tokenindex2mydict_map.insert(std::make_pair(token_list_index, index));
			mydictindex2tokenstring_map.insert(std::make_pair(index, feature_value));
		}
		++token_list_index;
	}
	std::map<int, std::string>::const_iterator ci1 = mydictindex2tokenstring_map.begin();
	while (ci1 != mydictindex2tokenstring_map.end()) {
		std::string feature_value = ci1->second;
		int mydict_index = ci1->first;
		++ci1;

		mydict2.insert(std::make_pair(mydict_index, std::make_pair(SetOfMonads(), SetOfMonads())));


		std::string query;
		if (feature_value.find_first_of("*?") == std::string::npos) {
			query = "SELECT ALL OBJECTS WHERE [" + index_OTN + " feature_value=\"" + encodeSTRINGstring(feature_value) + "\" GET monad_set]";
		} else {
			std::string feature_value_to_use = replace_substring(feature_value, "\\", "\\\\");
			feature_value_to_use = replace_substring(feature_value_to_use, ".", "\\.");
			feature_value_to_use = replace_substring(feature_value_to_use, "[", "\\[");
			feature_value_to_use = replace_substring(feature_value_to_use, "]", "\\]");
			feature_value_to_use = replace_substring(feature_value_to_use, "^", "\\^");
			feature_value_to_use = replace_substring(feature_value_to_use, "$", "\\$");
			feature_value_to_use = replace_substring(feature_value_to_use, "+", ".+");
			feature_value_to_use = replace_substring(feature_value_to_use, "*", ".*");
			feature_value_to_use = replace_substring(feature_value_to_use, "?", ".");
			query = "SELECT ALL OBJECTS WHERE [" + index_OTN + " feature_value~\"^" + encodeSTRINGstring(feature_value_to_use) + "$\" GET monad_set]";
		}
		Sheaf *pIndex_sheaf = 0;
		bool bResult = true;

		if (!m_pEnv->executeString(query,
					  bResult,
					  false, // bPrintResult
					  false  // bReportError
					  )) {
			bResult = false;
			ASSERT_THROW(false, "Wasn't able to execute query for harvesting.");
		}
		if (!bResult) {
			ASSERT_THROW(false, "Wasn't able to execute query for harvesting.");
		}

		pIndex_sheaf = m_pEnv->takeOverSheaf();

		SheafConstIterator index_sheaf_ci = pIndex_sheaf->const_iterator();
		
		while (index_sheaf_ci.hasNext()) {
			const Straw *pIndex_straw = index_sheaf_ci.next();
			StrawConstIterator index_straw_ci = pIndex_straw->const_iterator();
			while (index_straw_ci.hasNext()) {
				const MatchedObject *pIndex_mo = index_straw_ci.next();
				SetOfMonads index_som = pIndex_mo->getMonads();
				//monad_m first_monad = pIndex_mo->getFirst();
				//monad_m last_monad = pIndex_mo->getLast();
				SetOfMonads token_som = pIndex_mo->getEMdFValue(0)->getSOM();

				mydict2[mydict_index].first.unionWith(index_som);
				mydict2[mydict_index].second.unionWith(token_som);
			}
		}

		delete pIndex_sheaf;
	}

	// Find out whether there is more than one
	StringListConstIterator token_list_ci = token_list.const_iterator();
	unsigned int token_list_size = 0;
	while (token_list_ci.hasNext()) {
		++token_list_size;
		token_list_ci.next();
	}

	if (token_list_size == 1) {
		token_list_ci = token_list.const_iterator();
		std::string feature_value = token_list_ci.current();
		int mydict_index = tokenindex2mydict_map[0];
		const SetOfMonads& bookcase_som = mydict2[mydict_index].first;
		const SetOfMonads& token_som = mydict2[mydict_index].second;

		if (bookcase_som.isEmpty()
		    || token_som.isEmpty()) {
			return;
		}

		monad2monadMap lmdict;
		getLMDict(bookcase_som, lmdict);
			
		monad2monadMap::const_iterator bookcase_fmlm_ci = lmdict.begin();
		monad2monadMap::const_iterator bookcase_fmlm_cend = lmdict.end();
		SOMConstIterator ffvsom_ci = token_som.const_iterator();
		while (bookcase_fmlm_ci != bookcase_fmlm_cend) {
			monad_m first_monad = bookcase_fmlm_ci->first;
			monad_m last_monad = bookcase_fmlm_ci->second;
			++bookcase_fmlm_ci;

			std::set<monad_m> token_monads_to_hit;

			bool bContinue = true;
			while (bContinue && ffvsom_ci.hasNext()) {
				MonadSetElement ffvsom_mse = ffvsom_ci.current();
				monad_m token_first_monad = ffvsom_mse.first();
				monad_m token_last_monad = ffvsom_mse.last();

				//std::cerr << "UP240: bookcase_mse = (" << first_monad << "," << last_monad << ") and token_mse = (" << token_first_monad << "," << token_last_monad << ")" << std::endl;

				bool bDoAdd = false;
				bool bAdvance = false;

				monad_m fm_add;
				monad_m lm_add;

				token_decision(token_first_monad, token_last_monad, first_monad, last_monad, bDoAdd, bAdvance, bContinue, fm_add, lm_add);

				if (bDoAdd) {
					monad_m m;
					for (m = fm_add;
					     m <= lm_add;
					     ++m) {
						token_monads_to_hit.insert(m);
						//std::cerr << "UP241: bookcase_mse = (" << first_monad << "," << last_monad << ") and add_mse = (" << fm_add << "," << lm_add << ") adding monad " << m << std::endl;

						// Only add one if we are to reduce it to single bookcase hit.
						if (bReduceToSingleHitsWithinBookcases) {
							break;
						}
					}
				}

				if (bAdvance) {
					ffvsom_ci.next();
				}
			}
			
			std::set<monad_m>::const_iterator hit_monad_ci = token_monads_to_hit.begin();
			std::set<monad_m>::const_iterator hit_monad_cend = token_monads_to_hit.end();
			while (hit_monad_ci != hit_monad_cend) {
				result.addHit(first_monad, last_monad, *hit_monad_ci);
				// Only add one if we are to reduce it to single bookcase hit.
				if (bReduceToSingleHitsWithinBookcases) {
					break;
				}
				
				
				++hit_monad_ci;
			}
		}
	} else {
		FastSetOfMonads big_bookcase_som;
		bool bFound = getBigSOM(token_list, token_list_size, mydict2, tokenindex2mydict_map, big_bookcase_som);
			
		if (!bFound) {
			// Return without altering result, i.e.,
			// return empty hits.
			return;
		} else {
			monad2monadMap lmdict;
			getLMDict(big_bookcase_som, lmdict);

			// Now set up a vector of SOMConstIterator
			// objects which point to each of the
			// token_som SetOfMonads which correspond to
			// that particular token in the token_list.
			std::vector<SOMConstIterator> sci_vec;
			std::vector<std::set<monad_m> > seom_vec;

			token_list_ci = token_list.const_iterator();
			unsigned int token_list_index = 0;
			while (token_list_ci.hasNext()) {
				std::string feature_value = token_list_ci.next();
				int mydict_index = tokenindex2mydict_map[token_list_index];
				
				const SetOfMonads& token_som = mydict2[mydict_index].second;

				sci_vec.push_back(token_som.const_iterator());

				seom_vec.push_back(std::set<monad_m>());

				++token_list_index;
			}
			
			
			monad2monadMap::const_iterator bookcase_fmlm_ci = lmdict.begin();
			monad2monadMap::const_iterator bookcase_fmlm_cend = lmdict.end();
			while (bookcase_fmlm_ci != bookcase_fmlm_cend) {
				monad_m first_monad = bookcase_fmlm_ci->first;
				monad_m last_monad = bookcase_fmlm_ci->second;
				++bookcase_fmlm_ci;

				std::set<monad_m> first_token_monads_to_hit;
				
				bool bDoAddFirstTokens = true;

				unsigned int token_list_index = 0;
				while (token_list_index < token_list_size) {

					seom_vec[token_list_index] = std::set<monad_m>();

					std::set<monad_m>& token_monads_to_hit = seom_vec[token_list_index];

					// We use a reference so that it is
					// updated!
					SOMConstIterator& ffvsom_ci = sci_vec[token_list_index];

			
					bool bContinue = true;
					while (bContinue && ffvsom_ci.hasNext()) {
						MonadSetElement ffvsom_mse = ffvsom_ci.current();
						monad_m token_first_monad = ffvsom_mse.first();
						monad_m token_last_monad = ffvsom_mse.last();
						
						//std::cerr << "UP240: bookcase_mse = (" << first_monad << "," << last_monad << ") and token_mse = (" << token_first_monad << "," << token_last_monad << ")" << std::endl;
						
						bool bDoAdd = false;
						bool bAdvance = false;
						
						monad_m fm_add;
						monad_m lm_add;
						
						token_decision(token_first_monad, token_last_monad, first_monad, last_monad, bDoAdd, bAdvance, bContinue, fm_add, lm_add);

						if (bDoAdd) {
							monad_m m;
							for (m = fm_add;
							     m <= lm_add;
							     ++m) {
								token_monads_to_hit.insert(m);
								//std::cerr << "UP241: bookcase_mse = (" << first_monad << "," << last_monad << ") and add_mse = (" << fm_add << "," << lm_add << ") adding monad " << m << std::endl;
								
								// Only add one if we are to reduce it to single bookcase hit.
								if (bReduceToSingleHitsWithinBookcases) {
									break;
								}
							}
						} 
						
						if (bAdvance) {
							ffvsom_ci.next();
						}
					}


					if (token_monads_to_hit.empty()) {
						bDoAddFirstTokens = false;
						break;
					} else if (token_list_index == 0) {
						first_token_monads_to_hit = token_monads_to_hit;
					}

					++token_list_index;
				}
			
				if (bDoAddFirstTokens
				    && bUseLargestProximity
				    && token_list_size > 1) {
					unsigned int token_list_index = 0;
					SetOfMonads big_token_monads_to_hit;

					while (token_list_index < token_list_size) {
						std::set<monad_m> token_monads_to_hit = seom_vec[token_list_index];
						
						std::set<monad_m>::const_iterator this_ci = token_monads_to_hit.begin();
						std::set<monad_m>::const_iterator this_cend = token_monads_to_hit.end();

						while (this_ci != this_cend) {
							monad_m this_m = *this_ci++;
							big_token_monads_to_hit.add(this_m);
							
						}

						++token_list_index;
					}

					if (big_token_monads_to_hit.hasLargerGap(largest_proximity)) {
						bDoAddFirstTokens = false;
					} 
				}
				if (bDoAddFirstTokens) {
					std::set<monad_m>::const_iterator hit_monad_ci = first_token_monads_to_hit.begin();
					std::set<monad_m>::const_iterator hit_monad_cend = first_token_monads_to_hit.end();
					while (hit_monad_ci != hit_monad_cend) {
						result.addHit(first_monad, last_monad, *hit_monad_ci);
						// Only add one if we are to reduce it to single bookcase hit.
						if (bReduceToSingleHitsWithinBookcases) {
							break;
						}
						
						
						++hit_monad_ci;
					}
				}
			}
		}

	//////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////

		/*

			// Now get a masterBookcaseFMSOM which has only the
			// first_monads of those bookcases which have all tokens
			// in the token_list.  The reason we do this is that
			// we wish to have "and" between them.
			FastSetOfMonads masterBookcaseFMSOM;

			typedef std::map<monad_m, FastSetOfMonads> monad2FSOMMap;

			monad2FSOMMap bookcaseFM2TokenFMmap;

			monad2monadMap::const_iterator bookcase_fmlm_ci = lmdict.begin();
			monad2monadMap::const_iterator bookcase_fmlm_cend = lmdict.end();
			while (bookcase_fmlm_ci != bookcase_fmlm_cend) {
				monad_m bookcase_first_monad = bookcase_fmlm_ci->first;
				monad_m bookcase_last_monad = bookcase_fmlm_ci->second;
				token_list_ci = token_list.const_iterator();
				unsigned int token_list_index = 0;
				bool bFound = true;
				monad_m min_token_monad = 0;
				monad_m max_token_monad = 0;
				while (token_list_ci.hasNext()) {
					std::string feature_value = token_list_ci.next();
					int mydict_index = tokenindex2mydict_map[token_list_index];
				
					const SetOfMonads& token_som = mydict2[mydict_index].second;
					
					if (!token_som.hasMonadsInRange(bookcase_first_monad, bookcase_last_monad)) {
						bFound = false;
						break;
					} 

					++token_list_index;
				}

				if (bFound) {
					masterBookcaseFMSOM.add(bookcase_first_monad);
				}

				++bookcase_fmlm_ci;
			}

			if (bUseLargestProximity) {
				// Now reduce it further by taking only those that meet the largest_proximity criterion
				std::vector<SOMConstIterator> sci_vec;

				FastSetOfMonads masterBookcaseFMSOM2;
				FastSOMConstIterator masterFSOM_ci = masterBookcaseFMSOM.const_iterator();
				while (masterFSOM_ci.hasNext()) {
					MonadSetElement masterBookcaseFMMSE = masterFSOM_ci.next();
					monad_m bookcase_fm;
					for (bookcase_fm = masterBookcaseFMMSE.first();
					     bookcase_fm <= masterBookcaseFMMSE.last();
					     ++bookcase_fm) {
						//std::cerr << "UP240: token_fm_som = " << token_fm_som.toString() << std::endl;
						// TODO: DO SOMETHING
					}
				}
				masterBookcaseFMSOM = masterBookcaseFMSOM2;
			}


			FastSOMConstIterator ffvsom_ci = first_feature_token_som.const_iterator();
			FastSOMConstIterator masterFSOM_ci = masterBookcaseFMSOM.const_iterator();
			while (masterFSOM_ci.hasNext()) {
				MonadSetElement masterBookcaseFMMSE = masterFSOM_ci.next();

				monad_m bookcase_fm;
				for (bookcase_fm = masterBookcaseFMMSE.first();
				     bookcase_fm <= masterBookcaseFMMSE.last();
				     ++bookcase_fm) {
					monad_m first_monad = bookcase_fm;
					monad_m last_monad = lmdict[bookcase_fm];

					//std::cerr << "UP250: bookcase_fm = " << bookcase_fm << ", masterBookcaseFMMSE.first() = " << masterBookcaseFMMSE.first() << ", and last = " << masterBookcaseFMMSE.last() << std::endl;
					
					
					// Gather which ones we should add for this one.
					std::set<monad_m> token_monads_to_hit;
					
					bool bContinue = true;
					while (bContinue && ffvsom_ci.hasNext()) {
						MonadSetElement ffvsom_mse = ffvsom_ci.current();
						monad_m token_first_monad = ffvsom_mse.first();
						monad_m token_last_monad = ffvsom_mse.last();
						if (first_monad <= token_last_monad
						    && last_monad >= token_first_monad) {
							monad_m token_min_monad = MYMAX(first_monad, token_first_monad);
							monad_m token_max_monad = MYMIN(last_monad, token_last_monad);
							monad_m m;
							for (m = token_min_monad;
							     m <= token_max_monad;
							     ++m) {
								token_monads_to_hit.insert(m);
								// Only add one if we are to reduce it to single bookcase hit.
								if (bReduceToSingleHitsWithinBookcases) {
									break;
								}
							}
							
							ffvsom_ci.next();
						} else if (token_first_monad > last_monad) {
							bContinue = false;
						} else if (token_first_monad < first_monad) {
							ffvsom_ci.next();
						} else {
							bContinue = false;
						}
					}

					std::set<monad_m>::const_iterator hit_monad_ci = token_monads_to_hit.begin();
					std::set<monad_m>::const_iterator hit_monad_cend = token_monads_to_hit.end();
					while (hit_monad_ci != hit_monad_cend) {
						result.addHit(first_monad, last_monad, *hit_monad_ci);
						// Only add one if we are to reduce it to single bookcase hit.
						if (bReduceToSingleHitsWithinBookcases) {
							break;
						}
						
						
						++hit_monad_ci;
					}
				}
			}
			}
		*/
	}
}

