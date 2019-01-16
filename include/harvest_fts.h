/*
 * harvest_fts.h
 *
 * Full Text Search, part of the harvest library.
 *
 * Ulrik Sandborg-Petersen
 * Created: 8/25-2010
 * Last update: 1/16-2019
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


/**@file harvest_fts.h
 *@brief Header file for harvest full text search
 */

#ifndef HARVEST_FTS__H__
#define HARVEST_FTS__H__

#include "monads.h"
#include <string>
#include <set>
#include <list>
#include "emdros_environment.h"
#include "json_classes.h"
#include "string_list.h"

#include "emdros-lconfig.h"

#include <ostream>


// "Internal" functions. Internal to the harvesting stuff anyway.
extern void process_comma_separated_string_list(const std::string& comma_separated_string, std::list<std::string>& string_list);
extern SetOfMonads fill_with_largest_proximity(const SetOfMonads& SOM, monad_m largest_proximity, monad_m first_monad, monad_m last_monad);
extern std::string make_string_feature_comparison(const std::string& feature_name, const std::string feature_value);

/** Base class for all string filters.
 *
 * The string filters form a chain, applied with the
 * process(std::string, bool) method.
 *
 * The process_self() method is where the action is.
 *
 * This class is a dummy class, in that it does nothing in and of
 * itself, except if there is a "m_pNext" object, in which case it
 * applies that.
 *
 */
class EmdrosFTSStringFilter {
 public:
	/** Constructor. Sets m_pNext to 0.
	 */
	EmdrosFTSStringFilter();

	/** Destructor.
	 *
	 * Deletes down the chain, if m_pNext isn't nil.
	 */
	virtual ~EmdrosFTSStringFilter();
	
	/** The public entry point to the chain.
	 *
	 * First applies itself (with process_self).  If bUseThisValue
	 * from that call is true, we modify the value down the chain
	 * until we either hit the end of the chain, or hit a filter
	 * which sets bUseThisValue to false.
	 *
	 * The EmdrosFTSStringFilter's process_self filter does
	 * nothing at all, and passes all strings unmodified with
	 * bUseThisValue == true.
	 *
	 * @param input_string The input string to modify or allow/reject.
	 *
	 * @param bUseThisValue A bool saying whether we should use
	 * this string (true == use; false == discard).
	 *
	 * @return The (possibly modified) string.
	 */
	virtual std::string process(const std::string& input_string, bool& /* out */ bUseThisValue);

	void setNext(EmdrosFTSStringFilter *pNext);
 protected:
	/** The private entry point to the chain.
	 *
	 * Does the actual processing in this filter.
	 *
	 * The EmdrosFTSStringFilter class does nothing at all, and
	 * passes all strings unmodified with bUseThisValue == true.
	 *
	 * @param input_string The input string to modify or allow/reject.
	 *
	 * @param bUseThisValue A bool saying whether we should use
	 * this string (true == use; false == discard).
	 *
	 * @return The (possibly modified) string.
	 */
	virtual std::string process_self(const std::string& input_string, bool& /* out */ bUseThisValue);

	/* The next filter in the chain, or nil (0) to indicate
	 * end-of-chain.
	 */
	EmdrosFTSStringFilter *m_pNext;
};



/** The SFStripChars strips certain characters on either side of the
 * string value.
 *
 * If the constructor with no parameter is used, then the characters
 * which are stripped are the whitespace characters " \n\r\t\v".
 *
 * The other constructor lets the user say which characters to strip.
 */
class SFStripChars : public EmdrosFTSStringFilter {
 protected:
	std::string m_chars_to_strip;
 public:
	/** Constructor. Sets m_pNext to 0. Sets
	 * m_chars_to_strip to all whitespace characters.
	 */
	SFStripChars();

	/** Constructor. Sets m_pNext to 0. Sets
	 * m_chars_to_strip to chars_to_strip.
	 */
	SFStripChars(const std::string& chars_to_strip);

	/** Destructor.
	 *
	 * Deletes down the chain, if m_pNext isn't nil.
	 */
	virtual ~SFStripChars();

 protected:
	/** The private entry point to the chain.
	 *
	 * Does the actual processing in this filter.
	 *
	 * @param input_string The input string to modify or allow/reject.
	 *
	 * @param bUseThisValue A bool saying whether we should use
	 * this string (true == use; false == discard).
	 *
	 * @return The (possibly modified) string.
	 */
	virtual std::string process_self(const std::string& input_string, bool& /* out */ bUseThisValue);
};

