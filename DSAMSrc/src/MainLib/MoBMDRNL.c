/**********************
 *
 * File:		MoBMDRNL.c
 * Purpose:		The DRNL Composite filter is used to model the response of the
 *				basilar membrane.
 * Comments:	This is an implementation of a digital filter using the
 *				filters.c module.
 *				28-05-97 LPO: Corrected linearFLPCutOffScale parameter - it
 *				wasn't being used, and now a negative or zero value disables
 *				the filter altogether.
 *				06-06-97 LPO: added outputScale parameter.
 *				21-12-98 LPO: Implemented new parameters for filter bank.
 *				These parameters use the broken stick compression - see
 *				UtFilters.
 *				12-01-99 LPO: Implemented universal parameters list.
 * Authors:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		12 Jan 1999
 * Copyright:	(c) 1999-2000, University of Essex.
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
#include "GeNSpecLists.h"
#include "FiParFile.h"
#include "UtParArray.h"
#include "UtBandwidth.h"
#include "UtCFList.h"
#include "UtFilters.h"
#include "MoBMDRNL.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

BMDRNLPtr	bMDRNLPtr = NULL;

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
Free_BasilarM_DRNL(void)
{
	/* static const char	*funcName = "Free_BasilarM_DRNL";  */

	if (bMDRNLPtr == NULL)
		return(FALSE);
	FreeProcessVariables_BasilarM_DRNL();
	Free_ParArray(&bMDRNLPtr->nonLinBwidth);
	Free_ParArray(&bMDRNLPtr->comprScaleA);
	Free_ParArray(&bMDRNLPtr->comprScaleB);
	Free_ParArray(&bMDRNLPtr->linCF);
	Free_ParArray(&bMDRNLPtr->linBwidth);
	Free_ParArray(&bMDRNLPtr->linScaleG);
	Free_CFList(&bMDRNLPtr->theCFs);
	if (bMDRNLPtr->parList)
		FreeList_UniParMgr(&bMDRNLPtr->parList);
	if (bMDRNLPtr->parSpec == GLOBAL) {
		free(bMDRNLPtr);
		bMDRNLPtr = NULL;
	}
	return(TRUE);

}

/****************************** GetFitFuncPars ********************************/

/*
 * This function returns the number of parameters for the respective fit
 * function mode. Using it helps maintain the correspondence
 * between the mode names.
 */

int
GetFitFuncPars_BasilarM_DRNL(int mode)
{
	static const char	*funcName = "GetFitFuncPars_BasilarM_DRNL";

	switch (mode) {
	case GENERAL_FIT_FUNC_LOG_FUNC1_MODE:
		return(2);
		break;
	case GENERAL_FIT_FUNC_POLY_FUNC1_MODE:
		return(3);
		break;
	default:
		NotifyError("%s: Mode not listed (%d), returning zero.", funcName,
		  mode);
		return(0);
	}
}

/****************************** GetFitFuncValue *******************************/

/*
 * This function returns the value for the respective fit function mode.
 * Using it helps maintain the correspondence between the mode names.
 */

