//
// wxorder_chooser.cpp
// Ulrik Sandborg-Petersen
// Created: 4/14-2007
// Last update: 4/17-2017
//
//

/*
 * Copyright (C) 2001-2018     Ulrik Sandborg-Petersen
 * Copyright (C) 2018-present  Sandborg-Petersen Holding ApS, Denmark
 *
 * Licensed under the MIT License.
 *
 * Please see the file COPYING in the root of the sources for more details.
 *
 */





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
