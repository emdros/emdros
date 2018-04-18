//
//
// VwMtrcs.cpp : implementation file
//
// Originally from some code by Todd Hoatson
//
// Copyright assigned to Ulrik Petersen by Todd Hoatson's superior,
// Steve Echerd, in a letter dated November 11, 2003.
//
// Created: Sometime in March 2000
// Last update: 4/7-2018
//
//
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated t
 *   Copyright (C) 2003-2018  Ulrik Sandborg-Petersen
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
#pragma implementation "viewmetrics.h"
#endif

#include <viewmetrics.h>

#include <string_func.h>


#include <iostream>


#define DEFAULT_FONT_SIZE 12



//////////////////////////////////////////////////////////////////
//
// HTMLFontAttributes
//
//////////////////////////////////////////////////////////////////

HTMLFontAttributes::HTMLFontAttributes()
	: m_bBold(false),
	  m_bItalic(false),
	  m_bUnderline(false),
	  m_bSuperscript(false),
	  m_sizeFactor(100),
	  m_color(*wxBLACK)
{
}

HTMLFontAttributes::HTMLFontAttributes(bool bBold, bool bItalic, bool bUnderline, bool bSuperscript, unsigned int sizeFactor, const wxColour& color)
	: m_bBold(bBold),
	  m_bItalic(bItalic),
	  m_bUnderline(bUnderline),
	  m_bSuperscript(bSuperscript),
	  m_sizeFactor(sizeFactor),
	  m_color(color)
{
}


HTMLFontAttributes::HTMLFontAttributes(const HTMLFontAttributes& other)
{
	copy_to_self(other);
}


HTMLFontAttributes::~HTMLFontAttributes()
{
}


std::string HTMLFontAttributes::toString() const
{
	std::string result;
	if (m_bBold) {
		result += 'B';
	} else {
		result += '-';
	}

	if (m_bItalic) {
		result += 'I';
	} else {
		result += '-';
	}

	/*
	// This is not reflected in the fonts in String2PFontTrie,
	// so it should also be left out here...
	if (m_bUnderline) {
		result += 'U';
	} else {
		result += '-';
	}
	*/

	if (m_bSuperscript) {
		result += 'S';
	} else {
		result += '-';
	}

	result += long2string(m_sizeFactor);

	return result;
}


long HTMLFontAttributes::toLong() const
{
	long result = m_sizeFactor;

	result *= 2;
	if (m_bBold) {
		result += 1;
	}

	result *= 2;
	if (m_bItalic) {
		result += 1;
	}

	/*
	// This is not reflected in the fonts in String2PFontTrie,
	// so it should also be left out here...
	result *= 2;
	if (m_bUnderline) {
		result += 1;
	}
	*/

	result *= 2;
	if (m_bSuperscript) {
		result += 1;
	}

	return result;
}


HTMLFontAttributes HTMLFontAttributes::fromLong(long l)
{
	unsigned int sizeFactor = 0;
	bool bBold;
	bool bItalic;
	bool bUnderline = false;
	bool bSuperscript;

	bSuperscript = l & 0x01L;
	l = l >> 1;

	/*
	// This is not reflected in the fonts in String2PFontTrie,
	// so it should also be left out here...
	bUnderline = l & 0x01L;
	l = l >> 1;
	*/

	bItalic = l & 0x01L;
	l = l >> 1;

	bBold = l & 0x01L;
	l = l >> 1;

	sizeFactor = l;

	return HTMLFontAttributes(bBold, bItalic, bUnderline,
				  bSuperscript, sizeFactor, *wxBLACK);
}


HTMLFontAttributes& HTMLFontAttributes::operator=(const HTMLFontAttributes& other)
{
	copy_to_self(other);
	return *this;
}


void HTMLFontAttributes::copy_to_self(const HTMLFontAttributes& other)
{
	m_bBold = other.m_bBold;
	m_bItalic = other.m_bItalic;
	m_bUnderline = other.m_bUnderline;
	m_bSuperscript = other.m_bSuperscript;
	m_sizeFactor = other.m_sizeFactor;
	m_color = other.m_color;
}





