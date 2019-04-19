//
// importerwizard.h
// Martin Petersen
// Created: 10/11-2006
// Last update: 4/19-2019
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
	virtual std::list<std::string> GetImportFileName();
	virtual void OnBrowseFile(wxCommandEvent& event);
	virtual void OnPageChanged(wxWizardEvent& event);
	virtual void OnPageChanging(wxWizardEvent& event);
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
	wxTextCtrl *m_textCtrl;
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
