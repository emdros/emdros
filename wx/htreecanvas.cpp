//
// Horizontal Tree Canvas : implementation file
//
// Created: 9/8-2007
// Last update: 10/4-2018
//
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2007-2018  Ulrik Sandborg-Petersen
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
#pragma implementation "htreecanvas.h"
#endif

#include <wx/wx.h>
#include <wx/pen.h>
#include <wx/dataobj.h>
#include <wx/clipbrd.h>
#include <treeitem.h>

#include <iostream>


#include <viewmetrics.h>
#include <htreecanvas.h>
#include <debug.h> // For ASSERT_THROW
#include <md5_emdros.h>

#include <emdfdb.h>
#include <emdf_value.h>
#include <mql_sheaf.h>
#include <string_func.h>


#define TRACE(S) /* Nothing. */


////////////////////////////////////////////////////////////////////////////
//
// HTreeNode
//
////////////////////////////////////////////////////////////////////////////

HTreeNode::HTreeNode(id_d_t self_id_d,
		     id_d_t parent_id_d,
		     const SetOfMonads& som,
		     LayoutBoxBase *pBranchLabels,
		     bool bOwnsBranchLabels,
		     LayoutBoxBase *pMainBox,
		     bool bOwnsMainBox,
		     bool bBranchLabelsBelowMainBox)
	: m_x(0), m_y(0),
	  m_width(0), m_height(0),
	  m_main_box_width(0),
	  m_least_vertical_riser_width(0),
	  m_least_vertical_riser_height(0),
	  m_leftmost_x(0),
	  m_rightmost_x(0),
	  m_vertical_separation_distance_between_main_box_and_branch_box(0),
	  m_self_id_d(self_id_d),
	  m_parent_id_d(parent_id_d),
	  m_som(som),
	  m_pBranchLabels(pBranchLabels),
	  m_bOwnsBranchLabels(bOwnsBranchLabels),
	  m_bBranchLabelsBelowMainBox(bBranchLabelsBelowMainBox),
	  m_pMainBox(pMainBox),
	  m_bOwnsMainBox(bOwnsMainBox)
{
}


HTreeNode::~HTreeNode()
{
	if (m_bOwnsBranchLabels) {
		delete m_pBranchLabels;
	}
	if (m_bOwnsMainBox) {
		delete m_pMainBox;
	}
}

int HTreeNode::calculateMainHeight(void)
{
	m_height = m_pMainBox->GetHeightLP();
	return m_height;
}



std::string HTreeNode::getCharacteristicString(void) const
{
	std::string result = "(" + long2string(m_x) + "," + long2string(m_y) 
		+ "|" + id_d2string(m_self_id_d) 
		+ ">" + id_d2string(m_parent_id_d) + "m" + m_som.toString()
		+ "b";

	if (m_pBranchLabels != 0) {
		std::ostringstream ostr;
		m_pBranchLabels->toHTML(&ostr, false, "");
		result += ostr.str();
	}

	result += "i";

	if (m_pMainBox != 0) {
		std::ostringstream ostr;
		m_pMainBox->toHTML(&ostr, false, "");
		result += ostr.str();
	}

	result += ")";

	return result;
}

int HTreeNode::getBranchLabelsTotalHeight(void) const
{
	int nTotalHeight = 0;
	if (m_pBranchLabels != 0) {
		nTotalHeight += m_pBranchLabels->GetHeightLP();
		if (m_bBranchLabelsBelowMainBox) {
			nTotalHeight += m_vertical_separation_distance_between_main_box_and_branch_box;
		}
	}
	return nTotalHeight;
}

int HTreeNode::getVerticalRiserX(void) const
{
	return m_x + m_main_box_width/2;
}


void HTreeNode::calculateLeastVerticalRiserDimensions(ViewMetrics *pViewMetrics)
{
	int nWidthLP;
	int nHeightLP;
	if (m_pBranchLabels != 0) {
		nWidthLP = m_pBranchLabels->GetWidthLP();
		if (m_pMainBox != 0) {
			int nWidthMainBox = m_pMainBox->GetWidthLP();
			if (nWidthMainBox > nWidthLP) {
				nWidthLP = nWidthMainBox;
			}
		}
		m_vertical_separation_distance_between_main_box_and_branch_box = pViewMetrics->getLatinFontHeight() / 2;
		if (m_bBranchLabelsBelowMainBox) {
			nHeightLP = pViewMetrics->getLatinFontHeight() / 2;
		} else {
			nHeightLP = (m_pBranchLabels->GetHeightLP()*2);
		}
	} else {
		if (m_pMainBox != 0) {
			nWidthLP  = m_pMainBox->GetWidthLP();
			nHeightLP = (pViewMetrics->getLatinFontHeight() * 3) / 2;
		} else {
			nWidthLP = pViewMetrics->getLatinFontHeight();
			nHeightLP = pViewMetrics->getLatinFontHeight() / 2;
		}
	}

	m_least_vertical_riser_width = nWidthLP;
	m_least_vertical_riser_height = nHeightLP;
}


void HTreeNode::calculateDimensions(ViewMetrics *pViewMetrics)
{
	m_main_box_width = m_pMainBox->GetWidthLP();
	calculateLeastVerticalRiserDimensions(pViewMetrics);
	calculateMainHeight();
	int width = calculateMainWidth();
	m_width = (width > m_least_vertical_riser_width) ? width : m_least_vertical_riser_width;
}

void HTreeNode::setXRightToLeft(int total_canvas_width)
{
	m_x = total_canvas_width - m_x - m_width;

	if (m_leftmost_x == m_rightmost_x) {
		// Do nothing
	} else {
		int new_rightmost_x = total_canvas_width - m_leftmost_x;
		int new_leftmost_x = total_canvas_width - m_rightmost_x;
		setLeftmostX(new_leftmost_x);
		setRightmostX(new_rightmost_x);
	}
}


