//
// layout.cpp
//
// Ulrik Sandborg-Petersen
//
// Based on code by Todd Hoatson and/or Fred Eichelman, 
// though heavily modified by Ulrik Petersen.
//
// The copyright for the original code was assigned to Ulrik Petersen
// by Todd Hoatson's superior, Steve Echerd, in a letter dated
// November 11, 2003.
//
// Created: 7/17-2005
// Last update: 10/4-2018
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
#pragma implementation "layout.h"
#endif




#include <iostream>


#include <wx/wx.h>

#include <layout.h>
#include <viewmetrics.h>
#include <string_func.h>
#include <debug.h> // For ASSERT_THROW

#include <wx/sstream.h>
#include <wx/strconv.h>
#include <wx/pen.h>
#include <wx/version.h>

#ifndef WIN32
#include <sys/time.h>
#include <time.h>
#endif



wxColour LAYOUT_GRAY(0xAA, 0xAA, 0xAA);



/*
  #ifdef _DEBUG
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
  #endif
*/

// #define MYNBSP "<table width=\"12pt\"><tr><td> </td></tr></table>"
#define MYNBSP "&nbsp;"


//////////////////////////////////////////////////////////
//
// LayoutBoxBase
//
//////////////////////////////////////////////////////////


LayoutBoxBase::LayoutBoxBase(ViewMetrics *pMetrics, bool bSpaceAfter, bool bIsSelected, bool bIsRightToLeft)
	: LayoutItem(pMetrics),
	  m_bIsSelected(bIsSelected),
	  m_bSpaceAfter(bSpaceAfter),
	  m_bIsRightToLeft(bIsRightToLeft),
	  m_long(0)
{
	/*
	  // Set extents in x and y direction
	  // Already set in LayoutItem constructor!
	  m_yExtentLP = 0;
	  m_xExtentLP = 0;
	*/
}

#if wxCHECK_VERSION(2, 9, 0)
// wxWidgets version being used is 2.9.0 or greater.
// Nothing to do,
#else
// wxWidgets version is less than 2.9.0, 
// so we must typedef wxRasterOperation to int.
typedef int wxRasterOperationMode;
#endif



void LayoutBoxBase::DrawBox(wxDC *pDC, int xLeftLP, int yTopLP)
{
	// If it is selected, and it is not printing,
	// invert box.
	if (m_bIsSelected && !m_pMetrics->bIsPrinting) {
		int xRightLP = xLeftLP + m_xExtentLP;

#ifdef __WXOSX__
		// For Mac OS X, this seems to be necessary...
		// Otherwise, the inversion doesn't go box-to-box, but leaves
		// a small band of non-inverted pixels in between.
		xRightLP += 1;
#endif

		int yBottomLP = yTopLP + m_yExtentLP*m_pMetrics->nMMScaleFactor();
		
		wxPoint point(xLeftLP, yTopLP);
		/*
#ifdef __WXOSX__
		wxSize  mysize(abs(xRightLP-xLeftLP)-1, abs(yBottomLP-yTopLP)-1);
#else
		wxSize  mysize(abs(xRightLP-xLeftLP), abs(yBottomLP-yTopLP));
#endif
		*/
		wxSize  mysize(abs(xRightLP-xLeftLP), abs(yBottomLP-yTopLP));

		wxRasterOperationMode oldLogicalFunction = pDC->GetLogicalFunction();

		// wxINVERT does not work on __WXOSX__, but it does on
		// Windows.
#ifdef __WXMSW__
		pDC->SetLogicalFunction(wxINVERT);
#else
		pDC->SetLogicalFunction(wxXOR);
#endif
		pDC->SetPen(*wxBLACK_PEN);
		pDC->SetBrush(*wxWHITE_BRUSH);
		pDC->DrawRectangle(point, mysize);
		pDC->SetBrush(wxNullBrush);
		pDC->SetPen(wxNullPen);
		pDC->SetLogicalFunction(oldLogicalFunction);
	} /*else {
		if (m_bSpaceAfter) {
			std::cerr << "UP210: SpaceAfter is TRUE" << std::endl;
		} else {
			std::cerr << "UP214: SpaceAfter is FALSE" << std::endl;
		}
		}*/
	
}


//////////////////////////////////////////////////////////
//
// LayoutBox
//
//////////////////////////////////////////////////////////


LayoutBox::LayoutBox(wxDC *pDC, const wxstringvec_t& strings, ViewMetrics *pMetrics, bool bSpaceAfter, bool bIsRightToLeft, bool bIsCentered)
	: LayoutBoxBase(pMetrics, bSpaceAfter, false, bIsRightToLeft),
	  m_bIsGrayed(false),
	  m_bIsCentered(bIsCentered),
	  m_strings(strings)
{
	RecalculateExtents(pDC);
}


void LayoutBox::Draw(wxDC *pDC, int xLP, int yLP)
{
	int y = yLP;
	int x = xLP;
	unsigned int strings_size = m_strings.size();
	for (unsigned int index = 0; index < strings_size; ++index) {
		const InterlinearLineMetrics *pILM = m_pMetrics->getILM(index);

		if (pILM->bDisplayed) {
			wxString mystr = m_strings[index];
			DrawText(pDC, mystr, x, y, index);
			y = y + m_pMetrics->nMMScaleFactor()*pILM->lineHeightLP;
		}
	}

	DrawBox(pDC, xLP, yLP);
}

void LayoutBox::RecalculateExtents(wxDC *pDC)
{
	// Set extents in x and y direction
	m_yExtentLP = 0;
	m_xExtentLP = 0;

	unsigned int strings_size = m_strings.size();
	m_lengths.resize(strings_size);
	for (unsigned int index = 0; index < strings_size; ++index) {
	        const InterlinearLineMetrics *pILM = m_pMetrics->getILM(index);
		if (pILM->bDisplayed) {
			m_yExtentLP += pILM->height();
		}
		/*
		  #if defined(__WXGTK__)
		  if (pILM->fontEncoding != wxFONTENCODING_UTF8) {
		  wchar_t szBuf[1024];
		  size_t endbuf = wxConvISO8859_1.MB2WC(szBuf, m_strings[index].c_str(), 1024);
		  szBuf[endbuf] = L'\0';
		  m_strings[index] = wxString(szBuf);
		  }
		  #endif
		*/

		// Get extent
		wxCoord nWidth = 
			m_pMetrics->getTextWidth(pDC, 
						 m_strings[index], 
						 index);

		m_lengths[index] = nWidth;

		// Set m_xExtentLP to max of self and nWidth.
		if (pILM->bDisplayed) {
			if (m_xExtentLP < nWidth) {
				m_xExtentLP = nWidth;
			}
		}
	}

	if (m_bSpaceAfter) {
		m_xExtentLP += m_pMetrics->spaceBetweenWordsLP;
	}
}



/*
void LayoutBox::DrawBox(wxDC *pDC, int xLeftLP, int yTopLP, int xRightLP, int yBottomLP)
{
	// Extend box a little past the edges on either side.
	// This will make highlighted boxes appear seamlessly highlighted.
	int nHalfSpaceLP = m_pMetrics->spaceBetweenWordsLP/2;

	// For all others than Win32, this seems to be necessary...
	// Otherwise, the inversion doesn't go box-to-box, but leaves
	// a small band of non-inverted pixels in between.
#ifndef __WXMSW__
	nHalfSpaceLP += 1;
#endif

	xLeftLP -= nHalfSpaceLP;
	xRightLP += nHalfSpaceLP;


	// If it is selected, and it is not printing,
	// invert box.
	if (m_bIsSelected && !m_pMetrics->bIsPrinting) {
		wxPoint point(xLeftLP, yTopLP);
		wxSize  mysize(abs(xRightLP-xLeftLP), abs(yBottomLP-yTopLP));

		wxRasterOperationMode oldLogicalFunction = pDC->GetLogicalFunction();
		// wxINVERT does not work on __WXOSX__, but it does on
		// Windows.
#ifdef __WXMSW__
		pDC->SetLogicalFunction(wxINVERT);
#else
		pDC->SetLogicalFunction(wxXOR);
#endif
		pDC->SetPen(*wxBLACK_PEN);
		pDC->SetBrush(*wxWHITE_BRUSH);
		pDC->DrawRectangle(point, mysize);
		pDC->SetBrush(wxNullBrush);
		pDC->SetPen(wxNullPen);
		pDC->SetLogicalFunction(oldLogicalFunction);
				
	}
}
*/

void LayoutBox::DrawText(wxDC *pDC, wxString strOutput, int xLP, int yLP, unsigned int ilmIndex)
{
	const InterlinearLineMetrics* pILM = m_pMetrics->getILM(ilmIndex);
	wxFont *pFont = pILM->pFont;
        wxColour color = pILM->m_color;
        if (m_bIsGrayed) {
                color = LAYOUT_GRAY;
        } else {
                color = pILM->m_color;
        }

	// Set font and colors
	pDC->SetFont(*pFont);
	pDC->SetTextBackground(*wxWHITE);
	pDC->SetTextForeground(color);

	// Draw text
	if (m_bIsCentered) {
		int length = m_lengths[ilmIndex];
		int x = xLP + (m_xExtentLP - length)/2;
		pDC->DrawText(strOutput, x, yLP);
	} else if (m_bIsRightToLeft) {
		int length = m_lengths[ilmIndex];
		int x = xLP + m_xExtentLP - length;
		pDC->DrawText(strOutput, x, yLP);
	} else {
		pDC->DrawText(strOutput, xLP, yLP);
	}
	pDC->SetTextForeground(*wxBLACK);
	pDC->SetFont(wxNullFont);
}

void LayoutBox::toHTML(std::ostream *pOut, bool bIsRightToLeft, const std::string& filename)
{
	(void)(filename); // Silence a warning
	
	(*pOut) << "<div class=\"unit"
		<< ((bIsRightToLeft) ? "rtl" : "ltr")
		<< ((m_bIsSelected) ? "selected" : "")
		<< "\">";
	int strings_size = m_strings.size();
	for (int index = 0; index < m_pMetrics->getNoOfILMs(); ++index) {

		wxString mystr;
		if (index < strings_size) {
			mystr = m_strings[index];
			if (mystr.IsEmpty()) {
				mystr = wxString(wxT(MYNBSP));
			}
		} else {
			mystr = wxString(wxT(MYNBSP));
		}
		(*pOut) << "<p class=\"unitline" << index 
			<< "\">";
		(*pOut) << std::string((const char*)mystr.mb_str(wxConvUTF8));
		(*pOut) << "</p>";
	}
	(*pOut) << "</div>";
	
}

//////////////////////////////////////////////////////////
//
// SimpleLayoutBox
//
//////////////////////////////////////////////////////////



SimpleLayoutBox::SimpleLayoutBox(wxDC *pDC, const wxString& mystring, ViewMetrics *pMetrics, bool bSpaceAfter)
	: LayoutBox(pDC, wxstringvec_t(1, mystring), pMetrics, bSpaceAfter, false)
{
}





//////////////////////////////////////////////////////////
//
// LatinLayoutBox
//
//////////////////////////////////////////////////////////


LatinLayoutBox::LatinLayoutBox(wxDC *pDC, const wxString& mystring, ViewMetrics *pMetrics, wxColour color, bool bSpaceAfter) 
	: SimpleLayoutBox(pDC, mystring, pMetrics, bSpaceAfter),
	  m_color(color)
{

	RecalculateExtents(pDC);
}


LatinLayoutBox::~LatinLayoutBox()
{
}

void LatinLayoutBox::DrawText(wxDC *pDC, wxString strOutput, int xLP, int yLP, unsigned int ilmIndex)
{
	(void)(ilmIndex); // Silence a warning
	
	wxFont *pFont = m_pMetrics->getLatinFont();
  
	// Set font and colors
	pDC->SetFont(*pFont);
	pDC->SetTextBackground(*wxWHITE);
	pDC->SetTextForeground(m_color);
  
	// Draw text
	pDC->DrawText(strOutput, xLP, yLP);
  
	pDC->SetFont(wxNullFont);
	pDC->SetTextForeground(*wxBLACK);
}

void LatinLayoutBox::RecalculateExtents(wxDC *pDC)
{
	// Get extents in x and y direction
	m_yExtentLP = m_pMetrics->getLatinFontHeight();
	m_xExtentLP = 0;

	wxCoord nWidth = m_pMetrics->getTextWidth(pDC,
						  m_strings[0],
						  -1);

	m_xExtentLP = nWidth;
	if (m_bSpaceAfter) {
		m_xExtentLP += m_pMetrics->spaceBetweenWordsLP;
	}
}

void LatinLayoutBox::toHTML(std::ostream *pOut, bool bIsRightToLeft, const std::string& filename)
{
	(void)(filename); // Silence a warning
	
	(*pOut) << "<div class=\"latinunit"
		<< ((bIsRightToLeft) ? "rtl" : "ltr")
		<< ((m_bIsSelected) ? "selected" : "")
		<< "\">";
	wxString mystr = m_strings[0];
	(*pOut) << "<p class=\"latinunitline\">";
	(*pOut) << std::string((const char*)mystr.mb_str(wxConvUTF8));
	(*pOut) << "</p>";
	(*pOut) << "</div>";
}




