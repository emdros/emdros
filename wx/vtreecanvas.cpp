//
// Vertical Tree Canvas : implementation file
//
// Morphed from some code originally by John Thomson.
//
// Copyright assigned to Ulrik Petersen by John Thomson's superior,
// Steve Echerd, in a letter dated November 11, 2003.  John Thomson's
// informal consent was given in an email from John Thomson to Ulrik
// Petersen dated August 13, 2004.
//
// Created: 3/29-2000
// Last update: 5/17-2019
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



#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "vtreecanvas.h"
#endif

#include <wx/wx.h>
#include <wx/pen.h>
#include <wx/dataobj.h>
#include <wx/clipbrd.h>
#include <wx/version.h> // for wxCHECK_VERSION
//#include <mainframe.h>
//#include <ltc.h>
#include <treeitem.h>
//#include <fonts_struct.h>

#include <iostream>


#include <viewmetrics.h>
#include <vtreecanvas.h>
#include <debug.h> // For ASSERT_THROW



#if !defined(TRACE)
#define TRACE(S) /* Nothing. */
#endif


///////////////////////////////////////////////////////////////
//
// VTreePrintout
//
///////////////////////////////////////////////////////////////


VTreePrintout::VTreePrintout(wxString HeaderString, const wxString& title, wxPageSetupDialogData *pPageSetupDlgData) : wxPrintout(title), m_nMaxPage(32000), m_pPageSetupDlgData(pPageSetupDlgData) 
{ 
	m_strHeaderString = HeaderString;
}

wxSize VTreePrintout::GetTopLeftMarginMM()
{
	if (m_pPageSetupDlgData != 0) {
		wxSize result;
		wxPoint pt = m_pPageSetupDlgData->GetMarginTopLeft();
		result.SetWidth(pt.x);
		result.SetHeight(pt.y);
		return result;
	} else {
		return wxSize(25,30);
	}
}

wxSize VTreePrintout::GetBottomRightMarginMM()
{
	if (m_pPageSetupDlgData != 0) {
		wxSize result;
		wxPoint pt = m_pPageSetupDlgData->GetMarginBottomRight();
		result.SetWidth(pt.x);
		result.SetHeight(pt.y);
		return result;
	} else {
		return wxSize(25,30);
	}
}


void VTreePrintout::SetScaleFactor()
{
	wxDC *pDC = GetDC();
    
	int xPrinterPPI, yPrinterPPI;
	int xScreenPPI, yScreenPPI;
	GetPPIScreen(&xScreenPPI, &yScreenPPI);
	/*
	  wxSize DCPPI = pDC->GetPPI();
	  xScreenPPI = DCPPI.GetWidth();
	  yScreenPPI = DCPPI.GetHeight();
	*/

#ifdef __WXGTK__
	xScreenPPI = 72;
#endif
	GetPPIPrinter(&xPrinterPPI, &yPrinterPPI);
    
	double scale = (double)(((double)xPrinterPPI)/((double)xScreenPPI));
    
	int dcWidth, dcHeight;
	int pageWidth, pageHeight;
	GetPageSizePixels(&pageWidth, &pageHeight);
	pDC->GetSize(&dcWidth, &dcHeight);
	double myScale = scale * (double)(((double)dcWidth)/((double)pageWidth));
	pDC->SetUserScale(myScale, myScale);
	//GetVTreeCanvas()->OnBeginPrinting(pDC, this);
}


bool VTreePrintout::OnPrintPage(int page)
{
	SetScaleFactor();

	wxDC *pDC = GetDC();
  
	GetVTreeCanvas()->OnPrint(pDC, page); 
	return TRUE;
}

bool VTreePrintout::HasPage(int page)
{
	if (page > m_nMaxPage) {
		return FALSE;
	} else {
		return TRUE;
	}
}


bool VTreePrintout::OnBeginDocument(int startPage, int endPage)
{
	SetScaleFactor();

	if (!wxPrintout::OnBeginDocument(startPage, endPage)) {
		return FALSE;
	} else {
		return TRUE;
	}
}

void VTreePrintout::OnBeginPrinting()
{
	SetScaleFactor();

	GetVTreeCanvas()->OnBeginPrinting(GetDC(), this);
}



void VTreePrintout::OnEndPrinting()
{
	GetVTreeCanvas()->OnEndPrinting(); 
}



void VTreePrintout::GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo)
{
	// NOTE: It doesn't really matter what we say here;
	// It will never be called by anything except the print preview,
	// and that before we have had a chance to paginate.
	if (m_nMaxPage == 0) {
		*minPage = 1;
		*maxPage = 32000;
		*selPageFrom = 1;
		*selPageTo = 1;
	} else {
		*minPage = 1;
		*maxPage = m_nMaxPage;
		*selPageFrom = 1;
		*selPageTo = m_nMaxPage;
	}
}




////////////////////////////////////////////////////////////////////////////
//
// VertialTreeView
//
////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(VTreeCanvas, wxScrolledWindow)
   EVT_MOUSE_EVENTS(VTreeCanvas::OnMouseEvent)
   EVT_SIZE(VTreeCanvas::OnSize)
END_EVENT_TABLE()

VTreeCanvas::VTreeCanvas(wxWindow *pParent, 
			 const wxPoint& pos, 
			 const wxSize& size, 
			 const long style)
: wxScrolledWindow(pParent, -1, pos, size, style),
	m_nMapMode(wxMM_TEXT),
	m_nMagnification(100),
	pViewMetrics(0),
	m_labelWidth(120), 
	m_iOverallHeight(-1),
	m_pPrintMetrics(0),
	m_pFontHeaderFont(0)
{
}    



/*
  bool VTreeCanvas::OnCreate(wxDocument *pDoc, long flags) 
  {
  SetScrollbars(10,10,200,200);

  #ifdef __X__
  int nWidth, nHeight;
  m_pFrame->GetSize(&nWidth, &nHeight);
  m_pFrame->SetSize(-1, -1, nWidth, nHeight);
  #endif

  m_pFrame->Show(TRUE);
  Activate(TRUE);

  m_pFrame->Maximize(true);
  return true;
  }

*/

int VTreeCanvas::get_starting_x(size_t indent_index, bool printing) 
{
	if (printing) {
		return m_tpPrintTP.kIndentDistance*(indent_index) + m_pointUpperLeft.x;
	} else {
		return m_tp.kIndentDistance*(indent_index) + m_tp.kLeftMargin;
	}
}

VTreeCanvas::~VTreeCanvas()
{
	// WriteRegistrySettings();
	for(size_t j = 0; j < m_tree_items.size(); j++) 
		delete m_tree_items[j];

	delete m_pPrintMetrics;
	delete pViewMetrics;
}


int VTreeCanvas::get_treeitem_indent(size_t index, bool is_printing)
{
	// Calculate which vector to use
	TreeItemVec& vec = is_printing ? m_print_tree_items : m_tree_items;

	// Calculate indent
	return vec[index]->indent();
}

/*
  void VTreeCanvas::WriteRegistrySettings(void) const
  {
  CInterlinearView::WriteRegistrySettings();
  wxString classname = GetRuntimeClass()->m_lpszClassName;
  theApp.WriteProfileInt(classname, wxT("MarginTop"), m_rectMargins.GetY());
  theApp.WriteProfileInt(classname, wxT("MarginBottom"), m_rectMargins.bottom);
  theApp.WriteProfileInt(classname, wxT("MarginLeft"), m_rectMargins.GetLeft());
  theApp.WriteProfileInt(classname, wxT("MarginRight"), m_rectMargins.right);
  theApp.WriteProfileInt(classname, wxT("PaperOrientation"), m_dmOrientation);
  theApp.WriteProfileInt(classname, wxT("PaperSize"), m_dmPaperSize);
  }
*/

/*
  void VTreeCanvas::ReadRegistrySettings(void)
  {
  CInterlinearView::ReadRegistrySettings();
  wxString classname = GetRuntimeClass()->m_lpszClassName;
  int top = theApp.GetProfileInt(classname, wxT("MarginTop"), 1000);
  int bottom = theApp.GetProfileInt(classname, wxT("MarginBottom"), 1000);
  int left = theApp.GetProfileInt(classname, wxT("MarginLeft"), 1000);
  int right = theApp.GetProfileInt(classname, wxT("MarginRight"), 1000);
  m_dmOrientation = (short) theApp.GetProfileInt(classname, wxT("PaperOrientation"), DMORIENT_PORTRAIT);
  m_dmPaperSize = (short) theApp.GetProfileInt(classname, wxT("PaperSize"), DMPAPER_LETTER);

  m_rectMargins.SetRect(left, top, right, bottom);
  }
*/


/////////////////////////////////////////////////////////////////////////////
// VTreeCanvas drawing

int VTreeCanvas::GetHeightOfItem(size_t index, bool printing, int FirstLineInItem, int LastLineInItem)
{
	int height;
	if (printing) {
		CTreeItem* tree_item = m_print_tree_items[index];
		height = tree_item->height(FirstLineInItem, LastLineInItem);
		int boxHeight = m_tpPrintTP.kBoxHeight;
		if (height < boxHeight) height = boxHeight; //ensure room for tree symbol
		if (LastLineInItem >= tree_item->get_last_line_in_paragraph()) 
			height += m_tpPrintTP.kMinYDistanceBetweenItems;
	} else {
		CTreeItem* tree_item = m_tree_items[index];
		height = tree_item->height(FirstLineInItem, LastLineInItem);
		int boxHeight = m_tp.kBoxHeight;
		if (height < boxHeight) height = boxHeight; //ensure room for tree symbol
		if (LastLineInItem >= tree_item->get_last_line_in_paragraph()) 
			height += m_tp.kMinYDistanceBetweenItems;
	}
    
	return height;
}

