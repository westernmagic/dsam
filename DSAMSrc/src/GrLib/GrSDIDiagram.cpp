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
	x = DIAGRAM_DEFAULT_INITIAL_X;
	y = DIAGRAM_DEFAULT_INITIAL_Y;
	simulation = NULL;

}

/******************************************************************************/
/****************************** AdjustShapeToLabel ****************************/
/******************************************************************************/

void
SDIDiagram::AdjustShapeToLabel(wxClientDC& dc, wxShape *shape, wxString& label)
{
	bool	sizeChanged = FALSE;
	double	boxWidth, boxHeight;
	wxCoord	labelWidth, labelHeight;

	label.MakeLower();
	dc.GetTextExtent(label, &labelWidth, &labelHeight);
	shape->GetBoundingBoxMin(&boxWidth, &boxHeight);
	if ((labelWidth + DIAGRAM_LABEL_WIDTH_MARGIN) > boxWidth) {
		boxWidth = labelWidth + DIAGRAM_LABEL_WIDTH_MARGIN;
		sizeChanged = TRUE;
	}
	if ((labelHeight + DIAGRAM_LABEL_HEIGHT_MARGIN) > boxHeight) {
		boxHeight = labelHeight + DIAGRAM_LABEL_HEIGHT_MARGIN;
		sizeChanged = TRUE;
	}
	if (sizeChanged)
		shape->SetSize(boxWidth, boxHeight);

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
		AdjustShapeToLabel(dc, shape, myHandler->label);
	}

}

/******************************************************************************/
/****************************** SetShape **************************************/
/******************************************************************************/

wxShape *
SDIDiagram::CreateLoadShape(DatumPtr pc, wxClassInfo *shapeInfo, int type,
  wxBrush *brush)
{
	double	boxWidth, boxHeight;
	wxShape *shape = CreateBasicShape(shapeInfo, type, brush);

	SDIEvtHandler *myHandler = (SDIEvtHandler *) shape->GetEventHandler();
	myHandler->pc = pc;

	shape->SetSize(DIAGRAM_DEFAULT_SHAPE_WIDTH, DIAGRAM_DEFAULT_SHAPE_HEIGHT);
	shape->SetId(pc->stepNumber);
	AddShape(shape);
	wxClientDC dc(shape->GetCanvas());
	shape->GetCanvas()->PrepareDC(dc);
	shape->Move(dc, x, y);
	shape->Show(TRUE);
	shape->GetBoundingBoxMax(&boxWidth, &boxHeight);
	x += boxWidth + DIAGRAM_DEFAULT_X_SEPARATION;
	if ((x + boxWidth) > shape->GetCanvas()->GetClientSize().GetWidth()) {
		x = DIAGRAM_DEFAULT_INITIAL_X;
		y += DIAGRAM_DEFAULT_SHAPE_HEIGHT + DIAGRAM_DEFAULT_Y_SEPARATION;
	}
	return(shape);

}

/******************************************************************************/
/****************************** SetProcessClientData **************************/
/******************************************************************************/

/*
 * It assumes that the process, pc, is not NULL.
 */

void
SDIDiagram::SetProcessClientData(DatumPtr pc, wxShape *shape)
{
	if (pc->data)
		pc->data->clientData = shape;
	else
		pc->clientData = shape;

}

/******************************************************************************/
/****************************** DrawSimShapes *********************************/
/******************************************************************************/

void
SDIDiagram::DrawSimShapes()
{
	DatumPtr	pc;
	ModulePtr	module;
	wxShape		*shape;
	SDICanvas	*canvas = (SDICanvas *) GetCanvas();

	if (simulation == NULL)
		return;
	for (pc = simulation; pc != NULL; pc = pc->next) {
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
			wxLogError("SDIDiagram::DrawSimShapes: datum type %d not "
			  "implemented.\n", pc->type);
		} /* switch */
		if (shape)
			SetProcessClientData(pc, shape);
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

}

/******************************************************************************/
/****************************** DrawSimConnections ****************************/
/******************************************************************************/

/*
 * This routine draws all standard connections by going through the data
 * output connection lists.
 */

