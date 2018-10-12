/*
 * ctmf.cpp
 *
 * Main frame in ctwx.  
 * 
 * ctwx is a graphical front-end for chunking and indenting text in an
 * Emdros database.
 *
 * Ulrik Petersen
 * Created: 4/13-2005
 * Last update: 4/10-2017
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




#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "ctmf.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

////@begin includes
// #include <wx/stdpaths.h> // Not available in 2.4.2
#include <wx/utils.h> // Instead of stdpaths
#include <wx/filename.h>
#include <wx/filedlg.h>
#include <wx/strconv.h>
#include <wx/fs_zip.h>
#include <wx/clipbrd.h>
#include <fstream>
#include <sstream>
#include <wxutil_emdros.h>
#include <prefix_emdros.h>
#include <conndlg.h>
#include "ctmf.h"
#include "ctwx.h"
#include <monads.h>
#include <viewmetrics.h>
#include <mql_execution_environment.h>
#include <emdf_value.h>

////@end includes



////@begin XPM images
#include <commit.xpm>
#include <connect.xpm>
#include <flash.xpm>
#include <leftarrow.xpm>
#include <rightarrow.xpm>
#include <together.xpm>
////@end XPM images


#define min(X,Y)  ((X < Y) ? X : Y)


////////////////////////////////////////////////////////
//
// ChunkingToolConfigWizard
//
////////////////////////////////////////////////////////


ChunkingToolConfigWizard::ChunkingToolConfigWizard(wxWindow* parent,
						   wxString strInitialConfigFile,
						   int id,
						   const wxString& title,
						   const wxBitmap& bitmap,
						   const wxPoint& pos,
						   long style)
        : wxWizard(parent, id, title, bitmap, pos, style)
{
        CreateControls(strInitialConfigFile);
}

ChunkingToolConfigWizard::~ChunkingToolConfigWizard()
{
}

void ChunkingToolConfigWizard::CreateControls(wxString strInitialConfigFile)
{
	eCreateOrEditConfiguration create_or_edit = kCreateConfiguration;
	if (strInitialConfigFile.IsEmpty()) {
		create_or_edit = kCreateConfiguration;
	} else {
		create_or_edit = kEditConfiguration;
	}

        m_pCreateOrEditPage = 
		new CreateEditConfigurationWizardPage(this,
						      create_or_edit);

        m_pFileNamePage = 
		new GetConfigurationFilenameWizardPage(this,
						       strInitialConfigFile);

        wxWizardPageSimple::Chain(m_pCreateOrEditPage, m_pFileNamePage);

        GetPageAreaSizer()->Add(m_pCreateOrEditPage);
}


bool ChunkingToolConfigWizard::RunIt(void)
{
        bool bResult;
        if (RunWizard(m_pCreateOrEditPage)) {
                bResult = true;
        } else {
                bResult = false;
        }
        Close(true);
        return bResult;
}






////////////////////////////////////////////////////////
//
// WXILLayoutCanvas
//
////////////////////////////////////////////////////////

#define NO_OF_COLORS (5)

wxColour mycolorarray[NO_OF_COLORS] = {
	BLACK,
	DARK_BLUE,
	RED,
	BLUE,
	CYAN
	};

WXILLayoutCanvas::WXILLayoutCanvas(MainFrame *pParent, const wxPoint& pos, const wxSize& size, const long style, Configuration *pConf)
	: LayoutCanvas(pParent, pos, size, style), m_pFrame(pParent)
{
	InitializeViewMetrics(pConf);
}


WXILLayoutCanvas::~WXILLayoutCanvas()
{
	ClearLayoutBoxMap();
}


void WXILLayoutCanvas::ClearLayoutBoxMap()
{
	Monad2LayoutBoxMap::iterator it = m_box_map.begin();
	Monad2LayoutBoxMap::iterator itend = m_box_map.end();
	while (it != itend) {
		delete it->second;
		++it;
	}

	m_box_map.clear();
}

void WXILLayoutCanvas::HandleButtonDown(wxMouseEvent& event, wxPoint point, bool RightButton)
{
	UNUSED(event);
	UNUSED(RightButton);
	
	// the wxPoint point is already in logical coordinates, thanks
	// to LayoutCanvas::OnMouseEvent calling
	// CalcUnscrolledPosition.
	wxCoord lpX = point.x;
	wxCoord lpY = point.y;

	wxPoint pointLP(lpX, lpY);


	LayoutBoxBase *pBox = GetBoxForPoint(pointLP);

	if (pBox != 0) {
		pBox->SetSelected(true); 

		// Deselect whatever was selected
		if (m_pFrame->m_cur_selected_box != 0) {
			m_pFrame->m_cur_selected_box->SetSelected(false);
		}
		m_pFrame->m_cur_selected_box = pBox;
		m_pFrame->m_cur_selected_monad = pBox->GetLong();
		Refresh();
	} else {
		// FIXME: Say something!
	}
}


wxString getOutputAreaFontName(Configuration *pConf, unsigned int number)
{
	wxString outputAreaFontName;
	std::string key = "chunking_area_font_name";
	if (pConf != 0 && pConf->has_key(key)) {
		StringVec chunking_area_font_names = pConf->getValues(key);
		std::string chunking_area_font_name;
		if (number <= chunking_area_font_names.size()) {
			chunking_area_font_name = chunking_area_font_names[number-1];
		} else {
			// Use last value in vector.
			chunking_area_font_name = *chunking_area_font_names.rbegin();
		}
		outputAreaFontName = wxString(chunking_area_font_name.c_str(), wxConvUTF8);
	} else {
#ifdef __WXMSW__
#if wxUSE_UNICODE
		outputAreaFontName = wxString(wxT("Arial MS Unicode"));
#else
		outputAreaFontName = wxString(wxT("Courier New"));
#endif
#else  // Not Windows
		outputAreaFontName = wxString(wxT("Courier"));
#endif
	}
	return outputAreaFontName;
}

void WXILLayoutCanvas::AddPara(indent_t indent, bool bIsCentered)
{
	// FIXME: Add indent!
	AddParagraph(m_bIsRightToLeft, 
		     bIsCentered, 
		     indent, 
		     false); // bAddVerticalSpaceAfter
}

void WXILLayoutCanvas::InitializeViewMetrics(Configuration *pConf)
{
	if (m_pViewMetrics) {
		delete m_pViewMetrics;
	}

	// Number of interlinear lines
	if (pConf != 0 && pConf->has_key("data_feature")) {
		m_nInterlinearLines = pConf->getValues("data_feature").size();
	} else {
		m_nInterlinearLines = 1;
	}

	// Resize the wxstringvec_t that we use for adding objects...
	m_strings_for_addition.resize(m_nInterlinearLines);
	
	// Magnification
	int nOutputAreaMagnification;
	if (pConf != 0 && pConf->has_key("chunking_area_magnification")) {
		std::string chunking_area_font_size = pConf->getValues("chunking_area_magnification")[0];
		std::istringstream istr(chunking_area_font_size);
		istr >> nOutputAreaMagnification;
	} else {
		nOutputAreaMagnification = 100;
	}
	m_nMagnification = nOutputAreaMagnification;


	// Font names
	std::vector<wxString> font_names;
	for (unsigned int index = 1; index <= m_nInterlinearLines; ++index) {
		font_names.push_back(getOutputAreaFontName(pConf, index));
	}


	// Right-to-Left
	m_bIsRightToLeft = false;
	if (pConf != 0) {
		m_bIsRightToLeft = pConf->has("option", "right_to_left");
	}

	m_pViewMetrics = new ViewMetrics(m_nInterlinearLines,
					 wxMM_TEXT,
					 m_bIsRightToLeft, 
					 false,
					 100);

	for (unsigned int index2 = 0; index2 < m_nInterlinearLines; ++index2) {
		InterlinearLineMetrics *pILM = 
			new InterlinearLineMetrics(font_names[index2],
						   true,
						   wxFONTENCODING_ISO8859_1,
						   mycolorarray[index2 % NO_OF_COLORS]);
		m_pViewMetrics->setILM(index2, pILM);
		
	}

	// TECkits...
	if (pConf != 0) {
		std::string message;
		if (!compileTECkits(pConf, message)) {
			wxEmdrosErrorMessage(wxString(wxT("Error: Could not load TECkits...\n")));
		}
	}

	// Call base class to finish up...
	LayoutCanvas::InitializeViewMetrics();
}


LayoutBox *WXILLayoutCanvas::AddEmdrosObject(wxDC *pDC, const EmdrosObject* pObj)
{
	LayoutBox *pBox = 0;

	monad_m fm = pObj->first();

	Monad2LayoutBoxMap::iterator it = m_box_map.find(fm);

	if (it != m_box_map.end()) {
		pBox = it->second;

		pBox->SetSelected(false);
		AddBox(pDC, pBox, 
		       false, // Paragraph does not own box; we do!
		       false);  // Do not recalculate paragraph extents now
	} else {
		feature_vec_t::size_type i;
		for (i = 0; i < m_nInterlinearLines; ++i) {
			m_strings_for_addition[i] = pObj->getFeature(i);
		}
		pBox = new LayoutBox(pDC,
				     m_strings_for_addition,
				     m_pViewMetrics,
				     true, // bSpaceAfter
				     m_bIsRightToLeft,
				     true); // bIsCentered
		pBox->SetSelected(false);
		AddBox(pDC, pBox, 
		       false,   // Paragraph does not own box; we do!
		       false);  // Do not recalculate paragraph extents now

		m_box_map.insert(std::make_pair(fm, pBox));
		pBox->SetLong(pObj->first());
	}

	return pBox;
}


bool loadFile(std::string filename, std::string& outstring, std::string& message)
{
	std::ostringstream ostr;
	std::ifstream fin;
	fin.open(filename.c_str());
	if (!fin) {
		std::string newFileName = app_prefix() + filename;
		std::ifstream fin2;
		fin2.open(newFileName.c_str());
		if (!fin2) {
		        message += "ERROR: Could not open file " + filename +" for reading.\n\n";
			return false;
		} else {
	                char c;
			while (fin2.get(c)) ostr.put(c);
			fin2.close();
		}
	} else {
	        char c;
	        while (fin.get(c)) ostr.put(c);
	        fin.close();
	}
		
	outstring = ostr.str();
	return true;
}

bool string2encoding(const std::string& encoding_name, unsigned int& output)
{
	std::string locase_name;
	str_tolower(encoding_name, locase_name);
	if (encoding_name == "byte"
	    || encoding_name == "bytes") {
		output = kForm_Bytes;
		return true;
	} else if (encoding_name == "unicode"
		   || encoding_name == "utf8"
		   || encoding_name == "utf-8"
		   || encoding_name == "utf_8") {
		output = kForm_UTF8;
		return true;
	} else {
		return false;
	}
}



bool WXILLayoutCanvas::compileTECkits(Configuration *pConf, std::string& message)
{
	if (!pConf->has_key("data_feature_teckit_mapping")) {
		m_bHasTECkitTables = false;
		return true;
	} else {
		StringVec features = pConf->getValues("data_feature");
		m_teckit_vec.resize(features.size(), 0);
		bool bResult = false;
		for (StringVec::size_type i = 0;
		     i < features.size();
		     ++i) {
			std::string feature_name = features[i];
			if (!pConf->has_subkey("data_feature_teckit_mapping", 
					       feature_name)) {
				m_teckit_vec[i] = 0;
			} else {
				std::string filename = pConf->getSubkeyValues("data_feature_teckit_mapping", feature_name)[0];
				std::string mapping;
				if (!loadFile(filename, mapping, message)) {
					return false;
				} 
				
				char *szBuf;
				szBuf = new char[mapping.length() + 1];
				strcpy(szBuf, mapping.c_str());
				UInt32 lenArrTECkit;
				Byte *arrTECkit;
				if (TECkit_CompileOpt(szBuf, mapping.length(),
						      0,
						      0,
						      &arrTECkit,
						      &lenArrTECkit,
						      kForm_Bytes) == kStatus_NoError) {
					bResult = true;
				} else {
					message += "Error in compiling TECkit from filename " + filename + "\n";
					bResult = false;
				}
				TECkit_Converter cnvTECkitConverter;
				if (bResult) {
					// Use these defaults
					unsigned int input_encoding = kForm_Bytes;
					unsigned int output_encoding = kForm_UTF8;
					
					if (pConf->has_subkey("data_feature_teckit_in_encoding", feature_name)) {
						std::string input_encoding_name = pConf->getSubkeyValues("data_feature_teckit_in_encoding", feature_name)[0];
						if (!string2encoding(input_encoding_name, input_encoding)) {
							message += "unknown encoding for data_feature_teckit_in_encoding with feature name '" + feature_name + "'.\nValid values are: \"byte\" or \"unicode\".";
							bResult = false;
						}
					}
					
					if (bResult 
					    && pConf->has_subkey("data_feature_teckit_out_encoding", feature_name)) {
						std::string output_encoding_name = pConf->getSubkeyValues("data_feature_teckit_out_encoding", feature_name)[0];
						if (!string2encoding(output_encoding_name, output_encoding)) {
							message += "unknown encoding for data_feature_teckit_out_encoding with feature name '" + feature_name + "'.\nValid values are: \"byte\" or \"unicode\".";
							bResult = false;
						}
					}
					
					if (bResult) {
						TECkit_Status status = TECkit_CreateConverter(arrTECkit,
											      lenArrTECkit,
											      1,  // Direction is forward
											      input_encoding,
											      output_encoding,
											      &cnvTECkitConverter);
						if (status  == kStatus_NoError) {
							bResult = true;
							m_bHasTECkitTables = true;
							
							m_teckit_vec[i] = cnvTECkitConverter;
							
						}
					} else {
						message += "Error in creating TECkit converter for filename " + filename + "\n";
						bResult = false;
					}
				}
				delete[] szBuf;
			}
		}
		
		return bResult;
	}
}

std::string WXILLayoutCanvas::applyTECkit(unsigned int feature_index, const std::string& feature_value)
{
	if (!m_bHasTECkitTables) {
		return feature_value;
	} else {
		TECkit_Converter pConverter = m_teckit_vec[feature_index];
		if (pConverter == 0) {
			return feature_value;
		} else {
			// Calculate lengths
			std::string::size_type length = feature_value.length();
			std::string::size_type nInBufferLength = length + 1;
			std::string::size_type nOutBufferLength	= nInBufferLength*4 + 1;

			// Allocate and zero buffers
			Byte *szInBuffer = (Byte*) malloc(nInBufferLength);
			Byte *szOutBuffer = (Byte*) malloc(nOutBufferLength);
			memset(szInBuffer, 0, nInBufferLength);
			memset(szOutBuffer, 0, nOutBufferLength);

			// Copy feature-value to inBuffer
			memcpy(szInBuffer, feature_value.c_str(), length);
			UInt32 inUsed, outUsed, lookaheadCount;
			TECkit_Status status = 
				TECkit_ConvertBufferOpt(pConverter,
							szInBuffer,
							length,
							&inUsed,
							szOutBuffer,
							nOutBufferLength,
							&outUsed,
							kOptionsUnmapped_UseReplacementCharWithWarning
							| kOptionsMask_InputComplete,
							&lookaheadCount); // input is complete

			std::string result;
			if (status == kStatus_NoError) {
				result = std::string((char*) szOutBuffer, outUsed);
			} else {
				result = feature_value;
			}
			free(szInBuffer);
			free(szOutBuffer);
			return result;
		}
	}
}







/*!
 * MainFrame type definition
 */

