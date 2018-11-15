/*
 * infos.cpp
 *
 * classes FeatureInfo, EnumConstInfo
 *
 * Ulrik Sandborg Petersen
 * Created: 2/26-2001
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


/**@file infos.cpp
 *@brief Implementation of EMdF data infos (EMdF layer)
 */

#include <infos.h>
#include <monads.h>
#include <debug.h>




///////////////////////////////////////////////////////////
//
// FeatureInfo
//
///////////////////////////////////////////////////////////

FeatureInfo::FeatureInfo(const std::string& name,
			 const std::string& parameter1,
			 id_d_t retrieved_type,
			 const std::string& default_value)
	: m_name(name),
	  m_parameter1(parameter1),
	  m_default_value(default_value),
	  m_output_type(retrieved_type),  // Default is the same as the retrieved type. The normal case.
	  m_retrieved_type(retrieved_type),
	  m_retrieved_feature_name(name) // Default is the same as the name. The normal case.
{
	m_computed_feature_kind = getComputedFeatureKindFromComputedFeatureName(m_name);
	switch (m_computed_feature_kind) {
	case kCFKNone:
		break;
	case kCFKSelf:
		m_output_type = FEATURE_TYPE_ID_D;
		m_retrieved_type = FEATURE_TYPE_ID_D;
		m_retrieved_feature_name = "self";
		break;
	case kCFKFirstMonad:
	case kCFKLastMonad:
	case kCFKMonadCount:
	case kCFKMonadSetLength:
		m_output_type = FEATURE_TYPE_INTEGER;
		m_retrieved_type = FEATURE_TYPE_SET_OF_MONADS;
		if (m_parameter1.empty()) {
			// Set this before setting m_retrieved_feature_name
			m_parameter1 = "monads";
		}
		m_retrieved_feature_name = m_parameter1;
		break;
	}
}

FeatureInfo::FeatureInfo(const FeatureInfo& other)
{
	copyOther(other);
}

FeatureInfo::~FeatureInfo()
{
}

FeatureInfo& FeatureInfo::operator=(const FeatureInfo& other)
{
	copyOther(other);
	return *this;
}

bool FeatureInfo::operator==(const FeatureInfo& other) const
{
	if (m_name == other.m_name
	    && m_computed_feature_kind == other.m_computed_feature_kind
	    && m_output_type == other.m_output_type
	    && m_retrieved_type == other.m_retrieved_type
	    && m_default_value == other.m_default_value
	    && m_retrieved_feature_name == other.m_retrieved_feature_name) 
		return true;
	else
		return false;
}

bool FeatureInfo::canBePreQueried() const
{
	switch (m_computed_feature_kind) {
	case kCFKNone:
		// It is not computed. Default to being able to be pre queried.
		// It might not be, for other reasons, but that
		// is unknown here.
		return true;
		break;
	case kCFKSelf:
		// Self can always be prequeried.
		return true;
		break;
	case kCFKFirstMonad:
	case kCFKLastMonad:
	case kCFKMonadCount:
	case kCFKMonadSetLength:
		// Monad sets cannot be pre queried, for now.
		// There is no reason why it should not be able to be
		// pre-queried, however, so we might reverse this later.
		return false;
		break;
	}
	ASSERT_THROW(false,
		     "Unimplemented eComputedFeatureKind in FeatureInfo::canBePreQueried.");
	// We didn't know the 
	return true;
}

const std::string& FeatureInfo::getRetrievedFeatureName() const
{
	return m_retrieved_feature_name;
}

std::string FeatureInfo::getHumanReadableFeatureName() const
{
	switch (m_computed_feature_kind) {
	case kCFKNone:
		return m_name;
		break;
	case kCFKSelf:
		return "self";
		break;
	case kCFKFirstMonad:
	case kCFKLastMonad:
	case kCFKMonadCount:
	case kCFKMonadSetLength:
		return m_name + "(" + m_parameter1 + ")";
		break;
	}

	ASSERT_THROW(false,
		     "Unimplemented eComputedFeatureKind in FeatureInfo::getHumanReadableFeatureName().");
	return ""; // Fool some compilers into giving no warning
}


