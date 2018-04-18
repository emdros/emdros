/*
 * memobject.cpp
 *
 * MemObject classes (and supporting classes)
 *
 * Ulrik Sandborg-Petersen
 * Created: Sometime in 2005 or 2006.
 * Last update: 3/21-2013
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2005-2013  Ulrik Sandborg-Petersen
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

#include <memobject.h>
#include <string>
#include <debug.h>

MemObject::MemObject(int nNoOfFeatures)
	: m_OTN_surrogate(0), m_id_d(NIL)
{
	if (nNoOfFeatures > 0) {
		m_features.resize(nNoOfFeatures);
	}
}


MemObject::MemObject(const MemObject& other)
	: m_first(other.m_first),
	  m_last(other.m_last),
	  m_OTN_surrogate(other.m_OTN_surrogate),
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
















