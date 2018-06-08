/*
 * mql_sheaf.h
 *
 * MQL sheafs
 *
 * Ulrik Sandborg-Petersen
 * Created: 3/8-2001
 * Last update: 6/8-2018
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2018  Ulrik Sandborg-Petersen
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

/**@file mql_sheaf.h
 *@brief Header file for MQL Sheaf and related classes (MQL layer)
 */


#ifndef MQL_SHEAF__H__
#define MQL_SHEAF__H__


#ifndef SWIG
#include "emdf.h"
#endif // !defined SWIG
#include "monads.h"
#include <string>
#include <list>
#include <map>
#include "mql_query.h"
#include "mql_enums.h"
#include "arena.h"
#include "inst.h"
#include "llist.h"
#include "string2som_map.h"


class Sheaf; // forward declaration
class MQLExecEnv; // forward declaration
class EMdFValue; // forward declaration
class FlatSheaf; // forward declaration
class ObjectBlockBase; // forward declaration
class EMdFDB; // forward declaration
class EmdrosEnv; // forward declaration
class StringList; // forward declaration

#ifndef SWIG
/** A std::vector of EMdFValue pointers.
 *
 * Used for MacthedObject.
 */
typedef std::vector<EMdFValue*> value_vec_t;
#endif

////////////////////////////////////////////////////////////////
//
// Full sheaf
//
////////////////////////////////////////////////////////////////


/** Sheaf -- the return result of a topographic query
 *@defgroup SheafGroup Sheaf 
 *@ingroup MQL
 *
 * \section Intro Introduction
 *
 * The sheaf is the return type of all topographic queries.  A
 * topographic query is one that begins with SELECT ALL OBJECTS or
 * SELECT FOCUS OBJECTS.  The sheaf exists in two variants: a full
 * sheaf (class Sheaf) and a flat sheaf (class FlatSheaf).
 *
 * A sheaf is a list of straws.  A straw is a list of matched objects.
 * A matched object corresponds to an object block or a gap block in
 * the topographic query.  The matched object has, inside of it, a
 * sheaf, which may be the empty sheaf.
 *
 * \section FullSheafIntro Full sheaf
 * 
 * The general idea of the full sheaf is that its structure should
 * correspond to the structure of the topographic query from which it
 * arose.  Thus a matched object corresponds to an object block or a
 * gap block.  The string of blocks which the object block was a part
 * gives rise to a straw.  The straw, being a list of matched objects,
 * thus constitutes one whole matching of a string of blocks at the
 * same level.  The sheaf, being a list of straws, thus corresponds to
 * all possible matchings of a string of blocks.  An object block or a
 * gap block may optionally have a string of blocks nested inside
 * them.  This string in turn gives rise to a sheaf.  That is why the
 * matched object has a sheaf inside it.
 *
 *
 * \subsection MatchedObjectIntro Matched object
 *
 * Matched objects exist in two versions: One arising from an object
 * block (or an object_block_first), and one arising from a gap block
 * (or an opt_gap_block).  
 *
 * The variety arising from a gap block is called an <b>ID_M</b>
 * matched object, and has:
 *
 * - a set of monads corresponding to the monads of the gap that it matched
 *
 * - a focus boolean (saying whether the gap block in question had the
 *   FOCUS keyword)
 *
 * - an inner sheaf (for any nested strings of blocks)
 *
 * The variety arising from an object block is called an <b>ID_D</b>
 * object block, and has:
 *
 * - a set of monads corresponding to the set of monads for the object
 *   that it came from.
 *
 * - a FOCUS boolean (like the ID_M kind)
 *
 * - an inner sheaf (like the ID_M kind)
 *
 * - the id_d of the object from which it arose.
 *
 * - the object type of the object from which it arose.
 *
 * - a vector of EMdFValue objects that is non-empty if the object
 *   block had a GET clause.
 *
 * @see Sheaf, Straw, MatchedObject, SheafConstIterator,
 * StrawConstIterator.
 *
 *
 * \section FlatSheafIntro Flat sheaf
 *
 * A flat sheaf, on the other hand, results from a full sheaf by means
 * of a "flatten" operator (the full sheaf is always generated, even
 * if the query was declared RETURNING FLAT SHEAF).  The flat sheaf
 * has one flat straw for each object type requested.  If the query
 * was just declared RETURNING FLAT SHEAF, then all object types
 * present in object blocks in the query will have one flat straw in
 * the flat sheaf.  If the query was declared RETURNING FLAG SHEAF ON,
 * and then a list of object type names, then only those object types
 * will appear in the flat sheaf.
 *
 * Each flat straw contains all the matched objects from the full
 * sheaf that have the object type of the flat straw.  Thus a flat
 * straw consists of a list of all the matched objects with a given
 * object type.
 *
 * For example, the following query:
 *
 * \code
 * SELECT ALL OBJECTS
 * RETURNING FLAT SHEAF
 * [Clause
 *    [Phrase 
 *       [Word psp = verb]
 *    ]
 * ]
 * GO
 * \endcode
 *
 * will result in a flat sheaf with three flat straws: One for Clause,
 * one for Phrase, and one for Word.
 *
 * The following query:
 *
 * \code
 * SELECT ALL OBJECTS
 * RETURNING FLAT SHEAF ON Phrase, Word
 * [Clause
 *    [Phrase 
 *       [Word psp = verb]
 *    ]
 * ]
 * GO
 * \endcode
 * 
 * Will only have two flat straws, namely one for Phrase and one for Word.
 *
 * @see FlatSheaf, FlatStraw, FlatSheafConstIterator,
 * FlatStrawConstIterator.
 *
 */