int VTreeCanvas::GetHeightOfItemInTree(size_t index, bool printing, int FirstLineInItem, int LastLineInItem)
{
	if (printing) {
		return GetHeightOfItem(index, printing, FirstLineInItem, LastLineInItem);
	} else {
		return GetHeightOfItem(index, printing, FirstLineInItem, LastLineInItem);
	}
}




//Find the tree_item that corresponds to the given click points (in logical coords,
//relative to top left of scroll area). Also figure the rectangle for the label
//of that tree_item, if any--otherwise, don't alter it. Also compute the line number,
//that is, the index in m_tree_items of the result tree_item (-1 if not found)
//If an tree_item is not found, make the rectangle for the last tree_item
CTreeItem* VTreeCanvas::findTreeItemForPoint(wxPoint point,
					     int& targetLine, wxRect& labelRect)
{
	wxClientDC dc(this);

	targetLine = -1;
	if(m_tree_items.size() == 0) return 0;
	int heightSoFar = 0;
	int targetHeight = point.y;
	targetLine = -1;
	CTreeItem* tree_item = 0;
	for(size_t i = 0; i < m_tree_items.size(); ++i) {
		tree_item = m_tree_items[i];
		heightSoFar += m_tp.kMMScaleFactor*GetHeightOfItemInTree(i, false);
		if((m_tp.kMMScaleFactor*heightSoFar) > (m_tp.kMMScaleFactor*targetHeight)) {
			targetLine = (int) i;
			break;
		}
	}
	if(targetLine<0) {
		//compute rectangle for last tree_item
		tree_item = m_tree_items.back();
		targetLine = ((int)m_tree_items.size())-1;
	}
	labelRect.SetY(heightSoFar-m_tp.kMMScaleFactor*tree_item->height());
	labelRect.SetHeight(m_tp.kMMScaleFactor*tree_item->height());
	labelRect.SetX(get_starting_x(tree_item->indent()+1, false));
	wxSize size = tree_item->size_of_box();
	labelRect.SetHeight(m_tp.kMMScaleFactor*size.GetHeight());
	labelRect.SetWidth(size.GetWidth());
	if(targetLine<0) return 0; //click not near any field
	return tree_item;
}

//Toggle the expansion state of the specified tree_item, which is at the
//specified index in m_tree_items. Caller has confirmed that it can be toggled.
void VTreeCanvas::toggleExpansionState(CTreeItem* tree_item)
{
	if (tree_item->expansionState() == kCollapsed) {
		expand(tree_item->get_id_d_t());
	} else {
		collapse(tree_item->get_id_d_t()); // kExpanded
	}

	// Make tree
	wxClientDC dc(this);

	/*
	make_tree(m_tree_items, pViewMetrics, &dc);

	// Make visual layout
	CalculateLineTypes();

	// Set scroll sizes
	adjustHeights(&dc); // Calls invalidate
	*/



	// Resize
	//	wxRect rect;
	//	GetClientRect(&rect);
	//	OnSize(SIZE_RESTORED, rect.right, rect.bottom);

	// Update all other views
	UpdateAllViews();
	
	// Invalidate ourselves
	this->Refresh();	
}


//adjust heights of windows, typically when a sub-window grew by a line while
//typing
void VTreeCanvas::adjustHeights(wxDC* pDC)
{
	(void)(pDC); // Silence a warning
	
	// Calculate overall height
	CalculateOverallHeight();

	wxSizeEvent evt;
	OnSize(evt);

	// Invalidate client area
	this->Refresh();
}

void VTreeCanvas::GetClipBox(wxDC *pDC, wxRect& clipRect)
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

#if wxCHECK_VERSION(3,0,0)
void VTreeCanvas::DoPrepareDC(wxDC& dc)
#else
void VTreeCanvas::PrepareDC(wxDC& dc)
#endif
{
	wxScrolledWindow::DoPrepareDC(dc);
	dc.SetMapMode(m_nMapMode);
}


void VTreeCanvas::OnDraw(wxDC &dc)
{
	wxDC *pDC = &dc;
#if wxCHECK_VERSION(3,0,0)
	// wxWidgets version is >= 3.0.0. dc has already been
	// prepared.
#else
	// Only do this if wxWidgets version is < 3.0.0
	PrepareDC(dc);
#endif
	dc.GetUserScale(&m_xScaleFactor, &m_yScaleFactor);
	if(m_tree_items.size() == 0)
		return;
	int y = 0;
	wxRect clipRect;
	GetClipBox(pDC, clipRect);
	size_t startIndex = 0;
	size_t endIndex = m_tree_items.size()-1;
	int yItemHeight = 0;
	int nMMScaleFactor = pViewMetrics->nMMScaleFactor();
	for(size_t i = startIndex; i <= endIndex; i++) {
		// Get item height
		yItemHeight = GetHeightOfItemInTree(i, false);

		// Only draw if (part of) item is after top of clipbox
		if ((y + nMMScaleFactor*yItemHeight)*nMMScaleFactor >= clipRect.GetY()*nMMScaleFactor) {
			DrawTreeItem(pDC, i, y, pViewMetrics, m_tp, false);
			DrawLines(pDC, i, y, pViewMetrics, m_tp, false);
		}

		// Update y
		y += yItemHeight*nMMScaleFactor;

		// If we have processed past the clipbox, don't do any more
		if (y*nMMScaleFactor > (clipRect.GetHeight() + clipRect.GetY()) *nMMScaleFactor) {
			endIndex = i;
		}
	}
}

void VTreeCanvas::DrawTreeItem(wxDC* pDC, size_t item_index, int y, ViewMetrics* pMetrics, const CTreeParameters& tp, bool bIsPrinting, int FirstLineInItem, int LastLineInItem)
{
	(void)(tp); // Silence a warning
	
	CTreeItem* tree_item;
	if (bIsPrinting) {
		tree_item = m_print_tree_items[item_index];
	} else {
		tree_item = m_tree_items[item_index];
	}
	int x;

	x = get_starting_x(tree_item->indent()+1, bIsPrinting);
	int yBox = y;
	int yParagraph = y;
	tree_item->Draw(pDC,
			pMetrics,
			x,
			yBox, yParagraph, 
			FirstLineInItem, LastLineInItem);
}


void VTreeCanvas::DrawLines(wxDC* pDC, size_t item_index, int y, ViewMetrics* pMetrics, const CTreeParameters& tp, bool bIsPrinting, int FirstLineInItem, int LastLineInItem)
{
	for (size_t indent_index = 0; indent_index < m_line_types[item_index].size(); indent_index++) {
		DrawLine(pDC, item_index, indent_index, y, pMetrics, tp, bIsPrinting, FirstLineInItem, LastLineInItem);
	}
}

void VTreeCanvas::DrawLine(wxDC* pDC, size_t item_index, size_t indent_index, int y, ViewMetrics* pMetrics, const CTreeParameters& tp, bool bIsPrinting, int FirstLineInItem, int LastLineInItem)
{
#if wxCHECK_VERSION(3,0,0)
	wxPen treePen(*wxLIGHT_GREY, tp.kTreeLineWidth, wxPENSTYLE_SOLID); //for tree lines
#else
	wxPen treePen(*wxLIGHT_GREY, tp.kTreeLineWidth, wxSOLID); //for tree lines
#endif
	pDC->SetPen(treePen);
    
	CTreeItem* pItem;
	if (bIsPrinting) {
		pItem = m_print_tree_items[item_index];
	} else {
		pItem = m_tree_items[item_index];
	}
	switch (m_line_types[item_index][indent_index]) {
	case ltNone:
		break;
	case ltLabelI: {
		int yLabelHeight = pItem->box_height();
		int yItemHeight = GetHeightOfItemInTree(item_index, bIsPrinting, FirstLineInItem, LastLineInItem);
		//std::cerr << yItemHeight;
		int xLine = get_starting_x(indent_index, bIsPrinting) + tp.kVerticalIndent;
		pDC->DrawLine(xLine, yLabelHeight*tp.kMMScaleFactor + y, xLine, yItemHeight*tp.kMMScaleFactor + y);
	}
		break;
	case ltI: {
		int yItemHeight = GetHeightOfItemInTree(item_index, bIsPrinting, FirstLineInItem, LastLineInItem);
		int xLine = get_starting_x(indent_index, bIsPrinting) + tp.kVerticalIndent;
		pDC->DrawLine(xLine, y, xLine, yItemHeight*tp.kMMScaleFactor + y);
	}
		break;
	case ltL:{
		int yMiddle = y + tp.kMMScaleFactor*(tp.kTopLineHeight/2);
		int xLine = get_starting_x(indent_index, bIsPrinting) + tp.kVerticalIndent;
		int xLineEnd = get_starting_x(indent_index+1, bIsPrinting) - tp.kTextGap;
		pDC->DrawLine(xLine, y, xLine, yMiddle);
		pDC->DrawLine(xLine, yMiddle, xLineEnd, yMiddle);
	}
		break;
	case ltT: {
		int yMiddle = y + tp.kMMScaleFactor*(tp.kTopLineHeight/2);
		int yItemHeight = y + tp.kMMScaleFactor*GetHeightOfItemInTree(item_index, bIsPrinting, FirstLineInItem, LastLineInItem);
		int xLine = get_starting_x(indent_index, bIsPrinting) + tp.kVerticalIndent;
		int xLineEnd = get_starting_x(indent_index+1, bIsPrinting) - tp.kTextGap;
		pDC->DrawLine(xLine, y, xLine, yItemHeight);
		pDC->DrawLine(xLine, yMiddle, xLineEnd, yMiddle);
	}
		break;
	case ltIcon:
	case ltIconL:
	case ltIconT:
		DrawIcon(pDC, item_index, indent_index, y, pMetrics, tp, bIsPrinting, FirstLineInItem, LastLineInItem);
		break;
	}

	pDC->SetPen(wxNullPen);
}

