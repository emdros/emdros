/*
 * harvest_fts3.cpp
 *
 * Full Text Search engine version 3, part of the Emdros Harvest
 * library.
 *
 * Ulrik Sandborg-Petersen
 * Created: 3/19-2016
 * Last update: 2/13-2018
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2016-2018  Ulrik Sandborg-Petersen
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

/**@file harvest_fts3.cpp
 *@brief Implementation file for harvest full text search version 3
 */

#include <harvest_fts3.h>
#include <map>

#include <emdf.h>
#include <mql_sheaf.h>
#include <debug.h>
#include <string_func.h> // String functions in libemdf.



/******************************************************************
 *
 * Utility functions
 *
 ******************************************************************/

bool executeQuery_FTS3Search(EmdrosEnv *pEE, const std::string& query)
{
	bool bCompilerSuccess = false;
	bool bDBSuccess = pEE->executeString(query,
					     bCompilerSuccess,
					     false,
					     false);
	return bCompilerSuccess && bDBSuccess;
}



/******************************************************************
 *
 * TokenMonadList
 *
 ******************************************************************/

typedef std::map<monad_m, monad_m> FM2LMMap;

class BookcaseMonadList; // Forward declaration

class TokenMonadList {
	typedef std::list<monad_m> MList;
	MList m_mylist;
 public:
	TokenMonadList();
	~TokenMonadList();

	void append(monad_m m);

	bool query(EmdrosEnv *pEE, const std::string& token_otn, const std::list<std::string>& token_feature_name_list, const SetOfMonads& substrate, const std::string& term);

	std::string toMonadSetString() const;
	SetOfMonads toSetOfMonads() const;

	void merge_with_bookcases(const BookcaseMonadList& bookcase_monad_list, BookcaseHitList& /* out */ hit_list);

	long size();
};

class BookcaseMonadList {
 public:
	FM2LMMap m_lmdict;
 public:
	BookcaseMonadList();
	~BookcaseMonadList();

	void append(monad_m fm_bookcase, monad_m lm_bookcase);

	bool query(EmdrosEnv *pEE, const std::string& bookcase_otn, const SetOfMonads& substrate, const TokenMonadList& token_monad_list);
};


TokenMonadList::TokenMonadList()
{
}


TokenMonadList::~TokenMonadList()
{
	m_mylist.clear();
}



void TokenMonadList::append(monad_m m)
{
	m_mylist.push_back(m);
}


std::string local_getFeatureType(EmdrosEnv *pEE, const std::string& token_otn, const std::string& token_feature)
{
	std::string result = "string";

	std::string query_str = "SELECT FEATURES FROM OBJECT TYPE [" + token_otn + "]\nGO";

	if (!executeQuery_FTS3Search(pEE, query_str)) {
		return result;
	} else if (!pEE->isTable()) {
		return result;
	} else {
		Table *pTable = pEE->takeOverTable();

		TableIterator ti = pTable->iterator();
		while (ti.hasNext()) {
			std::string feature_name = pTable->getColumn(ti, 1);
			if (strcmp_nocase(feature_name, token_feature) == 0) {
				// Get type
				result = pTable->getColumn(ti, 2);
				break;
			}
			ti.next();
		}
	}
	return result;
}

