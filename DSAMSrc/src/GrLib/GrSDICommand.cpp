/**********************
 *
 * File:		GrSDICommand.cpp
 * Purpose: 	Command class for Simulation Design Interface.
 * Comments:	Revised from Julian Smart's Ogledit/doc.h
 *				Most user interface commands are routed through this, to give
 *				us the Undo/Redo mechanism. If you add more commands, such as
 *				changing the shape colour, you will need to add members to
 *				'remember' what the user applied (for 'Do') and what the
 *				previous state was (for 'Undo').
 *				You can have one member for each property to be changed.
 *				Assume we also have a pointer member wxShape *shape, which is
 *				set to the shape being changed. Let's assume we're changing
 *				the shape colour. Our member for this is shapeColour.
 *				- In 'Do':
 *					o Set a temporary variable 'temp' to the current colour
 *						for 'shape'.
 *					o Change the colour to the new colour.
 *					o Set shapeColour to the _old_ colour, 'temp'.
 *				- In 'Undo':
 *					o Set a temporary variable 'temp' to the current colour
 *						for 'shape'.
 *					o Change the colour to shapeColour (the old colour).
 *					o Set shapeColour to 'temp'.
 *
 *				So, as long as we have a pointer to the shape being changed, we
 *				only need one member variable for each property.
 *
 *				PROBLEM: when an Add shape command is redone, the 'shape'
 *				pointer changes.
 *				Assume, as here, that we keep a pointer to the old shape so we
 *				reuse it when we recreate.
 * Author:		L.P.O'Mard
 * Created:		13 Nov 2002
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

#include <wx/wxexpr.h>
#include <wx/docview.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "UtDatum.h"
#include "UtAppInterface.h"

#include "GrSDIDiagram.h"
#include "GrSDIDoc.h"
#include "GrSDIFrame.h"
#include "GrSDICommand.h"
#include "GrSDIEvtHandler.h"

/******************************************************************************/
/****************************** Bitmaps ***************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Methods (Subroutines) *************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Constructor 1 *********************************/
/******************************************************************************/

/*
 * Implementation of drawing command
 */

SDICommand::SDICommand(char *name, int command, SDIDocument *ddoc,
  wxClassInfo *info, int theProcessType, double xx, double yy, bool sel,
  wxShape *theShape, wxShape *fs, wxShape *ts): wxCommand(TRUE, name)
{
	SetBasic(command, ddoc, theShape);
	fromShape = fs;
	toShape = ts;
	shapeInfo = info;
	processType = theProcessType;
	x = xx;
	y = yy;
	selected = sel;

}

/******************************************************************************/
/****************************** Constructor 2 *********************************/
/******************************************************************************/

SDICommand::SDICommand(char *name, int command, SDIDocument *ddoc,
  wxBrush *backgroundColour, wxShape *theShape): wxCommand(TRUE, name)
{
	SetBasic(command, ddoc, theShape);
	shapeBrush = backgroundColour;

}

/******************************************************************************/
/****************************** Constructor 3 *********************************/
/******************************************************************************/

SDICommand::SDICommand(char *name, int command, SDIDocument *ddoc,
  const wxString& lab, wxShape *theShape): wxCommand(TRUE, name)
{
	SetBasic(command, ddoc, theShape);
	shapeLabel = lab;

}

/******************************************************************************/
/****************************** Destructor ************************************/
/******************************************************************************/

SDICommand::~SDICommand(void)
{
	if (shape && deleteShape) {
		shape->SetCanvas(NULL);
		delete shape;
	}

}

/******************************************************************************/
/****************************** SetBasic **************************************/
/******************************************************************************/

void
SDICommand::SetBasic(int command, SDIDocument *ddoc, wxShape *theShape)
{
	doc = ddoc;
	cmd = command;
	shape = theShape;
	x = 0.0;
	y = 0.0;
	selected = FALSE;
	shapePen = NULL;
	deleteShape = FALSE;
	fromShape = NULL;
	toShape = NULL;
	shapeInfo = NULL;
	shapeBrush = NULL;
	processType = -1;

}

/******************************************************************************/
/****************************** ConnectProcesses ******************************/
/******************************************************************************/

/*
 * This routine connects two specified processes and inserts them into the
 * simulation.
 * It also inserts the instructions into the simulation.
 */

void
SDICommand::ConnectProcesses(void)
{
	static const char *funcName = "SDICommand::ConnectProcesses";

	SDIEvtHandler *toHandler = (SDIEvtHandler *) toShape->GetEventHandler();
	SDIEvtHandler *fromHandler = (SDIEvtHandler *) fromShape->GetEventHandler();
	DatumPtr	toPc = toHandler->pc, fromPc = fromHandler->pc;

	if (!fromPc || !toPc) {
		wxLogError("%s: Both processes must be set before a connection\n"
		  "can be made.", funcName);
		return;
	}
	if (!ConnectOutSignalToIn_EarObject(fromPc->data, toPc->data)) {
		wxLogError("%s: Could not connect processes.", funcName);
		return;
	}

	DatumPtr	*simPtr = GetSimPtr_AppInterface();

	if (!DATUM_IN_SIMULATION(fromPc)) {
		if (DATUM_IN_SIMULATION(toPc))
			InsertInst_Utility_Datum(simPtr, toPc, fromPc);
		else {
			InsertInst_Utility_Datum(simPtr, NULL, fromPc);
			AppendInst_Utility_Datum(simPtr, fromPc, toPc);
		}
	} else {
		if (!DATUM_IN_SIMULATION(toPc))
			AppendInst_Utility_Datum(simPtr, fromPc, toPc);
	}

}

