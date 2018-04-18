/*
 * emdf_ffeatures.h
 *
 * Definitions of AST-classes for EMdF ffeatures
 *
 * Ulrik Petersen
 * Created: 7/19-2003
 * Last update: 11/27-2012
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2003-2012  Ulrik Sandborg-Petersen
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

/**@file emdf_ffeatures.h
 *@brief Header file for EMdF FFeatures expressions (EMdF layer)
 */



#ifndef EMDF_FFEATURES__H__
#define EMDF_FFEATURES__H__

#include "emdf_exception.h"


class EMdFValue; // Forward declaration
class EMdFDBDBError; // Forward declaration
class EMdFDB; // Forward declaration

class EMdFComparison {
 protected:
	friend class EMdFDB;
	// Members
	std::string m_feature_name;
	id_d_t m_feature_type_id;
	std::string m_object_type_name;
	id_d_t m_object_type_id;
	eComparisonOp m_comparison_op;
	EMdFValue *m_value;
	std::list<EnumConstInfo> m_in_enum_list;
	const IntegerList *m_in_integer_list; // This object does not own this IntegerList, so we must not delete it in the destructor.
 public:
	// Constructor for enum_feature IN (enum_const1, enum_const2)
	EMdFComparison(const std::string& feature_name,
		       id_d_t feature_type_id,
		       const std::string& object_type_name, 
		       id_d_t object_type_id, 
		       const std::list<EnumConstInfo>& in_enum_list);
	
	// Constructor for enum_feature IN (integer1, integer2)
	EMdFComparison(const std::string& feature_name,
		       id_d_t feature_type_id,
		       const std::string& object_type_name, 
		       id_d_t object_type_id, 
		       const IntegerList *pIn_integer_list);
	
	// Constructor for all others
	EMdFComparison(const std::string& feature_name,
		       id_d_t feature_type_id,
		       const std::string& object_type_name, 
		       id_d_t object_type_id, 
		       eComparisonOp comparison_op,
		       EMdFValue* value);
 private:
	// Leave unimplemented, so as to prevent the compiler from
	// generating crashing ones.
	EMdFComparison(const EMdFComparison& other);
	EMdFComparison& operator=(const EMdFComparison& other);
 public:
	
	virtual ~EMdFComparison();
	virtual std::string makeConstraints(EMdFDB *pDB) const;
};


class EMdFFFeatures; // Forward declaration

class EMdFFFactor {
 protected:
	friend class EMdFDB;
	// Members
	EMdFFFactor* m_ffactor;
	EMdFFFeatures* m_ffeatures;
	EMdFComparison* m_feature_comparison;
	// Constructors
 public:
	EMdFFFactor(EMdFFFactor* ffactor);
	EMdFFFactor(EMdFFFeatures* ffeatures);
	EMdFFFactor(EMdFComparison* feature_comparison);
 private:
	// Leave unimplemented, so as to prevent the compiler from
	// generating crashing ones.
	EMdFFFactor(const EMdFFFactor& other);
	EMdFFFactor& operator=(const EMdFFFactor& other);
 public:
	~EMdFFFactor();
	bool isNOT() const { return m_ffactor != 0; };
	bool isParenthesis() const { return m_ffeatures != 0; };
	bool isFeatureComparison() const { return m_feature_comparison != 0; };
	EMdFFFactor* getFFactor() { return m_ffactor; };
	EMdFFFeatures* getFFeatures() { return m_ffeatures; };
	EMdFComparison* getFeatureComparison() { return m_feature_comparison; };
	std::string makeConstraints(EMdFDB *pDB) const;
};

class EMdFFTerm {
 protected:
	friend class EMdFDB;
	// Members
	EMdFFFactor* m_ffactor;
	EMdFFTerm* m_fterm;
 public:
	// Constructors
	EMdFFTerm(EMdFFFactor* ffactor);
	EMdFFTerm(EMdFFTerm* fterm, EMdFFFactor* ffactor);
 private:
	// Leave unimplemented, so as to prevent the compiler from
	// generating crashing ones.
	EMdFFTerm(const EMdFFTerm& other);
	EMdFFTerm& operator=(const EMdFFTerm& other);
 public:
	~EMdFFTerm();
	bool isFFactor() const { return m_fterm == 0; };
	EMdFFFactor* getFFactor() { return m_ffactor; };
	EMdFFTerm* getFTerm() { return m_fterm; };
	std::string makeConstraints(EMdFDB *pDB) const;
};

class EMdFFFeatures {
 protected:
	friend class EMdFDB;
	// Members
	EMdFFTerm* m_fterm;
	EMdFFFeatures* m_ffeatures;
	std::string *m_constraints;
	// Constructors
 public:
	EMdFFFeatures(EMdFFTerm* fterm);
	EMdFFFeatures(EMdFFFeatures* ffeatures, EMdFFTerm* fterm);
 private:
	// Leave unimplemented, so as to prevent the compiler from
	// generating crashing ones.
	EMdFFFeatures(const EMdFFFeatures& other);
	EMdFFFeatures& operator=(const EMdFFFeatures& other);
 public:
	~EMdFFFeatures();
	bool isFTerm() const { return m_ffeatures == 0; };
	EMdFFTerm* getFTerm() { return m_fterm; };
	EMdFFFeatures* getFFeatures() { return m_ffeatures; };
	const std::string& getConstraints(EMdFDB *pDB);
	std::string makeConstraints(EMdFDB *pDB) const;
};


#endif /* EMDF_FFEATURES__H__ */
