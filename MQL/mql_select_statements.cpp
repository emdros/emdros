/*
 * mql_select_statements.cpp
 *
 * MQL SELECT statements
 *
 * Ulrik Petersen
 * Created: 3/6-2001 (March 6, 2001)
 * Last update: 5/11-2018
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


/**@file mql_select_statements.cpp
 *@brief Implementation of MQL SELECT statements (MQL layer)
 */


#include <emdfdb.h>
#include <mql_result.h>
#include <mql_select_statements.h>
#include <mql_query.h>
#include <string_func.h>
#include <mql_R.h>
#include <logging.h>
#include <monads.h>
#include <inst.h>


//////////////////////////////////////////////////////////////
//
// QueryBase
//
//////////////////////////////////////////////////////////////
QueryBase::QueryBase(eFocusSpec focus, 
		     MQLMonadSetElement* in_spec, 
		     MaxRange *max_range,
		     sheaf_return_type_pair *sheaf_return_type,
		     Topograph* query) 
	: m_focus(focus),
	  m_in(in_spec),
	  m_max_range(max_range),
	  m_sheaf_return_type(sheaf_return_type),
	  m_query(query),
	  m_pArbitrary_monad_set(0)
{
}

QueryBase::QueryBase(eFocusSpec focus, 
		     std::string *pArbitrary_monad_set,
		     MaxRange *max_range,
		     sheaf_return_type_pair *sheaf_return_type,
		     Topograph* query) 
	: m_focus(focus),
	  m_in(0),
	  m_max_range(max_range),
	  m_sheaf_return_type(sheaf_return_type),
	  m_query(query),
	  m_pArbitrary_monad_set(pArbitrary_monad_set)
{
}

QueryBase::~QueryBase()
{
	delete m_in;
	delete m_max_range;
	delete m_query;
	delete m_sheaf_return_type->second;
	delete m_sheaf_return_type;
	delete m_pArbitrary_monad_set;
}

void QueryBase::myWeed(MQLExecEnv *pEE, bool& bResult)
{
	m_max_range->weed(pEE, bResult);
	if (bResult) {
		m_query->weed(pEE, bResult);
	}
}

bool QueryBase::mySymbol(MQLExecEnv *pEE, bool& bResult)
{
	// Assume everything went well
	bResult = true;

	m_max_range->symbol(pEE, bResult);
	if (!bResult) {
		return true;
	}


	// Check arbitrary monad set existence, if necessary
	if (m_pArbitrary_monad_set != 0) {
		bool bExists;
		if (!pEE->pDB->monadSetExists(*m_pArbitrary_monad_set,
					      bExists,
					      m_Su)) {
			// DB error
			pEE->pError->appendError("Database error getting arbitrary monad set '" + *m_pArbitrary_monad_set + "'.");
			return false;
		} else {
			if (!bExists) {
				bResult = false;
				pEE->pError->appendError("Arbitrary monad set '" + *m_pArbitrary_monad_set + "' does not exist.");
				return true;
			}
		}
	}

	// If this is a flat sheaf, check the list of object type names
	if (m_sheaf_return_type->first == kFlatSheaf) {
		StringListConstIterator ci = m_sheaf_return_type->second->const_iterator();
		while (ci.hasNext()) {
			const std::string& object_type_name = ci.next();
      
			bool bDBResult;
			bool bObjectTypeExists;
			id_d_t object_type_id;
			eObjectRangeType dummyObjectRangeType;
			eMonadUniquenessType dummyMonadUniquenessType;
			bDBResult = pEE->pDB->objectTypeExists(object_type_name,
							       bObjectTypeExists, 
							       object_type_id,
							       dummyObjectRangeType,
							       dummyMonadUniquenessType);


			// Was there a DB error?
			if (!bDBResult) {
				pEE->pError->appendError("DB error checking existence of object type name '" + object_type_name + "' in flat sheaf list.\n");
				return false;
			}

			// Was the object type there?
			if (!bObjectTypeExists) {
				pEE->pError->appendError("Object type name '" + object_type_name + "' in flat sheaf list does not exist.\n");
				return false;
			}

			// If we came this far, it existed, 
			// so add it to the list
			m_flat_sheaf_object_types.push_back(std::pair<id_d_t, std::string>(object_type_id, object_type_name));
		}
	}

	return m_query->symbol(pEE, bResult);
}

bool QueryBase::myType(MQLExecEnv *pEE, bool& bResult)
{
	m_max_range->type(pEE, bResult);
	if (!bResult) {
		return true;
	}

	// FIXME: We should give kORTSingleRange if m_Su is a single range.
	return m_query->type(pEE, kORTMultipleRange, bResult);
}

bool QueryBase::myMonads(MQLExecEnv *pEE, bool& bResult)
{
	// Build m_Su
	if (m_in != 0)
		m_in->monadsBuildSet(m_Su);

	// Get all_m-1
	bool bAllM1IsEmptyDatabase = false;
	try {
		if (!pEE->updateAll_m_1()) {
			return false;
		}
	} catch (BadMonadsException& e) {
		bAllM1IsEmptyDatabase = true;
	}

	if (pEE->m_all_m_1.isEmpty()) {
		bAllM1IsEmptyDatabase = true;
	}

	// restrict Su
	if (bAllM1IsEmptyDatabase) {
		m_Su = SetOfMonads();
	} else {
		std::set<std::string> som_name_set;
		m_query->getOutermostSetOfMonadsFeatureSet(som_name_set);
		if (som_name_set.size() == 0) {
			// This means that we only had [gap] and
			// [gap?]  blocks in the outermost layer.
			// Assume that the correct one is "monads".
			m_Su = SetOfMonads::intersect(m_Su, pEE->m_all_m_1);
		} else if (som_name_set.size() == 1) {
			std::string som_name1 = *som_name_set.begin();
			if (strcmp_nocase(som_name1, "monads") == 0) {
				m_Su = SetOfMonads::intersect(m_Su, pEE->m_all_m_1);
			} else {
				// FIXME: Do an intersection with the
				// all_m_1 of the non-"monads" monad
				// set.
			}
		} else {
			// FIXME: Do a big-union of all_m_1 and the
			// all_m_1 of the other SetOfMonads feature
			// names.
		}
	}

	m_max_range->monads(pEE, bResult);
	if (!bResult) {
		return true;
	}


	// If we got this far, there were no DB errors
	return true;
}

