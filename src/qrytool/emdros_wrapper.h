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
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2005  Ulrik Petersen
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