IMPLEMENT_CLASS( MainFrame, wxFrame )

/*!
 * MainFrame event table definition
 */

	BEGIN_EVENT_TABLE( MainFrame, wxFrame )
////@begin MainFrame event table entries
	EVT_MENU(ID_MENU_FILE_CONNECT, MainFrame::OnFileConnect)
	EVT_MENU(ID_MENU_FILE_COMMIT_TO_DB, MainFrame::OnFileCommitToDB)
	EVT_MENU(ID_MENU_EDIT_SPLIT, MainFrame::OnEditSplit)
	EVT_MENU(ID_MENU_EDIT_COMBINE, MainFrame::OnEditCombine)
	EVT_MENU(ID_MENU_EDIT_MOVE_LEFT, MainFrame::OnEditMoveLeft)
	EVT_MENU(ID_MENU_EDIT_MOVE_RIGHT, MainFrame::OnEditMoveRight)
	EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
	EVT_MENU(wxID_HELP, MainFrame::OnHelpContents)
	EVT_MENU(ID_MENU_TOOLS_CONFIGURE, MainFrame::OnToolsConfigure)
	EVT_MENU(wxID_EXIT, MainFrame::OnFileExit)

//EVT_UPDATE_UI(wxID_EXIT, MainFrame::OnUpdateUINoExec)

        EVT_MAXIMIZE(MainFrame::OnMaximize)
        EVT_SIZE(MainFrame::OnSize)

