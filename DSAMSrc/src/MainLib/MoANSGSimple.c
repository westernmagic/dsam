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
#include "GeNSpecLists.h"
#include "FiParFile.h"
#include "UtString.h"
#include "UtRandom.h"
#include "UtParArray.h"
#include "UtANSGDist.h"
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
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (simpleSGPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	simpleSGPtr->parSpec = parSpec;
	simpleSGPtr->diagnosticMode = GENERAL_DIAGNOSTIC_OFF_MODE;
	simpleSGPtr->updateProcessVariablesFlag = TRUE;
	simpleSGPtr->ranSeed = -1;
	simpleSGPtr->numFibres = 5;
	simpleSGPtr->pulseDuration = 0.1e-3;
	simpleSGPtr->pulseMagnitude = 4.3;
	simpleSGPtr->refractoryPeriod = PS_REFRACTORY_PERIOD;
	if ((simpleSGPtr->distribution = Init_ParArray(wxT("Distribution"),
	  ModeList_ANSGDist(0), GetNumDistributionPars_ANSGDist,
	  CheckFuncPars_ANSGDist)) == NULL) {
		NotifyError(wxT("%s: Could not initialise distribution parArray structure"),
		  funcName);
		Free_ANSpikeGen_Simple();
		return(FALSE);
	}
	SetDefaultDistribution_ANSGDist(simpleSGPtr->distribution);

	if ((simpleSGPtr->diagnosticModeList = InitNameList_NSpecLists(
	  DiagModeList_NSpecLists(0), simpleSGPtr->diagFileName)) == NULL)
		return(FALSE);
	if (!SetUniParList_ANSpikeGen_Simple()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_ANSpikeGen_Simple();
		return(FALSE);
	}
	simpleSGPtr->numFibres2 = NULL;
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
	Free_ParArray(&simpleSGPtr->distribution);
	if (simpleSGPtr->diagnosticModeList)
		free(simpleSGPtr->diagnosticModeList);
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
	SetPar_UniParMgr(&pars[ANSPIKEGEN_SIMPLE_DIAGNOSTICMODE], wxT("DIAG_MODE"),
	  wxT("Diagnostic mode ('off', 'screen' or <file name>)."),
	  UNIPAR_NAME_SPEC_WITH_FILE,
	  &simpleSGPtr->diagnosticMode, simpleSGPtr->diagnosticModeList,
	  (void * (*)) SetDiagnosticMode_ANSpikeGen_Simple);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_SIMPLE_RANSEED], wxT("RAN_SEED"),
	  wxT("Random number seed (0 produces a different seed each run."),
	  UNIPAR_LONG,
	  &simpleSGPtr->ranSeed, NULL,
	  (void * (*)) SetRanSeed_ANSpikeGen_Simple);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_SIMPLE_NUMFIBRES], wxT("NUM_FIBRES"),
	  wxT("Number of fibres *** now obsolete, set by the 'DIST_MODE'***."),
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
	SetPar_UniParMgr(&pars[ANSPIKEGEN_SIMPLE_DISTRIBUTION], wxT("DISTRIBUTION"),
	  wxT("Channel fibre distribution mode ('standard', 'Gaussian' or ")
	  wxT("'dbl_Guassian')."),
	  UNIPAR_PARARRAY,
	  &simpleSGPtr->distribution, NULL,
	  (void * (*)) SetDistribution_ANSpikeGen_Simple);

	simpleSGPtr->parList->pars[ANSPIKEGEN_SIMPLE_NUMFIBRES].enabled = FALSE;
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
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(simpleSGPtr->parList);

}

/****************************** SetDiagnosticMode *****************************/

/*
 * This function sets the module's diagnosticMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDiagnosticMode_ANSpikeGen_Simple(WChar * theDiagnosticMode)
{
	static const WChar	*funcName = wxT("SetDiagnosticMode_ANSpikeGen_Simple");
	int		specifier;

	if (simpleSGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	simpleSGPtr->diagnosticMode = IdentifyDiag_NSpecLists(theDiagnosticMode,
	  simpleSGPtr->diagnosticModeList);
	return(TRUE);

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
	simpleSGPtr->numFibres = theNumFibres;
	SetStandardNumFibres_ANSGDist(simpleSGPtr->distribution, theNumFibres);
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

/****************************** SetDistribution ***********************************/

