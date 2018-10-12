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

/*
 * Copyright (C) 2001-2018     Ulrik Sandborg-Petersen
 * Copyright (C) 2018-present  Sandborg-Petersen Holding ApS, Denmark
 *
 * Licensed under the MIT License.
 *
 * Please see the file COPYING in the root of the sources for more details.
 *
 */


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
