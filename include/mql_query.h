/*
 * mql_query.h
 *
 * Definitions of AST-classes for MQL Query
 *
 * Ulrik Petersen
 * Created: 2/27-2001
 * Last update: 3/2-2019
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


/**@file mql_query.h
 *@brief Header file for classes related to topographic queries (MQL layer)
 */

/**
 *\defgroup Topograph Topographic queries
 *\ingroup MQL
 */

#ifndef MQL_QUERY_TYPES__H__
#define MQL_QUERY_TYPES__H__

#include <map>
#include <string>
#include <list>
#include <set>
#include <vector>
#include "string_list.h"
#include "debug.h"
// emdf.h also includes emdros-lconfig.h
#include "emdf.h"
// So that this will be defined
#include "pcre_emdros.h"
#include "infos.h"
#include "emdf_enums.h"
#include "mql_enums.h"
#include "emdf_exception.h"
#include "mql_types.h"




class Straw; // Forward declaration
class ObjectBlock; // Forward declaration
class SetOfMonads; // Forward declaration
class EMdFComparison; // Forward declaration
class EMdFFFactor; // Forward declaration
class EMdFFTerm; // Forward declaration
class EMdFFFeatures; // Forward declaration
class MQLObject; // Forward declaration
class MQLExecEnv; // Forward declaration
class Blocks; // Forward declaration
class EMdFDB; // Forward declaration
class Feature; // Forward declaration
class EnumConstCache; // Forward declaration
class EMdFValue; // Forward declaration
class ObjectBlockBase; // Forward declaration
class ObjectBlockString; // Forward declaration
class UnorderedGroup; // Forward declaration
class BlockString0; // Forward declaration
class BlockString1; // Forward declaration
class BlockString2; // Forward declaration
class BlockString; // Forward declaration
class Power; // Forward declaration
class Inst; // Forward declaration

/** 
 *
 */
class MonadSetRelationClause {
	eMonadSetRelationOperation m_operation;
	eUniverseOrSubstrate m_universe_or_substrate;
	std::string m_our_monad_set;
	id_d_t m_feature_type_id;
 public:
	MonadSetRelationClause(eMonadSetRelationOperation operation,
			       std::string* ourMonadSet,
			       eUniverseOrSubstrate universeOrSubstrate);
	MonadSetRelationClause(const MonadSetRelationClause& other);
	~MonadSetRelationClause();
	eMonadSetRelationOperation getOperation() const { return m_operation; };
	eUniverseOrSubstrate getUniverseOrSubstrate() const { return m_universe_or_substrate; };
	void setOperation(eMonadSetRelationOperation operation) { m_operation = operation; };
	std::string getOurMonadSet() const { return m_our_monad_set; };
	void weed(MQLExecEnv *pEE, bool& bResult);
	bool symbol(MQLExecEnv *pEE, id_d_t object_type_id, bool& bResult);
	bool type(MQLExecEnv *pEE, bool& bResult);
	bool exec(const SetOfMonads& object_som,
		       const SetOfMonads& universe, 
		       const SetOfMonads& substrate) const;
	std::string calculateCharacteristicString() const;
};


/** A std::multimap mapping std::string to ObjectBlock*.
 *
 * This is used for characteristic strings.
 */
typedef std::multimap<std::string, ObjectBlock*> String2COBPtrMMap;
/** A std::vector of ObjectBlockBase.
 */
typedef std::vector<ObjectBlockBase*> OBBVec;

typedef std::list<BlockString2*> BlockString2List;

/** Class to hold gaps and an iterator over them.
 *@ingroup Topograph
 *@internal
 */
class Gaps {
 private:
	SetOfMonads m_gaps_som; /**< The SetOfMonads that has the gaps. */
	SOMConstIterator m_ci; /**< The iterator over the set of gaps. */
 public:
	/** Constructor.
	 *@internal
	 *
	 * Takes the gaps from the Su parameter.
	 *
	 * @param Su The substrate from which to construct m_gaps_som.
	 */
	Gaps(const SetOfMonads& Su) {
		// Gets a SetOfMonads which is the complement of Su, 
		// but with 0..Su.first()-1 cut away, and with
		// Su.last()+1..MAX_MONAD cut away.
		// Note that this may be empty if either Su is empty,
		// or if Su is a single stretch.
		m_gaps_som = Su.getGaps();
		m_ci = m_gaps_som.const_iterator();
	};
	/** Proxy for m_ci.hasNext(). 
	 *@internal
	 * @return true iff there is at least one more gap.
	 */
	bool hasNext(void) const { return m_ci.hasNext(); };
	/** Proxy for m_ci.hasNext(). 
	 *@internal
	 *
	 * @return the current MonadSetElement (i.e., gap).
	 */
	const MonadSetElement& next(void)  { return m_ci.next(); };
	/** Proxy for m_ci.hasNext(). 
	 *@internal
	 *
	 * @return the current MonadSetElement (i.e., gap).
	 */
	const MonadSetElement& current(void) { return m_ci.current(); };
	/** Test whether there are no gaps.
	 *@internal
	 *
	 * @return true iff m_gaps_som is empty, i.e., the Su
	 * substrate passed to the constructor did not have any gaps.
	 */
	bool isEmpty(void) const { return m_gaps_som.isEmpty(); };
};

class StartMonadIterator {
 protected:
	SetOfMonads m_Su;
 public:
        StartMonadIterator(SetOfMonads Su) : m_Su(Su) {};
	virtual ~StartMonadIterator() {};
	virtual bool hasNext() { return false; };
	virtual monad_m next() { return 0; };
	virtual SetOfMonads getSu() { return m_Su; }
	virtual SetOfMonads getU() { 
		if (m_Su.isEmpty()) { 
			return m_Su; 
		} else { 
			return SetOfMonads(m_Su.first(), m_Su.last()); 
		} 
	}
	virtual std::string getSMIKind() { return "StartMonadIteratorBase"; }
 protected:
	// Disallow assignment, copy construction, and default construction
	StartMonadIterator() {};
	StartMonadIterator& operator=(const StartMonadIterator& other) { UNUSED(other); return *this;};
	StartMonadIterator(const StartMonadIterator& other) { UNUSED(other); };
};

class UsingRange {
 protected:
	emdf_ivalue m_start;
	emdf_ivalue m_end;
 public:
	UsingRange(emdf_ivalue start, emdf_ivalue end);
	virtual ~UsingRange() {};
	void weed(MQLExecEnv *pEE, bool& bResult);
	emdf_ivalue getStart() { return m_start; }
	emdf_ivalue getEnd()  { return m_end; }
 protected:
	// Disallow assignment, copy construction, and default construction
	UsingRange() {};
	UsingRange& operator=(const UsingRange& other) { UNUSED(other); return *this;};
	UsingRange(const UsingRange& other) { UNUSED(other); };
};

