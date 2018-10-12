/*
 * enum_const_cache.h
 *
 * EnumConstCache class
 *
 * Ulrik Petersen
 * Created: 1/20-2004
 * Last update: 4/29-2006
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


/**@file enum_const_cache.h
 *@brief Header file for EnumConstCache (EMdF layer)
 */


#ifndef ENUM_CONST_CACHE__H__
#define ENUM_CONST_CACHE__H__

#include "emdf.h"
#include "infos.h"
#include <vector>
#include <map>
#include <string>

/** A std::map mapping an int to an EnumConstInfo pointer.
 *@internal.
 */
typedef std::map<int, EnumConstInfo*> ValueECIMap;

/** A pointer to a ValueECIMap.
 *@internal
 */
typedef ValueECIMap* PValueECIMap;

/** A std::map mapping an id_d_t to a PValueECIMap.
 *@internal
 */
typedef std::map<id_d_t, PValueECIMap> IDD_VECIMap;

/** A std::map mapping a std::string to an EnumConstInfo pointer.
 *@internal
 */
typedef std::map<std::string, EnumConstInfo*> StringECIMap;

/** A pointer to a StringECIMap.
 *@internal
 */
typedef StringECIMap* PStringECIMap;

/** A std::map mapping an id_d_t to a PStringECIMap.
 *@internal
 */
typedef std::map<id_d_t, PStringECIMap> IDD_SECIMap;

/** A std::map mapping an id_d_t to a std::string.
 *@internal
 */
typedef std::map<id_d_t, std::string> IDD_StringMap; // enum_id to enum_name

/** A std::map mapping a std::string to an id_d_t.
 *@internal
 */
typedef std::map<std::string, id_d_t> String_IDDMap; // enum_name to enum_id

/** A cache of enumeration constants.
 *@internal
 */
class EnumConstCache {
	IDD_VECIMap m_idd_veci_map;
	IDD_SECIMap m_idd_seci_map;
	IDD_StringMap m_idd_enum_name_map;
	String_IDDMap m_enum_name_idd_map;
 public:
	EnumConstCache();
	~EnumConstCache();
	bool addEci(id_d_t enum_id, const std::string& enum_name, const EnumConstInfo& eci);
	void addEnum(id_d_t enum_id, const std::string& enum_name);
	void dropEnum(id_d_t enum_id);
	const EnumConstInfo* find(id_d_t enum_id, const std::string& enum_const_name) const;
	const EnumConstInfo* find(id_d_t enum_id, int enum_const_value) const;
	const EnumConstInfo* find(const std::string& enum_name, int enum_const_value) const;
	const EnumConstInfo* getDefault(id_d_t enum_id) const;
	void drop(id_d_t enum_id, const std::string& enum_const_name);
	void updateValue(id_d_t enum_id, const std::string& enum_const_name, int new_value);
	void setNewDefault(id_d_t enum_id, const std::string& enum_value_name);
	void clear();
	bool hasEnum(id_d_t enum_id);
};


#endif // ENUM_CONST_CACHE__H__
