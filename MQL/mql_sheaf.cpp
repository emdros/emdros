/*
 * mql_sheaf.cpp
 *
 * MQL sheafs
 *
 * Ulrik Petersen
 * Created: 3/8-2001
 * Last update: 11/30-2018
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


/**@file mql_sheaf.cpp
 *@brief Implementation of MQL Sheaf and related classes (MQL layer)
 */



#include <emdfdb.h>
#include <emdf_value.h>
#include <mql_sheaf.h>
#include <mql_execution_environment.h>
#include <mql_utility.h>
#include <string_func.h>
#include <mql_types.h>
#include <mql_query.h>
#include <environment_emdros.h>
#include <debug.h>
#include <string_list.h>


/////////////////////////////////////////////////////////////////
//
// MatchedObject
//
/////////////////////////////////////////////////////////////////
/** Constructor for an EMPTY_mo MatchedObject.
 *@internal
 *
 * @param SmMinus1 The monad at which the emptiness occurred, minus 1.
 */
MatchedObject::MatchedObject(monad_m SmMinus1)
	: m_sheaf(0)
{
	// kMOKEMPTY_mo
	m_id_d = -1;

	m_pObjectBlock = 0;

	m_last = SmMinus1;
	m_u.first = SmMinus1;
	m_values.m_pValue_vec = 0;
}


/** Constructor for an ID_D matched object.
 *@internal
 *
 * @param id_d The id_d of the object.
 *
 * @param monads The set of monads of the object.
 *
 * @param sheaf The inner sheaf.
 *
 * @param pObjectBlock A pointer to the object block from which this came.
 */
MatchedObject::MatchedObject(id_d_t id_d, 
			     const SetOfMonads& monads, 
			     Sheaf* sheaf, 
			     ObjectBlockBase *pObjectBlock)
	: m_sheaf(sheaf)
{
	m_pObjectBlock = pObjectBlock;
	if (monads.hasOnlyOneMSE()) {
		m_last = monads.last();
		m_u.first = monads.first();
	} else {
		m_last = -1*monads.last();
		m_u.pMonad_ms = new SetOfMonads(monads);
	}
	m_values.m_pValue_vec = 0;
	m_id_d = id_d;
}

/** Constructor for an ID_D matched object with a single range or single monad
 * SetOfMonads.
 *@internal
 *
 * @param id_d The id_d of the object.
 *
 * @param first_monad The first monad of the single range or single
 * monad SOM.
 *
 * @param last_monad The last monad of the single range or single
 * monad SOM.
 *
 * @param sheaf The inner sheaf.
 *
 * @param pObjectBlock A pointer to the object block from which this came.
 */
MatchedObject::MatchedObject(id_d_t id_d, 
			     monad_m first_monad,
			     monad_m last_monad,
			     Sheaf* sheaf, 
			     ObjectBlockBase *pObjectBlock)
	: m_sheaf(sheaf)
{
	m_pObjectBlock = pObjectBlock;
	m_last = last_monad;
	m_u.first = first_monad;
	m_values.m_pValue_vec = 0;
	m_id_d = id_d;
}

/** Constructor for an ID_M matched object.
 *@internal
 *
 * @param monads The set of monads of the gap.
 *
 * @param bRetrieve retrieve/noretrieve boolean
 *
 * @param bFocus Focus boolean.
 *
 * @param sheaf The inner sheaf.
 */
MatchedObject::MatchedObject(const SetOfMonads& monads, 
			     ObjectBlockBase *pOBB,
			     bool bRetrieve,
			     bool bFocus,
			     Sheaf* sheaf)
	: m_sheaf(sheaf)
{
	m_pObjectBlock = pOBB;
	m_sheaf = sheaf;
	if (bFocus) {
		m_id_d = -4;
	} else {
		if (bRetrieve) {
			m_id_d = -3;
		} else {
			m_id_d = -2;
		}
	}
	if (monads.hasOnlyOneMSE()) {
		m_last = monads.last();
		m_u.first = monads.first();
	} else {
		m_last = -1*monads.last();
		m_u.pMonad_ms = new SetOfMonads(monads);
	}
	m_values.m_pValue_vec = 0;
}

MatchedObject::MatchedObject(const InstObject *pInstObj, ObjectBlockBase *pObjectBlock)
{
	m_pObjectBlock = pObjectBlock;
	m_sheaf = 0;

	if (pInstObj->somHasOnlyOneMSE()) {
		m_last = pInstObj->last();
		m_u.first = pInstObj->first();
	} else {
		m_last = -1*pInstObj->last();
		SetOfMonads monads;
		pInstObj->getMonads(monads);
		m_u.pMonad_ms = new SetOfMonads(monads);
	}
	m_id_d = -1*(pInstObj->getID_D()) - 5;
	m_values.m_pInstObject = pInstObj;
}

/** Copy constructor for MatchedObject.
 *@internal
 * 
 * Used by #copyMO(OTHER).
 *
 */
MatchedObject::MatchedObject(const MatchedObject& other)
{
	if (other.m_sheaf == 0) {
		m_sheaf = 0;
	} else {
		m_sheaf = new Sheaf(*other.m_sheaf);
	}
	m_id_d = other.m_id_d;

	if (m_id_d >= 0 || m_id_d <= -2) {
		m_pObjectBlock = other.m_pObjectBlock;
	} else {
		m_pObjectBlock = 0;
	}
	m_last = other.m_last;
	if (m_last >= 0) {
		m_u.first = other.m_u.first;
	} else {
		m_u.pMonad_ms = new SetOfMonads(*other.m_u.pMonad_ms);
	}

	if (other.isInstObject()) {
		m_values.m_pInstObject = other.m_values.m_pInstObject;
	} else {
		if (other.m_values.m_pValue_vec == 0) {
			m_values.m_pValue_vec = 0;
		} else {
			//m_values.m_pValue_vec = new value_vec_t;
			m_values.m_pValue_vec = 0; // It will be created in addEMdFValue...
			for (value_vec_t::size_type i = 0; i < other.m_values.m_pValue_vec->size(); i++) {
				EMdFValue *pValue = new EMdFValue(*((*other.m_values.m_pValue_vec)[i]));
				addEMdFValue((unsigned int) i, pValue);
			}
		}
	}
}


/** Destructor.
 */
MatchedObject::~MatchedObject()
{
	delete m_sheaf;

	if (m_last < 0) {
		delete m_u.pMonad_ms;
	}

	if (!isInstObject()) {
		if (m_values.m_pValue_vec != 0) {
			for (value_vec_t::size_type i = 0; i < m_values.m_pValue_vec->size(); i++) {
				delete (*m_values.m_pValue_vec)[i];
				(*m_values.m_pValue_vec)[i] = 0;
			}
			delete m_values.m_pValue_vec;
		}
	}
}

/** Get the set of monads.
 *
 * Automatically takes care of creating a SetOfMonads if the set of
 * monads is only represented by a first and a last monad.
 *
 * @return The set of monads associated with the matched object.
 */
SetOfMonads MatchedObject::getMonads(void) const
{
	if (m_last >= 0) {
		return SetOfMonads(m_u.first, m_last);
	} else {
		return *m_u.pMonad_ms;
	}
}


/** Count objects recursively.
 *
 * @param bUseOnlyFocusObjects If true, only objects which
 * have the focus boolean set to true will be counted.
 *
 * @return The number of (possibly focus) objects in this
 * MatchedObject, and any embedded Sheaf.
 */
long MatchedObject::countObjects(bool bUseOnlyFocusObjects) const
{
	long result = 0;
	
	bool bDoIt = true;
	if (bUseOnlyFocusObjects) {
		bDoIt = getFocus();
	}

	if (bDoIt) {
		++result;
	}

	// Recurse
	if (m_sheaf != 0) {
		result += m_sheaf->countObjects(bUseOnlyFocusObjects);
	}

	return result;
}

/** Count objects recursively.
 *
 * If object_type_name is empty, all object types are counted.
 *
 * If object_type_name is "pow_m", only gap_blocks and opt_gap_blocks
 * are counted.
 *
 * If object_type_name is neither empty nor "pow_m", only objects of the
 * given object type are counted.
 *
 *
 * @param object_type_name A string giving the normalized
 * object type name of the objects to count.  This should have
 * been passed through normalizeOTName() before calling this
 * method.
 *
 * @param bUseOnlyFocusObjects If true, only objects of the
 * given object type which have the focus boolean set to true
 * will be counted.
 *
 * @return The number of (possibly focus) objects in this
 * MatchedObject, and any embedded Sheaf.
 */
long MatchedObject::countObjectsFromObjectType(const std::string& object_type_name, bool bUseOnlyFocusObjects) const
{
	long result = 0;
	
	bool bDoIt = true;
	if (bUseOnlyFocusObjects) {
		bDoIt = getFocus();
	}

	if (bDoIt && !object_type_name.empty()) {
		bDoIt = object_type_name == getObjectTypeName();
	}

	if (bDoIt) {
		++result;
	}

	// Recurse
	if (m_sheaf != 0) {
		result += m_sheaf->countObjectsFromObjectType(object_type_name, bUseOnlyFocusObjects);
	}

	return result;
}
	
/** Count straws recursively.
 *
 * @return The number of straws in this MatchedObject's
 * embedded Sheaf, or 0 if that sheaf is empty.
 */
long MatchedObject::countStraws() const
{
	long result = 0;
	if (m_sheaf != 0) {
		result += m_sheaf->countStraws();
	}
	return result;
}


/** Get a set of monads from a sheaf.
 *
 * This process is explained under Sheaf::getSOM().  The set of monads
 * is gathered up in \p som.
 *
 * The \p bUseOnlyFocusObjects boolean parameter should be set to
 * \p true if you only want matched objects with the focus
 * boolean set to true to be used.
 *
 * Recurses down into the inner sheaf if it is not empty.
 *
 * @param som The set of monads to use for gathering up the relevant
 * matched objects.
 *
 * @param bUseOnlyFocusObjects Should be \p true if you only wish to
 * use matched objects with the focus boolean set to \p true; \p false
 * if you wish to use all matched objects in the sheaf.
 */
void MatchedObject::getSOM(SetOfMonads& som, bool bUseOnlyFocusObjects) const
{
	bool bDoIt = true;
	if (bUseOnlyFocusObjects) {
		bDoIt = getFocus();
	}

	if (bDoIt) {
		som.unionWith(getMonads());
	}

	// Recurse
	if (m_sheaf != 0) {
		m_sheaf->getSOM(som, bUseOnlyFocusObjects);
	}
}

void MatchedObject::harvestMarks(String2SOMMap& result, bool bUseSingleMarks) const
{
	std::string marks_string = getMarksString();
	if (!marks_string.empty()) {
		SetOfMonads som = getMonads();
		
		if (bUseSingleMarks) {
			std::list<std::string> marks_list;

			// cut off the first backping, so as to
			// not have an empty string at the beginning
			// of marks_list
			split_string(marks_string.substr(1, std::string::npos), "`", marks_list);
			std::list<std::string>::const_iterator marks_list_ci = marks_list.begin();
			std::list<std::string>::const_iterator marks_list_cend = marks_list.end();
			while (marks_list_ci != marks_list_cend) {
				std::string mark = std::string("`") + *marks_list_ci;
				result.addMonadSet(mark, som);
				++marks_list_ci;
			}
		} else {
			result.addMonadSet(marks_string, som);
		}
	}

	// Recurse
	if (m_sheaf != 0) {
		m_sheaf->harvestMarks(result, bUseSingleMarks);
	}
}


/** Get inner sheaf.
 *
 * The pointer returned is a "const Sheaf*".  You should not attempt
 * to delete the sheaf.
 * 
 * @return A pointer to the inner sheaf.
 */
const Sheaf* MatchedObject::getSheaf(void) const
{
	return m_sheaf;
}

bool MatchedObject::sheafIsEmpty(void) const
{ 
	if (m_sheaf == 0) {
		return true;
	} else {
		const ListOfStraws *pLOS = m_sheaf->get_plist();
		if (pLOS == 0) {
			return true;
		} else {
			return pLOS->isEmpty();
		}
	}
}

/** Get the string-value of an EMdFValue at a given index.
 *
 * Convenience function for getEMdFValue(int).  
 *
 * Returns the given EMdFValue as a string.  For enums, the enum
 * constant name is returned, not the integer associated with
 * it. Integers and ID_Ds are converted to strings containing their
 * base-10 equivalents. 
 *
 * Lists of integer and id_d are returned as a string containing the
 * comma-delimited, (parenthesis-enclosed) string of the base-10
 * equivalents of each member integer or id_d. For example:
 * "(1,2,3)". Empty strings are returned as "()", whereas singletons
 * are returned as, e.g., "(1)".
 * 
 * Always succeeds, so long as index is in range.  Throws an
 * EmdrosException if index out of range.
 *
 * @see MatchedObject::getEMdFValue(int)
 *
 * @param index The integer index of the EMdFValue to get the value
 * of.
 * @return The given EMdFValue, converted to a string.
 */
