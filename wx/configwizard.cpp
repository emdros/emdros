//
// configwizard.cpp
// Ulrik Petersen
// Created: 10/24-2006
// Last update: 4/11-2016
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



#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "configwizard.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif


#include <wx/sizer.h>
#include <wx/valgen.h>
#include <configwizard.h>
#include <wxutil_emdros.h>


///////////////////////////////////////////////////////////////
//
// CreateEditConfigurationWizardPage
//
///////////////////////////////////////////////////////////////



CreateEditConfigurationWizardPage::CreateEditConfigurationWizardPage(wxWizard *pParent,
								     eCreateOrEditConfiguration create_or_edit)
	: wxWizardPageSimple(pParent)
{
	CreateControls(create_or_edit);
}


void CreateEditConfigurationWizardPage::CreateControls(eCreateOrEditConfiguration create_or_edit)
{
	wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);

	wxStaticText* itemStaticText1 = new wxStaticText(this, wxID_STATIC, 
							 _("Do you want to:"),
							 wxDefaultPosition, 
							 wxDefaultSize, 
							 0);
	pMainSizer->Add(itemStaticText1, 0, 
			  wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 
			  5);

	m_pRadioCreate = new wxRadioButton(this,
					   wxID_DEFAULT,
					   wxT("Create a new configuration"),
					   wxDefaultPosition,
					   wxDefaultSize,
					   wxRB_GROUP);
	m_pRadioCreate->SetValue(create_or_edit == kCreateConfiguration);
	pMainSizer->Add(m_pRadioCreate,
			0, 
			wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 
			5);

	m_pRadioEdit = new wxRadioButton(this,
					 wxID_DEFAULT,
					 wxT("Edit an existing configuration file"),
					 wxDefaultPosition,
					 wxDefaultSize,
					 0);
	m_pRadioEdit->SetValue(create_or_edit == kEditConfiguration);
	pMainSizer->Add(m_pRadioEdit,
			0, 
			wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 
			5);

	this->SetSizer(pMainSizer);
	pMainSizer->Fit(this);
}

eCreateOrEditConfiguration CreateEditConfigurationWizardPage::getCreateOrEdit(void) const
{
	if (m_pRadioCreate->GetValue()) {
		return kCreateConfiguration;
	} else {
		return kEditConfiguration;
	}
}

wxString CreateEditConfigurationWizardPage::getEditPath(void)
{
	return m_pBrowseWidget->getValue();
}


///////////////////////////////////////////////////////////////
//
// GetConfigurationFilenameWizardPage
//
///////////////////////////////////////////////////////////////


BEGIN_EVENT_TABLE( GetConfigurationFilenameWizardPage , wxWizardPageSimple )
	EVT_WIZARD_PAGE_CHANGED( wxID_ANY , GetConfigurationFilenameWizardPage::OnPageChanged )
	EVT_WIZARD_PAGE_CHANGING( wxID_ANY , GetConfigurationFilenameWizardPage::OnPageChanging )
END_EVENT_TABLE()




GetConfigurationFilenameWizardPage::GetConfigurationFilenameWizardPage(wxWizard *pParent,
								       wxString strInitialConfigFile)
	: wxWizardPageSimple(pParent),
	  m_strInitialConfigFile(strInitialConfigFile)
{
	CreateControls(strInitialConfigFile);
}


void GetConfigurationFilenameWizardPage::CreateControls(wxString strInitialConfigFile)
{
	wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);

	wxStaticText* itemStaticText1 = new wxStaticText(this, wxID_STATIC, 
							 _("Choose configuration file"),
							 wxDefaultPosition, 
							 wxDefaultSize, 
							 0);
	pMainSizer->Add(itemStaticText1, 0, 
			  wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 
			  5);

	m_pBrowseWidget = new BrowseWidget(this, ID_BROWSE_WIDGET,
					   true,
					   wxT("Configuration file:"),
					   strInitialConfigFile,
					   wxT("Choose configuration file"),
					   wxT("Configuration files (*.cfg)|*.cfg|All files (*)|*"));
	
	pMainSizer->Add(m_pBrowseWidget, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

	this->SetSizer(pMainSizer);
	pMainSizer->Fit(this);
}

eCreateOrEditConfiguration GetConfigurationFilenameWizardPage::getCreateOrEdit(void) const
{
	if (GetPrev() != NULL) {
		return ((CreateEditConfigurationWizardPage*) GetPrev())->getCreateOrEdit();
	} else {
		return kCreateConfiguration;
	}
}

wxString GetConfigurationFilenameWizardPage::getEditPath(void)
{
	return m_pBrowseWidget->getValue();
}