void VTreeCanvas::DrawIcon(wxDC* pDC, size_t item_index, size_t indent_index, int y, ViewMetrics* pMetrics, const CTreeParameters& tp, bool bIsPrinting, int FirstLineInItem, int LastLineInItem)
{
	(void)(pMetrics); // Silence a warning
	
	// Get tree item
	CTreeItem* pTreeItem;
	if (bIsPrinting) {
		pTreeItem = m_print_tree_items[item_index];
	} else {
		pTreeItem = m_tree_items[item_index];
	}

	// Get state
	TreeState state;
	state = pTreeItem->expansionState();

	// If this is a leaf, return
	if(state == kFixed) return;

	// See whether we should be drawing icon and box at all
	bool bDrawIcon;
	if (FirstLineInItem == 0)
		bDrawIcon = true;
	else
		bDrawIcon = false;

	// See whether we should draw a highlighted box rather than a normal one
	bool bDrawHighlightedBox;
	bDrawHighlightedBox = (state == kCollapsed) && pTreeItem->box_highlighted();

	// Get starting x
	int starting_x = get_starting_x(indent_index, bIsPrinting);

	// Make pens
#if wxCHECK_VERSION(3,0,0)
	wxPen blackPen(*wxBLACK, tp.kTreeLineWidth, wxPENSTYLE_SOLID); //for icon
	wxPen whitePen(*wxWHITE, tp.kTreeLineWidth, wxPENSTYLE_SOLID); //for icon
	wxPen treePen(*wxLIGHT_GREY, tp.kTreeLineWidth, wxPENSTYLE_SOLID); //for tree lines
#else
	wxPen blackPen(*wxBLACK, tp.kTreeLineWidth, wxSOLID); //for icon
	wxPen whitePen(*wxWHITE, tp.kTreeLineWidth, wxSOLID); //for icon
	wxPen treePen(*wxLIGHT_GREY, tp.kTreeLineWidth, wxSOLID); //for tree lines
#endif
	
	const wxColour *pBorderPenColor;
	if (bDrawHighlightedBox) {
		pBorderPenColor = wxWHITE;
	} else {
		pBorderPenColor = wxLIGHT_GREY;
	}
#if wxCHECK_VERSION(3,0,0)
	wxPen borderPen(*pBorderPenColor, tp.kTreeLineWidth, wxPENSTYLE_SOLID);
#else
	wxPen borderPen(*pBorderPenColor, tp.kTreeLineWidth, wxSOLID);
#endif

	//draw the box and horizontal line
	int leftBox = starting_x;
	int vLeftOver = tp.kTopLineHeight - tp.kBoxHeight;
	//	int vLeftOver = pTreeItem->box_height() - tp.kBoxHeight;
	int topBox = y + tp.kMMScaleFactor*(vLeftOver/2);
	int bottomBox = topBox + tp.kMMScaleFactor*tp.kBoxHeight;
	int rightBox = leftBox + tp.kBoxWidth;

	// Draw background for control box if necessary
	if (bDrawHighlightedBox && bDrawIcon) {
		// Calculate rectangle for box's background
		wxRect controlBoxBackground;
		int cbbgborder = tp.kOnePixel*2;
		controlBoxBackground.SetX(leftBox - cbbgborder);
		controlBoxBackground.SetY(topBox - tp.kMMScaleFactor*cbbgborder);
		// Plus one pixel because FillSolidRect draws one pixel above and to the left of the bottom and right
		controlBoxBackground.SetWidth(rightBox + cbbgborder + tp.kOnePixel - controlBoxBackground.GetX());
		controlBoxBackground.SetHeight(bottomBox + tp.kMMScaleFactor*(cbbgborder + tp.kOnePixel) - controlBoxBackground.GetY());

		// Make solid rect
		pDC->SetBrush(*wxBLACK_BRUSH);
		pDC->DrawRectangle(controlBoxBackground.GetX(),
				   controlBoxBackground.GetY(),
				   controlBoxBackground.GetWidth(),
				   controlBoxBackground.GetHeight());
		pDC->SetBrush(wxNullBrush);
	}

	//draw the actual box
	pDC->SetPen(borderPen);
	if (bDrawIcon) {
		pDC->DrawLine(leftBox, topBox, leftBox, bottomBox);
		pDC->DrawLine(leftBox, bottomBox, rightBox, bottomBox);
		pDC->DrawLine(rightBox, bottomBox, rightBox, topBox);
		pDC->DrawLine(rightBox, topBox, leftBox, topBox);
	}
	//draw the minus sign
	if (bDrawHighlightedBox) {
		pDC->SetPen(whitePen);
	} else {
		pDC->SetPen(blackPen);
	}
	int leftMinus = leftBox + tp.kIconGap;
	int yMinus = topBox + tp.kMMScaleFactor*(tp.kBoxHeight/2);
	if (bDrawIcon) {
		pDC->DrawLine(leftMinus + tp.kOnePixel, yMinus, leftMinus + tp.kIconWidth - tp.kOnePixel, yMinus);
	}

	//draw the line to the right of the box
	int branchRight = get_starting_x(indent_index+1, bIsPrinting) - tp.kTextGap;
	if (bDrawIcon) {
		pDC->SetPen(treePen);
		pDC->DrawLine(rightBox, yMinus, branchRight, yMinus);
	}


	// Draw the vertical in the plus if necessary
	int xVertical = get_starting_x(indent_index, bIsPrinting) + tp.kVerticalIndent;

	if(state == kCollapsed && bDrawIcon) {
		int topPlus = topBox + tp.kMMScaleFactor*(tp.kIconGap);
		if (bDrawHighlightedBox) {
			pDC->SetPen(whitePen);
		} else {
			pDC->SetPen(blackPen);
		}
		pDC->DrawLine(xVertical,topPlus + tp.kMMScaleFactor*tp.kOnePixel, 
			      xVertical,topPlus + tp.kMMScaleFactor*(tp.kIconHeight - tp.kOnePixel));
	} 

	LineType line_type = m_line_types[item_index][indent_index];
	switch (line_type) {
	case ltIcon:
		break;
	case ltIconT:
		{
			pDC->SetPen(treePen);
			if (bDrawIcon)
				{
					pDC->DrawLine(xVertical, y, xVertical, topBox);
					int yFinal = y + tp.kMMScaleFactor*GetHeightOfItemInTree(item_index, bIsPrinting, FirstLineInItem, LastLineInItem);
					pDC->DrawLine(xVertical, bottomBox, xVertical, yFinal);
				} else {
				int yFinal = y + tp.kMMScaleFactor*GetHeightOfItemInTree(item_index, bIsPrinting, FirstLineInItem, LastLineInItem);
				pDC->DrawLine(xVertical, y, xVertical, yFinal);
			}
		}
		break;
	case ltIconL:
		{
			if (bDrawIcon)
				{
					pDC->SetPen(treePen);
					pDC->DrawLine(xVertical, y, xVertical, topBox);
				}
		}
		break;
	default:
		break;
	}

	// Select old object
	pDC->SetPen(wxNullPen);
}


//Drawing the tree:
//- The starting point for drawing an object is kIndentDistance times its indent
//  plus kLeftMargin
//- Trees are drawn as part of the summary item
//- If an item has expansion state kFixed, draw no tree stuff
//- If it has expansion state kCollapsed, draw a box with a plus sign, and a line
//- If it has expansion state kExpanded, draw box with minus sign, line,
//	and for each subsequent node with a larger indent level, draw the tree,
//  that is, the horizontal lines extending from the branch into the box or label
//  of those that are one indent-level deeper, as well as the vertical line
//  under the present branch