/** A MatchedObject inside a Straw or a FlatStraw
 *@ingroup SheafGroup
 *
 * See the documentation for \ref SheafGroup "Sheaves" for more
 * information on what is inside a matched object and how it fits in
 * with the straw and the flat straw.
 *
 */
class MatchedObject {
 protected:
	// This is the inner sheaf.
	Sheaf* m_sheaf; /** The inner sheaf */

#ifndef SWIG
	/** A pointer to the object block base from which this 
	 * matched object arose.
	 *@internal
	 *
	 * Is 0 (nil) for EMPTY_mo, but is never 0 for ID_D or ID_M.
	 */
	ObjectBlockBase *m_pObjectBlock; 
#endif

	// Note: It is actually a distinct speed DISadvantage to gather up
	// m_kind, m_focus, and m_object_type_index in a bit field.
	// Been there, done that.  Note, however, that I did not try
	// with optimization turned on.

	/**
	 * \defgroup MatchedObjectSetOfMonads MatchedObject sets of monads.
	 *\ingroup SheafGroup
	 *
	 * The invariant is:
	 * If m_last >= 0, then
	 *   - m_last is the last monad
	 *   - m_u.first is the first monad.
	 *     (this is for those sets of monads that are only 1 MSE long.)
	 * If m_last < 0, then
	 *   - -1*m_last is the last monad, and
	 *   - m_u.pMonad_ms is the set of monads
	 *     (this is for those monad sets that are longer than 1 MSE.)
	 */
	/*@{*/

	/** The last monad of the monad set. 
	 *
	 * See the discussion under "MatchedObjectSetOfMonads" for more
	 *  information.
	 */
	monad_m m_last;
	/** Either the full monad set or the first monad of the set.
	 *
	 * See the discussion under "MatchedObjectSetOfMonads" for more
	 * information.
	 */
	union {
		/** The first monad of the monad set. 
		 *
		 * See the discussion under "MatchedObjectSetOfMonads" for more
		 * information.
		 */
		monad_m first;
		/** The full monad set.
		 *
		 * See the discussion under "MatchedObjectSetOfMonads" for more
		 * information.
		 */
		SetOfMonads *pMonad_ms;
	} m_u;
	/*@}*/
	/** The object id_d, and the focus boolean, specially coded.
	 *
	 * Invariant:
	 * If this object is a kMOKEMPTY_mo, then m_id_d is -1
	 * If this object is a kMOKID_M, then m_id_d is -2, -3, or -4
	 *     -2 means noretrieve
	 *     -3 means retrieve
	 *     -4 means focus
	 * If this object is a kMOKID_D, then:
	 *     EITHER m_id_d is >= 0, and id_d is m_id_d.  
	 *            Then m_value is of kind m_pValue_vec;
	 *     OR m_id_d is <= -5, and id_d is m_id_d + 5
	 *            Then m_value is of kind m_pInstObj
	 */
	id_d_t m_id_d;
	/** The value_vec_t vector of pointers to EMdFValue objects.
	 *
	 * Used for the GET clause.
	 *
	 * @see #MatchedObject::getEMdFValue(int),
	 * #MatchedObject::getEMdFValue(const std::string&),
	 * #MatchedObject::getEMdFValueIndex(const std::string&),
	 * #MatchedObject::getFeatureList(void),
	 * #MatchedObject::getNoOfEMdFValues(void),
	 * #MatchedObject::getEnumLabel(int).
	 *
	 * @see value_vec_t.
	 */
	union {
		value_vec_t *m_pValue_vec;
		const InstObject *m_pInstObject;
	} m_values;
 public:  
#ifndef SWIG
	MatchedObject(const SetOfMonads& monads, 
		      ObjectBlockBase *pOBB,
		      bool bRetrieve, bool bFocus, 
		      Sheaf *sheaf); 
	MatchedObject(id_d_t id_d, const SetOfMonads& monads, Sheaf* sheaf, ObjectBlockBase *pObjectBlock);
	MatchedObject(id_d_t id_d, monad_m first_monad, monad_m last_monad, Sheaf* sheaf, ObjectBlockBase *pObjectBlock);
	MatchedObject(const InstObject *pInstObj, ObjectBlockBase *pObjectBlock);
#endif
	MatchedObject(monad_m SmMinus1); // for kMOKEMPTY_mo
	MatchedObject(const MatchedObject& other);
	~MatchedObject();
	SetOfMonads getMonads(void) const;
	/** Get the ID_D.  Returns NIL if this is an ID_M MactchedObject.
	 *
	 * @see #getKind().
	 *
	 * @return The ID_D of the object, if this is an ID_D MatchedObject.
	 * Otherwise, return NIL.
	 *
	 */
	id_d_t getID_D(void) const { if (m_id_d >= 0) return m_id_d; else { if (m_id_d <= -5) return -1*(m_id_d + 5); else return NIL;}};
	const Sheaf* getSheaf(void) const;

