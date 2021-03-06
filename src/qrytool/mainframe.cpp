/*
 * mainframe.cpp
 *
 * Main frame in mqlqtwx.
 * mqlqtwx is a graphical front-end for querying Emdros databases.
 *
 * Ulrik Petersen
 * Created: 4/13-2005
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
#pragma implementation "mainframe.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

////@begin includes
// #include <wx/stdpaths.h> // Not available in 2.4.2
#include <wx/utils.h> // Instead of stdpaths
#include <wx/filename.h>
#include <wx/filedlg.h>
#include <wx/strconv.h>
#include <wx/fs_zip.h>
#include <wx/clipbrd.h>
#include <wx/dataobj.h>
#include <wx/version.h>
#include <fstream>
#include <sstream>
#include "exec.h"
#include <wxutil_emdros.h>
#include <prefix_emdros.h>
#include "qtconnwizard.h"

////@end includes

#include "mainframe.h"
#include "conndlg.h"
#include "mqlqtwx.h"
#include <monads.h>

////@begin XPM images
#include <connect.xpm>
#include <new.xpm>
#include <open.xpm>
#include <save.xpm>
#include <copy.xpm>
#include <cut.xpm>
#include <paste.xpm>
#include <flash.xpm>
#include <stop.xpm>
#include <clear.xpm>
////@end XPM images


#define min(X,Y)  ((X < Y) ? X : Y)



/*!
 * MainFrame type definition
 */

IMPLEMENT_CLASS( MainFrame, wxFrame )
IMPLEMENT_CLASS( MyTextCtrl, wxTextCtrl )

/*!
 * MainFrame event table definition
 */

BEGIN_EVENT_TABLE( MainFrame, wxFrame )
////@begin MainFrame event table entries
	EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
	EVT_MENU(wxID_HELP, MainFrame::OnHelpContents)
	EVT_MENU(ID_MENU_HELP_MQL_CHEAT_SHEET, MainFrame::OnHelpMQLCheatSheet)
	EVT_MENU(wxID_NEW, MainFrame::OnFileNew)
	EVT_MENU(wxID_OPEN, MainFrame::OnFileOpen)
	EVT_MENU(wxID_SAVE, MainFrame::OnFileSave)
	EVT_MENU(wxID_SAVEAS, MainFrame::OnFileSaveAs)
	EVT_MENU(ID_MENU_FILE_SAVE_OUTPUT_AS, MainFrame::OnFileSaveOutputAs)
	EVT_MENU(wxID_CUT, MainFrame::OnEditCut)
	EVT_MENU(wxID_COPY, MainFrame::OnEditCopy)
	EVT_MENU(wxID_PASTE, MainFrame::OnEditPaste)
	EVT_MENU(ID_MENU_TOOLS_NEW_CONNECTION, MainFrame::OnToolsNewConnection)
	EVT_MENU(ID_MENU_TOOLS_EXECUTE_QUERY, MainFrame::OnToolsExecuteQuery)
	EVT_MENU(ID_MENU_TOOLS_STOP_QUERY, MainFrame::OnToolsStopQuery)
	EVT_MENU(ID_MENU_TOOLS_CONFIGURE, MainFrame::OnToolsConfigure)
	EVT_MENU(wxID_EXIT, MainFrame::OnFileExit)
	EVT_MENU(ID_MENU_FILE_CLEAR_OUTPUT_WINDOW, MainFrame::OnClearOutputWindow)

	EVT_UPDATE_UI(wxID_ABOUT, MainFrame::OnUpdateUINoExec)
	EVT_UPDATE_UI(wxID_HELP, MainFrame::OnUpdateUINoExec)
	EVT_UPDATE_UI(ID_MENU_HELP_MQL_CHEAT_SHEET, MainFrame::OnUpdateUINoExec)
	EVT_UPDATE_UI(wxID_NEW, MainFrame::OnUpdateUINoExec)
	EVT_UPDATE_UI(wxID_OPEN, MainFrame::OnUpdateUINoExec)
	EVT_UPDATE_UI(wxID_SAVE, MainFrame::OnUpdateUINoExec)
	EVT_UPDATE_UI(wxID_SAVEAS, MainFrame::OnUpdateUINoExec)
	EVT_UPDATE_UI(ID_MENU_FILE_SAVE_OUTPUT_AS, MainFrame::OnUpdateUINoExec)
	EVT_UPDATE_UI(wxID_CUT, MainFrame::OnUpdateUINoExec)
	EVT_UPDATE_UI(wxID_COPY, MainFrame::OnUpdateUINoExec)
	EVT_UPDATE_UI(wxID_PASTE, MainFrame::OnUpdateUINoExec)
	EVT_UPDATE_UI(ID_MENU_TOOLS_NEW_CONNECTION, MainFrame::OnUpdateUINoExec)
	EVT_UPDATE_UI(ID_MENU_TOOLS_EXECUTE_QUERY, MainFrame::OnUpdateUINoExec)
	EVT_UPDATE_UI(ID_MENU_TOOLS_STOP_QUERY, MainFrame::OnUpdateUIExec)
	EVT_UPDATE_UI(ID_MENU_TOOLS_CONFIGURE, MainFrame::OnUpdateUINoExec)
	EVT_UPDATE_UI(wxID_EXIT, MainFrame::OnUpdateUINoExec)

	EVT_MAXIMIZE(MainFrame::OnMaximize)
	EVT_SIZE(MainFrame::OnSize)

////@end MainFrame event table entries

END_EVENT_TABLE()




BEGIN_EVENT_TABLE( MyTextCtrl, wxTextCtrl )
EVT_CHAR(MyTextCtrl::OnChar)
END_EVENT_TABLE()


