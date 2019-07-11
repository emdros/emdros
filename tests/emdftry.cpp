/*
 * emdftry.cpp
 *
 * Trial of EMdF framework
 * Created: 1/27-2001
 * Last update: 7/12-2019
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
#include <debug.h>
#include <string>
#include <infos.h>
#include <utils.h>
#include <emdf_output.h>
#include <encryption.h>
#include <inst.h>
#include <string_list.h>
#include <minidom.h>
#include <table.h>
#if USE_POSTGRESQL
#include <pgemdfdb.h>
#endif
#if USE_MYSQL
#include <mysqlemdfdb.h>
#endif
#if USE_SQLITE3
#include <sqlite3emdfdb.h>
#endif

#include <opt.h>
#include <messages.h>



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
	ostr << "emdftry from Emdros version " << EMDROS_VERSION << std::endl;
}


void print_usage(std::ostream& ostr)
{
	show_version(ostr);
	ostr << "Usage:\n";
	ostr << "   emdftry [options] \n";
	ostr << "OPTIONS:\n";
	printUsageStandardArguments(ostr);
	ostr << "DEFAULTS:\n";
	printUsageDefaultsOfStandardArguments(ostr);

}

bool testFastSOMRemoveMonadsBefore(const FastSetOfMonads fsom, monad_m monad, std::string toCompareWith)
{
	FastSetOfMonads result = fsom.removeMonadsBefore(monad);
	if (result.toString() == toCompareWith) {
		std::cout << "SUCCESS: Fast monad set removeMonadsBeforeion of set '" << fsom.toString() << "' to monad " << monad << " gave the correct result, which is: '" << toCompareWith << "'\n";
		return true;
	} else {
		std::cerr << "FAILURE: Fast monad set removeMonadsBeforeion of set '" << fsom.toString() << "' to monad " << monad << " DID NOT gave the correct result, which is: '" << toCompareWith << "'. Instead, it gave: '" << result.toString() <<  "'" << std::endl;
		return false;
	}
}

bool testFastSOMisMemberOf(const FastSetOfMonads fsom, monad_m monad, bool bShouldBeMember)
{
	bool bResult = fsom.isMemberOf(monad);
	if (bResult == bShouldBeMember) {
		std::cout << "SUCCESS: Fast monad set '" << fsom.toString() << "' revealed that monad " << monad << " was" << (bShouldBeMember ? "" : " not") << " a member, which is correct.\n";
		return true;
	} else {
		std::cerr << "FAILURE: Fast monad set '" << fsom.toString() << "' failed to say correctly whether monad " << monad << " was a member." << std::endl;
		return false;
	}
}


bool testFastGapExists(const FastSetOfMonads fsom, monad_m Sm, monad_m m, bool bExpectedResult)
{
	monad_m result_m;
	bool bResult = fsom.gapExists(Sm, result_m);
	if (bExpectedResult) {
		if (bResult) {
			if (m == result_m) {
				std::cout << "SUCCESS: Gap at " << Sm << " exists in fast set of monads " << fsom.toString() << " with m == " << m << std::endl;
				return true;
			} else {
				std::cerr << "FAILURE: Gap at " << Sm << " exists in fast set of monads " << fsom.toString() << ", but m == " << result_m << ", while it should be " << m << std::endl;
				return false;
			}
		} else {
			std::cerr << "FAILURE: Gap at " << Sm << " does not exist in fast set of monads" <<  fsom.toString() << ", which it should!" << std::endl;
			return false;
		}
	} else {
		if (bResult) {
			std::cerr << "FAILURE: Gap at " << Sm << " exists in fast set of monads " << fsom.toString() << ", which it shouldn't!" << std::endl;
			return false;
		} else {
			std::cerr << "SUCCESS: Gap at " << Sm << " does not exist in fast set of monads" << fsom.toString() << ", which it shouldn't anyway.\n";
			return true;
		}
	}
}

bool testSOMfromString(const std::string& instr, const std::string& toCompareWith)
{
	SetOfMonads result;
	result.fromString(instr);
	if (result.toString() == toCompareWith) {
		std::cout << "SUCCESS: SOM::fromString() with string '" << instr << "' gave the correct result.\n";
		return true;
	} else {
		std::cerr << "FAILURE: SOM::fromString() with string '" << instr << "' did not return the right set, instead returning: '" << result.toString() << "'." << std::endl;
		return false;
	}
}


bool testSOMfillInsertRange(const SetOfMonads& inSOM, monad_m first, monad_m last, std::string expectedResult)
{
	SetOfMonads result = inSOM.fillInsertRange(first, last);
	bool bResult = result.toString() == expectedResult;
	if (bResult) {
		std::cout << "SUCCESS: SetOfMonads::fillInsertRange(" << first << "," << last << ") returned '" << result.toString() << "', which it should.\n";
	} else {
		std::cerr << "FAILURE: SetOfMonads::fillInsertRange(" << first << "," << last << ") returned '" << result.toString() << "', but it should return '" << expectedResult << "'." << std::endl;
	}
	return bResult;
}

bool testSOMSelfEquality(const FastSetOfMonads& som)
{
	FastSetOfMonads som2(som);
	bool bResult = som == som2;
	if (bResult) {
		std::cout << "SUCCESS: Set of monads '" << som.toString() << "' compared equal to itself after a copy constructor.\n";
	} else {
		std::cerr << "ERROR: Set of monads '" << som.toString() << "' does not equal itself. The copied set was '" << som2.toString() << "'" << std::endl;
	}
	return bResult;
}

bool testSOMSOMEquality(const SetOfMonads& som1, const SetOfMonads& som2)
{
	bool bResult = som1 == som2;
	if (bResult) {
		std::cout << "SUCCESS: Set of monads '" << som1.toString() << "' compared equal to the other set of monads\n";
	} else {
		std::cerr << "ERROR: Set of monads '" << som1.toString() << "' does not equal the other set of monads, '" << som2.toString() << "'" << std::endl;
	}
	return bResult;
}

bool testSOMSOMEquality(const FastSetOfMonads& som1, const FastSetOfMonads& som2)
{
	bool bResult = som1 == som2;
	if (bResult) {
		std::cout << "SUCCESS: Set of monads '" << som1.toString() << "' compared equal to the other set of monads\n";
	} else {
		std::cerr << "ERROR: Set of monads '" << som1.toString() << "' does not equal the other set of monads, '" << som2.toString() << "'" << std::endl;
	}
	return bResult;
}

bool testSOMHasMonadsInRange(const FastSetOfMonads& som, monad_m fm, monad_m lm, bool bExpectedResult)
{
	bool bResult = som.hasMonadsInRange(fm,lm);
	if (bExpectedResult) {
		if (bResult) {
			std::cout << "SUCCESS: Fast Set of monads '" << som.toString() << "' has monads in range (" << fm << "," << lm << ")\n";
		} else {
			std::cerr << "FAILURE: Fast Set of monads '" << som.toString() << "' did NOT have monads in range (" << fm << "," << lm << ")" << std::endl;
		}
		return bResult;
	} else {
		if (!bResult) {
			std::cout << "SUCCESS: Fast Set of monads '" << som.toString() << "' did NOT have monads in range (" << fm << "," << lm << "), as it shouldn't\n";
		} else {
			std::cerr << "FAILURE: Fast Set of monads '" << som.toString() << "' DID have monads in range (" << fm << "," << lm << "), which it shouldn't" << std::endl;
		}
		return !bResult;
	}
}

bool testFastSOMPart_of(const SetOfMonads O, const FastSetOfMonads& Su, bool bExpectedResult)
{
	bool bResult = O.part_of(Su);
	if (bExpectedResult) {
		if (bResult) {
			std::cout << "SUCCESS: Set of monads '" << O.toString() << "' was part_of Fast Set Of monads " << Su.toString() << ", which it should be!\n";
		} else {
			std::cerr << "FAILURE: Set of monads '" << O.toString() << "' was NOT part_of Fast Set Of monads " << Su.toString() << ", which it should be!" << std::endl;
		}
		return bResult;
	} else {
		if (!bResult) {
			std::cout << "SUCCESS: Set of monads '" << O.toString() << "' was NOT part_of Fast Set Of monads " << Su.toString() << ", as it should not be.\n";
		} else {
			std::cerr << "FAILURE: Set of monads '" << O.toString() << "' WAS part_of Fast Set Of monads " << Su.toString() << ", which it should NOT be." << std::endl;
		}
		return !bResult;
	}
}

bool testSOMPart_of(const SetOfMonads& O, const SetOfMonads Su, bool bExpectedResult)
{
	bool bResult = O.part_of(Su);
	if (bExpectedResult) {
		if (bResult) {
			std::cout << "SUCCESS: Set of monads '" << O.toString() << "' was part_of Set Of monads " << Su.toString() << ", which it should be!\n";
		} else {
			std::cerr << "FAILURE: Set of monads '" << O.toString() << "' was NOT part_of Set Of monads " << Su.toString() << ", which it should be!" << std::endl;
		}
		return bResult;
	} else {
		if (!bResult) {
			std::cout << "SUCCESS: Set of monads '" << O.toString() << "' was NOT part_of Set Of monads " << Su.toString() << ", as it should not be.\n";
		} else {
			std::cerr << "FAILURE: Set of monads '" << O.toString() << "' WAS part_of Set Of monads " << Su.toString() << ", which it should NOT be." << std::endl;
		}
		return !bResult;
	}
}

bool testSOMToFromCompactString(const SetOfMonads& som, bool bShowSuccessError = true) 
{
	std::string somstr = som.toCompactString();
	SetOfMonads som2(somstr);
	bool bContinue = som == som2;
	if (bShowSuccessError) {
		if (bContinue) {
			std::cout << "SUCCESS: to/from compact string on SOM '" << som.toString() << "' succeeded with string '" << somstr << "'.\n";
		} else {
			std::cerr << "ERROR: to/from compact string on SOM '" << som.toString() << "' gave the following string: '" << somstr << "' and the following SOM: '" << som2.toString() << "'" << std::endl;
		}
	}
	return bContinue;
}

bool testSOMToFromCompactStringFM(const SetOfMonads& som, bool bShowSuccessError = true) 
{
	std::string somstr = som.toCompactStringFM();
	SetOfMonads som2;
	som2.fromCompactStringFM(somstr, som.first());
	bool bContinue = som == som2;
	if (bShowSuccessError) {
		if (bContinue) {
			std::cout << "SUCCESS: to/from compact string FM on SOM '" << som.toString() << "' succeeded with string '" << somstr << "'.\n";
		} else {
			std::cerr << "ERROR: to/from compact string FM on SOM '" << som.toString() << "' gave the following string: '" << somstr << "' and the following SOM: '" << som2.toString() << "'" << std::endl;
		}
	}
	return bContinue;
}

bool testSOMCompactStringEqualityWithFM(const SetOfMonads& som, bool bShowSuccessError = true) 
{
	std::string somstrFM = som.toCompactStringFM();
	SetOfMonads som2(som);
	som2.offset(100);
	std::string som2strFM = som2.toCompactStringFM();
	bool bContinue = false;
	bContinue = somstrFM == som2strFM;

	if (bShowSuccessError) {
		if (bContinue) {
			std::cout << "SUCCESS: compact string FM and offset(100) FM on SOM '" << som.toString() << "' and som2 '" << som2.toString() << "' succeeded with compact strings equal to '" << somstrFM << "' and '" << som2strFM << "'.\n";
		} else {
			std::cout << "ERROR: compact string FM and offset(100) FM on SOM '" << som.toString() << "' and som2 '" << som2.toString() << "' failed with compact strings unequal: '" << somstrFM << "' and '" << som2strFM << "'.\n";
		}
	}
	return bContinue;
}

bool testSOMCompactStringEqualityWithAndWithoutFM(const SetOfMonads& som, bool bShowSuccessError = true) 
{
	std::string somstr = som.toCompactString();
	std::string somstrFM = som.toCompactStringFM();
	bool bContinue = false;
	if (som.first() == 0) {
		bContinue = somstr == somstrFM;
	} else {
		bContinue = somstr != somstrFM;
	}
	if (bShowSuccessError) {
		if (bContinue) {
			std::cout << "SUCCESS: compact string w/wo FM on SOM '" << som.toString() << "' succeeded with compact strings '" << somstr << "' and '" << somstrFM << "'.\n";
		} else {
			std::cout << "ERROR: compact string w/wo FM on SOM '" << som.toString() << "' did not succeed with compact strings '" << somstr << "' and '" << somstrFM << "'.\n";
		}
	}
	return bContinue;
}

bool test_parse_C_backslashes(std::string instring, std::string expectedresult)
{
	std::string result = parse_C_backslashes(instring);
	bool bContinue = result == expectedresult;
	if (bContinue) {
		std::cout << "SUCCESS: parse_C_backslashes returned the correct string on the string '" << instring << "'\n";
	} else {
		std::cerr << "FAILURE: parse_C_backslashes did not return the correct string on the string '" << instring << "', instead returning '" << result << "'" << std::endl;
	}
	return bContinue;
}

bool test_strip(std::string instring, std::string stripchars, std::string expectedresult)
{
	std::string result = strip(instring, stripchars);
	bool bContinue = result == expectedresult;
	if (bContinue) {
		std::cout << "SUCCESS: strip returned the correct string on the string '" << instring << "'\n";
	} else {
		std::cerr << "FAILURE: strip did not return the correct string '" << expectedresult << "' on the string '" << instring << "', instead returning '" << result << "'" << std::endl;
	}
	return bContinue;
}

bool test_strip_whitespace(std::string instring, std::string expectedresult)
{
	std::string result = strip(instring);
	bool bContinue = result == expectedresult;
	if (bContinue) {
		std::cout << "SUCCESS: strip (whitespace) returned the correct string on the string '" << instring << "'\n";
	} else {
		std::cerr << "FAILURE: strip (whitespace) did not return the correct string '" << expectedresult << "' on the string '" << instring << "', instead returning '" << result << "'" << std::endl;
	}
	return bContinue;
}

bool test_is_hex(std::string instring, bool bExpectedResult)
{
	bool bResult = is_hex(instring);
	bool bContinue = bResult == bExpectedResult;
	if (bContinue) {
		std::cout << "SUCCESS: is_hex correctly returned " << bResult << " on the string '" << instring << "'\n";
	} else {
		std::cerr << "FAILURE: is_hex incorrectly returned " << bResult << " on the string '" << instring << "'" << std::endl;
	}
	return bContinue;
}

bool test_hex2char(std::string instring, unsigned char expected_result)
{
	unsigned char result = hex2char(instring.c_str());
	bool bContinue = result == expected_result;
	if (bContinue) {
		std::cout << "SUCCESS: hex2char reported the correct char for '" << instring << "'\n";
	} else {
		std::cerr << "FAILURE: hex2char did not report the correct char for '" << instring << "'" << std::endl;
	}
	return bContinue;
}

bool test_octal2char(std::string instring, unsigned char expected_result)
{
	unsigned char result = octal2char(instring.c_str());
	bool bContinue = result == expected_result;
	if (bContinue) {
		std::cout << "SUCCESS: octal2char reported the correct char for '" << instring << "'\n";
	} else {
		std::cerr << "FAILURE: octal2char did not report the correct char for '" << instring << "'" << std::endl;
	}
	return bContinue;
}

bool test_char2octal(unsigned char inchar, std::string expected_result)
{
	std::string result = char2octal(inchar);
	bool bContinue = result == expected_result;
	if (bContinue) {
		std::cout << "SUCCESS: char2octal reported the correct result string for '" << expected_result << "'\n";
	} else {
		std::cerr << "FAILURE: char2octal did not report the correct result string for '" << expected_result << "'" << std::endl;
	}
	return bContinue;
}

bool test_char2hex(unsigned char inchar, std::string expected_result)
{
	std::string result = char2hex(inchar);
	bool bContinue = result == expected_result;
	if (bContinue) {
		std::cout << "SUCCESS: char2hex reported the correct result string for '" << expected_result << "'\n";
	} else {
		std::cerr << "FAILURE: char2hex did not report the correct result string for '" << expected_result << "'" << std::endl;
	}
	return bContinue;
}

bool test_char2upperhex(unsigned char inchar, std::string expected_result)
{
	std::string result = char2upperhex(inchar);
	bool bContinue = result == expected_result;
	if (bContinue) {
		std::cout << "SUCCESS: char2upperhex reported the correct result string for '" << expected_result << "'\n";
	} else {
		std::cerr << "FAILURE: char2upperhex did not report the correct result string for '" << expected_result << "'" << std::endl;
	}
	return bContinue;
}

bool test_split_string(std::string instring, std::string split_chars, const std::list<std::string>& test_list)
{
	std::list<std::string> test_result;
	split_string(instring, split_chars, test_result);
	bool bResult = test_result == test_list;
	if (!bResult) {
		std::cerr << "FAILURE: test_split_string did not return the correct result on instring='" << instring << "', split_chars = '" << split_chars << "'" << std::endl;
		std::cerr << "It returned:" << std::endl;
		std::list<std::string>::const_iterator ci = test_result.begin();
		while (ci != test_result.end()) {
			std::cerr << "..... : '" << *ci << "'" << std::endl;
			++ci;
		}
	}
	return bResult;
}

bool test_split_string_on_string(std::string instring, std::string split_string, const std::list<std::string>& test_list)
{
	std::list<std::string> test_result;
	split_string_on_string(instring, split_string, test_result);
	bool bResult = test_result == test_list;
	if (!bResult) {
		std::cerr << "FAILURE: test_split_string_on_string did not return the correct result on instring='" << instring << "', split_string = '" << split_string << "'" << std::endl;
		std::cerr << "It returned:" << std::endl;
		std::list<std::string>::const_iterator ci = test_result.begin();
		while (ci != test_result.end()) {
			std::cerr << "..... : '" << *ci << "'" << std::endl;
			++ci;
		}
	}
	return bResult;
}

bool test_replace_substring(std::string instring, 
			    std::string substring_to_replace,
			    std::string replacement_string,
			    std::string correct_result)
{
	std::string strResult = replace_substring(instring, 
						   substring_to_replace,
						   replacement_string);

	bool bResult = strResult == correct_result;

	if (bResult) {
		std::cout << "SUCCESS: replace_substring works on the correct string '" << correct_result << "'\n";
	} else {
		std::cerr << "FAILURE: replace_substring returned '" << strResult << "', not '" << correct_result << "'" << std::endl;
	}
	return bResult;
}

bool test_str_tolower(std::string instring, 
		      std::string correct_result)
{
	std::string strResult;
	str_tolower(instring, strResult);

	bool bResult = strResult == correct_result;

	if (bResult) {
		std::cout << "SUCCESS: str_tolower works on the correct string '" << correct_result << "'\n";
	} else {
		std::cerr << "FAILURE: str_tolower returned '" << strResult << "', not '" << correct_result << "'" << std::endl;
	}
	return bResult;
}

bool test_str_toupper(std::string instring, 
		      std::string correct_result)
{
	std::string strResult;
	str_toupper(instring, strResult);

	bool bResult = strResult == correct_result;

	if (bResult) {
		std::cout << "SUCCESS: str_toupper works on the correct string '" << correct_result << "'\n";
	} else {
		std::cerr << "FAILURE: str_toupper returned '" << strResult << "', not '" << correct_result << "'" << std::endl;
	}
	return bResult;
}

bool test_MiniDOM_parse(std::string instring, std::string expectedresult)
{
	MiniDOMDocument *pDoc = 0;
	try {
		pDoc = MiniDOMParseString(instring);
	} catch (QDException& e) {
		std::cerr << "FAILURE: test_MiniDOM_parse: Calling MiniDOMParseString threw a QDException: '" << e.what() << "'" << std::endl;
		return false;
	}

	if (pDoc == 0) {
		std::cerr << "FAILURE: test_MiniDOM_parse returned an empty document." << std::endl;
		return false;
	} else {
		std::ostringstream ostr;
		try {
			pDoc->pretty(ostr);
		} catch (QDException& e) {
			delete pDoc;
			std::cerr << "FAILURE: test_MiniDOM_parse: Calling pDoc->pretty() threw a QDException: '" << e.what() << "'" << std::endl;
			return false;
		}
		std::string result = ostr.str();
		bool bContinue = result == expectedresult;
		if (bContinue) {
			std::cout << "SUCCESS: MiniDOMParseString returned the correct document.\n";
		} else {
			std::cerr << "FAILURE: MiniDOMParseString did not return the correct document on the string:\n---------------\n" << instring << "\n---------------\n, expecting:\n---------------\n" << expectedresult << "\n---------------\n,instead returning \n---------------\n" << result << "\n---------------\n" << std::endl;
		}
		delete pDoc;
		return bContinue;
	}
}



int test_backend(eBackendKind backend_kind, 
		 const std::string& hostname, 
		 const std::string& user, 
		 const std::string& password)
{
	std::cout << "----------------------------------\n"
		  << "Testing backend " << backend_kind2string(backend_kind) << '\n'
		  << "----------------------------------" << std::endl;

	EMdFDB *pDB = 0;
	std::string newdb;
	int result = 0;
	bool bContinue = true;
	bool bAllOK = true;
	newdb = "emdf1";

	// Make connection
	if (backend_kind == kPostgreSQL) {
#if USE_POSTGRESQL
		pDB = new PgEMdFDB(hostname, user, password, "emdf");
#else
		throw EmdrosException(std::string("PostgreSQL support not compiled in. Cannot make a PostgreSQL connection."));
#endif

	} else if (backend_kind == kMySQL) {
#if USE_MYSQL
		pDB = new MySQLEMdFDB(hostname, user, password, "emdf");
#else
		throw EmdrosException(std::string("MySQL support not compiled in. Cannot make a MySQL connection."));
#endif
	} else if (backend_kind == kSQLite2) {
		throw EmdrosException(std::string("SQLite 2.X.X no longer supported."));
	} else if (backend_kind == kSQLite3) {
#if USE_SQLITE3
		pDB = new SQLite3EMdFDB("emdf", password);
#else
		throw EmdrosException(std::string("SQLite 3.X.X support not compiled in. Cannot make a SQLite 3.X.X connection."));
#endif
	} else {
		ASSERT_THROW(false, "Unknown backend");
	}

#if !(USE_POSTGRESQL) && !(USE_MYSQL)
	UNUSED(hostname);
	UNUSED(user);
#endif


	// Test connection
	if (!pDB->connectionOk()) {
		DB_ERROR_MESSAGE;

		emdrosMessageConnectionNotOK(&std::cerr, backend_kind);

		delete pDB;
		return 1;
	}

	// Test EMdFValue
	if (bContinue) {
		EMdFValue toBeAssigned;
		EMdFValue evString(std::string("String"));
		EMdFValue evInt1(kEVInt, (long) 1);
		EMdFValue evIntMinus1(kEVInt, (long) -1);
		EMdFValue evID_DNIL(kEVID_D, (long) NIL);
		EMdFValue evID_D(kEVID_D, (long) 1);
		EMdFValue evEnum1(kEVEnum, (long) 1);
		EMdFValue evEnumMinus1(kEVEnum, (long) -1);
		IntegerList *pILInt = new IntegerList();
		pILInt->addValueBack(1);
		pILInt->addValueBack(2);
		EMdFValue evListOfInteger(kEVListOfInteger, pILInt);	
		IntegerList *pILID_D = new IntegerList();
		pILID_D->addValueBack(NIL);
		pILID_D->addValueBack(1);
		EMdFValue evListOfID_D(kEVListOfID_D, pILID_D);
		toBeAssigned = evEnum1;
		
		if (bContinue && evString.getString() != "String") {
			bContinue = false;
			std::cerr << "FAILURE: evString didn't contain what it should." << std::endl;
		}
		try {
			evID_D.getString();
			bContinue = false;
			std::cerr << "FAILURE: evID_D.getString() didn't throw an exception, which it should!" << std::endl;
		} catch (EmdrosException& e) {
			UNUSED(e);
			// This is what it should do.
		}
		if (bContinue && evInt1.getInt() != 1) {
			bContinue = false;
			std::cerr << "FAILURE: evInt1 didn't contain what it should." << std::endl;
		}
		if (bContinue && evIntMinus1.getInt() != -1) {
			bContinue = false;
			std::cerr << "FAILURE: evIntMinus1 didn't contain what it should." << std::endl;
		}
		if (bContinue && evID_DNIL.getID_D() != NIL) {
			bContinue = false;
			std::cerr << "FAILURE: evID_DNIL didn't contain what it should." << std::endl;
		}
		if (bContinue && evID_D.getID_D() != 1) {
			bContinue = false;
			std::cerr << "FAILURE: evID_D didn't contain what it should." << std::endl;
		}
		if (bContinue && evEnum1.getEnum() != 1) {
			bContinue = false;
			std::cerr << "FAILURE: evEnum1 didn't contain what it should." << std::endl;
		}
		if (bContinue && evEnumMinus1.getEnum() != -1) {
			bContinue = false;
			std::cerr << "FAILURE: evEnumMinus1 didn't contain what it should." << std::endl;
		}
		if (bContinue && evListOfInteger.getIntegerList()->getDelimitedString(",") != ",1,2,") {
			bContinue = false;
			std::cerr << "FAILURE: evListOfInteger didn't contain what it should." << std::endl;
			std::cerr << "         Instead, it returned '" << evListOfInteger.getIntegerList()->getDelimitedString(",") << "'." << std::endl;
		}
		if (bContinue && evListOfID_D.getIntegerList()->getDelimitedString(",") != ",0,1,") {
			bContinue = false;
			std::cerr << "FAILURE: evListOfID_D didn't contain what it should." << std::endl;
		}

		

		if (bContinue) {
			std::cout << "SUCCESS: All tested EMdFValue methods passed.\n";
		}
	}

	// Test Table
	if (bContinue) {
		Table mytable;
		mytable.startNewRow();
		mytable.append("String11");
		mytable.append("String12");

		TableIterator myiterator;
		myiterator = mytable.iterator();
		if (mytable.getColumn(myiterator, 1) != "String11") {
			bContinue = false;
			std::cerr << "FAILURE: Getting row 1, column 1 of the table failed." << std::endl;
		}

		if (bContinue && mytable.getColumn(myiterator, 2) != "String12") {
			bContinue = false;
			std::cerr << "FAILURE: Getting row 1, column 2 of the table failed." << std::endl;
		}
		try {
			mytable.getColumn(myiterator, 3);
			bContinue = false;
			std::cerr << "FAILURE: Getting row 1, column 3 of the table did NOT fail, as it should!" << std::endl;
		} catch (TableColumnException& e) {
			UNUSED(e);
			// This is what it should do!
		}

		try {
			mytable.getColumn(myiterator, 0);
			bContinue = false;
			std::cerr << "FAILURE: Getting row 1, column 0 of the table did NOT fail, as it should!" << std::endl;
		} catch (TableColumnException& e) {
			UNUSED(e);
			// This is what it should do!
		}

		mytable.newline();
		mytable.append("String21");
		mytable.append("String22");

		myiterator = mytable.find("String22", 2, false);
		if (myiterator.getColumn(2) != "String22") {
			bContinue = false;
			std::cerr << "FAILURE: finding row 2, column 2 failed." << std::endl;
		}
		myiterator = mytable.find("String21", 1, false);
		if (myiterator.getColumn(1) != "String21") {
			bContinue = false;
			std::cerr << "FAILURE: finding row 2, column 1 failed." << std::endl;
		}
		myiterator = mytable.find("String12", 2, false);
		if (myiterator.getColumn(2) != "String12") {
			bContinue = false;
			std::cerr << "FAILURE: finding row 1, column 2 failed." << std::endl;
		}
		myiterator = mytable.find("String11", 1, false);
		if (myiterator.getColumn(1) != "String11") {
			bContinue = false;
			std::cerr << "FAILURE: finding row 1, column 1 failed." << std::endl;
		}
		myiterator = mytable.find("DoesNotExist", 1, false);
		if (myiterator.hasNext()) {
			bContinue = false;
			std::cerr << "FAILURE: finding non-existent string found something, which it shouldn't." << std::endl;
		}

		if (bContinue) {
			std::cout << "SUCCESS: All tested Table methods passed.\n";
		}
	}

	// Test string_func.cpp
	if (bContinue) {
		long valueLong;
		monad_m valueMonad_m;
		id_d_t valueID_D;
		valueLong = string2long("100");
		bContinue = bContinue && valueLong == 100;
		if (!bContinue) {
			std::cerr << "FAILURE: string2long failed." << std::endl;
		}
		valueMonad_m = string2monad_m("100");
		bContinue = bContinue && valueMonad_m == 100;
		if (!bContinue) {
			std::cerr << "FAILURE: string2monad_m failed." << std::endl;
		}
		valueID_D = string2id_d(NIL_AS_VISIBLE_STRING);
		bContinue = bContinue && valueID_D == NIL;
		if (!bContinue) {
			std::cerr << "FAILURE: string2id_d failed on NIL." << std::endl;
		}
		valueID_D = string2id_d("10000");
		bContinue = bContinue && valueID_D == 10000;
		if (!bContinue) {
			std::cerr << "FAILURE: string2id_d failed on non-NIL id_d." << std::endl;
		}

		std::string strValue;
		strValue = monad_m2string(1);
		bContinue = bContinue && strValue == "1";
		if (!bContinue) {
			std::cerr << "FAILURE: monad_m2string." << std::endl;
		}
		strValue = id_d2string(NIL);
		bContinue = bContinue && strValue == NIL_AS_VISIBLE_STRING;
		if (!bContinue) {
			std::cerr << "FAILURE: id_d2string failed on NIL." << std::endl;
		}
		strValue = id_d2string(1);
		bContinue = bContinue && strValue == "1";
		if (!bContinue) {
			std::cerr << "FAILURE: id_d2string failed on non-NIL id_d." << std::endl;
		}
		strValue = long2string(0L);
		bContinue = bContinue && strValue == "0";
		if (!bContinue) {
			std::cerr << "FAILURE: long2string." << std::endl;
		}
		strValue = long2string(9L);
		bContinue = bContinue && strValue == "9";
		if (!bContinue) {
			std::cerr << "FAILURE: long2string." << std::endl;
		}
		strValue = long2string(1L);
		bContinue = bContinue && strValue == "1";
		if (!bContinue) {
			std::cerr << "FAILURE: long2string." << std::endl;
		}
		strValue = long2string(10L);
		bContinue = bContinue && strValue == "10";
		if (!bContinue) {
			std::cerr << "FAILURE: long2string." << std::endl;
		}
		strValue = long2string(13L);
		bContinue = bContinue && strValue == "13";
		if (!bContinue) {
			std::cerr << "FAILURE: long2string." << std::endl;
		}
		strValue = long2string(137L);
		bContinue = bContinue && strValue == "137";
		if (!bContinue) {
			std::cerr << "FAILURE: long2string." << std::endl;
		}
		strValue = long2string(-1334L);
		bContinue = bContinue && strValue == "-1334";
		if (!bContinue) {
			std::cerr << "FAILURE: long2string." << std::endl;
		}
		strValue = long2string(1234567890L);
		bContinue = bContinue && strValue == "1234567890";
		if (!bContinue) {
			std::cerr << "FAILURE: long2string." << std::endl;
		}
		strValue = long2string(MAX_MONAD);
		bContinue = bContinue && strValue == "2100000000";
		if (!bContinue) {
			std::cerr << "FAILURE: long2string." << std::endl;
		}
		// strcmp_nocase
		valueLong = strcmp_nocase("WxQt", "WXQt");
		bContinue = bContinue && valueLong == 0;
		if (!bContinue) {
			std::cerr << "FAILURE: strcmp_nocase(\"WxQt\", \"WXQt\")." << std::endl;
		}
		valueLong = strcmp_nocase("WxQtÅl", "WXQtÅl");
		bContinue = bContinue && valueLong == 0;
		if (!bContinue) {
			std::cerr << "FAILURE: strcmp_nocase(\"WxQtÅl\", \"WXQtÅl\")." << std::endl;
		}
		valueLong = strcmp_nocase("WxYq", "WXQt");
		bContinue = bContinue && valueLong == 1;
		if (!bContinue) {
			std::cerr << "FAILURE: strcmp_nocase(\"WxYq\", \"WXQt\")." << std::endl;
		}
		valueLong = strcmp_nocase("WxQt", "WXYq");
		bContinue = bContinue && valueLong == -1;
		if (!bContinue) {
			std::cerr << "FAILURE: strcmp_nocase(\"WxQt\", \"WXYq\")." << std::endl;
		}
		// strcmp_case
		valueLong = strcmp_case("WxQt", "WXQt");
		bContinue = bContinue && valueLong == 1;
		if (!bContinue) {
			std::cerr << "FAILURE: strcmp_case(\"WxQt\", \"WXQt\")." << std::endl;
		}
		valueLong = strcmp_case("WxQt", "WxQt");
		bContinue = bContinue && valueLong == 0;
		if (!bContinue) {
			std::cerr << "FAILURE: strcmp_case(\"WxQt\", \"WxQt\")." << std::endl;
		}
		valueLong = strcmp_case("WXYq", "WXQt");
		bContinue = bContinue && valueLong == 1;
		if (!bContinue) {
			std::cerr << "FAILURE: strcmp_case(\"WXYq\", \"WXQt\")." << std::endl;
		}
		valueLong = strcmp_case("WXQt", "WXYq");
		bContinue = bContinue && valueLong == -1;
		if (!bContinue) {
			std::cerr << "FAILURE: strcmp_case(\"WXQt\", \"WXYq\")." << std::endl;
		}
		bContinue = bContinue && is_identifier("aBc09");
		if (!bContinue) {
			std::cerr << "FAILURE: is_identifier(\"aBc09\") should have returned true." << std::endl;
		}
		bContinue = bContinue && !is_identifier("091a9");
		if (!bContinue) {
			std::cerr << "FAILURE: is_identifier(\"091a9\") should have returned false." << std::endl;
		}
		bContinue = bContinue && !is_identifier("");
		if (!bContinue) {
			std::cerr << "FAILURE: is_identifier(\"\") should have returned false." << std::endl;
		}
		bContinue = bContinue && !is_identifier("_a{");
		if (!bContinue) {
			std::cerr << "FAILURE: is_identifier(\"_a{\") should have returned false." << std::endl;
		}

		std::list<std::string> test_list;
		test_list.push_back("");
		bContinue = bContinue && test_split_string("", " \t", test_list);
		bContinue = bContinue && test_split_string(" ", " \t", test_list);
		bContinue = bContinue && test_split_string(" \t", " \t", test_list);
		bContinue = bContinue && test_split_string(" \t ", " \t", test_list);
		test_list.pop_back();

		test_list.push_back("abc");
		bContinue = bContinue && test_split_string("abc", " \t", test_list);
		test_list.push_back("");
		bContinue = bContinue && test_split_string("abc ", " \t", test_list);
		bContinue = bContinue && test_split_string("abc \t", " \t", test_list);
		test_list.push_front("");
		bContinue = bContinue && test_split_string("\tabc \t", " \t", test_list);
		bContinue = bContinue && test_split_string("\t abc \t", " \t", test_list);
		test_list.pop_back();
		bContinue = bContinue && test_split_string("\t abc", " \t", test_list);

		test_list.push_back("def");
		bContinue = bContinue && test_split_string("\t abc\tdef", " \t", test_list);
		test_list.push_back("");
		bContinue = bContinue && test_split_string("\t abc\tdef\t", " \t", test_list);
		test_list.pop_front();
		bContinue = bContinue && test_split_string("abc\tdef\t", " \t", test_list);
		test_list.pop_back();
		bContinue = bContinue && test_split_string("abc\tdef", " \t", test_list);

		test_list.push_front("");
		test_list.push_back("ghi");
		bContinue = bContinue && test_split_string("\t abc\tdef ghi", " \t", test_list);
		test_list.push_back("");
		bContinue = bContinue && test_split_string("\t abc\tdef\tghi\t", " \t", test_list);

		bContinue = bContinue && test_split_string("\t abc\tdef ghi ", " \t", test_list);
		test_list.pop_back();
		bContinue = bContinue && test_split_string("\t abc\tdef ghi", " \t", test_list);


		std::list<std::string> test_list2;
		test_list2.push_back("");
		bContinue = bContinue && test_split_string_on_string("", " \t", test_list2);
		bContinue = bContinue && test_split_string_on_string("", "\t ", test_list2);
		test_list2.pop_back();
		test_list2.push_back("aa");
		bContinue = bContinue && test_split_string_on_string("aa", " \t", test_list2);
		test_list2.pop_back();
		test_list2.push_back("abc");
		bContinue = bContinue && test_split_string_on_string("abc ", " ", test_list2);
		bContinue = bContinue && test_split_string_on_string("abc \t", " \t", test_list2);
		bContinue = bContinue && test_split_string_on_string("\tabc\t", "\t", test_list2);
		bContinue = bContinue && test_split_string_on_string("\t abc\t ", "\t ", test_list2);
		bContinue = bContinue && test_split_string_on_string("\t abc", "\t ", test_list2);

		test_list2.push_back("def");
		bContinue = bContinue && test_split_string_on_string("abc\n\ndef", "\n\n", test_list2);
		bContinue = bContinue && test_split_string_on_string("\tabc\tdef", "\t", test_list2);
		bContinue = bContinue && test_split_string_on_string("\t abc\t def\t ", "\t ", test_list2);
		bContinue = bContinue && test_split_string_on_string("abc\tdef\t", "\t", test_list2);
		bContinue = bContinue && test_split_string_on_string("abc\tdef", "\t", test_list2);
    
		test_list2.push_back("ghi");
		bContinue = bContinue && test_split_string_on_string("\t abc\t def\t ghi", "\t ", test_list2);
		bContinue = bContinue && test_split_string_on_string("\t abc\t def\t ghi\t ", "\t ", test_list2);

		std::list<std::string> test_list3;
		test_list3.push_back("abc ghi jkl\n(1926)");
		test_list3.push_back("blah");
		bContinue = bContinue && test_split_string_on_string("abc ghi jkl\n(1926)\n\nblah", "\n\n", test_list3);
		bContinue = bContinue && test_str_tolower(" Ånd, " , " Ånd, ");
		bContinue = bContinue && test_str_tolower("ÅND, " , "Ånd, ");
		bContinue = bContinue && test_str_tolower("ÅnD, " , "Ånd, ");
		bContinue = bContinue && test_str_tolower(" ANdeKRÆ" , " andekrÆ");
		bContinue = bContinue && test_str_toupper(" ånd, " , " åND, ");
		bContinue = bContinue && test_str_toupper("ÅND, " , "ÅND, ");
		bContinue = bContinue && test_str_toupper("ÅnDsæg, " , "ÅNDSæG, ");
		bContinue = bContinue && test_str_toupper(" ANdeKRæ" , " ANDEKRæ");
		bContinue = bContinue && test_replace_substring(" Ånd, ", ", ", "", " Ånd");
		bContinue = bContinue && test_replace_substring(" this is an icecream.", " ", "", "thisisanicecream.");
		bContinue = bContinue && test_replace_substring(" this is an icecream.", " i", "", " thiss ancecream.");
		bContinue = bContinue && test_replace_substring(" this is an icecream.", " i", "i ", " thisi s ani cecream.");
		bContinue = bContinue && test_replace_substring(" %%SPECIAL%% %%SPECIAL1%% ", "%%SPECIAL%% ", "substring_replacement is ", " substring_replacement is %%SPECIAL1%% ");
		bContinue = bContinue && test_replace_substring(" %%SPECIAL%% %%SPECIAL%% ", "%%SPECIAL%% ", "substring_replacement is ", " substring_replacement is substring_replacement is ");
		bContinue = bContinue && test_replace_substring(" a b c d e f  g ", " ", "", "abcdefg");
		bContinue = bContinue && test_replace_substring("abc", "abc", "cba", "cba");
		bContinue = bContinue && test_replace_substring("bcaabc", "abc", "cba", "bcacba");
		bContinue = bContinue && test_replace_substring("bca abc", "abc", "cba", "bca cba");





		bContinue = bContinue && test_is_hex("0123456789ABCDEFabcdef", true);
		bContinue = bContinue && test_is_hex("01dg23456789ABCDEFabcdef", false);
		bContinue = bContinue && test_is_hex("", false);
		bContinue = bContinue && test_is_hex("00", true);
		bContinue = bContinue && test_is_hex("aF", true);

		bContinue = bContinue && test_hex2char("ff", '\xff');
		bContinue = bContinue && test_hex2char("ad", '\xad');
		bContinue = bContinue && test_hex2char("00", '\x00');
		bContinue = bContinue && test_hex2char("03", '\x03');
		bContinue = bContinue && test_hex2char("20", '\x20');
		bContinue = bContinue && test_hex2char("A8", '\xA8');
		bContinue = bContinue && test_hex2char("FF", '\xFF');
		bContinue = bContinue && test_hex2char("C9", '\xC9');
		
		bContinue = bContinue && test_char2hex('\xff', "ff");
		bContinue = bContinue && test_char2hex('\xad', "ad");
		bContinue = bContinue && test_char2hex('\x00', "00");
		bContinue = bContinue && test_char2hex('\x03', "03");
		bContinue = bContinue && test_char2hex('\x20', "20");
		bContinue = bContinue && test_char2hex('\xA8', "a8");
		bContinue = bContinue && test_char2hex('\xFF', "ff");
		bContinue = bContinue && test_char2hex('\xC9', "c9");
		
		bContinue = bContinue && test_char2upperhex('\xff', "FF");
		bContinue = bContinue && test_char2upperhex('\xad', "AD");
		bContinue = bContinue && test_char2upperhex('\x00', "00");
		bContinue = bContinue && test_char2upperhex('\x03', "03");
		bContinue = bContinue && test_char2upperhex('\x20', "20");
		bContinue = bContinue && test_char2upperhex('\xA8', "A8");
		bContinue = bContinue && test_char2upperhex('\xFF', "FF");
		bContinue = bContinue && test_char2upperhex('\xC9', "C9");

		bContinue = bContinue && test_char2octal('\377', "377");
		bContinue = bContinue && test_char2octal('\310', "310");
		bContinue = bContinue && test_char2octal('\300', "300");
		bContinue = bContinue && test_char2octal('\200', "200");
		bContinue = bContinue && test_char2octal('\100', "100");
		bContinue = bContinue && test_char2octal('\000', "000");
		bContinue = bContinue && test_char2octal('\241', "241");
		bContinue = bContinue && test_char2octal('\203', "203");
		bContinue = bContinue && test_char2octal('\237', "237");

		bContinue = bContinue && test_octal2char("377", '\377');
		bContinue = bContinue && test_octal2char("310", '\310');
		bContinue = bContinue && test_octal2char("300", '\300');
		bContinue = bContinue && test_octal2char("200", '\200');
		bContinue = bContinue && test_octal2char("100", '\100');
		bContinue = bContinue && test_octal2char("000", '\000');
		bContinue = bContinue && test_octal2char("241", '\241');
		bContinue = bContinue && test_octal2char("203", '\203');
		bContinue = bContinue && test_octal2char("237", '\237');

		
		bContinue = bContinue && test_parse_C_backslashes("\\nadf", "\nadf");
		bContinue = bContinue && test_parse_C_backslashes("adf\\n", "adf\n");
		bContinue = bContinue && test_parse_C_backslashes("adf\\", "adf\\");
		bContinue = bContinue && test_parse_C_backslashes("adf\\\\", "adf\\");
		bContinue = bContinue && test_parse_C_backslashes("ad\\xaf\\n", "ad\xaf\n");
		bContinue = bContinue && test_parse_C_backslashes("adf\\040n", "adf\040n");

		bContinue = bContinue && test_strip(" a b c ", " \t\n\v\r", "a b c");
		bContinue = bContinue && test_strip("a b c ", " \t\n\v\r", "a b c");
		bContinue = bContinue && test_strip(" a b c", " \t\n\v\r", "a b c");
		
		bContinue = bContinue && test_strip("  a b c  ", " \t\n\v\r", "a b c");
		bContinue = bContinue && test_strip("a b c  ", " \t\n\v\r", "a b c");
		bContinue = bContinue && test_strip("  a b c", " \t\n\v\r", "a b c");

		bContinue = bContinue && test_strip("abc", " \t\n\v\r", "abc");
		bContinue = bContinue && test_strip("abc", "cb", "a");
		bContinue = bContinue && test_strip("abc", "abc", "");
		
		bContinue = bContinue && test_strip_whitespace(" a b c ", "a b c");
		bContinue = bContinue && test_strip_whitespace("a b c ", "a b c");
		bContinue = bContinue && test_strip_whitespace(" a b c", "a b c");
		
		bContinue = bContinue && test_strip_whitespace("  a b c  ", "a b c");
		bContinue = bContinue && test_strip_whitespace("a b c  ", "a b c");
		bContinue = bContinue && test_strip_whitespace("  a b c", "a b c");
		bContinue = bContinue && test_strip_whitespace("  a b c    ", "a b c");

		bContinue = bContinue && test_strip_whitespace("abc", "abc");

		
		
		if (bContinue) {
			std::cout << "SUCCESS: all string_func functions pass.\n";
		} else {
			std::cerr << "FAILURE: At least one string_func functions failed." << std::endl;
			bAllOK = false;
		}
	}

	// Test monad set
	if (bContinue) {
		SetOfMonads som1(1), 
			som2(4,10), 
			som21(2,4), 
			som22(2,4), 
			som212, 
			som2222, 
			som22222,
			emptysom;
		som21.add(7);
		som22.add(7,10);
		som212.add(2,7);
		som212.add(10);
		som212.add(12,17);
		som2222.add(1000000, 1010000);
		som2222.add(2000000, 3000000);
		som2222.add(45000000, 46000000);
		som2222.add(70000000, 70000001);
		som22222.add(54000000, 54002000);
		som22222.add(55000000, 55004000);
		som22222.add(56000000, 56008000);
		som22222.add(57000000, 57016000);

		bContinue = bContinue && testSOMSelfEquality(emptysom);
		bContinue = bContinue && testSOMSelfEquality(som1);
		bContinue = bContinue && testSOMSelfEquality(som2);
		bContinue = bContinue && testSOMSelfEquality(som21);
		bContinue = bContinue && testSOMSelfEquality(som22);
		bContinue = bContinue && testSOMSelfEquality(som212);
		bContinue = bContinue && testSOMSelfEquality(som2222);
		bContinue = bContinue && testSOMSelfEquality(som22222);

		bContinue = bContinue && testSOMToFromCompactString(emptysom);
		bContinue = bContinue && testSOMToFromCompactString(som1);
		bContinue = bContinue && testSOMToFromCompactString(som2);
		bContinue = bContinue && testSOMToFromCompactString(som21);
		bContinue = bContinue && testSOMToFromCompactString(som22);
		bContinue = bContinue && testSOMToFromCompactString(som212);
		bContinue = bContinue && testSOMToFromCompactString(som2222);
		bContinue = bContinue && testSOMToFromCompactString(som22222);

		bContinue = bContinue && testSOMToFromCompactStringFM(emptysom);
		bContinue = bContinue && testSOMToFromCompactStringFM(som1);
		bContinue = bContinue && testSOMToFromCompactStringFM(som2);
		bContinue = bContinue && testSOMToFromCompactStringFM(som21);
		bContinue = bContinue && testSOMToFromCompactStringFM(som22);
		bContinue = bContinue && testSOMToFromCompactStringFM(som212);
		bContinue = bContinue && testSOMToFromCompactStringFM(som2222);
		bContinue = bContinue && testSOMToFromCompactStringFM(som22222);

		bContinue = bContinue && testSOMCompactStringEqualityWithAndWithoutFM(emptysom);
		bContinue = bContinue && testSOMCompactStringEqualityWithAndWithoutFM(som1);
		bContinue = bContinue && testSOMCompactStringEqualityWithAndWithoutFM(som2);
		bContinue = bContinue && testSOMCompactStringEqualityWithAndWithoutFM(som21);
		bContinue = bContinue && testSOMCompactStringEqualityWithAndWithoutFM(som22);
		bContinue = bContinue && testSOMCompactStringEqualityWithAndWithoutFM(som212);
		bContinue = bContinue && testSOMCompactStringEqualityWithAndWithoutFM(som2222);
		bContinue = bContinue && testSOMCompactStringEqualityWithAndWithoutFM(som22222);

		bContinue = bContinue && testSOMCompactStringEqualityWithFM(emptysom);
		bContinue = bContinue && testSOMCompactStringEqualityWithFM(som1);
		bContinue = bContinue && testSOMCompactStringEqualityWithFM(som2);
		bContinue = bContinue && testSOMCompactStringEqualityWithFM(som21);
		bContinue = bContinue && testSOMCompactStringEqualityWithFM(som22);
		bContinue = bContinue && testSOMCompactStringEqualityWithFM(som212);
		bContinue = bContinue && testSOMCompactStringEqualityWithFM(som2222);
		bContinue = bContinue && testSOMCompactStringEqualityWithFM(som22222);
		

		bContinue = bContinue && som212.hasMonadsInRange(3,5);
		bContinue = bContinue && som212.hasMonadsInRange(7, 9);
		bContinue = bContinue && som212.hasMonadsInRange(10,10);
		bContinue = bContinue && !som212.hasMonadsInRange(8,8);
		bContinue = bContinue && !som212.hasMonadsInRange(11,11);
		bContinue = bContinue && !som212.hasMonadsInRange(20,20);

		bContinue = bContinue && testSOMPart_of(som21, som22, true);

		bContinue = bContinue && testSOMfromString(" { } ", " {  } ");
		bContinue = bContinue && testSOMfromString(" { 1 } ", " { 1 } ");
		bContinue = bContinue && testSOMfromString(" { 30 } ", " { 30 } ");
		bContinue = bContinue && testSOMfromString(" { 198 } ", " { 198 } ");
		bContinue = bContinue && testSOMfromString(" { 1-3 } ", " { 1-3 } ");
		bContinue = bContinue && testSOMfromString(" { 1-3 } ", " { 1-3 } ");
		bContinue = bContinue && testSOMfromString(" { 1-3, 8-10 } ", " { 1-3, 8-10 } ");
		bContinue = bContinue && testSOMfromString(" { 1-3, 8-10, 12 } ", " { 1-3, 8-10, 12 } ");
		bContinue = bContinue && testSOMfromString(" { 1-3,8-10,12,100000, 200000-300001 } ", " { 1-3, 8-10, 12, 100000, 200000-300001 } ");
		SetOfMonads fsom4;
		fsom4.add(129889,129940);
		fsom4.add(129987,129990);
		fsom4.add(130003,130004);
		SetOfMonads oldsom4;
		oldsom4.add(129887,129990);
		bContinue = bContinue && testSOMPart_of(oldsom4, fsom4, false);

		SetOfMonads oldsom5;
		oldsom5.add(5,6);
		oldsom5.add(10,15);
		bContinue = bContinue && testSOMfillInsertRange(oldsom5,
							       1, 1,
							       " { 6-7, 11-16 } ");
		bContinue = bContinue && testSOMfillInsertRange(oldsom5,
							       20,25,
							       " { 5-6, 10-15 } ");
		bContinue = bContinue && testSOMfillInsertRange(oldsom5,
							       4,6,
							       " { 4-9, 13-18 } ");
		bContinue = bContinue && testSOMfillInsertRange(oldsom5,
							       8,9,
							       " { 5-6, 8-9, 12-17 } ");
		bContinue = bContinue && testSOMfillInsertRange(oldsom5,
							       7,9,
							       " { 5-9, 13-18 } ");
		bContinue = bContinue && testSOMfillInsertRange(oldsom5,
							       8,10,
							       " { 5-6, 8-18 } ");
		bContinue = bContinue && testSOMfillInsertRange(oldsom5,
							       9,11,
							       " { 5-6, 9-18 } ");
		bContinue = bContinue && testSOMfillInsertRange(oldsom5,
							       10,12,
							       " { 5-6, 10-18 } ");
		bContinue = bContinue && testSOMfillInsertRange(oldsom5,
							       14,17,
							       " { 5-6, 10-19 } ");
		bContinue = bContinue && testSOMfillInsertRange(oldsom5,
							       8,8,
							       " { 5-6, 8, 11-16 } ");

                SetOfMonads diffSom1_3_5_8_10_13(1,3);
                diffSom1_3_5_8_10_13.add(5,8);
                diffSom1_3_5_8_10_13.add(10,13);

                SetOfMonads diffSom5_8_10_13(5,8);
                diffSom5_8_10_13.add(10,13);

                SetOfMonads diffSom1_3_5_8(1,3);
                diffSom1_3_5_8.add(5,8);

                SetOfMonads diffSom1_3_5_8_10_11(1,3);
                diffSom1_3_5_8_10_11.add(5,8);
                diffSom1_3_5_8_10_11.add(10,11);

                SetOfMonads diffSom1_3_10_13(1,3);
                diffSom1_3_10_13.add(10,13);

                SetOfMonads diffSom2_3_5_8_10_13(2,3);
                diffSom2_3_5_8_10_13.add(5,8);
                diffSom2_3_5_8_10_13.add(10,13);

                SetOfMonads diffSom1_3_6_7_10_13(1,3);
                diffSom1_3_6_7_10_13.add(6,7);
                diffSom1_3_6_7_10_13.add(10,13);


                SetOfMonads diffSom = diffSom1_3_5_8_10_13;
                diffSom.difference(SetOfMonads(1,4));
                bContinue = bContinue && testSOMSOMEquality(diffSom5_8_10_13, diffSom);

                diffSom = diffSom1_3_5_8_10_13;
                diffSom.difference(SetOfMonads(1,3));
                bContinue = bContinue && testSOMSOMEquality(diffSom5_8_10_13, diffSom);

                diffSom = diffSom1_3_5_8_10_13;
                diffSom.difference(SetOfMonads(10,13));
                bContinue = bContinue && testSOMSOMEquality(diffSom1_3_5_8, diffSom);

                diffSom = diffSom1_3_5_8_10_13;
                diffSom.difference(SetOfMonads(10,16));
                bContinue = bContinue && testSOMSOMEquality(diffSom1_3_5_8, diffSom);

                diffSom = diffSom1_3_5_8_10_13;
                diffSom.difference(SetOfMonads(9,13));
                bContinue = bContinue && testSOMSOMEquality(diffSom1_3_5_8, diffSom);

                diffSom = diffSom1_3_5_8_10_13;
                diffSom.difference(SetOfMonads(9,16));
                bContinue = bContinue && testSOMSOMEquality(diffSom1_3_5_8, diffSom);

                diffSom = diffSom1_3_5_8_10_13;
                diffSom.difference(SetOfMonads(12,13));
                bContinue = bContinue && testSOMSOMEquality(diffSom1_3_5_8_10_11, diffSom);

                diffSom = diffSom1_3_5_8_10_13;
                diffSom.difference(SetOfMonads(12,16));
                bContinue = bContinue && testSOMSOMEquality(diffSom1_3_5_8_10_11, diffSom);

                diffSom = diffSom1_3_5_8_10_13;
                diffSom.difference(SetOfMonads(5,8));
                bContinue = bContinue && testSOMSOMEquality(diffSom1_3_10_13, diffSom);

                diffSom = diffSom1_3_5_8_10_13;
                diffSom.difference(SetOfMonads(4,8));
                bContinue = bContinue && testSOMSOMEquality(diffSom1_3_10_13, diffSom);

                diffSom = diffSom1_3_5_8_10_13;
                diffSom.difference(SetOfMonads(5,9));
                bContinue = bContinue && testSOMSOMEquality(diffSom1_3_10_13, diffSom);

                diffSom = diffSom1_3_5_8_10_13;
                diffSom.difference(SetOfMonads(4,9));
                bContinue = bContinue && testSOMSOMEquality(diffSom1_3_10_13, diffSom);

                diffSom = diffSom1_3_5_8_10_13;
                diffSom.difference(SetOfMonads(1,1));
                bContinue = bContinue && testSOMSOMEquality(diffSom2_3_5_8_10_13, diffSom);

                diffSom = diffSom1_3_5_8_10_13;
                SetOfMonads diffSom2(5,5);
                diffSom2.add(8,8);
                diffSom.difference(diffSom2);
                bContinue = bContinue && testSOMSOMEquality(diffSom1_3_6_7_10_13, diffSom);

		SetOfMonads diffSom3(160,189);
		SetOfMonads diffSom4(160,189);
		SetOfMonads emptySOM;

		diffSom3.difference(diffSom4);
                bContinue = bContinue && testSOMSOMEquality(diffSom3, emptySOM);
		

		if (bContinue) {
			std::cout << "SUCCESS: all tested SetOfMonads methods pass.\n";
		} else {
			std::cerr << "FAILURE: At least one SetOfMonads method failed." << std::endl;
			bAllOK = false;
		}
	}
  

	// Test fast monad set
	if (bContinue) {
		FastSetOfMonads som1(1), 
			som2(4,10), 
			som21(2,4), 
			som22(2,4), 
			som212, 
			som2222, 
			som22222,
			emptysom;
		som21.add(7);
		som22.add(7,10);
		som212.add(2,7);
		som212.add(10);
		som212.add(12,17);
		som2222.add(1000000, 1010000);
		som2222.add(2000000, 3000000);
		som2222.add(45000000, 46000000);
		som2222.add(70000000, 70000001);
		som22222.add(54000000, 54002000);
		som22222.add(55000000, 55004000);
		som22222.add(56000000, 56008000);
		som22222.add(57000000, 57016000);

		bContinue = bContinue && testSOMSelfEquality(emptysom);
		bContinue = bContinue && testSOMSelfEquality(som1);
		bContinue = bContinue && testSOMSelfEquality(som2);
		bContinue = bContinue && testSOMSelfEquality(som21);
		bContinue = bContinue && testSOMSelfEquality(som22);
		bContinue = bContinue && testSOMSelfEquality(som212);
		bContinue = bContinue && testSOMSelfEquality(som2222);
		bContinue = bContinue && testSOMSelfEquality(som22222);

		bContinue = bContinue && testSOMHasMonadsInRange(som212, 3, 5, true);
		bContinue = bContinue && testSOMHasMonadsInRange(som212, 7, 9, true);
		bContinue = bContinue && testSOMHasMonadsInRange(som212, 10, 10, true);
		bContinue = bContinue && testSOMHasMonadsInRange(som212, 8, 8, false);
		bContinue = bContinue && testSOMHasMonadsInRange(som212, 11, 11, false);
		bContinue = bContinue && testSOMHasMonadsInRange(som212, 20, 20, false);
		bContinue = bContinue && testSOMHasMonadsInRange(som2222, 999999, 1000000, true);
		bContinue = bContinue && testSOMHasMonadsInRange(som2222, 1999999, 2000000, true);
		bContinue = bContinue && testSOMHasMonadsInRange(som2222, 70000001, 70000002, true);
		bContinue = bContinue && testSOMHasMonadsInRange(som2222, 70000000, 70000000, true);

		FastSetOfMonads fsom1;
		fsom1.add(1);
		SetOfMonads oldsom1;
		oldsom1.add(1,3);
		bContinue = bContinue && testFastSOMPart_of(oldsom1, fsom1, false);
		fsom1.add(1,3);
		bContinue = bContinue && testFastSOMPart_of(oldsom1, fsom1, true);
		SetOfMonads oldsom2;
		oldsom2.add(1);
		bContinue = bContinue && testFastSOMPart_of(oldsom2, fsom1, true);
		fsom1.add(4);
		bContinue = bContinue && testFastSOMPart_of(oldsom1, fsom1, true);
		fsom1.add(6,8);
		bContinue = bContinue && testFastSOMPart_of(oldsom1, fsom1, true);
		oldsom1.add(4);
		bContinue = bContinue && testFastSOMPart_of(oldsom1, fsom1, true);
		oldsom1.add(5);
		bContinue = bContinue && testFastSOMPart_of(oldsom1, fsom1, false);
		oldsom1.removeMSE(MonadSetElement(5,5));
		bContinue = bContinue && testFastSOMPart_of(oldsom1, fsom1, true);
		oldsom1.removeMSE(MonadSetElement(1,2));
		bContinue = bContinue && testFastSOMPart_of(oldsom1, fsom1, true);
		oldsom1.add(6);
		bContinue = bContinue && testFastSOMPart_of(oldsom1, fsom1, true);
		oldsom1.add(7);
		bContinue = bContinue && testFastSOMPart_of(oldsom1, fsom1, true);
		oldsom1.add(8);
		bContinue = bContinue && testFastSOMPart_of(oldsom1, fsom1, true);
		oldsom1.removeMSE(MonadSetElement(6,6));
		bContinue = bContinue && testFastSOMPart_of(oldsom1, fsom1, true);
		oldsom1.removeMSE(MonadSetElement(7,7));
		bContinue = bContinue && testFastSOMPart_of(oldsom1, fsom1, true);
		oldsom1.add(9);
		bContinue = bContinue && testFastSOMPart_of(oldsom1, fsom1, false);
		oldsom1.removeMSE(MonadSetElement(9,9));
		bContinue = bContinue && testFastSOMPart_of(oldsom1, fsom1, true);
		fsom1.add(10,10);
		bContinue = bContinue && testFastSOMPart_of(oldsom1, fsom1, true);
		oldsom1.add(10,10);
		bContinue = bContinue && testFastSOMPart_of(oldsom1, fsom1, true);

		FastSetOfMonads fsom2;
		fsom2.add(3448,3451);
		fsom2.add(3457,3472);
		fsom2.add(3478,3481);

		bContinue = bContinue && testFastGapExists(fsom2, 3440, 0, false);
		bContinue = bContinue && testFastGapExists(fsom2, 3448, 0, false);
		bContinue = bContinue && testFastGapExists(fsom2, 3451, 0, false);
		bContinue = bContinue && testFastGapExists(fsom2, 3457, 0, false);
		bContinue = bContinue && testFastGapExists(fsom2, 3472, 0, false);
		bContinue = bContinue && testFastGapExists(fsom2, 3478, 0, false);
		bContinue = bContinue && testFastGapExists(fsom2, 3481, 0, false);
		bContinue = bContinue && testFastGapExists(fsom2, 3482, 0, false);

		bContinue = bContinue && testFastGapExists(fsom2, 3449, 0, false);
		bContinue = bContinue && testFastGapExists(fsom2, 3477, 0, false);
		bContinue = bContinue && testFastGapExists(fsom2, 3456, 0, false);

		bContinue = bContinue && testFastGapExists(fsom2, 3452, 3456, true);
		bContinue = bContinue && testFastGapExists(fsom2, 3473, 3477, true);

		bContinue = bContinue && testFastSOMRemoveMonadsBefore(fsom2, 34, " { 3448-3451, 3457-3472, 3478-3481 } ");
		bContinue = bContinue && testFastSOMRemoveMonadsBefore(fsom2, 3448, " { 3448-3451, 3457-3472, 3478-3481 } ");
		bContinue = bContinue && testFastSOMRemoveMonadsBefore(fsom2, 3449, " { 3449-3451, 3457-3472, 3478-3481 } ");
		bContinue = bContinue && testFastSOMRemoveMonadsBefore(fsom2, 3451, " { 3451, 3457-3472, 3478-3481 } ");
		bContinue = bContinue && testFastSOMRemoveMonadsBefore(fsom2, 3454, " { 3457-3472, 3478-3481 } ");
		bContinue = bContinue && testFastSOMRemoveMonadsBefore(fsom2, 3480, " { 3480-3481 } ");
		bContinue = bContinue && testFastSOMRemoveMonadsBefore(fsom2, 3481, " { 3481 } ");
		bContinue = bContinue && testFastSOMRemoveMonadsBefore(fsom2, 3482, " {  } ");

    
		FastSetOfMonads fsom3;
		fsom3.add(19151);
		fsom3.add(19159,19163);

		SetOfMonads oldsom3;
		oldsom3.add(19153);
		bContinue = bContinue && testFastSOMPart_of(oldsom3, fsom3, false);
		bContinue = bContinue && testFastSOMRemoveMonadsBefore(fsom3, 19152, " { 19159-19163 } ");
		bContinue = bContinue && testFastSOMPart_of(oldsom3, fsom3.removeMonadsBefore(19152), false);
    
		FastSetOfMonads fsom4;
		fsom4.add(129889,129940);
		fsom4.add(129987,129990);
		fsom4.add(130003,130004);
		fsom4.add(130027,130076);
		SetOfMonads oldsom4;
		oldsom4.add(129887,129990);
		bContinue = bContinue && testFastSOMPart_of(oldsom4, fsom4, false);

		oldsom4.clear();
		oldsom4.add(129987,129990);
		bContinue = bContinue && testFastSOMPart_of(oldsom4, fsom4, true);
		bContinue = bContinue && testSOMPart_of(SetOfMonads(129990), oldsom4, true);
		oldsom4.clear();
		oldsom4.add(130027,130029);
		bContinue = bContinue && testFastSOMPart_of(oldsom4, fsom4, true);

		oldsom4.clear();
		oldsom4.add(130044,130050);
		bContinue = bContinue && testFastSOMPart_of(oldsom4, fsom4, true);
    
		bContinue = bContinue && testFastSOMRemoveMonadsBefore(fsom4, 129991, " { 130003-130004, 130027-130076 } ");
		bContinue = bContinue && testSOMPart_of(SetOfMonads(130027,130029), fsom4.removeMonadsBefore(129991), true);

                FastSetOfMonads diffSom1_3_5_8_10_13(1,3);
                diffSom1_3_5_8_10_13.add(5,8);
                diffSom1_3_5_8_10_13.add(10,13);

                FastSetOfMonads diffSom5_8_10_13(5,8);
                diffSom5_8_10_13.add(10,13);

                FastSetOfMonads diffSom1_3_5_8(1,3);
                diffSom1_3_5_8.add(5,8);

                FastSetOfMonads diffSom1_3_5_8_10_11(1,3);
                diffSom1_3_5_8_10_11.add(5,8);
                diffSom1_3_5_8_10_11.add(10,11);

                FastSetOfMonads diffSom1_3_10_13(1,3);
                diffSom1_3_10_13.add(10,13);

                FastSetOfMonads diffSom2_3_5_8_10_13(2,3);
                diffSom2_3_5_8_10_13.add(5,8);
                diffSom2_3_5_8_10_13.add(10,13);

                FastSetOfMonads diffSom1_3_6_7_10_13(1,3);
                diffSom1_3_6_7_10_13.add(6,7);
                diffSom1_3_6_7_10_13.add(10,13);


                FastSetOfMonads diffSom = diffSom1_3_5_8_10_13;
                diffSom.difference(FastSetOfMonads(1,4));
                bContinue = bContinue && testSOMSOMEquality(diffSom5_8_10_13, diffSom);

                diffSom = diffSom1_3_5_8_10_13;
                diffSom.difference(FastSetOfMonads(1,3));
                bContinue = bContinue && testSOMSOMEquality(diffSom5_8_10_13, diffSom);

                diffSom = diffSom1_3_5_8_10_13;
                diffSom.difference(FastSetOfMonads(10,13));
                bContinue = bContinue && testSOMSOMEquality(diffSom1_3_5_8, diffSom);

                diffSom = diffSom1_3_5_8_10_13;
                diffSom.difference(FastSetOfMonads(10,16));
                bContinue = bContinue && testSOMSOMEquality(diffSom1_3_5_8, diffSom);

                diffSom = diffSom1_3_5_8_10_13;
                diffSom.difference(FastSetOfMonads(9,13));
                bContinue = bContinue && testSOMSOMEquality(diffSom1_3_5_8, diffSom);

                diffSom = diffSom1_3_5_8_10_13;
                diffSom.difference(FastSetOfMonads(9,16));
                bContinue = bContinue && testSOMSOMEquality(diffSom1_3_5_8, diffSom);

                diffSom = diffSom1_3_5_8_10_13;
                diffSom.difference(FastSetOfMonads(12,13));
                bContinue = bContinue && testSOMSOMEquality(diffSom1_3_5_8_10_11, diffSom);

                diffSom = diffSom1_3_5_8_10_13;
                diffSom.difference(FastSetOfMonads(12,16));
                bContinue = bContinue && testSOMSOMEquality(diffSom1_3_5_8_10_11, diffSom);

                diffSom = diffSom1_3_5_8_10_13;
                diffSom.difference(FastSetOfMonads(5,8));
                bContinue = bContinue && testSOMSOMEquality(diffSom1_3_10_13, diffSom);

                diffSom = diffSom1_3_5_8_10_13;
                diffSom.difference(FastSetOfMonads(4,8));
                bContinue = bContinue && testSOMSOMEquality(diffSom1_3_10_13, diffSom);

                diffSom = diffSom1_3_5_8_10_13;
                diffSom.difference(FastSetOfMonads(5,9));
                bContinue = bContinue && testSOMSOMEquality(diffSom1_3_10_13, diffSom);

                diffSom = diffSom1_3_5_8_10_13;
                diffSom.difference(FastSetOfMonads(4,9));
                bContinue = bContinue && testSOMSOMEquality(diffSom1_3_10_13, diffSom);

                diffSom = diffSom1_3_5_8_10_13;
                diffSom.difference(FastSetOfMonads(1,1));
                bContinue = bContinue && testSOMSOMEquality(diffSom2_3_5_8_10_13, diffSom);

                diffSom = diffSom1_3_5_8_10_13;
                FastSetOfMonads diffSom2(5,5);
                diffSom2.add(8,8);
                diffSom.difference(diffSom2);
                bContinue = bContinue && testSOMSOMEquality(diffSom1_3_6_7_10_13, diffSom);

                FastSetOfMonads diffSom3(128,160);
                FastSetOfMonads diffSom4(128,160);
		diffSom3.difference(diffSom4);
                FastSetOfMonads emptyFSom;
                bContinue = bContinue && testSOMSOMEquality(diffSom3, emptyFSom);

		try {
			FastSetOfMonads fsom(0,9);
			fsom.removeMSE(MonadSetElement(0,1));
			fsom.addMSE(MonadSetElement(0,1));
			std::cout << "SUCCESS: FastSetOfMonads: removeMSE() followed by addMSE() succeeded!\n";
		} catch (EmdrosException& e) {
			UNUSED(e);
			bContinue = false;
			std::cerr << "FAILURE: FastSetOfMonads: removeMSE() followed by addMSE() failed!" << std::endl;
		}
		
		SetOfMonads som_isMember;
		som_isMember.add(429, 448);
		som_isMember.add(1008, 1037);
		som_isMember.add(1078, 1083);
		som_isMember.add(1085, 1093);
		som_isMember.add(1452, 1462);
		som_isMember.add(1600, 1611);
		som_isMember.add(2269, 2273);
		som_isMember.add(2706, 2725);
		som_isMember.add(3247, 3253);
		som_isMember.add(3360, 3365);
		som_isMember.add(3438, 3443);
		som_isMember.add(3600, 3605);
		som_isMember.add(4109, 4168);
		som_isMember.add(4248, 4256);
		som_isMember.add(4454, 4530);
		som_isMember.add(5914, 5919);
		som_isMember.add(7146, 7158);
		som_isMember.add(7204, 7218);
		som_isMember.add(7472, 7531);
		som_isMember.add(7803, 7808);
		som_isMember.add(7918, 8028);
		som_isMember.add(8322, 8325);
		som_isMember.add(8369, 8374);
		som_isMember.add(8529, 8531);
		som_isMember.add(8665, 8677);
		som_isMember.add(8688, 8696);
		som_isMember.add(8719, 8722);
		som_isMember.add(8780, 8810);
		som_isMember.add(8960, 8964);
		som_isMember.add(8998, 9001);
		som_isMember.add(9042, 9046);
		som_isMember.add(9085, 9091);
		som_isMember.add(9466, 9485);
		som_isMember.add(9637, 9645);
		som_isMember.add(10391, 10400);
		som_isMember.add(10441, 10443);
		som_isMember.add(10449, 10451);
		som_isMember.add(10463, 10470);
		som_isMember.add(10498, 10503);
		som_isMember.add(10630, 10635);
		
		FastSetOfMonads fsom_isMember(som_isMember);
		
		
		bContinue = bContinue && testFastSOMisMemberOf(fsom_isMember, 428, false);
		bContinue = bContinue && testFastSOMisMemberOf(fsom_isMember, 427, false);
		bContinue = bContinue && testFastSOMisMemberOf(fsom_isMember, 449, false);
		bContinue = bContinue && testFastSOMisMemberOf(fsom_isMember, 1007, false);
		bContinue = bContinue && testFastSOMisMemberOf(fsom_isMember, 1038, false);
		bContinue = bContinue && testFastSOMisMemberOf(fsom_isMember, 1039, false);
		bContinue = bContinue && testFastSOMisMemberOf(fsom_isMember, 1077, false);
		bContinue = bContinue && testFastSOMisMemberOf(fsom_isMember, 1084, false);
		bContinue = bContinue && testFastSOMisMemberOf(fsom_isMember, 1085, true);
		bContinue = bContinue && testFastSOMisMemberOf(fsom_isMember, 429, true);
		bContinue = bContinue && testFastSOMisMemberOf(fsom_isMember, 430, true);
		bContinue = bContinue && testFastSOMisMemberOf(fsom_isMember, 448, true);
		bContinue = bContinue && testFastSOMisMemberOf(fsom_isMember, 1008, true);
		bContinue = bContinue && testFastSOMisMemberOf(fsom_isMember, 1030, true);
		bContinue = bContinue && testFastSOMisMemberOf(fsom_isMember, 1037, true);
		bContinue = bContinue && testFastSOMisMemberOf(fsom_isMember, 1078, true);
		bContinue = bContinue && testFastSOMisMemberOf(fsom_isMember, 1080, true);
		bContinue = bContinue && testFastSOMisMemberOf(fsom_isMember, 1083, true);


		if (bContinue) {
			std::cout << "SUCCESS: all tested FastSetOfMonads methods pass.\n";
		} else {
			std::cerr << "FAILURE: At least one FastSetOfMonads method failed." << std::endl;
			bAllOK = false;
		}
	}

	// Test XML parsing
	if (bContinue) {
		bContinue = test_MiniDOM_parse("<?xml version='1.0' ?><empty/>",
					       "<?xml version='1.0' ?><empty/>");
		bContinue = test_MiniDOM_parse("<?xml version='1.0' ?><empty></empty>",
					       "<?xml version='1.0' ?><empty/>");
		bContinue = test_MiniDOM_parse("<?xml version='1.0' ?><root a=\"&quot;\" b=\"&amp;\"></root>",
					       "<?xml version='1.0' ?><root a=\"&quot;\" b=\"&amp;\"/>");
		bContinue = test_MiniDOM_parse("<?xml version='1.0' ?><root a=\"&quot;\" b=\"&amp;\"> some text <inner/> and this is some more text! </root>",
					       "<?xml version='1.0' ?><root a=\"&quot;\" b=\"&amp;\"> some text <inner/> and this is some more text! </root>");
		bContinue = test_MiniDOM_parse("<?xml version='1.0' ?><root a=\"&quot;\" b=\"&amp;\"><inner/></root>",
					       "<?xml version='1.0' ?><root a=\"&quot;\" b=\"&amp;\"><inner/></root>");
		bContinue = test_MiniDOM_parse("<?xml version='1.0' ?><root><inner kind=\"title\" name=\"My Title\">blah blah blah</inner><inner></inner></root>",
					       "<?xml version='1.0' ?><root><inner kind=\"title\" name=\"My Title\">blah blah blah</inner><inner/></root>");
		bContinue = test_MiniDOM_parse("<?xml version='1.0' ?><root><!-- this is a comment --><inner kind=\"title\" name=\"My Title\">\nblah blah blah</inner><inner/></root>",
					       "<?xml version='1.0' ?><root><!-- this is a comment --><inner kind=\"title\" name=\"My Title\">\nblah blah blah</inner><inner/></root>");

		if (bContinue) {
			std::cout << "SUCCESS: all tested MiniDOM methods pass.\n";
		} else {
			std::cerr << "FAILURE: At least one MiniDOM method failed." << std::endl;
			bAllOK = false;
		}
	}
  

	// Test database creation
	if (bContinue) {
		if (pDB->createDatabase(newdb, password)) {
			std::cout << "SUCCESS: " << newdb << " created!\n";
			result = 0;
		} else {
			std::string dberr_msg = pDB->errorMessage();
			pDB->clearLocalError();
			if (pDB->dropDatabase(newdb)) {
				if (pDB->createDatabase(newdb, password)) {
					std::cout << "SUCCESS: " << newdb << " created!\n";
					result = 0;
				} else {
					std::cerr << "FAILURE: Could not create " << newdb << ".\n";
					DB_ERROR_MESSAGE;
				}
			} else {
				std::cerr << "FAILURE: Could not create " << newdb << ", or drop it.\n";
				DB_ERROR_MESSAGE;
			}
		}
	}

	if (bContinue) {
		if (pDB->useDatabase(newdb, password)) {
			std::cout << "SUCCESS: Database context switch to database '" << newdb << "' succeeded.\n";
		} else {
			std::cerr << "FAILURE: Database context switch to database '" << newdb << "' failed.\n";
			DB_ERROR_MESSAGE;
		}
	}

	emdf_ivalue schema_version;
	if (bContinue) {
		if (pDB->getSchemaVersion(schema_version)) {
			// Note: You should not set this to EMDROS_VERSION_CURRENT.
			// The idea is to check here that EMDROS_VERSION_CURRENT 
			// (which is used in EMdFDB::createDatabase to seed 
			// the schema_version table) is set correctly.
			// It is also to remind YOU (yes, you know whom I'm pointing at!)
			// to update EMdFDB::getCurrentDatabaseVersion!
			if (schema_version == EMDROS_VERSION_1_2_0_PRE_208) {
				std::cout << "SUCCESS: emdros schema version is OK.\n";
			} else {
				std::cerr << "FAILURE: emdros schema version was " << schema_version << ", which is" << std::endl 
					  << "not the latest.  Go check in emdf.h whether EMDROS_VERSION_CURRENT" << std::endl
					  << "is set correctly, or check emdftry.cpp whether it checks for the right" << std::endl << "version." << std::endl;
				VALUE_ERROR;
			}
		} else {
			std::cerr << "FAILURE: getSchemaVersion failed!\n";
			DB_ERROR_MESSAGE;
		}
	}


	id_d_t id_d;
	if (bContinue) {
		if (pDB->getNextObjectID_D(id_d)) {
			if (id_d == SEQUENCE_START) {
				std::cout << "SUCCESS: object id_d = " << id_d << " == SEQUENCE_START" << " in getNextObjectID_D.\n";
			} else {
				std::cerr << "FAILURE: first object id_d = " << id_d << " != SEQUENCE_START == " << SEQUENCE_START << " in getNextObjectID_D." << std::endl;
				VALUE_ERROR;
			}
		} else {
			std::cerr << "FAILURE: getNextObjectID_D failed!\n";
			DB_ERROR_MESSAGE;
		}
	}

	if (bContinue) {
		if (pDB->getNextObjectID_D(id_d)) {
			std::cout << "SUCCESS: object id_d = " << id_d << " in getNextObjectID\n";
		} else {
			std::cerr << "FAILURE: getNextObjectID_D failed!\n";
			DB_ERROR_MESSAGE;
		}
	}

	if (bContinue) {
		if (pDB->getNextTypeID(id_d)) {
			std::cout << "SUCCESS: type id_d = " << id_d << " in getNextTypeID\n";
		} else {
			std::cerr << "FAILURE: getNextTypeID failed!\n";
			DB_ERROR_MESSAGE;
		}
	}
  
	if (bContinue) {
		if (pDB->getNextOtherID(id_d)) {
			std::cout << "SUCCESS: other id_d = " << id_d << " in getNextOtherID\n";
		} else {
			std::cerr << "FAILURE: getNextOtherID failed!\n";
			DB_ERROR_MESSAGE;
		}
	}
  
	// Create enumeration
	emdf_ivalue enum_id = 0;
	if (bContinue) {
		if (pDB->createEnum("phrase_type_t", enum_id)) {
			std::cout << "SUCCESS: creating enum \n";
		} else {
			std::cerr << "FAILURE: creating enum \n";
			DB_ERROR_MESSAGE;
		}
	}

	// Check that enum exists
	bool enum_exists;
	emdf_ivalue second_enum_id;
	if (bContinue) {
		if (pDB->enumExists("phrase_type_t", enum_exists, second_enum_id)) {
			if (enum_exists && second_enum_id == enum_id)
				std::cout << "SUCCESS: checking existence of enum\n";
			else {
				std::cerr << "FAILURE: checking existence of enum: enum_exists = " << enum_exists << " second_enum_id = " << second_enum_id << " enum_id = " << enum_id << std::endl;
				VALUE_ERROR;
			}
		} else {
			std::cerr << "FAILURE in Checking existence of enum \n";
			DB_ERROR_MESSAGE;
		}
	}

	// Create enumeration constants
	if (bContinue) {
		if (pDB->createEnumConstant(enum_id, "phrase_type_t", EnumConstInfo("PT_NotAppliccable", -1, true))
		    && pDB->createEnumConstant(enum_id, "phrase_type_t", EnumConstInfo("VP", 1, false))
		    && pDB->createEnumConstant(enum_id, "phrase_type_t", EnumConstInfo("NP", 3, false))) {
			std::cout << "SUCCESS: creating enumeration constants\n";
		} else {
			std::cerr << "FAILURE: creating enumeration constants\n";
			DB_ERROR_MESSAGE;
		}
	}
  
	// Check get default
	bool get_default_result;
	std::string default_constant_name;
	if (bContinue) {
		if (pDB->getDefault(enum_id, get_default_result, default_constant_name)) {
			if (get_default_result && default_constant_name == "PT_NotAppliccable")
				std::cout << "SUCCESS: checking default enumeration constant\n";
			else {
				std::cerr << "FAILURE: checking default enumeration constant: default_constant_name = " << default_constant_name << " get_default_result = " << get_default_result << "\n";
				VALUE_ERROR;
			}
		} else {
			std::cerr << "FAILURE in checking default enumeration constant\n";
			DB_ERROR_MESSAGE;
		}
	}

	// Check that constant exists
	bool enum_const_exists_result;
	emdf_ivalue NP_enum_const_value;
	bool is_default;
	if (bContinue) {
		if (pDB->enumConstExists("NP", 
					 enum_id,
					 enum_const_exists_result,
					 NP_enum_const_value,
					 is_default)) {
			if (enum_const_exists_result && NP_enum_const_value == 3 && !is_default)
				std::cout << "SUCCESS: checking existence of enumeration constant\n";
			else {
				std::cerr << "FAILURE: checking existence enumeration constant: result = " << enum_const_exists_result << " value = " << NP_enum_const_value << " is_default = " << is_default << std::endl;
				VALUE_ERROR;
			}
		} else {
			std::cerr << "FAILURE: checking existence of enumeration constant\n";
			DB_ERROR_MESSAGE;
		}
	}

	// Check that constant exists
	std::string enum_const_name;
	if (bContinue) {
		if (pDB->enumConstExists(1, 
					 enum_id,
					 enum_const_exists_result,
					 enum_const_name,
					 is_default)) {
			if (enum_const_exists_result && enum_const_name == "VP" && !is_default)
				std::cout << "SUCCESS: checking existence of enumeration constant \"VP\"\n";
			else {
				std::cerr << "FAILURE: checking existence enumeration constant \"VP\": result = " << enum_const_exists_result << " name = " << enum_const_name << " is_default = " << is_default << std::endl;
				VALUE_ERROR;
			}
		} else {
			std::cerr << "FAILURE: checking existence of enumeration constant \"VP\"\n";
			DB_ERROR_MESSAGE;
		}
	}

	// Check that constant exists
	if (bContinue) {
		if (pDB->getEnumConstNameFromValue(3,
						   std::string("phrase_type_t"),
						   enum_const_name)) {
			if (enum_const_name == "NP")
				std::cout << "SUCCESS: getting string for enum-value 3\n";
			else {
				std::cerr << "FAILURE: getting string for enum-value 3: name = " << enum_const_name << " is_default = " << is_default << std::endl;
				VALUE_ERROR;
			}
		} else {
			std::cerr << "FAILURE: getting string for enum-value 3\n";
			DB_ERROR_MESSAGE;
		}
	}

	if (bContinue) {
		if (pDB->updateEnumConst("NP", enum_id, NP_enum_const_value, 2)) {
			std::cout << "SUCCESS: updating enumeration constant\n";
		} else {
			std::cerr << "FAILURE: updating enumeration constant\n";
			DB_ERROR_MESSAGE;
		}
	}

	// Check that it was really updated
	if (bContinue) {
		if (pDB->enumConstExists("NP", 
					 enum_id,
					 enum_const_exists_result,
					 NP_enum_const_value,
					 is_default)) {
			if (enum_const_exists_result && NP_enum_const_value == 2 && !is_default)
				std::cout << "SUCCESS: 2nd checking existence of enumeration constant\n";
			else {
				std::cerr << "FAILURE: 2nd checking existence enumeration constant: result = " << enum_const_exists_result << " value = " << NP_enum_const_value << " is_default = " << is_default << std::endl;
				VALUE_ERROR;
			}
		} else {
			std::cerr << "FAILURE: 2nd checking existence of enumeration constant\n";
			DB_ERROR_MESSAGE;
		}
	}

	// Check setting default
	if (bContinue) {
		if (pDB->setDefaultEnumConst(enum_id, "VP")
		    && pDB->getDefault(enum_id, get_default_result, default_constant_name)) {
			if (get_default_result && default_constant_name == "VP")
				std::cout << "SUCCESS: setting default enum constant\n";
			else {
				std::cerr << "FAILURE: setting default enumeration constant: result = " << get_default_result << " default_constant_name = " << default_constant_name << std::endl;
				VALUE_ERROR;
			}
		} else {
			std::cerr << "FAILURE: setting default enumeration constant\n";
			DB_ERROR_MESSAGE;
		}
	}

	// Check that there are no monad sets
	if (bContinue) {
		std::list<std::string> monad_set_names;
		if (pDB->selectMonadSets(monad_set_names)) {
			if (monad_set_names.size() == 0) {
				std::cout << "SUCCESS: Checking that there are no monad sets yet\n";
			} else {
				std::cerr << "FAILURE: There are monad sets, but there shouldn't be!\n";
				VALUE_ERROR;
			}
		} else {
			std::cerr << "FAILURE: Checking that there are no monad sets yet\n";
			DB_ERROR_MESSAGE;
		}
	}

	// Check creation of monad sets
	SetOfMonads som1(1, 3);
	som1.add(5, 8);
	som1.add(10,15);
	if (bContinue) {
		if (pDB->createMonadSet("SOM1", som1)) {
			std::cout << "SUCCESS: creating monad set SOM1\n";
		} else {
			std::cerr << "FAILURE: creating monad set SOM1\n";
			DB_ERROR_MESSAGE;
		}
	}

	// Check creation of monad sets
	SetOfMonads mysom2(20, 25);
	if (bContinue) {
		if (pDB->createMonadSet("MYSOM2", mysom2)) {
			std::cout << "SUCCESS: creating monad set MYSOM2\n";
		} else {
			std::cerr << "FAILURE: creating monad set MYSOM2\n";
			DB_ERROR_MESSAGE;
		}
	}

	// Check that SOM1 exists
	if (bContinue) {
		bool bExists;
		SetOfMonads ShouldBeSOM1;
		if (pDB->monadSetExists("SOM1", bExists, ShouldBeSOM1)) {
			if (bExists && ShouldBeSOM1 == som1) {
				std::cout << "SUCCESS: Checking that SOM1 exists\n";
			} else {
				std::cerr << "FAILURE: Checking that SOM1 exists: bExists = " << bExists << ", ShouldBeSOM1 = " << ShouldBeSOM1.toString() << "!" << std::endl;
				VALUE_ERROR;
			}
		} else {
			std::cerr << "FAILURE: Checking that SOM1 exists\n";
			DB_ERROR_MESSAGE;
		}
	}

	// Check that MYSOM2 exists
	if (bContinue) {
		bool bExists;
		SetOfMonads ShouldBeMYSOM2;
		if (pDB->monadSetExists("MYSOM2", bExists, ShouldBeMYSOM2)) {
			if (bExists && ShouldBeMYSOM2 == mysom2) {
				std::cout << "SUCCESS: Checking that MYSOM2 exists\n";
			} else {
				std::cerr << "FAILURE: Checking that MYSOM2 exists: bExists = " << bExists << ", ShouldBeMYSOM2 = " << ShouldBeMYSOM2.toString() << "!" << std::endl;
				VALUE_ERROR;
			}
		} else {
			std::cerr << "FAILURE: Checking that MYSOM2 exists\n";
			DB_ERROR_MESSAGE;
		}
	}

	// Check that there are no monad sets
	if (bContinue) {
		std::list<std::string> monad_set_names;
		if (pDB->selectMonadSets(monad_set_names)) {
			if (monad_set_names.size() == 2) {
				std::cout << "SUCCESS: Checking that there are two monad sets\n";
			} else {
				std::cerr << "FAILURE: There aren't 2 monad sets, but " << monad_set_names.size() << "!" << std::endl;
				VALUE_ERROR;
			}
		} else {
			std::cerr << "FAILURE: SUCCESS: Checking that there are two monad sets\n";
			DB_ERROR_MESSAGE;
		}
	}

	// Check dropping of monad set
	if (bContinue) {
		if (pDB->dropMonadSet("MySOM2")) {
			std::cout << "SUCCESS: dropping monad set MYSOM2\n";
		} else {
			std::cerr << "FAILURE: dropping monad set MYSOM2\n";
			DB_ERROR_MESSAGE;
		}
	}

	// Check that mysom2 doesn't exist
	if (bContinue) {
		bool bExists;
		SetOfMonads dummysom;
		if (pDB->monadSetExists("MYSOM2", bExists, dummysom)) {
			if (!bExists) {
				std::cout << "SUCCESS: Checking that MYSOM2 doesn't exist any more\n";
			} else {
				std::cerr << "FAILURE: MYSOM2 still exists!" << std::endl;
				VALUE_ERROR;
			}
		} else {
			std::cerr << "FAILURE: Checking that MYSOM2 doesn't exist\n";
			DB_ERROR_MESSAGE;
		}
	}

  

  


	// Check create object type
	id_d_t phrase_object_type_id;
	std::list<FeatureInfo> FeatureInfos;
	// feature "phrase_type"
	FeatureInfo f_info_phrase_type("phrase_type",
				       "",
				       enum_id | FEATURE_TYPE_ENUM,
				       "-1");
	FeatureInfos.push_back(f_info_phrase_type);

	// feature "NP_type"
	FeatureInfo f_info_NP_type("NP_type",
				   "",
				   FEATURE_TYPE_INTEGER,
				   "0");
	FeatureInfos.push_back(f_info_NP_type);

	// feature "Description"
	FeatureInfo f_info_description("Description",
				       "",
				       FEATURE_TYPE_STRING,
				       "This is a missing description!");
	FeatureInfos.push_back(f_info_description);

	// feature "Parent"
	FeatureInfo f_info_parent("Parent",
				  "",
				  FEATURE_TYPE_ID_D,
				  "0");
	FeatureInfos.push_back(f_info_parent);

	if (bContinue) {
		if (pDB->createObjectType("Phrase", FeatureInfos, kORTMultipleRange, kMUTNonUniqueMonads, phrase_object_type_id)) {
			std::cout << "SUCCESS: creating object type with id_d " << phrase_object_type_id << std::endl;
		} else {
			std::cerr << "FAILURE: creating object type" << std::endl;
			DB_ERROR_MESSAGE;
		}
	}

	// Check existence
	id_d_t second_phrase_object_type_id;
	bool object_type_exists_result;
	eObjectRangeType objectRangeType;
	eMonadUniquenessType monadUniquenessType;
	if (bContinue) {
		if (pDB->objectTypeExists("Phrase", object_type_exists_result, second_phrase_object_type_id, objectRangeType, monadUniquenessType)) {
			if (object_type_exists_result &&
			    second_phrase_object_type_id == phrase_object_type_id
			    && objectRangeType == kORTMultipleRange)
				std::cout << "SUCCESS: checking existence of phrase object type with id_d " << phrase_object_type_id << std::endl;
			else {
				std::cerr << "FAILURE: checking existence of phrase object type: result = " << object_type_exists_result << " second_phrase_object_type_id = " << second_phrase_object_type_id  << " objectRangeType = " << objectRangeType << std::endl;
				VALUE_ERROR;
			}
		} else {
			std::cerr << "FAILURE: checking existence of phrase object type" << std::endl;
			DB_ERROR_MESSAGE;
		}
	}

	// Check FeatureExists
	bool feature_exists_result;
	id_d_t feature_type_id;
	std::string default_feature_value;
	bool is_computed;
	if (bContinue) {
		if (pDB->featureExists("Parent", phrase_object_type_id, feature_exists_result, feature_type_id, default_feature_value, is_computed)) {
			if (feature_exists_result && !is_computed 
			    && default_feature_value == "0")
				std::cout << "SUCCESS: checking existence of feature with id_d " << feature_type_id << std::endl;
			else {
				std::cerr << "FAILURE: checking existence of feature: result = " << feature_exists_result << " feature_type_id = " << feature_type_id << " default_feature_value = " <<  default_feature_value << " is_computed = " << is_computed << std::endl;
				VALUE_ERROR;
			}
		} else {
			std::cerr << "FAILURE: checking existence of feature" << std::endl;
			DB_ERROR_MESSAGE;
		}
	}

	if (bContinue) {
		if (pDB->createObjectType("Phrase2", FeatureInfos, kORTSingleRange, kMUTNonUniqueMonads, phrase_object_type_id)) {
			std::cout << "SUCCESS: creating object type with id_d " << phrase_object_type_id << std::endl;
		} else {
			std::cerr << "FAILURE: creating object type" << std::endl;
			DB_ERROR_MESSAGE;
		}
	}

	// Check existence
	if (bContinue) {
		if (pDB->objectTypeExists("Phrase2", object_type_exists_result, second_phrase_object_type_id, objectRangeType, monadUniquenessType)) {
			if (object_type_exists_result &&
			    second_phrase_object_type_id == phrase_object_type_id
			    && objectRangeType == kORTSingleRange)
				std::cout << "SUCCESS: checking existence of phrase object type with id_d " << phrase_object_type_id << std::endl;
			else {
				std::cerr << "FAILURE: checking existence of phrase object type: result = " << object_type_exists_result << " second_phrase_object_type_id = " << second_phrase_object_type_id  << " objectRangeType = " << objectRangeType << std::endl;
				VALUE_ERROR;
			}
		} else {
			std::cerr << "FAILURE: checking existence of phrase object type" << std::endl;
			DB_ERROR_MESSAGE;
		}
	}

	// Create "Word" object type
	FeatureInfos.erase(FeatureInfos.begin(), FeatureInfos.end());
	// feature "surface"
	FeatureInfo f_info_surface("surface",
				   "",
				   FEATURE_TYPE_STRING | FEATURE_TYPE_FROM_SET,
				   "");
	FeatureInfos.push_back(f_info_surface);

	// feature "lemma"
	FeatureInfo f_info_lemma("lemma",
				 "",
				 FEATURE_TYPE_STRING,
				 "");
	FeatureInfos.push_back(f_info_lemma);

	// feature "Description"
	FeatureInfo f_info_description2("Description",
					"",
					FEATURE_TYPE_STRING,
					"This is a missing description!");
	FeatureInfos.push_back(f_info_description2);

	// feature "Parent"
	FeatureInfo f_info_parent2("Parent", "", FEATURE_TYPE_ID_D, NIL_AS_STRING);
	FeatureInfos.push_back(f_info_parent2);

	id_d_t word_object_type_id;
	if (bContinue) {
		if (pDB->createObjectType("Word", FeatureInfos, kORTSingleMonad, kMUTNonUniqueMonads, word_object_type_id)) {
			std::cout << "SUCCESS: creating object type 'Word' with id_d " << word_object_type_id << std::endl;
		} else {
			std::cerr << "FAILURE: creating object type 'Word'" << std::endl;
			DB_ERROR_MESSAGE;
		}
	}

	id_d_t second_word_object_type_id;
	if (bContinue) {
		if (pDB->objectTypeExists("Word", 
					  object_type_exists_result, 
					  second_word_object_type_id, 
					  objectRangeType,
					  monadUniquenessType)) {
			if (object_type_exists_result &&
			    second_word_object_type_id == word_object_type_id
			    && objectRangeType == kORTSingleMonad
			    && monadUniquenessType == kMUTNonUniqueMonads)
				std::cout << "SUCCESS: checking existence of word object type with id_d " << phrase_object_type_id << std::endl;
			else {
				std::cerr << "FAILURE: checking existence of word object type: result = " << object_type_exists_result << " second_word_object_type_id = " << second_word_object_type_id << " objectRangeType = " << objectRangeType << " monadUniquenessType = " << monadUniquenessType << std::endl;
				VALUE_ERROR;
			}
		} else {
			std::cerr << "FAILURE: checking existence of word object type" << std::endl;
			DB_ERROR_MESSAGE;
		}
	}

	// Check adding feature
	// feature "Parallax"
	FeatureInfo f_info_parallax("Parallax",
				    "",
				    FEATURE_TYPE_ID_D, NIL_AS_STRING);
	if (bContinue) {
		if (pDB->addFeature("Word", word_object_type_id, f_info_parallax)) {
			std::cout << "SUCCESS: Adding feature parallax.\n";
		} else {
			std::cerr << "FAILURE: Adding feature parallax.\n";
			DB_ERROR_MESSAGE;
		}
	}

	// Check dropping feature
	if (bContinue) {
		if (pDB->dropFeature("Word", "Parallax", FEATURE_TYPE_ID_D, word_object_type_id)) {
			std::cout << "SUCCESS: Dropping feature parallax.\n";
		} else {
			std::cerr << "FAILURE: Dropping feature parallax.\n";
			DB_ERROR_MESSAGE;
		}
	}

	// Check create covered by relationship
	/*
	  if (bContinue) {
	  if (pDB->CreateCoveredByRelationship(phrase_object_type_id, word_object_type_id)) {
	  std::cout << "SUCCESS: creating covered_by relationship" << std::endl;
	  } else {
	  std::cerr << "FAILURE: creating covered_by relationship" << std::endl;
	  DB_ERROR_MESSAGE;
	  }
	  }
	*/

	// Check create buildable from relationship
	/*
	  if (bContinue) {
	  if (pDB->CreateBuildableFromRelationship(phrase_object_type_id, word_object_type_id)) {
	  std::cout << "SUCCESS: creating buildable_from relationship" << std::endl;
	  } else {
	  std::cerr << "FAILURE: creating buildable_from relationship" << std::endl;
	  DB_ERROR_MESSAGE;
	  }
	  }
	*/

	// Checking that "Covered_by" relationship exists
	/*
	  bool covered_by_exists1, covered_by_exists2;
	  if (bContinue) {
	  if (pDB->CoveredByRelationshipExists(phrase_object_type_id, word_object_type_id, covered_by_exists1)
	  && pDB->CoveredByRelationshipExists(phrase_object_type_id, 2543453, covered_by_exists2)) {
	  if (covered_by_exists1 && !covered_by_exists2)
	  std::cout << "SUCCESS: checking existence of covered_by relationship" << std::endl;
	  else {
	  std::cerr << "FAILURE: checking existence of covered_by relationship: 1 = " << covered_by_exists1 << " 2 = " << covered_by_exists2 << std::endl;
	  VALUE_ERROR;
	  }
	  } else {
	  std::cerr << "FAILURE: checking existence of covered_by relationship" << std::endl;
	  DB_ERROR_MESSAGE;
	  }
	  }
	*/

	// Checking that "buildable_from" relationship exists
	/*
	  bool buildable_from_exists1, buildable_from_exists2;
	  if (bContinue) {
	  if (pDB->BuildableFromRelationshipExists(phrase_object_type_id, word_object_type_id, buildable_from_exists1)
	  && pDB->BuildableFromRelationshipExists(phrase_object_type_id, 2543453, buildable_from_exists2)) {
	  if (buildable_from_exists1 && !buildable_from_exists2)
	  std::cout << "SUCCESS: checking existence of buildable_from relationship" << std::endl;
	  else {
	  std::cerr << "FAILURE: checking existence of buildable_from relationship: 1 = " << buildable_from_exists1 << " 2 = " << buildable_from_exists2 << std::endl;
	  VALUE_ERROR;
	  }
	  } else {
	  std::cerr << "FAILURE: checking existence of buildable_from relationship" << std::endl;
	  DB_ERROR_MESSAGE;
	  }
	  }

	  // Checking dropping of covered_by relationship
	  if (bContinue) {
	  if (pDB->DropCoveredByRelationship(phrase_object_type_id, word_object_type_id)) {
	  std::cout << "SUCCESS: dropping covered_by relationship" << std::endl;
      
	  } else {
	  std::cerr << "FAILURE: dropping covered_by relationship" << std::endl;
	  DB_ERROR_MESSAGE;
	  }
	  }

	  // Checking dropping of buildable_from relationship
	  if (bContinue) {
	  if (pDB->DropBuildableFromRelationship(phrase_object_type_id, word_object_type_id)) {
	  std::cout << "SUCCESS: dropping buildable_from relationship" << std::endl;
      
	  } else {
	  std::cerr << "FAILURE: dropping buildable_from relationship" << std::endl;
	  DB_ERROR_MESSAGE;
	  }
	  }

	  // Checking dropping of covered_by relationship
	  if (bContinue) {
	  if (pDB->DropCoveredByRelationship(phrase_object_type_id, word_object_type_id)) {
	  std::cout << "SUCCESS: 2nd dropping of covered_by relationship" << std::endl;
      
	  } else {
	  std::cerr << "FAILURE: 2nd dropping covered_by relationship" << std::endl;
	  DB_ERROR_MESSAGE;
	  }
	  }



	  // Checking that "Covered_by" relationship exists
	  if (bContinue) {
	  if (pDB->CoveredByRelationshipExists(phrase_object_type_id, word_object_type_id, covered_by_exists1)) {
	  if (!covered_by_exists1)
	  std::cout << "SUCCESS: 2nd checking existence of covered_by relationship" << std::endl;
	  else {
	  std::cerr << "FAILURE: 2nd checking existence of covered_by relationship: 1 = " << covered_by_exists1 << std::endl;
	  VALUE_ERROR;
	  }
	  } else {
	  std::cerr << "FAILURE: 2nd checking existence of covered_by relationship" << std::endl;
	  DB_ERROR_MESSAGE;
	  }
	  }

	  // Checking that "buildable_from" relationship exists
	  if (bContinue) {
	  if (pDB->BuildableFromRelationshipExists(phrase_object_type_id, word_object_type_id, buildable_from_exists1)) {
	  if (!buildable_from_exists1)
	  std::cout << "SUCCESS: 2nd checking existence of buildable_from relationship" << std::endl;
	  else {
	  std::cerr << "FAILURE: 2nd checking existence of buildable_from relationship: 1 = " << buildable_from_exists1 << std::endl;
	  VALUE_ERROR;
	  }
	  } else {
	  std::cerr << "FAILURE: 2nd checking existence of buildable_from relationship" << std::endl;
	  DB_ERROR_MESSAGE;
	  }
	  }
	*/

	// Check creation of object
	// Create phrase
	id_d_t phrase1;
	if (bContinue) {
		if (!pDB->getNextObjectID_D(phrase1)) {
			std::cerr << "FAILURE: Error in getting oid for phrase (getNextObjectID_D)." << std::endl;
			DB_ERROR_MESSAGE;
		}
	}

	SetOfMonads monads3;
	monads3.add(1,2);
	std::list<FeatureInfo> features;
	features.push_back(FeatureInfo("phrase_type", "", enum_id | FEATURE_TYPE_ENUM, "1"));
	features.push_back(FeatureInfo("NP_type", "", FEATURE_TYPE_INTEGER, "0"));
	features.push_back(FeatureInfo("Description", "", FEATURE_TYPE_STRING, "This is a missing description!"));
	features.push_back(FeatureInfo("Parent", "", FEATURE_TYPE_ID_D, NIL_AS_STRING));
	if (bContinue) {
		if (pDB->createObject(phrase1, "Phrase", phrase_object_type_id, monads3, kORTMultipleRange, features)) {
			std::cout << "SUCCESS: creating object of type Phrase\n";
		} else {
			std::cerr << "FAILURE: creating object of type Phrase" << std::endl;
			DB_ERROR_MESSAGE;
		}
	}

	// Create word1
	id_d_t word1;
	if (bContinue) {
		if (!pDB->getNextObjectID_D(word1)) {
			std::cerr << "FAILURE: getting id_d for word1 in getNextObjectID_D." << std::endl;
			DB_ERROR_MESSAGE;
		}
	}
	SetOfMonads monads1(1);
	features.erase(features.begin(), features.end());
	std::string word1_surface_value = "Very";
	std::string word1_lemma_value = "very";
	features.push_back(FeatureInfo("surface",
				       "",
				       FEATURE_TYPE_STRING | FEATURE_TYPE_FROM_SET, word1_surface_value));
	features.push_back(FeatureInfo("lemma",
				       "", FEATURE_TYPE_STRING, word1_lemma_value));
	features.push_back(FeatureInfo("Description", "", FEATURE_TYPE_STRING, "Adverb"));
	features.push_back(FeatureInfo("Parent", "", FEATURE_TYPE_ID_D, NIL_AS_STRING));
	if (bContinue) {
		if (pDB->createObject(word1, "Word", word_object_type_id, monads1, kORTSingleMonad, features)) {
			std::cout << "SUCCESS: creating 1st object of type Word\n";
		} else {
			std::cerr << "FAILURE: creating 1st object of type Word" << std::endl;
			DB_ERROR_MESSAGE;
		}
	}
  
	// Create 2nd word
	features.erase(features.begin(), features.end());
	id_d_t word2;
	if (bContinue) {
		if (!pDB->getNextObjectID_D(word2)) {
			std::cerr << "FAILURE: Error in getting oid for word2 (getNextObjectID_D)." << std::endl;
			DB_ERROR_MESSAGE;
		}
	}
	SetOfMonads monads2(2);
	features.push_back(FeatureInfo("surface", "", FEATURE_TYPE_STRING | FEATURE_TYPE_FROM_SET, "good"));
	features.push_back(FeatureInfo("lemma", "", FEATURE_TYPE_STRING, "good"));
	features.push_back(FeatureInfo("Description", "", FEATURE_TYPE_STRING, "Adjective"));
	features.push_back(FeatureInfo("Parent", "", FEATURE_TYPE_ID_D, NIL_AS_STRING));
	if (bContinue) {
		if (pDB->createObject(word2, "Word", word_object_type_id, monads2, kORTSingleMonad, features)) {
			std::cout << "SUCCESS: creating 2nd object of type Word\n";
		} else {
			std::cerr << "FAILURE: creating 2nd object of type Word" << std::endl;
			DB_ERROR_MESSAGE;
		}
	}



	// Checking ObjectID_DExists
	bool oidExists;
	id_d_t word1Type;
	if (bContinue) {
		if (pDB->objectID_DExists(word1, oidExists, word1Type)) {
			if (oidExists && word1Type == word_object_type_id) {
				std::cout << "SUCCESS: checking existence of id_d\n";
			} else {
				std::cerr << "FAILURE: checking existence of id_d " << std::endl;
				VALUE_ERROR;
			}
		} else {
			std::cerr << "FAILURE: checking existence of id_d " << std::endl;
			DB_ERROR_MESSAGE;
		}
	}

	// Check getting features from object
	if (bContinue) {
		FeatureInfos.clear();
		FeatureInfos.push_back(FeatureInfo("surface",
						   "",
						   FEATURE_TYPE_STRING | FEATURE_TYPE_FROM_SET,
						   "") // It doesn't matter what we use as default value
				       ); 
		FeatureInfos.push_back(FeatureInfo("lemma",
						   "",
						   FEATURE_TYPE_STRING,
						   "") // It doesn't matter what we use as default value
				       ); 
		std::list<std::list<std::string> > results;
		if (pDB->getFeatures("word", word_object_type_id, objectRangeType, FeatureInfos, word1, results)) {
			std::list<std::list<std::string> >::iterator it = results.begin();
			std::list<std::string> inner_list = *it;
			std::list<std::string>::const_iterator inner_list_ci = inner_list.begin();
			// Skip object_id_d
			++inner_list_ci;
			std::string surface_value = *inner_list_ci;
			if (surface_value != word1_surface_value) {
				std::cerr << "FAILURE: getting feature 'surface' for word with id_d " << word1 << " did not return the right string. The string returned was '" << surface_value << "'. I expected the string '" << word1_surface_value << "'." << std::endl;
				VALUE_ERROR;
			} else {
				std::cout << "SUCCESS: getting feature 'surface' for word with id_d " << word1 << " returned the correct string.\n";
			}
			++inner_list_ci;
			std::string lemma_value = *inner_list_ci;
			if (lemma_value != word1_lemma_value) {
				std::cerr << "FAILURE: getting feature 'lemma' for word with id_d " << word1 << " did not return the right string. The string returned was '" << lemma_value << "'. I expected the string '" << word1_lemma_value << "'." << std::endl;
				VALUE_ERROR;
			} else {
				std::cout << "SUCCESS: getting feature 'lemma' for word with id_d " << word1 << " returned the correct string.\n";
			}
			++it;
		} else {
			std::cerr << "FAILURE: Could not get features from word object with id_d " << word1 << std::endl;
			DB_ERROR_MESSAGE;
		}
	}

	// Check getting objects starting at Sm
	IDDList iddlist;
	if (bContinue) {
		if (pDB->getObjectsStartingAtSm("Word", 2, iddlist)) {
			if (iddlist.length() == 1) {
				if (*(iddlist.begin()) == word2) {
					std::cout << "SUCCESS: IDDList had 1 member which was " << word2 << ". (getObjectsStartingAtSm).\n";
				} else {
					std::cerr << "FAILURE: IDDList had 1 member which was not " << word2 << " but " << *(iddlist.begin()) << ". (getObjectsStartingAtSm)." << std::endl;
					VALUE_ERROR;
				}
			} else {
				std::cerr << "FAILURE: IDDList was not 1 long (getObjectsStartingAtSm)." << std::endl;
				VALUE_ERROR;
			}
		} else {
			std::cerr << "FAILURE: Could not get objects starting at 1. (getObjectsStartingAtSm)." << std::endl;
			DB_ERROR_MESSAGE;
		}
	}

	// Getting SOM for object word2
	SetOfMonads som2;
	if (bContinue) {
		if (pDB->getSOMForObject("Word", word2, kORTSingleMonad, som2)) {
			if (som2 == monads2) {
				std::cout << "SUCCESS: Getting SOM for word2 was OK.\n";
			} else {
				std::cerr << "FAILURE: Getting SOM for word2 was wrong." << std::endl;
				VALUE_ERROR;
			}
		} else {
			std::cerr << "FAILURE: Could not get SOM for word2" << std::endl;
			DB_ERROR_MESSAGE;
		}
	}

	// Getting min_m
	monad_m min_m;
	if (bContinue) {
		if (pDB->getMin_m(min_m)) {
#define EXPECTED_MIN_M  (1)
			if (min_m == EXPECTED_MIN_M) {
				std::cout << "SUCCESS: min_m is " << min_m << " which it should be.\n";
			} else {
				std::cerr << "FAILURE: min_m is not " << EXPECTED_MIN_M << " but " << min_m << "." << std::endl;
				VALUE_ERROR;
			}
		} else {
			std::cerr << "FAILURE: Could not get min_m" << std::endl;
			DB_ERROR_MESSAGE;
		}
	}

	// Getting max_m
	monad_m max_m;
	if (bContinue) {
		if (pDB->getMax_m(max_m)) {
#define EXPECTED_MAX_M  (2)
			if (max_m == EXPECTED_MAX_M) {
				std::cout << "SUCCESS: max_m is " << max_m << " which it should be.\n";
			} else {
				std::cerr << "FAILURE: max_m is not " << EXPECTED_MAX_M << " but " << max_m << "." << std::endl;
				VALUE_ERROR;
			}
		} else {
			std::cerr << "FAILURE: Could not get max_m" << std::endl;
			DB_ERROR_MESSAGE;
		}
	}

	// Get object types
	std::list<std::string> object_types_list;
	if (bContinue) {
		if (pDB->getObjectTypes(object_types_list)) {
			if (object_types_list.size() == 3) {
				std::cout << "SUCCESS: Object types list is:\n";
			} else {
				std::cerr << "FAILURE: Object types list wasn't 3 long.\n";
				VALUE_ERROR;
			}
			std::list<std::string>::const_iterator otci;
			otci = object_types_list.begin();
			while (otci != object_types_list.end()) {
				std::cout << "         '" << *otci << "'\n";
				++otci;
			}
		} else {
			std::cerr << "FAILURE: Couldn't get object types.\n";
			DB_ERROR_MESSAGE;
		}
	}

	// Check getting features
	features.erase(features.begin(), features.end());
	if (bContinue) {
		if (pDB->getFeaturesForObjectType(word_object_type_id,
						  features)) {
			if (features.size() == 5) {
				std::cout << "SUCCESS: Features of object type Word are:\n";
			} else {
				std::cerr << "FAILURE: Feature-list of object type Word wasn't 5 long.\n";
				VALUE_ERROR;
			}
			std::list<FeatureInfo>::const_iterator ci;
			ci = features.begin();
			while (ci != features.end()) {
				std::cout << "         ('" << ci->getRetrievedFeatureName() << "', " << ci->getRetrievedType() << ", '" << ci->getDefaultValue() << "', " << ci->getIsComputed() << ")\n";
				++ci;
			}
		} else {
			std::cerr << "FAILURE: Couldn't get list of features for object type word\n";
			DB_ERROR_MESSAGE;
		}
	}

	// Check getting enumerations
	std::list<std::string> enums;
	if (bContinue) {
		if (pDB->getEnumerations(enums)) {
			if (enums.size() == 1) {
				std::cout << "SUCCESS: Enumerations are:\n";
			} else {
				std::cerr << "FAILURE: Enumeration-list is not 1 long\n";
				VALUE_ERROR;
			}
			std::list<std::string>::const_iterator ci;
			ci = enums.begin();
			while (ci != enums.end()) {
				std::cout << "         '" << *ci << "'\n";
				++ci;
			}
		} else {
			std::cerr << "FAILURE: Couldn't get list of enumerations\n";
			DB_ERROR_MESSAGE;
		}
	}
    
	// Check enum constants
	std::list<EnumConstInfo> enum_consts;
	if (bContinue) {
		if (pDB->getEnumConstants(enum_id, enum_consts)) {
			if (enum_consts.size() == 3) {
				std::cout << "SUCCESS: Enumeration constants are:\n";
			} else {
				std::cerr << "FAILURE: Enumeration constant list wasn't 3 long.\n";
				VALUE_ERROR;
			}
			std::list<EnumConstInfo>::const_iterator ci;
			ci = enum_consts.begin();
			while (ci != enum_consts.end()) {
				std::cout << "         ('" << ci->getName() << "', " << ci->getValue() << ", " << ci->getIsDefault() << ")\n";
				++ci;
			}
		} else {
			std::cerr << "FAILURE: Couldn't get enum constants.\n";
			DB_ERROR_MESSAGE;
		}
	}

	// Check getting object types using enumeration
	object_types_list.erase(object_types_list.begin(), object_types_list.end());
	if (bContinue) {
		if (pDB->getObjectTypesUsingEnumeration(enum_id, object_types_list)) {
			if (object_types_list.size() == 2) {
				std::cout << "SUCCESS: Object types using enumeration are:\n";
			} else {
				std::cerr << "FAILURE: List of object types using enumeration was not 2 long.  It was: " << object_types_list.size() << " long...\n";
				VALUE_ERROR;
			}
			std::list<std::string>::const_iterator otci;
			otci = object_types_list.begin();
			while (otci != object_types_list.end()) {
				std::cout << "         '" << *otci << "'\n";
				++otci;
			}
		} else {
			std::cerr << "FAILURE: Couldn't get object types using enumeration.\n";
			DB_ERROR_MESSAGE;
		}
	}

	if (bContinue) {
		IntegerListNode *pILN1 = new IntegerListNode(1, 0);
		IntegerListNode *pILN2 = new IntegerListNode(2, 0);
		pILN1->setNext(pILN2);
		pILN2->setPrev(pILN1);
		IntegerList integerlist1(pILN2);
		delete pILN2;
		std::string myil1string = integerlist1.getDelimitedString(DEFAULT_LIST_DELIMITER);
		if (myil1string != " 1 2 ") {
			std::cerr << "FAILURE: basic IntegerList::getDelimitedString() test failed: It should be ' 1 2 ' but it was '" << myil1string << "'.\n";
			bContinue = false;
		}
		integerlist1.addValueFront(0);
		integerlist1.addValueBack(3);
		myil1string = integerlist1.getDelimitedString(DEFAULT_LIST_DELIMITER);
		if (myil1string != " 0 1 2 3 ") {
			std::cerr << "FAILURE: basic IntegerList::getDelimitedString() test failed: It should be ' 0 1 2 3 ' but it was '" << myil1string << "'.\n";
			bContinue = false;
		}

		IntegerList integerlist2 = integerlist1;

		myil1string = integerlist2.getDelimitedString(DEFAULT_LIST_DELIMITER);
		if (myil1string != " 0 1 2 3 ") {
			std::cerr << "FAILURE: basic IntegerList::getDelimitedString() test failed: It should be ' 0 1 2 3 ' but it was '" << myil1string << "'.\n";
			bContinue = false;
		}

		IntegerList *pIntegerlist3 = new IntegerList(integerlist1);

		myil1string = pIntegerlist3->getDelimitedString(DEFAULT_LIST_DELIMITER);
		if (myil1string != " 0 1 2 3 ") {
			std::cerr << "FAILURE: basic IntegerList::getDelimitedString() test failed: It should be ' 0 1 2 3 ' but it was '" << myil1string << "'.\n";
			bContinue = false;
		}

		delete pIntegerlist3;


		if (bContinue) {
			std::cout << "SUCCESS: All IntegerList functions pass.\n";
		} else {
			bAllOK = false;
		}
	}

  




	// Drop database
	if (pDB->dropDatabase(newdb)) {
		std::cout << "SUCCESS in dropping database '" << newdb << "'\n";
	} else {
		std::cerr << "FAILURE in dropping database '" << newdb << "'\n";
		DB_ERROR_MESSAGE;
	}


	if (bAllOK) {
		std::cout << "SUCCESS: All tests completed successfully on backend " << backend_kind2string(backend_kind) << '.' << std::endl;
	} else {
		std::cerr << "FAILURE: At least one test failed on backend " << backend_kind2string(backend_kind) << '.' << std::endl;
		result = 1;
	}

	// Clean up
	delete pDB;

	return result;
}