std::string MatchedObject::getFeatureAsString(int index) const
{
	const EMdFValue *pValue = getEMdFValue(index);
	if (pValue == 0) {
		throw EmdrosException("MatchedObject::getFeatureAsString: index out of range");
		return 0; // Just to fool the compiler into giving no warning.
	} else {
		switch (pValue->getKind()) {
		case kEVInt:
			return long2string(pValue->getInt());
			break;
		case kEVEnum: 
			{
				Feature *pFeature = m_pObjectBlock->getFeatureRetrievalFeature(index);
				return pFeature->getEnumConstNameFromValue(pValue->getEnum());
			}
			break;
		case kEVID_D:
			return id_d2string(pValue->getID_D());
			break;
		case kEVString:
			return pValue->getString();
			break;
		case kEVListOfInteger: {
			Feature *pFeature = m_pObjectBlock->getFeatureRetrievalFeature(index);
			id_d_t feature_type_id = pFeature->getOutputFeatureTypeID();
			if (featureTypeIdIsListOfENUM(feature_type_id)) {
				std::string result("(");
				IntegerList *pList = pValue->getIntegerList();
				IntegerListConstIterator ilci = pList->const_iterator();
				if (ilci.hasNext()) {
					long first_enum = ilci.next();
					result += pFeature->getEnumConstNameFromValue(first_enum);
				}
				while (ilci.hasNext()) {
					result += ',';
					
					long next_enum = ilci.next();
					result += pFeature->getEnumConstNameFromValue(next_enum);
				}
				
				result += ')';
				return result;
			} else {
				std::string result("(");
				IntegerList *pList = pValue->getIntegerList();
				IntegerListConstIterator ilci = pList->const_iterator();
				if (ilci.hasNext()) {
					long first_int = ilci.next();
					result += long2string(first_int);
				}
				while (ilci.hasNext()) {
					result += ',';
					
					long next_int = ilci.next();
					result += long2string(next_int);
				}
				
				result += ')';
				return result;
			}
		}
			break;

		case kEVListOfID_D:
			return std::string("(") + pValue->getIntegerList()->getDelimitedString(",") + ")";
			break;
		case kEVSetOfMonads:
			return pValue->toString();
			break;
		}
		ASSERT_THROW(false, "getFeatureAsString: Unknown EMdFValue kind.");
		return ""; // Just to fool the compiler into giving no warning
	}
}

/** Get the string-value of an EMdFValue at a given index.
 *
 * Convenience function for getEMdFValue(int).  
 *
 * Returns the given EMdFValue as a long.  For enums, the enum integer
 * value is returned, not the string name associated with it. Integers
 * and ID_Ds are returned as they are.
 *
 * Throws an EmdrosException if the EMdFValue in question is a string
 * or a list.  Throws an EmdrosException if index out of range.
 * 
 * @see MatchedObject::getEMdFValue(int)
 *
 * @param index The integer index of the EMdFValue to get the value
 * of.
 * @return The given EMdFValue, converted to a string.
 */
long MatchedObject::getFeatureAsLong(int index) const
{
	const EMdFValue *pValue = getEMdFValue(index);
	if (pValue == 0) {
		throw EmdrosException("MatchedObject::getFeatureAsLong: index out of range");
		return 0; // Just to fool the compiler into giving no warning.
	} else {
		switch (pValue->getKind()) {
		case kEVInt:
			return pValue->getInt();
			break;
		case kEVEnum:
			return pValue->getEnum();
			break;
		case kEVID_D:
			return pValue->getID_D();
			break;
		case kEVString:
			throw EmdrosException("MatchedObject::getFeatureAsLong: Cannot convert a string to a long.");
			return 0; // Just to fool the compiler into giving no warning.
			break;
		case kEVListOfInteger:
			throw EmdrosException("MatchedObject::getFeatureAsLong: Cannot convert a list of integer to a long.");
			return 0; // Just to fool the compiler into giving no warning.
			break;
		case kEVListOfID_D:
			throw EmdrosException("MatchedObject::getFeatureAsLong: Cannot convert a list of id_d to a long.");
			return 0; // Just to fool the compiler into giving no warning.
			break;
		case kEVSetOfMonads:
			throw EmdrosException("MatchedObject::getFeatureAsLong: Cannot convert a set of monads to a long.");
			return 0; // Just to fool the compiler into giving no warning.
			break;
		}
		ASSERT_THROW(false, "getFeatureAsLong: Unknown EMdFValue kind.");
		return 0; // Just to fool the compiler into giving no warning
	}
}




/** Get the EMdFValue at a given index.
 *
 * Only valid if this is an ID_D matched object.  If the object block
 * from which it arose had a GET clause, this will get the EMdFValue
 * with the given index.
 *
 * See MatchedObject::getEMdFValueIndex() and
 * MatchedObject::getFeatureList() for ways of getting the index to
 * use.
 *
 * See MatchedObject::getEMdFValue(feature_name) for another (slower)
 * method of getting at the EMdFValue objects.
 *
 * @param index The index into the std::vector of EMdFValue* at which
 * to get the EMdFValue.  The index is 0-based, not 1-based.
 *
 * @return 0 if index out of range, otherwise the EMdFValue at the
 * required spot.
 */
const EMdFValue *MatchedObject::getEMdFValue(int index) const
{
	if (isInstObject()) {
		unsigned int size = m_values.m_pInstObject->getFeatureValueArrSize();
		if (index < 0 
		    || ((unsigned int) index) >= size) {
			return 0;
		} else {
			return m_values.m_pInstObject->getFeature((unsigned int) index);
		}
	} else {
		if (m_values.m_pValue_vec == 0) {
			return 0;
		} else {
			value_vec_t::size_type size = m_values.m_pValue_vec->size();
			if (index < 0 
			    || ((value_vec_t::size_type) index) >= size) {
				return 0;
			} else {
				return (*m_values.m_pValue_vec)[index];
			}
		}
	}
}

/** Get the EMdFValue by means of the feature name.
 *
 * Only valid if this is an ID_D matched object.  If the object block
 * from which it arose had a GET clause, this will get the EMdFValue
 * from the given feature name.
 *
 * This is just a convenience method: It really calls
 * MatchedObject::getEMdFValueIndex() and then
 * MatchedObject::getEMdFValue(index) to obtain the desired result.
 *
 * This is an \p O(n) operation, where \p n is the number of EMdFValue
 * objects.  As such, it is slower than a simple
 * MatchedObject::getEMdFValue(int) by index.
 *
 * You should consider using MatchedObject::getFeatureList(), then
 * iterating over that to get the indexes to use, then use
 * MatchedObject::getEMdFValue(index) to get at the EMdFValue objects.
 *
 * @param feature_name The feature name (case-insensitive) for which
 * to get the EMdFValue.
 *
 * @return 0 if feature not found, otherwise the EMdFValue for the
 * given feature.
 */
const EMdFValue *MatchedObject::getEMdFValue(const std::string& feature_name) const
{
	int index = getEMdFValueIndex(feature_name);
	return getEMdFValue(index);
}

// Returns -1 on not found
/** Get the index in the EMdFValue vector of a given feature.
 *
 * Only valid if this is an ID_D matched object.  If the object block
 * from which it arose had a GET clause, this will get the index of
 * the EMdFValue from the given feature name.
 *
 * The feature name comparison used is is case-insensitive.
 *
 * This is an \p O(n) operation, where \p n is the number of EMdFValue
 * objects.  As such, it is slower than a simple
 * MatchedObject::getEMdFValue(int) by index.
 *
 * You should consider using MatchedObject::getFeatureList, then
 * iterating over that to get the indexes to use.
 *
 * @param feature_name The name of the feature to find.
 *
 * @return The index of the EMdFValue corresponding to the feature. -1
 * if not found.
 */
int MatchedObject::getEMdFValueIndex(const std::string& feature_name) const
{
	if (isInstObject()) {
		unsigned int size = m_values.m_pInstObject->getFeatureValueArrSize();
		if (size == 0) {
			return -1;
		} else {
			for (unsigned int i = 0; i < size; ++i) {
				Feature *pFeature = m_pObjectBlock->getFeatureRetrievalFeature(i);
				const std::string& v_feature_name = pFeature->getFeature();
				
				// If this was the one, return i
				if (strcmp_nocase(feature_name, v_feature_name) == 0) {
					return i;
				}
			}
			
			// If we came here, we didn't find the index
			return -1;
		}
	} else {
		if (m_values.m_pValue_vec == 0) {
			return -1;
		} else {
			// Iterate through vector
			value_vec_t::size_type size = m_values.m_pValue_vec->size();
			for (value_vec_t::size_type i = 0; i < size; ++i) {
				Feature *pFeature = m_pObjectBlock->getFeatureRetrievalFeature((unsigned int) i);
				const std::string& v_feature_name = pFeature->getFeature();
				
				// If this was the one, return i
				if (strcmp_nocase(feature_name, v_feature_name) == 0) {
					return (int) i;
				}
			}
			
			// If we came here, we didn't find the index
			return -1;
		}
	}
}

/** Get enumeration label associated with an enum EMdFValue.
 *
 * Only meaningful if this is an ID_D matched object.  If the object
 * block from which it arose had a GET clause, this will get the
 * enumeration constant label associated with an EMdFValue expressing
 * an enumeration constant integer.
 *
 * The problem that this method solves is that an EMdFValue will, even
 * if the underlying value is an enumeration constant, return that
 * constant's integer value rather than the enumeration constant label
 * associated with the value.  This method returns the label for an
 * EMdFValue at a given index.
 *
 * See MatchedObject::getEMdFValueIndex() and
 * MatchedObject::getFeatureList() for ways of getting the index to
 * use.
 *
 * @param index The index into the std::vector of EMdFValue* at which
 * to get the EMdFValue's enumeration label.  The index is 0-based,
 * not 1-based.
 *
 * @return The empty string (\p "" ) if either: a) The object block
 * did not have a GET clause, b) The index is out of range, c) the
 * value at the index was not an enumeration value.  Otherwise,
 * returns the label associated with the enumeration value at the
 * given index.
 */
std::string MatchedObject::getEnumLabel(int index) const
{
	value_vec_t::size_type size = 0;
	if (isInstObject()) {
		size = m_values.m_pInstObject->getFeatureValueArrSize();
	} else {
		if (m_values.m_pValue_vec == 0) {
			return "";
		} else {
			// Is index out of range?
			size = m_values.m_pValue_vec->size();
		}
	}
	if (index < 0 
	    || ((value_vec_t::size_type) index) >= size) {
		// Yes, index is out of range
		return "";
	} else {
		// No, index is not out of range
		
		// Get value
		const EMdFValue *pValue = getEMdFValue(index);
				
		// Get matching Feature
		Feature *pFeature = m_pObjectBlock->getFeatureRetrievalFeature(index);
		
		// Check that it is ENUM
		id_d_t feature_type_id = pFeature->getOutputFeatureTypeID();
		if (featureTypeIdIsENUM(feature_type_id)
		    || featureTypeIdIsListOfENUM(feature_type_id)) {
			// It was an enum
			return pFeature->getEnumConstNameFromValue(pValue->getEnum());
		} else {
			// It was not an enum
			return "";
		}
	}
}



/** Get list of feature-names in the EMdFValue vector.
 *
 * The list is returned in the order of appearance in the EMdFValue
 * vector.  This means that you can get the indexes of the EMdFValues
 * by calling this method, then iterating over the StringList and
 * keeping track of the index.
 *
 * This may be faster than using MatchedObject::getEMdFValue(const
 * std::string& feature_name).
 */
StringList MatchedObject::getFeatureList(void) const
{
	if (m_id_d < 0 && m_id_d >= -4) {
		return StringList();
	} else {
		StringList result;
		// We use the number of features gotten by
		// the GET part of the ObjectBlock from which
		// this MatchedObject arises.

		// We do not use the full vector of Feature objects,
		// since that may also contain, at the back, some
		// Feature objects for resolution of Object
		// References.
		int size = m_pObjectBlock->getNoOfGETFeatures();
		for (int i = 0; i < size; ++i) {
			Feature *pFeature = m_pObjectBlock->getFeatureRetrievalFeature(i);
			result.addStringBack(pFeature->getFeature());
		}
		return result;
	}
}


/** Get the number of EMdFValue objects in the value vector.
 *
 * Only valid if this is an ID_D matched object.  If the object block
 * from which it arose had a GET clause, this will return the number
 * of values in the value vector resulting from the GET clause.
 *
 * @return The number of elements in the value vector.
 */
unsigned int MatchedObject::getNoOfEMdFValues(void) const
{
	if (isInstObject()) {
		return m_values.m_pInstObject->getFeatureValueArrSize();
	} else {
		if (m_values.m_pValue_vec == 0) {
			return 0;
		} else {
			return (unsigned int) m_values.m_pValue_vec->size();
		}
	}
}

