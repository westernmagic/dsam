/**********************
 *
 * File:		ExtIPCClient.cpp
 * Purpose:		Inter-process communication client extension code module.
 * Comments:	
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

#include "ExtCommon.h"

// Any files included regardless of precompiled headers
#include <wx/socket.h>
#include <wx/filesys.h>
#include <wx/datstrm.h>
#include <wx/wfstream.h>

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
#include "ExtIPCClient.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Methods (Subroutines) *************************/
/******************************************************************************/

/****************************** Constructor ***********************************/

IPCClient::IPCClient(const wxString& hostName, uShort theServicePort)
{
	static const char *funcName = "IPCClient::IPCClient";
	wxIPV4address	addr;
	wxString	salutation;

	ok = true;
	if (!iPCUtils.InitInProcess()) {
		ok = false;
		return;
	}
	addr.Hostname((hostName.length())? hostName: "localhost");
	addr.Service((!theServicePort)? EXTIPCUTILS_DEFAULT_SERVER_PORT:
	  theServicePort);
	if (!Connect(addr)) {
		NotifyError("%s: Failed to connect to %s:%u\n", funcName,
		  addr.Hostname().c_str(), addr.Service());
		ok = false;
		return;
	}
	SetFlags(wxSOCKET_WAITALL);
	ReadString(salutation);
	DPrint("%s: %s\n", funcName, salutation.c_str());

}

/****************************** Destructor ************************************/

/*
 * The EarObjects are free'd by the main controlling routine.
 */

IPCClient::~IPCClient(void)
{
	
}

/****************************** ReadString ************************************/

/*
 * Reads a string from the input socket, until '\0' is reached.
 */

bool
IPCClient::ReadString(wxString &s)
{
	wxChar	ch = _T('\0');

	s.Clear();
	while (!Read(&ch, sizeof(ch)).Error()) {
		if (ch == '\n')
			break;
		else
			s += ch;
	}
	return(true);

}

/****************************** SendCommand ***********************************/

/*
 */

bool
IPCClient::SendCommand(IPCCommandSpecifier command)
{
	wxString	s;

	s.Printf("%s\n", iPCUtils.CommandList(command)->name);
	Write(s, s.length());
	return(true);

}

/****************************** CheckStatus ***********************************/

/*
 * This routine sends a status command to the server and waits for the
 * response.
 */

unsigned char
IPCClient::CheckStatus(void)
{
	unsigned char	status;

	SendCommand(IPC_COMMAND_STATUS);
	Read(&status, 1);
	return(status);

}

/****************************** Errors ****************************************/

/*
 * Test the check the errors returned command with a simulation.
 */

bool
IPCClient::Errors(void)
{
	unsigned char	numErrors, c;
	int		i;

	SendCommand(IPC_COMMAND_ERRMSGS);
	Read(&numErrors, 1);
	if (!numErrors)
		return(false);
	for (i = 0; i < numErrors; i++) {
		while (!Read(&c, 1).Error() && (c != '\n'))
			DPrint("%c", c);
		DPrint("\n");
	}
	return(true);

}

/****************************** InitSimulation ********************************/

/*
 * Initialise the server with a simulation given as a string.
 */

bool
IPCClient::InitSimulation(const char *simulation)
{
	unsigned char	eof = (unsigned char) EOF;

	WaitForReady();
	SendCommand(IPC_COMMAND_INIT);
	Write(simulation, strlen(simulation));
	Write(&eof, 1);
	return(true);

}

/****************************** CreateSPFFromSimScript ************************/

/*
 * This routine converts a simulation script to a simulation parameter file.
 * It changes the name given as a reference argument.
 * It returns 'false' if it fails in any way.
 */

bool
IPCClient::CreateSPFFromSimScript(wxFileName &fileName)
{
	static const char *funcName = "IPCClient::CreateSPFFromSimScript";
	bool	ok = true;
	EarObjectPtr	process = NULL;

	if ((process = Init_EarObject("Util_SimScript")) == NULL) {
		NotifyError("%s: Could not initialise Util_SimCript process", funcName);
		return(false);
	}
	DiagModeSpecifier oldDiagMode = GetDSAMPtr_Common()->diagMode;
	SetDiagMode(COMMON_OFF_DIAG_MODE);
	if (!ReadPars_ModuleMgr(process, (char *) fileName.GetFullPath().c_str())) {
		NotifyError("%s: Could not read simulation.", funcName);
		ok = false;
	}
	SetDiagMode(COMMON_CONSOLE_DIAG_MODE);
	if (ok) {
		fileName.SetExt("spf");
		FILE *oldFp = GetDSAMPtr_Common()->parsFile;
		SetParsFile_Common((char *) fileName.GetFullPath().c_str(), OVERWRITE);
		if (!PrintSimParFile_ModuleMgr(process)) {
			NotifyError("%s: Could not list simulation parameter file.",
			  funcName);
			ok = false;
		}
		fclose(GetDSAMPtr_Common()->parsFile);
		GetDSAMPtr_Common()->parsFile = oldFp;
	}
	SetDiagMode(oldDiagMode );
	Free_EarObject(&process);
	return(ok);

}

