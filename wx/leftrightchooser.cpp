//
// leftrightchooser.cpp
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


#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "leftrightchooser.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif


#include <wxutil_emdros.h>
#include <wx/sizer.h>
#include <wx/arrstr.h>
#include <leftrightchooser.h>
#include <algorithm>

BEGIN_EVENT_TABLE( LeftRightChooser, wxPanel )
        EVT_BUTTON(ID_LEFT_BUTTON, LeftRightChooser::OnMoveLeft)
        EVT_BUTTON(ID_RIGHT_BUTTON, LeftRightChooser::OnMoveRight)
	EVT_UPDATE_UI(ID_LEFT_BUTTON, LeftRightChooser::OnUpdateUIMoveLeft)
	EVT_UPDATE_UI(ID_RIGHT_BUTTON, LeftRightChooser::OnUpdateUIMoveRight)
	EVT_LISTBOX(ID_RIGHT_LISTBOX, LeftRightChooser::OnRightListBoxSelectionChanged)
END_EVENT_TABLE()


LeftRightChooser::LeftRightChooser(wxWindow *pParent,
				   int myid,
				   wxString leftTitle,
				   wxString rightTitle,
				   bool bWantsOnlyOneAtRight)
: 	wxPanel(pParent, myid),
	m_pParent(pParent),
	m_leftTitle(leftTitle),
	m_rightTitle(rightTitle),
	m_pLeftLB(0),
	m_pRightLB(0),
	m_pMainSizer(0),
	m_bWantsOnlyOneAtRight(bWantsOnlyOneAtRight)
{
	CreateControls();
}


LeftRightChooser::~LeftRightChooser()
{
}


void LeftRightChooser::setLeftStrings(const WXStringVec& stringvec)
{
	m_leftStrings = stringvec;
	setLBStrings(stringvec, m_pLeftLB);
}

void LeftRightChooser::setRightStrings(const WXStringVec& stringvec)
{
	if (m_bWantsOnlyOneAtRight && stringvec.size() > 1) {
		// Silently take only the first
		m_rightStrings.clear();
		m_rightStrings.push_back(stringvec[0]);
		setLBStrings(m_rightStrings, m_pRightLB);
	} else {
		m_rightStrings = stringvec;
		setLBStrings(stringvec, m_pRightLB);
	}
}

void LeftRightChooser::setLBStrings(const WXStringVec& stringvec, wxListBox *pLB)
{
	wxArrayString arrstr;
	WXStringVec::const_iterator ci = stringvec.begin();
	while (ci != stringvec.end()) {
		arrstr.Add(*ci);
		++ci;
	}

	// Delete everything in pLB
	pLB->Clear();

	// Insert the strings at the beginning
	pLB->InsertItems(arrstr, 0);
}

WXStringVec LeftRightChooser::getLeft(void) const
{
	return m_leftStrings;
}

WXStringVec LeftRightChooser::getRight(void) const
{
	return m_rightStrings;
}


