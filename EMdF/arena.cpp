/*
 * arena.cpp
 *
 * Arena and related classes
 *
 * Ulrik Sandborg-Petersen
 * Created: 3/2-2005
 * Last update: 6/24-2016
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2016  Ulrik Sandborg-Petersen
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

/**@file arena.cpp
 *@brief Implementation of Arena and related classes (EMdF layer)
 */

#include <arena.h>
#include <debug.h>

#include <iostream>

#include <cstring>

////////////////////////////////////////////////////////////////////
//
// ObjectArenaChunk
//
////////////////////////////////////////////////////////////////////
ObjectArenaChunk::ObjectArenaChunk(unsigned int object_size, unsigned int object_count_per_chunk)
	: m_object_size(object_size),
	  m_max_object_count(object_count_per_chunk),
	  m_cur_object_count(0),
	  m_index(0),
	  m_chunk_size(object_size * object_count_per_chunk),
	  m_pBytes(0)
{
	m_pBytes = new char[m_chunk_size];
	memset((void*) m_pBytes, 0, m_chunk_size);
}

ObjectArenaChunk::~ObjectArenaChunk()
{
	delete[] m_pBytes;
}

bool ObjectArenaChunk::canAllocateObject()
{
	return m_cur_object_count < m_max_object_count;
}
	
void *ObjectArenaChunk::allocateObject()
{
	if (canAllocateObject()) {
		void *pObj = (void*) (m_pBytes + m_index);

		m_index += m_object_size;
		++m_cur_object_count;
		
		return pObj;
	} else {
		return 0;
	}
}

////////////////////////////////////////////////////////////////////
//
// ObjectArena
//
////////////////////////////////////////////////////////////////////

ObjectArena::ObjectArena(unsigned int object_size, unsigned int object_count_per_chunk)
	: m_pCurChunk(0),
	  m_object_size(object_size),
	  m_object_count_per_chunk(object_count_per_chunk),
	  m_total_object_count(0)
{
	grow();
}


ObjectArena::~ObjectArena()
{
	deleteAll();
}


void *ObjectArena::allocateObject()
{
	void *pObj = m_pCurChunk->allocateObject();
	if (pObj == 0) {
		// m_pCurChunk was exhausted
		// Add another
		grow();

		// Then allocate in the new chunk
		pObj = m_pCurChunk->allocateObject();
	}

	++m_total_object_count;

	return pObj;
}

void ObjectArena::reset()
{
	deleteAll();
	grow();
}

void ObjectArena::deleteAll()
{
	std::vector<ObjectArenaChunk*>::iterator it = m_chunks.begin();
	std::vector<ObjectArenaChunk*>::iterator itend = m_chunks.end();
	while (it != itend) {
		delete *it;
		++it;
	}
	m_chunks.clear();

	m_total_object_count = 0;
}


void ObjectArena::grow()
{
	ObjectArenaChunk *pChunk = new ObjectArenaChunk(m_object_size, m_object_count_per_chunk);
	m_chunks.push_back(pChunk);
	m_pCurChunk = pChunk;
}



////////////////////////////////////////////////////////////////////
//
// ObjectArenaAccessor
//
////////////////////////////////////////////////////////////////////
ObjectArenaAccessor::ObjectArenaAccessor(ObjectArena *pMotherArena)
	: m_pMotherArena(pMotherArena)
{
}
ObjectArenaAccessor::~ObjectArenaAccessor()
{
}

void *ObjectArenaAccessor::operator[](unsigned int object_index)
{
	if (object_index < m_pMotherArena->m_total_object_count) {
		unsigned int object_count_per_chunk = m_pMotherArena->m_object_count_per_chunk;
		unsigned int index_into_chunk = object_index % object_count_per_chunk;
		unsigned int chunk_index = object_index / object_count_per_chunk;
		
		ObjectArenaChunk *pChunk = m_pMotherArena->m_chunks[chunk_index];
		unsigned int byte_index_into_chunk = index_into_chunk * m_pMotherArena->m_object_size;
		void *pObj = (void*) (pChunk->m_pBytes + byte_index_into_chunk);
		return pObj;
	} else {
		return 0;
	}
}

////////////////////////////////////////////////////////////////////
//
// ObjectArenaConstIterator
//
////////////////////////////////////////////////////////////////////
const ObjectArena *m_pMotherArena;
unsigned int m_chunk_number;
ObjectArenaChunk *m_pCurChunk;
unsigned int m_chunk_byte_index;
ObjectArenaConstIterator::ObjectArenaConstIterator(const ObjectArena *pMotherArena)
	: m_pMotherArena(pMotherArena),
	  m_chunk_number(0),
	  m_pCurChunk(0),
	  m_chunk_byte_index(0),
	  m_object_count(0)
{
}

