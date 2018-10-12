/*
 * consout.h
 *
 * Classes to do output from the query tool.
 *
 * Ulrik Petersen
 * Created: 4/16-2005
 * Last update: 8/30-2008
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



#ifndef CONSOUT__H__
#define CONSOUT__H__

#include "conf.h"
#include "qtoutput.h"

class ConsoleQTOutput : public QTOutputBase {
protected:
  std::ostream *m_pOut;
  bool m_bDeleteOut;
public:
  ConsoleQTOutput(std::ostream* pOut, bool bDeleteOut);
  ~ConsoleQTOutput();
  virtual void print(const std::string& toPrint);
  virtual void startUnit(const std::string& object_type_name);
  virtual void startUnitFeatures(const std::string& object_type_name);
  virtual void endUnit(const std::string& object_type_name);
  virtual void printFeature(const std::string& object_type_name,
			    unsigned int feature_index,
			    const std::string& feature_name,
			    const std::string& feature_value);
  virtual void startFocus(void);
  virtual void endFocus(void);
  virtual void newline(void);
  virtual void endl(void);
  virtual void flush(void);
  virtual void MyYield(void);

  virtual void startTable(const std::string& CSS_class);
  virtual void endTable(const std::string& CSS_class);
  virtual void startTableRow(const std::string& CSS_class);
  virtual void endTableRow(const std::string& CSS_class);
  virtual void startTableCell(const std::string& CSS_class);
  virtual void endTableCell(const std::string& CSS_class);
};



#endif // CONSOUT__H__