/*
 * This function sets the ParArray data structure for the module.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetDistribution_ANSpikeGen_Simple(ParArrayPtr theDistribution)
{
	static const WChar	*funcName = wxT("SetDistribution_ANSpikeGen_Simple");

	if (simpleSGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if (!CheckInit_ParArray(theDistribution, funcName)) {
		NotifyError(wxT("%s: ParArray structure not correctly set."),  funcName);
		return(FALSE);
	}
	simpleSGPtr->distribution = theDistribution; /* This doesn't actually do anything new */
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This program prints the parameters of the module to the standard output.
 */

BOOLN
PrintPars_ANSpikeGen_Simple(void)
{
	DPrint(wxT("Simple Post-Synaptic Firing Module Parameters:-\n"));
	DPrint(wxT("\tDiagnostic mode = %s\n"), simpleSGPtr->diagnosticModeList[
	  simpleSGPtr->diagnosticMode].name);
	DPrint(wxT("\tRandom number seed = %ld,"), simpleSGPtr->ranSeed);
	DPrint(wxT("\tNo. of fibres = %d,\n"), simpleSGPtr->numFibres);
	DPrint(wxT("\tPulse duration = %g ms,"), MSEC(simpleSGPtr->pulseDuration));
	DPrint(wxT("\tPulse magnitude = %g (nA?),\n"), simpleSGPtr->pulseMagnitude);
	DPrint(wxT("\tRefractory Period = %g ms\n"), MSEC(simpleSGPtr->
	  refractoryPeriod));
	PrintPars_ParArray(simpleSGPtr->distribution);
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
		NotifyError(wxT("%s: Not enough lines, or invalid parameters, in ")
		  wxT("module parameter file '%s'."), funcName, filePath);
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
	theModule->Free = Free_ANSpikeGen_Simple;
	theModule->GetUniParListPtr = GetUniParListPtr_ANSpikeGen_Simple;
	theModule->PrintPars = PrintPars_ANSpikeGen_Simple;
	theModule->ReadPars = ReadPars_ANSpikeGen_Simple;
	theModule->ResetProcess = ResetProcess_ANSpikeGen_Simple;
	theModule->RunProcess = RunModel_ANSpikeGen_Simple;
	theModule->SetParsPointer = SetParsPointer_ANSpikeGen_Simple;
	return(TRUE);

}

/**************************** ResetProcess ************************************/

/*
 * This routine resets the process variables.
 */

