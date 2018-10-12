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



#include "schema.h"
#include "emdfxml_dtd.h"
#include <string_func.h>
#include <qdxml.h>

class EMdFXMLHandler : public QDDocHandler {
	std::list<std::string> m_element_stack;

	std::string m_db_name;
	std::string m_monad_set_name;
	std::string m_enumeration_name;
	int m_enum_const_value;
	std::list<EnumConstInfo> m_enum_const_declarations;
	std::string m_object_type_name;
	std::string m_range_type;
	std::string m_monad_uniqueness_type;
	std::list<EMdFXMLFeatureInfo> m_feature_declarations;
	Inst *m_pInst;
	id_d_t m_object_id_d;
	SetOfMonads m_object_som;

	EMdFXMLEventHandler *m_pEVH;
	std::string m_error_message;
 public:
	EMdFXMLHandler(EMdFXMLEventHandler *pEventHandler);
	virtual ~EMdFXMLHandler();
	virtual void startElement(const std::string& tag, const AttributeMap& attrs);
	virtual void endElement(const std::string& tag);

	virtual std::string topElement();

	virtual std::string getErrorMessage() const { return m_error_message; };

protected:
	virtual std::string getRequiredAttribute(const std::string& tag, const std::string& attr_name, const AttributeMap& attrs);
	virtual std::string getOptionalAttribute(const std::string& attr_name, const AttributeMap& attrs, const std::string& default_value);

	virtual bool checkAttribIsBoolean(const std::string& attr_value, const std::string& tag, const std::string& attr_name);
	virtual void checkAttribIsIdentifier(const std::string& attr_value, const std::string& tag, const std::string& attr_name);
	virtual long checkAttribIsInteger(const std::string& attr_value, const std::string& tag, const std::string& attr_name);
	virtual SetOfMonads checkAttribIsSOM(const std::string& attr_value, const std::string& tag, const std::string& attr_name);
};

EMdFXMLHandler::EMdFXMLHandler(EMdFXMLEventHandler *pEventHandler)
	: m_pInst(0),
	  m_pEVH(pEventHandler)
{
}

EMdFXMLHandler::~EMdFXMLHandler()
{
	delete m_pInst;
}


void EMdFXMLHandler::checkTopElement(const std::string& tag, const std::string& required_parent)
{
	if (topElement() != required_parent) {
		m_error_message += "Error: Element <" + tag + "> must only occur as a child of the <" + required_parent + "> element.\n";
		throw QDException(m_error_message);
	}
}

std::string EMdFXMLHandler::getRequiredAttribute(const std::string& tag, const std::string& attr_name, const AttributeMap& attrs)
{
	AttributeMap::const_iterator ci = attrs.find(attr_name);
	if (ci == attrs.end()) {
		m_error_message += "Error: start-tag <" + tag + "> must have required attribute '" + attr_name + "'.\n";
		throw QDException(m_error_message);
	} else {
		return ci->second;
	}
}
	
std::string EMdFXMLHandler::getOptionalAttribute(const std::string& attr_name, const AttributeMap& attrs, const std::string& default_value)
{
	AttributeMap::const_iterator ci = attrs.find(attr_name);
	if (ci == attrs.end()) {
		return default_value;
	} else {
		return ci->second;
	}
}

std::string EMdFXMLHandler::topElement()
{
	if (m_element_stack.empty()) {
		return "";
	} else {
		return *m_element_stack.rbegin();
	}
}

bool EMdFXMLHandler::checkAttribIsBoolean(const std::string& attr_value, const std::string& tag, const std::string& attr_name)
{
	if (attr_value == "true") {
		return true;
	} else if (attr_value == "false") {
		return false;
	} else {
		m_error_message += "Error: Attribute '" + attr_name + "' on start-tag <" + tag + "...> is neither 'true' nor 'false'.\n";
		throw QDException(m_error_message);
	}
}

void EMdFXMLHandler::checkAttribIsIdentifier(const std::string& attr_value, const std::string& tag, const std::string& attr_name)
{
	if (is_identifier(attr_value)) {
		return; // Nothing to do
	} else {
		m_error_message += "Error: Attribute '" + attr_name + "' on start-tag <" + tag + "...> is not a C identifier.\n";
		throw QDException(m_error_message);
	}
}

