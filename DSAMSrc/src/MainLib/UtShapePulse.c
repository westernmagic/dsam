/**********************
 *
 * File:		UtShapePulse.c
 * Purpose:		This module turns a train of detected spikes into a pulse
 *				train with set magnitudes and durations.
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 * Author:		L. P. O'Mard
 * Created:		18th April 1996
 * Updated:	
 * Copyright:	(c) 1998, University of Essex.
 *
 *********************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "FiParFile.h"
#include "UtShapePulse.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

ShapePulsePtr	shapePulsePtr = NULL;

/******************************************************************************/
/****************************** Subroutines and functions *********************/
/******************************************************************************/

/****************************** Free ******************************************/

/*
 * This function releases of the memory allocated for the process
 * variables. It should be called when the module is no longer in use.
 * It is defined as returning a BOOLN value because the generic
 * module interface requires that a non-void value be returned.
 */

BOOLN
Free_Utility_ShapePulse(void)
{
	/* static const char	*funcName = "Free_Utility_ShapePulse"; */

	if (shapePulsePtr == NULL)
		return(FALSE);
	FreeProcessVariables_Utility_ShapePulse();
	if (shapePulsePtr->parList)
		FreeList_UniParMgr(&shapePulsePtr->parList);
	if (shapePulsePtr->parSpec == GLOBAL) {
		free(shapePulsePtr);
		shapePulsePtr = NULL;
	}
	return(TRUE);

}

/****************************** Init ******************************************/

/*
 * This function initialises the module by setting module's parameter
 * pointer structure.
 * The GLOBAL option is for hard programming - it sets the module's
 * pointer to the global parameter structure and initialises the
 * parameters. The LOCAL option is for generic programming - it
 * initialises the parameter structure currently pointed to by the
 * module's parameter pointer.
 */

