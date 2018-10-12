//
// wxorder_chooser.h
// Ulrik Petersen
// Created: 4/14-2007
// Last update: 10/3-2011
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





#ifndef ORDER_CHOOSER_H_
#define ORDER_CHOOSER_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "wxorder_chooser.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include <wx/panel.h>
#include "leftrightchooser.h" // For, e.g., WXStringVec
////@end includes


#define ID_ORDER_CHOOSER_WIDGET      (10310)
#define ID_ORDER_CHOOSER_MOVE_UP     (10311)
#define ID_ORDER_CHOOSER_MOVE_DOWN   (10312)
#define ID_ORDER_CHOOSER_LISTBOX     (10313)



class OrderChooserWidget : public wxPanel {
 protected:
	wxWindow *m_pParent;
	wxListBox *m_pListBox;
	wxButton *m_btnMoveUp;
	wxButton *m_btnMoveDown;
	wxBoxSizer *m_pMainSizer;
	wxString m_strLabel;
	WXStringVec m_strings;
	int m_nLastIndex;
	DECLARE_EVENT_TABLE()
public:
	OrderChooserWidget();
	OrderChooserWidget(wxWindow *pParent,
			   int myid = ID_ORDER_CHOOSER_WIDGET,
			   wxString strLabel = wxT(""), // May be empty
			   const WXStringVec& vecInitialValues = WXStringVec());
	virtual ~OrderChooserWidget();

	bool Create(wxWindow *pParent,
		    int myid = ID_ORDER_CHOOSER_WIDGET,
		    wxString strLabel = wxT(""), // May be empty
		    const WXStringVec& vecInitialValues = WXStringVec());


	wxString getSelection(void) const; // Returns the empty string if no selection made
	bool hasSelection(void) const; // Returns true if something is selected

	WXStringVec getStrings(void) const;
	
	void setStrings(const WXStringVec& vecStrings);

	void OnMoveUp(wxCommandEvent& event);
	void OnMoveDown(wxCommandEvent& event);
	void OnUpdateMoveUp(wxUpdateUIEvent& event);
	void OnUpdateMoveDown(wxUpdateUIEvent& event);
	void OnSelectionChanged(wxCommandEvent& event);
protected:
	/// Creates the controls and sizers
	virtual void SelectionChanged();
	void CreateControls();
	void transferStringsToLB();
};


#endif
// ORDER_CHOOSER_H_

