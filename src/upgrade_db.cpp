/*
 * Upgrade_db.cpp
 *
 *
 *
 * Tool to upgrade an Emdros database created with version 1.1.4 or 
 * earlier to one that works with Emdros release 1.1.8 and later.
 *
 * Run it with the name of the database to upgrade as the last
 * argument.
 * 
 * Supports -h, -u, and -p arguments (see output from --help).
 *
 * For example:
 *    ./upgrade_db book
 * 
 *    ./upgrade_db --host myhost.somedomain.com mydb
 * 
 *
 *
 * Created: 11/11-2001
 * Last update: 1/7-2009
 *
 * Return codes:
 * 
 * 0 : Success
 * 1 : Wrong usage
 * 2 : Connection to backend server could not be established
 * 3 : An exception occurred
 * 4 : We could not upgrade
 * 5 : Database error
 * */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2009  Ulrik Sandborg-Petersen
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


#include <emdros-config.h>
// Do this because emdros-config might #define malloc,
// in which case some systems will fail to use the real 
// malloc, thus causing build errors.
#undef malloc

#include <monads.h>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <emdfdb.h>
#include <table.h>
#include <emdros-lconfig.h>
#include <emdf_output.h>
#include <emdros_environment.h>
#include <mql_execution_environment.h>
#include <encryption.h>
#include <opt.h>
#include <messages.h>





void show_version(std::ostream& ostr)
{
	ostr << "upgrade_db from Emdros version " << EMDROS_VERSION << std::endl;
}

void print_usage(std::ostream& ostr)
{
	show_version(ostr);
	ostr << "Usage:" << std::endl;
	ostr << "   upgrade_db [options] databasename " << std::endl;
	ostr << "   upgrade_db --help" << std::endl;
	ostr << "   upgrade_db (-V|--version)" << std::endl;
	ostr << "OPTIONS:" << std::endl;
	printUsageStandardArguments(ostr);
	ostr << "DEFAULTS:" << std::endl;
	printUsageDefaultsOfStandardArguments(ostr);
}

bool show_warning(void)
{
	std::cout << std::endl;
	std::cout << "upgrade_db:" << std::endl;
	std::cout << "  This program upgrades an Emdros database created with a version" << std::endl;
	std::cout << "  of Emdros prior to 1.1.8 to a database that can be used with Emdros" << std::endl;
	std::cout << "  version 1.1.8 and later, but only to a database version in the 1.1-series." << std::endl;
	std::cout << std::endl;
	std::cout << "  WARNING: This software may make a lot of changes to the database." << std::endl;
	std::cout << "           It is recommended that you BACKUP YOUR DATABASE before proceeding." << std::endl;
	std::cout << std::endl;
	std::cout << "  Are you sure you want to proceed? (y/n)" << std::endl;
	char c;
	c = getchar();
	return c == 'Y' || c == 'y';
}