/** Node number
 * 
 * An index into MQLExecEnv's vector of QueryNode objects.
 */
typedef int node_number_t;


typedef enum {
	kQNKBlockString2,
	kQNKObjectBlock,
	kQNKFFeatures
} eQueryNodeKind;

/** The superclass of some Topograph AST nodes.
 *@ingroup Topograph
 *@internal
 *
 * The following kinds of nodes derive from this:
 *
 * - BlockString2
 * - ObjectBlock
 * - FFeatures
 *
 * This is used in MQLExecEnv::m_node_vector, and is mainly motivated
 * by the object reference machinery.
 *
 */
class QueryNode {
 protected:
	node_number_t m_node_number;
	eQueryNodeKind m_kind;
 public:
	QueryNode(eQueryNodeKind kind) { m_node_number = 0; m_kind = kind; };
	QueryNode(const QueryNode& other) { m_node_number = other.m_node_number; m_kind = other.m_kind; };
	~QueryNode() {};
	eQueryNodeKind getKind(void) const { return m_kind; };
	node_number_t getNodeNumber(void) const { return m_node_number; };
	void setNodeNumber(node_number_t node_number) { m_node_number = node_number; };
	QueryNode& operator=(const QueryNode& other) { m_node_number = other.m_node_number; m_kind = other.m_kind; return *this; };
};

#ifndef SWIG

typedef std::vector<QueryNode*> QueryNodeVector;
typedef std::vector<node_number_t> NodeNumberVector;
typedef std::list<node_number_t> NodeNumberList;
typedef std::set<node_number_t> NodeNumberSet;

// A NonParentORDSolution is a vector of const MatchedObject*
// which is used when resolving non-parent Object Reference Usages.
// The vector doesn't own the MatchedObjects*'s.
class NonParentORDSolution {
 private:
	typedef std::vector<const MatchedObject*> MOVec;
	MOVec m_vec;
 public:
	NonParentORDSolution();
	NonParentORDSolution(NonParentORDSolution& other);
	~NonParentORDSolution();
	void extendWithMe(NonParentORDSolution *pOther);
	const MatchedObject* operator[](int index);
	bool empty(void) const { return m_vec.empty(); };
	void push_back(const MatchedObject* pMO) { m_vec.push_back(pMO); };
};

typedef Llist<NonParentORDSolution> NonParentORDSolutionList;
typedef LlistIterator<NonParentORDSolution> NonParentORDSolutionListIterator;
#endif



/** data from the "noretrieve"/"retrieve"/"focus" trichotomy. 
 *@ingroup Topograph
 *@internal
 */
typedef enum { kNoRetrieve, /**< neither retrieve nor focus. */
	       kRetrieve,   /**< retrieve. */
	       kRetrieveFocus /**< focus. */
} eRetrieval; 

/** An AST member for a [gap] or [gap?] block.
 *@ingroup Topograph
 *@internal
 */
class GapBlock {
 protected:
	eRetrieval m_retrieval;
	Blocks* m_opt_blocks;
	ObjectBlockBase *m_pOBB;
	std::string *m_marks;
 public:
	GapBlock(eRetrieval retrieval, std::string* mark_declaration, Blocks *opt_blocks);
	virtual ~GapBlock();
	eRetrieval getRetrieval() { return m_retrieval; };
	void weed(MQLExecEnv *pEE, bool& bResult, bool is_first, bool is_last, node_number_t block_string2_parent);
	bool symbol(MQLExecEnv *pEE, bool bDoCalculatePreQueryString, bool& bResult);
	bool symbol(MQLExecEnv *pEE, bool& bResult);
	bool symbolObjectReferences(MQLExecEnv *pEE, bool& bResult, std::set<std::string>& ORD_set);
	bool symbolObjectReferences2(MQLExecEnv *pEE);
	bool type(MQLExecEnv *pEE, eObjectRangeType contextRangeType, bool& bResult);
	Blocks* getOptBlocks() { return m_opt_blocks; };
	void calculateMMapOutermost(String2COBPtrMMap& mmap, const std::string& prefix, EMdFDB *pDB);
	void calculateMMapInnermost(String2COBPtrMMap& mmap, std::string& inner_charstring, EMdFDB *pDB);
	bool aggregateQuery(MQLExecEnv *pEE, FastSetOfMonads& characteristic_set, const SetOfMonads& Su, eAggregateQueryStrategy strategy, monad_m largest_object_length_above, String2COBPtrMMap& mmap);
	void canChooseAQStrategyInnermostFirst(bool &bResult);
	bool isFocus(void) { return m_retrieval == kRetrieveFocus; };
	void addOBBToVec(OBBVec *pOBBVec);
	ObjectBlockBase *getOBB(void) { return m_pOBB; };
};


/** An AST member for a [gap?] block.
 *@ingroup Topograph
 *@internal
 */
class OptGapBlock : public GapBlock {
public:
	OptGapBlock(eRetrieval retrieval, std::string *mark_declaration, Blocks *opt_blocks) : GapBlock(retrieval, mark_declaration, opt_blocks) {};
	virtual ~OptGapBlock() {};
};

/** Kind of block.
 *@ingroup Topograph
 *@internal
 */
typedef enum { kOptGapBlock,           /**< [gap?] block. */
	       kGapBlock,              /**< [gap] block. */
	       kObjectBlock,           /**< [object] block. */
	       kObjectBlockNOTEXIST,   /**< NOTEXIST [object] block. */
	       kPowerBlock             /**< .. power block. */
} eBlockKind;




/** An object reference usage
 *@ingroup Topograph
 *@internal
 */
class ObjectReferenceUsage {
 private:
	std::string* m_object_reference;
	std::string* m_feature_name;
	std::string* m_parameter1;
	std::string m_object_reference_lower;
	FeatureInfo m_feature_info;
	node_number_t m_controlling_object_block_node_number;
	ObjectBlock *m_controlling_object_block_object;
	node_number_t m_ffeatures_parent;
	node_number_t m_block_string2_parent;
	unsigned int m_index_of_feature_in_MQLObject;
	unsigned int m_index_of_feature_in_MatchedObject;
	unsigned int m_matched_object_index; // In NonParentORDSolution
	bool m_bORDIsParent;
 public:
	ObjectReferenceUsage(std::string* object_reference, std::string* feature_name, std::string *parameter1);
	virtual ~ObjectReferenceUsage();
	bool getORDIsParent(void) const { return m_bORDIsParent; };
	id_d_t getOutputFeatureTypeId(void) const { return m_feature_info.getOutputType(); };
	void weed(MQLExecEnv *pEE, bool& bResult);
	bool symbol(MQLExecEnv *pEE, node_number_t ffeatures_parent, bool& bResult);
	bool symbolObjectReferences2(MQLExecEnv *pEE);
	