	/** See whether inner sheaf is empty.
	 *
	 * See the documentation for \ref SheafGroup "sheaves" for more
	 * information on the inner sheaf.
	 *
	 * @return \p true if inner sheaf is empty,
	 * \p false otherwise.
	 */
	bool sheafIsEmpty(void) const;

	// Kind
	/** The kind of the matched object (ID_D, ID_M, or EMPTY_mo).
	 *
	 * @return an eMOKind enum value, either kMOKID_D, kMOKID_M, or
	 * kMOKEMPTY_mo. NOTE that kMOKEMPTY_mo will never occur in a sheaf!
	 */
	eMOKind getKind(void) const { 
		if (m_id_d >= 0) {
			return kMOKID_D;
		} else if (m_id_d == -1) {
			return kMOKEMPTY_mo;
		} else if (m_id_d <= -5) {
			return kMOKID_D;
		} else if (m_id_d <= -2) {
			return kMOKID_M;
		} else {
			// we will never get here, due to the nature of the integers,
			// and the above IF statements, but the C++ compiler on Windows
			// isn't smart enough to figure that out, so we fool it into
			// giving no warning.
			return kMOKID_D;
		}
	};
	/** Return true if this is an ID_M.
	 *
	 * @return \p true if this is an ID_M matched object,
	 * \p false if it is not.
	 */
	bool isID_M(void) const { return m_id_d <= -2 && m_id_d >= -4; };
	/** Return true if the m_values union is of kind const InstObject*.
	 *
	 */
	bool isInstObject(void) const { return m_id_d <= -5; };
	/** Return true if this is an ID_D.
	 *
	 * @return \p true if this is an ID_D matched object,
	 * \p false if it is not.
	 */
	bool isID_D(void) const { return m_id_d >= 0 || m_id_d <= -5;  };
	/** Return true if this is an EMPTY_mo
	 *
	 * @return \p true if this is an EMPTY_mo matched object,
	 * \p false if it is not.
	 */
	bool isEMPTY_mo(void) const { return m_id_d == -1;  };

	// Focus
	/** Return true if the block from which this came had the FOCUS keyword.
	 *
	 * @return \p true if this matched object is in FOCUS,
	 * \p false otherwise.
	 */
	bool getFocus(void) const { if (isID_D()) return m_pObjectBlock->isFocus(); else return m_id_d == -4; };

	/** Return the marks string (given in the query with a backping
	 * ("`"), right after the object type in an object_block, or
	 * right after "[gap?" or "[gap".
	 *
	 * Can be called for all types of MatchedObjects, but will return "" (the
	 * empty string) if either there is no mark, or it is an EMPTY_MO.
	 *
	 * @return the mark string, including the backping(s).
	 */
	std::string getMarksString() const { 
		if (m_id_d >= 0 || m_id_d <= -2) { 
			return m_pObjectBlock->getMarkString(); 
		}  else {
			return ""; 
		}
	};

#ifndef SWIG
	const ObjectBlockBase *getObjectBlockBase(void) const { return m_pObjectBlock; };
#endif
	
