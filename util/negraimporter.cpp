/*
 * negra_importer.cpp
 *
 * A class to import NeGRA files (such as the TIGERCorpus)
 * into Emdros
 * Created: 12/16-2005
 * Last update: 4/11-2017
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


#include <negraimporter.h>
#include <emdf.h>
#include <list>
#include <map>
#include <sstream>
#include <iostream>
#include <string>
#include <string_func.h>
#include <fstream>
#include "negraschema.h"


//////////////////////////////////////////////////////////////
//
// NEGRANode
// 
//////////////////////////////////////////////////////////////

NEGRANode::NEGRANode(const std::string& morphtag,
		     const std::string& edge,
		     id_d_t parent,
		     const StringID_DPairList& secedges)
	: m_morphtag(morphtag),
	  m_edge(edge),
	  m_id_d(NIL),
	  m_parentID_D(NIL),
	  m_secedges(secedges),
	  m_parent(parent)
{
}

NEGRANode::~NEGRANode()
{
}

void NEGRANode::setParentID_Ds(std::map<id_d_t, id_d_t>& nonterm_id_to_id_d_map, id_d_t sentenceID_D) 
{
	if (m_parent >= 500) {
		m_parentID_D = nonterm_id_to_id_d_map[m_parent];
	} else if (m_parent == 0) {
		m_parentID_D = sentenceID_D;
	}
	for (StringID_DPairList::const_iterator ci = m_secedges.begin();
	     ci != m_secedges.end();
	     ++ci) {
		m_secedges_by_id_d.push_back(std::pair<std::string, id_d_t>(ci->first, nonterm_id_to_id_d_map[ci->second]));
	}
}




void NEGRANode::setParentsFeature(std::map<id_d_t, id_d_t> nonterm_id_to_id_d_map, std::map<id_d_t, NEGRANonTerminal*> nonterminal_map, id_d_t sentenceID_D)
{
	m_parents.erase(m_parents.begin(), m_parents.end()); // FIXME: Is this really necessary?
	id_d_t cur_parent = m_parent;
	if (cur_parent != 0) {
		m_parents.push_back(nonterm_id_to_id_d_map[cur_parent]);
		while (cur_parent != 0) {
			NEGRANonTerminal *pNonTerm = nonterminal_map[cur_parent];
			cur_parent = pNonTerm->getParent();
			if (cur_parent != 0) {
				m_parents.push_back(nonterm_id_to_id_d_map[cur_parent]);
			}
		}
	}
	m_parents.push_back(sentenceID_D);
}




//////////////////////////////////////////////////////////////
//
// NEGRATerminal
// 
//////////////////////////////////////////////////////////////

NEGRATerminal::NEGRATerminal(const std::string& surface,
			     const std::string& postag,
			     const std::string& morphtag,
			     const std::string& edge,
			     id_d_t parent,
			     const StringID_DPairList&  secedges,
			     monad_m monad) 
	: NEGRANode(morphtag, edge, parent, secedges)
{
	m_surface = surface;
	m_postag = postag;
	m_id_d = 0;
	m_monad = monad;
}


id_d_t NEGRATerminal::addID_D(id_d_t currentID_D)
{
	m_id_d = currentID_D;
	return currentID_D + 1;
}

/* Adds m_monad to nonterminals that are referenced by
   m_parent*/ 
void NEGRATerminal::addToParents(std::map<id_d_t, NEGRANonTerminal*>& nonterminals) 
{
	if (m_parent < 500) {
		; // Nothing to do.
	} else {
		nonterminals[m_parent]->addMonad(m_monad);
	}
}




void NEGRATerminal::dump() const 
{
	std::cout << "W: " << m_surface << " " << " " << m_postag << " " << 
		m_morphtag << " " << m_edge << " " << m_parent << " " << m_monad << " \n"; // FIXME: Print secedges
}

