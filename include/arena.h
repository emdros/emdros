/*
 * arena.h
 *
 * Arena and ArenaConstIterator
 *
 * Ulrik Sandborg-Petersen
 * Created: 3/2-2005
 * Last update: 6/23-2017
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2017  Ulrik Sandborg-Petersen
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

/**@file arena.h
 *@brief Header file for Arena and related classes (EMdF layer)
 */


#ifndef ARENA__H__
#define ARENA__H__

#include <string>
#include <cstring>
#include <cstdlib>
#include "debug.h"

/**
 *\defgroup Arena Arena: Chunks of memory.
 *\ingroup EMdF
 */



class ArenaConstIterator; // forward declaration
class ArenaAccessor; // forward declaration
class Bigstring; // forward declaration

/**\internal
 * @brief Arena: Chunks of easily allocated, easily freed memory.
 *\ingroup Arena
 * 
 * An "Arena", i.e., a linked listof large chunks of memory from which
 * one can allocate memory easily and then delete it all in one go.
 */
class Arena {
	friend class ArenaConstIterator;
	friend class ArenaAccessor;
 protected:
	/** A chunk memory.
	 *\ingroup Arena
	 *
	 * Part of the the linked list of chunks in an Arena.
	 */
	class Chunk {
	private:
		friend class Arena;
		friend class ArenaConstIterator;
		friend class ArenaAccessor;
		friend class Bigstring;
		enum { chunk_size = 512 * 1024 /**< The default chunk size */
		}; // 512KB
		Chunk *m_next; /**< Pointer to the next element in the linked list. */
		int m_index; /**< Index of the first free byte in m_bytes. */
		char *m_bytes; /**< The bytes of the chunk. */
		int m_chunk_size;
		/** Constructor: Constructs a fully free chunk with m_next=0. */
 		void allocateChunk(int mychunk_size) { m_bytes = new char[mychunk_size]; m_chunk_size = mychunk_size; }
	Chunk() : m_next(0), m_index(0), m_bytes(0), m_chunk_size(0) {allocateChunk(chunk_size);};
	private:
		// Leave unimplemented so as to prevent compiler from
		// producing ones that would crash...
		Chunk(const Chunk& other);
		Chunk& operator=(const Chunk& other);
	public:
		/** Constructor: Constructs a fully free chunk with m_next=pNext. 
		 * @param pNext The next Chunk. Afterwards. m_next == pNext. 
		 */
	        Chunk(Chunk *pNext, int mychunk_size = chunk_size) : m_next(pNext), m_index(0), m_bytes(0), m_chunk_size(0) { allocateChunk(mychunk_size); };
		/** Destructor. Also deletes m_next. */
		~Chunk() { delete m_next; delete[] m_bytes; }
		/** Set m_next.
		 * @param pNext The value to set m_next to.
		 */
		void setNext(Chunk *pNext) { m_next = pNext; };
		int getChunkSize() const { return m_chunk_size; };
	};
	Chunk *m_tail; /**< The tail of the linked list. */
	Chunk *m_head; /**< The head of the linked list. */
	int m_chunk_size;
 public:
	/** Constructor. Initializes to one chunk.*/
         Arena(int chunk_size = Chunk::chunk_size) : m_tail(0), m_head(0), m_chunk_size(chunk_size) { m_head = m_tail = new Chunk(0, m_chunk_size); };
	/** Destructor. Destroys all chunks. */
	virtual ~Arena() { delete m_head; };
 private:
	// Leave unimplemented, so as to prevent the compiler from
	// generating crashing ones...
	Arena(const Arena& other);
	Arena& operator=(const Arena& other);
 public:
	/** Clear the Arena by deleting all but the first chunk, and
	 *  then resetting the first chunk's index to 0. 
	 */
	void clear() {
		if (m_head != m_tail) {
			// There is more than one chunk,
			// so delete all but the first.
			Chunk *pTheRest = m_head->m_next;
			delete pTheRest;
			m_head->setNext(0);
			m_tail = m_head;
		} else {
			// There is only one Chunk, so do not delete anything.
		}
		m_head->m_index = 0;
	}

	unsigned long countBytes() const {
		unsigned long result = 0;
		const Chunk *pCurChunk = m_head;
		while (pCurChunk != 0) {
			result += pCurChunk->m_index;
			pCurChunk = pCurChunk->m_next;
		}
		return result;
	}
	
	/** Allocate a piece of memory of the given size.
	 *
	 * If the current chunk is not able to hold the given size,
	 * growAtLeast(size) is called.
	 *
	 * @param size The size of the piece of memory to allocate.
	 *
	 * @return A pointer within some Chunk to a piece of memory which is
	 * now allocated.
	 */
	void *allocate(int size) {
		if (m_tail->m_index + size > m_chunk_size) {
			growAtLeast(size);
		}
		ASSERT_THROW(m_tail->m_index + size <= m_tail->getChunkSize(),
			     "Chunk not grown correctly. size = " + int2string(size) + ", m_chunk_size = " + long2string(m_chunk_size));
		int current_index = m_tail->m_index;
		m_tail->m_index += size;
		return (void *) &(m_tail->m_bytes[current_index]);
	};

