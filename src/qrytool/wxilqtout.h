/*
 * wxilqtout.h
 *
 * Classes to do output from the query tool.
 *
 * Ulrik Petersen
 * Created: 4/16-2005
 * Last update: 5/11-2014
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2005-2014  Ulrik Sandborg-Petersen
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

#ifndef WXILQTOUT__H__
#define WXILQTOUT__H__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "wxilqtout.cpp"
#endif

#include <wx/font.h>


#include <map>

#include <emdros.h>

#include "qtoutput.h"

#include <htreecanvas.h>
#include <layout.h>

class WXILLayoutCanvas : public LayoutCanvas {
 protected:
	bool m_bIsRightToLeft;
	int m_nInterlinearLines;
 public:       
	WXILLayoutCanvas(wxWindow *pParent, const wxPoint& pos, const wxSize& size, const long style, Configuration *pConf);
	~WXILLayoutCanvas();
	virtual void InitializeViewMetrics(Configuration *pConf, int nInterlinearLines); 
	bool getIsRightToLeft(void) const { return m_bIsRightToLeft; };
};

class WXILQTOutput : public QTOutputBase {
 private:
	WXILLayoutCanvas *m_pCanvas;
	bool m_bIsRightToLeft;
	bool m_bDataTreeObjectTypeNameForNonterminals;
	int m_nPrintCount;
	wxString m_inputAreaFontName;
	int m_nInputAreaFontSize;
	wxString m_outputAreaFontName;
	int m_nOutputAreaFontSize;
	bool m_bFocusIsOn;
	wxstringvec_t m_current_unit_strings;
	HTreeBox *m_pCurrentTree;
 public:
	WXILQTOutput(WXILLayoutCanvas *pCanvas);
	~WXILQTOutput();
	virtual void print(const std::string& toPrint);
	virtual void printWords(wxDC *pDC, const wxString& wordString);
	virtual void printWord(wxDC *pDC, const wxString& wordString);
	virtual void startUnit(const std::string& object_type_name);
	virtual void startUnitFeatures(const std::string& object_type_name);
	virtual void endUnit(const std::string& object_type_name);
	virtual void endUnitFeatures(void);
	virtual void endSolution(void);
	virtual void startRaster(void);
	virtual void startLatin(void);
	virtual void printUnitBoundary(const std::string& unit_boundary);
	virtual bool getConfigurationValues(Configuration *pConf, EmdrosSchema *pSchema, std::string& message);
	virtual void printFeature(const std::string& object_type_name,
				  unsigned int feature_index,
				  const std::string& feature_name,
				  const std::string& feature_value);
	virtual void startFocus(void);
	virtual void endFocus(void);
	virtual void newline(void);
	virtual void endl(void);
	virtual void flush(void);
	virtual void MyYield(void);

	virtual void startTree(void);
	virtual void addTreeTerminal(const std::string& object_type_name,
				     id_d_t object_id_d,
				     bool bLastFeatureIsParentID_D,
				     const SetOfMonads& object_som,
				     const StringVec& features,
				     bool bFocus);
	virtual void addTreeNonTerminal(const std::string& object_type_name,
					id_d_t object_id_d,
					bool bLastFeatureIsParentID_D,
					const SetOfMonads& object_som,
					const StringVec& features,
					bool bFocus);
	virtual void endTree(void);

	virtual void startTable(const std::string& CSS_class);
	virtual void endTable(const std::string& CSS_class);
	virtual void startTableRow(const std::string& CSS_class);
	virtual void endTableRow(const std::string& CSS_class);
	virtual void startTableCell(const std::string& CSS_class);
	virtual void endTableCell(const std::string& CSS_class);

	wxString getInputAreaFontName(void) const { return m_inputAreaFontName; };
	wxString getOutputAreaFontName(void) const { return m_outputAreaFontName; };
	int getInputAreaFontSize(void) const { return m_nInputAreaFontSize; };
	int getOutputAreaFontSize(void) const { return m_nOutputAreaFontSize; };
	void emitUnitFeatures(void);
	void clearUnitFeatures(void);
	static void splitString(const wxString& mystring, wxChar charToSplitOn, std::list<wxString>& outList, bool& bEndsWithCharToSplitOn);
protected:
	int getInputAreaFontSizeFromConf(Configuration *pConf);
};



#endif // WXILQTOUT__H__
