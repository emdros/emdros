// VTreeCanvas : header file
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



#ifndef VTREECANVAS__H__
#define VTREECANVAS__H__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "vtreecanvas.cpp"
#endif


#include <wx/wx.h>
#include <wx/scrolwin.h>
#include <wx/print.h>
#include "treeitem.h"
#include "viewmetrics.h"

class CTreeParameters {
 public:
	int kTopLineHeight; // height of the top line in a BART box
	int kIconWidth; //width of the minus or plus sign
	int kIconGap; //width of gap between icon and box
	int kIconHeight; //height of plus sign
	int kBoxWidth; //width of the control box
	int kBoxHeight; //try making height the same
	int kTextGap;  //from line to label
	int kHeaderLineLength; //length of line from box to text
	int kIndentDistance;
	int kHalfIndent; //how much vertical line is indented
	int kLeftMargin; //gap at the far left of everything
	int kMMScaleFactor; // scale factor for y direction
	int kOnePixel; // One pixel in logical units
	int kVerticalIndent; // Distance from left side of kIndentDistance to the vertical line on the tree
	int kMinYDistanceBetweenItems;
	int kTreeLineWidth; // Width of the lines used to draw the tree
	int k1cmLP;
};


class CPagingInfo {
 public:
	CPagingInfo(int fiop = 0, int flifi = 0, int liop = 0, int llili = 0) 
		: FirstItemOnPage(fiop), 
		LastItemOnPage(liop), 
		FirstLineInFirstItem(flifi), 
		LastLineInLastItem(llili) {};
		void SetStartOfPage(int fiop, int flifi) { FirstItemOnPage = fiop; FirstLineInFirstItem = flifi; };
		void SetEndOfPage(int liop, int llili) { LastItemOnPage = liop; LastLineInLastItem = llili; };
		int FirstItemOnPage;
		int LastItemOnPage;
		int FirstLineInFirstItem;
		int LastLineInLastItem;
};

typedef enum {
	ltNone,
	ltLabelI,
	ltI,
	ltL,
	ltT,
	ltIcon,
	ltIconL,
	ltIconT
} LineType;

typedef std::vector<std::vector<LineType> > LineTypeVec;

class myRect {
 public:
	int left, right, top, bottom;
	myRect() : left(0), right(0), top(0), bottom(0) {};
		myRect(int l, int r, int t, int b) : left(l), right(r), top(t), bottom(b) {};
			~myRect() {};
};


class VTreeCanvas; // Forward declaration


class VTreePrintout : public wxPrintout
{
 private:
	int m_nMaxPage;
	wxString m_strHeaderString;
 public:
	wxPageSetupDialogData *m_pPageSetupDlgData;
	VTreePrintout(wxString HeaderString, const wxString& title, wxPageSetupDialogData *pPageSetupDlgData);
	virtual bool OnPrintPage(int page);
	virtual bool HasPage(int page);
	virtual bool OnBeginDocument(int startPage, int endPage);
	virtual void OnBeginPrinting();
	virtual void OnEndPrinting();
	wxSize GetTopLeftMarginMM();
	wxSize GetBottomRightMarginMM();
	wxString GetHeaderString() { return m_strHeaderString; };
	virtual void GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo);
	void SetMaxPage(int nMaxPage) { m_nMaxPage = nMaxPage; };
	virtual VTreeCanvas *GetVTreeCanvas() = 0; // Must be implemented
 private:
	void SetScaleFactor();
};

class VTreeCanvas : public wxScrolledWindow {
public:
	VTreeCanvas(wxWindow *pParent, const wxPoint& pos, const wxSize& size, const long style);
	virtual void OnDraw(wxDC &dc);
	void OnMouseEvent(wxMouseEvent& event);
	void OnSize(wxSizeEvent& event);
   
