/*
 * memobject.h
 *
 * MemObject classes (and supporting classes)
 *
 * Ulrik Sandborg-Petersen
 * Created: Sometime in 2005 or 2006.
 * Last update: 3/21-2013
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
#include <map>
#include <vector>
#include <string>

typedef std::vector<std::string> FeatureVector;

class MemEMdFDB; // Forward declaration

class MemObject {
 private:
	monad_m m_first, m_last;
	long m_OTN_surrogate;
	long m_special_long;
	id_d_t m_id_d;
	FeatureVector m_features;
 public:
	MemObject(int nNoOfFeatures);
	MemObject(const MemObject& other);
	~MemObject();

	void setFeature(unsigned int nFeatureNo, const std::string& featureVal);
	std::string getFeature(unsigned int nFeatureNo);

	void setFirstLast(monad_m first, monad_m last)
	{
		m_first = first;
		m_last = last;
	};

	monad_m first(void) const { return m_first; };
	monad_m last(void) const { return m_last; };

	unsigned int getNoOfFeatures(void) const { return m_features.size(); };

	void setObjectTypeSurrogate(long surrogate) { m_OTN_surrogate = surrogate; };
	long getObjectTypeSurrogate(void) const { return m_OTN_surrogate; };

	void setSpecialLong(long special_long) { m_special_long = special_long; };
	long getSpecialLong(void) const { return m_special_long; };

	void setID_D(id_d_t id_d)
	{
		m_id_d = id_d;		
	};
	id_d_t getID_D(void) const
	{ 
		return m_id_d; 
	};
};

#endif // MEMOBJECT_H__
