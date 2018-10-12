/*
 * mql_object_type_statements.h
 *
 * Definitions of AST-classes for MQL
 *
 * Ulrik Petersen
 * Created: 11/22-2002
 * Last update: 6/17-2011
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


/**@file mql_object_type_statements.h
 *@brief Header file for MQL OBJECT TYPE statements (MQL layer)
 */



#ifndef MQL_OBJECT_TYPE_STATEMENTS__H__
#define MQL_OBJECT_TYPE_STATEMENTS__H__

#include "mql_types.h"
#include "mql_execution_environment.h"
#include "debug.h"


// Base class for the OBJECT TYPE statements
class ObjectTypeStatement : public Statement {
protected:
  std::string* m_object_type_name;
  id_d_t m_object_type_id;
  eObjectRangeType m_objectRangeType;
  eMonadUniquenessType m_monadUniquenessType;
public:
  ObjectTypeStatement(MQLExecEnv *pEE, std::string* object_type_name) : Statement(pEE), m_object_type_name(object_type_name)  {};
  virtual ~ObjectTypeStatement();
protected:
  virtual bool symbolObjectTypeExists(bool& bObjectTypeExists);
  eObjectRangeType getObjectRangeType(void) const { return m_objectRangeType; };
};



// Base class for statements that require one or more ("all") object types
class ObjectTypesStatement : public Statement {
protected:
  std::list<ObjectTypeInfo> m_object_type_list;
  std::string m_object_type_name;
public:
  ObjectTypesStatement(MQLExecEnv *pEE, 
			const std::string& object_type_name) : Statement(pEE), m_object_type_name(object_type_name)  {};
  virtual ~ObjectTypesStatement();
  virtual bool symbol(bool& bResult);
  virtual bool exec();
protected:
  virtual bool symbolObjectTypeExists(const std::string& object_type_name, bool&bResult);
  virtual bool execOneType(const ObjectTypeInfo& oti) = 0;
};




// Helper class
class FeatureDeclaration {
private:
  FeatureDeclaration* m_next;
  std::string* m_feature_name;
  MQLType* m_type;
  Expression* m_default_specification;
  bool m_is_computed;
public:
  FeatureDeclaration(std::string* feature_name,
		     MQLType* feature_type,
		     Expression* default_specification,
		     bool is_computed,
		     FeatureDeclaration* next);
  virtual ~FeatureDeclaration();
  FeatureDeclaration* getNext() { return m_next; };
  void setNext(FeatureDeclaration* next) { m_next = next; };
  const std::string& getFeatureName() const { return *m_feature_name; };
  MQLType* getType() { return m_type; };
  Expression* getDefaultSpecification() { return m_default_specification; };
  bool getIsComputed() { return m_is_computed; };
  virtual void weed(MQLExecEnv *pEE, bool& bResult);
  bool symbolEnumerationsExist(MQLExecEnv *pEE, bool& bResult);
  bool symbolEnumConstantsExist(MQLExecEnv *pEE, bool& bResult);
  bool symbolFeaturesMustNotExist(MQLExecEnv *pEE, bool& bResult, id_d_t object_type_id);
  bool getFeatureInfo(MQLExecEnv *pEE, FeatureInfo& fi);
  bool typeAssignTypeID(MQLExecEnv *pEE);
  bool typeTypeCompatibility(MQLExecEnv *pEE, bool& bResult);
  bool execAdd(MQLExecEnv *pEE, const std::string& object_type_name, id_d_t object_type_id);
private:
  void weedSelfNotDeclared(MQLExecEnv *pEE, bool& bResult);
};


// CREATE OBJECT TYPE
class CreateObjectTypeStatement : public ObjectTypeStatement {
private:
  FeatureDeclaration* m_features;
  eObjectRangeType m_objectRangeType;
  eMonadUniquenessType m_monadUniquenessType;
  bool m_bIfNotExists;
  bool m_bObjectTypeExistsAlready;
public:
  CreateObjectTypeStatement(MQLExecEnv *pEE, 
			    std::string* object_type_name, 
			    eObjectRangeType objectRangeType,
			    eMonadUniquenessType monadUniquenessType,
			    FeatureDeclaration* feature_declaration_list,
			    bool bIfNotExists);
  virtual ~CreateObjectTypeStatement();
  virtual void weed(bool& bResult);
  virtual bool symbol(bool& bResult);
  virtual bool type(bool& bResult);
  virtual bool exec();
};


// Helper class feature_update
class FeatureUpdate {
private:
  FeatureUpdate* m_next;
  FeatureDeclaration* m_feature_addition;
  std::string* m_feature_removal;
  id_d_t m_feature_type_id_for_removal;
  eAddUpdateRemove m_AddOrRemove;
public:
  FeatureUpdate(FeatureDeclaration* feature_declaration, FeatureUpdate* next);
  FeatureUpdate(std::string* feature_name, FeatureUpdate* next);
  virtual ~FeatureUpdate();
  eAddUpdateRemove getKind() { return m_AddOrRemove; };
  const std::string& getFeatureRemoval() const { ASSERT_THROW(m_AddOrRemove == kRemove, "not removing"); return *m_feature_removal; };
  FeatureDeclaration* getFeatureAddition() { ASSERT_THROW(m_AddOrRemove == kAdd, "not adding"); return m_feature_addition; };
  FeatureUpdate* getNext() { return m_next; };
  void setNext(FeatureUpdate* next) { m_next = next; }
  virtual void weed(MQLExecEnv *pEE, bool& bResult);
  bool symbolExistenceOfFeatures(MQLExecEnv *pEE, bool& bResult, id_d_t object_type_id);
  bool symbolEnumerations(MQLExecEnv *pEE, bool& bResult);
  bool typeAssignTypeID(MQLExecEnv *pEE);
  bool typeTypeCompatibility(MQLExecEnv *pEE, bool& bResult);
  virtual bool exec(MQLExecEnv *pEE, const std::string& object_type_name, id_d_t object_type_id);
};

// UPDATE OBJECT TYPE
class UpdateObjectTypeStatement : public ObjectTypeStatement {
private:
  FeatureUpdate* m_features;
public:
  UpdateObjectTypeStatement(MQLExecEnv *pEE, 
			    std::string* object_type_name, 
			    FeatureUpdate* feature_update_list);
  virtual ~UpdateObjectTypeStatement();
  virtual void weed(bool& bResult);
  virtual bool symbol(bool& bResult);
  virtual bool type(bool& bResult);
  virtual bool exec();
};

// DROP OBJECT TYPE
class DropObjectTypeStatement : public ObjectTypeStatement {
public:
  DropObjectTypeStatement(MQLExecEnv *pEE, 
			   std::string* object_type_name);
  virtual ~DropObjectTypeStatement() {};
  virtual bool symbol(bool& bResult);
  virtual bool exec();
};




#endif /* MQL_OBJECT_TYPE_STATEMENTS__H__ */