void LeftRightChooser::CreateControls()
{
	m_pMainSizer = new wxFlexGridSizer(1,3,0,25);

	// Do left
	wxFlexGridSizer *pFlexGridSizerLeft = new wxFlexGridSizer(2, 1, 0, 0);
	pFlexGridSizerLeft->SetFlexibleDirection(wxBOTH);
	wxStaticText *pLabelLeft = new wxStaticText(this,
						    ID_LEFT_LABEL,
						    m_leftTitle);
	pFlexGridSizerLeft->Add(pLabelLeft);

	wxSize listBoxSize(180,150);

	if (m_pLeftLB == 0) {
		m_pLeftLB = new wxListBox(this,
					  ID_LEFT_LISTBOX,
					  wxDefaultPosition,
					  listBoxSize);
	}
	pFlexGridSizerLeft->Add(m_pLeftLB);

	m_pMainSizer->Add(pFlexGridSizerLeft);

	// Do middle
	wxFlexGridSizer *pMiddleSizer = new wxFlexGridSizer(3, 0, 5, 0);
	pMiddleSizer->SetFlexibleDirection(wxVERTICAL);
	pMiddleSizer->Add(new wxStaticText(this,
					   -1,
					   wxString(wxT(""))));
	wxButton *pMoveRightButton = new wxButton(this,
						 ID_RIGHT_BUTTON,
						 wxString(wxT(">")));
	//wxSizerFlags middleTopSizerFlags;
	//middleTopSizerFlags.Right();
	//pMiddleSizer->Add(pMoveRightButton, middleTopSizerFlags);
	pMiddleSizer->Add(pMoveRightButton);


	wxButton *pMoveLeftButton = new wxButton(this,
						 ID_LEFT_BUTTON,
						 wxString(wxT("<")));
	//wxSizerFlags middleBottomSizerFlags;
	//middleTopSizerFlags.Left();
	//pMiddleSizer->Add(pMoveLeftButton, middleBottomSizerFlags);
	pMiddleSizer->Add(pMoveLeftButton);

	wxSizerFlags middleSizerFlags;
	middleSizerFlags.Center();
	m_pMainSizer->Add(pMiddleSizer, middleSizerFlags);

	// Do right
	wxFlexGridSizer *pFlexGridSizerRight = new wxFlexGridSizer(2, 1, 0, 0);
	pFlexGridSizerRight->SetFlexibleDirection(wxBOTH);
	wxStaticText *pLabelRight = new wxStaticText(this,
						     ID_RIGHT_LABEL,
						     m_rightTitle);
	pFlexGridSizerRight->Add(pLabelRight);
	
	if (m_pRightLB == 0) {
		m_pRightLB = new wxListBox(this,
					   ID_RIGHT_LISTBOX,
					   wxDefaultPosition,
					   listBoxSize);
	}
	pFlexGridSizerRight->Add(m_pRightLB);

	this->SetSizer(m_pMainSizer);
	m_pMainSizer->Add(pFlexGridSizerRight);
	m_pMainSizer->Fit(this);
}

void LeftRightChooser::moveFromLeftToRight()
{
	moveItems(m_leftStrings, m_pLeftLB,
		  m_rightStrings, m_pRightLB);
}

void LeftRightChooser::moveFromRightToLeft()
{
	moveItems(m_rightStrings, m_pRightLB,
		  m_leftStrings, m_pLeftLB);
}

void LeftRightChooser::moveItems(WXStringVec& fromVec, wxListBox *pFromLB,
				 WXStringVec& toVec, wxListBox *pToLB)
{
	wxArrayInt selections;
	int number_of_selections = pFromLB->GetSelections(selections);

	for (int i = 0;
	     i < number_of_selections;
	     ++i) {
		int index = selections.Item(i);
		wxString mystring(fromVec[index]);
		toVec.push_back(mystring);
		fromVec.erase(fromVec.begin() + index - i, fromVec.begin() + index - i + 1);
	}

	std::sort(fromVec.begin(), fromVec.end());
	std::sort(toVec.begin(), toVec.end());

	setLBStrings(fromVec, pFromLB);
	setLBStrings(toVec, pToLB);
}

void LeftRightChooser::OnMoveRight(wxCommandEvent& event)
{
	(void)(event); // Silence a warning

	moveFromLeftToRight();
}

void LeftRightChooser::OnMoveLeft(wxCommandEvent& event)
{
	(void)(event); // Silence a warning

	moveFromRightToLeft();
}

void LeftRightChooser::OnRightListBoxSelectionChanged(wxCommandEvent& event)
{
	(void)(event); // Silence a warning

	wxArrayInt selections;
	int number_of_selections = m_pRightLB->GetSelections(selections);
	WXStringVec toVec;
	for (int i = 0;
	     i < number_of_selections;
	     ++i) {
		int index = selections.Item(i);
		wxString mystring(m_rightStrings[index]);
		toVec.push_back(mystring);
	}

	std::sort(toVec.begin(), toVec.end());

	rightListBoxSelectionChanged(toVec);
}

void LeftRightChooser::OnUpdateUIMoveLeft(wxUpdateUIEvent& event)
{
	if (m_rightStrings.size() == 0) {
		event.Enable(false);
	} else {
		event.Enable(true);
	}
}

void LeftRightChooser::OnUpdateUIMoveRight(wxUpdateUIEvent& event)
{
	if (m_leftStrings.size() == 0) {
		// We can't move from left if left is empty
		event.Enable(false);
	} else {
		// left is not empty
		if (m_bWantsOnlyOneAtRight) {
			// We want at most one at the right side.
			if (m_rightStrings.size() > 0) {
				// If there's one (or more), we don't want
				// to move right
				event.Enable(false);
			} else {
				// If there is 0, we can move right.
				event.Enable(true);
			}
		} else {
			// left is not empty,
			// and we don't care how many are at the right.
			event.Enable(true);
		}
	}
}

