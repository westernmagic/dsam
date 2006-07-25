/******************
 *
 * File:		MaANSGSimple.c
 * Purpose:		This module contains the model for the hair cell post-synaptic
 *				firing
 * Comments:	24-02-97 LPO: Amended to allow multiple fibres.
 * Authors:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		24 Feb 1997
 * Copyright:	(c) 1998, University of Essex.
 * 
 ******************/

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
#include "MoANSGSimple.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

SimpleSGPtr		simpleSGPtr = NULL;

/******************************************************************************/
/********************************* Subroutines and functions ******************/
/******************************************************************************/

/********************************* Init ***************************************/

/*
 * This function initialises the module by setting module's parameter pointer
 * structure.
 * The GLOBAL option is for hard programming - it sets the module's pointer to
 * the global parameter structure and initialises the parameters.
 * The LOCAL option is for generic programming - it initialises the parameter
 * structure currently pointed to by the module's parameter pointer.
 */

BOOLN
Init_ANSpikeGen_Simple(ParameterSpecifier parSpec)
{
	static const WChar *funcName = wxT("Init_ANSpikeGen_Simple");
	if (parSpec == GLOBAL) {
		if (simpleSGPtr != NULL)
			Free_ANSpikeGen_Simple();
		if ((simpleSGPtr = (SimpleSGPtr) malloc(sizeof(SimpleSG))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"), funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (simpleSGPtr == NULL) { 
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	simpleSGPtr->parSpec = parSpec;
	simpleSGPtr->updateProcessVariablesFlag = TRUE;
	simpleSGPtr->pulseDurationFlag = TRUE;
	simpleSGPtr->pulseMagnitudeFlag = TRUE;
	simpleSGPtr->refractoryPeriodFlag = TRUE;
	simpleSGPtr->numFibresFlag = TRUE;
	simpleSGPtr->ranSeedFlag = TRUE;
	simpleSGPtr->ranSeed = -1;
	simpleSGPtr->numFibres = 5;
	simpleSGPtr->pulseDuration = 0.1e-3;
	simpleSGPtr->pulseMagnitude = 4.3;
	simpleSGPtr->refractoryPeriod = PS_REFRACTORY_PERIOD;

	if (!SetUniParList_ANSpikeGen_Simple()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_ANSpikeGen_Simple();
		return(FALSE);
	}
	simpleSGPtr->timer = NULL;
	simpleSGPtr->remainingPulseTime = NULL;
	return(TRUE);

}

/********************************* Free ***************************************/

/*
 * This function releases of the memory allocated for the process variables.
 * It should be called when the module is no longer in use.
 * It is defined as returning a BOOLN value because the generic module
 * interface requires that a non-void value be returned.
 */

BOOLN
Free_ANSpikeGen_Simple(void)
{
	if (simpleSGPtr == NULL)
		return(TRUE);
	FreeProcessVariables_ANSpikeGen_Simple();
	if (simpleSGPtr->parList)
		FreeList_UniParMgr(&simpleSGPtr->parList);
	if (simpleSGPtr->parSpec == GLOBAL) {
		free(simpleSGPtr);
		simpleSGPtr = NULL;
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
SetUniParList_ANSpikeGen_Simple(void)
{
	static const WChar *funcName = wxT("SetUniParList_ANSpikeGen_Simple");
	UniParPtr	pars;

	if ((simpleSGPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  ANSPIKEGEN_SIMPLE_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = simpleSGPtr->parList->pars;
	SetPar_UniParMgr(&pars[ANSPIKEGEN_SIMPLE_RANSEED], wxT("RAN_SEED"),
	  wxT("Random number seed (0 produces a different seed each run."),
	  UNIPAR_LONG,
	  &simpleSGPtr->ranSeed, NULL,
	  (void * (*)) SetRanSeed_ANSpikeGen_Simple);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_SIMPLE_NUMFIBRES], wxT("NUM_FIBRES"),
	  wxT("Number of fibres."),
	  UNIPAR_INT,
	  &simpleSGPtr->numFibres, NULL,
	  (void * (*)) SetNumFibres_ANSpikeGen_Simple);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_SIMPLE_PULSEDURATION], wxT(
	  "PULSE_DURATION"),
	  wxT("Pulse duration (s)."),
	  UNIPAR_REAL,
	  &simpleSGPtr->pulseDuration, NULL,
	  (void * (*)) SetPulseDuration_ANSpikeGen_Simple);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_SIMPLE_PULSEMAGNITUDE], wxT("MAGNITUDE"),
	  wxT("Pulse magnitude (arbitrary units)."),
	  UNIPAR_REAL,
	  &simpleSGPtr->pulseMagnitude, NULL,
	  (void * (*)) SetPulseMagnitude_ANSpikeGen_Simple);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_SIMPLE_REFRACTORYPERIOD], wxT(
	  "REFRAC_PERIOD"),
	  wxT("Refractory period (s)."),
	  UNIPAR_REAL,
	  &simpleSGPtr->refractoryPeriod, NULL,
	  (void * (*)) SetRefractoryPeriod_ANSpikeGen_Simple);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_ANSpikeGen_Simple(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_ANSpikeGen_Simple");

	if (simpleSGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (simpleSGPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been "
		  "initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(simpleSGPtr->parList);

}

/********************************* SetRanSeed *********************************/

/*
 * This function sets the module's random number seed parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetRanSeed_ANSpikeGen_Simple(long theRanSeed)
{
	static const WChar *funcName = wxT("SetRanSeed_ANSpikeGen_Simple");

	if (simpleSGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	simpleSGPtr->ranSeedFlag = TRUE;
	simpleSGPtr->ranSeed = theRanSeed;
	simpleSGPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** SetNumFibres **********************************/

/*
 * This function sets the module's numFibres parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNumFibres_ANSpikeGen_Simple(int theNumFibres)
{
	static const WChar	*funcName = wxT("SetNumFibres_ANSpikeGen_Simple");

	if (simpleSGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theNumFibres < 1) {
		NotifyError(wxT("%s: Illegal no. of fibres (%d)."), funcName,
		  theNumFibres);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	simpleSGPtr->numFibresFlag = TRUE;
	simpleSGPtr->numFibres = theNumFibres;
	return(TRUE);

}

/****************************** SetPulseDuration ******************************/

/*
 * This function sets the module's pulseDuration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPulseDuration_ANSpikeGen_Simple(double thePulseDuration)
{
	static const WChar	*funcName = wxT("SetPulseDuration_ANSpikeGen_Simple");

	if (simpleSGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (thePulseDuration < 0.0) {
		NotifyError(wxT("%s: Pulse duration must be greater than zero.\n"),
		  funcName);
		return(FALSE);
	}
	simpleSGPtr->pulseDurationFlag = TRUE;
	simpleSGPtr->pulseDuration = thePulseDuration;
	simpleSGPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** SetPulseMagnitude *****************************/

/*
 * This function sets the module's pulseMagnitude parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPulseMagnitude_ANSpikeGen_Simple(double thePulseMagnitude)
{
	static const WChar	*funcName = wxT("SetPulseMagnitude_ANSpikeGen_Simple");

	if (simpleSGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	simpleSGPtr->pulseMagnitudeFlag = TRUE;
	simpleSGPtr->pulseMagnitude = thePulseMagnitude;
	return(TRUE);

}

/********************************* SetRefractoryPeriod ************************/

/*
 * This function sets the module's refractory period parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetRefractoryPeriod_ANSpikeGen_Simple(double theRefractoryPeriod)
{
	static const WChar *funcName = wxT("SetRefractoryPeriod_ANSpikeGen_Simple");

	if (simpleSGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theRefractoryPeriod < 0.0) {
		NotifyError(wxT("%s: Refractory period must be greater than zero.\n"),
		  funcName);
		return(FALSE);
	}
	simpleSGPtr->refractoryPeriodFlag = TRUE;
	simpleSGPtr->refractoryPeriod = theRefractoryPeriod;
	simpleSGPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetPars ************************************/

/*
 * This function sets all the module's parameters.
 */

BOOLN
SetPars_ANSpikeGen_Simple(long theRanSeed, int numFibres, double pulseDuration,
  double pulseMagnitude, double theRefractoryPeriod)
{
	BOOLN	ok;
	
	ok = TRUE;
	if (!SetRanSeed_ANSpikeGen_Simple(theRanSeed))
		ok = FALSE;
	if (!SetNumFibres_ANSpikeGen_Simple(numFibres))
		ok = FALSE;
	if (!SetPulseDuration_ANSpikeGen_Simple(pulseDuration))
		ok = FALSE;
	if (!SetPulseMagnitude_ANSpikeGen_Simple(pulseMagnitude))
		ok = FALSE;
	if (!SetRefractoryPeriod_ANSpikeGen_Simple(theRefractoryPeriod))
		ok = FALSE;
	return(ok);
	  
}

/********************************* CheckPars **********************************/

/*
 * This routine checks that all of the parameters for the module are set.
 * Because this module is defined by its coeffients, default values are always
 * set if none of the parameters are set, otherwise all of them must be
 * individually set.
 */

BOOLN
CheckPars_ANSpikeGen_Simple(void)
{
	static const WChar *funcName = wxT("CheckPars_ANSpikeGen_Simple");
	BOOLN	ok;
	
	ok = TRUE;
	if (simpleSGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (!simpleSGPtr->ranSeedFlag) {
		NotifyError(wxT("%s: Random number seed not correctly set."), funcName);
		ok = FALSE;
	}
	if (!simpleSGPtr->numFibresFlag) {
		NotifyError(wxT("%s: numFibres variable not set."), funcName);
		ok = FALSE;
	}
	if (!simpleSGPtr->pulseDurationFlag) {
		NotifyError(wxT("%s: pulseDuration variable not set."), funcName);
		ok = FALSE;
	}
	if (!simpleSGPtr->pulseMagnitudeFlag) {
		NotifyError(wxT("%s: pulseMagnitude variable not set."), funcName);
		ok = FALSE;
	}
	if (!simpleSGPtr->refractoryPeriodFlag) {
		NotifyError(wxT("%s: Refractory period not correctly set."), funcName);
		ok = FALSE;
	}
	if (simpleSGPtr->pulseDuration >= simpleSGPtr->refractoryPeriod) {
		NotifyError(wxT("%s: Pulse duration must be smaller than the\n"
		  "refractory period, %g ms (%g ms)."), funcName, MSEC(
		  simpleSGPtr->refractoryPeriod), MSEC(simpleSGPtr->pulseDuration));
		ok = FALSE;
	}
	return(ok);
		
}

/****************************** PrintPars *************************************/

/*
 * This program prints the parameters of the module to the standard output.
 */
 
BOOLN
PrintPars_ANSpikeGen_Simple(void)
{
	static const WChar *funcName = wxT("PrintPars_ANSpikeGen_Simple");

	if (!CheckPars_ANSpikeGen_Simple()) {
		NotifyError(wxT("%s: Parameters have not been correctly set."),
		  funcName);
		return(FALSE);
	}
	DPrint(wxT("Simple Post-Synaptic Firing Module Parameters:-\n"));
	DPrint(wxT("\tRandom number seed = %ld,"), simpleSGPtr->ranSeed);
	DPrint(wxT("\tNo. of fibres = %d,\n"), simpleSGPtr->numFibres);
	DPrint(wxT("\tPulse duration = %g ms,"), MSEC(simpleSGPtr->pulseDuration));
	DPrint(wxT("\tPulse magnitude = %g (nA?),\n"), simpleSGPtr->pulseMagnitude);
	DPrint(wxT("\tRefractory Period = %g ms\n"), MSEC(simpleSGPtr->
	  refractoryPeriod));
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.
 */
 
BOOLN
ReadPars_ANSpikeGen_Simple(WChar *fileName)
{
	static const WChar *funcName = wxT("ReadPars_ANSpikeGen_Simple");
	BOOLN	ok;
	WChar	*filePath;
	long	ranSeed;
	int		numFibres;
	double	pulseDuration, pulseMagnitude, refractoryPeriod;
    FILE    *fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = DSAM_fopen(filePath, "r")) == NULL) {
        NotifyError(wxT("%s: Cannot open data file '%s'.\n"), funcName,
		  filePath);
		return(FALSE);
    }
    DPrint(wxT("%s: Reading from '%s':\n"), funcName, filePath);
    Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, wxT("%ld"), &ranSeed))
		ok = FALSE;
 	if (!GetPars_ParFile(fp, wxT("%d"), &numFibres))
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
	if (!SetPars_ANSpikeGen_Simple(ranSeed, numFibres, pulseDuration,
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
SetParsPointer_ANSpikeGen_Simple(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_ANSpikeGen_Simple");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	simpleSGPtr = (SimpleSGPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_ANSpikeGen_Simple(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_ANSpikeGen_Simple");

	if (!SetParsPointer_ANSpikeGen_Simple(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_ANSpikeGen_Simple(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = simpleSGPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->CheckPars = CheckPars_ANSpikeGen_Simple;
	theModule->Free = Free_ANSpikeGen_Simple;
	theModule->GetUniParListPtr = GetUniParListPtr_ANSpikeGen_Simple;
	theModule->PrintPars = PrintPars_ANSpikeGen_Simple;
	theModule->ReadPars = ReadPars_ANSpikeGen_Simple;
	theModule->ResetProcess = ResetProcess_ANSpikeGen_Simple;
	theModule->RunProcess = RunModel_ANSpikeGen_Simple;
	theModule->SetParsPointer = SetParsPointer_ANSpikeGen_Simple;
	return(TRUE);

}

/**************************** ResetProcessThread ******************************/

/*
 * This routine resets the process thread-related variables.
 */

void
ResetProcessThread_ANSpikeGen_Simple(EarObjectPtr data,
  double timeGreaterThanRefractoryPeriod, int i)
{
	int		j;
	double	*timerPtr, *remainingPulseTimePtr;
	SimpleSGPtr	p = simpleSGPtr;

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
ResetProcess_ANSpikeGen_Simple(EarObjectPtr data)
{
	int		i;
	double	timeGreaterThanRefractoryPeriod;
	SimpleSGPtr	p = simpleSGPtr;

	ResetOutSignal_EarObject(data);
	timeGreaterThanRefractoryPeriod = p->refractoryPeriod + data->
	  outSignal->dt;
	if (data->threadRunFlag)
		ResetProcessThread_ANSpikeGen_Simple(data,
		  timeGreaterThanRefractoryPeriod, data->threadIndex);
	else  {
		for (i = 0; i < data->numThreads; i++) {
			ResetProcessThread_ANSpikeGen_Simple(data,
			  timeGreaterThanRefractoryPeriod, i);
		}
	}
}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 */

BOOLN
InitProcessVariables_ANSpikeGen_Simple(EarObjectPtr data)
{
	static const WChar *funcName = wxT(
	  "InitProcessVariables_ANSpikeGen_Simple");
	int		i;
	SimpleSGPtr	p = simpleSGPtr;
	
	if (p->updateProcessVariablesFlag || data->updateProcessFlag || (data->
	  timeIndex == PROCESS_START_TIME)) {
		p->arrayLength = _OutSig_EarObject(data)->numChannels * p->numFibres;
		if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
			FreeProcessVariables_ANSpikeGen_Simple();
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
			 	NotifyError(wxT("%s: Out of memory for remainingPulseTime "
				  "pointer array."), funcName);
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
			 		NotifyError(wxT("%s: Out of memory for remainingPulseTime "
					  "array."), funcName);
			 		return(FALSE);
				}
			}
			p->updateProcessVariablesFlag = FALSE;
		}
		ResetProcess_ANSpikeGen_Simple(data);
	}
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

void
FreeProcessVariables_ANSpikeGen_Simple(void)
{
	int		i;
	SimpleSGPtr	p = simpleSGPtr;

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
	simpleSGPtr->updateProcessVariablesFlag = TRUE;

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
CheckData_ANSpikeGen_Simple(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_ANSpikeGen_Simple");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	if (simpleSGPtr->pulseDuration < data->inSignal[0]->dt) {
		NotifyError(wxT("%s: Pulse duration is too small for sampling\n"
		  "interval, %g ms (%g ms)\n"), funcName, MSEC(data->inSignal[0]->dt),
		  MSEC(simpleSGPtr->pulseDuration));
		return(FALSE);
	}
	return(TRUE);

}

/********************************* RunModel ***********************************/

/*
 * This routine runs the input signal through the model and puts the result
 * into the output signal.  It checks that all initialisation has been
 * correctly carried out by calling the appropriate checking routines.
 */

BOOLN
RunModel_ANSpikeGen_Simple(EarObjectPtr data)
{
	static const WChar *funcName = wxT("RunModel_ANSpikeGen_Simple");
	register	ChanData	*inPtr, *outPtr;
	register	double		*timerPtr, *remainingPulseTimePtr;
	int		i, chan;
	ChanLen	j;
	SimpleSGPtr	p = simpleSGPtr;

	if (!data->threadRunFlag) {
		if (!CheckPars_ANSpikeGen_Simple())
			return(FALSE);
		if (!CheckData_ANSpikeGen_Simple(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Simple Post-Synaptic Spike "
		  "Firing"));
		if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels,
		  data->inSignal[0]->length, data->inSignal[0]->dt)) {
			NotifyError(wxT("%s: Could not initialise output signal."),
			  funcName);
			return(FALSE);
		}
		if (!InitProcessVariables_ANSpikeGen_Simple(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		p->dt = data->inSignal[0]->dt;
		for (chan = 0; chan < _OutSig_EarObject(data)->numChannels; chan++) {
			outPtr = _OutSig_EarObject(data)->channel[chan];
			for (j = 0; j < _OutSig_EarObject(data)->length; j++)
				*outPtr++ = 0.0;
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	for (i = 0, timerPtr = p->timer[data->threadIndex], remainingPulseTimePtr =
	  p->remainingPulseTime[data->threadIndex]; i < p->numFibres; i++)
		for (chan = _OutSig_EarObject(data)->offset; chan < _OutSig_EarObject(data)->
		  numChannels; chan++) {
			inPtr = data->inSignal[0]->channel[chan];
			outPtr = _OutSig_EarObject(data)->channel[chan];
			for (j = 0; j < _OutSig_EarObject(data)->length; j++) {
				if ((*timerPtr > p->refractoryPeriod) && (*inPtr > Ran01_Random(
				  data->randPars))) {
					*remainingPulseTimePtr = p->pulseDuration;
					*timerPtr = 0.0;
				}
				if (*remainingPulseTimePtr >= p->dt) {
					*outPtr += p->pulseMagnitude;
					*remainingPulseTimePtr -= p->dt;
				};
				*timerPtr += p->dt;
				inPtr++;
				outPtr++;
			}
			timerPtr++;
			remainingPulseTimePtr++;
		}
	SetProcessContinuity_EarObject(data);
	return(TRUE);
		
} /* RunModel_ANSpikeGen_Simple */
