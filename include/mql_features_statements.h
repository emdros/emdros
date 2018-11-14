/*
 * mql_features_statements.h
 *
 * Definitions of FEATURES statements for MQL
 *
 * Ulrik Petersen
 * Created: 11/22-2002
 * Last update: 11/14-2018
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



/**@file mql_features_statements.h
 *@brief Header file for MQL FEATURES statements (MQL layer)
 */


#ifndef MQL_FEATURES_STATEMENT__H__
#define MQL_FEATURES_STATEMENT__H__

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
	virtual void weed(bool& bResult);
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


#endif /* MQL_FEATURES_STATEMENT__H__ */
