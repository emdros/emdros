/*
 * output_style.cpp
 *
 * Base class for all output styles.
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
#include <emdros-lconfig.h>
#include <emdros.h>

#include "output_style.h"

#include <sstream>
#include <algorithm>


Output_style::Output_style(Configuration *pConf, QTOutputBase* pOut)
{
	m_pConf = pConf;
	m_bApplyFocus = m_pConf->has("option", "apply_focus");
	m_bFocus_on = false;
	m_bBreakAfterRaster = m_pConf->has("option", "break_after_raster");
	m_pOut = pOut;
}

void Output_style::writeStr(const std::string& str)
{
	(*m_pOut) << str;
}


void Output_style::turn_focus_off(void)
{
	if (m_bFocus_on) {
		m_pOut->endFocus();
		m_bFocus_on = false;
	}
}

void Output_style::show(const Solution *pSolution, int solution_number)
{
	start_solution(solution_number);

	m_bFocus_on = false;
	const OTTableMap& raster = pSolution->getOTTableMap(kRaster);
	for (OTTableMap::const_iterator ottm_ci = raster.begin();
	     ottm_ci != raster.end()
		     && !m_pOut->ShouldStop();
	     ++ottm_ci) {
		do_raster_unit(ottm_ci->second, // MyTable for this object type
			       pSolution->getOTTableMap(kReference),
			       pSolution->getOTTableMap(kData),
			       pSolution->getMonads(),
			       pSolution->getFocus());
	}
	end_solution();
}

void Output_style::do_raster_unit(MyTable *ras_list, 
				  const OTTableMap& ref_list, 
				  const OTTableMap& dat_list, 
				  const SetOfMonads& monads, 
				  const SetOfMonads& focus)
{
	start_raster_unit();
	ras_list->set_universe(monads.first(),monads.last());
	while (ras_list->hasNext()
	       && !m_pOut->ShouldStop()) {
		start_raster_unit_row();
		const MyRow& ras_row = ras_list->next();
		do_reference(ras_row.getFirstMonad(), "reference_unit", ref_list);

		for (monad_m m = ras_row.getFirstMonad();
		     m <= ras_row.getLastMonad()
			     && !m_pOut->ShouldStop();
		     ++m) {
			do_data(m, std::string("data_unit"), dat_list, monads, focus);
		}
		turn_focus_off();
		end_raster_unit_row();
	}
	turn_focus_off();
	end_raster_unit();
}

class object_sort_pred {
public:
	bool operator()(const std::pair<MyRow, std::string>& O1, const std::pair<MyRow, std::string>& O2) {
		monad_m o1_first = O1.first.getFirstMonad();
		monad_m o1_last = O1.first.getLastMonad();	
		monad_m o2_first = O2.first.getFirstMonad();
		monad_m o2_last = O2.first.getLastMonad();	

		if (o1_first < o2_first) {
			return true;
		} else if (o1_first > o2_first) {
			return false;
		} else {
			if (o1_last < o2_last) {
				return false;
			} else if (o1_last > o2_last) {
				return true;
			} else {
				return false;
			}
		}
	}
};

void Output_style::do_data(monad_m m, 
			   const std::string& unit_type, 
			   const OTTableMap& unit_list, 
			   const SetOfMonads& monads, 
			   const SetOfMonads& focus)
{
	StringVec data_sep_set;
	//bool bHasData_sep = false;
	if (m_pConf->has_key("data_sep")) {
		//bHasData_sep = true;
		data_sep_set = m_pConf->getValues("data_sep");
	}


	StringVec data_conn_set;
	//bool bHasData_conn = false;
	if (m_pConf->has_key("data_conn")) {
		//bHasData_conn = true;
		data_conn_set = m_pConf->getValues("data_conn");
	}

	if (!m_bFocus_on 
	    //&& monads.isMemberOf(m)
	    && focus.isMemberOf(m)) {
		m_pOut->startFocus();
		m_bFocus_on = true;
	}

	StringVec unit_type_set = m_pConf->getValues(unit_type);
	unsigned int index = 0;

	std::vector<std::pair<MyRow,std::string> > objects; // std::string is object type name

	

	for (index = 0;
	     index < unit_type_set.size();
	     ++index) {
		std::string unit;
		str_tolower(unit_type_set[index], unit);
		MyTable *pUnitTable = unit_list.find(unit)->second;
		pUnitTable->set_universe(m,m);
		while (pUnitTable->hasNext()) {
			const MyRow& U = pUnitTable->next();
			objects.push_back(std::make_pair(MyRow(U), unit));
		}
	}


	std::stable_sort(objects.begin(), objects.end(), object_sort_pred());

	std::vector<bool> vec_bWasPrinted(objects.size());

	for (index = 0;
	     index < objects.size();
	     ++index) {
		std::string unit = objects[index].second;
		MyRow myrow = objects[index].first;
		MyTable *pUnitTable = unit_list.find(unit)->second;

		vec_bWasPrinted[index] = 
			do_data_unit(m, unit, pUnitTable,
				     myrow, 
				     monads, focus,
				     true,  // is left
				     false); // wasAlreadyPrinted
		
	}

	int index2;
	for (index2 = objects.size()-1;
	     index2 >= 0;
	     --index2) {
		std::string unit = objects[index2].second;
		MyRow myrow = objects[index2].first;
		MyTable *pUnitTable = unit_list.find(unit)->second;
		do_data_unit(m, unit, pUnitTable,
			     myrow, 
			     monads, focus,
			     false,                     // is left
			     vec_bWasPrinted[index2]);  // Was already printed
	}

	if (m_bFocus_on
	    && !focus.isMemberOf(m+1)) {
		m_pOut->endFocus();
		m_bFocus_on = false;
	}
}

// Returns whether it was printed
bool Output_style::do_data_unit(monad_m m, 
				const std::string& unit,
				MyTable *pUnitTable,
				MyRow U,
				const SetOfMonads& monads,
				const SetOfMonads& focus,
				bool bIsLeft,
				bool bWasAlreadyPrinted)
{
	UNUSED(monads);
	UNUSED(focus);
	
	bool bWasPrinted = false;
	std::string space = " ";
  
	// Should we do row?
	bool bDoIt = false;
	if (bIsLeft) {
		bDoIt = m == U.getFirstMonad();
	} else {
		bDoIt = m == U.getLastMonad();
	}
	
	if (!bWasAlreadyPrinted && bDoIt) {
		bWasPrinted = true;
		if (bIsLeft) {
			m_pOut->startUnit(unit);
		}
		
		m_pOut->startUnitFeatures(unit);
		
		const StringVec& features = U.getFeatures();
		unsigned int feature_index;
		for (feature_index = 0;
		     feature_index < features.size();
		     ++feature_index) {
			std::string feature = features[feature_index];
			std::string feature_name = pUnitTable->getFeatureNameByIndex(feature_index);
			
			// delete
			if (m_pConf->has_key("delete")) {
				feature = del_accents(feature);
			}
			
			// data_sep
			/* FIXME: Implement
			   if (bHasData_sep && feature != "") {
			   for (StringVec::const_iterator data_sep_ci = data_sep_set.begin();
			   data_sep_ci != data_sep_set.end();
			   ++data_sep_ci) {
			   std::string sep = *data_sep_ci;
			   if (sep == feature.substr(feature.length()-1)) {
			   feature = feature.substr(0, feature.length()-2);
			   space = "";
			   }
			   }
			   }
			   
			   // data_conn
			   if (bHasData_conn && feature != "") {
			   for (StringVec::const_iterator data_conn_ci = data_conn_set.begin();
			   data_conn_ci != data_conn_set.end();
			   ++data_conn_ci) {
			   std::string conn = *data_conn_ci;
			   if (conn == feature.substr(feature.length()-1)) {
			   space = "";
			   }
			   }
			   }
			*/
			
			m_pOut->printFeature(unit, feature_index, feature_name, feature);
			
		} // end of feature-for
		
		m_pOut->endUnitFeatures();
		
	} // End of if-we-should-do-unit
	
	
	if (!bIsLeft
	    && (m == U.getLastMonad())) {
		m_pOut->endUnit(unit);
	}

	return bWasPrinted;
}

