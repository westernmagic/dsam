/**********************
 *
 * File:		GrSDIXMLDoc.cpp
 * Purpose: 	Simulation Design Interface base XML class.
 * Comments:	Contains a revision of the wxExpr code for saving the SDI.
 * Author:		L.P.O'Mard
 * Created:		14 Feb 2005
 * Updated:		
 * Copyright:	(c) 2005, University of Essex
 *
 **********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>

#include "ExtCommon.h"

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"

#include "UtDatum.h"
#include "UtSSSymbols.h"
#include "UtSSParser.h"
#include "UtSimScript.h"
#include "UtAppInterface.h"
#include "tinyxml.h"

#include "GrSDIShapes.h"
#include "GrSDIXMLDoc.h"

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
/****************************** Constructor ***********************************/
/******************************************************************************/

SDIXMLDocument::SDIXMLDocument(void)
{

}

/******************************************************************************/
/****************************** Destructor ************************************/
/******************************************************************************/

SDIXMLDocument::~SDIXMLDocument(void)
{

}

/******************************************************************************/
/****************************** AddRegions ************************************/
/******************************************************************************/

void
SDIXMLDocument::AddRegions(TiXmlNode &parent, wxShape *shape)
{
	int 	regionNo = 1;
	wxNode *node = shape->GetRegions().GetFirst();
	TiXmlElement regionsElement(SDI_XML_REGIONS_ELEMENT);

	while (node) {
		wxShapeRegion *region = (wxShapeRegion *) node->GetData();

		TiXmlElement regionElement(SDI_XML_REGION_ELEMENT);
		regionElement.SetAttribute(SDI_XML_ID_ATTRIBUTE, regionNo);

		regionElement.SetDoubleAttribute(SDI_XML_X_ATTRIBUTE, region->m_x);
		regionElement.SetDoubleAttribute(SDI_XML_Y_ATTRIBUTE, region->m_y);
		regionElement.SetDoubleAttribute(SDI_XML_WIDTH_ATTRIBUTE, region->
		  GetWidth());
		regionElement.SetDoubleAttribute(SDI_XML_HEIGHT_ATTRIBUTE, region->
		  GetHeight());

		regionElement.SetDoubleAttribute(SDI_XML_MIN_WIDTH_ATTRIBUTE, region->
		  m_minWidth);
		regionElement.SetDoubleAttribute(SDI_XML_MIN_HEIGHT_ATTRIBUTE, region->
		  m_minHeight);
		regionElement.SetDoubleAttribute(SDI_XML_PROPORTION_X_ATTRIBUTE, region->
		  m_regionProportionX);
		regionElement.SetDoubleAttribute(SDI_XML_PROPORTION_Y_ATTRIBUTE, region->
		  m_regionProportionY);

		regionElement.SetAttribute(SDI_XML_FORMAT_MODE_ATTRIBUTE, region->
		  m_formatMode);

		regionElement.SetAttribute(SDI_XML_POINTSIZE_ATTRIBUTE,region->
		  m_font ? region->m_font->GetPointSize() : 10);
		regionElement.SetAttribute(SDI_XML_FAMILY_ATTRIBUTE, (long)(region->
		  m_font ? region->m_font->GetFamily() : wxDEFAULT));
		regionElement.SetAttribute(SDI_XML_STYLE_ATTRIBUTE, (long)(region->
		  m_font ? region->m_font->GetStyle() : wxDEFAULT));
		regionElement.SetAttribute(SDI_XML_WEIGHT_ATTRIBUTE, (long)(region->
		  m_font ? region->m_font->GetWeight() : wxNORMAL));
		regionElement.SetAttribute(SDI_XML_TEXT_COLOUR_ATTRIBUTE, region->
		  m_textColour);

		// New members for pen colour/style
		regionElement.SetAttribute(SDI_XML_PEN_COLOUR_ATTRIBUTE, region->
		  m_penColour);
		regionElement.SetAttribute(SDI_XML_PEN_STYLE_ATTRIBUTE, (long)region->
		  m_penStyle);

		// Formatted text:
		// text1 = ((x y string) (x y string) ...)
	
		wxNode *textNode = region->m_formattedText.GetFirst();
		while (textNode) {
			TiXmlElement textElement(SDI_XML_TEXT_ELEMENT);
			wxShapeTextLine *line = (wxShapeTextLine *)textNode->GetData();
			textElement.SetDoubleAttribute(SDI_XML_X_ATTRIBUTE, line->GetX());
			textElement.SetDoubleAttribute(SDI_XML_Y_ATTRIBUTE, line->GetY());
			TiXmlText text(line->GetText());
			textElement.InsertEndChild(text);
			regionElement.InsertEndChild(textElement);
			textNode = textNode->GetNext();
		}

		regionsElement.InsertEndChild(regionElement);

		node = node->GetNext();
		regionNo++;
	}
	if (regionsElement.FirstChild())
		parent.InsertEndChild(regionsElement);

}