/*
//draw the tree for the specified node
//Note: changes here may need to be reflected in OnLButtonDown.
void VTreeCanvas::drawTree(wxDC* pDC, CTreeItem* tree_item, int heightSoFar, int height, int startIndex)
{
TreeState state = tree_item->expansionState();
if(state == kFixed) return;

// See whether we should draw a highlighted box rather than a normal one
bool bDrawHighlightedBox = (state == kCollapsed) && tree_item->box_highlighted();

// Make pens
wxPen blackPen(wxPENSTYLE_SOLID, kOnePixel, RGB(0,0,0)); //for icon
wxPen whitePen(wxPENSTYLE_SOLID, kOnePixel, RGB(255,255,255)); //for icon
//This attempt really makes more of a dashed line
//wxPen treePen(PS_DOT, 1,::GetSysColor(COLOR_BTNSHADOW)); //for tree lines

//This one causes all drawing in the window to stop!
//CBrush treeBrush(::GetSysColor(COLOR_BTNSHADOW));
//LOGBRUSH lBrush;
//treeBrush.GetLogBrush(&lBrush);
//wxPen treePen(PS_COSMETIC|PS_ALTERNATE|PS_ENDCAP_ROUND|PS_JOIN_BEVEL,1,&lBrush,0);

//So we use this one although it does not produce exactly the appearance
//used in the Windows Explorer
wxPen treePen(wxPENSTYLE_SOLID, kOnePixel,::GetSysColor(COLOR_BTNSHADOW)); //for tree lines

//draw the box and horizontal line
int leftBox = kLeftMargin+tree_item->indent()*kIndentDistance;
//	int vLeftOver = tree_item->height() - kBoxHeight;
//	int topBox = heightSoFar - kMMScaleFactor*height + kMMScaleFactor*(vLeftOver/2);
int vLeftOver = kTopLineHeight - kBoxHeight;
int topBox = heightSoFar - kMMScaleFactor*height + kMMScaleFactor*(vLeftOver/2);
int bottomBox = topBox+kMMScaleFactor*kBoxHeight;
int rightBox = leftBox + kBoxWidth;

// Draw background for control box if necessary
if (bDrawHighlightedBox) {
// Calculate rectangle for box's background
wxRect controlBoxBackground;
int cbbgborder = kOnePixel*2;
controlBoxBackground.GetLeft()	 = leftBox - cbbgborder;
controlBoxBackground.GetY()    = topBox - kMMScaleFactor*cbbgborder;
// Plus one pixel because FillSolidRect draws one pixel above and to the left of the bottom and right
controlBoxBackground.right  = rightBox + cbbgborder + kOnePixel;
controlBoxBackground.bottom = bottomBox + kMMScaleFactor*(cbbgborder + kOnePixel);

// Save background color, since FillSolidRect sets this
wxColour* oldBkColor = pDC->GetBkColor();

// Make solid rect
pDC->FillSolidRect(&controlBoxBackground, RGB(0,0,0));

// Set old Background color
pDC->SetBkColor(oldBkColor);
}

//draw the actual control box
pDC->MoveTo(leftBox, topBox);
wxPen borderPen;
if (bDrawHighlightedBox) 
{
borderPen.CreatePen(wxPENSTYLE_SOLID, kOnePixel, RGB(255,255,255)); 
}
else
{
borderPen.CreatePen(wxPENSTYLE_SOLID, kOnePixel, ::GetSysColor(COLOR_BTNSHADOW)); 
}

pDC->SetPen(borderPen);
pDC->DrawLine(leftBox, bottomBox);
pDC->DrawLine(rightBox, bottomBox);
pDC->DrawLine(rightBox, topBox);
pDC->DrawLine(leftBox, topBox);

//draw the minus sign
if (bDrawHighlightedBox)
{
pDC->SetPen(whitePen);
}
else
{
pDC->SetPen(blackPen);
}
int leftMinus = leftBox + kIconGap;
int vMinus = topBox+kMMScaleFactor*(kBoxHeight/2);
pDC->DrawLine(leftMinus, vMinus, leftMinus+kIconWidth, vMinus);

//draw the line to the right of the box
int branchRight = leftBox+kIndentDistance-kTextGap;
pDC->SetPen(treePen);
pDC->DrawLine(rightBox, vMinus, branchRight, vMinus);

if(state == kCollapsed) 
{
//draw the vertical bar of the plus
int hPlus = leftMinus + kIconWidth/2;
int topPlus = topBox + kMMScaleFactor*(kIconGap);
if (bDrawHighlightedBox)
{
pDC->SetPen(whitePen);
}
else
{
pDC->SetPen(blackPen);
}
pDC->DrawLine(hPlus,topPlus, hPlus,topPlus+kMMScaleFactor*kIconHeight);
} 
else 
{
//draw the tree
int nMM = pDC->GetMapMode();
int nMyMM = m_nMapMode;
pDC->SetPen(treePen);
int rootIndent = tree_item->indent();
int endHeight = heightSoFar;
int branchVPos = 0; //outside the loop so we save the last one
int branchLeft = leftBox+kHalfIndent;
for(int i=startIndex+1; i<m_tree_items.size() && m_tree_items[i]->indent()>rootIndent; i++) 
{
CTreeItem* branch = m_tree_items[i];
//Draw a branch line only if this tree_item is indented exactly one more level
if(branch->indent() == rootIndent+1) 
{
//branchVPos is modified only if we draw a horizontal.
//The last one we draw gives the position for the
//vertical
//				branchVPos = endHeight + kMMScaleFactor*(branch->height()/2);
branchVPos = endHeight + kMMScaleFactor*(kTopLineHeight/2);
pDC->DrawLine(branchLeft, branchVPos, branchRight, branchVPos);
}
endHeight += kMMScaleFactor*(GetHeightOfItemPlusOnePixel(i));
}
//draw the vertical if any
if(branchVPos != 0) {
wxPoint start(branchLeft,bottomBox+kMMScaleFactor*kOnePixel);
wxPoint end(branchLeft,branchVPos);
pDC->DrawLine(start.GetX(), start.GetY(), end.GetX(), end.GetY());
}
}
pDC->Pen(wxNullPen);
}

*/

 /////////////////////////////////////////////////////////////////////////////
 // VTreeCanvas message handlers

 // Erases the "box highlight" feature of all members of tree
void VTreeCanvas::EraseBoxHighlight(void)
{
	for (size_t i = 0; i < m_tree_items.size(); i++) {
		m_tree_items[i]->box_highlighted(false);
	}
}

// Used to find the range of selection when shift-clicking
void VTreeCanvas::FindSelectionRange(size_t current_indent, size_t index, int& start, int& end)
{
	CTreeItem *pOtherItem;
	bool found = false;
	bool stop_search = false;
	long i = (long) index;

	// Search backwards
	while (!stop_search) {
		i--;
		if (i < 0) {
			stop_search = true;
		} else {
			pOtherItem = m_tree_items[i];

			if (pOtherItem->indent() != current_indent) {
				if (pOtherItem->indent() < current_indent)
					stop_search = true;
				else
					continue;
			} else	{
				if (pOtherItem->highlighted()) {
					stop_search = true;
					found = true;
				}
			}
		}
	}

	// Search forwards
	if (!found) {
		stop_search = false;
		i = index;
		while (!stop_search) {
			i++;
			if (i > (long) m_tree_items.size()) {
				stop_search = true;
			} else {
				pOtherItem = m_tree_items[i];

				if (pOtherItem->indent() != current_indent) {
					if (pOtherItem->indent() < current_indent)
						stop_search = true;
					else
						continue;
				} else {
					if (pOtherItem->highlighted()) {
						stop_search = true;
						found = true;
					}
				}
			}
		}
	}

	if (i < (long) index) {
		start = (int) i+1;
		end = (int) index;
	} else {
		start = (int) index;
		end = ((int) i)-1;
	}
}

void VTreeCanvas::CalculateNodeRect(size_t start_index, size_t end_index, wxRect &rectNode, bool is_printing)
{
	// Calculate end-X
	ViewMetrics *pVM = is_printing ? m_pPrintMetrics : pViewMetrics;
	int endX = pVM->m_nViewWidthLP;

	/*
	  int nDummyHeight, endX;
	  this->GetClientSize(&endX, &nDummyHeight); // NOTE: This won't work with is_printing = true!
	*/


	// Calculate height
	int height = 0;
	for (size_t i = start_index; i <= end_index; i++) {
		height += GetHeightOfItemInTree(i, is_printing);
	}

	// Set rectangle
	rectNode.SetX(0);
	rectNode.SetY(0);
	rectNode.SetWidth(endX);
	rectNode.SetHeight(height);
}


void VTreeCanvas::HandleButtonDown(wxMouseEvent& event, wxPoint point, bool RightButton)
{
	int targetLine;
	wxRect labelRect;
	wxClientDC dc(this);
	wxCoord lpX, lpY;
	this->CalcUnscrolledPosition(point.x, point.y, &lpX, &lpY);
	wxPoint pointLP(lpX, lpY);
	CTreeItem* tree_item = findTreeItemForPoint(pointLP, targetLine, labelRect);
	if (!tree_item) return;
	int targetIndent = tree_item->indent();
	if (pointLP.x < m_tp.kLeftMargin+(targetIndent+1)*m_tp.kIndentDistance && tree_item->expansionState() != kFixed) {
		toggleExpansionState(tree_item);
		// ScrollToGivenPosition(targetLine);
		return;
#if wxCHECK_VERSION(2,8,0)
	} else if (labelRect.Contains(pointLP)) {
#else
	} else if (labelRect.Inside(pointLP)) {
#endif
		if (RightButton) {
			HandleRightButton(event, tree_item, point);
		} else {
			HandleLeftButton(event, tree_item, targetLine);
		}
	}
}


void VTreeCanvas::OnMouseEvent(wxMouseEvent& event)
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

void VTreeCanvas::OnUpdate() 
{
	// Get client DC
	wxClientDC dc(this);

	// Make tree
	make_tree(m_tree_items, pViewMetrics, &dc);

	// Make visual layout
	CalculateLineTypes();

	// Set scroll sizes 
	adjustHeights(&dc); // Also calls Refresh()
}



void VTreeCanvas::OnSize(wxSizeEvent& event) 
{
	(void)(event); // Silence a warning
	
	// Get device context
	wxClientDC dc(this);
	dc.SetMapMode(m_nMapMode);

	if (pViewMetrics != 0) {
		if (UpdateViewMetricsViewWidth()) {
			// Make tree
			make_tree(m_tree_items, pViewMetrics, &dc);
	    
			// Make visual layout
			CalculateLineTypes();
		}
	}


	// Calculate overall height
	CalculateOverallHeight();

	// Get client rect in logical points
	wxCoord clientWidth, clientHeight;
	GetClientSize(&clientWidth, &clientHeight);
	//dc.DPtoLP(&clientrect);

	// Scroll to home if m_iOverallHeight < clientrect.height
	if (m_iOverallHeight < clientHeight)
		this->Scroll(0,0);

	int height; 
	if (m_tree_items.size() == 0)
		height = 20;
	else
		height = m_tree_items[0]->box_height();

	int width;
	if (pViewMetrics != 0) {
		width = (pViewMetrics->m_nViewWidthLP < MIN_WIDTH) ? MIN_WIDTH : pViewMetrics->m_nViewWidthLP;
	} else {
		width = (clientWidth < MIN_WIDTH) ? MIN_WIDTH : clientWidth;
	}

	// Calculate sizes
	wxSize sizeTotal;
	wxSize sizePage;
	wxSize sizeLine;
	if (pViewMetrics) {
		sizeTotal = wxSize(width, m_iOverallHeight);
		sizePage  = wxSize(width, clientHeight);
		sizeLine  = wxSize(clientWidth/8, height);
	} else {
		sizeTotal = wxSize(20, m_iOverallHeight);
		sizePage  = wxSize(20, height*8);
		sizeLine  = wxSize(20, height);
	}

	// Set sizes
	this->SetVirtualSize(sizeTotal);
	this->SetScrollRate(sizeLine.GetWidth(), sizeLine.GetHeight());

	// Invalidate window.
	this->Refresh();
	// FIXME: Should I call event.Yield() here? Or something else to get it propagated upwards?
}



