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
	static const char *funcName = "Init_ANSpikeGen_Simple";
	if (parSpec == GLOBAL) {
		if (simpleSGPtr != NULL)
			Free_ANSpikeGen_Simple();
		if ((simpleSGPtr = (SimpleSGPtr) malloc(sizeof(SimpleSG))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (simpleSGPtr == NULL) { 
			NotifyError("%s:  'local' pointer not set.", funcName);
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
		NotifyError("%s: Could not initialise parameter list.", funcName);
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

/************************** InitFuncPtr ***************************************/


/*
 * This routine initialises the function pointer for the module.
 * It should be deallocated when the module is deallocated.
 */

SpikeGenFuncPtr
InitFuncPtr_ANSpikeGen_Simple(void)
{
	static const char *funcName = "InitFuncPtr_ANSpikeGen_Simple";
	SpikeGenFuncPtr	ptr;
	
	if ((ptr = (SpikeGenFuncPtr) malloc(sizeof(SpikeGenFunc))) == NULL) {
		NotifyError("%s: Out of memory.", funcName);
		return(NULL);
	}
	ptr->CheckPars = CheckPars_ANSpikeGen_Simple;
	ptr->Free = Free_ANSpikeGen_Simple;
	ptr->PrintPars = PrintPars_ANSpikeGen_Simple;
	ptr->ReadPars = ReadPars_ANSpikeGen_Simple;
	ptr->RunProcess = RunModel_ANSpikeGen_Simple;
	ptr->SetRanSeed = SetRanSeed_ANSpikeGen_Simple;
	ptr->SetPulseDuration = SetPulseDuration_ANSpikeGen_Simple;
	ptr->SetPulseMagnitude = SetPulseMagnitude_ANSpikeGen_Simple;
	ptr->SetRefractoryPeriod = SetRefractoryPeriod_ANSpikeGen_Simple;
	return(ptr);

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
	static const char *funcName = "SetUniParList_ANSpikeGen_Simple";
	UniParPtr	pars;

	if ((simpleSGPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  ANSPIKEGEN_SIMPLE_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = simpleSGPtr->parList->pars;
	SetPar_UniParMgr(&pars[ANSPIKEGEN_SIMPLE_RANSEED], "RAN_SEED",
	  "Random number seed (0 produces a different seed each run.",
	  UNIPAR_LONG,
	  &simpleSGPtr->ranSeed, NULL,
	  (void * (*)) SetRanSeed_ANSpikeGen_Simple);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_SIMPLE_NUMFIBRES], "NUM_FIBRES",
	  "Number of fibres.",
	  UNIPAR_INT,
	  &simpleSGPtr->numFibres, NULL,
	  (void * (*)) SetNumFibres_ANSpikeGen_Simple);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_SIMPLE_PULSEDURATION], "PULSE_DURATION",
	  "Pulse duration (s).",
	  UNIPAR_REAL,
	  &simpleSGPtr->pulseDuration, NULL,
	  (void * (*)) SetPulseDuration_ANSpikeGen_Simple);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_SIMPLE_PULSEMAGNITUDE], "MAGNITUDE",
	  "Pulse magnitude (arbitrary units).",
	  UNIPAR_REAL,
	  &simpleSGPtr->pulseMagnitude, NULL,
	  (void * (*)) SetPulseMagnitude_ANSpikeGen_Simple);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_SIMPLE_REFRACTORYPERIOD], "REFRAC_PERIOD",
	  "Refractory period (s).",
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
	static const char	*funcName = "GetUniParListPtr_ANSpikeGen_Simple";

	if (simpleSGPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (simpleSGPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
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
	static const char *funcName = "SetRanSeed_ANSpikeGen_Simple";

	if (simpleSGPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
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
	static const char	*funcName = "SetNumFibres_ANSpikeGen_Simple";

	if (simpleSGPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (theNumFibres < 1) {
		NotifyError("%s: Illegal no. of fibres (%d).", funcName, theNumFibres);
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
	static const char	*funcName = "SetPulseDuration_ANSpikeGen_Simple";

	if (simpleSGPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (thePulseDuration < 0.0) {
		NotifyError("%s: Pulse duration must be greater than zero.\n",
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
	static const char	*funcName = "SetPulseMagnitude_ANSpikeGen_Simple";

	if (simpleSGPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
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
	static const char *funcName = "SetRefractoryPeriod_ANSpikeGen_Simple";

	if (simpleSGPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (theRefractoryPeriod < 0.0) {
		NotifyError("%s: Refractory period must be greater than zero.\n",
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
	static const char *funcName = "CheckPars_ANSpikeGen_Simple";
	BOOLN	ok;
	
	ok = TRUE;
	if (simpleSGPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!simpleSGPtr->ranSeedFlag) {
		NotifyError("%s: Random number seed not correctly set.", funcName);
		ok = FALSE;
	}
	if (!simpleSGPtr->numFibresFlag) {
		NotifyError("%s: numFibres variable not set.", funcName);
		ok = FALSE;
	}
	if (!simpleSGPtr->pulseDurationFlag) {
		NotifyError("%s: pulseDuration variable not set.", funcName);
		ok = FALSE;
	}
	if (!simpleSGPtr->pulseMagnitudeFlag) {
		NotifyError("%s: pulseMagnitude variable not set.", funcName);
		ok = FALSE;
	}
	if (!simpleSGPtr->refractoryPeriodFlag) {
		NotifyError("%s: Refractory period not correctly set.", funcName);
		ok = FALSE;
	}
	if (simpleSGPtr->pulseDuration >= simpleSGPtr->refractoryPeriod) {
		NotifyError("%s: Pulse duration must be smaller than the\n"
		  "refractory period, %g ms (%g ms).", funcName, MSEC(
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
	static const char *funcName = "PrintPars_ANSpikeGen_Simple";

	if (!CheckPars_ANSpikeGen_Simple()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Simple Post-Synaptic Firing Module "
	  "Parameters:-\n");
	DPrint("\tRandom number seed = %ld,",
	  simpleSGPtr->ranSeed);
	DPrint("\tNo. of fibres = %d,\n", simpleSGPtr->numFibres);
	DPrint("\tPulse duration = %g ms,",
	  MSEC(simpleSGPtr->pulseDuration));
	DPrint("\tPulse magnitude = %g (nA?),\n",
	  simpleSGPtr->pulseMagnitude);
	DPrint("\tRefractory Period = %g ms\n",
	  MSEC(simpleSGPtr->refractoryPeriod));
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.
 */
 
BOOLN
ReadPars_ANSpikeGen_Simple(char *fileName)
{
	static const char *funcName = "ReadPars_ANSpikeGen_Simple";
	BOOLN	ok;
	char	*filePath;
	long	ranSeed;
	int		numFibres;
	double	pulseDuration, pulseMagnitude, refractoryPeriod;
    FILE    *fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(filePath, "r")) == NULL) {
        NotifyError("%s: Cannot open data file '%s'.\n", funcName, filePath);
		return(FALSE);
    }
    DPrint("%s: Reading from '%s':\n", funcName, filePath);
    Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, "%ld", &ranSeed))
		ok = FALSE;
 	if (!GetPars_ParFile(fp, "%d", &numFibres))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &pulseDuration))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &pulseMagnitude))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &refractoryPeriod))
		ok = FALSE;
	fclose(fp);
    Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "\
		  "parameter file '%s'.", funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_ANSpikeGen_Simple(ranSeed, numFibres, pulseDuration,
	  pulseMagnitude, refractoryPeriod)) {
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
InitProcessVariables_ANSpikeGen_Simple(EarObjectPtr data)
{
	static const char *funcName = "InitProcessVariables_ANSpikeGen_Simple";
	int		i, arrayLength;
	double	timeGreaterThanRefractoryPeriod;
	
	if (simpleSGPtr->updateProcessVariablesFlag || data->updateProcessFlag ||
	  (data->timeIndex == PROCESS_START_TIME)) {
		arrayLength = data->outSignal->numChannels * simpleSGPtr->numFibres;
		if (simpleSGPtr->updateProcessVariablesFlag ||
		  data->updateProcessFlag) {
			FreeProcessVariables_ANSpikeGen_Simple();
			if ((simpleSGPtr->timer = (double *)
			  calloc(arrayLength, sizeof(double))) == NULL) {
			 	NotifyError("%s: Out of memory for timer array.", funcName);
			 	return(FALSE);
			}
			if ((simpleSGPtr->remainingPulseTime = (double *)
			  calloc(arrayLength, sizeof(double))) == NULL) {
			 	NotifyError("%s: Out of memory for remainingPulseTime array.",
			 	  funcName);
			 	return(FALSE);
			}
			SetGlobalSeed_Random(simpleSGPtr->ranSeed);
			simpleSGPtr->updateProcessVariablesFlag = FALSE;
		}
		timeGreaterThanRefractoryPeriod = simpleSGPtr->refractoryPeriod +
		  data->outSignal->dt;
		for (i = 0; i < arrayLength; i++) {
			simpleSGPtr->timer[i] = timeGreaterThanRefractoryPeriod;
			simpleSGPtr->remainingPulseTime[i] = 0.0;
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
FreeProcessVariables_ANSpikeGen_Simple(void)
{
	if (simpleSGPtr->timer) {
		free(simpleSGPtr->timer);
		simpleSGPtr->timer = NULL;
	}
	if (simpleSGPtr->remainingPulseTime) {
		free(simpleSGPtr->remainingPulseTime);
		simpleSGPtr->remainingPulseTime = NULL;
	}
	simpleSGPtr->updateProcessVariablesFlag = TRUE;

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_ANSpikeGen_Simple(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_ANSpikeGen_Simple";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
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
	static const char	*funcName = "InitModule_ANSpikeGen_Simple";

	if (!SetParsPointer_ANSpikeGen_Simple(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_ANSpikeGen_Simple(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = simpleSGPtr;
	theModule->CheckPars = CheckPars_ANSpikeGen_Simple;
	theModule->Free = Free_ANSpikeGen_Simple;
	theModule->GetUniParListPtr = GetUniParListPtr_ANSpikeGen_Simple;
	theModule->PrintPars = PrintPars_ANSpikeGen_Simple;
	theModule->ReadPars = ReadPars_ANSpikeGen_Simple;
	theModule->RunProcess = RunModel_ANSpikeGen_Simple;
	theModule->SetParsPointer = SetParsPointer_ANSpikeGen_Simple;
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
CheckData_ANSpikeGen_Simple(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_ANSpikeGen_Simple";

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInit_SignalData(data->inSignal[0], "CheckData_ANSpikeGen_Simple"))
		return(FALSE);
	if (simpleSGPtr->pulseDuration < data->inSignal[0]->dt) {
		NotifyError("%s: Pulse duration is too small for sampling\n"
		  "interval, %g ms (%g ms)\n", funcName,
		  MSEC(data->inSignal[0]->dt), MSEC(simpleSGPtr->pulseDuration));
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
	static const char *funcName = "RunModel_ANSpikeGen_Simple";
	register	ChanData	*inPtr, *outPtr;
	register	double		*timerPtr, *remainingPulseTimePtr;
	int		i, chan;
	double	dt;
	ChanLen	j;
	
	if (!CheckPars_ANSpikeGen_Simple())
		return(FALSE);
	if (!CheckData_ANSpikeGen_Simple(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "Simple Post-Synaptic Spike Firing");
	if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels,
	  data->inSignal[0]->length, data->inSignal[0]->dt)) {
		NotifyError("%s: Could not initialise output signal.", funcName);
		return(FALSE);
	}
	if (!InitProcessVariables_ANSpikeGen_Simple(data)) {
		NotifyError("%s: Could not initialise the process variables.",
		  funcName);
		return(FALSE);
	}
	dt = data->inSignal[0]->dt;
	for (chan = 0; chan < data->outSignal->numChannels; chan++) {
		outPtr = data->outSignal->channel[chan];
		for (j = 0; j < data->outSignal->length; j++)
			*outPtr++ = 0.0;
	}
	for (i = 0, timerPtr = simpleSGPtr->timer, remainingPulseTimePtr =
	  simpleSGPtr->remainingPulseTime; i < simpleSGPtr->numFibres; i++)
		for (chan = 0; chan < data->outSignal->numChannels; chan++) {
			inPtr = data->inSignal[0]->channel[chan];
			outPtr = data->outSignal->channel[chan];
			for (j = 0; j < data->outSignal->length; j++) {
				if ((*timerPtr > simpleSGPtr->refractoryPeriod) && 
				  (*inPtr > Ran01_Random(&randomNumSeed))) {
					*remainingPulseTimePtr = simpleSGPtr->pulseDuration;
					*timerPtr = 0.0;
				}
				if (*remainingPulseTimePtr >= dt) {
					*outPtr += simpleSGPtr->pulseMagnitude;
					*remainingPulseTimePtr -= dt;
				};
				*timerPtr += dt;
				inPtr++;
				outPtr++;
			}
			timerPtr++;
			remainingPulseTimePtr++;
		}
	SetProcessContinuity_EarObject(data);
	return(TRUE);
		
} /* RunModel_ANSpikeGen_Simple */
