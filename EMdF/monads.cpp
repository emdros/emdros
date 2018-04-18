//
//
// monads.cpp
// monads and sets of monads
// Ulrik Petersen
// Created: 7/16-1999
// Last update: 8/17-2013
//
//
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 1999-2013  Ulrik Sandborg-Petersen
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

/**@file monads.cpp
 *@brief Implementation of SetOfMonads and MonadSetElement (EMdF layer)
 */

#include <iostream>
#include <sstream>
#include <monads.h>
#include <debug.h>


//////////////////////////////////////////////////////////////////
//
//  class SOMConstMonadIterator
//
//  Iterator over the monads, not the monad set elements
//
//////////////////////////////////////////////////////////////////


/** Constructor for a SOMConstMonadIterator.
 *
 * Just creates an empty SOMConstMonadIterator, which has #hasNext()
 * false and which is not associated with any SetOfMonads.
 *
 * This is useful if you want to declare a variable of type
 * SOMConstMonadIterator, without assigning any value to
 * it. Afterwards, you can assign a "real" SOMConstMonadIterator to
 * the variable.
 */
SOMConstMonadIterator::SOMConstMonadIterator()
{
	m_hint = SOMConstIterator();
	m_cur_monad = MAX_MONAD;
}

/** Constructor for a SOMConstMonadIterator.
 *
 * Creates a SOMConstMonadIterator which is associated with a specific
 * set of monads, and which points to the first monad of that SOM (if
 * the SOM is not empty).
 *
 * @param som The set of monads to be associated with.
 */
SOMConstMonadIterator::SOMConstMonadIterator(const SetOfMonads& som)
{
	m_hint = som.const_iterator();
	if (m_hint.hasNext()) {
		m_cur_monad = som.first();
	} else {
		m_cur_monad = MAX_MONAD;
	}

}

/** Copy constructor for SOMConstMonadIterator.
 *
 */
SOMConstMonadIterator::SOMConstMonadIterator(const SOMConstMonadIterator& somcmit)
{
	assign(somcmit);
}

/** Does the SetOfMonads have more monads?
 *
 * @return true if there are more monads, false if we have reached
 * the end.  <b>DO NOT</b> call next() if hasNext() returns false!
 */
bool SOMConstMonadIterator::hasNext(void)
{
	return m_hint.hasNext();
}

/** Get the current monad and advance iterator.
 *
 * Only call if #hasNext() returns true.
 *
 * @return The current monad.
 */
monad_m SOMConstMonadIterator::current(void)
{
	ASSERT_THROW(hasNext(), "hasNext() is false");
	monad_m result = m_cur_monad;
	return result;
}

/** Get the current monad and advance iterator.
 *
 * Only call if #hasNext() returns true.
 *
 * @return The current monad.
 */
monad_m SOMConstMonadIterator::next(void)
{
	ASSERT_THROW(hasNext(), "hasNext() is false");
	monad_m result = m_cur_monad;
	MonadSetElement mse = m_hint.current();
	m_cur_monad++;
	if (m_cur_monad > mse.last()) {
		m_hint.next(); // Will advance it, but return the current on.
		if (m_hint.hasNext()) {
			m_cur_monad = m_hint.current().first();
		} else {
			m_cur_monad = MAX_MONAD;
		}
	}
	return result;
}

/** Assignment operator
 *
 * @param other the other SOMConstMonadIterator.
 * 
 * @return ourselves.
 */
SOMConstMonadIterator& SOMConstMonadIterator::operator=(const SOMConstMonadIterator& other)
{
	assign(other);
	return *this;
}

/** Assignment method.
 *
 * Used in both operator= and the copy constructor.
 *
 * @param other the other SOMConstMonadIterator.
 */
void SOMConstMonadIterator::assign(const SOMConstMonadIterator& other)
{
	m_cur_monad = other.m_cur_monad;
	m_hint = other.m_hint;
}








//////////////////////////////////////////////////////////////////
//
//  class MonadSetElement 
//
//  UTILITY FUNCTIONS
//
//////////////////////////////////////////////////////////////////


/** Output operator on std::ostream and MonadSetElements.
 *\ingroup MonadSets
 *
 * @param s the output stream to output to.
 * 
 * @param mse the MonadSetElement to output.
 *
 * @return the std::ostream that we have output the mse to.
 */
std::ostream& operator<<(std::ostream& s, const MonadSetElement& mse)
{
	monad_m mse_first = mse.first();
	monad_m mse_last = mse.last();
	if (mse_first == mse_last)
		s << mse_first;
	else
		s << mse_first << "-" << mse_last;
	s << " ";
	return s;
}



//////////////////////////////////////////////////////////////////
//
//  class SetOfMonads
//
//  A set of monad_m's.
//
//  INVARIANT:
//  For all MonadSetElements mse in the vector, 
//  it is the case that:
//
//     1.1) Either its predecessor prev is empty 
//          (i.e., is not there), OR
//     1.2) prev.last+1 < mse.first,
//          AND
//     2.1) Either its successor succ is empty 
//          (i.e., is not there), OR
//     2.2) mse.last+1 < succ.first 
//
//  so, 
//
//  FOR ALL mse IN vector: (FOR ALL prev before mse: prev.last+1 <
//                        mse.first) AND (FOR ALL succ after mse:
//                        mse.last+1 < succ.first )
//
//  where the two nested FOR ALL quantifiers take care of 1.1 and 2.1 
//  by being vacuously true when prev and succ are not there.
//
//////////////////////////////////////////////////////////////////

/** Copy constructor.
 */
SetOfMonads::SetOfMonads(const SetOfMonads& other) 
{
	copyOther(other);
}

/** Constructor from FastSetOfMonads.
 */
SetOfMonads::SetOfMonads(const FastSetOfMonads& other) 
{
	m_first = MAX_MONAD;
	m_last = 0;
	FastSOMConstIterator ci = other.const_iterator();
	while (ci.hasNext()) {
	        addMSE(ci.next());
	}
}

/** Constructor from an array of first/last monad_m pairs.
 *
 * @param pMem Pointer to the first member of the array.
 *
 * @param size The length of the array.
 */
SetOfMonads::SetOfMonads(monad_m *pMem, long size)
{
	m_first = MAX_MONAD;
	m_last = 0;
	for (long i = 0; i < size; ++i) {
		monad_m first = *pMem++;
		monad_m last = *pMem++;
		add(first,last);
	}
}

/** Destructor.
 */
SetOfMonads::~SetOfMonads()
{
}



//
// Add a single mse safely.
//
/** Add a MonadSetElement to the set.
 *
 * This is equivalent to a union with the set of monads represented by
 * the MonadSetElement.
 *
 * @param mse The MonadSetElement to union with.
 */
void SetOfMonads::addMSE(MonadSetElement mse)
{
	// Get mse's first and last
	monad_m mse_first = mse.first_m;
	monad_m mse_last = mse.last_m;

	if (isEmpty()) {
		// If we are empty, simply push it at the end
		monad_ms.push_back(mse);
	} else if (mse_first > (m_last+1)) {
		// If it occurs after m_last, simply push it at the end
		monad_ms.push_back(mse);
	} else if (mse_first >= monad_ms.back().first()) {
		// If it is not empty, and it is adjacent to or
		// part_of the last one, simply extend the last one
		// using subsume.
		monad_ms.back().subsume(mse);
	} else if (mse_last == (m_first-1)) {
		monad_ms.front().subsume(mse);
	} else {
		// Otherwise, insert carefully...
		MonadSetElement current_mse = mse;
		SOMIterator cur = monad_ms.iterator();

		while (cur.hasNext()) {
			MonadSetElement itmse = cur.current();
			if (current_mse.overlap(itmse) 
			    || current_mse.adjacent(itmse)) {
				current_mse.subsume(itmse);
				cur = monad_ms.erase(cur);
			} else if (cur.current().past(current_mse)) {
				break;
			} else {
				cur.next();
			}
		}
		monad_ms.insertBefore(cur, current_mse);
	}

	// Take care of setting m_first and m_last
	if (mse_first < m_first)
		m_first = mse_first;
	if (mse_last > m_last)
		m_last = mse_last;
}

//
// Put to ostream
// Named so because we don't want it to be ambiguous
// in CObject_dm's whether to call
// CObject_d::put or SetOfMonads::put.
//
/** Put set to a stream in console output kind.
 *
 * This can be used, e.g., with std::ostringstream.
 *
 * \see SetOfMonads::toString().
 *
 * @param s The std::ostream that the set should be put to.
 *
 * @return The std::ostream that was passed.
 */
std::ostream& SetOfMonads::putme(std::ostream& s) const
{
	s << " { ";
	SOMConstIterator i = monad_ms.const_iterator();
	while (i.hasNext()) {
		i.next().putme(s);
		if (i.hasNext())
			s << ", ";
	}
	s << " } ";
	return s;
}

/** Convert from the canonical string representation.
 *
 * Clears the set (if non-empty) and lets us become the set found in
 * instr.
 *
 * \see SetOfMonads::toString()
 *
 * @param instr The string representing the set to be converted.
 */
void SetOfMonads::fromString(const std::string& instr)
{
	clear();
	enum eFSState { kNone, kFirst, kLast, kFinished };
	eFSState state = kNone;
	std::string::size_type length = instr.length();
	monad_m first = -1;
	monad_m last = -1;
	for (std::string::size_type i = 0;
	     i < length;
	     ++i) {
		char c = instr[i];
		switch (c) {
		case '{':
			state = kFirst;
			break;
		case '-':
			state = kLast;
			break;
		case ',':
		case '}':
			if (state == kLast) {
				if (first != -1 && last != -1) {
					add(first, last);
				}
			} else if (state == kFirst) {
				if (first != -1) {
					add(first);
				}
			} else {

			}
			if (c == ',') {
				first = last = -1;
				state = kFirst;
			} else if (c == '}') {
				state = kFinished;
			}
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			if (state == kFirst) {
				if (first == -1) {
					first = c - '0';
				} else {
					first = (first * 10) + (c - '0');
				}
			} else if (state == kLast) {
				if (last == -1) {
					last = c - '0';
				} else {
					last = (last * 10) + (c - '0');
				}
			} else {

			}
			break;
		case ' ':
		case '\n':
		case '\r':
		case '\t':
		case '\v':
			// Ignore whitespace
			break;
		default:
			// Ignore all others
			break;
		}
	}
}


/** Convert to a string representation.
 *
 * This is the same as the console output kind.  Uses
 * SetOfMonads::putme().
 *
 * \see SetOfMonads::putme.
 *
 * @return The string representation.
 */
std::string SetOfMonads::toString() const
{
	// Just use putme
	std::ostringstream ostr;
	putme(ostr);
	return ostr.str();

	/*
	 * The following is actually slower than using putme.
	 *
	 */
	/*
	  out = " { ";
	  MSEList::const_iterator i = begin();
	  while (i != end()) {
	  monad_m mse_first = i->first();
	  monad_m mse_last = i->last();
	  if (mse_first == mse_last)
	  out += monad_m2string(mse_first);
	  else
	  out += monad_m2string(mse_first) + "-" + monad_m2string(mse_last);
	  out += " ";
	  ++i;
	  if (i != end())
	  out += ", ";
	  }
	  out += "} ";
	*/
}

