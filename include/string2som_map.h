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
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2016  Ulrik Sandborg-Petersen
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

