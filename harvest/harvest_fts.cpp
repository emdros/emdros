/*
 * harvest_fts.cpp
 *
 * Full Text Search, part of the harvest library.
 *
 * Ulrik Sandborg-Petersen
 * Created: 8/25-2010
 * Last update: 11/30-2018
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



/**@file harvest_fts.cpp
 *@brief Implementation file for harvest full text search
 */

#include <harvest_fts.h>
#include <harvest_fts2.h>
#include <harvest_fts3.h>
#include <map>

#include <emdf.h>
#include <mql_sheaf.h>
#include <mql_execution_environment.h>
#include <emdfdb.h>
#include <string_func.h> // String functions in libemdf.


#define MYSIGN(A) ((A == 0) ? 0 : ((A < 0) ? -1 : 1))
#define MYABS(A) ((A < 0) ? -A : A)

#define MYMAX(A,B) ((A > B) ? (A) : (B))
#define MYMIN(A,B) ((A < B) ? (A) : (B))


#define FTS_STRING_FILTER_NOPARAM(FilterName, ClassName) \
	if (filter_name == FilterName) { return new ClassName(); }

#define FTS_STRING_FILTER_WITH_STRING_PARAM(FilterName, ClassName) \
	if (filter_name == FilterName) { if (ci.hasNext()) { return new ClassName(ci.next()); } else { return 0; } }


std::string make_index_name(const std::string& index_infix_suffix, const std::string& bookcase_OTN, const std::string& indexed_OTN, const std::list<std::string>& indexed_feature_name_list)
{
	return "bookcase_" + bookcase_OTN + index_infix_suffix + "_" + indexed_OTN + "_mdf_" + joinList("_mdf_", indexed_feature_name_list, 4096) + index_infix_suffix;
}

std::string make_string_feature_comparison(const std::string& feature_name, const std::string feature_value)
{
	std::string result;
	if (feature_value.find_first_of("*?") == std::string::npos) {
		result = feature_name + "=\"" + encodeSTRINGstring(feature_value) + "\"";
	} else {
		std::string feature_value_to_use = replace_substring(feature_value, "\\", "\\\\");
		feature_value_to_use = replace_substring(feature_value_to_use, ".", "\\.");
		feature_value_to_use = replace_substring(feature_value_to_use, "[", "\\[");
		feature_value_to_use = replace_substring(feature_value_to_use, "]", "\\]");
		feature_value_to_use = replace_substring(feature_value_to_use, "^", "\\^");
		feature_value_to_use = replace_substring(feature_value_to_use, "$", "\\$");
		feature_value_to_use = replace_substring(feature_value_to_use, "+", ".+");
		feature_value_to_use = replace_substring(feature_value_to_use, "*", ".*");
		feature_value_to_use = replace_substring(feature_value_to_use, "?", ".");
		result = feature_name + "~\"^" + encodeSTRINGstring(feature_value_to_use) + "$\"";
	}

	return result;
}


void process_comma_separated_string_list(const std::string& comma_separated_string, std::list<std::string>& string_list)
{
	std::list<std::string> tmp_list;

	// Make sure the output list is empty
	string_list = tmp_list;

	// Split on comma
	split_string(comma_separated_string, ",", tmp_list);

	std::list<std::string>::const_iterator ci = tmp_list.begin();
	while (ci != tmp_list.end()) {
		// Strip whitespace
		std::string tmp_str = strip(*ci);

		// Only add if it isn't empty
		if (!tmp_str.empty()) {
			string_list.push_back(tmp_str); 
		}

		++ci;
	}
}


SetOfMonads fill_with_largest_proximity(const SetOfMonads& SOM, monad_m largest_proximity, monad_m first_monad, monad_m last_monad)
{
	SetOfMonads resultSOM;

	if (largest_proximity > (last_monad - first_monad + 1)) {
		resultSOM.add(first_monad, last_monad);
		return resultSOM;
	} else {
		SOMConstIterator SOM_ci = SOM.const_iterator();
		while (SOM_ci.hasNext()) {
			MonadSetElement mse = SOM_ci.next();
			
			monad_m mse_first = mse.first();
			monad_m mse_last = mse.last();
			
			monad_m min_m = MYMAX(first_monad, mse_first - largest_proximity);
			monad_m max_m = MYMIN(last_monad, mse_last + largest_proximity);
			
			resultSOM.add(min_m, max_m);
		}
		return resultSOM;
	}
}


EmdrosFTSStringFilter *createStringFilter(StringListConstIterator& ci)
{
	if (!ci.hasNext()) {
		return 0;
	}

	// we always advance by at least one, and assume that the
	// iterator is placed at a filter name (not a parameter) when
	// we enter the function.
	std::string filter_name = ci.next();
	
#ifdef THIRD_PARTY_FTS_STRING_FILTERS
	THIRD_PARTY_FTS_STRING_FILTERS
#endif
	FTS_STRING_FILTER_WITH_STRING_PARAM("strip-chars", SFStripChars);
	FTS_STRING_FILTER_NOPARAM("strip-whitespace", SFStripChars);
	FTS_STRING_FILTER_NOPARAM("lowercase", SFLowerCase);
	FTS_STRING_FILTER_NOPARAM("uppercase", SFUpperCase);
	FTS_STRING_FILTER_NOPARAM("block-empty", SFBlockEmpty);

	// If none of the above found any filters, we return nil
	// to show that none was found.
	return 0;
}


EmdrosFTSStringFilter *stringFilterFactory(const StringList& filter_names_and_parameters)
{
	if (filter_names_and_parameters.isEmpty()) {
		return 0;
	}

	std::list<EmdrosFTSStringFilter*> mylist;

	StringListConstIterator ci = filter_names_and_parameters.const_iterator();

	while (ci.hasNext()) {
		EmdrosFTSStringFilter *pResult = createStringFilter(ci);
		if (pResult != 0) {
		        mylist.push_back(pResult);
		}
	}

	if (mylist.empty()) {
		return 0;
	} 

	// Chain them together with EmdrosFTSStringFilter::m_pNext.
	std::list<EmdrosFTSStringFilter*>::iterator myit = mylist.begin();
	while (myit != mylist.end()) {
		EmdrosFTSStringFilter *pCurrent = *myit;
		++myit;
		if (myit != mylist.end()) {
			EmdrosFTSStringFilter *pNext = *myit;
			pCurrent->setNext(pNext);
		} else {
			pCurrent->setNext(0);
		}
	}

	// Return the head.
	return *(mylist.begin());
}


EmdrosFTSStringFilter *stringFilterFactoryJSONValue(const JSONValue* pJSONObject, bool& bResult, std::string& error_message, const std::string& filter_name)
{
	bResult = true;

	if (pJSONObject == 0) {
		bResult = false;
		error_message += "pJSONObject is nil.";
		return 0;
	}

	if (pJSONObject->getKind() != kJSONObject) {
		bResult = false;
		error_message += "pJSONObject is not a JSON Object.";
		return 0;
	}

	std::list<std::string> key_names = pJSONObject->getObjectKeys();
	if (key_names.empty()) {
		bResult = false;
		error_message = "pJSONObject is empty.";
		return 0;
	}

	std::string myfilter_name = filter_name;
	if (myfilter_name == "") {
		if (key_names.size() != 1) {
			bResult = false;
			error_message = "filter_name is empty, but pJSONObject object has more than one key.";
			return 0;
		} else {
			myfilter_name = *(key_names.begin());
		}
	}

	if (!(pJSONObject->hasObjectKey(myfilter_name))) {
		bResult = false;
		error_message = "pJSONObject does not have the key '" + myfilter_name + "'.";
		return 0;
	}

	const JSONValue *pJSONList = pJSONObject->getObjectValue(myfilter_name);

	if (pJSONList->getKind() != kJSONList) {
		bResult = false;
		error_message = "pJSONObject['" +  myfilter_name + "'] isn't a JSON List.";
		return 0;
	}

	std::list<EmdrosFTSStringFilter*> mylist;

	const std::list<JSONValue*> jsonvalue_list = pJSONList->getList();
	std::list<JSONValue*>::const_iterator ci = jsonvalue_list.begin();
	while (ci != jsonvalue_list.end()) {
		EmdrosFTSStringFilter *pFilter = 0;

#define GET_NEXT_STRING_PARAM(PARAM, ERROR_MESSAGE) \
		if (ci == jsonvalue_list.end()) { \
			bResult = false; \
			error_message = ERROR_MESSAGE; \
			break; \
		} else { \
			++ci; \
		        if (ci == jsonvalue_list.end()) {	\
			        bResult = false; \
				error_message = ERROR_MESSAGE;	\
				break;				\
			} else { \
			        const JSONValue *pJSONValue = *ci; \
				if (pJSONValue->getKind() != kJSONString) { \
					bResult = false;		\
					error_message = ERROR_MESSAGE;	\
					break;				\
				} else {				\
					PARAM = pJSONValue->getString(); \
				} \
			} \
		} 

		const JSONValue *pThisValue = *ci;
		if (pThisValue->getKind() != kJSONString) {
			bResult = false; 
			break;
		}
		std::string this_string = pThisValue->getString();
		if (this_string == "strip-whitespace") {
			pFilter = new SFStripChars();
		} else if (this_string == "strip-chars") {
			std::string param;
			GET_NEXT_STRING_PARAM(param, "strip-chars needs 1 string argument");
			pFilter = new SFStripChars(param);
		} else if (this_string == "replace-string") {
			std::string param1, param2;
			GET_NEXT_STRING_PARAM(param1, "replace-string needs 2 string arguments");
			GET_NEXT_STRING_PARAM(param2, "replace-string needs 2 string arguments");
			pFilter = new SFReplaceString(param1, param2);
		} else if (this_string == "lowercase") {
			pFilter = new SFLowerCase();
		} else if (this_string == "uppercase") {
			pFilter = new SFUpperCase();
		} else if (this_string == "block-empty") {
			pFilter = new SFBlockEmpty();
		} else if (this_string == "stop-words") {
			if (ci == jsonvalue_list.end()) {
				bResult = false;
				error_message = "stop-words requires 1 argument; a JSON List of strings.";
				break;
			}
			++ci;
			if (ci == jsonvalue_list.end()) {
				bResult = false;
				error_message = "stop-words requires 1 argument; a JSON List of strings.";
				break;
			}
			const JSONValue *pStopWordJSONList = *ci;
			if (pStopWordJSONList->getKind() != kJSONList) {
				bResult = false;
				error_message = "stop-words requires 1 argument; a JSON List of strings.";
				break;
			}
			const std::list<JSONValue*>& stopwords_list = pStopWordJSONList->getList();
			std::list<JSONValue*>::const_iterator ci2 = stopwords_list.begin();
			StringList mystopword_list;
			while (ci2 != stopwords_list.end()) {
				const JSONValue *pStopWordJSONValue = *ci2;
				if (pStopWordJSONValue->getKind() != kJSONString) {
					bResult = false;
					error_message = "stop-words requires 1 argument; a JSON List of strings.";
					break;
				} else {
					mystopword_list.addValueBack(pStopWordJSONValue->getString());
				}
				++ci2;
			}
			pFilter = new SFStopWords(mystopword_list);
		} else {
			bResult = false;
			error_message = "Unknown filter with name '" + this_string + "'";
			break;
		}

		if (pFilter != 0) {
			mylist.push_back(pFilter);
		}
		++ci;
	}

	if (mylist.empty()) {
		return 0;
	} 

	// Chain them together with EmdrosFTSStringFilter::m_pNext.
	std::list<EmdrosFTSStringFilter*>::iterator myit = mylist.begin();
	while (myit != mylist.end()) {
		EmdrosFTSStringFilter *pCurrent = *myit;
		++myit;
		if (myit != mylist.end()) {
			EmdrosFTSStringFilter *pNext = *myit;
			pCurrent->setNext(pNext);
		} else {
			pCurrent->setNext(0);
		}
	}

	// Return the head.
	return *(mylist.begin());
}