void VTreeCanvas::ScrollToGivenPosition(int index, int deltaY)
{
	// Get client rect in pixels
	wxCoord clientWidth, clientHeight;
	GetClientSize(&clientWidth, &clientHeight);

	// Get m_iOverallHeight in device units
	wxClientDC dc(this);
	wxCoord xHeight;
	xHeight = dc.LogicalToDeviceX(m_iOverallHeight);

	if (xHeight < clientHeight) {
		Scroll(0, 0);
	} else {
		int yItemPos = pViewMetrics->nMMScaleFactor()*FindYofTreeItem(index) + deltaY;
		int xScrollPos = GetScrollPos(wxHORIZONTAL);
		// int yScrollPos = GetScrollPos(wxVERTICAL);

		int xPixelsPerScrollUnit;
		int yPixelsPerScrollUnit;
		GetScrollPixelsPerUnit(&xPixelsPerScrollUnit, &yPixelsPerScrollUnit);

		int xScrollUnits = xScrollPos / xPixelsPerScrollUnit;
		int yScrollUnits = yItemPos / yPixelsPerScrollUnit;


		Scroll(xScrollUnits, yScrollUnits);
		return;
		/*
		wxRect rectScrollRect(0, 
				      yScrollPos, 
				      pViewMetrics->m_nViewWidthLP, 
				      yScrollPos + clientHeight);


		// Only scroll if point is not already visible
#if wxCHECK_VERSION(2,8,0)
		if (rectScrollRect.Contains(wxPoint(xScrollPos, yItemPos))) {
			return;
		} else {
			Scroll(xScrollUnits, yScrollUnits);
		}
#else
		if (labelRect.Inside(wxPoint(xScrollPos, yItemPos))) {
			return;
		} else {
			Scroll(xScrollUnits, yScrollUnits);
		}
#endif
		*/
	}
}


int VTreeCanvas::FindYofTreeItem(size_t index)
{
	int heightSoFar = 0;
	for(size_t i=0; i<m_tree_items.size() && i < index; i++) {
		heightSoFar += GetHeightOfItemInTree(i, false);
	}

	return heightSoFar + 1;
}

int VTreeCanvas::get_index_from_oid(id_d_t oid, bool is_printing)
{
	// Calculate which vector to use
	TreeItemVec& vec = is_printing ? m_print_tree_items : m_tree_items;

	// Get index
	for (size_t i = 0; i < vec.size(); i++) {
		if (vec[i]->get_id_d_t() == oid)
			return (int) i;
	}
	return 0;
}

size_t VTreeCanvas::get_end_index_of_node_plus_decendants(size_t start_index, bool is_printing)
{
	// Calculate which vector to use
	TreeItemVec& vec = is_printing ? m_print_tree_items : m_tree_items;

	// Get end index
	size_t end_index = start_index;
	size_t indent = vec[start_index]->indent();
	for (size_t i = start_index+1; i < vec.size(); i++) {
		if (vec[i]->indent() <= indent) {
			return end_index;
		} else {
			end_index = i;
		}
	}
	return vec.size()-1;
}
void VTreeCanvas::CalculateTreeParameters(CTreeParameters& tp, bool printing)
{
	if (!printing) {
		tp.kOnePixel = pViewMetrics->onePixelAsLP(); // One pixel in logical units
		tp.kTopLineHeight = pViewMetrics->getLineHeight(0); 
		tp.kIconWidth = (tp.kTopLineHeight*4)/10;  //width of the minus or plus sign
		tp.kIconGap = tp.kIconWidth/4; //width of gap between icon and box
		tp.kIconHeight = tp.kIconWidth; //height of plus sign
		tp.kBoxWidth = tp.kIconWidth+2*tp.kIconGap; //width of the control box
		tp.kBoxHeight = tp.kBoxWidth; //try making height the same
		tp.kTextGap = 2*tp.kIconGap;  //gap between line from icon and text
		tp.kHeaderLineLength = tp.kIconWidth; //from right side of box to the gap between the line and the text
		tp.kIndentDistance = tp.kBoxWidth+tp.kHeaderLineLength+tp.kTextGap;
		tp.kHalfIndent = tp.kBoxWidth/2; //how much vertical line is indented
		tp.kVerticalIndent = tp.kHalfIndent; // Distance from left side of kIndentDistance to the vertical line on the tree
		tp.kLeftMargin = 4*tp.kOnePixel; //gap at the far left of everything
		tp.kMMScaleFactor = pViewMetrics->nMMScaleFactor(); // scale factor for y direction
		tp.kMinYDistanceBetweenItems = tp.kOnePixel; // Minimum Y Distance between items
		tp.kTreeLineWidth = tp.kOnePixel; // Width of the lines used to draw the tree
		tp.k1cmLP = pViewMetrics->ConvertMMtoLP(10);
	} else {
		tp.kOnePixel = m_pPrintMetrics->onePixelAsLP(); // One pixel in logical units
		//tp.kTopLineHeight = (int)(m_pPrintMetrics->latinLineHeightLP*m_yScaleFactor);
		//tp.kTopLineHeight = m_pPrintMetrics->latinLineHeightLP;
		tp.kTopLineHeight = m_pPrintMetrics->getLineHeight(0);
		tp.kIconWidth = (tp.kTopLineHeight*4)/10;  //width of the minus or plus sign
		tp.kIconGap = tp.kIconWidth/4; //width of gap between icon and box
		tp.kIconHeight = tp.kIconWidth; //height of plus sign
		tp.kBoxWidth = tp.kIconWidth+2*tp.kIconGap; //width of the control box
		tp.kBoxHeight = tp.kBoxWidth; //try making height the same
		tp.kTextGap = 2*tp.kIconGap;  //gap between line from icon and text
		tp.kHeaderLineLength = tp.kIconWidth; //from right side of box to the gap between the line and the text
		tp.kIndentDistance = tp.kBoxWidth+tp.kHeaderLineLength+tp.kTextGap;
		tp.kHalfIndent = tp.kBoxWidth/2; //how much vertical line is indented
		tp.kVerticalIndent = tp.kHalfIndent; // Distance from left side of kIndentDistance to the vertical line on the tree
		tp.kLeftMargin = 4*tp.kOnePixel; //gap at the far left of everything
		tp.kMMScaleFactor = m_pPrintMetrics->nMMScaleFactor(); // scale factor for y direction
		tp.kMinYDistanceBetweenItems = tp.kTopLineHeight/10; // Minimum Y Distance between items
		tp.kTreeLineWidth = m_pPrintMetrics->Convert10thofMMtoLP(4); // Width of the lines used to draw the tree
		tp.k1cmLP = m_pPrintMetrics->ConvertMMtoLP(10);
	}
}

void VTreeCanvas::ChangeMagnification(int magnification)
{
	if (magnification == m_nMagnification) {
		return;
	} else {
		wxClientDC dc(this);
		m_nMagnification = magnification;
		ASSERT_THROW(pViewMetrics != 0, "pViewMetrics was 0");
		pViewMetrics->Initialize(&dc);
		pViewMetrics->LoadFonts(m_nMagnification, &dc);
		pViewMetrics->RecalculateInterlinearLineHeight(&dc);
	}
	CalculateTreeParameters(m_tp, false);
	UpdateAllViews();
}

void VTreeCanvas::OnChangeInterlinearLine()
{
	wxClientDC dc(this);
	int targetLine;
	wxRect labelRect;
	wxCoord lpX, lpY;
	wxPoint point(0,2);
	this->CalcUnscrolledPosition(point.x, point.y, &lpX, &lpY);
	wxPoint pointLP(lpX, lpY);
	CTreeItem* tree_item = findTreeItemForPoint(pointLP, targetLine, labelRect);
	int deltaY = lpY - labelRect.GetY();
	pViewMetrics->LoadFonts(m_nMagnification, &dc);
	pViewMetrics->Initialize(&dc);
	pViewMetrics->RecalculateInterlinearLineHeight(&dc);
	CalculateTreeParameters(m_tp, false);
	UpdateAllViews();
	if (tree_item) {
		ScrollToGivenPosition(targetLine, deltaY);
	}
}


int VTreeCanvas::get_xStart_of_paragraph_from_indent(wxDC* pDC, int indent, int width_of_treeitem, bool bIsPrinting)
{
	(void)(pDC); // Silence a warning
	
	if (bIsPrinting) {
		// We are printing
		return width_of_treeitem 
			+ indent*m_tpPrintTP.kIndentDistance 
			+ m_tpPrintTP.kLeftMargin 
			+ m_tpPrintTP.kTextGap
			+ m_tpPrintTP.kHeaderLineLength;
		// + m_pointUpperLeft.x; // Plus margin: Don't add, because the display width is without margins.	  
		//+ m_pPrintMetrics->ConvertMMtoLP(10); // Plus 1 cm gap
	
	} else {
		// We are not printing
		return width_of_treeitem 
			+ indent*m_tp.kIndentDistance 
			+ m_tp.kLeftMargin 
			+ m_tp.kTextGap
			+ m_tp.kHeaderLineLength
			+ m_tp.k1cmLP; // Plus 1cm gap
	}
}

void VTreeCanvas::CalculateOverallHeight(void)
{
	int heightSoFar = 0;
	for(size_t i=0; i<m_tree_items.size(); i++) {
		heightSoFar += this->GetHeightOfItemInTree(i, false);
	}
	m_iOverallHeight = heightSoFar;
	if (m_iOverallHeight > 32767) {
		wxString strOverallHeight;
		strOverallHeight.Format(wxT("WARNING!!! m_iOverallHeight = %d > 32767!!!\n"), m_iOverallHeight);
		TRACE(strOverallHeight);
	}
}

