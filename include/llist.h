/*
 * llist.h
 *
 * Linked list templates
 *
 * Ulrik Petersen
 * Created: Sometime in 1998
 * Last update: 4/10-2017
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 1998-2017  Ulrik Sandborg-Petersen
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

/**@file llist.h
 *@brief Header file for linked list (EMdF layer)
 */


#ifndef __LList__h__
#define __LList__h__


#include "debug.h"

#include <iostream>

template <class T> class LlistIterator; // Forward declaration
template <class T> class LlistConstIterator; // Forward declaration


template<class T>
class Lnode {
	friend class LlistIterator<T>;
	friend class LlistConstIterator<T>;
 public:
	Lnode(T *value, Lnode<T> *next = 0);
	Lnode();
	~Lnode();
	void setNext(Lnode<T> *n);
	void setContent(T *value);
	Lnode<T> *getNext() const;
	T *getContent() const;
 private:
	T *content;
	Lnode<T> *next;
};

template<class T>
class Llist {
	friend class LlistIterator<T>;
	friend class LlistConstIterator<T>;
 public:
	Llist();
	~Llist();
	LlistIterator<T> iterator();
	LlistConstIterator<T> const_iterator() const;
	void push_back(T *p);
	void prepend(T *p);
	void prependWithMe(Llist<T>* pOther);
	void extendWithMe(Llist<T>* pOther);
	Lnode<T> *insertAfter(Lnode<T> *pIterator, T *p);
	LlistIterator<T> insertBefore(const LlistIterator<T> &iterator, T *p);
	LlistIterator<T> erase(const LlistIterator<T> &to_remove, bool bDeleteElement = false);
	void clear(bool bDeleteContents = false);
	bool lengthIs1() const;
	long size() const;
	bool empty() const { return m_pRoot == 0; };
	T *back() { return m_pTail->getContent(); };
	const Lnode<T> *getConstRoot(void) const { return m_pRoot; };
	Lnode<T> *getRoot(void) { return m_pRoot; };
 private:
	Lnode<T> *m_pRoot, *m_pTail;
	// Make copy-constructor private so that we don't accidentally call it
	Llist(const Llist<T>& other) {
		(void)(other); // Silence a warning
		ASSERT_THROW(false, "You shouldn't call copy-constructor of LList<T>"); };
};

template <class T> class LlistIterator {
 private:
	friend class Llist<T>;
	Llist<T> *m_pMotherLlist;
	Lnode<T> *m_pIter;
 public:
#ifndef SWIG
	LlistIterator(Llist<T> *pMotherLlist);
	LlistIterator(Llist<T> *pMotherLlist, Lnode<T> *pNode);
#endif
	LlistIterator();
	LlistIterator(const LlistIterator<T>& other);
	~LlistIterator();
#ifndef SWIG
	LlistIterator& operator=(const LlistIterator<T>& other);
#endif 
	bool hasNext() const; // Is the iterator == end iterator?  Doesn't alter iterator
	T* next(); // Gets current and advances iterator afterwards
	T* current(); // Gets current without altering iterator
 private:
	void assign(const LlistIterator<T>& other);
};


template <class T> class LlistConstIterator {
 private:
	friend class Llist<T>;
	const Llist<T> *m_pMotherLlist;
	const Lnode<T> *m_pIter;
 public:
#ifndef SWIG
	LlistConstIterator(const Llist<T> *pMotherLlist);
#endif
	LlistConstIterator();
	LlistConstIterator(const LlistConstIterator<T>& other);
	~LlistConstIterator();
#ifndef SWIG
	LlistConstIterator& operator=(const LlistConstIterator<T>& other);
#endif 
	bool hasNext() const; // Is the iterator == end iterator?  Doesn't alter iterator
	const T* next(); // Gets current and advances iterator afterwards
	const T* current(); // Gets current without altering iterator
 private:
	void assign(const LlistConstIterator<T>& other);
};