/** Add an EMdFValue at a given index.
 *@internal
 *
 * Will initialize the m_values.m_pValue_vec vector if not present already.
 *
 * The "addition" is really an assignment; the pointer will be
 * assigned at the given index.  The index MUST NOT be out of range.
 *
 * @param feature_index The index at which to add the EMdFValue pointer.
 *
 * @param pValue The EMdFValue pointer to add at a given index.
 */
void MatchedObject::addEMdFValue(unsigned int feature_index, EMdFValue *pValue)
{
	if (isInstObject()) {
		ASSERT_THROW(false,
			     "MatchedObject::addEMdFValue called even though m_values is a const InstObject*.  You cannot do this!");
	} else {
		// Create vector if not there already
		if (m_values.m_pValue_vec == 0) {
			ASSERT_THROW(m_pObjectBlock != 0, "pObjectBlock was 0");
			
			// Get length from ObjectBlockBase's vector of Feature*
			int length = m_pObjectBlock->getNoOfFeatureRetrievalFeatures();
			
			// Initialize to this length
			m_values.m_pValue_vec = new value_vec_t(length);
			
			// Set all to nil.  This is called defensive programming.
			for (int i = 0; i < length; ++i) {
				(*m_values.m_pValue_vec)[i] = 0;
			}
		}
  
		// Set the correct pointer
		(*m_values.m_pValue_vec)[feature_index] = pValue;
	}
}


/** Add ourselves to flat sheaf and recurse down into inner sheaf.
 *@internal
 *
 * @param pFlatSheaf The flat sheaf to add ourselves to.
 *
 * @return A boolean indicating whether we were added to pFlatSheaf.
 *
 */
bool MatchedObject::flatten(FlatSheaf *pFlatSheaf)
{
	// Add self
	bool bResult = pFlatSheaf->addMO(this);
  
	// Recurse
	if (m_sheaf != 0) {
		m_sheaf->flatten(pFlatSheaf);
		delete m_sheaf;
		m_sheaf = 0;
	}
	return bResult;
}

/** Get the object type name.
 *
 * @return If this is an ID_M, return "pow_m".  Otherwise, return the
 * object type of the object block from which this matched object
 * derives.
 */
std::string MatchedObject::getObjectTypeName() const 
{ 
	return (m_id_d < 0 && m_id_d >= -4) ? "pow_m" : m_pObjectBlock->getObjectTypeName(); 
}

/** Get the object type ID.
 *@internal
 * @return If this is an ID_M, return OBJECT_TYPE_POW_M, otherwise
 * return the object type id of the object block from which this
 * matched object arose.
 */
id_d_t MatchedObject::getObjectTypeId() const 
{ 
	return (m_id_d < 0 && m_id_d >= -4) ? OBJECT_TYPE_POW_M : m_pObjectBlock->getObjectTypeId(); 
}


/** Print ourselves to console.
 *@internal
 *
 * Also recurses down into inner sheaf, but only if bIsForFullSheaf is
 * \p true.
 *
 * @param pOut the EMdFOutput object to print to.
 *
 * @param bIsForFullSheaf Should be true if we are printing this as a
 * full sheaf, false if this is a flat sheaf.
 */
void MatchedObject::printConsole(EMdFOutput *pOut, bool bIsForFullSheaf) const
{
	/* Assumed 63-bits. Right now, we only have 31 bits. */
#define MAX_LONG_STRING_LENGTH (20) 
	if (!getRetrieve()) {
		// Nothing to do
		return;
	} else {
		const int MAX_ID_D_BUFSIZE = (8 + 3 + MAX_LONG_STRING_LENGTH + 1 + MAX_LONG_STRING_LENGTH + 3 + 4 + 1 + 1);
		std::string outstr;
		outstr.reserve(MAX_ID_D_BUFSIZE);

		char szBuffer[MAX_LONG_STRING_LENGTH];
		char *szLong;
		unsigned int nStringLength;

		if (isID_M()) {
			outstr.append("[ pow_m", 7);
		} else {
			outstr.append("[ ", 2);	
			outstr += m_pObjectBlock->getObjectTypeName();
			outstr.push_back(' ');
			
			long2szNonReversing(getID_D(), szBuffer, MAX_LONG_STRING_LENGTH, &szLong, &nStringLength);
			
			outstr.append(szLong, nStringLength);
		}

		if (m_last < 0) {
			outstr += m_u.pMonad_ms->toString();
		} else {
			if (m_last == m_u.first) {
				outstr.append(" { ", 3);
				long2szNonReversing(m_last, szBuffer, MAX_LONG_STRING_LENGTH, &szLong, &nStringLength);
		
				outstr.append(szLong, nStringLength);
				outstr.append(" } ", 3);
			} else {
				outstr.append(" { ", 3);
				long2szNonReversing(m_u.first, szBuffer, MAX_LONG_STRING_LENGTH, &szLong, &nStringLength);
		
				outstr.append(szLong, nStringLength);
				outstr.push_back('-');
				long2szNonReversing(m_last, szBuffer, MAX_LONG_STRING_LENGTH, &szLong, &nStringLength);
		
				outstr.append(szLong, nStringLength);
				outstr.append(" } ", 3);
			}
		}
		
		if (getFocus()) {
			outstr.append("true ", 5);
		} else {
			outstr.append("false ", 6);
		}
		/*
		outstr += bool_alpha2string(getFocus());
		outstr += ' ';
		*/

		pOut->out(outstr.c_str());

		/*		
		pOut->out("[ ");
		pOut->out(m_pObjectBlock->getObjectTypeName().c_str());
		pOut->out(" ");
		pOut->out(id_d2string(getID_D()).c_str());
		if (m_last < 0) {
			pOut->out(m_u.pMonad_ms->toString());
		} else {
			if (m_last == m_u.first) {
				pOut->out(" { ");
				pOut->out(monad_m2string(m_last).c_str());
				pOut->out(" } ");
			} else {
				pOut->out(" { ");
				pOut->out(monad_m2string(m_u.first).c_str());
				pOut->out('-');
				pOut->out(monad_m2string(m_last).c_str());
				pOut->out(" } ");
			}
		}

		pOut->out(bool_alpha2string(getFocus()).c_str());
		pOut->out(' ');

		*/

		std::string mark = getMarksString();
		if (!mark.empty()) {
			mark.push_back(' ');
			pOut->out(mark.c_str());
		}
	}

	if (isID_D()) {
		std::string outstr;
		outstr.reserve(512);
		unsigned int size = m_pObjectBlock->getNoOfGETFeatures();
		if (size != 0) {
			outstr.push_back('(');
			for (unsigned int i = 0; i < size; ) {
				// Get value 
				const EMdFValue *pValue = getEMdFValue(i);

				// Get matching Feature
				Feature *pFeature = m_pObjectBlock->getFeatureRetrievalFeature(i);

				// Output feature name
				outstr.append(pFeature->getHumanReadableFeatureName());
				outstr.append("=\"", 2);

				// Output string based on type
				id_d_t feature_type_id = pFeature->getOutputFeatureTypeID();
				if (featureTypeIdIsListOf(feature_type_id)) {
					const IntegerList *pIntegerList = pValue->getIntegerList();
					if (pIntegerList == 0) {
						// Do nothing
					} else {
						if (!featureTypeIdIsListOfENUM(feature_type_id)) {
							ASSERT_THROW(featureTypeIdIsListOfINTEGER(feature_type_id)
								     || featureTypeIdIsListOfID_D(feature_type_id),
								     "At this point, we only know how to do lists of integers and lists of id_ds.\nThis is something else. The feature_type_id is:" + long2string(feature_type_id));
							std::string mystr = pIntegerList->getDelimitedString(DEFAULT_LIST_DELIMITER);
							outstr.append(mystr);
						} else {
							IntegerListConstIterator ci = pIntegerList->const_iterator();
							StringList mystringlist;
							while (ci.hasNext()) {
								// Get next enum value
								long cur_enum_value = ci.next();
								std::string enum_const_name = pFeature->getEnumConstNameFromValue(cur_enum_value);
								mystringlist.addStringBack(enum_const_name);
							}
							outstr.append(mystringlist.getDelimitedString(DEFAULT_LIST_DELIMITER));
						}
					}
				} else if (featureTypeIdIsASCIIorSTRING(feature_type_id)) {
					// ASCII or STRING need to be encoded as STRING strings
					outstr.append(encodeSTRINGstring(pValue->toString()));

				} else if (featureTypeIdIsSOM(feature_type_id)) {
					// SetOfMonads need to be encoded as STRING strings
					outstr.append(encodeSTRINGstring(pValue->toString()));

				} else if (featureTypeIdIsENUM(feature_type_id)) {
					// ENUM
					outstr.append(pFeature->getEnumConstNameFromValue(pValue->getEnum()));

				} else { 
					// INTEGER, ID_D need not be escaped...
					ASSERT_THROW(featureTypeIdIsINTEGER(feature_type_id)
						     || featureTypeIdIsID_D(feature_type_id),
						     "At this point, we only know how to do integers and id_ds. This is something else.");
					outstr.append(pValue->toString());
				}

				outstr.push_back('\"');

				// out comma if this is not the last one
				++i;
				if (i < size) {
					outstr.push_back(',');
				}
			}
			outstr.push_back(')');
		}

		pOut->out(outstr);
	}

	// If we are printing as a full sheaf, and the inner sheaf is 
	// not the empty sheaf, we recurse into the inner sheaf.
	// Otherwise, if either we are not printing a full sheaf (but
	// a flat sheaf), or the inner sheaf is empty, we just print
	// the empty sheaf.
	if (bIsForFullSheaf && !sheafIsEmpty()) {
		m_sheaf->printConsole(pOut);
	} else {
		MQLExecEnv::m_EmptySheaf.printConsole(pOut);
	}
	pOut->out(" ]\n");
}

/** Print ourselves as XML.
 *@internal
 *
 * Also recurses down into inner sheaf, but only if bIsForFullSheaf is
 * \p true.
 *
 * @param pOut the EMdFOutput object to print to.
 *
 * @param bIsForFullSheaf Should be true if we are printing this as a
 * full sheaf, false if this is a flat sheaf.
 */
void MatchedObject::printXML(EMdFOutput* pOut, bool bIsForFullSheaf) const
{
	if (!getRetrieve()) {
		// Nothing to do
	} else if (isID_M()) {
		// Make attribute list
		AttributePairList attribute_list;
		attribute_list.push_back(AttributePair("object_type_name", "pow_m"));

		attribute_list.push_back(AttributePair("focus", bool_alpha2string(getFocus())));

		std::string marks = getMarksString();
		if (!marks.empty()) {
			attribute_list.push_back(AttributePair("marks", marks));
		}

		attribute_list.push_back(AttributePair("id_d", NIL_AS_VISIBLE_STRING));
    
		// out matched_object
		pOut->startTag("matched_object", attribute_list, true);
		if (m_last < 0) {
			m_u.pMonad_ms->printXML(pOut);
		} else {
			getMonads().printXML(pOut);
		}
		if (!sheafIsEmpty()) {
			m_sheaf->printXML(pOut);
		} else {
			MQLExecEnv::m_EmptySheaf.printXML(pOut);
		}
		pOut->endTag("matched_object", true);
	} else {
		// Get object type name
		const std::string& object_type_name = m_pObjectBlock->getObjectTypeName();

		// Make attribute list
		AttributePairList attribute_list;
		attribute_list.push_back(AttributePair("object_type_name", object_type_name));

		attribute_list.push_back(AttributePair("focus", bool_alpha2string(getFocus())));

		std::string marks(getMarksString());
		if (!marks.empty()) {
			attribute_list.push_back(AttributePair("marks", getMarksString()));
		}
		attribute_list.push_back(AttributePair("id_d", id_d2string(getID_D())));

		// out matched_object
		pOut->startTag("matched_object", attribute_list, true);
		if (m_last < 0) {
			m_u.pMonad_ms->printXML(pOut);
		} else {
			getMonads().printXML(pOut);
		}

		if (isInstObject() || m_values.m_pValue_vec != 0) {
			pOut->startTag("features", true);
			int size = m_pObjectBlock->getNoOfGETFeatures();
			for (int i = 0; i < size; ++i) {
				// Get value
				const EMdFValue *pValue = getEMdFValue(i);

				// Get Feature
				Feature *pFeature = m_pObjectBlock->getFeatureRetrievalFeature(i);

				// Get feature name
				const std::string& feature_name = pFeature->getHumanReadableFeatureName();

				// Get feature type and valstring
				std::string feature_type;
				std::string enum_name;
				id_d_t feature_type_id = pFeature->getOutputFeatureTypeID();
				std::string valstring;

				if (featureTypeIdIsListOf(feature_type_id)) {
					const IntegerList *pIntegerList = pValue->getIntegerList();
					if (pIntegerList == 0) {
						valstring = "";
					} else {
						if (!featureTypeIdIsListOfENUM(feature_type_id)) {
							ASSERT_THROW(featureTypeIdIsListOfINTEGER(feature_type_id)
								     || featureTypeIdIsListOfID_D(feature_type_id),
								     "At this point, we only know how to do lists of integers and lists of id_ds. This is something else.");
							valstring = pIntegerList->getDelimitedString(DEFAULT_LIST_DELIMITER);
						} else {
							IntegerListConstIterator ci = pIntegerList->const_iterator();
							StringList mystringlist;
							while (ci.hasNext()) {
								// Get next enum value
								long cur_enum_value = ci.next();
								std::string enum_const_name = pFeature->getEnumConstNameFromValue(cur_enum_value);
								mystringlist.addStringBack(enum_const_name);
							}
							valstring = mystringlist.getDelimitedString(DEFAULT_LIST_DELIMITER);
						}
					}
					if (featureTypeIdIsListOfINTEGER(feature_type_id)) {
						feature_type = "list_of_integer";
					} else if (featureTypeIdIsListOfID_D(feature_type_id)) {
						feature_type = "list_of_id_d";
					} else if (featureTypeIdIsListOfENUM(feature_type_id)){
						feature_type = "list_of_enum";
						enum_name = pFeature->getEnumName();
					} else {
						ASSERT_THROW(false, "Unknown FEATURE_TYPE.");
					}
				} else {
					// not ListOf
					if (featureTypeIdIsINTEGER(feature_type_id)) {
						feature_type = "integer";
						valstring = pValue->toString();
					} else if (featureTypeIdIsID_D(feature_type_id)) {
						feature_type = "id_d";
						valstring = pValue->toString();
					} else if (featureTypeIdIsASCII(feature_type_id)
						   || featureTypeIdIsSTRING(feature_type_id)) {
						feature_type = "string";
						valstring = pValue->toString();
					} else if (featureTypeIdIsSOM(feature_type_id)) {
						feature_type = "set_of_monads";
						valstring = pValue->toString();
					} else if (featureTypeIdIsENUM(feature_type_id)) {
						feature_type = "enum";
						enum_name = pFeature->getEnumName();
						valstring = pFeature->getEnumConstNameFromValue(pValue->getEnum());
					} else {
						ASSERT_THROW(false, "Unknown FEATURE_TYPE.");
					}
				}
  
				AttributePairList attribute_list;
				attribute_list.push_back(AttributePair("feature_name", feature_name));
				attribute_list.push_back(AttributePair("type", feature_type));
				if (!enum_name.empty()) {
					attribute_list.push_back(AttributePair("enum_name", enum_name));
				}
	
				pOut->newline();
				pOut->startTag("feature", attribute_list, false);
				pOut->outCharData(valstring);
				pOut->endTag("feature", false);
			}
			pOut->endTag("features", true);
		}

		// If we are printing as a full sheaf, and the inner sheaf is 
		// not the empty sheaf, we recurse into the inner sheaf.
		// Otherwise, if either we are not printing a full sheaf (but
		// a flat sheaf), or the inner sheaf is empty, we just print
		// the empty sheaf.
		if (bIsForFullSheaf && !sheafIsEmpty()) {
			m_sheaf->printXML(pOut);
		} else {
			MQLExecEnv::m_EmptySheaf.printXML(pOut);
		}
		pOut->endTag("matched_object", true);
	}
}


