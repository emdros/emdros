//
// dropdown_widget.h
// Ulrik Petersen
// Created: 4/14-2007
// Last update: 4/15-2007
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





#ifndef DROPDOWN_WIDGET_H_
#define DROPDOWN_WIDGET_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "dropdown_widget.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include <wx/panel.h>
#include <wx/combobox.h>
#include "leftrightchooser.h" // For, e.g., WXStringVec
////@end includes


#define ID_DROPDOWN_WIDGET_WIDGET      (10320)
#define ID_DROPDOWN_WIDGET_COMBOBOX     (10321)


class DropdownWidget : public wxPanel {
 protected:
	wxWindow *m_pParent;
	wxComboBox *m_pBox;
	WXStringVec m_strings;
	wxString m_strLabel;
	bool m_bVertical;
	DECLARE_EVENT_TABLE()
public:
	DropdownWidget(wxWindow *pParent,
		       int myid = ID_DROPDOWN_WIDGET_WIDGET,
		       bool bVertical = true, // Otherwise, horizontal
		       wxString strLabel = wxT(""), // May be empty
		       const WXStringVec& vecInitialValues = WXStringVec());
	virtual ~DropdownWidget();

	wxString getSelection(void) const;

	void setSelection(wxString selection);
	
	void setStrings(const WXStringVec& vecStrings);

	void OnSelectionChanged(wxCommandEvent& event);
protected:
	/// Creates the controls and sizers
	virtual void SelectionChanged(void);
	void CreateControls();
	void transferStringsToCB();
};


#endif
// DROPDOWN_WIDGET_H_

