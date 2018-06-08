/*
 * qtconnwizard.cpp
 *
 * The connection-creating wizard
 * mqlqtwx is a graphical front-end for querying Emdros databases.
 *
 * Ulrik Petersen
 * Created: 4/13-2005
 * Last update: 6/8-2018
 *
 */
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2005-2018  Ulrik Sandborg-Petersen
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
#pragma implementation "qtconnwizard.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include <wx/wx.h>
#include <wx/combobox.h>
#include <wx/validate.h>
#include <wx/valgen.h>
#include <wx/fontdlg.h>

#include <emdros-lconfig.h>

#include "qtconnwizard.h"



//////////////////////////////////////////////////
//
// FirstPage
//
//////////////////////////////////////////////////

FirstPage::FirstPage(wxWizard *pParent, 
		     const wxBitmap& bitmap)
	: wxWizardPageSimple(pParent, NULL, NULL, bitmap)
{
	CreateControls();
}

FirstPage::~FirstPage()
{
}

void FirstPage::CreateControls()
{
	wxStaticText *pWarning = new wxStaticText(this,
						  wxID_ANY,
						  wxT("Sorry.  This functionality is not here yet,\nbut is in the works... Please have patience.\nWatch http://emdros.org for announcements\nabout new releases, or subscribe to the\n'emdros-announce' mailinglist via\n      http://emdros.org.\n\n\n\n"));

	wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);
	pMainSizer->Add(pWarning);
	
	/*
	m_pLeftRightChooser = new LeftRightChooser(this,
						   ID_LEFTRIGHT_CHOOSER1,
						   wxString(wxT("Left")),
						   wxString(wxT("Right")));
	WXStringVec leftVec;
	leftVec.push_back(wxString(wxT("A")));
	leftVec.push_back(wxString(wxT("B")));
	leftVec.push_back(wxString(wxT("C")));
	leftVec.push_back(wxString(wxT("D")));
	leftVec.push_back(wxString(wxT("E")));
	m_pLeftRightChooser->setLeftStrings(leftVec);
	pMainSizer->Add(m_pLeftRightChooser);

	OrderChooserWidget *pOCW = new OrderChooserWidget(this);
	pMainSizer->Add(pOCW);
	pOCW->setStrings(leftVec);
	*/

	SetSizer(pMainSizer);
	pMainSizer->Fit(this);
}



BEGIN_EVENT_TABLE( RasterUnitWizardPage , wxWizardPageSimple )
EVT_WIZARD_PAGE_CHANGED( wxID_ANY , RasterUnitWizardPage::OnPageChanged )
EVT_WIZARD_PAGE_CHANGING( wxID_ANY , RasterUnitWizardPage::OnPageChanging )
END_EVENT_TABLE()


RasterUnitWizardPage::RasterUnitWizardPage(wxWizard *pParent, bool bDoesNotWantZero, wxString strMessage, wxString strRightLabel, const wxBitmap& bitmap )
	: wxWizardPageSimple(pParent, NULL, NULL, bitmap),
	  m_bDoesNotWantZero(bDoesNotWantZero),
	  m_strMessage(strMessage),
	  m_strRightLabel(strRightLabel)
{
	CreateControls();
}

RasterUnitWizardPage::~RasterUnitWizardPage()
{
}

void RasterUnitWizardPage::CreateControls()
{
	wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);

	wxStaticText *pMessage = new wxStaticText(this,
						  wxID_ANY,
						  m_strMessage);

	pMainSizer->Add(pMessage);

	CreateControlsSecondary(pMainSizer);

	m_pLeftRightChooser = new LeftRightChooser(this,
						   ID_LEFTRIGHT_CHOOSER1,
						   wxString(wxT("Object Types")),
						   m_strRightLabel,
						   true // We only want 1 on the right...
						   ); 
	pMainSizer->Add(m_pLeftRightChooser);
	SetSizer(pMainSizer);
	pMainSizer->Fit(this);
	
}

void RasterUnitWizardPage::CreateControlsSecondary(wxBoxSizer *pMainSizer)
{
	(void)(pMainSizer); // Silence a warning	
}

void RasterUnitWizardPage::OnPageChanged(wxWizardEvent& event)
{
	if (event.GetDirection()) { // Forward
		WXStringVec leftVec;
		
		EmdrosSchema *pSchema = ((ConnectionWizard*)GetParent())->GetSchema();
		ASSERT_THROW(pSchema != 0, "Could not get schema.");

		const EmdrosObjectTypes& object_types = pSchema->getObjectTypes();
		if (object_types.isEmpty()) {
			ASSERT_THROW(false, "No object types. This should have been checked in the 'connection status page'.");
		} else {
			StringVec OT_names = object_types.getObjectTypeNames();
			for (StringVec::const_iterator otci = OT_names.begin();
			     otci != OT_names.end();
			     ++otci) {
				std::string object_type_name = *otci;
				
				wxString OTString = wxString(object_type_name.c_str(), wxConvUTF8);
				leftVec.push_back(OTString);
			}
		}

		m_pLeftRightChooser->setLeftStrings(leftVec);
		WXStringVec rightVec; // Empty vector
		m_pLeftRightChooser->setRightStrings(rightVec);
		Validate();
	} else {
		// Nothing to do...
	}

}

void RasterUnitWizardPage::OnPageChanging(wxWizardEvent& event)
{
	if (event.GetDirection()) { // Forward
		WXStringVec myvec = m_pLeftRightChooser->getRight();
		if (m_bDoesNotWantZero && myvec.size() != 1) {
			wxMessageBox(wxT("Error: You cannot go on without\nspecifying precisely 1 unit."), wxT("Error"), wxOK|wxCENTRE|wxICON_ERROR);
			event.Veto();
		}
	} else {
		// Going backward... nothing to do!
	}
}

wxString RasterUnitWizardPage::getUnit()
{
	WXStringVec myvec = m_pLeftRightChooser->getRight();
	if (myvec.size() < 1) {
		return wxString(wxT(""));
	} else {
		return myvec[0];
	}
}


//////////////////////////////////////////////////
//
// DataUnitsWizardPage
//
//////////////////////////////////////////////////




BEGIN_EVENT_TABLE( DataUnitsWizardPage , wxWizardPageSimple )
EVT_WIZARD_PAGE_CHANGED( wxID_ANY , DataUnitsWizardPage::OnPageChanged )
EVT_WIZARD_PAGE_CHANGING( wxID_ANY , DataUnitsWizardPage::OnPageChanging )
END_EVENT_TABLE()



DataUnitsWizardPage::DataUnitsWizardPage(wxWizard *pParent, const wxBitmap& bitmap )
	: wxWizardPageSimple(pParent, NULL, NULL, bitmap)
{
	CreateControls();
}

DataUnitsWizardPage::~DataUnitsWizardPage()
{
}

void DataUnitsWizardPage::CreateControls()
{
	wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);

	wxStaticText *pMessage = new wxStaticText(this,
						  wxID_ANY,
						  wxT("Choose data unit(s).\n\nThe data units are the units to be displayed for a given 'hit'.\nYou must choose at least 1 data unit before you can go on.\n"));

	m_pLeftRightChooser = new LeftRightChooser(this,
						   ID_LEFTRIGHT_CHOOSER1,
						   wxString(wxT("Object Types")),
						   wxString(wxT("Data unit(s)")),
						   false // We do not only want 1 on the right...
						   ); 
	pMainSizer->Add(pMessage);

	pMainSizer->Add(m_pLeftRightChooser);
	SetSizer(pMainSizer);
	pMainSizer->Fit(this);
	
}

