/**********************
 *
 * File:		ExtIPCServer.h
 * Purpose:		Inter-process communication server extension code module.
 * Comments:	This code module was revised from the GrSimMgr.cpp code module.
 * Author:		L. P. O'Mard
 * Created:		06 Nov 2003
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


/*************************** IPCServer ****************************************/

class IPCServer {

	bool	ok;
	wxString	buffer;
	wxSocketBase	*sock;
	wxSocketServer	*myServer;
	wxEvtHandler	*handler;
	IPCServerModeSpecifier	mode;

  public:
	IPCServer(const wxString& hostName, uShort theServicePort);
	~IPCServer(void);

	NameSpecifier *	CommandList(int index);
	IPCServerModeSpecifier GetMode(void)	{ return mode; }
	wxSocketServer	*	GetServer(void)	{ return myServer; }
	wxSocketBase *	GetSocket(void)	{ return sock; };
	wxSocketBase *	InitConnection(void);
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

void	DPrint_IPCServer(char *format, va_list args);

void	EmptyDiagBuffer_IPCServer(char *s, int *c);

void	Notify_IPCServer(const char *format, va_list args,
		  CommonDiagSpecifier type);

/******************************************************************************/
/*************************** Call back prototypes *****************************/
/******************************************************************************/

#endif

