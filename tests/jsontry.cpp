/*
 * jsontry.cpp
 *
 * Test of JSON parsing and pretty-printing
 * Created: Sometime in July 2008
 * Last update: 6/18-2016
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2008-2016  Ulrik Sandborg-Petersen
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

#include <json_classes.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

void usage()
{
	std::cerr << "Usage: jsontry [-d|-c] <jsonfilename.json>" << std::endl;
	std::cerr << "OPTIONS:" << std::endl;
	std::cerr << "     -d    Debug: Upon failure, print the output scanned." << std::endl;
	std::cerr << "     -c    Compact the JSON output. Implies -d." << std::endl;
	std::cerr << "     -u    Escape Unicode as \\uxxxxxx. Implies -d." << std::endl;
	std::cerr << "DEFAULTS:" << std::endl;
	std::cerr << "     None, meaning: Pretty-print the output upon success." << std::endl;

}

int main(int argc, char *argv[])
{
	bool bPrintParsedInputUponFailure = false;
	bool bPrintCompactedOutput = false;
	bool bPrintUnicodeAsEscaped = false;
	std::string filename;

	if (argc == 2) {
		bPrintParsedInputUponFailure = false;
		filename = argv[1];
	} else if (argc == 3) {
		if (std::string(argv[1]) != "-d"
		    && std::string(argv[1]) != "-c"
		    && std::string(argv[1]) != "-u") {
			usage();
			return 1;
		}
	 	bPrintParsedInputUponFailure= true;
		if (std::string(argv[1]) == "-c") {
			bPrintCompactedOutput = true;
		} else if (std::string(argv[1]) == "-u") {
			bPrintUnicodeAsEscaped = true;
		}
		filename = argv[2];
	} else {
		usage();
		return 1;
	}
	std::ifstream fin;

	fin.open(filename.c_str());
	if (!fin) {
		std::cerr << "FAILURE: Could not open file " << filename << " for reading." << std::endl;
		std::cerr << "Action aborted." << std::endl;
		return 1;
	}

	std::ostringstream ostr;
	while (!fin.eof()) {
		std::string line;
		getline(fin, line);
		ostr << line << '\n';
	}

	int nResult = 0;
	bool bCompilerResult = false;
	JSONExecEnv *pEE = new JSONExecEnv();
	JSONValue *pValue = 0;
	pValue = parseJSON(pEE, ostr.str(), bCompilerResult);
	if (bCompilerResult) {
		if (bPrintCompactedOutput) {
			pValue->printCompact(&std::cout, bPrintUnicodeAsEscaped);
		} else {
			int indent_level = 0;
			pValue->pretty(&std::cout, indent_level, bPrintUnicodeAsEscaped);
		}
		std::cerr << "\nSUCCESS!" << std::endl;
		nResult = 0;
	} else {
		if (bPrintParsedInputUponFailure) {
			std::cerr << "---------- start of consumed input -----\n";
			std::cerr << pEE->getInputScannedSoFar() << '\n';
			std::cerr << "---------- end of consumed input -----\n";
		}
		std::cerr << pEE->getError();
		std::cerr << "\nFAILURE!" << std::endl;
		nResult = 2;
	}

	delete pEE;
	delete pValue;

	return nResult;
}