//
// Value-based linked lists
// 
// Note: These lists use exactly the same memory as
// std::list<value>, that is, 8 bytes per node + whatever
// the value takes, as evidenced by the following program.
//
// Both std::list and LVlist take 800MB (std::list 801MB).
//
// std::vector takes 400MB.  However, using std::vector
// in SetOfMonads is not an advantage: The speed decrease
// is about 100%.
//
// Whether using this list in SetOfMonads would be
// a _speed_ advantage, I don't know.
//
// #include <list>
// #include <iostream>
// #include <unistd.h>
// #include <monads.h>
// #include <llist.h>
// #include <vector>
// 
// typedef LVlist<MonadSetElement> CMSEList;
// 
// #define INNER  (1024*1024)
// #define OUTER  (50)
// 
// int main(int argc, char *argv[])
// {
//   if (argc == 1) {
//     std::list<MonadSetElement> mylist;
//     for (int i = 0; i < OUTER; ++i) {
//       for (int i2 = 0; i2 < INNER; ++i2) {
// 	mylist.push_back(MonadSetElement(0,3));
//       }
//     }
//   } else {
//     if (std::string(argv[1]) == std::string("1")) {
//       CMSEList mylist;
//       for (int i = 0; i < OUTER; ++i) {
// 	for (int i2 = 0; i2 < INNER; ++i2) {
// 	  mylist.push_back(MonadSetElement(0,3));
// 	}
//       }
//       mylist.clear();
//     } else {
//       std::vector<MonadSetElement> myvec;
//       for (int i = 0; i < OUTER; ++i) {
// 	for (int i2 = 0; i2 < INNER; ++i2) {
// 	  myvec.push_back(MonadSetElement(0,3));
// 	}
//       }
//     }
//   }
//   sleep(4);
//   return 0;
// }
//

template <class T> class LVlistIterator; // Forward declaration
template <class T> class LVlistConstIterator; // Forward declaration

template<class T>
class LVnode {
	friend class LVlistIterator<T>;
	friend class LVlistConstIterator<T>;
 public:
	LVnode(const T& value, LVnode<T> *next = 0);
	LVnode();
 private:
	LVnode(const LVnode& other);
	LVnode& operator=(const LVnode& other);
 public:
	~LVnode();
	void setNext(LVnode<T> *n);
	void setContent(const T& value);
	LVnode<T> *getNext() const;
	T& getContent();
	const T& getContent() const;
 private:
	LVnode<T> *next;
	T content;
};

template<class T>
class LVlist {
	friend class LVlistIterator<T>;
	friend class LVlistConstIterator<T>;
 public:
	LVlist();
 private:
	// Leave unimplemented, so as to prevent the compiler from
	// generating crashing ones.
	LVlist(const LVlist& other);
	LVlist& operator=(const LVlist& other);
 public:
	~LVlist();
	LVlistIterator<T> iterator();
	LVlistConstIterator<T> const_iterator() const;
	void push_back(const T& p);
	void prepend(const T& p);
	void prependWithMe(LVlist<T>* pOther);
	void extendWithMe(LVlist<T>* pOther);
	LVlistIterator<T> erase(const LVlistIterator<T>& to_remove);
	LVnode<T> *insertAfter(LVnode<T> *pIterator, const T& p);
	LVlistIterator<T> insertAfter(const LVlistIterator<T> iterator, const T& p);
	LVlistIterator<T> insertBefore(const LVlistIterator<T>& iterator, const T& p);
	void clear();
	long size() const;
	bool lengthIs1() const;
	bool empty() const { return m_pRoot == 0; };
	const LVnode<T> *getConstRoot(void) const { return m_pRoot; };
	LVnode<T> *getRoot(void) { return m_pRoot; };
	T& front(void) { ASSERT_THROW(m_pRoot != 0, "m_pRoot is 0"); return m_pRoot->getContent(); };
	T& back(void) { ASSERT_THROW(m_pTail != 0, "m_pTail is 0"); return m_pTail->getContent(); };
 private:
	LVnode<T> *m_pRoot, *m_pTail;
};

/** Non-constant iterator over a linked list of values.
 *
 * The values iterated over can be modified, and so can the list.
 *
 * \see LVlistConstIterator, with which it contrasts.
 */
template <class T> class LVlistIterator {
 private:
	friend class LVlist<T>;
	LVlist<T> *m_pMotherLVlist;
	LVnode<T> *m_pIter;
 public:
#ifndef SWIG
	LVlistIterator(LVlist<T> *pMotherLVlist);
	LVlistIterator(LVlist<T> *pMotherLVlist, LVnode<T> *pNode);
#endif
	LVlistIterator();
	LVlistIterator(const LVlistIterator<T>& other);
	~LVlistIterator();
#ifndef SWIG
	LVlistIterator& operator=(const LVlistIterator<T>& other);
#endif 
	bool hasNext() const; // Is the iterator == end iterator?  Doesn't alter iterator
	T& next(); // Gets current and advances iterator afterwards
	T& current(); // Gets current without altering iterator
 private:
	void assign(const LVlistIterator<T>& other);
};


