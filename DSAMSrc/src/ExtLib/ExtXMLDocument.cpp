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
/****************************** Constructor ***********************************/
/******************************************************************************/

DSAMXMLDocument::DSAMXMLDocument(void)
{
	printf("DSAMXMLDocument::DSAMXMLDocument: Entered.\n");
}

/******************************************************************************/
/****************************** Destructor ************************************/
/******************************************************************************/

DSAMXMLDocument::~DSAMXMLDocument(void)
{
	SetTabSize(4);

}

/******************************************************************************/
/****************************** Create ****************************************/
/******************************************************************************/

void
DSAMXMLDocument::Create(EarObjectPtr simProcess)
{
	const char* rootDecleration = "<?xml version=\"1.0\" standalone=\"no\" ?>";

	Parse(rootDecleration);
	TiXmlElement dSAMElement(DSAM_XML_DSAM_ELEMENT);
	dSAMElement.SetAttribute("version", GetDSAMPtr_Common()->version);
	AddAppInfo(dSAMElement);
	AddSimulation(dSAMElement, simProcess);
	InsertEndChild(dSAMElement);

}

/******************************************************************************/
/****************************** AddSimConnections **************************/
/******************************************************************************/

void
DSAMXMLDocument::AddSimConnections(TiXmlNode &node, DynaListPtr list,
  char * type)
{
	DynaListPtr	dNode;

	for (dNode = list; dNode != NULL; dNode = dNode->next) {
		TiXmlElement	connectionElement(type);
		connectionElement.SetAttribute(DSAM_XML_LABEL_ATTRIBUTE, (char *) dNode->
		  data);
		node.InsertEndChild(connectionElement);
	}
	
}

/******************************************************************************/
/****************************** AddSimObjects ******************************/
/******************************************************************************/

#define CREATE_OBJECT_ELEMENT(PC, TYPE)	\
			TiXmlElement objectElement(DSAM_XML_OBJECT_ELEMENT); \
			objectElement.SetAttribute(DSAM_XML_TYPE_ATTRIBUTE, (TYPE)); \
			if ((PC)->label && *(PC)->label) \
				objectElement.SetAttribute("label", (PC)->label); \

DatumPtr
DSAMXMLDocument::AddSimObjects(TiXmlNode &node, DatumPtr start)
{
	static const char *funcName = "DSAMXMLDocument::AddSimObjects";
	DatumPtr	pc, lastInstruction = NULL;

	for (pc = start; pc; pc = pc->next) {
		switch (pc->type) {
		case PROCESS: {
			if (pc->data->module->specifier == SIMSCRIPT_MODULE) {
				AddSimulation(node, pc->data);
				break;
			}
			CREATE_OBJECT_ELEMENT(pc, DSAM_XML_PROCESS_ATTRIBUTE_VALUE);
			objectElement.SetAttribute(DSAM_XML_NAME_ATTRIBUTE, pc->data->
			  module->name);
			AddSimConnections(objectElement, pc->u.proc.inputList,
			  DSAM_XML_INPUT_ELEMENT);
			AddSimConnections(objectElement, pc->u.proc.outputList,
			  DSAM_XML_OUTPUT_ELEMENT);
			AddParList(objectElement, GetUniParListPtr_ModuleMgr(pc->data));
			AddShapeInfo(objectElement, pc->data->clientData);
			node.InsertEndChild(objectElement);
			break; }
		case REPEAT: {
			CREATE_OBJECT_ELEMENT(pc, GetProcessName_Utility_Datum(pc));
			objectElement.SetAttribute(DSAM_XML_COUNT_ATTRIBUTE, pc->u.loop.
			  count);
			pc = lastInstruction = AddSimObjects(objectElement, pc->next);
			AddShapeInfo(objectElement, pc->clientData);
			node.InsertEndChild(objectElement);
			break; }
		case RESET: {
			CREATE_OBJECT_ELEMENT(pc, GetProcessName_Utility_Datum(pc));
			objectElement.SetAttribute(DSAM_XML_OBJLABEL_ATTRIBUTE, pc->u.
			  string);
			AddShapeInfo(objectElement, pc->clientData);
			node.InsertEndChild(objectElement);
			break; }
		case STOP:
			return(pc);
			break;
		default:
			NotifyError("%s: Not yet implemented (%d)", funcName, pc->type);
		}
		if ((lastInstruction = pc) == NULL)
			break;
	}
	return (lastInstruction);

}

