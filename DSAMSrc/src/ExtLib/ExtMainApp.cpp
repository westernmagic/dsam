/**********************
 *
 * File:		ExtMainApp.cpp
 * Purpose:		This routine contains the main handling code running simulations
 *				using the extensions library.
 * Comments:	This routine contains the main routine for when the non-GUI mode
 *				is used.
 * Author:		L. P. O'Mard
 * Created:		12 Dec 2003
 * Updated:		
 * Copyright:	(c) 2003 CNBH, University of Essex
 *
 **********************/
 
#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>

#include <ExtCommon.h>

#include <wx/app.h>
#include <wx/socket.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeNSpecLists.h"
#include "GeModuleMgr.h"
#include "UtString.h"
#include "UtOptions.h"
#include "UtDatum.h"
#include "UtSSSymbols.h"
#include "UtSimScript.h"
#include "UtAppInterface.h"
#include "ExtSimThread.h"
#include "ExtIPCUtils.h"
#include "ExtSocket.h"
#include "ExtSocketServer.h"
#include "ExtIPCServer.h"
#include "ExtRunThreadedProc.h"
#include "tinyxml.h"
#include "ExtXMLDocument.h"
#include "ExtMainApp.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

bool	initialiseNonGUIwxWidgets = false;
MainApp	*dSAMMainApp = NULL;

/******************************************************************************/
/****************************** Methods (Subroutines) *************************/
/******************************************************************************/

/****************************** Constructor ***********************************/

/*
 * This is the main routine for using the extensions library, and is called by
 * the "main" subroutine in the header file.
 */

MainApp::MainApp(int theArgc, wxChar **theArgv, int (* TheExternalMain)(void),
  int (* TheExternalRunSimulation)(void))
{
	static const char *funcName = "MainApp::MainApp";

	initOk = true;
	argc = theArgc;
	argv = theArgv;
	argsAreLocalFlag = (!argc || !argv);
	ExternalMain = TheExternalMain;
	threadedSimExecutionFlag = true;	/* should be set by command-line */
	ExternalRunSimulation = (TheExternalRunSimulation)?
	  TheExternalRunSimulation: TheExternalMain;
	SetReadXMLSimFile_Utility_SimScript(ReadXMLSimFile_MainApp);
	if (!GetDSAMPtr_Common()->usingGUIFlag) {
		if (!initialiseNonGUIwxWidgets && !wxInitialize()) {
			NotifyError("%s: Failed to initialize the wxWindows library, "
			  "aborting.", funcName);
			exit(1);
		}
		initialiseNonGUIwxWidgets = true;
	}
	serverFlag = false;
	superServerFlag = false;
	diagsOn = false;
	serverPort = EXTIPCUTILS_DEFAULT_SERVER_PORT;
	simThread = NULL;
	dSAMMainApp = this;
	SetUsingExtStatus(TRUE);
	symList = NULL;
	InitKeyWords_Utility_SSSymbols(&symList);
	if (ExternalMain)
		initOk = InitRun();
	if (initOk)
		CheckOptions();
	runThreadedProc = new RunThreadedProc();
	
}

/****************************** Destructor ************************************/

//
// This routine does not call wxUninitialize() because there are errors with
// trying to re-call wxInitialize() - so it needs to be only called.
//

MainApp::~MainApp(void)
{
	DeleteSimThread();
	if (runThreadedProc)
		delete runThreadedProc;
	FreeSymbols_Utility_SSSymbols(&symList);
	if( argsAreLocalFlag)
		FreeArgStrings();
	SetCanFreePtrFlag_AppInterface(TRUE);
	if (!initOk)
		CloseFiles();
	Free_AppInterface();
	FreeAll_EarObject();
	FreeNull_ModuleMgr();
	if (initialiseNonGUIwxWidgets)
		wxUninitialize();
	dSAMMainApp = NULL;

}

/****************************** Main ******************************************/

/*
 */

