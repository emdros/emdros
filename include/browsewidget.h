//
// browsewidget.h
// Ulrik Petersen
// Created: 10/24-2006
// Last update: 11/3-2010
//
//
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2006-2010  Ulrik Sandborg-Petersen
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