bool QueryBase::myExec(MQLExecEnv *pEE, MQLResult **ppMQLResult)
{
	bool bDBError = m_max_range->exec(pEE);
	if (!bDBError) {
		return false;
	}

	// If substrate is empty, we cannot retrieve anything.
	// We will even throw BadMonadsException if we go on.
	if (m_Su.isEmpty()) {
		pEE->pOBBVec = 0;

		// Declare sheaf
		Sheaf* pSheaf;

		// Write to log
		LOG_WRITE_TIME("QueryBase::myExec()", "Not executing because substrate is empty");

		// Create failed sheaf
		pSheaf = new Sheaf(true);

		// Make new MQLResult from sheaf
		*ppMQLResult = new MQLResult(pSheaf, pEE);
	} else {
		pEE->pOBBVec = new OBBVec;

		eAggregateQueryStrategy strategy;

		bool bCanChooseInnermostFirstStrategy = true;
		m_query->canChooseAQStrategyInnermostFirst(bCanChooseInnermostFirstStrategy);

		if (bCanChooseInnermostFirstStrategy) {
			if (pEE->m_all_m_1.getCardinality() > 2000000) {
				bCanChooseInnermostFirstStrategy = true;
			} else {
				bCanChooseInnermostFirstStrategy = true;
				//bCanChooseInnermostFirstStrategy = false;
			}
		}
		
		if (bCanChooseInnermostFirstStrategy) {
			strategy = kAQSInnermostFirst;
		} else {
			strategy = kAQSOutermostFirst;
		}

		// Calculate multi-maps
		m_query->calculateMMap(pEE->pDB, strategy);
		
			
		FastSetOfMonads outermostSOM;
		if (strategy == kAQSOutermostFirst) {
			outermostSOM = m_Su;
		} else {
			// Nothing to do to outermostSOM
		}

		

		// Perform aggregate querying
		if (!m_query->aggregateQuery(pEE, outermostSOM, m_Su, strategy))
			return false;

		// Add copies of each object block's ObjectBlockBase to pEE->pOBBVec.
		// These will be used instead of the object block
		// when constructing the matched object
		m_query->addOBBToVec(pEE->pOBBVec);

		// Declare variables
		Sheaf* pSheaf;

		// Make universe
		SetOfMonads U(m_Su.first(), m_Su.last());

		// Write to log
		LOG_WRITE_TIME("QueryBase::myExec()", "R_topograph starting.");

		// Execute query
		try {
			pSheaf = R_topograph(pEE, U, m_Su, m_max_range->getMaxRange(), m_query);
		} catch (EMdFDBException& e) {
			return false;
		}

		// Write to log
		LOG_WRITE_TIME("QueryBase::myExec()", "R_topograph finished.");

		// Make new MQLResult from sheaf
		*ppMQLResult = new MQLResult(pSheaf, pEE);
	}

	// If this is a flat sheaf, then flatten the sheaf
	if (m_sheaf_return_type->first == kFlatSheaf) {
		(*ppMQLResult)->flatten(m_flat_sheaf_object_types);
	}

	// If we got this far, there were no DB errors
	return true;
}


/////////////////////////////////////////////////////////////////
//
// SELECT (focus|all|) OBJECTS
//
// SelectStatement
//
/////////////////////////////////////////////////////////////////
SelectStatement::SelectStatement(MQLExecEnv *pEE, 
				 eFocusSpec focus, 
				 MQLMonadSetElement* in_spec, 
				 MaxRange *pMaxRange,
				 sheaf_return_type_pair *sheaf_return_type,
				 Topograph* query) 
	: Statement(pEE), QueryBase(focus, in_spec, pMaxRange, sheaf_return_type, query)
{
}

SelectStatement::SelectStatement(MQLExecEnv *pEE, 
				 eFocusSpec focus, 
				 std::string* arbitrary_monad_set,
				 MaxRange *pMaxRange,
				 sheaf_return_type_pair *sheaf_return_type,
				 Topograph* query) 
	: Statement(pEE), QueryBase(focus, arbitrary_monad_set, pMaxRange, sheaf_return_type, query)
{
}

SelectStatement::~SelectStatement()
{
}

void SelectStatement::weed(bool& bResult)
{
	// Just call base-class
	QueryBase::myWeed(m_pEE, bResult);
}

bool SelectStatement::symbol(bool& bResult)
{
	// Just call base-class
	return QueryBase::mySymbol(m_pEE, bResult);
}

bool SelectStatement::type(bool& bResult)
{
	// Just call base-class
	return QueryBase::myType(m_pEE, bResult);
}

bool SelectStatement::monads(bool& bResult)
{
	// Just call base-class
	return QueryBase::myMonads(m_pEE, bResult);
}

bool SelectStatement::exec()
{
	// Just call base-class
	return QueryBase::myExec(m_pEE, &m_result);
}

/////////////////////////////////////////////////////////////////
//
// SELECT OBJECTS AT
//
// SelectObjectsAtStatement
//
/////////////////////////////////////////////////////////////////
SelectObjectsAtStatement::SelectObjectsAtStatement(MQLExecEnv *pEE, monad_m at_monad, std::string* object_type_name) : Statement(pEE)
{
	m_at_monad = at_monad;
	m_object_type_name = object_type_name;
	m_object_type_id = NIL;
}

SelectObjectsAtStatement::~SelectObjectsAtStatement()
{
	delete m_object_type_name;
}

//
// - Check that the object type exists
//
bool SelectObjectsAtStatement::symbol(bool& bResult)
{
	// - Check that the object type exists
	eObjectRangeType dummyObjectRangeType;
	eMonadUniquenessType dummyMonadUniquenessType;
	if (!m_pEE->pDB->objectTypeExists(*m_object_type_name,
					  bResult,
					  m_object_type_id,
					  dummyObjectRangeType,
					  dummyMonadUniquenessType))
		return false;
	if (!bResult) {
		m_pEE->pError->appendError("Object type name '" + *m_object_type_name + "' does not exist.\n");
		return true;
	}
  
	// If we got this far, there were no DB errors and no compiler errors
	bResult = true;
	return true;
}

