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
/****************************** SDIAnalysisShape Methods **********************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDIAnalysisShape, SDIEllipseShape)

/****************************** Constructor ***********************************/

SDIAnalysisShape::SDIAnalysisShape(double width, double height):
  SDIEllipseShape(width, height)
{
}

/****************************** AddXMLInfo ************************************/

void
SDIAnalysisShape::AddXMLInfo(TiXmlNode &node)
{
	TiXmlElement myElement(SHAPE_XML_ANALYSIS_SHAPE_ELEMENT);
	SDIEllipseShape::AddXMLInfo(myElement);
	node.InsertEndChild(myElement);

}

/****************************** GetXMLInfo ************************************/

bool
SDIAnalysisShape::GetXMLInfo(TiXmlNode *node)
{
	static const char *funcName = "SDIAnalysisShape::GetXMLInfo";
	TiXmlElement *myElement;

	if ((myElement = node->FirstChildElement(
	  SHAPE_XML_ANALYSIS_SHAPE_ELEMENT)) == NULL) {
		NotifyError("%s: Could not find analysis element.", funcName);
		return(false);
	}
	SDIEllipseShape::GetXMLInfo(myElement);
	return(true);

}

/******************************************************************************/
/****************************** SDIDisplayShape Methods ***********************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDIDisplayShape, SDIRectangleShape)

/****************************** Constructor ***********************************/

SDIDisplayShape::SDIDisplayShape(double width, double height):
  SDIRectangleShape(width, height)
{
	// 0.3 of the smaller rectangle dimension
	SetCornerRadius((double) -0.3);
}

/****************************** AddXMLInfo ************************************/

void
SDIDisplayShape::AddXMLInfo(TiXmlNode &node)
{
	TiXmlElement myElement(SHAPE_XML_DISPLAY_SHAPE_ELEMENT);
	SDIRectangleShape::AddXMLInfo(myElement);
	node.InsertEndChild(myElement);

}

/****************************** GetXMLInfo ************************************/

bool
SDIDisplayShape::GetXMLInfo(TiXmlNode *node)
{
	static const char *funcName = "SDIDisplayShape::GetXMLInfo";
	TiXmlElement *myElement;

	if ((myElement = node->FirstChildElement(
	  SHAPE_XML_DISPLAY_SHAPE_ELEMENT)) == NULL) {
		NotifyError("%s: Could not find display element.", funcName);
		return(false);
	}
	SDIRectangleShape::GetXMLInfo(myElement);
	return(true);

}
/******************************************************************************/
/****************************** SDIControlShape Methods ***********************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDIControlShape, SDIPolygonShape)

/****************************** Constructor ***********************************/

