/**********************
 *
 * File:		MoIHCMeddis2000.c
 * Purpose:		This module contains the routines for synaptic adaptation of
 *				inner hair cells. It is a stochastic model, and with the
 *				exception of a probabilistic output option, is the same as the
 *				Meddis 95 module, but without the transmitter release stage.
 *				It is intended to be used with the Calcium transmitter release
 *				stage.
 * Comments:	Written using ModuleProducer version 1.2.5 (May  7 1999).
 * Authors:		L.P. O'Mard, revised Chris Sumner (7th May 2000).
 * Created:		16 Mar 2000
 * Updated:	
 * Copyright:	(c) 2000, CNBH University of Essex.
 *
 *********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "FiParFile.h"
#include "UtRandom.h"
#include "GeNSpecLists.h"
#include "MoIHCMeddis2000.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

HairCell2Ptr	hairCell2Ptr = NULL;

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
Free_IHC_Meddis2000(void)
{
	/* static const char	*funcName = "Free_IHC_Meddis2000";  */

	if (hairCell2Ptr == NULL)
		return(FALSE);
	if (hairCell2Ptr->parList)
		FreeList_UniParMgr(&hairCell2Ptr->parList);
	if (hairCell2Ptr->parSpec == GLOBAL) {
		free(hairCell2Ptr);
		hairCell2Ptr = NULL;
	}
	return(TRUE);

}

/****************************** InitOpModeList ********************************/

/*
 * This function initialises the 'opMode' list array
 */

