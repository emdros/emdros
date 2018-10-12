/*
 * messages.h
 *
 * Standard messages for Emdros
 *
 * Ulrik Petersen
 * Created: 10/6-2006 (October 6, 2006)
 * Last update: 10/6-2006
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


/**@file messages.h
 *@brief Interface for standard Emdros messages
 */

#ifndef _MESSAGES_EMDROS_H__
#define _MESSAGES_EMDROS_H__

#include <ostream>
#include "emdf_enums.h"

extern void emdrosMessageConnectionNotOK(std::ostream *pOut, eBackendKind kind);


#endif
