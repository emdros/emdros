/////////////////////////////////////////////////////////////////////////////
// Name:        samples/drawing/drawing.cpp
// Purpose:     shows and tests wxDC features
// Author:      Robert Roebling
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id: drawing.cpp,v 1.79 2005/09/15 15:18:37 ABX Exp $
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma implementation
    #pragma interface
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/colordlg.h"
#include "wx/image.h"
#include "wx/artprov.h"


#include <htreecanvas.h>

// ----------------------------------------------------------------------------
// ressources
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();

	virtual int OnExit() { DeleteLayout(); return 0; }

protected:
    void DeleteLayout();

    bool LoadLayout();
};


// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnRefresh(wxCommandEvent &event);

#if wxUSE_COLOURDLG
    wxColour SelectColour();
#endif // wxUSE_COLOURDLG
	
	void CreateLayout();
	void DeleteLayout();
	LayoutBox *CreateLB(const wxString& s);
	LayoutBox *CreateLB2(const wxString& s1, const wxString& s2);
	BorderedStackedLatinLayoutBox *CreateBSLLB(const wxString& s1, const wxString& s2, const wxString& s3, int nLineAfterWhichToAddSeparator);
	BorderedStackedLatinLayoutBox *CreateBSLLB4(const wxString& s1, const wxString& s2, const wxString& s3, const wxString& s4, int nLineAfterWhichToAddSeparator);
    HTreeCanvas   *m_canvas;

private:
    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------


// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

bool MyApp::LoadLayout()
{
    return true;
}

// `Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // Create the main application window
    MyFrame *frame = new MyFrame(wxT("Drawing sample"),
                                 wxPoint(50, 50), wxSize(550, 340));

    // Show it and tell the application that it's our main window
    frame->Show(true);
    SetTopWindow(frame);

    if ( !LoadLayout() )
    {
        wxLogError(wxT("Can't load one of the bitmap files needed ")
                   wxT("for this sample from the current or parent ")
                   wxT("directory, please copy them there."));

        // stop here
        DeleteLayout();

        return false;
    }

    // ok, continue
    return true;
}

void MyApp::DeleteLayout()
{
}

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU      (wxID_NEW,     MyFrame::OnRefresh)
    EVT_MENU      (wxID_EXIT,     MyFrame::OnQuit)