#undef CREATE_OBJECT_ELEMENT

/******************************************************************************/
/****************************** AddSimulation ******************************/
/******************************************************************************/

void
DSAMXMLDocument::AddSimulation(TiXmlNode &node, EarObjectPtr simProcess)
{
	TiXmlElement	simElement(DSAM_XML_SIMULAION_ELEMENT);
	AddParList(simElement, GetUniParListPtr_ModuleMgr(simProcess));
	AddSimObjects(simElement, GetSimulation_ModuleMgr(simProcess));
	node.InsertEndChild(simElement);

}

/******************************************************************************/
/****************************** AddParGeneral ******************************/
/******************************************************************************/

void
DSAMXMLDocument::AddParGeneral(TiXmlNode &node, UniParPtr p)
{
	int		i, oldIndex;
	wxString	str;

	switch (p->type) {
	case UNIPAR_PARLIST:
		if (p->valuePtr.parList.process)
			SET_PARS_POINTER(*p->valuePtr.parList.process);
		AddParListStandard(node, *p->valuePtr.parList.list);
		break;
	case UNIPAR_INT_ARRAY:
	case UNIPAR_REAL_ARRAY:
	case UNIPAR_STRING_ARRAY:
	case UNIPAR_NAME_SPEC_ARRAY:
		for (i = 0; i < *p->valuePtr.array.numElements; i++) {
			oldIndex = p->valuePtr.array.index;
			p->valuePtr.array.index = i;
			TiXmlElement parElement(DSAM_XML_PAR_ELEMENT);
			parElement.SetAttribute("name", p->abbr);
			str.Printf("%d:%s", i, GetParString_UniParMgr(p));
			parElement.SetAttribute("value", (char *) str.c_str());
			node.InsertEndChild(parElement);
			p->valuePtr.array.index = oldIndex;
		}
		break;
	case UNIPAR_SIMSCRIPT:
		break;
	default:
		TiXmlElement parElement(DSAM_XML_PAR_ELEMENT);
		parElement.SetAttribute("name", p->abbr);
		parElement.SetAttribute("value", GetParString_UniParMgr(p));
		node.InsertEndChild(parElement);
	}
	
}

/******************************************************************************/
/****************************** AddParListStandard *************************/
/******************************************************************************/

void
DSAMXMLDocument::AddParListStandard(TiXmlNode &node, UniParListPtr parList)
{
	int		i;

	for (i = 0; i < parList->numPars; i++)
		AddParGeneral(node, &parList->pars[i]);
}

/******************************************************************************/
/****************************** AddParList *********************************/
/******************************************************************************/

void
DSAMXMLDocument::AddParList(TiXmlNode &node, UniParListPtr parList)
{
	if (!parList)
		return;
	switch (parList->mode) {
	case UNIPAR_SET_IC:
		printf("DSAMXMLDocument::AddParList:Debug not yet implemented.\n"); 
		//break;
	default:
		AddParListStandard(node, parList);
	}

}

/******************************************************************************/
/****************************** AddAppInfo ******************************/
/******************************************************************************/

void
DSAMXMLDocument::AddAppInfo(TiXmlNode &node)
{
	TiXmlElement appElement(DSAM_XML_APPLICATION_ELEMENT);
	appElement.SetAttribute("name", GetPtr_AppInterface()->appName);
	appElement.SetAttribute("version", GetPtr_AppInterface()->appVersion);
	AddParList(appElement, GetPtr_AppInterface()->appParList);
	node.InsertEndChild(appElement);

}
