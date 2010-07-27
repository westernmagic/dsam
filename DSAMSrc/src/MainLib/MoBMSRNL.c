/**********************
 *
 * File:		MoBMSRNL.c
 * Purpose:		The Stapes resonance non-linear (SRNL) Composite filter is
 *				used to model the response of the basilar membrane.
 * Comments:	This was revised from the DRNL filter
 * Authors:		L. P. O'Mard
 * Created:		23 Apr 2010
 * Updated:
 * Copyright:	(c) 2010 Lowel P. O'Mard
 *
 *********************
 *
 *  This file is part of DSAM.
 *
 *  DSAM is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  DSAM is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with DSAM.  If not, see <http://www.gnu.org/licenses/>.
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
#include "UtString.h"
#include "MoBMSRNL.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

BMSRNLPtr	bMSRNLPtr = NULL;

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
Free_BasilarM_SRNL(void)
{
	if (bMSRNLPtr == NULL)
		return(FALSE);
	FreeProcessVariables_BasilarM_SRNL();
	Free_ParArray(&bMSRNLPtr->nonLinBwidth);
	Free_ParArray(&bMSRNLPtr->nonLinScaleG);
	Free_ParArray(&bMSRNLPtr->comprScaleA);
	Free_ParArray(&bMSRNLPtr->comprScaleB);
	Free_ParArray(&bMSRNLPtr->linCF);
	Free_ParArray(&bMSRNLPtr->linBwidth);
	Free_ParArray(&bMSRNLPtr->linScaleG);
	Free_CFList(&bMSRNLPtr->theCFs);
	if (bMSRNLPtr->parList)
		FreeList_UniParMgr(&bMSRNLPtr->parList);
	if (bMSRNLPtr->parSpec == GLOBAL) {
		free(bMSRNLPtr);
		bMSRNLPtr = NULL;
	}
	return(TRUE);

}

/****************************** StapesModeList ********************************/

/*
 * This function defines the 'stapesMode' list array
 */

NameSpecifier *
StapesModeList_BasilarM_SRNL(int index)
{
	static NameSpecifier	modeList[] = {

			{ wxT("USER"),	BASILARM_SRNL_STAPESMODE_USER },
			{ 0,			BASILARM_SRNL_STAPESMODE_NULL },
		};
	return(&modeList[index]);

}

/****************************** GetFitFuncPars ********************************/

/*
 * This function returns the number of parameters for the respective fit
 * function mode. Using it helps maintain the correspondence
 * between the mode names.
 */

int
GetFitFuncPars_BasilarM_SRNL(int mode)
{
	static const WChar	*funcName = wxT("GetFitFuncPars_BasilarM_SRNL");

	switch (mode) {
	case GENERAL_FIT_FUNC_EXP1_MODE:
	case GENERAL_FIT_FUNC_LOG1_MODE:
	case GENERAL_FIT_FUNC_LOG2_MODE:
	case GENERAL_FIT_FUNC_LINEAR1_MODE:
		return(2);
	case GENERAL_FIT_FUNC_POLY1_MODE:
		return(3);
	default:
		NotifyError(wxT("%s: Mode not listed (%d), returning zero."), funcName,
		  mode);
	}
	return(0);

}

/****************************** GetFitFuncValue *******************************/

/*
 * This function returns the value for the respective fit function mode.
 * Using it helps maintain the correspondence between the mode names.
 */

Float
GetFitFuncValue_BasilarM_SRNL(ParArrayPtr p, Float linCF)
{
	static const WChar	*funcName = wxT("GetFitFuncValue_BasilarM_SRNL");

	switch (p->mode) {
	case GENERAL_FIT_FUNC_EXP1_MODE:
		return(p->params[0] * exp(p->params[1] * linCF));
	case GENERAL_FIT_FUNC_LINEAR1_MODE:
		return(p->params[0] + p->params[1] * linCF);
	case GENERAL_FIT_FUNC_LOG1_MODE:
		return(pow(10.0, p->params[0] + p->params[1] * log10(linCF)));
	case GENERAL_FIT_FUNC_LOG2_MODE:
		return(p->params[0] + p->params[1] * log(linCF));
	case GENERAL_FIT_FUNC_POLY1_MODE:
		return(p->params[0] + p->params[1] * linCF + p->params[2] * linCF *
		  linCF);
	default:
		NotifyError(wxT("%s: Mode (%d) not listed, returning zero."), funcName,
		  p->mode);
	}
	return(0.0);

}

/****************************** SetDefaultParArrayPars ************************/

/*
 * This routine sets the default values for the parameter array parameters.
 */