/** Test for set equality with another SetOfMonads.
 *
 * \p NOTE: This is not SWIG-wrapped.
 *
 * @param other The other set to test for equality with.
 *
 * @return true on both sets the same, false otherwise.
 */
bool SetOfMonads::operator==(const SetOfMonads& other) const
{
	if (isEmpty()) {
		if (other.isEmpty()) 
			return true;
		else
			return false;
	}

	// Return at once if first/last are not the same
	if (m_first != other.m_first
	    || m_last != other.m_last)
		return false;

	SOMConstIterator us, them;

	us = monad_ms.const_iterator();
	them = other.monad_ms.const_iterator();

	while (us.hasNext() && them.hasNext()) {
		if (!(us.next() == them.next())) 
			return false;
	}

	if (!us.hasNext() && !them.hasNext())
		return true;
	else
		return false;
}

/** Get a std::list<monad_m> of all the monads in the set, in
 * ascending order.
 *
 * @param monad_list the output list.
 */
void SetOfMonads::getMonad_mList(std::list<monad_m>& monad_list) const
{
	SOMConstIterator i = monad_ms.const_iterator();
	while (i.hasNext()) {
		monad_m m;
		MonadSetElement curmse = i.next();
		for (m = curmse.first_m; m <= curmse.last_m; ++m) {
			monad_list.push_back(m);
		}
	}
}

/** Get a std::vector<monad_m> of all the monads in the set, in
 * ascending order.
 *
 * @param result the output vector.
 */
void SetOfMonads::getMonad_mVector(std::vector<monad_m>& result) const
{
	SOMConstIterator i = monad_ms.const_iterator();
	while (i.hasNext()) {
		monad_m m;
		MonadSetElement curmse = i.next();
		for (m = curmse.first_m; m <= curmse.last_m; ++m) {
			result.push_back(m);
		}
	}
}


/** Assignment operator.
 *
 * Clears ourselves and then copies \p other into ourselves.
 * Does not take over other: It is a clean copy.
 *
 * NOTE: Is not SWIG-wrapped.
 *
 * @param other the set to copy from.
 *
 * @return Ourselves after the assignment.
 */
SetOfMonads& SetOfMonads::operator=(const SetOfMonads& other)
{
	// Clear if not empty
	if (!isEmpty()) 
		clear();

	copyOther(other);

	// Return ourselves
	return *this;

}

long SetOfMonads::getCardinality() const
{
	long cardinality = 0;
	SOMConstIterator ci = this->const_iterator();
	while (ci.hasNext()) {
		MonadSetElement mse(ci.next());
		cardinality += 1 + mse.last() - mse.first();
	}

	return cardinality;
}


/** assignment method.
 *
 * NOTE: This assumes that we ourselves are empty!
 *
 * @param other The other SetOfMonads from which to get ourselves.
 */
void SetOfMonads::copyOther(const SetOfMonads& other) 
{
	SOMConstIterator ci = other.const_iterator();
	while (ci.hasNext()) {
		monad_ms.push_back(ci.next());
	}
  
	// Set first/last
	m_first = other.m_first;
	m_last = other.m_last;
}

/** Add a single monad to the set.
 *
 * @param monad the monad to add.
 */
void SetOfMonads::add(monad_m monad)
{
	MonadSetElement mse(monad);
	addMSE(mse);
}

/** Add a monad range to the set.
 *
 * @param first the first monad of the range to add.
 * @param last the first monad of the range to add.
 */
void SetOfMonads::add(monad_m first, monad_m last)
{
	MonadSetElement mse(first, last);
	addMSE(mse);
}

// part_of:
// Returns: bool, "Are we part_of other?"
//
// Explanation:
// There are six cases:
//
//               +-------------+
//               |      B      |
//               +-------------+
//    +---+  +-----+  +---+  +-----+  +---+
// A: | 1 |  |  2  |  | 3 |  |  4  |  | 5 |
//    +---+  +-----+  +---+  +-----+  +---+
//          +-----------------------+
//       A: |           6           |
//          +-----------------------+
//
//  We start by comparing the first A to the first B.
//
//  In case 1, we can return because A has monads outside B
//  In case 2, we can return because A has monads outside B
//  In case 4, we can return because A has monads outside B
//  In case 6, we can return because A has monads outside B
//  In case 3, we need to go on to the next A (but not necessarily B,
//             see 5)
//  In case 5, we need to advance B, because we have no decisive 
//             information, though we do not also advance A.
//
//  If we get to the end of A it is because all have been "inside" some
//             MSE in B.  Thus the loop terminates if all are "inside".
//  If we get to the end of B without getting to the end of A, it is 
//             because some element of A was to the right of all elements
//             in B.  Thus A had elements that were not in B.
//
//  Note that the logical and of the negations of all of the first 5
//             implies the sixth.
//
/** Test for part_of (set inclusion).
 *
 * Tests whether we are part_of \p other.  That is, whether we are a
 * subset of \p other.
 *
 * Tests whether \f$S \subseteq O\f$ where \f$S\f$ is outselves and \f$O\f$ is \p
 * other.
 *
 * @param other the set to see if we are a subset of.
 *
 * @return true if we are a subset, false otherwise.
 */
bool SetOfMonads::part_of(const SetOfMonads& other) const
{
	if (m_first >= other.m_first && m_last <= other.m_last) {
		if (other.monad_ms.lengthIs1()) {
			return true;
		}
	} else {
		return false;
	}

	// Get iterators
	SOMConstIterator A, B;
	A = monad_ms.const_iterator();
	B = other.monad_ms.const_iterator();

	// Get A/B first/last
	monad_m A_first_m, B_first_m;
	monad_m A_last_m, B_last_m;
	MonadSetElement curAmse(A.current()), curBmse(B.current());
	A_first_m = curAmse.first_m;
	A_last_m = curAmse.last_m;
	B_first_m = curBmse.first_m;
	B_last_m = curBmse.last_m;

	// Iterate
	while (A.hasNext() && B.hasNext()) {
		if (A_last_m < B_first_m) {
			return false;                     // 1
		} else if (A_first_m > B_last_m) {
			B.next();                              // 5
			if (B.hasNext()) {
				curBmse = B.current();
				B_first_m = curBmse.first_m;
				B_last_m = curBmse.last_m;
			}
		} else if (A_first_m >= B_first_m 
			   && A_last_m <= B_last_m) {
			A.next();                         // 3
			if (A.hasNext()) {
				curAmse = A.current();
				A_first_m = curAmse.first_m;
				A_last_m = curAmse.last_m;
			}
		} else if (A_first_m < B_first_m
			   && A_last_m > B_last_m) {
			return false;                     // 6
		} else if (A_first_m < B_first_m) {
			return false;                     // 2
		} else {
			return false;                     // 4
		}
	}
	if (!A.hasNext())
		return true;
	else
		return false;
}

/** Test for starts_in.
 *
 * Tests whether we start in \p other. That is, whether our first monad is a member
 * of \p other.
 *
 * @param other the set in which to search for our start monad. 
 *
 * @return true if we start in \p other, false otherwise
 */
bool SetOfMonads::starts_in(const SetOfMonads& other) const
{
    return other.isMemberOf(m_first);
}


// fillInsertRange
// Returns: a monad set with a particular range added 
//
// Explanation:
// 
// We wish to insert from monad first to monad last.
//
// That is, we wish to insert 
//    monads_to_add := last - first + 1
// monads, starting at monad first.
//
// Let S be ourselves.  The result is S'
//
// 1. Is S.last() < first?
//    YES: S' := S; Finish. (No monads to insert, since we are before first)
//    NO: Go to step 2.
// 2. Is S.first() >= last?
//    NO: Go to step 3
//    YES: 
//    BEGIN
//       S' := S;
//       For all mse's A in S': Add monads_to_add to A.first and A.last.
//       Finish.
//    END;
// 3. BEGIN
//      S' := /* empty monad set */;
//      For all MSE's A in S:
//        Decide according to the six cases outlined below:
//        Case 1: Add A to S'.
//        Case 2: Add mse(A.first, A.last + monads_to_add) to S'
//        Case 3: Add mse(first, A.last() + monads_to_add) to S' // Add MSE's length
//        Case 4: Add mse(first, A.last() + monads_to_add)  to S'
//        Case 5: Add mse(A.first() + monads_to_add, A.last() + monads_to_add)  to S'
//        Case 6: Add mse(A.first(), A.last() + monads_to_add) to S'
//      Finish;
//    END;
//
// There are six cases:
//
//            first          last
//               +-------------+
//               |             |
//               +-------------+
//    +---+  +-----+  +---+  +-----+  +---+
// A: | 1 |  |  2  |  | 3 |  |  4  |  | 5 |
//    +---+  +-----+  +---+  +-----+  +---+
//          +-----------------------+
//       A: |           6           |
//          +-----------------------+
//
/** Fill-insert a range of monads.
 *
 * This method returns a copy of ourselves, with the given range inserted.
 *
 * This means that we wish to insert "last - first + 1" monads,
 * starting at monad first.
 *
 * If first is > our own m_last, then a copy of ourselves is returned
 * unchanged.
 *
 * @param first The first monad in the range to insert.
 *
 * @param last The last monad in the range to insert. Note that last must
 *        be >= first.
 *
 * @return Ourselves, with the range inserted.
 */
SetOfMonads SetOfMonads::fillInsertRange(monad_m first, monad_m last) const
{
	ASSERT_THROW_X(last >= first, BadMonadsException, "SetOfMonads::fillInsertRange: last < first");
	monad_m monads_to_add = last - first + 1;
	if (m_last < first) {
		return SetOfMonads(*this);
	} else if (m_first >= last) {
		SetOfMonads result(*this);
		result.offset(monads_to_add);
		return result;
	} else {
		SetOfMonads result;
		
		SOMConstIterator A = monad_ms.const_iterator();

		monad_m A_first_m;
		monad_m A_last_m;
		MonadSetElement curAmse(A.current());
		A_first_m = curAmse.first_m;
		A_last_m = curAmse.last_m;

		// Iterate
		while (A.hasNext()) {
			if (A_last_m < first) { // 1
				result.addMSE(curAmse);
			} else if (A_first_m > last) { // 5
				result.add(A_first_m + monads_to_add, A_last_m + monads_to_add);
			} else if (A_first_m >= first
				   && A_last_m <= last) { // 3
				result.add(first, A_last_m + monads_to_add);
			} else if (A_first_m < first
				   && A_last_m > last) { // 6
				result.add(A_first_m, A_last_m + monads_to_add);
			} else if (A_first_m < first) {  // 2
				result.add(A_first_m, A_last_m + monads_to_add);
			} else { // 4
				result.add(first, A_last_m + monads_to_add);
			}

			A.next();                    
			if (A.hasNext()) {
				curAmse = A.current();
				A_first_m = curAmse.first_m;
				A_last_m = curAmse.last_m;
			}
		}

		result.add(first, last);

		return result;
	}
}

