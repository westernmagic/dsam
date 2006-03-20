/**********************
 *
 * File:		GrSDIBaseShapes.h
 * Purpose: 	Base Shape classes for Simulation Design Interface.
 * Comments:	Revised from Julian Smart's Ogledit/doc.h
 *				This code was moved from the GrSDIShapes code module.
 * Author:		L.P.O'Mard
 * Created:		10 Apr 2005
 * Updated:		
 * Copyright:	(c) 2005, CNBH, University of Essex
 *
 **********************/

#ifndef _GRSDIBASESHAPES_H
#define _GRSDIBASESHAPES_H 1

#ifdef __GNUG__
// #pragma interface
#endif

#include <wx/ogl/ogl.h>
#include <wx/ogl/drawn.h>

#include "tinyxml.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define SHAPE_DEFAULT_FONT_POINTSIZE	5
#define SDI_STANDARD_SHAPE_WIDTH		100

// Misc. elements.
#define SHAPE_XML_ARCS_ELEMENT				_T("arcs")
#define SHAPE_XML_ARROW_LIST_ELEMENT		_T("arrow_list")
#define SHAPE_XML_ARROW_ELEMENT				_T("arrow")
#define SHAPE_XML_ALIGNMENT_END_ELEMENT		_T("align_end")
#define SHAPE_XML_ALIGNMENT_START_ELEMENT	_T("align_start")
#define SHAPE_XML_ATTACHMENT_FROM_ELEMENT	_T("attachment_from")
#define SHAPE_XML_ATTACHMENT_TO_ELEMENT		_T("attachment_to")
#define SHAPE_XML_ATTACHMENTS_ELEMENT		_T("attachments")
#define SHAPE_XML_BRUSH_ELEMENT				_T("brush")
#define	SHAPE_XML_CONTROL_POINTS_ELEMENT	_T("control_points")
#define SHAPE_XML_FONT_ELEMENT				_T("font")
#define SHAPE_XML_INPUT_ELEMENT				_T("input")
#define SHAPE_XML_IS_SPLINE_ELEMENT			_T("is_spline")
#define SHAPE_XML_KEEP_LINES_STRAIGHT_ELEMENT	_T("is_spline")
#define SHAPE_XML_LINE_ELEMENT				_T("line")
#define SHAPE_XML_MISC_ELEMENT				_T("misc")
#define SHAPE_XML_OBJECT_ELEMENT			_T("object")
#define SHAPE_XML_OUTPUT_ELEMENT			_T("output")
#define SHAPE_XML_PEN_ELEMENT				_T("pen")
#define SHAPE_XML_REGION_ELEMENT			_T("region")
#define SHAPE_XML_REGIONS_ELEMENT			_T("regions")
#define SHAPE_XML_SHAPE_ELEMENT				_T("shape")
#define SHAPE_XML_TEXT_ELEMENT				_T("text")

