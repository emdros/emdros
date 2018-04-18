/*
 * mql_object_statements.cpp
 *
 * MQL OBJECT statements
 *
 * Ulrik Petersen
 * Created: 3/6-2001 (March 6, 2001)
 * Last update: 3/1-2017
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2017  Ulrik Sandborg-Petersen
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

/**@file mql_object_statements.cpp
 *@brief Implementation of MQL OBJECT statements (MQL layer)
 */


#include <iostream>
#include <emdfdb.h>
#include <mql_object_statements.h>
#include <string_func.h>
#include <inst_object.h>


////////////////////////////////////////////////////////////
//
// CreateObjectStatement
//
// Base class for all CREATE OBJECT statements
//
////////////////////////////////////////////////////////////
CreateObjectStatement::CreateObjectStatement(MQLExecEnv *pEE, 
					     id_d_t with_id_d, 
					     ObjectSpec* object_creation_spec)
	: Statement(pEE),
	  m_with_id_d(with_id_d),
	  m_object_creation_spec(object_creation_spec)
    
{
}

CreateObjectStatement::~CreateObjectStatement()
{
	delete m_object_creation_spec;
}

//
// - Check that the feature "self" is not assigned a value
//
// - Check that "object_type_name" is not all_m, pow_m, or any_m
// 
void CreateObjectStatement::weed(bool& bResult)
{
	//
	// - Check that the feature "self" is not assigned a value
	// - Check all feature assignments with weed()
	// 
	m_object_creation_spec->weed(m_pEE, bResult);
	if (!bResult)
		return;

	// 
	// - Check that "object_type_name" is not all_m, pow_m, or any_m
	// 
	m_object_creation_spec->weedObjectTypeNameNotSpecial(m_pEE, bResult);
	if (!bResult)
		return;

	// If we got this far, it means there were no errors
	bResult = true;
	return;
}

bool local_getFeatureEnv(MQLExecEnv *pEE,
			 const std::list<FeatureInfo>& object_type_features,
			 FeatureEnv &feature_env,
			 ObjectSpecBase *pObjectSpecBase)
{
	int feature_env_size = 0;
	std::list<FeatureInfo>::const_iterator it = object_type_features.begin();
	while (it != object_type_features.end()) {
		if (!is_self(it->getName())) {
			++feature_env_size;
		}
		++it;
	}
	feature_env.clearAndSetSize(feature_env_size);
	int feature_index = 0;
	it = object_type_features.begin();
	while (it != object_type_features.end()) {
		id_d_t feature_type_id;
		feature_type_id = it->getType();
		
		std::string lowercase_feature_name;
		str_tolower(it->getName(), lowercase_feature_name);

		if (is_self(lowercase_feature_name)) {
			++it;
			continue;
		}

		
		bool bEnumExists;
		std::string enum_name;
		std::string *pEnum_name;
		
		MQLType *pMQLType;
		
		// Decide type
		bool bIsWithIndex = featureTypeIdIsWithIndex(feature_type_id);
		bool bIsFromSet = featureTypeIdIsFromSet(feature_type_id);
		bool bIsListOf = featureTypeIdIsListOf(feature_type_id);
		switch (feature_type_id & FEATURE_TYPE_TYPE_MASK) {
		case FEATURE_TYPE_INTEGER:
			pMQLType = new MQLType(kInteger, bIsWithIndex);
			break;
		case FEATURE_TYPE_STRING:
			pMQLType = new MQLType(kString, bIsFromSet, bIsWithIndex);
			break;
		case FEATURE_TYPE_ASCII:
			pMQLType = new MQLType(kASCII, bIsFromSet, bIsWithIndex);
			break;
		case FEATURE_TYPE_ID_D:
			pMQLType = new MQLType(kID_D, bIsWithIndex);
			break;
		case FEATURE_TYPE_LIST_OF_INTEGER:
			pMQLType = new MQLType(kListOfInteger);
			break;
		case FEATURE_TYPE_LIST_OF_ID_D:
			pMQLType = new MQLType(kListOfID_D);
			break;
		case FEATURE_TYPE_ENUM:
		case FEATURE_TYPE_LIST_OF_ENUM:
			if (!pEE->pDB->enumExists(feature_type_id, bEnumExists, enum_name))
				return false;
			pEnum_name = new std::string(enum_name);
			pMQLType = new MQLType(pEnum_name, bIsListOf, bIsWithIndex);
			break;
		case FEATURE_TYPE_SET_OF_MONADS:
			pMQLType = new MQLType(kSetOfMonads, bIsWithIndex);
			break;
		default:
			ASSERT_THROW(false, "Unknown FEATURE_TYPE.");
			break;
		}
		
		// Assign type_id internally to the MQLType
		if (!pMQLType->typeAssignTypeID(pEE)) {
			delete pMQLType;
			return false;
		}
		FeatureInfo myfi(lowercase_feature_name,
				 it->getType(),
				 it->getDefaultValue(),
				 it->getIsComputed());
		feature_env.addEntry(feature_index, myfi, pMQLType);
		++feature_index;
		++it;
	}

	pObjectSpecBase->assignFeatureIndexes(feature_env);
	
	// Everything OK		
	return true;
}


bool CreateObjectStatement::getFeatureEnv(void)
{
	return local_getFeatureEnv(m_pEE, 
				   m_object_type_features, 
				   m_feature_env,
				   m_object_creation_spec);
}




