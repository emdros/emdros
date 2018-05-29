/*
 * emdf_enums.h
 *
 * EMdF enumerations
 *
 * Ulrik Petersen
 * Created: 8/21-2003
 * Last update: 4/18-2018
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2004-2018  Ulrik Sandborg-Petersen
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

/**@file emdf_enums.h
 *@brief Header file for EMdF enumerations (EMdF layer)
 *
 */



#ifndef EMDF_ENUMS__H__
#define EMDF_ENUMS__H__



// From emdf_output.h

/**The charsets which Emdros knows about.
 *
 * These are passed to EmdrosEnv's constructor.
 *
 * Used for EmdrosEnv::EmdrosEnv().
 *
 */
enum eCharsets {
  kCSASCII,       /**< 7-bit ASCII.  */
  kCSISO_8859_1,  /**< ISO-8859-1. */
  kCSISO_8859_8,  /**< ISO-8869-8. */
  kCSUTF8,        /**< UTF-8. */
  kCSISO_8859_2,  /**< ISO-8859-2.  */
  kCSISO_8859_3,  /**< ISO-8859-3.  */
  kCSISO_8859_4,  /**< ISO-8859-4.  */
  kCSISO_8859_5,  /**< ISO-8859-5.  */
  kCSISO_8859_6,  /**< ISO-8859-6.  */
  kCSISO_8859_7,  /**< ISO-8859-7.  */
  kCSISO_8859_9,  /**< ISO-8859-9.  */
  kCSISO_8859_10,  /**< ISO-8859-10.  */
  kCSISO_8859_13,  /**< ISO-8859-13.  */
  kCSISO_8859_14,  /**< ISO-8859-14.  */
  kCSISO_8859_15,  /**< ISO-8859-15.  */
  kCSISO_8859_16   /**< ISO-8859-16. */
};



// From emdf_output.h

/** Output kind (XML or Console).
 *
 * Used for EmdrosEnv::EmdrosEnv().
 */
typedef enum {
  kOKXML,         /**< XML.*/
  kOKConsole,     /**< Console (more or less human-readable).*/
  kOKCompactXML,  /**< Compact XML.*/
  kOKJSON,        /**< JSON.*/
  kOKCompactJSON  /**< Compact JSON.*/
} eOutputKind;



// From emdf_value.h

/** Comparison operator.
 *\internal
 *
 * Used for mapping MQL comparison ops to SQL ones.
 */
typedef enum  { 
  kEqual,                /**< =.*/
  kLessThan,             /**< <.*/ 
  kGreaterThan,          /**< >.*/
  kNotEqual,             /**< <>.*/
  kLessThanOrEqual,      /**< <=.*/ 
  kGreaterThanOrEqual,   /**< >=.*/
  kTilde,                /**< ~ (regex).*/
  kNotTilde,             /**< != (not regex).*/
  kIn,                   /**< IN (value-list).*/
  kHas                   /**< list HAS member.*/
} eComparisonOp;


// From emdf_value.h

/** EMdFValue:: kinds.
 *
 */
typedef enum {
  kEVInt,              /**< EMdFValue is an integer.*/
  kEVEnum,             /**< EMdFValue is an enum constant.*/
  kEVID_D,             /**< EMdFValue is an id_d.*/
  kEVString,           /**< EMdFValue is a string.*/
  kEVListOfInteger,    /**< EMdFValue is a list of integers.*/
  kEVListOfID_D,       /**< EMdFValue is a list of ID_Ds.*/
  kEVSetOfMonads       /**< EMdFValue is set of monads.*/
} eEVkind;


// From table.h

/** Table column type.
 *
 * Used for the table headers in Table::appendHeader().
 *
 */
typedef enum { 
  kTCString,        /**< Column is a string.*/
  kTCInteger,       /**< Column is an integer.*/
  kTCID_D,          /**< Column is an id_d.*/
  kTCMonad_m,       /**< Column is a monad_m.*/
  kTCBool,          /**< Column is a boolean.*/
  kTCEnum,          /**< Column is an enum.*/
  kTCListOfInteger, /**< Column is a list of integer.*/
  kTCListOfID_D,    /**< Column is a list of id_d.*/
  kTCListOfEnum,    /**< Column is a list of enum.*/
  kTCSetOfMonads    /**< Column is a set of monads.*/
} TableColumnType;


/** Object range types.
 *
 * Used to distinguish between object types that are created WITH
 * MULTIPLE RANGE OBJECTS, WITH SINGLE RANGE OBJECTS, and WITH SINGLE
 * MONAD OBJECTS
 *
 */
enum eObjectRangeType {
  kORTMultipleRange = 0, /**< Object type has been declared WITH MULTIPLE RANGE OBJECTS.*/
  kORTSingleRange = 1,  /**< Object type has been declared WITH SINGLE RANGE OBJECTS.*/
  kORTSingleMonad = 2  /**< Object type has been declared WITH SINGLE MONAD OBJECTS.*/
};


/** Object monad uniqueness
 *
 *
 * Used to distinguish between object types that are created WITH
 * UNIQUE FIRST MONADS, WITH UNIQUE FIRST AND LAST MONADS, and without
 * either...
 *
 */
enum eMonadUniquenessType {
	kMUTUniqueFirstMonads = 0,
	kMUTUniqueFirstAndLastMonads = 1,
	kMUTNonUniqueMonads = 3
};


/** Backend kind
 *
 * Used to distinguish among backends.
 *
 */
enum eBackendKind {
	kBackendNone = 0,/**< No backend selected */
	kPostgreSQL = 1, /**< PostgreSQL */
	kMySQL = 2,      /**< MySQL */
	kSQLite3 = 4,    /**< SQLite 3.X.X */
	kBPT = 5,        /**< Bit Packed Table version 1 */
	kBPT2 = 6        /**< Bit Packed Table version 2 */
};


/** A Monad Set Relation Clause operation
 *
 * part_of == "we" must be a subset of the other monad set.
 *
 * overlap == "we" must have a non-empty intersection with the other
 * monad set.
 *
 */
enum eMonadSetRelationOperation {
	kMSROPartOf,
	kMSROOverlap,
	kMSROStartsIn
};



#endif /* EMDF_ENUMS__H__ */
