/*
 * ctdoc.cpp
 *
 * Document in ctwx.
 * ctwx is a graphical front-end for chunking Emdros databases.
 *
 * Ulrik Petersen
 * Created: 12/14-2005
 * Last update: 1/26-2006
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2005-2006  Ulrik Petersen
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
#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "ctdoc.h"
#endif

#include "ctdoc.h"
#include <iostream>


//////////////////////////////////////////////////////////////
//
// EmdrosObject
//
//////////////////////////////////////////////////////////////


void EmdrosObject::pretty() const
{
	std::cerr << "EmdrosObject " << m_som << "{\n";
	for (feature_vec_t::size_type i = 0;
	     i < m_features.size();
	     ++i) {
		std::cerr << "   " << m_features[i] << '\n';
	}
	std::cerr << "}\n\n";
}


//////////////////////////////////////////////////////////////
//
// CTChunk
//
//////////////////////////////////////////////////////////////


CTChunk::CTChunk(SetOfMonads som, indent_t indent) 
	: m_first(som.first()), m_last(som.last()), m_indent(indent) 
{
}

CTChunk::CTChunk(monad_m first, monad_m last, indent_t indent) 
	: m_first(first), m_last(last), m_indent(indent) 
{
}

CTChunk::~CTChunk() 
{
}

CTChunk* CTChunk::split(monad_m split_before, indent_t indent) 
{ 
	if (m_first < split_before && split_before <= m_last) { 
		CTChunk *pNewChunk = 
			new CTChunk(split_before, m_last, indent); 
		m_last = split_before-1; 
		return pNewChunk; 
	} else { 
		return 0; 
	} 
}

void CTChunk::pretty() const
{
	std::cerr << "CTChunk {" << m_first << "-" << m_last << "} { indent = " << m_indent << "; }\n";
}




//////////////////////////////////////////////////////////////
//
// CTDoc
//
//////////////////////////////////////////////////////////////

CTDoc::CTDoc()
	: m_objectMonadHintIterator(m_objects.end()),
	  m_objects_end(m_objects.end())
{
}


CTDoc::~CTDoc()
{
	clean();
}


CTChunk *CTDoc::getChunkFromMonad(monad_m m)
{
	chunkmap_t::iterator it = m_chunks.upper_bound(m);
	if (it == m_chunks.end()) {
		if (m_chunks.rbegin()->first <= m) {
			return m_chunks.rbegin()->second;
		} else {
			return 0;
		}
	} else {
		--it;
		return it->second;
	}
}

const EmdrosObject *CTDoc::getObjectFromMonad(monad_m m)
{
	if (m_objectMonadHintIterator == m_objects_end) {
		objectmap_t::const_iterator it = m_objects.lower_bound(m);
		m_objectMonadHintIterator = it;
		++m_objectMonadHintIterator;
		if (it == m_objects_end) {
			return 0;
		} else {
			return it->second;
		}
	} else {
		if (m_objectMonadHintIterator->first == m) {
			const EmdrosObject *pObj = m_objectMonadHintIterator->second;
			++m_objectMonadHintIterator;
			return pObj;
		} else {
			objectmap_t::const_iterator it = m_objects.lower_bound(m);
			m_objectMonadHintIterator = it;
			++m_objectMonadHintIterator;
			if (it == m_objects_end) {
				return 0;
			} else {
				return it->second;
			}
		}
	}
}

void CTDoc::addObjects(objectlist_t objects)
{
	for (objectlist_t::iterator it = objects.begin();
	     it != objects.end();
	     ++it) {
		m_objects[(*it)->first()] = *it;
	}
	m_objectMonadHintIterator = m_objects_end = m_objects.end();
}

void CTDoc::addChunks(chunklist_t chunks)
{
	for (chunklist_t::iterator it = chunks.begin();
	     it != chunks.end();
	     ++it) {
		m_chunks[(*it)->first()] = *it;
	}
}

void CTDoc::addChunk(CTChunk *pChunk)
{
	m_chunks[pChunk->first()] = pChunk;
}

void CTDoc::removeChunk(CTChunk *pChunk)
{
	chunkmap_t::iterator it = m_chunks.find(pChunk->first());
	if (it->second != pChunk) {
		// FIXME: Throw an exception!
		return;
	} else {
		m_chunks.erase(it);
	}
}


void CTDoc::clean()
{
	for (objectmap_t::iterator it = m_objects.begin();
	     it != m_objects.end();
	     ++it) {
		delete it->second;
	}

	for (chunkmap_t::iterator it = m_chunks.begin();
	     it != m_chunks.end();
	     ++it) {
		delete it->second;
	}
}


void CTDoc::pretty() const
{
	for (objectmap_t::const_iterator it = m_objects.begin();
	     it != m_objects.end();
	     ++it) {
		it->second->pretty();
	}

	for (chunkmap_t::const_iterator it = m_chunks.begin();
	     it != m_chunks.end();
	     ++it) {
		it->second->pretty();
	}
}

