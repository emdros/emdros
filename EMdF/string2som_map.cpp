/*
 * string2som_map.cpp
 *
 * String to SetOfMonads Map
 *
 * Ulrik Sandborg-Petersen
 * Created: 6/22-2016
 * Last update: 6/22-2016
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


/**@file string2som_map.cpp
 *@brief String to SetOfMonads map (EMdF layer)
 */

#include "string2som_map.h"

////////////////////////////////////////////////////////////////
//
// String2SOMMapConstIterator
//
////////////////////////////////////////////////////////////////

String2SOMMapConstIterator::String2SOMMapConstIterator()
{
	m_pMother = 0;
}


String2SOMMapConstIterator::String2SOMMapConstIterator(const String2SOMMap *pMother)
{
	m_pMother = pMother;
	if (m_pMother != 0) {
		m_ci = m_pMother->m_map.begin();
	}
}


String2SOMMapConstIterator::String2SOMMapConstIterator(const String2SOMMapConstIterator& other)
{
	assign(other);
}


String2SOMMapConstIterator::~String2SOMMapConstIterator()
{
	// Nothing to do.
}



bool String2SOMMapConstIterator::hasNext() const
{
	if (m_pMother == 0) {
		return false;
	} else {
		return m_ci != m_pMother->m_map.end();
	}
}


std::string String2SOMMapConstIterator::next()
{
	ASSERT_THROW(hasNext(), "String2SOMMapConstIterator::next(): hasNext() is false. Thus this function should not be called.");
	std::string result_key = m_ci->first;
	++m_ci;
	return result_key;
}


std::string String2SOMMapConstIterator::current() const
{
	ASSERT_THROW(hasNext(), "String2SOMMapConstIterator::current(): hasNext() is false. Thus this function should not be called.");
	return m_ci->first;
}




void String2SOMMapConstIterator::assign(const String2SOMMapConstIterator& other)
{
	m_pMother = other.m_pMother;
	m_ci = other.m_ci;
}


String2SOMMapConstIterator& String2SOMMapConstIterator::operator=(const String2SOMMapConstIterator& other)
{
	assign(other);
	return *this;
}



////////////////////////////////////////////////////////////////
//
// String2SOMMap
//
////////////////////////////////////////////////////////////////


String2SOMMap::String2SOMMap()
{
}


String2SOMMap::~String2SOMMap()
{
	m_map.clear();
}



void String2SOMMap::addMonad(const std::string& key, monad_m monad)
{
	std::map<std::string, SetOfMonads>::iterator it = m_map.find(key);
	if (it == m_map.end()) {
		std::pair<std::map<std::string, SetOfMonads>::iterator, bool> mypair = m_map.insert(std::make_pair(key, SetOfMonads()));
		it = mypair.first;
	}
	it->second.add(monad);
}


void String2SOMMap::addMonadRange(const std::string& key, monad_m first_monad, monad_m last_monad)
{
	std::map<std::string, SetOfMonads>::iterator it = m_map.find(key);
	if (it == m_map.end()) {
		std::pair<std::map<std::string, SetOfMonads>::iterator, bool> mypair = m_map.insert(std::make_pair(key, SetOfMonads()));
		it = mypair.first;
	}
	it->second.add(first_monad, last_monad);
}


void String2SOMMap::addMonadSet(const std::string& key, const SetOfMonads& som)
{
	// insert returns a pair where pair::first is an iterator
	// pointing to the newly inserted element (if pair::second is
	// true), or to an existing element with an equivalent key (if
	// pair::second is false).
	std::pair<std::map<std::string, SetOfMonads>::iterator, bool> mypair = m_map.insert(std::make_pair(key, som));
	if (!mypair.second) {
		mypair.first->second.unionWith(som);
	}
}



SetOfMonads String2SOMMap::getMonadSet(const std::string& key) const
{
	std::map<std::string, SetOfMonads>::const_iterator ci = m_map.find(key);
	if (ci == m_map.end()) {
		return SetOfMonads();
	} else {
		return ci->second;
	}
}


String2SOMMapConstIterator String2SOMMap::const_iterator() const
{
	return String2SOMMapConstIterator(this);
}



bool String2SOMMap::hasKey(const std::string& key) const
{
	return m_map.find(key) != m_map.end();
}

bool String2SOMMap::isEmpty() const
{
	return m_map.empty();
}




