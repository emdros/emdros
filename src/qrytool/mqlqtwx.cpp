/*
 * mqlqtwx.cpp
 *
 * Main program in mqlqtwx.
 * mqlqtwx is a graphical front-end for querying Emdros databases.
 *
 * Ulrik Petersen
 * Created: 4/13-2005
 * Last update: 6/8-2018
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2005-2018  Ulrik Sandborg-Petersen
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

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "mqlqtwx.h"
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
#include <wx/config.h>
#include <wx/utils.h>
#include <wx/strconv.h>
#include <wx/dir.h>
#include <wx/fs_zip.h>
#include <wx/filename.h>
#include <wx/splash.h>

#include <fstream>

#include "conndlg.h"
#include "exec.h"



////@end includes

#include "mqlqtwx.h"

////@begin XPM images
#include "../../wx/EmdrosSplashScreen.xpm"
#include "../../wx/blue-E.xpm"
////@end XPM images

/*!
 * Application instance implementation
 */


////@begin implement app
IMPLEMENT_APP( EmdrosQueryToolApp )
////@end implement app

/*!
 * EmdrosQueryToolApp type definition
 */

IMPLEMENT_CLASS( EmdrosQueryToolApp, wxApp )

/*!
 * EmdrosQueryToolApp event table definition
 */

BEGIN_EVENT_TABLE( EmdrosQueryToolApp, wxApp )

////@begin EmdrosQueryToolApp event table entries
////@end EmdrosQueryToolApp event table entries

END_EVENT_TABLE()

/*!
 * Constructor for EmdrosQueryToolApp
 */

EmdrosQueryToolApp::EmdrosQueryToolApp()
{
	////@begin EmdrosQueryToolApp member initialisation
	////@end EmdrosQueryToolApp member initialisation
}

/*!
 * Initialisation for EmdrosQueryToolApp
 */

bool EmdrosQueryToolApp::OnInit()
{    
	SetAppName(wxT("EmdrosQueryTool"));


	wxConfigBase::Get(true);


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

	ConnectionData conndata;
	bool bCreatePNGs = false;
	wxString strMQLFile;
	for (int i = 1;
	     i < argc;
	     ++i) {
		wxString strArg = argv[i];
		if (strArg == wxT("-c")) {
			if (i < (argc - 1)) {
				++i;
				wxString strConfigFile = argv[i];
				conndata.m_strConfiguration = strConfigFile;
			}
		} else if (strArg == wxT("-b")) {
			if (i < (argc - 1)) {
				++i;
				wxString strBackend = argv[i];
				std::string str_backend = std::string((const char*)strBackend.mb_str(wxConvUTF8));
				eBackendKind backend_kind;
				bool bSuccess = string2backend_kind(str_backend, backend_kind);
				if (bSuccess) {
					conndata.m_backend_kind = backend_kind;
				}
			}
		} else if (strArg == wxT("--png")) {
			bCreatePNGs = true;
		} else if (strArg == wxT("-d")) {
			if (i < (argc - 1)) {
				++i;
				wxString strDatabase = argv[i];
				conndata.m_strDatabase = strDatabase;
			}
		} else if (strArg == wxT("-h")) {
			if (i < (argc - 1)) {
				++i;
				wxString strHost = argv[i];
				conndata.m_strHost = strHost;
			}
		} else if (strArg == wxT("-u")) {
			if (i < (argc - 1)) {
				++i;
				wxString strUser = argv[i];
				conndata.m_strUser = strUser;
			}
		} else if (strArg == wxT("-p")) {
			if (i < (argc - 1)) {
				++i;
				wxString strPassword = argv[i];
				conndata.m_strPassword = strPassword;
			}
		} else {
			strMQLFile = argv[i];
		}
	}

	m_pMainFrame = NULL;
	//int nReturnCode = 0;
	if (bCreatePNGs) {
		if (!GetConnection(conndata)) {
			return false;
		} else {
			m_pMainFrame = new MainFrame(NULL);
			
			m_pMainFrame->Show(false);
			
			m_pMainFrame->m_connectionData = conndata;

			if (!m_pMainFrame->Connect()) {
				return false;
			} else {
				std::string str_file_name = std::string((const char*) strMQLFile.mb_str(wxConvUTF8));
				if (str_file_name.empty()) {
					//nReturnCode = 
					exec_stream(&std::cin, m_pMainFrame->m_pEW);
				} else {
					std::ifstream fin(str_file_name.c_str());
					if (fin) {
						// nReturnCode = 
						exec_stream(&fin, m_pMainFrame->m_pEW);
					}
				}
				return false;
			}
		}
		return false;
	} else {
		//if (!GetConnection(conndata)) {
		//  return false;
		//} else {
		m_pMainFrame = new MainFrame(NULL);
		
		m_pMainFrame->SetIcon(wxIcon( blue_E_xpm ));
		
		
		m_pMainFrame->Show(true);
		
		SetTopWindow(m_pMainFrame);
		m_pMainFrame->ResizeSplitters();
		
		// Splash screen
		wxBitmap mybitmap(emdros_logo_scaled_down_medium_xpm);
		new wxSplashScreen(mybitmap,
				   wxSPLASH_TIMEOUT|wxSPLASH_CENTRE_ON_PARENT,
				   1500, 
				   m_pMainFrame,
				   -1, 
				   wxDefaultPosition,
				   wxDefaultSize,
				   wxSIMPLE_BORDER|wxSTAY_ON_TOP);
		Yield(); // On this app
		
		return true;
	}
}