//////////////////////////////////////////////////////////
//
// BorderedStackedLatinLayoutBox
//
//////////////////////////////////////////////////////////


BorderedStackedLatinLayoutBox::BorderedStackedLatinLayoutBox(wxDC *pDC, const wxstringvec_t& strings, ViewMetrics *pMetrics, wxColour beforeSeparatorLinesColor, wxColour afterSeparatorLinesColor, bool bIsCentered, bool bSpaceAfter, int nLineAfterWhichToAddSeparator)
	: LayoutBoxBase(pMetrics, bSpaceAfter, false, false),
	  m_bIsGrayed(false),
	  m_bIsCentered(bIsCentered),
	  m_nLineAfterWhichToAddSeparator(nLineAfterWhichToAddSeparator),
	  m_beforeSeparatorLinesColor(beforeSeparatorLinesColor),
	  m_afterSeparatorLinesColor(afterSeparatorLinesColor),
	  m_strings(strings)
{
	RecalculateExtents(pDC);
}


void BorderedStackedLatinLayoutBox::Draw(wxDC *pDC, int xLP, int yLP)
{
	wxFont *pFont = m_pMetrics->getLatinFont();

	// Set font and colors
	pDC->SetFont(*pFont);
	pDC->SetTextBackground(*wxWHITE);
	pDC->SetTextForeground(m_beforeSeparatorLinesColor);

	int y = yLP + m_pMetrics->getLatinFontHeight() / 4;
	int x = xLP + m_pMetrics->getLatinFontHeight() / 2;
	int xWidth = m_xExtentLP - m_pMetrics->getLatinFontHeight();
	if (m_bSpaceAfter) {
		xWidth -= m_pMetrics->spaceBetweenWordsLP;
	}
	unsigned int strings_size = m_strings.size();
	for (unsigned int index = 0; index < strings_size; ++index) {
		const wxString& mystr = m_strings[index];
		int myx;
		if (m_bIsCentered) {
			myx = x + (xWidth - m_lengths[index])/2;
		} else {
			myx = x;
		}
		DrawText(pDC, mystr, myx, y);
		y = y + m_pMetrics->nMMScaleFactor()*m_pMetrics->getLatinFontHeight();
		// After the line after which to add the separator,
		// add half a latin line's height.
		if ((m_nLineAfterWhichToAddSeparator >= 0)
		    && index == ((unsigned int) m_nLineAfterWhichToAddSeparator)) {
			y += (m_pMetrics->getLatinFontHeight() / 2);
		}

		if ((m_nLineAfterWhichToAddSeparator >= 0)
		    && index == ((unsigned int) m_nLineAfterWhichToAddSeparator)) {
			pDC->SetTextForeground(m_afterSeparatorLinesColor);
		}
	}

	pDC->SetTextForeground(*wxBLACK);
	pDC->SetFont(wxNullFont);

	DrawBox(pDC, xLP, yLP);
}

void BorderedStackedLatinLayoutBox::DrawText(wxDC *pDC, const wxString& strOutput, int xLP, int yLP)
{
	// Draw text
	pDC->DrawText(strOutput, xLP, yLP);
}

void BorderedStackedLatinLayoutBox::DrawBox(wxDC *pDC, int xLeftLP, int yTopLP)
{
	int xRightLP = xLeftLP + m_xExtentLP;
	if (m_bSpaceAfter) {
		xRightLP -= m_pMetrics->spaceBetweenWordsLP;
	}

	// Start with top
	int yBottomLP = yTopLP + m_yExtentLP;

	// Set logical function and black pen
	wxRasterOperationMode oldLogicalFunction = pDC->GetLogicalFunction();
	pDC->SetLogicalFunction(wxCOPY);

	int nPenWidth = getPenWidth(m_pMetrics->m_nMagnification);

	// Make pen for border
#if wxCHECK_VERSION(3,0,0)
	wxPen penBorder(*wxBLACK, nPenWidth, wxPENSTYLE_SOLID);
#else
	wxPen penBorder(*wxBLACK, nPenWidth, wxSOLID);
#endif

	// Draw bracket or parenthesis itself
	pDC->SetPen(penBorder);
	
	// Draw box around first string
	pDC->DrawLine(xLeftLP, yTopLP, xRightLP, yTopLP);
	pDC->DrawLine(xRightLP, yTopLP, xRightLP, yBottomLP);
	pDC->DrawLine(xLeftLP, yBottomLP, xRightLP, yBottomLP);
	pDC->DrawLine(xLeftLP, yTopLP, xLeftLP, yBottomLP);
	

	// Do we have more than one string?
	if (m_strings.size() > 1
	    && 	(m_nLineAfterWhichToAddSeparator >= 0)
	    && ((unsigned int) m_nLineAfterWhichToAddSeparator < m_strings.size())) {
		// Yes, so add the separator

		int ySecondBottomLP = yTopLP;

		// Add height used by the first lines:
		ySecondBottomLP += m_pMetrics->getLatinFontHeight() * (m_nLineAfterWhichToAddSeparator + 2);


		// Remove height used by border
		ySecondBottomLP -= m_pMetrics->getLatinFontHeight() / 2;

		pDC->DrawLine(xLeftLP, ySecondBottomLP, xRightLP, ySecondBottomLP);
		
	}
	pDC->SetPen(wxNullPen);
	pDC->SetLogicalFunction(oldLogicalFunction);

	// Call parent
	LayoutBoxBase::DrawBox(pDC, xLeftLP, yTopLP);
}



void BorderedStackedLatinLayoutBox::RecalculateExtents(wxDC *pDC)
{
	m_yExtentLP = 0;
	m_xExtentLP = 0;
	unsigned int strings_size = m_strings.size();
	m_lengths.resize(strings_size);
	for (unsigned int index = 0; index < strings_size; ++index) {
		m_yExtentLP += m_pMetrics->getLatinFontHeight();

		wxCoord nWidth = 
			m_pMetrics->getTextWidth(pDC,
						 m_strings[index],
						 -1);

		m_lengths[index] = nWidth;

		// Set m_xExtentLP to max of self and nWidth.
		if (m_xExtentLP < nWidth) {
			m_xExtentLP = nWidth;
		}
	}

	// Add a latin line's height to the vertical height
	// so as to make room for the border.
	if (m_nLineAfterWhichToAddSeparator >= 0) {
		m_yExtentLP += (m_pMetrics->getLatinFontHeight());
	} else {
		m_yExtentLP += (m_pMetrics->getLatinFontHeight() / 2);
	}

	// Add space for the border also horizontally
	m_xExtentLP += m_pMetrics->getLatinFontHeight();

	if (m_bSpaceAfter) {
		m_xExtentLP += m_pMetrics->spaceBetweenWordsLP;
	}

	int nPenWidth = getPenWidth(m_pMetrics->m_nMagnification);
	m_xExtentLP += 2*nPenWidth;
	m_yExtentLP += 2*nPenWidth;
}


void BorderedStackedLatinLayoutBox::toHTML(std::ostream *pOut, bool bIsRightToLeft, const std::string& filename)
{
	(void)(filename); // Silence a warning
	
	(*pOut) << "<div class=\"unit"
		<< ((bIsRightToLeft) ? "rtl" : "ltr")
		<< ((m_bIsSelected) ? "selected" : "")
		<< "\">";
	unsigned int strings_size = m_strings.size();
	for (unsigned int index = 0; index < strings_size; ++index) {

		wxString mystr = m_strings[index];
		if (mystr.IsEmpty()) {
			mystr = wxString(wxT(MYNBSP));
		}
		(*pOut) << "<p class=\"unitline" << index 
			<< "\">";
		(*pOut) << std::string((const char*)mystr.mb_str(wxConvUTF8));
		(*pOut) << "</p>";
	}
	(*pOut) << "</div>";
}


//////////////////////////////////////////////////////////
//
// StackedLatinLayoutBox
//
//////////////////////////////////////////////////////////


StackedLatinLayoutBox::StackedLatinLayoutBox(wxDC *pDC, const wxstringvec_t& strings, ViewMetrics *pMetrics, wxColour color, bool bSpaceAfter, bool bIsCentered)
	: LayoutBoxBase(pMetrics, bSpaceAfter, false, false),
	  m_bIsGrayed(false),
	  m_bIsCentered(bIsCentered),
	  m_color(color),
	  m_strings(strings)
{
	RecalculateExtents(pDC);
}


void StackedLatinLayoutBox::Draw(wxDC *pDC, int xLP, int yLP)
{
	wxFont *pFont = m_pMetrics->getLatinFont();

	// Set font and colors
	pDC->SetFont(*pFont);
	pDC->SetTextBackground(*wxWHITE);
	pDC->SetTextForeground(m_color);

	int y = yLP;
	int x = xLP;
	unsigned int strings_size = m_strings.size();
	for (unsigned int index = 0; index < strings_size; ++index) {
		const wxString& mystr = m_strings[index];
		DrawText(pDC, index, mystr, x, y);
		y = y + m_pMetrics->nMMScaleFactor()*m_pMetrics->getLatinFontHeight();
	}

	pDC->SetTextForeground(*wxBLACK);
	pDC->SetFont(wxNullFont);


	DrawBox(pDC, xLP, yLP);
}

void StackedLatinLayoutBox::DrawText(wxDC *pDC, int ilmIndex, const wxString& strOutput, int xLP, int yLP)
{
	// Draw text
	// FIXME: Draw right-to-left strings as flush-right
	if (m_bIsCentered) {
		int length = m_lengths[ilmIndex];
		int x = xLP + (m_xExtentLP - length)/2;
		pDC->DrawText(strOutput, x, yLP);
		/*
	} else if (m_bIsRightToLeft) {
		int length = m_lengths[ilmIndex];
		int x = xLP + m_xExtentLP - length;
		pDC->DrawText(strOutput, x, yLP);
		*/
	} else {
		pDC->DrawText(strOutput, xLP, yLP);
	}
}

void StackedLatinLayoutBox::RecalculateExtents(wxDC *pDC)
{
	m_yExtentLP = 0;
	m_xExtentLP = 0;
	unsigned int strings_size = m_strings.size();	
	m_lengths.resize(strings_size);
	for (unsigned int index = 0; index < strings_size; ++index) {
		m_yExtentLP += m_pMetrics->getLatinFontHeight();

		wxCoord nWidth = 
			m_pMetrics->getTextWidth(pDC,
						 m_strings[index],
						 -1);

		m_lengths[index] = nWidth;

		// Set m_xExtentLP to max of self and nWidth.
		if (m_xExtentLP < nWidth) {
			m_xExtentLP = nWidth;
		}
	}

	if (m_bSpaceAfter) {
		m_xExtentLP += m_pMetrics->spaceBetweenWordsLP;
	}
}


void StackedLatinLayoutBox::toHTML(std::ostream *pOut, bool bIsRightToLeft, const std::string& filename)
{
	(void)(filename); // Silence a warning
	
	(*pOut) << "<div class=\"unit"
		<< ((bIsRightToLeft) ? "rtl" : "ltr")
		<< ((m_bIsSelected) ? "selected" : "")
		<< "\">";
	unsigned int strings_size = m_strings.size();
	for (unsigned int index = 0; index < strings_size; ++index) {

		wxString mystr = m_strings[index];
		if (mystr.IsEmpty()) {
			mystr = wxString(wxT(MYNBSP));
		}
		(*pOut) << "<p class=\"unitline" << index 
			<< "\">";
		(*pOut) << std::string((const char*)mystr.mb_str(wxConvUTF8));
		(*pOut) << "</p>";
	}
	(*pOut) << "</div>";
	
}


//////////////////////////////////////////////////////////
//
// LayoutSymbolBox
//
//////////////////////////////////////////////////////////

LayoutSymbolBox::LayoutSymbolBox(wxDC *pDC, 
                                 ViewMetrics *pVwMetrics,
                                 Symbol_Type st,
                                 bool bSpaceAfter) 
	: LayoutBoxBase(pVwMetrics, bSpaceAfter, false, false),
          m_kind(st),
          m_bIsGrayed(false)
{
	RecalculateExtents(pDC);
}




LayoutSymbolBox::~LayoutSymbolBox(void)
{
}

void LayoutSymbolBox::toHTML(std::ostream *pOut, bool bIsRightToLeft, const std::string& filename)
{
	(void)(pOut); // Silence a warning
	(void)(bIsRightToLeft); // Silence a warning
	(void)(filename); // Silence a warning
	
        // FIXME: Implement
}


void LayoutSymbolBox::Draw(wxDC *pDC, int x, int y)
{
	// Calculate y values for 
        int nRadius = (m_circle_diameter/2);
	int yMiddle = y + m_pMetrics->nMMScaleFactor()*nRadius;
        int xMiddle = x + nRadius;
        int yBottom = y + m_pMetrics->nMMScaleFactor()*m_yExtentLP;
        int yTop = y;
        int xLeft = x;
        int xRight = x + 2*nRadius;

        pDC->DrawCircle(xMiddle, yMiddle, nRadius);

	// Make pen for symbol
        wxColour color;
        if (m_bIsGrayed) {
                color = LAYOUT_GRAY;
        } else {
                color = *wxBLACK;
        }

#if wxCHECK_VERSION(3,0,0)
	wxPen penSymbol(color, m_nLineWidth, wxPENSTYLE_SOLID);
#else
	wxPen penSymbol(color, m_nLineWidth, wxSOLID);
#endif

	// Draw null symbol
	pDC->SetPen(penSymbol);
        pDC->DrawCircle(xMiddle, yMiddle, nRadius);
        pDC->DrawLine(xLeft, yBottom, xRight, yTop);
	pDC->SetPen(wxNullPen);



	// Draw bounding box, for testing purposes
	/*
	pDC->DrawLine(x,y,x+m_xExtentLP,y);
	pDC->DrawLine(x+m_xExtentLP,y,x+m_xExtentLP,y+m_yExtentLP);
	pDC->DrawLine(x+m_xExtentLP,y+m_yExtentLP,x, y+m_yExtentLP);
	pDC->DrawLine(x, y+m_yExtentLP,x,y);
	*/

	DrawBox(pDC, x, y);
}

