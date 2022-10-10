/*
 * bpa.h
 *
 * BigPointerArray class.
 *
 * Ulrik Sandborg-Petersen
 * Created: 10/26-2015
 * Last update: 10/10-2022
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


#ifndef BPA__H__
#define BPA__H__

#include <cstdlib>
#include <cstring>

#include <string>
#include <vector>

template<class T> class BPAChunk {
 protected:
	unsigned long m_chunk_size; // Max number of items
	unsigned int m_load; // Current number of items
	T **m_data;
	BPAChunk<T> *m_next_chunk;
 public:
	BPAChunk(unsigned long chunk_size) {
		m_chunk_size = chunk_size;
		m_load = 0;
		m_data = (T**) malloc(chunk_size*sizeof(T*));
	};
	
	~BPAChunk() {
		free(m_data);
	};
	
	T *operator[](int item_relative_to_start) {
		// FIXME: No bounds checking?
		return m_data[item_relative_to_start];
	};
	
	bool append(T *item) {
		if (isFull()) {
			return false;
		} else {
			m_data[m_load++] = item;
			return true;
		}
	};
	
	bool assign_item(unsigned long index_relative_to_start, T *p) {
		if (index_relative_to_start >= m_chunk_size) {
			return false;
		} else {
			m_data[index_relative_to_start] = p;
			return true;
		}
	};

	unsigned long size() const { return m_load; };
		
	bool isEmpty() {
		return m_load == 0;
	};
		
	bool isFull() {
		return m_load == m_chunk_size;
	};
};


template<class T> class BigPointerArray {
 protected:

	unsigned long m_chunk_size;
	unsigned long m_load;
	std::vector<BPAChunk<T>*> m_chunks;
 public:
	BigPointerArray(unsigned long chunk_size) {
		m_chunk_size = 0;
		m_load = 0;
		m_chunks.push_back(new BPAChunk<T>(chunk_size));
	}
	~BigPointerArray() {
		for (unsigned int i = 0;
		     i < m_chunks.size();
		     ++i) {
			delete m_chunks[i];
		}
		m_chunks.clear();
	}

	void swap(unsigned long a, unsigned long b) {
		T *A = (*this)[a];
		T *B = (*this)[b];
		assign_item(b, A);
		assign_item(a, B);
	}

	bool assign_item(unsigned long index, T* p) {
		if (index >= m_load) {
			// Bounds checking
			return false;
		} else if (index < m_chunk_size) {
			// A common case
			return m_chunks[0]->assign_item(index, p);
		} else {
			// All other cases.
			int chunk_number = index / m_chunk_size;
			int chunk_index = index % m_chunk_size;
			return (m_chunks[chunk_number])->assign_item(chunk_index, p);
		}
	};

	// Returns index of newly appended item
	unsigned long append(T *p) {
		if (m_chunks[m_chunks.size()-1]->isFull()) {
			m_chunks.push_back(new BPAChunk<T>(m_chunk_size));
		}
		m_chunks[m_chunks.size()-1]->append(p);
		unsigned long index_of_item = m_load++;
		return index_of_item;
	}

	T *operator[](unsigned long index) {
		if (index >= m_load) {
			// Bounds checking
			return 0;
		} else if (index < m_chunk_size) {
			// A common case
			return (*m_chunks[0])[index];
		} else {
			// All other cases.
			int chunk_number = index / m_chunk_size;
			int chunk_index = index % m_chunk_size;
			return (*m_chunks[chunk_number])[chunk_index];
		}
	};
};



#endif // BPA__H__
