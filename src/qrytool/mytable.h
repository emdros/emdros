/*
 * mytable.h
 *
 * A multi-map-representation of an Emdros Table.
 *
 * Ulrik Petersen
 * Created: 4/9-2005
 * Last update: 11/10-2017
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2005-2017  Ulrik Sandborg-Petersen
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

#ifndef MYTABLE__H__
#define MYTABLE__H__

#include <set>
#include <map>
#include <vector>
#include <string>

#include <emdros.h>

class MyTable; // Forward declaration


enum eInputTableKind {
	kGET_FEATURES, kGET_MONADS
};

// Maps object type to MyTable*
typedef std::map<std::string, MyTable*> OTTableMap;
extern void clear_OTTableMap(OTTableMap& mymap);

/*
  # Each MyTable is structured as follows:
  # col 0   : id_d
  # col 1   : first monad
  # col 2   : last monad
  # col 3-n : feature values. The application needs to keep track of
  #           which feature is listed in which column
*/

typedef std::vector<std::string> StringVec;

class MyRow {
 private:
	id_d_t m_id_d;
	monad_m m_first_monad;
	monad_m m_last_monad;
	StringVec m_features;
 public:
	MyRow() { m_id_d = 0; m_first_monad = 0; m_last_monad = 0; };
	MyRow(const MyRow& other) { copyOther(other); };
        MyRow(id_d_t id_d, monad_m first_monad, monad_m last_monad) 
	 : m_id_d(id_d), m_first_monad(first_monad), m_last_monad(last_monad) {};

	~MyRow() {};

	void appendFeature(const std::string& feature) { 
		m_features.push_back(feature); 
	};
	
	id_d_t getID_D(void) const { return m_id_d; };
	
	monad_m getFirstMonad(void) const { return m_first_monad; };
	
	monad_m getLastMonad(void) const { return m_last_monad; };
	
	std::string toString(void) const {
		std::string result = id_d2number_string(m_id_d) + " | "
			+ monad_m2string(m_first_monad) + " | " 
			+ monad_m2string(m_last_monad);
		for (StringVec::const_iterator ci = m_features.begin();
			     ci != m_features.end();
		     ++ci) {
				result += " | " + *ci;
		}
		return result;
	};
	void appendOther(const MyRow& other, int start_column) {
		if (start_column <= 0) {
			m_features.push_back(id_d2number_string(other.getID_D()));
		}
		if (start_column <= 1) {
			m_features.push_back(monad_m2string(other.getFirstMonad()));
		}
		if (start_column <= 2) {
			m_features.push_back(monad_m2string(other.getLastMonad()));
		}
		for (StringVec::const_iterator ci = other.m_features.begin();
		     ci != other.m_features.end();
		     ++ci) {
			m_features.push_back(*ci);
		}
	};
	const StringVec& getFeatures(void) const { return m_features; };

	MyRow& operator=(const MyRow& other) { copyOther(other); return *this; };
 protected:
	void copyOther(const MyRow& other) {
		m_id_d = other.m_id_d;
		m_first_monad = other.m_first_monad;
		m_last_monad = other.m_last_monad;
		m_features = other.m_features;
	};
};

class MyTriple {
 public:
	monad_m first, second;
	bool bReverseOrder;
	MyTriple(monad_m f, monad_m s, bool bro = false) {
		first = f;
		second = s;
		bReverseOrder = bro;
	};
	~MyTriple() {};
	bool operator<(const MyTriple& O2) const {
		const MyTriple& O1 = *this;
		if (O1.bReverseOrder ||
		    O2.bReverseOrder) {
			if (O1.first < O2.first) {
				return true;
			} else if (O1.first >= O2.first) {
				return false;
			} else {
				return false;
			}
		} else {
			if (O1.first < O2.first) {
				return true;
			} else if (O1.first > O2.first) {
				return false;
			} else if (O1.second > O2.second) {
				return true;
			} else if (O1.second < O2.second) {
				return false;
			} else {
				return false;
			}
		}
	};
};



typedef std::multimap<MyTriple, MyRow> MonadMMyRowMap;


class MyTable {
 private:
	MonadMMyRowMap m_rows;
	MonadMMyRowMap::const_iterator m_cur, m_end;
	StringVec m_feature_names;
	monad_m m_largest_object_length;
 public:
	MyTable(MyTable *pOtherTable, const SetOfMonads& restriction_set);
	MyTable(const StringVec& feature_names);
	~MyTable() {};
	std::string toString() const;
	unsigned int length(void) const;
	const MyRow& first_row(void) const { return m_rows.begin()->second; };
	void set_universe(monad_m start_monad, monad_m end_monad);
	void set_universe_all(void) { m_cur = m_rows.begin(); m_end = m_rows.end(); };
	bool hasNext(void) const;
	const MyRow& next(void);
	const std::string& getFeatureNameByIndex(unsigned int i) { return m_feature_names[i]; };
	void addRow(const MyRow& row) { MyRow r(row); 
		monad_m f(row.getFirstMonad()); 
		monad_m l(row.getLastMonad()); 
		std::pair<const MyTriple, MyRow> val = std::pair<const MyTriple, MyRow>(MyTriple(f,l,false),r); 
		m_rows.insert(val); 
		monad_m object_length = l-f+1;
		if (object_length > m_largest_object_length)
			m_largest_object_length = object_length;
	};
};

typedef std::vector<MyTable*> MyTablePtrVec;

#endif // MYTABLE__H__
