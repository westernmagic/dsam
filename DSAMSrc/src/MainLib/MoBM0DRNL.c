/******************
 *
 * File:		MoBM0DRNL.c
 * Purpose:		Test module for the DRNL Composite filter, used to model the
 *				non-linear response of the Basilar Membrane.
 *				28-05-97 LPO: Corrected linearFLPCutOffScale parameter - it
 *				wasn't being used, and now a negative or zero value disables
 *				the filter altogether.
 *				06-06-97 LPO: added outputScale parameter.
 *				11-06-97 LPO: Introduced new compression function.
 *				27-05-98 LPO: Introduced facility to use different compression
 *				modes.
 *				04-10-98 LPO: Introduced the linear filter order parameter.
 *				11-11-98 LPO: Implemented universal parameter list.  The
 *				compressionPars array had to be done by hand.
 *				23-04-99 LPO: The 'compressionParsFlag' arrays was not being
 *				correctly initialised to 'NULL' in "Init_".
 * Authors:		L. P. O'Mard
 * Created:		11 Mar 1993
 * Updated:		23 Apr 1999
 * Copyright:	(c) 1999, University of Essex.
 * 
 ******************/
 
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
#include "MoBM0DRNL.h"

/******************************************************************************/
/*************************** Global Variables *********************************/
/******************************************************************************/

BM0DRNLPtr	bM0DRNLPtr = NULL;

/******************************************************************************/
/**************************** Subroutines & functions *************************/
/******************************************************************************/

/****************************** InitCompressionModeList ***********************/

/*
 * This routine intialises the Compression Mode list array.
 */

BOOLN
InitCompressionModeList_BasilarM_DRNL_Test(void)
{
	static NameSpecifier	compressionModeList[] = {

					{"DISABLED",		DRNLT_COMPRESSION_MODE_DISABLED },
					{"ORIGINAL",		DRNLT_COMPRESSION_MODE_ORIGINAL },
					{"INV_POWER",		DRNLT_COMPRESSION_MODE_INVPOWER },
					{"BROKEN_STICK1",	DRNLT_COMPRESSION_MODE_BROKENSTICK1 },
					{"UPTON_BSTICK",	DRNLT_COMPRESSION_MODE_UPTON_BSTICK },
					{"",				DRNLT_COMPRESSION_MODE_NULL }

				};


	bM0DRNLPtr->compressionModeList = compressionModeList;
	return(TRUE);

}

/****************************** InitLPFilterModeList **************************/

/*
 * This function initialises the 'lPFilterMode' list array
 */

BOOLN
InitLPFilterModeList_BasilarM_DRNL_Test(void)
{
	static NameSpecifier	modeList[] = {

			{ "BUTTERWORTH",	BASILARM_DRNL_TEST_BUTTERWORTH_LPFILTERMODE },
			{ "BEAUCHAMP",		BASILARM_DRNL_TEST_BEAUCHAMP_LPFILTERMODE },
			{ "",	BASILARM_DRNL_TEST_LPFILTERMODE_NULL },
		};
	bM0DRNLPtr->lPFilterModeList = modeList;
	return(TRUE);

}

/****************************** SetDefaultCompressionMode *********************/

/*
 * This sets the default compression mode and associated parameters.
 */

