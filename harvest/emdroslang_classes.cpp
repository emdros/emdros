/*
 * emdroslang_classes.cpp
 *
 * Emdros language classes
 *
 * Ulrik Sandborg-Petersen
 * Created: 11/11-2016
 * Last update: 5/23-2017
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2016-2017  Ulrik Sandborg-Petersen
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

#include "string_func.h"
#include "emdroslang_classes.h"
#include "emdroslangpars.h"
#include "memobject.h"

//////////////////////////////////////////////////////////////////
//
// ELASTNode
//
//////////////////////////////////////////////////////////////////


ELASTNode::ELASTNode()
{
	m_pNext = 0;
}


ELASTNode::~ELASTNode()
{
	delete m_pNext;
}

ELASTNode *ELASTNode::getTail()
{
	ELASTNode *pTail = this;
	while (pTail->m_pNext != 0) {
		pTail = pTail->m_pNext;
	}
	return pTail;
}

bool ELASTNode::weed(EmdroslangExecEnv *pEE, bool& bResult)
{
	if (m_pNext) {
		return m_pNext->weed(pEE, bResult);
	} else {
		return true;
	}
}


bool ELASTNode::symbol(EmdroslangExecEnv *pEE, bool& bResult)
{
	if (m_pNext) {
		return m_pNext->symbol(pEE, bResult);
	} else {
		return true;
	}
}


bool ELASTNode::type(EmdroslangExecEnv *pEE, bool& bResult)
{
	if (m_pNext) {
		return m_pNext->type(pEE, bResult);
	} else {
		return true;
	}
}


bool ELASTNode::exec(EmdroslangExecEnv *pEE, bool& bResult)
{
	if (m_pNext) {
		return m_pNext->exec(pEE, bResult);
	} else {
		return true;
	}
}


//////////////////////////////////////////////////////////////////
//
// ELStatement
//
//////////////////////////////////////////////////////////////////


ELStatement::ELStatement()
	: ELASTNode()
{
}

ELStatement::~ELStatement()
{
}

bool ELStatement::weed(EmdroslangExecEnv *pEE, bool& bResult)
{
	return ELASTNode::weed(pEE, bResult);
}

bool ELStatement::symbol(EmdroslangExecEnv *pEE, bool& bResult)
{
	return ELASTNode::symbol(pEE, bResult);
}

bool ELStatement::type(EmdroslangExecEnv *pEE, bool& bResult)
{
	return ELASTNode::type(pEE, bResult);
}

bool ELStatement::exec(EmdroslangExecEnv *pEE, bool& bResult)
{
	return ELASTNode::exec(pEE, bResult);
}


//////////////////////////////////////////////////////////////////
//
// ELFullyQualifiedFunctionName
//
//////////////////////////////////////////////////////////////////
ELFullyQualifiedFunctionName::ELFullyQualifiedFunctionName(const std::string& stylesheet_name,
							   const std::string& object_type_name,
							   const std::string& function_name)
	: ELASTNode(),
	  m_stylesheet_name(stylesheet_name),
	  m_object_type_name(object_type_name),
	  m_function_name(function_name)
{
	m_whole_name = makeWholeName();
}

ELFullyQualifiedFunctionName::~ELFullyQualifiedFunctionName()
{
}


bool ELFullyQualifiedFunctionName::isObjectTypeName() const
{
	if (m_stylesheet_name.empty()) {
		return false;
	} else {
		if (m_object_type_name.empty()) {
			return false;
		} else {
			if (m_function_name == "start"
			    || m_function_name == "end") {
				return true;
			} else {
				return false;
			}
		}
	}
}


std::string ELFullyQualifiedFunctionName::getStylesheetName() const
{
	return m_stylesheet_name;
}


std::string ELFullyQualifiedFunctionName::getObjectTypeName() const
{
	return m_object_type_name;
}



std::string ELFullyQualifiedFunctionName::getWholeName() const
{
	return m_whole_name;
}

std::string ELFullyQualifiedFunctionName::makeWholeName() const
{
	std::string result;
	bool bStylesheetNameIsEmpty = m_stylesheet_name.empty();
	bool bObjectTypeNameIsEmpty = m_object_type_name.empty();
	if (bStylesheetNameIsEmpty) {
		// If m_stylesheet_name is empty, then also ingore the
		// object type name.  It should also be empty, as per
		// the grammar.
		result = "::" + m_function_name;
	} else {
		// m_stylesheet_name is not empty.
		if (bObjectTypeNameIsEmpty) {
			result = m_stylesheet_name + "::" + m_function_name;
		} else {
			result = m_stylesheet_name + "::" + m_object_type_name + "::" + m_function_name;
		}
	}
	return result;
}


bool ELFullyQualifiedFunctionName::weed(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	bool bDBOK = true;
	if (!m_stylesheet_name.empty()
	    && !m_object_type_name.empty()) {
		if (m_function_name != "start"
		    && m_function_name != "end") {
			bResult = false;
			pEE->appendCompilerError("function name '" + m_whole_name + "' has an object type, but the function name is neither 'start' nor 'end'.");
		}
	}
	return bDBOK;
}



//////////////////////////////////////////////////////////////////
//
// ELExpression
//
//////////////////////////////////////////////////////////////////


std::string ELOperation2String(eELOperation operation)
{
	switch (operation) {
	case kOpNot:
		return "!";
		break;
	case kOpOr:
		return "||";
		break;
	case kOpAnd:
		return "&&";
		break;
	case kOpPlus:
		return "+";
		break;
	case kOpMinus:
		return "-";
		break;
	case kOpStar:
		return "*";
		break;
	case kOpSlash:
		return "/";
		break;
	case kOpPercent:
		return "%";
		break;
	case kOpEqEq:
		return "==";
		break;
	case kOpNeq:
		return "!=";
		break;
	case kOpLt:
		return "<";
		break;
	case kOpLeq:
		return "<=";
		break;
	case kOpGt:
		return ">";
		break;
	case kOpGeq:
		return ">=";
		break;
	case kOpLookup:
		return "lookup";
		break;
	case kOpSlice:
		return "slice";
		break;
	case kOpNA:
		return "N/A";
		break;
	}
	return "";
}


ELExpression::ELExpression(JSONValue *pLiteral)
	: ELASTNode(),
	  m_pLiteral(pLiteral),
	  m_pVarName(0),
	  m_pFQFunctionName(0),
	  m_pLHS(0),
	  m_operation(kOpNA),
	  m_pRHS(0),
	  m_result_type(kJSONNull),
	  m_expression_kind(kELEKLiteral)
{
}

ELExpression::ELExpression(const std::string& var_name)
	: ELASTNode(),
	  m_pLiteral(0),
	  m_pVarName(0),
	  m_pFQFunctionName(0),
	  m_pLHS(0),
	  m_operation(kOpNA),
	  m_pRHS(0),
	  m_result_type(kJSONNull),
	  m_expression_kind(kELEKVarName)
{
	m_pVarName = new std::string(var_name);
}

ELExpression::ELExpression(ELExpression *pLHS, eELOperation binary_operation, ELExpression *pRHS)
	: ELASTNode(),
	  m_pLiteral(0),
	  m_pVarName(0),
	  m_pFQFunctionName(0),
	  m_pLHS(pLHS),
	  m_operation(binary_operation),
	  m_pRHS(pRHS),
	  m_result_type(kJSONNull),
	  m_expression_kind(kELEKBinaryOperation)
{
}

ELExpression::ELExpression(eELOperation unary_operation, ELExpression *pOperand)
	: ELASTNode(),
	  m_pLiteral(0),
	  m_pVarName(0),
	  m_pFQFunctionName(0),
	  m_pLHS(pOperand),
	  m_operation(unary_operation),
	  m_pRHS(0),
	  m_result_type(kJSONNull),
	  m_expression_kind(kELEKUnaryOperation)
{
}

ELExpression::ELExpression(ELFullyQualifiedFunctionName *pFQFunctionName, ELExpression *pParamList)
	: ELASTNode(),
	  m_pLiteral(0),
	  m_pVarName(0),
	  m_pFQFunctionName(pFQFunctionName),
	  m_pLHS(0),
	  m_operation(kOpNA),
	  m_pRHS(pParamList),
	  m_result_type(kJSONNull),
	  m_expression_kind(kELEKFunctionCall)
{
}


ELExpression::~ELExpression()
{
	delete m_pRHS;
	delete m_pLHS;
	delete m_pFQFunctionName;
	delete m_pVarName;
	delete m_pLiteral;
}

JSONValue *ELExpression::eval(EmdroslangExecEnv *pEE, bool& bResult)
{
	switch (m_expression_kind) {
	case kELEKLiteral:
		bResult = true;
		return new JSONValue(*m_pLiteral);
		break;
	case kELEKVarName:
		return pEE->evalVarName(*m_pVarName, bResult);
		break;
	case kELEKBinaryOperation:
		return this->evalBinaryOperation(pEE, bResult);
		break;
	case kELEKUnaryOperation:
		return this->evalUnaryOperation(pEE, bResult);
		break;
	case kELEKFunctionCall:
		return this->evalFunctionCall(pEE, bResult);
		break;
	}

	bResult = false;
	return new JSONValue();
}

JSONValue *ELExpression::evalBinaryOpPlus(EmdroslangExecEnv *pEE, JSONValue *pLHSValue, JSONValue *pRHSValue, bool& bResult)
{
	bResult = true;
	JSONValue *pResult = 0;
			
	switch (pLHSValue->getKind()) {
	case kJSONNull:
		pResult = new JSONValue(*pRHSValue);
		break;
	case kJSONBoolean:
		{
			bool bLeft = pLHSValue->getBoolean();
			bool bRight = pRHSValue->castToBool();
			if ((bLeft && bRight)
			    || (!bLeft && !bRight)) {
				pResult = new JSONValue(false);
			} else {
				pResult = new JSONValue(true);
			}
		}
		break;
	case kJSONInteger:
		pResult = new JSONValue(pLHSValue->getInteger() + pRHSValue->castToInteger());
		break;
	case kJSONString:
		pResult = new JSONValue(new std::string(pLHSValue->getString() + pRHSValue->castToString()));
		break;
	case kJSONList:
		{
			pResult = new JSONValue(*pLHSValue); // Initial copy
			
			switch (pRHSValue->getKind()) {
			case kJSONNull: 
			case kJSONBoolean:
			case kJSONInteger:
			case kJSONString:	
				pResult->append(new JSONValue(*pRHSValue));
				break;
			case kJSONList:
				{
					const std::list<JSONValue*>& RHSList = pRHSValue->getList();
					std::list<JSONValue*>::const_iterator ci2 = RHSList.begin();
					std::list<JSONValue*>::const_iterator cend2 = RHSList.end();
					while (ci2 != cend2) {
						pResult->append(new JSONValue(*ci2));
						
						++ci2;
					}
				}
				break;
			case kJSONObject:
				{
					std::list<std::string> RHSKeysList = pRHSValue->getObjectKeys();
					std::list<std::string>::const_iterator ci3 = RHSKeysList.begin();
					std::list<std::string>::const_iterator cend3 = RHSKeysList.end();
					while (ci3 != cend3) {
						std::string key = *ci3;
						
						JSONValue *pValue = new JSONValue(pRHSValue->getObjectValue(key));
						
						JSONListElement *pKeyValueList = new JSONListElement(pValue);
						pKeyValueList->setNext(new JSONListElement(new JSONValue(new std::string(key))));
						JSONValue *pListValue = new JSONValue(pKeyValueList);
						
						pResult->append(pListValue);
						
						++ci3;
					}
				}
				break;
			}
		}
		break;
	case kJSONObject:
		{
			pResult = new JSONValue(*pLHSValue); // Initial copy
			
			switch (pRHSValue->getKind()) {
			case kJSONNull: 
			case kJSONBoolean:
			case kJSONInteger:
			case kJSONString:	
			case kJSONList:
				// Undefined operation.
				bResult = false;
				pEE->appendRuntimeError("Unable to evaluate operator + with lhs being a JSON object and rhs not being a JSON object");
				break;
			case kJSONObject:
				{
					std::list<std::string> RHSKeysList = pRHSValue->getObjectKeys();
					std::list<std::string>::const_iterator ci3 = RHSKeysList.begin();
					std::list<std::string>::const_iterator cend3 = RHSKeysList.end();
					while (ci3 != cend3) {
						std::string key = *ci3;
						
						JSONValue *pValue = new JSONValue(pRHSValue->getObjectValue(key));
						
						pResult->addKeyValue(key, pValue);
						
						++ci3;
					}
				}
				break;
			}
		}
		break;
	}

	return pResult;
}

JSONValue *ELExpression::evalBinaryOpMinus(EmdroslangExecEnv *pEE, JSONValue *pLHSValue, JSONValue *pRHSValue, bool& bResult)
{
	bResult = true;
	JSONValue *pResult = 0;
			
	switch (pLHSValue->getKind()) {
	case kJSONNull:
		pResult = new JSONValue(*pRHSValue);
		break;
	case kJSONBoolean:
		{
			bool bLeft = pLHSValue->getBoolean();
			bool bRight = pRHSValue->castToBool();
			if (bLeft && !bRight) {
				pResult = new JSONValue(true);
			} else {
				pResult = new JSONValue(false);
			}
		}
		break;
	case kJSONInteger:
		pResult = new JSONValue(pLHSValue->getInteger() - pRHSValue->castToInteger());
		break;
	case kJSONString:
		{
			std::string lhs_string = pLHSValue->getString();
			std::string rhs_string = pRHSValue->castToString();
			std::string::size_type lhs_string_length = lhs_string.length();
			std::string::size_type rhs_string_length = rhs_string.length();
			if (lhs_string_length >= rhs_string_length) {
				std::string::size_type rhs_index = lhs_string_length - rhs_string_length;
				if (lhs_string.substr(rhs_index, std::string::npos) == rhs_string) {
					pResult = new JSONValue(new std::string(lhs_string.substr(0, rhs_index)));
				} else {
					pResult = new JSONValue(new std::string(lhs_string));
				}
			} else {
				pResult = new JSONValue(new std::string(lhs_string));
			}
		}
		break;
	case kJSONList:
		{
			// Undefined operation
			bResult = false;
			pEE->appendRuntimeError("Cannot subtract from a list.");
			pResult = 0;
		}
		break;
	case kJSONObject:
		{
			switch (pRHSValue->getKind()) {
			case kJSONNull: 
			case kJSONBoolean:
			case kJSONInteger:
			case kJSONString:	
			case kJSONList:
				// Undefined operation.
				bResult = false;
				pEE->appendRuntimeError("Unable to evaluate operator - with lhs being a JSON object and rhs not being a JSON object");
				pResult = 0;
				break;
			case kJSONObject:
				{
					pResult = new JSONValue(*pLHSValue); // Initial copy
			
					std::list<std::string> RHSKeysList = pRHSValue->getObjectKeys();
					std::list<std::string>::const_iterator ci3 = RHSKeysList.begin();
					std::list<std::string>::const_iterator cend3 = RHSKeysList.end();
					while (ci3 != cend3) {
						std::string key = *ci3;
						
						JSONValue *pValue = new JSONValue(pRHSValue->getObjectValue(key));
						
						pResult->addKeyValue(key, pValue);
						
						++ci3;
					}
				}
				break;
			}
		}
		break;
	}

	return pResult;
}

JSONValue *ELExpression::evalBinaryOpStar(EmdroslangExecEnv *pEE, JSONValue *pLHSValue, JSONValue *pRHSValue, bool& bResult)
{
	bResult = true;
	JSONValue *pResult = 0;
			
	switch (pLHSValue->getKind()) {
	case kJSONNull:
		{
			// Undefined operation
			bResult = false;
			pEE->appendRuntimeError("Cannot apply * to a Left Hand Side null value.");
			pResult = 0;
		}
		break;
	case kJSONBoolean:
		{
			bool bLeft = pLHSValue->getBoolean();
			bool bRight = pRHSValue->castToBool();
			if (bLeft && !bRight) {
				pResult = new JSONValue(true);
			} else {
				pResult = new JSONValue(false);
			}
		}
		break;
	case kJSONInteger:
		pResult = new JSONValue(pLHSValue->getInteger() * pRHSValue->castToInteger());
		break;
	case kJSONString:
		{
			// Undefined operation
			bResult = false;
			pEE->appendRuntimeError("Cannot apply * to a Left Hand Side string value.");
			pResult = 0;
		}
		break;
	case kJSONList:
		{
			switch (pRHSValue->getKind()) {
			case kJSONInteger:
				{
					JSONListElement *pEmptyListElement = 0;
					pResult = new JSONValue(pEmptyListElement);

					long long count = pRHSValue->getInteger();
					long long index = 0;
					// Extend "count" times with pLHSValue's list
					while (index < count) {
						// Extend with pLHSValue's list
						const std::list<JSONValue*>& LHSList = pLHSValue->getList();
						std::list<JSONValue*>::const_iterator ci = LHSList.begin();
						std::list<JSONValue*>::const_iterator cend = LHSList.end();
						while (ci != cend) {
							pResult->append(new JSONValue(*ci));
							++ci;
						}
						++index;
					}
				}
				break;
			default:
				// Undefined operation
				bResult = false;
				pEE->appendRuntimeError("Cannot apply * to a LHS list value and a non-integer RHS value.");
				pResult = 0;
				break;
			}
		}
		break;
	case kJSONObject:
		{
			// Undefined operation
			bResult = false;
			pEE->appendRuntimeError("Cannot apply * to a Left Hand Side JSON Object value.");
			pResult = 0;
		}
		break;
	}

	return pResult;
}

JSONValue *ELExpression::evalBinaryOpSlash(EmdroslangExecEnv *pEE, JSONValue *pLHSValue, JSONValue *pRHSValue, bool& bResult)
{
	bResult = true;
	JSONValue *pResult = 0;
			
	switch (pLHSValue->getKind()) {
	case kJSONNull:
		{
			// Undefined operation
			bResult = false;
			pEE->appendRuntimeError("Cannot apply / to a Left Hand Side null value.");
			pResult = 0;
		}
		break;
	case kJSONBoolean:
		{
			// Undefined operation
			bResult = false;
			pEE->appendRuntimeError("Cannot apply / to a Left Hand Side boolean value.");
			pResult = 0;
		}
		break;
	case kJSONInteger:
		{
			long long RHSInteger = pRHSValue->castToInteger();
			if (RHSInteger == 0L) {
				// Undefined operation
				bResult = false;
				pEE->appendRuntimeError("Cannot apply / to a LHS number and a RHS zero.");
			pResult = 0;
			} else {
				pResult = new JSONValue(pLHSValue->getInteger() / RHSInteger);
			}
		}
		break;
	case kJSONString:
		{
			// Undefined operation
			bResult = false;
			pEE->appendRuntimeError("Cannot apply / to a Left Hand Side string value.");
			pResult = 0;
		}
		break;
	case kJSONList:
		{
			// Undefined operation
			bResult = false;
			pEE->appendRuntimeError("Cannot apply / to a LHS list value.");
			pResult = 0;
		}
		break;
	case kJSONObject:
		{
			// Undefined operation
			bResult = false;
			pEE->appendRuntimeError("Cannot apply / to a Left Hand Side JSON Object value.");
			pResult = 0;
		}
		break;
	}

	return pResult;
}

JSONValue *ELExpression::evalBinaryOpPercent(EmdroslangExecEnv *pEE, JSONValue *pLHSValue, JSONValue *pRHSValue, bool& bResult)
{
	bResult = true;
	JSONValue *pResult = 0;
			
	switch (pLHSValue->getKind()) {
	case kJSONNull:
		{
			// Undefined operation
			bResult = false;
			pEE->appendRuntimeError("Cannot apply % to a Left Hand Side null value.");
			pResult = 0;
		}
		break;
	case kJSONBoolean:
		{
			// Undefined operation
			bResult = false;
			pEE->appendRuntimeError("Cannot apply % to a Left Hand Side boolean value.");
			pResult = 0;
		}
		break;
	case kJSONInteger:
		{
			long long RHSInteger = pRHSValue->castToInteger();
			if (RHSInteger == 0L) {
				// Undefined operation
				bResult = false;
				pEE->appendRuntimeError("Cannot apply % to a LHS number and a RHS zero.");
			pResult = 0;
			} else {
				pResult = new JSONValue(pLHSValue->getInteger() % RHSInteger);
			}
		}
		break;
	case kJSONString:
		{
			std::string LHSString = pLHSValue->getString();
			std::string error_message;
			std::string result_string = this->applyPercentToString(LHSString, pRHSValue, bResult, error_message);
			if (bResult) {
				pResult = new JSONValue(new std::string(result_string));
			} else {
				bResult = false;
				pEE->appendRuntimeError(error_message);
				pResult = 0;
			}
		}
		break;
	case kJSONList:
		{
			// Undefined operation
			bResult = false;
			pEE->appendRuntimeError("Cannot apply % to a LHS list value.");
			pResult = 0;
		}
		break;
	case kJSONObject:
		{
			// Undefined operation
			bResult = false;
			pEE->appendRuntimeError("Cannot apply % to a Left Hand Side JSON Object value.");
			pResult = 0;
		}
		break;
	}

	return pResult;
}

JSONValue *ELExpression::evalBinaryOperation(EmdroslangExecEnv *pEE, bool& bResult)
{
	JSONValue *pLHSValue = m_pLHS->eval(pEE, bResult);
	JSONValue *pRHSValue = 0;
	JSONValue *pResult = 0;
	if (bResult) {
		pRHSValue = m_pRHS->eval(pEE, bResult);
	}
	if (!bResult) {
		delete pLHSValue;
		delete pRHSValue;
		return new JSONValue(); // null c'tor
	} else {
		switch (m_operation) {
		case kOpNot:
		case kOpLeq:
		case kOpNA:
			ASSERT_THROW(false, "Emdros implementation logic error: Operation '" + ELOperation2String(m_operation) + "' should not be found here.");
			break;
		case kOpOr:
			if (pLHSValue->castToBool()) {
				pResult = new JSONValue(true);
			} else if (pRHSValue->castToBool()) {
				pResult = new JSONValue(true);
			} else {
				pResult = new JSONValue(false);
			}
			break;
		case kOpAnd:
			if (pLHSValue->castToBool()) {
				if (pRHSValue->castToBool()) {
					pResult = new JSONValue(true);
				} else {
					pResult = new JSONValue(false);
				}
			} else {
				pResult = new JSONValue(false);
			}
			break;
		case kOpPlus:
			pResult = this->evalBinaryOpPlus(pEE, pLHSValue, pRHSValue, bResult);	
			break;
		case kOpMinus:
			pResult = this->evalBinaryOpMinus(pEE, pLHSValue, pRHSValue, bResult);	
			break;
		case kOpStar:
			pResult = this->evalBinaryOpStar(pEE, pLHSValue, pRHSValue, bResult);	
			break;
		case kOpSlash:
			pResult = this->evalBinaryOpSlash(pEE, pLHSValue, pRHSValue, bResult);	
			break;
		case kOpPercent:
			pResult = this->evalBinaryOpPercent(pEE, pLHSValue, pRHSValue, bResult);	
			break;
		case kOpEqEq:
		case kOpNeq:
		case kOpLt:
		case kOpGt:
		case kOpGeq:
		case kOpLookup:
		case kOpSlice:
			bResult = false;
			pEE->appendRuntimeError("Binary operator " + ELOperation2String(m_operation) + " not yet implemented.");
			pResult = 0; // FIXME: Implement!
			break;
		}

		delete pLHSValue;
		delete pRHSValue;
		if (!bResult) {
			delete pResult;
			return new JSONValue(); // null c'tor
		} else {
			return pResult;
		}
	}
}



std::string applyPercentToString(const std::string& LHSString, JSONValue *pRHSValue, bool& bResult, std::string& error_message)
{
	bResult = true;
	
	std::stringstream sout;
	std::string::const_iterator ci = LHSString.begin();
	std::string::const_iterator cend = LHSString.end();
	eJSONValueKind RHSKind = pRHSValue->getKind();
	std::list<JSONValue*> RHSList;
	bool bIsObject = false;
	switch (RHSKind) {
	case kJSONNull:
	case kJSONBoolean:
	case kJSONInteger:
	case kJSONString:
		bIsObject = false;
		RHSList.push_back(pRHSValue);
		break;
	case kJSONList:
		bIsObject = false;
		RHSList = pRHSValue->getList();
		break;
	case kJSONObject:
		bIsObject = true;
		break;
	}
	std::list<JSONValue*>::iterator json_value_it;
	std::list<JSONValue*>::iterator json_value_end;
	if (!bIsObject) {
		json_value_it = RHSList.begin();
		json_value_end = RHSList.end();
	}

	while (ci != cend
	       && bResult) {
		char c = *ci;
		if (c == '%') {
			bool bFound = false;
			std::string prefix;
			std::string object_key;
			char operation = '\0';
			++ci;
			while (ci != cend
			       && !bFound
			       && bResult) {
				c = *ci;
				switch (c) {
				case '%':
				case 'd':
				case 's':
					operation = c;
					bFound = true;
					break;
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					prefix += c;
					break;
				case '(':
					++ci;
					bool bObjectKeyEnded = false;
					while (ci != cend
					       && !bObjectKeyEnded) {
						c = *ci;
						switch (c) {
						case ')':
							bObjectKeyEnded = true;
							break;
						default:
							object_key += c;
							break;
						}
						++ci;
					}
					if (ci == cend) {
						error_message += "Unknown string formatting '%(...' without closing ')'.";
						bResult = false;
					}
					break;
				}
				++ci;
			}
			switch (operation) {
			case '\0':
				error_message += "Missing string formatting operation. Should be either %%, %d, or %s.";
				bResult = false;
				break;
			case '%':
				sout << '%';
				break;
			case 'd':
			case 's':
				JSONValue *pThisValue = 0;
				if (bIsObject) {
					pThisValue = (*pRHSValue)[object_key];
					if (pThisValue == 0) {
						bResult = false;
						error_message += "String formatting %(" + object_key + ")s did not match any key in the RHS object.";
					}
				} else {
					pThisValue = *json_value_it;
					++json_value_it;
				}
				if (bResult) {
					if (operation == 'd') {
						long long integer = pThisValue->castToInteger();
						if (prefix.empty()) {
							sout << longlong2string(integer);
						} else {
							sout << longlong2string_format(integer, prefix);
						}
					} else if (operation == 's') {
						sout << pThisValue->castToString();
					} else {
						bResult = false;
						error_message = "Error in the EmdrosLang implementation logic at line " + int2string(__LINE__);
					}
				}
				break;
			}
		} else {
			sout << c;
			++ci;
		}
	}
	if (bResult) {
		return sout.str();
	} else {
		return "";
	}
}

JSONValue *ELExpression::evalUnaryOperation(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	JSONValue *pResult = 0;
	
	switch (m_operation) {
	case kOpNot:
		{
			JSONValue *pUnnegatedResult = m_pLHS->eval(pEE, bResult);
			if (!bResult) {
				m_result_type = kJSONNull;
				pResult = new JSONValue(); // null c'tor
			} else {
				m_result_type = kJSONBoolean;
				pResult = new JSONValue(!(pUnnegatedResult->castToBool()));
			}
			delete pUnnegatedResult;
		}
		break;
	default:
		bResult = false;
		pResult = 0;
		break;
	}

	return pResult;
}

JSONValue *ELExpression::evalFunctionCall(EmdroslangExecEnv *pEE, bool& bResult)
{
	JSONValue *pResult = pEE->evalFunctionCall(m_pFQFunctionName, m_pRHS, bResult);
	if (!bResult) {
		delete pResult;
		pResult = new JSONValue(); // null c'tor
	}
	return pResult;
}

eJSONValueKind ELExpression::getResultType() const
{
	return m_result_type;
}

bool ELExpression::weed(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	bool bDBOK = true;

	switch (m_expression_kind) {
	case kELEKLiteral:
		break;
	case kELEKVarName:
		// It is first checked at symbol() time
		break;
	case kELEKBinaryOperation:
		bDBOK = this->weedBinaryOperation(pEE, bResult);
		break;
	case kELEKUnaryOperation:
		bDBOK = this->weedUnaryOperation(pEE, bResult);
		break;
	case kELEKFunctionCall:
		bDBOK = this->weedFunctionCall(pEE, bResult);
		break;
	}

	if (bResult && bDBOK) {
		return ELASTNode::weed(pEE, bResult);
	} else {
		return bDBOK;
	}
}

bool ELExpression::weedBinaryOperation(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	
	switch (m_operation) {
	case kOpOr:
	case kOpAnd:
	case kOpPlus:
	case kOpMinus:
	case kOpStar:
	case kOpSlash:
	case kOpPercent:
	case kOpEqEq:
	case kOpNeq:
	case kOpLt:
	case kOpLeq:
	case kOpGt:
	case kOpGeq:
	case kOpLookup:
	case kOpSlice:
		bResult = true;
		break;
	default:
		pEE->appendCompilerError("Unknown binary operator " + ELOperation2String(m_operation));
		bResult = false;
		break;
	}
	if (!bResult) {
		return true;
	}

	if (m_pLHS == 0) {
		pEE->appendCompilerError("Expression on left hand side of binary operator " + ELOperation2String(m_operation) + " is empty");
		bResult = false;
	}
	if (!bResult) {
		return true;
	}
	if (!m_pLHS->weed(pEE, bResult)) {
		return false;
	}
					       
	
	if (m_pRHS == 0) {
		pEE->appendCompilerError("Expression on right hand side of binary operator " + ELOperation2String(m_operation) + " is empty");
		bResult = false;
	}

	if (!bResult) {
		return true;
	}

	if (!m_pLHS->weed(pEE, bResult)) {
		return false;
	}

	if (!bResult) {
		return true;
	}


	return true;
}
       
bool ELExpression::weedUnaryOperation(EmdroslangExecEnv *pEE, bool& bResult)
{
	switch (m_operation) {
	case kOpNot:
		bResult = true;
		break;
	default:
		pEE->appendCompilerError("Unknown unary operator");
		bResult = false;
		break;
	}
	if (m_pLHS == 0) {
		pEE->appendCompilerError("Expression after unary operator is empty");
		bResult = false;
	}
	if (bResult) {
		return m_pLHS->weed(pEE, bResult);
	} else {
		return true;
	}
}

bool ELExpression::weedFunctionCall(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	bool bDBOK = true;
	bDBOK = m_pRHS->weed(pEE, bResult);

	if (!bResult) {
		return bDBOK;
	}

	return bDBOK;
}


bool ELExpression::symbol(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	bool bDBOK = true;
	
	switch (m_expression_kind) {
	case kELEKLiteral:
		break;
	case kELEKVarName:
		bDBOK = pEE->varNameHasBeenDeclared(*m_pVarName);
		break;
	case kELEKBinaryOperation:
		bDBOK = this->symbolBinaryOperation(pEE, bResult);
		break;
	case kELEKUnaryOperation:
		bDBOK = this->symbolUnaryOperation(pEE, bResult);
		break;
	case kELEKFunctionCall:
		bDBOK = this->symbolFunctionCall(pEE, bResult);
		break;
	}

	if (bResult && bDBOK) {
		return ELASTNode::symbol(pEE, bResult);
	} else {
		return bDBOK;
	}
}

bool ELExpression::symbolBinaryOperation(EmdroslangExecEnv *pEE, bool& bResult)
{
	bool bDBOK = m_pLHS->symbol(pEE, bResult);
	if (bDBOK && bResult) {
		bDBOK = m_pRHS->symbol(pEE, bResult);
	}
	return bDBOK;
}
       
bool ELExpression::symbolUnaryOperation(EmdroslangExecEnv *pEE, bool& bResult)
{
	bool bDBOK = m_pLHS->symbol(pEE, bResult);
	return bDBOK;
}

bool ELExpression::symbolFunctionCall(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	bool bDBOK = true;

	bDBOK = pEE->functionHasBeenDeclared(m_pFQFunctionName, bResult);

	if (bDBOK && bResult) {
		bDBOK = m_pRHS->symbol(pEE, bResult);
	}

	if (!bResult) {
		return bDBOK;
	}

	return bDBOK;
}



bool ELExpression::type(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	bool bDBOK = true;

	switch (m_expression_kind) {
	case kELEKLiteral:
		m_result_type = m_pLiteral->getKind();
		break;
	case kELEKVarName:
		break;
	case kELEKBinaryOperation:
		bDBOK = this->typeBinaryOperation(pEE, bResult);
		break;
	case kELEKUnaryOperation:
		bDBOK = this->typeUnaryOperation(pEE, bResult);
		break;
	case kELEKFunctionCall:
		bDBOK = this->typeFunctionCall(pEE, bResult);
		break;
	}

	if (bResult && bDBOK) {
		return ELASTNode::type(pEE, bResult);
	} else {
		return bDBOK;
	}
}

bool ELExpression::typeBinaryOperation(EmdroslangExecEnv *pEE, bool& bResult)
{
	bool bDBOK = m_pLHS->type(pEE, bResult);
	if (bDBOK && bResult) {
		bDBOK = m_pRHS->type(pEE, bResult);
	}
	return bDBOK;
}
       
bool ELExpression::typeUnaryOperation(EmdroslangExecEnv *pEE, bool& bResult)
{
	bool bDBOK = m_pLHS->type(pEE, bResult);
	return bDBOK;
}

bool ELExpression::typeFunctionCall(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	bool bDBOK = true;

	bDBOK = m_pRHS->type(pEE, bResult);

	if (!bResult) {
		return bDBOK;
	}

	return bDBOK;
}


//////////////////////////////////////////////////////////////////
//
// ELCompilationUnitPart
//
//////////////////////////////////////////////////////////////////

ELCompilationUnitPart::ELCompilationUnitPart()
	: ELASTNode()
{
}

ELCompilationUnitPart::~ELCompilationUnitPart()
{
}


//////////////////////////////////////////////////////////////////
//
// ELStylesheet
//
//////////////////////////////////////////////////////////////////
ELStylesheet::ELStylesheet(const std::string& stylesheet_kind,
			   const std::string& stylesheet_name,
			   const std::string& extends,
			   JSONValue *pBody)
	: ELCompilationUnitPart(),
	  m_stylesheet_kind(stylesheet_kind),
	  m_stylesheet_name(stylesheet_name),
	  m_extends(extends),
	  m_pBody(pBody)
{
}


ELStylesheet::~ELStylesheet()
{
	delete m_pBody;
}


bool ELStylesheet::weed(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	bool bDBOK = true;
	if (m_stylesheet_kind != "fetchinfo"
	    && m_stylesheet_kind != "xmlrenderinfo") {
		pEE->appendCompilerError("For stylesheet '" + m_stylesheet_name + "': Unknown stylesheet kind: " + m_stylesheet_kind + ". Should be either 'fetchinfo' or 'xmlrenderinfo'.");
		bResult = false;
	}

	if (!bDBOK || !bResult) {
		return bDBOK;
	}

	bDBOK = this->weedBody(pEE, bResult);

	if (!bDBOK || !bResult) {
		return bDBOK;
	}

	return bDBOK;
}

bool ELStylesheet::weedBody(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	bool bDBOK = true;
	if (m_pBody->getKind() != kJSONObject) {
		bResult = false;
		pEE->appendCompilerError("Stylesheet with name '" + m_stylesheet_name + "' is not defined as a JSON object.");
	}

	if (!bDBOK || !bResult) {
		return bDBOK;
	}

	if (m_stylesheet_kind == "fetchinfo") {
		// Check ["object_order"]
		JSONValue *pInner = (*m_pBody)["object_order"];
		if (pInner == 0) {
			bResult = false;
			pEE->appendCompilerError("Stylesheet with name '" + m_stylesheet_name + "' does not have obligatory key 'object_order'.");
		} else if (pInner->getKind() != kJSONString) {
			bResult = false;
			pEE->appendCompilerError("Stylesheet with name '" + m_stylesheet_name + "' has key 'object_order', but it does not point to a string.");
		} else {
			m_object_order = pInner->getString();
			if (m_object_order != "priority_list"
			    && m_object_order != "docindex") {
				bResult = false;
				pEE->appendCompilerError("Stylesheet with name '" + m_stylesheet_name + "': key 'object_order' should point to either 'priority_list' or 'docindex'");
			}
		}

		if (!bDBOK || !bResult) {
			return bResult;
		}

		// Check ["object_types"]
		pInner = (*m_pBody)["object_types"];
		if (pInner == 0) {
			bResult = false;
			pEE->appendCompilerError("Stylesheet with name '" + m_stylesheet_name + "' does not have obligatory key 'object_types'.");
		} else if (pInner->getKind() != kJSONList) {
			bResult = false;
			pEE->appendCompilerError("Stylesheet with name '" + m_stylesheet_name + "' has key 'object_types', but it does not point to a list.");
		} else {
			std::list<JSONValue*> object_types_list = pInner->getList();
			std::list<JSONValue*>::iterator ot_it = object_types_list.begin();
			std::list<JSONValue*>::iterator ot_end = object_types_list.end();
			while (bResult && ot_it != ot_end) {
				JSONValue *pListMember = *ot_it;
				if (pListMember->getKind() == kJSONString) {
					m_object_types.push_back(pListMember->getString());
				} else {
					bResult = false;
					pEE->appendCompilerError("Stylesheet list " + m_stylesheet_name + "[\"object_types\"] has a non-string member.");
					
				}
				++ot_it;
			}
		}

		if (!bDBOK || !bResult) {
			return bResult;
		}

		// Check ["docindex_feature"]
		pInner = (*m_pBody)["docindex_feature"];
		if (pInner == 0) {
			// This is only an error if m_object_order is "docindex".
			if (m_object_order == "docindex") {
				bResult = false;
				pEE->appendCompilerError("Stylesheet key " + m_stylesheet_name + "[\"object_order\"] ' is 'docindex', yet key 'docindex_feature' is missing. You must declare the docindex_feature");
			}
		} else if (pInner->getKind() != kJSONString) {
			bResult = false;
			pEE->appendCompilerError("Stylesheet key " + m_stylesheet_name + "[\"docindex_feature\"] is present, but it does not point to a string.");
		} else {
			m_docindex_feature = pInner->getString();
			if (m_docindex_feature.empty()) {
				bResult = false;
				pEE->appendCompilerError("Stylesheet key " + m_stylesheet_name + "[\"docindex_feature\"] is present, but it points to an empty string.");
			}
		}

		if (!bDBOK || !bResult) {
			return bResult;
		}
	} else {
		// Check ["elements"]
		const JSONValue *pInner = (*m_pBody)["elements"];
		if (pInner == 0) {
			bResult = false;
			pEE->appendCompilerError("Stylesheet with name '" + m_stylesheet_name + "' does not have obligatory key 'elements'.");
		} else if (pInner->getKind() != kJSONList) {
			bResult = false;
			pEE->appendCompilerError("Stylesheet with name '" + m_stylesheet_name + "' has key 'elements', but it does not point to a list.");
		} else {
			std::list<JSONValue*> elements_list = pInner->getList();
			std::list<JSONValue*>::iterator ot_it = elements_list.begin();
			std::list<JSONValue*>::iterator ot_end = elements_list.end();
			while (bResult && ot_it != ot_end) {
				JSONValue *pListMember = *ot_it;
				if (pListMember->getKind() == kJSONString) {
					m_elements.push_back(pListMember->getString());
				} else {
					bResult = false;
					pEE->appendCompilerError("Stylesheet list " + m_stylesheet_name + "[\"elements\"] has a non-string member.");
					
				}
				++ot_it;
			}
		}

		if (!bDBOK || !bResult) {
			return bResult;
		}
	}
}


bool ELStylesheet::symbol(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	bool bDBOK = true;

	if (m_stylesheet_name.empty()) {
		bResult = false;
		pEE->appendCompilerError("Stylesheet name is empty.");
	}

	if (!bDBOK || !bResult) {
		return bDBOK;
	}

	if (pEE->stylesheetNameHasBeenDeclared(m_stylesheet_name)) {
		bResult = false;
		pEE->appendCompilerError("Stylesheet name '" + m_stylesheet_name + "' has already been declared.");
	}

	if (!bDBOK || !bResult) {
		return bDBOK;
	}

	bDBOK = pEE->declareStylesheet(m_stylesheet_kind, m_stylesheet_name, bResult);

	if (!bDBOK || !bResult) {
		return bDBOK;
	}

	if (m_stylesheet_kind == "fetchinfo") {
		bDBOK = pEE->objectTypesExist(m_object_types, bResult);

		if (!bDBOK || !bResult) {
			return bDBOK;
		}

	} else {
		// ASSERT_THROW(m_stylesheet_kind == "xmlrenderinfo", "This branch has not been implemented.");
	}

	if (!bDBOK || !bResult) {
		return bDBOK;
	}

	return bDBOK;	
}

bool ELStylesheet::type(EmdroslangExecEnv *pEE, bool& bResult)
{
	UNUSED(bResult);
	UNUSED(pEE);

	// FIXME:
	// 1) Remember to check existence of 'extends'.
	// 2) Remember to check the existence of feature 'docindex_feature'
	return true;
}


bool ELStylesheet::exec(EmdroslangExecEnv *pEE, bool& bResult)
{
	UNUSED(bResult);
	UNUSED(pEE);

	// FIXME: Implement
	return true;
}


//////////////////////////////////////////////////////////////////
//
// EmdroslangExecEnv
//
//////////////////////////////////////////////////////////////////
ELParameterName::ELParameterName(const std::string& param_name)
{
	m_param_name = param_name;
}

ELParameterName::~ELParameterName()
{
}


//////////////////////////////////////////////////////////////////
//
// ELFunctionSignature
//
//////////////////////////////////////////////////////////////////

ELFunctionSignature::ELFunctionSignature(ELFullyQualifiedFunctionName *pFQFunctionName,
					 ELParameterName *pFormalParameterList)
	: ELASTNode(),
	  m_pFQFunctionName(pFQFunctionName),
	  m_pFormalParameterList(pFormalParameterList)
{
}


ELFunctionSignature::~ELFunctionSignature()
{
	delete m_pFormalParameterList;
	delete m_pFQFunctionName;
}



bool ELFunctionSignature::weed(EmdroslangExecEnv *pEE, bool& bResult)
{
	bool bDBOK = true;

	if (m_pFQFunctionName != 0) {
		bDBOK = m_pFQFunctionName->weed(pEE, bResult);
	}

	if (!bDBOK || !bResult) {
		return bDBOK;
	}

	if (m_pFormalParameterList != 0) {
		bDBOK = m_pFormalParameterList->weed(pEE, bResult);
		
		if (!bDBOK || !bResult) {
			return bDBOK;
		}
		
		m_formal_parameters.clear();
		ELParameterName *pParamName = m_pFormalParameterList;
		while (pParamName != 0) {
			m_formal_parameters.push_back(pParamName->getParamName());
			pParamName = (ELParameterName*) pParamName->getNext();
		}
	}

	return bDBOK;
}


bool ELFunctionSignature::symbol(EmdroslangExecEnv *pEE, bool& bResult)
{
	bool bDBOK = true;

	bDBOK = m_pFQFunctionName->symbol(pEE, bResult);

	if (!bDBOK || !bResult) {
		return bDBOK;
	}

	return bDBOK;
}


bool ELFunctionSignature::type(EmdroslangExecEnv *pEE, bool& bResult)
{
	bool bDBOK = true;

	bDBOK = m_pFQFunctionName->type(pEE, bResult);

	if (!bDBOK || !bResult) {
		return bDBOK;
	}

	return bDBOK;
}


bool ELFunctionSignature::exec(EmdroslangExecEnv *pEE, bool& bResult)
{
	UNUSED(bResult);
	UNUSED(pEE);
	
	ASSERT_THROW(false, "NOT IMPLEMENTED.");
	return false;
}





//////////////////////////////////////////////////////////////////
//
// ELFunctionDef
//
//////////////////////////////////////////////////////////////////
ELFunctionDef::ELFunctionDef(ELFunctionSignature *pSignature,
			     ELStatement *pBody)
	: ELCompilationUnitPart(),
	  m_pSignature(pSignature),
	  m_pBody(pBody)
{
}


ELFunctionDef::~ELFunctionDef()
{
	delete m_pBody;
	delete m_pSignature;
}



bool ELFunctionDef::weed(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	bool bDBOK = true;

	if (m_pSignature == 0) {
		bResult = false;
		pEE->appendCompilerError("Error: Function definition without signature.");
	} else {
		bDBOK = m_pSignature->weed(pEE, bResult);
	}

	if (!bDBOK || !bResult) {
		return bDBOK;
	}

	if (m_pBody == 0) {
		bResult = false;
		pEE->appendCompilerError("Error: Function definition without body.");
	} else {
		bDBOK = m_pBody->weed(pEE, bResult);
	}

	if (!bDBOK || !bResult) {
		return bDBOK;
	}

	return bDBOK;
}


bool ELFunctionDef::symbol(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	bool bDBOK = true;

	bDBOK = m_pSignature->symbol(pEE, bResult);

	if (!bDBOK || !bResult) {
		return bDBOK;
	}

	bDBOK = m_pBody->symbol(pEE, bResult);

	if (!bDBOK || !bResult) {
		return bDBOK;
	}

	std::set<std::string> features_set;
	
	bDBOK = m_pBody->symbolFeaturesToGet(pEE, bResult, features_set);

	if (!bDBOK || !bResult) {
		return bDBOK;
	}

	std::set<std::string>::const_iterator ci = features_set.begin();
	std::set<std::string>::const_iterator cend = features_set.end();
	while (ci != cend) {
		m_features_to_get.push_back(*ci);
		++ci;
	}

	return bDBOK;
}


bool ELFunctionDef::type(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	bool bDBOK = true;

	const ELFullyQualifiedFunctionName *pFQFunctionName = m_pSignature->getFQFunctionName();
	if (pFQFunctionName->isObjectTypeName()) {
		// FIXME: Add any docindex_feature from the stylesheet to the list of features to get.
		
		// FIXME: Check existence of m_features_to_get, and a
	} else {
		if (!m_features_to_get.empty()) {
			bResult = false;
			bDBOK = false;
			pEE->appendCompilerError("getFeature() called in a non-start(), non-end(), non-object-type function body.");
		}
	}
	return bDBOK;
}


bool ELFunctionDef::exec(EmdroslangExecEnv *pEE, bool& bResult)
{
	UNUSED(bResult);
	UNUSED(pEE);

	// FIXME: Implement
	return false;
}



//////////////////////////////////////////////////////////////////
//
// ELNoopStatement
//
//////////////////////////////////////////////////////////////////
ELNoopStatement::ELNoopStatement()
	: ELStatement()
{
}

ELNoopStatement::~ELNoopStatement()
{
}

bool ELNoopStatement::exec(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	UNUSED(pEE);
	return true;
}


//////////////////////////////////////////////////////////////////
//
// ELNewScopeStatement
//
//////////////////////////////////////////////////////////////////

ELNewScopeStatement::ELNewScopeStatement(ELStatement *pBody)
	: ELStatement(),
	  m_pBody(pBody)
{
}


ELNewScopeStatement::~ELNewScopeStatement()
{
	delete m_pBody;
}



bool ELNewScopeStatement::weed(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	bool bDBOK = true;

	if (m_pBody == 0) {
		bResult = false;
		pEE->appendCompilerError("Missing body in compound {} statement.");
	} else {
		bDBOK = m_pBody->weed(pEE, bResult);
	}

	if (!bDBOK || !bResult) {
		return bDBOK;
	}

	return bDBOK;
}


bool ELNewScopeStatement::symbol(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	bool bDBOK = true;

	bool bAddImportBarrier = false;
	pEE->pushNewScope(bAddImportBarrier);
	bDBOK = m_pBody->symbol(pEE, bResult);
	pEE->popScope();

	if (!bDBOK || !bResult) {
		return bDBOK;
	}

	return bDBOK;
}


bool ELNewScopeStatement::type(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	bool bDBOK = true;

	bool bAddImportBarrier = false;
	pEE->pushNewScope(bAddImportBarrier);
	bDBOK = m_pBody->type(pEE, bResult);
	pEE->popScope();

	if (!bDBOK || !bResult) {
		return bDBOK;
	}

	return bDBOK;
}


bool ELNewScopeStatement::exec(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	bool bDBOK = true;

	bool bAddImportBarrier = false;
	pEE->pushNewScope(bAddImportBarrier);
	bDBOK = m_pBody->exec(pEE, bResult);
	pEE->popScope();

	if (!bDBOK || !bResult) {
		return bDBOK;
	}

	return bDBOK;
}



//////////////////////////////////////////////////////////////////
//
// ELVarDeclStatement
//
//////////////////////////////////////////////////////////////////

ELVarDeclStatement::ELVarDeclStatement(const std::string& var_name,
				       ELExpression *pAssignment)
	: ELStatement(),
	  m_var_name(var_name),
	  m_pAssignment(pAssignment)
{
}


ELVarDeclStatement::~ELVarDeclStatement()
{
	delete m_pAssignment;
}



bool ELVarDeclStatement::weed(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	bool bDBOK = true;
	if (m_pAssignment != 0) {
		bDBOK = m_pAssignment->weed(pEE, bResult);
	}

	if (!bDBOK || !bResult) {
		return bDBOK;
	}
	
	return bDBOK;
}


bool ELVarDeclStatement::symbol(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	bool bDBOK = true;

	if (pEE->varNameHasBeenDeclared(m_var_name)) {
		bResult = false;
		pEE->appendCompilerError("Variable name " + m_var_name + " has already been declared.");
	} else {
		pEE->declareVariableName(m_var_name);
	}

	if (!bDBOK || !bResult) {
		return bDBOK;
	}
	
	if (m_pAssignment != 0) {
		bDBOK = m_pAssignment->symbol(pEE, bResult);
	}

	if (!bDBOK || !bResult) {
		return bDBOK;
	}

	return bDBOK;
}


bool ELVarDeclStatement::type(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	bool bDBOK = true;

	if (m_pAssignment != 0) {
		bDBOK = m_pAssignment->type(pEE, bResult);
	}

	if (!bDBOK || !bResult) {
		return bDBOK;
	}

	return bDBOK;
}


bool ELVarDeclStatement::exec(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	bool bDBOK = true;
	
	if (m_pAssignment != NULL) {
		JSONValue *pVarValue = m_pAssignment->eval(pEE, bResult);
		if (bDBOK && bResult) {
			pEE->assignVariableValue(m_var_name, pVarValue);
		} else {
			delete pVarValue;
		}
	}

	if (!bDBOK || !bResult) {
		return bDBOK;
	}

	return bDBOK;
}

//////////////////////////////////////////////////////////////////
//
// ELAssignmentStatement
//
//////////////////////////////////////////////////////////////////

ELAssignmentStatement::ELAssignmentStatement(const std::string& var_name,
				       ELExpression *pAssignment)
	: ELStatement(),
	  m_var_name(var_name),
	  m_pAssignment(pAssignment)
{
}


ELAssignmentStatement::~ELAssignmentStatement()
{
	delete m_pAssignment;
}



bool ELAssignmentStatement::weed(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	bool bDBOK = true;

	if (m_pAssignment == 0) {
		bResult = false;
		pEE->appendCompilerError("Assignment to variable '" + m_var_name + "' does not have an assignment value.");
	} else {
		bDBOK = m_pAssignment->weed(pEE, bResult);
	}

	if (!bDBOK || !bResult) {
		return bDBOK;
	}
	
	return bDBOK;
}


bool ELAssignmentStatement::symbol(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	bool bDBOK = true;

	if (!pEE->varNameHasBeenDeclared(m_var_name)) {
		bResult = false;
		pEE->appendCompilerError("Variable name " + m_var_name + " has not been declared.");
	} 

	if (!bDBOK || !bResult) {
		return bDBOK;
	}
	
	if (m_pAssignment != 0) {
		bDBOK = m_pAssignment->symbol(pEE, bResult);
	}

	if (!bDBOK || !bResult) {
		return bDBOK;
	}

	return bDBOK;
}


bool ELAssignmentStatement::type(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	bool bDBOK = true;

	if (m_pAssignment != 0) {
		bDBOK = m_pAssignment->type(pEE, bResult);
	}

	if (!bDBOK || !bResult) {
		return bDBOK;
	}

	return bDBOK;
}


bool ELAssignmentStatement::exec(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	bool bDBOK = true;
	
	if (m_pAssignment != NULL) {
		JSONValue *pVarValue = m_pAssignment->eval(pEE, bResult);
		if (bDBOK && bResult) {
			pEE->assignVariableValue(m_var_name, pVarValue);
		} else {
			delete pVarValue;
		}
	}

	if (!bDBOK || !bResult) {
		return bDBOK;
	}

	return bDBOK;
}

//////////////////////////////////////////////////////////////////
//
// ELFunctionCallStatement
//
//////////////////////////////////////////////////////////////////

ELFunctionCallStatement::ELFunctionCallStatement(ELExpression *pFunctionCallExpression)
	: ELStatement(),
	  m_pFunctionCallExpression(pFunctionCallExpression)
{
}


ELFunctionCallStatement::~ELFunctionCallStatement()
{
	delete m_pFunctionCallExpression;
}



bool ELFunctionCallStatement::weed(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	bool bDBOK = true;
	
	if (m_pFunctionCallExpression == 0) {
		bResult = false;
		pEE->appendCompilerError("Function call expression does not exist.");
	} else {
		bool bAddImportBarrier = true;
		pEE->pushNewScope(bAddImportBarrier);
		bDBOK = m_pFunctionCallExpression->weed(pEE, bResult);	
		pEE->popScope();
	}

	if (!bDBOK || !bResult) {
		return bDBOK;
	}

	return bDBOK;
}


bool ELFunctionCallStatement::symbol(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	bool bDBOK = true;
	
	bool bAddImportBarrier = true;
	pEE->pushNewScope(bAddImportBarrier);
	bDBOK = m_pFunctionCallExpression->symbol(pEE, bResult);
	pEE->popScope();

	if (!bDBOK || !bResult) {
		return bDBOK;
	}

	return bDBOK;
}


bool ELFunctionCallStatement::type(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	bool bDBOK = true;
	
	bool bAddImportBarrier = true;
	pEE->pushNewScope(bAddImportBarrier);
	bDBOK = m_pFunctionCallExpression->type(pEE, bResult);
	pEE->popScope();

	if (!bDBOK || !bResult) {
		return bDBOK;
	}

	return bDBOK;
}


bool ELFunctionCallStatement::exec(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	bool bDBOK = true;

	bool bAddImportBarrier = true;
	pEE->pushNewScope(bAddImportBarrier);	
	JSONValue *pReturnValue = m_pFunctionCallExpression->eval(pEE, bResult);
	pEE->popScope();

	// Since we are not assigning anything in an
	// ELFunctionCallStatement, throw the result away.
	delete pReturnValue;

	if (!bDBOK || !bResult) {
		return bDBOK;
	}

	return bDBOK;
}


//////////////////////////////////////////////////////////////////
//
// ELWhileStatement
//
//////////////////////////////////////////////////////////////////


ELWhileStatement::ELWhileStatement(ELExpression *pCondition,
				   ELStatement *pBody,
				   bool bIsWhileDo)
	: ELStatement(),
	  m_pCondition(pCondition),
	  m_pBody(pBody),
	  m_bIsWhileDo(bIsWhileDo)
{
}


ELWhileStatement::~ELWhileStatement()
{
	delete m_pCondition;
	delete m_pBody;
}



bool ELWhileStatement::weed(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	bool bDBOK = true;
	
	if (m_pCondition == 0) {
		bResult = false;
		pEE->appendCompilerError("While statement does not have any condition.");
	} else {
		bDBOK = m_pCondition->weed(pEE, bResult);
	}

	if (!bDBOK || !bResult) {
		return bDBOK;
	}

	if (m_pBody == 0) {
		bResult = false;
		pEE->appendCompilerError("While statement does not have any body.");
	} else {
		bDBOK = m_pBody->weed(pEE, bResult);
	}

	if (!bDBOK || !bResult) {
		return bDBOK;
	}
	
	return bDBOK;
}


bool ELWhileStatement::symbol(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	bool bDBOK = true;
	
	bDBOK = m_pCondition->symbol(pEE, bResult);

	if (!bDBOK || !bResult) {
		return bDBOK;
	}

	bDBOK = m_pBody->symbol(pEE, bResult);

	if (!bDBOK || !bResult) {
		return bDBOK;
	}
	
	return bDBOK;
}


bool ELWhileStatement::type(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	bool bDBOK = true;
	
	bDBOK = m_pCondition->type(pEE, bResult);

	if (!bDBOK || !bResult) {
		return bDBOK;
	}

	bDBOK = m_pBody->type(pEE, bResult);

	if (!bDBOK || !bResult) {
		return bDBOK;
	}
	
	return bDBOK;
}


bool ELWhileStatement::exec(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	bool bDBOK = true;

	bool bContinue = true;
	if (m_bIsWhileDo) {
		while (bContinue
		       && bResult
		       && bDBOK) {
			if (bContinue && bResult && bDBOK) {
				JSONValue *pConditionValue = 0;
				pConditionValue = m_pCondition->eval(pEE, bResult);
				if (!bResult) {
					bContinue = false;
				} else {
					bContinue = pConditionValue->castToBool();
				}
				delete pConditionValue;
			}
			
			if (bContinue && bResult && bDBOK) {
				bDBOK = m_pBody->exec(pEE, bResult);
			}
		}
	} else {
		do {
			if (bContinue && bResult && bDBOK) {
				bDBOK = m_pBody->exec(pEE, bResult);
			}

			if (bContinue && bResult && bDBOK) {
				JSONValue *pConditionValue = 0;
				pConditionValue = m_pCondition->eval(pEE, bResult);
				if (!bResult) {
					bContinue = false;
				} else {
					bContinue = pConditionValue->castToBool();
				}
				delete pConditionValue;
			}
		} while (bContinue
			 && bDBOK
			 && bResult);
	}
	
	return bDBOK;
}


//////////////////////////////////////////////////////////////////
//
// ELForStatement
//
//////////////////////////////////////////////////////////////////

ELForStatement::ELForStatement(const std::string& var_name,
			       ELExpression *pExpression,
			       ELStatement *pBody)
	: ELStatement(),
	  m_var_name(var_name),
	  m_pExpression(pExpression),
	  m_pBody(pBody),
	  m_pIterator(0)
{
}


ELForStatement::~ELForStatement()
{
	delete m_pIterator;
	delete m_pBody;
	delete m_pExpression;
}



bool ELForStatement::weed(EmdroslangExecEnv *pEE, bool& bResult)
{
	UNUSED(bResult);
	UNUSED(pEE);

	// FIXME: Implement
	return false;
}


bool ELForStatement::symbol(EmdroslangExecEnv *pEE, bool& bResult)
{
	UNUSED(bResult);
	UNUSED(pEE);

	// FIXME: Implement
	return false;
}


bool ELForStatement::type(EmdroslangExecEnv *pEE, bool& bResult)
{
	UNUSED(bResult);
	UNUSED(pEE);

	// FIXME: Implement
	return false;
}


bool ELForStatement::exec(EmdroslangExecEnv *pEE, bool& bResult)
{
	UNUSED(bResult);
	UNUSED(pEE);

	// FIXME: Implement
	return false;
}

//////////////////////////////////////////////////////////////////
//
// ELIfStatement
//
//////////////////////////////////////////////////////////////////


ELIfStatement::ELIfStatement(ELExpression *pCondition, ELStatement *pThen)
	: ELStatement(),
	  m_pCondition(pCondition),
	  m_pThen(pThen)
{
}


ELIfStatement::~ELIfStatement()
{
	delete m_pThen;
	delete m_pCondition;
}



bool ELIfStatement::weed(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	bool bDBOK = true;

	if (m_pCondition == 0) {
		bResult = false;
		pEE->appendCompilerError("Missing condition in if statement.");
	} else {
		bDBOK = m_pCondition->weed(pEE, bResult);
	}

	if (!bDBOK || !bResult) {
		return bDBOK;
	}
	
	if (m_pThen == 0) {
		bResult = false;
		pEE->appendCompilerError("Missing 'then' body in if statement.");
	} else {
		bDBOK = m_pThen->weed(pEE, bResult);
	}

	if (!bDBOK || !bResult) {
		return bDBOK;
	}

	// Recurse through the 'else' part(s).
	return ELStatement::weed(pEE, bResult);
}


bool ELIfStatement::symbol(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	bool bDBOK = true;

	bDBOK = m_pCondition->symbol(pEE, bResult);

	if (!bDBOK || !bResult) {
		return bDBOK;
	}
	
	bDBOK = m_pThen->symbol(pEE, bResult);

	if (!bDBOK || !bResult) {
		return bDBOK;
	}

	// Recurse through the 'else' part(s).
	return ELStatement::symbol(pEE, bResult);
}

bool ELIfStatement::type(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	bool bDBOK = true;

	bDBOK = m_pCondition->type(pEE, bResult);

	if (!bDBOK || !bResult) {
		return bDBOK;
	}
	
	bDBOK = m_pThen->type(pEE, bResult);

	if (!bDBOK || !bResult) {
		return bDBOK;
	}

	// Recurse through the 'else' part(s).
	return ELStatement::type(pEE, bResult);
}


bool ELIfStatement::exec(EmdroslangExecEnv *pEE, bool& bResult)
{
	bResult = true;
	bool bDBOK = true;
	
	JSONValue *pConditionValue = m_pCondition->eval(pEE, bResult);
	if (!bDBOK || !bResult || pConditionValue == 0) {
		delete pConditionValue;
		return bDBOK;
	}

	bool bConditionValue = pConditionValue->castToBool();
	delete pConditionValue;

	if (bConditionValue) {
		bDBOK = m_pThen->exec(pEE, bResult);
	} else {
		if (m_pNext != 0) {
			bDBOK = m_pNext->exec(pEE, bResult);
		} else {
			bResult = true;
			bDBOK = true;
		}
	}

	return bDBOK;
}


//////////////////////////////////////////////////////////////////
//
// EmdroslangExecEnv
//
//////////////////////////////////////////////////////////////////
ELScope::ELScope(bool bAddImportBarrier)
	: m_pUp(0),
	  m_bHasImportBarrier(bAddImportBarrier),
	  m_variables(33)
{
}

class ELScopeDeletionVisitor : public EmdrosStringHashTableVisitor<JSONValue*> {
public:
	ELScopeDeletionVisitor() {};
	virtual ~ELScopeDeletionVisitor() {};
	void operator()(JSONValue *&value) {delete value;};
};

ELScope::~ELScope()
{
	ELScopeDeletionVisitor visitor;
	m_variables.visitAllEntries(visitor);
}

bool ELScope::varNameHasBeenDeclared(const std::string& var_name)
{
	JSONValue *pValue = 0;
	if (m_variables.entryExists(var_name, &pValue)) {
		return true;
	} else {
		if (m_bHasImportBarrier) {
			return false;
		} else {
			if (m_pUp == 0) {
				return false;
			} else {
				return m_pUp->varNameHasBeenDeclared(var_name);
			}
		}
	}
}

JSONValue *ELScope::evalVarName(const std::string& var_name, bool& bResult)
{
	bResult = true;
	JSONValue *pResult = 0;
	if (m_variables.entryExists(var_name, &pResult)) {
		bResult = true;
		return pResult;
	} else {
		if (m_bHasImportBarrier) {
			bResult = false;
			return 0;
		} else {
			if (m_pUp == 0) {
				bResult = false;
				return 0;
			} else {
				return m_pUp->evalVarName(var_name, bResult);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////
//
// EmdroslangExecEnv
//
//////////////////////////////////////////////////////////////////

EmdroslangExecEnv::EmdroslangExecEnv()
	: m_pNode(0),
	  pLexer(0),
	  bCompilerError(false),
	  bRuntimeError(false)
{
	clean();
}


EmdroslangExecEnv::~EmdroslangExecEnv()
{
	delete m_pNode;
	delete pLexer;
}



void EmdroslangExecEnv::clearErrors(void)
{
	m_compiler_error = "";
	m_runtime_error = "";
	bCompilerError = false;
	bRuntimeError = false;
}


std::string EmdroslangExecEnv::getCompilerError()
{
	return m_compiler_error;
}

std::string EmdroslangExecEnv::getRuntimeError()
{
	return m_compiler_error;
}


void EmdroslangExecEnv::appendCompilerError(const std::string& error_message)
{
	m_compiler_error += error_message;
}

void EmdroslangExecEnv::appendRuntimeError(const std::string& error_message)
{
	m_runtime_error += error_message;
}


void EmdroslangExecEnv::clean()
{
	while (m_pScopeStackTop != 0) {
		popScope();
	}
	
	delete m_pNode;
	delete pLexer;

	m_pNode = 0;
	pLexer = 0;

	bCompilerError = false;
	bRuntimeError = false;
}


JSONValue *EmdroslangExecEnv::evalVarName(const std::string& var_name, bool& bResult)
{
	if (m_pScopeStackTop == 0) {
		bResult = false;
		this->appendRuntimeError("Missing scope for var_name '" + var_name + "'");
		return new JSONValue(); // null c'tor
	} else {
		JSONValue *pResult = m_pScopeStackTop->evalVarName(var_name, bResult);
		if (bResult) {
			if (pResult == 0) {
				bResult = false;
				this->appendRuntimeError("Variable '" + var_name + "' used before assignment.");
				return new JSONValue(); // null c'tor
			} else {
				return pResult;
			}
		} else {
			this->appendRuntimeError("Variable '" + var_name + "' not declared in this scope.");
			delete pResult;
			return new JSONValue(); // null c'tor
		}
	}
}

bool EmdroslangExecEnv::varNameHasBeenDeclared(const std::string& var_name)
{
	ELScope *pTopScope = m_pScopeStackTop;
	if (pTopScope == 0) {
		appendCompilerError("No scope present for var_name '" + var_name + "'");
		return false;
	} else {
		return pTopScope->varNameHasBeenDeclared(var_name);
	}
}

bool EmdroslangExecEnv::functionHasBeenDeclared(ELFullyQualifiedFunctionName *pFQFunctionName, bool& bResult)
{
	// FIXME: Implement
	bResult = false;
	appendRuntimeError("Function name '" + pFQFunctionName->getWholeName() + "' has not been declared.");
	return false;
}

JSONValue *EmdroslangExecEnv::evalFunctionCall(ELFullyQualifiedFunctionName *pFQFunctionName, ELExpression *pParamList, bool& bResult)
{
	JSONValue *pResult = 0;
	// Push new scope, with export barrier
	// Add all formal parameters to the scope

	// Call the function
	// pop the scope
	
	// return the result
}


bool EmdroslangExecEnv::objectTypesExist(const std::list<std::string>& object_types_list, bool& bResult)
{
	bool bDBOK = true;
	std::list<std::string>::const_iterator ci = object_types_list.begin();
	std::list<std::string>::const_iterator cend = object_types_list.end();
	while (bResult && bDBOK && ci != cend) {
		std::string object_type_name = *ci;
		
		bDBOK = this->objectTypeExists(object_type_name, bResult);
		
		++ci;
	}

	return bDBOK;
}


bool EmdroslangExecEnv::objectTypeExists(const std::string& object_type_name, bool& bResult)
{
	// FIXME: Implement
	bool bDBOK = true;
	bResult = true;
	this->appendCompilerError("Object type name '" + object_type_name + "': Existence verification not implemented yet.");

	return bDBOK;
}


void EmdroslangExecEnv::pushNewScope(bool bAddImportBarrier)
{
	ELScope *pNewScope = new ELScope(bAddImportBarrier);
	pNewScope->setUp(m_pScopeStackTop);
	m_pScopeStackTop = pNewScope;
}

void EmdroslangExecEnv::popScope()
{
	ELScope *pTopScope = m_pScopeStackTop;
	if (pTopScope != 0) {
		m_pScopeStackTop = pTopScope->getUp();

		delete pTopScope;
	}
}
