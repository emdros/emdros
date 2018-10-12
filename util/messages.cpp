/*
 * messages.cpp
 *
 * Standard messages for Emdros
 *
 * Ulrik Petersen
 * Created: 10/6-2006 (October 6, 2006)
 * Last update: 4/18-2018
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


/**@file messages.cpp
 *@brief Implementation of standard Emdros messages
 */


#include <string>
#include <ostream>
#include <emdf_enums.h>
#include <debug.h>

/** Print emdros message when connection to backend is not OK.
 *
 * @param pOut The std::ostream on which to print the message.
 *
 * @param kind The eBackendKind to which the message is to pertain. A
 * distinction is made between kMySQL and kPostgreSQL on the one hand,
 * and kSQLite3 on the other hand.
 */
void emdrosMessageConnectionNotOK(std::ostream *pOut, eBackendKind kind)
{
	if (kind == kMySQL || kind == kPostgreSQL) {
		(*pOut) << std::endl;
		(*pOut) << "Connection to backend server could not be established." << std::endl;
		(*pOut) << "This usually has one of the following causes:" << std::endl;
		(*pOut) << "  - The back-end is not running." << std::endl;
		(*pOut) << "  - The back-end is not configured for incoming TCP/IP connections." << std::endl;
		(*pOut) << "  - You did not supply a user/password with the -u/-p options." << std::endl;
		(*pOut) << "There are hints in the HOW-TO-USE document on configuring your back-end." << std::endl;
		(*pOut) << "The full documentation comes with Emdros." << std::endl;
		(*pOut) << "    Please see      <http://emdros.org/whattodo3.html>" << std::endl;
		(*pOut) << "    for more information on this problem." << std::endl;
	} else if (kind == kSQLite3) {
		(*pOut) << "Connection to SQLite 3 database could not be established." << std::endl;
	} else if (kind == kBPT) {
		(*pOut) << "Connection to BPT database could not be established." << std::endl;
		(*pOut) << "\nIf this is the Open Source version of Emdros, please be aware\n"
			<< "that the BPT backend is only available through licensing from\n"
			<< "the author of Emdros, Ulrik Sandborg-Petersen. Feel free to contact him:\n" 
			<< "ulrikp@emdros.org\n\nThanks!" << std::endl;
	} else if (kind == kBPT2) {
		(*pOut) << "Connection to BPT2 database could not be established." << std::endl;
		(*pOut) << "\nIf this is the Open Source version of Emdros, please be aware\n"
			<< "that the BPT2 backend is only available through licensing from\n"
			<< "the author of Emdros, Ulrik Sandborg-Petersen. Feel free to contact him:\n" 
			<< "ulrikp@emdros.org\n\nThanks!" << std::endl;
	} else {
		ASSERT_THROW(false,
			     "Unknown backend kind in emdrosMessageConnectionNotOK");
	}
}
