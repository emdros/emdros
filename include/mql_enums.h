/*
 * mql_enums.h
 *
 * MQL enumerations
 *
 * Ulrik Petersen
 * Created: 8/21-2003
 * Last update: 11/14-2018
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


/**@file mql_enums.h
 *@brief Header file for enumerations relating to the MQL layer
 */



#ifndef MQL_ENUMS__H__
#define MQL_ENUMS__H__

enum eUniverseOrSubstrate {
	kMSNSubstrate,
	kMSNUniverse
};

// From mql_sheaf.h

/** MatchedObject kind (in Sheaf)
 *
 * Used to distinguish which kind a MatchedObject is.
 * The user will only ever see kMOKID_D and kMOKID_M.  The others
 * are used internally while creating the sheaf.
 */
enum eMOKind { 
	kMOKNIL_mo,   /**< NIL (will never occur in sheaf -- means "failure" of an (opt)_gap_block). */
	kMOKEMPTY_mo, /**< EMPTY (will never occur in sheaf -- is for (opt_)gap blocks that are empty, and also for STAR blocks that are empty.) */
	kMOKID_D,     /**< ID_D (from an object_block_(first)) */
	kMOKID_M      /**< ID_D (from an (opt_)gap_block) */
};


// For mql_select_statements.cpp and mql_query.cpp
enum eAggregateQueryStrategy {
	kAQSOutermostFirst,
	kAQSInnermostFirst
};


enum eAggregateFunction {
	kAFMIN,
	kAFMAX,
	kAFSUM,
	kAFCOUNT_ALL,
	kAFCOUNT_FEATURE_COMPARISON
};



#endif /* MQL_ENUMS__H__ */
