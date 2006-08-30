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

#include "GrSDIEvtHandler.h"
#include "GrSDIBaseShapes.h"
#include "GrSDIShapes.h"
#include "GrSDIDiagram.h"
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

SDIXMLDocument::SDIXMLDocument(SDIDiagram *theDiagram)
{
	diagramLoadInitiated = false;
	diagram = theDiagram;
	diagram->SetLoadIDsFromFile(true);
	if (diagram->GetSimProcess())
		SetSimScriptPtr((SimScriptPtr) diagram->GetSimProcess()->module->
		  parsPtr);

}

/******************************************************************************/
/****************************** Destructor ************************************/
/******************************************************************************/

SDIXMLDocument::~SDIXMLDocument(void)
{

}

/******************************************************************************/
/****************************** AddShapeInfo **********************************/
/******************************************************************************/

void
SDIXMLDocument::AddShapeInfo(TiXmlNode &node, void *shapePtr)
{
	SDIShape *shape = (SDIShape *) shapePtr;
	shape->AddShapeInfo(node);

}

/******************************************************************************/
/****************************** AddLineShapes *********************************/
/******************************************************************************/

void
SDIXMLDocument::AddLineShapes(TiXmlNode &parent)
{
	wxShape *shape;

	TiXmlElement connectionsElement(DSAM_XML_CONNECTIONS_ELEMENT);

	wxNode *node = diagram->GetShapeList()->GetFirst();
	while (node) {
		shape = (wxShape *) node->GetData();
		if (shape->IsKindOf(CLASSINFO(wxLineShape)))
			AddShapeInfo(connectionsElement, shape);
		node = node->GetNext();
	}
	parent.InsertEndChild(connectionsElement);

}

/******************************************************************************/
/****************************** Create ****************************************/
/******************************************************************************/

void
SDIXMLDocument::Create(void)
{
	DSAMXMLDocument::Create(diagram->GetSimProcess());

}

/******************************************************************************/
/****************************** GetClassInfo **********************************/
/******************************************************************************/

wxClassInfo *
SDIXMLDocument::GetClassInfo(TiXmlElement *shapeElement)
{
	static const char *funcName = "SDIXMLDocument::GetClassInfo";
	wxString	type;

	type = wxConvUTF8.cMB2WX(shapeElement->Attribute(DSAM_XML_TYPE_ATTRIBUTE));
	if (type.empty()) {
		XMLNotifyWarning(shapeElement, wxT("%s: Could not find shape type"),
		  funcName);
		diagram->SetOk(false);
		return(NULL);
	}
	wxClassInfo *classInfo = wxClassInfo::FindClass(type);
	if (!classInfo) {
		XMLNotifyWarning(shapeElement, wxT("%s: Could not identify class for ")
		  wxT("'%s' shape type"), funcName, type.c_str());
		diagram->SetOk(false);
		return(NULL);
	}
	return(classInfo);

}

/******************************************************************************/
/****************************** CreateLoadShape *******************************/
/******************************************************************************/

SDIShape *
SDIXMLDocument::CreateLoadShape(TiXmlElement *myElement, DatumPtr pc)
{
	static const char *funcName = "SDIXMLDocument::CreateLoadShape";

	wxClassInfo *classInfo = GetClassInfo(myElement);
	if (!classInfo) {
		XMLNotifyWarning(myElement, wxT("%s: Could not identify class for ")
		  wxT("shape type"), funcName);
		diagram->SetOk(false);
		return(NULL);
	}
	SDIShape *shape = (SDIShape *) diagram->CreateLoadShape(pc, classInfo,
	  NULL);
	if (!shape) {
		XMLNotifyWarning(myElement, wxT("%s: Could not create shape."),
		  funcName);
		delete classInfo;
		return(NULL);
	}
	if (!((SDIShape *)shape)->GetShapeInfo(myElement))
		diagram->SetOk(false);
	return(shape);

}

/******************************************************************************/
/****************************** GetShapeInfo **********************************/
/******************************************************************************/

void
SDIXMLDocument::GetShapeInfo(TiXmlNode *parent, DatumPtr pc)
{
	TiXmlElement	*shapeElement;

	if (!diagramLoadInitiated) {
		diagramLoadInitiated = true;
		diagram->SetOk(true);
	}
	if ((shapeElement = parent->FirstChildElement(SHAPE_XML_SHAPE_ELEMENT)) ==
	  NULL)
		return;
	SDIShape *shape = CreateLoadShape(shapeElement, pc);
	if (!shape->GetShapeInfo(shapeElement)) {
		diagram->SetOk(false);
		return;
	}

}

/******************************************************************************/
/****************************** FindShape *************************************/
/******************************************************************************/

wxShape *
SDIXMLDocument::FindShape(long id)
{
	wxShape	*shape;

	wxNode *node = diagram->GetShapeList()->GetFirst();
	while (node) {
		shape = (wxShape *) node->GetData();
		if (shape->GetId() == id)
			return(shape);
		node = node->GetNext();
	}
	return(NULL);

}

/******************************************************************************/
/****************************** GetLineShapeInfo ******************************/
/******************************************************************************/

void
SDIXMLDocument::GetLineShapeInfo(TiXmlNode *parent)
{
	static const char *funcName = "SDIXMLDocument::GetLineShapeInfo";
	bool	ok = true;
	int		fromId = -1, toId = -1;
	TiXmlElement	*myElement, *shapeElement;
	TiXmlNode		*node;

	if ((myElement = parent->FirstChildElement(DSAM_XML_CONNECTIONS_ELEMENT)) ==
	  NULL)
		return;
	for (node = myElement->IterateChildren(SHAPE_XML_SHAPE_ELEMENT, NULL);
	  node; node = myElement->IterateChildren(SHAPE_XML_SHAPE_ELEMENT, node)) {
		shapeElement = node->ToElement();
		wxClassInfo *classInfo = GetClassInfo(shapeElement);
		if (!classInfo) {
			XMLNotifyWarning(shapeElement, wxT("%s: Could not identify class ")
			  wxT("for '%s' shape type"), funcName, type);
			diagram->SetOk(false);
			return;
		}
		SDIShape *shape = CreateLoadShape(shapeElement, NULL);
		TiXmlElement *lineShapeElement = shapeElement->FirstChildElement(
		  SHAPE_XML_LINE_SHAPE_ELEMENT);
		ATTRIBUTE_VAL(lineShapeElement, SHAPE_XML_FROM_ATTRIBUTE, fromId, true);
		ATTRIBUTE_VAL(lineShapeElement, SHAPE_XML_TO_ATTRIBUTE, toId, true);

		wxLineShape *lineShape = (wxLineShape *) shape;
		if ((fromId >= 0) && (toId >= 0)) {
			wxShape *from = FindShape(fromId);
			wxShape *to = FindShape(toId);
			from->AddLine((wxLineShape *) shape, to, lineShape->
			  GetAttachmentFrom(), lineShape->GetAttachmentTo());
		}
	}

}
