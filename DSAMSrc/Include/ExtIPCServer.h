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

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "UtNameSpecs.h"
#include "UtUIEEEFloat.h"
#include "UtUPortableIO.h"
#include "UtDatum.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define	EXTIPCSERVER_DEFAULT_SERVER_NAME		"DSAM IPC Server"
#define EXTIPCSERVER_MAX_COMMAND_CHAR			10
#define EXTIPCSERVER_MEMORY_FILE_NAME			"+.aif"

/******************************************************************************/
/*************************** Enum definitions *********************************/
/******************************************************************************/

enum {

	IPCSERVER_APP_SOCKET_ID = 1,
	IPCSERVER_APP_SERVER_ID

};

enum {

	IPCSERVER_COMMAND_QUIT,

	IPCSERVER_COMMAND_ERRMSGS,
	IPCSERVER_COMMAND_GET,
	IPCSERVER_COMMAND_GETFILES,
	IPCSERVER_COMMAND_INIT,
	IPCSERVER_COMMAND_PUT,
	IPCSERVER_COMMAND_RUN,
	IPCSERVER_COMMAND_SET,
	IPCSERVER_COMMAND_STATUS,

	IPCSERVER_COMMAND_NULL

};

enum {

	IPCSERVER_STATUS_READY = 0,
	IPCSERVER_STATUS_BUSY,
	IPCSERVER_STATUS_ERROR

};

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/********************************** Pre-references ****************************/

/*************************** IPCServer ****************************************/

class IPCServer {

	bool	ok, simulationInitialisedFlag;
	wxString	buffer;
	EarObjectPtr	inProcess, outProcess;
	wxArrayString	notificationList;
	wxSocketBase	*sock;
	wxSocketServer	*myServer;
	UPortableIOPtr	inUIOPtr, outUIOPtr;

  public:
	IPCServer(const wxString& hostName, uShort theServicePort);
	virtual	~IPCServer(void);

	virtual void	LoadSimFile(const wxString& fileName);

	void	AddNotification(wxString &s)	{ notificationList.Add(s); }
	void	BuildFileList(wxArrayString &list, DatumPtr pc);
	void	ClearNotifications(void)	{ notificationList.Empty(); }
	NameSpecifier *	CommandList(int index);
	wxSocketServer	*	GetServer(void)	{ return myServer; }
	wxSocketBase *	GetSocket(void)	{ return sock; };
	wxSocketBase *	InitConnection(bool wait = true);
	bool	InitInProcess(void);
	bool	InitOutProcess(void);
	void	OnInit(void);
	void	OnExecute(void);
	void	OnGet(void);
	bool	Ok(void)	{ return ok; }
	void	OnGetFiles(void);
	void	OnPut(void);
	void	OnSet(void);
	void	OnStatus(void);
	void	OnErrMsgs(int index = -1);
	bool	ProcessInput(void);
	void	ResetInProcess(void);
	void	ResetOutProcess(void);
	bool	RunInProcess(void);
	bool	RunOutProcess(void);

};

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

void	DPrint_IPCServer(char *format, va_list args);

void	EmptyDiagBuffer_IPCServer(char *s, int *c);

IPCServer *	GetPtr_IPCServer(void);

void	Notify_IPCServer(const char *format, va_list args,
		  CommonDiagSpecifier type);

/******************************************************************************/
/*************************** Call back prototypes *****************************/
/******************************************************************************/

#endif

