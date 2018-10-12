/*
 * penn_yylex.h
 *
 * Penn Treebank yylex
 *
 * Ulrik Petersen
 * Created: 5/1 (1st of May, 2001)
 * Last update: 3/21-2013
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


/**@file penn_yylex.h
 *@brief Header file for yylex() function (MQL layer)
 */

#ifndef PENN_YYLEX__H__
#define PENN_YYLEX__H__

#include <string>
#include "string_func.h"
#include "arena.h"

extern int penn_yylex(void *lvalp, void *parm);

#define INTEGER_MAGIC "*&$INTEGER$&*"

class PennToken{
public:
  PennToken() { pString = 0; pBigstring = 0; szTokenName = 0; };
  void setString(std::string* pS) { pString = pS; };
  void setInteger(long i) { pString = 0; integer = i; };
  void setName(const char *szName) { szTokenName = szName; };
  std::string *extractString(void) { std::string *pS = pString; pString = 0; return pS; }
  std::string getTokenName(void) const { if (pString != 0) { return "the string '" + *pString + "'"; } else if (szTokenName == 0) { return std::string("<<EOF>>"); } else if (strcmp(szTokenName, INTEGER_MAGIC) == 0) { return std::string("the integer ") + long2string(integer); } else { return std::string("the token ") + szTokenName; } };
  std::string *pString;
  Bigstring *pBigstring;
  long integer;
  const char *szTokenName;
};

inline void deletePennToken(PennToken* pT)
{
  if (pT!=0) {
    delete pT->pString;
    pT->pString = 0;
    delete pT;
  }
}


#endif // PENN_YYLEX__H__
