/**********************
 *
 * File:		MoBMGammaT.c
 * Purpose:		This is an implementation of a Gamma Tone filter using the
 *				UtFilters.c module.
 * Comments:	The Gamma Tone filter is used to model the response of the 
 *				Basilar Membrane.
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		26 Jan 1997
 * Copyright:	(c) 1997,  University of Essex
 *
 **********************/
 
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "FiParFile.h"
#include "UtBandwidth.h"
#include "UtCFList.h"
#include "UtFilters.h"
#include "MoBMGammaT.h"

/******************************************************************************/
/*************************** Global Variables *********************************/
/******************************************************************************/

BMGammaTPtr	bMGammaTPtr = NULL;

/******************************************************************************/
/*************************** Subroutines and functions ************************/
/******************************************************************************/

/********************************* Free ***************************************/

/*
 * This function releases of the memory allocated for the process variables.
 * It should be called when the module is no longer in use.
 * It is defined as returning a BOOLN value because the generic module
 * interface requires that a non-void value be returned.
 */

BOOLN
Free_BasilarM_GammaT(void)
{
	if (bMGammaTPtr == NULL)
		return(FALSE);
	FreeProcessVariables_BasilarM_GammaT();
	Free_CFList(&bMGammaTPtr->theCFs);
	if (bMGammaTPtr->parList)
		FreeList_UniParMgr(&bMGammaTPtr->parList);
	if (bMGammaTPtr->parSpec == GLOBAL) {
		free(bMGammaTPtr);
		bMGammaTPtr = NULL;
	}
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
Init_BasilarM_GammaT(ParameterSpecifier parSpec)
{
	static const char *funcName = "Init_BasilarM_GammaT";

	if (parSpec == GLOBAL) {
		if (bMGammaTPtr != NULL)
			Free_BasilarM_GammaT();
		if ((bMGammaTPtr = (BMGammaTPtr) malloc(sizeof(BMGammaT))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (bMGammaTPtr == NULL) { 
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	bMGammaTPtr->parSpec = parSpec;
	bMGammaTPtr->updateProcessVariablesFlag = TRUE;
	bMGammaTPtr->cascadeFlag = FALSE;
	bMGammaTPtr->cascade = 0;
	bMGammaTPtr->theCFs = NULL;

	if (!SetUniParList_BasilarM_GammaT()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_BasilarM_GammaT();
		return(FALSE);
	}
	bMGammaTPtr->numChannels = 0;
	bMGammaTPtr->coefficients = NULL;
	return(TRUE);

}

/********************************* SetUniParList ******************************/

/*
 * This routine initialises and sets the module's universal parameter list.
 * This list provides universal access to the module's parameters.
 */
 
BOOLN
SetUniParList_BasilarM_GammaT(void)
{
	static const char *funcName = "SetUniParList_BasilarM_GammaT";
	UniParPtr	pars;

	if ((bMGammaTPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  BM_GAMMT_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = bMGammaTPtr->parList->pars;
	SetPar_UniParMgr(&pars[BM_GAMMT_CASCADE], "CASCADE",
	  "Filter cascade.",
	  UNIPAR_INT,
	  &bMGammaTPtr->cascade, NULL,
	  (void * (*)) SetCascade_BasilarM_GammaT);
	SetPar_UniParMgr(&pars[BM_GAMMAT_THE_CFS], "CFLIST",
	  "Centre frequency specification",
	  UNIPAR_CFLIST,
	  &bMGammaTPtr->theCFs, NULL,
	  (void * (*)) SetCFList_BasilarM_GammaT);
	return(TRUE);

}

/********************************* GetUniParListPtr ***************************/

/*
 * This routine returns a pointer to the module's universal parameter list.
 */

UniParListPtr
GetUniParListPtr_BasilarM_GammaT(void)
{
	static const char *funcName = "GetUniParListPtr_BasilarM_GammaT";

	if (bMGammaTPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (bMGammaTPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(bMGammaTPtr->parList);

}

/********************************* CheckPars **********************************/

/*
 * This routine checks that the necessary parameters for the module have been
 * correctly initialised.
 * It returns TRUE if there are no problems.
 */
 
BOOLN
CheckPars_BasilarM_GammaT(void)
{
	static const char *funcName = "CheckPars_BasilarM_GammaT";
	BOOLN ok;
	
	ok = TRUE;
	if (bMGammaTPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!bMGammaTPtr->cascadeFlag) {
		NotifyError("%s: Filter cascade not set.", funcName);
		ok = FALSE;
	}
	if (!CheckPars_CFList(bMGammaTPtr->theCFs)) {
		NotifyError("%s: Centre frequency list parameters not correctly set.",
		  funcName);
		ok = FALSE;
	}
	return(ok);

}

/********************************* SetCascade *********************************/

/*
 * This function sets the module's cascade parameter.
 * It first checks that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetCascade_BasilarM_GammaT(int theCascade)
{
	static const char	 *funcName = "SetCascade_BasilarM_GammaT";

	if (bMGammaTPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	bMGammaTPtr->cascadeFlag = TRUE;
	bMGammaTPtr->cascade = theCascade;
	bMGammaTPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);
	
}

/********************************* SetCFList **********************************/

/*
 * This function sets the CFList data structure for the filter bank.
 * It returns TRUE if the operation is successful.
 */
 
BOOLN
SetCFList_BasilarM_GammaT(CFListPtr theCFList)
{
	static const char *funcName = "SetCFList_BasilarM_GammaT";

	if (bMGammaTPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckPars_CFList(theCFList)) {
		NotifyError("%s: Centre frequency structure not correctly set.",
		  funcName);
		return(FALSE);
	}
	if (bMGammaTPtr->theCFs != NULL)
		Free_CFList(&bMGammaTPtr->theCFs);
	bMGammaTPtr->theCFs = theCFList;
	bMGammaTPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetBandwidths ******************************/

/*
 * This function sets the band width mode for the gamma tone filters.
 * The band width mode defines the function for calculating the gamma tone
 * filter band width 3 dB down.
 * No checks are made on the correct length for the bandwidth array.
 */

BOOLN
SetBandwidths_BasilarM_GammaT(char *theBandwidthMode, double *theBandwidths)
{
	static const char *funcName = "SetBandwidths_BasilarM_GammaT";

	if (bMGammaTPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!SetBandwidths_CFList(bMGammaTPtr->theCFs, theBandwidthMode,
	  theBandwidths)) {
		NotifyError("%s: Failed to set bandwidth mode.", funcName);
		return(FALSE);
	}
	bMGammaTPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);
	
}

/********************************* SetPars ************************************/

/*
 * This function sets all the module's parameters.
 * The bandwidths for the CFList data structure must also be set, but it is
 * assumed that they are set independently.
 * It returns TRUE if the operation is successful.
 */
  
BOOLN
SetPars_BasilarM_GammaT(int theCascade, CFListPtr theCFs)
{
	static const char *funcName = "SetPars_BasilarM_GammaT";
	BOOLN	ok;
	
	ok = TRUE;
	if (!SetCascade_BasilarM_GammaT(theCascade))
		ok = FALSE;
	if (!SetCFList_BasilarM_GammaT(theCFs))
		ok = FALSE;
	if (!ok)
		NotifyError("%s: Failed to set all module parameters.", funcName);
	return(ok);
	
}

/****************************** GetCFListPtr **********************************/

/*
 * This routine returns a pointer to the module's CFList data pointer.
 *
 */

CFListPtr
GetCFListPtr_BasilarM_GammaT(void)
{
	static const char *funcName = "GetCFListPtr_BasilarM_GammaT";

	if (bMGammaTPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (bMGammaTPtr->theCFs == NULL) {
		NotifyError("%s: CFList data structure has not been correctly set.  "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(bMGammaTPtr->theCFs);

}

/****************************** PrintPars *************************************/

/*
 * This program prints the parameters of the module to the standard output.
 */
 
BOOLN
PrintPars_BasilarM_GammaT(void)
{
	static const char *funcName = "PrintPars_BasilarM_GammaT";
	
	if (!CheckPars_BasilarM_GammaT()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Gamma Tone Basilar Membrane Filter "\
	  "Module Parameters:-\n");
	PrintPars_CFList(bMGammaTPtr->theCFs);
	DPrint("\tFilter cascade = %d.\n", bMGammaTPtr->cascade);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.
 */
 
BOOLN
ReadPars_BasilarM_GammaT(char *fileName)
{
	static const char *funcName = "ReadPars_BasilarM_GammaT";
	BOOLN	ok;
	char	*filePath;
	int		filterCascade;
    FILE    *fp;
    CFListPtr	theCFs;
    
	filePath = GetParsFileFPath_Common(fileName);
    if ((fp = fopen(filePath, "r")) == NULL) {
        NotifyError("%s: Cannot open data file '%s'.\n", funcName, filePath);
		return(FALSE);
    }
    DPrint("%s: Reading from '%s':\n", funcName, filePath);
    Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, "%d", &filterCascade))
		ok = FALSE;
	if ((theCFs = ReadPars_CFList(fp)) == NULL)
		ok = FALSE;
	if (!ReadBandwidths_CFList(fp, theCFs))
		ok = FALSE;
    fclose(fp);
    Free_ParFile();
	if (!SetPars_BasilarM_GammaT(filterCascade, theCFs)) {
		NotifyError("%s: Could not set parameters.", funcName);
		return(FALSE);
	}
	return(ok);
   
}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_BasilarM_GammaT(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_BasilarM_GammaT";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	bMGammaTPtr = (BMGammaTPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_BasilarM_GammaT(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_BasilarM_GammaT";

	if (!SetParsPointer_BasilarM_GammaT(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_BasilarM_GammaT(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = bMGammaTPtr;
	theModule->CheckPars = CheckPars_BasilarM_GammaT;
	theModule->Free = Free_BasilarM_GammaT;
	theModule->GetUniParListPtr = GetUniParListPtr_BasilarM_GammaT;
	theModule->PrintPars = PrintPars_BasilarM_GammaT;
	theModule->ReadPars = ReadPars_BasilarM_GammaT;
	theModule->RunProcess = RunModel_BasilarM_GammaT;
	theModule->SetParsPointer = SetParsPointer_BasilarM_GammaT;
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

void
FreeProcessVariables_BasilarM_GammaT(void)
{
	int		i;

	if (bMGammaTPtr->coefficients == NULL)
		return;
	for (i = 0; i < bMGammaTPtr->numChannels; i++)
    	FreeGammaToneCoeffs_Filters(&bMGammaTPtr->coefficients[i]);
	free(bMGammaTPtr->coefficients);
	bMGammaTPtr->coefficients = NULL;
	bMGammaTPtr->updateProcessVariablesFlag = TRUE;

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 */

BOOLN
InitProcessVariables_BasilarM_GammaT(EarObjectPtr data)
{
	static const char *funcName = "InitProcessVariables_BasilarM_GammaT";
	int		i, j, cFIndex, stateVectorLength;
	double	sampleRate, *ptr;
	
	if (bMGammaTPtr->updateProcessVariablesFlag || data->updateProcessFlag ||
	  bMGammaTPtr->theCFs->updateFlag) {
		FreeProcessVariables_BasilarM_GammaT();
		if ((bMGammaTPtr->coefficients =
		  (GammaToneCoeffsPtr *) calloc(data->outSignal->numChannels,
		   sizeof(GammaToneCoeffsPtr))) == NULL) {
		 	NotifyError("%s: Out of memory.", funcName);
		 	return(FALSE);
		}
		sampleRate = 1.0 / data->inSignal[0]->dt;
		bMGammaTPtr->numChannels = data->outSignal->numChannels;
		for (i = 0; i < data->outSignal->numChannels; i++) {
			cFIndex = i / data->inSignal[0]->interleaveLevel;
			if ((bMGammaTPtr->coefficients[i] = InitGammaToneCoeffs_Filters(
			  bMGammaTPtr->theCFs->frequency[cFIndex], bMGammaTPtr->theCFs->
			  bandwidth[cFIndex], bMGammaTPtr->cascade, sampleRate)) == NULL) {
				NotifyError("%s: Could not initialise coefficients for channel "
				  "%d.", funcName, i);
				return(FALSE);
			}
		}
		SetLocalInfoFlag_SignalData(data->outSignal, TRUE);
		SetInfoChannelTitle_SignalData(data->outSignal, "Frequency (Hz)");
		SetInfoChannelLabels_SignalData(data->outSignal,
		  bMGammaTPtr->theCFs->frequency);
		SetInfoCFArray_SignalData(data->outSignal,
		  bMGammaTPtr->theCFs->frequency);
		bMGammaTPtr->updateProcessVariablesFlag = FALSE;
		bMGammaTPtr->theCFs->updateFlag = FALSE;
	} else if (data->timeIndex == PROCESS_START_TIME) {
		stateVectorLength = bMGammaTPtr->cascade * 
		  FILTERS_NUM_GAMMAT_STATE_VARS_PER_FILTER;
		for (i = 0; i < data->outSignal->numChannels; i++) {
			ptr = bMGammaTPtr->coefficients[i]->stateVector;
			for (j = 0; j < stateVectorLength; j++)
				*ptr++ = 0.0;
		}
				
	}
	return(TRUE);

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
RunModel_BasilarM_GammaT(EarObjectPtr data)
{
	static const char *funcName = "RunModel_BasilarM_GammaT";
	int		totalChannels;
				
	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}	
	if (!CheckPars_BasilarM_GammaT())
		return(FALSE);
		
	/* Initialise Variables and coefficients */
	
	SetProcessName_EarObject(data, "Gamma tone basilar membrane filtering");
	if (!CheckRamp_SignalData(data->inSignal[0])) {
		NotifyError("%s: Input signal not correctly initialised.", funcName);
		return(FALSE);
	}
	totalChannels = bMGammaTPtr->theCFs->numChannels *
	  data->inSignal[0]->numChannels;
	if (!InitOutFromInSignal_EarObject(data, totalChannels)) {
		NotifyError("%s: Cannot initialise output channel.", funcName);
		return(FALSE);
	}
	if (!InitProcessVariables_BasilarM_GammaT(data)) {
		NotifyError("%s: Could not initialise the process variables.",
		  funcName);
		return(FALSE);
	}
	/* Filter signal */
	GammaTone_Filters(data->outSignal, bMGammaTPtr->coefficients);

	SetProcessContinuity_EarObject(data);
	return(TRUE);

}
