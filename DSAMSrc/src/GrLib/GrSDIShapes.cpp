/**********************
 *
 * File:		GrSDIShapes.cpp
 * Purpose: 	Shape classes for Simulation Design Interface.
 * Comments:	Revised from Julian Smart's Ogledit/doc.h
 * Author:		L.P.O'Mard
 * Created:		14 Nov 2002
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
#include "GrSDIShapes.h"

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
/****************************** wxRoundedRectangleShape Methods ***************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(wxRoundedRectangleShape, wxRectangleShape)

/******************************************************************************/
/****************************** Constructor ***********************************/
/******************************************************************************/

wxRoundedRectangleShape::wxRoundedRectangleShape(double w, double h):
 wxRectangleShape(w, h)
{
	// 0.3 of the smaller rectangle dimension
	SetCornerRadius((double) -0.3);
}

/******************************************************************************/
/****************************** wxDiamondShape Methods ************************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(wxDiamondShape, wxPolygonShape)

/******************************************************************************/
/****************************** Constructor ***********************************/
/******************************************************************************/

wxDiamondShape::wxDiamondShape(double w, double h):
  wxPolygonShape()
{
	// wxPolygonShape::SetSize relies on the shape having non-zero
	// size initially.
	if (w == 0.0)
		w = 60.0;
	if (h == 0.0)
		h = 60.0;

	wxList *thePoints = new wxList;
	wxRealPoint *point = new wxRealPoint(0.0, (-h/2.0));
	thePoints->Append((wxObject*) point);

	point = new wxRealPoint((w/2.0), 0.0);
	thePoints->Append((wxObject*) point);

	point = new wxRealPoint(0.0, (h/2.0));
	thePoints->Append((wxObject*) point);

	point = new wxRealPoint((-w/2.0), 0.0);
	thePoints->Append((wxObject*) point);

	Create(thePoints);

}

#endif /* HAVE_WX_OGL_OGL_H */
