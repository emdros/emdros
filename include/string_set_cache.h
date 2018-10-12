/*
 * string_set_cache.h
 *
 * StringSetCache and related classes
 *
 * Ulrik Petersen
 * Created: 3/9-2007 (March 9, 2007)
 * Last update: 11/27-2012
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


/**@file string_set_cache.h
 *@internal
 *@brief StringSetCache and related classes (EMdF layer)
 */


#ifndef STRING_SET_CACHE__H__
#define STRING_SET_CACHE__H__

#include <string>
#include <map>
#include <set>

#include "emdf.h"

class StringSetsCache; // Forward declaration
class Table; // Forward declaration
class SetOfMonads; // Forward declaration

class StringSetCache {
 private:
	id_d_t m_object_type_id;
	std::string m_encoded_feature_name;
	std::map<id_d_t, std::string> m_cache_set_id_to_string;
	std::map<std::string, id_d_t> m_cache_string_to_set_id;
	bool m_bIsComplete;
	bool m_bHasFilledStringToSetIDCache;
 public:
	StringSetCache(id_d_t object_type_id, const std::string& encoded_feature_name); 
	~StringSetCache();
	bool operator<(const StringSetCache& other); // Only compares m_object_type_id and m_encoded_feature_name
	void addPair(id_d_t set_id, const std::string& stringToAdd);
	bool getStringFromID_D(id_d_t set_id, std::string& result) const; // Returns false on not there, true if there.
	bool getID_DFromString(const std::string& stringToFind, id_d_t& /* out */ result);
	void setIsComplete(bool bIsComplete) { m_bIsComplete = bIsComplete; };
	bool getIsComplete(void) const { return m_bIsComplete; };
	void getID_DsAsSOM(SetOfMonads& som) const; 
	void getStringsAsTable(Table& result) const;
private:
	void fillStringToSetIDDCache(void);
	StringSetCache(const StringSetCache& other);
};

typedef std::pair<id_d_t, std::string> OTIDStringPair;
class StringSetsCache {
 private:
	typedef std::map<OTIDStringPair, StringSetCache*> SSCMap;
	SSCMap m_sets;
 public:
	StringSetsCache();
	~StringSetsCache();
	void addPair(id_d_t object_type_id, const std::string& encoded_feature_name,
		     id_d_t string_set_id, const std::string& stringToAdd);
	bool getStringFromID_D(id_d_t object_type_id, const std::string& encoded_feature_name,
			       id_d_t set_id, std::string& result) const; // Returns false on not there, true if there.
	bool getID_DFromString(id_d_t object_type_id, const std::string& encoded_feature_name,
			       const std::string& stringToFind, id_d_t& /* out */ result) const;
	void setIsComplete(id_d_t object_type_id, const std::string& encoded_feature_name, bool bIsComplete);
	bool getIsComplete(id_d_t object_type_id, const std::string& encoded_feature_name) const;
	StringSetCache *getSSC(id_d_t object_type_id, const std::string& encoded_feature_name);
	void clear(void);
};


#endif // STRING_SET_CACHE__H__
