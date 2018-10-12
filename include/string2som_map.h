/*
 * string2som_map.h
 *
 * String to SetOfMonads Map
 *
 * Ulrik Sandborg-Petersen
 * Created: 6/22-2016
 * Last update: 6/22-2016
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


/**@file string2som_map.h
 *@brief String to SetOfMonads map (EMdF layer)
 */


#ifndef STRING2SOM_MAP_
#define STRING2SOM_MAP_

#include <string>
#include <map>

#include "monads.h"

class String2SOMMap; // Forward declaration

class String2SOMMapConstIterator {
 protected:
#ifndef SWIG
	const String2SOMMap *m_pMother;
	std::map<std::string, SetOfMonads>::const_iterator m_ci;
#endif // !defined(SWIG)
 public:
	String2SOMMapConstIterator();
	
#ifndef SWIG
	String2SOMMapConstIterator(const String2SOMMapConstIterator& other);
	String2SOMMapConstIterator(const String2SOMMap *pMother);
#endif // !defined(SWIG)

	~String2SOMMapConstIterator();

	bool hasNext() const;
	std::string next();
	std::string current() const;
#ifndef SWIG
	void assign(const String2SOMMapConstIterator& other);
	String2SOMMapConstIterator& operator=(const String2SOMMapConstIterator& other);
#endif
};

class String2SOMMap {
 protected:
	friend class String2SOMMapConstIterator;
#ifndef SWIG
	std::map<std::string, SetOfMonads> m_map;
#endif // !defined(SWIG)
 public:
	String2SOMMap();
	~String2SOMMap();

	void addMonad(const std::string& key, monad_m monad);
	void addMonadRange(const std::string& key, monad_m first_monad, monad_m last_monad);
	void addMonadSet(const std::string& key, const SetOfMonads& som);

	SetOfMonads getMonadSet(const std::string& key) const;
	String2SOMMapConstIterator const_iterator() const;

	bool hasKey(const std::string& key) const;
	
	bool isEmpty() const;
};

#endif // STRING2SOM_MAP_