/** Constant iterator over a linked list of values.
 *
 * The values iterated over cannot be modifed, and neither can the
 * list.
 *
 * \see LVlistIterator, with which it contrasts.
 */
template <class T> class LVlistConstIterator {
 private:
	friend class LVlist<T>;
	const LVlist<T> *m_pMotherLVlist;
	const LVnode<T> *m_pIter;
 public:
#ifndef SWIG
	LVlistConstIterator(const LVlist<T> *pMotherLVlist);
#endif
	LVlistConstIterator();
	LVlistConstIterator(const LVlistConstIterator<T>& other);
	~LVlistConstIterator();
#ifndef SWIG
	LVlistConstIterator& operator=(const LVlistConstIterator<T>& other);
#endif 
	bool hasNext() const; // Is the iterator == end iterator?  Doesn't alter iterator
	const T& next(); // Gets current and advances iterator afterwards
	const T& current(); // Gets current without altering iterator
 private:
	void assign(const LVlistConstIterator<T>& other);
};




//
// Lnode implementation
//

template<class T> Lnode<T>::Lnode()
{
	content = next = 0;
}

template<class T> Lnode<T>::Lnode(T *value, Lnode<T> *n)
{
	content = value;
	next = n;
}

template<class T> Lnode<T>::~Lnode()
{
	next = 0;
}

template<class T> void Lnode<T>::setNext(Lnode<T> *n) 
{
	next = n;
}

template<class T> void Lnode<T>::setContent(T *value) 
{
	content = value;
}

template<class T> Lnode<T> *Lnode<T>::getNext() const
{
	return next;
}

template<class T> T *Lnode<T>::getContent() const
{
	return content;
}




//
// Llist implementation
//

template<class T> Llist<T>::Llist()
: m_pRoot(0),
	m_pTail(0)
{
}

template<class T> Llist<T>::~Llist()
{
	// We assume we've cleared it!

	// We can't throw an exception, since C++11 declares d'tors
	// noexcept.
	
	// ASSERT_THROW(m_pRoot == 0, "We hadn't cleared the list!");
}

template<class T> LlistIterator<T> Llist<T>::iterator()
{
	return LlistIterator<T>(this);
}


template<class T> LlistConstIterator<T> Llist<T>::const_iterator() const
{
	return LlistConstIterator<T>(this);
}

template<class T> bool Llist<T>::lengthIs1() const
{
	if (m_pRoot == 0) {
		return false;
	} else {
		return m_pRoot->getNext() == 0;
	}
}



template<class T> void Llist<T>::push_back(T *p)
{
	Lnode<T> *newcomer = new Lnode<T>(p, 0);
	if (m_pRoot == 0) {
		m_pRoot = m_pTail = newcomer;
	} else {
		m_pTail->setNext(newcomer);
		m_pTail = newcomer;
	}
}

// Prepends my own to the beginning of pOther,
// and sets myself to empty.
template<class T> void Llist<T>::prependWithMe(Llist<T>* pOther)
{
	// Only do something if I am not empty
	if (m_pRoot != 0) {
		if (pOther->m_pRoot == 0) {
			pOther->m_pRoot = m_pRoot;
			pOther->m_pTail = m_pTail;
		} else {
			m_pTail->setNext(pOther->m_pRoot);
			pOther->m_pRoot = m_pRoot;
		}
		// Now set myself to empty
		m_pRoot = m_pTail = 0;
	}
}

// Appends my own to the end of pOther,
// and sets myself to empty.
template<class T> void Llist<T>::extendWithMe(Llist<T>* pOther)
{
	// Only do something if I am not empty.
	if (m_pRoot != 0) {
		// If the ther is empty, it's simple
		if (pOther->m_pRoot == 0) {
			pOther->m_pRoot = m_pRoot;
			pOther->m_pTail = m_pTail;
		} else {
			pOther->m_pTail->setNext(m_pRoot);
			pOther->m_pTail = m_pTail;
		}
		// Now set myself to empty
		m_pRoot = m_pTail = 0;
	}
}

template<class T> void Llist<T>::prepend(T *p)
{
	Lnode<T> *newcomer = new Lnode<T>(p, 0);
	if (!m_pRoot) {
		m_pRoot = m_pTail = newcomer;
	} else {
		newcomer->setNext(m_pRoot);
		m_pRoot = newcomer;
	}
}