/******************************************************************************/
/****************************** AddPenInfo ************************************/
/******************************************************************************/

void
SDIXMLDocument::AddPenInfo(TiXmlNode &parent, wxShape *shape)
{
	wxPen	*myPen;

	if ((myPen = shape->GetPen()) == NULL)
		return;
	TiXmlElement penElement(SDI_XML_PEN_ELEMENT);
	if (myPen->GetWidth() != 1)
		penElement.SetAttribute(SDI_XML_WIDTH_ATTRIBUTE, myPen->GetWidth());
	if ( myPen->GetStyle() != wxSOLID)
		penElement.SetAttribute(SDI_XML_STYLE_ATTRIBUTE, myPen->GetStyle());
	wxString penColour = wxTheColourDatabase->FindName(myPen->GetColour());
	if (penColour == wxEmptyString) {
		wxString hex(oglColourToHex(myPen->GetColour()));
		hex = wxString(_T("#")) + hex;
		penElement.SetAttribute(SDI_XML_COLOUR_ATTRIBUTE, hex);
	} else if (penColour != _T("BLACK"))
		penElement.SetAttribute(SDI_XML_COLOUR_ATTRIBUTE, penColour);
	if (penElement.FirstAttribute())
		parent.InsertEndChild(penElement);

}

/******************************************************************************/
/****************************** AddBrushInfo **********************************/
/******************************************************************************/

void
SDIXMLDocument::AddBrushInfo(TiXmlNode &parent, wxShape *shape)
{
	wxBrush	*myBrush;

	if ((myBrush = shape->GetBrush()) == NULL)
		return;
	TiXmlElement brushElement(SDI_XML_BRUSH_ELEMENT);
	wxString brushColour = wxTheColourDatabase->FindName(myBrush->GetColour(
	  ));

	if (brushColour == wxEmptyString) {
		wxString hex(oglColourToHex(myBrush->GetColour()));
		hex = wxString(_T("#")) + hex;
		brushElement.SetAttribute(SDI_XML_COLOUR_ATTRIBUTE, hex);
	} else if (brushColour != _T("WHITE"))
		brushElement.SetAttribute(SDI_XML_COLOUR_ATTRIBUTE, brushColour);

	if (myBrush->GetStyle() != wxSOLID)
		brushElement.SetAttribute(SDI_XML_TYPE_ATTRIBUTE, myBrush->GetStyle(
		  ));
	if (brushElement.FirstAttribute())
		parent.InsertEndChild(brushElement);

}

/******************************************************************************/
/****************************** AddLineInfo **********************************/
/******************************************************************************/

void
SDIXMLDocument::AddLineInfo(TiXmlNode &parent, wxShape *shape)
{
	if (!shape->GetLines().GetCount())
		return;
	TiXmlElement arcsElement(SDI_XML_ARCS_ELEMENT);
	wxNode *node = shape->GetLines().GetFirst();
	while (node) {
		TiXmlElement lineElement(SDI_XML_LINE_ELEMENT);
		wxShape *line = (wxShape *) node->GetData();
		lineElement.SetAttribute(SDI_XML_ID_ATTRIBUTE, line->GetId());
		arcsElement.InsertEndChild(lineElement);
		node = node->GetNext();
	}
	if (arcsElement.FirstChild())
		parent.InsertEndChild(arcsElement);

}

