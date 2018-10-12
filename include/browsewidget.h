//
// browsewidget.h
// Ulrik Petersen
// Created: 10/24-2006
// Last update: 11/3-2010
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





#ifndef BROWSEWIDGET_H_
#define BROWSEWIDGET_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "browsewidget.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/version.h>
////@end includes


#define ID_BROWSE_WIDGET   (10210)
#define ID_BROWSE_BUTTON   (10211)
#define ID_BROWSE_TEXTCTRL (10212)


class BrowseWidget : public wxPanel {
	wxWindow *m_pParent;
	wxString m_strLabel;
	wxString m_strValue;
	wxTextCtrl *m_textCtrl;
	wxButton *m_btnBrowse;
	wxFlexGridSizer *m_pMainSizer;
	bool m_bEnabled;
	wxString m_strFileDialogTitle;
	wxString m_strFileDialogFilter;
	long m_nFileDialogStyle;
	DECLARE_EVENT_TABLE()
public:
	BrowseWidget(wxWindow *pParent,
		     int myid = ID_BROWSE_WIDGET,
		     bool bEnabled = true,
		     wxString strLabel = wxT(""), // May be empty
		     wxString strInitialValue = wxT(""), // May be empty
		     wxString strFileDialogTitle = wxT("Choose file"),
		     wxString strFileDialogFilter = wxT("All files (*)|*"),
#if wxCHECK_VERSION(2,8,0)
		     long nFileDialogStyle = wxFD_OPEN | wxFD_FILE_MUST_EXIST
#else
		     long nFileDialogStyle = wxOPEN | wxFILE_MUST_EXIST
#endif
);


	void setEnabled(bool bEnabled) { m_bEnabled = bEnabled; };

	wxString getValue(void);
	
	void setValue(const wxString& value);

	void setStyleOpenFileMustExist() { 
#if wxCHECK_VERSION(2,8,0)
		m_nFileDialogStyle = wxFD_OPEN | wxFD_FILE_MUST_EXIST; 
#else
		m_nFileDialogStyle = wxOPEN | wxFILE_MUST_EXIST; 
#endif

	};
	void setStyleSaveOverwritePrompt() { 
#if wxCHECK_VERSION(2,8,0)
		m_nFileDialogStyle = wxFD_SAVE | wxFD_OVERWRITE_PROMPT; 
#else
		m_nFileDialogStyle = wxSAVE | wxOVERWRITE_PROMPT; 
#endif
	};

	void OnBrowse(wxCommandEvent& event);
	void OnUpdateControls(wxUpdateUIEvent& event);
protected:
	/// Creates the controls and sizers
	void CreateControls();
};


#endif
// BROWSEWIDGET_H_