/** Print ourselves as JSON.
 *@internal
 *
 * Also recurses down into inner sheaf, but only if bIsForFullSheaf is
 * \p true.
 *
 * @param pOut the EMdFOutput object to print to.
 *
 * @param bIsForFullSheaf Should be true if we are printing this as a
 * full sheaf, false if this is a flat sheaf.
 */
void MatchedObject::printJSON(EMdFOutput* pOut, bool bIsForFullSheaf) const
{
	if (!getRetrieve()) {
		// Nothing to do
	} else if (isID_M()) {
        pOut->jsonStartStruct();

        pOut->jsonLabel("object_type_name");
        pOut->jsonOutValue("pow_m");

        pOut->jsonLabel("focus");
        pOut->jsonOutValue(getFocus());

		std::string marks(getMarksString());
		if (!marks.empty()) {
            pOut->jsonLabel("marks");
            pOut->jsonOutValue(getMarksString());
		}

        pOut->jsonLabel("id_d");
        pOut->jsonOutValue();

        pOut->jsonLabel("monad_set");
		if (m_last < 0) {
			m_u.pMonad_ms->printJSON(pOut);
		} else {
			getMonads().printJSON(pOut);
		}

		if (!sheafIsEmpty()) {
			m_sheaf->printJSON(pOut);
		} else {
			MQLExecEnv::m_EmptySheaf.printJSON(pOut);
		}
        pOut->jsonEndStruct();
	} else {
        pOut->jsonStartStruct();

        pOut->jsonLabel("object_type_name");
        pOut->jsonOutValue(m_pObjectBlock->getObjectTypeName());

        pOut->jsonLabel("focus");
        pOut->jsonOutValue(getFocus());
        

		std::string marks(getMarksString());
		if (!marks.empty()) {
            pOut->jsonLabel("marks");
            pOut->jsonOutValue(getMarksString());
		}

        pOut->jsonLabel("id_d");
        pOut->jsonOutValueOrNull(getID_D());

        pOut->jsonLabel("monad_set");
		if (m_last < 0) {
			m_u.pMonad_ms->printJSON(pOut);
		} else {
			getMonads().printJSON(pOut);
		}

		if (isInstObject() || m_values.m_pValue_vec != 0) {
			pOut->jsonLabel("features");
			pOut->jsonStartArray();
			
			int size = m_pObjectBlock->getNoOfGETFeatures();
			for (int i = 0; i < size; ++i) {
				pOut->jsonStartStruct();
            
				// Get value
				const EMdFValue *pValue = getEMdFValue(i);

				// Get Feature
				Feature *pFeature = m_pObjectBlock->getFeatureRetrievalFeature(i);

				// Get feature name
				const std::string& feature_name = pFeature->getHumanReadableFeatureName();

				// Get feature type and valstring
				std::string feature_type;
				std::string enum_name;
				id_d_t feature_type_id = pFeature->getOutputFeatureTypeID();
				std::string valstring;

				if (featureTypeIdIsListOf(feature_type_id)) {
					const IntegerList *pIntegerList = pValue->getIntegerList();
					if (pIntegerList == 0) {
						valstring = "";
					} else {
						if (!featureTypeIdIsListOfENUM(feature_type_id)) {
							ASSERT_THROW(featureTypeIdIsListOfINTEGER(feature_type_id)
								     || featureTypeIdIsListOfID_D(feature_type_id),
								     "At this point, we only know how to do lists of integers and lists of id_ds. This is something else.");
							valstring = pIntegerList->getDelimitedString(DEFAULT_LIST_DELIMITER);
						} else {
							IntegerListConstIterator ci = pIntegerList->const_iterator();
							StringList mystringlist;
							while (ci.hasNext()) {
								// Get next enum value
								long cur_enum_value = ci.next();
								std::string enum_const_name = pFeature->getEnumConstNameFromValue(cur_enum_value);
								mystringlist.addStringBack(enum_const_name);
							}
							valstring = mystringlist.getDelimitedString(DEFAULT_LIST_DELIMITER);
						}
					}
					if (featureTypeIdIsListOfINTEGER(feature_type_id)) {
						feature_type = "list_of_integer";
					} else if (featureTypeIdIsListOfID_D(feature_type_id)) {
						feature_type = "list_of_id_d";
					} else if (featureTypeIdIsListOfENUM(feature_type_id)){
						feature_type = "list_of_enum";
						enum_name = pFeature->getEnumName();
					} else {
						ASSERT_THROW(false, "Unknown FEATURE_TYPE.");
					}
				} else {
					// not ListOf
					if (featureTypeIdIsINTEGER(feature_type_id)) {
						feature_type = "integer";
						valstring = pValue->toString();
					} else if (featureTypeIdIsID_D(feature_type_id)) {
						feature_type = "id_d";
						valstring = pValue->toString();
					} else if (featureTypeIdIsASCII(feature_type_id)
						   || featureTypeIdIsSTRING(feature_type_id)) {
						feature_type = "string";
						valstring = pValue->toString();
					} else if (featureTypeIdIsSOM(feature_type_id)) {
						feature_type = "set_of_monads";
						valstring = pValue->toString();
					} else if (featureTypeIdIsENUM(feature_type_id)) {
						feature_type = "enum";
						enum_name = pFeature->getEnumName();
						valstring = pFeature->getEnumConstNameFromValue(pValue->getEnum());
					} else {
						ASSERT_THROW(false, "Unknown FEATURE_TYPE.");
					}
				}
  
				pOut->jsonLabel("feature_name");
				pOut->jsonOutValue(feature_name);
				
				pOut->jsonLabel("feature_type");
				pOut->jsonOutValue(feature_type);
				
				if (!enum_name.empty()) {
					pOut->jsonLabel("enum_name");
					pOut->jsonOutValue(enum_name);
				}
				
				pOut->jsonLabel("feature_value");
				pOut->jsonOutValue(valstring);
				
				pOut->jsonEndStruct();
			}
			pOut->jsonEndArray();
		}
		
		// If we are printing as a full sheaf, and the inner sheaf is 
		// not the empty sheaf, we recurse into the inner sheaf.
		// Otherwise, if either we are not printing a full sheaf (but
		// a flat sheaf), or the inner sheaf is empty, we just print
		// the empty sheaf.

		if (bIsForFullSheaf && !sheafIsEmpty()) {
			m_sheaf->printJSON(pOut);
		} else {
			MQLExecEnv::m_EmptySheaf.printJSON(pOut);
		}

		pOut->jsonEndStruct();
	}
}


/////////////////////////////////////////////////////////////////
//
// Straw
//
/////////////////////////////////////////////////////////////////
/** Constructor.
 */
Straw::Straw() : m_last(0), m_list()
{
}

/** Copy constructor.
 */
Straw::Straw(const Straw& other) : m_last(0), m_list()
{
	copyOther(other);
}

/** Destructor.
 *
 * Deletes all the straws. Calls #deleteMos().
 */
Straw::~Straw()
{
	deleteMos();
}

/** Get a StrawConstIterator pointing to the first matched object.
 *
 * @return A StrawConstIterator pointing to the first matched object.
 */
StrawConstIterator Straw::const_iterator() const
{
	return StrawConstIterator(&m_list);
}

/** Copies and appends all of other's matched objects.
 *
 * @param other The other Straw to copy.
 */
void Straw::copyOther(const Straw& other)
{
	m_last = other.m_last;
	StrawConstIterator oi = other.const_iterator();
	while (oi.hasNext()) {
		m_list.push_back(copyMO((oi.current())));
		oi.next();
	}
}

/** Delete all matched objects and make the list of matched objects empty.
 */
void Straw::deleteMos(void)
{
	if (!m_list.empty()) {
		m_list.clear(true);
	}
}

/** Append a matched object to the list of matched objects.
 */
void Straw::append(MatchedObject* mo)
{
	m_list.push_back(mo);
	updateLast(mo);
}

/** Update Straw::m_last with mo's last monad.
 *
 * Sets m_last to be mo's last monad if and only if mo's last monad is
 * greater than the current m_last.
 *
 * @param mo A pointer to the MatchedObject to use.
 */
void Straw::updateLast(const MatchedObject* mo) 
{
	monad_m mo_last = mo->getLast();
	if (mo_last > m_last)
		m_last = mo_last;
}

/** Print on the console.
 *
 * Prints each matched object with the boolean parameter set to
 * signify a full sheaf (see MatchedObject::printConsole() for an
 * explanation of what this means).
 *
 * @param pOut A pointer to the EMdFOutput that must be printed on.
 */
void Straw::printConsole(EMdFOutput *pOut) const
{
	pOut->out(" < ");
	StrawConstIterator ci = const_iterator();
	while (ci.hasNext()) {
		// Output matched_object
		(ci.current())->printConsole(pOut, true);
		ci.next();
	}
	pOut->out(" > \n");
}

/** Print as XML.
 *
 * Prints each matched object with the boolean parameter set to
 * signify a full sheaf (see MatchedObject::printXML() for an
 * explanation of what this means).
 *
 * @param pOut A pointer to the EMdFOutput that must be printed on.
 */
void Straw::printXML(EMdFOutput* pOut) const
{
	pOut->startTag("straw", true);
	StrawConstIterator ci = const_iterator();
	while (ci.hasNext()) {
		// Output matched_object
		(ci.current())->printXML(pOut, true);
		ci.next();
	}
	pOut->endTag("straw", true);
}

/** Print as JSON.
 *
 * Prints each matched object with the boolean parameter set to
 * signify a full sheaf (see MatchedObject::printXML() for an
 * explanation of what this means).
 *
 * @param pOut A pointer to the EMdFOutput that must be printed on.
 */
void Straw::printJSON(EMdFOutput* pOut) const
{
    pOut->jsonStartStruct();
    pOut->jsonLabel("matched_objects");
	pOut->jsonStartArray();

	StrawConstIterator ci = const_iterator();
	while (ci.hasNext()) {
		// Output matched_object
		(ci.current())->printJSON(pOut, true);
		ci.next();
	}
	pOut->jsonEndArray();
	pOut->jsonEndStruct();
}

