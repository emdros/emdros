/*
 * emdf_value.h
 *
 * EMdFValue class
 *
 * Ulrik Petersen
 * Created: 1/18-2003
 * Last update: 11/15-2018
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


/**@file emdf_value.h
 *@brief Header file for EMdFValue (EMdF layer)
 */


#ifndef EMDF_VALUE__H__
#define EMDF_VALUE__H__

#include "emdf.h"
#include <string>
#include "emdf_enums.h"
#include "exception_emdros.h"

class IntegerList;    // Forward declaration
class SetOfMonads;    // Forward declaration
class EnumConstCache; // Forward declaration


/** An EMdF value.
 *
 * Is a union of an id_d_t, a long, and a pointer to std::string.
 *
 * Also has an eEVkind member, showing what kind it is.
 */
class EMdFValue {
private:
#ifndef SWIG
	union {
		id_d_t m_id_d;
		long m_int;
		std::string *m_pString;
		IntegerList *m_pIntegerList;
		SetOfMonads *m_pSOM;
	} m_u;
	eEVkind m_kind;
#endif /* ndef SWIG */
public: 
	EMdFValue(); // Don't use unless you plan to = assign a new value later
	EMdFValue(const EMdFValue& other);
	EMdFValue(eEVkind kind, long i);
	EMdFValue(const std::string& str);
	EMdFValue(const SetOfMonads& som);
	EMdFValue(eEVkind kind, IntegerList* pIntegerList);
	~EMdFValue();
	eEVkind getKind(void) const { return m_kind; };
	const std::string getString(void) const;
	SetOfMonads getSOM(void) const;
	id_d_t getID_D(void) const { return m_u.m_id_d; };
	long getInt(void) const { return m_u.m_int; };
	long getEnum(void) const { return m_u.m_int; };
	IntegerList *getIntegerList (void) const;
	bool compare(const EMdFValue& other, eComparisonOp op) const;
	std::string toString() const;
	std::string toString(id_d_t feature_type_id, EnumConstCache *pEnumCache) const;
	void changeString(const std::string& newString);
#ifndef SWIG
	EMdFValue& operator=(const EMdFValue& other);
#endif /* ndef SWIG */
private:
	bool compareInt(const EMdFValue& other, eComparisonOp op) const;
	bool compareString(const EMdFValue& other, eComparisonOp op) const;
	bool compareListOfIntegerWithOther(const EMdFValue& other, eComparisonOp op) const;
	bool compareSetOfMonads(const EMdFValue& other, eComparisonOp op) const;
	void assign(const EMdFValue& other);
	void deleteContents();
};




#endif // EMDF_VALUE__H__