	bool type(MQLExecEnv *pEE, bool& bResult);
	
	const std::string& getObjectReference() { return m_object_reference_lower; };
	const std::string& getFeatureName() { return *m_feature_name; };
	const FeatureInfo &getFeatureInfo() const { return m_feature_info; };
	void determineSolutionType(MQLExecEnv *pEE);
	node_number_t getControllingObjectBlockNodeNumber(void) const { return m_controlling_object_block_node_number; };
	void setControllingObjectBlockNodeNumber(MQLExecEnv *pEE, node_number_t OBB_node_number);
	ObjectBlock* getControllingObjectBlock(void) { return m_controlling_object_block_object; };
	const ObjectBlock* getControllingObjectBlock(void) const { return m_controlling_object_block_object; };
	unsigned int getMatchedObjectIndex(void) const { return m_matched_object_index; };
	void setMatchedObjectIndex(unsigned int matched_object_index) { m_matched_object_index = matched_object_index; };
	unsigned int getFeatureIndexInMQLObject(void) const { return m_index_of_feature_in_MQLObject; };
	unsigned int getFeatureIndexInMatchedObject(void) const { return m_index_of_feature_in_MatchedObject; };
	void setBlockString2Parent(node_number_t block_string2_parent) { m_block_string2_parent = block_string2_parent; };
};

/** A kind of value.
 *@ingroup Topograph
 *@internal
 */
typedef enum { 
	kValEnumConst, 
	kValInteger, 
	kValString, 
	kValObjectReferenceUsage 
} eValueKind;

/** AST member for a value.
 *@ingroup Topograph
 *@internal
 */
class Value {
 private:
	EMdFValue *m_emdf_value;
	ObjectReferenceUsage* m_object_reference_usage;
	std::string* m_enum_const;
	emdf_ivalue m_integer;
	std::string* m_string;
	eValueKind m_kind;
	id_d_t m_enum_id;
	emdf_ivalue m_enum_const_value;
 public:
	Value(std::string* str, eValueKind kind); // for enum_const and string
	Value(emdf_ivalue integer);
	Value(ObjectReferenceUsage* object_reference_usage);
	virtual ~Value();
	bool symbol(MQLExecEnv *pEE, id_d_t feature_type_id, node_number_t ffeatures_parent, bool& bResult);
	bool type(MQLExecEnv *pEE, bool& bResult);
	eValueKind getKind() { return m_kind; };
	const std::string& getEnumConst();
	std::string getAsString(MQLExecEnv *pEE, NonParentORDSolution *pNonParentORDSolution) const;
	EMdFValue *getAsNewedEMdFValue() const;
	emdf_ivalue getInteger();
	const std::string& getString();
	const EMdFValue* getEMdFValue(void);
	ObjectReferenceUsage* getObjectReferenceUsage();
	const ObjectReferenceUsage* getObjectReferenceUsageConst() const;
	/** Getter for the m_enum_id.
	 *
	 * @return the value if m_enum_id. 
	 */
	id_d_t getEnumId(void) const { return m_enum_id; };
};

/** AST member for a feature_comparison.
 *@ingroup Topograph
 *@internal
 */
class FeatureComparison {
 private:
	std::string* m_feature_name;
	std::string* m_parameter1;
	std::string m_object_type_name;
	id_d_t m_object_type_id;
	FeatureInfo m_feature_info;
	eComparisonOp m_comparison_op;
	Value* m_value;
	StringList *m_in_enum_list;
	IntegerList *m_in_integer_list;
	std::string m_in_integer_list_as_string;
	EnumConstCache *m_in_enum_const_cache;
	std::list<EnumConstInfo> m_in_enum_const_info_list;
	pcre *m_pcre;
	pcre_extra *m_pcre_extra;
	int *m_ovector;
	int m_ovectorsize;
	int m_feature_index;
	bool m_bCanBePreQueried;
	bool m_bContextHasBeenNegative;
	node_number_t m_ffeatures_parent;
 public:
	// For feature_name comparison_op (enum_const1, enum_const2)
	FeatureComparison(std::string* feature_name,
			  std::string* parameter1,
			  eComparisonOp m_comparison_op,
			  StringList *in_enum_list); 

	// For feature_name comparison_op (integer, integer, ..., integer)
	FeatureComparison(std::string* feature_name,
			  std::string* parameter1,
			  eComparisonOp m_comparison_op,
			  IntegerList *in_enum_list); 

	// For all others
	FeatureComparison(std::string* feature_name,
			  std::string* parameter1,
			  eComparisonOp comparison_op,
			  Value* value);

	virtual ~FeatureComparison();
	bool getCanBePreQueried() const { return m_bCanBePreQueried; }
	void weedFeatureConstraints(MQLExecEnv *pEE, bool& bResult, node_number_t node_number);
	bool symbol(MQLExecEnv *pEE, 
		    const std::string& object_type_name,
		    id_d_t enclosing_object_type_id, 
		    bool& bResult);
	bool symbolObjectReferences(MQLExecEnv *pEE, 
				    bool& bResult, 
				    std::set<std::string>& ORD_set);
	bool symbolObjectReferences2(MQLExecEnv *pEE);
	void symbolAddFeatures(MQLObject* pObj, EMdFDB *pDB);
#ifndef SWIG
	void symbolAddFeaturesToSet(std::set<std::string>& myset) const;
	bool symbolSetFeatureIndex(const std::vector<std::string>& feature_name_vec, bool& bResult);
#endif
	bool type(MQLExecEnv *pEE, bool& bResult);
	const std::string& getFeatureName() { return *m_feature_name; };
	eComparisonOp getComparisonOp() { return m_comparison_op; };
	Value* getValue() { return m_value; };
	void pretty(std::ostream *pOut, int indent, NonParentORDSolution *pNonParentORDSolution);
	bool compare(MQLExecEnv *pEE, const EMdFValue *left_value, NonParentORDSolution *pNonParentORDSolution);
	bool exec(MQLExecEnv *pEE, const InstObject *pInstObj);
	
	EMdFComparison *makeConstraints(EMdFDB *pDB, bool bContextHasBeenNegative);
	/** Get m_feature_index
	 *
	 * @return The feature index.
	 */
	short int getFeatureIndex() const { 
		return m_feature_index;
	};
	/** Set m_bCanBePreQueried to false.
	 */
	void resetCanBePreQueried(void) { m_bCanBePreQueried = false; };
	bool hasORU();
 private:
	bool compareObjectReferenceUsage(const EMdFValue* left_value, NonParentORDSolution *pNonParentORDSolution);
};


class FFeatures; // Forward declaration