wxString getInputAreaFontName(Configuration *pConf)
{
	wxString inputAreaFontName;
	std::string str("input_area_font_name");
	if (pConf != 0 && pConf->has_key(str)) {
		std::string input_area_font_name = pConf->getValues(str)[0];
		inputAreaFontName = wxString(input_area_font_name.c_str(), wxConvUTF8);
	} else {
#ifdef __WXMSW__
#if wxUSE_UNICODE
		inputAreaFontName = wxString(wxT("Arial MS Unicode"));
#else
		inputAreaFontName = wxString(wxT("Courier New"));
#endif
#else  // Not Windows
		inputAreaFontName = wxString(wxT("Courier"));
#endif
	}
	return inputAreaFontName;
}




/*!
 * MainFrame constructors
 */

MainFrame::MainFrame( )
		: m_pEE(0), m_pEW(0), m_pConf(0), m_pOut(0), m_bExecuting(false)
{
	readConfig();
}

MainFrame::~MainFrame()
{
	writeConfig();
	delete m_pEE;
	delete m_pEW;
	delete m_pConf;
	delete m_pOut;
}

MainFrame::MainFrame( wxWindow* parent, 
		      wxWindowID id, 
		      const wxString& caption, 
		      const wxPoint& pos, 
		      const wxSize& size, 
		      long style )
	: wxFrame(parent, id, caption, pos, size, style), m_pEE(0), m_pEW(0), m_pConf(0), m_pOut(0), m_bExecuting(false)
{
	readConfig();

	//Create( parent, id, caption, pos, size, style );
	CreateControls();

	// Set default style of text controls
	if (m_pOut != 0) {
		ClearEditWindowAndSetDefaultStyle();		
	}
	m_pOut->print(std::string((const char*) (wxString(wxT("Welcome.  Press the first button ")
							  wxT("(\"connect to database\") in ")
							  wxT("order to specify a ")
							  wxT("configuration file (*.cfg).\n")
							  wxT("See the User's Guide for ")
							  wxT("details on how to write a ")
							  wxT("configuration file. ")
							  wxT("Press F1 to get the User's Guide.\n")).mb_str(wxConvUTF8))));

	Centre();
}

/*!
 * MainFrame creator
 */

bool MainFrame::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
	////@begin MainFrame creation
	wxFrame::Create( parent, id, caption, pos, size, style );

	CreateControls();
	Centre();

	////@end MainFrame creation
	return TRUE;
}

void MainFrame::SetCurFileName(const wxString& strFileName)
{
	m_strCurFileName = strFileName;
	if (m_strCurFileName.IsEmpty()) {
		SetTitle(wxT("Emdros Query Tool"));
	} else {
		SetTitle(wxT("EQT: ") + m_strCurFileName);
	}
}

/*!
 * Control creation for MainFrame
 */