void Output_style::do_reference(monad_m m, 
				const std::string& unit_type, 
				const OTTableMap& unit_list) 
{
	start_reference();

	int sep_index = 0;
	bool bDoIt = false;
	MyTable *pUnit = unit_list.begin()->second;
	if (pUnit != 0) {
		pUnit->set_universe(m,m);
		while (pUnit->hasNext()) {
			const MyRow& unit_row = pUnit->next();
			if (m >= unit_row.getFirstMonad()
			    && m <= unit_row.getLastMonad()) {
				bDoIt = true;
			}
		}
	}


	if (bDoIt) {
		StringVec ref_sep_set;
		bool bHasRefSep = false;
		if (m_pConf->has_key("reference_sep")) {
			bHasRefSep = true;
			ref_sep_set = m_pConf->getValues("reference_sep");
		}
		std::ostringstream myoutsstream;
		StringVec units = m_pConf->getValues(unit_type);
		std::string unit = *(units.begin());
		MyTable* pUnitTable = unit_list.find(unit)->second;
		pUnitTable->set_universe(m,m);
		while (pUnitTable->hasNext()) {
			const MyRow& unit_row = pUnitTable->next();
			if (m >= unit_row.getFirstMonad()
			    && m <= unit_row.getLastMonad()) {
				const StringVec& unit_features = unit_row.getFeatures();
				for (StringVec::const_iterator feat_ci = unit_features.begin();
				     feat_ci != unit_features.end()
					     ;) {
					myoutsstream << *feat_ci;
					++feat_ci;
					if (bHasRefSep
					    && feat_ci != unit_features.end()) {
						myoutsstream << ref_sep_set[sep_index];
						if (sep_index < (int) (ref_sep_set.size()-1)) {
							++sep_index;
						}
					} else if (feat_ci != unit_features.end()) {
						myoutsstream << " ";
					}
				}
			}
		}
		print_reference(myoutsstream.str());
	}

	end_reference();
}


