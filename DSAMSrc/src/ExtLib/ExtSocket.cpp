/**********************
 *
 * File:		ExtSocket.cpp
 * Purpose:		Forms classes so that the socket code can be overloaded for use
 *				with the inetd/xinetd superserver.
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
#include <wx/file.h>

#include "ExtSocket.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** SSSocket Methods (Subroutines) ****************/
/******************************************************************************/

/****************************** Constructor ***********************************/

SSSocket::SSSocket(void): SocketBase()
{
	reading = false;
	writing = false;
	error = false;
	flags = 0;
	lastCount = 0;
	inFile = new wxFile(wxFile::fd_stdin);
	outFile = new wxFile(wxFile::fd_stdout);
	outFile->Flush();

}

/****************************** Desstructor ***********************************/

SSSocket::~SSSocket(void)
{
	inFile->Detach();
	outFile->Detach();

}

/****************************** Read ******************************************/

SocketBase &
SSSocket::Read(void* buffer, wxUint32 nBytes)
{
	// Mask read events
	reading = true;
	lastCount = inFile->Read(buffer, nBytes);

	// If in wxSOCKET_WAITALL mode, all bytes should have been read.
	if (flags & wxSOCKET_WAITALL)
		error = (lastCount != nBytes);
	else
		error = (lastCount == 0);

	// Allow read events from now on
	reading = false;

	return *this;

}
/****************************** Write *****************************************/

SocketBase &
SSSocket::Write(const void* buffer, wxUint32 nBytes)
{
	// Mask write events
	writing = TRUE;

	lastCount = outFile->Write(buffer, nBytes);

	// If in wxSOCKET_WAITALL mode, all bytes should have been written.
	if (flags & wxSOCKET_WAITALL)
		error = (lastCount != nBytes);
	else
		error = (lastCount == 0);

	// Allow write events again
	writing = FALSE;

	return *this;

}

/******************************************************************************/
/****************************** Callbacks *************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Functions *************************************/
/******************************************************************************/

