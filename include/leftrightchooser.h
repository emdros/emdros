//
// leftrightchooser.h
// Ulrik Petersen
// Created: 8/27-2006
// Last update: 4/10-2017
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





#ifndef LEFTRIGHTCHOOSER_H_
#define LEFTRIGHTCHOOSER_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "leftrightchooser.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include <wx/listbox.h>
#include <wx/radiobox.h>
#include <wx/button.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <vector>
////@end includes

typedef std::vector<wxString> WXStringVec;

#define ID_LEFT_LISTBOX    (10200)
#define ID_RIGHT_LISTBOX   (10201)
#define ID_LEFT_LABEL      (10202)
#define ID_RIGHT_LABEL     (10203)
#define ID_LEFT_BUTTON     (10204)
#define ID_RIGHT_BUTTON    (10205)

class LeftRightChooser : public wxPanel {
	wxWindow *m_pParent;
	wxString m_leftTitle;
	wxString m_rightTitle;
	WXStringVec m_leftStrings;
	WXStringVec m_rightStrings;
	wxListBox *m_pLeftLB;
	wxListBox *m_pRightLB;
	wxFlexGridSizer *m_pMainSizer;
	bool m_bWantsOnlyOneAtRight;
public:
	LeftRightChooser(wxWindow *pParent,
			 int myid,
			 wxString leftTitle,
			 wxString rightTitle,
			 bool bWantsOnlyOneAtRight = false);
	~LeftRightChooser();

	void setLeftStrings(const WXStringVec& stringvec);
	void setRightStrings(const WXStringVec& stringvec);

	WXStringVec getLeft(void) const;
	WXStringVec getRight(void) const;

	void OnMoveLeft(wxCommandEvent& event);
	void OnMoveRight(wxCommandEvent& event);
	void OnUpdateUIMoveLeft(wxUpdateUIEvent& event);
	void OnUpdateUIMoveRight(wxUpdateUIEvent& event);
	void OnRightListBoxSelectionChanged(wxCommandEvent& event);
private:
	DECLARE_EVENT_TABLE()
protected:
	virtual void rightListBoxSelectionChanged(const WXStringVec& newValues) {
		(void)(newValues); // Silence a warning
	};
	void setLBStrings(const WXStringVec& stringvec, wxListBox *pLB);
	void moveFromLeftToRight();
	void moveFromRightToLeft();
	void moveItems(WXStringVec& fromVec, wxListBox *pFromLB,
		       WXStringVec& toVec, wxListBox *pToLB);
	/// Creates the controls and sizers
	void CreateControls();
};

#endif
// LEFTRIGHTCHOOSER_H_