bool TokenMonadList::query(EmdrosEnv *pEE, const std::string& token_otn, const std::list<std::string>& token_feature_name_list, const SetOfMonads& substrate, const std::string& term)
{
	std::string query_str = std::string("SELECT ALL OBJECTS IN") + substrate.toString() + "WHERE\n[" + token_otn;
	std::list<std::string>::const_iterator token_feature_ci = token_feature_name_list.begin();
	std::list<std::string>::const_iterator token_feature_ciend = token_feature_name_list.end();
	while (token_feature_ci != token_feature_ciend) {
		std::string token_feature = *token_feature_ci;
		
		std::string token_feature_type = local_getFeatureType(pEE, token_otn, token_feature);
		
		query_str += " ";
		
		if (token_feature_type == "string"
		    || token_feature_type == "ascii") {
			query_str += make_string_feature_comparison(token_feature, term); // Defined in harvest_fts.cpp.
		} else if (token_feature_type.substr(0, 7) == "list of integer") {
			query_str += token_feature + " HAS " + monad_m2string(string2monad_m(term));
		} else if (token_feature_type == "integer") {
			query_str += token_feature + " = " + monad_m2string(string2monad_m(term));
		} else {
			query_str += make_string_feature_comparison(token_feature, term);// Defined in harvest_fts.cpp.
		}

		++token_feature_ci;

		if (token_feature_ci != token_feature_ciend) {
			query_str += " OR ";
		}
	}
	
	query_str += "]\nGO";

	if (!executeQuery_FTS3Search(pEE, query_str)) {
		return false;
	} else if (!pEE->isSheaf()) {
		return false;
	} else {
		Sheaf *pToken_sheaf = pEE->takeOverSheaf();

		SheafConstIterator token_sheaf_ci = pToken_sheaf->const_iterator();

		while (token_sheaf_ci.hasNext()) {
			const Straw *pToken_straw = token_sheaf_ci.next();
			StrawConstIterator token_straw_ci = pToken_straw->const_iterator();
			while (token_straw_ci.hasNext()) {
				const MatchedObject *pToken_mo = token_straw_ci.next();
				monad_m token_fm = pToken_mo->getFirst();

				append(token_fm);
			}

		}

		delete pToken_sheaf;

		return true;
	}
}



std::string TokenMonadList::toMonadSetString() const
{
	std::list<std::string> str_list;
	str_list.push_back("{");
	MList::const_iterator ci = m_mylist.begin();
	MList::const_iterator ciend = m_mylist.end();
	while (ci != ciend) {
		monad_m m = *ci;
		str_list.push_back(monad_m2string(m));
		++ci;
		if (ci != ciend) {
			str_list.push_back(",");
		}
	}
	str_list.push_back("}");

	return joinListEmptyBetween(str_list, 32768);
}

SetOfMonads TokenMonadList::toSetOfMonads() const
{
	SetOfMonads result;
	MList::const_iterator ci = m_mylist.begin();
	MList::const_iterator ciend = m_mylist.end();
	while (ci != ciend) {
		monad_m m = *ci;
		result.add(m);
		++ci;
	}

	return result;
}



void TokenMonadList::merge_with_bookcases(const BookcaseMonadList& bookcase_monad_list, BookcaseHitList& /* out */ hit_list)
{
        hit_list.clearHitMap();

	FM2LMMap::const_iterator bci = bookcase_monad_list.m_lmdict.begin();
	FM2LMMap::const_iterator bciend = bookcase_monad_list.m_lmdict.end();

	MList::const_iterator tci = m_mylist.begin();
	MList::const_iterator tciend = m_mylist.end();
	while (tci != tciend) {
		if (bci == bciend) {
			break;
		}

		monad_m token_m = *tci;

		monad_m fm_bookcase = bci->first;
		monad_m lm_bookcase = bci->second;

		if (token_m < fm_bookcase) {
			++tci;
			continue;
		} else if (lm_bookcase < token_m) {
			++bci;
			continue;
		}

		ASSERT_THROW(fm_bookcase <= token_m && token_m <= lm_bookcase,
			     "Logic error: fm_bookcase <= token_m <= lm_bookcase is not the case.");

		hit_list.addHit(fm_bookcase, lm_bookcase, token_m);

		++tci;
	}
}


long TokenMonadList::size()
{
	return m_mylist.size();
}


	
	


/******************************************************************
 *
 * BookcaseMonadList
 *
 ******************************************************************/




BookcaseMonadList::BookcaseMonadList()
{
}


BookcaseMonadList::~BookcaseMonadList()
{
	m_lmdict.clear();
}



void BookcaseMonadList::append(monad_m fm_bookcase, monad_m lm_bookcase)
{
	m_lmdict[fm_bookcase] = lm_bookcase;
}