class FFactor {
 private:
	FFactor* m_ffactor;
	FFeatures* m_ffeatures;
	FeatureComparison* m_feature_comparison;
 public:
	FFactor(FFactor* ffactor);
	FFactor(FFeatures* ffeatures);
	FFactor(FeatureComparison* feature_comparison);
	virtual ~FFactor();
	void weedFeatureConstraints(MQLExecEnv *pEE, bool& bResult, node_number_t node_number);
	bool symbol(MQLExecEnv *pEE, 
		    const std::string& object_type_name,
		    id_d_t enclosing_object_type_id, 
		    bool& bResult);
	void symbolAddFeatures(MQLObject* pObj, EMdFDB *pDB);
#ifndef SWIG
	void symbolAddFeaturesToSet(std::set<std::string>& myset) const;
	bool symbolSetFeatureIndex(const std::vector<std::string>& feature_name_vec, bool& bResult);	
#endif
	bool symbolObjectReferences(MQLExecEnv *pEE, bool& bResult, std::set<std::string>& ORD_set);
	bool symbolObjectReferences2(MQLExecEnv *pEE);
	bool type(MQLExecEnv *pEE, bool& bResult);
	bool exec(MQLExecEnv *pEE, const InstObject *pInstObj);
	void pretty(std::ostream *pOut, int indent, NonParentORDSolution *pNonParentORDSolution);
	bool isNOT() const { return m_ffactor != 0; };
	bool isParenthesis() const { return m_ffeatures != 0; };
	bool isFeatureComparison() const { return m_feature_comparison != 0; };
	FFactor* getFFactor() { return m_ffactor; };
	FFeatures* getFFeatures() { return m_ffeatures; };
	FeatureComparison* getFeatureComparison() { return m_feature_comparison; };
	EMdFFFactor *makeConstraints(EMdFDB *pDB, bool bContextIsPositive, bool bContextHasBeenNegative) const;
	bool hasORU() const;
	void resetCanBePreQueried(void);
};

class FTerm {
 private:
	FFactor* m_ffactor;
	FTerm* m_fterm;
 public:
	FTerm(FFactor* ffactor);
	FTerm(FTerm* fterm, FFactor* ffactor);
	virtual ~FTerm();
	void weedFeatureConstraints(MQLExecEnv *pEE, bool& bResult, node_number_t node_number);
	bool symbol(MQLExecEnv *pEE, 
		    const std::string& object_type_name,
		    id_d_t enclosing_object_type_id, 
		    bool& bResult);
	void symbolAddFeatures(MQLObject* pObj, EMdFDB *pDB);
#ifndef SWIG
	void symbolAddFeaturesToSet(std::set<std::string>& myset) const;
	bool symbolSetFeatureIndex(const std::vector<std::string>& feature_name_vec, bool& bResult);	
#endif
	bool symbolObjectReferences(MQLExecEnv *pEE, bool& bResult, std::set<std::string>& ORD_set);
	bool symbolObjectReferences2(MQLExecEnv *pEE);
	bool type(MQLExecEnv *pEE, bool& bResult);
	bool exec(MQLExecEnv *pEE, const InstObject *pInstObj);
	void pretty(std::ostream *pOut, int indent, NonParentORDSolution *pNonParentORDSolution);
	bool isFFactor() const { return m_fterm == 0; };
	FFactor* getFFactor() { return m_ffactor; };
	FTerm* getFTerm() { return m_fterm; };
	EMdFFTerm *makeConstraints(EMdFDB *pDB, bool bContextIsPositive, bool bContextHasBeenNegative) const;
	void resetCanBePreQueried(void);
	bool hasORU() const;
};

class FFeatures : public QueryNode {
 private:
	FTerm* m_fterm;
	FFeatures* m_ffeatures;
	EMdFFFeatures *m_constraints;
	node_number_t m_block_string2_parent;
	typedef std::vector<ObjectReferenceUsage*> PORUVector;
	PORUVector m_NonParentORU_vector;
	NodeNumberSet m_OBBNodeNumbersToUseForNonParentORDs;
	BlockString2List m_block_string2_list;
	NodeNumberVector m_ORDOBBNodeNumber_vector;
	typedef std::vector<bool> BoolVector;
	BoolVector m_NonParentORDOBBMustBeRetrieved;
 public:
	FFeatures(FTerm* fterm);
	FFeatures(FFeatures* ffeatures, FTerm* fterm);
	virtual ~FFeatures();
	void weedFeatureConstraints(MQLExecEnv *pEE, bool& bResult, node_number_t node_number);
	bool hasNonParentORUs(void) const { return !m_NonParentORU_vector.empty(); };
	bool symbol(MQLExecEnv *pEE, 
		    const std::string& object_type_name,
		    id_d_t enclosing_object_type_id, 
		    bool& bResult);
	bool symbolObjectReferences(MQLExecEnv *pEE, bool& bResult, std::set<std::string>& ORD_set);
	bool symbolObjectReferences2(MQLExecEnv *pEE);
	void symbolAddFeatures(MQLObject* pObj, EMdFDB *pDB);
#ifndef SWIG
	void symbolAddFeaturesToSet(std::set<std::string>& myset) const;
	bool symbolSetFeatureIndex(const std::vector<std::string>& feature_name_vec, bool& bResult);	
#endif
	bool type(MQLExecEnv *pEE, bool& bResult);
	bool exec(MQLExecEnv *pEE, const InstObject *pInstObj);
	void pretty(std::ostream *pOut, int indent, NonParentORDSolution *pNonParentORDSolution);
	bool isFTerm() const { return m_ffeatures == 0; };
	FTerm* getFTerm() { return m_fterm; };
	FFeatures* getFFeatures() { return m_ffeatures; };
	const std::string& getConstraints(EMdFDB *pDB);
	EMdFFFeatures *getEMdFConstraints(EMdFDB *pDB);
	EMdFFFeatures *makeConstraints(EMdFDB *pDB, bool bContextIsPositive, bool bContextHasBeenNegative) const;
	unsigned int getMatchedObjectIndex(MQLExecEnv *pEE,
					   node_number_t ORDObjectBlockNodeNumber);

	void resetCanBePreQueried(void);
	void addNonParentORU(ObjectReferenceUsage *pORU);
	void setBlockString2Parent(node_number_t block_string2_parent) { m_block_string2_parent = block_string2_parent; };
	node_number_t getBlockString2Parent(void) const { return m_block_string2_parent; };
	const BlockString2List& getBlockString2List(void) const { return m_block_string2_list; };

	bool mustGetNonParentORDSolutionForOBB(node_number_t OBBNN) {
		return m_NonParentORDOBBMustBeRetrieved[OBBNN];
	};
	bool hasORU() const;
};

/** An enumeration covering the possibilities of FIRST, LAST, FIRST
 * AND LAST, and none of these.
 */
typedef enum { kFirst, kLast, kFirstAndLast, kNoFirstLast } eFirstLast;
class Blocks; // Forward declaration