void
SetDefaultParArrayPars_BasilarM_SRNL(void)
{
	int		i;
	Float	nonLinBwidth[] = {0.8, 58};
	Float	nonLinScaleG[] = {1.0, 0.0};
	Float	comprScaleA[] = {1.67, 0.45};
	Float	comprScaleB[] = {-5.85, 0.875};
	Float	linCF[] = {0.14, 0.95};
	Float	linBwidth[] = {1.3, 0.53};
	Float	linScaleG[] = {5.48, -0.97};

	SetMode_ParArray(bMSRNLPtr->nonLinBwidth, wxT("Log_func1"));
	for (i = 0; i < bMSRNLPtr->nonLinBwidth->numParams; i++)
		bMSRNLPtr->nonLinBwidth->params[i] = nonLinBwidth[i];
	SetMode_ParArray(bMSRNLPtr->nonLinScaleG, wxT("Linear_func1"));
	for (i = 0; i < bMSRNLPtr->nonLinScaleG->numParams; i++)
		bMSRNLPtr->nonLinScaleG->params[i] = nonLinScaleG[i];
	SetMode_ParArray(bMSRNLPtr->comprScaleA, wxT("Log_func1"));
	for (i = 0; i < bMSRNLPtr->comprScaleA->numParams; i++)
		bMSRNLPtr->comprScaleA->params[i] = comprScaleA[i];
	SetMode_ParArray(bMSRNLPtr->comprScaleB, wxT("Log_func1"));
	for (i = 0; i < bMSRNLPtr->comprScaleB->numParams; i++)
		bMSRNLPtr->comprScaleB->params[i] = comprScaleB[i];
	SetMode_ParArray(bMSRNLPtr->linCF, wxT("Log_func1"));
	for (i = 0; i < bMSRNLPtr->linCF->numParams; i++)
		bMSRNLPtr->linCF->params[i] = linCF[i];
	SetMode_ParArray(bMSRNLPtr->linBwidth, wxT("Log_func1"));
	for (i = 0; i < bMSRNLPtr->linBwidth->numParams; i++)
		bMSRNLPtr->linBwidth->params[i] = linBwidth[i];
	SetMode_ParArray(bMSRNLPtr->linScaleG, wxT("Log_func1"));
	for (i = 0; i < bMSRNLPtr->linScaleG->numParams; i++)
		bMSRNLPtr->linScaleG->params[i] = linScaleG[i];

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
Init_BasilarM_SRNL(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_BasilarM_SRNL");

	if (parSpec == GLOBAL) {
		if (bMSRNLPtr != NULL)
			Free_BasilarM_SRNL();
		if ((bMSRNLPtr = (BMSRNLPtr) malloc(sizeof(BMSRNL))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (bMSRNLPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	bMSRNLPtr->parSpec = parSpec;
	bMSRNLPtr->updateProcessVariablesFlag = TRUE;
	bMSRNLPtr->nonLinGTCascade = 3;
	bMSRNLPtr->nonLinLPCascade = 4;
	if ((bMSRNLPtr->nonLinBwidth = Init_ParArray(wxT("NonLinBwidth"),
	  FitFuncModeList_NSpecLists(0), GetFitFuncPars_BasilarM_SRNL, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise NonLinBwidth parArray ")
		  wxT("structure"), funcName);
		Free_BasilarM_SRNL();
		return(FALSE);
	}
	if ((bMSRNLPtr->nonLinScaleG = Init_ParArray(wxT("nonLinScaleG"),
	  FitFuncModeList_NSpecLists(0), GetFitFuncPars_BasilarM_SRNL, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise nonLinScaleG parArray ")
		  wxT("structure"), funcName);
		Free_BasilarM_SRNL();
		return(FALSE);
	}
	if ((bMSRNLPtr->comprScaleA = Init_ParArray(wxT("ComprScaleA"),
	  FitFuncModeList_NSpecLists(0), GetFitFuncPars_BasilarM_SRNL, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise ComprScaleA parArray ")
		  wxT("structure"), funcName);
		Free_BasilarM_SRNL();
		return(FALSE);
	}
	if ((bMSRNLPtr->comprScaleB = Init_ParArray(wxT("ComprScaleB"),
	  FitFuncModeList_NSpecLists(0), GetFitFuncPars_BasilarM_SRNL, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise ComprScaleB parArray ")
		  wxT("structure"), funcName);
		Free_BasilarM_SRNL();
		return(FALSE);
	}
	bMSRNLPtr->comprExponent = 0.1;
	bMSRNLPtr->linGTCascade = 3;
	bMSRNLPtr->linLPCascade = 4;
	if ((bMSRNLPtr->linCF = Init_ParArray(wxT("LinCF"),
	  FitFuncModeList_NSpecLists(0), GetFitFuncPars_BasilarM_SRNL, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise LinCF parArray structure"),
		  funcName);
		Free_BasilarM_SRNL();
		return(FALSE);
	}
	if ((bMSRNLPtr->linBwidth = Init_ParArray(wxT("LinBwidth"),
	  FitFuncModeList_NSpecLists(0), GetFitFuncPars_BasilarM_SRNL, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise LinBwidth parArray ")
		  wxT("structure"), funcName);
		Free_BasilarM_SRNL();
		return(FALSE);
	}
	if ((bMSRNLPtr->linScaleG = Init_ParArray(wxT("linScaleG"),
	  FitFuncModeList_NSpecLists(0), GetFitFuncPars_BasilarM_SRNL, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise linScaleG parArray ")
		  wxT("structure"), funcName);
		Free_BasilarM_SRNL();
		return(FALSE);
	}
	bMSRNLPtr->stapesMode = BASILARM_SRNL_STAPESMODE_USER;
	bMSRNLPtr->stapesCF = -1.0;
	if ((bMSRNLPtr->theCFs = GenerateDefault_CFList(
	  CFLIST_DEFAULT_MODE_NAME, CFLIST_DEFAULT_CHANNELS,
	  CFLIST_DEFAULT_LOW_FREQ, CFLIST_DEFAULT_HIGH_FREQ, wxT("internal_static"),
	  GetNonLinBandwidth_BasilarM_SRNL)) == NULL) {
		NotifyError(wxT("%s: could not set default CFList."), funcName);
		return(FALSE);
	}
	SetDefaultParArrayPars_BasilarM_SRNL();

	if (!SetUniParList_BasilarM_SRNL()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_BasilarM_SRNL();
		return(FALSE);
	}
	bMSRNLPtr->numChannels = 0;
	bMSRNLPtr->compressionA = NULL;
	bMSRNLPtr->compressionB = NULL;
	bMSRNLPtr->nonLinearGT1 = NULL;
	bMSRNLPtr->nonLinearGT2 = NULL;
	bMSRNLPtr->linearGT = NULL;
	bMSRNLPtr->nonLinearLP = NULL;
	bMSRNLPtr->linearLP = NULL;
	bMSRNLPtr->linearF = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_BasilarM_SRNL(void)
{
	static const WChar *funcName = wxT("SetUniParList_BasilarM_SRNL");
	UniParPtr	pars;

	if ((bMSRNLPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  BM_SRNL_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = bMSRNLPtr->parList->pars;
	SetPar_UniParMgr(&pars[BM_SRNL_NONLINGTCASCADE], wxT("NL_GT_CASCADE"),
	  wxT("Nonlinear gammatone filter cascade."),
	  UNIPAR_INT,
	  &bMSRNLPtr->nonLinGTCascade, NULL,
	  (void * (*)) SetNonLinGTCascade_BasilarM_SRNL);
	SetPar_UniParMgr(&pars[BM_SRNL_NONLINLPCASCADE], wxT("NL_LP_CASCADE"),
	  wxT("Nonlinear low-pass filter cascade."),
	  UNIPAR_INT,
	  &bMSRNLPtr->nonLinLPCascade, NULL,
	  (void * (*)) SetNonLinLPCascade_BasilarM_SRNL);
	SetPar_UniParMgr(&pars[BM_SRNL_NONLINBWIDTH], wxT("NL_BW_VAR_FUNC"),
	  wxT("Non-linear bandwidth variable function (Hz vs non-linear CF)."),
	  UNIPAR_PARARRAY,
	  &bMSRNLPtr->nonLinBwidth, NULL,
	  (void * (*)) SetNonLinBwidth_BasilarM_SRNL);
	SetPar_UniParMgr(&pars[BM_SRNL_NONLINSCALEG], wxT("NL_SCALER"),
	  wxT("Nonlinear filter scale variable function (vs non-linear CF)."),
	  UNIPAR_PARARRAY,
	  &bMSRNLPtr->nonLinScaleG, NULL,
	  (void * (*)) SetNonLinScaleG_BasilarM_SRNL);
	SetPar_UniParMgr(&pars[BM_SRNL_COMPRSCALEA], wxT("COMP_A_VAR_FUNC"),
	  wxT("Compression A (linear) scale variable function (vs non-linear CF)."),
	  UNIPAR_PARARRAY,
	  &bMSRNLPtr->comprScaleA, NULL,
	  (void * (*)) SetComprScaleA_BasilarM_SRNL);
	SetPar_UniParMgr(&pars[BM_SRNL_COMPRSCALEB], wxT("COMP_B_VAR_FUNC"),
	  wxT("Compression b (gain) scale variable function (vs non-linear CF)."),
	  UNIPAR_PARARRAY,
	  &bMSRNLPtr->comprScaleB, NULL,
	  (void * (*)) SetComprScaleB_BasilarM_SRNL);
	SetPar_UniParMgr(&pars[BM_SRNL_COMPREXPONENT], wxT("COMP_N_EXPON"),
	  wxT("Compression exponent, n (units)."),
	  UNIPAR_REAL,
	  &bMSRNLPtr->comprExponent, NULL,
	  (void * (*)) SetComprExponent_BasilarM_SRNL);
	SetPar_UniParMgr(&pars[BM_SRNL_LINGTCASCADE], wxT("L_GT_CASCADE"),
	  wxT("Linear gammatone filter cascade."),
	  UNIPAR_INT,
	  &bMSRNLPtr->linGTCascade, NULL,
	  (void * (*)) SetLinGTCascade_BasilarM_SRNL);
	SetPar_UniParMgr(&pars[BM_SRNL_LINLPCASCADE], wxT("L_LP_CASCADE"),
	  wxT("Linear low-pass filter cascade."),
	  UNIPAR_INT,
	  &bMSRNLPtr->linLPCascade, NULL,
	  (void * (*)) SetLinLPCascade_BasilarM_SRNL);
	SetPar_UniParMgr(&pars[BM_SRNL_LINCF], wxT("L_CF_VAR_FUNC"),
	  wxT("Linear CF variable function (Hz vs linear CF)."),
	  UNIPAR_PARARRAY,
	  &bMSRNLPtr->linCF, NULL,
	  (void * (*)) SetLinCF_BasilarM_SRNL);
	SetPar_UniParMgr(&pars[BM_SRNL_LINBWIDTH], wxT("L_BW_VAR_FUNC"),
	  wxT("Linear bandwidth variable function (Hz vs non-linear CF)."),
	  UNIPAR_PARARRAY,
	  &bMSRNLPtr->linBwidth, NULL,
	  (void * (*)) SetLinBwidth_BasilarM_SRNL);
	SetPar_UniParMgr(&pars[BM_SRNL_LINSCALEG], wxT("L_SCALER"),
	  wxT("Linear filter scale variable function (vs non-linear CF)."),
	  UNIPAR_PARARRAY,
	  &bMSRNLPtr->linScaleG, NULL,
	  (void * (*)) SetLinScaleG_BasilarM_SRNL);
	SetPar_UniParMgr(&pars[BASILARM_SRNL_STAPESMODE], wxT("STAPES_MODE"),
	  wxT("Stapes resonance mode ('user' only at present)."),
	  UNIPAR_NAME_SPEC,
	  &bMSRNLPtr->stapesMode, StapesModeList_BasilarM_SRNL(0),
	  (void * (*)) SetStapesMode_BasilarM_SRNL);
	SetPar_UniParMgr(&pars[BASILARM_SRNL_STAPESCF], wxT("STAPES_CF"),
	  wxT("Stapes resonance CF ('user' mode only)."),
	  UNIPAR_REAL,
	  &bMSRNLPtr->stapesCF, NULL,
	  (void * (*)) SetStapesCF_BasilarM_SRNL);
	SetPar_UniParMgr(&pars[BM_SRNL_THECFS], wxT("CFLIST"),
	  wxT("Centre frequency specification."),
	  UNIPAR_CFLIST,
	  &bMSRNLPtr->theCFs, NULL,
	  (void * (*)) SetCFList_BasilarM_SRNL);

	SetEnabledPars_BasilarM_SRNL();
	return(TRUE);

}

/********************************* SetEnabledPars ****************************/

/*
 * This routine sets the parameter list so that the correct parameters
 * are enabled/disabled.
 */

BOOLN
SetEnabledPars_BasilarM_SRNL(void)
{
	static const WChar *funcName = wxT("SetEnabledPars_BasilarM_SRNL");
	BMSRNLPtr	p = bMSRNLPtr;

	if (p == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	switch (p->stapesMode) {
	case BASILARM_SRNL_STAPESMODE_USER:
		p->parList->pars[BASILARM_SRNL_STAPESCF].enabled = TRUE;
		break;
	default:
		p->parList->pars[BASILARM_SRNL_STAPESCF].enabled = FALSE;
	}
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_BasilarM_SRNL(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_BasilarM_SRNL");

	if (bMSRNLPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (bMSRNLPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(bMSRNLPtr->parList);

}

/****************************** SetNonLinGTCascade ****************************/

/*
 * This function sets the module's nonLinGTCascade parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNonLinGTCascade_BasilarM_SRNL(int theNonLinGTCascade)
{
	static const WChar	*funcName = wxT("SetNonLinGTCascade_BasilarM_SRNL");

	if (bMSRNLPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMSRNLPtr->nonLinGTCascade = theNonLinGTCascade;
	return(TRUE);

}

/****************************** SetNonLinLPCascade ****************************/

/*
 * This function sets the module's nonLinLPCascade parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNonLinLPCascade_BasilarM_SRNL(int theNonLinLPCascade)
{
	static const WChar	*funcName = wxT("SetNonLinLPCascade_BasilarM_SRNL");

	if (bMSRNLPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMSRNLPtr->nonLinLPCascade = theNonLinLPCascade;
	return(TRUE);

}

/****************************** SetNonLinBwidth *******************************/

/*
 * This function sets the ParArray data structure for the module.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetNonLinBwidth_BasilarM_SRNL(ParArrayPtr theNonLinBwidth)
{
	static const WChar	*funcName = wxT("SetNonLinBwidth_BasilarM_SRNL");

	if (bMSRNLPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if (!CheckInit_ParArray(theNonLinBwidth, funcName)) {
		NotifyError(wxT("%s: ParArray structure not correctly set."), funcName);
		return(FALSE);
	}
	bMSRNLPtr->updateProcessVariablesFlag = TRUE;
	if (bMSRNLPtr->theCFs) {
		bMSRNLPtr->theCFs->bandwidthMode.Func =
		  GetNonLinBandwidth_BasilarM_SRNL;
		if (!SetBandwidths_CFList(bMSRNLPtr->theCFs, wxT("internal_static"),
		  NULL)) {
			NotifyError(wxT("%s: Failed to set bandwidth mode."), funcName);
			return(FALSE);
		}
	}
	bMSRNLPtr->nonLinBwidth = theNonLinBwidth;
	return(TRUE);

}

/****************************** SetNonLinScaleG *******************************/

/*
 * This function sets the ParArray data structure for the module.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetNonLinScaleG_BasilarM_SRNL(ParArrayPtr theNonLinScaleG)
{
	static const WChar	*funcName = wxT("SetNonLinScaleG_BasilarM_SRNL");

	if (bMSRNLPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if (!CheckInit_ParArray(theNonLinScaleG, funcName)) {
		NotifyError(wxT("%s: ParArray structure not correctly set."), funcName);
		return(FALSE);
	}
	bMSRNLPtr->updateProcessVariablesFlag = TRUE;
	bMSRNLPtr->nonLinScaleG = theNonLinScaleG;
	return(TRUE);

}

/****************************** SetComprScaleA ********************************/

/*
 * This function sets the ParArray data structure for the module.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetComprScaleA_BasilarM_SRNL(ParArrayPtr theComprScaleA)
{
	static const WChar	*funcName = wxT("SetComprScaleA_BasilarM_SRNL");

	if (bMSRNLPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if (!CheckInit_ParArray(theComprScaleA, funcName)) {
		NotifyError(wxT("%s: ParArray structure not correctly set."), funcName);
		return(FALSE);
	}
	bMSRNLPtr->updateProcessVariablesFlag = TRUE;
	bMSRNLPtr->comprScaleA = theComprScaleA;
	return(TRUE);

}

/****************************** SetComprScaleB ********************************/

/*
 * This function sets the ParArray data structure for the module.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetComprScaleB_BasilarM_SRNL(ParArrayPtr theComprScaleB)
{
	static const WChar	*funcName = wxT("SetComprScaleB_BasilarM_SRNL");

	if (bMSRNLPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if (!CheckInit_ParArray(theComprScaleB, funcName)) {
		NotifyError(wxT("%s: ParArray structure not correctly set."), funcName);
		return(FALSE);
	}
	bMSRNLPtr->updateProcessVariablesFlag = TRUE;
	bMSRNLPtr->comprScaleB = theComprScaleB;
	return(TRUE);

}

/****************************** SetComprExponent ******************************/

/*
 * This function sets the module's comprExponent parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetComprExponent_BasilarM_SRNL(Float theComprExponent)
{
	static const WChar	*funcName = wxT("SetComprExponent_BasilarM_SRNL");

	if (bMSRNLPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMSRNLPtr->comprExponent = theComprExponent;
	return(TRUE);

}

/****************************** SetLinGTCascade *******************************/

/*
 * This function sets the module's linGTCascade parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLinGTCascade_BasilarM_SRNL(int theLinGTCascade)
{
	static const WChar	*funcName = wxT("SetLinGTCascade_BasilarM_SRNL");

	if (bMSRNLPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMSRNLPtr->linGTCascade = theLinGTCascade;
	return(TRUE);

}

/****************************** SetLinLPCascade *******************************/

/*
 * This function sets the module's linLPCascade parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLinLPCascade_BasilarM_SRNL(int theLinLPCascade)
{
	static const WChar	*funcName = wxT("SetLinLPCascade_BasilarM_SRNL");

	if (bMSRNLPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMSRNLPtr->linLPCascade = theLinLPCascade;
	return(TRUE);

}

/****************************** SetLinCF **************************************/

/*
 * This function sets the ParArray data structure for the module.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetLinCF_BasilarM_SRNL(ParArrayPtr theLinCF)
{
	static const WChar	*funcName = wxT("SetLinCF_BasilarM_SRNL");

	if (bMSRNLPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if (!CheckInit_ParArray(theLinCF, funcName)) {
		NotifyError(wxT("%s: ParArray structure not correctly set."), funcName);
		return(FALSE);
	}
	bMSRNLPtr->updateProcessVariablesFlag = TRUE;
	bMSRNLPtr->linCF = theLinCF;
	return(TRUE);

}

/****************************** SetLinBwidth **********************************/

/*
 * This function sets the ParArray data structure for the module.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetLinBwidth_BasilarM_SRNL(ParArrayPtr theLinBwidth)
{
	static const WChar	*funcName = wxT("SetLinBwidth_BasilarM_SRNL");

	if (bMSRNLPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if (!CheckInit_ParArray(theLinBwidth, funcName)) {
		NotifyError(wxT("%s: ParArray structure not correctly set."), funcName);
		return(FALSE);
	}
	bMSRNLPtr->updateProcessVariablesFlag = TRUE;
	bMSRNLPtr->linBwidth = theLinBwidth;
	return(TRUE);

}

/****************************** SetLinScaleG **********************************/

/*
 * This function sets the ParArray data structure for the module.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetLinScaleG_BasilarM_SRNL(ParArrayPtr theLinScaleG)
{
	static const WChar	*funcName = wxT("SetLinScaleG_BasilarM_SRNL");

	if (bMSRNLPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if (!CheckInit_ParArray(theLinScaleG, funcName)) {
		NotifyError(wxT("%s: ParArray structure not correctly set."), funcName);
		return(FALSE);
	}
	bMSRNLPtr->updateProcessVariablesFlag = TRUE;
	bMSRNLPtr->linScaleG = theLinScaleG;
	return(TRUE);

}

/****************************** SetStapesMode *********************************/

/*
 * This function sets the module's stapesMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetStapesMode_BasilarM_SRNL(WChar * theStapesMode)
{
	static const WChar	*funcName = wxT("SetStapesMode_BasilarM_SRNL");
	int		specifier;

	if (bMSRNLPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theStapesMode,
		StapesModeList_BasilarM_SRNL(0))) == BASILARM_SRNL_STAPESMODE_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theStapesMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMSRNLPtr->stapesMode = specifier;
	SetEnabledPars_BasilarM_SRNL();
	return(TRUE);

}

/****************************** SetStapesCF ***********************************/

/*
 * This function sets the module's stapesCF parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetStapesCF_BasilarM_SRNL(Float theStapesCF)
{
	static const WChar	*funcName = wxT("SetStapesCF_BasilarM_SRNL");

	if (bMSRNLPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMSRNLPtr->stapesCF = theStapesCF;
	return(TRUE);

}

/****************************** SetCFList *************************************/

/*
 * This function sets the CFList data structure for the filter bank.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetCFList_BasilarM_SRNL(CFListPtr theCFList)
{
	static const WChar	*funcName = wxT("SetCFList_BasilarM_SRNL");

	if (bMSRNLPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if (!CheckPars_CFList(theCFList)) {
		NotifyError(wxT("%s: Centre frequency structure not correctly set."),
		  funcName);
		return(FALSE);
	}
	if (bMSRNLPtr->nonLinBwidth) {
		theCFList->bandwidthMode.Func = GetNonLinBandwidth_BasilarM_SRNL;
		if (!SetBandwidths_CFList(theCFList, wxT("internal_static"), NULL)) {
			NotifyError(wxT("%s: Failed to set bandwidth mode."), funcName);
			return(FALSE);
		}
		theCFList->bParList->pars[BANDWIDTH_PAR_MODE].enabled = FALSE;
	}
	if (bMSRNLPtr->theCFs != NULL)
		Free_CFList(&bMSRNLPtr->theCFs);
	bMSRNLPtr->updateProcessVariablesFlag = TRUE;
	bMSRNLPtr->theCFs = theCFList;
	return(TRUE);

}

/****************************** GetCFListPtr **********************************/

/*
 * This routine returns a pointer to the module's CFList data pointer.
 */

CFListPtr
GetCFListPtr_BasilarM_SRNL(void)
{
	static const WChar	*funcName = wxT("GetCFListPtr_BasilarM_SRNL");

	if (bMSRNLPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(NULL);
	}
	if (bMSRNLPtr->theCFs == NULL) {
		NotifyError(wxT("%s: CFList data structure has not been correctly ")
		  wxT("set.  NULL returned."), funcName);
		return(NULL);
	}
	return(bMSRNLPtr->theCFs);

}

/****************************** GetNonLinBandwidth ****************************/

/*
 * This routine returns a pointer to the module's CFList data pointer.
 */

Float
GetNonLinBandwidth_BasilarM_SRNL(BandwidthModePtr modePtr, Float theCF)
{
	return(GetFitFuncValue_BasilarM_SRNL(bMSRNLPtr->nonLinBwidth, theCF));

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the lUTEar.parsFile file pointer.
 */

BOOLN
PrintPars_BasilarM_SRNL(void)
{
	DPrint(wxT("SRNL Basilar Membrane Filter Module Parameters:-\n"));
	DPrint(wxT("\tNonlinear gammatone filter cascade  = %d,\n"),
	  bMSRNLPtr->nonLinGTCascade);
	DPrint(wxT("\tNonlinear low-pass filter cascade = %d,\n"),
	  bMSRNLPtr->nonLinLPCascade);
	PrintPars_ParArray(bMSRNLPtr->nonLinBwidth);
	PrintPars_ParArray(bMSRNLPtr->nonLinScaleG);
	PrintPars_ParArray(bMSRNLPtr->comprScaleA);
	PrintPars_ParArray(bMSRNLPtr->comprScaleB);
	DPrint(wxT("\tCompression exponent  = %g,\n"), bMSRNLPtr->comprExponent);
	DPrint(wxT("\tLinear gammatone filter cascade = %d,\n"),
	  bMSRNLPtr->linGTCascade);
	DPrint(wxT("\tLinear low-pass filter cascade = %d,\n"), bMSRNLPtr->
	  linLPCascade);
	PrintPars_ParArray(bMSRNLPtr->linCF);
	PrintPars_ParArray(bMSRNLPtr->linBwidth);
	PrintPars_ParArray(bMSRNLPtr->linScaleG);
	DPrint(wxT("\tStapes mode = %s,\n"), StapesModeList_BasilarM_SRNL(
	  bMSRNLPtr->stapesMode)->name);
	if (bMSRNLPtr->stapesMode == BASILARM_SRNL_STAPESMODE_USER)
		DPrint(wxT("\tstapesCF = %f Hz,\n"), bMSRNLPtr->stapesCF);
	PrintPars_CFList(bMSRNLPtr->theCFs);
	return(TRUE);

}

/************************** SetParsPointer ************************************/

/*
 * This routine sets the global parameter pointer for the module to that
 * associated with the module instance.
 */

BOOLN
SetParsPointer_BasilarM_SRNL(ModulePtr theModule)
{
	static const WChar *funcName = wxT("SetParsPointer_BasilarM_SRNL");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	bMSRNLPtr = (BMSRNLPtr) theModule->parsPtr;
	return(TRUE);

}

/************************** InitModule ****************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_BasilarM_SRNL(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_BasilarM_SRNL");

	if (!SetParsPointer_BasilarM_SRNL(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_BasilarM_SRNL(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = bMSRNLPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->Free = Free_BasilarM_SRNL;
	theModule->GetUniParListPtr = GetUniParListPtr_BasilarM_SRNL;
	theModule->PrintPars = PrintPars_BasilarM_SRNL;
	theModule->RunProcess = RunModel_BasilarM_SRNL;
	theModule->SetParsPointer = SetParsPointer_BasilarM_SRNL;
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
CheckData_BasilarM_SRNL(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_BasilarM_SRNL");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	/*** Put additional checks here. ***/
	return(TRUE);

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 * It initialises the coefficients for all the filters involved in
 * the SRNL composite filter.
 * It assumes that the centre frequency list is set up correctly.
 * It also assumes that the output signal has already been initialised.
 * Two nonLinearGT filters are initialised, so that each has its own state
 * vectors.
 */

BOOLN
InitProcessVariables_BasilarM_SRNL(EarObjectPtr data)
{
	static const WChar *funcName = wxT("InitProcessVariables_BasilarM_SRNL");
	int		i, j, cFIndex;
	Float	sampleRate, centreFreq, linearFCentreFreq;
	BMSRNLPtr	p = bMSRNLPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag ||
	  p->theCFs->updateFlag) {
		FreeProcessVariables_BasilarM_SRNL();
		p->linearF = Init_EarObject(wxT("NULL"));
		p->numChannels = _OutSig_EarObject(data)->numChannels;
		if (!InitSubProcessList_EarObject(data, BM_SRNL_NUM_SUB_PROCESSES)) {
			NotifyError(wxT("%s: Could not initialise %d sub-process list for ")
			  wxT("process."), funcName, BM_SRNL_NUM_SUB_PROCESSES);
			return(FALSE);
		}
		data->subProcessList[BM_SRNL_LINEARF] = p->linearF;
		if ((p->compressionA = (Float *) calloc(p->numChannels, sizeof(
		  Float))) == NULL) {
		 	NotifyError(wxT("%s: Out of memory (compressionA)."), funcName);
		 	return(FALSE);
		}
		if ((p->compressionB = (Float *) calloc(p->numChannels, sizeof(
		  Float))) == NULL) {
		 	NotifyError(wxT("%s: Out of memory (compressionB)."), funcName);
		 	return(FALSE);
		}
		if ((p->nonLinearGT1 = (GammaToneCoeffsPtr *) calloc(p->numChannels,
		  sizeof(GammaToneCoeffsPtr))) == NULL) {
			NotifyError(wxT("%s: Out of memory (nonLinearGT1)."), funcName);
			return(FALSE);
		}
		if ((p->nonLinearGT2 = (GammaToneCoeffsPtr *) calloc(p->numChannels,
		  sizeof(GammaToneCoeffsPtr))) == NULL) {
			NotifyError(wxT("%s: Out of memory (nonLinearGT2)."), funcName);
			return(FALSE);
		}
		if ((p->linearGT = (GammaToneCoeffsPtr *) calloc(p->numChannels,
		  sizeof(GammaToneCoeffsPtr))) == NULL) {
		 	NotifyError(wxT("%s: Out of memory (linearGT)."), funcName);
		 	return(FALSE);
		}
		if ((p->nonLinLPCascade > 0) && ((p->nonLinearLP = (ContButtCoeffsPtr *)
		  calloc( p->numChannels, sizeof(ContButtCoeffsPtr))) == NULL)) {
		 	NotifyError(wxT("%s: Out of memory (linearLP)."), funcName);
		 	return(FALSE);
		}
		if ((p->linLPCascade > 0) && ((p->linearLP = (ContButtCoeffsPtr *)
		  calloc(p->numChannels, sizeof(ContButtCoeffsPtr))) == NULL)) {
		 	NotifyError(wxT("%s: Out of memory (linearLP)."), funcName);
		 	return(FALSE);
		}
		sampleRate = 1.0 / _InSig_EarObject(data, 0)->dt;
		for (i = 0; i < _OutSig_EarObject(data)->numChannels; i++) {
			cFIndex = i / _InSig_EarObject(data, 0)->interleaveLevel;
			centreFreq = p->theCFs->frequency[cFIndex];
			if ((p->nonLinearGT1[i] = InitGammaToneCoeffs_Filters(p->stapesCF,
			  GetFitFuncValue_BasilarM_SRNL(p->nonLinBwidth, p->stapesCF),
			  p->nonLinGTCascade, sampleRate)) == NULL) {
				NotifyError(wxT("%s: Could not set nonLinearGT1[%d]."),
				  funcName, i);
				return(FALSE);
			}
			if ((p->nonLinearGT2[i] = InitGammaToneCoeffs_Filters(p->stapesCF,
			  GetFitFuncValue_BasilarM_SRNL(p->nonLinBwidth, p->stapesCF),
			  p->nonLinGTCascade, sampleRate)) == NULL) {
				NotifyError(wxT("%s: Could not set nonLinearGT2[%d]."),
				  funcName, i);
				return(FALSE);
			}
			if (p->nonLinearLP && ((p->nonLinearLP[i] =
			  InitIIR2ContCoeffs_Filters(p->nonLinLPCascade, p->stapesCF,
			  _InSig_EarObject(data, 0)->dt, LOWPASS)) == NULL)) {
				NotifyError(wxT("%s: Could not set nonLinearLP[%d]."), funcName,
				  i);
				return(FALSE);
			}
			linearFCentreFreq = GetFitFuncValue_BasilarM_SRNL(p->linCF,
			  centreFreq);
			if ((p->linearGT[i] = InitGammaToneCoeffs_Filters(linearFCentreFreq,
			  GetFitFuncValue_BasilarM_SRNL(p->linBwidth, centreFreq),
			  p->linGTCascade, sampleRate)) == NULL) {
				NotifyError(wxT("%s: Could not set linearGT[%d]."), funcName,
				  i);
				return(FALSE);
			}
			if (p->linearLP && ((p->linearLP[i] = InitIIR2ContCoeffs_Filters(
			  p->linLPCascade, linearFCentreFreq, _InSig_EarObject(data, 0)->dt,
			  LOWPASS)) == NULL)) {
				NotifyError(wxT("%s: Could not set linearLP[%d]."), funcName,
				  i);
				return(FALSE);
			}
			p->compressionA[i] = GetFitFuncValue_BasilarM_SRNL(p->comprScaleA,
			  p->stapesCF);
			p->compressionB[i] = GetFitFuncValue_BasilarM_SRNL(p->comprScaleB,
			  p->stapesCF);
		}
		SetLocalInfoFlag_SignalData(_OutSig_EarObject(data), TRUE);
		SetInfoChannelTitle_SignalData(_OutSig_EarObject(data), wxT(
		  "Frequency (Hz)"));
		SetInfoChannelLabels_SignalData(_OutSig_EarObject(data), p->theCFs->
		  frequency);
		SetInfoCFArray_SignalData(_OutSig_EarObject(data), p->theCFs->
		  frequency);
		p->updateProcessVariablesFlag = FALSE;
		p->theCFs->updateFlag = FALSE;
	} else if (data->timeIndex == PROCESS_START_TIME) {
		for (i = 0; i < _OutSig_EarObject(data)->numChannels; i++) {
			for (j = 0; j < p->nonLinGTCascade *
			  FILTERS_NUM_GAMMAT_STATE_VARS_PER_FILTER; j++) {
				p->nonLinearGT1[i]->stateVector[j] = 0.0;
				p->nonLinearGT2[i]->stateVector[j] = 0.0;
			}
			for (j = 0; j < p->linGTCascade *
			  FILTERS_NUM_GAMMAT_STATE_VARS_PER_FILTER; j++)
				p->linearGT[i]->stateVector[j] = 0.0;
			for (j = 0; j < p->nonLinLPCascade *
			  FILTERS_NUM_CONTBUTT2_STATE_VARS; j++)
				p->nonLinearLP[i]->state[j] = 0.0;
			for (j = 0; j < p->linLPCascade *
			  FILTERS_NUM_CONTBUTT2_STATE_VARS; j++)
				p->linearLP[i]->state[j] = 0.0;
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
FreeProcessVariables_BasilarM_SRNL(void)
{
	int		i;

	if (bMSRNLPtr->linearF)
		Free_EarObject(&bMSRNLPtr->linearF);
	for (i = 0; i < bMSRNLPtr->numChannels; i++) {
		if (bMSRNLPtr->nonLinearGT1)
			FreeGammaToneCoeffs_Filters(&bMSRNLPtr->nonLinearGT1[i]);
		if (bMSRNLPtr->nonLinearGT2)
			FreeGammaToneCoeffs_Filters(&bMSRNLPtr->nonLinearGT2[i]);
		if (bMSRNLPtr->linearGT)
			FreeGammaToneCoeffs_Filters(&bMSRNLPtr->linearGT[i]);
	}
	if (bMSRNLPtr->nonLinearLP)
		for (i = 0; i < bMSRNLPtr->numChannels; i++)
			FreeIIR2ContCoeffs_Filters(&bMSRNLPtr->nonLinearLP[i]);
	if (bMSRNLPtr->linearLP)
		for (i = 0; i < bMSRNLPtr->numChannels; i++)
			FreeIIR2ContCoeffs_Filters(&bMSRNLPtr->linearLP[i]);
	if (bMSRNLPtr->compressionA)
		free(bMSRNLPtr->compressionA);
	if (bMSRNLPtr->compressionB)
		free(bMSRNLPtr->compressionB);
	if (bMSRNLPtr->nonLinearGT1)
		free(bMSRNLPtr->nonLinearGT1);
	if (bMSRNLPtr->nonLinearGT2)
		free(bMSRNLPtr->nonLinearGT2);
	if (bMSRNLPtr->linearGT)
		free(bMSRNLPtr->linearGT);
	if (bMSRNLPtr->nonLinearLP)
		free(bMSRNLPtr->nonLinearLP);
	if (bMSRNLPtr->linearLP)
		free(bMSRNLPtr->linearLP);
	bMSRNLPtr->compressionA = NULL;
	bMSRNLPtr->compressionB = NULL;
	bMSRNLPtr->nonLinearGT1 = NULL;
	bMSRNLPtr->nonLinearGT2 = NULL;
	bMSRNLPtr->linearGT = NULL;
	bMSRNLPtr->nonLinearLP = NULL;
	bMSRNLPtr->linearLP = NULL;
	bMSRNLPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/**************************** ApplyScale **************************************/

/*
 * This routine applies the respective scales each channel of the signal.
 * It assumes that the signal and the parameter array have been correctly
 * initialised.
 * This function returns without scaling if the default linear scale of 1.0 is
 * set.
 */

void
ApplyScale_BasilarM_SRNL(EarObjectPtr data, SignalDataPtr signal, ParArrayPtr p)
{
	int		chan;
	Float	scale;
	ChanLen	i;
	ChanData	*dataPtr;

	if ((p->mode == GENERAL_FIT_FUNC_LINEAR1_MODE) && (p->params[0] == 1.0) &&
	  (p->params[1] == 0.0))
		return;
	for (chan = _OutSig_EarObject(data)->offset; chan < signal->numChannels;
	  chan++) {
		scale = GetFitFuncValue_BasilarM_SRNL(p, _OutSig_EarObject(data)->info.
		  cFArray[chan]);
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
RunModel_BasilarM_SRNL(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("RunModel_BasilarM_SRNL");
	uShort	totalChannels;
	EarObjectPtr	linearF;
	SignalDataPtr	outSignal;
	BMSRNLPtr	p = bMSRNLPtr;

	if (!data->threadRunFlag) {
		if (!CheckData_BasilarM_SRNL(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("SRNL Basilar Membrane Filtering"));
		if (!CheckRamp_SignalData(_InSig_EarObject(data, 0))) {
			NotifyError(wxT("%s: Input signal not correctly initialised."),
			  funcName);
			return(FALSE);
		}
		totalChannels = p->theCFs->numChannels * _InSig_EarObject(data, 0)->
		  numChannels;
		if (!InitOutTypeFromInSignal_EarObject(data, totalChannels)) {
			NotifyError(wxT("%s: Output channels not initialised (%d)."),
			  funcName, totalChannels);
			return(FALSE);
		}
		if (!InitProcessVariables_BasilarM_SRNL(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		TempInputConnection_EarObject(data, p->linearF, 1);
		/*InitOutFromInSignal_EarObject(p->linearF, totalChannels);*/
		InitOutTypeFromInSignal_EarObject(p->linearF, totalChannels);
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	InitOutDataFromInSignal_EarObject(data);
	linearF = data->subProcessList[BM_SRNL_LINEARF];
	InitOutDataFromInSignal_EarObject(linearF);
	outSignal = _OutSig_EarObject(data);

	/* Filter signal */
	ApplyScale_BasilarM_SRNL(data, outSignal, p->nonLinScaleG);
	GammaTone_Filters(outSignal, p->nonLinearGT1);
	ApplyScale_BasilarM_SRNL(data, outSignal, p->nonLinScaleG);
	BrokenStick1Compression2_Filters(outSignal, p->compressionA, p->
	  compressionB, p->comprExponent);
	GammaTone_Filters(outSignal, p->nonLinearGT2);
	if (p->nonLinearLP)
		IIR2Cont_Filters(outSignal, p->nonLinearLP);

	GammaTone_Filters(linearF->outSignal, p->linearGT);
	if (p->linearLP)
		IIR2Cont_Filters(linearF->outSignal, p->linearLP);

	ApplyScale_BasilarM_SRNL(data, linearF->outSignal, p->linScaleG);
	Add_SignalData(outSignal, linearF->outSignal);

	SetProcessContinuity_EarObject(data);
	return(TRUE);

}
