//
// configwizard.h
// Ulrik Petersen
// Created: 10/24-2006
// Last update: 4/14-2007
//
//
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2006-2007  Ulrik Petersen
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




#ifndef CONFIGWIZARD_H_
#define CONFIGWIZARD_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "configwizard.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include <wx/wizard.h>
#include <wx/radiobox.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/combobox.h>
#include "schema.h"
#include "emdros_environment.h"
#include "browsewidget.h"
#include "wxorder_chooser.h"
#include "conndlg.h"
////@end includes

#define ID_RADIOBOX_CREATE_EDIT_CONFIGURATION     (10220)
#define ID_GET_CONFIGURATION_FILENAME_WIZARD_PAGE (10221)

typedef enum {
	kCreateConfiguration,
	kEditConfiguration
} eCreateOrEditConfiguration;

class CreateEditConfigurationWizardPage : public wxWizardPageSimple {
	BrowseWidget *m_pBrowseWidget;
	wxRadioButton *m_pRadioCreate;
 	wxRadioButton *m_pRadioEdit;
 public:
	CreateEditConfigurationWizardPage(wxWizard *pParent,
					  eCreateOrEditConfiguration create_or_edit);
	eCreateOrEditConfiguration getCreateOrEdit(void) const;
	wxString getEditPath(void);
 protected:
	void CreateControls(eCreateOrEditConfiguration create_or_edit);
};



class GetConfigurationFilenameWizardPage : public wxWizardPageSimple {
	BrowseWidget *m_pBrowseWidget;
	wxString m_strInitialConfigFile;
	DECLARE_EVENT_TABLE()
 public:
	GetConfigurationFilenameWizardPage(wxWizard *pParent,
					   wxString strInitialConfigFile);
	eCreateOrEditConfiguration getCreateOrEdit(void) const;
	wxString getEditPath(void);
	void OnPageChanged(wxWizardEvent& event);
	void OnPageChanging(wxWizardEvent& event);
 protected:
	void CreateControls(wxString strInitialConfigFile);
};

class DBConnectionWizardPage : public wxWizardPageSimple
{
 protected:
	ConnectionPanel *m_pConnectionPanel;
 public:
	DBConnectionWizardPage(app_checkConfigurationFunc_t conf_check_callback,
			       wxWizard *pParent, const wxBitmap& bitmap = wxNullBitmap);
	virtual ~DBConnectionWizardPage() {};
	void CreateControls(app_checkConfigurationFunc_t conf_check_callback);
	eBackendKind getBackend() const;
	wxString getHost() const;
	wxString getUser() const;
	wxString getPassword() const;
	wxString getDatabase() const;
};


class ConnectionStatusWizardPage : public wxWizardPageSimple
{
 protected:
	wxStaticText *m_pMessage;
	DECLARE_EVENT_TABLE()
 public:
	ConnectionStatusWizardPage(wxWizard *pParent, const wxBitmap& bitmap = wxNullBitmap);
	virtual ~ConnectionStatusWizardPage();
	void OnPageChanged(wxWizardEvent& event);
	void OnPageChanging(wxWizardEvent& event);
	EmdrosSchema *GetSchema(void);
	EmdrosSchema *TakeOverSchema(void);
	virtual DBConnectionWizardPage *GetConnectionPage(void); // Default just uses a cast on GetPrev().
	bool SchemaIsOK(wxString& schema_OK_message); // Default calls HasObjectTypes()
	bool HasObjectTypes(wxString& schema_OK_message);
 protected:
	void CreateControls();
	bool Connect(void);
	bool PopulateSchema();
	EmdrosSchema *m_pSchema;
	EmdrosEnv *m_pEE;
};


class GOOSVPOrderChooserWidget;

class GetOrderOfStringVecPage : public wxWizardPageSimple
{
 protected:
	wxStaticText *m_pMessage;
	wxString m_strLabel;
	GOOSVPOrderChooserWidget *m_pOCW;
	DECLARE_EVENT_TABLE()
 public:
	GetOrderOfStringVecPage(wxWizard *pParent, wxString strLabel = wxT(""), const wxBitmap& bitmap = wxNullBitmap);
	virtual ~GetOrderOfStringVecPage();
	void OnPageChanged(wxWizardEvent& event);	
	void OnUpdateUI(wxUpdateUIEvent& event);
	WXStringVec getStrings() const;
	void setStrings(const WXStringVec& stringvec);
	wxString getSelection() const; // Returns the empty string if no selection made
	virtual void OnSelectionChanged(); // Called when m_pOCW's selection changes
 protected:
	void CreateControls();
	virtual void OnPageChangedForward(); // Can be used to set the strings
	virtual bool getUpdateUIEnable(); // Called from OnUpdateUI
};


class GOOSVPOrderChooserWidget : public OrderChooserWidget
{
 public:
	GOOSVPOrderChooserWidget(GetOrderOfStringVecPage *pParent, int myid)
		: OrderChooserWidget(pParent, myid) {};
	virtual ~GOOSVPOrderChooserWidget() {};
 protected:
	virtual void SelectionChanged(void) {
		((GetOrderOfStringVecPage*)m_pParent)->OnSelectionChanged();
	};
};





#endif
// CONFIGWIZARD_H_