void MainFrame::CreateControls()
{    
	////@begin MainFrame content construction
	////@begin MainFrame member initialisation
	m_ctrlSplitterQueryResults = NULL;
	m_ctrlSplitterLeftRight = NULL;
	m_pEditWindow = NULL;
	m_pResultsWindow = NULL;
	m_ctrlToolBar = NULL;
	////@end MainFrame member initialisation


	SetCurFileName(wxT(""));
	MainFrame* itemFrame1 = this;

	//m_ctrlToolBar = new wxToolBar( itemFrame1, ID_TOOLBAR, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORIZONTAL );
	m_ctrlToolBar = this->CreateToolBar(wxTB_FLAT|wxTB_HORIZONTAL, ID_TOOLBAR);
	m_ctrlToolBar->SetToolBitmapSize(wxSize(48,48));
	itemFrame1->SetToolBar(m_ctrlToolBar);

	wxBitmap connectBitmap(itemFrame1->GetBitmapResource(wxT("connect.xpm")));
	m_ctrlToolBar->AddTool(ID_MENU_TOOLS_NEW_CONNECTION, wxT(""), connectBitmap, wxT("Connect to database"), wxITEM_NORMAL);
	m_ctrlToolBar->AddSeparator();

	wxBitmap itemtool21Bitmap(itemFrame1->GetBitmapResource(wxT("new.xpm")));
	m_ctrlToolBar->AddTool(wxID_NEW, wxT(""), itemtool21Bitmap, wxT("New query"), wxITEM_NORMAL);
	wxBitmap itemtool22Bitmap(itemFrame1->GetBitmapResource(wxT("open.xpm")));
	m_ctrlToolBar->AddTool(wxID_OPEN, wxT(""), itemtool22Bitmap, wxT("Open query"), wxITEM_NORMAL);
	wxBitmap itemtool23Bitmap(itemFrame1->GetBitmapResource(wxT("save.xpm")));
	m_ctrlToolBar->AddTool(wxID_SAVE, wxT(""), itemtool23Bitmap, wxT("Save query"), wxITEM_NORMAL);
	m_ctrlToolBar->AddSeparator();
	wxBitmap itemtool25Bitmap(itemFrame1->GetBitmapResource(wxT("copy.xpm")));
	m_ctrlToolBar->AddTool(wxID_COPY, wxT(""), itemtool25Bitmap, wxT("Copy to clipboard"), wxITEM_NORMAL);
	wxBitmap itemtool26Bitmap(itemFrame1->GetBitmapResource(wxT("cut.xpm")));
	m_ctrlToolBar->AddTool(wxID_CUT, wxT(""), itemtool26Bitmap, wxT("Cut to clipboard"), wxITEM_NORMAL);
	wxBitmap itemtool27Bitmap(itemFrame1->GetBitmapResource(wxT("paste.xpm")));
	m_ctrlToolBar->AddTool(wxID_PASTE, wxT(""), itemtool27Bitmap, wxT("Paste from clipboard"), wxITEM_NORMAL);
	m_ctrlToolBar->AddSeparator();
	wxBitmap itemtool28Bitmap(itemFrame1->GetBitmapResource(wxT("flash.xpm")));
	m_ctrlToolBar->AddTool(ID_MENU_TOOLS_EXECUTE_QUERY, wxT("Execute query"), itemtool28Bitmap, wxT(""), wxITEM_NORMAL);
	wxBitmap itemtool29Bitmap(itemFrame1->GetBitmapResource(wxT("stop.xpm")));
	m_ctrlToolBar->AddTool(ID_MENU_TOOLS_STOP_QUERY, wxT("Stop execution"), itemtool29Bitmap, wxT(""), wxITEM_NORMAL);
	wxBitmap itemtool30Bitmap(itemFrame1->GetBitmapResource(wxT("clear.xpm")));
	m_ctrlToolBar->AddTool(ID_MENU_FILE_CLEAR_OUTPUT_WINDOW, wxT("Clear output window"), itemtool30Bitmap, wxT(""), wxITEM_NORMAL);

	//wxStatusBar* itemStatusBar27 = new wxStatusBar( itemFrame1, ID_STATUSBAR, wxST_SIZEGRIP|wxNO_BORDER );
	//itemStatusBar27->SetFieldsCount(2);
	wxStatusBar* itemStatusBar27 = this->CreateStatusBar(2, wxST_SIZEGRIP|wxNO_BORDER, ID_STATUSBAR);
	itemFrame1->SetStatusBar(itemStatusBar27);

	m_ctrlSplitterLeftRight = new wxSplitterWindow( itemFrame1, ID_SPLITTERWINDOW_VERTICAL, wxDefaultPosition, wxSize(100, 100), wxSP_3DBORDER|wxSP_3DSASH|wxNO_BORDER );

	m_pSchemaTree = new wxTreeCtrl(m_ctrlSplitterLeftRight, ID_TREECTRL_SCHEMA, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE, wxDefaultValidator, wxT("Schema tree"));
	/*
	if (ShowToolTips())
		m_pSchemaTree->SetToolTip(wxT("The schema is shown here if you are connected to a database."));
	*/

	m_ctrlSplitterQueryResults = new wxSplitterWindow( m_ctrlSplitterLeftRight, ID_SPLITTERWINDOW, wxDefaultPosition, wxSize(100, 100), wxSP_3DBORDER|wxSP_3DSASH|wxNO_BORDER );

#if wxCHECK_VERSION(3,0,0)
	m_pEditWindow = new wxStyledTextCtrl(m_ctrlSplitterQueryResults,
					     ID_TEXTCTRL_EDIT_WINDOW,
					     wxDefaultPosition, 
					     wxDefaultSize,
					     0); // style
					 
					 
					 
	m_pEditWindow->SetHelpText(wxT("For writing your query or configuration."));


#elif wxCHECK_VERSION(2,8,0)
	m_pEditWindow = new MyTextCtrl( m_ctrlSplitterQueryResults,
 					ID_TEXTCTRL_EDIT_WINDOW,
					wxT(""), 
					wxDefaultPosition, 
					wxDefaultSize, 
					wxTE_MULTILINE | wxTE_RICH2 | wxTE_PROCESS_TAB );

	if (ShowToolTips()) {
		m_pEditWindow->SetToolTip(wxT("Write your query or configuration here."));
	}

#endif

	ClearEditWindowAndSetDefaultStyle();


	m_pResultsWindow = new WXILLayoutCanvas( m_ctrlSplitterQueryResults, wxDefaultPosition, wxDefaultSize, 0, m_pConf);
#if wxCHECK_VERSION(3,0,0)
	m_pResultsWindow->SetHelpText(wxT("The results will be shown here."));
#elif wxCHECK_VERSION(2,8,0)
	if (ShowToolTips())  {
		m_pResultsWindow->SetToolTip(wxT("The results will be shown here."));
	}
#endif
	
	m_ctrlSplitterQueryResults->SplitHorizontally(m_pEditWindow, m_pResultsWindow, 25);

	m_ctrlSplitterLeftRight->SplitVertically(m_ctrlSplitterQueryResults, m_pSchemaTree, 200);

	wxMenuBar* menuBar = new wxMenuBar;
	wxMenu* itemMenu3 = new wxMenu;
	itemMenu3->Append(wxID_NEW, wxT("New..."), wxT(""), wxITEM_NORMAL);
	itemMenu3->Append(wxID_OPEN, wxT("&Open"), wxT("Opens a query or a configuration file."), wxITEM_NORMAL);
	itemMenu3->Append(wxID_SAVE, wxT("&Save"), wxT("Saves the current query."), wxITEM_NORMAL);
	itemMenu3->Append(wxID_SAVEAS, wxT("Save &as"), wxT("Saves the current query as..."), wxITEM_NORMAL);
	itemMenu3->Append(ID_MENU_FILE_SAVE_OUTPUT_AS, wxT("Sa&ve output as HTML"), wxT("Saves the current output as HTML."), wxITEM_NORMAL);
	itemMenu3->Append(ID_MENU_FILE_CLEAR_OUTPUT_WINDOW, wxT("&Clear output window"), wxT("Clear output window"), wxITEM_NORMAL);
	itemMenu3->AppendSeparator();
	itemMenu3->Append(wxID_EXIT, wxT("E&xit"), wxT(""), wxITEM_NORMAL);
	menuBar->Append(itemMenu3, wxT("&File"));
	wxMenu* itemMenu10 = new wxMenu;
	itemMenu10->Append(wxID_COPY, wxT("Copy\tCtrl-C"), wxT("Copy the current selection to clipboard."), wxITEM_NORMAL);
	itemMenu10->Append(wxID_CUT, wxT("Cut\tCtrl-X"), wxT("Cuts the current selection to clipboard."), wxITEM_NORMAL);
	itemMenu10->Append(wxID_PASTE, wxT("Paste\tCtrl-V"), wxT("Pastes the current clipboard contents."), wxITEM_NORMAL);
	menuBar->Append(itemMenu10, wxT("&Edit"));
	wxMenu* itemMenu14 = new wxMenu;
	itemMenu14->Append(ID_MENU_TOOLS_NEW_CONNECTION, wxT("New &Connection..."), wxT(""), wxITEM_NORMAL);
	itemMenu14->Append(ID_MENU_TOOLS_EXECUTE_QUERY, wxT("E&xecute Query"), wxT("Executes the current query."), wxITEM_NORMAL);
	/*
	itemMenu14->Append(ID_MENU_TOOLS_CONFIGURE, wxT("Configure..."), wxT("Lets you create or edit a configuration file."), wxITEM_NORMAL);
	*/
	menuBar->Append(itemMenu14, wxT("&Tools"));
	wxMenu* itemMenu18 = new wxMenu;
	itemMenu18->Append(wxID_HELP, wxT("&Help Contents\tF1"), wxT("Brings up the help viewer"), wxITEM_NORMAL);
	itemMenu18->Append(ID_MENU_HELP_MQL_CHEAT_SHEET, wxT("MQL query language &Cheat sheet\tF2"), wxT("Brings up the help viewer with the MQL query language cheat sheet"), wxITEM_NORMAL);
#ifndef __WXMAC__
	// On Mac OS X, the About menu-item is moved to the Application Menu,
	// so we don't want a separator here.
	itemMenu18->AppendSeparator();
#endif
	itemMenu18->Append(wxID_ABOUT, wxT("A&bout Emdros Query Tool..."), wxT("Brings up the About box"), wxITEM_NORMAL);
	menuBar->Append(itemMenu18, wxT("&Help"));
	itemFrame1->SetMenuBar(menuBar);

	////@end MainFrame content construction
	m_pOut = new WXILQTOutput(m_pResultsWindow);

	wxCommandEvent evt;
	OnFileNew(evt);

	m_ctrlToolBar->Realize();
}

