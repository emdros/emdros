/*
 * mql_types.h
 *
 * Definitions of AST-classes for MQL
 *
 * Ulrik Petersen
 * Created: 2/27-2001
 * Last update: 6/8-2018
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2018  Ulrik Sandborg-Petersen
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

/**@file mql_types.h
 *@brief Header file for central MQL types (MQL layer)
 */



#ifndef MQL_TYPES__H__
#define MQL_TYPES__H__

/**
 *\defgroup MQL MQL layer
 */


#ifndef SWIG
#include "mql_result.h"
#include "mql_enums.h"
#include "emdf.h"
#include "emdf_hash.h"
#include <string>
#include <map>
#include <list>
#include "infos.h"
#include "monads.h"
#include "debug.h"
#include <utility>


class FeatureComparison; // Forward declaration
class Topograph; // Forward declaration
class MQLExecEnv; // Forward declaration
class MQLObject; // Forward declaration
class MatchedObject; // Forward declaration
class EMdFValue; // Forward declaration
class StringList; // Forward declaration
class IntegerList; // Forward declaration
class MQLMonadSetElement; // Forward declaration
class FeatureEnv; // Forward declaration


/** What kind of operation? (Add? Update? Remove?).
 */
typedef enum { kAdd, kUpdate, kRemove } eAddUpdateRemove;

#endif // !defined SWIG

// Base class for all statements
/** Base class for all MQL statements.
 *\ingroup MQL
 * \section Overview
 *
 * The Statement class is the base class for all MQL statements. It
 * contains the MQLResult object associated with each execution of a
 * statement.
 *
 * When executing more than one statement at a time in a single call
 * to EmdrosEnv::executeXXX or mqlExecuteXXX, only the results from
 * the last statement executed are available afterwards. Executing
 * more than one query is mostly used when parsing the output from the
 * mql(1) program.  
 *
 * \section Example
 *
 * You can get to the results of an MQL statement via the following
 * expression:
 *
 * \code
 * pEE->getStatement()->getResult(); // pEE is the EmdrosEnv environment
 * \endcode
 */
class Statement {
 private:
	bool m_bDeleteResult; 
 protected:
	MQLResult* m_result;
	MQLExecEnv *m_pEE;
 public:
	Statement(MQLExecEnv *pEE);
	virtual ~Statement();
	MQLResult* getResult();
#ifndef SWIG
	virtual void weed(bool& bResult);
	virtual bool symbol(bool& bResult);
	virtual bool type(bool& bResult);
	virtual bool monads(bool& bResult);
	virtual bool exec();
	void setDeleteResult(bool bDoDelete);
#endif // !defined SWIG
};


class MaxRange {
 protected:
	monad_m m_max_range;
	std::string m_object_type_name;
	std::string m_SOM_feature_name;
	id_d_t m_object_type_id;
	eObjectRangeType m_objectRangeType;
	eMonadUniquenessType m_monadUniquenessType;
	id_d_t m_feature_type_id;
 public:
	MaxRange(monad_m max_range);
	MaxRange(const std::string* pOTN, const std::string *pSOMFeatureName);
	virtual ~MaxRange();
	monad_m getMaxRange() const { return m_max_range; };
	virtual void weed(MQLExecEnv *pEE, bool& bResult);
	virtual bool symbol(MQLExecEnv *pEE, bool& bResult);
	virtual bool type(MQLExecEnv *pEE, bool& bResult);
	virtual bool monads(MQLExecEnv *pEE, bool& bResult);
	virtual bool exec(MQLExecEnv *pEE);
};

#ifndef SWIG
/** MQLType-type.
 */
typedef enum { kInteger, kString, kASCII, kID_D, kEnumeration, 
	       kListOfInteger, kListOfID_D, kListOfEnumeration, kSetOfMonads } eType;


// Helper class
class MQLType {
 private:
	eType m_type;
	std::string* m_Enumeration;
	id_d_t m_type_id;
	bool m_with_index;
	bool m_from_set;
	eObjectRangeType m_som_range_type;
 public:
	MQLType(std::string* Enumeration, bool list_of, bool with_index); // For Enumeration
	MQLType(eType type, bool from_set, bool with_index); // For kString and kASCII
	MQLType(eType type, bool with_index); // For kInteger and kID_D
	MQLType(eType type); // For kListOfInteger and kListOfID_D
	MQLType(eObjectRangeType som_range_type); // For kORTSingleMonad, kORTSingleRange, and kORTMultipleRange
	virtual ~MQLType();
	eType getType(void) const { return m_type; };
	bool getWithIndex(void) const { return m_with_index; };
	bool getFromSet(void) const { return m_from_set; };
	bool getListOf(void) const;
	const std::string& getEnumeration(void) const { ASSERT_THROW(m_type == kEnumeration || m_type == kListOfEnumeration, "type is not enumeration or list of enumeration"); return *m_Enumeration; };
	id_d_t getTypeId(void) const { return m_type_id; };
	bool symbolEnumerationExists(MQLExecEnv *pEE, bool& bResult);
	bool typeAssignTypeID(MQLExecEnv *pEE);
};

