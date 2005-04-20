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

#include "tinyxml.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define SDI_STANDARD_SHAPE_WIDTH      100

// Shape elements.
#define SHAPE_XML_ANALYSIS_SHAPE_ELEMENT	_T("analysis_shape")
#define SHAPE_XML_CONTROL_SHAPE_ELEMENT		_T("control_shape")
#define SHAPE_XML_DISPLAY_SHAPE_ELEMENT		_T("display_shape")
#define SHAPE_XML_FILTER_SHAPE_ELEMENT		_T("filter_shape")
#define SHAPE_XML_IO_SHAPE_ELEMENT			_T("io_shape")
#define SHAPE_XML_LINE_SHAPE_ELEMENT		_T("line_shape")
#define SHAPE_XML_MODEL_SHAPE_ELEMENT		_T("model_shape")
#define SHAPE_XML_USER_SHAPE_ELEMENT		_T("user_shape")
#define SHAPE_XML_TRANSFORM_SHAPE_ELEMENT	_T("transform_shape")
#define SHAPE_XML_UTILITY_SHAPE_ELEMENT		_T("utility_shape")

/******************************************************************************/
/*************************** Enum definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class pre-definitions ****************************/
/******************************************************************************/

class SDIShape;
class SDIEllipseShape;
class SDIPolygonShape;
class SDIRectangleShape;

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** SDIAnalysisShape *********************************/

/*
 * A few new shape classes so we have a 1:1 mapping
 * between palette symbol and unique class
 */
 
class SDIAnalysisShape: public SDIEllipseShape
{
	DECLARE_DYNAMIC_CLASS(SDIAnalysisShape)

  private:
  public:
	SDIAnalysisShape(double width = 0.0, double height = 0.0);

	void	AddXMLInfo(TiXmlNode &node);
	bool	GetXMLInfo(TiXmlNode *node);

};

/*************************** SDIControlShape **********************************/

class SDIControlShape: public SDIPolygonShape
{
	DECLARE_DYNAMIC_CLASS(SDIControlShape)

  private:
  public:
	SDIControlShape(double width = 0.0, double height = 0.0);

	void	AddXMLInfo(TiXmlNode &node);
	bool	GetXMLInfo(TiXmlNode *node);

};

/*************************** SDIDisplayShape **********************************/

/*
 * A few new shape classes so we have a 1:1 mapping
 * between palette symbol and unique class
 */
 
class SDIDisplayShape: public SDIRectangleShape
{
	DECLARE_DYNAMIC_CLASS(SDIDisplayShape)

  private:
  public:
	SDIDisplayShape(double width = 0.0, double height = 0.0);

	void	AddXMLInfo(TiXmlNode &node);
	bool	GetXMLInfo(TiXmlNode *node);

};

/*************************** SDIFilterShape ***********************************/

class SDIFilterShape: public SDIPolygonShape
{
	DECLARE_DYNAMIC_CLASS(SDIFilterShape)

  public:
    SDIFilterShape(double width = 0.0, double height = 0.0);

	void	AddXMLInfo(TiXmlNode &node);

};

/*************************** SDIIOShape ***************************************/

class SDIIOShape: public SDIPolygonShape
{
	DECLARE_DYNAMIC_CLASS(SDIIOShape)

  public:
    SDIIOShape(double width = 0.0, double height = 0.0);

	void	AddXMLInfo(TiXmlNode &node);
	bool	GetXMLInfo(TiXmlNode *node);

};

/*************************** SDILineShape *************************************/

class SDILineShape: public wxLineShape
{
	DECLARE_DYNAMIC_CLASS(SDILineShape)

  public:
 
	void	AddXMLArrowListInfo(TiXmlNode &node, wxNode *aNode);
	void	AddXMLControlPointsInfo(TiXmlNode &node, wxNode *cPNode);
	void	AddXMLInfo(TiXmlNode &node);
	bool	GetXMLArrowListInfo(TiXmlNode *parent);
	bool	GetXMLControlPointsInfo(TiXmlNode *node);
	bool	GetXMLInfo(TiXmlNode *node);

};

/*************************** SDIModelShape ************************************/

class SDIModelShape: public SDIPolygonShape
{
	DECLARE_DYNAMIC_CLASS(SDIModelShape)

  public:
    SDIModelShape(double width = 0.0, double height = 0.0);

	void	AddXMLInfo(TiXmlNode &node);

};

/*************************** SDITransformShape ********************************/

class SDITransformShape: public SDIPolygonShape
{
	DECLARE_DYNAMIC_CLASS(SDITransformShape)

  public:
    SDITransformShape(double width = 0.0, double height = 0.0);

	void	AddXMLInfo(TiXmlNode &node);
	bool	GetXMLInfo(TiXmlNode *node);

};

/*************************** SDIUserShape *************************************/

class SDIUserShape: public SDIPolygonShape
{
	DECLARE_DYNAMIC_CLASS(SDIUserShape)

  public:
    SDIUserShape(double width = 0.0, double height = 0.0);

	void	AddXMLInfo(TiXmlNode &node);

};

/*************************** SDIUtilityShape **********************************/

class SDIUtilityShape: public SDIRectangleShape
{
	DECLARE_DYNAMIC_CLASS(SDIUtilityShape)

  public:
    SDIUtilityShape(double width = 0.0, double height = 0.0);

	void	AddXMLInfo(TiXmlNode &node);
	bool	GetXMLInfo(TiXmlNode *node);

};

#endif
