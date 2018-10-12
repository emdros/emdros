//
// genericdlg.cpp
// Ulrik Petersen
// Created: 7/16-2005
// Last update: 3/21-2013
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
#pragma implementation "genericdlg.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif


#include <wx/sizer.h>
#include <wx/valgen.h>
#include <genericdlg.h>




RadioBoxInfo::RadioBoxInfo(wxString strLabel,
			   std::vector<wxString>& choices,
			   wxString strToolTip)
	: m_choices(choices),
	  m_strLabel(strLabel),
	  m_strToolTip(strToolTip),
	  m_pVector(0),
	  m_nResult(0)
{
	CopyChoices();
}

RadioBoxInfo::RadioBoxInfo(const RadioBoxInfo& other)
	: m_choices(other.m_choices),
	  m_strLabel(other.m_strLabel),
	  m_strToolTip(other.m_strToolTip),
	  m_pVector(0),
	  m_nResult(other.m_nResult)
{
	CopyChoices();
}

RadioBoxInfo::~RadioBoxInfo()
{
	delete[] m_pVector;
}

const wxString* RadioBoxInfo::getChoicesAsArray()
{
	return m_pVector;
}


void RadioBoxInfo::CopyChoices()
{
	m_pVector = new wxString[m_choices.size()];
	for (unsigned int i = 0; i < m_choices.size(); ++i) {
		m_pVector[i] = m_choices[i];
	}
}





BEGIN_EVENT_TABLE( MyGenericDialogBox, wxDialog )

	END_EVENT_TABLE()

	MyGenericDialogBox::MyGenericDialogBox(wxWindow *pParent,
					       wxString strTitle,
					       int id,
					       rbivec_t *pChoice_vector) // You do not own it!
		: wxDialog(pParent, id, strTitle),
		  m_pRbivec(pChoice_vector)
{
	CreateControls();
}


MyGenericDialogBox::~MyGenericDialogBox()
{
	// DO NOT delete the m_pRbivec;
	// you do not own it!

	// The reason is that it is most likely a local variable
	// higher up in the call chain.
}


void MyGenericDialogBox::CreateControls()
{
	// Create radioboxes
	wxBoxSizer *pRadioBoxBoxSizer = new wxBoxSizer(wxHORIZONTAL);
	for (unsigned int i = 0; i < m_pRbivec->size(); ++i) {
		pRadioBoxBoxSizer->Add(GetRadioBox((*m_pRbivec)[i]),
				       1,     // Stretch factor
				       wxEXPAND | // Make horizontally stretchable
				       wxALL, // Make a border all around
				       10     // Border width
				       );
	}

	wxBoxSizer *pButtonBoxSizer = new wxBoxSizer(wxHORIZONTAL);
	pButtonBoxSizer->Add(new wxButton(this, wxID_OK, wxT("OK")),
			     0, // Make horizontally unstretchable,
			     wxALL, // Make a border all around
			     5      // Border width
			     );
	pButtonBoxSizer->Add(new wxButton(this, wxID_CANCEL, wxT("Cancel")),
			     0, // Make horizontally unstretchable,
			     wxALL, // Make a border all around
			     5      // Border width
			     );
  
  
	wxBoxSizer *pTopLevelSizer = new wxBoxSizer(wxVERTICAL);
	pTopLevelSizer->Add(pRadioBoxBoxSizer,
			    1, // Make vertically stretchable
			    wxEXPAND | // Make horizontally stretchable
			    wxALL, // Make a border all around
			    5     // Border width
			    );
	pTopLevelSizer->Add(pButtonBoxSizer,
			    0,  // Make vertically unstretchable
			    wxALIGN_RIGHT // no border, and align right
			    );
  
	SetSizer(pTopLevelSizer);  // Use the top-level sizer for the dialog
	pTopLevelSizer->SetSizeHints(this);
}



wxRadioBox *MyGenericDialogBox::GetRadioBox(RadioBoxInfo& rbi)
{
	wxGenericValidator myvalidator(rbi.getResultPtr());
	wxRadioBox *pRadioBox = 
		new wxRadioBox(this,
			       -1,
			       rbi.getLabel(),
			       wxDefaultPosition,
			       wxDefaultSize,
			       rbi.getNoOfChoices(),
			       rbi.getChoicesAsArray(),
			       2, // Maximum number of columns
			       wxRA_SPECIFY_COLS, // Maybe other window styles?
			       myvalidator);
	pRadioBox->SetToolTip(rbi.getToolTip());
	return pRadioBox;
}