void DataUnitsWizardPage::OnPageChanged(wxWizardEvent& event)
{
	if (event.GetDirection()) { // Forward
		WXStringVec leftVec;
		
		EmdrosSchema *pSchema = ((ConnectionWizard*)GetParent())->GetSchema();
		ASSERT_THROW(pSchema != 0, "Could not get schema.");

		const EmdrosObjectTypes& object_types = pSchema->getObjectTypes();
		if (object_types.isEmpty()) {
			ASSERT_THROW(false, "No object types. This should have been checked in the 'connection status page'.");
		} else {
			StringVec OT_names = object_types.getObjectTypeNames();
			for (StringVec::const_iterator otci = OT_names.begin();
			     otci != OT_names.end();
			     ++otci) {
				std::string object_type_name = *otci;
				
				wxString OTString = wxString(object_type_name.c_str(), wxConvUTF8);
				leftVec.push_back(OTString);
			}
		}

		m_pLeftRightChooser->setLeftStrings(leftVec);
		WXStringVec rightVec; // Empty vector
		m_pLeftRightChooser->setRightStrings(rightVec);
		Validate();
	} else {
		// Nothing to do...
	}

}

void DataUnitsWizardPage::OnPageChanging(wxWizardEvent& event)
{
	if (event.GetDirection()) { // Forward
		WXStringVec myvec = m_pLeftRightChooser->getRight();
		if (myvec.size() < 1) {
			wxMessageBox(wxT("Error: You cannot go on without\nspecifying at least 1 data unit."), wxT("Error"), wxOK|wxCENTRE|wxICON_ERROR);
			event.Veto();
		}
	} else {
		// Going backward... nothing to do!
	}
}

WXStringVec DataUnitsWizardPage::getDataUnits()
{
	WXStringVec myvec = m_pLeftRightChooser->getRight();
	return myvec;
}


//////////////////////////////////////////////////
//
// DataUnitOrderWizardPage
//
//////////////////////////////////////////////////


BEGIN_EVENT_TABLE( DataUnitOrderWizardPage , GetOrderOfStringVecPage )
	EVT_UPDATE_UI(ID_LEFT_BOUNDARY_DROPDOWN, DataUnitOrderWizardPage::OnUpdateDropdown)
	EVT_UPDATE_UI(ID_RIGHT_BOUNDARY_DROPDOWN, DataUnitOrderWizardPage::OnUpdateDropdown)
END_EVENT_TABLE()


DataUnitOrderWizardPage::DataUnitOrderWizardPage(wxWizard *pParent, const wxBitmap& bitmap )
	: GetOrderOfStringVecPage(pParent, 
				  wxString(
					   wxT("Please choose the order of the data units.\n")
					   wxT("Data unit order is used as a hint for how to bracket units.\n")
					   wxT("\"Higher\" units should be listed first.\n")
					   wxT("\nYou may also choose \"boundaries\" to place on either side of each unit.\n")),
				  bitmap)
	
{
	CreateControls();
}

DataUnitOrderWizardPage::~DataUnitOrderWizardPage()
{
}

void DataUnitOrderWizardPage::CreateControls()
{
	GetOrderOfStringVecPage::CreateControls();
	WXStringVec left_values;
	left_values.push_back(wxString(wxT("None")));
	left_values.push_back(wxString(wxT("{")));
	left_values.push_back(wxString(wxT("[")));
	left_values.push_back(wxString(wxT("(")));

	WXStringVec right_values;
	right_values.push_back(wxString(wxT("None")));
	right_values.push_back(wxString(wxT("}")));
	right_values.push_back(wxString(wxT("]")));
	right_values.push_back(wxString(wxT(")")));

	m_pLeftDropdown = new DropdownWidget(this,
					     ID_LEFT_BOUNDARY_DROPDOWN,
					     true, // wxVERTICAL layout
					     wxString(wxT("Left boundary:")),
					     left_values
					     );
	m_pRightDropdown = new DropdownWidget(this,
					      ID_RIGHT_BOUNDARY_DROPDOWN,
					      true, // wxVERTICAL layout
					      wxString(wxT("Right boundary:")),
					      right_values
					      );

	wxBoxSizer *pBoundarySizer = new wxBoxSizer(wxHORIZONTAL);
	pBoundarySizer->Add(m_pLeftDropdown);
	pBoundarySizer->Add(m_pRightDropdown);
	this->GetSizer()->Add(pBoundarySizer);
}


void DataUnitOrderWizardPage::OnPageChangedForward()
{
	WXStringVec dataUnits = ((ConnectionWizard*)GetParent())->getDataUnits();
	setStrings(dataUnits);

	WXStringVec::const_iterator ci = dataUnits.begin();
	while (ci != dataUnits.end()) {
		wxString dataUnit = *ci;
		m_boundaries.insert(std::make_pair(dataUnit, std::make_pair(std::string("None"), std::string("None"))));
		++ci;
	}

	m_strCurrentOTN = dataUnits[0];
	setBoundaryDropdownsFromOTN();

	Validate();
}

void DataUnitOrderWizardPage::OnSelectionChanged()
{
	wxString strSelection = getSelection();
	if (strSelection.IsEmpty()) {
		return; 
	} else {
		saveBoundaries();
		setBoundaries(strSelection);
	}
}

void DataUnitOrderWizardPage::saveBoundaries()
{
	wxString strLeftBoundary = m_pLeftDropdown->getSelection();
	wxString strRightBoundary = m_pRightDropdown->getSelection();

	if (!strLeftBoundary.IsEmpty() && !strRightBoundary.IsEmpty()) {
		std::string leftBoundary = std::string((const char*)strLeftBoundary.mb_str(wxConvUTF8));
		std::string rightBoundary = std::string((const char*)strRightBoundary.mb_str(wxConvUTF8));
		m_boundaries[m_strCurrentOTN] = std::make_pair(leftBoundary, rightBoundary);
	}
}

void DataUnitOrderWizardPage::setBoundaries(wxString newOTN)
{
	if (!newOTN.IsEmpty()) {
		m_strCurrentOTN = newOTN;
		setBoundaryDropdownsFromOTN();
	}
}

void DataUnitOrderWizardPage::setBoundaryDropdownsFromOTN()
{
	std::string leftBoundary = m_boundaries[m_strCurrentOTN].first;
	std::string rightBoundary = m_boundaries[m_strCurrentOTN].second;
	wxString strLeftBoundary = wxString(leftBoundary.c_str(), wxConvUTF8);
	wxString strRightBoundary = wxString(rightBoundary.c_str(), wxConvUTF8);
	m_pLeftDropdown->setSelection(strLeftBoundary);
	m_pRightDropdown->setSelection(strRightBoundary);
}

void DataUnitOrderWizardPage::OnUpdateDropdown(wxUpdateUIEvent& event)
{
	if (m_pOCW->hasSelection()) {
		event.Enable(true);
	} else {
		event.Enable(false);
	}
}


//////////////////////////////////////////////////
//
// DataUnitFeaturesWizardPage
//
//////////////////////////////////////////////////




BEGIN_EVENT_TABLE( DataUnitFeaturesWizardPage , wxWizardPageSimple )
EVT_WIZARD_PAGE_CHANGED( wxID_ANY , DataUnitFeaturesWizardPage::OnPageChanged )
EVT_WIZARD_PAGE_CHANGING( wxID_ANY , DataUnitFeaturesWizardPage::OnPageChanging )
END_EVENT_TABLE()



DataUnitFeaturesWizardPage::DataUnitFeaturesWizardPage(wxWizard *pParent, const wxBitmap& bitmap )
	: wxWizardPageSimple(pParent, NULL, NULL, bitmap)
{
	CreateControls();
}