bool BookcaseMonadList::query(EmdrosEnv *pEE, const std::string& bookcase_otn, const SetOfMonads& substrate, const TokenMonadList& token_monad_list)
{
	SetOfMonads intersection = SetOfMonads::intersect(substrate, token_monad_list.toSetOfMonads());
	std::string query_str = std::string("GET OBJECTS HAVING MONADS IN") + intersection.toString() + "\n[" + bookcase_otn + "]\nGO";

	if (!executeQuery_FTS3Search(pEE, query_str)) {
		return false;
	} else if (!pEE->isFlatSheaf()) {
		return false;
	} else {
		FlatSheaf *pBookcase_sheaf = pEE->takeOverFlatSheaf();

		FlatSheafConstIterator bookcase_sheaf_ci = pBookcase_sheaf->const_iterator();

		while (bookcase_sheaf_ci.hasNext()) {
			const FlatStraw *pBookcase_straw = bookcase_sheaf_ci.next();
			FlatStrawConstIterator bookcase_straw_ci = pBookcase_straw->const_iterator();
			while (bookcase_straw_ci.hasNext()) {
				const MatchedObject *pBookcase_mo = bookcase_straw_ci.next();
				monad_m fm_bookcase = pBookcase_mo->getFirst();
				monad_m lm_bookcase = pBookcase_mo->getLast();

				append(fm_bookcase, lm_bookcase);
			}
		}

		delete pBookcase_sheaf;

		return true;
	}
}



/******************************************************************
 *
 * FTS3Tokenizer
 *
 ******************************************************************/

typedef std::pair<const std::string, monad_m> StringMPair;

typedef std::list<StringMPair> StringMPairList;

typedef std::list<StringMPairList> StringMPairListList;


class FTS3Tokenizer {
private:
	typedef enum {
		kNoQuote,
		kInQuote
	} eQuoteState;

	typedef enum {
		kOutOfToken,
		kInToken
	} eTokenState;

	StringMPairList m_no_order_list;
	StringMPairListList m_order_list_list;

public:
	FTS3Tokenizer();
	~FTS3Tokenizer();

	StringMPairListList process(bool bUseGoogleSyntax, const std::string& term_string);

	void clean();
private:
	void emit_token(const std::string& token_str, bool bUseGoogleSyntax, eQuoteState quote_state);

};

FTS3Tokenizer::FTS3Tokenizer()
{
}


FTS3Tokenizer::~FTS3Tokenizer()
{
}




/**
Returns a list of lists of two-tuples, where each two-tuple
contains:

(query-term, index-integer)

The index-integer may be < 0, meaning it can occur anywhere in
the input relative to the others.  If it is >= 0, it means
there is a relative order to the query terms, indicated by the
index-integer within the given list.

The list of lists contains:

1) One list in which all the query-terms appear with < negative
   index-integer, i.e., all the ones for which order doesn't
   matter.

2) One list for each "double-quote-enclosed" stretch of tokens.

Note that lists of kind #2 are only returned iff
bUseGoogleSyntax is true.  Otherwise, all query terms are
assumed to be on a par, and the list of lists contains one
list in which all the query terms have -1 as the
index-integer.

*/
StringMPairListList FTS3Tokenizer::process(bool bUseGoogleSyntax, const std::string& term_string)
{
        clean();

	std::string::const_iterator ci = term_string.begin();
	std::string::const_iterator ciend = term_string.end();

	eQuoteState quote_state = kNoQuote;
	eTokenState state = kOutOfToken;
	
	std::string token;
	
	while (ci != ciend) {
		char c = *ci;
		if (c == '"') {
			if (quote_state == kNoQuote) {
				quote_state = kInQuote;
				this->m_order_list_list.push_back(StringMPairList());
			} else {
				// quote_state == kInQuote
				emit_token(token, bUseGoogleSyntax, quote_state);
				token = "";
				quote_state = kNoQuote;
			}
		} else if (c == ' '
			   || c == '\t'
			   || c == '\n'
			   || c == '\r') {
			if (state == kOutOfToken) {
				; // Nothing to do
			} else {
				// kInToken
				state = kOutOfToken;
				emit_token(token, bUseGoogleSyntax, quote_state);
				token = "";
			}
		} else {
			state = kInToken;
			token += c;
		}

		++ci;
	}
	

	emit_token(token, bUseGoogleSyntax, quote_state);

	StringMPairListList result_list;
	result_list.push_back(m_no_order_list);
	StringMPairListList::iterator it = m_order_list_list.begin();
	StringMPairListList::iterator itend = m_order_list_list.end();
	while (it != itend) {
		result_list.push_back(*it);
		++it;
	}

        return result_list;
}