/*!
 * Should we show tooltips?
 */

bool MainFrame::ShowToolTips()
{
	return TRUE;
}

/*!
 * Get bitmap resources
 */

wxBitmap MainFrame::GetBitmapResource( const wxString& name )
{
	// Bitmap retrieval
	////@begin MainFrame bitmap retrieval
	if (name == wxT("new.xpm")) {
		wxBitmap bitmap(new_xpm);
		return bitmap;
	} else if (name == wxT("open.xpm")) {
		wxBitmap bitmap(open_xpm);
		return bitmap;
	} else if (name == wxT("save.xpm")) {
		wxBitmap bitmap(save_xpm);
		return bitmap;
	} else if (name == wxT("copy.xpm")) {
		wxBitmap bitmap(copy_xpm);
		return bitmap;
	} else if (name == wxT("cut.xpm")) {
		wxBitmap bitmap(cut_xpm);
		return bitmap;
	} else if (name == wxT("paste.xpm")) {
		wxBitmap bitmap(paste_xpm);
		return bitmap;
	} else if (name == wxT("stop.xpm")) {
		wxBitmap bitmap(stop_xpm);
		return bitmap;
	} else if (name == wxT("flash.xpm")) {
		wxBitmap bitmap(flash_xpm);
		return bitmap;
	} else if (name == wxT("clear.xpm")) {
		wxBitmap bitmap(clear_xpm);
		return bitmap;
	} else if (name == wxT("connect.xpm")) {
		wxBitmap bitmap(connect_xpm);
		return bitmap;
	}
	return wxNullBitmap;
	////@end MainFrame bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon MainFrame::GetIconResource( const wxString& name )
{
	(void)(name); // Silence a warning

	// Icon retrieval
	////@begin MainFrame icon retrieval
	return wxNullIcon;
	////@end MainFrame icon retrieval
}

void MainFrame::OnMaximize(wxMaximizeEvent& event)
{
	ResizeSplitters();

	// Pass it higher up in the hierarchy
	event.Skip();
}

void MainFrame::OnSize(wxSizeEvent& event)
{
	ResizeSplitters();

	// Pass it higher up in the hierarchy
	event.Skip();
}

void MainFrame::ResizeSplitters(void)
{
	if (m_ctrlSplitterQueryResults != 0
	    && m_ctrlSplitterLeftRight != 0) {
		wxSize clientSize = GetClientSize();
		
		int height = clientSize.GetHeight();
		int width = clientSize.GetWidth();
		
		int schematreewidth = min(300, int(width*0.4));
		int leftright_sashposition = width - schematreewidth;
		
		int halfheight = height / 2;
		int queryresults_sashposition = height - halfheight;
		
		m_ctrlSplitterQueryResults->SetSashPosition(queryresults_sashposition, TRUE);
		m_ctrlSplitterLeftRight->SetSashPosition(leftright_sashposition, TRUE);
		
		// Make sure we refresh this frame.
		Refresh();
	}
}


void MainFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
	(void)wxMessageBox(wxT("Emdros Query Tool version 1.5.0 from Emdros version")
			   wxT(EMDROS_VERSION)
			   wxT("\n\nCopyright (C) 2005-2014 Ulrik Sandborg-Petersen\n\nMade available under the MIT License.\n\nEmdros Query Tool comes with NO WARRANTY WHATSOEVER.\n\nSee  https://emdros.org/qrytool  for more information.\n\nPlease visit  https://scripturesys.com  for commercial support and licensing."), 
			   wxT("About Emdros Query Tool"));
}

void MainFrame::OnHelpContents(wxCommandEvent& WXUNUSED(event) )
{
	wxHtmlHelpController *help;
	help = new wxHtmlHelpController(wxHF_DEFAULT_STYLE);

	help->AddBook(wxFileName(EmdrosQueryToolApp::wxHelpPrefix() + wxT("EQTUsersGuide.htb")));
   
	help->DisplayContents();
}

void MainFrame::OnHelpMQLCheatSheet(wxCommandEvent& WXUNUSED(event) )
{
	wxHtmlHelpController *help;
	help = new wxHtmlHelpController(wxHF_DEFAULT_STYLE);

	help->AddBook(wxFileName(EmdrosQueryToolApp::wxHelpPrefix() + wxT("EQTUsersGuide.htb")));
   
	help->DisplaySection(3100);
}

void MainFrame::OnFileNew(wxCommandEvent& event)
{
	(void)(event); // Silence a warning

	ClearTextCtrls();
	SetCurFileName(wxT(""));
	this->Refresh();
}

void MainFrame::OnClearOutputWindow(wxCommandEvent& event)
{
	(void)(event); // Silence a warning	

	ClearOutputWindow();
}

