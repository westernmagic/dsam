/**********************
 *
 * File:		GrIPCServer.h
 * Purpose:		Inter-process communication server extension code module for
 *				The GUI.
 * Comments:	This is a subclass of the IPCServer class (ExtIPCServer code 
 *				module.
 * Author:		L. P. O'Mard
 * Created:		03 Dec 2003
 * Updated:		
 * Copyright:	(c) 2003, University of Essex
 *
 **********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>

#include "ExtCommon.h"

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "ExtIPCServer.h"
#include "GrIPCServer.h"
#include "GrSimMgr.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Methods (Subroutines) *************************/
/******************************************************************************/

/****************************** Constructor ***********************************/

GrIPCServer::GrIPCServer(const wxString& hostName, uShort theServicePort):
  IPCServer(hostName, theServicePort)
{

}

/****************************** Destructor ************************************/

GrIPCServer::~GrIPCServer(void)
{
	
}

/******************************************************************************/
/****************************** IPCServer Methods *****************************/
/******************************************************************************/

/****************************** LoadSimFile ***********************************/

/*
 * This routine loads the simulation file.  This is a virtual function and
 * classes that call this routine should create their on instance of this
 * this function.
 */

void
GrIPCServer::LoadSimFile(const wxString& fileName)
{
	wxGetApp().CreateDocument(fileName);

}

/****************************** SetNotification *******************************/

/*
 * Sets up the notification for event handling.
 */

void
GrIPCServer::SetNotification(wxEvtHandler *theHandler)
{
	GetServer()->SetEventHandler(*theHandler, IPCSERVER_APP_SERVER_ID);
	GetServer()->SetNotify(wxSOCKET_CONNECTION_FLAG);
	GetServer()->Notify(TRUE);

}

/******************************************************************************/
/****************************** Callbacks *************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Functions *************************************/
/******************************************************************************/