void LayoutSymbolBox::RecalculateExtents(wxDC *pDC)
{
	(void)(pDC); // Silence a warning
	
	// Set yExtent
	m_yExtentLP = m_pMetrics->getILM(0)->lineHeightLP;
  
	// Calculate parameters
	calculate_parameters();

        m_xExtentLP = m_circle_diameter;
  
	if (m_bSpaceAfter) {
		m_xExtentLP += m_pMetrics->spaceBetweenWordsLP;
	}
}


void LayoutSymbolBox::calculate_parameters()
{
        m_circle_diameter = m_yExtentLP;
	int nMetricsLineHeight = m_pMetrics->getLineHeight();
        m_nLineWidth = nMetricsLineHeight*3/32;
}


//////////////////////////////////////////////////////////
//
// LayoutBracketBox
//
//////////////////////////////////////////////////////////

LayoutBracketBox::LayoutBracketBox(wxDC *pDC, 
				   ViewMetrics *pVwMetrics,
				   bool opening, 
				   bool is_inside, 
				   wxString label,
				   Bracket_Size bs, Bracket_Type bt,
				   bool bIsRightToLeft,
				   bool bSpaceAfter) 
	: LayoutBoxBase(pVwMetrics, bSpaceAfter, false, bIsRightToLeft),
	  m_bracket_size(bs),
	  m_bracket_type(bt),
	  m_label(label),
	  m_bIsRightToLeft(bIsRightToLeft)
{
	// Set bracket direction
	if (m_bIsRightToLeft) {
		m_bracket_direction = (!opening) ? BD_LEFT : BD_RIGHT;
	} else {
		m_bracket_direction = (opening) ? BD_LEFT : BD_RIGHT;
	}
  
	// Set "has arrow"
	m_has_arrow = is_inside;
  
	// Set "has label"
	m_has_label = should_have_label(opening, is_inside);
  
	RecalculateExtents(pDC);
}




LayoutBracketBox::~LayoutBracketBox(void)
{
}

void LayoutBracketBox::toHTML(std::ostream *pOut, bool bIsRightToLeft, const std::string& filename)
{
	(void)(filename); // Silence a warning
	
	std::string strBracketType;
	switch (m_bracket_type) {
	case BT_BRACKET:
		strBracketType = "bracket";
		break;
	case BT_DOUBLE_BRACKET:
		strBracketType = "doublebracket";
		break;
	case BT_PARENTHESIS:
		strBracketType = "parenthesis";
		break;
	default:
		ASSERT_THROW(false, "Unknown m_bracket_type");
	}
	(*pOut) << "<div class=\"bracketunit" + strBracketType
		<< ((m_bracket_direction == BD_RIGHT) ? "right" : "left")
		<< ((bIsRightToLeft) ? "rtl" : "ltr")
		<< ((m_bIsSelected) ? "selected" : "")
		<< "\">";
	for (int index = 0; index < m_pMetrics->getNoOfILMs(); ++index) {
		(*pOut) << "<p class=\"unitline" << index 
			<< "\">";
		(*pOut) << MYNBSP;
		(*pOut) << "</p>";
	}
	(*pOut) << "</div>";
}


void LayoutBracketBox::Draw(wxDC *pDC, int x, int y)
{
	// Calculate y values for bracket
	int yMiddle = y + m_pMetrics->nMMScaleFactor()*(m_yExtentLP/2);
	int yTop = yMiddle - m_pMetrics->nMMScaleFactor()*(m_nHeight/2);
	int yBottom = yTop + m_pMetrics->nMMScaleFactor()*m_nHeight;

	// Calculate x values for bracket
	int xVertical; // Is the x of the outer point
	int xSerif;    // Is the x of the inner point
	if (m_bracket_direction == BD_LEFT) {
		xVertical = x + m_nLineWidth/2;
		if (m_bSpaceAfter) {
			xVertical += m_pMetrics->spaceBetweenWordsLP;
		}
		xSerif = xVertical + m_nSerifLength;
	} else { // BD_RIGHT
		xVertical = x + m_xExtentLP - m_nLineWidth/2;
		if (m_bSpaceAfter) {
			xVertical -= m_pMetrics->spaceBetweenWordsLP;
		}
		xSerif = xVertical - m_nSerifLength;
	}

	// Make pen for bracket or parenthesis
#if wxCHECK_VERSION(3,0,0)
	wxPen penBracket(*wxBLACK, m_nLineWidth, wxPENSTYLE_SOLID);
#else
	wxPen penBracket(*wxBLACK, m_nLineWidth, wxSOLID);
#endif

	// Draw bracket or parenthesis itself
	pDC->SetPen(penBracket);
	if (m_bracket_type == BT_BRACKET) {
		pDC->DrawLine(xSerif, yTop, xVertical, yTop);
		pDC->DrawLine(xVertical, yTop, xVertical, yBottom);
		pDC->DrawLine(xVertical, yBottom, xSerif, yBottom);
	} else if (m_bracket_type == BT_DOUBLE_BRACKET)	{ 
		// Calculate y's and x'es
		int yTopTopLine = yTop;
		int yTopBottomLine = yTop + m_pMetrics->nMMScaleFactor()*m_nSerifLength/2;
		int yBottomTopLine = yBottom - m_pMetrics->nMMScaleFactor()*m_nSerifLength/2;
		int yBottomBottomLine = yBottom;
		int xOuter;
		int xInner;
		if (m_bracket_direction == BD_LEFT) {
			xOuter = xVertical;
			xInner = xSerif - (m_nSerifLength)/2;
		} else { // BD_RIGHT
			xOuter = xVertical;
			xInner = xSerif + (m_nSerifLength)/2;
		}

		// Draw outer
		pDC->DrawLine(xSerif, yTopTopLine, xOuter, yTopTopLine);
		pDC->DrawLine(xOuter, yTopTopLine, xOuter, yBottomBottomLine);
		pDC->DrawLine(xOuter, yBottomBottomLine, xSerif, yBottomBottomLine);

		// Draw inner
		pDC->DrawLine(xSerif, yTopBottomLine, xInner, yTopBottomLine);
		pDC->DrawLine(xInner, yTopBottomLine, xInner, yBottomTopLine);
		pDC->DrawLine(xInner, yBottomTopLine, xSerif, yBottomTopLine);
	} else { // BT_PARENTHESIS
		int yTopCtrl = yTop + m_pMetrics->nMMScaleFactor()*m_nHeight/4;
		int yBottomCtrl = yBottom - m_pMetrics->nMMScaleFactor()*m_nHeight/4;
		wxPoint points[4] = {
		    wxPoint(xSerif, yBottom),
		    wxPoint(xVertical, yBottomCtrl),
		    wxPoint(xVertical, yTopCtrl),
		    wxPoint(xSerif, yTop)
		};
		pDC->DrawSpline(4, points);
	}
	pDC->SetPen(wxNullPen);

	// Make arrow if necessary
	if (m_has_arrow) {
		// Calculate parameters for arrow
		int xArrowHead;
		int xArrowWing;
		int xArrowTail;

		// Calculate length of the lines extending at an angle 
		// from the head of the arrow
		int nArrowWingLength = (m_nArrowLength*4)/10;

		// Calculate x parameters
		if (m_bracket_direction == BD_LEFT) {
			xArrowHead = xVertical + m_nInnerGap;
			xArrowWing = xArrowHead + nArrowWingLength;
			xArrowTail = xArrowHead + m_nArrowLength;
		} else { // BD_RIGHT
			xArrowHead = xVertical - m_nInnerGap;
			xArrowWing = xArrowHead - nArrowWingLength;
			xArrowTail = xArrowHead - m_nArrowLength;
		}

		// Calculate y parameters
		int yArrowWingLow = yMiddle - m_pMetrics->nMMScaleFactor()*nArrowWingLength;
		int yArrowWingHigh = yMiddle + m_pMetrics->nMMScaleFactor()*nArrowWingLength;

		// Draw arrow
#if wxCHECK_VERSION(3,0,0)
		wxPen penArrow(*wxBLACK, m_nArrowLineWidth, wxPENSTYLE_SOLID);
#else
		wxPen penArrow(*wxBLACK, m_nArrowLineWidth, wxSOLID);
#endif
		pDC->SetPen(penArrow);
		pDC->DrawLine(xArrowTail, yMiddle, xArrowHead, yMiddle);
		pDC->DrawLine(xArrowHead, yMiddle, xArrowWing, yArrowWingHigh);
		pDC->DrawLine(xArrowHead, yMiddle, xArrowWing, yArrowWingLow);
		pDC->SetPen(wxNullPen);
	}

	// Make label if necessary
	if (m_has_label) {
		pDC->SetFont(*m_pMetrics->getLatinFont());
		pDC->SetTextForeground(*wxBLACK);

		int yLabel = y + m_pMetrics->interlinearLineHeightLP/2 - m_pMetrics->latinLineHeightLP/2;

		// Do actual placement and drawing
		if (m_bracket_direction == BD_LEFT) {
			pDC->DrawText(m_label, xSerif + m_nInnerGap - m_nSerifLength/2  + m_nLineWidth/2, yLabel);
		} else {  // BD_RIGHT
			pDC->DrawText(m_label, xSerif - m_nInnerGap + m_nSerifLength/2 - m_nLineWidth/2 - m_nLabelXExtent, yLabel);
		}
		pDC->SetFont(wxNullFont);
	}

	


	// Draw bounding box, for testing purposes
	/*
	pDC->DrawLine(x,y,x+m_xExtentLP,y);
	pDC->DrawLine(x+m_xExtentLP,y,x+m_xExtentLP,y+m_yExtentLP);
	pDC->DrawLine(x+m_xExtentLP,y+m_yExtentLP,x, y+m_yExtentLP);
	pDC->DrawLine(x, y+m_yExtentLP,x,y);
	*/

	DrawBox(pDC, x, y);
}

void LayoutBracketBox::RecalculateExtents(wxDC *pDC)
{
	// Set yExtent
	m_yExtentLP = m_pMetrics->interlinearLineHeightLP;
  
	// Calculate parameters
	calculate_parameters(m_pMetrics);
  
	// Calculate xExtent
	if (!m_has_label && !m_has_arrow) { // has neither label nor arrow
		m_xExtentLP = m_nSerifLength + (m_nLineWidth*2)/2 + m_nOuterGap;
	} else if (m_has_label) { // has label
		m_nLabelXExtent = m_pMetrics->getTextWidth(pDC,
							   m_label,
							   -1);

		m_xExtentLP = m_nSerifLength + m_nInnerGap + m_nLabelXExtent + m_nOuterGap;
	} else { // has arrow
		m_xExtentLP = m_nSerifLength + m_nInnerGap + m_nArrowLength+ m_nOuterGap;
	}
	
	if (m_bSpaceAfter) {
		m_xExtentLP += m_pMetrics->spaceBetweenWordsLP;
	}
}


void LayoutBracketBox::calculate_parameters(ViewMetrics *pVwMetrics)
{
	int nMetricsLineHeight = pVwMetrics->getLineHeight();
	switch (m_bracket_size) {
	case BS_SMALLEST:
		m_nHeight = m_yExtentLP*7/10;
		m_nLineWidth = nMetricsLineHeight*2/32;
		break;
	case BS_SMALL:
		m_nHeight = m_yExtentLP*8/10;
		m_nLineWidth = nMetricsLineHeight*3/32;
		break;
	case BS_MEDIUM:
		m_nHeight = m_yExtentLP*9/10;
		m_nLineWidth = nMetricsLineHeight*4/32;
		break;
	case BS_LARGE:
		m_nHeight = m_yExtentLP;
		m_nLineWidth = nMetricsLineHeight*5/32;
		break;
	}

	// m_nLineWidth must be at least 1, otherwise it looks funny
	// for the case of 1 interlinear line in 100%.
	if ((m_nLineWidth/2) < 2) {
		m_nLineWidth = 1;
	}

	m_nSerifLength = m_yExtentLP*1/10;

	// Ensure a minimum length.
	// This looks good in 100%...
	if (m_nSerifLength < 4*m_nLineWidth) {
		m_nSerifLength = 4*m_nLineWidth;
	}
	


	m_nArrowLength = nMetricsLineHeight*6/10;
	m_nInnerGap = nMetricsLineHeight/6;
	//	m_nOuterGap = nMetricsLineHeight/6;
	m_nOuterGap = 0;
	m_nArrowLineWidth = nMetricsLineHeight*1/10;
}