EmdrosFTSStringFilter *stringFilterFactoryJSONString(const std::string& JSON_string, bool& bResult, std::string& error_message, const std::string& filter_name)
{
	JSONValue *pJSONValue = readAndParseJSONFromString(JSON_string, error_message);
	if (pJSONValue == 0) {
		bResult = false;
		return 0;
	} else {
		EmdrosFTSStringFilter *pResult = 
			stringFilterFactoryJSONValue(pJSONValue, bResult, error_message, filter_name);
		delete pJSONValue;
		return pResult;
	}
}


EmdrosFTSStringFilter *stringFilterFactoryJSONFileName(const std::string& JSON_filename, bool& bResult, std::string& error_message, const std::string& filter_name)
{
	JSONValue *pJSONValue = readAndParseJSONFromFile(JSON_filename, error_message);
	if (pJSONValue == 0) {
		bResult = false;
		return 0;
	} else {
		EmdrosFTSStringFilter *pResult = 
			stringFilterFactoryJSONValue(pJSONValue, bResult, error_message, filter_name);
		delete pJSONValue;
		return pResult;
	}
}

///////////////////////////////////////////////////////////////////////
//
// EmdrosFTSStringFilter
//
// The identity filter which always returns with bUseThisValue == true
//
// You should derive a descendant class from this and use your
// descendant class instead.
//
//////////////////////////////////////////////////////////////////////////
EmdrosFTSStringFilter::EmdrosFTSStringFilter()
	: m_pNext(0)
{
}

EmdrosFTSStringFilter::~EmdrosFTSStringFilter()
{
	delete m_pNext;
}

std::string EmdrosFTSStringFilter::process(const std::string& input_string, bool& /* out */ bUseThisValue)
{
	std::string result = process_self(input_string, bUseThisValue);
	if (!bUseThisValue) {
		return result;
	} else {
		if (m_pNext == 0) {
			return result;
		} else {
			return m_pNext->process(result, bUseThisValue);
		}
	}
}

std::string EmdrosFTSStringFilter::process_self(const std::string& input_string, bool& /* out */ bUseThisValue)
{
	bUseThisValue = true;
	return input_string;
}

void EmdrosFTSStringFilter::setNext(EmdrosFTSStringFilter *pNext)
{
	m_pNext = pNext;
}




//////////////////////////////////////////////////////////////////////////
//
// SFStripChars
//
// An EmdrosFTSStringFilter descendant which strips certain characters
// on either side of the input string.
//
//////////////////////////////////////////////////////////////////////////
SFStripChars::SFStripChars()
	: EmdrosFTSStringFilter()
{
	m_chars_to_strip = " \r\n\v\t";
}


SFStripChars::SFStripChars(const std::string& chars_to_strip)
	: EmdrosFTSStringFilter()
{
	m_chars_to_strip = chars_to_strip;
}


SFStripChars::~SFStripChars()
{
	// Nothing to do beyond what is done in ancestor.
}


std::string SFStripChars::process_self(const std::string& input_string, bool& /* out */ bUseThisValue)
{
	bUseThisValue = true;
	std::string result = strip(input_string, m_chars_to_strip);
	return result;
}



//////////////////////////////////////////////////////////////////////////
//
// SFReplaceString
//
// An EmdrosFTSStringFilter descendant which replaces one substring
// with another.
//
//////////////////////////////////////////////////////////////////////////
SFReplaceString::SFReplaceString(const std::string& string_to_replace, const std::string& replacement)
	: EmdrosFTSStringFilter(),
	  m_string_to_replace(string_to_replace),
	  m_replacement(replacement)
{
	m_bIsNoOp = m_string_to_replace.empty() || (m_string_to_replace == m_replacement);
}

SFReplaceString::~SFReplaceString()
{
	// Nothing to do beyond what is done in ancestor.
}


std::string SFReplaceString::process_self(const std::string& input_string, bool& /* out */ bUseThisValue)
{
	bUseThisValue = true;
	if (m_bIsNoOp) {
		return input_string;
	} else {
		std::string result = replace_substring(input_string, m_string_to_replace, m_replacement);
		return result;
	}
}





//////////////////////////////////////////////////////////////////////////
//
// SFLowerCase
//
// An EmdrosFTSStringFilter descendant which strips certain characters
// on either side of the input string.
//
//////////////////////////////////////////////////////////////////////////
SFLowerCase::SFLowerCase()
	: EmdrosFTSStringFilter()
{
}



SFLowerCase::~SFLowerCase()
{
	// Nothing to do beyond what is done in ancestor.
}


std::string SFLowerCase::process_self(const std::string& input_string, bool& /* out */ bUseThisValue)
{
	bUseThisValue = true;
	std::string result;
	str_tolower(input_string, result);
	return result;
}



//////////////////////////////////////////////////////////////////////////
//
// SFUpperCase
//
// An EmdrosFTSStringFilter descendant which makes all strings
// upper-case (according to toupper()), and passes all strings through
// with bUseThisValue == true.  Note that this doesn't work with UTF-8
// in general!
//
//////////////////////////////////////////////////////////////////////////
SFUpperCase::SFUpperCase()
	: EmdrosFTSStringFilter()
{
}



SFUpperCase::~SFUpperCase()
{
	// Nothing to do beyond what is done in ancestor.
}


std::string SFUpperCase::process_self(const std::string& input_string, bool& /* out */ bUseThisValue)
{
	bUseThisValue = true;
	std::string result;
	str_toupper(input_string, result);
	return result;
}


//////////////////////////////////////////////////////////////////////////
//
// SFBlockEmpty
//
//
//////////////////////////////////////////////////////////////////////////
SFBlockEmpty::SFBlockEmpty()
	: EmdrosFTSStringFilter()
{
}



SFBlockEmpty::~SFBlockEmpty()
{
	// Nothing to do beyond what is done in ancestor.
}


std::string SFBlockEmpty::process_self(const std::string& input_string, bool& /* out */ bUseThisValue)
{
	if (input_string.empty()) {
		bUseThisValue = false;
	} else {
		bUseThisValue = true;
	}
	return input_string;
}

//////////////////////////////////////////////////////////////////////////
//
// SFStopWords
//
//
//////////////////////////////////////////////////////////////////////////
SFStopWords::SFStopWords(const StringList& stop_word_list)
	: EmdrosFTSStringFilter()
{
	StringListConstIterator ci = stop_word_list.const_iterator();
	while (ci.hasNext()) {
		std::string s = ci.next();
		m_stop_word_set.insert(s);
	}
}



SFStopWords::~SFStopWords()
{
	// Nothing to do beyond what is done in ancestor.
}


std::string SFStopWords::process_self(const std::string& input_string, bool& /* out */ bUseThisValue)
{
	if (m_stop_word_set.find(input_string) != m_stop_word_set.end()) {
		bUseThisValue = false;
	} else {
		bUseThisValue = true;
	}
	return input_string;
}





//////////////////////////////////////////////////////////////////////////
//
// BookcaseIndexer
//
// The class which does the indexing.
//
//////////////////////////////////////////////////////////////////////////
BookcaseIndexer::BookcaseIndexer(EmdrosEnv *pSourceEnv, EmdrosEnv *pTargetEnv, std::ostream *pMQLOutStream, const std::string& bookcase_OTN, const std::string& indexed_OTN, const std::string& indexed_feature_name_list, EmdrosFTSStringFilter *pStringFilter)
	: m_pSourceEnv(pSourceEnv),
	  m_pTargetEnv(pTargetEnv),
	  m_pMQLOutStream(pMQLOutStream),
	  m_bookcase_OTN(bookcase_OTN),
	  m_indexed_OTN(indexed_OTN),
	  m_pStringFilter(pStringFilter)
{
	process_comma_separated_string_list(indexed_feature_name_list, m_indexed_feature_name_list);
}

