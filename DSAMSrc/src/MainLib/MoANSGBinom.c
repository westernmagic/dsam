/**********************
 *
 * File:		MoANSGBinom.c
 * Purpose:		Uses spike probabilities to produce the output from a number of
 *				fibres, using a binomial approximation.
 * Comments:	Written using ModuleProducer version 1.9 (May 27 1996).
 *				This is an approximation, so it should only be used when theree
 *				are a large number of fibres.  If in doubt, test against the
 *				output from the "Simple" spike generation module.
 *				It uses the refractory adjustment model module.
 * Author:		L. P. O'Mard, revised from code by M. J. Hewitt
 * Created:		25 Jun 1996
 * Updated:		19 Feb 1997
 * Copyright:	(c) 2000, University of Essex.
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
#include "UtString.h"
#include "UtRandom.h"
#include "MoANSGBinom.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

BinomialSGPtr	binomialSGPtr = NULL;

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
Free_ANSpikeGen_Binomial(void)
{
	if (binomialSGPtr == NULL)
		return(FALSE);
	FreeProcessVariables_ANSpikeGen_Binomial();
	if (binomialSGPtr->parList)
		FreeList_UniParMgr(&binomialSGPtr->parList);
	if (binomialSGPtr->parSpec == GLOBAL) {
		free(binomialSGPtr);
		binomialSGPtr = NULL;
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
Init_ANSpikeGen_Binomial(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_ANSpikeGen_Binomial");

	if (parSpec == GLOBAL) {
		if (binomialSGPtr != NULL)
			Free_ANSpikeGen_Binomial();
		if ((binomialSGPtr = (BinomialSGPtr) malloc(sizeof(
		  BinomialSG))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (binomialSGPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	binomialSGPtr->parSpec = parSpec;
	binomialSGPtr->updateProcessVariablesFlag = TRUE;
	binomialSGPtr->numFibresFlag = TRUE;
	binomialSGPtr->ranSeedFlag = TRUE;
	binomialSGPtr->pulseDurationFlag = TRUE;
	binomialSGPtr->pulseMagnitudeFlag = TRUE;
	binomialSGPtr->refractoryPeriodFlag = TRUE;
	binomialSGPtr->numFibres = 5000;
	binomialSGPtr->ranSeed = -1;
	binomialSGPtr->pulseDuration = 2e-05;
	binomialSGPtr->pulseMagnitude = 1.0;
	binomialSGPtr->refractoryPeriod = 1e-3;

	if (!SetUniParList_ANSpikeGen_Binomial()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_ANSpikeGen_Binomial();
		return(FALSE);
	}
	binomialSGPtr->numChannels = 0;
	binomialSGPtr->lastOutput = NULL;
	binomialSGPtr->remainingPulseIndex = NULL;
	binomialSGPtr->refractAdjData = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_ANSpikeGen_Binomial(void)
{
	static const WChar *funcName = wxT("SetUniParList_ANSpikeGen_Binom");
	UniParPtr	pars;

	if ((binomialSGPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  ANSPIKEGEN_BINOM_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = binomialSGPtr->parList->pars;
	SetPar_UniParMgr(&pars[ANSPIKEGEN_BINOM_NUMFIBRES], wxT("NUM_FIBRES"),
	  wxT("Number of fibres."),
	  UNIPAR_INT,
	  &binomialSGPtr->numFibres, NULL,
	  (void * (*)) SetNumFibres_ANSpikeGen_Binomial);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_BINOM_RANSEED], wxT("RAN_SEED"),
	  wxT("Random number seed (0 for different seed for each run)."),
	  UNIPAR_LONG,
	  &binomialSGPtr->ranSeed, NULL,
	  (void * (*)) SetRanSeed_ANSpikeGen_Binomial);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_BINOM_PULSEDURATION], wxT(
	  "PULSE_DURATION"),
	  wxT("Pulse duration (s)."),
	  UNIPAR_REAL,
	  &binomialSGPtr->pulseDuration, NULL,
	  (void * (*)) SetPulseDuration_ANSpikeGen_Binomial);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_BINOM_PULSEMAGNITUDE], wxT("MAGNITUDE"),
	  wxT("Pulse magnitude (arbitrary units)."),
	  UNIPAR_REAL,
	  &binomialSGPtr->pulseMagnitude, NULL,
	  (void * (*)) SetPulseMagnitude_ANSpikeGen_Binomial);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_BINOM_REFRACTORYPERIOD], wxT(
	  "REFRAC_PERIOD"),
	  wxT("Refractory period (s)."),
	  UNIPAR_REAL,
	  &binomialSGPtr->refractoryPeriod, NULL,
	  (void * (*)) SetRefractoryPeriod_ANSpikeGen_Binomial);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_ANSpikeGen_Binomial(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_ANSpikeGen_Binom");

	if (binomialSGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (binomialSGPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been "
		  "initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(binomialSGPtr->parList);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_ANSpikeGen_Binomial(int numFibres, long ranSeed,
  double pulseDuration, double pulseMagnitude, double refractoryPeriod)
{
	static const WChar	*funcName = wxT("SetPars_ANSpikeGen_Binomial");
	BOOLN	ok;

	ok = TRUE;
	if (!SetNumFibres_ANSpikeGen_Binomial(numFibres))
		ok = FALSE;
	if (!SetRanSeed_ANSpikeGen_Binomial(ranSeed))
		ok = FALSE;
	if (!SetPulseDuration_ANSpikeGen_Binomial(pulseDuration))
		ok = FALSE;
	if (!SetPulseMagnitude_ANSpikeGen_Binomial(pulseMagnitude))
		ok = FALSE;
	if (!SetRefractoryPeriod_ANSpikeGen_Binomial(refractoryPeriod))
		ok = FALSE;
	if (!ok)
		NotifyError(wxT("%s: Failed to set all module parameters.") ,funcName);
	return(ok);

}

/****************************** SetNumFibres **********************************/

