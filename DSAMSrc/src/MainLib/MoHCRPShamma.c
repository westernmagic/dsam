/**********************
 *
 * File:		MoHCRPShamma.c
 * Purpose:		This module contains the model for the Shamme hair cell receptor
 *				potential: Shamm, S. A. Chadwick R. S. Wilbur W. J. Morrish
 *				K. A. and Rinzel J.(1986) "A biophysical model oc cochlear
 *				processing: Intensity dependence of pure tone responses",
 *				J. Acoust. Soc. Am. [80], pp 133-145.
 * Comments:	The program uses a mixture of "hard" and "generic" programming:
 *				The test module is used in "hard" programming mode for testing
 *				purposes.
 *				05 Nov 96: Changed to interpret DRNL output as displacement -
 *				see Essex:24b.
 * Author:		L. P. O'Mard
 * Created:		18 Feb 1993
 * Updated:	    05 Nov 1996
 * Copyright:	(c) 1998, University of Essex.
 *
 **********************/

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
#include "MoHCRPShamma.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

ShammaPtr	shammaPtr = NULL;

/******************************************************************************/
/********************************* Subroutines and functions ******************/
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
Init_IHCRP_Shamma(ParameterSpecifier parSpec)
{
	static const char *funcName = "Init_IHCRP_Shamma";

	if (parSpec == GLOBAL) {
		if (shammaPtr != NULL)
			Free_IHCRP_Shamma();
		if ((shammaPtr = (ShammaPtr) malloc(sizeof(Shamma))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (shammaPtr == NULL) { 
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	shammaPtr->parSpec = parSpec;
	shammaPtr->updateProcessVariablesFlag = TRUE;
	shammaPtr->endocochlearPotEtFlag = TRUE;
	shammaPtr->reversalPotEkFlag = TRUE;
	shammaPtr->reversalPotCorrectionFlag = TRUE;
	shammaPtr->totalCapacitanceCFlag = TRUE;
	shammaPtr->restingConductanceG0Flag = TRUE;
	shammaPtr->kConductanceGkFlag = TRUE;
	shammaPtr->maxMConductanceGmaxFlag = TRUE;
	shammaPtr->ciliaCouplingGainFlag = TRUE;
	shammaPtr->ciliaTimeConstTcFlag = TRUE;
	shammaPtr->betaFlag = TRUE;
	shammaPtr->gammaFlag = TRUE;
	shammaPtr->referencePotFlag = TRUE;
	shammaPtr->endocochlearPot_Et = 0.1;
	shammaPtr->reversalPot_Ek = -0.084;
	shammaPtr->reversalPotCorrection = 0.04;
	shammaPtr->totalCapacitance_C = 6.21e-12;
	shammaPtr->restingConductance_G0 = 4.3e-09;
	shammaPtr->kConductance_Gk = 1.07e-08;
	shammaPtr->maxMConductance_Gmax = 4.18e-09;
	shammaPtr->beta = 0.25;
	shammaPtr->gamma = 1e+07;
	shammaPtr->ciliaCouplingGain_C = 16.0;
	shammaPtr->ciliaTimeConst_tc = 0.3e-3;
	shammaPtr->referencePot = 0.0;

	if (!SetUniParList_IHCRP_Shamma()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_IHCRP_Shamma();
		return(FALSE);
	}
	shammaPtr->lastCiliaDisplacement_u = NULL;
	shammaPtr->lastInput = NULL;
	shammaPtr->lastOutput = NULL;
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
Free_IHCRP_Shamma(void)
{
	if (shammaPtr == NULL)
		return(TRUE);
	FreeProcessVariables_IHCRP_Shamma();
	if (shammaPtr->parList)
		FreeList_UniParMgr(&shammaPtr->parList);
	if (shammaPtr->parSpec == GLOBAL) {
		free(shammaPtr);
		shammaPtr = NULL;
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
SetUniParList_IHCRP_Shamma(void)
{
	static const char *funcName = "SetUniParList_IHCRP_Shamma";
	UniParPtr	pars;

	if ((shammaPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  IHCRP_SHAMMA_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = shammaPtr->parList->pars;
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA_ENDOCOCHLEARPOT_ET], "E_T",
	  "Endocochlear potential, Et (V).",
	  UNIPAR_REAL,
	  &shammaPtr->endocochlearPot_Et, NULL,
	  (void * (*)) SetEndocochlearPot_IHCRP_Shamma);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA_REVERSALPOT_EK], "E_K",
	  "Reversal potential, Ek (V).",
	  UNIPAR_REAL,
	  &shammaPtr->reversalPot_Ek, NULL,
	  (void * (*)) SetReversalPot_IHCRP_Shamma);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA_REVERSALPOTCORRECTION], "RP_CORRECTION",
	  "Reversal potential correction, Rp/(Rt+Rp).",
	  UNIPAR_REAL,
	  &shammaPtr->reversalPotCorrection, NULL,
	  (void * (*)) SetReversalPotCorrection_IHCRP_Shamma);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA_TOTALCAPACITANCE_C], "C_TOTAL",
	  "Total capacitance, C = Ca + Cb (F).",
	  UNIPAR_REAL,
	  &shammaPtr->totalCapacitance_C, NULL,
	  (void * (*)) SetTotalCapacitance_IHCRP_Shamma);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA_RESTINGCONDUCTANCE_G0], "G0",
	  "Resting conductance, G0 (S).",
	  UNIPAR_REAL,
	  &shammaPtr->restingConductance_G0, NULL,
	  (void * (*)) SetRestingConductance_IHCRP_Shamma);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA_KCONDUCTANCE_GK], "G_K",
	  "Potassium conductance, Gk (S = Siemens).",
	  UNIPAR_REAL,
	  &shammaPtr->kConductance_Gk, NULL,
	  (void * (*)) SetKConductance_IHCRP_Shamma);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA_MAXMCONDUCTANCE_GMAX], "G_MAXC",
	  "Maximum mechanical conductance, Gmax (S).",
	  UNIPAR_REAL,
	  &shammaPtr->maxMConductance_Gmax, NULL,
	  (void * (*)) SetMaxMConductance_IHCRP_Shamma);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA_BETA], "BETA",
	  "Beta constant, exp(-G1/RT).",
	  UNIPAR_REAL,
	  &shammaPtr->beta, NULL,
	  (void * (*)) SetBeta_IHCRP_Shamma);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA_GAMMA], "GAMMA",
	  "Gamma constant, Z1/RT (/m).",
	  UNIPAR_REAL,
	  &shammaPtr->gamma, NULL,
	  (void * (*)) SetGamma_IHCRP_Shamma);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA_CILIATIMECONST_TC], "T_C",
	  "Cilia/BM time constant (s).",
	  UNIPAR_REAL,
	  &shammaPtr->ciliaTimeConst_tc, NULL,
	  (void * (*)) SetCiliaTimeConstTc_IHCRP_Shamma);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA_CILIACOUPLINGGAIN_C], "GAIN_C",
	  "Cilia/BM coupling gain, C (dB).",
	  UNIPAR_REAL,
	  &shammaPtr->ciliaCouplingGain_C, NULL,
	  (void * (*)) SetCiliaCouplingGain_IHCRP_Shamma);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA_REFERENCEPOT], "REF_POT",
	  "Reference potential (V).",
	  UNIPAR_REAL,
	  &shammaPtr->referencePot, NULL,
	  (void * (*)) SetReferencePot_IHCRP_Shamma);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_IHCRP_Shamma(void)
{
	static const char	*funcName = "GetUniParListPtr_IHCRP_Shamma";

	if (shammaPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (shammaPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(shammaPtr->parList);

}

/********************************* SetEndocochlearPot *************************/

/*
 * This function sets the module's endococlear, Et, parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetEndocochlearPot_IHCRP_Shamma(double theEndocochlearPot)
{
	static const char	*funcName = "SetEndocochlearPot_IHCRP_Shamma";

	if (shammaPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	shammaPtr->endocochlearPotEtFlag = TRUE;
	shammaPtr->endocochlearPot_Et = theEndocochlearPot;
	shammaPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetReversalPot *****************************/

/*
 * This function sets the module's reversal potential, Ek, parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetReversalPot_IHCRP_Shamma(double theReversalPot)
{
	static const char	*funcName = "SetReversalPot_IHCRP_Shamma";

	if (shammaPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	shammaPtr->reversalPotEkFlag = TRUE;
	shammaPtr->reversalPot_Ek = theReversalPot;
	shammaPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetReversalPotCorrection *******************/

/*
 * This function sets the module's reversal potential correction, Rp/(Rt+Rp),
 * parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetReversalPotCorrection_IHCRP_Shamma(double theReversalPotCorrection)
{
	static const char	*funcName = "SetReversalPotCorrection_IHCRP_Shamma";

	if (shammaPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	shammaPtr->reversalPotCorrectionFlag = TRUE;
	shammaPtr->reversalPotCorrection = theReversalPotCorrection;
	shammaPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetTotalCapacitance ************************/

/*
 * This function sets the module's total capacitance, C = Ca + Cb, parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetTotalCapacitance_IHCRP_Shamma(double theTotalCapacitance)
{
	static const char	*funcName = "SetTotalCapacitance_IHCRP_Shamma";

	if (shammaPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	shammaPtr->totalCapacitanceCFlag = TRUE;
	shammaPtr->totalCapacitance_C = theTotalCapacitance;
	shammaPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetRestingConductance **********************/

/*
 * This function sets the module's resting conductance, G0, parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetRestingConductance_IHCRP_Shamma(double theRestingConductance)
{
	static const char	*funcName = "SetRestingConductance_IHCRP_Shamma";

	if (shammaPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	shammaPtr->restingConductanceG0Flag = TRUE;
	shammaPtr->restingConductance_G0 = theRestingConductance;
	shammaPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetKConductance ****************************/

/*
 * This function sets the module's potassium conductance, Gk, parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetKConductance_IHCRP_Shamma(double theKConductance)
{
	static const char	*funcName = "SetKConductance_IHCRP_Shamma";

	if (shammaPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	shammaPtr->kConductanceGkFlag = TRUE;
	shammaPtr->kConductance_Gk = theKConductance;
	shammaPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetMaxMConductance *************************/

/*
 * This function sets the module's maximum mechanical sensitive conductance,
 * Gmax, parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetMaxMConductance_IHCRP_Shamma(double theMaxMConductance)
{
	static const char	 *funcName = "SetMaxMConductance_IHCRP_Shamma";

	if (shammaPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	shammaPtr->maxMConductanceGmaxFlag = TRUE;
	shammaPtr->maxMConductance_Gmax = theMaxMConductance;
	shammaPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetBeta ************************************/

/*
 * This function sets the module's beta = exp(-G1/RT) parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetBeta_IHCRP_Shamma(double theBeta)
{
	static const char	 *funcName = "SetBeta_IHCRP_Shamma";

	if (shammaPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	shammaPtr->betaFlag = TRUE;
	shammaPtr->beta = theBeta;
	shammaPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetGamma ***********************************/

/*
 * This function sets the module's gamma = Z1/RT parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetGamma_IHCRP_Shamma(double theGamma)
{
	static const char	 *funcName = "SetGamma_IHCRP_Shamma";

	if (shammaPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	shammaPtr->gammaFlag = TRUE;
	shammaPtr->gamma = theGamma;
	shammaPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetCiliaCouplingGain ***********************/

/*
 * This function sets the module's cilia coupling gain, n, parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetCiliaCouplingGain_IHCRP_Shamma(double theCiliaCoupingGain)
{
	static const char	 *funcName = "SetCiliaCouplingGain_IHCRP_Shamma";

	if (shammaPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	shammaPtr->ciliaCouplingGainFlag = TRUE;
	shammaPtr->ciliaCouplingGain_C = theCiliaCoupingGain;
	shammaPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetCiliaTimeConstTc ************************/

/*
 * This function sets the module's BM/cilia time constant, tc, parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetCiliaTimeConstTc_IHCRP_Shamma(double theCiliaTimeConstTc)
{
	static const char	 *funcName = "SetCiliaTimeConstTc_IHCRP_Shamma";

	if (shammaPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	shammaPtr->ciliaTimeConstTcFlag = TRUE;
	shammaPtr->ciliaTimeConst_tc = theCiliaTimeConstTc;
	shammaPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetReferencePot ****************************/

/*
 * This function sets the module's reference Potential, Eref, parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetReferencePot_IHCRP_Shamma(double theReferencePot)
{
	static const char	 *funcName = "SetCiliaTimeConstTc_IHCRP_Shamma";
	if (shammaPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	shammaPtr->referencePotFlag = TRUE;
	shammaPtr->referencePot = theReferencePot;
	shammaPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetPars ************************************/

/*
 * This function sets all the module's parameters.
 */

BOOLN
SetPars_IHCRP_Shamma(double Et, double Ek, double reversalPotCorrection,
  double C, double G0, double Gk, double Gmax, double beta, double gamma,
  double tc, double n, double Eref)
{
	BOOLN	ok;
	
	ok = TRUE;
	if (!SetBeta_IHCRP_Shamma(beta))
		ok = FALSE;
	if (!SetGamma_IHCRP_Shamma(gamma))
		ok = FALSE;
	if (!SetEndocochlearPot_IHCRP_Shamma(Et))
		ok = FALSE;
	if (!SetRestingConductance_IHCRP_Shamma(G0))
		ok = FALSE;
	if (!SetKConductance_IHCRP_Shamma(Gk))
		ok = FALSE;
	if (!SetMaxMConductance_IHCRP_Shamma(Gmax))
		ok = FALSE;
	if (!SetReversalPot_IHCRP_Shamma(Ek))
		ok = FALSE;
	if (!SetReversalPotCorrection_IHCRP_Shamma(reversalPotCorrection))
		ok = FALSE;
	if (!SetTotalCapacitance_IHCRP_Shamma(C))
		ok = FALSE;
	if (!SetCiliaTimeConstTc_IHCRP_Shamma(tc))
		ok = FALSE;
	if (!SetCiliaCouplingGain_IHCRP_Shamma(n))
		ok = FALSE;
	if (!SetReferencePot_IHCRP_Shamma(Eref))
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
CheckPars_IHCRP_Shamma(void)
{
	static const char *funcName = "CheckPars_IHCRP_Shamma";
	BOOLN	ok;
	
	ok = TRUE;
	if (shammaPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!shammaPtr->endocochlearPotEtFlag) {
		NotifyError("%s: Endocochlear potential, Et, not correctly set.",
		  funcName);
		ok = FALSE;
	}
	if (!shammaPtr->reversalPotEkFlag) {
		NotifyError("%s: Reversal potential, Ek, not correctly set.",
		  funcName);
		ok = FALSE;
	}
	if (!shammaPtr->reversalPotCorrectionFlag) {
		NotifyError("%s: Reversal potential correction, Rp/(Rt+Rp), not "\
		  "correctly set.", funcName);
		ok = FALSE;
	}
	if (!shammaPtr->totalCapacitanceCFlag) {
		NotifyError("%s: Total capacitance, C, not correctly set.",
		  funcName);
		ok = FALSE;
	}
	if (!shammaPtr->restingConductanceG0Flag) {
		NotifyError("%s: Resting conductanc, G0, not correctly set.",
		  funcName);
		ok = FALSE;
	}
	if (!shammaPtr->kConductanceGkFlag) {
		NotifyError("%s: Potassium conductanc, Gk, not correctly set.",
		  funcName);
		ok = FALSE;
	}
	if (!shammaPtr->maxMConductanceGmaxFlag) {
		NotifyError("%s: Maximum mechinally sensitive, Gmax, not correctly "\
		  "set.", funcName);
		ok = FALSE;
	}
	if (!shammaPtr->betaFlag) {
		NotifyError("%s: Beta constant, not correctly set.", funcName);
		ok = FALSE;
	}
	if (!shammaPtr->gammaFlag) {
		NotifyError("%s: Gamma constant, not correctly set.", funcName);
		ok = FALSE;
	}
	if (!shammaPtr->ciliaTimeConstTcFlag) {
		NotifyError("%s: BM/Cilia displacement time constant, tc, not "\
		  "correctly set.", funcName);
		ok = FALSE;
	}
	if (!shammaPtr->ciliaCouplingGainFlag) {
		NotifyError("%s: BM/Cilia coupling gain constant, n, not correctly "\
		  "set.", funcName);
		ok = FALSE;
	}
	if (!shammaPtr->referencePotFlag) {
		NotifyError("CheckPars_IHCRP_Shamma: Reference potential, Eref, not "\
		  "set.");
		ok = FALSE;
	}
	return(ok);
		
}

/****************************** PrintPars *************************************/

/*
 * This program prints the parameters of the module to the standard output.
 */
 
BOOLN
PrintPars_IHCRP_Shamma(void)
{
	static const char *funcName = "PrintPars_IHCRP_Shamma(";

	if (!CheckPars_IHCRP_Shamma()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Shamma et al. Receptor Potential Module "\
	  "Parameters:-\n");
	DPrint("\tEndocochlear potential, Et = %g V,\n",
	  shammaPtr->endocochlearPot_Et);
	DPrint("\tReversal potential, Ek = %g V,\n",
	  shammaPtr->reversalPot_Ek);
	DPrint("\tReversal potential correction Rp/(Rt+Rp) = "\
	  "%g,\n", shammaPtr->reversalPotCorrection);
	DPrint("\tTotal capacitance, Ct = %g F,\n",
	  shammaPtr->totalCapacitance_C);
	DPrint("\tResting conductance, G0 = %g S,\n",
	  shammaPtr->restingConductance_G0);
	DPrint("\tPotassium conductance, Gk = %g S,\n",
	  shammaPtr->kConductance_Gk);
	DPrint("\tMaximum mechanically sensitive Conductance, "\
	  "Gmax = %g S,\n", shammaPtr->maxMConductance_Gmax);
	DPrint("\tBeta = %g,\tgamma = %g /m.\n",
	  shammaPtr->beta, shammaPtr->gamma);
	DPrint("\tBM/Cilia: time constant = %g ms,\t gain = %g "\
	  "dB\n", MSEC(shammaPtr->ciliaTimeConst_tc),
	  shammaPtr->ciliaCouplingGain_C);
	DPrint("\tReference potential = %g V\n", 
	  shammaPtr->referencePot);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.
 */
 
BOOLN
ReadPars_IHCRP_Shamma(char *fileName)
{
	static const char *funcName = "ReadPars_IHCRP_Shamma";
	BOOLN	ok;
	char	*filePath;
	double	endocochlearPot_Et, reversalPot_Ek, reversalPotCorrection;
	double	totalCapacitance_C, restingConductance_G0, kConductance_Gk;
	double	maxMConductance_Gmax;
	double	beta, gamma, ciliaTimeConst_tc, ciliaCouplingGain_C;
	double	referencePot;
	FILE    *fp;
    
	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(filePath, "r")) == NULL) {
		NotifyError("%s: Cannot open data file '%s'.\n", funcName, filePath);
		return(FALSE);
	}
	DPrint("%s: Reading from '%s':\n", funcName, filePath);
	Init_ParFile();
	ok = TRUE;
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
	if (!GetPars_ParFile(fp, "%lf", &beta))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &gamma))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &ciliaTimeConst_tc))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &ciliaCouplingGain_C))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &referencePot))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "\
		  "parameter file '%s'.", funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_IHCRP_Shamma(endocochlearPot_Et, reversalPot_Ek,
	  reversalPotCorrection, totalCapacitance_C, restingConductance_G0,
	  kConductance_Gk, maxMConductance_Gmax, beta, gamma, ciliaTimeConst_tc,
	  ciliaCouplingGain_C, referencePot)) {
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
SetParsPointer_IHCRP_Shamma(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_IHCRP_Shamma";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	shammaPtr = (ShammaPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_IHCRP_Shamma(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_IHCRP_Shamma";

	if (!SetParsPointer_IHCRP_Shamma(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_IHCRP_Shamma(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = shammaPtr;
	theModule->CheckPars = CheckPars_IHCRP_Shamma;
	theModule->Free = Free_IHCRP_Shamma;
	theModule->GetUniParListPtr = GetUniParListPtr_IHCRP_Shamma;
	theModule->PrintPars = PrintPars_IHCRP_Shamma;
	theModule->ReadPars = ReadPars_IHCRP_Shamma;
	theModule->RunProcess = RunModel_IHCRP_Shamma;
	theModule->SetParsPointer = SetParsPointer_IHCRP_Shamma;
	return(TRUE);

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 * The spontaneous probability assumes preceeding silence.
 * Initial, spontaneous probability calculation: 1st sample is set to zero.
 * This value is used by the hair cell reservoir models.
 */

BOOLN
InitProcessVariables_IHCRP_Shamma(EarObjectPtr data)
{
	static const char *funcName = "InitProcessVariables_IHCRP_Shamma";
	int		i;
	double	restingPotential_V0;

	if (shammaPtr->updateProcessVariablesFlag || data->updateProcessFlag ||
	  (data->timeIndex == PROCESS_START_TIME)) {
		if (shammaPtr->updateProcessVariablesFlag || data->updateProcessFlag) {
			FreeProcessVariables_IHCRP_Shamma();
			if ((shammaPtr->lastInput = (double *)
			  calloc(data->outSignal->numChannels, sizeof(double))) == NULL) {
			 	NotifyError("%s: Out of memory for 'lastInput'.", funcName);
			 	return(FALSE);
			}
			if ((shammaPtr->lastOutput = (double *)
			  calloc(data->outSignal->numChannels, sizeof(double))) == NULL) {
			 	NotifyError("%s: Out of memory for 'lastOutput'.", funcName);
			 	return(FALSE);
			}
			if ((shammaPtr->lastCiliaDisplacement_u = (double *)
			  calloc(data->outSignal->numChannels, sizeof(double))) == NULL) {
			 	NotifyError("%s: Out of memory for 'lastCiliaDisplacement_u'.",
			 	  funcName);
			 	return(FALSE);
			}
			shammaPtr->updateProcessVariablesFlag = FALSE;
		}
		restingPotential_V0 = (shammaPtr->restingConductance_G0 *
		  shammaPtr->endocochlearPot_Et + shammaPtr->kConductance_Gk *
		  (shammaPtr->reversalPot_Ek + shammaPtr->endocochlearPot_Et *
		  shammaPtr->reversalPotCorrection)) /
		  (shammaPtr->restingConductance_G0 + shammaPtr->kConductance_Gk);
		for (i = 0; i < data->outSignal->numChannels; i++) {
			shammaPtr->lastInput[i] = 0.0;
			shammaPtr->lastOutput[i] = restingPotential_V0;
			shammaPtr->lastCiliaDisplacement_u[i] = 0.0;
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
FreeProcessVariables_IHCRP_Shamma(void)
{

	if (shammaPtr->lastInput != NULL) {
		free(shammaPtr->lastInput);
		shammaPtr->lastInput = NULL;
	}
	if (shammaPtr->lastOutput != NULL) {
		free(shammaPtr->lastOutput);
		shammaPtr->lastOutput = NULL;
	}
	if (shammaPtr->lastCiliaDisplacement_u != NULL) {
		free(shammaPtr->lastCiliaDisplacement_u);
		shammaPtr->lastCiliaDisplacement_u = NULL;
	}
	shammaPtr->updateProcessVariablesFlag = TRUE;

}

/********************************* RunModel ***********************************/

/*
 * This routine runs the input signal through the model and puts the result
 * into the output signal.  It checks that all initialisation has been
 * correctly carried out by calling the appropriate checking routines.
 * The reference potential, is not remembered for previous runs, and as added
 * after all processing has been done, because otherwise it causes problems
 * with the low-pass filtering effect.
 * The 'denom_val' calculation is employed to trap exponential overflow errors.
 */

BOOLN
RunModel_IHCRP_Shamma(EarObjectPtr data)
{
	static const char *funcName = "RunModel_IHCRP_Shamma";
	int		chan;
	ChanLen	i;
	double	leakageConductance_Ga, conductance_G, potential_V;
	double	ciliaDisplacement_u, lastInput, max_u, denom_val;
	register	double		dtOverC, gkEpk, dtOverTc, cGain, dt;
	register	ChanData	*inPtr, *outPtr;
	
	if (!CheckPars_IHCRP_Shamma()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}	
	if (!CheckRamp_SignalData(data->inSignal[0])) {
		NotifyError("%s: Input signal not correctly initialised.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "Shamma hair cell receptor potential");
	if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels,
	  data->inSignal[0]->length, data->inSignal[0]->dt)) {
		NotifyError("%s: Could not initialise output signal.", funcName);
		return(FALSE);
	}
	if (!InitProcessVariables_IHCRP_Shamma(data)) {
		NotifyError("%s: Could not initialise the process variables.",
		  funcName);
		return(FALSE);
	}
	dt = data->outSignal->dt;
	dtOverC = dt / shammaPtr->totalCapacitance_C;
	gkEpk = shammaPtr->kConductance_Gk * (shammaPtr->reversalPot_Ek +
	  shammaPtr->endocochlearPot_Et * shammaPtr->reversalPotCorrection);
	leakageConductance_Ga = shammaPtr->restingConductance_G0 -
	  shammaPtr->maxMConductance_Gmax / (1.0 + 1.0 / shammaPtr->beta);
	cGain = pow(10.0, shammaPtr->ciliaCouplingGain_C / 20.0);
	dtOverTc = dt / shammaPtr->ciliaTimeConst_tc;
        max_u = log(HUGE_VAL);
	for (chan = 0; chan < data->outSignal->numChannels; chan++) {
		ciliaDisplacement_u = shammaPtr->lastCiliaDisplacement_u[chan];
		inPtr = data->inSignal[0]->channel[chan];
		outPtr = data->outSignal->channel[chan];
		potential_V = shammaPtr->lastOutput[chan];
		lastInput = shammaPtr->lastInput[chan];
		for (i = 0; i < data->outSignal->length; i++, inPtr++, outPtr++) {       
			ciliaDisplacement_u += cGain * (*inPtr - lastInput) - 
			  ciliaDisplacement_u * dtOverTc;

			denom_val = ((-shammaPtr->gamma * ciliaDisplacement_u) >= max_u)?
			  1.0 + HUGE_VAL / shammaPtr->beta : 
			  1.0 + exp(-shammaPtr->gamma * ciliaDisplacement_u) /
			  shammaPtr->beta;

			conductance_G = shammaPtr->maxMConductance_Gmax / denom_val +
			  leakageConductance_Ga;
 
			*outPtr = (ChanData) (potential_V - dtOverC * (conductance_G *
			  (potential_V - shammaPtr->endocochlearPot_Et) +
			  shammaPtr->kConductance_Gk * potential_V - gkEpk));
			potential_V = *outPtr;
			lastInput = *inPtr;
		}
		shammaPtr->lastCiliaDisplacement_u[chan] = ciliaDisplacement_u;
		shammaPtr->lastInput[chan] = lastInput;
		shammaPtr->lastOutput[chan] = potential_V;
		outPtr = data->outSignal->channel[chan];
		for (i = 0; i < data->outSignal->length; i++)
			*outPtr++ += shammaPtr->referencePot;
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);
		
} /* RunModel_IHCRP_Shamma */
