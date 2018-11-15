/*
 * emdf_value.cpp
 *
 * EMdFValue class
 *
 * Ulrik Petersen
 * Created: 1/18-2003
 * Last update: 11/15-2018
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


/**@file emdf_value.cpp
 *@brief Implementation of EMdFValue (EMdF layer)
 */


#include <emdf_value.h>
#include <emdfdb.h>
#include <string_func.h>
#include <string_list.h>
#include <enum_const_cache.h>
#include <monads.h>
#include <debug.h>
#include <iostream>

// Don't use unless you plan to = assign a new value later
EMdFValue::EMdFValue()
{
	// Dummily assign kEVInt kind and 0 value
	m_kind = kEVInt;
	m_u.m_int = 0;
}

EMdFValue::EMdFValue(eEVkind kind, long i)
{
	m_kind = kind;
	switch (kind) {
	case kEVInt:
	case kEVEnum:
		m_u.m_int = i;
		break;
	case kEVID_D:
		m_u.m_id_d = i;
		break;
	default:
		ASSERT_THROW(false, "Unknown eEVkind");
	};
}

EMdFValue::EMdFValue(eEVkind kind, IntegerList* pIntegerList)
{
	ASSERT_THROW(kind == kEVListOfInteger || kind == kEVListOfID_D,
		     "kind is neither EVListOfInteger nor EVListOfID_D");
	m_kind = kind;
	m_u.m_pIntegerList = pIntegerList;
}

EMdFValue::EMdFValue(const SetOfMonads& som)
{
	m_kind = kEVSetOfMonads;
	m_u.m_pSOM = new SetOfMonads(som);
}

EMdFValue::EMdFValue(const EMdFValue& other)
{ 
	assign(other);
}

EMdFValue::EMdFValue(const std::string& str)
{
	m_kind = kEVString;
	m_u.m_pString = new std::string(str);
}

EMdFValue::~EMdFValue()
{
	deleteContents();
}


bool EMdFValue::compare(const EMdFValue& other, eComparisonOp op) const
{
	bool bResult;

	switch (m_kind) {
	case kEVInt:
	case kEVEnum:
	case kEVID_D:
		bResult = compareInt(other, op);
		break;
	case kEVString:
		bResult = compareString(other, op);
		break;
	case kEVListOfInteger:
	case kEVListOfID_D:
		bResult = compareListOfIntegerWithOther(other, op);
		break;
	case kEVSetOfMonads:
		bResult = compareSetOfMonads(other, op);
		break;
	default:
		ASSERT_THROW(false, "Unknown eEVKind");
	}
	return bResult;
} 

SetOfMonads EMdFValue::getSOM(void) const
{
	ASSERT_THROW(m_kind == kEVSetOfMonads, "EMdFValue::getSOM() called when the EMdFValue is not a set of monads.");
	return *m_u.m_pSOM;
}

const std::string EMdFValue::getString(void) const
{
	ASSERT_THROW(m_kind == kEVString, "EMdFValue::getString() called when the EMdFValue is not a string.");
	return *m_u.m_pString;
}

IntegerList *EMdFValue::getIntegerList (void) const
{ 
	ASSERT_THROW(m_kind == kEVListOfInteger 
		     || m_kind == kEVListOfID_D, 
		     "EMdFValue::getIntegerList() called when the EMdFValue is neither a list of integer, nor a list of ID_D.");
	return m_u.m_pIntegerList; 
}

bool EMdFValue::compareInt(const EMdFValue& other, eComparisonOp op) const
{
	// Get our own value
	long left_value;
	switch (m_kind) {
	case kEVInt:
		left_value = m_u.m_int;
		break;
	case kEVEnum:
		left_value = m_u.m_int;
		break;
	case kEVID_D:
		left_value = m_u.m_id_d;
		break;
	case kEVString:
		ASSERT_THROW(false, "It was an EVString!");
	case kEVSetOfMonads:
		ASSERT_THROW(false, "It was an EVSetOfMonads!");
	default:
		ASSERT_THROW(false, "Unknown eEVkind");
	}

	bool bResult;
	if (other.m_kind == kEVListOfID_D
	    || other.m_kind == kEVListOfInteger) {
		ASSERT_THROW(op == kIn, "operator is not kIn");
		bResult = other.compareListOfIntegerWithOther(*this, op);
	} else if (other.m_kind == kEVSetOfMonads) {
		ASSERT_THROW(op == kIn, "operator is not kIn");
		bResult = other.compareSetOfMonads(*this, op);
	} else {
		// Get other's value
		long right_value;
		switch (other.m_kind) {
		case kEVInt:
			right_value = other.m_u.m_int;
			break;
		case kEVEnum:
			right_value = other.m_u.m_int;
			break;
		case kEVID_D:
			right_value = other.m_u.m_id_d;
			break;
		case kEVString:
			ASSERT_THROW(false, "kind is string");
		default:
			ASSERT_THROW(false, "Unknown eEVKind");
		}

		// Compare
		switch (op) {
		case kEqual:
			bResult = left_value == right_value;
			break;
		case kLessThan:
			bResult = left_value < right_value;
			break;
		case kGreaterThan:
			bResult = left_value > right_value;
			break;
		case kNotEqual:
			bResult = left_value != right_value;
			break;
		case kLessThanOrEqual:
			bResult = left_value <= right_value;
			break;
		case kGreaterThanOrEqual:
			bResult = left_value >= right_value;
			break;
		default:
			ASSERT_THROW(false, "Unknown eEVKind");
			break;
		}
	}

	// Return result
	return bResult;
}

