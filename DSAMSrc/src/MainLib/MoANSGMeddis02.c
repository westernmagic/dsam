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
	static const WChar	*funcName = wxT("Init_ANSpikeGen_Meddis02");

	if (parSpec == GLOBAL) {
		if (meddis02SGPtr != NULL)
			Free_ANSpikeGen_Meddis02();
		if ((meddis02SGPtr = (Meddis02SGPtr) malloc(sizeof(Meddis02SG))) ==
		  NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"), funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (meddis02SGPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
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
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
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
	static const WChar *funcName = wxT("SetUniParList_ANSpikeGen_Meddis02");
	UniParPtr	pars;

	if ((meddis02SGPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  ANSPIKEGEN_MEDDIS02_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = meddis02SGPtr->parList->pars;
	SetPar_UniParMgr(&pars[ANSPIKEGEN_MEDDIS02_RANSEED], wxT("RAN_SEED"),
	  wxT("Random number seed (0 produces a different seed each run)."),
	  UNIPAR_LONG,
	  &meddis02SGPtr->ranSeed, NULL,
	  (void * (*)) SetRanSeed_ANSpikeGen_Meddis02);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_MEDDIS02_NUMFIBRES], wxT("NUM_FIBRES"),
	  wxT("Number of parallel independent fibres contacting the same IHC."),
	  UNIPAR_INT,
	  &meddis02SGPtr->numFibres, NULL,
	  (void * (*)) SetNumFibres_ANSpikeGen_Meddis02);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_MEDDIS02_PULSEDURATION], wxT(
	  "PULSE_DURATION"),
	  wxT("Excitary post-synaptic pulse duration (s)."),
	  UNIPAR_REAL,
	  &meddis02SGPtr->pulseDuration, NULL,
	  (void * (*)) SetPulseDuration_ANSpikeGen_Meddis02);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_MEDDIS02_PULSEMAGNITUDE], wxT(
	  "MAGNITUDE"),
	  wxT("Pulse magnitude (arbitrary units)."),
	  UNIPAR_REAL,
	  &meddis02SGPtr->pulseMagnitude, NULL,
	  (void * (*)) SetPulseMagnitude_ANSpikeGen_Meddis02);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_MEDDIS02_REFRACTORYPERIOD],
	  wxT("REFRAC_PERIOD"),
	  wxT("Absolute refractory period (s)."),
	  UNIPAR_REAL,
	  &meddis02SGPtr->refractoryPeriod, NULL,
	  (void * (*)) SetRefractoryPeriod_ANSpikeGen_Meddis02);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_MEDDIS02_RECOVERYTAU], wxT(
	  "RECOVERY_TAU"),
	  wxT("Recovery time constant, tau (s)."),
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
	static const WChar	*funcName = wxT("GetUniParListPtr_ANSpikeGen_Meddis02");

	if (meddis02SGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (meddis02SGPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
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
	static const WChar	*funcName = wxT("SetRanSeed_ANSpikeGen_Meddis02");

	if (meddis02SGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
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
	static const WChar	*funcName = wxT("SetNumFibres_ANSpikeGen_Meddis02");

	if (meddis02SGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theNumFibres < 1) {
		NotifyError(wxT("%s: Illegal no. of fibres (%d)."), funcName,
		  theNumFibres);
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
	static const WChar	*funcName = wxT("SetPulseDuration_ANSpikeGen_Meddis02");

	if (meddis02SGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
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
	static const WChar	*funcName = wxT(
	  "SetPulseMagnitude_ANSpikeGen_Meddis02");

	if (meddis02SGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
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
	static const WChar	*funcName = wxT(
	  "SetRefractoryPeriod_ANSpikeGen_Meddis02");

	if (meddis02SGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theRefractoryPeriod < 0.0) {
		NotifyError(wxT("%s: Refractory period must be greater than zero.\n"),
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
	static const WChar	*funcName = wxT("SetRecoveryTau_ANSpikeGen_Meddis02");

	if (meddis02SGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
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
	static const WChar	*funcName = wxT("CheckPars_ANSpikeGen_Meddis02");
	BOOLN	ok;

	ok = TRUE;
	if (meddis02SGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (!meddis02SGPtr->ranSeedFlag) {
		NotifyError(wxT("%s: ranSeed variable not set."), funcName);
		ok = FALSE;
	}
	if (!meddis02SGPtr->numFibresFlag) {
		NotifyError(wxT("%s: numFibres variable not set."), funcName);
		ok = FALSE;
	}
	if (!meddis02SGPtr->pulseDurationFlag) {
		NotifyError(wxT("%s: pulseDuration variable not set."), funcName);
		ok = FALSE;
	}
	if (!meddis02SGPtr->pulseMagnitudeFlag) {
		NotifyError(wxT("%s: pulseMagnitude variable not set."), funcName);
		ok = FALSE;
	}
	if (!meddis02SGPtr->refractoryPeriodFlag) {
		NotifyError(wxT("%s: refractoryPeriod variable not set."), funcName);
		ok = FALSE;
	}
	if (!meddis02SGPtr->recoveryTauFlag) {
		NotifyError(wxT("%s: recoveryTau variable not set."), funcName);
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
	static const WChar	*funcName = wxT("PrintPars_ANSpikeGen_Meddis02");

	if (!CheckPars_ANSpikeGen_Meddis02()) {
		NotifyError(wxT("%s: Parameters have not been correctly set."),
		  funcName);
		return(FALSE);
	}
	DPrint(wxT("Meddis 2002 post-synaptic firing module Parameters:-\n"));
	DPrint(wxT("\tRandom number seed = %ld,"), meddis02SGPtr->ranSeed);
	DPrint(wxT("\tNumber of fibres = %d,\n"), meddis02SGPtr->numFibres);
	DPrint(wxT("\tPulse duration = "));
	if (meddis02SGPtr->pulseDuration > 0.0)
		DPrint(wxT("%g ms,"), MSEC(meddis02SGPtr->pulseDuration));
	else
		DPrint(wxT("< prev. signal dt>,"));
	DPrint(wxT("\tPulse magnitude = %g (nA?),\n"), meddis02SGPtr->
	  pulseMagnitude);
	DPrint(wxT("\tAbsolute refractory period = %g (s),"),
	  meddis02SGPtr->refractoryPeriod);
	DPrint(wxT("\tRecovery time constant, tau = %g (s)\n"),
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
	static const WChar	*funcName = wxT("SetParsPointer_ANSpikeGen_Meddis02");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
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
	static const WChar	*funcName = wxT("InitModule_ANSpikeGen_Meddis02");

	if (!SetParsPointer_ANSpikeGen_Meddis02(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_ANSpikeGen_Meddis02(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = meddis02SGPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->CheckPars = CheckPars_ANSpikeGen_Meddis02;
	theModule->Free = Free_ANSpikeGen_Meddis02;
	theModule->GetUniParListPtr = GetUniParListPtr_ANSpikeGen_Meddis02;
	theModule->PrintPars = PrintPars_ANSpikeGen_Meddis02;
	theModule->ResetProcess = ResetProcess_ANSpikeGen_Meddis02;
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
	static const WChar	*funcName = wxT("CheckData_ANSpikeGen_Meddis02");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	if ((meddis02SGPtr->pulseDuration > 0.0) && (meddis02SGPtr->pulseDuration <
	  _InSig_EarObject(data, 0)->dt)) {
		NotifyError(wxT("%s: Pulse duration is too small for sampling\n")
		  wxT("interval, %g ms (%g ms)\n"), funcName,
		  MSEC(_InSig_EarObject(data, 0)->dt), MSEC(meddis02SGPtr->pulseDuration));
		return(FALSE);
	}
	/*** Put additional checks here. ***/
	return(TRUE);

}

/**************************** ResetProcessThread ******************************/

/*
 * This routine resets the process thread-related variables.
 */

void
ResetProcessThread_ANSpikeGen_Meddis02(EarObjectPtr data,
  double timeGreaterThanRefractoryPeriod, int i)
{
	int		j;
	double	*timerPtr, *remainingPulseTimePtr;
	Meddis02SGPtr	p = meddis02SGPtr;

	timerPtr = p->timer[i];
	remainingPulseTimePtr = p->remainingPulseTime[i];
	for (j = 0; j < p->arrayLength; j++) {
		*timerPtr++ = timeGreaterThanRefractoryPeriod;
		*remainingPulseTimePtr++ = 0.0;
	}

}

/**************************** ResetProcess ************************************/

/*
 * This routine resets the process variables.
 */

void
ResetProcess_ANSpikeGen_Meddis02(EarObjectPtr data)
{
	int		i;
	double	timeGreaterThanRefractoryPeriod;
	Meddis02SGPtr	p = meddis02SGPtr;

	ResetOutSignal_EarObject(data);
	timeGreaterThanRefractoryPeriod = p->refractoryPeriod + data->
	  outSignal->dt;
	if (data->threadRunFlag)
		ResetProcessThread_ANSpikeGen_Meddis02(data,
		  timeGreaterThanRefractoryPeriod, data->threadIndex);
	else  {
		for (i = 0; i < p->numThreads; i++) {
			ResetProcessThread_ANSpikeGen_Meddis02(data,
			  timeGreaterThanRefractoryPeriod, i);
		}
	}
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
	static const WChar	*funcName =
	  wxT("InitProcessVariables_ANSpikeGen_Meddis02");
	int		i;
	Meddis02SGPtr	p = meddis02SGPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag || (data->
	  timeIndex == PROCESS_START_TIME)) {
		p->arrayLength = _OutSig_EarObject(data)->numChannels * p->numFibres;
		if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
			FreeProcessVariables_ANSpikeGen_Meddis02();
			if (!SetRandPars_EarObject(data, p->ranSeed, funcName))
				return(FALSE);
			p->numThreads = data->numThreads;
			if ((p->timer = (double **) calloc(p->numThreads, sizeof(
			  double*))) == NULL) {
			 	NotifyError(wxT("%s: Out of memory for timer pointer array."),
				  funcName);
			 	return(FALSE);
			}
			if ((p->remainingPulseTime = (double **) calloc(p->numThreads,
			  sizeof(double*))) == NULL) {
			 	NotifyError(wxT("%s: Out of memory for remainingPulseTime ")
				  wxT("pointer array."), funcName);
			 	return(FALSE);
			}
			for (i = 0; i < p->numThreads; i++) {
				if ((p->timer[i] = (double *) calloc(p->arrayLength, sizeof(
				  double))) == NULL) {
			 		NotifyError(wxT("%s: Out of memory for timer array."),
					  funcName);
			 		return(FALSE);
				}
				if ((p->remainingPulseTime[i] = (double *) calloc(p->
				  arrayLength, sizeof(double))) == NULL) {
			 		NotifyError(wxT("%s: Out of memory for remainingPulseTime ")
					  wxT("array."), funcName);
			 		return(FALSE);
				}
			}
			p->updateProcessVariablesFlag = FALSE;
		}
		ResetProcess_ANSpikeGen_Meddis02(data);
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
	int		i;
	Meddis02SGPtr	p = meddis02SGPtr;

	if (p->timer) {
		for (i = 0; i < p->numThreads; i++)
			if (p->timer[i])
				free(p->timer[i]);
		free(p->timer);
		p->timer = NULL;
	}
	if (p->remainingPulseTime) {
		for (i = 0; i < p->numThreads; i++)
			if (p->remainingPulseTime[i])
				free(p->remainingPulseTime[i]);
		free(p->remainingPulseTime);
		p->remainingPulseTime = NULL;
	}
	p->updateProcessVariablesFlag = TRUE;
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
	static const WChar	*funcName = wxT("RunModel_ANSpikeGen_Meddis02");
	register ChanData	 *inPtr, *outPtr;
	register	double		*timerPtr, *remainingPulseTimePtr;
	int		i, chan;
	double	spikeProb, excessTime;
	ChanLen	j;
	SignalDataPtr	outSignal;
	Meddis02SGPtr	p = meddis02SGPtr;

	if (!data->threadRunFlag) {
		if (!CheckPars_ANSpikeGen_Meddis02())
			return(FALSE);
		if (!CheckData_ANSpikeGen_Meddis02(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Meddis 2002 Post-Synaptic Spike ")
		  wxT("Firing"));
		if (!InitOutSignal_EarObject(data, _InSig_EarObject(data, 0)->
		  numChannels, _InSig_EarObject(data, 0)->length, _InSig_EarObject(data,
		  0)->dt)) {
			NotifyError(wxT("%s: Could not initialise output signal."),
			  funcName);
			return(FALSE);
		}
		if (!InitProcessVariables_ANSpikeGen_Meddis02(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		p->dt = _InSig_EarObject(data, 0)->dt;
		p->wPulseDuration = (p->pulseDuration > 0.0)? p->pulseDuration: p->dt;
		for (chan = 0; chan < _OutSig_EarObject(data)->numChannels; chan++) {
			outPtr = _OutSig_EarObject(data)->channel[chan];
			for (j = 0; j < _OutSig_EarObject(data)->length; j++)
				*outPtr++ = 0.0;
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	outSignal = _OutSig_EarObject(data);
	for (i = 0, timerPtr = p->timer[data->threadIndex], remainingPulseTimePtr =
	  p->remainingPulseTime[data->threadIndex]; i < p->numFibres; i++)
		for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
			inPtr = _InSig_EarObject(data, 0)->channel[chan];
			outPtr = outSignal->channel[chan];
			for (j = 0; j < outSignal->length; j++) {
				if ((*inPtr > 0.0) && (*timerPtr > p->refractoryPeriod)) {
					excessTime = *timerPtr - p->refractoryPeriod;
					spikeProb = 1.0 - exp(-excessTime / p->recoveryTau);
					if (spikeProb > Ran01_Random(data->randPars)) {
						*remainingPulseTimePtr = p->wPulseDuration;
						*timerPtr = 0.0;
					}
				}
				if (*remainingPulseTimePtr > 0.0) {
					*outPtr += p->pulseMagnitude;
					*remainingPulseTimePtr -= p->dt;
				}
				*timerPtr += p->dt;
				inPtr++;
				outPtr++;
			}
			timerPtr++;
			remainingPulseTimePtr++;
		}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

