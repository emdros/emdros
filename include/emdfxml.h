/*
 * emdfxml.cpp
 *
 * Utilities to read / write emdfxml.
 *
 * Ulrik Sandborg-Petersen
 * Created: 10/3-2017 (October 3, 2017)
 * Last update: 10/3-2017
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



#ifndef EMDXML_H_
#define EMDXML_H_

#include <string>
#include <list>

#include <emdf_enums.h>
#include <infos.h>
#include <monads.h>
#include <mql_sheaf.h>

class EMdFXMLFeatureInfo {
 public:
	std::string m_feature_name;
	std::string m_feature_type;
	bool m_is_from_set;
	bool m_is_with_index;
	std::string m_default_value;
 public:
	EMdFXMLFeatureInfo(const std::string& feature_name,
			   const std::string& feature_type,
			   bool is_from_set,
			   bool is_with_index,
			   const std::string& default_value);
};

class EMdFXMLEventHandler {
 public:
	EMdFXMLEventHandler() = 0;
	virtual ~EMdFXMLEventHandler() = 0;

	virtual void onStartDatabase(const std::string& name) = 0;
	virtual void onEndDatabase(const std::string& name) = 0;

	virtual void onStartSchema() = 0;
	virtual void onEndSchema() = 0;

	virtual void onStartMonadSets() = 0;
	virtual void onDeclareMonadSet(const std::string& name, const SetOfMonads& monads)
	virtual void onEndMonadSets() = 0;

	virtual void onStartEnumerations() = 0;
	virtual void onDeclareEnumeration(const std::string& name, const std::list<EnumConstInfo>& enum_consts) = 0;
	virtual void onEndEnumerations() = 0;

	virtual void onStartObjectTypeDeclarations() = 0;
	virtual void onDeclareObjectType(const std::string& object_type_name,
					 eObjectRangeType range_type,
					 eMonadUniquenessType uniqueness_type,
					 const std::list<EMdFXMLFeatureInfo>& feature_declarations) = 0;
	virtual void onEndObjectTypeDeclarations() = 0;

	virtual void onStartData() = 0;
	virtual void onObjectTypeData(const std::string& object_type_name, FlatSheaf *pObjects) = 0;
	virtual void onEndData() = 0;
};

#ifndef SWIG
bool read_EMdFXML_fromStream(std::istream *pIn, EMdFXMLEventHandler *pEventHandler, std::string& /* out */ error_message);
#endif

bool read_EMdFXML_fromFile(const std::string& file_name, EMdFXMLEventHandler *pEventHandler, std::string& /* out */ error_message);


#endif /* !defined(EMDXML_H_) */
