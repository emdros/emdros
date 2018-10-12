/*
 * jsontry.cpp
 *
 * Test of JSON parsing and pretty-printing
 * Created: Sometime in July 2008
 * Last update: 6/18-2016
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
