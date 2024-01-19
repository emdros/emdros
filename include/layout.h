//
// layout.h
//
// Ulrik Petersen
//
// Based on code by Todd Hoatson and/or Fred Eichelman, 
// though heavily modified by Ulrik Petersen.
//
// The copyright for the original code was assigned to Ulrik Petersen
// by Todd Hoatson's superior, Steve Echerd, in a letter dated
// November 11, 2003.
//
// Created: 7/17-2005
// Last update: 1/19-2024
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




#ifndef DISPBOX__H__
#define DISPBOX__H__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "layout.cpp"
#endif



#include <wx/wx.h>
#include <wx/scrolwin.h>
#include <wx/string.h>
#include <wx/dc.h>
#include <wx/colour.h>
#include <wx/filesys.h>
#include <vector>

#include "emdros-lconfig.h"

#include <ostream>

#include "viewmetrics.h"

#include "qdxml.h"


typedef enum { BS_SMALLEST, BS_SMALL, BS_MEDIUM, BS_LARGE } Bracket_Size;
typedef enum { BD_LEFT, // stands at the left
	       BD_RIGHT   // stands at the right
} Bracket_Direction;

typedef enum { BT_BRACKET, BT_PARENTHESIS, BT_DOUBLE_BRACKET } Bracket_Type;

typedef enum { SK_NULL_ELEMENT } Symbol_Type;


#include <list>

enum PLACEMENT { 
	FRONT, // At the (logical) beginning of the line 
	BACK   // At the (logical) end of the line
};



class ViewMetrics; // Forward declaration

typedef std::vector<wxString> wxstringvec_t;

class LayoutItem {
 protected:
	ViewMetrics *m_pMetrics;
	int m_xExtentLP;
	int m_yExtentLP;
 public:
	LayoutItem(ViewMetrics *pViewMetrics) 
		: m_pMetrics(pViewMetrics), 
		m_xExtentLP(0), 
		m_yExtentLP(0) {};
	virtual ~LayoutItem() {};
	int GetWidthLP(void) const { return m_xExtentLP; };
	int GetHeightLP(void) const { return m_yExtentLP; };
	virtual void toHTML(std::ostream *pOut, const std::string& filename) {
		(void)(pOut); // Silence a warning
		(void)(filename); // Silence a warning
		/* By default, do nothing. */ };
	virtual void doToHTML(std::ostream *pOut, bool bIsRightToLeft, const std::string& filename) {
		(void)(pOut); // Silence a warning
		(void)(bIsRightToLeft); // Silence a warning
		(void)(filename); // Silence a warning
		/* By default, do nothing. */ };
};

typedef enum {
	kH1,
	kH2,
	kB,
	kEM,
	kU,
	kSUP,
	kBR,
	kA,
	kHR,
	kP,
	kHTMLText,
	kHTMLTagNone
} eHTMLTagKind;


class LayoutBoxBase : public LayoutItem {
 protected:
	bool m_bIsSelected;
	bool m_bSpaceAfter;
	bool m_bIsRightToLeft;
	long m_long;
 public:
	LayoutBoxBase(ViewMetrics *pMetrics, bool bSpaceAfter, bool bIsSelected, bool bIsRightToLeft);
	virtual ~LayoutBoxBase() {};
	virtual void Draw(wxDC *pDC, int xLP, int yLP) {
		(void)(pDC); // Silence a warning
		(void)(xLP); // Silence a warning
		(void)(yLP); // Silence a warning
		/* must be implemented. */ };
	bool IsSelected(void) const { return m_bIsSelected; };
	void SetSelected(bool selected) { m_bIsSelected = selected; };
	long GetLong(void) const { return m_long; };
	void SetLong(long l) { m_long = l; };
	bool GetSpaceAfter(void) const { return m_bSpaceAfter; };
	virtual void RecalculateExtents(wxDC *pDC) {
		(void)(pDC); // Silence a warning
		/* must be implemented. */ };
	virtual void DrawBox(wxDC *pDC, int xLeftLP, int yTopLP);

