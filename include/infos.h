/*
 * infos.h
 *
 * classes FeatureInfo, EnumConstInfo, ObjectTypeInfo
 *
 * Ulrik Sandborg Petersen
 * Created: 2/26-2001
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


/**@file infos.h
 *@brief Header file for EMdF data infos (EMdF layer)
 */


#ifndef INFOS__H__
#define INFOS__H__

#include "emdf_enums.h"


#include "emdf.h"
#include "emdf_value.h"
#include <list>
#include <string>


class FeatureInfo {
private:
	std::string m_name;
	std::string m_parameter1;
	std::string m_default_value;
	id_d_t m_output_type;
	id_d_t m_retrieved_type;
	std::string m_retrieved_feature_name;
	eComputedFeatureKind m_computed_feature_kind;
public:
	FeatureInfo(const std::string& name,
		    const std::string& parameter1,
		    id_d_t retrieved_type,
		    const std::string& default_value);
	FeatureInfo(const FeatureInfo& other);
	FeatureInfo() : m_name(), m_parameter1(), m_default_value(), m_output_type(0), m_retrieved_type(0), m_retrieved_feature_name(), m_computed_feature_kind(kCFKNone) {};
	FeatureInfo& operator=(const FeatureInfo& other);
	bool operator==(const FeatureInfo& other) const;
	void copyOther(const FeatureInfo& other);
	~FeatureInfo();
	eComputedFeatureKind getComputedFeatureKind() const { return m_computed_feature_kind; };
	const std::string& getFeatureName() const;
	const std::string& getParameter1() const;
	id_d_t getOutputType(void) const;
	id_d_t getRetrievedType(void) const;
	const std::string& getDefaultValue() const;
	const std::string& getRetrievedFeatureName() const;
	std::string getHumanReadableFeatureName() const;
	bool getIsComputed() const;
	bool getIsSelf() const;
	bool canBePreQueried() const;
	void changeIntoComputedValue(EMdFValue *pValue) const;
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
