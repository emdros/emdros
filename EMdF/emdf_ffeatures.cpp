/*
 * emdf_ffeatures.cpp
 *
 * Definitions of AST-classes for EMdF ffeatures
 *
 * Ulrik Petersen
 * Created: 7/19-2003
 * Last update: 5/28-2018
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2003-2018  Ulrik Sandborg-Petersen
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

/**@file emdf_ffeatures.cpp
 *@brief Implementation of EMdF FFeatures expressions (EMdF layer)
 */


#include <string>
#include <infos.h>
#include <emdf_value.h>
#include <emdf_ffeatures.h>
#include <string_func.h>
#include <string_list.h>
#include <emdfdb.h>
#include <debug.h>


////////////////////////////////////////////////////////////
//
// EMdFComparison 
//
////////////////////////////////////////////////////////////
EMdFComparison::EMdFComparison(const std::string& feature_name,
			       id_d_t feature_type_id,
			       const std::string& object_type_name,
			       id_d_t object_type_id,
			       eComparisonOp comparison_op,
			       EMdFValue* value)
	: m_feature_name(feature_name),
	  m_feature_type_id(feature_type_id),
	  m_object_type_id(object_type_id),
	  m_comparison_op(comparison_op),
	  m_value(value),
	  m_in_integer_list(0)
{
	m_object_type_name = normalizeOTName(object_type_name);
}

EMdFComparison::EMdFComparison(const std::string& feature_name,
			       id_d_t feature_type_id,
			       const std::string& object_type_name,
			       id_d_t object_type_id,
			       const std::list<EnumConstInfo>& in_enum_list)
	: m_feature_name(feature_name),
	  m_feature_type_id(feature_type_id),
	  m_object_type_id(object_type_id),
	  m_comparison_op(kIn),
	  m_value(0),
	  m_in_enum_list(in_enum_list),
	  m_in_integer_list(0)
{
	m_object_type_name = normalizeOTName(object_type_name);
}


EMdFComparison::EMdFComparison(const std::string& feature_name,
			       id_d_t feature_type_id,
			       const std::string& object_type_name,
			       id_d_t object_type_id,
			       const IntegerList *pIn_integer_list)
	: m_feature_name(feature_name),
	  m_feature_type_id(feature_type_id),
	  m_object_type_id(object_type_id),
	  m_comparison_op(kIn),
	  m_value(0),
	  m_in_integer_list(pIn_integer_list)
{
	m_object_type_name = normalizeOTName(object_type_name);
}


EMdFComparison::~EMdFComparison()
{
	delete m_value;
	// DON'T delete m_in_integer_list: We don't own the object here.
}

std::string EMdFComparison::makeConstraints(EMdFDB *pDB) const
{
	// Construct SQL feature name
	std::string SQL_feature_name;
	SQL_feature_name = EMdFDB::encodeFeatureNameForPrequeryString(m_feature_name);

	// Create feature comparison
	std::string result;
	result = SQL_feature_name;
	result += " ";
	result += pDB->eComparisonOp2BackendComparisonOp(m_comparison_op);
	result += " ";

	if (m_value != 0) {
		bool bIsComputed = false;
		std::string value_string = m_value->toString();
		if (featureTypeIdIsFromSet(m_feature_type_id)) {
			FeatureInfo myfi(m_feature_name, m_feature_type_id, value_string, bIsComputed);
			// The penultimate "false" on FeatureInfo2SQLvalue means that 
			// we must note create any IDD-String association in the 
			// OT_mdf_FEATURE_NAME_set table, but since this feature
			// is not a STRING or ASCII feature, it doesn't matter:
			// We won't call getID_DFromStringSet().
			result += pDB->FeatureInfo2SQLvalue(m_object_type_name, m_object_type_id, false, myfi);
		} else if (featureTypeIdIsSTRING(m_feature_type_id)
			   || featureTypeIdIsASCII(m_feature_type_id)) {
			value_string = pDB->escapeStringForSQL(value_string);
			result += pDB->getMYSQL_BINARY() + value_string;
		} else if (featureTypeIdIsListOf(m_feature_type_id)) {
			// This shouldn't be checked here: It is not checked
			// until type-checking time, which is after symbol-checking-time,
			// which is when this is called.
			// ASSERT_THROW(m_comparison_op == kHas, "Internal error: comparison operator should be HAS.");
			result += "'% " + value_string + " %'";
		} else { // INTEGER, ID_D, ENUM
			result += value_string; 
		}
		return result;
	} else if (m_in_integer_list != 0) {
		// Make sure we are doing an IN
		ASSERT_THROW(m_comparison_op == kIn, "m_comparison_op must be IN.");
		result += " (";
    
		IntegerListConstIterator ci = m_in_integer_list->const_iterator();
		// The grammar dictates that it is not empty, so we don't need
		// to check... otherwise, it would have been an error to do
		// as we do here, since it would result in "IN ()".
		while (ci.hasNext()) {
			long myint = ci.next();
			result += long2string(myint);
			if (ci.hasNext()) {
				result += ',';
			}
		}

		result += ')';
    
		return result;
	} else {
		// Make sure we are doing an IN
		ASSERT_THROW(m_comparison_op == kIn, "m_comparison_op must be IN.");
		result += " (";
    
		std::list<EnumConstInfo>::const_iterator ci;
		std::list<EnumConstInfo>::const_iterator cend = m_in_enum_list.end();
		for (ci = m_in_enum_list.begin(); ci != cend; ) {
			EnumConstInfo eci(*ci);
			long enum_const = eci.getValue();
			result += long2string(enum_const);
			++ci;
			if (ci != cend) {
				result += ',';
			}
		}
    
		result += ')';
    
		return result;
	}
}




