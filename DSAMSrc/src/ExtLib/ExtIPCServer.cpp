/**********************
 *
 * File:		ExtIPCServer.cpp
 * Purpose:		Inter-process communication server extension code module.
 * Comments:	This code module was revised from the GrSimMgr.cpp code module.
 * Author:		L. P. O'Mard
 * Created:		06 Nov 2003
 * Updated:		
 * Copyright:	(c) 2003, University of Essex
 *
 **********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>

#ifndef WX_PRECOMP
#	include <wx/wx.h>
#endif

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
#include "UtDatum.h"
#include "UtSSSymbols.h"
#include "UtSimScript.h"
#include "UtAppInterface.h"
#include "UtUIEEEFloat.h"
#include "UtUPortableIO.h"
#include "FiDataFile.h"
#include "FiAIFF.h"
#include "UtSSParser.h"
#include "ExtIPCServer.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

IPCServer	*iPCServer = NULL;

/******************************************************************************/
/****************************** Methods (Subroutines) *************************/
/******************************************************************************/

/****************************** Constructor ***********************************/

IPCServer::IPCServer(const wxString& hostName, uShort theServicePort)
{
	static const char *funcName = "IPCServer::IPCServer";
	wxIPV4address	addr;

	iPCServer = this;
	ok = true;
	simulationInitialisedFlag = false;
	sock = NULL;
	inProcess = NULL;
	outProcess = NULL;
	inUIOPtr = NULL;
	outUIOPtr = NULL;
	addr.Hostname((hostName.length())? hostName: "localhost");
	addr.Service((!theServicePort)? EXTIPCSERVER_DEFAULT_SERVER_PORT:
	  theServicePort);

	myServer = new wxSocketServer(addr);
	if (!myServer->Ok()) {
		NotifyError("%s: Could not listen at specified port.", funcName);
		ok = FALSE;
	}
	SetDPrintFunc(DPrint_IPCServer);
	SetNotifyFunc(Notify_IPCServer);

}

/****************************** Destructor ************************************/

/*
 * The EarObjects are free'd by the main controlling routine.
 */

IPCServer::~IPCServer(void)
{
	if (inUIOPtr)
		FreeMemory_UPortableIO(&inUIOPtr);
	if (outUIOPtr)
		FreeMemory_UPortableIO(&outUIOPtr);
	if (myServer)
		myServer->Destroy();
	iPCServer = NULL;
	
}

/******************************************************************************/
/****************************** IPCServer Methods *****************************/
/******************************************************************************/

/****************************** CommandList ***********************************/

NameSpecifier *
IPCServer::CommandList(int index)
{
	static NameSpecifier	modeList[] = {

					{ "QUIT",		IPCSERVER_COMMAND_QUIT},
					{ "INIT",		IPCSERVER_COMMAND_INIT},
					{ "RUN",		IPCSERVER_COMMAND_RUN},
					{ "SET",		IPCSERVER_COMMAND_SET},
					{ "GET",		IPCSERVER_COMMAND_GET},
					{ "GETFILES",	IPCSERVER_COMMAND_GETFILES},
					{ "PUT",		IPCSERVER_COMMAND_PUT},
					{ "ERRMSGS",	IPCSERVER_COMMAND_ERRMSGS},
					{ "",		IPCSERVER_COMMAND_NULL},
				
				};
	return (&modeList[index]);

}

/****************************** InitConnection ********************************/

/*
 * This function initialised the connection and returns a pointer to the
 * socket.
 */

wxSocketBase *
IPCServer::InitConnection(void)
{
	static const char *funcName = "IPCServer::OnServerEvent";
	wxString	salutation;
	wxIPV4address	addr;

	sock = myServer->Accept(FALSE);
	if (!sock) {
		NotifyError("%s: Couldn't accept a new connection.\n", funcName);
		return(NULL);
	}
	myServer->GetLocal(addr);
	salutation.Printf("Host %s running %s, DSAM version %s.\n", addr.Hostname(
	  ).c_str(), EXTIPCSERVER_DEFAULT_SERVER_NAME, DSAM_VERSION);
	sock->Write(salutation.GetData(), salutation.length());
	return(sock);


}

