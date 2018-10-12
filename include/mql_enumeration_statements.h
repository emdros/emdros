/*
 * mql_enumeration_statements.h
 *
 * Definitions of AST-classes for MQL
 *
 * Ulrik Petersen
 * Created: 11/22-2002
 * Last update: 3/14-2007
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


/**@file mql_enumeration_statements.h
 *@brief Header file for MQL ENUMERATION statements (MQL layer)
 */



#ifndef MQL_ENUMERATION_STATEMENTS__H__
#define MQL_ENUMERATION_STATEMENTS__H__

#include "mql_types.h"
#include "mql_execution_environment.h"
#include <set>

class EnumerationStatement : public Statement {
 protected:
	std::string* m_enumeration_name;
	id_d_t m_enum_id;
 public:
	EnumerationStatement(MQLExecEnv *pEE, 
			     std::string* enumeration_name) : Statement(pEE), 
		m_enumeration_name(enumeration_name), m_enum_id(NIL) {};
		virtual ~EnumerationStatement();
		bool symbolEnumerationExists(bool& bResult, bool bReportError); // Gets the enum_id
};

// Helper class ec_declaration
class ECDeclaration {
 private:
	ECDeclaration* m_next;
	bool m_is_default;
	std::string* m_ec_name;
	long* m_opt_ec_initialization;
	long m_value;
 public:
	ECDeclaration(bool is_default, std::string* ec_name, long* opt_ec_initialization, ECDeclaration* next);
	virtual ~ECDeclaration();
	void weedDefault(MQLExecEnv *pEE, bool& bResult);
	bool getIsDefault();
	bool hasOptEcInitialization() { return m_opt_ec_initialization != 0; };
	long getOptEcInitialization();
	const std::string& getEcName();
	long getValue(void) { return m_value; };
	ECDeclaration* getNext() { return m_next; };
	void setNext(ECDeclaration* next) { m_next = next; };
	bool symbolExistenceOfConstants(MQLExecEnv *pEE, id_d_t enum_id, bool& bResult);
	void symbolAssignValues(void);
	void symbolValueMoreThanOnce(MQLExecEnv *pEE, bool& bResult);
	bool createConstants(MQLExecEnv *pEE, id_d_t enum_id, const std::string& enum_name);
 private:
	void weedDefaultPrivate(MQLExecEnv *pEE, bool& bResult, bool& has_met_default);
	void symbolAssignValuesPrivate(long& previous);
	void symbolValueMoreThanOncePrivate(MQLExecEnv *pEE, bool& bResult, std::set<long>& value_set, std::set<std::string>& const_set);
};

// CREATE ENUMERATION
class CreateEnumerationStatement : public EnumerationStatement {
 protected:
	ECDeclaration* m_ec_declarations;
 public:
	CreateEnumerationStatement(MQLExecEnv *pEE, 
				   std::string* enumeration_name, 
				   ECDeclaration* ec_declaration_list);
	virtual ~CreateEnumerationStatement();
	virtual void weed(bool& bResult);
	virtual bool symbol(bool& bResult);
	virtual bool exec();
};

// Helper class: ec_update
class ECUpdate {
 private:
	ECUpdate* m_next;
	eAddUpdateRemove m_kind;
	std::string* m_ec_name;
	long m_ec_initialization;
	bool m_is_default;
 public:
	ECUpdate(bool is_default, 
		 std::string* ec_name, 
		 long ec_initialization, 
		 eAddUpdateRemove kind, 
		 ECUpdate* next);
	virtual ~ECUpdate();
	void weedDefault(MQLExecEnv *pEE, bool& bResult);
	bool getIsDefault() { return m_is_default; };
	long getEcInitialization() { return m_ec_initialization; };
	const std::string& getEcName() { return *m_ec_name; };
	eAddUpdateRemove getKind() { return m_kind; };
	ECUpdate* getNext() { return m_next; };
	void setNext(ECUpdate* next) { m_next = next; };
	bool symbolExistenceOfConstants(MQLExecEnv *pEE, id_d_t enum_id, bool& bResult);
	bool symbolDefaultIsRemoved(MQLExecEnv *pEE, id_d_t enum_id, bool& bDefaultIsRemoved);
	bool hasDefault(void);
	const std::string& getDefault(void) const; // Assumes that one is the default
	bool symbolNoTwoValuesSame(MQLExecEnv *pEE, id_d_t enum_id, bool& bResult);
	bool execAddUpdateRemove(MQLExecEnv *pEE, id_d_t enum_id, const std::string& enum_name);
  
 private:
	void weedDefaultPrivate(MQLExecEnv *pEE, bool& bResult, bool& has_met_default);
	void symbolDoLocalUpdatePrivate(std::list<EnumConstInfo>& enum_list);
	void symbolCheckNoTwoValuesSamePrivate(MQLExecEnv *pEE, const std::list<EnumConstInfo>& enum_list, std::set<long>& value_set, bool& bResult);
};

// UPDATE ENUMERATION
class UpdateEnumerationStatement : public EnumerationStatement {
 private:
	ECUpdate* m_ec_update_list;
 public:
	UpdateEnumerationStatement(MQLExecEnv *pEE, 
				   std::string* enumeration_name,
				   ECUpdate* ec_update_list);
	virtual ~UpdateEnumerationStatement();
	virtual void weed(bool& bResult);
	virtual bool symbol(bool& bResult);
	virtual bool exec();
};

// DROP ENUMERATION
class DropEnumerationStatement : public EnumerationStatement {
 public:
	DropEnumerationStatement(MQLExecEnv *pEE, 
				 std::string* enumeration_name);
	virtual ~DropEnumerationStatement();
	virtual bool symbol(bool& bResult);
	virtual bool exec();
};





#endif /* MQL_ENUMERATION_STATEMENTS__H__ */
