/**********************
 *
 * File:		ExtIPCMgr.cpp
 * Purpose:		Inter-process communication manager.
 * Comments:	This code module was removed from the GrSimMgr.cpp code module.
 *				This server will probably not be directly available to the
 *				internet, however, it is being coded to avoid "buffer overrun"
 *				problems.
 * Author:		L. P. O'Mard
 * Created:		06 Jan 1995
 * Updated:		
 * Copyright:	(c) 2003, University of Essex
 *
 **********************/

#ifndef _EXTIPCSERVER_H
#define _EXTIPCSERVER_H 1

#if defined(GRAPHICS_SUPPORT) && defined(__cplusplus)
#	include <wx/socket.h>
#endif

#include "UtNameSpecs.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define	EXTIPCSERVER_DEFAULT_SERVER_NAME		"DSAM IPC Server"
#define EXTIPCSERVER_DEFAULT_SERVER_PORT		3000
#define EXTIPCSERVER_MAX_COMMAND_CHAR			10

/******************************************************************************/
/*************************** Enum definitions *********************************/
/******************************************************************************/

enum {

	IPCSERVER_APP_SOCKET_ID = 1,
	IPCSERVER_APP_SERVER_ID

};

enum {

	IPCSERVER_COMMAND_QUIT,
	IPCSERVER_COMMAND_INIT,
	IPCSERVER_COMMAND_RUN,
	IPCSERVER_COMMAND_SET,
	IPCSERVER_COMMAND_GET,
	IPCSERVER_COMMAND_PUT,
	IPCSERVER_COMMAND_NULL

};

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	IPCSERVER_COMMAND_MODE,
	IPCSERVER_DATA_MODE

} IPCServerModeSpecifier;

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/********************************** Pre-references ****************************/


/*************************** IPCMgr *******************************************/

class IPCServer {

	bool	ok;
	wxString	buffer;
	wxSocketServer	*myServer;
	wxEvtHandler	*handler;
	IPCServerModeSpecifier	mode;

  public:
	IPCServer(const wxString& hostName, uShort theServicePort);
	~IPCServer(void);

	NameSpecifier *	CommandList(int index);
	IPCServerModeSpecifier GetMode(void)	{ return mode; }
	wxSocketServer	*	GetServer(void)	{ return myServer; }
	bool	Ok(void)	{ return ok; }
	void	ProcessInput(wxSocketBase *sock);
	void	ResetCommandMode(void)	{ mode = IPCSERVER_COMMAND_MODE; }
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