//////////////////////////////////////////////////////////
//
// LayoutLine
//
//////////////////////////////////////////////////////////

LayoutLine::LayoutLine(wxDC *pDC, ViewMetrics *pViewMetrics, int nNumIndents, bool bOwnsBoxes, bool bIsRightToLeft, bool bIsCentered)
	: LayoutItem(pViewMetrics),
	  m_indent(nNumIndents),
	  m_bIsRightToLeft(bIsRightToLeft), m_bIsCentered(bIsCentered),
	  m_bOwnsBoxes(bOwnsBoxes)
{
	(void)(pDC); // Silence a warning
	
	// x-extent is at least indent * paragraph-indent
	m_xExtentLP = m_indent*m_pMetrics->xParagraphIndentLP;
}

LayoutLine::~LayoutLine()
{
	if (m_bOwnsBoxes) {
		for (PBoxList::iterator it = m_boxes.begin(); 
		     it != m_boxes.end();
		     ++it) {
			delete *it;
		}
	}
	m_boxes.erase(m_boxes.begin(), m_boxes.end());
}

void LayoutLine::AddBox(LayoutBoxBase *pBox, PLACEMENT placement)
{
	/*
	 // Obsoleted by LayoutBoxBase::m_bSpaceAfter
	if (!m_boxes.empty()
	    && pBox->GetSpaceAfter()) {
		m_xExtentLP += m_pMetrics->spaceBetweenWordsLP;
	}
	*/

	adjustMyExtents(pBox);

	if (placement == FRONT) {
		m_boxes.push_front(pBox);
	} else {
		m_boxes.push_back(pBox);
	}
}


LayoutBoxBase *LayoutLine::RemoveBox(PLACEMENT placement)
{
	// Get box to remove, and remove it from list
	LayoutBoxBase *pToRemove = 0;
	if (placement == FRONT) {
		pToRemove = m_boxes.front();
		m_boxes.pop_front();
	} else {
		pToRemove = m_boxes.back();
		m_boxes.pop_back();
	}

	// Remove box's width from x-extent
	m_xExtentLP -= pToRemove->GetWidthLP();

	/*
	// Obsoleted by LayoutBoxBase::m_bSpaceAfter
	// Remove space between words if there are still boxes left.
	if (!m_boxes.empty()) {
	    m_xExtentLP -= m_pMetrics->spaceBetweenWordsLP;
	}
	*/

	// FIXME: yExtent might need to be reduced...
	// This would be done by rescanning the remaining boxes...

	return pToRemove;
}

void LayoutLine::adjustMyExtents(LayoutBoxBase *pBox)
{
	m_xExtentLP += pBox->GetWidthLP();

	// Set m_yExtentLP to max of m_yExtentLP and box's y-extent,
	// adding 1mm to the height by default, so as to separate the
	// lines...
	int yBoxExtentLP = pBox->GetHeightLP() + m_pMetrics->Convert10thofMMtoLP(10);
	m_yExtentLP = (m_yExtentLP < yBoxExtentLP) ? yBoxExtentLP : m_yExtentLP;

}


LayoutBoxBase *LayoutLine::GetBoxForX(int targetX)
{
	LayoutBoxBase *pResultBox = 0;

	int LeftEdgeLP = m_pMetrics->marginLP; // FIXME: This should be the actual left edge!
	int RightEdgeLP;
	if (m_pMetrics->bIsPrinting) {
		// When printing, totalWidth is the width of the
		// printing area, while marginLP is the left margin.
		RightEdgeLP = m_pMetrics->totalWidthLP + m_pMetrics->marginLP;
	} else {
		RightEdgeLP = m_pMetrics->totalWidthLP - m_pMetrics->marginLP;
	}
	int CurrentXLP;

	// Compute X coordinate to start at
	if (m_bIsRightToLeft) {
		// Start from right edge and work left
		CurrentXLP = RightEdgeLP - (m_indent * m_pMetrics->xParagraphIndentLP);
	} else {
		CurrentXLP = LeftEdgeLP + (m_indent * m_pMetrics->xParagraphIndentLP);
	}

	// For right-to-left, adjust x location to left of box
	int OldXLP = CurrentXLP;
	for (PBoxList::iterator it = m_boxes.begin();
	     it != m_boxes.end();
	     ++it) {
		LayoutBoxBase *pBox = *it;
		if (m_bIsRightToLeft) {
			CurrentXLP -= pBox->GetWidthLP();
			// The next line has been obsoleted by LayoutBoxBase::m_bSpaceAfter
			// CurrentXLP -= m_pMetrics->spaceBetweenWordsLP;
			if (CurrentXLP <= targetX
			    && targetX <= OldXLP) {
				pResultBox = pBox;
				break;
			}
		} else {
			CurrentXLP += pBox->GetWidthLP();
			// This has been obsoleted by LayoutBoxBase::m_bSpaceAfter
			//CurrentXLP += pBox->GetWidthLP() + m_pMetrics->spaceBetweenWordsLP;
			if (OldXLP <= targetX
			    && targetX <= CurrentXLP) {
				pResultBox = pBox;
				break;
			}
		}
		OldXLP = CurrentXLP;
	}
	
	return pResultBox;
}


void LayoutLine::Draw(wxDC *pDC, int xLP, int yLP, bool bAlwaysFlushLeft)
{
	int LeftEdgeLP = xLP;
	int RightEdgeLP;
	if (m_pMetrics->bIsPrinting) {
		// When printing, totalWidth is the width of the
		// printing area, while marginLP is the left margin.
		RightEdgeLP = m_pMetrics->totalWidthLP + m_pMetrics->marginLP;
	} else {
		RightEdgeLP = m_pMetrics->totalWidthLP - m_pMetrics->marginLP;
	}
	int nLineWidth = RightEdgeLP - LeftEdgeLP;
	int NewXLP;

	// Compute X coordinate to start at
	if (bAlwaysFlushLeft) {
		NewXLP = LeftEdgeLP + GetWidthLP();
	} else if (m_bIsRightToLeft) {
		// Start from right edge and work left
		NewXLP = RightEdgeLP - (m_indent * m_pMetrics->xParagraphIndentLP);
	} else {
		NewXLP = LeftEdgeLP + (m_indent * m_pMetrics->xParagraphIndentLP);
	}

	// For right-to-left, adjust x location to left of box
	for (PBoxList::iterator it = m_boxes.begin();
	     it != m_boxes.end();
	     ++it) {
		LayoutBoxBase *pBox = *it;
		if (m_bIsRightToLeft) {
			// This occurs in HTreeBox paragraphs
			if (pBox->GetWidthLP() > nLineWidth) {
				NewXLP += pBox->GetWidthLP() - nLineWidth;
			}
			NewXLP -= pBox->GetWidthLP();
		}
		
		pBox->Draw(pDC, NewXLP, yLP);
		
		if (m_bIsRightToLeft) {
			// Nothing to do
			; // NewXLP -= m_pMetrics->spaceBetweenWordsLP;
		} else {
			NewXLP += pBox->GetWidthLP();
			// NewXLP += pBox->GetWidthLP() + m_pMetrics->spaceBetweenWordsLP;
		}
	}
}

bool LayoutLine::HasNAMEAnchor(const wxString& name_attribute)
{
	for (PBoxList::iterator it = m_boxes.begin();
	     it != m_boxes.end();
	     ++it) {
		LayoutBoxBase *pBox = *it;
		if (pBox->getHTMLTagKind() == kA) {
			AnchorBox *pAnchorBox = (AnchorBox*) pBox;
			// Only return true here if this is a NAME
			// anchor, and the attribute is right.
			if (pAnchorBox->getAnchorKind() == kANAME
			    && pAnchorBox->attributeIsThis(name_attribute)) {
				return true;
			}
		}
	}

	return false;
}

void LayoutLine::toHTML(std::ostream *pOut, const std::string& filename)
{
	// Don't wrap at line-breaks, since the lines may not pertain to 
	// the width of the HTML client.
	for (PBoxList::iterator it = m_boxes.begin();
	     it != m_boxes.end();
	     ++it) {
		(*it)->toHTML(pOut, m_bIsRightToLeft, filename);
	}
}

void LayoutLine::RecalculateExtents(wxDC *pDC, bool bRecalculateBoxes)
{
	if (bRecalculateBoxes) {
		for (PBoxList::iterator it = m_boxes.begin(); 
		     it != m_boxes.end();
		     ++it) {
			(*it)->RecalculateExtents(pDC);
		}
	}


	// Do width/height
	m_xExtentLP = 0;
	m_yExtentLP = 0;
	for (PBoxList::iterator it = m_boxes.begin(); 
	     it != m_boxes.end();
	     ++it) {
		adjustMyExtents(*it);
	}
}


//////////////////////////////////////////////////////////
//
// LayoutParagraph
//
//////////////////////////////////////////////////////////


LayoutParagraph::LayoutParagraph(ViewMetrics *pViewMetrics, int xUsableWidthLP, bool bIsRightToLeft, bool bIsCentered, int indent, bool add_vertical_space_after)
	: LayoutItem(pViewMetrics),
	  m_lines(2, (LayoutLine*)0), // Start out with two nil-lines
	  m_xUsableWidthLP(xUsableWidthLP),
	  m_bIsRightToLeft(bIsRightToLeft), m_bIsCentered(bIsCentered),
	  m_bVerticalSpaceAfter(add_vertical_space_after),
	  m_indent(indent),
	  m_nVerticalSpaceAfterLP(pViewMetrics->getLineHeight(0))
{
	m_lines.resize(0); // Return to zero-size after initial 2 element size
}


LayoutParagraph::~LayoutParagraph()
{
	delete_lines();

	// Delete boxes
	for (PBoxList::iterator it2 = m_owned_boxes.begin();
	     it2 != m_owned_boxes.end();
	     ++it2) {
		delete *it2;
	}
	m_owned_boxes.erase(m_owned_boxes.begin(), m_owned_boxes.end());
	m_boxes.erase(m_boxes.begin(), m_boxes.end());
}


/*
void LayoutParagraph::AddBox(wxDC *pDC, LayoutBoxBase *pBox)
{
	AddBox(pDC, 
	       pBox, 
	       true,  // bAddToM_Boxes	
	       true); // bRecalculateExtents
}
*/

void LayoutParagraph::AddBox(wxDC *pDC, LayoutBoxBase *pBox, bool bAddToM_Boxes, bool bRecalculateExtents, bool bParagraphOwnsTheBox)
{
	if (bParagraphOwnsTheBox) {
		m_owned_boxes.push_back(pBox);
	}
	if (bAddToM_Boxes) {
		m_boxes.push_back(pBox);
	}
	
	LayoutLine *pLine = 0;
	bool bAdded = false;

	if (m_lines.empty()) {
		pLine = new LayoutLine(pDC, m_pMetrics, m_indent, false, m_bIsRightToLeft, m_bIsCentered); 
		m_lines.push_back(pLine);
	} else {
		pLine = m_lines.back();
	}

	// Try adding at back
	pLine->AddBox(pBox, BACK);

	if (pLine->GetWidthLP() > m_xUsableWidthLP) {
		pLine->RemoveBox(BACK);
		bAdded = false;
	} else {
		bAdded = true;
	}

	if (!bAdded) {
		pLine = new LayoutLine(pDC, m_pMetrics, m_indent, false, m_bIsRightToLeft, m_bIsCentered);
		m_lines.push_back(pLine);
		pLine->AddBox(pBox, BACK);
	}

	// If this is a kBR, then break with a new line
	if (pBox->getHTMLTagKind() == kBR) {
		pLine = new LayoutLine(pDC, m_pMetrics, m_indent, false, m_bIsRightToLeft, m_bIsCentered);
		m_lines.push_back(pLine);
	}

	if (bRecalculateExtents) {
		RecalculateExtents(pDC);
	}
}

void LayoutParagraph::RecalculateExtents(wxDC *pDC, bool bRecalculateBoxes)
{
	// Recalculate x-extent and y-extent 
	m_yExtentLP = 0;
	m_xExtentLP = 0;
	PLineList::const_iterator cend = m_lines.end();
	for (PLineList::const_iterator it = m_lines.begin(); 
	     it != cend;
	     ++it) {	
		LayoutLine *pLine = *it;
		pLine->RecalculateExtents(pDC, bRecalculateBoxes);
		m_yExtentLP += pLine->GetHeightLP();
		int xLineExtent = pLine->GetWidthLP();
		if (m_xExtentLP < xLineExtent) {
			m_xExtentLP = xLineExtent;
		}
	}
	if (m_bVerticalSpaceAfter) {
		m_yExtentLP += m_nVerticalSpaceAfterLP;
	}
}

LayoutLine *LayoutParagraph::GetLineForY(int targetY)
{
	LayoutLine *pResultLine = 0;
	int yExtentLP = 0;

	if (!m_lines.empty()) {
		for (PLineList::iterator it = m_lines.begin(); 
		     it != m_lines.end();
		     ++it) {
			LayoutLine *pLine = *it;
			int newY = yExtentLP + pLine->GetHeightLP();
			
			if (yExtentLP <= targetY 
			    && targetY <= newY) {
				pResultLine = pLine;
				break;
			}
			
			yExtentLP = newY;
			
		}
	}
	
	return pResultLine;
}

#define mymin(X,Y) (((X) < (Y)) ? (X) : (Y))