//
// - If the user specified an id, check that the id is 
//   not in use already.
//
// - Check that the object type exists.  In doing so, store its 
//   object type_id in the AST.
//
// - All features must be exist.
//
// - Make sure that all features are given a value.  If a feature 
//   is not given an explicit value, then use the default value.
//   If a feature exists for the object type, but is not in the list
//   of feature assignments, then add it to the list of feature 
//   assignments.
//
bool CreateObjectStatement::symbol(bool& bResult)
{
	//
	// - If the user specified an id, check that the id is 
	//   not in use already.
	//
	if (m_with_id_d != NIL) {
		bool bID_DExists;
		id_d_t dummy_type_id;
		if (!m_pEE->pDB->objectID_DExists(m_with_id_d, bID_DExists, dummy_type_id))
			return false;
		if (bID_DExists) {
			m_pEE->pError->appendError(std::string("Object with id_d ") + id_d2string(m_with_id_d) + " exists already.  Cannot create new object with same id_d.\n");
			bResult = false;
			return true;
		}
	}

	//
	// - Check that the object type exists.  In doing so, store its 
	//   object type_id in the AST.
	//
	if (!m_object_creation_spec->symbolObjectTypeExists(m_pEE, bResult))
		return false;
	if (!bResult) {
		m_pEE->pError->appendError(std::string("Object type ") + m_object_creation_spec->getObjectTypeName() + " does not exist.  Cannot create objects of non-existing object type.\n");
		return true;
	}

	// Get list of object type features
	if (!m_pEE->pDB->getFeaturesForObjectType(m_object_creation_spec->getObjectTypeId(), 
						  m_object_type_features))
		return false;

	// This must be done before any other operation on
	// m_object_creation_spec that uses m_feature_env or which
	// indirectly uses FeatureAssignment::m_feature_index.
	if (!getFeatureEnv()) {
		return false;
	}


	//
	// - All features must be exist.
	//
	if (!m_object_creation_spec->symbolAllFeaturesMustExist(m_pEE, m_feature_env, bResult))
		return false;
	if (!bResult)
		return true;

	//
	// - Make sure that all features are given a value.  If a feature 
	//   is not given an explicit value, then use the default value.
	//   If a feature exists for the object type, but is not in the list
	//   of feature assignments, then add it to the list of feature 
	//   assignments.
	//
	if (!m_object_creation_spec->symbolAllFeaturesMustBeAssigned(m_pEE, m_feature_env))
		return false;

	// If we got this far, there were no compiler errors and not db errors
	bResult = true;
	return true;
}


//
// - Assign a type to each feature assignment
//
// - Check for type compatibility between feature and expression
//
bool CreateObjectStatement::type(bool& bResult)
{
	//
	// - Assign a type to each feature assignment
	//
	//if (!m_object_creation_spec->typeAssignTypes(m_pEE, m_object_type_features, m_feature_env))
	//return false;
	
	//
	// - Check for type compatibility between feature and expression
	//
	if (!m_object_creation_spec->typeTypeCompatibility(m_pEE, m_feature_env, bResult))
		return false;
	if (!bResult)
		return true;
	
	// If we got this far, there were no DB errors and no compiler errors
	bResult = true;
	return true;
}

//
// - Check for WITH SINGLE RANGE OBJECTS constraint.
// 
// Must be called in base class AFTER monad set has been created.
// 
bool CreateObjectStatement::monads(bool& bResult)
{
	// Assume it's OK.
	bResult = true;

	eObjectRangeType objectRangeType 
		= m_object_creation_spec->getObjectRangeType();

	// Is this a WITH SINGLE RANGE OBJECTS object type?
  
	if (objectRangeType == kORTSingleRange) {
		// Check that there is exactly one MSE
		if (!m_monads.hasOnlyOneMSE()) {
			m_pEE->pError->appendError("Object type has been declared WITH SINGLE RANGE OBJECTS, yet the monad set\ndoes not have exactly one stretch of monads.\n");
			bResult = false;
			return true;
		}
	} else if (objectRangeType == kORTSingleMonad) {
		// Check that there is exactly one monad
		if (m_monads.first() != m_monads.last()) {
			m_pEE->pError->appendError("Object type has been declared WITH SINGLE MONAD OBJECTS, yet the monad set\ndoes not have exactly one monad.\n");
			bResult = false;
			return true;
		}
	}

	// This statement can't fail DB-wise.
	return true;
}


bool CreateObjectStatement::exec()
{
	// Make object type name
	std::string object_type_name;
	object_type_name = m_object_creation_spec->getObjectTypeName();

	// Make list of features
	std::list<FeatureInfo> l;
	if (!m_object_creation_spec->execMakeFeatureList(m_pEE, m_feature_env, l))
		return false;

	// Create object
	if (!m_pEE->pDB->createObject(m_with_id_d, 
				      object_type_name, 
				      m_object_creation_spec->getObjectTypeId(),
				      m_monads, 
				      m_object_creation_spec->getObjectRangeType(),
				      l))
		return false;

	// Make result:  The id_d of the newly created object
	m_result = new MQLResult();
	m_result->append(id_d2string(m_with_id_d));
	m_result->appendHeader("id_d", kTCID_D);

	if (!m_pEE->updateAll_m_1()) {
		m_pEE->pError->appendError("Could not update all_m_1 after object creation.");
		return false;
	}

	// If we got this far, there were no database errors
	return true;
}



////////////////////////////////////////////////////////////
//
// CREATE OBJECT FROM MONADS
//
// CreateObjectFromMonadsStatement
//
////////////////////////////////////////////////////////////
CreateObjectFromMonadsStatement::CreateObjectFromMonadsStatement(
								 MQLExecEnv *pEE, 
								 MQLMonadSetElement* monad_set,
								 id_d_t with_id_d,
								 ObjectSpec* object_creation_spec)
	: CreateObjectStatement(pEE, with_id_d, object_creation_spec),
	  m_monad_set(monad_set)
{
}

CreateObjectFromMonadsStatement::~CreateObjectFromMonadsStatement()
{
	delete m_monad_set;
}

