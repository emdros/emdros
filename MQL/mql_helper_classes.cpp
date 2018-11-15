/*
 * mql_helper_classes.cpp
 *
 * MQL helper classes
 *
 * Ulrik Petersen
 * Created: 2/27-2001
 * Last update: 11/14-2018
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


/**@file mql_helper_classes.cpp
 *@brief Implementation of MQL helper classes (MQL layer)
 *
 * This file contains implementations of most of the non-statement,
 * non-query classes in MQL.
 *
 * The classes are typically defined either in mql_types.h or in the
 * corresponding mql_XXX_statements.h file.
 */



#include <emdfdb.h>
#include <string_list.h>
#include <inst.h>
#include <sstream>
#include <mql_types.h>
#include <mql_error.h>
#include <inst_object.h>
#include <string_func.h>
#include <mql_execution_environment.h>
#include <mql_object.h>
#include <mql_sheaf.h>
#include <mql_object_type_statements.h>
#include <mql_monads_statements.h>
#include <mql_enumeration_statements.h>
#include <mql_object_statements.h>
#include <mql_query.h>
#include <monads.h>
#include <debug.h>

#include <climits> // For LONG_MAX and LONG_MIN



MaxRange::MaxRange(monad_m max_range)
	: m_max_range(max_range)
{
}

MaxRange::MaxRange(const std::string* pOTN, const std::string *pSOMFeatureName)
	: m_max_range(0),
	  m_object_type_name(*pOTN),
	  m_SOM_feature_name(*pSOMFeatureName)
{
	delete pOTN;
	delete pSOMFeatureName;
}

MaxRange::~MaxRange()
{
}

void MaxRange::weed(MQLExecEnv *pEE, bool& bResult)
{
	UNUSED(pEE); // FIXME: Remove from parameters
	
	bResult = true;
}

bool MaxRange::symbol(MQLExecEnv *pEE, bool& bResult)
{
	// If we don't have an object type name, don't bother
	// checking.
	if (m_object_type_name.empty()) {
		m_feature_type_id = FEATURE_TYPE_SET_OF_MONADS;
		bResult = true;
		return true;
	}

	bool bDBResult;
	bool bObjectTypeExists;
	bDBResult = pEE->pDB->objectTypeExists(m_object_type_name,
					       bObjectTypeExists, 
					       m_object_type_id,
					       m_objectRangeType,
					       m_monadUniquenessType);

	// Was there a DB error?
	if (!bDBResult) {
		pEE->pError->appendError("DB error checking existence of object type name '" + m_object_type_name + "'.\n");
		return false;
	}

	// Was the object type there?
	if (!bObjectTypeExists) {
		pEE->pError->appendError("Object type name '" + m_object_type_name + "' does not exist.\n");
		bResult = false;
		return true;
	}

	if (strcmp_nocase(m_SOM_feature_name, "monads") == 0) {
		bResult = true;
		m_feature_type_id = FEATURE_TYPE_SET_OF_MONADS;
		return true;
	} else {
		m_feature_type_id = FEATURE_TYPE_INTEGER;
		pEE->pError->appendError("On the WITH MAX RANGE clause, the feature " + m_SOM_feature_name + "\nis not 'monads'. Other monad sets have not yet been implemented.\n");
		bResult = false;
		return true;
		
		/*
		bool bFeatureExists;
		std::string dummy_default_value;
		bool dummy_is_computed;
		if (!pEE->pDB->featureExists(m_our_monad_set,
					     object_type_id,
					     bFeatureExists,
					     m_feature_type_id,
					     dummy_default_value,
					     dummy_is_computed))
			return false;
		if (!bFeatureExists) {
			pEE->pError->appendError("The feature " + m_our_monad_set + " does not exist\non the object type of the object block with a monad set relation clause.\n");
			bResult = false;
			return true;
		} else {
			bResult = true;
			return true;
		}
		*/
	}
}

bool MaxRange::type(MQLExecEnv *pEE, bool& bResult)
{
	// If we don't have an object type name, don't bother
	// checking.
	if (m_object_type_name.empty()) {
		bResult = true;
		return true;
	}

	if (featureTypeIdIsSOM(m_feature_type_id)) {
		bResult = true;
	} else {
		bResult = false;
		pEE->pError->appendError("On the WITH MAX RANGE clause, the feature " + m_SOM_feature_name + "\nis not of type 'SET OF MONADS'.\n");
	}
	return true;
}

bool MaxRange::monads(MQLExecEnv *pEE, bool& bResult)
{
	// If we don't have an object type name, don't bother
	// checking.
	if (m_object_type_name.empty()) {
		bResult = true;
		return true;
	}

	if (!pEE->pDB->getLargestObjectLength(m_object_type_name,
					      m_objectRangeType,
					      m_max_range)) {
		pEE->pError->appendError("Could not get largest object length for feature 'monads' on\nobject type name '" + m_object_type_name + "' .\n");
		return false;
	}

	// No compiler error.
	bResult = true;

	// No DB error.
	return true;
}

bool MaxRange::exec(MQLExecEnv *pEE)
{
	UNUSED(pEE);
	
	return true;
}




//////////////////////////////////////////////////////////////
//
// MQLType 
//
//////////////////////////////////////////////////////////////
MQLType::MQLType(std::string* Enumeration, bool list_of, bool with_index) // For Enumeration and list of enumeration
{
	if (list_of) {
		m_type = kListOfEnumeration;
	} else {
		m_type = kEnumeration;
	}
	m_Enumeration = Enumeration;

	// Doesn't make sense for enumerations, since it is an integer type.
	m_from_set = false; 

	// May make sense on an enumeration, especially on the BPT engine,
	// so allow it.
	m_with_index = with_index;
}

MQLType::MQLType(eType type, bool withIndex) // For kInteger and kID_D
{
	ASSERT_THROW(type == kInteger || type == kID_D || type == kSetOfMonads,
		     "Type was neither kInteger nor kID_D nor kSetOfMonads");
	m_type = type;
	m_Enumeration = 0;

	// Doesn't make sense for enumerations, since it is an integer type.
	m_from_set = false; 

	// But this might make sense, so we let the user decide.
	m_with_index = withIndex;
}

MQLType::MQLType(eType type) // For kListOfInteger and kListOfID_D
{
	ASSERT_THROW(type == kListOfInteger || type == kListOfID_D,
		     "type was neither kListOfInteger nor kListOfID_D");
	m_type = type;
	m_Enumeration = 0;

	// Doesn't make sense for integers
	m_from_set = false; 

	// Also doesn't make much sense, since this is a list...
	m_with_index = false;
}

MQLType::MQLType(eType type, bool fromSet, bool withIndex) // For kString and kASCII
{
	ASSERT_THROW(type == kString || type == kASCII,
		     "Type was neither kString nor kASCII");
	m_type = type;
	m_Enumeration = 0;
	m_from_set = fromSet;
	m_with_index = withIndex;
}

MQLType::MQLType(eObjectRangeType som_range_type) // For kSetOfMonads
{
	m_type = kSetOfMonads;
	m_Enumeration = 0;
	m_from_set = false;
	m_with_index = false;
	m_som_range_type = som_range_type;
}


MQLType::~MQLType()
{
	delete m_Enumeration;
}

//
// - Checks that the enumeration exists.  Result in bResult.
//
// - If it exists, gets its enum_id into m_type_id.
//
bool MQLType::symbolEnumerationExists(MQLExecEnv *pEE, bool& bResult)
{
	ASSERT_THROW(m_type == kEnumeration
		     || m_type == kListOfEnumeration,
		     "Type was neither kEnumeration nor kListOfEnumeration");
	return pEE->pDB->enumExists(*m_Enumeration, bResult, m_type_id);
}

//
// Assign type id to m_type_id.
//
bool MQLType::typeAssignTypeID(MQLExecEnv *pEE)
{
	bool bResult; // Dummy: It should already have been checked
	switch (m_type) {
	case kInteger:
		m_type_id = FEATURE_TYPE_INTEGER;
		if (m_with_index) {
			m_type_id |= FEATURE_TYPE_WITH_INDEX;
		}
		break;
	case kListOfInteger:
		m_type_id = FEATURE_TYPE_LIST_OF_INTEGER;
		break;
	case kString:
		m_type_id = FEATURE_TYPE_STRING;
		if (m_from_set) {
			m_type_id |= FEATURE_TYPE_FROM_SET;
		}
		if (m_with_index) {
			m_type_id |= FEATURE_TYPE_WITH_INDEX;
		}
		break;
	case kASCII:
		m_type_id = FEATURE_TYPE_ASCII;
		if (m_from_set) {
			m_type_id |= FEATURE_TYPE_FROM_SET;
		}
		if (m_with_index) {
			m_type_id |= FEATURE_TYPE_WITH_INDEX;
		}
		break;
	case kID_D:
		m_type_id = FEATURE_TYPE_ID_D;
		if (m_with_index) {
			m_type_id = m_type_id | FEATURE_TYPE_WITH_INDEX;
		}
		break;
	case kListOfID_D:
		m_type_id = FEATURE_TYPE_LIST_OF_ID_D;
		break;
	case kEnumeration:
		if (!pEE->pDB->enumExists(*m_Enumeration, bResult, m_type_id))
			return false;
		// enumerations can't have indexes
		m_type_id |= FEATURE_TYPE_ENUM;
		if (m_with_index) {
			m_type_id |= FEATURE_TYPE_WITH_INDEX;
		}
		break;
	case kListOfEnumeration:
		if (!pEE->pDB->enumExists(*m_Enumeration, bResult, m_type_id))
			return false;
		m_type_id |= FEATURE_TYPE_LIST_OF_ENUM;
		// lists of enumerations can't have indexes
		break;
	case kSetOfMonads:
		m_type_id = FEATURE_TYPE_SET_OF_MONADS;
		break;
	default:
		ASSERT_THROW(false, "Unknown MQLType type.");
		break;
	};
	return true;
}



//////////////////////////////////////////////////////////////
//
// FeatureEnv 
//
//////////////////////////////////////////////////////////////
FeatureEnv::FeatureEnv()
	: m_feature_name2index_map(257)
{
}

FeatureEnv::~FeatureEnv()
{
	clearAndSetSize(0);
}

void FeatureEnv::clearAndSetSize(int size)
{
	m_feature_name2index_map.clear();

	// Clear m_MQLType_vec, deleting all contents
	std::vector<MQLType*>::size_type length = m_MQLType_vec.size();
	std::vector<MQLType*>::size_type index = 0;
	while (index < length) {
		MQLType *pMQLType = m_MQLType_vec[index];
		delete pMQLType;
		++index;
	}
	m_MQLType_vec.clear();


	m_FeatureInfo_vec.clear();

	if (size > 0) {
		m_MQLType_vec.resize(size, 0);
		m_FeatureInfo_vec.resize(size, FeatureInfo());
	}
}

void FeatureEnv::addEntry(int feature_index, const FeatureInfo& fi, MQLType *pMQLType)
{
	m_feature_name2index_map.addEntry(fi.getHumanReadableFeatureName(), feature_index);
	m_MQLType_vec[feature_index] = pMQLType;
	m_FeatureInfo_vec[feature_index] = fi;
}

int FeatureEnv::size() const
{
	return (int) m_MQLType_vec.size();
}

int FeatureEnv::findFeatureIndex(const std::string& locase_feature_name) const
{
	int index;
	bool bExists = m_feature_name2index_map.entryExists(locase_feature_name, &index);
	ASSERT_THROW(bExists,
		     "Error in FeatureEnv::findFeatureIndex(\"" + locase_feature_name + "\"): feature name not found.");
	return index;
		/*
	std::map<std::string, int>::const_iterator ci = 
		m_feature_name2index_map.find(locase_feature_name);
	ASSERT_THROW(ci != m_feature_name2index_map.end(),
		     "Error in FeatureEnv::findFeatureIndex(\"" + locase_feature_name + "\"): feature name not found.");
	return ci->second;
		*/
}

