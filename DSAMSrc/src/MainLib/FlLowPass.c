/**********************
 *
 * File:		FlLowPass.c
 * Purpose:		This is an implementation of a 1st order Butterworth low-pass
 *				filter.  The filter is used to approximate the effects of
 *				dendrite filtering.
 * Comments:	It uses the filter IIR filter from the UtFilter module.
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		03 Dec 1996
 * Copyright:	(c) 1999, University of Essex
 *
 **********************/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "FiParFile.h"
#include "UtFilters.h"
#include "FlLowPass.h"

/******************************************************************************/
/**************************** Global subroutines ******************************/
/******************************************************************************/

LowPassFPtr	lowPassFPtr = NULL;

/******************************************************************************/
/**************************** Subroutines and functions ***********************/
/******************************************************************************/

/****************************** InitModeList **********************************/

/*
 * This routine intialises the Mode list array.
 */

BOOLN
InitModeList_Filter_LowPass(void)
{
	static NameSpecifier	list[] = {

					{ "NORMAL", DENDRITE_MODE_NORMAL },
					{ "SCALED", DENDRITE_MODE_SCALED },
					{ "", DENDRITE_MODE_NULL }
				};
	lowPassFPtr->modeList = list;
	return(TRUE);

}

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
Init_Filter_LowPass(ParameterSpecifier parSpec)
{
	static const char *funcName = "Init_Filter_LowPass";

	if (parSpec == GLOBAL) {
		if (lowPassFPtr != NULL)
			Free_Filter_LowPass();
		if ((lowPassFPtr = (LowPassFPtr) malloc(sizeof(LowPassF))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (lowPassFPtr == NULL) { 
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	lowPassFPtr->parSpec = parSpec;
	lowPassFPtr->updateProcessVariablesFlag = TRUE;
	lowPassFPtr->modeFlag = FALSE;
	lowPassFPtr->cutOffFrequencyFlag = FALSE;
	lowPassFPtr->signalMultiplierFlag = FALSE;
	lowPassFPtr->mode = 0;
	lowPassFPtr->cutOffFrequency = 0.0;
	lowPassFPtr->signalMultiplier = 0.0;
	lowPassFPtr->numChannels = 0;
	lowPassFPtr->coefficients = NULL;

	InitModeList_Filter_LowPass();
	if (!SetUniParList_Filter_LowPass()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_Filter_LowPass();
		return(FALSE);
	}
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
Free_Filter_LowPass(void)
{
	if (lowPassFPtr == NULL)
		return(TRUE);
	FreeProcessVariables_Filter_LowPass();
	if (lowPassFPtr->parList)
		FreeList_UniParMgr(&lowPassFPtr->parList);
	if (lowPassFPtr->parSpec == GLOBAL) {
		free(lowPassFPtr);
		lowPassFPtr = NULL;
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
SetUniParList_Filter_LowPass(void)
{
	static const char *funcName = "SetUniParList_Filter_LowPass";
	UniParPtr	pars;

	if ((lowPassFPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  FILTER_LOW_PASS_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = lowPassFPtr->parList->pars;
	SetPar_UniParMgr(&pars[FILTER_LOW_PASS_MODE], "MODE",
	  "Output mode for model ('normal' or 'scaled').",
	  UNIPAR_NAME_SPEC,
	  &lowPassFPtr->mode, lowPassFPtr->modeList,
	  (void * (*)) SetMode_Filter_LowPass);
	SetPar_UniParMgr(&pars[FILTER_LOW_PASS_CUTOFFFREQUENCY], "CUT_OFF_FREQ",
	  "Cut-off frequency (3 dB down point - Hz).",
	  UNIPAR_REAL,
	  &lowPassFPtr->cutOffFrequency, NULL,
	  (void * (*)) SetCutOffFrequency_Filter_LowPass);
	SetPar_UniParMgr(&pars[FILTER_LOW_PASS_SIGNALMULTIPLIER], "SCALE",
	  "Signal multiplier scale ('scaled' mode only).",
	  UNIPAR_REAL,
	  &lowPassFPtr->signalMultiplier, NULL,
	  (void * (*)) SetSignalMultiplier_Filter_LowPass);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Filter_LowPass(void)
{
	static const char	*funcName = "GetUniParListPtr_Filter_LowPass";

	if (lowPassFPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (lowPassFPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(lowPassFPtr->parList);

}

/****************************** SetMode ***************************************/

/*
 * This function sets the module's mode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMode_Filter_LowPass(char *theMode)
{
	static const char	*funcName = "SetMode_Filter_LowPass";
	int		specifier;

	if (lowPassFPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theMode, lowPassFPtr->modeList)) ==
	  DENDRITE_MODE_NULL) {
		NotifyError("%s: Illegal mode name (%s).", funcName, theMode);
		return(FALSE);
	}
	lowPassFPtr->modeFlag = TRUE;
	lowPassFPtr->mode = specifier;
	return(TRUE);

}

/********************************* SetCutOffFrequency *************************/

/*
 * This function sets the module's lower, 3dB down cut off frequency parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetCutOffFrequency_Filter_LowPass(double theCutOffFrequency)
{
	static const char	 *funcName = "SetCutOffFrequency_Filter_LowPass";

	if (lowPassFPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	lowPassFPtr->cutOffFrequencyFlag = TRUE;
	lowPassFPtr->cutOffFrequency = theCutOffFrequency;
	lowPassFPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetSignalMultiplier ************************/

/*
 * This function sets the module's signal multiplying factor.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetSignalMultiplier_Filter_LowPass(double theSignalMultiplier)
{
	static const char	 *funcName = "SetSignalMultiplier_Filter_LowPass";

	if (lowPassFPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	lowPassFPtr->signalMultiplierFlag = TRUE;
	lowPassFPtr->signalMultiplier = theSignalMultiplier;
	return(TRUE);

}

/********************************* CheckPars **********************************/

/*
 * This routine checks that the necessary parameters for the module have been
 * correctly initialised.
 * It returns TRUE if there are no problems.
 */
 
BOOLN
CheckPars_Filter_LowPass(void)
{
	static const char *funcName = "CheckPars_Filter_LowPass";

	BOOLN ok;
	
	ok = TRUE;
	if (lowPassFPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!lowPassFPtr->modeFlag) {
		NotifyError("%s: Mode not set.", funcName);
		ok = FALSE;
	}
	if (!lowPassFPtr->cutOffFrequencyFlag) {
		NotifyError("%s: Cut-off frequency not set.", funcName);
		ok = FALSE;
	}
	if (!lowPassFPtr->signalMultiplierFlag) {
		NotifyError("%s: Signal multiplying factor not set.", funcName);
		ok = FALSE;
	}
	return(ok);

}

/********************************* SetPars ************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */
 
BOOLN
SetPars_Filter_LowPass(char *theMode, double theLowerCutOffFreq, 
  double theSignalMultiplier)
{
	static const char *funcName = "SetPars_Filter_LowPass";
	BOOLN	ok;
	
	ok = TRUE;
	if (!SetCutOffFrequency_Filter_LowPass(theLowerCutOffFreq))
		ok = FALSE;
	if (!SetMode_Filter_LowPass(theMode))
		ok = FALSE;
	if (!SetSignalMultiplier_Filter_LowPass(theSignalMultiplier))
		ok = FALSE;
	if (!ok)
		NotifyError("%s: Failed to set all module parameters.", funcName);
	return(ok);
	
}

/****************************** PrintPars *************************************/

/*
 * This program prints the parameters of the module to the standard output.
 */
 
BOOLN
PrintPars_Filter_LowPass(void)
{
	static const char *funcName = "PrintPars_Filter_LowPass";

	if (!CheckPars_Filter_LowPass()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Low-Pass Filter Module Parameters:-\n");
	DPrint("\tMode = %s,",
	  lowPassFPtr->modeList[lowPassFPtr->mode].name);
	DPrint("\tCut-off Frequency = %g Hz\n", lowPassFPtr->cutOffFrequency);
	if (lowPassFPtr->mode == DENDRITE_MODE_SCALED)
		DPrint("\tSignal multiplying factor = %g (?).\n",
		  lowPassFPtr->signalMultiplier);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.
 */
 
BOOLN
ReadPars_Filter_LowPass(char *fileName)
{
	static const char *funcName = "ReadPars_Filter_LowPass";
	BOOLN	ok;
	char	*filePath;
	char	mode[MAXLINE];
	double	cutOffFrequency, signalMultiplier;
    FILE    *fp;
    
	filePath = GetParsFileFPath_Common(fileName);
    if ((fp = fopen(filePath, "r")) == NULL) {
        NotifyError("%s: Cannot open data file '%s'.\n", funcName, filePath);
		return(FALSE);
    }
    DPrint("%s: Reading from '%s':\n", funcName, filePath);
    Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, "%s", mode))
		ok = FALSE;;
	if (!GetPars_ParFile(fp, "%lf", &cutOffFrequency))
		ok = FALSE;;
	if (!GetPars_ParFile(fp, "%lf", &signalMultiplier))
		ok = FALSE;;
   fclose(fp);
    Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "\
		  "parameter file '%s'.", funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_Filter_LowPass(mode, cutOffFrequency, signalMultiplier)) {
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
SetParsPointer_Filter_LowPass(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_Filter_LowPass";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	lowPassFPtr = (LowPassFPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Filter_LowPass(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_Filter_LowPass";

	if (!SetParsPointer_Filter_LowPass(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_Filter_LowPass(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = lowPassFPtr;
	theModule->CheckPars = CheckPars_Filter_LowPass;
	theModule->Free = Free_Filter_LowPass;
	theModule->GetUniParListPtr = GetUniParListPtr_Filter_LowPass;
	theModule->PrintPars = PrintPars_Filter_LowPass;
	theModule->ReadPars = ReadPars_Filter_LowPass;
	theModule->RunProcess = RunModel_Filter_LowPass;
	theModule->SetParsPointer = SetParsPointer_Filter_LowPass;
	return(TRUE);

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 */

BOOLN
InitProcessVariables_Filter_LowPass(EarObjectPtr data)
{
	static const char *funcName = "InitProcessVariables_Filter_LowPass";
	int		i, j;
	double	*statePtr;

	if (lowPassFPtr->updateProcessVariablesFlag || data->updateProcessFlag) {
		FreeProcessVariables_Filter_LowPass();
		if ((lowPassFPtr->coefficients =
		  (ContButt1CoeffsPtr *) calloc(data->outSignal->numChannels,
		   sizeof(ContButt1CoeffsPtr))) == NULL) {
		 	NotifyError("%s: Out of memory.", funcName);
		 	return(FALSE);
		}
		lowPassFPtr->numChannels = data->outSignal->numChannels;
	 	for (i = 0; i < data->outSignal->numChannels; i++)
			lowPassFPtr->coefficients[i] = InitIIR1ContCoeffs_Filters(
			  lowPassFPtr->cutOffFrequency, data->inSignal[0]->dt, LOWPASS);
		lowPassFPtr->updateProcessVariablesFlag = FALSE;
	} else if (data->timeIndex == PROCESS_START_TIME) {
		for (i = 0; i < data->outSignal->numChannels; i++) {
			statePtr = lowPassFPtr->coefficients[i]->state;
			for (j = 0; j < FILTERS_NUM_CONTBUTT1_STATE_VARS; j++)
				*statePtr++ = 0.0;
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
FreeProcessVariables_Filter_LowPass(void)
{
	int		i;

	if (lowPassFPtr->coefficients == NULL)
		return;
	for (i = 0; i < lowPassFPtr->numChannels; i++)
		free(lowPassFPtr->coefficients[i]);
	free(lowPassFPtr->coefficients);
	lowPassFPtr->coefficients = NULL;
	lowPassFPtr->updateProcessVariablesFlag = TRUE;

}

/**************************** RunModel ****************************************/

/*
 * This routine filters the input signal and puts the result into the output
 * signal.
 * It checks that all initialisation has been correctly carried out by calling
 * the appropriate checking routines.
 * This routine adjusts the signal inputs for the sampling interval, i.e.
 * each sample is multiplied by, pulse duration / dt.
 */

BOOLN
RunModel_Filter_LowPass(EarObjectPtr data)
{
	static const char *funcName = "RunModel_Filter_LowPass";
	int			chan;
	ChanLen		i;
	register ChanData	*inPtr, *outPtr;
	
	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}	
	SetProcessName_EarObject(data, "Low-pass filter process");
	if (!CheckPars_Filter_LowPass())
		return(FALSE);
	if (!CheckRamp_SignalData(data->inSignal[0])) {
		NotifyError("%s: Input signal not correctly initialised.", funcName);
		return(FALSE);
	}
	if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels,
	  data->inSignal[0]->length, data->inSignal[0]->dt)) {
		NotifyError("%s: Couldn't initialse output signal.", funcName);
		return(FALSE);
	}
	if (!InitProcessVariables_Filter_LowPass(data)) {
		NotifyError("%s: Could not initialise the process variables.",
		  funcName);
		return(FALSE);
	}
	for (chan = 0; chan < data->inSignal[0]->numChannels; chan++) {
		inPtr = data->inSignal[0]->channel[chan];
		outPtr = data->outSignal->channel[chan];
		switch (lowPassFPtr->mode) {
		case DENDRITE_MODE_NORMAL:
			for (i = 0; i < data->inSignal[0]->length; i++)
				*outPtr++ = *inPtr++;
			break;
		case DENDRITE_MODE_SCALED:
			for (i = 0; i < data->inSignal[0]->length; i++)
				*outPtr++ = *inPtr++ * lowPassFPtr->signalMultiplier;
			break;
		} /* switch */
		
	}

	/* Filter signal */
	
	IIR1Cont_Filters(data->outSignal, lowPassFPtr->coefficients);

	SetProcessContinuity_EarObject(data);
	return(TRUE);

}
