/**********************
 *
 * File:		MoBMZhang.c
 * Purpose:		Incorporates the Zhang et al. BM code
 * Comments:	Written using ModuleProducer version 1.4.0 (Mar 14 2002).
 *				This code was revised from the ARLO matlab code.
 * Author:		Revised by L. P. O'Mard
 * Created:		13 Jun 2002
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
#include "UtBandwidth.h"
#include "UtCFList.h"
#include "UtCmplxM.h"
#include "UtZhang.h"
#include "MoBMZhang.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

BMZhangPtr	bMZhangPtr = NULL;

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
Free_BasilarM_Zhang(void)
{
	/* static const char	*funcName = "Free_BasilarM_Zhang";  */

	if (bMZhangPtr == NULL)
		return(FALSE);
	FreeProcessVariables_BasilarM_Zhang();
	Free_CFList(&bMZhangPtr->cFList);
	if (bMZhangPtr->parList)
		FreeList_UniParMgr(&bMZhangPtr->parList);
	if (bMZhangPtr->parSpec == GLOBAL) {
		free(bMZhangPtr);
		bMZhangPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitModelList *********************************/

/*
 * This function initialises the 'model' list array
 */

BOOLN
InitModelList_BasilarM_Zhang(void)
{
	static NameSpecifier	modeList[] = {

		{ "FEED_FORWARD_NL",	BASILARM_ZHANG_MODEL_FEED_FORWARD_NL },
		{ "FEED_BACK_NL",		BASILARM_ZHANG_MODEL_FEED_BACK_NL },
		{ "SHARP_LINEAR",		BASILARM_ZHANG_MODEL_SHARP_LINEAR },
		{ "BROAD_LINEAR",		BASILARM_ZHANG_MODEL_BROAD_LINEAR },
		{ "BROAD_LINEAR_HIGH",	BASILARM_ZHANG_MODEL_BROAD_LINEAR_HIGH },
		{ "",					BASILARM_ZHANG_MODEL_NULL },
	};
	bMZhangPtr->modelList = modeList;
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
Init_BasilarM_Zhang(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_BasilarM_Zhang";

	if (parSpec == GLOBAL) {
		if (bMZhangPtr != NULL)
			Free_BasilarM_Zhang();
		if ((bMZhangPtr = (BMZhangPtr) malloc(sizeof(
		  BMZhang))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (bMZhangPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	bMZhangPtr->parSpec = parSpec;
	bMZhangPtr->updateProcessVariablesFlag = TRUE;
	bMZhangPtr->modelFlag = TRUE;
	bMZhangPtr->speciesFlag = TRUE;
	bMZhangPtr->nbOrderFlag = TRUE;
	bMZhangPtr->wbOrderFlag = TRUE;
	bMZhangPtr->cornerCPFlag = TRUE;
	bMZhangPtr->slopeCPFlag = TRUE;
	bMZhangPtr->strenghCPFlag = TRUE;
	bMZhangPtr->x0CPFlag = TRUE;
	bMZhangPtr->s0CPFlag = TRUE;
	bMZhangPtr->x1CPFlag = TRUE;
	bMZhangPtr->s1CPFlag = TRUE;
	bMZhangPtr->shiftCPFlag = TRUE;
	bMZhangPtr->cutCPFlag = TRUE;
	bMZhangPtr->kCPFlag = TRUE;
	bMZhangPtr->r0Flag = TRUE;
	bMZhangPtr->model = BASILARM_ZHANG_MODEL_FEED_FORWARD_NL;
	bMZhangPtr->species = UT_ZHANG_SPECIES_HUMAN;
	bMZhangPtr->nbOrder = 3;
	bMZhangPtr->wbOrder = 3;
	bMZhangPtr->cornerCP = BASILARM_ZHANG_ABS_DB - 12.0;
	bMZhangPtr->slopeCP = 0.22;
	bMZhangPtr->strenghCP = 0.08;
	bMZhangPtr->x0CP = 5.0;
	bMZhangPtr->s0CP = 12.0;
	bMZhangPtr->x1CP = 5.0;
	bMZhangPtr->s1CP = 5.0;
	bMZhangPtr->shiftCP = 7.0;
	bMZhangPtr->cutCP = 800.0;
	bMZhangPtr->kCP = 3;
	bMZhangPtr->r0 = 0.05;
	bMZhangPtr->cFList = NULL;

	InitModelList_BasilarM_Zhang();
	if (!SetUniParList_BasilarM_Zhang()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_BasilarM_Zhang();
		return(FALSE);
	}
	bMZhangPtr->bM = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_BasilarM_Zhang(void)
{
	static const char *funcName = "SetUniParList_BasilarM_Zhang";
	UniParPtr	pars;

	if ((bMZhangPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  BASILARM_ZHANG_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = bMZhangPtr->parList->pars;
	SetPar_UniParMgr(&pars[BASILARM_ZHANG_MODEL], "MODEL",
	  "Model type ('feed_forward_nl').",
	  UNIPAR_NAME_SPEC,
	  &bMZhangPtr->model, bMZhangPtr->modelList,
	  (void * (*)) SetModel_BasilarM_Zhang);
	SetPar_UniParMgr(&pars[BASILARM_ZHANG_SPECIES], "SPECIES",
	  "Species type ('Human').",
	  UNIPAR_NAME_SPEC,
	  &bMZhangPtr->species, SpeciesList_Utility_Zhang(0),
	  (void * (*)) SetSpecies_BasilarM_Zhang);
	SetPar_UniParMgr(&pars[BASILARM_ZHANG_NBORDER], "N_ORDER",
	  "Order of the narrow bandpass filter (int).",
	  UNIPAR_INT,
	  &bMZhangPtr->nbOrder, NULL,
	  (void * (*)) SetNbOrder_BasilarM_Zhang);
	SetPar_UniParMgr(&pars[BASILARM_ZHANG_WBORDER], "GAMMA_CP",
	  "Order of the wide bandpass filter (int).",
	  UNIPAR_INT,
	  &bMZhangPtr->wbOrder, NULL,
	  (void * (*)) SetWbOrder_BasilarM_Zhang);
	SetPar_UniParMgr(&pars[BASILARM_ZHANG_CORNERCP], "CORNER_CP",
	  "Corner parameter at which the BM starts compression",
	  UNIPAR_REAL,
	  &bMZhangPtr->cornerCP, NULL,
	  (void * (*)) SetCornerCP_BasilarM_Zhang);
	SetPar_UniParMgr(&pars[BASILARM_ZHANG_SLOPECP], "SLOPE_CP",
	  "Slope of compression nonlinearity in Boltzmann combination.",
	  UNIPAR_REAL,
	  &bMZhangPtr->slopeCP, NULL,
	  (void * (*)) SetSlopeCP_BasilarM_Zhang);
	SetPar_UniParMgr(&pars[BASILARM_ZHANG_STRENGHCP], "STRENGTH_CP",
	  "Strength of compression nonlinearity in Boltzmann combination.",
	  UNIPAR_REAL,
	  &bMZhangPtr->strenghCP, NULL,
	  (void * (*)) SetStrenghCP_BasilarM_Zhang);
	SetPar_UniParMgr(&pars[BASILARM_ZHANG_X0CP], "X0_CP",
	  "Parameter in Boltzman function.",
	  UNIPAR_REAL,
	  &bMZhangPtr->x0CP, NULL,
	  (void * (*)) SetX0CP_BasilarM_Zhang);
	SetPar_UniParMgr(&pars[BASILARM_ZHANG_S0CP], "S0_CP",
	  "Parameter in Boltzman function.",
	  UNIPAR_REAL,
	  &bMZhangPtr->s0CP, NULL,
	  (void * (*)) SetS0CP_BasilarM_Zhang);
	SetPar_UniParMgr(&pars[BASILARM_ZHANG_X1CP], "X1_CP",
	  "Parameter in Boltzman function.",
	  UNIPAR_REAL,
	  &bMZhangPtr->x1CP, NULL,
	  (void * (*)) SetX1CP_BasilarM_Zhang);
	SetPar_UniParMgr(&pars[BASILARM_ZHANG_S1CP], "S1_CP",
	  "Parameter in Boltzman function.",
	  UNIPAR_REAL,
	  &bMZhangPtr->s1CP, NULL,
	  (void * (*)) SetS1CP_BasilarM_Zhang);
	SetPar_UniParMgr(&pars[BASILARM_ZHANG_SHIFTCP], "SHIFT_CP",
	  "Parameter in Boltzman function.",
	  UNIPAR_REAL,
	  &bMZhangPtr->shiftCP, NULL,
	  (void * (*)) SetShiftCP_BasilarM_Zhang);
	SetPar_UniParMgr(&pars[BASILARM_ZHANG_CUTCP], "CUT_CP",
	  "Cut-off frequency of control-path low-path filter (Hz).",
	  UNIPAR_REAL,
	  &bMZhangPtr->cutCP, NULL,
	  (void * (*)) SetCutCP_BasilarM_Zhang);
	SetPar_UniParMgr(&pars[BASILARM_ZHANG_KCP], "K_CP",
	  "Order of control-path low-pass filter.",
	  UNIPAR_INT,
	  &bMZhangPtr->kCP, NULL,
	  (void * (*)) SetKCP_BasilarM_Zhang);
	SetPar_UniParMgr(&pars[BASILARM_ZHANG_R0], "R0",
	  "Ratio of tau_LB (lower bound of tau_SP) to tau_narrow",
	  UNIPAR_REAL,
	  &bMZhangPtr->r0, NULL,
	  (void * (*)) SetR0_BasilarM_Zhang);
	SetPar_UniParMgr(&pars[BASILARM_ZHANG_CFLIST], "CFLIST",
	  "Centre frequency list",
	  UNIPAR_CFLIST,
	  &bMZhangPtr->cFList, NULL,
	  (void * (*)) SetCFList_BasilarM_Zhang);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_BasilarM_Zhang(void)
{
	static const char	*funcName = "GetUniParListPtr_BasilarM_Zhang";

	if (bMZhangPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (bMZhangPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(bMZhangPtr->parList);

}

/****************************** SetModel **************************************/

/*
 * This function sets the module's model parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetModel_BasilarM_Zhang(char * theModel)
{
	static const char	*funcName = "SetModel_BasilarM_Zhang";
	int		specifier;

	if (bMZhangPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theModel,
		bMZhangPtr->modelList)) == BASILARM_ZHANG_MODEL_NULL) {
		NotifyError("%s: Illegal name (%s).", funcName, theModel);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZhangPtr->modelFlag = TRUE;
	bMZhangPtr->updateProcessVariablesFlag = TRUE;
	bMZhangPtr->model = specifier;
	return(TRUE);

}

/****************************** SetSpecies ************************************/

/*
 * This function sets the module's species parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSpecies_BasilarM_Zhang(char * theSpecies)
{
	static const char	*funcName = "SetSpecies_BasilarM_Zhang";
	int		specifier;

	if (bMZhangPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theSpecies,
		SpeciesList_Utility_Zhang(0))) ==
		  UT_ZHANG_SPECIES_NULL) {
		NotifyError("%s: Illegal name (%s).", funcName, theSpecies);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZhangPtr->speciesFlag = TRUE;
	bMZhangPtr->updateProcessVariablesFlag = TRUE;
	bMZhangPtr->species = specifier;
	return(TRUE);

}

/****************************** SetNbOrder ************************************/

/*
 * This function sets the module's nbOrder parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNbOrder_BasilarM_Zhang(int theNbOrder)
{
	static const char	*funcName = "SetNbOrder_BasilarM_Zhang";

	if (bMZhangPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZhangPtr->nbOrderFlag = TRUE;
	bMZhangPtr->updateProcessVariablesFlag = TRUE;
	bMZhangPtr->nbOrder = theNbOrder;
	return(TRUE);

}

/****************************** SetWbOrder ************************************/

/*
 * This function sets the module's wbOrder parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetWbOrder_BasilarM_Zhang(int theWbOrder)
{
	static const char	*funcName = "SetWbOrder_BasilarM_Zhang";

	if (bMZhangPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZhangPtr->wbOrderFlag = TRUE;
	bMZhangPtr->updateProcessVariablesFlag = TRUE;
	bMZhangPtr->wbOrder = theWbOrder;
	return(TRUE);

}

/****************************** SetCornerCP ***********************************/

/*
 * This function sets the module's cornerCP parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCornerCP_BasilarM_Zhang(double theCornerCP)
{
	static const char	*funcName = "SetCornerCP_BasilarM_Zhang";

	if (bMZhangPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZhangPtr->cornerCPFlag = TRUE;
	bMZhangPtr->updateProcessVariablesFlag = TRUE;
	bMZhangPtr->cornerCP = theCornerCP;
	return(TRUE);

}

/****************************** SetSlopeCP ************************************/

/*
 * This function sets the module's slopeCP parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSlopeCP_BasilarM_Zhang(double theSlopeCP)
{
	static const char	*funcName = "SetSlopeCP_BasilarM_Zhang";

	if (bMZhangPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZhangPtr->slopeCPFlag = TRUE;
	bMZhangPtr->updateProcessVariablesFlag = TRUE;
	bMZhangPtr->slopeCP = theSlopeCP;
	return(TRUE);

}

/****************************** SetStrenghCP **********************************/

/*
 * This function sets the module's strenghCP parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetStrenghCP_BasilarM_Zhang(double theStrenghCP)
{
	static const char	*funcName = "SetStrenghCP_BasilarM_Zhang";

	if (bMZhangPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZhangPtr->strenghCPFlag = TRUE;
	bMZhangPtr->updateProcessVariablesFlag = TRUE;
	bMZhangPtr->strenghCP = theStrenghCP;
	return(TRUE);

}

/****************************** SetX0CP ***************************************/

/*
 * This function sets the module's x0CP parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetX0CP_BasilarM_Zhang(double theX0CP)
{
	static const char	*funcName = "SetX0CP_BasilarM_Zhang";

	if (bMZhangPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZhangPtr->x0CPFlag = TRUE;
	bMZhangPtr->updateProcessVariablesFlag = TRUE;
	bMZhangPtr->x0CP = theX0CP;
	return(TRUE);

}

/****************************** SetS0CP ***************************************/

/*
 * This function sets the module's s0CP parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetS0CP_BasilarM_Zhang(double theS0CP)
{
	static const char	*funcName = "SetS0CP_BasilarM_Zhang";

	if (bMZhangPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZhangPtr->s0CPFlag = TRUE;
	bMZhangPtr->updateProcessVariablesFlag = TRUE;
	bMZhangPtr->s0CP = theS0CP;
	return(TRUE);

}

/****************************** SetX1CP ***************************************/

/*
 * This function sets the module's x1CP parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetX1CP_BasilarM_Zhang(double theX1CP)
{
	static const char	*funcName = "SetX1CP_BasilarM_Zhang";

	if (bMZhangPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZhangPtr->x1CPFlag = TRUE;
	bMZhangPtr->updateProcessVariablesFlag = TRUE;
	bMZhangPtr->x1CP = theX1CP;
	return(TRUE);

}

/****************************** SetS1CP ***************************************/

/*
 * This function sets the module's s1CP parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetS1CP_BasilarM_Zhang(double theS1CP)
{
	static const char	*funcName = "SetS1CP_BasilarM_Zhang";

	if (bMZhangPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZhangPtr->s1CPFlag = TRUE;
	bMZhangPtr->updateProcessVariablesFlag = TRUE;
	bMZhangPtr->s1CP = theS1CP;
	return(TRUE);

}

/****************************** SetShiftCP ************************************/

/*
 * This function sets the module's shiftCP parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetShiftCP_BasilarM_Zhang(double theShiftCP)
{
	static const char	*funcName = "SetShiftCP_BasilarM_Zhang";

	if (bMZhangPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZhangPtr->shiftCPFlag = TRUE;
	bMZhangPtr->updateProcessVariablesFlag = TRUE;
	bMZhangPtr->shiftCP = theShiftCP;
	return(TRUE);

}

/****************************** SetCutCP **************************************/

/*
 * This function sets the module's cutCP parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCutCP_BasilarM_Zhang(double theCutCP)
{
	static const char	*funcName = "SetCutCP_BasilarM_Zhang";

	if (bMZhangPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZhangPtr->cutCPFlag = TRUE;
	bMZhangPtr->updateProcessVariablesFlag = TRUE;
	bMZhangPtr->cutCP = theCutCP;
	return(TRUE);

}

/****************************** SetKCP ****************************************/

/*
 * This function sets the module's kCP parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetKCP_BasilarM_Zhang(int theKCP)
{
	static const char	*funcName = "SetKCP_BasilarM_Zhang";

	if (bMZhangPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZhangPtr->kCPFlag = TRUE;
	bMZhangPtr->updateProcessVariablesFlag = TRUE;
	bMZhangPtr->kCP = theKCP;
	return(TRUE);

}

/****************************** SetR0 *****************************************/

/*
 * This function sets the module's r0 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetR0_BasilarM_Zhang(double theR0)
{
	static const char	*funcName = "SetR0_BasilarM_Zhang";

	if (bMZhangPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMZhangPtr->r0Flag = TRUE;
	bMZhangPtr->updateProcessVariablesFlag = TRUE;
	bMZhangPtr->r0 = theR0;
	return(TRUE);

}

/****************************** SetCFList *************************************/

/*
 * This function sets the CFList data structure for the filter bank.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetCFList_BasilarM_Zhang(CFListPtr theCFList)
{
	static const char	*funcName = "SetCFList_BasilarM_Zhang";

	if (bMZhangPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if (!CheckPars_CFList(theCFList)) {
		NotifyError("%s: Centre frequency structure not correctly set.",
		  funcName);
		return(FALSE);
	}
	if (!SetBandwidths_CFList(theCFList, "internal_dynamic", NULL)) {
		NotifyError("%s: Failed to set bandwidth mode.", funcName);
		return(FALSE);
	}
	theCFList->bParList->pars[BANDWIDTH_PAR_MODE].enabled = FALSE;
	if (bMZhangPtr->cFList != NULL)
		Free_CFList(&bMZhangPtr->cFList);
	bMZhangPtr->updateProcessVariablesFlag = TRUE;
	bMZhangPtr->cFList = theCFList;
	return(TRUE);

}

/****************************** SetBandWidths *********************************/

/*
 * This function sets the band width mode for the gamma tone filters.
 * The band width mode defines the function for calculating the gamma tone
 * filter band width 3 dB down.
 * No checks are made on the correct length for the bandwidth array.
 */

BOOLN
SetBandWidths_BasilarM_Zhang(char *theBandwidthMode, double *theBandwidths)
{
	static const char	*funcName = "SetBandWidths_BasilarM_Zhang";

	if (bMZhangPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if (!SetBandwidths_CFList(bMZhangPtr->cFList, theBandwidthMode,
	  theBandwidths)) {
		NotifyError("%s: Failed to set bandwidth mode.", funcName);
		return(FALSE);
	}
	bMZhangPtr->updateProcessVariablesFlag = TRUE;
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
CheckPars_BasilarM_Zhang(void)
{
	static const char	*funcName = "CheckPars_BasilarM_Zhang";
	BOOLN	ok;

	ok = TRUE;
	if (bMZhangPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!bMZhangPtr->modelFlag) {
		NotifyError("%s: model variable not set.", funcName);
		ok = FALSE;
	}
	if (!bMZhangPtr->speciesFlag) {
		NotifyError("%s: species variable not set.", funcName);
		ok = FALSE;
	}
	if (!bMZhangPtr->nbOrderFlag) {
		NotifyError("%s: nbOrder variable not set.", funcName);
		ok = FALSE;
	}
	if (!bMZhangPtr->wbOrderFlag) {
		NotifyError("%s: wbOrder variable not set.", funcName);
		ok = FALSE;
	}
	if (!bMZhangPtr->cornerCPFlag) {
		NotifyError("%s: cornerCP variable not set.", funcName);
		ok = FALSE;
	}
	if (!bMZhangPtr->slopeCPFlag) {
		NotifyError("%s: slopeCP variable not set.", funcName);
		ok = FALSE;
	}
	if (!bMZhangPtr->strenghCPFlag) {
		NotifyError("%s: strenghCP variable not set.", funcName);
		ok = FALSE;
	}
	if (!bMZhangPtr->x0CPFlag) {
		NotifyError("%s: x0CP variable not set.", funcName);
		ok = FALSE;
	}
	if (!bMZhangPtr->s0CPFlag) {
		NotifyError("%s: s0CP variable not set.", funcName);
		ok = FALSE;
	}
	if (!bMZhangPtr->x1CPFlag) {
		NotifyError("%s: x1CP variable not set.", funcName);
		ok = FALSE;
	}
	if (!bMZhangPtr->s1CPFlag) {
		NotifyError("%s: s1CP variable not set.", funcName);
		ok = FALSE;
	}
	if (!bMZhangPtr->shiftCPFlag) {
		NotifyError("%s: shiftCP variable not set.", funcName);
		ok = FALSE;
	}
	if (!bMZhangPtr->cutCPFlag) {
		NotifyError("%s: cutCP variable not set.", funcName);
		ok = FALSE;
	}
	if (!bMZhangPtr->kCPFlag) {
		NotifyError("%s: kCP variable not set.", funcName);
		ok = FALSE;
	}
	if (!bMZhangPtr->r0Flag) {
		NotifyError("%s: r0 variable not set.", funcName);
		ok = FALSE;
	}
	if (!CheckPars_CFList(bMZhangPtr->cFList)) {
		NotifyError("%s: Centre frequency list parameters not correctly set.",
		  funcName);
		ok = FALSE;
	}
	return(ok);

}

/****************************** GetCFListPtr **********************************/

/*
 * This routine returns a pointer to the module's CFList data pointer.
 */

CFListPtr
GetCFListPtr_BasilarM_Zhang(void)
{
	static const char	*funcName = "GetCFListPtr_BasilarM_Zhang";

	if (bMZhangPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (bMZhangPtr->cFList == NULL) {
		NotifyError("%s: CFList data structure has not been correctly set.  "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(bMZhangPtr->cFList);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the lUTEar.parsFile file pointer.
 */

BOOLN
PrintPars_BasilarM_Zhang(void)
{
	static const char	*funcName = "PrintPars_BasilarM_Zhang";

	if (!CheckPars_BasilarM_Zhang()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Zhang et al. BM Module Parameters:-\n");
	DPrint("\tModel = %s,", bMZhangPtr->modelList[bMZhangPtr->
	  model].name);
	DPrint("\tspecies = %s \n", SpeciesList_Utility_Zhang(bMZhangPtr->
	  species)->name);
	DPrint("\tNarrow/wide bandpass filter Orders = %d/%d\n", bMZhangPtr->
	  nbOrder, bMZhangPtr->wbOrder);
	DPrint("\tNonlinearity corner/slope/strengh = %g/%g/%g,\n",
	  bMZhangPtr->cornerCP, bMZhangPtr->slopeCP, bMZhangPtr->
	  strenghCP);
	DPrint("\tControl path Boltzmann parameters:\n");
	DPrint("\t\tx0 = %g,", bMZhangPtr->x0CP);
	DPrint("\ts0 = %g,", bMZhangPtr->s0CP);
	DPrint("\tx1 = %g,", bMZhangPtr->x1CP);
	DPrint("\ts1 = %g,", bMZhangPtr->s1CP);
	DPrint("\tshift = %g,\n", bMZhangPtr->shiftCP);
	DPrint("\tCut-off frequency of control-path low-path filter, cut = %g "
	  "(Hz)\n", bMZhangPtr->cutCP);
	DPrint("\tOrder of control-path low-pass filter, k = %d,\n",
	  bMZhangPtr->kCP);
	DPrint("\t Ratio of tau_LB (lower bound of tau_SP) to tau_narrow, r0 = "
	  "%g\n", bMZhangPtr->r0);
	PrintPars_CFList(bMZhangPtr->cFList);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_BasilarM_Zhang(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_BasilarM_Zhang";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	bMZhangPtr = (BMZhangPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 * It also initialises the process structure.
 */

BOOLN
InitModule_BasilarM_Zhang(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_BasilarM_Zhang";

	if (!SetParsPointer_BasilarM_Zhang(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_BasilarM_Zhang(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = bMZhangPtr;
	theModule->CheckPars = CheckPars_BasilarM_Zhang;
	theModule->Free = Free_BasilarM_Zhang;
	theModule->GetUniParListPtr = GetUniParListPtr_BasilarM_Zhang;
	theModule->PrintPars = PrintPars_BasilarM_Zhang;
	theModule->RunProcess = RunModel_BasilarM_Zhang;
	theModule->SetParsPointer = SetParsPointer_BasilarM_Zhang;
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
CheckData_BasilarM_Zhang(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_BasilarM_Zhang";

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInit_SignalData(data->inSignal[0], funcName))
		return(FALSE);
	/*** Put additional checks here. ***/
	return(TRUE);

}

/****************************** CochleaF2x ************************************/

/*
 * Original name: 'cochlea_f2x'.
 * Extracted from the cmpa.c code (LPO).
 */

/*/ ----------------------------------------------------------------------------
 ** Calculate the location on Basilar Membrane from best frequency
*/
double
CochleaF2x_BasilarM_Zhang(int species,double f)
{
  double x;
  switch(species)
    {
    case 0: /*/human */
      x=(1.0/0.06)*log10((f/165.4)+0.88);
      break;
    default:
    case 1: /*/cat */
      x = 11.9 * log10(0.80 + f / 456.0);
      break;
    };
   return(x);
}

/****************************** CochleaX2f ************************************/

/*
 * Original name: 'cochlea_xf2'.
 * Extracted from the cmpa.c code (LPO).
 */

/* ----------------------------------------------------------------------------
 ** Calculate the best frequency from the location on basilar membrane 
 */
double
CochleaX2f_BasilarM_Zhang(int species,double x)
{
  double f;
  switch(species)
    {
    case 0: /*human */
      f=165.4*(pow(10,(0.06*x))-0.88);
      break;
    default:
    case 1: /*/cat */
      f = 456.0*(pow(10,x/11.9)-0.80);
      break;
    };
  return(f);
}

/****************************** GetTau ****************************************/

/*
 * Original name: get_tau
 * Orignal file: cmpa.a
 * Get TauMax, TauMin for the tuning filter. The TauMax is determined by the
 * bandwidth/Q10 of the tuning filter at low level. The TauMin is determined by
 * the gain change between high and low level
 * Also the calculation is different for different species.
 * LPO I use the DSAM 'ERBFromF_Bandwidth' function here instead of the
 * 'erbGM' function from the original.  I have added the (1.0 / 1.2) factor
 * which is where the original code differed.  I do not know where it came from.
 */

double
GetTau_BasilarM_Zhang(int species, double cf,int order, double* _taumax,
  double* _taumin, double* _taurange)
{
  double taumin,taumax,taurange;
  double ss0,cc0,ss1,cc1;
  double Q10,bw,gain,ratio;
  double xcf, x1000;
  gain = 20+42*log10(cf/1e3);     /*/ estimate compression gain of the filter */
  if(gain>70) gain = 70;
  if(gain<15) gain = 15;
  ratio = pow(10,(-gain/(20.0*order)));  /*/ ratio of TauMin/TauMax according
  										to the gain, order */

  /*/ Calculate the TauMax according to different species */
  switch(species)
    {
    case 1:
      /* Cat parameters: Tau0 vs. CF (from Carney & Yin 88) - only good for */
	  /* low  CFs. */
      /* Note: Tau0 is gammatone time constant for 'high' levels (80 dB rms) */
      /* parameters for cat Tau0 vs. CF */
      xcf = CochleaF2x_BasilarM_Zhang(species,cf);	/* position of cf unit;
	  													 from Liberman's map */
      x1000 = CochleaF2x_BasilarM_Zhang(species,1000);	/* position for 1
	  														Khz */
      ss0 = 6.; cc0 = 1.1; ss1 = 2.2; cc1 = 1.1;
      taumin = ( cc0 * exp( -xcf / ss0) + cc1 * exp( -xcf /ss1) ) * 1e-3;
	  /* above: in sec */
      taurange = taumin * xcf/x1000; 
      taumax = taumin+taurange;
      break;
    case 0:
      /* Human Parameters: From Mike Heinz: */
      /* Bandwidths now are based on Glasberg and Moore's (1990) */
	  /* ERB=f(CF,level) equations  */
      taumax =  1.0 / (PIx2 * 1.019 * (1.0 / 1.2) * ERBFromF_Bandwidth(cf));
      break;
    case 9:
      /* Universal species from data fitting : From Xuedong Zhang,Ian 
	   * (JASA 2001) */
      /* the Q10 determine the taumax(bandwidths at low level) Based on Cat*/
      Q10 = pow(10,0.4708*log10(cf/1e3)+0.4664);
      bw = cf/Q10;
      taumax = 2.0/(PIx2*bw);
    }; /*/end of switch */
  taumin =  taumax*ratio;
  taurange = taumax-taumin;
  *_taumin = taumin;
  *_taumax = taumax;
  *_taurange = taurange;
  return 0;

}

/****************************** RunGammaTone ***********************************/

/*
 * Original name: runGammaTone
 * Original file: filters.c
 */

/**

   Pass the signal through the gammatone filter\\
   1. shift the signal by centeral frequency of the gamma tone filter\\
   2. low pass the shifted signal \\
   3. shift back the signal \\
   4. take the real part of the signal as output
   @author Xuedong Zhang
 */

double
RunGammaTone_BasilarM_Zhang(TGammaTone *p, double x)
{
	int		i,j;
	double	out;
	Complex	c1, c2, c_phase;

	x *= p->gain;
	p->phase += p->delta_phase;

	EXP_CMPLXM(c_phase, p->phase); /*/ FREQUENCY SHIFT */
	SCALER_MULT_CMPLXM(p->gtf[0], c_phase,x);
	for( j = 1; j <= p->Order; j++) {      /*/ IIR Bilinear transformation LPF */
		ADD_CMPLXM(c1, p->gtf[j-1], p->gtfl[j-1]);
		SCALER_MULT_CMPLXM(c2, c1, p->c2LP);
		SCALER_MULT_CMPLXM(c1, p->gtfl[j], p->c1LP);
		ADD_CMPLXM(p->gtf[j], c1, c2);
	};
	CONV2CONJ_CMPLX(c_phase); /*/ FREQ SHIFT BACK UP */
	MULT_CMPLXM(c1, c_phase, p->gtf[p->Order]);
	out = c1.re;
	for(i = 0; i <= p->Order; i++)
		p->gtfl[i] = p->gtf[i];
	return(out);

}

/****************************** RunGammaTone2 *********************************/

/*
 * Original name: runGammaTone2
 * Original file: filters.c
 */

void
RunGammaTone2_BasilarM_Zhang(TGammaTone *p, const double *in, double *out,
  const int length)
{
  int register loopSig,loopGT;
  double	x;
  Complex	c1,c2,c_phase;

	for(loopSig = 0; loopSig < length; loopSig++) {
		x = p->gain * in[loopSig];
		p->phase += p->delta_phase;

		EXP_CMPLXM(c_phase,p->phase); /*/ FREQUENCY SHIFT */
		SCALER_MULT_CMPLXM(p->gtf[0],c_phase,x);
		for( loopGT = 1; loopGT <= p->Order; loopGT++) { /*/ IIR Bilinear transformation LPF */
			ADD_CMPLXM(c1,p->gtf[loopGT-1],p->gtfl[loopGT-1]);
			SCALER_MULT_CMPLXM(c2,c1,p->c2LP);
			SCALER_MULT_CMPLXM(c1,p->gtfl[loopGT],p->c1LP);
			ADD_CMPLXM(p->gtf[loopGT],c1,c2);
		}
		CONV2CONJ_CMPLX(c_phase); /* FREQ SHIFT BACK UP */
		MULT_CMPLXM(c1,c_phase,p->gtf[p->Order]);
		for(loopGT=0; loopGT<=p->Order;loopGT++)
			p->gtfl[loopGT] = p->gtf[loopGT];

		out[loopSig] = c1.re;
	}

}

/****************************** SetGammaToneTau ********************************/

/*
 * Original name: setGammaToneTau
 * Original file: filters.c
 */

/**

   Reset the tau of the gammatone filter\\
   it recalculate the c1 c2 used by the filtering function
 */
void
setGammaToneTau_BasilarM_Zhang(TGammaTone *p, double tau)
{
  double dtmp;
  p->tau = tau;
  dtmp = tau*2.0/p->tdres;
  p->c1LP = (dtmp-1)/(dtmp+1);
  p->c2LP = 1.0/(dtmp+1);

}

/****************************** SetZhangGTCoeffs ******************************/

/*
 * Original name: initgammatone
 * Original file: filters.c
 */

void
SetZhangGTCoeffs_BasilarM_Zhang(TGammaTone* res,double _tdres,
  double _Fshift,double _tau,double _gain,int _order)
{
	double c;
	int	i;

	res->tdres = _tdres;
	res->F_shift = _Fshift;
	res->delta_phase = -TWOPI*_Fshift*_tdres;
	res->phase = 0;
	res->tau = _tau;

	c = 2.0/_tdres; /* for bilinear transformation */
	res->c1LP = (_tau*c-1)/(_tau*c+1);
	res->c2LP = 1/(_tau*c+1);
	res->gain = _gain;
	res->Order = _order;
	for( i = 0; i <= res->Order; i++) 
		res->gtf[i].re = res->gtfl[i].re = res->gtf[i].im = res->gtfl[i].im =
		  0.0;
	res->Run = RunGammaTone_BasilarM_Zhang;
	res->Run2 = RunGammaTone2_BasilarM_Zhang;
	res->SetTau = setGammaToneTau_BasilarM_Zhang;

}

/****************************** InitBasilarMembrane ****************************/

/*
 * Extracted from the cmpa.c code (LPO).
 * User GetTau_BasilarM_Zhang, initGammaTone, initBoltzman
 *
 * ##### Get Basilar Membrane ########
 * 1. Get a structure of BasilarMembrane
 * 2. Specify wide band filter in the BasilarMembrane Model 
 *    //WB filter not used for all model versions, but it's computed here
 *	  anyway
 * 3. Specify the OHC model in the control path
 *    3.2 Specify the NL function used in the OHC
 * 4. Specify the AfterOHC NL in the control path
 *
 * DSAM changes: L. P. O'Mard.
 * adsdb: I have removed this variable which is now encampessed in the
 * 'bMZhangPtr->cornerCP' parameter.
 */

BOOLN
InitBasilarMembrane_BasilarM_Zhang(TBasilarMembranePtr bm, int model,
  int species, double tdres, double cf)
{
	static const char *funcName = "initBasilarMembrane";
	int		bmmodel;
	double	taumax,taumin,taurange; /* general */
	double	x, centerfreq,tauwb,tauwbmin,dtmp,wb_gain; /* for wb */
	double	dc,R,minR; /* for afterohc */
	TNonLinear *p;

	if (!bm) {
		NotifyError("%s: Basilar membrane structure not initialised.",
		  funcName);
		return(FALSE);
	}
	/* Model numbers as used in ARLO Heinz et al., Fig. 4 */
	if(model == 1)
		bmmodel = FeedForward_NL;
	else if(model == 2)
		bmmodel = FeedBack_NL;
	else if(model == 3)
		bmmodel = Sharp_Linear;
	else if(model == 4)
		bmmodel = Broad_Linear;
	else if(model == 5)
		bmmodel = Broad_Linear_High;
	bm->bmmodel = bmmodel;
	bm->tdres = tdres;

	/*
	*  Determine taumax,taumin,order here
	*/
	/* bm->Run2 = Run2BasilarMembrane; */

	bm->bmorder = bMZhangPtr->nbOrder;
	GetTau_BasilarM_Zhang(species, cf, bm->bmorder, &taumax, &taumin,
	  &taurange);

	bm->TauMax = taumax;
	bm->TauMin = taumin;
	if(bm->bmmodel&Broad_ALL) 
		bm->tau = taumin;
	else
		bm->tau = taumax;

	SetZhangGTCoeffs_BasilarM_Zhang(&(bm->bmfilter), tdres, cf, bm->tau,
	  1.0, bm->bmorder);
	SetZhangGTCoeffs_BasilarM_Zhang(&(bm->gfagain), tdres, cf, taumin, 1.0,
	  1);
	/*
	* Get Wbfilter parameters
	*/
	x = CochleaF2x_BasilarM_Zhang(species,cf);
	/* shift the center freq Qing use 1.1 shift */
	centerfreq = CochleaX2f_BasilarM_Zhang(species,x+1.2);
	bm->wborder = bMZhangPtr->wbOrder;
	tauwb = taumin+0.2*(taumax-taumin);
	tauwbmin = tauwb/taumax*taumin;
	dtmp = tauwb*PIx2*(centerfreq-cf);
	wb_gain = pow((1+dtmp*dtmp), bm->wborder/2.0);
	bm->TauWB = tauwb;
	bm->TauWBMin = tauwbmin;
	SetZhangGTCoeffs_BasilarM_Zhang(&(bm->wbfilter), tdres, centerfreq,
	  tauwb, wb_gain, bm->wborder);

	bm->A = (tauwb / taumax - tauwbmin / taumin) / (taumax - taumin);
	bm->B= (taumin * taumin * tauwb - taumax * taumax * tauwbmin) / (taumax *
	  taumin * (taumax - taumin));

	/*
	* Init OHC model
	*/
	/* LPO absdb = 20;*/ /* The value that the BM starts compression */
	InitLowPass_Utility_Zhang(&(bm->ohc.hclp), tdres, bMZhangPtr->
	  cutCP, 1.0, bMZhangPtr->kCP);
	/* This is now use in both Human & Cat MODEL */
	/*/ parameter into boltzman is corner,slope,strength,x0,s0,x1,s1,asym
	// The corner determines the level that BM have compressive nonlinearity */
	/*/set OHC Nonlinearity as boltzman function combination */
	init_boltzman(&(bm->ohc.hcnl), bMZhangPtr->cornerCP, bMZhangPtr->
	  slopeCP, bMZhangPtr->strenghCP, bMZhangPtr->x0CP,
	  bMZhangPtr->s0CP, bMZhangPtr->x1CP, bMZhangPtr->s1CP,
	  bMZhangPtr->shiftCP);
	bm->ohc.Run = RunHairCell;
	/*bm->ohc.Run2 = RunHairCell2; */

	/*
	* Init AfterOHC model
	*/
	p = &(bm->afterohc);
	dc = (bMZhangPtr->shiftCP - 1) / (bMZhangPtr->shiftCP + 1.0) /
	  2.0;
	dc -= 0.05;
	minR = bMZhangPtr->r0;
	p->TauMax = taumax;
	p->TauMin = taumin;
	R = taumin/taumax;
	if (R<minR)
		p->minR = 0.5 * R;
	else
		p->minR = minR;
	p->A = p->minR / (1.0 - p->minR); /* makes x = 0; output = 1; */
	p->dc = dc;
	R = R-p->minR;
	/* This is for new nonlinearity */
	p->s0 = -dc / log(R / (1.0 - p->minR));
	p->Run = RunAfterOhcNL_Utility_Zhang;
	p->Run2 = RunAfterOhcNL2_Utility_Zhang;
	return (TRUE);

}

/****************************** InitProcessVariables **************************/

/*
 * This function allocates the memory for the process variables.
 * It assumes that all of the parameters for the module have been
 * correctly initialised.
 */

BOOLN
InitProcessVariables_BasilarM_Zhang(EarObjectPtr data)
{
	static const char	*funcName = "InitProcessVariables_BasilarM_Zhang";
	BMZhangPtr	p = bMZhangPtr;
	int		i, cFIndex;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag || p->cFList->
	  updateFlag) {
		/*** Additional update flags can be added to above line ***/
		FreeProcessVariables_BasilarM_Zhang();
		if ((p->bM = (TBasilarMembrane *) calloc(data->outSignal->numChannels,
		  sizeof(TBasilarMembrane))) == NULL) {
		 	NotifyError("%s: Out of memory for coefficients array.", funcName);
		 	return(FALSE);
		}
		SetLocalInfoFlag_SignalData(data->outSignal, TRUE);
		SetInfoChannelTitle_SignalData(data->outSignal, "Frequency (Hz)");
		SetInfoChannelLabels_SignalData(data->outSignal,
		   p->cFList->frequency);
		SetInfoCFArray_SignalData(data->outSignal, p->cFList->frequency);
		bMZhangPtr->updateProcessVariablesFlag = FALSE;
		p->cFList->updateFlag = FALSE;
	}
	if (data->timeIndex == PROCESS_START_TIME) {
		for (i = 0; i < data->outSignal->numChannels; i++) {
			cFIndex = i / data->inSignal[0]->interleaveLevel;
			if (!InitBasilarMembrane_BasilarM_Zhang(&p->bM[i], p->model +
			  1, p->species, data->outSignal->dt, p->cFList->frequency[
			  cFIndex])) {
				NotifyError("%s: Could not initialised BM for channel (%d).",
				  funcName, i);
				FreeProcessVariables_BasilarM_Zhang();
				return(FALSE);
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
FreeProcessVariables_BasilarM_Zhang(void)
{
	if (bMZhangPtr->bM) {
		free(bMZhangPtr->bM);
		bMZhangPtr->bM = NULL;
	}
	return(TRUE);

}

/****************************** Run2BasilarMembrane ***************************/

/*
 * Extracted from the cmpa.c code (LPO).
 */

void
Run2BasilarMembrane_BasilarM_Zhang(TBasilarMembrane *bm, const double *in,
  double *out, const int length)
{
	register int i;
	double wb_gain,dtmp,taunow;
	double wbout,ohcout;
	double x, x1,out1;

	for(i=0; i<length; i++) {
		x = in[i];
		/*/ pass the signal through the tuning filter */
		x1 = bm->bmfilter.Run(&(bm->bmfilter),x);
		switch(bm->bmmodel){
		case Broad_Linear_High: /* /adjust the gain of the tuning filter */
			out1 = x1*pow((bm->TauMin/bm->TauMax),bm->bmfilter.Order);
			break;
		case FeedBack_NL:
			/*/get the output of the tuning filter as the control signal */
			wbout = x1;
			/*/ pass the control signal through OHC model */
			ohcout = bm->ohc.Run(&(bm->ohc),wbout);
			/*/ pass the control signal through nonliearity after OHC */
			bm->tau = bm->afterohc.Run(&(bm->afterohc),ohcout);     
			/*/ set the tau of the tuning filter */
			bm->bmfilter.SetTau(&(bm->bmfilter),bm->tau);     
			/*  Gain Control of the tuning filter */
			out1 = pow((bm->tau/bm->TauMax),bm->bmfilter.Order)*x1;  
			break;
		case FeedForward_NL:
			/*/get the output of the wide-band pass as the control signal */
			wbout = bm->wbfilter.Run(&(bm->wbfilter),x);
			/*/ scale the tau for wide band filter in control path */
			taunow = bm->A*bm->tau*bm->tau-bm->B*bm->tau;

			/*/set the tau of the wide-band filter*/
			bm->wbfilter.SetTau(&(bm->wbfilter),taunow);
			/*/ normalize the gain of the wideband pass filter as 0dB at CF */
			dtmp = taunow*PIx2*(bm->wbfilter.F_shift-bm->bmfilter.F_shift);
			wb_gain = pow((1+dtmp*dtmp), bm->wbfilter.Order/2.0);
			bm->wbfilter.gain=wb_gain;

			/*/ pass the control signal through OHC model*/
			ohcout = bm->ohc.Run(&(bm->ohc),wbout);
			/*/ pass the control signal through nonliearity after OHC */
			bm->tau = bm->afterohc.Run(&(bm->afterohc),ohcout);
			/*/ set the tau of the tuning filter */
			bm->bmfilter.SetTau(&(bm->bmfilter),bm->tau);
			/*/ Gain Control of the tuning filter */
			out1 = pow((bm->tau/bm->TauMax),bm->bmfilter.Order)*x1;
			break;
		default:
		case Sharp_Linear:
		case Broad_Linear:
			/* / if linear model, needn't get the control signal */
			out1 = x1;
			break;
		};
		out[i] = out1;
	};
	bm->gfagain.Run2(&(bm->gfagain),out,out,length);

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
RunModel_BasilarM_Zhang(EarObjectPtr data)
{
	static const char	*funcName = "RunModel_BasilarM_Zhang";
	register ChanData	 *inPtr, *outPtr;
	int		chan;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckPars_BasilarM_Zhang())
		return(FALSE);
	if (!CheckData_BasilarM_Zhang(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "Zhang et al. Non-linear BM filtering");

	/*** Example Initialise output signal - ammend/change if required. ***/
	if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels,
	  data->inSignal[0]->length, data->inSignal[0]->dt)) {
		NotifyError("%s: Cannot initialise output channels.", funcName);
		return(FALSE);
	}

	if (!InitProcessVariables_BasilarM_Zhang(data)) {
		NotifyError("%s: Could not initialise the process variables.",
		  funcName);
		return(FALSE);
	}
	for (chan = 0; chan < data->inSignal[0]->numChannels; chan++) {
		inPtr = data->inSignal[0]->channel[chan];
		outPtr = data->outSignal->channel[chan];
		Run2BasilarMembrane_BasilarM_Zhang(&bMZhangPtr->bM[chan],
		  inPtr, outPtr, data->outSignal->length);
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