// Inserts p after pIterator.
// Returns the node newly created.
template<class T> Lnode<T> *Llist<T>::insertAfter(Lnode<T> *pIterator, T *p)
{
	Lnode<T> *newcomer = new Lnode<T>(p, 0);
	Lnode<T> *next = pIterator->getNext();
	newcomer->setNext(next);
	pIterator->setNext(newcomer);
	if (pIterator == m_pTail) {
		m_pTail = newcomer;
	}
	return newcomer;
}

// Inserts p before iterator.
// Returns the iterator for the node newly created
template <class T> LlistIterator<T> Llist<T>::insertBefore(const LlistIterator<T>& iterator, T *p)
{
	Lnode<T> *newcomer = new Lnode<T>(p, 0);
	Lnode<T> *next = iterator.m_pIter;
	newcomer->setNext(next);
	// Are we to insert before one past the end?
	if (next == 0) {
		// Yes.

		// So is the list empty?
		if (m_pRoot == 0) {
			// Yes, empty.
			m_pRoot = m_pTail = newcomer;
		} else {
			// No, not empty, so we just update the tail.
			m_pTail->setNext(newcomer);
			m_pTail = newcomer;
		}
	} else {
		// The thing to insert before wasn't the one after the tail

		// So are we inserting before the root?
		if (next == m_pRoot) {
			// Yes.
			// Move root one backwards.
			// Don't bother about the tail: It should stay as it is.
			m_pRoot = newcomer;

			// We have already set newcomer->next to be the root, above.
		} else {
			// The thing to insert before wasn't the root, nor one past the tail

			// Find the node before the thing before which to insert
			Lnode<T> *pIterator = m_pRoot;
			Lnode<T> *pIteratorMinus1 = 0;
			while (pIterator != next) {
				pIteratorMinus1 = pIterator;
				pIterator = pIterator->getNext();
			}

			// Set previous's next to newcomer
			pIteratorMinus1->setNext(newcomer);

			// We have already set newcomer->next to be "next", above
		}
	}

	// Construct return value with newcomer
	// as the iterator
	LlistIterator<T> return_iterator(this, newcomer);

	// Return value
	return return_iterator;
}



// Removes the element pointed to by to_delete.
// Returns the iterator that points one beyond to_delete.
// This is like STL's std::list::erase().
template <class T> LlistIterator<T> Llist<T>::erase(const LlistIterator<T>& to_remove, bool bDeleteElement)
{
	// We must have length >= 1.
	ASSERT_THROW(m_pRoot != 0, "length == 0");

	// Get node to remove
	Lnode<T> *pNodeToRemove = to_remove.m_pIter;
	ASSERT_THROW(pNodeToRemove != 0, "no node to remove");

	// Find node after the thing to be removed
	Lnode<T> *pNodeAfterRemoved = pNodeToRemove->getNext();

	if (pNodeToRemove == m_pRoot) {
		if (lengthIs1()) {
			m_pRoot = m_pTail = 0;
		} else {
			m_pRoot = pNodeAfterRemoved;
		}
		if (bDeleteElement) {
			delete pNodeToRemove->getContent();
		}
		delete pNodeToRemove;
	} else {
		// The thing to remove wasn't the root.

		// Find the node before the thing to be removed
		Lnode<T> *pIterator = m_pRoot;
		Lnode<T> *pIteratorMinus1 = 0;
		while (pIterator != pNodeToRemove) {
			pIteratorMinus1 = pIterator;
			pIterator = pIterator->getNext();
		}

		// Set previous's next to the one after
		pIteratorMinus1->setNext(pNodeAfterRemoved);

		// If the thing to remove was the tail,
		// we should update the tail to be the one before it.
		if (m_pTail == pNodeToRemove) {
			m_pTail = pIteratorMinus1;
		}

		// delete the one if necessary
		if (bDeleteElement) {
			delete pNodeToRemove->getContent();
		}
		delete pNodeToRemove;
	}

	// Construct return value with pNodeAfterDeleted
	// as the iterator
	LlistIterator<T> return_iterator(this, pNodeAfterRemoved);

	// Return value
	return return_iterator;
}



template<class T> void Llist<T>::clear(bool bDeleteContents)
{
	Lnode<T> *current = m_pRoot;

	while (current) {
		Lnode<T> *next = current->getNext();
		if (bDeleteContents) {
			delete current->getContent();
		}
		delete current;
		current = next;
	}
	m_pRoot = m_pTail = 0;
}

template<class T> long Llist<T>::size() const
{
	long result = 0;

	Lnode<T> *current = m_pRoot;
	Lnode<T> *next;

	while (current != 0) {
		next = current->getNext();

		++result;

		current = next;
	}
	return result;
}




//
// LlistIterator implementation
//

