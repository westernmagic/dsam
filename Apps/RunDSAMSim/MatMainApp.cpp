/**********************
 *
 * File:		MatMainApp.cpp
 * Purpose:		This routine contains the DSAM matlab/octave specific class
 *				derived from the MainApp class for handling the DSAM extensions
 *				library.
 * Comments:	This was create for use by the scripting language interface
 *				RunDSAMSim.
 *				It was revised from the previous DSAMMatrix code module.
 * Author:		L. P. O'Mard
 * Created:		07 Jan 2004
 * Updated:		
 * Copyright:	(c) 2004, CNBH, University of Essex
 *
 *********************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <DSAM.h>

#include "MatMainApp.h"

/******************************************************************************/
/*************************** Global Variables *********************************/
/******************************************************************************/

MatMainApp	*matMainAppPtr = NULL;

/******************************************************************************/
/****************************** Methods (Subroutines) *************************/
/******************************************************************************/

/****************************** Constructor ***********************************/

/*
 * This is the main routine for using the extensions library, and is called by
 * the "main" subroutine in the header file.
 * The 'SetParsFilePath_Common' call is needed because octave/matlab do not
 * re-initialise the global variables on subsequent runs.
 */

MatMainApp::MatMainApp(char *programName, char *simFile, char *parameterOptions,
  double *theInputData, int theNumChannels, int theInterleaveLevel,
  ChanLen theLength, double theDt, bool theStaticTimeFlag,
  double theOutputTimeOffset): MainApp(0, NULL)
{
	static const char *funcName = "MatMainApp::MatMainApp";

	matMainAppPtr = this;
	strcpy(serverHost, "");
	autoNumRunsMode = GENERAL_BOOLEAN_OFF;
	serverMode = GENERAL_BOOLEAN_OFF;
	serverPort = EXTIPCUTILS_DEFAULT_SERVER_PORT;
	segmentDuration = -1.0;
	updateProcessVariablesFlag = false;

	if (!SetUniParList_MatMainApp(&parList)) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		SetInitStatus(false);
		return;
	}
	numberOfRuns = 1;
	inputProcess = NULL;
	myClient = NULL;
	SetParsFilePath_Common(NULL);
	numChannels = theNumChannels;
	if (theInterleaveLevel < 0) 
		interleaveLevel = (numChannels == 2)? 2:
		  SIGNALDATA_DEFAULT_INTERLEAVE_LEVEL;
	else
		interleaveLevel = theInterleaveLevel;
	dt = theDt;
	staticTimeFlag = theStaticTimeFlag;
	outputTimeOffset = theOutputTimeOffset;
	length = theLength;
	inputData = theInputData;
	if (!SetArgStrings(programName, simFile, parameterOptions)) {
		NotifyError("%s: Could not set argument strings.", funcName);
		SetInitStatus(false);
		return;
	}
#	if DSAM_DEBUG
	for (int i = 0; i < GetArgc(); i++)
		printf("%s: %2d: %s\n", funcName, i, GetArgv()[i]);
#	endif /* DSAM_DEBUG */
	if (!InitRun()) {
		NotifyError("%s: Could not execute initialisation run.", funcName);
		SetInitStatus(false);
		return;
	}

}

/****************************** Destructor ************************************/

/*.
 */

MatMainApp::~MatMainApp(void)
{
	if (inputProcess) {
		if (!serverMode)
			DisconnectOutSignalFromIn_EarObject(inputProcess,
			  GetSimProcess_AppInterface());
		Free_EarObject(&inputProcess);
	}
	if (myClient)
		delete myClient;
	if (parList)
		FreeList_UniParMgr(&parList);
	matMainAppPtr = NULL;

}

/****************************** InitRun ***************************************/

/*
 * This function runs the simulation in the first initialisation run
 * during which the application interface is initialised.
 */

bool
MatMainApp::InitRun(void)
{
	InitProcessVariables_AppInterface(InitApp_MatMainApp, GetArgc(), GetArgv());
	if (!GetDSAMPtr_Common()->appInitialisedFlag) {
		NotifyError("RunDSAMSim: Could not initialise process variables.");
		return(false);
	}
	return(true);
}

/****************************** SetArgStrings *********************************/

/*
 * This routine sets the argument strings using the global argc and argv
 * variables.
 */