void
ResetProcess_ANSpikeGen_Simple(EarObjectPtr data)
{
	int		i, chan;
	double	timeGreaterThanRefractoryPeriod, *timerPtr, *remainingPulseTimePtr;
	SimpleSGPtr	p = simpleSGPtr;
	SignalDataPtr	outSignal = _OutSig_EarObject(data);

	ResetOutSignal_EarObject(data);
	timeGreaterThanRefractoryPeriod = p->refractoryPeriod + outSignal->dt;
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		timerPtr = p->timer[chan];
		remainingPulseTimePtr = p->remainingPulseTime[chan];
		for (i = 0; i < p->numFibres2[chan]; i++) {
			*timerPtr++ = timeGreaterThanRefractoryPeriod;
			*remainingPulseTimePtr++ = 0.0;
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
	SignalDataPtr	outSignal;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag || (data->
	  timeIndex == PROCESS_START_TIME)) {
		if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
			FreeProcessVariables_ANSpikeGen_Simple();
			outSignal = _OutSig_EarObject(data);
			if (!SetRandPars_EarObject(data, p->ranSeed, funcName))
				return(FALSE);
			p->numChannels = outSignal->numChannels;
			if ((p->numFibres2 = (int *) calloc(p->numChannels, sizeof(int))) ==
			  NULL) {
			 	NotifyError(wxT("%s: Out of memory for numFibres2 array."),
				  funcName);
			 	return(FALSE);
			}
			if ((p->timer = (double **) calloc(p->numChannels, sizeof(
			  double *))) == NULL) {
			 	NotifyError(wxT("%s: Out of memory for timer pointer array."),
				  funcName);
			 	return(FALSE);
			}
			if ((p->remainingPulseTime = (double **) calloc(p->numChannels,
			  sizeof(double *))) == NULL) {
			 	NotifyError(wxT("%s: Out of memory for remainingPulseTime ")
				  wxT("pointer array."), funcName);
			 	return(FALSE);
			}
			SetFibres_ANSGDist(p->numFibres2, p->distribution,
			  outSignal->info.cFArray, p->numChannels);
			for (i = 0; i < p->numChannels; i++) {
				if ((p->timer[i] = (double *) calloc(p->numFibres2[i], sizeof(
				  double))) == NULL) {
			 		NotifyError(wxT("%s: Out of memory for timer array."),
					  funcName);
			 		return(FALSE);
				}
				if ((p->remainingPulseTime[i] = (double *) calloc(p->numFibres2[i],
				  sizeof(double))) == NULL) {
			 		NotifyError(wxT("%s: Out of memory for remainingPulseTime ")
					  wxT("array."), funcName);
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

	if (p->numFibres2) {
		free(p->numFibres2);
		p->numFibres2 = NULL;
	}
	if (p->timer) {
		for (i = 0; i < p->numChannels; i++)
			if (p->timer[i])
				free(p->timer[i]);
		free(p->timer);
		p->timer = NULL;
	}
	if (p->remainingPulseTime) {
		for (i = 0; i < p->numChannels; i++)
			if (p->remainingPulseTime[i])
				free(p->remainingPulseTime[i]);
		free(p->remainingPulseTime);
		p->remainingPulseTime = NULL;
	}
	p->updateProcessVariablesFlag = TRUE;

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
	if (simpleSGPtr->pulseDuration >= simpleSGPtr->refractoryPeriod) {
		NotifyError(wxT("%s: Pulse duration must be smaller than the\n")
		  wxT("refractory period, %g ms (%g ms)."), funcName, MSEC(
		  simpleSGPtr->refractoryPeriod), MSEC(simpleSGPtr->pulseDuration));
		return(FALSE);
	}
	if (simpleSGPtr->pulseDuration < _InSig_EarObject(data, 0)->dt) {
		NotifyError(wxT("%s: Pulse duration is too small for sampling\n")
		  wxT("interval, %g ms (%g ms)\n"), funcName, MSEC(_InSig_EarObject(
		  data, 0)->dt), MSEC(simpleSGPtr->pulseDuration));
		return(FALSE);
	}
	if (!CheckPars_ParArray(simpleSGPtr->distribution, _InSig_EarObject(data, 0))) {
		NotifyError(wxT("%s: Distribution parameters invalid."), funcName);
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
	SignalDataPtr	outSignal;
	SimpleSGPtr	p = simpleSGPtr;

	if (!data->threadRunFlag) {
		if (!CheckData_ANSpikeGen_Simple(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Simple Post-Synaptic Spike ")
		  wxT("Firing"));
		if (!InitOutSignal_EarObject(data, _InSig_EarObject(data, 0)->
		  numChannels, _InSig_EarObject(data, 0)->length, _InSig_EarObject(data,
		  0)->dt)) {
			NotifyError(wxT("%s: Could not initialise output signal."),
			  funcName);
			return(FALSE);
		}
		if (!InitProcessVariables_ANSpikeGen_Simple(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		if (p->diagnosticMode != GENERAL_DIAGNOSTIC_OFF_MODE) {
			OpenDiagnostics_NSpecLists(&p->fp, p->diagnosticModeList, p->diagnosticMode);
			PrintFibres_ANSGDist(p->fp, wxT(""), p->numFibres2,
			  _OutSig_EarObject(data)->info.cFArray, p->numChannels);
			CloseDiagnostics_NSpecLists(&p->fp);
		}
		p->dt = _InSig_EarObject(data, 0)->dt;
		for (chan = 0; chan < _OutSig_EarObject(data)->numChannels; chan++) {
			outPtr = _OutSig_EarObject(data)->channel[chan];
			for (j = 0; j < _OutSig_EarObject(data)->length; j++)
				*outPtr++ = 0.0;
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	outSignal = _OutSig_EarObject(data);
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		timerPtr = p->timer[chan];
		remainingPulseTimePtr = p->remainingPulseTime[chan];
		for (i = 0; i < p->numFibres2[chan]; i++) {
			inPtr = _InSig_EarObject(data, 0)->channel[chan];
			outPtr = outSignal->channel[chan];
			for (j = 0; j < outSignal->length; j++) {
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
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

} /* RunModel_ANSpikeGen_Simple */