BookcaseIndexer::~BookcaseIndexer()
{
	clear_query_cache();
}


bool BookcaseIndexer::process(const SetOfMonads& substrate, bool bEmitVacuum)
{
	// Make sure we are pristine in the query cache.
	clear_query_cache();

	if (m_pSourceEnv == 0) {
		appendError("Source Environment's is nil. Cannot continue.");
		return false;
	}

	if (!(m_pSourceEnv->connectionOk())) {
		appendError("Source Environment's connection is not OK.");
		return false;
	}

	if (m_bookcase_OTN == "") {
		appendError("bookcase_OTN is empty. Cannot continue.");
		return false;
	}

	if (m_indexed_OTN == "") {
		appendError("indexed_OTN is empty. Cannot continue.");
		return false;
	}

	if (m_indexed_feature_name_list.empty()) {
		appendError("indexed_feature_name is empty. Cannot continue.");
		return false;
	}

	m_index_OTN = make_index_name("_index", m_bookcase_OTN, m_indexed_OTN, m_indexed_feature_name_list);

	emit(std::string("DROP OBJECT TYPE [") + m_index_OTN + "]");
	go(); // Ignore the error if the object type did not exist.
	errorMessage(); // Clear error; ignore error message.

	eObjectRangeType objectRangeType;
	bool bObjectTypeExists;
	id_d_t object_type_id;
	eMonadUniquenessType monadUniquenessType;
	if (!m_pSourceEnv->getMQLEE()->pDB->objectTypeExists(m_bookcase_OTN,
							     bObjectTypeExists,
							     object_type_id,
							     objectRangeType,
							     monadUniquenessType)) {
		appendError("Bookcase object type '" + m_bookcase_OTN + "' does not exist!");
		return false;
	}

	std::string range_type_string;
	switch (objectRangeType) {
	case kORTMultipleRange:
		range_type_string = "WITH MULTIPLE RANGE OBJECTS";
		break;
	case kORTSingleRange: 
		range_type_string = "WITH SINGLE RANGE OBJECTS";
		break;
	case kORTSingleMonad:
		range_type_string = "WITH SINGLE MONAD OBJECTS";
		break;
	default:
		ASSERT_THROW(false,
			     "Unknown range type in BookcaseIndexer::process().");
		return false;
	}


	emit(std::string("CREATE OBJECT TYPE ") + range_type_string + " [" + m_index_OTN + " feature_value : STRING WITH INDEX; monad_set : STRING;]");
	if (!go()) {
		appendError("Could not create object type: " + m_index_OTN);
		return false;
	}

	emit("DROP INDEXES ON OBJECT TYPE [" + m_index_OTN + "]");
	if (!go()) {
		appendError("Could not drop indexes on object type: " + m_index_OTN);
		return false;
	}

	emit("BEGIN TRANSACTION");
	if (!go()) {
		appendError("Could not begin transaction");
		return false;
	}

	SetOfMonads substrate_som;
	if (substrate.isEmpty()) {
		substrate_som.add(1, MAX_MONAD);
	} else {
		substrate_som = substrate;
	}

	Sheaf *pBookcase_sheaf = 0;
	int count = 0;
	bool bEmitFinalGO = false;
	try {
		emit("CREATE OBJECTS WITH OBJECT TYPE [" + m_index_OTN + "]");

		std::string query = "SELECT ALL OBJECTS IN " + substrate_som.toString() + " WHERE [" + m_bookcase_OTN + "]GO";

		if (!execQueryOnSource(query)) {
			appendError("Could not get all bookcase objects in substrate\nby executing the following query:\n" + query);
			return false;
		}
                
                pBookcase_sheaf = m_pSourceEnv->takeOverSheaf();

                SheafConstIterator bookcase_sheaf_ci = pBookcase_sheaf->const_iterator();
		while (bookcase_sheaf_ci.hasNext()) {
			const Straw *pBookcaseStraw = bookcase_sheaf_ci.next();
			
			StrawConstIterator bookcase_straw_ci = pBookcaseStraw->const_iterator();
			while (bookcase_straw_ci.hasNext()) {
				const MatchedObject *pMO = bookcase_straw_ci.next();
				
				SetOfMonads mo_som = pMO->getMonads();

				if (!process_bookcase(mo_som)) {
					appendError("Could not process bookcase with monads:" + mo_som.toString());
					throw EmdrosException("Could not process bookcase with monads:" + mo_som.toString());
				} else {
					bEmitFinalGO = true;
				}
				
				++count;
				
				if ((count % 100) == 0) {
					bEmitFinalGO = false;

					if (!go()) {
						appendError("Could not process 'go' for bookcase with monads:" + mo_som.toString());
						throw EmdrosException("Could not process 'go' for bookcase with monads:" + mo_som.toString());
					} else {
						emit("CREATE OBJECTS WITH OBJECT TYPE [" + m_index_OTN + "]");
					}
				}
			}
		}
	} catch (EmdrosException& e) {
		std::cerr << e.what() << std::endl;
	} catch (...) {
		
	}

	delete pBookcase_sheaf;

	if (bEmitFinalGO) {
		if (!go()) {
			appendError("Could not emit final 'GO'");
			return false;
		}
	}

	emit("COMMIT TRANSACTION");
	if (!go()) {
		appendError("Could not commit transaction");
		return false;
	}

	emit("CREATE INDEXES ON OBJECT TYPE [" + m_index_OTN + "]");
	if (!go()) {
		appendError("Could not create indexes on object type: " + m_index_OTN);
		return false;
	}
	
	if (bEmitVacuum) {
		emit("VACUUM DATABASE ANALYZE");
		if (!go()) {
			appendError("Could not vacuum database at the end (though everything else went fine!)");
			return false;
		}
	}

	return true;
}


std::string BookcaseIndexer::errorMessage(void)
{
	std::string result = m_error_message;
	m_error_message.clear();
	return result;
}


bool BookcaseIndexer::process_bookcase(const SetOfMonads& bookcase_som)
{
	std::string query = "SELECT ALL OBJECTS IN " + bookcase_som.toString() + " WHERE [" + m_indexed_OTN + " GET " + joinList(",", m_indexed_feature_name_list, 4096) + "]";

	std::string object_query_prefix = "CREATE OBJECT FROM MONADS=" + bookcase_som.toString() + "[feature_value:=\"";

        Sheaf *pIndexedOTSheaf = 0;

	try {
		if (!execQueryOnSource(query)) {
			appendError("Could not get objects by executing query:\n" + query);
			return false;
		}

		typedef std::map<std::string, SetOfMonads*> String2PSOMMap;
		String2PSOMMap mymap;

		pIndexedOTSheaf = m_pSourceEnv->takeOverSheaf();
		
		if (pIndexedOTSheaf == 0) {
			appendError("Sheaf was nil when getting objects by executing query:\n" + query);
			return false;
		}
		

		unsigned int no_of_indexed_feature_names = (unsigned int) m_indexed_feature_name_list.size();
		unsigned int i;
		SheafConstIterator indexed_OT_sheaf_ci = pIndexedOTSheaf->const_iterator();

		while (indexed_OT_sheaf_ci.hasNext()) {
			const Straw *pIndexedOTStraw = indexed_OT_sheaf_ci.next();

			StrawConstIterator indexed_OT_straw_ci = pIndexedOTStraw->const_iterator();
			while (indexed_OT_straw_ci.hasNext()) {
				const MatchedObject *pMO = indexed_OT_straw_ci.next();

				SetOfMonads mo_som = pMO->getMonads();

				std::set<std::string> feature_value_set;
				
				for (i = 0;
				     i < no_of_indexed_feature_names;
				     ++i) {
					std::string feature_value = pMO->getFeatureAsString(i);

					bool bUseValue = true;
					if (m_pStringFilter) {
						feature_value = m_pStringFilter->process(feature_value, bUseValue);
					}


					if (bUseValue) {
						feature_value_set.insert(feature_value);
					}

				}

				if (!feature_value_set.empty()) {
					std::set<std::string>::const_iterator set_ci = feature_value_set.begin();
					while (set_ci != feature_value_set.end()) {
						std::string feature_value = *set_ci++;
						if (mymap.find(feature_value) == mymap.end()) {
							mymap.insert(std::make_pair(feature_value, new SetOfMonads()));
						}
					
						// Add mo_som to SetOfMonads for feature_value
						mymap[feature_value]->unionWith(mo_som);
					}
				}
			}
		}

		delete pIndexedOTSheaf;
		pIndexedOTSheaf = 0;

		String2PSOMMap::const_iterator map_ci = mymap.begin();
		while (map_ci != mymap.end()) {
			std::string feature_value = map_ci->first;
			const SetOfMonads *pSOM = map_ci->second;

			emit(object_query_prefix + encodeSTRINGstring(feature_value) + "\";monad_set:=\"" + encodeSTRINGstring(pSOM->toCompactString()) + "\";]");
			
			++map_ci;
		}

		// Clear mymap, deleting all SetOfMonad objects.
		String2PSOMMap::iterator map_i = mymap.begin();
		while (map_i != mymap.end()) {
			SetOfMonads *pSOM = map_i->second;
			
			delete pSOM;
			
			++map_i;
		}
		mymap.clear();
	} catch (...) {
		; // Do nothing
	}

	// This is safe, even if we have deleted it already,
	// since we set it to nil when we do.
	delete pIndexedOTSheaf;

	return true;
}


void BookcaseIndexer::emit(const std::string& query)
{
	m_query_cache.push_back(query);
}