////@end MainFrame event table entries

	END_EVENT_TABLE()







/*!
 * MainFrame constructors
 */

	MainFrame::MainFrame( )
		: m_pChunkingArea(0), m_pEE(0), m_pConf(0), m_pDoc()
{
}

MainFrame::~MainFrame()
{
	delete m_pEE;
	delete m_pConf;
	delete m_pChunkingArea;
	delete m_pDoc;
}

MainFrame::MainFrame( wxWindow* parent, 
		      const ConnectionData& conndata,
		      wxWindowID id, 
		      const wxString& caption, 
		      const wxPoint& pos, 
		      const wxSize& size, long style )
	: m_pChunkingArea(0), m_pEE(0), m_pConf(0), m_pDoc(0)
{
	m_connectionData = conndata;
	Create( parent, id, caption, pos, size, style );
	//Connect();

	/*
	// Set default style of text controls
	if (m_pOut != 0) {
	wxTextAttr inputAreaTextAttr(*wxBLACK, wxNullColour,
	wxFont(m_pOut->getInputAreaFontSize(), 
	wxMODERN, wxNORMAL, wxNORMAL, 
	FALSE, 
	m_pOut->getInputAreaFontName()));
	m_pEditWindow->SetDefaultStyle(inputAreaTextAttr);

	wxTextAttr outputAreaTextAttr(*wxBLACK, wxNullColour,
	wxFont(m_pOut->getOutputAreaFontSize(), 
	wxMODERN, wxNORMAL, wxNORMAL, 
	FALSE, 
	m_pOut->getOutputAreaFontName()));
	}
	*/
}

