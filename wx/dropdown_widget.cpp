//
// dropdown_widget.cpp
// Ulrik Petersen
// Created: 4/14-2007
// Last update: 4/10-2017
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