class ObjectBlockBase : public QueryNode {
 protected:
	eRetrieval m_retrieval;
	std::string* m_object_type_name;

	unsigned int m_nNoOfGET_features;

	// Need not have the same size as nNoOfGET_features;
	// We may need to get more features into the MatchedObject
	// than we have for the GET list of features in the query.
	// If so, this vector is longer than the m_pFeature_retrieval
        // list.
	std::vector<Feature*> m_Feature_retrieval_vec;
	id_d_t m_object_type_id;
	eObjectRangeType m_objectRangeType;
	eMonadUniquenessType m_monadUniquenessType;
	bool m_bIsNOTEXIST;
	std::string *m_mark;
	node_number_t m_block_string2_parent;
	monad_m m_largest_object_length;
 public:
	ObjectBlockBase(std::string* object_type_name,
			std::string* mark_declaration,
			eRetrieval retrieval,
			GrammarFeature* GET_feature_retrieval,
			bool bIsNOTEXIST);
	ObjectBlockBase(std::string* object_type_name,
			std::string* mark_declaration,
			eRetrieval retrieval,
			Feature* GET_feature_retrieval,
			bool bIsNOTEXIST);
	ObjectBlockBase(const ObjectBlockBase& other);
	virtual ~ObjectBlockBase();
	const std::string& getObjectTypeName() const { return *m_object_type_name; };
	monad_m getLargestObjectLength() const { return m_largest_object_length; };
	int getNoOfGETFeatures() { return m_nNoOfGET_features; };
	int getNoOfFeatureRetrievalFeatures(void) const { return (int) m_Feature_retrieval_vec.size(); };
	Feature* getFeatureRetrievalFeature(unsigned int index);
	bool isNOTEXIST(void) const { return m_bIsNOTEXIST; }
	eRetrieval getRetrieval() const { return m_retrieval; };
	bool isFocus(void) const { return m_retrieval == kRetrieveFocus; };
	bool isToBeRetrieved(void) const { return m_retrieval != kNoRetrieve; };
	id_d_t getObjectTypeId() const { return m_object_type_id; };
	std::string getMarkString(void) const { return *m_mark; };
	void weed(MQLExecEnv *pEE, bool& bResult);
	bool symbol(MQLExecEnv *pEE, bool& bResult);
	bool type(MQLExecEnv *pEE, bool& bResult);
 protected:
	bool symbolObjectTypeExists(MQLExecEnv *pEE, bool& bObjectTypeExists);
 private:
	void copyFeatureRetrievalToVector(Feature *pFeature);
};

class ObjectBlock : public ObjectBlockBase {
 protected:
	ObjectBlockBase *m_pOBB;
	std::string* m_object_reference;
	eFirstLast m_first_last;
	FFeatures* m_feature_constraints;
	Blocks* m_opt_blocks;
	std::string m_pre_query_string;
	std::string m_characteristic_string;
	MQLObject* m_object;
	bool m_bDoCalculatePreQueryString;
	bool m_bIsFirstInBlockString;
	bool m_bIsLastInBlockString;
	bool m_bIsAfterNonNOTEXISTInBlockString;
	std::string m_object_reference_lower;
	MonadSetRelationClause *m_pMSRC;
 public:
	Inst *m_inst;
	ObjectBlock(std::string* object_type_name,
		    std::string* mark_declaration,
		    std::string* object_reference,
		    eRetrieval retrieval,
		    eFirstLast first_last,
		    MonadSetRelationClause *pMSRC,
		    FFeatures* feature_constraints,
		    GrammarFeature* feature_retrieval,
		    Blocks* opt_blocks,
		    bool bIsNOTEXIST);
	ObjectBlock(std::string* object_type_name,
		    std::string* mark_declaration,
		    std::string* object_reference,
		    eRetrieval retrieval,
		    eFirstLast first_last,
		    MonadSetRelationClause *pMSRC,
		    FFeatures* feature_constraints,
		    Feature *GET_feature_retrieval,
		    Blocks* opt_blocks,
		    bool bIsNOTEXIST);
	virtual ~ObjectBlock();
	void weedFeatureConstraints(MQLExecEnv *pEE, bool& bResult);
	void weed(MQLExecEnv *pEE, bool& bResult, bool is_first, bool is_last, node_number_t block_string2_parent, bool& bHasMetNonNOTEXISTInBlockString);
	bool symbol(MQLExecEnv *pEE, bool bDoCalculatePreQueryString, bool& bResult);
	bool symbolObjectReferences(MQLExecEnv *pEE, bool& bResult, std::set<std::string>& ORD_set);
	bool symbolObjectReferences2(MQLExecEnv *pEE);
	void addFeatureToBeRetrieved(MQLExecEnv *pEE, const FeatureInfo& feature_info, unsigned int& index_assigned);
	bool type(MQLExecEnv *pEE, eObjectRangeType contextRangeType, bool& bResult);
	bool isFirstInBlockString(void) const { return m_bIsFirstInBlockString; };
	bool isLastInBlockString(void) const { return m_bIsLastInBlockString; };
	bool isSoleBlockInBlockString(void) const { return m_bIsFirstInBlockString && m_bIsLastInBlockString; };
	bool isAfterNonNOTEXISTInBlockString() const { return m_bIsAfterNonNOTEXISTInBlockString; };
	void setIsAfterNonNOTEXISTInBlockString(bool bValue) { m_bIsAfterNonNOTEXISTInBlockString = bValue; };
	eObjectRangeType getObjectRangeType() const { return m_objectRangeType; };
	const MonadSetRelationClause *getMSRC() const { return m_pMSRC; }
	const std::string& getObjectReference() const { ASSERT_THROW(m_object_reference != 0, "object reference is 0"); return m_object_reference_lower; };
	eFirstLast getFirstLast() const { return m_first_last; };
	ObjectBlockBase *getOBB(void) { return m_pOBB; };
	const std::string& getPreQueryString(void) const { return m_pre_query_string; };
	EMdFFFeatures *getEMdFConstraints(EMdFDB *pDB);

	const std::string& getCharacteristicString(void) const { return m_characteristic_string; }
	bool calculatePreQueryString(EMdFDB *pDB);
	void calculateCharacteristicStringOutermost(const std::string& prefix);
	void calculateCharacteristicStringInnermost(const std::string& prefix);
	FFeatures* getFeatureConstraints() { return m_feature_constraints; };
	Blocks* getOptBlocks() { return m_opt_blocks; };
	MQLObject* getObject() { return m_object; };
	const MQLObject* getObject() const { return m_object; };
	bool makeInst(MQLExecEnv *pEE, const SetOfMonads& Su, eMonadSetRelationOperation *pMonadSetOperation, String2COBPtrMMap& mmap);
	bool aggregateQuery(MQLExecEnv *pEE, FastSetOfMonads& characteristic_set, const SetOfMonads& Su, eAggregateQueryStrategy strategy, monad_m largest_object_length_above, String2COBPtrMMap& mmap);
	void canChooseAQStrategyInnermostFirst(bool &bResult);
	bool hasFirstLast() const;
	void calculateMMapOutermost(String2COBPtrMMap& mmap, const std::string& prefix, EMdFDB *pDB);
	void calculateMMapInnermost(String2COBPtrMMap& mmap, std::string& inner_charstring, EMdFDB *pDB);
	StartMonadIterator* getSMI(MQLExecEnv *pEE, const SetOfMonads& U, const SetOfMonads& Su, monad_m Sm, bool bCameFromBlockString);
	void addOBBToVec(OBBVec *pOBBVec);
};

