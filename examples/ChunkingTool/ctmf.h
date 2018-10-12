/*
 * ctmf.h
 *
 * main frame in ctwx.
 * ctwx is a graphical front-end for Chunking text in Emdros databases.
 *
 * Ulrik Petersen
 * Created: 4/13-2005
 * Last update: 10/4-2007
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



#ifndef _CTMF_H_
#define _CTMF_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "ctmf.cpp"
#endif



/*!
 * Includes
 */

////@begin includes
#include "TECkit/TECkit_Compiler.h"
#include "TECkit/TECkit_Engine.h"

#include "wx/frame.h"
#include "wx/toolbar.h"
#include "wx/string.h"
#include "wx/statusbr.h"
#include "wx/treectrl.h"
#include "wx/wxhtml.h"
#include <wx/wizard.h>
#include <emdros_environment.h>
#include <string>
#include <map>
#include <conf.h>
#include <layout.h>
#include <schema.h>
#include <configwizard.h>
#include "ctdoc.h"

////@end includes

typedef class ConnectionData {
public:
	eBackendKind m_backend_kind;
	wxString m_strConfiguration;
	wxString m_strHost;
	wxString m_strDatabase;
	wxString m_strUser;
	wxString m_strPassword;
	ConnectionData() { m_backend_kind = DEFAULT_BACKEND_ENUM; };
} ConnectionData;

typedef std::vector<TECkit_Converter> FeatureTECkitTableVec;

/*!
 * Forward declarations
 */

////@begin forward declarations
class MainFrame; 
////@end forward declarations

class ChunkingToolConfigWizard : public wxWizard {
public:
        ChunkingToolConfigWizard(wxWindow* parent,
				 wxString strInitialConfigFile,
				 int id = -1,
				 const wxString& title = wxEmptyString,
				 const wxBitmap& bitmap = wxNullBitmap,
				 const wxPoint& pos = wxDefaultPosition,
				 long style = wxDEFAULT_DIALOG_STYLE);
        ~ChunkingToolConfigWizard();
        bool RunIt(void);
	void OnPageChanged(wxWizardEvent& event);

protected:
        /// Creates the controls and sizers
        void CreateControls(wxString strInitialConfigFile);
        wxWizardPageSimple *m_pCreateOrEditPage;
	GetConfigurationFilenameWizardPage *m_pFileNamePage;
};



class WXILLayoutCanvas : public LayoutCanvas {
 protected:
        bool m_bIsRightToLeft;
        unsigned int m_nInterlinearLines;
	MainFrame *m_pFrame;
	FeatureTECkitTableVec m_teckit_vec;
	bool m_bHasTECkitTables;
	wxstringvec_t m_strings_for_addition;
 public:
        WXILLayoutCanvas(MainFrame *pParent, const wxPoint& pos, const wxSize& size, const long style, Configuration *pConf);
        virtual ~WXILLayoutCanvas();
        virtual void InitializeViewMetrics(Configuration *pConf);
	void AddPara(indent_t indent, bool bIsCentered);
	LayoutBox *AddEmdrosObject(wxDC *pDC, const EmdrosObject* pObj);
	wxClientDC *GetClientDC(void) { return new wxClientDC(this); };
	virtual void HandleButtonDown(wxMouseEvent& event, wxPoint point, bool RightButton);
	bool isRightToLeft() const { return m_bIsRightToLeft; };
	std::string applyTECkit(unsigned int feature_index, const std::string& feature_value);
 protected:
	typedef std::map<monad_m, LayoutBox*> Monad2LayoutBoxMap;
	Monad2LayoutBoxMap m_box_map;
	void ClearLayoutBoxMap();
	bool compileTECkits(Configuration *pConf, std::string& message);
	
};




