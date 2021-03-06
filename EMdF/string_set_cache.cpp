/*
 * string_set_cache.cpp
 *
 * StringSetCache and related classes
 *
 * Ulrik Petersen
 * Created: 3/9-2007 (March 9, 2007)
 * Last update: 3/11-2008
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


/**@file string_set_cache.cpp
 *@internal
 *@brief StringSetCache and related classes (EMdF layer)
 */

#include <string_set_cache.h>
#include <monads.h>
#include <table.h>

//////////////////////////////////////////////////////////////////
//
// StringSetCache
//
//////////////////////////////////////////////////////////////////

StringSetCache::StringSetCache(id_d_t object_type_id, const std::string& encoded_feature_name)
	: m_object_type_id(object_type_id),
	  m_encoded_feature_name(encoded_feature_name),
	  m_bIsComplete(false),
	  m_bHasFilledStringToSetIDCache(false)
{
}

 
StringSetCache::~StringSetCache()
{
}


bool StringSetCache::operator<(const StringSetCache& other)
{
	if (m_object_type_id < other.m_object_type_id) {
		return true;
	} else if (m_object_type_id > other.m_object_type_id) {
		return false;
	} else {
		if (m_encoded_feature_name < other.m_encoded_feature_name) {
			return true;
		} else {
			return false;
		}
	}
}

 // Only compares m_object_type_id and m_encoded_feature_name
void StringSetCache::addPair(id_d_t set_id, const std::string& stringToAdd)
{
	/*
	if (m_cache_set_id_to_string.find(set_id) != m_cache_set_id_to_string.end()) {
		return; // Already added; nothing to add.
	} else {
	*/
		// Assume we aren't complete
		setIsComplete(false);

		m_cache_set_id_to_string[set_id] = stringToAdd;

		if (m_bHasFilledStringToSetIDCache) {
			m_cache_string_to_set_id[stringToAdd] = set_id;
		}
		//m_cache_string_to_set_id[stringToAdd] = set_id;
		/*
	}
		*/
}


bool StringSetCache::getStringFromID_D(id_d_t set_id, std::string& result) const
{
	std::map<id_d_t, std::string>::const_iterator ci = m_cache_set_id_to_string.find(set_id);
	if (ci == m_cache_set_id_to_string.end()) {
		return false; // Not there.
	} else {
		result = ci->second;
		return true;
	}	
}


void StringSetCache::fillStringToSetIDDCache(void)
{
	std::map<id_d_t, std::string>::const_iterator ci = 
		m_cache_set_id_to_string.begin();
	std::map<id_d_t, std::string>::const_iterator cend = 
		m_cache_set_id_to_string.end();

	while (ci != cend) {
		m_cache_string_to_set_id[ci->second] = ci->first;
		++ci;
	}

	m_bHasFilledStringToSetIDCache = true;
}

 // Returns false on not there, true if there.
bool StringSetCache::getID_DFromString(const std::string& stringToFind, id_d_t& /* out */ result)
{
	if (!m_bHasFilledStringToSetIDCache) {
		fillStringToSetIDDCache();
	}
	std::map<std::string, id_d_t>::const_iterator ci = m_cache_string_to_set_id.find(stringToFind);
	if (ci == m_cache_string_to_set_id.end()) {
		return false; // Not there.
	} else {
		result = ci->second;
		return true;
	}	
}


void StringSetCache::getID_DsAsSOM(SetOfMonads& som) const
{
	std::map<id_d_t, std::string>::const_iterator ci = m_cache_set_id_to_string.begin();
	std::map<id_d_t, std::string>::const_iterator cend = m_cache_set_id_to_string.end();
	
	while (ci != cend) {
		som.add(ci->first);
		++ci;
	}
}

void StringSetCache::getStringsAsTable(Table& result) const
{
	std::map<id_d_t, std::string>::const_iterator ci = m_cache_set_id_to_string.begin();
	std::map<id_d_t, std::string>::const_iterator cend = m_cache_set_id_to_string.end();
	
	while (ci != cend) {
		result.startNewRow();
		result.append(ci->second);
		++ci;
	}
}