/*
 * This function sets the module's numFibres parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNumFibres_ANSpikeGen_Binomial(int theNumFibres)
{
	static const WChar	*funcName = wxT("SetNumFibres_ANSpikeGen_Binomial");

	if (binomialSGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	binomialSGPtr->numFibresFlag = TRUE;
	binomialSGPtr->numFibres = theNumFibres;
	return(TRUE);

}

/****************************** SetRanSeed ************************************/

/*
 * This function sets the module's ranSeed parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRanSeed_ANSpikeGen_Binomial(long theRanSeed)
{
	static const WChar	*funcName = wxT("SetRanSeed_ANSpikeGen_Binomial");

	if (binomialSGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	binomialSGPtr->ranSeedFlag = TRUE;
	binomialSGPtr->updateProcessVariablesFlag = TRUE;
	binomialSGPtr->ranSeed = theRanSeed;
	return(TRUE);

}

/****************************** SetPulseDuration ******************************/

/*
 * This function sets the module's pulseDuration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPulseDuration_ANSpikeGen_Binomial(double thePulseDuration)
{
	static const WChar	*funcName = wxT("SetPulseDuration_ANSpikeGen_Binomial");

	if (binomialSGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (thePulseDuration < 0.0) {
		NotifyError(wxT("%s: Pulse duration must be greater than zero.\n"),
		  funcName);
		return(FALSE);
	}
	binomialSGPtr->pulseDurationFlag = TRUE;
	binomialSGPtr->updateProcessVariablesFlag = TRUE;
	binomialSGPtr->pulseDuration = thePulseDuration;
	return(TRUE);

}

/****************************** SetPulseMagnitude *****************************/