DataUnitFeaturesWizardPage::~DataUnitFeaturesWizardPage()
{
}

void DataUnitFeaturesWizardPage::CreateControls()
{
	wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);

	wxStaticText *pMessage = new wxStaticText(this,
						  wxID_ANY,
						  wxT("Choose data unit features.\n\nThe data unit features will be displayed for a given 'hit'.\nYou must choose at least 1 data unit feature before you can go on.\n"));

	m_pLeftRightChooser = new LeftRightChooser(this,
						   ID_LEFTRIGHT_CHOOSER1,
						   wxString(wxT("Features")),
						   wxString(wxT("Data unit Feature(s)")),
						   false // We do not only want 1 on the right...
						   ); 
	pMainSizer->Add(pMessage);

	pMainSizer->Add(m_pLeftRightChooser);
	SetSizer(pMainSizer);
	pMainSizer->Fit(this);
}


void DataUnitFeaturesWizardPage::OnPageChanged(wxWizardEvent& event)
{
	if (event.GetDirection()) { // Forward
		WXStringVec leftVec;
		
		EmdrosSchema *pSchema = ((ConnectionWizard*)GetParent())->GetSchema();
		ASSERT_THROW(pSchema != 0, "Could not get schema.");

		const EmdrosObjectTypes& object_types = pSchema->getObjectTypes();
		if (object_types.isEmpty()) {
			ASSERT_THROW(false, "No object types. This should have been checked in the 'connection status page'.");
		} else {
			WXStringVec dataUnits = ((ConnectionWizard*)GetParent())->getDataUnits();

			for (WXStringVec::const_iterator duci = dataUnits.begin();
			     duci != dataUnits.end();
			     ++duci) {
				wxString strDataUnit = *duci;
				std::string dataUnit = std::string((const char*) strDataUnit.mb_str(wxConvUTF8));
				std::list<FeatureNameAndType> featuresOfDataUnit = object_types.getFeaturesOfObjectType(dataUnit);
				for (std::list<FeatureNameAndType>::const_iterator fci = featuresOfDataUnit.begin();
				     fci != featuresOfDataUnit.end();
				     ++fci) {
					std::string feature_name = fci->name;

					std::string du_dot_fn = dataUnit + "." + feature_name;
					wxString strDuDotFN = wxString(du_dot_fn.c_str(), wxConvUTF8);
					leftVec.push_back(strDuDotFN);
				}
			}
		}

		m_pLeftRightChooser->setLeftStrings(leftVec);
		WXStringVec rightVec; // Empty vector
		m_pLeftRightChooser->setRightStrings(rightVec);
		Validate();
	} else {
		// Nothing to do...
	}
}

void DataUnitFeaturesWizardPage::OnPageChanging(wxWizardEvent& event)
{
	if (event.GetDirection()) { // Forward
		WXStringVec myvec = m_pLeftRightChooser->getRight();
		if (myvec.size() < 1) {
			wxMessageBox(wxT("Error: You cannot go on without\nspecifying at least 1 data unit feature."), wxT("Error"), wxOK|wxCENTRE|wxICON_ERROR);
			event.Veto();
		}
	} else {
		// Going backward... nothing to do!
	}
}

WXStringVec DataUnitFeaturesWizardPage::getDataUnitFeatures()
{
	WXStringVec myvec = m_pLeftRightChooser->getRight();
	return myvec;
}


//////////////////////////////////////////////////
//
// DataUnitFeatureOrderWizardPage
//
//////////////////////////////////////////////////


BEGIN_EVENT_TABLE( DataUnitFeatureOrderWizardPage , wxWizardPageSimple )
	EVT_WIZARD_PAGE_CHANGED( wxID_ANY , DataUnitFeatureOrderWizardPage::OnPageChanged )
	EVT_UPDATE_UI(ID_USE_TECKIT_CHECKBOX, DataUnitFeatureOrderWizardPage::OnUpdateUseTECKitCheckbox)
	EVT_UPDATE_UI(ID_TECKIT_PATH_BROWSE_WIDGET, DataUnitFeatureOrderWizardPage::OnUpdateUseTECKit)
	EVT_UPDATE_UI(ID_TECKIT_INPUT_DROPDOWN_WIDGET, DataUnitFeatureOrderWizardPage::OnUpdateUseTECKit)
	EVT_UPDATE_UI(ID_TECKIT_OUTPUT_DROPDOWN_WIDGET, DataUnitFeatureOrderWizardPage::OnUpdateUseTECKit)
END_EVENT_TABLE()


DataUnitFeatureOrderWizardPage::DataUnitFeatureOrderWizardPage(wxWizard *pParent, const wxBitmap& bitmap )
	: wxWizardPageSimple(pParent, NULL, NULL, bitmap)
{
	CreateControls();
}

DataUnitFeatureOrderWizardPage::~DataUnitFeatureOrderWizardPage()
{
}

void DataUnitFeatureOrderWizardPage::CreateControls()
{
	wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);

	m_pOTNDropdown = new DUFODropdownWidget(this,
						ID_DATA_UNIT_COMBOBOX,
						true, // wxVERTICAL layout
						wxString(wxT("For each of these data unit(s) with features to display:")));
	pMainSizer->Add(m_pOTNDropdown);

	wxStaticText *pStaticText1 = new wxStaticText(this, 
						      wxID_ANY,
						      wxT("... please choose the order of the interlinear display lines:"));
	pMainSizer->Add(pStaticText1);

	m_pOCW = new DUFOOrderChooserWidget(this);
	pMainSizer->Add(m_pOCW);

	m_pUseTECKit = new wxCheckBox(this,
				      ID_USE_TECKIT_CHECKBOX,
				      wxT("Use TECKit mapping for this feature"),
				      wxDefaultPosition,
				      wxDefaultSize,
				      wxCHK_2STATE,
				      wxGenericValidator(&m_bUseTECKit));

	pMainSizer->Add(m_pUseTECKit);
	m_pBrowseTECKitPath = new BrowseWidget(this,
					       ID_TECKIT_PATH_BROWSE_WIDGET);
	pMainSizer->Add(m_pBrowseTECKitPath);

	WXStringVec input_values;
	input_values.push_back(wxString(wxT("Byte")));
	input_values.push_back(wxString(wxT("Unicode")));

	WXStringVec output_values;
	output_values.push_back(wxString(wxT("Byte")));
	output_values.push_back(wxString(wxT("Unicode")));

	m_pInputDropdown = new DropdownWidget(this,
					      ID_TECKIT_INPUT_DROPDOWN_WIDGET,
					      false, // wxHORIZONTAL layout
					      wxString(wxT("Byte")),
					      input_values
					      );
	pMainSizer->Add(m_pInputDropdown);
	m_pOutputDropdown = new DropdownWidget(this,
					      ID_TECKIT_OUTPUT_DROPDOWN_WIDGET,
					      false, // wxHORIZONTAL layout
					      wxString(wxT("Unicode")),
					      output_values
					      );
	pMainSizer->Add(m_pOutputDropdown);

	SetSizer(pMainSizer);
	pMainSizer->Fit(this);
}

void DataUnitFeatureOrderWizardPage::OnPageChanged(wxWizardEvent& event)
{
	if (event.GetDirection()) { // Forward
		OnPageChangedForward();
	} else {
		// Nothing to do...
	}

}