bool
MatMainApp::SetArgStrings(char *programName, char *simFile,
  char *parameterOptions)
{
	static const char *funcName = "MainApp::SetArgStrings";
	int		numArgs = NUM_BASE_ARGUMENTS;

	if ((numArgs = SetParameterOptionArgs(0, parameterOptions, TRUE)) < 0) {
		NotifyError("%s: Could not count the parameter options", funcName);
		return(false);
	}
	numArgs += NUM_BASE_ARGUMENTS;
	if (!InitArgv(numArgs)) {
		NotifyError("%s: could not initialise arg strings.", funcName);
		return(false);
	}
	SetArgvString(0, programName, strlen(programName));
	SetArgvString(1, "-s", 2);
	SetArgvString(2, simFile, strlen(simFile));
	SetParameterOptionArgs(NUM_BASE_ARGUMENTS, parameterOptions, false);
	return(true);

}

/****************************** AutoSetNumberOfRuns ***************************/

/*
 * This routine sets the number of runs according to a loaded simulation script
 * which has a 'DataFile_In' process at the beginning, or if there is external
 * input data.
 */

bool
MatMainApp::AutoSetNumberOfRuns(ChanLen inputLength, double dt)
{
	static char *funcName = "MatMainApp::AutoSetNumberOfRuns";
	double	totalDuration;
	FILE	*savedErrorsFilePtr = GetDSAMPtr_Common()->errorsFile;
	EarObjectPtr	process;

	if (!GetDSAMPtr_Common()->segmentedMode) {
		numberOfRuns = 1;
		return(TRUE);
	}

	if (inputLength)
		totalDuration = inputLength * dt;
	else {
		SetErrorsFile_Common("off", OVERWRITE);
		process = GetFirstProcess_Utility_Datum(GetSimulation_ModuleMgr(
		  GetPtr_AppInterface()->audModel));
		GetDSAMPtr_Common()->errorsFile = savedErrorsFilePtr;
		if (!process)
			return(TRUE);
		numberOfRuns = 1;	/* Default value */
		if (StrCmpNoCase_Utility_String(process->module->name, "DataFile_In") !=
		  0) {
			NotifyError("%s: Operation failed. First process is not "
			  "DataFile_In or the input signal is not a matlab matrix.\n",
			  funcName);
			return(TRUE);
		}
		segmentDuration = *GetUniParPtr_ModuleMgr(process, "duration")->
		  valuePtr.r;
		if ((totalDuration = (((DataFilePtr) process->module->parsPtr)->
		  GetDuration)()) < 0.0) {
			NotifyError("%s: Could not determine signal size for data file.",
			  funcName);
			return(FALSE);
		}
	}
	if (segmentDuration < 0.0) {
		NotifyError("%s: Segment duration must be set when using auto 'number "
		  "of runs' mode.", funcName);
		return(FALSE);
	}
	if (segmentDuration > totalDuration) {
		NotifyError("%s: Segment size (%g ms) is larger than total signal "
		  "duration (%g ms).", funcName, MILLI(segmentDuration),
		  MILLI(totalDuration));
		return(FALSE);
	}
	numberOfRuns = (int) floor(totalDuration / segmentDuration);
	return(TRUE);

}

/****************************** SetInputProcessData ***************************/

/*
 * This routine updates the process time and resets the output signal
 * channel pointers accordingly using the input data.
 * The outSignal->length is actually the segment length in segment mode.
 */

void
MatMainApp::SetInputProcessData(EarObjectPtr process, ChanLen signalLength,
  double *data)
{
	register ChanData	*outPtr, *inPtr;
	int		chan;
	ChanLen	i;

	for (chan = 0; chan < process->outSignal->numChannels; chan++) {
		outPtr = process->outSignal->channel[chan];
		inPtr = data + process->timeIndex - PROCESS_START_TIME + chan;
		for (i = 0; i < signalLength; i++, inPtr += process->outSignal->
		  numChannels)
			*outPtr++ = *inPtr;
	}
	SetTimeContinuity_EarObject(process);

}

/****************************** InitInputEarObject ****************************/

/*
 * This function returns a pointer to an Earobject for providing input to a
 * simulation.
 * It uses the low level DSAM functions to create the EarObject, so that
 * unnecessary copies of the data need not be made.
 * This means that it cannot be free'd in the same way as a normal EarObject.
 */