class Block : public ByMonads {
 private:
	eBlockKind m_kind;
	GapBlock* m_gap_block;
	OptGapBlock* m_opt_gap_block;
	ObjectBlock* m_object_block;
	Power *m_power_block;
 public:
	Block(OptGapBlock* opt_gap_block);	
	Block(GapBlock* gap_block);
	Block(Power* power);
	Block(ObjectBlock* object_block);
	Block(ObjectBlock* object_block, MQLMonadSetElement *pMSE);
	virtual ~Block();
	void getOutermostSetOfMonadsFeatureSet(std::set<std::string>& som_name_set);

	void weed(MQLExecEnv *pEE, bool& bResult, bool is_first, bool is_last, node_number_t block_string2_parent, bool& bHasMetNonNOTEXISTInBlockString);
	bool symbol(MQLExecEnv *pEE, bool bDoCalculatePreQueryString, bool& bResult);
	bool symbol(MQLExecEnv *pEE, bool& bResult);
	bool symbolObjectReferences(MQLExecEnv *pEE, bool& bResult, std::set<std::string>& ORD_set);
	bool symbolObjectReferences2(MQLExecEnv *pEE);
	bool type(MQLExecEnv *pEE, eObjectRangeType contextRangeType, bool& bResult);
	eBlockKind getKind() { return m_kind; };
	bool hasObjectBlockInBlockString();
	bool isGapOrOptGapBlock(void); // Is it an opt_gap_block or a gap_block?
	bool isPowerBlock() { return m_kind == kPowerBlock; };
	bool isNOTEXISTObjectBlock(void) { return m_object_block != 0 && m_object_block->isNOTEXIST(); };
	GapBlock* getGapBlock(void) { return m_gap_block; };
	OptGapBlock* getOptGapBlock(void) { return m_opt_gap_block; };
	ObjectBlock* getObjectBlock(void) { return m_object_block; };
	Power *getPowerBlock(void) { return m_power_block; };
	bool isStar(void) const { return m_monad_set != 0; };
	bool aggregateQuery(MQLExecEnv *pEE, FastSetOfMonads& characteristic_set, const SetOfMonads& Su, eAggregateQueryStrategy strategy, monad_m largest_object_length_above, String2COBPtrMMap& mmap);
	void canChooseAQStrategyInnermostFirst(bool &bResult);
	bool hasFirstLast() const;
	void calculateMMapOutermost(String2COBPtrMMap& mmap, const std::string& prefix, EMdFDB *pDB);
	void calculateMMapInnermost(String2COBPtrMMap& mmap, std::string& inner_charstring, EMdFDB *pDB);
	void addOBBToVec(OBBVec *pOBBVec);
	StartMonadIterator* getSMI(MQLExecEnv *pEE, const SetOfMonads& U, const SetOfMonads& Su, monad_m Sm, bool bCameFromBlockString);
};


class Power {
 private:
	emdf_ivalue m_limit_low;
	emdf_ivalue m_limit_high;
 public:
	Power(emdf_ivalue limit); // m_limit_low is 0, m_limit_high is limit
	Power(emdf_ivalue limit_low, emdf_ivalue limit_high);
	virtual ~Power();
	void weed(bool &bResult);
	emdf_ivalue getLimitLow() const { return m_limit_low; };
	emdf_ivalue getLimitHigh() const { return m_limit_high; };
};




class BlockString0 {
private:
	Block *m_block;
	BlockString *m_block_string;
public:
	BlockString0(Block* block);
	BlockString0(BlockString* block_string);
	virtual ~BlockString0();
	void getOutermostSetOfMonadsFeatureSet(std::set<std::string>& som_name_set);

	void weedPower(MQLExecEnv *pEE, bool& bResult);
	void weed(MQLExecEnv *pEE, bool& bResult, bool is_first, bool is_last, bool bPrevIsGap, bool bPrevIsPower, node_number_t block_string2_parent, bool& bHasMetNonNOTEXISTInBlockString);
	bool symbol(MQLExecEnv *pEE, bool bDoCalculatePreQueryString, bool& bResult);
	bool symbolObjectReferences(MQLExecEnv *pEE, bool& bResult, std::set<std::string>& ORD_set);
	bool symbolObjectReferences2(MQLExecEnv *pEE);
	bool hasObjectBlockInBlockString();
	bool isBlock() { return m_block_string == 0; };
	bool isBlockString() { return m_block_string != 0; };
	bool symbol(MQLExecEnv *pEE, bool& bResult);
	bool type(MQLExecEnv *pEE, eObjectRangeType contextRangeType, bool& bResult);
	Block* getBlock() { return m_block; };
	BlockString *getBlockString() { return m_block_string; }
	bool aggregateQuery(MQLExecEnv *pEE, FastSetOfMonads& characteristic_set, const SetOfMonads& Su, eAggregateQueryStrategy strategy, monad_m largest_object_length_above, String2COBPtrMMap& mmap);
	void canChooseAQStrategyInnermostFirst(bool &bResult);
	bool hasFirstLast() const;
	bool firstBlockIsPower();
	void calculateMMapOutermost(String2COBPtrMMap& mmap, const std::string& prefix, EMdFDB *pDB);
	void calculateMMapInnermost(String2COBPtrMMap& mmap, std::string& inner_charstring, EMdFDB *pDB);
	void addOBBToVec(OBBVec *pOBBVec);
	StartMonadIterator* getSMI(MQLExecEnv *pEE, const SetOfMonads& U, const SetOfMonads& Su, monad_m Sm);
};



class BlockString1 {
private:
	BlockString0 *m_block_string0;
	SetOfMonads *m_pStarSOM;
	MQLMonadSetElement *m_pMQLMSE;
public:
	BlockString1(BlockString0* block_string0);
	BlockString1(BlockString0* block_string0, MQLMonadSetElement *pMSEs);
	virtual ~BlockString1();
	void getOutermostSetOfMonadsFeatureSet(std::set<std::string>& som_name_set);

