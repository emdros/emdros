/*
 * qdtry.cpp
 *
 * Trial of QDXML framework
 * Created: 1/27-2001
 * Last update: 5/11-2018
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



#include <iostream>
#include <fstream>
#include <istream>
#include <ostream>
#include <string>
#include <debug.h>
#include <minidom.h>
#include <opt.h>
#include <messages.h>
#include <exception_emdros.h>



#define DB_ERROR_MESSAGE  {						\
		std::string dberr_msg = pDB->errorMessage();		\
		std::string err_msg;					\
		if (dberr_msg.length() > 0) {				\
			err_msg = "DB Error message = '" + dberr_msg + "'\n"; \
		} else {						\
			err_msg = "No DB error message available.\n";	\
		};							\
		std::cerr << err_msg;					\
		result = 1;						\
		bContinue = false;					\
		bAllOK = false;						\
	}

#define VALUE_ERROR {				\
		result = 2;			\
		bContinue = false;		\
		bAllOK = false;			\
	}

void show_version(std::ostream& ostr)
{
	ostr << "qdtry from Emdros version " << EMDROS_VERSION << std::endl;
}

bool test_MiniDOM_parse(std::istream *pIn)
{
	MiniDOMDocument *pDoc = 0;
	try {
		pDoc = MiniDOMParseStream(pIn);
	} catch (QDException& e) {
		std::cerr << "FAILURE: test_MiniDOM_parse: Calling MiniDOMParseStream threw a QDException: '" << e.what() << "'" << std::endl;
		return false;
	}

	if (pDoc == 0) {
		std::cerr << "FAILURE: test_MiniDOM_parse returned an empty document." << std::endl;
		return false;
	} else {
		std::cout << "SUCCESS: MiniDOMParseString returned a document." << '\n';
	}

	std::ostringstream ostr1;
	pDoc->pretty(ostr1);

	MiniDOMDocument *pDoc2 = 0;
	try {
		std::istringstream istr(ostr1.str());
		pDoc2 = MiniDOMParseStream(&istr);
	} catch (QDException& e) {
		std::cerr << "FAILURE: test_MiniDOM_parse: 2nd time: Calling MiniDOMParseStream threw a QDException: '" << e.what() << "'" << std::endl;
		delete pDoc;
		return false;
	}

	if (pDoc2 == 0) {
		std::cerr << "FAILURE: test_MiniDOM_parse 2nd time returned an empty document." << std::endl;
		delete pDoc;
		return false;
	} else {
		std::cout << "SUCCESS: MiniDOMParseString 2nd time returned a document." << '\n';
	}

	std::ostringstream ostr2;
	pDoc2->pretty(ostr2);

	delete pDoc;
	delete pDoc2;

	MiniDOMDocument *pDoc3 = 0;
	try {
		std::istringstream istr(ostr1.str());
		pDoc3 = MiniDOMParseStream(&istr);
	} catch (QDException& e) {
		std::cerr << "FAILURE: test_MiniDOM_parse: 3rd time: Calling MiniDOMParseStream threw a QDException: '" << e.what() << "'" << std::endl;
		delete pDoc3;
		return false;
	}

	if (pDoc3 == 0) {
		std::cerr << "FAILURE: test_MiniDOM_parse 3rd time returned an empty document." << std::endl;
		delete pDoc3;
		return false;
	} else {
		std::cout << "SUCCESS: MiniDOMParseString 3rd time returned a document." << '\n';
	}

	std::ostringstream ostr3;
	pDoc3->pretty(ostr3);

	delete pDoc3;

	if (ostr2.str() != ostr3.str()) {
		std::cerr << "FAILURE: test_MiniDOM_parse 3rd time didn't return the same document as the 2nd time." << std::endl;
		return false;
	} else {
		std::cout << "SUCCESS: test_MiniDOM_parse 3rd time returned the same document as the 2nd time." << '\n';
	}

	for (int i = 0; i < 10; ++i) {
		MiniDOMDocument *pDoc4 = 0;
		try {
			std::istringstream istr(ostr1.str());
			pDoc4 = MiniDOMParseStream(&istr);
		} catch (QDException& e) {
			std::cerr << "FAILURE: test_MiniDOM_parse: nth time: Calling MiniDOMParseStream threw a QDException: '" << e.what() << "'" << std::endl;
			delete pDoc4;
			return false;
		}

		if (pDoc4 == 0) {
			std::cerr << "FAILURE: test_MiniDOM_parse nth time returned an empty document." << std::endl;
			return false;
		} else {
			std::cout << "SUCCESS: MiniDOMParseString nth time returned a document." << '\n';
		}

		delete pDoc4;
		pDoc4 = 0;
	}
	
	return true;
}





int main(int argc, char* argv[])
{
	int result = 0;

	try {
		std::istream *pIn = 0;
		std::ifstream fin;
		//bool bIsCIN = false;
		if (argc < 2) {
			pIn = &std::cin;
			//bIsCIN = true;
		} else {
			std::string filename = argv[1];
			
			fin.open(filename.c_str());
			if (!fin) {
				std::cerr << "FAILURE: Could not open file " << filename << " for reading." << std::endl;
				std::cerr << "Action aborted." << std::endl;
				result = 4;
			}
			pIn = &fin;
			//bIsCIN = false;
		}
		
		if (test_MiniDOM_parse(pIn)) {
			std::cout << "SUCCESS." << std::endl;
			result = 0;
		} else {
			std::cerr << "ERROR." << std::endl;
			result = 1;
		}
		
		return result;
	} catch (EmdrosException& e) {
		std::cerr << "ERROR: Exception thrown: " << e.what() << std::endl;
		return 1;
	} catch (std::exception& e) {
		std::cerr << "ERROR: std::exception thrown: " << e.what() << std::endl;
		return 1;
	} catch (...) {
		std::cerr << "ERROR: Unknown Exception thrown." << std::endl;
		return 1;
	}
}


