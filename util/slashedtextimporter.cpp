/*
 * slashedtextimporter.cpp
 *
 * A class to import slashed text files
 * into Emdros
 * Created: 8/8-2006
 * Last update: 12/26-2006
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2006  Ulrik Petersen
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
	feature_infos_word.push_back(FeatureInfo("surface", FEATURE_TYPE_STRING | FEATURE_TYPE_FROM_SET | FEATURE_TYPE_WITH_INDEX, "", false));
	feature_infos_word.push_back(FeatureInfo("tag", FEATURE_TYPE_STRING | FEATURE_TYPE_FROM_SET | FEATURE_TYPE_WITH_INDEX, "", false));
	feature_infos_word.push_back(FeatureInfo("lemma", FEATURE_TYPE_STRING | FEATURE_TYPE_FROM_SET | FEATURE_TYPE_WITH_INDEX, "", false));
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