/*!
 * MainFrame creator
 */

bool MainFrame::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
	////@begin MainFrame member initialisation
	m_ctrlToolBar = NULL;
	////@end MainFrame member initialisation

	////@begin MainFrame creation
	wxFrame::Create( parent, id, caption, pos, size, style );

	CreateControls();
	Centre();

	////@end MainFrame creation
	return TRUE;
}

/*!
 * Control creation for MainFrame
 */

void MainFrame::CreateControls()
{    
	////@begin MainFrame content construction

	m_strCurFileName = wxT("");
	MainFrame* itemFrame1 = this;

	wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);
	pMainSizer->SetMinSize( wxSize( 15,15 ) ); 

	m_ctrlToolBar = this->CreateToolBar(wxTB_FLAT|wxTB_HORIZONTAL, ID_TOOLBAR);
	m_ctrlToolBar->SetToolBitmapSize(wxSize(48, 48));
	itemFrame1->SetToolBar(m_ctrlToolBar);

	wxBitmap itemtool21Bitmap(itemFrame1->GetBitmapResource(wxT("connect.xpm")));
	m_ctrlToolBar->AddTool(ID_MENU_FILE_CONNECT, wxT("Connect"), itemtool21Bitmap, wxT("New connection"), wxITEM_NORMAL);
	wxBitmap itemtool23Bitmap(itemFrame1->GetBitmapResource(wxT("commit.xpm")));
	m_ctrlToolBar->AddTool(ID_MENU_FILE_COMMIT_TO_DB, wxT("Commit"), itemtool23Bitmap, wxT("Commit changes to DB"), wxITEM_NORMAL);
	m_ctrlToolBar->AddSeparator();
	wxBitmap itemtool24Bitmap(itemFrame1->GetBitmapResource(wxT("flash.xpm")));
	m_ctrlToolBar->AddTool(ID_MENU_EDIT_SPLIT, wxT("Split"), itemtool24Bitmap, wxT("Split"), wxITEM_NORMAL);

	wxBitmap itemtool27Bitmap(itemFrame1->GetBitmapResource(wxT("together.xpm")));
	m_ctrlToolBar->AddTool(ID_MENU_EDIT_COMBINE, wxT("Combine"), itemtool27Bitmap, wxT("Combine"), wxITEM_NORMAL);

	wxBitmap itemtool25Bitmap(itemFrame1->GetBitmapResource(wxT("leftarrow.xpm")));
	m_ctrlToolBar->AddTool(ID_MENU_EDIT_MOVE_LEFT, wxT("Move left"), itemtool25Bitmap, wxT("Move left"), wxITEM_NORMAL);
	wxBitmap itemtool26Bitmap(itemFrame1->GetBitmapResource(wxT("rightarrow.xpm")));
	m_ctrlToolBar->AddTool(ID_MENU_EDIT_MOVE_RIGHT, wxT("Move right"), itemtool26Bitmap, wxT("Move right"), wxITEM_NORMAL);

	wxStatusBar* itemStatusBar27 = new wxStatusBar( itemFrame1, ID_STATUSBAR, wxST_SIZEGRIP|wxNO_BORDER );
	itemStatusBar27->SetFieldsCount(2);
	itemFrame1->SetStatusBar(itemStatusBar27);

	m_pChunkingArea = new WXILLayoutCanvas( itemFrame1, wxDefaultPosition, wxDefaultSize, 0, m_pConf);


	wxMenuBar* menuBar = new wxMenuBar;
	wxMenu* itemMenu3 = new wxMenu;
	itemMenu3->Append(ID_MENU_FILE_CONNECT, wxT("Co&nnect..."), wxT(""), wxITEM_NORMAL);
	itemMenu3->Append(ID_MENU_FILE_COMMIT_TO_DB, wxT("&Commit"), wxT("Commits changes to DB."), wxITEM_NORMAL);
	itemMenu3->AppendSeparator();
	itemMenu3->Append(wxID_EXIT, wxT("E&xit"), wxT(""), wxITEM_NORMAL);
	menuBar->Append(itemMenu3, wxT("&File"));

	wxMenu* itemMenu10 = new wxMenu;
	itemMenu10->Append(ID_MENU_EDIT_SPLIT, wxT("Split\tCtrl-S"), wxT("Split after current"), wxITEM_NORMAL);
	itemMenu10->Append(ID_MENU_EDIT_COMBINE, wxT("Combine\tCtrl-D"), wxT("Combine current and previous"), wxITEM_NORMAL);
	itemMenu10->Append(ID_MENU_EDIT_MOVE_LEFT, wxT("Move left\tCtrl-K"), wxT("Moves the current chunk to the left."), wxITEM_NORMAL);
	itemMenu10->Append(ID_MENU_EDIT_MOVE_RIGHT, wxT("Move right\tCtrl-L"), wxT("Moves the current chunk to the right."), wxITEM_NORMAL);
	menuBar->Append(itemMenu10, wxT("&Edit"));

	wxMenu* itemMenu14 = new wxMenu;
	itemMenu14->Append(ID_MENU_TOOLS_CONFIGURE, wxT("&Configure..."), wxT("Configure Chunking Tool"), wxITEM_NORMAL);
	menuBar->Append(itemMenu14, wxT("&Tools"));

	wxMenu* itemMenu18 = new wxMenu;
	itemMenu18->Append(wxID_HELP, wxT("&Help Contents\tF1"), wxT("Brings up the help viewer"), wxITEM_NORMAL);
	itemMenu18->AppendSeparator();
	itemMenu18->Append(wxID_ABOUT, wxT("A&bout Emdros Chunking Tool..."), wxT("Brings up the About box"), wxITEM_NORMAL);
	menuBar->Append(itemMenu18, wxT("&Help"));
	itemFrame1->SetMenuBar(menuBar);

	m_ctrlToolBar->Realize();

	pMainSizer->Add( m_pChunkingArea, 1, wxALL|wxEXPAND, 0 );
	
	this->SetSizer( pMainSizer );

	this->Layout();

	Centre();


	////@end MainFrame content construction
}