/****************************** InitInProcess *********************************/

/*
 */

bool
IPCServer::InitInProcess(void)
{
	static const char *funcName = "IPCServer::InitInProcess";

	if (!inProcess && ((inProcess = Init_EarObject("DataFile_In")) == NULL)) {
		NotifyError("%s: Cannot initialised input process EarObject.", 
		  funcName);
		return(false);
	}
	SetPar_ModuleMgr(inProcess, "filename", EXTIPCSERVER_MEMORY_FILE_NAME);
	((DataFilePtr) inProcess->module->parsPtr)->uIOPtr = inUIOPtr;
	ConnectOutSignalToIn_EarObject(inProcess, GetSimProcess_AppInterface());
	return(true);

}

/****************************** InitOutProcess *********************************/

/*
 */

bool
IPCServer::InitOutProcess(void)
{
	static const char *funcName = "IPCServer::InitOutProcess";

	if (!outProcess && ((outProcess = Init_EarObject("DataFile_Out")) ==
	  NULL)) {
		NotifyError("%s: Cannot initialised input process EarObject.", 
		  funcName);
		return(false);
	}
	SetPar_ModuleMgr(outProcess, "filename", EXTIPCSERVER_MEMORY_FILE_NAME);
	((DataFilePtr) outProcess->module->parsPtr)->uIOPtr = inUIOPtr;
	ConnectOutSignalToIn_EarObject(GetSimProcess_AppInterface(), outProcess);
	return(true);

}

/****************************** ResetInProcess ********************************/

/*
 * This routine resets the input process ready for the new simulation.
 */

void
IPCServer::ResetInProcess(void)
{
	if (!inProcess)
		return;
		
	DisconnectOutSignalFromIn_EarObject(inProcess, GetSimProcess_AppInterface(
	  ));
	Free_EarObject(&inProcess);

}

/****************************** ResetOutProcess *******************************/

/*
 * This routine resets the output process ready for the new simulation.
 */

void
IPCServer::ResetOutProcess(void)
{
	if (!outProcess)
		return;
		
	DisconnectOutSignalFromIn_EarObject(GetSimProcess_AppInterface(),
	  outProcess);
	Free_EarObject(&outProcess);

}

/****************************** RunInProcess **********************************/

/*
 * This function runs the input process.  It assumes that the DataFile
 * process is GUI safe, and so temporarily unsets the 'usingGUIFlag'.
 * This avoids an attempted call to the "TestDestroy_SimThread" routine.
 */

