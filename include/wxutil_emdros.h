/*
 * wxutil.h
 *
 * Some useful utility functions for wxWidgets
 *
 * Ulrik Petersen
 * Created: 11/3-2005
 * Last update: 11/4-2010
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2005-2010  Ulrik Sandborg-Petersen
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

#ifndef WXUTIL__H__
#define WXUTIL__H__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "wxutil_emdros.cpp"
#endif

#include <wx/string.h>
#include <wx/version.h>

extern void wxEmdrosErrorMessage(wxString msg);
extern void wxEmdrosInfoMessage(wxString msg);

/*
 * Compatibility for wxWidgets 2.6.x.
 */
#if wxCHECK_VERSION(2,8,0)
/* Nothing to do, since these are all defined correctly in 2.8.0 */
#else
#define wxFD_OPEN (wxOPEN)
#define wxFD_FILE_MUST_EXIST (wxFILE_MUST_EXIST)
#define wxFD_SAVE (wxSAVE)
#define wxFD_MULTIPLE (wxMULTIPLE)
#define wxFD_OVERWRITE_PROMPT (wxOVERWRITE_PROMPT)
#endif



#endif // WXUTIL__H__
