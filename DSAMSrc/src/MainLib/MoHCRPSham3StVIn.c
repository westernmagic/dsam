/**********************
 *
 * File:		MoHCRPSham3StVIn.c
 * Purpose:		This module contains a revised model for the Shamme hair cell
 *				receptor potential: Shamm, S. A. Chadwick R. S. Wilbur W. J.
 *				Morrish K. A. and Rinzel J.(1986) "A biophysical model of
 *				cochlear processing: Intensity dependence of pure tone
 *				responses", J. Acoust. Soc. Am. [80], pp 133-145.
 * Comments:	Written using ModuleProducer version 1.3.2 (Mar 27 2001).
 * Author:		C. J. Sumner
 * Created:		20 Aug 2001
 * Updated:	
 * Copyright:	(c) 2001, CNBH, University of Essex.
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
#include "MoHCRPSham3StVIn.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

Sham3StVInPtr	sham3StVInPtr = NULL;

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
Free_IHCRP_Shamma3StateVelIn(void)
{
	/* static const char	*funcName = "Free_IHCRP_Shamma3StateVelIn";  */

	if (sham3StVInPtr == NULL)
		return(FALSE);
	FreeProcessVariables_IHCRP_Shamma3StateVelIn();
	if (sham3StVInPtr->parList)
		FreeList_UniParMgr(&sham3StVInPtr->parList);
	if (sham3StVInPtr->parSpec == GLOBAL) {
		free(sham3StVInPtr);
		sham3StVInPtr = NULL;
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
Init_IHCRP_Shamma3StateVelIn(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_IHCRP_Shamma3StateVelIn";

	if (parSpec == GLOBAL) {
		if (sham3StVInPtr != NULL)
			Free_IHCRP_Shamma3StateVelIn();
		if ((sham3StVInPtr = (Sham3StVInPtr) malloc(sizeof(Sham3StVIn))) ==
		  NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (sham3StVInPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	sham3StVInPtr->parSpec = parSpec;
	sham3StVInPtr->updateProcessVariablesFlag = TRUE;
	sham3StVInPtr->endocochlearPot_EtFlag = TRUE;
	sham3StVInPtr->reversalPot_EkFlag = TRUE;
	sham3StVInPtr->reversalPotCorrectionFlag = TRUE;
	sham3StVInPtr->totalCapacitance_CFlag = TRUE;
	sham3StVInPtr->restingConductance_G0Flag = TRUE;
	sham3StVInPtr->kConductance_GkFlag = TRUE;
	sham3StVInPtr->maxMConductance_GmaxFlag = TRUE;
	sham3StVInPtr->ciliaTimeConst_tcFlag = TRUE;
	sham3StVInPtr->ciliaCouplingGain_CFlag = TRUE;
	sham3StVInPtr->referencePotFlag = TRUE;
	sham3StVInPtr->sensitivity_s0Flag = TRUE;
	sham3StVInPtr->sensitivity_s1Flag = TRUE;
	sham3StVInPtr->offset_u0Flag = TRUE;
	sham3StVInPtr->offset_u1Flag = TRUE;
	sham3StVInPtr->endocochlearPot_Et = 0.1;
	sham3StVInPtr->reversalPot_Ek = -0.07045;
	sham3StVInPtr->reversalPotCorrection = 0.04;
	sham3StVInPtr->totalCapacitance_C = 6e-12;
	sham3StVInPtr->restingConductance_G0 = 1.974e-09;
	sham3StVInPtr->kConductance_Gk = 1.8e-08;
	sham3StVInPtr->maxMConductance_Gmax = 8e-09;
	sham3StVInPtr->ciliaTimeConst_tc = 0.00213;
	sham3StVInPtr->ciliaCouplingGain_C = 16;
	sham3StVInPtr->referencePot = 0.0;
	sham3StVInPtr->sensitivity_s0 = 8.5e-08;
	sham3StVInPtr->sensitivity_s1 = 5e-09;
	sham3StVInPtr->offset_u0 = 7e-09;
	sham3StVInPtr->offset_u1 = 7e-09;

	if (!SetUniParList_IHCRP_Shamma3StateVelIn()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_IHCRP_Shamma3StateVelIn();
		return(FALSE);
	}
	sham3StVInPtr->lastCiliaDisplacement_u = NULL;
	sham3StVInPtr->lastInput = NULL;
	sham3StVInPtr->lastOutput = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_IHCRP_Shamma3StateVelIn(void)
{
	static const char *funcName = "SetUniParList_IHCRP_Shamma3StateVelIn";
	UniParPtr	pars;

	if ((sham3StVInPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  IHCRP_SHAMMA3STATEVELIN_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = sham3StVInPtr->parList->pars;
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA3STATEVELIN_ENDOCOCHLEARPOT_ET], "E_T",
	  "Endocochlear potential, Et (V).",
	  UNIPAR_REAL,
	  &sham3StVInPtr->endocochlearPot_Et, NULL,
	  (void * (*)) SetEndocochlearPot_Et_IHCRP_Shamma3StateVelIn);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA3STATEVELIN_REVERSALPOT_EK], "E_K",
	  "Reversal potential, Ek (V).",
	  UNIPAR_REAL,
	  &sham3StVInPtr->reversalPot_Ek, NULL,
	  (void * (*)) SetReversalPot_Ek_IHCRP_Shamma3StateVelIn);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA3STATEVELIN_REVERSALPOTCORRECTION],
	  "RP_CORRECTION",
	  "Reversal potential correction, Rp/(Rt+Rp).",
	  UNIPAR_REAL,
	  &sham3StVInPtr->reversalPotCorrection, NULL,
	  (void * (*)) SetReversalPotCorrection_IHCRP_Shamma3StateVelIn);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA3STATEVELIN_TOTALCAPACITANCE_C],
	  "C_TOTAL",
	  "Total capacitance, C = Ca + Cb (F).",
	  UNIPAR_REAL,
	  &sham3StVInPtr->totalCapacitance_C, NULL,
	  (void * (*)) SetTotalCapacitance_C_IHCRP_Shamma3StateVelIn);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA3STATEVELIN_RESTINGCONDUCTANCE_G0], "G0",
	  "Resting conductance, G0 (S).",
	  UNIPAR_REAL,
	  &sham3StVInPtr->restingConductance_G0, NULL,
	  (void * (*)) SetRestingConductance_G0_IHCRP_Shamma3StateVelIn);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA3STATEVELIN_KCONDUCTANCE_GK], "G_K",
	  "Potassium conductance, Gk (S = Siemens).",
	  UNIPAR_REAL,
	  &sham3StVInPtr->kConductance_Gk, NULL,
	  (void * (*)) SetKConductance_Gk_IHCRP_Shamma3StateVelIn);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA3STATEVELIN_MAXMCONDUCTANCE_GMAX],
	  "G_MAXC",
	  "Maximum mechanical conductance, Gmax (S).",
	  UNIPAR_REAL,
	  &sham3StVInPtr->maxMConductance_Gmax, NULL,
	  (void * (*)) SetMaxMConductance_Gmax_IHCRP_Shamma3StateVelIn);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA3STATEVELIN_CILIATIMECONST_TC], "T_C",
	  "Cilia/BM time constant (s).",
	  UNIPAR_REAL,
	  &sham3StVInPtr->ciliaTimeConst_tc, NULL,
	  (void * (*)) SetCiliaTimeConst_tc_IHCRP_Shamma3StateVelIn);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA3STATEVELIN_CILIACOUPLINGGAIN_C],
	  "GAIN_C",
	  "Cilia/BM coupling gain, C (dB).",
	  UNIPAR_REAL,
	  &sham3StVInPtr->ciliaCouplingGain_C, NULL,
	  (void * (*)) SetCiliaCouplingGain_C_IHCRP_Shamma3StateVelIn);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA3STATEVELIN_REFERENCEPOT], "REF_POT",
	  "Reference potential (V).",
	  UNIPAR_REAL,
	  &sham3StVInPtr->referencePot, NULL,
	  (void * (*)) SetReferencePot_IHCRP_Shamma3StateVelIn);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA3STATEVELIN_SENSITIVITY_S0], "S0",
	  "Sensitivity constant, S0 (/m).",
	  UNIPAR_REAL,
	  &sham3StVInPtr->sensitivity_s0, NULL,
	  (void * (*)) SetSensitivity_s0_IHCRP_Shamma3StateVelIn);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA3STATEVELIN_SENSITIVITY_S1], "S1",
	  "Sensitivity constant, S1 (/m).",
	  UNIPAR_REAL,
	  &sham3StVInPtr->sensitivity_s1, NULL,
	  (void * (*)) SetSensitivity_s1_IHCRP_Shamma3StateVelIn);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA3STATEVELIN_OFFSET_U0], "U0",
	  "Offset constant, U0 (m).",
	  UNIPAR_REAL,
	  &sham3StVInPtr->offset_u0, NULL,
	  (void * (*)) SetOffset_u0_IHCRP_Shamma3StateVelIn);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA3STATEVELIN_OFFSET_U1], "U1",
	  "Offset constant, U1 (m).",
	  UNIPAR_REAL,
	  &sham3StVInPtr->offset_u1, NULL,
	  (void * (*)) SetOffset_u1_IHCRP_Shamma3StateVelIn);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_IHCRP_Shamma3StateVelIn(void)
{
	static const char	*funcName = "GetUniParListPtr_IHCRP_Shamma3StateVelIn";

	if (sham3StVInPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (sham3StVInPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(sham3StVInPtr->parList);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_IHCRP_Shamma3StateVelIn(double endocochlearPot_Et,
  double reversalPot_Ek, double reversalPotCorrection,
  double totalCapacitance_C, double restingConductance_G0,
  double kConductance_Gk, double maxMConductance_Gmax,
  double ciliaTimeConst_tc, double ciliaCouplingGain_C, double referencePot,
  double sensitivity_s0, double sensitivity_s1, double offset_u0,
  double offset_u1)
{
	static const char	*funcName = "SetPars_IHCRP_Shamma3StateVelIn";
	BOOLN	ok;

	ok = TRUE;
	if (!SetEndocochlearPot_Et_IHCRP_Shamma3StateVelIn(endocochlearPot_Et))
		ok = FALSE;
	if (!SetReversalPot_Ek_IHCRP_Shamma3StateVelIn(reversalPot_Ek))
		ok = FALSE;
	if (!SetReversalPotCorrection_IHCRP_Shamma3StateVelIn(
	  reversalPotCorrection))
		ok = FALSE;
	if (!SetTotalCapacitance_C_IHCRP_Shamma3StateVelIn(totalCapacitance_C))
		ok = FALSE;
	if (!SetRestingConductance_G0_IHCRP_Shamma3StateVelIn(
	  restingConductance_G0))
		ok = FALSE;
	if (!SetKConductance_Gk_IHCRP_Shamma3StateVelIn(kConductance_Gk))
		ok = FALSE;
	if (!SetMaxMConductance_Gmax_IHCRP_Shamma3StateVelIn(maxMConductance_Gmax))
		ok = FALSE;
	if (!SetCiliaTimeConst_tc_IHCRP_Shamma3StateVelIn(ciliaTimeConst_tc))
		ok = FALSE;
	if (!SetCiliaCouplingGain_C_IHCRP_Shamma3StateVelIn(ciliaCouplingGain_C))
		ok = FALSE;
	if (!SetReferencePot_IHCRP_Shamma3StateVelIn(referencePot))
		ok = FALSE;
	if (!SetSensitivity_s0_IHCRP_Shamma3StateVelIn(sensitivity_s0))
		ok = FALSE;
	if (!SetSensitivity_s1_IHCRP_Shamma3StateVelIn(sensitivity_s1))
		ok = FALSE;
	if (!SetOffset_u0_IHCRP_Shamma3StateVelIn(offset_u0))
		ok = FALSE;
	if (!SetOffset_u1_IHCRP_Shamma3StateVelIn(offset_u1))
		ok = FALSE;
	if (!ok)
		NotifyError("%s: Failed to set all module parameters." ,funcName);
	return(ok);

}

/****************************** SetEndocochlearPot_Et *************************/

/*
 * This function sets the module's endocochlearPot_Et parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetEndocochlearPot_Et_IHCRP_Shamma3StateVelIn(double theEndocochlearPot_Et)
{
	static const char	*funcName =
	  "SetEndocochlearPot_Et_IHCRP_Shamma3StateVelIn";

	if (sham3StVInPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sham3StVInPtr->endocochlearPot_EtFlag = TRUE;
	sham3StVInPtr->updateProcessVariablesFlag = TRUE;
	sham3StVInPtr->endocochlearPot_Et = theEndocochlearPot_Et;
	return(TRUE);

}

/****************************** SetReversalPot_Ek *****************************/

/*
 * This function sets the module's reversalPot_Ek parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetReversalPot_Ek_IHCRP_Shamma3StateVelIn(double theReversalPot_Ek)
{
	static const char	*funcName =
	  "SetReversalPot_Ek_IHCRP_Shamma3StateVelIn";

	if (sham3StVInPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sham3StVInPtr->reversalPot_EkFlag = TRUE;
	sham3StVInPtr->updateProcessVariablesFlag = TRUE;
	sham3StVInPtr->reversalPot_Ek = theReversalPot_Ek;
	return(TRUE);

}

/****************************** SetReversalPotCorrection **********************/

/*
 * This function sets the module's reversalPotCorrection parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetReversalPotCorrection_IHCRP_Shamma3StateVelIn(double
  theReversalPotCorrection)
{
	static const char	*funcName =
	  "SetReversalPotCorrection_IHCRP_Shamma3StateVelIn";

	if (sham3StVInPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sham3StVInPtr->reversalPotCorrectionFlag = TRUE;
	sham3StVInPtr->updateProcessVariablesFlag = TRUE;
	sham3StVInPtr->reversalPotCorrection = theReversalPotCorrection;
	return(TRUE);

}

/****************************** SetTotalCapacitance_C *************************/

/*
 * This function sets the module's totalCapacitance_C parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTotalCapacitance_C_IHCRP_Shamma3StateVelIn(double theTotalCapacitance_C)
{
	static const char	*funcName =
	  "SetTotalCapacitance_C_IHCRP_Shamma3StateVelIn";

	if (sham3StVInPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sham3StVInPtr->totalCapacitance_CFlag = TRUE;
	sham3StVInPtr->updateProcessVariablesFlag = TRUE;
	sham3StVInPtr->totalCapacitance_C = theTotalCapacitance_C;
	return(TRUE);

}

/****************************** SetRestingConductance_G0 **********************/

/*
 * This function sets the module's restingConductance_G0 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRestingConductance_G0_IHCRP_Shamma3StateVelIn(double
  theRestingConductance_G0)
{
	static const char	*funcName =
	  "SetRestingConductance_G0_IHCRP_Shamma3StateVelIn";

	if (sham3StVInPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sham3StVInPtr->restingConductance_G0Flag = TRUE;
	sham3StVInPtr->updateProcessVariablesFlag = TRUE;
	sham3StVInPtr->restingConductance_G0 = theRestingConductance_G0;
	return(TRUE);

}

/****************************** SetKConductance_Gk ****************************/

/*
 * This function sets the module's kConductance_Gk parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetKConductance_Gk_IHCRP_Shamma3StateVelIn(double theKConductance_Gk)
{
	static const char	*funcName =
	  "SetKConductance_Gk_IHCRP_Shamma3StateVelIn";

	if (sham3StVInPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sham3StVInPtr->kConductance_GkFlag = TRUE;
	sham3StVInPtr->updateProcessVariablesFlag = TRUE;
	sham3StVInPtr->kConductance_Gk = theKConductance_Gk;
	return(TRUE);

}

/****************************** SetMaxMConductance_Gmax ***********************/

/*
 * This function sets the module's maxMConductance_Gmax parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMaxMConductance_Gmax_IHCRP_Shamma3StateVelIn(double theMaxMConductance_Gmax)
{
	static const char	*funcName =
	  "SetMaxMConductance_Gmax_IHCRP_Shamma3StateVelIn";

	if (sham3StVInPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sham3StVInPtr->maxMConductance_GmaxFlag = TRUE;
	sham3StVInPtr->updateProcessVariablesFlag = TRUE;
	sham3StVInPtr->maxMConductance_Gmax = theMaxMConductance_Gmax;
	return(TRUE);

}

/****************************** SetCiliaTimeConst_tc **************************/

/*
 * This function sets the module's ciliaTimeConst_tc parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCiliaTimeConst_tc_IHCRP_Shamma3StateVelIn(double theCiliaTimeConst_tc)
{
	static const char	*funcName =
	  "SetCiliaTimeConst_tc_IHCRP_Shamma3StateVelIn";

	if (sham3StVInPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sham3StVInPtr->ciliaTimeConst_tcFlag = TRUE;
	sham3StVInPtr->updateProcessVariablesFlag = TRUE;
	sham3StVInPtr->ciliaTimeConst_tc = theCiliaTimeConst_tc;
	return(TRUE);

}

/****************************** SetCiliaCouplingGain_C ************************/

/*
 * This function sets the module's ciliaCouplingGain_C parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCiliaCouplingGain_C_IHCRP_Shamma3StateVelIn(double theCiliaCouplingGain_C)
{
	static const char	*funcName =
	  "SetCiliaCouplingGain_C_IHCRP_Shamma3StateVelIn";

	if (sham3StVInPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sham3StVInPtr->ciliaCouplingGain_CFlag = TRUE;
	sham3StVInPtr->updateProcessVariablesFlag = TRUE;
	sham3StVInPtr->ciliaCouplingGain_C = theCiliaCouplingGain_C;
	return(TRUE);

}

/****************************** SetReferencePot *******************************/

/*
 * This function sets the module's referencePot parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetReferencePot_IHCRP_Shamma3StateVelIn(double theReferencePot)
{
	static const char	*funcName = "SetReferencePot_IHCRP_Shamma3StateVelIn";

	if (sham3StVInPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sham3StVInPtr->referencePotFlag = TRUE;
	sham3StVInPtr->updateProcessVariablesFlag = TRUE;
	sham3StVInPtr->referencePot = theReferencePot;
	return(TRUE);

}

/****************************** SetSensitivity_s0 *****************************/

/*
 * This function sets the module's sensitivity_s0 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSensitivity_s0_IHCRP_Shamma3StateVelIn(double theSensitivity_s0)
{
	static const char	*funcName =
	  "SetSensitivity_s0_IHCRP_Shamma3StateVelIn";

	if (sham3StVInPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sham3StVInPtr->sensitivity_s0Flag = TRUE;
	sham3StVInPtr->updateProcessVariablesFlag = TRUE;
	sham3StVInPtr->sensitivity_s0 = theSensitivity_s0;
	return(TRUE);

}

/****************************** SetSensitivity_s1 *****************************/

/*
 * This function sets the module's sensitivity_s1 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSensitivity_s1_IHCRP_Shamma3StateVelIn(double theSensitivity_s1)
{
	static const char	*funcName =
	  "SetSensitivity_s1_IHCRP_Shamma3StateVelIn";

	if (sham3StVInPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sham3StVInPtr->sensitivity_s1Flag = TRUE;
	sham3StVInPtr->updateProcessVariablesFlag = TRUE;
	sham3StVInPtr->sensitivity_s1 = theSensitivity_s1;
	return(TRUE);

}

/****************************** SetOffset_u0 **********************************/

/*
 * This function sets the module's offset_u0 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOffset_u0_IHCRP_Shamma3StateVelIn(double theOffset_u0)
{
	static const char	*funcName = "SetOffset_u0_IHCRP_Shamma3StateVelIn";

	if (sham3StVInPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sham3StVInPtr->offset_u0Flag = TRUE;
	sham3StVInPtr->updateProcessVariablesFlag = TRUE;
	sham3StVInPtr->offset_u0 = theOffset_u0;
	return(TRUE);

}

/****************************** SetOffset_u1 **********************************/

/*
 * This function sets the module's offset_u1 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOffset_u1_IHCRP_Shamma3StateVelIn(double theOffset_u1)
{
	static const char	*funcName = "SetOffset_u1_IHCRP_Shamma3StateVelIn";

	if (sham3StVInPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sham3StVInPtr->offset_u1Flag = TRUE;
	sham3StVInPtr->updateProcessVariablesFlag = TRUE;
	sham3StVInPtr->offset_u1 = theOffset_u1;
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
CheckPars_IHCRP_Shamma3StateVelIn(void)
{
	static const char	*funcName = "CheckPars_IHCRP_Shamma3StateVelIn";
	BOOLN	ok;

	ok = TRUE;
	if (sham3StVInPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!sham3StVInPtr->endocochlearPot_EtFlag) {
		NotifyError("%s: endocochlearPot_Et variable not set.", funcName);
		ok = FALSE;
	}
	if (!sham3StVInPtr->reversalPot_EkFlag) {
		NotifyError("%s: reversalPot_Ek variable not set.", funcName);
		ok = FALSE;
	}
	if (!sham3StVInPtr->reversalPotCorrectionFlag) {
		NotifyError("%s: reversalPotCorrection variable not set.", funcName);
		ok = FALSE;
	}
	if (!sham3StVInPtr->totalCapacitance_CFlag) {
		NotifyError("%s: totalCapacitance_C variable not set.", funcName);
		ok = FALSE;
	}
	if (!sham3StVInPtr->restingConductance_G0Flag) {
		NotifyError("%s: restingConductance_G0 variable not set.", funcName);
		ok = FALSE;
	}
	if (!sham3StVInPtr->kConductance_GkFlag) {
		NotifyError("%s: kConductance_Gk variable not set.", funcName);
		ok = FALSE;
	}
	if (!sham3StVInPtr->maxMConductance_GmaxFlag) {
		NotifyError("%s: maxMConductance_Gmax variable not set.", funcName);
		ok = FALSE;
	}
	if (!sham3StVInPtr->ciliaTimeConst_tcFlag) {
		NotifyError("%s: ciliaTimeConst_tc variable not set.", funcName);
		ok = FALSE;
	}
	if (!sham3StVInPtr->ciliaCouplingGain_CFlag) {
		NotifyError("%s: ciliaCouplingGain_C variable not set.", funcName);
		ok = FALSE;
	}
	if (!sham3StVInPtr->referencePotFlag) {
		NotifyError("%s: referencePot variable not set.", funcName);
		ok = FALSE;
	}
	if (!sham3StVInPtr->sensitivity_s0Flag) {
		NotifyError("%s: sensitivity_s0 variable not set.", funcName);
		ok = FALSE;
	}
	if (!sham3StVInPtr->sensitivity_s1Flag) {
		NotifyError("%s: sensitivity_s1 variable not set.", funcName);
		ok = FALSE;
	}
	if (!sham3StVInPtr->offset_u0Flag) {
		NotifyError("%s: offset_u0 variable not set.", funcName);
		ok = FALSE;
	}
	if (!sham3StVInPtr->offset_u1Flag) {
		NotifyError("%s: offset_u1 variable not set.", funcName);
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
PrintPars_IHCRP_Shamma3StateVelIn(void)
{
	static const char	*funcName = "PrintPars_IHCRP_Shamma3StateVelIn";

	if (!CheckPars_IHCRP_Shamma3StateVelIn()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Modified Shamma Receptor Potential Module  Module Parameters:-\n");
	DPrint("\tEndocochlear potential, Et = %g V,\n", sham3StVInPtr->
	  endocochlearPot_Et);
	DPrint( "\tReversal potential, Ek = %g V,\n", sham3StVInPtr->
	  reversalPot_Ek);
	DPrint("\tReversal potential correction Rp/(Rt+Rp) = %g,\n", sham3StVInPtr->
	  reversalPotCorrection);
	DPrint("\tTotal capacitance, Cm = %g F,\n", sham3StVInPtr->
	  totalCapacitance_C);
	DPrint("\tResting conductance, G0 = %g S,\n", sham3StVInPtr->
	  restingConductance_G0);
	DPrint("\tPotassium conductance, Gk = %g S,\n", sham3StVInPtr->
	  kConductance_Gk);
	DPrint("\tMaximum mechanically sensitive Conductance, Gmax = %g S,\n",
	  sham3StVInPtr->maxMConductance_Gmax);
	DPrint("\tBM/Cilia: time constant = %g ms,", sham3StVInPtr->
	  ciliaTimeConst_tc);
	DPrint("\tGain = %g dB,\n", sham3StVInPtr->ciliaCouplingGain_C);
	DPrint("\tReference potential = %g V,\n", sham3StVInPtr->referencePot);
	DPrint("\tTransduction function 0:\n\t\tSensitivity, s0 = %g (/m), ", 
	  sham3StVInPtr->sensitivity_s0);
	DPrint("\tOffset, u0 = %g (m)\n", sham3StVInPtr->offset_u0  );
	DPrint("\tTransduction function 1:\n\t\tSensitivity, s1 = %g (/m),",
	  sham3StVInPtr->sensitivity_s1);
	DPrint("\tOffset, u1 = %g (m)\n", sham3StVInPtr->offset_u1 );
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_IHCRP_Shamma3StateVelIn(char *fileName)
{
	static const char	*funcName = "ReadPars_IHCRP_Shamma3StateVelIn";
	BOOLN	ok = TRUE;
	char	*filePath;
	double	endocochlearPot_Et, reversalPot_Ek, reversalPotCorrection;
	double	totalCapacitance_C, restingConductance_G0, kConductance_Gk;
	double	maxMConductance_Gmax, ciliaTimeConst_tc, ciliaCouplingGain_C;
	double	referencePot, sensitivity_s0, sensitivity_s1, offset_u0, offset_u1;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(filePath, "r")) == NULL) {
		NotifyError("%s: Cannot open data file '%s'.\n", funcName, fileName);
		return(FALSE);
	}
	DPrint("%s: Reading from '%s':\n", funcName, fileName);
	Init_ParFile();
	if (!GetPars_ParFile(fp, "%lf", &endocochlearPot_Et))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &reversalPot_Ek))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &reversalPotCorrection))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &totalCapacitance_C))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &restingConductance_G0))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &kConductance_Gk))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &maxMConductance_Gmax))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &ciliaTimeConst_tc))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &ciliaCouplingGain_C))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &referencePot))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &sensitivity_s0))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &offset_u0))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &sensitivity_s1))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &offset_u1))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "
		  "parameter file '%s'.", funcName, fileName);
		return(FALSE);
	}
	if (!SetPars_IHCRP_Shamma3StateVelIn(endocochlearPot_Et, reversalPot_Ek,
	  reversalPotCorrection, totalCapacitance_C, restingConductance_G0,
	  kConductance_Gk, maxMConductance_Gmax, ciliaTimeConst_tc,
	  ciliaCouplingGain_C, referencePot, sensitivity_s0, sensitivity_s1,
	  offset_u0, offset_u1)) {
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
SetParsPointer_IHCRP_Shamma3StateVelIn(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_IHCRP_Shamma3StateVelIn";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	sham3StVInPtr = (Sham3StVInPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 * It also initialises the process structure.
 */

BOOLN
InitModule_IHCRP_Shamma3StateVelIn(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_IHCRP_Shamma3StateVelIn";

	if (!SetParsPointer_IHCRP_Shamma3StateVelIn(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_IHCRP_Shamma3StateVelIn(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = sham3StVInPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->CheckPars = CheckPars_IHCRP_Shamma3StateVelIn;
	theModule->Free = Free_IHCRP_Shamma3StateVelIn;
	theModule->GetUniParListPtr = GetUniParListPtr_IHCRP_Shamma3StateVelIn;
	theModule->PrintPars = PrintPars_IHCRP_Shamma3StateVelIn;
	theModule->ReadPars = ReadPars_IHCRP_Shamma3StateVelIn;
	theModule->RunProcess = RunModel_IHCRP_Shamma3StateVelIn;
	theModule->SetParsPointer = SetParsPointer_IHCRP_Shamma3StateVelIn;
	return(TRUE);

}

/****************************** CheckData *************************************/

/*
 * This routine checks that the 'data' EarObject and input signal are
 * correctly initialised.
 * It should also include checks that ensure that the module's
 * parameters are compatible with the signal parameters, i.e. dt is
 * not too small, etc...
 */

BOOLN
CheckData_IHCRP_Shamma3StateVelIn(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_IHCRP_Shamma3StateVelIn";

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
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
InitProcessVariables_IHCRP_Shamma3StateVelIn(EarObjectPtr data)
{
	static const char	*funcName =
	  "InitProcessVariables_IHCRP_Shamma3StateVelIn";
	int		i;
	double	restingPotential_V0;
	Sham3StVInPtr p = sham3StVInPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag ||
	  (data->timeIndex == PROCESS_START_TIME)) {
		if (p->updateProcessVariablesFlag || data->updateProcessFlag) {

			FreeProcessVariables_IHCRP_Shamma3StateVelIn();
			if ((p->lastInput = (double *) calloc(data->outSignal->numChannels,
			  sizeof(double))) == NULL) {
				NotifyError("%s: Out of memory for 'lastInput'.", funcName);
				return(FALSE);
			}
			if ((p->lastOutput = (double *) calloc(data->outSignal->numChannels,
			  sizeof(double))) == NULL) {
				NotifyError("%s: Out of memory for 'lastOutput'.", funcName);
				return(FALSE);
			}
			if ((p->lastCiliaDisplacement_u = (double *)
				calloc(data->outSignal->numChannels, sizeof(double))) == NULL) {
				NotifyError("%s: Out of memory for 'lastCiliaDisplacement_u'.",
				  funcName);
				return(FALSE);
			}
			p->updateProcessVariablesFlag = FALSE;
		}

		restingPotential_V0 = (p->restingConductance_G0 *
		  p->endocochlearPot_Et + p->kConductance_Gk * (p->reversalPot_Ek +
		  p->endocochlearPot_Et * p->reversalPotCorrection)) /
		  (p->restingConductance_G0 + p->kConductance_Gk);

		if (data->timeIndex == PROCESS_START_TIME) {
			for (i = 0; i < data->outSignal->numChannels; i++) {
				p->lastInput[i] = 0.0;
				p->lastOutput[i] = restingPotential_V0;
				p->lastCiliaDisplacement_u[i] = 0.0;
			}	
		}
	}
	return(TRUE);

}

/****************************** FreeProcessVariables **************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

BOOLN
FreeProcessVariables_IHCRP_Shamma3StateVelIn(void)
{
	if (sham3StVInPtr->lastInput != NULL) {
		free(sham3StVInPtr->lastInput);
		sham3StVInPtr->lastInput = NULL;
	}
	if (sham3StVInPtr->lastOutput != NULL) {
		free(sham3StVInPtr->lastOutput);
		sham3StVInPtr->lastOutput = NULL;
	}
	if (sham3StVInPtr->lastCiliaDisplacement_u != NULL) {
		free(sham3StVInPtr->lastCiliaDisplacement_u);
		sham3StVInPtr->lastCiliaDisplacement_u = NULL;
	}
	sham3StVInPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

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
 */

BOOLN
RunModel_IHCRP_Shamma3StateVelIn(EarObjectPtr data)
{
	static const char	*funcName = "RunModel_IHCRP_Shamma3StateVelIn";
	register ChanData	 *inPtr, *outPtr;
	int	chan;
	ChanLen	i;
	double	conductance_G, potential_V;
	double	ciliaDisplacement_u, lastInput;
	Sham3StVInPtr p = sham3StVInPtr;

	if (!data->threadRunFlag) {
		if (!CheckPars_IHCRP_Shamma3StateVelIn()) {
			NotifyError("%s: Parameters invalid.", funcName);
			return(FALSE);
			}
		if (!CheckData_IHCRP_Shamma3StateVelIn(data)) {
			NotifyError("%s: Process data invalid.", funcName);
			return(FALSE);
		}
		if (!CheckRamp_SignalData(data->inSignal[0])) {
			NotifyError("%s: Input signal not correctly initialised.",
			  funcName);
			return(FALSE);
		}
 		SetProcessName_EarObject(data, "Modified Shamma hair cell receptor "
		  "potential");

		if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels,
		  data->inSignal[0]->length, data->inSignal[0]->dt)) {
			NotifyError("%s: Cannot initialise output channels.", funcName);
			return(FALSE);
		}

		if (!InitProcessVariables_IHCRP_Shamma3StateVelIn(data)) {
			NotifyError("%s: Could not initialise the process variables.",
			  funcName);
			return(FALSE);
		}

		p->dtOverC = data->outSignal->dt / p->totalCapacitance_C;
		p->gkEpk = p->kConductance_Gk * (p->reversalPot_Ek +
		  p->endocochlearPot_Et * p->reversalPotCorrection);
		p->ciliaAct = 1.0 / (1.0 + exp(p->offset_u0 / p->sensitivity_s0) *
		  (1.0 + exp(p->offset_u1 / p->sensitivity_s1)));
		p->leakageConductance_Ga = p->restingConductance_G0 -
		  p->maxMConductance_Gmax * p->ciliaAct;

		p->cGaindt = pow(10.0, p->ciliaCouplingGain_C / 20.0) * data->
		  outSignal->dt;
		p->dtOverTc = data->outSignal->dt / p->ciliaTimeConst_tc;
		if (data->initThreadRunFlag)
			return(TRUE);
	}

	for (chan = data->outSignal->offset; chan < data->outSignal->numChannels;
	  chan++) {
		inPtr = data->inSignal[0]->channel[chan];
		outPtr = data->outSignal->channel[chan];
		ciliaDisplacement_u = p->lastCiliaDisplacement_u[chan];
		potential_V = p->lastOutput[chan];
		lastInput = p->lastInput[chan];
		for (i = 0; i < data->outSignal->length; i++, inPtr++, outPtr++) {
			ciliaDisplacement_u += p->cGaindt * (*inPtr ) - 
			  ciliaDisplacement_u * p->dtOverTc;
			p->ciliaAct = 1.0 / (1.0 + exp((p->offset_u0 -
			  ciliaDisplacement_u) / p->sensitivity_s0) * (1.0 + exp((
			  p->offset_u1 - ciliaDisplacement_u) / p->sensitivity_s1)));
			conductance_G = p->maxMConductance_Gmax * p->ciliaAct +
			  p->leakageConductance_Ga; 
			*outPtr = (ChanData) (potential_V - p->dtOverC * (conductance_G *
			  (potential_V - p->endocochlearPot_Et) + p->kConductance_Gk *
			  potential_V - p->gkEpk));
			potential_V = *outPtr;
			lastInput = *inPtr;
		}
		p->lastCiliaDisplacement_u[chan] = ciliaDisplacement_u;
		p->lastInput[chan] = lastInput;
		p->lastOutput[chan] = potential_V;
		outPtr = data->outSignal->channel[chan];
		for (i = 0; i < data->outSignal->length; i++)
			*outPtr++ += p->referencePot;
	}

	SetProcessContinuity_EarObject(data);
	return(TRUE);

}
