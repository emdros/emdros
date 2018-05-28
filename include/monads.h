//
//
// monads.h
// monad_ms and sets of monads
// Ulrik Petersen
// Created: 7/16-1999
// Last update: 5/28-2018
//
//
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 1999-2018  Ulrik Sandborg-Petersen
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

/**@file monads.h
 *@brief Header file for SetOfMonads and MonadSetElement (EMdF layer)
 */



#ifndef MONADS__H__
#define MONADS__H__

#ifndef SWIG
#include "emdf.h"
#include "emdf_output.h"
#include "string_func.h"
#include "exception_emdros.h"
#include <string>
#include <vector>
#include <map>
#include <ostream>
#endif // !defined SWIG
#undef list
#include "llist.h"
#include "debug.h"



/**
 *\defgroup MonadSets Monad sets
 *\ingroup EMdF
 */

class SetOfMonads; // forward declaration
#ifndef SWIG
class FastSetOfMonads; // forward declaration
#endif


//////////////////////////////////////////////////////////////////
//
//  class MonadSetElement
//
//  Contains two monad_m's, a first and a last.
//  The basic idea is that it represents a contiguous string 
//  of monads, from first till last (inclusive).
//  INVARIANT: first <= last
//
//////////////////////////////////////////////////////////////////
/** Monad set element.
 *\ingroup MonadSets
 *
 * Represents a maximal stretch of monads in a set of monads.  Has two
 * monads: \p first_m and \p last_m.  
 *
 * The idea is that the MonadSetElement represents all the monads from
 * \p first_m (inclusive) to \p last_m (inclusive).
 *
 * The invariant is that \f[\mathtt{first\_m} \leq
 * \mathtt{last\_m}\f].  If this constraint is violated, then
 * BadMonadsException is thrown.
 */
class MonadSetElement {
 private:
	monad_m first_m;   /**< The first monad in the monad set element. */
	monad_m last_m;    /**< The last monad in the monad set element. */
 public:
	friend class SetOfMonads; 
#ifndef SWIG
	friend class FastSetOfMonads; 
#endif
	/** Constructor for ranges.
	 *
	 * Use this constructor to create a MonadSetElement which spans a
	 * range of monads from first to last.
	 *
	 * Note that \p last \f$\geq\f$ \p first MUST hold.  Otherwise,
	 * BadMonadsException is thrown.
	 *
	 * @param first The first monad of the range.
	 *
	 * @param last The last monad of the range.
	 */
	MonadSetElement(monad_m first, monad_m last)
		: first_m(0), last_m(0) {
		if (first <= last) {
			this->first_m = first;
			this->last_m = last;
		} else {
			ASSERT_THROW_X(false, BadMonadsException, "first > last: " + monad_m2string(first) + " > " + monad_m2string(last));
		}
	};

	/** Constructor for singleton MonadSetElements.
	 *
	 * Use this constructor to create a MonadSetElement which
	 * represents a single monad: first and last will both be set to
	 * \p monad.
	 *
	 * @param monad The monad of the singleton.
	 */
         MonadSetElement(monad_m monad) : first_m(monad), last_m(monad) { };