int
MainApp::Main(void)
{
	static const char *funcName = "MainApp::Main";
	bool	ok = false;

	if (!initOk) {
		NotifyWarning("%s: Program not using application interface or not "
		  "correctly initialised.\n", funcName);
		return(false);
	}
	if (serverFlag)
		return(RunServer());
	if (!InitMain(TRUE)) {
		 NotifyError("%s: Could not initialise the main program.",
		   funcName);
		return(false);
	}
	if (!threadedSimExecutionFlag)
		return(RunSimulation());
	StartSimThread(wxTHREAD_JOINABLE);
	ok = CXX_BOOL(simThread->Wait());
	delete simThread;
	return(ok);

}

/****************************** RunServer *************************************/

/*
 * This function runs the server.
 * Note that the diagnostics are set to off by default.  Only the -d option
 * can turn them on.
 */

int
MainApp::RunServer(void)
{
	static const char *funcName = "MainApp::RunServer";

	SetOnExecute_AppInterface(OnExecute_MainApp);
	SetDPrintFunc(DPrintSysLog_MainApp);
	if (diagsOn)
		SetDiagMode(COMMON_DIALOG_DIAG_MODE);
	else
		SetDiagMode(COMMON_OFF_DIAG_MODE);
	InitMain(FALSE);
	IPCServer *server = new IPCServer(wxT(""), serverPort, superServerFlag);
	if (!server->Ok()) {
		NotifyError("%s: Could not start server.\n", funcName);
		return(false);
	}
	for ( ; ; ) {
       SocketBase *socket = server->InitConnection();
        if (!socket) {
            NotifyError("%s: Connection failed.", funcName);
            break;
        }
		while (socket->IsConnected()) {
			while (server->ProcessInput())
				;
			delete socket;
			if (superServerFlag)
				return(0);
			break;
		}

	}
	return(0);

}

/****************************** CheckOptions **********************************/

/*
 * This routine checks for the program call options.
 * It sets the ones only to be used here to be ignored by other routines.
 * Two instances of the "-S" flag set the super server flag indicating that the
 * server is to be run by xinetd/inetd.
 */

void
MainApp::CheckOptions(void)
{
	int		optInd = 1, optSub = 0;
	char	c, *argument;

	while ((c = Process_Options(argc, argv, &optInd, &optSub, &argument,
	  "SI:d:")))
		switch (c) {
		case 'S':
			if (!serverFlag)
				serverFlag = TRUE;
			else
				superServerFlag = TRUE;
			MarkIgnore_Options(argc, argv, "-S", OPTIONS_NO_ARG);
			break;
		case 'I':
			serverPort = atoi(argument);
			MarkIgnore_Options(argc, argv, "-I", OPTIONS_WITH_ARG);
			break;
		case 'd':
			SetDiagMode_AppInterface(argument);
			switch (GetPtr_AppInterface()->diagModeSpecifier) {
			case GENERAL_DIAGNOSTIC_SCREEN_MODE:
			case GENERAL_DIAGNOSTIC_ERROR_MODE:
				diagsOn = true;
				break;
			default:
				;
			} /* switch */
			break;
		default:
			;
		} /* switch */

}

/****************************** InitArgv **************************************/

/*
 * This allocates the memory for the argv pointer.
 */

bool
MainApp::InitArgv(int theArgc)
{
	static const char *funcName = "MainApp::InitArgv";

	if (!theArgc)
		return(TRUE);
	argc = theArgc;
	if ((argv = (char **) calloc(argc, sizeof(
	  char *))) == NULL) {
		NotifyError("%s: Out of memory for argv[%d] array.", funcName, argc);
		return(FALSE);
	}
	return(TRUE);

}

/****************************** SetArgvString *********************************/

bool
MainApp::SetArgvString(int index, char *string, int size)
{
	static const char *funcName = "MainApp::SetArgvString";
	
	if ((argv[index] = (char *) malloc(size + 1)) == NULL) {
		NotifyError("%s: Out of memory for argv[%d].", funcName, index);
		return(FALSE);
	}
	strcpy(argv[index], string);
	return(TRUE);

}

