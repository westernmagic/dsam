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

#ifndef _GRIPCSERVER_H
#define _GRIPCSERVER_H 1

#if defined(GRAPHICS_SUPPORT) && defined(__cplusplus)
#	include <wx/socket.h>
#endif

#include "ExtIPCServer.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Enum definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/********************************** Pre-references ****************************/


/*************************** GrIPCServer **************************************/

class GrIPCServer: public IPCServer {

  public:
	GrIPCServer(const wxString& hostName, uShort theServicePort);
	~GrIPCServer(void);

	void	LoadSimFile(const wxString& fileName);
	void	SetNotification(wxEvtHandler *theHandler);

};

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Call back prototypes *****************************/
/******************************************************************************/

#endif