	/** Return true if the block is to be retrieved.
	 *
	 * @return \p true if this matched object is to be retrieved,
	 * \p false otherwise.
	 */
	bool getRetrieve(void) const { if (isID_D()) return m_pObjectBlock->isToBeRetrieved(); else return m_id_d <= -3; };

	/** Count objects recursively.
	 *
	 * @param bUseOnlyFocusObjects If true, only objects which
	 * have the focus boolean set to true will be counted.
	 *
	 * @return The number of (possibly focus) objects in this
	 * MatchedObject, and any embedded Sheaf.
	 */
	emdros_int64 countObjects(bool bUseOnlyFocusObjects) const;

	/** Count objects recursively.
	 *
	 * If object_type_name is empty, all object types are counted.
	 *
	 * If object_type_name is "pow_m", only gap_blocks and
	 * opt_gap_blocks are counted.
	 *
	 * If object_type_name is neither empty nor "pow_m", only objects of the
	 * given object type are counted.
	 *
         *
	 * @param object_type_name A string giving the normalized
	 * object type name of the objects to count.  This should have
	 * been passed through normalizeOTName() before calling this
	 * method.
	 *
	 * @param bUseOnlyFocusObjects If true, only objects of the
	 * given object type which have the focus boolean set to true
	 * will be counted.
	 *
	 * @return The number of (possibly focus) objects in this
	 * MatchedObject, and any embedded Sheaf.
	 */
	emdros_int64 countObjectsFromObjectType(const std::string& object_type_name, bool bUseOnlyFocusObjects) const;
	
	/** Count straws recursively.
	 *
	 * @return The number of straws in this MatchedObject's
	 * embedded Sheaf, or 0 if that sheaf is empty.
	 */
	emdros_int64 countStraws() const;

	// See Sheaf::getSOM() for an explanation
	void getSOM(SetOfMonads& som, bool bUseOnlyFocusObjects) const;

	// See Sheaf::harvestMarks() for an explanation
	void harvestMarks(String2SOMMap& result, bool bUseSingleMarks) const;
#ifndef SWIG
	/** Return the last monad of the matched object.
	 *
	 * @return the last monad of the monad set of the matched object.
	 */
	bool somHasOnlyOneMSE(void) const { return (m_last >= 0) ? true : (m_u.pMonad_ms->hasOnlyOneMSE()); };
	monad_m getFirst(void) const { return (m_last >= 0) ? m_u.first : (m_u.pMonad_ms->first()); };
	monad_m getLast(void) const { return (m_last >= 0) ? m_last : (m_last*-1); };
#endif // !defined SWIG

	// See the comment before the function in ../MQL/mql_sheaf.cpp.
	std::string getFeatureAsString(int index) const;

	// See the comment before the function in ../MQL/mql_sheaf.cpp.
	emdros_int64 getFeatureAsLong(int index) const;

	// Returns 0 on index out of range.
	// Is the fastest
	const EMdFValue *getEMdFValue(int index) const;

	// Returns 0 on index out of range.
	// Is slow.
	const EMdFValue *getEMdFValue(const std::string& feature_name) const;

	// Get the index of a feature name to be used for 
	// getEMdFValue(unsigned int index).
	// Returns -1 on not found
	int getEMdFValueIndex(const std::string& feature_name) const;

	// Get list of feature-names in the order in which they appear in
	// the value vector.  Thus this list can be used as a basis for
	// getting the indexes of the EMdFValues to be used with
	// MatchedObject::getEMdFValue().
	StringList getFeatureList(void) const;

	// Get number of values in list of EMdFValue's.
	unsigned int getNoOfEMdFValues(void) const;

	// Get enum label from an index into the EMdFValue vector
	std::string getEnumLabel(int index) const;
#ifndef SWIG
	void addEMdFValue(unsigned int feature_index, EMdFValue *pValue);
	bool flatten(FlatSheaf *pFlatSheaf);
#endif
	std::string getObjectTypeName() const;
	id_d_t getObjectTypeId() const;
	void printConsole(EMdFOutput *pOut, bool bIsForFullSheaf) const;
	void printXML(EMdFOutput* pOut, bool bIsForFullSheaf) const;
	void printJSON(EMdFOutput* pOut, bool bIsForFullSheaf) const;
};

