/**********************
 *
 * File:		GrSDIDoc.h
 * Purpose: 	Simulation Design Interface document classes.
 * Comments:	Revised from Julian Smart's Ogledit/doc.h
 * Author:		L.P.O'Mard
 * Created:		04 Nov 2002
 * Updated:		
 * Copyright:	(c) 2002, University of Essex
 *
 **********************/

#ifndef _GRSDIDOC_H
#define _GRSDIDOC_H 1

#ifdef __GNUG__
// #pragma interface
#endif

#include <wx/docview.h>
#include <wx/string.h>
#include <wx/wxexpr.h>

#include <wx/ogl/ogl.h>

#if wxUSE_STD_IOSTREAM
class ostream;
class istream;
#endif

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define	SDI_DOCUMENT_DIAGRAM_EXTENSION	"dia"

/******************************************************************************/
/*************************** Enum definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** SDIDocument **************************************/

/*
 * A diagram document, which contains a diagram.
 */
 
class SDIDocument: public wxDocument
{
	DECLARE_DYNAMIC_CLASS(SDIDocument)

  private:

  public:
	SDIDiagram diagram;

	SDIDocument(void);
	~SDIDocument(void);

	#if wxUSE_STD_IOSTREAM
	virtual ostream&	SaveObject(ostream& stream);
	virtual istream&	LoadObject(istream& stream);
	#else
	virtual wxOutputStream&	SaveObject(wxOutputStream& stream);
	virtual wxInputStream&	LoadObject(wxInputStream& stream);
	#endif

	inline wxDiagram *	GetDiagram() { return &diagram; }

	bool	OnCloseDocument(void);
	bool	OnNewDocument(void);

};

#endif
  // _OGLSAMPLE_DOC_H_