void HTreeNode::Draw(wxDC *pDC, int y, int yParentMiddle, int yParentBottom, int xParentLeft, int xParentRight, int xOffset, int yOffset, const wxPen& pen)
{
	if (getParentID_D() != NIL) {
		int xMiddle = getVerticalRiserX();
		int yParent;
		if (xMiddle >= xParentLeft
		    && xMiddle <= xParentRight) {
			yParent = yParentBottom;
		} else {
			yParent = yParentMiddle;
		}
		if (m_pBranchLabels != 0) {
			int yBranchLabels;
			int xBranchLabels = xMiddle - m_pBranchLabels->GetWidthLP()/2;
			if (m_bBranchLabelsBelowMainBox) {
				if (m_pMainBox != 0) {
					yBranchLabels = (y + m_pMainBox->GetHeightLP() + m_vertical_separation_distance_between_main_box_and_branch_box);

					int ySeparator = yBranchLabels - (m_vertical_separation_distance_between_main_box_and_branch_box / 2);
					
					pDC->SetPen(pen);
					pDC->DrawLine(xBranchLabels + xOffset,
						      ySeparator + yOffset,
						      xBranchLabels + m_pBranchLabels->GetWidthLP() + xOffset,
						      ySeparator + yOffset);
					pDC->DrawLine(xMiddle
						      + xOffset, 
						      yParent + yOffset, 
						      xMiddle + xOffset, 
						      y + yOffset);
					pDC->SetPen(wxNullPen);
				} else {
					yBranchLabels = y;
				}
			} else {
				yBranchLabels = (y - (m_pBranchLabels->GetHeightLP()*3)/2);

				
				pDC->SetPen(pen);
				pDC->DrawLine(xMiddle + xOffset,
					      y + yOffset, 
					      xMiddle + xOffset,
					      yBranchLabels 
					      + m_pBranchLabels->GetHeightLP()
					      + yOffset);
				pDC->DrawLine(xMiddle + xOffset, 
					      yParent + yOffset, 
					      xMiddle + xOffset, 
					      yBranchLabels + yOffset);
				pDC->SetPen(wxNullPen);
			}

			m_pBranchLabels->Draw(pDC, xBranchLabels + xOffset, yBranchLabels + yOffset);
		} else {
			int xMiddle = getVerticalRiserX();

			pDC->SetPen(pen);
			pDC->DrawLine(xMiddle + xOffset,
				      y + yOffset,
				      xMiddle + xOffset,
				      yParent + yOffset - 1);
			pDC->SetPen(wxNullPen);
		}
	}
}

void HTreeNode::RecalculateExtents(wxDC *pDC, ViewMetrics *pViewMetrics)
{
	if (m_pBranchLabels != 0) {
		m_pBranchLabels->RecalculateExtents(pDC);
	}

	if (m_pMainBox != 0) {
		m_pMainBox->RecalculateExtents(pDC);
	}

	calculateDimensions(pViewMetrics);
}


////////////////////////////////////////////////////////////////////////////
//
// HTerminalNode
//
////////////////////////////////////////////////////////////////////////////

HTerminalNode::HTerminalNode(id_d_t self_id_d,
			     id_d_t parent_id_d,
			     const SetOfMonads& som,
			     LayoutBoxBase *pBranchLabels,
			     bool bOwnsBranchLabels,
			     LayoutBoxBase *pTerminalBox,
			     bool bOwnsTerminalBox,
			     bool bBranchLabelsBelowMainBox)
	: HTreeNode(self_id_d,
		    parent_id_d,
		    som,
		    pBranchLabels,
		    bOwnsBranchLabels,
		    pTerminalBox,
		    bOwnsTerminalBox,
		    bBranchLabelsBelowMainBox)
{
}


HTerminalNode::~HTerminalNode()
{
}


	
int HTerminalNode::calculateMainWidth(void)
{
	int mainbox_width = m_pMainBox->GetWidthLP();
	int branchlabels_width = 0;
	if (m_pBranchLabels != 0) {
		branchlabels_width = m_pBranchLabels->GetWidthLP();
	}
	m_width = (mainbox_width > branchlabels_width) ? mainbox_width : branchlabels_width;
	return m_width;
}

void HTerminalNode::Draw(wxDC *pDC, int y, int yParentMiddle, int yParentBottom, int xParentLeft, int xParentRight, int xOffset, int yOffset, const wxPen& pen)
{
	HTreeNode::Draw(pDC, y, yParentMiddle, yParentBottom, xParentLeft, xParentRight, xOffset, yOffset, pen);
	m_pMainBox->Draw(pDC, 
			 m_x + xOffset, 
			 y + yOffset);
}





////////////////////////////////////////////////////////////////////////////
//
// HNonTerminalNode
//
////////////////////////////////////////////////////////////////////////////

HNonTerminalNode::HNonTerminalNode(id_d_t self_id_d,
				   id_d_t parent_id_d,
				   const SetOfMonads& som,
				   LayoutBoxBase *pBranchLabels,
				   bool bOwnsBranchLabels,
				   LayoutBoxBase *pCategoryBox,
				   bool bOwnsCategoryBox,
				   bool bBranchLabelsBelowMainBox)
	: HTreeNode(self_id_d,
		    parent_id_d,
		    som,
		    pBranchLabels,
		    bOwnsBranchLabels,
		    pCategoryBox,
		    bOwnsCategoryBox,
		    bBranchLabelsBelowMainBox)
{
}


HNonTerminalNode::~HNonTerminalNode()
{
}


	
int HNonTerminalNode::calculateMainWidth(void)
{
	int mainbox_width = m_pMainBox->GetWidthLP();
	int branchlabels_width = 0;
	if (m_pBranchLabels != 0) {
		branchlabels_width = m_pBranchLabels->GetWidthLP();
	}
	m_width = (mainbox_width > branchlabels_width) ? mainbox_width : branchlabels_width;
	return m_width;
}

void HNonTerminalNode::Draw(wxDC *pDC, int y, int yParentMiddle, int yParentBottom, int xParentLeft, int xParentRight, int xOffset, int yOffset, const wxPen& pen)
{
	HTreeNode::Draw(pDC, y, yParentMiddle, yParentBottom, xParentLeft, xParentRight, xOffset, yOffset, pen);

	if (m_leftmost_x == m_rightmost_x) {
		; // Do nothing
	} else {
		int yMiddle = y + m_pMainBox->GetHeightLP()/2;

		pDC->SetPen(pen);
		pDC->DrawLine(m_x + xOffset,
			      yMiddle + yOffset, 
			      m_leftmost_x + xOffset - 1,
			      yMiddle + yOffset);
		pDC->DrawLine(m_x + m_pMainBox->GetWidthLP() + xOffset,
			      yMiddle + yOffset,
			      m_rightmost_x + xOffset,
			      yMiddle + yOffset);
		pDC->SetPen(wxNullPen);
	}

	m_pMainBox->Draw(pDC, 
			 m_x + xOffset,
			 y + yOffset);
}



////////////////////////////////////////////////////////////////////////////
//
// HTreeBox
//
////////////////////////////////////////////////////////////////////////////

HTreeBox::HTreeBox(ViewMetrics *pMetrics, bool bSpaceAfter, bool bIsRightToLeft)
	: LayoutBoxBase(pMetrics, bSpaceAfter, false, bIsRightToLeft)
{
	
}


HTreeBox::~HTreeBox()
{
	clearLayout();
}

