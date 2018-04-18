/*
 * conndlg.h
 *
 * The complex connection dialog in mqlqtwx.
 * mqlqtwx is a graphical front-end for querying Emdros databases.
 *
 * Ulrik Petersen
 * Created: 4/13-2005
 * Last update: 3/31-2013
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2005-2012  Ulrik Sandborg-Petersen
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

#ifndef _CONNDLG_H_
#define _CONNDLG_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "conndlg.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/valtext.h"
#include "emdf_enums.h"
#include "conf.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG 10011
#define SYMBOL_CONNECTIONDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_CONNECTIONDIALOG_TITLE _("Connection settings...")
#define SYMBOL_CONNECTIONDIALOG_IDNAME ID_DIALOG
#define SYMBOL_CONNECTIONDIALOG_SIZE wxSize(400, 300)
#define SYMBOL_CONNECTIONDIALOG_POSITION wxDefaultPosition
#define ID_TEXTCTRL 10012
#define ID_TEXTCTRL1 10013
#define ID_TEXTCTRL2 10014
#define ID_TEXTCTRL3 10015
#define ID_TEXTCTRL4 10016
#define ID_TEXTCTRL5 10017
#define ID_BUTTON_BROWSE_CONF 10018
#define ID_BUTTON_BROWSE_DATABASE 10019
#define ID_COMBOBOX1 10020
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif
#ifndef wxFIXED_MINSIZE
#define wxFIXED_MINSIZE 0
#endif

/*!
 * ConnectionDialog class declaration
 */


class ConnectionPanel: public wxPanel
{    
	DECLARE_DYNAMIC_CLASS( ConnectionPanel )
		DECLARE_EVENT_TABLE()

		public:
	/// Constructors
	ConnectionPanel( );
	ConnectionPanel( app_checkConfigurationFunc_t conf_check_callback, bool bHasConfiguration, wxWindow* parent, wxWindowID id = SYMBOL_CONNECTIONDIALOG_IDNAME, const wxString& caption = SYMBOL_CONNECTIONDIALOG_TITLE, const wxPoint& pos = SYMBOL_CONNECTIONDIALOG_POSITION, const wxSize& size = SYMBOL_CONNECTIONDIALOG_SIZE, long style = SYMBOL_CONNECTIONDIALOG_STYLE );
	virtual ~ConnectionPanel();

	/// Creation
	bool Create( app_checkConfigurationFunc_t conf_check_callback,  wxWindow* parent, wxWindowID id = SYMBOL_CONNECTIONDIALOG_IDNAME, const wxString& caption = SYMBOL_CONNECTIONDIALOG_TITLE, const wxPoint& pos = SYMBOL_CONNECTIONDIALOG_POSITION, const wxSize& size = SYMBOL_CONNECTIONDIALOG_SIZE, long style = SYMBOL_CONNECTIONDIALOG_STYLE );

	/// Creates the controls and sizers
	void CreateControls();

	////@begin ConnectionPanel event handler declarations
	void OnBrowseConfiguration(wxCommandEvent& event);
	void OnBrowseDatabase(wxCommandEvent& event);
	void OnBackendComboBoxChangeSelection(wxCommandEvent& event);
	void OnUpdateBtnBrowseDatabase(wxUpdateUIEvent& event);
	void DoUpdateBtnBrowseDatabase();
	void DoInitialUpdateBtnBrowseDatabase();
	////@end ConnectionPanel event handler declarations

	////@begin ConnectionPanel member function declarations
	// Getters
	eBackendKind getBackendKindFromComboBoxString(void);
	eBackendKind getBackend();
	wxString getHost();
	wxString getPassword();
	wxString getUser();
	wxString getDatabase();
	wxString getConfiguration();

	// Setters
	void setBackend(eBackendKind value);
	void setHost(wxString value);
	void setPassword(wxString value);
	void setUser(wxString value);
	void setDatabase(wxString value);
	void setConfiguration(wxString value);


	/// Retrieves bitmap resources
	wxBitmap GetBitmapResource( const wxString& name );