/** The SFReplaceString filter takes 2 parameters, both of which must
 * be strings (both of which may be empty; however, if the first is
 * empty, the filter is a no-op).  Replaces all substrings in the
 * token corresponding to the first parameter with the second
 * parameter.  Never blocks the token.
 */
class SFReplaceString : public EmdrosFTSStringFilter {
 protected:
	std::string m_string_to_replace;
	std::string m_replacement;
	bool m_bIsNoOp;
 public:
	/** Constructor. Sets m_pNext to 0.
	 */
	SFReplaceString(const std::string& string_to_replace, const std::string& replacement);

	/** Destructor.
	 *
	 * Deletes down the chain, if m_pNext isn't nil.
	 */
	virtual ~SFReplaceString();

 protected:
	/** The private entry point to the chain.
	 *
	 * Does the actual processing in this filter.
	 *
	 * @param input_string The input string to modify or allow/reject.
	 *
	 * @param bUseThisValue A bool saying whether we should use
	 * this string (true == use; false == discard).
	 *
	 * @return The (possibly modified) string.
	 */
	virtual std::string process_self(const std::string& input_string, bool& /* out */ bUseThisValue);
};



/** The SFLowerCase lower-cases the string with str_tolower() from the
 * EMdF library, which in turn does it according to the current locale
 * with tolower() from the C standard library.  Note that this does
 * not work with UTF-8!
 *
 */
class SFLowerCase : public EmdrosFTSStringFilter {
 public:
	/** Constructor. Sets m_pNext to 0. Sets
	 */
	SFLowerCase();

	/** Destructor.
	 *
	 * Deletes down the chain, if m_pNext isn't nil.
	 */
	virtual ~SFLowerCase();

 protected:
	/** The private entry point to the chain.
	 *
	 * Does the actual processing in this filter.
	 *
	 * @param input_string The input string to modify or allow/reject.
	 *
	 * @param bUseThisValue A bool saying whether we should use
	 * this string (true == use; false == discard).
	 *
	 * @return The (possibly modified) string.
	 */
	virtual std::string process_self(const std::string& input_string, bool& /* out */ bUseThisValue);
};



/** The SFUpperCase upper-cases the string with str_toupper() from the
 * EMdF library, which in turn does it according to the current locale
 * with toupper() from the C standard library.  Note that this does
 * not work with UTF-8!
 *
 */
class SFUpperCase : public EmdrosFTSStringFilter {
 public:
	/** Constructor. Sets m_pNext to 0. Sets
	 */
	SFUpperCase();

	/** Destructor.
	 *
	 * Deletes down the chain, if m_pNext isn't nil.
	 */
	virtual ~SFUpperCase();

 protected:
	/** The private entry point to the chain.
	 *
	 * Does the actual processing in this filter.
	 *
	 * @param input_string The input string to modify or allow/reject.
	 *
	 * @param bUseThisValue A bool saying whether we should use
	 * this string (true == use; false == discard).
	 *
	 * @return The (possibly modified) string.
	 */
	virtual std::string process_self(const std::string& input_string, bool& /* out */ bUseThisValue);
};


/** The SFBlockEmpty blocks all empty strings, i.e., sets
 * bUseThisValue to false iff the input string is empty.  Otherwise,
 * sets bUseThisValue to true.
 *
 */
class SFBlockEmpty : public EmdrosFTSStringFilter {
 public:
	/** Constructor. Sets m_pNext to 0. Sets
	 */
	SFBlockEmpty();

	/** Destructor.
	 *
	 * Deletes down the chain, if m_pNext isn't nil.
	 */
	virtual ~SFBlockEmpty();

 protected:
	/** The private entry point to the chain.
	 *
	 * Does the actual processing in this filter.
	 *
	 * @param input_string The input string to modify or allow/reject.
	 *
	 * @param bUseThisValue A bool saying whether we should use
	 * this string (true == use; false == discard).
	 *
	 * @return The (possibly modified) string.
	 */
	virtual std::string process_self(const std::string& input_string, bool& /* out */ bUseThisValue);
};


/** The SFBlockEmpty blocks all empty strings, i.e., sets
 * bUseThisValue to false iff the input string is empty.  Otherwise,
 * sets bUseThisValue to true.
 *
 */
