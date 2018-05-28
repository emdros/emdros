/*
 * harvest.cpp
 *
 * A class to "harvest" a sheaf.
 *
 * Ulrik Petersen
 * Created: 4/9-2005
 * Last update: 11/10-2017
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2005-2017  Ulrik Sandborg-Petersen
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

#include <emdros.h>

#include "emdros_wrapper.h"
#include "harvest.h"


Harvest::Harvest(EmdrosWrapper *pEWrap, Sheaf *pSheaf)
{
	m_pEW = pEWrap;
	m_pSheaf = pSheaf;
	m_pConf = pEWrap->getPConf();

	SetOfMonadsList *pSeparated_solution_monad_list = new SetOfMonadsList();
	SetOfMonads unified_solution_monad_set;
	SetOfMonads *pFocusMonadSet;

	m_hit_type= kHitTypeOutermost;
	if (m_pConf->has_key("hit_type")) {
		std::string hit_type = *(m_pConf->getValues("hit_type").begin());
		if (strcmp_nocase(hit_type, "focus") == 0) {
			m_hit_type = kHitTypeFocus;
		} else if (strcmp_nocase(hit_type, "innermost") == 0) {
			m_hit_type = kHitTypeInnermost;
		} else if (strcmp_nocase(hit_type, "innermost_focus") == 0) {
			m_hit_type = kHitTypeInnermostFocus;
		} else if (strcmp_nocase(hit_type, "outermost") == 0) {
			m_hit_type = kHitTypeOutermost;
		} else {
			m_hit_type = kHitTypeOutermost;
		}

	}


	sheaf_monads_separated(m_pSheaf, m_hit_type, true, pSeparated_solution_monad_list);

	m_pEW->printIfNotQuiet(long2string(pSeparated_solution_monad_list->size()) + " solutions found.");

	// Calculate big-union of all monad sets in separated_solution_monad_list
	SetOfMonadsListConstIterator ssml_ci = 
		pSeparated_solution_monad_list->const_iterator();
	while (ssml_ci.hasNext()) {
		unified_solution_monad_set.unionWith(*(ssml_ci.next()));
	}

	if (unified_solution_monad_set.isEmpty()) {
		m_pEW->printIfNotQuiet("No monads arose from the query. Thus, no solutions to display.");
		
		pSeparated_solution_monad_list->clear(true);
		delete pSeparated_solution_monad_list;

		return;

	}

	m_pEW->printIfNotQuiet("Now querying backend for units...");
  
	if (m_pConf->has("option", "apply_focus")) {
		pFocusMonadSet = new SetOfMonads(sheaf_focus(m_pSheaf));
	} else {
		pFocusMonadSet = 0;
	}

	bool bOnlySingleRasterUnits;
	if (m_pConf->has("option", "single_raster_units")) {
		bOnlySingleRasterUnits = true;
	} else {
		bOnlySingleRasterUnits = false;
	}

	// Get raster unit
	MyTable *pRasterTable = 0;
	std::string raster_unit;
	if (m_pConf->has_key("raster_unit")) {
		raster_unit = *(m_pConf->getValues("raster_unit").begin());
		pRasterTable = getTable(unified_solution_monad_set, raster_unit, StringVec());
	} else {
		raster_unit = "pow_m";
		std::string raster_context_before = m_pConf->getValues("raster_context_before")[0];
		std::string raster_context_after  = m_pConf->getValues("raster_context_after")[0];

		monad_m nRasterContextBefore = string2monad_m(raster_context_before)
;		monad_m nRasterContextAfter  = string2monad_m(raster_context_after);
		
		
		pRasterTable = getRasterTableWithContext(pSeparated_solution_monad_list, nRasterContextBefore, nRasterContextAfter);
	}

	SetOfMonads raster_monads(maximal_set(pRasterTable, unified_solution_monad_set));

	m_pEW->getPOut()->MyYield();
	if (m_pEW->getPOut()->ShouldStop()) {
		delete pRasterTable;
		delete pFocusMonadSet;

		pSeparated_solution_monad_list->clear(true);
		delete pSeparated_solution_monad_list;

		return;
	}

	// Get reference unit

	// There should be only one, so we don't care about any others
	MyTable *pRefUnitTable;
	std::string refunit;
	if (m_pConf->has_key("reference_unit")) {
		refunit = *(m_pConf->getValues("reference_unit").begin());
		pRefUnitTable = getTable(raster_monads, refunit, m_pConf->getSubkeyValues("reference_feature", refunit));
	} else {
		refunit = "";
		pRefUnitTable = 0;
	}
  
	m_pEW->getPOut()->MyYield();
	if (m_pEW->getPOut()->ShouldStop()) {
		delete pRasterTable;
		delete pFocusMonadSet;
		delete pRefUnitTable;
		
		pSeparated_solution_monad_list->clear(true);
		delete pSeparated_solution_monad_list;

		return;
	}

	// Get data unit
	m_pEW->getPOut()->MyYield();
	StringVec data_unit_set = m_pConf->getValues("data_unit");
	MyTablePtrVec data_unit_table_vec;
	for (StringVec::const_iterator dataunit_ci = data_unit_set.begin();
	     dataunit_ci != data_unit_set.end()
		     && !m_pEW->getPOut()->ShouldStop();
	     ++dataunit_ci) {
		std::string dataunit = *dataunit_ci;
		StringVec features_to_get = m_pConf->getSubkeyValues("data_feature", dataunit);
		if (m_pConf->has_subkey("data_tree_parent", dataunit)
		    && m_pConf->has_key("output_style")
		    && *m_pConf->getValues("output_style").begin() == "tree") {
			features_to_get.push_back(m_pConf->getSubkeyValues("data_tree_parent", dataunit)[0]);
		}

		MyTable *pT = getTable(raster_monads, dataunit, features_to_get);
		data_unit_table_vec.push_back(pT);
	}

	m_pEW->printIfNotQuiet("...Done querying backend... now calculating solutions...");
  
	m_pEW->getPOut()->MyYield();
	SetOfMonadsListConstIterator somci 
		= pSeparated_solution_monad_list->const_iterator();
	while (somci.hasNext() && !m_pEW->getPOut()->ShouldStop()) {
		m_pEW->getPOut()->MyYield();
		Solution *pSolution = new Solution();

		// Get raster unit
		SetOfMonads raster_monads_this_solution = overlap_set(pRasterTable, *(somci.current()));
		MyTable *pT = new MyTable(pRasterTable, raster_monads_this_solution);
    

		bool bDoIt = false;
		if (bOnlySingleRasterUnits) {
			pT->set_universe_all();

			// We use objects_seen_now to guard against
			// setting bDoIt to false
			// if any object spans more than one row.
			std::set<id_d_t> objects_seen_now;
			while (pT->hasNext()) {
				id_d_t raster_id_d = pT->next().getID_D();

				if (m_raster_id_ds.find(raster_id_d) == m_raster_id_ds.end()) {
					m_raster_id_ds.insert(raster_id_d);
					objects_seen_now.insert(raster_id_d);
					bDoIt = true;
				} else if (objects_seen_now.find(raster_id_d) == objects_seen_now.end()) {
					bDoIt = false;
					break;
				}
			}
		} else {
			bDoIt = true;
		}

		if (!bDoIt) {
			delete pT; // Raster table
			delete pSolution;
		} else {
			pSolution->addMyTable(kRaster, raster_unit, pT);

			//pSolution->setMonads(*(somci.current()));
			pSolution->setMonads(raster_monads_this_solution);
			if (pFocusMonadSet != 0) {
				SetOfMonads *pF = new SetOfMonads(SetOfMonads::intersect(raster_monads_this_solution, *pFocusMonadSet));
				pSolution->setFocus(pF);
			} else {
				pSolution->setFocus(0);
			}


			m_pEW->getPOut()->MyYield();

			if (pRefUnitTable != 0) {
				// Get reference unit
				pT = new MyTable(pRefUnitTable, raster_monads_this_solution);
				pSolution->addMyTable(kReference, refunit, pT);
			} else {
				pSolution->addMyTable(kReference, "", 0);
			}
    
			// Get data units
			StringVec data_unit_set = m_pConf->getValues("data_unit");
			unsigned int myindex = 0;
			for (StringVec::const_iterator dataunit_ci = data_unit_set.begin();
			     dataunit_ci != data_unit_set.end()
				     && !m_pEW->getPOut()->ShouldStop();
			     ++dataunit_ci) {
				m_pEW->getPOut()->MyYield();
				std::string dataunit = *dataunit_ci;
				MyTable *pT = new MyTable(data_unit_table_vec[myindex], raster_monads_this_solution);
				pSolution->addMyTable(kData, dataunit, pT);
				++myindex;
			}
    
			m_solutions.push_back(pSolution);
		}
		somci.next();
	}

	// Clean up
	delete pFocusMonadSet;

	for (unsigned int myindex2 = 0; myindex2 < data_unit_table_vec.size(); ++myindex2) {
		delete data_unit_table_vec[myindex2];
	}
  
	delete pRasterTable;
	delete pRefUnitTable;

	pSeparated_solution_monad_list->clear(true);
	delete pSeparated_solution_monad_list;
	

	if (bOnlySingleRasterUnits) {
		m_pEW->printIfNotQuiet(long2string(m_solutions.size()) + " unique raster units found.");
	}
}

Harvest::~Harvest()
{
	m_solutions.clear(true);
}


void Harvest::sheaf_monads_separated(const Sheaf *pSheaf, eHarvestHitType hit_type, bool bIsOutermost, SetOfMonadsList *pSom_List)
{
	SheafConstIterator shci = pSheaf->const_iterator();
	while (shci.hasNext()) {
		const Straw *pStraw = shci.next();
		if (bIsOutermost) {
			if (pSom_List->empty()) {
				pSom_List->push_back(new SetOfMonads());
			} else {
				SetOfMonads *pSOM = pSom_List->back();
				if (pSOM->isEmpty()) {
					; // Nothing to do.
					// reuse the one from the last iteration
				} else {
					// Last one wasn't empty,
					// so we add another one.
					pSom_List->push_back(new SetOfMonads());
				}
			}
		}
		straw_monads_separated(pStraw, hit_type, pSom_List);
	}

	if (bIsOutermost
	    && !pSom_List->empty()
	    && pSom_List->back()->isEmpty()) {
		// Remove last one, since it was empty.
		SetOfMonadsListIterator lit = pSom_List->iterator();
		while (true) {
			SetOfMonadsListIterator lit2 = lit;
			lit.next();
			if (!lit.hasNext()) {
				pSom_List->erase(lit2);
				break;
			}
		}
	}
}

void Harvest::straw_monads_separated(const Straw *pStraw, eHarvestHitType hit_type, SetOfMonadsList *pSom_List)
{
	if (hit_type == kHitTypeOutermost) {
		SetOfMonads *pStraw_som = pSom_List->back();
		StrawConstIterator strci = pStraw->const_iterator();
		while (strci.hasNext()) {
			pStraw_som->unionWith((strci.next())->getMonads());
		}
	} else if (hit_type == kHitTypeInnermost
		   || hit_type == kHitTypeInnermostFocus) {
		bool bAllAreInnermost = true;
		StrawConstIterator strci = pStraw->const_iterator();
		while (strci.hasNext()) {
			const MatchedObject *pMo = strci.next();
			if (!pMo->sheafIsEmpty()) {
				bAllAreInnermost = false;
				break;
			}
		}
		if (bAllAreInnermost) {
			SetOfMonads *pSOM = pSom_List->back();
			strci = pStraw->const_iterator();
			while (strci.hasNext()) {
				const MatchedObject *pMo = strci.next();
				bool bAddIt = false;
				if (hit_type == kHitTypeInnermostFocus) {
					bAddIt = pMo->getFocus();
				} else { // kHitTypeInnermost
					bAddIt = true;
				}
				if (bAddIt) {
					pSOM->unionWith(pMo->getMonads());
				}
			}
			if (!pSOM->isEmpty()) {
				pSom_List->push_back(new SetOfMonads());
			}
		} else {
			strci = pStraw->const_iterator();
			while (strci.hasNext()) {
				const MatchedObject *pMo = strci.next();
				if (!pMo->sheafIsEmpty()) {
					const Sheaf *pInnerSheaf = pMo->getSheaf();
					sheaf_monads_separated(pInnerSheaf, 
							       hit_type,
							       false,
							       pSom_List);
				}
			}
		}
	} else if (hit_type == kHitTypeFocus) {
		StrawConstIterator strci = pStraw->const_iterator();
		while (strci.hasNext()) {
			const MatchedObject *pMo = strci.next();
			if (pMo->getFocus()) {
				SetOfMonads *pSOM = pSom_List->back();
				pSOM->unionWith(pMo->getMonads());
				pSom_List->push_back(new SetOfMonads());
			}
			if (!pMo->sheafIsEmpty()) {
				const Sheaf *pInnerSheaf = pMo->getSheaf();
				sheaf_monads_separated(pInnerSheaf, 
						       hit_type,
						       false,
						       pSom_List);
			}
		}
	} else {
		ASSERT_THROW(false, 
			     "Unknown hit type (second instance) in Harvest::straw_monads_separated.");
	}
}

SetOfMonads Harvest::maximal_set(MyTable *pTable, SetOfMonads mset)
{
	SetOfMonads s(mset);
	pTable->set_universe_all();
	while (pTable->hasNext()) {
		const MyRow& r = pTable->next();
		s.add(r.getFirstMonad(), r.getLastMonad());
	}
	return s;
}

SetOfMonads Harvest::overlap_set(MyTable *pTable, SetOfMonads mset)
{
	SetOfMonads s;

	// Iterate over the mset
	//SOMConstIterator somci = mset.const_iterator();

	pTable->set_universe(mset.first(), mset.last());
	if (!pTable->hasNext()) {
		pTable->set_universe_all();
	}

	//while (somci.hasNext()) {
	//MonadSetElement sommse = somci.next();

	//pTable->set_universe(sommse.first(), sommse.last());

	std::set<id_d_t> id_d_s_overlapping_set;

	MonadSetElement msetmse(mset.first(), mset.last());

	while (pTable->hasNext()) {
		const MyRow& row = pTable->next();
		
		// Check if there is overlap
		MonadSetElement rowmse(row.getFirstMonad(), row.getLastMonad());
		if (rowmse.overlap(msetmse)) {
			id_d_s_overlapping_set.insert(row.getID_D());
		}
	}

	pTable->set_universe(mset.first(), mset.last());
	if (!pTable->hasNext()) {
		pTable->set_universe_all();
	}

	while (pTable->hasNext()) {
		const MyRow& row = pTable->next();
		
		if (id_d_s_overlapping_set.find(row.getID_D()) != id_d_s_overlapping_set.end()) {
			// Yes, so add it.
			MonadSetElement rowmse(row.getFirstMonad(), row.getLastMonad());
			s.addMSE(rowmse);
		}
	}

	return s;
}

SetOfMonads Harvest::sheaf_focus(const Sheaf *pSheaf)
{
	SetOfMonads som;
	SheafConstIterator shci = pSheaf->const_iterator();
	while (shci.hasNext()) {
		const Straw *pStraw = shci.next();
		som.unionWith(straw_focus(pStraw));
	}
	return som;
}

SetOfMonads Harvest::straw_focus(const Straw *pStraw)
{
	SetOfMonads straw_som;
	StrawConstIterator strci = pStraw->const_iterator();
	while (strci.hasNext()) {
		const MatchedObject *pMo = strci.next();
		if (pMo->getFocus()) {
			straw_som.unionWith(pMo->getMonads());
		} 
		if (!pMo->sheafIsEmpty()) {
			const Sheaf *pInnerSheaf = pMo->getSheaf();
			straw_som.unionWith(sheaf_focus(pInnerSheaf));
		}
	}
	return straw_som;
}

MyTable *Harvest::getRasterTableWithContext(SetOfMonadsList *pSeparated_solution_monad_list, monad_m nRasterContextBefore, monad_m nRasterContextAfter)
{
	MyTable *pResult = new MyTable(StringVec());
	
	SetOfMonadsListConstIterator slci = pSeparated_solution_monad_list->const_iterator();
	id_d_t fake_raster_id_d = 1;
	while (slci.hasNext()) {
		const SetOfMonads *pSOM = slci.next();
		monad_m first = pSOM->first() - nRasterContextBefore;
		if (first < 1) {
			first = 1;
		}

		monad_m last = pSOM->last() + nRasterContextAfter;
		if (last > MAX_MONAD) {
			last = MAX_MONAD;
		}

		MyRow row(fake_raster_id_d, first, last);

		++fake_raster_id_d;

		pResult->addRow(row);
	}

	return pResult;
}

// ############## getting a table with adresses and features #######

MyTable *Harvest::getTable(const SetOfMonads& mset, 
			   const std::string& unit, 
			   const StringVec& features)
{
	unsigned int features_size = features.size();
	bool bAddFeatures = features_size > 0;
	// Add features 
	std::string feature_string;
	if (bAddFeatures) {
		StringVec::const_iterator feat_ci = features.begin();
		feature_string = *feat_ci;
		++feat_ci;
		while (feat_ci != features.end()) {
			feature_string += ", " + *feat_ci;
			++feat_ci;
		}
	}

	std::string query = "GET OBJECTS HAVING MONADS IN " + mset.toString() + "[" + unit;
	if (bAddFeatures) {
		query += " GET " + feature_string;
	}
	query += "] GO";

	if (!m_pEW->executeString(query)) {
		throw EmdrosException("Error executing query:\n" + query + "\n");
	}
	FlatSheaf *pFlatSheaf = m_pEW->takeOverFlatSheaf();
	FlatSheafConstIterator fshci = pFlatSheaf->const_iterator();
	FlatStraw *pFlatStraw = fshci.current(); // There is only 1, since it is a flat sheaf from GET OBJECTS with only one object type
	FlatStrawConstIterator fstrci = pFlatStraw->const_iterator();

	MyTable *pResult = new MyTable(features);
	while (fstrci.hasNext()) {
		MatchedObject *pMo = fstrci.next();
		id_d_t id_d = pMo->getID_D();
		SetOfMonads som = pMo->getMonads();
		SOMConstIterator somci = som.const_iterator();
		while (somci.hasNext()) {
			MonadSetElement mse = somci.next();
			MyRow row(id_d, mse.first(), mse.last());
      
			if (bAddFeatures) {
				for (unsigned myindex = 0; myindex < features_size; ++myindex) {
					std::string value_string;
					value_string = pMo->getFeatureAsString(myindex);

					// If it is a list of enum, or a list of
					// id_d, or a list of integer,
					// we should strip off the beginning
					// '(' and the trailing ')',
					// simply because it looks bad to have
					// () () () () () ()
					// whenever the list is empty.
					const EMdFValue *pValue = pMo->getEMdFValue(myindex);
					if (pValue->getKind() == kEVListOfInteger
					    || pValue->getKind() == kEVListOfID_D) {
						value_string = value_string.substr(1, value_string.length() - 2);
					}
					row.appendFeature(value_string);
				}
			}
      
			pResult->addRow(row);
		}
	}

	delete pFlatSheaf;

	return pResult;
}