void FTS3Tokenizer::clean()
{
	m_no_order_list.clear();
	m_order_list_list.clear();
}



void FTS3Tokenizer::emit_token(const std::string& token_str, bool bUseGoogleSyntax, eQuoteState quote_state)
{
        // FIXME: Put the token through a filter chain here...
        // token_str = process_filters()

        if (token_str.empty()) {
		; // Nothing to do. 
	} else if (bUseGoogleSyntax) {
		if (quote_state == kNoQuote) {
			m_no_order_list.push_back(StringMPair(token_str, -1));
		} else {
			monad_m next_index_integer = 0;
			// quote_state == kInQuote
			if (m_order_list_list.empty()) {
				m_order_list_list.push_back(StringMPairList());
			}
			StringMPairListList::reverse_iterator rit = m_order_list_list.rbegin();
			// assert(rit != m_order_list_list.rend())
			// Is the last list empty?
			if ((*rit).empty()) {
				// Yes
				next_index_integer = 0;
			} else {
				// Take the monad of the last element
				// of the the last list, add 1.
				next_index_integer = (*rit).rbegin()->second + 1;
			}
			(*rit).push_back(StringMPair(token_str, next_index_integer));
		}
	} else {
		m_no_order_list.push_back(StringMPair(token_str, -1));
	}
}




/******************************************************************
 *
 * FTS3TermDict
 *
 ******************************************************************/
typedef std::map<std::string, BookcaseHitList*> Term2PHitListMap;
typedef Term2PHitListMap::iterator FTS3TermDictIterator;

class FTS3TermDict {
 protected:
	Term2PHitListMap m_map;
 public:
	FTS3TermDict();
	~FTS3TermDict();
	
	void addHitList(const std::string& term, BookcaseHitList *pHitList);	
	BookcaseHitList *findHitList(const std::string& term);
	FTS3TermDictIterator begin();
	FTS3TermDictIterator end();
};

FTS3TermDict::FTS3TermDict()
{
}



FTS3TermDict::~FTS3TermDict()
{
	Term2PHitListMap::iterator it = m_map.begin();
	while (it != m_map.end()) {
		BookcaseHitList *pHitList = it->second;
		delete pHitList;
		++it;
	}
	m_map.clear();
}


void FTS3TermDict::addHitList(const std::string& term, BookcaseHitList *pHitList)
{
	ASSERT_THROW(m_map.find(term) == m_map.end(),
		     std::string("Logic Error: term '") + term + "' already in FTS3TermDict");
	m_map[term] = pHitList;
}

BookcaseHitList *FTS3TermDict::findHitList(const std::string& term)
{
	Term2PHitListMap::iterator it = m_map.find(term);
	if (it == m_map.end()) {
		return 0;
	} else {
		return it->second;
	}
}

FTS3TermDictIterator FTS3TermDict::begin()
{
	return m_map.begin();
}

FTS3TermDictIterator FTS3TermDict::end()
{
	return m_map.end();
}



/******************************************************************
 *
 * FTS3Search
 *
 ******************************************************************/

void local_filter_term_lists(long token_range, const StringMPairListList& term_list_list, FTS3TermDict *pTermDict, BookcaseHitList& /* out */ hit_list)
{
	StringMPairListList::const_iterator ci = term_list_list.begin();
	StringMPairListList::const_iterator ciend = term_list_list.end();
	while (ci != ciend) {
		BookcaseHitList term_overall_hit_list;
		bool bIsFirstTerm = true;

		const StringMPairList& term_list = *ci;
		StringMPairList::const_iterator ci2 = term_list.begin();
		StringMPairList::const_iterator ci2end = term_list.end();
		while (ci2 != ci2end) {
			const std::string& term = (*ci2).first;
			monad_m index_integer = (*ci2).second;

			BookcaseHitList *pTerm_hit_list = pTermDict->findHitList(term);
			if (pTerm_hit_list == 0) {
				ASSERT_THROW(false,
					     "Logic error: Could not find term '" + term + "' in FTS3TermDict.");
				break;
			}

			if (index_integer < 0) {
				if (bIsFirstTerm) {
					term_overall_hit_list.merge(*pTerm_hit_list);
					bIsFirstTerm = false;
				} else {
					term_overall_hit_list.merge_range(*pTerm_hit_list, token_range, false);
				}
			} else if (index_integer == 0) {
				term_overall_hit_list.merge(*pTerm_hit_list);
			} else {
				term_overall_hit_list.merge_range(*pTerm_hit_list, 1, true);
			}
			
			++ci2;
		}
		
		hit_list.merge(term_overall_hit_list);
		
		++ci;
	}
}


