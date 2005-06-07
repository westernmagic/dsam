/******************
 *		
 * File:		GrDisplayS.cpp
 * Purpose: 	Display class module.
 * Comments:	It was created by extraction from the original GrSignalDisp
 *				module.
 *				23-06-98 LPO: Introduced display window resizing:
 *				DisplayS::OnSize
 * Author:		L.P.O'Mard
 * Created:		27 Jul 1997
 * Updated:		23 Jun 1998
 * Copyright:	(c) 1998, University of Essex
 *
 ******************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "ExtCommon.h"

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "UtDatum.h"

#include "GrUtils.h"
#include "GrSimMgr.h"
#include "GrLines.h"
#include "GrModParDialog.h"
#include "GrSignalDisp.h"
#include "GrCanvas.h"
#include "GrDisplayS.h"

/******************************************************************************/
/****************************** Bitmaps ***************************************/
/******************************************************************************/

#if defined(__WXGTK__) || defined(__WXMOTIF__)
#include "Bitmaps/dsam.xpm"
#endif

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** DisplayS **************************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Event tables *************************************/
/******************************************************************************/

BEGIN_EVENT_TABLE(DisplayS, wxFrame)
	EVT_MOVE(DisplayS::OnMove)
END_EVENT_TABLE()

/******************************************************************************/
/****************************** Methods ***************************************/
/******************************************************************************/

/****************************** Constructor ***********************************/

DisplayS::DisplayS(wxFrame *frame, SignalDispPtr signalDispPtr): wxFrame(frame,
  -1, (wxChar *)signalDispPtr->title,  wxPoint(signalDispPtr->frameXPos,
  signalDispPtr->frameYPos), wxSize(signalDispPtr->frameWidth, signalDispPtr->
  frameHeight))
{
	wxLayoutConstraints	*c;

	// Load bitmaps and icons
	SetIcon(wxICON(dsam));
	
	mySignalDispPtr = signalDispPtr;

	// Create a new canvas
	canvas = new MyCanvas(this, signalDispPtr);
 	
	c = new wxLayoutConstraints;

	c->left.SameAs(this, wxLeft, 4);
	c->right.SameAs(this, wxRight, 4);
	c->top.SameAs(this, wxTop, 4);
	c->bottom.SameAs(this, wxBottom, 4);

	canvas->SetConstraints(c);
	
	wxGetApp().displays.Add(this);

}

/****************************** Destructor ************************************/

DisplayS::~DisplayS(void)
{
	delete canvas;
	mySignalDispPtr->display = NULL;
	wxGetApp().displays.Remove(this);

}

/****************************** SetDisplayTitle *******************************/

void
DisplayS::SetDisplayTitle(void)
{
	SetTitle((wxChar *) signalDispPtr->title);
	
}

/****************************** OnMove ****************************************/

void
DisplayS::OnMove(wxMoveEvent& event)
{
	wxPoint pos = event.GetPosition();
	GetSignalDispPtr()->frameXPos = pos.x;
	GetSignalDispPtr()->frameYPos = pos.y;
	GetSize(&GetSignalDispPtr()->frameWidth, &GetSignalDispPtr()->frameHeight);

    event.Skip();

}