//
// - Just ask the database
//
bool SelectObjectsAtStatement::exec()
{
	// Get results from DB
	IDDList DBResult;
	if (!m_pEE->pDB->getObjectsStartingAtSm(*m_object_type_name, 
						m_at_monad, 
						DBResult))
		return false;

	// Create new MQLResult
	m_result = new MQLResult();
	m_result->appendHeader("id_d", kTCID_D);
  
	// Add the results from the DB to MQLResult
	IDDList::const_iterator cbegin(DBResult.begin());
	IDDList::const_iterator ci(cbegin);
	IDDList::const_iterator cend(DBResult.end());
	while (ci != cend) {
		// Add newline unless it is the first
		if (ci != cbegin)
			m_result->newline();

		// Append this id_d
		m_result->append(id_d2string(*ci));

		// Advance iterator
		++ci;
	}

	// If we got this far, there were no DB errors
	return true;
}


/////////////////////////////////////////////////////////////////
//
// SELECT OBJECTS HAVING MONADS IN
//
// SelectObjectsHavingMonadsInStatement
//
/////////////////////////////////////////////////////////////////
SelectObjectsHavingMonadsInStatement::SelectObjectsHavingMonadsInStatement(MQLExecEnv *pEE, 
									   MQLMonadSetElement* monads, 
									   const std::string& object_type_name) : ObjectTypesStatement(pEE, object_type_name), m_AST_monads(monads)
{
}

SelectObjectsHavingMonadsInStatement::~SelectObjectsHavingMonadsInStatement()
{
	delete m_AST_monads;
}

// 
// - Check that all the ranges of monads are positive and monotonic
//
void SelectObjectsHavingMonadsInStatement::weed(bool& bResult)
{
	// Monad-ranges should be positive and monotonic
	m_AST_monads->weed(m_pEE, true, bResult);
	if (!bResult)
		return;

	// If we got this far, there were no DB errors and no compiler errors
	bResult = true;
	return;
}

//
// - Build the set of monads
//
bool SelectObjectsHavingMonadsInStatement::monads(bool& bResult)
{
	//
	// - Build the set of monads
	//
	m_AST_monads->monadsBuildSet(m_monads);
  
	// There were no DB errors and no compiler errors
	bResult = true;
	return true;
} 


bool SelectObjectsHavingMonadsInStatement::exec()
{
	// Make result
	m_result = new MQLResult();
	m_result->appendHeader("object_type_name", kTCString);
	m_result->appendHeader("monad_m", kTCMonad_m);
	m_result->appendHeader("id_d", kTCID_D);

	// Call base class
	return ObjectTypesStatement::exec();
}

bool SelectObjectsHavingMonadsInStatement::execOneType(const ObjectTypeInfo& oti)
{
	// Get object type name
	std::string object_type_name = oti.getObjectTypeName();
	eObjectRangeType objectRangeType = oti.getObjectRangeType();

	// Ask DB
	if (!m_pEE->pDB->getObjectsHavingMonadsInMonadSet(object_type_name, 
							  m_monads,
							  objectRangeType,
							  *m_result)) {
		m_result->clear();
		return false;
	}

	// If we got this far, there were no DB errors
	return true;
}


  



/////////////////////////////////////////////////////////////////
//
// GET OBJECTS HAVING MONADS IN
//
// GetObjectsHavingMonadsInStatement
//
/////////////////////////////////////////////////////////////////
GetObjectsHavingMonadsInStatement::GetObjectsHavingMonadsInStatement(MQLExecEnv *pEE, 
								     MQLMonadSetElement* monads, 
								     std::string *object_type_name,
								     std::string *monads_feature) 
	: ObjectTypeStatement(pEE, object_type_name), 
	  m_AST_monads(monads),
	  m_bGetAll(false),
	  m_pFeaturesToGet(0),
	  m_monads_feature(monads_feature)
{
}

GetObjectsHavingMonadsInStatement::GetObjectsHavingMonadsInStatement(MQLExecEnv *pEE, 
								     MQLMonadSetElement* monads, 
								     std::string *object_type_name,
								     std::string *monads_feature,
								     bool bDummy)
	: ObjectTypeStatement(pEE, object_type_name), 
	  m_AST_monads(monads),
	  m_bGetAll(true),
	  m_pFeaturesToGet(0),
	  m_monads_feature(monads_feature)
{
	UNUSED(bDummy);
}

GetObjectsHavingMonadsInStatement::GetObjectsHavingMonadsInStatement(MQLExecEnv *pEE, 
								     MQLMonadSetElement* monads, 
								     std::string *object_type_name,
								     std::string *monads_feature,
								     GrammarFeature *pFeaturesToGet) 
	: ObjectTypeStatement(pEE, object_type_name), 
	  m_AST_monads(monads),
	  m_bGetAll(false),
	  m_monads_feature(monads_feature)
{
	if (pFeaturesToGet != 0) {
		m_pFeaturesToGet = Feature::FromGrammarFeature(pFeaturesToGet);
		delete pFeaturesToGet;
	}
}


GetObjectsHavingMonadsInStatement::~GetObjectsHavingMonadsInStatement()
{
	delete m_AST_monads;
	delete m_pFeaturesToGet;
	delete m_monads_feature;
}

// 
// - Check that all the ranges of monads are positive and monotonic
//
void GetObjectsHavingMonadsInStatement::weed(bool& bResult)
{
	// Monad-ranges should be positive and monotonic
	m_AST_monads->weed(m_pEE, true, bResult);
	if (!bResult)
		return;

	// If we got this far, there were no compiler errors
	bResult = true;
	return;
}


