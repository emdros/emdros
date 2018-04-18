/*
 * infos.h
 *
 * classes FeatureInfo, EnumConstInfo, ObjectTypeInfo
 *
 * Ulrik Petersen
 * Created: 2/26-2001
 * Last update: 11/27-2012
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2012  Ulrik Sandborg-Petersen
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

/**@file infos.h
 *@brief Header file for EMdF data infos (EMdF layer)
 */


#ifndef INFOS__H__
#define INFOS__H__

#include "emdf_enums.h"


#include "emdf.h"
#include <list>
#include <string>

class FeatureInfo {
private:
  std::string m_name;
  id_d_t m_type;
  std::string m_default_value;
  bool m_is_computed;
public:
  FeatureInfo(std::string name, id_d_t type,
	      std::string default_value, bool is_computed);
  FeatureInfo(const FeatureInfo& other);
  FeatureInfo() : m_name(), m_type(0), m_default_value(), m_is_computed(false) {};
  FeatureInfo& operator=(const FeatureInfo& other);
  bool operator==(const FeatureInfo& other) const;
  void copyOther(const FeatureInfo& other);
  ~FeatureInfo();
  const std::string& getName(void) const;
  id_d_t getType(void) const;
  const std::string& getDefaultValue(void) const;
  bool getIsComputed(void) const;
};



class EnumConstInfo {
private:
  std::string m_name;
  int m_value;
  bool m_is_default;
public:
  EnumConstInfo(std::string name, int value, bool is_default);
  EnumConstInfo(const EnumConstInfo& other);
  ~EnumConstInfo();
  const std::string& getName(void) const;
  int getValue(void) const;
  bool getIsDefault(void) const;
  bool operator==(const EnumConstInfo& other) const;
  void setValue(int new_value) { m_value = new_value; };
  void setIsDefault(bool bNewIsDefault) { m_is_default = bNewIsDefault; };
};


class ObjectTypeInfo {
private:
  std::string m_object_type_name;
  id_d_t m_object_type_id;
  eObjectRangeType m_objectRangeType;
public:
  ObjectTypeInfo(std::string object_type_name, 
		 id_d_t object_type_id, 
		 eObjectRangeType objectRangeType);  
  ObjectTypeInfo(const ObjectTypeInfo& other);
  ~ObjectTypeInfo();
  const std::string& getObjectTypeName(void) const;
  id_d_t getObjectTypeId(void) const;
  eObjectRangeType getObjectRangeType(void) const;
  bool operator==(const ObjectTypeInfo& other) const;
};


#endif // INFOS__H__
