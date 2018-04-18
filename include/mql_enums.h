/*
 * mql_enums.h
 *
 * MQL enumerations
 *
 * Ulrik Petersen
 * Created: 8/21-2003
 * Last update: 5/23-2017
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2004-2017  Ulrik Sandborg-Petersen
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