/******************************************************************************/
/****************************** AddAttachmentsInfo ****************************/
/******************************************************************************/

void
SDIXMLDocument::AddAttachmentsInfo(TiXmlNode &parent, wxShape *shape)
{
	if (!shape->GetAttachments().GetCount())
		return;
	TiXmlElement attachmentsElement(SDI_XML_ATTACHMENTS_ELEMENT);
	wxNode *node = shape->GetAttachments().GetFirst();
	while (node) {
		wxAttachmentPoint *point = (wxAttachmentPoint *)node->GetData();
		TiXmlElement pointElement(SDI_XML_POINT_ELEMENT);
		pointElement.SetAttribute(SDI_XML_ID_ATTRIBUTE, point->m_id);
		pointElement.SetDoubleAttribute(SDI_XML_X_ATTRIBUTE, point->m_x);
		pointElement.SetDoubleAttribute(SDI_XML_Y_ATTRIBUTE, point->m_y);
		attachmentsElement.InsertEndChild(pointElement);
		node = node->GetNext();
	}
	if (attachmentsElement.FirstChild())
		parent.InsertEndChild(attachmentsElement);

}

/******************************************************************************/
/****************************** AddPointInfo **********************************/
/******************************************************************************/

void
SDIXMLDocument::AddPointInfo(TiXmlNode &parent, wxRealPoint *point)
{
	TiXmlElement pointElement(SDI_XML_POINT_ELEMENT);
	pointElement.SetDoubleAttribute(SDI_XML_X_ATTRIBUTE, point->x);
	pointElement.SetDoubleAttribute(SDI_XML_Y_ATTRIBUTE, point->y);
	parent.InsertEndChild(pointElement);

}

/******************************************************************************/
/****************************** AddAnalysisShapeInfo **************************/
/******************************************************************************/

void
SDIXMLDocument::AddAnalysisShapeInfo(TiXmlNode &parent, wxShape *shape)
{
	SDIAnalysisShape *myShape = (SDIAnalysisShape *) shape;
	TiXmlElement myElement(SDI_XML_ANALYSIS_SHAPE_ELEMENT);
	myElement.SetDoubleAttribute(SDI_XML_X_ATTRIBUTE, myShape->GetX());
	myElement.SetDoubleAttribute(SDI_XML_Y_ATTRIBUTE, myShape->GetY());
	myElement.SetDoubleAttribute(SDI_XML_WIDTH_ATTRIBUTE, myShape->GetWidth());
	myElement.SetDoubleAttribute(SDI_XML_HEIGHT_ATTRIBUTE, myShape->GetHeight(
	  ));
	parent.InsertEndChild(myElement);

}

/******************************************************************************/
/****************************** AddPolygonShapeInfo ***************************/
/******************************************************************************/

void
SDIXMLDocument::AddPolygonShapeInfo(TiXmlElement &parent, wxShape *shape)
{
	wxPolygonShape *myShape = (wxPolygonShape *) shape;
	parent.SetDoubleAttribute(SDI_XML_X_ATTRIBUTE, myShape->GetX());
	parent.SetDoubleAttribute(SDI_XML_Y_ATTRIBUTE, myShape->GetY());

	wxObjectList::compatibility_iterator node = myShape->GetPoints()->GetFirst(
	  );
	TiXmlElement pointListElement(SDI_XML_POINT_LIST_ELEMENT);
	pointListElement.SetAttribute(SDI_XML_ID_ATTRIBUTE,
	  SDI_XML_CURRENT_ATTRIBUTE_VALUE);
	while (node) {
		wxRealPoint *point = (wxRealPoint *)node->GetData();
		AddPointInfo(pointListElement, point);
		node = node->GetNext();
	}
	parent.InsertEndChild(pointListElement);
	TiXmlElement originalPointListElement(SDI_XML_POINT_LIST_ELEMENT);
	originalPointListElement.SetAttribute(SDI_XML_ID_ATTRIBUTE,
	  SDI_XML_ORIGINAL_ATTRIBUTE_VALUE);
	// Save the original (unscaled) points
	node = myShape->GetOriginalPoints()->GetFirst();
	while (node) {
		wxRealPoint *point = (wxRealPoint *)node->GetData();
		AddPointInfo(originalPointListElement, point);
		node = node->GetNext();
	}
	parent.InsertEndChild(originalPointListElement);

}

