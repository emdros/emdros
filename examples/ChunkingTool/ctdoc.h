/*
 * ctdoc.h
 *
 * Document in ctwx.
 * ctwx is a graphical front-end for chunking Emdros databases.
 *
 * Ulrik Petersen
 * Created: 12/14-2005
 * Last update: 1/26-2006
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


#ifndef _CWDOC_H_
#define _CWDOC_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "ctdoc.cpp"
#endif

#include <monads.h>
#include <map>
#include <string>
#include <vector>
#include <list>

#include <layout.h>

typedef wxstringvec_t feature_vec_t;

class EmdrosObject {
protected:
	feature_vec_t m_features;
	SetOfMonads m_som;
public:
	EmdrosObject(const SetOfMonads& som, const feature_vec_t& features)
		: m_features(features), m_som(som) {};
	~EmdrosObject() {};
	const wxString& getFeature(feature_vec_t::size_type index) const { return m_features[index]; };
	const SetOfMonads& getSOM() const { return m_som; };
	feature_vec_t::size_type getNoOfFeatures(void) const { return m_features.size(); };
	monad_m first() const { return m_som.first(); };
	monad_m last() const { return m_som.last(); };
	void pretty() const;
};

typedef std::list<EmdrosObject*> objectlist_t;

typedef unsigned int indent_t;


class CTChunk {
protected:
	monad_m m_first, m_last;
	indent_t m_indent;
	CTChunk() : m_first(1), m_last(0), m_indent(0) {};
public:
	CTChunk(SetOfMonads som, indent_t indent);
	CTChunk(monad_m first, monad_m last, indent_t indent);
	~CTChunk();
	SetOfMonads getSOM(void) const { return SetOfMonads(m_first, m_last); };
	indent_t getIndent(void) const { return m_indent; };
	void increaseIndent(void) { ++m_indent; };
	void decreaseIndent(void) { if (m_indent != 0) --m_indent; };
	CTChunk* split(monad_m split_before, indent_t indent);
	CTChunk* combine(monad_m split_before, indent_t indent);
	monad_m first() const { return m_first; };
	monad_m last() const { return m_last; };
	void pretty() const;
};

typedef std::list<CTChunk*> chunklist_t;

typedef std::map<monad_m, CTChunk*> chunkmap_t;
typedef std::map<monad_m, EmdrosObject*> objectmap_t;


class CTDoc {
protected:
	chunkmap_t m_chunks;
	objectmap_t m_objects;
	objectmap_t::const_iterator m_objectMonadHintIterator, m_objects_end;
public:
	CTDoc();
	~CTDoc();
	CTChunk *getChunkFromMonad(monad_m m);
	const EmdrosObject *getObjectFromMonad(monad_m m);
	void addObjects(objectlist_t objects);
	void addChunks(chunklist_t chunks);
	void addChunk(CTChunk *pChunk);
	void removeChunk(CTChunk *pChunk);
	void clean();
	void pretty() const;
	const chunkmap_t& getChunks(void) const { return m_chunks; };
	const objectmap_t& getObjects(void) const { return m_objects; };
};


#endif
// _CWDOC_H_
