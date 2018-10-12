/*
 * harvest_fts.h
 *
 * Full Text Search, part of the harvest library.
 *
 * Ulrik Sandborg-Petersen
 * Created: 8/25-2010
 * Last update: 5/11-2014
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

#ifndef HARVEST_FTS2__H__
#define HARVEST_FTS2__H__

#include <harvest_fts.h>
#include <string>
#include <map>
#include <set>
#include <list>


/** Bookcase indexer2 class.
 *
 */
class BookcaseIndexer2 : public BookcaseIndexer {
 public:
	/** BookcaseIndexer2 constructor.
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
	 * @param indexed_feature_name The feature name of the indexed
	 * object type which must be indexed.
	 *
	 * @param EmdrosFTSStringFilter If nil, it is ignored, and the
	 * string value of the indexed_feature_name for any given
	 * object is used unmodified.  If non-nil, then the result of
	 * the corresponding pStringFilter->process(feature_value,
	 * bUse) is used, but only if bUse is true after the call.  In
	 * this way, you can apply filters (such as stop-word removal
	 * and normalization of strings) to the feature-values.
	 *
	 */
	BookcaseIndexer2(EmdrosEnv *pSourceEnv, EmdrosEnv *pTargetEnv, std::ostream *pMQLOutStream, const std::string& bookcase_OTN, const std::string& indexed_OTN, const std::string& indexed_feature_name, EmdrosFTSStringFilter *pStringFilter, long max_bookcases_per_object);

	/** BookcaseIndexer destructor.
	 *
	 * Doesn't tear down or close either pSourceEnv or pTargetEnv
	 * or pMQLOutStream.
	 *
	 */
	virtual ~BookcaseIndexer2();

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

	virtual void print_mymap();


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
	 * the range bookcase_som.first() .. bookcase_som.last().
	 * @internal
	 *
	 * @return true on success, false on error. If false, the
	 * errorMessage() method will tell what's wrong.
	 */
	virtual bool process_bookcase(const SetOfMonads& bookcase_som);

	/** Process one bookcase object, whose monads are in the range
	 * first_monad, last_monad.
	 * @internal
	 *
	 * @return true on success, false on error. If false, the
	 * errorMessage() method will tell what's wrong.
	 */
	virtual bool process_bookcase(monad_m first_monad, monad_m last_monad);

	virtual void clear_mymap();

 protected:
	// Members
	long m_max_bookcases_per_object;
	long m_bookcase_count;
	long m_object_count;
	typedef std::map<std::string, std::pair<SetOfMonads*, SetOfMonads*> > string2PSOMPairMap;
	string2PSOMPairMap m_mymap;
};


class BookcaseHarvester2 : public BookcaseHarvester {
 public:
	BookcaseHarvester2(EmdrosEnv *pEnv, const std::string& bookcase_OTN, const std::string& indexed_OTN, const std::string& indexed_feature_name);
	virtual ~BookcaseHarvester2();
	virtual void process(const SetOfMonads& substrate, const StringList& token_list, monad_m largest_proximity, bool bReduceToSingleHitsWithinBookcases, BookcaseHitList& result);
 protected:
	typedef std::map<int, std::pair<SetOfMonads, SetOfMonads> > int2SOMPairMap;
	void getLMDict(const SetOfMonads& substrate, monad2monadMap& lmdict);
	bool getBigSOMs(const StringList& token_list, unsigned int token_list_length, monad_m largest_proximity, const int2SOMPairMap& mydict2, const std::map<int, int>& tokenindex2mydict_map, FastSetOfMonads& big_bookcase_som, FastSetOfMonads& first_feature_token_som);
	bool getBigSOM(const StringList& token_list, unsigned int token_list_length, const int2SOMPairMap& mydict2, const std::map<int, int>& tokenindex2mydict_map, FastSetOfMonads& big_bookcase_som);

};


extern bool harvestFTS2NonGoogle(EmdrosEnv *pEnv, const std::string& bookcase_OTN, const std::string& indexed_OTN, const std::string& indexed_feature_name, const SetOfMonads& substrate, const std::string& query, monad_m largest_proximity, EmdrosFTSStringFilter *pStringFilterList, bool bReduceToSingleHitsWithinBookcases, std::string& error_message, BookcaseHitList& result);

extern bool harvestFTS2Google(EmdrosEnv *pEnv, const std::string& bookcase_OTN, const std::string& indexed_OTN, const std::string& indexed_feature_name, const SetOfMonads& substrate, const std::string query, monad_m largest_proximity, EmdrosFTSStringFilter *pStringFilterList, bool bReduceToSingleHitsWithinBookcases, std::string& error_message, BookcaseHitList& result);



#endif // HARVEST_FTS2__H__
