/*
 * schema.h
 *
 * Classes to hold an Emdros DB schema
 *
 * Ulrik Petersen
 * Created: 5/31-2005
 * Last update: 10/3-2011
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2005-2011  Ulrik Sandborg-Petersen
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

#ifndef SCHEMA__H__
#define SCHEMA__H__

#include "monads.h"
#include "infos.h"
#include "emdros_environment.h"
#include <string>
#include <map>
#include <set>
#include <list>
#include "conf.h"

class EmdrosSchemaPart {
protected:
  EmdrosEnv *m_pEE;
public:
  EmdrosSchemaPart(EmdrosEnv *pEE) : m_pEE(pEE) {};
  ~EmdrosSchemaPart() {}; // Don't delete pEE; you don't own it!
  bool ExecuteEmdrosString(std::string query) { 
    bool bCompilerResult = true; 
    if (!m_pEE->executeString(query, bCompilerResult, false, false))
      return false;
    else
      return bCompilerResult;
  };
  Table *TakeOverTable() { return m_pEE->takeOverTable(); };
};

typedef std::map<std::string, SetOfMonads> stringSOMMap;

class EmdrosMonadSets : protected EmdrosSchemaPart {
private:
  stringSOMMap m_map;
public:
  typedef stringSOMMap::const_iterator const_iterator;
  EmdrosMonadSets(EmdrosEnv *pEE) : EmdrosSchemaPart(pEE) {};
  ~EmdrosMonadSets() {};
  bool populate(std::ostream *pOut); // pOut used for messages.
  const_iterator begin() const { return m_map.begin(); };
  const_iterator end() const { return m_map.end(); };
  bool isEmpty(void) const { return m_map.begin() == m_map.end(); };
};


typedef std::set<std::string> StringSet;
typedef std::map<std::string, StringSet> StringStringSetMap;
typedef StringStringSetMap EnumerationMap;

class EmdrosEnumerations : protected EmdrosSchemaPart {
private:
  EnumerationMap m_map;
public:
  EmdrosEnumerations(EmdrosEnv *pEE) : EmdrosSchemaPart(pEE) {};
  ~EmdrosEnumerations() {};
  bool populate(std::ostream *pOut); // pOut used for messages.
  bool isEmpty(void) const { return m_map.begin() == m_map.end(); };
  StringVec getEnumNames(void) const;
  StringVec getEnumConstantNames(const std::string& enumeration_name) const;
};



typedef struct FeatureNameAndType {
  FeatureNameAndType(const std::string& n, const std::string& t) : name(n), type(t) {};
  ~FeatureNameAndType() {};
  std::string name;
  std::string type;
} FeatureNameAndType;

typedef std::list<FeatureNameAndType> fi_list_t;
typedef std::map<std::string, fi_list_t> string_fi_list_map;

class EmdrosObjectTypes : protected EmdrosSchemaPart {
private:
  string_fi_list_map m_map;
public:
  EmdrosObjectTypes(EmdrosEnv *pEE) : EmdrosSchemaPart(pEE) {};
  ~EmdrosObjectTypes() {};
  bool populate(std::ostream *pOut); // pOut used for messages.
  bool isEmpty(void) const { return m_map.begin() == m_map.end(); };
  StringVec getObjectTypeNames(void) const;
  std::list<FeatureNameAndType> getFeaturesOfObjectType(const std::string& object_type_name) const;
  static bool FeatureTypeIsEnum(const FeatureNameAndType& fnat);
};

class EmdrosSchema {
private:
  EmdrosEnumerations m_enums;
  EmdrosObjectTypes m_object_types;
  EmdrosMonadSets m_monad_sets;
public:
  EmdrosSchema(EmdrosEnv *pEE);
  ~EmdrosSchema();
  bool populate(std::ostream *pOut); // pOut used for messages.
  const EmdrosEnumerations& getEnums(void) const { return m_enums; };
  const EmdrosObjectTypes& getObjectTypes(void) const { return m_object_types; }
  const EmdrosMonadSets& getMonadSets(void) const { return m_monad_sets; };
};


#endif // SCHEMA__H__