void FeatureInfo::changeIntoComputedValue(EMdFValue *pValue) const
{
	
	// First test the type
	switch (m_computed_feature_kind) {
	case kCFKNone:
		// Nothing to check...
		break;
	case kCFKSelf:
		ASSERT_THROW(pValue->getKind() == kEVInt,
		     "ERROR: FeatureInfo::computeValue() called with left value\nwhich is not an integer.\n");
		break;
	case kCFKFirstMonad:
	case kCFKLastMonad:
	case kCFKMonadCount:
	case kCFKMonadSetLength:
		ASSERT_THROW(pValue->getKind() == kEVSetOfMonads,
		     "ERROR: FeatureInfo::computeValue() called with left value\nwhich is not a set of monads.\n");
		break;
	}
	
	long result = 0;
	switch (m_computed_feature_kind) {
	case kCFKNone:
		// It is not computed. Do nothing.
		break;
	case kCFKSelf:
		// It is self. Do nothing.
		break;
	case kCFKFirstMonad:
		{
			SetOfMonads som(pValue->getSOM());
			result = som.first();
			*pValue = EMdFValue(kEVInt, result);
		}
		break;
	case kCFKLastMonad:
		{
			SetOfMonads som(pValue->getSOM());
			result = som.last();
			*pValue = EMdFValue(kEVInt, result);
		}
		break;
	case kCFKMonadCount:
		{
			SetOfMonads som(pValue->getSOM());
			result = som.getCardinality();
			*pValue = EMdFValue(kEVInt, result);
		}
		break;
	case kCFKMonadSetLength:
		{
			SetOfMonads som(pValue->getSOM());
			result = som.last() - som.first() + 1;
			*pValue = EMdFValue(kEVInt, result);
		}
		break;
	}

	ASSERT_THROW(false,
		     "Unimplemented eComputedFeatureKind in FeatureInfo::computeValue().");
}

       

void FeatureInfo::copyOther(const FeatureInfo& other)
{
	m_name = other.m_name;
	m_parameter1 = other.m_parameter1;
	m_default_value = other.m_default_value; 
	m_output_type = other.m_output_type; 
	m_retrieved_type = other.m_retrieved_type; 
	m_retrieved_feature_name = other.m_retrieved_feature_name;
	m_computed_feature_kind = other.m_computed_feature_kind;
}



const std::string& FeatureInfo::getFeatureName() const
{
	return m_name;
}

const std::string& FeatureInfo::getParameter1() const
{
	return m_parameter1;
}

id_d_t FeatureInfo::getOutputType() const
{
	return m_output_type;
}

id_d_t FeatureInfo::getRetrievedType() const
{
	return m_retrieved_type;
}

const std::string& FeatureInfo::getDefaultValue() const
{
	return m_default_value;
}

bool FeatureInfo::getIsComputed() const
{
	return m_computed_feature_kind != kCFKNone;
}




///////////////////////////////////////////////////////////
//
// EnumConstInfo
//
///////////////////////////////////////////////////////////


EnumConstInfo::EnumConstInfo(std::string name, int value, bool is_default):
	m_name(name), 
	m_value(value), 
	m_is_default(is_default)
{

}

EnumConstInfo::EnumConstInfo(const EnumConstInfo& other) :
	m_name(other.m_name),
	m_value(other.m_value),
	m_is_default(other.m_is_default)
{

}


EnumConstInfo::~EnumConstInfo()
{

}

const std::string& EnumConstInfo::getName(void) const
{
	return m_name;
}

int EnumConstInfo::getValue(void) const
{
	return m_value;
}

bool EnumConstInfo::getIsDefault(void) const
{
	return m_is_default;
}

bool EnumConstInfo::operator==(const EnumConstInfo& other) const
{
	if (m_name == other.m_name
	    && m_value == other.m_value
	    && m_is_default == other.m_is_default)
		return true;
	else
		return false;
}




///////////////////////////////////////////////////////////
//
// ObjectTypeInfo
//
///////////////////////////////////////////////////////////

ObjectTypeInfo::ObjectTypeInfo(std::string object_type_name, 
			       id_d_t object_type_id, 
			       eObjectRangeType objectRangeType) : 
	m_object_type_name(object_type_name),
	m_object_type_id(object_type_id),
	m_objectRangeType(objectRangeType)
{
}


ObjectTypeInfo::ObjectTypeInfo(const ObjectTypeInfo& other)
{
	m_object_type_name = other.m_object_type_name;
	m_object_type_id = other.m_object_type_id;
	m_objectRangeType = other.m_objectRangeType;
}


ObjectTypeInfo::~ObjectTypeInfo()
{
}


const std::string& ObjectTypeInfo::getObjectTypeName(void) const
{
	return m_object_type_name;
}


id_d_t ObjectTypeInfo::getObjectTypeId(void) const
{
	return m_object_type_id;
}


eObjectRangeType ObjectTypeInfo::getObjectRangeType(void) const
{
	return m_objectRangeType;
}


bool ObjectTypeInfo::operator==(const ObjectTypeInfo& other) const
{
	return m_object_type_name == other.m_object_type_name 
		&& m_object_type_id == other.m_object_type_id 
		&& m_objectRangeType == other.m_objectRangeType;
}