bool
IPCServer::RunInProcess(void)
{
	static const char *funcName = "IPCServer::RunInProcess";
	bool	ok = true;
	BOOLN	oldUsingGUIFlag = GetDSAMPtr_Common()->usingGUIFlag;

	ResetProcess_EarObject(inProcess);
	GetDSAMPtr_Common()->usingGUIFlag = FALSE;
	if (!RunProcess_ModuleMgr(inProcess)) {
		NotifyError("%s: Could not run input process.", funcName);
		ok = false;
	}
	GetDSAMPtr_Common()->usingGUIFlag = oldUsingGUIFlag;
	return(ok);

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
IPCServer::RunOutProcess(void)
{
	static const char *funcName = "IPCServer::RunOutProcess";
	bool	ok = true;
	BOOLN	oldUsingGUIFlag = GetDSAMPtr_Common()->usingGUIFlag;
	BOOLN	oldSegmentedMode = GetDSAMPtr_Common()->segmentedMode;

	if (!InitMemory_UPortableIO(&outUIOPtr, GetFileSize_AIFF(
	  GetSimProcess_AppInterface()->outSignal, 0L))) {
		NotifyError("%s: Could not prepare output data buffer.", funcName);
		return(false);
	}
	((DataFilePtr) outProcess->module->parsPtr)->uIOPtr = outUIOPtr;
	GetDSAMPtr_Common()->segmentedMode = FALSE;
	GetDSAMPtr_Common()->usingGUIFlag = FALSE;
	if (!RunProcess_ModuleMgr(outProcess)) {
		NotifyError("%s: Could not run output process.", funcName);
		ok = false;
	}
	GetDSAMPtr_Common()->segmentedMode = oldSegmentedMode;
	GetDSAMPtr_Common()->usingGUIFlag = oldUsingGUIFlag;
	return(ok);

}

/****************************** InitSimulation ********************************/

/*
 */

void
IPCServer::InitSimulation(wxSocketBase *sock)
{
	static const char *funcName = "IPCServer::InitSimulation";
	unsigned char c;

	ResetInProcess();
	ResetOutProcess();
	wxString tempFileName = wxFileName::CreateTempFileName("simFile");
	wxRemoveFile(tempFileName);
	tempFileName.append(".spf");
	wxFFileOutputStream	*oStream = new wxFFileOutputStream(tempFileName);
	if (!oStream->Ok()) {
		NotifyError("%s: Could not open temporary file '%s'.", funcName,
		  tempFileName.c_str());
		return;
	}
	while (!sock->Read(&c, 1).Error() && (c != (unsigned char) EOF))
		oStream->PutC(c);
	delete oStream;
	LoadSimFile(tempFileName);
	wxRemoveFile(tempFileName);
	if (inUIOPtr)
		InitInProcess();
	InitOutProcess();

}

/****************************** OnPut *****************************************/

/*
 */

void
IPCServer::OnPut(wxSocketBase *sock)
{
	static const char *funcName = "IPCServer::OnPut";
	size_t	length;
	UPortableIOPtr	oldUIOPtr = uPortableIOPtr;

	sock->SetFlags(wxSOCKET_WAITALL);
 	sock->Read(&length, sizeof(length));
	uPortableIOPtr = inUIOPtr;
	if (!InitMemory_UPortableIO(&inUIOPtr, length)) {
		NotifyError("%s: Could not initialise memory for input process signal",
		  funcName);
		return;
	}
	sock->Read(inUIOPtr->memStart, length);
	uPortableIOPtr = oldUIOPtr;
	if (simulationInitialisedFlag)
		InitInProcess();

}

/****************************** OnGet *****************************************/

/*
 */

void
IPCServer::OnGet(wxSocketBase *sock)
{
	UPortableIOPtr	uIOPtr;
	ChanLen	length = 0;

	if (!GetPtr_AppInterface()->simulationFinishedFlag) {
		sock->Write(&length, sizeof(length));
		return;
	}
	RunOutProcess();
	uIOPtr = ((DataFilePtr) outProcess->module->parsPtr)->uIOPtr;
	sock->SetFlags(wxSOCKET_WAITALL);
 	sock->Write(&uIOPtr->length, sizeof(uIOPtr->length));
	sock->Write(uIOPtr->memStart, uIOPtr->length);

}

/****************************** BuildFileList *********************************/

/*
 */

void
IPCServer::BuildFileList(wxArrayString &list, DatumPtr pc)
{
	while (pc) {
		if (pc->type == PROCESS)
			switch (pc->data->module->specifier) {
			case SIMSCRIPT_MODULE:
				BuildFileList(list, pc);
				break;
			case DATAFILE_MODULE:
				if (((DataFilePtr) pc->data->module->parsPtr)->inputMode)
					break;
				list.Add(GetUniParPtr_ModuleMgr(pc->data, "fileName")->valuePtr.
				  s);
				break;
			default:
				;
			} /* Switch */
		pc = pc->next;
	}
}

/****************************** OnGetFiles ************************************/

/*
 */

void
IPCServer::OnGetFiles(wxSocketBase *sock)
{
	static const char *funcName = "IPCServer::OnGetFiles";
	wxUint8	byte;
	size_t	i, j, length;
	wxArrayString	list;
	unsigned char	numFiles = 0;

	if (!GetPtr_AppInterface()->simulationFinishedFlag) {
 		sock->Write(&numFiles, sizeof(numFiles));
		return;
	}
	BuildFileList(list, GetSimulation_AppInterface());
	numFiles = list.Count();
	if (!numFiles) {
		NotifyError("%s: No output files to transfer.\n", funcName);
		return;
	}
	sock->Write(&numFiles, 1);
	for (i = 0; i < numFiles; i++) {
		sock->Write(list[i], list[i].length());
		sock->Write("\n", 1);
		wxFFileInputStream inStream(GetParsFileFPath_Common((char *) list[i].
		  c_str()));
		if (!inStream.Ok()) {
			NotifyError("%s: Could not open '%s' for transfer.", funcName,
			  list[i].c_str());
			return;
		}
		wxDataInputStream	data(inStream);

		length = inStream.GetSize();
		sock->Write(&length, sizeof(length));
		for (j = 0; j < length; j++) {
			byte = data.Read8();
			sock->Write(&byte, 1);
		}
	}
	/*** Don't forget to delete files after. ***/
}

/****************************** SetParameters *********************************/

/*
 */

void
IPCServer::SetParameters(wxSocketBase *sock)
{
	static const char *funcName = "IPCServer::SetParameters";
	int		i, numTokens;
	unsigned char c;
	wxString	cmdStr, parameter, value;

	while (!sock->Read(&c, 1).Error() && (c != '\n'))
		if (c != '\r')
			cmdStr += c;
	wxStringTokenizer tokenizer(cmdStr);
	if ((numTokens = tokenizer.CountTokens()) % 2 != 0) {
		NotifyError("%s: parameter settings must be in <name> <value> pairs.",
		  funcName);
		return;
	}
	for (i = 0; i < numTokens / 2; i++) {
		parameter = tokenizer.GetNextToken().c_str();
		value = tokenizer.GetNextToken().c_str();
		if (!SetProgramParValue_AppInterface((char *) parameter.c_str(),
		  (char *) value.c_str(), FALSE) && !SetUniParValue_Utility_Datum(
		  GetSimulation_AppInterface(), (char *) parameter.c_str(),
		  (char *) value.c_str())) {
			NotifyError("%s: Could not set '%s' parameter to '%s'.", funcName,
			 (char *)  parameter.c_str(), (char *) value.c_str());
			return;
		}
	}

}

/****************************** ProcessInput **********************************/

/*
 * This routine processes the socket input.
 * The call to 'wxFileName::CreateTempFileName' creates the temporary file:
 * I could find no way to stop this, so I have to remove the file before adding
 * the extension and re-creating it.
 */

void
IPCServer::ProcessInput(wxSocketBase *sock)
{
	static const char *funcName = "IPCServer::ProcessInput";
	unsigned char c;

	sock->SetNotify(wxSOCKET_LOST_FLAG);
	buffer.Clear();
	while(!sock->Read(&c, 1).Error() && (c != '\n'))
		if (c != '\r')
			buffer += c;
	switch (Identify_NameSpecifier((char *) buffer.c_str(), CommandList(0))) {
	case IPCSERVER_COMMAND_QUIT:
		OnExit_AppInterface();
		break;
	case IPCSERVER_COMMAND_INIT:
		InitSimulation(sock);
		break;
	case IPCSERVER_COMMAND_RUN:
		if (inProcess)
			RunInProcess();
		if (!OnExecute_AppInterface())
			break;
		break;
	case IPCSERVER_COMMAND_SET:
		SetParameters(sock);
		break;
	case IPCSERVER_COMMAND_GET:
		OnGet(sock);
		break;
	case IPCSERVER_COMMAND_GETFILES:
		OnGetFiles(sock);
		break;
	case IPCSERVER_COMMAND_PUT:
		OnPut(sock);
		break; 
	case IPCSERVER_COMMAND_ERRMSGS:
		PrintNotificationList();
		break;
	default:
		NotifyError("%s: Unknown command given (%s).", funcName, buffer.
		  c_str());
	}
	sock->SetNotify(wxSOCKET_LOST_FLAG | wxSOCKET_INPUT_FLAG);

}

/****************************** PrintNotificationList *************************/

/*
 * This routine prints the notification list.  If the index is given then
 * only the specified notication is printed.  A negative index assumes that
 * all notifications should be printed.
 */

void
IPCServer::PrintNotificationList(int index)
{
	unsigned char numNotifications;

	if (!notificationList.GetCount() || ((index >= 0) && ((notificationList.
	  GetCount() < (size_t) index)))) {
		numNotifications = 0;
		sock->Write(&numNotifications, 1);
		return;
	}
	if (index < 0) {
		numNotifications = notificationList.GetCount();
		sock->Write(&numNotifications, 1);
		for (unsigned int i = 0; i < notificationList.GetCount(); i++)
			sock->Write(notificationList[i], notificationList[i].length());
	} else {
		numNotifications = 1;
		sock->Write(&numNotifications, 1);
		sock->Write(notificationList[index], notificationList[index].length());
	}
		
}

/****************************** LoadSimFile ***********************************/

/*
 * This routine loads the simulation file.  This is a virtual function and
 * classes that call this routine should create their on instance of this
 * this function.
 */

void
IPCServer::LoadSimFile(const wxString& fileName)
{
	printf("IPCServer::LoadSimFile: Does nothing\n");

}

/******************************************************************************/
/****************************** Callbacks *************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Functions *************************************/
/******************************************************************************/

/*************************** GetPtr_IPCServer *********************************/

/*
 */

IPCServer *
GetPtr_IPCServer(void)
{
	static const char *funcName = "GetPtr_IPCServer";

	if (!iPCServer) {
		NotifyError("%s: Server not initialised.\n", funcName);
		return(NULL);
	}
	return(iPCServer);

}

/*************************** EmptyDiagWinBuffer *******************************/

/*
 * This routine prints the print buffer, then resets its counter to zero.
 * It expects the count 'c' to point beyond the last character.
 * It expects the socket to have been correctly initialised.
 */

void
EmptyDiagBuffer_IPCServer(char *s, int *c)
{
	*(s + *c) = '\0';
	GetPtr_IPCServer()->GetSocket()->Write(s, *c);
	*c = 0;

}

/*************************** DPrint *******************************************/

/*
 * This routine prints out a diagnostic message, preceded by a bell sound.
 * It is used in the same way as the printf statement.
 * There are different versions for the different compile options.
 * It assumes that all real values are 'double'.
 * It does not check that the format string is less than SMALL_STRING
 * characters.
 * The 'buffer' string is used to retain control of the formatting, as WxWin
 * does not yet have sufficient controls on the number output using the '<<'
 * operator.
 */
 
void
DPrint_IPCServer(char *format, va_list args)
{
	if (!GetPtr_IPCServer()->GetSocket())
		return;
	DPrintBuffer_Common(format, args, EmptyDiagBuffer_IPCServer);
	
}

/***************************** Notify *****************************************/

/*
 * All notification messages are stored in the notification list.
 * This list is reset when the noficiationCount is zero.
 */

void
Notify_IPCServer(const char *format, va_list args, CommonDiagSpecifier type)
{
	wxString	message;

	if (!GetDSAMPtr_Common()->notificationCount)
		GetPtr_IPCServer()->ClearNotifications();
	message.PrintfV(format, args);
	message.insert(0, ": ");
	message.insert(0, DiagnosticTitle(type));
	message.append("\n");
	GetPtr_IPCServer()->AddNotification(message);

} /* NotifyMessage */
