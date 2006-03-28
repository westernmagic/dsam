/**********************
 *
 * File:		GrSDIDiagram.cpp
 * Purpose: 	Diagram class Simulation Design Interface.
 * Comments:	Revised from Julian Smart's Ogledit/doc.h
 * Author:		L.P.O'Mard
 * Created:		13 Nov 2002
 * Updated:		
 * Copyright:	(c) 2002, CNBH, University of Essex
 *
 **********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#ifdef USE_WX_OGL

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

#include <wx/tokenzr.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "UtDatum.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "GrSDIBaseShapes.h"
#include "GrSDIShapes.h"
#include "GrSDIEvtHandler.h"
#include "GrSDIDiagram.h"
#include "GrSDICanvas.h"
#include "UtSSSymbols.h"
#include "UtSSParser.h"
#include "UtAppInterface.h"
#include "UtSimScript.h"

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
/****************************** Constructor ***********************************/
/******************************************************************************/

SDIDiagram::SDIDiagram(void)
{
	ok = false;
	loadIDsFromFile = false;
	x = DIAGRAM_DEFAULT_INITIAL_X;
	y = DIAGRAM_DEFAULT_INITIAL_Y;
	direction = 1;
	xScale = 1.0;
	yScale = 1.0;
	simProcess = NULL;

}

/******************************************************************************/
/****************************** AddShape **************************************/
/******************************************************************************/

/*
 * My Add shape method.
 */

void
SDIDiagram::AddShape(wxShape *shape)
{
	wxDiagram::AddShape(shape);
	SDIEvtHandler *myHandler = (SDIEvtHandler *) shape->GetEventHandler();
	if (myHandler->pc) {
		myHandler->ResetLabel();
		wxClientDC dc(shape->GetCanvas());
		shape->FormatText(dc, (char*) (const char *) myHandler->label);
		shape->GetCanvas()->PrepareDC(dc);
	}

}

/******************************************************************************/
/****************************** CreateLoadShape *******************************/
/******************************************************************************/

/*
 * When 'pc' is NULL, a line shape is being set up.
 */

wxShape *
SDIDiagram::CreateLoadShape(DatumPtr pc, wxClassInfo *shapeInfo,
  wxBrush *brush)
{
	bool	lineShape = (!pc);
	wxShape *shape = CreateBasicShape(shapeInfo, (lineShape)? -1:
	  pc->classSpecifier, brush);

	if (!lineShape) {
		SDIEvtHandler *myHandler = (SDIEvtHandler *) shape->GetEventHandler();
		myHandler->pc = pc;
		shape->SetSize(DIAGRAM_DEFAULT_SHAPE_WIDTH,
		  DIAGRAM_DEFAULT_SHAPE_HEIGHT);
		pc->clientData = shape;
		if (pc->type == PROCESS)
			pc->data->clientData = shape;
	}

	AddShape(shape);
	wxClientDC dc(shape->GetCanvas());
	shape->GetCanvas()->PrepareDC(dc);
	shape->Move(dc, x, y);
	shape->Show(TRUE);
	return(shape);

}

/******************************************************************************/
/****************************** DrawSimShapes *********************************/
/******************************************************************************/

