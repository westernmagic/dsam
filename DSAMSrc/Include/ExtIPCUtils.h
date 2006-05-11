/**********************
 *
 * File:		ExtIPCUtils.h
 * Purpose:		Inter-process communication server extension utility code
 *				module.
 * Comments:	This code module was revised from the ExtIPCServer.cpp code
 *				module.
 * Author:		L. P. O'Mard
 * Created:		09 Jan 2004
 * Updated:		
 * Copyright:	(c) 2004, University of Essex
 *
 **********************/

#ifndef _EXTIPCUTILS_H
#define _EXTIPCUTILS_H 1

#if defined(GRAPHICS_SUPPORT) && defined(__cplusplus)
#	include <wx/socket.h>
#endif

#include "UtNameSpecs.h"
#include "UtUIEEEFloat.h"
#include "UtUPortableIO.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define EXTIPCUTILS_MAX_COMMAND_CHAR		10
#define EXTIPCUTILS_MEMORY_FILE_NAME		wxT("+.aif")
#define EXTIPCUTILS_DEFAULT_SERVER_PORT		3300


#ifndef HEADER_ONLY /* This allows for the simple extraction of the defines. */

/******************************************************************************/
/*************************** Enum definitions *********************************/
/******************************************************************************/

enum {

	IPC_STATUS_READY = 0,
	IPC_STATUS_BUSY,
	IPC_STATUS_ERROR

};

enum {

	IPC_APP_SOCKET_ID = 1,
	IPC_APP_SERVER_ID

};

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	IPC_COMMAND_QUIT,

	IPC_COMMAND_ERRMSGS,
	IPC_COMMAND_GET,
	IPC_COMMAND_GETFILES,
	IPC_COMMAND_INIT,
	IPC_COMMAND_PUT,
	IPC_COMMAND_PUT_ARGS,
	IPC_COMMAND_RUN,
	IPC_COMMAND_SET,
	IPC_COMMAND_STATUS,

	IPC_COMMAND_NULL

} IPCCommandSpecifier;

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/********************************** Pre-references ****************************/

/*************************** IPCUtils *****************************************/

class IPCUtils {

	bool	inProcessConnectedFlag, outProcessConnectedFlag;
	EarObjectPtr	outProcess, inProcess;
	EarObjectPtr	inProcessCustomer, outProcessSupplier;
	UPortableIOPtr	inUIOPtr, outUIOPtr;

  public:

	IPCUtils(void);
	~IPCUtils(void);

	NameSpecifier *	CommandList(int index);
	void	ConnectToOutProcess(EarObjectPtr supplierProcess);
	void	ConnectToInProcess(EarObjectPtr customerProcess);
	void	DisconnectInProcess(void);
	void	DisconnectOutProcess(void);
	EarObjectPtr	GetInProcess(void)	{ return inProcess; }
	bool	InProcessConnected(void)	{ return inProcessConnectedFlag; }
	EarObjectPtr	GetOutProcess(void)	{ return outProcess; }
	UPortableIOPtr	GetInUIOPtr(void)	{ return inUIOPtr; }
	UPortableIOPtr	GetOutUIOPtr(void)	{ return outUIOPtr; }
	bool	InitInputMemory(ChanLen length);
	bool	InitInProcess(void);
	bool	InitOutProcess(void);
	void	ResetInProcess(void);
	void	ResetOutProcess(void);
	bool	RunOutProcess(void);
	bool	RunInProcess(void);

};

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

NameSpecifier *	CommandList_IPCUtils(int index);
EarObjectPtr	InitOutProcess_IPCUtils(UPortableIOPtr uIOPtr,
				  EarObjectPtr simProcess);

/******************************************************************************/
/*************************** Call back prototypes *****************************/
/******************************************************************************/

#endif /* HEADER_ONLY */
#endif

