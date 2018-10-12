/*
 * slashedtextimporter.h
 *
 * A class to import slashed text files.
 * into Emdros
 * Created: 8/8-2006
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


#ifndef SLASHEDTEXT_IMPORTER_EMDROS_H_
#define SLASHEDTEXT_IMPORTER_EMDROS_H_

#include "emdf.h"
#include <list>
#include <map>
#include <vector>
#include <sstream>
#include <string>
#include "monads.h"
#include "pennimporter.h" // For EmdrosImporterbase 



class SlashedTextImporter : public EmdrosImporterBase {
 protected:
	std::string m_word_internal_separator;
	std::string m_paragraph_separator_string;
	PEMOList m_words;
	PEMOList m_paragraphs;
	PEMOList m_documents;
public:
	SlashedTextImporter(monad_m starting_monad, id_d_t starting_id_d,
			    const std::string& word_internal_separator = "/",
			    const std::string& paragraph_separator_string = "\n\n");
	virtual ~SlashedTextImporter();
	virtual bool readDocumentFromStream(std::istream *pIn);
	virtual void putSchema(std::ostream *pOut);
	virtual void putCorpusAsMQL(std::ostream *pOut);
 private:
	void cleanDocuments(void); 
	void cleanParagraphs(void); 
	void cleanWords(void);
	void emitMQLDocuments(std::ostream *pOut);
	void emitMQLParagraphs(std::ostream *pOut);
	void emitMQLWords(std::ostream *pOut);
};


#endif