void HTreeBox::Draw(wxDC *pDC, int xLP, int yLP)
{
	// PrepareDC(pDC);
	// pDC->GetUserScale(&m_xScaleFactor, &m_yScaleFactor);
	if(m_objs.empty()) return;

	// Draw all roots
	const Monad2IDDMMap& roots = this->getRoots();

	int nPenWidth = getPenWidth(m_pMetrics->m_nMagnification);
#if wxCHECK_VERSION(3,0,0)
	wxPen pen(*wxBLACK, nPenWidth, wxPENSTYLE_SOLID);
#else
	wxPen pen(*wxBLACK, nPenWidth, wxSOLID);
#endif
	
	Monad2IDDMMap::const_iterator root_ci = roots.begin();
	while (root_ci != roots.end()) {
		id_d_t root_id_d = root_ci->second;

		this->DrawNode(pDC, root_id_d, 0, 0, 0, 0, xLP, yLP, pen);

		++root_ci;
	}

}


void HTreeBox::DrawToBitmap(wxBitmap **ppBitmap)
{
	// Make compatible in-memory DC
	wxMemoryDC dcMemory;

	int nBorder = 10;

	int nBitmapWidth = this->GetWidthLP() + 2*nBorder;
	int nBitmapHeight = this->GetHeightLP() + 2*nBorder;

	// Calculate wxRect for bitmap
	wxRect rectBitmap;
	rectBitmap.SetX(0);
	rectBitmap.SetY(0);
	rectBitmap.SetWidth(nBitmapWidth);
	rectBitmap.SetHeight(nBitmapHeight);

	*ppBitmap = new wxBitmap(nBitmapWidth, nBitmapHeight, -1);

	// Select bitmap into memoryDC
	dcMemory.SelectObject(*(*ppBitmap));

	// Draw white background
	dcMemory.SetBrush(*wxWHITE_BRUSH);
	dcMemory.SetPen(*wxWHITE_PEN);
	dcMemory.DrawRectangle(rectBitmap);

	// Draw everything	
	dcMemory.SetMapMode(wxMM_TEXT);
	this->Draw(&dcMemory, nBorder, nBorder);

	// Deselect bitmap
	dcMemory.SelectObject(wxNullBitmap);
}


void HTreeBox::RecalculateExtents(wxDC *pDC)
{
	Monad2IDDMap::const_iterator terminal_ci = m_terminals.begin();
	while (terminal_ci != m_terminals.end()) {
		id_d_t id_d = terminal_ci->second;
		
		HTreeNode *pNode = getNode(id_d);
		
		pNode->RecalculateExtents(pDC, m_pMetrics);
		
		++terminal_ci;
	}
	
	IDD2TreeNodeMap::iterator objs_it = m_objs.begin();
	while (objs_it != m_objs.end()) {
		HTreeNode *pNode = objs_it->second;
		pNode->RecalculateExtents(pDC, m_pMetrics);
		++objs_it;
	}

	// This will recalculate m_xExtentLP and m_yExtentLP
	layout();
}

	
/////////////////////////////////////////////////////////////////////////////
// HTreeBox layout

void HTreeBox::addTerminal(HTerminalNode *pNode)
{
	this->addObject(pNode);
	monad_m fm = pNode->first();
	id_d_t id_d = pNode->getID_D();
	m_terminals.insert(std::make_pair(fm, id_d));
}

void HTreeBox::addNonTerminal(HNonTerminalNode *pNode)
{
	this->addObject(pNode);
}

void HTreeBox::addObject(HTreeNode *pNode)
{
	id_d_t id_d = pNode->getID_D();
	id_d_t parent = pNode->getParentID_D();
	monad_m fm = pNode->first();

	if (m_children.find(parent) == m_children.end()) {
		m_children.insert(std::make_pair(parent, Monad2IDDMMap()));
	}

	// This strange incantation of std::pair is necessary on Solaris,
	// as of 2009-01-22
	m_children[parent].insert(std::pair<const monad_m, id_d_t>(fm, id_d));

	m_objs.insert(std::make_pair(id_d, pNode));

	pNode->calculateDimensions(m_pMetrics);
}



HTreeNode *HTreeBox::getNode(id_d_t id_d)
{
	IDD2TreeNodeMap::iterator ci = m_objs.find(id_d);
	if (ci != m_objs.end()) {
		return ci->second;
	} else {
		return 0;
	}
}

int HTreeBox::getInterWordSpace() const
{
	return m_pMetrics->getLatinFontHeight()/2;
}


std::string HTreeBox::getCharacteristicString()
{
	std::string result = long2string(m_pMetrics->m_nMagnification) + "+";

	IDD2TreeNodeMap::const_iterator ci = m_objs.begin();
	while (ci != m_objs.end()) {
		HTreeNode* pNode = ci->second;

		result += pNode->getCharacteristicString();
		
		++ci;
	}

	return result;
}

void HTreeBox::toHTML(std::ostream *pOut, bool bIsRightToLeft, const std::string& filename)
{
	(void)(bIsRightToLeft); // Silence a warning
	
	// Get message
	std::string characteristic_string = getCharacteristicString();

	std::string md5digest = emdros_MD5Digest(characteristic_string);

	wxString strHTMLFileName(filename.c_str(), wxConvUTF8);
	wxFileName fn(strHTMLFileName);

	wxString strIMGSRCFileName = fn.GetName() + wxT("-") + wxString(md5digest.c_str(), wxConvUTF8) + wxT(".png");
	wxString strPNGFileName = fn.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) + strIMGSRCFileName;
	
	std::string png_filename = std::string((const char*) strPNGFileName.mb_str(wxConvUTF8));
	std::string imgsrc_filename = std::string((const char*) strIMGSRCFileName.mb_str(wxConvUTF8));
	

	std::cerr << "UP201: png_filename = '" << png_filename << "'" << std::endl;

	// emit <img src="<md5-digest>.png">
	(*pOut) << "<img src=\"" << imgsrc_filename << "\">";

	// emit image as <md5-digest>.png
	wxBitmap *pBitmap;
	this->DrawToBitmap(&pBitmap);

	

	pBitmap->SaveFile(strPNGFileName, wxBITMAP_TYPE_PNG, NULL);
	
	delete pBitmap;
}

/**
 * Save box as a PNG.
 *
 * The filename will be constructed as:
 *
 * base_filename + (optional: "-" + magnification) + "-" + first monad + ".png".
 *
 * @param base_filename The base of the filename, including
 * path. Should not end in .png -- this will be added.
 *
 * @param bUseMagnificationInFilename If true, then "-" + the
 * magnification will be added to the base_filename before the "-" +
 * first_monad.
 *
 * @param filenameused upon return, this will contain the filename used.
 *
 * @first_monad upon return, this will contain the first monad.
 *
 */
void HTreeBox::savePNG(const std::string& base_filename, bool bUseMagnificationInFilename, std::string& /* out */ filenameused, monad_m& first_monad)
{
	// Get message
	std::string filename = base_filename;
	if (bUseMagnificationInFilename) {
		long magnification = m_pMetrics->m_nMagnification;
		filename += "-" + long2string(magnification);
	}

	first_monad = m_terminals.begin()->first;
	filename += "-" + monad_m2string(first_monad);

	filename += ".png";

	wxString strPNGFileName(filename.c_str(), wxConvUTF8);

	// emit image as <md5-digest>.png
	wxBitmap *pBitmap;
	this->DrawToBitmap(&pBitmap);
	
	pBitmap->SaveFile(strPNGFileName, wxBITMAP_TYPE_PNG, NULL);
	
	delete pBitmap;

	filenameused = filename;
}