/****************************** InitSimFromFile *******************************/

/*
 * Initialise the server with a simulation given as a file.
 */

bool
IPCClient::InitSimFromFile(const wxString &simFileName)
{
	static const char *funcName = "IPCClient::InitSimFromFile";
	wxUint32	i, length;
	wxUint8	byte;
	unsigned char	eof = (unsigned char) EOF;
	wxFileName fileName = simFileName;

	WaitForReady();
	if ((fileName.GetExt().CmpNoCase("sim") == 0) && !CreateSPFFromSimScript(
	  fileName)) {
		NotifyError("%s: Could not convert SIM file ('%s') to SPF file.",
		  funcName, fileName.GetFullName().c_str());
		return(false);
	}
	wxFFileInputStream inStream(fileName.GetFullPath());
	if (!inStream.Ok()) {
		NotifyError("%s: Could not open simulation file '%s'.", funcName,
		  fileName.GetFullName().c_str());
		return(false);
	}
	wxDataInputStream	data(inStream);

	length = inStream.GetSize();
	SendCommand(IPC_COMMAND_INIT);
	for (i = 0; i < length; i++) {
		byte = data.Read8();
		Write(&byte, 1);
	}
	Write(&eof, 1);
	return(true);

}

/****************************** GetAllOutputFiles *****************************/

/*
 * This function gets any output files created by the simulation.
 */

bool
IPCClient::GetAllOutputFiles(void)
{
	wxUint8		byte, numFiles;
	wxUint32	i, j, length;
	wxString	fileName;

	WaitForReady();
	SendCommand(IPC_COMMAND_GETFILES);
	Read(&numFiles, sizeof(numFiles));
	for (i = 0; i < numFiles; i++) {
		ReadString(fileName);
		printf("TestGetFilesCommand: Got file '%s'\n", fileName.c_str());
		Read(&length, sizeof(length));
		wxFFileOutputStream outStream(fileName);
		wxDataOutputStream	data(outStream);
		for (j = 0; j < length; j++) {
			Read(&byte, 1);
			data.Write8(byte);
		}
	}
	return(true);

}

/****************************** GetSimProcess *********************************/

/*
 */

EarObjectPtr
IPCClient::GetSimProcess(void)
{
	static const char *funcName = "IPCClient::GetSimProcess";
	wxUint32	length;

	WaitForReady();
	SendCommand(IPC_COMMAND_GET);
 	Read(&length, sizeof(length));
	if (!length) {
		NotifyError("%s: No data available from server.\n", funcName);
		return(NULL);
	}
	if (!iPCUtils.InitInputMemory(length)) {
		NotifyError("%s: Could not initialise memory for input process signal",
		  funcName);
		return(NULL);
	}
	Read(iPCUtils.GetInUIOPtr()->memStart, length);
	iPCUtils.RunInProcess();
	return(iPCUtils.GetInProcess());

}

/****************************** RunSimulation *********************************/

/*
 */

bool
IPCClient::RunSimulation(void)
{
	static const char *funcName = "IPCClient::RunSimulation";
	bool	ok = true;

	SendCommand(IPC_COMMAND_RUN);
	if (Errors()) {
		NotifyError("%s: Could not run remote simulation.", funcName);
		ok = false;
	}
	return(ok);

}

/****************************** SendInputProcess ******************************/

/*
 */

bool
IPCClient::SendInputProcess(void)
{
	static const char *funcName = "IPCClient::SendInputProcess";
	wxUint32	length;
	UPortableIOPtr	uIOPtr;

	if (!iPCUtils.GetOutProcess()) {
		NotifyError("%s: output process not initialised.", funcName);
		return(false);
	}
	if (!iPCUtils.RunOutProcess()) {
		NotifyError("%s: Could not run the output process (memory file "
		  "conversion.", funcName);
		return(false);
	}
	uIOPtr = ((DataFilePtr) iPCUtils.GetOutProcess()->module->parsPtr)->uIOPtr;
	length = (wxUint32) uIOPtr->length;
	SendCommand(IPC_COMMAND_PUT);
 	Write(&length, sizeof(length));
	Write(uIOPtr->memStart, uIOPtr->length);
	if (Errors()) {
		NotifyError("%s: Could not send input data.", funcName);
		return(false);
	}
	return(true);

}

/****************************** SendArguments *********************************/

/*
 * It counts the number of valid strings, then sends only those.
 * Invalid strings not to be sent will have a zero character at the beginning.
 */

bool
IPCClient::SendArguments(int argc, char **argv)
{
	wxUint8	numArgs;
	int		i;
	
	for (i = numArgs = 0; i < argc; i++)
		if (*argv[i])
			numArgs++;
	SendCommand(IPC_COMMAND_PUT_ARGS);
	Write(&numArgs, sizeof(numArgs));
	for (i = 0; i < argc; i++)
		if (*argv[i])
			Write(argv[i], strlen(argv[i]) + 1);
	return(true);

}

/******************************************************************************/
/****************************** Callbacks *************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Functions *************************************/
/******************************************************************************/

