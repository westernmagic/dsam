/**********************
 *
 * File:		GrSDIBaseShapes.cpp
 * Purpose: 	Base Shape classes for Simulation Design Interface.
 * Comments:	Revised from Julian Smart's Ogledit/doc.h
 *				This code was moved from the GrSDIShapes code module.
 * Author:		L.P.O'Mard
 * Created:		10 Apr 2005
 * Updated:		
 * Copyright:	(c) 2005, CNBH, University of Essex
 *
 **********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#ifdef USE_WX_OGL

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

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GrSDIBaseShapes.h"
#include "GrSDIShapes.h"
#include "UtSSSymbols.h"
#include "ExtXMLDocument.h"

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
/****************************** Basic Shape Methods ***************************/
/******************************************************************************/

/******************************************************************************/
/****************************** SDIShape Methods ******************************/
/******************************************************************************/

IMPLEMENT_ABSTRACT_CLASS(SDIShape, wxShape)

/****************************** Constructor ***********************************/

SDIShape::SDIShape(wxShapeCanvas *can): wxShape(can)
{
}

/******************************************************************************/
/****************************** AddPenInfo ************************************/
/******************************************************************************/

void
SDIShape::AddPenInfo(TiXmlNode &parent)
{
	wxPen	*myPen;

	if ((myPen = GetPen()) == NULL)
		return;
	TiXmlElement penElement(SHAPE_XML_PEN_ELEMENT);
	if (myPen->GetWidth() != 1)
		penElement.SetAttribute(SHAPE_XML_WIDTH_ATTRIBUTE, myPen->GetWidth());
	if ( myPen->GetStyle() != wxSOLID)
		penElement.SetAttribute(SHAPE_XML_STYLE_ATTRIBUTE, myPen->GetStyle());
	wxString penColour = wxTheColourDatabase->FindName(myPen->GetColour());
	if (penColour == wxEmptyString) {
		wxString hex(oglColourToHex(myPen->GetColour()));
		hex = wxString(wxT("#")) + hex;
		penElement.SetAttribute(SHAPE_XML_COLOUR_ATTRIBUTE, (const char *) hex.
		  mb_str());
	} else if (penColour != wxT("BLACK"))
		penElement.SetAttribute(SHAPE_XML_COLOUR_ATTRIBUTE, (const char *)
		  penColour.mb_str());
	if (penElement.FirstAttribute())
		parent.InsertEndChild(penElement);

}

/******************************************************************************/
/****************************** AddBrushInfo **********************************/
/******************************************************************************/

void
SDIShape::AddBrushInfo(TiXmlNode &parent)
{
	wxBrush	*myBrush;

	if ((myBrush = GetBrush()) == NULL)
		return;
	TiXmlElement brushElement(SHAPE_XML_BRUSH_ELEMENT);
	wxString brushColour = wxTheColourDatabase->FindName(myBrush->GetColour(
	  ));

	if (brushColour == wxEmptyString) {
		wxString hex(oglColourToHex(myBrush->GetColour()));
		hex = wxString(wxT("#")) + hex;
		brushElement.SetAttribute(SHAPE_XML_COLOUR_ATTRIBUTE, (const char *)
		  hex.mb_str());
	} else if (brushColour != wxT("WHITE"))
		brushElement.SetAttribute(SHAPE_XML_COLOUR_ATTRIBUTE, (const char *)
		  brushColour.mb_str());

	if (myBrush->GetStyle() != wxSOLID)
		brushElement.SetAttribute(SHAPE_XML_TYPE_ATTRIBUTE, myBrush->GetStyle(
		  ));
	if (brushElement.FirstAttribute())
		parent.InsertEndChild(brushElement);

}

/******************************************************************************/
/****************************** AddLineInfo ***********************************/
/******************************************************************************/

