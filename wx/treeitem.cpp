//
//
// TreeItem.cpp 
// implementation for CTreeItem
// Ulrik Petersen 
//
// Morphed from some code originally by John Thomson.
//
// Copyright assigned to Ulrik Petersen by John Thomson's superior,
// Steve Echerd, in a letter dated November 11, 2003.  John Thomson's
// informal consent was given in an email from John Thomson to Ulrik
// Petersen dated August 13, 2004.
//
// Created: 3/29-2000
// Last update: 10/12-2008
//
//
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2003-2008  Ulrik Sandborg-Petersen
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


#include <treeitem.h>
#include <viewmetrics.h>


#include <iostream>

// #define SEPARATOR_X (250)

CTreeItem::CTreeItem(LayoutBoxBase* pLayoutBox, id_d_t oid, 
		     int indent, TreeState ts, bool highlight, 
		     bool box_highlight, LayoutParagraph* pLayoutParagraph,
		     bool bOwnsBox)
{
	this->oid = oid;
	indent_count = indent;
	state = ts;
	highlight_state = highlight;
	m_box_highlight_state = box_highlight;
	pBox = pLayoutBox;
	pParagraph = pLayoutParagraph;
	m_pParagraphSurrogate = 0;
	m_bOwnsBox = bOwnsBox;
	m_bOwnsParagraphSurrogate = false;
}

CTreeItem::CTreeItem(LayoutBoxBase* pLayoutBox, id_d_t oid, 
		     int indent, TreeState ts, bool highlight, 
		     bool box_highlight, LayoutBoxBase* pParagraphSurrogate,
		     bool bOwnsBox, bool bOwnsParagraphSurrogate)
{
	this->oid = oid;
	indent_count = indent;
	state = ts;
	highlight_state = highlight;
	m_box_highlight_state = box_highlight;
	pBox = pLayoutBox;
	pParagraph = 0;
	m_pParagraphSurrogate = pParagraphSurrogate;
	m_bOwnsBox = bOwnsBox;
	m_bOwnsParagraphSurrogate = bOwnsParagraphSurrogate;
}

CTreeItem::CTreeItem(LayoutBoxBase *pLayoutBox,
		     id_d_t oid, 
                     int indent, 
                     TreeState ts, 
                     bool highlight, 
                     bool box_highlight,
		     bool bOwnsBox)
{
	pBox = pLayoutBox;
	pParagraph = 0;
	m_pParagraphSurrogate = 0;
	this->oid = oid;
	indent_count = indent;
	state = ts;
	highlight_state = highlight;
	m_box_highlight_state = box_highlight;                      
	m_bOwnsBox = bOwnsBox;
	m_bOwnsParagraphSurrogate = false;
}

CTreeItem::CTreeItem(LayoutBoxBase *pLayoutBox,
		     LayoutParagraph *pLayoutParagraph,
		     id_d_t oid, 
                     int indent, 
                     TreeState ts, 
                     bool highlight, 
                     bool box_highlight,
		     bool bOwnsBox)
{
	pBox = pLayoutBox;
	pParagraph = pLayoutParagraph;
	m_pParagraphSurrogate = 0;
	this->oid = oid;
	indent_count = indent;
	state = ts;
	highlight_state = highlight;
	m_box_highlight_state = box_highlight;                      
	m_bOwnsBox = bOwnsBox;
	m_bOwnsParagraphSurrogate = false;
}

CTreeItem::~CTreeItem()
{
	if (m_bOwnsBox) {
		delete pBox;
	}
	delete pParagraph;
	if (m_bOwnsParagraphSurrogate) {
		delete m_pParagraphSurrogate;
	}

}

bool CTreeItem::highlighted() const
{
	return highlight_state;
}

void CTreeItem::highlighted(bool hl)
{
	highlight_state = hl;
}


bool CTreeItem::box_highlighted(void) const
{
	return m_box_highlight_state;
}