class SFStopWords : public EmdrosFTSStringFilter {
 protected:
	std::set<std::string> m_stop_word_set;
 public:
	/** Constructor. Sets m_pNext to 0. Sets
	 */
	SFStopWords(const StringList& list_of_stop_words);

	/** Destructor.
	 *
	 * Deletes down the chain, if m_pNext isn't nil.
	 */
	virtual ~SFStopWords();

 protected:
	/** The private entry point to the chain.
	 *
	 * Does the actual processing in this filter.
	 *
	 * @param input_string The input string to modify or allow/reject.
	 *
	 * @param bUseThisValue A bool saying whether we should use
	 * this string (true == use; false == discard).
	 *
	 * @return The (possibly modified) string.
	 */
	virtual std::string process_self(const std::string& input_string, bool& /* out */ bUseThisValue);
};




/** Bookcase indexer class.
 *
 */
class BookcaseIndexer {
 public:
	/** BookcaseIndexer constructor.
	 *
	 * @param pSourceEnv The EmdrosEnv from which to read. Must be
	 * a valid EmdrosEnv, with pSourceEnv->connectinOk() returning
	 * true.
	 *
	 * @param pTargetEnv The EmdrosEnv to which to write the
	 * data. May be nil (in which case it isn't used), otherwise
	 * it must be a valid EmdrosEnv, with
	 * pTargetEnv->connectionOk() returning true.
	 *
	 * @param pMQLOutStream A std::ostream (descendant) to which
	 * the MQL to create the object types and objects in the
	 * bookcase is emitted.  If both pTargetEnv and pMQLOutStream
	 * are non-nil, the MQL is both sent to pTargetEnv and to
	 * pMQLOutStream.
	 *
	 * @param bookcase_OTN The Name of the surrounding "bookcase"
	 * Object Type, within whose objects the indexed object type
	 * must be found.
	 *
	 * @param indexed_OTN The name of the "indexed object type",
	 * whose objects are indexed.
	 *
	 * @param indexed_feature_name_list A string representing a
	 * comma-separated list of feature names of the indexed object
	 * type which must be indexed in this one index.
	 *
	 * @param EmdrosFTSStringFilter If nil, it is ignored, and the
	 * string value of the indexed_feature_name_list members for
	 * any given object is used unmodified.  If non-nil, then the
	 * result of the corresponding
	 * pStringFilter->process(feature_value, bUse) is used, but
	 * only if bUse is true after the call.  In this way, you can
	 * apply filters (such as stop-word removal and normalization
	 * of strings) to the feature-values.
	 *
	 */
	BookcaseIndexer(EmdrosEnv *pSourceEnv, EmdrosEnv *pTargetEnv, std::ostream *pMQLOutStream, const std::string& bookcase_OTN, const std::string& indexed_OTN, const std::string& indexed_feature_name_list, EmdrosFTSStringFilter *pStringFilter);

	/** BookcaseIndexer destructor.
	 *
	 * Doesn't tear down or close either pSourceEnv or pTargetEnv
	 * or pMQLOutStream.
	 *
	 */
	virtual ~BookcaseIndexer();

	/** Main indexer function.
	 *
	 * This runs the indexing process within the substrate given.
	 * If the substrate is the empty monad set, then all monads in
	 * the database are used as the substrate.  In fact, the monad
	 * set {1 - MAX_MONAD } is used (where MAX_MONADS is something
	 * like 2.1 billion).  This doesn't hurt performance.
	 *
	 * @param substrate the monad set to index against.
	 *
	 * @bEmitVacuum must be true in order to emit VACUUM DATABASE
	 * ANLYZE GO at the end. If this is false, then this is not
	 * emitted.
	 *
	 * @return true on success; false on error.  You should
	 * inspect the result of errorMessage() to see what went
	 * wrong, as well as pSourceStream and/or pTargetStream's
	 * error messages, in case false is returned.
	 */
	virtual bool process(const SetOfMonads& substrate, bool bEmitVacuum = true);

	/** Returns the error message (if any; otherwise an empty
	 * string), and clears the error message locally in this
	 * object.
	 *
	 * @param A string containing the error message (if any;
	 * otherwise the empty string).
	 */
	std::string errorMessage(void);
 protected:
	/** Process one bookcase object, whose monads are as in
	 * bookcase_som.
	 * @internal
	 *
	 * @return true on success, false on error. If false, the
	 * errorMessage() method will tell what's wrong.
	 */
	virtual bool process_bookcase(const SetOfMonads& bookcase_som);