MQLType* FeatureEnv::findMQLType(int feature_index) const
{
	ASSERT_THROW(feature_index >= 0 && feature_index < (int) m_MQLType_vec.size(),
		     "Error in FeatureEnv::findMQLType(" + int2string(feature_index) + "): Index out of range (" + int2string((int) m_MQLType_vec.size()) + ").");
	return m_MQLType_vec[feature_index];
}

const FeatureInfo& FeatureEnv::getFeatureInfoConstRef(int feature_index) const
{
	ASSERT_THROW(feature_index >= 0 && feature_index < (int) m_FeatureInfo_vec.size(),
		     "Error in FeatureEnv::getFeatureInfoConstRef(" + int2string(feature_index) + "): Index out of range (" + int2string((int) m_FeatureInfo_vec.size()) + ").");
	return m_FeatureInfo_vec[feature_index];
}


//////////////////////////////////////////////////////////////
//
// Expression 
//
//////////////////////////////////////////////////////////////

// pSOM == nil means empty set.
Expression::Expression(MQLMonadSetElement *pSOM) // For kExprSetOfMonads
{
	m_string = 0;
	m_list_of_identifier = 0;
	m_list_of_integer = 0;
	m_type = kExprSetOfMonads;
	m_integer = 0;
	m_pSOM = 0;
	m_pMQLMSE = pSOM;
}

Expression::Expression(const SetOfMonads& som) // For kExprSetOfMonads
{
	m_string = 0;
	m_list_of_identifier = 0;
	m_list_of_integer = 0;
	m_type = kExprSetOfMonads;
	m_integer = 0;
	m_pSOM = new SetOfMonads(som);
	m_pMQLMSE = 0;
}

Expression::Expression(long integer) // For kExprSetOfMonads
{
	m_string = 0;
	m_list_of_identifier = 0;
	m_list_of_integer = 0;
	m_type = kExprInteger;
	m_integer = integer;
	m_pSOM = 0;
	m_pMQLMSE = 0;
}

Expression::Expression() // For kExprEmptyList
{
	m_string = 0;
	m_list_of_identifier = 0;
	m_list_of_integer = 0;
	m_type = kExprEmptyList;
	m_integer = 0;
	m_pSOM = 0;
	m_pMQLMSE = 0;
}

Expression::Expression(std::string* str, eExprType type) // For kExprString and kExprIdentifier
{
	if (type == kExprString || type == kExprIdentifier) {
		m_string = str;
		m_type = type;
		m_list_of_identifier = 0;
		m_list_of_integer = 0;
		m_integer = 0;
		m_pSOM = 0;
		m_pMQLMSE = 0;
	} else {
		ASSERT_THROW(false,
			     "Type was neither kExprString nor kExprIdentifier");
	}
}

Expression::Expression(StringList *pStringList, eExprType type)  // for kExprListOfIdentifier and for kExprEmptyList (pStringList must be 0 in that case)
{
	if (type == kExprListOfIdentifier || type == kExprEmptyList) {
		m_list_of_identifier = pStringList;
		m_list_of_integer = 0;
		m_string = 0;
		m_type = type;
		m_integer = 0;
		m_pSOM = 0;
		m_pMQLMSE = 0;
	} else {
		ASSERT_THROW(false,
			     "Type was neither kExprListOfIdentifier nor kExprEmptyList");
	}
}

Expression::Expression(IntegerList *pIntegerList)  // For kExprListOfInteger
{
	m_list_of_integer = pIntegerList;
	m_list_of_identifier = 0;
	m_string = 0;
	m_type = kExprListOfInteger;
	m_integer = 0;
	m_pSOM = 0;
	m_pMQLMSE = 0;
}


Expression::~Expression()
{
	delete m_string;
	delete m_list_of_identifier;
	delete m_list_of_integer;
	delete m_pSOM;
	delete m_pMQLMSE;
}

void Expression::weed(MQLExecEnv *pEE, bool& bResult)
{
	if (m_type == kExprSetOfMonads) {
		if (m_pMQLMSE != 0) {
			m_pSOM = new SetOfMonads();
			m_pMQLMSE->weed(pEE, true, bResult);
			if (!bResult) {
				return;
			}
			m_pMQLMSE->monadsBuildSet(*m_pSOM);
			delete m_pMQLMSE;
			m_pMQLMSE = 0;
		} else if (m_pSOM == 0) {
			m_pSOM = new SetOfMonads();
		}
	}
}

long Expression::getInteger()
{
	ASSERT_THROW(m_type == kExprInteger,
		     "type was not kExprInteger");
	return m_integer;
}

const std::string& Expression::getString()
{
	ASSERT_THROW(m_type == kExprString,
		     "type was not kExprString");
	return *m_string;
}

const SetOfMonads& Expression::getSOM() const
{
	ASSERT_THROW(m_type == kExprSetOfMonads,
		     "type was not kExprSetOfMonads");
	ASSERT_THROW(m_pSOM != 0,
		     "m_pSOM was nil. Weed wasn't called on Expression.");
	return *m_pSOM;
}

const std::string& Expression::getIdentifier()
{
	ASSERT_THROW(m_type == kExprIdentifier,
		     "Type was not kExprIdentifier");
	return *m_string;
}

long Expression::getEnumValue()
{
	ASSERT_THROW(m_type == kExprIdentifier,
		     "Type is not kExprIdentifier");
	return m_enum_value;
}

IntegerList *Expression::getCopyOfIntegerList(void) const
{ 
	if (m_list_of_integer == 0) {
		return 0; 
	} else {
		return new IntegerList(*m_list_of_integer); 
	}
}

StringList *Expression::getCopyOfIdentifierList(void) const
{ 
	if (m_list_of_identifier == 0) {
		return 0; 
	} else {
		return new StringList(*m_list_of_identifier); 
	}
}


bool Expression::typeTypeCompatibility(MQLExecEnv *pEE, MQLType* other_type, bool& bResult)
{
	bool dummy_is_default; // Dummy variable
	StringListConstIterator ci; 
	switch(m_type) {
	case kExprInteger:
		bResult = (other_type->getType() == kInteger
			   || other_type->getType() == kID_D);
		if (bResult && other_type->getType() == kID_D) {
			bResult = m_integer >= 0;
			if (!bResult) {
				pEE->pError->appendError("You cannot assign a number below 0 to an id_d.\n");
			}
		}
		break;
	case kExprString:
		bResult = other_type->getType() == kString || other_type->getType() == kASCII;
		break;
	case kExprIdentifier:
		// Is other an enumeration?
		bResult = other_type->getType() == kEnumeration;
   
		// If other isn't enumeration, we won't bother checking existence
		if (!bResult)
			return true;

		// If other is an enumeration, we must check that the enumeration
		// constant exists.
		// This assumes that we have run typeAssignTypeID on the other_type.
		m_enum_id = other_type->getTypeId();
		if (!pEE->pDB->enumConstExists(*m_string, m_enum_id, bResult, 
					       m_enum_value, dummy_is_default))
			return false;

		// If it didn't exist, we return now
		if (!bResult)
			return true;

		// Else, the result is true.
		bResult = true;
		break;
	case kExprEmptyList:
		bResult = other_type->getType() == kListOfInteger
			|| other_type->getType() == kListOfID_D
			|| other_type->getType() == kListOfEnumeration;
		if (bResult && other_type->getType() == kListOfEnumeration) {
			m_list_of_integer = new IntegerList();
		}
		break;
	case kExprListOfInteger:
		bResult = other_type->getType() == kListOfInteger
			|| other_type->getType() == kListOfID_D;
		break;
	case kExprSetOfMonads:
		bResult = other_type->getType() == kSetOfMonads;
		break;
	case kExprListOfIdentifier:
		bResult = other_type->getType() == kListOfEnumeration;

		if (!bResult) 
			return true;

		// We wish to get a list of the enumeration constant values
		m_list_of_integer = new IntegerList();

		// If other is an enumeration, we must check that the enumeration
		// constants exist.
		// This assumes that we have run typeAssignTypeID on the other_type.
		m_enum_id = other_type->getTypeId();
		ci = m_list_of_identifier->const_iterator();
		while (ci.hasNext()) {
			std::string enum_const = ci.next();
			long enum_value;
			bool bExists;

			if (!pEE->pDB->enumConstExists(enum_const, m_enum_id, bExists, 
						       enum_value, dummy_is_default))
				return false;

			if (!bExists) {
				bResult = false;
				pEE->pError->appendError("Enumeration constant " + enum_const + " does not exist for the enumeration " + other_type->getEnumeration() +  ".\n");
			} else {
				// So it existed, so we add it to the list of integers
				m_list_of_integer->addIntegerBack(enum_value);
			}
		}
		// If one of them didn't exist, we return now
		if (!bResult)
			return true;

		// Else, the result is true.
		bResult = true;

		break;
	}

	return true;
}

bool Expression::getAsString(MQLExecEnv *pEE, std::string& result, bool bConvertEnumConstToInteger)
{
	std::ostringstream mystrstream;
	bool bEnumConstExists;
	bool dummy_is_default;
	long value;
	switch(m_type) {
	case kExprInteger:
		mystrstream << m_integer;
		result = mystrstream.str();
		break;
	case kExprString:
		result = *m_string;
		break;
	case kExprIdentifier:
		if (bConvertEnumConstToInteger) {
			if (!pEE->pDB->enumConstExists(*m_string,
						       m_enum_id,
						       bEnumConstExists,
						       value,
						       dummy_is_default))
				return false;
			mystrstream << value;
			result = mystrstream.str();
		} else {
			result = *m_string;
		}
		break;
	case kExprListOfInteger:
		if (m_list_of_integer == 0) {
			result = "";
		} else {
			result = m_list_of_integer->getDelimitedString(DEFAULT_LIST_DELIMITER);
		}
		break;
	case kExprListOfIdentifier:
		if (m_list_of_identifier == 0) {
			result = "";
		} else {
			if (m_list_of_integer == 0) {
				result = m_list_of_identifier->getDelimitedString(DEFAULT_LIST_DELIMITER);
			} else {
				result = m_list_of_integer->getDelimitedString(DEFAULT_LIST_DELIMITER);
			}
		}
		break;
	case kExprEmptyList:
		result = "";
		break;
	case kExprSetOfMonads:
		ASSERT_THROW(m_pSOM != 0,
			     "m_pSOM was nil");
		result = m_pSOM->toString();
		break;
	};

	// If we got this far, there were no DB errors
	return true;
}






//////////////////////////////////////////////////////////////
//
// FeatureDeclaration 
//
//////////////////////////////////////////////////////////////
FeatureDeclaration::FeatureDeclaration(std::string* feature_name,
				       MQLType* feature_type,
				       Expression* default_specification,
				       bool is_computed,
				       FeatureDeclaration* next)
{
	m_feature_name = feature_name;
	m_type = feature_type;
	m_default_specification = default_specification;
	m_is_computed = is_computed;
	m_next = next;
}

FeatureDeclaration::~FeatureDeclaration()
{
	delete m_next;
	delete m_feature_name;
	delete m_type;
	delete m_default_specification;
}