/****************************** ProtectQuotedStr ******************************/

/*
 * This function replaces characters between quotes so the strings are reqarded
 * as non-delimited.
 * The speech marks are also replaced with spaces.
 * It returns FALSE if it enounters an error.
 */

bool
MainApp::ProtectQuotedStr(char *str)
{
	static const char *funcName = "MainApp::ProtectQuotedStr";
	bool	quotesOn = FALSE;
	char	*p;

	for (p = str; *p != '\0'; p++) {
		if (*p == MAINAPP_QUOTE) {
			quotesOn = (!quotesOn);
			*p = ' ';
		} else
			if (quotesOn && (*p == ' '))
				*p = MAINAPP_SPACE_SUBST;
	}
	if (quotesOn) {
		NotifyError("%s: Incomplete quoted string in parameters.", funcName);
		return(FALSE);
	}
	return(TRUE);

}

/****************************** RestoreQuotedStr ******************************/

/*
 * This routine returns the spaces to a protected quoted string.
 */

char *
MainApp::RestoreQuotedStr(char *str)
{
	char	*p;

	for (p = str; *p != '\0'; p++)
		if (*p == MAINAPP_SPACE_SUBST)
			*p = ' ';
	return(str);

}

/****************************** SetParameterOptionArgs ************************/

/*
 * This function extracts the parameter options from a string.
 * In the count mode it only counts the parameter options, it does not add it to
 * the global argv list.
 * It returns the number of parameters, or a negative number if an error occurs.
 */

int
MainApp::SetParameterOptionArgs(int indexStart, char *parameterOptions,
  bool countOnly)
{
	static const char *funcName = "MainApp::SetParameterOptionArgs";
	char	*workStr, *token;
	int		count = 0;

	if ((workStr = (char *) malloc(strlen(parameterOptions) + 1)) == NULL) {
		NotifyError("%s: Out of memory for workStr.", funcName);
		return(-1);
	}
	strcpy(workStr, parameterOptions);
	if (!ProtectQuotedStr(workStr)) {
		NotifyError("%s; Could note protect quoted parameters.", funcName);
		return(-1);
	}
	token = strtok(workStr, MAINAPP_PARAMETER_STR_DELIMITERS);
	while (token) {
		if (!countOnly)
			SetArgvString(indexStart + count, RestoreQuotedStr(token),
			  strlen(token));
		count++;
		token = strtok(NULL, MAINAPP_PARAMETER_STR_DELIMITERS);
	}
	free(workStr);
	return(count);
}
	
/****************************** RemoveCommands ********************************/

/*
 * This routine removes the commands with a set prefix from the argv list by
 * setting the first character to zero.
 * It assumes that the argv list has a valid list of command pairs after the
 * offset index;
 */

void
MainApp::RemoveCommands(int offset, char *prefix)
{
	int		i;

	for (i = offset; i < argc; i++)
		if (StrNCmpNoCase_Utility_String(argv[i], prefix) == 0)
			*argv[i] = *argv[i + 1] = '\0';

}

/****************************** FreeArgStrings ********************************/

/*
 * This routine frees the argument strings.
 */

void
MainApp::FreeArgStrings(void)
{
	int		i;

	if (argv) {
		for (i = 0; i < argc; i++)
			if (argv[i])
				free(argv[i]);
		free(argv);
	}

}

/****************************** InitRun ***************************************/

/*
 * This function runs the simulation in the first initialisation run
 * during which the application interface is initialised.
 * With correct usage of the 'InitProcessVariables_AppInterface' routine
 * nothing after the program should be run.
 */

