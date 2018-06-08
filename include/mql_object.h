/*
 * mql_object.h
 *
 * MQL representation of an object while processing the query
 *
 * Ulrik Petersen
 * Created: 4/2-2001
 * Last update: 6/9-2018
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
	const EMdFValue* getFeatureValue(unsigned int index) const { 
		return m_feature_value_vec_inst[index];
	};
	
	short int getFeatureIndex(const std::string& feature_name) const;
	const std::vector<std::string>& getFeatureNames();
	const std::list<FeatureInfo>& getFeatureInfos() { 
		return m_feature_infos_inst; 
	};
};

#endif /* MQL_OBJECT__H__ */
