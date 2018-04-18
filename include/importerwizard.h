//
// importerwizard.h
// Martin Petersen
// Created: 10/11-2006
// Last update: 1/22-2009
//
//
/************************************************************************
 *
 *   Emdros - the database engine for analyzed or annotated text
 *   Copyright (C) 2006-2009  Ulrik Sandborg-Petersen
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




#ifndef IMPORTERWIZARD_H_
#define IMPORTERWIZARD_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "importerwizard.cpp"
#endif


/*!
 * Includes
 */

////@begin includes
#include <wx/wizard.h>
#include <wx/radiobox.h>
#include <wx/button.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/checkbox.h>
#include <wx/valgen.h>
#include <fstream>
#include "browsewidget.h"
#include "pennimporter.h"
#include <list>
#include <string>

////@end includes
enum eImporter{
	kPlainText,
	kSlashedText,
	kPennTreebank,
	kNeGRA,
	kTIGERXML,
	kSFM,
	kUnboundBible
};

#define ID_BUTTON_BROWSE_IMPORT_FILENAME   (10340)
#define ID_IMPORT_FORMAT_TEXTCTRL          (10341)
#define ID_IMPORT_DATABASE_TEXTCTRL        (10342)
#define ID_CHECKBOX_EMIT_DATABASE		   (10343)
#define ID_STATIC_TEXT_DATABASE_NAME       (10344)
#define ID_SMF_IMPORT_FORMAT               (10345)

//Id's for the summary page
#define ID_SUMMARY_IMPORT_FORMAT	       (10346)
#define ID_SUMMARY_IMPORT_FILENAME         (10347)
#define ID_SUMMARY_DATABASE_NAME           (10348)
#define ID_SUMMARY_SFM_FILENAME            (10349)
#define ID_SUMMARY_E_CR_DB                 (10350)
#define ID_SUMMARY_E_SCH                   (10351)
#define ID_SUMMARY_OUTPUT_FILENAME         (10352)
#define ID_SUMMARY_START_MONAD             (10353)
#define ID_SUMMARY_START_IDD               (10354)

/////////////////////////////////////////////////////////
// ImportFormatWizardPage
//////////////////////////////////////////////////////
class ImportFormatWizardPage : public wxWizardPageSimple {
 public:
	ImportFormatWizardPage(wxWizard *parent);
	virtual ~ImportFormatWizardPage(){};
	eImporter GetImportFormat() const;
 private:
	wxComboBox *m_pComboBox;
};

/////////////////////////////////////////////////////////
// ImportFileNameWizardPage
////////////////////////////////////////////////////////
class ImportFileNameWizardPage : public wxWizardPageSimple {
 public:
	ImportFileNameWizardPage(wxWizard* parent);
	virtual ~ImportFileNameWizardPage(){};
	std::list<std::string> GetImportFileName();
	void OnBrowseFile(wxCommandEvent& event);
	void OnPageChanged(wxWizardEvent& event);
	void OnPageChanging(wxWizardEvent& event);
 private:
	void CreateControls();
	bool m_bChooseMultiple;
	wxTextCtrl *m_textCtrl;
	wxString m_strValue;
	DECLARE_EVENT_TABLE()
};

/////////////////////////////////////////////////////////
// ImportDatabaseWizardPage
/////////////////////////////////////////////////////////
class ImportDatabaseWizardPage : public wxWizardPageSimple {
 public:
	ImportDatabaseWizardPage(wxWizard *parent);
	virtual ~ImportDatabaseWizardPage(){};
	wxString GetDatabaseName() const;
	wxString GetSFMImportFileName() const;
	void OnPageChanging(wxWizardEvent& event);
	void OnSMFImportChosen(wxUpdateUIEvent& event);
	bool GetEmitCreateDatabase() const;
	bool GetEmitSchema() const;
 private:
	void CreateControls();
	wxCheckBox *m_pDbCheckbox, *m_pSchemaCheckbox;
	wxTextCtrl *m_textCtrl, *m_pSFMTextCtrl;
	wxString m_strValue;
	bool m_bEmitDatabase, m_bEmitSchema;
	BrowseWidget *m_pBrowseWidget;
	DECLARE_EVENT_TABLE()
};