void
SDIDiagram::DrawSimShapes()
{
	double	boxWidth, boxHeight, totalWidth;
	DatumPtr	pc;
	ModulePtr	module;
	wxShape		*shape;
	SDICanvas	*canvas = (SDICanvas *) GetCanvas();

	if ((pc = GetSimulation_ModuleMgr(simProcess)) == NULL)
		return;
	while (pc) {
		shape = NULL;
		switch (pc->type) {
		case PROCESS: {
			module = pc->data->module;
			shape = CreateLoadShape(pc, canvas->GetClassInfo(module->
			  classSpecifier), (module->onFlag)? DIAGRAM_ENABLED_BRUSH:
			    DIAGRAM_DISENABLED_BRUSH);
				shape->GetBoundingBoxMax(&boxWidth, &boxHeight);
				totalWidth = boxWidth + DIAGRAM_DEFAULT_X_SEPARATION;
				x += direction * totalWidth;
				if (((direction > 0) && (x + totalWidth) > shape->GetCanvas()->
				  GetClientSize().GetWidth()) || ((direction < 0) && (x -
				  totalWidth) < 0)) {
					direction = - direction;
					y += DIAGRAM_DEFAULT_SHAPE_HEIGHT +
					DIAGRAM_DEFAULT_Y_SEPARATION;
				}
			break; }
		case REPEAT:
		case RESET:
			shape = CreateLoadShape(pc, canvas->GetClassInfo(
			  CONTROL_MODULE_CLASS), DIAGRAM_ENABLED_BRUSH);
			break;
		case STOP: {
			DatumPtr	ppc = pc;
			while (ppc && (ppc->type != PROCESS))
				ppc = ppc->previous;
			shape = (wxShape *) ppc->clientData;
			pc->clientData = shape;
			break; }
		default:
			wxLogError("SDIDiagram::DrawSimShapes: datum type %d not "
			  "implemented.\n", pc->type);
		} /* switch */
		pc = pc->next;
	}
}

/******************************************************************************/
/****************************** AddLineShape **********************************/
/******************************************************************************/

wxShape *
SDIDiagram::AddLineShape(wxShape *fromShape, wxShape *toShape, int lineType)
{
	wxShape *shape = CreateBasicShape(CLASSINFO(SDILineShape), lineType,
	  wxRED_BRUSH);
	SDILineShape *lineShape = (SDILineShape *) shape;
	switch (lineType) {
	case REPEAT:
		lineShape->MakeLineControlPoints(4);
		lineShape->AddArrow(ARROW_ARROW, ARROW_POSITION_MIDDLE,
		  DIAGRAM_ARROW_SIZE, 0.0, DIAGRAM_ARROW_TEXT);
		break;
	default:
		lineShape->MakeLineControlPoints(2);
		lineShape->AddArrow(ARROW_ARROW, ARROW_POSITION_END,
		  DIAGRAM_ARROW_SIZE, 0.0, DIAGRAM_ARROW_TEXT);
	} /* switch */
	AddShape(shape);
	fromShape->AddLine(lineShape, toShape);
	shape->Show(TRUE);

	wxClientDC dc(shape->GetCanvas());
	shape->GetCanvas()->PrepareDC(dc);
	// It won't get drawn properly unless you move both
	// connected images
	fromShape->Move(dc, fromShape->GetX(), fromShape->GetY());
	toShape->Move(dc, toShape->GetX(), toShape->GetY());
	return(shape);

}

/******************************************************************************/
/****************************** DrawDefaultConnection *************************/
/******************************************************************************/

/*
 * This routine draws all a line to the next datum which is not type "STOP".
 */

void
SDIDiagram::DrawDefaultConnection(DatumPtr pc, wxShape *shape)
{
	DatumPtr 	toPc;

	for (toPc = pc->next; toPc && (toPc->type == STOP); toPc = toPc->
	  next)
		;
	if (toPc)
		AddLineShape(shape, (wxShape *) toPc->clientData, -1);

}

/******************************************************************************/
/****************************** DrawSimConnections ****************************/
/******************************************************************************/

/*
 * This routine draws all standard connections by going through the data
 * output connection lists.
 * A "for" loop must be used here and not "while" because "continue" is used
 * and we need pc = pc->next to always be executed.
 * This routine needs the process 'clientData' field to be set.
 */

