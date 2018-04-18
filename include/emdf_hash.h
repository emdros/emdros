/*
 * emdf_hash.h
 *
 * Hash table class for Emdros
 *
 * Ulrik Petersen
 * Created: 7/12-2013
 * Last update: 5/23-2017
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2013-2017  Ulrik Sandborg-Petersen
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


/**@file emdf_hash.h
 *@brief Header file for basic hash table for Emdros
 */


#ifndef EMDF_HASH__
#define EMDF_HASH__

#include <string>
#include <cstdlib>


/*
 * <stdint.h> does not exist in MS Visual Studio 2008.
 * Hence, we typedef some things ourselves.
 *
 */
#ifdef _MSC_VER

typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;

#else
#include <stdint.h>
#endif

#include <vector>

#include "emdfdb.h" // For hash_djb2().
#include "debug.h"


// EmdrosStringHashTableNode

template<class V> class EmdrosStringHashTableNode {
 public:
	std::string m_key;
	V m_value;
	EmdrosStringHashTableNode<V> *m_pNext;

	EmdrosStringHashTableNode<V>(const std::string& key, V value) : m_key(key), m_value(value), m_pNext(0) {};
	~EmdrosStringHashTableNode() {};
	bool operator==(const EmdrosStringHashTableNode<V>& other) {
		return (m_key == other.m_key);
	};
};

template<class V> class EmdrosStringHashTableVisitor {
 public:
	EmdrosStringHashTableVisitor<V>() {};
	virtual ~EmdrosStringHashTableVisitor<V>() {};

	virtual void operator()(V& value){};
};


// EmdrosStringHashTable

template<class V> class EmdrosStringHashTable {
 protected:
	int m_hash_table_size; // length of m_table in nodes
	int m_no_of_keys; // number of keys in table at the moment
	EmdrosStringHashTableNode<V> **m_table;
 public:
	EmdrosStringHashTable(int initial_hash_table_size);
	~EmdrosStringHashTable();

	bool addEntry(const std::string& key, V value);
	bool entryExists(const std::string& key, V *pValue) const;
	void clear();
	void visitAllEntries(EmdrosStringHashTableVisitor<V>& visitor);
};


template<class V> EmdrosStringHashTable<V>::EmdrosStringHashTable(int initial_hash_table_size)
{
	m_table = (EmdrosStringHashTableNode<V>**) malloc(initial_hash_table_size * sizeof(EmdrosStringHashTable<V>*));
	m_hash_table_size = initial_hash_table_size;
	for (int index = 0;
	     index < m_hash_table_size;
	     ++index) {
		m_table[index] = 0;
	}
	m_no_of_keys = 0;
}


template<class V> EmdrosStringHashTable<V>::~EmdrosStringHashTable()
{
	clear();
	free(m_table);
}


/**
 * Add an entry to the hash table if the key is not there already.
 * 
 * @param key the key of the value
 *
 * @param value the value of the key
 *
 * @return true iff the key was not there, and the entry was therefore
 * added. false if the key was there, and the entry was therefore not
 * added.
 */

template<class V> bool EmdrosStringHashTable<V>::addEntry(const std::string& key, V value)
{
	ASSERT_THROW(m_table != 0,
		     "EmdrosStringHashTable::addEntry(): m_table is 0.")
	long hash_index = hash_djb2(key, m_hash_table_size);
	EmdrosStringHashTableNode<V> *pNode;
	pNode = m_table[hash_index];
	if (pNode == 0) {
		m_table[hash_index] = new EmdrosStringHashTableNode<V>(key, value);
		++m_no_of_keys;
		return true;
	} else {
		while (pNode != 0) {
			if (pNode->m_key == key) {
				return false;
			}
			pNode = pNode->m_pNext;
		}
		pNode = new EmdrosStringHashTableNode<V>(key, value);
		pNode->m_pNext = m_table[hash_index];
		m_table[hash_index] = pNode;
		++m_no_of_keys;
		return true;
	}
}

/**
 *
 * @param key The key to look for
 *
 * @param pValue The value of the key if it exists, otherwise undefined
 *
 * @return true iff the key exists.
 */
template<class V> bool EmdrosStringHashTable<V>::entryExists(const std::string& key, V *pValue) const
{
	ASSERT_THROW(m_table != 0,
		     "EmdrosStringHashTable::addEntry(): m_table is 0.")
	long hash_index = hash_djb2(key, m_hash_table_size);
	EmdrosStringHashTableNode<V> *pNode;
	pNode = m_table[hash_index];
	while (pNode != 0) {
		if (pNode->m_key == key) {
			*pValue = pNode->m_value;
			return true;
		}
		pNode = pNode->m_pNext;
	}
	return false;
}


template<class V> void EmdrosStringHashTable<V>::clear()
{
	EmdrosStringHashTableNode<V> *pNode;
	int table_index = 0;
	while (table_index < m_hash_table_size) {
		pNode = m_table[table_index];
		while (pNode != 0) {
			EmdrosStringHashTableNode<V> *pNextNode = pNode->m_pNext;
			delete pNode;
			pNode = pNextNode;
		}
		m_table[table_index] = 0;
		++table_index;
	}
}


template<class V> void EmdrosStringHashTable<V>::visitAllEntries(EmdrosStringHashTableVisitor<V>& visitor)
{
	EmdrosStringHashTableNode<V> *pNode;
	int table_index = 0;
	while (table_index < m_hash_table_size) {
		pNode = m_table[table_index];
		while (pNode != 0) {
			EmdrosStringHashTableNode<V> *pNextNode = pNode->m_pNext;
			visitor(pNode->m_value);
			pNode = pNextNode;
		}
		++table_index;
	}
}

inline int hash_djb2_int(int value, long hashtable_size)
{    
	//  djb2
	// From http://www.cse.yorku.ca/~oz/hash.html

        uint32_t hash = 5381;
        uint32_t c;

	char *p = (char*) &value;
	int i = 0;
	int length = sizeof(int);
	while (i < length) {
		c = (uint32_t) *p++;
		hash = ((hash << 5) + hash) ^ c; /* hash * 33 ^ c */
		++i;
	}
        return hash % hashtable_size;
}




#endif /* EMDF_HASH__ */
