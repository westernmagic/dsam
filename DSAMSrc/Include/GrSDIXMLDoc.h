/**********************
 *
 * File:		GrSDIXMLDoc.h
 * Purpose: 	Simulation Design Interface base XML class.
 * Comments:	Contains a revision of the wxExpr code for saving the SDI.
 * Author:		L.P.O'Mard
 * Created:		14 Feb 2005
 * Updated:		
 * Copyright:	(c) 2005, University of Essex
 *
 **********************/

#ifndef _GRSDIXMLDOC_H
#define _GRSDIXMLDOC_H 1

#ifdef __GNUG__
// #pragma interface
#endif

#include <wx/string.h>

#include <wx/ogl/ogl.h>

#if wxUSE_STD_IOSTREAM
class ostream;
class istream;
#endif

#include "ExtXMLDocument.h"

#include "UtDynaList.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

// Shape elements.
#define SDI_XML_ANALYSIS_SHAPE_ELEMENT	_T("analysis_shape")
#define SDI_XML_CONTROL_SHAPE_ELEMENT	_T("control_shape")
#define SDI_XML_DISPLAY_SHAPE_ELEMENT	_T("display_shape")
#define SDI_XML_FILTER_SHAPE_ELEMENT	_T("filter_shape")
#define SDI_XML_IO_SHAPE_ELEMENT		_T("io_shape")
#define SDI_XML_MODEL_SHAPE_ELEMENT		_T("model_shape")
#define SDI_XML_USER_SHAPE_ELEMENT		_T("user_shape")
#define SDI_XML_TRANSFORM_SHAPE_ELEMENT	_T("transform_shape")
#define SDI_XML_UTILITY_SHAPE_ELEMENT	_T("utility_shape")

// Misc. elements.
#define SDI_XML_ARCS_ELEMENT			_T("arcs")
#define SDI_XML_ATTACHMENTS_ELEMENT		_T("attachments")
#define SDI_XML_BRUSH_ELEMENT			_T("brush")
#define SDI_XML_INPUT_ELEMENT			_T("input")
#define SDI_XML_LINE_ELEMENT			_T("line")
#define SDI_XML_MISC_ELEMENT			_T("misc")
#define SDI_XML_OBJECT_ELEMENT			_T("object")
#define SDI_XML_OUTPUT_ELEMENT			_T("output")
#define SDI_XML_PEN_ELEMENT				_T("pen")
#define SDI_XML_POINT_ELEMENT			_T("point")
#define SDI_XML_POINT_LIST_ELEMENT		_T("point_list")
#define SDI_XML_REGION_ELEMENT			_T("region")
#define SDI_XML_REGIONS_ELEMENT			_T("regions")
#define SDI_XML_SHAPE_ELEMENT			_T("shape")

#define SDI_XML_TEXT_ELEMENT			_T("text")

#define	SDI_XML_BRANCH_SPACING_ATTRIBUTE	_T("branch_spacing")
#define	SDI_XML_BRANCH_STYLE_ATTRIBUTE	_T("branch_style")
#define	SDI_XML_CENTRE_RESIZE_ATTRIBUTE	_T("centre_resize")
#define	SDI_XML_COLOUR_ATTRIBUTE		_T("colour")
#define SDI_XML_CORNER_ATTRIBUTE		_T("corner")
#define SDI_XML_COUNT_ATTRIBUTE			_T("count")
#define SDI_XML_FAMILY_ATTRIBUTE		_T("family")
#define	SDI_XML_FIXED_WIDTH_ATTRIBUTE	_T("fixed_width")
#define	SDI_XML_FIXED_HEIGHT_ATTRIBUTE	_T("fixed_height")
#define SDI_XML_FORMAT_MODE_ATTRIBUTE	_T("format_mode")
#define SDI_XML_HEIGHT_ATTRIBUTE		_T("height")
#define	SDI_XML_HILITE_ATTRIBUTE		_T("hilite")
#define SDI_XML_ID_ATTRIBUTE			_T("id")
#define SDI_XML_OBJLABEL_ATTRIBUTE		_T("obj_label")
#define	SDI_XML_STYLE_ATTRIBUTE			_T("style")
#define	SDI_XML_MIN_HEIGHT_ATTRIBUTE	_T("min_height")
#define	SDI_XML_MIN_WIDTH_ATTRIBUTE		_T("min_width")
#define SDI_XML_NAME_ATTRIBUTE			_T("name")
#define SDI_XML_PEN_COLOUR_ATTRIBUTE	_T("pen_colour")
#define SDI_XML_PEN_STYLE_ATTRIBUTE		_T("pen_style")
#define SDI_XML_POINTSIZE_ATTRIBUTE		_T("point_size")
#define SDI_XML_PROPORTION_X_ATTRIBUTE	_T("proportion_x")
#define SDI_XML_PROPORTION_Y_ATTRIBUTE	_T("proportion_y")
#define SDI_XML_STYLE_ATTRIBUTE			_T("style")
#define SDI_XML_TEXT_COLOUR_ATTRIBUTE	_T("text_colour")
#define SDI_XML_TYPE_ATTRIBUTE			_T("type")
#define	SDI_XML_WEIGHT_ATTRIBUTE		_T("weight")
#define	SDI_XML_WIDTH_ATTRIBUTE			_T("width")
#define	SDI_XML_X_ATTRIBUTE				_T("x")
#define	SDI_XML_Y_ATTRIBUTE				_T("y")

