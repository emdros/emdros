/*
 * mql_select_statements.h
 *
 * Definitions of AST-classes for MQL
 *
 * Ulrik Petersen
 * Created: 11/22-2002
 * Last update: 7/21-2016
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2002-2016  Ulrik Sandborg-Petersen
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

/**@file mql_select_statements.h
 *@brief Header file for MQL SELECT statements (MQL layer)
 */



#ifndef MQL_SELECT_STATEMENTS__H__
#define MQL_SELECT_STATEMENTS__H__

#include "mql_types.h"
#include "mql_enumeration_statements.h"
#include "mql_object_type_statements.h"
#include "mql_execution_environment.h"


// SELECT (focus|all|) OBJECTS
class SelectStatement : public Statement, public QueryBase {
public:
  SelectStatement(MQLExecEnv *pEE, 
		  eFocusSpec focus, 
		  MQLMonadSetElement* in_spec, 
		  MaxRange *pMaxRange,
		  sheaf_return_type_pair *sheaf_return_type,
		  Topograph* query);
  SelectStatement(MQLExecEnv *pEE, 
		  eFocusSpec focus, 
		  std::string* arbitrary_monad_set,
		  MaxRange *pMaxRange,
		  sheaf_return_type_pair *sheaf_return_type,
		  Topograph* query);
  virtual ~SelectStatement();
  virtual void weed(bool& bResult);
  virtual bool symbol(bool& bResult);
  virtual bool type(bool& bResult);
  virtual bool monads(bool& bResult);
  virtual bool exec();
};

// SELECT OBJECTS AT
class SelectObjectsAtStatement : public Statement {
private:
  monad_m m_at_monad;
  std::string* m_object_type_name;
  id_d_t m_object_type_id;
public:
  SelectObjectsAtStatement(MQLExecEnv *pEE, 
			    monad_m at_monad, std::string* object_type_name);
  virtual ~SelectObjectsAtStatement();
  virtual bool symbol(bool& bResult);
  virtual bool exec();
};

// SELECT OBJECTS HAVING MONADS IN
class SelectObjectsHavingMonadsInStatement : public ObjectTypesStatement {
private:
  MQLMonadSetElement* m_AST_monads;
  SetOfMonads m_monads;
public:
  SelectObjectsHavingMonadsInStatement(MQLExecEnv *pEE, 
					MQLMonadSetElement* monads, 
					const std::string& object_type_name);
  virtual ~SelectObjectsHavingMonadsInStatement();
  virtual void weed(bool& bResult);
  virtual bool monads(bool& bResult);
  virtual bool exec();
private:
  bool execOneType(const ObjectTypeInfo& oti);
};

// GET OBJECTS HAVING MONADS IN
class GetObjectsHavingMonadsInStatement : public ObjectTypeStatement {
private:
  MQLMonadSetElement* m_AST_monads;
  SetOfMonads m_monads;
  bool m_bGetAll;
  Feature *m_pFeaturesToGet;
  ObjectBlockBase *m_pObjectBlockBase; // Don't delete!
  std::string *m_monads_feature;
public:
  GetObjectsHavingMonadsInStatement(MQLExecEnv *pEE, 
				    MQLMonadSetElement* monads, 
				    std::string *object_type_name,
				    std::string *monads_feature);
  GetObjectsHavingMonadsInStatement(MQLExecEnv *pEE, 
				    MQLMonadSetElement* monads, 
				    std::string *object_type_name,
				    std::string *monads_feature,
				    bool bDummy);
  GetObjectsHavingMonadsInStatement(MQLExecEnv *pEE, 
				    MQLMonadSetElement* monads, 
				    std::string *object_type_name,
				    std::string *monads_feature,
				    GrammarFeature *pFeaturesToGet);
  virtual ~GetObjectsHavingMonadsInStatement();
  virtual void weed(bool& bResult);
  virtual bool symbol(bool& bResult);
  virtual bool type(bool& bResult);
  virtual bool monads(bool& bResult);
  virtual bool exec();
};

// GET AGGREGATE FEATURES
class GetAggregateFeaturesStatement : public ObjectTypeStatement {
private:
	AggregateFeature *m_aggregate_feature_list;
	MQLMonadSetElement* m_AST_monads;
	std::string *m_monads_feature;
	FFeatures *m_feature_constraints;
	Feature *m_pFeaturesToGet;
	ObjectBlock *m_pObjectBlock;
	SetOfMonads m_monads;
 public:
	GetAggregateFeaturesStatement(MQLExecEnv *pEE,
				      AggregateFeature *aggregate_feature_list,
				      MQLMonadSetElement* in_clause,
				      std::string *monads_feature,
				      std::string *object_type_name,
				      FFeatures *feature_constraints);
	virtual ~GetAggregateFeaturesStatement();
	virtual void weed(bool& bResult);
	virtual bool symbol(bool& bResult);
	virtual bool type(bool& bResult);
	virtual bool monads(bool& bResult);
	virtual bool exec();
};

// SELECT OBJECT TYPES
class SelectObjectTypesStatement : public Statement {
public:
  SelectObjectTypesStatement(MQLExecEnv *pEE);
  virtual ~SelectObjectTypesStatement();
  virtual bool exec();
};

// SELECT FEATURES
class SelectFeaturesStatement : public ObjectTypeStatement {
public:
  SelectFeaturesStatement(MQLExecEnv *pEE, 
			   std::string* object_type_name) : ObjectTypeStatement(pEE, object_type_name) {};
  virtual ~SelectFeaturesStatement() {}
  virtual bool symbol(bool& bResult);
  virtual bool exec();
};

// SELECT ENUMERATIONS
class SelectEnumerationsStatement : public Statement {
public:
  SelectEnumerationsStatement(MQLExecEnv *pEE);
  virtual ~SelectEnumerationsStatement();
  virtual bool exec();
};

// SELECT ENUMERATION CONSTANTS
class SelectEnumerationConstantsStatement : public EnumerationStatement {
public:
  SelectEnumerationConstantsStatement(MQLExecEnv *pEE, 
				       std::string* enumeration_name) : EnumerationStatement(pEE, enumeration_name) {};
  virtual ~SelectEnumerationConstantsStatement() {};
  virtual bool symbol(bool& bResult);
  virtual bool exec();
};

// SELECT OBJECT TYPES USING ENUMERATION
class SelectObjectTypesUsingEnumerationStatement : public EnumerationStatement {
public:
  SelectObjectTypesUsingEnumerationStatement(MQLExecEnv *pEE, 
					      std::string* enumeration_name) : EnumerationStatement(pEE, enumeration_name) {};
  virtual ~SelectObjectTypesUsingEnumerationStatement() {};
  virtual bool symbol(bool& bResult);
  virtual bool exec();
};

// SELECT MAX_M
class SelectMaxMStatement : public Statement {
public:
  SelectMaxMStatement(MQLExecEnv *pEE) : Statement(pEE) {};
  virtual ~SelectMaxMStatement() {};
  virtual bool exec();
};

// SELECT MIN_M
class SelectMinMStatement : public Statement {
public:
  SelectMinMStatement(MQLExecEnv *pEE) : Statement(pEE) {};
  virtual ~SelectMinMStatement() {};
  virtual bool exec();
};

// SELECT MONAD SETS
class SelectMonadSetsStatement : public Statement {
public:
  SelectMonadSetsStatement(MQLExecEnv *pEE) : Statement(pEE) {};
  virtual ~SelectMonadSetsStatement() {};
  virtual bool exec();
};



#endif /* MQL_SELECT_STATEMENTS__H__ */
