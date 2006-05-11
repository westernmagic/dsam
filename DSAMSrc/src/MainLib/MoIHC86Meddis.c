/**********************
 *
 * File:		MoIHC86Meddis.c
 * Purpose:		This module contains the model Meddis 86 hair cell module.
 * Comments:	The parameters A and B have been changed so that the hair cell
 *				responds correctly for  Intensity levels in dB (SPL).
 * Author:		L. P. O'Mard
 * Created:		18 Feb 1993
 * Updated:		07 May 1998
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
#include "UtString.h"
#include "MoIHC86Meddis.h"

/******************************************************************************/
/*************************** Global Variables *********************************/
/******************************************************************************/

HairCellPtr	hairCellPtr = NULL;

/******************************************************************************/
/*************************** Subroutines and Functions ************************/
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
Init_IHC_Meddis86(ParameterSpecifier parSpec)
{
	static const WChar *funcName = wxT("Init_IHC_Meddis86");

	if (parSpec == GLOBAL) {
		if (hairCellPtr != NULL)
			Free_IHC_Meddis86();
		if ((hairCellPtr = (HairCellPtr) malloc(sizeof(HairCell))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (hairCellPtr == NULL) { 
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	hairCellPtr->parSpec = parSpec;
	hairCellPtr->updateProcessVariablesFlag = TRUE;
	hairCellPtr->permConstAFlag = TRUE;
	hairCellPtr->permConstBFlag = TRUE;
	hairCellPtr->releaseRateFlag = TRUE;
	hairCellPtr->replenishRateFlag = TRUE;
	hairCellPtr->lossRateFlag = TRUE;
	hairCellPtr->reprocessRateFlag = TRUE;
	hairCellPtr->recoveryRateFlag = TRUE;
	hairCellPtr->maxFreePoolFlag = TRUE;
	hairCellPtr->firingRateFlag = TRUE;
					
	hairCellPtr->permConst_A = 100.0;
	hairCellPtr->permConst_B = 6000.0;
	hairCellPtr->releaseRate_g = 2000.0;
	hairCellPtr->replenishRate_y = 5.05;
	hairCellPtr->lossRate_l = 2500.0;
	hairCellPtr->reprocessRate_x = 66.31;
	hairCellPtr->recoveryRate_r = 6580.0;
	hairCellPtr->maxFreePool_M = 1.0;
	hairCellPtr->firingRate_h = 50000.0;

	if (!SetUniParList_IHC_Meddis86()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_IHC_Meddis86();
		return(FALSE);
	}
	hairCellPtr->hCChannels = NULL;
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
Free_IHC_Meddis86(void)
{
	if (hairCellPtr == NULL)
		return(TRUE);
	FreeProcessVariables_IHC_Meddis86();
	if (hairCellPtr->parList)
		FreeList_UniParMgr(&hairCellPtr->parList);
	if (hairCellPtr->parSpec == GLOBAL) {
		free(hairCellPtr);
		hairCellPtr = NULL;
	}
	return(TRUE);

}

/********************************* SetUniParList ******************************/

/*
 * This routine initialises and sets the module's universal parameter list.
 * This list provides universal access to the module's parameters.
 */
 
BOOLN
SetUniParList_IHC_Meddis86(void)
{
	static const WChar *funcName = wxT("SetUniParList_IHC_Meddis86");
	UniParPtr	pars;

	if ((hairCellPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  MEDDIS86_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = hairCellPtr->parList->pars;
	SetPar_UniParMgr(&pars[MEDDIS86_PERM_CONST_A], wxT("PERM_CONST_A"),
	  wxT("Permeability constant A (units/s)."),
	  UNIPAR_REAL,
	  &hairCellPtr->permConst_A, NULL,
	  (void * (*)) SetPermConstA_IHC_Meddis86);
	SetPar_UniParMgr(&pars[MEDDIS86_PERM_CONST_B], wxT("PERM_CONST_B"),
	  wxT("Permeability constant B (units/s)."),
	  UNIPAR_REAL,
	  &hairCellPtr->permConst_B, NULL,
	  (void * (*)) SetPermConstB_IHC_Meddis86);
	SetPar_UniParMgr(&pars[MEDDIS86_RELEASE_RATE_G], wxT("RELEASE_G"),
	  wxT("Release rate (units/s)."),
	  UNIPAR_REAL,
	  &hairCellPtr->releaseRate_g, NULL,
	  (void * (*)) SetReleaseRate_IHC_Meddis86);
	SetPar_UniParMgr(&pars[MEDDIS86_REPLENISH_RATE_Y], wxT("REPLENISH_Y"),
	  wxT("Replenishment rate (units/s)."),
	  UNIPAR_REAL,
	  &hairCellPtr->replenishRate_y, NULL,
	  (void * (*)) SetReplenishRate_IHC_Meddis86);
	SetPar_UniParMgr(&pars[MEDDIS86_LOSS_RATE_L], wxT("LOSS_L"),
	  wxT("Loss rate (units/s)."),
	  UNIPAR_REAL,
	  &hairCellPtr->lossRate_l, NULL,
	  (void * (*)) SetLossRate_IHC_Meddis86);
	SetPar_UniParMgr(&pars[MEDDIS86_REPROCESS_RATE_X], wxT("REPROCESS_X"),
	  wxT("Reprocessing rate (units/s)."),
	  UNIPAR_REAL,
	  &hairCellPtr->reprocessRate_x, NULL,
	  (void * (*)) SetReprocessRate_IHC_Meddis86);
	SetPar_UniParMgr(&pars[MEDDIS86_RECOVERY_RATE_R], wxT("RECOVERY_R"),
	  wxT("Recovery rate (units/s)."),
	  UNIPAR_REAL,
	  &hairCellPtr->recoveryRate_r, NULL,
	  (void * (*)) SetRecoveryRate_IHC_Meddis86);
	SetPar_UniParMgr(&pars[MEDDIS86_MAX_FREE_POOL_M], wxT("MAX_FREE_POOL_M"),
	  wxT("Max. no. of transmitter packets in free pool."),
	  UNIPAR_REAL,
	  &hairCellPtr->maxFreePool_M, NULL,
	  (void * (*)) SetMaxFreePool_IHC_Meddis86);
	SetPar_UniParMgr(&pars[MEDDIS86_FIRING_RATE_H], wxT("FIRING_RATE_H"),
	  wxT("Firing rate (spikes/s)"),
	  UNIPAR_REAL,
	  &hairCellPtr->firingRate_h, NULL,
	  (void * (*)) SetFiringRate_IHC_Meddis86);

	return(TRUE);

}

/********************************* GetUniParListPtr ***************************/

/*
 * This routine returns a pointer to the module's universal parameter list.
 */

UniParListPtr
GetUniParListPtr_IHC_Meddis86(void)
{
	static const WChar *funcName = wxT("GetUniParListPtr_IHC_Meddis86");

	if (hairCellPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (hairCellPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been "
		  "initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(hairCellPtr->parList);

}

/********************************* SetPermConstA ******************************/

/*
 * This function sets the module's permeability constant A parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPermConstA_IHC_Meddis86(double thePermConstA)
{
	static const WChar	*funcName = wxT("SetPermConstA_IHC_Meddis86");

	if (hairCellPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	hairCellPtr->permConstAFlag = TRUE;
	hairCellPtr->permConst_A = thePermConstA;
	hairCellPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetPermConstB ******************************/

/*
 * This function sets the module's permeability constant B parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPermConstB_IHC_Meddis86(double thePermConstB)
{
	static const WChar	*funcName = wxT("SetPermConstB_IHC_Meddis86");

	if (hairCellPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	hairCellPtr->permConstBFlag = TRUE;
	hairCellPtr->permConst_B = thePermConstB;
	hairCellPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetReleaseRate *****************************/

/*
 * This function sets the module's release rate constant, r, parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetReleaseRate_IHC_Meddis86(double theReleaseRate)
{
	static const WChar	*funcName = wxT("SetReleaseRate_IHC_Meddis86");

	if (hairCellPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	hairCellPtr->releaseRateFlag = TRUE;
	hairCellPtr->releaseRate_g = theReleaseRate;
	hairCellPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetReplenishRate ***************************/

/*
 * This function sets the module's replenish rate, y, parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetReplenishRate_IHC_Meddis86(double theReplenishRate)
{
	static const WChar	*funcName = wxT("SetReplenishRate_IHC_Meddis86");

	if (hairCellPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	hairCellPtr->replenishRateFlag = TRUE;
	hairCellPtr->replenishRate_y = theReplenishRate;
	hairCellPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetLossRate ********************************/

/*
 * This function sets the module's loss rate, l, parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetLossRate_IHC_Meddis86(double theLossRate)
{
	static const WChar	*funcName = wxT("SetLossRate_IHC_Meddis86");

	if (hairCellPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	hairCellPtr->lossRateFlag = TRUE;
	hairCellPtr->lossRate_l = theLossRate;
	hairCellPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetReprocessRate ***************************/

/*
 * This function sets the module's permeability constant B parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetReprocessRate_IHC_Meddis86(double theReprocessRate)
{
	static const WChar	*funcName = wxT("SetReprocessRate_IHC_Meddis86");

	if (hairCellPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	hairCellPtr->reprocessRateFlag = TRUE;
	hairCellPtr->reprocessRate_x = theReprocessRate;
	hairCellPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetRecoveryRate ****************************/

/*
 * This function sets the module's permeability constant B parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetRecoveryRate_IHC_Meddis86(double theRecoveryRate)
{
	static const WChar	*funcName = wxT("SetRecoveryRate_IHC_Meddis86");

	if (hairCellPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	hairCellPtr->recoveryRateFlag = TRUE;
	hairCellPtr->recoveryRate_r = theRecoveryRate;
	hairCellPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetMaxFreePool *****************************/

/*
 * This function sets the module's permeability constant B parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetMaxFreePool_IHC_Meddis86(double theMaxFreePool)
{
	static const WChar	*funcName = wxT("SetMaxFreePool_IHC_Meddis86");

	if (hairCellPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	hairCellPtr->maxFreePoolFlag = TRUE;
	hairCellPtr->maxFreePool_M = theMaxFreePool;
	hairCellPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetFiringRate ******************************/

/*
 * This function sets the module's permeability constant h parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetFiringRate_IHC_Meddis86(double theFiringRate)
{
	static const WChar	*funcName = wxT("SetFiringRate_IHC_Meddis86");

	if (hairCellPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	hairCellPtr->firingRateFlag = TRUE;
	hairCellPtr->firingRate_h = theFiringRate;
	hairCellPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetPars ************************************/

/*
 * This function sets all the module's parameters.
 */

BOOLN
SetPars_IHC_Meddis86(double A, double B, double g, double y, double l, double x,
  double r, double M, double h)
{
	BOOLN	ok;
	
	ok = TRUE;
	if (!SetPermConstA_IHC_Meddis86(A))
		ok = FALSE;
	if (!SetPermConstB_IHC_Meddis86(B))
		ok = FALSE;
	if (!SetReleaseRate_IHC_Meddis86(g))
		ok = FALSE;
	if (!SetReplenishRate_IHC_Meddis86(y))
	    ok = FALSE;
	if (!SetLossRate_IHC_Meddis86(l))
		ok = FALSE;
	if (!SetReprocessRate_IHC_Meddis86(x))
		ok = FALSE;
	if (!SetRecoveryRate_IHC_Meddis86(r))
		ok = FALSE;
	if (!SetMaxFreePool_IHC_Meddis86(M))
		ok = FALSE;
	if (!SetFiringRate_IHC_Meddis86(h))
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
CheckPars_IHC_Meddis86(void)
{
	static const WChar *funcName = wxT("CheckPars_IHC_Meddis86");
	BOOLN	ok;
	
	ok = TRUE;
	if (hairCellPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (!hairCellPtr->permConstAFlag) {
		NotifyError(wxT("%s: Permeability constant, A, not correctly set."),
		  funcName);
		ok = FALSE;
	}
	if (!hairCellPtr->permConstBFlag) {
		NotifyError(wxT("%s: Permeability constant, B, not correctly set."),
		  funcName);
		ok = FALSE;
	}
	if (!hairCellPtr->releaseRateFlag) {
		NotifyError(wxT("%s: Release rate constant, g, not correctly set."),
		  funcName);
		ok = FALSE;
	}
	if (!hairCellPtr->replenishRateFlag) {
		NotifyError(wxT("%s: Replenish rate constant, y, not correctly set."),
		  funcName);
		ok = FALSE;
	}
	if (!hairCellPtr->lossRateFlag) {
		NotifyError(wxT("%s: Loss rate constant, l, not correctly set."),
		  funcName);
		ok = FALSE;
	}
	if (!hairCellPtr->reprocessRateFlag) {
		NotifyError(wxT("%s: Reprocessing rate constant, x, not correctly "
		  "set."), funcName);
		ok = FALSE;
	}
	if (!hairCellPtr->recoveryRateFlag) {
		NotifyError(wxT("%s: Recovery rate constant, r, not correctly set."),
		  funcName);
		ok = FALSE;
	}
	if (!hairCellPtr->maxFreePoolFlag) {
		NotifyError(wxT("%s: Max. free transmitter in pool constant, M, not "
		  "correctly set."), funcName);
		ok = FALSE;
	}
	if (!hairCellPtr->firingRateFlag) {
		NotifyError(wxT("%s: Firing rate constant, h, not correctly set."),
		  funcName);
		ok = FALSE;
	}
	return(ok);
		
}

/****************************** PrintPars *************************************/

/*
 * This program prints the parameters of the module to the standard output.
 */
 
BOOLN
PrintPars_IHC_Meddis86(void)
{
	static const WChar *funcName = wxT("PrintPars_IHC_Meddis86");

	if (!CheckPars_IHC_Meddis86()) {
		NotifyError(wxT("%s: Parameters have not been correctly set."),
		  funcName);
		return(FALSE);
	}
	DPrint(wxT("Meddis 86 IHC Synapse Module Parameters:-\n"));
	DPrint(wxT("\tPermeability constant, A = %g,\tand B = %g\n"),
	  hairCellPtr->permConst_A, hairCellPtr->permConst_B);
	DPrint(wxT("\tRelease rate, g = %g /s,\tReplenishment rate, y = %g /s\n"),
	  hairCellPtr->releaseRate_g, hairCellPtr->replenishRate_y);
	DPrint(wxT("\tLoss rate, l = %g /s,\tReprocessing rate, x = %g /s\n"),
	  hairCellPtr->lossRate_l, hairCellPtr->reprocessRate_x);
	DPrint(wxT("\tRecovery rate, r = %g /s,\t Firing rate, h = %g spikes/s\n"),
	  hairCellPtr->recoveryRate_r, hairCellPtr->firingRate_h);
	DPrint(wxT("\tMaximum transmitter packets in free pool, M = %g.\n"),
	  hairCellPtr->maxFreePool_M);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.
 */
 
BOOLN
ReadPars_IHC_Meddis86(WChar *fileName)
{
	static const WChar *funcName = wxT("ReadPars_IHC_Meddis86");
	BOOLN	ok;
	WChar	*filePath;
	double	permConst_A, permConst_B, releaseRate_g, replenishRate_y;
	double	lossRate_l, reprocessRate_x, recoveryRate_r, maxFreePool_M;
	double	firingRate_h;
    FILE    *fp;
    
	filePath = GetParsFileFPath_Common(fileName);
    if ((fp = fopen(ConvUTF8_Utility_String(filePath), "r")) == NULL) {
        NotifyError(wxT("%s: Cannot open data file '%s'.\n"), funcName,
		  filePath);
		return(FALSE);
    }
    DPrint(wxT("%s: Reading from '%s':\n"), funcName, filePath);
    Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &permConst_A))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &permConst_B))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &releaseRate_g))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &replenishRate_y))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &lossRate_l))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &reprocessRate_x))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &recoveryRate_r))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &maxFreePool_M))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &firingRate_h))
		ok = FALSE;
    fclose(fp);
    Free_ParFile();
	if (!ok) {
		NotifyError(wxT("%s: Not enough lines, or invalid parameters, in "
		  "module parameter file '%s'."), funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_IHC_Meddis86(permConst_A, permConst_B, releaseRate_g,
	  replenishRate_y, lossRate_l, reprocessRate_x, recoveryRate_r,
	  maxFreePool_M, firingRate_h)) {
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
SetParsPointer_IHC_Meddis86(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_IHC_Meddis86");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	hairCellPtr = (HairCellPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_IHC_Meddis86(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_IHC_Meddis86");

	if (!SetParsPointer_IHC_Meddis86(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_IHC_Meddis86(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = hairCellPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->CheckPars = CheckPars_IHC_Meddis86;
	theModule->Free = Free_IHC_Meddis86;
	theModule->GetUniParListPtr = GetUniParListPtr_IHC_Meddis86;
	theModule->PrintPars = PrintPars_IHC_Meddis86;
	theModule->ReadPars = ReadPars_IHC_Meddis86;
	theModule->RunProcess = RunModel_IHC_Meddis86;
	theModule->SetParsPointer = SetParsPointer_IHC_Meddis86;
	return(TRUE);

}

/********************************* CheckData **********************************/

/*
 * This routine checks that the input signal is correctly initialised
 * (using CheckRamp_SignalData), and determines whether the parameter values
 * are valid for the signal sampling interval.
 */

BOOLN
CheckData_IHC_Meddis86(EarObjectPtr data)
{
	static const WChar *funcName = wxT("CheckData_IHC_Meddis86");
	double	dt;
	
	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}	
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	if (!CheckRamp_SignalData(data->inSignal[0]))
		return(FALSE);
	dt = data->inSignal[0]->dt;
	if (dt > MEDDIS86_MAX_DT) {
		NotifyError(wxT("%s: Maximum sampling interval exceeded."), funcName);
		return(FALSE);
	}
	return(TRUE);
	
}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 * It initialises the hair cells to the spontaneous firing rate.
 * The Spontaneous firing rate is determined from the equilbrium state of the
 * system with no input: the reservoir contents do not change in time.
 */

BOOLN
InitProcessVariables_IHC_Meddis86(EarObjectPtr data)
{
	static const WChar *funcName = wxT("InitProcessVariables_IHC_Meddis86");
	int		i;
	double	spontPerm_k0, spontCleft_c0, spontFreePool_q0, spontReprocess_w0;
	HairCellPtr	hC;
	
	if (hairCellPtr->updateProcessVariablesFlag || data->updateProcessFlag ||
	  (data->timeIndex == PROCESS_START_TIME)) {
		hC = hairCellPtr;		/* Shorter variable for long formulae. */
		if (hairCellPtr->updateProcessVariablesFlag ||
		  data->updateProcessFlag) {
			FreeProcessVariables_IHC_Meddis86();
			if ((hC->hCChannels = (HairCellVarsPtr) calloc(
			  data->outSignal->numChannels, sizeof (HairCellVars))) == NULL) {
				NotifyError(wxT("%s: Out of memory."), funcName);
				return(FALSE);
			}
			hairCellPtr->updateProcessVariablesFlag = FALSE;
		}
		spontPerm_k0 = hC->releaseRate_g * hC->permConst_A / (hC->permConst_A +
		  hC->permConst_B);
		spontCleft_c0 = (hC->maxFreePool_M * hC->replenishRate_y *
		  spontPerm_k0 / (hC->replenishRate_y * (hC->lossRate_l +
		  hC->recoveryRate_r) + spontPerm_k0 * hC->lossRate_l));
		spontFreePool_q0 = spontCleft_c0 * (hC->lossRate_l +
		  hC->recoveryRate_r) / spontPerm_k0;
		spontReprocess_w0 = spontCleft_c0 * hC->recoveryRate_r /
		  hC->reprocessRate_x;

		for (i = 0; i < data->outSignal->numChannels; i++) {
			hC->hCChannels[i].cleftC = spontCleft_c0;
			hC->hCChannels[i].reservoirQ = spontFreePool_q0;
			hC->hCChannels[i].reprocessedW = spontReprocess_w0;
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
FreeProcessVariables_IHC_Meddis86(void)
{
	if (hairCellPtr->hCChannels == NULL)
		return;
	free(hairCellPtr->hCChannels);
	hairCellPtr->hCChannels = NULL;
	hairCellPtr->updateProcessVariablesFlag = TRUE;

}

/********************************* RunModel ***********************************/

/*
 * This routine runs the input signal through the model and puts the result
 * into the output signal.  It checks that all initialisation has been
 * correctly carried out by calling the appropriate checking routines.
 */

BOOLN
RunModel_IHC_Meddis86(EarObjectPtr data)
{
	static const WChar *funcName = wxT("RunModel_IHC_Meddis86");
	register	double	replenish, reprocessed, ejected, reUptake;
	register	double	reUptakeAndLost;
	int		i;
	ChanLen	j;
	double	st_Plus_A, kdt;
	ChanData	*inPtr, *outPtr;
	HairCellPtr	hC = hairCellPtr;
	
	if (!data->threadRunFlag) {
		if (!CheckPars_IHC_Meddis86())		
			return(FALSE);
		if (!CheckData_IHC_Meddis86(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Meddis 86 IHC Synapse"));
		if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels,
		  data->inSignal[0]->length, data->inSignal[0]->dt)) {
			NotifyError(wxT("%s: Could not initialise output signal."),
			  funcName);
			return(FALSE);
		}

		if (!InitProcessVariables_IHC_Meddis86(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		hC->dt = data->outSignal->dt;
		hC->ymdt = hC->replenishRate_y * hC->maxFreePool_M * hC->dt;
		hC->xdt = hC->reprocessRate_x * hC->dt;
		hC->ydt = hC->replenishRate_y * hC->dt;
		hC->l_Plus_rdt = (hC->lossRate_l + hC->recoveryRate_r) * hC->dt;
		hC->rdt = hC->recoveryRate_r * hC->dt;
		hC->gdt = hC->releaseRate_g * hC->dt;
		hC->hdt = hC->firingRate_h * hC->dt;
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	for (i = data->outSignal->offset; i < data->outSignal->numChannels; i++) {
		inPtr = data->inSignal[0]->channel[i];
		outPtr = data->outSignal->channel[i];
		for (j = 0; j < data->outSignal->length; j++) {
			if ((st_Plus_A = *inPtr++ + hC->permConst_A) > 0.0)
				kdt = hC->gdt * st_Plus_A / (st_Plus_A + hC->permConst_B);
			else
				kdt = 0.0;
			replenish = (hC->hCChannels[i].reservoirQ < hC->maxFreePool_M)?
			  hC->ymdt - hC->ydt * hC->hCChannels[i].reservoirQ: 0.0;
			reprocessed = hC->xdt * hC->hCChannels[i].reprocessedW;
			ejected = kdt * hC->hCChannels[i].reservoirQ;
			reUptake = hC->rdt * hC->hCChannels[i].cleftC;
			reUptakeAndLost = hC->l_Plus_rdt * hC->hCChannels[i].cleftC;

			hC->hCChannels[i].reservoirQ += replenish - ejected + reprocessed;
			hC->hCChannels[i].cleftC += ejected - reUptakeAndLost;
			hC->hCChannels[i].reprocessedW += reUptake - reprocessed;
			
			/* Spike prob. */
			*outPtr++ = (ChanData) (hC->hdt * hC->hCChannels[i].cleftC);
		}
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);
		
} /* RunModel_IHC_Meddis86 */