void Output_style::start_reference(void)
{
	// Nothing to do
}

void Output_style::print_reference(const std::string& reference)
{
	m_pOut->startLatin();
	m_pOut->print("(" + reference + ") ");
	m_pOut->startRaster();
}


void Output_style::end_reference(void)
{
	// Nothing to do
}

void Output_style::start_raster_unit(void)
{
	m_pOut->startRaster();
}

void Output_style::start_raster_unit_row(void)
{
	// Nothing to do
}


void Output_style::end_raster_unit_row(void)
{
	if (m_bBreakAfterRaster) {
		m_pOut->newline();
	}
}


void Output_style::end_raster_unit()
{
}


void Output_style::start_solution(int solution_number)
{
	std::ostringstream ostr;
	ostr << "\nSolution number: " << solution_number << '\n';
	m_pOut->startLatin();
	writeStr(ostr.str());
}


void Output_style::end_solution()
{
	m_pOut->endSolution();
}





//////////////////////////////////////////////////////////////
//
// KWIC_Output_style
//
//////////////////////////////////////////////////////////////

KWIC_Output_style::KWIC_Output_style(Configuration *pConf, QTOutputBase* pOut)
	: Output_style(pConf, pOut),
	  m_state(kKWICBeforeKeyword)
{
}



KWIC_Output_style::~KWIC_Output_style()
{
}


void KWIC_Output_style::start_raster_unit_row(void)
{
	m_pOut->startTableRow("kwic");
}

void KWIC_Output_style::end_raster_unit_row(void)
{
	end_KWIC_AfterKeyword();
	m_pOut->endTableRow("kwic");
}

void KWIC_Output_style::do_data(monad_m m, const std::string& unit_type, 
				     const OTTableMap& unit_list, 
				     const SetOfMonads& monads, 
				     const SetOfMonads& focus)
{
	switch (m_state) {
	case kKWICBeforeKeyword:
		if (focus.isMemberOf(m)) {
			end_KWIC_BeforeKeyword();
			m_state = kKWICKeyword;
			start_KWIC_Keyword();
		}
		break;
	case kKWICKeyword:
		// Nothing to do
		break;
	case kKWICAfterKeyword:
		// Nothing to do
		break;
	}
	Output_style::do_data(m, unit_type,
			      unit_list,
			      monads,
			      focus);
	switch (m_state) {
	case kKWICBeforeKeyword:
		// Nothing to do
		break;
	case kKWICKeyword:
		end_KWIC_Keyword();
		m_state = kKWICAfterKeyword;
		start_KWIC_AfterKeyword();
		break;
	case kKWICAfterKeyword:
		// Nothing to do
		break;
	}
}