/** Test for part_of (set inclusion), comparing with a FastSetOfMonads.
 *
 * Tests whether we are part_of \p other.  That is, whether we are a
 * subset of \p other.
 *
 * Tests whether \f$S \subseteq O\f$ where \f$S\f$ is outselves and \f$O\f$ is \p
 * other.
 *
 * @param other the set to see if we are a subset of.
 *
 * @return true if we are a subset, false otherwise.
 */
bool SetOfMonads::part_of(const FastSetOfMonads& other) const
{
	if (m_first >= other.m_first && m_last <= other.m_last) {
		if (other.hasOnlyOneMSE()) {
			return true;
		}
	} else {
		return false;
	}

	// Get iterators
	SOMConstIterator A;
	A = monad_ms.const_iterator();

	// Iterate
	while (A.hasNext()) {
		if (!other.subsumesRange(A.next())) {
			return false;
		}
	}
	return true;
}

// Make this = this union other
/** Set union.
 * 
 * Add all the monads of \p other to ourselves, i.e., make ourselves
 * the union of ourselves and other.
 *
 * Equivalent to \f$S ::= S \cup O\f$ where \f$S\f$ is ourselves and \f$O\f$ is \p
 * other.
 *
 * @param other the set to union with.
 */
void SetOfMonads::unionWith(const SetOfMonads& other)
{
	SOMConstIterator O = other.monad_ms.const_iterator();

	while (O.hasNext()) {
		addMSE(O.next());
	}
}

/** Test for monad membership.
 *
 * Tests whether \p m is a member of ourselves.  That is, whether \f$m
 * \in S\f$, where \f$S\f$ is ourselves.
 *
 * @param m the monad to test whether it is a member of ourselves.
 *
 * @return true if \f$m \in S\f$, where \f$S\f$ is ourselves, false otherwise.
 */
bool SetOfMonads::isMemberOf(monad_m m) const
{
	SOMConstIterator O = monad_ms.const_iterator();

	while (O.hasNext()) {
		MonadSetElement curOmse(O.next());
		monad_m Ofirst = curOmse.first_m;
		if (m >= Ofirst && m <= curOmse.last_m)
			return true;
		if (m < Ofirst)
			return false;
	}

	return false;
}

/** Test for monad range overlap.
 *
 * Tests whether \p {first_m-last_m} overlaps with ourselves.  That
 * is, whether the intersction of {first_m-last_m} and ourselves is
 * non-empty.
 *
 * @param first_m the first monad in the range to test
 *
 * @param last_m the last monad in the range to test. Note that last_m
 * must be >= first_m.
 *
 * @return true if \f$\{\mathrm{first_m-last_m}\} \cap S \neq
 * \emptyset\f$, where \f$S\f$ is ourselves, false otherwise.
 */
bool SetOfMonads::hasMonadsInRange(monad_m first_m, monad_m last_m) const
{
	SOMConstIterator O = monad_ms.const_iterator();
	MonadSetElement other(first_m, last_m);

	while (O.hasNext()) {
		MonadSetElement curOmse(O.next());
		if (curOmse.overlap(other)) {
			return true;
		} 
		if (last_m < curOmse.first()) {
			return false;
		}
	}

	return false;
}

bool SetOfMonads::hasLargerGap(monad_m than_this)
{
	if (isEmpty()) {
		return false;
	} else if ((m_last - m_first) < than_this) {
		return false;
	} else if (hasOnlyOneMSE()) {
		return false;
	}

	SOMConstIterator ci = const_iterator(); 
	if (ci.hasNext()) {
		MonadSetElement first_mse = ci.next();
		monad_m prev_monad = first_mse.last();

		while (ci.hasNext()) {
			MonadSetElement mse = ci.next();
			monad_m cur_monad = mse.first();
			if ((cur_monad - prev_monad) > than_this) {
				return true;
			}
			prev_monad = mse.last();
		}

	}
	return false;
}


/** Test for monad membership.
 *
 * Tests whether \p m is a member of ourselves.  That is, whether \f$m
 * \in S\f$, where \f$S\f$ is ourselves.
 *
 * @param m the monad to test whether it is a member of ourselves.
 *
 * @param hint_it a SOMConstIterator which gives a hint as to where to
 * look.  This must not be pointing to an MSE after m, so a safe bet
 * is to start with the beginning iterator (call
 * SOM.const_iterator()).  It will be updated to the next MSE if need
 * be.)
 *
 * @return true if \f$m \in S\f$, where \f$S\f$ is ourselves, false otherwise.
 */
bool SetOfMonads::isMemberOf(monad_m m, SOMConstIterator& hint_it) const
{
	if (m > last()) {
		// Forward hint_it to past the last one, thereby signalling
		// that we're done.
		while(hint_it.hasNext()) {
			hint_it.next();
		}
		return false;
	} else {
		// Rewind to start if we are past (which we should not be!)
		if (hint_it.current().first() > m) {
			hint_it = const_iterator();
		}

		// Advance hint_it until it is not the case that
		// hint_it.current().last() < m.
		// That is, until hint_it.current().last() >= m.
		while(hint_it.hasNext() && hint_it.current().isBefore(m)) {
			hint_it.next();
		}

		if (!hint_it.hasNext()) {
			return false; 
		} else {
			if (hint_it.current().first() <= m) {
				return true;
			} else {
				return false;
			}
		}
	}
}



//
// Removes the monads of other from us
//
/** Set difference.
 *
 * Removes the monads of \p other from ourselves.
 *
 * Equivalent to \f$S ::= S-O\f$ where \f$S\f$ is ourselves and \f$O\f$ is \p
 * other.
 *
 * @param other The other monad set, containing the monads to remove
 * from ourselves.
 */
void SetOfMonads::difference(const SetOfMonads& other)
{
	SOMConstIterator i = other.monad_ms.const_iterator();
	while (i.hasNext()) {
		removeMSE(i.next());
	}
}

// RemoveMSE
//
// Removes the monads of an MSE B from ourselves.
//
// Explanation:
// There are six cases:
//
//               +-------------+
//               |      B      |
//               +-------------+
//    +---+  +-----+  +---+  +-----+  +---+
// A: | 1 |  |  2  |  | 3 |  |  4  |  | 5 |
//    +---+  +-----+  +---+  +-----+  +---+
//          +-----------------------+
//       A: |           6           |
//          +-----------------------+
//
// We go through the list of MSEs.
// A is the current one.
// We start at the beginning.
//
//  In case 1, we go on to the next A
//  In case 2, we truncate A: A.last = B.first-1 and go on 
//             to the next one
//  In case 3, we delete A and go on to the next one
//  In case 4, we truncate A: A.first = B.last+1 and go on
//             to the next one
//  In case 5, we break the while-loop, since we are done
//  In case 6, we split A and stop the while-loop, since we are done:
//             oldlast := A.last;  A.last := B.first-1; 
//             insert new A(B.last+1,oldlast) after A;
//             break;
//
//  Note that the logical and of the negations of all of the first 5
//             implies the sixth.
//
/** Remove a range from the set.
 *
 * Remove the range of monads represented by the stretch of monads from \p
 * B_first to \p B_last.
 *
 * Equivalent to \f$S ::= S-\{\mathtt{B\_first}..\mathtt{B\_last}\}\f$
 * where \f$S\f$ is ourselves.
 *
 * NOTE: This is not SWIG_wrapped.  Use
 * SetOfMonads::removeMSE(MonadSetElement) instead.
 *
 * \see SetOfMonads::removeMSE(MonadSetElement).
 *
 * @param B_first the first monad of the range to remove.
 * @param B_last the last monad of the range to remove.
 */
void SetOfMonads::removeMSE(monad_m B_first, monad_m B_last)
{
	if (isEmpty())
		return;
	else {
		SOMIterator A = monad_ms.iterator();

		// Get A first/last
		MonadSetElement curmse = A.current();
		monad_m A_first_m = curmse.first_m;
		monad_m A_last_m = curmse.last_m;

		while (A.hasNext()) {
			if (A_last_m < B_first) {     // 1 
				A.next();			    
				if (A.hasNext()) {
					curmse = A.current();
					A_first_m = curmse.first_m;
					A_last_m = curmse.last_m;
				}
			} else if (A_first_m > B_last) {
				   break;                      // 5
			} else if (A_first_m >= B_first 
				   && A_last_m <= B_last) {     // 3
				A = monad_ms.erase(A);
				if (A.hasNext()) {
					curmse = A.current();
					A_first_m = curmse.first_m;
					A_last_m = curmse.last_m;
				}
			} else if (A_first_m < B_first
				   && A_last_m > B_last) {
				monad_m oldlast = A_last_m;      // 6
				A.current().truncate(B_first, B_last);
				MonadSetElement newmse(B_last+1, oldlast);
				monad_ms.insertAfter(A, newmse);
				break;
			} else if (A_first_m < B_first) {
				A.next().truncate(B_first, B_last);	// 2
				if (A.hasNext()) {
					curmse = A.current();
					A_first_m = curmse.first_m;
					A_last_m = curmse.last_m;
				}
			} else {
				A.next().truncate(B_first, B_last);   // 4
				if (A.hasNext()) {
					curmse = A.current();
					A_first_m = curmse.first_m;
					A_last_m = curmse.last_m;
				}
			} 
		}
	}

	// Take care of setting m_first and m_last
	if (isEmpty()) {
		m_first = MAX_MONAD;
		m_last = 0;
	} else {
		m_first = monad_ms.front().first_m;
		m_last = monad_ms.back().last_m;
	}
}

/** Remove a MonadSetElement.
 *
 * Uses SetOfMonads::removeMSE(monad_m, monad_m) to remove \p B.
 *
 * @param B the MonadSetElement to remove.
 */
void SetOfMonads::removeMSE(const MonadSetElement& B)
{
	removeMSE(B.first_m, B.last_m);
}

// Intersect
// Returns the intersection of Aset and Bset.
//
// Explanation:
// There are six cases:
//
//               +-------------+
//               |      B      |
//               +-------------+
//    +---+  +-----+  +---+  +-----+  +---+
// A: | 1 |  |  2  |  | 3 |  |  4  |  | 5 |
//    +---+  +-----+  +---+  +-----+  +---+
//          +-----------------------+
//       A: |           6           |
//          +-----------------------+
//
//  We start by comparing the first A to the first B.
//
//  In case 1, 
//             - Go to the next A
//  In case 2, 
//             - add B.first_m..A.last_m
//             - go to the next A
//  In case 3, 
//             - add A
//             - go to the next A
//  In case 4, 
//             - add A.first_m..B.last_m
//             - go to the next B
//  In case 5, 
//             - go to the next B
//  In case 6, 
//             - add B
//             - go to the next B.
//
//
//  Note that the logical and of the negations of all of the first 5
//             implies the sixth.
//
/** Set intersection
 *
 * Intersects \p Aset with \p Bset and returns the result.
 *
 * Equivalent to \f$\mathrm{return}\quad \mathtt{Aset} \cap \mathtt{Bset}\f$.
 *
 * NOTE: This is a static method, so we cannot do anything with
 * "ourselves": There is no "ourselves" to tamper with.
 *
 * @param Aset the first set to do intersection on.
 * @param Bset the second set to do intersection on.
 * @return the set intersection of Aset and Bset.
 */
