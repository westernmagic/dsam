/**********************
 *
 * File:		GrSDIDoc.cpp
 * Purpose: 	Simulation Design Interface base document class.
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

#ifdef USE_WX_OGL

#include <stdio.h>
#include <stdlib.h>

#include "ExtCommon.h"

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"

#include "UtDatum.h"
#include "UtSSSymbols.h"
#include "UtSSParser.h"
#include "UtSimScript.h"
#include "UtAppInterface.h"
#include "tinyxml.h"

#include "GrIPCServer.h"
#include "GrSimMgr.h"
#include "GrSDIFrame.h"
#include "GrSDIDiagram.h"
#include "GrSDIDoc.h"
#include "GrSDIXMLDoc.h"

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
/****************************** SetAppInterfaceFile ***************************/
/******************************************************************************/

/*
 * Sets the application interface file and name.
 */

void
SDIDocument::SetAppInterfaceFile(wxFileName &fileName)
{
	SetParsFilePath_AppInterface((char *) fileName.GetPath().c_str());
	SetSimFileName_AppInterface((char *) fileName.GetFullName().c_str());

}

/******************************************************************************/
/****************************** OnCloseDocument *******************************/
/******************************************************************************/

bool
SDIDocument::OnCloseDocument(void)
{
	diagram.DeleteAllShapes();
	diagram.SetSimProcess(NULL);
	return true;

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

wxOutputStream&
SDIDocument::SaveObject(wxOutputStream& stream)
{
	wxFileName	tempFileName, fileName = GetFilename();

	wxDocument::SaveObject(stream);
	if (fileName.GetExt().IsSameAs(SDI_DOCUMENT_XML_FILE_EXT, FALSE))
		return(SaveXMLObject(stream));

	DiagModeSpecifier oldDiagMode = GetDSAMPtr_Common()->diagMode;
	tempFileName.AssignTempFileName("diag");

	SetDiagMode(COMMON_CONSOLE_DIAG_MODE);
	if (fileName.GetExt().IsSameAs(SDI_DOCUMENT_SIM_FILE_EXT, FALSE)) {
		WriteParFiles_Datum((char *) fileName.GetPath().c_str(),
		  GetSimulation_AppInterface());
		WriteSimScript_Datum((char *)tempFileName.GetFullPath().c_str(),
		  GetSimulation_AppInterface());
	} else {
		FILE *oldFp = GetDSAMPtr_Common()->parsFile;
		SetDiagMode(COMMON_CONSOLE_DIAG_MODE);
		SetParsFile_Common((char * )tempFileName.GetFullPath().c_str(),
		  OVERWRITE);
		ListParameters_AppInterface();
		fclose(GetDSAMPtr_Common()->parsFile);
		GetDSAMPtr_Common()->parsFile = oldFp;
	}
	SetDiagMode(oldDiagMode);
	wxTransferFileToStream(tempFileName.GetFullPath(), stream);
	wxRemoveFile(tempFileName.GetFullPath());

	return stream;

}

/******************************************************************************/
/****************************** LoadObject ************************************/
/******************************************************************************/

wxInputStream&
SDIDocument::LoadObject(wxInputStream& stream)
{
	/*static const char *funcName = "SDIDocument::LoadObject";*/
	printf("SDIDocument::LoadObject: Entered.\n");
	wxDocument::LoadObject(stream);

	wxFileName	fileName = GetFilename();
	SetSimFileType_AppInterface(GetSimFileType_Utility_SimScript((char *)
	  fileName.GetExt().c_str()));

	SetAppInterfaceFile(fileName);
	wxString tempFileName = wxFileName::CreateTempFileName("simFile");
	wxTransferStreamToFile(stream, tempFileName);

	diagram.DeleteAllShapes();
	wxGetApp().simFile = tempFileName;
	if (!wxGetApp().GetFrame()->SetSimFileAndLoad()) {
		wxRemoveFile(tempFileName);
		return(stream);
	}

#	if wxUSE_PROLOGIO
	wxFileName	diagFileName = fileName;
	diagFileName.SetExt(SDI_DOCUMENT_DIAGRAM_EXTENSION);
	diagram.SetSimProcess(GetSimProcess_AppInterface());
	if (!diagFileName.FileExists())
		diagram.DrawSimulation();
	else {
		if (!diagram.LoadFile(diagFileName.GetFullPath()) ||
		  !diagram.SetShapeHandlers() || !diagram.VerifyDiagram()) {
			wxLogError("%s: Error loading diagram file - default layout "
			  "used.\n", funcName);
			diagram.DeleteAllShapes();
			diagram.DrawSimulation();
		}
	}
#	else
	diagram.SetSimProcess(GetSimProcess_AppInterface());
	diagram.DrawSimulation();
#	endif
	wxRemoveFile(tempFileName);
	wxGetApp().SetAudModelLoadedFlag(true);
	GetDocumentTemplate()->SetDirectory(fileName.GetPath());
	wxSetWorkingDirectory(fileName.GetPath()); // Above line seems necessary.
	return stream;

}

/******************************************************************************/
/****************************** SaveXMLObject *********************************/
/******************************************************************************/

wxOutputStream&
SDIDocument::SaveXMLObject(wxOutputStream& stream)
{
	static const char *funcName = "SDIDocument::SaveXMLObject";

	printf("%s: Entered\n", funcName);
	wxDocument::SaveObject(stream);
	SDIXMLDocument	doc(diagram.GetSimProcess());
	doc.SaveFile(GetFilename());
	return(stream);

}

#endif /* USE_WX_OGL */