void HTreeBox::clearLayout()
{
	IDD2TreeNodeMap::iterator objs_it = m_objs.begin();
	while (objs_it != m_objs.end()) {
		HTreeNode *pNode = objs_it->second;
		delete pNode;
		++objs_it;
	}
	m_objs.clear();

	m_terminals.clear();

	m_children.clear();

	m_xExtentLP = 0;
	m_yExtentLP = 0;
}

void HTreeBox::layout()
{
	// Set X for all terminals
	int x = 10; // start 10 pixels into the window

	int max_height = 0;
	Monad2IDDMap::const_iterator terminal_ci = m_terminals.begin();

	if (terminal_ci != m_terminals.end()) {
		HTreeNode *pFirstNode = getNode(terminal_ci->second);

		int firstNodeWidth = pFirstNode->getWidth();

		int firstNodeParentWidth = 0;
		id_d_t firstNodeParent_id_d = pFirstNode->getParentID_D();
		if (firstNodeParent_id_d == NIL) {
			firstNodeParentWidth = 0; // Parent isn't there, so its width is 0
		} else if (m_objs.find(firstNodeParent_id_d) == m_objs.end()) {
			firstNodeParentWidth = 0; // Can't find parent, so we set its width to 0.
		} else {
			firstNodeParentWidth = m_objs.find(firstNodeParent_id_d)->second->getWidth();
		}

		// Start the first one so far into the picture that the 
		// parent can be shown wholly as well.		
		if (firstNodeParentWidth > firstNodeWidth) {
			x += (firstNodeParentWidth - firstNodeWidth) / 2;
		}
	}

	// Now layout all terminals
	while (terminal_ci != m_terminals.end()) {
		id_d_t id_d = terminal_ci->second;
		
		HTreeNode *pNode = getNode(id_d);
		pNode->setX(x);
		int height = pNode->getHeight();
		if (height > max_height) {
			max_height = height;
		}

		int width = pNode->getWidth();

		int parentWidth = 0;
		id_d_t parent_id_d = pNode->getParentID_D();
		if (parent_id_d == NIL) {
			parentWidth = 0; // Parent isn't there, so its width is 0
		} else if (m_objs.find(parent_id_d) == m_objs.end()) {
			parentWidth = 0; // Can't find parent, so we set its width to 0.
		} else {
			parentWidth = m_objs.find(parent_id_d)->second->getWidth();
		}

		if (width < parentWidth) {
			x += parentWidth;
		} else {
			x += width;
		}


		m_xExtentLP = x;

		x += this->getInterWordSpace()*2;

		++terminal_ci;
	}

	// Set Y for all terminals to max_height
	terminal_ci = m_terminals.begin();
	while (terminal_ci != m_terminals.end()) {
		id_d_t id_d = terminal_ci->second;
		
		HTreeNode *pNode = getNode(id_d);
		pNode->setY(max_height);

		++terminal_ci;
	}

	// Make sure that display is at least max_height
	// high, even if there are no nonterminals.
	m_yExtentLP = max_height;

	// Layout all roots
	const Monad2IDDMMap& roots = this->getRoots();
	
	Monad2IDDMMap::const_iterator root_ci = roots.begin();
	while (root_ci != roots.end()) {
		id_d_t root_id_d = root_ci->second;

		this->layoutNode(root_id_d);

		++root_ci;
	}

	// If we are doing right-to-left, then reverse everything.
	if (m_bIsRightToLeft) {
		terminal_ci = m_terminals.begin();
		while (terminal_ci != m_terminals.end()) {

			id_d_t id_d = terminal_ci->second;
		
			HTreeNode *pNode = getNode(id_d);
			pNode->setXRightToLeft(m_xExtentLP);
			
			++terminal_ci;
		}

		root_ci = roots.begin();
		while (root_ci != roots.end()) {
			id_d_t root_id_d = root_ci->second;

			this->setXRightToLeft(root_id_d);

			++root_ci;
		}
	}
}


void HTreeBox::layoutNode(id_d_t id_d)
{
	HTreeNode *pNode = getNode(id_d);
	
	if (pNode->isTerminal()) {
		// Nothing to do. We have already laid it out.
		; 
	} else {
		// Node is a non-terminal


		// Layout children
		const Monad2IDDMMap& children_by_monads = getChildren(id_d);
		
		Monad2IDDMMap::const_iterator child_ci = children_by_monads.begin();
		while (child_ci != children_by_monads.end()) {
			id_d_t child_id_d = child_ci->second;

			this->layoutNode(child_id_d);

			++child_ci;
		}


		// Find X for this node
		id_d_t leftmost_child_id_d= children_by_monads.begin()->second;
		HTreeNode *pLeftmostChild = m_objs[leftmost_child_id_d];
		int leftmost_x = pLeftmostChild->getVerticalRiserX();

		id_d_t rightmost_child_id_d= children_by_monads.rbegin()->second;
		HTreeNode *pRightmostChild = m_objs[rightmost_child_id_d];
		int rightmost_x = pRightmostChild->getVerticalRiserX();
		pNode->setRightmostX(rightmost_x);


		int right_x_to_use_for_node_placement = rightmost_x;

		MonadSetElement first_mse = pNode->getSOM().const_iterator().current();

		monad_m last_monad_to_consider = first_mse.last() + 1;
		if (last_monad_to_consider < pNode->getSOM().last()) {
			// Only do this exercise 
			// if there is more than one MSE in the pNode->getSOM().
			HTreeNode *pLastObject = 0;
			if (m_terminals.find(last_monad_to_consider) != m_terminals.end()) {
				id_d_t terminal_id_d = m_terminals[last_monad_to_consider];
				pLastObject = m_objs[terminal_id_d];
			} else {
				id_d_t terminal_id_d = m_terminals[last_monad_to_consider-1];
				pLastObject = m_objs[terminal_id_d];
			}
			
			int last_object_vertical_riser_x = pLastObject->getVerticalRiserX();
			if (rightmost_x > last_object_vertical_riser_x) {
				right_x_to_use_for_node_placement 
					= last_object_vertical_riser_x;
			}
		}
		
		// FIXME: What do we do if 
		// pNode->getMainBoxWidth() is > 
		// right_x_to_use_for_node_placement - leftmost_x ?
		int x;
		if (right_x_to_use_for_node_placement <= leftmost_x) {
			x = leftmost_x - (pNode->getMainBoxWidth()/2);
		} else {
			x = (right_x_to_use_for_node_placement - leftmost_x - pNode->getMainBoxWidth()) / 2 + leftmost_x;
		}
		
		if (x < 0) {
			x = 0;
		}

		pNode->setX(x);

		pNode->setLeftmostX(leftmost_x);
		
			
		// Set Y to be the maximum of all child_y + child_vertical_riser

		int y = 0;
		child_ci = children_by_monads.begin();
		while (child_ci != children_by_monads.end()) {
			id_d = child_ci->second;
			HTreeNode *pChildNode = m_objs[id_d];
			int child_vertical_riser_height = pChildNode->getLeastVerticalRiserHeight();

			int child_y = pChildNode->getY();
			int candidate_y = child_y + child_vertical_riser_height;

			if (y < candidate_y) {
				y = candidate_y;
			}

			++child_ci;
		}		

		// Now add the object's height, so as to be the upper left
		y += pNode->getHeight();

		// If node's branch labels are below main box, add 
		// total height of branch labels + separator
		// between branch labels and main box.
		if (pNode->areBranchLabelsBelowMainBox()) {
			y += pNode->getBranchLabelsTotalHeight();
		}

		pNode->setY(y);

		// Set own height to max of y and own height
		if (m_yExtentLP < y) {
			m_yExtentLP = y;
		}
	}
}

