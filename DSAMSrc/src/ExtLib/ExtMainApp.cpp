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
#include "GeModuleMgr.h"
#include "UtString.h"
#include "UtOptions.h"
#include "UtDatum.h"
#include "UtSSSymbols.h"
#include "UtSimScript.h"
#include "UtAppInterface.h"
#include "ExtSimThread.h"
#include "ExtIPCUtils.h"
#include "ExtIPCServer.h"
#include "ExtMainApp.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

MainApp	*dSAMMainApp = NULL;

/******************************************************************************/
/****************************** Methods (Subroutines) *************************/
/******************************************************************************/

/****************************** Constructor ***********************************/

/*
 * This is the main routine for using the extensions library, and is called by
 * the "main" subroutine in the header file.
 */

MainApp::MainApp(int theArgc, char **theArgv, int (* TheExternalMain)(void),
  int (* TheExternalRunSimulation)(void))
{
	initOk = true;
	argc = theArgc;
	argv = theArgv;
	ExternalMain = TheExternalMain;
	ExternalRunSimulation = (TheExternalRunSimulation)?
	  TheExternalRunSimulation: TheExternalMain;
	serverFlag = false;
	serverPort = EXTIPCUTILS_DEFAULT_SERVER_PORT;
	simThread = NULL;
	dSAMMainApp = this;
	SetUsingExtStatus(TRUE);
	if (ExternalMain)
		initOk = InitRun();
	CheckOptions();
	
}

/****************************** Destructor ************************************/

/*
 */

MainApp::~MainApp(void)
{
	DeleteSimThread();
	
	FreeArgStrings();
	SetCanFreePtrFlag_AppInterface(TRUE);
	Free_AppInterface();
	dSAMMainApp = NULL;

}

/****************************** Main ******************************************/

/*
 */

int
MainApp::Main(void)
{
	static const char *funcName = "MainApp::Main";

	printf("MainApp::Main: Entered\n");
	if (serverFlag)
		return(RunServer());
	if (!InitMain(TRUE)) {
		 NotifyError("%s: Could not initialise the main program.",
		   funcName);
		return(false);
	}
	return(RunSimulation());

}

/****************************** RunServer *************************************/

/*
 */

int
MainApp::RunServer(void)
{
	static const char *funcName = "MainApp::RunServer";

	printf("MainApp::Main: Starting server operation.\n");
	SetOnExecute_AppInterface(OnExecute_MainApp);
	InitMain(FALSE);
	IPCServer *server = new IPCServer("", serverPort);
	if (!server->Ok()) {
		NotifyError("%s: Could not start server.\n", funcName);
		return(false);
	}
	while (true) {
		printf("%s: Debug: Waiting for connection on port %d\n", funcName,
		  serverPort);
		
        wxSocketBase *socket = server->InitConnection();
        if (!socket) {
            NotifyError("%s: Connection failed.", funcName);
            break;
        }
		printf("%s: Debug: Got client\n", funcName);
		while (socket->IsConnected()) {
			while (server->ProcessInput())
				;
			printf("%s: Debug: Client lost.\n", funcName);
			delete socket;
			break;
		}

	}
	return(0);

}

/****************************** CheckOptions **********************************/

/*
 * This routine checks for the program call options.
 * It sets the ones only to be used here to be ignored by other routines.
 */

void
MainApp::CheckOptions(void)
{
	int		optInd = 1, optSub = 0;
	char	c, *argument;

	while ((c = Process_Options(argc, argv, &optInd, &optSub, &argument,
	  "SI:")))
		switch (c) {
		case 'S':
			serverFlag = TRUE;
			MarkIgnore_Options(argc, argv, "-S", OPTIONS_NO_ARG);
			break;
		case 'I':
			serverPort = atoi(argument);
			MarkIgnore_Options(argc, argv, "-I", OPTIONS_WITH_ARG);
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
	token = strtok(workStr, MAINAPP_PARAMETER_STR_DELIMITERS);
	while (token) {
		if (!countOnly)
			SetArgvString(indexStart + count, token, strlen(token));
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
		return(FALSE);
	}
	if (GetPtr_AppInterface()->updateProcessVariablesFlag)
		ResetStepCount_Utility_Datum();
	for (int i = 0; i < argc; i++)
		printf("%2d: %s\n", i, argv[i]);
	if (!InitProcessVariables_AppInterface(NULL, argc, argv)) {
		NotifyError("%s: Could not initialise process variables.", funcName);
		return(FALSE);
	}

	return(TRUE);

}

/****************************** StartSimThread ********************************/

/*
 * This routine starts the simulation thread.
 */

void
MainApp::StartSimThread(void)
{
	static const char *funcName = "MainApp::StartSimThread";

	mainCritSect.Enter();
	SwitchGUILocking_Common(TRUE);
	mainCritSect.Leave();
	simThread = new SimThread();
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
/****************************** Callbacks *************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Functions *************************************/
/******************************************************************************/

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
	  "\t-S            \t: Run in server mode\n"
	  "\t-I <x>        \t: Run using server ID <x>.\n"
	  );

}

