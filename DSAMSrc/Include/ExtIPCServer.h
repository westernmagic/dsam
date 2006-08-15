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

#define	EXTIPC_DEFAULT_SERVER_NAME		wxT("DSAM IPC Server")

#ifndef __WINDOWS__
#	include <syslog.h>
#else
#	define syslog	fprintf
#	define vsyslog	vfprintf
#	define LOG_INFO	stderr
#endif

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

class SocketBase;
class SocketServer;

/*************************** IPCServer ****************************************/

class IPCServer {

	bool	ok, simulationInitialisedFlag, successfulRunFlag;
	IPCUtils	iPCUtils;
	wxString	buffer;
	wxArrayString	notificationList;
	SocketBase	*sock;
	SocketServer	*myServer;

  public:
	IPCServer(const wxString& hostName, uShort theServicePort,
	  bool superServerFlag = false);
	virtual	~IPCServer(void);

	virtual bool	LoadSimFile(const wxString& fileName);

	void	AddNotification(wxString &s)	{ notificationList.Add(s); }
	void	BuildFileList(wxArrayString &list, DatumPtr pc);
	void	ClearNotifications(void)	{ notificationList.Empty(); }
	SocketServer *	GetServer(void)	{ return myServer; }
	SocketBase *	GetSocket(void)	{ return sock; };
	SocketBase *	InitConnection(bool wait = true);
	bool	InitInProcess(void);
	void	OnInit(void);
	void	OnExecute(void);
	void	OnGet(void);
	bool	Ok(void)	{ return ok; }
	void	OnGetFiles(void);
	void	OnGetPar(void);
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

void	EmptyDiagBuffer_IPCServer(WChar *s, int *c);

IPCServer *	GetPtr_IPCServer(void);

void	Notify_IPCServer(const WChar *message, CommonDiagSpecifier type);

/******************************************************************************/
/*************************** Call back prototypes *****************************/
/******************************************************************************/

#endif