//
// - Check that the features exist for the given object type.
//
bool GetObjectsHavingMonadsInStatement::symbol(bool& bResult)
{
	if (!ObjectTypeStatement::symbolObjectTypeExists(bResult)) {
		return false;
	}
	if (!bResult) {
		return true;
	}

	// Is this ALL?
	if (m_bGetAll) {
		std::list<FeatureInfo> features_of_OT;
		if (!m_pEE->pDB->getFeaturesForObjectType(m_object_type_id,
							  features_of_OT)) {
			m_pEE->pError->appendError("Database error getting features for object type '" + *m_object_type_name + "'.\n");
			return false;
		}
		std::list<FeatureInfo>::reverse_iterator feat_rci = features_of_OT.rbegin();
		while (feat_rci != features_of_OT.rend()) {
			std::string featureName = feat_rci->getName();

			// We even add "self"
			Feature *pNewFeature = new Feature(new std::string(featureName), m_pFeaturesToGet);
			m_pFeaturesToGet = pNewFeature;

			feat_rci++;
		}
	}


	//
	if (m_pFeaturesToGet != 0) {
		m_pFeaturesToGet->assignListIndex(0);

		if (!m_pFeaturesToGet->symbolFeaturesExist(m_pEE, 
							   m_object_type_id, 
							   bResult))
			return false;
		if (!bResult)
			return true;
	}

	if (m_pFeaturesToGet != 0) {
		m_pObjectBlockBase = new ObjectBlockBase(new std::string(*m_object_type_name),
							 new std::string(""),
							 kRetrieve,
							 new Feature(*m_pFeaturesToGet),
							 false // Is (not, in this case) NOT EXIST
							 );
	} else {
		m_pObjectBlockBase = new ObjectBlockBase(new std::string(*m_object_type_name),
							 new std::string(""),
							 kRetrieve,
							 (Feature*) 0,
							 false // Is (not, in this case) NOT EXIST
							 );
	}

	m_pEE->pOBBVec = new OBBVec;
	m_pEE->pOBBVec->push_back(m_pObjectBlockBase);
  
	if (!m_pObjectBlockBase->symbol(m_pEE, bResult)) {
		return false;
	}
	if (!bResult) {
		return true;
	}

	if (strcmp_nocase(*m_monads_feature, "monads") != 0) {
		bool bMonadFeatureExists;
		id_d_t monad_feature_type_id;
		std::string default_value;   // Dummy variable
		bool is_computed;            // Dummy variable
		if (!m_pEE->pDB->featureExists(*m_monads_feature,
					     m_object_type_id,
					     bMonadFeatureExists,
					     monad_feature_type_id,
					     default_value,
					     is_computed))
			return false;
		if (!bMonadFeatureExists) {
			m_pEE->pError->appendError("Monad feature " + *m_monads_feature + " does not exist on object type " + *m_object_type_name);
			return false;
		}

		// Strictly speaking, this should be done in the type
		// stage, but we'll cut a corner here since we know
		// the information right now.
		if (!featureTypeIdIsSOM(monad_feature_type_id)) {
			m_pEE->pError->appendError("Feature " + *m_monads_feature + " on object type " + *m_object_type_name + " is not of type set of monads.");
			return false;
		}
	}
				  


	// If we got this far, there were no DB errors and no compiler errors
	bResult = true;
	return true;
}

bool GetObjectsHavingMonadsInStatement::type(bool& bResult)
{
	if (!ObjectTypeStatement::type(bResult)) {
		return false;
	}
	if (!bResult) {
		return true;
	}

	//
	// - Get feature name and type
	//
	if (m_pFeaturesToGet != 0) {
		if (!m_pFeaturesToGet->typeFeatureName(m_pEE, bResult))
			return false;
		if (!bResult)
			return true;
	}

	// If we got this far, there were no DB errors and no compiler errors
	bResult = true;
	return true;
}


//
// - Build the set of monads
//
bool GetObjectsHavingMonadsInStatement::monads(bool& bResult)
{
	//
	// - Build the set of monads
	//
	m_AST_monads->monadsBuildSet(m_monads);
  
	// There were no DB errors and no compiler errors
	bResult = true;
	return true;
} 


bool GetObjectsHavingMonadsInStatement::exec()
{
	std::list<std::pair<id_d_t, std::string> > object_type_list;
	object_type_list.push_back(std::pair<id_d_t, std::string>(m_object_type_id, *m_object_type_name));

	Inst *pInst;
	std::vector<std::string> FeatureNames;
	std::list<FeatureInfo> FeatureInfos;
	if (m_pFeaturesToGet != 0) {
		m_pFeaturesToGet->execMakeFeatureNameVector(FeatureNames);
		m_pFeaturesToGet->execMakeFeatureList(FeatureInfos);
	}
	//unsigned int no_of_features_to_get = FeatureNames.size(); // Unused variable
	pInst = new Inst(FeatureNames);

	// Get all_m-1
	bool bAllM1IsEmptyDatabase = false;
	try {
		if (!m_pEE->pDB->getAll_m_1(m_pEE->m_all_m_1)) {
			return false;
		}
	} catch (BadMonadsException& e) {
		bAllM1IsEmptyDatabase = true;
	}
  
	// Return an empty inst
	if (bAllM1IsEmptyDatabase) {
		delete pInst;
		ListOfStraws *pLOS = new ListOfStraws();
		Sheaf *pSheaf = new Sheaf(pLOS);

		m_result = new MQLResult(pSheaf, m_pEE);
		m_result->flatten(object_type_list);
		return true;
	}

	// We currently don't have the grammar support for doing pre
	// query strings.
	std::string pre_query_string = "";
	EMdFFFeatures *pre_query_constraints = 0;

	if (!m_pEE->pDB->getObjectsHavingMonadsIn(*m_object_type_name,
						  m_object_type_id,
						  m_objectRangeType,
						  m_monads,
						  m_pEE->m_all_m_1,
						  pre_query_string,
						  pre_query_constraints,
						  FeatureInfos,
						  *m_monads_feature,
						  pInst)) {
		delete pInst;
		Sheaf *pSheaf = new Sheaf(true); // A failed sheaf
		m_result = new MQLResult(pSheaf, m_pEE);
		m_result->flatten(object_type_list);
		return false;
	}


	/*
	FlatSheaf *pFlatSheaf = new FlatSheaf(object_type_list);
	SetOfMonads Universe = SetOfMonads(1, MAX_MONAD);
	pInst->set_current_universe(Universe);
	Inst::const_iterator ci = pInst->begin();
	while (ci.hasNext()) {
		const InstObject* pInstObject = ci.next();

		id_d_t id_d = pInstObject->getID_D();

		MatchedObject *pMo = 0;
		if (pInstObject->somHasOnlyOneMSE()) {
			pMo = new MatchedObject(id_d,
						pInstObject->first(),
						pInstObject->last(),
						0, // inner sheaf
						m_pObjectBlockBase);
		} else {
			SetOfMonads som;
			pInstObject->getMonads(som);

			pMo = new MatchedObject(id_d,
						som,
						0, // inner sheaf
						m_pObjectBlockBase);
		}
		
		if (m_pFeaturesToGet != 0) {
			unsigned int feature_index = 0;
			for (feature_index = 0;
			     feature_index < no_of_features_to_get;
			     feature_index++) {
				const EMdFValue *pInstObjectValue = pInstObject->getFeature(feature_index);
				EMdFValue *pNewValue = new EMdFValue(*pInstObjectValue);
				pMo->addEMdFValue(feature_index, pNewValue);
			}
		}

		pFlatSheaf->addMO(pMo);
	}

	delete pInst;

	*/

	FlatSheaf *pFlatSheaf = new FlatSheaf(object_type_list);
	FlatStraw *pFlatStraw = new FlatStraw(*m_object_type_name, pInst, m_pObjectBlockBase);
	pFlatSheaf->setOBBVec(m_pEE->pOBBVec);
	pFlatSheaf->addFlatStraw(m_object_type_id, pFlatStraw);

	
	m_result = new MQLResult(pFlatSheaf, m_pEE);

	// If we came this far, there were no DB errors
	return true;
}

  

