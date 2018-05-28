/*
 * unboundbibleimporter.cpp
 *
 * A class to import Unbound Bible files into Emdros
 * Created: 12/16-2005
 * Last update: 4/27-2013
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2005-2013  Ulrik Sandborg-Petersen
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

#include <monads.h>
#include <iostream>
#include <unboundbibleimporter.h>
#include <debug.h>
#include "simplebibleschema.h"

/////////////////////////////////////////////////////////////////
//
// UnboundBibleImporter
//
/////////////////////////////////////////////////////////////////


bool UnboundBibleImporter::readDocumentFromStream(std::istream *pIn)
{
	std::map<std::string, std::string> namesmap;
	namesmap.insert(std::pair<std::string, std::string>("01O", "Genesis"));
	namesmap.insert(std::pair<std::string, std::string>("02O", "Exodus"));
	namesmap.insert(std::pair<std::string, std::string>("03O", "Leviticus"));
	namesmap.insert(std::pair<std::string, std::string>("04O", "Numbers"));
	namesmap.insert(std::pair<std::string, std::string>("05O", "Deuteronomy"));
	namesmap.insert(std::pair<std::string, std::string>("06O", "Joshua"));
	namesmap.insert(std::pair<std::string, std::string>("07O", "Judges"));
	namesmap.insert(std::pair<std::string, std::string>("08O", "Ruth"));
	namesmap.insert(std::pair<std::string, std::string>("09O", "I_Samuel"));
	namesmap.insert(std::pair<std::string, std::string>("10O", "II_Samuel"));
	namesmap.insert(std::pair<std::string, std::string>("11O", "I_Kings"));
	namesmap.insert(std::pair<std::string, std::string>("12O", "II_Kings"));
	namesmap.insert(std::pair<std::string, std::string>("13O", "I_Chronicles"));
	namesmap.insert(std::pair<std::string, std::string>("14O", "II_Chronicles"));
	namesmap.insert(std::pair<std::string, std::string>("15O", "Ezra"));
	namesmap.insert(std::pair<std::string, std::string>("16O", "Nehemiah"));
	namesmap.insert(std::pair<std::string, std::string>("17O", "Esther"));
	namesmap.insert(std::pair<std::string, std::string>("18O", "Job"));
	namesmap.insert(std::pair<std::string, std::string>("19O", "Psalms"));
	namesmap.insert(std::pair<std::string, std::string>("20O", "Proverbs"));
	namesmap.insert(std::pair<std::string, std::string>("21O", "Ecclesiastes"));
	namesmap.insert(std::pair<std::string, std::string>("22O", "Canticles"));
	namesmap.insert(std::pair<std::string, std::string>("23O", "Isaiah"));
	namesmap.insert(std::pair<std::string, std::string>("24O", "Jeremiah"));
	namesmap.insert(std::pair<std::string, std::string>("25O", "Lamentations"));
	namesmap.insert(std::pair<std::string, std::string>("26O", "Ezekiel"));
	namesmap.insert(std::pair<std::string, std::string>("27O", "Daniel"));
	namesmap.insert(std::pair<std::string, std::string>("28O", "Hosea"));
	namesmap.insert(std::pair<std::string, std::string>("29O", "Joel"));
	namesmap.insert(std::pair<std::string, std::string>("30O", "Amos"));
	namesmap.insert(std::pair<std::string, std::string>("31O", "Obadiah"));
	namesmap.insert(std::pair<std::string, std::string>("32O", "Jonah"));
	namesmap.insert(std::pair<std::string, std::string>("33O", "Micah"));
	namesmap.insert(std::pair<std::string, std::string>("34O", "Nahum"));
	namesmap.insert(std::pair<std::string, std::string>("35O", "Habakkuk"));
	namesmap.insert(std::pair<std::string, std::string>("36O", "Zephaniah"));
	namesmap.insert(std::pair<std::string, std::string>("37O", "Haggai"));
	namesmap.insert(std::pair<std::string, std::string>("38O", "Zechariah"));
	namesmap.insert(std::pair<std::string, std::string>("39O", "Malachi"));
	namesmap.insert(std::pair<std::string, std::string>("40N", "Matthew"));
	namesmap.insert(std::pair<std::string, std::string>("41N", "Mark"));
	namesmap.insert(std::pair<std::string, std::string>("42N", "Luke"));
	namesmap.insert(std::pair<std::string, std::string>("43N", "John"));
	namesmap.insert(std::pair<std::string, std::string>("44N", "Acts"));
	namesmap.insert(std::pair<std::string, std::string>("45N", "Romans"));
	namesmap.insert(std::pair<std::string, std::string>("46N", "I_Corinthians"));
	namesmap.insert(std::pair<std::string, std::string>("47N", "II_Corinthians"));
	namesmap.insert(std::pair<std::string, std::string>("48N", "Galatians"));
	namesmap.insert(std::pair<std::string, std::string>("49N", "Ephesians"));
	namesmap.insert(std::pair<std::string, std::string>("50N", "Philippians"));
	namesmap.insert(std::pair<std::string, std::string>("51N", "Colossians"));
	namesmap.insert(std::pair<std::string, std::string>("52N", "I_Thessalonians"));
	namesmap.insert(std::pair<std::string, std::string>("53N", "II_Thessalonians"));
	namesmap.insert(std::pair<std::string, std::string>("54N", "I_Timothy"));
	namesmap.insert(std::pair<std::string, std::string>("55N", "II_Timothy"));
	namesmap.insert(std::pair<std::string, std::string>("56N", "Titus"));
	namesmap.insert(std::pair<std::string, std::string>("57N", "Philemon"));
	namesmap.insert(std::pair<std::string, std::string>("58N", "Hebrews"));
	namesmap.insert(std::pair<std::string, std::string>("59N", "James"));
	namesmap.insert(std::pair<std::string, std::string>("60N", "I_Peter"));
	namesmap.insert(std::pair<std::string, std::string>("61N", "II_Peter"));
	namesmap.insert(std::pair<std::string, std::string>("62N", "I_John"));
	namesmap.insert(std::pair<std::string, std::string>("63N", "II_John"));
	namesmap.insert(std::pair<std::string, std::string>("64N", "III_John"));
	namesmap.insert(std::pair<std::string, std::string>("65N", "Jude"));
	namesmap.insert(std::pair<std::string, std::string>("66N", "Revelation"));
	namesmap.insert(std::pair<std::string, std::string>("67A", "Baruch"));
	namesmap.insert(std::pair<std::string, std::string>("68A", "Sirach"));
	namesmap.insert(std::pair<std::string, std::string>("69A", "I_Esdras"));
	namesmap.insert(std::pair<std::string, std::string>("70A", "II_Esdras"));
	namesmap.insert(std::pair<std::string, std::string>("71A", "Judith"));
	namesmap.insert(std::pair<std::string, std::string>("72A", "Epistle_to_the_Laodicaeans"));
	namesmap.insert(std::pair<std::string, std::string>("73A", "I_Maccabees"));
	namesmap.insert(std::pair<std::string, std::string>("74A", "II_Maccabees"));
	namesmap.insert(std::pair<std::string, std::string>("75A", "III_Maccabees"));
	namesmap.insert(std::pair<std::string, std::string>("76A", "IV_Maccabees"));
	namesmap.insert(std::pair<std::string, std::string>("77A", "Prayer_of_Manasseh"));
	namesmap.insert(std::pair<std::string, std::string>("78A", "Psalm_151"));
	namesmap.insert(std::pair<std::string, std::string>("79A", "Tobit"));
	namesmap.insert(std::pair<std::string, std::string>("80A", "Wisdom_of_Solomon"));
	namesmap.insert(std::pair<std::string, std::string>("81A", "Susanna"));
	namesmap.insert(std::pair<std::string, std::string>("82A", "Bel_and_the_Dragon"));
	namesmap.insert(std::pair<std::string, std::string>("83A", "Prayer_of_Azariah")); // Also Song of the three young men
	namesmap.insert(std::pair<std::string, std::string>("84A", "Psalms_of_Solomon"));
	namesmap.insert(std::pair<std::string, std::string>("85A", "Odes"));
	namesmap.insert(std::pair<std::string, std::string>("86A", "Additions_to_Esther"));
	namesmap.insert(std::pair<std::string, std::string>("19A", "Psalms_Vulgata"));
	namesmap.insert(std::pair<std::string, std::string>("25A", "Letter_of_Jeremiah"));

	m_last_book = "";
	m_last_chapter = 0;
	m_last_verse = 0;
	m_curmonad = 1;
	
	while (true) {
		std::string line;
		getline(*pIn, line);
		line = strip(line); // Remove whitspace from both ends
		if (line.length() == 0) {
			break;
		} else {
			std::vector<std::string> arr;
			split_string(line, " \t\r\n", arr);
			std::string book;
			try {
				book = namesmap[arr[0]];
			} catch (...) {
				// This probably means that the 
				// string in arr[0] was not in namesmap...
				// and so the format of the bible is wrong...
				return false;
			}
			if (arr.size() <= 3
			    || !string_is_number(arr[1]) 
			    || !string_is_number(arr[2])) {
				std::cerr << "Skipping line: " << line << std::endl;
			} else {
				long chapter = string2long(arr[1]);
				long verse = string2long(arr[2]);
				if (arr.size() > 3) {
					add_bcv(book, chapter, verse);
					std::vector<std::string>::size_type i;
					for (i = 3;
					     i < arr.size();
					     ++i) {
						add_word(arr[i]);
					}
				}
			}
		}
	}
	if (m_curmonad != 1) {
		closeLastObject(m_books);
		closeLastObject(m_chapters);
		closeLastObject(m_verses);
	}
	return true;
}

void UnboundBibleImporter::add_bcv(const std::string& book, long chapter, long verse)
{
	if (book != m_last_book) {
		if (m_last_book != "") {
			closeLastObject(m_books);
			closeLastObject(m_chapters);
			closeLastObject(m_verses);
		}
		createBook(book);
		createChapter(book, chapter);
		createVerse(book, chapter, verse);
	} else if (chapter != m_last_chapter) {
		closeLastObject(m_chapters);
		closeLastObject(m_verses);
		createChapter(book, chapter);
		createVerse(book, chapter, verse);
	} else {
		closeLastObject(m_verses);
		createVerse(book, chapter, verse);
	}
	m_last_book = book;
	m_last_chapter = chapter;
	m_last_verse = verse;
}

EmdrosMemObject UnboundBibleImporter::createBook(const std::string& bookname)
{
	EmdrosMemObject obj(m_curmonad);
	obj.setFeature("book", bookname);
	m_books.push_back(obj);
	return obj;
}

EmdrosMemObject UnboundBibleImporter::createChapter(const std::string& bookname, long chapter)
{
	EmdrosMemObject obj(m_curmonad);
	obj.setFeature("book", bookname);
	obj.setFeature("chapter", long2string(chapter));
	m_chapters.push_back(obj);
	return obj;
}

EmdrosMemObject UnboundBibleImporter::createVerse(const std::string& bookname, long chapter, long verse)
{
	EmdrosMemObject obj(m_curmonad);
	obj.setFeature("book", bookname);
	obj.setFeature("chapter", long2string(chapter));
	obj.setFeature("verse", long2string(verse));
	m_verses.push_back(obj);
	return obj;
}

void UnboundBibleImporter::closeLastObject(EMOList& list)
{
	EmdrosMemObject &obj = *list.rbegin();
	const SetOfMonads &monads = obj.getMonads();
	obj.addMonads(SetOfMonads(monads.first(), m_curmonad-1));
}

void UnboundBibleImporter::add_word(const std::string& surface)
{
	EmdrosMemObject obj(m_curmonad);
	obj.setFeature("surface", surface);

	std::string stripped_surface;

	// Remove punctuation and space
	stripped_surface = remove_punct(surface);
	stripped_surface = remove_char(stripped_surface, ' ');
	
	obj.setFeature("stripped_surface", stripped_surface);

	m_words.push_back(obj);
	++m_curmonad;
}

void UnboundBibleImporter::putSchema(std::ostream *pOut)
{
	std::string schema(simple_bible_schema);

	(*pOut) << schema << std::endl;
}

void UnboundBibleImporter::putCorpusAsMQL(std::ostream *pOut)
{
	EMOList::const_iterator ci, cend, ctmp;
	
	// Books
	(*pOut) << "////////////////////////////////\n"
		<< "//\n"
		<< "// Books\n"
		<< "//\n"
		<< "////////////////////////////////\n";
	(*pOut) << "BEGIN TRANSACTION GO\n";
	std::list<FeatureInfo> book_feature_infos;
	book_feature_infos.push_back(FeatureInfo("book", 
						 FEATURE_TYPE_ENUM, 
						 "Genesis", 
						 false));
	(*pOut) << "CREATE OBJECTS WITH OBJECT TYPE [Book]\n";
	for (ci = m_books.begin();
	     ci != m_books.end();
	     ++ci) {
		ci->putMQL(pOut, book_feature_infos, "");
	}
	(*pOut) << "GO\nCOMMIT TRANSACTION GO\n\n";


	// Chapters
	(*pOut) << "////////////////////////////////\n"
		<< "//\n"
		<< "// Chapters\n"
		<< "//\n"
		<< "////////////////////////////////\n";
	(*pOut) << "BEGIN TRANSACTION GO\n";
	std::list<FeatureInfo> chapter_feature_infos;
	chapter_feature_infos.push_back(FeatureInfo("book", 
						    FEATURE_TYPE_ENUM, 
						    "Genesis", 
						    false));
	chapter_feature_infos.push_back(FeatureInfo("chapter", 
						    FEATURE_TYPE_INTEGER,
						    "1", 
						    false));
	(*pOut) << "CREATE OBJECTS WITH OBJECT TYPE [Chapter]\n";
	for (ci = m_chapters.begin();
	     ci != m_chapters.end();
	     ++ci) {
		ci->putMQL(pOut, chapter_feature_infos, "");
	}
	(*pOut) << "GO\nCOMMIT TRANSACTION GO\n\n";


	// Verses
	(*pOut) << "////////////////////////////////\n"
		<< "//\n"
		<< "// Verses\n"
		<< "//\n"
		<< "////////////////////////////////\n";
	(*pOut) << "BEGIN TRANSACTION GO\n";
	std::list<FeatureInfo> verse_feature_infos;
	verse_feature_infos.push_back(FeatureInfo("book", 
						  FEATURE_TYPE_ENUM, 
						  "Genesis", 
						  false));
	verse_feature_infos.push_back(FeatureInfo("chapter", 
						  FEATURE_TYPE_INTEGER,
						  "1", 
						  false));
	verse_feature_infos.push_back(FeatureInfo("verse", 
						  FEATURE_TYPE_INTEGER,
						  "1", 
						  false));
	(*pOut) << "CREATE OBJECTS WITH OBJECT TYPE [Verse]\n";
	for (ci = m_verses.begin();
	     ci != m_verses.end();
	     ++ci) {
		ci->putMQL(pOut, verse_feature_infos, "");
	}
	(*pOut) << "GO\nCOMMIT TRANSACTION GO\n\n";

	// Words
	(*pOut) << "////////////////////////////////\n"
		<< "//\n"
		<< "// Words\n"
		<< "//\n"
		<< "////////////////////////////////\n";
	std::list<FeatureInfo> word_feature_infos;
	word_feature_infos.push_back(FeatureInfo("surface", 
						 FEATURE_TYPE_STRING, 
						 "", 
						 false));
	word_feature_infos.push_back(FeatureInfo("stripped_surface", 
						 FEATURE_TYPE_STRING, 
						 "", 
						 false));
	bool bTransactionInProgress = true;
	(*pOut) << "BEGIN TRANSACTION GO\n";
	(*pOut) << "CREATE OBJECTS WITH OBJECT TYPE [Word]\n";

	long counter = 1;
	for (ci = m_words.begin();
	     ci != m_words.end();
	     ++ci) {
		ci->putMQL(pOut, word_feature_infos, "");
		++counter;
		if ((counter % 50000) == 1) {
			(*pOut) << "GO\nCOMMIT TRANSACTION GO\n";
			ctmp = ci;
			++ctmp;
			if (ctmp != m_words.end()) {
				(*pOut) << "BEGIN TRANSACTION GO\n";
				(*pOut) << "CREATE OBJECTS WITH OBJECT TYPE [Word]\n";
				bTransactionInProgress = true;
			} else {
				bTransactionInProgress = false;
			}
			
		}
	}
	if (bTransactionInProgress) {
		(*pOut) << "GO\nCOMMIT TRANSACTION GO\n\n";
	}
}
