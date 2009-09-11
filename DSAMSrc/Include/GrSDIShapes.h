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

#define SDI_STANDARD_SHAPE_WIDTH	100
#define SHAPE_DEBUG					1

// Shape elements.
#define SHAPE_XML_ANALYSIS_SHAPE_ELEMENT	wxT("analysis_shape")
#define SHAPE_XML_CONTROL_SHAPE_ELEMENT		wxT("control_shape")
#define SHAPE_XML_DISPLAY_SHAPE_ELEMENT		wxT("display_shape")
#define SHAPE_XML_FILTER_SHAPE_ELEMENT		wxT("filter_shape")
#define SHAPE_XML_IO_SHAPE_ELEMENT			wxT("io_shape")
#define SHAPE_XML_LINE_SHAPE_ELEMENT		wxT("line_shape")
#define SHAPE_XML_MODEL_SHAPE_ELEMENT		wxT("model_shape")
#define SHAPE_XML_USER_SHAPE_ELEMENT		wxT("user_shape")
#define SHAPE_XML_TRANSFORM_SHAPE_ELEMENT	wxT("transform_shape")
#define SHAPE_XML_UTILITY_SHAPE_ELEMENT		wxT("utility_shape")

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
class SDICompositeShape;
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

	void	AddXMLInfo(DSAMXMLNode *node);

};

/*************************** SDIControlParentShape **********************************/

class SDIControlParentShape: public SDIPolygonShape
{
	DECLARE_DYNAMIC_CLASS(SDIControlParentShape)

  private:
  public:
	SDIControlParentShape(double width = 0.0, double height = 0.0);

	void	AddXMLInfo(DSAMXMLNode *node);
	bool	GetXMLInfo(wxXmlNode *myElement);

};

/*************************** SDIControlShape **********************************/

class SDIControlShape: public SDICompositeShape
{
	DECLARE_DYNAMIC_CLASS(SDICompositeShape)

  private:
  public:
	SDIControlShape(double width = 0.0, double height = 0.0);

	void	AddXMLInfo(DSAMXMLNode *node);
	bool	GetXMLInfo(wxXmlNode *myElement);

	SDIShape *	GetParentShape(void)	{ return (SDIShape *)
				  m_children.GetFirst()->GetData(); }
#	if SHAPE_DEBUG
	void	SetSize(double w, double h, bool recursive);
#	endif // DEBUG

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

	void	AddXMLInfo(DSAMXMLNode *node);
	bool	GetXMLInfo(wxXmlNode *myElement);

};

/*************************** SDIFilterShape ***********************************/

class SDIFilterShape: public SDIPolygonShape
{
	DECLARE_DYNAMIC_CLASS(SDIFilterShape)

  public:
    SDIFilterShape(double width = 0.0, double height = 0.0);

	void	AddXMLInfo(DSAMXMLNode *node);
	bool	GetXMLInfo(wxXmlNode *myElement);

};

/*************************** SDIIOShape ***************************************/

class SDIIOShape: public SDIPolygonShape
{
	DECLARE_DYNAMIC_CLASS(SDIIOShape)

  public:
    SDIIOShape(double width = 0.0, double height = 0.0);

	void	AddXMLInfo(DSAMXMLNode *node);
	bool	GetXMLInfo(wxXmlNode *myElement);

};

/*************************** SDILineShape *************************************/

class SDILineShape: public wxLineShape
{
	DECLARE_DYNAMIC_CLASS(SDILineShape)

  public:

	void	AddXMLArrowListInfo(DSAMXMLNode *node, wxNode *aNode);
	void	AddXMLControlPointsInfo(DSAMXMLNode *node, wxNode *cPNode);
	void	AddXMLInfo(DSAMXMLNode *node);
	friend	wxRealPoint *	GetPointInfo(wxXmlNode *myElement);
	bool	GetXMLArrowListInfo(wxXmlNode *myElement);
	bool	GetXMLControlPointsInfo(wxXmlNode *myElement);
	bool	GetXMLInfo(wxXmlNode *myElement);

};

/*************************** SDIModelShape ************************************/

class SDIModelShape: public SDIPolygonShape
{
	DECLARE_DYNAMIC_CLASS(SDIModelShape)

  public:
    SDIModelShape(double width = 0.0, double height = 0.0);

	void	AddXMLInfo(DSAMXMLNode *node);
	bool	GetXMLInfo(wxXmlNode *myElement);

};

/*************************** SDITransformShape ********************************/

class SDITransformShape: public SDIPolygonShape
{
	DECLARE_DYNAMIC_CLASS(SDITransformShape)

  public:
    SDITransformShape(double width = 0.0, double height = 0.0);

	void	AddXMLInfo(DSAMXMLNode *node);
	bool	GetXMLInfo(wxXmlNode *myElement);

};

/*************************** SDIUserShape *************************************/

class SDIUserShape: public SDIPolygonShape
{
	DECLARE_DYNAMIC_CLASS(SDIUserShape)

  public:
    SDIUserShape(double width = 0.0, double height = 0.0);

	void	AddXMLInfo(DSAMXMLNode *node);
	bool	GetXMLInfo(wxXmlNode *myElement);

};

/*************************** SDIUtilityShape **********************************/

class SDIUtilityShape: public SDIRectangleShape
{
	DECLARE_DYNAMIC_CLASS(SDIUtilityShape)

  public:
    SDIUtilityShape(double width = 0.0, double height = 0.0);

	void	AddXMLInfo(DSAMXMLNode *node);
	bool	GetXMLInfo(wxXmlNode *myElement);

};

#endif
