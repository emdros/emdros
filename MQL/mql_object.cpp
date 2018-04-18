/*
 * mql_object.cpp
 *
 * MQL representation of an object while processing the query
 *
 * Ulrik Petersen
 * Created: 4/2-2001
 * Last update: 1/9-2009
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2009  Ulrik Sandborg-Petersen
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

/**@file mql_object.cpp
 *@brief Implementation of MQLObject (MQL layer)
 */


#include <emdfdb.h>
#include <mql_object.h>
#include <mql_extern.h>
#include <debug.h>
#include <algorithm>

MQLObject::MQLObject() : 
	m_feature_name_vec_inst(0)
{
}

MQLObject::~MQLObject()
{
	delete m_feature_name_vec_inst;
	unsigned int feature_value_vec_size = m_feature_value_vec_inst.size();
	unsigned int i;
	for (i = 0; i < feature_value_vec_size; ++i) {
		delete m_feature_value_vec_inst[i];
	}
}

void MQLObject::addFeature(const FeatureInfo& fi)
{
	// Get feature name
	std::string feature_name;
	feature_name = fi.getName();

	// Get which it is
	std::list<FeatureInfo> *pList;
	pList = &m_feature_infos_inst;

	// Only add if not there already
	std::list<FeatureInfo>::const_iterator ci(pList->begin());
	std::list<FeatureInfo>::const_iterator cend(pList->end());
	while (ci != cend) {
		// Check if this is the one
		if (strcmp_nocase(ci->getName(), feature_name) == 0) {
			// It was, so return without adding.
			return;
		}

		// Advance iterator
		++ci;
	}

	// It wasn't there, so add it at the back
	pList->push_back(fi);

	// Resize m_feature_value_vec
	m_feature_value_vec_inst.resize(pList->size());
}

void MQLObject::retrieveFeatureValues(const InstObject *pOther)
{
	std::list<FeatureInfo>::const_iterator fii(m_feature_infos_inst.begin());
	std::list<FeatureInfo>::const_iterator fiend(m_feature_infos_inst.end());
	unsigned int index = 0;
	while (fii != fiend) {
		delete m_feature_value_vec_inst[index];
		m_feature_value_vec_inst[index] = new EMdFValue(*(pOther->getFeature(index)));
		++index;
		++fii;
	}
}


//
// Get a vector of strings of feature-names in m_map
// Must only be called after all the adding of feature names has taken place.
const std::vector<std::string>& MQLObject::getFeatureNames() 
{
	std::vector<std::string> *pVec;
	std::list<FeatureInfo> *pList;
	pVec = m_feature_name_vec_inst;
	pList = &m_feature_infos_inst;


	// Should we create the name vector?
	if (pVec == 0) {
		// Yes

		// Allocate new vector
		pVec = new std::vector<std::string>;

		// Set the appropriate member
		m_feature_name_vec_inst = pVec;

		// Resize member to same length as appropriate list
		pVec->resize(pList->size());


		unsigned int index = 0;
		std::list<FeatureInfo>::const_iterator fi_i(pList->begin());
		std::list<FeatureInfo>::const_iterator fi_iend(pList->end());
		while (fi_i != fi_iend) {
			(*pVec)[index] = fi_i->getName();
			++index;
			++fi_i;
		}
    
		// Sort vector
		// DON'T do this.  The vector needs to be the same order as
		// the FeatureInfos.
		// std::sort(m_feature_name_vec->begin(), m_feature_name_vec->end());
	}
	return *pVec;
}

short int MQLObject::getFeatureIndex(const std::string& feature_name) const
{
	const std::list<FeatureInfo> *pList;
	pList = &m_feature_infos_inst;

	std::list<FeatureInfo>::const_iterator fi_i(pList->begin());
	std::list<FeatureInfo>::const_iterator fi_iend(pList->end());
	short int index = 0;
	while (fi_i != fi_iend) {
		if (strcmp_nocase(fi_i->getName(), feature_name) == 0)
			return index;
		++index;
		++fi_i;
	}
	// We shouldn't get here, since it should have been in m_feature_infos
	ASSERT_THROW(false,
		     "We shouldn't get here, since it should have been in m_feature_infos");
	return -1;
}