void NEGRATerminal::emitMQL(std::ostream *pOut) 
{
	(*pOut) << "CREATE OBJECT FROM MONADS={" << m_monad << "}\nWITH ID_D=" << m_id_d << "\n[\n";
	(*pOut) << "  surface:=\"" << encodeSTRINGstring(m_surface) << "\";\n";
	(*pOut) << "  postag:=\"" << encodeSTRINGstring(m_postag) << "\";\n";
	(*pOut) << "  morphtag:=\"" << encodeSTRINGstring(m_morphtag) << "\";\n";
	(*pOut) << "  edge:=\"" << encodeSTRINGstring(m_edge) << "\";\n";
	(*pOut) << "  parent:=" <<  m_parentID_D << ";\n";
	(*pOut) << "  parents:=(" << joinList(",", m_parents) << ");\n";

	int secedgeslen = m_secedges_by_id_d.size();
	int index = 0;
	while (index < secedgeslen) {
		(*pOut) << "  secedge" << index+1 << ":=\"" << encodeSTRINGstring(m_secedges_by_id_d[index].first) << "\";\n";
		(*pOut) << "  secparent" << index+1 << ":=" << m_secedges_by_id_d[index].second << ";\n";
		++index;
	}
	(*pOut) << "]\n";
}


//////////////////////////////////////////////////////////////
//
// NEGRANonTerminal
// 
//////////////////////////////////////////////////////////////

NEGRANonTerminal::NEGRANonTerminal(id_d_t id, 
				   const std::string& tag, 
				   const std::string& morphtag, 
				   const std::string& edge, 
				   id_d_t parent, 
				   const StringID_DPairList& secedges)
	: NEGRANode(morphtag, edge, parent, secedges)
{
	m_id = id;
	m_tag = tag;
}


void NEGRANonTerminal::addMonads(const SetOfMonads& som)
{
	m_som.unionWith(som);
}

void NEGRANonTerminal::addMonad(monad_m m)
{
	m_som.add(m);
}


id_d_t NEGRANonTerminal::addID_D(id_d_t currentID_D, std::map<id_d_t, id_d_t>& nonterm_id_to_id_d_map)
{
	m_id_d = currentID_D;
	nonterm_id_to_id_d_map[m_id] = m_id_d;
	return currentID_D + 1;
}

void NEGRANonTerminal::dump()
{
	std::cout << "P: " << m_id << ", " << m_tag << ", " << m_morphtag << ", " << m_edge << ", " << m_parent << ", " << m_som.toString() << ", " << std::endl; // FIXME: Print secedges
}


void NEGRANonTerminal::emitMQL(std::ostream *pOut)
{
	(*pOut) << "CREATE OBJECT FROM MONADS=" << m_som.toString() << "\nWITH ID_D=" << m_id_d << "\n[\n";
	(*pOut) << "  tag:=\"" << encodeSTRINGstring(m_tag) << "\";\n" ;
	(*pOut) << "  morphtag:=\"" << encodeSTRINGstring(m_morphtag) << "\";\n";
	(*pOut) << "  edge:=\"" << encodeSTRINGstring(m_edge) << "\";\n";
	(*pOut) << "  parent:=" << m_parentID_D << ";\n";
	(*pOut) << "  parents:=(" << joinList(",", m_parents) << ");\n";

	int secedgeslen = m_secedges_by_id_d.size();
	int index = 0;
	while (index < secedgeslen) {
		(*pOut) << "  secedge" << index+1 << ":=\"" << encodeSTRINGstring(m_secedges_by_id_d[index].first) << "\";\n";
		(*pOut) << "  secparent" << index+1 << ":=" << m_secedges_by_id_d[index].second << ";\n";
		++index;
	}
	(*pOut) << "]\n";
}


/* Adds m_som to nonterminals that are referenced by
   m_parent*/ 
void NEGRANonTerminal::addToParents(std::map<id_d_t, NEGRANonTerminal*>& nonterminals) 
{
	if (m_parent < 500) {
		; // Nothing to do.
	} else {
		nonterminals[m_parent]->addMonads(m_som);
	}
}

//////////////////////////////////////////////////////////////
//
// NEGRASentence
// 
//////////////////////////////////////////////////////////////

    
NEGRASentence::NEGRASentence(id_d_t sentence_id, 
			     const std::string& editor_id, 
			     const std::string& date, 
			     const std::string& origin_id, 
			     monad_m starting_monad)
{
	m_sentence_id = sentence_id;
	m_editor_id = editor_id;
	m_date = date;
	m_origin_id = origin_id;
	m_id_d = 0;
	m_starting_monad = starting_monad;
}

