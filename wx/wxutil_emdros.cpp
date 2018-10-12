/*
 * wxutil.cpp
 *
 * Some useful utility functions for wxWidgets
 *
 * Ulrik Petersen
 * Created: 11/3-2005
 * Last update: 11/3-2005
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


#ifndef WXUTIL__H__
#define WXUTIL__H__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "wxutil_emdros.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/msgdlg.h>
#include <wx/string.h>
#include <wxutil_emdros.h>

void wxEmdrosErrorMessage(wxString msg)
{
  wxMessageDialog wxMSGDlg(NULL,
			   msg,
			   wxT("Error!"),
			   wxOK | wxICON_ERROR);
  wxMSGDlg.ShowModal();
}

void wxEmdrosInfoMessage(wxString msg)
{
  wxMessageDialog wxMSGDlg(NULL,
			   msg,
			   wxT("Information"),
			   wxOK | wxICON_EXCLAMATION);
  wxMSGDlg.ShowModal();
}





#endif // WXUTIL__H__
