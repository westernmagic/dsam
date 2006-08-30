/**********************
 *
 * File:		GrDiagFrame.cpp
 * Purpose: 	Frame class for diagnostic output.
 * Comments:	
 * Author:		L.P.O'Mard
 * Created:		21 Nov 2002
 * Updated:		
 * Copyright:	(c) 2002, CNBH, University of Essex
 *
 **********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "ExtCommon.h"

#include "GeCommon.h"
#include "GrDiagFrame.h"
#include "GrFonts.h"
#include "GrIPCServer.h"
#include "GrSimMgr.h"

/******************************************************************************/
/****************************** Bitmaps ***************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Event tables *************************************/
/******************************************************************************/

IMPLEMENT_CLASS(DiagFrame, wxFrame)

BEGIN_EVENT_TABLE(DiagFrame, wxFrame)
	EVT_SIZE(DiagFrame::OnSize)
	EVT_CLOSE(DiagFrame::OnCloseWindow)
END_EVENT_TABLE()

/******************************************************************************/
/****************************** Methods (Subroutines) *************************/
/******************************************************************************/

/****************************** Constructor ***********************************/

// Note that for the 'LayoutConstraints' to work properly the panel must be
// set using constraints..

DiagFrame::DiagFrame(wxFrame *parent, const wxString& title, const wxPoint& pos,
  const wxSize& size, long style): wxFrame(parent, -1, title, pos, size, style)
{

	fileMenu = NULL;

	scrolledWindow = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition,
	  wxSize(100,100) );
	scrolledWindow->SetScrollbars( 10, 10, 50, 50 );
	// Make text window for diagnostics

	diagnosticsText = new wxTextCtrl(this, wxID_ANY, wxT("DSAM Diagnostics ")
	  wxT("Window\n"), wxPoint(0,0), wxSize(100,100), wxTE_MULTILINE);
	diagnosticsText->SetFont(*diagnosticsWindowFont);
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
	topSizer->Add(scrolledWindow, 1, wxEXPAND);
	topSizer->Add(diagnosticsText, 0, wxEXPAND);
	SetAutoLayout(true);
	SetSizer(topSizer);

}

/******************************************************************************/
/*************************** OnSize *******************************************/
/******************************************************************************/

void
DiagFrame::OnSize(wxSizeEvent& event)
{
	int cw, ch;
	GetClientSize(&cw, &ch);
	diagnosticsText->SetSize(0, 0, cw, ch);

}

/******************************************************************************/
/*************************** OnCloseWindow ************************************/
/******************************************************************************/

void
DiagFrame::OnCloseWindow(wxCloseEvent& event)
{
	wxGetApp().CloseDiagWindow();

}

/******************************************************************************/
/************************ Subroutines and functions ***************************/
/******************************************************************************/