/////////////////////////////////////////////////////////////////
//
// SELECT AGGREGATE FEATURES 
//
// GetAggregateFeaturesStatement
//
/////////////////////////////////////////////////////////////////
GetAggregateFeaturesStatement::GetAggregateFeaturesStatement(MQLExecEnv *pEE,
							     AggregateFeature *aggregate_feature_list,
							     MQLMonadSetElement* in_clause,
							     std::string *monads_feature,
							     std::string *object_type_name,
							     FFeatures *feature_constraints) 
	: ObjectTypeStatement(pEE, object_type_name),
	  m_aggregate_feature_list(aggregate_feature_list),
	  m_AST_monads(in_clause),
	  m_monads_feature(monads_feature),
	  m_feature_constraints(feature_constraints),
	  m_pFeaturesToGet(0),
	  m_pObjectBlock(0)
{
}

GetAggregateFeaturesStatement::~GetAggregateFeaturesStatement()
{
	delete m_aggregate_feature_list;
	delete m_AST_monads;
	delete m_monads_feature;
	delete m_feature_constraints;
	delete m_pFeaturesToGet;	
	delete m_pObjectBlock;
}

// 
// - Check that all the ranges of monads are positive and monotonic
//
void GetAggregateFeaturesStatement::weed(bool& bResult)
{
	// Monad-ranges should be positive and monotonic
	m_AST_monads->weed(m_pEE, true, bResult);
	if (!bResult)
		return;

	if (m_feature_constraints != 0) {
		node_number_t ffeatures_parent = 0;
		m_feature_constraints->weedFeatureConstraints(m_pEE, bResult, ffeatures_parent);
	}

	if (!bResult) {
		return;
	}

	if (m_aggregate_feature_list != 0) {
		m_aggregate_feature_list->weed(m_pEE, bResult);
	}
	return;
}


