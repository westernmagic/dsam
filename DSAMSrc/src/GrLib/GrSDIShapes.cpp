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
/****************************** SDIAnalysisShape Methods **********************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDIAnalysisShape, wxEllipseShape)

SDIAnalysisShape::SDIAnalysisShape(double w, double h): wxEllipseShape(w, h)
{
}

/******************************************************************************/
/****************************** SDIControlShape Methods ***********************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDIControlShape, wxRectangleShape)

/******************************************************************************/
/****************************** Constructor ***********************************/
/******************************************************************************/

SDIControlShape::SDIControlShape(double w, double h): wxRectangleShape(w, h)
{
	// 0.3 of the smaller rectangle dimension
	SetCornerRadius((double) -0.3);
}

/******************************************************************************/
/****************************** SDIUtilityShape Methods ***********************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDIUtilityShape, wxPolygonShape)

SDIUtilityShape::SDIUtilityShape(double w, double h): wxPolygonShape()
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

/******************************************************************************/
/****************************** SDIFilterShape Methods ***********************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDIFilterShape, wxPolygonShape)

SDIFilterShape::SDIFilterShape(double w, double h): wxPolygonShape()
{
	// wxPolygonShape::SetSize relies on the shape having non-zero
	// size initially.
	if (w == 0.0)
		w = 60.0;
	if (h == 0.0)
		h = 60.0;

	double	pipe = w / 6.0;
	double	neck = h / 8.0;
	double	hW = w / 10.0;
	double	hH = h / 4.0;

	wxList *thePoints = new wxList;
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0), (h / 2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0 + pipe), (h /
	  2.0)));
	thePoints->Append((wxObject*) new wxRealPoint(0.0, (h / 2.0 - neck)));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0 - pipe), (h / 2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0), (h / 2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0), hH));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0 - hW), hH));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0 - hW), -hH));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0), -hH));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0), (-h / 2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0 - pipe), (-h /
	  2.0)));
	thePoints->Append((wxObject*) new wxRealPoint(0.0, (-h / 2.0 + neck)));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0 + pipe), (-h /
	  2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0), (-h / 2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0), -hH));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0 + hW), -hH));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0 + hW), +hH));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0), +hH));
	Create(thePoints);

 }

/******************************************************************************/
/****************************** SDIIOShape Methods ****************************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDIIOShape, wxPolygonShape)

SDIIOShape::SDIIOShape(double w, double h): wxPolygonShape()
{
	// wxPolygonShape::SetSize relies on the shape having non-zero
	// size initially.
	if (w == 0.0)
		w = 60.0;
	if (h == 0.0)
		h = 60.0;

	double	flange = h / 8.0;

	wxList *thePoints = new wxList;
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0), (h / 2.0 -
	  flange)));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0), (h / 2.0 -
	  flange)));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0), (h / 2.0)));
	thePoints->Append((wxObject*) new wxRealPoint(w, 0.0));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0), (-h / 2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0), (-h / 2.0 +
	  flange)));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0), (-h / 2.0 +
	  flange)));
	Create(thePoints);

 }

/******************************************************************************/
/****************************** SDIModelShape Methods *************************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDIModelShape, wxPolygonShape)

SDIModelShape::SDIModelShape(double w, double h): wxPolygonShape()
{
	// wxPolygonShape::SetSize relies on the shape having non-zero
	// size initially.
	if (w == 0.0)
		w = 60.0;
	if (h == 0.0)
		h = 60.0;

	double	edge = h / 4.0;
	double	hW = w / 10.0;
	double	hH = h / 4.0;

	wxList *thePoints = new wxList;
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0), (h / 2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0 - edge), (h /
	  2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0), (h / 2.0 -
	  edge)));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0), (-h / 2.0 +
	  edge)));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0 - edge), (-h /
	  2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0), (-h / 2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0), -hH));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0 + hW), -hH));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0 + hW), hH));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0), hH));
	Create(thePoints);

 }

/******************************************************************************/
/****************************** SDIStimulusShape Methods **********************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDIStimulusShape, wxPolygonShape)

SDIStimulusShape::SDIStimulusShape(double w, double h): wxPolygonShape()
{
	// wxPolygonShape::SetSize relies on the shape having non-zero
	// size initially.
	if (w == 0.0)
		w = 60.0;
	if (h == 0.0)
		h = 60.0;

	double	flange = h / 8.0;
	double	base = w / 6.0;
	double	slope = w / 7.0;

	wxList *thePoints = new wxList;
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0 + base + slope),
	  (h / 2.0 - flange)));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0), (h / 2.0 -
	  flange)));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0), (h / 2.0)));
	thePoints->Append((wxObject*) new wxRealPoint(w, 0.0));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0), (-h / 2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0), (-h / 2.0 +
	  flange)));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0 + base + slope),
	  (-h / 2.0 + flange)));
	// Base 
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0 + base), (-h /
	  2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0), (-h / 2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0), (h / 2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0 + base), (h /
	  2.0)));
	Create(thePoints);

 }

/******************************************************************************/
/****************************** SDIStimulusShape Methods **********************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDITransformShape, wxPolygonShape)

SDITransformShape::SDITransformShape(double w, double h): wxPolygonShape()
{
	// wxPolygonShape::SetSize relies on the shape having non-zero
	// size initially.
	if (w == 0.0)
		w = 60.0;
	if (h == 0.0)
		h = 60.0;

	double	offset = h / 8.0;
	wxList *thePoints = new wxList;
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0 - offset), (h /
	  2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0 - offset), (h /
	  2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0 + offset), (-h /
	  2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0 + offset), (-h /
	  2.0)));
	Create(thePoints);

 }

/******************************************************************************/
/****************************** SDIUserShape Methods **************************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDIUserShape, wxRectangleShape)

SDIUserShape::SDIUserShape(double w, double h): wxRectangleShape(w, h)
{
}

#endif /* HAVE_WX_OGL_OGL_H */