bool BookcaseIndexer::go(void)
{
	bool bResult = true;

	// Emit "\nGO"
	emit("\nGO\n");

	// Get query
	std::string query = joinList("\n", m_query_cache);

	clear_query_cache();

	// Emit to MQLOutputStream if present
	if (m_pMQLOutStream) {
		(*m_pMQLOutStream) << query;
	}

	// Emit to m_pTargetEnv if present
	if (m_pTargetEnv != 0) {
		if (!(m_pTargetEnv->connectionOk())) {
			appendError("Target Environment's connection is not OK.");
			bResult = false;
		} else {
			bool bCompilerOK = true;

			if (!(m_pTargetEnv->executeString(query, bCompilerOK, false, false))) {
				appendError("Target Environment DB error:");
				appendError("----------------------------");
				appendError(m_pTargetEnv->getDBError());
				appendError("----------------------------");
				bResult = false;
			} 
			if (!bCompilerOK) {
				appendError("Target Environment Compiler error:");
				appendError("----------------------------");
				appendError(m_pTargetEnv->getCompilerError());
				appendError("----------------------------");
				bResult = false;
			}
		}
	}

	return bResult;
}


void BookcaseIndexer::clear_query_cache()
{
	m_query_cache.clear();
}


void BookcaseIndexer::appendError(const std::string& message)
{
	m_error_message += message + "\n";
}


bool BookcaseIndexer::execQueryOnSource(const std::string query)
{
	bool bResult = true;

	bool bCompilerOK = true;
		
	if (!(m_pSourceEnv->executeString(query, bCompilerOK, false, false))) {
		appendError("Source Environment DB error:");
		appendError("----------------------------");
		appendError(m_pSourceEnv->getDBError());
		appendError("----------------------------");
		bResult = false;
	} 
	if (!bCompilerOK) {
		appendError("Source Environment Compiler error:");
		appendError("----------------------------");
		appendError(m_pSourceEnv->getCompilerError());
		appendError("----------------------------");
		bResult = false;
	}
	return bResult;
}


///////////////////////////////////////////////////////////////////////
//
// BookcaseHit
//
//////////////////////////////////////////////////////////////////////////
BookcaseHit::BookcaseHit(monad_m bookcase_fm, monad_m bookcase_lm, monad_m hit_fm)
	: m_bookcase_fm(bookcase_fm),
	  m_bookcase_lm(bookcase_lm),
	  m_hit_fm(hit_fm)
{
}

BookcaseHit::~BookcaseHit()
{
}

BookcaseHit::BookcaseHit(const BookcaseHit& other)
{
	assign(other);
}


BookcaseHit& BookcaseHit::operator=(const BookcaseHit& other)
{
	assign(other);
	return *this;
}

void BookcaseHit::assign(const BookcaseHit& other)
{
	m_bookcase_fm = other.m_bookcase_fm;
	m_bookcase_lm = other.m_bookcase_lm;
	m_hit_fm = other.m_hit_fm;
}


///////////////////////////////////////////////////////////////////////
//
// BookcaseHitListConstIterator
//
//////////////////////////////////////////////////////////////////////////
BookcaseHitListConstIterator::BookcaseHitListConstIterator()
{
	m_pMother = 0;
	m_max_visit_count = -1; // Don't count visits.
}


BookcaseHitListConstIterator::BookcaseHitListConstIterator(const BookcaseHitList* pMotherHitList)
{
	m_max_visit_count = -1; // Don't count visits.
	m_pMother = pMotherHitList;
	if (m_pMother != 0) {
		m_bookcase_ci = m_pMother->m_hits.begin();
		if (m_bookcase_ci != m_pMother->m_hits.end()) {
			m_fm_ci = m_bookcase_ci->second.begin();
		}
	}
}

BookcaseHitListConstIterator::BookcaseHitListConstIterator(const BookcaseHitList* pMotherHitList, int OFFSET, int LIMIT)
{
	m_pMother = pMotherHitList;
	if (m_pMother != 0) {
		m_bookcase_ci = m_pMother->m_hits.begin();
		if (m_bookcase_ci != m_pMother->m_hits.end()) {
			m_fm_ci = m_bookcase_ci->second.begin();

			// Count OFFSET next's into the structure.
			m_max_visit_count = OFFSET; 
			
			while (hasNext()) {
				this->next();
			}
		} 
	}

	// Make sure we don't do more than LIMIT
	m_max_visit_count = LIMIT;
}


BookcaseHitListConstIterator::BookcaseHitListConstIterator(const BookcaseHitListConstIterator& other)
{
	assign(other);
}


BookcaseHitListConstIterator::~BookcaseHitListConstIterator()
{
	// Nothing to do. We don't own anything
}


bool BookcaseHitListConstIterator::hasNext() const
{
	if (m_pMother == 0) {
		return false;
	} else {
		if (m_bookcase_ci == m_pMother->m_hits.end()
		    || m_max_visit_count == 0) {
			return false;
		} else {
			return true;
		}
	}
}

 //  Is the iterator == end iterator?  Doesn't alter iterator
BookcaseHit BookcaseHitListConstIterator::next()
{
	BookcaseHit result = current();
	++m_fm_ci;
	if (m_fm_ci == m_bookcase_ci->second.end()) {
		++m_bookcase_ci;
		if (m_bookcase_ci != m_pMother->m_hits.end()) {
			m_fm_ci = m_bookcase_ci->second.begin();
		}
	}
	if (m_max_visit_count > 0) {
		--m_max_visit_count;
	}
	return result;
}

 // Gets the current and advances the iterator
BookcaseHit BookcaseHitListConstIterator::current()
{
	ASSERT_THROW(hasNext(), "BookcaseHitListConstIterator::current(): There are no more hits.");
	monad_m bookcase_fm = m_bookcase_ci->first.first;
	monad_m bookcase_lm = m_bookcase_ci->first.second;
	monad_m hit_fm = *m_fm_ci;
	return BookcaseHit(bookcase_fm, bookcase_lm, hit_fm);
}

 // Gets the current without altering iterator
BookcaseHitListConstIterator& BookcaseHitListConstIterator::operator=(const BookcaseHitListConstIterator& other)
{
	assign(other);
	return *this;
}


void BookcaseHitListConstIterator::assign(const BookcaseHitListConstIterator& other)
{
	m_pMother = other.m_pMother;
	m_bookcase_ci = other.m_bookcase_ci;
	m_fm_ci = other.m_fm_ci;
	m_max_visit_count = other.m_max_visit_count;
}


///////////////////////////////////////////////////////////////////////
//
// BookcaseHitList
//
//////////////////////////////////////////////////////////////////////////
BookcaseHitList::BookcaseHitList()
{
	m_hit_count = 0;
	setMaxHitsPerPage(10);
}

BookcaseHitList::BookcaseHitList(const BookcaseHitList& other)
{
	assign(other);
}

BookcaseHitList::~BookcaseHitList()
{
}

BookcaseHitList& BookcaseHitList::operator=(const BookcaseHitList& other)
{
	assign(other);
	return *this;
}

int BookcaseHitList::getHitCount()
{
	return m_hit_count;
}

int BookcaseHitList::getBookcaseCount()
{
	return (int) m_hits.size();
}

bool BookcaseHitList::empty() const
{
	return m_hits.empty();
}

void BookcaseHitList::clearHitMap()
{
	m_hits.clear();
	m_hit_count = 0;
}

void BookcaseHitList::assign(const BookcaseHitList& other)
{
	m_hits = other.m_hits;
	m_hit_count = other.m_hit_count;
	m_max_hits_per_page = other.m_max_hits_per_page;
	m_tokens = other.m_tokens;
	m_monads_before_hit_word = other.m_monads_before_hit_word;
	m_monads_after_hit_word = other.m_monads_after_hit_word;
	m_bookcase_lm2title_map = other.m_bookcase_lm2title_map;
}

int BookcaseHitList::getPageCount()
{
	if (m_max_hits_per_page <= 0) {
		return 1;
	} else if (m_hit_count == 0) {
		return 0;
	} else {
		int remainder = m_hit_count % m_max_hits_per_page;
		int page_count = m_hit_count / m_max_hits_per_page;
		if (remainder != 0) {
			++page_count;
		}
		return page_count;
	}
}


BookcaseHitListConstIterator BookcaseHitList::const_iterator()
{
	return BookcaseHitListConstIterator(this);
}

BookcaseHitListConstIterator BookcaseHitList::const_iterator(int page_number)
{
	if (m_max_hits_per_page <= 0) {
		return this->const_iterator();
	} else if (page_number < 1) {
		return this->const_iterator();
	} else {
		return BookcaseHitListConstIterator(this, (page_number - 1) * m_max_hits_per_page, m_max_hits_per_page);
	}
}


BookcaseHitList BookcaseHitList::removeThoseNotPresentInOther(const BookcaseHitList& other, monad_m proximity_length_backward, monad_m proximity_length_forward)
{
	BookcaseHitList new_hit_list;
	HitMap::const_iterator bookcase_ci = m_hits.begin();
	HitMap::const_iterator bookcase_cend = m_hits.end();
	while (bookcase_ci != bookcase_cend) {
		monad_m bookcase_fm = bookcase_ci->first.first;
		monad_m bookcase_lm = bookcase_ci->first.second;
		HitMap::const_iterator other_ci = other.m_hits.find(std::pair<monad_m, monad_m>(bookcase_fm, bookcase_lm));
		HitMap::const_iterator other_cend = other.m_hits.end();
		if (other_ci != other_cend) {
			// this bookcase is also in other, so peruse it.
			std::set<monad_m>::const_iterator set_ci = bookcase_ci->second.begin();
			std::set<monad_m>::const_iterator set_cend = bookcase_ci->second.end();
			while (set_ci != set_cend) {
				monad_m m = *set_ci;

				monad_m start_m = MYMAX(m - proximity_length_backward, bookcase_fm);
				monad_m end_m = MYMIN(m + proximity_length_forward, bookcase_lm);
				std::set<monad_m>::const_iterator lower_bound_it = other_ci->second.lower_bound(start_m);
				if (lower_bound_it != other_ci->second.end()) {
					monad_m lower_bound = *lower_bound_it;
					if (lower_bound <= end_m) {
						new_hit_list.addHit(bookcase_fm, bookcase_lm, m);
					} 
				}
				
				++set_ci;
			}
		}
		++bookcase_ci;
	}

	/*
	BookcaseHitListConstIterator my_ci = this->const_iterator();
	while (my_ci.hasNext()) {
		BookcaseHit myhit(my_ci.next());
		if (other.hasHit(myhit, proximity_length)) {
			new_hit_list.addHit(myhit.m_bookcase_fm, myhit.m_bookcase_lm, myhit.m_hit_fm);
		}
	}
	*/
	return new_hit_list;
}

