/*
 * mql_object_type_statements.cpp
 *
 * MQL OBJECT TYPE statements
 *
 * Ulrik Petersen
 * Created: 3/1-2001
 * Last update: 2/1-2014
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2014  Ulrik Sandborg-Petersen
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

/**@file mql_object_type_statements.cpp
 *@brief Implementation of MQL OBJECT TYPE statements (MQL layer)
 */


#include <emdfdb.h>
#include <mql_object_type_statements.h>
#include <infos.h>
#include <list>


/////////////////////////////////////////////////////////////
//
// Base class for all OBJECT TYPE statements
//
// ObjectTypeStatement
//
/////////////////////////////////////////////////////////////
ObjectTypeStatement::~ObjectTypeStatement()
{
  delete m_object_type_name;
}

//
// Checks whether object type exists.
// Sets m_object_type_id if it does.
bool ObjectTypeStatement::symbolObjectTypeExists(bool& bResult)
{
	if (!m_pEE->pDB->objectTypeExists(*m_object_type_name,
					  bResult, m_object_type_id,
					  m_objectRangeType,
					  m_monadUniquenessType)) {
		return false;
	} else {
		return true;
	}
}


/////////////////////////////////////////////////////////////
//
// Base class for all statements that require one or
// all object types
//
// ObjectTypesStatement
//
/////////////////////////////////////////////////////////////

ObjectTypesStatement::~ObjectTypesStatement()
{
  // Nothing to do  
}


// If "all", retrieve all object types.
// If one object type, check that it exists
bool ObjectTypesStatement::symbol(bool& bResult)
{
  if (strcmp_nocase(m_object_type_name, "all") == 0) {
    // Get object types
    std::list<std::string> object_types;
    if (!m_pEE->pDB->getObjectTypes(object_types)) {
      return false;
    }

    // Iterate through object types
    std::list<std::string>::const_iterator ci(object_types.begin());
    std::list<std::string>::const_iterator cend(object_types.end());
    while (ci != cend) {
      if (!symbolObjectTypeExists(*ci, bResult))
	return false;

      if (!bResult) {
	      m_pEE->pError->appendError("Object type " + *ci + " does not exist.\n");
	return true;
      }

      // Advance iterator
      ++ci;
    }
  } else {
    if (!symbolObjectTypeExists(m_object_type_name, bResult))
      return false;

    if (!bResult) {
	    m_pEE->pError->appendError("Object type " + m_object_type_name + " does not exist.\n");
      return true;
    }
  }

  // If we got this far, there was no DB error and no Compiler error
  bResult = true;
  return true;
}

bool ObjectTypesStatement::symbolObjectTypeExists(const std::string& object_type_name, bool& bResult)
{
  // Set object type id
  id_d_t object_type_id;
  eObjectRangeType objectRangeType;
  eMonadUniquenessType monadUniquenessType;
  if (!m_pEE->pDB->objectTypeExists(object_type_name,
				    bResult,
				    object_type_id,
				    objectRangeType,
				    monadUniquenessType)) {
    // Return DB error
    return false;
  }

  // If it wasn't found, return no DB error, but compiler error
  if (!bResult)
    return true;

  // Append to list
  m_object_type_list.push_back(ObjectTypeInfo(object_type_name, object_type_id, objectRangeType));

  // If we got this far, there was no DB error and no compiler error
  return true;
}



bool ObjectTypesStatement::exec()
{
  std::list<ObjectTypeInfo>::const_iterator ci, cend;
  ci = m_object_type_list.begin();
  cend = m_object_type_list.end();
  while (ci != cend) {
    // Execute for one type
    if (!execOneType(*ci))
      return false;
    ++ci;
  }

  if (!m_pEE->updateAll_m_1()) {
    m_pEE->pError->appendError("Could not update all_m_1a.");
    return false;
  }

  // If we got this far, there was no DB error
  return true;
}








/////////////////////////////////////////////////////////////
//
// CREATE OBJECT TYPE
//
// CreateObjectTypeStatement 
//
/////////////////////////////////////////////////////////////
CreateObjectTypeStatement::CreateObjectTypeStatement(
           MQLExecEnv *pEE,
           std::string* object_type_name, 
	   eObjectRangeType objectRangeType,
	   eMonadUniquenessType monadUniquenessType,
	   FeatureDeclaration* feature_declaration_list,
	   bool bIfNotExists)
  : ObjectTypeStatement(pEE, object_type_name)
{
  m_features = feature_declaration_list;
  m_objectRangeType = objectRangeType;
  m_monadUniquenessType = monadUniquenessType;
  m_bIfNotExists = bIfNotExists;
  m_bObjectTypeExistsAlready = false;
}

