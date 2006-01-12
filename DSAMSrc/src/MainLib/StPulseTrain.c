/**********************
 *
 * File:		StPulseTrain.c
 * Purpose:		The pulse train stimulus generation module.
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 *				21-11-97 LPO: Amended routine so that the first pulse occurrs
 *				in the first sample of the output.
 * Author:		L. P. O'Mard
 * Created:		04 Mar 1996
 * Updated:		21 Nov 1997
 * Copyright:	(c) 1997, University of Essex
 *
 *********************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "FiParFile.h"
#include "StPulseTrain.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

PulseTrainPtr	pulseTrainPtr = NULL;

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
Free_PulseTrain(void)
{
	/* static const char	*funcName = "Free_PulseTrain"; */

	if (pulseTrainPtr == NULL)
		return(FALSE);
	if (pulseTrainPtr->parList)
		FreeList_UniParMgr(&pulseTrainPtr->parList);
	if (pulseTrainPtr->parSpec == GLOBAL) {
		free(pulseTrainPtr);
		pulseTrainPtr = NULL;
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
Init_PulseTrain(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_PulseTrain";

	if (parSpec == GLOBAL) {
		if (pulseTrainPtr != NULL)
			Free_PulseTrain();
		if ((pulseTrainPtr = (PulseTrainPtr) malloc(sizeof(PulseTrain))) ==
		  NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (pulseTrainPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	pulseTrainPtr->parSpec = parSpec;
	pulseTrainPtr->pulseRateFlag = TRUE;
	pulseTrainPtr->pulseDurationFlag = TRUE;
	pulseTrainPtr->amplitudeFlag = TRUE;
	pulseTrainPtr->durationFlag = TRUE;
	pulseTrainPtr->dtFlag = TRUE;
	pulseTrainPtr->pulseRate = 360.0;
	pulseTrainPtr->pulseDuration = 0.1e-3;
	pulseTrainPtr->amplitude = 3.4e-7;
	pulseTrainPtr->duration = 0.1;
	pulseTrainPtr->dt = DEFAULT_DT;

	if (!SetUniParList_PulseTrain()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_PulseTrain();
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
SetUniParList_PulseTrain(void)
{
	static const char *funcName = "SetUniParList_PulseTrain";
	UniParPtr	pars;

	if ((pulseTrainPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  PULSETRAIN_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = pulseTrainPtr->parList->pars;
	SetPar_UniParMgr(&pars[PULSETRAIN_PULSERATE], "RATE",
	  "Pulse rate (pulses/s)",
	  UNIPAR_REAL,
	  &pulseTrainPtr->pulseRate, NULL,
	  (void * (*)) SetPulseRate_PulseTrain);
	SetPar_UniParMgr(&pars[PULSETRAIN_PULSEDURATION], "PULSE_DURATION",
	  "Pulse duration (s).",
	  UNIPAR_REAL,
	  &pulseTrainPtr->pulseDuration, NULL,
	  (void * (*)) SetPulseDuration_PulseTrain);
	SetPar_UniParMgr(&pars[PULSETRAIN_AMPLITUDE], "AMPLITUDE",
	  "Amplitude (arbitary units).",
	  UNIPAR_REAL,
	  &pulseTrainPtr->amplitude, NULL,
	  (void * (*)) SetAmplitude_PulseTrain);
	SetPar_UniParMgr(&pars[PULSETRAIN_DURATION], "DURATION",
	  "Duration (s).",
	  UNIPAR_REAL,
	  &pulseTrainPtr->duration, NULL,
	  (void * (*)) SetDuration_PulseTrain);
	SetPar_UniParMgr(&pars[PULSETRAIN_SAMPLINGINTERVAL], "DT",
	  "Sampling interval, dt (s).",
	  UNIPAR_REAL,
	  &pulseTrainPtr->dt, NULL,
	  (void * (*)) SetSamplingInterval_PulseTrain);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_PulseTrain(void)
{
	static const char	*funcName = "GetUniParListPtr_PulseTrain";

	if (pulseTrainPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (pulseTrainPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(pulseTrainPtr->parList);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_PulseTrain(double pulseRate, double
  pulseDuration, double amplitude, double duration, double samplingInterval)
{
	static const char	*funcName = "SetPars_PulseTrain";
	BOOLN	ok;

	ok = TRUE;
	if (!SetPulseRate_PulseTrain(pulseRate))
		ok = FALSE;
	if (!SetPulseDuration_PulseTrain(pulseDuration))
		ok = FALSE;
	if (!SetAmplitude_PulseTrain(amplitude))
		ok = FALSE;
	if (!SetDuration_PulseTrain(duration))
		ok = FALSE;
	if (!SetSamplingInterval_PulseTrain(samplingInterval))
		ok = FALSE;
	if (!ok)
		NotifyError("%s: Failed to set all module parameters." ,funcName);
	return(ok);

}

/****************************** SetPulseRate **********************************/

/*
 * This function sets the module's pulseRate parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPulseRate_PulseTrain(double thePulseRate)
{
	static const char	*funcName = "SetPulseRate_PulseTrain";

	if (pulseTrainPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (thePulseRate < DBL_EPSILON) {
		NotifyError("%s: Illegal value (%g).", funcName, thePulseRate);
		return(FALSE);
	}
	pulseTrainPtr->pulseRateFlag = TRUE;
	pulseTrainPtr->pulseRate = thePulseRate;
	return(TRUE);

}

/****************************** SetPulseDuration ******************************/

/*
 * This function sets the module's pulseDuration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPulseDuration_PulseTrain(double thePulseDuration)
{
	static const char	*funcName = "SetPulseDuration_PulseTrain";

	if (pulseTrainPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (thePulseDuration < DBL_EPSILON) {
		NotifyError("%s: Illegal value (%g).", funcName, thePulseDuration);
		return(FALSE);
	}
	pulseTrainPtr->pulseDurationFlag = TRUE;
	pulseTrainPtr->pulseDuration = thePulseDuration;
	return(TRUE);

}

/****************************** SetAmplitude **********************************/

/*
 * This function sets the module's amplitude parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetAmplitude_PulseTrain(double theAmplitude)
{
	static const char	*funcName = "SetAmplitude_PulseTrain";

	if (pulseTrainPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	pulseTrainPtr->amplitudeFlag = TRUE;
	pulseTrainPtr->amplitude = theAmplitude;
	return(TRUE);

}

/****************************** SetDuration ***********************************/

/*
 * This function sets the module's duration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDuration_PulseTrain(double theDuration)
{
	static const char	*funcName = "SetDuration_PulseTrain";

	if (pulseTrainPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (theDuration < DBL_EPSILON) {
		NotifyError("%s: Illegal value (%g).", funcName, theDuration);
		return(FALSE);
	}
	pulseTrainPtr->durationFlag = TRUE;
	pulseTrainPtr->duration = theDuration;
	return(TRUE);

}

/****************************** SetSamplingInterval ***************************/

/*
 * This function sets the module's samplingInterval parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSamplingInterval_PulseTrain(double theSamplingInterval)
{
	static const char *funcName ="SetSamplingInterval_PulseTrain";

	if (pulseTrainPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	pulseTrainPtr->dtFlag = TRUE;
	pulseTrainPtr->dt = theSamplingInterval;
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
CheckPars_PulseTrain(void)
{
	static const char	*funcName = "CheckPars_PulseTrain";
	BOOLN	ok;

	ok = TRUE;
	if (pulseTrainPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!pulseTrainPtr->pulseRateFlag) {
		NotifyError("%s: pulseRate variable not set.", funcName);
		ok = FALSE;
	}
	if (!pulseTrainPtr->pulseDurationFlag) {
		NotifyError("%s: pulseDuration variable not set.", funcName);
		ok = FALSE;
	}
	if (!pulseTrainPtr->amplitudeFlag) {
		NotifyError("%s: amplitude variable not set.", funcName);
		ok = FALSE;
	}
	if (!pulseTrainPtr->durationFlag) {
		NotifyError("%s: duration variable not set.", funcName);
		ok = FALSE;
	}
	if (!pulseTrainPtr->dtFlag) {
		NotifyError("%s: dt variable not set.", funcName);
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
PrintPars_PulseTrain(void)
{
	static const char	*funcName = "PrintPars_PulseTrain";

	if (!CheckPars_PulseTrain()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Pulse-Train Module Parameters:-\n");
	DPrint("\tPulse rate = %g /s,\t",
	  pulseTrainPtr->pulseRate);
	DPrint("\tPulse duration = %g ms\n",
	  MSEC(pulseTrainPtr->pulseDuration));
	DPrint("\tAmplitude = %g (arbitrary units),\n",
	  pulseTrainPtr->amplitude);
	DPrint("\tDuration = %g ms,\t",
	  MSEC(pulseTrainPtr->duration));
	DPrint("\tSampling interval = %g ms\n",
	  MSEC(pulseTrainPtr->dt));
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_PulseTrain(char *fileName)
{
	static const char	*funcName = "ReadPars_PulseTrain";
	BOOLN	ok;
	char	*filePath;
	double	pulseRate, pulseDuration, amplitude, duration, samplingInterval;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(filePath, "r")) == NULL) {
		NotifyError("%s: Cannot open data file '%s'.\n", funcName, filePath);
		return(FALSE);
	}
	DPrint("%s: Reading from '%s':\n", funcName, filePath);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, "%lf", &pulseRate))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &pulseDuration))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &amplitude))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &duration))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &samplingInterval))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "
		  "parameter file '%s'.", funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_PulseTrain(pulseRate, pulseDuration, amplitude,
	  duration, samplingInterval)) {
		NotifyError("%s: Could not set parameters.", funcName);
		return(FALSE);
	}
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_PulseTrain(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_PulseTrain";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	pulseTrainPtr = (PulseTrainPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_PulseTrain(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_PulseTrain";

	if (!SetParsPointer_PulseTrain(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_PulseTrain(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = pulseTrainPtr;
	theModule->CheckPars = CheckPars_PulseTrain;
	theModule->Free = Free_PulseTrain;
	theModule->GetUniParListPtr = GetUniParListPtr_PulseTrain;
	theModule->PrintPars = PrintPars_PulseTrain;
	theModule->ReadPars = ReadPars_PulseTrain;
	theModule->RunProcess = GenerateSignal_PulseTrain;
	theModule->SetParsPointer = SetParsPointer_PulseTrain;
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
CheckData_PulseTrain(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_PulseTrain";
	double	pulsePeriod;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	pulsePeriod = 1.0 / pulseTrainPtr->pulseRate;
	if (pulsePeriod <= pulseTrainPtr->pulseDuration) {
		NotifyError("%s: Pulse rate is too high for the pulse duration\n(rate "
		  "must be < %g /s)", funcName, 1.0 / pulseTrainPtr->pulseDuration);
		return(FALSE);
	}
	if (pulsePeriod / pulseTrainPtr->dt < 2.0) {
		NotifyError("%s: Pulse rate is too low for the sampling interval\n"
		  "(rate must be > %g /s)", funcName, 1.0 / (pulseTrainPtr->dt * 2.0));
		return(FALSE);
	}
	if (pulsePeriod > pulseTrainPtr->duration) {
		NotifyError("%s: Pulse rate is too low for the signal duration\n(rate "
		  "must be > %g /s)", funcName, 1.0 / pulseTrainPtr->duration);
		return(FALSE);
	}
	if (pulseTrainPtr->pulseDuration < pulseTrainPtr->dt) {
		NotifyError("%s: Pulse duration is less then sampling interval "
		  "( use > %g ms).", funcName, MSEC(pulseTrainPtr->pulseDuration));
		return(FALSE);
	}
	return(TRUE);

}

/****************************** GenerateSignal ********************************/

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
GenerateSignal_PulseTrain(EarObjectPtr data)
{
	static const char	*funcName = "GenerateSignal_PulseTrain";
	register	ChanData	 *outPtr;
	double		pulsePeriod, t;
	ChanLen		i;
	PulseTrainPtr	p = pulseTrainPtr;

	if (!data->threadRunFlag) {
		if (!CheckPars_PulseTrain())
			return(FALSE);
		if (!CheckData_PulseTrain(data)) {
			NotifyError("%s: Process data invalid.", funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, "Pulse-train stimulus");
		data->updateProcessFlag = TRUE;
		if ( !InitOutSignal_EarObject(data, 1, (ChanLen) floor(p->
		  duration / p->dt + 0.5), p->dt) ) {
			NotifyError("%s: Cannot initialise output signal", funcName);
			return(FALSE);
		}
		data->outSignal->rampFlag = TRUE; /* Do not ramp. */
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	pulsePeriod = 1.0 / p->pulseRate;
	if (data->timeIndex == PROCESS_START_TIME) {
		p->nextPulseTime = pulsePeriod;
		p->remainingPulseTime = p->pulseDuration;
	}
	outPtr = data->outSignal->channel[0];
	for (i = 0, t = (data->timeIndex + 1) * p->dt; i < data->outSignal->length;
	  i++, t += p->dt, outPtr++) {
		if (p->remainingPulseTime > 0.0) {
			*outPtr = p->amplitude;
			p->remainingPulseTime -= p->dt;
		} else {
			if (t >= p->nextPulseTime) {
				p->remainingPulseTime = p->pulseDuration;
				p->nextPulseTime += pulsePeriod;
			}
		}
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

