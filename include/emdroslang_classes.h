/*
 * emdroslang_classes.h
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

#ifndef EMDROSLANG_CLASSES_HEADER_H_
#define EMDROSLANG_CLASSES_HEADER_H_

#include <string>
#include <map>
#include <set>
#include <list>

#include "json_classes.h"
#include "emdroslang_lexer.h"
#include "emdf_hash.h"

class EmdroslangExecEnv; // Forward declaration
class JSONValue; // Forward declaration

class ELASTNode {
 protected:
	ELASTNode *m_pNext;
 public:
	ELASTNode();
	virtual ~ELASTNode();
	
	virtual void setNext(ELASTNode *pNext) { m_pNext = pNext; };
	virtual ELASTNode *getNext(void) { return m_pNext; };
	virtual ELASTNode *getTail();

	virtual bool weed(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool symbol(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool type(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool exec(EmdroslangExecEnv *pEE, bool& bResult);
};

class ELStatement : public ELASTNode {
 public:
	ELStatement();
	virtual ~ELStatement();

	virtual bool symbolFeaturesToGet(EmdroslangExecEnv *pEE, bool& bResult, std::set<std::string>& features_to_get);
	
	virtual bool weed(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool symbol(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool type(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool exec(EmdroslangExecEnv *pEE, bool& bResult);
};

enum eELExpressionKind {
	kELEKLiteral,
	kELEKVarName,
	kELEKBinaryOperation,
	kELEKUnaryOperation,
	kELEKFunctionCall
};

enum eELOperation {
	kOpNot,
	kOpOr,
	kOpAnd,
	kOpPlus,
	kOpMinus,
	kOpStar,
	kOpSlash,
	kOpPercent,
	kOpEqEq,
	kOpNeq,
	kOpLt,
	kOpLeq,
	kOpGt,
	kOpGeq,
	kOpLookup,
	kOpSlice,
	kOpNA // Not Applicable
};

class ELFullyQualifiedFunctionName : public ELASTNode {
 protected:
	std::string m_stylesheet_name;
	std::string m_object_type_name;
	std::string m_function_name;
	std::string m_whole_name;
 public:
	ELFullyQualifiedFunctionName(const std::string& stylesheet_name,
				     const std::string& object_type_name,
				     const std::string& function_name);
	virtual ~ELFullyQualifiedFunctionName();

	virtual bool isObjectTypeName() const;
	virtual std::string getStylesheetName() const;
	virtual std::string getObjectTypeName() const;
	virtual std::string getWholeName() const;

	virtual bool weed(EmdroslangExecEnv *pEE, bool& bResult);
	
 protected:
	virtual std::string makeWholeName() const;
};


class ELExpression : public ELASTNode {
 protected:
	JSONValue *m_pLiteral;
	std::string *m_pVarName;

	ELFullyQualifiedFunctionName *m_pFQFunctionName;
	
	ELExpression *m_pLHS;
	eELOperation m_operation;
	ELExpression *m_pRHS;

	eJSONValueKind m_result_type;
	eELExpressionKind m_expression_kind;
 public:
	ELExpression(JSONValue *pLiteral);
	ELExpression(const std::string& var_name);
	ELExpression(ELExpression *pLHS, eELOperation binary_operation, ELExpression *pRHS);
	ELExpression(ELFullyQualifiedFunctionName *pFQFunctionName, ELExpression *pParamList);
	ELExpression(eELOperation unary_operation, ELExpression *pOperand);
	virtual ~ELExpression();

	JSONValue *eval(EmdroslangExecEnv *pEE, bool& bResult);
	JSONValue *evalBinaryOperation(EmdroslangExecEnv *pEE, bool& bResult);
	JSONValue *evalBinaryOpPlus(EmdroslangExecEnv *pEE, JSONValue *pLHSValue, JSONValue *pRHSValue, bool& bResult);
	JSONValue *evalBinaryOpMinus(EmdroslangExecEnv *pEE, JSONValue *pLHSValue, JSONValue *pRHSValue, bool& bResult);
	JSONValue *evalBinaryOpStar(EmdroslangExecEnv *pEE, JSONValue *pLHSValue, JSONValue *pRHSValue, bool& bResult);
	JSONValue *evalBinaryOpSlash(EmdroslangExecEnv *pEE, JSONValue *pLHSValue, JSONValue *pRHSValue, bool& bResult);
	JSONValue *evalBinaryOpPercent(EmdroslangExecEnv *pEE, JSONValue *pLHSValue, JSONValue *pRHSValue, bool& bResult);
	JSONValue *evalUnaryOperation(EmdroslangExecEnv *pEE, bool& bResult);
	JSONValue *evalFunctionCall(EmdroslangExecEnv *pEE, bool& bResult);
	std::string applyPercentToString(const std::string& LHSString, JSONValue *pRHSValue, bool& bResult, std::string& error_message);
	eJSONValueKind getResultType() const;
	
	virtual bool weed(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool weedBinaryOperation(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool weedUnaryOperation(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool weedFunctionCall(EmdroslangExecEnv *pEE, bool& bResult);

	virtual bool symbol(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool symbolBinaryOperation(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool symbolUnaryOperation(EmdroslangExecEnv *pEE, bool& bResult);	
	virtual bool symbolFunctionCall(EmdroslangExecEnv *pEE, bool& bResult);

	virtual bool type(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool typeBinaryOperation(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool typeUnaryOperation(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool typeFunctionCall(EmdroslangExecEnv *pEE, bool& bResult);
};

class ELCompilationUnitPart : public ELASTNode {
 public:
	ELCompilationUnitPart();
	virtual ~ELCompilationUnitPart();
};

class ELStylesheet : public ELCompilationUnitPart {
 protected:
	std::string m_stylesheet_kind;
	std::string m_stylesheet_name;
	std::string m_extends;
	JSONValue *m_pBody;
 protected:
	std::string m_object_order;
	std::string m_docindex_feature;
	std::list<std::string> m_object_types;
	std::list<std::string> m_elements;
 public:
	ELStylesheet(const std::string& stylesheet_kind,
		     const std::string& stylesheet_name,
		     const std::string& extends,
		     JSONValue *pBody);
	virtual ~ELStylesheet();
	virtual bool weed(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool symbol(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool type(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool exec(EmdroslangExecEnv *pEE, bool& bResult);
 protected:
	virtual bool weedBody(EmdroslangExecEnv *pEE, bool& bResult);
};

class ELParameterName : public ELASTNode {
 protected:
	std::string m_param_name;
 public:
	ELParameterName(const std::string& param_name);
	virtual ~ELParameterName();

	virtual std::string getParamName() const { return m_param_name; };
};

class ELFunctionSignature : public ELASTNode {
 protected:
	ELFullyQualifiedFunctionName *m_pFQFunctionName;
	ELParameterName *m_pFormalParameterList;
 protected:
	std::list<std::string> m_formal_parameters;
 public:
	ELFunctionSignature(ELFullyQualifiedFunctionName *pFQFunctionName,
			    ELParameterName *pFormalParameterList);
	virtual ~ELFunctionSignature();

	virtual const ELFullyQualifiedFunctionName *getFQFunctionName() const { return m_pFQFunctionName; };
	virtual const std::list<std::string>& getFormalParameters() const { return m_formal_parameters; };

	virtual bool weed(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool symbol(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool type(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool exec(EmdroslangExecEnv *pEE, bool& bResult);
};


class ELFunctionDef : public ELCompilationUnitPart {
 protected:
	ELFunctionSignature *m_pSignature;
	ELStatement *m_pBody;
	std::list<std::string> m_features_to_get;
 public:
	ELFunctionDef(ELFunctionSignature *pSignature,
		      ELStatement *pBody);
	virtual ~ELFunctionDef();

	virtual void appendFeatureToGet(const std::string& feature_to_get);

	virtual bool weed(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool symbol(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool type(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool exec(EmdroslangExecEnv *pEE, bool& bResult);
};


class ELNoopStatement : public ELStatement {
 public:
	ELNoopStatement();
	virtual ~ELNoopStatement();

	virtual bool exec(EmdroslangExecEnv *pEE, bool& bResult);
};

class ELNewScopeStatement : public ELStatement {
 protected:
	ELStatement *m_pBody;
 public:
	ELNewScopeStatement(ELStatement *pBody);
	virtual ~ELNewScopeStatement();

	virtual bool weed(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool symbol(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool type(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool exec(EmdroslangExecEnv *pEE, bool& bResult);
};

class ELVarDeclStatement : public ELStatement {
 protected:
	std::string m_var_name;
	ELExpression *m_pAssignment;
 public:
	ELVarDeclStatement(const std::string& var_name,
			   ELExpression *pAssignment);
	virtual ~ELVarDeclStatement();
			    
	virtual bool weed(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool symbol(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool type(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool exec(EmdroslangExecEnv *pEE, bool& bResult);
};

// There is a subtle difference between, on the one hand, an
// ELVarDeclStatement (which ensures that a variable exists, and
// optionally assigns an expression value to it), and on the other
// hand an ELAssignmentStatement (which assumes that the variable
// exists, and throws an error if it doesn't).
class ELAssignmentStatement : public ELStatement {
 protected:
	std::string m_var_name;
	ELExpression *m_pAssignment;
 public:
	ELAssignmentStatement(const std::string& var_name,
			   ELExpression *pAssignment);
	virtual ~ELAssignmentStatement();
			    
	virtual bool weed(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool symbol(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool type(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool exec(EmdroslangExecEnv *pEE, bool& bResult);
};

class ELFunctionCallStatement : public ELStatement {
 protected:
	ELExpression *m_pFunctionCallExpression;
 public:
	ELFunctionCallStatement(ELExpression *pFunctionCallExpression);
	virtual ~ELFunctionCallStatement();

	virtual bool weed(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool symbol(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool type(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool exec(EmdroslangExecEnv *pEE, bool& bResult);
};

class ELWhileStatement : public ELStatement {
 protected:
	ELExpression *m_pCondition;
	ELStatement *m_pBody;
	bool m_bIsWhileDo; // false == do...while();, true = while()...;
 public:
	ELWhileStatement(ELExpression *pCondition,
			 ELStatement *pBody,
			 bool bIsWhileDo);
	virtual ~ELWhileStatement();

	virtual bool weed(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool symbol(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool type(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool exec(EmdroslangExecEnv *pEE, bool& bResult);
};

class ELExpressionIterator {
 protected:
	ELExpression *pExpression;
	std::list<JSONValue *> *m_pList;
	std::list<JSONValue *>::iterator m_list_it;

	std::map<std::string, JSONValue *> *m_pObject;
	std::map<std::string, JSONValue *>::iterator m_object_it;

	std::string *m_pString;
	std::string::size_type m_string_iterator;

	bool m_bIsReversed;
 public:
	ELExpressionIterator(ELExpression *pExpression, bool bIsReversed);
	virtual ~ELExpressionIterator();

	bool hasNext();
	ELExpression *getNext();
};

class ELForStatement : public ELStatement {
 protected:
	std::string m_var_name;
	ELExpression *m_pExpression;
	ELStatement *m_pBody;
	
	ELExpressionIterator *m_pIterator;
 public:
	ELForStatement(const std::string& var_name,
		       ELExpression *pExpression,
		       ELStatement *pBody);
	virtual ~ELForStatement();

	virtual bool weed(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool symbol(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool type(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool exec(EmdroslangExecEnv *pEE, bool& bResult);
};

class ELIfStatement : public ELStatement {
 protected:
	ELExpression *m_pCondition;
	ELStatement *m_pThen;
	// Elseif -> ... -> else is in the m_pNext variable!
 public:
	ELIfStatement(ELExpression *pCondition, ELStatement *pThen);
	virtual ~ELIfStatement();

	virtual bool weed(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool symbol(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool type(EmdroslangExecEnv *pEE, bool& bResult);
	virtual bool exec(EmdroslangExecEnv *pEE, bool& bResult);
};


typedef EmdrosStringHashTable<JSONValue*> VarNameValueHashTable;

class ELScope {
 protected:
	ELScope *m_pUp;
	bool m_bHasImportBarrier;
	VarNameValueHashTable m_variables;
 public:
	ELScope(bool bHaddImportBarrier);
	virtual ~ELScope();

	virtual bool varNameHasBeenDeclared(const std::string& var_name);
	virtual JSONValue *evalVarName(const std::string& var_name, bool& bResult);
	

	virtual void setUp(ELScope *pNewUp) { m_pUp = pNewUp; };
	virtual ELScope *getUp() { return m_pUp; };
};

class EmdroslangExecEnv {
 protected:
	std::string m_compiler_error;
	std::string m_runtime_error;
	ELCompilationUnitPart *m_pNode;
	ELScope *m_pScopeStackTop;
 public:
	EmdroslangScanner *pLexer;
	bool bCompilerError;
	bool bRuntimeError;

	EmdroslangExecEnv();
	~EmdroslangExecEnv();

	bool hasError() const { return bCompilerError || bRuntimeError; };
	void clearErrors(void);
	std::string getCompilerError();
	std::string getRuntimeError();
	void appendCompilerError(const std::string& error_message);
	void appendRuntimeError(const std::string& error_message);

	JSONValue *evalVarName(const std::string& var_name, bool& bResult);
	bool declareStylesheet(const std::string& stylesheet_kind, const std::string& stylesheet_name, bool& bResult);
	bool stylesheetNameHasBeenDeclared(const std::string& stylesheet_name);
	bool objectTypesExist(const std::list<std::string>& object_types_list, bool& bResult);
	bool objectTypeExists(const std::string& object_type_name, bool& bResult);
	bool varNameHasBeenDeclared(const std::string& var_name);
	void declareVariableName(const std::string& var_name);
	void assignVariableValue(const std::string& var_name, JSONValue *pVarValue);
	bool functionHasBeenDeclared(ELFullyQualifiedFunctionName *pFQFunctionName, bool& bResult);
	JSONValue *evalFunctionCall(ELFullyQualifiedFunctionName *pFQFunctionName, ELExpression *pParamList, bool& bResult);

	void pushNewScope(bool bAddImportBarrie);
	void popScope();

	void clean();
};

extern ELCompilationUnitPart *parseCompilationUnit(EmdroslangExecEnv* pEE, const std::string& compilation_unit_string, bool& bResult);

#endif // EMDROSLANG_CLASSES_HEADER_H_
