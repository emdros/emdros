/*
 * browsewidget.cpp
 *
 * A widget for "browse file and get file path" functionality
 *
 * Ulrik Petersen
 * Created: 10/24-2006
 * Last update: 4/19-2019
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


#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "browsewidget.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/validate.h"
#include "wx/filename.h"
#include <browsewidget.h>
#include <wxutil_emdros.h>
#include <string_func.h>
#include <prefix_emdros.h>
#include <emdros-lconfig.h>
#include <conf.h>



/*!
 * BrowseWidget event table definition
 */
BEGIN_EVENT_TABLE( BrowseWidget, wxPanel )
	EVT_BUTTON(ID_BROWSE_BUTTON, BrowseWidget::OnBrowse)
	EVT_UPDATE_UI(ID_BROWSE_BUTTON, BrowseWidget::OnUpdateControls)
	EVT_UPDATE_UI(ID_BROWSE_TEXTCTRL, BrowseWidget::OnUpdateControls)
END_EVENT_TABLE()

/*!
 * BrowseWidget constructor.
 */
BrowseWidget::BrowseWidget(wxWindow* parent, int myid, 
			   bool bEnabled,
			   wxString strLabel,
			   wxString strInitialValue,
			   wxString strFileDialogTitle,
			   wxString strFileDialogFilter,
			   long nFileDialogStyle)
	: wxPanel(parent, myid,
		  wxDefaultPosition,
		  wxDefaultSize,
		  wxTAB_TRAVERSAL | wxWS_EX_VALIDATE_RECURSIVELY),
	  m_strLabel(strLabel),
	  m_strValue(strInitialValue),
	  m_bEnabled(bEnabled),
	  m_strFileDialogTitle(strFileDialogTitle),
	  m_strFileDialogFilter(strFileDialogFilter),
	  m_nFileDialogStyle(nFileDialogStyle)
{
	CreateControls();
}

/*!
 * BrowseWidget creator
 */
void BrowseWidget::CreateControls()
{
	////@begin BrowseWidget content construction
	m_textCtrl = NULL;
	m_btnBrowse = NULL;
	m_pMainSizer = NULL;
  
	wxSize sizeBroad = wxDefaultSize;
	sizeBroad.SetWidth(300);

	m_pMainSizer = new wxFlexGridSizer(1, 3, 0, 0);
	m_pMainSizer->AddGrowableCol(1, 1);
	this->SetSizer(m_pMainSizer);

	//wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxHORIZONTAL);


	wxStaticText* itemStaticText13 = new wxStaticText( this, wxID_STATIC, m_strLabel, wxDefaultPosition, wxDefaultSize, 0 );
	m_pMainSizer->Add(itemStaticText13, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

	// m_pMainSizer->Add(itemBoxSizer4, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 0);

	m_textCtrl = new wxTextCtrl( this, ID_BROWSE_TEXTCTRL, m_strValue, wxDefaultPosition, sizeBroad, 0 );
	m_pMainSizer->Add(m_textCtrl, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5);

	m_btnBrowse = new wxButton( this, ID_BROWSE_BUTTON, _("Browse..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_pMainSizer->Add(m_btnBrowse, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);


	// Set validators
	m_textCtrl->SetValidator( wxTextValidator(wxFILTER_NONE, & m_strValue) );

	this->SetSizer(m_pMainSizer);

	////@end BrowseWidget content construction
}



void BrowseWidget::OnBrowse(wxCommandEvent& event)
{
	(void)(event); // Silence a warning

	wxString strDefaultDir;
	wxString strDefaultFile;

	if (m_strValue.IsEmpty()) {
		strDefaultDir = ::wxGetHomeDir();
		strDefaultFile = wxT("");
	} else {
		wxFileName fn(m_strValue);
		strDefaultDir = fn.GetPath();
		strDefaultFile = fn.GetName();
	}
	wxFileDialog dlgFileDialog(this,
				   m_strFileDialogTitle,
				   strDefaultDir,
				   strDefaultFile,
				   m_strFileDialogFilter,
				   m_nFileDialogStyle);
	int nReturn;
	if ((nReturn = dlgFileDialog.ShowModal()) == wxID_OK) {
		m_textCtrl->SetValue(dlgFileDialog.GetPath());
	}
}


void BrowseWidget::OnUpdateControls(wxUpdateUIEvent& event)
{
	event.Enable(m_bEnabled);
}

wxString BrowseWidget::getValue(void)
{
	// Call the validator's TransferFromWindow
	TransferDataFromWindow();

	return m_strValue;
}

void BrowseWidget::setValue(const wxString& value)
{
	m_strValue = value;

	// Call the validator's TransferFromWindow
	TransferDataToWindow();
}