ViewMetrics::ViewMetrics(int nNoOfInterlinearLines, wxMappingMode mapmode, bool rightToLeft, bool isPrinting, int nMagnification)
	: m_nMap_mode(mapmode),
	  m_nOne_pixelLP(0),
	  m_latinFont(0),
	  m_nViewWidthLP(0),
	  bIsRightToLeft(rightToLeft),
	  bIsPrinting(isPrinting),
	  bShowBrackets(true),
	  xGapAtRightLP(0),
	  marginLP(0),
	  interlinearLineHeightLP(0),
	  latinLineHeightLP(0),
	  spaceBetweenWordsLP(0),
	  displayWidthLP(0),
	  displayHeightLP(0),
	  m_nMagnification(nMagnification),
	  m_stringSizeCache()
{
	m_ilmvec.resize(nNoOfInterlinearLines);

	for (unsigned int i = 0; i < m_ilmvec.size(); ++i) {
		m_ilmvec[i] = new InterlinearLineMetrics();
	}

	set_initial_defaults();

	startHTMLFontAttributeStack();
}

// Clone
ViewMetrics::ViewMetrics(ViewMetrics *pOther, wxMappingMode map_mode, bool bIsPrinting)
	: m_nMap_mode(map_mode),
	  m_nOne_pixelLP(0),
	  m_latinFont(new wxFont(*pOther->m_latinFont)),
	  m_nViewWidthLP(0),
	  bIsRightToLeft(pOther->bIsRightToLeft),
	  bIsPrinting(bIsPrinting),
	  bShowBrackets(pOther->bShowBrackets),
	  xGapAtRightLP(0),
	  marginLP(0),
	  interlinearLineHeightLP(0),
	  latinLineHeightLP(pOther->latinLineHeightLP),
	  spaceBetweenWordsLP(0),
	  displayWidthLP(0),
	  displayHeightLP(0),
	  m_nMagnification(pOther->m_nMagnification),
	  m_stringSizeCache()
{
	m_ilmvec.resize(pOther->getNoOfILMs());
  
	for (unsigned int i = 0; i < m_ilmvec.size(); ++i) {
		InterlinearLineMetrics *pOtherILM = pOther->getILM(i);
		m_ilmvec[i] = new InterlinearLineMetrics(pOtherILM->strFontName,
							 pOtherILM->bDisplayed,
							 pOtherILM->fontEncoding,
							 pOtherILM->m_color);
	}
  
	set_initial_defaults();

	startHTMLFontAttributeStack();
}


ViewMetrics::~ViewMetrics()
{
	for (unsigned int i = 0; i < m_ilmvec.size(); ++i) {
		delete m_ilmvec[i];
	}
	delete m_latinFont;
}


void ViewMetrics::set_initial_defaults(void)
{
	// Set some sensible default
	m_nViewWidthLP = ConvertMMtoLP(120);

	// Set xGapAtRightLP
	xGapAtRightLP = ConvertMMtoLP(1);
}


void ViewMetrics::startHTMLFontAttributeStack(void)
{
	// First clear it...
	while (!m_HTML_font_attribute_stack.empty()) {
		m_HTML_font_attribute_stack.pop();
	}
	
	// Then push back a normal, non-decorated, black HTMLFontAttributes object.
	m_HTML_font_attribute_stack.push(HTMLFontAttributes(false, false, false, false, 100, *wxBLACK));
}

void ViewMetrics::ChangeMagnification(int magnification, wxDC *pDC)
{
	LoadFonts(magnification, pDC); // Sets m_nMagnification
	Initialize(pDC); // Must come AFTER LoadFonts, since it uses some fonts!
	RecalculateInterlinearLineHeight(pDC);
}

