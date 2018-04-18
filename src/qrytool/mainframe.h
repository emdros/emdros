/*
 * mainframe.h
 *
 * main frame in mqlqtwx.
 * mqlqtwx is a graphical front-end for querying Emdros databases.
 *
 * Ulrik Petersen
 * Created: 4/13-2005
 * Last update: 2/28-2007
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2005-2007  Ulrik Petersen
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

#ifndef _MAINFRAME_H_
#define _MAINFRAME_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "mainframe.cpp"
#endif


/*!
 * Includes
 */

////@begin includes
#include "wx/frame.h"
#include "wx/toolbar.h"
#include "wx/string.h"
#include "wx/statusbr.h"
#include "wx/splitter.h"
#include "wx/treectrl.h"
#include "wx/wxhtml.h"
#include <emdros_environment.h>
#include <string>
#include <conf.h>
#include <layout.h>
#include <emdf_enums.h>
#include "wxilqtout.h"
#include "schema.h"
#include "emdros_wrapper.h"
////@end includes

typedef class ConnectionData {
public:
	eBackendKind m_backend_kind;
	wxString m_strHost;
	wxString m_strConfiguration;
	wxString m_strDatabase;
	wxString m_strUser;
	wxString m_strPassword;
	ConnectionData() { m_backend_kind = DEFAULT_BACKEND_ENUM; };
} ConnectionData;

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxSplitterWindow;
class WXILQTOutput;
class WXILLayoutCanvas;
////@end forward declarations



/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_FRAME 10000
#define SYMBOL_MAINFRAME_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxMINIMIZE_BOX|wxMAXIMIZE_BOX
#define SYMBOL_MAINFRAME_TITLE _("Emdros Query Tool")
#define SYMBOL_MAINFRAME_IDNAME ID_FRAME
#define SYMBOL_MAINFRAME_SIZE wxSize(720, 540)
#define SYMBOL_MAINFRAME_POSITION wxDefaultPosition
#define ID_MENU_TOOLS_NEW_CONNECTION 10005
#define ID_MENU_TOOLS_EXECUTE_QUERY 10006
#define ID_MENU_TOOLS_STOP_QUERY 11230
#define ID_MENU_TOOLS_CONFIGURE 10007
#define ID_MENU_HELP_ABOUT 10008
#define ID_MENU_HELP_HELP_CONTENTS 11235
#define ID_MENU_HELP_MQL_CHEAT_SHEET 11236
#define ID_MENU_FILE_SAVE_OUTPUT_AS 11237
#define ID_MENU_FILE_CLEAR_OUTPUT_WINDOW 11238
#define ID_TOOLBAR 10002
#define ID_STATUSBAR 10003
#define ID_SPLITTERWINDOW 10004
#define ID_SPLITTERWINDOW_VERTICAL 11231
#define ID_TREECTRL_SCHEMA 11232
#define ID_TEXTCTRL_EDIT_WINDOW 10009
#define ID_TEXTCTRL_RESULTS 10010
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

class MyTextCtrl: public wxTextCtrl
{
 protected:
	DECLARE_CLASS( MyTextCtrl )
	DECLARE_EVENT_TABLE()
 public:
	/// Constructors
	MyTextCtrl() {};
	MyTextCtrl(wxWindow* parent, 
		   wxWindowID id = SYMBOL_MAINFRAME_IDNAME, 
		   wxString title = wxT(""),
		   const wxPoint& pos = SYMBOL_MAINFRAME_POSITION, 
		   const wxSize& size = SYMBOL_MAINFRAME_SIZE, 
		   long style = SYMBOL_MAINFRAME_STYLE )
		: wxTextCtrl(parent, id, title, pos, size, style) {
	};
	~MyTextCtrl() {
	};

	void OnChar(wxKeyEvent& event);
};


/*!
 * MainFrame class declaration
 */