/*
 * This function sets the module's pulseMagnitude parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPulseMagnitude_ANSpikeGen_Binomial(double thePulseMagnitude)
{
	static const WChar	*funcName = wxT(
	  "SetPulseMagnitude_ANSpikeGen_Binomial");

	if (binomialSGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	binomialSGPtr->pulseMagnitudeFlag = TRUE;
	binomialSGPtr->pulseMagnitude = thePulseMagnitude;
	return(TRUE);

}

/****************************** SetRefractoryPeriod ***************************/

/*
 * This function sets the module's refractoryPeriod parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRefractoryPeriod_ANSpikeGen_Binomial(double theRefractoryPeriod)
{
	static const WChar	*funcName =
	  wxT("SetRefractoryPeriod_ANSpikeGen_Binomial");

	if (binomialSGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theRefractoryPeriod < 0.0) {
		NotifyError(wxT("%s: Refractory period must be greater than zero "
		  "(%g ms)."), funcName, MSEC(theRefractoryPeriod));
		return(FALSE);
	}
	binomialSGPtr->refractoryPeriodFlag = TRUE;
	binomialSGPtr->refractoryPeriod = theRefractoryPeriod;
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
CheckPars_ANSpikeGen_Binomial(void)
{
	static const WChar	*funcName = wxT("CheckPars_ANSpikeGen_Binomial");
	BOOLN	ok;

	ok = TRUE;
	if (binomialSGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (!binomialSGPtr->numFibresFlag) {
		NotifyError(wxT("%s: numFibres variable not set."), funcName);
		ok = FALSE;
	}
	if (!binomialSGPtr->ranSeedFlag) {
		NotifyError(wxT("%s: ranSeed variable not set."), funcName);
		ok = FALSE;
	}
	if (!binomialSGPtr->pulseDurationFlag) {
		NotifyError(wxT("%s: pulseDuration variable not set."), funcName);
		ok = FALSE;
	}
	if (!binomialSGPtr->pulseMagnitudeFlag) {
		NotifyError(wxT("%s: pulseMagnitude variable not set."), funcName);
		ok = FALSE;
	}
	if (!binomialSGPtr->refractoryPeriodFlag) {
		NotifyError(wxT("%s: refractoryPeriod variable not set."), funcName);
		ok = FALSE;
	}
	if (binomialSGPtr->pulseDuration >= binomialSGPtr->refractoryPeriod) {
		NotifyError(wxT("%s: Pulse duration must be smaller than the\n"
		  "refractory period, %g ms (%g ms)."), funcName,
		  MSEC(binomialSGPtr->refractoryPeriod),
		  MSEC(binomialSGPtr->pulseDuration));
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
PrintPars_ANSpikeGen_Binomial(void)
{
	static const WChar	*funcName = wxT("PrintPars_ANSpikeGen_Binomial");

	if (!CheckPars_ANSpikeGen_Binomial()) {
		NotifyError(wxT("%s: Parameters have not been correctly set."),
		  funcName);
		return(FALSE);
	}
	DPrint(wxT("Binomial Post-synaptic Firing Module:-\n"));
	DPrint(wxT("\tNo. fibres = %d,"), binomialSGPtr->numFibres);
	DPrint(wxT("\tRandom no. seed = %ld\n"), binomialSGPtr->ranSeed);
	DPrint(wxT("\tPulse duration = %g ms,"), MSEC(binomialSGPtr->
	  pulseDuration));
	DPrint(wxT("\tPulse magnitude = %g (nA?),\n"), binomialSGPtr->
	  pulseMagnitude);
	DPrint(wxT("\trefractoryPeriod = %g ms\n"), MSEC(binomialSGPtr->
	  refractoryPeriod));
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_ANSpikeGen_Binomial(WChar *fileName)
{
	static const WChar	*funcName = wxT("ReadPars_ANSpikeGen_Binomial");
	BOOLN	ok;
	WChar	*filePath;
	int		numFibres;
	long	ranSeed;
	double	pulseDuration, pulseMagnitude, refractoryPeriod;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = DSAM_fopen(filePath, "r")) == NULL) {
		NotifyError(wxT("%s: Cannot open data file '%s'.\n"), funcName,
		  filePath);
		return(FALSE);
	}
	DPrint(wxT("%s: Reading from '%s':\n"), funcName, filePath);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, wxT("%d"), &numFibres))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%ld"), &ranSeed))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &pulseDuration))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &pulseMagnitude))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &refractoryPeriod))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError(wxT("%s: Not enough lines, or invalid parameters, in "
		  "module parameter file '%s'."), funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_ANSpikeGen_Binomial(numFibres, ranSeed, pulseDuration,
	  pulseMagnitude, refractoryPeriod)) {
		NotifyError(wxT("%s: Could not set parameters."), funcName);
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
SetParsPointer_ANSpikeGen_Binomial(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_ANSpikeGen_Binomial");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	binomialSGPtr = (BinomialSGPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_ANSpikeGen_Binomial(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_ANSpikeGen_Binomial");

	if (!SetParsPointer_ANSpikeGen_Binomial(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_ANSpikeGen_Binomial(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = binomialSGPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->CheckPars = CheckPars_ANSpikeGen_Binomial;
	theModule->Free = Free_ANSpikeGen_Binomial;
	theModule->GetUniParListPtr = GetUniParListPtr_ANSpikeGen_Binomial;
	theModule->PrintPars = PrintPars_ANSpikeGen_Binomial;
	theModule->ReadPars = ReadPars_ANSpikeGen_Binomial;
	theModule->RunProcess = RunModel_ANSpikeGen_Binomial;
	theModule->SetParsPointer = SetParsPointer_ANSpikeGen_Binomial;
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
CheckData_ANSpikeGen_Binomial(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_ANSpikeGen_Binomial");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	if (binomialSGPtr->pulseDuration < data->inSignal[0]->dt) {
		NotifyError(wxT("%s: Pulse duration is too small for sampling\n"
		  "interval, %g ms (%g ms)\n"), funcName, MSEC(data->inSignal[0]->dt),
		  MSEC(binomialSGPtr->pulseDuration));
		return(FALSE);
	}
	return(TRUE);

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 * It initialises the 'refractAdjData' EarObject used in the to create the
 * data adjusted for refractory effects.
 */