void
SetDefaultCompressionMode_BasilarM_DRNL_Test(void)
{
	int		i;
	double	defValues[] = { 50.0, 0.008, 0.25};

	SetCompressionMode_BasilarM_DRNL_Test("broken_stick1");
	for (i = 0; i < bM0DRNLPtr->numCompressionPars; i++) {
		bM0DRNLPtr->compressionPars[i] = defValues[i];
		bM0DRNLPtr->compressionParsFlag[i] = TRUE;
	}

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
Init_BasilarM_DRNL_Test(ParameterSpecifier parSpec)
{
	static const char *funcName = "Init_BasilarM_DRNL_Test";

	if (parSpec == GLOBAL) {
		if (bM0DRNLPtr != NULL)
			Free_BasilarM_DRNL_Test();
		if ((bM0DRNLPtr = (BM0DRNLPtr) malloc(sizeof(BM0DRNL))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (bM0DRNLPtr == NULL) { 
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	bM0DRNLPtr->parSpec = parSpec;
	bM0DRNLPtr->updateProcessVariablesFlag = TRUE;
	bM0DRNLPtr->nonLinGTCascadeFlag = TRUE;
	bM0DRNLPtr->nonLinLPCascadeFlag = TRUE;
	bM0DRNLPtr->compressionModeFlag = TRUE;
	bM0DRNLPtr->lPFilterModeFlag = TRUE;
	bM0DRNLPtr->compressionParsFlag = NULL;
	bM0DRNLPtr->linGTCascadeFlag = TRUE;
	bM0DRNLPtr->linLPCascadeFlag = TRUE;
	bM0DRNLPtr->linCFFlag = TRUE;
	bM0DRNLPtr->linBwidthFlag = TRUE;
	bM0DRNLPtr->linScalerFlag = TRUE;
	bM0DRNLPtr->nonLinGTCascade = 3;
	bM0DRNLPtr->nonLinLPCascade = 3;
	bM0DRNLPtr->compressionPars = NULL;
	bM0DRNLPtr->lPFilterMode = BASILARM_DRNL_TEST_BUTTERWORTH_LPFILTERMODE;
	bM0DRNLPtr->linGTCascade = 2;
	bM0DRNLPtr->linLPCascade = 4;
	bM0DRNLPtr->linCF = 700.0;
	bM0DRNLPtr->linBwidth = 130.0;
	bM0DRNLPtr->linScaler = 83.0;
	if ((bM0DRNLPtr->theCFs = GenerateDefault_CFList("single", 1, 750.0,
	  CFLIST_DEFAULT_HIGH_FREQ, CFLIST_DEFAULT_BW_MODE_NAME,
	  CFLIST_DEFAULT_BW_MODE_FUNC)) == NULL) {
		NotifyError("%s: could not set default CFList.", funcName);
		return(FALSE);
	}
	bM0DRNLPtr->nonLinearGT1 = NULL;
	bM0DRNLPtr->nonLinearGT2 = NULL;
	bM0DRNLPtr->nonLinearLP.set = FALSE;
	bM0DRNLPtr->linearLP.set = FALSE;
	bM0DRNLPtr->linearF = NULL;

	InitCompressionModeList_BasilarM_DRNL_Test();
	InitLPFilterModeList_BasilarM_DRNL_Test();
	if (!SetUniParList_BasilarM_DRNL_Test()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_BasilarM_DRNL_Test();
		return(FALSE);
	}
	SetDefaultCompressionMode_BasilarM_DRNL_Test();
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
Free_BasilarM_DRNL_Test(void)
{
	if (bM0DRNLPtr == NULL)
		return(TRUE);
	FreeProcessVariables_BasilarM_DRNL_Test();
	Free_CFList(&bM0DRNLPtr->theCFs);
	if (bM0DRNLPtr->compressionPars) {
		free(bM0DRNLPtr->compressionPars);
		free(bM0DRNLPtr->compressionParsFlag);
		bM0DRNLPtr->compressionPars = NULL;
		bM0DRNLPtr->compressionParsFlag = NULL;
	}
	if (bM0DRNLPtr->parList)
		FreeList_UniParMgr(&bM0DRNLPtr->parList);
	if (bM0DRNLPtr->parSpec == GLOBAL) {
		free(bM0DRNLPtr);
		bM0DRNLPtr = NULL;
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
SetUniParList_BasilarM_DRNL_Test(void)
{
	static const char *funcName = "SetUniParList_BasilarM_DRNL_Test";
	UniParPtr	pars;

	if ((bM0DRNLPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  BASILARM_DRNL_TEST_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = bM0DRNLPtr->parList->pars;
	SetPar_UniParMgr(&pars[BASILARM_DRNL_TEST_NONLINGTCASCADE], "NL_GT_CASCADE",
	  "Nonlinear gammatone filter cascade ('order').",
	  UNIPAR_INT,
	  &bM0DRNLPtr->nonLinGTCascade, NULL,
	  (void * (*)) SetNonLinGTCascade_BasilarM_DRNL_Test);
	SetPar_UniParMgr(&pars[BASILARM_DRNL_TEST_NONLINLPCASCADE], "NL_LP_CASCADE",
	  "Nonlinear low-pass filter cascade ('order').",
	  UNIPAR_INT,
	  &bM0DRNLPtr->nonLinLPCascade, NULL,
	  (void * (*)) SetNonLinLPCascade_BasilarM_DRNL_Test);
	SetPar_UniParMgr(&pars[BASILARM_DRNL_TEST_LPFILTERMODE], "LP_FILTER",
	  "Low-pass filter mode ('butterworth' or 'Beauchamp').",
	  UNIPAR_NAME_SPEC,
	  &bM0DRNLPtr->lPFilterMode, bM0DRNLPtr->lPFilterModeList,
	  (void * (*)) SetLPFilterMode_BasilarM_DRNL_Test);
	SetPar_UniParMgr(&pars[BASILARM_DRNL_TEST_COMPRESSIONMODE], "COMP_MODE",
	  "Compression mode ('original', 'inv_power', 'broken_stick1' or "
	    "'upton_bstick').",
	  UNIPAR_NAME_SPEC,
	  &bM0DRNLPtr->compressionMode, bM0DRNLPtr->compressionModeList,
	  (void * (*)) SetCompressionMode_BasilarM_DRNL_Test);
	SetPar_UniParMgr(&pars[BASILARM_DRNL_TEST_COMPRESSIONPARS], "COMP_PAR",
	  "Compression parameters.",
	  UNIPAR_REAL_ARRAY,
	  &bM0DRNLPtr->compressionPars, &bM0DRNLPtr->numCompressionPars,
	  (void * (*)) SetCompressionPar_BasilarM_DRNL_Test);
	SetPar_UniParMgr(&pars[BASILARM_DRNL_TEST_LINGTCASCADE], "L_GT_CASCADE",
	  "Linear gammatone filter cascade ('order').",
	  UNIPAR_INT,
	  &bM0DRNLPtr->linGTCascade, NULL,
	  (void * (*)) SetLinGTCascade_BasilarM_DRNL_Test);
	SetPar_UniParMgr(&pars[BASILARM_DRNL_TEST_LINLPCASCADE], "L_LP_CASCADE",
	  "Linear low-pass filter cascade ('order').",
	  UNIPAR_INT,
	  &bM0DRNLPtr->linLPCascade, NULL,
	  (void * (*)) SetLinLPCascade_BasilarM_DRNL_Test);
	SetPar_UniParMgr(&pars[BASILARM_DRNL_TEST_LINCF], "L_CF",
	  "Linear filter Centre Frequency (Hz).",
	  UNIPAR_REAL,
	  &bM0DRNLPtr->linCF, NULL,
	  (void * (*)) SetLinCF_BasilarM_DRNL_Test);
	SetPar_UniParMgr(&pars[BASILARM_DRNL_TEST_LINBWIDTH], "L_BWIDTH",
	  "Linear filter band width (Hz).",
	  UNIPAR_REAL,
	  &bM0DRNLPtr->linBwidth, NULL,
	  (void * (*)) SetLinBwidth_BasilarM_DRNL_Test);
	SetPar_UniParMgr(&pars[BASILARM_DRNL_TEST_LINSCALER], "L_SCALER",
	  "Linear filter scale (units).",
	  UNIPAR_REAL,
	  &bM0DRNLPtr->linScaler, NULL,
	  (void * (*)) SetLinScaler_BasilarM_DRNL_Test);
	SetPar_UniParMgr(&pars[BASILARM_DRNL_TEST_THECFS], "CFLIST",
	  "",
	  UNIPAR_CFLIST,
	  &bM0DRNLPtr->theCFs, NULL,
	  (void * (*)) SetCFList_BasilarM_DRNL_Test);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_BasilarM_DRNL_Test(void)
{
	static const char	*funcName = "GetUniParListPtr_BasilarM_DRNL_Test";

	if (bM0DRNLPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (bM0DRNLPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(bM0DRNLPtr->parList);

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
CheckPars_BasilarM_DRNL_Test(void)
{
	static const char	*funcName = "CheckPars_BasilarM_DRNL_Test";
	BOOLN	ok;
	int		i;

	ok = TRUE;
	if (bM0DRNLPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!bM0DRNLPtr->nonLinGTCascadeFlag) {
		NotifyError("%s: nonLinGTCascade variable not set.", funcName);
		ok = FALSE;
	}
	if (!bM0DRNLPtr->nonLinLPCascadeFlag) {
		NotifyError("%s: nonLinLPCascade variable not set.", funcName);
		ok = FALSE;
	}
	if (!bM0DRNLPtr->lPFilterModeFlag) {
		NotifyError("%s: lPFilterMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!bM0DRNLPtr->compressionModeFlag) {
		NotifyError("%s: compressionMode variable not set.", funcName);
		ok = FALSE;
	} else {
		for (i = 0; i < bM0DRNLPtr->numCompressionPars; i++)
			if (!bM0DRNLPtr->compressionParsFlag[i]) {
				NotifyError("%s: Compression parameter [%d] not set.", funcName,
				  i);
				ok = FALSE;
			}
	}
	if (!bM0DRNLPtr->linGTCascadeFlag) {
		NotifyError("%s: linGTCascade variable not set.", funcName);
		ok = FALSE;
	}
	if (!bM0DRNLPtr->linLPCascadeFlag) {
		NotifyError("%s: linLPCascade variable not set.", funcName);
		ok = FALSE;
	}
	if (!bM0DRNLPtr->linCFFlag) {
		NotifyError("%s: linCF variable not set.", funcName);
		ok = FALSE;
	}
	if (!bM0DRNLPtr->linBwidthFlag) {
		NotifyError("%s: linBwidth variable not set.", funcName);
		ok = FALSE;
	}
	if (!bM0DRNLPtr->linScalerFlag) {
		NotifyError("%s: linScaler variable not set.", funcName);
		ok = FALSE;
	}
	if (!CheckPars_CFList(bM0DRNLPtr->theCFs)) {
		NotifyError("%s: Centre frequency list parameters not correctly set.",
		  funcName);
		ok = FALSE;
	}
	return(ok);

}

/****************************** SetNonLinGTCascade ****************************/

/*
 * This function sets the module's nonLinGTCascade parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNonLinGTCascade_BasilarM_DRNL_Test(int theNonLinGTCascade)
{
	static const char	*funcName = "SetNonLinGTCascade_BasilarM_DRNL_Test";

	if (bM0DRNLPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (theNonLinGTCascade < 0) {
		NotifyError("%s: Illegal value (%d).  Use zero to disable.", funcName,
		  theNonLinGTCascade);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bM0DRNLPtr->nonLinGTCascadeFlag = TRUE;
	bM0DRNLPtr->nonLinGTCascade = theNonLinGTCascade;
	bM0DRNLPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** SetNonLinLPCascade ****************************/

/*
 * This function sets the module's nonLinLPCascade parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNonLinLPCascade_BasilarM_DRNL_Test(int theNonLinLPCascade)
{
	static const char	*funcName = "SetNonLinLPCascade_BasilarM_DRNL_Test";

	if (bM0DRNLPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (theNonLinLPCascade < 0) {
		NotifyError("%s: Illegal value (%d).  Use zero to disable.", funcName,
		  theNonLinLPCascade);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bM0DRNLPtr->nonLinLPCascadeFlag = TRUE;
	bM0DRNLPtr->nonLinLPCascade = theNonLinLPCascade;
	bM0DRNLPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** SetCompressionParsArray ***********************/

/*
 * This sets up the array of parameters for a particular compression mode.
 * It assumes that an illegal mode specifier will not be used.
 * This mode of operation was required to reconcile with the universal
 * parameters list implementation.
 */

BOOLN
SetCompressionParsArray_BasilarM_DRNL_Test(int mode)
{
	static const char *funcName = "SetCompressionParsArray_BasilarM_DRNL_Test";
	int		numPars;

	if (bM0DRNLPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	switch (mode) {
	case DRNLT_COMPRESSION_MODE_DISABLED:
		numPars = 0;
		break;
	case DRNLT_COMPRESSION_MODE_ORIGINAL:
	case DRNLT_COMPRESSION_MODE_INVPOWER:
		numPars = 2;
		break;
	case DRNLT_COMPRESSION_MODE_BROKENSTICK1:
		numPars = 3;
		break;
	case DRNLT_COMPRESSION_MODE_UPTON_BSTICK:
		numPars = 4;
		break;
	default:
		return(FALSE);
	}
	if (numPars != bM0DRNLPtr->numCompressionPars) {
		if (bM0DRNLPtr->compressionPars)
			free(bM0DRNLPtr->compressionPars);
		if (bM0DRNLPtr->compressionParsFlag)
			free(bM0DRNLPtr->compressionParsFlag);
		if ((bM0DRNLPtr->compressionPars = (double *) calloc(numPars,
		  sizeof(double))) == NULL) {
			NotifyError("%s: Output of memory for compression parameter "
			  "array([%d]).",
			  funcName, numPars);
			return(FALSE);
		}
		if ((bM0DRNLPtr->compressionParsFlag = (BOOLN *) calloc(numPars,
		  sizeof(BOOLN))) == NULL) {
			NotifyError("%s: Output of memory for compression parameter "
			  "flags array([%d]).",
			  funcName, numPars);
			return(FALSE);
		}
		bM0DRNLPtr->numCompressionPars = numPars;
	}
	return(TRUE);

}

/****************************** SetLPFilterMode *******************************/

/*
 * This function sets the module's lPFilterMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLPFilterMode_BasilarM_DRNL_Test(char * theLPFilterMode)
{
	static const char	*funcName = "SetLPFilterMode_BasilarM_DRNL_Test";
	int		specifier;

	if (bM0DRNLPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theLPFilterMode,
		bM0DRNLPtr->lPFilterModeList)) ==
		BASILARM_DRNL_TEST_LPFILTERMODE_NULL) {
		NotifyError("%s: Illegal name (%s).", funcName, theLPFilterMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bM0DRNLPtr->lPFilterModeFlag = TRUE;
	bM0DRNLPtr->lPFilterMode = specifier;
	return(TRUE);

}

/****************************** SetCompressionMode ****************************/

/*
 * This function sets the module's compressionMode parameter.
 * This routine also sets the 'numCompressionPars' module parameter, as it is
 * used by the universal parameters list.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCompressionMode_BasilarM_DRNL_Test(char *theCompressionMode)
{
	static const char	*funcName = "SetCompressionMode_BasilarM_DRNL_Test";
	int specifier;

	if (bM0DRNLPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theCompressionMode,
	  bM0DRNLPtr->compressionModeList)) == DRNLT_COMPRESSION_MODE_NULL) {
		NotifyError("%s: Illegal compression mode  (%s).", funcName,
		  theCompressionMode);
		return(FALSE);
	}
	bM0DRNLPtr->compressionModeFlag = TRUE;
	bM0DRNLPtr->compressionMode = specifier;
	SetCompressionParsArray_BasilarM_DRNL_Test(specifier);
	bM0DRNLPtr->parList->pars[BASILARM_DRNL_TEST_COMPRESSIONPARS].enabled =
	  (specifier != DRNLT_COMPRESSION_MODE_DISABLED);
	bM0DRNLPtr->parList->updateFlag = TRUE;
	return(TRUE);

}

/********************** SetCompressionPar *************************************/

/*
 * This function sets values in the module's compression parameter array.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetCompressionPar_BasilarM_DRNL_Test(int index, double parameterValue)
{
	static const char *funcName = "SetCompressionPar_BasilarM_DRNL_Test";

	if (bM0DRNLPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (index > bM0DRNLPtr->numCompressionPars) {
		NotifyError("%s: Illegal index value (max: %d).", funcName,
		  bM0DRNLPtr->numCompressionPars);
		return(FALSE);
	}
	bM0DRNLPtr->compressionParsFlag[index] = TRUE;
	bM0DRNLPtr->compressionPars[index] = parameterValue;
	return(TRUE);
	
}

/****************************** SetLinGTCascade *******************************/

/*
 * This function sets the module's linGTCascade parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLinGTCascade_BasilarM_DRNL_Test(int theLinGTCascade)
{
	static const char	*funcName = "SetLinGTCascade_BasilarM_DRNL_Test";

	if (bM0DRNLPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (theLinGTCascade < 0) {
		NotifyError("%s: Illegal value (%d).  Use zero to disable.", funcName,
		  theLinGTCascade);
		return(FALSE);
	}
	bM0DRNLPtr->linGTCascadeFlag = TRUE;
	bM0DRNLPtr->linGTCascade = theLinGTCascade;
	bM0DRNLPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** SetLinLPCascade *******************************/

/*
 * This function sets the module's linLPCascade parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLinLPCascade_BasilarM_DRNL_Test(int theLinLPCascade)
{
	static const char	*funcName = "SetLinLPCascade_BasilarM_DRNL_Test";

	if (bM0DRNLPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (theLinLPCascade < 0) {
		NotifyError("%s: Illegal value (%d).  Use zero to disable.", funcName,
		  theLinLPCascade);
		return(FALSE);
	}
	bM0DRNLPtr->linLPCascadeFlag = TRUE;
	bM0DRNLPtr->linLPCascade = theLinLPCascade;
	bM0DRNLPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** SetLinCF **************************************/

/*
 * This function sets the module's linCF parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLinCF_BasilarM_DRNL_Test(double theLinCF)
{
	static const char	*funcName = "SetLinCF_BasilarM_DRNL_Test";

	if (bM0DRNLPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (theLinCF <= 0.0) {
		NotifyError("%s: Illegal value (%g)!", funcName, theLinCF);
		return(FALSE);
	}
	bM0DRNLPtr->linCFFlag = TRUE;
	bM0DRNLPtr->linCF = theLinCF;
	bM0DRNLPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** SetLinBwidth **********************************/

/*
 * This function sets the module's linBwidth parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLinBwidth_BasilarM_DRNL_Test(double theLinBwidth)
{
	static const char	*funcName = "SetLinBwidth_BasilarM_DRNL_Test";

	if (bM0DRNLPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (theLinBwidth <= 0.0) {
		NotifyError("%s: Illegal value (%g)!", funcName, theLinBwidth);
		return(FALSE);
	}
	bM0DRNLPtr->linBwidthFlag = TRUE;
	bM0DRNLPtr->linBwidth = theLinBwidth;
	bM0DRNLPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** SetLinScaler **********************************/

/*
 * This function sets the module's linScaler parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLinScaler_BasilarM_DRNL_Test(double theLinScaler)
{
	static const char	*funcName = "SetLinScaler_BasilarM_DRNL_Test";

	if (bM0DRNLPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bM0DRNLPtr->linScalerFlag = TRUE;
	bM0DRNLPtr->linScaler = theLinScaler;
	bM0DRNLPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/*********************** SetCFList ********************************************/

/*
 * This function sets the CFList data structure for the filter bank.
 * It returns TRUE if the operation is successful.
 */
 
BOOLN
SetCFList_BasilarM_DRNL_Test(CFListPtr theCFList)
{
	static const char *funcName = "SetCFList_BasilarM_DRNL_Test";

	if (bM0DRNLPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckPars_CFList(theCFList)) {
		NotifyError("%s: Centre frequency structure not initialised "\
		  "correctly set.", funcName);
		return(FALSE);
	}
	if (bM0DRNLPtr->theCFs != NULL)
		Free_CFList(&bM0DRNLPtr->theCFs);
	bM0DRNLPtr->theCFs = theCFList;
	bM0DRNLPtr->updateProcessVariablesFlag = TRUE;
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
SetBandwidths_BasilarM_DRNL_Test(char *theBandwidthMode, double *theBandwidths)
{
	static const char *funcName = "SetBandwidths_BasilarM_DRNL_Test";

	if (bM0DRNLPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!SetBandwidths_CFList(bM0DRNLPtr->theCFs, theBandwidthMode,
	  theBandwidths)) {
		NotifyError("%s: Failed to set bandwidth mode.", funcName);
		return(FALSE);
	}
	bM0DRNLPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);
		
}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_BasilarM_DRNL_Test(int nonLinGTCascade, int nonLinLPCascade,
  char * lPFilterMode, char * compressionMode, double *compressionParsArray,
  int linGTCascade, int linLPCascade, double linCF, double linBwidth,
  double linScaler, CFListPtr theCFs)
{
	static const char	*funcName = "SetPars_BasilarM_DRNL_Test";
	BOOLN	ok;
	int		i;

	ok = TRUE;
	if (!SetNonLinGTCascade_BasilarM_DRNL_Test(nonLinGTCascade))
		ok = FALSE;
	if (!SetNonLinLPCascade_BasilarM_DRNL_Test(nonLinLPCascade))
		ok = FALSE;
	if (!SetLPFilterMode_BasilarM_DRNL_Test(lPFilterMode))
		ok = FALSE;
	if (!SetCompressionMode_BasilarM_DRNL_Test(compressionMode))
		ok = FALSE;
	else
		for (i = 0; i < bM0DRNLPtr->numCompressionPars; i++)
			if (!SetCompressionPar_BasilarM_DRNL_Test(i, compressionParsArray[
			  i]))
				ok = FALSE;
	if (!SetLinGTCascade_BasilarM_DRNL_Test(linGTCascade))
		ok = FALSE;
	if (!SetLinLPCascade_BasilarM_DRNL_Test(linLPCascade))
		ok = FALSE;
	if (!SetLinCF_BasilarM_DRNL_Test(linCF))
		ok = FALSE;
	if (!SetLinBwidth_BasilarM_DRNL_Test(linBwidth))
		ok = FALSE;
	if (!SetLinScaler_BasilarM_DRNL_Test(linScaler))
		ok = FALSE;
	if (!SetCFList_BasilarM_DRNL_Test(theCFs))
		ok = FALSE;
	if (!ok)
		NotifyError("%s: Failed to set all module parameters." ,funcName);
	return(ok);

}

/****************************** GetCFListPtr **********************************/

/*
 * This routine returns a pointer to the module's CFList data pointer.
 *
 */

CFListPtr
GetCFListPtr_BasilarM_DRNL_Test(void)
{
	static const char *funcName = "GetCFListPtr_BasilarM_DRNL_Test";
	
	if (bM0DRNLPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (bM0DRNLPtr->theCFs == NULL) {
		NotifyError("%s: CFList data structure has not been correctly set.  "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(bM0DRNLPtr->theCFs);

}

/****************************** PrintPars *************************************/

/*
 * This program prints the parameters of the module to the standard output.
 */
 
BOOLN
PrintPars_BasilarM_DRNL_Test(void)
{
	static const char *funcName = "PrintPars_BasilarM_DRNL_Test";
	
	if (!CheckPars_BasilarM_DRNL_Test()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("DRNL Test Basilar Membrane Filter Module Parameters:-\n");
	PrintPars_CFList(bM0DRNLPtr->theCFs);
	DPrint("\tNonlinear gammatone filter cascade ('order') = %d\n",
	  bM0DRNLPtr->nonLinGTCascade);
	DPrint("\tNonlinear low-pass filter cascade ('order')  = %d\n",\
	  bM0DRNLPtr->nonLinLPCascade);
	DPrint("\tLow-pass filter mode = %s ,\n", bM0DRNLPtr->lPFilterModeList[
	  bM0DRNLPtr->lPFilterMode].name);
	DPrint("\tCompression mode: %s,\n",
	  bM0DRNLPtr->compressionModeList[bM0DRNLPtr->compressionMode].name);
	switch (bM0DRNLPtr->compressionMode) {
	case DRNLT_COMPRESSION_MODE_DISABLED:
		DPrint("\tCompression disabled.\n");
		break;
	case DRNLT_COMPRESSION_MODE_ORIGINAL:
		DPrint("\tCompression threshold = %g,", bM0DRNLPtr->compressionPars[0]);
		DPrint("\tCompression exponent = %g,\n",
		  bM0DRNLPtr->compressionPars[1]);
		break;
	case DRNLT_COMPRESSION_MODE_INVPOWER:
		DPrint("\tCompression shift = %g,", bM0DRNLPtr->compressionPars[0]);
		DPrint("\tCompression slope = %g,\n", bM0DRNLPtr->compressionPars[1]);
		break;
	case DRNLT_COMPRESSION_MODE_BROKENSTICK1:
		DPrint("\tCompression: a = %g,", bM0DRNLPtr->compressionPars[0]);
		DPrint("\tb = %g,", bM0DRNLPtr->compressionPars[1]);
		DPrint("\tc = %g,\n", bM0DRNLPtr->compressionPars[2]);
		break;
	case DRNLT_COMPRESSION_MODE_UPTON_BSTICK:
		DPrint("\tCompression: a = %g,", bM0DRNLPtr->compressionPars[0]);
		DPrint("\tb = %g,", bM0DRNLPtr->compressionPars[1]);
		DPrint("\tc = %g,", bM0DRNLPtr->compressionPars[2]);
		DPrint("\td = %g,\n", bM0DRNLPtr->compressionPars[3]);
		break;
	}
	DPrint("\tLinear gammatone filter cascade ('order') = %d\n",
	  bM0DRNLPtr->linGTCascade);
	DPrint("\tLinear low-pass filter cascade ('order')  = %d\n",\
	  bM0DRNLPtr->linLPCascade);
	DPrint("\tLinear CF = %g (Hz),", bM0DRNLPtr->linCF);
	DPrint("\tBandwidth = %g (Hz),", bM0DRNLPtr->linBwidth);
	DPrint("\tScale = %g (units.)\n", bM0DRNLPtr->linScaler);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_BasilarM_DRNL_Test(char *fileName)
{
	static const char	*funcName = "ReadPars_BasilarM_DRNL_Test";
	BOOLN	ok;
	char	*filePath, compressionMode[MAXLINE], lPFilterMode[MAXLINE];
	int		i, nonLinGTCascade, nonLinLPCascade, linGTCascade, linLPCascade;
	double	linCF, linBwidth, linScaler;
	CFListPtr	theCFs;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(filePath, "r")) == NULL) {
		NotifyError("%s: Cannot open data file '%s'.\n", funcName, fileName);
		return(FALSE);
	}
	DPrint("%s: Reading from '%s':\n", funcName, fileName);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, "%d", &nonLinGTCascade))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%d", &nonLinLPCascade))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%s", lPFilterMode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%s", compressionMode))
		ok = FALSE;
	if (!SetCompressionParsArray_BasilarM_DRNL_Test(Identify_NameSpecifier(
	  compressionMode, bM0DRNLPtr->compressionModeList))) {
		NotifyError("%s: Could not set compression parameters arrays.",
		  funcName);
		return(FALSE);
	}
	for (i = 0; i < bM0DRNLPtr->numCompressionPars; i++)
		if (!GetPars_ParFile(fp, "%lf", &bM0DRNLPtr->compressionPars[i]))
			ok = FALSE;
	if (!GetPars_ParFile(fp, "%d", &linGTCascade))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%d", &linLPCascade))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &linCF))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &linBwidth))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &linScaler))
		ok = FALSE;
	if ((theCFs = ReadPars_CFList(fp)) == NULL)
		 ok = FALSE;
	if (!ReadBandwidths_CFList(fp, theCFs))
		 ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "
		  "parameter file '%s'.", funcName, fileName);
		return(FALSE);
	}
	if (!SetPars_BasilarM_DRNL_Test(nonLinGTCascade, nonLinLPCascade,
	  lPFilterMode, compressionMode, bM0DRNLPtr->compressionPars, linGTCascade,
	  linLPCascade, linCF, linBwidth, linScaler, theCFs)) {
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
SetParsPointer_BasilarM_DRNL_Test(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_BasilarM_DRNL_Test";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	bM0DRNLPtr = (BM0DRNLPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_BasilarM_DRNL_Test(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_BasilarM_DRNL_Test";

	if (!SetParsPointer_BasilarM_DRNL_Test(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_BasilarM_DRNL_Test(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = bM0DRNLPtr;
	theModule->CheckPars = CheckPars_BasilarM_DRNL_Test;
	theModule->Free = Free_BasilarM_DRNL_Test;
	theModule->GetUniParListPtr = GetUniParListPtr_BasilarM_DRNL_Test;
	theModule->PrintPars = PrintPars_BasilarM_DRNL_Test;
	theModule->ReadPars = ReadPars_BasilarM_DRNL_Test;
	theModule->RunProcess = RunModel_BasilarM_DRNL_Test;
	theModule->SetParsPointer = SetParsPointer_BasilarM_DRNL_Test;
	return(TRUE);

}

/**************************** InitLPFilterArray *******************************/

/*
 * This function initialises the low-pass filter array.
 * It returns FALSE if it fails in any way.
 */

BOOLN
InitLPFilterArray_BasilarM_DRNL_Test(LowPassFilterPtr p, int cascade)
{
	static const char *funcName = "InitLPFilterArray_BasilarM_DRNL_Test";

	if (!cascade)
		return(TRUE);

	switch (bM0DRNLPtr->lPFilterMode) {
	case BASILARM_DRNL_TEST_BUTTERWORTH_LPFILTERMODE:
		if ((p->ptr.butt = (TwoPoleCoeffsPtr *) calloc(bM0DRNLPtr->numChannels,
		  sizeof(TwoPoleCoeffsPtr))) == NULL) {
		 	NotifyError("%s: Out of memory.", funcName);
		 	return(FALSE);
		}
		p->stateVectorLen = FILTERS_NUM_IIR2_STATE_VARS;
		break;
	case BASILARM_DRNL_TEST_BEAUCHAMP_LPFILTERMODE:
			if ((p->ptr.beau = (ContButtCoeffsPtr *) calloc(
			  bM0DRNLPtr->numChannels, sizeof(ContButtCoeffsPtr))) == NULL) {
		 		NotifyError("%s: Out of memory (linearLP).", funcName);
		 		return(FALSE);
			}
		p->stateVectorLen = FILTERS_NUM_CONTBUTT2_STATE_VARS;
		break;
	default:
		;
	}
	p->set = TRUE;
	p->cascade = cascade;
	return(TRUE);

}

/**************************** InitLPFilter ************************************/

/*
 * This function initialises a low-pass filter.
 * It returns FALSE if it fails in any way.
 */

BOOLN
InitLPFilter_BasilarM_DRNL_Test(LowPassFilterPtr p, int index,
  double cutOffFrequency, double dt)
{
	static const char *funcName = "InitLPFilter_BasilarM_DRNL_Test";

	if (!p->set)
		return(TRUE);

	switch (bM0DRNLPtr->lPFilterMode) {
	case BASILARM_DRNL_TEST_BUTTERWORTH_LPFILTERMODE:
		if ((p->ptr.butt[index] = InitIIR2Coeffs_Filters(Filters_butt2Poly,
		  p->cascade, cutOffFrequency, 1.0 / dt, LOWPASS)) == NULL) {
			NotifyError("%s: Could not initialise butterworth filter (%d).",
			  funcName, index);
			return(FALSE);
		}
		break;
	case BASILARM_DRNL_TEST_BEAUCHAMP_LPFILTERMODE:
		if ((p->ptr.beau[index] = InitIIR2ContCoeffs_Filters(p->cascade,
		  cutOffFrequency, dt, LOWPASS)) == NULL) {
			NotifyError("%s: Could not initialise Beauchamp filter (%d).",
			  funcName, index);
			return(FALSE);
		}
		break;
	default:
		;
	}
	return(TRUE);

}

/**************************** ResetLPFilter ***********************************/

/*
 * This function resets the stat of a low-pass filter.
 */

void
ResetLPFilter_BasilarM_DRNL_Test(LowPassFilterPtr p)
{
	int		i, j;

	if (!p->set)
		return;

	switch (bM0DRNLPtr->lPFilterMode) {
	case BASILARM_DRNL_TEST_BUTTERWORTH_LPFILTERMODE:
		for (i = 0; i < bM0DRNLPtr->numChannels; i++)
			for (j = 0; j < p->cascade * p->stateVectorLen; j++)
				p->ptr.butt[i]->state[j] = 0.0;
		break;
	case BASILARM_DRNL_TEST_BEAUCHAMP_LPFILTERMODE:
		for (i = 0; i < bM0DRNLPtr->numChannels; i++)
			for (j = 0; j < p->cascade * p->stateVectorLen; j++)
				p->ptr.beau[i]->state[j] = 0.0;
		break;
	default:
		;
	}

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 * It initialises the coefficients for all the filters involved in
 * the DRNL composite filter.
 * It assumes that the centre frequency list is set up correctly.
 * It also assumes that the output signal has already been initialised.
*/

BOOLN
InitProcessVariables_BasilarM_DRNL_Test(EarObjectPtr data)
{
	static const char *funcName = "InitProcessVariables_BasilarM_DRNL_Test";
	int		i, j, cFIndex;
	double	sampleRate, centreFreq;
	
	if (bM0DRNLPtr->updateProcessVariablesFlag || data->updateProcessFlag ||
	  bM0DRNLPtr->theCFs->updateFlag) {
		FreeProcessVariables_BasilarM_DRNL_Test();
		bM0DRNLPtr->linearF = Init_EarObject("NULL");
		bM0DRNLPtr->numChannels = data->outSignal->numChannels;
		if (bM0DRNLPtr->nonLinGTCascade) {
			if ((bM0DRNLPtr->nonLinearGT1 = (GammaToneCoeffsPtr *) calloc(
			  bM0DRNLPtr->numChannels, sizeof(GammaToneCoeffsPtr))) == NULL) {
		 		NotifyError("%s: Out of memory (nonLinearGT1).", funcName);
		 		return(FALSE);
			}
			if ((bM0DRNLPtr->nonLinearGT2 = (GammaToneCoeffsPtr *) calloc(
			  bM0DRNLPtr->numChannels, sizeof(GammaToneCoeffsPtr))) == NULL) {
		 		NotifyError("%s: Out of memory (nonLinearGT2).", funcName);
		 		return(FALSE);
			}
		}
		if (bM0DRNLPtr->linGTCascade) {
			if ((bM0DRNLPtr->linearGT = (GammaToneCoeffsPtr *) calloc(
			  bM0DRNLPtr->numChannels, sizeof(GammaToneCoeffsPtr))) == NULL) {
		 		NotifyError("%s: Out of memory (linearGT).", funcName);
		 		return(FALSE);
			}
		}

		if (!InitLPFilterArray_BasilarM_DRNL_Test(&bM0DRNLPtr->nonLinearLP,
		  bM0DRNLPtr->nonLinLPCascade)) {
			NotifyError("%s: Could not initialise nonlinear low-pass filter.",
			  funcName);
			return(FALSE);
		}
		if (!InitLPFilterArray_BasilarM_DRNL_Test(&bM0DRNLPtr->linearLP,
		  bM0DRNLPtr->linLPCascade)) {
			NotifyError("%s: Could not initialise linear low-pass filter.",
			  funcName);
			return(FALSE);
		}

		sampleRate = 1.0 / data->inSignal[0]->dt;
		for (i = 0; i < data->outSignal->numChannels; i++) {
			cFIndex = i / data->inSignal[0]->interleaveLevel;
			centreFreq = bM0DRNLPtr->theCFs->frequency[cFIndex];
			if (bM0DRNLPtr->nonLinearGT1) {
				if ((bM0DRNLPtr->nonLinearGT1[i] = InitGammaToneCoeffs_Filters(
				  centreFreq, bM0DRNLPtr->theCFs->bandwidth[cFIndex],
				  bM0DRNLPtr->nonLinGTCascade, sampleRate)) == NULL) {
					NotifyError("%s: Could not initialise nonlinear GT 1 "
					  "coefficients for channel %d.", funcName, i);
					return(FALSE);
				}
				if ((bM0DRNLPtr->nonLinearGT2[i] = InitGammaToneCoeffs_Filters(
				  centreFreq, bM0DRNLPtr->theCFs->bandwidth[cFIndex],
				  bM0DRNLPtr->nonLinGTCascade, sampleRate)) == NULL) {
					NotifyError("%s: Could not initialise nonlinear GT 2 "
					  "coefficients for channel %d.", funcName, i);
					return(FALSE);
				}
			}
			if (bM0DRNLPtr->linearGT) {
				if ((bM0DRNLPtr->linearGT[i] = InitGammaToneCoeffs_Filters(
				  bM0DRNLPtr->linCF, bM0DRNLPtr->linBwidth, bM0DRNLPtr->
				  linGTCascade, sampleRate)) == NULL) {
					NotifyError("%s: Could not initialise nonlinear GT 2 "
					  "coefficients for channel %d.", funcName, i);
					return(FALSE);
				}
			}

			if (!InitLPFilter_BasilarM_DRNL_Test(&bM0DRNLPtr->nonLinearLP, i,
			  centreFreq, data->inSignal[0]->dt)) {
				NotifyError("%s: Could not initialise low-pass filter.",
				  funcName);
				return(FALSE);
			}
			if (!InitLPFilter_BasilarM_DRNL_Test(&bM0DRNLPtr->linearLP, i,
			  bM0DRNLPtr->linCF, data->inSignal[0]->dt)) {
				NotifyError("%s: Could not initialise low-pass filter.",
				  funcName);
				return(FALSE);
			}
		}
		SetLocalInfoFlag_SignalData(data->outSignal, TRUE);
		SetInfoChannelTitle_SignalData(data->outSignal, "Frequency (Hz)");
		SetInfoChannelLabels_SignalData(data->outSignal,
		  bM0DRNLPtr->theCFs->frequency);
		SetInfoCFArray_SignalData(data->outSignal,
		  bM0DRNLPtr->theCFs->frequency);
		bM0DRNLPtr->updateProcessVariablesFlag = FALSE;
		bM0DRNLPtr->theCFs->updateFlag = FALSE;
	} else if (data->timeIndex == PROCESS_START_TIME) {
		for (i = 0; i < data->outSignal->numChannels; i++) {
			for (j = 0; j < bM0DRNLPtr->nonLinGTCascade * 
			  FILTERS_NUM_GAMMAT_STATE_VARS_PER_FILTER; j++) {
				bM0DRNLPtr->nonLinearGT1[i]->stateVector[j] = 0.0;
				bM0DRNLPtr->nonLinearGT2[i]->stateVector[j] = 0.0;
			}
			for (j = 0; j < bM0DRNLPtr->linGTCascade *
			  FILTERS_NUM_GAMMAT_STATE_VARS_PER_FILTER; j++)
				bM0DRNLPtr->linearGT[i]->stateVector[j] = 0.0;
		}
		ResetLPFilter_BasilarM_DRNL_Test(&bM0DRNLPtr->nonLinearLP);
		ResetLPFilter_BasilarM_DRNL_Test(&bM0DRNLPtr->linearLP);
	}
	return(TRUE);

}

/**************************** FreeLPFilters ***********************************/

/*
 * This routine releases the memory allocated for the low-pass filters
 * if they have been initialised.
 */

void
FreeLPFilters_BasilarM_DRNL_Test(LowPassFilterPtr p)
{
	int		i;

	if (!p->set)
		return;

	switch (bM0DRNLPtr->lPFilterMode) {
	case BASILARM_DRNL_TEST_BUTTERWORTH_LPFILTERMODE:
		for (i = 0; i < bM0DRNLPtr->numChannels; i++)
			FreeTwoPoleCoeffs_Filters(&p->ptr.butt[i]);
		free(p->ptr.butt);
		p->ptr.butt = NULL;
		break;
	case BASILARM_DRNL_TEST_BEAUCHAMP_LPFILTERMODE:
		for (i = 0; i < bM0DRNLPtr->numChannels; i++)
			FreeIIR2ContCoeffs_Filters(&p->ptr.beau[i]);
		free(p->ptr.beau);
		p->ptr.beau = NULL;
		break;
	default:
		;
	}
	p->set = FALSE;

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

void
FreeProcessVariables_BasilarM_DRNL_Test(void)
{
	int		i;

	if (bM0DRNLPtr->linearF == NULL)
		return;
	Free_EarObject(&bM0DRNLPtr->linearF);
	for (i = 0; bM0DRNLPtr->nonLinearGT1 && (i < bM0DRNLPtr->numChannels);
	  i++) {
		FreeGammaToneCoeffs_Filters(&bM0DRNLPtr->nonLinearGT1[i]);
		FreeGammaToneCoeffs_Filters(&bM0DRNLPtr->nonLinearGT2[i]);
	}
	for (i = 0; bM0DRNLPtr->linearGT && (i < bM0DRNLPtr->numChannels); i++)
		FreeGammaToneCoeffs_Filters(&bM0DRNLPtr->linearGT[i]);
	free(bM0DRNLPtr->nonLinearGT1);
	free(bM0DRNLPtr->nonLinearGT2);
	free(bM0DRNLPtr->linearGT);
	bM0DRNLPtr->linearGT = NULL;
	bM0DRNLPtr->nonLinearGT1 = NULL;
	bM0DRNLPtr->nonLinearGT2 = NULL;
	FreeLPFilters_BasilarM_DRNL_Test(&bM0DRNLPtr->nonLinearLP);
	FreeLPFilters_BasilarM_DRNL_Test(&bM0DRNLPtr->linearLP);
	bM0DRNLPtr->updateProcessVariablesFlag = TRUE;

}

/**************************** RunLPFilter *************************************/

/*
 * This function runs a low-pass filter on the signal.
 */

void
RunLPFilter_BasilarM_DRNL_Test(SignalDataPtr signal, LowPassFilterPtr p)
{
	if (!p->set)
		return;

	switch (bM0DRNLPtr->lPFilterMode) {
	case BASILARM_DRNL_TEST_BUTTERWORTH_LPFILTERMODE:
		IIR2_Filters(signal, p->ptr.butt);
		break;
	case BASILARM_DRNL_TEST_BEAUCHAMP_LPFILTERMODE:
		IIR2Cont_Filters(signal, p->ptr.beau);
		break;
	default:
		;
	}


}

/**************************** RunModel ****************************************/

/*
 * This routine filters the input signal and puts the result into the output
 * signal.
 * It checks that all initialisation has been correctly carried out by calling
 * the appropriate checking routines.
 * The appropriate coefficients are calculated at the beginning, then the memory
 * used is released before the program returns.
 * The linear filter signal is initialised by running this routine.  Thereafter
 * memory is only re-allocated if the signal changes in dimension.  The memory
 * for the linear filter signal is free'd when the module is un-initialised.
 */

BOOLN
RunModel_BasilarM_DRNL_Test(EarObjectPtr data)
{
	static const char *funcName = "RunModel_BasilarM_DRNL_Test";
	uShort	totalChannels;
				
	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}	
	if (!CheckPars_BasilarM_DRNL_Test())
		return(FALSE);
		
	/* Initialise Variables and coefficients */
	
	SetProcessName_EarObject(data, "DRNL_Test Basilar Membrane Filtering");
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	if (!CheckRamp_SignalData(data->inSignal[0])) {
		NotifyError("%s: Input signal not correctly initialised.", funcName);
		return(FALSE);
	}
	totalChannels = bM0DRNLPtr->theCFs->numChannels *
	  data->inSignal[0]->numChannels;
	if (!InitOutFromInSignal_EarObject(data, totalChannels)) {
		NotifyError("%s: Cannot initialise output channel.", funcName);
		return(FALSE);
	}
	if (!InitProcessVariables_BasilarM_DRNL_Test(data)) {
		NotifyError("%s: Could not initialise the process variables.",
		  funcName);
		return(FALSE);
	}
	TempInputConnection_EarObject(data, bM0DRNLPtr->linearF, 1);
	InitOutFromInSignal_EarObject(bM0DRNLPtr->linearF, totalChannels);
	
	/* Filter non-linear signal path */
	if (bM0DRNLPtr->nonLinearGT1)
		GammaTone_Filters(data->outSignal, bM0DRNLPtr->nonLinearGT1);

	switch (bM0DRNLPtr->compressionMode) {
	case DRNLT_COMPRESSION_MODE_ORIGINAL:
		Compression_Filters(data->outSignal, bM0DRNLPtr->compressionPars[0],
		  bM0DRNLPtr->compressionPars[1]);
		break;
	case DRNLT_COMPRESSION_MODE_INVPOWER:
		InversePowerCompression_Filters(data->outSignal,
		  bM0DRNLPtr->compressionPars[0], bM0DRNLPtr->compressionPars[1]);
		break;
	case DRNLT_COMPRESSION_MODE_BROKENSTICK1:
		BrokenStick1Compression_Filters(data->outSignal,
		  bM0DRNLPtr->compressionPars[0], bM0DRNLPtr->compressionPars[1],
		  bM0DRNLPtr->compressionPars[2]);
		break;
	case DRNLT_COMPRESSION_MODE_UPTON_BSTICK:
		UptonBStick1Compression_Filters(data->outSignal,
		  bM0DRNLPtr->compressionPars[0], bM0DRNLPtr->compressionPars[1],
		  bM0DRNLPtr->compressionPars[2], bM0DRNLPtr->compressionPars[3]);
		break;
	default:
		;
	}
	if (bM0DRNLPtr->nonLinearGT2)
		GammaTone_Filters(data->outSignal, bM0DRNLPtr->nonLinearGT2);

	RunLPFilter_BasilarM_DRNL_Test(data->outSignal, &bM0DRNLPtr->nonLinearLP);
	
	/* Filter linear signal path */
	if (bM0DRNLPtr->linearGT)
		GammaTone_Filters(bM0DRNLPtr->linearF->outSignal, bM0DRNLPtr->linearGT);

	RunLPFilter_BasilarM_DRNL_Test(bM0DRNLPtr->linearF->outSignal,
	  &bM0DRNLPtr->linearLP);
	
	Scale_SignalData(bM0DRNLPtr->linearF->outSignal, bM0DRNLPtr->linScaler);
	Add_SignalData(data->outSignal, bM0DRNLPtr->linearF->outSignal);

	SetProcessContinuity_EarObject(data);
	return(TRUE);

}