//
// - Check that no computed features are declared.
//
void FeatureDeclaration::weed(MQLExecEnv *pEE, bool& bResult)
{
	// First deal with next
	if (m_next != 0) {
		m_next->weed(pEE, bResult);
		if (!bResult)
			return;
	}

	// Then deal with ourselves

	//
	// - Check that computed features are not declared
	//
	weedComputedFeatureNotDeclared(pEE, bResult);
	if (!bResult)
		return;

	//
	// - Check default specification Expression
	//
	if (m_default_specification != 0) {
		m_default_specification->weed(pEE, bResult);
		if (!bResult) {
			return;
		}
	}

	// If we got this far, there were no compiler errors
	bResult = true;
	return;
}

//
// - Check that a computed feature is not declared.
//
void FeatureDeclaration::weedComputedFeatureNotDeclared(MQLExecEnv *pEE, bool& bResult)
{
	FeatureInfo dummy_feature_info(getFeatureName(),
				       "",
				       FEATURE_TYPE_INTEGER,
				       "0");
	bResult = !dummy_feature_info.getIsComputed();
	if (!bResult) {
		pEE->pError->appendError("Feature '" + getFeatureName() + "' is implicitly declared and cannot be declared explicitly.\n");
	}
	return;
}

bool FeatureDeclaration::symbolEnumerationsExist(MQLExecEnv *pEE, bool& bResult)
{
	// Deal with next first
	if (m_next != 0) {
		if (!m_next->symbolEnumerationsExist(pEE, bResult))
			return false;
		if (!bResult)
			return true;
	}

	// Then deal with outselves
	if (m_type->getType() == kEnumeration) {
		if (!m_type->symbolEnumerationExists(pEE, bResult))
			return false;
		if (!bResult) {
			pEE->pError->appendError("Enumeration " + m_type->getEnumeration() + " used as the type of feature " + *m_feature_name + " does not exist.\n");
			return true;
		}
	} else { // Not enumeration
		bResult = true;
	}
	return true;
}

bool FeatureDeclaration::symbolEnumConstantsExist(MQLExecEnv *pEE, bool& bResult)
{
	// Deal with next first
	if (m_next != 0) {
		if (!m_next->symbolEnumConstantsExist(pEE, bResult))
			return false;
		if (!bResult)
			return true;
	}

	// Then deal with outselves
	bResult = true;
	if (m_type->getType() == kEnumeration) {
		if (m_default_specification != 0 
		    && m_default_specification->getKind() == kExprIdentifier) {
			id_d_t enum_id = m_type->getTypeId();
			long value;
			bool is_default;
			if (!pEE->pDB->enumConstExists(m_default_specification->getIdentifier(),
						       enum_id, bResult, value, is_default)) {
				return false;
			} else {
				// bResult has now been set to whatever it is...
			}
		} else { // Not identifier, or empty: Will be caught in type-checker
			bResult = true;
		}
	} else { // Not enumeration
		bResult = true;
	}

	if (!bResult) {
		pEE->pError->appendError(std::string("Enumeration constant ") + m_default_specification->getIdentifier() + " does not exist in the enumeration " + m_type->getEnumeration() + " used as the type of feature " + *m_feature_name + ".\n");
	}
	return true;
}

//
// - Check that none of the features exist for the object type 
//   object_type_id.
//
bool FeatureDeclaration::symbolFeaturesMustNotExist(MQLExecEnv *pEE, bool& bResult, id_d_t object_type_id)
{
	// Deal with next first
	if (m_next != 0) {
		if (!m_next->symbolFeaturesMustNotExist(pEE, bResult, object_type_id))
			return false;
		if (!bResult)
			return true;
	}

	bool bFeatureExists;
	id_d_t feature_type_id;      // Dummy variable
	std::string default_value;   // Dummy variable
	bool is_computed;            // Dummy variable
	if (!pEE->pDB->featureExists(*m_feature_name,
				     object_type_id,
				     bFeatureExists,
				     feature_type_id,
				     default_value,
				     is_computed))
		return false;

	bResult = !bFeatureExists;
	return true;
}

//
// - Assign type-id to each feature.
//
bool FeatureDeclaration::typeAssignTypeID(MQLExecEnv *pEE)
{
	// Deal with next first
	if (m_next != 0) {
		if (!m_next->typeAssignTypeID(pEE))
			return false;
	}

	// Then deal with outselves
	if (!m_type->typeAssignTypeID(pEE))
		return false;

	return true;
}

//
// - Check that there is type-compatibility between the feature
//   and the default specification, and make a default specification
//   if it does not exist.
//
bool FeatureDeclaration::typeTypeCompatibility(MQLExecEnv *pEE, bool& bResult)
{
	// Deal with next first
	if (m_next != 0) {
		if (!m_next->typeTypeCompatibility(pEE, bResult))
			return false;
		if (!bResult)
			return true;
	}

	// Then deal with ourselves
	if (m_default_specification != 0) {
		if (!m_default_specification->typeTypeCompatibility(pEE, m_type, bResult)) {
			pEE->pError->appendError("Database error in checking type compatibility of feature default value of " + *m_feature_name + ".\n");
			return false;
		}
		if (!bResult) {
			pEE->pError->appendError("Feature " + *m_feature_name + " is not type-compatible with its default value.\n");
			return true;
		}
	} else { // m_default_specification == 0
		std::string *pDefault_string;
		switch (m_type->getType()) {
		case kInteger:
			m_default_specification = new Expression((long) 0);
			break;
		case kString:
			pDefault_string = new std::string("");
			m_default_specification = new Expression(pDefault_string, kExprString);
			break;
		case kASCII:
			pDefault_string = new std::string("");
			m_default_specification = new Expression(pDefault_string, kExprString);
			break;
		case kID_D:
			m_default_specification = new Expression((id_d_t) NIL);
			break;
		case kEnumeration:
			pDefault_string = new std::string;
			if (!pEE->pDB->getDefault(m_type->getTypeId(),
						  bResult,
						  *pDefault_string)) {
				pEE->pError->appendError(std::string("Database error: Could not find default value in enumeration for feature ") + *m_feature_name + ".\n");
				delete pDefault_string;
				return false;
			} else {
				m_default_specification = new Expression(pDefault_string, kExprIdentifier);
			}
			if (!bResult) {
				pEE->pError->appendError(std::string("The specified default enumeraiton constant does not exist for the enumeration of feature ") + *m_feature_name + ".\n");
			}
			break;
		case kListOfInteger:
		case kListOfID_D:
		case kListOfEnumeration:
			m_default_specification = new Expression(); // kExprEmptyList
			break;
		case kSetOfMonads:
			m_default_specification = new Expression(static_cast<MQLMonadSetElement*>(0));
			m_default_specification->weed(pEE, bResult);
			break;
		}
	}

	return true;
}

bool FeatureDeclaration::getFeatureInfo(MQLExecEnv *pEE, FeatureInfo& fi)
{
	std::string feature_value;
	if (!getDefaultSpecification()->getAsString(pEE, feature_value, false))
		return false;
	fi = FeatureInfo(getFeatureName(),
			 "",
			 getType()->getTypeId(),
			 feature_value);
	return true;
}


bool FeatureDeclaration::execAdd(MQLExecEnv *pEE, const std::string& object_type_name, id_d_t object_type_id)
{
	// Deal with next first
	if (m_next != 0) {
		if (!m_next->execAdd(pEE, object_type_name, object_type_id))
			return false;
	}

	// Then deal with ourselves
	FeatureInfo fi;
	if (!getFeatureInfo(pEE, fi))
		return false;
	if (!pEE->pDB->addFeature(object_type_name, object_type_id, fi))
		return false;


	// If we got this far, there were no DB errors
	return true;
}


//////////////////////////////////////////////////////////////
//
// FeatureUpdate 
//
//////////////////////////////////////////////////////////////

FeatureUpdate::FeatureUpdate(FeatureDeclaration* feature_declaration, FeatureUpdate* next)
{
	m_AddOrRemove = kAdd;
	m_feature_addition = feature_declaration;
	m_feature_removal = 0;
	m_next = next;
}

FeatureUpdate::FeatureUpdate(std::string* feature_name, FeatureUpdate* next)
{
	m_AddOrRemove = kRemove;
	m_feature_addition = 0;
	m_feature_removal = feature_name;
	m_next = next;
}

FeatureUpdate::~FeatureUpdate()
{
	delete m_next;
	delete m_feature_addition;
	delete m_feature_removal;
}

void FeatureUpdate::weed(MQLExecEnv *pEE, bool& bResult)
{
	// First do next
	if (m_next != 0) {
		m_next->weed(pEE, bResult);
		if (!bResult)
			return;
	}


	// Then do ourselves
	switch (m_AddOrRemove) {
	case kAdd:
		// The FeatureDeclaration knows how to do this
		m_feature_addition->weed(pEE, bResult);
		return;
		break;
	case kRemove:
		{
			FeatureInfo dummy_feature_info(getFeatureRemoval(),
						       "",
						       FEATURE_TYPE_INTEGER,
						       "0");
			bResult = !dummy_feature_info.getIsComputed();
			
			if (!bResult) {
				pEE->pError->appendError("Cannot remove feature '" + getFeatureRemoval() + "'.\n");
			}
		}
		return;
	default:
		ASSERT_THROW(false,
			     "Kind unknown");
		break;
	};
	if (!bResult)
		return;
  
	// If we got this far, there were no errors
	bResult = true;
	return;
}

// 
// - Check that all the features that are to be removed do exist.
//   
// - Check that all the features that are to be added do not exist.
//
bool FeatureUpdate::symbolExistenceOfFeatures(MQLExecEnv *pEE, 
					      bool& bResult, 
					      id_d_t object_type_id)
{
	// First do next
	if (m_next != 0) {
		if (!m_next->symbolExistenceOfFeatures(pEE, bResult, object_type_id))
			return false;
		if (!bResult)
			return true;
	}

	// Then do ourselves
	std::string default_value;   // Dummy variable
	bool is_computed;            // Dummy variable
	switch (m_AddOrRemove) {
	case kAdd:
		// Features for addition must not exist.
		if (!m_feature_addition->symbolFeaturesMustNotExist(pEE, bResult, object_type_id)) {
			pEE->pError->appendError("Database error while checking whether feature " + *m_feature_removal + " exists.\n");
      
			return false;
		}
		if (!bResult) {
			pEE->pError->appendError("Feature " + m_feature_addition->getFeatureName() + " exists already.  Cannot add again.\n");
		}
		return true;
		break;
	case kRemove:
		// Features for removal must exist.
		if (!pEE->pDB->featureExists(*m_feature_removal,
					     object_type_id,
					     bResult,
					     m_feature_type_id_for_removal,
					     default_value,
					     is_computed)) {
			pEE->pError->appendError("Database error while checking whether feature " + *m_feature_removal + " exists.\n");
			return false;
		}
		if (!bResult) {
			pEE->pError->appendError("Feature " + *m_feature_removal + " does not exist for removal.\n");
		}
		return true;
	default:
		ASSERT_THROW(false,
			     "Unknown kind");
		return false; // Just so we don't get a warning
		break;
	}
	return false;
}

//
// - Check that the enumerations exist for the types   
//   whose features are enumerations
// - Check that, within these enumerations, any default
//   specification which is an enumeration constant exists
//   in that enumeration.
bool FeatureUpdate::symbolEnumerations(MQLExecEnv *pEE, bool& bResult)
{
	// First do next
	if (m_next != 0) {
		if (!m_next->symbolEnumerations(pEE, bResult))
			return false;
		if (!bResult)
			return true;
	}

	// Then do ourselves
	switch (m_AddOrRemove) {
	case kAdd:
		//
		// - Check that the enumerations exist for the types   
		//   whose features are enumerations
		if (!m_feature_addition->symbolEnumerationsExist(pEE, bResult))
			return false;
		if (!bResult) 
			return true;


		// - Check that, within these enumerations, any default
		//   specification which is an enumeration constant exists
		//   in that enumeration.
		if (!m_feature_addition->symbolEnumConstantsExist(pEE, bResult))
			return false;
		if (!bResult)
			return true;
		break;
	case kRemove:
		bResult = true;
		return true;
	default:
		ASSERT_THROW(false,
			     "Unknown task");
		return false; // Just so we don't get a warning
		break;
	};

	return true;
}