/******************************************************************************/
/****************************** Do ********************************************/
/******************************************************************************/

bool
SDICommand::Do(void)
{
	switch (cmd) {
	case SDIFRAME_CUT: {
		if (shape) {
			deleteShape = TRUE;

			shape->Select(FALSE);

			// Generate commands to explicitly remove each connected line.
			RemoveLines(shape);

			doc->GetDiagram()->RemoveShape(shape);
			if (shape->IsKindOf(CLASSINFO(wxLineShape))) {
				wxLineShape *lineShape = (wxLineShape *)shape;
				fromShape = lineShape->GetFrom();
				toShape = lineShape->GetTo();
				DisconnectProcesses();
			} else {
				SDIEvtHandler *myHandler = (SDIEvtHandler *) shape->
				  GetEventHandler();
				myHandler->FreeInstruction();
			}
			shape->Unlink();

			doc->Modify(TRUE);
			doc->UpdateAllViews();
		}

		break; }
	case SDIFRAME_ADD_SHAPE: {
		wxShape *theShape = NULL;
		if (shape)
			theShape = shape; // Saved from undoing the shape
		else {
			theShape = (wxShape *)shapeInfo->CreateObject();
			theShape->AssignNewIds();
			theShape->SetEventHandler(new SDIEvtHandler(theShape, theShape,
			  wxString(""), processType));
			theShape->SetCentreResize(FALSE);
			theShape->SetPen(wxBLACK_PEN);
			theShape->SetBrush(wxCYAN_BRUSH);
			theShape->SetSize(60, 60);
		}
		doc->GetDiagram()->AddShape(theShape);
		theShape->Show(TRUE);

		wxClientDC dc(theShape->GetCanvas());
		theShape->GetCanvas()->PrepareDC(dc);

		theShape->Move(dc, x, y);

		shape = theShape;
		deleteShape = FALSE;

		doc->Modify(TRUE);
		doc->UpdateAllViews();
		break; }
	case SDIFRAME_ADD_LINE: {
		wxShape *theShape = NULL;
		if (shape)
			theShape = shape; // Saved from undoing the line
		else {
			ConnectProcesses();
			theShape = (wxShape *)shapeInfo->CreateObject();
			theShape->AssignNewIds();
			theShape->SetEventHandler(new SDIEvtHandler(theShape, theShape,
			  wxString("")));
			theShape->SetPen(wxBLACK_PEN);
			theShape->SetBrush(wxRED_BRUSH);

			wxLineShape *lineShape = (wxLineShape *)theShape;

			// Yes, you can have more than 2 control points, in which case
			// it becomes a multi-segment line.
			lineShape->MakeLineControlPoints(2);
			lineShape->AddArrow(ARROW_ARROW, ARROW_POSITION_END, 10.0, 0.0,
			  "Normal arrowhead");
		}

		doc->GetDiagram()->AddShape(theShape);

		fromShape->AddLine((wxLineShape *)theShape, toShape);

		theShape->Show(TRUE);

		wxClientDC dc(theShape->GetCanvas());
		theShape->GetCanvas()->PrepareDC(dc);

		// It won't get drawn properly unless you move both
		// connected images
		fromShape->Move(dc, fromShape->GetX(), fromShape->GetY());
		toShape->Move(dc, toShape->GetX(), toShape->GetY());

		shape = theShape;
		deleteShape = FALSE;

		// Create processes if necessary and connect them
		
		doc->Modify(TRUE);
		doc->UpdateAllViews();
		break; }
	case SDIFRAME_CHANGE_BACKGROUND_COLOUR: {
		if (shape) {
			wxClientDC dc(shape->GetCanvas());
			shape->GetCanvas()->PrepareDC(dc);

			wxBrush *oldBrush = shape->GetBrush();
			shape->SetBrush(shapeBrush);
			shapeBrush = oldBrush;
			shape->Draw(dc);

			doc->Modify(TRUE);
			doc->UpdateAllViews();
		}
		break; }
	case SDIFRAME_EDIT_PROCESS: {
		if (shape) {
			SDIEvtHandler *myHandler = (SDIEvtHandler *) shape->GetEventHandler(
			  );
			wxString oldLabel(myHandler->label);
			myHandler->label = shapeLabel;
			shapeLabel = oldLabel;
			
			if (!myHandler->pc) {
				myHandler->InitInstruction();
				myHandler->EditInstruction();
			} else
				myHandler->EditInstruction();

			wxClientDC dc(shape->GetCanvas());
			shape->GetCanvas()->PrepareDC(dc);

			shape->FormatText(dc, (char*) (const char*) myHandler->label);
			shape->Draw(dc);

			doc->Modify(TRUE);
			doc->UpdateAllViews();
		}
		break; }
	}
	return TRUE;

}