void
SDIShape::AddLineInfo(TiXmlNode &parent)
{
	if (!GetLines().GetCount())
		return;
	TiXmlElement arcsElement(SHAPE_XML_ARCS_ELEMENT);
	wxNode *node = GetLines().GetFirst();
	while (node) {
		TiXmlElement lineElement(SHAPE_XML_LINE_ELEMENT);
		wxShape *line = (wxShape *) node->GetData();
		lineElement.SetAttribute(DSAM_XML_ID_ATTRIBUTE, line->GetId());
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
SDIShape::AddAttachmentsInfo(TiXmlNode &parent)
{
	if (!GetAttachments().GetCount())
		return;
	TiXmlElement attachmentsElement(SHAPE_XML_ATTACHMENTS_ELEMENT);
	wxNode *node = GetAttachments().GetFirst();
	while (node) {
		wxAttachmentPoint *point = (wxAttachmentPoint *)node->GetData();
		TiXmlElement pointElement(SHAPE_XML_POINT_ELEMENT);
		pointElement.SetAttribute(DSAM_XML_ID_ATTRIBUTE, point->m_id);
		pointElement.SetDoubleAttribute(SHAPE_XML_X_ATTRIBUTE, point->m_x);
		pointElement.SetDoubleAttribute(SHAPE_XML_Y_ATTRIBUTE, point->m_y);
		attachmentsElement.InsertEndChild(pointElement);
		node = node->GetNext();
	}
	if (attachmentsElement.FirstChild())
		parent.InsertEndChild(attachmentsElement);

}

/******************************************************************************/
/****************************** AddFontInfo ***********************************/
/******************************************************************************/

void
SDIShape::AddFontInfo(TiXmlNode &parent, wxFont *font)
{
	TiXmlElement fontElement(SHAPE_XML_FONT_ELEMENT);

	fontElement.SetAttribute(SHAPE_XML_POINTSIZE_ATTRIBUTE, (font)?
	  font->GetPointSize() : SHAPE_DEFAULT_FONT_POINTSIZE);
	fontElement.SetAttribute(SHAPE_XML_FAMILY_ATTRIBUTE, (long)((font)?
	  font->GetFamily() : wxDEFAULT));
	fontElement.SetAttribute(SHAPE_XML_STYLE_ATTRIBUTE, (long)((font)?
	  font->GetStyle() : wxDEFAULT));
	fontElement.SetAttribute(SHAPE_XML_WEIGHT_ATTRIBUTE, (long)((font)?
	  font->GetWeight() : wxNORMAL));
	parent.InsertEndChild(fontElement);

}

/******************************************************************************/
/****************************** AddRegions ************************************/
/******************************************************************************/

void
SDIShape::AddRegions(TiXmlNode &parent)
{
	wxNode *node = GetRegions().GetFirst();
	TiXmlElement regionsElement(SHAPE_XML_REGIONS_ELEMENT);

	while (node) {
		wxShapeRegion *region = (wxShapeRegion *) node->GetData();

		TiXmlElement regionElement(SHAPE_XML_REGION_ELEMENT);

		regionElement.SetDoubleAttribute(SHAPE_XML_X_ATTRIBUTE, region->m_x);
		regionElement.SetDoubleAttribute(SHAPE_XML_Y_ATTRIBUTE, region->m_y);
		regionElement.SetDoubleAttribute(SHAPE_XML_WIDTH_ATTRIBUTE, region->
		  GetWidth());
		regionElement.SetDoubleAttribute(SHAPE_XML_HEIGHT_ATTRIBUTE, region->
		  GetHeight());

		regionElement.SetDoubleAttribute(SHAPE_XML_MIN_WIDTH_ATTRIBUTE, region->
		  m_minWidth);
		regionElement.SetDoubleAttribute(SHAPE_XML_MIN_HEIGHT_ATTRIBUTE,
		  region->m_minHeight);
		regionElement.SetDoubleAttribute(SHAPE_XML_PROPORTION_X_ATTRIBUTE,
		  region->m_regionProportionX);
		regionElement.SetDoubleAttribute(SHAPE_XML_PROPORTION_Y_ATTRIBUTE,
		  region->m_regionProportionY);

		regionElement.SetAttribute(SHAPE_XML_FORMAT_MODE_ATTRIBUTE, region->
		  m_formatMode);

		AddFontInfo(regionElement, region->m_font);

		regionElement.SetAttribute(SHAPE_XML_TEXT_COLOUR_ATTRIBUTE, (const 
		  char *) region->m_textColour.mb_str());

		// New members for pen colour/style
		regionElement.SetAttribute(SHAPE_XML_PEN_COLOUR_ATTRIBUTE, (const 
		  char *) region->m_penColour.mb_str());
		regionElement.SetAttribute(SHAPE_XML_PEN_STYLE_ATTRIBUTE, (long)region->
		  m_penStyle);

		// Formatted text:
		// text1 = ((x y string) (x y string) ...)
	
		wxNode *textNode = region->m_formattedText.GetFirst();
		while (textNode) {
			TiXmlElement textElement(SHAPE_XML_TEXT_ELEMENT);
			wxShapeTextLine *line = (wxShapeTextLine *)textNode->GetData();
			textElement.SetDoubleAttribute(SHAPE_XML_X_ATTRIBUTE, line->GetX());
			textElement.SetDoubleAttribute(SHAPE_XML_Y_ATTRIBUTE, line->GetY());
			TiXmlText text((const char *) line->GetText().mb_str());
			textElement.InsertEndChild(text);
			regionElement.InsertEndChild(textElement);
			textNode = textNode->GetNext();
		}

		regionsElement.InsertEndChild(regionElement);

		node = node->GetNext();
	}
	if (regionsElement.FirstChild())
		parent.InsertEndChild(regionsElement);

}

/******************************************************************************/
/****************************** AddShapeInfo **********************************/
/******************************************************************************/

void
SDIShape::AddShapeInfo(TiXmlNode &node)
{
	TiXmlElement shapeElement(SHAPE_XML_SHAPE_ELEMENT);
	shapeElement.SetAttribute(DSAM_XML_TYPE_ATTRIBUTE, (const char *) wxString(
	  GetClassInfo()->GetClassName()).mb_str());
	shapeElement.SetAttribute(DSAM_XML_ID_ATTRIBUTE, GetId());
	shapeElement.SetDoubleAttribute(SHAPE_XML_X_ATTRIBUTE, GetX());
	shapeElement.SetDoubleAttribute(SHAPE_XML_Y_ATTRIBUTE, GetY());
	AddPenInfo(shapeElement);
	AddBrushInfo(shapeElement);
	AddLineInfo(shapeElement);

	// Misc. attributes
	if (m_attachmentMode != 0)
		shapeElement.SetAttribute(SHAPE_XML_USE_ATTACHMENTS_ATTRIBUTE, 
		  m_attachmentMode);
	if (m_sensitivity != OP_ALL)
		shapeElement.SetAttribute(SHAPE_XML_SENSITIVITY_ATTRIBUTE, 
		  m_sensitivity);
	if (!m_spaceAttachments)
		shapeElement.SetAttribute(SHAPE_XML_SPACE_ATTACHMENTS_ATTRIBUTE,
		  m_spaceAttachments);
	if (m_fixedWidth)
		shapeElement.SetAttribute(SHAPE_XML_FIXED_WIDTH_ATTRIBUTE, 
		  m_fixedWidth);
	if (m_fixedHeight)
		shapeElement.SetAttribute(SHAPE_XML_FIXED_HEIGHT_ATTRIBUTE, 
		  m_fixedHeight);
	if (m_shadowMode != SHADOW_NONE)
		shapeElement.SetAttribute(SHAPE_XML_SHADOW_MODE_ATTRIBUTE, 
		  m_shadowMode);
	if (!m_centreResize)
		shapeElement.SetAttribute(SHAPE_XML_CENTRE_RESIZE_ATTRIBUTE, 0);
	shapeElement.SetAttribute(SHAPE_XML_MAINTAIN_ASPECT_RATIO_ATTRIBUTE,
	  m_maintainAspectRatio);
	if (m_highlighted)
		shapeElement.SetAttribute(SHAPE_XML_HILITE_ATTRIBUTE, m_highlighted);

	if (m_parent) // For composite objects
		shapeElement.SetAttribute(SHAPE_XML_PARENT_ATTRIBUTE, m_parent->GetId(
		  ));
	if (m_rotation != 0.0)
		shapeElement.SetAttribute(SHAPE_XML_ROTATION_ATTRIBUTE, (int)
		  m_rotation);

	if (!IsKindOf(CLASSINFO(wxLineShape))) {
		shapeElement.SetAttribute(SHAPE_XML_NECK_LENGTH_ATTRIBUTE, 
		  GetBranchNeckLength());
		shapeElement.SetAttribute(SHAPE_XML_STEM_LENGTH_ATTRIBUTE, 
		  GetBranchStemLength());
		shapeElement.SetAttribute(SHAPE_XML_BRANCH_SPACING_ATTRIBUTE, 
		  GetBranchSpacing());
		shapeElement.SetAttribute(SHAPE_XML_BRANCH_STYLE_ATTRIBUTE, 
		  GetBranchStyle());
	}

	AddAttachmentsInfo(shapeElement);
	AddRegions(shapeElement);
	if (IsKindOf(CLASSINFO(SDIAnalysisShape)))
		((SDIAnalysisShape *) this)->AddXMLInfo(shapeElement);
	else if (IsKindOf(CLASSINFO(SDIControlShape)))
		((SDIControlShape *) this)->AddXMLInfo(shapeElement);
	else if (IsKindOf(CLASSINFO(SDIDisplayShape)))
		((SDIDisplayShape *) this)->AddXMLInfo(shapeElement);
	else if (IsKindOf(CLASSINFO(SDIFilterShape)))
		((SDIFilterShape *) this)->AddXMLInfo(shapeElement);
	else if (IsKindOf(CLASSINFO(SDIIOShape)))
		((SDIIOShape *) this)->AddXMLInfo(shapeElement);
	else if (IsKindOf(CLASSINFO(SDILineShape)))
		((SDILineShape *) this)->AddXMLInfo(shapeElement);
	else if (IsKindOf(CLASSINFO(SDIModelShape)))
		((SDIModelShape *) this)->AddXMLInfo(shapeElement);
	else if (IsKindOf(CLASSINFO(SDIUserShape)))
		((SDIUserShape *) this)->AddXMLInfo(shapeElement);
	else if (IsKindOf(CLASSINFO(SDITransformShape)))
		((SDITransformShape *) this)->AddXMLInfo(shapeElement);
	else if (IsKindOf(CLASSINFO(SDIUtilityShape)))
		((SDIUtilityShape *) this)->AddXMLInfo(shapeElement);

	node.InsertEndChild(shapeElement);
}

/****************************** GetPenInfo ************************************/

bool
SDIShape::GetPenInfo(TiXmlNode *parent)
{
	static const wxChar *funcName = wxT("SDIShape::GetPenInfo");
	bool	ok = true;
	int		penWidth = 1, penStyle = wxSOLID;
	wxString	penColour = wxEmptyString;
	TiXmlElement	*myElement;

	if ((myElement = parent->FirstChildElement(SHAPE_XML_PEN_ELEMENT)) == NULL)
		return(true);
	ATTRIBUTE_VAL(myElement, SHAPE_XML_WIDTH_ATTRIBUTE, penWidth, false);
	ATTRIBUTE_VAL(myElement, SHAPE_XML_STYLE_ATTRIBUTE, penStyle, false);
	STR_ATTRIBUTE_VAL(myElement, SHAPE_XML_COLOUR_ATTRIBUTE, penColour, false);
	if (penColour == wxEmptyString)
		penColour = wxT("BLACK");
	if (penColour.GetChar(0) == '#') {
		wxColour col(oglHexToColour(penColour.After('#')));
		m_pen = wxThePenList->FindOrCreatePen(col, penWidth, penStyle);
	} else
		m_pen = wxThePenList->FindOrCreatePen(penColour, penWidth, penStyle);

	if (!m_pen)
		m_pen = wxBLACK_PEN;

	return(ok);

}

/****************************** GetBrushInfo **********************************/

bool
SDIShape::GetBrushInfo(TiXmlNode *parent)
{
	static const wxChar *funcName = wxT("SDIShape::GetBrushInfo");
	bool	ok = true;
	int		brushStyle = wxSOLID;
	wxString	brushColour = wxEmptyString;
	TiXmlElement	*myElement;

	if ((myElement = parent->FirstChildElement(SHAPE_XML_BRUSH_ELEMENT)) ==
	  NULL)
		return(true);
	ATTRIBUTE_VAL(myElement, SHAPE_XML_STYLE_ATTRIBUTE, brushStyle, false);
	STR_ATTRIBUTE_VAL(myElement, SHAPE_XML_COLOUR_ATTRIBUTE, brushColour,
	  false);

	if (brushColour == wxEmptyString)
		brushColour = wxT("WHITE");

	if (brushColour.GetChar(0) == '#') {
		wxColour col(oglHexToColour(brushColour.After('#')));
		m_brush = wxTheBrushList->FindOrCreateBrush(col, brushStyle);
	} else
		m_brush = wxTheBrushList->FindOrCreateBrush(brushColour, brushStyle);

	if (!m_brush)
		m_brush = wxWHITE_BRUSH;
	return(true);

}

/****************************** GetAttachmentsInfo ****************************/

bool
SDIShape::GetAttachmentsInfo(TiXmlNode *parent)
{
	static const wxChar *funcName = wxT("SDIShape::GetAttachmentsInfo");
	bool	ok = true;
	TiXmlNode	*node;
	TiXmlElement	*myElement, *pointElement;

	if ((myElement = parent->FirstChildElement(
	  SHAPE_XML_ATTACHMENTS_ELEMENT)) == NULL)
		return(true);
	for (node = myElement->IterateChildren(SHAPE_XML_POINT_ELEMENT, NULL);
	  node; node = myElement->IterateChildren(SHAPE_XML_POINT_ELEMENT, node)) {
		pointElement = node->ToElement();
		wxAttachmentPoint *point = new wxAttachmentPoint;
		ATTRIBUTE_VAL(pointElement, DSAM_XML_ID_ATTRIBUTE, point->m_id, false);
		ATTRIBUTE_VAL(pointElement, SHAPE_XML_X_ATTRIBUTE, point->m_x, false);
		ATTRIBUTE_VAL(pointElement, SHAPE_XML_Y_ATTRIBUTE, point->m_y, false);
		 m_attachmentPoints.Append((wxObject *)point);
	}
	return(ok);

}

/****************************** GetFontInfo ***********************************/

wxFont *
SDIShape::GetFontInfo(TiXmlNode *parent)
{
	static const wxChar *funcName = wxT("SDIShape::GetFontInfo");
	bool	ok = true;
	int		fontSize = SHAPE_DEFAULT_FONT_POINTSIZE, fontFamily = wxSWISS;
	int		fontStyle = wxNORMAL, fontWeight = wxNORMAL;
	TiXmlElement	*myElement;

	if ((myElement = parent->FirstChildElement(SHAPE_XML_FONT_ELEMENT)) == NULL)
		return(NULL);

	ATTRIBUTE_VAL(myElement, SHAPE_XML_POINTSIZE_ATTRIBUTE, fontSize, false);
	ATTRIBUTE_VAL(myElement, SHAPE_XML_FAMILY_ATTRIBUTE, fontFamily, false);
	ATTRIBUTE_VAL(myElement, SHAPE_XML_STYLE_ATTRIBUTE, fontStyle, false);
	ATTRIBUTE_VAL(myElement, SHAPE_XML_WEIGHT_ATTRIBUTE, fontWeight, false);

	wxFont *font = wxTheFontList->FindOrCreateFont(fontSize, fontFamily,
	  fontStyle, fontWeight);
	return(font);

}

/****************************** GetRegionsInfo ********************************/

bool
SDIShape::GetRegionsInfo(TiXmlNode *parent)
{
	static const wxChar *funcName = wxT("SDIShape::GetRegionsInfo");
	bool	ok = true;
	int		formatMode = FORMAT_NONE, penStyle = wxSOLID;
	double	x = 0.0, y = 0.0, width = 0.0, height = 0.0, minWidth = 5.0;
	double	minHeight = 5.0, regionProportionX = -1.0, regionProportionY = -1.0;
	TiXmlNode	*node, *textNode;
	TiXmlElement	*myElement, *regionElement;

	if ((myElement = parent->FirstChildElement(SHAPE_XML_REGIONS_ELEMENT)) ==
	  NULL)
		return(true);
	ClearRegions();
	for (node = myElement->IterateChildren(SHAPE_XML_REGION_ELEMENT, NULL);
	  node; node = myElement->IterateChildren(SHAPE_XML_REGION_ELEMENT, node)) {
		regionElement = node->ToElement();
		wxShapeRegion *region = new wxShapeRegion;
		ATTRIBUTE_VAL(regionElement, SHAPE_XML_X_ATTRIBUTE, x, false);
		ATTRIBUTE_VAL(regionElement, SHAPE_XML_Y_ATTRIBUTE, y, false);
		region->SetPosition(x, y);

		ATTRIBUTE_VAL(regionElement, SHAPE_XML_WIDTH_ATTRIBUTE, width, false);
		ATTRIBUTE_VAL(regionElement, SHAPE_XML_HEIGHT_ATTRIBUTE, height, false);
		region->SetSize(width, height);

		ATTRIBUTE_VAL(regionElement, SHAPE_XML_MIN_WIDTH_ATTRIBUTE, minWidth,
		  false);
		ATTRIBUTE_VAL(regionElement, SHAPE_XML_MIN_HEIGHT_ATTRIBUTE, minHeight,
		  false);
		region->SetMinSize(minWidth, minHeight);

		ATTRIBUTE_VAL(regionElement, SHAPE_XML_PROPORTION_X_ATTRIBUTE,
		  regionProportionX, false);
		ATTRIBUTE_VAL(regionElement, SHAPE_XML_PROPORTION_Y_ATTRIBUTE,
		  regionProportionY, false);
		region->SetProportions(regionProportionX, regionProportionY);

		ATTRIBUTE_VAL(regionElement, SHAPE_XML_FORMAT_MODE_ATTRIBUTE,
		  formatMode, false);
		region->SetFormatMode(formatMode);

		region->SetFont(GetFontInfo(regionElement));

		wxString regionTextColour = wxEmptyString;
		STR_ATTRIBUTE_VAL(regionElement, SHAPE_XML_TEXT_COLOUR_ATTRIBUTE,
		  regionTextColour, false);
		if (regionTextColour.IsEmpty())
			regionTextColour = wxT("BLACK");
		region->m_textColour = regionTextColour;

    	wxString penColour = wxEmptyString;
		STR_ATTRIBUTE_VAL(regionElement, SHAPE_XML_TEXT_COLOUR_ATTRIBUTE,
		  penColour, false);
		if (penColour)
			region->SetPenColour(penColour);

		ATTRIBUTE_VAL(regionElement, SHAPE_XML_PEN_STYLE_ATTRIBUTE, penStyle,
		  false);
		region->SetPenStyle(penStyle);
		for (textNode = regionElement->IterateChildren(SHAPE_XML_TEXT_ELEMENT,
		  NULL); textNode; textNode = regionElement->IterateChildren(
		  SHAPE_XML_TEXT_ELEMENT, textNode)) {
			TiXmlElement *textElement = textNode->ToElement();
			ATTRIBUTE_VAL(textElement, SHAPE_XML_X_ATTRIBUTE, x, false);
			ATTRIBUTE_VAL(textElement, SHAPE_XML_Y_ATTRIBUTE, y, false);
			TiXmlText* text = textElement->FirstChild()->ToText();
			if (text) {
				wxShapeTextLine *line = new wxShapeTextLine(x, y, (wxChar *)
				  text->Value());
				region->m_formattedText.Append(line);
			}
		}
		m_regions.Append(region);
	}
	return(ok);

}

/****************************** GetShapeInfo **********************************/

bool
SDIShape::GetShapeInfo(TiXmlElement *myElement)
{
	static const wxChar *funcName = wxT("SDIShape::GetShapeInfo");
	bool	ok = true;
	int		iVal = 0;
	double	x = 0.0, y = 0.0;
	wxString	strValue;

	ATTRIBUTE_VAL(myElement, DSAM_XML_ID_ATTRIBUTE, iVal, true);
	m_id = iVal;
	wxRegisterId(m_id);
	ATTRIBUTE_VAL(myElement, SHAPE_XML_X_ATTRIBUTE, x, true);
	ATTRIBUTE_VAL(myElement, SHAPE_XML_Y_ATTRIBUTE, y, true);
	if (ok) {
		SetX(x);
		SetY(y);
	}
	ClearText();

	wxString brushString = wxEmptyString;
	m_attachmentMode = ATTACHMENT_MODE_NONE;

	STR_ATTRIBUTE_VAL(myElement, SHAPE_XML_TEXT_COLOUR_ATTRIBUTE, strValue,
	  false);
	SetTextColour(strValue);

	GetPenInfo(myElement);
	GetBrushInfo(myElement);
	GetAttachmentsInfo(myElement);
	ATTRIBUTE_VAL(myElement, SHAPE_XML_USE_ATTACHMENTS_ATTRIBUTE,
	  m_attachmentMode, false);
	ATTRIBUTE_VAL(myElement, SHAPE_XML_SENSITIVITY_ATTRIBUTE, m_sensitivity,
	  false);

	ATTRIBUTE_VAL(myElement, SHAPE_XML_SPACE_ATTACHMENTS_ATTRIBUTE, iVal,
	  false);
	m_spaceAttachments = (iVal != 0);

	ATTRIBUTE_VAL(myElement, SHAPE_XML_FIXED_WIDTH_ATTRIBUTE, iVal, false);
	m_fixedWidth = (iVal != 0);

	ATTRIBUTE_VAL(myElement, SHAPE_XML_FIXED_HEIGHT_ATTRIBUTE, iVal, false);
	m_fixedHeight = (iVal != 0);

	ATTRIBUTE_VAL(myElement, SHAPE_XML_FORMAT_MODE_ATTRIBUTE, m_formatMode,
	  false);
	ATTRIBUTE_VAL(myElement, SHAPE_XML_SHADOW_MODE_ATTRIBUTE, m_shadowMode,
	  false);

	ATTRIBUTE_VAL(myElement, SHAPE_XML_NECK_LENGTH_ATTRIBUTE,
	  m_branchNeckLength, false);

	ATTRIBUTE_VAL(myElement, SHAPE_XML_STEM_LENGTH_ATTRIBUTE,
	  m_branchStemLength, false);

	ATTRIBUTE_VAL(myElement, SHAPE_XML_BRANCH_SPACING_ATTRIBUTE,
	  m_branchSpacing, false);

	ATTRIBUTE_VAL(myElement, SHAPE_XML_BRANCH_STYLE_ATTRIBUTE, iVal, false);
	m_branchStyle = iVal;

	ATTRIBUTE_VAL(myElement, SHAPE_XML_CENTRE_RESIZE_ATTRIBUTE, iVal, false);
	m_centreResize = (iVal != 0);

	ATTRIBUTE_VAL(myElement, SHAPE_XML_MAINTAIN_ASPECT_RATIO_ATTRIBUTE, iVal,
	  false);
	m_maintainAspectRatio = (iVal != 0);

	ATTRIBUTE_VAL(myElement, SHAPE_XML_HILITE_ATTRIBUTE, iVal, false);
	m_highlighted = (iVal != 0);

	ATTRIBUTE_VAL(myElement, SHAPE_XML_ROTATION_ATTRIBUTE, m_rotation, false);

	ATTRIBUTE_VAL(myElement, SHAPE_XML_ROTATION_ATTRIBUTE, iVal, false);

	SetFont(oglMatchFont((iVal)? iVal: 10));

	// Read text regions
	GetRegionsInfo(myElement);
	if (IsKindOf(CLASSINFO(SDIAnalysisShape)))
		((SDIAnalysisShape *) this)->GetXMLInfo(myElement);
	else if (IsKindOf(CLASSINFO(SDIControlShape)))
		((SDIControlShape *) this)->GetXMLInfo(myElement);
	else if (IsKindOf(CLASSINFO(SDIDisplayShape)))
		((SDIDisplayShape *) this)->GetXMLInfo(myElement);
	else if (IsKindOf(CLASSINFO(SDIFilterShape)))
		((SDIFilterShape *) this)->GetXMLInfo(myElement);
	else if (IsKindOf(CLASSINFO(SDIIOShape)))
		((SDIIOShape *) this)->GetXMLInfo(myElement);
	else if (IsKindOf(CLASSINFO(SDILineShape)))
		((SDILineShape *) this)->GetXMLInfo(myElement);
	else if (IsKindOf(CLASSINFO(SDIModelShape)))
		((SDIModelShape *) this)->GetXMLInfo(myElement);
	else if (IsKindOf(CLASSINFO(SDIUserShape)))
		((SDIUserShape *) this)->GetXMLInfo(myElement);
	else if (IsKindOf(CLASSINFO(SDITransformShape)))
		((SDITransformShape *) this)->GetXMLInfo(myElement);
	else if (IsKindOf(CLASSINFO(SDIUtilityShape)))
		((SDIUtilityShape *) this)->GetXMLInfo(myElement);
	return(ok);

}

/******************************************************************************/
/****************************** SDIPolygonShape Methods ***********************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDIPolygonShape, wxPolygonShape)

/****************************** Constructor ***********************************/

SDIPolygonShape::SDIPolygonShape(double width, double height): wxPolygonShape()
{
	// wxPolygonShape::SetSize relies on the shape having non-zero
	// size initially.
	w = (width == 0.0)? 60.0: width;
	h = (height == 0.0)? 60.0: height;

}

/****************************** AddXMLInfo ************************************/

void
SDIPolygonShape::AddXMLInfo(TiXmlNode &node)
{
	wxObjectList::compatibility_iterator objectNode = GetPoints()->GetFirst();
	TiXmlElement pointListElement(SHAPE_XML_POINT_LIST_ELEMENT);
	pointListElement.SetAttribute(DSAM_XML_ID_ATTRIBUTE,
	  DSAM_XML_CURRENT_ATTRIBUTE_VALUE);
	while (objectNode) {
		wxRealPoint *point = (wxRealPoint *)objectNode->GetData();
		AddPointInfo(pointListElement, point);
		objectNode = objectNode->GetNext();
	}
	node.InsertEndChild(pointListElement);
	TiXmlElement originalPointListElement(SHAPE_XML_POINT_LIST_ELEMENT);
	originalPointListElement.SetAttribute(DSAM_XML_ID_ATTRIBUTE,
	  DSAM_XML_ORIGINAL_ATTRIBUTE_VALUE);
	// Save the original (unscaled) points
	objectNode = GetOriginalPoints()->GetFirst();
	while (objectNode) {
		wxRealPoint *point = (wxRealPoint *)objectNode->GetData();
		AddPointInfo(originalPointListElement, point);
		objectNode = objectNode->GetNext();
	}
	node.InsertEndChild(originalPointListElement);

}

/****************************** GetXMLInfo ************************************/

bool
SDIPolygonShape::GetXMLInfo(TiXmlNode *elementNode)
{
	static const wxChar *funcName = wxT("SDIPolygonShape::GetXMLInfo");
	bool	ok = true;
	double	height = 100.0, width = 100.0;
	wxRealPoint	*point;
	TiXmlNode	*node, *pointNode;
	TiXmlElement *pointsListElement;

	TiXmlElement *myElement = elementNode->ToElement();
	Create(NULL);
	wxList	*points = GetPoints();
	wxList	*originalPoints = GetOriginalPoints();
	for (node = myElement->IterateChildren(SHAPE_XML_POINT_LIST_ELEMENT, NULL);
	  node; node = myElement->IterateChildren(SHAPE_XML_POINT_LIST_ELEMENT,
	  node)) {
		pointsListElement = node->ToElement();
		wxString id = wxEmptyString;
		STR_ATTRIBUTE_VAL(pointsListElement, DSAM_XML_ID_ATTRIBUTE, id, true);
		if (id.IsSameAs((wxChar *) DSAM_XML_CURRENT_ATTRIBUTE_VALUE)) {
			for (pointNode = pointsListElement->IterateChildren(
			  SHAPE_XML_POINT_ELEMENT, NULL); pointNode; pointNode =
			  pointsListElement->IterateChildren(SHAPE_XML_POINT_ELEMENT,
			  pointNode)) {
				if ((point = GetPointInfo(pointNode->ToElement())) == NULL) {
					NotifyError(wxT("%s: Could not set point."), funcName);
					ok = false;
					break;
				}
				points->Append((wxObject*) point);
			}
		}
		if (id.IsSameAs((wxChar *) DSAM_XML_ORIGINAL_ATTRIBUTE_VALUE)) {
			double minX = 1000;
			double minY = 1000;
			double maxX = -1000;
			double maxY = -1000;
			for (pointNode = pointsListElement->IterateChildren(
			  SHAPE_XML_POINT_ELEMENT, NULL); pointNode; pointNode =
			  pointsListElement->IterateChildren(SHAPE_XML_POINT_ELEMENT,
			  pointNode)) {
				if ((point = GetPointInfo(pointNode->ToElement())) == NULL) {
					NotifyError(wxT("%s: Could not set point."), funcName);
					ok = false;
					break;
				}
				originalPoints->Append((wxObject*) point);
				if (point->x < minX)
					minX = point->x;
				if (point->y < minY)
					minY = point->y;
				if (point->x > maxX)
					maxX = point->x;
				if (point->y > maxY)
					maxY = point->y;
			}
			SetOriginalWidth(maxX - minX);
			SetOriginalHeight(maxY - minY);
		}
	}
	if (points->IsEmpty()) { // If no points_list assign a diamond instead.

		point = new wxRealPoint(0.0, (-height/2));
		points->Append((wxObject*) point);

		point = new wxRealPoint((width/2), 0.0);
		points->Append((wxObject*) point);

		point = new wxRealPoint(0.0, (height/2));
		points->Append((wxObject*) point);

		point = new wxRealPoint((-width/2), 0.0);
		points->Append((wxObject*) point);

		point = new wxRealPoint(0.0, (-height/2));
		points->Append((wxObject*) point);
	}
	if (originalPoints->IsEmpty()) {
		point = new wxRealPoint(0.0, (-height/2));
		originalPoints->Append((wxObject*) point);

		point = new wxRealPoint((width/2), 0.0);
		originalPoints->Append((wxObject*) point);

		point = new wxRealPoint(0.0, (height/2));
		originalPoints->Append((wxObject*) point);

		point = new wxRealPoint((-width/2), 0.0);
		originalPoints->Append((wxObject*) point);

		point = new wxRealPoint(0.0, (-height/2));
		originalPoints->Append((wxObject*) point);

		SetOriginalWidth(width);
		SetOriginalHeight(height);
	}
	CalculateBoundingBox();
	return(ok);

}

/******************************************************************************/
/****************************** SDIEllipseShape Methods ***********************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDIEllipseShape, wxEllipseShape)

/****************************** Constructor ***********************************/

SDIEllipseShape::SDIEllipseShape(double width, double height): wxEllipseShape(
  width, height)
{
}

/****************************** AddXMLInfo ************************************/

void
SDIEllipseShape::AddXMLInfo(TiXmlNode &node)
{
	TiXmlElement *myElement = node.ToElement();

	myElement->SetDoubleAttribute(SHAPE_XML_WIDTH_ATTRIBUTE, GetWidth());
	myElement->SetDoubleAttribute(SHAPE_XML_HEIGHT_ATTRIBUTE, GetHeight());

}

/****************************** GetXMLInfo ************************************/

bool
SDIEllipseShape::GetXMLInfo(TiXmlNode *node)
{
	static const wxChar *funcName = wxT("SDIEllipseShape::GetXMLInfo");
	bool	ok = true;

	TiXmlElement *myElement = node->ToElement();
	ATTRIBUTE_VAL(myElement, SHAPE_XML_WIDTH_ATTRIBUTE, m_width, true)
	ATTRIBUTE_VAL(myElement, SHAPE_XML_HEIGHT_ATTRIBUTE, m_height, true)
	return(ok);

}

/******************************************************************************/
/****************************** SDIRectangleShape Methods *********************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDIRectangleShape, wxRectangleShape)

/****************************** Constructor ***********************************/

SDIRectangleShape::SDIRectangleShape(double width, double height):
  wxRectangleShape(width, height)
{

}

/****************************** AddXMLInfo ************************************/

void
SDIRectangleShape::AddXMLInfo(TiXmlNode &node)
{
	TiXmlElement *myElement = node.ToElement();

	myElement->SetDoubleAttribute(SHAPE_XML_WIDTH_ATTRIBUTE, GetWidth());
	myElement->SetDoubleAttribute(SHAPE_XML_HEIGHT_ATTRIBUTE, GetHeight());
	if (m_cornerRadius != 0.0)
		myElement->SetDoubleAttribute(SHAPE_XML_CORNER_ATTRIBUTE,
		  m_cornerRadius);

}

/****************************** GetXMLInfo ************************************/

bool
SDIRectangleShape::GetXMLInfo(TiXmlNode *node)
{
	static const wxChar *funcName = wxT("SDIRectangleShape::GetXMLInfo");
	bool	ok = true;

	TiXmlElement *myElement = node->ToElement();
	ATTRIBUTE_VAL(myElement, SHAPE_XML_WIDTH_ATTRIBUTE, m_width, true)
	ATTRIBUTE_VAL(myElement, SHAPE_XML_HEIGHT_ATTRIBUTE, m_height, true)
	return(ok);

}

/******************************************************************************/
/****************************** Friend Methods ********************************/
/******************************************************************************/

/****************************** AddPointInfo **********************************/

void
AddPointInfo(TiXmlNode &parent, wxRealPoint *point)
{
	TiXmlElement pointElement(SHAPE_XML_POINT_ELEMENT);
	pointElement.SetDoubleAttribute(SHAPE_XML_X_ATTRIBUTE, point->x);
	pointElement.SetDoubleAttribute(SHAPE_XML_Y_ATTRIBUTE, point->y);
	parent.InsertEndChild(pointElement);

}

/****************************** GetPointInfo **********************************/

wxRealPoint *
GetPointInfo(TiXmlElement *myElement)
{
	static const wxChar *funcName = wxT("SDIPolygonShape::GetPointInfo");
	bool	ok = true;

	wxRealPoint *point = new wxRealPoint;
	ATTRIBUTE_VAL(myElement, SHAPE_XML_X_ATTRIBUTE, point->x, true);
	ATTRIBUTE_VAL(myElement, SHAPE_XML_Y_ATTRIBUTE, point->y, true);
	if (!ok) {
		delete point;
		return(NULL);
	}
	return(point);

}

#endif /* USE_WX_OGL */