/******************************************************************************/
/****************************** AddControlShapeInfo ***************************/
/******************************************************************************/

void
SDIXMLDocument::AddControlShapeInfo(TiXmlNode &parent, wxShape *shape)
{
	TiXmlElement myElement(SDI_XML_CONTROL_SHAPE_ELEMENT);
	AddPolygonShapeInfo(myElement, shape);
	parent.InsertEndChild(myElement);

}

/******************************************************************************/
/****************************** AddFilterShapeInfo ****************************/
/******************************************************************************/

void
SDIXMLDocument::AddFilterShapeInfo(TiXmlNode &parent, wxShape *shape)
{
	TiXmlElement myElement(SDI_XML_FILTER_SHAPE_ELEMENT);
	AddPolygonShapeInfo(myElement, shape);
	parent.InsertEndChild(myElement);

}

/******************************************************************************/
/****************************** AddIOShapeInfo ********************************/
/******************************************************************************/

void
SDIXMLDocument::AddIOShapeInfo(TiXmlNode &parent, wxShape *shape)
{
	TiXmlElement myElement(SDI_XML_IO_SHAPE_ELEMENT);
	AddPolygonShapeInfo(myElement, shape);
	parent.InsertEndChild(myElement);

}

/******************************************************************************/
/****************************** AddModelShapeInfo *****************************/
/******************************************************************************/

void
SDIXMLDocument::AddModelShapeInfo(TiXmlNode &parent, wxShape *shape)
{
	TiXmlElement myElement(SDI_XML_MODEL_SHAPE_ELEMENT);
	AddPolygonShapeInfo(myElement, shape);
	parent.InsertEndChild(myElement);

}

/******************************************************************************/
/****************************** AddUserShapeInfo ******************************/
/******************************************************************************/

void
SDIXMLDocument::AddUserShapeInfo(TiXmlNode &parent, wxShape *shape)
{
	TiXmlElement myElement(SDI_XML_USER_SHAPE_ELEMENT);
	AddPolygonShapeInfo(myElement, shape);
	parent.InsertEndChild(myElement);

}

/******************************************************************************/
/****************************** AddTransformShapeInfo *************************/
/******************************************************************************/

void
SDIXMLDocument::AddTransformShapeInfo(TiXmlNode &parent, wxShape *shape)
{
	TiXmlElement myElement(SDI_XML_TRANSFORM_SHAPE_ELEMENT);
	AddPolygonShapeInfo(myElement, shape);
	parent.InsertEndChild(myElement);

}

/******************************************************************************/
/****************************** AddUtilityShapeInfo ***************************/
/******************************************************************************/

void
SDIXMLDocument::AddUtilityShapeInfo(TiXmlNode &parent, wxShape *shape)
{
	SDIUtilityShape *myShape = (SDIUtilityShape *) shape;
	TiXmlElement myElement(SDI_XML_UTILITY_SHAPE_ELEMENT);
	myElement.SetDoubleAttribute(SDI_XML_X_ATTRIBUTE, myShape->GetX());
	myElement.SetDoubleAttribute(SDI_XML_Y_ATTRIBUTE, myShape->GetY());
	myElement.SetDoubleAttribute(SDI_XML_WIDTH_ATTRIBUTE, myShape->GetWidth());
	myElement.SetDoubleAttribute(SDI_XML_HEIGHT_ATTRIBUTE, myShape->GetHeight(
	  ));
	if (myShape->GetCornerRadius() != 0.0)
		myElement.SetDoubleAttribute(SDI_XML_CORNER_ATTRIBUTE, myShape->
		  GetCornerRadius());

	parent.InsertEndChild(myElement);

}

/******************************************************************************/
/****************************** AddShapeInfo **********************************/
/******************************************************************************/

