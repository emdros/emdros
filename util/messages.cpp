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
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2005-2018  Ulrik Sandborg-Petersen
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
