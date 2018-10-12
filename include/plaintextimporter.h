/*
 * plaintextimporter.h
 *
 * A class to import plain text files.
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


#ifndef PLAINTEXT_IMPORTER_EMDROS_H_
#define PLAINTEXT_IMPORTER_EMDROS_H_

#include "emdf.h"
#include <list>
#include <map>
#include <vector>
#include <sstream>
#include <string>
#include "monads.h"
#include "pennimporter.h" // For EmdrosImporterbase 



class PlainTextImporter : public EmdrosImporterBase {
 protected:
	PEMOList m_words;
	PEMOList m_paragraphs;
	PEMOList m_documents;
public:
	PlainTextImporter(monad_m starting_monad, id_d_t starting_id_d);
	virtual ~PlainTextImporter();
	virtual bool readDocumentFromStream(std::istream *pIn);
	virtual void putSchema(std::ostream *pOut);
	virtual void putCorpusAsMQL(std::ostream *pOut);
 private:
	void cleanObjects(PEMOList& pemo_list);
	void emitMQLDocuments(std::ostream *pOut);
	void emitMQLParagraphs(std::ostream *pOut);
	void emitMQLWords(std::ostream *pOut);
	void emitMQLObjects(std::ostream *pOut, const std::list<FeatureInfo>& feature_infos, PEMOList& pemo_list, std::string object_type_name);
	
};


#endif
