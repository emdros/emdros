/*
 * mql_monads_statements.h
 *
 * Definitions of AST-classes for MQL
 *
 * Ulrik Petersen
 * Created: 11/22-2002
 * Last update: 11/10-2005
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



#ifndef MQL_MONADS_STATEMENTS__H__
#define MQL_MONADS_STATEMENTS__H__

#include "mql_types.h"
#include "mql_execution_environment.h"
#include "mql_object_type_statements.h"


typedef enum { kSOUnion, kSOIntersect, kSODifference, kSONone, kSOReplace } eSetOperator;

class MonadSetChainElement {
private:
  MQLMonadSetElement *m_set;
  MonadSetChainElement *m_next;
  eSetOperator m_operator;
public:
  MonadSetChainElement(MQLMonadSetElement *myset, 
			eSetOperator op, 
			MonadSetChainElement *next);
  virtual ~MonadSetChainElement();
  MQLMonadSetElement *getSet(void) { return m_set; };
  eSetOperator getSetOperator(void) const { return m_operator; };
  MonadSetChainElement *getNext(void) { return m_next; };
  void weed(MQLExecEnv *pEE, bool& bResult);
  void setNext(MonadSetChainElement *next) { m_next = next; };
  void exec(SetOfMonads& monads);
};

// Class for monad set calculation
class MonadSetCalculationStatement : public Statement {
private:
  MonadSetChainElement *m_chain;
public:
  MonadSetCalculationStatement(MQLExecEnv *pEE, MonadSetChainElement *chain);
  virtual ~MonadSetCalculationStatement();
  virtual void weed(bool& bResult);
  virtual bool exec();
};

// Base class for Insert and Delete monads
class MonadsStatement : public Statement {
protected:
  MQLMonadSetElement* m_AST_monads;
  SetOfMonads m_monads;
  SetOfMonads m_segment;
public:
  MonadsStatement(MQLExecEnv *pEE, 
		   MQLMonadSetElement* monads) : Statement(pEE), m_AST_monads(monads) {};
  virtual ~MonadsStatement();
  virtual void weed(bool& bResult);
  virtual bool symbol(bool& bResult);
};

// INSERT MONADS
class InsertMonadsStatement : public MonadsStatement {
public:
  InsertMonadsStatement(MQLExecEnv *pEE, 
			 MQLMonadSetElement* monads) : MonadsStatement(pEE, monads) {};
  virtual ~InsertMonadsStatement() {};
  virtual void weed(bool& bResult);
  virtual bool symbol(bool& bResult);
  virtual bool exec();
};

// DELETE MONADS
class DeleteMonadsStatement : public MonadsStatement {
public:
  DeleteMonadsStatement(MQLExecEnv *pEE, 
			 MQLMonadSetElement* monads) : MonadsStatement(pEE, monads) {};
  virtual ~DeleteMonadsStatement() {};
  virtual void weed(bool& bResult);
  virtual bool symbol(bool& bResult);
  virtual bool exec();
};


// GET MONADS
class GetMonadsStatement : public ObjectTypeStatement {
  ID_D* m_id_ds;
public:
  GetMonadsStatement(MQLExecEnv *pEE, 
		      ID_D *pID_Ds, 
		      std::string* object_type_name);
  virtual ~GetMonadsStatement();
  virtual bool symbol(bool& bResult);
  virtual bool exec();
};

// GET MONAD SETS
class GetMonadSetsStatement : public Statement {
protected:
  StringListNode* m_pMonad_set_names;
  bool m_bAll;
  std::list<std::string> m_monad_set_names;
  std::list<SetOfMonads> m_monad_sets;
public:
  GetMonadSetsStatement(MQLExecEnv *pEE, 
			 StringListNode* monad_set_name_list) 
    : Statement(pEE), m_pMonad_set_names(monad_set_name_list), m_bAll(false) {};
  GetMonadSetsStatement(MQLExecEnv *pEE)
    : Statement(pEE), m_pMonad_set_names(0), m_bAll(true) {};
  virtual ~GetMonadSetsStatement();
  virtual bool symbol(bool& bResult);
  virtual bool exec();
};

// Base class for CREATE/UPDATE/DROP MONAD SET
class MonadSetStatement : public Statement {
protected:
  std::string *m_pMonadSetName;
  bool m_bCheckExistence;
  SetOfMonads m_som;
public:
  MonadSetStatement(MQLExecEnv *pEE, 
		    std::string *monad_set_name,
		    bool bCheckExistence) 
  : Statement(pEE), 
    m_pMonadSetName(monad_set_name), 
    m_bCheckExistence(bCheckExistence) {};
  virtual ~MonadSetStatement();
  virtual bool symbol(bool& bResult);
};


// CREATE/UPDATE MONAD SET
class CreateOrUpdateMonadSetStatement : public MonadSetStatement {
protected:
  std::string *m_pOtherMonadSetName;
  MQLMonadSetElement *m_pMonadSet;
  SetOfMonads m_other_som;
public:
  CreateOrUpdateMonadSetStatement(MQLExecEnv *pEE, 
				   std::string *monad_set_name, 
				   MQLMonadSetElement *pMonadSet,
				   bool bCheckExistence)
  : MonadSetStatement(pEE, monad_set_name, bCheckExistence), 
    m_pOtherMonadSetName(0), 
    m_pMonadSet(pMonadSet) {};
  CreateOrUpdateMonadSetStatement(MQLExecEnv *pEE, 
				   std::string *monad_set_name, 
				   std::string *other_monad_set_name,
				   bool bCheckExistence)
  : MonadSetStatement(pEE, monad_set_name, bCheckExistence), 
    m_pOtherMonadSetName(other_monad_set_name), 
    m_pMonadSet(0) {};
  virtual ~CreateOrUpdateMonadSetStatement();
  virtual void weed(bool& bResult);
  virtual bool symbol(bool& bResult);
  virtual bool monads(bool& bResult);
};


// CREATE MONAD SET
class CreateMonadSetStatement : public CreateOrUpdateMonadSetStatement {
public:
  CreateMonadSetStatement(MQLExecEnv *pEE, 
			   std::string *monad_set_name, 
			   MQLMonadSetElement *pMonadSet)
    : CreateOrUpdateMonadSetStatement(pEE, monad_set_name, pMonadSet, false) {};
  virtual ~CreateMonadSetStatement() {};
  virtual bool exec();
};

// UPDATE MONAD SET
class UpdateMonadSetStatement : public CreateOrUpdateMonadSetStatement {
private:
  eSetOperator m_op;
public:
  UpdateMonadSetStatement(MQLExecEnv *pEE, 
			   std::string *monad_set_name, 
			   eSetOperator op,
			   MQLMonadSetElement *pMonadSet)
    : CreateOrUpdateMonadSetStatement(pEE, monad_set_name, pMonadSet, true), m_op(op) {};
  UpdateMonadSetStatement(MQLExecEnv *pEE, 
			   std::string *monad_set_name, 
			   eSetOperator op,
			   std::string *other_monad_set_name)
    : CreateOrUpdateMonadSetStatement(pEE, monad_set_name, other_monad_set_name, true), m_op(op) {}
  virtual ~UpdateMonadSetStatement() {};
  virtual bool exec();
};


// DROP MONAD SET
class DropMonadSetStatement : public MonadSetStatement {
public:
  DropMonadSetStatement(MQLExecEnv *pEE, 
		   std::string *monad_set_name) 
  : MonadSetStatement(pEE, monad_set_name, true) {};
  virtual ~DropMonadSetStatement() {};
  virtual bool exec();
};



#endif /* MQL_MONADS_STATEMENTS__H__ */