SetOfMonads SetOfMonads::intersect(const SetOfMonads& Aset, const SetOfMonads& Bset)
{
	// Declare empty result
	SetOfMonads result;

	// If either is empty, return the empty result
	if (Aset.isEmpty()
	    || Bset.isEmpty()) {
		return result;
	}

	// Get iterators
	SOMConstIterator A = Aset.monad_ms.const_iterator();
	SOMConstIterator B = Bset.monad_ms.const_iterator();

	// Get A/B first/last
	MonadSetElement curAmse(A.current()), curBmse(B.current());
	monad_m A_first_m, B_first_m;
	monad_m A_last_m, B_last_m;
	A_first_m = curAmse.first_m;
	A_last_m = curAmse.last_m;
	B_first_m = curBmse.first_m;
	B_last_m = curBmse.last_m;

	// Iterate
	while (A.hasNext() && B.hasNext()) {
		if (A_last_m < B_first_m) {      // 1
			//  In case 1, 
			//             - Go to the next A
			A.next();
			if (A.hasNext()) {
				curAmse = A.current();
				A_first_m = curAmse.first_m;
				A_last_m = curAmse.last_m;
			}
		} else if (A_first_m > B_last_m) {   // 5
			//  In case 5, 
			//               - go to the next B
			B.next();
			if (B.hasNext()) {
				curBmse = B.current();
				B_first_m = curBmse.first_m;
				B_last_m = curBmse.last_m;
			}
		} else if (A_first_m >= B_first_m 
			   && A_last_m <= B_last_m) { // 3
			//  In case 3, 
			//             - add A
			//             - go to the next A
			result.addMSE(A.next());
			if (A.hasNext()) {
				curAmse = A.current();
				A_first_m = curAmse.first_m;
				A_last_m = curAmse.last_m;
			}
		} else if (A_first_m < B_first_m
			   && A_last_m > B_last_m) {
			//  In case 6,                        // 6
			//             - add B
			//             - go to the next B.
			result.addMSE(B.next());
			if (B.hasNext()) {
				curBmse = B.current();
				B_first_m = curBmse.first_m;
				B_last_m = curBmse.last_m;
			}

		} else if (A_first_m < B_first_m) {  // 2
			//  In case 2, 
			//             - add B.first_m..A.last_m
			//             - go to the next A
			result.add(B_first_m, A_last_m);
			A.next();
			if (A.hasNext()) {
				curAmse = A.current();
				A_first_m = curAmse.first_m;
				A_last_m = curAmse.last_m;
			}
		} else { // 4
			//  In case 4, 
			//             - add A.first_m..B.last_m
			//             - go to the next B
			result.add(curAmse.first_m, curBmse.last_m);
			B.next();
			if (B.hasNext()) {
				curBmse = B.current();
				B_first_m = curBmse.first_m;
				B_last_m = curBmse.last_m;
			}
		} 
	}
	return result;
}

// Overlap
// Returns true if Aset and Bset have common monads
//
// Explanation:
// There are six cases:
//
//               +-------------+
//               |      B      |
//               +-------------+
//    +---+  +-----+  +---+  +-----+  +---+
// A: | 1 |  |  2  |  | 3 |  |  4  |  | 5 |
//    +---+  +-----+  +---+  +-----+  +---+
//          +-----------------------+
//       A: |           6           |
//          +-----------------------+
//
//  We start by comparing the first A to the first B.
//
//  In case 1, 
//             - Go to the next A
//  In case 2, 
//             - return true
//  In case 3, 
//             - return true
//  In case 4, 
//             - return true
//  In case 5, 
//             - go to the next B
//  In case 6, 
//             - return true
//
//
//  Note that the logical and of the negations of all of the first 5
//             implies the sixth.
//
/** Set overlap
 *
 * Returns true if \p Aset and \p Bset have common monads
 *
 * NOTE: This is a static method, so we cannot do anything with
 * "ourselves": There is no "ourselves" to tamper with.
 *
 * @param Aset the first set to compare for overlap
 * @param Bset the second set to compare for overlap
 * @return true if the two sets have at least one common monad, 
 *         false otherwise
 */
bool SetOfMonads::overlap(const SetOfMonads& Aset, 
			  const SetOfMonads& Bset)
{
	// If either is empty, return false
	if (Aset.isEmpty()
	    || Bset.isEmpty()) {
		return false;
	}

	// Get iterators
	SOMConstIterator A = Aset.monad_ms.const_iterator();
	SOMConstIterator B = Bset.monad_ms.const_iterator();

	// Get A/B first/last
	MonadSetElement curAmse(A.current()), curBmse(B.current());
	monad_m A_first_m, B_first_m;
	monad_m A_last_m, B_last_m;
	A_first_m = curAmse.first_m;
	A_last_m = curAmse.last_m;
	B_first_m = curBmse.first_m;
	B_last_m = curBmse.last_m;

	// Iterate
	while (A.hasNext() && B.hasNext()) {
		if (A_last_m < B_first_m) {      // 1
			//  In case 1, 
			//             - Go to the next A
			A.next();
			if (A.hasNext()) {
				curAmse = A.current();
				A_first_m = curAmse.first_m;
				A_last_m = curAmse.last_m;
			}
		} else if (A_first_m > B_last_m) {   // 5
			//  In case 5, 
			//               - go to the next B
			B.next();
			if (B.hasNext()) {
				curBmse = B.current();
				B_first_m = curBmse.first_m;
				B_last_m = curBmse.last_m;
			}
		} else if (A_first_m >= B_first_m 
			   && A_last_m <= B_last_m) { // 3
			//  In case 3, 
			//            - return true
			return true;
		} else if (A_first_m < B_first_m
			   && A_last_m > B_last_m) {
			//  In case 6, 
			//            - return true
			return true;
		} else if (A_first_m < B_first_m) { // 2
			//  In case 2, 
			//            - return true
			return true;
		} else { // 4
			//  In case 4, 
			//            - return true
			return true;
		} 
	}
	return false;
}


//
// Returns the answer to the question:
// "Does a gap exist in ourselves which starts at Sm?
// If the result is true, m will be set to the end of the gap.
//
// The algorithm works by looking at each MSE in the list and
// looking at Sm's position in relation to mse.last_m
//
//    +-----------+
//    |    mse    +
//    +-----------+
//      ^          ^           ^
//    false      true        go on
//   (<last)   (=last+1)   (>last+1)
//
/** Tests whether a gap exists, starting at \p Sm.
 *
 * "Does a gap exist in ourselves which starts at Sm?
 * If the result is true, m will be set to the end of the gap.
 *
 * A gap is defined as a maximal stretch of monads that is "not there"
 * in a set of monads.  The starting monad \p Sm must be right after
 * the end of a MonadSetElement in the set.  If \p Sm starts somewhere
 * in the middle of a gap, we return false.  Neither is it a gap if \p
 * Sm starts right after the end of the set: A gap is always bounded
 * on both sides.
 *
 * The algorithm works by looking at each MSE in the list and
 * looking at Sm's position in relation to mse.last_m
 *
 \verbatim
 +-----------+
 |    mse    +
 +-----------+
 ^          ^           ^
 false      true        go on
 (<last)   (=last+1)   (>last+1)
 \endverbatim
 *
 * @param Sm the starting monad at which we wish to test for a gap's presence.
 *
 * @param m used to return the end of the gap if present (only valid
 * if we return true).
 *
 * @return true if there is a gap, false if not.
 */
bool SetOfMonads::gapExists(monad_m Sm, monad_m& m) const
{
	// The gap must be part_of the set
	if (Sm < m_first || Sm > m_last)
		return false;

	SOMConstIterator ci = monad_ms.const_iterator();
	while (ci.hasNext()) {
		MonadSetElement curmse(ci.current());
		if (Sm == curmse.last_m+1) {
			// Calculate m as next.first_m-1
			// next is always there, because of the if-clause 
			// at the beginning of the function.
			ci.next();
			curmse = ci.current();
			m = curmse.first_m - 1;
			return true;
		} else if (Sm < curmse.last_m) {
			return false;
		} 
		ci.next();
	}
	return false;
}

/** Offset by \p m monads.
 *
 * Add \p m monads to all the monad set elements in the set.
 *
 * \see MonadSetElement::offset().
 *
 * @param m the number of monads to add to each MonadSetElement.
 */
void SetOfMonads::offset(monad_m m)
{
	SOMIterator i = monad_ms.iterator();
	while (i.hasNext()) {
		i.next().offset(m);
	}
	m_first +=m;
	m_last +=m;
}

/** Make empty.
 *
 * Make ourselves the empty set.
 *
 * Equivalent to \f$S ::= \emptyset\f$.
 */
void SetOfMonads::clear()
{
	monad_ms.clear();
	m_first = MAX_MONAD;
	m_last = 0;
}

#if defined(_MSC_VER)

/* This warning says that monad_m is converted to char, which may
   result in loss of data.  This is perfectly fine for num2string,
   however, since we shift-left and then & the value with 0x3f.  Here
   we turn it off.  We will turn it on again after the function.
*/
#pragma warning ( disable : 4244)
#endif



/** Convert a monad to a compact string.
 *
 * @internal
 *
 * @param result The resulting compact string (output).
 *
 * @param m The monad the convert.
 *
 */
inline void num2string(std::string& result, monad_m m)
{
	char m0,m1,m2,m3,m4,m5;
	m0 = m & 0x3F;
	m1 = (m >> 6) & 0x3f;
	m2 = (m >> 12) & 0x3f;
	m3 = (m >> 18) & 0x3f;
	m4 = (m >> 24) & 0x3f;
	m5 = (m >> 30) & 0x3f;
	bool bBegun = false;
	if (m5 > 0) {
		result += (m5 + '0');
		bBegun = true;
	}
	if ((m4 > 0) || bBegun) {
		result += (m4 + '0');
		bBegun = true;
	}
	if ((m3 > 0) || bBegun) {
		result += (m3 + '0');
		bBegun = true;
	}
	if ((m2 > 0) || bBegun) {
		result += (m2 + '0');
		bBegun = true;
	}
	if ((m1 > 0) || bBegun) {
		result += (m1 + '0');
		bBegun = true;
	}
	if ((m0 > 0) || bBegun) {
		result += (m0 + '0');
	}
}

#if defined(_MSC_VER)

