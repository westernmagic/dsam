/**********************
 *
 * File:		GrSDIDoc.cpp
 * Purpose: 	Simulation Design Interface document classes.
 * Comments:	Revised from Julian Smart's Ogledit/doc.h
 * Author:		L.P.O'Mard
 * Created:		04 Nov 2002
 * Updated:		
 * Copyright:	(c) 2002, University of Essex
 *
 **********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#ifdef HAVE_WX_OGL_OGL_H

#ifdef __GNUG__
// #pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "GeCommon.h"
#include "GrSDIDiagram.h"
#include "GrSDIDoc.h"

#if wxUSE_STD_IOSTREAM
#include <iostream.h>
#endif

/******************************************************************************/
/****************************** Bitmaps ***************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Methods (Subroutines) *************************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDIDocument, wxDocument)

/******************************************************************************/
/****************************** Constructor ***********************************/
/******************************************************************************/

SDIDocument::SDIDocument(void)
{
}

/******************************************************************************/
/****************************** Destructor ************************************/
/******************************************************************************/

SDIDocument::~SDIDocument(void)
{
}

/******************************************************************************/
/****************************** OnCloseDocument *******************************/
/******************************************************************************/

bool
SDIDocument::OnCloseDocument(void)
{
	diagram.DeleteAllShapes();
	return TRUE;

}

/******************************************************************************/
/****************************** SaveObject ************************************/
/******************************************************************************/

#if wxUSE_STD_IOSTREAM
ostream&
SDIDocument::SaveObject(ostream& stream)
{
	wxDocument::SaveObject(stream);

	char buf[400];
	(void) wxGetTempFileName("diag", buf);

	diagram.SaveFile(buf);
	wxTransferFileToStream(buf, stream);

	wxRemoveFile(buf);

	return stream;

}

/******************************************************************************/
/****************************** LoadObject ************************************/
/******************************************************************************/

istream&
SDIDocument::LoadObject(istream& stream)
{
	wxDocument::LoadObject(stream);

	char buf[400];
	(void) wxGetTempFileName("diag", buf);

	wxTransferStreamToFile(stream, buf);

	diagram.DeleteAllShapes();
	diagram.LoadFile(buf);
	wxRemoveFile(buf);

	return stream;

}

#else

/******************************************************************************/
/****************************** SaveObject ************************************/
/******************************************************************************/

wxOutputStream&
SDIDocument::SaveObject(wxOutputStream& stream)
{
	wxDocument::SaveObject(stream);
	char buf[400];
	(void) wxGetTempFileName("diag", buf);

	diagram.SaveFile(buf);

	wxTransferFileToStream(buf, stream);

	wxRemoveFile(buf);

	return stream;

}

/******************************************************************************/
/****************************** LoadObject ************************************/
/******************************************************************************/

wxInputStream&
SDIDocument::LoadObject(wxInputStream& stream)
{
	wxDocument::LoadObject(stream);

	char buf[400];
	(void) wxGetTempFileName("diag", buf);

	wxTransferStreamToFile(stream, buf);

	diagram.DeleteAllShapes();
	diagram.LoadFile(buf);
	wxRemoveFile(buf);

	return stream;

}

#endif

#endif /* HAVE_WX_OGL_OGL_H */
