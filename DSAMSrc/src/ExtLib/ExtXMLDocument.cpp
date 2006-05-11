/**********************
 *
 * File:		ExtXMLDocument.cpp
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
#include "UtString.h"
#include "UtAppInterface.h"
#include "tinyxml.h"
#include "ExtMainApp.h"

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
	mySimProcess = NULL;
	simScriptPtr = NULL;
	SetTabSize(4);

}

/******************************************************************************/
/****************************** Destructor ************************************/
/******************************************************************************/

DSAMXMLDocument::~DSAMXMLDocument(void)
{

}

/******************************************************************************/
/****************************** Create ****************************************/
/******************************************************************************/

void
DSAMXMLDocument::Create(EarObjectPtr simProcess)
{
	const wxChar* rootDecleration = wxT("<?xml version=\"1.0\" standalone=\"no"
	  "\" ?>");

	Parse((const char *) wxConvUTF8.cWX2MB(rootDecleration));
	TiXmlElement dSAMElement(DSAM_XML_DSAM_ELEMENT);
	dSAMElement.SetAttribute(DSAM_XML_VERSION_ATTRIBUTE, (const char *) 
	  wxConvUTF8.cWX2MB(GetDSAMPtr_Common()->version));
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
		connectionElement.SetAttribute(DSAM_XML_LABEL_ATTRIBUTE, (char *)
		  dNode->data);
		node.InsertEndChild(connectionElement);
	}
	
}

/******************************************************************************/
/****************************** AddSimObjects *********************************/
/******************************************************************************/

#define CREATE_OBJECT_ELEMENT(PC, TYPE)	\
			TiXmlElement objectElement(DSAM_XML_OBJECT_ELEMENT); \
			objectElement.SetAttribute(DSAM_XML_TYPE_ATTRIBUTE, (TYPE)); \
			if ((PC)->label && *(PC)->label) \
				objectElement.SetAttribute(DSAM_XML_LABEL_ATTRIBUTE, \
				  (const char *) wxConvUTF8.cWX2MB((PC)->label))

