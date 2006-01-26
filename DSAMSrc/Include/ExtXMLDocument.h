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

#define DSAM_XML_APPLICATION_ELEMENT		_T("application")
#define DSAM_XML_CFLIST_ELEMENT				_T("cflist")
#define DSAM_XML_CONNECTIONS_ELEMENT		_T("connections")
#define DSAM_XML_DSAM_ELEMENT				_T("dsam")
#define DSAM_XML_INPUT_ELEMENT				_T("input")
#define DSAM_XML_PAR_ELEMENT				_T("par")
#define DSAM_XML_PAR_LIST_ELEMENT			_T("par_list")
#define DSAM_XML_PARARRAY_ELEMENT			_T("par_array")
#define DSAM_XML_OBJECT_ELEMENT				_T("object")
#define DSAM_XML_OUTPUT_ELEMENT				_T("output")
#define DSAM_XML_SIMULAION_ELEMENT			_T("simulation")

#define DSAM_XML_COUNT_ATTRIBUTE			_T("count")
#define DSAM_XML_ID_ATTRIBUTE				_T("id")
#define DSAM_XML_ENABLED_ATTRIBUTE			_T("enabled")
#define DSAM_XML_LABEL_ATTRIBUTE			_T("label")
#define DSAM_XML_NAME_ATTRIBUTE				_T("name")
#define DSAM_XML_OBJLABEL_ATTRIBUTE			_T("obj_label")
#define DSAM_XML_TYPE_ATTRIBUTE				_T("type")
#define DSAM_XML_VALUE_ATTRIBUTE			_T("value")
#define DSAM_XML_VERSION_ATTRIBUTE			_T("version")

#define DSAM_XML_CURRENT_ATTRIBUTE_VALUE	_T("current")
#define DSAM_XML_ORIGINAL_ATTRIBUTE_VALUE	_T("original")
#define DSAM_XML_PROCESS_ATTRIBUTE_VALUE	_T("process")

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
	DynaBListPtr	labelBList;
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
	void	AddParList(TiXmlNode &node, UniParListPtr parList, const char
			  *name = NULL);
	void	AddParListStandard(TiXmlNode &node, UniParListPtr parList);
	void	AddSimConnections(TiXmlNode &node, DynaListPtr list, char * type);
	DatumPtr	AddSimObjects(TiXmlNode &node, DatumPtr start);
	void	AddSimulation(TiXmlNode &node, EarObjectPtr simProcess);
	void	Create(EarObjectPtr simProcess);
	wxString &	CreateNotification(TiXmlNode *node, char *format, va_list args);
	void	GetApplicationInfo(void);
	bool	GetCFListInfo(TiXmlElement *parentElement, UniParList *parList);
	bool	GetConnectionInfo(TiXmlElement *objectElement, DynaListPtr *p,
			  const char * type);
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
	void	XMLNotifyWarning(TiXmlNode *node, char *format, ...);
	void	XMLNotifyError(TiXmlNode *node, char *format, ...);
	
};

#endif