/** Assignment operator.
 *
 * This is not SWIG-wrapped.
 *
 * @param other The Straw to assign from.
 *
 * @return A reference to ourselves. 
 */
Straw& Straw::operator=(const Straw& other)
{
	deleteMos();
	copyOther(other);
	return *this;
}

/** Prepend another straw to our list.
 *@internal
 *
 * Prepend pOther to our list, taking over the matched objects. Then
 * delete pOther.
 *
 *@see Straw::prepend(const Straw& other)
 *
 *@param pOther The other sheaf to prepend and take over.
 *
 */
void Straw::prependAndTakeOver(Straw *pOther)
{
	// Prepend other to my own list,
	// and set other's list to empty.
	pOther->m_list.prependWithMe(&m_list);

	// We are to take it over, so we delete it.
	delete pOther;
}

/** Prepend other's matched objects, but do so by copying.
 *
 * @see Straw::prependAndTakeOver(Straw *pOther)
 *
 * @param other The other straw to prepend.
 */
void Straw::prepend(const Straw& other)
{
	StrawConstIterator ci(&other.m_list);
	if (ci.hasNext()) {
		// Add the first one
		const MatchedObject* pMO = ci.next();
		m_list.prepend(copyMO(pMO));
    
		// Are there any more?
		Lnode<MatchedObject> *pIterator;
		pIterator = m_list.getRoot();
		while (ci.hasNext()) {
			pMO = ci.next();
			pIterator = m_list.insertAfter(pIterator, copyMO(pMO));
		}
	}
}

/** Flatten.
 *@internal
 *
 * Add matched objects to the object-type-straws of pFlatSheaf.
 *
 * @see Sheaf::flatten(), MatchedObject::flatten().
 *
 * @param pFlatSheaf The flat sheaf to add to.
 */ 
void Straw::flatten(FlatSheaf *pFlatSheaf)
{
	StrawConstIterator ci = const_iterator();
	while (ci.hasNext()) {
		MatchedObject *pMO = const_cast<MatchedObject*>(ci.next());
		// Flatten matched object, and delete it if it was not
		// added to the flat sheaf.
		if (!pMO->flatten(pFlatSheaf)) {
			delete pMO;
		}
	}
	m_list.clear(false);
}


/** Count objects recursively.
 *
 * @param bUseOnlyFocusObjects If true, only objects which
 * have the focus boolean set to true will be counted.
 *
 * @return The number of (possibly focus) objects in the
 * MatchedObject's of this straw, and any embedded Sheaves
 * therein.
 */
long Straw::countObjects(bool bUseOnlyFocusObjects) const
{
	long result = 0;
	
	StrawConstIterator ci = const_iterator();
	while (ci.hasNext()) {
		result += (ci.next())->countObjects(bUseOnlyFocusObjects);
	}
	
	return result;
}
	

/** Count objects recursively.
 *
 *
 * If object_type_name is empty, all object types are counted.
 *
 * If object_type_name is "pow_m", only gap_blocks and opt_gap_blocks
 * are counted.
 *
 * If object_type_name is neither empty nor "pow_m", only objects of the
 * given object type are counted.
 *
 *
 * @param object_type_name A string giving the normalized
 * object type name of the objects to count.  This should have
 * been passed through normalizeOTName() before calling this
 * method.
 *
 * @param bUseOnlyFocusObjects If true, only objects of the
 * given object type which have the focus boolean set to true
 * will be counted.
 *
 * @return The number of (possibly focus) objects in the
 * MatchedObject's in this straw, and any embedded Sheaves
 * therein.
 */
long Straw::countObjectsFromObjectType(const std::string& object_type_name, bool bUseOnlyFocusObjects) const
{
	long result = 0;
	
	StrawConstIterator ci = const_iterator();
	while (ci.hasNext()) {
		result += (ci.next())->countObjectsFromObjectType(object_type_name, bUseOnlyFocusObjects);
	}
	return result;
}
	
/** Count straws recursively.
 *
 * @return The 1 + the number of straws inside the embedded
 * sheaves in its MatchedObject's.
 */
long Straw::countStraws() const
{
	long result = 1;
	
	StrawConstIterator ci = const_iterator();
	while (ci.hasNext()) {
		result += (ci.next())->countStraws();
	}
	return result;
}


/** Get a set of monads from a sheaf.
 *
 * This process is explained under Sheaf::getSOM().  The set of monads
 * is gathered up in \p som.
 *
 * The \p bUseOnlyFocusObjects boolean parameter should be set to
 * \p true if you only want matched objects with the focus
 * boolean set to true to be used.
 *
 * Iterates over all matched objects in the straw.
 *
 * @param som The set of monads to use for gathering up the relevant
 * matched objects.
 * 
 * @param bUseOnlyFocusObjects Should be \p true if you only wish to
 * use matched objects with the focus boolean set to \p true; \p false
 * if you wish to use all matched objects in the sheaf.
 */
void Straw::getSOM(SetOfMonads& som, bool bUseOnlyFocusObjects) const
{
	StrawConstIterator ci = const_iterator();
	while (ci.hasNext()) {
		(ci.current())->getSOM(som, bUseOnlyFocusObjects);
		ci.next();
	}
}

void Straw::harvestMarks(String2SOMMap& result, bool bUseSingleMarks) const
{
	StrawConstIterator ci = const_iterator();
	while (ci.hasNext()) {
		(ci.next())->harvestMarks(result, bUseSingleMarks);
	}
}


/////////////////////////////////////////////////////////////////
//
// ListOfStraws
//
/////////////////////////////////////////////////////////////////
/** Constructor for empty ListOfStraws.
 *@internal
 */
ListOfStraws::ListOfStraws() 
{
}

/** Copy constructor for ListOfStraws.
 *@internal
 */
ListOfStraws::ListOfStraws(const ListOfStraws& other)
{
	copyOther(other);
}

/** Destructor.
 *@internal
 */
ListOfStraws::~ListOfStraws()
{
	deleteStraws();
}

/** Get SheafIterator over ourselves, pointing to the first straw.
 *@internal
 */
SheafIterator ListOfStraws::iterator() 
{
	return SheafIterator(&m_list);
}

/** Get SheafConstIterator over ourselves, pointing to the first straw.
 *@internal
 */
SheafConstIterator ListOfStraws::const_iterator() const
{
	return SheafConstIterator(&m_list);
}

/** Push the straw at the back of the list.
 *@internal
 *
 * @param straw The straw to push on the end of the list.
 */
void ListOfStraws::append(Straw* straw)
{
	m_list.push_back(straw);
}

/** Append pOther and delete it, without deleting its straws.
 *@internal
 *
 * NOTE: Do not do anything with the pOther pointer after
 * calling this method!
 *
 * @param pOther The other ListOfStraws to append and subsume.
 */
void ListOfStraws::appendAndSubsume(ListOfStraws* pOther)
{
	// Append other's list to my list,
	// and set pOther's list to empty.
	pOther->m_list.extendWithMe(&m_list);

	// Now delete other
	delete pOther;
}


/** Append copies of the straws in pOther. 
 *@internal
 *
 * Append copies of the straws in pOther. Leave the original straws
 * and pOther alone.
 *
 * @param pOther the other ListOfStraws to copy straws from.
 */
void ListOfStraws::appendACopy(const ListOfStraws *pOther)
{
	SheafConstIterator oi = pOther->const_iterator();
	while (oi.hasNext()) {
		m_list.push_back(new Straw(*(oi.current())));
		oi.next();
	}
}




/** Append and subsume pOther, filtering out unwanted straws and
 * MatchedObject objects.
 *@internal
 *
 * Appends pOther, taking its straws away from it, and appending its
 * straws to ourselves, while removing "unwanted straws" and "unwanted
 * MatchedObject" objects.
 *
 * An "unwanted MatchedObject" is one that is not to be retrieved.
 * These are removed from the straws.
 *
 * An "unwanted straw" is one that is empty after all "unwanted
 * MatchedObject" objects have been removed.
 *
 * NOTE: DO NOT do anything with the pOther pointer after this method
 * has been called: It has been deleted!
 *
 * @param pOther The other ListOfStraws to append and subsume.
 *
 * @return The number of Straws added.
 */
long ListOfStraws::appendAndSubsumeWhileFiltering(ListOfStraws* pOther)
{
	long straw_count = 0;
	SheafConstIterator other_ci = pOther->const_iterator();
	while (other_ci.hasNext()) {
		Straw *pNewStraw = new Straw();
		const Straw *pOtherStraw = other_ci.next();

		StrawConstIterator other_straw_ci = pOtherStraw->const_iterator();
		while (other_straw_ci.hasNext()) {
			const MatchedObject *pOtherMO = other_straw_ci.next();
			if (pOtherMO->getRetrieve()) {
				pNewStraw->append(copyMO(pOtherMO));
			}
		}

		// Only append if it is not empty.
		if (pNewStraw->isEmpty()) {
			delete pNewStraw;
		} else {
			this->append(pNewStraw);
			++straw_count;
		}
	}

	// Now delete other
	delete pOther;

	return straw_count;
}


/** Append and subsume pOther, starting at a given index and adding
 *only those that are within a given count.
 *  @internal
 *
 * Appends pOther, taking its straws away from it, and appending its
 * straws to ourselves, while removing "unwanted straws" and "unwanted
 * MatchedObject" objects, "unwanted" meaning 'being outside of a
 * given range'
 *
 * NOTE: DO NOT do anything with the pOther pointer after this method
 * has been called: It has been deleted!
 *
 * @param pOther The other ListOfStraws to append and subsume.
 *
 * @return The number of Straws added.
 */
long ListOfStraws::appendAndSubsumeWhileCounting(ListOfStraws* pOther, long start_index, long count)
{
	long straw_count = 0;
	long straw_index = 0;
	SheafConstIterator other_ci = pOther->const_iterator();
	while (other_ci.hasNext()) {
		const Straw *pOtherStraw = other_ci.next();
		Straw *pNewStraw = new Straw(*pOtherStraw);

		if (straw_index >= start_index 
		    && straw_count <= count) {
			this->append(pNewStraw);
			++straw_count;
		}
		++straw_index;
		if (straw_index > start_index + count) {
			break;
		}
	}

	// Now delete other
	delete pOther;

	return straw_count;
}


/** Point on the console as console-style.
 * @internal
 *
 * @param pOut the EMdFOutput to print to.
 */
void ListOfStraws::printConsole(EMdFOutput *pOut, bool bPrintStartAndEnd) const
{
	if (bPrintStartAndEnd) {
		pOut->out(" < ");
	}
	SheafConstIterator ci = const_iterator();
	while (ci.hasNext()) {
		// Output straw
		(ci.current())->printConsole(pOut);
		ci.next();
	}
	if (bPrintStartAndEnd) {
		pOut->out(" > \n");
	}
}

void ListOfStraws::printLOSStartConsole(EMdFOutput *pOut)
{
	pOut->out(" < ");
}

void ListOfStraws::printLOSEndConsole(EMdFOutput *pOut)
{
	pOut->out(" > \n");
}

/** Point on the console as XML-style.
 * @internal
 *
 * @param pOut the EMdFOutput to print to.
 */
void ListOfStraws::printXML(EMdFOutput* pOut) const
{
	SheafConstIterator ci = const_iterator();
	while (ci.hasNext()) {
		// Output straw
		(ci.current())->printXML(pOut);
		ci.next();
	}
}

/** Point on the console as JSON-style.
 * @internal
 *
 * @param pOut the EMdFOutput to print to.
 */
void ListOfStraws::printJSON(EMdFOutput* pOut) const
{
	SheafConstIterator ci = const_iterator();
	while (ci.hasNext()) {
		// Output straw
		(ci.current())->printJSON(pOut);
		ci.next();
	}
}

/** Assignment operator.
 *@internal
 *
 * @param other The other ListOfStraws to copy straws from.
 *
 * @return ourselves.
 */
ListOfStraws& ListOfStraws::operator=(const ListOfStraws& other)
{
	deleteStraws();
	copyOther(other);
	return *this;
}

/** Copy method.
 *@internal
 * Copies the state of the other ListOfStraws into ourselves.
 *
 * @param other the other ListOfStraws from which to copy the contents.
 */
void ListOfStraws::copyOther(const ListOfStraws& other)
{
	SheafConstIterator oi = other.const_iterator();
	while (oi.hasNext()) {
		m_list.push_back(new Straw(*(oi.current())));
		oi.next();
	}
}

/** Delete all straws. 
 *@internal
 */
void ListOfStraws::deleteStraws(void)
{
	if (!m_list.empty()) {
		m_list.clear(true);
	}
}

/** Prepend a straw s onto all straws in ourselves.
 *@internal
 *
 * Only makes a copy of s as many times as is needed, then deletes
 * it. If there is only one with which to join it, we prepend and take
 * it over.
 *
 * NOTE: Also "takes over" the straw.  Therefore, DO NOT do anything
 * with the straw pointer s afterwards.
 *
 * @param s The straw to prepend to all straws in ourselves.
 *
 */
