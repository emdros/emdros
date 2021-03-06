/*
 * inst.h
 *
 * inst(T,Su) class
 *
 * Ulrik Petersen
 * Created: 2/26-2001
 * Last update: 11/14-2018
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


/**@file inst.h
 *@brief Header file for Inst and related classes (EMdF layer)
 */


#ifndef INST__H__
#define INST__H__

#include "emdf.h"
#include "infos.h"
#include "inst_object.h"
#include "arena.h"
#include "monads.h"
#include <list>
#include <vector>
#include <map>
#include <string>



/**************************************************************************
 *
 * Skip lists
 *
 * See Pugh, William (1990), "Skip lists: A probabilistic alternative
 * to balanced trees.", Communications of the ACM, 33(6):668--676,
 * June 1990.
 *
 * Based on the paper and reference implementation available at
 * ftp://ftp.cs.umd.edu/pub/skipLists
 *
 * Basically, a skipList is a probablistic linked list with access and
 * insert times O(log(N)).
 *
 * This is with p = 1/8. Therefore, with MAX_NUMBER_OF_LEVELS set to
 * 11, the maximum number of elements the list can hold comfortably is
 * 8^11, which is well beyond what we will ever need.
 *
 **************************************************************************/




/** Get the next node after pNode.
 *@internal
 *
 * @param pNode the node after which to get the next node.
 */
#define GET_NEXT(pNode) (pNode->m_forward[0])

/** Get the key of a node (the first monad).
 *@internal
 *
 * @param pNode The node from which to get the key.
 */
#define GET_KEY(pNode) (pNode->m_pValue->first())
#define GET_ID_D(pNode) (pNode->m_pValue->getID_D())


/** A skip list node.
 *@internal.
 */
typedef struct SkipListNode {
	InstObject *m_pValue; /**< The payload of this node. */
	struct SkipListNode *m_forward[1]; /**< The forward array. */
} *PCSkipListNode;

class SkipList; // Forward declaration

/** A const iterator over a SkipList.
 *@internal
 */
class SkipListConstIterator {
 private:
	friend class SkipList;
	const SkipList *m_pMotherSkipList;
	const SkipListNode *m_iter;
 public:
	SkipListConstIterator(const SkipList *pMotherSkipList);
	SkipListConstIterator(const SkipList *pMotherSkipList, SkipListNode *pIter);
	SkipListConstIterator();
	SkipListConstIterator(const SkipListConstIterator& other);
	~SkipListConstIterator() {}; /**< The destructor. */
#ifndef SWIG
	SkipListConstIterator& operator=(const SkipListConstIterator& other);
	bool operator==(const SkipListConstIterator& other) const { return m_iter == other.m_iter; }; /**< equality operator. */
	bool operator!=(const SkipListConstIterator& other) const { return m_iter != other.m_iter; }; /**< inequality operator. */
#endif 
	// Is the iterator == end iterator?  Doesn't alter iterator
	bool hasNext() const;
	// Gets current and advances iterator afterwards
	const InstObject *next();
	// Gets current without altering iterator
	const InstObject *current();
	monad_m advanceToNextKey();
	monad_m currentKey();
};


/** A skip list over InstObject objects.
 *@internal
 */
class SkipList {
 protected:
	friend class SkipListConstIterator;
	int m_level;
	SkipListNode *m_NIL;
	SkipListNode *m_head;
	int m_randomsLeft;
	int m_randomBits;
	unsigned long m_rand;
	Arena *m_pAllocator;
 public:
	typedef SkipListConstIterator const_iterator;
	SkipList();
 private:
	SkipList(const SkipList& other);
	SkipList& operator=(const SkipList& other);
 public:

	~SkipList();
	//
	// Adapted from B. Stroustrup, The C++ Programming Language, 3rd
	// edition, p. 686.
	//
	/** Generate a random number. */
	unsigned long random() { return (m_rand = m_rand * 1103515245 + 12345); };

	void insert(monad_m key, InstObject *pValue);
	const_iterator search(monad_m key) const;
	/** Get a const_iterator for the beginning of the skip list.
	 *
	 * @return A const_iterator pointing to the start of the skip list.
	 */
	const_iterator begin() const { return SkipListConstIterator(this, GET_NEXT(m_head)); };

	/** Get a const_iterator for the "one-beyond-the-end" of the skip list.
	 *
	 * @return A const_iterator pointing one beyond the end of the
	 * skip list.
	 */
	const_iterator end() const { return SkipListConstIterator(this, m_NIL); }
	/** Allocate a chunk of the given size from the m_pAllocator member.
	 *
	 * @param size The size of the chunk to allocate.
	 */
	void *allocate(int size) { return m_pAllocator->allocate(size); };
	bool equal_range(monad_m key, const_iterator& cibegin, const_iterator& ciend);
	bool hasObject(monad_m key, id_d_t object_id_d);
	void purge();

 protected:
	int randomLevel();
};






/** A std::list of id_d_t.
 *@internal
 *
 * Used in IDDList.
 *
 * @see class IDDLisz.
 */
typedef std::list<id_d_t> IDDListList;

/** A list of id_d_t values.
 *@internal
 */
class IDDList {
 private:
	IDDListList m_list;
 public:
	typedef IDDListList::const_iterator const_iterator;
	IDDList();
	~IDDList();

	/** Append oid to the list.
	 *
	 * @param oid The id_d to append.
	 */
	void addOid(id_d_t oid) { m_list.push_back(oid); };
	IDDList::const_iterator begin() const;
	IDDList::const_iterator end() const;
	long length() const;
	void makeEmpty();
 private:
	/* Make the copy-constructor and assignment operator
	 * private so that they are not used. */
	IDDList(const IDDList& other);
	IDDList& operator=(const IDDList& other);
};

