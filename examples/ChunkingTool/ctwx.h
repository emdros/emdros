/*
 * ctwx.h
 *
 * Main program in ctwx.
 * ctwx is a graphical front-end for chunking Emdros databases.
 *
 * Ulrik Petersen
 * Created: 4/13-2005
 * Last update: 12/16-2005
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
#pragma interface "ctwx.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/image.h"
#include "ctmf.h"
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
 * EmdrosChunkingToolApp class declaration
 */

class EmdrosChunkingToolApp: public wxApp
{    
    DECLARE_CLASS( EmdrosChunkingToolApp )
    DECLARE_EVENT_TABLE()
    
    MainFrame *m_pMainFrame;

public:
    /// Constructor
    EmdrosChunkingToolApp();

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
DECLARE_APP(EmdrosChunkingToolApp)
////@end declare app

#endif
// _CTWX_H_
