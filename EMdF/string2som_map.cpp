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
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2016  Ulrik Sandborg-Petersen
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




