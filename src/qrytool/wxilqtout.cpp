/*
 * wxilqtout.cpp
 *
 * Classes to do output from the query tool.
 *
 * Ulrik Petersen
 * Created: 4/16-2005
 * Last update: 4/11-2017
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2005-2017  Ulrik Sandborg-Petersen
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
#pragma implementation "wxilqtout.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wxilqtout.h"
#include <wx/event.h>
#include <wx/app.h>
#include <wx/utils.h>
#include <wx/strconv.h>
#include <wx/string.h>
#include <wx/event.h>
#include <viewmetrics.h>
#include "mqlqtwx.h"


#define NO_OF_COLORS (5)

wxColour mycolorarray[NO_OF_COLORS] = {
	BLACK,
	DARK_BLUE,
	RED,
	DARK_GREEN,
	LIGHT_GREEN
};

WXILLayoutCanvas::WXILLayoutCanvas(wxWindow *pParent, const wxPoint& pos, const wxSize& size, const long style, Configuration *pConf)
	: LayoutCanvas(pParent, pos, size, style)
{
	InitializeViewMetrics(pConf, 1);
}


WXILLayoutCanvas::~WXILLayoutCanvas()
{
}


wxString getOutputAreaFontName(Configuration *pConf, int number)
{
	wxString outputAreaFontName;
	std::ostringstream ostr;
	ostr << "output_area_font_name_" << number;
	if (pConf != 0 && pConf->has_key(ostr.str())) {
		std::string output_area_font_name = pConf->getValues(ostr.str())[0];
		outputAreaFontName = wxString(output_area_font_name.c_str(), wxConvUTF8);
	} else {
#ifdef __WXMSW__
#if wxUSE_UNICODE
		outputAreaFontName = wxString(wxT("Arial MS Unicode"));
#else
		outputAreaFontName = wxString(wxT("Courier New"));
#endif
#else  // Not Windows
		outputAreaFontName = wxString(wxT("Courier"));
#endif
	}
	return outputAreaFontName;
}

void WXILLayoutCanvas::InitializeViewMetrics(Configuration *pConf, int nInterlinearLines)
{
	if (m_pViewMetrics) {
		delete m_pViewMetrics;
	}

	m_nInterlinearLines = nInterlinearLines;

	
	// Magnification
	int nOutputAreaMagnification;
	if (pConf != 0 && pConf->has_key("output_area_magnification")) {
		std::string output_area_font_size = pConf->getValues("output_area_magnification")[0];
		std::istringstream istr(output_area_font_size);
		istr >> nOutputAreaMagnification;
	} else {
		nOutputAreaMagnification = 100;
	}
	m_nMagnification = nOutputAreaMagnification;


	// Font names
	std::vector<wxString> font_names;
	for (int index = 1; index <= m_nInterlinearLines; ++index) {
		font_names.push_back(getOutputAreaFontName(pConf, index));
	}


	// Right-to-Left
	m_bIsRightToLeft = false;
	if (pConf != 0) {
		m_bIsRightToLeft = pConf->has("option", "right_to_left");
	}

	m_pViewMetrics = new ViewMetrics(m_nInterlinearLines,
					 wxMM_TEXT,
					 m_bIsRightToLeft, 
					 false,
					 100);

	for (int index2 = 0; index2 < nInterlinearLines; ++index2) {
		InterlinearLineMetrics *pILM = 
			new InterlinearLineMetrics(font_names[index2],
						   true,
						   wxFONTENCODING_ISO8859_1,
						   mycolorarray[index2 % NO_OF_COLORS]);
		m_pViewMetrics->setILM(index2, pILM);
		
	}

	// Call base class to finish up...
	LayoutCanvas::InitializeViewMetrics();
}







WXILQTOutput::WXILQTOutput(WXILLayoutCanvas *pCanvas)
{
	m_pCanvas = pCanvas;
	m_bIsRightToLeft = false;
	m_nPrintCount = 0;
	m_bFocusIsOn = false;
	m_current_paragraph_state = ST_LATIN;
#ifdef __WXMSW__
#if wxUSE_UNICODE
	m_inputAreaFontName = wxString(wxT("Arial MS Unicode"));
#else
	m_inputAreaFontName = wxString(wxT("Courier New"));
#endif
#else  // Not Windows
	m_inputAreaFontName = wxString(wxT("Courier"));
#endif
	
	m_nInputAreaFontSize = 11;
	
#ifdef __WXMSW__
#if wxUSE_UNICODE
	m_outputAreaFontName = wxString(wxT("Arial MS Unicode"));
#else
	m_outputAreaFontName = wxString(wxT("Courier New"));
#endif
#else  // Not Windows
	m_outputAreaFontName = wxString(wxT("Courier"));
#endif

	m_pCurrentTree = 0;
}


WXILQTOutput::~WXILQTOutput()
{
	// Do nothing: We certainly don't delete the LayoutCanvas
	// since we don't own it.
}


void WXILQTOutput::printWords(wxDC *pDC, const wxString& wordString)
{
	// Split it into words
	if (m_state == ST_RASTER
	    && wordString.IsSameAs(wxT(' '))) {
		// Don't print single spaces when in raster-mode!
		return;
	}
	if (wordString.Find(wxT(' ')) == -1) {
		// There was only one line (non-ended)
		printWord(pDC, wordString);
	} else {
		std::list<wxString> word_list;
		bool bEndsWithSpace = false;
		WXILQTOutput::splitString(wordString, wxT(' '), word_list, bEndsWithSpace);
		if (m_state == ST_LATIN
		    && m_bIsRightToLeft
		    && m_current_paragraph_state != ST_LATIN) {
			std::list<wxString>::reverse_iterator ci = word_list.rbegin();
			while (ci != word_list.rend()) {
				printWord(pDC, *ci);
				++ci;
			}
		} else {
			std::list<wxString>::const_iterator ci = word_list.begin();
			while (ci != word_list.end()) {
				printWord(pDC, *ci);
				++ci;
			}
		}
	}
	
}

void WXILQTOutput::printWord(wxDC *pDC, const wxString& wordString)
{
	if (m_state == ST_LATIN) {
		LatinLayoutBox *pBox =
			new LatinLayoutBox(pDC, wordString, m_pCanvas->GetViewMetrics(), *wxBLACK, true);
		m_pCanvas->AddBox(pDC,
				  pBox,
				  false, // bRecalculateParagraphExtentsNow
				  true); // bParagraphOwnsTheBox
	} else {
		wxstringvec_t stringvec;
		stringvec.push_back(wordString);
		LayoutBox *pBox =
			new LayoutBox(pDC,
				      stringvec,
				      m_pCanvas->GetViewMetrics(),
				      true,
				      m_bIsRightToLeft);
		pBox->SetSelected(m_bFocusIsOn);
		m_pCanvas->AddBox(pDC,
				  pBox,
				  false, // bRecalculateParagraphExtentsNow
				  true); // bParagraphOwnsTheBox
	} 
}

void WXILQTOutput::splitString(const wxString& instring, wxChar charToSplitOn, std::list<wxString>& outList, bool& bEndsWithCharToSplitOn)
{
        bEndsWithCharToSplitOn = false;
	wxString mystring = instring;
	if (mystring.Find(charToSplitOn) == -1) {
		// There was only one line (non-ended)
		outList.push_back(mystring);
	} else {
		int curindex = mystring.Find(charToSplitOn);
		while (curindex != -1) {
			wxString thisline = mystring.Mid(0, curindex);
			outList.push_back(thisline);
			if (curindex + 1 >= (int) mystring.Len()) {
				bEndsWithCharToSplitOn = true;
				mystring = wxT("");
			} else {
				// Get everything after the charToSplitOn
				mystring = mystring.Mid(curindex+1);
			}
			curindex = mystring.Find(charToSplitOn);
		}
		if (!mystring.IsEmpty()) {
			outList.push_back(mystring);
		}
	}
}

void WXILQTOutput::print(const std::string& toPrint)
{
	wxString mystring = wxString(toPrint.c_str(), wxConvUTF8);

	wxDC *pDC = m_pCanvas->getDC();

	// Split it into lines
	if (mystring.Find(wxT('\n')) == -1) {
		// There was only one line (non-ended)
		printWords(pDC, mystring);
	} else {
		std::list<wxString> line_list;
		bool bEndsWithNewline = false;
		WXILQTOutput::splitString(mystring, wxT('\n'), line_list, bEndsWithNewline);
		std::list<wxString>::const_iterator ci = line_list.begin();
		while (ci != line_list.end()) {
			printWords(pDC, *ci);
			++ci;
			if (ci != line_list.end()) {
				newline();
			} else {
				if (bEndsWithNewline) {
					newline();
				}
			}
		}
	}
	
	delete pDC;

	++m_nPrintCount;
	if (m_nPrintCount > 400) {
		MyYield();
		m_nPrintCount = 0;
	}
}


void WXILQTOutput::emitUnitFeatures(void)
{
	bool bDoIt = false;
	for (int index = 0; index < m_nInterlinearLines; ++index) {
		if (!m_current_unit_strings[index].IsEmpty()) {
			bDoIt = true; 
			break;
		}
	}
	if (bDoIt) {
		wxDC *pDC = m_pCanvas->getDC();
		LayoutBox *pBox = new LayoutBox(pDC,
						m_current_unit_strings,
						m_pCanvas->GetViewMetrics(),
						true, // bSpaceAfter
						m_bIsRightToLeft);
		pBox->SetSelected(m_bFocusIsOn);
		m_pCanvas->AddBox(pDC,
				  pBox,
				  false, // bRecalculateParagraphExtentsNow
				  true); // bParagraphOwnsTheBox
		delete pDC;
	}
}

void WXILQTOutput::clearUnitFeatures(void)
{
	for (int index = 0; index < m_nInterlinearLines; ++index) {
		m_current_unit_strings[index].Clear();
	}
}


void WXILQTOutput::startUnit(const std::string& object_type_name)
{
	StringStringMap::const_iterator ci = m_data_left_boundary_map.find(object_type_name);
	if (ci != m_data_left_boundary_map.end()) {
		printUnitBoundary(ci->second);
	}
}


void WXILQTOutput::startUnitFeatures(const std::string& object_type_name)
{
	clearUnitFeatures();
	if (m_data_names.find(object_type_name) != m_data_names.end()) {
		std::list<std::string>::const_iterator ci = m_data_names[object_type_name].begin();
		std::list<std::string>::const_iterator cend = m_data_names[object_type_name].end();
		int index = 0;
		while (ci != cend) {
			m_current_unit_strings[index] = wxString((*ci).c_str(), wxConvUTF8);
			++index;
			++ci;
		}

	}
}


void WXILQTOutput::endUnitFeatures(void)
{
	emitUnitFeatures();
}




void WXILQTOutput::endUnit(const std::string& object_type_name)
{
	StringStringMap::const_iterator ci = m_data_right_boundary_map.find(object_type_name);
	if (ci != m_data_right_boundary_map.end()) {
		printUnitBoundary(ci->second);
	}
}


void WXILQTOutput::endSolution(void)
{
	wxDC *pDC = m_pCanvas->getDC();
	HTMLHRBox *pBox = new HTMLHRBox(m_pCanvas->GetViewMetrics());
	m_pCanvas->AddParagraph(false, // bIsRightToLeft. This must be false, or we won't get a full HR line
				false, // bIsCentered,
				0, // indent
				true); // bAdd_vertical_space_after
	m_pCanvas->AddBox(pDC,
			  pBox,
			  false,
			  true);
	delete pDC;
}




void WXILQTOutput::printUnitBoundary(const std::string& unit_boundary)
{
	if (unit_boundary.empty()) {
		return;
	} else {
		switch(unit_boundary[0]) {
		case '(':
			m_pCanvas->AddLayoutBracketBox(true,
						       false,
						       wxT(""),
						       BS_MEDIUM, // FIXME: Add configurable size
						       BT_PARENTHESIS,
						       m_bIsRightToLeft,
						       m_bFocusIsOn, // bIsSelected
						       false); // bRecalculateParagraphExtentsNow
			break;
		case '[':
			m_pCanvas->AddLayoutBracketBox(true,
						       false,
						       wxT(""),
						       BS_MEDIUM, // FIXME: Add configurable size
						       BT_BRACKET,
						       m_bIsRightToLeft,
						       m_bFocusIsOn,
						       false); // bRecalculateParagraphExtentsNow
			break;
		case '{':
			m_pCanvas->AddLayoutBracketBox(true,
						       false,
						       wxT(""),
						       BS_MEDIUM, // FIXME: Add configurable size
						       BT_DOUBLE_BRACKET,
						       m_bIsRightToLeft,
						       m_bFocusIsOn,
						       false); // bRecalculateParagraphExtentsNow
			break;

		case ')':
			m_pCanvas->AddLayoutBracketBox(false,
						       false,
						       wxT(""),
						       BS_MEDIUM, // FIXME: Add configurable size
						       BT_PARENTHESIS,
						       m_bIsRightToLeft,
						       m_bFocusIsOn,
						       false); // bRecalculateParagraphExtentsNow
			break;
		case ']':
			m_pCanvas->AddLayoutBracketBox(false,
						       false,
						       wxT(""),
						       BS_MEDIUM, // FIXME: Add configurable size
						       BT_BRACKET,
						       m_bIsRightToLeft,
						       m_bFocusIsOn,
						       false); // bRecalculateParagraphExtentsNow

			break;
		case '}':
			m_pCanvas->AddLayoutBracketBox(false,
						       false,
						       wxT(""),
						       BS_MEDIUM, // FIXME: Add configurable size
						       BT_DOUBLE_BRACKET,
						       m_bIsRightToLeft,
						       m_bFocusIsOn,
						       false); // bRecalculateParagraphExtentsNow
			break;

		default:
			// Print it if all else fails.
			print(unit_boundary);
			break;
		}
	}
}


void WXILQTOutput::printFeature(const std::string& object_type_name,
				unsigned int feature_index,
				const std::string& feature_name,
				const std::string& feature_value)
{
	std::string new_feature_value = applyTECkit(object_type_name, feature_index, feature_name, feature_value);
	int index = getIndexOfFeature(object_type_name, feature_name);
	if (m_current_unit_strings[index].IsEmpty()) {
		m_current_unit_strings[index] = wxString(new_feature_value.c_str(), wxConvUTF8);
	
	} else {
		m_current_unit_strings[index] += wxT(' ') + wxString(new_feature_value.c_str(), wxConvUTF8);
	}
}


void WXILQTOutput::startFocus(void)
{
	m_bFocusIsOn = true;
}


void WXILQTOutput::endFocus(void)
{
	m_bFocusIsOn = false;
}

void WXILQTOutput::startLatin(void)
{
	m_state = ST_LATIN;
}

void WXILQTOutput::startRaster(void)
{
	if (m_state != ST_RASTER) {
		m_state = ST_RASTER;
		if (m_current_paragraph_state != ST_RASTER) {
			
			m_pCanvas->AddParagraph(m_bIsRightToLeft,
						false, // bIsCentered
						0,     // indent
						false); // AddVerticalSpaceAfter
			m_current_paragraph_state = ST_RASTER;
		}
	}
}


void WXILQTOutput::newline(void)
{
	if (m_state == ST_LATIN) {
		m_pCanvas->AddParagraph(false, // bIsRightToLeft
					false, // bIsCentered
					0,     // indent
					false); // AddVerticalSpaceAfter
		m_current_paragraph_state = ST_LATIN;
	} else {
		m_pCanvas->AddParagraph(m_bIsRightToLeft,
					false, // bIsCentered
					0,     // indent
					false); // AddVerticalSpaceAfter
		m_current_paragraph_state = ST_RASTER;
	}
}


void WXILQTOutput::endl(void)
{
	newline();
	//flush();
}


void WXILQTOutput::MyYield(void)
{
	wxGetApp().Yield();
}

void WXILQTOutput::flush(void)
{
	m_pCanvas->RecalculateParagraphExtents();
	// Send an OnSize event to the canvas...(!)
	m_pCanvas->flush();
}


int WXILQTOutput::getInputAreaFontSizeFromConf(Configuration *pConf)
{
	int nInputAreaFontSize = 0;
	if (pConf != 0 && pConf->has_key("input_area_font_size")) {
		std::string input_area_font_size = pConf->getValues("input_area_font_size")[0];
		std::istringstream istr(input_area_font_size);
		istr >> nInputAreaFontSize;
	} else {
		nInputAreaFontSize = 12;
	}
	
	if (nInputAreaFontSize < 7) {
		nInputAreaFontSize = 7;
	} else if (nInputAreaFontSize > 48) {
		nInputAreaFontSize = 48;
	}

	return nInputAreaFontSize;
}




bool WXILQTOutput::getConfigurationValues(Configuration *pConf, EmdrosSchema *pSchema, std::string& message)
{
	if (pConf->has_key("input_area_font_name")) {
		std::string input_area_font_name = pConf->getValues("input_area_font_name")[0];
		m_inputAreaFontName = wxString(input_area_font_name.c_str(), wxConvUTF8);
	} else {
#ifdef __WXMSW__
#if wxUSE_UNICODE
		m_inputAreaFontName = wxString(wxT("Arial MS Unicode"));
#else
		m_inputAreaFontName = wxString(wxT("Courier New"));
#endif
#else  // Not Windows
		m_inputAreaFontName = wxString(wxT("Courier"));
#endif
	}

	m_bIsRightToLeft = pConf->has("option", "right_to_left");

	m_nInputAreaFontSize = getInputAreaFontSizeFromConf(pConf);

	m_bDataTreeObjectTypeNameForNonterminals = pConf->has("data_tree_object_type_name_for_nonterminals", "true");


	if (!QTOutputBase::getConfigurationValues(pConf, pSchema, message)) {
		m_current_unit_strings.resize(m_nInterlinearLines);
		return false; 
	} else {
		m_current_unit_strings.resize(m_nInterlinearLines);
		return true;
	}
}


void WXILQTOutput::startTable(const std::string& CSS_class)
{
	UNUSED(CSS_class);
	
	// TODO: Do table implementation
	newline();
}


void WXILQTOutput::endTable(const std::string& CSS_class)
{
	UNUSED(CSS_class);
	
	// TODO: Do table implementation
	newline();
}


void WXILQTOutput::startTableRow(const std::string& CSS_class)
{
	UNUSED(CSS_class);
	
	// TODO: Do table implementation
	startRaster();
}


void WXILQTOutput::endTableRow(const std::string& CSS_class)
{
	UNUSED(CSS_class);
	
	// TODO: Do table implementation
	newline();
}


void WXILQTOutput::startTableCell(const std::string& CSS_class)
{
	UNUSED(CSS_class);
	
	// TODO: Do table implementation
}


void WXILQTOutput::endTableCell(const std::string& CSS_class)
{
	UNUSED(CSS_class);
	
	// TODO: Do table implementation
}


void WXILQTOutput::startTree(void)
{
	m_pCurrentTree = new HTreeBox(m_pCanvas->m_pViewMetrics,
				      false, // bSpaceAfter
				      m_bIsRightToLeft);
}


void WXILQTOutput::addTreeTerminal(const std::string& object_type_name,
				   id_d_t object_id_d,
				   bool bLastFeatureIsParentID_D,
				   const SetOfMonads& object_som,
				   const StringVec& features,
				   bool bFocus)
{
	id_d_t parent_id_d = 0;
	wxstringvec_t feature_strings;

	if (m_data_names.find(object_type_name) != m_data_names.end()) {
		std::list<std::string>::const_iterator ci = m_data_names[object_type_name].begin();
		std::list<std::string>::const_iterator cend = m_data_names[object_type_name].end();
		while (ci != cend) {
			feature_strings.push_back(wxString((*ci).c_str(), wxConvUTF8));
			++ci;
		}

	}

	if (features.size() != 0) {
		unsigned int index;
		unsigned int end_index = features.size() - 1;

		if (bLastFeatureIsParentID_D) {
			parent_id_d = string2id_d(features[end_index]);
			end_index -= 1;
		} else {
			parent_id_d = 0;
		}
		for (index = 0;
		     index <= end_index;
		     ++index) {
			feature_strings.push_back(wxString(features[index].c_str(), wxConvUTF8));
		}
	}
	wxClientDC dc(m_pCanvas);
	LayoutBox *pTerminalBox = new LayoutBox(&dc,
						feature_strings,
						m_pCanvas->m_pViewMetrics,
						true,
						m_bIsRightToLeft,
						true); // bIsCentered


	if (bFocus) {
		pTerminalBox->SetSelected(true);
	}


	LayoutBox *pBranchLabels = 0;

	HTerminalNode *pTerminalNode =
		new HTerminalNode(object_id_d,
				  parent_id_d,
				  object_som,
				  pBranchLabels,
				  false, // bOwnsBranchLabels,
				  pTerminalBox,
				  true); // bOwnsTerminalBox

	m_pCurrentTree->addTerminal(pTerminalNode);
}


void WXILQTOutput::addTreeNonTerminal(const std::string& object_type_name,
				      id_d_t object_id_d,
				      bool bLastFeatureIsParentID_D,
				      const SetOfMonads& object_som,
				      const StringVec& features,
				      bool bFocus)
{
	wxstringvec_t feature_strings;

	if (m_bDataTreeObjectTypeNameForNonterminals) {
		feature_strings.push_back(wxString(object_type_name.c_str(), wxConvUTF8));
	}

	if (m_data_names.find(object_type_name) != m_data_names.end()) {
		std::list<std::string>::const_iterator ci = m_data_names[object_type_name].begin();
		std::list<std::string>::const_iterator cend = m_data_names[object_type_name].end();
		while (ci != cend) {
			feature_strings.push_back(wxString((*ci).c_str(), wxConvUTF8));
			++ci;
		}

	}


	
	id_d_t parent_id_d = 0;
	if (features.size() > 0) {
		unsigned int index;
		unsigned int end_index = features.size() - 1;

		bool bGoOn = true;
		
		if (bLastFeatureIsParentID_D) {
			parent_id_d = string2id_d(features[end_index]);
			if (features.size() == 1) {
				// If the ID_D is the only one, we don't want to print it!
				bGoOn = false;
			} else {
				end_index -= 1;
			}
		} else {
			parent_id_d = 0;
		}

		if (bGoOn) {
			for (index = 0;
			     index <= end_index;
			     ++index) {
				feature_strings.push_back(wxString(features[index].c_str(), wxConvUTF8));
			}
		}
	}


	wxClientDC dc(m_pCanvas);
	BorderedStackedLatinLayoutBox * pCategoryBox = 
		new BorderedStackedLatinLayoutBox(&dc,
						  feature_strings,
						  m_pCanvas->m_pViewMetrics,
						  *wxBLACK,
						  *wxBLUE,
						  true,   // bIsCentered
						  false,  // bSpaceAfter
						  0); // First line is the category; the rest are added data, in blue.

	if (bFocus) {
		pCategoryBox->SetSelected(true);
	}

	LayoutBox *pBranchLabels = 0;

	HNonTerminalNode *pNonTerminalNode =
		new HNonTerminalNode(object_id_d,
				     parent_id_d,
				     object_som,
				     pBranchLabels,
				     false, // bOwnsBranchLabels,
				     pCategoryBox,
				     true,  // bOwnsCategoryBox
				     false); // Show branch labels below category box.

	m_pCurrentTree->addNonTerminal(pNonTerminalNode);
}


void WXILQTOutput::endTree(void)
{
	m_pCurrentTree->treeIsFinished();

	wxClientDC dc(m_pCanvas);
	//m_pCurrentTree->RecalculateExtents(&dc);
	m_pCanvas->AddBox(&dc,
			  m_pCurrentTree,
			  false, // bRecalculateParagraphExtentsNow
			  true); // bParagraphOwnsTheBox
	m_pCurrentTree = 0;
}