void ListOfStraws::joinAndTakeOver(Straw* s)
{
	if (m_list.empty()) {
		m_list.push_back(s);
	} else {
		if (m_list.lengthIs1()) {
			// Get first straw in list
			Straw *pStraw = m_list.iterator().current();
			pStraw->prependAndTakeOver(s);
		} else {
			SheafIterator ci = m_list.iterator();
			while (ci.hasNext()) {
				Straw* pStraw = ci.next();
				pStraw->prepend(*s);
			}
		}
	}
}


//
// Always makes a copy of s
/** Prepend a copy of a straw to all straws in ourselves. 
 *@internal
 *
 * Prepend a copy of a straw to all straws in ourselves. Do nothing
 * to the straw itself.
 *
 * This is the same as joinAndTakeOver(Straw*), except that this
 * always makes a copy of the straw, and does nothing to the straw
 * (i.e., does not subsume or delete it!).
 *
 * @see ListOfStraws::joinAndTakeOver(Straw*)
 *
 * @param mys The straw to prepend to each of the straws in ourselves.
 *
 */
void ListOfStraws::join(const Straw* mys)
{
	if (m_list.empty()) {
		m_list.push_back(new Straw(*mys));
	} else {
		if (m_list.lengthIs1()) {
			// Get first straw in list
			Straw *pStraw = m_list.iterator().current();
			pStraw->prepend(*mys);
		} else {
			SheafIterator ci = m_list.iterator();
			while (ci.hasNext()) {
				Straw* pStraw = ci.next();
				pStraw->prepend(*mys);
			}
		}
	}
}


/////////////////////////////////////////////////////////////////
//
// Sheaf
//
/////////////////////////////////////////////////////////////////
// fail
/** Constructor.
 *@internal
 *
 * @param isFail Should be set to true if this is a failed sheaf.
 * Otherwise, the sheaf will be an empty (but not failed) sheaf.
 *
 * The invariant is: if m_pList is nil (0), it is a failed sheaf;
 * otherwise, it is not a failed sheaf.
 *
 */
Sheaf::Sheaf(bool isFail)
{
	if (isFail) {
		m_plist = 0;
	} else {
		m_plist = new ListOfStraws();
	}
	m_pOBBVec = 0;
}

/** Constructor.
 *@internal
 *
 * @param plist The inner ListOfStraws will be set to plist.
 */
Sheaf::Sheaf(ListOfStraws* plist)
{
	m_plist = plist;
	m_pOBBVec = 0;
}

/** Copy constructor.
 */
Sheaf::Sheaf(const Sheaf& other)
{
	copyOther(other);
}

/** Destructor.
 */
Sheaf::~Sheaf()
{
	delete m_plist;
	if (m_pOBBVec != 0) {
		for (OBBVec::size_type i = 0; i < m_pOBBVec->size(); ++i) {
			delete ((*m_pOBBVec)[i]);
		}
		delete m_pOBBVec;
	}
}

/** Get a SheafIterator pointing to the first straw (if any).
 *
 * If the sheaf is a failed sheaf, the SheafIterator will return false
 * on hasNext().
 *
 * @return A SheafIterator pointing to the first straw (if any).
 *
 */
SheafIterator Sheaf::iterator()
{
	if (m_plist == 0) {
		return SheafIterator(0);
	} else {
		return m_plist->iterator();
	}
}

/** Get a SheafConstIterator pointing to the first straw.
 *
 * If the sheaf is a failed sheaf, the SheafIterator will return false
 * on hasNext().
 * 
 * @return A SheafConstIterator pointing to the first straw (if any).
 *
 */
SheafConstIterator Sheaf::const_iterator() const
{
	if (m_plist == 0) {
		return SheafConstIterator(0);
	} else {
		return m_plist->const_iterator();
	}
}

/** Copy other sheaf.
 *@internal
 *
 * @param other The other sheaf to copy.
 */
void Sheaf::copyOther(const Sheaf& other)
{
	if (other.m_plist == 0) {
		m_plist = 0;
	} else {
		m_plist = new ListOfStraws(*other.m_plist);
	}
	ASSERT_THROW(other.m_pOBBVec == 0, "other.m_pOBBVec was non-0");
	m_pOBBVec = 0;
}

/** Delete all straws.
 */
void Sheaf::deleteStraws(void)
{
	delete m_plist;
}

/** Is this sheaf a failed sheaf?
 *
 * @return \p true if this is a failed sheaf, \p false otherwise.
 */
bool Sheaf::isFail(void) const
{
	return m_plist == 0;
}

/** Assignment operator.
 *@internal
 *
 * The straws are copied, using Sheaf::copyOther().
 *
 * @param other The other sheaf to assign from.
 *
 * @return A reference to ourselves.
 */
Sheaf& Sheaf::operator=(const Sheaf& other)
{
	deleteStraws();
	delete m_pOBBVec;
	copyOther(other);
	return *this;
}

/** Print as console output.
 *
 * @param pOut the EMdFOutput to print on.
 */
void Sheaf::printConsole(EMdFOutput *pOut) const
{
	if (isFail()) {
		pOut->out(" // \n");
	} else {
		if (m_plist->isEmpty()) {
			pOut->out(" //  <  > \n");
		} else {
			pOut->out(" // ");
			m_plist->printConsole(pOut);
		}
	}
}

/** Print the start of a sheaf as console output.
 *
 * @param pOut the EMdFOutput to print on.
 */
void Sheaf::printSheafStartConsole(EMdFOutput *pOut)
{
	pOut->out(" // ");
}

/** Print as XML output.
 *
 * @param pOut the EMdFOutput to print on.
 */
void Sheaf::printXML(EMdFOutput* pOut) const
{
	if (isFail()) {
		// Do nothing
	} else {
		pOut->startTag("sheaf", true);
		m_plist->printXML(pOut);
		pOut->endTag("sheaf", true);
	}
}

/** Print start of sheaf as XML output.
 *
 * @param pOut the EMdFOutput to print on.
 */
void Sheaf::printSheafStartXML(EMdFOutput* pOut)
{
	pOut->startTag("sheaf", true);
}

/** Print end of sheaf as XML output.
 *
 * @param pOut the EMdFOutput to print on.
 */
void Sheaf::printSheafEndXML(EMdFOutput* pOut)
{
	pOut->endTag("sheaf", true);
}

/** Print XML DTD for sheaf, straw, and matched object.
 *
 * @param pOut The EMdFOutput to print the DTD on.
 */
void Sheaf::printDTD(EMdFOutput* pOut) 
{
	pOut->out("  <!ELEMENT sheaf (straw)* >"); 
	pOut->newline();
	pOut->out("  <!ELEMENT straw (matched_object)* >"); 
	pOut->newline();
	pOut->out("  <!ELEMENT matched_object (monad_set, (features)?, (sheaf)?) >"); 
	pOut->newline();
	pOut->out("  <!ELEMENT monad_set (mse)+ >"); 
	pOut->newline();
	pOut->out("  <!ELEMENT mse EMPTY>"); 
	pOut->newline();
	pOut->out("  <!ELEMENT features (feature)+ >"); 
	pOut->newline();
	pOut->out("  <!ELEMENT feature (#PCDATA) >"); 
	pOut->newline();
	pOut->out("  <!ATTLIST sheaf >"); 
	pOut->newline();
	pOut->out("  <!ATTLIST straw >"); 
	pOut->newline();
	pOut->out("  <!ATTLIST matched_object "); 
	pOut->newline();
	pOut->out("            object_type_name CDATA #REQUIRED "); 
	pOut->newline();
	pOut->out("            focus (false | true) #REQUIRED"); 
	pOut->newline();
	pOut->out("            marks (CDATA) #IMPLIED"); 
	pOut->newline();
	pOut->out("            id_d CDATA \"nil\""); 
	pOut->newline();
	pOut->out("  >"); 
	pOut->newline();
	pOut->out("  <!ATTLIST monad_set >"); 
	pOut->newline();
	pOut->out("  <!ATTLIST mse"); 
	pOut->newline();
	pOut->out("            first CDATA #REQUIRED"); 
	pOut->newline();
	pOut->out("            last  CDATA #REQUIRED"); 
	pOut->newline();
	pOut->out("  >"); 
	pOut->newline();
	pOut->out("  <!ATTLIST feature"); 
	pOut->newline();
	pOut->out("            feature_name CDATA #REQUIRED"); 
	pOut->newline();
	pOut->out("            feature_type (string|integer|id_d|enum|list_of_integer|list_of_id_d|list_of_enum) #REQUIRED"); 
	pOut->newline();
	pOut->out("            enum_type CDATA #IMPLIED "); 
	pOut->newline();
	pOut->out("  >"); 
	pOut->newline();
}

/** Print as XML output.
 *
 * @param pOut the EMdFOutput to print on.
 */
void Sheaf::printJSON(EMdFOutput* pOut) const
{
	if (isFail()) {
		// Do nothing
	} else {
		if (!m_plist->isEmpty()) {
			pOut->jsonLabel("sheaf");
			pOut->jsonStartStruct();

			pOut->jsonLabel("straws");
			pOut->jsonStartArray();
			m_plist->printJSON(pOut);
			pOut->jsonEndArray();

			pOut->jsonEndStruct();
		}
	}
}

/** Print start of sheaf as JSON output.
 *
 * @param pOut the EMdFOutput to print on.
 */
void Sheaf::printSheafStartJSON(EMdFOutput* pOut, bool hasLOS)
{
    pOut->jsonLabel("sheaf");
    pOut->jsonStartStruct();
    if (hasLOS) {
        pOut->jsonLabel("straws");
        pOut->jsonStartArray();
    }
}

/** Print end of sheaf as JSON output.
 *
 * @param pOut the EMdFOutput to print on.
 */
void Sheaf::printSheafEndJSON(EMdFOutput* pOut, bool hasLOS)
{
    if (hasLOS) {
        pOut->jsonEndArray();
    }
    pOut->jsonEndStruct();

}

/** Flatten the sheaf.
 *@internal
 *
 * @param pFlatSheaf The flat sheaf to add our matched objects to.
 */
void Sheaf::flatten(FlatSheaf *pFlatSheaf)
{
	if (isFail()) {
		pFlatSheaf->setIsFail(true);
	} else {
		SheafConstIterator ci = const_iterator();
		while (ci.hasNext()) {
			Straw *pStraw = const_cast<Straw*>(ci.next());
			pStraw->flatten(pFlatSheaf);
		}
		m_plist->deleteStraws();
	}
}


/** Count objects recursively.
 *
 * @param bUseOnlyFocusObjects If true, only objects which
 * have the focus boolean set to true will be counted.
 *
 * @return The number of (possibly focus) objects in the
 * MatchedObject's of this straw, and any embedded Sheaves
 * therein.
 */
long Sheaf::countObjects(bool bUseOnlyFocusObjects) const
{
	long result = 0;

	SheafConstIterator ci = const_iterator();
	while (ci.hasNext()) {
		result += (ci.next())->countObjects(bUseOnlyFocusObjects);
	}
	
	return result;
}

/** Count objects recursively.
 *
 * If object_type_name is empty, all object types are counted.
 *
 * If object_type_name is "pow_m", only gap_blocks and opt_gap_blocks
 * are counted.
 *
 * If object_type_name is neither empty nor "pow_m", only objects of the
 * given object type are counted.
 *
 *
 * @param object_type_name A string giving the normalized
 * object type name of the objects to count.  This should have
 * been passed through normalizeOTName() before calling this
 * method.
 *
 * @param bUseOnlyFocusObjects If true, only objects of the
 * given object type which have the focus boolean set to true
 * will be counted.
 *
 * @return The number of (possibly focus) objects in the
 * MatchedObject's in this straw, and any embedded Sheaves
 * therein.
 */
long Sheaf::countObjectsFromObjectType(const std::string& object_type_name, bool bUseOnlyFocusObjects) const
{
	long result = 0;

	SheafConstIterator ci = const_iterator();
	while (ci.hasNext()) {
		result += (ci.next())->countObjectsFromObjectType(object_type_name, bUseOnlyFocusObjects);
	}

	return result;
}
	
/** Count straws recursively.
 *
 * @return The number of straws inside the sheaf, recursively.
 */
long Sheaf::countStraws() const
{
	long result = 0;
	SheafConstIterator ci = const_iterator();
	while (ci.hasNext()) {
		result += (ci.next())->countStraws();
	}
	return result;
}
	

