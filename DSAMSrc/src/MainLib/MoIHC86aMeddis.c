/******************
 *
 * File:		MoIHC86aMeddis.cc
 * Purpose:		This module contains the routines for the Meddis 86a Hair cell
 *				model module.
 * Comments:	It is expects input from a IHC receptor potential model module.
 * Authors:		L. P. O'Mard
 * Created:		12 Jul 1996
 * Updated:		05 Sep 1996
 * Copyright:	(c) 1998, University of Essex
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
#include "MoIHC86aMeddis.h"
#include "UtRandom.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

/* #define DEBUG	1 */

/******************************************************************************/
/*************************** Global Variables *********************************/
/******************************************************************************/

HairCell3Ptr	hairCell3Ptr = NULL;

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
Init_IHC_Meddis86a(ParameterSpecifier parSpec)
{
	static const char *funcName = "Init_IHC_Meddis86a";

	if (parSpec == GLOBAL) {
		if (hairCell3Ptr != NULL)
			Free_IHC_Meddis86a();
		if ((hairCell3Ptr = (HairCell3Ptr) malloc(sizeof(HairCell3))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (hairCell3Ptr == NULL) { 
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	hairCell3Ptr->parSpec = parSpec;
	hairCell3Ptr->updateProcessVariablesFlag = TRUE;
	hairCell3Ptr->replenishRateFlag = TRUE;
	hairCell3Ptr->lossRateFlag = TRUE;
	hairCell3Ptr->reprocessRateFlag = TRUE;
	hairCell3Ptr->recoveryRateFlag = TRUE;
	hairCell3Ptr->maxFreePoolFlag = TRUE;
	hairCell3Ptr->permeabilityPHFlag = TRUE;
	hairCell3Ptr->permeabilityPZFlag = TRUE;
	hairCell3Ptr->firingRateFlag = TRUE;
					
	hairCell3Ptr->maxFreePool_M = 1;
	hairCell3Ptr->replenishRate_y = 5.05;
	hairCell3Ptr->lossRate_l = 2500.0;
	hairCell3Ptr->reprocessRate_x = 66.31;
	hairCell3Ptr->recoveryRate_r = 6580.0;
	hairCell3Ptr->permeabilityPH_h = 800.0;
	hairCell3Ptr->permeabilityPZ_z = 8.6538e+10;
	hairCell3Ptr->firingRate_h2 = 50000.0;

	if (!SetUniParList_IHC_Meddis86a()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_IHC_Meddis86a();
		return(FALSE);
	}
	hairCell3Ptr->hCChannels = NULL;
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
Free_IHC_Meddis86a(void)
{
	if (hairCell3Ptr == NULL)
		return(TRUE);
	FreeProcessVariables_IHC_Meddis86a();
	if (hairCell3Ptr->parList)
		FreeList_UniParMgr(&hairCell3Ptr->parList);
	if (hairCell3Ptr->parSpec == GLOBAL) {
		free(hairCell3Ptr);
		hairCell3Ptr = NULL;
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
SetUniParList_IHC_Meddis86a(void)
{
	static const char *funcName = "SetUniParList_IHC_Meddis86a";
	UniParPtr	pars;

	if ((hairCell3Ptr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  IHC_MEDDIS86A_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = hairCell3Ptr->parList->pars;
	SetPar_UniParMgr(&pars[IHC_MEDDIS86A_MAXFREEPOOL_M], "MAX_FREE_POOL",
	  "Max. no. of transmitter packets in free pool.",
	  UNIPAR_REAL,
	  &hairCell3Ptr->maxFreePool_M, NULL,
	  (void * (*)) SetMaxFreePool_IHC_Meddis86a);
	SetPar_UniParMgr(&pars[IHC_MEDDIS86A_REPLENISHRATE_Y], "REPLENISH_Y",
	  "Replenishment rate (units per second).",
	  UNIPAR_REAL,
	  &hairCell3Ptr->replenishRate_y, NULL,
	  (void * (*)) SetReplenishRate_IHC_Meddis86a);
	SetPar_UniParMgr(&pars[IHC_MEDDIS86A_LOSSRATE_L], "LOSS_L",
	  "Loss rate (units per second).",
	  UNIPAR_REAL,
	  &hairCell3Ptr->lossRate_l, NULL,
	  (void * (*)) SetLossRate_IHC_Meddis86a);
	SetPar_UniParMgr(&pars[IHC_MEDDIS86A_REPROCESSRATE_X], "REPROCESS_X",
	  "Reprocessing rate (units per second).",
	  UNIPAR_REAL,
	  &hairCell3Ptr->reprocessRate_x, NULL,
	  (void * (*)) SetReprocessRate_IHC_Meddis86a);
	SetPar_UniParMgr(&pars[IHC_MEDDIS86A_RECOVERYRATE_R], "RECOVERY_R",
	  "Recovery rate (units per second).",
	  UNIPAR_REAL,
	  &hairCell3Ptr->recoveryRate_r, NULL,
	  (void * (*)) SetRecoveryRate_IHC_Meddis86a);
	SetPar_UniParMgr(&pars[IHC_MEDDIS86A_PERMEABILITYPH_H], "PERM_CONST_H",
	  "Permeability constant, h.",
	  UNIPAR_REAL,
	  &hairCell3Ptr->permeabilityPH_h, NULL,
	  (void * (*)) SetPermeabilityPH_IHC_Meddis86a);
	SetPar_UniParMgr(&pars[IHC_MEDDIS86A_PERMEABILITYPZ_Z], "PERM_CONST_Z",
	  "Permeability constant, z.",
	  UNIPAR_REAL,
	  &hairCell3Ptr->permeabilityPZ_z, NULL,
	  (void * (*)) SetPermeabilityPZ_IHC_Meddis86a);
	SetPar_UniParMgr(&pars[IHC_MEDDIS86A_FIRINGRATE_H2], "FIRING_RATE_H2",
	  "Firing rate (spikes per second).",
	  UNIPAR_REAL,
	  &hairCell3Ptr->firingRate_h2, NULL,
	  (void * (*)) SetFiringRate_IHC_Meddis86a);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_IHC_Meddis86a(void)
{
	static const char	*funcName = "GetUniParListPtr_IHC_Meddis86a";

	if (hairCell3Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (hairCell3Ptr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(hairCell3Ptr->parList);

}

/********************************* SetReplenishRate ***************************/

/*
 * This function sets the module's replenish rate, y, parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetReplenishRate_IHC_Meddis86a(double theReplenishRate)
{
	static const char	*funcName = "SetReplenishRate_IHC_Meddis86a";

	if (hairCell3Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	hairCell3Ptr->replenishRateFlag = TRUE;
	hairCell3Ptr->replenishRate_y = theReplenishRate;
	hairCell3Ptr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetLossRate ********************************/

/*
 * This function sets the module's loss rate, l, parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetLossRate_IHC_Meddis86a(double theLossRate)
{
	static const char	*funcName = "SetLossRate_IHC_Meddis86a";

	if (hairCell3Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	hairCell3Ptr->lossRateFlag = TRUE;
	hairCell3Ptr->lossRate_l = theLossRate;
	hairCell3Ptr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetReprocessRate ***************************/

/*
 * This function sets the module's reprocess rate, x  parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetReprocessRate_IHC_Meddis86a(double theReprocessRate)
{
	static const char	*funcName = "SetReprocessRate_IHC_Meddis86a";

	if (hairCell3Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	hairCell3Ptr->reprocessRateFlag = TRUE;
	hairCell3Ptr->reprocessRate_x = theReprocessRate;
	hairCell3Ptr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetRecoveryRate ****************************/

/*
 * This function sets the module's recovery rate, r parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetRecoveryRate_IHC_Meddis86a(double theRecoveryRate)
{
	static const char	*funcName = "SetRecoveryRate_IHC_Meddis86a";

	if (hairCell3Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	hairCell3Ptr->recoveryRateFlag = TRUE;
	hairCell3Ptr->recoveryRate_r = theRecoveryRate;
	hairCell3Ptr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetMaxFreePool *****************************/

/*
 * This function sets the module's maximum free transmitter pool, M parameter.
 * This is a double variable to maintain transparency with Meddis 86 model.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetMaxFreePool_IHC_Meddis86a(double theMaxFreePool)
{
	static const char *funcName = "SetMaxFreePool_IHC_Meddis86a";

	if (hairCell3Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (theMaxFreePool < 1.0) {
		NotifyError("%s: Illegal initial value: M = %d.", funcName,
		  theMaxFreePool);
		return(FALSE);
	}
	hairCell3Ptr->maxFreePoolFlag = TRUE;
	hairCell3Ptr->maxFreePool_M = theMaxFreePool;
	hairCell3Ptr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetPermeabilityPH **************************/

/*
 * This function sets the module's permeabilityP constant H parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPermeabilityPH_IHC_Meddis86a(double thePermeabilityPH)
{
	static const char	*funcName = "SetPermeabilityPH_IHC_Meddis86a";

	if (hairCell3Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	hairCell3Ptr->permeabilityPHFlag = TRUE;
	hairCell3Ptr->permeabilityPH_h = thePermeabilityPH;
	hairCell3Ptr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetPermeabilityPZ **************************/

/*
 * This function sets the module's permeability constant Z parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPermeabilityPZ_IHC_Meddis86a(double thePermeabilityPZ)
{
	static const char	*funcName = "SetPermeabilityPZ_IHC_Meddis86a";

	if (hairCell3Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	hairCell3Ptr->permeabilityPZFlag = TRUE;
	hairCell3Ptr->permeabilityPZ_z = thePermeabilityPZ;
	hairCell3Ptr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetFiringRate ******************************/

/*
 * This function sets the module's permeability constant h parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetFiringRate_IHC_Meddis86a(double theFiringRate)
{
	static const char	*funcName = "SetFiringRate_IHC_Meddis86a";

	if (hairCell3Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	hairCell3Ptr->firingRateFlag = TRUE;
	hairCell3Ptr->firingRate_h2 = theFiringRate;
	hairCell3Ptr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetPars ************************************/

/*
 * This function sets all the module's parameters.
 */

BOOLN
SetPars_IHC_Meddis86a(double y, double l, double x, double r, double M, double h,
  double z, double h2)
{
	BOOLN	ok;
	
	ok = TRUE;
	if (!SetReplenishRate_IHC_Meddis86a(y))
		ok = FALSE;
	if (!SetLossRate_IHC_Meddis86a(l))
		ok = FALSE;
	if (!SetReprocessRate_IHC_Meddis86a(x))
		ok = FALSE;
	if (!SetRecoveryRate_IHC_Meddis86a(r))
		ok = FALSE;
	if (!SetMaxFreePool_IHC_Meddis86a(M))
		ok = FALSE;
	if (!SetPermeabilityPH_IHC_Meddis86a(h))
		ok = FALSE;
	if (!SetPermeabilityPZ_IHC_Meddis86a(z))
		ok = FALSE;
	if (!SetFiringRate_IHC_Meddis86a(h2))
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
CheckPars_IHC_Meddis86a(void)
{
	static const char *funcName = "CheckPars_IHC_Meddis86a";
	BOOLN	ok;
	
	ok = TRUE;
	if (hairCell3Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!hairCell3Ptr->replenishRateFlag) {
		NotifyError("%s: Replenish rate constant, y, not correctly set.",
		  funcName);
		ok = FALSE;
	}
	if (!hairCell3Ptr->lossRateFlag) {
		NotifyError("%s: Loss rate constant, l, not correctly set.", funcName);
		ok = FALSE;
	}
	if (!hairCell3Ptr->reprocessRateFlag) {
		NotifyError("%s: Reprocessing rate constant, x, not correctly set.",
		  funcName);
		ok = FALSE;
	}
	if (!hairCell3Ptr->recoveryRateFlag) {
		NotifyError("%s: Recovery rate constant, r, not correctly set.",
		  funcName);
		ok = FALSE;
	}
	if (!hairCell3Ptr->maxFreePoolFlag) {
		NotifyError("%s: Max. free transmitter in pool constant, M, not "\
		  "correctly set.", funcName);
		ok = FALSE;
	}
	if (!hairCell3Ptr->permeabilityPHFlag) {
		NotifyError("%s: Permeability constant, h, not correctly set.",
		  funcName);
		ok = FALSE;
	}
	if (!hairCell3Ptr->permeabilityPZFlag) {
		NotifyError("%s: Permeability constant, z, not correctly set.",
		  funcName);
		ok = FALSE;
	}
	if (!hairCell3Ptr->firingRateFlag) {
		NotifyError("%s: Firing rate constant, h2, not correctly set.",
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
PrintPars_IHC_Meddis86a(void)
{
	static const char *funcName = "PrintPars_IHC_Meddis86a";

	if (!CheckPars_IHC_Meddis86a()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Meddis 86a Inner Hair Cell Module Parameters:-\n");
	DPrint("\tPermeability constants: z = %g,\th = %g \n",
	  hairCell3Ptr->permeabilityPZ_z, hairCell3Ptr->permeabilityPH_h);
	DPrint("\tReplenishment rate, y = %g /s,",
	  hairCell3Ptr->replenishRate_y);
	DPrint("\tLoss rate, l = %g /s,\n",
	  hairCell3Ptr->lossRate_l);
	DPrint("\tReprocess rate, x = %g /s,",
	  hairCell3Ptr->reprocessRate_x);
	DPrint("\tRecovery rate, r = %g /s,\n",
	  hairCell3Ptr->recoveryRate_r);
	DPrint("\tFiring rate, h2 = %g spikes/s\n",
	  hairCell3Ptr->firingRate_h2);
	DPrint("\tMaximum transmitter packets in free pool, "\
	  "M = %g,\n", hairCell3Ptr->maxFreePool_M);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.
 */
 
BOOLN
ReadPars_IHC_Meddis86a(char *fileName)
{
	static const char *funcName = "ReadPars_IHC_Meddis86a";
	BOOLN	ok;
	char	*filePath;
	double	replenishRate_y, lossRate_l, reprocessRate_x, recoveryRate_r;
	double	permeabilityPH_h, permeabilityPZ_z, maxFreePool_M, firingRate_h2;
	FILE	*fp;
	
	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(filePath, "r")) == NULL) {
		NotifyError("%s: Cannot open data file '%s'.\n", funcName, filePath);
		return(FALSE);
	}
	DPrint("%s: Reading from '%s';\n", funcName, filePath);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, "%lf", &maxFreePool_M))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &replenishRate_y))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &lossRate_l))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &reprocessRate_x))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &recoveryRate_r))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &firingRate_h2))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &permeabilityPH_h))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &permeabilityPZ_z))
		ok = FALSE;
 	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "\
		  "parameter file '%s'.", funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_IHC_Meddis86a(replenishRate_y, lossRate_l, reprocessRate_x,
	  recoveryRate_r, maxFreePool_M, permeabilityPH_h, permeabilityPZ_z,
	  firingRate_h2)) {
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
SetParsPointer_IHC_Meddis86a(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_IHC_Meddis86a";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	hairCell3Ptr = (HairCell3Ptr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_IHC_Meddis86a(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_IHC_Meddis86a";

	if (!SetParsPointer_IHC_Meddis86a(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_IHC_Meddis86a(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = hairCell3Ptr;
	theModule->CheckPars = CheckPars_IHC_Meddis86a;
	theModule->Free = Free_IHC_Meddis86a;
	theModule->GetUniParListPtr = GetUniParListPtr_IHC_Meddis86a;
	theModule->PrintPars = PrintPars_IHC_Meddis86a;
	theModule->ReadPars = ReadPars_IHC_Meddis86a;
	theModule->RunProcess = RunModel_IHC_Meddis86a;
	theModule->SetParsPointer = SetParsPointer_IHC_Meddis86a;
	return(TRUE);

}

/********************************* CheckData **********************************/

/*
 * This routine checks that the input signal is correctly initialised, and
 * determines whether the parameter values are valid for the signal
 * sampling interval.
 */

BOOLN
CheckData_IHC_Meddis86a(EarObjectPtr data)
{
	static const char *funcName = "CheckData_IHC_Meddis86a";
	BOOLN	ok;
	double	dt;
	
	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}	
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	if (!CheckRamp_SignalData(data->inSignal[0]))
		return(FALSE);
	dt = data->inSignal[0]->dt;
	if (dt > MEDDIS86A_MAX_DT) {
		NotifyError("%s: Maximum sampling interval exceeded.", funcName);
		return(FALSE);
	}
	ok = TRUE;
	if (hairCell3Ptr->replenishRate_y * dt >= 1.0) {
		NotifyError("%s: Replenishment rate, y = %g /s is too high for the "\
		  "sampling interval.", funcName, hairCell3Ptr->replenishRate_y);
		ok = FALSE;
	}
	if (hairCell3Ptr->lossRate_l * dt >= 1.0) {
		NotifyError("%s: Loss rate, l = %g /s is too high for the sampling "\
		  "interval.", funcName, hairCell3Ptr->lossRate_l);
		ok = FALSE;
	}
	if (hairCell3Ptr->recoveryRate_r * dt >= 1.0) {
		NotifyError("%s: Recovery rate, r = %g /s is too high for the "\
		  "sampling interval.", funcName, hairCell3Ptr->recoveryRate_r);
		ok = FALSE;
	}
	if (hairCell3Ptr->reprocessRate_x * dt >= 1.0) {
		NotifyError("%s: Reprocess rate, x = %g /s is too high for the "\
		  "sampling interval.", funcName, hairCell3Ptr->reprocessRate_x);
		ok = FALSE;
	}
	/* if (hairCell3Ptr->permeabilityPH_h > maximumH) {
		maximumH = log(1.0 / (hairCell3Ptr->permeabilityPZ_z * dt));
		NotifyError("%s: The permeability constant, h = %g, is too high "\
		  "for\nthe signal sampling interval, dt = %g ms, and the "\
		  "permeability constant, z = %g (h < %g).", funcName,
		  hairCell3Ptr->permeabilityPH_h, dt, hairCell3Ptr->permeabilityPZ_z);
		ok = FALSE;
	} */
	return(ok);
	
}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 * It initialises the hair cells to the spontaneous firing rate.
 * The Spontaneous firing rate is determined from the equilbrium state of the
 * system with no input: the reservoir contents do not change in time.
 * For the p'(0) value it uses the first value from the input signal, which it
 * assumes to contain the appropriate value.  The is assumed to be the same
 * for all channels.
 */

BOOLN
InitProcessVariables_IHC_Meddis86a(EarObjectPtr data)
{
	static const char *funcName = "InitProcessVariables_IHC_Meddis86a";
	int		i;
	double	spontPerm_k0, spontCleft_c0, spontFreePool_q0, spontReprocess_w0;
	HairCell3Ptr	hC;
	
	if (hairCell3Ptr->updateProcessVariablesFlag || data->updateProcessFlag ||
	  (data->timeIndex == PROCESS_START_TIME)) {
		hC = hairCell3Ptr;		/* Shorter variable for long formulae. */
		if (hairCell3Ptr->updateProcessVariablesFlag ||
		  data->updateProcessFlag) {
			FreeProcessVariables_IHC_Meddis86a();
			if ((hC->hCChannels = (HairCellVars3Ptr) calloc(
			  data->outSignal->numChannels, sizeof (HairCellVars3))) == NULL) {
				NotifyError("%s: Out of memory.", funcName);
				return(FALSE);
			}
			hairCell3Ptr->updateProcessVariablesFlag = FALSE;
		}
		spontPerm_k0 = hC->permeabilityPZ_z * exp(hC->permeabilityPH_h *
		  data->inSignal[0]->channel[0][0]);
		spontCleft_c0 = hC->maxFreePool_M * hC->replenishRate_y *
		   spontPerm_k0 / (hC->replenishRate_y * (hC->lossRate_l +
		   hC->recoveryRate_r) + spontPerm_k0 * hC->lossRate_l);
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
FreeProcessVariables_IHC_Meddis86a(void)
{
	if (hairCell3Ptr->hCChannels == NULL)
		return;
	free(hairCell3Ptr->hCChannels);
	hairCell3Ptr->hCChannels = NULL;
	hairCell3Ptr->updateProcessVariablesFlag = TRUE;

}
/********************************* RunModel ***********************************/

/*
 * This routine runs the input signal through the model and puts the result
 * into the output signal.  It checks that all initialisation has been
 * correctly carried out by calling the appropriate checking routines.
 */

BOOLN
RunModel_IHC_Meddis86a(EarObjectPtr data)
{
	static const char *funcName = "RunModel_IHC_Meddis86a";
	BOOLN	clipped;
	int		i;
	ChanLen	j;
	double	dt, ymdt, ydt, xdt, rdt, kdt, zdt, l_Plus_rdt, h2dt;
	double	reUptake, reUptakeAndLost,replenish, reprocessed, ejected;
	ChanData	*inPtr, *outPtr;
	HairCell3Ptr	hC;
	
	if (!CheckPars_IHC_Meddis86a())		
		return(FALSE);
	if (!CheckData_IHC_Meddis86a(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels,
	  data->inSignal[0]->length, data->inSignal[0]->dt)) {
		NotifyError("%s: Could not initialise output signal.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "Meddis 86a quantal hair cell");
	if (!InitProcessVariables_IHC_Meddis86a(data)) {
		NotifyError("%s: Could not initialise the process variables.",
		  funcName);
		return(FALSE);
	}
	dt = data->outSignal->dt;
	hC = hairCell3Ptr;		/* Shorter name for use with long equations. */
	
	ymdt = hC->replenishRate_y * hC->maxFreePool_M * dt;
	xdt = hC->reprocessRate_x * dt;
	ydt = hC->replenishRate_y * dt;
	l_Plus_rdt = (hC->lossRate_l + hC->recoveryRate_r) * dt;
	rdt = hC->recoveryRate_r * dt;
	zdt = hC->permeabilityPZ_z * dt;
	h2dt = hC->firingRate_h2 * dt;
	for (i = 0, clipped = FALSE; i < data->outSignal->numChannels; i++) {
		inPtr = data->inSignal[0]->channel[i];
		outPtr = data->outSignal->channel[i];
		for (j = 0; j < data->outSignal->length; j++) {
			kdt = zdt * exp(hC->permeabilityPH_h * *inPtr++);
			if (kdt >= 1.0) {
				kdt = 0.99;
				clipped = TRUE;
			}
			replenish = (hC->hCChannels[i].reservoirQ < hC->maxFreePool_M)?
			  ymdt - ydt * hC->hCChannels[i].reservoirQ: 0.0;
			reprocessed = xdt * hC->hCChannels[i].reprocessedW;
			ejected = kdt * hC->hCChannels[i].reservoirQ;	
			reUptake = rdt * hC->hCChannels[i].cleftC;
			reUptakeAndLost = l_Plus_rdt * hC->hCChannels[i].cleftC;

			hC->hCChannels[i].reservoirQ += replenish - ejected + reprocessed;
			hC->hCChannels[i].cleftC += ejected - reUptakeAndLost;
			hC->hCChannels[i].reprocessedW += reUptake - reprocessed;

			/* Spike prob. */
			*outPtr++ = (ChanData) (h2dt * hC->hCChannels[i].cleftC);
		}
	}
	/*if (clipped)
		NotifyWarning("%s: Clipping has occurred.", funcName); */
	SetProcessContinuity_EarObject(data);
	return(TRUE);
		
} /* RunModel_IHC_Meddis86a */