class FeatureEnv {
 private:
	//std::map<std::string, int> m_feature_name2index_map;
	EmdrosStringHashTable<int> m_feature_name2index_map;
	std::vector<MQLType*> m_MQLType_vec;
	std::vector<FeatureInfo> m_FeatureInfo_vec;
 public:
	FeatureEnv();
	~FeatureEnv();

	void clearAndSetSize(int size);
	void addEntry(int feature_index, const FeatureInfo& fi, MQLType *pMQLType);
	int size() const;
	int findFeatureIndex(const std::string& locase_feature_name) const;
	MQLType* findMQLType(int feature_index) const;
	const FeatureInfo& getFeatureInfoConstRef(int feature_index) const;
};


/** Expression type.
 */
typedef enum { kExprInteger, kExprString, kExprIdentifier, 
	       kExprListOfInteger, kExprListOfIdentifier, 
	       kExprEmptyList, kExprSetOfMonads } eExprType;

// Helper class
class Expression {
 private:
	eExprType m_type;
	std::string* m_string;
	emdros_int64 m_integer;
	id_d_t m_enum_id;
	emdros_int64 m_enum_value;
	StringList *m_list_of_identifier;
	IntegerList *m_list_of_integer;
	SetOfMonads *m_pSOM;
	MQLMonadSetElement *m_pMQLMSE;
 public:
	Expression(); // for kExprEmptyList
	Expression(emdros_int64 integer); // For kExprInteger
	Expression(const SetOfMonads& som); // For kExprSetOfMonads
	Expression(MQLMonadSetElement *pSOM); // For kExprSetOfMonads
	Expression(std::string* str, eExprType type); // For kExprString and kExprIdentifier
	Expression(StringList *pStringList, eExprType type);  // for kExprListOfIdentifier 
	Expression(IntegerList *pIntegerList);  // For kExprListOfInteger
	~Expression();
	void weed(MQLExecEnv *pEE, bool& bResult);
	eExprType getKind() { return m_type; };
	emdros_int64 getInteger();
	const SetOfMonads& getSOM() const;
	const std::string& getString();
	const std::string& getIdentifier();
	emdros_int64 getEnumValue();
	bool getAsString(MQLExecEnv *pEE, std::string& result, bool bConvertEnumConstToInteger);
	bool typeTypeCompatibility(MQLExecEnv *pEE, MQLType* type, bool& bResult);
	IntegerList *getCopyOfIntegerList(void) const;
	StringList *getCopyOfIdentifierList(void) const;
	const StringList *getIdentifierList(void) const { return m_list_of_identifier; };
};

// Helper class
class MQLMonadSetElement {
 private:
	monad_m m_mse_first, m_mse_last;
	MQLMonadSetElement* m_next;
	bool m_bIsFirst;
 public:
	MQLMonadSetElement(monad_m mse_first, monad_m mse_last, MQLMonadSetElement* next, bool bIsFirst);
	MQLMonadSetElement(monad_m mse_singleton, MQLMonadSetElement* next, bool bIsFirst);
	virtual ~MQLMonadSetElement();
	monad_m getMseFirst() { return m_mse_first; };
	monad_m getMseLast() { return m_mse_last; };
	MQLMonadSetElement* getNext() { return m_next; };
	void setNext(MQLMonadSetElement* next) { m_next = next; };
	void weed(MQLExecEnv *pEE, bool bWeedPositive, bool& bResult);
	void monadsBuildSet(SetOfMonads& monads);
	void setIsFirst(bool bIsFirst) { m_bIsFirst = bIsFirst; };
 private:
	void weedMonotonic(MQLExecEnv *pEE, bool& bResult);
	void weedPositive(MQLExecEnv *pEE, bool& bResult);
};


// By monads
class ByMonads {
 protected:
	MQLMonadSetElement* m_monad_set;
	SetOfMonads m_monads;
 public:
	ByMonads(MQLMonadSetElement* monad_set);
	virtual ~ByMonads();
	virtual void myWeed(MQLExecEnv *pEE, bool& bResult);
	virtual void myWeedStar(MQLExecEnv *pEE, bool& bResult);
	virtual bool myMonads(MQLExecEnv *pEE, bool& bResult);
	const SetOfMonads& getMonads(void) const { return m_monads; };
 protected:  
	bool getObjectsByMonads(MQLExecEnv *pEE, 
				const std::string& object_type_name, 
				eObjectRangeType objectRangeType,
				std::list<id_d_t>& objects);
};




