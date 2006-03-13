/**********************
 *
 * File:		GrMainApp.cpp
 * Purpose:		This routine contains the classed GUI specific class derived
 *				from the MainApp class for handling the DSAM extensions
 *				library.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		16 Dec 2003
 * Updated:		
 * Copyright:	(c) 2003 CNBH, University of Essex
 *
 **********************/
 
#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>

#include <ExtCommon.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "UtSSSymbols.h"
#include "UtAppInterface.h"
#include "ExtMainApp.h"
#include "GrSimMgr.h"
#include "GrSDIFrame.h"
#include "tinyxml.h"
#include "ExtXMLDocument.h"
#include "GrSDIXMLDoc.h"
#include "GrMainApp.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/
/******************************************************************************/
/****************************** Methods (Subroutines) *************************/
/******************************************************************************/

/****************************** Constructor ***********************************/

/*
 * This is the main routine for using the extensions library, and is called by
 * the "main" subroutine in the header file.
 */

GrMainApp::GrMainApp(int theArgc, wxChar **theArgv): MainApp(theArgc, theArgv,
  MainSimulation)
{
	dSAMMainApp = this;
	wxGetApp().ResetDefaultDisplayPos();
	if (GetPtr_AppInterface())
		wxGetApp().SetConfiguration(GetPtr_AppInterface()->parList);
	wxFileName simFile(GetPtr_AppInterface()->simulationFile);
	wxSetWorkingDirectory(simFile.GetPath());
	SetOnExecute_AppInterface(OnExecute_MyApp);
	SetOnExit_AppInterface(OnExit_MyApp);
	ResetGUIDialogs();
	SetDiagMode(COMMON_DIALOG_DIAG_MODE);
	InitMain();

}

/****************************** InitXMLDocument *******************************/

/*
 * This initialises the 
 */

void
GrMainApp::InitXMLDocument(void)
{
	doc = new SDIXMLDocument(diagram);

}

/****************************** ResetSimulation *******************************/

bool
GrMainApp::ResetSimulation(void)
{
	bool ok = MainApp::ResetSimulation();
	wxGetApp().EnableSimParMenuOptions(ok);
	return(ok);

}

/****************************** SetRunIndicators ******************************/

void
GrMainApp::SetRunIndicators(bool on)
{
	wxGetApp().programMenu->Enable(SDIFRAME_STOP_SIMULATION, on);

}

/******************************************************************************/
/****************************** Callbacks *************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Functions *************************************/
/******************************************************************************/