int LayoutParagraph::GetHeightLP(int first_line, int last_line)
{
	int yExtentLP = 0;

	int lines_size = (int) m_lines.size() - 1;
	
	if (!m_lines.empty()) {
		int myLastLine = mymin(lines_size, last_line);

		int nLineNo = first_line;
		while (nLineNo <= myLastLine) {
			LayoutLine *pLine = m_lines[nLineNo];
			
			yExtentLP += pLine->GetHeightLP();
			
			nLineNo++;
		}
	}

	if (m_bVerticalSpaceAfter && last_line >= lines_size) {
		yExtentLP += m_nVerticalSpaceAfterLP;
	}

	return yExtentLP;
}



void LayoutParagraph::Draw(wxDC *pDC, int xLP, int yLP, int first_line, int last_line, bool bFlushLeftIfOneLine)
{
	if (!m_lines.empty()) {
		int lines_size = (int) m_lines.size() - 1;
		int myLastLine = mymin(lines_size, last_line);

		if (bFlushLeftIfOneLine && ((myLastLine - first_line) == 0)) {
			int nLineNo = first_line;
			LayoutLine *pLine = m_lines[nLineNo];
			pLine->Draw(pDC, 
				    xLP, yLP, 
				    true); // bAlwaysFlushLeft
		} else {
			int nLineNo = first_line;
			while (nLineNo <= myLastLine) {
				LayoutLine *pLine = m_lines[nLineNo];
				pLine->Draw(pDC, 
					    xLP, yLP, 
					    false); // bAlwaysFlushLeft
				yLP += m_pMetrics->nMMScaleFactor()*pLine->GetHeightLP();
				++nLineNo;
			}
		}
	}
}


void LayoutParagraph::rewrap(wxDC *pDC, int xUsableWidthLP)
{
	delete_lines();
	m_xUsableWidthLP = xUsableWidthLP;
	for (PBoxList::iterator it = m_boxes.begin();
	     it != m_boxes.end();
	     ++it) {
		AddBox(pDC, 
		       *it, 
		       false, // bAddToM_Boxes 
		       false, // bRecalculateExtents
		       false); // bParagraphOwnsTheBox
		       
	}
	RecalculateExtents(pDC);
}


void LayoutParagraph::delete_lines(void)
{
	// Delete lines
	for (PLineList::iterator it = m_lines.begin(); 
	     it != m_lines.end();
	     ++it) {	
		delete *it;
	}
	m_lines.erase(m_lines.begin(), m_lines.end());
}

// Returns -1 if not found
int LayoutParagraph::GetYForNAMEAnchor(const wxString& name_attribute)
{
	int yExtentLP = 0;

	if (!m_lines.empty()) {
		for (PLineList::iterator it = m_lines.begin(); 
		     it != m_lines.end();
		     ++it) {
			LayoutLine *pLine = *it;
			int newY = yExtentLP + pLine->GetHeightLP();
			
			bool bHasNAMEAnchor = pLine->HasNAMEAnchor(name_attribute);
			if (bHasNAMEAnchor) {
				return yExtentLP;
			}
			
			yExtentLP = newY;
			
		}
	}
	
	// If we came this far, there was no such NAME anchor in this paragraph
	return -1;
}


void LayoutParagraph::toHTML(std::ostream *pOut, const std::string& filename)
{
	(*pOut) << "<div class=\"paragraph\">";

	for (PLineList::iterator it = m_lines.begin(); 
	     it != m_lines.end();
	     ++it) {	
		(*it)->toHTML(pOut, filename);
	}
	
	(*pOut) << "</div>\n";
}
 
 
//////////////////////////////////////////////////////////
//
// LayoutCanvas
//
//////////////////////////////////////////////////////////
 
 
BEGIN_EVENT_TABLE(LayoutCanvas, wxScrolledWindow)
EVT_SIZE(LayoutCanvas::OnSize)
EVT_MOUSE_EVENTS(LayoutCanvas::OnMouseEvent)
END_EVENT_TABLE()




LayoutCanvas::LayoutCanvas(wxWindow *pParent, const wxPoint& pos, const wxSize& size, const long style)
: wxScrolledWindow(pParent, -1, pos, size, style),
		   m_nMapMode(wxMM_TEXT),
		   m_nMagnification(100),
		   m_pViewMetrics(0),
		   m_iOverallHeight(0),
		   m_bParagraphEnded(false)
{
	SetCursor(wxCursor(wxCURSOR_ARROW));
	SetBackgroundColour(*wxWHITE);

	// Calculate sizes
	wxSize sizeTotal;
	wxSize sizePage;
	wxSize sizeLine;
	int height = 5;
	sizeTotal = wxSize(20, 20);
	sizePage  = wxSize(20, height*8);
	sizeLine  = wxSize(20, height);

	// Set sizes
	SetVirtualSize(sizeTotal);
	SetScrollRate(sizeLine.GetWidth(), sizeLine.GetHeight());
}



LayoutCanvas::~LayoutCanvas()
{
	Clear();
	delete m_pViewMetrics;
}



void LayoutCanvas::OnDraw(wxDC &dc)
{
	if(m_paragraphs.size() == 0) return;

	wxDC *pDC = &dc;
	pDC->GetUserScale(&m_xScaleFactor, &m_yScaleFactor);

	int y = m_pViewMetrics->marginLP;

	wxRect clipRect;
	GetClipBox(pDC, clipRect);
	int yItemHeight = 0;
	int nMMScaleFactor = m_pViewMetrics->nMMScaleFactor();
	int nYFirstClip = clipRect.GetY()*nMMScaleFactor;
	int nYLastClip = (clipRect.GetHeight() + clipRect.GetY()) *nMMScaleFactor;
	for (PParagraphList::iterator it = m_paragraphs.begin();
	     it != m_paragraphs.end();
	     it++) {
		// Get item height
		yItemHeight = (*it)->GetHeightLP(0, 0x7FFFFFFF);

		// Only draw if (part of) item is after top of clipbox
		if ((y + nMMScaleFactor*yItemHeight)*nMMScaleFactor >= nYFirstClip) {
			DrawItem(pDC, *it, y);
		}

		// Update y
		y += yItemHeight*nMMScaleFactor;

		// If we have processed past the clipbox, don't do any more
		if (y*nMMScaleFactor > nYLastClip) {
			break;
		}
	}
}

void LayoutCanvas::DrawItem(wxDC *pDC, LayoutParagraph *pPara, int y)
{
	pPara->Draw(pDC, 
		    m_pViewMetrics->marginLP,  // FIXME: Add better-looking x
		    y, 
		    0, 0x7FFFFFFF,  // FIXME: Add smarter first_linst, last_line
		    false); // bFlushLeftIfOneLine
}

void LayoutCanvas::OnMouseEvent(wxMouseEvent& event)
{
	if (event.ButtonDown()) {
		wxClientDC dc(this);
		wxPoint point = event.GetLogicalPosition(dc);

		wxCoord lpX, lpY;
		this->CalcUnscrolledPosition(point.x, point.y, &lpX, &lpY);
		wxPoint pointLP(lpX, lpY);

		HandleButtonDown(event, pointLP, event.RightIsDown());


		// As per the instructions for wxMouseEvent in the
		// wxWidgets instructions.
		// Otherwise, the underlying window won't get the
		// focus.
		event.Skip();
	} else {
		event.Skip();
	}
}