	/** Emit the query-string to the query cache.
	 *
	 * Only on calling "go()" will the query be emitted to the
	 * pTargetEnv and/or pMQLOutStream.
	 * 
	 * @param query The query-string to emit.  Need not be a full
	 * query.
	 */

	virtual void emit(const std::string& query);

	/** Append "\nGO" to the query cache.  Then, emit the query as
	 * one string both to pTargetEnv (if non-nil) and to
	 * pMQLOutStream (if non-nil).  If both are nil, nothing is
	 * emitted anywhere.  Then clears the local query cache.
	 *
	 * @return true on success, false on error.  If false, the
	 * errorMessage() method will tell what's wrong.
	 */
	 virtual bool go(void);

	 /** Clear the query cache.
	  *
	  */
	 virtual void clear_query_cache();

	 virtual void appendError(const std::string& message);

	 virtual bool execQueryOnSource(const std::string query);

 protected:
	// Members
	std::string m_error_message;
	std::list<std::string> m_query_cache;
	EmdrosEnv *m_pSourceEnv;
	EmdrosEnv *m_pTargetEnv;
	std::ostream *m_pMQLOutStream;
	std::string m_bookcase_OTN;
	std::string m_indexed_OTN;
	std::list<std::string> m_indexed_feature_name_list;
	std::string m_index_OTN;
	EmdrosFTSStringFilter *m_pStringFilter;
};

class BookcaseHit {
 public:
	monad_m m_bookcase_fm;
	monad_m m_bookcase_lm;
	monad_m m_hit_fm;
	BookcaseHit(monad_m bookcase_fm, monad_m bookcase_lm, monad_m hit_fm);
	~BookcaseHit();

	BookcaseHit(const BookcaseHit& other);
#ifndef SWIG
	BookcaseHit& operator=(const BookcaseHit& other);
#endif
 private:
	void assign(const BookcaseHit& other);
};

class BookcaseHitList;

typedef std::map<std::pair<monad_m, monad_m>, std::set<monad_m> > HitMap;

class BookcaseHitListConstIterator {
	const BookcaseHitList *m_pMother;
	HitMap::const_iterator m_bookcase_ci;
	std::set<monad_m>::const_iterator m_fm_ci;
	int m_max_visit_count;
 public:
	BookcaseHitListConstIterator();
	BookcaseHitListConstIterator(const BookcaseHitList* pMotherHitList);
	BookcaseHitListConstIterator(const BookcaseHitList* pMotherHitList, int OFFSET, int LIMIT);
	BookcaseHitListConstIterator(const BookcaseHitListConstIterator& other);
	~BookcaseHitListConstIterator();
	bool hasNext() const; //  Is the iterator == end iterator?  Doesn't alter iterator
	BookcaseHit next(); // Gets the current and advances the iterator
	BookcaseHit current(); // Gets the current without altering iterator
#ifndef SWIG
	BookcaseHitListConstIterator& operator=(const BookcaseHitListConstIterator& other);
#endif
 private:
	void assign(const BookcaseHitListConstIterator& other);
};

class BookcaseHitList {
 protected:
	// bookcase_fm, bookcase_lm --> hit_fm;
	HitMap m_hits;
	int m_hit_count;
	friend class BookcaseHitListConstIterator;
	int m_max_hits_per_page;
 public:
	BookcaseHitList();
	BookcaseHitList(const BookcaseHitList& other);
	~BookcaseHitList();
	BookcaseHitList& operator=(const BookcaseHitList& other);
 public:
	// Pagination
	void setMaxHitsPerPage(int max_hits_per_page) { m_max_hits_per_page = max_hits_per_page;}; // Defaults to 10 in the c'tor
	int getPageCount(); // As counted by m_max_hits_per_page
	int getHitCount();
	int getBookcaseCount();
	BookcaseHitListConstIterator const_iterator(int page_number); // Paged, page_number >= 1