void DataUnitFeatureOrderWizardPage::OnPageChangedForward()
{
	WXStringVec dataUnits = ((ConnectionWizard*)GetParent())->getDataUnits();
	m_pOTNDropdown->setStrings(dataUnits);

	WXStringVec dataUnitFeatures = ((ConnectionWizard*)GetParent())->getDataUnitFeatures();

	std::map<std::string, WXStringVec > duf_map = ((ConnectionWizard*)GetParent())->getDataUnitFeatureMap();


	std::map<std::string, WXStringVec >::const_iterator ci2 = duf_map.begin();
	while (ci2 != duf_map.end()) {
		std::string dataUnit = ci2->first;
		const WXStringVec& duFeatures = ci2->second;
		wxString strDataUnit(dataUnit.c_str(), wxConvUTF8);
		m_features.insert(std::make_pair(strDataUnit, duFeatures));
		WXStringVec::const_iterator ci3 = duFeatures.begin();
		while (ci3 != duFeatures.end()) {
			wxString strDuDotFeature = *ci3;
			m_TECKitData.insert(std::make_pair(strDuDotFeature, FeatureTECKitData()));

			++ci3;
		}
		++ci2;
	}
	
	m_strCurrentOTN = dataUnits[0];
	setFeaturesFromOTN();

	Validate();
}

void DataUnitFeatureOrderWizardPage::OnOTNSelectionChanged()
{
	wxString strSelection = getOTNSelection();
	if (strSelection == wxT("")) {
		return; 
	} else {
		saveTECKit();
		saveFeatures();
		setFeatures(strSelection);
		setTECKit(wxT(""));
	}
}

void DataUnitFeatureOrderWizardPage::OnFeatureSelectionChanged(void)
{
	if (m_pOCW->hasSelection()) {
		wxString strSelection = m_pOCW->getSelection();
		saveTECKit();
		setTECKit(strSelection);
	}
}

void DataUnitFeatureOrderWizardPage::saveFeatures()
{
	WXStringVec vecFeatures = m_pOCW->getStrings();
	m_features[m_strCurrentOTN] = vecFeatures;
}

void DataUnitFeatureOrderWizardPage::setFeatures(wxString newOTN)
{
	if (!newOTN.IsEmpty()) {
		m_strCurrentOTN = newOTN;
		m_strCurrentDuDotFN = wxT("");
		setFeaturesFromOTN();
	}
}

void DataUnitFeatureOrderWizardPage::setFeaturesFromOTN()
{
	WXStringVec vecFeatures = m_features[m_strCurrentOTN];
	m_pOCW->setStrings(vecFeatures);
	Validate();
}

void DataUnitFeatureOrderWizardPage::saveTECKit()
{
	wxString strTECKitPath = m_pBrowseTECKitPath->getValue();
	wxString strInputEncoding = m_pInputDropdown->getSelection();
	wxString strOutputEncoding = m_pOutputDropdown->getSelection();
	bool bInputIsUnicode = strInputEncoding == wxT("Unicode");
	bool bOutputIsUnicode = strOutputEncoding == wxT("Unicode");
	FeatureTECKitData ftd(m_pUseTECKit->GetValue(), strTECKitPath, bInputIsUnicode, bOutputIsUnicode);
	
	m_TECKitData[m_strCurrentDuDotFN] = ftd;
}

void DataUnitFeatureOrderWizardPage::setTECKit(wxString newDuDotFeatureName)
{
	if (!newDuDotFeatureName.IsEmpty()) {
		m_strCurrentDuDotFN = newDuDotFeatureName;
		setTECKitFromDuDotFN();
	} else {
		m_strCurrentDuDotFN = wxT("");
		setTECKitFromDuDotFN();
	}
}

void DataUnitFeatureOrderWizardPage::setTECKitFromDuDotFN()
{
	FeatureTECKitData ftd;
	if (m_strCurrentDuDotFN.IsEmpty()) {
		ftd = FeatureTECKitData();
	} else {
		ftd = m_TECKitData[m_strCurrentDuDotFN];
	}
	m_bUseTECKit = ftd.m_bUseTECKit;
	m_pUseTECKit->SetValue(m_bUseTECKit);
	m_pBrowseTECKitPath->setValue(ftd.m_strTECKitPath);
	if (ftd.m_bInputIsUnicode) {
		m_pInputDropdown->setSelection(wxT("Unicode"));
	} else {
		m_pInputDropdown->setSelection(wxT("Byte"));
	}

	if (ftd.m_bOutputIsUnicode) {
		m_pOutputDropdown->setSelection(wxT("Unicode"));
	} else {
		m_pOutputDropdown->setSelection(wxT("Byte"));
	}

	TransferDataToWindow();
	Validate();
}

void DataUnitFeatureOrderWizardPage::OnUpdateUseTECKitCheckbox(wxUpdateUIEvent& event)
{
	if (m_pOCW->hasSelection()) {
		event.Enable(true);
	} else {
		event.Enable(false);
	}
}

void DataUnitFeatureOrderWizardPage::OnUpdateUseTECKit(wxUpdateUIEvent& event)
{
	if (m_pUseTECKit->GetValue() && m_pOCW->hasSelection()) {
		event.Enable(true);
	} else {
		event.Enable(false);
	}
}

wxString DataUnitFeatureOrderWizardPage::getOTNSelection(void) const
{
	return m_pOTNDropdown->getSelection();
}

wxString DataUnitFeatureOrderWizardPage::getFeatureSelection(void) const
{
	return m_pOCW->getSelection();
}



//////////////////////////////////////////////////
//
// ReferenceUnitWizardPage
//
//////////////////////////////////////////////////

ReferenceUnitWizardPage::ReferenceUnitWizardPage(wxWizard *pParent, const wxBitmap& bitmap)
	: RasterUnitWizardPage(pParent, 
			       false,
			       wxT("Please either choose a reference unit, or leave the right box\nblank, in order to have no reference unit.\n\nThe reference unit's features (to be chosen on the next page)\nare displayed with each hit."),
			       wxT("Reference unit"))
			       
{
	(void)(bitmap); // Silence a warning	
}

ReferenceUnitWizardPage::~ReferenceUnitWizardPage()
{
}


//////////////////////////////////////////////////
//
// ReferenceFeaturesWizardPage
//
//////////////////////////////////////////////////


const wxString ReferenceFeaturesWizardPage::m_strMsgHasReferenceUnit = wxT("Choose reference unit feature(s).\n\nThe reference unit features are shown next to a given 'hit'.\nYou must choose at least 1 reference unit feature before you can go on.\n");
const wxString ReferenceFeaturesWizardPage::m_strMsgHasNoReferenceUnit = wxT("You have not chosen a reference unit on the previous page.\nHence, you cannot choose any reference unit features here.\n\nIf you wish to use a reference unit, you can go back and select one.");



BEGIN_EVENT_TABLE( ReferenceFeaturesWizardPage , wxWizardPageSimple )
	EVT_WIZARD_PAGE_CHANGED( wxID_ANY , ReferenceFeaturesWizardPage::OnPageChanged )
	EVT_WIZARD_PAGE_CHANGING( wxID_ANY , ReferenceFeaturesWizardPage::OnPageChanging )
	EVT_UPDATE_UI(ID_LEFTRIGHT_CHOOSER1, ReferenceFeaturesWizardPage::OnUpdateLeftRightChooser)
END_EVENT_TABLE()



ReferenceFeaturesWizardPage::ReferenceFeaturesWizardPage(wxWizard *pParent, const wxBitmap& bitmap )
	: wxWizardPageSimple(pParent, NULL, NULL, bitmap),
	  m_bHasReferenceUnit(true),
	  m_pLeftRightChooser(0),
	  m_pMessage(0)
{
	CreateControls();
}