	/** Returns whether other is contained within the borders of myself.
	 *
	 * @param other The MSE to compare with.
	 *
	 * @return Whether other is contained within the borders of myself.
	 */
	bool subsumes(const MonadSetElement& other) const {
		return first_m <= other.first_m && other.last_m <= last_m;
	};
	//
	// Subsume under other.
	// This means:
	//     set first_m to other.first() iff first_m > other.first().
	//     set last_m to other.last() iff last_m < other.last().
	// 
	// If there is no overlap, or if they are not adjacent, a
	// BadMonadsException is thrown.
	//
#ifndef SWIG
	void subsume(MonadSetElement& other) {
		if (first_m > other.first_m)
			first_m = other.first_m;
		if (last_m < other.last_m)
			last_m = other.last_m;
	};
	void truncate(monad_m other_first_m, monad_m other_last_m) {
		if (first_m < other_first_m)
			last_m = other_first_m-1;
		else if (last_m > other_last_m)
			first_m = other_last_m+1;
	};
	bool adjacent(const MonadSetElement& b) const {
		return ((last_m+1 == b.first_m) || (b.last_m+1 == first_m));
	};
	bool overlap(const MonadSetElement& b) const {
		return ((last_m >= b.first_m) && (first_m <= b.last_m));
	};
	// Is this.first() > b.last() + 1 ?
	// That is, is this past b?
	bool past(const MonadSetElement& b) const {
		return first_m > (b.last_m + 1);
	};
	bool operator==(const MonadSetElement& b) const {
		return (first_m == b.first_m && last_m == b.last_m);
	};
#endif // !defined SWIG
	// Returns true on this and b having the same first_m and
	// this and b having the same last_m.
	/** Check whether to MonadSetElements are equal.
	 *
	 * Returns true on \p *this and \p b having the same \p first_m and
	 * \p *this and \p b having the same \p last_m.
	 *
	 * @param b The other MonadSetElement to compare with.
	 * @return true on b equal to ourselves, false otherwise.
	 */
	bool equals(const MonadSetElement& b) const {
		return (*this) == b;
	};
	/** Get first_m. 
	 *
	 * @return first_m.
	 */
	monad_m first(void) const { return first_m; };
	/** Get last_m. 
	 *
	 * @return last_m.
	 */
	monad_m last(void) const { return last_m; };
	/** Offset a MonadSetElement. 
	 *
	 * Add \p m to both \p first_m and \p last_m, thereby offsetting
	 * the MonadSetElement \p m monads.
	 *
	 * @param m The number of monads to offset by.
	 */
	void offset(monad_m m) {
		first_m += m;
		last_m += m;
	};

	/** Print on EMdFOutput using Console style.
	 *
	 * @param pOut The EMdFOutput object to use.
	 */
	void printConsole(EMdFOutput *pOut) const {
		if (first_m == last_m) {
			pOut->out(" " + monad_m2string(first_m));
		} else {
			pOut->out(" " + monad_m2string(first_m) + "-" + monad_m2string(last_m));
		}
	};
	/** Print on EMdFOutput using XML style.
	 *
	 * @param pOut The EMdFOutput object to use.
	 */
	void printXML(EMdFOutput *pOut) const {
		// Make attribute list
		AttributePairList attribute_list;
		attribute_list.push_back(AttributePair("first", monad_m2string(first_m)));
		attribute_list.push_back(AttributePair("last", monad_m2string(last_m)));
      
		// Emit tag
		pOut->startSingleTag("mse", attribute_list, true);
	};

	/** Print on EMdFOutput using JSON style.
	 *
	 * @param pOut The EMdFOutput object to use.
	 */
	void printJSON(EMdFOutput *pOut) const {
        pOut->jsonStartStruct();

        pOut->jsonLabel("first");
        pOut->jsonOutValue(first_m);

        pOut->jsonLabel("last");
        pOut->jsonOutValue(last_m);

        pOut->jsonEndStruct();
	};

#ifndef SWIG
	std::ostream& putme(std::ostream& s) const {
		if (first_m == last_m)
			s << first_m;
		else
			s << first_m << "-" << last_m;
		return s;
	};
	bool operator<(const MonadSetElement& other) const {
		if (first_m < other.first_m)
			return true;
		else if (first_m > other.first_m)
			return false;
		else if (last_m < other.last_m) // So first_m == other.first_m
			return true;
		else 
			return false;
	};
#endif // !defined SWIG
	// 
	/** Check whether we are lexicographically before other
	 *
	 * Returns true on this object being "lexicographically" before
	 * the \p other object.
	 *
	 * That is, true if and only if this->first_m < other.first_m or
	 * (this->first_m == other.first_m and this->last_m <
	 * other.last_m)
	 *
	 * @param other The other MonadSetElement to compare with.
	 *
	 * @return True if and only if we are lexicographically smaller
	 * than other.
	 */
	bool isBefore(const MonadSetElement& other) const {
		return (*this) < other;
	};
	/** Check whether we are "before" a monad m.
	 *
	 * Returns true on this object's m_last being less than m
	 *
	 * @param m The monad to check against.
	 *
	 * @return True on m_last being < m, false otherwise.
	 *
	 */
	bool isBefore(monad_m m) const {
		return last_m < m;
	};
};