/*
        Ensure that only those bookcases which are represented in all
        HitLists are present after filtering.

        Return True iff for all hit lists in term_dict it holds that
        each hit list is non-empty after filtering.

*/
bool local_filter_hit_list_bookcases(FTS3TermDict *pTermDict)
{
        BookcaseHitList *pPrev_hit_list = 0;

	FTS3TermDictIterator it = pTermDict->begin();
	FTS3TermDictIterator itend = pTermDict->end();

	while (it != itend) {
		BookcaseHitList *pHit_list = it->second;

		if (pPrev_hit_list != 0) {
			pHit_list->filter_both(*pPrev_hit_list);

			if (pHit_list->empty()
			    || pPrev_hit_list->empty()) {
				return false;
			}

		}

		pPrev_hit_list = pHit_list;

		++it;
	}
        return true;
}


TokenMonadList *local_query_term_FTS3Search(EmdrosEnv *pEE, const std::string& token_otn, const std::list<std::string>& token_feature_name_list, const SetOfMonads& substrate, const std::string& term)
{
	TokenMonadList *pToken_monad_list = new TokenMonadList();
	pToken_monad_list->query(pEE, token_otn, token_feature_name_list, substrate, term);
	return pToken_monad_list;
}



BookcaseMonadList *local_query_bookcase(EmdrosEnv *pEE, const std::string& bookcase_otn, const SetOfMonads& substrate, TokenMonadList *pToken_monad_list)
{
	BookcaseMonadList *pBookcase_monad_list = new BookcaseMonadList();
	if (pToken_monad_list != 0) {
		pBookcase_monad_list->query(pEE, bookcase_otn, substrate, *pToken_monad_list);
	} else {
		// pTokenMonadList is 0, meaning we had no hits
	}
	return pBookcase_monad_list;
}




void local_query_FTS3Search(EmdrosEnv *pEE, const std::string& bookcase_otn, const std::string& token_otn, const std::list<std::string>& token_feature_name_list, const SetOfMonads& substrate, const StringMPairListList& term_list_list, FTS3TermDict *pTermDict, BookcaseHitList& result)
{
	typedef std::map<std::string, TokenMonadList*> Term2TMLMap;
	Term2TMLMap term_token_dict;
	
	TokenMonadList *pSmallest_token_monad_list = 0;
        long smallest_token_monad_list_length = MAX_MONAD;

	StringMPairListList::const_iterator ci = term_list_list.begin();
	StringMPairListList::const_iterator ciend = term_list_list.end();
	while (ci != ciend) {
		const StringMPairList& term_list = *ci;

		StringMPairList::const_iterator ci2 = term_list.begin();
		StringMPairList::const_iterator ci2end = term_list.end();
		while (ci2 != ci2end) {
			const std::string& term = ci2->first;

			// Only query if we haven't done so before.
			if (term_token_dict.find(term) == term_token_dict.end()) {
				TokenMonadList *pQuery_result = local_query_term_FTS3Search(pEE, token_otn, token_feature_name_list, substrate, term);
				
				term_token_dict.insert(std::make_pair(term, pQuery_result));

				long query_result_length = pQuery_result->size();
				if (query_result_length <= smallest_token_monad_list_length) {
					pSmallest_token_monad_list = pQuery_result;
					smallest_token_monad_list_length = query_result_length;
				}
			}
			++ci2;
		}
		++ci;
	}
	
        BookcaseMonadList *pBookcase_monad_list = local_query_bookcase(pEE, bookcase_otn, substrate, pSmallest_token_monad_list);

	ci = term_list_list.begin();
	while (ci != ciend) {
		const StringMPairList& term_list = *ci;

		StringMPairList::const_iterator ci2 = term_list.begin();
		StringMPairList::const_iterator ci2end = term_list.end();
		while (ci2 != ci2end) {
			const std::string& term = ci2->first;
			// Only add it if pTermDict does not already
			// have this term.
			if (pTermDict->findHitList(term) == 0) {
				TokenMonadList *pToken_monad_list = term_token_dict[term];

				BookcaseHitList *pHit_list = new BookcaseHitList(result);
				pHit_list->clearHitMap();
				
				pToken_monad_list->merge_with_bookcases(*pBookcase_monad_list, *pHit_list);
				
				pTermDict->addHitList(term, pHit_list);
			}

			++ci2;
		}
		++ci;
	}
}

