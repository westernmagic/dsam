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
#include "UtDatum.h"
#include "UtSSSymbols.h"
#include "UtSimScript.h"
#include "UtAppInterface.h"
#include "UtUIEEEFloat.h"
#include "UtUPortableIO.h"
#include "FiDataFile.h"
#include "UtSSParser.h"
#include "ExtMainApp.h"
#include "ExtIPCUtils.h"
#include "ExtSocket.h"
#include "ExtSocketServer.h"
#include "ExtIPCServer.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

IPCServer	*iPCServer = NULL;

/******************************************************************************/
/****************************** Methods (Subroutines) *************************/
/******************************************************************************/

/****************************** Constructor ***********************************/

/*
 * The superServerFlag argument indicates that the server is to be started by
 * inetd/xinetd.
 */

IPCServer::IPCServer(const wxString& hostName, uShort theServicePort,
  bool superServerFlag)
{
	static const char *funcName = "IPCServer::IPCServer";
	wxIPV4address	addr;

	iPCServer = this;
	ok = true;
	simulationInitialisedFlag = false;
	successfulRunFlag = false;
	sock = NULL;

	if (!hostName.IsEmpty())
		addr.Hostname(hostName);
	addr.Service((!theServicePort)? EXTIPCUTILS_DEFAULT_SERVER_PORT:
	  theServicePort);
	myServer = new SocketServer(addr, superServerFlag, wxSOCKET_NONE);
	if (!myServer->Ok()) {
		NotifyError("%s: Could not listen at port %u.", funcName,
		  theServicePort);
		ok = FALSE;
	}
	SetNotifyFunc(Notify_IPCServer);

}

/****************************** Destructor ************************************/

/*
 * The EarObjects are free'd by the main controlling routine.
 */

IPCServer::~IPCServer(void)
{
	if (myServer)
		myServer->Destroy();
	iPCServer = NULL;
	
}

/******************************************************************************/
/****************************** IPCServer Methods *****************************/
/******************************************************************************/

/****************************** InitConnection ********************************/

/*
 * This function initialised the connection and returns a pointer to the
 * socket.
 */

SocketBase *
IPCServer::InitConnection(bool wait)
{
	static const char *funcName = "IPCServer::OnServerEvent";
	wxString	salutation;
	wxIPV4address	addr;

	sock = myServer->Accept(wait);
	if (!sock) {
		NotifyError("%s: Couldn't accept a new connection.\n", funcName);
		return(NULL);
	}
	sock->SetFlags(wxSOCKET_WAITALL);
	myServer->GetLocal(addr);
	salutation.Printf("Host %s running %s, DSAM version %s.\n", addr.Hostname(
	  ).c_str(), EXTIPC_DEFAULT_SERVER_NAME, DSAM_VERSION);
	sock->Write(salutation.GetData(), salutation.length());
	return(sock);


}

/****************************** OnInit ****************************************/

/*
 */

void
IPCServer::OnInit(void)
{
	static const char *funcName = "IPCServer::OnInit";
	unsigned char c;

	if (!GetPtr_AppInterface()->simulationFinishedFlag) {
		return;
	}
	iPCUtils.ResetInProcess();
	iPCUtils.ResetOutProcess();
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
	iPCUtils.InitOutProcess();
	iPCUtils.ConnectToOutProcess(GetSimProcess_AppInterface());
	simulationInitialisedFlag = true;

}

/****************************** OnPut *****************************************/

/*
 * Reads signal from client.
 */

void
IPCServer::OnPut(void)
{
	static const char *funcName = "IPCServer::OnPut";
	char	*p;
	wxUint32	i, length;

 	sock->Read(&length, sizeof(length));
	if (!iPCUtils.InitInputMemory(length)) {
		NotifyError("%s: Could not initialise memory for input process signal",
		  funcName);
		return;
	}
	p = iPCUtils.GetInUIOPtr()->memStart;
	for (i = 0; i < length; i++)
		sock->Read(p++, 1);
	
	if (!iPCUtils.InitInProcess()) {
		NotifyError("%s: Could not initialise the input process.", funcName);
		return;
	}

}

/****************************** OnPutArgs *************************************/

/*
 */

void
IPCServer::OnPutArgs(void)
{
	bool	ok = true;
	unsigned char	c;
	wxUint8	i, argc;
	wxString	s;

 	sock->Read(&argc, sizeof(argc));
	ok = dSAMMainApp->InitArgv(argc);
	for (i = 0; i < argc; i++) {
		s.Clear();
		while (!sock->Read(&c, 1).Error() && (c != '\0'))
			s += c;
		s += '\0';
		dSAMMainApp->SetArgvString(i, (char *) s.c_str(), s.length());
	}
	ResetCommandArgFlags_AppInterface();

}

/****************************** OnGet *****************************************/

/*
 */

