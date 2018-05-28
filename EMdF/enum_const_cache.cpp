/*
 * enum_const_cache.cpp
 *
 * EnumConstCache class
 *
 * Ulrik Petersen
 * Created: 1/20-2004
 * Last update: 5/28-2018
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2004-2018  Ulrik Sandborg-Petersen
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

/**@file enum_const_cache.cpp
 *@brief Implementation of EnumConstCache (EMdF layer)
 */


#include <enum_const_cache.h>
#include <debug.h>


/** Local function to clear a ValueECIMap.
 *
 * @internal
 *
 * Local function to delete all EnumConstInfos
 * in a ValueECIMap, and to clear the map.
 *
 * @param VECIMap The ValueECIMap to clear.
 */
void clearVECIMap(ValueECIMap& VECIMap)
{
	ValueECIMap::iterator it = VECIMap.begin();
	ValueECIMap::iterator end = VECIMap.end();

	for (; it != end; ++it) {
		delete it->second;
	}

	VECIMap.clear();
}

/** Local function to clear a StringECIMap 
 *@internal
 *
 * @param SECIMap The StringECIMap to clear.
 */
static void clearSECIMap(StringECIMap& SECIMap)
{
	// Only clear it; don't do anything to the objects.
	// This is because the objects are already taken care of in
	// the VECIMap.
	SECIMap.clear();
}



EnumConstCache::EnumConstCache()
{
	// Nothing to do
}


EnumConstCache::~EnumConstCache()
{
	clear();
}

//
// Return true on added, false on not added.
// Note that false does not mean error.
bool EnumConstCache::addEci(id_d_t enum_id, const std::string& enum_name, const EnumConstInfo& eci)
{
	// First make sure the enum is in the cache
	if (!hasEnum(enum_id)) {
		addEnum(enum_id, enum_name);
	}

	// Then add it

	// First to the VECI map

	// Copy eci onto heap
	EnumConstInfo *pECI = new EnumConstInfo(eci);

	// Find ValueECIMap
	PValueECIMap pVECIMap = m_idd_veci_map[enum_id];

	// Make sure it isn't there already
	int value = eci.getValue();
	if (pVECIMap->find(value) != pVECIMap->end()) {
		// It was there. Don't add, and return false.
		// (this is not an error.)
		delete pECI;
		return false;
	} else {
		// Insert at [value]
		pVECIMap->insert(ValueECIMap::value_type(value, pECI));
	}

	// Then to the SECI map

	// Find StringECIMap
	PStringECIMap pSECIMap = m_idd_seci_map[enum_id];

	// Make sure it isn't there already
	std::string enum_const_name = eci.getName();
	if (pSECIMap->find(enum_const_name) != pSECIMap->end()) {
		// It was there. We should stop NOW!
		// This is because it should not NOT be in the VECI map,
		// and at the same time be in the SECI map.
		ASSERT_THROW(false, "It was there.");
	} else {
		// Insert at [value]
		pSECIMap->insert(StringECIMap::value_type(enum_const_name, pECI));
	}

  
	// If we came this far, we added it
	return true;
}


void EnumConstCache::addEnum(id_d_t enum_id, const std::string& enum_name)
{
	if (!hasEnum(enum_id)) {
		m_idd_enum_name_map.insert(IDD_StringMap::value_type(enum_id, enum_name));
		m_enum_name_idd_map.insert(String_IDDMap::value_type(enum_name, enum_id));
		PValueECIMap pVECIMap = new ValueECIMap;
		m_idd_veci_map.insert(IDD_VECIMap::value_type(enum_id, pVECIMap));
		PStringECIMap pSECIMap = new StringECIMap;
		m_idd_seci_map.insert(IDD_SECIMap::value_type(enum_id, pSECIMap));
	}
}

void EnumConstCache::dropEnum(id_d_t enum_id)
{
	if (hasEnum(enum_id)) {
		// Get name of enum and erase from m_idd_enum_name_map.
		IDD_StringMap::iterator idd_name_it = m_idd_enum_name_map.find(enum_id);
		std::string enum_name = idd_name_it->second;
		m_idd_enum_name_map.erase(idd_name_it);

		// Erase from m_enum_name_idd_map
		String_IDDMap::iterator name_idd_it = m_enum_name_idd_map.find(enum_name);
		m_enum_name_idd_map.erase(name_idd_it);

		// Drop in m_idd_seci_map
		IDD_SECIMap::iterator idd_seci_it = m_idd_seci_map.find(enum_id);
		PStringECIMap pSECIMap = idd_seci_it->second;
		clearSECIMap(*pSECIMap);
		delete pSECIMap;
		m_idd_seci_map.erase(idd_seci_it);

		// Find in m_idd_veci_map and clear that
		IDD_VECIMap::iterator idd_veci_it = m_idd_veci_map.find(enum_id);
		PValueECIMap pVECIMap = idd_veci_it->second;
		clearVECIMap(*pVECIMap);
		delete pVECIMap;
		m_idd_veci_map.erase(idd_veci_it);
	}
}