int main(int argc, char* argv[])
{
	// Declare option-strings
	std::string database_to_upgrade;
	std::string password;
	std::string hostname;
	std::string user;
	eCharsets charset = kCSISO_8859_1;
	eBackendKind backend_kind;

	addStandardArguments(false // Do not add -e | --encoding
			     );

	// Parse arguments
	std::list<std::string> surplus_arguments;

	std::string error_message;
	if (!parseArguments(argc, argv, error_message, surplus_arguments)) {
		print_usage(std::cerr);
		std::cerr << error_message << std::endl;
		return 1;
	} else {
		bool bShowVersion;
		bool bShowHelp;
		std::string error_message;
		if (!getStandardArguments(bShowVersion, bShowHelp,
					  hostname,
					  user,
					  password,
					  backend_kind,
					  charset,
					  error_message)) {
			print_usage(std::cerr);
			std::cerr << error_message << std::endl;
			return 1;
		}

		// -V | --version
		if (bShowVersion) {
			show_version(std::cout);
			return 0;
		}

		// --help
		if (bShowHelp) {
			print_usage(std::cout);
			return 0;
		}
		if (surplus_arguments.size() == 0) {
			print_usage(std::cerr);
			std::cerr << "ERROR: You must specify a database to upgrade on the command line." << std::endl;
			return 1;
		} else if (surplus_arguments.size() == 1) {
			database_to_upgrade = *surplus_arguments.begin();
		} else {
			std::cerr << "Error: You can only specify one database o upgrade. You specified: '" << joinList("' and '", surplus_arguments) << "'." << std::endl;
			return 1;
		}

		if (database_to_upgrade == "") {
			print_usage(std::cerr);
			std::cerr << "ERROR: Database name to upgrade must not be the empty string." << std::endl;
			return 1;
		}
	}

	// Make connection
	EmdrosEnv *pEE = new EmdrosEnv(kOKConsole, charset,
				       hostname,
				       user, password,
				       database_to_upgrade,
				       backend_kind);

	EMdFDB *pDB;
	pDB = (pEE->getMQLEE())->pDB;

	// Zero-fill password
	zeroFillString(password);

	int nResult = 3;
	try {

		// Check that we connected
		if (!pDB->connectionOk()) {
			emdrosMessageConnectionNotOK(&std::cerr, backend_kind);
			delete pEE;
			return 2;
		}

		// Check whether we need upgrading
		std::string strCurrentDBVersion;
		bool bNeedsUpgrading;
		bool bCanUpgrade;
		int nCurrentVersion;
		int nVersionWhichWeCanUpgradeTo;
		std::string strVersionWhichWeCanUpgradeTo;
		if (!pDB->getCurrentDatabaseVersion(database_to_upgrade, 
						    "", // Right now, we only upgrade PostgreSQL and MySQL databases, so we just pass an empty key.
						    strCurrentDBVersion, 
						    nCurrentVersion, 
						    nVersionWhichWeCanUpgradeTo, 
						    strVersionWhichWeCanUpgradeTo, 
						    bNeedsUpgrading, 
						    bCanUpgrade)) {
			std::cerr << "ERROR: Database error..." << std::endl;
			std::cerr << pDB->errorMessage() << std::endl; 
			std::cerr << "Could not determine current database version. Aborting without modifying DB." << std::endl;
			delete pEE;
			return 5;
		}
    
		// Show current version
		std::cout << "Current database version:" << std::endl;
		std::cout << "    *** " << strCurrentDBVersion << " ***" << std::endl;

		// Check to see whether we should upgrade
		if (!bNeedsUpgrading) {
			std::cout << "The database is compatible with the current version of Emdros," << std::endl
				  << "and doesn't need upgrading. Aborting without modifying database." << std::endl;
			delete pEE;
			return 0;
		}

		if (!bCanUpgrade) {
			std::cout << "ERROR: Cannot upgrade the database." << std::endl;
			std::cout << "Exiting without doing anything to the database." << std::endl << std::endl;
			std::cout << "This most likely means that the upgrade_db program does not provide an" << std::endl;
			std::cout << "upgrade path from the version of the database to the current version of" << std::endl;
			std::cout << "Emdros." << std::endl << std::endl;
			std::cout << "You may be able to use the mqldump program to dump the database and reload it" << std::endl;
			std::cout << "with the current version of Emdros." << std::endl;
			delete pEE;
			return 4;
		}

		std::cout << "We can upgrade the DB to the following version:" << std::endl;
		std::cout << "    *** " << strVersionWhichWeCanUpgradeTo << " ***" << std::endl;
		if (nVersionWhichWeCanUpgradeTo != nCurrentVersion) {
			std::cout << "INFORMATION:" << std::endl;
			std::cout << "    We CANNOT upgrade directly to the current version." << std::endl;
			if (nVersionWhichWeCanUpgradeTo == EMDROS_VERSION_1_1_8_TO_1_1_20) {
				std::cout << "    It looks like you can only upgrade to a version compatible with 1.1.8." << std::endl
					  << "    This is actually good news, since mqldump from the 1.1-series works with" << std::endl
					  << "    databases from 1.1.8 and upwards.  You can then use a 1.1-series mqldump" << std::endl
					  << "    to dump the database and reimport it in the new series.  Just use an" << std::endl
					  << "    mqldump from the 1.1-series.  mqldump was introduced in 1.1.11." << std::endl;
			} else {
				std::cout << "    You may be able to use mqldump to dump the database and import it again" << std::endl;
				std::cout << "    with the current version.  Use an mqldump that matches the version of" << std::endl;
				std::cout << "    the database." << std::endl;
			}
		}

		// Show warning
		if (!show_warning()) {
			std::cout << "OK, aborting without modifying DB." << std::endl;
			delete pEE;
			return 0;
		}


		// Show our intention
		std::cout << "Upgrading database to version 1.1.8 and beyond..." << std::endl;

		if (pDB->upgradeDatabase(database_to_upgrade, nCurrentVersion)) {
			nResult = 0;
		} else {
			nResult = 5;
			std::cerr << "ERROR: Database error..." << std::endl;
			std::cerr << pDB->errorMessage() << std::endl; 
		}
	} catch (EMdFDBException e) {
		std::cerr << "ERROR: EMdFDBException (Database error)..." << std::endl;
		std::cerr << pEE->getDBError() << std::endl; 
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (BadMonadsException e) {
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (WrongCharacterSetException e) {
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (EMdFOutputException e) {
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (EmdrosException e) {
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (...) {
		std::cerr << "Unknown exception occurred.  Program aborted." << std::endl;
	} 

	// Clean up
	delete pEE;

	if (nResult == 0) {
		std::cout << "Database " << database_to_upgrade << " was successfully upgraded." << std::endl;
	} else {
		std::cerr << "An error occurred while upgrading database " << database_to_upgrade << "." << std::endl;
		std::cerr << "Reason: ";
		if (nResult == 3) {
			std::cerr << "An exception occurred. (See above.)" << std::endl;
		} else if (nResult == 5) {
			std::cerr << "A database error occurred. (See above.)" << std::endl;
		}
		std::cerr << "Database was not successfully upgraded." << std::endl;
		std::cerr << "Try restoring from a backup and trying once more." << std::endl;
		std::cerr << "If this does not work, please contact the Emdros maintainer," << std::endl;
		std::cerr << "Ulrik Petersen, at ulrikp@users.sourceforge.net with a problem description." << std::endl;
	}
	return nResult;
}
