/*
 * mql_object_statements.h
 *
 * Definitions of AST-classes for MQL
 *
 * Ulrik Petersen
 * Created: 11/22-2002
 * Last update: 11/16-2011
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



#ifndef MQL_OBJECT_STATEMENTS__H__
#define MQL_OBJECT_STATEMENTS__H__

#include "mql_types.h"
#include "mql_object_type_statements.h"
#include "mql_execution_environment.h"


class InstObject; // Forward declaration


// Base class for ObjectSpec and ObjectSpecNoOT
class ObjectSpecBase {
 protected:
	FeatureAssignment* m_feature_assignments;
 public:
	ObjectSpecBase(FeatureAssignment* feature_assignment_list);
	virtual ~ObjectSpecBase();
	void weed(MQLExecEnv *pEE, bool& bResult);
	void weedObjectTypeNameNotSpecial(MQLExecEnv *pEE, const std::string& object_type_name, bool& bResult);
	virtual void assignFeatureIndexes(const FeatureEnv& feature_env);
	FeatureAssignment* getFeatureAssignments() { return m_feature_assignments; };
	bool symbolObjectTypeExists(MQLExecEnv *pEE, 
				    const std::string& object_type_name, 
				    id_d_t&  /* out */ object_type_id, 
				    bool& bTypeExists,
				    eObjectRangeType& objectRangeType,
				    eMonadUniquenessType& monadUniquenessType);
	bool symbolAllFeaturesMustBeAssigned(MQLExecEnv *pEE, 
					     id_d_t object_type_id, 
					     const FeatureEnv& feature_env);
	bool symbolAllFeaturesMustExist(MQLExecEnv *pEE, 
					const std::string& object_type_name, id_d_t object_type_id, 
					const FeatureEnv& feature_env,
					bool& bResult);
	bool symbolNoFeatureMayBeComputed(MQLExecEnv *pEE, bool& bResult);
	bool execMakeFeatureList(MQLExecEnv *pEE, 
				 const FeatureEnv& feature_env,
				 std::list<FeatureInfo>& features);
	bool typeTypeCompatibility(MQLExecEnv *pEE, const FeatureEnv& feature_env, bool& bResult);
};

// Helper class: object_creation_specification
class ObjectSpec : public ObjectSpecBase {
 private:
	std::string* m_object_type_name;
	id_d_t m_object_type_id;
	eObjectRangeType m_objectRangeType;
	eMonadUniquenessType m_monadUniquenessType;
 public:
	ObjectSpec(std::string* object_type_name,
		   FeatureAssignment* feature_assignment_list);
	virtual ~ObjectSpec();
	void weedObjectTypeNameNotSpecial(MQLExecEnv *pEE, bool& bResult);
	const std::string& getObjectTypeName() const { return *m_object_type_name; };
	id_d_t getObjectTypeId(void) const { return m_object_type_id; };
	eObjectRangeType getObjectRangeType(void) const { return m_objectRangeType; };
	bool symbolObjectTypeExists(MQLExecEnv *pEE, bool& bTypeExists);
	bool symbolAllFeaturesMustBeAssigned(MQLExecEnv *pEE,
					     const FeatureEnv& feature_env);
	bool symbolAllFeaturesMustExist(MQLExecEnv *pEE, 
					const FeatureEnv& feature_env,
					bool& bResult);
};

// Helper class: object_creation_no_object_type
class ObjectSpecNoOT : public ObjectSpecBase, public ByMonads {
	id_d_t m_with_id_d;
	ObjectSpecNoOT* m_next;
	ObjectSpecNoOT* m_prev;
 public:
	ObjectSpecNoOT(MQLMonadSetElement *pMonadSet,
		       id_d_t with_id_d,
		       FeatureAssignment* feature_assignment_list,
		       ObjectSpecNoOT *next);
	virtual ~ObjectSpecNoOT();
	void weed(MQLExecEnv *pEE, 
		  bool& bResult);
	void DoWeed(MQLExecEnv *pEE, 
		    bool& bResult);
	bool symbol(MQLExecEnv *pEE, 
		    const std::string& object_type_name, 
		    id_d_t object_type_id, 
		    const FeatureEnv& feature_env,
		    bool &bResult);
	bool DoSymbol(MQLExecEnv *pEE, 
		      const std::string& object_type_name, 
		      id_d_t object_type_id, 
		      const FeatureEnv& feature_env,

		      bool &bResult);
	virtual void assignFeatureIndexes(const FeatureEnv& feature_env);
	bool type(MQLExecEnv *pEE, 
		  id_d_t object_type_id, 
		  const std::list<FeatureInfo>& object_type_features, 
		  const FeatureEnv& feature_env,
		  bool &bResult);
	bool DoType(MQLExecEnv *pEE, 
		    id_d_t object_type_id, 
		    const std::list<FeatureInfo>& object_type_features, 
		    const FeatureEnv& feature_env,
		    bool &bResult);
	bool monads(MQLExecEnv *pEE, 
		    eObjectRangeType objectRangeType,
		    bool& bResult);
	bool DoMonads(MQLExecEnv *pEE, 
		      eObjectRangeType objectRangeType,
		      bool& bResult);
	bool exec(MQLExecEnv *pEE, 
		  std::list<InstObject*>& output_list, 
		  const FeatureEnv& feature_env);
	bool DoExec(MQLExecEnv *pEE, 
		    std::list<InstObject*>& output_list, 
		    const FeatureEnv& feature_env);
	ObjectSpecNoOT* getNext() { return m_next; };
	void setNext(ObjectSpecNoOT* next) { m_next = next; };
 private:
	void execMakeFeatureArray(MQLExecEnv *pEE, 
				  const FeatureEnv& feature_env,
				  EMdFValue *feature_value_arr);
};