void GetConfigurationFilenameWizardPage::OnPageChanged(wxWizardEvent& event)
{
	if (event.GetDirection()) { // Forward
		if (getCreateOrEdit() == kCreateConfiguration) {
			m_pBrowseWidget->setValue(wxT(""));
			m_pBrowseWidget->setStyleSaveOverwritePrompt();
			m_pBrowseWidget->Validate();
		} else {
			m_pBrowseWidget->setValue(m_strInitialConfigFile);
			m_pBrowseWidget->setStyleOpenFileMustExist();
		}
	} else {
		// We are going backward,
		// so no need to do anything...
	}
}

void GetConfigurationFilenameWizardPage::OnPageChanging(wxWizardEvent& event)
{
	if (event.GetDirection()) { // Forward
		if (getEditPath().IsEmpty()) {
			wxEmdrosErrorMessage(wxT("You cannot go on without specifying a filename."));
			event.Veto();
		} else {
			// Nothing to do!
		}
	} else {
		// Nothing to do!
	}
}




///////////////////////////////////////////////////////////////
//
// DBConnectionWizardPage
//
///////////////////////////////////////////////////////////////



DBConnectionWizardPage::DBConnectionWizardPage(app_checkConfigurationFunc_t conf_check_callback,
					       wxWizard *pParent,
					       const wxBitmap& bitmap)
	: wxWizardPageSimple(pParent, NULL, NULL, bitmap)
{
	CreateControls(conf_check_callback);
}


void DBConnectionWizardPage::CreateControls(app_checkConfigurationFunc_t conf_check_callback)
{
	wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);

	m_pConnectionPanel = new ConnectionPanel(conf_check_callback,
						 false, // Doesn't want "Configuration" box
						 this // parent
						 );


	pMainSizer->Add(m_pConnectionPanel, 0, 
			  wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 
			  5);

	this->SetSizer(pMainSizer);
	pMainSizer->Fit(this);
}

eBackendKind DBConnectionWizardPage::getBackend() const
{
	return m_pConnectionPanel->getBackend();
}

wxString DBConnectionWizardPage::getHost() const
{
	return m_pConnectionPanel->getHost();
}

wxString DBConnectionWizardPage::getPassword() const
{
	return m_pConnectionPanel->getPassword();
}

wxString DBConnectionWizardPage::getUser() const
{
	return m_pConnectionPanel->getUser();
}

wxString DBConnectionWizardPage::getDatabase() const
{
	return m_pConnectionPanel->getDatabase();
}




///////////////////////////////////////////////////////////////
//
// ConnectionStatusWizardPage
//
///////////////////////////////////////////////////////////////


BEGIN_EVENT_TABLE( ConnectionStatusWizardPage , wxWizardPageSimple )
EVT_WIZARD_PAGE_CHANGED( wxID_ANY , ConnectionStatusWizardPage::OnPageChanged )
EVT_WIZARD_PAGE_CHANGING( wxID_ANY , ConnectionStatusWizardPage::OnPageChanging )
END_EVENT_TABLE()


ConnectionStatusWizardPage::ConnectionStatusWizardPage(wxWizard *pParent, const wxBitmap& bitmap )
: wxWizardPageSimple(pParent, NULL, NULL, bitmap),
	m_pSchema(0),
	m_pEE(0)
{
	CreateControls();
}

ConnectionStatusWizardPage::~ConnectionStatusWizardPage()
{
	delete m_pEE;
	delete m_pSchema;
}

void ConnectionStatusWizardPage::CreateControls()
{
	wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);

	m_pMessage = new wxStaticText(this,
				      wxID_ANY,
				      wxT(""));

	pMainSizer->Add(m_pMessage);

	SetSizer(pMainSizer);
	pMainSizer->Fit(this);
}

void ConnectionStatusWizardPage::OnPageChanging(wxWizardEvent& event)
{
	if (event.GetDirection()) { // Forward
		if (m_pSchema == 0) {
			event.Veto();
		} else {
			// Nothing to do!
		}
	} else {
		// Nothing to do!
	}
}

void ConnectionStatusWizardPage::OnPageChanged(wxWizardEvent& event)
{
	if (event.GetDirection()) { // Forward
		if (Connect()) {
			if (PopulateSchema()) {
				wxString schema_OK_message;
				if (SchemaIsOK(schema_OK_message)) {
					m_pMessage->SetLabel(wxString(wxT("Connection to database established.\nPress 'Next' to continue.")));
				} else {
					m_pMessage->SetLabel(wxString(wxT("Connection to database went fine,\nand we could get the schema,\nbut something is wrong with the schema:\n\n"))
							     + schema_OK_message
							     + wxString(wxT("Please press the 'back' button below,\nand try setting the correct parameters.\nOtherwise, click 'Cancel' to stop the wizard.")));
				}
			} else {
				m_pMessage->SetLabel(wxString(wxT("Could not get database schema from\nthe database specified.\n\nPlease press the 'back' button below,\nand try setting the correct parameters.\nOtherwise, click 'Cancel' to stop the wizard.")));
				
			}
		} else {
			m_pMessage->SetLabel(wxString(wxT("Could not connect to database.\n\nPlease press the 'back' button below,\nand try setting the correct parameters.\nOtherwise, click 'Cancel' to stop the wizard.")));
		}
	} else {
		// We are going backward,
		// so no need to do anything...
	}

}

