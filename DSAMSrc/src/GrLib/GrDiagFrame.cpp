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
  const wxSize& size): wxFrame(parent, -1, title, pos, size)
{

	fileMenu = NULL;

	// Make text window for diagnostics
	int width, height;
	parent->GetClientSize(&width, &height);
	diagnosticsText = new wxTextCtrl(this, -1, "DSAM Diagnostics "
	  "Window\n", wxPoint(0, 0), wxSize(width, height), wxTE_MULTILINE);
	diagnosticsText->SetFont(*diagnosticsWindowFont);

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

