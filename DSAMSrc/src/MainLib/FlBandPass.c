/**********************
 *
 * File:		FlBandPass.c
 * Purpose:		This is an implementation of a pre-emphasis band pass filter
 *				using the filters.c module.
 *				Beauchamp K. and Yuen C. (1979) "Digital Methods for signal
 *				analysis", George Allen & Unwin, London. P 256-257.
 * Comments:	The band-pass filter is used to model the effects of the
 *				outer- and middle-ear on an input signal.
 *				LPO (10-09-95): A pre-attenuation parameter has been added to
 *				the module definition.
 *				20-07-98 LPO: Name changed from MoPreEmph.[ch]
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		20 Jul 1998
 * Copyright:	(c) 1998, University of Essex
 *
 **********************/ 
 
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
#include "UtFilters.h"
#include "FlBandPass.h"

/******************************************************************************/
/**************************** Global subroutines ******************************/
/******************************************************************************/

BandPassFPtr	bandPassFPtr = NULL;

/******************************************************************************/
/**************************** Subroutines and functions ***********************/
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
Init_Filter_BandPass(ParameterSpecifier parSpec)
{
	static const char *funcName = "Init_Filter_BandPass";

	if (parSpec == GLOBAL) {
		if (bandPassFPtr != NULL)
			Free_Filter_BandPass();
		if ((bandPassFPtr = (BandPassFPtr) malloc(sizeof(BandPassF))) ==
		  NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (bandPassFPtr == NULL) { 
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	bandPassFPtr->parSpec = parSpec;
	bandPassFPtr->updateProcessVariablesFlag = TRUE;
	bandPassFPtr->cascadeFlag = FALSE;
	bandPassFPtr->upperCutOffFreqFlag = FALSE;
	bandPassFPtr->lowerCutOffFreqFlag = FALSE;
	bandPassFPtr->preAttenuationFlag = FALSE;
	bandPassFPtr->cascade = 0;
	bandPassFPtr->upperCutOffFreq = 0.0;
	bandPassFPtr->lowerCutOffFreq = 0.0;
	bandPassFPtr->preAttenuation = 0.0;

	if (!SetUniParList_Filter_BandPass()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_Filter_BandPass();
		return(FALSE);
	}
	bandPassFPtr->coefficients = NULL;
	bandPassFPtr->numChannels = 0;
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
Free_Filter_BandPass(void)
{
	if (bandPassFPtr == NULL)
		return(TRUE);
	FreeProcessVariables_Filter_BandPass();
	if (bandPassFPtr->coefficients)
		free(bandPassFPtr->coefficients);
	if (bandPassFPtr->parList)
		FreeList_UniParMgr(&bandPassFPtr->parList);
	if (bandPassFPtr->parSpec == GLOBAL) {
		free(bandPassFPtr);
		bandPassFPtr = NULL;
	}
	return(TRUE);

}

/********************************* SetUniParList ******************************/

/*
 * This routine initialises and sets the module's universal parameter list.
 * This list provides universal access to the module's parameters.
 */
 
BOOLN
SetUniParList_Filter_BandPass(void)
{
	static const char *funcName = "SetUniParList_Filter_BandPass";
	UniParPtr	pars;

	if ((bandPassFPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  BANDPASS_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = bandPassFPtr->parList->pars;
	SetPar_UniParMgr(&pars[BANDPASS_FILTERCASCADE], "CASCADE",
	  "Filter cascade.",
	  UNIPAR_INT,
	  &bandPassFPtr->cascade, NULL,
	  (void * (*)) SetCascade_Filter_BandPass);
	SetPar_UniParMgr(&pars[BANDPASS_PREATTENUATION], "ATTENUATION",
	  "Pre-attenuation for filter (dB).",
	  UNIPAR_REAL,
	  &bandPassFPtr->preAttenuation, NULL,
	  (void * (*)) SetPreAttenuation_Filter_BandPass);
	SetPar_UniParMgr(&pars[BANDPASS_LOWERCUTOFFFREQ], "LOWER_FREQ",
	  "Lower, 3 dB down cut-off frequency (Hz).",
	  UNIPAR_REAL,
	  &bandPassFPtr->lowerCutOffFreq, NULL,
	  (void * (*)) SetLowerCutOffFreq_Filter_BandPass);
	SetPar_UniParMgr(&pars[BANDPASS_UPPERCUTOFFFREQ], "UPPER_FREQ",
	  "Upper, 3 dB down cut-off frequency (Hz).",
	  UNIPAR_REAL,
	  &bandPassFPtr->upperCutOffFreq, NULL,
	  (void * (*)) SetUpperCutOffFreq_Filter_BandPass);

	return(TRUE);

}

/********************************* GetUniParListPtr ***************************/

/*
 * This routine returns a pointer to the module's universal parameter list.
 */

UniParListPtr
GetUniParListPtr_Filter_BandPass(void)
{
	static const char *funcName = "GetUniParListPtr_Filter_BandPass";

	if (bandPassFPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (bandPassFPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(bandPassFPtr->parList);

}

/********************************* CheckPars **********************************/

/*
 * This routine checks that the necessary parameters for the module have been
 * correctly initialised.
 * It returns TRUE if there are no problems.
 */
 
BOOLN
CheckPars_Filter_BandPass(void)
{
	static const char *funcName = "CheckPars_Filter_BandPass";
	BOOLN	ok;
	
	ok = TRUE;
	if (bandPassFPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!bandPassFPtr->cascadeFlag) {
		NotifyError("%s: Filter cascade not set.", funcName);
		ok = FALSE;
	}
	if (!bandPassFPtr->lowerCutOffFreqFlag) {
		NotifyError("%s: Lower cut off frequency not set.", funcName);
		ok = FALSE;
	}
	if (!bandPassFPtr->upperCutOffFreqFlag) {
		NotifyError("%s: Upper cut off frequency not set.", funcName);
		ok = FALSE;
	}
	if (!bandPassFPtr->preAttenuationFlag) {
		NotifyError("%s: Pre-attenuation parameter not set.", funcName);
		ok = FALSE;
	}
	return(ok);

}

/********************************* SetCascade *********************************/

/*
 * This function sets the module's lower, 3dB down cut off frequency parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetCascade_Filter_BandPass(int theCascade)
{
	static const char *funcName = "SetCascade_Filter_BandPass";

	if (bandPassFPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	bandPassFPtr->cascadeFlag = TRUE;
	if (theCascade < 1) {
		NotifyError("%s: This value must be greater than 0 (%d).\n", funcName,
		  theCascade);
		return(FALSE);
	}
	bandPassFPtr->cascade = theCascade;
	return(TRUE);

}

/********************************* SetLowerCutOffFreq *************************/

/*
 * This function sets the module's lower, 3dB down cut off frequency parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetLowerCutOffFreq_Filter_BandPass(double theLowerCutOffFreq)
{
	static const char	*funcName = "SetLowerCutOffFreq_Filter_BandPass";

	if (bandPassFPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	bandPassFPtr->lowerCutOffFreqFlag = TRUE;
	bandPassFPtr->lowerCutOffFreq = theLowerCutOffFreq;
	bandPassFPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetUpperCutOffFreq *************************/

/*
 * This function sets the module's upper, 3dB down cut off frequency parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetUpperCutOffFreq_Filter_BandPass(double theUpperCutOffFreq)
{
	static const char	*funcName = "SetUpperCutOffFreq_Filter_BandPass";

	if (bandPassFPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	bandPassFPtr->upperCutOffFreqFlag = TRUE;
	bandPassFPtr->upperCutOffFreq = theUpperCutOffFreq;
	bandPassFPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************** SetPreAttenuation *************************************/

/*
 * This function sets the module's total filter attenuation parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPreAttenuation_Filter_BandPass(double thePreAttenuation)
{
	static const char	*funcName = "SetPreAttenuation_Filter_BandPass";

	if (bandPassFPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	bandPassFPtr->preAttenuationFlag = TRUE;
	bandPassFPtr->preAttenuation = thePreAttenuation;
	bandPassFPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);
	
}

/********************************* SetPars ************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */
 
BOOLN
SetPars_Filter_BandPass(int theCascade, double theLowerCutOffFreq,
  double theUpperCutOffFreq, double preAttenuation)
{
	static const char *funcName = "SetPars_Filter_BandPass";
	BOOLN	ok;
	
	ok = TRUE;
	if (!SetCascade_Filter_BandPass(theCascade))
		ok = FALSE;
	if (!SetLowerCutOffFreq_Filter_BandPass(theLowerCutOffFreq))
		ok = FALSE;
	if (!SetUpperCutOffFreq_Filter_BandPass(theUpperCutOffFreq))
		ok = FALSE;
	if (!SetPreAttenuation_Filter_BandPass(preAttenuation))
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
PrintPars_Filter_BandPass(void)
{
	static const char *funcName = "PrintPars_Filter_BandPass";

	if (!CheckPars_Filter_BandPass()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Band Pass Filter Module Parameters:-\n");
	DPrint("\tFilter cascade = %d,\tPre-attenuation = %g dB\n",
	  bandPassFPtr->cascade, bandPassFPtr->preAttenuation);
	DPrint("\tLower/Upper cut-off Frequencies = %g / %g Hz."\
	  "\n", bandPassFPtr->lowerCutOffFreq, bandPassFPtr->upperCutOffFreq);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.
 */
 
BOOLN
ReadPars_Filter_BandPass(char *fileName)
{
	static const char *funcName = "ReadPars_Filter_BandPass";
	BOOLN	ok;
	char	*filePath;
	int		cascade;
	double	lowerCutOffFrequency, upperCutOffFrequency, preAttenuation;
    FILE    *fp;
    
	filePath = GetParsFileFPath_Common(fileName);
    if ((fp = fopen(filePath, "r")) == NULL) {
        NotifyError("%s: Cannot open data file '%s'.\n", funcName, filePath);
		return(FALSE);
    }
    DPrint("%s: Reading from '%s':\n", funcName, filePath);
    Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, "%d", &cascade))
		ok = FALSE;;
	if (!GetPars_ParFile(fp, "%lf", &preAttenuation))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &lowerCutOffFrequency))
		ok = FALSE;;
	if (!GetPars_ParFile(fp, "%lf", &upperCutOffFrequency))
		ok = FALSE;;
    fclose(fp);
    Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "\
		  "parameter file '%s'.", funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_Filter_BandPass(cascade, lowerCutOffFrequency,
	  upperCutOffFrequency, preAttenuation)) {
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
SetParsPointer_Filter_BandPass(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_Filter_BandPass";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	bandPassFPtr = (BandPassFPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Filter_BandPass(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_Filter_BandPass";

	if (!SetParsPointer_Filter_BandPass(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_Filter_BandPass(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = bandPassFPtr;
	theModule->CheckPars = CheckPars_Filter_BandPass;
	theModule->Free = Free_Filter_BandPass;
	theModule->GetUniParListPtr = GetUniParListPtr_Filter_BandPass;
	theModule->PrintPars = PrintPars_Filter_BandPass;
	theModule->ReadPars = ReadPars_Filter_BandPass;
	theModule->RunProcess = RunModel_Filter_BandPass;
	theModule->SetParsPointer = SetParsPointer_Filter_BandPass;
	return(TRUE);

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 */

BOOLN
InitProcessVariables_Filter_BandPass(EarObjectPtr data)
{
	static const char *funcName = "InitProcessVariables_Filter_BandPass";
	BOOLN	ok = TRUE;
	int		i, j;
	double	*statePtr;
	
	if (bandPassFPtr->updateProcessVariablesFlag || data->updateProcessFlag) {
		FreeProcessVariables_Filter_BandPass();
		if ((bandPassFPtr->coefficients = (BandPassCoeffsPtr *) calloc(data->
		  outSignal->numChannels, sizeof(BandPassCoeffsPtr))) == NULL) {
		 	NotifyError("%s: Out of memory.", funcName);
		 	return(FALSE);
		}
		bandPassFPtr->numChannels = data->outSignal->numChannels;
	 	for (i = 0; (i < data->outSignal->numChannels) && ok; i++)
			if ((bandPassFPtr->coefficients[i] = InitBandPassCoeffs_Filters(
			  bandPassFPtr->cascade, bandPassFPtr->lowerCutOffFreq,
			  bandPassFPtr->upperCutOffFreq, data->inSignal[0]->dt)) == NULL) {
				NotifyError("%s: Failed initialised filter channel %d.",
				  funcName, i);
				ok = FALSE;
			}
		if (!ok) {
			FreeProcessVariables_Filter_BandPass();
			return(FALSE);
		}
		bandPassFPtr->updateProcessVariablesFlag = FALSE;
	} else if (data->timeIndex == PROCESS_START_TIME) {
		for (i = 0; i < data->outSignal->numChannels; i++) {
			statePtr = bandPassFPtr->coefficients[i]->state;
			for (j = 0; j < bandPassFPtr->cascade *
			  FILTERS_NUM_CONTBUTT2_STATE_VARS; j++)
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
FreeProcessVariables_Filter_BandPass(void)
{
	int		i;

	if (bandPassFPtr->coefficients == NULL)
		return;
	for (i = 0; i < bandPassFPtr->numChannels; i++)
		FreeBandPassCoeffs_Filters(&bandPassFPtr->coefficients[i]);
	free(bandPassFPtr->coefficients);
	bandPassFPtr->coefficients = NULL;

}

/**************************** RunModel ****************************************/

/*
 * This routine filters the input signal and puts the result into the output
 * signal.
 * It checks that all initialisation has been correctly carried out by calling
 * the appropriate checking routines.
 * The appropriate coefficients are calculated at the beginning, then the 
 * memory used is released before the program returns.
 */

BOOLN
RunModel_Filter_BandPass(EarObjectPtr data)
{
	static const char *funcName = "RunModel_Filter_BandPass";

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}	
	if (!CheckPars_Filter_BandPass())
		return(FALSE);
	SetProcessName_EarObject(data, "Band pass filter module process");
	if (!CheckRamp_SignalData(data->inSignal[0])) {
		NotifyError("%s: Input signal not correctly initialised.", funcName);
		return(FALSE);
	}
	if (!InitOutFromInSignal_EarObject(data, 0)) {
		NotifyError("%s: Could not initialise the process output signal.",
		  funcName);
		return(FALSE);
	}
	if (!InitProcessVariables_Filter_BandPass(data)) {
		NotifyError("%s: Could not initialise the process variables.",
		  funcName);
		return(FALSE);
	}

	/* Filter signal */
	
	if (fabs(bandPassFPtr->preAttenuation) > DBL_EPSILON)
		GaindB_SignalData(data->outSignal, bandPassFPtr->preAttenuation);
	BandPass_Filters(data->outSignal, bandPassFPtr->coefficients);

	SetProcessContinuity_EarObject(data);
	return(TRUE);

}
