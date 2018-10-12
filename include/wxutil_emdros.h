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