ReferenceFeaturesWizardPage::~ReferenceFeaturesWizardPage()
{
}

void ReferenceFeaturesWizardPage::CreateControls()
{
	wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);

	m_pMessage = new wxStaticText(this,
				      wxID_ANY,
				      m_strMsgHasReferenceUnit);
	pMainSizer->Add(m_pMessage);

	m_pLeftRightChooser = new LeftRightChooser(this,
						   ID_LEFTRIGHT_CHOOSER1,
						   wxString(wxT("Feature(s)")),
						   wxString(wxT("Reference unit feature(s)")),
						   false // We do not only want 1 on the right...
						   ); 

	pMainSizer->Add(m_pLeftRightChooser);
	SetSizer(pMainSizer);
	pMainSizer->Fit(this);
	
}

void ReferenceFeaturesWizardPage::OnPageChanged(wxWizardEvent& event)
{
	if (event.GetDirection()) { // Forward
		WXStringVec leftVec;
		wxString strReferenceUnit = ((ConnectionWizard*)GetParent())->getReferenceUnit();
		if (strReferenceUnit.IsEmpty()) {
			m_bHasReferenceUnit = false;
		} else {
			m_bHasReferenceUnit = true;
			EmdrosSchema *pSchema = ((ConnectionWizard*)GetParent())->GetSchema();
			ASSERT_THROW(pSchema != 0, "Could not get schema.");
			
			const EmdrosObjectTypes& object_types = pSchema->getObjectTypes();
			if (object_types.isEmpty()) {
				ASSERT_THROW(false, "No object types. This should have been checked in the 'connection status page'.");
			} else {
				std::string referenceUnit = std::string((const char*)strReferenceUnit.mb_str(wxConvUTF8));
				std::list<FeatureNameAndType> featuresOfReferenceunit = object_types.getFeaturesOfObjectType(referenceUnit);
				for (std::list<FeatureNameAndType>::const_iterator fci = featuresOfReferenceunit.begin();
				     fci != featuresOfReferenceunit.end();
				     ++fci) {
					std::string feature_name = fci->name;
					
					std::string du_dot_fn = referenceUnit + "." + feature_name;
					wxString strDuDotFN = wxString(du_dot_fn.c_str(), wxConvUTF8);
					leftVec.push_back(strDuDotFN);
				}
			}
		}

		if (m_bHasReferenceUnit) {
			m_pMessage->SetLabel(m_strMsgHasReferenceUnit);
		} else {
			m_pMessage->SetLabel(m_strMsgHasNoReferenceUnit);
		}

		m_pLeftRightChooser->setLeftStrings(leftVec);
		WXStringVec rightVec; // Empty vector
		m_pLeftRightChooser->setRightStrings(rightVec);
		Validate();
		Refresh();
	} else {
		// Nothing to do...
	}

}

void ReferenceFeaturesWizardPage::OnPageChanging(wxWizardEvent& event)
{
	if (event.GetDirection()) { // Forward
		WXStringVec myvec = m_pLeftRightChooser->getRight();
		if (m_bHasReferenceUnit) {
			if (myvec.size() < 1) {
				wxMessageBox(wxT("Error: You cannot go on without\nspecifying at least 1 data unit."), wxT("Error"), wxOK|wxCENTRE|wxICON_ERROR);
				event.Veto();
			}
		}
	} else {
		// Going backward... nothing to do!
	}
}

WXStringVec ReferenceFeaturesWizardPage::getReferenceFeatures()
{
	WXStringVec myvec = m_pLeftRightChooser->getRight();
	return myvec;
}


void ReferenceFeaturesWizardPage::OnUpdateLeftRightChooser(wxUpdateUIEvent& event)
{
	if (m_bHasReferenceUnit) {
		event.Enable(true);
	} else {
		event.Enable(false);
	}
}


//////////////////////////////////////////////////
//
// ReferenceFeatureOrderWizardPage
//
//////////////////////////////////////////////////

const wxString ReferenceFeatureOrderWizardPage::m_strMsgHasReferenceUnit
= wxString(
	   wxT("Please choose the order of the reference unit features.\n")
	   wxT("\nYou may also choose \"separator\" strings to place\n")
	   wxT("after each reference unit feature.\n\n\n"));
const wxString ReferenceFeatureOrderWizardPage::m_strMsgHasNoReferenceUnit
= wxString(
	   wxT("You have not chosen any reference unit.\n")
	   wxT("Hence, you cannot choose the order of any reference unit\nfeatures.")
	   wxT("\nYou can, however, go back and select a reference unit.")
	   );



BEGIN_EVENT_TABLE( ReferenceFeatureOrderWizardPage , GetOrderOfStringVecPage )
	EVT_UPDATE_UI(ID_LEFT_BOUNDARY_DROPDOWN, ReferenceFeatureOrderWizardPage::OnUpdateDropdown)
END_EVENT_TABLE()


ReferenceFeatureOrderWizardPage::ReferenceFeatureOrderWizardPage(wxWizard *pParent, const wxBitmap& bitmap )
	: GetOrderOfStringVecPage(pParent, 
				  wxString(wxT("\n\n\n\n\n")),
				  bitmap)
	
{
	CreateControls();
}

ReferenceFeatureOrderWizardPage::~ReferenceFeatureOrderWizardPage()
{
}

void ReferenceFeatureOrderWizardPage::CreateControls()
{
	GetOrderOfStringVecPage::CreateControls();
	WXStringVec separator_values;
	separator_values.push_back(wxString(wxT("None")));
	separator_values.push_back(wxString(wxT("SPACE")));
	separator_values.push_back(wxString(wxT("COMMA")));
	separator_values.push_back(wxString(wxT("COMMA_SPACE")));
	separator_values.push_back(wxString(wxT("COLON")));
	separator_values.push_back(wxString(wxT("COLON_SPACE")));
	separator_values.push_back(wxString(wxT("{")));
	separator_values.push_back(wxString(wxT("}")));
	separator_values.push_back(wxString(wxT("[")));
	separator_values.push_back(wxString(wxT("]")));
	separator_values.push_back(wxString(wxT("(")));
	separator_values.push_back(wxString(wxT(")")));

	m_pAfterDropdown = new DropdownWidget(this,
					     ID_LEFT_BOUNDARY_DROPDOWN,
					     true, // wxVERTICAL layout
					     wxString(wxT("Insert this string after the selected feature:")),
					     separator_values
					     );

	this->GetSizer()->Add(m_pAfterDropdown);
}


void ReferenceFeatureOrderWizardPage::OnPageChangedForward()
{
	if (!((ConnectionWizard*)GetParent())->hasReferenceUnit()) {
		m_bHasReferenceUnit = false;
		m_pMessage->SetLabel(m_strMsgHasNoReferenceUnit);
		m_strCurrentDuDotFN = wxT("");
	} else {
		m_bHasReferenceUnit = true;
		m_pMessage->SetLabel(m_strMsgHasReferenceUnit);
		
		WXStringVec referenceFeatures = ((ConnectionWizard*)GetParent())->getReferenceFeatures();
		setStrings(referenceFeatures);
		
		WXStringVec::const_iterator ci = referenceFeatures.begin();
		while (ci != referenceFeatures.end()) {
			wxString referenceFeature = *ci;
			m_separators.insert(std::make_pair(referenceFeature, wxString(wxT("SPACE"))));
			++ci;
		}
		
		m_strCurrentDuDotFN = referenceFeatures[0];
		setSeparatorDropdownFromDuDotFN();
	}

	Validate();
	Refresh();
}