END_EVENT_TABLE()

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, wxID_ANY, title, pos, size,
                 wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE)
{

    wxMenu *menuFile = new wxMenu;
    menuFile->Append(wxID_NEW, wxT("Refresh\tCtrl-N"), wxT("Refresh canvas"));
    menuFile->Append(wxID_EXIT, wxT("E&xit\tAlt-X"), wxT("Quit this program"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, wxT("&File"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    ViewMetrics *pViewMetrics = new ViewMetrics(2, wxMM_TEXT, false, false, 100);
    pViewMetrics->setILM(0, new InterlinearLineMetrics(wxT("Gentium"), true, wxFONTENCODING_ISO8859_1, *wxBLACK, wxFONTFAMILY_DEFAULT, 1.2));
    pViewMetrics->setILM(1, new InterlinearLineMetrics(wxT("Gentium"), true, wxFONTENCODING_ISO8859_1, *wxBLACK, wxFONTFAMILY_DEFAULT, 1.2));

    m_canvas = new HTreeCanvas( pViewMetrics, this , wxPoint(0,0) , wxDefaultSize, 0 , false);
    m_canvas->SetScrollbars( 10, 10, 100, 240 );

    m_canvas->InitializeViewMetrics();

    CreateLayout();
    m_canvas->OnUpdate();
}

// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}


void MyFrame::OnRefresh(wxCommandEvent& WXUNUSED(event))
{
	m_canvas->Refresh();
}


void MyFrame::CreateLayout()
{
	SetOfMonads S0_som = SetOfMonads(1,6);
	S0_som.add(8,13);
	
	HNonTerminalNode *S0 = new HNonTerminalNode(100, NIL, S0_som, 0, false, CreateLB(wxT("S0")), true);
	m_canvas->addNonTerminal(S0);

	SetOfMonads VP1_som = SetOfMonads(1,3);
	VP1_som.add(6);
	
	HNonTerminalNode *VP1 = new HNonTerminalNode(101, S0->getID_D(), VP1_som, 
						     NULL, false, 
						     CreateBSLLB(wxT("VP1"), wxT("ads"), wxT(""), 0), true);
	m_canvas->addNonTerminal(VP1);

	HNonTerminalNode *PP = new HNonTerminalNode(102, VP1->getID_D(),
						    SetOfMonads(1,2), 
						    CreateLB(wxT("dsa")), true, 
						    CreateBSLLB4(wxT("PP"), wxT("dsa"), wxT("prepP"), wxT("blah1"), 2), true);
	m_canvas->addNonTerminal(PP);


	SetOfMonads NP0_som = SetOfMonads(4,5);
	NP0_som.add(8,13);
	HNonTerminalNode *NP0 = new HNonTerminalNode(103, S0->getID_D(), NP0_som, 
						     NULL, false,
						     CreateBSLLB(wxT("NP0"), wxT("Subj"), wxT("Blah"), 0), true);
	m_canvas->addNonTerminal(NP0);

	HNonTerminalNode *S1 = new HNonTerminalNode(104, NP0->getID_D(),
						    SetOfMonads(8,13), 
						    CreateLB2(wxT("321"), wxT("456")), true, 
						    CreateBSLLB(wxT("S1"), wxT("Main"), wxT("Subord"), 0), true,
						    true);
	m_canvas->addNonTerminal(S1);
	
	HNonTerminalNode *NP1 = new HNonTerminalNode(105, S1->getID_D(), SetOfMonads(10,12), 
						     CreateLB(wxT("22")), true, 
						     CreateLB(wxT("NP1")), true, 
						     true);
	m_canvas->addNonTerminal(NP1);

	HNonTerminalNode *AP = new HNonTerminalNode(106, NP1->getID_D(), SetOfMonads(10,11), 
						    CreateLB(wxT("11")), true, 
						    CreateLB(wxT("AP")), true);
	m_canvas->addNonTerminal(AP);

	m_canvas->addTerminal(new HTerminalNode(1, PP->getID_D(), 
						SetOfMonads(1), 
						CreateBSLLB(wxT("AS"), wxT("DEF"), wxT(""), -1), true,
						CreateLB2(wxT("Unter"), wxT("prep")), true));
	m_canvas->addTerminal(new HTerminalNode(2, PP->getID_D(), 
						SetOfMonads(2), 
						CreateLB2(wxT("ASD1"), wxT("ds1")), true,
						CreateLB2(wxT("anderem"), wxT("adj")), true));
	m_canvas->addTerminal(new HTerminalNode(3, VP1->getID_D(), 
						SetOfMonads(3), 
						CreateLB2(wxT("2321"), wxT("ds")), true,
						CreateLB2(wxT("wurde"), wxT("verb")), true));
	m_canvas->addTerminal(new HTerminalNode(4, NP0->getID_D(), 
						SetOfMonads(4), 
						CreateLB2(wxT("21"), wxT("323")), true,
						CreateLB(wxT("ein")), true));
	m_canvas->addTerminal(new HTerminalNode(5, NP0->getID_D(), 
						SetOfMonads(5), 
						CreateLB2(wxT("dfg"), wxT("DDE")), true,
						CreateLB(wxT("Beteiligungsmodell")), true));
	m_canvas->addTerminal(new HTerminalNode(6, VP1->getID_D(), 
						SetOfMonads(6), 
						CreateLB2(wxT("gfr"), wxT("er")), true,
						CreateLB(wxT("geschaffen")), true));
	m_canvas->addTerminal(new HTerminalNode(7, NIL,
						SetOfMonads(7), 
						CreateLB2(wxT("231"), wxT("FFE")), true,
						CreateLB(wxT(",")), true));
	m_canvas->addTerminal(new HTerminalNode(8, S1->getID_D(), 
						SetOfMonads(8), 
						CreateLB2(wxT("2332"), wxT("DAS")), true,
						CreateLB(wxT("das")), true));
	m_canvas->addTerminal(new HTerminalNode(9, S1->getID_D(), 
						SetOfMonads(9), 
						CreateLB2(wxT("DUS"), wxT("DES")), true,
						CreateLB(wxT("heute")), true));
	m_canvas->addTerminal(new HTerminalNode(10, AP->getID_D(), 
						SetOfMonads(10), 
						CreateLB2(wxT("ASD"), wxT("ASD")), true,
						CreateLB(wxT("insgesamt")), true, true));
	m_canvas->addTerminal(new HTerminalNode(11, AP->getID_D(), 
						SetOfMonads(11), 
						CreateLB2(wxT("asd"), wxT("ds")), true,
						CreateLB(wxT("17")), true, true));
	m_canvas->addTerminal(new HTerminalNode(12, NP1->getID_D(), 
						SetOfMonads(12), 
						CreateLB2(wxT("23!"), wxT("ds!")), true,
						CreateLB2(wxT("Gesellschaftler"), wxT("noun")), true));
	m_canvas->addTerminal(new HTerminalNode(13, S1->getID_D(), 
						SetOfMonads(13), 
						CreateLB(wxT("ds")), true,
						CreateLB(wxT("umfasst")), true));
	m_canvas->addTerminal(new HTerminalNode(14, NIL,
						SetOfMonads(14), 
						CreateLB2(wxT("12"), wxT("321")), true,
						CreateLB(wxT(".")), true));


	

	
}

LayoutBox *MyFrame::CreateLB(const wxString& s)
{
	wxClientDC dc(this);
	return new SimpleLayoutBox(&dc, s, m_canvas->GetViewMetrics(), true);
}

BorderedStackedLatinLayoutBox *MyFrame::CreateBSLLB4(const wxString& s1, const wxString& s2, const wxString& s3, const wxString& s4, int nLineAfterWhichToAddSeparator)
{
	wxClientDC dc(this);
	wxstringvec_t strings;
	strings.push_back(s1);
	if (!s2.IsEmpty()) {
		strings.push_back(s2);
	}

	if (!s3.IsEmpty()) {
		strings.push_back(s3);
	}
	
	if (!s4.IsEmpty()) {
		strings.push_back(s4);
	}
	
	return new BorderedStackedLatinLayoutBox(&dc, strings, m_canvas->GetViewMetrics(), *wxBLACK, *wxBLUE, true, false, nLineAfterWhichToAddSeparator);
}

BorderedStackedLatinLayoutBox *MyFrame::CreateBSLLB(const wxString& s1, const wxString& s2, const wxString& s3, int nLineAfterWhichToAddSeparator)
{
	wxClientDC dc(this);
	wxstringvec_t strings;
	strings.push_back(s1);
	if (!s2.IsEmpty()) {
		strings.push_back(s2);
	}

	if (!s3.IsEmpty()) {
		strings.push_back(s3);
	}
	
	return new BorderedStackedLatinLayoutBox(&dc, strings, m_canvas->GetViewMetrics(), *wxBLACK, *wxBLUE, true, false, nLineAfterWhichToAddSeparator);
}


LayoutBox *MyFrame::CreateLB2(const wxString& s1, const wxString& s2)
{
	wxClientDC dc(this);
	wxstringvec_t strings;
	strings.push_back(s1);
	strings.push_back(s2);
	
	return new LayoutBox(&dc, strings, m_canvas->GetViewMetrics(), false, false, true);
}

void MyFrame::DeleteLayout()
{
}
