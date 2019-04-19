/*
 * importtool.cpp
 *
 * Main program in ImportTool.
 * ImportTool is a graphical front-end for Chunking text in Emdros databases.
 *
 * Martin Petersen
 * Created: 10/11-2006
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
#pragma implementation "importtool.h"
#endif

#include <emdros-config.h>
// Do this because emdros-config might #define malloc,
// in which case some systems will fail to use the real 
// malloc, thus causing build errors.
#undef malloc

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
#include <wx/frame.h>
#include <wx/sizer.h>
#include <wx/arrstr.h>
#include <wx/utils.h> // For wxBusyCursor
#include <wx/stattext.h>
#include <wx/statline.h> // for wxStaticLine
#include <wx/strconv.h>
#include <wx/dir.h>
#include <wx/fs_zip.h>
#include <wx/filename.h>
#include <wx/splash.h>

#include <emdros-lconfig.h>
#include <conndlg.h>
#include <prefix_emdros.h>
#include <conf.h>
#include <sstream>

////@end includes

#include "importtool.h"

////@begin XPM images
//#include <EmdrosSplashScreen.xpm>
#include "../../wx/blue-E.xpm"
////@end XPM images

/*!
 * Application instance implementation
 */


////@begin implement app
IMPLEMENT_APP( EmdrosImportToolApp )
////@end implement app

/*!
 * EmdrosImportToolApp type definition
 */

IMPLEMENT_CLASS( EmdrosImportToolApp, wxApp )

/*!
 * EmdrosImportToolApp event table definition
 */

BEGIN_EVENT_TABLE( EmdrosImportToolApp, wxApp )

////@begin EmdrosImportToolApp event table entries
////@end EmdrosImportToolApp event table entries

END_EVENT_TABLE()

/*!
 * Constructor for EmdrosImportToolApp
 */

EmdrosImportToolApp::EmdrosImportToolApp()
{
	////@begin EmdrosImportToolApp member initialisation
	////@end EmdrosImportToolApp member initialisation
}

/*!
 * Initialisation for EmdrosImportToolApp
 */

bool EmdrosImportToolApp::OnInit()
{    
	SetAppName(wxT("EmdrosImportTool"));

#if wxUSE_XPM
	wxImage::AddHandler( new wxXPMHandler );
#endif

	wxImage::AddHandler( new wxPNGHandler );

#if wxUSE_LIBJPEG
	wxImage::AddHandler( new wxJPEGHandler );
#endif
#if wxUSE_GIF
	wxImage::AddHandler( new wxGIFHandler );
#endif


	FindAppPath();

	wxFileSystem::AddHandler(new wxZipFSHandler);

        m_pMainFrame = new ImportToolMainFrame(NULL);
    
	m_pMainFrame->SetIcon(wxIcon( blue_E_xpm ));
    
	m_pMainFrame->Show(true);
	
	SetTopWindow(m_pMainFrame);
	
	Yield(); // On this app
    
	return true;
}

/*!
 * Cleanup for EmdrosImportToolApp
 */
int EmdrosImportToolApp::OnExit()
{    
	////@begin EmdrosImportToolApp cleanup
	return wxApp::OnExit();
	////@end EmdrosImportToolApp cleanup
}



wxString EmdrosImportToolApp::wxHelpPrefix(void)
{
#ifdef __WXMSW__
	wxChar buf[512];
	*buf = wxT('\0');
	GetModuleFileName(NULL, buf, 511);
	wxString result = wxPathOnly(wxString(buf, wxConvUTF8)) + wxT("\\");
#else
	wxString result = wxString(prefix().c_str(), wxConvUTF8) + wxT("share/emdros/importtool/");
	if (!wxDir::Exists(result)) {
		result = ::wxGetHomeDir();
	}
#endif
	return result;
}


void EmdrosImportToolApp::FindAppPath()
{
	// Find the absolute path where this application has been run from.
	wxString str;
	wxString argv0 = argv[0];
	wxString cwd = ::wxGetCwd();

#if defined(__WXMAC__) && !defined(__DARWIN__)
	// On Mac OS (not X), the current directory is the relevant one when
	// the application starts.
	m_strAppPath =  cwd;
	return;
#endif

	if (wxIsAbsolutePath(argv0)) {
		m_strAppPath = wxPathOnly(argv0);
		return;
	} else {
		// Is it a relative path?
		wxString currentDir(cwd);
		if (currentDir.Last() != wxFILE_SEP_PATH)
			currentDir += wxFILE_SEP_PATH;

		str = currentDir + argv0;
		if (wxFileExists(str)) {
			m_strAppPath = wxPathOnly(str);
			return;
		} 
	}

	// OK, it's neither an absolute path nor a relative path.
	// Search PATH.

	wxPathList pathList;
	pathList.AddEnvList(wxT("PATH"));
	str = pathList.FindAbsoluteValidPath(argv0);
	if (!str.IsEmpty()) {
		m_strAppPath = wxPathOnly(str); 
		return;
	}

	// Failed
	m_strAppPath = wxEmptyString;
	return;
}


