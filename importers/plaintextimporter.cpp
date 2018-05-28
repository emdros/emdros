/*
 * plaintextimporter.cpp
 *
 * A class to import plain text files
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
						 FEATURE_TYPE_STRING 
						 | FEATURE_TYPE_FROM_SET 
						 | FEATURE_TYPE_WITH_INDEX, 
						 "", 
						 false));
	feature_infos_word.push_back(FeatureInfo("stripped_surface", 
						 FEATURE_TYPE_STRING 
						 | FEATURE_TYPE_FROM_SET 
						 | FEATURE_TYPE_WITH_INDEX, 
						 "", 
						 false));
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
