//
// importerwizard.cpp
// Martin Petersen and Ulrik Petersen
// Created: 10/11-2006
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
#pragma implementation "importerwizard.h"
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
#include <wx/utils.h> // For wxBusyCursor
#include <wx/stattext.h>
#include <wx/statline.h> // for wxStaticLine
#include <importerwizard.h>
#include <debug.h>
#include <monads.h>
#include <string_func.h>

#include <pennimporter.h>
#include <plaintextimporter.h>
#include <negraimporter.h>
#include <tigerxmlimporter.h>
#include <unboundbibleimporter.h>
#include <slashedtextimporter.h>
#include <sfmimporter.h>


BEGIN_EVENT_TABLE( ImporterWizard, wxWizard )
END_EVENT_TABLE()

ImporterWizard::ImporterWizard(wxWindow* parent, int id, const wxString& title, 
			       const wxBitmap& bitmap, const wxPoint& pos, long style)
: wxWizard(parent, id, title, bitmap, pos, style)
{
	CreateControls();
}

ImporterWizard::~ImporterWizard()
{
	
}

bool ImporterWizard::RunIt()
{
	bool bResult = false;

	if (RunWizard(m_pPage1)) {
		bResult = true;
		wxEmdrosInfoMessage(wxT("About to import."));
		bResult = Process();
		if (bResult) {
			(void) wxMessageBox(wxString(wxT("MQL file with import successfully dumped as:\n\n"))+ m_pSummaryPage->GetOutputFileName(),
					    wxT("Import success"),
					    wxOK | wxICON_INFORMATION);
		} else {
			(void) wxMessageBox(wxT("Import failed."),
					    wxT("Import failure"),
					    wxOK | wxICON_ERROR);
		}
	} else {
		wxEmdrosInfoMessage(wxT("You cancelled the wizard."));
		bResult = false;
	}

	Close(true);

	return bResult;
}

void ImporterWizard::CreateControls()
{
	m_pPage1 = new wxWizardPageSimple(this);
	m_pFormatPage = new ImportFormatWizardPage(this);
	m_pFilePage = new ImportFileNameWizardPage(this);
	m_pDatabasePage = new ImportDatabaseWizardPage(this);
	m_pOutputPage = new ImportOutputFileWizardPage(this);
	m_pAdvancedPage = new ImportAdvancedWizardPage(this);
	m_pSummaryPage = new ImportSummaryWizardPage(this);
	new wxStaticText(m_pPage1, wxID_ANY, 
			 wxT("\n")
			 wxT("Welcome to the Emdros Importer Wizard.\n\n")
			 wxT("This wizard will help you import text into Emdros.\n\n")
			 wxT("The wizard helps you build an MQL file from your text,\n")
			 wxT("which can then be imported into Emdros with the mql program.\n\n")
			 wxT("Click Next to continue.\n"), 
			 wxPoint(5,5));
	wxWizardPageSimple::Chain(m_pPage1, m_pFormatPage);
	wxWizardPageSimple::Chain(m_pFormatPage, m_pFilePage);
	wxWizardPageSimple::Chain(m_pFilePage, m_pDatabasePage);
	wxWizardPageSimple::Chain(m_pDatabasePage, m_pOutputPage);
	wxWizardPageSimple::Chain(m_pOutputPage, m_pAdvancedPage);
	wxWizardPageSimple::Chain(m_pAdvancedPage, m_pSummaryPage);
	GetPageAreaSizer()->Add(m_pPage1);
}

bool ImporterWizard::Process()
{
	EmdrosImporterBase *pImporter = GetImporter();
	pImporter->readDocumentsFromFiles(m_pSummaryPage->GetImportFileName());
	std::ofstream fout;
	std::string outputFile = std::string((const char*)m_pSummaryPage->GetOutputFileName().mb_str(wxConvUTF8));
	std::string dbName = std::string((const char*)m_pSummaryPage->GetDatabaseName().mb_str(wxConvUTF8));
	bool bResult = false;
	if (openFile(fout, outputFile)) {
		wxBusyCursor wait;
		if (m_pSummaryPage->GetEmitCreateDatabase()) {
			fout << "CREATE DATABASE '" << dbName << "' GO" << std::endl;
		}
		if (dbName != "" || m_pSummaryPage->GetEmitCreateDatabase()) {
			fout << "USE DATABASE '" << dbName << "' GO" << std::endl;
		}
		
		if (m_pSummaryPage->GetEmitSchema()
		    || m_pSummaryPage->GetEmitCreateDatabase()) {
			pImporter->putSchema(&fout);
		}
		pImporter->putCorpusAsMQL(&fout);
		bResult = true;
	} else {
		bResult = false;
	}

	if (bResult) {
		wxEmdrosInfoMessage(wxT("Successfully emitted corpus."));
	}
	return bResult;
}