	BookcaseHitListConstIterator const_iterator(); // All, not paged
	void addHit(monad_m bookcase_fm, monad_m bookcase_lm, monad_m hit_fm);
	BookcaseHitList removeThoseNotPresentInOther(const BookcaseHitList& other, monad_m proximity_length_backward, monad_m proximity_length_forward);
	bool hasHit(monad_m bookcase_fm, monad_m bookcase_lm, monad_m hit_fm, monad_m proximity_length) const;
	BookcaseHitList removeAllButFirstHitInEachBookcase();
	bool harvestHits(EmdrosEnv *pEnv, const std::string& bookcase_OTN, const std::string& bookcase_title_OTN, const std::string& bookcase_title_feature_name, const std::string& token_OTN, const std::string& token_feature_name, monad_m monads_before_hit_word, monad_m monads_after_hit_word, const std::string& query = "", EmdrosFTSStringFilter *pStringFilterList = 0, int page_no = -1);
#ifdef SWIG
%apply std::string& OUTPUT { std::string& snippet }
%apply std::string& OUTPUT { std::string& snippet_title }
#endif
	void getSnippet(const BookcaseHit& hit, const std::string& hit_token_prefix, const std::string& hit_token_suffix, const std::string& hit_token_infix, bool bStripToken, std::string& snippet, std::string& snippet_title);

	// For FTS3
	void clearHitMap();
	bool empty() const;
	void merge(const BookcaseHitList& other);
	void merge_range(const BookcaseHitList& other, int token_range, bool bDirectionMatters);
	void filter_both(BookcaseHitList& other);
	static void filter(BookcaseHitList& hl1, const BookcaseHitList& hl2);
 protected:
	// monad_m --> token_feature_name
	typedef std::map<monad_m, std::string> HarvestTokenMap;
	typedef HarvestTokenMap HarvestBookcaseMap;
	HarvestTokenMap m_tokens;
	//std::map<std::string, std::string> m_token2filtered_token;
	//std::set<std::string> m_query_terms;
	monad_m m_monads_before_hit_word;
	monad_m m_monads_after_hit_word;
	HarvestBookcaseMap m_bookcase_lm2title_map;
 protected:
	std::string replace_templates(const std::string& instring, const BookcaseHit& hit, monad_m token_monad);
	void assign(const BookcaseHitList& other);
};

typedef std::map<monad_m, monad_m> monad2monadMap;

class BookcaseHarvester2; // Forward declaration

class BookcaseHarvester {
 public:
	BookcaseHarvester(EmdrosEnv *pEnv, const std::string& bookcase_OTN, const std::string& indexed_OTN, const std::string& indexed_feature_name_list);
	virtual ~BookcaseHarvester();
	virtual void process(const SetOfMonads& substrate, const StringList& token_list, monad_m largest_proximity, bool bReduceToSingleHitsWithinBookcases, BookcaseHitList& result);
 protected:
	EmdrosEnv *m_pEnv;
	std::string m_bookcase_OTN;
	std::string m_indexed_OTN;
	std::list<std::string> m_indexed_feature_name_list;

	monad_m m_bookcase_largest_object_length;
};


class BookcaseHarvesterParameters {
	BookcaseHarvesterParameters(EmdrosEnv *pEnv,
				    const std::string& bookcase_OTN, 
				    const std::string& indexed_OTN, 
				    const std::string& indexed_feature_name_list, 
				    const SetOfMonads& substrate, 
				    const std::string query, 
				    monad_m largest_proximity, 
				    EmdrosFTSStringFilter *pStringFilterList, 
				    bool bReduceToSingleHitsWithinBookcases, 
				    int fts_engine_version)
		: m_pEnv(pEnv),
		m_bookcase_OTN(bookcase_OTN), 
		m_indexed_OTN(indexed_OTN), 
		m_indexed_feature_name_list(indexed_feature_name_list), 
		m_substrate(substrate), 
		m_query(query), 
		m_largest_proximity(largest_proximity), 
		m_pStringFilterList(pStringFilterList), 
		m_bReduceToSingleHitsWithinBookcases(bReduceToSingleHitsWithinBookcases), 
		m_fts_engine_version(fts_engine_version) {}
	~BookcaseHarvesterParameters() {};
 protected:
	friend class BookcaseHarvester;
	friend class BookcaseHarvester2;
	EmdrosEnv *m_pEnv;
	const std::string& m_bookcase_OTN;
	const std::string& m_indexed_OTN;
	const std::string& m_indexed_feature_name_list;
	const SetOfMonads& m_substrate;
	const std::string m_query;
	monad_m m_largest_proximity;
	EmdrosFTSStringFilter *m_pStringFilterList;
	bool m_bReduceToSingleHitsWithinBookcases;
	int m_fts_engine_version;
};