/*!
 * Should we show tooltips?
 */

bool MainFrame::ShowToolTips()
{
	return TRUE;
}

/*!
 * Get bitmap resources
 */

wxBitmap MainFrame::GetBitmapResource( const wxString& name )
{
	// Bitmap retrieval
	////@begin MainFrame bitmap retrieval
	if (name == wxT("connect.xpm")) {
		wxBitmap bitmap(connect_xpm);
		return bitmap;
	} else if (name == wxT("commit.xpm")) {
		wxBitmap bitmap(commit_xpm);
		return bitmap;
	} else if (name == wxT("leftarrow.xpm")) {
		wxBitmap bitmap(leftarrow_xpm);
		return bitmap;
	} else if (name == wxT("rightarrow.xpm")) {
		wxBitmap bitmap(rightarrow_xpm);
		return bitmap;
	} else if (name == wxT("flash.xpm")) {
		wxBitmap bitmap(flash_xpm);
		return bitmap;
	} else if (name == wxT("together.xpm")) {
		wxBitmap bitmap(together_xpm);
		return bitmap;
	} 

	return wxNullBitmap;
	////@end MainFrame bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon MainFrame::GetIconResource( const wxString& name )
{
	UNUSED(name);
	
	// Icon retrieval
	////@begin MainFrame icon retrieval
	return wxNullIcon;
	////@end MainFrame icon retrieval
}


void MainFrame::OnMaximize(wxMaximizeEvent& event)
{
	// Pass it higher up in the hierarchy
	event.Skip();
}



void MainFrame::OnSize(wxSizeEvent& event)
{
	// Pass it higher up in the hierarchy
	event.Skip();
}


void MainFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
	wxString message = wxT("Emdros Chunking Tool version 1.2.0\nfrom Emdros version ");
	message += wxString(EMDROS_VERSION, wxConvUTF8);
	message += wxT("\n\nCopyright (C) 2005-2013 Ulrik Sandborg-Petersen\n\nMade available under the MIT License.\nEmdros Chunking Tool comes with NO WARRANTY WHATSOEVER.\n\nSee https://emdros.org/examples/chunkingtool for more information");
	(void)wxMessageBox(message, wxT("About Emdros Chunking Tool"));
}

void MainFrame::OnToolsConfigure(wxCommandEvent& WXUNUSED(event) )
{
	ChunkingToolConfigWizard *pWizard = new ChunkingToolConfigWizard(this,
									 m_connectionData.m_strConfiguration);
	if (pWizard->RunIt()) {
		// Do something
	} else {
		// Do something else
	}
}


void MainFrame::OnHelpContents(wxCommandEvent& WXUNUSED(event) )
{
	wxHtmlHelpController *help;
	help = new wxHtmlHelpController(wxHF_DEFAULT_STYLE);

	wxString strHelpFileName = EmdrosChunkingToolApp::wxHelpPrefix() + wxT("ChunkingToolGuide.htb");

	help->AddBook(wxFileName(strHelpFileName));
   
	help->DisplayContents();
}


void MainFrame::OnEditSplit(wxCommandEvent& event)
{
	UNUSED(event);
	
	if (m_cur_selected_box != 0) {
		monad_m m = (monad_m) m_cur_selected_box->GetLong();
		CTChunk *pChunk = m_pDoc->getChunkFromMonad(m);
		if (pChunk != 0) {
			CTChunk *pNewChunk = pChunk->split(m, 
							   pChunk->getIndent()); // New indent
			if (pNewChunk != 0) {
				m_pDoc->addChunk(pNewChunk);
				UpdateChunkingArea();
			} else {
			}
		} else {
			// FIXME: Say something!
		}
	} else {
		// Nothing to do: Not selected!
	}
}


void MainFrame::OnEditCombine(wxCommandEvent& event)
{
	UNUSED(event);
	
	if (m_cur_selected_box != 0) {
		monad_m m = (monad_m) m_cur_selected_box->GetLong();
		CTChunk *pCurrentChunk = m_pDoc->getChunkFromMonad(m);
		if (pCurrentChunk != 0) {
			monad_m current_chunk_first = pCurrentChunk->first();
			CTChunk *pPreviousChunk = m_pDoc->getChunkFromMonad(current_chunk_first-1);
			if (pPreviousChunk != 0) {
				monad_m current_chunk_last = pCurrentChunk->last();
				monad_m previous_chunk_first = pPreviousChunk->first();
				indent_t previous_indent = pPreviousChunk->getIndent();
				CTChunk *pNewChunk = new CTChunk(SetOfMonads(previous_chunk_first, current_chunk_last), previous_indent);
				
				
				if (pNewChunk != 0) {
					m_pDoc->removeChunk(pCurrentChunk);
					m_pDoc->removeChunk(pPreviousChunk);
					delete pCurrentChunk;
					delete pPreviousChunk;
					m_pDoc->addChunk(pNewChunk);
					UpdateChunkingArea();
				} else {
				}
			} else {
				// FIXME: Say something!
			}
		} else {
			// FIXME: Say something!
		}
	} else {
		// Nothing to do: Not selected!
	}
}


void MainFrame::OnEditMoveLeft(wxCommandEvent& event)
{
	UNUSED(event);

	if (m_cur_selected_box != 0) {
		monad_m m = (monad_m) m_cur_selected_box->GetLong();
		CTChunk *pChunk = m_pDoc->getChunkFromMonad(m);
		if (pChunk != 0) {
			if (m_pChunkingArea->isRightToLeft()) {
				pChunk->increaseIndent();
			} else {
				pChunk->decreaseIndent();
			}
			UpdateChunkingArea();
		} else {
			// FIXME: Say something!
		}
	} else {
		// Nothing to do: Not selected!
	}

}