EmdrosImporterBase* ImporterWizard::GetImporter()
{
	eImporter imp = m_pSummaryPage->GetImportFormat();
	monad_m starting_monad;
	id_d_t starting_id_d;
	m_pSummaryPage->GetStartingMonad().ToLong(&starting_monad, 10);
	m_pSummaryPage->GetStartingIDD().ToLong(&starting_id_d, 10);
	std::string myStdString = 
		std::string((const char*)m_pSummaryPage->GetSFMImportFileName().mb_str(wxConvUTF8));
	EmdrosImporterBase *pBase;
	switch(imp) {
	case kPlainText:
		pBase = new PlainTextImporter(starting_monad, starting_id_d);
		break;
	case kSlashedText:
		pBase = new SlashedTextImporter(starting_monad, starting_id_d);
		break;
	case kNeGRA:
		pBase = new NEGRAImporter(starting_monad, starting_id_d);
		break;
	case kTIGERXML:
		pBase = new TIGERXMLImporter(starting_monad, starting_id_d,
					     "", // FIXME: Fill these out!
					     "");
		break;
	case kSFM:
		pBase = new SFMImporter(starting_monad, starting_id_d, myStdString);
		break;
	case kUnboundBible:
		pBase = new UnboundBibleImporter();
		break;
	case kPennTreebank:
		pBase = new PennTreebankImporter(true, starting_monad, starting_id_d);
		break;
	default:
		ASSERT_THROW(false, "Unknown import format.");
		break;
	}
	return pBase;
}

bool ImporterWizard::openFile(std::ofstream& fout, const std::string& filename)
{
	bool bResult = false;
	fout.open(filename.c_str());
	if (!fout) {
		wxEmdrosErrorMessage(wxT("Could not open file for writing."));
		bResult = false;
	} else {
		bResult = true;
	}

	return bResult;
}

//////////////////////////////////////////////////////////////////
//ImportFormatWizardPage
//
//
//////////////////////////////////////////////////////////////////
#define NO_OF_CHOICES (7)
 
wxString mychoices[NO_OF_CHOICES] = {
	wxString(wxT("Plain text")),
	wxString(wxT("Slashed text")),
	wxString(wxT("Penn Treebank")),
	wxString(wxT("NeGRA")),
	wxString(wxT("TIGERXML")),
	wxString(wxT("SFM (SIL Standard Format Marker)")),
	wxString(wxT("Unbound Bible")),
};
 
// Must be in the same order as above
eImporter mychoices_enum[NO_OF_CHOICES] = {
	kPlainText,
	kSlashedText,
	kPennTreebank,
	kNeGRA,
	kTIGERXML,
	kSFM,
	kUnboundBible
};

ImportFormatWizardPage::ImportFormatWizardPage(wxWizard *parent)
	:wxWizardPageSimple(parent)
{
	m_pComboBox = new wxComboBox(this, 
				     wxID_ANY, mychoices[0], wxDefaultPosition,
				     wxDefaultSize, NO_OF_CHOICES, mychoices,
				     wxCB_READONLY);
	wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);
	pMainSizer->Add(new wxStaticText(this,
					 wxID_ANY,
					 wxT("Choose import format:")),
			0, wxALL, 5);
	pMainSizer->Add(m_pComboBox, 0, wxALL, 5);
	SetSizer(pMainSizer);
	pMainSizer->Fit(this);
}

eImporter ImportFormatWizardPage::GetImportFormat(void) const 
{
	wxString choice = m_pComboBox->GetValue();
	for (int index = 0; index < NO_OF_CHOICES; ++index) {
		if (mychoices[index].Cmp(choice) == 0) {
			return mychoices_enum[index];
		}
	};
	return kPlainText;
}

