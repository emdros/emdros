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

/*
 * Copyright (C) 2001-2018     Ulrik Sandborg-Petersen
 * Copyright (C) 2018-present  Sandborg-Petersen Holding ApS, Denmark
 *
 * Licensed under the MIT License.
 *
 * Please see the file COPYING in the root of the sources for more details.
 *
 */



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
