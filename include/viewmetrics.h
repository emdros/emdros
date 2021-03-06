// VwMtrcs.h : header file
//
// Originally from some code by Todd Hoatson
//
// Copyright assigned to Ulrik Petersen by Todd Hoatson's superior,
// Steve Echerd, in a letter dated November 11, 2003.
//
// Created: Sometime in March 2000
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


#ifndef VIEWMETRICS__H__
#define VIEWMETRICS__H__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "viewmetrics.cpp"
#endif


#include <wx/colour.h>
#include <wx/font.h>
#include <wx/string.h>
#include <wx/dc.h>

#include <algorithm>
#include <vector>
#include <map>
#include <stack>
#include <string>

#define PRIMARY_TEXT_RATIO(x) (x * 9/10)
#define MIN_WIDTH	            80         // client region x width

#define RED         wxColour(255,0,0)
#define LIGHT_GREEN wxColour(0,255,0)
#define BLUE        wxColour(0,0,255)

#define CYAN        wxColour(0,255,255)
#define MAGENTA     wxColour(255,0,255)
#define YELLOW      wxColour(255,255,0)

#define DARK_RED    wxColour(128,0,0)
#define DARK_GREEN  wxColour(0,128,0)
#define DARK_BLUE   wxColour(0,0,128)

#define TEAL        wxColour(0,128,128)
#define PLUM        wxColour(128,0,128)
#define GOLD        wxColour(128,128,0)

#define BLACK       wxColour(0,0,0)
#define WHITE       wxColour(255,255,255)
#define GRAY        wxColour(110,110,110)
#define DARK_GRAY   wxColour(80,80,80)


#if wxCHECK_VERSION(2, 9, 0)
// wxWidgets version being used is 2.9.0 or greater.
// Nothing to do,
#else
// wxWidgets version is less than 2.9.0, 
// so we must typedef wxMappingMode
typedef int wxMappingMode;
#endif


extern int CalculateLineHeightForFont(wxDC *pDC, wxFont *pFont);



class InterlinearLineMetrics {
 public:
	InterlinearLineMetrics() 
		: bDisplayed(true),
		pFont(NULL),
		strFontName(wxT("Arial")), // FIXME: WIll this only work on Windows?
		fontEncoding(wxFONTENCODING_ISO8859_1),
		fontFamily(wxFONTFAMILY_DEFAULT),
		m_color(*wxBLACK),
		m_pointSizeScaleFactor(1.0) {
	};
 InterlinearLineMetrics(wxString fontName, bool displayed, wxFontEncoding fontEnc = wxFONTENCODING_ISO8859_1, wxColour clr = *wxBLACK, wxFontFamily fontFam = wxFONTFAMILY_DEFAULT, double pointSizeScaleFactor = 1.0)
		: bDisplayed(displayed),
		pFont(NULL),
		strFontName(fontName),
		fontEncoding(fontEnc),
		fontFamily(fontFam),
		m_color(clr),
		m_pointSizeScaleFactor(pointSizeScaleFactor) { 
	};
	~InterlinearLineMetrics() { delete pFont; };
	bool bDisplayed;
	wxFont *pFont;
	wxString strFontName;
	wxFontEncoding fontEncoding;
	wxFontFamily fontFamily;
	wxColour m_color;
	double m_pointSizeScaleFactor;
	int lineHeightLP;
	int height() const { return (bDisplayed) ? lineHeightLP : 0; };
	void loadFont(wxDC *pDC, int pointSize) {
		delete pFont;
		pFont = new wxFont((int)(pointSize*m_pointSizeScaleFactor),
				   fontFamily,
				   wxFONTSTYLE_NORMAL,
				   wxFONTWEIGHT_NORMAL,
				   false,           // Underline? No.
				   strFontName,
				   fontEncoding);
		if (!pFont->Ok()) {
			delete pFont;
			pFont = new wxFont(pointSize,
					   wxFONTFAMILY_DEFAULT,
					   wxFONTSTYLE_NORMAL,
					   wxFONTWEIGHT_NORMAL,
					   false);           // Underline? No.
			
		}
		
		lineHeightLP = CalculateLineHeightForFont(pDC, pFont);
	};
};

typedef std::vector<InterlinearLineMetrics*> ILMVec;