//
// - Check that the feature "self" is not declared 
//   (assigned a value)
//
// - Check that "object_type_name" is not all_m, pow_m, or any_m
// 
// - Check that all the ranges of monads are positive and monotonic
//
void CreateObjectFromMonadsStatement::weed(bool& bResult)
{
	// Call base class
	CreateObjectStatement::weed(bResult);
	if (!bResult)
		return;

	// Monad-ranges should be monotonic and positive
	m_monad_set->weed(m_pEE, true, bResult);
	if (!bResult)
		return;

	// If we got this far, there were no DB errors and no compiler errors
	bResult = true;
	return;
}

bool CreateObjectFromMonadsStatement::symbol(bool& bResult)
{
	// Call base class
	if (!CreateObjectStatement::symbol(bResult))
		return false;
	if (!bResult)
		return true;

	// If we got this far, there were no DB errors and no compiler errors
	bResult = true;
	return true;
}

//
// - Build the set of monads
// 
bool CreateObjectFromMonadsStatement::monads(bool& bResult)
{
	//
	// - Build the set of monads
	//
	m_monad_set->monadsBuildSet(m_monads);

	// Call base class
	if (!CreateObjectStatement::monads(bResult)) {
		return false;
	}
	if (!bResult) {
		return true;
	}

	// If we got this far, we had no database errors and no compiler errors
	bResult = true;
	return true;
}

bool CreateObjectFromMonadsStatement::exec()
{
	// If the user did not specify an id_d, autogenerate one
	if (m_with_id_d == NIL) {
		if (!m_pEE->pDB->getNextObjectID_D(m_with_id_d))
			return false;
	} else {
		// Otherwise, if he did specify an id_d, make sure that the next one
		// generated will be higher.
		if (!m_pEE->pDB->setNextObjectID_DIfNotHigher(m_with_id_d+1))
			return false;
	}

	// Call base class
	if (!CreateObjectStatement::exec())
		return false;

	// If we got this far, there were no DB errors
	return true;
}





////////////////////////////////////////////////////////////
//
// CREATE OBJECT FROM ID_DS
//
// CreateObjectFromID_DsStatement
//
////////////////////////////////////////////////////////////
CreateObjectFromID_DsStatement::CreateObjectFromID_DsStatement(
							       MQLExecEnv *pEE, 
							       ID_D* id_d_specification,
							       id_d_t with_id_d,
							       ObjectSpec* object_creation_spec)
	: CreateObjectStatement(pEE, with_id_d, object_creation_spec),
	  m_id_d_list(id_d_specification)
{

}

CreateObjectFromID_DsStatement::~CreateObjectFromID_DsStatement()
{
	delete m_id_d_list;
}

//
// - Check that the feature "self" is not declared 
//   (assigned a value)
//
// - Check that "object_type_name" is not all_m, pow_m, or any_m
// 
// - Check that none of the id_ds is NIL
//
void CreateObjectFromID_DsStatement::weed(bool& bResult)
{
	// Call base class
	CreateObjectStatement::weed(bResult);
	if (!bResult)
		return;

	// None of the id_ds may be NIL
	m_id_d_list->weedNoneIsNIL(m_pEE, bResult);
	if (!bResult)
		return;

	// If we got this far, there were no compiler errors and no DB errors
	bResult = true;
	return;
}

//
// - Check everything from base class
//
// - Check that the objects exist for all the id_ds
//
bool CreateObjectFromID_DsStatement::symbol(bool& bResult)
{
	// Check base-class
	if (!CreateObjectStatement::symbol(bResult))
		return false;
	if (!bResult)
		return true;

	// Check that all the objects exist
	if (!m_id_d_list->symbolObjectsExist(m_pEE, bResult))
		return false;
	if (!bResult)
		return true;

	// If we got this far, there were no compiler-errors and no DB-errors
	bResult = true;
	return true;
}

//
// - Build the set of monads from the id_ds
//
bool CreateObjectFromID_DsStatement::monads(bool& bResult)
{
	// Iterate through id_ds
	ID_D* pID_D;
	pID_D = m_id_d_list;
	while (pID_D != 0) {
		// Get monads from DB
		SetOfMonads id_d_monads;
		if (!m_pEE->pDB->getMonadsFromID_D(pID_D->getID_D(), pID_D->getObjectTypeId(), id_d_monads))
			return false;

		// Add id_d_monads to local monads
		m_monads.unionWith(id_d_monads);

		// Advance iterator
		pID_D = pID_D->getNext();
	}

	// Call base class
	if (!CreateObjectStatement::monads(bResult)) {
		return false;
	}
	if (!bResult) {
		return true;
	}

	// If we got this far, then there were no compile-errors and no DB errors
	bResult = true;
	return true;
}

// Simply call base-class
bool CreateObjectFromID_DsStatement::exec()
{
	// If the user did not specify an id_d, autogenerate one
	if (m_with_id_d == NIL) {
		if (!m_pEE->pDB->getNextObjectID_D(m_with_id_d))
			return false;
	} else {
		// Otherwise, if he did specify an id_d, make sure that the next one
		// generated will be higher.
		if (!m_pEE->pDB->setNextObjectID_DIfNotHigher(m_with_id_d+1))
			return false;
	}

	// Call base class
	if (!CreateObjectStatement::exec())
		return false;

	// If we got this far, there were no DB errors
	return true;
}



////////////////////////////////////////////////////////////
//
// CREATE OBJECT FROM (focus|all|) QUERY
//
// CreateObjectFromQueryStatement
//
////////////////////////////////////////////////////////////
CreateObjectFromQueryStatement::CreateObjectFromQueryStatement(
							       MQLExecEnv *pEE, 
							       eFocusSpec focus, 
							       id_d_t with_id_d,
							       ObjectSpec* object_creation_spec,
							       MQLMonadSetElement* in_spec, 
							       MaxRange *max_range,
							       sheaf_return_type_pair *sheaf_return_type,
							       Topograph* query)
	: CreateObjectStatement(pEE, with_id_d, object_creation_spec),
	  QueryBase(focus, in_spec, max_range, sheaf_return_type, query)
{

}