BOOLN
InitProcessVariables_ANSpikeGen_Binomial(EarObjectPtr data)
{
	static const WChar *funcName = wxT(
	  "InitProcessVariables_ANSpikeGen_Binomial");
	int		i;
	BinomialSGPtr	p = binomialSGPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
		FreeProcessVariables_ANSpikeGen_Binomial();
		if (!SetRandPars_EarObject(data, p->ranSeed, funcName))
			return(FALSE);
		if ((p->refractAdjData = Init_EarObject(wxT("Util_Refractory"))) ==
		  NULL) {
			NotifyError(wxT("%s: Out of memory for refractAdjData EarObject."),
			  funcName);
			return(FALSE);
		}
		if (!InitSubProcessList_EarObject(data,
		  ANSPIKEGEN_BINOM_NUM_SUB_PROCESSES)) {
			NotifyError(wxT("%s: Could not initialise %d sub-process list for "
			  "process."), funcName, ANSPIKEGEN_BINOM_NUM_SUB_PROCESSES);
			return(FALSE);
		}
		data->subProcessList[ANSPIKEGEN_REFRACTADJDATA] = p->refractAdjData;
		SetRealPar_ModuleMgr(p->refractAdjData, wxT("period"), p->
		  refractoryPeriod);
		p->numChannels = _OutSig_EarObject(data)->numChannels;
		if ((p->remainingPulseIndex = (ChanLen *) calloc(p->numChannels, sizeof(
		  ChanLen))) == NULL) {
			NotifyError(wxT("%s: Out of memory for remainingPulseIndex array."),
			  funcName);
			return(FALSE);
		}
		if ((p->lastOutput = (double *) calloc(p->numChannels, sizeof(
		  double))) == NULL) {
			NotifyError(wxT("%s: Out of memory for lastOutput array."),
			  funcName);
			return(FALSE);
		}
		p->updateProcessVariablesFlag = FALSE;
	}
	if (data->timeIndex == PROCESS_START_TIME) {
		for (i = 0; i < p->numChannels; i++) {
			p->remainingPulseIndex[i] = 0;
			p->lastOutput[i] = 0.0;
		}
	}
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

