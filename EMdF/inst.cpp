/*
 * inst.cpp
 *
 * inst(T,U) class
 *
 * Ulrik Petersen
 * Created: 2/26-2001
 * Last update: 1/6-2014
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


/**@file inst.cpp
 *@brief Implementation of Inst and related classes (EMdF layer)
 */


#include <ctime>
#include <debug.h>
#include <inst.h>
#include <logging.h>


////////////////////////////////////////////////////////////
//
// IDDList
//
////////////////////////////////////////////////////////////
/** Constructor.
 *
 * Nothing special is done.
 */
IDDList::IDDList() : m_list()
{
}

/** Destructor.
 *
 * Nothing special is done.
 */
IDDList::~IDDList()
{
}

/** Returns a const_iterator pointing to the beginning of the list.
 */
IDDList::const_iterator IDDList::begin() const
{
	return m_list.begin();
}

/** Returns a const_iterator pointing one beyond the end of the list.
 */
IDDList::const_iterator IDDList::end() const
{
	return m_list.end();
}

/** Return the size (length) of the list.
 */
long IDDList::length() const
{
	return m_list.size();
}

/** Erase all members of the list.
 */
void IDDList::makeEmpty()
{
	m_list.erase(m_list.begin(), m_list.end());
}


////////////////////////////////////////////////////////////
//
// InstContents
//
////////////////////////////////////////////////////////////
/** Constructor.
 *
 * @param feature_names A list of the names of the features in each
 * InstObject.
 */
InstContents::InstContents(const std::vector<std::string>& feature_names)
	: m_feature_names(feature_names), 
	  m_is_aggregate(false), 
	  m_min_monad(MAX_MONAD),
	  m_max_monad(0),
	  m_is_frozen(false)
{
}

/** Destructor. 
 *
 * Calls m_sl_by_monad_m.purge().
 */
InstContents::~InstContents()
{
	// Delete all InstObject objects
	m_sl_by_monad_m.purge();
}

/** Add an InstObject to the set.
 *
 * Assumes it is not there!
 *
 * @param pObj The object to add.
 *
 * @param first_monad The first monad of the object.
 *
 * @param last_monad The last monad of the object.
 */
void InstContents::addObject(InstObject *pObj, monad_m first_monad, monad_m last_monad)
{
	// We must not add after it is frozen
	ASSERT_THROW(!m_is_frozen, "Don't add anything after Inst is frozen!");

	// Set min_monad
	if (m_min_monad > first_monad)
		m_min_monad = first_monad;
      
	// Set max_monad
	if (m_max_monad < last_monad) 
		m_max_monad = last_monad;

	// Add to m_sl_by_monad_m
	m_sl_by_monad_m.insert(first_monad, pObj);
}


/** Get the "big union" of all the sets of monads.
 *
 * Currently, this makes in result a single mse with min..max where
 * min is the minimum monad in ourselves and max is the maximum The
 * reason it is called bigUnion is that it used to be a
 * set-theoretical union of all of our objects' monad-sets.  This took
 * too long, however.  q26.txt in particular went from 11.5 minutes to
 * 1.5 minutes just by implementing this with just min..max (written
 * in 2002).
 *
 * @param result The resulting SetOfMonads.
 */
void InstContents::bigUnion(FastSetOfMonads& result)
{
	// Write to log
	LOG_WRITE_TIME("InstContents::bigUnion", "Starting big-union.");

	// Set to the empty set
	result.clear();
  
	/*
	// Iterate through map
	Monad_m2InstObjectMap::const_iterator ci(m_map_by_id_d.begin());
	Monad_m2InstObjectMap::const_iterator cend(m_map_by_id_d.end());
	monad_m min_monad, max_monad;
	min_monad = MAX_MONAD;
	max_monad = 0;
	while (ci != cend) {
	// Get first and last monad of object
	const SetOfMonads& rsom = ci->second->getMonads();
	monad_m first_m = rsom.first();
	monad_m last_m = rsom.last();
    
	// Set min_monad
	min_monad = (min_monad > first_m) ? first_m : min_monad;

	// Set max_monad
	max_monad = (max_monad < last_m) ? last_m : max_monad;

	// Advance iterator
	++ci;
	}
	*/


	// Add m_min_monad .. m_max_monad, but only if there were
	// some members in the m_map_by_id_d.
	// If there were no members, we never got the chance to 
	// set m_min_monad and m_max_monad, and we would get
	// a BadMonadsException by adding the mse (MAX_MONAD, 0).
	if (!(m_min_monad == MAX_MONAD && m_max_monad == 0)) {
		result.add(m_min_monad, m_max_monad);
	}

	// Write to log
	LOG_WRITE_TIME("InstContents::bigUnion", "Big-union finished.");
}

