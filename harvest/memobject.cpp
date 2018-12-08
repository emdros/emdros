/*
 * memobject.cpp
 *
 * MemObject classes (and supporting classes)
 *
 * Ulrik Sandborg-Petersen
 * Created: Sometime in 2005 or 2006.
 * Last update: 12/8-2018
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



#include <memobject.h>
#include <string>
#include <debug.h>

MemObject::MemObject(int nNoOfFeatures)
	: MemObjectBase()
{
	if (nNoOfFeatures > 0) {
		m_features.resize(nNoOfFeatures);
	}
}


MemObject::MemObject(const MemObject& other)
	: MemObjectBase(other),
	  m_first(other.m_first),
	  m_last(other.m_last),
	  m_id_d(other.m_id_d),
	  m_features(other.m_features)
{
}


MemObject::~MemObject()
{
}



void MemObject::setFeature(unsigned int nFeatureIndex, const std::string& featureVal)
{
	//std::string normalizedFN;
	//str_tolower(featureName, normalizedFN);
	//m_features[normalizedFN] = featureVal;
	m_features[nFeatureIndex] = featureVal;
}


std::string MemObject::getFeature(unsigned int nFeatureIndex)
{
	/*
	std::string normalizedFN;
	str_tolower(featureName, normalizedFN);
	if (m_features.find(normalizedFN) == m_features.end()) {
		return defaultVal;
	} else {
		return m_features[normalizedFN];
	}
	*/
	if (nFeatureIndex >= m_features.size()) {
		throw EmdrosException("MemObject::getFeature(" + long2string(nFeatureIndex) + "): index out of range.");
	}
	return m_features[nFeatureIndex];
}
