	virtual void doToHTML(std::ostream *pOut, bool bIsRightToLeft, const std::string& filename) {
		(void)(pOut);
		(void)(bIsRightToLeft);
		(void)(filename);
		/* Do nothing by default */ };
	virtual eHTMLTagKind getHTMLTagKind(void) const { return kHTMLTagNone; };
};

class AnchorBox; // Forward declaration

class HTMLBox : public LayoutBoxBase {
 protected:
	AnchorBox *m_pEnclosingHREFAnchor;
 public:
	HTMLBox(ViewMetrics *pMetrics, bool bSpaceAfter);
	virtual ~HTMLBox() {};
	void setEnclosingHREFAnchor(AnchorBox *pAHREF) { m_pEnclosingHREFAnchor = pAHREF; };
	AnchorBox *getEnclosingHREFAnchor(void) { return m_pEnclosingHREFAnchor; };
};

class ZeroWidthHTMLBox : public HTMLBox {
 protected:
	eHTMLTagKind m_kind;
	bool m_bStartTag;
 public:
	ZeroWidthHTMLBox(ViewMetrics *pMetrics, eHTMLTagKind kind, bool bStartTag);
	virtual ~ZeroWidthHTMLBox() {};
	virtual void doToHTML(std::ostream *pOut, bool bIsRightToLeft, const std::string& filename);
	eHTMLTagKind getKind() const { return m_kind; };
	bool isStartTag() const { return m_bStartTag; };
	virtual eHTMLTagKind getHTMLTagKind(void) const { return m_kind; };
};

typedef enum {
	kANAME,
	kAHREF,
	kAEnd
} eAnchorKind;

class AnchorBox : public ZeroWidthHTMLBox {
 protected:
	eAnchorKind m_anchor_kind;
	wxString m_attribute;
 public:
	AnchorBox(ViewMetrics *pMetrics, eAnchorKind kind, const wxString& attribute); // For start tag
	AnchorBox(ViewMetrics *pMetrics); // For end tag
	virtual ~AnchorBox() {};
	bool attributeIsThis(const wxString& comp) const {
		return m_attribute == comp;
	}
	eAnchorKind getAnchorKind() const { return m_anchor_kind; };
	wxString getAttribute() const { return m_attribute; };
	virtual void doToHTML(std::ostream *pOut, bool bIsRightToLeft, const std::string& filename);
};

class HTMLTextBox : public HTMLBox {
 protected:
	wxString m_text;
	HTMLFontAttributes m_font_attr;
	int m_nUnderlinePosition;
 public:
	HTMLTextBox(ViewMetrics *pMetrics, wxDC *pDC, const HTMLFontAttributes& font_attr, const wxString& text, bool bSpaceAfter);
	virtual ~HTMLTextBox();
	virtual void Draw(wxDC *pDC, int xLP, int yLP);
	virtual void doToHTML(std::ostream *pOut, bool bIsRightToLeft, const std::string& filename);
	virtual eHTMLTagKind getHTMLTagKind(void) const { return kHTMLText; };
	virtual void RecalculateExtents(wxDC *pDC);
};

class HTMLHRBox : public HTMLBox {
 public:
	HTMLHRBox(ViewMetrics *pMetrics);
	virtual ~HTMLHRBox();
	virtual void Draw(wxDC *pDC, int xLP, int yLP);
	virtual void doToHTML(std::ostream *pOut, bool bIsRightToLeft, const std::string& filename);
	virtual eHTMLTagKind getHTMLTagKind(void) const { return kHR; };
};



typedef std::vector<int> intvec_t;