void MainFrame::OnEditMoveRight(wxCommandEvent& event)
{
	UNUSED(event);
	
	if (m_cur_selected_box != 0) {
		monad_m m = (monad_m) m_cur_selected_box->GetLong();
		CTChunk *pChunk = m_pDoc->getChunkFromMonad(m);
		if (pChunk != 0) {
			if (m_pChunkingArea->isRightToLeft()) {
				pChunk->decreaseIndent();
			} else {
				pChunk->increaseIndent();
			}
			UpdateChunkingArea();
		} else {
			// FIXME: Say something!
		}
	} else {
		// Nothing to do: Not selected!
	}
}


void MainFrame::OnFileConnect(wxCommandEvent& event)
{
	UNUSED(event);
	
	ConnectionData conndata;
	if (!wxGetApp().GetConnection(conndata)) {
		// This means that the user pressed 'cancel'.
	} else {
		m_connectionData = conndata;
		Connect();
		if (!LoadDocument()) {
			wxEmdrosErrorMessage(wxString(wxT("Error: Could not load document from database.\n")));
		} else {
			// m_pDoc->pretty();
			UpdateChunkingArea();
		}
	}
}

void MainFrame::OnFileCommitToDB(wxCommandEvent& event)
{
	UNUSED(event);
	
	bool bDoCommitTransaction = m_pEE->beginTransaction();

	std::string query = "DELETE OBJECTS BY MONADS = {1-" + monad_m2string(MAX_MONAD) + "}\n[Chunk]\nGO";
	if (!ExecuteEmdrosStatement(query)) {
		if (bDoCommitTransaction) {
			m_pEE->abortTransaction();
		}
		wxEmdrosErrorMessage(wxString(wxT("Error: Could not delete all chunks.\n")));
		return;
	} 

	const chunkmap_t& chunks = m_pDoc->getChunks();

	query = "CREATE OBJECTS WITH OBJECT TYPE [Chunk]\n";
	
	chunkmap_t::const_iterator chunkci = chunks.begin();

	while (chunkci != chunks.end()) {
		const CTChunk *pChunk = chunkci->second;

		query += "CREATE OBJECT FROM MONADS={" + monad_m2string(pChunk->first()) + "-" + monad_m2string(pChunk->last()) + "}\n[indent:=" + long2string(pChunk->getIndent()) +";]\n";

		++chunkci;
	}
	query += "GO";

	if (!ExecuteEmdrosStatement(query)) {
		if (bDoCommitTransaction) {
			m_pEE->abortTransaction();
		}
		wxEmdrosErrorMessage(wxString(wxT("Error: Could not create chunks.\n")));
		return;
	} 

	if (bDoCommitTransaction) {
		m_pEE->commitTransaction();
	}
}



void MainFrame::OnFileExit(wxCommandEvent& event)
{
	UNUSED(event);
	
	Close();
}


void MainFrame::UpdateChunkingArea()
{
	wxBusyCursor wait;

	const chunkmap_t& chunks = m_pDoc->getChunks();
	chunkmap_t::const_iterator chunkci = chunks.begin();
	chunkmap_t::const_iterator cend = chunks.end();
	m_cur_selected_box = 0;
	m_pChunkingArea->Clear();
	wxClientDC *pDC = m_pChunkingArea->GetClientDC();
	
	while (chunkci != cend) {
		const CTChunk* pChunk = chunkci->second;
		m_pChunkingArea->AddPara(pChunk->getIndent(),
					 false); // Is not centered
		monad_m m = pChunk->first();
		monad_m chunk_last = pChunk->last();
		do {
			const EmdrosObject *pObj = m_pDoc->getObjectFromMonad(m);
			if (pObj != 0) {
				LayoutBox *pBox = m_pChunkingArea->AddEmdrosObject(pDC, pObj);

				if (m_cur_selected_monad == m) {
					m_cur_selected_box = pBox;
					pBox->SetSelected(true); // FIXME: Deselect whatever was selected!
				}
				
				// Advance m 1 past end of object
				m = pObj->last() + 1;
			} else {
				++m;
			}
		} while (m <= chunk_last);
		++chunkci;
	}

	delete pDC;

	m_pChunkingArea->RecalculateParagraphExtents();

	m_pChunkingArea->flush();
}


bool MainFrame::LoadDocument()
{
	delete m_pDoc;
	m_pDoc = new CTDoc();
	
	wxBusyCursor wait;
	
	bool bChunkTypeExists;
	if (!ObjectTypeExists(CHUNK_OT_NAME, bChunkTypeExists)) {
		wxEmdrosErrorMessage(wxString(wxT("Error: Database error while finding chunk object type.\n")));

		return false;
	}

	if (!bChunkTypeExists) {
		if (!CreateChunkType(CHUNK_OT_NAME)) {
			wxEmdrosErrorMessage(wxString(wxT("Error: Database error while creating chunk object type.\n")));
			
			return false;
		}
		if (!DoPreChunking()) {
			wxEmdrosInfoMessage(wxString(wxT("Warning: Could not do pre-chunking.")));
			// Don't return false... just go on...
		}
	}
	

	// Get objects
	objectlist_t objects;
	if (!LoadObjects(objects)) {
		for (objectlist_t::iterator it = objects.begin();
		     it != objects.end();
		     ++it) {
			delete *it;
		}

		wxEmdrosErrorMessage(wxString(wxT("Error: Could not load data_unit objects.\n")));
		
		return false;
	}

	// Get chunks
	chunklist_t chunks;
	if (!LoadChunks(chunks)) {
		for (objectlist_t::iterator it = objects.begin();
		     it != objects.end();
		     ++it) {
			delete *it;
		}

		for (chunklist_t::iterator it = chunks.begin();
		     it != chunks.end();
		     ++it) {
			delete *it;
		}
		
		wxEmdrosErrorMessage(wxString(wxT("Error: Could not load chunks.\n")));
		return false;
	}

	m_pDoc->addObjects(objects);

	m_pDoc->addChunks(chunks);

	return true;
}