void disposeOfObjectSpecNoOT(ObjectSpecNoOT **ppHead);

// Base class for all CREATE OBJECT statements
class CreateObjectStatement : public Statement {
 protected:
	id_d_t m_with_id_d;
	ObjectSpec* m_object_creation_spec;
	std::list<FeatureInfo> m_object_type_features;
	FeatureEnv m_feature_env;
	SetOfMonads m_monads;
 public:
	CreateObjectStatement(MQLExecEnv *pEE, 
			      id_d_t with_id_d, 
			      ObjectSpec* object_creation_spec);
	virtual ~CreateObjectStatement();
	virtual void weed(bool& bResult);
	virtual bool symbol(bool& bResult);
	virtual bool type(bool& bResult);
	virtual bool monads(bool& bResult);
	virtual bool exec();
protected:
	virtual bool getFeatureEnv(void);
};

// CREATE OBJECT FROM MONADS
class CreateObjectFromMonadsStatement : public CreateObjectStatement {
 private:
	MQLMonadSetElement* m_monad_set;
 public:
	CreateObjectFromMonadsStatement(MQLExecEnv *pEE, 
					MQLMonadSetElement* monad_set,
					id_d_t with_id_d,
					ObjectSpec* object_creation_spec);
	virtual ~CreateObjectFromMonadsStatement();
	virtual void weed(bool& bResult);
	virtual bool symbol(bool& bResult);
	virtual bool monads(bool& bResult);
	virtual bool exec();
};

// CREATE OBJECT FROM ID_DS
class CreateObjectFromID_DsStatement : public CreateObjectStatement {
 private:
	ID_D* m_id_d_list;
 public:
	CreateObjectFromID_DsStatement(MQLExecEnv *pEE, 
				       ID_D* id_d_specification,
				       id_d_t with_id_d,
				       ObjectSpec* object_creation_spec);
	virtual ~CreateObjectFromID_DsStatement();
	virtual void weed(bool& bResult);
	virtual bool symbol(bool& bResult);
	virtual bool monads(bool& bResult);
	virtual bool exec();
};

// CREATE OBJECT FROM (focus|all|) QUERY
class CreateObjectFromQueryStatement : public CreateObjectStatement, public QueryBase {
 public:
	CreateObjectFromQueryStatement(MQLExecEnv *pEE, 
				       eFocusSpec focus, 
				       id_d_t with_id_d,
				       ObjectSpec* object_creation_spec,
				       MQLMonadSetElement* in_spec, 
				       MaxRange *max_range,
				       sheaf_return_type_pair *sheaf_return_type,
				       Topograph* query);
	virtual ~CreateObjectFromQueryStatement();
	virtual void weed(bool& bResult);
	virtual bool symbol(bool& bResult);
	virtual bool type(bool& bResult);
	virtual bool monads(bool& bResult);
	virtual bool exec();
};

// Base class for all CREATE OBJECT statements
class CreateObjectsStatement : public Statement {
 protected:
	ObjectSpecNoOT* m_object_spec_list;
	std::string *m_pObject_type_name;
	std::list<FeatureInfo> m_object_type_features;
	id_d_t m_object_type_id;
	eObjectRangeType m_objectRangeType;
	FeatureEnv m_feature_env;
 public:
	CreateObjectsStatement(MQLExecEnv *pEE, 
			       std::string *pObject_type_name,
			       ObjectSpecNoOT* object_spec_list);
	virtual ~CreateObjectsStatement();
	virtual void weed(bool& bResult);
	virtual bool symbol(bool& bResult);
	virtual bool type(bool& bResult);
	virtual bool monads(bool& bResult);
	virtual bool exec();
protected:
	bool getFeatureEnv(void);
};