bool EMdFValue::compareString(const EMdFValue& other, eComparisonOp op) const
{
	// Make sure that other is string
	if (other.m_kind != kEVString) {
		ASSERT_THROW(false, "Kind is not EVString");
	}

	// Compare
	bool bResult;
	switch (op) {
	case kEqual:
		bResult = *m_u.m_pString == *other.m_u.m_pString;
		break;
	case kLessThan:
		bResult = *m_u.m_pString < *other.m_u.m_pString;
		break;
	case kGreaterThan:
		bResult = *m_u.m_pString > *other.m_u.m_pString;
		break;
	case kNotEqual:
		bResult = *m_u.m_pString != *other.m_u.m_pString;
		break;
	case kLessThanOrEqual:
		bResult = *m_u.m_pString <= *other.m_u.m_pString;
		break;
	case kGreaterThanOrEqual:
		bResult = *m_u.m_pString >= *other.m_u.m_pString;
		break;
	default:
		ASSERT_THROW(false, "Unknown operator");
		break;
	}

	// Return result
	return bResult;
}

std::string EMdFValue::toString() const
{
	std::string result;
	switch (m_kind) {
	case kEVInt:
	case kEVEnum:
		result = long2string(m_u.m_int);
		break;
	case kEVID_D:
		result = id_d2number_string(m_u.m_id_d);
		break;
	case kEVString:
		result = *m_u.m_pString;
		break;
	case kEVListOfInteger:
	case kEVListOfID_D:
		result = m_u.m_pIntegerList->getDelimitedString(DEFAULT_LIST_DELIMITER);
		break;
	case kEVSetOfMonads:
		result = m_u.m_pSOM->toString();
		break;
	default:
		ASSERT_THROW(false, "Unknown kEVKind");
	}
	return result;
}

std::string EMdFValue::toString(id_d_t feature_type_id, EnumConstCache *pEnumCache) const
{
	if (featureTypeIdIsENUM(feature_type_id)) {
		if (m_kind != kEVEnum) {
			return toString();
		} else {
			id_d_t enum_id = STRIP_ENUM_ID_OF_LOWER_BITS(feature_type_id);
			long enum_value = getInt();
			const EnumConstInfo* pECI = pEnumCache->find(enum_id, enum_value);
			return pECI->getName();
		}
	} else if (featureTypeIdIsListOfENUM(feature_type_id)) {
		if (m_kind != kEVListOfInteger) {
			return toString();
		} else {
			id_d_t enum_id = STRIP_ENUM_ID_OF_LOWER_BITS(feature_type_id);
			std::string result("(");
			IntegerList *pList = getIntegerList();
			IntegerListConstIterator ilci = pList->const_iterator();
			if (ilci.hasNext()) {
				long first_enum = ilci.next();
				const EnumConstInfo* pECI = pEnumCache->find(enum_id, first_enum);
				result += pECI->getName();
			}
			
			while (ilci.hasNext()) {
				result += ',';
				
				long next_enum = ilci.next();
				const EnumConstInfo* pECI = pEnumCache->find(enum_id, next_enum);
				result += pECI->getName();
			}
			
			result += ')';
			return result;
		}
	} else {
		// It isn't enum or list of enum. Do the normal thing.
		return toString();
	}
}

void EMdFValue::assign(const EMdFValue& other)
{
	m_kind = other.m_kind;
	switch (m_kind) {
	case kEVInt:
	case kEVEnum:
		m_u.m_int = other.m_u.m_int;
		break;
	case kEVID_D:
		m_u.m_id_d = other.m_u.m_id_d;
		break;
	case kEVString:
		m_u.m_pString = new std::string(*other.m_u.m_pString);
		break;
	case kEVListOfInteger:
	case kEVListOfID_D:
		if (other.m_u.m_pIntegerList == 0) {
			m_u.m_pIntegerList = 0;
		} else {
			m_u.m_pIntegerList = new IntegerList(*other.m_u.m_pIntegerList);
		}
		break;
	case kEVSetOfMonads:
		m_u.m_pSOM = new SetOfMonads(*(other.m_u.m_pSOM));
		break;
	default:
		ASSERT_THROW(false, "Unknown eEVKind");
	};
}