SDIControlShape::SDIControlShape(double width, double height): SDIPolygonShape(
  width, height)
{
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

/****************************** AddXMLInfo ************************************/

void
SDIControlShape::AddXMLInfo(TiXmlNode &node)
{
	TiXmlElement myElement(SHAPE_XML_CONTROL_SHAPE_ELEMENT);
	SDIPolygonShape::AddXMLInfo(myElement);
	node.InsertEndChild(myElement);

}

/****************************** AddXMLInfo ************************************/

bool
SDIControlShape::GetXMLInfo(TiXmlNode *node)
{
	TiXmlElement *myElement = node->FirstChildElement(
	  SHAPE_XML_CONTROL_SHAPE_ELEMENT);
	SDIPolygonShape::GetXMLInfo(myElement);
	return(true);

}

/******************************************************************************/
/****************************** SDIFilterShape Methods ************************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDIFilterShape, SDIPolygonShape)

/****************************** Constructor ***********************************/

SDIFilterShape::SDIFilterShape(double width, double height): SDIPolygonShape(
  width, height)
{

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

/****************************** AddXMLInfo ************************************/

void
SDIFilterShape::AddXMLInfo(TiXmlNode &node)
{
	TiXmlElement myElement(SHAPE_XML_FILTER_SHAPE_ELEMENT);
	SDIPolygonShape::AddXMLInfo(myElement);
	node.InsertEndChild(myElement);

}

/****************************** GetXMLInfo ************************************/

bool
SDIFilterShape::GetXMLInfo(TiXmlNode *node)
{
	static const char *funcName = "SDIFilterShape::GetXMLInfo";
	TiXmlElement *myElement;

	if ((myElement = node->FirstChildElement(SHAPE_XML_FILTER_SHAPE_ELEMENT)) ==
	  NULL) {
		NotifyError("%s: Could not find filter element.", funcName);
		return(false);
	}
	SDIPolygonShape::GetXMLInfo(myElement);
	return(true);

}

/******************************************************************************/
/****************************** SDIIOShape Methods ****************************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDIIOShape, SDIPolygonShape)

/****************************** Constructor ***********************************/

SDIIOShape::SDIIOShape(double width, double height): SDIPolygonShape(width,
  height)
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

/****************************** AddXMLInfo ************************************/

void
SDIIOShape::AddXMLInfo(TiXmlNode &node)
{
	TiXmlElement myElement(SHAPE_XML_IO_SHAPE_ELEMENT);
	SDIPolygonShape::AddXMLInfo(myElement);
	node.InsertEndChild(myElement);

}

/****************************** GetXMLInfo ************************************/

bool
SDIIOShape::GetXMLInfo(TiXmlNode *node)
{
	static const char *funcName = "SDIIOShape::GetXMLInfo";
	TiXmlElement *myElement;

	if ((myElement = node->FirstChildElement(SHAPE_XML_IO_SHAPE_ELEMENT)) ==
	  NULL) {
		NotifyError("%s: Could not find IO element.", funcName);
		return(false);
	}
	SDIPolygonShape::GetXMLInfo(myElement);
	return(true);

}

/******************************************************************************/
/****************************** SDILineShape Methods **************************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDILineShape, wxLineShape)

/****************************** AddXMLControlPointsInfo ***********************/

void
SDILineShape::AddXMLControlPointsInfo(TiXmlNode &node, wxNode *cPNode)
{
	if (!cPNode)
		return;
	TiXmlElement myElement(SHAPE_XML_CONTROL_POINTS_ELEMENT);
	while (cPNode) {
		wxRealPoint *point = (wxRealPoint *) cPNode->GetData();
		AddPointInfo(myElement, point);
		cPNode = cPNode->GetNext();
	}
	node.InsertEndChild(myElement);

}

/****************************** AddXMLArrowListInfo ***************************/

void
SDILineShape::AddXMLArrowListInfo(TiXmlNode &node, wxNode *aNode)
{
	if (!aNode)
		return;
	TiXmlElement myElement(SHAPE_XML_ARROW_LIST_ELEMENT);
	while (aNode) {
		wxArrowHead *head = (wxArrowHead *) aNode->GetData();
		TiXmlElement arrowElement(SHAPE_XML_ARROW_ELEMENT);
		arrowElement.SetAttribute(DSAM_XML_ID_ATTRIBUTE, head->GetId());
		arrowElement.SetAttribute(SHAPE_XML_NAME_ATTRIBUTE, head->GetName());
		arrowElement.SetAttribute(SHAPE_XML_TYPE_ATTRIBUTE, head->_GetType());
		arrowElement.SetAttribute(SHAPE_XML_END_ATTRIBUTE, head->GetArrowEnd());
		arrowElement.SetDoubleAttribute(SHAPE_XML_X_OFFSET_ATTRIBUTE, head->
		  GetXOffset());
		arrowElement.SetDoubleAttribute(SHAPE_XML_Y_OFFSET_ATTRIBUTE, head->
		  GetYOffset());
		arrowElement.SetDoubleAttribute(SHAPE_XML_SIZE_ATTRIBUTE, head->
		  GetArrowSize());
		arrowElement.SetDoubleAttribute(SHAPE_XML_SPACING_ATTRIBUTE, head->
		  GetSpacing());
		myElement.InsertEndChild(arrowElement);
		aNode = aNode->GetNext();
	}
	node.InsertEndChild(myElement);

}

/****************************** AddXMLInfo ************************************/

void
SDILineShape::AddXMLInfo(TiXmlNode &node)
{
	TiXmlElement myElement(SHAPE_XML_LINE_SHAPE_ELEMENT);
	if (m_from)
		myElement.SetAttribute(SHAPE_XML_FROM_ATTRIBUTE, m_from->GetId());
	if (m_to)
		myElement.SetAttribute(SHAPE_XML_TO_ATTRIBUTE, m_to->GetId());
	if (m_attachmentFrom)
		myElement.SetAttribute(SHAPE_XML_ATTACHMENT_FROM_ELEMENT,
		  m_attachmentFrom);
	if (m_attachmentTo)
		myElement.SetAttribute(SHAPE_XML_ATTACHMENT_TO_ELEMENT, m_attachmentTo);
	if (m_alignmentStart)
		myElement.SetAttribute(SHAPE_XML_ALIGNMENT_START_ELEMENT,
		  m_alignmentStart);
	if (m_alignmentEnd)
		myElement.SetAttribute(SHAPE_XML_ALIGNMENT_END_ELEMENT, m_alignmentEnd);
	myElement.SetAttribute(SHAPE_XML_ISSPLINE_ATTRIBUTE, IsSpline());
	if (GetMaintainStraightLines())
		myElement.SetAttribute(SHAPE_XML_KEEP_LINES_STRAIGHT_ELEMENT,
		  GetMaintainStraightLines());
	AddXMLControlPointsInfo(myElement, GetLineControlPoints()->GetFirst());
	AddXMLArrowListInfo(myElement, GetArrows().GetFirst());
	node.InsertEndChild(myElement);

}

/****************************** GetXMLControlPointsInfo ***********************/

bool
SDILineShape::GetXMLControlPointsInfo(TiXmlNode *parent)
{
	static const char *funcName = "SDILineShape::GetXMLControlPointsInfo";
	bool	ok = true;
	wxRealPoint	*point;
	TiXmlNode	*pointNode;
	TiXmlElement	*myElement;

	if ((myElement = parent->FirstChildElement(
	  SHAPE_XML_CONTROL_POINTS_ELEMENT)) == NULL)
		return(true);
	if (m_lineControlPoints)
		ClearPointList(*m_lineControlPoints);
	else
		m_lineControlPoints = new wxList;
	for (pointNode = myElement->IterateChildren(SHAPE_XML_POINT_ELEMENT, NULL);
	  pointNode; pointNode = myElement->IterateChildren(SHAPE_XML_POINT_ELEMENT,
	  pointNode)) {
		if ((point = GetPointInfo(pointNode->ToElement())) == NULL) {
			NotifyError("%s: Could not set point.", funcName);
			ok = false;
			break;
		}
		m_lineControlPoints->Append((wxObject*) point);
	}
		
	return(ok);
	
}

/****************************** GetXMLArrowListInfo ***************************/

bool
SDILineShape::GetXMLArrowListInfo(TiXmlNode *parent)
{
	static const char *funcName = "SDILineShape::GetXMLArrowListInfo";
	bool	ok = true;
	int		arrowEnd = 0, id = -1, type = ARROW_ARROW;
	double	xOffset = 0.0, yOffset = 0.0, size = 0.0, spacing = 5.0;
    wxString arrowName;

	TiXmlNode	*node;
	TiXmlElement	*myElement;

	if ((myElement = parent->FirstChildElement(SHAPE_XML_ARROW_LIST_ELEMENT)) ==
	  NULL)
		return(true);
	for (node = myElement->IterateChildren(SHAPE_XML_ARROW_ELEMENT, NULL);
	  node; node = myElement->IterateChildren(SHAPE_XML_ARROW_ELEMENT, node)) {
		TiXmlElement *arrowElement = node->ToElement();
		ATTRIBUTE_VAL(arrowElement, DSAM_XML_ID_ATTRIBUTE, id, true);
		STR_ATTRIBUTE_VAL(arrowElement, SHAPE_XML_NAME_ATTRIBUTE, arrowName,
		  true);
		ATTRIBUTE_VAL(arrowElement, SHAPE_XML_TYPE_ATTRIBUTE, type, true);
		ATTRIBUTE_VAL(arrowElement, SHAPE_XML_END_ATTRIBUTE, arrowEnd, true);
		ATTRIBUTE_VAL(arrowElement, SHAPE_XML_X_OFFSET_ATTRIBUTE, xOffset,
		  true);
		ATTRIBUTE_VAL(arrowElement, SHAPE_XML_Y_OFFSET_ATTRIBUTE, yOffset,
		  true);
		ATTRIBUTE_VAL(arrowElement, SHAPE_XML_SIZE_ATTRIBUTE, size, true);
		ATTRIBUTE_VAL(arrowElement, SHAPE_XML_SPACING_ATTRIBUTE, spacing,
		  false);
		if (!ok)
			break;
		wxRegisterId(id);
		wxArrowHead *arrowHead = AddArrow(type, arrowEnd, size, xOffset,
		  arrowName, NULL, id);
		arrowHead->SetYOffset(yOffset);
		arrowHead->SetSpacing(spacing);
	}

	return(ok);

}

/****************************** GetXMLInfo ************************************/

bool
SDILineShape::GetXMLInfo(TiXmlNode *node)
{
	static const char *funcName = "SDILineShape::GetXMLInfo";
	bool	ok = true;
	int		iVal;
	TiXmlElement *myElement;

	if ((myElement = node->FirstChildElement(SHAPE_XML_LINE_SHAPE_ELEMENT)) ==
	  NULL) {
		NotifyError("%s: Could not find line shape element.", funcName);
		return(false);
	}
	ATTRIBUTE_VAL(myElement, SHAPE_XML_ISSPLINE_ATTRIBUTE, iVal, true);
	SetSpline(iVal != 0);

	// Compatibility: check for no regions.
	if (m_regions.GetCount() == 0) {
		wxShapeRegion *newRegion = new wxShapeRegion;
		newRegion->SetName(_T("Middle"));
		newRegion->SetSize(150, 50);
		m_regions.Append((wxObject *)newRegion);
		if (m_text.GetCount() > 0) {
			newRegion->ClearText();
			wxNode *node = m_text.GetFirst();
			while (node) {
				wxShapeTextLine *textLine = (wxShapeTextLine *)node->GetData();
				wxNode *next = node->GetNext();
				newRegion->GetFormattedText().Append((wxObject *)textLine);
				delete node;
				node = next;
			}
		}

		newRegion = new wxShapeRegion;
		newRegion->SetName(wxT("Start"));
		newRegion->SetSize(150, 50);
		m_regions.Append((wxObject *)newRegion);

		newRegion = new wxShapeRegion;
		newRegion->SetName(wxT("End"));
		newRegion->SetSize(150, 50);
		m_regions.Append((wxObject *)newRegion);
	}

	m_attachmentTo = 0;
	m_attachmentFrom = 0;
	ATTRIBUTE_VAL(myElement, SHAPE_XML_ATTACHMENT_FROM_ELEMENT,
	  m_attachmentFrom, false);
	ATTRIBUTE_VAL(myElement, SHAPE_XML_ATTACHMENT_TO_ELEMENT,
	  m_attachmentTo, false);
	ATTRIBUTE_VAL(myElement, SHAPE_XML_ISSPLINE_ATTRIBUTE, iVal, true);
	ATTRIBUTE_VAL(myElement, SHAPE_XML_ALIGNMENT_START_ELEMENT,
	  m_alignmentStart, false);
	ATTRIBUTE_VAL(myElement, SHAPE_XML_ALIGNMENT_END_ELEMENT,
	  m_alignmentEnd, false);
	SetMaintainStraightLines(iVal != 0);
	if (ok && !GetXMLControlPointsInfo(myElement))
		ok = false;
	if (ok && !GetXMLArrowListInfo(myElement))
		ok = false;
	return(ok);

}

/******************************************************************************/
/****************************** SDIModelShape Methods *************************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDIModelShape, SDIPolygonShape)

/****************************** Constructor ***********************************/

SDIModelShape::SDIModelShape(double width, double height): SDIPolygonShape(
  width, height)
{
	double	edge = h / 4.0;
	double	hW = w / 16.0;
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

/****************************** AddXMLInfo ************************************/

void
SDIModelShape::AddXMLInfo(TiXmlNode &node)
{
	TiXmlElement myElement(SHAPE_XML_MODEL_SHAPE_ELEMENT);
	SDIPolygonShape::AddXMLInfo(myElement);
	node.InsertEndChild(myElement);

}

/****************************** GetXMLInfo ************************************/

bool
SDIModelShape::GetXMLInfo(TiXmlNode *node)
{
	static const char *funcName = "SDIModelShape::GetXMLInfo";
	TiXmlElement *myElement;

	if ((myElement = node->FirstChildElement(SHAPE_XML_MODEL_SHAPE_ELEMENT)) ==
	  NULL) {
		NotifyError("%s: Could not find model element.", funcName);
		return(false);
	}
	SDIPolygonShape::GetXMLInfo(myElement);
	return(true);

}

/******************************************************************************/
/****************************** SDIUserShape Methods **************************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDIUserShape, SDIPolygonShape)

/****************************** Constructor ***********************************/

SDIUserShape::SDIUserShape(double width, double height): SDIPolygonShape(width,
  height)
{

	double	flange = h / 8.0;
	double	base = w / 12.0;
	double	slope = w / 14.0;
	double	hd = w / 4.0;

	wxList *thePoints = new wxList;
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0), (h / 2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0 + base), (h /
	  2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0 + base + slope),
	  (h / 2.0 - flange)));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0 - hd),  (h / 2.0 -
	  flange)));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0 - hd),  (h / 2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0), 0.0));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0 - hd), (-h / 2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0 - hd), (-h / 2.0 +
	  flange)));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0 + base + slope),
	  (-h / 2.0 + flange)));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0 + base), (-h /
	  2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0), (-h / 2.0)));
	Create(thePoints);

}

