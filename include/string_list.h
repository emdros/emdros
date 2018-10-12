/*
 * string_list.h
 *
 * ParserList and associated classes
 *
 * Ulrik Petersen
 * Created: 7/8-2003
 * Last update: 4/26-2013
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


/**@file string_list.h
 *@brief ParserList and related classes (EMdF layer)
 */


#ifndef STRING_LIST__H__
#define STRING_LIST__H__

#include <vector>
#include <string>
#include <sstream>
#include "debug.h"

#ifndef SWIG
template<typename T> struct T_is_pointer { static const bool value = false; };
template<typename T> struct T_is_pointer<T*> { static const bool value = true; };

template<typename T> struct delete_if_pointer{ void doIt(T val) {(void) val;}; };
template<typename T> struct delete_if_pointer<T*> { void doIt(T* val) { delete val; } };

template<typename T> class ParserListNode {
 protected:
	T m_value;
	ParserListNode<T> *m_next; // Points from head to tail
	ParserListNode<T> *m_prev; // Points from tail to head
 public:
	ParserListNode(const T& myval, ParserListNode<T> *prev);
 private:
	// Leave unimplemented, so as to prevent the compiler from
	// generating crashing ones.
	ParserListNode(const ParserListNode& other);
	ParserListNode& operator=(const ParserListNode& other);
 public:
	virtual ~ParserListNode();
	ParserListNode<T>* getNext() { return m_next; }; 
	ParserListNode<T>* getPrev() { return m_prev; };
	void setPrev(ParserListNode<T> *prev) { m_prev = prev; };
	void setNext(ParserListNode<T>* next) { m_next = next; };
	T getValue() const { return m_value; };
};

typedef ParserListNode<std::string> StringListNode;
typedef ParserListNode<long> IntegerListNode;
#endif /* !defined SWIG */

template<typename T> class ParserListConstIterator; // Forward declaration

template<typename T> class ParserList {
 private:
#ifndef SWIG
	ParserListNode<T> *m_tail;
	ParserListNode<T> *m_head;
	friend class ParserListConstIterator<T>;
#endif 
 public:
	// Create empty parser list, to be populated later
	// with addParserListNodes.
	ParserList();

	// Copy constructor
	ParserList(const ParserList<T>& other);

	// Constructor for undoing getDelimitedString
	ParserList(const std::string& myval, const std::string& delimiter);
#ifndef SWIG
	// Create a string list and call addParserListNodes with tail
	ParserList(ParserListNode<T> *tail);

        ParserList<T>& operator=(const ParserList<T>& other);
#endif
	std::vector<T> getAsVector(void) const;
	int getLength() const;
	virtual ~ParserList();
	ParserListConstIterator<T> const_iterator() const;
	void addValueFront(const T& myval);
	void addValueBack(const T& myval);
	bool isEmpty(void) const;
	std::string getDelimitedString(std::string delimiter) const;
 private:
	// Add the string list nodes, making a copy of both strings
	// and ParserListNodes.
	void addParserListNodes(ParserListNode<T> *tail);
 protected:
	void assignOther(const ParserList<T>& other);
	void copyOther(const ParserList<T>& other);
};


template<typename T> class ParserListConstIterator {
 private:
#ifndef SWIG
	ParserListNode<T> *m_current;
	const ParserList<T> *m_pMotherParserList;
#endif
 public:
	ParserListConstIterator();
#ifndef SWIG
	ParserListConstIterator(const ParserList<T> *pMotherParserList);
#endif
	ParserListConstIterator(const ParserListConstIterator<T>& other);
	virtual ~ParserListConstIterator();
	bool hasNext() const; // Is the iterator == end iterator?  Doesn't alter iterator
	T next(); // Gets current and advances iterator afterwards
	T previous();  // Regresses iterator and then gets current
	T current(); // Gets current without altering iterator
#ifndef SWIG
	ParserListConstIterator<T>& operator=(const ParserListConstIterator<T>& other);
#endif
 protected:
	void assign(const ParserListConstIterator<T>& other);
};


//////////////////////////////////////////////////
//
// 
// ParserListNode
//
//
//////////////////////////////////////////////////

template<typename T> ParserListNode<T>::ParserListNode(const T& myval, ParserListNode<T> *prev)
: m_value(myval),
	m_next(0),
	m_prev(prev)
{
	T val(myval);
	m_value = val;
	m_next = 0;
	m_prev = prev;
}


template<typename T> ParserListNode<T>::~ParserListNode()
{
	delete_if_pointer<T> dif; dif.doIt(m_value); 
	delete m_prev;
}



