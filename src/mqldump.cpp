/*
 * mqldump.cpp
 *
 * A tool to dump Emdros databases in MQL
 * Created: 5/1-2001 (1st of May, 2001)
 * Last update: 1/7-2009
 *
 * Return codes:
 * 
 * 0 : Success
 * 1 : Wrong usage
 * 2 : Connection to backend server could not be established
 * 3 : An exception occurred
 * 4 : Could not open file
 * 5 : Database error
 * 6 : Compiler error
 *
 */
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

#include <emdros-lconfig.h>
#include <encryption.h>
#include <iostream>
#include <string>
#include <environment_emdros.h>
#include <mql_exporter.h>
#include <opt.h>
#include <messages.h>



void show_version(std::ostream& ostr)
{
	ostr << "mqldump from Emdros version " << EMDROS_VERSION << std::endl;
}

void print_usage(std::ostream& ostr)
{
	show_version(ostr);
	ostr << "Usage:" << std::endl;
	ostr << "   mqldump [options] database [object type names...] " << std::endl;
	ostr << "   mqldump (--help|-V|--version)" << std::endl;
	ostr << "OPTIONS:" << std::endl;
	ostr << "   -v , --verbose       Be verbose" << std::endl;
	printUsageStandardArguments(ostr);
	ostr << "   -o filename          Dump to file rather than stdout" << std::endl;
	ostr << "   -n , --no-create-db            Don't issue statements to create and use db" << std::endl;
	ostr << "   --encoding encoding            Give db encoding in CREATE DATABASE stmnt." << std::endl
	     << "                                  (no effect if -n is used)" << std::endl;
	ostr << "   -i , --no-create-info          Don't issue statements to create object" << std::endl;
	ostr << "                                  types and enums (i.e., meta-data)." << std::endl;
	ostr << "   -t , --no-create-object-types  Don't issue statements to create object" << std::endl
	     << "                                  types" << std::endl;
	ostr << "   -e , --no-create-enums         Don't issue statements to create enums" << std::endl;
	ostr << "   -m , --no-create-monad-sets    Don't issue statements to create arbitrary" << std::endl;
	ostr << "                                  monad sets." << std::endl;
	ostr << "   -d , --no-create-objects       Don't issue statements to create" << std::endl 
	     << "                                  object data" << std::endl;
	ostr << "   --no-create-transactions       Don't issue transaction statements." << std::endl;
	ostr << "   --batch-create-objects         Issue CREATE OBJECTS WITH OBJECT TYPE" << std::endl
	     << "                                  statements rather than individual CREATE" << std::endl
	     << "                                  OBJECTS statements." << std::endl;
	ostr << "   --batch-size number            Set number of objects in each CREATE OBJECTS" << std::endl
	     << "                                  WITH OBJECT TYPE statement. Implies" << std::endl
	     << "                                  --batch-create-objects." << std::endl
	     << "   --start number                 Start dumping from the specified monad." << std::endl
	     << "   --end number                   Finish dumping at the specified monad." << std::endl;
	ostr << "DEFAULTS:  ";
	printUsageDefaultsOfStandardArguments(ostr);
	ostr << "--start <MIN_M>  --end <MAX_M>" << std::endl
	     << "--batch-size " << DEFAULT_BATCH_SIZE 
	     << " (but batch-creation doesn't happen by default.)"<< std::endl;
}