/** Get the "big union" of all the sets of monads.
 *
 * @param result The resulting SetOfMonads.
 */
void InstContents::almostRealBigUnion(FastSetOfMonads& result)
{
	// Write to log
	LOG_WRITE_TIME("InstContents::almostRealBigUnion", "Starting almost real big-union.");

	// Set to the empty set
	result.clear();
  
	// Find the start of the sequence
	InstContents::const_iterator ci = m_sl_by_monad_m.begin();

	// If key was greater than the greatest entry, 
	while (ci.hasNext()) {
		const InstObject *pObj = ci.next();
		monad_m first_monad = pObj->first();
		monad_m last_monad = pObj->last();
		result.add(first_monad, last_monad);
	}

	// Write to log
	LOG_WRITE_TIME("InstContents::almostRealBigUnion", "Almost real big-union finished.");
}


//
// Returns in ci1 and ci2 the beginning and end of a series of objects
// that have the starting monad Sm.
// If the list is nonempty, ci2 points to the element just after the 
// last element.
// If there are no objects starting at Sm, both ci1 and ci2 are end()
//
void InstContents::objectsAtSm(monad_m Sm, InstContents::const_iterator& ci1)
{
	ci1 = m_sl_by_monad_m.search(Sm);
	if (ci1.currentKey() != Sm) {
		ci1 = end();
	}
	return;
}


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
void InstContents::setIsAggregate(bool isAggregate) 
{ 
	m_is_aggregate = isAggregate; 
}



/////////////////////////////////////////////////////////////////
//
// Inst
//
/////////////////////////////////////////////////////////////////

/** Constructor from another Inst.
 *
 * This simply sets m_pContents to pOther->m_pContents, and makes sure
 * we don't delete the m_pContents once we are destructed.
 *
 * @param pOther The other Inst whose InstContents we are to borrow.
 */
Inst::Inst(Inst* pOther)
{
	m_pContents = pOther->m_pContents;
	m_bDoDelete = false;
}

/** Constructor: Create an empty Inst.
 *
 * @param feature_names The names of the features in each InstObject
 * (if any).
 *
 */
Inst::Inst(const std::vector<std::string>& feature_names)
{
	m_pContents = new InstContents(feature_names);
	m_bDoDelete = true;
}

/** Destructor.
 */
Inst::~Inst()
{
	if (m_bDoDelete)
		delete m_pContents;
}


/** Return a const_iterator pointing to teh beginning of the current universe.
 */
Inst::const_iterator Inst::begin() const // relative to the current universe
{
	// Find beginning of universe
	Inst::const_iterator ci(m_pContents->m_sl_by_monad_m.search(m_first_monad));
	while (true) {
		// If we hit the end, return with end()
		if (!ci.hasNext()) {
			break;
		}

		// Is this set of monads within the range?
		if (ci.current()->last() <= m_last_monad) {
			// Yes, break out of loop
			break; 
		} else {
			// No

			// Advance iterator
			ci.next();

			// Is this not the end?
			if (ci.hasNext()) {
				// Yes, this is not the end

				// Is there no hope because this iterator points to an object
				// whose first monad is greater than m_last_monad?
				if (ci.currentKey() > m_last_monad) {
					// Yes, set ci to the end of the map
					ci = m_pContents->end();
				}
			}
		}
	}
	return ci;
}

/** Advance the iterator to the first object that has a higher key
 * than the current object at the iterator.
 *
 * @param ci The iterator to use and advance.
 */
void Inst::nextAtHigherKey(Inst::const_iterator& ci) // relative to the current universe
{
	// If we have already hit the end, return ci
	if (!ci.hasNext())
		return;

	monad_m first = ci.advanceToNextKey();

	// Is there no hope?
	if (first > m_last_monad) {
		// Yes, there is no hope.  Set ci to the end and return
		ci = m_pContents->end();
		return;
	} 
}


////////////////////////////////////////////////////////////////////
//
// SkipListConstIterator
//
////////////////////////////////////////////////////////////////////

/** Construct a const_iterator over a SkipList.
 */
SkipListConstIterator::SkipListConstIterator(const SkipList *pMotherSkipList)
{
	m_pMotherSkipList = pMotherSkipList;
	m_iter = GET_NEXT(m_pMotherSkipList->m_head);
}

SkipListConstIterator::SkipListConstIterator(const SkipList *pMotherSkipList, SkipListNode *pIter)
{
	m_pMotherSkipList = pMotherSkipList;
	m_iter = pIter;
}