////////////////////////////////////////////////////////////
//
// EMdFFFactor 
//
////////////////////////////////////////////////////////////
EMdFFFactor::EMdFFFactor(EMdFFFactor* ffactor)
	: m_ffactor(ffactor),
	  m_ffeatures(0),
	  m_feature_comparison(0)
{
}

EMdFFFactor::EMdFFFactor(EMdFFFeatures* ffeatures)
	: m_ffactor(0),
	  m_ffeatures(ffeatures),
	  m_feature_comparison(0)
{
}

EMdFFFactor::EMdFFFactor(EMdFComparison* feature_comparison)
	: m_ffactor(0),
	  m_ffeatures(0),
	  m_feature_comparison(feature_comparison)
{
}

EMdFFFactor::~EMdFFFactor()
{
	delete m_ffactor;
	delete m_ffeatures;
	delete m_feature_comparison;
}

std::string EMdFFFactor::makeConstraints(EMdFDB *pDB) const
{
	if (isNOT()) {
		std::string ffactor = m_ffactor->makeConstraints(pDB);
		return std::string("NOT (") + ffactor + std::string(")");
	} else if (isParenthesis()) {
		std::string ffeatures = m_ffeatures->makeConstraints(pDB);
		return std::string("(") + ffeatures + ")";
	} else { // is_feature_comparison
		ASSERT_THROW(isFeatureComparison(), "isFeatureComparison() must return true.");
		return m_feature_comparison->makeConstraints(pDB);
	}
}


////////////////////////////////////////////////////////////
//
// EMdFFTerm 
//
////////////////////////////////////////////////////////////
EMdFFTerm::EMdFFTerm(EMdFFFactor* ffactor)
	: m_ffactor(ffactor),
	  m_fterm(0)
{
}

EMdFFTerm::EMdFFTerm(EMdFFTerm* fterm, EMdFFFactor* ffactor)
	: m_ffactor(ffactor),
	  m_fterm(fterm)
{
}

EMdFFTerm::~EMdFFTerm()
{
	delete m_ffactor;
	delete m_fterm;
}

std::string EMdFFTerm::makeConstraints(EMdFDB *pDB) const
{
	if (isFFactor()) {
		return m_ffactor->makeConstraints(pDB);
	} else {
		ASSERT_THROW(m_fterm != 0, "Null-pointer violation.");
		ASSERT_THROW(m_ffactor != 0, "Null-pointer violation.");
		std::string A, B;
		A = m_fterm->makeConstraints(pDB);
		B = m_ffactor->makeConstraints(pDB);
		return A + " AND " + B;
	}
}


////////////////////////////////////////////////////////////
//
// EMdFFFeatures 
//
////////////////////////////////////////////////////////////
EMdFFFeatures::EMdFFFeatures(EMdFFTerm* fterm)
	: m_fterm(fterm),
	  m_ffeatures(0),
	  m_constraints(0)
{
}

EMdFFFeatures::EMdFFFeatures(EMdFFFeatures* ffeatures, EMdFFTerm* fterm)
	: m_fterm(fterm),
	  m_ffeatures(ffeatures),
	  m_constraints(0)
{
}

EMdFFFeatures::~EMdFFFeatures()
{
	delete m_fterm;
	delete m_ffeatures;
	delete m_constraints;
}

const std::string& EMdFFFeatures::getConstraints(EMdFDB *pDB)
{
	if (m_constraints == 0) {
		m_constraints = new std::string;
		*m_constraints = makeConstraints(pDB);
	} 
	return *m_constraints;
}

std::string EMdFFFeatures::makeConstraints(EMdFDB *pDB) const
{
	if (isFTerm()) {
		return m_fterm->makeConstraints(pDB);
	} else {
		ASSERT_THROW(m_fterm != 0, "Null-pointer violation.");
		std::string A, B;
		A = m_ffeatures->makeConstraints(pDB);
		B = m_fterm->makeConstraints(pDB);
		return A + " OR " + B;
	}
}