bool FeatureUpdate::typeAssignTypeID(MQLExecEnv *pEE)
{
	// Deal with next first
	if (m_next != 0) {
		if (!m_next->typeAssignTypeID(pEE))
			return false;
	}

	// Then deal with outselves
	if (m_feature_addition != 0) {
		if (!m_feature_addition->typeAssignTypeID(pEE))
			return false;
	}

	return true;
}

bool FeatureUpdate::typeTypeCompatibility(MQLExecEnv *pEE, bool& bResult)
{
	// Deal with next first
	if (m_next != 0) {
		if (!m_next->typeTypeCompatibility(pEE, bResult))
			return false;
		if (!bResult)
			return true;
	}

	// Then deal with ourselves
	if (m_feature_addition != 0) {
		if (!m_feature_addition->typeTypeCompatibility(pEE, bResult))
			return false;
		if (!bResult)
			return true;
	}

	return true;
}

bool FeatureUpdate::exec(MQLExecEnv *pEE, const std::string& object_type_name, id_d_t object_type_id)
{
	// First do next
	if (m_next != 0) {
		if (!m_next->exec(pEE, object_type_name, object_type_id))
			return false;
	}

	// Then do ourselves
	switch (m_AddOrRemove) {
	case kAdd:
		if (!m_feature_addition->execAdd(pEE, object_type_name, object_type_id))
			return false;
		break;
	case kRemove:
		if (!pEE->pDB->dropFeature(object_type_name, *m_feature_removal, m_feature_type_id_for_removal, object_type_id))
			return false;
		break;
	default:
		ASSERT_THROW(false,
			     "Unknown task");
		return false; // Just so we don't get a warning
		break;
	};
  

	// If we got this far, there were no DB errors
	return true;
}


//////////////////////////////////////////////////////////////
//
// MQLMonadSetElement 
//
//////////////////////////////////////////////////////////////

MQLMonadSetElement::MQLMonadSetElement(monad_m mse_first, 
				       monad_m mse_last, 
				       MQLMonadSetElement* next,
				       bool bIsFirst)
{
	m_mse_first = mse_first;
	m_mse_last = mse_last;
	m_next = next;
	m_bIsFirst = bIsFirst;
}

MQLMonadSetElement::MQLMonadSetElement(monad_m mse_singleton, 
				       MQLMonadSetElement* next,
				       bool bIsFirst)
{
	m_mse_first = m_mse_last = mse_singleton;
	m_next = next;
	m_bIsFirst = bIsFirst;
}

MQLMonadSetElement::~MQLMonadSetElement()
{
	if (m_bIsFirst) {
		MQLMonadSetElement *pCur = m_next;
		while (pCur != 0) {
			MQLMonadSetElement *pNext = pCur->m_next;
			delete pCur;
			pCur = pNext;
		}
	}
	// Don't do anything special for non-first ones, 
	// apart from the usual destruction.
}

void MQLMonadSetElement::weed(MQLExecEnv *pEE, bool bWeedPositive, bool& bResult)
{
	if (bWeedPositive) {
		weedPositive(pEE, bResult);
	}
	if (!bResult) {
		return;
	}
	weedMonotonic(pEE, bResult);
}

//
// - Checks that each monad set element is monotonic.
//   I.e., that mse_last >= mse_first.
//
void MQLMonadSetElement::weedMonotonic(MQLExecEnv *pEE, bool& bResult)
{
	MQLMonadSetElement *pElem = this;
	while (pElem != 0) {
		bResult = (pElem->m_mse_last >= pElem->m_mse_first);
		if (!bResult) {
			pEE->pError->appendError("Range in monad set must be monotonic (i.e., in 'A - B',\nB must be greater than or equal to A).\n");
			pEE->pError->appendError("The offending mse was " + monad_m2string(pElem->m_mse_first) + "-" + monad_m2string(pElem->m_mse_last) +".\n");
			return;
		}
		pElem = pElem->m_next;
	}
	return;
}

//
// - Checks that both mse_first and mse_last are positive.
// 
void MQLMonadSetElement::weedPositive(MQLExecEnv *pEE, bool& bResult)
{
	MQLMonadSetElement *pElem = this;
	while (pElem != 0) {
		bResult = (pElem->m_mse_first >= 1) && (pElem->m_mse_last >= 1);
		if (!bResult) {
			pEE->pError->appendError("All monads in a monad set must be positive (i.e., greater than or equal to 1).\n");
			pEE->pError->appendError("The offending mse was " + monad_m2string(pElem->m_mse_first) + "-" + monad_m2string(pElem->m_mse_last) +".\n");
			return;
		}
		pElem = pElem->m_next;
	}
}

void MQLMonadSetElement::monadsBuildSet(SetOfMonads& monads)
{
	MQLMonadSetElement *pElem = this;
	while (pElem != 0) {
		monads.add(pElem->m_mse_first, pElem->m_mse_last);
		pElem = pElem->m_next;
	}
}


//////////////////////////////////////////////////////////////
//
// MonadSetChainElement
//
//////////////////////////////////////////////////////////////

MonadSetChainElement::MonadSetChainElement(MQLMonadSetElement *myset, eSetOperator op, MonadSetChainElement *next)
{
	m_set = myset;
	m_operator = op;
	m_next = next;
}

MonadSetChainElement::~MonadSetChainElement()
{
	delete m_set;
	delete m_next;
}

//
// - Checks that each monad set element is monotonic.
//   I.e., that mse_last >= mse_first.
// - And that each element has only positive members
//
void MonadSetChainElement::weed(MQLExecEnv *pEE, bool& bResult)
{
	// First deal with next
	if (m_next != 0) {
		m_next->weed(pEE, bResult);
		if (!bResult)
			return;
	}

	// Then deal with ourselves
	ASSERT_THROW(m_set != 0,
		     "m_set was 0");
	m_set->weed(pEE, true, bResult);

	// Return
	return;
}

void MonadSetChainElement::exec(SetOfMonads& monads)
{
	// First deal with next
	if (m_next != 0) {
		m_next->exec(monads);
	} else { // m_next == 0
		// This is the first in the chain.
		// Just build monad set, and return.
		m_set->monadsBuildSet(monads);
		return;
	}

	//
	// Then deal with ourselves
	//

	// Get our own monads
	SetOfMonads self_monads;
	m_set->monadsBuildSet(self_monads);
  
	// Apply operation
	switch (m_operator) {
	case kSOUnion:
		monads.unionWith(self_monads);
		break;
	case kSOIntersect:
		monads = SetOfMonads::intersect(monads, self_monads);
		break;
	case kSODifference:
		monads.difference(self_monads);
		break;
	case kSONone:
		ASSERT_THROW(false,
			     "kSONone found");
		break;
	default:
		ASSERT_THROW(false,
			     "Unknown operator");
		break;
	}
	return;
}




//////////////////////////////////////////////////////////////
//
// FeatureAssignment 
//
//////////////////////////////////////////////////////////////

FeatureAssignment::FeatureAssignment(std::string* feature_name,
				     Expression* expression,
				     FeatureAssignment* next)
{
	m_feature_index = -1;
	m_pFeature_name = feature_name;
	str_tolower(*m_pFeature_name);
	m_expression = expression;
	m_next = next;
}

void FeatureAssignment::assignFeatureIndex(const FeatureEnv& feature_env)
{
	// Iterate over list
	/*
	FeatureAssignment *pFA = this;
	if (pFA != 0) {
		pFA->m_feature_index = feature_env.findFeatureIndex(*(pFA->m_pFeature_name));
		pFA = pFA->m_next;
	}
	*/
	if (m_next != 0) {
		m_next->assignFeatureIndex(feature_env);
 	}
 	
	m_feature_index = feature_env.findFeatureIndex(*m_pFeature_name);
}

FeatureAssignment::~FeatureAssignment()
{
	delete m_next;
	delete m_expression;
	delete m_pFeature_name;
}

//
// - Check that a computed feature is not assigned to
//
// - Weed feature_assignment
//
void FeatureAssignment::weed(MQLExecEnv *pEE, bool& bResult)
{
	// First deal with next
	if (m_next != 0) {
		m_next->weed(pEE, bResult);
		if (!bResult)
			return;
	}

	// Then deal with ourselves
	weedComputedFeatureNotAssigned(pEE, bResult);
	if (!bResult) {
		return;
	}

	m_expression->weed(pEE, bResult);
	if (!bResult) {
		return;
	}
}

//
// - Check that a computed feature is not assigned to
//
void FeatureAssignment::weedComputedFeatureNotAssigned(MQLExecEnv *pEE, bool& bResult)
{
	FeatureInfo dummy_feature_info(getFeatureName(),
				       "",
				       FEATURE_TYPE_INTEGER,
				       "0");
	bool bIsComputed = dummy_feature_info.getIsComputed();
	if (bIsComputed) {
		bResult = false;
	} else {
		bResult = true;
	}
	if (!bResult) {
		pEE->pError->appendError("Cannot assign to feature '" + getFeatureName() + "', since this feature is read-only.\n");
	}
}

//
// Returns true if a member of the list has a feature of name
// feature_name.  The comparison is done caseinsensitively.
// 
bool FeatureAssignment::listHasFeature(const std::string& feature_name)
{
	// First deal with ourselves...
	// It is OK to do it with ==, not strcmp_nocase, since
	// m_feature_name has been lower-cased in the constructor,
	// and feature_name has been lower-cased before we call here.
	if (*m_pFeature_name == feature_name) {
		return true;
	}

	// Then deal with next
	if (m_next != 0) {
		return m_next->listHasFeature(feature_name);
	}

	// If we got here, then we don't have it.
	return false;
}

bool FeatureAssignment::symbolNoFeatureMayBeComputed(MQLExecEnv *pEE, bool& bResult)
{
	// Deal with next first
	if (m_next != 0) {
		if (!m_next->symbolNoFeatureMayBeComputed(pEE, bResult))
			return false;
		if (!bResult)
			return true;
	}

	// Then deal with ourselves
	bResult = !m_is_computed;
	if (!bResult) {
		pEE->pError->appendError("Feature " + *m_pFeature_name + " is a computed feature.  Cannot assign to a computed feature.\n");
	} 
	return true;
}

//
// Make a std::list<FeatureInfo> from the list
//
bool FeatureAssignment::execMakeFeatureList(MQLExecEnv *pEE,
					    const FeatureEnv& feature_env,
					    std::list<FeatureInfo>& features)
{
	// First deal with next
	if (m_next != 0) {
		m_next->execMakeFeatureList(pEE, feature_env, features);
	}

	// Then deal with ourselves
	MQLType *pMQLType = feature_env.findMQLType(m_feature_index);

	// Then deal with ourselves
	std::string feature_value;
	if (!m_expression->getAsString(pEE, feature_value, true))
		return false;
	FeatureInfo f_info(*m_pFeature_name,               // feature name
			   "",                             // parameter1
			   pMQLType->getTypeId(),          // feature type
			   feature_value);                 // feature value
	features.push_back(f_info);
	return true;
}