void KWIC_Output_style::start_solution(int solution_number)
{
	UNUSED(solution_number);
	
	// Nothing to do...
	// But must be overridden, so as to
	// not do what Output_style does.
}


void KWIC_Output_style::end_solution(void)
{
	// Nothing to do...
	// But must be overridden, so as to
	// not do what Output_style does.
}



void KWIC_Output_style::start_reference(void)
{
	m_pOut->startTableCell("reference");
}


void KWIC_Output_style::print_reference(const std::string& reference)
{
	writeStr(reference);
}


void KWIC_Output_style::end_reference(void)
{
	m_pOut->endTableCell("reference");
	m_state = kKWICBeforeKeyword;
	start_KWIC_BeforeKeyword();
}


void KWIC_Output_style::start_KWIC_BeforeKeyword(void)
{
	m_pOut->startTableCell("beforekeyword");
}


void KWIC_Output_style::end_KWIC_BeforeKeyword(void)
{
	m_pOut->endTableCell("beforekeyword");
}


void KWIC_Output_style::start_KWIC_Keyword(void)
{
	m_pOut->startTableCell("keyword");
}


void KWIC_Output_style::end_KWIC_Keyword()
{
	m_pOut->endTableCell("keyword");

}


void KWIC_Output_style::start_KWIC_AfterKeyword(void)
{
	m_pOut->startTableCell("afterkeyword");
}


void KWIC_Output_style::end_KWIC_AfterKeyword()
{
	m_pOut->endTableCell("afterkeyword");
}


void KWIC_Output_style::start_raster_unit(void)
{
}

void KWIC_Output_style::end_raster_unit(void)
{
}


//////////////////////////////////////////////////////////////
//
// Tree_Output_style
//
//////////////////////////////////////////////////////////////

Tree_Output_style::Tree_Output_style(Configuration *pConf, QTOutputBase* pOut)
	: Output_style(pConf, pOut)
{
}


void Tree_Output_style::show(const Solution *pSolution, int solution_number)
{
	start_solution(solution_number);

	print_reference(pSolution);

	m_pOut->startRaster();

	print_tree(pSolution);
	
	end_solution();
}


void Tree_Output_style::print_reference(const Solution *pSolution)
{
	do_reference(pSolution->getMonads().first(), "reference_unit", 
		     pSolution->getOTTableMap(kReference));
}