SkipListConstIterator::SkipListConstIterator()
{
	m_pMotherSkipList = 0;
	m_iter = 0;
}

SkipListConstIterator::SkipListConstIterator(const SkipListConstIterator& other)
{
	m_pMotherSkipList = other.m_pMotherSkipList;
	m_iter = other.m_iter;
}

SkipListConstIterator& SkipListConstIterator::operator=(const SkipListConstIterator& other) 
{
	m_pMotherSkipList = other.m_pMotherSkipList;
	m_iter = other.m_iter;
	return *this;
}

// Is the iterator == end iterator?  Doesn't alter iterator
bool SkipListConstIterator::hasNext() const 
{
	ASSERT_THROW(m_pMotherSkipList != 0, "m_pMotherSkipList is 0");
	return m_iter != m_pMotherSkipList->m_NIL;
}

// Gets current and advances iterator afterwards
const InstObject *SkipListConstIterator::next() 
{
	ASSERT_THROW(m_pMotherSkipList != 0, "m_pMotherSkipList is 0");
	ASSERT_THROW(hasNext(), "hasNext() is false");
	const InstObject *pValue = m_iter->m_pValue;
	m_iter = GET_NEXT(m_iter);
	return pValue;
}

// Returns 0 on hasNext() has been reached;
// otherwise, returns the monad of the next key.
monad_m SkipListConstIterator::advanceToNextKey()
{
	ASSERT_THROW(m_pMotherSkipList != 0, "mother skip list is 0");
	// This is equivalent to hasNext()
	ASSERT_THROW(m_iter != m_pMotherSkipList->m_NIL, "iter is at end");
	monad_m current_monad = GET_KEY(m_iter);
	while (true) {
		m_iter = GET_NEXT(m_iter);

		// This is equivalent to hasNext()
		if (m_iter != m_pMotherSkipList->m_NIL) {
			monad_m first = GET_KEY(m_iter);
			if (first > current_monad) {
				// Return if we have reached goal.
				return first;
			}
		} else {
			// Return if we don't have next
			return 0;
		}
	}
}

// Gets current without altering iterator
const InstObject *SkipListConstIterator::current() 
{
	ASSERT_THROW(m_pMotherSkipList != 0, "mother skip list is 0");
	return m_iter->m_pValue;
}

monad_m SkipListConstIterator::currentKey() 
{
	ASSERT_THROW(m_pMotherSkipList != 0, "mother skip list is 0");
	return GET_KEY(m_iter);
}


////////////////////////////////////////////////////////////////////
//
// SkipList
//
////////////////////////////////////////////////////////////////////


/** The maximum number of levels to have in a SkipListNode.
 */
#define MAX_NUMBER_OF_LEVELS (18)

/** The index of the highest level.
 */
#define MAX_LEVEL (MAX_NUMBER_OF_LEVELS - 1)




/** How many bits do we use from the random number?
 *@internal
 */
#define BITS_TO_USE_FROM_RANDOM (30)

// 
/** Use Arena allocator to get memory for node 
 *@internal
 *
 *@param maximum_level The maximum number of levels to be used with
 *this skip list node.
 */
#define NEW_CSKIPLISTNODE(maximum_level) ((PCSkipListNode) m_pAllocator->allocate(sizeof(SkipListNode) + sizeof(PCSkipListNode)*maximum_level))

/** Delete skip list node
 *@internal
 *
 * currently doesn't do anything, since everything is cleared in one go
 * when the arena is cleared.
 *
 * It is left here in case I ever want to change the deletion policy.
 *
 * @param n The node to clear.
 */
#define DELETE_CSKIPLISTNODE(n) 



SkipList::SkipList()
{
	// Get random seed from system clock
	// m_rand = (unsigned long) time(NULL);

	// Choose an arbitrary, but carefully chosen, number...
	m_rand = 278081371UL;
  
	// Create allocator
	m_pAllocator = new Arena();
  
	// Make m_NIL
	m_NIL = NEW_CSKIPLISTNODE(0);

	// The destructor of InstObject assumes that the array of
	// EMdFValue's has been allocated with Arena.  Since we
	// pass a nil array with length 0, this is OK here.
	m_NIL->m_pValue = new InstObject(NIL, 0x7fffffff, 0x7fffffff, 0, 0);
	m_NIL->m_forward[0] = 0;
  
	m_randomBits = random();
	m_randomsLeft = BITS_TO_USE_FROM_RANDOM / 3;
  
	m_level = 0;
  
	// Create head with maximum number of levels, 
	// and set all forward pointers to m_NIL;
	m_head = NEW_CSKIPLISTNODE(MAX_NUMBER_OF_LEVELS);
	for (int i = 0; i < MAX_NUMBER_OF_LEVELS; ++i) {
		m_head->m_forward[i] = m_NIL;
	}
	m_head->m_pValue = 0;
}


