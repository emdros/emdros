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

/*
 * Copyright (C) 2001-2018     Ulrik Sandborg-Petersen
 * Copyright (C) 2018-present  Sandborg-Petersen Holding ApS, Denmark
 *
 * Licensed under the MIT License.
 *
 * Please see the file COPYING in the root of the sources for more details.
 *
 */


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
