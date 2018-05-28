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
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2005-2008  Ulrik Sandborg-Petersen
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

#ifndef CONSOUT__H__
#define CONSOUT__H__

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
