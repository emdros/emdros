/*
 * bptdump.cpp
 *
 * A front-end for dumping an Emdros database as a Bit Packed Table
 * (BPT)
 *
 * Created: 5/1-2001 (1st of May, 2001)
 * Last update: 6/8-2018
 *
 * Return codes:
 * 
 * 0 : Success
 * 1 : Wrong usage
 * 2 : Connection to backend server could not be established
 * 3 : An exception occurred
 * 4 : Something else went wrong
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2001-2018  Ulrik Sandborg-Petersen
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
#include <iostream>
#include <sstream>
#include <fstream>
#include <string_func.h>
#include <emdfdb.h>
#include <table.h>
#include <emdros-lconfig.h>
#include <emdf_output.h>
#include <emdros_environment.h>
#include <encryption.h>
#include <opt.h>
#include <messages.h>

#include <bptdumper.h>
#include <bpt2dumper.h>


bool exec_bptdumper(EmdrosEnv *pSourceEE, const std::string& bptfilename, const std::string& bptkey, eBackendKind backend_kind, long schema_version, const std::string& payload_filename, const std::string& payload_inkey, const std::string& payload_codec)
{
	bool bResult = false;

	std::string error_message;
	if (backend_kind == kBPT) {
		bResult = BPTdumpAsBPT(pSourceEE, bptfilename, bptkey, error_message, schema_version, payload_filename, payload_inkey, payload_codec);
	} else if (backend_kind == kBPT2) {
		bResult = BPT2dumpAsBPT2(pSourceEE, bptfilename, bptkey, error_message, schema_version, payload_filename, payload_inkey);
	} else {
		bResult = false;
		error_message = "Unknown BPT backend.\n\n";
	}
	if (bResult) {
		std::cout << "SUCCESS!\nThe database was successfully dumped as:\n" << bptfilename << std::endl;
	} else {
		std::cerr << "FAILURE!\nSomething went wrong while dumping database:\n\n"
			  << "---------------------------------------------------------------\n"
			  << error_message 
			  << "\n---------------------------------------------------------------\n"
			  << "FAILURE!" << std::endl;
			
	}

	// Return result
	return bResult;
}



void show_version(std::ostream& ostr)
{
	ostr << "bptdumper from Emdros version " << EMDROS_VERSION << std::endl;
}

void print_usage(std::ostream& ostr)
{
	show_version(ostr);
	ostr << "Usage:" << std::endl;
	ostr << "   bptdumper [options] bptfilename " << std::endl;
	ostr << "   bptdumper (--help|-V|--version)" << std::endl;
	ostr << "OPTIONS:" << std::endl;
	printUsageStandardArguments(ostr);
	ostr << "   -d , --dbname db     Dump this database (required)" << std::endl;
	ostr << "   -pd, --payload-db db    Add this database as a payload (default none)" << std::endl;
	ostr << "   -pk, --payload-key key  Use this key to decrypt the payload (default none)" << std::endl;
	ostr << "   -pc, --payload-codec cd Use this code to compress the payload (zlib, lzma)" << std::endl;
	ostr << "   --key key               Encrypt BPT db with this key" << std::endl;
	ostr << "   -bv, --bpt-version (1|2) Which BPT version to use" << std::endl;
	ostr << "   --schema (1|3|5|7)      Which BPT schema version to use" << std::endl;
	ostr << "DEFAULTS:" << std::endl;
	printUsageDefaultsOfStandardArguments(ostr);
	ostr << "   --bpt-version 1" << std::endl;
	ostr << "   --schema 3 (for BPT version 1)" << std::endl;
	ostr << "   --schema 1 (for BPT version 2)" << std::endl;
	ostr << "   --payload-codec zlib" << std::endl;
}

int main(int argc, char* argv[])
{
	// Set defaults
	eOutputKind output_kind = kOKConsole;
	std::string db_to_dump;
	std::string bptkey;
	std::string bptfilename;
	std::string bptschema_version;
	std::string bpt_version;
	long schema_version;
	std::string hostname;
	std::string user;
	std::string password;
	eBackendKind backend_kind;
	eCharsets charset;
	std::string payload_filename;
	std::string payload_inkey;
	std::string payload_codec = "zlib";
	eBackendKind bpt_backend_kind = kBPT;


	addStandardArguments(false // Do add -e | --encoding
			     );

	addOption("-d", "--dbname", true, 
		  "",
		  "ERROR: -d and --dbname must have a database-name as their argument.\n"
		  "       example: -d book1\n");

	addOption("--key", "--key", true, 
		  "",
		  "ERROR: --key must have a key as its argument.");

	addOption("-pd", "--payload-db", true, 
		  "",
		  "ERROR: -pd and --payload-db must have a BPT database-name as their argument.\n"
		  "       example: -pd book1\n");

	addOption("-pk", "--payload-key", true, 
		  "",
		  "ERROR: --pk and --payload-key must have a key as its argument.");

	addOption("-pc", "--payload-codec", true, 
		  "zlib",
		  "ERROR: -pc and --payload-codec must have either 'zlib' or 'lzma' as its argument.");

	addOption("--schema", "--schema", true, 
		  "3",
		  "ERROR: --key must have a key as its argument.");

	addOption("-bv", "--bpt-version", true, 
		  "1",
		  "ERROR: --key must have a key as its argument.");

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

		// Database name
		getArgumentValue("-d", db_to_dump);

		// Key
		getArgumentValue("--key", bptkey);

		// Payload db
		getArgumentValue("-pd", payload_filename);

		// Payload key
		getArgumentValue("-pk", payload_inkey);

		// Payload codec
		getArgumentValue("-pc", payload_codec);

		// Schema
		getArgumentValue("--schema", bptschema_version);

		// Payload db
		getArgumentValue("-bv", bpt_version);

		if (bpt_version == "1") {
			schema_version = string2long(bptschema_version);
			bpt_backend_kind = kBPT;
		} else if (bpt_version == "2") {
			// We don't have any other than schema version 1
			schema_version = 1;
			bpt_backend_kind = kBPT2;
		} else {
			std::cerr << "Error: Unknown bpt backend version: " << bpt_version << ".\n"
				  << "       Only known versions so far: 1 and 2.\n" << std::endl;
			return 1;
		}

		if (payload_codec != "zlib"
		    && payload_codec != "lzma") {
			std::cerr << "Error: Unknown bpt payload codec: " << payload_codec << std::endl;
			std::cerr << "       Currently known codecs: 'zlib' and 'lzma'." << std::endl;
			return 1;
		}

		if (surplus_arguments.size() == 1) {
			bptfilename = *surplus_arguments.begin();
		} else {
			std::cerr << "Error: You can only specify one bptfilename. You specified: '" << joinList("' and '", surplus_arguments) << "'." << std::endl;
			return 1;
		}
	}

	// Make connection
	EmdrosEnv *pEE = new EmdrosEnv(output_kind, 
				       charset,
				       hostname,
				       user, password,
				       db_to_dump,
				       backend_kind);
	// Zero-fill password
	zeroFillString(password);

	// Check that we connected
	if (!pEE->connectionOk()) {
		emdrosMessageConnectionNotOK(&std::cerr, backend_kind);
		delete pEE;
		return 2;
	}

	int nResult = 3;
	try {
		if (exec_bptdumper(pEE, bptfilename, bptkey, bpt_backend_kind, schema_version, payload_filename, payload_inkey, payload_codec)) {
			nResult = 0;
		} else {
			nResult = 4;
		}
	} catch (EMdFNULLValueException& e) {
		std::cerr << "ERROR: EMdFNULLValueException (Database error)..." << std::endl;
		std::cerr << pEE->getDBError() << std::endl; 
		std::cerr << pEE->getCompilerError() << std::endl;
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (EMdFDBDBError& e) {
		std::cerr << "ERROR: EMdFDBDBError (Database error)..." << std::endl;
		std::cerr << pEE->getDBError() << std::endl; 
		std::cerr << pEE->getCompilerError() << std::endl;
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (EMdFDBException& e) {
		std::cerr << "ERROR: EMdFDBException (Database error)..." << std::endl;
		std::cerr << pEE->getDBError() << std::endl; 
		std::cerr << pEE->getCompilerError() << std::endl;
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (TableColumnException& e) {
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (TableRowException& e) {
		std::cerr << e.what() << std::endl << "Program aborted." << std::endl;
	} catch (TableException& e) {
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

	// Clean up
	delete pEE;
	return nResult;
}