bool
MatMainApp::InitInputEarObject(ChanLen segmentLength)
{
	static char	*funcName = "InitResultEarObject";

	if (dt <= 0.0) {
		NotifyError("%s: dt must be greater than zero.", funcName);
		return(false);
	}
	if (inputProcess)
		Free_EarObject(&inputProcess); 	/* Just in case */
	if ((inputProcess = Init_EarObject("NULL")) == NULL) {
		NotifyError("%s: Could not initialise data results earObject.",
		  funcName);
		return(false);
	}
	if (!InitOutSignal_EarObject(inputProcess, numChannels, segmentLength,
	  dt)) {
		NotifyError("%s: Cannot initialise input process", funcName);
		return(false);
	}
	SetStaticTimeFlag_SignalData(inputProcess->outSignal, staticTimeFlag);
	SetLocalInfoFlag_SignalData(inputProcess->outSignal, TRUE);
	SetInterleaveLevel_SignalData(inputProcess->outSignal, interleaveLevel);
	inputProcess->outSignal->rampFlag = TRUE;
	SetInputProcessData(inputProcess, length, inputData);
	return(true);

}

/****************************** RunSimulationLocal ****************************/

/*
 * This function runs the simulation process using the DSAM AppInterface API.
 * The 'inputProcess' EarObject cannot be freed here as it may be used later.
 */

bool
MatMainApp::RunSimulationLocal(void)
{
	static const char *funcName = "RunSimulation";
	bool	ok = true;
	int		i;
	ChanLen	segmentLength = 0;
	
	GetPtr_AppInterface()->updateProcessVariablesFlag = TRUE;
	if (!ResetSimulation()) {
		NotifyError("%s: Could not set the simulation\n", funcName);
		return(false);
	}
	if (autoNumRunsMode) {
		if (!AutoSetNumberOfRuns(length, dt))
			ok = FALSE;
		else
			segmentLength = (ChanLen) floor(segmentDuration  / dt + 0.5);
	} else
		segmentLength = length;
	EarObjectPtr	audModel = GetSimProcess_AppInterface();
	if (ok && inputData) {
		if (!InitInputEarObject(segmentLength)) {
			NotifyError("Could not initialise input process.");
			return(false);
		}
		ConnectOutSignalToIn_EarObject(inputProcess, audModel);
	}
	if (ok) {
		PrintPars_ModuleMgr(audModel);
		ResetSim_AppInterface();
		if (inputProcess)
			ResetProcess_EarObject(inputProcess);
		for (i = 0; i < numberOfRuns; i++) {
			if (inputProcess)
				SetInputProcessData(inputProcess, length, inputData);
			if (!RunSim_AppInterface()) {
				ok = false;
				break;
			}
		}
	}
	return(ok);

}

/****************************** RunSimulationRemote ***************************/

/*
 * This function runs the simulation process using the DSAM AppInterface API.
 * The 'inputProcess' EarObject cannot be freed here as it may be used later.
 */

bool
MatMainApp::RunSimulationRemote(void)
{
	static const char *funcName = "MatMainApp::RunSimulationRemote";

	if (!myClient)
		myClient = new IPCClient(serverHost, serverPort);
	if (!myClient->Ok())
		return(false);
	RemoveCommands(NUM_BASE_ARGUMENTS - 1, MATMAINAPP_COMMAND_PREFIX);
	if (!myClient->SendArguments(GetArgc(), GetArgv())) {
		NotifyError("%s: Could not initialise remote simulation.", funcName);
		return(false);
	}
	if (!myClient->InitSimFromFile(GetPtr_AppInterface()->simulationFile)) {
		NotifyError("%s: Could not initialise remote simulation.", funcName);
		return(false);
	}
#	if DSAM_DEBUG
	for (int i = 0; i < GetArgc(); i++)
		printf("%s: %2d: %s\n", funcName, i, GetArgv()[i]);
#	endif /* DSAM_DEBUG */
	if (inputData) {
		if (!InitInputEarObject(length)) {
			NotifyError("%s: Could not initialise input process.", funcName);
			return(false);
		}
		if (!myClient->GetIPCUtils()->InitOutProcess()) {
			NotifyError("%s: Could not initialise output process for input "
			  "data.", funcName);
			return(false);
		}
		myClient->GetIPCUtils()->ConnectToOutProcess(inputProcess);
		if (!myClient->SendInputProcess()) {
			NotifyError("%s: Could not send the input data.", funcName);
			return(false);
		}
	}
	if (!myClient->RunSimulation()) {
		NotifyError("%s: Could not run remote simulation.", funcName);
		return(false);
	}
	myClient->GetAllOutputFiles();
	return(true);

}