const EnumConstInfo* EnumConstCache::find(id_d_t enum_id, const std::string& enum_const_name) const
{
	// First see if we can find the enum
	IDD_SECIMap::const_iterator ci = m_idd_seci_map.find(enum_id);
	if (ci == m_idd_seci_map.end()) {
		// enum wasn't there; return 0
		return 0;
	} else {
		// Get pointer to SECIMap from iterator
		PStringECIMap pSECIMap = ci->second;

		// See if we can find the value in the VECI map
		StringECIMap::const_iterator ci2 = pSECIMap->find(enum_const_name);
		if (ci2 == pSECIMap->end()) {
			// It wasn't there; return 0
			return 0;
		} else {
			return ci2->second;
		}
	}
}


const EnumConstInfo* EnumConstCache::find(id_d_t enum_id, int enum_const_value) const
{
	// First see if we can find the enum
	IDD_VECIMap::const_iterator ci = m_idd_veci_map.find(enum_id);
	if (ci == m_idd_veci_map.end()) {
		// enum wasn't there; return 0
		return 0;
	} else {
		// Get pointer to VECIMap from iterator
		PValueECIMap pVECIMap = ci->second;

		// See if we can find the value in the VECI map
		ValueECIMap::const_iterator ci2 = pVECIMap->find(enum_const_value);
		if (ci2 == pVECIMap->end()) {
			// It wasn't there; return 0
			return 0;
		} else {
			return ci2->second;
		}
	}
}

const EnumConstInfo* EnumConstCache::find(const std::string& enum_name, int enum_const_value) const
{
	// See if we can find the enum
	String_IDDMap::const_iterator ci = m_enum_name_idd_map.find(enum_name);
	if (ci == m_enum_name_idd_map.end()) {
		// The enum was not in the cache, and hence the const wasn't there either
		return 0;
	} else {
		// Get id_d
		id_d_t id_d = ci->second;

		// use other find
		return this->find(id_d, enum_const_value);
	}
}


const EnumConstInfo* EnumConstCache::getDefault(id_d_t enum_id) const
{
	// First see if we can find the enum
	IDD_VECIMap::const_iterator ci = m_idd_veci_map.find(enum_id);
	if (ci == m_idd_veci_map.end()) {
		// The enum was not in the cache, and hence the const wasn't there either
		return 0;
	} else {
		// Get pointer to ValueECIMap from iterator
		const PValueECIMap pVECIMap = ci->second;

		// Iterate over map
		ValueECIMap::const_iterator ci2 = pVECIMap->begin();
		while (ci2 != pVECIMap->end()) {
			// Get EnumConstInfo from iterator
			const EnumConstInfo* pECI = ci2->second;

			// If this is the default, return it.
			if (pECI->getIsDefault()) {
				return pECI;
			}

			// Advance iterator
			++ci2;
		}

		// If we got this far, we couldn't find it.
		// This is an not error, since we may not have all
		// the enum constants in the cache.
		return 0;
	}
}

// Local function 
void EnumConstCache::clear()
{
	// Clear m_idd_seci_map
	IDD_SECIMap::iterator idd_seci_iterator = m_idd_seci_map.begin();
	IDD_SECIMap::iterator idd_seci_end = m_idd_seci_map.end();
	for (;idd_seci_iterator != idd_seci_end; ++idd_seci_iterator) {
		PStringECIMap pSECIMap = idd_seci_iterator->second;
		clearSECIMap(*pSECIMap);
		delete pSECIMap;
	}

	// Clear m_idd_veci_map
	IDD_VECIMap::iterator idd_veci_iterator = m_idd_veci_map.begin();
	IDD_VECIMap::iterator idd_veci_end = m_idd_veci_map.end();
	for (;idd_veci_iterator != idd_veci_end; ++idd_veci_iterator) {
		PValueECIMap pVECIMap = idd_veci_iterator->second;
		clearVECIMap(*pVECIMap);
		delete pVECIMap;
	}

	// Clear all maps
	m_idd_veci_map.clear();
	m_idd_seci_map.clear();
	m_idd_enum_name_map.clear();
	m_enum_name_idd_map.clear();
}


bool EnumConstCache::hasEnum(id_d_t enum_id)
{
	return m_idd_enum_name_map.find(enum_id) != m_idd_enum_name_map.end();
}