/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_FRAME 10000
#define CHUNK_OT_NAME "Chunk"
#define SYMBOL_MAINFRAME_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxMINIMIZE_BOX|wxMAXIMIZE_BOX
#define SYMBOL_MAINFRAME_TITLE _("Emdros Chunking Tool")
#define SYMBOL_MAINFRAME_IDNAME ID_FRAME
#define SYMBOL_MAINFRAME_SIZE wxSize(620, 450)
#define SYMBOL_MAINFRAME_POSITION wxDefaultPosition
#define ID_MENU_FILE_CONNECT 10005
#define ID_MENU_FILE_COMMIT_TO_DB 10006
#define ID_MENU_EDIT_SPLIT 10010
#define ID_MENU_EDIT_COMBINE 10013
#define ID_MENU_EDIT_MOVE_LEFT 10011
#define ID_MENU_EDIT_MOVE_RIGHT 10012
#define ID_MENU_HELP_ABOUT 10008
#define ID_MENU_HELP_HELP_CONTENTS 11235
#define ID_MENU_HELP_MQL_CHEAT_SHEET 11236
#define ID_MENU_FILE_SAVE_OUTPUT_AS 11237
#define ID_MENU_TOOLS_CONFIGURE 11238
#define ID_TOOLBAR 10002
#define ID_STATUSBAR 10003
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

/*!
 * MainFrame class declaration
 */

class MainFrame: public wxFrame
{    
	DECLARE_CLASS( MainFrame )
	DECLARE_EVENT_TABLE()

public:
	/// Constructors
	MainFrame( );
	MainFrame( wxWindow* parent, 
		   const ConnectionData& conndata,
		   wxWindowID id = SYMBOL_MAINFRAME_IDNAME, 
		   const wxString& caption = SYMBOL_MAINFRAME_TITLE, 
		   const wxPoint& pos = SYMBOL_MAINFRAME_POSITION, 
		   const wxSize& size = SYMBOL_MAINFRAME_SIZE, 
		   long style = SYMBOL_MAINFRAME_STYLE );
	~MainFrame();

	bool Create( wxWindow* parent, wxWindowID id = SYMBOL_MAINFRAME_IDNAME, const wxString& caption = SYMBOL_MAINFRAME_TITLE, const wxPoint& pos = SYMBOL_MAINFRAME_POSITION, const wxSize& size = SYMBOL_MAINFRAME_SIZE, long style = SYMBOL_MAINFRAME_STYLE );

	/// Creates the controls and sizers
	void CreateControls();

	////@begin MainFrame event handler declarations
	void OnAbout(wxCommandEvent& event);
	void OnMaximize(wxMaximizeEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnToolsConfigure(wxCommandEvent& event);
	void OnHelpContents(wxCommandEvent& event);
	void OnHelpMQLCheatSheet(wxCommandEvent& event);
	void OnFileConnect(wxCommandEvent& event);
	void OnFileCommitToDB(wxCommandEvent& event);
	void OnFileExit(wxCommandEvent& event);
	void OnEditSplit(wxCommandEvent& event);
	void OnEditCombine(wxCommandEvent& event);
	void OnEditMoveLeft(wxCommandEvent& event);
	void OnEditMoveRight(wxCommandEvent& event);
	////@end MainFrame event handler declarations

	////@begin MainFrame member function declarations

	/// Retrieves bitmap resources
	wxBitmap GetBitmapResource( const wxString& name );

	/// Retrieves icon resources
	wxIcon GetIconResource( const wxString& name );
    
	bool Connect();
	bool LoadDocument();
	void UpdateChunkingArea();
	bool DoPreChunking();
	bool LoadObjects(objectlist_t& objects);
	bool LoadChunks(chunklist_t& chunks);
	bool ObjectTypeExists(std::string object_type_name, bool& bExists);
	bool CreateChunkType(std::string chunk_ot_name);
	bool ExecuteEmdrosStatement(std::string query);

	////@end MainFrame member function declarations

	/// Should we show tooltips?
	static bool ShowToolTips();

	////@begin MainFrame member variables
	WXILLayoutCanvas* m_pChunkingArea;
	wxString m_strCurFileName;
	ConnectionData m_connectionData;
	EmdrosEnv *m_pEE;
	Configuration *m_pConf;
	wxToolBar* m_ctrlToolBar;
	CTDoc* m_pDoc;
	monad_m m_cur_selected_monad;
	LayoutBoxBase *m_cur_selected_box;
	////@end MainFrame member variables
};

#endif
// _CTMF_H_