void
SDIDiagram::DrawSimConnections(void)
{
	DatumPtr	pc, toPc;
	DynaBListPtr labelBList;

	if ((pc = GetSimulation_ModuleMgr(simProcess)) == NULL)
		return;
	labelBList = ((SimScriptPtr) simProcess->module->parsPtr)->labelBList;
	for (; pc != NULL; pc = pc->next) {
		if (pc->type == STOP)
			continue;
		wxShape *fromShape = (wxShape *) (pc->clientData);
		switch (pc->type) {
		case PROCESS: 
			if (pc->u.proc.outputList) {
				for (DynaListPtr p = pc->u.proc.outputList; p != NULL; p = p->
				  next) {
					toPc = (DatumPtr) FindElement_Utility_DynaBList(labelBList,
					  CmpProcessLabel_Utility_Datum, (char *) p->data)->data;
					AddLineShape(fromShape, (wxShape *) toPc->clientData, -1);
				}
			} else {
				for (toPc = pc->next; toPc && (toPc->type == STOP); toPc =
				  toPc->next)
					;
				if (!toPc)
					continue;
				switch (toPc->type) {
				case RESET:
				case REPEAT:
					AddLineShape(fromShape, (wxShape *) toPc->clientData, -1);
					break;
				default:
					EarObjRefPtr	p;
					for (p = pc->data->customerList; p != NULL; p = p->next)
						AddLineShape(fromShape, (wxShape *) p->earObject->
						  clientData, -1);
				} /* switch */
			}
			break;
		case REPEAT: {
			int	level = 0;
			for (toPc = pc->next; (toPc->type != STOP) || level; toPc = toPc->
			  next)
				switch (toPc->type) {
				case REPEAT:
					level++;
					break;
				case STOP:
					level--;
					break;
				default:
					;
				} /* switch */
			AddLineShape(fromShape, (wxShape *) toPc->clientData, REPEAT);
			DrawDefaultConnection(pc, fromShape);
			break; }
		case RESET:
			DrawDefaultConnection(pc, fromShape);
			break;
		default:
			;
		} /* switch */
	}

}

/******************************************************************************/
/****************************** DrawSimulation ********************************/
/******************************************************************************/

void
SDIDiagram::DrawSimulation(void)
{
	if (ok)
		return;
	DrawSimShapes();
	DrawSimConnections();
	ok = true;

}

/******************************************************************************/
/****************************** CreateBasicShape ******************************/
/******************************************************************************/

wxShape *
SDIDiagram::CreateBasicShape(wxClassInfo *shapeInfo, int type, wxBrush *brush)
{
	SDIShape *theShape = (SDIShape *) shapeInfo->CreateObject();
	if (!loadIDsFromFile)
		theShape->AssignNewIds();
	theShape->SetEventHandler(new SDIEvtHandler(theShape, theShape, wxString(
	  ""), type));
	theShape->SetCentreResize(false);
	theShape->SetPen(wxBLACK_PEN);
	theShape->SetBrush(brush);
	theShape->GetFont()->SetPointSize((int) (SHAPE_DEFAULT_FONT_POINTSIZE *
	  xScale));
	return(theShape);

}

/******************************************************************************/
/****************************** VerifyDiagram *********************************/
/******************************************************************************/

/*
 * This function checks that the loaded diagram corresponds with the simulation.
 * It returns 'false' if it finds any discrepancies.
 * Nothing should be selected at this point, so there will be no need to 
 * run the 'UnselectAllShapes' routine, so that 'control' shapes are not in
 * the shape list.
 */

