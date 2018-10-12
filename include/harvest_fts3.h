/*
 * harvest_fts3.h
 *
 * Full Text Search version 3, part of the harvest library.
 *
 * Ulrik Sandborg-Petersen
 * Created: 3/19-2016
 * Last update: 2/13-2018
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


/**@file harvest_fts3.h
 *@brief Header file for harvest full text search version 3
 */

#ifndef HARVEST_FTS3__H__
#define HARVEST_FTS3__H__

#include <string>
#include <map>
#include <set>
#include <list>

#include <emdf.h>
#include <emdros_environment.h>
#include <harvest_fts.h>

class FTS3Search {
 protected:
	std::string m_bookcase_otn;
	std::string m_token_otn;
	std::list<std::string> m_token_feature_name_list;
	SetOfMonads m_substrate;
 public:
	FTS3Search(const std::string& bookcase_otn, const std::string& token_otn, const std::string& token_feature_name_list, const SetOfMonads& substrate);
	~FTS3Search();


	bool locate(EmdrosEnv *pEE, bool bUseGoogleSyntax, long token_range, const std::string& term_string, BookcaseHitList& result);
};


extern bool harvestFTS3(EmdrosEnv *pEnv, bool bUseGoogleSyntax, const std::string& bookcase_OTN, const std::string& indexed_OTN, const std::string& indexed_feature_name_list, const SetOfMonads& substrate, const std::string& query, monad_m largest_proximity, bool bReduceToSingleHitsWithinBookcases, std::string& error_message, BookcaseHitList& result);

extern bool harvestFTS3NonGoogle(EmdrosEnv *pEnv, const std::string& bookcase_OTN, const std::string& indexed_OTN, const std::string& indexed_feature_name_list, const SetOfMonads& substrate, const std::string& query, monad_m largest_proximity, bool bReduceToSingleHitsWithinBookcases, std::string& error_message, BookcaseHitList& result);

extern bool harvestFTS3Google(EmdrosEnv *pEnv, const std::string& bookcase_OTN, const std::string& indexed_OTN, const std::string& indexed_feature_name_list, const SetOfMonads& substrate, const std::string& query, monad_m largest_proximity, bool bReduceToSingleHitsWithinBookcases, std::string& error_message, BookcaseHitList& result);


#endif // HARVEST_FTS3__H__
