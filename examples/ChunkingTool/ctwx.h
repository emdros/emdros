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
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2005  Ulrik Petersen
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