BookcaseHitList BookcaseHitList::removeAllButFirstHitInEachBookcase()
{
	BookcaseHitList new_hit_list;
	HitMap::const_iterator bookcase_ci = m_hits.begin();
	HitMap::const_iterator bookcase_cend = m_hits.end();
	while (bookcase_ci != bookcase_cend) {
		monad_m bookcase_fm = bookcase_ci->first.first;
		monad_m bookcase_lm = bookcase_ci->first.second;
		monad_m hit_fm = *(bookcase_ci->second.begin());
		new_hit_list.addHit(bookcase_fm, bookcase_lm, hit_fm);
		++bookcase_ci;
	}

	return new_hit_list;
}

void BookcaseHitList::merge(const BookcaseHitList& other)
{
	HitMap::const_iterator ci = other.m_hits.begin();
	HitMap::const_iterator cend = other.m_hits.end();
	while (ci != cend) {
		monad_m other_fm_bookcase = ci->first.first;
		monad_m other_lm_bookcase = ci->first.second;

		std::set<monad_m>::const_iterator ci2 = ci->second.begin();
		std::set<monad_m>::const_iterator ci2end = ci->second.end();
		while (ci2 != ci2end) {
			monad_m other_m = *ci2;

			addHit(other_fm_bookcase, other_lm_bookcase, other_m);

			++ci2;
		}
		
		++ci;
	}
}


void BookcaseHitList::merge_range(const BookcaseHitList& other, int token_range, bool bDirectionMatters)
{
        BookcaseHitList result_hit_list(*this);
	result_hit_list.clearHitMap();

	HitMap::const_iterator ci = other.m_hits.begin();
	HitMap::const_iterator ciend = other.m_hits.end();
	while (ci != ciend) {
		monad_m other_fm_bookcase = ci->first.first;
		monad_m other_lm_bookcase = ci->first.second;
		HitMap::const_iterator ci3 = this->m_hits.find(std::make_pair(other_fm_bookcase, other_lm_bookcase));
		if (ci3 == this->m_hits.end()) {
			; // Nothing to do for this bookcase
		} else {
			std::set<monad_m>::const_iterator ci2 = ci->second.begin();
			std::set<monad_m>::const_iterator ci2end = ci->second.end();
			while (ci2 != ci2end) {
				monad_m other_m = *ci2;
				
				std::set<monad_m>::const_iterator ci4 = ci3->second.begin();
				std::set<monad_m>::const_iterator ci4end = ci3->second.end();
				while (ci4 != ci4end) {
					monad_m my_m = *ci4;
					
					monad_m difference = other_m - my_m;

					bool bDoIt = true;

					if (bDirectionMatters) {
						bDoIt = (MYSIGN(difference) == MYSIGN(token_range)) && (MYABS(difference) <= token_range);
					} else {
						bDoIt = MYABS(difference) <= MYABS(token_range);
					}
					
					if (bDoIt) {
						result_hit_list.addHit(other_fm_bookcase, other_lm_bookcase, my_m);
						result_hit_list.addHit(other_fm_bookcase, other_lm_bookcase, other_m);
					}
					++ci4;
				}
				
				++ci2;
			}
		}
		
		++ci;
	}

        this->assign(result_hit_list);
}



void BookcaseHitList::filter_both(BookcaseHitList& other)
{
	if (getBookcaseCount() < other.getBookcaseCount()) {
		BookcaseHitList::filter(*this, other);
		BookcaseHitList::filter(other, *this);
	} else {
		BookcaseHitList::filter(other, *this);
		BookcaseHitList::filter(*this, other);
	}
}



void BookcaseHitList::filter(BookcaseHitList& hl1, const BookcaseHitList& hl2)
{
	BookcaseHitList result_hit_list(hl1);
	result_hit_list.clearHitMap();

	HitMap::const_iterator ci = hl1.m_hits.begin();
	HitMap::const_iterator ciend = hl1.m_hits.end();
	while (ci != ciend) {
		monad_m hl1_fm_bookcase = ci->first.first;
		monad_m hl1_lm_bookcase = ci->first.second;
		
		HitMap::const_iterator ci3 = hl2.m_hits.find(std::make_pair(hl1_fm_bookcase, hl1_lm_bookcase));
		if (ci3 == hl2.m_hits.end()) {
			; // Nothing to do for this bookcase
		} else {
			std::set<monad_m>::const_iterator ci4 = ci->second.begin();
			std::set<monad_m>::const_iterator ci4end = ci->second.end();
			while (ci4 != ci4end) {
				monad_m hl1_m = *ci4;

				result_hit_list.addHit(hl1_fm_bookcase, hl1_lm_bookcase, hl1_m);
				
				++ci4;
			}
		}
		++ci;
	}

	hl1.assign(result_hit_list);
}



bool BookcaseHitList::hasHit(monad_m bookcase_fm, monad_m bookcase_lm, monad_m hit_fm, monad_m proximity_length) const
{
	HitMap::const_iterator ci = m_hits.find(std::pair<monad_m, monad_m>(bookcase_fm, bookcase_lm));
	if (ci == m_hits.end()) {
		return false;
	} else {
		monad_m start_m = MYMAX(hit_fm - proximity_length, bookcase_fm);
		monad_m end_m = MYMIN(hit_fm + proximity_length, bookcase_lm);
		std::set<monad_m>::const_iterator lower_bound_it = ci->second.lower_bound(start_m);
		if (lower_bound_it == ci->second.end()) {
			return false;
		} else {
			monad_m lower_bound = *lower_bound_it;
			if (lower_bound <= end_m) {
				return true;
			} else {
				return false;
			}
		}
	}
}


void BookcaseHitList::addHit(monad_m bookcase_fm, monad_m bookcase_lm, monad_m hit_fm)
{
	HitMap::iterator it = m_hits.find(std::make_pair(bookcase_fm, bookcase_lm));
	if (it == m_hits.end()) {
		m_hits.insert(std::make_pair(std::make_pair(bookcase_fm, bookcase_lm), std::set<monad_m>()));
		it = m_hits.find(std::make_pair(bookcase_fm, bookcase_lm));
	}
	it->second.insert(hit_fm);
	++m_hit_count;
}


/** Convert a BookcaseHit to a snippet.
 *
 * The following are replaced with their monad-values represented in
 * base 10:
 *
 * %bcfm : hit.m_bookcase_fm
 * %bclm : hit.m_bookcase_lm
 * %hm : hit.m_hit_fm
 * %tm : The monad of the current token in the snippet
 *
 */
void BookcaseHitList::getSnippet(const BookcaseHit& hit, const std::string& hit_token_prefix, const std::string& hit_token_suffix, const std::string& hit_token_infix, bool bStripToken, std::string& snippet, std::string& snippet_title)
{
	if (m_tokens.size() == 0) {
		snippet = "";
		snippet_title = "";
	}

	bool bUseHitTokenInfix = !hit_token_infix.empty();

	// bool bUseFilteredTokens = !m_token2filtered_token.empty();

	monad_m start_m = MYMAX(hit.m_bookcase_fm, hit.m_hit_fm - m_monads_before_hit_word);
	monad_m end_m = MYMIN(hit.m_bookcase_lm, hit.m_hit_fm + m_monads_after_hit_word);
	std::list<std::string> snippet_list;
	//std::set<std::string>::const_iterator query_terms_cend = m_query_terms.end();
	for (monad_m m = start_m;
	     m <= end_m;
	     ++m) {
		const std::string& token = m_tokens[m];
		/*
		std::string possibly_filtered_token;
		if (bUseFilteredTokens) {
			possibly_filtered_token = m_token2filtered_token[token];
		} else {
			possibly_filtered_token = token;
		}
		if (m_query_terms.find(possibly_filtered_token) != query_terms_cend) {
		*/
		if (hit.m_hit_fm == m) {
			snippet_list.push_back(replace_templates(hit_token_prefix, hit, m));
			if (bStripToken) {
				snippet_list.push_back(strip(token));
			} else {
				snippet_list.push_back(token);
			}
			snippet_list.push_back(hit_token_suffix);
			if (bUseHitTokenInfix) {
				snippet_list.push_back(hit_token_infix);
			}
		} else {
			if (bStripToken) {
				snippet_list.push_back(strip(token));
			} else {
				snippet_list.push_back(token);
			}
			if (bUseHitTokenInfix) {
				snippet_list.push_back(hit_token_infix);
			}
		}
	}
	
	snippet = strip(joinListEmptyBetween(snippet_list));
	
	HarvestBookcaseMap::iterator bookcase_it = m_bookcase_lm2title_map.find(hit.m_bookcase_lm);
	if (bookcase_it != m_bookcase_lm2title_map.end()) {
		snippet_title = bookcase_it->second;
	} else {
		snippet_title = "";
	}
}

std::string BookcaseHitList::replace_templates(const std::string& instring, const BookcaseHit& hit, monad_m token_monad)
{
	if (instring.find('%') == std::string::npos) {
		return instring;
	} else {
		std::string tmp = replace_substring(instring, "%bcfm", monad_m2string(hit.m_bookcase_fm));
		tmp = replace_substring(tmp, "%bclm", monad_m2string(hit.m_bookcase_lm));
		tmp = replace_substring(tmp, "%hm", monad_m2string(hit.m_hit_fm));
		return replace_substring(tmp, "%tm", monad_m2string(token_monad));
	}
}

