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
#define SHAPE_XML_ARCS_ELEMENT				"arcs"
#define SHAPE_XML_ARROW_LIST_ELEMENT		"arrow_list"
#define SHAPE_XML_ARROW_ELEMENT				"arrow"
#define SHAPE_XML_ALIGNMENT_END_ELEMENT		"align_end"
#define SHAPE_XML_ALIGNMENT_START_ELEMENT	"align_start"
#define SHAPE_XML_ATTACHMENT_FROM_ELEMENT	"attachment_from"
#define SHAPE_XML_ATTACHMENT_TO_ELEMENT	"attachment_To"
#define SHAPE_XML_ATTACHMENTS_ELEMENT		"attachments"
#define SHAPE_XML_BRUSH_ELEMENT				"brush"
#define	SHAPE_XML_CONTROL_POINTS_ELEMENT	"control_points"
#define SHAPE_XML_FONT_ELEMENT				"font"
#define SHAPE_XML_INPUT_ELEMENT				"input"
#define SHAPE_XML_IS_SPLINE_ELEMENT			"is_spline"
#define SHAPE_XML_KEEP_LINES_STRAIGHT_ELEMENT	"is_spline"
#define SHAPE_XML_LINE_ELEMENT				"line"
#define SHAPE_XML_MISC_ELEMENT				"misc"
#define SHAPE_XML_OBJECT_ELEMENT			"object"
#define SHAPE_XML_OUTPUT_ELEMENT			"output"
#define SHAPE_XML_PEN_ELEMENT				"pen"
#define SHAPE_XML_REGION_ELEMENT			"region"
#define SHAPE_XML_REGIONS_ELEMENT			"regions"
#define SHAPE_XML_SHAPE_ELEMENT				"shape"
#define SHAPE_XML_TEXT_ELEMENT				"text"

// Shape-related attributes.
#define	SHAPE_XML_BRANCH_SPACING_ATTRIBUTE	"branch_spacing"
#define	SHAPE_XML_BRANCH_STYLE_ATTRIBUTE	"branch_style"
#define	SHAPE_XML_CENTRE_RESIZE_ATTRIBUTE	"centre_resize"
#define	SHAPE_XML_COLOUR_ATTRIBUTE			"colour"
#define SHAPE_XML_CORNER_ATTRIBUTE			"corner"
#define SHAPE_XML_COUNT_ATTRIBUTE			"count"
#define SHAPE_XML_END_ATTRIBUTE				"end"
#define SHAPE_XML_FAMILY_ATTRIBUTE			"family"
#define	SHAPE_XML_FIXED_HEIGHT_ATTRIBUTE	"fixed_height"
#define	SHAPE_XML_FIXED_WIDTH_ATTRIBUTE		"fixed_width"
#define SHAPE_XML_FORMAT_MODE_ATTRIBUTE		"format_mode"
#define SHAPE_XML_FROM_ATTRIBUTE			"from"
#define SHAPE_XML_HEIGHT_ATTRIBUTE			"height"
#define	SHAPE_XML_HILITE_ATTRIBUTE			"hilite"
#define	SHAPE_XML_ISSPLINE_ATTRIBUTE		"is_spline"
#define SHAPE_XML_OBJLABEL_ATTRIBUTE		"obj_label"
#define	SHAPE_XML_STYLE_ATTRIBUTE			"style"
#define	SHAPE_XML_MAINTAIN_ASPECT_RATIO_ATTRIBUTE	"maintain_aspect_ratio"
#define	SHAPE_XML_MIN_HEIGHT_ATTRIBUTE		"min_height"
#define	SHAPE_XML_MIN_WIDTH_ATTRIBUTE		"min_width"
#define	SHAPE_XML_MODE_ATTRIBUTE			"mode"
#define	SHAPE_XML_NAME_ATTRIBUTE			"name"
#define	SHAPE_XML_NECK_LENGTH_ATTRIBUTE		"neck_length"
#define	SHAPE_XML_PARENT_ATTRIBUTE			"parent"
#define SHAPE_XML_PEN_COLOUR_ATTRIBUTE		"pen_colour"
#define SHAPE_XML_PEN_STYLE_ATTRIBUTE		"pen_style"
#define SHAPE_XML_POINT_ELEMENT				"point"
#define SHAPE_XML_POINT_LIST_ELEMENT		"point_list"
#define SHAPE_XML_POINTSIZE_ATTRIBUTE		"point_size"
#define SHAPE_XML_PROPORTION_X_ATTRIBUTE	"proportion_x"
#define SHAPE_XML_PROPORTION_Y_ATTRIBUTE	"proportion_y"
#define	SHAPE_XML_ROTATION_ATTRIBUTE		"rotation"
#define	SHAPE_XML_SENSITIVITY_ATTRIBUTE		"sensitivity"
#define	SHAPE_XML_SHADOW_MODE_ATTRIBUTE		"shadow_mode"
#define	SHAPE_XML_SIZE_ATTRIBUTE			"size"
#define	SHAPE_XML_SPACE_ATTACHMENTS_ATTRIBUTE	"space_attachments"
#define	SHAPE_XML_SPACING_ATTRIBUTE			"spacing"
#define SHAPE_XML_STYLE_ATTRIBUTE			"style"
#define	SHAPE_XML_STEM_LENGTH_ATTRIBUTE		"stem_length"
#define	SHAPE_XML_TO_ATTRIBUTE				"to"
#define SHAPE_XML_TEXT_COLOUR_ATTRIBUTE		"text_colour"
#define SHAPE_XML_TYPE_ATTRIBUTE			"type"
#define	SHAPE_XML_USE_ATTACHMENTS_ATTRIBUTE	"use_attachments"
#define	SHAPE_XML_WEIGHT_ATTRIBUTE			"weight"
#define	SHAPE_XML_WIDTH_ATTRIBUTE			"width"
#define	SHAPE_XML_X_ATTRIBUTE				"x"
#define	SHAPE_XML_X_OFFSET_ATTRIBUTE		"x_offset"
#define	SHAPE_XML_Y_ATTRIBUTE				"y"
#define	SHAPE_XML_Y_OFFSET_ATTRIBUTE		"y_offset"


/******************************************************************************/
/*************************** Macro definitions ********************************/
/******************************************************************************/

#define ATTRIBUTE_VAL(ELMNT, ATTR, X, WARN) \
	{ \
		BackupVar	oldVar = BackupVar(X); \
		if ((ELMNT)->Attribute((ATTR), &(X)) == NULL) { \
			X = oldVar.Value(X); \
			if (WARN) { \
				NotifyWarning(wxT("%s: Could not get '%s' attribute value."), \
				  funcName, (ATTR)); \
				ok = false; \
			} \
		} \
	}

#define STR_ATTRIBUTE_VAL(ELMNT, ATTR, X, WARN) \
	(X) = (wxChar *) (ELMNT)->Attribute(ATTR); \
	if ((X).IsEmpty() && (WARN)) { \
		NotifyWarning(wxT("%s: Could not get '%s' attribute value."), \
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
