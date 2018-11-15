/*
 * mql_object.cpp
 *
 * MQL representation of an object while processing the query
 *
 * Ulrik Petersen
 * Created: 4/2-2001
 * Last update: 11/15-2018
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
	feature_name = fi.getHumanReadableFeatureName();

	// Get which it is
	std::list<FeatureInfo> *pList;
	pList = &m_feature_infos_inst;

	// Only add if not there already
	std::list<FeatureInfo>::const_iterator ci(pList->begin());
	std::list<FeatureInfo>::const_iterator cend(pList->end());
	while (ci != cend) {
		// Check if this is the one
		if (strcmp_nocase(ci->getHumanReadableFeatureName(), feature_name) == 0) {
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
			(*pVec)[index] = fi_i->getHumanReadableFeatureName();
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
		if (strcmp_nocase(fi_i->getHumanReadableFeatureName(), feature_name) == 0)
			return index;
		++index;
		++fi_i;
	}
	// We shouldn't get here, since it should have been in m_feature_infos
	ASSERT_THROW(false,
		     "We shouldn't get here, since it should have been in m_feature_infos");
	return -1;
}



