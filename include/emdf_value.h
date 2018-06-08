/*
 * emdf_value.h
 *
 * EMdFValue class
 *
 * Ulrik Petersen
 * Created: 1/18-2003
 * Last update: 6/8-2018
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

/**@file emdf_value.h
 *@brief Header file for EMdFValue (EMdF layer)
 */


#ifndef EMDF_VALUE__H__
#define EMDF_VALUE__H__

#include "emdf.h"
#include <string>
#include "emdf_enums.h"
#include "exception_emdros.h"

class IntegerList; // Forward declaration
class SetOfMonads; // Forward declaration


/** An EMdF value.
 *
 * Is a union of an id_d_t, a long, and a pointer to std::string.
 *
 * Also has an eEVkind member, showing what kind it is.
 */
class EMdFValue {
private:
#ifndef SWIG
  union {
    id_d_t m_id_d;
    emdros_int64 m_int;
    std::string *m_pString;
    IntegerList *m_pIntegerList;
    SetOfMonads *m_pSOM;
  } m_u;
  eEVkind m_kind;
#endif /* ndef SWIG */
public: 
  EMdFValue(); // Don't use unless you plan to = assign a new value later
  EMdFValue(const EMdFValue& other);
  EMdFValue(eEVkind kind, emdros_int64 i);
  EMdFValue(const std::string& str);
  EMdFValue(const SetOfMonads& som);
  EMdFValue(eEVkind kind, IntegerList* pIntegerList);
  ~EMdFValue();
  eEVkind getKind(void) const { return m_kind; };
  const std::string getString(void) const;
  SetOfMonads getSOM(void) const;
  id_d_t getID_D(void) const { return m_u.m_id_d; };
  emdros_int64 getInt(void) const { return m_u.m_int; };
  emdros_int64 getEnum(void) const { return m_u.m_int; };
  IntegerList *getIntegerList (void) const;
  bool compare(const EMdFValue& other, eComparisonOp op) const;
  std::string toString(void) const;
  void changeString(const std::string& newString);
#ifndef SWIG
  EMdFValue& operator=(const EMdFValue& other);
#endif /* ndef SWIG */
private:
  bool compareInt(const EMdFValue& other, eComparisonOp op) const;
  bool compareString(const EMdFValue& other, eComparisonOp op) const;
  bool compareListOfIntegerWithAtomicValue(const EMdFValue& other, eComparisonOp op) const;
  bool compareSetOfMonads(const EMdFValue& other, eComparisonOp op) const;
  void assign(const EMdFValue& other);
  void deleteContents();
};




#endif // EMDF_VALUE__H__
