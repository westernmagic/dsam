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
#include <wx/ogl/drawn.h>

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define SDI_STANDARD_SHAPE_WIDTH      100

/******************************************************************************/
/*************************** Enum definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** SDIAnalysisShape *********************************/

/*
 * A few new shape classes so we have a 1:1 mapping
 * between palette symbol and unique class
 */
 
class SDIAnalysisShape: public wxEllipseShape
{
	DECLARE_DYNAMIC_CLASS(SDIAnalysisShape)

  private:
  public:
	SDIAnalysisShape(double w = 0.0, double h = 0.0);

};

/*************************** SDIControlShape **********************************/

/*
 * A few new shape classes so we have a 1:1 mapping
 * between palette symbol and unique class
 */
 
class SDIControlShape: public wxRectangleShape
{
	DECLARE_DYNAMIC_CLASS(SDIControlShape)

  private:
  public:
	SDIControlShape(double w = 0.0, double h = 0.0);

};

/*************************** SDIUtilityShape **********************************/

class SDIUtilityShape: public wxPolygonShape
{
	DECLARE_DYNAMIC_CLASS(SDIUtilityShape)

  private:
  public:
	SDIUtilityShape(double w = 0.0, double h = 0.0);

};

/*************************** SDIFilterShape ***********************************/

class SDIFilterShape: public wxPolygonShape
{
	DECLARE_DYNAMIC_CLASS(SDIFilterShape)

  public:
    SDIFilterShape(double w = 0.0, double h = 0.0);

};

/*************************** SDIIOShape ***************************************/

class SDIIOShape: public wxPolygonShape
{
	DECLARE_DYNAMIC_CLASS(SDIIOShape)

  public:
    SDIIOShape(double w = 0.0, double h = 0.0);

};

/*************************** SDIModelShape ************************************/

class SDIModelShape: public wxPolygonShape
{
	DECLARE_DYNAMIC_CLASS(SDIModelShape)

  public:
    SDIModelShape(double w = 0.0, double h = 0.0);

};

/*************************** SDIStimulusShape *********************************/

class SDIStimulusShape: public wxPolygonShape
{
	DECLARE_DYNAMIC_CLASS(SDIStimulusShape)

  public:
    SDIStimulusShape(double w = 0.0, double h = 0.0);

};

/*************************** SDITransformShape ********************************/

class SDITransformShape: public wxPolygonShape
{
	DECLARE_DYNAMIC_CLASS(SDITransformShape)

  public:
    SDITransformShape(double w = 0.0, double h = 0.0);

};

/*************************** SDIUserShape *************************************/

class SDIUserShape: public wxRectangleShape
{
	DECLARE_DYNAMIC_CLASS(SDIUserShape)

  public:
    SDIUserShape(double w = 0.0, double h = 0.0);

};

#endif