class MainFrame: public wxFrame
{    
	DECLARE_CLASS( MainFrame )
	DECLARE_EVENT_TABLE()

public:
	/// Constructors
	MainFrame( );
	MainFrame( wxWindow* parent, 
		   wxWindowID id = SYMBOL_MAINFRAME_IDNAME, 
		   const wxString& caption = SYMBOL_MAINFRAME_TITLE, 
		   const wxPoint& pos = SYMBOL_MAINFRAME_POSITION, 
		   const wxSize& size = SYMBOL_MAINFRAME_SIZE, 
		   long style = SYMBOL_MAINFRAME_STYLE );
	~MainFrame();

	bool Create( wxWindow* parent, wxWindowID id = SYMBOL_MAINFRAME_IDNAME, const wxString& caption = SYMBOL_MAINFRAME_TITLE, const wxPoint& pos = SYMBOL_MAINFRAME_POSITION, const wxSize& size = SYMBOL_MAINFRAME_SIZE, long style = SYMBOL_MAINFRAME_STYLE );

	/// Creates the controls and sizers
	void CreateControls();

	////@begin MainFrame event handler declarations
	void OnAbout(wxCommandEvent& event);
	void OnMaximize(wxMaximizeEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnHelpContents(wxCommandEvent& event);
	void OnHelpMQLCheatSheet(wxCommandEvent& event);
	void OnFileNew(wxCommandEvent& event);
	void OnFileOpen(wxCommandEvent& event);
	void OnFileSave(wxCommandEvent& event);
	void OnFileSaveAs(wxCommandEvent& event);
	void OnFileSaveOutputAs(wxCommandEvent& event);
	void OnClearOutputWindow(wxCommandEvent& event);
	void OnFileExit(wxCommandEvent& event);
	void OnEditCopy(wxCommandEvent& event);
	void OnEditCut(wxCommandEvent& event);
	void OnEditPaste(wxCommandEvent& event);
	void OnToolsNewConnection(wxCommandEvent& event);
	void OnToolsExecuteQuery(wxCommandEvent& event);
	void OnToolsStopQuery(wxCommandEvent& event);
	void OnToolsConfigure(wxCommandEvent& event);
	void OnUpdateUIExec(wxUpdateUIEvent& event);
	void OnUpdateUINoExec(wxUpdateUIEvent& event);
	////@end MainFrame event handler declarations

	////@begin MainFrame member function declarations

	/// Retrieves bitmap resources
	wxBitmap GetBitmapResource( const wxString& name );

	wxTextCtrl *GetFocusTextCtrl(void);
	void ReadQuery();
	void SaveQuery();
	void readConfig();
	void writeConfig();
	void SaveTextCtrl(const wxString& filename, wxTextCtrl *pTextCtrl);
	void SaveOutputAreaAsHTML(const wxString& filename);
	void ClearTextCtrls(void);
	void ClearOutputWindow(void);
	void ClearSchemaTree(wxString msg);
	void PopulateSchemaTree(void);
	void AddEnumerationsToSchemaTree(EmdrosSchema *pSchema);
	void AddObjectTypesToSchemaTree(EmdrosSchema *pSchema);
	void AddMonadSetsToSchemaTree(EmdrosSchema *pSchema);
	bool ExecuteEmdrosString(std::string query);

	void SetCurFileName(const wxString& strFileName);

	/// Retrieves icon resources
	wxIcon GetIconResource( const wxString& name );
    
	bool Connect();

	void ResizeSplitters(void);
	void TreeControlCopyToClipboard(wxTreeCtrl* pTree);

	////@end MainFrame member function declarations

	/// Should we show tooltips?
	static bool ShowToolTips();

	////@begin MainFrame member variables
	wxSplitterWindow* m_ctrlSplitterQueryResults;
	wxSplitterWindow* m_ctrlSplitterLeftRight;
	MyTextCtrl* m_pEditWindow;
	WXILLayoutCanvas* m_pResultsWindow;
	wxTreeCtrl* m_pSchemaTree;
	wxString m_strCurFileName;
	ConnectionData m_connectionData;
	EmdrosEnv *m_pEE;
	EmdrosWrapper *m_pEW;
	Configuration *m_pConf;
	WXILQTOutput *m_pOut;
	wxToolBar* m_ctrlToolBar;
	bool m_bExecuting;
	wxString m_strQueryFileDir;
	////@end MainFrame member variables
};

#endif
// _MAINFRAME_H_