void CTreeItem::box_highlighted(bool bhl)
{
	m_box_highlight_state = bhl;
}


int CTreeItem::height(int first_line, int last_line) const
{
	int boxHeight = pBox->GetHeightLP();
	if (pParagraph) {
		int paraHeight = pParagraph->GetHeightLP(first_line, last_line);
		return std::max(paraHeight, boxHeight);
	} else if (m_pParagraphSurrogate) {
		int paraSurrogateHeight = m_pParagraphSurrogate->GetHeightLP();
		return std::max(paraSurrogateHeight, boxHeight);
	} else {
		return boxHeight;
	}
}

int CTreeItem::box_height() const
{
	return pBox->GetHeightLP();
}

unsigned int CTreeItem::indent() const
{
	return indent_count;
}

id_d_t CTreeItem::get_id_d_t(void) const
{
	return oid;
}

void CTreeItem::indent(int ind)
{
	indent_count = ind;
}

TreeState CTreeItem::expansionState() const
{
	return state;
}

void CTreeItem::expansionState(TreeState ts)
{
	state = ts;
}

void CTreeItem::select(void)
{
	highlighted(true);
}

void CTreeItem::deselect(void)
{
	highlighted(false);
}

void CTreeItem::Draw(wxDC* pDC, ViewMetrics *pMetrics, int x, int yBox, int yParagraph, int first_line, int last_line)
{
	// Draw box if drawing the first line as well
	if (first_line == 0) {
		pBox->SetSelected(highlighted());
		pBox->Draw(pDC, x, yBox);
	}

	// Draw paragraph
	if (pParagraph) {
		int xParagraphLP = x + pBox->GetWidthLP() + pMetrics->Convert10thofMMtoLP(40);
		pParagraph->Draw(pDC, 
				 xParagraphLP, yParagraph, 
				 first_line, last_line, 
				 pMetrics->bIsRightToLeft); // bFlushLeftIfOneLine
	} else if (m_pParagraphSurrogate) {
		int xParagraphLP = x + pBox->GetWidthLP() + pMetrics->Convert10thofMMtoLP(40);
		m_pParagraphSurrogate->Draw(pDC, 
					    xParagraphLP, yParagraph);
		
	}
}

wxSize CTreeItem::size_of_box()
{
	wxSize size1(pBox->GetWidthLP(), pBox->GetHeightLP());
	return size1;
}

// Returns the index of the last line in the paragraph,
// or 0 if no paragraph present.
int CTreeItem::get_last_line_in_paragraph(void) const
{
	if (pParagraph) {
		return pParagraph->getNoOfLines() - 1;
	} else {
		return 0;
	}
}

int CTreeItem::GetHeightLP(int first_line, int last_line)
{
	if (pParagraph) {
		return pParagraph->GetHeightLP(first_line, last_line);
	} else if (m_pParagraphSurrogate) {
		return m_pParagraphSurrogate->GetHeightLP();
	} else {
		return 0;
	}
}

int CTreeItem::getNoOfLines(void) const
{
	if (pParagraph) {
		return pParagraph->getNoOfLines();
	} else if (m_pParagraphSurrogate) {
		return 1;
	} else {
		return 0;
	}
}
/*
  void CTreeItem::get_first_bcv(const CSyntaxMdFDB& mdfdb, CBcv& bcv) const
  {
  CWord* pWord;
  CSyntax_object* pObj = mdfdb.get_syntax_object_by_id_d(oid);
  pWord = mdfdb.get_word_at(pObj->first());

  bcv = pWord->get_bcv();
  }

  void CTreeItem::get_last_bcv(const CSyntaxMdFDB& mdfdb, CBcv& bcv) const
  {
  CWord* pWord;
  CSyntax_object* pObj = mdfdb.get_syntax_object_by_id_d(oid);
  pWord = mdfdb.get_word_at(pObj->last());
  bcv = pWord->get_bcv();
  }
*/