/******************************************************************************/
/****************************** DisconnectProcesses ***************************/
/******************************************************************************/

/*
 * This routine connects two specified processes and inserts them into the
 * simulation.
 */

void
SDICommand::DisconnectProcesses(void)
{
	static const char *funcName = "SDICommand::DisconnectProcesses";

	SDIEvtHandler *toHandler = (SDIEvtHandler *) toShape->GetEventHandler();
	SDIEvtHandler *fromHandler = (SDIEvtHandler *) fromShape->GetEventHandler();
	DatumPtr	toPc = toHandler->pc, fromPc = fromHandler->pc;

	if (!fromPc || !toPc) {
		wxLogError("%s: Both processes must be set before a connection\n"
		  "can be made.", funcName);
		return;
	}
	DisconnectOutSignalFromIn_EarObject(fromPc->data, toPc->data);

	printf("SDICommand::Undo: need to uninstall processes here too.\n");

	DisconnectSim_Utility_Datum(fromPc, toPc);

}

/******************************************************************************/
/****************************** Undo ******************************************/
/******************************************************************************/

bool
SDICommand::Undo(void)
{
	switch (cmd) {
	case SDIFRAME_CUT: {
		if (shape) {
			doc->GetDiagram()->AddShape(shape);
			shape->Show(TRUE);

			if (shape->IsKindOf(CLASSINFO(wxLineShape))) {
				wxLineShape *lineShape = (wxLineShape *)shape;
				fromShape->AddLine(lineShape, toShape);
				ConnectProcesses();
			}
			if (selected)
				shape->Select(TRUE);

			deleteShape = FALSE;
		}
		doc->Modify(TRUE);
		doc->UpdateAllViews();
		break; }
	case SDIFRAME_ADD_SHAPE: {
		if (shape) {
			wxClientDC dc(shape->GetCanvas());
			shape->GetCanvas()->PrepareDC(dc);

			shape->Select(FALSE, &dc);
			doc->GetDiagram()->RemoveShape(shape);
			shape->Unlink();
			deleteShape = TRUE;
		}
		doc->Modify(TRUE);
		doc->UpdateAllViews();
		break; }
	case SDIFRAME_ADD_LINE: {
		if (shape) {
			wxClientDC dc(shape->GetCanvas());
			shape->GetCanvas()->PrepareDC(dc);
			DisconnectProcesses();
			shape->Select(FALSE, &dc);
			doc->GetDiagram()->RemoveShape(shape);
			shape->Unlink();
			deleteShape = TRUE;
		}
		doc->Modify(TRUE);
		doc->UpdateAllViews();
		break; }
	case SDIFRAME_CHANGE_BACKGROUND_COLOUR: {
		if (shape) {
			wxClientDC dc(shape->GetCanvas());
			shape->GetCanvas()->PrepareDC(dc);

			wxBrush *oldBrush = shape->GetBrush();
			shape->SetBrush(shapeBrush);
			shapeBrush = oldBrush;
			shape->Draw(dc);

			doc->Modify(TRUE);
			doc->UpdateAllViews();
		}
		break; }
	case SDIFRAME_EDIT_PROCESS: {
		if (shape) {
			SDIEvtHandler *myHandler = (SDIEvtHandler *) shape->GetEventHandler(
			  );
			wxString oldLabel(myHandler->label);
			myHandler->label = shapeLabel;
			shapeLabel = oldLabel;

			if (!myHandler->pc) {
				myHandler->InitInstruction();
				myHandler->EditInstruction();
			} else
				myHandler->EditInstruction();

			wxClientDC dc(shape->GetCanvas());
			shape->GetCanvas()->PrepareDC(dc);

			shape->FormatText(dc, (char*) (const char*) myHandler->label);
			shape->Draw(dc);

			doc->Modify(TRUE);
			doc->UpdateAllViews();
		}

		break; }
	}
	return TRUE;

}

/******************************************************************************/
/****************************** RemoveLines ***********************************/
/******************************************************************************/

// Remove each individual line connected to a shape by sending a command.
void
SDICommand::RemoveLines(wxShape *shape)
{
	wxNode *node = shape->GetLines().First();
	while (node) {
		wxLineShape *line = (wxLineShape *)node->Data();
		doc->GetCommandProcessor()->Submit(new SDICommand("Cut", SDIFRAME_CUT,
		  doc, NULL, -1, 0.0, 0.0, line->Selected(), line));

		node = shape->GetLines().First();
	}

}

#endif /* HAVE_WX_OGL_OGL_H */
