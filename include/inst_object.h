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