void VTreeCanvas::DrawNodeToBitmap(id_d_t id_d, wxBitmap **ppBitmap)
{
	// Trace oid
	wxString strString;
	strString.Format(wxT("DrawNodeToBitmap: id_d = %d\n"), id_d);
	TRACE(strString);

	// Make compatible in-memory DC
	wxClientDC DC(this);
	wxMemoryDC dcMemory;

	bool bIsPrinting = false;

	// Calculate starting tree_item index
	int start_index = get_index_from_oid(id_d, bIsPrinting);

	// Calculate ending tree-item index
	int end_index = get_end_index_of_node_plus_decendants(start_index, bIsPrinting);


	// Calculate wxRect for bitmap
	wxRect rectBitmap;
	CalculateNodeRect(start_index, end_index, rectBitmap, bIsPrinting);

	// Create bitmap
	// std::cerr << "UP100: rect.w = " << rectBitmap.GetWidth() << ", rect.h = " << rectBitmap.GetHeight() << std::endl;
	*ppBitmap = new wxBitmap(rectBitmap.GetWidth(), rectBitmap.GetHeight(), -1);

	// Select bitmap into memoryDC
	dcMemory.SelectObject(*(*ppBitmap));

	// Draw white background
	dcMemory.SetBrush(*wxWHITE_BRUSH);
	dcMemory.SetPen(*wxWHITE_PEN);
	dcMemory.DrawRectangle(rectBitmap);

	// Draw node
	ViewMetrics *pVM = bIsPrinting ? m_pPrintMetrics : pViewMetrics;
	const CTreeParameters& tp = bIsPrinting ? m_tpPrintTP : m_tp;
	int y = 0;
	int yItemHeight = 0;
	int nMMScaleFactor = pVM->nMMScaleFactor();
	for(int i = start_index; i <= end_index; i++) {
		// Get item height
		yItemHeight = GetHeightOfItemInTree(i, bIsPrinting);

		// Draw item + lines
		DrawTreeItem(&dcMemory, i, y, pVM, tp, bIsPrinting);
		DrawLines(&dcMemory, i, y, pVM, tp, bIsPrinting);

		// Update y
		y += yItemHeight*nMMScaleFactor;
	}


	/*
	// Copy part of bitmap
	int indent = get_treeitem_indent(start_index, bIsPrinting);
	int startX = get_starting_x(indent, bIsPrinting);
	wxRect rectFinal(0,0, rectBitmap.Width() - startX, rectBitmap.Height());

	wxMemoryDC dcMemory2;
	long can_do_BITBLT = dcMemory2.GetDeviceCaps(RASTERCAPS) & RC_BITBLT;


	wxBitmap btmFinal;
	BOOL bFinalCreated = btmFinal.CreateCompatibleBitmap(&dcMemory2, rectFinal.Width(), rectFinal.Height());
	strString.Format("FinalCreated = %d\n", bFinalCreated);
	TRACE(strString);
	strString.Format("startX = %d, rectBitmap.Width() = %d\n", startX, rectBitmap.Width());
	TRACE(strString);
	wxBitmap *pOldBitmap2 = dcMemory2.SelectObject(&btmFinal);

	dcMemory2.FillRect(&rectFinal, &brWhiteBrush);
	dcMemory2.SetBkColor(RGB(255,255,255));
	dcMemory2.SetTextColor(RGB(0,0,0));
	dcMemory2.Rectangle(0,50,100,100);
	BOOL bBitBltHappened = dcMemory2.BitBlt(0,0, rectFinal.Width(), rectFinal.Height(), &dcMemory, startX, 0, SRCCOPY);
	strString.Format("BitBltHappened= %d\n", bBitBltHappened);
	TRACE(strString);
	dcMemory2.SelectObject(pOldBitmap2);

	*/

	// Deselect bitmap
	dcMemory.SelectObject(wxNullBitmap);

	//btmBitmap.SaveFile(wxT("test.xpm"), wxBITMAP_TYPE_XPM);
}