double
GetFitFuncValue_BasilarM_DRNL(ParArrayPtr p, double linCF)
{
	static const char	*funcName = "GetFitFuncValue_BasilarM_DRNL";

	switch (p->mode) {
	case GENERAL_FIT_FUNC_LOG_FUNC1_MODE:
		return(pow(10.0, p->params[0] + p->params[1] * log10(linCF)));
		break;
	case GENERAL_FIT_FUNC_POLY_FUNC1_MODE:
		return(p->params[0] + p->params[1] * linCF + p->params[2] * linCF *
		  linCF);
		break;
	default:
		NotifyError("%s: Mode (%d) not listed, returning zero.", funcName,
		  p->mode);
		return(0.0);
	}
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
Init_BasilarM_DRNL(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_BasilarM_DRNL";

	if (parSpec == GLOBAL) {
		if (bMDRNLPtr != NULL)
			Free_BasilarM_DRNL();
		if ((bMDRNLPtr = (BMDRNLPtr) malloc(sizeof(BMDRNL))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (bMDRNLPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	bMDRNLPtr->parSpec = parSpec;
	bMDRNLPtr->nonLinGTCascadeFlag = FALSE;
	bMDRNLPtr->nonLinLPCascadeFlag = FALSE;
	bMDRNLPtr->comprExponentFlag = FALSE;
	bMDRNLPtr->linGTCascadeFlag = FALSE;
	bMDRNLPtr->linLPCascadeFlag = FALSE;
	bMDRNLPtr->nonLinGTCascade = 0;
	bMDRNLPtr->nonLinLPCascade = 0;
	if ((bMDRNLPtr->nonLinBwidth = Init_ParArray("NonLinBwidth", 
	  FitFuncModeList_NSpecLists(0), GetFitFuncPars_BasilarM_DRNL)) == NULL) {
		NotifyError("%s: Could not initialise NonLinBwidth parArray structure",
		  funcName);
		Free_BasilarM_DRNL();
		return(FALSE);
	}
	if ((bMDRNLPtr->comprScaleA = Init_ParArray("ComprScaleA",
	  FitFuncModeList_NSpecLists(0), GetFitFuncPars_BasilarM_DRNL)) == NULL) {
		NotifyError("%s: Could not initialise ComprScaleA parArray structure",
		  funcName);
		Free_BasilarM_DRNL();
		return(FALSE);
	}
	if ((bMDRNLPtr->comprScaleB = Init_ParArray("ComprScaleB",
	  FitFuncModeList_NSpecLists(0), GetFitFuncPars_BasilarM_DRNL)) == NULL) {
		NotifyError("%s: Could not initialise ComprScaleB parArray structure",
		  funcName);
		Free_BasilarM_DRNL();
		return(FALSE);
	}
	bMDRNLPtr->comprExponent = 0.0;
	bMDRNLPtr->linGTCascade = 0;
	bMDRNLPtr->linLPCascade = 0;
	if ((bMDRNLPtr->linCF = Init_ParArray("LinCF", FitFuncModeList_NSpecLists(
	  0), GetFitFuncPars_BasilarM_DRNL)) == NULL) {
		NotifyError("%s: Could not initialise LinCF parArray structure",
		  funcName);
		Free_BasilarM_DRNL();
		return(FALSE);
	}
	if ((bMDRNLPtr->linBwidth = Init_ParArray("LinBwidth",
	  FitFuncModeList_NSpecLists(0), GetFitFuncPars_BasilarM_DRNL)) == NULL) {
		NotifyError("%s: Could not initialise LinBwidth parArray structure",
		  funcName);
		Free_BasilarM_DRNL();
		return(FALSE);
	}
	if ((bMDRNLPtr->linScaleG = Init_ParArray("linScaleG",
	  FitFuncModeList_NSpecLists(0), GetFitFuncPars_BasilarM_DRNL)) == NULL) {
		NotifyError("%s: Could not initialise linScaleG parArray structure",
		  funcName);
		Free_BasilarM_DRNL();
		return(FALSE);
	}
	bMDRNLPtr->theCFs = NULL;

	if (!SetUniParList_BasilarM_DRNL()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_BasilarM_DRNL();
		return(FALSE);
	}
	bMDRNLPtr->compressionA = NULL;
	bMDRNLPtr->compressionB = NULL;
	bMDRNLPtr->nonLinearGT1 = NULL;
	bMDRNLPtr->nonLinearGT2 = NULL;
	bMDRNLPtr->linearGT = NULL;
	bMDRNLPtr->nonLinearLP = NULL;
	bMDRNLPtr->linearLP = NULL;
	bMDRNLPtr->linearF = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_BasilarM_DRNL(void)
{
	static const char *funcName = "SetUniParList_BasilarM_DRNL";
	UniParPtr	pars;

	if ((bMDRNLPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  BASILARM_DRNL_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = bMDRNLPtr->parList->pars;
	SetPar_UniParMgr(&pars[BASILARM_DRNL_NONLINGTCASCADE], "NL_GT_CASCADE",
	  "Nonlinear gammatone filter cascade.",
	  UNIPAR_INT,
	  &bMDRNLPtr->nonLinGTCascade, NULL,
	  (void * (*)) SetNonLinGTCascade_BasilarM_DRNL);
	SetPar_UniParMgr(&pars[BASILARM_DRNL_NONLINLPCASCADE], "NL_LP_CASCADE",
	  "Nonlinear low-pass filter cascade.",
	  UNIPAR_INT,
	  &bMDRNLPtr->nonLinLPCascade, NULL,
	  (void * (*)) SetNonLinLPCascade_BasilarM_DRNL);
	SetPar_UniParMgr(&pars[BASILARM_DRNL_NONLINBWIDTH], "NL_BW_VAR_FUNC",
	  "Non-linear bandwidth variable function (Hz vs non-linear CF).",
	  UNIPAR_PARARRAY,
	  &bMDRNLPtr->nonLinBwidth, NULL,
	  (void * (*)) SetNonLinBwidth_BasilarM_DRNL);
	SetPar_UniParMgr(&pars[BASILARM_DRNL_COMPRSCALEA], "COMP_A_VAR_FUNC",
	  "Compression A (linear) scale variable function (vs non-linear CF).",
	  UNIPAR_PARARRAY,
	  &bMDRNLPtr->comprScaleA, NULL,
	  (void * (*)) SetComprScaleA_BasilarM_DRNL);
	SetPar_UniParMgr(&pars[BASILARM_DRNL_COMPRSCALEB], "COMP_B_VAR_FUNC",
	  "Compression b (gain) scale variable function (vs non-linear CF).",
	  UNIPAR_PARARRAY,
	  &bMDRNLPtr->comprScaleB, NULL,
	  (void * (*)) SetComprScaleB_BasilarM_DRNL);
	SetPar_UniParMgr(&pars[BASILARM_DRNL_COMPREXPONENT], "COMP_N_EXPON",
	  "Compression exponent, n (units).",
	  UNIPAR_REAL,
	  &bMDRNLPtr->comprExponent, NULL,
	  (void * (*)) SetComprExponent_BasilarM_DRNL);
	SetPar_UniParMgr(&pars[BASILARM_DRNL_LINGTCASCADE], "L_GT_CASCADE",
	  "Linear gammatone filter cascade.",
	  UNIPAR_INT,
	  &bMDRNLPtr->linGTCascade, NULL,
	  (void * (*)) SetLinGTCascade_BasilarM_DRNL);
	SetPar_UniParMgr(&pars[BASILARM_DRNL_LINLPCASCADE], "L_LP_CASCADE",
	  "Linear low-pass filter cascade.",
	  UNIPAR_INT,
	  &bMDRNLPtr->linLPCascade, NULL,
	  (void * (*)) SetLinLPCascade_BasilarM_DRNL);
	SetPar_UniParMgr(&pars[BASILARM_DRNL_LINCF], "L_CF_VAR_FUNC",
	  "Linear CF variable function (Hz vs linear CF).",
	  UNIPAR_PARARRAY,
	  &bMDRNLPtr->linCF, NULL,
	  (void * (*)) SetLinCF_BasilarM_DRNL);
	SetPar_UniParMgr(&pars[BASILARM_DRNL_LINBWIDTH], "L_BW_VAR_FUNC",
	  "Linear bandwidth variable function (Hz vs non-linear CF).",
	  UNIPAR_PARARRAY,
	  &bMDRNLPtr->linBwidth, NULL,
	  (void * (*)) SetLinBwidth_BasilarM_DRNL);
	SetPar_UniParMgr(&pars[BASILARM_DRNL_LINSCALEG], "L_SCALER",
	  "Linear filter scale variable function (vs non-linear CF).",
	  UNIPAR_PARARRAY,
	  &bMDRNLPtr->linScaleG, NULL,
	  (void * (*)) SetLinScaleG_BasilarM_DRNL);
	SetPar_UniParMgr(&pars[BASILARM_DRNL_THECFS], "CFLIST",
	  "Centre frequency specification.",
	  UNIPAR_CFLIST,
	  &bMDRNLPtr->theCFs, NULL,
	  (void * (*)) SetCFList_BasilarM_DRNL);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_BasilarM_DRNL(void)
{
	static const char	*funcName = "GetUniParListPtr_BasilarM_DRNL";

	if (bMDRNLPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (bMDRNLPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(bMDRNLPtr->parList);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_BasilarM_DRNL(int nonLinGTCascade, int nonLinLPCascade,
  ParArrayPtr nonLinBwidth, ParArrayPtr comprScaleA, ParArrayPtr comprScaleB,
  double comprExponent, int linGTCascade, int linLPCascade, ParArrayPtr
  linCF, ParArrayPtr linBwidth, ParArrayPtr linScaleG, CFListPtr theCFs)
{
	static const char	*funcName = "SetPars_BasilarM_DRNL";
	BOOLN	ok;

	ok = TRUE;
	if (!SetNonLinGTCascade_BasilarM_DRNL(nonLinGTCascade))
		ok = FALSE;
	if (!SetNonLinLPCascade_BasilarM_DRNL(nonLinLPCascade))
		ok = FALSE;
	if (!SetNonLinBwidth_BasilarM_DRNL(nonLinBwidth))
		ok = FALSE;
	if (!SetComprScaleA_BasilarM_DRNL(comprScaleA))
		ok = FALSE;
	if (!SetComprScaleB_BasilarM_DRNL(comprScaleB))
		ok = FALSE;
	if (!SetComprExponent_BasilarM_DRNL(comprExponent))
		ok = FALSE;
	if (!SetLinGTCascade_BasilarM_DRNL(linGTCascade))
		ok = FALSE;
	if (!SetLinLPCascade_BasilarM_DRNL(linLPCascade))
		ok = FALSE;
	if (!SetLinCF_BasilarM_DRNL(linCF))
		ok = FALSE;
	if (!SetLinBwidth_BasilarM_DRNL(linBwidth))
		ok = FALSE;
	if (!SetLinScaleG_BasilarM_DRNL(linScaleG))
		ok = FALSE;
	if (!SetCFList_BasilarM_DRNL(theCFs))
		ok = FALSE;
	if (!ok)
		NotifyError("%s: Failed to set all module parameters." ,funcName);
	return(ok);

}

/****************************** SetNonLinGTCascade ****************************/

/*
 * This function sets the module's nonLinGTCascade parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNonLinGTCascade_BasilarM_DRNL(int theNonLinGTCascade)
{
	static const char	*funcName = "SetNonLinGTCascade_BasilarM_DRNL";

	if (bMDRNLPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMDRNLPtr->nonLinGTCascadeFlag = TRUE;
	bMDRNLPtr->nonLinGTCascade = theNonLinGTCascade;
	return(TRUE);

}

/****************************** SetNonLinLPCascade ****************************/

/*
 * This function sets the module's nonLinLPCascade parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNonLinLPCascade_BasilarM_DRNL(int theNonLinLPCascade)
{
	static const char	*funcName = "SetNonLinLPCascade_BasilarM_DRNL";

	if (bMDRNLPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMDRNLPtr->nonLinLPCascadeFlag = TRUE;
	bMDRNLPtr->nonLinLPCascade = theNonLinLPCascade;
	return(TRUE);

}

/****************************** SetNonLinBwidth *******************************/

/*
 * This function sets the ParArray data structure for the module.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetNonLinBwidth_BasilarM_DRNL(ParArrayPtr theNonLinBwidth)
{
	static const char	*funcName = "SetNonLinBwidth_BasilarM_DRNL";

	if (bMDRNLPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if (!CheckInit_ParArray(theNonLinBwidth, funcName)) {
		NotifyError("%s: ParArray structure not correctly set.",  funcName);
		return(FALSE);
	}
	bMDRNLPtr->updateProcessVariablesFlag = TRUE;
	bMDRNLPtr->nonLinBwidth = theNonLinBwidth;
	return(TRUE);

}

/****************************** SetComprScaleA ********************************/

/*
 * This function sets the ParArray data structure for the module.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetComprScaleA_BasilarM_DRNL(ParArrayPtr theComprScaleA)
{
	static const char	*funcName = "SetComprScaleA_BasilarM_DRNL";

	if (bMDRNLPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if (!CheckInit_ParArray(theComprScaleA, funcName)) {
		NotifyError("%s: ParArray structure not correctly set.",  funcName);
		return(FALSE);
	}
	bMDRNLPtr->updateProcessVariablesFlag = TRUE;
	bMDRNLPtr->comprScaleA = theComprScaleA;
	return(TRUE);

}

/****************************** SetComprScaleB ********************************/

/*
 * This function sets the ParArray data structure for the module.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetComprScaleB_BasilarM_DRNL(ParArrayPtr theComprScaleB)
{
	static const char	*funcName = "SetComprScaleB_BasilarM_DRNL";

	if (bMDRNLPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if (!CheckInit_ParArray(theComprScaleB, funcName)) {
		NotifyError("%s: ParArray structure not correctly set.",  funcName);
		return(FALSE);
	}
	bMDRNLPtr->updateProcessVariablesFlag = TRUE;
	bMDRNLPtr->comprScaleB = theComprScaleB;
	return(TRUE);

}

/****************************** SetComprExponent ******************************/

/*
 * This function sets the module's comprExponent parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetComprExponent_BasilarM_DRNL(double theComprExponent)
{
	static const char	*funcName = "SetComprExponent_BasilarM_DRNL";

	if (bMDRNLPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMDRNLPtr->comprExponentFlag = TRUE;
	bMDRNLPtr->comprExponent = theComprExponent;
	return(TRUE);

}

/****************************** SetLinGTCascade *******************************/

/*
 * This function sets the module's linGTCascade parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLinGTCascade_BasilarM_DRNL(int theLinGTCascade)
{
	static const char	*funcName = "SetLinGTCascade_BasilarM_DRNL";

	if (bMDRNLPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMDRNLPtr->linGTCascadeFlag = TRUE;
	bMDRNLPtr->linGTCascade = theLinGTCascade;
	return(TRUE);

}

/****************************** SetLinLPCascade *******************************/

/*
 * This function sets the module's linLPCascade parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLinLPCascade_BasilarM_DRNL(int theLinLPCascade)
{
	static const char	*funcName = "SetLinLPCascade_BasilarM_DRNL";

	if (bMDRNLPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMDRNLPtr->linLPCascadeFlag = TRUE;
	bMDRNLPtr->linLPCascade = theLinLPCascade;
	return(TRUE);

}

/****************************** SetLinCF **************************************/

/*
 * This function sets the ParArray data structure for the module.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetLinCF_BasilarM_DRNL(ParArrayPtr theLinCF)
{
	static const char	*funcName = "SetLinCF_BasilarM_DRNL";

	if (bMDRNLPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if (!CheckInit_ParArray(theLinCF, funcName)) {
		NotifyError("%s: ParArray structure not correctly set.",  funcName);
		return(FALSE);
	}
	bMDRNLPtr->updateProcessVariablesFlag = TRUE;
	bMDRNLPtr->linCF = theLinCF;
	return(TRUE);

}

/****************************** SetLinBwidth **********************************/

/*
 * This function sets the ParArray data structure for the module.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetLinBwidth_BasilarM_DRNL(ParArrayPtr theLinBwidth)
{
	static const char	*funcName = "SetLinBwidth_BasilarM_DRNL";

	if (bMDRNLPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if (!CheckInit_ParArray(theLinBwidth, funcName)) {
		NotifyError("%s: ParArray structure not correctly set.",  funcName);
		return(FALSE);
	}
	bMDRNLPtr->updateProcessVariablesFlag = TRUE;
	bMDRNLPtr->linBwidth = theLinBwidth;
	return(TRUE);

}

/****************************** SetLinScaleG **********************************/

/*
 * This function sets the ParArray data structure for the module.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetLinScaleG_BasilarM_DRNL(ParArrayPtr theLinScaleG)
{
	static const char	*funcName = "SetLinScaleG_BasilarM_DRNL";

	if (bMDRNLPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if (!CheckInit_ParArray(theLinScaleG, funcName)) {
		NotifyError("%s: ParArray structure not correctly set.",  funcName);
		return(FALSE);
	}
	bMDRNLPtr->updateProcessVariablesFlag = TRUE;
	bMDRNLPtr->linScaleG = theLinScaleG;
	return(TRUE);

}

/****************************** SetCFList *************************************/

/*
 * This function sets the CFList data structure for the filter bank.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetCFList_BasilarM_DRNL(CFListPtr theCFList)
{
	static const char	*funcName = "SetCFList_BasilarM_DRNL";

	if (bMDRNLPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if (!CheckPars_CFList(theCFList)) {
		NotifyError("%s: Centre frequency structure not correctly set.",
		  funcName);
		return(FALSE);
	}
	if (bMDRNLPtr->nonLinBwidth) {
		theCFList->bandwidthMode.Func = GetNonLinBandwidth_BasilarM_DRNL;
		if (!SetBandwidths_CFList(theCFList, "internal_static", NULL)) {
			NotifyError("%s: Failed to set bandwidth mode.", funcName);
			return(FALSE);
		}
		theCFList->bParList->pars[BANDWIDTH_PAR_MODE].enabled = FALSE;
	}
	if (bMDRNLPtr->theCFs != NULL)
		Free_CFList(&bMDRNLPtr->theCFs);
	bMDRNLPtr->updateProcessVariablesFlag = TRUE;
	bMDRNLPtr->theCFs = theCFList;
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
CheckPars_BasilarM_DRNL(void)
{
	static const char	*funcName = "CheckPars_BasilarM_DRNL";
	BOOLN	ok;

	ok = TRUE;
	if (bMDRNLPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!bMDRNLPtr->nonLinGTCascadeFlag) {
		NotifyError("%s: nonLinGTCascade variable not set.", funcName);
		ok = FALSE;
	}
	if (!bMDRNLPtr->nonLinLPCascadeFlag) {
		NotifyError("%s: nonLinLPCascade variable not set.", funcName);
		ok = FALSE;
	}
	if (!CheckInit_ParArray(bMDRNLPtr->nonLinBwidth, funcName)) {
		NotifyError("%s: Variable nonLinBwidth parameter array not correctly "
		  "set.", funcName);
		ok = FALSE;
	}
	if (!CheckInit_ParArray(bMDRNLPtr->comprScaleA, funcName)) {
		NotifyError("%s: Variable comprScaleA parameter array not correctly "
		  "set.", funcName);
		ok = FALSE;
	}
	if (!CheckInit_ParArray(bMDRNLPtr->comprScaleB, funcName)) {
		NotifyError("%s: Variable comprScaleB parameter array not correctly "
		  "set.", funcName);
		ok = FALSE;
	}
	if (!bMDRNLPtr->comprExponentFlag) {
		NotifyError("%s: comprExponent variable not set.", funcName);
		ok = FALSE;
	}
	if (!bMDRNLPtr->linGTCascadeFlag) {
		NotifyError("%s: linGTCascade variable not set.", funcName);
		ok = FALSE;
	}
	if (!bMDRNLPtr->linLPCascadeFlag) {
		NotifyError("%s: linLPCascade variable not set.", funcName);
		ok = FALSE;
	}
	if (!CheckInit_ParArray(bMDRNLPtr->linCF, funcName)) {
		NotifyError("%s: Variable linCF parameter array not correctly set.",
		  funcName);
		ok = FALSE;
	}
	if (!CheckInit_ParArray(bMDRNLPtr->linBwidth, funcName)) {
		NotifyError("%s: Variable linBwidth parameter array not correctly set.",
		  funcName);
		ok = FALSE;
	}
	if (!CheckInit_ParArray(bMDRNLPtr->linScaleG, funcName)) {
		NotifyError("%s: Variable linScaleG parameter array not correctly set.",
		  funcName);
		ok = FALSE;
	}
	if (!CheckPars_CFList(bMDRNLPtr->theCFs)) {
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
GetCFListPtr_BasilarM_DRNL(void)
{
	static const char	*funcName = "GetCFListPtr_BasilarM_DRNL";

	if (bMDRNLPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (bMDRNLPtr->theCFs == NULL) {
		NotifyError("%s: CFList data structure has not been correctly set.  "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(bMDRNLPtr->theCFs);

}

/****************************** GetNonLinBandwidth ****************************/

/*
 * This routine returns a pointer to the module's CFList data pointer.
 */

double
GetNonLinBandwidth_BasilarM_DRNL(BandwidthModePtr modePtr, double theCF)
{
	return(GetFitFuncValue_BasilarM_DRNL(bMDRNLPtr->nonLinBwidth, theCF));	

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the lUTEar.parsFile file pointer.
 */

BOOLN
PrintPars_BasilarM_DRNL(void)
{
	static const char	*funcName = "PrintPars_BasilarM_DRNL";

	if (!CheckPars_BasilarM_DRNL()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("DRNL Basilar Membrane Filter Module Parameters:-\n");
	DPrint("\tNonlinear gammatone filter cascade  = %d,\n",
	  bMDRNLPtr->nonLinGTCascade);
	DPrint("\tNonlinear low-pass filter cascade = %d,\n",
	  bMDRNLPtr->nonLinLPCascade);
	PrintPars_ParArray(bMDRNLPtr->nonLinBwidth);
	PrintPars_ParArray(bMDRNLPtr->comprScaleA);
	PrintPars_ParArray(bMDRNLPtr->comprScaleB);
	DPrint("\tCompression exponent  = %g,\n", bMDRNLPtr->comprExponent);
	DPrint("\tLinear gammatone filter cascade = %d,\n",
	  bMDRNLPtr->linGTCascade);
	DPrint("\tLinear low-pass filter cascade = %d,\n", bMDRNLPtr->linLPCascade);
	PrintPars_ParArray(bMDRNLPtr->linCF);
	PrintPars_ParArray(bMDRNLPtr->linBwidth);
	PrintPars_ParArray(bMDRNLPtr->linScaleG);
	PrintPars_CFList(bMDRNLPtr->theCFs);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_BasilarM_DRNL(char *fileName)
{
	static const char	*funcName = "ReadPars_BasilarM_DRNL";
	BOOLN	ok = TRUE;
	char	*filePath;
	int		nonLinGTCascade, nonLinLPCascade, linGTCascade, linLPCascade;
	double	comprExponent;
	FILE	*fp;
	CFListPtr	theCFs;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(filePath, "r")) == NULL) {
		NotifyError("%s: Cannot open data file '%s'.\n", funcName, fileName);
		return(FALSE);
	}
	DPrint("%s: Reading from '%s':\n", funcName, fileName);
	Init_ParFile();
	if (!GetPars_ParFile(fp, "%d", &nonLinGTCascade))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%d", &nonLinLPCascade))
		ok = FALSE;
	if (!ReadPars_ParArray(fp, bMDRNLPtr->nonLinBwidth))
		 ok = FALSE;
	if (!ReadPars_ParArray(fp, bMDRNLPtr->comprScaleA))
		 ok = FALSE;
	if (!ReadPars_ParArray(fp, bMDRNLPtr->comprScaleB))
		 ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &comprExponent))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%d", &linGTCascade))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%d", &linLPCascade))
		ok = FALSE;
	if (!ReadPars_ParArray(fp, bMDRNLPtr->linCF))
		 ok = FALSE;
	if (!ReadPars_ParArray(fp, bMDRNLPtr->linBwidth))
		 ok = FALSE;
	if (!ReadPars_ParArray(fp, bMDRNLPtr->linScaleG))
		 ok = FALSE;
	if ((theCFs = ReadPars_CFList(fp)) == NULL)
		 ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "
		  "parameter file '%s'.", funcName, fileName);
		return(FALSE);
	}
	if (!SetPars_BasilarM_DRNL(nonLinGTCascade, nonLinLPCascade,
	  bMDRNLPtr->nonLinBwidth, bMDRNLPtr->comprScaleA, bMDRNLPtr->comprScaleB,
	  comprExponent, linGTCascade, linLPCascade, bMDRNLPtr->linCF,
	  bMDRNLPtr->linBwidth, bMDRNLPtr->linScaleG, theCFs)) {
		NotifyError("%s: Could not set parameters.", funcName);
		return(FALSE);
	}
	return(TRUE);

}

/************************** SetParsPointer ************************************/

/*
 * This routine sets the global parameter pointer for the module to that
 * associated with the module instance.
 */

BOOLN
SetParsPointer_BasilarM_DRNL(ModulePtr theModule)
{
	static const char *funcName = "SetParsPointer_BasilarM_DRNL";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	bMDRNLPtr = (BMDRNLPtr) theModule->parsPtr;
	return(TRUE);

}

/************************** InitModule ****************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_BasilarM_DRNL(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_BasilarM_DRNL";

	if (!SetParsPointer_BasilarM_DRNL(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_BasilarM_DRNL(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = bMDRNLPtr;
	theModule->CheckPars = CheckPars_BasilarM_DRNL;
	theModule->Free = Free_BasilarM_DRNL;
	theModule->GetUniParListPtr = GetUniParListPtr_BasilarM_DRNL;
	theModule->PrintPars = PrintPars_BasilarM_DRNL;
	theModule->ReadPars = ReadPars_BasilarM_DRNL;
	theModule->RunProcess = RunModel_BasilarM_DRNL;
	theModule->SetParsPointer = SetParsPointer_BasilarM_DRNL;
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
CheckData_BasilarM_DRNL(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_BasilarM_DRNL";

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInit_SignalData(data->inSignal[0], funcName))
		return(FALSE);
	/*** Put additional checks here. ***/
	return(TRUE);

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 * It initialises the coefficients for all the filters involved in
 * the DRNL composite filter.
 * It assumes that the centre frequency list is set up correctly.
 * It also assumes that the output signal has already been initialised.
 * Two nonLinearGT filters are initialised, so that each has its own state
 * vectors.
 */

BOOLN
InitProcessVariables_BasilarM_DRNL(EarObjectPtr data)
{
	static const char *funcName = "InitProcessVariables_BasilarM_DRNL";
	int		i, j, cFIndex;
	double	sampleRate, centreFreq, linearFCentreFreq;
	
	if (bMDRNLPtr->updateProcessVariablesFlag || data->updateProcessFlag ||
	  bMDRNLPtr->theCFs->updateFlag) {
		FreeProcessVariables_BasilarM_DRNL();
		bMDRNLPtr->linearF = Init_EarObject("NULL");
		bMDRNLPtr->numChannels = data->outSignal->numChannels;
		if ((bMDRNLPtr->compressionA = (double *)
		  calloc(bMDRNLPtr->numChannels, sizeof(double))) == NULL) {
		 	NotifyError("%s: Out of memory (compressionA).", funcName);
		 	return(FALSE);
		}
		if ((bMDRNLPtr->compressionB = (double *)
		  calloc(bMDRNLPtr->numChannels, sizeof(double))) == NULL) {
		 	NotifyError("%s: Out of memory (compressionB).", funcName);
		 	return(FALSE);
		}
		if ((bMDRNLPtr->nonLinearGT1 = (GammaToneCoeffsPtr *) calloc(
		  bMDRNLPtr->numChannels, sizeof(GammaToneCoeffsPtr))) == NULL) {
			NotifyError("%s: Out of memory (nonLinearGT1).", funcName);
			return(FALSE);
		}
		if ((bMDRNLPtr->nonLinearGT2 = (GammaToneCoeffsPtr *) calloc(
		  bMDRNLPtr->numChannels, sizeof(GammaToneCoeffsPtr))) == NULL) {
			NotifyError("%s: Out of memory (nonLinearGT2).", funcName);
			return(FALSE);
		}
		if ((bMDRNLPtr->linearGT = (GammaToneCoeffsPtr *) calloc(
		 bMDRNLPtr->numChannels, sizeof(GammaToneCoeffsPtr))) == NULL) {
		 	NotifyError("%s: Out of memory (linearGT).", funcName);
		 	return(FALSE);
		}
		if ((bMDRNLPtr->nonLinearLP = (ContButtCoeffsPtr *) calloc(
		  bMDRNLPtr->numChannels, sizeof(ContButtCoeffsPtr))) == NULL) {
		 	NotifyError("%s: Out of memory (linearLP).", funcName);
		 	return(FALSE);
		}
		if ((bMDRNLPtr->linearLP = (ContButtCoeffsPtr *) calloc(
		  bMDRNLPtr->numChannels, sizeof(ContButtCoeffsPtr))) == NULL) {
		 	NotifyError("%s: Out of memory (linearLP).", funcName);
		 	return(FALSE);
		}
		sampleRate = 1.0 / data->inSignal[0]->dt;
		for (i = 0; i < data->outSignal->numChannels; i++) {
			cFIndex = i / data->inSignal[0]->interleaveLevel;
			centreFreq = bMDRNLPtr->theCFs->frequency[cFIndex];
			if ((bMDRNLPtr->nonLinearGT1[i] = InitGammaToneCoeffs_Filters(
			  centreFreq, bMDRNLPtr->theCFs->bandwidth[cFIndex],
			  bMDRNLPtr->nonLinGTCascade, sampleRate)) == NULL) {
				NotifyError("%s: Could not set nonLinearGT1[%d].", funcName, i);
				return(FALSE);
			}
			if ((bMDRNLPtr->nonLinearGT2[i] = InitGammaToneCoeffs_Filters(
			  centreFreq, bMDRNLPtr->theCFs->bandwidth[cFIndex],
			  bMDRNLPtr->nonLinGTCascade, sampleRate)) == NULL) {
				NotifyError("%s: Could not set nonLinearGT2[%d].", funcName, i);
				return(FALSE);
			}
			if ((bMDRNLPtr->nonLinearLP[i] = InitIIR2ContCoeffs_Filters(
			  bMDRNLPtr->nonLinLPCascade, centreFreq, data->inSignal[0]->dt,
			  LOWPASS)) == NULL) {
				NotifyError("%s: Could not set nonLinearLP[%d].", funcName, i);
				return(FALSE);
			}
			linearFCentreFreq = GetFitFuncValue_BasilarM_DRNL(bMDRNLPtr->linCF,
			  centreFreq);
			if ((bMDRNLPtr->linearGT[i] = InitGammaToneCoeffs_Filters(
			  linearFCentreFreq, GetFitFuncValue_BasilarM_DRNL(
			  bMDRNLPtr->linBwidth, centreFreq), bMDRNLPtr->linGTCascade,
			  sampleRate)) == NULL) {
				NotifyError("%s: Could not set linearGT[%d].", funcName, i);
				return(FALSE);
			}
			if ((bMDRNLPtr->linearLP[i] = InitIIR2ContCoeffs_Filters(
			  bMDRNLPtr->linLPCascade, linearFCentreFreq, data->inSignal[0]->dt,
			  LOWPASS)) == NULL) {
				NotifyError("%s: Could not set linearLP[%d].", funcName, i);
				return(FALSE);
			}
			bMDRNLPtr->compressionA[i] = GetFitFuncValue_BasilarM_DRNL(
			  bMDRNLPtr->comprScaleA, centreFreq);
			bMDRNLPtr->compressionB[i] = GetFitFuncValue_BasilarM_DRNL(
			  bMDRNLPtr->comprScaleB, centreFreq);
		}
		SetLocalInfoFlag_SignalData(data->outSignal, TRUE);
		SetInfoChannelTitle_SignalData(data->outSignal, "Frequency (Hz)");
		SetInfoChannelLabels_SignalData(data->outSignal,
		  bMDRNLPtr->theCFs->frequency);
		SetInfoCFArray_SignalData(data->outSignal,
		  bMDRNLPtr->theCFs->frequency);
		bMDRNLPtr->updateProcessVariablesFlag = FALSE;
		bMDRNLPtr->theCFs->updateFlag = FALSE;
	} else if (data->timeIndex == PROCESS_START_TIME) {
		for (i = 0; i < data->outSignal->numChannels; i++) {
			for (j = 0; j < bMDRNLPtr->nonLinGTCascade * 
			  FILTERS_NUM_GAMMAT_STATE_VARS_PER_FILTER; j++) {
				bMDRNLPtr->nonLinearGT1[i]->stateVector[j] = 0.0;
				bMDRNLPtr->nonLinearGT2[i]->stateVector[j] = 0.0;
			}
			for (j = 0; j < bMDRNLPtr->linGTCascade *
			  FILTERS_NUM_GAMMAT_STATE_VARS_PER_FILTER; j++)
				bMDRNLPtr->linearGT[i]->stateVector[j] = 0.0;
			for (j = 0; j < bMDRNLPtr->nonLinLPCascade *
			  FILTERS_NUM_CONTBUTT2_STATE_VARS; j++)
				bMDRNLPtr->nonLinearLP[i]->state[j] = 0.0;
			for (j = 0; j < bMDRNLPtr->linLPCascade *
			  FILTERS_NUM_CONTBUTT2_STATE_VARS; j++)
				bMDRNLPtr->linearLP[i]->state[j] = 0.0;
		}
	}
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 * It just checks the first set of coefficients, and assumes that the others
 * are in the same state.
 */

BOOLN
FreeProcessVariables_BasilarM_DRNL(void)
{
	int		i;

	if (bMDRNLPtr->linearF == NULL)
		return(FALSE);
	Free_EarObject(&bMDRNLPtr->linearF);
	for (i = 0; i < bMDRNLPtr->numChannels; i++) {
		FreeGammaToneCoeffs_Filters(&bMDRNLPtr->nonLinearGT1[i]);
		FreeGammaToneCoeffs_Filters(&bMDRNLPtr->nonLinearGT2[i]);
		FreeGammaToneCoeffs_Filters(&bMDRNLPtr->linearGT[i]);
		FreeIIR2ContCoeffs_Filters(&bMDRNLPtr->nonLinearLP[i]);
		FreeIIR2ContCoeffs_Filters(&bMDRNLPtr->linearLP[i]);
	}
	free(bMDRNLPtr->compressionA);
	free(bMDRNLPtr->compressionB);
	free(bMDRNLPtr->nonLinearGT1);
	free(bMDRNLPtr->nonLinearGT2);
	free(bMDRNLPtr->linearGT);
	free(bMDRNLPtr->nonLinearLP);
	free(bMDRNLPtr->linearLP);
	bMDRNLPtr->compressionA = NULL;
	bMDRNLPtr->compressionB = NULL;
	bMDRNLPtr->nonLinearGT1 = NULL;
	bMDRNLPtr->nonLinearGT2 = NULL;
	bMDRNLPtr->linearGT = NULL;
	bMDRNLPtr->nonLinearLP = NULL;
	bMDRNLPtr->linearLP = NULL;
	bMDRNLPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/**************************** ApplyScale **************************************/

/*
 * This routine applies the respective scales each channel of the signal.
 * It assumes that the signal and the parameter array have been correctly
 * initialised.
 *
 */

void
ApplyScale_BasilarM_DRNL(EarObjectPtr data, SignalDataPtr signal, ParArrayPtr p)
{
	int		chan;
	double	scale;
	ChanLen	i;
	ChanData	*dataPtr;

	for (chan = 0; chan < signal->numChannels; chan++) {
		scale = GetFitFuncValue_BasilarM_DRNL(p, data->outSignal->info.cFArray[
		  chan]);
		for (i = 0, dataPtr = signal->channel[chan]; i < signal->length; i++)
			*(dataPtr++) *= scale;
	}
	
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
RunModel_BasilarM_DRNL(EarObjectPtr data)
{
	static const char	*funcName = "RunModel_BasilarM_DRNL";
	int		totalChannels;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckPars_BasilarM_DRNL())
		return(FALSE);
	if (!CheckData_BasilarM_DRNL(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "DRNL Basilar Membrane Filtering");
	if (!CheckRamp_SignalData(data->inSignal[0])) {
		NotifyError("%s: Input signal not correctly initialised.", funcName);
		return(FALSE);
	}
	totalChannels = bMDRNLPtr->theCFs->numChannels * data->inSignal[
	  0]->numChannels;
	if (!InitOutFromInSignal_EarObject(data, totalChannels)) {
		NotifyError("%s: Output channels not initialised (%d).", funcName,
		  totalChannels);
		return(FALSE);
	}
	if (!InitProcessVariables_BasilarM_DRNL(data)) {
		NotifyError("%s: Could not initialise the process variables.",
		  funcName);
		return(FALSE);
	}
	/* "Manual" connection of original input signal to the linear filter. */
	bMDRNLPtr->linearF->inSignal[0] = data->inSignal[0];
	InitOutFromInSignal_EarObject(bMDRNLPtr->linearF, totalChannels);
	
	/* Filter signal */
	
	GammaTone_Filters(data->outSignal, bMDRNLPtr->nonLinearGT1);
	BrokenStick1Compression2_Filters(data->outSignal, bMDRNLPtr->compressionA,
	  bMDRNLPtr->compressionB, bMDRNLPtr->comprExponent);
	GammaTone_Filters(data->outSignal, bMDRNLPtr->nonLinearGT2);
	IIR2Cont_Filters(data->outSignal, bMDRNLPtr->nonLinearLP);
	
	GammaTone_Filters(bMDRNLPtr->linearF->outSignal, bMDRNLPtr->linearGT);
	IIR2Cont_Filters(bMDRNLPtr->linearF->outSignal, bMDRNLPtr->linearLP);
	
	ApplyScale_BasilarM_DRNL(data, bMDRNLPtr->linearF->outSignal, bMDRNLPtr->
	  linScaleG);
	Add_SignalData(data->outSignal, bMDRNLPtr->linearF->outSignal);
	
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