template<class T> LlistIterator<T>::LlistIterator(Llist<T> *pMotherLlist)
: m_pMotherLlist(pMotherLlist),
	m_pIter(0)
{
	if (m_pMotherLlist != 0) {
		m_pIter = m_pMotherLlist->m_pRoot;
	} else {
		m_pIter = 0;
	}
}

template<class T> LlistIterator<T>::LlistIterator(Llist<T> *pMotherLlist, Lnode<T> *pNode)
: m_pMotherLlist(pMotherLlist),
	m_pIter(pNode)
{
}

/** Constructor.
 *
 * The only constructor which it is safe for the user to call.
 */
template<class T> LlistIterator<T>::LlistIterator()
:  m_pMotherLlist(0),
	m_pIter(0)
{
}

template<class T> LlistIterator<T>::LlistIterator(const LlistIterator<T>& other)
{
	assign(other);
}


/** Destructor
 */
template<class T> LlistIterator<T>::~LlistIterator()
{
}

template<class T> LlistIterator<T>& LlistIterator<T>::operator=(const LlistIterator<T>& other)
{
	assign(other);
	return *this;
}

	template<class T> void LlistIterator<T>::assign(const LlistIterator<T>& other)
{
	m_pMotherLlist = other.m_pMotherLlist;
	m_pIter = other.m_pIter;
}


/** Does the linked list have more elements?
 *
 * @return true if there are more elements, false if we have reached
 * the end.  <b>DO NOT</b> call next() if hasNext() returns false!
 */
template<class T> bool LlistIterator<T>::hasNext() const
{
	if (m_pMotherLlist == 0) {
		return false; 
	} else {
		return m_pIter != 0;
	}
}


/** Get the current element and advance iterator
 *
 * Only call if hasNext() returns true.
 *
 * @return A pointer to the current element.
 */
template<class T> T* LlistIterator<T>::next()
{
	// This is redundant, as it is already checked in hasNext()
	// ASSERT_THROW(m_pMotherLlist != 0, "m_pMotherList was 0");
	ASSERT_THROW(hasNext(), "hasNext() is false");
	T* TResult = m_pIter->content;
	m_pIter = m_pIter->next;
	return TResult;
}

/** Get current value and do not touch iterator.
 *
 * @return A constant reference to the current value.
 */
template<class T> T* LlistIterator<T>::current()
{
	ASSERT_THROW(m_pMotherLlist != 0, "m_pMotherList is 0, i.e., not set");
	return m_pIter->content;
}




//
// LlistConstIterator implementation
//

template<class T> LlistConstIterator<T>::LlistConstIterator(const Llist<T> *pMotherLlist)
: m_pMotherLlist(pMotherLlist),
	m_pIter(0)
{
	if (m_pMotherLlist != 0) {
		m_pIter = m_pMotherLlist->m_pRoot;
	} else {
		m_pIter = 0;
	}
}

/** Constructor.
 *
 * The only constructor which it is safe for the user to call.
 */
template<class T> LlistConstIterator<T>::LlistConstIterator()
:  m_pMotherLlist(0), m_pIter(0)
{
}

template<class T> LlistConstIterator<T>::LlistConstIterator(const LlistConstIterator<T>& other)
{
	assign(other);
}


/** Destructor
 */
template<class T> LlistConstIterator<T>::~LlistConstIterator()
{
}

template<class T> LlistConstIterator<T>& LlistConstIterator<T>::operator=(const LlistConstIterator<T>& other)
{
	assign(other);
	return *this;
}

	template<class T> void LlistConstIterator<T>::assign(const LlistConstIterator<T>& other)
{
	m_pMotherLlist = other.m_pMotherLlist;
	m_pIter = other.m_pIter;
}


/** Does the linked list have more elements?
 *
 * @return true if there are more elements, false if we have reached
 * the end.  <b>DO NOT</b> call next() if hasNext() returns false!
 */
template<class T> bool LlistConstIterator<T>::hasNext() const
{
	if (m_pMotherLlist == 0) {
		return false; 
	} else {
		return m_pIter != 0;
	}
}


/** Get the current element and advance iterator
 *
 * Only call if hasNext() returns true.
 *
 * @return A const pointer to the current element.
 */
template<class T> const T* LlistConstIterator<T>::next()
{
	// This is redundant, as it is already checked in hasNext()
	// ASSERT_THROW(m_pMotherLlist != 0, "m_pMotherList is 0");
	ASSERT_THROW(hasNext(), "hasNext() is false");
	T* TResult = m_pIter->content;
	m_pIter = m_pIter->next;
	return TResult;
}