int main(int argc, char* argv[])
{
	int result = 0;
	std::string hostname;
	std::string password;
	std::string user;
	eBackendKind backend_kind;
	eCharsets dummy_charset;
	bool bTestedSQLite3 = false;
	bool bTestedPostgreSQL = false;
	bool bTestedMySQL = false;

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
		
		// Did the user specify a backend?
		if (!getArgumentPresent("-b")) {
			// No, so we say it's "kBackenNone" to indicate
			// further down that we want them all...
			backend_kind = kBackendNone;
		}

		if (surplus_arguments.size() != 0) {
			print_usage(std::cerr);
			std::cerr << "ERROR: Extraneous argument(s) not parsed: '" << joinList("' and '", surplus_arguments) << "'." << std::endl;
			return 1;
		}
	}


#if USE_SQLITE3
	if (result == 0
	    && (backend_kind == kBackendNone || backend_kind == kSQLite3)) {
		bTestedSQLite3 = true;
		result = test_backend(kSQLite3, hostname, user, password);
		if (result != 0) {
			std::cerr << "FAILURE: At least one test failed." << std::endl;
		}
	}
#endif
#if USE_POSTGRESQL
	if (result == 0
	    && (backend_kind == kBackendNone || backend_kind == kPostgreSQL)) {
		bTestedPostgreSQL = true;
		result = test_backend(kPostgreSQL, hostname, user, password);
		if (result != 0) {
			std::cerr << "FAILURE: At least one test failed." << std::endl;
		}
	}
#endif
#if USE_MYSQL
	if (result == 0
	    && (backend_kind == kBackendNone || backend_kind == kMySQL)) {
		bTestedMySQL = true;
		result = test_backend(kMySQL, hostname, user, password);
		if (result != 0) {
			std::cerr << "FAILURE: At least one test failed." << std::endl;
		}
	}
#endif

	std::cout << "Backends tested:\n";
	if (bTestedSQLite3) {
		std::cout << "                 - SQLite 3.X.X\n";
	}
	if (bTestedPostgreSQL) {
		std::cout << "                 - PostgreSQL\n";
	}
	if (bTestedMySQL) {
		std::cout << "                 - MySQL\n";
	}

	if (result == 0) {
		std::cout << "SUCCESS: All tests completed successfully.\n";
	} else {
		std::cerr << "FAILURE: At least one test failed." << std::endl;
	}

	return result;
}


