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

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "UtDatum.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
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
	x = 60.0;
	y = 60.0;

}

/******************************************************************************/
/****************************** SetShape **************************************/
/******************************************************************************/

wxShape *
SDIDiagram::CreateLoadShape(DatumPtr pc, wxClassInfo *shapeInfo, int type,
  wxBrush *brush)
{
	wxShape *shape = CreateBasicShape(shapeInfo, type, brush);

	SDIEvtHandler *myHandler = (SDIEvtHandler *) shape->GetEventHandler();
	myHandler->pc = pc;
	myHandler->ResetLabel();

	shape->SetSize(DIAGRAM_DEFAULT_SHAPE_WIDTH, DIAGRAM_DEFAULT_SHAPE_HEIGHT);
	AddShape(shape);
	shape->Show(TRUE);
	wxClientDC dc(shape->GetCanvas());
	shape->FormatText(dc, (char*) (const char *) myHandler->label);
	shape->GetCanvas()->PrepareDC(dc);
	shape->Move(dc, x, y);
	x += DIAGRAM_DEFAULT_SHAPE_WIDTH + 20;
	return(shape);

}

/******************************************************************************/
/****************************** DrawSimShapes *********************************/
/******************************************************************************/

void
SDIDiagram::DrawSimShapes(DatumPtr start)
{
	printf("SDIDiagram::DrawSimShapes: Entered\n");
	DatumPtr	pc;
	ModulePtr	module;
	wxShape		*shape;
	SDICanvas	*canvas = (SDICanvas *) GetCanvas();

	if (start == NULL)
		return;
	for (pc = start; pc != NULL; pc = pc->next) {
		shape = NULL;
		switch (pc->type) {
		case PROCESS: {
			module = pc->data->module;
			shape = CreateLoadShape(pc, canvas->GetClassInfo(module->
			  classSpecifier), module->classSpecifier, wxCYAN_BRUSH);
			break; }
		case REPEAT:
			shape = CreateLoadShape(pc, canvas->GetClassInfo(
			  CONTROL_MODULE_CLASS), CONTROL_MODULE_CLASS, wxCYAN_BRUSH);
			break;
		case STOP: {
			DatumPtr	ppc = pc;
			while (ppc && (ppc->type != PROCESS))
				ppc = ppc->previous;
			shape = (wxShape *) ppc->data->clientData;
			break; }
		default:
			printf("SDIDiagram::DrawSimShapes: datum type %d not implemented."
			  "\n", pc->type);
		} /* switch */
		if (shape) {
			if (pc->data)
				pc->data->clientData = shape;
			else
				pc->clientData = shape;
		}
	}
}

/******************************************************************************/
/****************************** CreateLineShape *******************************/
/******************************************************************************/

void
SDIDiagram::AddLineShape(wxShape *fromShape, wxShape *toShape, int lineType)
{
	wxShape *shape = CreateBasicShape(CLASSINFO(wxLineShape), -1, wxRED_BRUSH);
	wxLineShape *lineShape = (wxLineShape *) shape;
	switch (lineType) {
	case REPEAT:
		lineShape->MakeLineControlPoints(4);
		lineShape->AddArrow(ARROW_ARROW, ARROW_POSITION_MIDDLE,
		  DIAGRAM_ARROW_SIZE, 0.0, DIAGRAM_ARROW_TEXT);
		break;
	default:
		lineShape->MakeLineControlPoints(2);
		lineShape->AddArrow(ARROW_HOLLOW_CIRCLE, ARROW_POSITION_END,
		  DIAGRAM_HOLLOW_CIRCLE_SIZE, 0.0, DIAGRAM_HOLLOW_CIRCLE_TEXT);
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

}

/******************************************************************************/
/****************************** DrawSimConnections ****************************/
/******************************************************************************/

/*
 * This routine draws all standard connections by going through the data
 * output connection lists.
 */

void
SDIDiagram::DrawSimConnections(DatumPtr start)
{
	printf("SDIDiagram::DrawSimConnections: Entered\n");
	DatumPtr	pc, toPc;
	DynaBListPtr labelBList;

	if (start == NULL)
		return;
	SET_PARS_POINTER(GetPtr_AppInterface()->audModel);
	labelBList = GetLabelBList_Utility_SimScript();
	for (pc = start; pc != NULL; pc = pc->next) {
		if (pc->type == STOP)
			continue;
		wxShape *fromShape = (wxShape *) (GET_DATUM_CLIENT_DATA(pc));
		if ((pc->type == PROCESS) && pc->u.proc.outputList) {
			for (DynaListPtr p = pc->u.proc.outputList; p != NULL; p = p->
			  next) {
				toPc = (DatumPtr) FindElement_Utility_DynaBList(labelBList,
				  CmpProcessLabel_Utility_Datum, (char *) p->data)->data;
				AddLineShape(fromShape, (wxShape *) toPc->data->clientData, -1);
			}
		} else {
			for (toPc = pc->next; toPc && (toPc->type == STOP); toPc = toPc->
			  next)
				;
			if (toPc)
				AddLineShape(fromShape, (wxShape *) GET_DATUM_CLIENT_DATA(toPc),
				  -1);
		}
		if (pc->type == REPEAT) {
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
			AddLineShape(fromShape, (wxShape *) GET_DATUM_CLIENT_DATA(toPc),
			  REPEAT);
		}
	}

}

/******************************************************************************/
/****************************** DrawSimulation ********************************/
/******************************************************************************/

void
SDIDiagram::DrawSimulation(DatumPtr start)
{
	printf("SDIDiagram::DrawSimulation: Entered\n");
	DrawSimShapes(start);
	DrawSimConnections(start);

}

/******************************************************************************/
/****************************** CreateShape ***********************************/
/******************************************************************************/

wxShape *
SDIDiagram::CreateBasicShape(wxClassInfo *shapeInfo, int type, wxBrush *brush)
{
	wxShape *theShape = (wxShape *) shapeInfo->CreateObject();
	theShape->AssignNewIds();
	theShape->SetEventHandler(new SDIEvtHandler(theShape, theShape,
	  wxString(""), type));
	theShape->SetCentreResize(FALSE);
	theShape->SetPen(wxBLACK_PEN);
	theShape->SetBrush(brush);
	return(theShape);

}

/******************************************************************************/
/****************************** OnShapeSave ***********************************/
/******************************************************************************/

bool
SDIDiagram::OnShapeSave(wxExprDatabase& db, wxShape& shape, wxExpr& expr)
{
	wxDiagram::OnShapeSave(db, shape, expr);
	SDIEvtHandler *handler = (SDIEvtHandler *)shape.GetEventHandler();
	expr.AddAttributeValueString("label", handler->label);
	return TRUE;

}

/******************************************************************************/
/****************************** OnShapeLoad ***********************************/
/******************************************************************************/

bool
SDIDiagram::OnShapeLoad(wxExprDatabase& db, wxShape& shape, wxExpr& expr)
{
	printf("SDIDiagram::OnShapeLoad: Called.\n");
	wxDiagram::OnShapeLoad(db, shape, expr);
	char *label = NULL;
	expr.AssignAttributeValue("label", &label);
	SDIEvtHandler *handler = new SDIEvtHandler(&shape, &shape, wxString(label));
	shape.SetEventHandler(handler);

	if (label)
		delete[] label;
	return TRUE;

}

#endif /* HAVE_WX_OGL_OGL_H */
