/*
 * qtconnwizard.h
 *
 * The complex connection dialog in mqlqtwx.
 * mqlqtwx is a graphical front-end for querying Emdros databases.
 *
 * Ulrik Petersen
 * Created: 4/13-2005
 * Last update: 4/27-2007
 *
 */

/*
 * Copyright (C) 2001-2018     Ulrik Sandborg-Petersen
 * Copyright (C) 2018-present  Sandborg-Petersen Holding ApS, Denmark
 *
 * Licensed under the MIT License.
 *
 * Please see the file COPYING in the root of the sources for more details.
 *
 */



#ifndef _QTCONNWIZARD_H_
#define _QTCONNWIZARD_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "qtconnwizard.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include <wx/wizard.h>
#include <wx/checkbox.h>
#include <wx/textctrl.h>
#include <wx/spinctrl.h>
#include <leftrightchooser.h>
#include <wxorder_chooser.h>
#include <dropdown_widget.h>
#include <conndlg.h>
#include <schema.h>
#include <configwizard.h>
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_QTCONNWIZARD 10300
#define SYMBOL_QTCONNWIZARD_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_QTCONNWIZARD_TITLE _("Create connection configuration...")
#define SYMBOL_QTCONNWIZARD_POSITION wxDefaultPosition
#define ID_PAGE_1 10301
#define ID_LEFTRIGHT_CHOOSER1 10302
#define ID_LEFT_BOUNDARY_DROPDOWN 10500
#define ID_RIGHT_BOUNDARY_DROPDOWN 10501
#define ID_DATA_UNIT_COMBOBOX 10502
#define ID_USE_TECKIT_CHECKBOX 10503
#define ID_TECKIT_PATH_BROWSE_WIDGET 10504
#define ID_TECKIT_INPUT_DROPDOWN_WIDGET 10505
#define ID_TECKIT_OUTPUT_DROPDOWN_WIDGET 10506
#define ID_FONT_BUTTON1 10507
#define ID_FONT_BUTTON2 10508
#define ID_FONT_BUTTON3 10509
#define ID_FONT_BUTTON4 10510
#define ID_OUTPUT_AREA_OPTIONS_COMBOBOX 10511
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif
#ifndef wxFIXED_MINSIZE
#define wxFIXED_MINSIZE 0
#endif

class FirstPage : public wxWizardPageSimple
{
 protected:
	LeftRightChooser *m_pLeftRightChooser;
 public:
	FirstPage(wxWizard *pParent, const wxBitmap& bitmap = wxNullBitmap);
	virtual ~FirstPage();
	void CreateControls();
};

class RasterUnitWizardPage : public wxWizardPageSimple
{
 protected:
	bool m_bDoesNotWantZero;
	wxString m_strMessage;
	wxString m_strRightLabel;
	LeftRightChooser *m_pLeftRightChooser;
	DECLARE_EVENT_TABLE()
 public:
	RasterUnitWizardPage(wxWizard *pParent, bool bDoesNotWantZero = true, wxString strMessage = wxT(""), wxString strRightLabel = wxT(""), const wxBitmap& bitmap = wxNullBitmap);
	virtual ~RasterUnitWizardPage();
	void OnPageChanged(wxWizardEvent& event);
	void OnPageChanging(wxWizardEvent& event);
	wxString getUnit();
 protected:
	void CreateControls();
	virtual void CreateControlsSecondary(wxBoxSizer *pMainSizer);
};

class DataUnitsWizardPage : public wxWizardPageSimple
{
  protected:
       LeftRightChooser *m_pLeftRightChooser;
       DECLARE_EVENT_TABLE()
  public:
       DataUnitsWizardPage(wxWizard *pParent, const wxBitmap& bitmap = wxNullBitmap);
       virtual ~DataUnitsWizardPage();
       void OnPageChanged(wxWizardEvent& event);
       void OnPageChanging(wxWizardEvent& event);
       WXStringVec getDataUnits();
 protected:
       void CreateControls();
};


