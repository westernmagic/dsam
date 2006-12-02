/**********************
 *
 * File:		MoNCHHuxley.c
 * Purpose:		This module is Ray's general purpose neuron model, based on the
 *				Hodgkin Huxley implementation.
 * Comments:	Written using ModuleProducer version 1.10 (Oct 13 1996).
 *				18-12-96 LPO: debug operationMode added.  The output channels
 *				are in the order: V, leakage current, conductance, current,
 *				conductance, current,....
 *				03-01-97 LPO: The module has been amended so that up to three
 *				EarObjects can be connect to the module for (1) current
 *				injections; (2) excitatory conductance and (3) inhibitory
 *				conductance.
 *				If the current injection mode is "ON" then the first signal is
 *				always the current injection.
 *				15-01-97 LPO: Currected the activation (proportion of channels
 *				open) for the conductance calculations.
 *				18-01-97 The leakage conductance and potential are now part of
 *				of the IonChanList module - so the leakage conductance can be
 *				changed using the temperature scaling in the "Rothman" mode.
 *				27-01-98 LPO: Re-introduced the "restingPotential" parameter.
 *				If this is set to a non-physical value (>=0) then a calibration
 *				run is carried out to calculate the resting potential.
 *				30-01-98 LPO: Introducing a separate "diagnostic" (debug) mode.
 *				02-02-98 LPO: Fixed memory leak: iCList was not being free'd.
 * Author:		L. P. O'Mard
 * Created:		14 Oct 1996
 * Updated:		02 Feb 1998
 * Copyright:	(c) 1998, University of Essex
 *
 *********************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "GeNSpecLists.h"
#include "FiParFile.h"
#include "UtDynaList.h"
#include "UtIonChanList.h"
#include "UtString.h"
#include "MoNCHHuxley.h"

#include "UtDebug.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

HHuxleyNCPtr	hHuxleyNCPtr = NULL;

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
Free_Neuron_HHuxley(void)
{
	if (hHuxleyNCPtr == NULL)
		return(FALSE);
	FreeProcessVariables_Neuron_HHuxley();
	Free_IonChanList(&hHuxleyNCPtr->iCList);
	if (hHuxleyNCPtr->diagnosticModeList)
		free(hHuxleyNCPtr->diagnosticModeList);
	if (hHuxleyNCPtr->parList)
		FreeList_UniParMgr(&hHuxleyNCPtr->parList);
	if (hHuxleyNCPtr->parSpec == GLOBAL) {
		free(hHuxleyNCPtr);
		hHuxleyNCPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitInjectionModeList *************************/

/*
 * This routine intialises the InjectionMode list array.
 */

BOOLN
InitInjectionModeList_Neuron_HHuxley(void)
{
	static NameSpecifier	modeList[] = {

				{ wxT("OFF"),	HHUXLEYNC_INJECTION_OFF},
				{ wxT("ON"),	HHUXLEYNC_INJECTION_ON},
				{ NULL,			HHUXLEYNC_INJECTION_NULL }

			};
	hHuxleyNCPtr->injectionModeList = modeList;
	return(TRUE);

}

/****************************** InitOperationModeList *************************/

/*
 * This routine intialises the OperationMode list array.
 */

