/**********************
 *
 * File:		GrSDIFrame.cpp
 * Purpose: 	Frame clase for Simulation Design Interface .
 * Comments:	
 * Author:		L.P.O'Mard
 * Created:		11 Nov 2002
 * Updated:		
 * Copyright:	(c) 2002, CNBH, University of Essex
  *
 **********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#ifdef HAVE_WX_OGL_OGL_H

#ifdef __GNUG__
// #pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/colordlg.h>
#include <wx/docview.h>

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in wx_setup.h!
#endif

#include "GeCommon.h"
#include "GrSimMgr.h"
#include "GrSDIPalette.h"
#include "GrSDICanvas.h"
#include "GrSDIFrame.h"

/******************************************************************************/
/****************************** Bitmaps ***************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Methods (Subroutines) *************************/
/******************************************************************************/

IMPLEMENT_CLASS(SDIFrame, wxDocChildFrame)

BEGIN_EVENT_TABLE(SDIFrame, wxDocChildFrame)
    EVT_SIZE(SDIFrame::OnSize)
    EVT_CLOSE(SDIFrame::OnCloseWindow)
END_EVENT_TABLE()

/******************************************************************************/
/*************************** Constructor **************************************/
/******************************************************************************/

SDIFrame::SDIFrame(wxDocument *doc, wxView *view, wxFrame *frame,
  const wxString& title, const wxPoint& pos, const wxSize& size, long type):
  wxDocChildFrame(doc, view, frame, -1, title, pos, size, type)
{
	canvas = NULL;

#	ifdef __WXMSW__
	subframe->SetIcon(wxString(isCanvas ? "chrt_icn" : "notepad_icn"));
#	endif
	// Make a menubar
	wxMenu *file_menu = new wxMenu;

	file_menu->Append(wxID_CLOSE, "&Close");

	file_menu->AppendSeparator();
	file_menu->Append(wxID_PRINT, "&Print...");
	file_menu->Append(wxID_PRINT_SETUP, "Print &Setup...");
	file_menu->Append(wxID_PREVIEW, "Print Pre&view");

	wxMenu *editMenu = (wxMenu *) NULL;

	editMenu = new wxMenu;
	editMenu->Append(wxID_UNDO, "&Undo");
	editMenu->Append(wxID_REDO, "&Redo");
	editMenu->AppendSeparator();
	editMenu->Append(SDIFRAME_CUT, "&Cut last segment");

	doc->GetCommandProcessor()->SetEditMenu(editMenu);
	doc->GetCommandProcessor()->Initialize();

	wxMenu *help_menu = new wxMenu;
	help_menu->Append(SDIFRAME_ABOUT, "&About");

	wxMenuBar *menu_bar = new wxMenuBar;

	menu_bar->Append(file_menu, "&File");
	menu_bar->Append(editMenu, "&Edit");
	menu_bar->Append(help_menu, "&Help");

	//// Associate the menu bar with the frame
	SetMenuBar(menu_bar);
	
	//Set Palette
	palette = new EditorToolPalette(this, wxPoint(0, 0), wxSize(-1,-1),
	  wxTB_VERTICAL);

	Centre(wxBOTH);

}

/******************************************************************************/
/*************************** OnSize *******************************************/
/******************************************************************************/

void
SDIFrame::OnSize(wxSizeEvent& event)
{
  if (canvas && palette)
  {
    int cw, ch;
    GetClientSize(&cw, &ch);
    int paletteX = 0;
    int paletteY = 0;
    int paletteW = 30;
    int paletteH = ch;
    int canvasX = paletteX + paletteW;
    int canvasY = 0;
    int canvasW = cw - paletteW;
    int canvasH = ch;
    
    palette->SetSize(paletteX, paletteY, paletteW, paletteH);
    canvas->SetSize(canvasX, canvasY, canvasW, canvasH);
  }
}

/******************************************************************************/
/*************************** OnCloseWindow ************************************/
/******************************************************************************/

void
SDIFrame::OnCloseWindow(wxCloseEvent& event)
{
	wxDocChildFrame::OnCloseWindow(event);
	if (!event.GetVeto())
		wxOGLCleanUp();

}

#endif /* HAVE_WX_OGL_H */
