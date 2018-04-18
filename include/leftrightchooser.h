//
// leftrightchooser.h
// Ulrik Petersen
// Created: 8/27-2006
// Last update: 4/10-2017
//
//
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2006-2017  Ulrik Sandborg-Petersen
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




#ifndef LEFTRIGHTCHOOSER_H_
#define LEFTRIGHTCHOOSER_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "leftrightchooser.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include <wx/listbox.h>
#include <wx/radiobox.h>
#include <wx/button.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <vector>
////@end includes

typedef std::vector<wxString> WXStringVec;

#define ID_LEFT_LISTBOX    (10200)
#define ID_RIGHT_LISTBOX   (10201)
#define ID_LEFT_LABEL      (10202)
#define ID_RIGHT_LABEL     (10203)
#define ID_LEFT_BUTTON     (10204)
#define ID_RIGHT_BUTTON    (10205)

class LeftRightChooser : public wxPanel {
	wxWindow *m_pParent;
	wxString m_leftTitle;
	wxString m_rightTitle;
	WXStringVec m_leftStrings;
	WXStringVec m_rightStrings;
	wxListBox *m_pLeftLB;
	wxListBox *m_pRightLB;
	wxFlexGridSizer *m_pMainSizer;
	bool m_bWantsOnlyOneAtRight;
public:
	LeftRightChooser(wxWindow *pParent,
			 int myid,
			 wxString leftTitle,
			 wxString rightTitle,
			 bool bWantsOnlyOneAtRight = false);
	~LeftRightChooser();

	void setLeftStrings(const WXStringVec& stringvec);
	void setRightStrings(const WXStringVec& stringvec);

	WXStringVec getLeft(void) const;
	WXStringVec getRight(void) const;

	void OnMoveLeft(wxCommandEvent& event);
	void OnMoveRight(wxCommandEvent& event);
	void OnUpdateUIMoveLeft(wxUpdateUIEvent& event);
	void OnUpdateUIMoveRight(wxUpdateUIEvent& event);
	void OnRightListBoxSelectionChanged(wxCommandEvent& event);
private:
	DECLARE_EVENT_TABLE()
protected:
	virtual void rightListBoxSelectionChanged(const WXStringVec& newValues) {
		(void)(newValues); // Silence a warning
	};
	void setLBStrings(const WXStringVec& stringvec, wxListBox *pLB);
	void moveFromLeftToRight();
	void moveFromRightToLeft();
	void moveItems(WXStringVec& fromVec, wxListBox *pFromLB,
		       WXStringVec& toVec, wxListBox *pToLB);
	/// Creates the controls and sizers
	void CreateControls();
};

#endif
// LEFTRIGHTCHOOSER_H_