CreateObjectFromQueryStatement::~CreateObjectFromQueryStatement()
{

}
void CreateObjectFromQueryStatement::weed(bool& bResult)
{
	// Call base class
	CreateObjectStatement::weed(bResult);
	if (!bResult)
		return;

	// Call base class
	QueryBase::myWeed(m_pEE, bResult);
	if (!bResult)
		return;

	// If we got this far, there were no compiler errors
	bResult = true;
	return;
}

bool CreateObjectFromQueryStatement::symbol(bool& bResult)
{
	// Call base class
	if (!CreateObjectStatement::symbol(bResult))
		return false;
	if (!bResult)
		return true;

	// Call base class
	if (!QueryBase::mySymbol(m_pEE, bResult))
		return false;
	if (!bResult)
		return true;


	// TODO: Implement
	m_pEE->pError->appendError("Not implemented yet.\n");
	return false;
}

bool CreateObjectFromQueryStatement::type(bool& bResult)
{
	// Call base class
	QueryBase::myType(m_pEE, bResult);
	if (!bResult)
		return true;

	// TODO: Implement
	m_pEE->pError->appendError("Not implemented yet.\n");
	return false;
}

bool CreateObjectFromQueryStatement::monads(bool& bResult)
{
	// TODO: Implement
	m_pEE->pError->appendError("Not implemented yet.\n");
	return false;

	// This is where we determine the monad set...
	//
	// 

	// Call base class. This must be done AFTER the creation
	// of the monad set.
	if (!CreateObjectStatement::monads(bResult))
		return false;
	if (!bResult)
		return true;
}


bool CreateObjectFromQueryStatement::exec()
{
	// TODO: Implement
	m_pEE->pError->appendError("Not implemented yet.\n");
	return false;
}


////////////////////////////////////////////////////////////
//
// CreateObjectsStatement
//
////////////////////////////////////////////////////////////
CreateObjectsStatement::CreateObjectsStatement(MQLExecEnv *pEE, 
					       std::string *pObject_type_name,
					       ObjectSpecNoOT* object_spec_list)
	: Statement(pEE),
	  m_object_spec_list(object_spec_list),
	  m_pObject_type_name(pObject_type_name)
{
}


CreateObjectsStatement::~CreateObjectsStatement()
{
	delete m_pObject_type_name;

	// Delete, not recursively, but iteratively.
	// Will also set m_object_spec_list to 0
	disposeOfObjectSpecNoOT(&m_object_spec_list);
}


void CreateObjectsStatement::weed(bool& bResult)
{
	// Simply pass the buck
	m_object_spec_list->weed(m_pEE, bResult);
}

bool CreateObjectsStatement::getFeatureEnv(void)
{
	return local_getFeatureEnv(m_pEE, 
				   m_object_type_features, 
				   m_feature_env,
				   m_object_spec_list);
}




bool CreateObjectsStatement::symbol(bool& bResult)
{
	//
	// - Check that the object type exists.  In doing so, store its 
	//   object type_id in the AST.
	//
	eMonadUniquenessType dummyMonadUniquenessType;
	if (!m_object_spec_list->symbolObjectTypeExists(m_pEE, 
							*m_pObject_type_name, 
							m_object_type_id, 
							bResult,
							m_objectRangeType,
							dummyMonadUniquenessType))
		return false;
	if (!bResult) {
		m_pEE->pError->appendError(std::string("Object type ") + *m_pObject_type_name + " does not exist.  Cannot create objects of non-existing object type.\n");
		return true;
	}

	// Get list of object type features
	if (!m_pEE->pDB->getFeaturesForObjectType(m_object_type_id, 
						  m_object_type_features))
		return false;


	// This must be done before any other operation on
	// m_object_creation_spec that uses m_feature_env or which
	// indirectly uses FeatureAssignment::m_feature_index.
	if (!getFeatureEnv()) {
		return false;
	}

	// Pass the buck
	if (!m_object_spec_list->symbol(m_pEE,
					*m_pObject_type_name,
					m_object_type_id,
					m_feature_env,
					bResult))
		return false;
	if (!bResult)
		return true;

	// If we got this far, there were no compiler errors and no DB errors
	bResult = true;
	return true;
}


bool CreateObjectsStatement::type(bool& bResult)
{
	// Simply pass the buck
	if (!m_object_spec_list->type(m_pEE, 
				      m_object_type_id,
				      m_object_type_features, 
				      m_feature_env,
				      bResult)) 
		return false;
	if (!bResult)
		return true;

	// If we got this far, there were no compiler errors and no DB errors
	bResult = true;
	return true;
}


bool CreateObjectsStatement::monads(bool& bResult)
{
	// Simply pass the buck
	if (!m_object_spec_list->monads(m_pEE, 
					m_objectRangeType,
					bResult)) 
		return false;
	if (!bResult)
		return true;

	// If we got this far, there were no compiler errors and no DB errors
	bResult = true; 
	return true;
}


static void LocalDeleteObjectList(std::list<InstObject*>& l)
{
	std::list<InstObject*>::iterator it = l.begin();
	while (it != l.end()) {
		(*it)->~InstObject();
		++it;
	}
	l.erase(l.begin(), l.end());
}

