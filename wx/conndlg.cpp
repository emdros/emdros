/*
 * conndlg.cpp
 *
 * The complex connection dialog in mqlqtwx.
 * mqlqtwx is a graphical front-end for querying Emdros databases.
 *
 * Ulrik Petersen
 * Created: 4/13-2005
 * Last update: 11/2-2018
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
#pragma implementation "conndlg.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include "wx/combobox.h"
#include "wx/validate.h"
#include "wx/config.h"
#include "wx/filefn.h"
#include "wx/filename.h"
#include "wx/version.h"

#include <conndlg.h>
#include <wxutil_emdros.h>
#include <string_func.h>
#include <prefix_emdros.h>
#include <emdros-lconfig.h>
#include <conf.h>


class BackendKindValidator : public wxValidator
{
protected:
	eBackendKind *m_valPtr;
public:
	BackendKindValidator(eBackendKind *valPtr = NULL) { 
		m_valPtr = valPtr;
	};
	/*
	BackendKindValidator(const BackendKindValidator& other) : wxValidator(other) {
		m_valPtr = other.m_valPtr;
	}
	*/
	~BackendKindValidator() {};
	virtual wxObject* Clone() const {
		return new BackendKindValidator(m_valPtr);
	};
	virtual bool Validate(wxWindow *pParent) {
		if (pParent == 0) {
			return false;
		} else {
			if (pParent->IsKindOf(CLASSINFO(ConnectionPanel))) {
				wxString strBackendKind = ((ConnectionPanel*)pParent)->m_cbBackendCB->GetValue();
				eBackendKind backend = getBackendKindFromString(strBackendKind);
				return backend != kBackendNone;
			} else {
				return false;
			}

		}
	};
	
	virtual bool TransferFromWindow() {
		wxWindow *pWindow = GetWindow();
		if (pWindow->IsKindOf(CLASSINFO(wxComboBox))) {
			wxString strBackendKind = ((wxComboBox*)pWindow)->GetValue();
			(*m_valPtr) = getBackendKindFromString(strBackendKind);
			return (*m_valPtr) != kBackendNone;

		} else {
			return false;
		}
	}
	
	virtual bool TransferToWindow() {
		if (*m_valPtr == kBackendNone) {
			return false;
		} else {
			wxWindow *pWindow = GetWindow();
			if (pWindow->IsKindOf(CLASSINFO(wxComboBox))) {
				std::string backendKind = backend_kind2string(*m_valPtr);
				wxString strBackendKind(backendKind.c_str(), wxConvUTF8);
				((wxComboBox*)pWindow)->SetValue(strBackendKind);
				return true;
			} else {
				return false;
			}
		}
	};

protected:
	eBackendKind getBackendKindFromString(wxString strBackendKind) {
		std::string backendKind = (const char*) strBackendKind.mb_str(wxConvUTF8);
		eBackendKind backend;
		if (string2backend_kind(backendKind, backend)) {
			return backend;
		} else {
			return kBackendNone;
		}
	};
};

/*!
 * ConnectionPanel type definition
 */

IMPLEMENT_DYNAMIC_CLASS( ConnectionPanel, wxPanel )

/*!
 * ConnectionPanel event table definition
 */
BEGIN_EVENT_TABLE( ConnectionPanel, wxPanel )
EVT_BUTTON(ID_BUTTON_BROWSE_CONF, ConnectionPanel::OnBrowseConfiguration)
EVT_BUTTON(ID_BUTTON_BROWSE_DATABASE, ConnectionPanel::OnBrowseDatabase)
EVT_COMBOBOX(ID_COMBOBOX1, ConnectionPanel::OnBackendComboBoxChangeSelection)
EVT_UPDATE_UI(ID_BUTTON_BROWSE_DATABASE, ConnectionPanel::OnUpdateBtnBrowseDatabase)
END_EVENT_TABLE()

/*!
 * ConnectionPanel constructor.
 */
ConnectionPanel::ConnectionPanel( )
: m_backend(DEFAULT_BACKEND_ENUM), 
	m_bHasConfiguration(true),
	m_ctrlDatabase(0),
	m_ctrlConfiguration(0),
	m_btnBrowseConfiguration(0),
	m_sizerAll(0),
	m_sizerConfiguration(0),
	m_sizerNonSimple(0),
	m_btnBrowseDatabase(0),
	m_cbBackendCB(0),
	m_itemTextCtrlUser(0),
	m_itemTextCtrlPassword(0),
	m_itemTextCtrlHost(0),
	m_conf_check_callback(0)
{
}

