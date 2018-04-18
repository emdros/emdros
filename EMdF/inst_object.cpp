/*
 * inst_object.cpp
 *
 * InstObject class
 *
 * Ulrik Petersen
 * Created: 6/30-2001
 * Last update: 1/17-2013
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2013  Ulrik Petersen
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

/**@file inst_object.cpp
 *@brief Implementation of InstObject (EMdF layer)
 */


#ifdef __DJGPP__
#include <inst_o~1.h>
#else
#include <inst_object.h>
#endif
#include <debug.h>


InstObject::InstObject(id_d_t id_d, monad_m first_monad, monad_m last_monad, EMdFValue *feature_value_arr, int size_of_array)
	: m_id_d(id_d), m_first(first_monad),
	  m_feature_value_arr(feature_value_arr),
	  m_feature_value_arr_size(size_of_array)
{
	m_feature_value_arr_size |= union_is_mlast;
	m_u.last = last_monad;
}


// If this constructor is invoked, and bMustDeleteFeatureArray
// is true, then feature_value_arr
// will be delete[]'ed in the destructor.
// If bMustDeleteFeatureArray is false,
// then feature_value_arr will not be delete[]'ed.
// This constructor is used in ObjectSpecNoOT::exec() 
// in the MQL library with bMustDeleteFeatureArray set to true.
// It is used in EMdFDB::getInst() with the boolean
// set to false.
InstObject::InstObject(id_d_t id_d, SetOfMonads* pSOM, EMdFValue *feature_value_arr, int size_of_array, bool bMustDeleteFeatureArray)
	: m_id_d(id_d), m_first(pSOM->first()),
	  m_feature_value_arr(feature_value_arr),
	  m_feature_value_arr_size(size_of_array)
{
	if (bMustDeleteFeatureArray) {
		m_feature_value_arr_size |=
			union_is_pMonads | must_delete_feature_array;
	} else {
		m_feature_value_arr_size |= union_is_pMonads;
	}
	m_u.pMonad_ms = pSOM;
}

InstObject::~InstObject()
{
	// If this was created with the SetOfMonads* constructor,
	// we must delete[] the feature value array.
	// Otherwise, we must call each destructor individually
	if (m_feature_value_arr_size & must_delete_feature_array) {
		delete[] m_feature_value_arr;
	} else {
		int feature_value_arr_size = getFeatureValueArrSize();
		for (int i = 0; i < feature_value_arr_size; ++i) {
			m_feature_value_arr[i].~EMdFValue();
		}
	}

	// If this was created with the non-SetOfMonads* 
	// constructor, yet we have a monad set to delete,
	// then delete it.  Also delete it if this was
	// created with the SetOfMonads* constructor.
	if (m_feature_value_arr_size & union_is_pMonads) {
		delete m_u.pMonad_ms;
	} 
}

const EMdFValue* InstObject::getFeature(unsigned int index) const
{
	ASSERT_THROW(m_feature_value_arr != 0, "feature_val_arr is 0");
	return &(m_feature_value_arr[index]);
}

void InstObject::addMse(monad_m first_monad, monad_m last_monad)
{
	if (m_feature_value_arr_size & union_is_mlast) {
		m_u.pMonad_ms = new SetOfMonads(m_first, m_u.last);
		m_u.pMonad_ms->add(first_monad, last_monad);
		m_feature_value_arr_size = 
			(m_feature_value_arr_size & size_mask)
			| union_is_pMonads;
		if (m_first > first_monad) {
			m_first = first_monad;
		}
	} else {
		m_u.pMonad_ms->add(first_monad, last_monad);
		if (m_first > first_monad) {
			m_first = first_monad;
		}
	}
}

bool InstObject::somHasOnlyOneMSE() const
{
	if (m_feature_value_arr_size & union_is_pMonads) {
		return m_u.pMonad_ms->hasOnlyOneMSE();
	} else {
		return true;
	}
}



// result must be empty upon calling this.
void InstObject::getMonads(SetOfMonads &result) const
{
	if (m_feature_value_arr_size & union_is_pMonads) {
		result = *m_u.pMonad_ms;
	} else {
		result.add(m_first, m_u.last);
	}
}


void InstObject::changeString(unsigned int index, const std::string& newString)
{
	ASSERT_THROW(m_feature_value_arr != 0, "feature_val_arr is 0");
	m_feature_value_arr[index].changeString(newString);
}
