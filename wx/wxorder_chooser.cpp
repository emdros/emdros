//
// wxorder_chooser.cpp
// Ulrik Sandborg-Petersen
// Created: 4/14-2007
// Last update: 4/17-2017
//
//
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2007-2017  Ulrik Sandborg-Petersen
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
#pragma implementation "wxorder_chooser.h"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/validate.h"

#include <wxorder_chooser.h>
#include <wxutil_emdros.h>
#include <string_func.h>
////@end includes


/*!
 * OrderChooserWidget event table definition
 */
BEGIN_EVENT_TABLE( OrderChooserWidget, wxPanel )
	EVT_BUTTON(ID_ORDER_CHOOSER_MOVE_UP, OrderChooserWidget::OnMoveUp)
	EVT_BUTTON(ID_ORDER_CHOOSER_MOVE_DOWN, OrderChooserWidget::OnMoveDown)
	EVT_UPDATE_UI(ID_ORDER_CHOOSER_MOVE_UP, OrderChooserWidget::OnUpdateMoveUp)
	EVT_UPDATE_UI(ID_ORDER_CHOOSER_MOVE_DOWN, OrderChooserWidget::OnUpdateMoveDown)
	EVT_LISTBOX(ID_ORDER_CHOOSER_LISTBOX, OrderChooserWidget::OnSelectionChanged)
END_EVENT_TABLE()

/*!
 * OrderChooserWidget constructor.
 */
OrderChooserWidget::OrderChooserWidget( )
	: m_pParent(0),
	  m_btnMoveUp(0),
	  m_btnMoveDown(0),
	  m_pMainSizer(0)
{
}

/*!
 * OrderChooserWidget constructor.
 */
OrderChooserWidget::OrderChooserWidget(wxWindow *pParent,
				       int myid,
				       wxString strLabel,
				       const WXStringVec& vecInitialValues)
	: wxPanel(pParent, myid),
	  m_pParent(pParent),
	  m_btnMoveUp(0),
	  m_btnMoveDown(0),
	  m_pMainSizer(0),
	  m_strLabel(strLabel),
	  m_strings(vecInitialValues),
	  m_nLastIndex(vecInitialValues.size()-1)
{
	CreateControls();
}


OrderChooserWidget::~OrderChooserWidget()
{
}


/*!
 * OrderChooserWidget creator
 */
bool OrderChooserWidget::Create(wxWindow *pParent,
				int myid,
				wxString strLabel,
				const WXStringVec& vecInitialValues)
{
	m_pParent = pParent;
	m_strLabel = strLabel;
	m_strings = vecInitialValues;
	m_nLastIndex = vecInitialValues.size()-1;

	////@begin OrderChooserWidget creation
	SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS|wxWS_EX_VALIDATE_RECURSIVELY);
	wxPanel::Create( pParent, myid );

	CreateControls();
	GetSizer()->Fit(this);
	GetSizer()->SetSizeHints(this);

	////@end OrderChooserWidget creation
	return TRUE;
}

/*!
 * Control creation for OrderChooserWidget
 */

void OrderChooserWidget::CreateControls()
{    
	////@begin OrderChooserWidget content construction
	m_btnMoveUp = NULL;
	m_btnMoveDown = NULL;
	m_pListBox = NULL;
  
	OrderChooserWidget* itemDialog1 = this;

	wxSize sizeLB(180,200);

	m_pMainSizer = new wxBoxSizer(wxHORIZONTAL);
	itemDialog1->SetSizer(m_pMainSizer);

	m_pListBox = new wxListBox(this, ID_ORDER_CHOOSER_LISTBOX,
				   wxDefaultPosition,
				   sizeLB,
				   0, NULL,
				   wxLB_SINGLE | wxLB_HSCROLL | wxLB_NEEDED_SB);
	m_pMainSizer->Add(m_pListBox, 0, wxALIGN_LEFT|wxALIGN_TOP|wxALL, 5);

	wxBoxSizer *pRightSizer = new wxBoxSizer(wxVERTICAL);
	m_pMainSizer->Add(pRightSizer, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

	m_btnMoveUp = new wxButton(this, ID_ORDER_CHOOSER_MOVE_UP,
				   wxString(wxT("Move Up")));

	m_btnMoveDown = new wxButton(this, ID_ORDER_CHOOSER_MOVE_DOWN,
				   wxString(wxT("Move Down")));

	pRightSizer->Add(m_btnMoveUp);
	pRightSizer->Add(m_btnMoveDown);
}

void OrderChooserWidget::OnSelectionChanged(wxCommandEvent& event)
{	
	(void)(event); // Silence a warning

	SelectionChanged();
}

void OrderChooserWidget::SelectionChanged()
{
}


void OrderChooserWidget::OnMoveUp(wxCommandEvent& event)
{
	(void)(event); // Silence a warning

	int nSelection = m_pListBox->GetSelection();
	if (nSelection == wxNOT_FOUND || nSelection == 0) {
		return;
	} else {
		wxString strTemp;
		strTemp = m_strings[nSelection - 1];
		m_strings[nSelection - 1] = m_strings[nSelection];
		m_strings[nSelection] = strTemp;
		
		transferStringsToLB();
		m_pListBox->SetSelection(nSelection-1);
	}
}
	
void OrderChooserWidget::OnMoveDown(wxCommandEvent& event)
{
	(void)(event); // Silence a warning

	int nSelection = m_pListBox->GetSelection();
	if (nSelection == wxNOT_FOUND || nSelection == m_nLastIndex) {
		return;
	} else {
		wxString strTemp;
		strTemp = m_strings[nSelection + 1];
		m_strings[nSelection + 1] = m_strings[nSelection];
		m_strings[nSelection] = strTemp;
		
		transferStringsToLB();
		m_pListBox->SetSelection(nSelection+1);
	}
}


void OrderChooserWidget::OnUpdateMoveUp(wxUpdateUIEvent& event)
{
	int nSelection = m_pListBox->GetSelection();
	if (nSelection == wxNOT_FOUND || nSelection == 0) {
		event.Enable(false);
	} else {
		event.Enable(true);
	}
}


void OrderChooserWidget::OnUpdateMoveDown(wxUpdateUIEvent& event)
{
	int nSelection = m_pListBox->GetSelection();
	if (nSelection == wxNOT_FOUND || nSelection == m_nLastIndex) {
		event.Enable(false);
	} else {
		event.Enable(true);
	}
}


void OrderChooserWidget::setStrings(const WXStringVec& stringvec)
{
	m_strings = stringvec;
	m_nLastIndex = stringvec.size()-1;
	transferStringsToLB();
}

void OrderChooserWidget::transferStringsToLB()
{
	wxArrayString arrstr;
	WXStringVec::const_iterator ci = m_strings.begin();
	while (ci != m_strings.end()) {
		arrstr.Add(*ci);
		++ci;
	}

	// Delete everything in pLB
	m_pListBox->Clear();

	// Insert the strings at the beginning
	m_pListBox->InsertItems(arrstr, 0);
}

WXStringVec OrderChooserWidget::getStrings(void) const
{
	return m_strings;
}

wxString OrderChooserWidget::getSelection(void) const // Returns the empty string if no selection made
{
	int nSelection = m_pListBox->GetSelection();
	if (nSelection == wxNOT_FOUND) {
		return wxString();
	} else {
		return m_pListBox->GetString(nSelection);
	}
}

bool OrderChooserWidget::hasSelection(void) const // Returns true if something is selected
{
	int nSelection = m_pListBox->GetSelection();
	if (nSelection == wxNOT_FOUND) {
		return false;
	} else {
		return true;
	} 
}