//
// - Check that the features exist for the given object type.
//
bool GetAggregateFeaturesStatement::symbol(bool& bResult)
{
	if (!ObjectTypeStatement::symbolObjectTypeExists(bResult)) {
		return false;
	}
	if (!bResult) {
		return true;
	}

	AggregateFeature *pAF = m_aggregate_feature_list;
	
	if (pAF != 0) {
		bool bDBResult = pAF->symbol(m_pEE, *m_object_type_name, m_object_type_id, bResult);
		if (!bDBResult || !bResult) {
			return bDBResult;
		}
	}

	std::set<std::string> feature_name_set;
	pAF = m_aggregate_feature_list;
	while (pAF != 0) {
		if (pAF->hasFeature()) {
			std::string normalized_feature_name;
			str_tolower(pAF->getFeatureName(), normalized_feature_name);
			feature_name_set.insert(normalized_feature_name);
		
		}

		if (pAF->hasFeatureComparison()) {
			pAF->getFeatureComparison()->symbolAddFeaturesToSet(feature_name_set);
		}

		pAF = pAF->getNext();
	}
	if (m_feature_constraints != 0) {
		// First call symbol()
		if (!m_feature_constraints->symbol(m_pEE,
						   *m_object_type_name,
						   m_object_type_id,
						   bResult)) {
			return false;
		}

		if (!bResult) {
			return true;
		}
		
		m_feature_constraints->symbolAddFeaturesToSet(feature_name_set);
	}	

	// Create m_pFeaturesToGet
	std::set<std::string>::const_iterator fns_ci = feature_name_set.begin();
	std::set<std::string>::const_iterator fns_cend = feature_name_set.end();
	Feature *pNextFeature = 0;
	while (fns_ci != fns_cend) {
		std::string *feature_name = new std::string(*fns_ci);
		m_pFeaturesToGet = new Feature(feature_name, pNextFeature);
		pNextFeature = m_pFeaturesToGet;
		++fns_ci;
	}

	if (m_pFeaturesToGet != 0) {
		m_pFeaturesToGet->assignListIndex(0);

		if (!m_pFeaturesToGet->symbolFeaturesExist(m_pEE, 
							   m_object_type_id, 
							   bResult))
			return false;
		if (!bResult)
			return true;
	}

	// Set feature index into pInstObj on all pAF's
	std::vector<std::string> FeatureNames;
	if (m_pFeaturesToGet != 0) {
		m_pFeaturesToGet->execMakeFeatureNameVector(FeatureNames);
	}
	
	pAF = m_aggregate_feature_list;
	while (pAF != 0) {
		if (pAF->hasFeatureComparison()) {
			if (!pAF->getFeatureComparison()->symbolSetFeatureIndex(FeatureNames, bResult)) {
				m_pEE->pError->appendError("Symbol checking failed on one of the Aggregate Features.\n");
				return false;
			}
			if (!bResult) {
				m_pEE->pError->appendError("Symbol checking failed on one of the Aggregate Features.\n");
				return true;
			}
		}

		pAF = pAF->getNext();
	}

	/*
	 * This has to be re-done in the exec() stage, since
	 * ObjectBlock's take-over means that
	 * FeatureComparison::m_feature_index is obliterated.
	 *
	 * It is done here, as well as in the exec stage, because
	 * we need to actually symbol-check.
	 *
	 */
	if (m_feature_constraints != 0) {
		if (!m_feature_constraints->symbolSetFeatureIndex(FeatureNames, bResult)) {
			return false;
		}
		if (!bResult) {
			m_pEE->pError->appendError("Symbol checking failed on the Object Block's feature constraints.\n");
			return true;
		}
	}


	// Create m_pObjectBlock
	if (m_pFeaturesToGet != 0) {
		m_pObjectBlock = new ObjectBlock(new std::string(*m_object_type_name),
						 new std::string(""), // mark_declaration
						 new std::string(""), // object_reference
						 kRetrieve,
						 kNoFirstLast,
						 new MonadSetRelationClause(kMSROPartOf, new std::string(*m_monads_feature), kMSNSubstrate),
						 m_feature_constraints,
						 new Feature(*m_pFeaturesToGet),
						 (Blocks*) 0, // opt_blocks
						 false // Is (not, in this case) NOT EXIST
						 );
	} else {
		m_pObjectBlock = new ObjectBlock(new std::string(*m_object_type_name),
						 new std::string(""), // mark_declaration
						 new std::string(""), // object_reference
						 kRetrieve,
						 kNoFirstLast,
						 new MonadSetRelationClause(kMSROPartOf, new std::string(*m_monads_feature), kMSNSubstrate),
						 m_feature_constraints,
						 (GrammarFeature*)0, // No other features to retrieve
						 (Blocks*) 0, // opt_blocks
						 false // Is (not, in this case) NOT EXIST
						 );
	}
	m_feature_constraints = 0; // m_pObjectBlock has now officially taken over m_feature_constraints.
	
	/*
	 * DON'T do this.
	 * It is completely unnecessary, and we delete m_pObjectBlockBase
	 * ourselves (following C++ best practices).
	 * We don't want it deleted in MQLExecEnv d'tor.
	 *
	 */
	/*
	m_pEE->pOBBVec = new OBBVec;
	m_pEE->pOBBVec->push_back(m_pObjectBlock);
	*/

	bool bDoCalculatePreQueryString = true;
	if (!m_pObjectBlock->symbol(m_pEE, bDoCalculatePreQueryString, bResult)) {
		return false;
	}
	
	if (!bResult) {
		return true;
	}

	if (strcmp_nocase(*m_monads_feature, "monads") != 0) {
		bool bMonadFeatureExists;
		id_d_t monad_feature_type_id;
		std::string default_value;   // Dummy variable
		bool is_computed;            // Dummy variable
		if (!m_pEE->pDB->featureExists(*m_monads_feature,
					       m_object_type_id,
					       bMonadFeatureExists,
					       monad_feature_type_id,
					       default_value,
					       is_computed))
			return false;
		if (!bMonadFeatureExists) {
			m_pEE->pError->appendError("Monad feature " + *m_monads_feature + " does not exist on object type " + *m_object_type_name);
			return false;
		}

		// Strictly speaking, this should be done in the type
		// stage, but we'll cut a corner here since we know
		// the information right now.
		if (!featureTypeIdIsSOM(monad_feature_type_id)) {
			m_pEE->pError->appendError("Feature " + *m_monads_feature + " on object type " + *m_object_type_name + " is not of type set of monads.");
			return false;
		}
	}

	// If we got this far, there were no DB errors and no compiler errors
	bResult = true;
	return true;
}

bool GetAggregateFeaturesStatement::type(bool& bResult)
{
	if (!ObjectTypeStatement::type(bResult)) {
		return false;
	}
	if (!bResult) {
		return true;
	}

	//
	// - Get feature name and type
	//
	if (m_pFeaturesToGet != 0) {
		if (!m_pFeaturesToGet->typeFeatureName(m_pEE, bResult))
			return false;
		if (!bResult)
			return true;
	}

	if (m_pObjectBlock != 0) {
		eObjectRangeType contextRangeType = kORTMultipleRange;
		if (!m_pObjectBlock->type(m_pEE, contextRangeType, bResult)) {
			return false;
		}
		if (!bResult) {
			return true;
		}
	}

	// If we got this far, there were no DB errors and no compiler errors
	bResult = true;
	return true;
}


//
// - Build the set of monads
//
bool GetAggregateFeaturesStatement::monads(bool& bResult)
{
	//
	// - Build the set of monads
	//
	m_AST_monads->monadsBuildSet(m_monads);
  
	// There were no DB errors and no compiler errors
	bResult = true;
	return true;
} 


