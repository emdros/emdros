/*
 * schema.cpp
 *
 * Classes to hold an Emdros DB schema
 *
 * Ulrik Petersen
 * Created: 5/31-2005
 * Last update: 1/2-2012
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2005-2012  Ulrik Sandborg-Petersen
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


#include "schema.h"
#include <string_func.h>



bool EmdrosMonadSets::populate(std::ostream *pOut)
{
	if (!ExecuteEmdrosString("GET MONAD SETS ALL GO")) {
		(*pOut) << "Error: Could not get monad sets\nfor schema tree.";
		return false;
	}

	Table *pMonadSetsTable = TakeOverTable(); // From base class
	TableIterator ti = pMonadSetsTable->iterator();
	std::string monad_set_name;
	while (ti.hasNext()) {
		TableRow& trow = ti.next();
		monad_set_name = trow.getColumn(1);
		monad_m mse_first = string2monad_m(trow.getColumn(2));
		monad_m mse_last = string2monad_m(trow.getColumn(3));

		if (m_map.find(monad_set_name) == m_map.end()) {
			m_map[monad_set_name] = SetOfMonads(mse_first, mse_last);
		} else {
			m_map[monad_set_name].add(mse_first, mse_last);
		}
	}

	delete pMonadSetsTable;
	return true;
}



// pOut used for messages
bool EmdrosEnumerations::populate(std::ostream *pOut)
{
	if (!ExecuteEmdrosString("SELECT ENUMERATIONS GO")) {
		(*pOut) << "Error: Could not get enumerations\nfor schema tree.";
		return false;
	}

	Table *pEnumerationsTable = TakeOverTable();  // From base clase
	TableIterator ti = pEnumerationsTable->iterator();
	while (ti.hasNext()) {
		TableRow& trow = ti.next();
		std::string enumeration_name = trow.getColumn(1);
    
		if (!ExecuteEmdrosString(std::string("SELECT ENUMERATION CONSTANTS FROM ENUM ") + enumeration_name + " GO")) {
			(*pOut) << "Error: Could not get enumeration constants for enumeration\n'" << enumeration_name << "' for schema tree.\n";
			continue;
		}

		StringSet sset;
		Table *pEnumerationConstantsTable = TakeOverTable(); // From base clase
		TableIterator ti2 = pEnumerationConstantsTable->iterator();
		while (ti2.hasNext()) {
			TableRow& trow2 = ti2.next();
			std::string enum_const_name = trow2.getColumn(1);
			sset.insert(enum_const_name);
		}

		m_map[enumeration_name] = sset;

		delete pEnumerationConstantsTable;
	}

	delete pEnumerationsTable;
	return true;
}

StringVec EmdrosEnumerations::getEnumNames(void) const
{
	StringVec result;
	EnumerationMap::const_iterator ci = m_map.begin();
	EnumerationMap::const_iterator cend = m_map.end();
	while (ci != cend) {
		result.push_back(ci->first);
		++ci;
	}
	return result;
}

StringVec EmdrosEnumerations::getEnumConstantNames(const std::string& enumeration_name) const
{
	StringVec result;

	// Return the empty list if the enumeration name is not there.
	EnumerationMap::const_iterator ci = m_map.find(enumeration_name);
	if (ci == m_map.end()) {
		// The empty list.
		return result;
	} else {
		const StringSet& sset = ci->second;
		StringSet::const_iterator ssetci = sset.begin();
		StringSet::const_iterator ssetcend = sset.end();
		while (ssetci != ssetcend) {
			result.push_back(*ssetci);
			++ssetci;
		}
		return result;
	}
}




bool EmdrosObjectTypes::populate(std::ostream *pOut)
{
	if (!ExecuteEmdrosString("SELECT OBJECT TYPES GO")) {
		(*pOut) << "Error: Could not get enumerations\nfor schema tree.";
		return false;
	}

	Table *pObjectTypesTable = TakeOverTable();
	TableIterator ti = pObjectTypesTable->iterator();
	while (ti.hasNext()) {
		TableRow& trow = ti.next();
		std::string object_type_name;
		str_tolower(trow.getColumn(1), object_type_name);

		m_map[object_type_name] = fi_list_t();

		if (!ExecuteEmdrosString(std::string("SELECT FEATURES FROM OBJECT TYPE [") + object_type_name + "] GO")) {
			(*pOut) << "Error: Could not get features for object type '" << object_type_name << "'.";
		}

		Table *pFeaturesTable = m_pEE->takeOverTable();
		TableIterator ti2 = pFeaturesTable->iterator();
		while (ti2.hasNext()) {
			TableRow& trow2 = ti2.next();
			std::string feature_name;
			str_tolower(trow2.getColumn(1), feature_name);

			// We might get an enumeration name which needs to
			// be lower-cased.  All others (e.g., 'id_d', 'string', 'integer' 
			// etc.) are already lower-cased as they need to be,
			// by virtue of being documented constants in the Emdros
			// interface.
			std::string feature_type_name;
			str_tolower(trow2.getColumn(2), feature_type_name);

			FeatureNameAndType fnat(feature_name, feature_type_name);

			m_map[object_type_name].push_back(fnat);
		}

		delete pFeaturesTable;
	}

	delete pObjectTypesTable;
	return true; 
}


StringVec EmdrosObjectTypes::getObjectTypeNames(void) const
{
	StringVec result;
	string_fi_list_map::const_iterator ci = m_map.begin();
	string_fi_list_map::const_iterator cend = m_map.end();
	while (ci != cend) {
		result.push_back(ci->first);
		++ci;
	}
	return result;
}

std::list<FeatureNameAndType> EmdrosObjectTypes::getFeaturesOfObjectType(const std::string& object_type_name) const
{
	std::string test_name;
	str_tolower(object_type_name, test_name);
	// Return the empty list if the object type name is not there.
	string_fi_list_map::const_iterator ci = m_map.find(test_name);
	if (ci == m_map.end()) {
		// The empty list.
		std::list<FeatureNameAndType> result;
		return result;
	} else {
		return ci->second;
	}
}

bool EmdrosObjectTypes::FeatureTypeIsEnum(const FeatureNameAndType& fnat)
{
	std::string feature_type_name = fnat.type;
	if (strcmp_nocase(feature_type_name, "integer") != 0
	    && strcmp_nocase(feature_type_name, "id_d") != 0
	    && strcmp_nocase(feature_type_name, "string") != 0
	    && strcmp_nocase(feature_type_name, "ascii") != 0) {
		return true;
	} else {
		return false;
	}
}






EmdrosSchema::EmdrosSchema(EmdrosEnv *pEE)
	: m_enums(pEE), m_object_types(pEE), m_monad_sets(pEE)
{
}

EmdrosSchema::~EmdrosSchema()
{
	// Don't delete pEE; you don't own it!
}

// pOut used for messages.
bool EmdrosSchema::populate(std::ostream *pOut)
{
	if (m_enums.populate(pOut)) {
		if (m_object_types.populate(pOut)) {
			if (m_monad_sets.populate(pOut)) {
				return true;
			} else {
				return false;
			}
		} else {
			return false;
		}
	} else {
		return false;
	}
}


