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

#include "wx/txtstrm.h"

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
/****************************** GotDiagramInfo ********************************/
/******************************************************************************/

/*
 * This routine returns TRUE if Diagram information can be found in the file.
 */

bool
SDIDocument::GotDiagramInfo(wxInputStream& stream)
{
	wxTextInputStream text(stream);

	while (!stream.Eof())
		if (text.ReadLine().Contains(SIMSCRIPT_SIMPARFILE_SDI_DIVIDER))
			return(true);
	return (false);

}

/******************************************************************************/
/****************************** LoadObject ************************************/
/******************************************************************************/

wxInputStream&
SDIDocument::LoadObject(wxInputStream& stream)
{
	printf("SDIDocument::LoadObject: Entered\n");
	wxDocument::LoadObject(stream);

	SetSimFileType_AppInterface(GetSimFileType_Utility_SimScript((char *)
	  GetDocumentTemplate()->GetDefaultExtension().GetData()));
	printf("SDIDocument::LoadObject: Here I need to set the directory and "
	  "file name\n");
	wxFileName	fileName = GetFilename();
	printf("SDIDocument::LoadObject: directory = '%s'\n", (char *)
	  fileName.GetPath().GetData());
	printf("SDIDocument::LoadObject: file name = '%s'\n", (char *)
	  fileName.GetFullName().GetData());

	SetParsFilePath_AppInterface((char *) fileName.GetPath().GetData());
	SetSimFileName_AppInterface((char *) fileName.GetFullName().GetData());
	wxString tempFileName = wxFileName::CreateTempFileName("simFile");
	/* The 'wxGetTempFileName' routine is obsolete and should be replaced.*/
//	(void) wxGetTempFileName("diag", buf);
	printf("SDIDocument::LoadObject: File name = '%s'\n", (char *) tempFileName.
	  GetData());

	wxTransferStreamToFile(stream, tempFileName);

	diagram.DeleteAllShapes();
//	printf("SDIDocument::LoadObject: file name = '%s'\n", (char *) GetFilename(
//	  ).GetData());
//	wxGetApp().simFile.SetCwd(GetDocumentTemplate()->GetDirectory());
	wxGetApp().simFile = tempFileName;
	wxGetApp().GetFrame()->SetSimFileAndLoad();
	
//	if (!GotDiagramInfo(stream)) {
//		printf("SDIDocument::LoadObject: Draw basic simulation layout\n");
//		return(stream);
//	}
	diagram.LoadFile(tempFileName);
	wxRemoveFile(tempFileName);

	return stream;

}

#endif

#endif /* HAVE_WX_OGL_OGL_H */