#ifndef SWIG

// NOTE: It is not an advantage to use std::vector instead of
// std::list here, even though it used to compile cleanly. The engine
// is about 100% slower on my standard list of queries to run if one
// uses std::vector instead of std::list.

/** An LVlist of MonadSetElement.
 */
typedef LVlist<MonadSetElement> MSEList;

// %template () LVlist<MonadSetElement>;

/** An iterator over an LVlist of MonadSetElements.
 */
typedef LVlistIterator<MonadSetElement> SOMIterator;

std::ostream& operator<<(std::ostream& s, const MonadSetElement& mse);
#endif

/** Constant iterator over SetOfMonads.
 *\ingroup MonadSets
 *
 *\section Example
 *\code
 *  SetOfMonads som; // Assumed to be initialized from somewhere
 *  SOMConstIterator sci = som.const_iterator();
 *
 *  // This is one way of iterating
 *  while (sci.hasNext()) {
 *      MonadSetElement mse = sci.current();
 *   
 *      // Process mse ...
 *
 *      // Are there any left?
 *      if (sci.hasNext()) {
 *          // Do something if the just-processed mse is not the last
 *      }
 * 
 *      sci.next();
 *  }
 *
 *  // And this is another, simpler method:
 *  SetOfMonads som; // Assumed to be initialized from somewhere
 *  SOMConstIterator sci = som.const_iterator();
 *  while (sci.hasNext()) {
 *      MonadSetElement mse = sci.next();
 *   
 *      // Process mse ...
 *
 *      // Note how there is no si.next() again, since next() 
 *      // first gets current element and then advances iterator.
 *  }
 \endcode
 */
typedef LVlistConstIterator<MonadSetElement> SOMConstIterator;

#ifdef SWIG
%template (SOMConstIterator) LVlistConstIterator<MonadSetElement>;
#endif


/** Constant iterator over the monads in a SetOfMonads.
 *\ingroup MonadSets
 *
 * This is a complementary class to SOMConstIterator.  Where
 * SOMConstIterator iterates over the MonadSetElement objects in a
 * SetOfMonads, this class iterates over the individual monads in a
 * SetOfMonads.
 */
class SOMConstMonadIterator {
 private:
	SOMConstIterator m_hint;
	monad_m m_cur_monad;
 public:
	SOMConstMonadIterator();
	SOMConstMonadIterator(const SetOfMonads& som);
	SOMConstMonadIterator(const SOMConstMonadIterator& som);
	bool hasNext(void);
	monad_m next(void);
	monad_m current(void);
#ifndef SWIG
	SOMConstMonadIterator& operator=(const SOMConstMonadIterator& other);
#endif
 private:
	void assign(const SOMConstMonadIterator& other);
};