void
SDIXMLDocument::AddShapeInfo(TiXmlNode &node, void *shapePtr)
{
	wxShape *shape = (wxShape *) shapePtr;
	TiXmlElement shapeElement(SDI_XML_SHAPE_ELEMENT);
	AddPenInfo(shapeElement, shape);
	AddBrushInfo(shapeElement, shape);
	AddLineInfo(shapeElement, shape);

	// Misc. attributes
	if (shape->GetAttachmentMode() != 0)
		shapeElement.SetAttribute(SDI_XML_USE_ATTACHMENTS_ATTRIBUTE, shape->
		  GetAttachmentMode());
	if (shape->GetSensitivityFilter() != OP_ALL)
		shapeElement.SetAttribute(SDI_XML_SENSITIVITY_ATTRIBUTE, shape->
		  GetSensitivityFilter());
	if (!shape->GetSpaceAttachments())
		shapeElement.SetAttribute(SDI_XML_SPACE_ATTACHMENTS_ATTRIBUTE,
		  shape->GetSpaceAttachments());
	if (shape->GetFixedWidth())
		shapeElement.SetAttribute(SDI_XML_FIXED_WIDTH_ATTRIBUTE, shape->
		  GetFixedWidth());
	if (shape->GetFixedHeight())
		shapeElement.SetAttribute(SDI_XML_FIXED_HEIGHT_ATTRIBUTE, shape->
		  GetFixedHeight());
	if (shape->GetShadowMode() != SHADOW_NONE)
		shapeElement.SetAttribute(SDI_XML_SHADOW_MODE_ATTRIBUTE, shape->
		  GetShadowMode());
	if (shape->GetCentreResize() != true)
		shapeElement.SetAttribute(SDI_XML_CENTRE_RESIZE_ATTRIBUTE, 0);
	shapeElement.SetAttribute(SDI_XML_MAINTAIN_ASPECT_RATIO_ATTRIBUTE,
	  shape->GetMaintainAspectRatio());
	if (shape->IsHighlighted() != false)
		shapeElement.SetAttribute(SDI_XML_HILITE_ATTRIBUTE, shape->
		  IsHighlighted());

	if (shape->GetParent()) // For composite objects
		shapeElement.SetAttribute(SDI_XML_PARENT_ATTRIBUTE, shape->GetParent()->
		  GetId());
	if (shape->GetRotation() != 0.0)
		shapeElement.SetAttribute(SDI_XML_ROTATION_ATTRIBUTE, (int)shape->
		  GetRotation());

	if (!shape->IsKindOf(CLASSINFO(wxLineShape))) {
		shapeElement.SetAttribute(SDI_XML_NECK_LENGTH_ATTRIBUTE, shape->
		  GetBranchNeckLength());
		shapeElement.SetAttribute(SDI_XML_STEM_LENGTH_ATTRIBUTE, shape->
		  GetBranchStemLength());
		shapeElement.SetAttribute(SDI_XML_BRANCH_SPACING_ATTRIBUTE, shape->
		  GetBranchSpacing());
		shapeElement.SetAttribute(SDI_XML_BRANCH_STYLE_ATTRIBUTE, shape->
		  GetBranchStyle());
	}

	AddAttachmentsInfo(shapeElement, shape);
	AddRegions(shapeElement, shape);
	if (shape->IsKindOf(CLASSINFO(SDIAnalysisShape)))
		AddAnalysisShapeInfo(shapeElement, shape);
	if (shape->IsKindOf(CLASSINFO(SDIControlShape)))
		AddControlShapeInfo(shapeElement, shape);;
	if (shape->IsKindOf(CLASSINFO(SDIFilterShape)))
		AddFilterShapeInfo(shapeElement, shape);;
	if (shape->IsKindOf(CLASSINFO(SDIIOShape)))
		AddIOShapeInfo(shapeElement, shape);;
	if (shape->IsKindOf(CLASSINFO(SDIModelShape)))
		AddModelShapeInfo(shapeElement, shape);;
	if (shape->IsKindOf(CLASSINFO(SDIUserShape)))
		AddUserShapeInfo(shapeElement, shape);;
	if (shape->IsKindOf(CLASSINFO(SDITransformShape)))
		AddTransformShapeInfo(shapeElement, shape);;
	if (shape->IsKindOf(CLASSINFO(SDIUtilityShape)))
		AddUtilityShapeInfo(shapeElement, shape);;

	node.InsertEndChild(shapeElement);
}