class LayoutBox : public LayoutBoxBase {
 protected:
	bool m_bIsGrayed;
	bool m_bIsCentered;
	wxstringvec_t m_strings;
	intvec_t m_lengths;
 public:
	LayoutBox(wxDC *pDC, const wxstringvec_t& strings, ViewMetrics *pMetrics, bool bSpaceAfter, bool bIsRightToLeft, bool bIsCentered = false);
	virtual ~LayoutBox() {};
	virtual void Draw(wxDC *pDC, int xLP, int yLP);
	virtual void DrawText(wxDC *pDC, wxString strOutput, int xLP, int yLP, unsigned int ilmIndex);
	virtual void RecalculateExtents(wxDC *pDC);
	bool IsGrayed(void) const { return m_bIsGrayed; };
	void SetGrayed(bool grayed) { m_bIsGrayed = grayed; };
	virtual void doToHTML(std::ostream *pOut, bool bIsRightToLeft, const std::string& filename);
};

// One that has only 1 string-item,
// but which uses the first line of the viewmetrics items
// to get its font
class SimpleLayoutBox : public LayoutBox {
 public:
	SimpleLayoutBox(wxDC *pDC, const wxString& mystring, ViewMetrics *pMetrics, bool bSpaceAfter);
	virtual ~SimpleLayoutBox() {};
};

// One that has only 1 string-item
// but which uses ViewMetric's latin font.
class LatinLayoutBox : public SimpleLayoutBox {
 protected:
	wxColour m_color;
 public:  
	LatinLayoutBox(wxDC *pDC, const wxString& mystring, ViewMetrics *pMetrics, wxColour color, bool bSpaceAfter);
	~LatinLayoutBox();
	virtual void DrawText(wxDC *pDC, wxString strOutput, int xLP, int yLP, unsigned int ilmIndex);
	virtual void RecalculateExtents(wxDC *pDC);
	virtual void doToHTML(std::ostream *pOut, bool bIsRightToLeft, const std::string& filename);
};

class StackedLatinLayoutBox : public LayoutBoxBase {
 protected:
	bool m_bIsGrayed;
	bool m_bIsCentered;
	wxColour m_color;
	wxstringvec_t m_strings;
	intvec_t m_lengths;
 public:
	StackedLatinLayoutBox(wxDC *pDC, const wxstringvec_t& strings, ViewMetrics *pMetrics, wxColour color, bool bSpaceAfter, bool bIsCentered);
	virtual ~StackedLatinLayoutBox() {};
	virtual void Draw(wxDC *pDC, int xLP, int yLP);
	virtual void DrawText(wxDC *pDC, int ilmIndex, const wxString& strOutput, int xLP, int yLP);	
	virtual void RecalculateExtents(wxDC *pDC);
	bool IsGrayed(void) const { return m_bIsGrayed; };
	void SetGrayed(bool grayed) { m_bIsGrayed = grayed; };
	virtual void doToHTML(std::ostream *pOut, bool bIsRightToLeft, const std::string& filename);
};


class BorderedStackedLatinLayoutBox : public LayoutBoxBase {
 protected:
	bool m_bIsGrayed;
	bool m_bIsCentered;
	int m_nLineAfterWhichToAddSeparator;
	wxColour m_beforeSeparatorLinesColor;
	wxColour m_afterSeparatorLinesColor;
	wxstringvec_t m_strings;
	intvec_t m_lengths;
 public:
	BorderedStackedLatinLayoutBox(wxDC *pDC, const wxstringvec_t& strings, ViewMetrics *pMetrics, wxColour beforeSeparatorLinesColor, wxColour afterSeparatorLinesColor, bool bIsCentered, bool bSpaceAfter, int nLineAfterWhichToAddSeparator = -1); // For nLineAfterWhichToAddSeparator: If -1, no separator is added
	virtual ~BorderedStackedLatinLayoutBox() {};
	virtual void Draw(wxDC *pDC, int xLP, int yLP);
	virtual void DrawText(wxDC *pDC, const wxString& strOutput, int xLP, int yLP);	
	virtual void DrawBox(wxDC *pDC, int xLeftLP, int yTopLP);

	virtual void RecalculateExtents(wxDC *pDC);
	bool IsGrayed(void) const { return m_bIsGrayed; };
	void SetGrayed(bool grayed) { m_bIsGrayed = grayed; };
	virtual void doToHTML(std::ostream *pOut, bool bIsRightToLeft, const std::string& filename);
};