bool BookcaseHitList::harvestHits(EmdrosEnv *pEnv, const std::string& bookcase_OTN, const std::string& bookcase_title_OTN, const std::string& bookcase_title_feature_name, const std::string& token_OTN, const std::string& token_feature_name, monad_m monads_before_hit_word, monad_m monads_after_hit_word, const std::string& query, EmdrosFTSStringFilter *pStringFilterList, int page_number)
{
	UNUSED(query);
	UNUSED(bookcase_OTN);
	UNUSED(pStringFilterList); // FIXME: Find out why this is not
				   // used any more, and document it.
	
	m_monads_before_hit_word = monads_before_hit_word;
	m_monads_after_hit_word = monads_after_hit_word;

	m_tokens.clear();
	/*
	m_query_terms.clear();

	if (!query.empty()) {
		std::string tmp_query = replace_substring(query, "\"", " ");
		StringList token_list = harvestFTSTokenize(tmp_query, pStringFilterList);
		StringListConstIterator token_list_ci = token_list.const_iterator();
		while (token_list_ci.hasNext()) {
			m_query_terms.insert(token_list_ci.next());
		}
	}
	*/

	//
	// Build grandSOM as the big-union of all "relevant" monads,
	// i.e., the ranges [m_hit_fm) - m_monads_before_hit_word, m_hit_fm + m_monads_after_hit_word]
	// intersected with [m_bookcase_fm, m_bookcase_lm].
	//
	SetOfMonads grandSOM;
	BookcaseHitListConstIterator my_ci;
	if (page_number < 1) {
		my_ci = this->const_iterator();
	} else {
		my_ci = this->const_iterator(page_number);
	}
	monad_m prev_end_m = 0;
	while (my_ci.hasNext()) {
		BookcaseHit myhit(my_ci.next());
		monad_m start_m = MYMAX(myhit.m_bookcase_fm, myhit.m_hit_fm - m_monads_before_hit_word);
		monad_m end_m = MYMIN(myhit.m_bookcase_lm, myhit.m_hit_fm + m_monads_after_hit_word);
		if (end_m > prev_end_m) {
			grandSOM.add(start_m, end_m);
			prev_end_m = end_m;
		}
	}

	// If results are empty, we report success now.
	if (grandSOM.isEmpty()) {
		return true;
	}

	std::string MQL_query = "GET OBJECTS HAVING MONADS IN " + grandSOM.toString() + "[" + token_OTN + " GET " + token_feature_name + "]GO";
	FlatSheaf *pIndex_sheaf = 0;
	bool bResult = true;
	
	if (!pEnv->executeString(MQL_query,
				 bResult,
				 false, // bPrintResult
				 false  // bReportError
				 )) {
		bResult = false;
		return false;
	}
	if (!bResult) {
		return false;
	}
	
	pIndex_sheaf = pEnv->takeOverFlatSheaf();
	
	FlatSheafConstIterator index_sheaf_ci = pIndex_sheaf->const_iterator();
	
	while (index_sheaf_ci.hasNext()) {
		const FlatStraw *pIndex_straw = index_sheaf_ci.next();
		FlatStrawConstIterator index_straw_ci = pIndex_straw->const_iterator();
		while (index_straw_ci.hasNext()) {
			const MatchedObject *pIndex_mo = index_straw_ci.next();
			monad_m first_monad = pIndex_mo->getFirst();
			std::string feature_value = pIndex_mo->getFeatureAsString(0);
			m_tokens[first_monad] = feature_value;
			/*
			if (pStringFilterList != 0 
			    && m_token2filtered_token.find(feature_value) == m_token2filtered_token.end()) {
				bool bDummyStopWord;
				std::string filtered_token = pStringFilterList->process(feature_value, bDummyStopWord);
				m_token2filtered_token[feature_value] = filtered_token;
			}
			*/
		}
	}

	delete pIndex_sheaf;

	if (!bookcase_title_feature_name.empty()) {
		MQL_query = "GET OBJECTS HAVING MONADS IN " + grandSOM.toString() + "[" + bookcase_title_OTN + " GET " + bookcase_title_feature_name + "]GO";
		
		if (!pEnv->executeString(MQL_query,
					 bResult,
					 false, // bPrintResult
					 false  // bReportError
					 )) {
			bResult = false;
			return false;
		}
		if (!bResult) {
			return false;
		}
		
		pIndex_sheaf = pEnv->takeOverFlatSheaf();
		
		index_sheaf_ci = pIndex_sheaf->const_iterator();
		
		while (index_sheaf_ci.hasNext()) {
			const FlatStraw *pIndex_straw = index_sheaf_ci.next();
			FlatStrawConstIterator index_straw_ci = pIndex_straw->const_iterator();
			while (index_straw_ci.hasNext()) {
				const MatchedObject *pIndex_mo = index_straw_ci.next();
				monad_m last_monad = pIndex_mo->getLast();
				std::string feature_value = pIndex_mo->getFeatureAsString(0);
				m_bookcase_lm2title_map[last_monad] = feature_value;
			}
		}
		
		delete pIndex_sheaf;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////
//
// BookcaseHarvester
//
//////////////////////////////////////////////////////////////////////////
BookcaseHarvester::BookcaseHarvester(EmdrosEnv *pEnv, const std::string& bookcase_OTN, const std::string& indexed_OTN, const std::string& indexed_feature_name_list)
	: m_pEnv(pEnv),
	  m_bookcase_OTN(bookcase_OTN),
	  m_indexed_OTN(indexed_OTN),
	  m_bookcase_largest_object_length(0)
{
	process_comma_separated_string_list(indexed_feature_name_list, m_indexed_feature_name_list);
}

BookcaseHarvester::~BookcaseHarvester()
{
	// Nothing to do. We don't own anything.
}

void BookcaseHarvester::process(const SetOfMonads& substrate, const StringList& token_list, monad_m largest_proximity, bool bReduceToSingleHitsWithinBookcases, BookcaseHitList& result)
{
	UNUSED(substrate); // FIXME: Use substrate!
	
        if (token_list.isEmpty()) {
		return;
	}


	eObjectRangeType objectRangeType;
	bool bObjectTypeExists;
	id_d_t object_type_id;
	eMonadUniquenessType monadUniquenessType;
	if (!m_pEnv->getMQLEE()->pDB->objectTypeExists(m_bookcase_OTN,
						      bObjectTypeExists,
						      object_type_id,
						      objectRangeType,
						      monadUniquenessType)) {
		ASSERT_THROW(false,
			     "Bookcase object type '" + m_bookcase_OTN + "' does not exist!");
	}

	if (!bObjectTypeExists) {
		ASSERT_THROW(false,
			     "Bookcase object type '" + m_bookcase_OTN + "' does not exist!");
	}

	if (!m_pEnv->getMQLEE()->pDB->getLargestObjectLength(m_bookcase_OTN, 
							     objectRangeType,
							     m_bookcase_largest_object_length)) {
		ASSERT_THROW(false,
			     "Could not get largest object length for bookcase object type '" + m_bookcase_OTN + "'");
	}

	// Make sure we don't look too far!
	bool bUseLargestProximity = true;
	if (largest_proximity <= 1) {
		bUseLargestProximity = false;
	} else if (largest_proximity >= m_bookcase_largest_object_length) {
		bUseLargestProximity = false;
	}

	std::string index_OTN = make_index_name("_index", m_bookcase_OTN, m_indexed_OTN, m_indexed_feature_name_list);

	// feature_value --> { first_monad --> SetOfMonads }
	typedef std::map<int, std::map<monad_m, SetOfMonads> > mydict_t;

        mydict_t mydict;
        monad2monadMap lmdict;
        
	// Turn the token list into a token set,
	// so as not to query for the same token more than once
	// if it appears more than once in the token_list.
	StringListConstIterator token_ci = token_list.const_iterator();
	std::map<std::string, int> tokenstring2int_map;
	std::map<int, std::string> mydictindex2tokenstring_map;
	std::map<int, int> tokenindex2mydict_map;
	int token_list_index = 0;
	int mydict_index = 0;
	while (token_ci.hasNext()) {
		std::string feature_value = token_ci.next();
		std::map<std::string, int>::const_iterator ci = tokenstring2int_map.find(feature_value);
		if (ci == tokenstring2int_map.end()) {
			int index = mydict_index;
			tokenstring2int_map.insert(std::make_pair(feature_value, index));
			tokenindex2mydict_map.insert(std::make_pair(token_list_index, index));
			mydictindex2tokenstring_map.insert(std::make_pair(index, feature_value));
			++mydict_index;
		} else {
			int index = ci->second;
			tokenindex2mydict_map.insert(std::make_pair(token_list_index, index));
			mydictindex2tokenstring_map.insert(std::make_pair(index, feature_value));
		}
		++token_list_index;
	}
	std::map<int, std::string>::const_iterator ci1 = mydictindex2tokenstring_map.begin();
	while (ci1 != mydictindex2tokenstring_map.end()) {
		std::string feature_value = ci1->second;
		int mydict_index = ci1->first;
		++ci1;

		mydict.insert(std::make_pair(mydict_index, std::map<monad_m, SetOfMonads>()));


		std::string query = "SELECT ALL OBJECTS WHERE [" + index_OTN + " " + make_string_feature_comparison("feature_value", feature_value) + " GET monad_set]";

		Sheaf *pIndex_sheaf = 0;
		bool bResult = true;

		if (!m_pEnv->executeString(query,
					  bResult,
					  false, // bPrintResult
					  false  // bReportError
					  )) {
			bResult = false;
			ASSERT_THROW(false, "Wasn't able to execute query for harvesting.");
		}
		if (!bResult) {
			ASSERT_THROW(false, "Wasn't able to execute query for harvesting.");
		}

		pIndex_sheaf = m_pEnv->takeOverSheaf();

		SheafConstIterator index_sheaf_ci = pIndex_sheaf->const_iterator();
		
		while (index_sheaf_ci.hasNext()) {
			const Straw *pIndex_straw = index_sheaf_ci.next();
			StrawConstIterator index_straw_ci = pIndex_straw->const_iterator();
			while (index_straw_ci.hasNext()) {
				const MatchedObject *pIndex_mo = index_straw_ci.next();
				monad_m first_monad = pIndex_mo->getFirst();
				monad_m last_monad = pIndex_mo->getLast();
				std::string monad_set_str = pIndex_mo->getFeatureAsString(0);
				SetOfMonads som;
				som.fromCompactString(monad_set_str);
				if (mydict[mydict_index].find(first_monad) == mydict[mydict_index].end()) {
					mydict[mydict_index].insert(std::make_pair(first_monad, som));
					lmdict.insert(std::make_pair(first_monad, last_monad));

				} else {
					ASSERT_THROW(lmdict[first_monad] == last_monad,
						     "first_monad " + long2string(first_monad) + " does not have same last_monad in lmdict for feature value '" + feature_value + "' ... last_monad = " + long2string(last_monad) + ", while lmdict has last_monad" + long2string(lmdict[first_monad]));
					mydict[mydict_index][first_monad].unionWith(som);
				}
			}
		}

		delete pIndex_sheaf;
	}

	// Find out whether there is more than one
	StringListConstIterator token_list_ci = token_list.const_iterator();
	monad_m token_list_size = 0;
	while (token_list_ci.hasNext()) {
		++token_list_size;
		token_list_ci.next();
	}

	if (token_list_size == 1) {
		token_list_ci = token_list.const_iterator();
		std::string feature_value = token_list_ci.current();
		int mydict_index = tokenindex2mydict_map[0];
		std::map<monad_m, SetOfMonads>::const_iterator fmsom_ci = mydict[mydict_index].begin();
		std::map<monad_m, SetOfMonads>::const_iterator fmsom_cend = mydict[mydict_index].end();
		while (fmsom_ci != fmsom_cend) {
			monad_m first_monad = fmsom_ci->first;
			monad_m last_monad = lmdict[first_monad];
			SetOfMonads som = mydict[mydict_index][first_monad];
			if (bReduceToSingleHitsWithinBookcases) {
				monad_m m = som.first();
				result.addHit(first_monad, last_monad, m);
			} else {
				SOMConstIterator som_ci = som.const_iterator();
				while (som_ci.hasNext()) {
					MonadSetElement mse = som_ci.next();
					for (monad_m m = mse.first();
					     m <= mse.last();
					     ++m) {
						result.addHit(first_monad, last_monad, m);
					}
				}
			}
			++fmsom_ci;
		}
	} else {
		FastSetOfMonads masterSOM;

		token_list_ci = token_list.const_iterator();
		monad_m token_list_index = 0;
		while (token_list_ci.hasNext()) {
			std::string feature_value = token_list_ci.next();
			int mydict_index = tokenindex2mydict_map[token_list_index];
			std::map<monad_m, SetOfMonads>::const_iterator ffv_ci = mydict[mydict_index].begin();
			std::map<monad_m, SetOfMonads>::const_iterator ffv_cend = mydict[mydict_index].end();

			FastSetOfMonads SOM = FastSetOfMonads();
			
			while (ffv_ci != ffv_cend) {
				monad_m first_monad = ffv_ci->first;

				if (token_list_index == 0) {
					masterSOM.add(first_monad);
				} else {
					SOM.add(first_monad);
				}

				++ffv_ci;
			}

			if (token_list_index > 0) {
				masterSOM = FastSetOfMonads::intersect(masterSOM, SOM);
			}

			++token_list_index;
		}

		if (largest_proximity <= 1) {
			FastSOMConstIterator masterSOMCI = masterSOM.const_iterator();
			while (masterSOMCI.hasNext()) {
				MonadSetElement master_mse = masterSOMCI.next();
				monad_m master_mse_first = master_mse.first();
				monad_m master_mse_last = master_mse.last();
				monad_m m = master_mse_first;
				while (m <= master_mse_last) {
					monad_m first_monad = m;
					monad_m last_monad = lmdict[first_monad];
						
						
					int first_mydict_index = tokenindex2mydict_map[0];
					SetOfMonads masterSOM2 = mydict[first_mydict_index][m];					
						
					monad_m index = 1;
					while (index < token_list_size) {
						int mydict_index = tokenindex2mydict_map[index];
						
						SetOfMonads other_som = mydict[mydict_index][m];
						
						other_som.offset(-index);
						
						masterSOM2 = SetOfMonads::intersect(masterSOM2, other_som);
						
						++index;
						
					}
					
					if (!masterSOM2.isEmpty()) {
						if (bReduceToSingleHitsWithinBookcases) {
							result.addHit(first_monad, last_monad, masterSOM2.first());
						} else {
							SOMConstMonadIterator masterSOM2_ci(masterSOM2);
							while (masterSOM2_ci.hasNext()) {
								monad_m m2 = masterSOM2_ci.next();
								
								result.addHit(first_monad, last_monad, m2);
							}
						}
					}
					
					++m;
				}
			}	
		} else { // token_list.size() > 1 && largest_proximity > 1
			FastSOMConstIterator masterSOMCI = masterSOM.const_iterator();
			while (masterSOMCI.hasNext()) {
				MonadSetElement master_mse = masterSOMCI.next();
				monad_m master_mse_first = master_mse.first();
				monad_m master_mse_last = master_mse.last();
				monad_m m = master_mse_first;
				while (m <= master_mse_last) {
					monad_m first_monad = m;
					monad_m last_monad = lmdict[first_monad];


					int first_mydict_index = tokenindex2mydict_map[0];
					SetOfMonads first_feature_SOM = mydict[first_mydict_index][m];

					if (largest_proximity > (last_monad - first_monad + 1)) {
						// If the largest
						// proximity within
						// which to search is
						// greather than the
						// span of
						// first_monad..last_monad,
						// then we know at
						// this point that all
						// the monads in the
						// first_feature_SOM
						// set of monads are
						// going to be valid
						// results.  The
						// reason this is true
						// is that we have
						// already vetted all
						// the first_monad
						// entries in the
						// masterSOM, as being
						// within the
						// first_monad..last_monad
						// range.
						// So we just go ahead and add them all.

						if (!first_feature_SOM.isEmpty()) {
							if (bReduceToSingleHitsWithinBookcases) {
								result.addHit(first_monad, last_monad, first_feature_SOM.first());
							} else {
								SOMConstMonadIterator first_feature_SOM_monad_ci(first_feature_SOM);
								while (first_feature_SOM_monad_ci.hasNext()) {
									monad_m m2 = first_feature_SOM_monad_ci.next();
									
									result.addHit(first_monad, last_monad, m2);
								}
							}
						}

					} else {
						if (bUseLargestProximity
						    && token_list_size > 1) {
							SetOfMonads masterSOM2 = first_feature_SOM;

							token_list_index = 1;
							while (token_list_index < token_list_size) {
								int mydict_index = tokenindex2mydict_map[token_list_index];
							
								const SetOfMonads& other_som = mydict[mydict_index][m];
								SOMConstIterator other_som_ci = other_som.const_iterator();
								SetOfMonads masterSOM3;
								while (other_som_ci.hasNext()) {
									MonadSetElement mse = other_som_ci.next();
									for (monad_m m2 = mse.first();
									     m2 <= mse.last();
									     ++m2) {
										monad_m other_fm = MYMAX(m2 - largest_proximity, first_monad);
										monad_m other_lm = MYMIN(m2 + largest_proximity, last_monad);
										SetOfMonads masterSOM4(other_fm, other_lm);
										masterSOM3.unionWith(SetOfMonads::intersect(masterSOM2, masterSOM4));
									}
								}
								masterSOM2 = masterSOM3;
								
								++token_list_index;
							}

							/*
							  if (masterSOM2.hasLargerGap(largest_proximity)) {
								// Do not add it!
							} else {
							*/
							if (!masterSOM2.isEmpty()) {
								if (bReduceToSingleHitsWithinBookcases) {
									result.addHit(first_monad, last_monad, masterSOM2.first());
								} else {
									SOMConstMonadIterator masterSOM2_ci(masterSOM2);
									while (masterSOM2_ci.hasNext()) {
										monad_m m2 = masterSOM2_ci.next();
											
										result.addHit(first_monad, last_monad, m2);
									}
								}
							}
						} else {
							SetOfMonads masterSOM2 = fill_with_largest_proximity(first_feature_SOM, largest_proximity, first_monad, last_monad);

							token_list_index = 1;
							while (token_list_index < token_list_size) {
								int mydict_index = tokenindex2mydict_map[token_list_index];
							
								SetOfMonads other_som = mydict[mydict_index][m];
								
								SetOfMonads other_som_filled = fill_with_largest_proximity(other_som, largest_proximity, first_monad, last_monad);
								
							
								masterSOM2 = SetOfMonads::intersect(masterSOM2, other_som_filled);
								
								++token_list_index;
							}
							
							SetOfMonads masterSOM3 = SetOfMonads::intersect(masterSOM2, first_feature_SOM);
							
							
							if (!masterSOM3.isEmpty()) {
								if (bReduceToSingleHitsWithinBookcases) {
									result.addHit(first_monad, last_monad, masterSOM3.first());
								} else {
									SOMConstMonadIterator masterSOM3_ci(masterSOM3);
									while (masterSOM3_ci.hasNext()) {
										monad_m m2 = masterSOM3_ci.next();
										
										result.addHit(first_monad, last_monad, m2);
									}
								}
							}
						}
					}

					++m;
				}
			}	
		}
	}
}



bool harvestFTS_with_version(EmdrosEnv *pEnv, int fts_engine_version, const std::string& bookcase_OTN, const std::string& indexed_OTN, const std::string& indexed_feature_name_list, const SetOfMonads& substrate, const StringList& token_list, monad_m largest_proximity, bool bReduceToSingleHitsWithinBookcases, std::string& error_message, BookcaseHitList& result)
{
	if (fts_engine_version == 1
	    || fts_engine_version == 2) {
		BookcaseHarvester *pHarvester = 0;
		if (fts_engine_version == 2) {
			pHarvester = new BookcaseHarvester2(pEnv, bookcase_OTN, indexed_OTN, indexed_feature_name_list);
		} else {
			pHarvester = new BookcaseHarvester(pEnv, bookcase_OTN, indexed_OTN, indexed_feature_name_list);
		}
		try {
			pHarvester->process(substrate, token_list, largest_proximity, bReduceToSingleHitsWithinBookcases, result);
			if (bReduceToSingleHitsWithinBookcases) {
				result = result.removeAllButFirstHitInEachBookcase();
			}
		} catch (EmdrosException& e) {
			delete pHarvester;
			error_message += e.what();
			return false;
		}
		delete pHarvester;
		return true;
	} else if (fts_engine_version == 3) {
		bool bResult = false;
		try {
			std::string query;
			StringListConstIterator qlci = token_list.const_iterator();
			while (qlci.hasNext()) {
				std::string this_term = qlci.next();
				query += this_term;
				if (qlci.hasNext()) {
					query += " ";
				}
			}
			bool bUseGoogleSyntax = true; // FIXME: Pass it as a parameter!
			bResult = harvestFTS3(pEnv, bUseGoogleSyntax, bookcase_OTN, indexed_OTN, indexed_feature_name_list, substrate, query, largest_proximity, bReduceToSingleHitsWithinBookcases, error_message, result);
		} catch (EmdrosException& e) {
			error_message += e.what();
			return false;
		}
		return bResult;
	} else {
		error_message += "Unknown fts_engine_version: " + long2string(fts_engine_version);
		return false;
	}
}


bool harvestFTS(EmdrosEnv *pEnv, const std::string& bookcase_OTN, const std::string& indexed_OTN, const std::string& indexed_feature_name_list, const SetOfMonads& substrate, const StringList& token_list, monad_m largest_proximity, bool bReduceToSingleHitsWithinBookcases, std::string& error_message, BookcaseHitList& result)
{
	return harvestFTS_with_version(pEnv, 1, bookcase_OTN, indexed_OTN, indexed_feature_name_list, substrate, token_list, largest_proximity, bReduceToSingleHitsWithinBookcases, error_message, result);
}


bool harvestFTSNonGoogle_with_version(EmdrosEnv *pEnv, int fts_engine_version, const std::string& bookcase_OTN, const std::string& indexed_OTN, const std::string& indexed_feature_name_list, const SetOfMonads& substrate, const std::string& query, monad_m largest_proximity, EmdrosFTSStringFilter *pStringFilterList, bool bReduceToSingleHitsWithinBookcases, std::string& error_message, BookcaseHitList& result)
{
	StringList token_list = harvestFTSTokenize(query, pStringFilterList);
	return harvestFTS_with_version(pEnv, fts_engine_version, bookcase_OTN, indexed_OTN, indexed_feature_name_list, substrate, token_list, largest_proximity, bReduceToSingleHitsWithinBookcases, error_message, result);
}

bool harvestFTSNonGoogle(EmdrosEnv *pEnv, const std::string& bookcase_OTN, const std::string& indexed_OTN, const std::string& indexed_feature_name_list, const SetOfMonads& substrate, const std::string& query, monad_m largest_proximity, EmdrosFTSStringFilter *pStringFilterList, bool bReduceToSingleHitsWithinBookcases, std::string& error_message, BookcaseHitList& result)
{
	StringList token_list = harvestFTSTokenize(query, pStringFilterList);
	return harvestFTS(pEnv, bookcase_OTN, indexed_OTN, indexed_feature_name_list, substrate, token_list, largest_proximity, bReduceToSingleHitsWithinBookcases, error_message, result);
}

StringList harvestFTSTokenize(const std::string& input_string, EmdrosFTSStringFilter *pStringFilterList)
{
	std::list<std::string> result_std_list;
	split_string(strip(input_string), " \t\n\r\v", result_std_list);
	
	StringList result;
	std::list<std::string>::const_iterator ci = result_std_list.begin();
	while (ci != result_std_list.end()) {
		std::string token_candidate = *ci;
		if (pStringFilterList == 0) {
			result.addStringBack(token_candidate);
		} else {
			bool bShouldAdd = false;
			std::string morphed_token_candidate = pStringFilterList->process(token_candidate, bShouldAdd);
			if (bShouldAdd && !morphed_token_candidate.empty()) {
				result.addStringBack(morphed_token_candidate);
			}
		}
		++ci;
	}
	return result;
}

std::list<StringList> harvestFTSGoogleTokenize(const std::string& input_string, EmdrosFTSStringFilter *pStringFilterList)
{
	if (input_string.find('"') == std::string::npos) {
		std::list<StringList> result;
		result.push_back(harvestFTSTokenize(input_string, pStringFilterList));
		return result;
	} else {
		std::string::size_type input_string_length = input_string.length();
		std::vector<std::string> token_chain_list;
		// We only add one if the first character exists and isn't '"'
		// We will add one below if the first character is '"'
		int index_of_back = 0;
		if (input_string_length > 0 && input_string[0] != '"') {
			token_chain_list.push_back("");
			index_of_back = 0;
		} else {
			index_of_back = -1;
		}
		bool bInQuotes = false;
		for (std::string::size_type index = 0;
		     index < input_string_length;
		     ++index) {
			char c = input_string[index];
			if (c == '"') {
				if (bInQuotes) {
					bInQuotes = false;
				} else {
					bInQuotes = true;
				}
				token_chain_list.push_back("");
				index_of_back += 1;
			} else {
				token_chain_list[index_of_back] += c;
			}
		}
		std::list<StringList> result;
		std::vector<std::string>::const_iterator chain_ci = token_chain_list.begin();
		while (chain_ci != token_chain_list.end()) {
			std::string cur_input_string = strip(*chain_ci);
			if (!cur_input_string.empty()) {
				result.push_back(harvestFTSTokenize(cur_input_string, pStringFilterList));
			}

			++chain_ci;
		}
		return result;
	}
}

bool harvestFTSGoogle_with_version(EmdrosEnv *pEnv, int fts_engine_version, const std::string& bookcase_OTN, const std::string& indexed_OTN, const std::string& indexed_feature_name_list, const SetOfMonads& substrate, const std::string query, monad_m largest_proximity, EmdrosFTSStringFilter *pStringFilterList, bool bReduceToSingleHitsWithinBookcases, std::string& error_message, BookcaseHitList& result)
{
	std::list<StringList> token_list_list = harvestFTSGoogleTokenize(query, pStringFilterList);
	if (token_list_list.size() == 1) {
		const StringList& token_list = *(token_list_list.begin());
		monad_m lp;
		if (query.find('"') == std::string::npos) {
			lp = largest_proximity;
		} else {
			lp = 1;
		}
		return harvestFTS_with_version(pEnv, fts_engine_version, bookcase_OTN, indexed_OTN, indexed_feature_name_list, substrate, token_list, lp, bReduceToSingleHitsWithinBookcases, error_message, result);
	} else {
		std::list<BookcaseHitList> bookcase_hit_list_list;
		std::list<StringList>::const_iterator token_list_list_ci = token_list_list.begin();
		while (token_list_list_ci != token_list_list.end()) {
			bookcase_hit_list_list.push_back(BookcaseHitList());
			const StringList& token_list = *token_list_list_ci;
			if (!harvestFTS_with_version(pEnv, fts_engine_version, bookcase_OTN, indexed_OTN, indexed_feature_name_list, substrate, token_list, 1, false, error_message, *(bookcase_hit_list_list.rbegin()))) {
				return false;
			}
			++token_list_list_ci;
		}
		
		// Combine the BookcaseHitLists
		result = *bookcase_hit_list_list.begin();
		if (bookcase_hit_list_list.size() > 1) {
			token_list_list_ci = token_list_list.begin();
			std::list<BookcaseHitList>::const_iterator bookcase_hit_list_list_ci = bookcase_hit_list_list.begin();
			++bookcase_hit_list_list_ci;
			while (bookcase_hit_list_list_ci != bookcase_hit_list_list.end()) {
				const BookcaseHitList& other_bookcase_hit_list = *bookcase_hit_list_list_ci;
				int token_list_length = 0;
				StringListConstIterator token_list_ci = (*token_list_list_ci).const_iterator();
				while (token_list_ci.hasNext()) {
					token_list_ci.next();
					++token_list_length;
				}
				result = result.removeThoseNotPresentInOther(other_bookcase_hit_list, largest_proximity, largest_proximity + token_list_length - 1);
				
				++token_list_list_ci;
				++bookcase_hit_list_list_ci;
			}
		}
		if (bReduceToSingleHitsWithinBookcases) {
			result = result.removeAllButFirstHitInEachBookcase();
		}
	}
	
	return true;
}

bool harvestFTSGoogle(EmdrosEnv *pEnv, const std::string& bookcase_OTN, const std::string& indexed_OTN, const std::string& indexed_feature_name_list, const SetOfMonads& substrate, const std::string query, monad_m largest_proximity, EmdrosFTSStringFilter *pStringFilterList, bool bReduceToSingleHitsWithinBookcases, std::string& error_message, BookcaseHitList& result)
{
	return harvestFTSGoogle_with_version(pEnv, 1, bookcase_OTN, indexed_OTN, indexed_feature_name_list, substrate, query, largest_proximity, pStringFilterList, bReduceToSingleHitsWithinBookcases, error_message, result);
}


/* Just so we can make the amalgamation. */
#undef MYSIGN
#undef MYABS
#undef MYMIN
#undef MYMAX
