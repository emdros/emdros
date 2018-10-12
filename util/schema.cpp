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

/*
 * Copyright (C) 2001-2018     Ulrik Sandborg-Petersen
 * Copyright (C) 2018-present  Sandborg-Petersen Holding ApS, Denmark
 *
 * Licensed under the MIT License.
 *
 * Please see the file COPYING in the root of the sources for more details.
 *
 */



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


