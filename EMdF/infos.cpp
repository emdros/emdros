/*
 * infos.cpp
 *
 * classes FeatureInfo, EnumConstInfo
 *
 * Ulrik Petersen
 * Created: 2/26-2001
 * Last update: 6/8-2018
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2018  Ulrik Sandborg-Petersen
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


EnumConstInfo::EnumConstInfo(std::string name, emdros_int64 value, bool is_default):
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

emdros_int64 EnumConstInfo::getValue(void) const
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
	    && m_is_default == other.m_is_default) {
		return true;
	} else {
		return false;
	}
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