SkipList::~SkipList() 
{
	purge();
	delete m_NIL->m_pValue;
	DELETE_CSKIPLISTNODE(m_NIL);
	DELETE_CSKIPLISTNODE(m_head);
	delete m_pAllocator;
}

inline bool cmp_SkipListNode(monad_m key, InstObject *pValue, SkipListNode *q)
{
	volatile monad_m node_first = GET_KEY(q);
	if (node_first < key) {
		return true;
	} else if (node_first > key) {
		return false;
	} else {
		return GET_ID_D(q) < pValue->getID_D();
	}
}

// This is how many bits random() returns.
void SkipList::insert(monad_m key, InstObject *pValue) 
{
	int k;
	SkipListNode *update[MAX_NUMBER_OF_LEVELS];
	SkipListNode *p, *q;
	p = m_head;
	k = m_level;
	do {
		//while (q = p->m_forward[k], GET_KEY(q) < key) p = q;
		while (q = p->m_forward[k], cmp_SkipListNode(key, pValue, q)) p = q;
		update[k] = p;
	} while (--k >= 0);

	k = randomLevel();
	if (k > m_level) {
		k = ++m_level;
		update[k] = m_head;
	}
	q = NEW_CSKIPLISTNODE(k);
	q->m_pValue = pValue;
	do {
		p = update[k];
		q->m_forward[k] = p->m_forward[k];
		p->m_forward[k] = q;
	} while (--k >= 0);
}

SkipList::const_iterator SkipList::search(monad_m key) const 
{
	int k;
	SkipListNode *p, *q;
	p = m_head;
	k = m_level;
	do {
		while (q = p->m_forward[k], GET_KEY(q) < key) p = q;
	} while (--k >= 0);
	return SkipListConstIterator(this, q);
}

// Finds
bool SkipList::equal_range(monad_m key, SkipList::const_iterator& cibegin, SkipList::const_iterator& ciend) 
{
	// Find the start of the sequence
	cibegin = search(key);

	// If key was greater than the greatest entry, 
	// return cibegin = end(), ciend = end().
	if (!cibegin.hasNext()) {
		ciend = end();
		return false;
	}
    
	// If the key was not key, 
	// return cibegin = end(), ciend = end().
	if (GET_KEY(cibegin.m_iter) != key) {
		cibegin = ciend = end();
		return false;
	}

	// Find the end of the range
	// FIXME: Could we use search(key+1) ?  
	// That would be O(log(N)) rather than O(N).
	// Perhaps even a variant of search() that takes
	// an iterator (viz., cibegin) and starts from there.
	ciend = cibegin;
	while (ciend.hasNext() && GET_KEY(ciend.m_iter) == key) {
		ciend.next();
	}

	return true;
}

// Returns true if object with first monad key and object id_d
// object_id_d is present, false otherwise.
bool SkipList::hasObject(monad_m key, id_d_t object_id_d)
{
	// Find the start of the sequence
	SkipList::const_iterator cibegin = search(key);

	// If key was greater than the greatest entry, 
	if (!cibegin.hasNext()) {
		return false;
	}
    
	// If the key was not present
	if (GET_KEY(cibegin.m_iter) != key) {
		return false;
	}

	// Go towards the end of the range
	while (cibegin.hasNext() && GET_KEY(cibegin.m_iter) == key) {
		const InstObject *pValue = cibegin.next();
		if (pValue->getID_D() == object_id_d) {
			return true;
		}
	}

	return false;
}

void SkipList::purge() 
{
	SkipListNode *p, *q;
	p = GET_NEXT(m_head);
	while (p != m_NIL) {
		q = GET_NEXT(p);
		p->m_pValue->~InstObject();
		DELETE_CSKIPLISTNODE(p);
		p = q;
	} 
	// Set all forward pointers in head to m_NIL;
	for (int i = 0; i < MAX_NUMBER_OF_LEVELS; ++i) {
		m_head->m_forward[i] = m_NIL;
	}
	m_level = 0;
}


int SkipList::randomLevel() 
{
	int level = 0;
	unsigned int b;
	do {
		b = m_randomBits & 0x00000001;
		if (!b) {
			++level;
		}
		m_randomBits >>= 1;
		--m_randomsLeft;
		if (m_randomsLeft == 0) {
			m_randomBits = random();
			m_randomsLeft = BITS_TO_USE_FROM_RANDOM / 1;
		}
	} while (!b);
	return (level > MAX_LEVEL) ? MAX_LEVEL : level;
}

