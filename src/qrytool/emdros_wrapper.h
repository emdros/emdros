/*
 * emdros_wrapper.h
 *
 * A wrapper around EmdrosEnv.
 *
 * Ulrik Petersen
 * Created: 4/9-2005
 * Last update: 5/31-2005
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



#ifndef EMDROS_WRAPPER__H__
#define EMDROS_WRAPPER__H__

#include "conf.h"
#include "qtoutput.h"
#include <emdros_environment.h>
#include <mql_sheaf.h>
#include <table.h>
#include <emdros-lconfig.h>


class EmdrosSchema; // Forward declaration

class EmdrosWrapper {
  EmdrosEnv *m_pEE;
  Configuration *m_pConf;
  QTOutputBase *m_pOut;
  EmdrosSchema *m_pSchema;
public:
  EmdrosWrapper(EmdrosEnv *pEE,
		Configuration *pConf,
		QTOutputBase *pOut);
  ~EmdrosWrapper();

  bool executeString(const std::string& query);

  bool retrieveSchema(std::ostream *pOut);
  EmdrosSchema *getPSchema(void) { return m_pSchema; };

  // Call this before printing results
  bool configurePOut(std::string& message);  // returns true on success, false on failure. message will (probably) hold something if it returns false.

  void show_result();
  Table *takeOverTable(void) { return m_pEE->takeOverTable(); };
  FlatSheaf *takeOverFlatSheaf(void) { return m_pEE->takeOverFlatSheaf(); };
  void printIfNotQuiet(const std::string& toPrint);
  QTOutputBase *getPOut(void) { return m_pOut; };
  Configuration *getPConf(void) { return m_pConf; };
  EmdrosEnv *getPEE(void) { return m_pEE; };
private:
  void show_table(Table *pTable);
  void show_sheaf(Sheaf *pSheaf);
};

#endif // EMDROS_WRAPPER__H__