// Helper class: id_d_list, id_d
class ID_D {
 private:
	ID_D* m_next;
	id_d_t m_id_d;
	id_d_t m_object_type_id;
 public:
	ID_D(id_d_t id_d, ID_D* next);
	virtual ~ID_D();
	id_d_t getID_D() { return m_id_d; };
	id_d_t getObjectTypeId() { return m_object_type_id; };
	ID_D* getNext() { return m_next; };
	void setNext(ID_D* next) { m_next = next; };
	void weedNoneIsNIL(MQLExecEnv *pEE, bool& bResult);
	void execMakeList(std::list<id_d_t>& result);
	void execMakeSOM(SetOfMonads& som);
	bool symbolObjectsExist(MQLExecEnv *pEE, bool& bResult);
	bool symbolObjectsExistInType(MQLExecEnv *pEE, 
				      const std::string& object_type_name, 
				      id_d_t object_type_id, 
				      bool& bResult);
	bool symbolAllAreOfSameType(MQLExecEnv *pEE, id_d_t object_type_id, bool& bResult);
};

/** What kind of block focus? Focus or All? 
 */
typedef enum { kFocus, kAll } eFocusSpec;
/** Which kind of sheaf should we return? (Full? or Flat?).
 */
typedef enum { kFullSheaf, kFlatSheaf } sheaf_return_type_t;

/** A std::pair of sheaf_return_type_t and StringList*.
 */
typedef std::pair<sheaf_return_type_t, StringList *> sheaf_return_type_pair;



// Base class for all query statements
class QueryBase {
 protected:
	eFocusSpec m_focus;
	MQLMonadSetElement* m_in;
	MaxRange *m_max_range;
	sheaf_return_type_pair *m_sheaf_return_type;
	Topograph* m_query;
	SetOfMonads m_Su;
	std::string *m_pArbitrary_monad_set;
	std::list<std::pair<id_d_t, std::string> > m_flat_sheaf_object_types;
 public:
	QueryBase(eFocusSpec focus, 
		  MQLMonadSetElement* in_spec, 
		  MaxRange *max_range,
		  sheaf_return_type_pair *sheaf_return_type,
		  Topograph* query);
	QueryBase(eFocusSpec focus, 
		  std::string *pArbitrary_monad_set,
		  MaxRange *max_range,
		  sheaf_return_type_pair *sheaf_return_type,
		  Topograph* query);
	virtual ~QueryBase();
	virtual void myWeed(MQLExecEnv *pEE, bool& bResult);
	virtual bool mySymbol(MQLExecEnv *pEE, bool& bResult);
	virtual bool myType(MQLExecEnv *pEE, bool& bResult);
	virtual bool myMonads(MQLExecEnv *pEE, bool& bResult);
	virtual bool myExec(MQLExecEnv *pEE, MQLResult **ppMQLResult);
};

// Helper class: feature_assignment
class FeatureAssignment {
 private:
	FeatureAssignment* m_next;
	std::string *m_pFeature_name;
	int m_feature_index;
	Expression* m_expression;
	bool m_is_computed;
 public:
	FeatureAssignment(std::string* feature_name,
			  Expression* expression,
			  FeatureAssignment* next);
	virtual ~FeatureAssignment();
	void weed(MQLExecEnv *pEE, bool& bResult);
	const std::string& getFeatureName() const { return *m_pFeature_name; };
	int getFeatureIndex() const { return m_feature_index; };
	//static int getOrMakeFeatureId(const std::string& feature_name);
	//static void my_str_tolower(const std::string& instring, std::string& outstring, int& feature_id);
	void assignFeatureIndex(const FeatureEnv& feature_env);
	Expression* getExpression() { return m_expression; };
	FeatureAssignment* getNext() { return m_next; };
	void setNext(FeatureAssignment* next) { m_next = next; };
	bool listHasFeature(const std::string& feature_name);
	bool symbolNoFeatureMayBeComputed(MQLExecEnv *pEE, bool& bResult);
	bool execMakeFeatureList(MQLExecEnv *pEE, 
				 const FeatureEnv& feature_env,
				 std::list<FeatureInfo>& features);
	void execMakeFeatureArray(MQLExecEnv *pEE, 
				  const FeatureEnv& feature_env, 
				  EMdFValue *feature_value_arr);
	bool typeTypeCompatibility(MQLExecEnv *pEE, 
				   const FeatureEnv& feature_env, 
				   bool& bResult);
	void setIsComputed(bool is_computed) { m_is_computed = is_computed; };
	bool getIsComputed(void) const { return m_is_computed; };
 protected:
	void weedSelfNotAssigned(MQLExecEnv *pEE, bool& bResult);
};

