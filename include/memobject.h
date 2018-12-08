/*
 * memobject.h
 *
 * MemObject classes (and supporting classes)
 *
 * Ulrik Sandborg-Petersen
 * Created: Sometime in 2005 or 2006.
 * Last update: 12/8-2018
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

#ifndef MEMOBJECT_H__
#define MEMOBJECT_H__

#include "emdf.h"
#include "monads.h"
#include "mql_sheaf.h"
#include <map>
#include <vector>
#include <string>

typedef std::vector<std::string> FeatureVector;

class MemEMdFDB; // Forward declaration

class MemObjectBase {
protected:
	long m_OTN_surrogate;
	long m_special_long;
public:
	MemObjectBase() : m_OTN_surrogate(0), m_special_long(0) { };
	MemObjectBase(const MemObjectBase& other) : m_OTN_surrogate(other.m_OTN_surrogate), m_special_long(other.m_special_long) {}
	virtual ~MemObjectBase() {};

	virtual std::string getFeature(unsigned int nFeatureNo) = 0;

	virtual monad_m first(void) const = 0;
	virtual monad_m last(void) const = 0;

	virtual unsigned int getNoOfFeatures(void) const = 0;

	virtual void setObjectTypeSurrogate(long surrogate) { m_OTN_surrogate = surrogate; };
	virtual long getObjectTypeSurrogate(void) const { return m_OTN_surrogate; };

	virtual void setSpecialLong(long special_long) { m_special_long = special_long; };
	virtual long getSpecialLong(void) const { return m_special_long; };

	virtual id_d_t getID_D(void) const = 0;
};

class MemObject : public MemObjectBase {
protected:
	monad_m m_first, m_last;
	id_d_t m_id_d;
	FeatureVector m_features;
 public:
	MemObject(int nNoOfFeatures);
	MemObject(const MemObject& other);
	virtual ~MemObject();

	virtual void setFeature(unsigned int nFeatureNo, const std::string& featureVal);
	virtual std::string getFeature(unsigned int nFeatureNo);

	virtual void setFirstLast(monad_m first, monad_m last)
	{
		m_first = first;
		m_last = last;
	};

	virtual monad_m first(void) const { return m_first; };
	virtual monad_m last(void) const { return m_last; };

	virtual unsigned int getNoOfFeatures(void) const { return (unsigned int) m_features.size(); };

	virtual void setID_D(id_d_t id_d)
	{
		m_id_d = id_d;		
	};
	virtual id_d_t getID_D(void) const
	{ 
		return m_id_d; 
	};
};

// This variant DOES NOT own the m_pMO.
class MemObjectFromMatchedObject : public MemObjectBase {
protected:
	const MatchedObject *m_pMO;
 public:
	MemObjectFromMatchedObject(const MatchedObject *pMO) : m_pMO(pMO) {};
	MemObjectFromMatchedObject(const MemObjectFromMatchedObject& other) : MemObjectBase(other), m_pMO(other.m_pMO) {};
	virtual ~MemObjectFromMatchedObject() {}; // We do not own the m_pMO

	virtual std::string getFeature(unsigned int nFeatureNo) { return m_pMO->getFeatureAsString(nFeatureNo); }

	virtual monad_m first(void) const { return m_pMO->getFirst(); };
	virtual monad_m last(void) const { return m_pMO->getLast(); };

	virtual unsigned int getNoOfFeatures(void) const { return m_pMO->getNoOfEMdFValues(); };

	virtual id_d_t getID_D(void) const
	{ 
		return m_pMO->getID_D();
	};
};

// This variant DOES own the m_pMO.
class MemObjectWithOwnedMatchedObject : public MemObjectFromMatchedObject {
 public:
	MemObjectWithOwnedMatchedObject(const MatchedObject *pMO) : MemObjectFromMatchedObject(pMO) {};
	MemObjectWithOwnedMatchedObject(const MemObjectWithOwnedMatchedObject& other) : MemObjectFromMatchedObject(new MatchedObject(*other.m_pMO)) {};
	virtual ~MemObjectWithOwnedMatchedObject() { delete m_pMO; }; // We DO own the m_pMO
};

#endif // MEMOBJECT_H__