void HTreeBox::setXRightToLeft(id_d_t id_d)
{
	HTreeNode *pNode = getNode(id_d);
	
	if (pNode->isTerminal()) {
		// Nothing to do. We have already reversed it around the x-axis.
		; 
	} else {
		// Node is a non-terminal


		// Layout children
		const Monad2IDDMMap& children_by_monads = getChildren(id_d);
		
		Monad2IDDMMap::const_iterator child_ci = children_by_monads.begin();
		while (child_ci != children_by_monads.end()) {
			id_d_t child_id_d = child_ci->second;

			this->setXRightToLeft(child_id_d);

			++child_ci;
		}

		// Find real, right-to-left X for this node
		pNode->setXRightToLeft(m_xExtentLP);
	}
}

void HTreeBox::findRoots()
{
	IDD2TreeNodeMap::iterator objs_it = m_objs.begin();
	while (objs_it != m_objs.end()) {
		HTreeNode *pNode = objs_it->second;
		id_d_t parent_id_d = pNode->getParentID_D();
		if (parent_id_d == NIL) {
			; // Do nothing
		} else if (m_objs.find(parent_id_d) == m_objs.end()) {
			pNode->setParentID_D(NIL);
			if (m_children.find(NIL) == m_children.end()) {
			  m_children.insert(std::pair<id_d_t, Monad2IDDMMap>(NIL, Monad2IDDMMap()));
			}
			Monad2IDDMMap::const_iterator monad_ci = m_children[parent_id_d].begin();
			Monad2IDDMMap::const_iterator monad_cend = m_children[parent_id_d].end();
			while (monad_ci != monad_cend) {
				monad_m fm = monad_ci->first;
				id_d_t id_d = monad_ci->second;
				// This strange incantation of
				// std::pair is necessary on Solaris,
				// as of 2009-01-22
				m_children[NIL].insert(std::pair<const monad_m, id_d_t>(fm, id_d));
				++monad_ci;
			}

			m_children.erase(parent_id_d);
		}
		++objs_it;
	}
}

void HTreeBox::treeIsFinished(void)
{
	findRoots();
}

const HTreeBox::Monad2IDDMMap& HTreeBox::getRoots()
{
	return this->getChildren(NIL);
}

const HTreeBox::Monad2IDDMMap& HTreeBox::getChildren(id_d_t parent_id_d) const
{
	const static Monad2IDDMMap empty_map = Monad2IDDMMap();
	IDD2Monad2IDDMMap::const_iterator ci;
	ci = m_children.find(parent_id_d);
	if (ci != m_children.end()) {
		return ci->second;
	} else {
		return empty_map; // Empty map
	}
}



void HTreeBox::DrawNode(wxDC *pDC, id_d_t id_d, int yParentMiddle, int yParentBottom, int xParentLeft, int xParentRight, int xOffset, int yOffset, const wxPen& pen)
{
	HTreeNode *pNode = getNode(id_d);
	
	// Nodes are layout with 0 as the bottom,
	// so we invert it here.
	int y = m_yExtentLP - pNode->getY();

	pNode->Draw(pDC, y, yParentMiddle, yParentBottom, xParentLeft, xParentRight, xOffset, yOffset, pen);

	// Draw children
	const Monad2IDDMMap& children_by_monads = getChildren(id_d);
	
	Monad2IDDMMap::const_iterator child_ci = children_by_monads.begin();
	while (child_ci != children_by_monads.end()) {
		id_d_t child_id_d = child_ci->second;
		this->DrawNode(pDC, child_id_d, pNode->getMiddleY(y), pNode->getBottomY(y), pNode->getX(), pNode->getX() + pNode->getMainBoxWidth(), xOffset, yOffset, pen);
		
		++child_ci;
	}
}




////////////////////////////////////////////////////////////////////////////
//
// Vertical tree canvas
//
////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(HTreeCanvas, wxScrolledWindow)
   EVT_MOUSE_EVENTS(HTreeCanvas::OnMouseEvent)
   EVT_SIZE(HTreeCanvas::OnSize)
END_EVENT_TABLE()

HTreeCanvas::HTreeCanvas(ViewMetrics *pMetrics,
			 wxWindow *pParent, 
			 const wxPoint& pos, 
			 const wxSize& size, 
			 const long style, 
			 bool bRightToLeft)
	: wxScrolledWindow(pParent, -1, pos, size, style),
	m_pBox(0),
	m_pViewMetrics(pMetrics),
	m_nMapMode(wxMM_TEXT),
	m_nMagnification(100)
{
	SetBackgroundColour(*wxWHITE);
	m_pBox = new HTreeBox(m_pViewMetrics,
			      false,   // bSpaceAfter
			      bRightToLeft);
			      
}    


HTreeCanvas::~HTreeCanvas()
{
	// WriteRegistrySettings();
	delete m_pViewMetrics;
}

/////////////////////////////////////////////////////////////////////////////
// HTreeCanvas drawing

//adjust heights of windows, typically when a sub-window grew by a line while
//typing
void HTreeCanvas::adjustHeights(wxDC* pDC)
{
	(void)(pDC); // Silence a warning
	
	wxSizeEvent evt;
	OnSize(evt);

	// Invalidate client area
	this->Refresh();
}


void HTreeCanvas::PrepareDC(wxDC *pDC)
{
	pDC->SetMapMode(m_nMapMode);
}


void HTreeCanvas::OnDraw(wxDC &dc)
{
	wxDC *pDC = &dc;
	PrepareDC(pDC);
	m_pBox->Draw(pDC, 10, 10);
}