/* This warning says that monad_m is converted to char, which may
   result in loss of data.  This is perfectly fine for num2string,
   however, since we shift-left and then & the value with 0x3f.
   Here we turn it on again.
*/
#pragma warning ( default : 4244)
#endif



/** Transform to compact string
 *
 * @return The compact string
 */
std::string SetOfMonads::toCompactString() const
{
	std::string result;
	SOMConstIterator ci = const_iterator();
	monad_m first_m, last_m;
	monad_m prev = 0;
	while (ci.hasNext()) {
		MonadSetElement mse = ci.next();
		first_m = mse.first();
		num2string(result, first_m - prev);
		prev = first_m;
		last_m = mse.last();
		if (last_m != first_m) {
			result += 'z';
			num2string(result, last_m - prev);
			prev = last_m;
		} else {

		}
		if (ci.hasNext()) {
			result += 'y';
		}
	}
	return result;
}

/** Transform to compact string
 *
 * @return The compact string
 */
std::string SetOfMonads::toCompactStringFM() const
{
	if (isEmpty()) {
		return "zz";
	}
	std::string result;
	SOMConstIterator ci = const_iterator();
	monad_m first_m, last_m;
	monad_m prev = first();
	bool bIsNotFirst = false;
	while (ci.hasNext()) {
		MonadSetElement mse = ci.next();
		first_m = mse.first();
		if (bIsNotFirst) {
			num2string(result, first_m - prev);
		}
		prev = first_m;
		last_m = mse.last();
		if (last_m != first_m) {
			if (bIsNotFirst) {
				result += 'z';
			}
			num2string(result, last_m - prev);
			prev = last_m;
		} else {

		}
		if (ci.hasNext()) {
			bIsNotFirst = true;
			result += 'y';
		}
	}
	return result;
}


/** Add from compact string
 *
 * Note that the set of monads is assumed to have been cleared
 * beforehand if this is an initialization.  This is because we wish
 * to be able to call this efficiently from the constructor that takes
 * a string.
 *
 * @param inStr The compact string to add from
 */
void SetOfMonads::fromCompactString(const std::string& inStr)
{
	std::string::size_type i = 0, end = inStr.size();
	if (end == 0) {
		return;
	}
	monad_m prev = 0;
	monad_m first_m = 0;
	monad_m last_m = 0;
	monad_m cur = 0;
	char last_separator = 'y';
	while (i != end) {
		char c = inStr[i++];
		switch (c) {
		case 'y':
			switch (last_separator) {
			case 'y':
				first_m = cur + prev;
				add(first_m);
				prev = first_m;
				cur = 0;
				break;
			case 'z':
				last_m = cur + prev;
				add(first_m, last_m);
				prev = last_m;
				cur = 0;
			}
			last_separator = c;
			break;
		case 'z':
			first_m = cur + prev;
			prev = first_m;
			cur = 0;
			last_separator = c;
			break;
		default:
			cur = (cur<<6) + (c - '0');
			break;
		}
	}
	switch (last_separator) {
	case 'y':
		add(cur + prev);
		break;
	case 'z':
		add(first_m, cur + prev);
		break;
	}
}


/*
void SetOfMonads::fromCompactString(const std::string& inStr)
{
	std::string::size_type i = 0, end = inStr.size();
	if (end == 0) {
		return;
	}
	monad_m prev = 0;
	monad_m first_m = 0, last_m = 0;
	char c;
	while (i != end) {
		bool bContinue = true;
		first_m = 0;
		while (bContinue) {
			c = inStr[i++];
			if (c == 'z' || c == 'y') {
				bContinue = false;
			} else {
				first_m <<= 6;
				first_m |= c - '0';
			}
			if (i == end) {
				bContinue = false;
			}
		}
		first_m += prev;
		prev = first_m;
		if (i == end) {
			add(first_m);
		} else {
			if (c == 'z') {
				last_m = 0;
				bContinue = true;
				while (bContinue) {
					c = inStr[i++];
					if (c == 'z' || c == 'y') {
						--i;
						bContinue = false;
					} else {
						last_m <<= 6;
						last_m |= c - '0';
					}
					if (i == end) {
						bContinue = false;
					}
				}
				last_m += prev;
				prev = last_m;
				add(first_m, last_m);
				if (i != end) {
					c = inStr[i++];
				}
			} else if (c == 'y') {
				add(first_m);
			}
		}
	}
}

*/

/** Add from compact string and first monad
 *
 * Note that the set of monads is assumed to have been cleared
 * beforehand if this is an initialization.  This is because we wish
 * to be able to call this efficiently from the constructor that takes
 * a string.
 *
 * @param inStr The compact string to add from
 */
void SetOfMonads::fromCompactStringFM(const std::string& inStr, monad_m first_monad)
{
	if (inStr == "zz") {
		// The set is empty.
		return;
	}
	std::string::size_type i = 0, end = inStr.size();
	if (end == 0) {
		add(first_monad);
		return;
	}
	monad_m prev = first_monad;
	monad_m first_m = 0, last_m = 0;
	char c = inStr[0];
	bool bIsNotFirst = false;
	while (i != end) {
		bool bContinue = true;
		if (bIsNotFirst) {
			first_m = 0;
			while (bContinue) {
				c = inStr[i++];
				if (c == 'z' || c == 'y') {
				bContinue = false;
				} else {
					first_m <<= 6;
					first_m |= c - '0';
				}
				if (i == end) {
					bContinue = false;
				}
			}
		}
		first_m += prev;
		prev = first_m;
		if (i == end) {
			add(first_m);
		} else {
			if (c == 'z'
			    || !bIsNotFirst) {
				last_m = 0;
				bContinue = true;
				while (bContinue) {
					c = inStr[i++];
					if (c == 'z' || c == 'y') {
						--i;
						bContinue = false;
					} else {
						last_m <<= 6;
						last_m |= c - '0';
					}
					if (i == end) {
						bContinue = false;
					}
				}
				last_m += prev;
				prev = last_m;
				add(first_m, last_m);
				if (i != end) {
					c = inStr[i++];
				}
			} else if (c == 'y') {
				add(first_m);
			}
			bIsNotFirst = true;
		}
	}
}




/** Print on EMdFOutput using Console style.
 *
 *
 * This is the same method as SetOfMonads::putme().
 *
 * \see SetOfMonads::putme().
 *
 * @param pOut The EMdFOutput object to use.
 */
void SetOfMonads::printConsole(EMdFOutput *pOut) const
{
	pOut->out("{");
	SOMConstIterator ci = monad_ms.const_iterator();
	while (ci.hasNext()) {
		ci.next().printConsole(pOut);
		if (ci.hasNext()) {
			pOut->out(",");
		}
	}
	pOut->out(" }");
}

/** Print on EMdFOutput using XML style.
 *
 * @param pOut The EMdFOutput object to use.
 */
void SetOfMonads::printXML(EMdFOutput *pOut) const
{
	pOut->startTag("monad_set", true);
	SOMConstIterator ci = monad_ms.const_iterator();
	while (ci.hasNext()) {
		ci.next().printXML(pOut);
	}
	pOut->endTag("monad_set", true);
}

/** Print on EMdFOutput using JSON style.
 *
 * @param pOut The EMdFOutput object to use.
 */
void SetOfMonads::printJSON(EMdFOutput *pOut) const
{
    pOut->jsonStartArray();

	SOMConstIterator ci = monad_ms.const_iterator();
	while (ci.hasNext()) {
		ci.next().printJSON(pOut);
	}

    pOut->jsonEndArray();
}

/** Output operator on std::ostream and SetOfMonads
 *\ingroup MonadSets
 *
 * @param str the output stream to output to.
 * 
 * @param som the SetOfMonads to output.
 *
 * @return the std::ostream that we have output the SOM to.
 */
std::ostream& operator<<(std::ostream& str, const SetOfMonads& som)
{
	str << " { ";
	SOMConstIterator ci = som.const_iterator();
	while (ci.hasNext()) {
		str << ci.next();
		if (ci.hasNext())
			str << " , ";
	}
	str << " } ";
	return str;
}

/** Fill gaps.
 *
 * Returns a set of monads which contains all of the monads
 * of self, but which has gaps filled in in such as way that the 
 * minimum size of any gap in the returned set is min_gap.
 *
 * @param min_gap The minimum size of any gaps that are left
 * in the return result after this method.
 *
 * @return A SetOfMonads which is based on us, but which has had its
 * gaps filled as per min_gap.  We ourselves remain untouched.
 */
SetOfMonads SetOfMonads::fillGaps(monad_m min_gap) const
{
	SetOfMonads result;

	// Create iterator and previous monad
	SOMConstIterator ci;
	monad_m prev_last;

	// Add first MSE and advance iterator
	ci = monad_ms.const_iterator();
	MonadSetElement curmse(ci.next());
	result.addMSE(curmse);

	// Set previous monad
	prev_last = curmse.last_m;

	// Iterate through
	while (ci.hasNext()) {
		// Get current and advance iterator
		curmse = ci.next();

		// Should we add gap or just *ci?
		if ((curmse.first_m - prev_last - 1) >= min_gap) {
			// Only add *ci
			result.addMSE(curmse);
		} else {
			// Add from prev_last to curmse.last_m
			result.add(prev_last, curmse.last_m);
		}

		// Set previous last monad
		prev_last = curmse.last_m;
	}

	// Return result
	return result;
}


/** Get a SetOfMonads which has the gaps of ourselves.
 *
 * @return a SetOfMonads consisting of the gaps of in ourselves. (May
 * be empty, if either we are empty, or we have only 1 MSE).
 */
SetOfMonads SetOfMonads::getGaps(void) const
{
	SetOfMonads resultSOM;
	if (isEmpty()) {
		return resultSOM; // An empty SOM
	} else {
		// Get beginning of mselist
		SOMConstIterator ci = monad_ms.const_iterator();
    
		// Set prev_last to last_m of first MSE,
		// and advance iterator to second mse (if there)
		monad_m prev_last = ci.next().last_m;
    
		// Iterate through
		while (ci.hasNext()) {
			// Get current mse and advance iterator
			MonadSetElement curmse(ci.next());
      
			// Get first of this mse
			monad_m this_first = curmse.first_m;
      
			// Push gap to the back
			resultSOM.add(prev_last + 1, this_first-1);
      
			// Set prev_last
			prev_last = curmse.last_m;
		}

		return resultSOM;
	}

}



/** Get SOMConstIterator pointing to beginning.
 *
 * Use this method to obtain a SOMConstIterator which points to the
 * beginning of the set (i.e., the first MonadSetElement).
 *
 * @return a SOMConstIterator pointing to the first MonadSetElement.
 */
SOMConstIterator SetOfMonads::const_iterator() const
{
	return monad_ms.const_iterator();
}



/** Constructor to associate ourselves with a specific FastSetOfMonads.
 *
 * @param som The FastSetOfMonads to be associated with.
 */
FastSOMConstIterator::FastSOMConstIterator(const FastSetOfMonads& som)
	: m_mother(som.monad_ms),
	  m_hint(som.monad_ms.begin())
{
}