bool CreateObjectsStatement::exec()
{
	// Create new MQLResult
	m_result = new MQLResult();
	m_result->appendHeader("object_count", kTCInteger);
	m_result->newline();

  
	// Remove 'self' from m_object_type_features.
	// This is so that we do not assign anything to it.
	// m_object_type_features (the features which the object type has)
	// is not the same as m_object_spec_list (the actual data to put in
	// each feature).  Self has already been weeded out of that
	// if it was there.
	std::list<FeatureInfo>::iterator it = m_object_type_features.begin();
	while (it != m_object_type_features.end()) {
		// If this is "self", then erase it and break out of the while loop,
		// since we are done.  All we want is to removed "self" 
		// from the list of features which the object type has.
		if (is_self(it->getName())) {
			m_object_type_features.erase(it);
			break;
		}
		++it;
	}

	// Start transaction if possible
	bool bDoCommit;
	bDoCommit = m_pEE->pDB->beginTransaction();

	// Make list of objects to create
	std::list<InstObject*> object_list;
	if (!m_object_spec_list->exec(m_pEE, object_list, m_feature_env)) {
		// Give error message
		m_pEE->pError->appendError(std::string("Something went wrong while making list of objects to create.\n"));

		// Abort transaction
		if (bDoCommit) {
			m_pEE->pDB->abortTransaction();
		}

		// Say in result that 0 were created
		m_result->append(long2string(0));

		// Delete objects in list
		LocalDeleteObjectList(object_list);

		// Return DB error
		return false;
	}
 
	// Create objects
	long object_count = 0;
	if (!m_pEE->pDB->createObjects(*m_pObject_type_name,
				       m_object_type_id,
				       m_object_type_features,
				       object_list,
				       m_objectRangeType,
				       object_count)) {
		// Give error message
		m_pEE->pError->appendError(std::string("Something went wrong while creating objects. Some objects may have been\ncreated.  Be sure to check the object count in the result table.\n"));

		// Abort transaction
		if (bDoCommit) {
			m_pEE->pDB->abortTransaction();
			// Say in result that 0 were created
			m_result->append(long2string(0));
		} else {
			// Say in result how many were created
			m_result->append(long2string(object_count));
		}

		// Delete objects in list
		LocalDeleteObjectList(object_list);

		// Return DB error
		return false;
	}

	// Commit transaction if necessary
	if (bDoCommit) {
		if (!m_pEE->pDB->commitTransaction()) {
			// Give error message
			m_pEE->pError->appendError(std::string("Could not commit transaction.\n"));

			// Say in result that 0 were created
			m_result->append(long2string(0));

			// Delete objects in list
			LocalDeleteObjectList(object_list);
      
			// Return DB error
			return false;
		}
	}

	// Say in result how many were created
	m_result->append(long2string(object_count));

	// Delete objects in list
	LocalDeleteObjectList(object_list);

	if (!m_pEE->updateAll_m_1()) {
		m_pEE->pError->appendError("Could not update all_m_1 after object creation.");
		return false;
	}

	// If we got this far, there were no DB errors
	return true;
}




////////////////////////////////////////////////////////////
//
// UpdateObjectsStatement
//
// Base class for all UPDATE OBJECTS statements
//
////////////////////////////////////////////////////////////
UpdateObjectsStatement::UpdateObjectsStatement(MQLExecEnv *pEE, ObjectSpec* update_spec)
	: Statement(pEE), m_update_spec(update_spec)
{
}

UpdateObjectsStatement::~UpdateObjectsStatement()
{
	delete m_update_spec;
}

//
// - Check that the feature "self" is not assigned a value
// 
// - Check that "object_type_name" is not all_m, pow_m, or any_m
// 
void UpdateObjectsStatement::weed(bool& bResult)
{
	//
	// - Check that the feature "self" is not assigned a value
	// - Check all feature-assignments with weed
	// 
	m_update_spec->weed(m_pEE, bResult);
	if (!bResult)
		return;

	// 
	// - Check that "object_type_name" is not all_m, pow_m, or any_m
	// 
	m_update_spec->weedObjectTypeNameNotSpecial(m_pEE, bResult);
	if (!bResult)
		return;

	// If we got this far, it means there were no compiler errors
	bResult = true;
	return;
}

bool UpdateObjectsStatement::getFeatureEnv(void)
{
	return local_getFeatureEnv(m_pEE, 
				   m_object_type_features, 
				   m_feature_env,
				   m_update_spec);
}



//
// - Check that the object type actually exists
//
// - Check that all the features exist
//
// - Check that none of the features are computed features
//
bool UpdateObjectsStatement::symbol(bool& bResult)
{
	if (m_update_spec != 0) {
		//
		// - Check that the object type actually exists
		//
		if (!m_update_spec->symbolObjectTypeExists(m_pEE, bResult))
			return false;
		if (!bResult)
			return true;

		// Get list of object type features
		if (!m_pEE->pDB->getFeaturesForObjectType(m_update_spec->getObjectTypeId(), 
							  m_object_type_features))
			return false;


		// This must be done before any other operation on
		// m_object_creation_spec that uses m_feature_env or
		// which indirectly uses
		// FeatureAssignment::m_feature_index.
		if (!getFeatureEnv()) {
			return false;
		}

		//
		// - Check that all the features exist
		//
		if (!m_update_spec->symbolAllFeaturesMustExist(m_pEE, m_feature_env, bResult))
			return false;
		if (!bResult)
			return true;

		//
		// - Check that none of the features are computed features
		//
		if (!m_update_spec->symbolNoFeatureMayBeComputed(m_pEE, bResult))
			return false;
		if (!bResult)
			return true;
	}

	// If we got this far, there were no DB errors and no compiler errors
	bResult = true;
	return true;
}

//
// - Assign a type to each feature assignment
//
// - Check for type compatibility between feature and expression
//
bool UpdateObjectsStatement::type(bool& bResult)
{
	//
	// - Assign a type to each feature assignment
	//
	//if (!m_update_spec->typeAssignTypes(m_pEE, m_object_type_features))
	//	return false;
	
	//
	// - Check for type compatibility between feature and expression
	//
	if (!m_update_spec->typeTypeCompatibility(m_pEE, m_feature_env, bResult))
		return false;
	if (!bResult)
		return true;
	
	// If we got this far, there were no DB errors and no compiler errors
	bResult = true;
	return true;
}