bool
MainApp::InitRun(void)
{
	static const char *funcName = "MainApp::InitRun";

	wxCriticalSectionLocker locker(mainCritSect);

	if (GetDSAMPtr_Common()->appInitialisedFlag) {
		NotifyWarning("%s: Attempted to re-initialise application.", funcName);
		return(TRUE);
	}
	(* ExternalMain)();
	SetCanFreePtrFlag_AppInterface(FALSE);
	return(CXX_BOOL(GetDSAMPtr_Common()->appInitialisedFlag));

}

/****************************** InitMain **************************************/

/*
 * This routine carries out the main initialisation of the application
 * interface.
 */

bool
MainApp::InitMain(bool loadSimulationFlag)
{
	static const char *funcName = "MainApp::InitMain";

	ResetStepCount_Utility_Datum();
	if (!GetPtr_AppInterface() || !GetPtr_AppInterface()->Init)
		return(true);
	GetPtr_AppInterface()->PrintExtMainAppUsage = PrintUsage_MainApp;
	ResetCommandArgFlags_AppInterface();
	GetPtr_AppInterface()->canLoadSimulationFlag = loadSimulationFlag;
	if (!InitProcessVariables_AppInterface(NULL, argc, argv)) {
		NotifyError("%s: Could not initialise process variables.", funcName);
		return(false);
	}
	GetPtr_AppInterface()->canLoadSimulationFlag = TRUE;
	return(true);
	
}

/****************************** InitXMLDocument *******************************/

/*
 * This initialises the 
 */

void
MainApp::InitXMLDocument(void)
{
	doc = new DSAMXMLDocument;

}

/****************************** LoadXMLDocument *******************************/

/*
 * This routine checks that the simulation run has been initialised.
 */

bool
MainApp::LoadXMLDocument(void)
{
	static const char *funcName = "MainApp::LoadXMLDocument";
	bool	ok = true;

	InitXMLDocument();
	if (!doc->LoadFile(simFileName.GetFullPath().c_str())) {
		NotifyError("%s: Could not load XML file '%s' (Error: %s).", funcName,
		  simFileName.GetFullPath().c_str(), doc->ErrorDesc());
		ok = false;
	}
	if (ok && !doc->Translate()) {
		NotifyError("%s: Could not translate XML document.", funcName);
		ok = false;
	}
	if (ok) {
		GetPtr_AppInterface()->audModel = doc->GetSimProcess();
		SetSimulationFileFlag_AppInterface(FALSE);
	}
	delete doc;
	return(ok);

}

/****************************** CheckInitialisation ***************************/

/*
 * This routine checks that the simulation run has been initialised.
 */

bool
MainApp::CheckInitialisation(void)
{
	static const char *funcName = "MainApp::CheckInitialisation";

	if (!GetPtr_AppInterface()) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(false);
	}
	if (GetPtr_AppInterface()->updateProcessVariablesFlag)
		ResetStepCount_Utility_Datum();
#	if DSAM_DEBUG
	for (int i = 0; i < argc; i++)
		printf("%s: %2d: %s\n", funcName, i, argv[i]);
#	endif /* DSAM_DEBUG */
	if (!InitProcessVariables_AppInterface(NULL, argc, argv)) {
		NotifyError("%s: Could not initialise process variables.", funcName);
		return(false);
	}
	return(true);

}

/****************************** StartSimThread ********************************/

/*
 * This routine starts the simulation thread.
 */

void
MainApp::StartSimThread(wxThreadKind kind)
{
	static const char *funcName = "MainApp::StartSimThread";

	mainCritSect.Enter();
	SwitchGUILocking_Common(TRUE);
	mainCritSect.Leave();
	simThread = new SimThread(kind);
	if (simThread->Create() != wxTHREAD_NO_ERROR)
		wxLogFatalError("%s: Can't create simulation thread!", funcName);

	if (simThread->Run() != wxTHREAD_NO_ERROR)
		wxLogError("%s: Cannot start simulation thread.", funcName);

}

/****************************** DeleteSimThread *******************************/