void Tree_Output_style::print_tree(const Solution *pSolution)
{
	SetOfMonads focusMonads = pSolution->getFocus();

	if (m_pConf->has_key("data_tree_terminal_unit")) {
		m_pOut->startTree();

		std::string terminal_object_type;
		str_tolower(m_pConf->getValues("data_tree_terminal_unit")[0], terminal_object_type);
		
		// Add nodes
		const OTTableMap& dat_list = pSolution->getOTTableMap(kData);

		StringVec data_unit_type_set = m_pConf->getValues("data_unit");
		unsigned int index = 0;
		for (index = 0;
		     index < data_unit_type_set.size();
		     ++index) {
			std::string unit;
			str_tolower(data_unit_type_set[index], unit);
			MyTable *pUnitTable = dat_list.find(unit)->second;
			pUnitTable->set_universe(pSolution->getMonads().first(),
						 pSolution->getMonads().last());
			typedef std::map<id_d_t, std::list<MyRow> > ID_D2RowListMap;
			ID_D2RowListMap object_map;
			
			while (pUnitTable->hasNext()) {
				MyRow row = MyRow(pUnitTable->next());
				if (object_map.find(row.getID_D()) == object_map.end()) {
					object_map.insert(std::make_pair(row.getID_D(), std::list<MyRow>()));
				}
				object_map[row.getID_D()].push_back(row);
			}

			ID_D2RowListMap::const_iterator ci2;
			ci2 = object_map.begin();
			while (ci2 != object_map.end()) {
				id_d_t object_id_d = ci2->first;

				// Get object's SetOfMonads
				SetOfMonads object_som;
				std::list<MyRow>::const_iterator ci3;
				ci3 = ci2->second.begin();
				while (ci3 != ci2->second.end()) {
					const MyRow row = *ci3;
					object_som.add(row.getFirstMonad(),
						       row.getLastMonad());
					++ci3;
				}
				
				bool bHasParent = m_pConf->has_subkey("data_tree_parent", unit);
				bool bFocus = false;
				if (m_bApplyFocus) {
					bFocus = SetOfMonads::overlap(focusMonads, object_som);
				}
				if (strcmp_nocase(unit, terminal_object_type) == 0) {
					// Is terminal
					m_pOut->addTreeTerminal(unit,
								object_id_d,
								bHasParent,
								object_som,
								ci2->second.begin()->getFeatures(),
								bFocus);
							    
				} else {
					// Is non-terminal
					m_pOut->addTreeNonTerminal(unit,
								   object_id_d,
								   bHasParent,
								   object_som,
								   ci2->second.begin()->getFeatures(),
								   bFocus);
				}
				++ci2;
			}

			// FIXME: Remember to check, in exec.cpp,
			// that data_tree_parent is set for the terminal
			// object type!
		}

		m_pOut->endTree();
	}
}



//////////////////////////////////////////////////////////////
//
// XML_Output_style
//
//////////////////////////////////////////////////////////////

XML_Output_style::XML_Output_style(Configuration *pConf, QTOutputBase* pOut)
	: Output_style(pConf, pOut)
{
}

void XML_Output_style::turn_focus_off(void)
{
	if (m_bFocus_on) {
		// m_pOut->endFocus();
		writeStr("</focus>");
		m_bFocus_on = false;
	}
}

void XML_Output_style::show(const Solution *pSolution, int solution_number)
{
	start_solution(solution_number);

	m_bFocus_on = false;
	const OTTableMap& raster = pSolution->getOTTableMap(kRaster);
	for (OTTableMap::const_iterator ottm_ci = raster.begin();
	     ottm_ci != raster.end()
		     && !m_pOut->ShouldStop();
	     ++ottm_ci) {
		do_raster_unit(ottm_ci->second, // MyTable for this object type
			       pSolution->getOTTableMap(kReference),
			       pSolution->getOTTableMap(kData),
			       pSolution->getMonads(),
			       pSolution->getFocus());
	}
	end_solution();
}

void XML_Output_style::do_raster_unit(MyTable *ras_list, 
				  const OTTableMap& ref_list, 
				  const OTTableMap& dat_list, 
				  const SetOfMonads& monads, 
				  const SetOfMonads& focus)
{
	start_raster_unit();
	ras_list->set_universe(monads.first(),monads.last());
	while (ras_list->hasNext()
	       && !m_pOut->ShouldStop()) {
		std::string reference;

		const MyRow& ras_row = ras_list->next();
		do_reference(ras_row.getFirstMonad(), "reference_unit", ref_list, reference);
		start_raster_unit_row(reference);

		for (monad_m m = ras_row.getFirstMonad();
		     m <= ras_row.getLastMonad()
			     && !m_pOut->ShouldStop();
		     ++m) {
			do_data(m, std::string("data_unit"), dat_list, monads, focus);
		}
		turn_focus_off();
		end_raster_unit_row();
	}
	turn_focus_off();
	end_raster_unit();
}


