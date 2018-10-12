//
// genericdlg.h
// Ulrik Petersen
// Created: 7/16-2005
// Last update: 3/27-2006
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





#ifndef GENERICDIALOG_H_
#define GENERICDIALOG_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "genericdlg.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include <wx/dialog.h>
#include <wx/radiobox.h>
#include <wx/string.h>
#include <vector>
////@end includes


class RadioBoxInfo {
 private:
	std::vector<wxString> m_choices;
	wxString m_strLabel;
	wxString m_strToolTip;
	wxString *m_pVector;
	int m_nResult;
 public:
	RadioBoxInfo(wxString strLabel,
		     std::vector<wxString>& choices,
		     wxString strToolTip);
	RadioBoxInfo(const RadioBoxInfo& other);
	~RadioBoxInfo();
	const wxString* getChoicesAsArray();
	int getNoOfChoices() const { return (int) m_choices.size(); };
	int *getResultPtr() { return &m_nResult; };
	wxString getLabel() const  { return m_strLabel; };
	wxString getToolTip() const { return m_strToolTip; };
	int getResult() const { return m_nResult; };
 private:
	void CopyChoices();

};

typedef std::vector<RadioBoxInfo> rbivec_t;


class MyGenericDialogBox : public wxDialog { 
 protected:
	rbivec_t *m_pRbivec;
 public:
  
	MyGenericDialogBox(wxWindow *pParent,
			   wxString strTitle,
			   int id,
			   rbivec_t *pChoice_vector);
	~MyGenericDialogBox();

	/// Creates the controls and sizers
	void CreateControls();


 private:
	wxRadioBox *GetRadioBox(RadioBoxInfo& rbi);
	DECLARE_EVENT_TABLE()
		};

#endif
// GENERICDIALOG_H_

