/*
 * templatelang.h
 *
 * Template language classes
 *
 * Ulrik Sandborg-Petersen
 * Created: 7/28-2008
 * Last update: 12/21-2017
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2008-2017  Ulrik Sandborg-Petersen
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

#ifndef TEMPLATELANG_HEADER_H_
#define TEMPLATELANG_HEADER_H_

#include <string>
#include <map>
#include <set>
#include <list>

#include "templatelang_lexer.h"
#include "qdxml.h"
#include "arena.h"

class TemplateLangExecEnv; // Forward declaration
class MemObject; // Forward declaration
class JSONValue; // Forward declaration

class TemplateASTNode {
 protected:
	TemplateASTNode *m_pNext;
 public:
	TemplateASTNode();
	virtual ~TemplateASTNode();
	
	virtual void setNext(TemplateASTNode *pNext) { m_pNext = pNext; };
	virtual TemplateASTNode *getNext(void) { return m_pNext; };

	virtual void exec(TemplateLangExecEnv *pEE);

	virtual bool isTemplateString() const { return false; };

	virtual void simplifyAST();
};

typedef enum {
	kTemplateID_D,
	kTemplateFirstMonad,
	kTemplateDBName
} eTemplateSimpleNodeKind;

class TemplateDictlookupFeature : public TemplateASTNode {
 protected:
	std::string m_dict_name;
	long m_feature_index;
	std::string m_default_value;
	bool m_bMangleFeature;
 public:
	TemplateDictlookupFeature(std::string *pDictName, long feature_index, std::string *pDefaultValue, bool bMangleFeature);
	virtual ~TemplateDictlookupFeature();

	virtual void exec(TemplateLangExecEnv *pEE);
};



class TemplateDictlookupVar : public TemplateASTNode {
 protected:
	std::string m_dict_name;
	std::string m_var_name;
	std::string m_default_value;
	bool m_bMangleVar;
 public:
	TemplateDictlookupVar(std::string *pDictName, std::string *pVarName, std::string *pDefaultValue, bool bMangleVar);
	virtual ~TemplateDictlookupVar();

	virtual void exec(TemplateLangExecEnv *pEE);
};



class TemplateDictlookupCounter : public TemplateASTNode {
 protected:
	std::string m_dict_name;
	std::string m_counter_name;
	std::string m_default_value;
 public:
	TemplateDictlookupCounter(std::string *pDictName, std::string *pCounterName, std::string *pDefaultValue, bool bMangleVar);
	virtual ~TemplateDictlookupCounter();

	virtual void exec(TemplateLangExecEnv *pEE);
};




class TemplateSimpleNode : public TemplateASTNode {
 protected:
	eTemplateSimpleNodeKind m_kind;
 public:
	TemplateSimpleNode(eTemplateSimpleNodeKind kind);
	virtual ~TemplateSimpleNode();

	virtual void exec(TemplateLangExecEnv *pEE);
};

typedef enum {
	kMKXML,
	kMKJSON,
	kMKNone
} eMangleKind;

class TemplateFeature : public TemplateASTNode {
 protected:
	long m_index;
	eMangleKind m_mangle_kind;
 public:
	TemplateFeature(long index, eMangleKind mangle_kind);
	virtual ~TemplateFeature();

	virtual void exec(TemplateLangExecEnv *pEE);
};

class TemplateAttrib : public TemplateASTNode {
 protected:
	std::string m_attrib_name;
 public:
	TemplateAttrib(std::string *pString);
	virtual ~TemplateAttrib();

	virtual void exec(TemplateLangExecEnv *pEE);
};

class TemplateAttribNoMangle : public TemplateASTNode {
 protected:
	std::string m_attrib_name;
 public:
	TemplateAttribNoMangle(std::string *pString);
	virtual ~TemplateAttribNoMangle();

	virtual void exec(TemplateLangExecEnv *pEE);
};


class TemplateCopyAttribs : public TemplateASTNode {
 public:
	TemplateCopyAttribs();
	virtual ~TemplateCopyAttribs();

	virtual void exec(TemplateLangExecEnv *pEE);
};

class TemplateCounter : public TemplateASTNode {
 protected:
	std::string m_counter_name;
 public:
	TemplateCounter(std::string *pString);
	virtual ~TemplateCounter();

	virtual void exec(TemplateLangExecEnv *pEE);
};

class TemplateCounterFormat : public TemplateASTNode {
 protected:
	std::string m_counter_name;
	std::string m_format;
 public:
	TemplateCounterFormat(std::string *pString, std::string *pFormat);
	virtual ~TemplateCounterFormat();

	virtual void exec(TemplateLangExecEnv *pEE);
};

class TemplateSetCounter : public TemplateASTNode {
 protected:
	std::string m_counter_name;
	long long m_value;
 public:
	TemplateSetCounter(std::string *pString, long long value);
	virtual ~TemplateSetCounter();

	virtual void exec(TemplateLangExecEnv *pEE);
};

class TemplateSetCounterVar : public TemplateASTNode {
 protected:
	std::string m_counter_name;
	std::string m_variable_name;
 public:
	TemplateSetCounterVar(std::string *pString, std::string *pVariableName);
	virtual ~TemplateSetCounterVar();

	virtual void exec(TemplateLangExecEnv *pEE);
};

class TemplateIncCounter : public TemplateASTNode {
 protected:
	std::string m_counter_name;
	long long m_value;
	std::string m_var_name;
 public:
	TemplateIncCounter(std::string *pString, long long value, std::string *pVarName);
	virtual ~TemplateIncCounter();

	virtual void exec(TemplateLangExecEnv *pEE);
};

class TemplateDecCounter : public TemplateASTNode {
 protected:
	std::string m_counter_name;
	long long m_value;
	std::string m_var_name;
 public:
	TemplateDecCounter(std::string *pString, long long value, std::string *pVarName);
	virtual ~TemplateDecCounter();

	virtual void exec(TemplateLangExecEnv *pEE);
};

class TemplateString : public TemplateASTNode {
 protected:
	std::string m_string;
 public:
	TemplateString(std::string *pString);
	virtual ~TemplateString();

	virtual void exec(TemplateLangExecEnv *pEE);

	std::string getString() const { return m_string; };
	void setString(const std::string& newStr) { m_string = newStr; };

	virtual bool isTemplateString() const { return true; };
};

enum eTemplateNixingType {
	kTNTOff,
	kTNTDoc,
	kTNTAll
};

class TemplateNixing : public TemplateASTNode {
 protected:
	eTemplateNixingType m_nixingType;
 public:
	TemplateNixing(eTemplateNixingType nixingType);
	virtual ~TemplateNixing();

	virtual void exec(TemplateLangExecEnv *pEE);
};



class TemplateIf : public TemplateASTNode {
 protected:
	TemplateASTNode *m_pCondition;
	TemplateASTNode *m_pExecIfTrue;
	TemplateASTNode *m_pExecIfFalse;
 public:
	TemplateIf(TemplateASTNode *pCondition,
		   TemplateASTNode *pExecIfTrue,
		   TemplateASTNode *pExecIfFalse);
	virtual ~TemplateIf();

	virtual void exec(TemplateLangExecEnv *pEE);

	virtual void simplifyAST();
};

class TemplateIfFeatureEqual : public TemplateASTNode {
 protected:
	long m_feature_index;
	std::string m_value;
 public:
	TemplateIfFeatureEqual(long feature_index, std::string *pString);
	virtual ~TemplateIfFeatureEqual();

	virtual void exec(TemplateLangExecEnv *pEE);
};


class TemplateIfVarEqual : public TemplateASTNode {
 protected:
	std::string m_variable_name;
	std::string m_value;
	bool m_bValueIsVarName;
 public:
	TemplateIfVarEqual(std::string *pVariableName, std::string *pString, bool bStringIsVarName);
	virtual ~TemplateIfVarEqual();

	virtual void exec(TemplateLangExecEnv *pEE);
};

class TemplateIfListEmpty : public TemplateASTNode {
 protected:
	std::string m_list_name;
 public:
	TemplateIfListEmpty(std::string *pListName);
	virtual ~TemplateIfListEmpty();

	virtual void exec(TemplateLangExecEnv *pEE);
};


class TemplateIfNamedSetEmpty : public TemplateASTNode {
 protected:
	std::string m_named_set_name;
 public:
	TemplateIfNamedSetEmpty(std::string *pNamedSetName);
	virtual ~TemplateIfNamedSetEmpty();

	virtual void exec(TemplateLangExecEnv *pEE);
};


class TemplateIfAttribEqual : public TemplateASTNode {
 protected:
	std::string m_attrib_name;
	std::string m_value;
 public:
	TemplateIfAttribEqual(std::string *pAttribName, std::string *pString);
	virtual ~TemplateIfAttribEqual();

	virtual void exec(TemplateLangExecEnv *pEE);
};


class TemplateIfHasAttrib : public TemplateASTNode {
 protected:
	std::string m_attrib_name;
 public:
	TemplateIfHasAttrib(std::string *pAttribName);
	virtual ~TemplateIfHasAttrib();

	virtual void exec(TemplateLangExecEnv *pEE);
};


class TemplateListAppendBegin : public TemplateASTNode {
 protected:
	std::string m_strListID;
	std::string *m_pStrDelimiter;
 public:
	TemplateListAppendBegin(std::string *pStrListID, std::string *pDelimiter);
	virtual ~TemplateListAppendBegin();

	virtual void exec(TemplateLangExecEnv *pEE);
};


class TemplateListAppendEnd : public TemplateASTNode {
 public:
	TemplateListAppendEnd();
	virtual ~TemplateListAppendEnd();

	virtual void exec(TemplateLangExecEnv *pEE);
};


class TemplateNamedSetAddBegin : public TemplateASTNode {
 protected:
	std::string m_strNamedSetID;
	std::string m_strDelimiter;
 public:
	TemplateNamedSetAddBegin(std::string *pStrNamedSetID, std::string *pStrDelimiter);
	virtual ~TemplateNamedSetAddBegin();

	virtual void exec(TemplateLangExecEnv *pEE);
};


class TemplateNamedSetAddEnd : public TemplateASTNode {
 public:
	TemplateNamedSetAddEnd();
	virtual ~TemplateNamedSetAddEnd();

	virtual void exec(TemplateLangExecEnv *pEE);
};


class TemplateNamedSetRemoveBegin : public TemplateASTNode {
 protected:
	std::string m_strNamedSetID;
	std::string m_strDelimiter;
 public:
	TemplateNamedSetRemoveBegin(std::string *pStrNamedSetID, std::string *pStrDelimiter);
	virtual ~TemplateNamedSetRemoveBegin();

	virtual void exec(TemplateLangExecEnv *pEE);
};


class TemplateNamedSetRemoveEnd : public TemplateASTNode {
 public:
	TemplateNamedSetRemoveEnd();
	virtual ~TemplateNamedSetRemoveEnd();

	virtual void exec(TemplateLangExecEnv *pEE);
};


class TemplateSetVarBegin : public TemplateASTNode {
 protected:
	std::string m_strVarName;
 public:
	TemplateSetVarBegin(std::string *pStrVarName);
	virtual ~TemplateSetVarBegin();

	virtual void exec(TemplateLangExecEnv *pEE);
};


class TemplateSetVarNow : public TemplateASTNode {
 protected:
	std::string m_strVarName;
	std::string m_strValue;
 public:
	TemplateSetVarNow(std::string *pStrVarName, std::string *pStrValue);
	virtual ~TemplateSetVarNow();

	virtual void exec(TemplateLangExecEnv *pEE);
};


class TemplateSetVarEnd : public TemplateASTNode {
 public:
	TemplateSetVarEnd();
	virtual ~TemplateSetVarEnd();

	virtual void exec(TemplateLangExecEnv *pEE);
};



class TemplateEmitVar : public TemplateASTNode {
 protected:
	std::string m_strVarName;
 public:
	TemplateEmitVar(std::string *pStrVarName);
	virtual ~TemplateEmitVar();

	virtual void exec(TemplateLangExecEnv *pEE);
};




class TemplateListClear : public TemplateASTNode {
 protected:
	std::string m_strListID;
 public:
	TemplateListClear(std::string *pStrListID);
	virtual ~TemplateListClear();

	virtual void exec(TemplateLangExecEnv *pEE);
};


class TemplateListEmit : public TemplateASTNode {
 protected:
	std::string m_strListID;
	std::string m_strSeparator;
 public:
	TemplateListEmit(std::string *pStrListID, std::string *pStrSeparator);
	virtual ~TemplateListEmit();

	virtual void exec(TemplateLangExecEnv *pEE);
};


class TemplateNamedSetClear : public TemplateASTNode {
 protected:
	std::string m_strNamedSetID;
 public:
	TemplateNamedSetClear(std::string *pStrNamedSetID);
	virtual ~TemplateNamedSetClear();

	virtual void exec(TemplateLangExecEnv *pEE);
};


class TemplateNamedSetEmit : public TemplateASTNode {
 protected:
	std::string m_strNamedSetID;
	std::string m_strSeparator;
 public:
	TemplateNamedSetEmit(std::string *pStrNamedSetID, std::string *pStrSeparator);
	virtual ~TemplateNamedSetEmit();

	virtual void exec(TemplateLangExecEnv *pEE);
};


typedef std::map<std::string, std::string> TemplateLangVariableMap;

typedef std::map<std::string, std::set<std::string> > TemplateLangNamedSetMap;

typedef enum {
	kETDoc,
	kETList,
	kETDelimitedList,
	kETVar,
	kETNamedSet
} eEmitTarget;

class TemplateLangExecEnv {
 protected:
	const JSONValue *m_pTopStylesheet;
	std::string *m_pError;
	TemplateASTNode *m_pNode;
	Bigstring *m_pOutput;
	bool m_bOwnsBigstring;
	std::string m_DBName;
	AttributeMap m_attrs;
	typedef std::map<std::string, long long> CounterMap;
	CounterMap m_counters;
	typedef std::map<std::string, std::list<std::string> > StringListMap;
	StringListMap m_lists;
	StringListMap m_delimited_lists;
	typedef std::map<std::string, std::set<std::string> > StringNamedSetMap;
	StringNamedSetMap m_named_sets;
	typedef std::map<std::string, Bigstring*> VariableMap;
	VariableMap m_variables;
	VariableMap m_sets_under_construction;
	std::string m_strCurrentSetVar;
	std::list<std::string> m_list_stack;
	std::list<std::string*> m_list_delimiter_stack;
	std::list<std::string> m_named_set_name_stack;
	std::list<std::string> m_named_set_delimiter_stack;
	std::map<std::string, std::list<std::string> > m_named_set_string_temp_area;
	std::list<eEmitTarget> m_emit_target_stack;
	eTemplateNixingType m_nixingType;
 public:
	MemObject *m_pObject;
	TemplateLanguageScanner *pLexer;

	TemplateLangExecEnv();
	~TemplateLangExecEnv();
	bool bLastIfCondition;
	bool bSyntaxError;

	eEmitTarget getCurEmitTarget();
	void pushEmitTargetStack(eEmitTarget emitTarget);
	eEmitTarget popEmitTargetStack();

	void setVariables(const TemplateLangVariableMap& variables);

	void simplifyAST();

	bool listIsEmpty(const std::string& list_name);

	bool namedSetIsEmpty(const std::string& named_set_name);

	void setBigstring(Bigstring *pBigstring);
	void setObject(MemObject *pObj) { m_pObject = pObj; };

	void setVar(const std::string& variable_name, const std::string& value);

	void setNixing(eTemplateNixingType nixingType) { m_nixingType = nixingType; };
	eTemplateNixingType getNixing() const { return m_nixingType; };
	

	std::string getDBName() const { return m_DBName; };
	void setDBName(const std::string& DBName) { m_DBName = DBName; };

	void setTopStylesheet(const JSONValue *pStopStyleSheet);

	void setTopNode(TemplateASTNode *pNode);
	TemplateASTNode *extractTopNode(void);

	void clearError(void);
	std::string getError();
	void addToError(const std::string& addition);
	void prependError(const std::string& addition);

	void clearOutput(void);
	std::string getOutput();
	Bigstring *getOutputBigstring() { return m_pOutput; };
	std::string getAndClearOutput();
	void addToOutput(const std::string& addition);
	void addBigstringToOutput(Bigstring *pBigstring);
	void setAttributes(const AttributeMap& attrs);
	bool hasAttribute(const std::string& attrib_name);
	std::string getAttribute(const std::string& attrib_name);
	const AttributeMap& getAllAttributes() const;

	std::string dictLookup(const std::string& dict_name, const std::string& dict_key, const std::string& default_value);

	long long getCounter(const std::string& counter_name); // Initializes to 0 if not present!
	void setCounter(const std::string& counter_name, long long value); // Creates if not present!
	void incCounter(const std::string& counter_name, long long value); // Initializes to 0 if not present!
	void incCounter(const std::string& counter_name, const std::string& variable_name); // Initializes to 0 if not present!
	void decCounter(const std::string& counter_name, long long value); // Initializes to 0 if not present!
	void decCounter(const std::string& counter_name, const std::string& variable_name); // Initializes to 0 if not present!

	void emitBigstring(Bigstring *pBigstring);
	void emitStdString(const std::string& str);

	
	void enterListMode(const std::string& list_name, std::string *pStrDelimiter);
	void exitListMode();
	void emitList(const std::string& list_name, const std::string& separator);
	void clearList(const std::string& list_name);

	void enterNamedSetAddMode(const std::string& named_set_name, const std::string& delimiter);
	void exitNamedSetAddMode();

	void enterNamedSetRemoveMode(const std::string& named_set_name, const std::string& delimiter);
	void exitNamedSetRemoveMode();
	
	void exitNamedSetAddOrRemoveMode(bool bAdd);

	void emitNamedSet(const std::string& named_set_name, const std::string& separator);
	void clearNamedSet(const std::string& named_set_name);

	void enterSetVarMode(const std::string& var_name);
	void exitSetVarMode();
	void emitVar(const std::string& var_name);
	std::string getVar(const std::string& var_name);
 private:
	void createVarIfNotExists(const std::string& var_name);
	void createNamedSetIfNotExists(const std::string& named_set_name);
 public:

	void clear();

	void clean();
};

extern TemplateASTNode *parseTemplate(TemplateLangExecEnv* pEE, const std::string& template_string, bool& bResult);

extern std::string escapeXMLEntities(const std::string& input);

#endif