FTS3Search::FTS3Search(const std::string& bookcase_otn, const std::string& token_otn, const std::string& token_feature_name_list, const SetOfMonads& substrate)
{
	m_bookcase_otn = bookcase_otn;
	m_token_otn = token_otn;
	process_comma_separated_string_list(token_feature_name_list, m_token_feature_name_list); // Defined in harvest_fts.cpp.
	m_substrate = substrate;
}



FTS3Search::~FTS3Search()
{
}





bool FTS3Search::locate(EmdrosEnv *pEE, bool bUseGoogleSyntax, long token_range, const std::string& term_string, BookcaseHitList& result)
{
        FTS3Tokenizer *pTokenizer = new FTS3Tokenizer(); // FIXME: Add pFilter_list
	long my_token_range = token_range;

	StringMPairListList term_list_list = pTokenizer->process(bUseGoogleSyntax, term_string);

	delete pTokenizer;

	FTS3TermDict *pTermDict = new FTS3TermDict();

	local_query_FTS3Search(pEE, m_bookcase_otn, m_token_otn, m_token_feature_name_list, m_substrate, term_list_list, pTermDict, result);
	if (!local_filter_hit_list_bookcases(pTermDict)) {
		delete pTermDict;
		result.clearHitMap();
		return false;
	} else {
		local_filter_term_lists(my_token_range, term_list_list, pTermDict, result);
		delete pTermDict;
		return true;
	}
}







/******************************************************************
 *
 * Utility functions
 *
 ******************************************************************/

bool harvestFTS3(EmdrosEnv *pEnv, bool bUseGoogleSyntax, const std::string& bookcase_OTN, const std::string& indexed_OTN, const std::string& indexed_feature_name_list, const SetOfMonads& substrate, const std::string& query, monad_m largest_proximity, bool bReduceToSingleHitsWithinBookcases, std::string& error_message, BookcaseHitList& result)
{
	try {
		if (!pEnv->connectionOk()) {
			return false;
		} else {
			FTS3Search fts3_search(bookcase_OTN, indexed_OTN, indexed_feature_name_list, substrate);
			fts3_search.locate(pEnv, bUseGoogleSyntax, largest_proximity, query, result);
			if (bReduceToSingleHitsWithinBookcases) {
				result = result.removeAllButFirstHitInEachBookcase();
			} 
		}
	} catch (EmdrosException e) {
		error_message += e.what();
		return false;
	}
	return true;
}

bool harvestFTS3NonGoogle(EmdrosEnv *pEnv, const std::string& bookcase_OTN, const std::string& indexed_OTN, const std::string& indexed_feature_name_list, const SetOfMonads& substrate, const std::string& query, monad_m largest_proximity, bool bReduceToSingleHitsWithinBookcases, std::string& error_message, BookcaseHitList& result)
{
	bool bUseGoogleSyntax = false;
	return harvestFTS3(pEnv, bUseGoogleSyntax, bookcase_OTN, indexed_OTN, indexed_feature_name_list, substrate, query, largest_proximity, bReduceToSingleHitsWithinBookcases, error_message, result);
}



bool harvestFTS3Google(EmdrosEnv *pEnv, const std::string& bookcase_OTN, const std::string& indexed_OTN, const std::string& indexed_feature_name_list, const SetOfMonads& substrate, const std::string& query, monad_m largest_proximity, bool bReduceToSingleHitsWithinBookcases, std::string& error_message, BookcaseHitList& result)
{
	bool bUseGoogleSyntax = true;
	return harvestFTS3(pEnv, bUseGoogleSyntax, bookcase_OTN, indexed_OTN, indexed_feature_name_list, substrate, query, largest_proximity, bReduceToSingleHitsWithinBookcases, error_message, result);
}