/** Copy constructor for FastSOMConstIterator.
 *
 */
FastSOMConstIterator::FastSOMConstIterator(const FastSOMConstIterator& somit)
	: m_mother(somit.m_mother),
	  m_hint(somit.m_hint)
{
}


/** Does the FastSetOfMonads have more MonadSetElement objects?
 *
 * @return true if there are more MonadSetElement objects, false if we
 * have reached the end.  <b>DO NOT</b> call next() if hasNext()
 * returns false!
 */
bool FastSOMConstIterator::hasNext(void)
{
	return m_hint != m_mother.end();
}


/** Get the current MonadSetElement and advance iterator.
 *
 * @see FastSOMConstIterator::current()
 *
 * @return The current MonadSetElement.
 */
MonadSetElement FastSOMConstIterator::next(void)
{
	ASSERT_THROW(m_hint != m_mother.end(), "m_hint has hit end of mother");
	MonadSetElement result(m_hint->second);
	++m_hint;
	return result;
}


/** Get the current MonadSetElement and but do not advance iterator.
 *
 * @see FastSOMConstIterator::next()
 *
 * @return The current MonadSetElement.
 */
MonadSetElement FastSOMConstIterator::current(void)
{
	ASSERT_THROW(m_hint != m_mother.end(), "hint has hit end of mother");
	return m_hint->second;
}


/** Assignment operator.
 *
 * @param other the other FastSOMConstIterator
 * 
 * @return ourselves.
 */
FastSOMConstIterator& FastSOMConstIterator::operator=(const FastSOMConstIterator& other)
{
	assign(other);
	return *this;
}



/** Assignment method.
 *
 * Used in both operator= and the copy constructor.
 *
 * @param other the other FastSOMConstIterator.
 */
void FastSOMConstIterator::assign(const FastSOMConstIterator& other)
{
	m_cur_monad = other.m_cur_monad;
	m_hint = other.m_hint;
}





/** Constructor for empty FastSetOfMonads.
 */
FastSetOfMonads::FastSetOfMonads()
{
	m_first = MAX_MONAD;
	m_last = 0;
}


/** Constructor from SetOfMonads
 *
 * @param other The other SetOfMonads to set ourselves equal to.
 */
FastSetOfMonads::FastSetOfMonads(const SetOfMonads& other)
{
	m_first = other.m_first;
	m_last = other.m_last;
	SOMConstIterator ci = other.const_iterator();
	while (ci.hasNext()) {
		MonadSetElement curmse = ci.next();
		monad_ms.insert(std::make_pair(curmse.first_m, curmse));
	}
}

/** Copy constructor from FastSetOfMonads
 *
 * @param other The other FastSetOfMonads to set ourselves equal to.
 */
FastSetOfMonads::FastSetOfMonads(const FastSetOfMonads& other)
{
	assign(other);
}

/** Get FastSOMConstIterator pointing to beginning.
 *
 * Use this method to obtain a FastSOMConstIterator which points to
 * the beginning of the set (i.e., the first MonadSetElement).
 *
 * @return a FastSOMConstIterator pointing to the first
 * MonadSetElement.
 */
FastSOMConstIterator FastSetOfMonads::const_iterator() const
{
	return FastSOMConstIterator(*this);
}

/** Set intersection
 *
 * Intersects \p Aset with \p Bset and returns the result.
 *
 * Equivalent to \f$\mathrm{return}\quad \mathtt{Aset} \cap \mathtt{Bset}\f$.
 *
 * NOTE: This is a static method, so we cannot do anything with
 * "ourselves": There is no "ourselves" to tamper with.
 *
 * @param Aset the first set to do intersection on.
 * @param Bset the second set to do intersection on.
 * @return the set intersection of Aset and Bset.
 */
FastSetOfMonads FastSetOfMonads::intersect(const FastSetOfMonads& Aset, const FastSetOfMonads& Bset)
{
	// Declare empty result
	FastSetOfMonads result;

	// If either is empty, return the empty result
	if (Aset.isEmpty()
	    || Bset.isEmpty()) {
		return result;
	}

	// Get iterators
	FastSOMConstIterator A = Aset.const_iterator();
	FastSOMConstIterator B = Bset.const_iterator();

	// Get A/B first/last
	MonadSetElement curAmse(A.current()), curBmse(B.current());
	monad_m A_first_m, B_first_m;
	monad_m A_last_m, B_last_m;
	A_first_m = curAmse.first_m;
	A_last_m = curAmse.last_m;
	B_first_m = curBmse.first_m;
	B_last_m = curBmse.last_m;

	// Iterate
	while (A.hasNext() && B.hasNext()) {
		if (A_last_m < B_first_m) {      // 1
			//  In case 1, 
			//             - Go to the next A
			A.next();
			if (A.hasNext()) {
				curAmse = A.current();
				A_first_m = curAmse.first_m;
				A_last_m = curAmse.last_m;
			}
		} else if (A_first_m > B_last_m) {   // 5
			//  In case 5, 
			//               - go to the next B
			B.next();
			if (B.hasNext()) {
				curBmse = B.current();
				B_first_m = curBmse.first_m;
				B_last_m = curBmse.last_m;
			}
		} else if (A_first_m >= B_first_m 
			   && A_last_m <= B_last_m) { // 3
			//  In case 3, 
			//             - add A
			//             - go to the next A
			result.addMSE(A.next());
			if (A.hasNext()) {
				curAmse = A.current();
				A_first_m = curAmse.first_m;
				A_last_m = curAmse.last_m;
			}
		} else if (A_first_m < B_first_m
			   && A_last_m > B_last_m) { // 6
			//  In case 6, 
			//             - add B
			//             - go to the next B.
			result.addMSE(B.next());
			if (B.hasNext()) {
				curBmse = B.current();
				B_first_m = curBmse.first_m;
				B_last_m = curBmse.last_m;
			}

		} else if (A_first_m < B_first_m) { // 2
			//  In case 2, 
			//             - add B.first_m..A.last_m
			//             - go to the next A
			result.add(B_first_m, A_last_m);
			A.next();
			if (A.hasNext()) {
				curAmse = A.current();
				A_first_m = curAmse.first_m;
				A_last_m = curAmse.last_m;
			}
		} else { // 4
			//  In case 4, 
			//             - add A.first_m..B.last_m
			//             - go to the next B
			result.add(curAmse.first_m, curBmse.last_m);
			B.next();
			if (B.hasNext()) {
				curBmse = B.current();
				B_first_m = curBmse.first_m;
				B_last_m = curBmse.last_m;
			}
		} 
	}
	return result;
}


/** Add a MonadSetElement to the set.
 *
 * This is equivalent to a union with the set of monads represented by
 * the MonadSetElement.
 *
 * @param mse The MonadSetElement to union with.
 */
void FastSetOfMonads::addMSE(MonadSetElement mse)
{
	// Get mse's first and last
	monad_m mse_first = mse.first();
	monad_m mse_last = mse.last();

	if (isEmpty()) {
		// If we are empty, simply push it at the end
		monad_ms.insert(std::make_pair(mse_first,mse));
	} else if (mse_first > (m_last+1)) {
		// If it occurs after m_last, simply push it at the end
		monad_ms.insert(std::make_pair(mse_first,mse));
	} else if (mse_first >= monad_ms.rbegin()->second.first_m) {
		// If it is not empty, and it is part_of (or 
		// adjecent to) the last one, then...
		// simply extend the last one using subsume.
		MonadSetElement new_mse = monad_ms.rbegin()->second;
		new_mse.subsume(mse);
		monad_ms.erase(--monad_ms.end());
		monad_ms.insert(std::make_pair(new_mse.first(),new_mse));
	} else if (mse_last == (m_first-1)) {
		MonadSetElement new_mse = monad_ms.begin()->second;
		new_mse.subsume(mse);
		monad_ms.erase(monad_ms.begin());
		monad_ms.insert(std::make_pair(new_mse.first(),new_mse));
	} else if (mse_last < (m_first - 1)) {
		monad_ms.insert(std::make_pair(mse.first(),mse));
	} else {
		// Otherwise, insert carefully...
		/*
		MonadSetElement current_mse = mse;
		FastSOMIterator cur = monad_ms.begin();

		while (cur != monad_ms.end()) {
			MonadSetElement itmse = cur->second;
			if (current_mse.overlap(itmse) 
			    || current_mse.adjacent(itmse)) {
				current_mse.subsume(itmse);
				FastSOMIterator curCur = cur;
				++cur;
				monad_ms.erase(curCur);
			} else if (cur->second.past(current_mse)) {
				break;
			} else {
				++cur;
			}
		}
		monad_ms.insert(std::make_pair(current_mse.first(),current_mse));
		*/
		MonadSetElement current_mse = mse;
		FastSOMIterator mend = monad_ms.end();
		FastSOMIterator cur = monad_ms.upper_bound(mse_first-1);

		// Move one backwards if this is not the end
		if (cur != monad_ms.begin()) {
			--cur;
		}
		while (cur != mend) {
			MonadSetElement itmse = cur->second;
			if (current_mse.overlap(itmse) 
			    || current_mse.adjacent(itmse)) {
				current_mse.subsume(itmse);
				FastSOMIterator curCur = cur;
				++cur;
				monad_ms.erase(curCur);
			} else if (cur->second.past(current_mse)) {
				break;
			} else {
				++cur;
			}
		}
		monad_ms.insert(std::make_pair(current_mse.first(),current_mse));

	}

	// Take care of setting m_first and m_last
	if (mse_first < m_first) {
 		m_first = mse_first;
	}
	if (mse_last > m_last) {
		m_last = mse_last;
	}

	// Check that first and last are set correctly
	ASSERT_THROW(m_first == monad_ms.begin()->second.first_m
		     && m_last == monad_ms.rbegin()->second.last_m,
		     "First and last not set correctly");
}



/** Add a single monad to the set.
 *
 * @param monad the monad to add.
 */
void FastSetOfMonads::add(monad_m monad)
{
	add(monad,monad);
}


/** Add a monad range to the set.
 *
 * @param first the first monad of the range to add.
 * @param last the first monad of the range to add.
 */
void FastSetOfMonads::add(monad_m first, monad_m last)
{
	addMSE(MonadSetElement(first,last));
}





/** Make empty.
 *
 * Make ourselves the empty set.
 *
 * Equivalent to \f$S ::= \emptyset\f$.
 */
void FastSetOfMonads::clear()
{
	monad_ms.clear();
	m_first = MAX_MONAD;
	m_last = 0;
}


/** Set overlap
 *
 * Returns true if \p Aset and \p Bset have common monads
 *
 * NOTE: This is a static method, so we cannot do anything with
 * "ourselves": There is no "ourselves" to tamper with.
 *
 * @param Aset the first set to compare for overlap
 * @param Bset the second set to compare for overlap
 * @return true if the two sets have at least one common monad, 
 *         false otherwise
 */
