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
/****************************** DrawSimulation ********************************/
/******************************************************************************/

void
SDIDiagram::DrawSimulation(DatumPtr start)
{
	printf("SDIDiagram::DrawSimulation: Entered\n");
	DatumPtr	pc;
	SDICanvas	*canvas = (SDICanvas *) GetCanvas();

	if (start == NULL)
		return;
	for (pc = start; pc != NULL; pc = pc->next) {
		wxShape *theShape = CreateShape(canvas->GetClassInfo(pc->data->
		  module->classSpecifier), pc->type, wxCYAN_BRUSH);
		theShape->SetSize(60, 60);
		AddShape(theShape);
		theShape->Show(TRUE);
		wxClientDC dc(theShape->GetCanvas());
		theShape->GetCanvas()->PrepareDC(dc);
		theShape->Move(dc, 0, 0);
	}

}

/******************************************************************************/
/****************************** CreateShape ***********************************/
/******************************************************************************/

wxShape *
SDIDiagram::CreateShape(wxClassInfo *shapeInfo, int type, wxBrush *brush)
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