/** Copy a matched object.
 *@internal
 *
 * This is done by means of the copy constructor of MatchedObject.
 *
 * The value is created with new.
 */
#define copyMO(OTHER) (new MatchedObject(*(OTHER)))

/** An LList<MatchedObject>.
 *@internal
 */
typedef Llist<MatchedObject> MOList;

/** An LListIterator<MatchedObject>.
 */
typedef LlistIterator<MatchedObject> StrawIterator;

/** An LListConstIterator<MatchedObject>.
 */
typedef LlistConstIterator<MatchedObject> StrawConstIterator;

#ifdef SWIG
%template (StrawIterator) LlistIterator<MatchedObject>;
%template (StrawConstIterator) LlistConstIterator<MatchedObject>;
#endif

/** A Straw, i.e., a list of MatchedObject's.
 *@ingroup SheafGroup
 *
 * See the documentation for \ref SheafGroup "Sheaves" for more
 * information on the relationship between a Straw, its
 * MatchedObjects, and its parent Sheaf.
 *
 */
class Straw {
 private:
	monad_m m_last;
	MOList m_list;
 public:
#ifndef SWIG
	Straw();
#endif // !defined SWIG
	~Straw();
	Straw(const Straw& other);
	StrawConstIterator const_iterator() const;
	monad_m getLast(void) const { return m_last; };
#ifndef SWIG
	bool isEmpty(void) const { return m_list.empty(); };
	Straw& operator=(const Straw& other);
	void append(MatchedObject* mo);
	void prepend(const Straw& other);
	void prependAndTakeOver(Straw *pOther);
	void updateLast(const MatchedObject* mo);
	void flatten(FlatSheaf *pFlatSheaf);
#endif // !defined SWIG
	void printConsole(EMdFOutput *pOut) const;
	void printXML(EMdFOutput* pOut) const;
	void printJSON(EMdFOutput* pOut) const;

	/** Count objects recursively.
	 *
	 * @param bUseOnlyFocusObjects If true, only objects which
	 * have the focus boolean set to true will be counted.
	 *
	 * @return The number of (possibly focus) objects in the
	 * MatchedObject's of this straw, and any embedded Sheaves
	 * therein.
	 */
	emdros_int64 countObjects(bool bUseOnlyFocusObjects) const;

	/** Count objects recursively.
	 *
	 * If object_type_name is empty, all object types are counted.
	 *
	 * If object_type_name is "pow_m", only gap_blocks and
	 * opt_gap_blocks are counted.
	 *
	 * If object_type_name is neither empty nor "pow_m", only objects of the
	 * given object type are counted.
	 *
         *
	 * @param object_type_name A string giving the normalized
	 * object type name of the objects to count.  This should have
	 * been passed through normalizeOTName() before calling this
	 * method.
	 *
	 * @param bUseOnlyFocusObjects If true, only objects of the
	 * given object type which have the focus boolean set to true
	 * will be counted.
	 *
	 * @return The number of (possibly focus) objects in the
	 * MatchedObject's in this straw, and any embedded Sheaves
	 * therein.
	 */
	emdros_int64 countObjectsFromObjectType(const std::string& object_type_name, bool bUseOnlyFocusObjects) const;
	
	/** Count straws recursively.
	 *
	 * @return The number of straws inside the sheaf, recursively.
	 */
	emdros_int64 countStraws() const;

	
	// See Sheaf::getSOM() for an explanation
	void getSOM(SetOfMonads& som, bool bUseOnlyFocusObjects) const;
	// See Sheaf::harvestMarks() for an explanation
	void harvestMarks(String2SOMMap& result, bool bUseSingleMarks) const;
 private:
	void copyOther(const Straw& other);
	void deleteMos(void);
};

#ifndef SWIG
/** An Llist of Straw.
 *@ingroup SheafGroup
 *@internal
 */
typedef Llist<Straw> StrawList;
#endif

/** A const_iterator over an Llist of Straw.
 *@ingroup SheafGroup
 *@internal
 */
typedef LlistConstIterator<Straw> SheafConstIterator;
/** An iterator over an Llist of Straw.
 *@ingroup SheafGroup
 *@internal
 */
typedef LlistIterator<Straw> SheafIterator;

#ifdef SWIG
%template (SheafConstIterator) LlistConstIterator<Straw>;
%template (SheafIterator) LlistIterator<Straw>;
#endif


/** A list of straws (making up the innards of a sheaf)
 *@ingroup SheafGroup
 *@internal
 */