/** Set of monads.
 *\ingroup MonadSets
 *
 * Implements an efficient set of monads.  The idea is to store an
 * ordered list of maximal stretches of monads, each of which is a
 * MonadSetElement object.  The invariants are:
 *
 * -# There is at least one empty (or non-represented) monad in between
 * each MonadSetElement (mse).  Thus if mseA ends at monad A_last,
 * then the next mse in the list, mseB, must begin at monad A_last+2
 * or later.
 * -# The list is sorted in ascending order.
 *
 * This gives a unique representation of each possible set of monads.
 *
 * To iterate over a SetOfMonads, use a SOMConstIterator object.  This
 * can be created with SetOfMonads::const_iterator().
 *
 * \see SOMConstIterator for an example of how to use the iterator.
 *
 * \section Examples
 *\code
 *  // Declare and fill som with monads
 *  SetOfMonads som;
 *  som.add(1);      // Now it is { 1 }
 *  som.add(3,6);    // Now it is { 1, 3-6 }
 *  som.add(10,13) ; // Now it is { 1, 3-6, 10-13 }
 *
 *  // Get string representation
 *  std::string stringRepresentation = som.toString();
 *
 *  // Declare and fill som2 with monads
 *  SetOfMonads som2;
 *  som2.add(2,7);  // Now it is { 2-7 }
 *
 *  // Declare and fill som3 with monads
 *  SetOfMonads som3;
 *  som3.add(2,4);  // Now it is { 2-4 }
 *  
 *
 *  // Add the monads of som2 to som
 *  som.unionWith(som2) // som is now { 1-7, 10-13 }
 *
 *  // Get set intersection of som and som3
 *  SetOfMonads som4;
 *  som4 = SetOfMonads::intersect(som, som3); // som4 is now { 2-4 }
 *
 *  // Subtract the monads of som2 from som
 *  som.difference(som2); // som is now { 10-13 }
 \endcode
*/
class SetOfMonads {
#ifndef SWIG
 private:
	MSEList monad_ms;
	monad_m m_first, m_last;
	friend class FastSetOfMonads;
#endif // !defined SWIG
 public:

	/** Get first monad of set.
	 * @return The first monad of the set.
	 */
	monad_m first() const { return m_first; };
	/** Get last monad of set.
	 * @return The last monad of the set.
	 */
	monad_m last() const { return m_last; };
	SOMConstIterator const_iterator() const;
	std::ostream& putme(std::ostream& s) const;
	std::string toString(void) const;
	void fromString(const std::string& instr);
	void printConsole(EMdFOutput *pOut) const;
	void printXML(EMdFOutput *pOut) const;
	void printJSON(EMdFOutput *pOut) const;
        SetOfMonads() : monad_ms(), m_first(MAX_MONAD), m_last(0) {};
	SetOfMonads(const SetOfMonads& other);
#ifndef SWIG
	SetOfMonads(const FastSetOfMonads& other);
	// Not SWIG-wrapped because it resulted in a SWIGTYPE_p_long.
	SetOfMonads(monad_m *pMem, long size);
#endif
	/** Constructor from compact string.
	 * 
	 * Constructs a set from a compact string.  This would previously
	 * have been gotten from SetOfMonads::toCompactString().
	 *
	 * @param compactString The compact string to create from.
	 */
        SetOfMonads(const std::string& compactString) : monad_ms(), m_first(MAX_MONAD), m_last(0) { fromCompactString(compactString); };

