/**********************
 *
 * File:		GrSDIShapes.h
 * Purpose: 	Shape classes for Simulation Design Interface.
 * Comments:	Revised from Julian Smart's Ogledit/doc.h
 * Author:		L.P.O'Mard
 * Created:		14 Nov 2002
 * Updated:		
 * Copyright:	(c) 2002, CNBH, University of Essex
 *
 **********************/

#ifndef _GRSDISHAPES_H
#define _GRSDISHAPES_H 1

#ifdef __GNUG__
// #pragma interface
#endif

#include <wx/ogl/ogl.h>

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Enum definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** wxRoundedRectangleShape **************************/

/*
 * A few new shape classes so we have a 1:1 mapping
 * between palette symbol and unique class
 */
 
class wxRoundedRectangleShape: public wxRectangleShape
{
	DECLARE_DYNAMIC_CLASS(wxRoundedRectangleShape)

  private:
  public:
	wxRoundedRectangleShape(double w = 0.0, double h = 0.0);

};

/*************************** wxDiamondShape ***********************************/

class wxDiamondShape: public wxPolygonShape
{
	DECLARE_DYNAMIC_CLASS(wxDiamondShape)

  private:
  public:
	wxDiamondShape(double w = 0.0, double h = 0.0);

};

#endif