	void weedPower(MQLExecEnv *pEE, bool& bResult);
	void weed(MQLExecEnv *pEE, bool& bResult, bool is_first, bool is_last, bool bPrevIsGap, bool bPrevIsPower, node_number_t block_string2_parent, bool& bHasMetNonNOTEXISTInBlockString);
	bool symbol(MQLExecEnv *pEE, bool bDoCalculatePreQueryString, bool& bResult);
	bool symbolObjectReferences(MQLExecEnv *pEE, bool& bResult, std::set<std::string>& ORD_set);
	bool symbolObjectReferences2(MQLExecEnv *pEE);
	bool hasObjectBlockInBlockString();
	bool isNonStar() { return m_pStarSOM == 0; };
	bool isStar() { return m_pStarSOM != 0; };
	bool symbol(MQLExecEnv *pEE, bool& bResult);
	bool type(MQLExecEnv *pEE, eObjectRangeType contextRangeType, bool& bResult);
	BlockString0* getBlockString0() { return m_block_string0; };
	const SetOfMonads *getStarSOM() const { return m_pStarSOM; }
	bool aggregateQuery(MQLExecEnv *pEE, FastSetOfMonads& characteristic_set, const SetOfMonads& Su, eAggregateQueryStrategy strategy, monad_m largest_object_length_above, String2COBPtrMMap& mmap);
	void canChooseAQStrategyInnermostFirst(bool &bResult);
	bool hasFirstLast() const;
	bool firstBlockIsPower();
	void calculateMMapOutermost(String2COBPtrMMap& mmap, const std::string& prefix, EMdFDB *pDB);
	void calculateMMapInnermost(String2COBPtrMMap& mmap, std::string& inner_charstring, EMdFDB *pDB);
	void addOBBToVec(OBBVec *pOBBVec);
	StartMonadIterator* getSMI(MQLExecEnv *pEE, const SetOfMonads& U, const SetOfMonads& Su, monad_m Sm);
};



class BlockString2 : public QueryNode {
private:
	BlockString1 *m_block_string1;
	BlockString2 *m_block_string2;
	bool m_bIsExclamation;
	node_number_t m_block_string2_parent;
	std::set<std::string> m_ORD_children;
	const Straw* m_pORUStraw;
public:
	BlockString2(BlockString1* block_string1);
	BlockString2(BlockString1* block_string1, BlockString2* block_string2, bool bHasExclamation);
	virtual ~BlockString2();
	void getOutermostSetOfMonadsFeatureSet(std::set<std::string>& som_name_set);

	void weedPower(MQLExecEnv *pEE, bool& bResult);
	void weed(MQLExecEnv *pEE, bool& bResult, bool is_first, bool is_last, bool bPrevIsGap, bool bPrevIsPower, node_number_t block_string2_parent, bool& bHasMetNonNOTEXISTInBlockString);
	bool symbol(MQLExecEnv *pEE, bool bDoCalculatePreQueryString, bool& bResult);
	bool symbolObjectReferences(MQLExecEnv *pEE, bool& bResult, std::set<std::string>& ORD_set);
	bool symbolObjectReferences2(MQLExecEnv *pEE);
	bool isBlockString1() { return m_block_string2 == 0; };
	bool isBlockString1BlockString2() { return m_block_string2 != 0; };
	bool symbol(MQLExecEnv *pEE, bool& bResult);
	bool type(MQLExecEnv *pEE, eObjectRangeType contextRangeType, bool& bResult);
	BlockString1* getBlockString1() { return m_block_string1; };
	BlockString2 *getBlockString2() { return m_block_string2; }
	bool aggregateQuery(MQLExecEnv *pEE, FastSetOfMonads& characteristic_set, const SetOfMonads& Su, eAggregateQueryStrategy strategy, monad_m largest_object_length_above, String2COBPtrMMap& mmap);
	void canChooseAQStrategyInnermostFirst(bool &bResult);
	bool hasFirstLast() const;
	bool firstBlockIsPower();
	void calculateMMapOutermost(String2COBPtrMMap& mmap, const std::string& prefix, EMdFDB *pDB);
	void calculateMMapInnermost(String2COBPtrMMap& mmap, std::string& inner_charstring, EMdFDB *pDB);
	void addOBBToVec(OBBVec *pOBBVec);
	bool nextIsPowerBlock();
	Power *getPowerBlock();
	
	void setORUStraw(const Straw* pORUStraw) { m_pORUStraw = pORUStraw; };
	const Straw* getORUStraw(void) const { return m_pORUStraw; };

	void addORDChild(const std::string& ORD);

	node_number_t getBlockString2Parent(void) const { return m_block_string2_parent; };
	
	bool hasORDChild(const std::string& ORD);

	ObjectBlock* getImmediateObjectBlock(void);

	bool hasObjectBlockInBlockString();

	bool hasImmediateORDChild(const std::string& ORD);
};




class BlockString {
private:
	BlockString2 *m_block_string2;
	BlockString *m_block_string;
public:
	BlockString(BlockString2* block_string2);
	BlockString(BlockString2* block_string2, BlockString* block_string);
	virtual ~BlockString();

	void getOutermostSetOfMonadsFeatureSet(std::set<std::string>& som_name_set);

	void weedPower(MQLExecEnv *pEE, bool& bResult);
	void weed(MQLExecEnv *pEE, bool& bResult, bool is_first, bool is_last, bool bPrevIsGap, bool bPrevIsPower, node_number_t block_string2_parent, bool& bHasMetNonNOTEXISTInBlockString);
	bool symbol(MQLExecEnv *pEE, bool bDoCalculatePreQueryString, bool& bResult);
	bool symbolObjectReferences(MQLExecEnv *pEE, bool& bResult, std::set<std::string>& ORD_set);
	bool symbolObjectReferences2(MQLExecEnv *pEE);
	bool hasObjectBlockInBlockString();
	bool isBlockString2() { return m_block_string == 0; };
	bool isBlockString2ORBlockString() { return m_block_string != 0; };
	bool symbol(MQLExecEnv *pEE, bool& bResult);
	bool type(MQLExecEnv *pEE, eObjectRangeType contextRangeType, bool& bResult);
	BlockString2* getBlockString2() { return m_block_string2; };
	BlockString *getBlockString() { return m_block_string; }
	bool aggregateQuery(MQLExecEnv *pEE, FastSetOfMonads& characteristic_set, const SetOfMonads& Su, eAggregateQueryStrategy strategy, monad_m largest_object_length_above, String2COBPtrMMap& mmap);
	void canChooseAQStrategyInnermostFirst(bool &bResult);
	bool hasFirstLast() const;
	bool firstBlockIsPower();
	void calculateMMapOutermost(String2COBPtrMMap& mmap, const std::string& prefix, EMdFDB *pDB);
	void calculateMMapInnermost(String2COBPtrMMap& mmap, std::string& inner_charstring, EMdFDB *pDB);
	void addOBBToVec(OBBVec *pOBBVec);
	StartMonadIterator* getSMI(MQLExecEnv *pEE, const SetOfMonads& U, const SetOfMonads& Su, monad_m Sm);
};

