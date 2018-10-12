/*
 * templatelang_classes.cpp
 *
 * Template language classes
 *
 * Ulrik Sandborg-Petersen
 * Created: 7/28-2008
 * Last update: 12/21-2017
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



#include <json_classes.h>
#include <string_func.h>
#include <templatelang_classes.h>
#include "templatelangpars.h"
#include <memobject.h>

//////////////////////////////////////////////////////////////////
//
// TemplateASTNode
//
//////////////////////////////////////////////////////////////////


TemplateASTNode::TemplateASTNode()
{
	m_pNext = 0;
}


TemplateASTNode::~TemplateASTNode()
{
	delete m_pNext;
}


void TemplateASTNode::exec(TemplateLangExecEnv *pEE)
{
	if (m_pNext) {
		m_pNext->exec(pEE);
	}
}

void TemplateASTNode::simplifyAST()
{
	if (m_pNext) {
		m_pNext->simplifyAST();
	}

	if (this->isTemplateString()) {
		if (m_pNext) {
			if (m_pNext->isTemplateString()) {
				TemplateString *pNext = (TemplateString *) m_pNext;
				std::string next_string = pNext->getString();

				TemplateString *pCur = (TemplateString *) this;
				std::string cur_string = pCur->getString();

				pCur->setString(next_string + cur_string);

				m_pNext = pNext->m_pNext;
				pNext->m_pNext = 0;
				delete pNext;
			}
		}
	}
}




//////////////////////////////////////////////////////////////////
//
// TemplateSimpleNode
//
//////////////////////////////////////////////////////////////////


TemplateSimpleNode::TemplateSimpleNode(eTemplateSimpleNodeKind kind)
{
	m_kind = kind;
}


TemplateSimpleNode::~TemplateSimpleNode()
{
	// Nothing to do
}


void TemplateSimpleNode::exec(TemplateLangExecEnv *pEE)
{
	TemplateASTNode::exec(pEE);

	switch (m_kind) {
	case kTemplateID_D:
		pEE->addToOutput(id_d2string(pEE->m_pObject->getID_D()));
		break;
	case kTemplateFirstMonad:
		pEE->addToOutput(monad_m2string(pEE->m_pObject->first()));
		break;
	case kTemplateDBName:
		pEE->addToOutput(pEE->getDBName());
		break;
	}
}




//////////////////////////////////////////////////////////////////
//
// TemplateFeature
//
//////////////////////////////////////////////////////////////////

TemplateFeature::TemplateFeature(long index, eMangleKind mangle_kind)
{
	m_index = index;
	m_mangle_kind = mangle_kind;
}


TemplateFeature::~TemplateFeature()
{
}


void TemplateFeature::exec(TemplateLangExecEnv *pEE)
{
	TemplateASTNode::exec(pEE);

	std::string tmp(pEE->m_pObject->getFeature(m_index));
	switch (m_mangle_kind) {
	case kMKXML:
		if (hasXMLCharsToMangle(tmp)) {
			pEE->addToOutput(escapeXMLEntities(tmp));
		} else {
			pEE->addToOutput(tmp);
		}
		break;
	case kMKJSON:
		if (hasJSONCharsToMangle(tmp)) {
			pEE->addToOutput(escapeJSONChars(tmp, false));
		} else {
			pEE->addToOutput(tmp);
		}
		break;
	case kMKNone:
		pEE->addToOutput(tmp);
		break;
	}
}




//////////////////////////////////////////////////////////////////
//
// TemplateAttrib
//
//////////////////////////////////////////////////////////////////

TemplateAttrib::TemplateAttrib(std::string *pString)
{
	m_attrib_name = *pString;
	delete pString;
}


TemplateAttrib::~TemplateAttrib()
{
}


void TemplateAttrib::exec(TemplateLangExecEnv *pEE)
{
	TemplateASTNode::exec(pEE);

	std::string tmp(pEE->getAttribute(m_attrib_name));
	if (hasXMLCharsToMangle(tmp)) {
		pEE->addToOutput(escapeXMLEntities(tmp));
	} else {
		pEE->addToOutput(tmp);
	}
}




//////////////////////////////////////////////////////////////////
//
// TemplateAttribNoMangle
//
//////////////////////////////////////////////////////////////////

TemplateAttribNoMangle::TemplateAttribNoMangle(std::string *pString)
{
	m_attrib_name = *pString;
	delete pString;
}


TemplateAttribNoMangle::~TemplateAttribNoMangle()
{
}


void TemplateAttribNoMangle::exec(TemplateLangExecEnv *pEE)
{
	TemplateASTNode::exec(pEE);

	std::string tmp(pEE->getAttribute(m_attrib_name));
	pEE->addToOutput(tmp);
}


//////////////////////////////////////////////////////////////////
//
// TemplateCopyAttribs
//
//////////////////////////////////////////////////////////////////

TemplateCopyAttribs::TemplateCopyAttribs()
{
}


TemplateCopyAttribs::~TemplateCopyAttribs()
{
}


void TemplateCopyAttribs::exec(TemplateLangExecEnv *pEE)
{
	TemplateASTNode::exec(pEE);

	const AttributeMap& attrs = pEE->getAllAttributes();

	AttributeMap::const_iterator ci = attrs.begin();
	while (ci != attrs.end()) {
		std::string attrib_name = ci->first;

		pEE->addToOutput(" ");
		pEE->addToOutput(attrib_name);
		pEE->addToOutput("=\"");

		std::string tmp(pEE->getAttribute(attrib_name));
		if (hasXMLCharsToMangle(tmp)) {
			pEE->addToOutput(escapeXMLEntities(tmp));
		} else {
			pEE->addToOutput(tmp);
		}
		pEE->addToOutput("\"");
		
		++ci;
	}
}





//////////////////////////////////////////////////////////////////
//
// TemplateCounter
//
//////////////////////////////////////////////////////////////////

TemplateCounter::TemplateCounter(std::string *pString)
{
	m_counter_name = *pString;
	delete pString;
}


TemplateCounter::~TemplateCounter()
{
}


void TemplateCounter::exec(TemplateLangExecEnv *pEE)
{
	TemplateASTNode::exec(pEE);

	pEE->addToOutput(longlong2string(pEE->getCounter(m_counter_name)));
}


//////////////////////////////////////////////////////////////////
//
// TemplateCounterFormat
//
//////////////////////////////////////////////////////////////////

TemplateCounterFormat::TemplateCounterFormat(std::string *pString, std::string *pFormat)
{
	m_counter_name = *pString;
	delete pString;
	m_format = *pFormat;
	delete pFormat;
}


TemplateCounterFormat::~TemplateCounterFormat()
{
}


void TemplateCounterFormat::exec(TemplateLangExecEnv *pEE)
{
	TemplateASTNode::exec(pEE);

	long long counter_value = pEE->getCounter(m_counter_name);

	pEE->addToOutput(longlong2string_format(counter_value, m_format));
}





//////////////////////////////////////////////////////////////////
//
// TemplateSetCounter
//
//////////////////////////////////////////////////////////////////

TemplateSetCounter::TemplateSetCounter(std::string *pString, long long value)
{
	m_counter_name = *pString;
	delete pString;
	m_value = value;
}


TemplateSetCounter::~TemplateSetCounter()
{
}


void TemplateSetCounter::exec(TemplateLangExecEnv *pEE)
{
	TemplateASTNode::exec(pEE);

	pEE->setCounter(m_counter_name, m_value);
}




//////////////////////////////////////////////////////////////////
//
// TemplateSetCounterVar
//
//////////////////////////////////////////////////////////////////

TemplateSetCounterVar::TemplateSetCounterVar(std::string *pString, std::string *pVariableName)
{
	m_counter_name = *pString;
	delete pString;
	m_variable_name = *pVariableName;
	delete pVariableName;
}


TemplateSetCounterVar::~TemplateSetCounterVar()
{
}


void TemplateSetCounterVar::exec(TemplateLangExecEnv *pEE)
{
	TemplateASTNode::exec(pEE);

	std::string varValue = pEE->getVar(m_variable_name);

	long long value = string2longlong(varValue);

	pEE->setCounter(m_counter_name, value);
}





//////////////////////////////////////////////////////////////////
//
// TemplateIncCounter
//
//////////////////////////////////////////////////////////////////

TemplateIncCounter::TemplateIncCounter(std::string *pString, long long value, std::string *pVarName)
{
	m_counter_name = *pString;
	delete pString;
	m_value = value;
	if (pVarName != 0) {
		m_var_name = *pVarName;
		delete pVarName;
	}
}


TemplateIncCounter::~TemplateIncCounter()
{
}


void TemplateIncCounter::exec(TemplateLangExecEnv *pEE)
{
	TemplateASTNode::exec(pEE);

	if (m_var_name.empty()) {
		pEE->incCounter(m_counter_name, m_value);
	} else {
		pEE->incCounter(m_counter_name, m_var_name);
	}
}





//////////////////////////////////////////////////////////////////
//
// TemplateDecCounter
//
//////////////////////////////////////////////////////////////////

TemplateDecCounter::TemplateDecCounter(std::string *pString, long long value, std::string *pVarName)
{
	m_counter_name = *pString;
	delete pString;
	m_value = value;
	if (pVarName != 0) {
		m_var_name = *pVarName;
		delete pVarName;
	}
}


TemplateDecCounter::~TemplateDecCounter()
{
}


void TemplateDecCounter::exec(TemplateLangExecEnv *pEE)
{
	TemplateASTNode::exec(pEE);

	if (m_var_name.empty()) {
		pEE->decCounter(m_counter_name, m_value);
	} else {
		pEE->decCounter(m_counter_name, m_var_name);
	}
}





//////////////////////////////////////////////////////////////////
//
// TemplateString
//
//////////////////////////////////////////////////////////////////

TemplateString::TemplateString(std::string *pString)
{
	m_string = *pString;
	delete pString;
}


TemplateString::~TemplateString()
{
}


void TemplateString::exec(TemplateLangExecEnv *pEE)
{
	TemplateASTNode::exec(pEE);

	pEE->addToOutput(m_string);
}


//////////////////////////////////////////////////////////////////
//
// TemplateNixing
//
//////////////////////////////////////////////////////////////////

TemplateNixing::TemplateNixing(eTemplateNixingType nixingType)
{
	m_nixingType = nixingType;
}


TemplateNixing::~TemplateNixing()
{
}


void TemplateNixing::exec(TemplateLangExecEnv *pEE)
{
	TemplateASTNode::exec(pEE);

	pEE->setNixing(m_nixingType);
}


//////////////////////////////////////////////////////////////////
//
// TemplateDictlookupFeature
//
//////////////////////////////////////////////////////////////////

TemplateDictlookupFeature::TemplateDictlookupFeature(std::string *pDictName, long feature_index, std::string *pDefaultValue, bool bMangleFeature)
{
	m_dict_name = *pDictName;
	delete pDictName;
	m_feature_index = feature_index;
	m_default_value = *pDefaultValue;
	delete pDefaultValue;
	m_bMangleFeature = bMangleFeature;
}


TemplateDictlookupFeature::~TemplateDictlookupFeature()
{
}


void TemplateDictlookupFeature::exec(TemplateLangExecEnv *pEE)
{
	TemplateASTNode::exec(pEE);

	std::string featureValue; //  = pEE->m_pObject->getFeature(m_feature_index);

	if (m_bMangleFeature) {
		featureValue = pEE->m_pObject->getFeature(m_feature_index);
		if (hasXMLCharsToMangle(featureValue)) {
			featureValue = escapeXMLEntities(featureValue);
		} else {
			; // Nothing to do
		}
	} else {
		featureValue = pEE->m_pObject->getFeature(m_feature_index);
	}
	pEE->addToOutput(pEE->dictLookup(m_dict_name, featureValue, m_default_value));
}





//////////////////////////////////////////////////////////////////
//
// TemplateDictlookupVar
//
//////////////////////////////////////////////////////////////////

TemplateDictlookupVar::TemplateDictlookupVar(std::string *pDictName, std::string *pVarName, std::string *pDefaultValue, bool bMangleVar)
{
	m_dict_name = *pDictName;
	delete pDictName;
	m_var_name = *pVarName;
	delete pVarName;
	m_default_value = *pDefaultValue;
	delete pDefaultValue;
	m_bMangleVar = bMangleVar;
}


TemplateDictlookupVar::~TemplateDictlookupVar()
{
}


void TemplateDictlookupVar::exec(TemplateLangExecEnv *pEE)
{
	TemplateASTNode::exec(pEE);

	std::string varValue;

	if (m_bMangleVar) {
		varValue = pEE->getVar(m_var_name);
		if (hasXMLCharsToMangle(varValue)) {
			varValue = escapeXMLEntities(varValue);
		} else {
			; // Nothing to do
		}
	} else {
		varValue = pEE->getVar(m_var_name);
	}
	pEE->addToOutput(pEE->dictLookup(m_dict_name, varValue, m_default_value));
}





//////////////////////////////////////////////////////////////////
//
// TemplateDictlookupCounter
//
//////////////////////////////////////////////////////////////////

TemplateDictlookupCounter::TemplateDictlookupCounter(std::string *pDictName, std::string *pCounterName, std::string *pDefaultValue, bool bMangleVar)
{
	UNUSED(bMangleVar); // FIXME: Find out why it is not used, and
			    // use it if it should be used.
			    // Otherwise, remove it.
	
	m_dict_name = *pDictName;
	delete pDictName;
	m_counter_name = *pCounterName;
	delete pCounterName;
	m_default_value = *pDefaultValue;
	delete pDefaultValue;
}


TemplateDictlookupCounter::~TemplateDictlookupCounter()
{
}


void TemplateDictlookupCounter::exec(TemplateLangExecEnv *pEE)
{
	TemplateASTNode::exec(pEE);

	std::string counterValue = longlong2string(pEE->getCounter(m_counter_name));

	pEE->addToOutput(pEE->dictLookup(m_dict_name, counterValue, m_default_value));
}









//////////////////////////////////////////////////////////////////
//
// TemplateIf
//
//////////////////////////////////////////////////////////////////


TemplateIf::TemplateIf(TemplateASTNode *pCondition,
		       TemplateASTNode *pExecIfTrue,
		       TemplateASTNode *pExecIfFalse)
{
	m_pCondition = pCondition;
	m_pExecIfTrue = pExecIfTrue;
	m_pExecIfFalse = pExecIfFalse;
}


TemplateIf::~TemplateIf()
{
	delete m_pCondition;
	delete m_pExecIfTrue;
	delete m_pExecIfFalse;
}

void TemplateIf::simplifyAST()
{
	TemplateASTNode::simplifyAST();

	m_pCondition->simplifyAST();

	if (m_pExecIfTrue) {
		m_pExecIfTrue->simplifyAST();
	}

	if (m_pExecIfFalse) {
		m_pExecIfFalse->simplifyAST();
	}
}


void TemplateIf::exec(TemplateLangExecEnv *pEE)
{
	TemplateASTNode::exec(pEE);

	m_pCondition->exec(pEE);
	if (pEE->bLastIfCondition) {
		if (m_pExecIfTrue != 0) {
			m_pExecIfTrue->exec(pEE);
		}
	} else {	
		if (m_pExecIfFalse != 0) {
			m_pExecIfFalse->exec(pEE);
		}
	}
}




//////////////////////////////////////////////////////////////////
//
// TemplateIfFeatureEqual
//
//////////////////////////////////////////////////////////////////

TemplateIfFeatureEqual::TemplateIfFeatureEqual(long feature_index, std::string *pString)
{
	m_feature_index = feature_index;
	m_value = *pString;
	delete pString;
}


TemplateIfFeatureEqual::~TemplateIfFeatureEqual()
{
}


void TemplateIfFeatureEqual::exec(TemplateLangExecEnv *pEE)
{
	TemplateASTNode::exec(pEE);

	bool bConditionResult = false;

	std::string featureValue = pEE->m_pObject->getFeature(m_feature_index);
	if (featureValue == m_value) {
		bConditionResult = true;
	} else {
		bConditionResult = false;
	}

	pEE->bLastIfCondition = bConditionResult;
}





//////////////////////////////////////////////////////////////////
//
// TemplateIfVarEqual
//
//////////////////////////////////////////////////////////////////

TemplateIfVarEqual::TemplateIfVarEqual(std::string *pVariableName, std::string *pString, bool bStringIsVarName)
{

	m_variable_name = *pVariableName;
	delete pVariableName;

	m_value = *pString;
	delete pString;

	m_bValueIsVarName = bStringIsVarName;
}


TemplateIfVarEqual::~TemplateIfVarEqual()
{
}


void TemplateIfVarEqual::exec(TemplateLangExecEnv *pEE)
{
	TemplateASTNode::exec(pEE);

	bool bConditionResult = false;

	std::string variableValue = pEE->getVar(m_variable_name);
	if (m_bValueIsVarName) {
		std::string variable2Value = pEE->getVar(m_value);
		if (variableValue == variable2Value) {
			bConditionResult = true;
		} else {
			bConditionResult = false;
		}
	} else {
		if (variableValue == m_value) {
			bConditionResult = true;
		} else {
			bConditionResult = false;
		}
	}

	pEE->bLastIfCondition = bConditionResult;
}



//////////////////////////////////////////////////////////////////
//
// TemplateIfListEmpty
//
//////////////////////////////////////////////////////////////////

TemplateIfListEmpty::TemplateIfListEmpty(std::string *pListName)
{

	m_list_name = *pListName;
	delete pListName;
}


TemplateIfListEmpty::~TemplateIfListEmpty()
{
}


void TemplateIfListEmpty::exec(TemplateLangExecEnv *pEE)
{
	TemplateASTNode::exec(pEE);

	bool bConditionResult = pEE->listIsEmpty(m_list_name);

	pEE->bLastIfCondition = bConditionResult;
}



//////////////////////////////////////////////////////////////////
//
// TemplateIfNamedSetEmpty
//
//////////////////////////////////////////////////////////////////

TemplateIfNamedSetEmpty::TemplateIfNamedSetEmpty(std::string *pNamedSetName)
{

	m_named_set_name = *pNamedSetName;
	delete pNamedSetName;
}


TemplateIfNamedSetEmpty::~TemplateIfNamedSetEmpty()
{
}


void TemplateIfNamedSetEmpty::exec(TemplateLangExecEnv *pEE)
{
	TemplateASTNode::exec(pEE);

	bool bConditionResult = pEE->namedSetIsEmpty(m_named_set_name);

	pEE->bLastIfCondition = bConditionResult;
}





//////////////////////////////////////////////////////////////////
//
// TemplateIfAttribEqual
//
//////////////////////////////////////////////////////////////////

TemplateIfAttribEqual::TemplateIfAttribEqual(std::string *pAttribName, std::string *pString)
{
	m_attrib_name = *pAttribName;
	delete pAttribName;
	m_value = *pString;
	delete pString;
}


TemplateIfAttribEqual::~TemplateIfAttribEqual()
{
}


void TemplateIfAttribEqual::exec(TemplateLangExecEnv *pEE)
{
	TemplateASTNode::exec(pEE);

	bool bConditionResult = false;

	std::string attribValue = pEE->getAttribute(m_attrib_name);
	if (attribValue == m_value) {
		bConditionResult = true;
	} else {
		bConditionResult = false;
	}

	pEE->bLastIfCondition = bConditionResult;
}




//////////////////////////////////////////////////////////////////
//
// TemplateIfHasAttrib
//
//////////////////////////////////////////////////////////////////

TemplateIfHasAttrib::TemplateIfHasAttrib(std::string *pAttribName)
{
	m_attrib_name = *pAttribName;
	delete pAttribName;
}


TemplateIfHasAttrib::~TemplateIfHasAttrib()
{
}


void TemplateIfHasAttrib::exec(TemplateLangExecEnv *pEE)
{
	TemplateASTNode::exec(pEE);

	bool bConditionResult = pEE->hasAttribute(m_attrib_name);

	pEE->bLastIfCondition = bConditionResult;
}




//////////////////////////////////////////////////////////////////
//
// TemplateListAppendBegin
//
//////////////////////////////////////////////////////////////////

TemplateListAppendBegin::TemplateListAppendBegin(std::string *pStrListID, std::string *pStrDelimiter)
{
	m_strListID = *pStrListID;
	delete pStrListID;

	m_pStrDelimiter = pStrDelimiter;
}


TemplateListAppendBegin::~TemplateListAppendBegin()
{
}


void TemplateListAppendBegin::exec(TemplateLangExecEnv *pEE)
{
	TemplateASTNode::exec(pEE);

	pEE->enterListMode(m_strListID, m_pStrDelimiter);
}


//////////////////////////////////////////////////////////////////
//
// TemplateListAppendEnd
//
//////////////////////////////////////////////////////////////////

TemplateListAppendEnd::TemplateListAppendEnd()
{
}


TemplateListAppendEnd::~TemplateListAppendEnd()
{
}


void TemplateListAppendEnd::exec(TemplateLangExecEnv *pEE)
{
	TemplateASTNode::exec(pEE);

	pEE->exitListMode();
}




//////////////////////////////////////////////////////////////////
//
// TemplateNamedSetAddBegin
//
//////////////////////////////////////////////////////////////////

TemplateNamedSetAddBegin::TemplateNamedSetAddBegin(std::string *pStrNamedSetID, std::string *pStrDelimiter)
{
	m_strNamedSetID = *pStrNamedSetID;
	m_strDelimiter = *pStrDelimiter;
	delete pStrNamedSetID;
	delete pStrDelimiter;
}


TemplateNamedSetAddBegin::~TemplateNamedSetAddBegin()
{
}


void TemplateNamedSetAddBegin::exec(TemplateLangExecEnv *pEE)
{
	TemplateASTNode::exec(pEE);

	pEE->enterNamedSetAddMode(m_strNamedSetID, m_strDelimiter);
}


//////////////////////////////////////////////////////////////////
//
// TemplateNamedSetAddEnd
//
//////////////////////////////////////////////////////////////////

TemplateNamedSetAddEnd::TemplateNamedSetAddEnd()
{
}


TemplateNamedSetAddEnd::~TemplateNamedSetAddEnd()
{
}


void TemplateNamedSetAddEnd::exec(TemplateLangExecEnv *pEE)
{
	TemplateASTNode::exec(pEE);

	pEE->exitNamedSetAddMode();
}




//////////////////////////////////////////////////////////////////
//
// TemplateNamedSetRemoveBegin
//
//////////////////////////////////////////////////////////////////

TemplateNamedSetRemoveBegin::TemplateNamedSetRemoveBegin(std::string *pStrNamedSetID, std::string *pStrDelimiter)
{
	m_strNamedSetID = *pStrNamedSetID;
	m_strDelimiter = *pStrDelimiter;
	delete pStrNamedSetID;
	delete pStrDelimiter;
}


TemplateNamedSetRemoveBegin::~TemplateNamedSetRemoveBegin()
{
}


void TemplateNamedSetRemoveBegin::exec(TemplateLangExecEnv *pEE)
{
	TemplateASTNode::exec(pEE);

	pEE->enterNamedSetRemoveMode(m_strNamedSetID, m_strDelimiter);
}


//////////////////////////////////////////////////////////////////
//
// TemplateNamedSetRemoveEnd
//
//////////////////////////////////////////////////////////////////

TemplateNamedSetRemoveEnd::TemplateNamedSetRemoveEnd()
{
}


TemplateNamedSetRemoveEnd::~TemplateNamedSetRemoveEnd()
{
}


void TemplateNamedSetRemoveEnd::exec(TemplateLangExecEnv *pEE)
{
	TemplateASTNode::exec(pEE);

	pEE->exitNamedSetRemoveMode();
}




//////////////////////////////////////////////////////////////////
//
// TemplateListSetVarNow
//
//////////////////////////////////////////////////////////////////

TemplateSetVarNow::TemplateSetVarNow(std::string *pStrVarName, std::string *pStrValue)
{
	m_strVarName = *pStrVarName;
	delete pStrVarName;

	m_strValue = *pStrValue;
	delete pStrValue;
}


TemplateSetVarNow::~TemplateSetVarNow()
{
}


void TemplateSetVarNow::exec(TemplateLangExecEnv *pEE)
{
	TemplateASTNode::exec(pEE);

	pEE->setVar(m_strVarName, m_strValue);
}


//////////////////////////////////////////////////////////////////
//
// TemplateListSetVarBegin
//
//////////////////////////////////////////////////////////////////

TemplateSetVarBegin::TemplateSetVarBegin(std::string *pStrVarName)
{
	m_strVarName = *pStrVarName;
	delete pStrVarName;
}


TemplateSetVarBegin::~TemplateSetVarBegin()
{
}


void TemplateSetVarBegin::exec(TemplateLangExecEnv *pEE)
{
	TemplateASTNode::exec(pEE);

	pEE->enterSetVarMode(m_strVarName);
}


//////////////////////////////////////////////////////////////////
//
// TemplateSetVarEnd
//
//////////////////////////////////////////////////////////////////

TemplateSetVarEnd::TemplateSetVarEnd()
{
}


TemplateSetVarEnd::~TemplateSetVarEnd()
{
}


void TemplateSetVarEnd::exec(TemplateLangExecEnv *pEE)
{
	TemplateASTNode::exec(pEE);

	pEE->exitSetVarMode();
}


//////////////////////////////////////////////////////////////////
//
// TemplateEmitVar
//
//////////////////////////////////////////////////////////////////

TemplateEmitVar::TemplateEmitVar(std::string *pStrVarName)
{
	m_strVarName = *pStrVarName;
	delete pStrVarName;
}


TemplateEmitVar::~TemplateEmitVar()
{
}


void TemplateEmitVar::exec(TemplateLangExecEnv *pEE)
{
	TemplateASTNode::exec(pEE);

	pEE->emitVar(m_strVarName);
}





//////////////////////////////////////////////////////////////////
//
// TemplateListClear
//
//////////////////////////////////////////////////////////////////

TemplateListClear::TemplateListClear(std::string *pStrListID)
{
	m_strListID = *pStrListID;
	delete pStrListID;
}


TemplateListClear::~TemplateListClear()
{
}


void TemplateListClear::exec(TemplateLangExecEnv *pEE)
{
	TemplateASTNode::exec(pEE);

	pEE->clearList(m_strListID);
}



//////////////////////////////////////////////////////////////////
//
// TemplateNamedSetClear
//
//////////////////////////////////////////////////////////////////

TemplateNamedSetClear::TemplateNamedSetClear(std::string *pStrNamedSetID)
{
	m_strNamedSetID = *pStrNamedSetID;
	delete pStrNamedSetID;
}


TemplateNamedSetClear::~TemplateNamedSetClear()
{
}


void TemplateNamedSetClear::exec(TemplateLangExecEnv *pEE)
{
	TemplateASTNode::exec(pEE);

	pEE->clearNamedSet(m_strNamedSetID);
}




//////////////////////////////////////////////////////////////////
//
// TemplateListEmit
//
//////////////////////////////////////////////////////////////////

TemplateListEmit::TemplateListEmit(std::string *pStrListID, std::string *pStrSeparator)
{
	m_strListID = *pStrListID;
	delete pStrListID;

	m_strSeparator = *pStrSeparator;
	delete pStrSeparator;
}


TemplateListEmit::~TemplateListEmit()
{
}


void TemplateListEmit::exec(TemplateLangExecEnv *pEE)
{
	TemplateASTNode::exec(pEE);

	pEE->emitList(m_strListID, m_strSeparator);
}




//////////////////////////////////////////////////////////////////
//
// TemplateNamedSetEmit
//
//////////////////////////////////////////////////////////////////

TemplateNamedSetEmit::TemplateNamedSetEmit(std::string *pStrNamedSetID, std::string *pStrSeparator)
{
	m_strNamedSetID = *pStrNamedSetID;
	delete pStrNamedSetID;

	m_strSeparator = *pStrSeparator;
	delete pStrSeparator;
}


TemplateNamedSetEmit::~TemplateNamedSetEmit()
{
}


void TemplateNamedSetEmit::exec(TemplateLangExecEnv *pEE)
{
	TemplateASTNode::exec(pEE);

	pEE->emitNamedSet(m_strNamedSetID, m_strSeparator);
}






//////////////////////////////////////////////////////////////////
//
// TemplateLangExecEnv
//
//////////////////////////////////////////////////////////////////

TemplateLangExecEnv::TemplateLangExecEnv()
{
	m_pObject = 0;
	m_pError = new std::string();
	m_pNode = 0;
	m_pOutput = new Bigstring();
	m_bOwnsBigstring = true;
	pLexer = 0;
	m_pTopStylesheet = 0; // We don't own it! Don't delete it!
	clean();
}


TemplateLangExecEnv::~TemplateLangExecEnv()
{
	clean();
	delete m_pError;
	delete m_pNode;
	if (m_bOwnsBigstring) {
		delete m_pOutput;
	}
	delete pLexer;
}

void TemplateLangExecEnv::setVariables(const TemplateLangVariableMap& variables)
{
	if (!variables.empty()) {
		TemplateLangVariableMap::const_iterator ci = variables.begin();
		TemplateLangVariableMap::const_iterator cend = variables.end();
		while (ci != cend) {
			std::string variable_name = ci->first;
			std::string variable_value = ci->second;
			setVar(variable_name, variable_value);
			++ci;
		}
	}
}


void TemplateLangExecEnv::simplifyAST()
{
	if (m_pNode != 0) {
		m_pNode->simplifyAST();
	}
}

bool TemplateLangExecEnv::listIsEmpty(const std::string& list_name)
{
	bool bConditionResult = false;

	StringListMap::iterator it = m_lists.find(list_name);
	if (it == m_lists.end()) {
		bConditionResult = true;
	} else if (it->second.empty()) {
		bConditionResult = true;
	} else {
		bConditionResult = false;
	}

	return bConditionResult;
}


bool TemplateLangExecEnv::namedSetIsEmpty(const std::string& named_set_name)
{
	bool bConditionResult = false;

	StringNamedSetMap::iterator it = m_named_sets.find(named_set_name);
	if (it == m_named_sets.end()) {
		bConditionResult = true;
	} else if (it->second.empty()) {
		bConditionResult = true;
	} else {
		bConditionResult = false;
	}

	return bConditionResult;
}


void TemplateLangExecEnv::setVar(const std::string& variable_name, const std::string& value)
{
	createVarIfNotExists(variable_name);

	m_variables[variable_name]->clear();
	m_variables[variable_name]->addChars(value.data(), value.length());
}

void TemplateLangExecEnv::setBigstring(Bigstring *pBigstring)
{
	if (m_bOwnsBigstring) {
		delete m_pOutput;
	}
	m_pOutput = pBigstring;
	m_bOwnsBigstring = false;
}

void TemplateLangExecEnv::clean()
{
	m_nixingType = kTNTOff;
	m_counters.clear();
	m_lists.clear();
	m_delimited_lists.clear();
	m_named_sets.clear();
	m_emit_target_stack.clear();
	m_emit_target_stack.push_back(kETDoc);
	m_list_stack.clear();
	m_list_delimiter_stack.clear();
	delete pLexer;
	pLexer = 0;
	m_strCurrentSetVar = "";
	VariableMap::iterator it = m_variables.begin();
	while (it != m_variables.end()) {
		Bigstring *pBigstring = it->second;
		
		delete pBigstring;

		++it;
	}
	m_variables.clear();
	// We mustn't set m_pTopStylesheet to 0 here,
	// as clean() is called after it has been set!
}


void TemplateLangExecEnv::setTopNode(TemplateASTNode *pNode)
{
	delete m_pNode;
	m_pNode = pNode;
}


void TemplateLangExecEnv::setTopStylesheet(const JSONValue *pTopStylesheet)
{
	m_pTopStylesheet = pTopStylesheet;
}


TemplateASTNode *TemplateLangExecEnv::extractTopNode(void)
{
	TemplateASTNode *pResult = m_pNode;
	m_pNode = 0;
	return pResult;
}



void TemplateLangExecEnv::clearError(void)
{
	*m_pError = "";
}


std::string TemplateLangExecEnv::getError()
{
	return *m_pError;
}

void TemplateLangExecEnv::prependError(const std::string& addition)
{
	*m_pError = addition + "\n" + *m_pError;
}

void TemplateLangExecEnv::addToError(const std::string& addition)
{
	*m_pError += addition;
}

std::string TemplateLangExecEnv::dictLookup(const std::string& dict_name, const std::string& dict_key, const std::string& default_value)
{
	if (m_pTopStylesheet == 0) {
		return default_value;
	}
	if (m_pTopStylesheet->getKind() != kJSONObject) {
		return default_value;
	}

	if (!m_pTopStylesheet->hasObjectKey("dictionaries")) {
		return default_value;
	}

	const JSONValue *pDictionaries = m_pTopStylesheet->getObjectValue("dictionaries");
	if (pDictionaries == 0 || pDictionaries->getKind() != kJSONObject) {
		return default_value;
	}
	if (!pDictionaries->hasObjectKey(dict_name)) {
		return default_value;
	}

	const JSONValue *pMyDict = pDictionaries->getObjectValue(dict_name);
	if (pMyDict == 0 || pMyDict->getKind() != kJSONObject) {
		return default_value;
	}
	if (!pMyDict->hasObjectKey(dict_key)) {
		return default_value;
	} 

	const JSONValue *pMyValue = pMyDict->getObjectValue(dict_key);
	if (pMyValue == 0 || pMyValue->getKind() != kJSONString) {
		return default_value;
	} else {
		return pMyValue->getString();
	}
}



void TemplateLangExecEnv::clearOutput(void)
{
	m_pOutput->clear(); 
}


std::string TemplateLangExecEnv::getOutput()
{
	return m_pOutput->toString();
}

eEmitTarget TemplateLangExecEnv::getCurEmitTarget() 
{
	if (m_emit_target_stack.empty()) {
		pushEmitTargetStack(kETDoc);
	}
	return *m_emit_target_stack.rbegin();
}

void TemplateLangExecEnv::pushEmitTargetStack(eEmitTarget emitTarget)
{
	m_emit_target_stack.push_back(emitTarget);
}

eEmitTarget TemplateLangExecEnv::popEmitTargetStack()
{
	eEmitTarget topEmitTarget = getCurEmitTarget();
	m_emit_target_stack.pop_back();
	return topEmitTarget;
}

void TemplateLangExecEnv::addToOutput(const std::string& addition)
{
	if (m_nixingType == kTNTAll) { 
		// Nothing to do... nixing everything!
	} else {
		eEmitTarget topEmitTarget = getCurEmitTarget();
		switch (topEmitTarget) {
		case kETDoc:
			if (m_nixingType == kTNTOff) {
				m_pOutput->addChars(addition.data(), addition.length());
			}
			break;
		case kETList:
			m_lists[*m_list_stack.rbegin()].push_back(addition);
			break;
		case kETDelimitedList:
			m_delimited_lists[*m_list_stack.rbegin()].push_back(addition);
			break;
		case kETVar:
			createVarIfNotExists(m_strCurrentSetVar);
			m_variables[m_strCurrentSetVar]->addChars(addition.data(), addition.length());
			break;
		case kETNamedSet:
			{
				std::string strNamedSetName = *m_named_set_name_stack.rbegin();
				createNamedSetIfNotExists(strNamedSetName);
				m_sets_under_construction[strNamedSetName]->addChars(addition.data(), addition.length());
			}
			break;
		}
	} 
}

void TemplateLangExecEnv::addBigstringToOutput(Bigstring *pBigstring)
{
	if (m_nixingType == kTNTAll) {
	} else {
		eEmitTarget topEmitTarget = getCurEmitTarget();
		switch (topEmitTarget) {
		case kETDoc:
			if (m_nixingType == kTNTOff) {
				m_pOutput->addBigstring(pBigstring);
			}
			break;
		case kETList:
			m_lists[*m_list_stack.rbegin()].push_back(pBigstring->toString());
			break;
		case kETDelimitedList:
			m_delimited_lists[*m_list_stack.rbegin()].push_back(pBigstring->toString());
			break;
		case kETVar:
			createVarIfNotExists(m_strCurrentSetVar);
			m_variables[m_strCurrentSetVar]->addBigstring(pBigstring);
			break;
		case kETNamedSet:
			{
				std::string strNamedSetName = *m_named_set_name_stack.rbegin();
				createNamedSetIfNotExists(strNamedSetName);
				m_sets_under_construction[strNamedSetName]->addBigstring(pBigstring);
			}
			break;
		}
	}
}


void TemplateLangExecEnv::clear()
{
	clearOutput();
	clearError();
}

void TemplateLangExecEnv::setAttributes(const AttributeMap& attrs)
{
	m_attrs = attrs;
}

std::string TemplateLangExecEnv::getAttribute(const std::string& attrib_name)
{
	AttributeMap::const_iterator ci = m_attrs.find(attrib_name);
	if (ci != m_attrs.end()) {
		return ci->second;
	} else {
		return "";
	}
}


bool TemplateLangExecEnv::hasAttribute(const std::string& attrib_name)
{
	if (m_attrs.find(attrib_name) != m_attrs.end()) {
		return true;
	} else {
		return false;
	}
}

const AttributeMap& TemplateLangExecEnv::getAllAttributes() const
{
	return m_attrs;
}

long long TemplateLangExecEnv::getCounter(const std::string& counter_name)
{
	CounterMap::const_iterator it = m_counters.find(counter_name);
	if (it == m_counters.end()) {
		m_counters[counter_name] = 0;

		return 0;
	} else {
		return it->second;
	}
}


void TemplateLangExecEnv::setCounter(const std::string& counter_name, long long value)
{
	m_counters[counter_name] = value;
}


void TemplateLangExecEnv::incCounter(const std::string& counter_name, long long value)
{
	CounterMap::iterator it = m_counters.find(counter_name);
	if (it == m_counters.end()) {
		m_counters[counter_name] = value; // We start at 0, so after incrementing, 0 + value == value.
	} else {
		it->second += value;
	}

}

void TemplateLangExecEnv::incCounter(const std::string& counter_name, const std::string& variable_name)
{
	std::string varValue = getVar(variable_name);

	long long value = string2longlong(varValue);

	CounterMap::iterator it = m_counters.find(counter_name);
	if (it == m_counters.end()) {
		m_counters[counter_name] = value; // We start at 0, so after incrementing, 0 + value == value.
	} else {
		it->second += value;
	}
}



void TemplateLangExecEnv::decCounter(const std::string& counter_name, long long value)
{
	CounterMap::iterator it = m_counters.find(counter_name);
	if (it == m_counters.end()) {
		m_counters[counter_name] = -1 * value; // We start at 0, so after decrementing, 0 - value == -value.
	} else {
		it->second -= value;
	}
}

void TemplateLangExecEnv::decCounter(const std::string& counter_name, const std::string& variable_name)
{
	std::string varValue = getVar(variable_name);

	long long value = string2longlong(varValue);

	CounterMap::iterator it = m_counters.find(counter_name);
	if (it == m_counters.end()) {
		m_counters[counter_name] = -1 * value; // We start at 0, so after decrementing, 0 - value == -value.
	} else {
		it->second -= value;
	}
}


void TemplateLangExecEnv::enterListMode(const std::string& list_name, std::string *pStrDelimiter)
{
	if (pStrDelimiter == 0) {
		pushEmitTargetStack(kETList);

		if (m_delimited_lists.find(list_name) == m_lists.end()) {
			m_delimited_lists.insert(std::make_pair(list_name, std::list<std::string>()));
		}
	} else {
		pushEmitTargetStack(kETDelimitedList);
	}

	m_list_stack.push_back(list_name);

	m_list_delimiter_stack.push_back(pStrDelimiter);

	// Always add this, since we will be extending it at the
	// listappendend template tag.
	if (m_lists.find(list_name) == m_lists.end()) {
		m_lists.insert(std::make_pair(list_name, std::list<std::string>()));
	}
}


void TemplateLangExecEnv::exitListMode()
{
	eEmitTarget topEmitTarget = popEmitTargetStack();
	if (topEmitTarget == kETDelimitedList) {
		std::string list_name = *m_list_stack.rbegin();
		std::string *pStrDelimiter = *m_list_delimiter_stack.rbegin();
		if (pStrDelimiter == 0) {
			// Something wrong!
		} else {
			std::string list_value_before_splitting;
			
			StringListMap::const_iterator ci = m_delimited_lists.find(list_name);
			if (ci == m_delimited_lists.end()) {
				addToError("Error: Could not find delimited list '" + list_name);
			} else {
				list_value_before_splitting = joinListEmptyBetween(ci->second);
				std::list<std::string> list_after_splitting;
				if (pStrDelimiter->empty()) {
					m_lists[list_name].push_back(list_value_before_splitting);
				} else {
					std::list<std::string> value_list;
					split_string_on_string(list_value_before_splitting, *pStrDelimiter, value_list);
					std::list<std::string>::const_iterator ci2 = value_list.begin();
					std::list<std::string>::const_iterator cend2 = value_list.end();
					while (ci2 != cend2) {
						m_lists[list_name].push_back(*ci2);
						++ci2;
					}
				}
			}

			// Clear the delimited list, thus preparing
			// for the next listappendbegin.
			m_delimited_lists[list_name].clear();
		}
	}
	m_list_delimiter_stack.pop_back();
	m_list_stack.pop_back();
}

void TemplateLangExecEnv::enterNamedSetAddMode(const std::string& named_set_name, const std::string& delimiter)
{
	pushEmitTargetStack(kETNamedSet);
	m_named_set_name_stack.push_back(named_set_name);
	m_named_set_delimiter_stack.push_back(delimiter);
	createNamedSetIfNotExists(named_set_name);
}


void TemplateLangExecEnv::exitNamedSetAddMode()
{
	bool bAdd = true;
	exitNamedSetAddOrRemoveMode(bAdd);
}

void TemplateLangExecEnv::enterNamedSetRemoveMode(const std::string& named_set_name, const std::string& delimiter)
{
	pushEmitTargetStack(kETNamedSet);
	m_named_set_name_stack.push_back(named_set_name);
	m_named_set_delimiter_stack.push_back(delimiter);
	createNamedSetIfNotExists(named_set_name);
}


void TemplateLangExecEnv::exitNamedSetRemoveMode()
{
	bool bAdd = false;
	exitNamedSetAddOrRemoveMode(bAdd);
}

void TemplateLangExecEnv::exitNamedSetAddOrRemoveMode(bool bAdd)
{
	popEmitTargetStack();
	std::string named_set_name = *m_named_set_name_stack.rbegin();
	m_named_set_name_stack.pop_back();
	std::string delimiter = *m_named_set_delimiter_stack.rbegin();
	m_named_set_delimiter_stack.pop_back();

	std::string value;
	VariableMap::iterator it = m_sets_under_construction.find(named_set_name);
	if (it != m_sets_under_construction.end()) {
		it->second->toString(value);
		it->second->clear();
	}

	std::list<std::string> value_list;
	if (!value.empty()) {
		if (delimiter.empty()) {
			value_list.push_back(value);
		} else {
			split_string_on_string(value, delimiter, value_list);
		}

		std::list<std::string>::const_iterator value_list_ci = value_list.begin();
		std::list<std::string>::const_iterator value_list_cend = value_list.end();
		while (value_list_ci != value_list_cend) {
			std::string set_member = *value_list_ci++;
			if (bAdd) {
				m_named_sets[named_set_name].insert(set_member);
			} else {
				m_named_sets[named_set_name].erase(set_member);
			}
		}
	}
	
}

void TemplateLangExecEnv::clearList(const std::string& list_name)
{
	StringListMap::iterator it = m_lists.find(list_name);
	if (it == m_lists.end()) {
		addToError("Error: Could not clear list '" + list_name + "', list not found.");
	} else {
		it->second.clear();
	}
}

void TemplateLangExecEnv::clearNamedSet(const std::string& named_set_name)
{
	StringNamedSetMap::iterator it = m_named_sets.find(named_set_name);
	if (it == m_named_sets.end()) {
		// Nothing to do.
	} else {
		it->second.clear();
	}
	VariableMap::iterator ucit = m_sets_under_construction.find(named_set_name);
	if (ucit == m_sets_under_construction.end()) {
		// Nothing to do
	} else {
		ucit->second->clear();
	}
}

void TemplateLangExecEnv::emitList(const std::string& list_name, const std::string& separator)
{
	if (m_nixingType == kTNTAll) {
		// Nothing to do... we are nixing
	} else {
		StringListMap::const_iterator ci = m_lists.find(list_name);
		if (ci == m_lists.end()) {
			addToError("Error: Could not emit list '" + list_name + "' with separator '" + separator + "'");
			
		} else {
			Bigstring value;
			// joinList and joinListEmptyBetween
			// are defined in <string_func.h>.

			if (separator.empty()) {
				joinListEmptyBetweenInBigstring(&value, ci->second);
			} else {
				joinListInBigstring(&value, separator, ci->second);
			}

			addBigstringToOutput(&value);
		}
	}
}

void TemplateLangExecEnv::emitNamedSet(const std::string& named_set_name, const std::string& separator)
{
	if (m_nixingType == kTNTAll) {
		// Nothing to do... we are nixing
	} else {
		StringNamedSetMap::const_iterator ci = m_named_sets.find(named_set_name);
		if (ci == m_named_sets.end()) {
			addToError("Error: Could not emit named_set '" + named_set_name + "' with separator '" + separator + "'");
			
		} else {
			Bigstring value;
			// joinList and joinListEmptyBetween
			// are defined in <string_func.h>.
			std::list<std::string> set_as_list;
			std::set<std::string>::const_iterator set_value_ci = ci->second.begin();
			std::set<std::string>::const_iterator set_value_cend = ci->second.end();
			while (set_value_ci != set_value_cend) {
				set_as_list.push_back(*set_value_ci++);
			}
			if (separator.empty()) {
				joinListEmptyBetweenInBigstring(&value, set_as_list);
			} else {
				joinListInBigstring(&value, separator, set_as_list);
			}

			addBigstringToOutput(&value);
		}
	}
}

void TemplateLangExecEnv::enterSetVarMode(const std::string& var_name)
{
	createVarIfNotExists(var_name);
	m_strCurrentSetVar = var_name;
	m_variables[var_name]->clear();
	pushEmitTargetStack(kETVar);
}

void TemplateLangExecEnv::exitSetVarMode()
{
	popEmitTargetStack();
	m_strCurrentSetVar = "";
}

void TemplateLangExecEnv::createVarIfNotExists(const std::string& var_name)
{
	if (m_variables.find(var_name) == m_variables.end()) {
		m_variables.insert(std::make_pair(var_name, new Bigstring(4096)));
	} 
}

void TemplateLangExecEnv::createNamedSetIfNotExists(const std::string& named_set_name)
{
	if (m_sets_under_construction.find(named_set_name) == m_sets_under_construction.end()) {
		m_sets_under_construction.insert(std::make_pair(named_set_name, new Bigstring(4096)));
	}
	
	if (m_named_sets.find(named_set_name) == m_named_sets.end()) {
		m_named_sets.insert(std::make_pair(named_set_name, std::set<std::string>()));
	}
}

std::string TemplateLangExecEnv::getVar(const std::string& var_name)
{
	createVarIfNotExists(var_name);
	return m_variables[var_name]->toString();
}

void TemplateLangExecEnv::emitVar(const std::string& var_name)
{
	if (m_nixingType == kTNTAll) {
		// Nothing to do... we are nixing
	} else {
		createVarIfNotExists(var_name);

		addBigstringToOutput(m_variables[var_name]);
	}
}




/////////////////////////////////////////////////////////////////
//
// parser
//
/////////////////////////////////////////////////////////////////

Token *TemplateLangnewToken()
{
	return newToken();
}

int ttlex(void *lvalp, void *parm)
{
	return ((TemplateLangExecEnv*) parm)->pLexer->scan((Token*) lvalp);
}


// Prototypes
extern void *TemplateLangParserAlloc(void *(*mallocProc)(size_t));
extern void TemplateLangParserFree(void *p, void (*freeProc)(void*));
extern void TemplateLangParser(
		      void *yyp,                       /* The parser */
		      int yymajor,                     /* The major token code number */
		      Token *yyminor,                  /* The value for the token */
		      TemplateLangExecEnv *pArg /* optional argument */
		      );



