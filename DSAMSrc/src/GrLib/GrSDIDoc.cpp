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

#if defined(GRAPHICS_SUPPORT) && defined(__cplusplus)
// For compilers that support precompilation, includes "wx.h".
#	include <wx/wxprec.h>

#	ifdef __BORLANDC__
	    #pragma hdrstop
#	endif

// Any files you want to include if not precompiling by including
// the whole of <wx/wx.h>
#	ifndef WX_PRECOMP
#		include <wx/wx.h>
#	endif
// Any files included regardless of precompiled headers
#	include "wx/txtstrm.h"
#endif /* GRAPHICS_SUPPORT */

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"

#include "UtDatum.h"
#include "UtSSSymbols.h"
#include "UtSimScript.h"
#include "UtAppInterface.h"

#include "ExtIPCServer.h"

#include "GrSimMgr.h"
#include "GrSDIDiagram.h"
#include "GrSDIDoc.h"

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
	SetParsFilePath_AppInterface((char *) fileName.GetPath().GetData());
	SetSimFileName_AppInterface((char *) fileName.GetFullName().GetData());

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

wxOutputStream&
SDIDocument::SaveObject(wxOutputStream& stream)
{
	return stream;

}

/******************************************************************************/
/****************************** LoadObject ************************************/
/******************************************************************************/

wxInputStream&
SDIDocument::LoadObject(wxInputStream& stream)
{

	return stream;

}

#endif /* USE_WX_OGL */