bool GetAggregateFeaturesStatement::exec()
{
	std::list<std::pair<id_d_t, std::string> > object_type_list;
	object_type_list.push_back(std::pair<id_d_t, std::string>(m_object_type_id, *m_object_type_name));

	Inst *pInst = 0;
	std::vector<std::string> FeatureNames;
	std::list<FeatureInfo> FeatureInfos;
	if (m_pFeaturesToGet != 0) {
		m_pFeaturesToGet->execMakeFeatureNameVector(FeatureNames);
		m_pFeaturesToGet->execMakeFeatureList(FeatureInfos);
	}
	// unsigned int no_of_features_to_get = FeatureNames.size(); // Unused variable

	// Get pre-query string
	std::string pre_query_string(m_pObjectBlock->getPreQueryString());
	EMdFFFeatures *pre_query_constraints = m_pObjectBlock->getEMdFConstraints(m_pEE->pDB);


	// Get all_m-1
	bool bAllM1IsEmptyDatabase = false;
	try {
		if (!m_pEE->pDB->getAll_m_1(m_pEE->m_all_m_1)) {
			return false;
		}
	} catch (BadMonadsException& e) {
		bAllM1IsEmptyDatabase = true;
	}
  
	// Return an empty table
	if (bAllM1IsEmptyDatabase) {
		delete pInst;
		m_result = new MQLResult();
		return true;
	}

	// Make inst
	pInst = new Inst(FeatureNames);
	if (!m_pEE->pDB->getInst(*m_object_type_name,
				 m_object_type_id,
				 m_monads, 
				 m_pEE->m_all_m_1,
				 pre_query_string,
				 pre_query_constraints,
				 FeatureInfos,
				 *m_monads_feature,
				 kMSROPartOf,
				 *pInst)) {
		// DB failure.
		// Return an empty Table
		delete pInst;
		m_result = new MQLResult();
		return false;
	}
              	
	
	m_result = new MQLResult();

	// Find out indexes to retrieve, and their AggregateFeature objects
	std::vector<AggregateFeature*> aggregate_features_vec;
	AggregateFeature *pAF = m_aggregate_feature_list;
	while (pAF != 0) {
		std::string AF_feature_name = pAF->getFeatureName();

		if (!AF_feature_name.empty()) {
			int feature_index = m_pFeaturesToGet->getFeatureInstListIndex(AF_feature_name);

			// Did we find it or not?
			if (feature_index < 0) {
				// Did not find it.  This is really a logic
				// error, but since this is at the advanced
				// 'exec' stage, we don't throw an exception
				// -- we return failure.

				// Return empty table
				delete pInst;
				return false;
			}
		
			pAF->setInstObjectFeatureIndex(feature_index);
		} else {
			pAF->setInstObjectFeatureIndex(-1);
		}
		aggregate_features_vec.push_back(pAF);
		
		pAF = pAF->getNext();
	}

	FFeatures *feature_constraints = m_pObjectBlock->getFeatureConstraints();

	if (feature_constraints != 0) {
		bool bDummyResult; // Already checked in symbol() stage
		feature_constraints->symbolSetFeatureIndex(FeatureNames, bDummyResult);
		(void) bDummyResult; // Silence a compiler warning
	}
	


	//
	// Execute aggregation
	//
	pInst->set_current_universe(m_pEE->m_all_m_1);

	Inst::const_iterator inst_it = pInst->begin();
	if (feature_constraints == 0) {
		while (inst_it.hasNext()) {
			const InstObject *pInstObj = inst_it.next();

			// This calls AggregateFeature::exec() on
			// AggregateFeature::m_next, if non-nil.
			m_aggregate_feature_list->exec(m_pEE, pInstObj);
		}
	} else { // feature_constraints != 0
		while (inst_it.hasNext()) {
			const InstObject *pInstObj = inst_it.next();
			
			// Feature constraints present.
			// Only do it if they are true.
			if (feature_constraints->exec(m_pEE, pInstObj)) {
				// This calls AggregateFeature::exec() on
				// AggregateFeature::m_next, if non-nil.
				m_aggregate_feature_list->exec(m_pEE, pInstObj);
			}
		}
	}

	//
	// Add header
	//
	int nColumnNumber = 1;
	pAF = m_aggregate_feature_list;
	while (pAF != 0) {
		std::string column_name = "Column" + long2string(nColumnNumber);

		m_result->appendHeader(column_name, kTCInteger);

		++nColumnNumber;
		pAF = pAF->getNext();
	}

	//
	// Add values
	//
	// They appear in reverse order, due to the reverse order nature
	// of the LALR(1) grammar's parsing of the aggregate_feature_list.
	// Hence, we put them in a std::list<std::string>, and push them
	// at the front.
	pAF = m_aggregate_feature_list;
	std::list<std::string> column_values;
	while (pAF != 0) {
		column_values.push_front(long2string(pAF->getResult()));

		pAF = pAF->getNext();
	}
	m_result->append(column_values);
	

	// Clean up
	delete pInst;
	
	// If we came this far, there were no DB errors
	return true;
}

  



/////////////////////////////////////////////////////////////////
//
// SELECT OBJECT TYPES
//
// SelectObjectTypesStatement
//
/////////////////////////////////////////////////////////////////
SelectObjectTypesStatement::SelectObjectTypesStatement(MQLExecEnv *pEE) : Statement(pEE)
{
}

SelectObjectTypesStatement::~SelectObjectTypesStatement()
{
}

bool SelectObjectTypesStatement::exec()
{
	// Ask database
	std::list<std::string> l;
	if (!m_pEE->pDB->getObjectTypes(l)) {
		return false;
	}

	// Create new MQLResult
	m_result = new MQLResult();
	m_result->appendHeader("object type", kTCString);
  
	// Add the results from the DB to MQLResult
	std::list<std::string>::const_iterator ci(l.begin());
	std::list<std::string>::const_iterator cend(l.end());
	while (ci != cend) {
		m_result->append(*ci);
		++ci;
		if (ci != cend)
			m_result->newline();
	}

	// Return no database error
	return true;
}



////////////////////////////////////////////////////////////////
//
// SELECT FEATURES
//
// SelectFeaturesStatement
//
///////////////////////////////////////////////////////////////
//
// - Check that the object type exists
//
bool SelectFeaturesStatement::symbol(bool& bResult)
{
	//
	// - Check that the object type exists
	//
	if (!symbolObjectTypeExists(bResult)) {
		if (!bResult) {
			m_pEE->pError->appendError("Object type name '" + *m_object_type_name + "' does not exist.\n");
			return true;
		}
	}

	//
	// If we got this far, there were no database errors
	return true;
}

bool SelectFeaturesStatement::exec()
{
	// Get result
	std::list<FeatureInfo> FeatureInfos;
	if (!m_pEE->pDB->getFeaturesForObjectType(m_object_type_id,
						  FeatureInfos))
		return false;

	// Create new MQLResult
	m_result = new MQLResult();
	m_result->appendHeader("feature name", kTCString);
	m_result->appendHeader("type", kTCString);
	m_result->appendHeader("default", kTCString);
	m_result->appendHeader("computed", kTCBool);

	// Convert result
	std::list<FeatureInfo>::const_iterator ci(FeatureInfos.begin());
	std::list<FeatureInfo>::const_iterator cend(FeatureInfos.end());
	while (ci != cend) {
		// Name
		m_result->append(ci->getName());

		// Type
		std::string strType_id;
		bool bTypeIDGottenOK = m_pEE->pDB->typeIdToString(ci->getType(), strType_id);
		m_result->append(strType_id);

		// Default value
		std::string defval;
		if (m_pEE->pOut->isXML() || m_pEE->pOut->isJSON()) {
			defval = ci->getDefaultValue();
		} else {
			defval = std::string("'") + ci->getDefaultValue() + "'";
		}
		m_result->append(defval);

		// Is_computed
		m_result->append(bool_alpha2string(ci->getIsComputed()));
		if (!bTypeIDGottenOK)
			return false;
		++ci;
		if (ci != cend)
			m_result->newline();
	}
  
	// If we got this far, there were no database errors
	return true;
}