////////////////////////////////////////////////////////////
//ImportFileNameWizardPage
//
//
////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( ImportFileNameWizardPage , wxWizardPageSimple )
   EVT_BUTTON(ID_BUTTON_BROWSE_IMPORT_FILENAME, ImportFileNameWizardPage::OnBrowseFile)
   EVT_WIZARD_PAGE_CHANGED( wxID_ANY , ImportFileNameWizardPage::OnPageChanged )
   EVT_WIZARD_PAGE_CHANGING( wxID_ANY , ImportFileNameWizardPage::OnPageChanging )
END_EVENT_TABLE()

ImportFileNameWizardPage::ImportFileNameWizardPage(wxWizard *parent)
:wxWizardPageSimple(parent)
{
	m_bChooseMultiple = false;
	CreateControls();
}

void ImportFileNameWizardPage::OnBrowseFile(wxCommandEvent& event)
{
	(void)(event); // Silence a warning
	
	long style = wxFD_OPEN | wxFD_FILE_MUST_EXIST;

	if (m_bChooseMultiple) {
		style |= wxFD_MULTIPLE;
	}
	wxFileDialog dlgFileDialog(this,
				   wxT("Choose file to import"),
				   ::wxGetHomeDir(),
				   wxT(""),
				   wxT("All files (*)|*"),
				   style);
	int nReturn;
	if ((nReturn = dlgFileDialog.ShowModal()) == wxID_OK) {
		if (!m_bChooseMultiple)
			m_textCtrl->SetValue(dlgFileDialog.GetPath());
		else {
			m_strValue = wxT("");
			wxArrayString filePaths;
			dlgFileDialog.GetPaths(filePaths);
			for(unsigned int i = 0; i < filePaths.GetCount(); ++i)
				m_strValue+=filePaths.Item(i)+wxT("\n");
			TransferDataToWindow();
		}
	}
}

void ImportFileNameWizardPage::OnPageChanging(wxWizardEvent& event)
{
	if (event.GetDirection()) {
		TransferDataFromWindow();
		if (m_strValue.IsEmpty()) {
			wxEmdrosErrorMessage(wxT("You must enter an import file name!"));
			event.Veto();
		}
	}
}

void ImportFileNameWizardPage::OnPageChanged(wxWizardEvent& event)
{
	(void)(event); // Silence a warning

	if (this->GetPrev() == NULL)
		m_bChooseMultiple = false;
	else
		switch (((const ImportFormatWizardPage*)GetPrev())->GetImportFormat()) {
		case kPlainText:
		case kSlashedText:
		case kNeGRA:
		case kTIGERXML:
		case kSFM:
		case kUnboundBible:
			m_bChooseMultiple = false;
			break;
		case kPennTreebank:
			m_bChooseMultiple = true;
			break;
		default:
			ASSERT_THROW(false, "Unknown import format.");
			m_bChooseMultiple = false; // Just to make the compiler give no warning.
			break;
		}
}

