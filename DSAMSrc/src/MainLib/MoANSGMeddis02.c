/**********************
 *
 * File:		MoANSGMeddis02.c
 * Purpose:		Auditory nerve spike generation program, based on vessicle
 *				release with a refractory time an recovery period.
 * Comments:	Written using ModuleProducer version 1.4.0 (Oct 10 2002).
 * Author:		L. P. O'Mard from a matlab routine from R. Meddis
 * Created:		08 Nov 2002
 * Updated:	
 * Copyright:	(c) 2002, CNBH, University of Essex.
 *
 *********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "FiParFile.h"
#include "UtRandom.h"
#include "MoANSGMeddis02.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

Meddis02SGPtr	meddis02SGPtr = NULL;

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
Free_ANSpikeGen_Meddis02(void)
{
	/* static const char	*funcName = "Free_ANSpikeGen_Meddis02";  */

	if (meddis02SGPtr == NULL)
		return(FALSE);
	FreeProcessVariables_ANSpikeGen_Meddis02();
	if (meddis02SGPtr->parList)
		FreeList_UniParMgr(&meddis02SGPtr->parList);
	if (meddis02SGPtr->parSpec == GLOBAL) {
		free(meddis02SGPtr);
		meddis02SGPtr = NULL;
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
Init_ANSpikeGen_Meddis02(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_ANSpikeGen_Meddis02";

	if (parSpec == GLOBAL) {
		if (meddis02SGPtr != NULL)
			Free_ANSpikeGen_Meddis02();
		if ((meddis02SGPtr = (Meddis02SGPtr) malloc(sizeof(Meddis02SG))) ==
		  NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (meddis02SGPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	meddis02SGPtr->parSpec = parSpec;
	meddis02SGPtr->updateProcessVariablesFlag = TRUE;
	meddis02SGPtr->ranSeedFlag = TRUE;
	meddis02SGPtr->numFibresFlag = TRUE;
	meddis02SGPtr->pulseDurationFlag = TRUE;
	meddis02SGPtr->pulseMagnitudeFlag = TRUE;
	meddis02SGPtr->refractoryPeriodFlag = TRUE;
	meddis02SGPtr->recoveryTauFlag = TRUE;
	meddis02SGPtr->ranSeed = 0;
	meddis02SGPtr->numFibres = 1;
	meddis02SGPtr->pulseDuration = -1.0;
	meddis02SGPtr->pulseMagnitude = 1.0;
	meddis02SGPtr->refractoryPeriod = 0.75e-3;
	meddis02SGPtr->recoveryTau = 0.8e-3;

	if (!SetUniParList_ANSpikeGen_Meddis02()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_ANSpikeGen_Meddis02();
		return(FALSE);
	}
	meddis02SGPtr->timer = NULL;
	meddis02SGPtr->remainingPulseTime = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_ANSpikeGen_Meddis02(void)
{
	static const char *funcName = "SetUniParList_ANSpikeGen_Meddis02";
	UniParPtr	pars;

	if ((meddis02SGPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  ANSPIKEGEN_MEDDIS02_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = meddis02SGPtr->parList->pars;
	SetPar_UniParMgr(&pars[ANSPIKEGEN_MEDDIS02_RANSEED], "RAN_SEED",
	  "Random number seed (0 produces a different seed each run).",
	  UNIPAR_LONG,
	  &meddis02SGPtr->ranSeed, NULL,
	  (void * (*)) SetRanSeed_ANSpikeGen_Meddis02);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_MEDDIS02_NUMFIBRES], "NUM_FIBRES",
	  "Number of parallel independent fibres contacting the same IHC.",
	  UNIPAR_INT,
	  &meddis02SGPtr->numFibres, NULL,
	  (void * (*)) SetNumFibres_ANSpikeGen_Meddis02);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_MEDDIS02_PULSEDURATION], "PULSE_DURATION",
	  "Excitary post-synaptic pulse duration (s).",
	  UNIPAR_REAL,
	  &meddis02SGPtr->pulseDuration, NULL,
	  (void * (*)) SetPulseDuration_ANSpikeGen_Meddis02);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_MEDDIS02_PULSEMAGNITUDE], "MAGNITUDE",
	  "Pulse magnitude (arbitrary units).",
	  UNIPAR_REAL,
	  &meddis02SGPtr->pulseMagnitude, NULL,
	  (void * (*)) SetPulseMagnitude_ANSpikeGen_Meddis02);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_MEDDIS02_REFRACTORYPERIOD],
	  "REFRAC_PERIOD",
	  "Absolute refractory period (s).",
	  UNIPAR_REAL,
	  &meddis02SGPtr->refractoryPeriod, NULL,
	  (void * (*)) SetRefractoryPeriod_ANSpikeGen_Meddis02);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_MEDDIS02_RECOVERYTAU], "RECOVERY_TAU",
	  "Recovery time constant, tau (s).",
	  UNIPAR_REAL,
	  &meddis02SGPtr->recoveryTau, NULL,
	  (void * (*)) SetRecoveryTau_ANSpikeGen_Meddis02);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_ANSpikeGen_Meddis02(void)
{
	static const char	*funcName = "GetUniParListPtr_ANSpikeGen_Meddis02";

	if (meddis02SGPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (meddis02SGPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(meddis02SGPtr->parList);

}

/****************************** SetRanSeed ************************************/

/*
 * This function sets the module's ranSeed parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRanSeed_ANSpikeGen_Meddis02(long theRanSeed)
{
	static const char	*funcName = "SetRanSeed_ANSpikeGen_Meddis02";

	if (meddis02SGPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	meddis02SGPtr->ranSeedFlag = TRUE;
	meddis02SGPtr->updateProcessVariablesFlag = TRUE;
	meddis02SGPtr->ranSeed = theRanSeed;
	return(TRUE);

}

/****************************** SetNumFibres **********************************/

/*
 * This function sets the module's numFibres parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNumFibres_ANSpikeGen_Meddis02(int theNumFibres)
{
	static const char	*funcName = "SetNumFibres_ANSpikeGen_Meddis02";

	if (meddis02SGPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (theNumFibres < 1) {
		NotifyError("%s: Illegal no. of fibres (%d).", funcName, theNumFibres);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	meddis02SGPtr->numFibresFlag = TRUE;
	meddis02SGPtr->updateProcessVariablesFlag = TRUE;
	meddis02SGPtr->numFibres = theNumFibres;
	return(TRUE);

}

/****************************** SetPulseDuration ******************************/

/*
 * This function sets the module's pulseDuration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPulseDuration_ANSpikeGen_Meddis02(double thePulseDuration)
{
	static const char	*funcName = "SetPulseDuration_ANSpikeGen_Meddis02";

	if (meddis02SGPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	meddis02SGPtr->pulseDurationFlag = TRUE;
	meddis02SGPtr->updateProcessVariablesFlag = TRUE;
	meddis02SGPtr->pulseDuration = thePulseDuration;
	return(TRUE);

}

/****************************** SetPulseMagnitude *****************************/

/*
 * This function sets the module's pulseMagnitude parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPulseMagnitude_ANSpikeGen_Meddis02(double thePulseMagnitude)
{
	static const char	*funcName = "SetPulseMagnitude_ANSpikeGen_Meddis02";

	if (meddis02SGPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	meddis02SGPtr->pulseMagnitudeFlag = TRUE;
	meddis02SGPtr->updateProcessVariablesFlag = TRUE;
	meddis02SGPtr->pulseMagnitude = thePulseMagnitude;
	return(TRUE);

}

/****************************** SetRefractoryPeriod ***************************/

/*
 * This function sets the module's refractoryPeriod parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRefractoryPeriod_ANSpikeGen_Meddis02(double theRefractoryPeriod)
{
	static const char	*funcName = "SetRefractoryPeriod_ANSpikeGen_Meddis02";

	if (meddis02SGPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (theRefractoryPeriod < 0.0) {
		NotifyError("%s: Refractory period must be greater than zero.\n",
		  funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	meddis02SGPtr->refractoryPeriodFlag = TRUE;
	meddis02SGPtr->updateProcessVariablesFlag = TRUE;
	meddis02SGPtr->refractoryPeriod = theRefractoryPeriod;
	return(TRUE);

}

/****************************** SetRecoveryTau ********************************/

/*
 * This function sets the module's recoveryTau parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRecoveryTau_ANSpikeGen_Meddis02(double theRecoveryTau)
{
	static const char	*funcName = "SetRecoveryTau_ANSpikeGen_Meddis02";

	if (meddis02SGPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	meddis02SGPtr->recoveryTauFlag = TRUE;
	meddis02SGPtr->updateProcessVariablesFlag = TRUE;
	meddis02SGPtr->recoveryTau = theRecoveryTau;
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
CheckPars_ANSpikeGen_Meddis02(void)
{
	static const char	*funcName = "CheckPars_ANSpikeGen_Meddis02";
	BOOLN	ok;

	ok = TRUE;
	if (meddis02SGPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!meddis02SGPtr->ranSeedFlag) {
		NotifyError("%s: ranSeed variable not set.", funcName);
		ok = FALSE;
	}
	if (!meddis02SGPtr->numFibresFlag) {
		NotifyError("%s: numFibres variable not set.", funcName);
		ok = FALSE;
	}
	if (!meddis02SGPtr->pulseDurationFlag) {
		NotifyError("%s: pulseDuration variable not set.", funcName);
		ok = FALSE;
	}
	if (!meddis02SGPtr->pulseMagnitudeFlag) {
		NotifyError("%s: pulseMagnitude variable not set.", funcName);
		ok = FALSE;
	}
	if (!meddis02SGPtr->refractoryPeriodFlag) {
		NotifyError("%s: refractoryPeriod variable not set.", funcName);
		ok = FALSE;
	}
	if (!meddis02SGPtr->recoveryTauFlag) {
		NotifyError("%s: recoveryTau variable not set.", funcName);
		ok = FALSE;
	}
	return(ok);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the lUTEar.parsFile file pointer.
 */

BOOLN
PrintPars_ANSpikeGen_Meddis02(void)
{
	static const char	*funcName = "PrintPars_ANSpikeGen_Meddis02";

	if (!CheckPars_ANSpikeGen_Meddis02()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Meddis 2002 post-synaptic firing module Parameters:-\n");
	DPrint("\tRandom number seed = %ld,", meddis02SGPtr->ranSeed);
	DPrint("\tNumber of fibres = %d,\n", meddis02SGPtr->numFibres);
	DPrint("\tPulse duration = ");
	if (meddis02SGPtr->pulseDuration > 0.0)
		DPrint("%g ms,", MSEC(meddis02SGPtr->pulseDuration));
	else
		DPrint("< prev. signal dt>,");
	DPrint("\tPulse magnitude = %g (nA?),\n", meddis02SGPtr->pulseMagnitude);
	DPrint("\tAbsolute refractory period = %g (s),",
	  meddis02SGPtr->refractoryPeriod);
	DPrint("\tRecovery time constant, tau = %g (s)\n",
	  meddis02SGPtr->recoveryTau);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_ANSpikeGen_Meddis02(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_ANSpikeGen_Meddis02";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	meddis02SGPtr = (Meddis02SGPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 * It also initialises the process structure.
 */

BOOLN
InitModule_ANSpikeGen_Meddis02(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_ANSpikeGen_Meddis02";

	if (!SetParsPointer_ANSpikeGen_Meddis02(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_ANSpikeGen_Meddis02(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = meddis02SGPtr;
	theModule->CheckPars = CheckPars_ANSpikeGen_Meddis02;
	theModule->Free = Free_ANSpikeGen_Meddis02;
	theModule->GetUniParListPtr = GetUniParListPtr_ANSpikeGen_Meddis02;
	theModule->PrintPars = PrintPars_ANSpikeGen_Meddis02;
	theModule->RunProcess = RunModel_ANSpikeGen_Meddis02;
	theModule->SetParsPointer = SetParsPointer_ANSpikeGen_Meddis02;
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
CheckData_ANSpikeGen_Meddis02(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_ANSpikeGen_Meddis02";

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	if ((meddis02SGPtr->pulseDuration > 0.0) && (meddis02SGPtr->pulseDuration <
	  data->inSignal[0]->dt)) {
		NotifyError("%s: Pulse duration is too small for sampling\n"
		  "interval, %g ms (%g ms)\n", funcName,
		  MSEC(data->inSignal[0]->dt), MSEC(meddis02SGPtr->pulseDuration));
		return(FALSE);
	}
	/*** Put additional checks here. ***/
	return(TRUE);

}

/****************************** InitProcessVariables **************************/

/*
 * This function allocates the memory for the process variables.
 * It assumes that all of the parameters for the module have been
 * correctly initialised.
 */

BOOLN
InitProcessVariables_ANSpikeGen_Meddis02(EarObjectPtr data)
{
	static const char	*funcName = "InitProcessVariables_ANSpikeGen_Meddis02";
	int		i, arrayLength;
	double	timeGreaterThanRefractoryPeriod;

	if (meddis02SGPtr->updateProcessVariablesFlag || data->updateProcessFlag ||
	  (data->timeIndex == PROCESS_START_TIME)) {
		arrayLength = data->outSignal->numChannels * meddis02SGPtr->numFibres;
		if (meddis02SGPtr->updateProcessVariablesFlag ||
		  data->updateProcessFlag) {
			FreeProcessVariables_ANSpikeGen_Meddis02();
			if ((meddis02SGPtr->timer = (double *) calloc(arrayLength, sizeof(
			  double))) == NULL) {
			 	NotifyError("%s: Out of memory for timer array.", funcName);
			 	return(FALSE);
			}
			if ((meddis02SGPtr->remainingPulseTime = (double *) calloc(
			  arrayLength, sizeof(double))) == NULL) {
			 	NotifyError("%s: Out of memory for remainingPulseTime array.",
			 	  funcName);
			 	return(FALSE);
			}
			meddis02SGPtr->updateProcessVariablesFlag = FALSE;
		}
		timeGreaterThanRefractoryPeriod = meddis02SGPtr->refractoryPeriod +
		  data->outSignal->dt;
		for (i = 0; i < arrayLength; i++) {
			meddis02SGPtr->timer[i] = timeGreaterThanRefractoryPeriod;
			meddis02SGPtr->remainingPulseTime[i] = 0.0;
		}
	}
	return(TRUE);

}

/****************************** FreeProcessVariables **************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

BOOLN
FreeProcessVariables_ANSpikeGen_Meddis02(void)
{
	if (meddis02SGPtr->timer) {
		free(meddis02SGPtr->timer);
		meddis02SGPtr->timer = NULL;
	}
	if (meddis02SGPtr->remainingPulseTime) {
		free(meddis02SGPtr->remainingPulseTime);
		meddis02SGPtr->remainingPulseTime = NULL;
	}
	meddis02SGPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** RunModel **************************************/

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
RunModel_ANSpikeGen_Meddis02(EarObjectPtr data)
{
	static const char	*funcName = "RunModel_ANSpikeGen_Meddis02";
	register ChanData	 *inPtr, *outPtr;
	register	double		*timerPtr, *remainingPulseTimePtr;
	int		i, chan;
	double	dt, spikeProb, excessTime, pulseDuration;
	ChanLen	j;
	Meddis02SGPtr	p = meddis02SGPtr;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckPars_ANSpikeGen_Meddis02())
		return(FALSE);
	if (!CheckData_ANSpikeGen_Meddis02(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "Meddis 2002 Post-Synaptic Spike Firing");
	if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels,
	  data->inSignal[0]->length, data->inSignal[0]->dt)) {
		NotifyError("%s: Could not initialise output signal.", funcName);
		return(FALSE);
	}
	if (!InitProcessVariables_ANSpikeGen_Meddis02(data)) {
		NotifyError("%s: Could not initialise the process variables.",
		  funcName);
		return(FALSE);
	}
	dt = data->inSignal[0]->dt;
	pulseDuration = (p->pulseDuration > 0.0)? p->pulseDuration: dt;
	for (chan = 0; chan < data->outSignal->numChannels; chan++) {
		outPtr = data->outSignal->channel[chan];
		for (j = 0; j < data->outSignal->length; j++)
			*outPtr++ = 0.0;
	}
	for (i = 0, timerPtr = p->timer, remainingPulseTimePtr =
	  p->remainingPulseTime; i < p->numFibres; i++)
		for (chan = 0; chan < data->outSignal->numChannels; chan++) {
			inPtr = data->inSignal[0]->channel[chan];
			outPtr = data->outSignal->channel[chan];
			for (j = 0; j < data->outSignal->length; j++) {
				if ((*inPtr > 0.0) && (*timerPtr > p->refractoryPeriod)) {
					excessTime = *timerPtr - p->refractoryPeriod;
					spikeProb = 1.0 - exp(-excessTime / p->recoveryTau);
					if (spikeProb > Ran01_Random(&p->ranSeed)) {
						*remainingPulseTimePtr = pulseDuration;
						*timerPtr = 0.0;
					}
				}
				if (*remainingPulseTimePtr > 0.0) {
					*outPtr += p->pulseMagnitude;
					*remainingPulseTimePtr -= dt;
				}
				*timerPtr += dt;
				inPtr++;
				outPtr++;
			}
			timerPtr++;
			remainingPulseTimePtr++;
		}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

