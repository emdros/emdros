/*
 * emdf_wstring.cpp
 *
 * Functions for wstring
 *
 * Ulrik Petersen
 * Created: 3/8-2002 (March 8, 2002)
 * Last update: 4/29-2006
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2002-2006   Ulrik Petersen
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


#include <emdros-lconfig.h>
#include <emdf_wstring.h>

#if HAVE_OSTREAM
#include <ostream>
#else
#include <ostream.h>
#endif


#if !HAVE_OSTREAM_OUT_WSTRING


// Output a wchar_t
void emdf_out_wchar_t(std::ostream& ostr, wchar_t wc)
{
  char c;
  // Out lower 8 bits
  c = wc & 0xFF;
  ostr << c;
  // Out higher 8 bits
  c = (wc >> 8) & 0xFF;
  ostr << c;
}

namespace std {

// Overload operator
std::ostream& operator<<(std::ostream& ostr, std::wstring& ws)
{
  std::wstring::const_iterator ci(ws.begin());
  std::wstring::const_iterator cend(ws.end());
  while (ci != cend) {
    emdf_out_wchar_t(ostr, *ci);
    ++ci;
  }
  return ostr;
}

} // namespace std

#endif // !HAVE_OSTREAM_OUT_WSTRING