void XML_Output_style::do_data(monad_m m, 
			   const std::string& unit_type, 
			   const OTTableMap& unit_list, 
			   const SetOfMonads& monads, 
			   const SetOfMonads& focus)
{
	StringVec data_sep_set;
	//bool bHasData_sep = false;
	if (m_pConf->has_key("data_sep")) {
		//bHasData_sep = true;
		data_sep_set = m_pConf->getValues("data_sep");
	}


	StringVec data_conn_set;
	//bool bHasData_conn = false;
	if (m_pConf->has_key("data_conn")) {
		//bHasData_conn = true;
		data_conn_set = m_pConf->getValues("data_conn");
	}

	if (!m_bFocus_on 
	    //&& monads.isMemberOf(m)
	    && focus.isMemberOf(m)) {
		// m_pOut->startFocus();
		writeStr("\n<focus>");
		m_bFocus_on = true;
	}

	StringVec unit_type_set = m_pConf->getValues(unit_type);
	unsigned int index = 0;

	std::vector<std::pair<MyRow,std::string> > objects; // std::string is object type name

	

	for (index = 0;
	     index < unit_type_set.size();
	     ++index) {
		std::string unit;
		str_tolower(unit_type_set[index], unit);
		MyTable *pUnitTable = unit_list.find(unit)->second;
		pUnitTable->set_universe(m,m);
		while (pUnitTable->hasNext()) {
			const MyRow& U = pUnitTable->next();
			objects.push_back(std::make_pair(MyRow(U), unit));
		}
	}


	std::stable_sort(objects.begin(), objects.end(), object_sort_pred());

	std::vector<bool> vec_bWasPrinted(objects.size());

	for (index = 0;
	     index < objects.size();
	     ++index) {
		std::string unit = objects[index].second;
		MyRow myrow = objects[index].first;
		MyTable *pUnitTable = unit_list.find(unit)->second;

		vec_bWasPrinted[index] = 
			do_data_unit(m, unit, pUnitTable,
				     myrow, 
				     monads, focus,
				     true,  // is left
				     false); // wasAlreadyPrinted
		
	}

	int index2;
	for (index2 = objects.size()-1;
	     index2 >= 0;
	     --index2) {
		std::string unit = objects[index2].second;
		MyRow myrow = objects[index2].first;
		MyTable *pUnitTable = unit_list.find(unit)->second;
		do_data_unit(m, unit, pUnitTable,
			     myrow, 
			     monads, focus,
			     false,                     // is left
			     vec_bWasPrinted[index2]);  // Was already printed
	}

	if (m_bFocus_on
	    && !focus.isMemberOf(m+1)) {
		//m_pOut->endFocus();
		writeStr("\n</focus>");
		m_bFocus_on = false;
	}
}

// Returns whether it was printed
bool XML_Output_style::do_data_unit(monad_m m, 
				    const std::string& unit,
				    MyTable *pUnitTable,
				    MyRow U,
				    const SetOfMonads& monads,
				    const SetOfMonads& focus,
				    bool bIsLeft,
				    bool bWasAlreadyPrinted)
{
	UNUSED(monads);
	UNUSED(focus);
	
	bool bWasPrinted = false;
	std::string space = " ";
  
	// Should we do row?
	bool bDoIt = false;
	if (bIsLeft) {
		bDoIt = m == U.getFirstMonad();
	} else {
		bDoIt = m == U.getLastMonad();
	}
	
	if (!bWasAlreadyPrinted && bDoIt) {
		bWasPrinted = true;
		if (bIsLeft) {
			writeStr("\n<unit_start object_type_name=\"" + unit + "\"/>");
			
		}
		
		const StringVec& features = U.getFeatures();
		unsigned int feature_index;
		for (feature_index = 0;
		     feature_index < features.size();
		     ++feature_index) {
			std::string feature = features[feature_index];
			std::string feature_name = pUnitTable->getFeatureNameByIndex(feature_index);
			
			// delete
			if (m_pConf->has_key("delete")) {
				feature = del_accents(feature);
			}
			
			// data_sep
			/* FIXME: Implement
			   if (bHasData_sep && feature != "") {
			   for (StringVec::const_iterator data_sep_ci = data_sep_set.begin();
			   data_sep_ci != data_sep_set.end();
			   ++data_sep_ci) {
			   std::string sep = *data_sep_ci;
			   if (sep == feature.substr(feature.length()-1)) {
			   feature = feature.substr(0, feature.length()-2);
			   space = "";
			   }
			   }
			   }
			   
			   // data_conn
			   if (bHasData_conn && feature != "") {
			   for (StringVec::const_iterator data_conn_ci = data_conn_set.begin();
			   data_conn_ci != data_conn_set.end();
			   ++data_conn_ci) {
			   std::string conn = *data_conn_ci;
			   if (conn == feature.substr(feature.length()-1)) {
			   space = "";
			   }
			   }
			   }
			*/

			// std::string new_feature_value = m_pOut->applyTECkit(unit, feature_index, feature_name, feature);
			writeStr("\n<f feature_name=\"" + feature_name + "\" unit_name=\"" + unit + "\">");
			///writeStr(escapeXMLEntities(new_feature_value));
			writeStr(escapeXMLEntities(feature));
			writeStr("</f>");

			
		} // end of feature-for
		
	} // End of if-we-should-do-unit
	
	
	if (!bIsLeft
	    && (m == U.getLastMonad())) {
		writeStr("\n<unit_end object_type_name=\"" + unit + "\"/>");
	}

	return bWasPrinted;
}

