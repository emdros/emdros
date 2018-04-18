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
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2002-2007  Ulrik Petersen
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