void
FreeProcessVariables_ANSpikeGen_Binomial(void)
{
	BinomialSGPtr	p = binomialSGPtr;

	if (p->refractAdjData)
		Free_EarObject(&p->refractAdjData);
	if (p->remainingPulseIndex != NULL) {
		free(p->remainingPulseIndex);
		p->remainingPulseIndex = NULL;
	}
	if (p->lastOutput != NULL) {
		free(p->lastOutput);
		p->lastOutput = NULL;
	}
	p->updateProcessVariablesFlag = TRUE;

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
 * The 'refractAdjData' is manually connected to the input signal.
 */

BOOLN
RunModel_ANSpikeGen_Binomial(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("RunModel_ANSpikeGen_Binomial");
	register	ChanData	*inPtr, *outPtr, *pulsePtr;
	register	double		output = 0.0;
	int		chan;
	double	*lastOutputPtr;
	ChanLen	i, pulseTimer = 0;
	ChanLen	*remainingPulseIndexPtr;
	ChanData	*pastEndOfData;
	EarObjectPtr	refractAdjData;
	BinomialSGPtr	p = binomialSGPtr;

	if (!data->threadRunFlag) {
		if (!CheckPars_ANSpikeGen_Binomial())
			return(FALSE);
		if (!CheckData_ANSpikeGen_Binomial(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Binomial Post-synaptic Firing"));
		if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels,
		  data->inSignal[0]->length, data->inSignal[0]->dt)) {
			NotifyError(wxT("%s: Could not initialise output signal."),
			  funcName);
			return(FALSE);
		}
		if (!InitProcessVariables_ANSpikeGen_Binomial(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		TempInputConnection_EarObject(data, p->refractAdjData, 1);
		p->pulseDurationIndex = (ChanLen) (p->pulseDuration / data->inSignal[
		  0]->dt + 0.5);
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	refractAdjData = data->subProcessList[ANSPIKEGEN_REFRACTADJDATA];
	RunProcessStandard_ModuleMgr(refractAdjData);
	remainingPulseIndexPtr = p->remainingPulseIndex + _OutSig_EarObject(data)->offset;
	lastOutputPtr = p->lastOutput + _OutSig_EarObject(data)->offset;
	for (chan = _OutSig_EarObject(data)->offset; chan < _OutSig_EarObject(data)->numChannels;
	  chan++) {
		inPtr = refractAdjData->outSignal->channel[chan];
		outPtr = _OutSig_EarObject(data)->channel[chan];
		for (i = 0; i < _OutSig_EarObject(data)->length; i++)
			*outPtr++ = 0.0;
		if (p->numFibres < 1)
			continue;
		outPtr = _OutSig_EarObject(data)->channel[chan];
		pastEndOfData = _OutSig_EarObject(data)->channel[chan] +
		  _OutSig_EarObject(data)->length;
		for (i = 0; i < _OutSig_EarObject(data)->length; i++, outPtr++) {
			output = p->pulseMagnitude * GeomDist_Random(*inPtr++,
			  p->numFibres, data->randPars);
			if (output > 0.0) {
				for (pulseTimer = p->pulseDurationIndex, pulsePtr = outPtr;
				  pulseTimer && (pulsePtr < pastEndOfData); pulsePtr++,
				  pulseTimer--) {
					*pulsePtr += output;
					if (*remainingPulseIndexPtr) {
						*pulsePtr += *lastOutputPtr;
						(*remainingPulseIndexPtr)--;
					}
				}
			}
		}		
		*lastOutputPtr++ = output;
		*remainingPulseIndexPtr++ = pulseTimer;
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

