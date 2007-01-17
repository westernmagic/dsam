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

#ifdef HAVE_CONFIG_H
#	include "RunDSAMSimSetup.h"
#endif /* HAVE_CONFIG_H */

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

MatMainApp::MatMainApp(wxChar *programName, const wxChar *simFile,
  const wxChar *parameterOptions, double *theInputData, int theNumChannels,
  int theInterleaveLevel, ChanLen theLength, double theDt,
  bool theStaticTimeFlag, double theOutputTimeOffset): MainApp(0, NULL)
{
	static const wxChar *funcName = wxT("MatMainApp::MatMainApp");

	matMainAppPtr = this;
	autoNumRunsMode = GENERAL_BOOLEAN_OFF;
	serverMode = GENERAL_BOOLEAN_OFF;
	segmentDuration = -1.0;
	updateProcessVariablesFlag = false;
	SetNotifyFunc(Notify_MatMainApp);

	if (!SetUniParList_MatMainApp(&parList)) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		SetInitStatus(false);
		return;
	}
	numberOfRuns = 1;
	inputProcess = NULL;
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
		NotifyError(wxT("%s: Could not set argument strings."), funcName);
		SetInitStatus(false);
		return;
	}
#	if DSAM_DEBUG
	for (int i = 0; i < GetArgc(); i++)
		printf("%s: %2d: %s\n", funcName, i, GetArgv()[i]);