bool
SDIDiagram::VerifyDiagram(void)
{
	static const char *funcName = "SDIDiagram::VerifyDiagram";
	int		numDiagConnections = 0, numSimConnections = 0;
	DatumPtr	pc, toPc;
	EarObjectPtr	fromProcess, toProcess;
	EarObjRefPtr p;
	wxNode *node = GetShapeList()->GetFirst();

	// Check processes exist for each shape line, and that the connection
	// exists in the simulation
	while (node) {
		wxShape *shape = (wxShape *) node->GetData();
		if (shape->IsKindOf(CLASSINFO(wxLineShape))) {
			wxLineShape *lineShape = (wxLineShape *) shape;
			wxShape *fromShape = lineShape->GetFrom();
			wxShape *toShape = lineShape->GetTo();
			if (!fromShape || !toShape) {
				wxLogWarning("%s: Diagram line is not connected to a valid "
				  "process.", funcName);
				return (false);
			}
			if ((SHAPE_PC(fromShape)->type == PROCESS) && (SHAPE_PC(toShape)->
			  type == PROCESS)) {
				fromProcess = SHAPE_PC(fromShape)->data;
				toProcess = SHAPE_PC(toShape)->data;
				for (p = fromProcess->customerList; (p != NULL) && (p->
				  earObject->handle != toProcess->handle); p = p->next)
					;
				if (!p) {
					wxLogWarning("%s: Diagram line does not correspond to a "
					  "simulation connection.", funcName);
					return(false);
				}
			}
			numDiagConnections++;
		}
		node = node->GetNext();
	}
	// Check if there are any undrawn connections or shapes
	if ((pc = GetSimulation_ModuleMgr(simProcess)) == NULL)
		return(false);
	while (pc) {
		if (pc->type == PROCESS) {
			if (!pc->clientData && pc->data) {
				wxLogWarning("%s: Process has no description (step %d, label "
				  "%s'.", funcName, pc->stepNumber, pc->label);
				return (false);
			}
			for (p = pc->data->customerList; (p != NULL); p = p->next)
				numSimConnections++;
		} else {
			for (toPc = pc->next; toPc && (toPc->type == STOP); toPc =
			  toPc->next)
				;
			if (toPc)
				numSimConnections++;
		}
		pc = pc->next;
	}
	if (numDiagConnections != numSimConnections) {
		wxLogWarning("%s: The number of diagram lines (%d) does not "
		  "correspond\nto the number of simulation connections (%d).", funcName,
		  numDiagConnections, numSimConnections);
		return(false);
	}
	return(TRUE);

}

/******************************************************************************/
/****************************** RedrawShapeLabel ******************************/
/******************************************************************************/

// Temporary

void
SDIDiagram::RedrawShapeLabel(wxShape *shape)
{
	wxClientDC dc(shape->GetCanvas());
	shape->GetCanvas()->PrepareDC(dc);
    SDIEvtHandler *myHandler = (SDIEvtHandler *) shape->GetEventHandler();

	shape->FormatText(dc, (const char *) myHandler->label);
	shape->Draw(dc);

}

/******************************************************************************/
/****************************** UnselectAllShapes *****************************/
/******************************************************************************/

/*
 * This routine unselects all shapes.
 * It returns 'true' if the canvas needs to be redrawn.
 */

bool
SDIDiagram::UnselectAllShapes(void)
{
	bool	redraw = false;
	wxShape	*shape;
	wxNode *node = GetShapeList()->GetFirst();
	wxClientDC	dc(GetCanvas());
	GetCanvas()->PrepareDC(dc);

	while (node) {
		shape = (wxShape *) node->GetData();
		if (shape->Selected()) {
			shape->Select(false, &dc);
			redraw = true;
		}
		node = node->GetNext();
	}
	return(redraw);

}

/******************************************************************************/
/****************************** Rescale ***************************************/
/******************************************************************************/

/*
 * This routine redraws the diagram using a new scale;
 * If there is a selection, then there are additional 'control' shapes added
 * to the diagram shape list - first unselect everything to simplify code.
 */

void
SDIDiagram::Rescale(double theXScale, double theYScale)
{
	double	rescaleX, rescaleY;
	wxShape *shape;
	wxNode *node = GetShapeList()->GetFirst();

	UnselectAllShapes();
	rescaleX = theXScale / xScale;
	rescaleY = theYScale / yScale;
	xScale = theXScale;
	yScale = theYScale;
	while (node) {
		shape = (wxShape *) node->GetData();
		shape->SetSize(DIAGRAM_DEFAULT_SHAPE_WIDTH * xScale,
			  DIAGRAM_DEFAULT_SHAPE_HEIGHT * yScale);
		shape->SetX(shape->GetX() * rescaleX);
		shape->SetY(shape->GetY() * rescaleY);
		shape->GetFont()->SetPointSize((int) (SHAPE_DEFAULT_FONT_POINTSIZE *
		  xScale));
		if (!shape->IsKindOf(CLASSINFO(wxLineShape)))
			RedrawShapeLabel(shape);
		wxClientDC dc(shape->GetCanvas());
		shape->GetCanvas()->PrepareDC(dc);
		shape->Move(dc, shape->GetX(), shape->GetY());
		node = node->GetNext();
	}
	
}

#endif /* USE_WX_OGL */