void NEGRASentence::addTerminal(NEGRATerminal *pTerminal)
{
	m_terminals.push_back(pTerminal);
}

void NEGRASentence::addNonTerminal(NEGRANonTerminal *pNonTerminal)
{
	m_nonterminals[pNonTerminal->getID()] = pNonTerminal;
}

/* Apply monads and make graph structure. */
id_d_t NEGRASentence::finalize(id_d_t currentID_D)
{
	unsigned int index;
	for (index = 0;
	     index < m_terminals.size();
	     ++index) {
		NEGRATerminal *pTerminal = m_terminals[index];
		pTerminal->addToParents(m_nonterminals);
		currentID_D = pTerminal->addID_D(currentID_D);
		m_nonterm_id_to_id_d_map[index] = pTerminal->getID_D();
	}

	std::map<id_d_t, NEGRANonTerminal*>::iterator it;
	for (it = m_nonterminals.begin();
	     it != m_nonterminals.end();
	     ++it) {
		it->second->addToParents(m_nonterminals);
		currentID_D = it->second->addID_D(currentID_D, m_nonterm_id_to_id_d_map);
	}

	for (index = 0;
	     index < m_terminals.size();
	     ++index) {
		NEGRATerminal *pTerminal = m_terminals[index];
		pTerminal->setParentID_Ds(m_nonterm_id_to_id_d_map, currentID_D);
	}
        
    
	for (it = m_nonterminals.begin();
	     it != m_nonterminals.end();
	     ++it) {
		it->second->setParentID_Ds(m_nonterm_id_to_id_d_map, currentID_D);
	}
        

	for (index = 0;
	     index < m_terminals.size();
	     ++index) {
		NEGRATerminal *pTerminal = m_terminals[index];
		pTerminal->setParentsFeature(m_nonterm_id_to_id_d_map, m_nonterminals, currentID_D);
	}

	for (it = m_nonterminals.begin();
	     it != m_nonterminals.end();
	     ++it) {
		it->second->setParentsFeature(m_nonterm_id_to_id_d_map, m_nonterminals, currentID_D);
	}
    
	m_id_d = currentID_D;
	++currentID_D;
	return currentID_D;
}
        

void NEGRASentence::dump()
{
	std::cout << "S: " << m_sentence_id<< " " << m_editor_id<< " " << m_date<< " " << m_origin_id<< " " << m_starting_monad;
	unsigned int index;
	for (index = 0;
	     index < m_terminals.size();
	     ++index) {
		NEGRATerminal *pTerminal = m_terminals[index];
		pTerminal->dump();
	}
	
	std::map<id_d_t, NEGRANonTerminal*>::iterator it;
	for (it = m_nonterminals.begin();
	     it != m_nonterminals.end();
	     ++it) {
		it->second->dump();
	}
}

unsigned int NEGRASentence::emitMQLWords(std::ostream* pOut)
{
	unsigned int index;
	for (index = 0;
	     index < m_terminals.size();
	     ++index) {
		NEGRATerminal *pTerminal = m_terminals[index];
		pTerminal->emitMQL(pOut);
	}
	return index;
}

unsigned int NEGRASentence::emitMQLPhrases(std::ostream* pOut)
{
	unsigned int index = 0;
	std::map<id_d_t, NEGRANonTerminal*>::iterator it;
	for (it = m_nonterminals.begin();
	     it != m_nonterminals.end();
	     ++it) {
		it->second->emitMQL(pOut);
		++index;
	}
	return index;
}

