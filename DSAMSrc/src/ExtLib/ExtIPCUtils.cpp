/**********************
 *
 * File:		ExtIPCUtils.cpp
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

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>

#include <ExtCommon.h>

#include <wx/socket.h>
#include <wx/filename.h>
#include "wx/datstrm.h"
#include <wx/wfstream.h>
#include <wx/protocol/protocol.h>
#include <wx/tokenzr.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "UtUIEEEFloat.h"
#include "UtUPortableIO.h"
#include "FiDataFile.h"
#include "FiAIFF.h"
#include "ExtIPCUtils.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Methods (Subroutines) *************************/
/******************************************************************************/

/****************************** Constructor ***********************************/

IPCUtils::IPCUtils(void)
{
	inUIOPtr = NULL;
	outUIOPtr = NULL;
	outProcess = NULL;
	inProcess = NULL;
	inProcessCustomer = NULL;
	outProcessSupplier = NULL;
	inProcessConnectedFlag = false;
	outProcessConnectedFlag = false;

}

/****************************** Destructor ************************************/

IPCUtils::~IPCUtils(void)
{
	if (inUIOPtr)
		FreeMemory_UPortableIO(&inUIOPtr);
	if (outUIOPtr)
		FreeMemory_UPortableIO(&outUIOPtr);
	ResetOutProcess();
	ResetInProcess();

}

/****************************** CommandList ***********************************/

NameSpecifier *
IPCUtils::CommandList(int index)
{
	static NameSpecifier	modeList[] = {

					{ wxT("QUIT"),		IPC_COMMAND_QUIT},
					{ wxT("ERRMSGS"),	IPC_COMMAND_ERRMSGS},
					{ wxT("GET"),		IPC_COMMAND_GET},
					{ wxT("GETFILES"),	IPC_COMMAND_GETFILES},
					{ wxT("GETPAR"),	IPC_COMMAND_GETPAR},
					{ wxT("INIT"),		IPC_COMMAND_INIT},
					{ wxT("PUT"),		IPC_COMMAND_PUT},
					{ wxT("PUT_ARGS"),	IPC_COMMAND_PUT_ARGS},
					{ wxT("RUN"),		IPC_COMMAND_RUN},
					{ wxT("SET"),		IPC_COMMAND_SET},
					{ wxT("STATUS"),	IPC_COMMAND_STATUS},
					{ wxT(""),			IPC_COMMAND_NULL},
				
				};
	return (&modeList[index]);

}

/****************************** InitOutProcess ********************************/

/*
 */

bool
IPCUtils::InitOutProcess(void)
{
	static const wxChar *funcName = wxT("IPCServer::InitOutProcess");

	if (!outProcess && ((outProcess = Init_EarObject(wxT("DataFile_Out"))) ==
	  NULL)) {
		NotifyError(wxT("%s: Cannot initialised input process EarObject."), 
		  funcName);
		return(false);
	}
	SetPar_ModuleMgr(outProcess, wxT("filename"), EXTIPCUTILS_MEMORY_FILE_NAME);
	((DataFilePtr) outProcess->module->parsPtr)->uIOPtr = inUIOPtr;
	return(true);

}

/****************************** InitInProcess *********************************/

/*
 */

bool
IPCUtils::InitInProcess(void)
{
	static const wxChar *funcName = wxT("IPCUtils::InitInProcess");

	if (!inProcess && ((inProcess = Init_EarObject(wxT("DataFile_In"))) ==
	  NULL)) {
		NotifyError(wxT("%s: Cannot initialised input process EarObject."), 
		  funcName);
		return(false);
	}
	SetPar_ModuleMgr(inProcess, wxT("filename"), EXTIPCUTILS_MEMORY_FILE_NAME);
	((DataFilePtr) inProcess->module->parsPtr)->uIOPtr = inUIOPtr;
	return(true);

}

/****************************** ConnectToOutProcess ***************************/

/*
 */

void
IPCUtils::ConnectToOutProcess(EarObjectPtr supplierProcess)
{
	outProcessSupplier = supplierProcess;
	ConnectOutSignalToIn_EarObject(outProcessSupplier, outProcess);
	outProcessConnectedFlag = true;

}

/****************************** ConnectToInProcess ****************************/

/*
 */

void
IPCUtils::ConnectToInProcess(EarObjectPtr customerProcess)
{
	inProcessCustomer = customerProcess;
	ConnectOutSignalToIn_EarObject(inProcess, inProcessCustomer);
	inProcessConnectedFlag = true;

}