#	endif /* DSAM_DEBUG */
	if (!InitRun()) {
		NotifyError(wxT("%s: Could not execute initialisation run."), funcName);
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
		NotifyError(wxT("RunDSAMSim: Could not initialise process variables."));
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
MatMainApp::SetArgStrings(wxChar *programName, const wxChar *simFile,
  const wxChar *parameterOptions)
{
	static const wxChar *funcName = wxT("MainApp::SetArgStrings");
	int		numArgs = MAINAPP_NUM_BASE_ARGUMENTS;

	if ((numArgs = SetParameterOptionArgs(0, parameterOptions, TRUE)) < 0) {
		NotifyError(wxT("%s: Could not count the parameter options"), funcName);
		return(false);
	}
	numArgs += MAINAPP_NUM_BASE_ARGUMENTS;
	if (!InitArgv(numArgs)) {
		NotifyError(wxT("%s: could not initialise arg strings."), funcName);
		return(false);
	}
	SetArgvString(0, programName, DSAM_strlen(programName));
	SetArgvString(1, wxT("-s"), 2);
	SetArgvString(2, simFile, DSAM_strlen(simFile));
	SetParameterOptionArgs(MAINAPP_NUM_BASE_ARGUMENTS, parameterOptions, false);
	return(true);

}

/****************************** AutoSetNumberOfRuns ***************************/

/*
 * This routine sets the number of runs according to a loaded simulation script
 * which has a 'DataFile_In' process at the beginning, or if there is external
 * input data.
 */

bool
MatMainApp::AutoSetNumberOfRuns(double dt)
{
	static wxChar *funcName = wxT("MatMainApp::AutoSetNumberOfRuns");
	double	totalDuration;
	EarObjectPtr	process;

	if (!GetDSAMPtr_Common()->segmentedMode) {
		numberOfRuns = 1;
		return(TRUE);
	}

	if (length)
		totalDuration = length * dt;
	else {
		if ((process = GetDataFileInProcess_AppInterface()) == NULL)
			return(TRUE);
		numberOfRuns = 1;	/* Default value */
		segmentDuration = *GetUniParPtr_ModuleMgr(process, wxT("duration"))->
		  valuePtr.r;
		if ((totalDuration = (((DataFilePtr) process->module->parsPtr)->
		  GetDuration)()) < 0.0) {
			NotifyError(wxT("%s: Could not determine signal size for data "
			  "file."), funcName);
			return(FALSE);
		}
	}
	if (inputData && (segmentDuration < 0.0)) {
		NotifyError(wxT("%s: Segment duration must be set when using auto "
		  "'number of runs' mode with an external signal."), funcName);
		return(FALSE);
	}
	if (segmentDuration > totalDuration) {
		NotifyError(wxT("%s: Segment size (%g ms) is larger than total signal "
		  "duration (%g ms)."), funcName, MILLI(segmentDuration),
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
	SignalDataPtr	outSignal = _OutSig_EarObject(process);

	for (chan = 0; chan < outSignal->numChannels; chan++) {
		outPtr = outSignal->channel[chan];
		inPtr = data + process->timeIndex - PROCESS_START_TIME + chan;
		for (i = 0; i < signalLength; i++, inPtr += outSignal->numChannels)
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
	static wxChar	*funcName = wxT("InitResultEarObject");

	if (dt <= 0.0) {
		NotifyError(wxT("%s: dt must be greater than zero."), funcName);
		return(false);
	}
	if (inputProcess)
		Free_EarObject(&inputProcess); 	/* Just in case */
	if ((inputProcess = Init_EarObject(wxT("NULL"))) == NULL) {
		NotifyError(wxT("%s: Could not initialise data results earObject."),
		  funcName);
		return(false);
	}
	if (!InitOutSignal_EarObject(inputProcess, numChannels, segmentLength,
	  dt)) {
		NotifyError(wxT("%s: Cannot initialise input process"), funcName);
		return(false);
	}
	SignalDataPtr	outSignal = _OutSig_EarObject(inputProcess);
	SetStaticTimeFlag_SignalData(outSignal, staticTimeFlag);
	SetLocalInfoFlag_SignalData(outSignal, TRUE);
	SetInterleaveLevel_SignalData(outSignal, interleaveLevel);
	outSignal->rampFlag = TRUE;
	SetInputProcessData(inputProcess, segmentLength, inputData);
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
	static const wxChar *funcName = wxT("RunSimulation");
	bool	ok = true;
	int		i;
	ChanLen	segmentLength = 0;
	
	GetPtr_AppInterface()->updateProcessVariablesFlag = TRUE;
	if (!ResetSimulation()) {
		NotifyError(wxT("%s: Could not set the simulation\n"), funcName);
		return(false);
	}
	if (autoNumRunsMode) {
		if (!AutoSetNumberOfRuns(dt))
			ok = FALSE;
		else
			segmentLength = (ChanLen) floor(segmentDuration  / dt + 0.5);
	} else
		segmentLength = length;
	EarObjectPtr	audModel = GetSimProcess_AppInterface();
	if (ok && inputData) {
		if (!InitInputEarObject(segmentLength)) {
			NotifyError(wxT("Could not initialise input process."));
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
				SetInputProcessData(inputProcess, segmentLength, inputData);
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
	static const wxChar *funcName = wxT("MatMainApp::RunSimulationRemote");

	if (!CreateClient(serverHost, serverPort))
		return(false);
	RemoveCommands(MAINAPP_NUM_BASE_ARGUMENTS - 1, DSAMMAT_COMMAND_PREFIX);
	if (!GetClient()->SendArguments(GetArgc(), GetArgv())) {
		NotifyError(wxT("%s: Could not initialise remote simulation."),
		  funcName);
		return(false);
	}
	if (!GetClient()->InitSimFromFile(GetPtr_AppInterface()->simulationFile)) {
		NotifyError(wxT("%s: Could not initialise remote simulation."),
		  funcName);
		return(false);
	}
#	if DSAM_DEBUG
	for (int i = 0; i < GetArgc(); i++)
		printf("%s: %2d: %s\n", funcName, i, GetArgv()[i]);
#	endif /* DSAM_DEBUG */
	if (inputData) {
		if (!InitInputEarObject(length)) {
			NotifyError(wxT("%s: Could not initialise input process."),
			  funcName);
			return(false);
		}
		if (!GetClient()->GetIPCUtils()->InitOutProcess()) {
			NotifyError(wxT("%s: Could not initialise output process for input "
			  "data."), funcName);
			return(false);
		}
		GetClient()->GetIPCUtils()->ConnectToOutProcess(inputProcess);
		if (!GetClient()->SendInputProcess()) {
			NotifyError(wxT("%s: Could not send the input data."), funcName);
			return(false);
		}
	}
	if (!GetClient()->RunSimulation()) {
		NotifyError(wxT("%s: Could not run remote simulation."), funcName);
		return(false);
	}
	GetClient()->GetAllOutputFiles();
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
		return((GetClient())? GetClient()->GetSimProcess(): NULL);
	return(GetSimProcess_AppInterface());

}

/****************************** Main ******************************************/

/*
 * This function runs the simulation process .
 */

int
MatMainApp::Main(void)
{
	static const wxChar *funcName = wxT("MatMainApp::Main");

	if (!InitMain(FALSE)) {
		 NotifyError(wxT("%s: Could not initialise the main program."),
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
	static const wxChar *funcName = wxT("InitApp_MatMainApp");

	if (!GetPtr_AppInterface() && !Init_AppInterface(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise the application interface."),
		  funcName);
		exit(1);
	}

	SetAppName_AppInterface(PROGRAM_NAME);
	SetAppVersion_AppInterface(RUNDSAMSIM_VERSION);
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
	static const wxChar *funcName = wxT("SetUniParList_MatMainApp");
	UniParPtr	pars;

	if ((*parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  DSAMMAT_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = (*parList)->pars;
	SetPar_UniParMgr(&pars[DSAMMATRIX_AUTONUMRUNSMODE], wxT(
	  "AUTO_NUM_RUNS_MODE"),
	  wxT("Auto-setting of the number of runs (matlab matrix or data files "
	  "only) ('on' or 'off')."),
	  UNIPAR_BOOL,
	  &matMainAppPtr->autoNumRunsMode, NULL,
	  (void * (*)) SetAutoNumRunsMode_MatMainApp);
	SetPar_UniParMgr(&pars[DSAMMATRIX_NUMBEROFRUNS], wxT("NUM_RUNS"),
	  wxT("Number of repeat runs, or segments/frames."),
	  UNIPAR_INT,
	  &matMainAppPtr->numberOfRuns, NULL,
	  (void * (*)) SetNumberOfRuns_MatMainApp);
	SetPar_UniParMgr(&pars[DSAMMATRIX_SEGMENTDURATION], wxT("SEGMENT_DURATION"),
	  wxT("Segment duration for automatic number of runs operation (segment "
	  "mode)."),
	  UNIPAR_REAL,
	  &matMainAppPtr->segmentDuration, NULL,
	  (void * (*)) SetSegmentDuration_MatMainApp);
	SetPar_UniParMgr(&pars[DSAMMATRIX_SERVERMODE], DSAMMAT_COMMAND_PREFIX
	  wxT("MODE"),
	  wxT("Enables the server mode to run simulations on a remote system ('on' "
	  "or 'off')."),
	  UNIPAR_BOOL,
	  &matMainAppPtr->serverMode, NULL,
	  (void * (*)) SetServerMode_MatMainApp);
	SetPar_UniParMgr(&pars[DSAMMATRIX_SERVERHOST], DSAMMAT_COMMAND_PREFIX
	  wxT("HOST"),
	  wxT("Sets the remote server host - default is the local host."),
	  UNIPAR_STRING,
	  &matMainAppPtr->serverHost, NULL,
	  (void * (*)) SetServerHost_MatMainApp);
	SetPar_UniParMgr(&pars[DSAMMATRIX_SERVERPORT], DSAMMAT_COMMAND_PREFIX
	  wxT("PORT"),
	  wxT("Sets the remote server host port for connections."),
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
SetAutoNumRunsMode_MatMainApp(wxChar *theAutoNumRunsMode)
{
	static const wxChar	*funcName = PROGRAM_NAME wxT(": SetAutoNumRunsMode");
	int		specifier;

	if ((specifier = Identify_NameSpecifier(theAutoNumRunsMode,
	  BooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError(wxT("%s: Illegal auto. number of runs mode mode (%s): must "
		  "be 'on' or 'off'."), funcName, theAutoNumRunsMode);
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
	static const wxChar	*funcName = PROGRAM_NAME wxT(": SetNumberOfRuns");

	if (theNumberOfRuns < 0) {
		NotifyError(wxT("%s: Illegal number of runs (%d)."), funcName,
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
	static const wxChar *funcName = PROGRAM_NAME
	  wxT(": SetSegmentDuration_MatMainApp");

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
SetServerMode_MatMainApp(wxChar * theServerMode)
{
	static const wxChar	*funcName = wxT("SetServerMode_MatMainApp");
	int		specifier;

	if ((specifier = Identify_NameSpecifier(theServerMode,
		BooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError(wxT("%s: Illegal server mode (%s): must be 'on' or "
		  "'off'."), funcName, theServerMode);
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
SetServerHost_MatMainApp(wxChar *theServerHost)
{
	static const wxChar	*funcName = PROGRAM_NAME
	  wxT(": SetServerHost_MatMainApp");

	Snprintf_Utility_String(matMainAppPtr->serverHost, MAXLINE, wxT("%s"),
	  theServerHost);
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
	static const wxChar	*funcName = PROGRAM_NAME
	  wxT(": SetServerPort_MatMainApp");

	if (theServerPort < 0) {
		NotifyError(wxT("%s: Illegal port number (%d)."), funcName,
		  theServerPort);
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
	/*RegEntry_ModuleReg(wxT("BM_Carney2001"),
	  InitModule_BasilarM_Carney2001);*/
	return(TRUE);

}

/***************************** Notify *****************************************/

/*
 * All notification messages are stored in the notification list.
 * This list is reset when the noficiationCount is zero.
 */

void
Notify_MatMainApp(const wxChar *message, CommonDiagSpecifier type)
{
	FILE	*fp;

	switch (type) {
	case COMMON_ERROR_DIAGNOSTIC:
		fp = dSAM.errorsFile;
		break;
	case COMMON_WARNING_DIAGNOSTIC:
		fp = dSAM.warningsFile;
		break;
	default:
		fp = stdout;
	}
	fprintf(fp, wxConvUTF8.cWX2MB(message));
	fprintf(fp, "\n");


} /* NotifyMessage */

