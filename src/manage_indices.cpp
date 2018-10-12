/*
 * manage_indices.cpp
 *
 *
 *
 * Tool to manage indices on Emdros databases.  Either creates or
 * drops all indices.  If a "--type object-type" argument is given,
 * then only that object type gets its indices created or dropped.  If
 * no such argument is given, all all object types in the database
 * have their indices created or dropped.
 *
 * Synopsis:
 *   manage_indices --create [options] database-name
 *   manage_indices --drop [options] database-name
 *   manage_indices --help
 *
 * Supports -t, -h, -u, and -p arguments (see output from --help).
 * 
 *
 *
 * Created: 11/13-2001
 * Last update: 5/11-2018
 *
 * Return codes:
 * 
 * 0 : Success
 * 1 : Wrong usage
 * 2 : Connection to backend server could not be established
 * 3 : An exception occurred
 * 5 : Database error
 * */

/*
 * Copyright (C) 2001-2018     Ulrik Sandborg-Petersen
 * Copyright (C) 2018-present  Sandborg-Petersen Holding ApS, Denmark
 *
 * Licensed under the MIT License.
 *
 * Please see the file COPYING in the root of the sources for more details.
 *
 */



#include <emdfdb.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <infos.h>
#include <monads.h>
#include <utils.h>
#include <emdf_output.h>
#include <encryption.h>
#include <debug.h>
#include <opt.h>
#include <messages.h>



#if USE_POSTGRESQL
#include <pgemdfdb.h>
#endif
#if USE_MYSQL
#include <mysqlemdfdb.h>
#endif
#if USE_SQLITE2
#include <sqliteemdfdb.h>
#endif
#if USE_SQLITE3
#include <sqlite3emdfdb.h>
#endif


#define TASK_NONE    (0)
#define TASK_CREATE  (1)
#define TASK_DROP    (2)

void show_version(std::ostream& ostr)
{
	ostr << "manage_indices from Emdros version " << EMDROS_VERSION << std::endl;
}

void print_usage(std::ostream& ostr)
{
	show_version(ostr);
	ostr << "Usage:" << std::endl;
	ostr << "    manage_indices  --create  [options]  database-name  " << std::endl;
	ostr << "    manage_indices  --drop    [options]  database-name" << std::endl;
	ostr << "    manage_indices  --help" << std::endl;
	ostr << "    manage_indices  (-V|--version)" << std::endl;
	ostr << "OPTIONS:" << std::endl;
	printUsageStandardArguments(ostr);
	ostr << "   -t, --type objecttype" << std::endl;

	ostr << "DEFAULTS:" << std::endl;
	printUsageDefaultsOfStandardArguments(ostr);
}


int main(int argc, char* argv[])
{
	EMdFDB* pDB;

	int nTask = TASK_NONE;
	bool bObjectType = false;
	std::string strObjectType;
	std::string strDatabase;
	std::string hostname;
	std::string user;
	std::string password;
	eCharsets dummy_charset;
	eBackendKind backend_kind;

	addStandardArguments(false // Do not add -e | --encoding
			     );
	addOption("--create", "--create");
	addOption("--drop", "--drop");
	addOption("-t", "--type", true,
		  "",
		  "ERROR:  -t and --type must have an existing object type as their argument.\n"
		  "       example: -t clause\n");

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
					  dummy_charset,
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

		// task
		if (getArgumentPresent("--create")) {
			nTask = TASK_CREATE;
		}
		if (getArgumentPresent("--drop")) {
			nTask = TASK_DROP;
		} 

		if (nTask != TASK_DROP
		    && nTask != TASK_CREATE) {
			print_usage(std::cerr);
			std::cerr << "ERROR: You must specify one of --create or --drop when not doing\n"
				  << "       --version or --help." << std::endl;
			return 1;
		}

		bObjectType = getArgumentValue("-t", strObjectType);
		if (bObjectType
		    && strObjectType == "") {
			print_usage(std::cerr);
			std::cerr << "ERROR: -t and --type must have an object type as their argument,\n"
				  << "       not the empty string!" << std::endl;
			return 1;
		}

		if (surplus_arguments.size() == 0) {
			strDatabase = "";
			print_usage(std::cerr);
			std::cerr << "ERROR: You must specify a database on the command line." << std::endl;
			return 1;
		} else if (surplus_arguments.size() == 1) {
			strDatabase = *surplus_arguments.begin();
		} else {
			print_usage(std::cerr);
			std::cerr << "Error: You can only specify one database. You specified: '" << joinList("' and '", surplus_arguments) << "'." << std::endl;
			return 1;
		}
	}
	
	
	// Make connection
	if (backend_kind == kPostgreSQL) {
#if USE_POSTGRESQL
		pDB = new PgEMdFDB(hostname, user, password, strDatabase);
#else
		throw EmdrosException(std::string("PostgreSQL support not compiled in. Cannot make a PostgreSQL connection."));
#endif
	} else if (backend_kind == kMySQL) {
#if USE_MYSQL
		pDB = new MySQLEMdFDB(hostname, user, password, strDatabase);
#else
		throw EmdrosException(std::string("MySQL support not compiled in. Cannot make a MySQL connection."));
#endif
	} else if (backend_kind == kSQLite2) {
		throw EmdrosException(std::string("SQLite 2.X.X is no longer supported."));
	} else if (backend_kind == kSQLite3) {
#if USE_SQLITE3
		pDB = new SQLite3EMdFDB(strDatabase, password);
#else
		throw EmdrosException(std::string("SQLite 3.X.X support not compiled in. Cannot make a SQLite 3.X.X connection."));
#endif
	} else {
		print_usage(std::cerr);
		std::cerr << "Unknown backend." << std::endl;
		return 1;
	}


	// Check that we connected
	if (!pDB->connectionOk()) {
		emdrosMessageConnectionNotOK(&std::cerr, backend_kind);
		delete pDB;
		return 2;
	}

	int nResult = 3;
	try {
		switch (nTask) {
		case TASK_CREATE:
			if (bObjectType) {
				nResult = (pDB->createIndicesOnObjectType(strDatabase, password, strObjectType)) ? 0 : 5;
			} else {
				nResult = (pDB->createIndicesOnDatabase(strDatabase, password)) ? 0 : 5;
			}
			break;
		case TASK_DROP:
			if (bObjectType) {
				nResult = (pDB->dropIndicesOnObjectType(strDatabase, password, strObjectType)) ? 0 : 5;
			} else {
				nResult = (pDB->dropIndicesOnDatabase(strDatabase, password)) ? 0 : 5;
			}
			break;
		default:
			ASSERT_THROW(false, "Unknown task kind");
			break;
		}
		if (nResult != 0) {
			nResult = 5;
			std::cerr << "ERROR: Database error..." << std::endl;
			std::cerr << pDB->errorMessage() << std::endl; 
		}
	} catch (EMdFDBException& e) {
		std::cerr << "ERROR: EMdFDBException (Database error)..." << std::endl;
		std::cerr << pDB->errorMessage() << std::endl; 
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (BadMonadsException& e) {
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (WrongCharacterSetException& e) {
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (EMdFOutputException& e) {
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (EmdrosException& e) {
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (...) {
		std::cerr << "Unknown exception occurred.  Program aborted." << std::endl;
	} 

	// Zero-fill password
	zeroFillString(password);

	// Clean up
	delete pDB;
	return nResult;
}