////////////////////////////////////////////////////////////
//
// LayoutSymbolBox
//
////////////////////////////////////////////////////////////
class LayoutSymbolBox : public LayoutBoxBase {
 protected:
        Symbol_Type m_kind;
        bool m_bIsGrayed;
 public:
	LayoutSymbolBox(wxDC *pDC, 
                        ViewMetrics *pVwMetrics, 
                        Symbol_Type st,
                        bool bSpaceAfter);
	virtual ~LayoutSymbolBox(void);
	virtual void Draw(wxDC *pDC, int x, int y);
	virtual void RecalculateExtents(wxDC *pDC);
	virtual void doToHTML(std::ostream *pOut, bool bIsRightToLeft, const std::string& filename);
        bool IsGrayed(void) const { return m_bIsGrayed; };
	void SetGrayed(bool grayed) { m_bIsGrayed = grayed; };
 protected:
        void calculate_parameters();
        int m_circle_diameter;
        int m_nLineWidth;
};

////////////////////////////////////////////////////////////
//
// LayoutBracketBox
//
////////////////////////////////////////////////////////////

class LayoutBracketBox : public LayoutBoxBase {
 private:
	Bracket_Size m_bracket_size;
	Bracket_Direction m_bracket_direction;
	Bracket_Type m_bracket_type;
	wxString m_label;
	bool m_has_arrow;
	bool m_has_label;
	bool m_bIsRightToLeft;
 public:
	LayoutBracketBox(wxDC *pDC, 
			 ViewMetrics *pVwMetrics, 
			 bool opening, bool is_inside, 
			 wxString label, 
			 Bracket_Size bs, 
			 Bracket_Type bt,
			 bool bIsRightToLeft,
			 bool bSpaceAfter);
	virtual ~LayoutBracketBox(void);
	virtual void Draw(wxDC *pDC, int x, int y);
	static bool should_have_label(bool opening, bool is_inside) { return !opening && !is_inside; };
	virtual void RecalculateExtents(wxDC *pDC);
	virtual void doToHTML(std::ostream *pOut, bool bIsRightToLeft, const std::string& filename);
 private:
	int m_nSerifLength;
	int m_nLabelXExtent;
	int m_nHeight;
	int m_nInnerGap;
	int m_nOuterGap;
	int m_nLineWidth;
	int m_nArrowLength;
	int m_nArrowLineWidth;
	void calculate_parameters(ViewMetrics *pVwMetrics);
};

////////////////////////////////////////////////////////////
//
// LayoutLine
//
////////////////////////////////////////////////////////////

typedef std::list<LayoutBoxBase*> PBoxList;




class LayoutLine : public LayoutItem { // LayoutItem is defined in DispBox.h
 protected:
	// Boxes in logical (not display) order
	PBoxList m_boxes;
	int m_indent;
	bool m_bIsRightToLeft;
	bool m_bIsCentered;
	bool m_bOwnsBoxes;
 public:
	LayoutLine(wxDC *pDC, ViewMetrics *pViewMetrics, int nNumIndents, bool bOwnsBoxes, bool bIsRightToLeft, bool bIsCentered);
	virtual ~LayoutLine();
	void AddBox(LayoutBoxBase *pBox, PLACEMENT placement);
	LayoutBoxBase *RemoveBox(PLACEMENT placement);
	void Draw(wxDC *pDC, int xLP, int yLP, bool bAlwaysFlushLeft);
	void RecalculateExtents(wxDC *pDC, bool bRecalculateBoxes = true);
	LayoutBoxBase *GetBoxForX(int targetX);
	bool HasNAMEAnchor(const wxString& name_attribute);
	virtual void toHTML(std::ostream *pOut, const std::string& filename);
 protected:
	void adjustMyExtents(LayoutBoxBase *pBox);
};


////////////////////////////////////////////////////////////
//
// LayoutParagraph
//
////////////////////////////////////////////////////////////

