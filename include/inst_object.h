/*
 * inst_object.h
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


/**@file inst_object.h
 *@brief Header file for InstObject (EMdF layer)
 */


#ifndef INST_OBJECT__H__
#define INST_OBJECT__H__

#include "emdf.h"
#include "monads.h"
#include <vector>
#include <string>
#include "emdf_value.h"



enum {
  union_is_mlast = 0x00010000,
  union_is_pMonads = 0x00020000,
  must_delete_feature_array = 0x00040000,
  size_mask = 0x0000FFFF
};

class InstObject {
private:
  id_d_t m_id_d;
  monad_m m_first; // Always set to first monad
  union {
    monad_m last;
    SetOfMonads* pMonad_ms;
  } m_u;
  mutable EMdFValue *m_feature_value_arr;
  // Invariant:
  // m_feature_value_arr_size & size_mask is the size of the feature array
  // m_feature_value_arr_size & union_is_mlast:
  //   - m_u.m_last holds the last monad
  // m_feature_value_arr_size & union_is_pMonads:
  //   - m_u.pMonad_ms holds the set of monads
  // m_feature_value_arr_size & must_delete_feature_array:
  //   - the destructor must delete[] the feature value array.
  int m_feature_value_arr_size;
public:
  InstObject(id_d_t id_d, monad_m first_monad, monad_m last_monad, EMdFValue *feature_value_arr, int size_of_array);
  InstObject(id_d_t id_d, SetOfMonads *pSOM, EMdFValue *feature_value_arr, int size_of_array, bool bMustDeleteFeatureArray);
 private:
  InstObject(const InstObject& other);
  InstObject& operator=(const InstObject& other);
 public:
  ~InstObject();
  id_d_t getID_D(void) const { return m_id_d; };
  void getMonads(SetOfMonads &result) const;
  bool somHasOnlyOneMSE() const;
  void addMse(monad_m first_monad, monad_m last_monad);
  const EMdFValue* getFeature(unsigned int index) const;
  void changeString(unsigned int index, const std::string& newString);
  monad_m first() const { return m_first; };
  monad_m last() const  {
    if (m_feature_value_arr_size & union_is_mlast) {
      return m_u.last;
    } else {
      return m_u.pMonad_ms->last();
    }
  };
  int getFeatureValueArrSize(void) const { return m_feature_value_arr_size & size_mask; };
};




#endif // INST_OBJECT__H__