//////////////////////////////////////////////////
//
// 
// ParserListConstIterator
//
//
//////////////////////////////////////////////////



template<typename T> ParserListConstIterator<T>::ParserListConstIterator()
: m_current(0),
	m_pMotherParserList(0)
{
}


template<typename T> ParserListConstIterator<T>::ParserListConstIterator(const ParserList<T> *pMotherParserList)
: m_current(pMotherParserList->m_head),
	m_pMotherParserList(pMotherParserList)
{
}


template<typename T> ParserListConstIterator<T>::ParserListConstIterator(const ParserListConstIterator<T>& other)
: m_current(0), m_pMotherParserList(0)
{
	assign(other);
}


template<typename T> ParserListConstIterator<T>::~ParserListConstIterator()
{
}


// Is the iterator == end iterator?  Doesn't alter iterator
template<typename T> bool ParserListConstIterator<T>::hasNext() const
{
	return m_current != 0;
}

template<typename T> T ParserListConstIterator<T>::next()
{
	ASSERT_THROW(m_current != 0, "m_current is 0");
	T myval = m_current->getValue();
	m_current = m_current->getNext();
	return myval;
}

// Gets current and advances iterator afterwards
template<typename T> T ParserListConstIterator<T>::previous()
{
	if (m_current == 0) {
		ASSERT_THROW(m_pMotherParserList != 0,
			     "m_pMotherParserList is 0");
		m_current = m_pMotherParserList->m_tail;
	} else {
		m_current = m_current->getPrev();
	}
	return current();
}

// Regresses iterator and then gets current
template<typename T> T ParserListConstIterator<T>::current()
{
	ASSERT_THROW(m_current != 0,
		     "m_current is 0");
	T myval = m_current->getValue();
	return myval;
}

// Gets current without altering iterator
template<typename T> ParserListConstIterator<T>& ParserListConstIterator<T>::operator=(const ParserListConstIterator<T>& other)
{
	assign(other);
	return *this;
}


template<typename T> void ParserListConstIterator<T>::assign(const ParserListConstIterator<T>& other)
{
	m_current = other.m_current;
	m_pMotherParserList = other.m_pMotherParserList;
}


//////////////////////////////////////////////////
// 
// ParserList
//
//////////////////////////////////////////////////



template<typename T> ParserList<T>::ParserList() : m_tail(0), m_head(0)
{
}


template<typename T> ParserList<T>::ParserList(const ParserList<T>& other) : m_tail(0), m_head(0)
{
	copyOther(other);
}


template<typename T> ParserList<T>::ParserList(ParserListNode<T> *tail) : m_tail(0), m_head(0)
{
	if (tail != 0) 
		addParserListNodes(tail);
}


template<typename T> ParserList<T>::ParserList(const std::string& str, const std::string& delimiter) : m_tail(0), m_head(0)
{
	ASSERT_THROW(delimiter == " ", "delimiter is not ' '");
	std::istringstream myistrstream(str);
	T myT;
	myistrstream >> myT;
	while (myistrstream.good() && !myistrstream.eof()) {
		addValueBack(myT);
		myistrstream >> myT;
	}
}


template<typename T> ParserList<T>::~ParserList()
{
	delete m_tail;
}


template <typename T> ParserList<T>& ParserList<T>::operator=(const ParserList<T>& other)
{
	assignOther(other);
	return *this;
}


template <typename T> void ParserList<T>::assignOther(const ParserList<T>& other)
{
	delete m_tail;
	m_head = m_tail = 0;
	copyOther(other);
}


template<typename T> void ParserList<T>::copyOther(const ParserList<T>& other)
{
	ParserListConstIterator<T> ci = other.const_iterator();
	while (ci.hasNext()) {
		T myval = ci.next();
		this->addValueBack(myval);
	}
}

template<typename T> std::vector<T> ParserList<T>::getAsVector(void) const
{
	std::vector<T> result;
	ParserListConstIterator<T> ci = this->const_iterator();
	while (ci.hasNext()) {
		T myval = ci.next();
		result.push_back(myval);
	}
	return result;
}

template<typename T> int ParserList<T>::getLength() const
{
	int result = 0;
	ParserListConstIterator<T> ci = this->const_iterator();
	while (ci.hasNext()) {
		++result;
	}
	return result;
}
								



template<typename T> ParserListConstIterator<T> ParserList<T>::const_iterator() const
{
	return ParserListConstIterator<T>(this);
}

template<typename T> bool ParserList<T>::isEmpty(void) const
{
	return m_head == 0;
}


