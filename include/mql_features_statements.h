/*
 * mql_features_statements.h
 *
 * Definitions of FEATURES statements for MQL
 *
 * Ulrik Petersen
 * Created: 11/22-2002
 * Last update: 10/19-2013
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2002-2013  Ulrik Sandborg-Petersen
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


/**@file mql_features_statements.h
 *@brief Header file for MQL FEATURES statements (MQL layer)
 */


#ifndef MQL_XXX__H__
#define MQL_XXX__H__

#include "mql_types.h"
#include "mql_execution_environment.h"
#include "mql_object_type_statements.h"

// GET FEATURES
class GetFeaturesStatement : public Statement {
private:
  ID_D *m_id_ds;
  Feature *m_features;
  std::string *m_object_type_name;
  id_d_t m_object_type_id;
  eObjectRangeType m_objectRangeType;
public:
  GetFeaturesStatement(MQLExecEnv *pEE, 
		       GrammarFeature* feature_list,
		       ID_D* id_ds,
		       std::string* object_type_name);
  virtual ~GetFeaturesStatement();
  virtual bool symbol(bool& bResult);
  virtual bool type(bool& bResult);
  virtual bool exec();
};


// GET FEATURES
class GetSetFromFeatureStatement : public ObjectTypeStatement {
private:
  std::string *m_feature_name;
  id_d_t m_feature_type_id;
public:
  GetSetFromFeatureStatement(MQLExecEnv *pEE, 
			     std::string *pObjectTypeName,
			     std::string *pFeatureName);
  virtual ~GetSetFromFeatureStatement();
  virtual bool symbol(bool& bResult);
  virtual bool type(bool& bResult);
  virtual bool exec();
};


#endif /* MQL_XXX__H__ */
