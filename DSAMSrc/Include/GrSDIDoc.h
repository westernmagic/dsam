/**********************
 *
 * File:		GrSDIDoc.h
 * Purpose: 	Simulation Design Interface base document class.
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
#include <wx/filesys.h>

#include <wx/ogl/ogl.h>

#if wxUSE_STD_IOSTREAM
class ostream;
class istream;
#endif

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define	SDI_DOCUMENT_DIAGRAM_EXTENSION	"dia"
#define SDI_DOCUMENT_SIM_FILE_EXT		"sim"
#define SDI_DOCUMENT_SPF_FILE_EXT		"spf"

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

	virtual wxOutputStream&	SaveObject(wxOutputStream& stream);
	virtual wxInputStream&	LoadObject(wxInputStream& stream);

	inline wxDiagram *	GetDiagram() { return &diagram; }

	void	SetAppInterfaceFile(wxFileName &fileName);
	bool	OnCloseDocument(void);
	bool	OnNewDocument(void);

};

#endif