wxString GetAppPath(void)
{
	return wxGetApp().m_strAppPath;
}



std::string app_prefix(void)
{
#ifdef __WXMSW__
	wxString app_path_plus_etc = GetAppPath() + wxT("..\\etc\\");
	return std::string((const char*)app_path_plus_etc.mb_str(wxConvUTF8));
#elif defined(__WXMAC__)
	wxString app_path_plus_etc = GetAppPath() + wxT("../share/emdros/importtool/");
	return std::string((const char*)app_path_plus_etc.mb_str(wxConvUTF8));
#else
	std::string myprefix = prefix() + "share/emdros/importtool/";
	wxString result = wxString(myprefix.c_str(), wxConvUTF8);
	if (!wxDir::Exists(result)) {
		result = ::wxGetCwd();
	}
	return std::string((const char*)result.mb_str(wxConvUTF8));
#endif
}



bool app_checkConfiguration(Configuration *pConf, std::string& error_msg)
{
	// Check for consistency
	StringVec keysToCheck;
	keysToCheck.push_back("data_unit");
	keysToCheck.push_back("data_feature");
	std::ostringstream ssout;
	if (!pConf->check(keysToCheck, &ssout)) {
		error_msg = ssout.str()
			+ "Error: at least one key which needed to be defined\n"
			+ "was not defined in the configuration file.";
		return false;
	} else {
		if (pConf->has_key("reference_unit")) {
			if (!pConf->has_key("reference_feature")) {
				error_msg =  "Error: if you define reference_unit, you must also define at least one reference_feature.\n";
				return false;
			}
		}
	}
	return true;
}


/*!
 * ImportToolMainFrame type definition
 */

IMPLEMENT_CLASS( ImportToolMainFrame, wxFrame )


BEGIN_EVENT_TABLE( ImportToolMainFrame, wxFrame )
////@begin ImportToolMainFrame event table entries
	EVT_MENU(ID_MENU_FILE_RUN_WIZARD, ImportToolMainFrame::OnFileRunWizard)
	EVT_BUTTON(ID_MENU_FILE_RUN_WIZARD, ImportToolMainFrame::OnFileRunWizard)
	EVT_MENU(wxID_ABOUT, ImportToolMainFrame::OnAbout)
	EVT_MENU(wxID_EXIT, ImportToolMainFrame::OnFileExit)

	EVT_MAXIMIZE(ImportToolMainFrame::OnMaximize)
	EVT_SIZE(ImportToolMainFrame::OnSize)

////@end ImportToolMainFrame event table entries

END_EVENT_TABLE()

ImportToolMainFrame::ImportToolMainFrame( )
{
}

ImportToolMainFrame::~ImportToolMainFrame()
{
}

ImportToolMainFrame::ImportToolMainFrame( wxWindow* parent, 
		      wxWindowID id, 
		      const wxString& caption, 
		      const wxPoint& pos, 
		      const wxSize& size, 
		      long style )
	: wxFrame(parent, id, caption, pos, size, style)
{
	//Create( parent, id, caption, pos, size, style );
	CreateControls();

	Centre();
}

/*!
 * ImportToolMainFrame creator
 */

bool ImportToolMainFrame::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
	////@begin ImportToolMainFrame creation
	wxFrame::Create( parent, id, caption, pos, size, style );

	CreateControls();
	Centre();

	////@end ImportToolMainFrame creation
	return TRUE;
}


/*!
 * Control creation for ImportToolMainFrame
 */