	/** Constructor from compact string and first monad.
	 * 
	 * Constructs  a set from  a compact  string and  first monad.
	 * This    would   previously    have    been   gotten    from
	 * SetOfMonads::toCompactStringFM().
	 *
	 * @param compactString The compact string to create from.
	 */
 SetOfMonads(const std::string& compactString, monad_m first_monad) : monad_ms(), m_first(MAX_MONAD), m_last(0) { fromCompactStringFM(compactString, first_monad); };
	/** Constructor for singleton set.
	 * 
	 * Constructs a set with (initially) only one monad, \p m.
	 *
	 * @param m The monad to add to the empty set.
	 */
         SetOfMonads(monad_m m) : monad_ms(), m_first(m), m_last(m) { monad_ms.push_back(MonadSetElement(m)); };
	/** Constructor for single range set.
	 * 
	 * Constructs a set with (initially) a single range, from \p first
	 * to \p last.
	 *
	 * @param first The first monad of the range to add to an empty set.
	 * @param last The last monad of the range to add to an empty set.
	 */
	SetOfMonads(monad_m first, monad_m last) : monad_ms(), m_first(first), m_last(last) { monad_ms.push_back(MonadSetElement(first,last)); };
	~SetOfMonads();
	bool part_of(const SetOfMonads& other) const;
	bool starts_in(const SetOfMonads& other) const;
#ifndef SWIG
	bool part_of(const FastSetOfMonads& other) const;
#endif
	void unionWith(const SetOfMonads& other);
	void difference(const SetOfMonads& other);
	static SetOfMonads intersect(const SetOfMonads& Aset, const SetOfMonads& Bset);
	static bool overlap(const SetOfMonads& Aset, const SetOfMonads& Bset);
	void addMSE(MonadSetElement mse);
	void add(monad_m monad);
	void add(monad_m first, monad_m last);
	bool equals(SetOfMonads& other) const { return *this == other; };
	long getCardinality() const;
#ifndef SWIG
	// size() tells us the number of monad stretches.
	long size() const { return monad_ms.size(); };
	void copyOther(const SetOfMonads& som);
	SetOfMonads& operator=(const SetOfMonads& som);
	bool operator==(const SetOfMonads& other) const;
	void getMonad_mVector(std::vector<monad_m>& monad_vec) const;
	void getMonad_mList(std::list<monad_m>& monad_list) const;
	SetOfMonads getGaps(void) const;
	void removeMSE(monad_m B_first, monad_m B_last);
#endif // !defined SWIG
	bool isMemberOf(monad_m m) const;
	bool isMemberOf(monad_m m, SOMConstIterator& hint_it) const;
	bool hasMonadsInRange(monad_m first_m, monad_m last_m) const;
	bool hasLargerGap(monad_m than_this);
	/** Test whether we are the empty set.
	 *
	 * @return true iff we are the empty set.
	 */
	bool isEmpty(void) const { return monad_ms.empty(); };
	void removeMSE(const MonadSetElement& mse);
	bool gapExists(monad_m Sm, monad_m& m) const;
	void offset(monad_m m);
	void clear();
	SetOfMonads fillInsertRange(monad_m first, monad_m last) const;
	std::string toCompactString(void) const;
	void fromCompactString(const std::string& inStr);
	std::string toCompactStringFM(void) const;
	void fromCompactStringFM(const std::string& inStr, monad_m first_monad);
	/** Check whether set is a singleton or single range.
	 *
	 * Returns true if there is only one MonadSetElement in the set.
	 * This can be either a singleton or a single range.
	 *
	 * @return true if there is only one MonadSetElement, false if not.
	 */
	bool hasOnlyOneMSE(void) const { return monad_ms.lengthIs1(); };
	SetOfMonads fillGaps(monad_m min_gap) const;
};


#ifndef SWIG
std::ostream& operator<<(std::ostream& str, const SetOfMonads& som);
#endif

#ifndef SWIG
/** A std::map between monad_m and MonadSetElement.
 *
 * Is used in a fast set of monads.
 */
typedef std::map<monad_m, MonadSetElement> FastSOMMap;

/** An iterator over a FastSOMMap.
 */
typedef FastSOMMap::iterator FastSOMIterator;

/** Constant iterator over the MonadSetElement objects in a FastSetOfMonads.
 *\ingroup MonadSets
 *
 * This is the same as SOMConstIterator, but for a FastSetOfMonads.
 */
class FastSOMConstIterator {
 private:
	const FastSOMMap& m_mother;
	FastSOMMap::const_iterator m_hint;
	monad_m m_cur_monad;
 public:
	FastSOMConstIterator(const FastSetOfMonads& som);
	FastSOMConstIterator(const FastSOMConstIterator& som);
	bool hasNext(void);
	MonadSetElement next(void);
	MonadSetElement current(void);
#ifndef SWIG
	FastSOMConstIterator& operator=(const FastSOMConstIterator& other);
#endif
 private:
	void assign(const FastSOMConstIterator& other);
};


#endif // !defined(SWIG)