bool MainFrame::DoPreChunking()
{
	if (m_pConf->has_key("pre_chunking_unit")) {
		std::string pre_chunking_unit = m_pConf->getValues("pre_chunking_unit")[0];

		bool bPreChunkingTypeExists;
		if (!ObjectTypeExists(pre_chunking_unit, bPreChunkingTypeExists)) {
			wxEmdrosErrorMessage(wxString(wxT("Error: Database error while finding pre-chunking object type.\n")));
			
			return false;
		}

		std::string query = 
			"GET OBJECTS HAVING MONADS IN ALL\n"
			"[" + pre_chunking_unit + "]\n"
			"GO";
		

		if (!ExecuteEmdrosStatement(query)) {
			return false;
		}

		// Get flat sheaf
		FlatSheaf *pFlatSheaf = m_pEE->takeOverFlatSheaf();
		if (pFlatSheaf == 0) {
			wxEmdrosErrorMessage(wxString(wxT("Error: Result was not a flat sheaf while getting pre-chunking units.\n")));
			return false;
		} 

		// Were there no objects?
		if (pFlatSheaf->isFail()) {
			wxEmdrosErrorMessage(wxString(wxT("Error: There were no objects of the pre_chunking_unit type.\n")));
			delete pFlatSheaf;
			return false;
		} else {
			// There were objects, so traverse the flat sheaf...

			// There is only one FlatStraw, so we get it...
			FlatStraw *pObjectStraw = pFlatSheaf->const_iterator().current();
			FlatStrawConstIterator strawit = pObjectStraw->const_iterator();
			MatchedObject *pMO = 0;

			FastSetOfMonads fastSOM;

			Bigstring query;
			query.addsz("BEGIN TRANSACTION GO\n");
			query.addsz("CREATE OBJECTS WITH OBJECT TYPE [");
			query.addsz(CHUNK_OT_NAME);
			query.addsz("]\n");

			while (strawit.hasNext()) {
				pMO = strawit.next();
				SetOfMonads som = pMO->getMonads();

				fastSOM.unionWith(som);

				SOMConstIterator ci = som.const_iterator();
				while (ci.hasNext()) {
					MonadSetElement mse = ci.next();
					query.addsz("CREATE OBJECT FROM MONADS={");
					query.addsz(monad_m2string(mse.first()).c_str());
					query.addChar('-');
					query.addsz(monad_m2string(mse.last()).c_str());
					query.addsz("}[]\n");
				}

			}

			delete pFlatSheaf;

			// Get MIN_M and MAX_M
			monad_m min_m;
			if (!m_pEE->getMin_m(min_m)) {
				wxEmdrosErrorMessage(wxString(wxT("Error getting min_m while doing pre-chunking...\n")));
				return false;
			}

			monad_m max_m;
			if (!m_pEE->getMax_m(max_m)) {
				wxEmdrosErrorMessage(wxString(wxT("Error getting max_m while doing pre-chunking...\n")));
				return false;
			}

			if (min_m < fastSOM.first()) {
				query.addsz("CREATE OBJECT FROM MONADS={");
				query.addsz(monad_m2string(min_m).c_str());
				query.addChar('-');
				query.addsz(monad_m2string(fastSOM.first()-1).c_str());
				query.addsz("}[]\n");
			}

			if (max_m > fastSOM.last()) {
				query.addsz("CREATE OBJECT FROM MONADS={");
				query.addsz(monad_m2string(fastSOM.last()+1).c_str());
				query.addChar('-');
				query.addsz(monad_m2string(max_m).c_str());
				query.addsz("}[]\n");
			}

			// Fill gaps...
			if (!fastSOM.hasOnlyOneMSE()) {
				FastSOMConstIterator ci2 = fastSOM.const_iterator();
				monad_m previous = ci2.current().last()+1;
				ci2.next();
				while (ci2.hasNext()) {
					MonadSetElement mse = ci2.next();
					query.addsz("CREATE OBJECT FROM MONADS={");
					query.addsz(monad_m2string(previous).c_str());
					query.addChar('-');
					query.addsz(monad_m2string(mse.first()-1).c_str());
					query.addsz("}[]\n");
					previous = mse.last()+1;
				}
			}
			

			query.addsz("GO\nCOMMIT TRANSACTION GO");

			std::string strQuery;
			query.toString(strQuery);

			if (!ExecuteEmdrosStatement(strQuery)) {
				return false;
			}

			return true;
		}
	} else {
		// If we don't have the "pre_chunking_unit" configuration
		// variable, we just return true; we succeeded.
		return true;
	}
}


bool MainFrame::LoadObjects(objectlist_t& objects)
{
	std::string OTN = m_pConf->getValues("data_unit")[0];

	std::string query = 
		"GET OBJECTS HAVING MONADS IN ALL\n"
		"[" + OTN + "\n"
		"   GET ";

	StringVec feature_names = m_pConf->getValues("data_feature");

	StringVec::const_iterator ci = feature_names.begin();
	query += *ci;
	++ci;
	while (ci != feature_names.end()) {
		query += ", " + *ci;
		++ci;
	}

	query += "\n]\nGO";

	if (!ExecuteEmdrosStatement(query)) {
		return false;
	}

	// Get flat sheaf
	FlatSheaf *pFlatSheaf = m_pEE->takeOverFlatSheaf();
	if (pFlatSheaf == 0) {
		wxEmdrosErrorMessage(wxString(wxT("Error: Result was not a flat sheaf while getting data units.\n")));
		return false;
	} 

	// Were there no objects?
	if (pFlatSheaf->isFail()) {
		wxEmdrosErrorMessage(wxString(wxT("Error: There were no objects of the data_unit type.\n")));
		delete pFlatSheaf;
		return false;
	} else {
		// There were objects, so traverse the flat sheaf...

		// There is only one FlatStraw, so we get it...
		FlatStraw *pObjectStraw = pFlatSheaf->const_iterator().current();
		FlatStrawConstIterator strawit = pObjectStraw->const_iterator();
		MatchedObject *pMO = 0;
		StringVec::size_type feature_names_length = feature_names.size();
		bool bIsFirst = true;
		while (strawit.hasNext()) {
			pMO = strawit.next();
			SetOfMonads som = pMO->getMonads();

			if (bIsFirst) {
				bIsFirst = false;
				m_cur_selected_monad = som.first();
			}

			feature_vec_t features;
			for (StringVec::size_type i = 0; i < feature_names_length; ++i) {
				std::string feature;
				const EMdFValue *pValue = pMO->getEMdFValue(i);
				if (pValue->getKind() == kEVEnum) {
					feature = pMO->getEnumLabel(i);
				} else {
					feature = pValue->toString();
				}
				wxString feature_wxstring = wxString(m_pChunkingArea->applyTECkit(i,feature).c_str(), wxConvUTF8);
				features.push_back(feature_wxstring);
			}
			EmdrosObject* pObj = new EmdrosObject(som, features);
			objects.push_back(pObj);
		}

		delete pFlatSheaf;

		return true;
	}
}


