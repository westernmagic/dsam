/**********************
 *
 * File:		MoBMGammaC.c
 * Purpose:		This is an implementation of a GammaChirp filer using the
 *				UtGcFilters.c module. 
 * Comments:	Written using ModuleProducer version 1.2.10 (Oct  5 2000).
 * Authors:		Masashi Unoki and L. P. O'Mard
 * Created:		06 Oct 2000
 * Updated:		30 Jan 2001
 * Copyright:	(c) 2000, 2001, CNBH
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
#include "GeNSpecLists.h"
#include "UtBandwidth.h"
#include "UtCFList.h"
#include "UtFilters.h"
#include "UtGCFilters.h"
#include "MoBMGammaC.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

BMGammaCPtr	bMGammaCPtr = NULL;

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
Free_BasilarM_GammaChirp(void)
{
	/* static const char	*funcName = "Free_BasilarM_GammaChirp";  */

	if (bMGammaCPtr == NULL)
		return(FALSE);
	FreeProcessVariables_BasilarM_GammaChirp();
	Free_CFList(&bMGammaCPtr->theCFs);
	if (bMGammaCPtr->diagnosticModeList)
		free(bMGammaCPtr->diagnosticModeList);
	if (bMGammaCPtr->parList)
		FreeList_UniParMgr(&bMGammaCPtr->parList);
	if (bMGammaCPtr->parSpec == GLOBAL) {
		free(bMGammaCPtr);
		bMGammaCPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitOpModeList ********************************/

/*
 * This function initialises the 'opMode' list array
 */

BOOLN
InitOpModeList_BasilarM_GammaChirp(void)
{
	static NameSpecifier	modeList[] = {

			{ "FEED_BACK",		BASILARM_GAMMACHIRP_OPMODE_FEEDBACK },
			{ "FEED_FORWARD",	BASILARM_GAMMACHIRP_OPMODE_FEEDFORWARD },
			{ "NO_CONTROL",		BASILARM_GAMMACHIRP_OPMODE_NOCONTROL },
			{ "",	BASILARM_GAMMACHIRP_OPMODE_NULL },
		};
	bMGammaCPtr->opModeList = modeList;
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
Init_BasilarM_GammaChirp(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_BasilarM_GammaChirp";

	if (parSpec == GLOBAL) {
		if (bMGammaCPtr != NULL)
			Free_BasilarM_GammaChirp();
		if ((bMGammaCPtr = (BMGammaCPtr) malloc(sizeof(BMGammaC))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (bMGammaCPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	bMGammaCPtr->parSpec = parSpec;
	bMGammaCPtr->updateProcessVariablesFlag = TRUE;
	bMGammaCPtr->diagnosticModeFlag = FALSE;
	bMGammaCPtr->opModeFlag = FALSE;
	bMGammaCPtr->cascadeFlag = FALSE;
	bMGammaCPtr->bCoeffFlag = FALSE;
	bMGammaCPtr->cCoeff0Flag = FALSE;
	bMGammaCPtr->cCoeff1Flag = FALSE;
	bMGammaCPtr->cLowerLimFlag = FALSE;
	bMGammaCPtr->cUpperLimFlag = FALSE;
	bMGammaCPtr->diagnosticMode = 0;
	bMGammaCPtr->opMode = 0;
	bMGammaCPtr->cascade = 0;
	bMGammaCPtr->bCoeff = 0.0;
	bMGammaCPtr->cCoeff0 = 0.0;
	bMGammaCPtr->cCoeff1 = 0.0;
	bMGammaCPtr->cLowerLim = 0.0;
	bMGammaCPtr->cUpperLim = 0.0;
	bMGammaCPtr->theCFs = NULL;

	InitOpModeList_BasilarM_GammaChirp();
	if ((bMGammaCPtr->diagnosticModeList = InitNameList_NSpecLists(
	  DiagModeList_NSpecLists(0), bMGammaCPtr->diagnosticString)) == NULL)	
		return(FALSE);
	if (!SetUniParList_BasilarM_GammaChirp()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_BasilarM_GammaChirp();
		return(FALSE);
	}
	bMGammaCPtr->numChannels = 0;
	bMGammaCPtr->coefficientsGT = NULL;
	bMGammaCPtr->coefficientsERBGT = NULL;
	bMGammaCPtr->coefficientsAC = NULL;
	bMGammaCPtr->coefficientsLI = NULL;
	bMGammaCPtr->winPsEst = NULL;
	bMGammaCPtr->coefPsEst = 0.0;
	bMGammaCPtr->cmprs = 0.0;
	bMGammaCPtr->delaytimeLI = 0.0;
	bMGammaCPtr->cntlGammaC = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_BasilarM_GammaChirp(void)
{
	static const char *funcName = "SetUniParList_BasilarM_GammaChirp";
	UniParPtr	pars;

	if ((bMGammaCPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  BASILARM_GAMMACHIRP_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = bMGammaCPtr->parList->pars;
	SetPar_UniParMgr(&pars[BASILARM_GAMMACHIRP_DIAGNOSTICMODE], "DIAG_MODE",
	  "Diagnostic mode ('off', 'screen' or <file name>).",
	  UNIPAR_NAME_SPEC,
	  &bMGammaCPtr->diagnosticMode, bMGammaCPtr->diagnosticModeList,
	  (void * (*)) SetDiagnosticMode_BasilarM_GammaChirp);
	SetPar_UniParMgr(&pars[BASILARM_GAMMACHIRP_OPMODE], "OP_MODE",
	  "Operation mode ('FEED_BACK', 'FEED_FORWARD' or 'NO_CONTROL').",
	  UNIPAR_NAME_SPEC,
	  &bMGammaCPtr->opMode, bMGammaCPtr->opModeList,
	  (void * (*)) SetOpMode_BasilarM_GammaChirp);
	SetPar_UniParMgr(&pars[BASILARM_GAMMACHIRP_CASCADE], "CASCADE",
	  "Filter cascade.",
	  UNIPAR_INT,
	  &bMGammaCPtr->cascade, NULL,
	  (void * (*)) SetCascade_BasilarM_GammaChirp);
	SetPar_UniParMgr(&pars[BASILARM_GAMMACHIRP_BCOEFF], "B_COEFF",
	  "Gamma distribution envelope, 'b' coefficient (units)",
	  UNIPAR_REAL,
	  &bMGammaCPtr->bCoeff, NULL,
	  (void * (*)) SetBCoeff_BasilarM_GammaChirp);
	SetPar_UniParMgr(&pars[BASILARM_GAMMACHIRP_CCOEFF0], "C0_COEFF",
	  "'c0' coefficient.",
	  UNIPAR_REAL,
	  &bMGammaCPtr->cCoeff0, NULL,
	  (void * (*)) SetCCoeff0_BasilarM_GammaChirp);
	SetPar_UniParMgr(&pars[BASILARM_GAMMACHIRP_CCOEFF1], "C1_COEFF",
	  "'c1' coefficient.",
	  UNIPAR_REAL,
	  &bMGammaCPtr->cCoeff1, NULL,
	  (void * (*)) SetCCoeff1_BasilarM_GammaChirp);
	SetPar_UniParMgr(&pars[BASILARM_GAMMACHIRP_CLOWERLIM], "LOWER_C_LIM",
	  "Lower 'c' coefficient limit.",
	  UNIPAR_REAL,
	  &bMGammaCPtr->cLowerLim, NULL,
	  (void * (*)) SetCLowerLim_BasilarM_GammaChirp);
	SetPar_UniParMgr(&pars[BASILARM_GAMMACHIRP_CUPPERLIM], "UPPER_C_LIM",
	  "Upper 'c' coefficient limit.",
	  UNIPAR_REAL,
	  &bMGammaCPtr->cUpperLim, NULL,
	  (void * (*)) SetCUpperLim_BasilarM_GammaChirp);
	SetPar_UniParMgr(&pars[BASILARM_GAMMACHIRP_THECFS], "CFLIST",
	  "Centre frequency specification",
	  UNIPAR_CFLIST,
	  &bMGammaCPtr->theCFs, NULL,
	  (void * (*)) SetCFList_BasilarM_GammaChirp);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_BasilarM_GammaChirp(void)
{
	static const char	*funcName = "GetUniParListPtr_BasilarM_GammaChirp";

	if (bMGammaCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (bMGammaCPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(bMGammaCPtr->parList);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_BasilarM_GammaChirp(char * diagnosticMode, char * opMode,
  int cascade, double bCoeff, double cCoeff0, double cCoeff1, 
  double cLowerLim, double cUpperLim, CFListPtr theCFs)
{
	static const char	*funcName = "SetPars_BasilarM_GammaChirp";
	BOOLN	ok;

	ok = TRUE;
	if (!SetDiagnosticMode_BasilarM_GammaChirp(diagnosticMode))
		ok = FALSE;
	if (!SetOpMode_BasilarM_GammaChirp(opMode))
		ok = FALSE;
	if (!SetCascade_BasilarM_GammaChirp(cascade))
		ok = FALSE;
	if (!SetBCoeff_BasilarM_GammaChirp(bCoeff))
		ok = FALSE;
	if (!SetCCoeff0_BasilarM_GammaChirp(cCoeff0))
		ok = FALSE;
	if (!SetCCoeff1_BasilarM_GammaChirp(cCoeff1))
		ok = FALSE;
	if (!SetCLowerLim_BasilarM_GammaChirp(cLowerLim))
		ok = FALSE;
	if (!SetCUpperLim_BasilarM_GammaChirp(cUpperLim))
		ok = FALSE;
	if (!SetCFList_BasilarM_GammaChirp(theCFs))
		ok = FALSE;
	if (!ok)
		NotifyError("%s: Failed to set all module parameters." ,funcName);
	return(ok);

}

/****************************** SetDiagnosticMode *****************************/

/*
 * This function sets the module's diagnosticMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDiagnosticMode_BasilarM_GammaChirp(char * theDiagnosticMode)
{
	static const char	*funcName = "SetDiagnosticMode_BasilarM_GammaChirp";

	if (bMGammaCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	bMGammaCPtr->diagnosticModeFlag = TRUE;
	bMGammaCPtr->diagnosticMode = IdentifyDiag_NSpecLists(theDiagnosticMode,
	  bMGammaCPtr->diagnosticModeList);
	return(TRUE);

}

/****************************** SetOpMode *************************************/

/*
 * This function sets the module's opMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOpMode_BasilarM_GammaChirp(char * theOpMode)
{
	static const char	*funcName = "SetOpMode_BasilarM_GammaChirp";
	int		specifier;

	if (bMGammaCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theOpMode,
		bMGammaCPtr->opModeList)) == BASILARM_GAMMACHIRP_OPMODE_NULL) {
		NotifyError("%s: Illegal name (%s).", funcName, theOpMode);
		return(FALSE);
	}
	bMGammaCPtr->opModeFlag = TRUE;
	bMGammaCPtr->opMode = specifier;
	return(TRUE);

}

/****************************** SetCascade ************************************/

/*
 * This function sets the module's cascade parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCascade_BasilarM_GammaChirp(int theCascade)
{
	static const char	*funcName = "SetCascade_BasilarM_GammaChirp";

	if (bMGammaCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	bMGammaCPtr->cascadeFlag = TRUE;
	bMGammaCPtr->updateProcessVariablesFlag = TRUE;
	bMGammaCPtr->cascade = theCascade;
	return(TRUE);

}

/****************************** SetBCoeff *************************************/

/*
 * This function sets the module's bCoeff parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetBCoeff_BasilarM_GammaChirp(double theBCoeff)
{
	static const char	*funcName = "SetBCoeff_BasilarM_GammaChirp";

	if (bMGammaCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	bMGammaCPtr->bCoeffFlag = TRUE;
	bMGammaCPtr->bCoeff = theBCoeff;
	return(TRUE);

}

/****************************** SetCCoeff0 ************************************/

/*
 * This function sets the module's cCoeff0 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCCoeff0_BasilarM_GammaChirp(double theCCoeff0)
{
	static const char	*funcName = "SetCCoeff0_BasilarM_GammaChirp";

	if (bMGammaCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	bMGammaCPtr->cCoeff0Flag = TRUE;
	bMGammaCPtr->cCoeff0 = theCCoeff0;
	return(TRUE);

}

/****************************** SetCCoeff1 ************************************/

/*
 * This function sets the module's cCoeff1 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCCoeff1_BasilarM_GammaChirp(double theCCoeff1)
{
	static const char	*funcName = "SetCCoeff1_BasilarM_GammaChirp";

	if (bMGammaCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	bMGammaCPtr->cCoeff1Flag = TRUE;
	bMGammaCPtr->cCoeff1 = theCCoeff1;
	return(TRUE);

}

/****************************** SetCLowerLim **********************************/

/*
 * This function sets the module's cLowerLim parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCLowerLim_BasilarM_GammaChirp(double theCLowerLim)
{
	static const char	*funcName = "SetCLowerLim_BasilarM_GammaChirp";

	if (bMGammaCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	bMGammaCPtr->cLowerLimFlag = TRUE;
	bMGammaCPtr->cLowerLim = theCLowerLim;
	return(TRUE);

}

/****************************** SetCUpperLim **********************************/

/*
 * This function sets the module's cUpperLim parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCUpperLim_BasilarM_GammaChirp(double theCUpperLim)
{
	static const char	*funcName = "SetCUpperLim_BasilarM_GammaChirp";

	if (bMGammaCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	bMGammaCPtr->cUpperLimFlag = TRUE;
	bMGammaCPtr->cUpperLim = theCUpperLim;
	return(TRUE);

}

/****************************** SetCFList *************************************/

/*
 * This function sets the CFList data structure for the filter bank.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetCFList_BasilarM_GammaChirp(CFListPtr theCFList)
{
	static const char	*funcName = "SetCFList_BasilarM_GammaChirp";

	if (bMGammaCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckPars_CFList(theCFList)) {
		NotifyError("%s: Centre frequency structure not correctly set.",
		  funcName);
		return(FALSE);
	}
	if (bMGammaCPtr->theCFs != NULL)
		Free_CFList(&bMGammaCPtr->theCFs);
	bMGammaCPtr->updateProcessVariablesFlag = TRUE;
	bMGammaCPtr->theCFs = theCFList;
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
SetBandWidths_BasilarM_GammaChirp(char *theBandwidthMode, double *theBandwidths)
{
	static const char	*funcName = "SetBandWidths_BasilarM_GammaChirp";

	if (bMGammaCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!SetBandwidths_CFList(bMGammaCPtr->theCFs, theBandwidthMode,
	  theBandwidths)) {
		NotifyError("%s: Failed to set bandwidth mode.", funcName);
		return(FALSE);
	}
	bMGammaCPtr->updateProcessVariablesFlag = TRUE;
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
CheckPars_BasilarM_GammaChirp(void)
{
	static const char	*funcName = "CheckPars_BasilarM_GammaChirp";
	BOOLN	ok;

	ok = TRUE;
	if (bMGammaCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!bMGammaCPtr->diagnosticModeFlag) {
		NotifyError("%s: diagnosticMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!bMGammaCPtr->opModeFlag) {
		NotifyError("%s: opMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!bMGammaCPtr->cascadeFlag) {
		NotifyError("%s: cascade variable not set.", funcName);
		ok = FALSE;
	}
	if (!bMGammaCPtr->bCoeffFlag) {
		NotifyError("%s: bCoeff variable not set.", funcName);
		ok = FALSE;
	}
	if (!bMGammaCPtr->cCoeff0Flag) {
		NotifyError("%s: cCoeff0 variable not set.", funcName);
		ok = FALSE;
	}
	if (!bMGammaCPtr->cCoeff1Flag) {
		NotifyError("%s: cCoeff1 variable not set.", funcName);
		ok = FALSE;
	}
	if (!bMGammaCPtr->cLowerLimFlag) {
		NotifyError("%s: cLowerLim variable not set.", funcName);
		ok = FALSE;
	}
	if (!bMGammaCPtr->cUpperLimFlag) {
		NotifyError("%s: cUpperLim variable not set.", funcName);
		ok = FALSE;
	}
	if (!CheckPars_CFList(bMGammaCPtr->theCFs)) {
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
GetCFListPtr_BasilarM_GammaChirp(void)
{
	static const char	*funcName = "GetCFListPtr_BasilarM_GammaChirp";

	if (bMGammaCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (bMGammaCPtr->theCFs == NULL) {
		NotifyError("%s: CFList data structure has not been correctly set.  "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(bMGammaCPtr->theCFs);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the lUTEar.parsFile file pointer.
 */

BOOLN
PrintPars_BasilarM_GammaChirp(void)
{
	static const char	*funcName = "PrintPars_BasilarM_GammaChirp";

	if (!CheckPars_BasilarM_GammaChirp()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("GammaChirp BM Filter Module Parameters:-\n");
	DPrint("\tDiagnostic mode: %s,", bMGammaCPtr->diagnosticModeList[
	  bMGammaCPtr->diagnosticMode].name);
	DPrint("\tOperation mode: %s \n", bMGammaCPtr->opModeList[
	  bMGammaCPtr->opMode].name);
	DPrint("\tFilter cascade = %d,", bMGammaCPtr->cascade);
	DPrint("\tCoefficient, b = %g,\n", bMGammaCPtr->bCoeff);
	DPrint("\tCoefficient, c0 = %g,", bMGammaCPtr->cCoeff0);
	DPrint("\tCoefficient, c1 = %g\n", bMGammaCPtr->cCoeff1);
	DPrint("\tLower/upper c coefficient limits = %g / %g\n",
	  bMGammaCPtr->cLowerLim, bMGammaCPtr->cUpperLim);
	PrintPars_CFList(bMGammaCPtr->theCFs);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_BasilarM_GammaChirp(char *fileName)
{
	static const char	*funcName = "ReadPars_BasilarM_GammaChirp";
	BOOLN	ok = TRUE;
	char	*filePath, diagnosticMode[MAXLINE], opMode[MAXLINE];
	int		cascade;
	double	bCoeff, cCoeff0, cCoeff1, cLowerLim, cUpperLim;
	CFListPtr	theCFs;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(filePath, "r")) == NULL) {
		NotifyError("%s: Cannot open data file '%s'.\n", funcName, fileName);
		return(FALSE);
	}
	DPrint("%s: Reading from '%s':\n", funcName, fileName);
	Init_ParFile();
	if (!GetPars_ParFile(fp, "%s", diagnosticMode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%s", opMode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%d", &cascade))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &bCoeff))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &cCoeff0))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &cCoeff1))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &cLowerLim))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &cUpperLim))
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
	if (!SetPars_BasilarM_GammaChirp(diagnosticMode, opMode, cascade, 
	  bCoeff, cCoeff0, cCoeff1, cLowerLim, cUpperLim, theCFs)) {
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
SetParsPointer_BasilarM_GammaChirp(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_BasilarM_GammaChirp";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	bMGammaCPtr = (BMGammaCPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_BasilarM_GammaChirp(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_BasilarM_GammaChirp";

	if (!SetParsPointer_BasilarM_GammaChirp(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_BasilarM_GammaChirp(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = bMGammaCPtr;
	theModule->CheckPars = CheckPars_BasilarM_GammaChirp;
	theModule->Free = Free_BasilarM_GammaChirp;
	theModule->GetUniParListPtr = GetUniParListPtr_BasilarM_GammaChirp;
	theModule->PrintPars = PrintPars_BasilarM_GammaChirp;
	theModule->ReadPars = ReadPars_BasilarM_GammaChirp;
	theModule->RunProcess = RunModel_BasilarM_GammaChirp;
	theModule->SetParsPointer = SetParsPointer_BasilarM_GammaChirp;
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
CheckData_BasilarM_GammaChirp(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_BasilarM_GammaChirp";

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInit_SignalData(data->inSignal[0], funcName))
		return(FALSE);
	/*** Put additional checks here. ***/
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

void
FreeProcessVariables_BasilarM_GammaChirp(void)
{
	int	nch;
	BMGammaCPtr	p = bMGammaCPtr;


				/* Free of GammaToneCoeffs_Filters */
	if (p->coefficientsERBGT) {
		for (nch = 0; nch < p->numChannels; nch++)
    		FreeERBGammaToneCoeffs_GCFilters(&p->coefficientsERBGT[nch]);
		free(p->coefficientsERBGT);
		p->coefficientsERBGT = NULL;
	}
	if (p->coefficientsGT) {
		for (nch = 0; nch < p->numChannels; nch++)
    		FreeGammaToneCoeffs_Filters(&p->coefficientsGT[nch]);
		free(p->coefficientsGT);
		p->coefficientsGT = NULL;
	}
				/* Free of AsymCmpCoeffs_Filters */
	if (p->coefficientsAC) {
		for (nch = 0; nch < p->numChannels; nch++)
    		FreeAsymCmpCoeffs_GCFilters(&p->coefficientsAC[nch]);
		free(p->coefficientsAC);
		p->coefficientsAC = NULL;
	}
				/* Free of LeakyIntCoeffs_GCFilters */
	if (p->coefficientsLI) {
		for (nch = 0; nch < p->numChannels; nch++)
    		FreeLeakyIntCoeffs_GCFilters(&p->coefficientsLI[nch]);
		free(p->coefficientsLI);
		p->coefficientsLI = NULL;
	}
				/* Free of ERBWindow_Filters */
	if (p->winPsEst) {
 		FreeDoubleArray_Common(&p->winPsEst);
		free(p->winPsEst);
		p->winPsEst = NULL;
	}
				/* Free of CntlGammaChirp_Filters */
	if (p->cntlGammaC) {
		for (nch = 0; nch < p->numChannels; nch++)
    		FreeCntlGammaChirp_GCFilters(&p->cntlGammaC[nch]);
		free(p->cntlGammaC);
		p->cntlGammaC = NULL;
	}

	p->updateProcessVariablesFlag = TRUE;

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 */

BOOLN
InitProcessVariables_BasilarM_GammaChirp(EarObjectPtr data)
{
	static const char *funcName = "InitProcessVariables_BasilarM_GammaChirp";
	int	nch, nsmpl, cFIndex, stateVectorLength;
	double	sampleRate, *ptr;
	BMGammaCPtr	p = bMGammaCPtr;
	
	if (p->updateProcessVariablesFlag || data->updateProcessFlag ||
	  p->theCFs->updateFlag) {
		FreeProcessVariables_BasilarM_GammaChirp();
		p->numChannels = data->outSignal->numChannels;
				/* Memory allocate of GammaToneCoeffsPtr */
		if (p->cascade == 4) {
			if ((p->coefficientsERBGT = (ERBGammaToneCoeffsPtr *) calloc(
			  p->numChannels, sizeof(ERBGammaToneCoeffsPtr))) == NULL) {
				NotifyError("%s: Out of memory.", funcName);
				return(FALSE);
			}
		} else {
			if ((p->coefficientsGT = (GammaToneCoeffsPtr *) calloc(
			  p->numChannels, sizeof(GammaToneCoeffsPtr))) == NULL) {
				NotifyError("%s: Out of memory.", funcName);
				return(FALSE);
			}
		}
				/* Memory allocate of AsymCmpCoeffsPtr */
		if ((p->coefficientsAC = (AsymCmpCoeffsPtr *) calloc(p->numChannels,
		  sizeof(AsymCmpCoeffsPtr))) == NULL) {
		 	NotifyError("%s: Out of memory.", funcName);
		 	return(FALSE);
		}
				/* Memory allocate of OnePoleCoeffsPtr */
		if ((p->coefficientsLI = (OnePoleCoeffsPtr *) calloc(p->numChannels,
		  sizeof(OnePoleCoeffsPtr))) == NULL) {
		 	NotifyError("%s: Out of memory.", funcName);
		 	return(FALSE);
		}

		if ((p->winPsEst = (double *) calloc(p->numChannels * p->numChannels,
		  sizeof(double))) == NULL) {
		 	NotifyError("%s: Out of memory.", funcName);
		 	return(FALSE);
		}

		if ((p->cntlGammaC = (CntlGammaCPtr *) calloc(p->numChannels,
		  sizeof(CntlGammaCPtr))) == NULL) {
		 	NotifyError("%s: Out of memory.", funcName);
		 	return(FALSE);
		}

		sampleRate = 1.0 / data->inSignal[0]->dt;

		p->coefPsEst = BASILARM_GAMMACHIRP_COEFF_PS_EST;
		p->cmprs = BASILARM_GAMMACHIRP_COEFF_CMPRS;
		p->delaytimeLI = BASILARM_GAMMACHIRP_DELAY_TIME_LI;

		if (p->cascade == 4) {
			for (nch = 0; nch < p->numChannels; nch++) {
				cFIndex = nch / data->inSignal[0]->interleaveLevel;
				p->coefficientsERBGT[nch] = InitERBGammaToneCoeffs_GCFilters(
				  p->theCFs->frequency[cFIndex], p->theCFs->bandwidth[cFIndex],
				  p->bCoeff, p->cascade, sampleRate);
			}
		} else {
			for (nch = 0; nch < p->numChannels; nch++) {
				cFIndex = nch / data->inSignal[0]->interleaveLevel;
				if ((p->coefficientsGT[nch] = InitGammaToneCoeffs_Filters(
				  p->theCFs->frequency[cFIndex], p->theCFs->bandwidth[cFIndex],
				  p->cascade, sampleRate)) == NULL) {
					NotifyError("%s: Could not initialise coefficients for "
					  "channel %d.", funcName, nch);
					return(FALSE);
				}
			}
		}
		for (nch = 0; nch < p->numChannels; nch++) {
			p->coefficientsAC[nch] = InitAsymCmpCoeffs_GCFilters();
			p->coefficientsLI[nch] = InitLeakyIntCoeffs_GCFilters(
			  p->delaytimeLI, sampleRate);
			p->cntlGammaC[nch] = InitCntlGammaChirp_GCFilters();
		}
		p->winPsEst = InitERBWindow_GCFilters(p->theCFs->eRBDensity,
		  p->numChannels);

		SetLocalInfoFlag_SignalData(data->outSignal, TRUE);
		SetInfoChannelTitle_SignalData(data->outSignal, "Frequency (Hz)");
		SetInfoChannelLabels_SignalData(data->outSignal, p->theCFs->frequency);
		SetInfoCFArray_SignalData(data->outSignal, p->theCFs->frequency);
		p->updateProcessVariablesFlag = FALSE;
		p->theCFs->updateFlag = FALSE;

	} else if (data->timeIndex == PROCESS_START_TIME) {
		stateVectorLength = p->cascade * 
		  FILTERS_NUM_GAMMAT_STATE_VARS_PER_FILTER;
		if (p->cascade == 4) {
			for (nch = 0; nch < data->outSignal->numChannels; nch++) {
				ptr = p->coefficientsERBGT[nch]->stateVector;
				for (nsmpl = 0; nsmpl < stateVectorLength; nsmpl++)
					*ptr++ = 0.0;
			}
		} else {
			for (nch = 0; nch < data->outSignal->numChannels; nch++) {
				ptr = p->coefficientsGT[nch]->stateVector;
				for (nsmpl = 0; nsmpl < stateVectorLength; nsmpl++)
					*ptr++ = 0.0;
			}
		}
		stateVectorLength = GCFILTERS_NUM_CASCADE_ACF_FILTER *
			GCFILTERS_NUM_ACF_STATE_VARS_PER_FILTER;
		for (nch = 0; nch < data->outSignal->numChannels; nch++) {
			ptr = p->coefficientsAC[nch]->stateFVector;
			for (nsmpl = 0; nsmpl < stateVectorLength; nsmpl++)
				*ptr++ = 0.0;
			ptr = p->coefficientsAC[nch]->stateBVector;
			for (nsmpl = 0; nsmpl < stateVectorLength; nsmpl++)
				*ptr++ = 0.0;
		}
		stateVectorLength = GCFILTERS_NUM_LI_STATE_VARS_PER_FILTER; 
		for (nch = 0; nch < data->outSignal->numChannels; nch++) {
			ptr = p->coefficientsLI[nch]->stateVector;
			for (nsmpl = 0; nsmpl < stateVectorLength; nsmpl++)
				*ptr++ = 0.0;
		}
		for (nch = 0; nch < data->outSignal->numChannels; nch++) {
			p->cntlGammaC[nch]->outSignalLI = 0.0;
			p->cntlGammaC[nch]->aEst = 0.0;
			p->cntlGammaC[nch]->psEst = 0.0;
			if (p->opMode == BASILARM_GAMMACHIRP_OPMODE_NOCONTROL) 	
					/* opMode=1 -> "NC" */
				p->cntlGammaC[nch]->cEst = p->cCoeff0;
			else
				p->cntlGammaC[nch]->cEst = 0.0;
		}
	}
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
RunModel_BasilarM_GammaChirp(EarObjectPtr data)
{
	static const char	*funcName = "RunModel_BasilarM_GammaChirp";
	uShort	totalChannels;
	int		nch, cEstCnt, cFIndex;
	double	sampleRate;
	ChanLen	nsmpl;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckPars_BasilarM_GammaChirp())
		return(FALSE);
	if (!CheckData_BasilarM_GammaChirp(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "GammaChirp basilar membrane filtering "
	  "(Slaney GT)");
	if (!CheckRamp_SignalData(data->inSignal[0])) {
		NotifyError("%s: Input signal not correctly initialised.", funcName);
		return(FALSE);
	}
	totalChannels = bMGammaCPtr->theCFs->numChannels * data->inSignal[
	  0]->numChannels;
	if (!InitOutFromInSignal_EarObject(data, totalChannels)) {
		NotifyError("%s: Cannot initialise output channel.", funcName);
		return(FALSE);
	}
	if (!InitProcessVariables_BasilarM_GammaChirp(data)) {
		NotifyError("%s: Could not initialise the process variables.",
		  funcName);
		return(FALSE);
	}
	/* Beginig of GammaChirp_Filters */

	for (nch = 0; nch < data->outSignal->numChannels; nch++) {
		bMGammaCPtr->cntlGammaC[nch]->outSignalLI = 0.0;
		bMGammaCPtr->cntlGammaC[nch]->aEst = 0.0;
		bMGammaCPtr->cntlGammaC[nch]->psEst = 0.0;
		if (bMGammaCPtr->opMode == BASILARM_GAMMACHIRP_OPMODE_NOCONTROL)
				 		/* opMode=1 -> "NC" */
			bMGammaCPtr->cntlGammaC[nch]->cEst = bMGammaCPtr->cCoeff0;
		else
			bMGammaCPtr->cntlGammaC[nch]->cEst = 0.0;
	}

	if (bMGammaCPtr->cascade == 4)
		ERBGammaTone_GCFilters(data->outSignal, bMGammaCPtr->coefficientsERBGT);
	else
		GammaTone_Filters(data->outSignal, bMGammaCPtr->coefficientsGT);

	cEstCnt = 0;
	for (nch = 0; nch < data->outSignal->numChannels; nch++)
		if (bMGammaCPtr->cntlGammaC[nch]->cEst != 0.0)
			cEstCnt++;

	if ((bMGammaCPtr->opMode != BASILARM_GAMMACHIRP_OPMODE_NOCONTROL) || 
				(cEstCnt != 0)) {	/* opMode=2 -> "NC" */

		sampleRate = 1.0 / data->inSignal[0]->dt;
		for (nsmpl = 0; nsmpl < data->outSignal->length; nsmpl++) {

			if (bMGammaCPtr->opMode == BASILARM_GAMMACHIRP_OPMODE_FEEDFORWARD) {
						/* opMode=1 -> "FF" */
				CntlGammaChirp_GCFilters(data->outSignal, nsmpl, 
					bMGammaCPtr->cntlGammaC, bMGammaCPtr->cCoeff0,
					bMGammaCPtr->cCoeff1, bMGammaCPtr->cLowerLim,
					bMGammaCPtr->cUpperLim, bMGammaCPtr->winPsEst,
					bMGammaCPtr->coefPsEst, bMGammaCPtr->cmprs,
					bMGammaCPtr->coefficientsLI);
            }

			for (nch = 0; nch < bMGammaCPtr->numChannels; nch++) {
				cFIndex = nch / data->inSignal[0]->interleaveLevel;
				CalcAsymCmpCoeffs_GCFilters(bMGammaCPtr->coefficientsAC[nch],
					bMGammaCPtr->theCFs->frequency[cFIndex],
					bMGammaCPtr->theCFs->bandwidth[cFIndex],
					bMGammaCPtr->bCoeff, bMGammaCPtr->cntlGammaC[nch]->cEst,
					bMGammaCPtr->cascade, sampleRate);
			}

			AsymCmp_GCFilters(data->outSignal, nsmpl,
			  bMGammaCPtr->coefficientsAC);

			if (bMGammaCPtr->opMode == BASILARM_GAMMACHIRP_OPMODE_FEEDBACK) {
						/* opMode=0 -> "FB" */
				CntlGammaChirp_GCFilters(data->outSignal, nsmpl, 
					bMGammaCPtr->cntlGammaC, bMGammaCPtr->cCoeff0,
					bMGammaCPtr->cCoeff1, bMGammaCPtr->cLowerLim,
					bMGammaCPtr->cUpperLim, bMGammaCPtr->winPsEst,
					bMGammaCPtr->coefPsEst, bMGammaCPtr->cmprs,
					bMGammaCPtr->coefficientsLI);
			}
		}
	}

	/* end of GammaChirp_GCFilters */

	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