bool ReferenceFeatureOrderWizardPage::getUpdateUIEnable()
{
	return m_bHasReferenceUnit;
}


void ReferenceFeatureOrderWizardPage::OnSelectionChanged()
{
	wxString strSelection = getSelection();
	if (strSelection.IsEmpty()) {
		return; 
	} else {
		saveSeparators();
		setSeparators(strSelection);
	}
}

void ReferenceFeatureOrderWizardPage::saveSeparators()
{
	wxString strSeparator = m_pAfterDropdown->getSelection();

	if (!strSeparator.IsEmpty() && !m_strCurrentDuDotFN.IsEmpty()) {
		m_separators[m_strCurrentDuDotFN] = strSeparator;
	}
}

void ReferenceFeatureOrderWizardPage::setSeparators(wxString newDuDotFN)
{
	if (!newDuDotFN.IsEmpty()) {
		m_strCurrentDuDotFN = newDuDotFN;
		setSeparatorDropdownFromDuDotFN();
	}
}

void ReferenceFeatureOrderWizardPage::setSeparatorDropdownFromDuDotFN()
{
	wxString strSeparator = m_separators[m_strCurrentDuDotFN];

	m_pAfterDropdown->setSelection(strSeparator);
}

void ReferenceFeatureOrderWizardPage::OnUpdateDropdown(wxUpdateUIEvent& event)
{
	if (m_bHasReferenceUnit && m_pOCW->hasSelection()) {
		event.Enable(true);
	} else {
		event.Enable(false);
	}
}


//////////////////////////////////////////////////
//
// OutputAreaOptionsWizardPage
//
//////////////////////////////////////////////////

BEGIN_EVENT_TABLE( OutputAreaOptionsWizardPage , wxWizardPageSimple )
     EVT_WIZARD_PAGE_CHANGED( wxID_ANY , OutputAreaOptionsWizardPage::OnPageChanged )
     EVT_BUTTON(ID_FONT_BUTTON4, OutputAreaOptionsWizardPage::OnBrowseFont)
END_EVENT_TABLE()


OutputAreaOptionsWizardPage::OutputAreaOptionsWizardPage(wxWizard *pParent, const wxBitmap& bitmap )
	: wxWizardPageSimple(pParent, NULL, NULL, bitmap)
	
{
	CreateControls();
}

OutputAreaOptionsWizardPage::~OutputAreaOptionsWizardPage()
{
}

void OutputAreaOptionsWizardPage::CreateControls()
{
	m_nCurrentInterlinearLine = 1;

	wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);
	
	m_pInterlinearLineDropdownWidget = new OAODropdownWidget(this,
								 ID_OUTPUT_AREA_OPTIONS_COMBOBOX,
								 true, // wxVERTICAL layout
								 wxString(wxT("For each of these data interlinear lines(s):")));
	pMainSizer->Add(m_pInterlinearLineDropdownWidget);

	wxStaticText *pStaticText1 = new wxStaticText(this, 
						      wxID_ANY,
						      wxT("... please choose the font to use:"));
	pMainSizer->Add(pStaticText1);

	wxBoxSizer *pFontSizer = new wxBoxSizer(wxHORIZONTAL);

	wxStaticText *pSTFontName = new wxStaticText(this,
						     wxID_ANY,
						     wxT("Font name:"));
	pFontSizer->Add(pSTFontName);


	wxSize mySize = wxDefaultSize;
	mySize.x = 150;
	m_pTCFontName = new wxTextCtrl(this,
				       wxID_ANY,
				       wxT(""),
				       wxDefaultPosition,
				       mySize,
				       0); // Style
	pFontSizer->Add(m_pTCFontName,
			1,
			wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,
			5);

	wxButton *pFontButton = new wxButton(this, ID_FONT_BUTTON4,
					     wxT("Browse font..."),
					     wxDefaultPosition,
					     wxDefaultSize,
					     wxBU_EXACTFIT);
	pFontSizer->Add(pFontButton);

	pMainSizer->Add(pFontSizer,
			0,
			wxALL,
			5);


	SetSizer(pMainSizer);
	pMainSizer->Fit(this);
}

wxString OutputAreaOptionsWizardPage::getFontName()
{
	return m_pTCFontName->GetValue();
}




void OutputAreaOptionsWizardPage::OnBrowseFont(wxCommandEvent& event)
{
	(void)(event); // Silence a warning	

	wxFont fontInit(12,
			wxFONTFAMILY_DEFAULT,
			wxFONTSTYLE_NORMAL,
			wxFONTWEIGHT_NORMAL,
			false,
			getFontName(),
			wxFONTENCODING_DEFAULT);
	wxFont myfont = wxGetFontFromUser(this, fontInit);
	if (!myfont.Ok()) {
		// User pressed Cancel
		return;
	} else {
		m_pTCFontName->SetValue(myfont.GetFaceName());
		Refresh();
	}
}


void OutputAreaOptionsWizardPage::OnPageChanged(wxWizardEvent& event)
{
	if (event.GetDirection()) { // Forward
		OnPageChangedForward();
	} else {
		// Nothing to do...
	}

}

void OutputAreaOptionsWizardPage::OnPageChangedForward()
{
	int nNoOfInterlinearLines = ((ConnectionWizard*)GetParent())->getNoOfInterlinearLines();
	if (((ConnectionWizard*)GetParent())->getCreateOrEdit() == kCreateConfiguration) {
		// CREATE configuration
		for (int i = 1;
		     i <= nNoOfInterlinearLines;
		     ++i) {
			// FIXME: Do something more intelligent...
			m_font_names[i] = wxString(wxT("Courier New"));
		}
	} else {
		// EDIT configuration

		// FIXME: Do something different!
		for (int i = 1;
		     i <= nNoOfInterlinearLines;
		     ++i) {
			m_font_names[i] = wxString(wxT("Courier New"));
		}
	}
	Validate();
	Refresh();
}


void OutputAreaOptionsWizardPage::OnSelectionChanged()
{
	wxString strSelection = m_pInterlinearLineDropdownWidget->getSelection();
	if (strSelection.IsEmpty()) {
		m_nCurrentInterlinearLine = -1;
		return; 
	} else {
		int nInterlinearLine = (int) string2long(std::string((const char*)strSelection.mb_str(wxConvUTF8)));
		saveFont();
		setFont(nInterlinearLine);
	}
}

void OutputAreaOptionsWizardPage::saveFont()
{
	wxString strFont = m_pTCFontName->GetValue();

	if (!strFont.IsEmpty() && m_nCurrentInterlinearLine != -1) {
		m_font_names[m_nCurrentInterlinearLine] = strFont;
	}
}

void OutputAreaOptionsWizardPage::setFont(int nNewInterlinearLine)
{
	m_nCurrentInterlinearLine = nNewInterlinearLine;
	wxString strNewFont = m_font_names[nNewInterlinearLine];
	m_pTCFontName->SetValue(strNewFont);
}




//////////////////////////////////////////////////
//
// GeneralOptionsWizardPage
//
//////////////////////////////////////////////////

GeneralOptionsWizardPage::GeneralOptionsWizardPage(wxWizard *pParent, 
		     const wxBitmap& bitmap)
	: wxWizardPageSimple(pParent, NULL, NULL, bitmap)
{
	CreateControls();
}

GeneralOptionsWizardPage::~GeneralOptionsWizardPage()
{
}

