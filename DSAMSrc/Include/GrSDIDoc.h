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
#include <wx/filesys.h>

#include <wx/ogl/ogl.h>

#if wxUSE_STD_IOSTREAM
class ostream;
class istream;
#endif

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define SDI_DOCUMENT_SIM_FILE_EXT		wxT("sim")
#define SDI_DOCUMENT_SPF_FILE_EXT		wxT("spf")
#define SDI_DOCUMENT_XML_FILE_EXT		wxT("xml")

#if wxUSE_STD_IOSTREAM
#	define	SDI_DOC_OSTREAM wxSTD ostream
#	define	SDI_DOC_ISTREAM wxSTD istream
#else
#	define	SDI_DOC_OSTREAM wxOutputStream
#	define	SDI_DOC_ISTREAM wxInputStream
#endif

/******************************************************************************/
/*************************** Enum definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Pre reference definitions ************************/
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

	virtual SDI_DOC_OSTREAM&	SaveObject(SDI_DOC_OSTREAM& stream);
	virtual SDI_DOC_ISTREAM&	LoadObject(SDI_DOC_ISTREAM& stream);

	inline wxDiagram *	GetDiagram() { return &diagram; }

	bool	OnCloseDocument(void);
	bool	OnNewDocument(void);
	SDI_DOC_OSTREAM&	SaveXMLObject(SDI_DOC_OSTREAM& stream);

};

#endif