typedef std::vector<LayoutLine*> PLineList;


class LayoutParagraph : public LayoutItem {
protected:
	PLineList m_lines;
	PBoxList m_boxes;
	PBoxList m_owned_boxes;
	int m_xUsableWidthLP;
	bool m_bIsRightToLeft;
	bool m_bIsCentered;
	bool m_bVerticalSpaceAfter;
	int m_indent;
	int m_nVerticalSpaceAfterLP;
public:
	LayoutParagraph(ViewMetrics *pViewMetrics, int xUsableWidthLP, bool bIsRightToLeft, bool bIsCentered, int indent, bool add_vertical_space_after);
	virtual ~LayoutParagraph();
	// void AddBox(wxDC *pDC, LayoutBoxBase *pBox);
	void AddBox(wxDC *pDC, LayoutBoxBase *pBox, bool bAddToM_Boxes, bool bRecalculateExtents, bool bParagraphOwnsTheBox);
	void RecalculateExtents(wxDC *pDC, bool bRecalculateBoxes = true);
	void Draw(wxDC *pDC, int xLP, int yLP, int first_line, int last_line, bool bFlushLeftIfOneLine);
	void rewrap(wxDC *pDC, int xUsableWidthLP);
	int GetYForNAMEAnchor(const wxString& name_attribute); // Returns -1 if not found
	int GetHeightLP(int first_line, int last_line);
	int getNoOfLines(void) const { return (int) m_lines.size(); };
	LayoutLine *GetLineForY(int targetY);
	virtual void toHTML(std::ostream *pOut, const std::string& filename);
 protected:
	void delete_lines(void);
};




typedef std::list<LayoutParagraph*> PParagraphList;

class LayoutCanvas : public wxScrolledWindow {
 public:
	LayoutCanvas(wxWindow *pParent, const wxPoint& pos, const wxSize& size, const long style);
	virtual ~LayoutCanvas();
	virtual void OnDraw(wxDC &dc);
	void OnSize(wxSizeEvent& event);
	void OnMouseEvent(wxMouseEvent& event);

	DECLARE_EVENT_TABLE()
public:
	wxMappingMode m_nMapMode;
	int m_nMagnification;
	ViewMetrics *m_pViewMetrics;
 public:
	virtual void ChangeMagnification(int magnification);
	int GetMagnification(void) const { return m_nMagnification; };
	ViewMetrics *GetViewMetrics(void) { return m_pViewMetrics; };
	virtual void InitializeViewMetrics(); // Call this AFTER initializing it!
	bool UpdateViewMetricsViewWidth(void);
	void AddParagraph(LayoutParagraph *pParagraph);
	void AddParagraph(bool bIsRightToLeft, bool bIsCentered, int indent, bool bAdd_vertical_space_after);
	// void AddBox(LayoutBoxBase *pBox, bool bParagraphOwnsTheBox);
	void AddBox(wxDC *pDC, LayoutBoxBase *pBox, bool bRecalculateParagraphExtentsNow, bool bParagraphOwnsTheBox);
	// LayoutBoxBase *AddLayoutBox(const wxstringvec_t& strings, bool bIsSelected, bool bIsRightToLeft);
	// LayoutBoxBase *AddLayoutBox(wxDC *pDC, const wxstringvec_t& strings, bool bIsSelected, bool bIsRightToLeft, bool bRecalculateParagraphExtentsNow);
	int GetYForNAMEAnchor(const wxString& name_attribute); // Returns -1 if not found
	void RecalculateParagraphExtents();
	void AddLayoutBracketBox(bool opening, bool is_inside, 
				 wxString label, 
				 Bracket_Size bs, 
				 Bracket_Type bt,
				 bool bIsRightToLeft,
				 bool bIsSelected,
				 bool bRecalculateParagraphExtentsNow);
	LayoutBoxBase *GetBoxForPoint(wxPoint point);
	virtual void HandleButtonDown(wxMouseEvent& event, wxPoint point, bool RightButton) {
		(void)(event);
		(void)(point);
		(void)(RightButton);
	};
	