void EMdFValue::changeString(const std::string& newString)
{
	// Make sure it is a string already.
	ASSERT_THROW(m_kind == kEVString, "Kind is not string");

	// Set string
	*m_u.m_pString = newString;
}

EMdFValue& EMdFValue::operator=(const EMdFValue& other)
{
	deleteContents();
	assign(other);
	return *this;
}

void EMdFValue::deleteContents()
{
	switch (m_kind) {
	case kEVString:
		delete m_u.m_pString;
		break;
	case kEVListOfInteger:
	case kEVListOfID_D:
		delete m_u.m_pIntegerList;
		break;
	case kEVInt:
	case kEVEnum:
	case kEVID_D:
		// Do nothing
		break;
	case kEVSetOfMonads:
		delete m_u.m_pSOM;
		break;
	default:
		ASSERT_THROW(false, "Unknown eEVKind");
	}
}

bool EMdFValue::compareListOfIntegerWithOther(const EMdFValue& other, eComparisonOp op) const
{
	bool bResult = false;
	// We can only do HAS, IN, or = here.
	// This is called for both, with other and this switched around for
	// kHas, and with the other non-defined for =.
	ASSERT_THROW(op == kHas || op == kIn || op == kEqual,
		     "Op is neither kHas nor kIn, nor kEqual");

	// We must ourselves be a list of integer or ID_D.
	ASSERT_THROW(m_kind == kEVListOfInteger || m_kind == kEVListOfID_D,
		     "Kind is neither kEVListOfInteger nor kEVListOfID_D");

	// Other must be an atomic value of either Int, ID_D, or Enum type.
	ASSERT_THROW((((op == kHas || op == kIn) 
		       && (other.m_kind == kEVInt || other.m_kind == kEVID_D
			   || other.m_kind == kEVEnum))
		      || 
		      ((op == kEqual)
		       && (other.m_kind == kEVListOfInteger || m_kind == kEVListOfID_D))),
		     "Other must be an atomic value of either Int, ID_D or Enum type");
	
	if (op == kEqual) {
		IntegerListConstIterator ci = m_u.m_pIntegerList->const_iterator();
		IntegerListConstIterator ciother = other.m_u.m_pIntegerList->const_iterator();
		bResult = true;
		// This while-loop will terminate with bResult = false if it was not there.
		while (ci.hasNext()
		       && ciother.hasNext()) {
			long myint = ci.next();
			long otherint = ciother.next();
			if (myint != otherint) {
				bResult = false;
				break; // Break out of while-loop, since we are done!
			}
		}
		if (bResult) {
			bResult = !ci.hasNext() && !ciother.hasNext();
		}
		
	} else {
		// Get other's value
		long other_value;
		switch (other.m_kind) {
		case kEVInt:
			other_value = other.m_u.m_int;
			break;
		case kEVEnum:
			other_value = other.m_u.m_int;
			break;
		case kEVID_D:
			other_value = other.m_u.m_id_d;
			break;
		case kEVString:
			ASSERT_THROW(false, "Kind is kEVString");
			break;
		default:
			ASSERT_THROW(false, "Unknown eEVKind");
			break;
		}
		IntegerListConstIterator ci = m_u.m_pIntegerList->const_iterator();
		bResult = false;
		// This while-loop will terminate with bResult = false if it was not there.
		while (ci.hasNext()) {
			long myint = ci.next();
			if (myint == other_value) {
				bResult = true;
				break; // Break out of while-loop, since we are done!
			}
		}
	}
	
	
	return bResult;
}

bool EMdFValue::compareSetOfMonads(const EMdFValue& other, eComparisonOp op) const
{
	bool bResult = false;
	// We can only do HAS or IN here.
	// This is called for both, with other and this switched around for
	// kHas and kIn
	ASSERT_THROW(op == kHas || op == kIn || op == kEqual,
		     "Op is neither kHas nor kIn nor kEqual");

	// We must ourselves be a set of monads
	ASSERT_THROW(m_kind == kEVSetOfMonads,
		     "Kind is not kSetOfMonads");

	// Other must be an atomic value of int type, or a set of monads
	ASSERT_THROW(((op == kHas || op == kIn) && other.m_kind == kEVInt)
		     || ((op == kEqual) && other.m_kind == kEVSetOfMonads),
		     "Other must be an atomic value of Int, or a set of monads.");
	
	if (op == kEqual) {
		bResult = (*(m_u.m_pSOM)) == (*(other.m_u.m_pSOM));
	} else {
		ASSERT_THROW(op == kHas || op == kIn,
			     "op was neither kHas nor kIn.");

		// Get other's value
		long other_value;
		switch (other.m_kind) {
		case kEVInt:
			other_value = other.m_u.m_int;
			break;
		default:
			ASSERT_THROW(false, "Unknown eEVKind");
			break;
		}
		bResult = m_u.m_pSOM->isMemberOf((monad_m) other_value);
	}
	
	
	return bResult;
}