/** Get a set of monads from a sheaf.
 *
 * This procedure gets the set of monads which is the union of all
 * relevant matched objects in the sheaf.  If the \p
 * bUseOnlyFocusObjects boolean parameter is set to \p true, then only
 * matched objects with their focus objects set to true are relevant,
 * and thus used in gathering up the monads of the sheaf.  If the \p
 * bUseOnlyFocusObjects parameter is set to false, then all matched
 * objects in the sheaf are relevant and thus used.
 *
 * The process visits all matched objects in the sheaf.  This is done
 * as follows: This sheaf iterates over all its straws, calling
 * Straw::getSOM() for all of its straws.  Each straw, in turn,
 * iterates over all its MatchedObject's, calling getSOM() on each
 * MatchedObject.  If this MatchedObject is relevant (as explained
 * above), its monads will be added to the \p som parameter by means
 * of SetOfMonads::unionWith().  If the matched object's inner sheaf
 * is not empty, then that sheaf's Sheaf::getSOM() method will be
 * called, thus recursing down into the inner sheaf.
 *
 * @param som The set of monads to use for gathering up the relevant
 * matched objects.
 *
 * @param bUseOnlyFocusObjects Should be \p true if you only wish to
 * use matched objects with the focus boolean set to \p true; \p false
 * if you wish to use all matched objects in the sheaf.
 */
void Sheaf::getSOM(SetOfMonads& som, bool bUseOnlyFocusObjects) const
{
	SheafConstIterator ci = const_iterator();
	while (ci.hasNext()) {
		(ci.current())->getSOM(som, bUseOnlyFocusObjects);
		ci.next();
	}
}

/** Convenience function to get SOM of sheaf as a return type.
 *
 * Constructs a SetOfMonads on the stack, calls
 * Sheaf::getSOM(som,bUseOnlyFocusObjects), and returns result.
 *
 * @see Sheaf::getSOM(som,bUseOnlyFocusObjects)
 *
 * @param bUseOnlyFocusObjects Set this to \p true if you only want
 * the monads of focus-marked matched objects to be added to the SOM.
 *
 * @return The SetOfMonads asked for.
 */
SetOfMonads Sheaf::getSOM(bool bUseOnlyFocusObjects) const
{
	SetOfMonads result;
	getSOM(result, bUseOnlyFocusObjects);
	return result;
}

void Sheaf::harvestMarks(String2SOMMap& result, bool bUseSingleMarks) const
{
	SheafConstIterator ci = const_iterator();
	while (ci.hasNext()) {
		(ci.next())->harvestMarks(result, bUseSingleMarks);
	}
}


void Sheaf::appendAndSubsume(Sheaf *pOther)
{
	// Append other's list to my list,
	// and set pOther's list to empty.
	m_plist->appendAndSubsume(pOther->m_plist);

	// Now make sure we don't delete it accidentally, 
	// since it has already been deleted...
	pOther->m_plist = 0;

	// Now delete other
	delete pOther;
}



/////////////////////////////////////////////////////////////////
//
// FlatStrawConstIterator
//
/////////////////////////////////////////////////////////////////

/** Constructor for empty FlatStrawConstIterator which does
 *  not point to any Arena or Inst.
 */
FlatStrawConstIterator::FlatStrawConstIterator()
	: m_pMotherFlatStraw(0),
	  m_pArenaIt(0),
	  m_pPrevMO(0),
	  m_pCurMO(0)
{
}

/** Constructor which points to the beginning fo a FlatStraw.
 */
FlatStrawConstIterator::FlatStrawConstIterator(const FlatStraw *pMotherFlatStraw)
	: m_pMotherFlatStraw(pMotherFlatStraw),
	  m_pArenaIt(0),
	  m_pPrevMO(0),
	  m_pCurMO(0)
{
	if (pMotherFlatStraw->m_pArena != 0) {
		m_pArenaIt = new ArenaConstIterator(sizeof(MatchedObject*), pMotherFlatStraw->m_pArena);
	} else if (pMotherFlatStraw->m_pInst != 0) {
		SetOfMonads Universe = SetOfMonads(1, MAX_MONAD);
		pMotherFlatStraw->m_pInst->set_current_universe(Universe);
		m_InstIt = pMotherFlatStraw->m_pInst->begin();
	} 
}

/** Copy constructor. 
 * 
 * @param other The other FlatStrawConstIterator from which to
 * initialize.
 */
FlatStrawConstIterator::FlatStrawConstIterator(const FlatStrawConstIterator& other)
{
	assign(other);
}

/** Destructor. 
 */
FlatStrawConstIterator::~FlatStrawConstIterator()
{
	if (m_pArenaIt != 0) {
		delete m_pArenaIt;
	} else {
		delete m_pPrevMO;
		delete m_pCurMO;
	}
}

/** Test whether we have hit the end.
 * 
 * @return true iff we have not hit the end, i.e., there is at least one more left in the Arena or Inst
 */
bool FlatStrawConstIterator::hasNext() const
{
	if (m_pMotherFlatStraw == 0) {
		return false;
	} else {
		if (m_pArenaIt != 0) {
			return m_pArenaIt->hasNext();
		} else {
			return m_InstIt.hasNext();
		}
	}
}

/** Get current MatchedObject and advance iterator.
 *
 * @see FlatStrawConstIterator::current()
 *
 * @return A pointer to the current MatchedObject.
 */
MatchedObject *FlatStrawConstIterator::next()
{
	if (m_pArenaIt != 0) {
		void *pMem = m_pArenaIt->next();
		return *((MatchedObject**)pMem);
	} else {
		deleteAndShiftToNext();
		return m_pCurMO;
	}
}

/** Get current MatchedObject but DO NOT advance the iterator.
 *
 * @see FlatStrawConstIterator::next()
 *
 * @return A pointer to the current MatchedObject.
 */
MatchedObject *FlatStrawConstIterator::current() // Gets current without altering iterator
{
	if (m_pArenaIt != 0) {
		void *pMem = m_pArenaIt->current();
		return *((MatchedObject**)pMem);
	} else {
		setCurMO(m_InstIt.current());
		return m_pCurMO;
	}
}
	
FlatStrawConstIterator& FlatStrawConstIterator::operator=(const FlatStrawConstIterator& other)
{
	assign(other);
	return *this;
}

void FlatStrawConstIterator::assign(const FlatStrawConstIterator& other)
{
	m_pMotherFlatStraw = other.m_pMotherFlatStraw;
	
	if (other.m_pArenaIt) {
		m_pArenaIt = new ArenaConstIterator(*other.m_pArenaIt);
		m_pPrevMO = 0;
		m_pCurMO = 0;
	} else {
		m_pArenaIt = 0;
		m_InstIt = Inst::const_iterator(other.m_InstIt);
		if (other.m_pPrevMO == 0) {
			m_pPrevMO = 0;
		} else {
			m_pPrevMO = new MatchedObject(*other.m_pPrevMO);
		}
		if (other.m_pCurMO == 0) {
			m_pCurMO = 0;
		} else {
			m_pCurMO = new MatchedObject(*other.m_pCurMO);
		}
	}
}

void FlatStrawConstIterator::setCurMO(const InstObject* pInstObj)
{
	m_pCurMO = new MatchedObject(pInstObj, m_pMotherFlatStraw->m_pOBB);
}


void FlatStrawConstIterator::deleteAndShiftToNext()
{
	delete m_pPrevMO;
	m_pPrevMO = m_pCurMO;
	
	const InstObject *pInstObj = m_InstIt.next();
	m_pCurMO = 0;
	setCurMO(pInstObj);
}
       




/////////////////////////////////////////////////////////////////
//
// FlatStraw
//
/////////////////////////////////////////////////////////////////

/**
 * Constructor for Arena-based FlatStraw
 *
 * @param object_type_name The object type name of the object type
 * behind the FlatStraw (may be "pow_m").
 */
FlatStraw::FlatStraw(const std::string& object_type_name)
	: m_pArena(0),
	  m_pInst(0),
	  m_pOBB(0),
	  m_object_type_name(object_type_name)
{
	m_pArena = new Arena();
}

/**
 * Constructor for Inst-based FlatStraw
 *
 * @param object_type_name The object type name of the object type
 * behind the FlatStraw (may NOT be "pow_m").
 */
FlatStraw::FlatStraw(const std::string& object_type_name, Inst *pInst, ObjectBlockBase *pOBB)
	: m_pArena(0),
	  m_pInst(pInst),
	  m_pOBB(pOBB),
	  m_object_type_name(object_type_name)
{
}


/** Destructor.
 */
FlatStraw::~FlatStraw()
{
	if (m_pArena != 0) {
		FlatStrawConstIterator ci = const_iterator();
		while (ci.hasNext()) {
			MatchedObject *pMO = ci.next();
			delete pMO;
		}
		delete m_pArena;
	} else {
		delete m_pInst;
	}
}


/** Get a FlatStrawConstIterator on this FlatStraw.
 *
 * @return A FlatStrawConstIterator pointing to the first MatchedObject
 * in ourselves (if any).
 */
FlatStrawConstIterator FlatStraw::const_iterator(void) const 
{
	return FlatStrawConstIterator(this); 
}


/** Print on the console, console-style.
 *
 * Prints the flat straw on the EMdFOutput parameter pOut,
 * console-style.
 *
 * @param pOut A pointer to the EMdFOutput that must be printed on.
 */
void FlatStraw::printConsole(EMdFOutput *pOut) const
{
	pOut->out(" < ");
	FlatStrawConstIterator ci = const_iterator();
	while (ci.hasNext()) {
		MatchedObject *pMO = ci.next();
		pMO->printConsole(pOut, false);
	}
	pOut->out(" > \n");
}

/** Print on the console, XML-style.
 *
 * Prints the flat straw on the EMdFOutput parameter pOut, XML-style.
 *
 * @param pOut A pointer to the EMdFOutput that must be printed on.
 */
void FlatStraw::printXML(EMdFOutput* pOut) const
{
	AttributePairList attribute_list;
	attribute_list.push_back(AttributePair("object_type_name", m_object_type_name));
	pOut->startTag("flatstraw", attribute_list, true);
	FlatStrawConstIterator ci = const_iterator();
	while (ci.hasNext()) {
		// Output matched_object
		MatchedObject *pMO = ci.next();
		pMO->printXML(pOut, false);
	}
	pOut->endTag("flatstraw", true);
}

/** Print on the console, JSON-style.
 *
 * Prints the flat straw on the EMdFOutput parameter pOut, JSON-style.
 *
 * @param pOut A pointer to the EMdFOutput that must be printed on.
 */
void FlatStraw::printJSON(EMdFOutput* pOut) const
{
    pOut->jsonStartStruct();

    pOut->jsonLabel("object_type_name");
    pOut->jsonOutValue(m_object_type_name);

    pOut->jsonLabel("matched_objects");
    pOut->jsonStartArray();

	FlatStrawConstIterator ci = const_iterator();
	while (ci.hasNext()) {
		// Output matched_object
		MatchedObject *pMO = ci.next();
		pMO->printJSON(pOut, false);
	}

    pOut->jsonEndArray();
    pOut->jsonEndStruct();
}


//////////////////////////////////////////////////
// 
// FlatSheaf
//
//////////////////////////////////////////////////

/** Conststructor for an empty FlatSheafConstIterator that does not
 * point anywhere.
 */
FlatSheafConstIterator::FlatSheafConstIterator()
{
	m_pMotherFlatSheaf = 0;
}


/** Constructor for a FlastSheafConstIterator that points to the first
 *  MatchedObject in a FlatSheaf.
 *
 * @param pMotherFlatSheaf A pointer to the FlatSheaf over which we
 * wish to iterate.
 */
FlatSheafConstIterator::FlatSheafConstIterator(const FlatSheaf *pMotherFlatSheaf)
{
	m_current = pMotherFlatSheaf->m_map.begin();
	m_pMotherFlatSheaf = pMotherFlatSheaf;
}


/** Copy constructor.
 *
 * @param other The other FlatsSheafConstIterator from which to get
 * our state.
 */
FlatSheafConstIterator::FlatSheafConstIterator(const FlatSheafConstIterator& other)
{
	assign(other);
}


/** Destructor.
 */
FlatSheafConstIterator::~FlatSheafConstIterator()
{
}


/** Test whether we have not hit the end.
 *
 * @return true iff we have not hit the end, i.e., true iff there is
 * at least one more FlatStraw in the FlatSheaf.
 */
bool FlatSheafConstIterator::hasNext() const
{
	bool bResult = m_current != m_pMotherFlatSheaf->m_map.end();
	return bResult;
}

/** Get current FlatStraw and advance the iterator.
 *
 * @return A pointer to the current FlatStraw.
 */
FlatStraw* FlatSheafConstIterator::next()
{
	ASSERT_THROW(hasNext(), "hasNext() was false");
	FlatStraw *pFlatStraw = m_current->second;
	++m_current;
	return pFlatStraw;
}

/** Get current FlatStraw but DO NOT advance the iterator.
 *
 * @return A pointer to the current FlatStraw.
 */
FlatStraw* FlatSheafConstIterator::current()
{
	ASSERT_THROW(hasNext(), "hasNext() was false");
	return m_current->second;
}

/** Assignment operator.
 *
 * @param other The FlatSheafConstIterator whose state to copy.
 *
 * @return ourselves.
 */
FlatSheafConstIterator& FlatSheafConstIterator::operator=(const FlatSheafConstIterator& other)
{
	assign(other);
	return *this;
}