void ImportToolMainFrame::CreateControls()
{    
	////@begin ImportToolMainFrame content construction
	////@begin ImportToolMainFrame member initialisation
	m_pMainPanel = NULL;
	m_pRunWizardButton = NULL;

	ImportToolMainFrame* itemFrame1 = this;


	wxStatusBar* itemStatusBar27 = this->CreateStatusBar(1, wxST_SIZEGRIP|wxNO_BORDER, ID_STATUSBAR);
	itemFrame1->SetStatusBar(itemStatusBar27);


	wxMenuBar* menuBar = new wxMenuBar;
	wxMenu* itemMenu3 = new wxMenu;
	itemMenu3->Append(ID_MENU_FILE_RUN_WIZARD, wxT("Run Import Wizard..."), wxT(""), wxITEM_NORMAL);
	itemMenu3->AppendSeparator();
	itemMenu3->Append(wxID_EXIT, wxT("E&xit"), wxT(""), wxITEM_NORMAL);
	menuBar->Append(itemMenu3, wxT("&File"));

#ifdef __WXMAC__
	// On Mac OS X, the About menu-item is moved to the Application Menu,
	// so we don't create the Help menu at all
	itemMenu3->Append(wxID_ABOUT, wxT("A&bout Emdros Import Tool..."), wxT("Brings up the About box"), wxITEM_NORMAL);
#else
	// Help menu
	wxMenu* itemMenu18 = new wxMenu;
	itemMenu18->AppendSeparator();
	itemMenu18->Append(wxID_ABOUT, wxT("A&bout Emdros Import Tool..."), wxT("Brings up the About box"), wxITEM_NORMAL);
	menuBar->Append(itemMenu18, wxT("&Help"));
#endif

	itemFrame1->SetMenuBar(menuBar);

	wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);

	m_pMainPanel = new wxPanel(this, wxID_ANY,
				   wxDefaultPosition,
				   wxDefaultSize,
				   0);
	
	pMainSizer->Add(m_pMainPanel,
			1,
			wxALL|wxEXPAND,
			0);

	wxBoxSizer *pPanelSizer = new wxBoxSizer(wxVERTICAL);
	m_pMainPanel->SetSizer(pPanelSizer);

	wxStaticText *pStaticText = new wxStaticText(m_pMainPanel, 
						     wxID_ANY, 
						     wxT("\n")
						     wxT("Welcome to the Emdros Importer Wizard.\n\n")
						     wxT("This wizard will help you import text into Emdros.\n\n")
						     wxT("The wizard helps you build an MQL file from your text,\n")
						     wxT("which can then be imported into Emdros with the mql program.\n\n")
						     wxT("Click the button below to continue.\n"), 
						     wxDefaultPosition);
	pPanelSizer->Add(pStaticText,
			 0,
			 wxALL,
			 30);

	

	m_pRunWizardButton = new wxButton( m_pMainPanel, ID_MENU_FILE_RUN_WIZARD, _("Run Importer wizard..."), wxDefaultPosition, wxDefaultSize, 0 );
	pPanelSizer->Add(m_pRunWizardButton, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 15);



	itemFrame1->SetSizer(pMainSizer);

	itemFrame1->Layout();
	itemFrame1->Fit();

}

/*!
 * Should we show tooltips?
 */

bool ImportToolMainFrame::ShowToolTips()
{
	return TRUE;
}

/*!
 * Get bitmap resources
 */

wxBitmap ImportToolMainFrame::GetBitmapResource( const wxString& name )
{
        (void) name; // Unused
	return wxNullBitmap;
	////@end ImportToolMainFrame bitmap retrieval
}

void ImportToolMainFrame::OnMaximize(wxMaximizeEvent& event)
{
	// Pass it higher up in the hierarchy
	event.Skip();
}

void ImportToolMainFrame::OnSize(wxSizeEvent& event)
{
	// Pass it higher up in the hierarchy
	event.Skip();
}

void ImportToolMainFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
	(void)wxMessageBox(wxT("Emdros Import Tool version 1.4.0 from Emdros version")
			   wxT(EMDROS_VERSION)
			   wxT("\n\nCopyright (C) 2005-2014 Ulrik Sandborg-Petersen\n\nMade available under the MIT License.\n\nEmdros Import Tool comes with NO WARRANTY WHATSOEVER.\n\nSee  https://emdros.org/  for more information.\n\nPlease visit  https://scripturesys.com  for commercial support and licensing."), 
			   wxT("About Emdros Import Tool"),
			   wxOK | wxICON_INFORMATION);
}


void ImportToolMainFrame::OnFileRunWizard(wxCommandEvent& event)
{
        (void) event; // Unused

	ImporterWizard *pWizard = new ImporterWizard(NULL);
    
	pWizard->SetIcon(wxIcon( blue_E_xpm ));
    
	pWizard->RunIt();
	pWizard->Destroy();
}


void ImportToolMainFrame::OnFileExit(wxCommandEvent& event)
{
        (void) event; // Unused

	Close();
}

