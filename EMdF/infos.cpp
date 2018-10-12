/*
 * infos.cpp
 *
 * classes FeatureInfo, EnumConstInfo
 *
 * Ulrik Petersen
 * Created: 2/26-2001
 * Last update: 11/27-2005
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


///////////////////////////////////////////////////////////
//
// FeatureInfo
//
///////////////////////////////////////////////////////////

FeatureInfo::FeatureInfo(std::string name, id_d_t type,
   		   std::string default_value, bool is_computed) 
	: m_name(name), 
	  m_type(type), 
	  m_default_value(default_value), 
	  m_is_computed(is_computed)
{
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
      && m_type == other.m_type
      && m_default_value == other.m_default_value
      && m_is_computed == other.m_is_computed)
    return true;
  else
    return false;
}

void FeatureInfo::copyOther(const FeatureInfo& other)
{
  m_name = other.m_name; 
  m_type = other.m_type; 
  m_default_value = other.m_default_value; 
  m_is_computed = other.m_is_computed;
}



const std::string& FeatureInfo::getName(void) const
{
  return m_name;
}

id_d_t FeatureInfo::getType(void) const
{
  return m_type;
}

const std::string& FeatureInfo::getDefaultValue(void) const
{
  return m_default_value;
}

bool FeatureInfo::getIsComputed(void) const
{
  return m_is_computed;
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
  return
    m_object_type_name == other.m_object_type_name 
    && m_object_type_id == other.m_object_type_id 
    && m_objectRangeType == other.m_objectRangeType;
}