	DECLARE_EVENT_TABLE()
public:
	wxMappingMode m_nMapMode;
	int m_nMagnification;
	ViewMetrics *pViewMetrics;
public:
#if wxCHECK_VERSION(3,0,0)
	void DoPrepareDC(wxDC& dc);
#else
	void PrepareDC(wxDC& dc);
#endif
	void DoMakePrintMetrics(wxDC* pDC, VTreePrintout* pInfo);
	void DoMakePrintTree(wxDC *pDC, VTreePrintout* pInfo);
	void calculate_print_parameters(wxDC* pDC, VTreePrintout* pInfo);
	virtual void OnChangeInterlinearLine();
	virtual void OnUpdate();
	void CalculateScreenTreeParameters() { CalculateTreeParameters(m_tp, false); };
	virtual void make_tree(TreeItemVec &tree_items, ViewMetrics *pViewMetrics, wxDC *pDC) {
		(void)(tree_items); // Silence a warning
		(void)(pViewMetrics); // Silence a warning
		(void)(pDC); // Silence a warning
	}; // MUST BE IMPLEMENTED!
	virtual void UpdateAllViews() {};  // Must be implemented.
	CTreeItem *getTreeItem(size_t index) { return m_tree_items[index]; };
	size_t getTreeItemVecSize() const { return m_tree_items.size(); };
	int get_xStart_of_paragraph_from_indent(wxDC* pDC, int indent, int width_of_treeitem, bool bIsPrinting);
	int get_index_from_oid(id_d_t oid, bool is_printing = false);
	void ScrollToGivenPosition(int index, int deltaY = 0);
	virtual void ChangeMagnification(int magnification);
	int GetMagnification(void) const { return m_nMagnification; };
	ViewMetrics *GetViewMetrics(void) { return pViewMetrics; };
	virtual void InitializeViewMetrics(); // Call this AFTER initializing it!
	bool UpdateViewMetricsViewWidth(void);
	virtual void HandleButtonDown(wxMouseEvent& event, wxPoint point, bool RightButton);
	virtual void HandleRightButton(wxMouseEvent& event, CTreeItem* tree_item, wxPoint point) {
		(void)(event); // Silence a warning
		(void)(tree_item); // Silence a warning
		(void)(point); // Silence a warning
	}; // Must be implemented if you wisht the mouse to respond
	virtual void HandleLeftButton(wxMouseEvent& event, CTreeItem* tree_item, int index) {
		(void)(event); // Silence a warning
		(void)(tree_item); // Silence a warning
		(void)(index); // Silence a warning
	}; // Must be implemented if you wisht the mouse to respond
	void EraseBoxHighlight(void);
	void CalculateLineTypes(void);
	void DrawNodeToClipboard(id_d_t id_d);
	void DrawNodeToBitmap(id_d_t id_d, /* out */ wxBitmap **ppBitmap); // ppBitmap must point to an unitialized pointer. It must be freed after use with delete.
	void ShowPopup(wxMenu *menuPopUp, wxPoint point);
 public:
	/*
	  virtual void OnInitialUpdate();     // first time after construct
	  virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	*/
	//virtual bool OnPreparePrinting(wxPrintData* pInfo);
	bool OnBeginPrinting(wxDC *pDC, VTreePrintout *pVTreePrintout);
	//virtual void OnBeginPrinting(wxDC* pDC, wxPrintDialogData* pInfo);
	virtual void OnEndPrinting();
	virtual void OnPrint(wxDC* pDC, int pageNo);
	void FindSelectionRange(size_t current_indent, size_t index, int& start, int& end);
	//}}AFX_VIRTUAL

	// Implementation
 protected:
	void empty_line_types_vec();

	virtual ViewMetrics *CloneViewMetrics(ViewMetrics *pMetrics, wxMappingMode MapMode, bool bIsPrinting);
	