ObjectArenaConstIterator::~ObjectArenaConstIterator()
{
}


bool ObjectArenaConstIterator::hasNext() const
{
	return (m_object_count < m_pMotherArena->m_total_object_count);
}

void *ObjectArenaConstIterator::current() 
{
	if (hasNext()) {
		void *pObj = (void*) (m_pCurChunk->m_pBytes + m_chunk_byte_index);
		return pObj;
	} else {
		return 0;
	}
}

void *ObjectArenaConstIterator::next() 
{
	if (hasNext()) {
		void *pObj = (void*) (m_pCurChunk->m_pBytes + m_chunk_byte_index);
		if (m_chunk_byte_index < m_pCurChunk->m_chunk_size) {
			m_chunk_byte_index += m_pCurChunk->m_object_size;
		} else {
			++m_chunk_number;
			
			m_pCurChunk = m_pMotherArena->m_chunks[m_chunk_number];

			m_chunk_byte_index = 0;
		}
		++m_object_count;
		
		return pObj;
	} else {
		return 0;
	}
}


////////////////////////////////////////////////////////////////////
//
// ArenaConstIterator
//
////////////////////////////////////////////////////////////////////

/** Constructor.
 *
 * Constructs an empty ArenaConstIterator which is not associated with
 * any Arena.
 */
ArenaConstIterator::ArenaConstIterator()
	: m_current(0),
	  m_index(0),
	  m_pMotherArena(0),
	  m_size(0)
{
}


/** Constructor. Sets size and mother Arena.
 *
 * @param size The size in bytes of the elements in the Arena.
 *
 * @param pMotherArena The mother arena over which to iterate.
 */
ArenaConstIterator::ArenaConstIterator(unsigned int size, const Arena *pMotherArena)
	: m_current(pMotherArena->m_head),
	  m_index(0),
	  m_pMotherArena(pMotherArena),
	  m_size(size)
{
}


/** Copy constructor.
 *
 * @param other The other ArenaConstIterator from which to initialize.
 */
ArenaConstIterator::ArenaConstIterator(const ArenaConstIterator& other)
	: m_current(0),
	  m_index(0),
	  m_pMotherArena(0),
	  m_size(0)
{
	assign(other);
}


/** Destructor
 */
ArenaConstIterator::~ArenaConstIterator()
{
}


/** Check whether there are more elements in the Arena.
 *
 * @return True iff there is at least one more element of the specified size
 * in the Arena.
 */
bool ArenaConstIterator::hasNext() const
{
	if (m_current == 0) {
		return false;
	} else if (m_current->m_next != 0) {
		return true;
	} else if (m_current->m_index > m_index) {
		return true;
	} else {
		return false;
	}
}

/** Get pointer to current element and advance iterator.
 *
 * @return The pointer to the current element.
 */
void* ArenaConstIterator::next()
{
	ASSERT_THROW(hasNext(),
		     "hasNext() is false");
	void* pMem = (void*) (m_current->m_bytes + m_index);
	m_index += m_size;
	if (m_index >= Arena::Chunk::chunk_size
	    || m_index >= m_current->m_index) {
		m_current = m_current->m_next;
		m_index = 0;
	}
	return pMem;
}


/** Get pointer to current element but DO NOT advance iterator.
 *
 * @return The pointer to the current element.
 */
void* ArenaConstIterator::current()
{
	ASSERT_THROW(hasNext(), "hasNext() is false");
	void* pMem = m_current->m_bytes + m_index;
	return pMem;
}

/** Assignment operator.
 * 
 * @param other The other ArenaConstIterator from which to assign to
 * ourselves.
 * 
 * @return ourselves.
 */
ArenaConstIterator& ArenaConstIterator::operator=(const ArenaConstIterator& other)
{
	assign(other);
	return *this;
}


/** Assignment method.
 *
 * @param other The other ArenaConstIterator from which to assign to
 * ourselves.
 */
void ArenaConstIterator::assign(const ArenaConstIterator& other)
{
	m_current = other.m_current;
	m_index = other.m_index;
	m_pMotherArena = other.m_pMotherArena;
	m_size = other.m_size;
}



////////////////////////////////////////////////////////////
//
// Arena
//
////////////////////////////////////////////////////////////
/** Return an ArenaConstIterator over elements of the given size.
 *
 * @param size The size of the elements in the Arena.
 *
 * @return The ArenaConstIterator over this Arena with the specified
 * size element.
 */
ArenaConstIterator Arena::const_iterator(unsigned int size) const
{
	return ArenaConstIterator(size, this); 
}




