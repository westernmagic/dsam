/**********************
 *
 * File:		ExtXMLDocument.h
 * Purpose: 	DSAM simulation base XML class.
 * Comments:	Contains a revision of the wxExpr code for saving the SDI.
 * Author:		L.P.O'Mard
 * Created:		14 Feb 2005
 * Updated:		
 * Copyright:	(c) 2005, University of Essex
 *
 **********************/

#ifndef _EXTXMLDOCUMENT_H
#define _EXTXMLDOCUMENT_H 1

#ifdef __GNUG__
// #pragma interface
#endif

#include <wx/string.h>

#if wxUSE_STD_IOSTREAM
class ostream;
class istream;
#endif

#include "UtDynaList.h"
#include "UtDynaBList.h"
#include "UtSSParser.h"
#include "UtSimScript.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define DSAM_XML_APPLICATION_ELEMENT		"application"
#define DSAM_XML_CFLIST_ELEMENT				"cflist"
#define DSAM_XML_CONNECTIONS_ELEMENT		"connections"
#define DSAM_XML_DSAM_ELEMENT				"dsam"
#define DSAM_XML_ICLIST_ELEMENT				"iclist"
#define DSAM_XML_INPUT_ELEMENT				"input"
#define DSAM_XML_ION_CHANNELS_ELEMENT		"ion_channels"
#define DSAM_XML_PAR_ELEMENT				"par"
#define DSAM_XML_PAR_LIST_ELEMENT			"par_list"
#define DSAM_XML_PARARRAY_ELEMENT			"par_array"
#define DSAM_XML_OBJECT_ELEMENT				"object"
#define DSAM_XML_OUTPUT_ELEMENT				"output"
#define DSAM_XML_SIMULAION_ELEMENT			"simulation"

#define DSAM_XML_COUNT_ATTRIBUTE			"count"
#define DSAM_XML_ID_ATTRIBUTE				"id"
#define DSAM_XML_ENABLED_ATTRIBUTE			"enabled"
#define DSAM_XML_LABEL_ATTRIBUTE			"label"
#define DSAM_XML_NAME_ATTRIBUTE				"name"
#define DSAM_XML_OBJLABEL_ATTRIBUTE			"obj_label"
#define DSAM_XML_TYPE_ATTRIBUTE				"type"
#define DSAM_XML_VALUE_ATTRIBUTE			"value"
#define DSAM_XML_VERSION_ATTRIBUTE			"version"

#define DSAM_XML_CURRENT_ATTRIBUTE_VALUE	"current"
#define DSAM_XML_ORIGINAL_ATTRIBUTE_VALUE	"original"
#define DSAM_XML_PROCESS_ATTRIBUTE_VALUE	"process"

/******************************************************************************/
/*************************** Enum definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Pre reference definitions ************************/
/******************************************************************************/

class TiXmlNode;

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** SDIDocument **************************************/

class DSAMXMLDocument: public TiXmlDocument
{

  private:
	EarObjectPtr	mySimProcess;
	SimScriptPtr	simScriptPtr;

  public:
  
	DSAMXMLDocument(void);
	~DSAMXMLDocument(void);

	virtual void	AddShapeInfo(TiXmlNode &node, void *shape) { ; }
	virtual void	AddLineShapes(TiXmlNode &node)	{ ;}
	virtual void	GetShapeInfo(TiXmlNode *parent, DatumPtr pc)	{ ;}
	virtual void	GetLineShapeInfo(TiXmlNode *parent)	{ ;}

	void	AddAppInfo(TiXmlNode &parent);
	void	AddParGeneral(TiXmlNode &node, UniParPtr p);
	void	AddParList(TiXmlNode &node, UniParListPtr parList, const wxChar *
			  name = NULL);
	void	AddParListStandard(TiXmlNode &node, UniParListPtr parList);
	void	AddSimConnections(TiXmlNode &node, DynaListPtr list, char * type);
	DatumPtr	AddSimObjects(TiXmlNode &node, DatumPtr start);
	void	AddSimulation(TiXmlNode &node, EarObjectPtr simProcess);
	void	Create(EarObjectPtr simProcess);
	wxString &	CreateNotification(TiXmlNode *node, wxChar *format, va_list
	 			  args);
	void	GetApplicationInfo(void);
	bool	GetCFListInfo(TiXmlElement *parentElement, UniParList *parList);
	bool	GetConnectionInfo(TiXmlElement *objectElement, DynaListPtr *p,
			  const char * type);
	bool	GetICListInfo(TiXmlElement *parentElement, UniParList *parList);
	bool	GetParArrayInfo(TiXmlElement * myElement, UniParList *parList);
	bool	GetParListInfo(TiXmlNode *parListNode, UniParList *parList);
	bool	GetParInfo(TiXmlNode *parentElement, UniParList *parList);
	EarObjectPtr	GetSimProcess(void)		{ return mySimProcess; }
	bool	GetSimulationInfo(TiXmlNode *simNode);
	DatumPtr	InstallInst(TiXmlElement *objectElement, int type);
	DatumPtr	InstallProcess(TiXmlElement *objectElement);
	bool	InstallSimulationNodes(TiXmlElement *simElement);
	void	SetSimScriptPtr(SimScriptPtr ptr)	{ simScriptPtr = ptr; }
	bool	Translate(void);
	bool	ValidVersion(const wxString &s1, const wxString &s2);
	void	XMLNotifyWarning(TiXmlNode *node, wxChar *format, ...);
	void	XMLNotifyError(TiXmlNode *node, wxChar *format, ...);
	
};

#endif