	virtual ~VTreeCanvas();
	/*
	  virtual void WriteRegistrySettings(void) const;
	  virtual void ReadRegistrySettings(void);
	*/
	void drawTree(wxDC* pDC, CTreeItem* treeitem, int heightSoFar, int height, int startIndex);
	void adjustHeights(wxDC* pDC);
	void CalculateTreeParameters(CTreeParameters& tp, bool printing);
	void CalculateNodeRect(size_t start_index, size_t end_index, wxRect &rectNode, bool is_printing);
	bool FindTreeItemByID_D(id_d_t id_d, int &tree_item, bool is_printing = false);
	int lineClicked(wxPoint point, bool& before, wxRect& invertRect);
	CTreeItem* findTreeItemForPoint(wxPoint point, int& targetLine, wxRect& labelRect);
	void toggleExpansionState(CTreeItem* tree_item);
	virtual void expand(id_d_t id) {
		(void)(id); // Silence a warning
	}; // Must be implemented in descendant if you wish to honor clicks on + and -
	virtual void collapse(id_d_t id) {
		(void)(id); // Silence a warning
	};  // Must be implemented in descendant if you wish to honor clicks on + and -
	int FindYofTreeItem(size_t index);
	int GetHeightOfItemInTree(size_t index, bool printing, int FirstLineInItem = 0, int LastLineInItem = 0x7FFFFFFF);
public:
	int GetHeightOfItem(size_t index, bool printing, int FirstLineInItem = 0, int LastLineInItem = 0x7FFFFFFF);
protected:
	int get_starting_x(size_t indent_index, bool printing);
	int get_treeitem_indent(size_t index, bool is_printing);
	size_t get_end_index_of_node_plus_decendants(size_t start_index, bool is_printing);
	void DrawLines(wxDC* pDC, size_t item_index, int y, ViewMetrics* pMetrics, const CTreeParameters& tp, bool bIsPrinting, int FirstLineInItem = 0, int LastLineInItem = 0x7FFFFFFF);
	void DrawTreeItem(wxDC* pDC, size_t item_index, int y, ViewMetrics* pMetrics, const CTreeParameters& tp, bool bIsPrinting, int FirstLineInItem = 0, int LastLineInItem = 0x7FFFFFFF);
	void DrawLine(wxDC* pDC, size_t item_index, size_t indent_index, int y, ViewMetrics* pMetrics, const CTreeParameters& tp, bool bIsPrinting, int FirstLineInItem = 0, int LastLineInItem = 0x7FFFFFFF);
	void DrawIcon(wxDC* pDC, size_t item_index, size_t indent_index, int y, ViewMetrics* pMetrics, const CTreeParameters& tp, bool bIsPrinting, int FirstLineInItem = 0, int LastLineInItem = 0x7FFFFFFF);
	void CalculateOverallHeight(void);
	void CalculateLineTypes(TreeItemVec& tree_items, LineTypeVec& line_types);
	LineType GetLineType(size_t item_index, size_t indent_index, TreeItemVec& tree_items, LineTypeVec& line_types);
	bool ThereWasALineBefore(size_t item_index, size_t indent_index, LineTypeVec& line_types);
	bool ThereWillBeALineAfter(size_t item_index, size_t indent_index, TreeItemVec& tree_items);
	TreeItemVec m_tree_items;
	LineTypeVec m_line_types;
	int m_labelWidth; //width of label area
	//TODO: figure how to compute the necessary width.

	int m_iOverallHeight;  //height of all current tree items last computed by OnSize()

	// Printing-specific stuff
	ViewMetrics *m_pPrintMetrics;
	TreeItemVec m_print_tree_items;
	LineTypeVec m_print_line_types;
	myRect m_rectMargins; // in thousands of inches
	wxSize m_sizePrintArea; // in logical units
	short m_dmOrientation; // Orientation as in CPageSetupDialog 
	short m_dmPaperSize; // Paper size as in CPageSetupDialog 
	wxSize m_sizePaper; // Paper size in logical units
	wxSize m_sizeOffset; // Unprintable sizes on sides of page
	wxPoint m_pointUpperLeft; // the point at the upper left-hand corner of the page
	wxPoint m_pointLowerRight; // the point at the lower right-hand corner of the page
	std::vector<CPagingInfo> m_print_paging_info;
	CTreeParameters m_tpPrintTP;
	wxFont *m_pFontHeaderFont;
	int m_cxHeaderFont;
	int m_cyHeaderFont;
	wxString m_strHeader;
	double m_xScaleFactor, m_yScaleFactor;

	void empty_paging_info(void);
	void empty_print_tree(void);
	void DoPaginate(wxDC* pDC, VTreePrintout* pInfo);
	void DoPrintHeader(wxDC* pDC, int pageNo);
	void DoPrintPage(wxDC* pDC, int pageNo);
	void DoPrintFooter(wxDC* pDC, int pageNo);
	void MakeHeaderFont(wxDC* pDC);
	void MakeHeaderString(wxDC* pDC, VTreePrintout *pVTreePrintout);
	void MakeFooterReference(wxString& strReference, int nPageNo);
	void PrintHeaderFooterLine(wxDC* pDC, int yLine);

	enum { nHeaderHorizontalLineYSpace = 40 }; // In 10th of a mm

	// Generated message map functions
	//{{AFX_MSG(CInterlinearTreeView)
	/*
	  void OnRButtonDown(size_t nFlags, wxPoint point);
	  void OnLButtonDown(unsigned int nFlags, wxPoint point);
	*/
	void GetClipBox(wxDC *pDC, wxRect& clipRect);
 private:
	CTreeParameters m_tp;

};




#endif

