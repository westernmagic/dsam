/**********************
 *
 * File:		GrDSAMXMLDocument.h
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

#include <wx/ogl/ogl.h>

#if wxUSE_STD_IOSTREAM
class ostream;
class istream;
#endif

#include "UtDynaList.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define DSAM_XML_APPLICATION_ELEMENT		_T("application")
#define DSAM_XML_DSAM_ELEMENT				_T("dsam")
#define DSAM_XML_INPUT_ELEMENT				_T("input")
#define DSAM_XML_PAR_ELEMENT				_T("par")
#define DSAM_XML_OBJECT_ELEMENT				_T("object")
#define DSAM_XML_OUTPUT_ELEMENT				_T("output")
#define DSAM_XML_SIMULAION_ELEMENT			_T("simulation")

#define DSAM_XML_COUNT_ATTRIBUTE			_T("count")
#define DSAM_XML_LABEL_ATTRIBUTE			_T("label")
#define DSAM_XML_NAME_ATTRIBUTE				_T("name")
#define DSAM_XML_OBJLABEL_ATTRIBUTE			_T("obj_label")
#define DSAM_XML_TYPE_ATTRIBUTE				_T("type")

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
  public:
  
  	DSAMXMLDocument(void);
	~DSAMXMLDocument(void);

	virtual void	AddShapeInfo(TiXmlNode &node, void *shape) { printf(
	"no shape info\n"); }

	void	AddAppInfo(TiXmlNode &parent);
	void	AddParGeneral(TiXmlNode &node, UniParPtr p);
	void	AddParList(TiXmlNode &node, UniParListPtr parList);
	void	AddParListStandard(TiXmlNode &node, UniParListPtr parList);
	void	AddSimConnections(TiXmlNode &node, DynaListPtr list, char * type);
	DatumPtr	AddSimObjects(TiXmlNode &node, DatumPtr start);
	void	AddSimulation(TiXmlNode &node, EarObjectPtr simProcess);
	void	Create(EarObjectPtr simProcess);

};

#endif
