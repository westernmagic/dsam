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
#include "GrSDIEvtHandler.h"
#include "GrSDIDiagram.h"

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