/** Get current value and do not touch iterator.
 *
 * @return A constant reference to the current value.
 */
template<class T> const T* LlistConstIterator<T>::current()
{
	ASSERT_THROW(m_pMotherLlist != 0, "m_pMotherList is 0, i.e., not set");
	return m_pIter->content;
}

//
// LVnode implementation
//

template<class T> LVnode<T>::LVnode()
{
	content = next = 0;
}

template<class T> LVnode<T>::LVnode(const T& value, LVnode<T> *n) : next(n), content(value)
{
}

template<class T> LVnode<T>::~LVnode()
{
	next = 0;
}

template<class T> void LVnode<T>::setNext(LVnode<T> *n) 
{
	next = n;
}

template<class T> void LVnode<T>::setContent(const T& value) 
{
	content = value;
}

template<class T> LVnode<T> *LVnode<T>::getNext() const
{
	return next;
}

template<class T> const T& LVnode<T>::getContent() const
{
	return content;
}

template<class T> T& LVnode<T>::getContent()
{
	return content;
}




//
// LVlist implementation
//

template<class T> LVlist<T>::LVlist()
: m_pRoot(0),
	m_pTail(0)
{
}

template<class T> LVlist<T>::~LVlist()
{
	clear();
}

template<class T> LVlistIterator<T> LVlist<T>::iterator()
{
	return LVlistIterator<T>(this);
}


template<class T> LVlistConstIterator<T> LVlist<T>::const_iterator() const
{
	return LVlistConstIterator<T>(this);
}

template<class T> bool LVlist<T>::lengthIs1() const
{
	if (m_pRoot == 0) {
		return false;
	} else {
		return m_pRoot->getNext() == 0;
	}
}



template<class T> void LVlist<T>::push_back(const T& p)
{
	LVnode<T> *newcomer = new LVnode<T>(p, 0);
	if (m_pRoot == 0) {
		m_pRoot = m_pTail = newcomer;
	} else {
		m_pTail->setNext(newcomer);
		m_pTail = newcomer;
	}
}

// Prepends my own to the beginning of pOther,
// and sets myself to empty.
template<class T> void LVlist<T>::prependWithMe(LVlist<T>* pOther)
{
	// Only do something if I am not empty
	if (m_pRoot != 0) {
		if (pOther->m_pRoot == 0) {
			pOther->m_pRoot = m_pRoot;
			pOther->m_pTail = m_pTail;
		} else {
			m_pTail->setNext(pOther->m_pRoot);
			pOther->m_pRoot = m_pRoot;
		}
		// Now set myself to empty
		m_pRoot = m_pTail = 0;
	}
}

// Appends my own to the end of pOther,
// and sets myself to empty.
template<class T> void LVlist<T>::extendWithMe(LVlist<T>* pOther)
{
	// Only do something if I am not empty.
	if (m_pRoot != 0) {
		// If the ther is empty, it's simple
		if (pOther->m_pRoot == 0) {
			pOther->m_pRoot = m_pRoot;
			pOther->m_pTail = m_pTail;
		} else {
			pOther->m_pTail->setNext(m_pRoot);
			pOther->m_pTail = m_pTail;
		}
		// Now set myself to empty
		m_pRoot = m_pTail = 0;
	}
}

template<class T> void LVlist<T>::prepend(const T& p)
{
	LVnode<T> *newcomer = new LVnode<T>(p, 0);
	if (!m_pRoot) {
		m_pRoot = m_pTail = newcomer;
	} else {
		newcomer->setNext(m_pRoot);
		m_pRoot = newcomer;
	}
}

// Inserts p before iterator.
// Returns the iterator for the node newly created
template <class T> LVlistIterator<T> LVlist<T>::insertBefore(const LVlistIterator<T>& iterator, const T& p)
{
	LVnode<T> *newcomer = new LVnode<T>(p, 0);
	LVnode<T> *next = iterator.m_pIter;
	newcomer->setNext(next);
	// Are we to insert before one past the end?
	if (next == 0) {
		// Yes.

		// So is the list empty?
		if (m_pRoot == 0) {
			// Yes, empty.
			m_pRoot = m_pTail = newcomer;
		} else {
			// No, not empty, so we just update the tail.
			m_pTail->setNext(newcomer);
			m_pTail = newcomer;
		}
	} else {
		// The thing to insert before wasn't the one after the tail

		// So are we inserting before the root?
		if (next == m_pRoot) {
			// Yes.
			// Move root one backwards.
			// Don't bother about the tail: It should stay as it is.
			m_pRoot = newcomer;

			// We have already set newcomer->next to be the root, above.
		} else {
			// The thing to insert before wasn't the root, nor one past the tail

			// Find the node before the thing before which to insert
			LVnode<T> *pIterator = m_pRoot;
			LVnode<T> *pIteratorMinus1 = 0;
			while (pIterator != next) {
				pIteratorMinus1 = pIterator;
				pIterator = pIterator->getNext();
			}

			// Set previous's next to newcomer
			pIteratorMinus1->setNext(newcomer);

			// We have already set newcomer->next to be "next", above
		}
	}

	// Construct return value with newcomer
	// as the iterator
	LVlistIterator<T> return_iterator(this, newcomer);

	// Return value
	return return_iterator;
}

