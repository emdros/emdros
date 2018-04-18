/*
 * importtool.h
 *
 * Main program in ImportTool.
 * 
 *
 * Martin Petersen
 * Created: 10/11-2006
 * Last update: 5/13-2014
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2006-2014  Ulrik Sandborg-Petersen
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
#ifndef _CTWX_H_
#define _CTWX_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "importtool.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include <wx/app.h>
#include <wx/frame.h>
#include <wx/panel.h>
#include <wx/button.h>
#include "wx/image.h"
#include <importerwizard.h>

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
#define ID_MENU_FILE_RUN_WIZARD (10001)
#define ID_STATUSBAR            (10002)
////@end control identifiers

class ImportToolMainFrame: public wxFrame
{    
	DECLARE_CLASS( ImportToolMainFrame )
	DECLARE_EVENT_TABLE()

public:
	/// Constructors
	ImportToolMainFrame( );
	ImportToolMainFrame( wxWindow* parent, 
		   wxWindowID id = wxID_ANY, 
		   const wxString& caption = wxT("Emdros Import Tool"), 
		   const wxPoint& pos = wxDefaultPosition, 
		   const wxSize& size = wxDefaultSize, 
		   long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxMINIMIZE_BOX|wxMAXIMIZE_BOX);
	~ImportToolMainFrame();

	bool Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style);

	/// Creates the controls and sizers
	void CreateControls();

	////@begin MainFrame event handler declarations
	void OnAbout(wxCommandEvent& event);
	void OnMaximize(wxMaximizeEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnFileRunWizard(wxCommandEvent& event);
	void OnFileExit(wxCommandEvent& event);
	////@end MainFrame event handler declarations

	////@begin MainFrame member function declarations

	/// Retrieves bitmap resources
	wxBitmap GetBitmapResource( const wxString& name );

	/// Should we show tooltips?
	static bool ShowToolTips();

	////@begin MainFrame member variables
	wxPanel* m_pMainPanel;
	wxButton *m_pRunWizardButton;
	////@end MainFrame member variables
};


/*!
 * EmdrosImportToolApp class declaration
 */

class EmdrosImportToolApp: public wxApp
{    
    DECLARE_CLASS( EmdrosImportToolApp )
    DECLARE_EVENT_TABLE()
    
	    ImportToolMainFrame *m_pMainFrame;

public:
    /// Constructor
    EmdrosImportToolApp();

    /// Initialises the application
    virtual bool OnInit();

    /// Called on exit
    virtual int OnExit();

    
    void FindAppPath();


  //  bool GetConnection(ConnectionData& conndata);
    static wxString wxHelpPrefix(void);
    wxString m_strAppPath;
};

/*!
 * Application instance declaration 
 */

////@begin declare app
DECLARE_APP(EmdrosImportToolApp)
////@end declare app

#endif
// _CTWX_H_