/////////////////////////////////////////////////////////
// ImportOutputFileWizardPage
/////////////////////////////////////////////////////////
class ImportOutputFileWizardPage : public wxWizardPageSimple {
 public:
	ImportOutputFileWizardPage(wxWizard *parent);
	virtual ~ImportOutputFileWizardPage(){};
	wxString GetOutputFileName();
	void OnPageChanging(wxWizardEvent& event);
 private:
	void CreateControls();
	wxString m_strOutputFileName;
	BrowseWidget *m_pBrowseWidget;
	DECLARE_EVENT_TABLE()
};

/////////////////////////////////////////////////////////
// ImportAdvancedWizardPage
/////////////////////////////////////////////////////////
class ImportAdvancedWizardPage : public wxWizardPageSimple {
 public:
	ImportAdvancedWizardPage(wxWizard *parent);
	virtual ~ImportAdvancedWizardPage(){};
	wxString GetStartingMonad() const;
	wxString GetStartingIDD() const;
 private:
	void CreateControls();
	wxTextCtrl *m_pMonadTextCtrl, *m_pIdDTextCtrl;
	wxString m_starting_monad, m_starting_id_d;
};

/////////////////////////////////////////////////////////
// ImportSummaryWizardPage
/////////////////////////////////////////////////////////
class ImportSummaryWizardPage : public wxWizardPageSimple {
 public:
	ImportSummaryWizardPage(wxWizard *parent);
	virtual ~ImportSummaryWizardPage(){};
	eImporter GetImportFormat() const;
	std::list<std::string> GetImportFileName() const;
	wxString GetDatabaseName() const;
	wxString GetSFMImportFileName() const;
	bool GetEmitCreateDatabase()const;
	bool GetEmitSchema() const;
	wxString GetOutputFileName()const;
	wxString GetStartingMonad()const;
	wxString GetStartingIDD()const;
		
	void OnUpdateImportFormat(wxUpdateUIEvent& event);
	void OnUpdateImportFilename(wxUpdateUIEvent& event);
	void OnUpdateDatabaseName(wxUpdateUIEvent& event);
	void OnUpdateSFMImportFilename(wxUpdateUIEvent& event);
	void OnUpdateEmitCreateDatabase(wxUpdateUIEvent& event);
	void OnUpdateEmitSchema(wxUpdateUIEvent& event);
	void OnUpdateOutputFilename(wxUpdateUIEvent& event);
	void OnUpdateStartingMonad(wxUpdateUIEvent& event);
	void OnUpdateStartingIDD(wxUpdateUIEvent& event);
 private:
	void CreateControls();
	ImportFormatWizardPage *m_pFormatP;
	ImportFileNameWizardPage *m_pFileP;
	ImportDatabaseWizardPage *m_pDbPage;
	ImportOutputFileWizardPage *m_pOutputP;
	ImportAdvancedWizardPage *m_pAdvancedP;
	wxString m_strEmitCreateDb, m_strEmitSchema, m_strDbName, 
		m_strSFMImportFileName, m_strOutputFileName,
		m_strStartingMonad, m_strStartingIDD;
	std::list<std::string> m_strImportFileName;
	bool m_bEmitDb, m_bEmitSch;
	eImporter m_importFormat;
	DECLARE_EVENT_TABLE()
};

/////////////////////////////////////////////////////////
// ImporterWizard
/////////////////////////////////////////////////////////
class ImporterWizard : public wxWizard {
 public:
	ImporterWizard(wxWindow* parent, int id = -1,
		       const wxString& title = wxEmptyString, 
		       const wxBitmap& bitmap = wxNullBitmap, 
		       const wxPoint& pos = wxDefaultPosition, 
		       long style = wxDEFAULT_DIALOG_STYLE);
	virtual ~ImporterWizard();
	bool RunIt(void);
 private:
	DECLARE_EVENT_TABLE()
	bool openFile(std::ofstream& fout, const std::string& filename);
 protected:
	/// Creates the controls and sizers
	void CreateControls();
	bool Process();
	EmdrosImporterBase* GetImporter();
	wxWizardPageSimple *m_pPage1;
	ImportFormatWizardPage *m_pFormatPage;
	ImportFileNameWizardPage *m_pFilePage;
	ImportDatabaseWizardPage *m_pDatabasePage;
	ImportOutputFileWizardPage *m_pOutputPage;
	ImportAdvancedWizardPage *m_pAdvancedPage;
	ImportSummaryWizardPage *m_pSummaryPage;
};

#endif
// IMPORTERWIZARD_H_