// Removes the element pointed to by to_remove.
// Returns the iterator that points one beyond to_remove.
// This is like STL's std::list::erase().
template <class T> LVlistIterator<T> LVlist<T>::erase(const LVlistIterator<T>& to_remove)
{
	// We must have length >= 1.
	ASSERT_THROW(m_pRoot != 0, "length == 0");

	// Get node to remove
	LVnode<T> *pNodeToRemove = to_remove.m_pIter;
	ASSERT_THROW(pNodeToRemove != 0, "no node to remove");

	// Find node after the thing to be removed
	LVnode<T> *pNodeAfterRemoved = pNodeToRemove->getNext();

	if (pNodeToRemove == m_pRoot) {
		if (lengthIs1()) {
			m_pRoot = m_pTail = 0;
		} else {
			m_pRoot = pNodeAfterRemoved;
		}
		delete pNodeToRemove;
	} else {
		// The thing to remove wasn't the root.

		// Find the node before the thing to be removed
		LVnode<T> *pIterator = m_pRoot;
		LVnode<T> *pIteratorMinus1 = 0;
		while (pIterator != pNodeToRemove) {
			pIteratorMinus1 = pIterator;
			pIterator = pIterator->getNext();
		}

		// Set previous's next to the one after
		pIteratorMinus1->setNext(pNodeAfterRemoved);

		// If the thing to remove was the tail,
		// we should update the tail to be the one before it.
		if (m_pTail == pNodeToRemove) {
			m_pTail = pIteratorMinus1;
		}

		// delete the one if necessary
		delete pNodeToRemove;
	}

	// Construct return value with pNodeAfterDeleted
	// as the iterator
	LVlistIterator<T> return_iterator(this, pNodeAfterRemoved);

	// Return value
	return return_iterator;
}


// Inserts p after iterator.
// Returns an iterator pointing to the node newly created.
template<class T> LVlistIterator<T> LVlist<T>::insertAfter(const LVlistIterator<T> iterator, const T& p)
{
	LVnode<T> *pNewNode = this->insertAfter(iterator.m_pIter, p);
	return LVlistIterator<T>(this, pNewNode);
}

// Inserts p after pIterator.
// Returns the node newly created.
template<class T> LVnode<T> *LVlist<T>::insertAfter(LVnode<T> *pIterator, const T& p)
{
	LVnode<T> *newcomer = new LVnode<T>(p, 0);
	LVnode<T> *next = pIterator->getNext();
	newcomer->setNext(next);
	pIterator->setNext(newcomer);
	if (pIterator == m_pTail) {
		m_pTail = newcomer;
	}
	return newcomer;
}

template<class T> void LVlist<T>::clear()
{
	LVnode<T> *current = m_pRoot;

	while (current != 0) {
		LVnode<T> *next = current->getNext();
		delete current;
		current = next;
	}
	m_pRoot = m_pTail = 0;
}

template<class T> long LVlist<T>::size() const
{
	long result = 0;

	LVnode<T> *current = m_pRoot;
	LVnode<T> *next;

	while (current != 0) {
		next = current->getNext();

		++result;

		current = next;
	}
	return result;
}


//
// LVlistIterator implementation
//

template<class T> LVlistIterator<T>::LVlistIterator(LVlist<T> *pMotherLVlist)
: m_pMotherLVlist(pMotherLVlist)
{
	if (m_pMotherLVlist != 0) {
		m_pIter = m_pMotherLVlist->m_pRoot;
	} else {
		m_pIter = 0;
	}
}

template<class T> LVlistIterator<T>::LVlistIterator(LVlist<T> *pMotherLVlist, LVnode<T> *pNode)
: m_pMotherLVlist(pMotherLVlist),
	m_pIter(pNode)
{
}

