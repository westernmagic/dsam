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
#include "ExtXMLDocument.h"
#include "GrMainApp.h"
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
	wxGetApp().GetGrMainApp()->SetDiagram(&diagram);

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

SDI_DOC_OSTREAM&
SDIDocument::SaveObject(SDI_DOC_OSTREAM& stream)
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

SDI_DOC_ISTREAM&
SDIDocument::LoadObject(SDI_DOC_ISTREAM& stream)
{
	static const char *funcName = "SDIDocument::LoadObject";
	bool	isXMLFile;
	wxFileName tempFileName;
	wxDocument::LoadObject(stream);

	wxFileName	fileName = GetFilename();

	diagram.DeleteAllShapes();
	if ((isXMLFile = (GetSimFileType_Utility_SimScript((char *) fileName.GetExt(
	  ).c_str()) == UTILITY_SIMSCRIPT_XML_FILE)) == true) {
		wxGetApp().GetGrMainApp()->SetSimulationFile(fileName);
		if (!wxGetApp().GetGrMainApp()->LoadXMLDocument()) {
			NotifyError("%s: Could not load XML Document.", funcName);
			return(stream);
		}
		GetPtr_AppInterface()->canLoadSimulationFlag = false;
	} else {
// Do I need this stream-swapping stuff?
		wxGetApp().GetGrMainApp()->SetSimulationFile(fileName);
//		tempFileName = wxFileName::CreateTempFileName("simFile");
//		wxTransferStreamToFile(stream, tempFileName.GetFullName());
//		wxGetApp().GetGrMainApp()->SetSimulationFile(tempFileName);
	}
	if (!wxGetApp().GetFrame()->LoadSimulation()) {
		wxRemoveFile(tempFileName.GetFullName());
		return(stream);
	}
	GetPtr_AppInterface()->canLoadSimulationFlag = true;

	diagram.SetSimProcess(GetSimProcess_AppInterface());
	diagram.DrawSimulation();

//	if (!isXMLFile) {
//		wxRemoveFile(tempFileName.GetFullName());
//		wxGetApp().GetGrMainApp()->SetSimulationFile(fileName);
//	}
	wxGetApp().SetAudModelLoadedFlag(true);
	wxString lastDirectory = (fileName.GetPath().StartsWith(wxGetCwd()))?
	  fileName.GetPath(): wxGetCwd() + fileName.GetPathSeparator() + fileName.
	  GetPath();
	GetDocumentManager( )->SetLastDirectory(lastDirectory);
	return stream;

}

/******************************************************************************/
/****************************** SaveXMLObject *********************************/
/******************************************************************************/

SDI_DOC_OSTREAM&
SDIDocument::SaveXMLObject(SDI_DOC_OSTREAM& stream)
{
	wxDocument::SaveObject(stream);
	diagram.SetSimProcess(GetSimProcess_AppInterface());
	SDIXMLDocument	doc(&diagram);
	doc.Create();
	doc.SaveFile(GetFilename());
	return(stream);

}

#endif /* USE_WX_OGL */