void GeneralOptionsWizardPage::CreateControls()
{
	wxStaticText *pInstructions = new wxStaticText(this,
						       wxID_ANY,
						       wxT("Select the general options."));

	wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);
	pMainSizer->Add(pInstructions);

	m_pCBApplyFocus = new wxCheckBox(this,
					 wxID_ANY,
					 wxT("Highlight focus objects"),
					 wxDefaultPosition,
					 wxDefaultSize);

	pMainSizer->Add(m_pCBApplyFocus);


	m_pCBBreakAfterRaster = new wxCheckBox(this,
					       wxID_ANY,
					       wxT("Break after raster unit"),
					       wxDefaultPosition,
					       wxDefaultSize);
	pMainSizer->Add(m_pCBBreakAfterRaster);


	m_pCBSingleRasterUnits = new wxCheckBox(this,
						wxID_ANY,
						wxT("Single raster units"),
						wxDefaultPosition,
						wxDefaultSize);
	pMainSizer->Add(m_pCBSingleRasterUnits);

	wxString arrDirectionChoices[2] = {
		wxString(wxT("Left-to-right")),
		wxString(wxT("Right-to-left")),
	};

	m_pRBLTR_RTL = new wxRadioBox(this,
				      wxID_ANY,
				      wxT("Database text direction"),
				      wxDefaultPosition,
				      wxDefaultSize,
				      2,
				      arrDirectionChoices);
	pMainSizer->Add(m_pRBLTR_RTL);

	SetSizer(pMainSizer);
	pMainSizer->Fit(this);
}



bool GeneralOptionsWizardPage::getApplyFocus()
{
	return m_pCBApplyFocus->GetValue();
}


bool GeneralOptionsWizardPage::getBreakAfterRaster()
{
	return m_pCBBreakAfterRaster->GetValue();
}


bool GeneralOptionsWizardPage::getSingleRasterUnits()
{
	return m_pCBSingleRasterUnits->GetValue();
}


bool GeneralOptionsWizardPage::getIsRTL()
{
	return m_pRBLTR_RTL->GetSelection() != 0;
}


//////////////////////////////////////////////////
//
// InputAreaOptionsWizardPage
//
//////////////////////////////////////////////////

BEGIN_EVENT_TABLE( InputAreaOptionsWizardPage , wxWizardPageSimple )
	EVT_BUTTON(ID_FONT_BUTTON1, InputAreaOptionsWizardPage::OnBrowseFont)
END_EVENT_TABLE()

InputAreaOptionsWizardPage::InputAreaOptionsWizardPage(wxWizard *pParent, 
		     const wxBitmap& bitmap)
	: wxWizardPageSimple(pParent, NULL, NULL, bitmap)
{
	CreateControls();
}

InputAreaOptionsWizardPage::~InputAreaOptionsWizardPage()
{
}

void InputAreaOptionsWizardPage::CreateControls()
{
	wxStaticText *pInstructions = new wxStaticText(this,
						       wxID_ANY,
						       wxT("Select the Input Area options."));

	wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);
	pMainSizer->Add(pInstructions,
			0,
			wxBOTTOM,
			15);

	wxBoxSizer *pFontSizer = new wxBoxSizer(wxHORIZONTAL);

	wxStaticText *pSTFontName = new wxStaticText(this,
						     wxID_ANY,
						     wxT("Font name:"));
	pFontSizer->Add(pSTFontName);


	// FIXME: Set it to what it is in the current config file,
	// if doing Edit config file.
	wxString strDefaultFont = wxT("Courier New");
	wxSize mySize = wxDefaultSize;
	mySize.x = 150;
	m_pTCFontName = new wxTextCtrl(this,
				       wxID_ANY,
				       strDefaultFont,
				       wxDefaultPosition,
				       mySize,
				       0); // Style
	pFontSizer->Add(m_pTCFontName,
			    1,
			    wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,
			    5);

	wxButton *pFontButton = new wxButton(this, ID_FONT_BUTTON1,
					     wxT("Browse font..."),
					     wxDefaultPosition,
					     wxDefaultSize,
					     wxBU_EXACTFIT);
	pFontSizer->Add(pFontButton);

	pMainSizer->Add(pFontSizer,
			0,
			wxALL,
			5);


	wxBoxSizer *pFontSizeSizer = new wxBoxSizer(wxHORIZONTAL);

	wxStaticText *pSTFontSize = new wxStaticText(this,
						     wxID_ANY,
						     wxT("Font size:"));
	pFontSizeSizer->Add(pSTFontSize);
	// default value. 
	// FIXME: Add from current config file if we are editing!
	wxString strDefaultValue = wxT("12");

	m_pSCFontSize = new wxSpinCtrl(this, wxID_ANY,
				       strDefaultValue,
				       wxDefaultPosition,
				       wxDefaultSize,
				       wxSP_ARROW_KEYS,
				       6,  // min
				       48); // max,
	pFontSizeSizer->Add(m_pSCFontSize,
			    0,
			    wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,
			    5);
	pMainSizer->Add(pFontSizeSizer,
			0,
			wxALL,
			5);

	SetSizer(pMainSizer);
	pMainSizer->Fit(this);
}


wxString InputAreaOptionsWizardPage::getFontName()
{
	return m_pTCFontName->GetValue();
}


int InputAreaOptionsWizardPage::getFontSize()
{
	return m_pSCFontSize->GetValue();
}


void InputAreaOptionsWizardPage::OnBrowseFont(wxCommandEvent& event)
{
	(void)(event); // Silence a warning	

	wxFont fontInit(getFontSize(),
			wxFONTFAMILY_DEFAULT,
			wxFONTSTYLE_NORMAL,
			wxFONTWEIGHT_NORMAL,
			false,
			getFontName(),
			wxFONTENCODING_DEFAULT);
	wxFont myfont = wxGetFontFromUser(this, fontInit);
	if (!myfont.Ok()) {
		// User pressed Cancel
		return;
	} else {
		m_pTCFontName->SetValue(myfont.GetFaceName());
		m_pSCFontSize->SetValue(myfont.GetPointSize());
		Refresh();
	}
}








//////////////////////////////////////////////////
//
// ConnectionWizard
//
//////////////////////////////////////////////////

/*!
 * ConnectionWizard event table definition
 */
BEGIN_EVENT_TABLE( ConnectionWizard, wxWizard )
END_EVENT_TABLE()

/*!
 * ConnectionWizard constructor.
 */
ConnectionWizard::~ConnectionWizard()
{
}



/*!
 * ConnectionWizard constructor.
 */
ConnectionWizard::ConnectionWizard(wxWindow* parent, wxWindowID id, const wxString& caption, const wxBitmap& bitmap, const wxPoint& pos, long style)
	: wxWizard(parent, id, caption, bitmap, pos, style),
	  m_pPage1(0),
	  m_pConnectionPage(0),
	  m_pConnectionStatusPage(0),
	  m_pRasterUnitPage(0),
	  m_pDataUnitsPage(0),
	  m_pDataUnitOrderPage(0),
	  m_pDataUnitFeaturesPage(0),
	  m_pDataUnitFeatureOrderPage(0),
	  m_pReferenceUnitPage(0),
	  m_pReferenceFeatureOrderPage(0),
	  m_pGeneralOptionsWizardPage(0),
	  m_pInputAreaOptionsWizardPage(0)
{
	CreateControls();
}

EmdrosSchema *ConnectionWizard::GetSchema(void)
{
	if (m_pConnectionStatusPage == 0) {
		return 0;
	} else {
		return m_pConnectionStatusPage->GetSchema();
	}
}

WXStringVec ConnectionWizard::getDataUnits()
{
	if (m_pDataUnitsPage == 0) {
		return WXStringVec();
	} else {
		return m_pDataUnitsPage->getDataUnits();
	}
}

