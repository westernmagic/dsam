/**********************
 *
 * File:		ExtSocketServer.cpp
 * Purpose:		This class enables the use with the inetd/xinetd superserver.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		25 Feb 2004
 * Updated:		
 * Copyright:	(c) 2004, CNBH, University of Essex
 *
 **********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <wx/socket.h>
#include <wx/log.h>

#include "ExtSocketServer.h"
#include "ExtSocket.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Methods (Subroutines) *************************/
/******************************************************************************/

/****************************** Constructor ***********************************/

SocketServer::SocketServer(wxSockAddress& addr, bool useSuperServer,
  wxSocketFlags flags): wxSocketServer(addr, flags)
{
	usingSuperServer = useSuperServer;

}

/****************************** Accept ****************************************/

SocketBase *
SocketServer::Accept(bool wait)
{
	static const char *funcName = "SocketServer::Accept";

	if (!usingSuperServer) {
		SocketBase *socket = new SocketBase();
		if (!wxSocketServer::AcceptWith(*socket, wait)) {
			wxLogError("%s: Could not accept connection.", funcName);
			return(NULL);
		}
		return(socket);
	}
	return(new SSSocket());

}

/******************************************************************************/
/****************************** Callbacks *************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Functions *************************************/
/******************************************************************************/

