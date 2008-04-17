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

/******************************************************************************/
/*************************** Macro definitions ********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Enum definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Pre reference definitions ************************/
/******************************************************************************/

class SDIDiagram;
class SDIShape;
class DSAMXMLNode;
class wxXmlNode;

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** SDIDocument **************************************/

class SDIXMLDocument: public DSAMXMLDocument {

  private:
	bool	diagramLoadInitiated;
	SDIDiagram *diagram;

  public:
  
  	SDIXMLDocument(SDIDiagram *theDiagram = NULL);
	~SDIXMLDocument(void);

	void	AddShapeInfo(DSAMXMLNode *parent, void *shape);
	void	AddLineShapes(DSAMXMLNode *parent);
	void	Create(void);
	SDIShape *	CreateLoadShape(wxXmlNode *shapeElement, DatumPtr pc);
	wxShape *	FindShape(long id);
	wxClassInfo *	GetClassInfo(wxXmlNode *shapeElement);
	void	GetConnectionsInfo(wxXmlNode *simElement);
	void	GetLineShapeInfo(wxXmlNode *myElement);
	void	GetShapeInfo(wxXmlNode *shapeElement, DatumPtr pc);

};

#endif