void ImportFileNameWizardPage::CreateControls(void)
{
	m_textCtrl = NULL;
	
	wxSize sizeBroad = wxDefaultSize;
	sizeBroad.SetWidth(350);
	sizeBroad.SetHeight(120);
 
	wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(pMainSizer);
 
 
	wxStaticText* itemStaticText13 = new wxStaticText( this, wxID_ANY, wxT("Choose file(s) to import, by clicking on \"Browse\"."), wxDefaultPosition, wxDefaultSize, 0 );
	pMainSizer->Add(itemStaticText13, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
 
	wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
	pMainSizer->Add(itemBoxSizer4, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);
 
	m_textCtrl = new wxTextCtrl( this, ID_IMPORT_FORMAT_TEXTCTRL, wxT(""), wxDefaultPosition, sizeBroad, wxTE_MULTILINE | wxTE_DONTWRAP);
	itemBoxSizer4->Add(m_textCtrl, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
 
	wxButton *btnBrowse = new wxButton( this, ID_BUTTON_BROWSE_IMPORT_FILENAME, _("Browse..."), wxDefaultPosition, wxDefaultSize, 0 );
	itemBoxSizer4->Add(btnBrowse, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);
 
 
	// Set validators
	m_textCtrl->SetValidator( wxTextValidator(wxFILTER_NONE, & m_strValue) );
 
	SetSizer(pMainSizer);
	pMainSizer->Fit(this);
	
}

/*
  std::string myStdString = std::string((const char*)myString.mb_str(wxConvUTF8));
  extern void split_string(const std::string& instring, const std::string& splitchars, std::list<std::string>& outlist);
*/

std::list<std::string> ImportFileNameWizardPage::GetImportFileName()
{
	std::list<std::string> myList;
	std::string myStdString = std::string((const char*)m_strValue.mb_str(wxConvUTF8));
	split_string(myStdString, "\n", myList);
	return myList;
}

//////////////////////////////////////////////////////////////////
//ImportDatabaseWizardPage
//
//
//////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( ImportDatabaseWizardPage , wxWizardPageSimple )
EVT_WIZARD_PAGE_CHANGING(wxID_ANY, ImportDatabaseWizardPage::OnPageChanging)
EVT_UPDATE_UI(ID_BROWSE_WIDGET, ImportDatabaseWizardPage::OnSMFImportChosen)
END_EVENT_TABLE()

ImportDatabaseWizardPage::ImportDatabaseWizardPage(wxWizard *parent)
:wxWizardPageSimple(parent)
{
	m_bEmitDatabase = m_bEmitSchema = false;
	CreateControls();
}

void ImportDatabaseWizardPage::CreateControls()
{
	m_textCtrl = NULL;
	
	wxSize sizeBroad = wxDefaultSize;
	sizeBroad.SetWidth(300);
 
	wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(pMainSizer);	
	
	wxStaticText* itemStaticText13 = new wxStaticText( this, wxID_STATIC, wxT("Please choose database settings. "), wxDefaultPosition, wxDefaultSize, 0 );
	pMainSizer->Add(itemStaticText13, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
 
	m_pDbCheckbox = new wxCheckBox(this, ID_CHECKBOX_EMIT_DATABASE, wxT("Emit 'CREATE DATABASE'"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_bEmitDatabase));
	pMainSizer->Add(m_pDbCheckbox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
	pMainSizer->Add(itemBoxSizer4, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 0);
	
	wxStaticText* itemStaticTextDb = new wxStaticText( this, ID_STATIC_TEXT_DATABASE_NAME, wxT("Enter Database name: "), wxDefaultPosition, wxDefaultSize, 0 );
	itemBoxSizer4->Add(itemStaticTextDb, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	m_textCtrl = new wxTextCtrl( this, ID_IMPORT_DATABASE_TEXTCTRL, wxT(""), wxDefaultPosition, sizeBroad, 0);
	itemBoxSizer4->Add(m_textCtrl, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
 
 	m_pSchemaCheckbox = new wxCheckBox(this, wxID_ANY, wxT("Emit Schema"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_bEmitSchema));
	pMainSizer->Add(m_pSchemaCheckbox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

	wxStaticLine* itemStaticLine1 = new wxStaticLine( this , wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	pMainSizer->Add(itemStaticLine1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5);
	
	//Stuff for the SMFImporter-thing-a-me
	m_pBrowseWidget = new BrowseWidget(this, ID_BROWSE_WIDGET,
					   true,
					   wxT("SFM Import file:"),
					   wxT(""),
					   wxT("Choose SFM Import file"),
					   wxT("All files (*)|*"));
	pMainSizer->Add(m_pBrowseWidget, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	// Set validators
	m_textCtrl->SetValidator( wxTextValidator(wxFILTER_NONE, & m_strValue) );
 
	SetSizer(pMainSizer);
	pMainSizer->Fit(this);
}

wxString ImportDatabaseWizardPage::GetDatabaseName() const
{
	return m_strValue;
}

wxString ImportDatabaseWizardPage::GetSFMImportFileName() const
{
	return m_pBrowseWidget->getValue();
}

bool ImportDatabaseWizardPage::GetEmitCreateDatabase() const
{
	return m_bEmitDatabase;
}

bool ImportDatabaseWizardPage::GetEmitSchema() const
{
	return m_bEmitSchema;
}

void ImportDatabaseWizardPage::OnPageChanging(wxWizardEvent& event)
{
	if (event.GetDirection()) {
		TransferDataFromWindow();
		if (m_strValue.IsEmpty() && GetEmitCreateDatabase()) {
			wxEmdrosErrorMessage(wxT("If you want to emit 'CREATE DATABASE',\nyou must enter a database name!"));
			event.Veto();
		}
	}
}

void ImportDatabaseWizardPage::OnSMFImportChosen(wxUpdateUIEvent& event)
{
	ImportFormatWizardPage *p = (ImportFormatWizardPage*)this->GetPrev()->GetPrev();
	eImporter e = p->GetImportFormat();
	if (e == kSFM) {
		event.Enable(true);
	} else {
		event.Enable(false);
	}
	TransferDataFromWindow();
}

////////////////////////////////////////////////////////////
//ImportOutputFileWizardPage
//
//
////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( ImportOutputFileWizardPage,  wxWizardPageSimple )
   EVT_WIZARD_PAGE_CHANGING( wxID_ANY , ImportOutputFileWizardPage::OnPageChanging )
END_EVENT_TABLE()


ImportOutputFileWizardPage::ImportOutputFileWizardPage(wxWizard *parent)
 :wxWizardPageSimple(parent)
{
	CreateControls();
}

void ImportOutputFileWizardPage::CreateControls()
{
	
	wxSize sizeBroad = wxDefaultSize;
	sizeBroad.SetWidth(300);
 
	wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(pMainSizer);	
	
	wxStaticText* itemStaticText13 = new wxStaticText( this, wxID_STATIC, wxT("Choose an output filename, to which the MQL will be dumped."), wxDefaultPosition, wxDefaultSize, 0 );
	pMainSizer->Add(itemStaticText13, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	m_pBrowseWidget = new BrowseWidget(this, ID_BROWSE_WIDGET,
					   true,
					   wxT("Output filename:"),
					   m_strOutputFileName,
					   wxT("Choose an output file"),
					   wxT("All files (*)|*"), 
					   wxFD_SAVE | wxFD_OVERWRITE_PROMPT
					   );
	pMainSizer->Add(m_pBrowseWidget, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	SetSizer(pMainSizer);
	pMainSizer->Fit(this);
}

wxString ImportOutputFileWizardPage::GetOutputFileName()
{
	return m_pBrowseWidget->getValue();
}

void ImportOutputFileWizardPage::OnPageChanging(wxWizardEvent& event)
{
	if (event.GetDirection()) {
		TransferDataFromWindow();
		if (GetOutputFileName().IsEmpty()) {
			wxEmdrosErrorMessage(wxT("You must enter the name of a file\nto use for output!"));
			event.Veto();
		}
	}
}



////////////////////////////////////////////////////////////
//ImportAdvancedWizardPage
////////////////////////////////////////////////////////////
ImportAdvancedWizardPage::ImportAdvancedWizardPage(wxWizard *parent)
 :wxWizardPageSimple(parent)
{
	m_starting_id_d = m_starting_monad = wxT("1");
	CreateControls();
}

void ImportAdvancedWizardPage::CreateControls()
{
	m_pMonadTextCtrl = NULL;
	m_pIdDTextCtrl = NULL;
	
	wxSize sizeBroad = wxDefaultSize;
	sizeBroad.SetWidth(300);
 
	wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(pMainSizer);	
	
	wxStaticText* itemStaticText13 = new wxStaticText( this, wxID_STATIC, wxT("Advanced settings. "), wxDefaultPosition, wxDefaultSize, 0 );
	pMainSizer->Add(itemStaticText13, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
 
	wxStaticText* helpfulNotice = new wxStaticText( this, wxID_STATIC, wxT("These can be left as is unless you really know what you're doing. "), wxDefaultPosition, wxDefaultSize, 0 );
	pMainSizer->Add(helpfulNotice, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	//Monad boxsizer
	wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
	pMainSizer->Add(itemBoxSizer4, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);
	
	//id_d boxsizer
	wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
	pMainSizer->Add(itemBoxSizer5, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 0);

	wxStaticText* itemStaticTextMonad = new wxStaticText( this, wxID_ANY, wxT("Starting monad:"), wxDefaultPosition, wxDefaultSize, 0 );
	itemBoxSizer4->Add(itemStaticTextMonad, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	m_pMonadTextCtrl = new wxTextCtrl( this, wxID_ANY, wxT("1"), wxDefaultPosition, sizeBroad, 0);
	itemBoxSizer4->Add(m_pMonadTextCtrl, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
		
	wxStaticText* itemStaticTextIDD = new wxStaticText( this, wxID_ANY, wxT("Starting id_d:"), wxDefaultPosition, wxDefaultSize, 0 );
	itemBoxSizer5->Add(itemStaticTextIDD, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	m_pIdDTextCtrl = new wxTextCtrl( this, wxID_ANY, wxT("1"), wxDefaultPosition, sizeBroad, 0);
	itemBoxSizer5->Add(m_pIdDTextCtrl, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
 

	// Set validators
	m_pMonadTextCtrl->SetValidator( wxTextValidator(wxFILTER_NUMERIC, & m_starting_monad) );
	m_pIdDTextCtrl->SetValidator( wxTextValidator(wxFILTER_NUMERIC, & m_starting_id_d) );
 
	SetSizer(pMainSizer);
	pMainSizer->Fit(this);
}

wxString ImportAdvancedWizardPage::GetStartingMonad() const
{
	return m_starting_monad;
}

wxString ImportAdvancedWizardPage::GetStartingIDD() const
{
	return m_starting_id_d;
}

//////////////////////////////////////////////////////////////////
//ImportSummaryWizardPage
//////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( ImportSummaryWizardPage , wxWizardPageSimple )
EVT_UPDATE_UI(ID_SUMMARY_IMPORT_FORMAT, ImportSummaryWizardPage::OnUpdateImportFormat)
EVT_UPDATE_UI(ID_SUMMARY_IMPORT_FILENAME, ImportSummaryWizardPage::OnUpdateImportFilename)            
EVT_UPDATE_UI(ID_SUMMARY_DATABASE_NAME, ImportSummaryWizardPage::OnUpdateDatabaseName)
EVT_UPDATE_UI(ID_SUMMARY_SFM_FILENAME, ImportSummaryWizardPage::OnUpdateSFMImportFilename)
EVT_UPDATE_UI(ID_SUMMARY_E_CR_DB, ImportSummaryWizardPage::OnUpdateEmitCreateDatabase)
EVT_UPDATE_UI(ID_SUMMARY_E_SCH, ImportSummaryWizardPage::OnUpdateEmitSchema)
EVT_UPDATE_UI(ID_SUMMARY_OUTPUT_FILENAME, ImportSummaryWizardPage::OnUpdateOutputFilename)
EVT_UPDATE_UI(ID_SUMMARY_START_MONAD, ImportSummaryWizardPage::OnUpdateStartingMonad)
EVT_UPDATE_UI(ID_SUMMARY_START_IDD, ImportSummaryWizardPage::OnUpdateStartingIDD)
END_EVENT_TABLE()

ImportSummaryWizardPage::ImportSummaryWizardPage(wxWizard *parent)
:wxWizardPageSimple(parent)
{
	m_importFormat = kPlainText;	
	m_strEmitCreateDb = m_strEmitSchema = m_strDbName = 
		m_strSFMImportFileName = m_strOutputFileName =
		m_strStartingMonad = m_strStartingIDD = wxT("");
	CreateControls();
}

void ImportSummaryWizardPage::OnUpdateImportFormat(wxUpdateUIEvent& event)
{
	m_pFormatP = (ImportFormatWizardPage*)this->GetPrev()->GetPrev()->GetPrev()->GetPrev()->GetPrev();
	m_importFormat = m_pFormatP->GetImportFormat();
	event.SetText(mychoices[m_importFormat]);
	TransferDataFromWindow();
}

void ImportSummaryWizardPage::OnUpdateImportFilename(wxUpdateUIEvent& event)
{
	m_pFileP = (ImportFileNameWizardPage*)this->GetPrev()->GetPrev()->GetPrev()->GetPrev();
	m_strImportFileName = m_pFileP->GetImportFileName();
	std::string myStdString = joinList("\n", m_strImportFileName);
	wxString myWXString = wxString(myStdString.c_str(), wxConvUTF8);
	event.SetText(myWXString);
	TransferDataFromWindow();
}

void ImportSummaryWizardPage::OnUpdateDatabaseName(wxUpdateUIEvent& event)
{
	m_pDbPage = (ImportDatabaseWizardPage*)this->GetPrev()->GetPrev()->GetPrev();
	m_strDbName = m_pDbPage->GetDatabaseName();
	event.SetText(m_strDbName);
	TransferDataFromWindow();
}

void ImportSummaryWizardPage::OnUpdateSFMImportFilename(wxUpdateUIEvent& event)
{
	m_pDbPage = (ImportDatabaseWizardPage*)this->GetPrev()->GetPrev()->GetPrev();
	m_strSFMImportFileName = m_pDbPage->GetSFMImportFileName();
	if (m_strSFMImportFileName.IsEmpty())
		m_strSFMImportFileName = wxT("N/A");
	event.SetText(m_strSFMImportFileName);
	TransferDataFromWindow();
}

void ImportSummaryWizardPage::OnUpdateEmitCreateDatabase(wxUpdateUIEvent& event)
{
	m_pDbPage = (ImportDatabaseWizardPage*)this->GetPrev()->GetPrev()->GetPrev();
	if (m_pDbPage->GetEmitCreateDatabase()) {
		m_bEmitDb = true;
		m_strEmitCreateDb = wxT("yes");
	} else {
		m_bEmitDb = false;
		m_strEmitCreateDb = wxT("no");
	}
	event.SetText(m_strEmitCreateDb);
	TransferDataFromWindow();
}

void ImportSummaryWizardPage::OnUpdateEmitSchema(wxUpdateUIEvent& event)
{
	m_pDbPage = (ImportDatabaseWizardPage*)this->GetPrev()->GetPrev()->GetPrev();
	if (m_pDbPage->GetEmitSchema()) {
		m_bEmitSch = true;
		m_strEmitSchema = wxT("yes");
	} else {
		m_bEmitSch = false;
		m_strEmitSchema = wxT("no");
	}
	event.SetText(m_strEmitSchema);
	TransferDataFromWindow();
}

void ImportSummaryWizardPage::OnUpdateOutputFilename(wxUpdateUIEvent& event) 
{
	m_pOutputP = (ImportOutputFileWizardPage*)this->GetPrev()->GetPrev();
	m_strOutputFileName = m_pOutputP->GetOutputFileName();
	event.SetText(m_strOutputFileName);
	TransferDataFromWindow();
}

void ImportSummaryWizardPage::OnUpdateStartingMonad(wxUpdateUIEvent& event) 
{
	m_pAdvancedP = (ImportAdvancedWizardPage*)this->GetPrev();
	m_strStartingMonad = m_pAdvancedP->GetStartingMonad();
	event.SetText(m_strStartingMonad);
	TransferDataFromWindow();
}

void ImportSummaryWizardPage::OnUpdateStartingIDD(wxUpdateUIEvent& event) 
{
	m_pAdvancedP = (ImportAdvancedWizardPage*)this->GetPrev();
	m_strStartingIDD = m_pAdvancedP->GetStartingIDD();
	event.SetText(m_strStartingIDD);
	TransferDataFromWindow();
}

eImporter ImportSummaryWizardPage::GetImportFormat() const
{
	return m_importFormat;
}


std::list<std::string> ImportSummaryWizardPage::GetImportFileName() const
{ 
	return m_strImportFileName;
}

wxString ImportSummaryWizardPage::GetDatabaseName() const
{
	return m_strDbName;
}

wxString ImportSummaryWizardPage::GetSFMImportFileName() const
{
	return m_strSFMImportFileName;
}

wxString ImportSummaryWizardPage::GetOutputFileName() const
{
	return m_strOutputFileName;
}

wxString ImportSummaryWizardPage::GetStartingMonad() const
{
	return m_strStartingMonad;
}

wxString ImportSummaryWizardPage::GetStartingIDD() const 
{
	return m_strStartingIDD;
}

bool ImportSummaryWizardPage::GetEmitCreateDatabase() const 
{
	return m_bEmitDb;
}

bool ImportSummaryWizardPage::GetEmitSchema() const
{
	return m_bEmitSch;
}

void ImportSummaryWizardPage::CreateControls()
{
	wxSize sizeBroad = wxDefaultSize;
	sizeBroad.SetWidth(300);

	wxFlexGridSizer *pMainSizer = new wxFlexGridSizer(9, 2, 0, 0);
	this->SetSizer(pMainSizer);

	wxStaticText* itemStaticText1 = new wxStaticText( this, wxID_STATIC, wxT("Import format:"), wxDefaultPosition, wxDefaultSize, 0 );
	pMainSizer->Add(itemStaticText1, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	wxStaticText* itemStaticText2 = new wxStaticText( this, ID_SUMMARY_IMPORT_FORMAT, mychoices[GetImportFormat()], wxDefaultPosition, wxDefaultSize, 0 );
	pMainSizer->Add(itemStaticText2, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	wxStaticText* itemStaticText3 = new wxStaticText( this, wxID_STATIC, wxT("File(s) to import:"), wxDefaultPosition, wxDefaultSize, 0 );
	pMainSizer->Add(itemStaticText3, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	wxStaticText* itemStaticText4 = new wxStaticText( this, ID_SUMMARY_IMPORT_FILENAME, wxT("") , wxDefaultPosition, wxDefaultSize, 0 );
	pMainSizer->Add(itemStaticText4, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	wxStaticText* itemStaticText5 = new wxStaticText( this, wxID_STATIC, wxT("Create database:"), wxDefaultPosition, wxDefaultSize, 0 );
	pMainSizer->Add(itemStaticText5, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	wxStaticText* itemStaticText6 = new wxStaticText( this, ID_SUMMARY_E_CR_DB, m_strEmitCreateDb, wxDefaultPosition, wxDefaultSize, 0 );
	pMainSizer->Add(itemStaticText6, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	wxStaticText* itemStaticText7 = new wxStaticText( this, wxID_STATIC, wxT("Database name:"), wxDefaultPosition, wxDefaultSize, 0 );
	pMainSizer->Add(itemStaticText7, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	wxStaticText* itemStaticText8 = new wxStaticText( this, ID_SUMMARY_DATABASE_NAME, m_strDbName, wxDefaultPosition, wxDefaultSize, 0 );
	pMainSizer->Add(itemStaticText8, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	wxStaticText* itemStaticText9 = new wxStaticText( this, wxID_STATIC, wxT("Emit MQL Schema:"), wxDefaultPosition, wxDefaultSize, 0 );
	pMainSizer->Add(itemStaticText9, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	wxStaticText* itemStaticText10 = new wxStaticText( this, ID_SUMMARY_E_SCH, m_strEmitSchema, wxDefaultPosition, wxDefaultSize, 0 );
	pMainSizer->Add(itemStaticText10, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

	wxStaticText* itemStaticText11 = new wxStaticText( this, wxID_STATIC, wxT("SFM schema:"), wxDefaultPosition, wxDefaultSize, 0 );
	pMainSizer->Add(itemStaticText11, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	wxStaticText* itemStaticText12 = new wxStaticText( this, ID_SUMMARY_SFM_FILENAME, m_strSFMImportFileName, wxDefaultPosition, wxDefaultSize, 0 );
	pMainSizer->Add(itemStaticText12, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	wxStaticText* itemStaticText13 = new wxStaticText( this, wxID_STATIC, wxT("Output filename:"), wxDefaultPosition, wxDefaultSize, 0 );
	pMainSizer->Add(itemStaticText13, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	wxStaticText* itemStaticText14 = new wxStaticText( this, ID_SUMMARY_OUTPUT_FILENAME, GetOutputFileName(), wxDefaultPosition, wxDefaultSize, 0 );
	pMainSizer->Add(itemStaticText14, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	wxStaticText* itemStaticText15 = new wxStaticText( this, wxID_STATIC, wxT("Starting monad:"), wxDefaultPosition, wxDefaultSize, 0 );
	pMainSizer->Add(itemStaticText15, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	wxStaticText* itemStaticText16 = new wxStaticText( this, ID_SUMMARY_START_MONAD, GetStartingMonad(), wxDefaultPosition, wxDefaultSize, 0 );
	pMainSizer->Add(itemStaticText16, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	wxStaticText* itemStaticText17 = new wxStaticText( this, wxID_STATIC, wxT("Starting id_d:"), wxDefaultPosition, wxDefaultSize, 0 );
	pMainSizer->Add(itemStaticText17, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	wxStaticText* itemStaticText18 = new wxStaticText( this, ID_SUMMARY_START_IDD, GetStartingIDD(), wxDefaultPosition, wxDefaultSize, 0 );
	pMainSizer->Add(itemStaticText18, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	SetSizer(pMainSizer);
	pMainSizer->Fit(this);
}
