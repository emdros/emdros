//
// TreeItem.h -- tree items
//
// Morphed from some code originally by John Thomson.
//
// Copyright assigned to Ulrik Petersen by John Thomson's superior,
// Steve Echerd, in a letter dated November 11, 2003.  John Thomson's
// informal consent was given in an email from John Thomson to Ulrik
// Petersen dated August 13, 2004.
//
// Last update: 5/31-2017
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



#ifndef __TREE_ITEM_H__
#define __TREE_ITEM_H__

#include <algorithm>
#include <vector>
#include <string>

#include "emdf.h"
#include "layout.h"


class CTreeItem;  // forward declaration

typedef std::vector<CTreeItem*> TreeItemVec;


enum TreeState{ kFixed = 0, kCollapsed = 1, kExpanded = 2};




class CTreeItem {
 public:
	//CTreeItem(LayoutBox* pLayoutBox, id_d_t oid, int indent, TreeState ts = kCollapsed, bool highlight = false, bool box_highlight = false, LayoutSimpleLine* pLayoutLine = 0);
	CTreeItem(LayoutBoxBase* pLayoutBox, id_d_t oid, int indent, TreeState ts, bool highlight, bool box_highlight, LayoutParagraph* pLayoutParagraph, bool bOwnsBox);
	CTreeItem(LayoutBoxBase* pLayoutBox, id_d_t oid, int indent, TreeState ts, bool highlight, bool box_highlight, LayoutBoxBase* pParagraphSurrogate, bool bOwnsBox, bool bOwnsParagraphSurrogate);
	CTreeItem(LayoutBoxBase *pLayoutBox, id_d_t oid, int indent, TreeState ts, bool highlight, bool box_highlight, bool bOwnsBox);
	CTreeItem(LayoutBoxBase *pLayoutBox, LayoutParagraph *pLayoutParagraph, id_d_t oid, int indent, TreeState ts, bool highlight, bool box_highlight, bool bOwnsBox);
	~CTreeItem();
	int height(int first_line = 0, int last_line = 0x7FFFFFFF) const;
	int box_height() const;
	int GetHeightLP(int first_line, int last_line);
	void select(void);
	void deselect(void);
	bool highlighted(void) const;
	bool box_highlighted(void) const;
	void box_highlighted(bool bhl);
	int get_last_line_in_paragraph(void) const;
	unsigned int indent(void) const;
	TreeState expansionState() const;
	void expansionState(TreeState ts);
	id_d_t get_id_d_t(void) const;
	int getNoOfLines(void) const;
	void Draw(wxDC* pDC, ViewMetrics *pMetrics, int x, int yBox, int yParagraph, int first_line = 0, int last_line = 0x7FFFFFF);
	wxSize size_of_box();
	//void get_first_bcv(const CSyntaxMdFDB& mdfdb, CBcv& bcv) const;
	//void get_last_bcv(const CSyntaxMdFDB& mdfdb, CBcv& bcv) const;

 protected:
	void highlighted(bool hl);
	void indent(int ind);
 private:
	bool m_bOwnsBox;
	bool m_bOwnsParagraphSurrogate;
	int indent_count;
	bool highlight_state;
	bool m_box_highlight_state;
	TreeState state;
	id_d_t oid;
	LayoutBoxBase* pBox;
	LayoutParagraph *pParagraph;
	LayoutBoxBase *m_pParagraphSurrogate;
	
};







#endif // __TREE_ITEM_H__