#define	SDI_XML_MAINTAIN_ASPECT_RATIO_ATTRIBUTE		_T("maintain_aspect_ratio")
#define	SDI_XML_NECK_LENGTH_ATTRIBUTE		_T("neck_length")
#define	SDI_XML_PARENT_ATTRIBUTE			_T("parent")
#define	SDI_XML_ROTATION_ATTRIBUTE			_T("rotation")
#define	SDI_XML_SENSITIVITY_ATTRIBUTE		_T("sensitivity")
#define	SDI_XML_SHADOW_MODE_ATTRIBUTE		_T("shadow_mode")
#define	SDI_XML_SPACE_ATTACHMENTS_ATTRIBUTE	_T("space_attachments")
#define	SDI_XML_STEM_LENGTH_ATTRIBUTE		_T("stem_length")
#define	SDI_XML_USE_ATTACHMENTS_ATTRIBUTE	_T("use_attachments")

#define SDI_XML_CURRENT_ATTRIBUTE_VALUE		_T("current")
#define SDI_XML_PROCESS_ATTRIBUTE_VALUE		_T("process")
#define SDI_XML_ORIGINAL_ATTRIBUTE_VALUE	_T("original")

/******************************************************************************/
/*************************** Enum definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Pre reference definitions ************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** SDIDocument **************************************/

class SDIXMLDocument: public DSAMXMLDocument {

  private:

  public:
  
  	SDIXMLDocument();
	~SDIXMLDocument(void);

	void	AddAnalysisShapeInfo(TiXmlNode &parent, wxShape *shape);
	void	AddAttachmentsInfo(TiXmlNode &parent, wxShape *shape);
	void	AddBrushInfo(TiXmlNode &parent, wxShape *shape);
	void	AddControlShapeInfo(TiXmlNode &parent, wxShape *shape);
	void	AddFilterShapeInfo(TiXmlNode &parent, wxShape *shape);
	void	AddIOShapeInfo(TiXmlNode &parent, wxShape *shape);
	void	AddLineInfo(TiXmlNode &parent, wxShape *shape);
	void	AddModelShapeInfo(TiXmlNode &parent, wxShape *shape);
	void	AddPenInfo(TiXmlNode &parent, wxShape *shape);
	void	AddPointInfo(TiXmlNode &parent,wxRealPoint *point);
	void	AddPolygonShapeInfo(TiXmlElement &parent, wxShape *shape);
	void	AddRegions(TiXmlNode &parent, wxShape *shape);
	void	AddShapeInfo(TiXmlNode &node, void *shapePtr);
	void	AddTransformShapeInfo(TiXmlNode &parent, wxShape *shape);
	void	AddUserShapeInfo(TiXmlNode &parent, wxShape *shape);
	void	AddUtilityShapeInfo(TiXmlNode &parent, wxShape *shape);

};

#endif