void MainFrame::OnFileOpen(wxCommandEvent& event)
{
	(void)(event); // Silence a warning	

	//wxStandardPaths stdpaths;
	wxFileDialog dlgFileDialog(this,
				   wxT("Choose query file"),
				   //stdpaths.GetUserDataDir(),
				   m_strQueryFileDir,
				   wxT(""),
				   wxT("MQL Queries (*.mql)|*.mql|TXT files (*.txt)|*.txt|All files (*.*)|*.*"),
#if wxCHECK_VERSION(2,8,0)
				   wxFD_OPEN | wxFD_FILE_MUST_EXIST
#else
				   wxOPEN | wxFILE_MUST_EXIST
#endif
				   );
	int nReturn;
	if ((nReturn = dlgFileDialog.ShowModal()) == wxID_OK) {
		SetCurFileName(dlgFileDialog.GetPath());

		// Set m_strQueryFileDir to the path-part of the 
		// gotten filename.
		wxFileName fn(m_strCurFileName);
		m_strQueryFileDir = fn.GetPath();
		
		ReadQuery();
	}
}

void MainFrame::OnFileSave(wxCommandEvent& event)
{
	if (m_strCurFileName.IsEmpty()) {
		OnFileSaveAs(event);
	} else {
		SaveQuery();
	}
}

void MainFrame::OnFileSaveAs(wxCommandEvent& event)
{
	(void)(event); // Silence a warning	

	if (m_strCurFileName.empty()) {
		SetCurFileName(wxT("query.mql"));
	}
	wxString strQueryFileDir;
	wxString strQueryFileFile;
	wxFileName fn(m_strCurFileName);
	if (fn.IsAbsolute()) {
		strQueryFileDir = fn.GetPath();
		strQueryFileFile = fn.GetFullName();
	} else {
		strQueryFileDir = ::wxGetHomeDir();
		strQueryFileFile = wxT("query.mql");
	}

	m_strQueryFileDir = fn.GetPath();
	//wxStandardPaths stdpaths;
	wxFileDialog dlgFileDialog(this,
				   wxT("Choose filename to save as"),
				   strQueryFileDir,
				   strQueryFileFile,
				   wxT("MQL Queries (*.mql)|*.mql|CFG files (*.cfg)|*.cfg|TXT files (*.txt)|*.txt|All files (*)|*"),
#if wxCHECK_VERSION(2,8,0)
				   wxFD_SAVE
#else
				   wxSAVE
#endif
				   );
	int nReturn;
	if ((nReturn = dlgFileDialog.ShowModal()) == wxID_OK) {
		SetCurFileName(dlgFileDialog.GetPath());

		// Set m_strQueryFileDir to the path-part of the 
		// gotten filename.
		wxFileName fn(m_strCurFileName);
		m_strQueryFileDir = fn.GetPath();

		SaveQuery();
	}
}

void MainFrame::OnFileSaveOutputAs(wxCommandEvent& event)
{
	(void)(event); // Silence a warning	

	wxString strOutputFileName = wxT("output.html");

	wxFileDialog dlgFileDialog(this,
				   wxT("Choose filename to save output as HTML"),
				   m_strQueryFileDir,
				   strOutputFileName,
				   wxT("HTML files (*.html)|*.html|All files (*)|*"),
#if wxCHECK_VERSION(2,8,0)
				   wxFD_SAVE
#else
				   wxSAVE
#endif
				   );

	int nReturn;
	if ((nReturn = dlgFileDialog.ShowModal()) == wxID_OK) {
		strOutputFileName = dlgFileDialog.GetPath();

		// Set m_strQueryFileDir to the path-part of the 
		// gotten filename.
		wxFileName fn(m_strCurFileName);
		m_strQueryFileDir = fn.GetPath();

		SaveOutputAreaAsHTML(strOutputFileName);
	}
}

void MainFrame::DoCopyOrCutOrPaste(int eventID, wxCommandEvent& event)
{
	wxWindow *pFocusWindow = wxWindow::FindFocus();
	
	if (eventID == wxID_COPY) {
		if (pFocusWindow == m_pResultsWindow) {
			// Cannot copy from results window
			event.Skip();
		} else if (pFocusWindow == m_pEditWindow) {
			m_pEditWindow->Copy();
		} else {
			TreeControlCopyToClipboard(m_pSchemaTree);
		}
	} else if (eventID == wxID_CUT) {
		if (pFocusWindow == m_pResultsWindow) {
			// Cannot cut from results window
			event.Skip();
		} else if (pFocusWindow == m_pEditWindow) {
			m_pEditWindow->Cut();
		} else {
			// Cannot cut from tree control
			event.Skip();
		}
	} else if (eventID == wxID_PASTE) {
		if (pFocusWindow == m_pResultsWindow) {
			// Cannot paste to results window
			event.Skip();
		} else if (pFocusWindow == m_pEditWindow) {
			m_pEditWindow->Paste();
		} else {
			// Cannot paste to tree control
			event.Skip();
		}
	} else {
		// Unknown eventID
		event.Skip();
	}
}

void MainFrame::OnEditCopy(wxCommandEvent& event)
{
	DoCopyOrCutOrPaste(wxID_COPY, event);
}


void MainFrame::TreeControlCopyToClipboard(wxTreeCtrl* pTree)
{
	// Note: If I ever add the wxTR_MULTIPLE style, I will have to use
	// GetSelections() instead!
	wxTreeItemId treeItemId = pTree->GetSelection();

	// Check that it is OK
	if (treeItemId.IsOk()) {
		wxString label = pTree->GetItemText(treeItemId);

		// If there is a space in there, it means it is
		// a 'OT : feature_type' string, and we only want
		// the object type.  So extract everything up to (but not
		// including) the first space, or the whole string
		// if there is no space.
		wxString toCopy = label.BeforeFirst(wxT(' '));

		// Copy text to clipboard as text.
		if (wxTheClipboard->Open()) {
			wxTheClipboard->SetData(new wxTextDataObject(toCopy));
			wxTheClipboard->Close();
		}
	}
}