long EMdFXMLHandler::checkAttribIsInteger(const std::string& attr_value, const std::string& tag, const std::string& attr_name)
{
	if (string_is_number(attr_value)) {
		return string2long(attr_value);
	} else {
		m_error_message += "Error: Attribute '" + attr_name + "' on start-tag <" + tag + "...> is not a base-10 integer.\n";
		throw QDException(m_error_message);
	}
}

SetOfMonads EMdFXMLHandler::checkAttribIsSOM(const std::string& attr_value, const std::string& tag, const std::string& attr_name)
{
	try {
		SetOfMonads som = SetOfMonads();
		som.fromString(attr_value);
		return som;
	} catch (BadMonadsException e) {
		m_error_message += "Error: attribute '" + attr_name + "' on start-tag <" + tag + "...> is not a valid monad set.\n";
		throw QDException(m_error_message);
	}
}

void EMdFXMLHandler::startElement(const std::string& tag, const AttributeMap& attrs)
{
	if (tag == "f") {
		checkTopElement(tag, "o");
		handleFeature(attrs); // FIXME: Do it properly, with
				      // InstObject and EMdFValues.
	} else if (tag == "o") {
		checkTopElement(tag, "object_type_data");
		std::string str_id_d = getOptionalAttribute("id_d", attrs, "NIL");
		if (str_id_d.length() == 3) {
			str_tolower(str_id_d);
			if (str_id_d == "nil") {
				str_id_d = "0";
			}
		}
		m_object_id_d = checkAttribIsInteger(str_id_d, tag, "id_d");

		std::string m = getRequiredAttribute(tag, "m", attrs);
		m_object_som = checkAttribIsSOM(m, tag, "m");
	} else if (tag == "object_type_data") {
		checkTopElement(tag, "data");
		m_object_type_name = getRequiredAttribute(tag, "name", attrs);
		std::vector<std::string> feature_names_dummy_vec;
		m_pInst = new Inst(feature_names_dummy_vec);
	} else if (tag == "feature_declaration") {
		checkTopElement(tag, "object_type_declaration");
		std::string name = getRequiredAttribute(tag, "name", attrs);
		std::string feature_type = getRequiredAttribute(tag, "feature_type", attrs);
		std::string real_feature_type;
		str_tolower(feature_type, real_feature_type);
		if (real_feature_type == "integer"
		    || real_feature_type == "id_d"
		    || real_feature_type == "list of integer"
		    || real_feature_type == "list of id_d"
		    || real_feature_type == "string"
		    || real_feature_type == "ascii"
		    || real_feature_type == "single monad set of monads"
		    || real_feature_type == "single range set of monads"
		    || real_feature_type == "multiple range set of monads"
		    || real_feature_type == "set of monads"
		    || is_identifier(real_feature_type)
		    || (real_feature_type.substr(0, 8) == "list of " && is_identifier(real_feature_type.subtr(8, std::string::npos)))) {
			// All OK.
		} else {
			m_error_message += "Error: attribute 'feature_type' on start-element <" + tag + "...> does not name a feature type.\n";
			throw QDException(m_error_message);
		}
		std::string with_index = getOptionalAttribtue("with_index", attrs, "false");
		bool bWithIndex = checkAttribIsBoolean(with_index, tag, "with_index");
		std::string from_set = getOptionalAttribtue("from_set", attrs, "false");
		bool bFromSet = checkAttribIsBoolean(from_set, tag, "from_set");
		std::string default_value = getOptionalAttribtue("default_value", attrs, "");
		EMdFXMLFeatureInfo feature_info = EMdFXMLFeatureInfo(name, feature_type, bFromSet, bWithIndex, default_value);
		m_feature_declarations.push_back(feature_info);
	} else if (tag == "object_type_declaration") {
		checkTopElement(tag, "object_type_declarations");
		m_object_type_name = getRequiredAttribute(tag, "name", attrs);
		m_range_type = getOptionalAttribute(tag, "range_type", "WITH MULTIPLE RANGE OBJECTS");
		str_tolower(m_range_type);
		if (m_range_type != "with multiple range objects"
		    && m_range_type != "with single range objects"
		    && m_range_type != "with single monad objects") {
			m_error_message += "Error: attribute 'range_type' on start-tag <" + tag + "...> must be one of:\n...... WITH MULTIPLE RANGE OBJECTS\n...... WITH SINGLE RANGE OBJECTS\n...... WITH SINGLE MONAD OBJECTS\n";
			throw QDException(m_error_message);
		}
		m_monad_uniqueness_type = getOptionalAttribute(tag, "monad_uniqueness_type", "WITHOUT UNIQUE MONADS");
		str_tolower(m_monad_uniqueness_type);
		if (m_monad_uniqueness_type != "having unique first monads"
		    && m_monad_uniqueness_type != "having unique first and last monads"
		    && m_monad_uniqueness_type != "without unique monads") {
			m_error_message += "Error: attribute 'monad_uniqueness_type' on start-tag <" + tag + "...> must be one of:\n...... HAVING UNIQUE FIRST MONADS\n...... HAVING UNIQUE FIRST AND LAST MONADS\n...... WITHOUT UNIQUE MONADS\n";
			throw QDException(m_error_message);
		}
	} else if (tag == "object_type_declarations") {
		checkTopElement(tag, "schema");
		pEVH->onStartObjectTypeDeclarations();
	} else if (tag == "enum_const") {
		checkTopElement(tag, "enumeration");
		std::string enum_const_name = getRequiredAttribute(tag, "name", attrs);
		checkAttribIsIdentifier(enum_const_name, tag, "name");
		std::string value = getOptionalAttribute("value", attrs, "");
		if (value.empty()) {
			++m_enum_const_value;
		} else {
			m_enum_const_value = (int) checkAttribIsInteger(value, tag, "value");
		}
		std::string is_default = getOptionalAttribute("default", attrs, "false");
		bool bIsDefault = checkAttribIsBoolean(is_default, tag, "default");
		EnumConstInfo enum_const_info = EnumConstInfo(enum_const_name, m_enum_const_value, bIsDefault);
		m_enum_const_declarations.push_back(enum_const_info);
	} else if (tag == "enumeration") {
		checkTopElement(tag, "enumerations");
		m_enumeration_name = getRequiredAttribute(tag, "name", attrs);
		m_enum_const_value = 0;
	} else if (tag == "enumerations") {
		checkTopElement(tag, "schema");
		pEVH->onStartEnumerations();
	} else if (tag == "monad_set") {
		checkTopElement(tag, "monad_sets");
		std::string name = getRequiredAttribute(tag, "name", attrs);
		std::string monads = getRequiredAttribute(tag, "monads", attrs);
		SetOfMonads som = checkAttribIsSOM(const std::string& attr_value, const std::string& tag, const std::string& attr_name);
		pEVH->onDeclareMonadSet(name, som);
	} else if (tag == "monad_sets") {
		checkTopElement(tag, "schema");
		pEVH->onStartMonadSets();
	} else if (tag == "data") {
		checkTopElement(tag, "db");
		pEVH->onStartData();
	} else if (tag == "schema") {
		checkTopElement(tag, "db");
		pEVH->onStartSchema();
	} else if (tag == "db") {
		checkTopElement(tag, "emdfxml");
		m_db_name = getOptionalAttribute("name", attrs, "");
		m_pEVH->onStartDatabase(m_db_name);
	} else if (tag == "emdfxml") {
		if (!m_elem_stack.empty()) {
			m_error_message += "Error: Start-tag <emdfxml> must be the top-level element.\n";
			throw QDException(m_error_message);
		}
	} else {
		m_error_message += "Unknown start-element <" + tag + "> in emdfxml.\n";
		throw QDException(m_error_message);
	}
	m_element_stack.push_back(tag);
}

void EMdFXMLHandler::endElement(const std::string& tag)
{
	m_element_stack.pop_back();
	XXX; // FIXME: Implement
}





bool read_EMdFXML_fromStream(std::istream *pIn, EMdFXMLEventHandler *pEventHandler, std::string& /* out */ error_message)
{
	// FIXME: Implement

}


bool read_EMdFXML_fromFile(const std::string& file_name, EMdFXMLEventHandler *pEventHandler, std::string& /* out */ error_message)
{
	// FIXME: Implement
}
	