bool FastSetOfMonads::overlap(const SetOfMonads& Aset, const FastSetOfMonads& Bset)
{
	// If either is empty, return false
	if (Aset.isEmpty()
	    || Bset.isEmpty()) {
		return false;
	}
       
	// Get iterators
	SOMConstIterator A = Aset.monad_ms.const_iterator();
	
	while (A.hasNext()) {
		MonadSetElement curAmse = A.next();
		if (Bset.hasMonadsInRange(curAmse.first_m, curAmse.last_m)) {
			return true;
		}
	}

	return false;
}

long FastSetOfMonads::getCardinality() const
{
	long cardinality = 0;
	FastSOMConstIterator ci = this->const_iterator();
	while (ci.hasNext()) {
		MonadSetElement mse(ci.next());
		cardinality += 1 + mse.last() - mse.first();
	}

	return cardinality;
}





/** Get a std::vector<monad_m> of all the monads in the set, in
 * ascending order.
 *
 * @param result the output vector
 */
void FastSetOfMonads::getMonad_mVector(std::vector<monad_m>& result) const
{
	FastSOMConstIterator i = const_iterator();
	while (i.hasNext()) {
		monad_m m;
		MonadSetElement curmse = i.next();
		for (m = curmse.first_m; m <= curmse.last_m; ++m) {
			result.push_back(m);
		}
	}
}



/** Test for monad membership.
 *
 * Tests whether \p m is a member of ourselves.  That is, whether \f$m
 * \in S\f$, where \f$S\f$ is ourselves.
 *
 * @param m the monad to test whether it is a member of ourselves.
 *
 * @return true if \f$m \in S\f$, where \f$S\f$ is ourselves, false otherwise.
 */
bool FastSetOfMonads::isMemberOf(monad_m m) const
{
	if (isEmpty()) {
		return false;
	}

	FastSOMMap::const_iterator ci = monad_ms.upper_bound(m-1);

	if (ci == monad_ms.end()) {
		return monad_ms.rbegin()->second.overlap(MonadSetElement(m,m));
	} else {
		if (ci->second.first_m > m) {
			if (ci == monad_ms.begin()) {
				return false;
			} else {
				--ci;
				if (ci->second.first_m <= m && m <= ci->second.last_m) {
					return true;
				} else {
					return false;
				}
			}
		} else if (ci->second.last_m < m) {
			return false;
		} else {
			return true;
		}
	}
}


/** Test for monad range overlap.
 *
 * Tests whether \p {first_m-last_m} overlaps with ourselves.  That
 * is, whether the intersction of {first_m-last_m} and ourselves is
 * non-empty.
 *
 * @param first_m the first monad in the range to test
 *
 * @param last_m the last monad in the range to test. Note that last_m
 * must be >= first_m.
 *
 * @return true if \f$\{\mathrm{first_m-last_m}\} \cap S \neq
 * \emptyset\f$, where \f$S\f$ is ourselves, false otherwise.
 */
bool FastSetOfMonads::hasMonadsInRange(monad_m first_m, monad_m last_m) const
{
	if (isEmpty()) {
		return false;
	} else if (m_first > last_m) {
		return false;
	} else if (m_last < first_m) {
		return false;
	}

	FastSOMMap::const_iterator ci = monad_ms.upper_bound(first_m-1);

	MonadSetElement other(first_m, last_m);

	if (ci == monad_ms.end()) {
		return monad_ms.rbegin()->second.overlap(other);
	} else {
		MonadSetElement curmse(ci->second);

		if (curmse.first_m > last_m) {
			--ci;
			if (ci->second.overlap(other)) {
				return true;
			} else {
				return false;
			}
		} else if (curmse.first_m <= first_m) {
		       return curmse.overlap(other);
		} else {
			return true;
		}
	}
}


bool FastSetOfMonads::hasLargerGap(monad_m than_this)
{
	if (isEmpty()) {
		return false;
	} else if ((m_last - m_first) < than_this) {
		return false;
	} else if (hasOnlyOneMSE()) {
		return false;
	}

	FastSOMConstIterator ci = const_iterator(); 
	if (ci.hasNext()) {
		MonadSetElement first_mse = ci.next();
		monad_m prev_monad = first_mse.last();

		while (ci.hasNext()) {
			MonadSetElement mse = ci.next();
			monad_m cur_monad = mse.first();
			if ((cur_monad - prev_monad) > than_this) {
				return true;
			}
			prev_monad = mse.last();
		}

	}
	return false;
}


/** Test for monad range inclusion.
 *
 * @param range The MonadSetElement to test for.
 * 
 * @return true iff range is a proper subset of one of the
 * MonadSetElements.
 */
bool FastSetOfMonads::subsumesRange(const MonadSetElement& range) const
{
	if (isEmpty()) {
		return false;
	} else if (m_first <= range.first_m
		   && range.last_m <= m_last) {
		if (hasOnlyOneMSE()) {
			return true;
		}
	} else if (m_first > range.last_m) {
		return false;
	} else if (m_last < range.first_m) {
		return false;
	}

	FastSOMMap::const_iterator ci = monad_ms.upper_bound(range.first_m-1);

	if (ci == monad_ms.end()) {
		return monad_ms.rbegin()->second.subsumes(range);
	} else {
		MonadSetElement curmse(ci->second);

		if (curmse.first_m > range.last_m) {
			--ci;
			return ci->second.subsumes(range);
		} else {
		       return curmse.subsumes(range);
		} 
	}
}


/** Fill gaps.
 *
 * Returns a set of monads which contains all of the monads
 * of self, but which has gaps filled in in such as way that the 
 * minimum size of any gap in the returned set is min_gap.
 *
 * @param min_gap The minimum size of any gaps that are left
 * in the return result after this method.
 *
 * @return A SetOfMonads which is based on us, but which has had its
 * gaps filled as per min_gap.  We ourselves remain untouched.
 */
FastSetOfMonads FastSetOfMonads::fillGaps(monad_m min_gap) const
{
	FastSetOfMonads result;

	// Create iterator and previous monad
	monad_m prev_last;

	// Add first MSE and advance iterator
	FastSOMMap::const_iterator ci = monad_ms.begin();
	MonadSetElement curmse(ci->second);
	++ci;
	result.addMSE(curmse);

	// Set previous monad
	prev_last = curmse.last_m;

	// Iterate through
	while (ci != monad_ms.end()) {
		// Get current and advance iterator
		curmse = ci->second;
		++ci;

		// Should we add gap or just *ci?
		if ((curmse.first_m - prev_last - 1) >= min_gap) {
			// Only add *ci
			result.addMSE(curmse);
		} else {
			// Add from prev_last to curmse.last_m
			result.add(prev_last, curmse.last_m);
		}

		// Set previous last monad
		prev_last = curmse.last_m;
	}

	// Return result
	return result;
}



/** Test for set equality with another FastSetOfMonads.
 *
 * \p NOTE: This is not SWIG-wrapped.
 *
 * @param other The other set to test for equality with.
 *
 * @return true on both sets the same, false otherwise.
 */
bool FastSetOfMonads::operator==(const FastSetOfMonads& other) const
{
	if (isEmpty()) {
		if (other.isEmpty()) 
			return true;
		else
			return false;
	}

	// Return at once if first/last are not the same
	if (m_first != other.m_first
	    || m_last != other.m_last)
		return false;

        FastSOMConstIterator myit = const_iterator();
        FastSOMConstIterator otherit = other.const_iterator();
        while (myit.hasNext()
               && otherit.hasNext()) {
                if (!(myit.next() == otherit.next())) {
                        return false;
                }
        }
        return !myit.hasNext() && !otherit.hasNext();
}




/** Convert to a string representation.
 *
 * This is the same as the console output kind.  Uses
 * SetOfMonads::putme().
 *
 * \see SetOfMonads::putme.
 *
 * @return The string representation.
 */
std::string FastSetOfMonads::toString() const
{
	// Just use putme
	std::ostringstream ostr;
	putme(ostr);
	return ostr.str();

	/*
	 * The following is actually slower than using putme.
	 *
	 */
	/*
	  out = " { ";
	  MSEList::const_iterator i = begin();
	  while (i != end()) {
	  monad_m mse_first = i->first();
	  monad_m mse_last = i->last();
	  if (mse_first == mse_last)
	  out += monad_m2string(mse_first);
	  else
	  out += monad_m2string(mse_first) + "-" + monad_m2string(mse_last);
	  out += " ";
	  ++i;
	  if (i != end())
	  out += ", ";
	  }
	  out += "} ";
	*/
}


//
// Put to ostream
// Named so because we don't want it to be ambiguous
// in CObject_dm's whether to call
// CObject_d::put or SetOfMonads::put.
//
/** Put set to a stream in console output kind.
 *
 * This can be used, e.g., with std::ostringstream.
 *
 * \see SetOfMonads::toString().
 *
 * @param s The std::ostream that the set should be put to.
 *
 * @return The std::ostream that was passed.
 */
std::ostream& FastSetOfMonads::putme(std::ostream& s) const
{
	s << " { ";
	FastSOMConstIterator i = const_iterator();
	while (i.hasNext()) {
		i.next().putme(s);
		if (i.hasNext())
			s << ", ";
	}
	s << " } ";
	return s;
}


/**
 * Removes all monads before monad
 *
 * @param monad the first monad not to remove
 *
 * @return a set of monads equal to self, except with all monads
 * before "monad" removed.
 */
