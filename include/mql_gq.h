/*
 * mql_gq.h
 *
 * MQL "Get Query from stream" scanner
 *
 * Ulrik Petersen
 * Created: 6/23-2007
 * Last update: 6/7-2011
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


/**@file mql_gq.h
 *@brief Header file for MQLGQScanner (MQL layer)
 */


#ifndef MQL_GQ__H__
#define MQL_GQ__H__

#include "emdros-lconfig.h"
#include "arena.h"

#if HAVE_ISTREAM
#include <istream>
#else
#include <istream.h>
#endif

class MQLGQScanner {
protected:
	std::istream *pIn;
	Bigstring *pBigstring;
	char *bot, *tok, *ptr, *cur, *pos, *lim, *top, *eof;
	char *bufbase1, *bufbase2;
	bool bCurIsBufbase1;
public:
	MQLGQScanner(std::istream *pIn);
	virtual ~MQLGQScanner();
	Bigstring *scan();
	bool noMoreInput() { return eof != 0 && cur == eof; };
protected:
	char *fill(char* cursor);
	void addToken(void);
private:
	MQLGQScanner(const MQLGQScanner&); // Copy constructor unimplemented
	MQLGQScanner& operator=(const MQLGQScanner&); // Assignment operator unimplemented
};

#endif // MQL_GQ__H__