/****************************** AddXMLInfo ************************************/

void
SDIUserShape::AddXMLInfo(TiXmlNode &node)
{
	TiXmlElement myElement(SHAPE_XML_USER_SHAPE_ELEMENT);
	SDIPolygonShape::AddXMLInfo(myElement);
	node.InsertEndChild(myElement);

}

/****************************** GetXMLInfo ************************************/

bool
SDIUserShape::GetXMLInfo(TiXmlNode *node)
{
	static const char *funcName = "SDIUserShape::GetXMLInfo";
	TiXmlElement *myElement;

	if ((myElement = node->FirstChildElement(SHAPE_XML_USER_SHAPE_ELEMENT)) ==
	  NULL) {
		NotifyError("%s: Could not find user element.", funcName);
		return(false);
	}
	SDIPolygonShape::GetXMLInfo(myElement);
	return(true);

}

/******************************************************************************/
/****************************** SDITransformShape Methods *********************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDITransformShape, SDIPolygonShape)

/****************************** Constructor ***********************************/

SDITransformShape::SDITransformShape(double width, double height):
  SDIPolygonShape(width, height)
{
	double	inset = h / 7.0;
	wxList *thePoints = new wxList;
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0), (h / 2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0), (h / 2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((w / 2.0 - inset), (-h /
	  2.0)));
	thePoints->Append((wxObject*) new wxRealPoint((-w / 2.0 + inset), (-h /
	  2.0)));
	Create(thePoints);

}