void MainFrame::OnEditCut(wxCommandEvent& event)
{
	DoCopyOrCutOrPaste(wxID_CUT, event);
}

void MainFrame::OnEditPaste(wxCommandEvent& event)
{
	DoCopyOrCutOrPaste(wxID_PASTE, event);
}

void MainFrame::ClearEditWindowAndSetDefaultStyle()
{
	int nInputAreaFontSize;
	wxString strInputAreaFontName;
	if (m_pOut != 0) {
		nInputAreaFontSize = m_pOut->getInputAreaFontSize();
		strInputAreaFontName = m_pOut->getInputAreaFontName();
	} else {
		nInputAreaFontSize = 12;
		strInputAreaFontName = wxT("Courier New");
	}
	
	wxFont *pFont = new wxFont(nInputAreaFontSize, 
				   wxFONTFAMILY_MODERN, 
				   wxFONTSTYLE_NORMAL, 
				   wxFONTWEIGHT_NORMAL, 
				   FALSE, 
				   strInputAreaFontName);

	pFont->SetPointSize(nInputAreaFontSize);
	
#if wxCHECK_VERSION(3,0,0)
	m_pEditWindow->ClearAll();
	
	m_pEditWindow->SetFont(*pFont);
#elif wxCHECK_VERSION(2,8,0)
	m_pEditWindow->Clear();
	
	wxTextAttr inputAreaTextAttr(*wxBLACK, wxNullColour,
				     *pFont,
				     wxTEXT_ALIGNMENT_LEFT);
	inputAreaTextAttr.SetFlags(wxTEXT_ATTR_TEXT_COLOUR
				   | wxTEXT_ATTR_FONT_FACE
				   | wxTEXT_ATTR_FONT_SIZE
				   | wxTEXT_ATTR_FONT_WEIGHT
				   | wxTEXT_ATTR_FONT_ITALIC
				   | wxTEXT_ATTR_FONT_UNDERLINE
				   | wxTEXT_ATTR_ALIGNMENT);
	
	m_pEditWindow->SetDefaultStyle(inputAreaTextAttr);
	m_pEditWindow->SetStyle(0, m_pEditWindow->GetLastPosition(), inputAreaTextAttr);
#endif
	delete pFont;
}


wxString MainFrame::GetEditWindowText()
{
	return m_pEditWindow->GetValue();
}

void MainFrame::OnToolsExecuteQuery(wxCommandEvent& event)
{
	(void)(event); // Silence a warning	

	std::istringstream istr(std::string((const char*)(GetEditWindowText().mb_str(wxConvUTF8))));
	if (m_pEE == 0 || !m_pEE->connectionOk()) {
		wxEmdrosErrorMessage(wxT("Error: The connection to the backend is not OK.\n")
				     wxT("You must use \"Tools\"-->\"New connection...\"\n")
				     wxT("to establish a proper connection."));
		return;
	} else {
		m_bExecuting = true;
		int nReturnCode = exec_stream(&istr, m_pEW);
		if (m_pOut->ShouldStop()) {
			wxEmdrosInfoMessage(wxT("You successfully stopped the process."));
			m_pOut->UnStop();
		}
		if (nReturnCode != 0) {
			wxEmdrosErrorMessage(wxT("Error: Something went wrong while executing query."));
		}
		m_bExecuting = false;
	}
	return;
}

void MainFrame::OnToolsStopQuery(wxCommandEvent& event)
{
	(void)(event); // Silence a warning	

	m_pOut->Stop();
}

void MainFrame::OnToolsNewConnection(wxCommandEvent& event)
{
	(void)(event); // Silence a warning	

	ConnectionData conndata;
	if (!wxGetApp().GetConnection(conndata)) {
		// This means that the user pressed 'cancel'.
	} else {
		m_connectionData = conndata;
		Connect();
	}
}

void MainFrame::OnToolsConfigure(wxCommandEvent& event)
{
	(void)(event); // Silence a warning	

	//wxEmdrosInfoMessage(wxT("Sorry. Not implemented yet.\nPlease close this dialog box, then\npress F1 to see the User's Guide about how to write\na *.cfg configuration file."));
	try {
		ConnectionWizard *pWizard = new ConnectionWizard(this);
		if (pWizard->RunIt()) {
			wxEmdrosInfoMessage(wxT("Finished successfully!"));
		} else {
			wxEmdrosInfoMessage(wxT("Aborted."));
		}
	} catch (EmdrosException& e) {
		wxEmdrosInfoMessage(wxString(e.what().c_str(), wxConvUTF8));
	}
}


void MainFrame::OnFileExit(wxCommandEvent& event)
{
	(void)(event); // Silence a warning	

	Close();
}

void MainFrame::ClearTextCtrls(void)
{
#if wxCHECK_VERSION(3,0,0)
	m_pEditWindow->ClearAll();
#elif wxCHECK_VERSION(2,8,0)
	m_pEditWindow->Clear();
#endif
	m_pResultsWindow->Clear();
	this->Refresh();
}

void MainFrame::ClearOutputWindow(void)
{
	m_pResultsWindow->Clear();
	this->Refresh();
}

void MainFrame::ReadQuery()
{
	std::ifstream fin;
	fin.open(std::string((const char*)(m_strCurFileName.mb_str(wxConvUTF8))).c_str(), std::ios::in);
	if (!fin) {
		wxEmdrosErrorMessage(wxT("Could not open file."));
	} else {
		ClearTextCtrls();
		while (!fin.eof()) {
#define BUFFERSIZE (2048)
			char szBuffer[BUFFERSIZE];      
			fin.getline(szBuffer, BUFFERSIZE);
			m_pEditWindow->AppendText(wxString(szBuffer, wxConvUTF8));
			m_pEditWindow->AppendText(wxString(wxT("\n")));
		} 
		fin.close();
	}
}