// object_type_features map must map lowercase feature names
// to indexes into feature_value_arr.
void FeatureAssignment::execMakeFeatureArray(MQLExecEnv *pEE, 
					     const FeatureEnv& feature_env,
					     EMdFValue *feature_value_arr)
{
	// First deal with next
	if (m_next != 0) {
		m_next->execMakeFeatureArray(pEE, feature_env, feature_value_arr);
	}

	// Then deal with ourselves

	// Find index into feature_value_arr from m_feature_id
	int index = m_feature_index;
	MQLType *pMQLType = feature_env.findMQLType(index);
  
	// Create EMdFValue
	EMdFValue *pValue = &(feature_value_arr[index]);
	switch (pMQLType->getType()) {
	case kInteger:
		pValue = new(pValue) EMdFValue(kEVInt, m_expression->getInteger());
		break;
	case kString:
		pValue = new(pValue) EMdFValue(m_expression->getString());
		break;
	case kASCII:
		pValue = new(pValue) EMdFValue(m_expression->getString());
		break;
	case kID_D:
		pValue = new(pValue) EMdFValue(kEVID_D, m_expression->getInteger());
		break;
	case kEnumeration:
		pValue = new(pValue) EMdFValue(kEVEnum, m_expression->getEnumValue());
		break;
	case kListOfInteger:
		pValue = new(pValue) EMdFValue(kEVListOfInteger, m_expression->getCopyOfIntegerList());
		break;
	case kListOfID_D:
		pValue = new(pValue) EMdFValue(kEVListOfID_D, m_expression->getCopyOfIntegerList());
		break;
	case kListOfEnumeration:
		// This assumes that m_expression->typeTypeCompatibility() has been run,
		// which converts the list of identifiers into a list of enum-values.
		pValue = new(pValue) EMdFValue(kEVListOfInteger, m_expression->getCopyOfIntegerList());
		break;
	case kSetOfMonads:
		pValue = new(pValue) EMdFValue(m_expression->getSOM());
		break;
	default:
		ASSERT_THROW(false,
			     "Unknown type kind");
		break;
	}

	/* The following has been obsoleted by the use of the
	   placement new operator above. */
	/*
	// Copy to array
	feature_value_arr[index] = *pValue;

	// Clean up after ourselves
	delete pValue;
	*/
}


std::list<FeatureInfo>::const_iterator LocalListHasFeature(const std::list<FeatureInfo>& l, const std::string& feature_name)
{
	std::list<FeatureInfo>::const_iterator ci(l.begin());
	std::list<FeatureInfo>::const_iterator cend(l.end());
	while (ci != cend) {
		if (strcmp_nocase(ci->getHumanReadableFeatureName(), feature_name) == 0)
			return ci;
		++ci;
	}
	// If we got this far, the feature was not in the list
	return cend;
}


//
// - Check for type compatibility between feature and expression
//
bool FeatureAssignment::typeTypeCompatibility(MQLExecEnv *pEE, 
					      const FeatureEnv& feature_env,
					      bool& bResult)
{
	// First deal with next
	if (m_next != 0) {
		if (!m_next->typeTypeCompatibility(pEE, feature_env, bResult))
			return false;
		if (!bResult)
			return true;
	}

	// Then deal with ourselves
	MQLType *pMQLType = feature_env.findMQLType(m_feature_index);
	if (!m_expression->typeTypeCompatibility(pEE, pMQLType, bResult))
		return false;
	if (!bResult) {
		std::string expression;
		m_expression->getAsString(pEE, expression, false);
		pEE->pError->appendError("There was type incompatibility between the feature " + *m_pFeature_name + " and its assignment '" + expression + "'\n");
		return true;
	}

	// If we got this far, there were no DB errors and no compiler errors
	bResult = true;
	return true;
}

//////////////////////////////////////////////////////////////
//
// ObjectSpec
//
//////////////////////////////////////////////////////////////

void disposeOfObjectSpecNoOT(ObjectSpecNoOT **ppHead)
{
	ObjectSpecNoOT *pToDelete = *ppHead;
	ObjectSpecNoOT *pOS = pToDelete, *pNext;
	while (pOS != 0) {
		pNext = pOS->getNext();
		delete pOS;
		pOS = pNext;
	}

	// Finally, set the original pointer to 0.
	*ppHead = 0;
}



ObjectSpec::ObjectSpec(std::string* object_type_name,
		       FeatureAssignment* feature_assignment_list)
	: ObjectSpecBase(feature_assignment_list)
{
	m_object_type_name = object_type_name;
}

ObjectSpec::~ObjectSpec()
{
	delete m_object_type_name;
}

void ObjectSpec::weedObjectTypeNameNotSpecial(MQLExecEnv *pEE, bool& bResult)
{
	// Call base class to do the dirty job
	ObjectSpecBase::weedObjectTypeNameNotSpecial(pEE, *m_object_type_name, bResult);
}


bool ObjectSpec::symbolObjectTypeExists(MQLExecEnv *pEE, bool& bTypeExists)
{
	// Call base class to do the dirty job
	return ObjectSpecBase::symbolObjectTypeExists(pEE, *m_object_type_name, m_object_type_id, bTypeExists, m_objectRangeType, m_monadUniquenessType);
}


bool ObjectSpec::symbolAllFeaturesMustBeAssigned(MQLExecEnv *pEE, 
						 const FeatureEnv& feature_env)

{
	// Call base class to do the dirty job
	return ObjectSpecBase::symbolAllFeaturesMustBeAssigned(pEE, 
							       m_object_type_id,
							       feature_env);
}


bool ObjectSpec::symbolAllFeaturesMustExist(MQLExecEnv *pEE, 

					    const FeatureEnv& feature_env,
					    
					    bool& bResult)
{
	// Call base class to do the dirty job
	return ObjectSpecBase::symbolAllFeaturesMustExist(pEE, 
							  *m_object_type_name, 
							  m_object_type_id, 
							  feature_env,
							  bResult);
}







//////////////////////////////////////////////////////////////
//
// ObjectSpecBase 
//
//////////////////////////////////////////////////////////////

ObjectSpecBase::ObjectSpecBase(FeatureAssignment* feature_assignment_list)
{
	m_feature_assignments = feature_assignment_list;
}

ObjectSpecBase::~ObjectSpecBase()
{
	delete m_feature_assignments;
}

//
// - Check that a computed feature is not assigned to
//
void ObjectSpecBase::weed(MQLExecEnv *pEE, bool& bResult)
{
	if (m_feature_assignments != 0) {
		m_feature_assignments->weed(pEE, bResult);
	} else {
		bResult = true;
	}
	return;
}


// Local helper function
void LocalWeedObjectTypeNameNotSpecial(MQLExecEnv *pEE, std::string special_type, const std::string& object_type_name, bool& bResult)
{
	bResult = strcmp_nocase(special_type, object_type_name) != 0;
	if (!bResult) {
		pEE->pError->appendError("Cannot create object of type " + special_type + ", since this object type is a special object type, implicitly defined.\n");
	}
}

//
// - Check that "object_type_name" is not all_m, pow_m, or any_m
// 
void ObjectSpecBase::weedObjectTypeNameNotSpecial(MQLExecEnv *pEE, const std::string& object_type_name, bool& bResult)
{
	LocalWeedObjectTypeNameNotSpecial(pEE, "all_m", object_type_name, bResult);
	if (!bResult) {
		return;
	}

	LocalWeedObjectTypeNameNotSpecial(pEE, "pow_m", object_type_name, bResult);
	if (!bResult) {
		return;
	}

	LocalWeedObjectTypeNameNotSpecial(pEE, "any_m", object_type_name, bResult);
	if (!bResult) {
		return;
	}
}

void ObjectSpecBase::assignFeatureIndexes(const FeatureEnv& feature_env)
{
	if (m_feature_assignments != 0) {
		m_feature_assignments->assignFeatureIndex(feature_env);
	}
}



//
// - Check that the object type exists.  In doing so, store its
//   type_id in the AST.
// 
bool ObjectSpecBase::symbolObjectTypeExists(MQLExecEnv *pEE, 
					    const std::string& object_type_name, 
					    id_d_t& /* out */ object_type_id, 
					    bool& bTypeExists,
					    eObjectRangeType& /* out */ objectRangeType,
					    eMonadUniquenessType& /* out */ monadUniquenessType)
{
	if (!pEE->pDB->objectTypeExists(object_type_name, 
					bTypeExists, object_type_id,
					objectRangeType,
					monadUniquenessType))
		return false;
	return true;
}

//
// - Make sure that all features are given a value.  If a feature 
//   is not given an explicit value, then use the default value.
//   If a feature exists for the object type, but is not in the list
//   of feature assignments, then add it to the list of feature 
//   assignments.
//
bool ObjectSpecBase::symbolAllFeaturesMustBeAssigned(MQLExecEnv *pEE, 
						     id_d_t object_type_id,
						     const FeatureEnv& feature_env)
{
	UNUSED(pEE);
	UNUSED(object_type_id);
	
	// Iterate through list of object type features
	int length = feature_env.size();
	int index = 0;
	while (index < length) {
		bool bMakeFeatureAssignment;
		const FeatureInfo &fi = feature_env.getFeatureInfoConstRef(index);
		if (m_feature_assignments == 0 && !fi.getIsComputed()) {
			bMakeFeatureAssignment = true;
		} else {
			// listHasFeature relies on the fact that fi's
			// feature-name has been lower-cased.
			bMakeFeatureAssignment = 
				!fi.getIsComputed() 
				&& !m_feature_assignments->listHasFeature(fi.getHumanReadableFeatureName());
		}

		if (bMakeFeatureAssignment) {
			//
			// Make feature name
			//
			std::string* pFeature_name;
			pFeature_name = new std::string(fi.getHumanReadableFeatureName());

			//
			// Make expression
			//
			Expression* pExpr;
			std::string* pString;
			SetOfMonads som;

			// Decide type
			switch (fi.getOutputType() & FEATURE_TYPE_TYPE_MASK) {
			case FEATURE_TYPE_INTEGER:
				pExpr = new Expression(string2long(fi.getDefaultValue()));
				break;
			case FEATURE_TYPE_ID_D:
				pExpr = new Expression(string2long(fi.getDefaultValue()));
				break;
			case FEATURE_TYPE_STRING:
				pString = new std::string(fi.getDefaultValue());
				pExpr = new Expression(pString, kExprString);
				break;
			case FEATURE_TYPE_ASCII:
				pString = new std::string(fi.getDefaultValue());
				pExpr = new Expression(pString, kExprString);
				break;
			case FEATURE_TYPE_ENUM:
				pString = new std::string(fi.getDefaultValue());
				pExpr = new Expression(pString, kExprIdentifier);
				break;
			case FEATURE_TYPE_LIST_OF_INTEGER:
			case FEATURE_TYPE_LIST_OF_ID_D:
			case FEATURE_TYPE_LIST_OF_ENUM:
				pExpr = new Expression(); // kExprEmptyList
				break;
			case FEATURE_TYPE_SET_OF_MONADS:
				som.fromString(fi.getDefaultValue());
				pExpr = new Expression(som); // kExprSetOfMonads
				break;
			default:
				ASSERT_THROW(false, "Unknown FEATURE_TYPE.");
				break;
			}

			//
			// Make feature assignment
			//
			FeatureAssignment* pNewFA;
			pNewFA = new FeatureAssignment(pFeature_name, pExpr, m_feature_assignments);

			// Assign its feature index
			pNewFA->assignFeatureIndex(feature_env);

			// Set FA's is_computed flag from this index's
			// feature info
			pNewFA->setIsComputed(fi.getIsComputed());


			//
			// Insert it at the beginning of the list
			//
			m_feature_assignments = pNewFA;
		}
		++index;
	}

	// If we got this far, there were no errors
	return true;
}