/*!
 * Cleanup for EmdrosQueryToolApp
 */
int EmdrosQueryToolApp::OnExit()
{    
	//m_pMainFrame->Destroy();
	////@begin EmdrosQueryToolApp cleanup
	return wxApp::OnExit();
	////@end EmdrosQueryToolApp cleanup
}


bool EmdrosQueryToolApp::GetConnection(ConnectionData& conndata)
{
	if (conndata.m_strHost.empty())
		conndata.m_strHost = wxT("localhost");
	if (conndata.m_strUser.empty())
		conndata.m_strUser = wxT("emdf");
	if (conndata.m_backend_kind != kSQLite3
	    && conndata.m_backend_kind != kPostgreSQL
	    && conndata.m_backend_kind != kBPT
	    && conndata.m_backend_kind != kBPT2
	    && conndata.m_backend_kind != kMySQL) {
		conndata.m_backend_kind = DEFAULT_BACKEND_ENUM;
	}
      
  
	ConnectionDialog  dlg(app_checkConfiguration, true, m_pMainFrame);
	/*
	dlg.setBackend(conndata.m_backend_kind);
	dlg.setHost(conndata.m_strHost);
	dlg.setUser(conndata.m_strUser);
	dlg.setPassword(conndata.m_strPassword);
	dlg.setDatabase(conndata.m_strDatabase);
	dlg.setConfiguration(conndata.m_strConfiguration);
	*/
	if (dlg.ShowModal() == wxID_OK) {
		conndata.m_backend_kind = dlg.getBackend();
		conndata.m_strHost = dlg.getHost();
		conndata.m_strPassword = dlg.getPassword();
		conndata.m_strUser = dlg.getUser();
		conndata.m_strDatabase = dlg.getDatabase();
		conndata.m_strConfiguration = dlg.getConfiguration();
		if (conndata.m_strDatabase.IsEmpty() || conndata.m_strConfiguration.IsEmpty()) {
			return false;
		} else {
			return true;
		}
	} else {
		return false;
	}
}


wxString EmdrosQueryToolApp::wxHelpPrefix(void)
{
#ifdef __WXMSW__
	wxChar buf[512];
	*buf = wxT('\0');
	GetModuleFileName(NULL, buf, 511);
	wxString result = wxPathOnly(wxString(buf, wxConvUTF8)) + wxT("\\");
#else
	wxString result = wxString(app_prefix().c_str(), wxConvUTF8);

	if (!wxDir::Exists(result)) {
		result = ::wxGetHomeDir();
	}
#endif
	return result;
}


void EmdrosQueryToolApp::FindAppPath()
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



std::string app_prefix()
{
#ifdef __WXMSW__
	wxString app_path_plus_etc = GetAppPath() + wxT("\\..\\etc\\qrytool\\");
	return std::string((const char*)app_path_plus_etc.mb_str(wxConvUTF8));
#elif defined(__WXMAC__)
	wxString app_path_plus_etc = GetAppPath() + wxT("/../share/emdros/qrytool/");
	return std::string((const char*)app_path_plus_etc.mb_str(wxConvUTF8));
#else
	wxString result = GetAppPath() + wxT("/../share/emdros/qrytool/");
	if (!wxDir::Exists(result)) {
		result = ::wxGetCwd();
	}
	return std::string((const char*)result.mb_str(wxConvUTF8));
#endif
}