WXStringVec ConnectionWizard::getDataUnitFeatures()
{
	if (m_pDataUnitFeaturesPage == 0) {
		return WXStringVec();
	} else {
		return m_pDataUnitFeaturesPage->getDataUnitFeatures();
	}
}

wxString ConnectionWizard::getReferenceUnit()
{
	if (m_pReferenceUnitPage == 0) {
		return wxString();
	} else {
		return m_pReferenceUnitPage->getUnit();
	}
}

WXStringVec ConnectionWizard::getReferenceFeatures()
{
	if (m_pReferenceFeaturesPage == 0) {
		return WXStringVec();
	} else {
		if (hasReferenceUnit()) {
			return m_pReferenceFeaturesPage->getReferenceFeatures();
		} else {
			return WXStringVec();
		}
	}
}

bool ConnectionWizard::hasReferenceUnit()
{
	return !getReferenceUnit().IsEmpty();
}



/*!
 * Should we show tooltips?
 */
bool ConnectionWizard::ShowToolTips()
{
	return TRUE;
}

/*!
 * Get bitmap resources
 */
wxBitmap ConnectionWizard::GetBitmapResource( const wxString& name )
{
	(void)(name); // Silence a warning	

	// Bitmap retrieval
	////@begin ConnectionWizard bitmap retrieval
	return wxNullBitmap;
	////@end ConnectionWizard bitmap retrieval
}

/*!
 * Get icon resources
 */
wxIcon ConnectionWizard::GetIconResource( const wxString& name )
{
	(void)(name); // Silence a warning	

	// Icon retrieval
	////@begin ConnectionWizard icon retrieval
	return wxNullIcon;
	////@end ConnectionWizard icon retrieval
}

bool ConnectionWizard::RunIt()
{
	bool bResult = RunWizard(m_pPage1);
	Destroy();
	return bResult;
}

void ConnectionWizard::CreateControls()
{
	m_pPage1 = new FirstPage(this);
	m_pCreateEditConfigurationPage = new CreateEditConfigurationWizardPage(this, kCreateConfiguration); // Could also be kEditConfiguration; it only sets the default...
	m_pGetConfigurationFilenamePage = new GetConfigurationFilenameWizardPage(this, wxString(wxT(""))); // NOTE: String parameter is for setting default parameter. Should be set to the current one...
	m_pConnectionPage = new DBConnectionWizardPage(app_checkConfiguration, this);
	m_pConnectionStatusPage = new ConnectionStatusWizardPage(this);
	m_pRasterUnitPage = new RasterUnitWizardPage(this, true, wxT("Choose raster unit.\n\nThe raster unit is the unit of context to be displayed for a given 'hit'.\nYou must choose exactly 1 raster unit before you can go on.\n"), wxT("Raster unit"));
	m_pDataUnitsPage = new DataUnitsWizardPage(this);
	m_pDataUnitOrderPage = new DataUnitOrderWizardPage(this);
	m_pDataUnitFeaturesPage = new DataUnitFeaturesWizardPage(this);
	m_pDataUnitFeatureOrderPage = new DataUnitFeatureOrderWizardPage(this);
	m_pReferenceUnitPage = new ReferenceUnitWizardPage(this);
	m_pReferenceFeaturesPage = new ReferenceFeaturesWizardPage(this);
	m_pReferenceFeatureOrderPage = new ReferenceFeatureOrderWizardPage(this);
	m_pGeneralOptionsWizardPage = new GeneralOptionsWizardPage(this);
	m_pInputAreaOptionsWizardPage = new InputAreaOptionsWizardPage(this);
	m_pOutputAreaOptionsWizardPage = new OutputAreaOptionsWizardPage(this);

	wxWizardPageSimple::Chain(m_pPage1, m_pCreateEditConfigurationPage);
	wxWizardPageSimple::Chain(m_pCreateEditConfigurationPage, m_pGetConfigurationFilenamePage);
	wxWizardPageSimple::Chain(m_pGetConfigurationFilenamePage, m_pConnectionPage);
	wxWizardPageSimple::Chain(m_pConnectionPage, m_pConnectionStatusPage);
	wxWizardPageSimple::Chain(m_pConnectionStatusPage, m_pRasterUnitPage);
	wxWizardPageSimple::Chain(m_pRasterUnitPage, m_pDataUnitsPage);
	wxWizardPageSimple::Chain(m_pDataUnitsPage, m_pDataUnitOrderPage);
	wxWizardPageSimple::Chain(m_pDataUnitOrderPage, m_pDataUnitFeaturesPage);
	wxWizardPageSimple::Chain(m_pDataUnitFeaturesPage, m_pDataUnitFeatureOrderPage);
	wxWizardPageSimple::Chain(m_pDataUnitFeatureOrderPage, m_pReferenceUnitPage);
	wxWizardPageSimple::Chain(m_pReferenceUnitPage, m_pReferenceFeaturesPage);
	wxWizardPageSimple::Chain(m_pReferenceFeaturesPage, m_pReferenceFeatureOrderPage);
	wxWizardPageSimple::Chain(m_pReferenceFeatureOrderPage, m_pGeneralOptionsWizardPage);
	wxWizardPageSimple::Chain(m_pGeneralOptionsWizardPage, m_pInputAreaOptionsWizardPage);
	wxWizardPageSimple::Chain(m_pInputAreaOptionsWizardPage, m_pOutputAreaOptionsWizardPage);
	
	GetPageAreaSizer()->Add(m_pPage1);
}


eCreateOrEditConfiguration ConnectionWizard::getCreateOrEdit()
{
	if (m_pCreateEditConfigurationPage == 0) {
		return kCreateConfiguration;
	} else {
		return m_pCreateEditConfigurationPage->getCreateOrEdit();
	}
}

int ConnectionWizard::getNoOfInterlinearLines()
{
	// Get as the length of the largest WXStringVec in duf_map,
	// i.e., as the number of data unit features for the
	// data unit with the highest number of data unit features.

	std::map<std::string, WXStringVec > duf_map = getDataUnitFeatureMap();
	int nLargestVecSize = 1;
	std::map<std::string, WXStringVec >::const_iterator ci = duf_map.begin();
	while (ci != duf_map.end()) {
		int nLength = (int) ci->second.size();
		if (nLength > nLargestVecSize) {
			nLargestVecSize = nLength;
		}
		++ci;
	}

	return nLargestVecSize;
}


std::map<std::string, WXStringVec > ConnectionWizard::getDataUnitFeatureMap()
{
	WXStringVec dataUnitFeatures = getDataUnitFeatures();

	WXStringVec::const_iterator ci = dataUnitFeatures.begin();
	std::map<std::string, WXStringVec > duf_map;
	while (ci != dataUnitFeatures.end()) {
		wxString strDataUnitDotFeature = *ci;
		std::string dataUnitDotFeature = std::string((const char*) strDataUnitDotFeature.mb_str(wxConvUTF8));
		std::vector<std::string> dufvec;
		split_string(dataUnitDotFeature, ".", dufvec);
		if (dufvec.size() == 2) {
			std::string dataUnit = dufvec[0];
			std::string featureName = dufvec[1];
			if (duf_map.find(dataUnit) == duf_map.end()) {
				duf_map.insert(std::make_pair(dataUnit, WXStringVec()));
			}
			wxString strFeatureName = wxString(featureName.c_str(), wxConvUTF8);
			wxString strDataUnit = wxString(dataUnit.c_str(), wxConvUTF8);
			wxString strDuDotFeature = strDataUnit + wxT(".") + strFeatureName;
			duf_map[dataUnit].push_back(strDuDotFeature);
		}
		++ci;
	}
	return duf_map;
}