void ViewMetrics::LoadFonts(int magnification, wxDC *pInDC)
{
	m_nMagnification = magnification;

	// magnification is a percentage
	int pointSize = (int) ((DEFAULT_FONT_SIZE * magnification) / 100);

	// now round size to a multiple of 10
	//pointSize = ((pointSize + 5) / 10) * 10;
	
	wxDC *pDC = pInDC;

	// Set map mode
	pDC->SetMapMode(m_nMap_mode);

	for (unsigned int i = 0; i < m_ilmvec.size(); ++i) {
		m_ilmvec[i]->loadFont(pDC, pointSize);
	}

	delete m_latinFont;
	m_latinFont = new wxFont(pointSize, wxFONTFAMILY_ROMAN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	latinLineHeightLP = CalculateLineHeightForFont(pDC, m_latinFont);

	Long2PFontMap::iterator it 
		= m_HTML_font_map.begin();
	while (it != m_HTML_font_map.end()) {
		long characteristic_long = it->first;
		HTMLFontAttributes font_attr = HTMLFontAttributes::fromLong(characteristic_long);
		wxFont *pNewFont = this->getFontFromHTMLFontAttributes(font_attr,
								       false); // Use cache? No: We are going to replace it...
		delete it->second;
		it->second = pNewFont;
		++it;
	}
}

void ViewMetrics::Initialize(wxDC *pInDC)
{
	wxDC *pDC = pInDC;

	clearStringSizeCache();

	// select mapping mode
	wxMappingMode nOriginalMapMode = pDC->GetMapMode();
	pDC->SetMapMode(m_nMap_mode);

	// Calculate width of one pixel
	m_nOne_pixelLP = std::max(pDC->DeviceToLogicalXRel(1), 1);

	wxCoord nWidth;
	wxCoord nHeight;
	wxCoord nDescent;
	wxCoord nExternalLeading;

	// Get space between words
	wxSize       viewCharSize;
	pDC->SetFont(*m_latinFont);
	pDC->GetTextExtent(wxT("x"), 
			   &nWidth, &nHeight, 
			   &nDescent, &nExternalLeading, 
			   NULL);
	marginLP = nWidth;
	xParagraphIndentLP = marginLP * 4;


	// For spaceBetweenWordsLP, it looks better to base it 
	// on "A" rather than "x"
	pDC->GetTextExtent(wxT("A"), 
			   &nWidth, &nHeight, 
			   &nDescent, &nExternalLeading, 
			   NULL);

	spaceBetweenWordsLP = pDC->LogicalToDeviceXRel(nHeight/3);

	// ensure spaceBetweenWordsLP is an even number of pixels 
	// to ensure smooth selection
	// highlighting (highlight extends from 1/2 space before word to 1/2 space
	// after, and if space is odd number, there is a 1 pixel gap between each
	// selected word)
	if ((spaceBetweenWordsLP % 2) == 1)
		spaceBetweenWordsLP++;

	// convert space between words back to logical units
	spaceBetweenWordsLP = pDC->DeviceToLogicalXRel(spaceBetweenWordsLP);

	// Initialize m_iOverallWidth
	m_nViewWidthLP = ConvertMMtoLP(160); // 16 cm = 160 mm

	// clean up
	pDC->SetMapMode(nOriginalMapMode);
}


void ViewMetrics::Update(wxDC *pDCIn, int clientWidth, int clientHeight) 
{
	wxDC *pDC = pDCIn;
    
	// select mapping mode
	pDC->SetMapMode(m_nMap_mode);
    
	totalWidthLP = (MIN_WIDTH > clientWidth) ? MIN_WIDTH : clientWidth;
    
	if (bIsPrinting) {
		displayWidthLP = totalWidthLP;
	} else {
		displayWidthLP = totalWidthLP - (2 * marginLP);
	}
    
	displayHeightLP = clientHeight;
	RecalculateInterlinearLineHeight(pDC);
}


void ViewMetrics::RecalculateInterlinearLineHeight(wxDC *pDC)
{
	(void)(pDC); // Silence a warning
	
	interlinearLineHeightLP = 0;
	for (unsigned int i = 0; i < m_ilmvec.size(); ++i) {
		interlinearLineHeightLP += m_ilmvec[i]->height();
	}

	// add some so there's a little space between interlinear line sets
	interlinearLineHeightLP += 5*onePixelAsLP();
}


int ViewMetrics::ConvertMMtoLP(int mm) const
{
	return Convert10thofMMtoLP(mm*10);
}

int ViewMetrics::Convert10thofMMtoLP(int mm) const
{
	switch (m_nMap_mode) {
	case wxMM_TEXT:
		return (mm * 4)/10; // Screen pixels are approximately 4 to a mm.
		break;
	case wxMM_LOMETRIC:
		return mm * 1; // 10 units/mm / 10
		break;
	case wxMM_METRIC:
		return mm * 10; // 1 units/mm / 10
		break;
	case wxMM_POINTS:
		return (mm * 283) / (100*10); // 72 units / in / 25.4 mm/in / 10
		break;
	case wxMM_TWIPS:
		return (mm * 57)/10; // 1440 units/in / 25.4 mm/in / 10
		break;
	default:
		return (mm*4) / 10;
	}
}

void ViewMetrics::SetViewWidthMM(int ViewWidthInMM)
{
	m_nViewWidthLP = ConvertMMtoLP(ViewWidthInMM);
}

void ViewMetrics::setILM(unsigned int index, InterlinearLineMetrics *pILM)
{
	size_t i;
	if (index >= m_ilmvec.size()) {
		size_t current_size = m_ilmvec.size();
		m_ilmvec.resize(index+1);
		for (i = current_size; i <= index; ++i) {
			m_ilmvec[i] = new InterlinearLineMetrics();
		}
	}
	delete m_ilmvec[index];
	m_ilmvec[index] = pILM;
}

int ViewMetrics::getLineHeight(unsigned int index)
{
	if (index >= m_ilmvec.size()) {
		return ConvertMMtoLP(6);
	} else {
		return m_ilmvec[index]->lineHeightLP;
	}
}

HTMLFontAttributes ViewMetrics::startBold(void)
{
	HTMLFontAttributes current_font_attributes = m_HTML_font_attribute_stack.top();
	current_font_attributes.m_bBold = true;

	m_HTML_font_attribute_stack.push(current_font_attributes);
	return current_font_attributes;
}


HTMLFontAttributes ViewMetrics::startEm(void)
{
	HTMLFontAttributes current_font_attributes = m_HTML_font_attribute_stack.top();
	current_font_attributes.m_bItalic = !current_font_attributes.m_bItalic;

	m_HTML_font_attribute_stack.push(current_font_attributes);
	return current_font_attributes;
}


HTMLFontAttributes ViewMetrics::startUnderline(void)
{
	HTMLFontAttributes current_font_attributes = m_HTML_font_attribute_stack.top();
	current_font_attributes.m_bUnderline = true;

	m_HTML_font_attribute_stack.push(current_font_attributes);
	return current_font_attributes;
}


HTMLFontAttributes ViewMetrics::startSuperscript(void)
{
	HTMLFontAttributes current_font_attributes = m_HTML_font_attribute_stack.top();
	current_font_attributes.m_bSuperscript = true;
	current_font_attributes.m_sizeFactor = 80;

	m_HTML_font_attribute_stack.push(current_font_attributes);
	return current_font_attributes;
}


HTMLFontAttributes ViewMetrics::startH1(void)
{
	HTMLFontAttributes current_font_attributes = m_HTML_font_attribute_stack.top();
	current_font_attributes.m_bBold = true;
	current_font_attributes.m_sizeFactor = 160;

	m_HTML_font_attribute_stack.push(current_font_attributes);
	return current_font_attributes;
}


HTMLFontAttributes ViewMetrics::startH2(void)
{
	HTMLFontAttributes current_font_attributes = m_HTML_font_attribute_stack.top();
	current_font_attributes.m_bBold = true;
	current_font_attributes.m_sizeFactor = 120;

	m_HTML_font_attribute_stack.push(current_font_attributes);
	return current_font_attributes;
}


HTMLFontAttributes ViewMetrics::startAnchorNAME(void)
{
	HTMLFontAttributes current_font_attributes = m_HTML_font_attribute_stack.top();
	// Just push the same.
	// This is done because we wish to be able to pop any </a>.
	m_HTML_font_attribute_stack.push(current_font_attributes);
	return current_font_attributes;
}


HTMLFontAttributes ViewMetrics::startAnchorHREF(void)
{
	HTMLFontAttributes current_font_attributes = m_HTML_font_attribute_stack.top();
	current_font_attributes.m_color = BLUE;

	m_HTML_font_attribute_stack.push(current_font_attributes);
	return current_font_attributes;
}


HTMLFontAttributes ViewMetrics::endTag(void)
{
	m_HTML_font_attribute_stack.pop();
	return m_HTML_font_attribute_stack.top();
}

HTMLFontAttributes ViewMetrics::topFontAttributeStack(void)
{
	return m_HTML_font_attribute_stack.top();
}

void ViewMetrics::clearStringSizeCache(void)
{
	if (!m_stringSizeCache.empty()) {
		m_stringSizeCache.clear();
	}
}

// Also selects the right font, and deselects it before exiting.
// nInterlinearLine < 0 means "latin font".
int ViewMetrics::getTextWidth(wxDC *pDC, const wxString& strToBeMeasured, int nInterlinearLine)
{
	wxString strSurrogate;
	if (nInterlinearLine < 0) {
		// Latin font
		strSurrogate = wxT("L+") + strToBeMeasured;
	} else {
		strSurrogate = wxString::Format(wxT("%d+"), nInterlinearLine) + strToBeMeasured;
	}

	if (!m_stringSizeCache.empty()) {
		wxstr2size_map::const_iterator it = m_stringSizeCache.find(strSurrogate);
		if (it != m_stringSizeCache.end()) {
			// We hit the cache!
			return it->second;
		} 
	}

	if (nInterlinearLine < 0) {
		pDC->SetFont(*m_latinFont);
	} else {
		const InterlinearLineMetrics *pILM = getILM(nInterlinearLine);
		pDC->SetFont(*pILM->pFont);
	}
	
	// Get extent
	wxCoord nWidth, nDummyHeight;
	wxCoord nDescent, nExternalLeading;
	pDC->GetTextExtent(strToBeMeasured,
			   &nWidth, &nDummyHeight,
			   &nDescent, &nExternalLeading);
	pDC->SetFont(wxNullFont);
	
	m_stringSizeCache.insert(std::make_pair(strSurrogate, nWidth));
	return nWidth;
}

wxFont *ViewMetrics::getFontFromHTMLFontAttributes(const HTMLFontAttributes& font_attributes, bool bUseCacheMap)
{
	long characteristic_long = font_attributes.toLong();
	wxFont *pResult = 0;

	if (bUseCacheMap) {
		Long2PFontMap::iterator it = m_HTML_font_map.find(characteristic_long);
		if (it != m_HTML_font_map.end()) {
			pResult = it->second;
		}
	}

	if (pResult == 0) {
		int nPointSize = ((m_nMagnification * DEFAULT_FONT_SIZE * font_attributes.m_sizeFactor) / 10000);
		wxFontStyle nFontStyle = (font_attributes.m_bItalic) ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL;
		wxFontWeight fontWeight = (font_attributes.m_bBold) ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL;
		
		wxFont *pNewFont = new wxFont(nPointSize,
					      wxFONTFAMILY_ROMAN,
					      nFontStyle,
					      fontWeight,
					      false,
					      wxT(""),  // Choose default facename
					      wxFONTENCODING_DEFAULT);
		if (bUseCacheMap) {
			m_HTML_font_map.insert(std::make_pair(characteristic_long, pNewFont));
		}
		pResult = pNewFont;
	}
	return pResult;
}



int CalculateLineHeightForFont(wxDC *pDC, wxFont *pFont)
{
	wxString testString(wxT("A"));
	wxCoord nWidth;
	wxCoord nHeight;
	wxCoord nDescent;
	wxCoord nExternalLeading;
  
	// select font and get its metrics
	pDC->SetFont(*pFont);
	pDC->GetTextExtent(testString, 
			   &nWidth, &nHeight, 
			   &nDescent, &nExternalLeading);
  
  
	// Calculate view character size (will work for Bold, Italic versions, too)
	return nHeight + nExternalLeading;
}
  
