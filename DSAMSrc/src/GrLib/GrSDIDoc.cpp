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

#include "DSAM.h"

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
	printf("SDIDocument::SDIDocument: Entered\n");

}

/******************************************************************************/
/****************************** Destructor ************************************/
/******************************************************************************/

SDIDocument::~SDIDocument(void)
{
	printf("SDIDocument::~SDIDocument: Entered\n");
	ResetSimulation_AppInterface();
	wxGetApp().SetAudModelLoadedFlag(false);

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
/****************************** OnNewDocument *********************************/
/******************************************************************************/

bool
SDIDocument::OnNewDocument(void)
{
	static const char *funcName = "DIDocument::OnNewDocument";

	wxDocument::OnNewDocument();
	printf("SDIDocument::OnNewDocument: Called\n");
	ResetSimulation_AppInterface();
	if ((GetPtr_AppInterface()->audModel = Init_EarObject("Util_SimScript")) ==
	  NULL) {
		NotifyError("%s: Could not initialise process.", funcName);
		return(false);
	}
	SetProcessSimPtr_Utility_SimScript(GetPtr_AppInterface()->audModel);
	wxGetApp().SetAudModelLoadedFlag(true);
	return (true);

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
