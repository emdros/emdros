/*
 * mql_result.h
 *
 * MQL results
 *
 * Ulrik Petersen
 * Created: 3/8-2001
 * Last update: 1/17-2013
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


/**@file mql_result.h
 *@brief Header file for MQLResult (MQL layer)
 */



#ifndef MQL_RESULT__H__
#define MQL_RESULT__H__

#include "emdf.h"
#include "table.h"
#include <string>
#include <map>
#include <list>

class MQLExecEnv; // forward declaration
class Sheaf; // forward declaration
class EMdFOutput; // forward declaration
class FlatSheaf; // forward declaration
class EMdFDB; // forward declaration
class StringList; // forward declaration


class MQLResult : public Table {
private:
  Sheaf *m_sheaf;
  FlatSheaf *m_flatSheaf;
  bool m_bDeleteFullSheaf;
  bool m_bDeleteFlatSheaf;
public:
#ifndef SWIG
  MQLResult(); // For lists
  MQLResult(Sheaf* sheaf, MQLExecEnv *pEE);
  MQLResult(FlatSheaf* flat_sheaf, MQLExecEnv *pEE);
  void setDeleteFullSheaf(bool bDoDelete);
  void setDeleteFlatSheaf(bool bDoDelete);
#endif // !defined SWIG
  ~MQLResult();
  bool isSheaf() const;
  bool isFlatSheaf() const;
  bool isTable() const;
  Sheaf* getSheaf(void) { return m_sheaf; };
  FlatSheaf *getFlatSheaf(void) { return m_flatSheaf; };
  Table* getTable(void) { return this; };
  void out(EMdFOutput *pOut) const;
  static void printDTD(EMdFOutput* pOut);
#ifndef SWIG
  bool flatten(const std::list<std::pair<id_d_t, std::string> >& object_types);
#endif
  bool flatten(); // For all object types in the sheaf
  bool flatten(StringList *pObjectTypeNames, EMdFDB *pDB);
};


#endif /* MQL_RESULT__H__ */
