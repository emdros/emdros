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
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2003-2017  Ulrik Sandborg-Petersen
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