// Shape-related attributes.
#define	SHAPE_XML_BRANCH_SPACING_ATTRIBUTE	_T("branch_spacing")
#define	SHAPE_XML_BRANCH_STYLE_ATTRIBUTE	_T("branch_style")
#define	SHAPE_XML_CENTRE_RESIZE_ATTRIBUTE	_T("centre_resize")
#define	SHAPE_XML_COLOUR_ATTRIBUTE			_T("colour")
#define SHAPE_XML_CORNER_ATTRIBUTE			_T("corner")
#define SHAPE_XML_COUNT_ATTRIBUTE			_T("count")
#define SHAPE_XML_END_ATTRIBUTE				_T("end")
#define SHAPE_XML_FAMILY_ATTRIBUTE			_T("family")
#define	SHAPE_XML_FIXED_HEIGHT_ATTRIBUTE	_T("fixed_height")
#define	SHAPE_XML_FIXED_WIDTH_ATTRIBUTE		_T("fixed_width")
#define SHAPE_XML_FORMAT_MODE_ATTRIBUTE		_T("format_mode")
#define SHAPE_XML_FROM_ATTRIBUTE			_T("from")
#define SHAPE_XML_HEIGHT_ATTRIBUTE			_T("height")
#define	SHAPE_XML_HILITE_ATTRIBUTE			_T("hilite")
#define	SHAPE_XML_ISSPLINE_ATTRIBUTE		_T("is_spline")
#define SHAPE_XML_OBJLABEL_ATTRIBUTE		_T("obj_label")
#define	SHAPE_XML_STYLE_ATTRIBUTE			_T("style")
#define	SHAPE_XML_MAINTAIN_ASPECT_RATIO_ATTRIBUTE	_T("maintain_aspect_ratio")
#define	SHAPE_XML_MIN_HEIGHT_ATTRIBUTE		_T("min_height")
#define	SHAPE_XML_MIN_WIDTH_ATTRIBUTE		_T("min_width")
#define	SHAPE_XML_MODE_ATTRIBUTE			_T("mode")
#define	SHAPE_XML_NAME_ATTRIBUTE			_T("name")
#define	SHAPE_XML_NECK_LENGTH_ATTRIBUTE		_T("neck_length")
#define	SHAPE_XML_PARENT_ATTRIBUTE			_T("parent")
#define SHAPE_XML_PEN_COLOUR_ATTRIBUTE		_T("pen_colour")
#define SHAPE_XML_PEN_STYLE_ATTRIBUTE		_T("pen_style")
#define SHAPE_XML_POINT_ELEMENT				_T("point")
#define SHAPE_XML_POINT_LIST_ELEMENT		_T("point_list")
#define SHAPE_XML_POINTSIZE_ATTRIBUTE		_T("point_size")
#define SHAPE_XML_PROPORTION_X_ATTRIBUTE	_T("proportion_x")
#define SHAPE_XML_PROPORTION_Y_ATTRIBUTE	_T("proportion_y")
#define	SHAPE_XML_ROTATION_ATTRIBUTE		_T("rotation")
#define	SHAPE_XML_SENSITIVITY_ATTRIBUTE		_T("sensitivity")
#define	SHAPE_XML_SHADOW_MODE_ATTRIBUTE		_T("shadow_mode")
#define	SHAPE_XML_SIZE_ATTRIBUTE			_T("size")
#define	SHAPE_XML_SPACE_ATTACHMENTS_ATTRIBUTE	_T("space_attachments")
#define	SHAPE_XML_SPACING_ATTRIBUTE			_T("spacing")
#define SHAPE_XML_STYLE_ATTRIBUTE			_T("style")
#define	SHAPE_XML_STEM_LENGTH_ATTRIBUTE		_T("stem_length")
#define	SHAPE_XML_TO_ATTRIBUTE				_T("to")
#define SHAPE_XML_TEXT_COLOUR_ATTRIBUTE		_T("text_colour")
#define SHAPE_XML_TYPE_ATTRIBUTE			_T("type")
#define	SHAPE_XML_USE_ATTACHMENTS_ATTRIBUTE	_T("use_attachments")
#define	SHAPE_XML_WEIGHT_ATTRIBUTE			_T("weight")
#define	SHAPE_XML_WIDTH_ATTRIBUTE			_T("width")
#define	SHAPE_XML_X_ATTRIBUTE				_T("x")
#define	SHAPE_XML_X_OFFSET_ATTRIBUTE		_T("x_offset")
#define	SHAPE_XML_Y_ATTRIBUTE				_T("y")
#define	SHAPE_XML_Y_OFFSET_ATTRIBUTE		_T("y_offset")


/******************************************************************************/
/*************************** Macro definitions ********************************/
/******************************************************************************/

#define ATTRIBUTE_VAL(ELMNT, ATTR, X, WARN) \
	{ \
		BackupVar	oldVar = BackupVar(X); \
		if ((ELMNT)->Attribute((ATTR), &(X)) == NULL) { \
			X = oldVar.Value(X); \
			if (WARN) { \
				NotifyWarning("%s: Could not get '%s' attribute value.", \
				  funcName, (ATTR)); \
				ok = false; \
			} \
		} \
	}

