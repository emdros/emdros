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

/*
 * Copyright (C) 2001-2018     Ulrik Sandborg-Petersen
 * Copyright (C) 2018-present  Sandborg-Petersen Holding ApS, Denmark
 *
 * Licensed under the MIT License.
 *
 * Please see the file COPYING in the root of the sources for more details.
 *
 */


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
