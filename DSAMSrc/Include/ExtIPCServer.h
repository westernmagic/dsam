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
#include "ExtIPCUtils.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define	EXTIPC_DEFAULT_SERVER_NAME		"DSAM IPC Server"

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

/*************************** IPCServer ****************************************/

class IPCServer {

	bool	ok, simulationInitialisedFlag;
	IPCUtils	iPCUtils;
	wxString	buffer;
	wxArrayString	notificationList;
	wxSocketBase	*sock;
	wxSocketServer	*myServer;

  public:
	IPCServer(const wxString& hostName, uShort theServicePort);
	virtual	~IPCServer(void);

	virtual void	LoadSimFile(const wxString& fileName);

	void	AddNotification(wxString &s)	{ notificationList.Add(s); }
	void	BuildFileList(wxArrayString &list, DatumPtr pc);
	void	ClearNotifications(void)	{ notificationList.Empty(); }
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
	void	OnPutArgs(void);
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