EmdrosSchema *ConnectionStatusWizardPage::GetSchema(void)
{
	return m_pSchema;
}


EmdrosSchema *ConnectionStatusWizardPage::TakeOverSchema(void)
{
	EmdrosSchema *pResult = m_pSchema;
	m_pSchema = 0; // Make sure we don't delete it in our destructor
	return pResult;
}


DBConnectionWizardPage *ConnectionStatusWizardPage::GetConnectionPage(void)
{
	return (DBConnectionWizardPage*)GetPrev();
}

bool ConnectionStatusWizardPage::Connect(void)
{
	delete m_pEE;

	DBConnectionWizardPage *pConnectionPage = GetConnectionPage();

	if (pConnectionPage == 0) {
		return false;
	} else {
		eBackendKind backend_kind = pConnectionPage->getBackend();
		wxString strHost = pConnectionPage->getHost();
		wxString strUser = pConnectionPage->getUser();
		wxString strPassword = pConnectionPage->getPassword();
		wxString strDatabase = pConnectionPage->getDatabase();

		std::string host(strHost.mb_str(wxConvUTF8));
		std::string user(strUser.mb_str(wxConvUTF8));
		std::string password(strPassword.mb_str(wxConvUTF8));
		std::string database (strDatabase.mb_str(wxConvUTF8));

		/*
		std::cerr << "UP101: backend = " << backend_kind << "\n       "
			  << "database =  '" << database << "'\n       "
			  << "password =  '" << password << "'\n       "
			  << "host =  '" << host << "'\n       "
			  << "user =  '" << user << "'" << std::endl;
		*/

		m_pEE = new EmdrosEnv(kOKConsole,
				      kCSUTF8,
				      host,
				      user,
				      password,
				      database,
				      backend_kind);

		if (m_pEE->connectionOk()) {
			return true;
		} else {
			delete m_pEE;
			m_pEE = 0;
			return false;
		}
	}
}

bool ConnectionStatusWizardPage::PopulateSchema()
{
	if (m_pEE == 0) {
		// std::cerr << "UP100: PopulateSchema(): m_pEE == 0" << std::endl;
		return false;
	} else if (!m_pEE->connectionOk()) {
		// std::cerr << "UP100: PopulateSchema(): connection not OK" << std::endl;
		// Connection to backend not OK.
		return false;
	} else {
		delete m_pSchema;
		m_pSchema = new EmdrosSchema(m_pEE);
		std::ostringstream sout;
		if (!m_pSchema->populate(&sout)) {
			delete m_pSchema;
			m_pSchema = 0;
			// std::cerr << "UP100: PopulateSchema(): could not populate" << std::endl << sout.str() << std::endl;
			return false; 
		} else {
			return true;
		}
	}
}

bool ConnectionStatusWizardPage::SchemaIsOK(wxString& schema_OK_message)
{
	return HasObjectTypes(schema_OK_message);
}

bool ConnectionStatusWizardPage::HasObjectTypes(wxString& schema_OK_message)
{
	if (m_pSchema == 0) {
		schema_OK_message += wxT("Schema was not populated.\n\n");
		return false;
	} else {
		StringVec object_type_names = m_pSchema->getObjectTypes().getObjectTypeNames();
		if (object_type_names.size() > 0) {
			return true;
		} else {
			schema_OK_message += wxT("Schema had no object types.\n\n");
			return false;
		}
	}
}


///////////////////////////////////////////////////////////////
//
// RasterUnitWizardPage
//
///////////////////////////////////////////////////////////////