/** A skip list mapping monad_ms to InstObject pointers.
 *@internal
 *
 * Used in InstContents.
 *
 */
typedef SkipList Monad_m2InstObjectSkipList;

/** The contents of an Inst. A protected API, used in Inst.
 *@internal
 */
class InstContents {
	/** A const_iterator over the underlying SkipList.
	 */
	typedef SkipList::const_iterator const_iterator; 
 private:
	std::vector<std::string> m_feature_names;
	Monad_m2InstObjectSkipList m_sl_by_monad_m;
	bool m_is_aggregate;
	monad_m m_min_monad, m_max_monad;
	bool m_is_frozen; // After this is set to true, you cannot alter the inst
	friend class Inst;
	InstContents::const_iterator myend;
 private:
	InstContents(const std::vector<std::string>& feature_names);
	~InstContents();
	void addObject(InstObject *pObj, monad_m first_monad, monad_m last_monad);
	void objectsAtSm(monad_m Sm, InstContents::const_iterator& ci1);
	void bigUnion(FastSetOfMonads& result);
	void almostRealBigUnion(FastSetOfMonads& result);
	bool hasObject(monad_m key, id_d_t object_id_d) { return m_sl_by_monad_m.hasObject(key, object_id_d); };
	void setIsAggregate(bool isAggregate);
	bool isAggregate(void) const { return m_is_aggregate; };
	InstContents::const_iterator search(monad_m key) const { return m_sl_by_monad_m.search(key); };

	/** Get a const_iterator pointing to the end.
	 *
	 * Is faster if fillingIsDone() has been called.
	 *
	 * @return A const_iterator pointing to the end.
	 */
	InstContents::const_iterator end() const {
		if (m_is_frozen) {
			return myend;
		} else {
			return m_sl_by_monad_m.end();
		}
	};
	void fillingIsDone(void) { m_is_frozen = true; myend = m_sl_by_monad_m.end(); };
};

/** An inst.
 *@internal
 *
 * An inst is a set of objects (InstObject) easily accessible by
 * the first monad of the object.
 *
 * One of the qualities of an Inst is that it is able to look quickly
 * for all objects within a given Universe.
 */
class Inst {
 private:
	SetOfMonads m_current_universe;
	monad_m m_first_monad, m_last_monad;
	InstContents *m_pContents;
	bool m_bDoDelete;
 public:
	typedef InstContents::const_iterator const_iterator; 
	Inst(Inst* pOther);
	Inst(const std::vector<std::string>& feature_names);
	Inst(const std::list<std::string>& feature_names);
	Inst(const std::list<FeatureInfo>& feature_infos);	
 private:
	Inst(const Inst& other);
	Inst& operator=(const Inst& other);
 public:
	~Inst();

	void nextAtHigherKey(Inst::const_iterator& ci); // relative to the current universe
	Inst::const_iterator begin() const; // relative to the current universe
	Inst::const_iterator search(monad_m key) const { return m_pContents->search(key); };

	/** Return a const_iterator pointing one beyond the end of the
	 * current universe.
	 */
	Inst::const_iterator end() const { return m_pContents->end(); }; // end of m_map_by_monad_m

	bool hasObject(monad_m key, id_d_t object_id_d) { return m_pContents->hasObject(key, object_id_d); };

	/** Add an InstObject to the set.
	 * @param pObj The object to add.
	 *
	 * @param first_monad The first monad of the object.
	 *
	 * @param last_monad The last monad of the object.
	 */
	void addObject(InstObject *pObj, monad_m first_monad, monad_m last_monad) { m_pContents->addObject(pObj, first_monad, last_monad); };

	/** Set the current universe.
	 *
	 * @param universe The Universe to use next.
	 */
	void set_current_universe(const SetOfMonads& universe) { m_current_universe = universe; m_first_monad = universe.first(); m_last_monad = universe.last(); };
	/** Get the range of objects for a given monad.
	 *
	 * @param Sm The monad to look for objects at.
	 *
	 * @param ci1 The beginning of the range of objects (if any
	 * have that monad as the first, otherwise it points to one
	 * beyond the end).
	 *
	 * @param ci2 A const_iterator pointing one beyond the end of
	 * the range of objects (if any have that monad as the first,
	 * otherwise it points to one beyond the end).
	 */
	void objectsAtSm(monad_m Sm, Inst::const_iterator& ci1) { m_pContents->objectsAtSm(Sm, ci1); };

	/** Get the "big union" of all the member sets of monads.
	 */
	void bigUnion(FastSetOfMonads& result) { m_pContents->bigUnion(result); };

	/** Get the "almost real big union" of all the member sets of monads.
	 */
	void almostRealBigUnion(FastSetOfMonads& result) { m_pContents->almostRealBigUnion(result); };

	/** Set the "aggregate" bool of this Inst.
	 *
	 * This is a bool to tell the difference between an ephemeral
	 * Inst, transient while executing a query (false), and one
	 * that has been computed once, at the start of a given query,
	 * and which is to stay around until the end of the query
	 * execution (true).
	 *
	 * @param isAggregate Should be true if and only if this Inst
	 * is to be sticking around until the end of the query
	 * execution.
	 */
	void setIsAggregate(bool isAggregate) { m_pContents->setIsAggregate(isAggregate); };

	/** Returns true if setIsAggregate(true) has been called.
	 */
	bool isAggregate(void) const { return m_pContents->isAggregate(); };

	/** Tell the datastructure that we are done filling it with objects.
	 */
	void fillingIsDone(void) { m_pContents->fillingIsDone(); };
	/** Allocate a chunk from the arena.
	 */
	void *allocate(int size) { return m_pContents->m_sl_by_monad_m.allocate(size); };
};




#endif // INST__H__