unsigned int NEGRASentence::emitMQLSelf(std::ostream* pOut)
{
	(*pOut) << "CREATE OBJECT FROM MONADS={" 
		<< m_terminals[0]->getMonad() 
		<< '-' 
		<< (*m_terminals.rbegin())->getMonad() 
		<< "}\nWITH ID_D=" << m_id_d << "\n[\n";
	(*pOut) << "  sentence_id:=" << m_sentence_id << ";\n";
	(*pOut) << "  editor_id:=\"" << encodeSTRINGstring(m_editor_id) << "\";\n";
	(*pOut) << "  date:=" << m_date << ";\n";
	(*pOut) << "  origin_id:=\""<< encodeSTRINGstring(m_origin_id) << "\";\n";
	(*pOut) << "]\n\n";

	return 1;
}

monad_m NEGRASentence::getNoOfMonads() const
{
	return (*m_terminals.rbegin())->getMonad() - (*m_terminals.begin())->getMonad() + 1;
}
    


    
//////////////////////////////////////////////////////////////
//
// NEGRAImporter
// 
//////////////////////////////////////////////////////////////


const int state_init = 0;
const int state_table = 1;
const int state_BOS = 2;
const int state_sentence = 3;
const int state_EOS = 4;

NEGRAImporter::NEGRAImporter(monad_m starting_monad, 
			     id_d_t starting_id_d)
	: EmdrosImporterBase(starting_monad, starting_id_d)
{
	m_state = state_init;
	m_pCurrent_sentence = 0;
	m_count = 0;
}

NEGRAImporter::~NEGRAImporter()
{
	cleanSentences();
}

bool NEGRAImporter::readDocumentFromStream(std::istream *pIn)
{
	while (true) {
		std::string line;
		getline(*pIn, line);
		line = remove_from_substring_and_out(line, "%%");
		StringVector arr;
		split_string(strip(line), " \t\r\n", arr);
		if (arr.size() == 1
		    || line.length() == 0) {
			; // Do nothing...
		} else if (line[0] == '#') {
			m_state = parseHashLine(line);
			if (m_state == state_BOS) {
				parseBOS(line);
				m_state = state_sentence;
			} else if (m_state == state_EOS) {
				parseEOS(line);
				m_state = state_init;
			} else if (m_state == state_sentence) {
				parseSentenceLine(line);
			}
		} else if (m_state == state_sentence) {
			parseSentenceLine(line);
		} else if (m_state == state_table) {
			; // Nothing to do
		}

		if (pIn->eof()) {
			break;
		}
	}
	return true;
}

int NEGRAImporter::parseHashLine(const std::string& line)
{
	if (line.substr(0,7) == "#FORMAT") {
		return state_init;
	} else if (line.substr(0,4) == "#BOT") {
		return state_table;
	} else if (line.substr(0,4) == "#EOT") {
		return state_init;
	} else if (line.substr(0,4) == "#BOS") {
		return state_BOS;
	} else if (line.substr(0,4) == "#EOS") {
		return state_EOS;
	} else {
		return state_sentence;
	}
}

void NEGRAImporter::parseSentenceLine(const std::string& line)
{
	StringVector items;
	split_string(line, " \t\r\n\v", items);
	if (items[0][0] == '#') {
		parsePhraseLine(items);
	} else {
		parseWordLine(items);
	}
}    

void NEGRAImporter::parseWordLine(const StringVector& items)
{
	std::string surface = items[0];
	std::string postag = items[1];
	std::string morphtag = items[2];
	std::string edge = items[3];
	id_d_t parent = string2id_d(items[4]);
	StringID_DPairList secedges;
	if (items.size() > 5) {
		secedges = parseSecEdges(items, 5);
	} else {
		; // Leave secedges empty
	}
	NEGRATerminal *pWord = new NEGRATerminal(surface, postag, morphtag, edge, parent, secedges, m_cur_monad);
	++m_cur_monad;
	m_pCurrent_sentence->addTerminal(pWord);
}

void NEGRAImporter::parsePhraseLine(const StringVector& items)
{
	id_d_t id = string2id_d(items[0].substr(1));
	std::string tag = items[1];
	std::string morphtag = items[2];
	std::string edge = items[3];
	id_d_t parent = string2id_d(items[4]);
	StringID_DPairList secedges;
	if (items.size() > 5) {
		secedges = parseSecEdges(items, 5);
	} else {
		; // Leave secedges empty
	}
	
	NEGRANonTerminal *pPhrase = new NEGRANonTerminal(id, tag, morphtag, edge, parent, secedges);
	m_pCurrent_sentence->addNonTerminal(pPhrase);
}
 
