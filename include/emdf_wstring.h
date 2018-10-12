/*
 * emdf_wstring.h
 *
 * Definitions for std::wstring if missing
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




#ifndef EMDF_WSTRING__H__
#define EMDF_WSTRING__H__

#include "emdros-lconfig.h"
#include <string>

#if !HAVE_WSTRING

namespace std {
  typedef basic_string<wchar_t> wstring;
}

#endif


#if !HAVE_OSTREAM_OUT_WSTRING

namespace std {
extern  std::ostream& operator<<(std::ostream& ostr, std::wstring& ws);
}

#endif


#endif /* EMDF_WSTRING__H__ */
