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

/*
 * Copyright (C) 2001-2018     Ulrik Sandborg-Petersen
 * Copyright (C) 2018-present  Sandborg-Petersen Holding ApS, Denmark
 *
 * Licensed under the MIT License.
 *
 * Please see the file COPYING in the root of the sources for more details.
 *
 */


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