	/// Retrieves icon resources
	wxIcon GetIconResource( const wxString& name );
	////@end ConnectionPanel member function declarations

	/// Should we show tooltips?
	static bool ShowToolTips();

	// Configuration
	void readConfig();
	void writeConfig();
	

	////@begin ConnectionPanel member variables
	eBackendKind m_backend;
	wxString m_strDatabase;
	wxString m_strConfiguration;
	wxString m_strHost;
	wxString m_strUser;
	wxString m_strPassword;

	wxString m_strConfigurationFileDir;
	wxString m_strConfigurationFileFile;
	wxString m_strDatabaseDir;
	wxString m_strDatabaseFile;

	bool m_bHasConfiguration;

	wxTextCtrl* m_ctrlDatabase;
	wxTextCtrl* m_ctrlConfiguration;
	wxButton* m_btnBrowseConfiguration;

	wxFlexGridSizer* m_sizerAll;
	wxBoxSizer* m_sizerConfiguration;
	wxBoxSizer* m_sizerSimple;
	wxFlexGridSizer* m_sizerNonSimple;
	wxButton *m_btnBrowseDatabase;
	wxComboBox * m_cbBackendCB;

	wxTextCtrl *m_itemTextCtrlUser;
	wxTextCtrl* m_itemTextCtrlPassword;
	wxTextCtrl* m_itemTextCtrlHost;


	app_checkConfigurationFunc_t m_conf_check_callback;
	////@end ConnectionPanel member variables
};

class ConnectionDialog: public wxDialog
{    
	DECLARE_DYNAMIC_CLASS( ConnectionDialog )
		DECLARE_EVENT_TABLE()
   

		public:
	/// Constructors
	ConnectionDialog( );
	ConnectionDialog( app_checkConfigurationFunc_t conf_check_callback, bool bHasConfiguration, wxWindow* parent, wxWindowID id = SYMBOL_CONNECTIONDIALOG_IDNAME, const wxString& caption = SYMBOL_CONNECTIONDIALOG_TITLE, const wxPoint& pos = SYMBOL_CONNECTIONDIALOG_POSITION, const wxSize& size = SYMBOL_CONNECTIONDIALOG_SIZE, long style = SYMBOL_CONNECTIONDIALOG_STYLE );

	/// Creation
	bool Create( app_checkConfigurationFunc_t conf_check_callback, bool bHasConfiguration, wxWindow* parent, wxWindowID id = SYMBOL_CONNECTIONDIALOG_IDNAME, const wxString& caption = SYMBOL_CONNECTIONDIALOG_TITLE, const wxPoint& pos = SYMBOL_CONNECTIONDIALOG_POSITION, const wxSize& size = SYMBOL_CONNECTIONDIALOG_SIZE, long style = SYMBOL_CONNECTIONDIALOG_STYLE );

	/// Creates the controls and sizers
	void CreateControls();

	////@begin ConnectionDialog event handler declarations
	////@end ConnectionDialog event handler declarations

	////@begin ConnectionDialog member function declarations
	eBackendKind getBackendKindFromComboBoxString(void);

	/// Retrieves bitmap resources
	wxBitmap GetBitmapResource( const wxString& name );

	/// Retrieves icon resources
	wxIcon GetIconResource( const wxString& name );
	////@end ConnectionDialog member function declarations

	/// Should we show tooltips?
	static bool ShowToolTips();

	// Getters
	eBackendKind getBackend();
	wxString getHost();
	wxString getPassword();
	wxString getUser();
	wxString getDatabase();
	wxString getConfiguration();
	
	// Setters
	void setBackend(eBackendKind value);
	void setHost(wxString value);
	void setPassword(wxString value);
	void setUser(wxString value);
	void setDatabase(wxString value);
	void setConfiguration(wxString value);

	////@begin ConnectionDialog member variables
	ConnectionPanel *m_pConnectionPanel;
	bool m_bHasConfiguration;

	app_checkConfigurationFunc_t m_conf_check_callback;
	////@end ConnectionDialog member variables
};

/*!
 * ConnectionPanel class declaration
 */

#endif
// _CONNDLG_H_