void VTreeCanvas::DrawNodeToClipboard(id_d_t id_d)
{
	wxBitmap *pBitmap;

	DrawNodeToBitmap(id_d, &pBitmap);

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

/////////////////////////////////////////////////////////////////////////////
// VTreeCanvas printing


bool VTreeCanvas::OnBeginPrinting(wxDC *pDC, VTreePrintout *pVTreePrintout)
{
	int xPPIPrinter, yPPIPrinter;
	pVTreePrintout->GetPPIPrinter(&xPPIPrinter, &yPPIPrinter);
	wxSize DCPPI = pDC->GetPPI();

	int nPhysicalWidth, nPhysicalHeight;
	int nDCWidth, nDCHeight;
	pVTreePrintout->GetPageSizePixels(&nPhysicalWidth, &nPhysicalHeight);
	pDC->GetSize(&nDCWidth, &nDCHeight);

	pDC->GetUserScale(&m_xScaleFactor, &m_yScaleFactor);

	wxSize topLeftMarginMM = pVTreePrintout->GetTopLeftMarginMM();
	wxSize bottomRightMarginMM = pVTreePrintout->GetBottomRightMarginMM();

	// Get pixels per mm
	double logUnitFactorX, logUnitFactorY;
	int leftMargin;
	int topMargin;

	int rightMargin;
	int bottomMargin;

	if (!pVTreePrintout->IsPreview()) {
		logUnitFactorX = (((double)xPPIPrinter)/25.4);
		logUnitFactorY = (((double)yPPIPrinter)/25.4);

		leftMargin = (int)(((double)topLeftMarginMM.GetWidth())*logUnitFactorX/m_xScaleFactor);
		topMargin = (int)(((double)topLeftMarginMM.GetHeight())*logUnitFactorY/m_yScaleFactor);

		rightMargin = (int)(((double)bottomRightMarginMM.GetWidth())*logUnitFactorX/m_xScaleFactor);
		bottomMargin = (int)(((double)bottomRightMarginMM.GetHeight())*logUnitFactorY/m_yScaleFactor);
	} else {
		logUnitFactorX = (((double)DCPPI.GetWidth())/25.4);
		logUnitFactorY = (((double)DCPPI.GetHeight())/25.4);

		leftMargin = (int)(((double)topLeftMarginMM.GetWidth())*logUnitFactorX);
		topMargin = (int)(((double)topLeftMarginMM.GetHeight())*logUnitFactorY);

		rightMargin = (int)(((double)bottomRightMarginMM.GetWidth())*logUnitFactorX);
		bottomMargin = (int)(((double)bottomRightMarginMM.GetHeight())*logUnitFactorY);
	}

	m_rectMargins = myRect(leftMargin, rightMargin, topMargin, bottomMargin);
  
	// Make print-metrics
	DoMakePrintMetrics(pDC, pVTreePrintout);

	// Make header font
	MakeHeaderFont(pDC);
  
	// Calculate print parameters such as width and height of page
	calculate_print_parameters(pDC, pVTreePrintout);

	// Update ViewMetrics with page width & height
	int nClientWidth, nClientHeight;
	nClientWidth = m_sizePrintArea.GetWidth();
	nClientHeight = m_sizePrintArea.GetHeight();

	m_pPrintMetrics->marginLP = m_rectMargins.left;

	m_pPrintMetrics->Update(pDC, nClientWidth, nClientHeight);

	// Initialize tree parameters
	CalculateTreeParameters(m_tpPrintTP, true);
  
	// Make the header string
	MakeHeaderString(pDC, pVTreePrintout);
  
	// Make print-tree
	DoMakePrintTree(pDC, pVTreePrintout);
  
	// Paginate
	DoPaginate(pDC, pVTreePrintout);
  
	// Set maximum page
	pVTreePrintout->SetMaxPage(m_print_paging_info.size());

	return TRUE;
}

void VTreeCanvas::OnPrint(wxDC* pDC, int pageNo)
{
	DoPrintHeader(pDC, pageNo);
	DoPrintPage(pDC, pageNo);
	DoPrintFooter(pDC, pageNo);
}


void VTreeCanvas::OnEndPrinting()
{
	// Delete print metrics
	delete m_pPrintMetrics;
	m_pPrintMetrics = 0;
  
	// Delete header font
	delete m_pFontHeaderFont;
	m_pFontHeaderFont = NULL;
  
	// Do away with tree
	empty_print_tree();
  
	// Do away with paging info
	empty_paging_info();
}

void VTreeCanvas::empty_paging_info(void)
{
	m_print_paging_info.resize(0);
}

void VTreeCanvas::DoPaginate(wxDC* pDC, VTreePrintout* pInfo)
{
	(void)(pDC); // Silence a warning
	(void)(pInfo); // Silence a warning

	// Make sure we start from scratch
	empty_paging_info();
    
	int item_index; // This must be signed, because it must be able to be -1
	CPagingInfo paging_info;
	int y;
    
	item_index = 0;
	y = 0;
	paging_info.SetStartOfPage(0,0);
    
	while (item_index < (int) m_print_tree_items.size()) {
		// Initialize.
		bool bHasMadeLastPageOfItem = false;
	
		// Get item
		CTreeItem* pItem = m_print_tree_items[item_index];
	
		// Calculate y plus height of item
		int NewY;
		NewY = y + GetHeightOfItemInTree(item_index, true);
		
		// Will this item not overflow the page?
		if (NewY < m_sizePrintArea.GetHeight()) {	
			// Yes, this item will not overflow this page.	
			// Go boldly once more around.
			y = NewY;
		} else {// page over-flow
			// No, this page will overflow.
			    
			// Find last line that will fit on page 
			// (note how LastLine lags one behind the line we are measuring)
			int LastLine = -1;
	
			// Is there a paragraph?
			int nNoOfLines = 0;
			if ((nNoOfLines = pItem->getNoOfLines()) != 0) {
				// Yes, there was a paragraph.	Now find the last line that will fit on the page
				LastLine = -1;
				NewY = y + pItem->GetHeightLP(0,0);
				
				while (NewY < m_sizePrintArea.GetHeight()) {
					LastLine++;
					NewY += pItem->GetHeightLP(LastLine+1, LastLine+1);
					if ((LastLine+1+1) == nNoOfLines) {
						break;
					}
				}
			}
			    
			// Make page-break if there was no room for the top line
			// and if this is not at the top of the page
			if (LastLine == -1) {
				CTreeItem* pPrevItem = m_print_tree_items[item_index-1];
				paging_info.SetEndOfPage(item_index - 1, 
							 pPrevItem->get_last_line_in_paragraph());
				m_print_paging_info.push_back(paging_info);
				y = 0;
			} else {
				// Otherwise, it means that we must terminate the page with LastLine
				paging_info.SetEndOfPage(item_index, LastLine);
				m_print_paging_info.push_back(paging_info);
				y = 0;
			}

		    
			// Place (rest of) item on new page.  
			// Start page.	Notice how this works even if LastLine is -1
			paging_info.SetStartOfPage(item_index, LastLine+1);
		    
			// Is this item not bigger than the page?
			if (pItem->height(LastLine+1, 0x7FFFFFFF) < m_sizePrintArea.GetHeight()) {
				// Yes, this is not bigger than the page.
				// Go boldly once more around
				y = 0 + pItem->height(LastLine+1, 0x7FFFFFFF);
			} else {
				// No, this is bigger than the page.
				int lineno = LastLine+1;
				int no_of_lines = pItem->getNoOfLines();

				if (no_of_lines == 0)
					return; // This means that the page is shorter than the box height.	 Can't print.
			
				// Process individual lines of the paragraph
				while (lineno < no_of_lines) {
					// Get lineheight
					int line_height = pItem->GetHeightLP(lineno,lineno);
					
					// Page overflow?
					if ((line_height + y) < m_sizePrintArea.GetHeight()) {
						// No, no page-overflow yet
						y += line_height;
					} else {
						// Yes, page overflow
					
						// Make page break
						paging_info.SetEndOfPage(item_index, lineno);
						m_print_paging_info.push_back(paging_info);
						y = 0;
					
						// Make start of new page, but only if we have not 
						// exhausted the number of lines.
						if (lineno + 1 < no_of_lines) {
							paging_info.SetStartOfPage(item_index, lineno+1);
						}
					
						// Have we emitted the last page of this item?
						if ((lineno + 1) >= no_of_lines) {
							bHasMadeLastPageOfItem = true;
						}
					}
					
					// One more line in the item
					lineno++;
				}
			}
		}

		// Increment item index
		item_index++;
		
		// If we have exhausted the vector, and we have not yet made 
		// the last page for this item, make the page.
		if (item_index >= (int) m_print_tree_items.size() && !bHasMadeLastPageOfItem) {
			// Put last item on page
			paging_info.LastItemOnPage = item_index - 1; // -1 because we have incremented it above.
			paging_info.LastLineInLastItem = pItem->get_last_line_in_paragraph();
			m_print_paging_info.push_back(paging_info);
			y = 0; // Not really necessary; only for good form...
		}
	}
}


void VTreeCanvas::calculate_print_parameters(wxDC* pDC, VTreePrintout *pPrintout)
{
	int nPhysicalWidth, nPhysicalHeight;
	int nDCWidth, nDCHeight;
	pPrintout->GetPageSizePixels(&nPhysicalWidth, &nPhysicalHeight);
	pDC->GetSize(&nDCWidth, &nDCHeight);

	pDC->GetUserScale(&m_xScaleFactor, &m_yScaleFactor);
	// Get paper size
	if (!pPrintout->IsPreview()) {
		m_sizePaper.SetWidth((int)(nPhysicalWidth/m_xScaleFactor));
		m_sizePaper.SetHeight((int)(nPhysicalHeight/m_yScaleFactor));
	} else {
		m_sizePaper.SetWidth((int)(nDCWidth/m_xScaleFactor));
		m_sizePaper.SetHeight((int)(nDCHeight/m_yScaleFactor));
	}
  
	// Get offset
	m_sizeOffset.SetWidth(0);
	m_sizeOffset.SetHeight(0);

	// Calculate space for header- and footer-material
	int header_material_height = m_cyHeaderFont +
		m_pPrintMetrics->Convert10thofMMtoLP(nHeaderHorizontalLineYSpace);

	// If margins are too small, increase them.
	m_rectMargins.top = std::max(m_rectMargins.top, header_material_height + m_sizeOffset.GetHeight());
	m_rectMargins.bottom = std::max(m_rectMargins.bottom, header_material_height + m_sizeOffset.GetHeight());
	m_rectMargins.left = std::max(m_rectMargins.left, m_sizeOffset.GetWidth());
	m_rectMargins.right = std::max(m_rectMargins.right, m_sizeOffset.GetWidth());

	// Get width of print-area
	m_pPrintMetrics->m_nViewWidthLP = 
		m_sizePaper.GetWidth() // paper-width
		- m_rectMargins.left  // minus left margin
		- m_rectMargins.right; // minus right margin
	//		- m_sizeOffset.GetWidth()*2; // minus offset
	m_sizePrintArea.SetWidth(m_pPrintMetrics->m_nViewWidthLP);
	// Get height of print-area
	m_sizePrintArea.SetHeight(m_sizePaper.GetHeight() // paper-height
				  - m_rectMargins.top // minus top margin
				  - m_rectMargins.bottom); // minus bottom margin

	//		- m_sizeOffset.GetHeight()*2; // minus offset

	// Get the point at the upper left-hand corner of the page
	m_pointUpperLeft.x = m_rectMargins.left; //- m_sizeOffset.GetWidth();
	m_pointUpperLeft.y = m_rectMargins.top;// - m_sizeOffset.GetHeight();

	// Get the point at the lower right-hand corner of the page
	//	m_pointLowerRight.x = m_sizePaper.GetWidth() - m_rectMargins.right - m_sizeOffset.GetWidth();
	//	m_pointLowerRight.y = m_sizePaper.GetHeight() - m_rectMargins.bottom - m_sizeOffset.GetHeight();
	m_pointLowerRight.x = m_pointUpperLeft.x + m_sizePrintArea.GetWidth();
	m_pointLowerRight.y = m_pointUpperLeft.y + m_sizePrintArea.GetHeight();
}

ViewMetrics *VTreeCanvas::CloneViewMetrics(ViewMetrics *pMetrics, wxMappingMode MapMode, bool bIsPrinting)
{
	ViewMetrics *pResult = new ViewMetrics(pMetrics, MapMode, bIsPrinting);
	return pResult;
}

void VTreeCanvas::DoMakePrintMetrics(wxDC* pDC, VTreePrintout* pInfo)
{
	(void)(pInfo); // Silence a warning

	delete m_pPrintMetrics;

	// Make a clone of pViewMetrics using pDC's map mode.
	m_pPrintMetrics = CloneViewMetrics(pViewMetrics, pDC->GetMapMode(), true);
    
	// Load fonts
	m_pPrintMetrics->LoadFonts(m_nMagnification, pDC);
    
	// Initialize
	m_pPrintMetrics->Initialize(pDC);

	// Recalculate interlinear height
	m_pPrintMetrics->RecalculateInterlinearLineHeight(pDC);
}


void VTreeCanvas::DoPrintHeader(wxDC* pDC, int pageNo)
{
	(void)(pageNo); // Silence a warning

	// Get height of space in which to draw the horizontal line
	int nLPLineYSpace = m_pPrintMetrics->Convert10thofMMtoLP(nHeaderHorizontalLineYSpace);
  
	// out header text
	int xText;
	int yText;
	xText = m_pointUpperLeft.x;
	yText = m_pPrintMetrics->nMMScaleFactor()*(m_pointUpperLeft.y - nLPLineYSpace - m_cyHeaderFont);
	pDC->SetFont(*m_pFontHeaderFont);
	pDC->DrawText(m_strHeader, xText, yText);
  
	// Make line
	int yLine;
	yLine = m_pPrintMetrics->nMMScaleFactor()*(m_pointUpperLeft.y - (nLPLineYSpace/2));
	PrintHeaderFooterLine(pDC, yLine);
}

void VTreeCanvas::DoPrintPage(wxDC* pDC, int nPageNo)
{
	// Get paging info
	CPagingInfo pi = m_print_paging_info[nPageNo-1];
  
	// Calculate lines in item
	int FirstLineInItem;
	int LastLineInItem;
	FirstLineInItem = pi.FirstLineInFirstItem;
	if (pi.FirstItemOnPage == pi.LastItemOnPage) {
		LastLineInItem = pi.LastLineInLastItem;
	} else{
		LastLineInItem = 0x7FFFFFFF;
	}
  
	int y = m_tpPrintTP.kMMScaleFactor*m_pointUpperLeft.y;
  
	// Print
	for (int item_index = pi.FirstItemOnPage; item_index <= pi.LastItemOnPage; item_index++) {
		// Get tree item
		CTreeItem* tree_item = m_print_tree_items[item_index];
    
		// out the tree-item itself
		DrawTreeItem(pDC, item_index, y, m_pPrintMetrics, m_tpPrintTP, true, FirstLineInItem, LastLineInItem);

		// out the lines
		DrawLines(pDC, item_index, y, m_pPrintMetrics, m_tpPrintTP, true, FirstLineInItem, LastLineInItem);
    
		// Update y
		y += tree_item->height(FirstLineInItem, LastLineInItem)*m_tpPrintTP.kMMScaleFactor;
		if (LastLineInItem >= tree_item->get_last_line_in_paragraph())
			y += m_tpPrintTP.kMinYDistanceBetweenItems*m_tpPrintTP.kMMScaleFactor;
    
		// Update FirstLineInItem and LastLineInItem
		if ((item_index + 1) <= pi.LastItemOnPage) {
			// FirstLineInItem defaults to 0
			FirstLineInItem = 0;
      
			// LastLineInItem could be either pi.LastItemOnPage or 0x7FFFFFFF (meaning, "all lines")
			if ((item_index + 1) == pi.LastItemOnPage) {
				LastLineInItem = pi.LastLineInLastItem;
			} else {
				LastLineInItem = 0x7FFFFFFF;
			}	
		}
	}
}

void VTreeCanvas::DoPrintFooter(wxDC* pDC, int nPageNo)
{
	// Get height of space in which to draw the horizontal line
	int nLPLineYSpace = m_pPrintMetrics->Convert10thofMMtoLP(nHeaderHorizontalLineYSpace);
  
	// out page number
	wxString strPageNumber;
	strPageNumber.Format(wxT("Page %d of %d"), nPageNo, m_print_paging_info.size());
	int xText;
	int yText;
	xText = m_pointLowerRight.x - strPageNumber.Length()*m_cxHeaderFont;
	yText = m_pPrintMetrics->nMMScaleFactor()*(m_pointLowerRight.y + nLPLineYSpace);
	pDC->SetFont(*m_pFontHeaderFont);
	pDC->DrawText(strPageNumber, xText, yText);
  
	// out reference
	wxString strReference;
	MakeFooterReference(strReference, nPageNo);
	xText = m_pointUpperLeft.x;
	pDC->DrawText(strReference, xText, yText);
  
  
	// Make line
	int yLine;
	yLine = m_pPrintMetrics->nMMScaleFactor()*(m_pointLowerRight.y + (nLPLineYSpace/2));
	PrintHeaderFooterLine(pDC, yLine);
}

void VTreeCanvas::DoMakePrintTree(wxDC *pDC, VTreePrintout* pPrintout)
{	
	(void)(pPrintout); // Silence a warning

	make_tree(m_print_tree_items, m_pPrintMetrics, pDC);
	CalculateLineTypes(m_print_tree_items, m_print_line_types);
}

void VTreeCanvas::empty_print_tree(void)
{
	for(unsigned int j=0; j < m_print_tree_items.size(); j++) {
		delete m_print_tree_items[j];
		m_print_tree_items[j] = 0;
	}
	m_print_tree_items.resize(0);
}


void VTreeCanvas::empty_line_types_vec()
{
	m_line_types.resize(0);
}

void VTreeCanvas::CalculateLineTypes(void)
{
	CalculateLineTypes(m_tree_items, m_line_types);
}

void VTreeCanvas::CalculateLineTypes(TreeItemVec& tree_items, LineTypeVec& line_types)
{
	// Make as many as vectors there are vectors in tree_items.
	size_t size = tree_items.size();
	line_types.resize(size);

	// Resize the vector to be the size of the indent plus 1
	// And fill each vector with ltNone
	for (size_t i = 0; i < size; i++) {
		int max_size = tree_items[i]->indent() + 2;
		line_types[i].resize(max_size);
		for (int j=0; j < max_size; j++) {
			line_types[i][j] = ltNone;
		}
	}

	// Fill the vectors
	for (size_t item_index = 0; item_index < size; item_index++) {
		size_t indent_size = line_types[item_index].size();
		for (size_t indent_index = 0; indent_index < indent_size; indent_index++) {
			line_types[item_index][indent_index] =
				GetLineType(item_index, indent_index, tree_items, line_types);
		}
	}
}

LineType VTreeCanvas::GetLineType(size_t item_index, size_t indent_index, TreeItemVec& tree_items, LineTypeVec& line_types)
{
	CTreeItem* pItem = tree_items[item_index];
	if (indent_index == pItem->indent() + 1) {
		if (ThereWillBeALineAfter(item_index, indent_index, tree_items))
			return ltLabelI;
		else
			return ltNone;
	} else if (indent_index == pItem->indent()) {
		if (pItem->expansionState() == kFixed) {	
			if (ThereWillBeALineAfter(item_index, indent_index, tree_items))
				return ltT;
			else
				return ltL;
		} else { // kExpanded or kCollapsed
			if (ThereWasALineBefore(item_index, indent_index, line_types)) {
				if (ThereWillBeALineAfter(item_index, indent_index, tree_items))
					return ltIconT;
				else
					return ltIconL;
			} else { // No line before
				return ltIcon;
			}
		}
	} else { // indent_index < pItem->indent()
		if (ThereWasALineBefore(item_index, indent_index, line_types)
		    && ThereWillBeALineAfter(item_index, indent_index, tree_items))
			return ltI;
		else
			return ltNone;
	}

	return ltNone;
}

bool VTreeCanvas::ThereWillBeALineAfter(size_t item_index, size_t indent_index, TreeItemVec& tree_items)
{
	size_t size = tree_items.size();
	size_t rootIndent = indent_index;
	for (size_t i = item_index+1; i < size && tree_items[i]->indent() >= rootIndent; i++) {
		if (tree_items[i]->indent() == rootIndent)
			return true;
	}
	return false;
}


bool VTreeCanvas::ThereWasALineBefore(size_t item_index, size_t indent_index, LineTypeVec& line_types)
{
	if (item_index > 0 && indent_index < line_types[item_index-1].size()) {
		switch (line_types[item_index-1][indent_index]) {
		case ltLabelI:
		case ltI:
		case ltT:
		case ltIconT:
			return true;
			break;
		default:
			return false;
			break;
		}
	} else { // position did not exist
		return false;
	}
}



void VTreeCanvas::MakeHeaderFont(wxDC* pDC)
{
	// Create font
	m_pFontHeaderFont = new wxFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false);

	//
	// Compute the width and the height of a character
	//
	wxString testString(wxT("x"));
	wxCoord nWidth;
	wxCoord nHeight;
	wxCoord nDescent;
	wxCoord nExternalLeading;
  
	pDC->SetFont(*m_pFontHeaderFont);
	pDC->GetTextExtent(wxT("x"), 
			   &nWidth, &nHeight, 
			   &nDescent, &nExternalLeading, 
			   NULL);
	m_cyHeaderFont = nHeight + nExternalLeading;
	m_cxHeaderFont = nWidth;
}