//
// - All features must be exist.
//
bool ObjectSpecBase::symbolAllFeaturesMustExist(MQLExecEnv *pEE, 
						const std::string& object_type_name, 
						id_d_t object_type_id, 
						const FeatureEnv& feature_env,
						bool& bResult)
{
	UNUSED(object_type_id);
	
	// Assume they all exist until otherwise is proven
	bResult = true;

	// Iterate through list of feature assignments.
	// Find all the ones that aren't there before returning
	FeatureAssignment* pFA = m_feature_assignments;
	while (pFA != 0) {
		int index = pFA->getFeatureIndex();
		if (index < 0) {
			// We don't have the feature
			bResult = false;
			pEE->pError->appendError("Object type " + object_type_name + " does not have feature " + pFA->getFeatureName() + ".\n");
		} else {
			// We do have the feature.  Set info.
			const FeatureInfo &fi = feature_env.getFeatureInfoConstRef(index);
			pFA->setIsComputed(fi.getIsComputed());
		}
      
		// Get next
		pFA = pFA->getNext();
	}

	// If we got this far, there were no db errors.
	// bResult has already been set.
	return true;
}

bool ObjectSpecBase::symbolNoFeatureMayBeComputed(MQLExecEnv *pEE, bool& bResult)
{
	// Only perform check if there are feature assignments
	if (m_feature_assignments != 0) {
		if (!m_feature_assignments->symbolNoFeatureMayBeComputed(pEE, bResult))
			return false;
		if (!bResult)
			return true;
	} 

	// If we got this far, there were no compiler errors and no DB errors
	bResult = true;
	return true;
}

//
// - Check for type compatibility between feature and expression
//
bool ObjectSpecBase::typeTypeCompatibility(MQLExecEnv *pEE, 
					   const FeatureEnv& feature_env, 
					   bool& bResult)
{
	if (m_feature_assignments != 0)
		return m_feature_assignments->typeTypeCompatibility(pEE, feature_env, bResult);
	else /* The list of features specs may be empty, and the type may have no features. */
		return true; 
}


//
// Make a std::list<FeatureInfo> from the list
//
bool ObjectSpecBase::execMakeFeatureList(MQLExecEnv *pEE,
					 const FeatureEnv& feature_env,
					 std::list<FeatureInfo>& features)
{
	if (m_feature_assignments != 0)
		return m_feature_assignments->execMakeFeatureList(pEE, feature_env, features);
	else /* The list of features specs may be empty, and the type may have no features. */
		return true; 
}


//////////////////////////////////////////////////////////////
//
// ObjectSpecNoOT
//
//////////////////////////////////////////////////////////////

ObjectSpecNoOT::ObjectSpecNoOT(MQLMonadSetElement *pMonadSet,
			       id_d_t with_id_d,
			       FeatureAssignment* feature_assignment_list,
			       ObjectSpecNoOT *next) 
	: ObjectSpecBase(feature_assignment_list),
	  ByMonads(pMonadSet)
{
	m_with_id_d = with_id_d;
	m_next = next;
	m_prev = 0;
}


ObjectSpecNoOT::~ObjectSpecNoOT()
{
}


void ObjectSpecNoOT::weed(MQLExecEnv *pEE, bool& bResult)
{
	// Build prev-list
	ObjectSpecNoOT *pOS = this, *pPrev = 0;
	while (pOS != 0) {
		pOS->m_prev = pPrev;
		pPrev = pOS;
		pOS = pOS->m_next;
	}
  
	// Deal with objects in reverse order
	pOS = pPrev;
	while (pOS != 0) {
		pOS->DoWeed(pEE, bResult);
		if (!bResult)
			return;
		pOS = pOS->m_prev;
	}
}


void ObjectSpecNoOT::DoWeed(MQLExecEnv *pEE, bool& bResult)
{
	//
	// - Check that a computed feature is not assigned a value
	// - Check all feature assignments with weed()
	// 
	ObjectSpecBase::weed(pEE, bResult);
	if (!bResult)
		return;

	// Check monad set
	ByMonads::myWeed(pEE, bResult);
	if (!bResult)
		return;
  
	// If we got this far, it means there were no errors
	bResult = true;
	return;
}

void ObjectSpecNoOT::assignFeatureIndexes(const FeatureEnv& feature_env)
{
	ObjectSpecNoOT *pOS = this, *pPrev = 0;
	while (pOS != 0) {
		pPrev = pOS;
		pOS = pOS->m_next;
	}
  
	// Deal with objects in reverse order
	pOS = pPrev;
	while (pOS != 0) {
		if (pOS->m_feature_assignments != 0) {
			pOS->m_feature_assignments->assignFeatureIndex(feature_env);
		}
		pOS = pOS->m_prev;
	}
}


bool ObjectSpecNoOT::symbol(MQLExecEnv *pEE, 
			    const std::string& object_type_name, 
			    id_d_t object_type_id, 
			    const FeatureEnv& feature_env,
			    bool &bResult)
{
	// Go to end of prev-list
	ObjectSpecNoOT *pOS = this, *pPrev = 0;
	while (pOS != 0) {
		pPrev = pOS;
		pOS = pOS->m_next;
	}
  
	// Deal with objects in reverse order
	pOS = pPrev;
	while (pOS != 0) {
		if (!pOS->DoSymbol(pEE, object_type_name, object_type_id, feature_env, bResult))
			return false;
		if (!bResult)
			return true;
		pOS = pOS->m_prev;
	}
  
	// If we got this far, there were no compiler errors and not db errors
	bResult = true;
	return true;
}


bool ObjectSpecNoOT::DoSymbol(MQLExecEnv *pEE, 
			      const std::string& object_type_name, 
			      id_d_t object_type_id, 
			      const FeatureEnv& feature_env,
			      bool &bResult)
{
	//
	// - All features must be exist.
	//
	if (!ObjectSpecBase::symbolAllFeaturesMustExist(pEE, 
							object_type_name,
							object_type_id,
							feature_env,
							bResult))
		return false;
	if (!bResult)
		return true;

	//
	// - Make sure that all features are given a value.  If a feature 
	//   is not given an explicit value, then use the default value.
	//   If a feature exists for the object type, but is not in the list
	//   of feature assignments, then add it to the list of feature 
	//   assignments.
	//
	if (!ObjectSpecBase::symbolAllFeaturesMustBeAssigned(pEE, 
							     object_type_id,
							     feature_env))
		return false;
	if (!bResult)
		return true;

	// Check that no features are computed
	/* Don't check until we actually have something to check.
	   if (!ObjectSpecBase::symbolNoFeatureMayBeComputed(pEE, bResult))
	   return false;
	   if (!bResult)
	   return true;
	*/


	// If we got this far, there were no compiler errors and not db errors
	bResult = true;
	return true;
}



bool ObjectSpecNoOT::type(MQLExecEnv *pEE, id_d_t object_type_id, const std::list<FeatureInfo>& object_type_features, const FeatureEnv& feature_env, bool &bResult)
{
	// Go to end of prev-list
	ObjectSpecNoOT *pOS = this, *pPrev = 0;
	while (pOS != 0) {
		pPrev = pOS;
		pOS = pOS->m_next;
	}
  
	// Deal with objects in reverse order
	pOS = pPrev;
	while (pOS != 0) {
		if (!pOS->DoType(pEE,
				 object_type_id,
				 object_type_features,
				 feature_env,
				 bResult))
			return false;
		if (!bResult)
			return true;
		pOS = pOS->m_prev;
	}
  
	// If we got this far, there were no compiler errors and not db errors
	bResult = true;
	return true;
}

bool ObjectSpecNoOT::DoType(MQLExecEnv *pEE, id_d_t object_type_id, const std::list<FeatureInfo>& object_type_features, const FeatureEnv& feature_env, bool &bResult)
{
	UNUSED(object_type_id);
	UNUSED(object_type_features);
	
	//
	// - Check for type compatibility between feature and expression
	//
	if (!ObjectSpecBase::typeTypeCompatibility(pEE, feature_env, bResult))
		return false;
	if (!bResult)
		return true;
	
	// If we got this far, there were no DB errors and no compiler errors
	bResult = true;
	return true;
}


bool ObjectSpecNoOT::monads(MQLExecEnv *pEE, 
			    eObjectRangeType objectRangeType,
			    bool& bResult)
{
	// Go to end of prev-list
	ObjectSpecNoOT *pOS = this, *pPrev = 0;
	while (pOS != 0) {
		pPrev = pOS;
		pOS = pOS->m_next;
	}
  
	// Deal with objects in reverse order
	pOS = pPrev;
	while (pOS != 0) {
		if (!pOS->DoMonads(pEE, objectRangeType, bResult))
			return false;
		if (!bResult)
			return true;
		pOS = pOS->m_prev;
	}
  
	// If we got this far, there were no compiler errors and not db errors
	bResult = true;
	return true;
}

bool ObjectSpecNoOT::DoMonads(MQLExecEnv *pEE, 
			      eObjectRangeType objectRangeType,
			      bool& bResult)
{
	if (!ByMonads::myMonads(pEE, bResult))
		return false;
	if (!bResult) {
		return true;
	}

	// Is this a WITH SINGLE RANGE OBJECTS object type?
	if (objectRangeType == kORTSingleRange) {
		// Check that there is exactly one MSE
		if (!m_monads.hasOnlyOneMSE()) {
			pEE->pError->appendError("Object type has been declared WITH SINGLE RANGE OBJECTS, yet the monad set\n" + m_monads.toString() + "\ndoes not have exactly one stretch of monads.\n");
			bResult = false;
			return true;
		}
	} else if (objectRangeType == kORTSingleMonad) {
		// If it is SINGLE MONAD, then we need to check that first == last
		if (m_monads.first() != m_monads.last()) {
			pEE->pError->appendError("Object type has been declared WITH SINGLE MONAD OBJECTS, yet the monad set\n" + m_monads.toString() + "\ndoes not have exactly one monad.\n");
			bResult = false;
			return true;
		}
	}

	// If we came this far, there were no DB errors
	return true;
}

// The object_type_features map must contain all of the features of
// the object type minus computed features.  The string must be the
// feature name, and the int must be the index into feature_value_arr.
bool ObjectSpecNoOT::exec(MQLExecEnv *pEE, 
			  std::list<InstObject*>& output_list, 
			  const FeatureEnv& feature_env)
{
	// Go to end of prev-list
	ObjectSpecNoOT *pOS = this, *pPrev = 0;
	while (pOS != 0) {
		pPrev = pOS;
		pOS = pOS->m_next;
	}
  
	// Deal with objects in reverse order
	pOS = pPrev;
	while (pOS != 0) {
		if (!pOS->DoExec(pEE, output_list, feature_env)) 
			return false;
		pOS = pOS->m_prev;
	}
  
	// If we got this far, there were no db errors
	return true;
}

bool ObjectSpecNoOT::DoExec(MQLExecEnv *pEE, 
			    std::list<InstObject*>& output_list, 
			    const FeatureEnv& feature_env)
{
	// If the user did not specify an id_d, autogenerate one
	if (m_with_id_d == NIL) {
		if (!pEE->pDB->getNextObjectID_D(m_with_id_d)) {
			return false;
		}
	}

	// Make array of feature-values.
	EMdFValue *feature_value_arr;
	int feature_value_arr_size = feature_env.size();
	if (feature_value_arr_size == 0) {
		// Make it just a nil pointer if there are no features
		// to create (the object type could have no features
		// besides computed features.)
		feature_value_arr = 0;
	} else {
		feature_value_arr = (EMdFValue*) pEE->m_arena.allocate(sizeof(EMdFValue)*feature_value_arr_size);
	}

	// Fill the array
	execMakeFeatureArray(pEE, feature_env, feature_value_arr);

	// Create inst object
	InstObject *pObject;
	SetOfMonads *pSOM = new SetOfMonads(m_monads);
	pObject = (InstObject*) pEE->m_arena.allocate(sizeof(InstObject));

	// Use placement new operator on the new InstObject
	pObject = new (pObject) InstObject(m_with_id_d, 
					   pSOM, 
					   feature_value_arr, 
					   feature_value_arr_size, 
					   false); // Do not delete feature_value_arr

	// Place object at the end of the list
	output_list.push_back(pObject);

	// If we got this far, there were no errors in this object 
	// creation specification.
	return true;
}