/*!
 * ConnectionPanel constructor.
 */
ConnectionPanel::ConnectionPanel(app_checkConfigurationFunc_t conf_check_callback, bool bHasConfiguration, wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style)
  
	: m_backend(DEFAULT_BACKEND_ENUM), 
	  m_bHasConfiguration(bHasConfiguration),
	  m_ctrlDatabase(0),
	  m_ctrlConfiguration(0),
	  m_btnBrowseConfiguration(0),
	  m_sizerAll(0),
	  m_sizerConfiguration(0),
	  m_sizerNonSimple(0),
	  m_btnBrowseDatabase(0),
	  m_cbBackendCB(0),
	  m_itemTextCtrlUser(0),
	  m_itemTextCtrlPassword(0),
	m_itemTextCtrlHost(0)
{
	Create(conf_check_callback, parent, id, caption, pos, size, style);
}


ConnectionPanel::~ConnectionPanel()
{
	writeConfig();
}


/*!
 * ConnectionPanel creator
 */
bool ConnectionPanel::Create( app_checkConfigurationFunc_t conf_check_callback, wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
	(void)(caption); // Silence a warning

	m_conf_check_callback = conf_check_callback;

	////@begin ConnectionPanel member initialisation
	m_strHost = wxT("localhost");
	m_strUser = wxT("emdf");
	m_strPassword = wxT("");
	readConfig();
	////@end ConnectionPanel member initialisation

	////@begin ConnectionPanel creation
	SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS|wxWS_EX_VALIDATE_RECURSIVELY);
	wxPanel::Create( parent, id, pos, size, style );

	CreateControls();
	GetSizer()->Fit(this);
	GetSizer()->SetSizeHints(this);
	Centre();
	////@end ConnectionPanel creation
	return TRUE;
}

/*!
 * Control creation for ConnectionPanel
 */