void XML_Output_style::do_reference(monad_m m, 
				    const std::string& unit_type, 
				    const OTTableMap& unit_list,
				    std::string& /* out */ reference) 
{
	int sep_index = 0;
	bool bDoIt = false;
	MyTable *pUnit = unit_list.begin()->second;
	if (pUnit != 0) {
		pUnit->set_universe(m,m);
		while (pUnit->hasNext()) {
			const MyRow& unit_row = pUnit->next();
			if (m >= unit_row.getFirstMonad()
			    && m <= unit_row.getLastMonad()) {
				bDoIt = true;
			}
		}
	}


	if (!bDoIt) {
		reference = "";
	} else {
		StringVec ref_sep_set;
		bool bHasRefSep = false;
		if (m_pConf->has_key("reference_sep")) {
			bHasRefSep = true;
			ref_sep_set = m_pConf->getValues("reference_sep");
		}
		std::ostringstream myoutsstream;
		StringVec units = m_pConf->getValues(unit_type);
		std::string unit = *(units.begin());
		MyTable* pUnitTable = unit_list.find(unit)->second;
		pUnitTable->set_universe(m,m);
		while (pUnitTable->hasNext()) {
			const MyRow& unit_row = pUnitTable->next();
			if (m >= unit_row.getFirstMonad()
			    && m <= unit_row.getLastMonad()) {
				const StringVec& unit_features = unit_row.getFeatures();
				for (StringVec::const_iterator feat_ci = unit_features.begin();
				     feat_ci != unit_features.end()
					     ;) {
					myoutsstream << *feat_ci;
					++feat_ci;
					if (bHasRefSep
					    && feat_ci != unit_features.end()) {
						myoutsstream << ref_sep_set[sep_index];
						if (sep_index < (int) (ref_sep_set.size()-1)) {
							++sep_index;
						}
					} else if (feat_ci != unit_features.end()) {
						myoutsstream << " ";
					}
				}
			}
		}
		reference = myoutsstream.str();
	}
}


void XML_Output_style::start_reference(void)
{
	// Nothing to do
}

void XML_Output_style::print_reference(const std::string& reference)
{
	UNUSED(reference);
	
	// Nothing to do 
}


void XML_Output_style::end_reference(void)
{
	// Nothing to do
}

void XML_Output_style::start_raster_unit(void)
{
	writeStr("\n<raster_unit>");
}

void XML_Output_style::start_raster_unit_row(const std::string& reference)
{
	writeStr("\n<raster_unit_row reference=\"" + escapeXMLEntities(reference) + "\">");
}


void XML_Output_style::end_raster_unit_row(void)
{
	writeStr("\n</raster_unit_row>");
}


void XML_Output_style::end_raster_unit()
{
	writeStr("\n</raster_unit>");
}


void XML_Output_style::start_solution(int solution_number)
{
	std::ostringstream ostr;
	ostr << "\n<solution number=\"" << solution_number << "\">";
	m_pOut->startLatin();
	writeStr(ostr.str());
}


void XML_Output_style::end_solution()
{
	std::ostringstream ostr;
	ostr << "\n</solution>";
	m_pOut->startLatin();
	writeStr(ostr.str());
	m_pOut->endSolution();
}