class ListOfStraws {
 private:
	StrawList m_list;
 public:
#ifndef SWIG
	ListOfStraws();
#endif // !defined SWIG
	~ListOfStraws();
	ListOfStraws(const ListOfStraws& other);
	SheafIterator iterator();
	SheafConstIterator const_iterator() const;
#ifndef SWIG
	ListOfStraws& operator=(const ListOfStraws& other);
#endif // !defined SWIG
	bool isEmpty() const { return m_list.empty(); };
	emdros_int64 size() const { return m_list.size(); }; // WARNING: This is currently slow!
	static void printLOSStartConsole(EMdFOutput *pOut);
	static void printLOSEndConsole(EMdFOutput *pOut);
	void printConsole(EMdFOutput *pOut, bool bPrintStartAndEnd = true) const;
	void printXML(EMdFOutput* pOut) const;
	void printJSON(EMdFOutput* pOut) const;
#ifndef SWIG
	void append(Straw* straw);
	void appendAndSubsume(ListOfStraws *pOther);
	emdros_int64 appendAndSubsumeWhileFiltering(ListOfStraws *pOther);
	emdros_int64 appendAndSubsumeWhileCounting(ListOfStraws* pOther, emdros_int64 start_index, emdros_int64 count);
	void appendACopy(const ListOfStraws *pOther);
	void join(const Straw* mys);
	void joinAndTakeOver(Straw* s);
	void deleteStraws(void);
	void clearButDontDeleteStraws(void) { m_list.clear(false); };
 private:
	void copyOther(const ListOfStraws& other);
#endif // !defined SWIG
};



/** A Sheaf, i.e., a list of Straw's.
 *@ingroup SheafGroup
 *
 * See the documentation for \ref SheafGroup "Sheaves" for more
 * information on the relationship between a Sheaf, its Straw
 * children, and the Straws's MatchedObject children.
 *
 */
class Sheaf {
 protected:
	ListOfStraws* m_plist;
#ifndef SWIG
	OBBVec *m_pOBBVec;
#endif
 public:
#ifndef SWIG
	// isFail True:  Failed sheaf (no m_plist). 
	// isFail False: Empty sheaf  (empty m_plist)
	Sheaf(bool isFail); 
	Sheaf(ListOfStraws* plist);
#endif // !defined SWIG
	Sheaf(const Sheaf& other);
	~Sheaf();
	bool isFail(void) const;
	SheafIterator iterator();
	SheafConstIterator const_iterator() const;
#ifndef SWIG
	const ListOfStraws* get_plist(void) const { return m_plist; };
	ListOfStraws *takeOverPList(void) { ListOfStraws *pList = m_plist; m_plist = 0; return pList; }
	void appendAndSubsume(Sheaf *pOther);
	emdros_int64 appendAndSubsumeWithCounting(Sheaf *pOther, emdros_int64 start_index, emdros_int64 count);
	Sheaf& operator=(const Sheaf& other);
	void flatten(FlatSheaf *pFlatSheaf);
#endif
	void printConsole(EMdFOutput *pOut) const;
	void printXML(EMdFOutput* pOut) const;
	static void printSheafStartConsole(EMdFOutput *pOut);
	static void printSheafStartXML(EMdFOutput* pOut);
	static void printSheafEndXML(EMdFOutput* pOut);

	static void printDTD(EMdFOutput* pOut);

    void printJSON(EMdFOutput* pOut) const;
	static void printSheafStartJSON(EMdFOutput* pOut, bool hasLOS);
	static void printSheafEndJSON(EMdFOutput* pOut, bool hasLOS);

	// Gets big-union of sets of monads in sheaf's matched_objects. 
	// This is done recursively through all straws and inner sheafs.
	//
	// If  bUseOnlyFocusObjects is true, only matched_objects with their
	// focus boolean set to true will be included. Otherwise, all matched_objects
	// are considered. 


	/** Count objects recursively.
	 *
	 * @param bUseOnlyFocusObjects If true, only objects which
	 * have the focus boolean set to true will be counted.
	 *
	 * @return The number of (possibly focus) objects in the
	 * MatchedObject's of the straws of this Sheaf, and any
	 * embedded Sheaves therein.
	 */
	emdros_int64 countObjects(bool bUseOnlyFocusObjects) const;