bool UpdateObjectsStatement::exec()
{
	// Get feature infos
	std::list<FeatureInfo> features;
	if (!m_update_spec->execMakeFeatureList(m_pEE, m_feature_env, features))
		return false;

	// Iterate through candidates, making result along the way
	std::list<id_d_t>::const_iterator ci(m_update_candidates.begin());
	std::list<id_d_t>::const_iterator cend(m_update_candidates.end());
	m_result = new MQLResult();
	m_result->appendHeader("id_d", kTCID_D);
	while (ci != cend) {
		// Update object
		if (!m_pEE->pDB->updateObject(*ci, 
					      m_update_spec->getObjectTypeName(), 
					      m_update_spec->getObjectTypeId(),
					      features))
			return false;

		// Add newline to result if this is not the first
		m_result->startNewRow();

		// Add id_d to result
		m_result->append(id_d2string(*ci));

		// Advance iterator
		++ci;
	}

	// If we got this far, there were no DB errors
	return true;
}


////////////////////////////////////////////////////////////
//
// UPDATE OBJECTS BY MONADS
//
// UpdateObjectsByMonadsStatement
//
////////////////////////////////////////////////////////////
UpdateObjectsByMonadsStatement::UpdateObjectsByMonadsStatement(
							       MQLExecEnv *pEE, 
							       MQLMonadSetElement* monad_set,
							       ObjectSpec* update_spec)
	: UpdateObjectsStatement(pEE, update_spec),
	  ByMonads(monad_set)
{
}

UpdateObjectsByMonadsStatement::~UpdateObjectsByMonadsStatement()
{
}

//
// - Check base class
//
// - Check that all monad ranges and monotonic
//
// - Check that all monad ranges are positive 
//
void UpdateObjectsByMonadsStatement::weed(bool& bResult)
{
	//
	// Check base class
	//
	UpdateObjectsStatement::weed(bResult);
	if (!bResult)
		return;

	//
	// Check that all monad ranges are positive and monotonic
	//
	ByMonads::myWeed(m_pEE, bResult);
	if (!bResult)
		return;

	// If we got this far, there were no compiler errors
	bResult = true;
	return;
}

bool UpdateObjectsByMonadsStatement::symbol(bool& bResult)
{
	//
	// - Check base class
	//
	if (!UpdateObjectsStatement::symbol(bResult))
		return false;
	if (!bResult)
		return true;

	// If we got this far, there were no DB errors and no compiler errors
	bResult = true;
	return true;
}

//
// - Check base class
//
bool UpdateObjectsByMonadsStatement::type(bool& bResult)
{
	//
	// - Check base class
	//
	if (!UpdateObjectsStatement::type(bResult))
		return false;
	if (!bResult)
		return true;

	// If we got this far, there were no DB errors and no compiler errors
	bResult = true;
	return true;
}

//
// - Build the set of monads
//
bool UpdateObjectsByMonadsStatement::monads(bool& bResult)
{
	//
	// - Build the set of monads
	//
	if (!ByMonads::myMonads(m_pEE, bResult))
		return false;
	if (!bResult)
		return true;

	// If we got this far, there were no compiler errors and no DB errors
	bResult = true;
	return true;
} 


//
// - Get the objects which are part_of m_monads
//
// - Call base class to execute
//
bool UpdateObjectsByMonadsStatement::exec()
{
	// Get the objects which are part_of m_monads
	if (!getObjectsByMonads(m_pEE, 
				m_update_spec->getObjectTypeName(), 
				m_update_spec->getObjectRangeType(),
				m_update_candidates))
		return false;

	// Call base class to execute
	if (!UpdateObjectsStatement::exec())
		return false;

	// If we got this far, there were no DB errors
	return true;
}



////////////////////////////////////////////////////////////
//
// UPDATE OBJECTS BY ID_DS
//
// UpdateObjectsByID_DsStatement
//
////////////////////////////////////////////////////////////
UpdateObjectsByID_DsStatement::UpdateObjectsByID_DsStatement(
							     MQLExecEnv *pEE, 
							     ID_D* id_d_specification,
							     ObjectSpec* update_spec)
	: UpdateObjectsStatement(pEE, update_spec),
	  m_id_d_list(id_d_specification)
{
}

UpdateObjectsByID_DsStatement::~UpdateObjectsByID_DsStatement()
{
	delete m_id_d_list;
}

//
// - Call base class
//
// - Check that none of the elements in the list are NIL
//
void UpdateObjectsByID_DsStatement::weed(bool& bResult)
{
	//
	// - Call base class
	//
	UpdateObjectsStatement::weed(bResult);
	if (!bResult)
		return;

	//
	// - Check that none of the elements in the list are NIL
	//
	m_id_d_list->weedNoneIsNIL(m_pEE, bResult);
	if (!bResult)
		return;

	// If we got this far, there were no compiler errors
	bResult = true;
	return;
}

//
// - Call base class
//
// - Check that all the objects exist
//
bool UpdateObjectsByID_DsStatement::symbol(bool& bResult)
{
	//
	// - Call base class
	//
	if (!UpdateObjectsStatement::symbol(bResult))
		return false;
	if (!bResult)
		return true;

	// Get object type name and object type id
	std::string object_type_name = m_update_spec->getObjectTypeName();
	id_d_t object_type_id = m_update_spec->getObjectTypeId();

	//
	// - Check that all the id_ds refer to objects that exist in the given type.
	//
	if (!m_id_d_list->symbolObjectsExistInType(m_pEE, 
						   object_type_name, 
						   object_type_id, 
						   bResult))
		return false;
	if (!bResult)
		return true;

	// If we got this far, there were no compiler errors and no DB errors
	bResult = true;
	return true;
}

