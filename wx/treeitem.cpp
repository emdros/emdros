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

/*
 * Copyright (C) 2001-2018     Ulrik Sandborg-Petersen
 * Copyright (C) 2018-present  Sandborg-Petersen Holding ApS, Denmark
 *
 * Licensed under the MIT License.
 *
 * Please see the file COPYING in the root of the sources for more details.
 *
 */


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