/*
 * This routine deletes the simulation thread.
 * It first turns off the GUI output so that locking problems with exit
 * diagnostics do not occur.
 * It also waits until the simThread pointer is set to NULL.
 */

void
MainApp::DeleteSimThread(void)
{
	SetDiagMode(COMMON_CONSOLE_DIAG_MODE);
	if (simThread) {
		SetInterruptRequestStatus_Common(TRUE);
		simThread->SuspendDiagnostics();
		simThread->Delete();
	}

}

/****************************** ResetSimulation *******************************/

bool
MainApp::ResetSimulation(void)
{
	static const char *funcName = "MainApp::ResetSimulation";

	if (simThread) {
		wxLogWarning("%s: Running simulation not yet terminated!", funcName);
		return(false);
	}
	SwitchGUILocking_Common(false);
	if (!CheckInitialisation())
		return(false);
	return(true);

}

/****************************** RunSimulation *********************************/

bool
MainApp::RunSimulation(void)
{
	static const char *funcName = "MainApp::RunSimulation";

	SetInterruptRequestStatus_Common(FALSE);
	if (!ExternalRunSimulation){
		NotifyError("%s: External run simulation function not set.", funcName);
		return(false);
	}
	return(CXX_BOOL((* ExternalRunSimulation)()));

}

/****************************** GetSimProcess *********************************/

EarObjectPtr
MainApp::GetSimProcess(void)
{
	static const char *funcName = "MainApp::GetSimProcess";
	EarObjectPtr	audModel;

	if ((audModel = GetSimProcess_AppInterface()) == NULL) {
		NotifyError("%s: simulation did not run successfully.", funcName);
		return(NULL);
	}
	return (audModel);

}

/******************************************************************************/
/****************************** SetSimulationFile *****************************/
/******************************************************************************/

/*
 * Sets the application interface file and name.
 */

void
MainApp::SetSimulationFile(wxFileName &fileName)
{
	simFileName = fileName;
	SetParsFilePath_AppInterface((char *) fileName.GetPath().c_str());
	SetSimFileType_AppInterface(GetSimFileType_Utility_SimScript((char *)
	  fileName.GetExt().c_str()));
	SetSimulationFile_AppInterface((char *) fileName.GetFullPath().c_str());

}

/******************************************************************************/
/****************************** Callbacks *************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Functions *************************************/
/******************************************************************************/

/****************************** ReadXMLSimFile ********************************/

/*
 * This function loads an XML file.  It is called from here because the XML
 * library code used is in C++.
 */

BOOLN
ReadXMLSimFile_MainApp(char *theFileName)
{
	static const char *funcName = "ReadXMLSimFile_MainApp";

	wxFileName fileName(theFileName);
	dSAMMainApp->SetSimulationFile(fileName);
	if (!dSAMMainApp->LoadXMLDocument()) {
		NotifyError("%s: Could not load XML Document.", funcName);
		return(FALSE);
	}
	return(TRUE);

}

/****************************** Onexecute *************************************/

/*
 * This routine starts an execution thread.
 * It expects to be called by the operating routine's "OnExecute" routine.
 * So it cannot be a member function.
 */

BOOLN
OnExecute_MainApp(void)
{
	dSAMMainApp->StartSimThread();
	return(TRUE);

}

/****************************** PrintUsage ************************************/

/*
 * This routine prints the usage for the options used by myApp.
 * It expects to be called by the operating routine's "PrintUsage" routine.
 * So it cannot be a member function.
 */

void
PrintUsage_MainApp(void)
{
	fprintf(stderr, "\n"
	  "\t-S            \t: Run in server mode (add another -S with (x)inetd).\n"
	  "\t-I <x>        \t: Run using server ID <x>.\n"
	  );

}

/*************************** DPrintSysLog *************************************/

/*
 * This routine prints out a diagnostic message to the system log.
 */
 
void
DPrintSysLog_MainApp(char *format, va_list args)
{
	vsyslog(LOG_INFO, format, args);
	
}

