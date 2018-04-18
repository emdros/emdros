//
// genericdlg.cpp
// Ulrik Petersen
// Created: 7/16-2005
// Last update: 3/21-2013
//
//
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2003-2013  Ulrik Sandborg-Petersen
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