void EnumConstCache::drop(id_d_t enum_id, const std::string& enum_const_name)
{
	// First see if we can find the enum
	IDD_SECIMap::iterator it = m_idd_seci_map.find(enum_id);
	if (it == m_idd_seci_map.end()) {
		// enum wasn't there; return without doing anything
		return;
	} else {
		// Get pointer to SECIMap from iterator
		PStringECIMap pSECIMap = it->second;

		// See if we can find the value in the SECI map
		StringECIMap::iterator it2 = pSECIMap->find(enum_const_name);
		if (it2 == pSECIMap->end()) {
			// It wasn't there; return without doing anything
			return;
		} else {
			// Get its value
			int enum_const_value = it2->second->getValue();

			// DON'T delete the EnumConstInfo
			// It will be deleted when we do the VECI Map.
			//      delete it2->second;

			// Erase it from the SECIMap
			pSECIMap->erase(it2);

			//
			// Drop it from the VECIMap
			//
			IDD_VECIMap::iterator it3 = m_idd_veci_map.find(enum_id);
			if (it3 == m_idd_veci_map.end()) {
				// enum wasn't there; 
				// This is an ERROR, since it should be in both the SECIMap 
				// and the VECIMap
				ASSERT_THROW(false, "Enum wasn't there.");
			} else {
				// Get pointer to VECIMap from iterator
				PValueECIMap pVECIMap = it3->second;
	
				// See if we can find the value in the VECI map
				ValueECIMap::iterator it4 = pVECIMap->find(enum_const_value);
				if (it4 == pVECIMap->end()) {
					// It wasn't there; 
					// This is an ERROR, since it should be in both the SECIMap
					// and the VECIMap.
					ASSERT_THROW(false, "It wasn't there");
				} else {
					// Delete the EnumConstInfo
					delete it4->second;
	  
					// Erase it from the VECIMap
					pVECIMap->erase(it4);
				}
			}
		}
	}
}

void EnumConstCache::updateValue(id_d_t enum_id, const std::string& enum_const_name, int new_value)
{
	// First see if we can find the enum
	IDD_SECIMap::iterator it = m_idd_seci_map.find(enum_id);
	if (it == m_idd_seci_map.end()) {
		// enum wasn't there; return without doing anything
		return;
	} else {
		// Get pointer to SECIMap from iterator
		PStringECIMap pSECIMap = it->second;

		// See if we can find the value in the SECI map
		StringECIMap::iterator it2 = pSECIMap->find(enum_const_name);
		if (it2 == pSECIMap->end()) {
			// It wasn't there; return without doing anything
			return;
		} else {
			// Get pointer to EnumConstInfo from iterator
			EnumConstInfo *pECI = it2->second;

			int old_enum_const_value = pECI->getValue();

			// Update it
			pECI->setValue(new_value);

			//
			// Drop it from the VECIMap and insert it again
			//
			IDD_VECIMap::iterator it3 = m_idd_veci_map.find(enum_id);
			if (it3 == m_idd_veci_map.end()) {
				// enum wasn't there; 
				// This is an ERROR, since it should be in both the SECIMap 
				// and the VECIMap
				ASSERT_THROW(false, "Enum wasn't there");
			} else {
				// Get pointer to VECIMap from iterator
				PValueECIMap pVECIMap = it3->second;
	
				// See if we can find the value in the VECI map
				ValueECIMap::iterator it4 = pVECIMap->find(old_enum_const_value);
				if (it4 == pVECIMap->end()) {
					// It wasn't there; 
					// This is an ERROR, since it should be in both the SECIMap
					// and the VECIMap.
					ASSERT_THROW(false, "It wasn't there");
				} else {
					// Erase it from the VECIMap
					pVECIMap->erase(it4);

					// Insert it again
					pVECIMap->insert(ValueECIMap::value_type(new_value, pECI));
				}
			}
		}
	}
}

void EnumConstCache::setNewDefault(id_d_t enum_id, const std::string& enum_value_name)
{
	// First see if we can find the enum
	IDD_SECIMap::iterator it = m_idd_seci_map.find(enum_id);
	if (it == m_idd_seci_map.end()) {
		// enum wasn't there; return without doing anything
		return;
	} else {
		// Get pointer to SECIMap from iterator
		PStringECIMap pSECIMap = it->second;

		// See if we can find the value in the SECI map
		StringECIMap::iterator it2 = pSECIMap->begin();
		StringECIMap::iterator seci_end = pSECIMap->end();
		for (; it2 != seci_end; ++it2) {
			// Get EnumConstInfo pointer from iterator
			EnumConstInfo *pECI = it2->second;

			// Check how to update
			bool bIsDefault = pECI->getName() == enum_value_name;
			pECI->setIsDefault(bIsDefault);
		}
	}
}

