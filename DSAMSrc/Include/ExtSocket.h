/**********************
 *
 * File:		ExtSocket.h
 * Purpose:		Forms classes so that the socket code can be overloaded for use
 *				with the inetd/xinetd superserver.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		25 Feb 2004
 * Updated:		
 * Copyright:	(c) 2004, CNBH, University of Essex
 *
 **********************/

#ifndef _EXTSOCKET_H
#define _EXTSOCKET_H 1

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

class wxFile;

/*************************** SocketBase ***************************************/

class SocketBase: public wxSocketBase {

public:

	SocketBase(void): wxSocketBase() { ; }

	virtual bool	Destroy(void)	{ return wxSocketBase::Destroy(); }
	virtual bool	Error(void)	{ return wxSocketBase::Error(); }
	virtual bool	IsConnected(void)	{ return wxSocketBase::IsConnected(); }
	virtual SocketBase& Read(void* buffer, wxUint32 nBytes) { return
						  (SocketBase &) wxSocketBase::Read(buffer, nBytes); }
	virtual void	SetFlags(wxSocketFlags flags)	{wxSocketBase::SetFlags(
					  flags); }
	virtual SocketBase& Write(const void* buffer, wxUint32 nBytes) { return
						  (SocketBase &) wxSocketBase::Write(buffer, nBytes); }


};

/*************************** SSSocket *****************************************/

class SSSocket: public SocketBase {

	bool	reading, writing, error;
	wxUint32	lastCount;           // last IO transaction size
	wxFile	*inFile, *outFile;
	wxSocketFlags	flags;            // wxSocket flags

public:

	SSSocket(void);
	~SSSocket(void);

	bool	Destroy(void)	{ return(true); }
	bool	Error(void)	{ return error; }
	bool	IsConnected(void)	{ return true; }
	SocketBase&	Read(void* buffer, wxUint32 nBytes);
	void	SetFlags(wxSocketFlags theFlags)	{ flags = theFlags; }
	SocketBase&	Write(const void* buffer, wxUint32 nBytes);

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