class DataUnitOrderWizardPage : public GetOrderOfStringVecPage
{
 protected:
	typedef std::map<wxString, std::pair<std::string, std::string> > OTN2StringPairMap;
	OTN2StringPairMap m_boundaries;
	DropdownWidget *m_pLeftDropdown;
	DropdownWidget *m_pRightDropdown;
	wxString m_strCurrentOTN;
	DECLARE_EVENT_TABLE()
 public:
	DataUnitOrderWizardPage(wxWizard *pParent, const wxBitmap& bitmap = wxNullBitmap);
	virtual ~DataUnitOrderWizardPage();
	virtual void OnSelectionChanged();
 protected:
	void CreateControls();
	virtual void OnPageChangedForward();
	void OnUpdateDropdown(wxUpdateUIEvent& event);
	void saveBoundaries();
	void setBoundaryDropdownsFromOTN();
	void setBoundaries(wxString newOTN);
};

class DataUnitFeaturesWizardPage : public wxWizardPageSimple
{
  protected:
       LeftRightChooser *m_pLeftRightChooser;
       DECLARE_EVENT_TABLE()
  public:
       DataUnitFeaturesWizardPage(wxWizard *pParent, const wxBitmap& bitmap = wxNullBitmap);
       virtual ~DataUnitFeaturesWizardPage();
       void OnPageChanged(wxWizardEvent& event);
       void OnPageChanging(wxWizardEvent& event);
       WXStringVec getDataUnitFeatures(); // Returns it as a vector of data_unit + "." + feature-name
       int getNoOfInterlinearLines();
 protected:
       void CreateControls();
};

class FeatureTECKitData {
 public:
	FeatureTECKitData() {
		m_bUseTECKit = false;
		m_strTECKitPath = wxString(wxT(""));
		m_bInputIsUnicode = false;
		m_bOutputIsUnicode = true;
	};
	FeatureTECKitData(bool bUseTECKit,
			  wxString strPath,
			  bool bInputIsUnicode,
			  bool bOutputIsUnicode) {
		m_bUseTECKit = bUseTECKit;
		m_strTECKitPath = strPath;
		m_bInputIsUnicode = bInputIsUnicode;
		m_bOutputIsUnicode = bOutputIsUnicode;
	};
	~FeatureTECKitData() {};
	bool m_bUseTECKit;
	wxString m_strTECKitPath;
	bool m_bInputIsUnicode;
	bool m_bOutputIsUnicode;
};


class DUFOOrderChooserWidget; // Forward declaration
class DUFODropdownWidget; // Forward declaration
class DataUnitFeatureOrderWizardPage : public wxWizardPageSimple
{
 protected:
	typedef std::map<wxString, WXStringVec> OTN2WXStringVecMap;
	typedef std::map<wxString, FeatureTECKitData> OTNDotFeature2TECKitDataMap;
	OTN2WXStringVecMap m_features;
	OTNDotFeature2TECKitDataMap m_TECKitData;
	DUFODropdownWidget *m_pOTNDropdown;
	DUFOOrderChooserWidget *m_pOCW;
	wxCheckBox *m_pUseTECKit;
	bool m_bUseTECKit;
	BrowseWidget *m_pBrowseTECKitPath;
	DropdownWidget *m_pInputDropdown;
	DropdownWidget *m_pOutputDropdown;
	wxString m_strCurrentOTN;
	wxString m_strCurrentDuDotFN;
	DECLARE_EVENT_TABLE()
 public:
	DataUnitFeatureOrderWizardPage(wxWizard *pParent, const wxBitmap& bitmap = wxNullBitmap);
	virtual ~DataUnitFeatureOrderWizardPage();
	void OnPageChanged(wxWizardEvent& event);
	void OnOTNSelectionChanged();
	void OnFeatureSelectionChanged(void);
	void OnUpdateUseTECKitCheckbox(wxUpdateUIEvent& event);
	void OnUpdateUseTECKit(wxUpdateUIEvent& event);
 protected:
	void CreateControls();
	virtual void OnPageChangedForward();
	wxString getOTNSelection(void) const;
	wxString getFeatureSelection(void) const;
	void saveFeatures();
	void setFeatures(wxString newOTN);
	void setFeaturesFromOTN();
	void saveTECKit();
	void setTECKit(wxString newDuDotFeaturName);
	void setTECKitFromDuDotFN();
};


class DUFOOrderChooserWidget : public OrderChooserWidget
{
 public:
	DUFOOrderChooserWidget(DataUnitFeatureOrderWizardPage *pParent)
		: OrderChooserWidget(pParent) {};
	virtual ~DUFOOrderChooserWidget() {};
 protected:
	virtual void SelectionChanged(void) {
		((DataUnitFeatureOrderWizardPage*)m_pParent)->OnFeatureSelectionChanged();
	};
};



