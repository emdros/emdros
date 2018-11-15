/*
 * slashedtextimporter.cpp
 *
 * A class to import slashed text files
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


#include <slashedtextimporter.h>
#include <emdf.h>
#include <list>
#include <map>
#include <sstream>
#include <iostream>
#include <string>
#include <string_func.h>
#include <fstream>
#include <vector>
#include "slashedtextschema.h"

typedef std::vector<std::string> StringVector;


    
//////////////////////////////////////////////////////////////
//
// SlashedTextImporter
// 
//////////////////////////////////////////////////////////////


SlashedTextImporter::SlashedTextImporter(monad_m starting_monad, 
					 id_d_t starting_id_d,
					 const std::string& word_internal_separator,
					 const std::string& paragraph_separator_string)
	: EmdrosImporterBase(starting_monad, starting_id_d)
{
	m_word_internal_separator = word_internal_separator;
	m_paragraph_separator_string = paragraph_separator_string;
}

SlashedTextImporter::~SlashedTextImporter()
{
	cleanDocuments();
	cleanParagraphs();
	cleanWords();
}

bool SlashedTextImporter::readDocumentFromStream(std::istream *pIn)
{
	// Create document
	EmdrosMemObject *pDocument = new EmdrosMemObject(SetOfMonads(m_cur_monad), m_cur_id_d);
	++m_cur_id_d;
	m_documents.push_back(pDocument);

	std::string whole_document;
	while (true) {
		std::string line;
		getline(*pIn, line);
		whole_document += line;

		if (pIn->eof()) {
			break;
		}
	}

	StringVector paragraph_arr;
	split_string_on_string(whole_document, m_paragraph_separator_string, paragraph_arr);
	StringVector::const_iterator paragraph_ci = paragraph_arr.begin();
	StringVector::const_iterator paragraph_cend = paragraph_arr.end();
	while (paragraph_ci != paragraph_cend) {
		StringVector words_arr;
		split_string(*paragraph_ci, " \t\r\n", words_arr);
		if (words_arr.size() < 1) {
			; // Do nothing...
		} else {
			EmdrosMemObject *pParagraph = new EmdrosMemObject(SetOfMonads(m_cur_monad), m_cur_id_d);
			m_paragraphs.push_back(pParagraph);
			++m_cur_id_d;
			
			StringVector::const_iterator words_ci = words_arr.begin();
			StringVector::const_iterator words_cend = words_arr.end();
			while (words_ci != words_cend) {
				if (*words_ci != "") {
					StringVector single_word_arr;
					split_string_on_string(*words_ci, 
							       m_word_internal_separator,
							       single_word_arr);
					StringVector::size_type size = single_word_arr.size();
					if (size > 0) {
						EmdrosMemObject *pWord = new EmdrosMemObject(SetOfMonads(m_cur_monad), m_cur_id_d);
					
						pWord->setFeature("surface", single_word_arr[0]);

						if (size > 1) {
							pWord->setFeature("tag", single_word_arr[1]);
						} else {
							pWord->setFeature("tag","");
						}

						if (size > 2) {
							pWord->setFeature("lemma", single_word_arr[2]);
						} else {
							pWord->setFeature("lemma","");
						}

						m_words.push_back(pWord);
					
						// Add current monad to current document
						pDocument->addMonads(SetOfMonads(m_cur_monad));
					
						// Add current monad to current paragraph
						pParagraph->addMonads(SetOfMonads(m_cur_monad));
					
					
						++m_cur_monad;
						++m_cur_id_d;
					}
				}
				++words_ci;
			} 
		}
		
		++paragraph_ci;
	}

	return true;
}

void SlashedTextImporter::putCorpusAsMQL(std::ostream *pOut)
{
	emitMQLDocuments(pOut);
	emitMQLParagraphs(pOut);
	emitMQLWords(pOut);
}

void SlashedTextImporter::emitMQLDocuments(std::ostream *pOut)
{
	int count = 0;
	PEMOList::iterator it = m_documents.begin();
	PEMOList::iterator itend = m_documents.end();
	std::list<FeatureInfo> feature_infos_document; // No features...
	while (it != itend) {
		if (count == 0) {
			(*pOut) << "BEGIN TRANSACTION GO\n"
				<< "CREATE OBJECTS WITH OBJECT TYPE [Document]\n";
		}
		count += (*it)->putMQL(pOut, feature_infos_document, "");
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
            
void SlashedTextImporter::emitMQLParagraphs(std::ostream *pOut)
{
	int count = 0;
	PEMOList::iterator it = m_paragraphs.begin();
	PEMOList::iterator itend = m_paragraphs.end();
	std::list<FeatureInfo> feature_infos_paragraph; // No features...
	while (it != itend) {
		if (count == 0) {
			(*pOut) << "BEGIN TRANSACTION GO\n"
				<< "CREATE OBJECTS WITH OBJECT TYPE [Paragraph]\n";
		}
		count += (*it)->putMQL(pOut, feature_infos_paragraph, "");
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
            
void SlashedTextImporter::emitMQLWords(std::ostream *pOut)
{
	int count = 0;
	PEMOList::iterator it = m_words.begin();
	PEMOList::iterator itend = m_words.end();
	std::list<FeatureInfo> feature_infos_word;
	feature_infos_word.push_back(FeatureInfo("surface",
						 "",
						 FEATURE_TYPE_STRING | FEATURE_TYPE_FROM_SET | FEATURE_TYPE_WITH_INDEX,
						 ""));
	feature_infos_word.push_back(FeatureInfo("tag",
						 "",
						 FEATURE_TYPE_STRING | FEATURE_TYPE_FROM_SET | FEATURE_TYPE_WITH_INDEX,
						 ""));
	feature_infos_word.push_back(FeatureInfo("lemma",
						 "",
						 FEATURE_TYPE_STRING | FEATURE_TYPE_FROM_SET | FEATURE_TYPE_WITH_INDEX, ""));
	while (it != itend) {
		if (count == 0) {
			(*pOut) << "BEGIN TRANSACTION GO\n"
				<< "CREATE OBJECTS WITH OBJECT TYPE [Word]\n";
		}
		count += (*it)->putMQL(pOut, feature_infos_word, "");
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
            
void SlashedTextImporter::cleanWords()
{
	PEMOList::iterator it = m_words.begin();
	PEMOList::iterator itend = m_words.end();
	while (it != itend) {
		delete *it;
		++it;
	}
	m_words.erase(m_words.begin(), m_words.end());
}	

void SlashedTextImporter::cleanDocuments()
{
	PEMOList::iterator it = m_documents.begin();
	PEMOList::iterator itend = m_documents.end();
	while (it != itend) {
		delete *it;
		++it;
	}
	m_documents.erase(m_documents.begin(), m_documents.end());
}	

void SlashedTextImporter::cleanParagraphs()
{
	PEMOList::iterator it = m_paragraphs.begin();
	PEMOList::iterator itend = m_paragraphs.end();
	while (it != itend) {
		delete *it;
		++it;
	}
	m_paragraphs.erase(m_paragraphs.begin(), m_paragraphs.end());
}	

void SlashedTextImporter::putSchema(std::ostream *pOut)
{
	// FIXME: Implement
	(*pOut) << slashedtext_schema << std::endl;
}