bool MainFrame::LoadChunks(chunklist_t& chunks)
{
	std::string query = 
		"GET OBJECTS HAVING MONADS IN ALL\n"
		"[" CHUNK_OT_NAME "\n"
		"   GET indent\n"
		"]\nGO";
	if (!ExecuteEmdrosStatement(query)) {
		return false;
	}

	// Get flat sheaf
	FlatSheaf *pFlatSheaf = m_pEE->takeOverFlatSheaf();
	if (pFlatSheaf == 0) {
		wxEmdrosErrorMessage(wxString(wxT("Error: Result was not a flat sheaf while getting chunks.\n")));
		return false;
	} 

	// Were there no chunk objects?
	if (pFlatSheaf->isFail()) {
		return false;
	} else {
		// There were chunk objects, so traverse the flat sheaf...

		// There is only one FlatStraw, so we get it...
		FlatStraw *pChunkStraw = pFlatSheaf->const_iterator().current();
		FlatStrawConstIterator strawit = pChunkStraw->const_iterator();
		if (strawit.hasNext()) {
			MatchedObject *pMO = 0;
			while (strawit.hasNext()) {
				pMO = strawit.next();
				SetOfMonads som = pMO->getMonads();
				indent_t indent = (indent_t) pMO->getEMdFValue(0)->getInt();
				CTChunk *pNewChunk = new CTChunk(som.first(), som.last(), indent);
				chunks.push_back(pNewChunk);
				
			}
		} else {
			// Make a single chunk...
			
			// Get all_m_1
			SetOfMonads all_m_1;
			if (!m_pEE->getAll_m_1(all_m_1)) {
				wxEmdrosErrorMessage(wxString(wxT("Error: Could not get all_m_1 from database.\n")));
				delete pFlatSheaf;
				return false;
			}
			CTChunk* pChunkAll = new CTChunk(all_m_1.first(), all_m_1.last(), 0);
			chunks.push_back(pChunkAll);
		}

		
		delete pFlatSheaf;
		return true;
	}
}




bool MainFrame::CreateChunkType(std::string chunk_ot_name)
{
	std::string query = 
		std::string("CREATE OBJECT TYPE\n")
		+ "[" + chunk_ot_name + "\n"
		"  indent : INTEGER;\n"
		"]\n"
		"GO";
	if (!ExecuteEmdrosStatement(query)) {
		return false;
	} else {
		return true;
	}
}


bool MainFrame::ExecuteEmdrosStatement(std::string query)
{
	bool bCompileResult;
	if (!m_pEE->executeString(query, bCompileResult,
				  false, false)) {
		return false;
	} else {
		if (!bCompileResult) {
			wxEmdrosErrorMessage(wxString(wxT("Error: Compiler error executing this query:\n")) + wxString(query.c_str(), wxConvUTF8));
			return false;
		} else {
			return true;
		}
	}
}


bool MainFrame::ObjectTypeExists(std::string object_type_name, bool& bExists)
{
	id_d_t dummy_object_type_id;
	eObjectRangeType dummy_objectRangeType;
	eMonadUniquenessType dummy_monadUniquenessType;
	bool bDBOK = m_pEE->getMQLEE()->pDB->objectTypeExists(object_type_name,
								 bExists, 
								 dummy_object_type_id,
								 dummy_objectRangeType,
								 dummy_monadUniquenessType);
	if (!bDBOK) {
		m_pEE->clearErrors();
	}

	return bDBOK;
}

bool MainFrame::Connect()
{
	delete m_pConf;
	std::ostringstream ssout;
	m_pConf = parse_config_file(std::string((const char*)m_connectionData.m_strConfiguration.mb_str(wxConvUTF8)),
				    std::string(app_prefix()),
				    &ssout);
	if (m_pConf == 0) {
		//(*m_pOut) << ssout.str();
		wxEmdrosErrorMessage(wxString(wxT("Error: Could not load configuration file:\n")) + m_connectionData.m_strConfiguration);
		return false;
	} else {
		wxString strDB;
		if (m_connectionData.m_strDatabase.IsEmpty()) {
			if (m_pConf->has_key("database")) {
				strDB = wxString(m_pConf->getValues("database")[0].c_str(), wxConvUTF8);
			} else {
				wxEmdrosErrorMessage(wxString(wxT("Error: You must specify a database name,\n")
							      wxT("either in the connection dialog or in the\n")
							      wxT("configuration file (with the 'database' key).")));
				return false;
			}
		} else {
			strDB = m_connectionData.m_strDatabase;
			wxEmdrosInfoMessage(wxString(wxT("Successfully connected!")));
		}

		// Connect    
		delete m_pEE;
		// FIXME: This uses std::cout...
		m_pEE = new EmdrosEnv(kOKConsole,
				      kCSISO_8859_1, // FIXME: This should choose UTF8 sometimes...
				      std::string((const char*) m_connectionData.m_strHost.mb_str(wxConvUTF8)),
				      std::string((const char*) m_connectionData.m_strUser.mb_str(wxConvUTF8)),
				      std::string((const char*) m_connectionData.m_strPassword.mb_str(wxConvUTF8)),
				      std::string((const char*) strDB.mb_str(wxConvUTF8)),
				      m_connectionData.m_backend_kind);
		ExecuteEmdrosStatement(std::string("USE DATABASE '") + std::string((const char*) strDB.mb_str(wxConvUTF8)) + "' GO");

		m_pChunkingArea->InitializeViewMetrics(m_pConf);
		m_pChunkingArea->Clear();
		// m_pOut->print(std::string((const char*) (wxString(wxT("Successfully connected.\n")).mb_str(wxConvUTF8))));
		return true;
	}
}

