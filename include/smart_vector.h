/*
 * smart_vector.h
 *
 * SmartVector template
 *
 * Ulrik Petersen
 * Created: 1/18-2003
 * Last update: 7/17-2004
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2004  Ulrik Petersen
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

/**@file smart_vector.h
 *@brief Header file for smart vector (EMdF layer)
 */


#ifndef SMART_VECTOR__H__
#define SMART_VECTOR__H__

#include <vector>
#include <functional>

/////////////////////////////////////////////////////////////
//
// SmartVector template
//
// SmartVector takes a class type and a predicate as template 
// arguments. The predictate must be an "equals" predicate which
// compares two T*'s for equality.
//
// short int addValue(T* v, P p);
// - addValue only adds v if there is no member in the vector
//   already which compares equal with v using p.  It returns
//   the index of the existing value if present, and the index
//   of the new value if added.
//
// short int getIndex(T* v, P p);
// - Gets the index of the value that compares equal to v using p.
//   returns no_element if not present.
//
// T* operator[](short int index);
// - Index access operator.  There is no special bounds checking.
//
// void setValue(T* v, short int index);
// - Deletes the current value at index and sets to v at index.
//   Assumes that index is valid.
//
//
//


template<class T, class P = std::equal_to<T> > class SmartVector {
private:
  std::vector<T*> m_vec;
  bool m_bDelete;
public:
  static const short int no_element = -1;
  SmartVector(bool bDelete) : m_bDelete(bDelete) {};
  ~SmartVector(void);
  // Returns index of value
  short int addValue(T* v, P p, bool& /* out */ bWasAdded);
  short int getIndex(T* v, P p);
  T* operator[](short int index);
  void setValue(T* v, short int index);
  short int size(void) const { return m_vec.size(); };
};

//template<class T, class P> const short int SmartVector<T,P>::no_element = -1;

template <class T, class P> SmartVector<T,P>::~SmartVector(void)
{
  for (unsigned int i = 0; i < m_vec.size(); i++) {
    if (m_bDelete) {
      delete m_vec[i];
    }
    m_vec[i] = 0;
  }
  m_vec.resize(0);
}

  // Returns index of value
template <class T, class P> short int SmartVector<T,P>::addValue(T* v, P p, bool& /* out */ bWasAdded)
{
  bWasAdded = false;
  typename std::vector<T*>::const_iterator ci(m_vec.begin());
  typename std::vector<T*>::const_iterator ciend(m_vec.end());
  short int index = 0;
  while (ci != ciend) {
    if (p((*ci), v)) {
      return index;
    }
    index++;
    ci++;
  }

  // It wasn't there, so add it
  bWasAdded = true;
  m_vec.push_back(v);
  return index;
}

template <class T, class P> short int SmartVector<T,P>::getIndex(T* v, P p)
{
  typename std::vector<T*>::const_iterator ci(m_vec.begin());
  typename std::vector<T*>::const_iterator ciend(m_vec.end());
  short int index = 0;
  while (ci != ciend) {
    if (p((*ci), v)) {
      return index;
    }
    index++;
    ci++;
  }
  return no_element;
}

template <class T, class P> T* SmartVector<T,P>::operator[](short int index) 
{
  return m_vec[index];
}

template <class T, class P> void SmartVector<T,P>::setValue(T* v, short int index)
{
  m_vec[index] = v;
}



#endif // SMART_VECTOR__H__
