/*
 * mql_object.h
 *
 * MQL representation of an object while processing the query
 *
 * Ulrik Petersen
 * Created: 4/2-2001
 * Last update: 1/9-2009
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


/**@file mql_object.h
 *@brief Header file for MQLObject (MQL layer)
 */



#ifndef MQL_OBJECT__H__
#define MQL_OBJECT__H__

#include <string>
#include <map>
#include <list>
#include <vector>
#include "emdf.h"
#include "emdf_value.h"
#include "inst_object.h"
#include "mql_enums.h"


class EMdFDB; // Forward declaration

class MQLObject {
private:
	// Inst
	std::vector<EMdFValue*> m_feature_value_vec_inst;
	std::list<FeatureInfo> m_feature_infos_inst;
	std::vector<std::string> *m_feature_name_vec_inst;
 public:
	MQLObject();
	~MQLObject();
	void addFeature(const FeatureInfo& fi);
	void retrieveFeatureValues(const InstObject *pOther);
	const EMdFValue* getFeatureValue(short int index) const { 
		return m_feature_value_vec_inst[index];
	};
	
	short int getFeatureIndex(const std::string& feature_name) const;
	const std::vector<std::string>& getFeatureNames();
	const std::list<FeatureInfo>& getFeatureInfos() { 
		return m_feature_infos_inst; 
	};
};

#endif /* MQL_OBJECT__H__ */