	/** Count objects recursively.
	 *
	 * If object_type_name is empty, all object types are counted.
	 *
	 * If object_type_name is "pow_m", only gap_blocks and
	 * opt_gap_blocks are counted.
	 *
	 * If object_type_name is neither empty nor "pow_m", only objects of the
	 * given object type are counted.
	 *
         *
	 * @param object_type_name A string giving the normalized
	 * object type name of the objects to count.  This should have
	 * been passed through normalizeOTName() before calling this
	 * method.
	 *
	 * @param bUseOnlyFocusObjects If true, only objects of the
	 * given object type which have the focus boolean set to true
	 * will be counted.
	 *
	 * @return The number of (possibly focus) objects in the
	 * MatchedObject's in the straws of this Sheaf, and any
	 * embedded Sheaves therein.
	 */
	emdros_int64 countObjectsFromObjectType(const std::string& object_type_name, bool bUseOnlyFocusObjects) const;
	
	/** Count straws recursively.
	 *
	 * @return The number of straws inside this Sheaf, and any
	 * embedded sheaves in its inner MatchedObject's.
	 */
	emdros_int64 countStraws() const;
	
	
	// The method makes no distinction between matched_objects arising from 
	// (opt-)gap-blocks and object blocks.
	//
	// You should probably start with an empty set of monads unless you
	// want to include monads that are not in the sheaf.
	void getSOM(SetOfMonads& som, bool bUseOnlyFocusObjects) const;

	// The method retrives, in the String2SOMMap, the big-union of
	// the sets of monads from the matched_objects which have
	// "marks" on their blocks.  If bUseSingleMarks is true, every
	// mark becomes its own key in the result, with possible
	// duplication of monad set content.  If bUseSingleMarks is
	// false, then the concatenation of marks is used.
	// When there are no marks on a matched_object's block, then
	// that matched_object is not used in the result.
	void harvestMarks(String2SOMMap& result, bool bUseSingleMarks) const;

	// Here is a version which starts with an empty set and returns the result
	// rather than passing it as a parameter.
	SetOfMonads getSOM(bool bUseOnlyFocusObjects) const;
#ifndef SWIG
	void setOBBVec(OBBVec *pOBBVec) { m_pOBBVec = pOBBVec; };
	OBBVec *getOBBVec(void) { return m_pOBBVec; };
#endif
 protected:
	void copyOther(const Sheaf& other);
	void deleteStraws(void);
};


////////////////////////////////////////////////////////////////
//
// Flat sheaf
//
////////////////////////////////////////////////////////////////

class FlatStrawConstIterator; // forward declaration

/** A list of MatchedObjects, all belonging to a single object type,
 *  for inclusion in a FlatSheaf.
 *@ingroup SheafGroup
 */
class FlatStraw {
 private:
	friend class FlatStrawConstIterator;
#ifndef SWIG
	Arena *m_pArena;
	Inst *m_pInst;
	ObjectBlockBase *m_pOBB;
#endif
	std::string m_object_type_name;
 public:
        FlatStraw(const std::string& object_type_name);// : m_object_type_name(object_type_name) { m_pArena = new Arena(); };
#ifndef SWIG
        FlatStraw(const std::string& object_type_name, Inst *pInst, ObjectBlockBase *pOBB);// : m_object_type_name(object_type_name) { m_pArena = new Arena(); };
#endif
	~FlatStraw();
	FlatStrawConstIterator const_iterator() const;
	void addMO(const MatchedObject *pMO) { if (m_pArena != 0) { const MatchedObject** pMem = (const MatchedObject**) m_pArena->allocate(sizeof(MatchedObject*)); *pMem = pMO; } else {ASSERT_THROW(false, "Error: FlatStraw::addMO() was called, even though this was not an Arena-based FlatStraw.")};};
	void printConsole(EMdFOutput *pOut) const;
	void printXML(EMdFOutput* pOut) const;
	void printJSON(EMdFOutput* pOut) const;
	std::string getObjectTypeName(void) const { return m_object_type_name; };
};

/** A const iterator on a FlatStraw.
 *@ingroup SheafGroup
 */