BOOLN
InitOperationModeList_Neuron_HHuxley(void)
{
	static NameSpecifier	modeList[] = {

				{ wxT("NORMAL"),		HHUXLEYNC_OPERATION_NORMAL_MODE},
				{ wxT("VOLTAGE_CLAMP"),	HHUXLEYNC_OPERATION_VOLTAGE_CLAMP_MODE},
				{ NULL,					HHUXLEYNC_OPERATION_NULL }

			};
	hHuxleyNCPtr->operationModeList = modeList;
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
Init_Neuron_HHuxley(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_Neuron_HHuxley");

	if (parSpec == GLOBAL) {
		if (hHuxleyNCPtr != NULL)
			Free_Neuron_HHuxley();
		if ((hHuxleyNCPtr = (HHuxleyNCPtr) malloc(sizeof(HHuxleyNC))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (hHuxleyNCPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	hHuxleyNCPtr->parSpec = parSpec;
	hHuxleyNCPtr->updateProcessVariablesFlag = TRUE;
	hHuxleyNCPtr->diagnosticModeFlag = TRUE;
	hHuxleyNCPtr->operationModeFlag = TRUE;
	hHuxleyNCPtr->injectionModeFlag = TRUE;
	hHuxleyNCPtr->excitatoryReversalPotFlag = TRUE;
	hHuxleyNCPtr->inhibitoryReversalPotFlag = TRUE;
	hHuxleyNCPtr->shuntInhibitoryReversalPotFlag = TRUE;
	hHuxleyNCPtr->cellCapacitanceFlag = TRUE;
	hHuxleyNCPtr->restingPotentialFlag = TRUE;
	hHuxleyNCPtr->restingSignalDurationFlag = TRUE;
	hHuxleyNCPtr->restingCriteriaFlag = TRUE;
	hHuxleyNCPtr->diagnosticMode = GENERAL_DIAGNOSTIC_OFF_MODE;
	hHuxleyNCPtr->operationMode = HHUXLEYNC_OPERATION_NORMAL_MODE;
	hHuxleyNCPtr->injectionMode = HHUXLEYNC_INJECTION_OFF;
	hHuxleyNCPtr->excitatoryReversalPot = -0.01;
	hHuxleyNCPtr->inhibitoryReversalPot = -0.01;
	hHuxleyNCPtr->shuntInhibitoryReversalPot = -0.01;
	hHuxleyNCPtr->cellCapacitance = 10e-12;
	hHuxleyNCPtr->restingPotential = -65.53e-3;
	hHuxleyNCPtr->restingSignalDuration = 30e-3;
	hHuxleyNCPtr->restingCriteria = 1.0e-9;
	if ((hHuxleyNCPtr->iCList = GenerateDefault_IonChanList()) == NULL) {
		NotifyError(wxT("%s: could not set default ICList."), funcName);
		return(FALSE);
	}

	InitInjectionModeList_Neuron_HHuxley();
	InitOperationModeList_Neuron_HHuxley();
	if ((hHuxleyNCPtr->diagnosticModeList = InitNameList_NSpecLists(
	  DiagModeList_NSpecLists(0), hHuxleyNCPtr->diagFileName)) == NULL)
		return(FALSE);
	if (!SetUniParList_Neuron_HHuxley()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Neuron_HHuxley();
		return(FALSE);
	}
	DSAM_strcpy(hHuxleyNCPtr->diagFileName, DEFAULT_FILE_NAME);
	hHuxleyNCPtr->fp = NULL;
	hHuxleyNCPtr->restingRun = FALSE;
	hHuxleyNCPtr->numChannels = 0;
	hHuxleyNCPtr->state = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_Neuron_HHuxley(void)
{
	static const WChar *funcName = wxT("SetUniParList_Neuron_HHuxley");
	UniParPtr	pars;

	if ((hHuxleyNCPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  NEURON_HHUXLEY_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = hHuxleyNCPtr->parList->pars;
	SetPar_UniParMgr(&pars[NEURON_HHUXLEY_DIAGNOSTICMODE], wxT("DIAG_MODE"),
	  wxT("Diagnostic mode ('off', 'screen' or <file name>)."),
	  UNIPAR_NAME_SPEC_WITH_FILE,
	  &hHuxleyNCPtr->diagnosticMode, hHuxleyNCPtr->diagnosticModeList,
	  (void * (*)) SetDiagnosticMode_Neuron_HHuxley);
	SetPar_UniParMgr(&pars[NEURON_HHUXLEY_OPERATIONMODE], wxT("OP_MODE"),
	  wxT("Operation mode ('normal' or 'voltage clamp')."),
	  UNIPAR_NAME_SPEC,
	  &hHuxleyNCPtr->operationMode, hHuxleyNCPtr->operationModeList,
	  (void * (*)) SetOperationMode_Neuron_HHuxley);
	SetPar_UniParMgr(&pars[NEURON_HHUXLEY_INJECTIONMODE], wxT("INJECT_MODE"),
	  wxT("Injection mode ('on' or 'off')."),
	  UNIPAR_BOOL,
	  &hHuxleyNCPtr->injectionMode, hHuxleyNCPtr->injectionModeList,
	  (void * (*)) SetInjectionMode_Neuron_HHuxley);
	SetPar_UniParMgr(&pars[NEURON_HHUXLEY_EXCITATORYREVERSALPOT],
	  wxT("EXCIT_REV_POT"),
	  wxT("Excitatory reversal potential (V)."),
	  UNIPAR_REAL,
	  &hHuxleyNCPtr->excitatoryReversalPot, NULL,
	  (void * (*)) SetExcitatoryReversalPot_Neuron_HHuxley);
	SetPar_UniParMgr(&pars[NEURON_HHUXLEY_INHIBITORYREVERSALPOT],
	  wxT("INHIB_REV_POT"),
	  wxT("Inhibitory reversal potential (V)."),
	  UNIPAR_REAL,
	  &hHuxleyNCPtr->inhibitoryReversalPot, NULL,
	  (void * (*)) SetInhibitoryReversalPot_Neuron_HHuxley);
	SetPar_UniParMgr(&pars[NEURON_HHUXLEY_SHUNTINHIBITORYREVERSALPOT],
	  wxT("SHUNT_INHIB_REV_POT"),
	  wxT("Shunt inhibitory reversal potential (V)."),
	  UNIPAR_REAL,
	  &hHuxleyNCPtr->shuntInhibitoryReversalPot, NULL,
	  (void * (*)) SetShuntInhibitoryReversalPot_Neuron_HHuxley);
	SetPar_UniParMgr(&pars[NEURON_HHUXLEY_CELLCAPACITANCE], wxT("CAPACITANCE"),
	  wxT("Cell capacitance (Farads)."),
	  UNIPAR_REAL,
	  &hHuxleyNCPtr->cellCapacitance, NULL,
	  (void * (*)) SetCellCapacitance_Neuron_HHuxley);
	SetPar_UniParMgr(&pars[NEURON_HHUXLEY_RESTINGPOTENTIAL], wxT("RESTING_POT"),
	  wxT("Resting potential (>=0 for calibration run)."),
	  UNIPAR_REAL,
	  &hHuxleyNCPtr->restingPotential, NULL,
	  (void * (*)) SetRestingPotential_Neuron_HHuxley);
	SetPar_UniParMgr(&pars[NEURON_HHUXLEY_RESTINGSIGNALDURATION], wxT(
	  "RESTING_DUR"),
	  wxT("Resting signal duration - used when finding V0 (s)."),
	  UNIPAR_REAL,
	  &hHuxleyNCPtr->restingSignalDuration, NULL,
	  (void * (*)) SetRestingSignalDuration_Neuron_HHuxley);
	SetPar_UniParMgr(&pars[NEURON_HHUXLEY_RESTINGCRITERIA], wxT("RESTING_DV"),
	  wxT("Resting criteria, dV (V)."),
	  UNIPAR_REAL,
	  &hHuxleyNCPtr->restingCriteria, NULL,
	  (void * (*)) SetRestingCriteria_Neuron_HHuxley);
	SetPar_UniParMgr(&pars[NEURON_HHUXLEY_ICLIST], wxT("ICLIST"),
	  wxT("Ion Channel List"),
	  UNIPAR_ICLIST,
	  &hHuxleyNCPtr->iCList, NULL,
	  (void * (*)) SetICList_Neuron_HHuxley);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Neuron_HHuxley(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_Neuron_HHuxley");

	if (hHuxleyNCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (hHuxleyNCPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(hHuxleyNCPtr->parList);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_Neuron_HHuxley(IonChanListPtr iCList, WChar *diagnosticMode,
  WChar *operationMode, WChar *injectionMode, double excitatoryReversalPot,
  double inhibitoryReversalPot, double shuntInhibitoryReversalPot,
  double cellCapacitance, double restingPotential, double restingSignalDuration,
  double restingCriteria)
{
	static const WChar	*funcName = wxT("SetPars_Neuron_HHuxley");
	BOOLN	ok;

	ok = TRUE;
	if (!SetICList_Neuron_HHuxley(iCList))
		ok = FALSE;
	if (!SetDiagnosticMode_Neuron_HHuxley(diagnosticMode))
		ok = FALSE;
	if (!SetOperationMode_Neuron_HHuxley(operationMode))
		ok = FALSE;
	if (!SetInjectionMode_Neuron_HHuxley(injectionMode))
		ok = FALSE;
	if (!SetExcitatoryReversalPot_Neuron_HHuxley(excitatoryReversalPot))
		ok = FALSE;
	if (!SetInhibitoryReversalPot_Neuron_HHuxley(inhibitoryReversalPot))
		ok = FALSE;
	if (!SetShuntInhibitoryReversalPot_Neuron_HHuxley(
	  shuntInhibitoryReversalPot))
		ok = FALSE;
	if (!SetCellCapacitance_Neuron_HHuxley(cellCapacitance))
		ok = FALSE;
	if (!SetRestingPotential_Neuron_HHuxley(restingPotential))
		ok = FALSE;
	if (!SetRestingSignalDuration_Neuron_HHuxley(restingSignalDuration))
		ok = FALSE;
	if (!SetRestingCriteria_Neuron_HHuxley(restingCriteria))
		ok = FALSE;
	if (!ok)
		NotifyError(wxT("%s: Failed to set all module parameters.") ,funcName);
	return(ok);

}
/****************************** SetICList *************************************/

/*
 * This function sets the module's iCList parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetICList_Neuron_HHuxley(IonChanListPtr theICList)
{
	static const WChar	*funcName = wxT("SetICList_Neuron_HHuxley");

	if (hHuxleyNCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hHuxleyNCPtr->updateProcessVariablesFlag = TRUE;
	Free_IonChanList(&hHuxleyNCPtr->iCList);
	hHuxleyNCPtr->iCList = theICList;
	return(TRUE);

}

/****************************** SetInjectionMode ******************************/

/*
 * This function sets the module's injectionMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetInjectionMode_Neuron_HHuxley(WChar *theInjectionMode)
{
	static const WChar	*funcName = wxT("SetInjectionMode_Neuron_HHuxley");
	int		specifier;

	if (hHuxleyNCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theInjectionMode,
	  hHuxleyNCPtr->injectionModeList)) == HHUXLEYNC_INJECTION_NULL) {
		NotifyError(wxT("%s: Illegal injectionMode name (%s)."), funcName,
		  theInjectionMode);
		return(FALSE);
	}
	hHuxleyNCPtr->updateProcessVariablesFlag = TRUE;
	hHuxleyNCPtr->injectionModeFlag = TRUE;
	hHuxleyNCPtr->injectionMode = specifier;
	return(TRUE);

}

/****************************** SetDiagnosticMode *****************************/

/*
 * This function sets the module's diagnosticMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDiagnosticMode_Neuron_HHuxley(WChar *theDiagnosticMode)
{
	static const WChar	*funcName = wxT("SetDiagnosticMode_Neuron_HHuxley");

	if (hHuxleyNCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	hHuxleyNCPtr->diagnosticModeFlag = TRUE;
	hHuxleyNCPtr->diagnosticMode = IdentifyDiag_NSpecLists(theDiagnosticMode,
	  hHuxleyNCPtr->diagnosticModeList);
	return(TRUE);

}

/****************************** SetOperationMode ******************************/

/*
 * This function sets the module's operationMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOperationMode_Neuron_HHuxley(WChar *theOperationMode)
{
	static const WChar	*funcName = wxT("SetOperationMode_Neuron_HHuxley");
	int		specifier;

	if (hHuxleyNCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theOperationMode,
	  hHuxleyNCPtr->operationModeList)) == HHUXLEYNC_OPERATION_NULL) {
		NotifyError(wxT("%s: Illegal operationMode name (%s)."), funcName,
		  theOperationMode);
		return(FALSE);
	}
	hHuxleyNCPtr->operationModeFlag = TRUE;
	hHuxleyNCPtr->operationMode = specifier;
	return(TRUE);

}

/****************************** SetExcitatoryReversalPot **********************/

/*
 * This function sets the module's excitatoryReversalPot parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetExcitatoryReversalPot_Neuron_HHuxley(double theExcitatoryReversalPot)
{
	static const WChar	*funcName = wxT(
	  "SetExcitatoryReversalPot_Neuron_HHuxley");

	if (hHuxleyNCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hHuxleyNCPtr->excitatoryReversalPotFlag = TRUE;
	hHuxleyNCPtr->excitatoryReversalPot = theExcitatoryReversalPot;
	return(TRUE);

}

/****************************** SetInhibitoryReversalPot **********************/

/*
 * This function sets the module's inhibitoryReversalPot parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetInhibitoryReversalPot_Neuron_HHuxley(double theInhibitoryReversalPot)
{
	static const WChar	*funcName = wxT(
	  "SetInhibitoryReversalPot_Neuron_HHuxley");

	if (hHuxleyNCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hHuxleyNCPtr->inhibitoryReversalPotFlag = TRUE;
	hHuxleyNCPtr->inhibitoryReversalPot = theInhibitoryReversalPot;
	return(TRUE);

}

/****************************** SetShuntInhibitoryReversalPot *****************/

/*
 * This function sets the module's shuntInhibitoryReversalPot parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetShuntInhibitoryReversalPot_Neuron_HHuxley(double
  theShuntInhibitoryReversalPot)
{
	static const WChar	*funcName =
	  wxT("SetShuntInhibitoryReversalPot_Neuron_HHuxley");

	if (hHuxleyNCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hHuxleyNCPtr->shuntInhibitoryReversalPotFlag = TRUE;
	hHuxleyNCPtr->shuntInhibitoryReversalPot = theShuntInhibitoryReversalPot;
	return(TRUE);

}

/****************************** SetCellCapacitance ****************************/

/*
 * This function sets the module's cellCapacitance parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCellCapacitance_Neuron_HHuxley(double theCellCapacitance)
{
	static const WChar	*funcName = wxT("SetCellCapacitance_Neuron_HHuxley");

	if (hHuxleyNCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hHuxleyNCPtr->updateProcessVariablesFlag = TRUE;
	hHuxleyNCPtr->cellCapacitanceFlag = TRUE;
	hHuxleyNCPtr->cellCapacitance = theCellCapacitance;
	return(TRUE);

}

/****************************** SetRestingPotential ***************************/

/*
 * This function sets the module's restingPotential parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRestingPotential_Neuron_HHuxley(double theRestingPotential)
{
	static const WChar	*funcName = wxT("SetRestingPotential_Neuron_HHuxley");

	if (hHuxleyNCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hHuxleyNCPtr->restingPotentialFlag = TRUE;
	hHuxleyNCPtr->restingPotential = theRestingPotential;
	return(TRUE);

}

/****************************** SetRestingSignalDuration **********************/

/*
 * This function sets the module's restingSignalDuration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRestingSignalDuration_Neuron_HHuxley(double theRestingSignalDuration)
{
	static const WChar	*funcName = wxT(
	  "SetRestingSignalDuration_Neuron_HHuxley");

	if (hHuxleyNCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hHuxleyNCPtr->restingSignalDurationFlag = TRUE;
	hHuxleyNCPtr->restingSignalDuration = theRestingSignalDuration;
	return(TRUE);

}

/****************************** SetRestingCriteria ****************************/

/*
 * This function sets the module's restingCriteria parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRestingCriteria_Neuron_HHuxley(double theRestingCriteria)
{
	static const WChar	*funcName = wxT("SetRestingCriteria_Neuron_HHuxley");

	if (hHuxleyNCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	hHuxleyNCPtr->restingCriteriaFlag = TRUE;
	hHuxleyNCPtr->restingCriteria = theRestingCriteria;
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
CheckPars_Neuron_HHuxley(void)
{
	static const WChar	*funcName = wxT("CheckPars_Neuron_HHuxley");
	BOOLN	ok;

	ok = TRUE;
	if (hHuxleyNCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (!hHuxleyNCPtr->iCList) {
		NotifyError(wxT("%s: iCList variable not set."), funcName);
		ok = FALSE;
	}
	if (!hHuxleyNCPtr->diagnosticModeFlag) {
		NotifyError(wxT("%s: diagnosticMode variable not set."), funcName);
		ok = FALSE;
	}
	if (!hHuxleyNCPtr->operationModeFlag) {
		NotifyError(wxT("%s: operationMode variable not set."), funcName);
		ok = FALSE;
	}
	if (!hHuxleyNCPtr->injectionModeFlag) {
		NotifyError(wxT("%s: injectionMode variable not set."), funcName);
		ok = FALSE;
	}
	if (!hHuxleyNCPtr->excitatoryReversalPotFlag) {
		NotifyError(wxT("%s: excitatoryReversalPot variable not set."),
		  funcName);
		ok = FALSE;
	}
	if (!hHuxleyNCPtr->inhibitoryReversalPotFlag) {
		NotifyError(wxT("%s: inhibitoryReversalPot variable not set."),
		  funcName);
		ok = FALSE;
	}
	if (!hHuxleyNCPtr->shuntInhibitoryReversalPotFlag) {
		NotifyError(wxT("%s: shuntInhibitoryReversalPot variable not set."),
		  funcName);
		ok = FALSE;
	}
	if (!hHuxleyNCPtr->cellCapacitanceFlag) {
		NotifyError(wxT("%s: cellCapacitance variable not set."), funcName);
		ok = FALSE;
	}
	if (!hHuxleyNCPtr->restingPotentialFlag) {
		NotifyError(wxT("%s: restingPotential variable not set."), funcName);
		ok = FALSE;
	}
	if (!hHuxleyNCPtr->restingSignalDurationFlag) {
		NotifyError(wxT("%s: restingSignalDuration variable not set."),
		  funcName);
		ok = FALSE;
	}
	if (!hHuxleyNCPtr->restingCriteriaFlag) {
		NotifyError(wxT("%s: restingCriteria variable not set."), funcName);
		ok = FALSE;
	}
	return(ok);

}

/****************************** GetICListPtr **********************************/

/*
 * This routine returns a pointer to the module's ion channel list data pointer.
 *
 */

IonChanListPtr
GetICListPtr_Neuron_HHuxley(void)
{
	static const WChar *funcName = wxT("GetICListPtr_Neuron_HHuxley");

	if (hHuxleyNCPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (hHuxleyNCPtr->iCList == NULL) {
		NotifyError(wxT("%s: Ion Channel list data structure has not been ")
		  wxT("correctly set.  NULL returned."), funcName);
		return(NULL);
	}
	return(hHuxleyNCPtr->iCList);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the dSAM.parsFile file pointer.
 */

BOOLN
PrintPars_Neuron_HHuxley(void)
{
	static const WChar	*funcName = wxT("PrintPars_Neuron_HHuxley");

	if (!CheckPars_Neuron_HHuxley()) {
		NotifyError(wxT("%s: Parameters have not been correctly set."),
		  funcName);
		return(FALSE);
	}
	DPrint(wxT("Hodgkin-Huxley Neural Cell Module Parameters:-\n"));
	PrintPars_IonChanList(hHuxleyNCPtr->iCList);
	DPrint(wxT("\tDiagnostics mode: %s,"), hHuxleyNCPtr->diagnosticModeList[
	  hHuxleyNCPtr->diagnosticMode].name);
	DPrint(wxT("\tOperation mode: %s,"),
	  hHuxleyNCPtr->operationModeList[hHuxleyNCPtr->operationMode].name);
	DPrint(wxT("\tInjection mode = %s,\n"),
	  hHuxleyNCPtr->injectionModeList[hHuxleyNCPtr->injectionMode].name);
	DPrint(wxT("\tExcitatory reversal potential = %g (mV),\n"),
	  MILLI(hHuxleyNCPtr->excitatoryReversalPot));
	DPrint(wxT("\tInhibitory reversal potential = %g (mV),\n"),
	  MILLI(hHuxleyNCPtr->inhibitoryReversalPot));
	DPrint(wxT("\tShunt inhibitory reversal potential = %g (mV),\n"),
	  MILLI(hHuxleyNCPtr->shuntInhibitoryReversalPot));
	DPrint(wxT("\tCell capacitance = %g (F),\n"),
	  hHuxleyNCPtr->cellCapacitance);
	DPrint(wxT("\tRestingPotential = %g mV\n"),
	  MILLI(hHuxleyNCPtr->restingPotential));
	DPrint(wxT("\tResting calibration signal duration = %g ms,\n"),
	  MSEC(hHuxleyNCPtr->restingSignalDuration));
	DPrint(wxT("\tResting criteria, dV = %g mV.\n"),
	  MILLI(hHuxleyNCPtr->restingCriteria));
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_Neuron_HHuxley(WChar *fileName)
{
	static const WChar	*funcName = wxT("ReadPars_Neuron_HHuxley");
	BOOLN	ok;
	WChar	*filePath;
	IonChanListPtr	iCList;
	WChar	diagnosticMode[MAXLINE], operationMode[MAXLINE];
	WChar	injectionMode[MAXLINE];
	double	excitatoryReversalPot, inhibitoryReversalPot, cellCapacitance;
	double	restingPotential, restingSignalDuration, restingCriteria;
	double	shuntInhibitoryReversalPot;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = DSAM_fopen(filePath, "r")) == NULL) {
		NotifyError(wxT("%s: Cannot open data file '%s'.\n"), funcName,
		  filePath);
		return(FALSE);
	}
	DPrint(wxT("%s: Reading from '%s':\n"), funcName, filePath);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, wxT("%s"), diagnosticMode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%s"), operationMode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%s"), injectionMode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &excitatoryReversalPot))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &inhibitoryReversalPot))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &shuntInhibitoryReversalPot))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &cellCapacitance))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &restingPotential))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &restingSignalDuration))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &restingCriteria))
		ok = FALSE;
	if ((iCList = ReadPars_IonChanList(fp)) == NULL)
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError(wxT("%s: Not enough lines, or invalid parameters, in ")
		  wxT("module parameter file '%s'."), funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_Neuron_HHuxley(iCList, diagnosticMode, operationMode,
	  injectionMode, excitatoryReversalPot, inhibitoryReversalPot,
	  shuntInhibitoryReversalPot, cellCapacitance, restingPotential,
	  restingSignalDuration, restingCriteria)) {
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
SetParsPointer_Neuron_HHuxley(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Neuron_HHuxley");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	hHuxleyNCPtr = (HHuxleyNCPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Neuron_HHuxley(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Neuron_HHuxley");

	if (!SetParsPointer_Neuron_HHuxley(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Neuron_HHuxley(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = hHuxleyNCPtr;
	theModule->CheckPars = CheckPars_Neuron_HHuxley;
	theModule->Free = Free_Neuron_HHuxley;
	theModule->GetData = GetPotentialResponse_Neuron_HHuxley;
	theModule->GetUniParListPtr = GetUniParListPtr_Neuron_HHuxley;
	theModule->PrintPars = PrintPars_Neuron_HHuxley;
	theModule->ReadPars = ReadPars_Neuron_HHuxley;
	theModule->RunProcess = RunModel_Neuron_HHuxley;
	theModule->SetParsPointer = SetParsPointer_Neuron_HHuxley;
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
CheckData_Neuron_HHuxley(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Neuron_HHuxley");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	if ((hHuxleyNCPtr->injectionMode == HHUXLEYNC_INJECTION_OFF) &&
	  ((hHuxleyNCPtr->operationMode ==
	  HHUXLEYNC_OPERATION_VOLTAGE_CLAMP_MODE))) {
		NotifyError(wxT("%s: Voltage clamp mode must also use injection mode."),
		  funcName);
		return(FALSE);
	}
	/*** Put additional checks here. ***/
	return(TRUE);

}

/****************************** FindRestingPotential **************************/

/*
 * This routine calculates the resting potential of the model.
 * It uses a temporary EarObject with a zero input signal.
 * Note that the wxT("normal") operation mode must be used to calculate the
 * resting potential.
 */

double
FindRestingPotential_Neuron_HHuxley(double restingCriteria, double duration,
  double dt)
{
	static const WChar	*funcName = wxT("FindRestingPotential_Neuron_HHuxley");
	int		i, oldOperationMode, oldInjectionMode;
	double	deltaV, newPotential;
	EarObjectPtr	stimulus = NULL, findResting = NULL;
	
	hHuxleyNCPtr->restingRun = TRUE;
	if ((stimulus = Init_EarObject(wxT("null"))) == NULL) {
		NotifyError(wxT("%s: Out of memory for stimulus EarObjectPtr."),
		  funcName);
		exit(1);
	}
	if ((findResting = Init_EarObject(wxT("null"))) == NULL) {
		NotifyError(wxT("%s: Out of memory for findResing EarObjectPtr."),
		  funcName);
		exit(1);
	}
	if (!InitOutSignal_EarObject(stimulus, 1, (ChanLen) (duration / dt), dt)) {
		NotifyError(wxT("%s: Could not initialise stimulus output signal."),
		  funcName);
		return(FALSE);
	}
	oldOperationMode = hHuxleyNCPtr->operationMode;
	oldInjectionMode = hHuxleyNCPtr->injectionMode;
	hHuxleyNCPtr->operationMode = HHUXLEYNC_OPERATION_NORMAL_MODE;
	hHuxleyNCPtr->injectionMode = HHUXLEYNC_INJECTION_ON;
	ConnectOutSignalToIn_EarObject(stimulus, findResting);
	i = 0;
	do {
		RunModel_Neuron_HHuxley(findResting);
		newPotential = GetSample_EarObject(findResting, 0,
		  findResting->outSignal->length - 1);
		deltaV = newPotential - GetSample_EarObject(findResting, 0, 0);
		i++;
	} while ((fabs(deltaV) > restingCriteria) && (i <
	  HHUXLEY_FIND_REST_MAX_LOOP));
	Free_EarObject(&stimulus);
	Free_EarObject(&findResting);
	if (i == HHUXLEY_FIND_REST_MAX_LOOP)
		NotifyWarning(wxT("%s: Failed to satisfy resting criteria\nafter %d ")
		  wxT("loops."), funcName, i);
	hHuxleyNCPtr->operationMode = oldOperationMode;
	hHuxleyNCPtr->injectionMode = oldInjectionMode;
	hHuxleyNCPtr->restingRun = FALSE;
	return(newPotential);

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 * It also initialises the variables.
 */

BOOLN
InitProcessVariables_Neuron_HHuxley(EarObjectPtr data)
{
	static const WChar *funcName = wxT("InitProcessVariables_Neuron_HHuxley");
	BOOLN	ok = TRUE;
	int		i, j;
	double	*yPtr, *zPtr;
	DynaListPtr		node;
	HHuxleyNCPtr	p;
	IonChannelPtr	iC;
	ICTableEntryPtr	e;

	p = hHuxleyNCPtr;
	if (!p->restingRun && (p->updateProcessVariablesFlag ||
	  data->updateProcessFlag || p->iCList->updateFlag || (data->timeIndex ==
	  PROCESS_START_TIME))) {
		p->numChannels = _OutSig_EarObject(data)->numChannels;
		if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
			FreeProcessVariables_Neuron_HHuxley();
			OpenDiagnostics_NSpecLists(&hHuxleyNCPtr->fp,
			  hHuxleyNCPtr->diagnosticModeList, hHuxleyNCPtr->diagnosticMode);
			if ((p->state = (HHuxleyState *) calloc(p->numChannels,
			   sizeof(HHuxleyState))) == NULL) {
			 	NotifyError(wxT("%s: Out of memory."), funcName);
			 	return(FALSE);
			}
			for (i = 0; i < p->numChannels; i++) {
				if ((p->state[i].y = (double *) calloc(p->iCList->numChannels,
				  sizeof(double))) == NULL) {
					NotifyError(wxT("%s: Out of memory for state->y array"),
					  funcName);
					ok = FALSE;
				}
				if ((p->state[i].z = (double *) calloc(p->iCList->numChannels,
				  sizeof(double))) == NULL) {
					NotifyError(wxT("%s: Out of memory for state->z array"),
					  funcName);
					ok = FALSE;
				}
			}
			if (!ok) {
				FreeProcessVariables_Neuron_HHuxley();
				return(FALSE);
			}
			if (!p->restingRun && (p->restingPotential >= 0.0)) {
				for (i = 0; i < p->numChannels; i++) {
					p->state[i].potential_V = -p->restingPotential;
					yPtr = p->state[i].y;
					zPtr = p->state[i].z;
					for (j = 0; j < p->iCList->numChannels; j++)
						*yPtr++ = *zPtr++ = 0.0;
				}
				p->restingPotential = FindRestingPotential_Neuron_HHuxley(
				  p->restingCriteria, p->restingSignalDuration,
				  _OutSig_EarObject(data)->dt);
			}
			p->updateProcessVariablesFlag = FALSE;
			p->iCList->updateFlag = FALSE;
		}
		for (i = 0; i < p->numChannels; i++) {
			p->state[i].potential_V = p->restingPotential;
			yPtr = p->state[i].y;
			zPtr = p->state[i].z;
			for (node = p->iCList->ionChannels; node; node = node->next, yPtr++,
			  zPtr++) {
				iC = (IonChannelPtr) node->data;
				if (!iC->enabled)
					continue;
				if ((e = GetTableEntry_IonChanList(iC, p->restingPotential)) ==
				  NULL) {
					NotifyError(wxT("%s: Could not find entry."), funcName);
					return(FALSE);
				}
				*yPtr = e->yY;
				*zPtr = e->zZ;
			}
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
FreeProcessVariables_Neuron_HHuxley(void)
{
	int		i;

	if (hHuxleyNCPtr->state == NULL)
		return;
	for (i = 0; i < hHuxleyNCPtr->numChannels; i++) {
		if (hHuxleyNCPtr->state[i].y)
			free(hHuxleyNCPtr->state[i].y);
		if (hHuxleyNCPtr->state[i].z)
			free(hHuxleyNCPtr->state[i].z);
	}
	free(hHuxleyNCPtr->state);
	hHuxleyNCPtr->state = NULL;
	if (hHuxleyNCPtr->fp) {
		fclose(hHuxleyNCPtr->fp);
		hHuxleyNCPtr->fp = NULL;
	}
	hHuxleyNCPtr->updateProcessVariablesFlag = TRUE;

}

/****************************** GetPotentialResponse **************************/

/*
 * This routine is for compatibility with the other neuron modules, it just
 * returns the potential.
 */

void *
GetPotentialResponse_Neuron_HHuxley(void *potentialPtr)
{
	static const WChar *funcName = wxT("GetPotentialResponse_Neuron_HHuxley");
	double *potential;

	potential = (double *) potentialPtr;
	if (!CheckPars_Neuron_HHuxley()) {
		NotifyError(wxT("%s: Parameters have not been correctly set, zero ")\
		  wxT("returned."), funcName);
		return(NULL);
	}
	return(potentialPtr);

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
 * The 'deltaY' and 'deltaZ' intermediate variables were used because the
 * SGI C++ compiler had a bug which caused the x and y values to be calculated
 * incorrectly.  This was the best solution I could find for the problem.
 */

BOOLN
RunModel_Neuron_HHuxley(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("RunModel_Neuron_HHuxley");
	register	ChanData	*gExPtr, *gInPtr, *gSInPtr, *injPtr, *outPtr;
	register	double		deltaY, deltaZ;
	int		i, inSignal;
	double	*yPtr, *zPtr, ionChanCurrentSum, currentSum;
	double	activation, conductance;
	ChanLen	j;
	DynaListPtr		node;
	HHuxleyStatePtr	s;
	IonChannelPtr	iC;
	ICTableEntryPtr	e;
	SignalDataPtr	outSignal;
	HHuxleyNCPtr	p = hHuxleyNCPtr;

	if (!data->threadRunFlag) {
		if (!CheckPars_Neuron_HHuxley())
			return(FALSE);
		if (!CheckData_Neuron_HHuxley(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		if (!InitOutSignal_EarObject(data, _InSig_EarObject(data, 0)->numChannels,
		  _InSig_EarObject(data, 0)->length, _InSig_EarObject(data, 0)->dt)) {
			NotifyError(wxT("%s: Could not initialise output signal."),
			  funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Hodkin-Huxley neural cell"));
		if (!InitProcessVariables_Neuron_HHuxley(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		p->dt = _OutSig_EarObject(data)->dt;
		p->dtOverC = p->dt / p->cellCapacitance;
		p->debug = !p->restingRun && (p->diagnosticMode !=
		  GENERAL_DIAGNOSTIC_OFF_MODE);
		if (data->initThreadRunFlag)
			return(TRUE);
	}

	outSignal = _OutSig_EarObject(data);
	if (p->debug) {
		DynaListPtr	node;

		DSAM_fprintf(p->fp, wxT("Time(s)\tVm (mV)\tIleak (nA)"));
		for (node = p->iCList->ionChannels; node; node = node->next) {
			iC = (IonChannelPtr) node->data;
			if (!iC->enabled)
				continue;
			DSAM_fprintf(p->fp, wxT("\tG(%s)\tI(%s)"), iC->description, iC->
			  description);
		}
		DSAM_fprintf(p->fp, wxT("\n"));
	}

	for (i = 0; i < outSignal->numChannels; i++) {
		s = &p->state[i];
		inSignal = 0;
		injPtr = (p->injectionMode == HHUXLEYNC_INJECTION_OFF)?
		  (ChanData *) NULL: _InSig_EarObject(data, inSignal++)->channel[i];
		gExPtr = (inSignal < data->numInSignals)?
		  _InSig_EarObject(data, inSignal++)->channel[i]: (ChanData *) NULL;
		gInPtr = (inSignal < data->numInSignals)?
		  _InSig_EarObject(data, inSignal++)->channel[i]: (ChanData *) NULL;
		gSInPtr = (inSignal < data->numInSignals)?
		  _InSig_EarObject(data, inSignal++)->channel[i]: (ChanData *) NULL;
		outPtr = outSignal->channel[i];
		switch (p->operationMode) {
		case HHUXLEYNC_OPERATION_NORMAL_MODE:
			for (j = 0; j < outSignal->length; j++, outPtr++) {
				yPtr = s->y;
				zPtr = s->z;
				if (p->debug)
				  	DSAM_fprintf(p->fp, wxT("%g\t%g\t%g"),  MILLI(j * p->dt),
					  MILLI(s->potential_V), NANO(p->iCList->leakageCond *
					  (s->potential_V - p->iCList->leakagePot)));
				
				for (node = p->iCList->ionChannels, ionChanCurrentSum = 0.0;
				  node; node = node->next, yPtr++, zPtr++) {
					iC = (IonChannelPtr) node->data;
					if (!iC->enabled)
						continue;
					activation = iC->PowFunc(*yPtr, iC->activationExponent);
					if (p->debug) {
						conductance = iC->maxConductance * activation * *zPtr;
				  		DSAM_fprintf(p->fp, wxT("\t%g\t%g"), NANO(conductance),
						  NANO(conductance * (s->potential_V -
						  iC->equilibriumPot)));
				  	}
					ionChanCurrentSum += iC->maxConductance * activation *
					  *zPtr * (s->potential_V - iC->equilibriumPot);
				}
				if (p->debug)
					DSAM_fprintf(p->fp, wxT("\n"));
				currentSum = ionChanCurrentSum + p->iCList->leakageCond *
				  (s->potential_V - p->iCList->leakagePot);
				if (injPtr)
					currentSum -= *injPtr++;
				if (gExPtr)
					currentSum += *gExPtr++ * (s->potential_V -
					  p->excitatoryReversalPot);
				if (gInPtr)
					currentSum += *gInPtr++ * (s->potential_V -
					  p->inhibitoryReversalPot);
				if (gSInPtr)
					currentSum += *gSInPtr++ * (s->potential_V -
					  p->shuntInhibitoryReversalPot);
				*outPtr = s->potential_V - currentSum * p->dtOverC;
				yPtr = s->y;
				zPtr = s->z;
				for (node = p->iCList->ionChannels; node; node = node->next) {
					iC = (IonChannelPtr) node->data;
					if (!iC->enabled)
						continue;
					if ((e = GetTableEntry_IonChanList(iC, *outPtr)) == NULL) {
						NotifyError(wxT("%s: Could not find entry."), funcName);
						return(FALSE);
					}
					deltaY = (e->yY - *yPtr) * p->dt / e->ty;
					deltaZ = (e->zZ - *zPtr) * p->dt / e->tz;
					*yPtr++ += deltaY;
					*zPtr++ += deltaZ;
				}
				s->potential_V = *outPtr;
			}
			break;
		case HHUXLEYNC_OPERATION_VOLTAGE_CLAMP_MODE:
			for (j = 0; j < outSignal->length; j++, outPtr++, injPtr++) {
				if (p->debug)
					DSAM_fprintf(p->fp, wxT("%g\t%g\t%g"), MILLI(j * p->dt), MILLI(
					  *injPtr), NANO(p->iCList->leakageCond * (*injPtr -
					  p->iCList->leakagePot)));
				yPtr = s->y;
				zPtr = s->z;
				for (node = p->iCList->ionChannels, ionChanCurrentSum = 0.0;
				  node; node = node->next, yPtr++, zPtr++) {
					iC = (IonChannelPtr) node->data;
					if (!iC->enabled)
						continue;
					activation = iC->PowFunc(*yPtr, iC->activationExponent);
					if (p->debug) {
						conductance = iC->maxConductance * activation * *zPtr;
				  		DSAM_fprintf(p->fp, wxT("\t%g\t%g"), NANO(conductance),
						  NANO(conductance * (*injPtr - iC->equilibriumPot)));
				  	}
					ionChanCurrentSum += iC->maxConductance *
					  activation * *zPtr * (*injPtr - iC->equilibriumPot);
				}
				if (p->debug)
					DSAM_fprintf(p->fp, wxT("\n"));
				*outPtr = -ionChanCurrentSum - p->iCList->leakageCond *
				  (*injPtr - p->iCList->leakagePot);
				if (gExPtr)
					*outPtr -= *gExPtr++ * (*injPtr - p->excitatoryReversalPot);
				if (gInPtr)
					*outPtr -= *gInPtr++ * (*injPtr - p->inhibitoryReversalPot);
				if (gSInPtr)
					*outPtr -= *gSInPtr++ * (*injPtr - p->
					  shuntInhibitoryReversalPot);
				yPtr = s->y;
				zPtr = s->z;
				for (node = p->iCList->ionChannels; node; node = node->next) {
					iC = (IonChannelPtr) node->data;
					if (!iC->enabled)
						continue;
					if ((e = GetTableEntry_IonChanList(iC, *injPtr)) == NULL) {
						NotifyError(wxT("%s: Could not find entry."), funcName);
						return(FALSE);
					}
					deltaY = (e->yY - *yPtr) * p->dt / e->ty;
					deltaZ = (e->zZ - *zPtr) * p->dt / e->tz;
					*yPtr++ += deltaY;
					*zPtr++ += deltaZ;
				}
			}
			break;
		} /* switch */
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