class GrammarFeature {
 protected:
	GrammarFeature *m_next;
	std::string* m_feature;
 public:
	GrammarFeature(std::string* feature, GrammarFeature* next) {
		m_feature = feature;
		m_next = next;
	};
	~GrammarFeature() { delete m_feature; delete m_next; };
	std::string getFeature(void) const { if (m_feature != 0) return *m_feature; else return "";};
	GrammarFeature *getNext() { return m_next; };
};

// Helper class
class Feature {
 private:
	Feature *m_next;
	std::string* m_feature;
	id_d_t m_feature_type_id;
	bool m_is_computed;
	TableColumnType m_tc_type;
	std::string m_enum_name;
	int m_list_index;
	short int m_feature_index_inst;
	std::map<emdros_int64, std::string> m_enum_const_cache;
	int m_length;
 public:
	Feature(std::string* feature, Feature* next);
	static Feature *FromGrammarFeature(GrammarFeature *pGrammarFeature);
	Feature(const Feature& other);
	virtual ~Feature();
	const std::string& getFeature() { return *m_feature; };
	id_d_t getFeatureTypeID(void) const { return m_feature_type_id; };
	const std::string& getEnumName(void) const { return m_enum_name; };
	const std::string& getEnumConstNameFromValue(emdros_int64 value) { return m_enum_const_cache[value]; };
	Feature* getNext() { return m_next; };
	void setNext(Feature* next) { m_next = next; };
	int getLength(void) { return m_length = getLength(1); };
	bool symbolFeaturesExist(MQLExecEnv *pEE, id_d_t object_type_id, bool& bResult);
	bool typeFeatureName(MQLExecEnv *pEE, bool& bResult);
	void execMakeFeatureList(std::list<FeatureInfo>& FeatureInfos);
	void execMakeFeatureNameVector(std::vector<std::string>& FeatureNames);
	void execMakeNameList(MQLResult *pResult);
	void symbolAddToObject(MQLExecEnv *pEE, MQLObject *pObj);
#ifndef SWIG
	void symbolAddFeaturesToSet(std::set<std::string>& myset) const;
#endif
	void addEMdFValue(MatchedObject *pMO, MQLObject *pObj, bool bIterate);
	void makeVectorOfSelves(std::vector<Feature*> **ppVec);
	void assignListIndex(int current_index);
	int getFeatureInstListIndex(const std::string& feature_name) const;
 private:
	int getLength(int current_length) const { 
		if (m_next != 0) { 
			return m_next->getLength(current_length+1); 
		} else { 
			return current_length; 
		} 
	};
};

class AggregateFeature {
 protected:	
	eAggregateFunction m_function;
	emdros_int64 m_result;
	Feature *m_feature;
	FeatureComparison *m_feature_comparison;

	std::string m_object_type_name;
	id_d_t m_object_type_id;
	AggregateFeature *m_next;
	int m_inst_obj_feature_index;
 public:
	AggregateFeature(eAggregateFunction func);
	AggregateFeature(eAggregateFunction func, std::string *feature_name);
	AggregateFeature(eAggregateFunction func, FeatureComparison *feature_comparison);
	virtual ~AggregateFeature();

	virtual AggregateFeature* getNext() { return m_next; };
	virtual void setNext(AggregateFeature* next) { m_next = next; };

	virtual void setInstObjectFeatureIndex(int inst_obj_feature_index) { m_inst_obj_feature_index = inst_obj_feature_index; };
	
	virtual bool hasFeature() const { return m_feature != 0; };
	virtual bool hasFeatureComparison() const { return m_feature_comparison != 0; };
	virtual std::string getFeatureName() const;
	virtual FeatureComparison *getFeatureComparison() const { return m_feature_comparison; };
	
	virtual emdros_int64 getResult() const { return m_result; };
	virtual void weed(MQLExecEnv *pEE, bool& bResult);
	virtual bool symbol(MQLExecEnv *pEE, const std::string& object_type_name, id_d_t object_type_id, bool& bResult);
	virtual bool type(MQLExecEnv *pEE, bool& bResult);
	virtual void exec(MQLExecEnv *pEE, const InstObject *pInstObj);
};



#endif // !defined SWIG
#endif /* MQL_TYPES__H__ */