/** Constructor.
 *
 * The only constructor which it is safe for the user to call.
 */
template<class T> LVlistIterator<T>::LVlistIterator()
:  m_pMotherLVlist(0)
{
}

template<class T> LVlistIterator<T>::LVlistIterator(const LVlistIterator<T>& other)
{
	assign(other);
}


/** Destructor
 */
template<class T> LVlistIterator<T>::~LVlistIterator()
{
}

template<class T> LVlistIterator<T>& LVlistIterator<T>::operator=(const LVlistIterator<T>& other)
{
	assign(other);
	return *this;
}

	template<class T> void LVlistIterator<T>::assign(const LVlistIterator<T>& other)
{
	m_pMotherLVlist = other.m_pMotherLVlist;
	m_pIter = other.m_pIter;
}


/** Does the linked list have more elements?
 *
 * @return true if there are more elements, false if we have reached
 * the end.  <b>DO NOT</b> call next() if hasNext() returns false!
 */
template<class T> bool LVlistIterator<T>::hasNext() const
{
	if (m_pMotherLVlist == 0) {
		return false; 
	} else {
		return m_pIter != 0;
	}
}


/** Get the current element and advance iterator.
 *
 * Only call if hasNext() returns true.
 *
 * @return A reference to the current element.
 */
template<class T> T& LVlistIterator<T>::next()
{
	// This is redundant, as it is already checked in hasNext()
	// ASSERT_THROW(m_pMotherLVlist != 0, "m_pMotherLVlist is 0, i.e., not set");
	ASSERT_THROW(hasNext(), "hasNext() is false");
	T& TResult = m_pIter->content;
	m_pIter = m_pIter->next;
	return TResult;
}

/** Get current value and do not touch iterator.
 *
 * @return A constant reference to the current value.
 */
template<class T> T& LVlistIterator<T>::current()
{
	ASSERT_THROW(m_pMotherLVlist != 0, "m_pMotherList is 0");
	return m_pIter->content;
}




//
// LVlistConstIterator implementation
//

template<class T> LVlistConstIterator<T>::LVlistConstIterator(const LVlist<T> *pMotherLVlist)
: m_pMotherLVlist(pMotherLVlist)
{
	if (m_pMotherLVlist != 0) {
		m_pIter = m_pMotherLVlist->m_pRoot;
	} else {
		m_pIter = 0;
	}
}

/** Constructor.
 *
 * The only constructor which it is safe for the user to call.
 */
template<class T> LVlistConstIterator<T>::LVlistConstIterator()
:  m_pMotherLVlist(0), m_pIter(0)
{
}

template<class T> LVlistConstIterator<T>::LVlistConstIterator(const LVlistConstIterator<T>& other)
:  m_pMotherLVlist(0), m_pIter(0)
{
	assign(other);
}


/** Destructor
 */
template<class T> LVlistConstIterator<T>::~LVlistConstIterator()
{
}

template<class T> LVlistConstIterator<T>& LVlistConstIterator<T>::operator=(const LVlistConstIterator<T>& other)
{
	assign(other);
	return *this;
}

	template<class T> void LVlistConstIterator<T>::assign(const LVlistConstIterator<T>& other)
{
	m_pMotherLVlist = other.m_pMotherLVlist;
	m_pIter = other.m_pIter;
}


/** Does the linked list have more elements?
 *
 * @return true if there are more elements, false if we have reached
 * the end.  <b>DO NOT</b> call next() if hasNext() returns false!
 */
template<class T> bool LVlistConstIterator<T>::hasNext() const
{
	if (m_pMotherLVlist == 0) {
		return false; 
	} else {
		return m_pIter != 0;
	}
}


/** Get current value and advance iterator to next.
 *
 * Only call if hasNext() returns true.
 *
 * @return A constant reference to the current value.
 */
template<class T> const T& LVlistConstIterator<T>::next()
{
	// This is redundant, as it is already checked in hasNext()
	// ASSERT_THROW(m_pMotherLVlist != 0, "m_pMotherList is 0");
	ASSERT_THROW(hasNext(), "hasNext() is false");
	const T& TResult = m_pIter->content;
	m_pIter = m_pIter->next;
	return TResult;
}

/** Get current value and do not touch iterator.
 *
 * @return A constant reference to the current value.
 */
template<class T> const T& LVlistConstIterator<T>::current()
{
	ASSERT_THROW(m_pMotherLVlist != 0, "m_pMotherLVlist is 0");
	return m_pIter->content;
}




#endif // __LList__h__