void NEGRAImporter::parseBOS(std::string& line)
{
	StringVector items;
	split_string(line, " \t\r\n\v", items);
	id_d_t sentence_id = string2id_d(items[1]);
	std::string editor_id = items[2];
	std::string date = items[3];
	std::string origin_id = items[4];
	NEGRASentence *pSentence = new NEGRASentence(sentence_id, editor_id, date, origin_id, m_cur_monad);
	m_pCurrent_sentence = pSentence;
}

StringID_DPairList NEGRAImporter::parseSecEdges(const StringVector& secedges, StringVector::size_type starting_index)
{
	StringID_DPairList result;
	int state = 0;
	std::string curedge = "";
	id_d_t curparent = NIL;
	for (StringVector::size_type index = starting_index;
	     index < secedges.size();
	     ++index) {
		std::string it = secedges[index];
		if (state == 0) {
			curedge = it;
			state = 1;
		} else {
			state = 0;
			curparent = string2id_d(it);
			result.push_back(std::pair<std::string,id_d_t>(curedge, curparent));
		}
	}
	return result;
}

void NEGRAImporter::parseEOS(const std::string& line)
{
	UNUSED(line);
	m_cur_id_d = m_pCurrent_sentence->finalize(m_cur_id_d);
        m_sentences.push_back(m_pCurrent_sentence);
	/*
        m_count += m_pCurrent_sentence->getNoOfMonads()
        if self.bEmitMQLInStages and m_count > 100000:
            m_count = 0
            self.emitMQL()
            self.cleanSentences()
	*/
}

void NEGRAImporter::dump()
{
	PSentenceList::iterator it = m_sentences.begin();
	PSentenceList::iterator itend = m_sentences.end();
	while (it != itend) {
		(*it)->dump();
		++it;
	}
}

void NEGRAImporter::putCorpusAsMQL(std::ostream *pOut)
{
	emitMQLWords(pOut);
	emitMQLPhrases(pOut);
	emitMQLSentences(pOut);
	cleanSentences();
}

void NEGRAImporter::emitMQLWords(std::ostream *pOut)
{
	unsigned int count = 0;
	PSentenceList::iterator it = m_sentences.begin();
	PSentenceList::iterator itend = m_sentences.end();
	while (it != itend) {
		if (count == 0) {
			(*pOut) << "BEGIN TRANSACTION GO\n"
				<< "CREATE OBJECTS WITH OBJECT TYPE [Word]\n";
		}
		count += (*it)->emitMQLWords(pOut);
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
            
void NEGRAImporter::emitMQLPhrases(std::ostream *pOut)
{
	unsigned int count = 0;
	PSentenceList::iterator it = m_sentences.begin();
	PSentenceList::iterator itend = m_sentences.end();
	while (it != itend) {
		if (count == 0) {
			(*pOut) << "BEGIN TRANSACTION GO\n"
				<< "CREATE OBJECTS WITH OBJECT TYPE [Phrase]\n";
		}
		count += (*it)->emitMQLPhrases(pOut);
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
            
void NEGRAImporter::emitMQLSentences(std::ostream *pOut)
{
	int count = 0;
	PSentenceList::iterator it = m_sentences.begin();
	PSentenceList::iterator itend = m_sentences.end();
	while (it != itend) {
		if (count == 0) {
			(*pOut) << "BEGIN TRANSACTION GO\n"
				<< "CREATE OBJECTS WITH OBJECT TYPE [Sentence]\n";
		}
		count += (*it)->emitMQLSelf(pOut);
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
            
void NEGRAImporter::cleanSentences()
{
	PSentenceList::iterator it = m_sentences.begin();
	PSentenceList::iterator itend = m_sentences.end();
	while (it != itend) {
		delete *it;
		++it;
	}
	m_sentences.erase(m_sentences.begin(), m_sentences.end());
}	

void NEGRAImporter::putSchema(std::ostream *pOut)
{
	// FIXME: Implement
	(*pOut) << negra_schema << std::endl;
}


