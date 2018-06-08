/*
 * memobject.h
 *
 * MemObject classes (and supporting classes)
 *
 * Ulrik Sandborg-Petersen
 * Created: Sometime in 2005 or 2006.
 * Last update: 6/8-2018
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2005-2018  Ulrik Sandborg-Petersen
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
	emdros_int64 m_special_long;
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

	void setSpecialLong(emdros_int64 special_long) { m_special_long = special_long; };
	emdros_int64 getSpecialLong(void) const { return m_special_long; };

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
