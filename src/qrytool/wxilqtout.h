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

/*
 * Copyright (C) 2001-2018     Ulrik Sandborg-Petersen
 * Copyright (C) 2018-present  Sandborg-Petersen Holding ApS, Denmark
 *
 * Licensed under the MIT License.
 *
 * Please see the file COPYING in the root of the sources for more details.
 *
 */



#ifndef WXILQTOUT__H__
#define WXILQTOUT__H__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "wxilqtout.cpp"
#endif


#include "conf.h"
#include "qtoutput.h"
#include <htreecanvas.h>
#include <layout.h>
#include <wx/font.h>
#include <conf.h>
#include <map>

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