void HTreeCanvas::GetClipBox(wxDC *pDC, wxRect& clipRect)
{
	(void)(pDC); // Silence a warning
	
	int x, y;
	int width, height;
	int xScrollPixelsPerUnit, yScrollPixelsPerUnit;
	GetViewStart(&x, &y);
	GetScrollPixelsPerUnit(&xScrollPixelsPerUnit, &yScrollPixelsPerUnit);
	x *= xScrollPixelsPerUnit;
	y *= yScrollPixelsPerUnit;
	GetClientSize(&width, &height);

	clipRect.SetX(x);
	clipRect.SetY(y);
	clipRect.SetWidth(width);
	clipRect.SetHeight(height);
}



void HTreeCanvas::DrawToBitmap(wxBitmap **ppBitmap)
{
	m_pBox->DrawToBitmap(ppBitmap);
}



void HTreeCanvas::DrawToClipboard(void)
{
	wxBitmap *pBitmap;

	DrawToBitmap(&pBitmap);

	// Copy bitmap to clipboard
	if (wxTheClipboard->Open()) {
		wxBitmapDataObject *pBDO = new wxBitmapDataObject(*pBitmap);
    
		wxTheClipboard->SetData(pBDO);
		wxTheClipboard->Close();
	} else {
		wxMessageBox(wxT("Could not open clipboard"),
			     wxT("Error"));
	}

	delete pBitmap;
}



void HTreeCanvas::HandleButtonDown(wxMouseEvent& event, wxPoint point, bool RightButton)
{
	(void)(event); // Silence a warning
	(void)(RightButton); // Silence a warning
	
	wxRect labelRect;
	wxClientDC dc(this);
	wxCoord lpX, lpY;
	this->CalcUnscrolledPosition(point.x, point.y, &lpX, &lpY);
	wxPoint pointLP(lpX, lpY);
	/*
	CTreeItem* tree_item = findTreeItemForPoint(pointLP, targetLine, labelRect);
	if (!tree_item) return;
	int targetIndent = tree_item->indent();
	if (pointLP.x < m_tp.kLeftMargin+(targetIndent+1)*m_tp.kIndentDistance && tree_item->expansionState() != kFixed) {
		toggleExpansionState(tree_item);
		// ScrollToGivenPosition(targetLine);
		return;
	} else if (labelRect.Inside(pointLP)) {
		if (RightButton) {
			HandleRightButton(event, tree_item, point);
		} else {
			HandleLeftButton(event, tree_item, targetLine);
		}
	}
	*/ // FIXME: Implement!
}


void HTreeCanvas::OnMouseEvent(wxMouseEvent& event)
{
	if (event.ButtonDown()) {
		wxClientDC dc(this);
		wxPoint point = event.GetLogicalPosition(dc);
		HandleButtonDown(event, point, event.RightIsDown());

		// As per the instructions for wxMouseEvent in the
		// wxWidgets instructions.
		// Otherwise, the underlying window won't get the
		// focus.
		event.Skip();
	} else {
		event.Skip();
	}
}

void HTreeCanvas::OnUpdate() 
{
	// Get client DC
	wxClientDC dc(this);

	// Make tree
	layout();

	// Make visual layout
	// CalculateLineTypes(); // FIXME: Implement

	// Set scroll sizes 
	adjustHeights(&dc); // Also calls Refresh()
}



void HTreeCanvas::OnSize(wxSizeEvent& event) 
{
	(void)(event); // Silence a warning

	// Get device context
	wxClientDC dc(this);
	dc.SetMapMode(m_nMapMode);

	if (m_pViewMetrics != 0) {
		if (UpdateViewMetricsViewWidth()) {
			// Make layout
			layout();
		}
	}

	// Get client rect in logical points
	wxCoord clientWidth, clientHeight;
	GetClientSize(&clientWidth, &clientHeight);
	//dc.DPtoLP(&clientrect);

	// Scroll to home if m_pBox->GetHeightLP() < clientrect.height
	if (m_pBox->GetHeightLP() < clientHeight)
		this->Scroll(0,0);

	int height; 
	height = 20;

	int width = m_pBox->GetWidthLP();
	width = (width < MIN_WIDTH) ? MIN_WIDTH : width;


	// Calculate sizes
	wxSize sizeTotal;
	wxSize sizePage;
	wxSize sizeLine;
	if (m_pViewMetrics) {
		sizeTotal = wxSize(width, m_pBox->GetHeightLP());
		sizePage  = wxSize(clientWidth, clientHeight);
		sizeLine  = wxSize(clientWidth/8, height);
	} else {
		sizeTotal = wxSize(width, m_pBox->GetHeightLP());
		sizePage  = wxSize(clientWidth, height*8);
		sizeLine  = wxSize(width/8, height);
	}

	// Set sizes
	this->SetVirtualSize(sizeTotal);
	this->SetScrollRate(sizeLine.GetWidth(), sizeLine.GetHeight());

	// Invalidate window.
	this->Refresh();
	// FIXME: Should I call event.Yield() here? Or something else to get it propagated upwards?
}



void HTreeCanvas::ChangeMagnification(int magnification)
{
	if (magnification == m_nMagnification) {
		return;
	} else {
		wxClientDC dc(this);
		wxDC *pDC = &dc;

		m_nMagnification = magnification;
		ASSERT_THROW(m_pViewMetrics != 0, "m_pViewMetrics was 0");
		m_pViewMetrics->ChangeMagnification(magnification, pDC);

		m_pBox->RecalculateExtents(pDC);

		OnUpdate();
	}
}

void HTreeCanvas::OnChangeInterlinearLine()
{
	wxClientDC dc(this);
	m_pViewMetrics->LoadFonts(m_nMagnification, &dc);
	m_pViewMetrics->Initialize(&dc);
	m_pViewMetrics->RecalculateInterlinearLineHeight(&dc);
	UpdateAllViews();
}


void HTreeCanvas::InitializeViewMetrics()
{
	// Load fonts
	wxClientDC dc(this);
	m_pViewMetrics->LoadFonts(m_nMagnification, &dc);

	// Initialize
	m_pViewMetrics->Initialize(&dc);

	// Calculate interlinear height
	m_pViewMetrics->RecalculateInterlinearLineHeight(&dc);

	// Update ViewMetrics with client width & height
	/*
	// NOTE: This is done in UpdateViewMetricsViewWidth() as well,
	// which we call right afterwards, so this is a waste.
	int nWidth, nHeight;
	this->GetClientSize(&nWidth, &nHeight);
	wxScreenDC screenDC;
	m_pViewMetrics->Update(&screenDC, nWidth, nHeight);
	*/

	// Update ViewMetrics with view width
	UpdateViewMetricsViewWidth();
}

