/*
 * solution.h
 *
 * Class representing one solution.
 *
 * Ulrik Petersen
 * Created: 4/9-2005
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

#ifndef SOLUTION__H__ 
#define SOLUTION__H__

#include "mytable.h"

#include <string>

#include "monads.h"


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