/*
BEGIN_EVENT_TABLE( RasterUnitWizardPage , wxWizardPageSimple )
EVT_WIZARD_PAGE_CHANGED( wxID_ANY , RasterUnitWizardPage::OnPageChanged )
EVT_WIZARD_PAGE_CHANGING( wxID_ANY , RasterUnitWizardPage::OnPageChanging )
END_EVENT_TABLE()




ConnectionStatusWizardPage::ConnectionStatusWizardPage(wxWizard *pParent, const wxBitmap& bitmap )
: wxWizardPageSimple(pParent, NULL, NULL, bitmap),
	m_pEE(0), m_pSchema(0)
{
	CreateControls();
}

ConnectionStatusWizardPage::~ConnectionStatusWizardPage()
{
	delete m_pEE;
	delete m_pSchema;
}

void ConnectionStatusWizardPage::CreateControls()
{
	wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);

	m_pMessage = new wxStaticText(this,
				      wxID_ANY,
				      wxT(""));

	pMainSizer->Add(m_pMessage);

	SetSizer(pMainSizer);
	pMainSizer->Fit(this);
}

void ConnectionStatusWizardPage::OnPageChanging(wxWizardEvent& event)
{
	if (event.GetDirection()) { // Forward
		if (m_pSchema == 0) {
			event.Veto();
		} else {
			// Nothing to do!
		}
	} else {
		// Nothing to do!
	}
}

void ConnectionStatusWizardPage::OnPageChanged(wxWizardEvent& event)
{
	if (event.GetDirection()) { // Forward
		if (Connect()) {
			if (PopulateSchema()) {
				wxString schema_OK_message;
				if (SchemaIsOK(schema_OK_message)) {
					m_pMessage->SetLabel(wxString(wxT("Connection to database established. Press 'Next' to continue.")));
				} else {
					m_pMessage->SetLabel(wxString(wxT("Connection to database went fine,\nand we could get the schema,\nbut something is wrong with the schema:\n\n"))
							     + schema_OK_message
							     + wxString(wxT("Please press the 'back' button below,\nand try setting the correct parameters.\nOtherwise, click 'Cancel' to stop the wizard.")));
				}
			} else {
				m_pMessage->SetLabel(wxString(wxT("Could not get database schema from\nthe database specified.\n\nPlease press the 'back' button below,\nand try setting the correct parameters.\nOtherwise, click 'Cancel' to stop the wizard.")));
				
			}
		} else {
			m_pMessage->SetLabel(wxString(wxT("Could not connect to database.\n\nPlease press the 'back' button below,\nand try setting the correct parameters.\nOtherwise, click 'Cancel' to stop the wizard.")));
		}
	} else {
		// We are going backward,
		// so no need to do anything...
	}

}


*/


///////////////////////////////////////////////////////////////
//
// GetOrderOfStringVecPage
//
///////////////////////////////////////////////////////////////


BEGIN_EVENT_TABLE( GetOrderOfStringVecPage , wxWizardPageSimple )
	EVT_WIZARD_PAGE_CHANGED( wxID_ANY , GetOrderOfStringVecPage::OnPageChanged )
	EVT_UPDATE_UI(ID_ORDER_CHOOSER_WIDGET, GetOrderOfStringVecPage::OnUpdateUI)
END_EVENT_TABLE()


GetOrderOfStringVecPage::GetOrderOfStringVecPage(wxWizard *pParent, wxString strLabel, const wxBitmap& bitmap )
	: wxWizardPageSimple(pParent, NULL, NULL, bitmap),
	  m_pMessage(0),
	  m_strLabel(strLabel),
	  m_pOCW(0)
{
	CreateControls();
}

GetOrderOfStringVecPage::~GetOrderOfStringVecPage()
{
}


void GetOrderOfStringVecPage::CreateControls()
{
	wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);

	m_pMessage = new wxStaticText(this,
				      wxID_ANY,
				      m_strLabel);

	pMainSizer->Add(m_pMessage);

	m_pOCW = new GOOSVPOrderChooserWidget(this,
					      ID_ORDER_CHOOSER_WIDGET);

	pMainSizer->Add(m_pOCW);

	SetSizer(pMainSizer);
	pMainSizer->Fit(this);
}

WXStringVec GetOrderOfStringVecPage::getStrings() const
{
	if (m_pOCW == 0) {
		return WXStringVec();
	} else {
		return m_pOCW->getStrings();
	}
}

void GetOrderOfStringVecPage::setStrings(const WXStringVec& stringvec)
{
	if (m_pOCW == 0) {
		return;
	} else {
		m_pOCW->setStrings(stringvec);
	}
}


void GetOrderOfStringVecPage::OnPageChanged(wxWizardEvent& event)
{
	if (event.GetDirection()) { // Forward
		OnPageChangedForward();
	} else {
		// We are going backward,
		// so no need to do anything...
	}

}

void GetOrderOfStringVecPage::OnPageChangedForward()
{
}

void GetOrderOfStringVecPage::OnSelectionChanged()
{
}

wxString GetOrderOfStringVecPage::getSelection() const
{
	return m_pOCW->getSelection();
}

void GetOrderOfStringVecPage::OnUpdateUI(wxUpdateUIEvent& event)
{
	event.Enable(getUpdateUIEnable());
}

bool GetOrderOfStringVecPage::getUpdateUIEnable()
{
	return true;
}