void MainFrame::SaveValueToFile(const wxString& filename, const wxString& value)
{
	std::ofstream fout;
	fout.open(std::string((const char*)(filename.mb_str(wxConvUTF8))).c_str(), std::ios::out);
	if (!fout) {
		wxEmdrosErrorMessage(wxT("Could not open file for writing."));
	} else  {
		fout << std::string((const char*) value.mb_str(wxConvUTF8));
	}

}

void MainFrame::SaveOutputAreaAsHTML(const wxString& filename)
{
	std::ofstream fout;
	std::string str_filename = std::string((const char*)(filename.mb_str(wxConvUTF8)));
	fout.open(str_filename.c_str(), std::ios::out);
	if (!fout) {
		wxEmdrosErrorMessage(wxT("Could not open file for writing."));
	} else  {
		wxBusyCursor busyCursor;
		m_pResultsWindow->toHTML(&fout, str_filename);
	}
}

void MainFrame::SaveQuery()
{
	SaveValueToFile(m_strCurFileName, GetEditWindowText());
}

bool MainFrame::Connect()
{
	ClearSchemaTree(wxString(wxT("")));
	delete m_pConf;
	std::ostringstream ssout;
	m_pConf = parse_config_file(std::string((const char*)m_connectionData.m_strConfiguration.mb_str(wxConvUTF8)),
				    std::string(app_prefix()),
				    &ssout);
	if (m_pConf == 0) {
		(*m_pOut) << ssout.str();
		wxEmdrosErrorMessage(wxString(wxT("Error: Could not load configuration file:\n")) + m_connectionData.m_strConfiguration);
		ClearSchemaTree(wxString(wxT("Configuration not loaded")));
		this->Refresh();
		return false;
	} else {
		wxString strDB;
		if (m_connectionData.m_strDatabase.IsEmpty()) {
			if (m_pConf->has_key("database")) {
				strDB = wxString(m_pConf->getValues("database")[0].c_str(), wxConvUTF8);
			} else {
				wxEmdrosErrorMessage(wxString(wxT("Error: You must specify a database name,\n")
							      wxT("either in the connection dialog or in the\n")
							      wxT("configuration file (with the 'database' key).")));
				ClearSchemaTree(wxString(wxT("No database specified")));
				this->Refresh();
				return false;
			}
		} else {
			strDB = m_connectionData.m_strDatabase;
		}

		// Connect    
		delete m_pEE;
		
		// FIXME: This uses std::cout...
		m_pEE = new EmdrosEnv(kOKConsole,
				      kCSISO_8859_1, // FIXME: This should choose UTF8 sometimes...
				      std::string((const char*) m_connectionData.m_strHost.mb_str(wxConvUTF8)),
				      std::string((const char*) m_connectionData.m_strUser.mb_str(wxConvUTF8)),
				      std::string((const char*) m_connectionData.m_strPassword.mb_str(wxConvUTF8)),
				      std::string((const char*) strDB.mb_str(wxConvUTF8)),
				      m_connectionData.m_backend_kind);

		// Make new EmdrosWrapper
		delete m_pEW;
		m_pEW = new EmdrosWrapper(m_pEE, m_pConf, m_pOut);
		std::string message;
		if (!m_pEW->configurePOut(message)) {
			wxString wxStrMsg = wxString(message.c_str(), wxConvUTF8);
			wxEmdrosErrorMessage(wxString(wxT("Error: While configuring, I encountered the following problem:\n") + wxStrMsg));
		}

		// Initialize m_pResultsWindow
		m_pResultsWindow->InitializeInterlinearViewMetrics(m_pConf, m_pOut->getNoOfInterlinearLines());
		m_pResultsWindow->Clear();

		// Set size and font name of input area
		if (m_pOut != 0) {
			ClearEditWindowAndSetDefaultStyle();
		}
	
		// Check that connection is OK
		if (!m_pEE->connectionOk()) {
			wxString error_message(wxT("Error: Could not connect to backend.\n")
					       wxT("Maybe the username, password, hostname,\n")
					       wxT("or database name were wrong..."));
#ifdef __WXMSW__
			if (m_connectionData.m_backend_kind == kMySQL) {
				error_message += wxString(wxT("\n\nOr it may be that you do not have\n")
							  wxT("libmysql.dll in your path.\n")
							  wxT("You can copy it from your MySQL\n")
							  wxT("installation to the directory where this\n")
							  wxT("program is located, or to C:\\WINDOWS\\SYSTEM32\\."));
			}
#endif
					       wxEmdrosErrorMessage(error_message);
			ClearSchemaTree(wxString(wxT("Could not establish connection...")));
			this->Refresh();
			return false;
		}
		PopulateSchemaTree();
		m_pOut->print(std::string((const char*) (wxString(wxT("Successfully connected.\n")).mb_str(wxConvUTF8))));
		this->Refresh();
		return true;
	}
}

void MainFrame::OnUpdateUIExec(wxUpdateUIEvent& event)
{
	event.Enable(m_bExecuting);
}

void MainFrame::OnUpdateUINoExec(wxUpdateUIEvent& event)
{
	event.Enable(!m_bExecuting);
}

void MainFrame::ClearSchemaTree(wxString msg)
{
	m_pSchemaTree->DeleteAllItems();
	m_pSchemaTree->AddRoot(msg);
}

void MainFrame::PopulateSchemaTree(void)
{
	ClearSchemaTree(m_connectionData.m_strDatabase);
	std::ostringstream stringstream;
	if (!m_pEW->retrieveSchema(&stringstream)) {
		wxEmdrosErrorMessage(wxT("Error: Could not retrieve schema."));
	} else {
		EmdrosSchema *pSchema = m_pEW->getPSchema();
		if (pSchema != 0) {
			AddEnumerationsToSchemaTree(pSchema);
			AddObjectTypesToSchemaTree(pSchema);
			AddMonadSetsToSchemaTree(pSchema);
		}
	}
}


