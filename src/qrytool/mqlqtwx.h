/*
 * mqlqtwx.h
 *
 * Main program in mqlqtwx.
 * mqlqtwx is a graphical front-end for querying Emdros databases.
 *
 * Ulrik Petersen
 * Created: 4/13-2005
 * Last update: 11/11-2005
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


#ifndef _MQLQTWX_H_
#define _MQLQTWX_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "mqlqtwx.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/image.h"
#include "mainframe.h"
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
////@end control identifiers

/*!
 * EmdrosQueryToolApp class declaration
 */

class EmdrosQueryToolApp: public wxApp
{    
    DECLARE_CLASS( EmdrosQueryToolApp )
    DECLARE_EVENT_TABLE()
    
    MainFrame *m_pMainFrame;

public:
    /// Constructor
    EmdrosQueryToolApp();

    /// Initialises the application
    virtual bool OnInit();

    /// Called on exit
    virtual int OnExit();

    MainFrame *GetMainFrame(void) { return m_pMainFrame; };
    void FindAppPath();


    bool GetConnection(ConnectionData& conndata);
    static wxString wxHelpPrefix(void);
    wxString m_strAppPath;
};

/*!
 * Application instance declaration 
 */

////@begin declare app
DECLARE_APP(EmdrosQueryToolApp)
////@end declare app

#endif
    // _MQLQTWX_H_
