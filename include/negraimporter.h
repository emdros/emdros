/*
 * negra_importer.h
 *
 * A class to import NeGRA files (such as the TIGERCorpus)
 * into Emdros
 * Created: 12/16-2005
 * Last update: 3/21-2013
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


#ifndef NEGRA_IMPORTER_EMDROS_H_
#define NEGRA_IMPORTER_EMDROS_H_

#include "emdf.h"
#include <list>
#include <map>
#include <vector>
#include <sstream>
#include <string>
#include "string_func.h"
#include "monads.h"
#include "pennimporter.h" // For EmdrosImporterbase 

typedef std::pair<id_d_t, id_d_t> ID_DPair;

typedef std::list<ID_DPair> ID_DPairList;

typedef std::pair<std::string, id_d_t> StringID_DPair;

typedef std::list<StringID_DPair> StringID_DPairList;

typedef std::vector<StringID_DPair> StringID_DPairVector;


typedef std::list<id_d_t> ID_DList;

typedef std::map<id_d_t, id_d_t> ID_DPairMap;

typedef std::vector<ID_DPair> ID_DPairVector;

typedef std::vector<std::string> StringVector;

typedef std::map<std::string, std::string> StringStringMap;


class NEGRANonTerminal; // Forward declaration

class NEGRANode {
protected:
	std::string m_morphtag, m_edge;
	id_d_t m_id_d, m_parentID_D;
	StringID_DPairList m_secedges;
	StringID_DPairVector m_secedges_by_id_d;
	ID_DList m_parents;
	id_d_t m_parent;
public:
	NEGRANode(const std::string& morphtag,
		  const std::string& edge,
		  id_d_t parent,
		  const StringID_DPairList& secedges);
	~NEGRANode();
	void setParentID_Ds(std::map<id_d_t, id_d_t>& nonterm_id_to_id_d_dict, id_d_t sentenceID_D);
	void setParentsFeature(std::map<id_d_t, id_d_t> nonterm_id_to_id_d_dict, std::map<id_d_t, NEGRANonTerminal*> nonterminal_dict, id_d_t sentenceID_D);
	id_d_t getParentID_D(void) const { return m_parentID_D; };
	id_d_t getParent(void) const { return m_parent; };
};


class NEGRANonTerminal : public NEGRANode {
protected:
	id_d_t m_id;
	std::string m_tag;
	SetOfMonads m_som;
public:
	id_d_t getID(void) const { return m_id; };
	NEGRANonTerminal(id_d_t id, 
			 const std::string& tag, 
			 const std::string& morphtag, 
			 const std::string& edge, 
			 id_d_t parent, 
			 const StringID_DPairList& secedges);
	void addMonads(const SetOfMonads& som);
	void addMonad(monad_m m);
	void addToParents(std::map<id_d_t, NEGRANonTerminal*>& nonterminals);
	id_d_t addID_D(id_d_t currentID_D, std::map<id_d_t, id_d_t>& nonterm_id_to_id_d_dict);
	void dump();
	void emitMQL(std::ostream *pOut);
};


class NEGRATerminal  : public NEGRANode {
public:
	std::string m_surface, m_postag;
	monad_m m_monad;
	NEGRATerminal(const std::string& surface,
		      const std::string& postag,
		      const std::string& morphtag,
		      const std::string& edge,
		      id_d_t parent,
		      const StringID_DPairList&  secedges,
		      monad_m monad);

	monad_m getMonad(void) const { return m_monad; };

	id_d_t getID_D(void) const { return m_id_d; };
	id_d_t addID_D(id_d_t currentID_D);
	void addToParents(std::map<id_d_t, NEGRANonTerminal*>& nonterminals);
	void dump() const;
	void emitMQL(std::ostream *pOut);
};

class NEGRASentence {
protected:
	std::map<id_d_t, id_d_t> m_nonterm_id_to_id_d_dict;
	id_d_t m_sentence_id;
	std::string m_editor_id;
	std::string m_date;
	std::string m_origin_id;
	monad_m m_starting_monad;
	std::vector<NEGRATerminal*> m_terminals;
	std::map<id_d_t, NEGRANonTerminal*> m_nonterminals;
	std::map<id_d_t, id_d_t> m_nonterm_id_to_id_d_map;
	id_d_t m_id_d;
public:
	NEGRASentence(id_d_t sentence_id, 
			   const std::string& editor_id, 
			   const std::string& date, 
			   const std::string& origin_id, 
			   monad_m starting_monad);
	void addTerminal(NEGRATerminal *pTerminal);
	void addNonTerminal(NEGRANonTerminal *pNonTerminal);
	id_d_t finalize(id_d_t currentID_D);
	void dump();
	unsigned int emitMQLWords(std::ostream* pOut);
	unsigned int emitMQLPhrases(std::ostream* pOut);
	unsigned int emitMQLSelf(std::ostream* pOut);
	monad_m getNoOfMonads() const;
};

typedef std::list<NEGRASentence*> PSentenceList;

class NEGRAImporter : public EmdrosImporterBase {
 protected:
	PSentenceList m_sentences;
	int m_state;
	int m_count;
	NEGRASentence *m_pCurrent_sentence;
public:
	NEGRAImporter(monad_m starting_monad, id_d_t starting_id_d);
	virtual ~NEGRAImporter();
	virtual bool readDocumentFromStream(std::istream *pIn);
	virtual void putSchema(std::ostream *pOut);
	virtual void putCorpusAsMQL(std::ostream *pOut);
	void dump(); // Just pretty-prints it. Not necessary for import.
 protected:
	int parseHashLine(const std::string& line);
	void parseSentenceLine(const std::string& line);
	void parseWordLine(const StringVector& items);
	void parsePhraseLine(const StringVector& items);
	void parseBOS(std::string& line);
	StringID_DPairList parseSecEdges(const StringVector& secedges, StringVector::size_type starting_index);
	void parseEOS(const std::string& line);
	void emitMQLWords(std::ostream *pOut);
	void emitMQLPhrases(std::ostream *pOut);
	void emitMQLSentences(std::ostream *pOut);
	void cleanSentences();
};


#endif
