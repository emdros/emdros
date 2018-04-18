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
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2002-2005  Ulrik Petersen
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
