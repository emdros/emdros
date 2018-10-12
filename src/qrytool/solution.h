/*
 * solution.h
 *
 * Class representing one solution.
 *
 * Ulrik Petersen
 * Created: 4/9-2005
 * Last update: 4/9-2005
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



#ifndef SOLUTION__H__ 
#define SOLUTION__H__

#include "mytable.h"
#include <string>
#include <monads.h>

enum eSolutionTableKind {
  kRaster, kData, kReference
};

class Solution {

  /*
   # A solution is a class with the following members
   # 'monads'   : a SetOfMonads, of the solutions proper 
   # 'focus'    : a SetOfMonads, of the focused monads
   # 'raster'   : an {object_type : PyTable} dictionary, with only one entry
   # 'data'     : an {object_type : PyTable} dictionary, one entry per type,
   # 'reference': an {object_type : PyTable} dictionary, one entry per type,

  */
private:
  SetOfMonads *m_monads, *m_focus;
  OTTableMap m_raster;
  OTTableMap m_data;
  OTTableMap m_reference;
public:
  Solution();

  ~Solution();

  std::string toString() const;

  void setMonads(const SetOfMonads& monads);

  SetOfMonads getMonads(void) const;

  void setFocus(SetOfMonads *pFocus);

  SetOfMonads getFocus(void) const;

  void addMyTable(eSolutionTableKind kind, 
		  const std::string& object_type, 
		  MyTable *pTable);
  MyTable *getMyTable(eSolutionTableKind kind, const std::string& object_type);
  const OTTableMap& getOTTableMap(eSolutionTableKind kind) const;
};

#endif // SOLUTION__H__
