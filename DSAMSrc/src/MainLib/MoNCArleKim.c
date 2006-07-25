/**********************
 *
 * File:		MoNCArleKim.c
 * Purpose:		This module implements Arle & Kim's version of the MacGregor
 *				Model.
 * Comments:	The changes implemented in "Neural Modeling of instrinsic and
 *				spike-discharge properties of cochlear nucleus neurons",
 *				J. E. Erle and D. O. Kim, Biological Cybernetics, Springer
 *				Verlag 1991. - have been added to this module version of the
 *				McGregor model.
 * Author:		Trevor Shackleton and M. J. Hewitt. (Revised L. P. O'Mard).
 * Created:		12 Jul 1993
 * Updated:		
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
#include "MoNCArleKim.h"

/******************************************************************************/
/*************************** Global Variables *********************************/
/******************************************************************************/

ArleKimPtr	arleKimPtr = NULL;

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
Init_Neuron_ArleKim(ParameterSpecifier parSpec)
{
	static const WChar *funcName = wxT("Init_Neuron_ArleKim");

	if (parSpec == GLOBAL) {
		if (arleKimPtr != NULL)
			Free_Neuron_ArleKim();
		if ((arleKimPtr = (ArleKimPtr) malloc(sizeof(ArleKim))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (arleKimPtr == NULL) { 
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	arleKimPtr->parSpec = parSpec;
	arleKimPtr->updateProcessVariablesFlag = TRUE;
	arleKimPtr->membraneTConstFlag = TRUE;
	arleKimPtr->thresholdTConstFlag = TRUE;
	arleKimPtr->accomConstFlag = TRUE;
	arleKimPtr->delayedRectKCondFlag = TRUE;
	arleKimPtr->restingThresholdFlag = TRUE;
	arleKimPtr->actionPotentialFlag = TRUE;
	arleKimPtr->kDecayTConstFlag = TRUE;
	arleKimPtr->nonLinearVConstFlag = TRUE;
	arleKimPtr->kReversalPotenFlag = TRUE;
	arleKimPtr->bReversalPotenFlag = TRUE;
	arleKimPtr->kRestingCondFlag = TRUE;
	arleKimPtr->bRestingCondFlag = TRUE;
	arleKimPtr->membraneTConst_Tm = 5e-3;
	arleKimPtr->kDecayTConst_TGk = 2e-3;
	arleKimPtr->thresholdTConst_TTh = 2e-3;
	arleKimPtr->accomConst_c = 0.0;
	arleKimPtr->delayedRectKCond_b = 0.00025;
	arleKimPtr->restingThreshold_Th0 = 12.0;
	arleKimPtr->actionPotential = 50.0;
	arleKimPtr->nonLinearVConst_Vnl = 0.5;
	arleKimPtr->kReversalPoten_Ek = -65.0;
	arleKimPtr->bReversalPoten_Eb = -65.0;
	arleKimPtr->kRestingCond_gk = 0.02;
	arleKimPtr->bRestingCond_gb = 0.0;

	if (!SetUniParList_Neuron_ArleKim()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Neuron_ArleKim();
		return(FALSE);
	}
	arleKimPtr->state = NULL;
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
Free_Neuron_ArleKim(void)
{
	if (arleKimPtr == NULL)
		return(TRUE);
	FreeProcessVariables_Neuron_ArleKim();
	if (arleKimPtr->parList)
		FreeList_UniParMgr(&arleKimPtr->parList);
	if (arleKimPtr->parSpec == GLOBAL) {
		free(arleKimPtr);
		arleKimPtr = NULL;
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
SetUniParList_Neuron_ArleKim(void)
{
	static const WChar *funcName = wxT("SetUniParList_Neuron_ArleKim");
	UniParPtr	pars;

	if ((arleKimPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  NEURON_ARLEKIM_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = arleKimPtr->parList->pars;
	SetPar_UniParMgr(&pars[NEURON_ARLEKIM_MEMBRANETCONST_TM], wxT("T_M"),
	  wxT("Cell membrane time-constant, tm (s)."),
	  UNIPAR_REAL,
	  &arleKimPtr->membraneTConst_Tm, NULL,
	  (void * (*)) SetMembraneTConst_Neuron_ArleKim);
	SetPar_UniParMgr(&pars[NEURON_ARLEKIM_KDECAYTCONST_TGK], wxT("T_GK"),
	  wxT("Potassium decay time-constant, tGk (s)."),
	  UNIPAR_REAL,
	  &arleKimPtr->kDecayTConst_TGk, NULL,
	  (void * (*)) SetKDecayTConst_Neuron_ArleKim);
	SetPar_UniParMgr(&pars[NEURON_ARLEKIM_THRESHOLDTCONST_TTH], wxT("T_TH"),
	  wxT("Threshold time-constant, tTh (s)."),
	  UNIPAR_REAL,
	  &arleKimPtr->thresholdTConst_TTh, NULL,
	  (void * (*)) SetThresholdTConst_Neuron_ArleKim);
	SetPar_UniParMgr(&pars[NEURON_ARLEKIM_ACCOMCONST_C], wxT("C"),
	  wxT("Accommodation constant, c (dimensionless)."),
	  UNIPAR_REAL,
	  &arleKimPtr->accomConst_c, NULL,
	  (void * (*)) SetAccomConst_Neuron_ArleKim);
	SetPar_UniParMgr(&pars[NEURON_ARLEKIM_DELAYEDRECTKCOND_B], wxT("B"),
	  wxT("Delayed rectifier postassium conductance, b (nano-Siemens)."),
	  UNIPAR_REAL,
	  &arleKimPtr->delayedRectKCond_b, NULL,
	  (void * (*)) SetDelayedRectKCond_Neuron_ArleKim);
	SetPar_UniParMgr(&pars[NEURON_ARLEKIM_RESTINGTHRESHOLD_TH0], wxT("TH0"),
	  wxT("Cell resting Threshold, Th0 (mV)."),
	  UNIPAR_REAL,
	  &arleKimPtr->restingThreshold_Th0, NULL,
	  (void * (*)) SetRestingThreshold_Neuron_ArleKim);
	SetPar_UniParMgr(&pars[NEURON_ARLEKIM_ACTIONPOTENTIAL], wxT("ACTION_POT"),
	  wxT("Action potential (mV)."),
	  UNIPAR_REAL,
	  &arleKimPtr->actionPotential, NULL,
	  (void * (*)) SetActionPotential_Neuron_ArleKim);
	SetPar_UniParMgr(&pars[NEURON_ARLEKIM_NONLINEARVCONST_VNL], wxT("V_NL"),
	  wxT("Non-linear voltage constant, Vnl (mV)."),
	  UNIPAR_REAL,
	  &arleKimPtr->nonLinearVConst_Vnl, NULL,
	  (void * (*)) SetNonLinearVConst_Neuron_ArleKim);
	SetPar_UniParMgr(&pars[NEURON_ARLEKIM_KREVERSALPOTEN_EK], wxT("E_K"),
	  wxT("Reversal potential of the potassium conductance, Ek (mV)."),
	  UNIPAR_REAL,
	  &arleKimPtr->kReversalPoten_Ek, NULL,
	  (void * (*)) SetKReversalPoten_Neuron_ArleKim);
	SetPar_UniParMgr(&pars[NEURON_ARLEKIM_BREVERSALPOTEN_EB], wxT("E_B"),
	  wxT("Reversal potential of all other conductances, Eb (mV)."),
	  UNIPAR_REAL,
	  &arleKimPtr->bReversalPoten_Eb, NULL,
	  (void * (*)) SetBReversalPoten_Neuron_ArleKim);
	SetPar_UniParMgr(&pars[NEURON_ARLEKIM_KRESTINGCOND_GK], wxT("G_K"),
	  wxT("Resting potassium conductance, gk (nS)."),
	  UNIPAR_REAL,
	  &arleKimPtr->kRestingCond_gk, NULL,
	  (void * (*)) SetKRestingCond_Neuron_ArleKim);
	SetPar_UniParMgr(&pars[NEURON_ARLEKIM_BRESTINGCOND_GB], wxT("G_B"),
	  wxT("Resting component of all other conductances, gb (nS)."),
	  UNIPAR_REAL,
	  &arleKimPtr->bRestingCond_gb, NULL,
	  (void * (*)) SetBRestingCond_Neuron_ArleKim);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Neuron_ArleKim(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_Neuron_ArleKim");

	if (arleKimPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (arleKimPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been "
		  "initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(arleKimPtr->parList);

}

/********************************* SetMembraneTConst **************************/

/*
 * This function sets the module's membrane time constant, Tm, parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetMembraneTConst_Neuron_ArleKim(double theMembraneTConst)
{
	static const WChar *funcName = wxT("SetMembraneTConst_Neuron_ArleKim");

	if (arleKimPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theMembraneTConst == 0.0) {
		NotifyError(wxT("%s: Invalid value = %g mV\n"), funcName,
		  theMembraneTConst);
		return(FALSE);
	} 
	arleKimPtr->membraneTConstFlag = TRUE;
	arleKimPtr->membraneTConst_Tm = theMembraneTConst;
	arleKimPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetKDecayTConst ****************************/

/*
 * This function sets the module's potassium conductance decay time constant,
 * TGk, parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetKDecayTConst_Neuron_ArleKim(double theKDecayTConst)
{
	static const WChar *funcName = wxT("SetKDecayTConst_Neuron_ArleKim");

	if (arleKimPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theKDecayTConst == 0.0) {
		NotifyError(wxT("%s: Invalid value = %g mV\n"), funcName,
		  theKDecayTConst);
		return(FALSE);
	} 
	arleKimPtr->kDecayTConstFlag = TRUE;
	arleKimPtr->kDecayTConst_TGk = theKDecayTConst;
	arleKimPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetThresholdTConst *************************/

/*
 * This function sets the module's threshold time constant, TTh, parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetThresholdTConst_Neuron_ArleKim(double theThresholdTConst)
{
	static const WChar *funcName = wxT("SetThresholdTConst_Neuron_ArleKim");

	if (arleKimPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theThresholdTConst == 0.0) {
		NotifyError(wxT("%s: Invalid value = %g mV\n"), funcName,
		  theThresholdTConst);
		return(FALSE);
	} 
	arleKimPtr->thresholdTConstFlag = TRUE;
	arleKimPtr->thresholdTConst_TTh = theThresholdTConst;
	arleKimPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetAccomConst ******************************/

/*
 * This function sets the module's accomodation constant, c, parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetAccomConst_Neuron_ArleKim(double theAccomConst)
{
	static const WChar	*funcName = wxT("SetAccomConst_Neuron_ArleKim");

	if (arleKimPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	arleKimPtr->accomConstFlag = TRUE;
	arleKimPtr->accomConst_c = theAccomConst;
	arleKimPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetDelayedRectKCond ************************/

/*
 * This function sets the module's delayed rectifier potassium conductance
 * strength, b, parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetDelayedRectKCond_Neuron_ArleKim(double theDelayedRectKCond)
{
	static const WChar	*funcName = wxT("SetDelayedRectKCond_Neuron_ArleKim");

	if (arleKimPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	arleKimPtr->delayedRectKCondFlag = TRUE;
	arleKimPtr->delayedRectKCond_b = theDelayedRectKCond;
	arleKimPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetRestingThreshold ************************/

/*
 * This function sets the module's resting threhold, Th0, parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetRestingThreshold_Neuron_ArleKim(double theRestingThreshold)
{
	static const WChar	*funcName = wxT("SetRestingThreshold_Neuron_ArleKim");

	if (arleKimPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	arleKimPtr->restingThresholdFlag = TRUE;
	arleKimPtr->restingThreshold_Th0 = theRestingThreshold;
	arleKimPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetActionPotential *************************/

/*
 * This function sets the module's action potential parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetActionPotential_Neuron_ArleKim(double theActionPotential)
{
	static const WChar	*funcName = wxT("SetActionPotential_Neuron_ArleKim");

	if (arleKimPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	arleKimPtr->actionPotentialFlag = TRUE;
	arleKimPtr->actionPotential = theActionPotential;
	return(TRUE);

}

/********************************* SetNonLinearVConst *************************/

/*
 * This function sets the module's non-linear voltage constant, Vnl, parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetNonLinearVConst_Neuron_ArleKim(double theNonLinearVConst)
{
	static const WChar *funcName = wxT("SetNonLinearVConst_Neuron_ArleKim");

	if (arleKimPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theNonLinearVConst == 0.0) {
		NotifyError(wxT("%s: Invalid value = %g mV\n"), funcName,
		  theNonLinearVConst);
		NotifyWarning(wxT("SetNonLinearVConst_Neuron_ArleKim: To set the term "
		  "involving this parameter to zero set gb to zero."));
		return(FALSE);
	} 
	arleKimPtr->nonLinearVConstFlag = TRUE;
	arleKimPtr->nonLinearVConst_Vnl = theNonLinearVConst;
	arleKimPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetKReversalPoten **************************/

/*
 * This function sets the module's reversal potential of the K conductance
 * parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetKReversalPoten_Neuron_ArleKim(double theKReversalPoten)
{
	static const WChar	*funcName = wxT("SetKReversalPoten_Neuron_ArleKim");

	if (arleKimPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	arleKimPtr->kReversalPotenFlag = TRUE;
	arleKimPtr->kReversalPoten_Ek = theKReversalPoten;
	arleKimPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetBReversalPoten **************************/

/*
 * This function sets the module's reversal potential of all other conductances
 * parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetBReversalPoten_Neuron_ArleKim(double theBReversalPoten)
{
	static const WChar	*funcName = wxT("SetBReversalPoten_Neuron_ArleKim");

	if (arleKimPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	arleKimPtr->bReversalPotenFlag = TRUE;
	arleKimPtr->bReversalPoten_Eb = theBReversalPoten;
	arleKimPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetKRestingCond ****************************/

/*
 * This function sets the module's resting potassium conductance parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetKRestingCond_Neuron_ArleKim(double theKRestingCond)
{
	static const WChar	*funcName = wxT("SetKRestingCond_Neuron_ArleKim");

	if (arleKimPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	arleKimPtr->kRestingCondFlag = TRUE;
	arleKimPtr->kRestingCond_gk = theKRestingCond;
	arleKimPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetBRestingCond ****************************/

/*
 * This function sets the module's resting component of all other conductances
 * parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetBRestingCond_Neuron_ArleKim(double theBRestingCond)
{
	static const WChar	*funcName = wxT("SetBRestingCond_Neuron_ArleKim");

	if (arleKimPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	arleKimPtr->bRestingCondFlag = TRUE;
	arleKimPtr->bRestingCond_gb = theBRestingCond;
	arleKimPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetPars ************************************/

/*
 * This function sets all the module's parameters.
 */

BOOLN
SetPars_Neuron_ArleKim(double tm, double tGk, double tTh, double c, double b,
  double tH0, double aP, double vnl, double ek, double eb, double gk,
  double gb)
{
	BOOLN	ok;
	
	ok = TRUE;
	if (!SetMembraneTConst_Neuron_ArleKim(tm))
		ok = FALSE;
	if (!SetKDecayTConst_Neuron_ArleKim(tGk))
		ok = FALSE;
	if (!SetThresholdTConst_Neuron_ArleKim(tTh))
		ok = FALSE;
	if (!SetAccomConst_Neuron_ArleKim(c))
	    ok = FALSE;
	if (!SetDelayedRectKCond_Neuron_ArleKim(b))
		ok = FALSE;
	if (!SetRestingThreshold_Neuron_ArleKim(tH0))
		ok = FALSE;
	if (!SetActionPotential_Neuron_ArleKim(aP))
		ok = FALSE;
	if (!SetNonLinearVConst_Neuron_ArleKim(vnl))
		ok = FALSE;
	if (!SetKReversalPoten_Neuron_ArleKim(ek))
		ok = FALSE;
	if (!SetBReversalPoten_Neuron_ArleKim(eb))
		ok = FALSE;
	if (!SetKRestingCond_Neuron_ArleKim(gk))
		ok = FALSE;
	if (!SetBRestingCond_Neuron_ArleKim(gb))
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
CheckPars_Neuron_ArleKim(void)
{
	static const WChar *funcName = wxT("CheckPars_Neuron_ArleKim");
	BOOLN	ok;
	
	ok = TRUE;
	if (arleKimPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (!arleKimPtr->membraneTConstFlag) {
		NotifyError(wxT("%s: Membrane time constant, Tm, not correctly set."),
		  funcName);
		ok = FALSE;
	}
	if (!arleKimPtr->kDecayTConstFlag) {
		NotifyError(wxT("%s: Potassium decay time  constant, TGk, not correctly "
		  "set."), funcName);
		ok = FALSE;
	}
	if (!arleKimPtr->thresholdTConstFlag) {
		NotifyError(wxT("%s: Threshold time constant, TTh, not correctly set."),
		  funcName);
		ok = FALSE;
	}
	if (!arleKimPtr->accomConstFlag) {
		NotifyError(wxT("%s: Accomodation constant, c, not correctly set."),
		  funcName);
		ok = FALSE;
	}
	if (!arleKimPtr->delayedRectKCondFlag) {
		NotifyError(wxT("%s: Delayed rectifier potassium conductance, b, not "
		  "correctly set."), funcName);
		ok = FALSE;
	}
	if (!arleKimPtr->restingThresholdFlag) {
		NotifyError(wxT("%s: Resting threshold constant, Th0, not correctly "
		  "set."), funcName);
		ok = FALSE;
	}
	if (!arleKimPtr->actionPotentialFlag) {
		NotifyError(wxT("%s: Action potential constant not correctly set."),
		  funcName);
		ok = FALSE;
	}
	if (!arleKimPtr->nonLinearVConstFlag) {
		NotifyError(wxT("%s: Non-linear voltage constant, Vnl, not correctly set."),
		  funcName);
		ok = FALSE;
	}
	if (!arleKimPtr->kReversalPotenFlag) {
		NotifyError(wxT("%s: Reversal potential of the potassium conductance, "
		  "Ek, not correctly set."), funcName);
		ok = FALSE;
	}
	if (!arleKimPtr->bReversalPotenFlag) {
		NotifyError(wxT("%s: Reversal potential of all other conductances, Eb, "
		  "not correctly set."), funcName);
		ok = FALSE;
	}
	if (!arleKimPtr->kRestingCondFlag) {
		NotifyError(wxT("%s: Resting potassium conductance, gk, not correctly "
		  "set."), funcName);
		ok = FALSE;
	}
	if (!arleKimPtr->bRestingCondFlag) {
		NotifyError(wxT("%s: Resting component for all other conductances, gb, "
		  "not correctly set."), funcName);
		ok = FALSE;
	}
	/*
	This needs some work.
	dEExpression = (arleKimPtr->kRestingCond_gk +
	  arleKimPtr->bRestingCond_gb) / arleKimPtr->membraneTConst_Tm;
	if (dEExpression < ARLEKIM_DE_LIMIT) {
		NotifyError(wxT("%s:  The total conductance, gk + gb, is too high or the "
		  "membrane time-constant is too low.\n  This may cause the model "
		  "output to produce a floating-point error.\n"), funcName);
	} */
	return(ok);
		
}

/****************************** GetPotentialResponse **************************/

/*
 * This routine subtracts the action potential from the response if the
 * threshold is exceeded.
 *
 */

void *
GetPotentialResponse_Neuron_ArleKim(void *potentialPtr)
{
	static const WChar *funcName = wxT("GetPotentialResponse_Neuron_ArleKim");
	double		restingPotential_Er, totalConductance_G, *potential;
	ArleKimPtr	p;

	if (!CheckPars_Neuron_ArleKim()) {
		NotifyError(wxT("%s: Parameters have not been correctly set, zero "
		  "returned."), funcName);
		return(NULL);
	}
	potential = (double *) potentialPtr;
	p = arleKimPtr;
	totalConductance_G = p->kRestingCond_gk + p->bRestingCond_gb;
	restingPotential_Er = (p->kRestingCond_gk * p->kReversalPoten_Ek +
	  p->bRestingCond_gb * p->bReversalPoten_Eb) / totalConductance_G;
	if (*potential - restingPotential_Er > p->restingThreshold_Th0 +
	  p->accomConst_c)
		*potential -= p->actionPotential;

	return(potentialPtr);

}

/****************************** PrintPars *************************************/

/*
 * This program prints the parameters of the module to the standard output.
 */
 
BOOLN
PrintPars_Neuron_ArleKim(void)
{
	static const WChar *funcName = wxT("PrintPars_Neuron_ArleKim");

	if (!CheckPars_Neuron_ArleKim()) {
		NotifyError(wxT("%s: Parameters have not been correctly set."),
		  funcName);
		return(FALSE);
	}
	DPrint(wxT("ArleKim Neural Cell Module Parameters:-\n"));
	DPrint(wxT("\tMembrane time constant, Tm = %g ms,\n"),
	  MSEC(arleKimPtr->membraneTConst_Tm));
	DPrint(wxT("\tPotassium decay time constant, TGk = %g ms,\n"),
	  MSEC(arleKimPtr->kDecayTConst_TGk));
	DPrint(wxT("\tThreshold rise time constant, TTh = %g ms,\n"),
	  MSEC(arleKimPtr->thresholdTConst_TTh));
	DPrint(wxT("\tAccommodation constant, c = %g,\n"),
	  arleKimPtr->accomConst_c);
	DPrint(wxT("\tDelayed rectifier potassium conductance, b = "
	  "%g nano Siemens.\n"), arleKimPtr->delayedRectKCond_b);
	DPrint(wxT("\tResting threshold, Th0 = %g mV,\tAction ")\
	  wxT("potential = %g mV.\n"), arleKimPtr->restingThreshold_Th0,
	  arleKimPtr->actionPotential);
	DPrint(wxT("\tNon-linear voltage constant, Vnl = %g mV,\n"),
	  arleKimPtr->nonLinearVConst_Vnl);
	DPrint(wxT("\tReversal potential of the potassium "
	  "conductance, Ek = %g mV,\n"), arleKimPtr->kReversalPoten_Ek);
	DPrint(wxT("\tReversal potential of all other ")\
	  wxT("conductances, Eb = %g mV,\n"), arleKimPtr->bReversalPoten_Eb);
	DPrint(wxT("\tResting potassium conductance, gk = %g nS,\n"),
	  arleKimPtr->kRestingCond_gk);
	DPrint(wxT("\tAll other resting conductances, gb = %g nS,\n"),
	  arleKimPtr->bRestingCond_gb);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.
 */
 
BOOLN
ReadPars_Neuron_ArleKim(WChar *fileName)
{
	static const WChar *funcName = wxT("ReadPars_Neuron_ArleKim");
	BOOLN	ok;
	WChar	*filePath;
	double	membraneTConst_Tm, kDecayTConst_TGk, thresholdTConst_TTh;
	double	accomConst_c, delayedRectKCond_b, restingThreshold_Th0;
	double	actionPotential, nonLinearVConst_Vnl, kReversalPoten_Ek;
	double	bReversalPoten_Eb, kRestingCond_gk, bRestingCond_gb;
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
	if (!GetPars_ParFile(fp, wxT("%lf"), &membraneTConst_Tm))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &kDecayTConst_TGk))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &thresholdTConst_TTh))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &accomConst_c))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &delayedRectKCond_b))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &restingThreshold_Th0))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &actionPotential))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &nonLinearVConst_Vnl))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &kReversalPoten_Ek))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &bReversalPoten_Eb))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &kRestingCond_gk))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &bRestingCond_gb))
		ok = FALSE;
    fclose(fp);
    Free_ParFile();
	if (!ok) {
		NotifyError(wxT("%s: Not enough lines, or invalid parameters, in "
		  "module parameter file '%s'."), funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_Neuron_ArleKim(membraneTConst_Tm, kDecayTConst_TGk,
	  thresholdTConst_TTh, accomConst_c, delayedRectKCond_b, 
	  restingThreshold_Th0, actionPotential, nonLinearVConst_Vnl,
	  kReversalPoten_Ek, bReversalPoten_Eb, kRestingCond_gk, bRestingCond_gb)) {
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
SetParsPointer_Neuron_ArleKim(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Neuron_ArleKim");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	arleKimPtr = (ArleKimPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Neuron_ArleKim(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Neuron_ArleKim");

	if (!SetParsPointer_Neuron_ArleKim(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Neuron_ArleKim(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."), funcName);
		return(FALSE);
	}
	theModule->parsPtr = arleKimPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->CheckPars = CheckPars_Neuron_ArleKim;
	theModule->Free = Free_Neuron_ArleKim;
	theModule->GetData = GetPotentialResponse_Neuron_ArleKim;
	theModule->GetUniParListPtr = GetUniParListPtr_Neuron_ArleKim;
	theModule->PrintPars = PrintPars_Neuron_ArleKim;
	theModule->ReadPars = ReadPars_Neuron_ArleKim;
	theModule->RunProcess = RunModel_Neuron_ArleKim;
	theModule->SetParsPointer = SetParsPointer_Neuron_ArleKim;
	return(TRUE);

}

/********************************* CheckData **********************************/

/*
 * This routine checks that the input signal is correctly initialised, and
 * determines whether the parameter values are valid for the signal sampling
 * interval.
 */

BOOLN
CheckData_Neuron_ArleKim(EarObjectPtr data)
{
	static const WChar *funcName = wxT("CheckData_Neuron_ArleKim");
	
	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}	
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	if (!CheckRamp_SignalData(data->inSignal[0]))
		return(FALSE);
	return(TRUE);
	
}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 */

BOOLN
InitProcessVariables_Neuron_ArleKim(EarObjectPtr data)
{
	static const WChar *funcName = wxT("InitProcessVariables_Neuron_ArleKim");
	int		i;
	ArleKimPtr	p = arleKimPtr;
	
	if (p->updateProcessVariablesFlag || data->updateProcessFlag ||(data->
	  timeIndex == PROCESS_START_TIME)) {
		if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
			FreeProcessVariables_Neuron_ArleKim();
			if ((p->state = (ArleKimStatePtr) calloc(_OutSig_EarObject(data)->
			  numChannels, sizeof(ArleKimState))) == NULL) {
			 	NotifyError(wxT("%s: Out of memory."), funcName);
			 	return(FALSE);
			}
			p->updateProcessVariablesFlag = FALSE;
		}
		for (i = 0; i < _OutSig_EarObject(data)->numChannels; i++) {
			p->state[i].potential_V = 0.0;
			p->state[i].kConductance_Gk = 0.0;
			p->state[i].bConductance_Gb = 0.0;
			p->state[i].threshold_Th = p->restingThreshold_Th0;
			p->state[i].lastSpikeState = 0;
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
FreeProcessVariables_Neuron_ArleKim(void)
{
	if (arleKimPtr->state == NULL)
		return;
	free(arleKimPtr->state);
	arleKimPtr->state = NULL;
	arleKimPtr->updateProcessVariablesFlag = TRUE;

}

/********************************* RunModel ***********************************/

/*
 * This routine runs the input signal through the model and puts the result
 * into the output signal.  It checks that all initialisation has been
 * correctly carried out by calling the appropriate checking routines.
 * potential_V = Cell-membrane pot. above resting level (mV);
 * kEquilibriumPot_Vk = Equlibrium potential of K conductance. (mV);
 * kConductance_Gk = Potassium conductance (ns);
 * threshold_Th = Time-varying cell threshold (mV);
 * spikeVariable_s = spike variable for cell.
 */

BOOLN
RunModel_Neuron_ArleKim(EarObjectPtr data)
{
	static const WChar *funcName = wxT("RunModel_Neuron_ArleKim");
	int			i, spikeState_s;
	double		dt;
	register double	dGk, dTh, dV;
	ChanLen		j;
	ChanData	*inPtr, *outPtr;
	ArleKimPtr	p = arleKimPtr;
	
	if (!data->threadRunFlag) {
		if (!CheckPars_Neuron_ArleKim())		
			return(FALSE);
		if (!CheckData_Neuron_ArleKim(data))
			return(FALSE);
		if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels,
		  data->inSignal[0]->length, data->inSignal[0]->dt)) {
			NotifyError(wxT("%s: Could not initialise output signal."),
			  funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("ArleKim neural cell"));
		if (!InitProcessVariables_Neuron_ArleKim(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		dt = _OutSig_EarObject(data)->dt;

		p->totalConductance_G = p->kRestingCond_gk + p->bRestingCond_gb;
		p->restingPotential_Er = (p->kRestingCond_gk * p->kReversalPoten_Ek +
		  p->bRestingCond_gb * p->bReversalPoten_Eb) / p->totalConductance_G;
		p->kEquilibriumPot_Vk = p->kReversalPoten_Ek - p->restingPotential_Er;
		p->bEquilibriumPot_Vb = p->bReversalPoten_Eb - p->restingPotential_Er;

		p->tmOverDt  = p->membraneTConst_Tm / dt;
		p->tGkOverDt = p->kDecayTConst_TGk / dt;
		p->tThOverDt = p->thresholdTConst_TTh / dt;
		p->bOverDt = p->delayedRectKCond_b / dt;
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	for (i = _OutSig_EarObject(data)->offset; i < _OutSig_EarObject(data)->numChannels; i++) {
		spikeState_s = p->state[i].lastSpikeState;
		inPtr = data->inSignal[0]->channel[i];
		outPtr = _OutSig_EarObject(data)->channel[i];
		for (j = 0; j < _OutSig_EarObject(data)->length; j++, outPtr++) {
			p->state[i].bConductance_Gb = p->bRestingCond_gb *
			 (exp(p->state[i].potential_V / p->nonLinearVConst_Vnl) - 1.0);
			dV =  (-p->state[i].potential_V + (*(inPtr++) +
			  p->state[i].kConductance_Gk * (p->kEquilibriumPot_Vk -
			  p->state[i].potential_V) + p->state[i].bConductance_Gb *
			   (p->bEquilibriumPot_Vb - p->state[i].potential_V)) /
			    p->totalConductance_G) / p->tmOverDt;
			dTh = (-(p->state[i].threshold_Th - p->restingThreshold_Th0) +
			  p->accomConst_c * p->state[i].potential_V ) / p->tThOverDt;	
			*outPtr = (ChanData) (p->state[i].potential_V +
			  p->restingPotential_Er);
			/* add increment to gk if at start of spike, otherwise allow decay*/
			if (p->state[i].lastSpikeState < spikeState_s) {
				dGk = (-p->state[i].kConductance_Gk + p->bOverDt) / p->
				  tGkOverDt;
				*outPtr += (ChanData) p->actionPotential;
			} else
				dGk = -p->state[i].kConductance_Gk / p->tGkOverDt;
			p->state[i].threshold_Th += dTh;
			p->state[i].potential_V += dV;
			p->state[i].kConductance_Gk += dGk;
			p->state[i].lastSpikeState = spikeState_s;
			spikeState_s = (p->state[i].potential_V < p->state[i].threshold_Th)?
			  0: 1;
		}
	}
 	SetProcessContinuity_EarObject(data);
	return(TRUE);
		
} /* RunModel_Neuron_ArleKim */