int main(int argc, char* argv[])
{
	// Set defaults
	eOutputKind output_kind = kOKConsole;
	MQLExportEnv de;
	eBackendKind backend_kind;
	eCharsets dummy_charset = kCSISO_8859_1;

	addStandardArguments(false // Do not add -e | --encoding
			     );

	addOption("-v", "--verbose");

	addOption("--no-create-transactions", "--no-create-transactions");

	addOption("-n", "--no-create-db");

	addOption("-i", "--no-create-info");

	addOption("--batch-create-objects", "--batch-create-objects");

	addOption("-e", "--no-create-enums");

	addOption("-m", "--no-create-monad-sets");

	addOption("-t", "--no-create-object-types");

	addOption("-d", "--no-create-objects");

	addOption("--encoding", "--encoding", true, 
		  "", // Default value
		  "ERROR: --encoding must have an argument.\n"
		  "       example: --encoding 'utf-8' or --encoding 'iso-8859-1'\n");

	addOption("-o", "--output", true, 
		  "", // Default value
		  "ERROR: -o and --output must have a filename as their argument.\n"
		  "       example: -o dump.mql\n");

	addOption("--start", "--start", true, 
		  "", // Default value
		  "ERROR: --start must have a monad as its argument\n"
		  "       example: --start 1\n");

	addOption("--end", "--end", true, 
		  "", // Default value
		  "ERROR: --end must have a monad as its argument\n"
		  "       example: --end 10000\n");

	addOption("--batch-size", "--batch-size", true, 
		  long2string(DEFAULT_BATCH_SIZE).c_str(), // Default value
		  "ERROR: --batch-size must have a number as its argument\n"
		  "       example: --end 50000\n");


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
					  de.hostname,
					  de.user,
					  de.password,
					  backend_kind,
					  dummy_charset,
					  error_message)) {
			print_usage(std::cerr);
			std::cerr << error_message << std::endl;
			return 1;
		}

		if (bShowVersion) {
			show_version(std::cout);
			return 0;
		}

		// --help
		if (bShowHelp) {
			print_usage(std::cout);
			return 0;
		}

		if (getArgumentPresent("-v")) {
			de.bVerbose = true;
		}

		if (getArgumentPresent("--no-create-transactions")) {
			de.bUseTransactions = false;
		}

		if (getArgumentPresent("-n")) {
			de.bCreateAndUseDB = false;
		}

		if (getArgumentPresent("-i")) {
			de.bCreateEnums = false;
			de.bCreateObjectTypes = false;
		}

		if (getArgumentPresent("--batch-create-objects")) {
			de.bBatchCreateObjects = true;
		}

		if (getArgumentPresent("-e")) {
			de.bCreateEnums = false;
		}

		if (getArgumentPresent("-m")) {
			de.bCreateArbitraryMonadSets = false;
		}

		if (getArgumentPresent("-t")) {
			de.bCreateObjectTypes = false;
		}

		if (getArgumentPresent("-d")) {
			de.bCreateObjectData = false;
		}

		// -o | --output
		getArgumentValue("-o", de.filename);

		if (getArgumentPresent("--encoding")) {
			de.bDumpEncoding = true;
			getArgumentValue("--encoding", de.encoding);
			eCharsets charset;
			bool bShowError = !string2charset(de.encoding, charset);
			if (!bShowError) {
				if (charset != kCSUTF8
				    && charset != kCSISO_8859_1) {
					bShowError = true;
				}
			}
			if (bShowError) {
				std::cerr << "ERROR: The argument of --encoding must be a valid encoding.\n"
					  << "       Valid encodings:\n"
					  << "          - utf-8\n"
					  << "          - iso-8859-1" << std::endl;
				return 1;
			}
		}


		// --start
		std::string start_string;
		if (getArgumentValue("--start", start_string)) {
			if (string_is_number(start_string)) {
				de.m_start = string2long(start_string);
			} else {
				print_usage(std::cerr);
				std::cerr << "ERROR: The argument of --start must be a number, and not '" << start_string << "'." << std::endl;
				return 1;
			}
		}


		// --end
		std::string end_string;
		if (getArgumentValue("--end", end_string)) {
			if (string_is_number(end_string)) {
				de.m_end = string2long(end_string);
			} else {
				print_usage(std::cerr);
				std::cerr << "ERROR: The argument of --end must be a number, and not '" << end_string << "'." << std::endl;
				return 1;
			}
		}

		// --batch-size
		std::string batch_size_string;
		if (getArgumentValue("--batch-size", batch_size_string)) {
			if (string_is_number(batch_size_string)) {
				de.nBatchSize = string2long(batch_size_string);
				de.bBatchCreateObjects = true;
			} else {
				print_usage(std::cerr);
				std::cerr << "ERROR: The argument of --batch-size must be a number, and not '" << end_string << "'." << std::endl;
				return 1;
			}
		}


		// There must be surplus arguments
		if (surplus_arguments.size() == 0) {
			print_usage(std::cerr);
			std::cerr << "ERROR: You must at least specify a database on the command line.\n"
				  << "       If you wish, you can also specify a number of object types to dump,\n"
				  << "       after the database." << std::endl;
			return 1;
		} else if (surplus_arguments.size() == 1) {
			// If there is only one surplus argument, it is the DB name
			de.db = *surplus_arguments.begin();
		} else {
			// Otherwise, the first is the DB name, and the rest are object types.
			de.db = *surplus_arguments.begin();
			surplus_arguments.erase(surplus_arguments.begin());
			de.object_types = surplus_arguments;
		}
	}

	
	// Make connection
	EmdrosEnv *pEE = new EmdrosEnv(output_kind, kCSISO_8859_1,
				       de.hostname,
				       de.user, de.password,
				       de.db,
				       backend_kind);
	// Zero-fill password
	zeroFillString(de.password);

	// Check that we connected
	if (!pEE->connectionOk()) {
		emdrosMessageConnectionNotOK(&std::cerr, backend_kind);
		delete pEE;
		return 2;
	}

	int nResult = 3;
	std::string message;
	try {
		if (de.filename == "") {
			MQLExporter exporter(de, pEE);
			nResult = exporter.doIt(message);
		} else {
			MQLExporter exporter(de, pEE, de.filename);
			nResult = exporter.doIt(message);
		}
		if (nResult != 0) {
			std::cerr << pEE->getDBError() << std::endl; 
			std::cerr << pEE->getCompilerError() << std::endl;
			std::cerr << message << std::endl;
		}
	} catch (EMdFDBException e) {
		std::cerr << "ERROR: EMdFDBException (Database error)..." << std::endl;
		std::cerr << pEE->getDBError() << std::endl; 
		std::cerr << pEE->getCompilerError() << std::endl;
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
	return nResult;
}