class ObjectBlockString {
 protected:
	ObjectBlockString *m_object_block_string;
	ObjectBlock *m_object_block;
 public:
	ObjectBlockString(ObjectBlock *object_block);
	ObjectBlockString(ObjectBlockString *object_block_string, ObjectBlock *object_block);
	~ObjectBlockString();

	ObjectBlockString *getObjectBlockString() { return m_object_block_string; };
	ObjectBlock* getObjectBlock() { return m_object_block; };
	emdf_ivalue getObjectBlockCount();

	void weed(MQLExecEnv *pEE, bool& bResult, node_number_t block_string2_parent, bool& bHasMetNonNOTEXISTInBlockString);
	bool symbol(MQLExecEnv *pEE, bool& bResult);
	bool type(MQLExecEnv *pEE, eObjectRangeType contextRangeType, bool& bResult);
	void getOutermostSetOfMonadsFeatureSet(std::set<std::string>& som_name_set);
	StartMonadIterator* getSMI(MQLExecEnv *pEE, const SetOfMonads& U, const SetOfMonads& Su, monad_m Sm);

	void calculateMMapOutermost(String2COBPtrMMap& mmap, const std::string& prefix, EMdFDB *pDB);
	void calculateMMapInnermost(String2COBPtrMMap& mmap, std::string& inner_charstring, EMdFDB *pDB);
	void addOBBToVec(OBBVec *pOBBVec);
	bool symbolObjectReferences(MQLExecEnv *pEE, bool& bResult, std::set<std::string>& ORD_set);
	bool symbolObjectReferences2(MQLExecEnv *pEE);

	bool aggregateQuery(MQLExecEnv *pEE, FastSetOfMonads& characteristic_set, const SetOfMonads& Su, eAggregateQueryStrategy strategy, monad_m largest_object_length_above, String2COBPtrMMap& mmap);
	void canChooseAQStrategyInnermostFirst(bool &bResult);
};

class UnorderedGroup {
 protected:
	ObjectBlockString *m_object_block_string;
 public:
	UnorderedGroup(ObjectBlockString *object_block_string);
	~UnorderedGroup();

	ObjectBlockString *getObjectBlockString() { return m_object_block_string; };
	
	void getOutermostSetOfMonadsFeatureSet(std::set<std::string>& som_name_set);
	void weed(MQLExecEnv *pEE, bool& bResult, node_number_t block_string2_parent);
	bool symbol(MQLExecEnv *pEE, bool& bResult);
	bool symbolObjectReferences(MQLExecEnv *pEE, bool& bResult, std::set<std::string>& ORD_set);
	bool symbolObjectReferences2(MQLExecEnv *pEE);
	emdf_ivalue getObjectBlockCount() { return m_object_block_string->getObjectBlockCount(); };
	
	bool type(MQLExecEnv *pEE, eObjectRangeType contextRangeType, bool& bResult);
	bool aggregateQuery(MQLExecEnv *pEE, FastSetOfMonads& characteristic_set, const SetOfMonads& Su, eAggregateQueryStrategy strategy, monad_m largest_object_length_above, String2COBPtrMMap& mmap);
	void canChooseAQStrategyInnermostFirst(bool &bResult);
	bool hasFirstLast() const;
	bool firstBlockIsPower();
	void calculateMMapOutermost(String2COBPtrMMap& mmap, const std::string& prefix, EMdFDB *pDB);
	void calculateMMapInnermost(String2COBPtrMMap& mmap, std::string& inner_charstring, EMdFDB *pDB);
	void addOBBToVec(OBBVec *pOBBVec);

};


class Blocks {
 private:
	BlockString* m_block_string;
	UsingRange* m_using_range;
	UnorderedGroup *m_unordered_group;
 public:
	Blocks(BlockString *block_string, UsingRange *using_range);
	Blocks(UnorderedGroup *unordered_group, UsingRange *using_range);
	virtual ~Blocks();
	
	void getOutermostSetOfMonadsFeatureSet(std::set<std::string>& som_name_set);

	bool isBlockString() const { return m_block_string != 0; };

	// Note: This and the following method are actually needed, 
	// since we need to be recursive.
	void weed(MQLExecEnv *pEE, bool& bResult, node_number_t block_string2_parent);
	bool symbol(MQLExecEnv *pEE, bool bDoCalculatePreQueryString, bool& bResult);
	bool symbol(MQLExecEnv *pEE, bool& bResult);
	bool symbolObjectReferences(MQLExecEnv *pEE, bool& bResult, std::set<std::string>& ORD_set);
	bool symbolObjectReferences2(MQLExecEnv *pEE);
	bool type(MQLExecEnv *pEE, eObjectRangeType contextRangeType, bool& bResult);
	BlockString* getBlockString() { return m_block_string; };
	UsingRange* getUsingRange() { return m_using_range; };
	UnorderedGroup* getUnorderedGroup() { return m_unordered_group; };
	void calculateMMapOutermost(String2COBPtrMMap& mmap, const std::string& prefix, EMdFDB *pDB);
	void calculateMMapInnermost(String2COBPtrMMap& mmap, std::string& inner_charstring, EMdFDB *pDB);
	bool aggregateQuery(MQLExecEnv *pEE, FastSetOfMonads& characteristic_set, const SetOfMonads& Su, eAggregateQueryStrategy strategy, monad_m largest_object_length_above, String2COBPtrMMap& mmap);
	void canChooseAQStrategyInnermostFirst(bool &bResult);
	bool hasFirstLast() const;
	void addOBBToVec(OBBVec *pOBBVec);
};

class Topograph {
 private:
	Blocks* m_blocks;
	String2COBPtrMMap m_mmap;
 public:
	Topograph(Blocks* blocks);
	virtual ~Topograph();
	void getOutermostSetOfMonadsFeatureSet(std::set<std::string>& som_name_set);
	void weed(MQLExecEnv *pEE, bool& bResult);
	bool symbol(MQLExecEnv *pEE, bool& bResult);
	bool type(MQLExecEnv *pEE, eObjectRangeType contextRangeType, bool& bResult);
	Blocks* getBlocks() { return m_blocks; };
	bool aggregateQuery(MQLExecEnv *pEE, FastSetOfMonads& characteristic_set, const SetOfMonads& Su, eAggregateQueryStrategy strategy);
	void canChooseAQStrategyInnermostFirst(bool &bResult);
	bool hasFirstLast() const;
	void calculateMMap(EMdFDB *pDB, eAggregateQueryStrategy strategy);
	void addOBBToVec(OBBVec *pOBBVec);
};



#endif /* MQL_QUERY_TYPES__H__ */
