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
	FreeSim_AppInterface();
	wxGetApp().SetAudModelLoadedFlag(false);

}

/******************************************************************************/
/****************************** OnCloseDocument *******************************/
/******************************************************************************/

bool
SDIDocument::OnCloseDocument(void)
{
	diagram.DeleteAllShapes();
	diagram.SetSimulation(NULL);
	return TRUE;

}

/******************************************************************************/
/****************************** OnNewDocument *********************************/
/******************************************************************************/

bool
SDIDocument::OnNewDocument(void)
{
	static const char *funcName = "SDIDocument::OnNewDocument";

	wxDocument::OnNewDocument();
	FreeSim_AppInterface();
	wxClientDC dc(diagram.GetCanvas());
	diagram.Clear(dc);			// This was not being done automatically.
	if ((GetPtr_AppInterface()->audModel = Init_EarObject("Util_SimScript")) ==
	  NULL) {
		NotifyError("%s: Could not initialise process.", funcName);
		return(false);
	}
	SetProcessSimPtr_Utility_SimScript(GetPtr_AppInterface()->audModel);
	SetSimulationFileFlag_AppInterface(FALSE);
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
	char tempFileName[MAX_FILE_PATH];

	wxDocument::SaveObject(stream);
	(void) wxGetTempFileName("diag", tempFileName);

	bool dialogOutputFlag = CXX_BOOL(GetDSAMPtr_Common()->dialogOutputFlag);
	FILE *oldFp = GetDSAMPtr_Common()->parsFile;
	SetGUIDialogStatus(FALSE);
	SetParsFile_Common(tempFileName, OVERWRITE);
	ListParameters_AppInterface();
	fclose(GetDSAMPtr_Common()->parsFile);
	GetDSAMPtr_Common()->parsFile = oldFp;
	SetGUIDialogStatus(dialogOutputFlag);
	wxTransferFileToStream(tempFileName, stream);
	wxRemoveFile(tempFileName);

	wxFileName	diagFileName = GetFilename();
	diagFileName.SetExt(SDI_DOCUMENT_DIAGRAM_EXTENSION);
	diagram.SaveFile(diagFileName.GetFullPath());

	return stream;

}

/******************************************************************************/
/****************************** LoadObject ************************************/
/******************************************************************************/

wxInputStream&
SDIDocument::LoadObject(wxInputStream& stream)
{
	static const char *funcName = "SDIDocument::LoadObject";

	wxDocument::LoadObject(stream);

	wxFileName	fileName = GetFilename();
	SetSimFileType_AppInterface(GetSimFileType_Utility_SimScript((char *)
	  fileName.GetExt().GetData()));

	SetParsFilePath_AppInterface((char *) fileName.GetPath().GetData());
	SetSimFileName_AppInterface((char *) fileName.GetFullName().GetData());
	wxString tempFileName = wxFileName::CreateTempFileName("simFile");
	wxTransferStreamToFile(stream, tempFileName);

	diagram.DeleteAllShapes();
	wxGetApp().simFile = tempFileName;
	if (!wxGetApp().GetFrame()->SetSimFileAndLoad())
		return(stream);

	wxFileName	diagFileName = fileName;
	diagFileName.SetExt(SDI_DOCUMENT_DIAGRAM_EXTENSION);
	diagram.SetSimulation(GetSimulation_AppInterface());
	if (!diagFileName.FileExists())
		diagram.DrawSimulation();
	else {
		;
		if (!diagram.LoadFile(diagFileName.GetFullPath()) ||
		  !diagram.SetShapeHandlers() || !diagram.VerifyDiagram()) {
			wxLogError("%s: Error loading diagram file - default layout "
			  "used.\n", funcName);
			diagram.DeleteAllShapes();
			diagram.DrawSimulation();
		}
	}
	wxRemoveFile(tempFileName);
	wxGetApp().SetAudModelLoadedFlag(true);

	return stream;

}

#endif

#endif /* HAVE_WX_OGL_OGL_H */