//
// - Call base class
//
// - Check that all the objects are of the specified type
//
bool UpdateObjectsByID_DsStatement::type(bool& bResult)
{
	//
	// - Call base class
	//
	if (!UpdateObjectsStatement::type(bResult))
		return false;
	if (!bResult)
		return true;

	//
	// - Check that all the objects are of the specified type
	//
	id_d_t object_type_id = m_update_spec->getObjectTypeId();
	if (!m_id_d_list->symbolAllAreOfSameType(m_pEE, object_type_id, bResult))
		return false;
	if (!bResult)
		return true;

	// If we got this far, there were no compiler errors and no DB errors
	bResult = true;
	return true;
}

bool UpdateObjectsByID_DsStatement::exec()
{
	// Get id_ds
	m_id_d_list->execMakeList(m_update_candidates);

	// Call base class to execute
	if (!UpdateObjectsStatement::exec())
		return false;

	// If we got this far, there were no DB errors
	return true;
}



////////////////////////////////////////////////////////////
//
// UPDATE OBJECTS BY (focus|all|) QUERY
//
// UpdateObjectsByQueryStatement
//
////////////////////////////////////////////////////////////
UpdateObjectsByQueryStatement::UpdateObjectsByQueryStatement(
							     MQLExecEnv *pEE, 
							     eFocusSpec focus, 
							     ObjectSpec* update_spec,
							     MQLMonadSetElement* in_spec, 
							     MaxRange *max_range,
							     sheaf_return_type_pair *sheaf_return_type,
							     Topograph* query)
	: UpdateObjectsStatement(pEE, update_spec),
	  QueryBase(focus, in_spec, max_range, sheaf_return_type, query)
{
}

UpdateObjectsByQueryStatement::~UpdateObjectsByQueryStatement()
{
}

void UpdateObjectsByQueryStatement::weed(bool& bResult)
{
	// TODO: Implement
	bResult = false;
	m_pEE->pError->appendError("Not implemented yet.\n");
	return;
}

bool UpdateObjectsByQueryStatement::symbol(bool& bResult)
{
	// TODO: Implement
	bResult = false;
	m_pEE->pError->appendError("Not implemented yet.\n");
	return false;
}

bool UpdateObjectsByQueryStatement::type(bool& bResult)
{
	// TODO: Implement
	bResult = false;
	m_pEE->pError->appendError("Not implemented yet.\n");
	return false;
}

bool UpdateObjectsByQueryStatement::exec()
{
	// TODO: Implement
	m_pEE->pError->appendError("Not implemented yet.\n");
	return false;
}




////////////////////////////////////////////////////////////
//
// DeleteObjectsStatement
//
// Base class for all DELETE OBJECTS statements
//
////////////////////////////////////////////////////////////
DeleteObjectsStatement::DeleteObjectsStatement(MQLExecEnv *pEE, const std::string& object_type_name)
	:  ObjectTypesStatement(pEE, object_type_name)
{
}

DeleteObjectsStatement::~DeleteObjectsStatement()
{
}

bool DeleteObjectsStatement::exec()
{
	// Is result there already?
	if (m_result == 0) {
		// Create result
		m_result = new MQLResult();

		// Create header
		m_result->appendHeader("id_d", kTCID_D);
	}

	// Call base class
	return ObjectTypesStatement::exec();
}


bool DeleteObjectsStatement::execOneType(const ObjectTypeInfo& oti)
{
	// Get object type and id_d
	std::string object_type_name = oti.getObjectTypeName();
	id_d_t object_type_id = oti.getObjectTypeId();
	eObjectRangeType objectRangeType = oti.getObjectRangeType();

	// Iterate through candidates, making result along the way
	std::list<id_d_t>::const_iterator ci(m_delete_candidates.begin());
	std::list<id_d_t>::const_iterator cend(m_delete_candidates.end());
	while (ci != cend) {
		// Update object
		if (!m_pEE->pDB->dropObject(*ci, 
					    object_type_id, 
					    object_type_name, 
					    objectRangeType))
			return false;

		// Add newline to result if this is not the first
		m_result->startNewRow();

		// Add id_d to result
		m_result->append(id_d2string(*ci));

		// Advance iterator
		++ci;
	}

	// If we got this far, there were no DB errors
	return true;
}



////////////////////////////////////////////////////////////
//
// DELETE OBJECTS BY MONADS
//
// DeleteObjectsByMonadsStatement
//
////////////////////////////////////////////////////////////
DeleteObjectsByMonadsStatement::DeleteObjectsByMonadsStatement(
							       MQLExecEnv *pEE, 
							       MQLMonadSetElement* monad_set,
							       const std::string& object_type_name)
	: DeleteObjectsStatement(pEE, object_type_name),
	  ByMonads(monad_set)
{
}

DeleteObjectsByMonadsStatement::~DeleteObjectsByMonadsStatement()
{
}

//
// Check that all monad ranges are positive and monotonic
//
void DeleteObjectsByMonadsStatement::weed(bool& bResult)
{
	//
	// Check that all monad ranges are positive and monotonic
	//
	ByMonads::myWeed(m_pEE, bResult);
	if (!bResult)
		return;

	// If we got this far, there were no compiler errors
	bResult = true;
	return;
}

//
// - Build the set of monads
//
bool DeleteObjectsByMonadsStatement::monads(bool& bResult)
{
	//
	// - Build the set of monads
	//
	if (!ByMonads::myMonads(m_pEE, bResult))
		return false;
	if (!bResult)
		return true;

	// If we got this far, there were no compiler errors and no DB errors
	bResult = true;
	return true;
} 