DatumPtr
DSAMXMLDocument::AddSimObjects(TiXmlNode &node, DatumPtr start)
{
	static const wxChar *funcName = wxT("DSAMXMLDocument::AddSimObjects");
	DatumPtr	pc, lastInstruction = NULL;

	for (pc = start; pc; pc = pc->next) {
		switch (pc->type) {
		case PROCESS: {
			if (pc->data->module->specifier == SIMSCRIPT_MODULE) {
				AddSimulation(node, pc->data);
				break;
			}
			CREATE_OBJECT_ELEMENT(pc, DSAM_XML_PROCESS_ATTRIBUTE_VALUE);
			objectElement.SetAttribute(DSAM_XML_NAME_ATTRIBUTE, (const char *)
			   wxConvUTF8.cWX2MB(pc->data->module->name));
			if (!pc->data->module->onFlag)
				objectElement.SetAttribute(DSAM_XML_ENABLED_ATTRIBUTE, 0);
			AddSimConnections(objectElement, pc->u.proc.inputList,
			  DSAM_XML_INPUT_ELEMENT);
			AddSimConnections(objectElement, pc->u.proc.outputList,
			  DSAM_XML_OUTPUT_ELEMENT);
			AddParList(objectElement, GetUniParListPtr_ModuleMgr(pc->data));
			AddShapeInfo(objectElement, pc->clientData);
			node.InsertEndChild(objectElement);
			break; }
		case REPEAT: {
			CREATE_OBJECT_ELEMENT(pc, (const char *) wxConvUTF8.cWX2MB(
			  GetProcessName_Utility_Datum(pc)));
			objectElement.SetAttribute(DSAM_XML_COUNT_ATTRIBUTE, pc->u.loop.
			  count);
			AddShapeInfo(objectElement, pc->clientData);
			pc = lastInstruction = AddSimObjects(objectElement, pc->next);
			node.InsertEndChild(objectElement);
			break; }
		case RESET: {
			CREATE_OBJECT_ELEMENT(pc, (const char *) wxConvUTF8.cWX2MB(
			  GetProcessName_Utility_Datum(pc)));
			objectElement.SetAttribute(DSAM_XML_OBJLABEL_ATTRIBUTE,
			  (const char *) wxConvUTF8.cWX2MB(pc->u.ref.string));
			AddShapeInfo(objectElement, pc->clientData);
			node.InsertEndChild(objectElement);
			break; }
		case STOP:
			return(pc);
			break;
		default:
			NotifyError(wxT("%s: Not yet implemented (%d)"), funcName, pc->
			  type);
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
	AddLineShapes(simElement);
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
	case UNIPAR_CFLIST: {
		TiXmlElement cFListElement(DSAM_XML_CFLIST_ELEMENT);
		AddParList(cFListElement, (*p->valuePtr.cFPtr)->cFParList);
		AddParList(cFListElement, (*p->valuePtr.cFPtr)->bParList);
		node.InsertEndChild(cFListElement);
		break; }
	case UNIPAR_PARARRAY: {
		TiXmlElement parArrayElement(DSAM_XML_PARARRAY_ELEMENT);
		parArrayElement.SetAttribute(DSAM_XML_NAME_ATTRIBUTE, (const char *)
		  wxConvUTF8.cWX2MB((*p->valuePtr.pAPtr)->name));
		AddParList(parArrayElement, (*p->valuePtr.pAPtr)->parList);
		node.InsertEndChild(parArrayElement);
		break; }
	case UNIPAR_PARLIST:
		if (p->valuePtr.parList.process)
			SET_PARS_POINTER(*p->valuePtr.parList.process);
		AddParList(node, *p->valuePtr.parList.list, p->abbr);
		break;
	case UNIPAR_INT_ARRAY:
	case UNIPAR_REAL_ARRAY:
	case UNIPAR_REAL_DYN_ARRAY:
	case UNIPAR_STRING_ARRAY:
	case UNIPAR_NAME_SPEC_ARRAY:
		for (i = 0; i < *p->valuePtr.array.numElements; i++) {
			oldIndex = p->valuePtr.array.index;
			p->valuePtr.array.index = i;
			TiXmlElement parElement(DSAM_XML_PAR_ELEMENT);
			parElement.SetAttribute(DSAM_XML_NAME_ATTRIBUTE, (const char *)
			  wxConvUTF8.cWX2MB(p->abbr));
			str.Printf(wxT("%d:%s"), i, GetParString_UniParMgr(p));
			parElement.SetAttribute(DSAM_XML_VALUE_ATTRIBUTE, (const char *)
			  str.mb_str());
			node.InsertEndChild(parElement);
			p->valuePtr.array.index = oldIndex;
		}
		break;
	case UNIPAR_SIMSCRIPT:
		break;
	default:
		TiXmlElement parElement(DSAM_XML_PAR_ELEMENT);
		parElement.SetAttribute(DSAM_XML_NAME_ATTRIBUTE, (const char *)
		  wxConvUTF8.cWX2MB(p->abbr));
		parElement.SetAttribute(DSAM_XML_VALUE_ATTRIBUTE, (const char *)
		  wxConvUTF8.cWX2MB(GetParString_UniParMgr(p)));
		node.InsertEndChild(parElement);
	}
	
}

/******************************************************************************/
/****************************** AddParListStandard ****************************/
/******************************************************************************/

void
DSAMXMLDocument::AddParListStandard(TiXmlNode &node, UniParListPtr parList)
{
	int		i;

	for (i = 0; i < parList->numPars; i++)
		if (parList->pars[i].enabled)
			AddParGeneral(node, &parList->pars[i]);
}

/******************************************************************************/
/****************************** AddParList ************************************/
/******************************************************************************/

void
DSAMXMLDocument::AddParList(TiXmlNode &node, UniParListPtr parList, const wxChar
  *name)
{
	if (!parList)
		return;
	TiXmlElement parListElement(DSAM_XML_PAR_LIST_ELEMENT);
	if (name)
		parListElement.SetAttribute(DSAM_XML_NAME_ATTRIBUTE, (const char *)
		  wxConvUTF8.cWX2MB(name));
	switch (parList->mode) {
	case UNIPAR_SET_IC:
		printf("DSAMXMLDocument::AddParList:Debug not yet implemented.\n"); 
		//break; - go to default.
	default:
		AddParListStandard(parListElement, parList);
	}
	node.InsertEndChild(parListElement);

}

/******************************************************************************/
/****************************** AddAppInfo ************************************/
/******************************************************************************/

void
DSAMXMLDocument::AddAppInfo(TiXmlNode &node)
{
	TiXmlElement appElement(DSAM_XML_APPLICATION_ELEMENT);
	appElement.SetAttribute(DSAM_XML_NAME_ATTRIBUTE, (char *)
	  GetPtr_AppInterface()->appName);
	appElement.SetAttribute(DSAM_XML_VERSION_ATTRIBUTE, (char *)
	  GetPtr_AppInterface()->appVersion);
	AddParList(appElement, GetPtr_AppInterface()->parList);
	node.InsertEndChild(appElement);

}

/******************************************************************************/
/****************************** CreateNotification ****************************/
/******************************************************************************/

/*
 * This routine creates the notification for warning and error messages.
 */

wxString &
DSAMXMLDocument::CreateNotification(TiXmlNode *node, wxChar *format, va_list
  args)
{
	static wxString	msg;
	wxString	location;

	msg.PrintfV(format, args);
	location.Printf(wxT(": line %d, column %d"), node->Row(), node->Column());
	msg += location;
	return(msg);

}

/******************************************************************************/
/****************************** XMLNotifyError ******************************/
/******************************************************************************/

/*
 * This routine prints an error, adding the line and column of the problem
 */

void
DSAMXMLDocument::XMLNotifyError(TiXmlNode *node, wxChar *format, ...)
{
	va_list	args;

	va_start(args, format);
	NotifyError((wxChar *) CreateNotification(node, format, args).c_str());
	va_end(args);

}

/******************************************************************************/
/****************************** XMLNotifyWarning ******************************/
/******************************************************************************/

/*
 * This routine prints an error, adding the line and column of the problem
 */

void
DSAMXMLDocument::XMLNotifyWarning(TiXmlNode *node, wxChar *format, ...)
{
	va_list	args;

	va_start(args, format);
	NotifyWarning((wxChar*) CreateNotification(node, format, args).c_str());
	va_end(args);

}

/******************************************************************************/
/****************************** ValidVersion **********************************/
/******************************************************************************/

/*
 * This routine checks if the first two version numbers of two version strings
 * correspond.
 */

bool
DSAMXMLDocument::ValidVersion(const wxString &s1, const wxString &s2)
{
	static const wxChar *funcName = wxT("DSAMXMLDocument::ValidVersion");
	size_t		pos;

	if ((pos = s1.find(DSAM_VERSION_SEPARATOR, s1.find(DSAM_VERSION_SEPARATOR) +
	  1)) == s1.npos) {
		NotifyError(wxT("%s: Could not find valid version (%s)."), funcName,
		  s1.c_str());
		return(false);
	}
	if (s1.compare(0, pos, s2, 0, pos) != 0) {
		NotifyError(wxT("%s: Only versions '%s.x' are fully supported."),
		  funcName, s2.substr(0, pos).c_str());
		return(false);
	}
	return(true);

}

/******************************************************************************/
/****************************** GetParArrayInfo *******************************/
/******************************************************************************/

bool
DSAMXMLDocument::GetParArrayInfo(TiXmlElement * myElement, UniParList *parList)
{
	static const wxChar *funcName = wxT("DSAMXMLDocument::GetParArrayInfo");
	const wxChar	*parName;
	UniParPtr	par;
	UniParList	*subParList;
	TiXmlNode	*node, *node2;
	TiXmlElement	*parArrayElement;

	for (node = myElement->IterateChildren(DSAM_XML_PARARRAY_ELEMENT,
	  NULL); node; node = myElement->IterateChildren(
	  DSAM_XML_PARARRAY_ELEMENT, node)) {
		parArrayElement = node->ToElement();
		if ((parName = (const wxChar *) wxConvUTF8.cMB2WX(parArrayElement->
		  Attribute(DSAM_XML_NAME_ATTRIBUTE))) == NULL) {
			XMLNotifyError(parArrayElement, wxT("%s: '%s' element must have a "
			  "name."), funcName, wxT(DSAM_XML_PARARRAY_ELEMENT));
			return(false);
		}
		subParList = parList;
		if ((par = FindUniPar_UniParMgr(&subParList, (wxChar *) parName,
		  UNIPAR_SEARCH_ABBR)) == NULL) {
			XMLNotifyError(parArrayElement, wxT("%s: '%s' parameter '%s' not "
			  "found"), funcName, DSAM_XML_PARARRAY_ELEMENT, (wxChar *)
			  parName);
			return(false);
		}
		if ((node2 = parArrayElement->FirstChildElement(
		  DSAM_XML_PAR_LIST_ELEMENT)) == NULL) {
		  	XMLNotifyError(parArrayElement, wxT("%s: Could not find sub-"
			  "parameter list for '%s' par_array."), funcName, (wxChar *)
			  parName);
			return(false);
		}
		if (!GetParListInfo(node2, subParList)) {
			XMLNotifyError(parArrayElement, wxT("%s: Could not set sub-"
			  "parameter list"), funcName);
			return(false);
		}
	}
	return(true);

}

/******************************************************************************/
/****************************** GetCFListInfo *********************************/
/******************************************************************************/

bool
DSAMXMLDocument::GetCFListInfo(TiXmlElement *parentElement, 
  UniParList *parList)
{
	static const wxChar *funcName = wxT("DSAMXMLDocument::GetCFListInfo");
	UniParPtr	par;
	TiXmlNode	*node;
	TiXmlElement	*cFListElement;

	if ((cFListElement = parentElement->FirstChildElement(
	  DSAM_XML_CFLIST_ELEMENT)) != NULL) {
		if ((par = FindUniPar_UniParMgr(&parList, wxT("CFLIST"),
		  UNIPAR_SEARCH_ABBR)) == NULL) {
			XMLNotifyError(cFListElement, wxT("%s: CFlist list not found"),
			  funcName);
			return(false);
		}
		if ((node = cFListElement->IterateChildren(DSAM_XML_PAR_LIST_ELEMENT,
		  NULL)) == NULL) {
			XMLNotifyError(cFListElement, wxT("%s CFList parameters not found"),
			  funcName);
			return(false);
		}
		if (!GetParListInfo(node, (*par->valuePtr.cFPtr)->cFParList)) {
			XMLNotifyError(node->ToElement(), wxT("%s: Could not set CFList "
			  "parameters"), funcName);
			return(false);
		}
		if ((node = cFListElement->IterateChildren(DSAM_XML_PAR_LIST_ELEMENT,
		  node)) == NULL) {
			XMLNotifyError(cFListElement, wxT("%s CFList parameters not found"),
			  funcName);
			return(false);
		}
		if (!GetParListInfo(node, (*par->valuePtr.cFPtr)->bParList)) {
			XMLNotifyError(node->ToElement(), wxT("%s: Could not set CFList "
			  "bandwidth parameters"), funcName);
			return(false);
		}
	}
	return(true);

}

/******************************************************************************/
/****************************** GetParInfo ************************************/
/******************************************************************************/

bool
DSAMXMLDocument::GetParInfo(TiXmlNode *parentElement, UniParList *parList)
{
	static const wxChar *funcName = wxT("DSAMXMLDocument::GetParInfo");
	const wxChar	*parName, *parValue;
	UniParPtr	par;
	TiXmlNode	*node;
	TiXmlElement	*parElement;

	for (node = parentElement->IterateChildren(DSAM_XML_PAR_ELEMENT, NULL);
	  node; node = parentElement->IterateChildren(DSAM_XML_PAR_ELEMENT,
	  node)) {
		parElement = node->ToElement();
		if ((parName = (const wxChar *) wxConvUTF8.cMB2WX(parElement->Attribute(
		  DSAM_XML_NAME_ATTRIBUTE))) == NULL) {
			XMLNotifyError(parElement, wxT("%s: Missing parameter %s in '%s' "
			  "element."), funcName, DSAM_XML_NAME_ATTRIBUTE,
			  DSAM_XML_PAR_ELEMENT);
			return(false);
		}
		if ((par = FindUniPar_UniParMgr(&parList, (wxChar *) parName,
		  UNIPAR_SEARCH_ABBR)) == NULL) {
			XMLNotifyError(parElement, wxT("%s: Parameter '%s' not found."),
			  funcName, (wxChar *) parName);
			return(false);
		}
		if ((parValue = (const wxChar *) wxConvUTF8.cMB2WX(parElement->
		  Attribute(DSAM_XML_VALUE_ATTRIBUTE))) == NULL) {
			XMLNotifyError(parElement, wxT("%s: Missing parameter %s in '%s' "
			  "element."), funcName, DSAM_XML_VALUE_ATTRIBUTE,
			  DSAM_XML_PAR_ELEMENT);
			return(false);
		}
		if (!SetParValue_UniParMgr(&parList, par->index, (wxChar *) parValue)) {
			XMLNotifyError(parElement, wxT("%s: Could not set %s parameter to "
			  "%s."), funcName, parName, parValue);
			return(false);
		}
	}
	return(true);

}

/******************************************************************************/
/****************************** GetParListInfo ********************************/
/******************************************************************************/

bool
DSAMXMLDocument::GetParListInfo(TiXmlNode *parListNode, UniParList *parList)
{
	static const wxChar *funcName = wxT("DSAMXMLDocument::GetParListInfo");
	const wxChar	*parName;
	UniParPtr	par;
	TiXmlNode	*node;
	TiXmlElement	*parListElement;

	parListElement = parListNode->ToElement();
	if (!GetParInfo(parListElement, parList)) {
		XMLNotifyError(parListElement, wxT("%s: Failed reading parameters."),
		  funcName);
		return(false);
	}
	for (node = parListElement->IterateChildren(DSAM_XML_PAR_LIST_ELEMENT,
	  NULL); node; node = parListElement->IterateChildren(
	  DSAM_XML_PAR_LIST_ELEMENT, node)) {
		if ((parName = (const wxChar *) wxConvUTF8.cMB2WX(node->ToElement()->
		  Attribute(DSAM_XML_NAME_ATTRIBUTE))) == NULL) {
			XMLNotifyError(node->ToElement(), wxT("%s: '%s' Sub-parameter must "
			  "have a name."), funcName, DSAM_XML_PAR_LIST_ELEMENT);
			return(false);
		}
		if ((par = FindUniPar_UniParMgr(&parList, (wxChar *) parName,
		  UNIPAR_SEARCH_ABBR)) == NULL) {
			XMLNotifyError(node->ToElement(), wxT("%s: Sub-parameter list '%s' "
			  "parameter '%s' not found"), funcName, DSAM_XML_PAR_LIST_ELEMENT,
			  (wxChar *) parName);
			return(false);
		}
		if (!GetParListInfo(node, *par->valuePtr.parList.list)) {
			XMLNotifyError(node->ToElement(), wxT("%s: Could not set sub-"
			  "parameter list"), funcName);
			return(false);
		}
	}
	if (!GetCFListInfo(parListElement, parList)) {
		XMLNotifyError(parListElement, wxT("%s: Failed reading CFList "
		  "parameters."), funcName);
		return(false);
	}
	if (!GetParArrayInfo(parListElement, parList)) {
		XMLNotifyError(parListElement, wxT("%s: Failed reading parArray "
		  "parameters."), funcName);
		return(false);
	}
	return(true);

}

/******************************************************************************/
/****************************** GetApplicationInfo ****************************/
/******************************************************************************/

void
DSAMXMLDocument::GetApplicationInfo(void)
{
	static const wxChar *funcName = wxT("DSAMXMLDocument::GetApplicationInfo");
	const wxChar	*appVersion;
	TiXmlElement	*appElement, *parListElement;
	
	if ((appElement = RootElement()->FirstChildElement(
	  DSAM_XML_APPLICATION_ELEMENT)) == NULL)
		return;
	wxString appName = (wxChar *) appElement->Attribute(
	  DSAM_XML_NAME_ATTRIBUTE);
	if (appName.compare((wxChar *) GetPtr_AppInterface()->appName) != 0) {
		NotifyWarning(wxT("%s: Invalid application '%s', element ignored."),
		  funcName, appName.c_str());
		return;
	}
	if (!(appVersion = (const wxChar *) wxConvUTF8.cMB2WX(appElement->Attribute(
	  DSAM_XML_VERSION_ATTRIBUTE))) || !ValidVersion((wxChar *) appVersion,
	  (wxChar *) GetPtr_AppInterface()->appVersion)) {
		XMLNotifyWarning(appElement, wxT("%s: Invalid application version, "
		  "element ignored."), funcName);
		return;
	}
	if ((parListElement = appElement->FirstChildElement(
	  DSAM_XML_PAR_LIST_ELEMENT)) != NULL)
		GetParListInfo(parListElement, GetPtr_AppInterface()->parList);
}

/******************************************************************************/
/****************************** GetConnectionInfo *****************************/
/******************************************************************************/

bool
DSAMXMLDocument::GetConnectionInfo(TiXmlElement *objectElement, DynaListPtr *p,
  const char * type)
{
	static const wxChar *funcName = wxT("DSAMXMLDocument::GetConnectionInfo");
	const wxChar	*label;
	TiXmlNode		*node;
	TiXmlElement	*connectionElement;

	*p = NULL;
	for (node = objectElement->IterateChildren(type, NULL);
	  node; node = objectElement->IterateChildren(type, node)) {
		connectionElement = node->ToElement();
		if ((label = (const wxChar *) wxConvUTF8.cMB2WX(connectionElement->
		  Attribute(DSAM_XML_LABEL_ATTRIBUTE))) == NULL) {
			XMLNotifyError(objectElement, wxT("%s: Connection has no label."),
			  funcName);
			return(false);
		}
		
		Append_Utility_DynaList(p, InstallSymbol_Utility_SSSymbols(
		  &simScriptPtr->symList, (wxChar *) label, STRING)->name);
	}
	return(true);

}

/******************************************************************************/
/****************************** InstallProcess ********************************/
/******************************************************************************/

DatumPtr
DSAMXMLDocument::InstallProcess(TiXmlElement *objectElement)
{
	static const wxChar *funcName = wxT("DSAMXMLDocument::InstallProcess");
	const wxChar	*moduleName, *enabledStatus;
	DatumPtr	pc;
	TiXmlElement	*parListElement;

	if ((moduleName = (const wxChar *) wxConvUTF8.cMB2WX(objectElement->
	  Attribute(DSAM_XML_NAME_ATTRIBUTE))) == NULL) {
		XMLNotifyError(objectElement, wxT("%s: Missing process name"),
		  funcName);
		return(NULL);
	}
	if ((pc = InstallInst(objectElement, PROCESS)) == NULL) {
		XMLNotifyError(objectElement, wxT("%s: Could not initialise "
		  "instruction for '%s'"), funcName, (wxChar *) moduleName);
		return(NULL);
	}
	pc->u.proc.moduleName = InitString_Utility_String((wxChar *) moduleName);
	if (!InitProcessInst_Utility_Datum(pc)) {
		XMLNotifyError(objectElement, wxT("%s: Could not initialise process "
		  "'%s'"), funcName, (wxChar *) moduleName);
		return(NULL);
	}
	if ((enabledStatus = (const wxChar *) wxConvUTF8.cMB2WX(objectElement->
	  Attribute(DSAM_XML_ENABLED_ATTRIBUTE))) != NULL)
		EnableProcess_Utility_Datum(pc, (int) wcstol(enabledStatus, NULL, 0));
	if (((parListElement = objectElement->FirstChildElement(
	  DSAM_XML_PAR_LIST_ELEMENT)) != NULL) && !GetParListInfo(parListElement,
	  GetUniParListPtr_ModuleMgr(pc->data))) {
		XMLNotifyError(objectElement, wxT("%s: Could not initialise '%s' "
		  "module parameters"), funcName, (wxChar *) moduleName);
		return(NULL);
	}
	if (!GetConnectionInfo(objectElement, &pc->u.proc.inputList,
	  DSAM_XML_INPUT_ELEMENT)) {
		XMLNotifyError(objectElement, wxT("%s: Could not '%s' module input "
		  "connections"), funcName, (wxChar *) moduleName);
		return(NULL);
	}
	if (!GetConnectionInfo(objectElement, &pc->u.proc.outputList,
	  DSAM_XML_OUTPUT_ELEMENT)) {
		XMLNotifyError(objectElement, wxT("%s: Could not '%s' module output "
		  "connections"), funcName, (wxChar *) moduleName);
		return(NULL);
	}
	if (pc->data->module->specifier == SIMSCRIPT_MODULE) {
		XMLNotifyError(objectElement, wxT("%s: Must process sub simulation.  "
		  "Remember to check the preservation of the simProcess pars pointer."),
		  funcName);

	}
	return(pc);

}

/******************************************************************************/
/****************************** InstallInst ***********************************/
/******************************************************************************/

DatumPtr
DSAMXMLDocument::InstallInst(TiXmlElement *objectElement, int type)
{
	static const wxChar *funcName = wxT("DSAMXMLDocument::InstallInst");
	const wxChar	*label;
	DatumPtr	pc;

	if ((pc = InstallInst_Utility_Datum(simScriptPtr->simPtr, type)) == NULL) {
		XMLNotifyError(objectElement, wxT("%s: Could not install "
		  "instruction"), funcName);
		return(NULL);
	}
	if ((label = (const wxChar *) wxConvUTF8.cMB2WX(objectElement->Attribute(
	  DSAM_XML_LABEL_ATTRIBUTE))) != NULL)
		pc->label = InitString_Utility_String((wxChar *) label);
	return(pc);

}

/******************************************************************************/
/****************************** InstallSimulationNodes ************************/
/******************************************************************************/

bool
DSAMXMLDocument::InstallSimulationNodes(TiXmlElement *simElement)
{
	static const wxChar *funcName = wxT(
	  "DSAMXMLDocument::InstallSimulationNodes");
	const wxChar	*label;
	wxString	objectType;
	SymbolPtr	sp;
	DatumPtr	pc;
	TiXmlNode		*node;
	TiXmlElement	*objectElement;

	for (node = simElement->IterateChildren(DSAM_XML_OBJECT_ELEMENT, NULL);
	  node; node = simElement->IterateChildren(DSAM_XML_OBJECT_ELEMENT, node)) {
		objectElement = node->ToElement();
		objectType = wxConvUTF8.cMB2WX(objectElement->Attribute(
		  DSAM_XML_TYPE_ATTRIBUTE));
		if (objectType.empty()) {
			XMLNotifyError(objectElement, wxT("%s: Missing object 'type' in "
			  "simulation"), funcName);
			return(false);
		}
		if (objectType.compare(wxT(DSAM_XML_PROCESS_ATTRIBUTE_VALUE)) == 0) {
			if ((pc = InstallProcess(objectElement)) == NULL) {
				XMLNotifyError(objectElement, wxT("%s: Could not install "
				  "simulation object"), funcName);
				return(false);
			}
			GetShapeInfo(objectElement, pc);
		} else if ((sp = LookUpSymbol_Utility_SSSymbols(dSAMMainApp->GetSymList(
		  ), (wxChar *) objectType.c_str())) != NULL) {
			switch (sp->type) {
			case REPEAT:
				if ((pc = InstallInst(objectElement, sp->type)) == NULL)
					break;
				if (!objectElement->Attribute(DSAM_XML_COUNT_ATTRIBUTE, &pc->u.
				  loop.count)) {
					XMLNotifyError(objectElement, wxT("%s: Could not find "
					  "count."), funcName);
					return(false);
				}
				GetShapeInfo(objectElement, pc);
				if (!InstallSimulationNodes(objectElement))
					return(false);
				pc->u.loop.stopPC = InstallInst_Utility_Datum(simScriptPtr->
				  simPtr, STOP);
				break;
			case RESET:
				if ((pc = InstallInst(objectElement, sp->type)) == NULL)
					break;
				if ((label = (const wxChar *) wxConvUTF8.cMB2WX(objectElement->
				  Attribute(DSAM_XML_OBJLABEL_ATTRIBUTE))) == NULL) {
					XMLNotifyError(objectElement, wxT("%s: reset process label "
					  "missing"), funcName);
					return(false);
				}
				pc->u.ref.string = InitString_Utility_String((wxChar *) label);
				GetShapeInfo(objectElement, pc);
				break;
			default:
				;
			}
			if (!pc) {
				XMLNotifyError(objectElement, wxT("%s: Could not install "
				  "simulation '%s' object"), funcName, sp->name);
				return(false);
			}
		}
		if (pc->label && *pc->label &&  !Insert_Utility_DynaBList(
		  simScriptPtr->labelBListPtr, CmpProcessLabels_Utility_Datum, pc)) {
			XMLNotifyError(objectElement, wxT("%s: Cannot insert process "
			  "labelled '%s' into simulation."), funcName, pc->label);
			return(false);
		}
	}
	return(true);

}

/******************************************************************************/
/****************************** GetSimulationInfo *****************************/
/******************************************************************************/

bool
DSAMXMLDocument::GetSimulationInfo(TiXmlNode *simNode)
{
	static const wxChar *funcName = wxT("DSAMXMLDocument::GetSimulationInfo");
	bool	ok = true;
	DatumPtr	simulation;
	TiXmlElement	*simElement, *parListElement;

	simElement = simNode->ToElement();
	if ((mySimProcess = Init_EarObject(wxT("Util_SimScript"))) == NULL) {
		NotifyError(wxT("%s: Could not initialise process."), funcName);
		return(false);
	}
	SET_PARS_POINTER(mySimProcess);
	SetProcessSimPtr_Utility_SimScript(mySimProcess);
	simScriptPtr = (SimScriptPtr) mySimProcess->module->parsPtr;
	simScriptPtr->simFileType = UTILITY_SIMSCRIPT_XML_FILE;
	SetParsFilePath_Utility_SimScript(GetPtr_AppInterface()->workingDirectory);
	if ((parListElement = simElement->FirstChildElement(
	  DSAM_XML_PAR_LIST_ELEMENT)) != NULL)
		GetParListInfo(parListElement, GetUniParListPtr_ModuleMgr(
		  mySimProcess));
	if (!InstallSimulationNodes(simElement)) {
		XMLNotifyError(simElement, wxT("%s: Could not install simulation."),
		  funcName);
		ok = false;
	}
	simulation = (ok)? GetSimulation_ModuleMgr(mySimProcess): NULL;
	if (ok)
		ok = CXX_BOOL(ResolveInstLabels_Utility_Datum(simulation,
		  simScriptPtr->labelBList));
	if (ok && !SetDefaultConnections_Utility_Datum(simulation)) {
		XMLNotifyError(simElement, wxT("%s Could not set default forward "
		  "connections"), funcName);
		ok = false;
	}
		if (ok)
			GetLineShapeInfo(simElement);
	if (ok && !SetSimulation_Utility_SimScript(simulation)) {
		XMLNotifyError(simElement, wxT("%s: Not enough lines, or invalid "
		  "parameters, in simulation node"), funcName);
		ok = false;
	}
	//simElement->Print(stdout, 0);	// Debug
	if (!ok)
		Free_EarObject(&mySimProcess);
	return(ok);
	
}

/******************************************************************************/
/****************************** Translate *************************************/
/******************************************************************************/

/*
 * This routine translates the document into information that is used by DSAM.
 */

bool
DSAMXMLDocument::Translate(void)
{
	static const wxChar *funcName = wxT("DSAMXMLDocument::Translate");
	const wxChar	*docDSAMVersion;
	TiXmlElement	*dSAMElement, *simElement;

	if ((dSAMElement = FirstChildElement(DSAM_XML_DSAM_ELEMENT)) == NULL) {
		NotifyError(wxT("%s: This is not a DSAM document, or file is corrupt."),
		  funcName);
		return(false);
	}
	if (!(docDSAMVersion = (const wxChar *) wxConvUTF8.cMB2WX(dSAMElement->
	  Attribute(DSAM_XML_VERSION_ATTRIBUTE))) || !ValidVersion((wxChar *)
	    docDSAMVersion, (wxChar *) GetDSAMPtr_Common()->version)) {
		NotifyError(wxT("%s: Could not find valid DSAM version."), funcName);
		return(false);
	}
	GetApplicationInfo();
	if ((simElement = RootElement()->FirstChildElement(
	  DSAM_XML_SIMULAION_ELEMENT)) == NULL) {
		NotifyError(wxT("%s: Could not load simulation."), funcName);
		return(false);
	}
	if (!GetSimulationInfo(simElement))
		return(false);
	return(true);
}