void ConnectionPanel::CreateControls()
{    
	////@begin ConnectionPanel content construction
	m_ctrlDatabase = NULL;
	m_ctrlConfiguration = NULL;
  
	ConnectionPanel* itemDialog1 = this;

	wxSize sizeBroad = wxDefaultSize;
	sizeBroad.SetWidth(300);

	m_sizerAll = new wxFlexGridSizer(3, 1, 0, 0);
	itemDialog1->SetSizer(m_sizerAll);


	m_sizerSimple = new wxBoxSizer(wxVERTICAL);
	m_sizerAll->Add(m_sizerSimple, 0, wxALIGN_LEFT|wxALL, 5);

	m_sizerNonSimple = new wxFlexGridSizer(5, 2, 0, 0);
	m_sizerAll->Add(m_sizerNonSimple, 0, wxALIGN_LEFT|wxALL, 5);

	
	wxBoxSizer *pBackendSizer = new wxBoxSizer(wxHORIZONTAL);
	m_sizerSimple->Add(pBackendSizer, 0, wxALIGN_LEFT|wxALL, 5);
	wxStaticText* itemStaticTextBackendCB = new wxStaticText( itemDialog1, wxID_STATIC, _("Backend:"), wxDefaultPosition, wxDefaultSize, 0 );
	pBackendSizer->Add(itemStaticTextBackendCB, 0, wxALL, 5);

	const int number_of_backends = USE_SQLITE3 + USE_POSTGRESQL + USE_MYSQL + USE_BPT;
	wxString choices[number_of_backends + 1] = {
#if USE_SQLITE3
		wxString(backend_kind2string(kSQLite3).c_str(), wxConvUTF8),
#endif
#if USE_POSTGRESQL
		wxString(backend_kind2string(kPostgreSQL).c_str(), wxConvUTF8),
#endif
#if USE_MYSQL
		wxString(backend_kind2string(kMySQL).c_str(), wxConvUTF8),
#endif
#if USE_BPT
		wxString(backend_kind2string(kBPT).c_str(), wxConvUTF8),
#endif
		wxString(wxT(""))
	};

	m_cbBackendCB = new wxComboBox( itemDialog1, ID_COMBOBOX1, wxString(backend_kind2string(DEFAULT_BACKEND_ENUM).c_str(), wxConvUTF8), wxDefaultPosition, sizeBroad, number_of_backends, choices, wxCB_READONLY );
	m_cbBackendCB->SetHelpText(_("The name of the SQL backend database engine to use.\nUse (the) one that is installed on your system.\nSQLite3 is included in Emdros by default."));
	if (ShowToolTips())
		m_cbBackendCB->SetToolTip(_("The name of the SQL backend database engine to use.\nUse (the) one that is installed on your system.\nSQLite3 is included in Emdros by default."));
	pBackendSizer->Add(m_cbBackendCB, 0, wxALIGN_LEFT|wxALL, 5);


	m_sizerConfiguration = new wxBoxSizer(wxHORIZONTAL);

	wxStaticText* itemStaticText13 = new wxStaticText( itemDialog1, wxID_STATIC, _("Configuration:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_sizerConfiguration->Add(itemStaticText13, 0, wxALL, 5);
	

	m_sizerSimple->Add(m_sizerConfiguration, 0, wxALIGN_LEFT|wxALL, 0);
		
	m_ctrlConfiguration = new wxTextCtrl( itemDialog1, ID_TEXTCTRL5, wxT(""), wxDefaultPosition, sizeBroad, 0 );
	m_ctrlConfiguration->SetHelpText(_("The name and path of the configuration file."));
	if (ShowToolTips())
		m_ctrlConfiguration->SetToolTip(_("The name and path of the configuration file."));
	m_sizerConfiguration->Add(m_ctrlConfiguration, 0, wxALL, 5);
	
	m_btnBrowseConfiguration = new wxButton( itemDialog1, ID_BUTTON_BROWSE_CONF, _("Browse..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_sizerConfiguration->Add(m_btnBrowseConfiguration, 0, wxALL, 0);
 
	wxBoxSizer *pDBSizer = new wxBoxSizer(wxHORIZONTAL);
	m_sizerSimple->Add(pDBSizer, 0, wxALIGN_RIGHT|wxALL, 5);
	
	wxStaticText* itemStaticText11 = new wxStaticText( itemDialog1, wxID_STATIC, _("database:"), wxDefaultPosition, wxDefaultSize, 0 );
	pDBSizer->Add(itemStaticText11, 0, wxALL, 5);

	m_ctrlDatabase = new wxTextCtrl( itemDialog1, ID_TEXTCTRL4, wxT(""), wxDefaultPosition, sizeBroad, 0 );
	pDBSizer->Add(m_ctrlDatabase, 0, wxALIGN_LEFT|wxALL, 5);

	m_btnBrowseDatabase = new wxButton( itemDialog1, ID_BUTTON_BROWSE_DATABASE, _("Browse..."), wxDefaultPosition, wxDefaultSize, 0 );
	pDBSizer->Add(m_btnBrowseDatabase, 0, wxALL, 0);


	wxStaticText* itemStaticText3 = new wxStaticText( itemDialog1, wxID_STATIC, wxT("host:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_sizerNonSimple->Add(itemStaticText3, 0, wxALIGN_RIGHT|wxALL, 5);

	m_itemTextCtrlHost = new wxTextCtrl( itemDialog1, ID_TEXTCTRL, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
	m_sizerNonSimple->Add(m_itemTextCtrlHost, 0, wxALIGN_LEFT|wxALL, 5);

	wxStaticText* itemStaticText7 = new wxStaticText( itemDialog1, wxID_STATIC, wxT("user:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_sizerNonSimple->Add(itemStaticText7, 0, wxALIGN_RIGHT|wxALL, 5);

	m_itemTextCtrlUser = new wxTextCtrl( itemDialog1, ID_TEXTCTRL2, wxT(""), wxDefaultPosition, sizeBroad, 0 );
	m_itemTextCtrlUser->SetHelpText(wxT("The database user to connect to the backend database as."));
	if (ShowToolTips())
		m_itemTextCtrlUser->SetToolTip(wxT("The database user to connect\nto the backend database as."));
	m_sizerNonSimple->Add(m_itemTextCtrlUser, 0, wxALIGN_LEFT|wxALL, 5);

	wxStaticText* itemStaticText9 = new wxStaticText( itemDialog1, wxID_STATIC, _("password:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_sizerNonSimple->Add(itemStaticText9, 0, wxALIGN_RIGHT|wxALL, 5);

	m_itemTextCtrlPassword = new wxTextCtrl( itemDialog1, ID_TEXTCTRL3, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
	m_itemTextCtrlPassword->SetHelpText(_("The password to use when connecting to the backend database."));
	if (ShowToolTips())
		m_itemTextCtrlPassword->SetToolTip(_("The password to use when connecting\nto the backend database."));
	m_sizerNonSimple->Add(m_itemTextCtrlPassword, 0, wxALIGN_LEFT|wxALL, 5);


	// Set validators
	m_cbBackendCB->SetValidator( BackendKindValidator(&m_backend) );
	m_itemTextCtrlHost->SetValidator( wxTextValidator(wxFILTER_NONE, & m_strHost) );
	m_itemTextCtrlUser->SetValidator( wxTextValidator(wxFILTER_NONE, & m_strUser) );
	m_itemTextCtrlPassword->SetValidator( wxTextValidator(wxFILTER_NONE, & m_strPassword) );
	m_ctrlDatabase->SetValidator( wxTextValidator(wxFILTER_NONE, & m_strDatabase) );
	m_ctrlConfiguration->SetValidator( wxTextValidator(wxFILTER_NONE, & m_strConfiguration) );

	////@end ConnectionPanel content construction
	DoInitialUpdateBtnBrowseDatabase();
}

/*!
 * Should we show tooltips?
 */
bool ConnectionPanel::ShowToolTips()
{
	return TRUE;
}

/*!
 * Get bitmap resources
 */
wxBitmap ConnectionPanel::GetBitmapResource( const wxString& name )
{
	(void)(name); // Silence a warning

	// Bitmap retrieval
	////@begin ConnectionPanel bitmap retrieval
	return wxNullBitmap;
	////@end ConnectionPanel bitmap retrieval
}

/*!
 * Get icon resources
 */
wxIcon ConnectionPanel::GetIconResource( const wxString& name )
{
	(void)(name); // Silence a warning

	// Icon retrieval
	////@begin ConnectionPanel icon retrieval
	return wxNullIcon;
	////@end ConnectionPanel icon retrieval
}


void ConnectionPanel::OnBackendComboBoxChangeSelection(wxCommandEvent& event)
{
	(void)(event); // Silence a warning

	// Call the validator's TransferFromWindow
	TransferDataFromWindow();

	// Make sure that UI elements are updated
	Validate();
}


void ConnectionPanel::OnBrowseConfiguration(wxCommandEvent& event)
{
	(void)(event); // Silence a warning

	wxString strConfigurationFileDir;
	wxString strConfigurationFileFile;
	wxFileName fn(m_strConfiguration);
	if (fn.IsAbsolute()) {
		strConfigurationFileDir = fn.GetPath();
		strConfigurationFileFile = fn.GetFullName();
	} else {
		strConfigurationFileDir = ::wxGetHomeDir();
		strConfigurationFileFile = wxT("");
	}
	wxFileDialog dlgFileDialog(this,
				   wxT("Choose configuration file"),
				   strConfigurationFileDir,
				   strConfigurationFileFile,
				   wxT("Configuration files (*.cfg)|*.cfg|TXT files (*.txt)|*.txt|All files (*)|*"),
				   wxFD_OPEN | wxFD_FILE_MUST_EXIST
				   );
	int nReturn;
	bool bContinue = true;
	while (bContinue) {
		if ((nReturn = dlgFileDialog.ShowModal()) == wxID_OK) {
			m_ctrlConfiguration->SetValue(dlgFileDialog.GetPath());

			std::ostringstream strout;
			Configuration *pConf = 
				parse_config_file(std::string((const char*)(dlgFileDialog.GetPath().mb_str(wxConvUTF8))),
						  "",
						  &strout);
			if (pConf == 0) {
				bContinue = true;
				wxEmdrosErrorMessage(wxString(wxT("Error: The requested file is not a valid configuration file. Please choose one that is, or press cancel.")));
			} else {
				bContinue = false;
				std::string error_msg;
				if (!m_conf_check_callback(pConf,error_msg)) {
					wxEmdrosErrorMessage(wxString(wxT("Error: The request file is not a valid configuration file.  Here is what is wrong:\n")) + wxString(error_msg.c_str(), wxConvUTF8) + wxT("\n--------------------\nPlease choose one that is valid, or press cancel."));
					bContinue = true;
				} else {
					wxString strDatabase = m_ctrlDatabase->GetValue();

					if (strDatabase.IsEmpty()
					    || strDatabase == m_strDatabase) {
						if (pConf->has_key("database")) {
							std::string db = pConf->getValues("database")[0];
							m_ctrlDatabase->SetValue(wxString(db.c_str(), wxConvUTF8));
						} 
					}
				}
	  
	  
			}
			delete pConf;
		} else {
			bContinue = false;
		}
	}
}


void ConnectionPanel::OnBrowseDatabase(wxCommandEvent& event)
{
	(void)(event); // Silence a warning

	wxString strDatabaseDir;
	wxString strDatabaseFile;
	wxFileName fn(m_strConfiguration);
	if (fn.IsAbsolute()) {
		strDatabaseDir = fn.GetPath();
		strDatabaseFile = fn.GetFullName();
	} else {
		strDatabaseDir = ::wxGetHomeDir();
		strDatabaseFile = wxT("");
	}

	wxFileDialog dlgFileDialog(this,
				   wxT("Choose database file"),
				   strDatabaseDir,
				   strDatabaseFile,
				   wxT("All files (*)|*"),
				   wxFD_OPEN | wxFD_FILE_MUST_EXIST
				   );
	int nReturn;
	if ((nReturn = dlgFileDialog.ShowModal()) == wxID_OK) {
		m_ctrlDatabase->SetValue(dlgFileDialog.GetPath());
	}
}


eBackendKind ConnectionPanel::getBackendKindFromComboBoxString(void)
{
	if (m_cbBackendCB == 0) {
		return kBackendNone;
	} else {
		wxString strBackendKind = m_cbBackendCB->GetValue();
		std::string backendKind = (const char*) strBackendKind.mb_str(wxConvUTF8);
		eBackendKind backend;
		if (string2backend_kind(backendKind, backend)) {
			return backend;
		} else {
			return kBackendNone;
		}
	}
}

void ConnectionPanel::OnUpdateBtnBrowseDatabase(wxUpdateUIEvent& event)
{
	(void)(event); // Silence a warning

	DoUpdateBtnBrowseDatabase();
}

void ConnectionPanel::DoUpdateBtnBrowseDatabase()
{
	if (m_bHasConfiguration) {
		if (!m_ctrlConfiguration->IsEnabled()) {
			m_ctrlConfiguration->Enable(true);
		}
		if (!m_btnBrowseConfiguration->IsEnabled()) {
			m_btnBrowseConfiguration->Enable(true);
		}
	} else {
		if (m_ctrlConfiguration->IsEnabled()) {
			m_ctrlConfiguration->Enable(false);
		}
		if (m_btnBrowseConfiguration->IsEnabled()) {
			m_btnBrowseConfiguration->Enable(false);
		}
	}
	if (m_backend == kSQLite3
	    || m_backend == kBPT) {
		if (!m_btnBrowseDatabase->IsEnabled()) {
			m_btnBrowseDatabase->Enable(true);
		}

		if (m_itemTextCtrlUser->IsEnabled()) {
			m_itemTextCtrlUser->Enable(false);
		}
		if (m_itemTextCtrlPassword->IsEnabled()) {
			m_itemTextCtrlPassword->Enable(false);
		}
		if (m_itemTextCtrlHost->IsEnabled()) {
			m_itemTextCtrlHost->Enable(false);
		}
		Validate();
	} else {
		if (m_btnBrowseDatabase->IsEnabled()) {
			m_btnBrowseDatabase->Enable(false);
		}

		if (!m_itemTextCtrlUser->IsEnabled()) {
			m_itemTextCtrlUser->Enable(true);
		}
		if (!m_itemTextCtrlPassword->IsEnabled()) {
			m_itemTextCtrlPassword->Enable(true);
		}
		if (!m_itemTextCtrlHost->IsEnabled()) {
			m_itemTextCtrlHost->Enable(true);
		}
		Validate();
	}
}


void ConnectionPanel::DoInitialUpdateBtnBrowseDatabase()
{
	DoUpdateBtnBrowseDatabase();
}


eBackendKind ConnectionPanel::getBackend()
{
	TransferDataFromWindow();
	return m_backend;
}

wxString ConnectionPanel::getHost()
{
	TransferDataFromWindow();
	return m_strHost;
}

wxString ConnectionPanel::getPassword() 
{
	TransferDataFromWindow();
	return m_strPassword;
}

wxString ConnectionPanel::getUser() 
{
	TransferDataFromWindow();
	return m_strUser;
}

wxString ConnectionPanel::getDatabase()
{
	TransferDataFromWindow();
	return m_strDatabase;
}

wxString ConnectionPanel::getConfiguration()
{
	TransferDataFromWindow();
	return m_strConfiguration;
}


void ConnectionPanel::setBackend(eBackendKind value)
{
	m_backend = value;
	TransferDataToWindow();
}

void ConnectionPanel::setHost(wxString value)
{
	m_strHost = value;
	TransferDataToWindow();
}

void ConnectionPanel::setPassword(wxString value)
{
	m_strPassword = value;
	TransferDataToWindow();
}

void ConnectionPanel::setUser(wxString value)
{
	m_strUser = value;
	TransferDataToWindow();
}

void ConnectionPanel::setDatabase(wxString value)
{
	m_strDatabase = value;
	TransferDataToWindow();
}

void ConnectionPanel::setConfiguration(wxString value)
{
	m_strConfiguration = value;
	TransferDataToWindow();
}


void ConnectionPanel::readConfig()
{
	// Create it if it does not exist;
	// use the name set by the app's constructor's call to
	// SetAppName for the root of the tree.
	wxConfigBase *config = wxConfigBase::Get();

	// Flush all current changes to disk/registry
	config->Flush();

	config->SetPath(wxT("/ConnectionPanel/"));

	wxString myString;


	if (config->Read(wxT("Backend"), &myString)) {
		if (!string2backend_kind(std::string((const char*)myString.mb_str(wxConvUTF8)), m_backend)) {
			m_backend = DEFAULT_BACKEND_ENUM;
		}
	} else {
		m_backend = DEFAULT_BACKEND_ENUM;
	}

	if (config->Read(wxT("Configuration"), &myString)) {
		m_strConfiguration = myString;
	} else {
		m_strConfiguration = wxT("");
	}

	if (config->Read(wxT("Database"), &myString)) {
		m_strDatabase = myString;
	} else {
		m_strDatabaseDir = wxT("");
	}

	if (config->Read(wxT("UserName"), &myString)) {
		m_strUser = myString;
	} else {
		m_strUser = wxT("emdf");
	}


	if (config->Read(wxT("Host"), &myString)) {
		m_strHost = myString;
	} else {
		m_strHost = wxT("localhost");
	}
}


void ConnectionPanel::writeConfig()
{
	// Create it if it does not exist;
	// use the name set by LTCApp::LTCApp's call to
	// SetAppName for the root of the tree.
	wxConfigBase *config = wxConfigBase::Get();

	config->SetPath(wxT("/ConnectionPanel/"));

	wxString strBackend = wxString(backend_kind2string(m_backend).c_str(), wxConvUTF8);
	config->Write(wxT("Backend"), strBackend);

	config->Write(wxT("Configuration"), m_strConfiguration);

	config->Write(wxT("Database"), m_strDatabase);

	config->Write(wxT("UserName"), m_strUser);
	config->Write(wxT("Host"), m_strHost);

	// Flush all current changes to disk/registry
	config->Flush();
}



/////////////////////////////////////////////////////////
//
// ConnectionDialog
//
/////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC_CLASS( ConnectionDialog, wxDialog )

/*!
 * ConnectionDialog event table definition
 */
BEGIN_EVENT_TABLE( ConnectionDialog, wxDialog )
END_EVENT_TABLE()


/*!
 * ConnectionDialog constructor.
 */
ConnectionDialog::ConnectionDialog( )
: m_pConnectionPanel(0),
	m_bHasConfiguration(true),
	m_conf_check_callback(0)
{
}

/*!
 * ConnectionDialog constructor.
 */
ConnectionDialog::ConnectionDialog(app_checkConfigurationFunc_t conf_check_callback, bool bHasConfiguration, wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style)
  
	: m_pConnectionPanel(0),
	  m_bHasConfiguration(bHasConfiguration),
	  m_conf_check_callback(0)
{
	Create(conf_check_callback, bHasConfiguration, parent, id, caption, pos, size, style);
}

/*!
 * ConnectionDialog creator
 */
bool ConnectionDialog::Create( app_checkConfigurationFunc_t conf_check_callback, bool bHasConfiguration, wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
	m_conf_check_callback = conf_check_callback;
	m_bHasConfiguration = bHasConfiguration;

	////@begin ConnectionDialog creation
	SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS|wxWS_EX_VALIDATE_RECURSIVELY);
	wxDialog::Create( parent, id, caption, pos, size, style );

	CreateControls();
	GetSizer()->Fit(this);
	GetSizer()->SetSizeHints(this);
	Centre();
	////@end ConnectionDialog creation
	return TRUE;
}

/*!
 * Control creation for ConnectionDialog
 */

void ConnectionDialog::CreateControls()
{    
	////@begin ConnectionDialog content construction
	ConnectionDialog* itemDialog1 = this;

	wxSize sizeBroad = wxDefaultSize;
	sizeBroad.SetWidth(300);

	wxFlexGridSizer *pSizerAll = new wxFlexGridSizer(2, 1, 0, 0);
	itemDialog1->SetSizer(pSizerAll);

	m_pConnectionPanel = new ConnectionPanel(m_conf_check_callback,
						 m_bHasConfiguration,
						 this);
	pSizerAll->Add(m_pConnectionPanel, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 0);

	wxBoxSizer* itemBoxSizer7 = new wxBoxSizer(wxHORIZONTAL);
	pSizerAll->Add(itemBoxSizer7, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 0);

	wxButton* itemButton17 = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	itemBoxSizer7->Add(itemButton17, 0, wxALIGN_LEFT|wxALL, 5);

	wxButton* itemButton18 = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	itemBoxSizer7->Add(itemButton18, 0, wxALIGN_LEFT|wxALL, 5);

	////@end ConnectionDialog content construction
}

/*!
 * Should we show tooltips?
 */
bool ConnectionDialog::ShowToolTips()
{
	return TRUE;
}

/*!
 * Get bitmap resources
 */
wxBitmap ConnectionDialog::GetBitmapResource( const wxString& name )
{
	(void)(name); // Silence a warning

	// Bitmap retrieval
	////@begin ConnectionDialog bitmap retrieval
	return wxNullBitmap;
	////@end ConnectionDialog bitmap retrieval
}

/*!
 * Get icon resources
 */
wxIcon ConnectionDialog::GetIconResource( const wxString& name )
{
	(void)(name); // Silence a warning

	// Icon retrieval
	////@begin ConnectionDialog icon retrieval
	return wxNullIcon;
	////@end ConnectionDialog icon retrieval
}


eBackendKind ConnectionDialog::getBackend()
{
	return m_pConnectionPanel->getBackend();
}

wxString ConnectionDialog::getHost()
{
	return m_pConnectionPanel->getHost();
}

wxString ConnectionDialog::getPassword()
{
	return m_pConnectionPanel->getPassword();
}

wxString ConnectionDialog::getUser()
{
	return m_pConnectionPanel->getUser();
}

wxString ConnectionDialog::getDatabase()
{
	return m_pConnectionPanel->getDatabase();
}

wxString ConnectionDialog::getConfiguration()
{
	return m_pConnectionPanel->getConfiguration();
}


void ConnectionDialog::setBackend(eBackendKind value)
{
	m_pConnectionPanel->setBackend(value);
}

void ConnectionDialog::setHost(wxString value)
{
	m_pConnectionPanel->setHost(value);
	TransferDataToWindow();
}

void ConnectionDialog::setPassword(wxString value)
{
	m_pConnectionPanel->setPassword(value);
	TransferDataToWindow();
}

void ConnectionDialog::setUser(wxString value)
{
	m_pConnectionPanel->setUser(value);
	TransferDataToWindow();
}

void ConnectionDialog::setDatabase(wxString value)
{
	m_pConnectionPanel->setDatabase(value);
	TransferDataToWindow();
}

void ConnectionDialog::setConfiguration(wxString value)
{
	m_pConnectionPanel->setConfiguration(value);
	TransferDataToWindow();
}