void LayoutCanvas::OnSize(wxSizeEvent& event)
{
	(void)(event); // Silence a warning
	
	// Get device context
	wxClientDC dc(this);
	wxClientDC *pDC = &dc;
	dc.SetMapMode(m_nMapMode);

	
	if (m_pViewMetrics != 0) {
		if (UpdateViewMetricsViewWidth()) {
			rewrap_paragraphs(pDC);
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
	if (m_paragraphs.size() == 0)
		height = 20;
	else
		height = (*m_paragraphs.begin())->GetHeightLP(0,1);

	int maxParagraphWidth = 0;
	if (m_paragraphs.size() == 0) {
		maxParagraphWidth = 0;
	} else {
		for (PParagraphList::iterator it = m_paragraphs.begin();
		     it != m_paragraphs.end();
		     it++) {
			// Get item width
			int yItemWidth = (*it)->GetWidthLP();
			
			if (maxParagraphWidth < yItemWidth) {
				maxParagraphWidth = yItemWidth;
			}
		}
	}
	
	int width = (maxParagraphWidth < MIN_WIDTH) ? MIN_WIDTH : maxParagraphWidth;

	// Calculate sizes
	wxSize sizeTotal;
	wxSize sizePage;
	wxSize sizeLine;
	if (m_pViewMetrics) {
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



void LayoutCanvas::ChangeMagnification(int magnification)
{
	if (magnification == m_nMagnification) {
		return;
	} else {
		wxClientDC dc(this);
		m_nMagnification = magnification;
		ASSERT_THROW(m_pViewMetrics != 0, "pViewMetrics was 0");
		m_pViewMetrics->LoadFonts(m_nMagnification, &dc);
		m_pViewMetrics->Initialize(&dc);
		m_pViewMetrics->RecalculateInterlinearLineHeight(&dc);
		RecalculateParagraphExtents();
		rewrap_paragraphs(&dc);
		flush();
	}
}



void LayoutCanvas::InitializeViewMetrics()
{
	// Load fonts
	wxClientDC dc(this);
	m_pViewMetrics->LoadFonts(m_nMagnification, &dc);

	// Initialize
	m_pViewMetrics->Initialize(&dc);

	// Calculate interlinear height
	m_pViewMetrics->RecalculateInterlinearLineHeight(&dc);

	// Update ViewMetrics with client width & height
	int nWidth, nHeight;
	this->GetClientSize(&nWidth, &nHeight);
	wxScreenDC screenDC;
	m_pViewMetrics->Update(&screenDC, nWidth, nHeight);

	// Update ViewMetrics with view width
	UpdateViewMetricsViewWidth();

	m_pViewMetrics->SetViewWidthMM(120);
}


bool LayoutCanvas::UpdateViewMetricsViewWidth(void)
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


void LayoutCanvas::rewrap_paragraphs(wxDC *pDC)
{
	for (PParagraphList::iterator it = m_paragraphs.begin(); 
	     it != m_paragraphs.end();
	     ++it) {	
		(*it)->rewrap(pDC, m_pViewMetrics->displayWidthLP);
	}
}



void LayoutCanvas::Clear(void)
{
	PParagraphList::iterator it = m_paragraphs.begin();
	while (it != m_paragraphs.end()) {
		delete *it;
		*it = 0;
		++it;
	}
	m_paragraphs.erase(m_paragraphs.begin(), m_paragraphs.end());
}



void LayoutCanvas::CalculateOverallHeight()
{
	// This is necessary because CalculateOverallHeight
	// is called from LayoutCanvas::OnSize(),
	// which gets called before we have had a chance
	// to InitializeViewMetrics().
	if (m_pViewMetrics == 0) {
		m_iOverallHeight = 0;
	} else {
		m_iOverallHeight = m_pViewMetrics->marginLP;
	}

	for (PParagraphList::iterator it = m_paragraphs.begin(); 
	     it != m_paragraphs.end();
	     ++it) {	
		m_iOverallHeight += (*it)->GetHeightLP(0, 0x7FFFFFFF);
	}
}


void LayoutCanvas::GetClipBox(wxDC *pDC, wxRect& clipRect)
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



void LayoutCanvas::AddParagraph(LayoutParagraph *pParagraph)
{
	m_paragraphs.push_back(pParagraph);
	m_bParagraphEnded = false;
}

void LayoutCanvas::AddParagraph(bool bIsRightToLeft, bool bIsCentered, int indent, bool bAdd_vertical_space_after)
{
	LayoutParagraph *pPara = 
		new LayoutParagraph(m_pViewMetrics, 
				    m_pViewMetrics->displayWidthLP, 
				    bIsRightToLeft,
				    bIsCentered,
				    indent,
				    bAdd_vertical_space_after);
	m_paragraphs.push_back(pPara);
	m_bParagraphEnded = false;
}

/*
void LayoutCanvas::AddBox(LayoutBoxBase *pBox, bool bParagraphOwnsTheBox)
{
	wxClientDC dc(this);
	AddBox(&dc, pBox, bParagraphOwnsTheBox, true);
}
*/

void LayoutCanvas::AddBox(wxDC *pDC, LayoutBoxBase *pBox, bool bRecalculateParagraphExtentsNow, bool bParagraphOwnsTheBox)
{
	if (m_paragraphs.empty()
	    || m_bParagraphEnded) {
		AddParagraph(false, // FIXME: Add real right-to-left
			     false,
			     0, // Indent
			     false); // bAdd_vertical_space_after
	}
	m_paragraphs.back()->AddBox(pDC, pBox, true, bRecalculateParagraphExtentsNow, bParagraphOwnsTheBox);
}


/*
void LayoutCanvas::AddLatinLayoutBox(const wxString& str, wxColour color)
{
	wxClientDC dc(this);
	LatinLayoutBox *pBox = new LatinLayoutBox(&dc, 
						  str,
						  m_pViewMetrics,
						  color,
						  true); // bSpaceAfter
	AddBox(&dc, pBox, );
}
*/

 /*
LayoutBoxBase *LayoutCanvas::AddLayoutBox(const wxstringvec_t& strings, bool bIsSelected, bool bIsRightToLeft)
{
	wxClientDC dc(this);
	return AddLayoutBox(&dc, strings, bIsSelected, bIsRightToLeft, true);
}
 */

  /*
LayoutBoxBase *LayoutCanvas::AddLayoutBox(wxDC *pDC, const wxstringvec_t& strings, bool bIsSelected, bool bIsRightToLeft, bool bRecalculateParagraphExtentsNow)
{
	AddBox(pDC, pBox, bRecalculateParagraphExtentsNow);
	return pBox;
}
  */

void LayoutCanvas::RecalculateParagraphExtents()
{
	wxClientDC dc(this);
	PParagraphList::const_iterator it = m_paragraphs.begin();
	PParagraphList::const_iterator cend = m_paragraphs.end();
	LayoutParagraph *pPara = 0;
	while (it != cend) {
		pPara = *it;
		pPara->RecalculateExtents(&dc);
		++it;
	}
}


void LayoutCanvas::AddLayoutBracketBox(bool opening, bool is_inside, 
				       wxString label, 
				       Bracket_Size bs, 
				       Bracket_Type bt,
				       bool bIsRightToLeft,
				       bool bIsSelected,
				       bool bRecalculateParagraphExtentsNow)
{
	wxClientDC dc(this);
	LayoutBracketBox *pBox = new LayoutBracketBox(&dc, m_pViewMetrics,
						      opening, is_inside, 
						      label, 
						      bs, 
						      bt,
						      bIsRightToLeft,
						      true); // SpaceAfter
	pBox->SetSelected(bIsSelected);
	AddBox(&dc, 
	       pBox, 
	       bRecalculateParagraphExtentsNow,
	       true); // bParagraphOwnsTheBox
}


void LayoutCanvas::flush()
{
	wxSizeEvent event;
	OnSize(event);
}

LayoutBoxBase *LayoutCanvas::GetBoxForPoint(wxPoint point)
{
	LayoutBoxBase *pBox = 0;
	PParagraphList::iterator it = m_paragraphs.begin();
	int y = m_pViewMetrics->marginLP;
	LayoutParagraph *pPara = 0;
	int targetY = point.y;
	while (it != m_paragraphs.end()) {
		int paragraph_height = (*it)->GetHeightLP(0, 0x7fffffff);
		int newY = y + paragraph_height;
		if (y <= targetY
		    && targetY <= newY) {
			pPara = *it;
			break;
		}
		y = newY;
		++it;
	}
       
	if (pPara != 0) {
		LayoutLine *pLayoutLine = pPara->GetLineForY(targetY - y);
		if (pLayoutLine != 0) {
			pBox = pLayoutLine->GetBoxForX(point.x);
		}
	}

	return pBox;
}

// Returns true on HREF-anchor at point, and sets href_attribute if it is.
bool LayoutCanvas::CheckForHREFAnchorAtPoint(wxPoint point, wxString& href_attribute)
{
	LayoutBoxBase *pBox = GetBoxForPoint(point);

	if (pBox == 0) {
		return false;
	} else if (pBox->getHTMLTagKind() == kHTMLTagNone) {
		return false;
	} else {
		HTMLBox* pHTMLBox = (HTMLBox*) pBox;
		AnchorBox *pAnchorBox = pHTMLBox->getEnclosingHREFAnchor();
		if (pAnchorBox == 0) {
			return false;
		} else {
			href_attribute = pAnchorBox->getAttribute();
			return true;
		}
	}
}

// Returns -1 if not found
int LayoutCanvas::GetYForNAMEAnchor(const wxString& name_attribute)
{
	PParagraphList::iterator it = m_paragraphs.begin();
	int y = m_pViewMetrics->marginLP;
	LayoutParagraph *pPara = 0;
	while (it != m_paragraphs.end()) {
		pPara = *it;
		int paragraph_height = pPara->GetHeightLP(0, 0x7fffffff);
		int newY = y + paragraph_height;
		int yNAMEAnchor = pPara->GetYForNAMEAnchor(name_attribute);
		if (yNAMEAnchor != -1) {
			return y + yNAMEAnchor;
		}
		y = newY;
		++it;
	}
       
	// If we came this far, the anchor was not found.
	return -1;
}


void LayoutCanvas::toHTML(std::ostream *pOut, const std::string& filename)
{
	printHTMLHeader(pOut);
	PParagraphList::iterator it = m_paragraphs.begin();
	PParagraphList::iterator end = m_paragraphs.end();
	LayoutParagraph *pPara = 0;
	while (it != end) {
		pPara = *it;
		pPara->toHTML(pOut, filename);
		++it;
	}
	printHTMLFooter(pOut);
}


#define DEFAULT_POINT_SIZE (12)

void printHTMLBracketDivCSS(std::ostream *pOut, 
			    Bracket_Direction bracket_direction, 
			    Bracket_Type bracket_type, 
			    bool bIsRightToLeft)
{
	std::string strBracketType;
	std::string border_type;
	switch (bracket_type) {
	case BT_BRACKET:
		strBracketType = "bracket";
		border_type = "solid";
		break;
	case BT_DOUBLE_BRACKET:
		strBracketType = "doublebracket";
		border_type = "double";
		break;
	case BT_PARENTHESIS:
		strBracketType = "parenthesis";
		border_type = "dashed";
		break;
	default:
		ASSERT_THROW(false,
			     "Unknown bracket type");
	}
	(*pOut) << "div.bracketunit" + strBracketType
		<< ((bracket_direction == BD_RIGHT) ? "right" : "left")
		<< ((bIsRightToLeft) ? "rtl" : "ltr")
		<< " {\n";
	(*pOut) << "float: " 
		<< ((bIsRightToLeft) ? "right" : "left")
		<<";\n";
	(*pOut) << "margin-bottom: 10pt;\n"
		<< "border-bottom: " << border_type << " black;\n"
		<< "border-top: " << border_type << " black;\n"
		<< "margin-left: 6pt;\n"
		<< "margin-right: 6pt;\n";
	if (bracket_direction == BD_LEFT) {
		(*pOut) << "border-left: " << border_type << " black;\n"
			<< "border-right: none;\n";
	} else { // BD_RIGHT
		(*pOut) << "border-right: " << border_type << " black;\n"
			<< "border-left: none;\n";
	}

		       
	(*pOut) << "}\n";
}

 
void LayoutCanvas::printHTMLHeader(std::ostream *pOut)
{
	(*pOut) << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/TD/xhtml11.dtd\">\n";
	(*pOut) << "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n";
	(*pOut) << "<head>\n";
	(*pOut) << "<title>Output from Emdros Query Tool</title>\n";
	(*pOut) << "<meta http-equiv=\"Content-type\" content='text/html; charset=\"utf-8\"' />\n";
	
	// Do style-sheet
	(*pOut) << "<style>\n";
	(*pOut) << "div.unitltr {\n"
		<< "margin-bottom: 1em;\n"
		<< "float: left;\n"
		<< "color: #EEE;\n"
		<< "}\n";
	(*pOut) << "div.unitrtl {\n"
		<< "margin-bottom: 1em;\n"
		<< "float: right;\n"
		<< "color: #EEE;\n"
		<< "}\n";
	(*pOut) << "div.unitltrselected {\n"
		<< "margin-bottom: 1em;\n"
		<< "float: left;\n"
		<< "color: #000;\n"
		<< "background-color: #EEE;\n"
		<< "}\n";
	(*pOut) << "div.unitrtlselected {\n"
		<< "margin-bottom: 1em;\n"
		<< "float: right;\n"
		<< "color: #000;\n"
		<< "background-color: #EEE;\n"
		<< "}\n";
	(*pOut) << "div.paragraph {\n"
		<< "float: none;\n"
		<< "clear: both;\n"
		<< "margin-bottom: 6pt;\n"
		<< "}\n";
	printHTMLBracketDivCSS(pOut, BD_LEFT, BT_PARENTHESIS, false /* bRightToLeft */);
	printHTMLBracketDivCSS(pOut, BD_LEFT, BT_PARENTHESIS, true /* bRightToLeft */);
	printHTMLBracketDivCSS(pOut, BD_RIGHT, BT_PARENTHESIS, false /* bRightToLeft */);
	printHTMLBracketDivCSS(pOut, BD_RIGHT, BT_PARENTHESIS, true /* bRightToLeft */);
	printHTMLBracketDivCSS(pOut, BD_LEFT, BT_BRACKET, false /* bRightToLeft */);
	printHTMLBracketDivCSS(pOut, BD_LEFT, BT_BRACKET, true /* bRightToLeft */);
	printHTMLBracketDivCSS(pOut, BD_RIGHT, BT_BRACKET, false /* bRightToLeft */);
	printHTMLBracketDivCSS(pOut, BD_RIGHT, BT_BRACKET, true /* bRightToLeft */);
	printHTMLBracketDivCSS(pOut, BD_LEFT, BT_DOUBLE_BRACKET, false /* bRightToLeft */);
	printHTMLBracketDivCSS(pOut, BD_LEFT, BT_DOUBLE_BRACKET, true /* bRightToLeft */);
	printHTMLBracketDivCSS(pOut, BD_RIGHT, BT_DOUBLE_BRACKET, false /* bRightToLeft */);
	printHTMLBracketDivCSS(pOut, BD_RIGHT, BT_DOUBLE_BRACKET, true /* bRightToLeft */);

	for (int ILMindex = 0;
	     ILMindex < m_pViewMetrics->getNoOfILMs();
	     ++ILMindex) {
		InterlinearLineMetrics *pILM = m_pViewMetrics->getILM(ILMindex);
		wxString strFontName = pILM->strFontName;
		wxColour color = pILM->m_color;

		// magnification is a percentage
		int pointSize = (int) ((DEFAULT_POINT_SIZE * m_nMagnification) / 100);

		std::string name = "p.unitline" + long2string((long) ILMindex);
		printDTDParagraphEntry(pOut, name, strFontName, color, pointSize);
	}



	(*pOut) << "div.latinunitltr {\n"
		<< "float: left;\n"
		<< "margin-bottom: 1em;\n"
		<< "color: #EEE;\n"
		<< "}\n";
	(*pOut) << "div.latinunitrtl {\n"
		<< "float: right;\n"
		<< "margin-bottom: 1em;\n"
		<< "color: #EEE;\n"
		<< "}\n";
	(*pOut) << "div.latinunitltrselected {\n"
		<< "float: left;\n"
		<< "margin-bottom: 1em;\n"
		<< "color: #000;\n"
		<< "background-color: #EEE;\n"
		<< "}\n";
	(*pOut) << "div.latinunitrtlselected {\n"
		<< "float: right;\n"
		<< "margin-bottom: 1em;\n"
		<< "color: #000;\n"
		<< "background-color: #EEE;\n"
		<< "}\n";
	printDTDParagraphEntry(pOut, "p.latinunitline", wxString(wxT("Times New Roman\", \"Arial\", \"Helvetica\", \"serif")), *wxBLACK, DEFAULT_POINT_SIZE);


	(*pOut) << "</style>\n";

	(*pOut) << "</head>\n"
		<< "<body>\n";
	
}

void LayoutCanvas::printHTMLFooter(std::ostream *pOut)
{
	(*pOut) << "</body>\n"
		<< "</html>";
}

void LayoutCanvas::printDTDParagraphEntry(std::ostream *pOut, std::string name, wxString strFontName, wxColour color, int pointsize)
{
	(*pOut) << name << " {\n"
		<< "font-size: " << long2string((long) pointsize) << "pt;\n"
		<< "font-family: \"" << std::string((const char*)strFontName.mb_str(wxConvUTF8)) << "\";\n"
		<< "color: #" 
		<< char2hex(color.Red())
		<< char2hex(color.Green())
		<< char2hex(color.Blue())
		<< ";\n"
		<< "margin: 0em;\n"
		<< "padding: 0em " << (pointsize/4) << "pt;\n"
		<< "}\n";
	(*pOut) << name << "selected {\n"
		<< "font-size: " << long2string((long) pointsize) << "pt;\n"
		<< "font-family: \"" << std::string((const char*)strFontName.mb_str(wxConvUTF8)) << "\";\n"
		<< "color: #000000;\n" 
		<< "background-color: #EEEEEE;\n"
		<< "margin: 0em;\n"
		<< "padding: 0em " << (pointsize/4) << "pt;\n"
		<< "}\n";
}


//////////////////////////////////////////////////////////
//
// HTMLBox
//
//////////////////////////////////////////////////////////

HTMLBox::HTMLBox(ViewMetrics *pMetrics, bool bSpaceAfter)
	: LayoutBoxBase(pMetrics, bSpaceAfter, false, false),
	  m_pEnclosingHREFAnchor(0)
{
}



//////////////////////////////////////////////////////////
//
// ZeroWidthHTMLBox
//
//////////////////////////////////////////////////////////

ZeroWidthHTMLBox::ZeroWidthHTMLBox(ViewMetrics *pMetrics, eHTMLTagKind kind, bool bStartTag)
	: HTMLBox(pMetrics, false),
	  m_kind(kind),
	  m_bStartTag(bStartTag)
{
	if (m_kind == kBR) {
	       	m_yExtentLP = pMetrics->getLatinFontHeight();
	}
}

void ZeroWidthHTMLBox::toHTML(std::ostream *pOut, bool bIsRightToLeft, const std::string& filename)
{
	(void)(bIsRightToLeft); // Silence a warning
	(void)(filename); // Silence a warning
	
	if (m_bStartTag) {
		(*pOut) << '<';
	} else {
		(*pOut) << "</";
	}

	switch (m_kind) {
	case kH1:
		(*pOut) << "h1>";
		break;
	case kH2:
		(*pOut) << "h2>";
		break;
	case kB:
		(*pOut) << "b>";
		break;
	case kEM:
		(*pOut) << "em>";
		break;
	case kU:
		(*pOut) << "u>";
		break;
	case kSUP:
		(*pOut) << "sup>";
		break;
	case kBR:
		(*pOut) << "br>";
		break;
	case kHR:
		(*pOut) << "hr>";
		break;
	case kP:
		(*pOut) << "p>";
		break;
	case kA:
		ASSERT_THROW(false, std::string("I do not know how to deal with A in this method."));
		break;
	case kHTMLText:
		ASSERT_THROW(false, std::string("I do not know how to deal with kHTMLText in this method."));
		break;
	case kHTMLTagNone:
		ASSERT_THROW(false, std::string("I do not know how to deal with kHTMLTagNone in this method."));
		break;
	}
}

//////////////////////////////////////////////////////////
//
// HTMLTextBox
//
//////////////////////////////////////////////////////////



HTMLTextBox::HTMLTextBox(ViewMetrics *pMetrics, wxDC *pDC, const HTMLFontAttributes& font_attr, const wxString& text, bool bSpaceAfter)
	: HTMLBox(pMetrics, bSpaceAfter),
	  m_text(text),
	  m_font_attr(font_attr)
{
	RecalculateExtents(pDC);
}

HTMLTextBox::~HTMLTextBox()
{
}

void HTMLTextBox::RecalculateExtents(wxDC *pDC)
{
	// Get extent
	wxCoord nWidth, nHeight;
	wxCoord nDescent, nExternalLeading;
	wxFont *pFont = m_pMetrics->getFontFromHTMLFontAttributes(m_font_attr, true);
	pDC->SetFont(*pFont);
	pDC->GetTextExtent(m_text,
			   &nWidth, &nHeight,
			   &nDescent, &nExternalLeading);
	pDC->SetFont(wxNullFont);
	
	m_xExtentLP = nWidth;

	m_yExtentLP = nHeight + nExternalLeading;

	if (m_bSpaceAfter) {
		m_xExtentLP += m_pMetrics->spaceBetweenWordsLP;
	}

	m_nUnderlinePosition = nHeight - nDescent;
}


void HTMLTextBox::Draw(wxDC *pDC, int xLP, int yLP)
{	
	// Set font and colors
	wxFont *pFont = m_pMetrics->getFontFromHTMLFontAttributes(m_font_attr, true);
	pDC->SetFont(*pFont);
	pDC->SetTextBackground(*wxWHITE);
	pDC->SetTextForeground(m_font_attr.m_color);
	
	pDC->DrawText(m_text, xLP, yLP);
	pDC->SetTextForeground(*wxBLACK);
	pDC->SetFont(wxNullFont);

	if (m_font_attr.m_bUnderline) {
		int yUnderline = yLP + m_pMetrics->nMMScaleFactor()*m_nUnderlinePosition;
#if wxCHECK_VERSION(3,0,0)
		pDC->SetPen(wxPen(m_font_attr.m_color, 1, wxPENSTYLE_SOLID));
#else
		pDC->SetPen(wxPen(m_font_attr.m_color, 1, wxSOLID));
#endif
		pDC->DrawLine(xLP, yUnderline, xLP + m_xExtentLP, yUnderline);
		pDC->SetPen(wxNullPen);
	}
}

void HTMLTextBox::toHTML(std::ostream *pOut, bool bIsRightToLeft, const std::string& filename)
{
	(void)(bIsRightToLeft); // Silence a warning
	(void)(filename); // Silence a warning
	
	std::string text = (const char*) m_text.mb_str(wxConvUTF8);

	// FIXME: Mangle entities!
	(*pOut) << text;

	if (m_bSpaceAfter) {
		(*pOut) << ' ';
	}
}

//////////////////////////////////////////////////////////
//
// HTMLHRBox
//
//////////////////////////////////////////////////////////


HTMLHRBox::HTMLHRBox(ViewMetrics *pMetrics)
	: HTMLBox(pMetrics, false)
{
}


HTMLHRBox::~HTMLHRBox()
{
}


void HTMLHRBox::Draw(wxDC *pDC, int xLP, int yLP)
{
#if wxCHECK_VERSION(3,0,0)
	pDC->SetPen(wxPen(GRAY, 2, wxPENSTYLE_SOLID));
#else
	pDC->SetPen(wxPen(GRAY, 2, wxSOLID));
#endif
	pDC->DrawLine(xLP,
		      yLP, 
		      m_pMetrics->totalWidthLP - m_pMetrics->marginLP, 
		      yLP);
}


void HTMLHRBox::toHTML(std::ostream *pOut, bool bIsRightToLeft, const std::string& filename)
{
	(void)(bIsRightToLeft); // Silence a warning
	(void)(filename); // Silence a warning
	
	(*pOut) << "<hr>";
}




//////////////////////////////////////////////////////////
//
// AnchorBox
//
//////////////////////////////////////////////////////////

AnchorBox::AnchorBox(ViewMetrics *pMetrics, eAnchorKind kind, const wxString& attribute)
	: ZeroWidthHTMLBox(pMetrics, kA, true),
	  m_anchor_kind(kind),
	  m_attribute(attribute)
{
}


AnchorBox::AnchorBox(ViewMetrics *pMetrics)
	: ZeroWidthHTMLBox(pMetrics, kA, false),
	  m_anchor_kind(kAEnd)
{
}

void AnchorBox::toHTML(std::ostream *pOut, bool bIsRightToLeft, const std::string& filename)
{
	(void)(bIsRightToLeft); // Silence a warning
	(void)(filename); // Silence a warning
	
	if (m_bStartTag) {
		switch (m_anchor_kind) {
		case kANAME:
			// FIXME: Mangle string with HTML entities &gt;, &amp; etc.
			(*pOut) << "<A NAME=\"" << ((const char*)(m_attribute.mb_str(wxConvUTF8))) << "\">";
			break;
		case kAHREF:
			// FIXME: Mangle string with HTML entities &gt;, &amp; etc.
			(*pOut) << "<A HREF=\"" << ((const char*)(m_attribute.mb_str(wxConvUTF8))) << "\">";
			break;
		case kAEnd:
			ASSERT_THROW(false, "AnchorBox::toHTML(): kind is kAEnd, but m_bIsStartTag is also true. Conflict!");
			break;
		}
	} else {
		(*pOut) << "</A>";
	}
}

//////////////////////////////////////////////////////////
//
// QDHTMLHandler
//
//////////////////////////////////////////////////////////


QDHTMLHandler::QDHTMLHandler(HTMLCanvas *pCanvas)
	: m_chars(""),
	  m_font_attributes(pCanvas->m_pViewMetrics->topFontAttributeStack()),
	  m_bInBody(false),
	  m_bInParagraph(false)
{
	m_pCanvas = pCanvas;
	m_pMetrics = pCanvas->m_pViewMetrics;
	m_pDC = new wxClientDC(pCanvas);
	m_href_anchor_stack.push(0);
}


QDHTMLHandler::~QDHTMLHandler()
{
	delete m_pDC;

	// We don't need to delete the contents, as we don't own them!
	while (!m_href_anchor_stack.empty()) {
		m_href_anchor_stack.pop();
	}
}


void QDHTMLHandler::startElement(const std::string& tag, const AttributeMap& attrs)
{
	handleChars();

	if (tag == "a") {
		eAnchorKind anchor_kind = kANAME;
		std::string attribute;
		if (attrs.find("name") != attrs.end()) {
			anchor_kind = kANAME;
			attribute = getAttribute(attrs, "name");
			m_font_attributes = m_pMetrics->startAnchorNAME();
		} else if (attrs.find("href") != attrs.end()) {
			anchor_kind = kAHREF;
			attribute = getAttribute(attrs, "href");
			m_font_attributes = m_pMetrics->startAnchorHREF();
		} else {
			throw QDException(std::string("Anchor start-tag <a ....> without either href or name! Please add either href or name"));

		}
		AnchorBox *pABox = new AnchorBox(m_pMetrics,
						 anchor_kind,
						 wxString(attribute.c_str(), wxConvUTF8));
		if (anchor_kind == kAHREF) {
			m_href_anchor_stack.push(pABox);
		} else {
			m_href_anchor_stack.push(m_href_anchor_stack.top());
		}
		m_pCanvas->AddBox(m_pDC, pABox,
				  false, // Recalculate paragraph extents now
				  true); // Paragraph owns box
	} else if (tag == "p") {
		m_pCanvas->AddParagraph(false, // bIsRightToLeft
					false, // bIsCentered
					0,     // Indent
					true); // bAdd_vertical_space_after
		m_bInParagraph = true;
	} else if (tag == "br") {
		ZeroWidthHTMLBox *pZWBox = new ZeroWidthHTMLBox(m_pMetrics,
								kBR,
								true);
		m_pCanvas->AddBox(m_pDC, pZWBox,
				  false, // Recalculate paragraph extents now
				  true); // Paragraph owns box
	} else if (tag == "b") {
		m_font_attributes = m_pMetrics->startBold();
		/*
		ZeroWidthHTMLBox *pZWBox = new ZeroWidthHTMLBox(m_pMetrics,
								kB,
								true);
		m_pCanvas->AddBox(m_pDC, pZWBox,
				  false, // Recalculate paragraph extents now
				  true); // Paragraph owns box
		*/
	} else if (tag == "em") {
		m_font_attributes = m_pMetrics->startEm();
		/*
		ZeroWidthHTMLBox *pZWBox = new ZeroWidthHTMLBox(m_pMetrics,
								kEM,
								true);
		m_pCanvas->AddBox(m_pDC, pZWBox,
				  false, // Recalculate paragraph extents now
				  true); // Paragraph owns box
		*/
	} else if (tag == "u") {
		m_font_attributes = m_pMetrics->startUnderline();
		/*
		ZeroWidthHTMLBox *pZWBox = new ZeroWidthHTMLBox(m_pMetrics,
								kEM,
								true);
		m_pCanvas->AddBox(m_pDC, pZWBox,
				  false, // Recalculate paragraph extents now
				  true); // Paragraph owns box
		*/
	} else if (tag == "sup") {
		m_font_attributes = m_pMetrics->startSuperscript();
		/*
		ZeroWidthHTMLBox *pZWBox = new ZeroWidthHTMLBox(m_pMetrics,
								kSUP,
								true);
		m_pCanvas->AddBox(m_pDC, pZWBox,
				  false, // Recalculate paragraph extents now
				  true); // Paragraph owns box
		*/
	} else if (tag == "h1") {
		m_pCanvas->AddParagraph(false, // bIsRightToLeft
					false, // bIsCentered
					0,     // Indent
					true); // bAdd_vertical_space_after

		m_font_attributes = m_pMetrics->startH1();
		/*
		ZeroWidthHTMLBox *pZWBox = new ZeroWidthHTMLBox(m_pMetrics,
								kH1,
								true);
		m_pCanvas->AddBox(m_pDC, pZWBox,
				  false, // Recalculate paragraph extents now
				  true); // Paragraph owns box
		*/
	} else if (tag == "h2") {
		m_pCanvas->AddParagraph(false, // bIsRightToLeft
					false, // bIsCentered
					0,     // Indent
					true); // bAdd_vertical_space_after

		m_font_attributes = m_pMetrics->startH2();
		/*
		ZeroWidthHTMLBox *pZWBox = new ZeroWidthHTMLBox(m_pMetrics,
								kH2,
								true);
		m_pCanvas->AddBox(m_pDC, pZWBox,
				  false, // Recalculate paragraph extents now
				  true); // Paragraph owns box
		*/
	} else if (tag == "hr") {
		m_pCanvas->AddParagraph(false, // bIsRightToLeft
					false, // bIsCentered
					0,     // Indent
					true); // bAdd_vertical_space_after
		HTMLHRBox * pHRBox = new HTMLHRBox(m_pMetrics);
		m_pCanvas->AddBox(m_pDC, pHRBox,
				  false, // Recalculate paragraph extents now
				  true); // Paragraph owns box
		m_bInParagraph = true;
	} else if (tag == "html") {
		; // Do nothing
	} else if (tag == "head") {
		; // Do nothing
	} else if (tag == "title") {
		; // Do nothing
	} else if (tag == "meta") {
		; // Do nothing
	} else if (tag == "body") {
		m_bInBody = true;
	} else {
		throw QDException(std::string("Unknown start-tag: <") + tag + ">");
	}
}


void QDHTMLHandler::endElement(const std::string& tag)
{
	handleChars();

	if (tag == "a") {
		m_font_attributes = m_pMetrics->endTag();
		AnchorBox *pABox = new AnchorBox(m_pMetrics);
		m_pCanvas->AddBox(m_pDC, pABox,
				  false, // Recalculate paragraph extents now
				  true); // Paragraph owns box

		m_href_anchor_stack.pop();
	} else if (tag == "p") {
		; // Tell Canvas that it is to start a new paragraph
		m_pCanvas->SetParagraphEnded(true);
		m_bInParagraph = false;
	} else if (tag == "br") {
		; // Nothing to do for end-tag
	} else if (tag == "b") {
		m_font_attributes = m_pMetrics->endTag();
		/*
		ZeroWidthHTMLBox *pZWBox = new ZeroWidthHTMLBox(m_pMetrics,
								kB,
								false);
		m_pCanvas->AddBox(m_pDC, pZWBox,
				  false, // Recalculate paragraph extents now
				  true); // Paragraph owns box
		*/
	} else if (tag == "em") {
		m_font_attributes = m_pMetrics->endTag();
		/*
		ZeroWidthHTMLBox *pZWBox = new ZeroWidthHTMLBox(m_pMetrics,
								kEM,
								false);
		m_pCanvas->AddBox(m_pDC, pZWBox,
				  false, // Recalculate paragraph extents now
				  true); // Paragraph owns box
		*/
	} else if (tag == "u") {
		m_font_attributes = m_pMetrics->endTag();
		/*
		ZeroWidthHTMLBox *pZWBox = new ZeroWidthHTMLBox(m_pMetrics,
								kEM,
								false);
		m_pCanvas->AddBox(m_pDC, pZWBox,
				  false, // Recalculate paragraph extents now
				  true); // Paragraph owns box
		*/
	} else if (tag == "sup") {
		m_font_attributes = m_pMetrics->endTag();
		/*
		ZeroWidthHTMLBox *pZWBox = new ZeroWidthHTMLBox(m_pMetrics,
								kSUP,
								false);
		m_pCanvas->AddBox(m_pDC, pZWBox,
				  false, // Recalculate paragraph extents now
				  true); // Paragraph owns box
		*/
	} else if (tag == "h1") {
		m_font_attributes = m_pMetrics->endTag();
		/*
		ZeroWidthHTMLBox *pZWBox = new ZeroWidthHTMLBox(m_pMetrics,
								kH1,
								false);
		m_pCanvas->AddBox(m_pDC, pZWBox,
				  false, // Recalculate paragraph extents now
				  true); // Paragraph owns box
		*/
		m_pCanvas->SetParagraphEnded(true);
		m_bInParagraph = false;
	} else if (tag == "h2") {
		m_font_attributes = m_pMetrics->endTag();
		/*
		ZeroWidthHTMLBox *pZWBox = new ZeroWidthHTMLBox(m_pMetrics,
								kH2,
								false);
		m_pCanvas->AddBox(m_pDC, pZWBox,
				  false, // Recalculate paragraph extents now
				  true); // Paragraph owns box
		*/
		m_pCanvas->SetParagraphEnded(true);
		m_bInParagraph = false;
	} else if (tag == "hr") {
		; // Tell Canvas that it is to start a new paragraph
		m_pCanvas->SetParagraphEnded(true);
		m_bInParagraph = false;
	} else if (tag == "html") {
		; // Do nothing
	} else if (tag == "head") {
		; // Do nothing
	} else if (tag == "title") {
		; // Do nothing
	} else if (tag == "meta") {
		; // Do nothing
	} else if (tag == "body") {
		m_bInBody = false;
	} else {
		throw QDException(std::string("Unknown end-tag: </") + tag + ">");
	}
}


void QDHTMLHandler::startDocument(void)
{
	m_chars = "";
	m_pCanvas->SetParagraphEnded(false);
	m_bInBody = false;
	m_bInParagraph = false;
}


void QDHTMLHandler::endDocument(void)
{
	m_pCanvas->SetParagraphEnded(true);
	m_pCanvas->RecalculateParagraphExtents();
}

void QDHTMLHandler::addText(const std::string& str, bool bSpaceAfter)
{
	wxString strText(str.c_str(), wxConvUTF8);
	HTMLTextBox *pBox = new HTMLTextBox(m_pMetrics,
					    m_pDC,
					    m_font_attributes,
					    strText,
					    bSpaceAfter);

	pBox->setEnclosingHREFAnchor(m_href_anchor_stack.top());

	m_pCanvas->AddBox(m_pDC, pBox,
			  false, // Recalculate paragraph extents now
			  true); // Paragraph owns box
}



void QDHTMLHandler::handleChars(void)
{
	if (!m_bInBody) {
		m_chars = "";
		return; // Nothing to do when not within <body>...</body>.
	}
	if (m_chars.empty()) {
		return; // Nothing to do
	} else if (!is_other_than_whitespace(m_chars)) {
		if (m_bInParagraph) {
			addText("", true); // Empty string, with a space after it!
		}
			
		// Clean up for next text() call.
		m_chars = "";
		return;
	} else {
		if (is_whitespace(m_chars[0])) {
			addText("", true); // Empty string, with a space after it!
		}
		std::list<std::string> token_list;
		split_string(m_chars, " \n\t\r", token_list);
		std::list<std::string>::const_iterator ci = token_list.begin();
		std::list<std::string>::const_iterator cend = token_list.end();
		while (ci != cend) {
			std::string text = *ci;
			++ci;
			bool bAddSpaceAfter = true;
			if (ci == cend) {
				if (is_whitespace(m_chars[m_chars.length()-1])) {
					; // Leave it as true
				} else {
					bAddSpaceAfter = false;
				}
			}
			addText(text, bAddSpaceAfter);
		}

		// Clean up for next text() call.
		m_chars = "";
	}
}

void QDHTMLHandler::text(const std::string& str)
{
	m_chars += str;
}


//////////////////////////////////////////////////////////
//
// HTMLCanvas
//
//////////////////////////////////////////////////////////

HTMLCanvas::HTMLCanvas(wxWindow *pParent, const wxPoint& pos, const wxSize& size, const long style, ViewMetrics *pMetrics)
	: LayoutCanvas(pParent, pos, size, style)
{
	m_pViewMetrics = pMetrics;
	m_pFS = new wxFileSystem();
}


HTMLCanvas::~HTMLCanvas()
{
	delete m_pFS;
}


void HTMLCanvas::HandleButtonDown(wxMouseEvent& event, wxPoint point, bool RightButton)
{
	(void)(event); // Silence a warning
	if (!RightButton) {
		wxString anchor;
		if (CheckForHREFAnchorAtPoint(point, anchor)) {
			std::string error_message;
			if (!LoadPage(anchor, error_message)) {
				// FIXME: What to do on error?
			}
		}
	}
}

// Returns true on no error, false on error, in which case error_message has the error message.
bool HTMLCanvas::SetPage(const std::string& strHTML, /* out */ std::string& error_message)
{
	// Clear canvas
	Clear();


	bool bResult = true;
	error_message = "";
	try {

		std::cerr << "UP230: HTMLCanvas::SetPage(): " << std::endl;
		
#ifndef WIN32
		timeval t1, t2;
		gettimeofday(&t1, NULL);
#endif
	

		std::istringstream istr(strHTML);

		QDHTMLHandler dh(this);

		QDParser parser;
		parser.parse(&dh, &istr);

#ifndef WIN32
		gettimeofday(&t2, NULL);
		long long mytime = (t2.tv_usec + 1000000L * t2.tv_sec) - (t1.tv_usec + 1000000L * t1.tv_sec);
		std::cerr << "UP230: HTMLCanvas::SetPage: parsing done." << mytime << " microseconds." << std::endl;
#endif

	


	} catch (QDException& e) {
		bResult = false;
		error_message = e.what();
	} catch (...) {
		bResult = false;
		error_message = "An unknown exception occurred while parsing HTML.";
	}

	// Send an OnSize event.
	flush();

	Scroll(0,0);

	return bResult;
}



bool HTMLCanvas::LoadPage(const wxString& url, /* out */ std::string& error_message)
{
	if (url.IsEmpty()) {
		return true;
	}

	wxBusyCursor busyCursor;
	
	bool return_value = true;
	// bool bNeedsRefresh = false;
	
	if (!m_history.empty()) {
		int xDummy;
		int yScrollUnits;
		GetViewStart(&xDummy, &yScrollUnits);
		m_history[m_history_position].SetY(yScrollUnits);
	}
	
	// Local anchor.
	if (url[0] == wxT('#')) {
		wxString anchor = url.Mid(1); // 1 to the end of the string
		return_value = ScrollToAnchor(anchor, error_message);
	} else if (url.Find(wxT('#')) != wxNOT_FOUND
		   && url.BeforeFirst(wxT('#')) == m_opened_page_without_anchor) {
		wxString anchor = url.AfterFirst(wxT('#'));
		return_value = ScrollToAnchor(anchor, error_message);
	} else {
		// bNeedsRefresh = true;
		wxFSFile *pFile = m_pFS->OpenFile(url);
		if (pFile == NULL) {
			error_message = (const char*) (wxString(wxT("Could not open file with URL:")) + url).mb_str(wxConvUTF8);
			return false;
		} else {
			return_value = LoadPageFromFSFile(pFile, error_message);
		}
	}
	SetLocation(url);
	return return_value;
}


bool HTMLCanvas::LoadPageFromFSFile(wxFSFile *pFile, std::string& error_message)
{
	wxInputStream *pStream = pFile->GetStream();
	if (pStream == NULL) {
		error_message = "Could not open file";
		return false;
	}
	
	// Read contents into strContents
	wxString strContents;
	wxStringOutputStream sos(&strContents);
	pStream->Read(sos);
	
	std::string strHTML = std::string((const char*) strContents.mb_str(wxConvUTF8));
	
	return SetPage(strHTML, error_message);
}

void HTMLCanvas::SetLocation(const wxString& url)
{
	wxString location;
	if (url.Find(wxT('#')) != wxNOT_FOUND) {
		location = url.BeforeFirst(wxT('#'));
	} else {
		location = url;
	}
	m_opened_page_without_anchor = location;

	AddToHistory(url);
}


void HTMLCanvas::SetRelatedFrame(wxFrame *pFrame, const wxString& title)
{
	pFrame->SetTitle(title);
}


void HTMLCanvas::SelectAll(void)
{
	// FIXME: Implement
}


bool HTMLCanvas::HistoryCanBack(void)
{
	if (m_history.empty()) {
		return false;
	} else if (m_history_position == 0) {
		return false;
	} else {
		return true;
	}
}


bool HTMLCanvas::HistoryCanForward(void)
{
	if (m_history.empty()) {
		return false;
	} else if (m_history.size()-1 == m_history_position) {
		return false;
	} else {
		return true;
	}
}


void HTMLCanvas::HistoryBack(void)
{
	if (HistoryCanBack()) {
		--m_history_position;
		std::string error_message;
		if (!LoadPage(m_history[m_history_position].getURL(), error_message)) {
			// FIXME: What do we do on error?
		}
	}
}


void HTMLCanvas::HistoryForward(void)
{
	if (HistoryCanForward()) {
		++m_history_position;
		std::string error_message;
		if (!LoadPage(m_history[m_history_position].getURL(), error_message)) {
			// FIXME: What do we do on error?
		}
	}
}


wxString HTMLCanvas::GetOpenedPage(void)
{
	if (m_history.empty()) {
		return wxT("");
	} else {
		return m_history[m_history_position].getURL();
	}
}

bool HTMLCanvas::ScrollToAnchor(const wxString& anchor, /* out */ std::string& error_message)
{
	int yForAnchor = GetYForNAMEAnchor(anchor);
	if (yForAnchor != -1) {
		int y;
		int xScrollPixelsPerUnit, yScrollPixelsPerUnit;
		GetScrollPixelsPerUnit(&xScrollPixelsPerUnit, &yScrollPixelsPerUnit);
		y = yForAnchor / yScrollPixelsPerUnit;
		Scroll(0, y);
		return true;
	} else {
		error_message += "Could not find anchor '#" 
			+ std::string((const char*) anchor.mb_str(wxConvUTF8))
			+ "'.";
		return false;
	}
}


void HTMLCanvas::AddToHistory(const wxString& url)
{
	if (m_history.empty()) {
		m_history.push_back(URLHistoryItem(url, 0));
		m_history_position = 0;
	} else {
		const URLHistoryItem& uhi = m_history[m_history_position];
		if (uhi.getURL() == url) {
			return; 
		} else {
			if (m_history.size()-1 > m_history_position) {
				m_history.erase(m_history.begin() + m_history_position+1, 
						m_history.end());
				m_history.push_back(URLHistoryItem(url, 0));
				++m_history_position;
			} else {
				m_history.push_back(URLHistoryItem(url, 0));
				++m_history_position;
			}
		}
	}
}




int getPenWidth(int nMagnification)
{
	int nPenWidth = 1;
	if (nMagnification < 150) {
		nPenWidth = 1;
	} else if (nMagnification < 250) {
		nPenWidth = 2;
	} else if (nMagnification < 350) {
		nPenWidth = 3;
	} else if (nMagnification < 450) {
		nPenWidth = 4;
	} else if (nMagnification < 550) {
		nPenWidth = 5;
	} else if (nMagnification < 650) {
		nPenWidth = 6;
	} else if (nMagnification < 750) {
		nPenWidth = 7;
	} else if (nMagnification < 850) {
		nPenWidth = 8;
	} else {
		nPenWidth = 10;
	}
	return nPenWidth;
}