BOOLN
InitOpModeList_IHC_Meddis2000(void)
{
	static NameSpecifier	modeList[] = {

			{ "spike",	IHC_MEDDIS2000_OPMODE_SPIKE },
			{ "prob",	IHC_MEDDIS2000_OPMODE_PROB },
			{ "",	IHC_MEDDIS2000_OPMODE_NULL },
		};
	hairCell2Ptr->opModeList = modeList;
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
Init_IHC_Meddis2000(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_IHC_Meddis2000";

	if (parSpec == GLOBAL) {
		if (hairCell2Ptr != NULL)
			Free_IHC_Meddis2000();
		if ((hairCell2Ptr = (HairCell2Ptr) malloc(sizeof(HairCell2))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (hairCell2Ptr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;
	hairCell2Ptr->parSpec = parSpec;
	hairCell2Ptr->opModeFlag = FALSE;
	hairCell2Ptr->diagModeFlag = FALSE;
	hairCell2Ptr->ranSeedFlag = FALSE;
	hairCell2Ptr->CaVrevFlag = FALSE;
	hairCell2Ptr->betaCaFlag = FALSE;
	hairCell2Ptr->gammaCaFlag = FALSE;
	hairCell2Ptr->pCaFlag = TRUE;
	hairCell2Ptr->GCaMaxFlag = FALSE;
	hairCell2Ptr->perm_Ca0Flag = FALSE;
	hairCell2Ptr->perm_zFlag = FALSE;
	hairCell2Ptr->tauCaChanFlag = FALSE;
	hairCell2Ptr->tauConcCaFlag = FALSE;
	hairCell2Ptr->maxFreePool_MFlag = FALSE;
	hairCell2Ptr->replenishRate_yFlag = FALSE;
	hairCell2Ptr->lossRate_lFlag = FALSE;
	hairCell2Ptr->reprocessRate_xFlag = FALSE;
	hairCell2Ptr->recoveryRate_rFlag = FALSE;
	hairCell2Ptr->opMode = 0;
	hairCell2Ptr->diagMode = 0;
	hairCell2Ptr->ranSeed = 0;
	hairCell2Ptr->CaVrev = 0.0;
	hairCell2Ptr->betaCa = 0.0;
	hairCell2Ptr->gammaCa = 0.0;
	hairCell2Ptr->pCa = 3.0;
	hairCell2Ptr->GCaMax = 0.0;
	hairCell2Ptr->perm_Ca0 = 0.0;
	hairCell2Ptr->perm_z = 0.0;
	hairCell2Ptr->tauCaChan = 0.0;
	hairCell2Ptr->tauConcCa = 0.0;
	hairCell2Ptr->maxFreePool_M = 0;
	hairCell2Ptr->replenishRate_y = 0.0;
	hairCell2Ptr->lossRate_l = 0.0;
	hairCell2Ptr->reprocessRate_x = 0.0;
	hairCell2Ptr->recoveryRate_r = 0.0;

	InitOpModeList_IHC_Meddis2000();

	if ((hairCell2Ptr->diagModeList = InitNameList_NSpecLists(
	  DiagModeList_NSpecLists(0), hairCell2Ptr->diagFileName)) == NULL)
		return(FALSE);

	if (!SetUniParList_IHC_Meddis2000()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_IHC_Meddis2000();
		return(FALSE);
	}
	strcpy(hairCell2Ptr->diagFileName, DEFAULT_FILE_NAME);
	hairCell2Ptr->hCChannels = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_IHC_Meddis2000(void)
{
	static const char *funcName = "SetUniParList_IHC_Meddis2000";
	UniParPtr	pars;

	if ((hairCell2Ptr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  IHC_MEDDIS2000_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = hairCell2Ptr->parList->pars;
	SetPar_UniParMgr(&pars[IHC_MEDDIS2000_OPMODE], "OP_MODE",
	  "Output mode: stochastic ('spike') or probability ('prob').",
	  UNIPAR_NAME_SPEC,
	  &hairCell2Ptr->opMode, hairCell2Ptr->opModeList,
	  (void * (*)) SetOpMode_IHC_Meddis2000);
	SetPar_UniParMgr(&pars[IHC_MEDDIS2000_DIAGMODE], "DIAG_MODE",
	  "Diagnostic mode. Outputs internal states of running model ('off', "
	  "'screen' or <file name>).",
	  UNIPAR_NAME_SPEC_WITH_FILE,
	  &hairCell2Ptr->diagMode, hairCell2Ptr->diagModeList,
	  (void * (*)) SetDiagMode_IHC_Meddis2000);
	SetPar_UniParMgr(&pars[IHC_MEDDIS2000_RANSEED], "RAN_SEED",
	  "Random number seed (0 for different seed for each run).",
	  UNIPAR_LONG,
	  &hairCell2Ptr->ranSeed, NULL,
	  (void * (*)) SetRanSeed_IHC_Meddis2000);
	SetPar_UniParMgr(&pars[IHC_MEDDIS2000_CAVREV], "REV_POT_ECA",
	  "Calcium reversal potential, E_Ca (Volts).",
	  UNIPAR_REAL,
	  &hairCell2Ptr->CaVrev, NULL,
	  (void * (*)) SetCaVrev_IHC_Meddis2000);
	SetPar_UniParMgr(&pars[IHC_MEDDIS2000_BETACA], "BETA_CA",
	  "Calcium channel Boltzmann function parameter, beta.",
	  UNIPAR_REAL,
	  &hairCell2Ptr->betaCa, NULL,
	  (void * (*)) SetBetaCa_IHC_Meddis2000);
	SetPar_UniParMgr(&pars[IHC_MEDDIS2000_GAMMACA], "GAMMA_CA",
	  "Calcium channel Boltzmann function parameter, gamma.",
	  UNIPAR_REAL,
	  &hairCell2Ptr->gammaCa, NULL,
	  (void * (*)) SetGammaCa_IHC_Meddis2000);
	SetPar_UniParMgr(&pars[IHC_MEDDIS2000_PCA], "POWER_CA",
	  "Calcium channel transmitter release exponent (power).",
	  UNIPAR_REAL,
	  &hairCell2Ptr->pCa, NULL,
	  (void * (*)) SetPCa_IHC_Meddis2000);
	SetPar_UniParMgr(&pars[IHC_MEDDIS2000_GCAMAX], "GMAX_CA",
	  "Maximum calcium conductance (Siemens).",
	  UNIPAR_REAL,
	  &hairCell2Ptr->GCaMax, NULL,
	  (void * (*)) SetGCaMax_IHC_Meddis2000);
	SetPar_UniParMgr(&pars[IHC_MEDDIS2000_PERM_CA0], "CONC_THRESH_CA",
	  "Calcium threshold Concentration.",
	  UNIPAR_REAL,
	  &hairCell2Ptr->perm_Ca0, NULL,
	  (void * (*)) SetPerm_Ca0_IHC_Meddis2000);
	SetPar_UniParMgr(&pars[IHC_MEDDIS2000_PERM_Z], "PERM_Z",
	  "Transmitter release permeability, Z (unitless gain)",
	  UNIPAR_REAL,
	  &hairCell2Ptr->perm_z, NULL,
	  (void * (*)) SetPerm_z_IHC_Meddis2000);
	SetPar_UniParMgr(&pars[IHC_MEDDIS2000_TAUCACHAN], "TAU_M",
	  "Calcium current time constant (s).",
	  UNIPAR_REAL,
	  &hairCell2Ptr->tauCaChan, NULL,
	  (void * (*)) SetTauCaChan_IHC_Meddis2000);
	SetPar_UniParMgr(&pars[IHC_MEDDIS2000_TAUCONCCA], "TAU_CA",
	  "Calcium ion diffusion (accumulation) time constant (s).",
	  UNIPAR_REAL,
	  &hairCell2Ptr->tauConcCa, NULL,
	  (void * (*)) SetTauConcCa_IHC_Meddis2000);
	SetPar_UniParMgr(&pars[IHC_MEDDIS2000_MAXFREEPOOL_M], "MAX_FREE_POOL_M",
	  "Max. no. of transmitter packets in free pool (integer).",
	  UNIPAR_INT,
	  &hairCell2Ptr->maxFreePool_M, NULL,
	  (void * (*)) SetMaxFreePool_M_IHC_Meddis2000);
	SetPar_UniParMgr(&pars[IHC_MEDDIS2000_REPLENISHRATE_Y], "REPLENISH_Y",
	  "Replenishment rate (units per second).",
	  UNIPAR_REAL,
	  &hairCell2Ptr->replenishRate_y, NULL,
	  (void * (*)) SetReplenishRate_y_IHC_Meddis2000);
	SetPar_UniParMgr(&pars[IHC_MEDDIS2000_LOSSRATE_L], "LOSS_L",
	  "Loss rate (units per second).",
	  UNIPAR_REAL,
	  &hairCell2Ptr->lossRate_l, NULL,
	  (void * (*)) SetLossRate_l_IHC_Meddis2000);
	SetPar_UniParMgr(&pars[IHC_MEDDIS2000_REPROCESSRATE_X], "REPROCESS_X",
	  "Reprocessing rate (units per second).",
	  UNIPAR_REAL,
	  &hairCell2Ptr->reprocessRate_x, NULL,
	  (void * (*)) SetReprocessRate_x_IHC_Meddis2000);
	SetPar_UniParMgr(&pars[IHC_MEDDIS2000_RECOVERYRATE_R], "RECOVERY_R",
	  "Recovery rate (units per second).",
	  UNIPAR_REAL,
	  &hairCell2Ptr->recoveryRate_r, NULL,
	  (void * (*)) SetRecoveryRate_r_IHC_Meddis2000);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_IHC_Meddis2000(void)
{
	static const char	*funcName = "GetUniParListPtr_IHC_Meddis2000";

	if (hairCell2Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (hairCell2Ptr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(hairCell2Ptr->parList);

}

/****************************** SetOpMode *************************************/

/*
 * This function sets the module's opMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOpMode_IHC_Meddis2000(char * theOpMode)
{
	static const char	*funcName = "SetOpMode_IHC_Meddis2000";
	int		specifier;

	if (hairCell2Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theOpMode,
		hairCell2Ptr->opModeList)) == IHC_MEDDIS2000_OPMODE_NULL) {
		NotifyError("%s: Illegal name (%s).", funcName, theOpMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;
	hairCell2Ptr->opModeFlag = TRUE;
	hairCell2Ptr->opMode = specifier;
	return(TRUE);

}

/****************************** SetDiagMode ***********************************/

/*
 * This function sets the module's diagMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDiagMode_IHC_Meddis2000(char * theDiagMode)
{
	static const char	*funcName = "SetDiagMode_IHC_Meddis2000";

	if (hairCell2Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}

	/*** Put any other required checks here. ***/
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;
	hairCell2Ptr->diagModeFlag = TRUE;
	hairCell2Ptr->diagMode = IdentifyDiag_NSpecLists(theDiagMode,
	  hairCell2Ptr->diagModeList);
	return(TRUE);

}

/****************************** SetRanSeed ************************************/

/*
 * This function sets the module's ranSeed parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRanSeed_IHC_Meddis2000(long theRanSeed)
{
	static const char	*funcName = "SetRanSeed_IHC_Meddis2000";

	if (hairCell2Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;
	hairCell2Ptr->ranSeedFlag = TRUE;
	hairCell2Ptr->ranSeed = theRanSeed;
	return(TRUE);

}

/****************************** SetCaVrev *************************************/

/*
 * This function sets the module's CaVrev parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCaVrev_IHC_Meddis2000(double theCaVrev)
{
	static const char	*funcName = "SetCaVrev_IHC_Meddis2000";

	if (hairCell2Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;
	hairCell2Ptr->CaVrevFlag = TRUE;
	hairCell2Ptr->CaVrev = theCaVrev;
	return(TRUE);

}

/****************************** SetBetaCa *************************************/

/*
 * This function sets the module's betaCa parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetBetaCa_IHC_Meddis2000(double theBetaCa)
{
	static const char	*funcName = "SetBetaCa_IHC_Meddis2000";

	if (hairCell2Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;
	hairCell2Ptr->betaCaFlag = TRUE;
	hairCell2Ptr->betaCa = theBetaCa;
	return(TRUE);

}

/****************************** SetGammaCa ************************************/

/*
 * This function sets the module's gammaCa parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetGammaCa_IHC_Meddis2000(double theGammaCa)
{
	static const char	*funcName = "SetGammaCa_IHC_Meddis2000";

	if (hairCell2Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;
	hairCell2Ptr->gammaCaFlag = TRUE;
	hairCell2Ptr->gammaCa = theGammaCa;
	return(TRUE);

}

/****************************** SetPCa ****************************************/

/*
 * This function sets the module's pCa parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPCa_IHC_Meddis2000(double thePCa)
{
	static const char	*funcName = "SetPCa_IHC_Meddis2000";

	if (hairCell2Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;
	hairCell2Ptr->pCaFlag = TRUE;
	hairCell2Ptr->pCa = thePCa;
	return(TRUE);

}

/****************************** SetGCaMax *************************************/

/*
 * This function sets the module's GCaMax parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetGCaMax_IHC_Meddis2000(double theGCaMax)
{
	static const char	*funcName = "SetGCaMax_IHC_Meddis2000";

	if (hairCell2Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;
	hairCell2Ptr->GCaMaxFlag = TRUE;
	hairCell2Ptr->GCaMax = theGCaMax;
	return(TRUE);

}

/****************************** SetPerm_Ca0 ***********************************/

/*
 * This function sets the module's perm_Ca0 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPerm_Ca0_IHC_Meddis2000(double thePerm_Ca0)
{
	static const char	*funcName = "SetPerm_Ca0_IHC_Meddis2000";

	if (hairCell2Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;
	hairCell2Ptr->perm_Ca0Flag = TRUE;
	hairCell2Ptr->perm_Ca0 = thePerm_Ca0;
	return(TRUE);

}

/****************************** SetPerm_z *************************************/

/*
 * This function sets the module's Perm_z parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPerm_z_IHC_Meddis2000(double thePerm_z)
{
	static const char	*funcName = "SetPerm_z_IHC_Meddis2000";

	if (hairCell2Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;
	hairCell2Ptr->perm_zFlag = TRUE;
	hairCell2Ptr->perm_z = thePerm_z;
	return(TRUE);

}

/****************************** SetTauCaChan **********************************/

/*
 * This function sets the module's tauCaChan parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTauCaChan_IHC_Meddis2000(double theTauCaChan)
{
	static const char	*funcName = "SetTauCaChan_IHC_Meddis2000";

	if (hairCell2Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;
	hairCell2Ptr->tauCaChanFlag = TRUE;
	hairCell2Ptr->tauCaChan = theTauCaChan;
	return(TRUE);

}

/****************************** SetTauConcCa **********************************/

/*
 * This function sets the module's tauConcCa parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTauConcCa_IHC_Meddis2000(double theTauConcCa)
{
	static const char	*funcName = "SetTauConcCa_IHC_Meddis2000";

	if (hairCell2Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;
	hairCell2Ptr->tauConcCaFlag = TRUE;
	hairCell2Ptr->tauConcCa = theTauConcCa;
	return(TRUE);

}

/****************************** SetMaxFreePool_M ******************************/

/*
 * This function sets the module's maxFreePool_M parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMaxFreePool_M_IHC_Meddis2000(int theMaxFreePool_M)
{
	static const char	*funcName = "SetMaxFreePool_M_IHC_Meddis2000";

	if (hairCell2Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;
	hairCell2Ptr->maxFreePool_MFlag = TRUE;
	hairCell2Ptr->maxFreePool_M = theMaxFreePool_M;
	return(TRUE);

}

/****************************** SetReplenishRate_y ****************************/

/*
 * This function sets the module's replenishRate_y parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetReplenishRate_y_IHC_Meddis2000(double theReplenishRate_y)
{
	static const char	*funcName = "SetReplenishRate_y_IHC_Meddis2000";

	if (hairCell2Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;
	hairCell2Ptr->replenishRate_yFlag = TRUE;
	hairCell2Ptr->replenishRate_y = theReplenishRate_y;
	return(TRUE);

}

/****************************** SetLossRate_l *********************************/

/*
 * This function sets the module's lossRate_l parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLossRate_l_IHC_Meddis2000(double theLossRate_l)
{
	static const char	*funcName = "SetLossRate_l_IHC_Meddis2000";

	if (hairCell2Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;
	hairCell2Ptr->lossRate_lFlag = TRUE;
	hairCell2Ptr->lossRate_l = theLossRate_l;
	return(TRUE);

}

/****************************** SetReprocessRate_x ****************************/

/*
 * This function sets the module's reprocessRate_x parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetReprocessRate_x_IHC_Meddis2000(double theReprocessRate_x)
{
	static const char	*funcName = "SetReprocessRate_x_IHC_Meddis2000";

	if (hairCell2Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;
	hairCell2Ptr->reprocessRate_xFlag = TRUE;
	hairCell2Ptr->reprocessRate_x = theReprocessRate_x;
	return(TRUE);

}

/****************************** SetRecoveryRate_r *****************************/

/*
 * This function sets the module's recoveryRate_r parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRecoveryRate_r_IHC_Meddis2000(double theRecoveryRate_r)
{
	static const char	*funcName = "SetRecoveryRate_r_IHC_Meddis2000";

	if (hairCell2Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;
	hairCell2Ptr->recoveryRate_rFlag = TRUE;
	hairCell2Ptr->recoveryRate_r = theRecoveryRate_r;
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
CheckPars_IHC_Meddis2000(void)
{
	static const char	*funcName = "CheckPars_IHC_Meddis2000";
	BOOLN	ok;

	ok = TRUE;
	if (hairCell2Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!hairCell2Ptr->opModeFlag) {
		NotifyError("%s: opMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!hairCell2Ptr->diagModeFlag) {
		NotifyError("%s: diagMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!hairCell2Ptr->ranSeedFlag) {
		NotifyError("%s: ranSeed variable not set.", funcName);
		ok = FALSE;
	}
	if (!hairCell2Ptr->CaVrevFlag) {
		NotifyError("%s: CaVrev variable not set.", funcName);
		ok = FALSE;
	}
	if (!hairCell2Ptr->betaCaFlag) {
		NotifyError("%s: betaCa variable not set.", funcName);
		ok = FALSE;
	}
	if (!hairCell2Ptr->gammaCaFlag) {
		NotifyError("%s: gammaCa variable not set.", funcName);
		ok = FALSE;
	}
	if (!hairCell2Ptr->pCaFlag) {
		NotifyError("%s: pCa variable not set.", funcName);
		ok = FALSE;
	}
	if (!hairCell2Ptr->GCaMaxFlag) {
		NotifyError("%s: GCaMax variable not set.", funcName);
		ok = FALSE;
	}
	if (!hairCell2Ptr->perm_Ca0Flag) {
		NotifyError("%s: Perm_Ca0 variable not set.", funcName);
		ok = FALSE;
	}
	if (!hairCell2Ptr->perm_zFlag) {
		NotifyError("%s: Perm_z variable not set.", funcName);
		ok = FALSE;
	}
	if (!hairCell2Ptr->tauCaChanFlag) {
		NotifyError("%s: tauCaChan variable not set.", funcName);
		ok = FALSE;
	}
	if (!hairCell2Ptr->tauConcCaFlag) {
		NotifyError("%s: tauConcCa variable not set.", funcName);
		ok = FALSE;
	}
	if (!hairCell2Ptr->maxFreePool_MFlag) {
		NotifyError("%s: maxFreePool_M variable not set.", funcName);
		ok = FALSE;
	}
	if (!hairCell2Ptr->replenishRate_yFlag) {
		NotifyError("%s: replenishRate_y variable not set.", funcName);
		ok = FALSE;
	}
	if (!hairCell2Ptr->lossRate_lFlag) {
		NotifyError("%s: lossRate_l variable not set.", funcName);
		ok = FALSE;
	}
	if (!hairCell2Ptr->reprocessRate_xFlag) {
		NotifyError("%s: reprocessRate_x variable not set.", funcName);
		ok = FALSE;
	}
	if (!hairCell2Ptr->recoveryRate_rFlag) {
		NotifyError("%s: recoveryRate_r variable not set.", funcName);
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
PrintPars_IHC_Meddis2000(void)
{
	static const char	*funcName = "PrintPars_IHC_Meddis2000";

	if (!CheckPars_IHC_Meddis2000()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Meddis 2000 IHC Module Parameters:-\n");
	DPrint("\tOperational Mode = %s \n",
	  hairCell2Ptr->opModeList[hairCell2Ptr->opMode].name);
	DPrint("\tDiagnostic Mode = %s \n",
	  hairCell2Ptr->diagModeList[hairCell2Ptr->diagMode].name);
	DPrint("\tRandom Seed = %ld \n", hairCell2Ptr->ranSeed);
	DPrint("\tCalcium reversal potential = %g (V)\n", hairCell2Ptr->CaVrev);
	DPrint("\tBeta = %g \n", hairCell2Ptr->betaCa);
	DPrint("\tGamma = %g \n", hairCell2Ptr->gammaCa);
	DPrint("\tcalcium stoichiometry = %g \n", hairCell2Ptr->pCa);
	DPrint("\tMax calcium conductance = %g (S)\n", hairCell2Ptr->GCaMax);
	DPrint("\tZero calcium transmitter permeability = %g \n",
	  hairCell2Ptr->perm_Ca0);
	DPrint("\tTransmitter release permeability constant, Z = %g \n",
	  hairCell2Ptr->perm_z);
	DPrint("\tCalcium channel time constant = %g (s)\n",
	  hairCell2Ptr->tauCaChan);
	DPrint("\tCalcium accumulation time constant = %g (s)\n",
	  hairCell2Ptr->tauConcCa);
	DPrint("\tMaximum transmitter packets in free pool, M  = %d \n",
	  hairCell2Ptr->maxFreePool_M);
	DPrint("\tReplenishment rate, y = %g /s\n", hairCell2Ptr->replenishRate_y);
	DPrint("\tLoss rate, l = %g /s,\tReprocessing rate, x = %g /s\n",
	  hairCell2Ptr->lossRate_l, hairCell2Ptr->reprocessRate_x);
	DPrint("\tRecovery rate, r = %g /s\n", hairCell2Ptr->recoveryRate_r);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_IHC_Meddis2000(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_IHC_Meddis2000";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	hairCell2Ptr = (HairCell2Ptr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_IHC_Meddis2000(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_IHC_Meddis2000";

	if (!SetParsPointer_IHC_Meddis2000(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_IHC_Meddis2000(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = hairCell2Ptr;
	theModule->CheckPars = CheckPars_IHC_Meddis2000;
	theModule->Free = Free_IHC_Meddis2000;
	theModule->GetUniParListPtr = GetUniParListPtr_IHC_Meddis2000;
	theModule->PrintPars = PrintPars_IHC_Meddis2000;
	theModule->RunProcess = RunModel_IHC_Meddis2000;
	theModule->SetParsPointer = SetParsPointer_IHC_Meddis2000;
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
CheckData_IHC_Meddis2000(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_IHC_Meddis2000";
	double	dt;
	BOOLN		ok;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInit_SignalData(data->inSignal[0], funcName))
		return(FALSE);

	/*** Put additional checks here. ***/

	/* Checks taken and modified from MoIHC95Meddis.c (CJS 1/2/00) */
	dt = data->inSignal[0]->dt;
	if (dt > MEDDIS2000_MAX_DT) {
		NotifyError("%s: Maximum sampling interval exceeded.", funcName);
		return(FALSE);
	}
	ok = TRUE;
	if (hairCell2Ptr->replenishRate_y * dt >= 1.0) {
		NotifyError("%s: Replenishment rate, y = %g /s is too high for the "
		  "sampling interval.", funcName, hairCell2Ptr->replenishRate_y);
		ok = FALSE;
	}
	if (hairCell2Ptr->lossRate_l * dt >= 1.0) {
		NotifyError("%s: Loss rate, l = %g /s is too high for the sampling "
		  "interval.", funcName, hairCell2Ptr->lossRate_l);
		ok = FALSE;
	}
	if (hairCell2Ptr->recoveryRate_r * dt >= 1.0) {
		NotifyError("%s: Recovery rate, r = %g /s is too high for the "
		  "sampling interval.", funcName, hairCell2Ptr->recoveryRate_r);
		ok = FALSE;
	}
	if (hairCell2Ptr->reprocessRate_x * dt >= 1.0) {
		NotifyError("%s: Reprocess rate, x = %g /s is too high for the "
		  "sampling interval.", funcName, hairCell2Ptr->reprocessRate_x);
		ok = FALSE;
	}

	/* Additional checks added for the new code */
	if ( dt/hairCell2Ptr->tauCaChan  >= 1.0) {
		NotifyError("%s: tauCaChan = %g /s is too high for the " 
		  "sampling interval.", funcName, hairCell2Ptr->tauCaChan);
		ok = FALSE;
	}
	if ( dt/hairCell2Ptr->tauConcCa  >= 1.0) {
		NotifyError("%s: tauConcCa = %g /s is too high for the "
		  "sampling interval.", funcName, hairCell2Ptr->tauConcCa);
		ok = FALSE;
	}	

	return(TRUE);

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

/* N.B. Pretty much lifted from the Meddis 95 synapse, but modifed for the new 
 * transmitter relese function (CJS 1-2-2000).
 */
 

BOOLN
InitProcessVariables_IHC_Meddis2000(EarObjectPtr data)
{
	static const char *funcName = "InitProcessVariables_IHC_Meddis2000";
	int		i;
	double	spontPerm_k0, spontCleft_c0, spontFreePool_q0, spontReprocess_w0;
	HairCell2Ptr	hC;
	
	double	  	ICa;		/* Calcium current */
	double	  	ssactCa;	/* steady state Calcium activation */

	if (hairCell2Ptr->updateProcessVariablesFlag || data->updateProcessFlag ||
	  (data->timeIndex == PROCESS_START_TIME)) {
		hC = hairCell2Ptr;		/* Shorter variable for long formulae. */

		if (hairCell2Ptr->updateProcessVariablesFlag ||
		  data->updateProcessFlag) {
			randomNumSeed = hairCell2Ptr->ranSeed;
			FreeProcessVariables_IHC_Meddis2000();
			OpenDiagnostics_NSpecLists(&hairCell2Ptr->fp, hairCell2Ptr->
			  diagModeList, hairCell2Ptr->diagMode);

			if ((hC->hCChannels = (HairCellVars2Ptr) calloc(
			  data->outSignal->numChannels, sizeof (HairCellVars2))) == NULL) {
				NotifyError("%s: Out of memory.", funcName);
				return(FALSE);
			}
			hairCell2Ptr->updateProcessVariablesFlag = FALSE;
		} 

		ssactCa = 1.0 / ( 1.0 + (exp(- (hC->gammaCa*(data->inSignal[0]->channel[
		  0][0]))) / hC->betaCa));		
		ICa = hC->GCaMax*pow(ssactCa,3)*(data->inSignal[0]->channel[0][0] -
		  hC->CaVrev);
		spontPerm_k0 = ( -ICa > hC->perm_Ca0 ) ? (hC->perm_z * (pow(-ICa, hC->
		  pCa) - pow(hC->perm_Ca0,hC->pCa))) : 0; 
		spontCleft_c0 = hC->maxFreePool_M * hC->replenishRate_y * spontPerm_k0 /
		  (hC->replenishRate_y * (hC->lossRate_l + hC->recoveryRate_r) +
		  spontPerm_k0 * hC->lossRate_l);
		if (spontCleft_c0>0) {
		   if (hC->opMode == IHC_MEDDIS2000_OPMODE_PROB) 
		      spontFreePool_q0 = spontCleft_c0 * (hC->lossRate_l +
		        hC->recoveryRate_r) / spontPerm_k0;
			else 
				spontFreePool_q0 = floor( (spontCleft_c0 * (hC->lossRate_l +
		        hC->recoveryRate_r) / spontPerm_k0) + 0.5);
		} else
		   spontFreePool_q0 = hC->maxFreePool_M;

		spontReprocess_w0 = spontCleft_c0 * hC->recoveryRate_r / hC->
		  reprocessRate_x;

		for (i = 0; i < data->outSignal->numChannels; i++) {
			hC->hCChannels[i].actCa = ssactCa;
			hC->hCChannels[i].concCa = -ICa;		
			hC->hCChannels[i].reservoirQ = spontFreePool_q0;
			hC->hCChannels[i].cleftC = spontCleft_c0;
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
FreeProcessVariables_IHC_Meddis2000(void)
{

	if (hairCell2Ptr->hCChannels == NULL)
		return;
	free(hairCell2Ptr->hCChannels);
	hairCell2Ptr->hCChannels = NULL;
	hairCell2Ptr->updateProcessVariablesFlag = TRUE;

}



/****************************** RunModel **************************************/

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
 * ssactCa:	steady state Calcium activation.
 * ICa:		Calcium current.
 */

BOOLN
RunModel_IHC_Meddis2000(EarObjectPtr data)
{
	static const char	*funcName = "RunModel_IHC_Meddis2000";
	register	ChanData	 *inPtr, *outPtr;

	BOOLN	debug;
	int		i;
	register double replenish, reprocessed, ejected;
	ChanLen	j;
	double	dt, ydt, xdt, rdt, kdt, zdt, l_Plus_rdt;
	double	reUptake, reUptakeAndLost, timer, ssactCa, ICa, k0pow, Vin;
	HairCell2Ptr	hC;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckPars_IHC_Meddis2000())
		return(FALSE);
	if (!CheckData_IHC_Meddis2000(data)) {
	 	NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "Meddis 2000 IHC. Calcium transmitter "
	  "release and quantal synapse");

	/*** Put your code here: Initialise output signal. ***/

	if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels,
	  data->inSignal[0]->length, data->inSignal[0]->dt)) {
		NotifyError("%s: Could not initialise output signal.", funcName);
		return(FALSE);
	}

	if (!InitProcessVariables_IHC_Meddis2000(data)) {
		NotifyError("%s: Could not initialise the process variables.",
		  funcName);
		return(FALSE);
	}

	dt = data->outSignal->dt;
	hC = hairCell2Ptr;		/* Shorter name for use with long equations. */
	ydt = hC->replenishRate_y * dt;
	rdt = hC->recoveryRate_r * dt;
	xdt = hC->reprocessRate_x * dt;
	zdt = hC->perm_z * dt;
	k0pow = pow( hC->perm_Ca0, hC->pCa );	
	l_Plus_rdt = (hC->lossRate_l + hC->recoveryRate_r) * dt;

	/* Set up debug/diagnostic mode */
	debug = (hairCell2Ptr->diagMode !=GENERAL_DIAGNOSTIC_OFF_MODE);
	if (debug) {
		fprintf(hairCell2Ptr->fp, "Time(s)\tVm (V)\tactCa (-)\tICa (A)\tconcCa "
		  "(-)\tkdt (P)\tQ \tC \tW \tEjected");
		fprintf(hairCell2Ptr->fp, "\n");
	}

	/*** Put your code here: process output signal. ***/
	/*** (using 'inPtr' and 'outPtr' for each channel?) ***/

	for (i = 0, timer = DBL_MAX; i < data->outSignal->numChannels; i++) {
		inPtr = data->inSignal[0]->channel[i];
		for (j = 0, outPtr = data->outSignal->channel[i]; j <
		  data->outSignal->length; j++, outPtr++) {

			/*** Calcium controlled transmitter release function ***/
			Vin = *inPtr;

			/* Ca current */
			ssactCa = 	1/( 1 + exp(-hC->gammaCa*Vin)/hC->betaCa );		
	
			hC->hCChannels[i].actCa += (ssactCa - hC->hCChannels[i].actCa) *
			  dt / hC->tauCaChan; 
			ICa = hC->GCaMax * pow(hC->hCChannels[i].actCa, 3) * (Vin -
			  hC->CaVrev);
			
			/* Calcium Ion accumulation and diffusion */
			hC->hCChannels[i].concCa += (-ICa - hC->hCChannels[i].concCa) *
			  dt / hC->tauConcCa;			

			/* power law release function */
			kdt = ( hC->hCChannels[i].concCa > hC->perm_Ca0 ) ? (zdt * (pow(
			  hC->hCChannels[i].concCa, hC->pCa) - k0pow)): 0; 

			/* Increment input pointer */
			inPtr++; 

 			/* end of new transmitter release function */

			/*** Synapse ***/

			switch(hairCell2Ptr->opMode) {
				/* spike output mode */
				case IHC_MEDDIS2000_OPMODE_SPIKE:
					replenish = (hC->hCChannels[i].reservoirQ <
					  hC->maxFreePool_M)? GeomDist_Random(ydt, (int) (
					  hC->maxFreePool_M - hC->hCChannels[i].reservoirQ)): 0;

					ejected = GeomDist_Random(kdt, (int) hC->hCChannels[
					  i].reservoirQ);	
		
					reUptakeAndLost = l_Plus_rdt * hC->hCChannels[i].cleftC;
					reUptake = rdt * hC->hCChannels[i].cleftC;
					reprocessed = (hC->hCChannels[i].reprocessedW < 1.0)? 0:
					GeomDist_Random(xdt, (int) floor(hC->hCChannels[
					  i].reprocessedW));

					hC->hCChannels[i].reservoirQ += replenish - ejected +
					  reprocessed;
					*outPtr = hC->hCChannels[i].cleftC += ejected -
					  reUptakeAndLost;

					if (ejected > 0)
						*outPtr = ejected; 
					else
						*outPtr = 0.0;
			
					hC->hCChannels[i].reprocessedW += reUptake - reprocessed;
					break;

				/* probability output mode */
				case IHC_MEDDIS2000_OPMODE_PROB:
					replenish = (hC->hCChannels[i].reservoirQ <
					  hC->maxFreePool_M)? ydt * (hC->maxFreePool_M -
					  hC->hCChannels[i].reservoirQ): 0.0;
		
					ejected = kdt * hC->hCChannels[i].reservoirQ;
		
					reUptakeAndLost = l_Plus_rdt * hC->hCChannels[i].cleftC;
					reUptake = rdt * hC->hCChannels[i].cleftC;
		
					reprocessed = xdt * hC->hCChannels[i].reprocessedW;	
					hC->hCChannels[i].reservoirQ += replenish - ejected +
					  reprocessed;
					hC->hCChannels[i].cleftC += ejected - reUptakeAndLost;

					*outPtr = ejected; 
			
					hC->hCChannels[i].reprocessedW += reUptake - reprocessed;
					break;

				/* neither mode set. error and exit */
				default: 
					NotifyError("%s: Illegal output mode.", funcName);
					exit(0);
				}

			/* diagnostic mode output */
			if (debug) {
				fprintf(hairCell2Ptr->fp,
				  "%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g", j * dt, Vin,
				  hC->hCChannels[i].actCa, ICa, hC->hCChannels[i].concCa, kdt,
				  hC->hCChannels[i].reservoirQ, hC->hCChannels[i].cleftC,
				  hC->hCChannels[i].reprocessedW, ejected);
				fprintf(hairCell2Ptr->fp, "\n");
			}		 

		}
	}

	SetProcessContinuity_EarObject(data);
	return(TRUE);

}