	// Returns true on HREF-anchor at point, and sets href_attribute if it is.
	bool CheckForHREFAnchorAtPoint(wxPoint point, wxString& href_attribute);
	void flush(void);
	void Clear(void);
	virtual void toHTML(std::ostream *pOut, const std::string& filename);
	void SetParagraphEnded(bool bEnded) { m_bParagraphEnded = bEnded; };
	wxDC *getDC(void) { return new wxClientDC(this); };
 protected:	
	virtual void printDTDParagraphEntry(std::ostream *pOut, std::string name, wxString strFontName, wxColour color, int pointsize);
	virtual void printHTMLHeader(std::ostream *pOut);
	virtual void printHTMLFooter(std::ostream *pOut);
	void DrawItem(wxDC *pDC, LayoutParagraph *pPara, int y);
	void CalculateOverallHeight();
	void GetClipBox(wxDC *pDC, wxRect& clipRect);
	void rewrap_paragraphs(wxDC *pDC);
	PParagraphList m_paragraphs;
	double m_xScaleFactor, m_yScaleFactor;
	int m_iOverallHeight;  //height of all current paragraph items
	bool m_bParagraphEnded;
};



class HTMLCanvas; // Forward declaration

class QDHTMLHandler : public QDDocHandler {
	HTMLCanvas *m_pCanvas;
	ViewMetrics *m_pMetrics;
	wxDC *m_pDC;
	std::string m_chars;
	HTMLFontAttributes m_font_attributes;
	bool m_bInBody;
	bool m_bInParagraph;
	typedef std::stack<AnchorBox*> AnchorBoxStack;
	AnchorBoxStack m_href_anchor_stack;
 public:
	QDHTMLHandler(HTMLCanvas *pCanvas);
	virtual ~QDHTMLHandler();
	virtual void startElement(const std::string& tag, const AttributeMap& attrs);
	virtual void endElement(const std::string& tag);
	virtual void startDocument(void);
	virtual void endDocument(void);
	virtual void text(const std::string& str);
 protected:
	void handleChars(void);
	void addText(const std::string& str, bool bSpaceAfter);
};

class URLHistoryItem {
 protected:
	wxString m_url;
	int m_y; // In Scroll Units
 public:
	URLHistoryItem(const wxString& url, int y = 0) { m_url = url; m_y = y; };
	~URLHistoryItem() {};
	int GetY(void) const { return m_y; };
	void SetY(int y) { m_y = y; };
	const wxString& getURL(void) const { return m_url; };
};




class HTMLCanvas : public LayoutCanvas {
 protected:
	typedef std::vector<URLHistoryItem> URLHistory;
	URLHistory m_history;
	URLHistory::size_type m_history_position;
	wxString m_opened_page_without_anchor;
	wxFileSystem *m_pFS;
 public:
	HTMLCanvas(wxWindow *pParent, const wxPoint& pos, const wxSize& size, const long style, ViewMetrics *pMetrics);
	virtual ~HTMLCanvas();

	virtual void HandleButtonDown(wxMouseEvent& event, wxPoint point, bool RightButton);

	// Returns true on no error, false on error, in which case error_message has the error message.
	bool SetPage(const std::string& strHTML, /* out */ std::string& error_message);
	bool LoadPage(const wxString& url, /* out */ std::string& error_message);
	void SetRelatedFrame(wxFrame *pFrame, const wxString& title);
	void SelectAll(void);
	bool HistoryCanBack(void);
	bool HistoryCanForward(void);
	void HistoryBack(void);
	void HistoryForward(void);
	wxString GetOpenedPage(void);
 protected:
	virtual bool LoadPageFromFSFile(wxFSFile *pFile, std::string& error_message);
	void SetLocation(const wxString& url);
	bool ScrollToAnchor(const wxString& anchor, /* out */ std::string& error_message);
	void AddToHistory(const wxString& url);
};


extern int getPenWidth(int nMagnification);



#endif
