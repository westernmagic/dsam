/**********************
 *
 * File:		AnALSR.c
 * Purpose:		This proces module calculates the average localised
 *				synchronised rate (ALSR).
 *				Yound E. D. and Sachs M. B. (1979) "Representation of steady-
 *				state vowels in the temporal aspects of the discharge patterns
 *				of populations of auditory-nerve fibers", J. Acoust. Soc. Am,
 *				Vol 66, pages 1381-1403.
 * Comments:	Written using ModuleProducer version 1.3.3 (Oct  1 2001).
 *				It expects multi-channel input from a period histogram
 * Author:		L. P. O'Mard
 * Created:		22 Jan 2002
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
#include "AnALSR.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

ALSRPtr	aLSRPtr = NULL;

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
Free_Analysis_ALSR(void)
{
	/* static const char	*funcName = "Free_Analysis_ALSR";  */

	if (aLSRPtr == NULL)
		return(FALSE);
	FreeProcessVariables_Analysis_ALSR();
	if (aLSRPtr->parList)
		FreeList_UniParMgr(&aLSRPtr->parList);
	if (aLSRPtr->parSpec == GLOBAL) {
		free(aLSRPtr);
		aLSRPtr = NULL;
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
Init_Analysis_ALSR(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_Analysis_ALSR";

	if (parSpec == GLOBAL) {
		if (aLSRPtr != NULL)
			Free_Analysis_ALSR();
		if ((aLSRPtr = (ALSRPtr) malloc(sizeof(ALSR))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (aLSRPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	aLSRPtr->parSpec = parSpec;
	aLSRPtr->updateProcessVariablesFlag = TRUE;
	aLSRPtr->lowerAveLimitFlag = TRUE;
	aLSRPtr->upperAveLimitFlag = TRUE;
	aLSRPtr->normaliseFlag = TRUE;
	aLSRPtr->lowerAveLimit = -1.0;
	aLSRPtr->upperAveLimit = 1.0;
	aLSRPtr->normalise = 1.0;

	if (!SetUniParList_Analysis_ALSR()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_Analysis_ALSR();
		return(FALSE);
	}
	aLSRPtr->modulusFT = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_Analysis_ALSR(void)
{
	static const char *funcName = "SetUniParList_Analysis_ALSR";
	UniParPtr	pars;

	if ((aLSRPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  ANALYSIS_ALSR_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = aLSRPtr->parList->pars;
	SetPar_UniParMgr(&pars[ANALYSIS_ALSR_LOWERAVELIMIT], "LOWER_LIMIT",
	  "Averaging window limit below CF (octaves)",
	  UNIPAR_REAL,
	  &aLSRPtr->lowerAveLimit, NULL,
	  (void * (*)) SetLowerAveLimit_Analysis_ALSR);
	SetPar_UniParMgr(&pars[ANALYSIS_ALSR_UPPERAVELIMIT], "UPPER_LIMIT",
	  "Averaging window limit above CF (octaves)",
	  UNIPAR_REAL,
	  &aLSRPtr->upperAveLimit, NULL,
	  (void * (*)) SetUpperAveLimit_Analysis_ALSR);
	SetPar_UniParMgr(&pars[ANALYSIS_ALSR_NORMALISE], "NORMALISE",
	  "Normalisation factor (units.)",
	  UNIPAR_REAL,
	  &aLSRPtr->normalise, NULL,
	  (void * (*)) SetNormalise_Analysis_ALSR);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Analysis_ALSR(void)
{
	static const char	*funcName = "GetUniParListPtr_Analysis_ALSR";

	if (aLSRPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (aLSRPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(aLSRPtr->parList);

}

/****************************** SetLowerAveLimit ******************************/

/*
 * This function sets the module's lowerAveLimit parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLowerAveLimit_Analysis_ALSR(double theLowerAveLimit)
{
	static const char	*funcName = "SetLowerAveLimit_Analysis_ALSR";

	if (aLSRPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	aLSRPtr->lowerAveLimitFlag = TRUE;
	aLSRPtr->updateProcessVariablesFlag = TRUE;
	aLSRPtr->lowerAveLimit = theLowerAveLimit;
	return(TRUE);

}

/****************************** SetUpperAveLimit ******************************/

/*
 * This function sets the module's upperAveLimit parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetUpperAveLimit_Analysis_ALSR(double theUpperAveLimit)
{
	static const char	*funcName = "SetUpperAveLimit_Analysis_ALSR";

	if (aLSRPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	aLSRPtr->upperAveLimitFlag = TRUE;
	aLSRPtr->updateProcessVariablesFlag = TRUE;
	aLSRPtr->upperAveLimit = theUpperAveLimit;
	return(TRUE);

}

/****************************** SetNormalise **********************************/

/*
 * This function sets the module's normalise parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNormalise_Analysis_ALSR(double theNormalise)
{
	static const char	*funcName = "SetNormalise_Analysis_ALSR";

	if (aLSRPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	aLSRPtr->normaliseFlag = TRUE;
	aLSRPtr->updateProcessVariablesFlag = TRUE;
	aLSRPtr->normalise = theNormalise;
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
CheckPars_Analysis_ALSR(void)
{
	static const char	*funcName = "CheckPars_Analysis_ALSR";
	BOOLN	ok;

	ok = TRUE;
	if (aLSRPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!aLSRPtr->lowerAveLimitFlag) {
		NotifyError("%s: lowerAveLimit variable not set.", funcName);
		ok = FALSE;
	}
	if (!aLSRPtr->upperAveLimitFlag) {
		NotifyError("%s: upperAveLimit variable not set.", funcName);
		ok = FALSE;
	}
	if (!aLSRPtr->normaliseFlag) {
		NotifyError("%s: normalise variable not set.", funcName);
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
PrintPars_Analysis_ALSR(void)
{
	static const char	*funcName = "PrintPars_Analysis_ALSR";

	if (!CheckPars_Analysis_ALSR()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("ALSR Analysis Module Parameters:-\n");
	DPrint("\tAveraging window limit below CF = %g,\n", aLSRPtr->lowerAveLimit);
	DPrint("\tAveraging window limit above CF = %g,\n", aLSRPtr->upperAveLimit);
	DPrint("\tNormalisation factor = %g(units).\n", aLSRPtr->normalise);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Analysis_ALSR(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_Analysis_ALSR";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	aLSRPtr = (ALSRPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 * It also initialises the process structure.
 */

BOOLN
InitModule_Analysis_ALSR(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_Analysis_ALSR";

	if (!SetParsPointer_Analysis_ALSR(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_Analysis_ALSR(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = aLSRPtr;
	theModule->CheckPars = CheckPars_Analysis_ALSR;
	theModule->Free = Free_Analysis_ALSR;
	theModule->GetUniParListPtr = GetUniParListPtr_Analysis_ALSR;
	theModule->PrintPars = PrintPars_Analysis_ALSR;
	theModule->RunProcess = Calc_Analysis_ALSR;
	theModule->SetParsPointer = SetParsPointer_Analysis_ALSR;
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
CheckData_Analysis_ALSR(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_Analysis_ALSR";

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInit_SignalData(data->inSignal[0], funcName))
		return(FALSE);
	if (data->inSignal[0]->numChannels < 2) {
		NotifyError("%s: This module expects multi-channel input.", funcName);
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
InitProcessVariables_Analysis_ALSR(EarObjectPtr data)
{
	static const char	*funcName = "InitProcessVariables_Analysis_ALSR";

	if (aLSRPtr->updateProcessVariablesFlag || data->updateProcessFlag) {
		/*** Additional update flags can be added to above line ***/
		FreeProcessVariables_Analysis_ALSR();
		if ((aLSRPtr->modulusFT = Init_EarObject("Ana_FourierT")) == NULL) {
			NotifyError("%s: Could not initialise the 'modulusFT' EarObject.",
			  funcName);
			return(FALSE);
		}
		aLSRPtr->updateProcessVariablesFlag = FALSE;
	}
	if (!SetPar_ModuleMgr(aLSRPtr->modulusFT, "output_mode", "modulus")) {
		NotifyError("%s: Could not set the 'Ana_FourierT' output mode.",
		  funcName);
		return(FALSE);
	}
	if (data->timeIndex == PROCESS_START_TIME) {
		ResetProcess_EarObject(aLSRPtr->modulusFT);
		/*** Put reset (to zero ?) code here ***/
	}
	return(TRUE);

}

/****************************** FreeProcessVariables **************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

BOOLN
FreeProcessVariables_Analysis_ALSR(void)
{
	/*static const char	*funcName = "FreeProcessVariables_Analysis_ALSR";*/

	Free_EarObject(&aLSRPtr->modulusFT);
	return(TRUE);

}

/****************************** Calc ******************************************/

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
 * The output is normalised by the first (the second sample) FFT component.
 */

BOOLN
Calc_Analysis_ALSR(EarObjectPtr data)
{
	static const char	*funcName = "Calc_Analysis_ALSR";
	register ChanData	 *outPtr;
	char	channelTitle[MAXLINE];
	int		chan, minChan, maxChan, minWinChan, maxWinChan, numChannels;
	double	dF, *cFs;
	ChanLen	i, minIndex, maxIndex;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckPars_Analysis_ALSR())
		return(FALSE);
	if (!CheckData_Analysis_ALSR(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "ALSR Analysis Module process");

	/*** Example Initialise output signal - ammend/change if required. ***/
	if (!InitOutSignal_EarObject(data, 1, data->inSignal[0]->length, data->
	  inSignal[0]->dt)) {
		NotifyError("%s: Cannot initialise output channels.", funcName);
		return(FALSE);
	}

	if (!InitProcessVariables_Analysis_ALSR(data)) {
		NotifyError("%s: Could not initialise the process variables.",
		  funcName);
		return(FALSE);
	}
	SetLocalInfoFlag_SignalData(data->outSignal, TRUE);
	SetInfoSampleTitle_SignalData(data->outSignal, "Frequency (Hz) ");
	snprintf(channelTitle, MAXLINE, "ALSR function (+%g / -%g octaves)",
	  aLSRPtr->lowerAveLimit, aLSRPtr->upperAveLimit);
	SetInfoChannelTitle_SignalData(data->outSignal, channelTitle);
	SetInfoChannelLabels_SignalData(data->outSignal, NULL);
	SetInfoCFArray_SignalData(data->outSignal, NULL);
	if (!GetChannelLimits_SignalData(data->inSignal[0], &minChan, &maxChan, 
	  aLSRPtr->lowerAveLimit, aLSRPtr->upperAveLimit,
	  SIGNALDATA_LIMIT_MODE_OCTAVE)) {
		NotifyError("%s: Could not find a channel limits for signal.",
		  funcName);
		return(FALSE);
	}
	/* "Manual" connection of original input signal to the linear filter. */
	aLSRPtr->modulusFT->inSignal[0] = data->inSignal[0];
	if (!RunProcess_ModuleMgr(aLSRPtr->modulusFT)) {
		NotifyError("%s: Could not run 'Ana_FourierT' process.", funcName);
		return(FALSE);
	}
	dF = aLSRPtr->modulusFT->outSignal->dt;
	cFs = data->inSignal[0]->info.cFArray;
	SetSamplingInterval_SignalData(data->outSignal, dF);
	minIndex = (ChanLen) floor(cFs[minChan] / dF + 0.5);
	maxIndex = (ChanLen) floor(cFs[maxChan] / dF + 0.5);
	outPtr = data->outSignal->channel[0];
	for (i = 0; i < minIndex - 1; i++)
		*outPtr++ = 0.0;
	outPtr = data->outSignal->channel[0] + minIndex;
	for (i = minIndex; i <= maxIndex; i++, outPtr++) {
		GetWindowLimits_SignalData(data->inSignal[0], &minWinChan, &maxWinChan,
		  i * dF, aLSRPtr->lowerAveLimit, aLSRPtr->upperAveLimit,
		  SIGNALDATA_LIMIT_MODE_OCTAVE);
		for (chan = minWinChan, *outPtr = 0.0; chan <= maxWinChan; chan++)
			*outPtr += aLSRPtr->modulusFT->outSignal->channel[chan][i];
	}
	for (i = maxIndex + 1; i < data->outSignal->length; i++)
		*outPtr++ = 0.0;
	outPtr = data->outSignal->channel[0] + minIndex;
	numChannels = maxIndex - minIndex + 1;
	for (i = minIndex; i <= maxIndex; i++)
		*outPtr++ /= numChannels;
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