//////////////////////////////////////////////////////////////////
//
// StringSetsCache
//
//////////////////////////////////////////////////////////////////


StringSetsCache::StringSetsCache()
{
}


StringSetsCache::~StringSetsCache()
{
	clear();
}


void StringSetsCache::addPair(id_d_t object_type_id, 
			      const std::string& encoded_feature_name,
			      id_d_t string_set_id, 
			      const std::string& stringToAdd)
{
	OTIDStringPair mypair(object_type_id, encoded_feature_name);
	SSCMap::iterator it = m_sets.find(mypair);
	if (it == m_sets.end()) {
		StringSetCache *pSSC = new StringSetCache(object_type_id, encoded_feature_name);
		m_sets.insert(std::make_pair(mypair, pSSC));
		it = m_sets.find(mypair);
	}

	it->second->addPair(string_set_id, stringToAdd);
}


bool StringSetsCache::getStringFromID_D(id_d_t object_type_id, 
					const std::string& encoded_feature_name,
					id_d_t set_id, 
					std::string& result) const
{
	OTIDStringPair mypair(object_type_id, encoded_feature_name);
	SSCMap::const_iterator ci = m_sets.find(mypair);
	if (ci == m_sets.end()) {
		return false; // Not there.
	} else {
		return ci->second->getStringFromID_D(set_id, result);
	}	
}

 // Returns false on not there, true if there.
bool StringSetsCache::getID_DFromString(id_d_t object_type_id, 
					const std::string& encoded_feature_name,
					const std::string& stringToFind, 
					id_d_t& /* out */ result) const
{
	OTIDStringPair mypair(object_type_id, encoded_feature_name);
	SSCMap::const_iterator ci = m_sets.find(mypair);
	if (ci == m_sets.end()) {
		return false; // Not there.
	} else {
		return ci->second->getID_DFromString(stringToFind, result);
	}	
}


void StringSetsCache::setIsComplete(id_d_t object_type_id, 
				    const std::string& encoded_feature_name, 
				    bool bIsComplete)
{
	OTIDStringPair mypair(object_type_id, encoded_feature_name);
	SSCMap::iterator it = m_sets.find(mypair);

	// We might as well add it...
	// If the caller really thinks it is complete,
	// who are we to say it is not,
	// even if it is empty right now.
	if (it == m_sets.end()) {
		StringSetCache *pSSC = new StringSetCache(object_type_id, encoded_feature_name);
		m_sets.insert(std::make_pair(mypair, pSSC));
		it = m_sets.find(mypair);
	}
	it->second->setIsComplete(bIsComplete);
}

bool StringSetsCache::getIsComplete(id_d_t object_type_id, 
				    const std::string& encoded_feature_name) const
{
	OTIDStringPair mypair(object_type_id, encoded_feature_name);
	SSCMap::const_iterator ci = m_sets.find(mypair);
	if (ci == m_sets.end()) {
		return false; // Not there.
	} else {
		return ci->second->getIsComplete();
	}	
}

StringSetCache *StringSetsCache::getSSC(id_d_t object_type_id, const std::string& encoded_feature_name)
{
	OTIDStringPair mypair(object_type_id, encoded_feature_name);
	SSCMap::iterator it = m_sets.find(mypair);

	// We might as well add it...
	// If the caller really thinks it is complete,
	// who are we to say it is not,
	// even if it is empty right now.
	if (it == m_sets.end()) {
		StringSetCache *pSSC = new StringSetCache(object_type_id, encoded_feature_name);
		m_sets.insert(std::make_pair(mypair, pSSC));
		it = m_sets.find(mypair);
	}
	return it->second;
}

void StringSetsCache::clear(void)
{
	SSCMap::iterator it = m_sets.begin();
	while (it != m_sets.end()) {
		delete it->second;
		++it;
	}
	m_sets.clear();
}
