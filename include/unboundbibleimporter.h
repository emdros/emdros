/*
 * unboundbibleimporter.cpp
 *
 * A class to import Unbound Bible files into Emdros
 * Created: 12/16-2005
 * Last update: 10/3-2011
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


#ifndef _UNBOUNDBIBLE_IMPORTER_H__
#define _UNBOUNDBIBLE_IMPORTER_H__

#include <map>
#include <list>
#include <string>
#include <iostream>

#include "emdf_output.h"
#include "pennimporter.h"

class UnboundBibleImporter : public EmdrosImporterBase {
 protected:
	EMOList m_books;
	EMOList m_chapters;
	EMOList m_verses;
	EMOList m_words;
	std::string m_last_book;
	long m_last_chapter;
	long m_last_verse;
	monad_m m_curmonad;
 public:
	UnboundBibleImporter() : EmdrosImporterBase(0,0) { };
	virtual ~UnboundBibleImporter() {};
	virtual bool readDocumentFromStream(std::istream *pIn);
	virtual void putSchema(std::ostream *pOut);
	virtual void putCorpusAsMQL(std::ostream *pOut);
 protected:
	void add_bcv(const std::string& book, long chapter, long verse);
	void add_word(const std::string& surface);
	EmdrosMemObject createBook(const std::string& bookname);
	EmdrosMemObject createChapter(const std::string& bookname, long chapter);
	EmdrosMemObject createVerse(const std::string& bookname, long chapter, long verse);
	void closeLastObject(EMOList& list);
};


#endif