	/** Allocate a piece of memory of the given size, aligned on a
	 * byte-boundary which is a multiple of 4.
	 *
	 * If the current chunk is not able to hold the given size +
	 * alignment, growAtLeast(size) is called.
	 *
	 * @param size The size of the piece of memory to allocate.
	 *
	 * @return A pointer within some Chunk to a piece of memory which is
	 * now allocated.
	 */
	void *allocate_align4(int size) {
		// Calculate alignment padding from m_tail_>m_index:
		// 0 -> 0
		// 1 -> 3
		// 2 -> 2
		// 3 -> 1
		int alignment = (m_tail->m_index & 0x03L);
		if (alignment) {
			alignment = 4 - alignment;
		}
		int new_size = size + alignment;
		if (alignment + new_size > m_chunk_size) {
			growAtLeast(size);
			new_size = size;
			alignment = 0;
		}
		ASSERT_THROW(m_tail->m_index + new_size <= m_tail->getChunkSize(),
			     "Chunk not grown correctly");
		int current_index = m_tail->m_index + alignment;
		m_tail->m_index += new_size;
		return (void *) &(m_tail->m_bytes[current_index]);
	};
	/** Allocate a piece of memory of size 1 byte.
	 *
	 * If the current chunk is not able to hold the given size,
	 * grow() is called.
	 *
	 * @return A pointer within some Chunk to a piece of memory which is
	 * now allocated.
	 */
	void *allocate1() {
		if (m_tail->m_index + 1 >= m_chunk_size) {
			grow();
		}
		void *pResult = m_tail->m_bytes + m_tail->m_index;
		++(m_tail->m_index);
		return pResult;
	};
	ArenaConstIterator const_iterator(unsigned int size) const;
	ArenaAccessor accessor(unsigned int size);
 protected:
	/** Add a new chunk.*/
	void grow(void) { 
		Chunk *pCurrentTail = m_tail; 
		m_tail = new Chunk(0, m_chunk_size); 
		pCurrentTail->setNext(m_tail); 
	};
	/** Add a new chunk of size at least "size".*/
	void growAtLeast(int at_least_size) { 
		Chunk *pCurrentTail = m_tail;
		
		int real_chunk_size = 0;
		if (at_least_size <= m_chunk_size) {
			real_chunk_size = m_chunk_size;
		} else {
			// real_chunk_size is set to at_least_size*2,
			// but rounded down to the nearest number
			// divisible by 4.
			real_chunk_size = (at_least_size*2) & (~0x00000003);
		}
		
		m_tail = new Chunk(0, real_chunk_size); 
		pCurrentTail->setNext(m_tail); 
	};
};



/** Const iterator on specific size chunks in an Arena.
 *\ingroup Arena
 *
 * This iterates over the contents of chunks in an Arena.
 *
 * Each content must have a specific size.
 *
 * The first content must be at the beginning of the first chunk.
 */
class ArenaConstIterator {
 private:
	Arena::Chunk *m_current;
	int m_index;
	const Arena *m_pMotherArena;
	unsigned int m_size;
 public:
	ArenaConstIterator();
	ArenaConstIterator(unsigned int size, const Arena *pMotherArena);
	ArenaConstIterator(const ArenaConstIterator& other);
	virtual ~ArenaConstIterator();
	bool hasNext() const; // Is the iterator == end iterator?  Doesn't alter iterator
	void *next(); // Gets current and advances iterator afterwards
	void *current(); // Gets current without altering iterator
	ArenaConstIterator& operator=(const ArenaConstIterator& other);
 private:
	void assign(const ArenaConstIterator& other);
};


/** A "big string".
 *\ingroup Arena
 *
 * This is a string which is capable of holding lots and lots of data,
 * even many megabytes.
 *
 * This is useful on Windows Visual C++, where the implementation of
 * std::string chokes on strings larger than about 1MB.
 */
class Bigstring : protected Arena {
 public:
	/** Constructor. Constructs an empty string. */
        Bigstring(int chunk_size = Chunk::chunk_size) : Arena(chunk_size) {};
	/* Destructor. Deallocates all memory. */
	~Bigstring() {};
	void addBigstring(Bigstring *pOther) {
		Chunk *pOtherChunk = pOther->m_head;
		while (pOtherChunk != 0) {
			int other_chunk_size = pOtherChunk->m_index;
			if (pOtherChunk->m_index > 0) {
				int left_in_tail = m_tail->getChunkSize() - m_tail->m_index;
				if (other_chunk_size <= left_in_tail) {
					addCharsSimple(pOtherChunk->m_bytes, other_chunk_size);
				} else {
					addCharsSimple(pOtherChunk->m_bytes, left_in_tail);
					addCharsSimple(pOtherChunk->m_bytes + left_in_tail, other_chunk_size - left_in_tail);
				}
			}
			pOtherChunk = pOtherChunk->m_next;
		}

	}
	/** Add a single char.
	 *
	 * @param c the char to add.
	 */
	void addChar(char c) { void *pMem = allocate1(); *((char *)pMem)  = c; };
	/** Add a string with a number of chars. This version can add
	 * a string of length greater than m_chunk_size.
	 * 
	 * @param pSrc The string to add. 
	 *
	 * @param nchars The number of chars to add from pSrc.
	 */
	void addChars(const char *pSrc, size_t nchars) {
		size_t nCharsLeft = nchars;
		const char *pStrLocal = pSrc;
		while (nCharsLeft > 0) {
			int nCharsAdded = 0;
			size_t left_in_chunk = m_tail->getChunkSize() - m_tail->m_index;

			if (left_in_chunk == 0) {
				grow();
			}
			left_in_chunk = m_tail->getChunkSize() - m_tail->m_index;

			if (nCharsLeft > left_in_chunk) {
				addCharsSimple(pStrLocal, left_in_chunk);
				nCharsAdded = left_in_chunk;
			} else {
				addCharsSimple(pStrLocal, nCharsLeft);
				nCharsAdded = nCharsLeft;
			}
			pStrLocal += nCharsAdded;
			nCharsLeft -= nCharsAdded;
		}
	}
	
