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