FastSetOfMonads FastSetOfMonads::removeMonadsBefore(monad_m monad) const
{
	if (isEmpty() || m_last < monad) {
		return FastSetOfMonads();
	} 

	FastSetOfMonads result(*this);

	monad_m mm1 = monad-1;
	if (m_first <= mm1) { 
		FastSOMMap::iterator it = result.monad_ms.upper_bound(mm1);
		if (it == result.monad_ms.end()) {
			monad_m last_m = result.monad_ms.rbegin()->second.last_m;
			result.clear();
			result.add(monad, last_m);
			result.m_first = monad;
			result.m_last = last_m;
			// Check that first and last got set correctly
			ASSERT_THROW(result.m_first == result.monad_ms.begin()->second.first_m
				     && result.m_last == result.monad_ms.rbegin()->second.last_m,
				     "First and last not set correctly");
			return result;
		} else {
			if (it->second.first_m > monad) {
				--it;
				if (it->second.first_m > monad) {
					ASSERT_THROW(false, "first_m > monad");
					return result;
				} else if (it->second.last_m < monad) {
					++it;
					result.monad_ms.erase(result.monad_ms.begin(), it);
					if (result.monad_ms.begin() == result.monad_ms.end()) {
						result.m_first = MAX_MONAD;
						result.m_last = 0;
					} else {
						result.m_first = result.monad_ms.begin()->second.first_m;
						result.m_last = result.monad_ms.rbegin()->second.last_m;						
					}
					// Check that first and last got set correctly
					ASSERT_THROW(result.m_first == result.monad_ms.begin()->second.first_m
						     && result.m_last == result.monad_ms.rbegin()->second.last_m,
						     "First and last not set correctly");
					return result;
				} else {
					it->second.first_m = monad;
					result.monad_ms.erase(result.monad_ms.begin(), it);
					result.m_first = monad;
					// Check that first and last got set correctly
					ASSERT_THROW(result.m_first == result.monad_ms.begin()->second.first_m
						     && result.m_last == result.monad_ms.rbegin()->second.last_m,
						     "first and last not set correctly");
					return result;
				}
			} else if (it->second.last_m < monad) {
				result.monad_ms.erase(result.monad_ms.begin(), it);
				result.m_first = result.monad_ms.begin()->second.first_m;
				// Check that first and last got set correctly
				ASSERT_THROW(result.m_first == result.monad_ms.begin()->second.first_m
					     && result.m_last == result.monad_ms.rbegin()->second.last_m,
					     "First and last not set correctly");
				return result;
			} else {
				it->second.first_m = monad;
				result.monad_ms.erase(result.monad_ms.begin(), it);
				result.m_first = monad;

				// Check that first and last got set correctly
				ASSERT_THROW(result.m_first == result.monad_ms.begin()->second.first_m
					     && result.m_last == result.monad_ms.rbegin()->second.last_m,
					     "First and last not set correctly");
				return result;
			}
		}
	}

	// Check that first and last got set correctly
	ASSERT_THROW(result.m_first == result.monad_ms.begin()->second.first_m
		     && result.m_last == result.monad_ms.rbegin()->second.last_m,
		     "First and last not set correctly");
	return result;
}


/** Tests whether a gap exists, starting at \p Sm.
 *
 * "Does a gap exist in ourselves which starts at Sm?
 * If the result is true, m will be set to the end of the gap.
 *
 * A gap is defined as a maximal stretch of monads that is "not there"
 * in a set of monads.  The starting monad \p Sm must be right after
 * the end of a MonadSetElement in the set.  If \p Sm starts somewhere
 * in the middle of a gap, we return false.  Neither is it a gap if \p
 * Sm starts right after the end of the set: A gap is always bounded
 * on both sides.
 *
 * The algorithm works by looking at each MSE in the list and
 * looking at Sm's position in relation to mse.last_m
 *
 \verbatim
 +-----------+
 |    mse    +
 +-----------+
 ^          ^           ^
 false      true        go on
 (<last)   (=last+1)   (>last+1)
 \endverbatim
 *
 * @param Sm the starting monad at which we wish to test for a gap's presence.
 *
 * @param m used to return the end of the gap if present (only valid
 * if we return true).
 *
 * @return true if there is a gap, false if not.
 */
bool FastSetOfMonads::gapExists(monad_m Sm, monad_m& m) const
{
	// The gap must be part_of the set
	// (This also catches the empty set,
	// since in that case, m_first = MAX_MONAD
	// (and m_last = 0, so the || is doubly true).
	if (Sm < m_first || Sm > m_last) {
		return false; 
	} else if (hasOnlyOneMSE()) {
		return false;
	}

	FastSOMMap::const_iterator ci = monad_ms.upper_bound(Sm);
	if (ci == monad_ms.end()) {
		// This means that there was none higher than Sm.
		return false;
	} else {
		MonadSetElement curmse(ci->second);
		
		if (curmse.first_m > Sm) {
			m = curmse.first_m - 1;
			--ci;
			return ci->second.last_m + 1 == Sm;
		} else if (curmse.first_m == Sm) {
			return false;
		} else if (curmse.last_m >= Sm) {
			return false;
		} else {
			bool bResult = curmse.last_m + 1 == Sm;
			if (bResult) {
				++ci;
				if (ci == monad_ms.end()) {
					return false;
				} else {
					m = ci->second.first_m - 1;
				}
			}
			return bResult;
		} 

	}
	return false;
}


/** Set union.
 * 
 * Add all the monads of \p other to ourselves, i.e., make ourselves
 * the union of ourselves and other.
 *
 * Equivalent to \f$S ::= S \cup O\f$ where \f$S\f$ is ourselves and \f$O\f$ is \p
 * other.
 *
 * @param other the set to union with.
 */
void FastSetOfMonads::unionWith(const SetOfMonads& other)
{
	SOMConstIterator ci = other.const_iterator();
	while (ci.hasNext()) {
	        addMSE(ci.next());
	}
}

/** Set union.
 * 
 * Add all the monads of \p other to ourselves, i.e., make ourselves
 * the union of ourselves and other.
 *
 * Equivalent to \f$S ::= S \cup O\f$ where \f$S\f$ is ourselves and \f$O\f$ is \p
 * other.
 *
 * @param other the set to union with.
 */
void FastSetOfMonads::unionWith(const FastSetOfMonads& other)
{
	FastSOMConstIterator ci = other.const_iterator();
	while (ci.hasNext()) {
	        addMSE(ci.next());
	}
}

/** Set difference.
 *
 * Removes the monads of \p other from ourselves.
 *
 * Equivalent to \f$S ::= S-O\f$ where \f$S\f$ is ourselves and \f$O\f$ is \p
 * other.
 *
 * @param other The other monad set, containing the monads to remove
 * from ourselves.
 */
void FastSetOfMonads::difference(const FastSetOfMonads& other)
{
	FastSOMConstIterator i = other.const_iterator();
	while (i.hasNext()) {
		removeMSE(i.next());
	}
}



// RemoveMSE
//
// Removes the monads of an MSE B from ourselves.
//
// Explanation:
// There are six cases:
//
//               +-------------+
//               |      B      |
//               +-------------+
//    +---+  +-----+  +---+  +-----+  +---+
// A: | 1 |  |  2  |  | 3 |  |  4  |  | 5 |
//    +---+  +-----+  +---+  +-----+  +---+
//          +-----------------------+
//       A: |           6           |
//          +-----------------------+
//
// We go through the list of MSEs.
// A is the current one.
// We start at the beginning.
//
//  In case 1, we go on to the next A
//  In case 2, we truncate A: A.last = B.first-1 and go on 
//             to the next one
//  In case 3, we delete A and go on to the next one
//  In case 4, we truncate A: A.first = B.last+1 and go on
//             to the next one
//  In case 5, we break the while-loop, since we are done
//  In case 6, we split A and stop the while-loop, since we are done:
//             oldlast := A.last;  A.last := B.first-1; 
//             insert new A(B.last+1,oldlast) after A;
//             break;
//
//  Note that the logical and of the negations of all of the first 5
//             implies the sixth.
//
/** Remove a range from the set.
 *
 * Remove the range of monads represented by the stretch of monads from \p
 * B_first to \p B_last.
 *
 * Equivalent to \f$S ::= S-\{\mathtt{B\_first}..\mathtt{B\_last}\}\f$
 * where \f$S\f$ is ourselves.
 *
 * NOTE: This is not SWIG_wrapped.  Use
 * SetOfMonads::removeMSE(MonadSetElement) instead.
 *
 * \see SetOfMonads::removeMSE(MonadSetElement).
 *
 * @param B_first the first monad of the range to remove.
 * @param B_last the last monad of the range to remove.
 */
void FastSetOfMonads::removeMSE(const MonadSetElement& mse_to_remove)
{
        monad_m B_first = mse_to_remove.first_m;
        monad_m B_last = mse_to_remove.last_m;

	if (isEmpty()) {
		// If we are empty, there is nothing to do
                return;
        } else {
		FastSOMIterator A = monad_ms.upper_bound(B_first);
                if (A != monad_ms.begin()) {
                        --A;
                }

		// Get A first/last
		MonadSetElement curmse = A->second;
                monad_m A_first_m = curmse.first();
                monad_m A_last_m = curmse.last();

		while (A != monad_ms.end()) {
			if (A_last_m < B_first) {     // 1 
                                ++A;
				if (A != monad_ms.end()) {
					curmse = A->second;
					A_first_m = curmse.first_m;
					A_last_m = curmse.last_m;
				}
			} else if (A_first_m > B_last) {
				   break;                      // 5
			} else if (A_first_m >= B_first 
				   && A_last_m <= B_last) {     // 3
				monad_ms.erase(A);
                                A = monad_ms.upper_bound(B_first);
                                if (A != monad_ms.begin()) {
                                        --A;
                                }
				if (A != monad_ms.end()) {
					curmse = A->second;
					A_first_m = curmse.first_m;
					A_last_m = curmse.last_m;
				}
			} else if (A_first_m < B_first
				   && A_last_m > B_last) {
				monad_m oldlast = A_last_m;      // 6
				MonadSetElement new_mse = A->second;
				new_mse.truncate(B_first, B_last);
				monad_ms.erase(A);
				monad_ms.insert(std::make_pair(new_mse.first(), new_mse));

				MonadSetElement new_mse2(B_last+1, oldlast);
                                monad_ms.insert(std::make_pair(new_mse2.first(), new_mse2));
				break;
			} else if (A_first_m < B_first) {
				MonadSetElement new_mse = A->second;	// 2
				new_mse.truncate(B_first, B_last);
				FastSOMIterator tmpA = A;
				++A;
				monad_ms.erase(tmpA);
				monad_ms.insert(std::make_pair(new_mse.first(), new_mse));

				if (A != monad_ms.end()) {
					curmse = A->second;
					A_first_m = curmse.first_m;
					A_last_m = curmse.last_m;
				}
			} else {
				MonadSetElement new_mse = A->second;	// 4
				new_mse.truncate(B_first, B_last);
				FastSOMIterator tmpA = A;
				++A;
				monad_ms.erase(tmpA);
				monad_ms.insert(std::make_pair(new_mse.first(), new_mse));

				if (A != monad_ms.end()) {
					curmse = A->second;
					A_first_m = curmse.first_m;
					A_last_m = curmse.last_m;
				}
			} 
		}
        }

	// Take care of setting m_first and m_last
	// Take care of setting m_first and m_last
	if (isEmpty()) {
		m_first = MAX_MONAD;
		m_last = 0;
	} else {
		m_first = monad_ms.begin()->second.first_m;
		m_last = monad_ms.rbegin()->second.last_m;
		// Check that first and last are set correctly
		ASSERT_THROW(m_first == monad_ms.begin()->second.first_m
			     && m_last == monad_ms.rbegin()->second.last_m,
			     "First and last not set correctly");

	}
}

void FastSetOfMonads::offset(monad_m m)
{
	FastSetOfMonads fsom;
	FastSOMConstIterator ci = const_iterator();
	while (ci.hasNext()) {
		MonadSetElement mse = ci.next();
		mse.offset(m);
		fsom.addMSE(mse);
	}
	*this = fsom;
}

FastSetOfMonads& FastSetOfMonads::operator=(const FastSetOfMonads& other)
{
	assign(other);
	return *this;
}


void FastSetOfMonads::assign(const FastSetOfMonads& other)
{
	m_first = other.m_first;
	m_last = other.m_last;
	monad_ms = other.monad_ms;
}