bool HTreeCanvas::UpdateViewMetricsViewWidth(void)
{
	if (m_pViewMetrics == 0) {
		return false;
	} else {
		bool result = false;
		int xNewValue;
		int xMinimum = m_pViewMetrics->ConvertMMtoLP(50);
		int xGapAtRight = m_pViewMetrics->xGapAtRightLP;

		// Get client rect
		int nClientWidth, nClientHeight;
		this->GetClientSize(&nClientWidth, &nClientHeight);

		// Convert client rect to LP
		wxClientDC dc(this);
		int nLogicalWidth = dc.DeviceToLogicalXRel(nClientWidth);

		// Calculate new view width for m_pViewMetrics.
		xNewValue = nLogicalWidth - xGapAtRight;
		if (xNewValue < xMinimum)
			xNewValue= xMinimum;

		// Should we update?
		if (m_pViewMetrics->m_nViewWidthLP != xNewValue) {
			// Yes, we should.
			m_pViewMetrics->m_nViewWidthLP = xNewValue;
			result = true;
		}

		m_pViewMetrics->Update(&dc, nClientWidth, nClientHeight);

		return result;
	}
}




void HTreeCanvas::ShowPopup(wxMenu *pMenuPopUp, wxPoint point)
{
	this->PopupMenu(pMenuPopUp, point);
}

////////////////////////////////////////////////////////////////////////////
//
// HTreeDrawerConfiguration
//
////////////////////////////////////////////////////////////////////////////

HTreeDrawerConfiguration::HTreeDrawerConfiguration(bool bIsRightToLeft)
{
        m_bIsRightToLeft = bIsRightToLeft;
        m_pViewMetrics = 0;
}


HTreeDrawerConfiguration::~HTreeDrawerConfiguration()
{
        delete m_pViewMetrics;
}


void HTreeDrawerConfiguration::setTerminalType(const std::string& terminal_object_type_name,
                                               const std::string& terminal_parent_feature)
{
        m_terminal_object_type_name = terminal_object_type_name;
        m_terminal_parent_feature = terminal_parent_feature;
}


void HTreeDrawerConfiguration::addTerminalFeature(const std::string& feature_name,
                                                  const std::string& feature_font_name)
{
        m_terminal_display_features.push_back(feature_name);
        m_terminal_display_feature_font_names.push_back(feature_font_name);
}


void HTreeDrawerConfiguration::addNonTerminalType(const std::string& nonterminal_object_type_name,
                                                  const std::string& nonterminal_parent_feature)
{
        std::string OTN = normalizeOTName(nonterminal_object_type_name);
        m_nonterminal_display_features.insert(std::make_pair(OTN, std::list<std::string>()));
        m_nonterminal_parent_features.insert(std::make_pair(OTN, nonterminal_parent_feature));
}


void HTreeDrawerConfiguration::addNonTerminalFeature(const std::string& nonterminal_object_type_name, const std::string& feature_name)
{
        std::string OTN = normalizeOTName(nonterminal_object_type_name);
        m_nonterminal_display_features[OTN].push_back(feature_name);
}


 // Creates m_pViewMetrics
void HTreeDrawerConfiguration::freeze()
{
        if (m_pViewMetrics == 0) {
                int nNoOfInterlinearLines = m_terminal_display_features.size();
                m_pViewMetrics = new ViewMetrics(nNoOfInterlinearLines,
                                                 wxMM_TEXT,
                                                 isRightToLeft(),
                                                 false, // bIsPrinting
                                                 100); // nMagnification
        }
}


std::list<std::string> HTreeDrawerConfiguration::getNonTerminalObjectTypeNames(void) const
{
        std::list<std::string> result_list;
        std::map<std::string, std::list<std::string> >::const_iterator ci = m_nonterminal_display_features.begin();
        while (ci != m_nonterminal_display_features.end()) {
                std::string nonterminal_object_type_name = ci->first;
                result_list.push_back(nonterminal_object_type_name);
                ++ci;
        }
        return result_list;
}


std::string HTreeDrawerConfiguration::getNonTerminalParentFeature(const std::string& nonterminal_object_type_name) const
{
        std::string OTN = normalizeOTName(nonterminal_object_type_name);
        std::string result = m_nonterminal_parent_features.find(OTN)->second;
        return result;
}


std::list<std::string> HTreeDrawerConfiguration::getNonTerminalDisplayFeatures(const std::string& nonterminal_object_type_name) const
{
        std::string OTN = normalizeOTName(nonterminal_object_type_name);
        std::list<std::string> result = m_nonterminal_display_features.find(OTN)->second;
        return result;
}


std::string HTreeDrawerConfiguration::getTerminalObjectTypeName(void) const
{
        return m_terminal_object_type_name;
}


std::string HTreeDrawerConfiguration::getTerminalParentFeature(void) const
{
        return m_terminal_parent_feature;
}


std::list<std::string> HTreeDrawerConfiguration::getTerminalDisplayFeatures(void) const
{
        return m_terminal_display_features;
}



////////////////////////////////////////////////////////////////////////////
//
// HTreeDrawer
//
////////////////////////////////////////////////////////////////////////////


// Does not own pConf
HTreeDrawer::HTreeDrawer(HTreeDrawerConfiguration *pConf)
{
        m_pConf = pConf;
        m_pConf->freeze(); // Safe to do even if done before!

        m_pTreeBox = 0;
}



HTreeDrawer::~HTreeDrawer()
{
        clearTree();
        // Do not delete m_pConf -- you do not own it!
}


bool HTreeDrawer::execQuery(EmdrosEnv *pEnv, const std::string& query)
{
        bool bCompilerResult;
        if (!pEnv->executeString(query,
                                 bCompilerResult,
                                 false,    // bPrintResult
                                 false)) { // bReportError
                return false;
        }
        if (!bCompilerResult) {
                return false;
        }
        return true;
}

bool HTreeDrawer::harvest(EmdrosEnv *pEnv, monad_m first_monad, monad_m last_monad)
{
        clearTree();
        m_pTreeBox = new HTreeBox(m_pConf->getViewMetrics(),
                                  false, // bSpaceAfter (space after whole box?)
                                  m_pConf->isRightToLeft());
        if (!harvestTerminals(pEnv, first_monad, last_monad)) {
                return false;
        }
        if (!harvestNonTerminals(pEnv, first_monad, last_monad)) {
                return false;
        }
        return true;
}


