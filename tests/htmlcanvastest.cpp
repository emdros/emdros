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


#include <layout.h>
#include <qdxml.h>

#include <fstream>

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
    HTMLCanvas *m_canvas;

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


// `Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // Create the main application window
    MyFrame *frame = new MyFrame(wxT("HTML sample"),
                                 wxPoint(50, 50), wxSize(550, 340));

    // Show it and tell the application that it's our main window
    frame->Show(true);
    SetTopWindow(frame);

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

    ViewMetrics *pViewMetrics = new ViewMetrics(1, wxMM_TEXT, false, false, 100);
    pViewMetrics->setILM(0, new InterlinearLineMetrics(wxT("Gentium"), true, wxFONTENCODING_ISO8859_1, *wxBLACK, wxFONTFAMILY_DEFAULT, 1.2));

    m_canvas = new HTMLCanvas( this , wxPoint(0,0) , wxDefaultSize, 0 , pViewMetrics);
    m_canvas->SetScrollbars( 10, 10, 100, 240 );
    m_canvas->ChangeMagnification(100);

    m_canvas->m_pViewMetrics = pViewMetrics;
    m_canvas->InitializeViewMetrics();

    CreateLayout();
    // m_canvas->OnUpdate();
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
	std::string sHtml = "<?xml version='1.0' encoding='utf-8' ?>\n"
		"<html><body>"
		"<h1>This is a heading<sup>1</sup></h1> and this is some text right after the h1 heading, outside any p element."
		"<h2>And so is this<sup>2</sup></h2> and this is some text right after the h2 heading, outside any p element."
		"<p><hr/><br/><br/><a href=\"#Owl\">This links to 'Owl' below.</a><br/><br/><br/></p>"
		"<p><hr/><br/><br/><a href=\"#Owl2\">This links to the second instance of 'Owl' below.</a><br/><br/><br/></p>"
		"<p>While this is a <a href=\"http://www.emdros.org/\">long-winded<sup>3</sup> para</a>graph -- in which we meet <em>Winnie the <b>Pooh</b> <em>and</em> all</em> his <u>friends and relations</u>, and in which <a name=\"Owl\">Owl </a>is compelled to say he was wrong, which was much against his will.<sup>squared</sup>.<hr/></p>"
		"<p>In fact, <a name=\"Owl2\">Owl</a> never came back to the forest, even, after this humiliation.</p>"
		"<hr/>"
		"<br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/>"
		"<hr/>"
		"<br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/><br/>"
		"<hr/>"
		"</body></html>";

	
	std::istringstream istr(sHtml);

	wxClientDC dc(m_canvas);

	QDHTMLHandler dh(m_canvas);

	try {
		QDParser parser;
		parser.parse(&dh, &istr);
	} catch (QDException e) {
		std::cerr << "UP200: e = " << e.what() << std::endl;
	}
}

void MyFrame::DeleteLayout()
{
}