/****************************** AddXMLInfo ************************************/

void
SDITransformShape::AddXMLInfo(TiXmlNode &node)
{
	TiXmlElement myElement(SHAPE_XML_TRANSFORM_SHAPE_ELEMENT);
	SDIPolygonShape::AddXMLInfo(myElement);
	node.InsertEndChild(myElement);

}

/****************************** GetXMLInfo ************************************/

bool
SDITransformShape::GetXMLInfo(TiXmlNode *node)
{
	static const char *funcName = "SDITransformShape::GetXMLInfo";
	TiXmlElement *myElement;

	if ((myElement = node->FirstChildElement(
	  SHAPE_XML_TRANSFORM_SHAPE_ELEMENT)) == NULL) {
		NotifyError("%s: Could not find transform element.", funcName);
		return(false);
	}
	SDIPolygonShape::GetXMLInfo(myElement);
	return(true);

}

/******************************************************************************/
/****************************** SDIUtilityShape Methods ***********************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDIUtilityShape, SDIRectangleShape)

/****************************** Constructor ***********************************/

SDIUtilityShape::SDIUtilityShape(double width, double height):
  SDIRectangleShape(width, height)
{
}

/****************************** AddXMLInfo ************************************/

void
SDIUtilityShape::AddXMLInfo(TiXmlNode &node)
{
	TiXmlElement myElement(SHAPE_XML_UTILITY_SHAPE_ELEMENT);
	SDIRectangleShape::AddXMLInfo(myElement);
	node.InsertEndChild(myElement);

}

/****************************** GetXMLInfo ************************************/

bool
SDIUtilityShape::GetXMLInfo(TiXmlNode *node)
{
	static const char *funcName = "SDIUtilityShape::GetXMLInfo";
	TiXmlElement *myElement;

	if ((myElement = node->FirstChildElement(
	  SHAPE_XML_UTILITY_SHAPE_ELEMENT)) == NULL) {
		NotifyError("%s: Could not find utility element.", funcName);
		return(false);
	}
	SDIRectangleShape::GetXMLInfo(myElement);
	return(true);

}

#endif /* USE_WX_OGL */