#ifndef SWIG
/** A fast set of monads.
 *\ingroup MonadSets
 *
 * This implementation of a set of monads uses a binary tree (actually
 * a std::map) to store the MonadSetElement objects, which can lead to
 * speed increases when having very large monad sets.  However, it
 * uses more memory than a SetOfMonads.
 *
 */
class FastSetOfMonads {
 private:
	FastSOMMap monad_ms;
	monad_m m_first, m_last;
	friend class FastSOMConstIterator;
	friend class SetOfMonads;
 public:
         FastSetOfMonads();
	/** Constructor from a single monad.
	 *
	 * Calls #FastSetOfMonads::add(monad_m).
	 *
	 * @see FastSetOfMonads::add(monad_m).
	 *
	 * @param m The single monad to initialize with.
	 */
         FastSetOfMonads(monad_m m) : monad_ms(), m_first(MAX_MONAD), m_last(0)  { add(m); };
	/** Constructor from a single monad range.
	 *
	 * Calls #FastSetOfMonads::add(monad_m,monad_m).
	 *
	 * @see FastSetOfMonads::add(monad_m,monad_m).
	 *
	 * @param fm The first monad of the range to initialize with.
	 * @param lm The last monad of the range to initialize with.
	 */
        FastSetOfMonads(monad_m fm, monad_m lm)  : monad_ms(), m_first(MAX_MONAD), m_last(0)  { add(fm,lm); };
	FastSetOfMonads(const SetOfMonads& other);
	FastSetOfMonads(const FastSetOfMonads& other);
	/** Destructor.
	 */
	~FastSetOfMonads() {};

	/** Get first monad of set.
	 * @return The first monad of the set.
	 */
	monad_m first() const { return m_first; };
	/** Get last monad of set.
	 * @return The last monad of the set.
	 */
	monad_m last() const { return m_last; };
	FastSOMConstIterator const_iterator() const;

	void addMSE(MonadSetElement mse);
	void removeMSE(const MonadSetElement& mse);
	void add(monad_m monad);
	void add(monad_m first, monad_m last);

	static FastSetOfMonads intersect(const FastSetOfMonads& Aset, const FastSetOfMonads& Bset);
	void clear();
	static bool overlap(const SetOfMonads& Aset, const FastSetOfMonads& Bset);
	/** Check whether set is a singleton or single range.
	 *
	 * Returns true if there is only one MonadSetElement in the set.
	 * This can be either a singleton or a single range.
	 *
	 * @return true if there is only one MonadSetElement, false if not.
	 */
	bool hasOnlyOneMSE(void) const { 
		FastSOMConstIterator ci = const_iterator(); 
		if (ci.hasNext()) {
			ci.next();
			if (ci.hasNext()) {
				return false;
			} else {
				return true;
			}
		} else {
			return false;
		}
	};


	long getCardinality() const;

	/** Test whether we are the empty set.
	 *
	 * @return true iff we are the empty set.
	 */
	bool isEmpty(void) const { return monad_ms.empty(); };
	void getMonad_mVector(std::vector<monad_m>& monad_vec) const;
	bool isMemberOf(monad_m m) const;
	bool hasMonadsInRange(monad_m first_m, monad_m last_m) const;
	bool hasLargerGap(monad_m than_this);
	FastSetOfMonads fillGaps(monad_m min_gap) const;
	void unionWith(const SetOfMonads& other);
	void unionWith(const FastSetOfMonads& other);
	void difference(const FastSetOfMonads& other);
	bool subsumesRange(const MonadSetElement& range) const;
	FastSetOfMonads removeMonadsBefore(monad_m monad) const;
	bool operator==(const FastSetOfMonads& other) const;
	FastSetOfMonads& operator=(const FastSetOfMonads& other);
 protected:
	void assign(const FastSetOfMonads& other);
 public:
	std::string toString(void) const;
	std::ostream& putme(std::ostream& s) const;

	bool gapExists(monad_m Sm, monad_m& m) const;

	void offset(monad_m m);
};



#endif // !defined(SWIG)


#endif //MONADS__H__