// object_type_features map must map lowercase feature names
// to indexes into feature_value_arr.
void ObjectSpecNoOT::execMakeFeatureArray(MQLExecEnv *pEE, 
					  const FeatureEnv& feature_env, 
					  EMdFValue *feature_value_arr)
{
	if (m_feature_assignments != 0)
		m_feature_assignments->execMakeFeatureArray(pEE, feature_env, feature_value_arr);
}


//////////////////////////////////////////////////////////////
//
// ID_D 
//
//////////////////////////////////////////////////////////////

ID_D::ID_D(id_d_t id_d, ID_D* next)
{
	m_id_d = id_d;
	m_next = next;
}

ID_D::~ID_D()
{
	delete m_next;
}

//
// - Check that no id_d in the list is NIL
// 
void ID_D::weedNoneIsNIL(MQLExecEnv *pEE, bool& bResult)
{
	// First check next
	if (m_next != 0) {
		m_next->weedNoneIsNIL(pEE, bResult);
		if (!bResult)
			return;
	}

	// Then deal with ourselves
	bResult = m_id_d != NIL;
	if (!bResult) {
		pEE->pError->appendError("Cannot have NIL in the list of id_ds.\n");
	}
}

//
// - Check that all the objects exist in the given type.
//   The parameter object_type_id must be the object type id 
//   of object_type_name.
//
bool ID_D::symbolObjectsExistInType(MQLExecEnv *pEE, const std::string& object_type_name, id_d_t object_type_id, bool& bResult)
{
	// First deal with next
	if (m_next != 0) {
		if (!m_next->symbolObjectsExistInType(pEE, object_type_name, object_type_id, bResult))
			return false;
		if (!bResult)
			return true;
	}

	// Then deal with ourselves
	if (!pEE->pDB->objectID_DExistsInType(m_id_d, object_type_name, bResult))
		return false;
	if (!bResult) {
		pEE->pError->appendError("Object with id_d " + id_d2string(m_id_d) + " does not exist in object type " + object_type_name + ".\n");
		return true;
	} else {
		// Set m_object_type_id
		m_object_type_id = object_type_id;
	}

	// If we got this far, there were no DB errors and no compiler-errors
	bResult = true;
	return true;
}

//
// - Check that all the objects exist
//
bool ID_D::symbolObjectsExist(MQLExecEnv *pEE, bool& bResult)
{
	// First deal with next
	if (m_next != 0) {
		if (!m_next->symbolObjectsExist(pEE, bResult))
			return false;
		if (!bResult)
			return true;
	}

	// Then deal with ourselves
	if (!pEE->pDB->objectID_DExists(m_id_d, bResult, m_object_type_id))
		return false;
	if (!bResult) {
		pEE->pError->appendError("Object with id_d " + id_d2string(m_id_d) + " does not exist.\n");
		return true;
	}

	// If we got this far, there were no DB errors and no compiler-errors
	bResult = true;
	return true;
}

bool ID_D::symbolAllAreOfSameType(MQLExecEnv *pEE, id_d_t object_type_id, bool& bResult)
{
	// First deal with next
	if (m_next != 0) {
		if (!m_next->symbolAllAreOfSameType(pEE, object_type_id, bResult))
			return false;
		if (!bResult)
			return true;
	}

	// Then deal with ourselves
	bResult = m_object_type_id == object_type_id;
	if (!bResult) {
		pEE->pError->appendError("Objects in id_d list are not all of the same type.\n");
		return true;
	}
  
	// If we got this far, there were no DB errors and no compiler-errors
	bResult = true;
	return true;
}

void ID_D::execMakeSOM(SetOfMonads& som)
{
	// First deal with ourselves
	som.add(m_id_d);

	// Then deal with next
	// This is because if this list is sorted in ascending order, 
	// Then this order will be faster, since the last will be added first,
	// And it is quicker to add to the front of the SOM than to the back.
	if (m_next != 0) {
		m_next->execMakeSOM(som);
	}
}



void ID_D::execMakeList(std::list<id_d_t>& result)
{
	// First deal with next
	if (m_next != 0) {
		m_next->execMakeList(result);
	}

	// Then deal with ourselves
	result.push_back(m_id_d);
}

//////////////////////////////////////////////////////////////
//
// Feature 
//
//////////////////////////////////////////////////////////////



Feature::Feature(std::string* feature, std::string *parameter1, Feature* next)
	: m_feature_info()
{
	m_feature = feature;
	m_parameter1 = (parameter1 == 0) ? new std::string("") : parameter1;
	m_next = next;
	m_length = -1;
	m_feature_index_inst = -1;
	m_list_index = -1;
}


// Turns GrammarFeature around
Feature *Feature::FromGrammarFeature(GrammarFeature *pGrammarFeature)
{
	Feature *pCurF = 0;

	GrammarFeature *pCurGF = pGrammarFeature;
  
	std::list<GrammarFeature*> mylist;
	do {
		mylist.push_back(pCurGF);
		pCurGF = pCurGF->getNext();
	} while (pCurGF != 0);

	std::list<GrammarFeature*>::iterator it = mylist.begin();
	while (it != mylist.end()) {
		pCurGF = *it;
		pCurF = new Feature(new std::string (pCurGF->getFeature()), new std::string(pCurGF->getParameter1()), pCurF);
		++it;
	}

	// I know, it can't be 0 because we did a do...while above,
	// however, this is called defensive programming.
	if (pCurF != 0) 
		pCurF->assignListIndex(0);

	// Return the head
	return pCurF;
}

Feature::Feature(const Feature& other)
{
	m_feature = new std::string(*other.m_feature);
	m_parameter1 = new std::string(*other.m_parameter1);
	m_feature_info = other.m_feature_info;
	m_retrieved_feature_type_id = other.m_retrieved_feature_type_id;
	m_output_feature_type_id = other.m_output_feature_type_id;
	m_is_computed = other.m_is_computed;
	m_tc_type = other.m_tc_type;
	m_enum_name = other.m_enum_name;
	m_list_index = other.m_list_index;
	m_feature_index_inst = other.m_feature_index_inst;
	if (other.m_enum_const_cache.size() > 0) {
		std::map<long, std::string>::const_iterator 
			ci = other.m_enum_const_cache.begin();
		while (ci != other.m_enum_const_cache.end()) {
			m_enum_const_cache[ci->first] = ci->second;
			++ci;
		}
	}
	if (other.m_next != 0) {
		m_next = new Feature(*other.m_next);
	} else {
		m_next = 0;
	}
}


Feature::~Feature()
{
	delete m_next;
	delete m_feature;
	delete m_parameter1;
}

// Returns the head, and turns the list around.
// Must be called with pPrev = 0.
/*
  Feature *Feature::turnAround(Feature *pPrev)
  {
  Feature *pHead;

  // Deal with next first
  if (m_next != 0) {
  pHead = m_next->turnAround(this);
  } else {
  pHead = this;
  }

  // Then deal with ourselves  
  m_prev = pPrev;

  // If this is the tail, then go to the head and
  // assign list index
  if (pPrev == 0) {
  pHead->assignListIndex(0);
  }

  // Return the head
  return pHead;
  }
*/

void Feature::weed(MQLExecEnv *pEE, bool& bResult)
{
	// Then with prev
	if (m_next != 0) {
		m_next->weed(pEE, bResult);
		if (!bResult) {
			return;
		}
	}
}

void Feature::symbolAddFeaturesToSet(std::set<std::string>& myset) const
{
	std::string lowercase_feature_name;
	str_tolower(*m_feature, lowercase_feature_name);
	myset.insert(lowercase_feature_name);
}

bool Feature::symbolFeaturesExist(MQLExecEnv *pEE, id_d_t object_type_id, bool& bResult)
{
	FeatureInfo dummy_feature_info = FeatureInfo(*m_feature,
						     *m_parameter1,
						     FEATURE_TYPE_INTEGER,
						     "0"); // Default value
	
	// Deal with ourselves
	std::string default_value;
	bool bDummyIsComputed = false;
	if (!pEE->pDB->featureExists(dummy_feature_info.getRetrievedFeatureName(),
				     object_type_id, 
				     bResult,
				     m_retrieved_feature_type_id,
				     default_value,
				     bDummyIsComputed)) {
		pEE->pError->appendError(std::string("Feature '") + *m_feature + "' caused\na DB error during symbol checking.\n");
		return false;
	}
	if (!bResult) {
		pEE->pError->appendError(std::string("Feature '") + *m_feature + "' does not exist for the given object type.\n");
		return true;
	}

	m_feature_info = FeatureInfo(*m_feature,
				     *m_parameter1,
				     m_retrieved_feature_type_id,
				     default_value);

	m_output_feature_type_id = m_feature_info.getOutputType();
	m_is_computed = m_feature_info.getIsComputed();
	


	// Get enum consts if necessary
	if (featureTypeIdIsENUM(m_retrieved_feature_type_id)
	    || featureTypeIdIsListOfENUM(m_retrieved_feature_type_id)) {
		std::list<EnumConstInfo> enumConsts;
		if (!pEE->pDB->getEnumConstants(m_retrieved_feature_type_id, enumConsts)) {
			pEE->pError->appendError("Could not get enumeration constants from feature.\n");
			return false;
		}

		// Traverse list; add to enum const cache
		std::list<EnumConstInfo>::const_iterator ci = enumConsts.begin();
		std::list<EnumConstInfo>::const_iterator cend = enumConsts.end();
		while (ci != cend) {
			// m_enum_const_cache.insert(std::pair<long, std::string>(ci->getValue(), std::string(ci->getHumanReadableFeatureName())));
			m_enum_const_cache[ci->getValue()] = std::string(ci->getName());
			++ci;
		}
	}

	// Then deal with next
	if (m_next != 0) {
		if (!m_next->symbolFeaturesExist(pEE, object_type_id, bResult))
			return false;
		if (!bResult)
			return true;
	}

	// If we got this far, there were no DB errors and no compiler errors
	bResult = true;
	return true;
}



void Feature::symbolAddToObject(MQLExecEnv *pEE, MQLObject *pObj)
{
	// Deal with ourselves first

	// Create FeatureInfo
	FeatureInfo info(*m_feature, *m_parameter1, m_retrieved_feature_type_id, "");
	
	// Add to pObj
	pObj->addFeature(info);
	
	// Get indexes
	m_feature_index_inst = pObj->getFeatureIndex(info.getHumanReadableFeatureName());

	// Then with prev
	if (m_next != 0) {
		m_next->symbolAddToObject(pEE, pObj);
	}
}


bool Feature::typeFeatureName(MQLExecEnv *pEE, bool& bResult)
{
	// Deal with ourselves first
	if (!pEE->pDB->typeIdToTableColumnType(m_output_feature_type_id,
					       m_tc_type,
					       m_enum_name))
		return false;
	if (!bResult) {
		pEE->pError->appendError("Enumeration does not exist for the given object type.\n");
		return true;
	}


	// Then deal with next
	if (m_next != 0) {
		if (!m_next->typeFeatureName(pEE, bResult))
			return false;
		if (!bResult)
			return true;
	}

	// If we got this far, there were no DB errors and no compiler errors
	bResult = true;
	return true;
}