// Base class for all UPDATE OBJECTS statements
class UpdateObjectsStatement : public Statement {
 protected:
	ObjectSpec* m_update_spec;
	std::list<FeatureInfo> m_object_type_features;
	std::list<id_d_t> m_update_candidates;
	FeatureEnv m_feature_env;
 public:
	UpdateObjectsStatement(MQLExecEnv *pEE, 
			       ObjectSpec* update_spec);
	virtual ~UpdateObjectsStatement();
	virtual void weed(bool& bResult);
	virtual bool symbol(bool& bResult);
	virtual bool type(bool& bResult);
	virtual bool exec();
 protected:
	virtual bool getFeatureEnv(void);
};

// UPDATE OBJECTS BY MONADS
class UpdateObjectsByMonadsStatement : public UpdateObjectsStatement, public ByMonads {
 public:
	UpdateObjectsByMonadsStatement(MQLExecEnv *pEE, 
				       MQLMonadSetElement* monad_set,
				       ObjectSpec* update_spec);
	virtual ~UpdateObjectsByMonadsStatement();
	virtual void weed(bool& bResult);
	virtual bool symbol(bool& bResult);
	virtual bool type(bool& bResult);
	virtual bool monads(bool& bResult);
	virtual bool exec();
};

// UPDATE OBJECTS BY ID_DS
class UpdateObjectsByID_DsStatement : public UpdateObjectsStatement {
 private:
	ID_D* m_id_d_list;
 public:
	UpdateObjectsByID_DsStatement(MQLExecEnv *pEE, 
				      ID_D* id_d_specification,
				      ObjectSpec* update_spec);
	virtual ~UpdateObjectsByID_DsStatement();
	virtual void weed(bool& bResult);
	virtual bool symbol(bool& bResult);
	virtual bool type(bool& bResult);
	virtual bool exec();
};

// UPDATE OBJECTS BY (focus|all|) QUERY
class UpdateObjectsByQueryStatement : public UpdateObjectsStatement, public QueryBase {
 public:
	UpdateObjectsByQueryStatement(MQLExecEnv *pEE, 
				      eFocusSpec focus, 
				      ObjectSpec* update_spec,
				      MQLMonadSetElement* in_spec, 
				      MaxRange *max_range,
				      sheaf_return_type_pair *sheaf_return_type,
				      Topograph* query);
	virtual ~UpdateObjectsByQueryStatement();
	virtual void weed(bool& bResult);
	virtual bool symbol(bool& bResult);
	virtual bool type(bool& bResult);
	virtual bool exec();
};


// Base class for all DELETE OBJECTS statements
class DeleteObjectsStatement : public ObjectTypesStatement {
 protected:
	std::list<id_d_t> m_delete_candidates;
 public:
	DeleteObjectsStatement(MQLExecEnv *pEE, 
			       const std::string& object_type_name);
	virtual ~DeleteObjectsStatement();
	virtual bool exec();
 protected:
	virtual bool execOneType(const ObjectTypeInfo& oti);
};

// DELETE OBJECTS BY MONADS
class DeleteObjectsByMonadsStatement : public DeleteObjectsStatement, public ByMonads {
 public:
	DeleteObjectsByMonadsStatement(MQLExecEnv *pEE, 
				       MQLMonadSetElement* monad_set,
				       const std::string& object_type_name);
	virtual ~DeleteObjectsByMonadsStatement();
	virtual void weed(bool& bResult);
	virtual bool monads(bool& bResult);
	virtual bool exec();
 protected:
	virtual bool execOneType(const ObjectTypeInfo& oti);
};

// DELETE OBJECTS BY ID_DS
class DeleteObjectsByID_DsStatement : public DeleteObjectsStatement {
 private:
	ID_D* m_id_d_list;
 public:
	DeleteObjectsByID_DsStatement(MQLExecEnv *pEE, 
				      ID_D* id_d_specification,
				      const std::string& object_type_name);
	virtual ~DeleteObjectsByID_DsStatement();
	virtual void weed(bool& bResult);
	virtual bool symbol(bool& bResult);
	virtual bool exec();
};

// DELETE OBJECTS BY (focus|all|) QUERY
class DeleteObjectsByQueryStatement : public DeleteObjectsStatement, public QueryBase {
 public:
	DeleteObjectsByQueryStatement(MQLExecEnv *pEE, 
				      eFocusSpec focus, 
				      const std::string& object_type_name,
				      MQLMonadSetElement* in_spec, 
				      MaxRange *max_range,
				      sheaf_return_type_pair *sheaf_return_type,
				      Topograph* query);
	virtual ~DeleteObjectsByQueryStatement();
	virtual void weed(bool& bResult);
	virtual bool symbol(bool& bResult);
	virtual bool type(bool& bResult);
	virtual bool exec();
};




#endif /* MQL_OBJECT_STATEMENTS__H__ */