class DUFODropdownWidget : public DropdownWidget
{
 public:
	DUFODropdownWidget(DataUnitFeatureOrderWizardPage *pParent,
			   int myid,
			   bool bIsVertical,
			   wxString strLabel)
		: DropdownWidget(pParent, myid, bIsVertical, strLabel) {};
	virtual ~DUFODropdownWidget() {};
 protected:
	virtual void SelectionChanged(void) {
		((DataUnitFeatureOrderWizardPage*)m_pParent)->OnOTNSelectionChanged();
	};
};

class ReferenceUnitWizardPage : public RasterUnitWizardPage
{
 public:
	ReferenceUnitWizardPage(wxWizard *pParent, const wxBitmap& bitmap = wxNullBitmap);
	virtual ~ReferenceUnitWizardPage();
};


class ReferenceFeaturesWizardPage : public wxWizardPageSimple
{
 protected:
	bool m_bHasReferenceUnit;
	LeftRightChooser *m_pLeftRightChooser;
	wxStaticText *m_pMessage;
	static const wxString m_strMsgHasReferenceUnit;
	static const wxString m_strMsgHasNoReferenceUnit;
	DECLARE_EVENT_TABLE()
	    
 public:
       ReferenceFeaturesWizardPage(wxWizard *pParent, const wxBitmap& bitmap = wxNullBitmap);
       virtual ~ReferenceFeaturesWizardPage();
       void OnPageChanged(wxWizardEvent& event);
       void OnPageChanging(wxWizardEvent& event);
       void OnUpdateLeftRightChooser(wxUpdateUIEvent& event);
       WXStringVec getReferenceFeatures();
 protected:
       void CreateControls();
};


class ReferenceFeatureOrderWizardPage : public GetOrderOfStringVecPage
{
 protected:
	bool m_bHasReferenceUnit;
	typedef std::map<wxString, wxString > RUDotFN2SeparatorMap;
	RUDotFN2SeparatorMap m_separators;
	DropdownWidget *m_pAfterDropdown;
	wxString m_strCurrentDuDotFN;
	static const wxString m_strMsgHasReferenceUnit;
	static const wxString m_strMsgHasNoReferenceUnit;
	DECLARE_EVENT_TABLE()
 public:
	ReferenceFeatureOrderWizardPage(wxWizard *pParent, const wxBitmap& bitmap = wxNullBitmap);
	virtual ~ReferenceFeatureOrderWizardPage();
	virtual void OnSelectionChanged();
 protected:
	void CreateControls();
	virtual void OnPageChangedForward();
	void OnUpdateDropdown(wxUpdateUIEvent& event);
	void saveSeparators();
	void setSeparatorDropdownFromDuDotFN();
	void setSeparators(wxString newDuDotFN);
	virtual bool getUpdateUIEnable();
};

class GeneralOptionsWizardPage : public wxWizardPageSimple
{
 protected:
	wxCheckBox *m_pCBApplyFocus;
	wxCheckBox *m_pCBBreakAfterRaster;
	wxCheckBox *m_pCBSingleRasterUnits;
	wxRadioBox *m_pRBLTR_RTL;
 public:
	GeneralOptionsWizardPage(wxWizard *pParent, const wxBitmap& bitmap = wxNullBitmap);
	bool getApplyFocus();
	bool getBreakAfterRaster();
	bool getSingleRasterUnits();
	bool getIsRTL();
	virtual ~GeneralOptionsWizardPage();
	void CreateControls();
};



class InputAreaOptionsWizardPage : public wxWizardPageSimple
{
 protected:
	wxTextCtrl *m_pTCFontName;
	wxSpinCtrl *m_pSCFontSize;
	DECLARE_EVENT_TABLE()
 public:
	InputAreaOptionsWizardPage(wxWizard *pParent, const wxBitmap& bitmap = wxNullBitmap);
	virtual ~InputAreaOptionsWizardPage();
	wxString getFontName();
	int getFontSize();
	void OnBrowseFont(wxCommandEvent& event);
	void CreateControls();
};


