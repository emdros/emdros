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

/*
 * Copyright (C) 2001-2018     Ulrik Sandborg-Petersen
 * Copyright (C) 2018-present  Sandborg-Petersen Holding ApS, Denmark
 *
 * Licensed under the MIT License.
 *
 * Please see the file COPYING in the root of the sources for more details.
 *
 */


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
