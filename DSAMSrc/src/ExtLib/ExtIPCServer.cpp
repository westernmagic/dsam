/**********************
 *
 * File:		EXTIPCSERVER.cpp
 * Purpose:		Inter-process communication server extension code module.
 * Comments:	This code module was revised from the GrSimMgr.cpp code module.
 * Author:		L. P. O'Mard
 * Created:		06 Jan 1995
 * Updated:		
 * Copyright:	(c) 2003, University of Essex
 *
 **********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>

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
		buffer += '\0';
		if (!buffer.length())
			break;
		printf("%s: Got string '%s'\n", funcName, (char *) buffer.GetData());
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
			switch (stream.GetLastError()) {
			case wxSTREAM_NO_ERROR:     printf( _T("wxSTREAM_NO_ERROR\n") ); break;
			case wxSTREAM_EOF:          printf( _T("wxSTREAM_EOF\n") ); break;
			case wxSTREAM_READ_ERROR:   printf( _T("wxSTREAM_READ_ERROR\n") ); break;
			case wxSTREAM_WRITE_ERROR:  printf( _T("wxSTREAM_WRITE_ERROR\n") ); break;
			default: printf( _T("Huh?\n") ); break;
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