bool DeleteObjectsByMonadsStatement::execOneType(const ObjectTypeInfo& oti) 
{
	// Make sure we are pristine
	m_delete_candidates.clear();

	// Get object type and id_d
	std::string object_type_name = oti.getObjectTypeName();
	id_d_t object_type_id = oti.getObjectTypeId();
	eObjectRangeType objectRangeType = oti.getObjectRangeType();

	// Delete
	if (!m_pEE->pDB->dropObjectsInMonads(m_monads, 
					     object_type_name, 
					     object_type_id, 
					     objectRangeType,
					     m_delete_candidates)) {
		return false;
	}

	// Iterate through candidates, making result along the way
	std::list<id_d_t>::const_iterator ci(m_delete_candidates.begin());
	std::list<id_d_t>::const_iterator cend(m_delete_candidates.end());
	while (ci != cend) {
		// Start new row
		m_result->startNewRow();

		// Add id_d to result
		m_result->append(id_d2string(*ci));

		// Advance iterator
		++ci;
	}

	std::cerr << "Done!" << std::endl;

	// If we got this far, there were no DB errors
	return true;
}

bool DeleteObjectsByMonadsStatement::exec()
{
	bool bDoCommit;
	bDoCommit = m_pEE->pDB->beginTransaction();

	// Call base class
	if (!DeleteObjectsStatement::exec()) {
		if (bDoCommit)
			m_pEE->pDB->abortTransaction();
		if (m_result != 0) {
			m_result->clear();
		}
		return false;
	}

	// Commit transaction
	if (bDoCommit) {
		if (!m_pEE->pDB->commitTransaction()) {
			m_pEE->pError->appendError("DeleteObjectsByMonadsStatement::exec: Could not commit transaction.");
			return false;
		}
	}

	// If we got this far, there were no DB errors
	return true;
}



////////////////////////////////////////////////////////////
//
// DELETE OBJECTS BY ID_DS
//
// DeleteObjectsByID_DsStatement
//
////////////////////////////////////////////////////////////
DeleteObjectsByID_DsStatement::DeleteObjectsByID_DsStatement(
							     MQLExecEnv *pEE,
							     ID_D* id_d_specification,
							     const std::string& object_type_name)
	: DeleteObjectsStatement(pEE, object_type_name),
	  m_id_d_list(id_d_specification)
{
}

DeleteObjectsByID_DsStatement::~DeleteObjectsByID_DsStatement()
{
	delete m_id_d_list;
}


//
// Check that object type name is not "all"
void DeleteObjectsByID_DsStatement::weed(bool& bResult)
{
	// Cannot do "all"
	if (strcmp_nocase(m_object_type_name, "all") == 0) {
		m_pEE->pError->appendError("Cannot use 'all' as object type in DELETE OBJECTS BY ID_DS statement.\n");
		bResult = false;
	} else {
		// None may be NIL
		m_id_d_list->weedNoneIsNIL(m_pEE, bResult);

		// Was this OK?
		if (!bResult) {
			// No. Return.
			return;
		} else {
			// Yes.
			// If we got this far, there were no weed errors
			bResult = true;
		}
	}
	return;
}


//
// - Check base class
//
// - Check that all the id_ds refer to objects that exist.
//
// - Check that all the objects are of the given type.
//
bool DeleteObjectsByID_DsStatement::symbol(bool& bResult)
{
	//
	//
	// - Check base class
	//
	if (!DeleteObjectsStatement::symbol(bResult))
		return false;
	if (!bResult)
		return true;

	//
	// - Check that all the id_ds refer to objects that exist in the given type.
	//
	std::string object_type_name = m_object_type_list.begin()->getObjectTypeName();
	id_d_t object_type_id = m_object_type_list.begin()->getObjectTypeId();

	if (!m_id_d_list->symbolObjectsExistInType(m_pEE, 
						   object_type_name, 
						   object_type_id, 
						   bResult))
		return false;
	if (!bResult)
		return true;

	// If we got this far, there were no DB errors and no compiler errors
	bResult = true;
	return true;
}

bool DeleteObjectsByID_DsStatement::exec()
{
	// Get id_ds
	m_id_d_list->execMakeList(m_delete_candidates);

	// Call base class to execute
	if (!DeleteObjectsStatement::exec())
		return false;

	// If we got this far, there were no DB errors
	return true;
}



////////////////////////////////////////////////////////////
//
// DELETE OBJECTS BY (focus|all|) QUERY
//
// DeleteObjectsByQueryStatement
//
////////////////////////////////////////////////////////////
DeleteObjectsByQueryStatement::DeleteObjectsByQueryStatement(
							     MQLExecEnv *pEE,
							     eFocusSpec focus, 
							     const std::string& object_type_name,
							     MQLMonadSetElement* in_spec, 
							     MaxRange *max_range,
							     sheaf_return_type_pair *sheaf_return_type,
							     Topograph* query)
	: DeleteObjectsStatement(pEE, object_type_name),
	  QueryBase(focus, in_spec, max_range, sheaf_return_type, query)
{
}

DeleteObjectsByQueryStatement::~DeleteObjectsByQueryStatement()
{
}

void DeleteObjectsByQueryStatement::weed(bool& bResult)
{
	// TODO: Implement
	bResult = false;
	m_pEE->pError->appendError("Not implemented yet.\n");
	return;
}

bool DeleteObjectsByQueryStatement::symbol(bool& bResult)
{
	// TODO: Implement
	bResult = false;
	m_pEE->pError->appendError("Not implemented yet.\n");
	return false;
}

bool DeleteObjectsByQueryStatement::type(bool& bResult)
{
	// TODO: Implement
	bResult = false;
	m_pEE->pError->appendError("Not implemented yet.\n");
	return false;
}

bool DeleteObjectsByQueryStatement::exec()
{
	// TODO: Implement
	m_pEE->pError->appendError("Not implemented yet.\n");
	return false;
}



