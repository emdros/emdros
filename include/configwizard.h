//
// configwizard.h
// Ulrik Petersen
// Created: 10/24-2006
// Last update: 4/14-2007
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