void MainFrame::AddEnumerationsToSchemaTree(EmdrosSchema *pSchema)
{
	wxTreeItemId EnumRoot = m_pSchemaTree->AppendItem(m_pSchemaTree->GetRootItem(),
							  wxT("Enumerations"));

	const EmdrosEnumerations& enums = pSchema->getEnums();
	if (enums.isEmpty()) {
		m_pSchemaTree->AppendItem(EnumRoot,
					  wxT("no enumerations defined..."));        
	} else {
		StringVec enum_names = enums.getEnumNames();
		for (StringVec::const_iterator enumci = enum_names.begin();
		     enumci != enum_names.end();
		     ++enumci) {
			std::string enumeration_name = *enumci;
    
			wxString EnumString = wxString(enumeration_name.c_str(), wxConvUTF8);
			wxTreeItemId this_enumeration_root = 
				m_pSchemaTree->AppendItem(EnumRoot, EnumString);    
    
			StringVec enum_consts = enums.getEnumConstantNames(enumeration_name);
			for (StringVec::const_iterator enum_const_ci = enum_consts.begin();
			     enum_const_ci != enum_consts.end();
			     ++enum_const_ci) {
				std::string enum_const_name = *enum_const_ci;
				wxString EnumConstString = wxString(enum_const_name.c_str(), wxConvUTF8);
				m_pSchemaTree->AppendItem(this_enumeration_root, 
							  EnumConstString);    
			}
		}
	}
}

bool MainFrame::ExecuteEmdrosString(std::string query)
{
	return m_pEW->executeString(query);
}



void MainFrame::AddObjectTypesToSchemaTree(EmdrosSchema *pSchema)
{
	wxTreeItemId OTRoot = m_pSchemaTree->AppendItem(m_pSchemaTree->GetRootItem(),
							wxT("Object Types"));
	const EmdrosObjectTypes& object_types = pSchema->getObjectTypes();
	if (object_types.isEmpty()) {
		m_pSchemaTree->AppendItem(OTRoot,
					  wxT("no object types defined..."));        
	} else {
		StringVec OT_names = object_types.getObjectTypeNames();
		for (StringVec::const_iterator otci = OT_names.begin();
		     otci != OT_names.end();
		     ++otci) {
			std::string object_type_name = *otci;
    
			wxString OTString = wxString(object_type_name.c_str(), wxConvUTF8);
			wxTreeItemId this_OT_root = 
				m_pSchemaTree->AppendItem(OTRoot, OTString);    
      
			std::list<FeatureNameAndType> features 
				= object_types.getFeaturesOfObjectType(object_type_name);

			for (std::list<FeatureNameAndType>::const_iterator featntci = features.begin();
			     featntci != features.end();
			     ++featntci) {
				std::string feature_name = featntci->name;
				std::string feature_type_name = featntci->type;
				std::string feature_string = feature_name + " : " + feature_type_name;
				wxString FeatureString = wxString(feature_string.c_str(), wxConvUTF8);
				wxTreeItemId feature_root = m_pSchemaTree->AppendItem(this_OT_root, 
										      FeatureString);
				if (EmdrosObjectTypes::FeatureTypeIsEnum(*featntci)) {
					const EmdrosEnumerations& enums = pSchema->getEnums();
					StringVec enum_consts = enums.getEnumConstantNames(feature_type_name);
					for (StringVec::const_iterator enum_const_ci = enum_consts.begin();
					     enum_const_ci != enum_consts.end();
					     ++enum_const_ci) {
						std::string enum_const_name = *enum_const_ci;
						wxString EnumConstString = wxString(enum_const_name.c_str(), wxConvUTF8);
						m_pSchemaTree->AppendItem(feature_root, EnumConstString);    
					}
				}
			}
		}
	}
}


void MainFrame::AddMonadSetsToSchemaTree(EmdrosSchema *pSchema)
{
	wxTreeItemId MQLSetRoot = m_pSchemaTree->AppendItem(m_pSchemaTree->GetRootItem(),
							    wxT("Monad sets"));
  
	const EmdrosMonadSets& monad_sets = pSchema->getMonadSets();
	if (monad_sets.isEmpty()) {
		m_pSchemaTree->AppendItem(MQLSetRoot, wxString(wxT("No monad sets defined...")));
	} else {
		for (EmdrosMonadSets::const_iterator msci = monad_sets.begin();
		     msci != monad_sets.end();
		     ++msci) {
			std::string monad_set_name = msci->first;
			SetOfMonads som = msci->second;
			wxString MonadSetNameString = wxString(monad_set_name.c_str(), wxConvUTF8);
			wxTreeItemId this_monad_set_root = 
				m_pSchemaTree->AppendItem(MQLSetRoot, MonadSetNameString);    
      
			wxString MonadSetSetString = wxString(som.toString().c_str(), wxConvUTF8);
			m_pSchemaTree->AppendItem(this_monad_set_root, 
						  MonadSetSetString);    
		}
	}
}

void MainFrame::readConfig()
{
	// Create it if it does not exist;
	// use the name set by the app's constructor's call to
	// SetAppName for the root of the tree.
	wxConfigBase *config = wxConfigBase::Get();
	
	// Flush all current changes to disk/registry
	config->Flush();
	
	config->SetPath(wxT("/QueryTool/"));
	
	wxString myString;


	if (config->Read(wxT("QueryFileDir"), &myString)) {
		m_strQueryFileDir = myString;
	} else {
		m_strQueryFileDir = ::wxGetHomeDir();
	}
}

void MainFrame::writeConfig()
{
	// Create it if it does not exist;
	// use the name set by LTCApp::LTCApp's call to
	// SetAppName for the root of the tree.
	wxConfigBase *config = wxConfigBase::Get();

	config->SetPath(wxT("/QueryTool/"));

	config->Write(wxT("QueryFileDir"), m_strQueryFileDir);

	// Flush all current changes to disk/registry
	config->Flush();
}




void MyTextCtrl::OnChar(wxKeyEvent& event)
{
	if (event.GetKeyCode() == WXK_TAB) {
		WriteText(wxT("\t"));
	} else {
		event.Skip();
	}
}

