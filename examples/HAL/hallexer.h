/*
 * hallexer.h
 *
 * HALScanner
 *
 * Ulrik Petersen
 * Created: 6/23-2007
 * Last update: 6/23-2007
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



/**@file hallexer.h
 *@brief Header file for HALScanner
 */


#ifndef HALLEXER__H__
#define HALLEXER__H__

#include <emdros-lconfig.h>

#if HAVE_ISTREAM
#include <istream>
#else
#include <istream.h>
#endif

class HAL_Execution_environment; // Forward declaration

class HALScanner {
protected:
	std::istream *pIn;
	HAL_Execution_environment *pHEE;
	char *bot, *tok, *ptr, *cur, *pos, *lim, *top, *eof;
public:
	HALScanner(std::istream *pIn);
	virtual ~HALScanner();
	int scan(HAL_Execution_environment *pHee);
	bool noMoreInput() { return eof != 0 && cur == eof; };
protected:
	char *fill(char* cursor);
	void addWord(void);
private:
	HALScanner(const HALScanner&); // Copy constructor unimplemented
	HALScanner& operator=(const HALScanner&); // Assignment operator unimplemented
};

#endif /* HALLEXER__H__ */
