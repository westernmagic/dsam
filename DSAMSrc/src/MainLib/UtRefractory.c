/**********************
 *
 * File:		UtRefractory.c
 * Purpose:		Applies a refractory effect adjustment for spike probabilities.
 *				R. Meddis and M. J. Hewitt (1991)"Virtual pitch and phase
 *				sensitivity of a computer modle of the auditory periphery. 1:
 *				pitch identification" Journal of the Acoustical Society of
 *				America, 89, pp 2866-2882.
 * Comments:	Written using ModuleProducer version 1.9 (May 27 1996).
 * Author:		L. P. O'Mard, revised from code by M. J. Hewitt
 * Created:		26 Jun 1996
 * Updated:	
 * Copyright:	(c) 1998, University of Essex
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
#include "UtRefractory.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

RefractAdjPtr	refractAdjPtr = NULL;

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
Free_Utility_RefractoryAdjust(void)
{
	/* static const char	*funcName = "Free_Utility_RefractoryAdjust";  */

	if (refractAdjPtr == NULL)
		return(FALSE);
	FreeProcessVariables_Utility_RefractoryAdjust();
	if (refractAdjPtr->parList)
		FreeList_UniParMgr(&refractAdjPtr->parList);
	if (refractAdjPtr->parSpec == GLOBAL) {
		free(refractAdjPtr);
		refractAdjPtr = NULL;
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
Init_Utility_RefractoryAdjust(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_Utility_RefractoryAdjust";

	if (parSpec == GLOBAL) {
		if (refractAdjPtr != NULL)
			Free_Utility_RefractoryAdjust();
		if ((refractAdjPtr = (RefractAdjPtr) malloc(sizeof(RefractAdj))) ==
		  NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (refractAdjPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	refractAdjPtr->parSpec = parSpec;
	refractAdjPtr->updateProcessVariablesFlag = TRUE;
	refractAdjPtr->refractoryPeriodFlag = TRUE;
	refractAdjPtr->refractoryPeriod = 0.0;

	if (!SetUniParList_Utility_RefractoryAdjust()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_Utility_RefractoryAdjust();
		return(FALSE);
	}
	refractAdjPtr->numChannels = 0;
	refractAdjPtr->lastOutput = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_Utility_RefractoryAdjust(void)
{
	static const char *funcName = "SetUniParList_Utility_RefractoryAdjust";
	UniParPtr	pars;

	if ((refractAdjPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  UTILITY_REFRACTORYADJUST_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = refractAdjPtr->parList->pars;
	SetPar_UniParMgr(&pars[UTILITY_REFRACTORYADJUST_REFRACTORYPERIOD],
	  "PERIOD",
	  "Refractory period (s).",
	  UNIPAR_REAL,
	  &refractAdjPtr->refractoryPeriod, NULL,
	  (void * (*)) SetRefractoryPeriod_Utility_RefractoryAdjust);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Utility_RefractoryAdjust(void)
{
	static const char	*funcName = "GetUniParListPtr_Utility_RefractoryAdjust";

	if (refractAdjPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (refractAdjPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(refractAdjPtr->parList);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_Utility_RefractoryAdjust(double refractoryPeriod)
{
	static const char	*funcName = "SetPars_Utility_RefractoryAdjust";
	BOOLN	ok;

	ok = TRUE;
	if (!SetRefractoryPeriod_Utility_RefractoryAdjust(refractoryPeriod))
		ok = FALSE;
	if (!ok)
		NotifyError("%s: Failed to set all module parameters." ,funcName);
	return(ok);

}

/****************************** SetRefractoryPeriod ***************************/

/*
 * This function sets the module's refractoryPeriod parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRefractoryPeriod_Utility_RefractoryAdjust(double theRefractoryPeriod)
{
	static const char	*funcName =
	  "SetRefractoryPeriod_Utility_RefractoryAdjust";

	if (refractAdjPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (theRefractoryPeriod < 0.0) {
		NotifyError("%s: Refractory period must be greater than zero.",
		  funcName);
		return(FALSE);
	}
	refractAdjPtr->refractoryPeriodFlag = TRUE;
	refractAdjPtr->updateProcessVariablesFlag = TRUE;
	refractAdjPtr->refractoryPeriod = theRefractoryPeriod;
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
CheckPars_Utility_RefractoryAdjust(void)
{
	static const char	*funcName = "CheckPars_Utility_RefractoryAdjust";
	BOOLN	ok;

	ok = TRUE;
	if (refractAdjPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!refractAdjPtr->refractoryPeriodFlag) {
		NotifyError("%s: refractoryPeriod variable not set.", funcName);
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
PrintPars_Utility_RefractoryAdjust(void)
{
	static const char	*funcName = "PrintPars_Utility_RefractoryAdjust";

	if (!CheckPars_Utility_RefractoryAdjust()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("AN Refractory Adjustment Utility Module "
	  "Parameters:-\n");
	DPrint("\tRefractory period = %g ms\n",
	  MSEC(refractAdjPtr->refractoryPeriod));
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_Utility_RefractoryAdjust(char *fileName)
{
	static const char	*funcName = "ReadPars_Utility_RefractoryAdjust";
	BOOLN	ok;
	char	*filePath;
	double	refractoryPeriod;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(filePath, "r")) == NULL) {
		NotifyError("%s: Cannot open data file '%s'.\n", funcName, filePath);
		return(FALSE);
	}
	DPrint("%s: Reading from '%s':\n", funcName, filePath);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, "%lf", &refractoryPeriod))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "
		  "parameter file '%s'.", funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_Utility_RefractoryAdjust(refractoryPeriod)) {
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
SetParsPointer_Utility_RefractoryAdjust(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_Utility_RefractoryAdjust";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	refractAdjPtr = (RefractAdjPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Utility_RefractoryAdjust(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_Utility_RefractoryAdjust";

	if (!SetParsPointer_Utility_RefractoryAdjust(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_Utility_RefractoryAdjust(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = refractAdjPtr;
	theModule->CheckPars = CheckPars_Utility_RefractoryAdjust;
	theModule->Free = Free_Utility_RefractoryAdjust;
	theModule->GetUniParListPtr = GetUniParListPtr_Utility_RefractoryAdjust;
	theModule->PrintPars = PrintPars_Utility_RefractoryAdjust;
	theModule->ReadPars = ReadPars_Utility_RefractoryAdjust;
	theModule->RunProcess = Process_Utility_RefractoryAdjust;
	theModule->SetParsPointer = SetParsPointer_Utility_RefractoryAdjust;
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
CheckData_Utility_RefractoryAdjust(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_Utility_RefractoryAdjust";

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInit_SignalData(data->inSignal[0],
	  "CheckData_Utility_RefractoryAdjust"))
		return(FALSE);
	if (refractAdjPtr->refractoryPeriod >= _GetDuration_SignalData(data->
	  inSignal[0])) {
		NotifyError("%s: Refractory period (%g ms) is too long for signal "
		  "length (%g ms).", funcName, MSEC(refractAdjPtr->refractoryPeriod),
		  MSEC(_GetDuration_SignalData(data->inSignal[0])));
		return(FALSE);
	}
	return(TRUE);

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 * The initial input probabilities assume preceeding silence.
 */

BOOLN
InitProcessVariables_Utility_RefractoryAdjust(EarObjectPtr data)
{
	static const char *funcName =
	  "InitProcessVariables_Utility_RefractoryAdjust";
	int		i, j;
	double	*ptr;
	ChanLen	k, refractoryPeriodIndex;

	if (refractAdjPtr->updateProcessVariablesFlag ||
	  data->updateProcessFlag || (data->timeIndex == PROCESS_START_TIME)) {
		refractoryPeriodIndex = (ChanLen) (refractAdjPtr->refractoryPeriod /
		  data->outSignal->dt + 0.5);
		if (refractAdjPtr->updateProcessVariablesFlag ||
		  data->updateProcessFlag) {
			FreeProcessVariables_Utility_RefractoryAdjust();
		  	refractAdjPtr->numChannels = data->outSignal->numChannels;
			if ((refractAdjPtr->lastOutput = (double **)
			  calloc(refractAdjPtr->numChannels, sizeof(double *))) == NULL) {
			 	NotifyError("%s: Out of memory for 'lastOutput pointers'.",
			 	  funcName);
			 	return(FALSE);
			}
			for (i = 0; i < refractAdjPtr->numChannels; i++)
				if ((refractAdjPtr->lastOutput[i] = (double *)
				  calloc(refractoryPeriodIndex, sizeof(double))) == NULL) {
					NotifyError("%s: Out of memory for 'lastOutput arrays'.",
					  funcName);
					for (j = 0; j < i - 1; j++)
						free(refractAdjPtr->lastOutput[j]);
					free(refractAdjPtr->lastOutput);
					refractAdjPtr->lastOutput = NULL;
					return(FALSE);
				}
			refractAdjPtr->updateProcessVariablesFlag = FALSE;
		}
		for (i = 0; i < refractAdjPtr->numChannels; i++) {
			ptr = refractAdjPtr->lastOutput[i];
			for (k = 0; k < refractoryPeriodIndex; k++)
				*ptr++ = 0.0;
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
FreeProcessVariables_Utility_RefractoryAdjust(void)
{
	int		i;
	
	if (refractAdjPtr->lastOutput != NULL) {
		for (i = 0; i < refractAdjPtr->numChannels; i++)
			free(refractAdjPtr->lastOutput[i]);
		free(refractAdjPtr->lastOutput);
		refractAdjPtr->lastOutput = NULL;
	}
	refractAdjPtr->updateProcessVariablesFlag = TRUE;

}

/****************************** Process ***************************************/

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
Process_Utility_RefractoryAdjust(EarObjectPtr data)
{
	static const char	*funcName = "RunModel_Utility_RefractoryAdjust";
	register	ChanData	 *outPtr, *sumPtr;
	register	double		sum;
	int		chan;
	double	*lastOutputPtr;
	ChanLen	i, j, refractoryPeriodIndex;

	if (!CheckPars_Utility_RefractoryAdjust())
		return(FALSE);
	if (!CheckData_Utility_RefractoryAdjust(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "Meddis 91 AN Refractory Adustment Process");
	if (!InitOutFromInSignal_EarObject(data, 0)) {
		NotifyError("%s: Could not initialise output signal.", funcName);
		return(FALSE);
	}
	if (!InitProcessVariables_Utility_RefractoryAdjust(data)) {
		NotifyError("%s: Could not initialise the process variables.",
		  funcName);
		return(FALSE);
	}
	refractoryPeriodIndex = (ChanLen) (refractAdjPtr->refractoryPeriod / 
	  data->outSignal->dt + 0.5);
	for (chan = 0; chan < data->outSignal->numChannels; chan++) {
		outPtr = data->outSignal->channel[chan];
		for (i = 0; i < data->outSignal->length; i++) {
			sum = 0.0;
			if (i < refractoryPeriodIndex) {
				lastOutputPtr = refractAdjPtr->lastOutput[chan] + i;
				for (j = i; j < refractoryPeriodIndex; j++)
					sum += *lastOutputPtr++;
				sumPtr = data->outSignal->channel[chan];
			} else
				sumPtr = outPtr - refractoryPeriodIndex;
			while (sumPtr < outPtr)
				sum += *sumPtr++;
			*outPtr++ *= (1.0 - sum);
		}		
		lastOutputPtr = refractAdjPtr->lastOutput[chan] + refractoryPeriodIndex;
		for (i = 0; i < refractoryPeriodIndex; i++)
			*--lastOutputPtr = *--outPtr;
	}
	SetUtilityProcessContinuity_EarObject(data);
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

