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

/*
 * Copyright (C) 2001-2018     Ulrik Sandborg-Petersen
 * Copyright (C) 2018-present  Sandborg-Petersen Holding ApS, Denmark
 *
 * Licensed under the MIT License.
 *
 * Please see the file COPYING in the root of the sources for more details.
 *
 */


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