	/** Add a string with a number of chars. This cannot add a
	 * string of length greater than m_chunk_size.
	 * 
	 * @param pSrc The string to add. 
	 *
	 * @param nchars The number of chars to add from pSrc.
	 */
	void addCharsSimple(const char *pSrc, size_t nchars) {
		void *pMem = allocate((int) nchars); memcpy(pMem, pSrc, nchars);
	}
	/** Add a zero-delimited string.
	 * 
	 * @param sz The zero-delimited string to add. The trailing
	 * zero is NOT added.  */
	void addsz(const char *sz) { size_t len = strlen(sz); addChars(sz, len); };
	/** Convert to a std::string.
	 *
	 * @param str The output std::string to which to "export".
	 */
	void toString(std::string& str) {
		str = toString();
	};
	/** Convert to a std::string.
	 *
	 * @return The output std::string to which to "export".
	 */
	std::string toString() {
		std::string str;
		unsigned long strLength = countBytes();
		str.reserve(strLength);
		Chunk *pChunk = m_head;
		while (pChunk != 0) {
			if (pChunk->m_index > 0) {
				str.append((char*) pChunk->m_bytes, pChunk->m_index);
			}
			pChunk = pChunk->m_next;
		} 
		return str;
	};
	bool is_other_than_whitespace() {
		Chunk *pChunk = m_head;
		int index;
		while (pChunk != 0) {
			index = 0;
			while (index < pChunk->m_index) {
				char c = pChunk->m_bytes[index];
				switch (c) {
					// NOTE: '\v' is missing, but I don't think it is ever used!
				case ' ':
				case '\n':
				case '\t':
				case '\r':
					break;
				default:
					return true;
				}
				++index;
			}
			pChunk = pChunk->m_next;
		} 
		return false;
	}
	/** Test if we are empty. 
	 * @return true iff the string is the empty string. 
	 */
	bool empty() const { return !const_iterator(1).hasNext(); };

	/** Clear the BigString.
	 */
	void clear() {
		Arena::clear();
		m_head->m_bytes[0] = '\0';
	}
};


class ObjectArenaChunk {
 protected:
	friend class ObjectArenaAccessor;
	friend class ObjectArenaConstIterator;
	unsigned int m_object_size;
	unsigned int m_max_object_count;
	unsigned int m_cur_object_count;
	unsigned int m_index; /**< Index of the first free byte in m_pBytes. */
	unsigned int m_chunk_size;
	char *m_pBytes;
 public:
	ObjectArenaChunk(unsigned int object_size, unsigned int object_count_per_chunk);
	~ObjectArenaChunk();

	bool canAllocateObject();
	
	// Returns 0 upon space not available in chunk
	void *allocateObject();
};

class ObjectArena {
 protected:
	friend class ObjectArenaAccessor;
	friend class ObjectArenaConstIterator;
	std::vector<ObjectArenaChunk*> m_chunks;
	ObjectArenaChunk *m_pCurChunk;
	unsigned int m_object_size;
	unsigned int m_object_count_per_chunk;
	unsigned int m_total_object_count;
 public:
	ObjectArena(unsigned int object_size, unsigned int object_count_per_chunk);
	~ObjectArena();
	void *allocateObject();
	void reset();
 protected:
	void deleteAll();
	void grow();
};

class ObjectArenaAccessor {
 private:
	ObjectArena *m_pMotherArena;
 public:
	ObjectArenaAccessor(ObjectArena *pMotherArena);
	~ObjectArenaAccessor();

	void *operator[](unsigned int object_index);
};

class ObjectArenaConstIterator {
 private:
	const ObjectArena *m_pMotherArena;
	unsigned int m_chunk_number;
	ObjectArenaChunk *m_pCurChunk;
	unsigned int m_chunk_byte_index;
	unsigned int m_object_count;
 public:
	ObjectArenaConstIterator(const ObjectArena *pMotherArena);
	~ObjectArenaConstIterator();

	bool hasNext() const;

	void *current();
	void *next();
};



#endif // ARENA__H__
