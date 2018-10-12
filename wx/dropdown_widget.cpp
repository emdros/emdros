//
// dropdown_widget.cpp
// Ulrik Petersen
// Created: 4/14-2007
// Last update: 4/10-2017
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
#pragma implementation "dropdown_widget.h"
#endif

/*!
 * Includes
 */

////@begin includes
#include <dropdown_widget.h>
#include <wx/stattext.h>
////@end includes

/*!
 * OrderChooserWidget event table definition
 */
BEGIN_EVENT_TABLE( DropdownWidget, wxPanel )
	EVT_COMBOBOX(ID_DROPDOWN_WIDGET_COMBOBOX, DropdownWidget::OnSelectionChanged)
END_EVENT_TABLE()



DropdownWidget::DropdownWidget(wxWindow *pParent,
			       int myid,
			       bool bVertical,
			       wxString strLabel,
			       const WXStringVec& vecInitialValues)
	: wxPanel(pParent, myid),
	  m_pParent(pParent),
	  m_pBox(0),
	  m_strings(vecInitialValues),
	  m_strLabel(strLabel),
	  m_bVertical(bVertical)
{
	CreateControls();
}


DropdownWidget::~DropdownWidget()
{
}


/*!
 * Control creation for DropdownWidget
 */

void DropdownWidget::CreateControls()
{    
	////@begin DropdownWidget content construction
	m_pBox = NULL;

	int mystyle;
	if (m_bVertical) {
		mystyle = wxVERTICAL;
	} else {
		mystyle = wxHORIZONTAL;
	}

	wxBoxSizer *pMainSizer = new wxBoxSizer(mystyle);
	this->SetSizer(pMainSizer);

	wxStaticText *pLabel = new wxStaticText(this, wxID_ANY, m_strLabel);
	pMainSizer->Add(pLabel, 0, wxALIGN_LEFT|wxALIGN_TOP|wxALL, 5);

	wxSize mySize = wxDefaultSize;
	mySize.x = 100;

	m_pBox = new wxComboBox(this, ID_DROPDOWN_WIDGET_COMBOBOX,
				m_strings[0],
				wxDefaultPosition,
				mySize,
				0, NULL,
				wxCB_READONLY);
	pMainSizer->Add(m_pBox, 0, wxALIGN_LEFT|wxALIGN_TOP|wxALL, 5);

	transferStringsToCB();
}


void DropdownWidget::setStrings(const WXStringVec& stringvec)
{
	m_strings = stringvec;
	transferStringsToCB();
}

void DropdownWidget::transferStringsToCB()
{
	// Delete everything in pLB
	m_pBox->Clear();

	// Append the strings one by one
	WXStringVec::const_iterator ci = m_strings.begin();
	while (ci != m_strings.end()) {
		m_pBox->Append(*ci);
		++ci;
	}

	if (m_strings.begin() != m_strings.end()) {
		setSelection(*m_strings.begin());
	}
}

wxString DropdownWidget::getSelection(void) const
{
	return m_pBox->GetValue();
}


void DropdownWidget::setSelection(wxString selection)
{
	m_pBox->SetValue(selection);
}


void DropdownWidget::OnSelectionChanged(wxCommandEvent& event)
{
	(void)(event); // Silence a warning

	SelectionChanged();
}

void DropdownWidget::SelectionChanged()
{
}


