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
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2004-2006  Ulrik Petersen
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