/****************************** DisconnectOutProcess **************************/

/*
 */

void
IPCUtils::DisconnectOutProcess(void)
{
	if (!outProcessConnectedFlag)
		return;
	DisconnectOutSignalFromIn_EarObject(outProcessSupplier, outProcess);
	outProcessConnectedFlag = false;

}

/****************************** DisconnectInProcess ***************************/

/*
 */

void
IPCUtils::DisconnectInProcess(void)
{
	if (!inProcessConnectedFlag)
		return;
	DisconnectOutSignalFromIn_EarObject(inProcess, inProcessCustomer);
	inProcessConnectedFlag = false;

}

/****************************** ResetOutProcess *******************************/

/*
 * This routine resets the output process ready for the new simulation.
 */

void
IPCUtils::ResetOutProcess(void)
{
	if (!outProcess)
		return;
	DisconnectOutProcess();
	Free_EarObject(&outProcess);

}

/****************************** ResetInProcess ********************************/

/*
 * This routine resets the input process ready for the new simulation.
 */

void
IPCUtils::ResetInProcess(void)
{
	if (!inProcess)
		return;
	DisconnectInProcess();
	Free_EarObject(&inProcess);

}

/****************************** RunOutProcess ********************************/

/*
 * This function writes the simulation output to the 'memory file'.
 * This call is set up to ignore segment processing mode, as the memory
 * allocated for the output data does not grow.  Such growth would be in
 * appropriate for socket transfer.  The other side of the socket must sort this
 * out as required.
 */

bool
IPCUtils::RunOutProcess(void)
{
	static const wxChar *funcName = wxT("IPCUtils::RunOutProcess");
	bool	ok = true;
	BOOLN	oldUsingGUIFlag = GetDSAMPtr_Common()->usingGUIFlag;
	BOOLN	oldSegmentedMode = GetDSAMPtr_Common()->segmentedMode;

	if (!InitMemory_UPortableIO(&outUIOPtr, GetFileSize_AIFF(
	  _OutSig_EarObject(outProcessSupplier), 0L))) {
		NotifyError(wxT("%s: Could not prepare output data buffer."), funcName);
		return(false);
	}
	((DataFilePtr) outProcess->module->parsPtr)->uIOPtr = outUIOPtr;
	GetDSAMPtr_Common()->segmentedMode = FALSE;
	GetDSAMPtr_Common()->usingGUIFlag = FALSE;
	if (!RunProcess_ModuleMgr(outProcess)) {
		NotifyError(wxT("%s: Could not run output process."), funcName);
		ok = false;
	}
	GetDSAMPtr_Common()->segmentedMode = oldSegmentedMode;
	GetDSAMPtr_Common()->usingGUIFlag = oldUsingGUIFlag;
	return(ok);

}

/****************************** InitInputMemory *******************************/

/*
 */

bool
IPCUtils::InitInputMemory(ChanLen length)
{
	static const wxChar *funcName = wxT("IPCUtils::InitInputMemory");

	if (!InitMemory_UPortableIO(&inUIOPtr, length)) {
		NotifyError(wxT("%s: Could not initialise memory for input process "
		  "signal"), funcName);
		return(false);
	}
	return(true);

}
/****************************** RunInProcess **********************************/

/*
 * This function runs the input process.  It assumes that the DataFile
 * process is GUI safe, and so temporarily unsets the 'usingGUIFlag'.
 * This avoids an attempted call to the "TestDestroy_SimThread" routine.
 */

bool
IPCUtils::RunInProcess(void)
{
	static const wxChar *funcName = wxT("IPCUtils::RunInProcess");
	bool	ok = true;
	BOOLN	oldUsingGUIFlag = GetDSAMPtr_Common()->usingGUIFlag;

	if (!inProcess)
		return(false);
	ResetProcess_EarObject(inProcess);
	GetDSAMPtr_Common()->usingGUIFlag = FALSE;
	((DataFilePtr) inProcess->module->parsPtr)->uIOPtr = inUIOPtr;
	if (!RunProcess_ModuleMgr(inProcess)) {
		NotifyError(wxT("%s: Could not run input process."), funcName);
		ok = false;
	}
	GetDSAMPtr_Common()->usingGUIFlag = oldUsingGUIFlag;
	return(ok);

}

/******************************************************************************/
/****************************** Methods (Subroutines) *************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Callbacks *************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Functions *************************************/
/******************************************************************************/

