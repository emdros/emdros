/*
 * plaintextimporter.cpp
 *
 * A class to import plain text files
 * into Emdros
 * Created: 8/8-2006
 * Last update: 11/15-2018
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


#include <plaintextimporter.h>
#include <emdf.h>
#include <list>
#include <map>
#include <sstream>
#include <iostream>
#include <string>
#include <string_func.h>
#include <fstream>
#include <vector>
#include "plaintextschema.h"

typedef std::vector<std::string> StringVector;


    
//////////////////////////////////////////////////////////////
//
// PlainTextImporter
// 
//////////////////////////////////////////////////////////////


PlainTextImporter::PlainTextImporter(monad_m starting_monad, 
			     id_d_t starting_id_d)
	: EmdrosImporterBase(starting_monad, starting_id_d)
{
}

PlainTextImporter::~PlainTextImporter()
{
	cleanObjects(m_words);
	cleanObjects(m_paragraphs);
	cleanObjects(m_documents);
}

bool PlainTextImporter::readDocumentFromStream(std::istream *pIn)
{
	// Create document
	EmdrosMemObject *pDocument = new EmdrosMemObject(SetOfMonads(m_cur_monad), m_cur_id_d);
	++m_cur_id_d;
	m_documents.push_back(pDocument);

	std::string big_string;
	while (true) {
		std::string line;
		getline(*pIn, line);
		big_string += line + '\n';

		if (pIn->eof()) {
			break;
		}
	}

	big_string = remove_char(big_string, '\r');

	StringVector paragraph_arr;
	split_string_on_string(big_string, "\n\n", paragraph_arr);
	if (big_string.length() == 0) {
		; // Do nothing...
	} else {
		StringVector::const_iterator paragraph_ci = paragraph_arr.begin();
		StringVector::const_iterator paragraph_cend = paragraph_arr.end();
		while (paragraph_ci != paragraph_cend) {
			StringVector arr;
			split_string(*paragraph_ci, " \t\r\n", arr);
			if (paragraph_ci->length() == 0) {
				; // Do nothing...
			} else {
				// Create document
				EmdrosMemObject *pParagraph = new EmdrosMemObject(SetOfMonads(m_cur_monad), m_cur_id_d);
				++m_cur_id_d;
				m_paragraphs.push_back(pParagraph);
				
				StringVector::const_iterator ci = arr.begin();
				StringVector::const_iterator cend = arr.end();
				while (ci != cend) {
					if (*ci != "") {
						EmdrosMemObject *pWord = new EmdrosMemObject(SetOfMonads(m_cur_monad), m_cur_id_d);
						pWord->setFeature("surface", *ci);
						std::string stripped_surface;
						
						// Remove punctuation and space
						stripped_surface = remove_punct(*ci);
						stripped_surface = remove_char(stripped_surface, ' ');
						
						pWord->setFeature("stripped_surface", stripped_surface);
						
						
						m_words.push_back(pWord);
						
						// Add current monad to current paragraph
						pParagraph->addMonads(SetOfMonads(m_cur_monad));
						
						// Add current monad to current document
						pDocument->addMonads(SetOfMonads(m_cur_monad));
						
					
						++m_cur_monad;
						++m_cur_id_d;
					}
					++ci;
				} 
			}
			++paragraph_ci;
		}
	}


	return true;
}

void PlainTextImporter::putCorpusAsMQL(std::ostream *pOut)
{
	emitMQLDocuments(pOut);
	emitMQLParagraphs(pOut);
	emitMQLWords(pOut);
}

void PlainTextImporter::emitMQLDocuments(std::ostream *pOut)
{
	std::list<FeatureInfo> feature_infos_document; // No features...
	emitMQLObjects(pOut, feature_infos_document, m_documents, "Document");
}
            
void PlainTextImporter::emitMQLParagraphs(std::ostream *pOut)
{
	std::list<FeatureInfo> feature_infos_paragraph; // No features...
	emitMQLObjects(pOut, feature_infos_paragraph, m_paragraphs, "Paragraph");
}
            
void PlainTextImporter::emitMQLWords(std::ostream *pOut)
{
	std::list<FeatureInfo> feature_infos_word;
	feature_infos_word.push_back(FeatureInfo("surface",
						 "",
						 FEATURE_TYPE_STRING 
						 | FEATURE_TYPE_FROM_SET 
						 | FEATURE_TYPE_WITH_INDEX, 
						 ""));
	feature_infos_word.push_back(FeatureInfo("stripped_surface",
						 "",
						 FEATURE_TYPE_STRING 
						 | FEATURE_TYPE_FROM_SET 
						 | FEATURE_TYPE_WITH_INDEX, 
						 ""));
	emitMQLObjects(pOut, feature_infos_word, m_words, "Word");
}
            

void PlainTextImporter::cleanObjects(PEMOList& pemo_list)
{
	PEMOList::iterator it = pemo_list.begin();
	PEMOList::iterator itend = pemo_list.end();
	while (it != itend) {
		delete *it;
		++it;
	}
	pemo_list.erase(pemo_list.begin(), pemo_list.end());
}	

void PlainTextImporter::putSchema(std::ostream *pOut)
{
	// FIXME: Implement
	(*pOut) << plaintext_schema << std::endl;
}


void PlainTextImporter::emitMQLObjects(std::ostream *pOut, const std::list<FeatureInfo>& feature_infos, PEMOList& pemo_list, std::string object_type_name)
{
	int count = 0;
	PEMOList::iterator it = pemo_list.begin();
	PEMOList::iterator itend = pemo_list.end();

	while (it != itend) {
		if (count == 0) {
			(*pOut) << "BEGIN TRANSACTION GO\n"
				<< "CREATE OBJECTS WITH OBJECT TYPE [" << object_type_name << "]\n";
		}
		count += (*it)->putMQL(pOut, feature_infos, "");
		if (count >= 50000) {
			count = 0;
			(*pOut) << "GO\n"
				<< "COMMIT TRANSACTION GO\n";
		}
		++it;
	}
	if (count != 0) {
		(*pOut) << "GO\n"
			<< "COMMIT TRANSACTION GO\n";
	}
}