void Feature::execMakeFeatureList(std::list<FeatureInfo>& FeatureInfos)
{
	// Deal with ourselves first
	FeatureInfos.push_back(m_feature_info);

	// Then deal with next
	if (m_next != 0) {
		m_next->execMakeFeatureList(FeatureInfos);
	}
}

void Feature::execMakeFeatureNameVector(std::vector<std::string>& FeatureNames)
{
	// Deal with ourselves first
	FeatureNames.push_back(m_feature_info.getHumanReadableFeatureName());

	// Then deal with next
	if (m_next != 0) {
		m_next->execMakeFeatureNameVector(FeatureNames);
	}
}


void Feature::execMakeNameList(MQLResult *pResult)
{
	//
	// Deal with ourselves first
	//
	pResult->appendHeader(*m_feature, m_tc_type, m_enum_name);

	// Then deal with next
	if (m_next != 0) {
		m_next->execMakeNameList(pResult);
	}
}

void Feature::addEMdFValue(MatchedObject *pMO, MQLObject *pObj, bool bIterate)
{

	//
	// Deal with ourselves first
	//

	// Get value from object
	const EMdFValue *pOtherValue = pObj->getFeatureValue(m_feature_index_inst);
	// It must not be 0.
	ASSERT_THROW(pOtherValue != 0,
		     "pOtherValue is 0");

	// Create new value with copy-constructor
	EMdFValue *pValue = new EMdFValue(*pOtherValue);

	// Add to MO
	pMO->addEMdFValue(m_list_index, pValue);

	// Deal with next first
	if (bIterate 
	    && m_next != 0) {
		m_next->addEMdFValue(pMO, pObj, bIterate);
	}
}

std::string Feature::getHumanReadableFeatureName()
{
	return m_feature_info.getHumanReadableFeatureName();
}


int Feature::getFeatureInstListIndex(const std::string& feature_name) const
{
	// Deal with ourselves first
	if (strcmp_nocase(feature_name, *m_feature) == 0) {
		return m_list_index;
	}
  
	// Then deal with next
	if (m_next != 0) { 
		return m_next->getFeatureInstListIndex(feature_name);
	}

	// Return this as a fallback, if not found
	return -1;
}

void Feature::assignListIndex(int current_index)
{
	// Deal with ourselves first
	m_list_index = current_index; 
  
	// Then deal with next
	if (m_next != 0) { 
		m_next->assignListIndex(current_index+1); 
	}
}


// Make a vector of this list
void Feature::makeVectorOfSelves(std::vector<Feature*> **ppVec)
{
	// Allocate a vector with the right length,
	// If this is the Head
	if (m_list_index == 0) {
		// Get length.
		int length = getLength();

		*ppVec = new std::vector<Feature*>(length); 
	}

	// Deal with ourselves first
	(*(*ppVec))[m_list_index] = new Feature(*this);

	// Then deal with next
	if (m_next != 0) {
		m_next->makeVectorOfSelves(ppVec);
	} 
}


//////////////////////////////////////////////////////////////
//
// ByMonads
//
//////////////////////////////////////////////////////////////

ByMonads::ByMonads(MQLMonadSetElement* monad_set) : m_monad_set(monad_set)
{
}

ByMonads::~ByMonads()
{
	delete m_monad_set;
}

// 
// - Check that all the ranges of monads are positive and monotonic
//
void ByMonads::myWeed(MQLExecEnv *pEE, bool& bResult)
{
	// Monad-ranges should be monotonic and positive
	m_monad_set->weed(pEE, true, bResult);
	if (!bResult)
		return;

	// If we got this far, there were no DB errors and no compiler errors
	bResult = true;
	return;
}

// 
// - Check that all the ranges of monads are monotonic
//
void ByMonads::myWeedStar(MQLExecEnv *pEE, bool& bResult)
{
	// Monad-ranges should be monotonic (but need not be positive;
	// that is what the 'false' tells the set's weed method.)
	m_monad_set->weed(pEE, false, bResult);
	if (!bResult)
		return;

	// We really need it to be non-empty, but the grammar takes
	// care of that.

	// If we got this far, there were no compiler errors
	bResult = true;
	return;
}

//
// - Build the set of monads
//
bool ByMonads::myMonads(MQLExecEnv *pEE, bool& bResult)
{
	UNUSED(pEE);
	
	//
	// - Build the set of monads
	//
	m_monad_set->monadsBuildSet(m_monads);
  
	// There were no DB errors and no compiler errors
	bResult = true;
	return true;
} 

bool ByMonads::getObjectsByMonads(MQLExecEnv *pEE, 
				  const std::string& object_type_name, 
				  eObjectRangeType objectRangeType, 
				  std::list<id_d_t>& objects)
{
	// Get objects
	Table intermediate_result;
	if (!pEE->pDB->getObjectsHavingMonadsInMonadSet(object_type_name,
							m_monads,
							objectRangeType,
							intermediate_result))
		return false;

	TableIterator i = intermediate_result.iterator();
	while (i.hasNext()) {
		// Get id_d in string form
		std::string str_id_d = intermediate_result.getColumn(i, 3);

		// Put at back
		objects.push_back(string2id_d(str_id_d));

		// Advance inst iterator
		i.next();
	}

	// If we got this far, there were no DB errors
	return true;
}




//////////////////////////////////////////////////////////////
//
// AggregateFeature
//
//////////////////////////////////////////////////////////////
AggregateFeature::AggregateFeature(eAggregateFunction func)
	: m_function(func),
	  m_result(0),
	  m_feature(0),
	  m_feature_comparison(0),
	  m_object_type_name(""),
	  m_object_type_id(0),
	  m_next(0)
{
	ASSERT_THROW(func == kAFCOUNT_ALL,
		     "AggregateFeature(eAggregateFunction) called with illegal func = " + long2string((long) func));
}



AggregateFeature::AggregateFeature(eAggregateFunction func, std::string *feature_name)
	: m_function(func),
	  m_result(0),
	  m_feature(0),
	  m_feature_comparison(0),
	  m_object_type_name(""),
	  m_object_type_id(0),
	  m_next(0)
{
	ASSERT_THROW(feature_name != 0,
		     "AggregateFeature(eAggregateFunction, std::string *) called with illegal std::string * == 0");

	m_feature = new Feature(feature_name,
				new std::string(""), // parameter1
				0);
	
	if (func == kAFMIN) {
		m_result = LONG_MAX;
	} else if (func == kAFMAX) {
		m_result = LONG_MIN;
	} else if (func == kAFSUM) {
		m_result = 0;
	} else {
		ASSERT_THROW(false,
			     "AggregateFeature(eAggregateFunction, std::string *) called with illegal func = " + long2string((long) func));
	}

}



AggregateFeature::AggregateFeature(eAggregateFunction func, FeatureComparison *feature_comparison)
	: m_function(func),
	  m_result(0),
	  m_feature(0),
	  m_feature_comparison(feature_comparison),
	  m_object_type_name(""),
	  m_object_type_id(0),
	  m_next(0)
	
{
	ASSERT_THROW(func == kAFCOUNT_FEATURE_COMPARISON,
		     "AggregateFeature(eAggregateFunction, FeatureComparison *) called with illegal func = " + long2string((long) func));
	ASSERT_THROW(m_feature_comparison != 0,
		     "AggregateFeature(eAggregateFunction, FeatureComparison *) called with illegal FeatureComparison * == 0");
}



AggregateFeature::~AggregateFeature()
{
	delete m_feature;
	delete m_feature_comparison;
	delete m_next;
}

std::string AggregateFeature::getFeatureName() const
{
	if (m_feature != 0) {
		return m_feature->getFeature();
	} else if (m_feature_comparison != 0) {
		return m_feature_comparison->getFeatureName();
	} else {
		// This is not a logic error.
		// If we have a COUNT(*), then no feature name is
		// present.
		return "";
	}
}


void AggregateFeature::weed(MQLExecEnv *pEE, bool& bResult)
{
	bResult = true;
	if (m_feature_comparison != 0) {
		m_feature_comparison->weedFeatureConstraints(pEE, bResult, 0);
	}
	if (!bResult) {
		return;
	}

	// Then deal with next
	if (m_next != 0) {
		m_next->weed(pEE, bResult);

		if (!bResult) {
			return;
		}
	}
}



bool AggregateFeature::symbol(MQLExecEnv *pEE, const std::string& object_type_name, id_d_t object_type_id, bool& bResult)
{
	bool bDBResult = true;
	bResult = true;

	m_object_type_name = object_type_name;
	m_object_type_id = object_type_id;

	if (m_feature != 0) {
		bDBResult = m_feature->symbolFeaturesExist(pEE, object_type_id, bResult);
	}
	if (!bResult || !bDBResult) {
		return bDBResult;
	}

	if (m_feature_comparison != 0) {
		bDBResult = m_feature_comparison->symbol(pEE, object_type_name, object_type_id, bResult);
	}

	if (!bResult || !bDBResult) {
		return bDBResult;
	}

	// Then deal with next
	if (m_next != 0) {
		if (!m_next->symbol(pEE, object_type_name, object_type_id, bResult))
			return false;
		if (!bResult)
			return true;
	}

	return bDBResult;
}



bool AggregateFeature::type(MQLExecEnv *pEE, bool& bResult)
{
	bResult = true;
	bool bDBResult = true;

	if (m_feature != 0) {
		bDBResult = m_feature->typeFeatureName(pEE, bResult);
		if (!bResult || !bDBResult) {
			return bDBResult;
		}
	
		if (!featureTypeIdIsINTEGER(m_feature->getOutputFeatureTypeID())) {
			bResult = false;
			pEE->pError->appendError("Error in aggregate function: feature with name " + m_feature->getFeature() + "\ndoes not have feature type INTEGER.\nThis is required by the aggregate function used.");
		}
		if (!bResult || !bDBResult) {
			return bDBResult;
		}
	}		
	
	if (m_feature_comparison != 0) {
		bDBResult = m_feature_comparison->type(pEE, bResult);
	}

	if (!bResult || !bDBResult) {
		return bDBResult;
	}

	// Then deal with next
	if (m_next != 0) {
		if (!m_next->type(pEE, bResult))
			return false;
		if (!bResult)
			return true;
	}

	return bDBResult;
}



void AggregateFeature::exec(MQLExecEnv *pEE, const InstObject *pInstObj)
{
	long feature_value = 0;
	const EMdFValue *pEMdFValue = 0;
	switch (m_function) {
	case kAFMIN:
		pEMdFValue = pInstObj->getFeature(m_inst_obj_feature_index);
		
		feature_value = pEMdFValue->getInt();
		if (feature_value < m_result) {
			m_result = feature_value;
		}
		break;
	case kAFMAX:
		pEMdFValue = pInstObj->getFeature(m_inst_obj_feature_index);
		
		feature_value = pEMdFValue->getInt();
		if (feature_value > m_result) {
			m_result = feature_value;
		}
		break;
	case kAFSUM:
		pEMdFValue = pInstObj->getFeature(m_inst_obj_feature_index);
		
		feature_value = pEMdFValue->getInt();
		m_result += feature_value;
		break;
	case kAFCOUNT_ALL:
		++m_result;
		break;
	case kAFCOUNT_FEATURE_COMPARISON:
		pEMdFValue = pInstObj->getFeature(m_inst_obj_feature_index);
		
		if (m_feature_comparison->compare(pEE, pEMdFValue, 0)) {
			++m_result;
		}
		break;
	}

	// Then deal with next
	if (m_next != 0) {
		m_next->exec(pEE, pInstObj);
	}
}