#define STR_ATTRIBUTE_VAL(ELMNT, ATTR, X, WARN) \
	(X) = (ELMNT)->Attribute(ATTR); \
	if ((X).IsEmpty() && (WARN)) { \
		NotifyWarning("%s: Could not get '%s' attribute value.", \
		  funcName, (ATTR)); \
		ok = false; \
	}

/******************************************************************************/
/*************************** Enum definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** BackupVar ****************************************/

class BackupVar
{
	int		i;
	long	l;
	double	r;
	wxString	s;

  public:
	BackupVar(int theI)		{ i = theI; }
	BackupVar(long theL)		{ l = theL; }
	BackupVar(double theR)	{ r = theR; }
	BackupVar(wxString &theS)	{ s = theS; }

	int		Value(int theI)				{ return (true)? i: theI; }
	long	Value(long theL)			{ return (true)? l: theL; }
	double	Value(double theR)			{ return (true)? r: theR; }
	wxString	Value(wxString& theS)	{ return (true)? s: theS; }

};

/*************************** SDIShape *****************************************/

class SDIShape: public wxShape
{
	DECLARE_ABSTRACT_CLASS(SDIShape)

  public:
	SDIShape(wxShapeCanvas *can = NULL);

	virtual	void	AddXMLInfo(TiXmlNode &node) { ; }
	virtual	void	MyAddLine(wxShape* from, wxShape *to) { ; };
	virtual	bool	GetXMLInfo(TiXmlNode *node) { return true; }

	void	AddAttachmentsInfo(TiXmlNode &parent);
	void	AddBrushInfo(TiXmlNode &parent);
	void	AddFontInfo(TiXmlNode &parent, wxFont *font);
	void	AddLineInfo(TiXmlNode &parent);
	void	AddPenInfo(TiXmlNode &parent);
	friend void	AddPointInfo(TiXmlNode &parent, wxRealPoint *point);
	void	AddRegions(TiXmlNode &parent);
	void	AddShapeInfo(TiXmlNode &node);
	bool	GetAttachmentsInfo(TiXmlNode *parent);
	bool	GetBrushInfo(TiXmlNode *parent);
	wxFont *	GetFontInfo(TiXmlNode *parent);
	bool	GetPenInfo(TiXmlNode *parent);
	friend	wxRealPoint *	GetPointInfo(TiXmlElement *myElement);
	bool	GetRegionsInfo(TiXmlNode *parent);
	bool	GetShapeInfo(TiXmlElement *myElement);

};

/*************************** SDIPolygonShape **********************************/

class SDIPolygonShape: public wxPolygonShape
{
	DECLARE_DYNAMIC_CLASS(SDIPolygonShape)

	double	w, h;

  public:
    SDIPolygonShape(double width = 0.0, double height = 0.0);

	virtual	void AddXMLInfo(TiXmlNode &node);
	virtual	bool GetXMLInfo(TiXmlNode *node);

	friend	wxRealPoint *	GetPointInfo(TiXmlElement *myElement);

};

/*************************** SDIEllipseShape **********************************/

/*
 * A few new shape classes so we have a 1:1 mapping
 * between palette symbol and unique class
 */
 
class SDIEllipseShape: public wxEllipseShape
{
	DECLARE_DYNAMIC_CLASS(SDIEllipseShape)

  private:
  public:
	SDIEllipseShape(double width = 0.0, double height = 0.0);

	virtual	void AddXMLInfo(TiXmlNode &node);
	virtual	bool GetXMLInfo(TiXmlNode *node);

};

/*************************** SDIRectangleShape ********************************/

class SDIRectangleShape: public wxRectangleShape
{
	DECLARE_DYNAMIC_CLASS(SDIRectangleShape)

  public:
    SDIRectangleShape(double width = 0.0, double height = 0.0);

	virtual	void AddXMLInfo(TiXmlNode &node);
	virtual	bool GetXMLInfo(TiXmlNode *node);

};

#endif