class FlatStrawConstIterator {
#ifndef SWIG
	const FlatStraw *m_pMotherFlatStraw;
	ArenaConstIterator *m_pArenaIt;
	Inst::const_iterator m_InstIt;
	MatchedObject *m_pPrevMO;
	MatchedObject *m_pCurMO;
#endif
 public:
	FlatStrawConstIterator();
	FlatStrawConstIterator(const FlatStraw *pMotherFlatStraw); // : ArenaConstIterator(sizeof(MatchedObject*), pMotherFlatStraw->m_pArena) {};
	/** Copy constructor. 
	 * 
	 * @param other The other FlatStrawConstIterator from which to
	 * initialize.
	 */
	FlatStrawConstIterator(const FlatStrawConstIterator& other);
	/** Test whether we have hit the end.
	 * 
	 * @return true iff we have not hit the end, i.e., there is at least one more left in the Arena.
	 */
	bool hasNext() const; // { return ArenaConstIterator::hasNext(); }; // Is the iterator == end iterator?  Doesn't alter iterator
	MatchedObject *next(); // Gets current and advances iterator afterwards
	MatchedObject *current(); // Gets current without altering iterator
	/** Destructor. 
	 */
	~FlatStrawConstIterator();
	FlatStrawConstIterator& operator=(const FlatStrawConstIterator& other);
 private:
	void assign(const FlatStrawConstIterator& other);
	void deleteAndShiftToNext();
#ifndef SWIG
	void setCurMO(const InstObject* pInstObj);
#endif
	
};




/** A const iterator on a FlatSheaf.
 *@ingroup SheafGroup
 */
class FlatSheafConstIterator {
 private:
#ifndef SWIG
	std::map<id_d_t, FlatStraw*>::const_iterator m_current;
	const FlatSheaf *m_pMotherFlatSheaf;
#endif
 public:
	FlatSheafConstIterator();
	FlatSheafConstIterator(const FlatSheaf *pMotherFlatSheaf);
	FlatSheafConstIterator(const FlatSheafConstIterator& other);
	~FlatSheafConstIterator();
	bool hasNext() const; // Is the iterator == end iterator?  Doesn't alter iterator
	FlatStraw* next(); // Gets current and advances iterator afterwards
	FlatStraw* current(); // Gets current without altering iterator
#ifndef SWIG
	FlatSheafConstIterator& operator=(const FlatSheafConstIterator& other);
#endif
 private:
	void assign(const FlatSheafConstIterator& other);
};

/** A sheaf holding FlatStraw objects, each associated with a single
 *  object type.
 *@ingroup SheafGroup
 *
 * The constructors specify whether we wish to get MatchedObjects of
 * "all" object types (via the addMO() method), or whether we wish to
 * get only a select few.
 */
class FlatSheaf {
 private:
	friend class FlatSheafConstIterator;
	std::map<id_d_t, FlatStraw*> m_map;
	bool m_bDoAll;
	bool m_bIsFail;
#ifndef SWIG
	OBBVec *m_pOBBVec;
#endif
 public:
	FlatSheaf(); // For "all"
#ifndef SWIG
	FlatSheaf(const std::list<std::pair<id_d_t, std::string> >& object_types); // For only a select few
#endif
	~FlatSheaf();
	bool addMO(const MatchedObject *pMO);
	/** Set m_bIsFail.
	 *@internal
	 *
	 * @param bIsFail The value to set m_bIsFail to.
	 */
	void setIsFail(bool bIsFail) { m_bIsFail = bIsFail; }
	/** Check whether we are a failed FlatSheaf.
	 *
	 * @return true iff we are a failed sheaf.
	 */
	bool isFail(void) const { return m_bIsFail; };
	void printConsole(EMdFOutput *pOut) const;
	void printXML(EMdFOutput* pOut) const;
	void printJSON(EMdFOutput* pOut) const;
	static void printDTD(EMdFOutput *pOut);
	void addFlatStraw(id_d_t object_type_id, FlatStraw *pFlatStraw);
#ifndef SWIG
	/** Set the m_pOBBVec.
	 *@internal
	 *
	 * @param pOBBVec the pointer to set m_pOBBVec to.
	 */
	void setOBBVec(OBBVec *pOBBVec) { m_pOBBVec = pOBBVec; };
#endif
	FlatSheafConstIterator const_iterator(void) const { return FlatSheafConstIterator(this); };
 private:
	FlatStraw* addEmptyFlatStraw(id_d_t object_type_id, const std::string& object_type_name);
};

#ifndef SWIG
extern FlatSheaf *mql_flatten_sheaf(const std::list<std::pair<id_d_t, std::string> >& object_types, Sheaf **ppSheaf);

extern FlatSheaf *mql_flatten_sheaf(Sheaf **ppSheaf); // For all object types in the sheaf

extern FlatSheaf *mql_flatten_sheaf(StringList *pObjectTypeNames, EMdFDB *pDB, Sheaf **ppSheaf);

extern FlatSheaf *mql_flatten_sheaf(StringList *pObjectTypeNames, EmdrosEnv *pEnv, Sheaf **ppSheaf);

#endif



#endif /* MQL_SHEAF__H__ */
