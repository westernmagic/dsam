/**********************
 *
 * File:		DSAMMatrix.cpp
 * Purpose:		Utilities to set and handle the arguments usingmyArgc and
 *				myArgv.
 * Comments:	This was create for use by the scripting language interface
 *				RunDSAMSim.
 * Author:		L. P. O'Mard
 * Created:		18 Feb 2003
 * Updated:		
 * Copyright:	(c) 2003, University of Essex
 *
 *********************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GeCommon.h>

#include <DSAM.h>

#include "DSAMMatrix.h"

/******************************************************************************/
/*************************** Global Variables *********************************/
/******************************************************************************/

BOOLN	numberOfRunsFlag = FALSE, segmentDurationFlag = FALSE;
int		myArgc = 0, numberOfRuns = 1;
char	**myArgv = NULL;
double	segmentDuration = -1.0;
EarObjectPtr	inputProcess = NULL;

/******************************************************************************/
/*************************** Subroutines and functions ************************/
/******************************************************************************/

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the programs universal parameter
 * list. This list provides universal access to the program's
 * parameters.  It needs to be called from an initialise routine.
 */

BOOLN
SetUniParList_DSAMMatrix(UniParListPtr *parList)
{
	static const char *funcName = "SetUniParList";
	UniParPtr	pars;

	if ((*parList = InitList_UniParMgr(UNIPAR_SET_GENERAL, DSAMMAT_NUM_PARS,
	  NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = (*parList)->pars;
	SetPar_UniParMgr(&pars[DSAMMATRIX_NUMBEROFRUNS], "NUM_RUNS",
	  "Number of repeat runs, or segments/frames.",
	  UNIPAR_INT,
	  &numberOfRuns, NULL,
	  (void * (*)) SetNumberOfRuns_DSAMMatrix);
	SetPar_UniParMgr(&pars[DSAMMATRIX_SEGMENTDURATION], "SEGMENT_DURATION",
	  "Segment duration for automatic number of runs operation (segment mode).",
	  UNIPAR_REAL,
	  &segmentDuration, NULL,
	  (void * (*)) SetSegmentDuration_DSAMMatrix);
	return(TRUE);

}

/****************************** AutoSetNumberOfRuns ***************************/

/*
 * This routine sets the number of runs according to a loaded simulation script
 * which has a 'DataFile_In' process at the beginning, if the 'inputProcess' is
 * set.
 */

BOOLN
AutoSetNumberOfRuns_DSAMMatrix(void)
{
	static char *funcName = PROGRAM_NAME": AutoSetNumberOfRuns_DSAMMatrix";
	double	totalDuration;
	FILE	*savedErrorsFilePtr = GetDSAMPtr_Common()->errorsFile;
	EarObjectPtr	process;

	if (numberOfRuns > 0)
		return(TRUE);

	if (!GetDSAMPtr_Common()->segmentedMode) {
		numberOfRuns = 1;
		return(TRUE);
	}

	if (inputProcess)
		totalDuration = _GetDuration_SignalData(inputProcess->outSignal);
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
		  "duration (%g ms).", funcName, MILLI(segmentDuration), MILLI(
		  totalDuration));
		return(FALSE);
	}
	numberOfRuns = (int) floor(totalDuration / segmentDuration);
	return(TRUE);

}

/****************************** SetNumberOfRuns *******************************/

/*
 * This functions sets the number of simulation runs.
 * It returns false if it fails in any way.
 */

BOOLN
SetNumberOfRuns_DSAMMatrix(int theNumberOfRuns)
{
	static const char	*funcName = PROGRAM_NAME": SetNumberOfRuns";

	numberOfRuns = theNumberOfRuns;
	numberOfRunsFlag = TRUE;
	return(TRUE);

}

/****************************** SetSegmentDuration ****************************/

/*
 * This functions sets the segment duration which is used in conjunction with
 * the automatic setting of the number of runs in segment process mode.
 */

BOOLN
SetSegmentDuration_DSAMMatrix(double theSegmentDuration)
{
	static const char *funcName = PROGRAM_NAME": SetSegmentDuration_DSAMMatrix";

	segmentDuration = theSegmentDuration;
	segmentDurationFlag = TRUE;
	return(TRUE);

}

/****************************** InitArgv **************************************/

/*
 * This allocates the memory for the argv pointer.
 */

BOOLN
InitArgv_DSAMMatrix(int argc)
{
	static const char *funcName = "InitArgv_DSAMMatrix";

	if (!argc)
		return(TRUE);
	myArgc = argc;
	if ((myArgv = (char **) calloc(myArgc, sizeof(char *))) == NULL) {
		NotifyError("%s: Out of memory for myArgv[%d] array.", funcName,
		  myArgc);
		return(FALSE);
	}
	return(TRUE);

}

/****************************** SetArgvString *********************************/

BOOLN
SetArgvString_DSAMMatrix(int index, char *string, int size)
{
	static const char *funcName = "SetArgv_DSAMMatrix";
	
	if ((myArgv[index] = (char *) malloc(size + 1)) == NULL) {
		NotifyError("%s: Out of memory for myArgv[%d].", funcName, index);
		return(FALSE);
	}
	strcpy(myArgv[index], string);
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
SetParameterOptionArgs_DSAMMatrix(int indexStart, char *parameterOptions,
  BOOLN countOnly)
{
	static const char *funcName = "SetParameterOptionArgs_DSAMMatrix";
	char	*workStr, *token;
	int		count = 0;

	if ((workStr = (char *) malloc(strlen(parameterOptions) + 1)) == NULL) {
		NotifyError("%s: Out of memory for workStr.", funcName);
		return(-1);
	}
	strcpy(workStr, parameterOptions);
	token = strtok(workStr, PARAMETER_STR_DELIMITERS);
	while (token) {
		if (!countOnly)
			SetArgvString_DSAMMatrix(indexStart + count, token, strlen(token));
		count++;
		token = strtok(NULL, PARAMETER_STR_DELIMITERS);
	}
	free(workStr);
	return(count);
}
	
/****************************** SetArgStrings *********************************/

/*
 * This routine sets the argument strings using the global argc and argv
 * variables.
 */

BOOLN
SetArgStrings_DSAMMatrix(char *programName, char *simFile, char *diagMode,
  char *parameterOptions)
{
	static const char *funcName = "SetArgStrings_DSAMMatrix";
	int		numArgs = NUM_BASE_ARGUMENTS;

	if ((numArgs = SetParameterOptionArgs_DSAMMatrix(0, parameterOptions,
	  TRUE)) < 0) {
		NotifyError("%s: Could not count the parameter options", funcName);
		return(FALSE);
	}
	numArgs += NUM_BASE_ARGUMENTS;
	if (!InitArgv_DSAMMatrix(numArgs)) {
		NotifyError("%s: could not initialise arg strings.", funcName);
		return(FALSE);
	}
	if (!diagMode || (diagMode[0] == '\0'))
		diagMode = "off";
	SetArgvString_DSAMMatrix(0, programName, strlen(programName));
	SetArgvString_DSAMMatrix(1, "-d", 2);
	SetArgvString_DSAMMatrix(2, diagMode, strlen(diagMode));
	SetArgvString_DSAMMatrix(3, "-s", 2);
	SetArgvString_DSAMMatrix(4, simFile, strlen(simFile));
	SetParameterOptionArgs_DSAMMatrix(NUM_BASE_ARGUMENTS, parameterOptions,
	  FALSE);
	return(TRUE);

}

/****************************** InitInputEarObject ****************************/

/*
 * This function returns a pointer to an Earobject for providing input to a
 * simulation.
 * It uses the low level DSAM functions to create the EarObject, so that
 * unnecessary copies of the data need not be made.
 * This means that it cannot be free'd in the same way as a normal EarObject.
 */

EarObjectPtr
InitInputEarObject_DSAMMatrix(double *inputData, uShort numChannels,
  ChanLen length, double dt, BOOLN staticTimeFlag, double outputTimeOffset)
{
	static char	*funcName = "InitResultEarObject_DSAMMatrix";
	int		chan;
	EarObjectPtr	data;

	if (dt <= 0.0) {
		NotifyError("%s: dt must be greater than zero.", funcName);
		return(NULL);
	}
	if ((data = Init_EarObject("NULL")) == NULL) {
		NotifyError("%s: Could not initialise data results earObject.",
		  funcName);
		return(NULL);
	}
	data->outSignal = Init_SignalData(funcName);
	SetLength_SignalData(data->outSignal, length);
	SetSamplingInterval_SignalData(data->outSignal, dt);
	SetOutputTimeOffset_SignalData(data->outSignal, dt);
	data->outSignal->rampFlag = TRUE;
	if (!InitChannels_SignalData(data->outSignal, numChannels, TRUE)) {
		NotifyError("%s: Cannot initialise output channels for.", funcName);
		Free_SignalData(&data->outSignal);
		data->localOutSignalFlag = FALSE;
		Free_EarObject(&data);
		return(NULL);
	}
	data->localOutSignalFlag = TRUE;
	SetStaticTimeFlag_SignalData(data->outSignal, staticTimeFlag);
	SetLocalInfoFlag_SignalData(data->outSignal, TRUE);
	for (chan = 0; chan < data->outSignal->numChannels; chan++)
		data->outSignal->channel[chan] = inputData + length * chan;
	return(data);

}

/****************************** RegisterUserModules ***************************/

/*
 * This routine registers my user modules.
 */

BOOLN
RegisterUserModules_DSAMMatrix(void)
{
	/*RegEntry_ModuleReg("BM_Carney2001", InitModule_BasilarM_Carney2001); */
	return(TRUE);

}

/****************************** Init ******************************************/

/*
 * This routine carries out general initialisation tasks.
 * It sets the initialParameter global variable.
 */

BOOLN
Init_DSAMMatrix(void)
{
	static char *funcName = "Init_DSAMMatrix";

	if (!GetPtr_AppInterface() && !Init_AppInterface(GLOBAL)) {
		NotifyError("%s: Could not initialise the application interface.",
		  funcName);
		exit(1);
	}

	SetAppName_AppInterface(PROGRAM_NAME);
	SetAppVersion_AppInterface(DSAMMAT_VERSION);
	SetCompiledDSAMVersion_AppInterface(DSAM_VERSION);

	SetAppSetUniParList_AppInterface(SetUniParList_DSAMMatrix);

	//SetAppPrintUsage_AppInterface(PrintUsage);
	//SetAppProcessOptions_AppInterface(ProcessOptions);
	//SetAppPostInitFunc_AppInterface(PostInitFunc);

	SetAppRegisterUserModules_AppInterface(RegisterUserModules_DSAMMatrix);
	return(TRUE);

}

/****************************** FreeArgStrings ********************************/

/*
 * This routine frees the argument strings.
 */

void
FreeArgStrings_DSAMMatrix(void)
{
	int		i;

	if (myArgv) {
		for (i = 0; i < myArgc; i++)
			if (myArgv)
				free(myArgv[i]);
		free(myArgv);
	}

}

/****************************** Free ******************************************/

/*.
 */

void
Free_DSAMMatrix(void)
{
	if (inputProcess)
		Free_EarObject(&inputProcess);
	Free_AppInterface();

}

/****************************** RunSimulation *********************************/

/*
 * This function runs the simulation process using the DSAM AppInterface API.
 * The 'inputProcess' EarObject cannot be freed here as it may be used later.
 */

EarObjectPtr
RunSimulation_DSAMMatrix(char *programName, char *simFile, char *diagMode,
  char *parameterOptions, double *inputData, uShort numChannels, ChanLen length,
  double dt, BOOLN staticTimeFlag, double outputTimeOffset)
{
	static const char *funcName = "RunSimulation_DSAMMatrix";
	BOOLN	ok = TRUE;
	int		i;
	EarObjectPtr	audModel;

	if (!SetArgStrings_DSAMMatrix(programName, simFile, diagMode,
	  parameterOptions)) {
		NotifyError("%s: Could not set argument strings.", funcName);
		return(NULL);
	}
	
//	for (int i = 0; i < myArgc; i++)
//		printf("argv[%d] = '%s'\n", i, myArgv[i]);
	if (!InitProcessVariables_AppInterface(Init_DSAMMatrix, myArgc, myArgv)) {
		NotifyError("RunDSAMSim: Could not initialise process variables.");
		return(NULL);
	}
	audModel = GetSimProcess_AppInterface();

	if (inputData) {
		if (inputProcess)
			Free_EarObject(&inputProcess); 	/* Just in case */
		if ((inputProcess = InitInputEarObject_DSAMMatrix(inputData,
		  numChannels, length, dt, staticTimeFlag, outputTimeOffset)) == NULL) {
			NotifyError("Could not initialise input process.");
			return(NULL);
		}
		ConnectOutSignalToIn_EarObject(inputProcess, audModel);
	}

	if ((numberOfRuns < 1) && !AutoSetNumberOfRuns_DSAMMatrix())
		ok = FALSE;
	if (ok) {
		PrintPars_ModuleMgr(audModel);
		ResetSim_AppInterface();
		for (i = 0; i < numberOfRuns; i++)
			if (!RunSim_AppInterface()) {
				ok = FALSE;
				break;
			}
	}	
	if (inputProcess)
		DisconnectOutSignalFromIn_EarObject(inputProcess, audModel);
	FreeArgStrings_DSAMMatrix();
	return((ok)? audModel: NULL);

}

