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

#include "wx/socket.h"
#include "wx/sckstrm.h"
#include "wx/protocol/protocol.h"

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "UtDatum.h"
#include "UtSSSymbols.h"
#include "UtSimScript.h"
#include "UtAppInterface.h"
#include "ExtIPCServer.h"
#include "GrSimMgr.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Methods (Subroutines) *************************/
/******************************************************************************/

/****************************** Constructor ***********************************/

IPCServer::IPCServer(const wxString& hostName, uShort theServicePort)
{
	static const char *funcName = "IPCServer::IPCServer";
	char	userId[MAXLINE];
	wxIPV4address	addr;

	printf("IPCServer::IPCServer: Entered\n");
	ok = TRUE;
	sock = NULL;
	handler = NULL;
	mode = IPCSERVER_COMMAND_MODE;
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

IPCServer::~IPCServer(void)
{
	if (myServer)
		myServer->Destroy();
	
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
					{ "PUT",		IPCSERVER_COMMAND_PUT},
					{ "",		IPCSERVER_COMMAND_NULL},
				
				};
	return (&modeList[index]);

}

/****************************** SetNotification *******************************/

/*
 * Sets up the notification for event handling.
 */

void
IPCServer::SetNotification(wxEvtHandler *theHandler)
{
	handler = theHandler;
	myServer->SetEventHandler(*handler, IPCSERVER_APP_SERVER_ID);
	myServer->SetNotify(wxSOCKET_CONNECTION_FLAG);
	myServer->Notify(TRUE);

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
	salutation.Printf("Host %s running %s, DSAM version %s.\n", (char *)
	  addr.Hostname().GetData(), EXTIPCSERVER_DEFAULT_SERVER_NAME,
	  DSAM_VERSION);
	sock->Write(salutation.GetData(), salutation.length());
	return(sock);


}
/****************************** ProcessInput **********************************/

void
IPCServer::ProcessInput(wxSocketBase *sock)
{
	static const char *funcName = "IPCServer::ProcessInput";
	unsigned char c;

	switch (mode) {
	case IPCSERVER_COMMAND_MODE:
		sock->Read((char *) &c, 1);
		if (isalpha(c)) {
			if (buffer.length() == EXTIPCSERVER_MAX_COMMAND_CHAR) {
				NotifyError("%s: Attempt to overrun buffer\n", funcName);
				buffer.Clear();
				return;
			}
			buffer += c;
			break;
		}
		if (!buffer.length())
			break;
		printf("%s: Got string '%s', length %d\n", funcName,
		  (char *) buffer.GetData(), buffer.length());
		printf("%s: Command specifier = %d\n", funcName, Identify_NameSpecifier(
		  (char *) buffer.GetData(), CommandList(0)));
		switch (Identify_NameSpecifier((char *) buffer.GetData(), CommandList(
		  0))) {
		case IPCSERVER_COMMAND_QUIT:
			printf("%s: Got quit command\n", funcName);
			OnExit_AppInterface();
			break;
		case IPCSERVER_COMMAND_INIT: {
			sock->SetNotify(wxSOCKET_LOST_FLAG);
			wxSocketInputStream	stream(*sock);
			while (stream.Read((char *) &c, 1).GetLastError() ==
			  wxSTREAM_NOERROR) {
				printf("%c", c);
			}
			printf("%s: Stream ended.\n", funcName);
			sock->SetNotify(wxSOCKET_LOST_FLAG | wxSOCKET_INPUT);
			break; }
		case IPCSERVER_COMMAND_RUN:
			OnExecute_AppInterface();
			break;
		case IPCSERVER_COMMAND_SET:
			break;
		case IPCSERVER_COMMAND_GET:
			printf("%s: Got GET command\n", funcName);
			break;
		case IPCSERVER_COMMAND_PUT:
			break;
		default:
			NotifyError("%s: Unknown command given (%s).", funcName, (char *)
			  buffer.GetData());
		}
		buffer.Clear();
		break;
	default:
		NotifyError("%s: Mode (%d) not implemented.\n", funcName, mode);
	} /* switch */

}

/******************************************************************************/
/****************************** Callbacks *************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Functions *************************************/
/******************************************************************************/

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
	wxGetApp().GetServerSocket()->Write(s, *c);
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
	if (!wxGetApp().GetServerSocket())
		return;
	DPrintBuffer_Common(format, args, EmptyDiagBuffer_IPCServer);
	
}

/***************************** Notify *****************************************/

/*
 * The first message is sent accross the network.  Subsequent messages are sent
 * to the local console.
 */

void
Notify_IPCServer(const char *format, va_list args, CommonDiagSpecifier type)
{
	char	message[LONG_STRING], *heading;
	DiagModeSpecifier	oldDiagMode = GetDSAMPtr_Common()->diagMode;

	if (!GetDSAMPtr_Common()->notificationCount) {
		snprintf(message, LONG_STRING, "%s: %s\n", DiagnosticTitle(type),
		  format);
		DPrint_IPCServer(message, args);
		SetDiagMode(COMMON_CONSOLE_DIAG_MODE);
		heading = "\nDiagnostics:-\n";
	} else
		heading = "";
	snprintf(message, LONG_STRING, "%s%s\n", heading, format);
	DPrint(message, args);
	GetDSAMPtr_Common()->diagMode = oldDiagMode;

} /* NotifyMessage */