////////////////////////////////////////////////////////////////
//
// SELECT ENUMERATIONS
//
// SelectEnumerationsStatement
//
///////////////////////////////////////////////////////////////
SelectEnumerationsStatement::SelectEnumerationsStatement(MQLExecEnv *pEE) : Statement(pEE)
{
}

SelectEnumerationsStatement::~SelectEnumerationsStatement()
{
}

bool SelectEnumerationsStatement::exec()
{
	// Ask database
	std::list<std::string> l;
	if (!m_pEE->pDB->getEnumerations(l)) {
		return false;
	}

	// Create new MQLResult
	m_result = new MQLResult();
	m_result->appendHeader("enum name", kTCString);
  
	// Add the results from the DB to MQLResult
	std::list<std::string>::const_iterator ci(l.begin());
	std::list<std::string>::const_iterator cend(l.end());
	while (ci != cend) {
		m_result->append(*ci);
		++ci;
		if (ci != cend)
			m_result->newline();
	}

	// Return no database error
	return true;
}


////////////////////////////////////////////////////////////////
//
// SELECT ENUMERATION CONSTANTS
//
// SelectEnumerationConstantsStatement
//
///////////////////////////////////////////////////////////////
bool SelectEnumerationConstantsStatement::symbol(bool& bResult)
{
	// Check that the enumeration exists and get its enum_id
	if (!symbolEnumerationExists(bResult, true)) 
		return false;

	// Return no database error
	return true;
}

bool SelectEnumerationConstantsStatement::exec()
{
	// Get result
	std::list<EnumConstInfo> l;
	if (!m_pEE->pDB->getEnumConstants(m_enum_id, l))
		return false;

	// Create new MQLResult
	m_result = new MQLResult();
	m_result->appendHeader("enum const", kTCString);
	m_result->appendHeader("value", kTCInteger);
	m_result->appendHeader("default", kTCBool);

	// Convert result
	std::list<EnumConstInfo>::const_iterator ci(l.begin());
	std::list<EnumConstInfo>::const_iterator cend(l.end());
	while (ci != cend) {
		m_result->append(ci->getName());
		m_result->append(int2string(ci->getValue()));
		m_result->append(bool_alpha2string(ci->getIsDefault()));
		++ci;
		if (ci != cend)
			m_result->newline();
	}
  
	// If we got this far, there were no database errors
	return true;
}


////////////////////////////////////////////////////////////////
//
// SELECT OBJECT TYPES USING ENUMERATION
//
// SelectObjectTypesUsingEnumerationStatement
//
///////////////////////////////////////////////////////////////
bool SelectObjectTypesUsingEnumerationStatement::symbol(bool& bResult)
{
	// Check that the enumeration exists and get its enum_id
	if (!symbolEnumerationExists(bResult, true))
		return false;

	// Return no database error
	return true;
}

bool SelectObjectTypesUsingEnumerationStatement::exec()
{
	// Ask database
	std::list<std::string> l;
	if (!m_pEE->pDB->getObjectTypesUsingEnumeration(m_enum_id, l)) {
		return false;
	}

	// Create new MQLResult
	m_result = new MQLResult();
	m_result->appendHeader("object type name", kTCString);
  
	// Add the results from the DB to MQLResult
	std::list<std::string>::const_iterator ci(l.begin());
	std::list<std::string>::const_iterator cend(l.end());
	while (ci != cend) {
		m_result->append(*ci);
		++ci;
		if (ci != cend)
			m_result->newline();
	}

	// Return no database error
	return true;
}


////////////////////////////////////////////////////////////////
//
// SELECT MAX_M
//
// SelectMaxMStatement
//
///////////////////////////////////////////////////////////////
bool SelectMaxMStatement::exec()
{
	// Ask database
	monad_m max_m;
	if (!m_pEE->pDB->getMax_m(max_m)) {
		return false;
	}

	// Create new MQLResult
	m_result = new MQLResult();
	m_result->appendHeader("max_m", kTCMonad_m);
  
	// Add the results from the DB to MQLResult
	m_result->startNewRow();
	m_result->append(monad_m2string(max_m));

	// Return no database error
	return true;
}

////////////////////////////////////////////////////////////////
//
// SELECT MIN_M
//
// SelectMinMStatement
//
///////////////////////////////////////////////////////////////
bool SelectMinMStatement::exec()
{
	// Ask database
	monad_m min_m;
	if (!m_pEE->pDB->getMin_m(min_m)) {
		return false;
	}

	// Create new MQLResult
	m_result = new MQLResult();
	m_result->appendHeader("min_m", kTCMonad_m);
  
	// Add the results from the DB to MQLResult
	m_result->startNewRow();
	m_result->append(monad_m2string(min_m));

	// Return no database error
	return true;
}


////////////////////////////////////////////////////////////////
//
// SELECT MONAD SETS
//
// SelectMonadSetsStatement
//
///////////////////////////////////////////////////////////////
bool SelectMonadSetsStatement::exec()
{
	std::list<std::string> monad_set_names;
	// Get all monad sets into m_monad_set_names
	if (!m_pEE->pDB->selectMonadSets(monad_set_names)) {
		m_pEE->pError->appendError("Could not get monad set names.\n");
		return false;
	}

	// Make result
	m_result = new MQLResult();
	m_result->appendHeader("monad_set_name", kTCString);

	std::list<std::string>::const_iterator ci;
	for (ci = monad_set_names.begin(); ci != monad_set_names.end(); ++ci) {
		m_result->startNewRow();
		m_result->append(*ci);
	}

	// If we got this far, there were no DB errors
	return true;
}