CreateObjectTypeStatement::~CreateObjectTypeStatement()
{
  delete m_features;
}

//
// - Check that "self" is not declared
//
// - Check that declarations of STRING and ASCII have lengths
//   within the prescribed limits.
//
void CreateObjectTypeStatement::weed(bool& bResult)
{
  if (m_features) {
    //
    // - Check that "self" is not declared
    //
    // - Check that declarations of STRING and ASCII have lengths
    //   within the prescribed limits.
    //
    m_features->weed(m_pEE, bResult);
    if (!bResult)
      return;
  }
  else
    bResult = true;
  return;
}

//
// - Check that the object type does not exist
//
// - Check that all the object types in covered_by and
//   buildable_from do exist.
//
// - Check that the enumerations exist for the types 
//   whose features are enumerations
//
// - Check that, within these enumerations, any default
//   specification which is an enumeration constant exists
//   in that enumeration.
//
bool CreateObjectTypeStatement::symbol(bool& bResult)
{
  // Check that object type does not exist
  if (!symbolObjectTypeExists(m_bObjectTypeExistsAlready))
    return false;

  if (!m_bIfNotExists) {
	  if (m_bObjectTypeExistsAlready) {
		  m_pEE->pError->appendError("Object type " + *m_object_type_name + " exists already.\n");
		  bResult = false;
		  return true;
	  }
  }

  if (m_features != 0) {
    //
    // - Check that the enumerations exist for the types   
    //   whose features are enumerations
    if (!m_features->symbolEnumerationsExist(m_pEE, bResult))
      return false;
    if (!bResult) 
      return true;


    // - Check that, within these enumerations, any default
    //   specification which is an enumeration constant exists
    //   in that enumeration.
    if (!m_features->symbolEnumConstantsExist(m_pEE, bResult))
      return false;
    if (!bResult)
      return true;
  }


  // Set default
  bResult = true;
  return true;
}

//
// - Assign type-id to each feature.
//
// - Check that there is type-compatibility between the feature
//   and the default specification
//
bool CreateObjectTypeStatement::type(bool& bResult)
{
  //
  // - Assign type-id to each feature.
  //
  if (m_features != 0 && !m_features->typeAssignTypeID(m_pEE)) {
    m_pEE->pError->appendError("Could not assign type id\n");
    return false;
  }

  //
  // - Check that there is type-compatibility between the feature
  //   and the default specification, and make a default specification
  //   if it does not exist.
  //
  if (m_features != 0 && !m_features->typeTypeCompatibility(m_pEE, bResult)) {
    m_pEE->pError->appendError("Checking type-compatibility failed.\n");
    return false;
  }
  if (!bResult)
    return true;

  // If we got this far, we were a success regardless of bResult's value
  bResult = true;
  return true;
}

bool CreateObjectTypeStatement::exec()
{
	// Do we need to check whether it doesn't exist already?
	if (m_bIfNotExists) {
		// Yes. If it does, return success now, before
		// trying to create.
		if (m_bObjectTypeExistsAlready) {
			return true;
		}
	}
	// Build list of feature infos
	std::list<FeatureInfo> FeatureInfos;
	FeatureDeclaration* f = m_features;
	while (f != 0) {
		// Get feature info from f
		FeatureInfo fi;
		if (!f->getFeatureInfo(m_pEE, fi))
			return false;
		
		// Add to list
		FeatureInfos.push_back(fi);
		
		// Advance iterator
		f = f->getNext();
	}
	
	// Create object type in DB
	id_d_t object_type_id; // Dummy variable
	if (!m_pEE->pDB->createObjectType(*m_object_type_name,
					  FeatureInfos,
					  m_objectRangeType,
					  m_monadUniquenessType,
				    object_type_id))
		return false;
	
	// If we got this far, there were no errors
	return true;
}



/////////////////////////////////////////////////////////////
//
// UPDATE OBJECT TYPE
//
// UpdateObjectTypeStatement 
//
/////////////////////////////////////////////////////////////
UpdateObjectTypeStatement::UpdateObjectTypeStatement(
           MQLExecEnv *pEE,
           std::string* object_type_name, 
	   FeatureUpdate* feature_update_list)
  : ObjectTypeStatement(pEE, object_type_name)
{
  m_features = feature_update_list;
}

UpdateObjectTypeStatement::~UpdateObjectTypeStatement()
{
  delete m_features;
}

