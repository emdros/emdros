/*
 * mql_features_statements.cpp
 *
 * MQL FEATURES statements
 *
 * Ulrik Petersen
 * Created: 3/6-2001 (March 6, 2001)
 * Last update: 10/21-2013
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2013  Ulrik Sandborg-Petersen
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

/**@file mql_features_statements.cpp
 *@brief Implementation of MQL FEATURES statements (MQL layer)
 */


#include <emdfdb.h>
#include <mql_features_statements.h>
#include <string_func.h>


/////////////////////////////////////////////////////////////
//
// GET FEATURES
//
// GetFeaturesStatement
//
////////////////////////////////////////////////////////////
GetFeaturesStatement::GetFeaturesStatement(MQLExecEnv *pEE, 
					   GrammarFeature* feature_list,
					   ID_D* id_ds,
					   std::string* object_type_name)
  : Statement(pEE),
    m_id_ds(id_ds),
    m_object_type_name(object_type_name),
    m_object_type_id(NIL)
{
  if (feature_list != 0) {
    m_features = Feature::FromGrammarFeature(feature_list);
    delete feature_list;
  }
}

GetFeaturesStatement::~GetFeaturesStatement()
{
  delete m_features;
  delete m_object_type_name;
  delete m_id_ds;
}

//
// - Check that the object type exists. In doing so, store its object
//   type_id in the AST.
//
// - Check that the objects with the given id_ds exists.
//
// - Check that the objects all belong to the same type, 
//   namely the one given.
//
// - Check that the features exist for the given object type.
//
bool GetFeaturesStatement::symbol(bool& bResult)
{
  //
  // - Check that the object type exists. In doing so, store its object
  //   type_id in the AST.
  //
  eMonadUniquenessType dummyMonadUniquenessType;
  if (!m_pEE->pDB->objectTypeExists(*m_object_type_name, 
				    bResult, m_object_type_id,
				    m_objectRangeType,
				    dummyMonadUniquenessType))
    return false;
  if (!bResult) {
    m_pEE->pError->appendError("Object type name '" + *m_object_type_name + "' does not exist.\n");
    return true;
  }

  //
  // - Check that all the id_ds refer to objects that exist in the given type.
  //
  // 2004-01-13 UP: Commented out because it was prohibitively expensive
  /*
  if (!m_id_ds->symbolObjectsExistInType(m_pEE, *m_object_type_name, m_object_type_id, bResult))
    return false;
  if (!bResult)
    return true;
  */

  //
  // - Check that the features exist for the given object type.
  //
  if (!m_features->symbolFeaturesExist(m_pEE, m_object_type_id, bResult))
    return false;
  if (!bResult)
    return true;

  // If we got this far, there were no DB errors and no compiler errors
  bResult = true;
  return true;
}

bool GetFeaturesStatement::type(bool& bResult)
{
  //
  // - Get feature name and type
  //
  if (!m_features->typeFeatureName(m_pEE, bResult))
    return false;
  if (!bResult)
    return true;

  // If we got this far, there were no DB errors and no compiler errors
  bResult = true;
  return true;
}

bool GetFeaturesStatement::exec()
{
  // Create result
  m_result = new MQLResult();

  // Add headers
  m_result->appendHeader("id_d", kTCID_D);
  m_features->execMakeNameList(m_result);

  // Get a list of FeatureInfos
  std::list<FeatureInfo> FeatureInfos;
  m_features->execMakeFeatureList(FeatureInfos);

  // Get a list of the id_ds
  // std::list<id_d_t> id_ds;
  // m_id_ds->execMakeList(id_ds);

  // Get a SOM with the id_ds
  SetOfMonads so_id_ds;
  m_id_ds->execMakeSOM(so_id_ds);

  // Ask the database
  std::list<std::list<std::string> > results;
  if (!m_pEE->pDB->getFeatures(*m_object_type_name,
			       m_object_type_id,
			       m_objectRangeType,
			       FeatureInfos,
			       so_id_ds,
			       results))
    return false;
  
  // Add the results
  std::list<std::list<std::string> >::const_iterator ci(results.begin());
  std::list<std::list<std::string> >::const_iterator cend(results.end());
  while (ci != cend) {
    // Append result
    m_result->append(*ci);

    // Advance iterator
    ++ci;
  }
  
  // If we got this far, there were no DB errors
  return true;
}




/////////////////////////////////////////////////////////////
//
// GET SET FROM FEATURE
//
// GetSetFromFeatureStatement
//
////////////////////////////////////////////////////////////
GetSetFromFeatureStatement::GetSetFromFeatureStatement(MQLExecEnv *pEE, 
						       std::string *pObjectTypeName,
						       std::string *pFeatureName)
  : ObjectTypeStatement(pEE, pObjectTypeName),
    m_feature_name(pFeatureName)
{
}

GetSetFromFeatureStatement::~GetSetFromFeatureStatement()
{
  delete m_feature_name;
}

//
// - Check that the object type exists. In doing so, store its object
//   type_id in the AST.
//
// - Check that the feature exists for the given object type.
//
bool GetSetFromFeatureStatement::symbol(bool& bResult)
{
  //
  // - Call base class to chec hat the object type exists. In doing
  //   so, store its object type_id in the AST.
  //
  if (!symbolObjectTypeExists(bResult))
    return false;
  if (!bResult) {
    m_pEE->pError->appendError("Object type name '" + *m_object_type_name + "' does not exist.\n");
    return true;
  }

  //
  // - Check that the features exist for the given object type.
  //
  std::string dummyDefaultValue;
  bool dummyIsComputed;

  if (!m_pEE->pDB->featureExists(*m_feature_name,
				 m_object_type_id,
				 bResult,
				 m_feature_type_id,
				 dummyDefaultValue,
				 dummyIsComputed))
    return false;
  if (!bResult) {
    m_pEE->pError->appendError("The feature name '" + *m_feature_name + "' does not exist on object type '" + *m_object_type_name + "'.\n");
    return true;
  }

  // If we got this far, there were no DB errors and no compiler errors
  bResult = true;
  return true;
}

//
// - Check that the feature is indeed a FROM SET feature.
//
bool GetSetFromFeatureStatement::type(bool& bResult)
{
  bResult = featureTypeIdIsFromSet(m_feature_type_id);
  if (!bResult) {
    m_pEE->pError->appendError("The feature name '" + *m_feature_name + "' on object type '" + *m_object_type_name + "' was not declared FROM SET.  Thus you cannot get any set from it.\n");
    return true;
  }

  // If we got this far, there were no DB errors and no compiler errors
  bResult = true;
  return true;
}


bool GetSetFromFeatureStatement::exec()
{
  // Create result
  m_result = new MQLResult();

  // Add headers
  m_result->appendHeader("value", kTCString);

  // Ask the database
  if (!m_pEE->pDB->getStringSetStrings(*m_object_type_name,
				       m_object_type_id,
				       *m_feature_name,
				       *m_result)) {
    m_result->clear();
    return false;
  }
  
  // If we got this far, there were no DB errors
  return true;
}