/** Factory for EmdrosFTSStringFilter which loops through the
 * filter_names_and_parameters StringList, and returns a chain of
 * EmdrosFTSStringFilter objects based on the strings in the
 * StringList.
 *
 * Each entry in the StringList should either be a filter name (in
 * case the filter takes no parameters in its constructor), or a
 * filter name followed by the string which should be passed to the
 * filter's constructor.
 *
 * The valid filters currently are (filter-name, bTakesParameter,
 * ClassName):
 *
 * - ("uppercase", false, SFUpperCase)
 * - ("lowercase", false, SFLowerCase)
 * - ("strip-whitespace", false, SFStripChars)
 * - ("srtrip-chars", true, SFStripChars)
 * - ("block-empty", false, SFBlockEmpty)
 * // - ("replace-substring", true, SFReplaceSubstring) # Not implemented yet!
 *
 */
extern EmdrosFTSStringFilter *stringFilterFactory(const StringList& filter_names_and_parameters);

extern EmdrosFTSStringFilter *stringFilterFactoryJSONValue(const JSONValue* pJSONObject, bool& bResult, std::string& error_message, const std::string& filter_name = "");

extern EmdrosFTSStringFilter *stringFilterFactoryJSONString(const std::string& JSON_string, bool& bResult, std::string& error_message, const std::string& filter_name = "");

extern EmdrosFTSStringFilter *stringFilterFactoryJSONFileName(const std::string& JSON_filename, bool& bResult, std::string& error_message, const std::string& filter_name = "");

extern bool harvestFTS(EmdrosEnv *pEnv, const std::string& bookcase_OTN, const std::string& indexed_OTN, const std::string& indexed_feature_name_list, const SetOfMonads& substrate, const StringList& token_list, monad_m largest_proximity, bool bReduceToSingleHitsWithinBookcases, std::string& error_message, BookcaseHitList& result);

extern bool harvestFTS_with_version(EmdrosEnv *pEnv, int fts_engine_version, const std::string& bookcase_OTN, const std::string& indexed_OTN, const std::string& indexed_feature_name_list, const SetOfMonads& substrate, const StringList& token_list, monad_m largest_proximity, bool bReduceToSingleHitsWithinBookcases, std::string& error_message, BookcaseHitList& result);


extern bool harvestFTSNonGoogle(EmdrosEnv *pEnv, const std::string& bookcase_OTN, const std::string& indexed_OTN, const std::string& indexed_feature_name_list, const SetOfMonads& substrate, const std::string& query, monad_m largest_proximity, EmdrosFTSStringFilter *pStringFilterList, bool bReduceToSingleHitsWithinBookcases, std::string& error_message, BookcaseHitList& result);

extern bool harvestFTSNonGoogle_with_version(EmdrosEnv *pEnv, int fts_engine_version, const std::string& bookcase_OTN, const std::string& indexed_OTN, const std::string& indexed_feature_name_list, const SetOfMonads& substrate, const std::string& query, monad_m largest_proximity, EmdrosFTSStringFilter *pStringFilterList, bool bReduceToSingleHitsWithinBookcases, std::string& error_message, BookcaseHitList& result);


extern bool harvestFTSGoogle(EmdrosEnv *pEnv, const std::string& bookcase_OTN, const std::string& indexed_OTN, const std::string& indexed_feature_name_list, const SetOfMonads& substrate, const std::string query, monad_m largest_proximity, EmdrosFTSStringFilter *pStringFilterList, bool bReduceToSingleHitsWithinBookcases, std::string& error_message, BookcaseHitList& result);

extern bool harvestFTSGoogle_with_version(EmdrosEnv *pEnv, int fts_engine_version, const std::string& bookcase_OTN, const std::string& indexed_OTN, const std::string& indexed_feature_name_list, const SetOfMonads& substrate, const std::string query, monad_m largest_proximity, EmdrosFTSStringFilter *pStringFilterList, bool bReduceToSingleHitsWithinBookcases, std::string& error_message, BookcaseHitList& result);

extern StringList harvestFTSTokenize(const std::string& input_string, EmdrosFTSStringFilter *pStringFilterList);

// Not yet implemented.
// extern bool harvestFTS(BookcaseHarvesterParameters *pParameters, std::string& error_message, BookcaseHitList& result);


#ifndef SWIG
extern std::list<StringList> harvestFTSGoogleTokenize(const std::string& input_string, EmdrosFTSStringFilter *pStringFilterList);
#endif


#endif // HARVEST_FTS__H__
