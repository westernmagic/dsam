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
#include "UtString.h"
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
	static const WChar *funcName = wxT("Init_IHCRP_Shamma");

	if (parSpec == GLOBAL) {
		if (shammaPtr != NULL)
			Free_IHCRP_Shamma();
		if ((shammaPtr = (ShammaPtr) malloc(sizeof(Shamma))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (shammaPtr == NULL) { 
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
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
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
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
	static const WChar *funcName = wxT("SetUniParList_IHCRP_Shamma");
	UniParPtr	pars;

	if ((shammaPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  IHCRP_SHAMMA_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = shammaPtr->parList->pars;
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA_ENDOCOCHLEARPOT_ET], wxT("E_T"),
	  wxT("Endocochlear potential, Et (V)."),
	  UNIPAR_REAL,
	  &shammaPtr->endocochlearPot_Et, NULL,
	  (void * (*)) SetEndocochlearPot_IHCRP_Shamma);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA_REVERSALPOT_EK], wxT("E_K"),
	  wxT("Reversal potential, Ek (V)."),
	  UNIPAR_REAL,
	  &shammaPtr->reversalPot_Ek, NULL,
	  (void * (*)) SetReversalPot_IHCRP_Shamma);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA_REVERSALPOTCORRECTION], wxT(
	  "RP_CORRECTION"),
	  wxT("Reversal potential correction, Rp/(Rt+Rp)."),
	  UNIPAR_REAL,
	  &shammaPtr->reversalPotCorrection, NULL,
	  (void * (*)) SetReversalPotCorrection_IHCRP_Shamma);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA_TOTALCAPACITANCE_C], wxT("C_TOTAL"),
	  wxT("Total capacitance, C = Ca + Cb (F)."),
	  UNIPAR_REAL,
	  &shammaPtr->totalCapacitance_C, NULL,
	  (void * (*)) SetTotalCapacitance_IHCRP_Shamma);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA_RESTINGCONDUCTANCE_G0], wxT("G0"),
	  wxT("Resting conductance, G0 (S)."),
	  UNIPAR_REAL,
	  &shammaPtr->restingConductance_G0, NULL,
	  (void * (*)) SetRestingConductance_IHCRP_Shamma);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA_KCONDUCTANCE_GK], wxT("G_K"),
	  wxT("Potassium conductance, Gk (S = Siemens)."),
	  UNIPAR_REAL,
	  &shammaPtr->kConductance_Gk, NULL,
	  (void * (*)) SetKConductance_IHCRP_Shamma);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA_MAXMCONDUCTANCE_GMAX], wxT("G_MAXC"),
	  wxT("Maximum mechanical conductance, Gmax (S)."),
	  UNIPAR_REAL,
	  &shammaPtr->maxMConductance_Gmax, NULL,
	  (void * (*)) SetMaxMConductance_IHCRP_Shamma);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA_BETA], wxT("BETA"),
	  wxT("Beta constant, exp(-G1/RT)."),
	  UNIPAR_REAL,
	  &shammaPtr->beta, NULL,
	  (void * (*)) SetBeta_IHCRP_Shamma);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA_GAMMA], wxT("GAMMA"),
	  wxT("Gamma constant, Z1/RT (/m)."),
	  UNIPAR_REAL,
	  &shammaPtr->gamma, NULL,
	  (void * (*)) SetGamma_IHCRP_Shamma);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA_CILIATIMECONST_TC], wxT("T_C"),
	  wxT("Cilia/BM time constant (s)."),
	  UNIPAR_REAL,
	  &shammaPtr->ciliaTimeConst_tc, NULL,
	  (void * (*)) SetCiliaTimeConstTc_IHCRP_Shamma);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA_CILIACOUPLINGGAIN_C], wxT("GAIN_C"),
	  wxT("Cilia/BM coupling gain, C (dB)."),
	  UNIPAR_REAL,
	  &shammaPtr->ciliaCouplingGain_C, NULL,
	  (void * (*)) SetCiliaCouplingGain_IHCRP_Shamma);
	SetPar_UniParMgr(&pars[IHCRP_SHAMMA_REFERENCEPOT], wxT("REF_POT"),
	  wxT("Reference potential (V)."),
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
	static const WChar	*funcName = wxT("GetUniParListPtr_IHCRP_Shamma");

	if (shammaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (shammaPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been "
		  "initialised. NULL returned."), funcName);
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
	static const WChar	*funcName = wxT("SetEndocochlearPot_IHCRP_Shamma");

	if (shammaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
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
	static const WChar	*funcName = wxT("SetReversalPot_IHCRP_Shamma");

	if (shammaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
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
	static const WChar	*funcName = wxT(
	  "SetReversalPotCorrection_IHCRP_Shamma");

	if (shammaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
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
	static const WChar	*funcName = wxT("SetTotalCapacitance_IHCRP_Shamma");

	if (shammaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
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
	static const WChar	*funcName = wxT("SetRestingConductance_IHCRP_Shamma");

	if (shammaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
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
	static const WChar	*funcName = wxT("SetKConductance_IHCRP_Shamma");

	if (shammaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
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
	static const WChar	 *funcName = wxT("SetMaxMConductance_IHCRP_Shamma");

	if (shammaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
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
	static const WChar	 *funcName = wxT("SetBeta_IHCRP_Shamma");

	if (shammaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
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
	static const WChar	 *funcName = wxT("SetGamma_IHCRP_Shamma");

	if (shammaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
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
	static const WChar	 *funcName = wxT("SetCiliaCouplingGain_IHCRP_Shamma");

	if (shammaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
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
	static const WChar	 *funcName = wxT("SetCiliaTimeConstTc_IHCRP_Shamma");

	if (shammaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
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
	static const WChar	 *funcName = wxT("SetCiliaTimeConstTc_IHCRP_Shamma");
	if (shammaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
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
	static const WChar *funcName = wxT("CheckPars_IHCRP_Shamma");
	BOOLN	ok;
	
	ok = TRUE;
	if (shammaPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (!shammaPtr->endocochlearPotEtFlag) {
		NotifyError(wxT("%s: Endocochlear potential, Et, not correctly set."),
		  funcName);
		ok = FALSE;
	}
	if (!shammaPtr->reversalPotEkFlag) {
		NotifyError(wxT("%s: Reversal potential, Ek, not correctly set."),
		  funcName);
		ok = FALSE;
	}
	if (!shammaPtr->reversalPotCorrectionFlag) {
		NotifyError(wxT("%s: Reversal potential correction, Rp/(Rt+Rp), not "
		  "correctly set."), funcName);
		ok = FALSE;
	}
	if (!shammaPtr->totalCapacitanceCFlag) {
		NotifyError(wxT("%s: Total capacitance, C, not correctly set."),
		  funcName);
		ok = FALSE;
	}
	if (!shammaPtr->restingConductanceG0Flag) {
		NotifyError(wxT("%s: Resting conductanc, G0, not correctly set."),
		  funcName);
		ok = FALSE;
	}
	if (!shammaPtr->kConductanceGkFlag) {
		NotifyError(wxT("%s: Potassium conductanc, Gk, not correctly set."),
		  funcName);
		ok = FALSE;
	}
	if (!shammaPtr->maxMConductanceGmaxFlag) {
		NotifyError(wxT("%s: Maximum mechinally sensitive, Gmax, not correctly "
		  "set."), funcName);
		ok = FALSE;
	}
	if (!shammaPtr->betaFlag) {
		NotifyError(wxT("%s: Beta constant, not correctly set."), funcName);
		ok = FALSE;
	}
	if (!shammaPtr->gammaFlag) {
		NotifyError(wxT("%s: Gamma constant, not correctly set."), funcName);
		ok = FALSE;
	}
	if (!shammaPtr->ciliaTimeConstTcFlag) {
		NotifyError(wxT("%s: BM/Cilia displacement time constant, tc, not "
		  "correctly set."), funcName);
		ok = FALSE;
	}
	if (!shammaPtr->ciliaCouplingGainFlag) {
		NotifyError(wxT("%s: BM/Cilia coupling gain constant, n, not correctly "
		  "set."), funcName);
		ok = FALSE;
	}
	if (!shammaPtr->referencePotFlag) {
		NotifyError(wxT("CheckPars_IHCRP_Shamma: Reference potential, Eref, "
		  "not set."));
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
	static const WChar *funcName = wxT("PrintPars_IHCRP_Shamma(");

	if (!CheckPars_IHCRP_Shamma()) {
		NotifyError(wxT("%s: Parameters have not been correctly set."),
		  funcName);
		return(FALSE);
	}
	DPrint(wxT("Shamma et al. Receptor Potential Module Parameters:-\n"));
	DPrint(wxT("\tEndocochlear potential, Et = %g V,\n"),
	  shammaPtr->endocochlearPot_Et);
	DPrint(wxT("\tReversal potential, Ek = %g V,\n"),
	  shammaPtr->reversalPot_Ek);
	DPrint(wxT("\tReversal potential correction Rp/(Rt+Rp) = %g,\n"),
	  shammaPtr->reversalPotCorrection);
	DPrint(wxT("\tTotal capacitance, Ct = %g F,\n"),
	  shammaPtr->totalCapacitance_C);
	DPrint(wxT("\tResting conductance, G0 = %g S,\n"),
	  shammaPtr->restingConductance_G0);
	DPrint(wxT("\tPotassium conductance, Gk = %g S,\n"),
	  shammaPtr->kConductance_Gk);
	DPrint(wxT("\tMaximum mechanically sensitive Conductance, Gmax = %g S,\n"),
	  shammaPtr->maxMConductance_Gmax);
	DPrint(wxT("\tBeta = %g,\tgamma = %g /m.\n"),
	  shammaPtr->beta, shammaPtr->gamma);
	DPrint(wxT("\tBM/Cilia: time constant = %g ms,\t gain = %g dB\n"), MSEC(
	  shammaPtr->ciliaTimeConst_tc), shammaPtr->ciliaCouplingGain_C);
	DPrint(wxT("\tReference potential = %g V\n"), shammaPtr->referencePot);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.
 */
 
BOOLN
ReadPars_IHCRP_Shamma(WChar *fileName)
{
	static const WChar *funcName = wxT("ReadPars_IHCRP_Shamma");
	BOOLN	ok;
	WChar	*filePath;
	double	endocochlearPot_Et, reversalPot_Ek, reversalPotCorrection;
	double	totalCapacitance_C, restingConductance_G0, kConductance_Gk;
	double	maxMConductance_Gmax;
	double	beta, gamma, ciliaTimeConst_tc, ciliaCouplingGain_C;
	double	referencePot;
	FILE    *fp;
    
	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = DSAM_fopen(filePath, "r")) == NULL) {
		NotifyError(wxT("%s: Cannot open data file '%s'.\n"), funcName,
		  filePath);
		return(FALSE);
	}
	DPrint(wxT("%s: Reading from '%s':\n"), funcName, filePath);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &endocochlearPot_Et))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &reversalPot_Ek))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &reversalPotCorrection))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &totalCapacitance_C))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &restingConductance_G0))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &kConductance_Gk))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &maxMConductance_Gmax))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &beta))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &gamma))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &ciliaTimeConst_tc))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &ciliaCouplingGain_C))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &referencePot))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError(wxT("%s: Not enough lines, or invalid parameters, in "
		  "module parameter file '%s'."), funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_IHCRP_Shamma(endocochlearPot_Et, reversalPot_Ek,
	  reversalPotCorrection, totalCapacitance_C, restingConductance_G0,
	  kConductance_Gk, maxMConductance_Gmax, beta, gamma, ciliaTimeConst_tc,
	  ciliaCouplingGain_C, referencePot)) {
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
SetParsPointer_IHCRP_Shamma(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_IHCRP_Shamma");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
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
	static const WChar	*funcName = wxT("InitModule_IHCRP_Shamma");

	if (!SetParsPointer_IHCRP_Shamma(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_IHCRP_Shamma(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = shammaPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
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
	static const WChar *funcName = wxT("InitProcessVariables_IHCRP_Shamma");
	int		i;
	double	restingPotential_V0;
	ShammaPtr	p = shammaPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag || (data->
	  timeIndex == PROCESS_START_TIME)) {
		if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
			FreeProcessVariables_IHCRP_Shamma();
			if ((p->lastInput = (double *) calloc(_OutSig_EarObject(data)->numChannels,
			   sizeof(double))) == NULL) {
			 	NotifyError(wxT("%s: Out of memory for 'lastInput'."),
				  funcName);
			 	return(FALSE);
			}
			if ((p->lastOutput = (double *) calloc(_OutSig_EarObject(data)->numChannels,
			   sizeof(double))) == NULL) {
			 	NotifyError(wxT("%s: Out of memory for 'lastOutput'."),
				  funcName);
			 	return(FALSE);
			}
			if ((p->lastCiliaDisplacement_u = (double *) calloc(data->
			  outSignal->numChannels, sizeof(double))) == NULL) {
			 	NotifyError(wxT("%s: Out of memory for "
				  "'lastCiliaDisplacement_u'."), funcName);
			 	return(FALSE);
			}
			p->updateProcessVariablesFlag = FALSE;
		}
		restingPotential_V0 = (p->restingConductance_G0 * p->
		  endocochlearPot_Et + p->kConductance_Gk * (p->reversalPot_Ek +
		  p->endocochlearPot_Et * p->reversalPotCorrection)) /
		  (p->restingConductance_G0 + p->kConductance_Gk);
		for (i = 0; i < _OutSig_EarObject(data)->numChannels; i++) {
			p->lastInput[i] = 0.0;
			p->lastOutput[i] = restingPotential_V0;
			p->lastCiliaDisplacement_u[i] = 0.0;
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
	static const WChar *funcName = wxT("RunModel_IHCRP_Shamma");
	int		chan;
	ChanLen	i;
	double	conductance_G, potential_V;
	double	ciliaDisplacement_u, lastInput, denom_val;
	register	double		dt;
	register	ChanData	*inPtr, *outPtr;
	SignalDataPtr	outSignal;
	ShammaPtr	p = shammaPtr;
	
	if (!data->threadRunFlag) {
		if (!CheckPars_IHCRP_Shamma()) {
			NotifyError(wxT("%s: Parameters have not been correctly set."),
			  funcName);
			return(FALSE);
		}
		if (data == NULL) {
			NotifyError(wxT("%s: EarObject not initialised."), funcName);
			return(FALSE);
		}	
		if (!CheckInSignal_EarObject(data, funcName))
			return(FALSE);
		if (!CheckRamp_SignalData(_InSig_EarObject(data, 0))) {
			NotifyError(wxT("%s: Input signal not correctly initialised."),
			  funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Shamma hair cell "
		  "receptor potential"));
		if (!InitOutSignal_EarObject(data, _InSig_EarObject(data, 0)->numChannels,
		  _InSig_EarObject(data, 0)->length, _InSig_EarObject(data, 0)->dt)) {
			NotifyError(wxT("%s: Could not initialise output signal."),
			  funcName);
			return(FALSE);
		}
		if (!InitProcessVariables_IHCRP_Shamma(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		dt = _OutSig_EarObject(data)->dt;
		p->dtOverC = dt / p->totalCapacitance_C;
		p->gkEpk = p->kConductance_Gk * (p->reversalPot_Ek + p->
		  endocochlearPot_Et * p->reversalPotCorrection);
		p->leakageConductance_Ga = p->restingConductance_G0 - p->
		  maxMConductance_Gmax / (1.0 + 1.0 / p->beta);
		p->cGain = pow(10.0, p->ciliaCouplingGain_C / 20.0);
		p->dtOverTc = dt / p->ciliaTimeConst_tc;
        p->max_u = log(HUGE_VAL);
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	outSignal = _OutSig_EarObject(data);
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		ciliaDisplacement_u = p->lastCiliaDisplacement_u[chan];
		inPtr = _InSig_EarObject(data, 0)->channel[chan];
		outPtr = outSignal->channel[chan];
		potential_V = p->lastOutput[chan];
		lastInput = p->lastInput[chan];
		for (i = 0; i < outSignal->length; i++, inPtr++, outPtr++) {
			ciliaDisplacement_u += p->cGain * (*inPtr - lastInput) - 
			  ciliaDisplacement_u *  p->dtOverTc;

			denom_val = ((-p->gamma * ciliaDisplacement_u) >= p->max_u)?
			  1.0 + HUGE_VAL / p->beta: 1.0 + exp(-p->gamma *
			  ciliaDisplacement_u) / p->beta;

			conductance_G = p->maxMConductance_Gmax / denom_val +
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
		outPtr = outSignal->channel[chan];
		for (i = 0; i < outSignal->length; i++)
			*outPtr++ += p->referencePot;
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);
		
} /* RunModel_IHCRP_Shamma */