void
IPCServer::OnGet(void)
{
	wxUint32	length = 0;

	if (!GetPtr_AppInterface()->simulationFinishedFlag ||
	  !GetSimProcess_AppInterface()->outSignal) {
		sock->Write(&length, sizeof(length));
		return;
	}
	iPCUtils.RunOutProcess();
	length = (wxUint32) iPCUtils.GetOutUIOPtr()->length; // Must correct this
 	sock->Write(&length, sizeof(length));
	sock->Write(iPCUtils.GetOutUIOPtr()->memStart, iPCUtils.GetOutUIOPtr()->
	  length);

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
IPCServer::OnGetFiles(void)
{
	static const char *funcName = "IPCServer::OnGetFiles";
	wxUint8	byte, numFiles = 0;
	wxUint32	i, j, length;
	wxArrayString	list;
	wxString	nameOnly;
	wxFileName	fileName;

	if (!GetPtr_AppInterface()->simulationFinishedFlag || !successfulRunFlag) {
 		sock->Write(&numFiles, sizeof(numFiles));
		return;
	}
	BuildFileList(list, GetSimulation_AppInterface());
	numFiles = list.Count();
	sock->Write(&numFiles, 1);
	for (i = 0; i < numFiles; i++) {
		fileName = GetParsFileFPath_Common((char *) list[i].c_str());
		nameOnly = fileName.GetFullName();
		sock->Write(nameOnly, nameOnly.length());
		sock->Write("\n", 1);
		wxFFileInputStream inStream(fileName.GetFullPath());
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
		if (!wxRemoveFile(fileName.GetFullPath()))
			NotifyError("%s: Could not remove file '%s' from server.", funcName,
			  fileName.GetFullPath().c_str());
	}

}

/****************************** OnSet *****************************************/

/*
 */

void
IPCServer::OnSet(void)
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

/****************************** OnExecute *************************************/

/*
 */

void
IPCServer::OnExecute(void)
{
	static const char *funcName = "IPCServer::OnExecute";

	if (!GetPtr_AppInterface()->simulationFinishedFlag) {
		return;
	}
	if (!simulationInitialisedFlag) {
		NotifyError("%s: Simulation not initialised.", funcName);
		return;
	}
	if (iPCUtils.GetInProcess() && !iPCUtils.InProcessConnected())
		iPCUtils.ConnectToInProcess(GetSimProcess_AppInterface());

	iPCUtils.RunInProcess();
	successfulRunFlag = CXX_BOOL(OnExecute_AppInterface());

}

/****************************** OnStatus **************************************/

/*
 */

void
IPCServer::OnStatus(void)
{
	unsigned char status = IPC_STATUS_READY;


	if (!GetPtr_AppInterface()->simulationFinishedFlag) {
		status = IPC_STATUS_BUSY;
		sock->Write(&status, 1);
		return;
	}
	if (notificationList.GetCount() > 0) {
		status = IPC_STATUS_ERROR;
		sock->Write(&status, 1);
		return;
	}
	sock->Write(&status, 1);
	
}

/****************************** ProcessInput **********************************/

/*
 * This routine processes the socket input.
 * The call to 'wxFileName::CreateTempFileName' creates the temporary file:
 * I could find no way to stop this, so I have to remove the file before adding
 * the extension and re-creating it.
 */

bool
IPCServer::ProcessInput(void)
{
	static const char *funcName = "IPCServer::ProcessInput";
	bool endProcessing;
	unsigned char c;

	sock->SetNotify(wxSOCKET_LOST_FLAG);
	buffer.Clear();
	while (((endProcessing = sock->Read(&c, 1).Error()) == false) && (c !=
	  '\n'))
		if (c != '\r')
			buffer += c;
	switch (Identify_NameSpecifier((char *) buffer.c_str(), iPCUtils.
	  CommandList(0))) {
	case IPC_COMMAND_QUIT:
		endProcessing = true;
		OnExit_AppInterface();
		break;
	case IPC_COMMAND_ERRMSGS:
		OnErrMsgs();
		notificationList.Clear();
		break;
	case IPC_COMMAND_GET:
		OnGet();
		break;
	case IPC_COMMAND_GETFILES:
		OnGetFiles();
		break;
	case IPC_COMMAND_INIT:
		OnInit();
		break;
	case IPC_COMMAND_PUT:
		OnPut();
		break; 
	case IPC_COMMAND_PUT_ARGS:
		OnPutArgs();
		break; 
	case IPC_COMMAND_RUN:
		OnExecute();
		break;
	case IPC_COMMAND_SET:
		OnSet();
		break;
	case IPC_COMMAND_STATUS:
		OnStatus();
		break;
	default:
		if (buffer.Length())
			NotifyError("%s: Unknown command given (%s).", funcName, buffer.
			  c_str());
	}
	sock->SetNotify(wxSOCKET_LOST_FLAG | wxSOCKET_INPUT_FLAG);
	return(!endProcessing);

}

/****************************** OnErrMsgs *************************************/

/*
 * This routine prints the notification list.  If the index is given then
 * only the specified notication is printed.  A negative index assumes that
 * all notifications should be printed.
 */

void
IPCServer::OnErrMsgs(int index)
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
	FreeSim_AppInterface();
	if (!SetParValue_UniParMgr(&GetPtr_AppInterface()->parList,
	  APP_INT_SIMULATIONFILE, (char *) fileName.c_str()))
		return;
	dSAMMainApp->ResetSimulation();

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