BOOLN
Init_Utility_ShapePulse(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_Utility_ShapePulse";

	if (parSpec == GLOBAL) {
		if (shapePulsePtr != NULL)
			Free_Utility_ShapePulse();
		if ((shapePulsePtr = (ShapePulsePtr) malloc(sizeof(ShapePulse))) ==
		  NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (shapePulsePtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	shapePulsePtr->parSpec = parSpec;
	shapePulsePtr->updateProcessVariablesFlag = TRUE;
	shapePulsePtr->eventThresholdFlag = TRUE;
	shapePulsePtr->pulseDurationFlag = TRUE;
	shapePulsePtr->pulseMagnitudeFlag = TRUE;
	shapePulsePtr->eventThreshold = 0.0;
	shapePulsePtr->pulseDuration = 0.2e-3;
	shapePulsePtr->pulseMagnitude = 3.8;
	shapePulsePtr->remainingPulseTime = NULL;

	if (!SetUniParList_Utility_ShapePulse()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_Utility_ShapePulse();
		return(FALSE);
	}
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_Utility_ShapePulse(void)
{
	static const char *funcName = "SetUniParList_Utility_ShapePulse";
	UniParPtr	pars;

	if ((shapePulsePtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  UTILITY_SHAPEPULSE_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = shapePulsePtr->parList->pars;
	SetPar_UniParMgr(&pars[UTILITY_SHAPEPULSE_EVENTTHRESHOLD], "THRESHOLD",
	  "Event threshold (arbitrary units).",
	  UNIPAR_REAL,
	  &shapePulsePtr->eventThreshold, NULL,
	  (void * (*)) SetEventThreshold_Utility_ShapePulse);
	SetPar_UniParMgr(&pars[UTILITY_SHAPEPULSE_PULSEDURATION], "DURATION",
	  "Pulse duration (s).",
	  UNIPAR_REAL,
	  &shapePulsePtr->pulseDuration, NULL,
	  (void * (*)) SetPulseDuration_Utility_ShapePulse);
	SetPar_UniParMgr(&pars[UTILITY_SHAPEPULSE_PULSEMAGNITUDE], "MAGNITUDE",
	  "Pulse magnitude (arbitrary units).",
	  UNIPAR_REAL,
	  &shapePulsePtr->pulseMagnitude, NULL,
	  (void * (*)) SetPulseMagnitude_Utility_ShapePulse);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Utility_ShapePulse(void)
{
	static const char	*funcName = "GetUniParListPtr_Utility_ShapePulse";

	if (shapePulsePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (shapePulsePtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(shapePulsePtr->parList);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_Utility_ShapePulse(double eventThreshold, double
  pulseDuration, double pulseMagnitude)
{
	static const char	*funcName = "SetPars_Utility_ShapePulse";
	BOOLN	ok;

	ok = TRUE;
	if (!SetEventThreshold_Utility_ShapePulse(eventThreshold))
		ok = FALSE;
	if (!SetPulseDuration_Utility_ShapePulse(pulseDuration))
		ok = FALSE;
	if (!SetPulseMagnitude_Utility_ShapePulse(pulseMagnitude))
		ok = FALSE;
	if (!ok)
		NotifyError("%s: Failed to set all module parameters." ,funcName);
	return(ok);

}

/****************************** SetEventThreshold *****************************/

/*
 * This function sets the module's eventThreshold parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetEventThreshold_Utility_ShapePulse(double theEventThreshold)
{
	static const char	*funcName = "SetEventThreshold_Utility_ShapePulse";

	if (shapePulsePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	shapePulsePtr->eventThresholdFlag = TRUE;
	shapePulsePtr->eventThreshold = theEventThreshold;
	return(TRUE);

}

/****************************** SetPulseDuration ******************************/

/*
 * This function sets the module's pulseDuration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPulseDuration_Utility_ShapePulse(double thePulseDuration)
{
	static const char	*funcName = "SetPulseDuration_Utility_ShapePulse";

	if (shapePulsePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (thePulseDuration < 0.0) {
		NotifyError("%s: Pulse duration must be greater than zero.\n",
		  funcName);
		return(FALSE);
	}
	shapePulsePtr->pulseDurationFlag = TRUE;
	shapePulsePtr->pulseDuration = thePulseDuration;
	shapePulsePtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** SetPulseMagnitude *****************************/

/*
 * This function sets the module's pulseMagnitude parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPulseMagnitude_Utility_ShapePulse(double thePulseMagnitude)
{
	static const char	*funcName = "SetPulseMagnitude_Utility_ShapePulse";

	if (shapePulsePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	shapePulsePtr->pulseMagnitudeFlag = TRUE;
	shapePulsePtr->pulseMagnitude = thePulseMagnitude;
	return(TRUE);

}

/****************************** CheckPars *************************************/

/*
 * This routine checks that the necessary parameters for the module
 * have been correctly initialised.
 * Other 'operational' tests which can only be done when all
 * parameters are present, should also be carried out here.
 * It returns TRUE if there are no problems.
 */

BOOLN
CheckPars_Utility_ShapePulse(void)
{
	static const char	*funcName = "CheckPars_Utility_ShapePulse";
	BOOLN	ok;

	ok = TRUE;
	if (shapePulsePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!shapePulsePtr->eventThresholdFlag) {
		NotifyError("%s: eventThreshold variable not set.", funcName);
		ok = FALSE;
	}
	if (!shapePulsePtr->pulseDurationFlag) {
		NotifyError("%s: pulseDuration variable not set.", funcName);
		ok = FALSE;
	}
	if (!shapePulsePtr->pulseMagnitudeFlag) {
		NotifyError("%s: pulseMagnitude variable not set.", funcName);
		ok = FALSE;
	}
	return(ok);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the dSAM.parsFile file pointer.
 */

BOOLN
PrintPars_Utility_ShapePulse(void)
{
	static const char	*funcName = "PrintPars_Utility_ShapePulse";

	if (!CheckPars_Utility_ShapePulse()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Shape Pulse Utility Module Parameters:-\n");
	DPrint("\tEvent threshold = %g (arbitrary units),\n",
	  shapePulsePtr->eventThreshold);
	DPrint("\tPulse duration = %g ms,",
	  MSEC(shapePulsePtr->pulseDuration));
	DPrint("\tPulse magnitude = %g (nA?).\n",
	  shapePulsePtr->pulseMagnitude);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_Utility_ShapePulse(char *fileName)
{
	static const char	*funcName = "ReadPars_Utility_ShapePulse";
	BOOLN	ok;
	char	*filePath;
	double	eventThreshold, pulseDuration, pulseMagnitude;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(filePath, "r")) == NULL) {
		NotifyError("%s: Cannot open data file '%s'.\n", funcName, filePath);
		return(FALSE);
	}
	DPrint("%s: Reading from '%s':\n", funcName, filePath);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, "%lf", &eventThreshold))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &pulseDuration))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &pulseMagnitude))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "
		  "parameter file '%s'.", funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_Utility_ShapePulse(eventThreshold, pulseDuration,
	  pulseMagnitude)) {
		NotifyError("%s: Could not set parameters.", funcName);
		return(FALSE);
	}
	return(TRUE);

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 */

BOOLN
InitProcessVariables_Utility_ShapePulse(EarObjectPtr data)
{
	static const char *funcName = "InitProcessVariables_Utility_ShapePulse";
	int		i;
	ShapePulsePtr	p = shapePulsePtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag ||
	  (data->timeIndex == PROCESS_START_TIME)) {
		if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
			FreeProcessVariables_Utility_ShapePulse();
			if ((p->remainingPulseTime = (double *) calloc(data->outSignal->
			  numChannels, sizeof(double))) == NULL) {
			 	NotifyError("%s: Out of memory for remainingPulseTime array.",
			 	  funcName);
			 	return(FALSE);
			}
			p->updateProcessVariablesFlag = FALSE;
		}
		for (i = 0; i < data->outSignal->numChannels; i++)
			p->remainingPulseTime[i] = 0.0;
	}
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

void
FreeProcessVariables_Utility_ShapePulse(void)
{
	if (shapePulsePtr->remainingPulseTime) {
		free(shapePulsePtr->remainingPulseTime);
		shapePulsePtr->remainingPulseTime = NULL;
	}
	shapePulsePtr->updateProcessVariablesFlag = TRUE;

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Utility_ShapePulse(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_Utility_ShapePulse";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	shapePulsePtr = (ShapePulsePtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Utility_ShapePulse(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_Utility_ShapePulse";

	if (!SetParsPointer_Utility_ShapePulse(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_Utility_ShapePulse(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = shapePulsePtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->CheckPars = CheckPars_Utility_ShapePulse;
	theModule->Free = Free_Utility_ShapePulse;
	theModule->GetUniParListPtr = GetUniParListPtr_Utility_ShapePulse;
	theModule->PrintPars = PrintPars_Utility_ShapePulse;
	theModule->ReadPars = ReadPars_Utility_ShapePulse;
	theModule->RunProcess = Process_Utility_ShapePulse;
	theModule->SetParsPointer = SetParsPointer_Utility_ShapePulse;
	return(TRUE);

}

/****************************** CheckData *************************************/

/*
 * This routine checks that the 'data' EarObject and input signal are
 * correctly initialised.
 * It should also include checks that ensure that the module's
 * parameters are compatible with the signal parameters, i.e. dt is
 * not too small, etc...
 * The 'CheckRamp_SignalData()' can be used instead of the
 * 'CheckInit_SignalData()' routine if the signal must be ramped for
 * the process.
 */

BOOLN
CheckData_Utility_ShapePulse(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_Utility_ShapePulse";

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	if (shapePulsePtr->pulseDuration < data->inSignal[0]->dt) {
		NotifyError("%s: Pulse duration is too small for sampling\n"
		  "interval, %g ms (%g ms)\n", funcName,
		  MSEC(data->inSignal[0]->dt), MSEC(shapePulsePtr->pulseDuration));
		return(FALSE);
	}
	/*** Put additional checks here. ***/
	return(TRUE);

}

/****************************** Process ***************************************/

/*
 * This routine allocates memory for the output signal, if necessary,
 * and generates the signal into channel[0] of the signal data-set.
 * It checks that all initialisation has been correctly carried out by
 * calling the appropriate checking routines.
 * It can be called repeatedly with different parameter values if
 * required.
 * Stimulus generation only sets the output signal, the input signal
 * is not used.
 * With repeated calls the Signal memory is only allocated once, then
 * re-used.
 */

BOOLN
Process_Utility_ShapePulse(EarObjectPtr data)
{
	static const char	*funcName = "Process_Utility_ShapePulse";
	register	ChanData	*inPtr, *outPtr, lastValue;
	register	double		*remainingPulseTimePtr, dt;
	BOOLN	riseDetected;
	int		chan;
	ChanLen	j;

	if (!data->threadRunFlag) {
		if (!CheckPars_Utility_ShapePulse())
			return(FALSE);
		if (!CheckData_Utility_ShapePulse(data)) {
			NotifyError("%s: Process data invalid.", funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, "Shape Pulse Utility Process");
		if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels,
		  data->inSignal[0]->length, data->inSignal[0]->dt)) {
			NotifyError("%s: Could not initialise output signal.", funcName);
			return(FALSE);
		}
		if (!InitProcessVariables_Utility_ShapePulse(data)) {
			NotifyError("%s: Could not initialise the process variables.",
			  funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	dt = data->inSignal[0]->dt;
	remainingPulseTimePtr = shapePulsePtr->remainingPulseTime + data->
	  outSignal->offset;
	for (chan = data->outSignal->offset; chan < data->outSignal->numChannels;
	  chan++) {
		inPtr = data->inSignal[0]->channel[chan];
		outPtr = data->outSignal->channel[chan];
		riseDetected = FALSE;
		lastValue = *inPtr++;
		for (j = 1; j < data->outSignal->length; j++) {
			if (!riseDetected)
				riseDetected = (*inPtr > lastValue);
			else {
				if (*inPtr < lastValue) {
					riseDetected = FALSE;
					if (lastValue > shapePulsePtr->eventThreshold)
						*remainingPulseTimePtr = shapePulsePtr->pulseDuration;
				}
			}
			if (*remainingPulseTimePtr > 0.0) {
				*outPtr += shapePulsePtr->pulseMagnitude;
				*remainingPulseTimePtr -= dt;
			}
			lastValue = *inPtr++;
			outPtr++;
		}
		remainingPulseTimePtr++;
	}
	SetUtilityProcessContinuity_EarObject(data);
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