void
SDIDiagram::DrawSimConnections(void)
{
	DatumPtr	pc, toPc;
	DynaBListPtr labelBList;

	if (simulation == NULL)
		return;
	SET_PARS_POINTER(GetPtr_AppInterface()->audModel);
	labelBList = GetLabelBList_Utility_SimScript();
	for (pc = simulation; pc != NULL; pc = pc->next) {
		if (pc->type == STOP)
			continue;
		wxShape *fromShape = (wxShape *) (GET_DATUM_CLIENT_DATA(pc));
		switch (pc->type) {
		case PROCESS: 
			if (pc->u.proc.outputList) {
				for (DynaListPtr p = pc->u.proc.outputList; p != NULL; p = p->
				  next) {
					toPc = (DatumPtr) FindElement_Utility_DynaBList(labelBList,
					  CmpProcessLabel_Utility_Datum, (char *) p->data)->data;
					AddLineShape(fromShape, (wxShape *) GET_DATUM_CLIENT_DATA(
					  toPc), -1);
				}
			} else {
				EarObjRefPtr	p;
				for (p = pc->data->customerList; p != NULL; p = p->next)
					AddLineShape(fromShape, (wxShape *) p->earObject->
					  clientData, -1);
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
			AddLineShape(fromShape, (wxShape *) GET_DATUM_CLIENT_DATA(toPc),
			  REPEAT);
			break; }
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
	DrawSimShapes();
	DrawSimConnections();

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
/****************************** FindShapeProcess ******************************/
/******************************************************************************/

DatumPtr
SDIDiagram::FindShapeProcess(uInt id)
{
	DatumPtr	pc;

	if (simulation == NULL)
		return(NULL);
	for (pc = simulation; pc != NULL; pc = pc->next)
		if ((pc->type == PROCESS) && (pc->stepNumber == id))
			return (pc);
	return(NULL);
	
}

/******************************************************************************/
/****************************** VerifyDiagram *********************************/
/******************************************************************************/

/*
 * This function checks that the loaded diagram corresponds with the simulation.
 * It returns 'FALSE' if it finds any discrepancies.
 */

bool
SDIDiagram::VerifyDiagram(void)
{
	static const char *funcName = "SDIDiagram::VerifyDiagram";
	int		numDiagConnections = 0, numSimConnections = 0;
	DatumPtr	pc;
	EarObjectPtr	fromProcess, toProcess;
	EarObjRefPtr p;
	wxNode *node = m_shapeList->First();

	// Check processes exist for each shape.
	while (node) {
		wxShape *shape = (wxShape *) node->Data();
		if (!shape->IsKindOf(CLASSINFO(wxLineShape))) {
			if ((pc = FindShapeProcess((uInt) shape->GetId())) == NULL)
				return(FALSE);
			SetProcessClientData(pc, shape);
			SHAPE_PC(shape) = pc;
		}
		node = node->Next();
	}
	// Check processes exist for each shape line, and that the connection
	// exists in the simulation
	node = m_shapeList->First();
	while (node) {
		wxShape *shape = (wxShape *) node->Data();
		if (shape->IsKindOf(CLASSINFO(wxLineShape))) {
			wxLineShape *lineShape = (wxLineShape *) shape;
			wxShape *fromShape = lineShape->GetFrom();
			wxShape *toShape = lineShape->GetTo();
			if (!fromShape || !toShape) {
				wxLogWarning("%s: Diagram line is not connected to a valid "
				  "process.", funcName);
				return (FALSE);
			}
			fromProcess = SHAPE_PC(fromShape)->data;
			toProcess = SHAPE_PC(toShape)->data;
			for (p = fromProcess->customerList; (p != NULL) && (p->earObject->
			  handle != toProcess->handle); p = p->next)
				;
			if (!p) {
				wxLogWarning("%s: Diagram line does not correspond to a "
				  "simulation connection.", funcName);
				return(FALSE);
			}
			numDiagConnections++;
		}
		node = node->Next();
	}
	// Check if any undrawn connections or shapes
	for (pc = simulation; pc != NULL; pc = pc->next) {
		if (pc->type == PROCESS) {
			if (!pc->clientData && (pc->data && !pc->data->clientData)) {
				wxLogWarning("%s: Process has no description (step %d, label "
				  "%s'.", funcName, pc->stepNumber, pc->label);
				return (FALSE);
			}
			for (p = pc->data->customerList; (p != NULL); p = p->next)
				numSimConnections++;
		}
	}
	if (numDiagConnections != numSimConnections) {
		wxLogWarning("%s: The number of diagram lines (%d) does not "
		  "correspond\nto the number of simulation connections (%d).", funcName,
		  numDiagConnections, numSimConnections);
		return(FALSE);
	}
	return(TRUE);

}


/******************************************************************************/
/****************************** OnShapeLoad ***********************************/
/******************************************************************************/

bool
SDIDiagram::OnShapeLoad(wxExprDatabase& db, wxShape& shape, wxExpr& expr)
{
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
