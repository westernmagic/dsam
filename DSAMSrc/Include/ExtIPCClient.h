/**********************
 *
 * File:		ExtIPCClient.h
 * Purpose:		Inter-process communication client extension code module.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		09 Jan 2004
 * Updated:		
 * Copyright:	(c) 2004, University of Essex
 *
 **********************/

#ifndef _EXTIPCCLIENT_H
#define _EXTIPCCLIENT_H 1

#if defined(GRAPHICS_SUPPORT) && defined(__cplusplus)
#	include <wx/socket.h>
#endif

#include "GeCommon.h"

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

class wxFileName;

/*************************** IPCClient ****************************************/

class IPCClient: public wxSocketClient {

	bool	ok;
	IPCUtils	iPCUtils;
	wxString	buffer;

  public:
	IPCClient(const wxString& hostName, uShort theServicePort);
	virtual	~IPCClient(void);

	unsigned char	CheckStatus(void);
	bool	CreateSPFFromSimScript(wxFileName &fileName);
	bool	Errors(void);
	bool	GetAllOutputFiles(void);
	EarObjectPtr	GetSimProcess(void);
	IPCUtils * GetIPCUtils(void)	{ return &iPCUtils; }
	bool	InitSimFromFile(const wxString &simFileName);
	bool	InitSimulation(const wxString& simulation);
	bool	Ok(void)	{ return ok; }
	bool	ReadString(wxString &s);
	bool	RunSimulation(void);
	bool	SendArguments(int argc, wxChar **argv);
	bool	SendCommand(IPCCommandSpecifier command);
	bool	SendInputProcess(void);
	void	WaitForReady(void)	{ while (CheckStatus() == IPC_STATUS_BUSY); }
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