int ttparse(TemplateLangExecEnv *pEE)
{
	void *pParser;
	Token *pToken;
	int hTokenId;

	pEE->bSyntaxError = false;
	pParser = TemplateLangParserAlloc( malloc );
	pToken = TemplateLangnewToken();
	while((hTokenId = ttlex(pToken, pEE))
	      && !pEE->bSyntaxError) {
		TemplateLangParser(pParser, hTokenId, pToken, pEE);
		pToken = TemplateLangnewToken();
	}
	TemplateLangParser(pParser, 0, pToken, pEE);
	TemplateLangParserFree(pParser, free );
	pToken->clear();
	delete pToken;

	if (pEE->bSyntaxError) {
		return 1;
	} else {
		return 0;
	}
}




TemplateASTNode *parseTemplate(TemplateLangExecEnv* pEE, const std::string& template_string, bool& bResult)
{
	pEE->clean();

	pEE->pLexer = new TemplateLanguageScanner(template_string);

	// Clear error
	pEE->clearError();

	// Declare return_value
	TemplateASTNode *pTemplateLangResult = 0;
  
	// Initialize bResult
	bResult = true;

	// Parser
	int iResult = ttparse(pEE);

	// Did parsing fail?
	if (iResult == 1) {
		bResult = false;
		pEE->prependError("Parsing failed\n");
	} else if (iResult == 2) {
		bResult = false;
		pEE->prependError("Parsing stack overflow. Parsing failed.\n");
	}

	// Was the parsing a success?
	if (bResult) {
		// If so, execute statements
		pEE->simplifyAST();
		pTemplateLangResult = pEE->extractTopNode();
	} else {
		pTemplateLangResult = 0;
	}

	// Return
	return pTemplateLangResult;
}

