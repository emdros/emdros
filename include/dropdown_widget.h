//
// dropdown_widget.h
// Ulrik Petersen
// Created: 4/14-2007
// Last update: 4/15-2007
//
//
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2007  Ulrik Petersen
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




#ifndef DROPDOWN_WIDGET_H_
#define DROPDOWN_WIDGET_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "dropdown_widget.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include <wx/panel.h>
#include <wx/combobox.h>
#include "leftrightchooser.h" // For, e.g., WXStringVec
////@end includes


#define ID_DROPDOWN_WIDGET_WIDGET      (10320)
#define ID_DROPDOWN_WIDGET_COMBOBOX     (10321)


class DropdownWidget : public wxPanel {
 protected:
	wxWindow *m_pParent;
	wxComboBox *m_pBox;
	WXStringVec m_strings;
	wxString m_strLabel;
	bool m_bVertical;
	DECLARE_EVENT_TABLE()
public:
	DropdownWidget(wxWindow *pParent,
		       int myid = ID_DROPDOWN_WIDGET_WIDGET,
		       bool bVertical = true, // Otherwise, horizontal
		       wxString strLabel = wxT(""), // May be empty
		       const WXStringVec& vecInitialValues = WXStringVec());
	virtual ~DropdownWidget();

	wxString getSelection(void) const;

	void setSelection(wxString selection);
	
	void setStrings(const WXStringVec& vecStrings);

	void OnSelectionChanged(wxCommandEvent& event);
protected:
	/// Creates the controls and sizers
	virtual void SelectionChanged(void);
	void CreateControls();
	void transferStringsToCB();
};


#endif
// DROPDOWN_WIDGET_H_