//
// - Check that "self" is not added or removed
//
// - Check that declarations of STRING and ASCII have lengths
//   within the prescribed limits.
//
void UpdateObjectTypeStatement::weed(bool& bResult)
{
  m_features->weed(m_pEE, bResult);
  return;
}

//
// - Check that the object type already exists.  Store the object 
//   type_id locally, as well as whether it is a WITH SINGLE RANGE
//   OBJECTS object type.
//
// - Check that all the features that are to be removed do exist.
//
// - Check that all the features that are to be added do not exist.
//
// - Check that the enumerations exist for the types 
//   whose features are enumerations for features that are to be
//   added.
//
// - Check that, within these enumerations, any default
//   specification which is an enumeration constant exists
//   in that enumeration for features that are to be added.
//
bool UpdateObjectTypeStatement::symbol(bool& bResult)
{
  //
  // - Check that the object type already exists.  Store the object 
  //   type_id locally, as well as whether it is a WITH SINGLE RANGE
  //   OBJECTS object type.
  if (!symbolObjectTypeExists(bResult))
    return false;
  if (!bResult) {
	  m_pEE->pError->appendError("Object type " + *m_object_type_name + " does not exist.\n");
    return true;
  }

  // 
  // - Check that all the features that are to be removed do exist.
  //   
  // - Check that all the features that are to be added do not exist.
  if (!m_features->symbolExistenceOfFeatures(m_pEE, bResult, m_object_type_id))
    return false;
  if (!bResult)
    return true;

  //
  // - Check that the enumerations exist for the types   
  //   whose features are enumerations
  //
  // - Check that, within these enumerations, any default
  //   specification which is an enumeration constant exists
  //   in that enumeration.
  if (!m_features->symbolEnumerations(m_pEE, bResult))
    return false;
  if (!bResult)
    return true;

  bResult = true;
  return true;
}

//
// - Assign type-id to each feature.
//
// - Check that there is type-compatibility between the feature
//   and the default specification
//
bool UpdateObjectTypeStatement::type(bool& bResult)
{
  //
  // - Assign type-id to each feature.
  //
  if (!m_features->typeAssignTypeID(m_pEE))
    return false;

  //
  // - Check that there is type-compatibility between the feature
  //   and the default specification
  //
  if (!m_features->typeTypeCompatibility(m_pEE, bResult))
    return false;

  // If we got this far, we were a success regardless of bResult's value
  return true;
}


bool UpdateObjectTypeStatement::exec()
{
  // Start transaction
  bool bDoCommit;
  bDoCommit = m_pEE->pDB->beginTransaction();

  // Add and remove the features 
  if (!m_features->exec(m_pEE, *m_object_type_name, m_object_type_id)) {
    if (bDoCommit)
      m_pEE->pDB->abortTransaction();
    return false;
  }

  // Commit transaction
  if (bDoCommit) {
    if (!m_pEE->pDB->commitTransaction()) {
      m_pEE->pError->appendError("UpdateObjectTypeStatement::exec: Could not commit transaction.");
      return false;
    }
  }
  return true;
}



/////////////////////////////////////////////////////////////
//
// DROP OBJECT TYPE
//
// DropObjectTypeStatement 
//
////////////////////////////////////////////////////////////
DropObjectTypeStatement::DropObjectTypeStatement(MQLExecEnv *pEE, std::string* object_type_name) : ObjectTypeStatement(pEE, object_type_name)
{
}

bool DropObjectTypeStatement::symbol(bool& bResult)
{
  //
  // - Check that the object type already exists.  Store the object 
  //   type_id locally, as well as whether it is a WITH SINGLE RANGE
  //   OBJECTS object type.
  if (!symbolObjectTypeExists(bResult))
    return false;

  if (!bResult) {
	  m_pEE->pError->appendError("Object type " + *m_object_type_name + " does not exist.\n");

  }

  // In any case, if we got this far, we succeeded.
  return true;
}

bool DropObjectTypeStatement::exec()
{
  // Drop object type in DB
  if (!m_pEE->pDB->dropObjectType(*m_object_type_name,
				  m_object_type_id))
    return false;

  // NOTE: We should not remove it from m_pEE 
  // when dropping an object type name.
  // This is because m_pEE's list of object types should be strictly
  // monotonic, i.e., one should only add to it.
  // Otherwise, separate queries might get different object type name
  // results, which is possible if you call 
  // EmdrosEnv::take_over_statement().

  // If we got this far, there were no errors
  return true;
}