class OAODropdownWidget; // Forward declaration
class OutputAreaOptionsWizardPage : public wxWizardPageSimple
{
 protected:
	OAODropdownWidget *m_pInterlinearLineDropdownWidget;
	int m_nCurrentInterlinearLine;
	typedef std::map<int, wxString> Int2FontNameMap;
	Int2FontNameMap m_font_names;
	wxTextCtrl *m_pTCFontName;
	DECLARE_EVENT_TABLE()
 public:
	OutputAreaOptionsWizardPage(wxWizard *pParent, const wxBitmap& bitmap = wxNullBitmap);
	virtual ~OutputAreaOptionsWizardPage();
	virtual void OnSelectionChanged();
	void OnUpdateUI(wxUpdateUIEvent& event);
	void OnPageChanged(wxWizardEvent& event);
	void OnBrowseFont(wxCommandEvent& event);
	WXStringVec getFontNames() const; // 0-based!
 protected:
	void CreateControls();
	virtual void OnPageChangedForward();
	void saveFont();
	void setFont(int nNewInterlinearLine);
	wxString getFontName();
};

class OAODropdownWidget : public DropdownWidget
{
 public:
	OAODropdownWidget(OutputAreaOptionsWizardPage *pParent,
			   int myid,
			   bool bIsVertical,
			   wxString strLabel)
		: DropdownWidget(pParent, myid, bIsVertical, strLabel) {};
	virtual ~OAODropdownWidget() {};
 protected:
	virtual void SelectionChanged(void) {
		((OutputAreaOptionsWizardPage*)m_pParent)->OnSelectionChanged();
	};
};





class ConnectionWizard: public wxWizard
{    
	DECLARE_EVENT_TABLE()
protected:
	FirstPage *m_pPage1;
	CreateEditConfigurationWizardPage *m_pCreateEditConfigurationPage;
	GetConfigurationFilenameWizardPage *m_pGetConfigurationFilenamePage;
	DBConnectionWizardPage *m_pConnectionPage;
	ConnectionStatusWizardPage *m_pConnectionStatusPage;
	RasterUnitWizardPage *m_pRasterUnitPage;
	DataUnitsWizardPage *m_pDataUnitsPage;
	DataUnitOrderWizardPage *m_pDataUnitOrderPage;
	DataUnitFeaturesWizardPage *m_pDataUnitFeaturesPage;
	DataUnitFeatureOrderWizardPage *m_pDataUnitFeatureOrderPage;
	ReferenceUnitWizardPage *m_pReferenceUnitPage;
	ReferenceFeaturesWizardPage *m_pReferenceFeaturesPage;
	ReferenceFeatureOrderWizardPage *m_pReferenceFeatureOrderPage;
	OutputAreaOptionsWizardPage *m_pOutputAreaOptionsWizardPage;
	GeneralOptionsWizardPage *m_pGeneralOptionsWizardPage;
	InputAreaOptionsWizardPage *m_pInputAreaOptionsWizardPage;
public:
	/// Constructors
	ConnectionWizard( wxWindow* parent, wxWindowID id = ID_QTCONNWIZARD, const wxString& caption = SYMBOL_QTCONNWIZARD_TITLE, const wxBitmap& = wxNullBitmap, const wxPoint& pos = SYMBOL_QTCONNWIZARD_POSITION, long style = SYMBOL_QTCONNWIZARD_STYLE );
	virtual ~ConnectionWizard();

	EmdrosSchema *GetSchema(void);

	WXStringVec getDataUnits();

	WXStringVec getDataUnitFeatures();

	std::map<std::string, WXStringVec > getDataUnitFeatureMap();

	wxString getReferenceUnit();

	bool hasReferenceUnit();

	WXStringVec getReferenceFeatures();

	int getNoOfInterlinearLines();



	eCreateOrEditConfiguration getCreateOrEdit();

	/// Creates the controls and sizers
	void CreateControls();

	////@begin ConnectionWizard event handler declarations
	//void OnBrowseConfiguration(wxCommandEvent& event);
	////@end ConnectionWizard event handler declarations

	////@begin ConnectionWizard member function declarations	
	bool RunIt();

	/// Retrieves bitmap resources
	wxBitmap GetBitmapResource( const wxString& name );

	/// Retrieves icon resources
	wxIcon GetIconResource( const wxString& name );
	////@end ConnectionWizard member function declarations

	/// Should we show tooltips?
	static bool ShowToolTips();

	////@begin ConnectionWizard member variables
	////@end ConnectionWizard member variables
};


#endif
// _QTCONNWIZARD_H_