class HTMLFontAttributes {
public:
	bool m_bBold;
	bool m_bItalic;
	bool m_bUnderline;
	bool m_bSuperscript;
	unsigned int m_sizeFactor; // In percent
	wxColour m_color;
	HTMLFontAttributes(bool bBold, bool bItalic, bool bUnderline, bool bSuperscript, unsigned int sizeFactor = 100, const wxColour& color = *wxBLACK);
	HTMLFontAttributes();
	HTMLFontAttributes(const HTMLFontAttributes& other);
	~HTMLFontAttributes();
	std::string toString() const;
	long toLong() const;
	static HTMLFontAttributes fromLong(long l);
	HTMLFontAttributes& operator=(const HTMLFontAttributes& other);
protected:
	void copy_to_self(const HTMLFontAttributes& other);
};

typedef std::stack<HTMLFontAttributes> FontAttributeStack;

#define LATIN_FONT = (-1)

class ViewMetrics {
 public:
	ViewMetrics(int nNoOfInterlinearLines, wxMappingMode map_mode, bool rightToLeft, bool isPrinting, int nMagnification);
	ViewMetrics(ViewMetrics *pOther, wxMappingMode map_mode, bool bIsPrinting); // Clone
	~ViewMetrics();
	void Initialize(wxDC *ppDC);
	void Update(wxDC *pDC, int clientWidth, int clientHeight);
	void RecalculateInterlinearLineHeight(wxDC *pDC);
	void LoadFonts(int magnification, wxDC *pDC);
	void ChangeMagnification(int magnification, wxDC *pDC);
	int nMMScaleFactor(void) const { return (m_nMap_mode == wxMM_TEXT) ? 1 : -1; };
	wxMappingMode get_map_mode(void) const { return m_nMap_mode; }; 
	int getNoOfILMs(void) const { return (int) m_ilmvec.size(); };
	int onePixelAsLP(void) const { return m_nOne_pixelLP; };
	int getSpaceBetweenWordsLP(void) const { return spaceBetweenWordsLP; };
	int ConvertMMtoLP(int mm) const;
	int Convert10thofMMtoLP(int mm) const;
	void SetViewWidthMM(int ViewWidthInMM);
	void setILM(unsigned int index, InterlinearLineMetrics *pILM);
	int getLineHeight(unsigned int index = 0);
	InterlinearLineMetrics *getILM(unsigned int index) { return m_ilmvec[index]; }; //FIXME: Throw an exception if out of range.
	wxFont *getFont(unsigned int index) { return m_ilmvec[index]->pFont; }; //FIXME: Throw an exception if out of range.
	wxFont *getLatinFont() { return m_latinFont; };
	int getLatinFontHeight() { return latinLineHeightLP; };

	int getTextWidth(wxDC *pDC, const wxString& strToBeMeasured, int nInterlinearLine);
	void clearStringSizeCache(void);

	HTMLFontAttributes startBold(void);
	HTMLFontAttributes startEm(void);
	HTMLFontAttributes startUnderline(void);
	HTMLFontAttributes startSuperscript(void);
	HTMLFontAttributes startH1(void);
	HTMLFontAttributes startH2(void);
	HTMLFontAttributes startAnchorHREF(void);
	HTMLFontAttributes startAnchorNAME(void);
	HTMLFontAttributes endTag(void);
	HTMLFontAttributes topFontAttributeStack(void);
	wxFont *getFontFromHTMLFontAttributes(const HTMLFontAttributes& font_attributes, bool bUseCacheMap);
 protected:
	void set_initial_defaults(void);
	wxMappingMode  m_nMap_mode;
	int	       m_nOne_pixelLP;
	ILMVec         m_ilmvec;
	wxFont         *m_latinFont;
	double         m_xUserScale, m_yUserScale;
 public:
	int	       m_nViewWidthLP; // width of view (not client) in logical units
	bool           bIsRightToLeft;
	bool           bIsPrinting;
	bool           bShowBrackets;
	int            xGapAtRightLP;
	int            marginLP;
	int            xParagraphIndentLP;
	int            interlinearLineHeightLP;
	int            latinLineHeightLP;
	int            spaceBetweenWordsLP;
	int            totalWidthLP;
	int            displayWidthLP;
	int            displayHeightLP;
	int            m_nMagnification;
 protected:
	FontAttributeStack m_HTML_font_attribute_stack;
	typedef std::map<long, wxFont*> Long2PFontMap;
	Long2PFontMap m_HTML_font_map;
	void startHTMLFontAttributeStack();
	typedef std::map<wxString, int> wxstr2size_map;
	wxstr2size_map m_stringSizeCache;
};


#endif