void VTreeCanvas::MakeHeaderString(wxDC* pDC, VTreePrintout *pVTreePrintout)
{
	(void)(pDC); // Silence a warning

	// Calculate max number of chars to show
	int max_chars = (m_pointLowerRight.x - m_pointUpperLeft.x) / m_cxHeaderFont;
	// Get fully qualified path name
	wxString result = pVTreePrintout->GetHeaderString();

	// See if we should remove chars
	int length = result.Length();
	if ((length > max_chars) && (length > 7)
	    && (max_chars > 7)) {
		// Replace the chars after "C:\" with "..."
		result.SetChar(3, wxT('.'));
		result.SetChar(4, wxT('.'));
		result.SetChar(5, wxT('.'));

		// Remove characters one by one until the string is short enough
		while (length > max_chars) {
			result = result.Left(6) + result.Right(length - 6 - 1);
			length = result.Length();
		}
	}

	// Set header
	m_strHeader = result;
}


void VTreeCanvas::PrintHeaderFooterLine(wxDC* pDC, int yLine)
{
	// Calculate width of line
	int nWidthOfLine = m_pPrintMetrics->Convert10thofMMtoLP(5);

	// Create pen
#if wxCHECK_VERSION(3,0,0)
	wxPen penLine(*wxBLACK, nWidthOfLine, wxPENSTYLE_SOLID);
#else
	wxPen penLine(*wxBLACK, nWidthOfLine, wxSOLID);
#endif

	// Draw line
	pDC->SetPen(penLine);
	pDC->DrawLine(m_pointUpperLeft.x, yLine, m_pointLowerRight.x, yLine);
	pDC->SetPen(wxNullPen);
}


void VTreeCanvas::MakeFooterReference(wxString& strReference, int nPageNo)
{
	strReference = wxString::Format(wxT("Page %d"), nPageNo);
}


void VTreeCanvas::InitializeViewMetrics()
{
	// Load fonts
	wxClientDC dc(this);
	pViewMetrics->LoadFonts(m_nMagnification, &dc);

	// Initialize
	pViewMetrics->Initialize(&dc);

	// Calculate interlinear height
	pViewMetrics->RecalculateInterlinearLineHeight(&dc);

	// Update ViewMetrics with client width & height
	/*
	// NOTE: This is done in UpdateViewMetricsViewWidth() as well,
	// which we call right afterwards, so this is a waste.
	int nWidth, nHeight;
	this->GetClientSize(&nWidth, &nHeight);
	wxScreenDC screenDC;
	pViewMetrics->Update(&screenDC, nWidth, nHeight);
	*/

	// Update ViewMetrics with view width
	UpdateViewMetricsViewWidth();
}

bool VTreeCanvas::UpdateViewMetricsViewWidth(void)
{
	if (pViewMetrics == 0) {
		return false;
	} else {
		bool result = false;
		int xNewValue;
		int xMinimum = pViewMetrics->ConvertMMtoLP(50);
		int xGapAtRight = pViewMetrics->xGapAtRightLP;

		// Get client rect
		int nClientWidth, nClientHeight;
		this->GetClientSize(&nClientWidth, &nClientHeight);

		// Convert client rect to LP
		wxClientDC dc(this);
		int nLogicalWidth = dc.DeviceToLogicalXRel(nClientWidth);

		// Calculate new view width for pViewMetrics.
		xNewValue = nLogicalWidth - xGapAtRight;
		if (xNewValue < xMinimum)
			xNewValue= xMinimum;

		// Should we update?
		if (pViewMetrics->m_nViewWidthLP != xNewValue) {
			// Yes, we should.
			pViewMetrics->m_nViewWidthLP = xNewValue;
			result = true;
		}
		pViewMetrics->Update(&dc, nClientWidth, nClientHeight);

		return result;
	}
}




void VTreeCanvas::ShowPopup(wxMenu *pMenuPopUp, wxPoint point)
{
	this->PopupMenu(pMenuPopUp, point);
}
