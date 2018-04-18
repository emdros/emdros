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
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2011-2012  Ulrik Sandborg-Petersen
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
