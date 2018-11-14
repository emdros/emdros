/*
 * infos.h
 *
 * classes FeatureInfo, EnumConstInfo, ObjectTypeInfo
 *
 * Ulrik Petersen
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
	std::string m_parameter1;
	id_d_t m_type;
	std::string m_default_value;
	bool m_is_computed;
public:
	FeatureInfo(std::string name,
		    id_d_t type,
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
