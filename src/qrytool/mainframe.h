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

/*
 * Copyright (C) 2001-2018     Ulrik Sandborg-Petersen
 * Copyright (C) 2018-present  Sandborg-Petersen Holding ApS, Denmark
 *
 * Licensed under the MIT License.
 *
 * Please see the file COPYING in the root of the sources for more details.
 *
 */



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