/****************************** GetSimProcess *********************************/

/*
 * This function returns the simulation process.
 */

EarObjectPtr
MatMainApp::GetSimProcess(void)
{
	if (serverMode)
		return((myClient)? myClient->GetSimProcess(): NULL);
	return(GetSimProcess_AppInterface());

}

/****************************** Main ******************************************/

/*
 * This function runs the simulation process .
 */

int
MatMainApp::Main(void)
{
	static const char *funcName = "MatMainApp::Main";

	if (!InitMain(FALSE)) {
		 NotifyError("%s: Could not initialise the main program.",
		   funcName);
		return(false);
	}
	if (serverMode)
		return(RunSimulationRemote());
	else
		return(RunSimulationLocal());
}

/******************************************************************************/
/*************************** Subroutines and functions ************************/
/******************************************************************************/

/****************************** InitApp ***************************************/

/*
 * This routine carries out general initialisation tasks.
 * It sets the initialParameter global variable.
 */

BOOLN
InitApp_MatMainApp(void)
{
	static const char *funcName = "InitApp_MatMainApp";

	if (!GetPtr_AppInterface() && !Init_AppInterface(GLOBAL)) {
		NotifyError("%s: Could not initialise the application interface.",
		  funcName);
		exit(1);
	}

	SetAppName_AppInterface(PROGRAM_NAME);
	SetAppVersion_AppInterface(DSAMMAT_VERSION);
	SetCompiledDSAMVersion_AppInterface(DSAM_VERSION);

	SetAppSetUniParList_AppInterface(SetUniParList_MatMainApp);

	//SetAppPrintUsage_AppInterface(PrintUsage);
	//SetAppProcessOptions_AppInterface(ProcessOptions);
	//SetAppPostInitFunc_AppInterface(PostInitFunc);

	SetAppRegisterUserModules_AppInterface(RegisterUserModules_MatMainApp);
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the programs universal parameter
 * list. This list provides universal access to the program's
 * parameters.  It needs to be called from an initialise routine.
 */

BOOLN
SetUniParList_MatMainApp(UniParListPtr *parList)
{
	static const char *funcName = "SetUniParList_MatMainApp";
	UniParPtr	pars;

	if ((*parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  DSAMMAT_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = (*parList)->pars;
	SetPar_UniParMgr(&pars[DSAMMATRIX_AUTONUMRUNSMODE], "AUTO_NUM_RUNS_MODE",
	  "Auto-setting of the number of runs (matlab matrix or data files only) "
	  "('on' or 'off').",
	  UNIPAR_BOOL,
	  &matMainAppPtr->autoNumRunsMode, NULL,
	  (void * (*)) SetAutoNumRunsMode_MatMainApp);
	SetPar_UniParMgr(&pars[DSAMMATRIX_NUMBEROFRUNS], "NUM_RUNS",
	  "Number of repeat runs, or segments/frames.",
	  UNIPAR_INT,
	  &matMainAppPtr->numberOfRuns, NULL,
	  (void * (*)) SetNumberOfRuns_MatMainApp);
	SetPar_UniParMgr(&pars[DSAMMATRIX_SEGMENTDURATION], "SEGMENT_DURATION",
	  "Segment duration for automatic number of runs operation (segment mode).",
	  UNIPAR_REAL,
	  &matMainAppPtr->segmentDuration, NULL,
	  (void * (*)) SetSegmentDuration_MatMainApp);
	SetPar_UniParMgr(&pars[DSAMMATRIX_SERVERMODE], MATMAINAPP_COMMAND_PREFIX
	  "MODE",
	  "Enables the server mode to run simulations on a remote system ('on' or "
	  "'off').",
	  UNIPAR_BOOL,
	  &matMainAppPtr->serverMode, NULL,
	  (void * (*)) SetServerMode_MatMainApp);
	SetPar_UniParMgr(&pars[DSAMMATRIX_SERVERHOST], MATMAINAPP_COMMAND_PREFIX
	  "HOST",
	  "Sets the remote server host - default is the local host.",
	  UNIPAR_STRING,
	  &matMainAppPtr->serverHost, NULL,
	  (void * (*)) SetServerHost_MatMainApp);
	SetPar_UniParMgr(&pars[DSAMMATRIX_SERVERPORT], MATMAINAPP_COMMAND_PREFIX
	  "PORT",
	  "Sets the remote server host port for connections.",
	  UNIPAR_INT,
	  &matMainAppPtr->serverPort, NULL,
	  (void * (*)) SetServerPort_MatMainApp);
	return(TRUE);

}

/****************************** SetAutoNumRunsMode ****************************/

/*
 * This functions sets the automatic number of runs setting mode.
 * It returns false if it fails in any way.
 */

BOOLN
SetAutoNumRunsMode_MatMainApp(char *theAutoNumRunsMode)
{
	static const char	*funcName = PROGRAM_NAME": SetAutoNumRunsMode";
	int		specifier;

	if ((specifier = Identify_NameSpecifier(theAutoNumRunsMode,
	  BooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError("%s: Illegal auto. number of runs mode mode (%s): must be "
		  "'on' or 'off'.", funcName, theAutoNumRunsMode);
		return(FALSE);
	}
	matMainAppPtr->updateProcessVariablesFlag = true;
	matMainAppPtr->autoNumRunsMode = specifier;
	return(TRUE);

}

/****************************** SetNumberOfRuns *******************************/

/*
 * This functions sets the number of simulation runs.
 * It returns false if it fails in any way.
 */

BOOLN
SetNumberOfRuns_MatMainApp(int theNumberOfRuns)
{
	static const char	*funcName = PROGRAM_NAME": SetNumberOfRuns";

	if (theNumberOfRuns < 0) {
		NotifyError("%s: Illegal number of runs (%d).", funcName,
		  theNumberOfRuns);
		return(FALSE);
	}
	matMainAppPtr->numberOfRuns = theNumberOfRuns;
	return(TRUE);

}

/****************************** SetSegmentDuration ****************************/

/*
 * This functions sets the segment duration which is used in conjunction with
 * the automatic setting of the number of runs in segment process mode.
 */

BOOLN
SetSegmentDuration_MatMainApp(double theSegmentDuration)
{
	static const char *funcName = PROGRAM_NAME": SetSegmentDuration_MatMainApp";

	matMainAppPtr->segmentDuration = theSegmentDuration;
	return(TRUE);

}

/****************************** SetServerMode *********************************/

/*
 * This function sets the module's serverMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetServerMode_MatMainApp(char * theServerMode)
{
	static const char	*funcName = "SetServerMode_MatMainApp";
	int		specifier;

	if ((specifier = Identify_NameSpecifier(theServerMode,
		BooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError("%s: Illegal server mode (%s): must be 'on' or 'off'.",
		  funcName, theServerMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	matMainAppPtr->updateProcessVariablesFlag = true;
	matMainAppPtr->serverMode = specifier;
	return(TRUE);

}

/****************************** SetServerHost *********************************/

/*
 * This function sets the module's serverHost parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetServerHost_MatMainApp(char *theServerHost)
{
	static const char	*funcName = PROGRAM_NAME": SetServerHost_MatMainApp";

	snprintf(matMainAppPtr->serverHost, MAXLINE, "%s", theServerHost);
	return(TRUE);

}

/****************************** SetServerPort *********************************/

/*
 * This functions sets the server port for connections.
 * It returns false if it fails in any way.
 */

BOOLN
SetServerPort_MatMainApp(int theServerPort)
{
	static const char	*funcName = PROGRAM_NAME": SetServerPort_MatMainApp";

	if (theServerPort < 0) {
		NotifyError("%s: Illegal port number (%d).", funcName, theServerPort);
		return(FALSE);
	}
	matMainAppPtr->serverPort = theServerPort;
	return(TRUE);

}

/****************************** RegisterUserModules ***************************/

/*
 * This routine registers my user modules.
 */

BOOLN
RegisterUserModules_MatMainApp(void)
{
	/*RegEntry_ModuleReg("BM_Carney2001", InitModule_BasilarM_Carney2001); */
	return(TRUE);

}