template<typename T> std::string ParserList<T>::getDelimitedString(std::string delimiter) const
{
	std::ostringstream mystrstream;
	ParserListConstIterator<T> ci = const_iterator();
	if (ci.hasNext()) {
		while (ci.hasNext()) {
			mystrstream << delimiter;
			mystrstream << ci.next();
		}
		mystrstream << delimiter;
	} else {
		// Leave mystrstream empty if list is empty.
	}
	return mystrstream.str();
}



template<typename T> void ParserList<T>::addValueFront(const T& myval)
{
	ParserListNode<T> *pNew = new ParserListNode<T>(myval, 0);
	if (m_head == 0) {
		m_head = m_tail = pNew;
	} else {
		m_head->setPrev(pNew);
		pNew->setNext(m_head);
		m_head = pNew;
	}
	/*
	ParserListNode<T> *pNew = new ParserListNode<T>(myval, 0);
	ParserListNode<T> *head = m_head;
	pNew->setPrev(head);
	if (head != 0) {
		ASSERT_THROW(head->getNext() == 0,
			     "head->getNext() is not 0, as it should be");
		head->setNext(pNew);
	}
	m_head = pNew;
	if (m_tail == 0) {
		m_tail = m_head;
	}
	*/
}

template<typename T> void ParserList<T>::addValueBack(const T& myval)
{
	ParserListNode<T> *pNew = new ParserListNode<T>(myval, 0);
	if (m_head == 0) {
		m_head = m_tail = pNew;
	} else {
		m_tail->setNext(pNew);
		pNew->setPrev(m_tail);
		m_tail = pNew;
	}
	/*
	ParserListNode<T> *pNew = new ParserListNode<T>(myval, 0);
	ParserListNode<T> *tail = m_tail;
	pNew->setNext(tail);
	if (tail != 0) {
		tail->setPrev(pNew);
	}
	m_tail = pNew;
	if (m_head == 0) {
		m_head = m_tail;
	}
	*/
}

template<typename T> void ParserList<T>::addParserListNodes(ParserListNode<T> *tail)
{
	if (tail->getPrev() != 0) {
		addParserListNodes(tail->getPrev());
	}
	addValueBack(tail->getValue());
}

typedef ParserListConstIterator<std::string> StringListConstIterator;
#ifdef SWIG
%template (StringListConstIterator) ParserListConstIterator<std::string>;
#endif

typedef ParserList<std::string> ParserListStdString;


class StringList :  public ParserListStdString {
 public:
	// Create empty string list, to be populated later
	// with addStringListNodes.
	StringList() : ParserListStdString() {};

	// Copy constructor
	StringList(const StringList& other) : ParserListStdString(other) {};

	StringListConstIterator const_iterator() const { return ParserListStdString::const_iterator(); };

#ifndef SWIG
	StringList& operator=(const StringList& other) { assignOther(other); return *this; };

	// Create a string list and call addStringListNodes with tail
	StringList(StringListNode *tail) 
		: ParserListStdString(tail) {};
#endif
	virtual ~StringList() {};
	void addStringFront(const std::string& str) { addValueFront(str); };
	void addStringBack(const std::string& str) { addValueBack(str); };
	bool isEmpty(void) const { return ParserListStdString::isEmpty(); };
	int getLength() const { return ParserListStdString::getLength(); };

	std::vector<std::string> getAsVector(void) const { return ParserListStdString::getAsVector(); };
};

typedef ParserListConstIterator<long> IntegerListConstIterator;
#ifdef SWIG
%template (IntegerListConstIterator) ParserListConstIterator<long>;
#endif

typedef ParserList<long> ParserListLong;
				     /*
				       #ifdef SWIG
				       %template (ParserListLong) ParserList<long>;
				       #endif
				     */





class IntegerList : public ParserListLong {
 public:
	// Create empty integer list, to be populated later
	// with addParserListNodes.
	IntegerList() {};

	// Copy constructor
	IntegerList(const IntegerList& other) : ParserListLong(other) {};
	IntegerList(const std::string& str, std::string delimiter) : ParserListLong(str, delimiter) {}

	IntegerListConstIterator const_iterator() const { return ParserListLong::const_iterator(); };

#ifndef SWIG
	IntegerList& operator=(const IntegerList& other) { assignOther(other); return *this; };

	// Create a integer list and call addParserListNodes with tail
	IntegerList(IntegerListNode *tail) : ParserListLong(tail) {};
#endif
	virtual ~IntegerList() {};
	void addIntegerFront(long l) { addValueFront(l); };
	void addIntegerBack(long l) { addValueBack(l); };
	bool isEmpty(void) const { return ParserListLong::isEmpty(); };
	std::vector<long> getAsVector(void) const { return ParserListLong::getAsVector(); };
};




#endif // STRING_LIST__H__