bool HTreeDrawer::harvestTerminals(EmdrosEnv *pEnv, monad_m first_monad, monad_m last_monad)
{
        std::string query = "GET OBJECTS HAVING MONADS IN {" + monad_m2string(first_monad) + '-' + monad_m2string(last_monad) + "}\n"
                + '['
                + m_pConf->getTerminalObjectTypeName()
                + " GET "
                + m_pConf->getTerminalParentFeature();
        std::list<std::string> terminal_display_features = m_pConf->getTerminalDisplayFeatures();
        std::list<std::string>::const_iterator ci = terminal_display_features.begin();
        while (ci != terminal_display_features.end()) {
                std::string feature_name = *ci;
                query += ", " + feature_name;
                ++ci;
        }

        query += "]\nGO";
        std::cerr << query;

        if (!execQuery(pEnv, query)) {
                return false;
        }
        
        if (!pEnv->isFlatSheaf()) {
                return false;
        }

        FlatSheaf *pFlatSheaf = pEnv->takeOverFlatSheaf();
        FlatSheafConstIterator fsci = pFlatSheaf->const_iterator();
        FlatStraw *pFlatStraw = fsci.next();
        FlatStrawConstIterator fstrci = pFlatStraw->const_iterator();

        wxBitmap dummy_bitmap(1024,768,32);
        wxMemoryDC *pDC = new wxMemoryDC();
	pDC->SelectObject(dummy_bitmap);

        while (fstrci.hasNext()) {
                MatchedObject *pMo = fstrci.next();
                SetOfMonads som = pMo->getMonads();
                id_d_t id_d = pMo->getID_D();
                const EMdFValue *pParentEMdFValue = pMo->getEMdFValue(0);
                id_d_t parent_id_d = pParentEMdFValue->getID_D();
                ci = terminal_display_features.begin();
                int nEMdFValueIndex = 1;
                wxstringvec_t feature_wxstrings;
                while (ci != terminal_display_features.end()) {
                        std::string feature_value = pMo->getFeatureAsString(nEMdFValueIndex);
                        feature_wxstrings.push_back(wxString(feature_value.c_str(), wxConvUTF8));
                        ++ci;
                        ++nEMdFValueIndex;
                }

                // TODO: Add vertical riser labels
                
                LayoutBox *pTerminalBox = 
                        new LayoutBox(pDC,
                                      feature_wxstrings,
                                      m_pConf->getViewMetrics(),
                                      true, // bSpaceAfter
                                      m_pConf->isRightToLeft());

                HTerminalNode *pTerminalNode =
                        new HTerminalNode(id_d,
                                          parent_id_d,
                                          som,
                                          0, // pBranchLabels // TODO: Fill in!
                                          true, // bOwnsBranchLabels
                                          pTerminalBox,
                                          true,  // bOwnsTerminalBox
					  true); // bBranchLabelsBelowMainBox
                m_pTreeBox->addTerminal(pTerminalNode);
        }

        delete pDC;

        return true;
}

bool HTreeDrawer::harvestNonTerminals(EmdrosEnv *pEnv, monad_m first_monad, monad_m last_monad)
{
        std::list<std::string> nonterminal_object_type_names = 
                m_pConf->getNonTerminalObjectTypeNames();
        std::list<std::string>::const_iterator ci = nonterminal_object_type_names.begin();
        while (ci != nonterminal_object_type_names.end()) {
                std::string OTN = *ci;
                if (!harvestNonTerminalObjectType(OTN, pEnv, first_monad, last_monad)) {
                        return false;
                }
                ++ci;
        }
        return true;
}


bool HTreeDrawer::harvestNonTerminalObjectType(const std::string& OTN, EmdrosEnv *pEnv, monad_m first_monad, monad_m last_monad)
{
        std::string query = "GET OBJECTS HAVING MONADS IN {" + monad_m2string(first_monad) + '-' + monad_m2string(last_monad) + "}\n"
                + '['
                + OTN
                + " GET "
                + m_pConf->getNonTerminalParentFeature(OTN);
        std::list<std::string> nonterminal_display_features = m_pConf->getNonTerminalDisplayFeatures(OTN);
        std::list<std::string>::const_iterator ci = nonterminal_display_features.begin();
        while (ci != nonterminal_display_features.end()) {
                std::string feature_name = *ci;
                query += ", " + feature_name;
                ++ci;
        }

        query += "]\nGO";
        std::cerr << query;

        if (!execQuery(pEnv, query)) {
                return false;
        }
        
        if (!pEnv->isFlatSheaf()) {
                return false;
        }

        FlatSheaf *pFlatSheaf = pEnv->takeOverFlatSheaf();
        FlatSheafConstIterator fsci = pFlatSheaf->const_iterator();
        FlatStraw *pFlatStraw = fsci.next();
        FlatStrawConstIterator fstrci = pFlatStraw->const_iterator();

        wxBitmap dummy_bitmap(1024,768,32);
        wxMemoryDC *pDC = new wxMemoryDC();
	pDC->SelectObject(dummy_bitmap);

        while (fstrci.hasNext()) {
                MatchedObject *pMo = fstrci.next();
                SetOfMonads som = pMo->getMonads();
                id_d_t id_d = pMo->getID_D();
                const EMdFValue *pParentEMdFValue = pMo->getEMdFValue(0);
                id_d_t parent_id_d = pParentEMdFValue->getID_D();
                ci = nonterminal_display_features.begin();
                int nEMdFValueIndex = 1;
                wxstringvec_t feature_wxstrings;
                while (ci != nonterminal_display_features.end()) {
                        std::string feature_value = pMo->getFeatureAsString(nEMdFValueIndex);
                        feature_wxstrings.push_back(wxString(feature_value.c_str(), wxConvUTF8));
                        ++ci;
                        ++nEMdFValueIndex;
                }

                // TODO: Add vertical riser labels
                
                StackedLatinLayoutBox *pNonTerminalBox = 
                        new StackedLatinLayoutBox(pDC,
                                                  feature_wxstrings,
                                                  m_pConf->getViewMetrics(),
                                                  *wxBLACK,
                                                  false,  // bSpaceAfter
						  true); // bIsCentered

                HNonTerminalNode *pNonTerminalNode =
                        new HNonTerminalNode(id_d,
                                             parent_id_d,
                                             som,
                                             0, // pBranchLabels // TODO: Fill in!
                                             true, // bOwnsBranchLabels
                                             pNonTerminalBox,
                                             true,  // bOwnsNonTerminalBox
					     true); // bBranchLabelsBelowMainBox
                m_pTreeBox->addNonTerminal(pNonTerminalNode);
        }

        delete pDC;

        return true;
}


wxBitmap HTreeDrawer::draw(int magnification)
{
        m_pTreeBox->layout();

        ViewMetrics *pViewMetrics = m_pConf->getViewMetrics();
        if (magnification != pViewMetrics->m_nMagnification) {
                wxBitmap dummy_bitmap(1024,
                                      768,
                                      32); // depth
                wxMemoryDC *pTempDC = new wxMemoryDC();
		pTempDC->SelectObject(dummy_bitmap);

                pViewMetrics->ChangeMagnification(magnification, pTempDC);
                m_pTreeBox->RecalculateExtents(pTempDC);

                delete pTempDC;
        }

        wxBitmap result_bitmap(m_pTreeBox->GetWidthLP(),
                               m_pTreeBox->GetHeightLP(),
                               32); // depth
        wxMemoryDC *pDC = new wxMemoryDC();
	pDC->SelectObject(result_bitmap);


        m_pTreeBox->Draw(pDC, 0, 0);


        delete pDC;
        return result_bitmap;
}


void HTreeDrawer::clearTree()
{
        delete m_pTreeBox;
        m_pTreeBox = 0;
}