/** Copy method.
 *
 * Copies the state of other into ourselves.
 *
 * @param other The other FlatSgeafConstIterator from which to get our
 * state.
 */
void FlatSheafConstIterator::assign(const FlatSheafConstIterator& other)
{
	m_current = other.m_current;
	m_pMotherFlatSheaf = other.m_pMotherFlatSheaf;
}



/////////////////////////////////////////////////////////////////
//
// FlatSheaf
//
/////////////////////////////////////////////////////////////////

/** Constructor.
 *@internal
 *
 * Is used when we wish to do "all" object types.
 *
 */
FlatSheaf::FlatSheaf() // For "all"
{
	m_bDoAll = true;
	m_bIsFail = false;
	m_pOBBVec = 0;
}


/** Constructor. When we wish to only get a selected number of object types.
 *
 * @param object_types A list of pairs of the object type id_ds and
 * object type names of the object types to get.
 */
FlatSheaf::FlatSheaf(const std::list<std::pair<id_d_t, std::string> >& object_types) // For only a select few
{
	m_bDoAll = false;
	m_bIsFail = false;
	std::list<std::pair<id_d_t, std::string> >::const_iterator ci = object_types.begin();
	std::list<std::pair<id_d_t, std::string> >::const_iterator cend = object_types.end();

	while (ci != cend) {
		id_d_t object_type_id = ci->first;
		std::string object_type_name = ci->second;

		addEmptyFlatStraw(object_type_id, object_type_name);

		++ci;
	}
	m_pOBBVec = 0;
}

/** Destructor.
 */
FlatSheaf::~FlatSheaf()
{
	std::map<id_d_t, FlatStraw*>::iterator it = m_map.begin();
	while (it != m_map.end()) {
		delete it->second;
		it->second = 0;
		++it;
	}
	m_map.clear();
	if (m_pOBBVec != 0) {
		for (OBBVec::size_type i = 0; i < m_pOBBVec->size(); ++i) {
			delete ((*m_pOBBVec)[i]);
		}
		delete m_pOBBVec;
	}
}


/** Add a MatchedObject to the FlatSheaf.
 *@internal
 *
 * This method only adds the MO if its object type is among those
 * which we wish to add (specified in the constructor of the
 * FlastSheaf).
 *
 * @param pMO the MatchedObject to add.
 *
 * @return true if it is added, false if not.
 */
bool FlatSheaf::addMO(const MatchedObject *pMO)
{
	// Should we add?
	bool bDoAdd = true;

	// Get the MatchedObject's object type id
	id_d_t object_type_id = pMO->getObjectTypeId();
	
	// Try to find the flat straw in the map
	std::map<id_d_t, FlatStraw*>::iterator it = m_map.find(object_type_id);
	
	// Add the flat straw if it is not there already,
	// and/or get the flat straw pointer
	FlatStraw *pFlatStraw = 0;
	if (it == m_map.end()) {
		if (m_bDoAll) {
			std::string object_type_name = pMO->getObjectTypeName();
			pFlatStraw = addEmptyFlatStraw(object_type_id, object_type_name);
		} else {
			bDoAdd = false;
		}
	} else {
		pFlatStraw = it->second;
	}
	
	// Do it!
	if (bDoAdd) {
			pFlatStraw->addMO(pMO);
	}
	return bDoAdd;
}

void FlatSheaf::addFlatStraw(id_d_t object_type_id, FlatStraw *pFlatStraw)
{
	std::map<id_d_t, FlatStraw*>::iterator it = m_map.find(object_type_id);

	// Add the flat straw if it is not there already,
	// and/or get the flat straw pointer
	if (it != m_map.end()) {
		FlatStraw *pCurrentFlatStraw = it->second;
		delete pCurrentFlatStraw;
		m_map.erase(it);
	}
	m_map.insert(std::make_pair(object_type_id, pFlatStraw));
}

/** Add an empty flat straw associated with the given object type.
 *@internal
 *
 * @param object_type_id The object type id of the object type.
 *
 * @param object_type_name The object type name of the object type.
 *
 * @return The newly created FlatStraw.
 */
FlatStraw *FlatSheaf::addEmptyFlatStraw(id_d_t object_type_id, const std::string& object_type_name)
{
	// Add it unconditionally
	FlatStraw *pResult = new FlatStraw(object_type_name);
	m_map[object_type_id] = pResult;
	return pResult;
}


/** Print on the console, console-style.
 *
 * Prints the flat sheaf on the EMdFOutput parameter pOut,
 * console-style.
 *
 * @param pOut A pointer to the EMdFOutput that must be printed on.
 */
void FlatSheaf::printConsole(EMdFOutput *pOut) const
{
	if (isFail()) {
		pOut->out(" // \n");
	} else {
		pOut->out(" // < ");
		FlatSheafConstIterator ci = const_iterator();
		while (ci.hasNext()) {
			FlatStraw *pFlatStraw = ci.next();
			pFlatStraw->printConsole(pOut);
		}
		pOut->out(" >\n");
	}
}

/** Print on the console, XML-style.
 *
 * Prints the flat sheaf on the EMdFOutput parameter pOut,
 * XML-style.
 *
 * @param pOut A pointer to the EMdFOutput that must be printed on.
 */
void FlatSheaf::printXML(EMdFOutput* pOut) const
{
	if (isFail()) {
		// Do nothing
	} else {
		pOut->startTag("flatsheaf", true);
		FlatSheafConstIterator ci = const_iterator();
		while (ci.hasNext()) {
			FlatStraw *pFlatStraw = ci.next();
			pFlatStraw->printXML(pOut);
		}
		pOut->endTag("flatsheaf", true);
	}
}



/** Print the part of the DTD that pertains to a FlatSheaf.
 *
 * @param pOut the EMdFOutput on which to print the DTD part.
 */
void FlatSheaf::printDTD(EMdFOutput* pOut) 
{
	pOut->out("  <!ELEMENT flatsheaf (flatstraw)* >"); 
	pOut->newline();
	pOut->out("  <!ELEMENT flatstraw (matched_object)* >"); 
	pOut->newline();
	pOut->out("  <!ATTLIST flatstraw");
	pOut->newline();
	pOut->out("      object_type_name CDATA #REQUIRED >");
	pOut->newline();
}

/** Print on the console, JSON-style.
 *
 * Prints the flat sheaf on the EMdFOutput parameter pOut,
 * JSON-style.
 *
 * @param pOut A pointer to the EMdFOutput that must be printed on.
 */
void FlatSheaf::printJSON(EMdFOutput* pOut) const
{
	if (isFail()) {
		// Do nothing
	} else {
		pOut->jsonLabel("flatsheaf");
        pOut->jsonStartStruct();
		pOut->jsonLabel("straws");
        pOut->jsonStartArray();

		FlatSheafConstIterator ci = const_iterator();
		while (ci.hasNext()) {
			FlatStraw *pFlatStraw = ci.next();
			pFlatStraw->printJSON(pOut);
		}

        pOut->jsonEndArray();
		pOut->jsonEndStruct();
	}
}


/** Flatten a full sheaf to a flat sheaf.
 *@ingroup SheafGroup
 *
 * See the documentation for \ref SheafGroup "Sheaves" for more
 * information on what a full sheaf and a flat sheaf are.
 *
 * This incarnation produces a flat sheaf from a specified set of
 * object types.
 *
 * @param object_types A std::list of std::pair's, where the second
 * member of the pair is an object type name, and the first member is
 * that object type's object type id.
 *
 * @param ppSheaf The full sheaf to flatten.
 *
 * @return The flattened sheaf, or nil (0) if an error occurred, such
 * as pSheaf itself being nil.
 *
 */
FlatSheaf *mql_flatten_sheaf(const std::list<std::pair<id_d_t, std::string> >& object_types, Sheaf **ppSheaf)
{
	if (*ppSheaf == 0) {
		return 0;
	} else {
		FlatSheaf *pFlatSheaf ;
		if (object_types.empty()) {
			pFlatSheaf = new FlatSheaf();
			(*ppSheaf)->flatten(pFlatSheaf);
		} else {
			pFlatSheaf = new FlatSheaf(object_types);
			(*ppSheaf)->flatten(pFlatSheaf);
		}
		pFlatSheaf->setOBBVec((*ppSheaf)->getOBBVec());
		(*ppSheaf)->setOBBVec(0);
		delete (*ppSheaf);
		*ppSheaf = 0;
		return pFlatSheaf;
	}
}


/** Flatten a full sheaf to a flat sheaf.
 *@ingroup SheafGroup
 *
 * See the documentation for \ref SheafGroup "Sheaves" for more
 * information on what a full sheaf and a flat sheaf are.
 *
 * This incarnation produces a flat sheaf from all the object types
 * available in the full sheaf, minus pow_m.
 *
 * This function is not SWIG-wrapped.
 *
 * @param ppSheaf A pointer to a pointer to the full sheaf to flatten.
 * The sheaf is deleted, and the pointer to the sheaf set to 0.
 *
 * @return The flattened sheaf, or nil (0) if an error occurred, such
 * as pSheaf itself being nil.
 *
 */
FlatSheaf *mql_flatten_sheaf(Sheaf **ppSheaf) // For all object types in the sheaf
{
	if (*ppSheaf == 0) {
		return 0;
	} else {
		FlatSheaf *pFlatSheaf = new FlatSheaf();
		(*ppSheaf)->flatten(pFlatSheaf);
		pFlatSheaf->setOBBVec((*ppSheaf)->getOBBVec());
		(*ppSheaf)->setOBBVec(0);
		delete (*ppSheaf);
		*ppSheaf = 0;
		return pFlatSheaf;
	}
}

/** Flatten a full sheaf to a flat sheaf.
 *@ingroup SheafGroup
 *
 * See the documentation for \ref SheafGroup "Sheaves" for more
 * information on what a full sheaf and a flat sheaf are.
 *
 * This incarnation produces a flat sheaf from a specified set of
 * object types.
 *
 * @param pObjectTypeNames A StringList of object type names
 * signifying the object types to use (filter in, not out) when
 * producing the flat sheaf.
 *
 * @param pDB The EMdFDB associated with this full sheaf.
 *
 * @param ppSheaf A pointer to a pointer to full sheaf to flatten.
 * This is set to 0 if it is not a failed sheaf.
 *
 * @return The flattened sheaf, or nil (0) if an error occurred, such
 * as pSheaf itself being nil.
 *
 */
FlatSheaf *mql_flatten_sheaf(StringList *pObjectTypeNames, EMdFDB *pDB, Sheaf **ppSheaf)
{
	if (pObjectTypeNames == 0 || pObjectTypeNames->isEmpty()) {
		return mql_flatten_sheaf(ppSheaf);
	} else {
		StringListConstIterator ci = pObjectTypeNames->const_iterator();
		std::list<std::pair<id_d_t, std::string> > object_types;
		while (ci.hasNext()) {
			const std::string& object_type_name = ci.next();
      
			bool bDBResult;
			bool bObjectTypeExists;
			id_d_t object_type_id;
			eObjectRangeType dummyObjectRangeType;
			eMonadUniquenessType dummyMonadUniquenessType;			
			bDBResult = pDB->objectTypeExists(object_type_name,
							  bObjectTypeExists, 
							  object_type_id,
							  dummyObjectRangeType,
							  dummyMonadUniquenessType);


			// Was there a DB error?
			if (!bDBResult) {
				return 0;
			}

			// Was the object type there?
			if (!bObjectTypeExists) {
				return 0;
			}

			// If we came this far, it existed, 
			// so add it to the list
			object_types.push_back(std::pair<id_d_t, std::string>(object_type_id, object_type_name));
		}
    
		// If we came this far, there were no DB errors
		return mql_flatten_sheaf(object_types, ppSheaf);
	}
}

/** Flatten a full sheaf to a flat sheaf.
 *@ingroup SheafGroup
 *
 * See the documentation for \ref SheafGroup "Sheaves" for more
 * information on what a full sheaf and a flat sheaf are.
 *
 * This incarnation produces a flat sheaf from a specified set of
 * object types, and takes an EmdrosEnv instead of an EMdFDB.  It is a
 * thin wrapper around the incarnation that takes an EMdFDB and
 * otherwise identical parameters.
 *
 * @param pObjectTypeNames A StringList of object type names
 * signifying the object types to use (filter in, not out) when
 * producing the flat sheaf.
 *
 * @param pEnv The EmdrosEnv with which this full sheaf is associated.
 *
 * @param ppSheaf A pointer to a pointer to the full sheaf to flatten.
 * The full sheaf is deleted, and the pointer to the sheaf set to 0,
 * unless it was a failed sheaf.
 *
 * @return The flattened sheaf, or nil (0) if an error occurred, such
 * as pSheaf itself being nil.
 *
 */
FlatSheaf *mql_flatten_sheaf(StringList *pObjectTypeNames, EmdrosEnv *pEnv, Sheaf **ppSheaf)
{
	EMdFDB *pDB = pEnv->getMQLEE()->pDB;
	return mql_flatten_sheaf(pObjectTypeNames, pDB, ppSheaf);
}
