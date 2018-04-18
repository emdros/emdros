//
// wxorder_chooser.h
// Ulrik Petersen
// Created: 4/14-2007
// Last update: 10/3-2011
//
//
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2007-2011  Ulrik Sandborg-Petersen
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




#ifndef ORDER_CHOOSER_H_
#define ORDER_CHOOSER_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "wxorder_chooser.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include <wx/panel.h>
#include "leftrightchooser.h" // For, e.g., WXStringVec
////@end includes


#define ID_ORDER_CHOOSER_WIDGET      (10310)
#define ID_ORDER_CHOOSER_MOVE_UP     (10311)
#define ID_ORDER_CHOOSER_MOVE_DOWN   (10312)
#define ID_ORDER_CHOOSER_LISTBOX     (10313)



class OrderChooserWidget : public wxPanel {
 protected:
	wxWindow *m_pParent;
	wxListBox *m_pListBox;
	wxButton *m_btnMoveUp;
	wxButton *m_btnMoveDown;
	wxBoxSizer *m_pMainSizer;
	wxString m_strLabel;
	WXStringVec m_strings;
	int m_nLastIndex;
	DECLARE_EVENT_TABLE()
public:
	OrderChooserWidget();
	OrderChooserWidget(wxWindow *pParent,
			   int myid = ID_ORDER_CHOOSER_WIDGET,
			   wxString strLabel = wxT(""), // May be empty
			   const WXStringVec& vecInitialValues = WXStringVec());
	virtual ~OrderChooserWidget();

	bool Create(wxWindow *pParent,
		    int myid = ID_ORDER_CHOOSER_WIDGET,
		    wxString strLabel = wxT(""), // May be empty
		    const WXStringVec& vecInitialValues = WXStringVec());


	wxString getSelection(void) const; // Returns the empty string if no selection made
	bool hasSelection(void) const; // Returns true if something is selected

	WXStringVec getStrings(void) const;
	
	void setStrings(const WXStringVec& vecStrings);

	void OnMoveUp(wxCommandEvent& event);
	void OnMoveDown(wxCommandEvent& event);
	void OnUpdateMoveUp(wxUpdateUIEvent& event);
	void OnUpdateMoveDown(wxUpdateUIEvent& event);
	void OnSelectionChanged(wxCommandEvent& event);
protected:
	/// Creates the controls and sizers
	virtual void SelectionChanged();
	void CreateControls();
	void transferStringsToLB();
};


#endif
// ORDER_CHOOSER_H_

