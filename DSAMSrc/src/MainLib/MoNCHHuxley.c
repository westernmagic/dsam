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
	/* static const char	*funcName = "Free_Neuron_HHuxley";  */

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

					{ "OFF",	HHUXLEYNC_INJECTION_OFF},
					{ "ON",		HHUXLEYNC_INJECTION_ON},
					{ "",		HHUXLEYNC_INJECTION_NULL }
				
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

					{ "NORMAL",			HHUXLEYNC_OPERATION_NORMAL_MODE},
					{ "VOLTAGE_CLAMP",	HHUXLEYNC_OPERATION_VOLTAGE_CLAMP_MODE},
					{ "",				HHUXLEYNC_OPERATION_NULL }
				
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
	static const char	*funcName = "Init_Neuron_HHuxley";

	if (parSpec == GLOBAL) {
		if (hHuxleyNCPtr != NULL)
			Free_Neuron_HHuxley();
		if ((hHuxleyNCPtr = (HHuxleyNCPtr) malloc(sizeof(HHuxleyNC))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (hHuxleyNCPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	hHuxleyNCPtr->parSpec = parSpec;
	hHuxleyNCPtr->updateProcessVariablesFlag = TRUE;
	hHuxleyNCPtr->diagnosticModeFlag = FALSE;
	hHuxleyNCPtr->operationModeFlag = FALSE;
	hHuxleyNCPtr->injectionModeFlag = FALSE;
	hHuxleyNCPtr->excitatoryReversalPotFlag = FALSE;
	hHuxleyNCPtr->inhibitoryReversalPotFlag = FALSE;
	hHuxleyNCPtr->shuntInhibitoryReversalPotFlag = FALSE;
	hHuxleyNCPtr->cellCapacitanceFlag = FALSE;
	hHuxleyNCPtr->restingPotentialFlag = FALSE;
	hHuxleyNCPtr->restingSignalDurationFlag = FALSE;
	hHuxleyNCPtr->restingCriteriaFlag = FALSE;
	hHuxleyNCPtr->iCList = NULL;
	hHuxleyNCPtr->diagnosticMode = 0;
	hHuxleyNCPtr->operationMode = 0;
	hHuxleyNCPtr->injectionMode = 0;
	hHuxleyNCPtr->excitatoryReversalPot = 0.0;
	hHuxleyNCPtr->inhibitoryReversalPot = 0.0;
	hHuxleyNCPtr->shuntInhibitoryReversalPot = 0.0;
	hHuxleyNCPtr->cellCapacitance = 0.0;
	hHuxleyNCPtr->restingPotential = 0.0;
	hHuxleyNCPtr->restingSignalDuration = 0.0;
	hHuxleyNCPtr->restingCriteria = 0.0;

	InitInjectionModeList_Neuron_HHuxley();
	InitOperationModeList_Neuron_HHuxley();
	if ((hHuxleyNCPtr->diagnosticModeList = InitNameList_NSpecLists(
	  DiagModeList_NSpecLists(0), hHuxleyNCPtr->diagFileName)) == NULL)
		return(FALSE);
	if (!SetUniParList_Neuron_HHuxley()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_Neuron_HHuxley();
		return(FALSE);
	}
	strcpy(hHuxleyNCPtr->diagFileName, DEFAULT_FILE_NAME);
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
	static const char *funcName = "SetUniParList_Neuron_HHuxley";
	UniParPtr	pars;

	if ((hHuxleyNCPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  NEURON_HHUXLEY_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = hHuxleyNCPtr->parList->pars;
	SetPar_UniParMgr(&pars[NEURON_HHUXLEY_DIAGNOSTICMODE], "DIAG_MODE",
	  "Diagnostic mode ('off', 'screen' or <file name>).",
	  UNIPAR_NAME_SPEC_WITH_FILE,
	  &hHuxleyNCPtr->diagnosticMode, hHuxleyNCPtr->diagnosticModeList,
	  (void * (*)) SetDiagnosticMode_Neuron_HHuxley);
	SetPar_UniParMgr(&pars[NEURON_HHUXLEY_OPERATIONMODE], "OP_MODE",
	  "Operation mode ('normal' or 'voltage clamp').",
	  UNIPAR_NAME_SPEC,
	  &hHuxleyNCPtr->operationMode, hHuxleyNCPtr->operationModeList,
	  (void * (*)) SetOperationMode_Neuron_HHuxley);
	SetPar_UniParMgr(&pars[NEURON_HHUXLEY_INJECTIONMODE], "INJECT_MODE",
	  "Injection mode ('on' or 'off').",
	  UNIPAR_BOOL,
	  &hHuxleyNCPtr->injectionMode, hHuxleyNCPtr->injectionModeList,
	  (void * (*)) SetInjectionMode_Neuron_HHuxley);
	SetPar_UniParMgr(&pars[NEURON_HHUXLEY_EXCITATORYREVERSALPOT],
	  "EXCIT_REV_POT",
	  "Excitatory reversal potential (V).",
	  UNIPAR_REAL,
	  &hHuxleyNCPtr->excitatoryReversalPot, NULL,
	  (void * (*)) SetExcitatoryReversalPot_Neuron_HHuxley);
	SetPar_UniParMgr(&pars[NEURON_HHUXLEY_INHIBITORYREVERSALPOT],
	  "INHIB_REV_POT",
	  "Inhibitory reversal potential (V).",
	  UNIPAR_REAL,
	  &hHuxleyNCPtr->inhibitoryReversalPot, NULL,
	  (void * (*)) SetInhibitoryReversalPot_Neuron_HHuxley);
	SetPar_UniParMgr(&pars[NEURON_HHUXLEY_SHUNTINHIBITORYREVERSALPOT],
	  "SHUNT_INHIB_REV_POT",
	  "Shunt inhibitory reversal potential (V).",
	  UNIPAR_REAL,
	  &hHuxleyNCPtr->shuntInhibitoryReversalPot, NULL,
	  (void * (*)) SetShuntInhibitoryReversalPot_Neuron_HHuxley);
	SetPar_UniParMgr(&pars[NEURON_HHUXLEY_CELLCAPACITANCE], "CAPACITANCE",
	  "Cell capacitance (Farads).",
	  UNIPAR_REAL,
	  &hHuxleyNCPtr->cellCapacitance, NULL,
	  (void * (*)) SetCellCapacitance_Neuron_HHuxley);
	SetPar_UniParMgr(&pars[NEURON_HHUXLEY_RESTINGPOTENTIAL], "RESTING_POT",
	  "Resting potential (>=0 for calibration run).",
	  UNIPAR_REAL,
	  &hHuxleyNCPtr->restingPotential, NULL,
	  (void * (*)) SetRestingPotential_Neuron_HHuxley);
	SetPar_UniParMgr(&pars[NEURON_HHUXLEY_RESTINGSIGNALDURATION], "RESTING_DUR",
	  "Resting signal duration - used when finding V0 (s).",
	  UNIPAR_REAL,
	  &hHuxleyNCPtr->restingSignalDuration, NULL,
	  (void * (*)) SetRestingSignalDuration_Neuron_HHuxley);
	SetPar_UniParMgr(&pars[NEURON_HHUXLEY_RESTINGCRITERIA], "RESTING_DV",
	  "Resting criteria, dV (V).",
	  UNIPAR_REAL,
	  &hHuxleyNCPtr->restingCriteria, NULL,
	  (void * (*)) SetRestingCriteria_Neuron_HHuxley);
	SetPar_UniParMgr(&pars[NEURON_HHUXLEY_ICLIST], "ICLIST",
	  "Ion Channel List",
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
	static const char	*funcName = "GetUniParListPtr_Neuron_HHuxley";

	if (hHuxleyNCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (hHuxleyNCPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
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
SetPars_Neuron_HHuxley(IonChanListPtr iCList, char *diagnosticMode,
  char *operationMode, char *injectionMode, double excitatoryReversalPot,
  double inhibitoryReversalPot, double shuntInhibitoryReversalPot,
  double cellCapacitance, double restingPotential, double restingSignalDuration,
  double restingCriteria)
{
	static const char	*funcName = "SetPars_Neuron_HHuxley";
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
		NotifyError("%s: Failed to set all module parameters." ,funcName);
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
	static const char	*funcName = "SetICList_Neuron_HHuxley";

	if (hHuxleyNCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
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
SetInjectionMode_Neuron_HHuxley(char *theInjectionMode)
{
	static const char	*funcName = "SetInjectionMode_Neuron_HHuxley";
	int		specifier;

	if (hHuxleyNCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theInjectionMode,
	  hHuxleyNCPtr->injectionModeList)) == HHUXLEYNC_INJECTION_NULL) {
		NotifyError("%s: Illegal injectionMode name (%s).", funcName,
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
SetDiagnosticMode_Neuron_HHuxley(char *theDiagnosticMode)
{
	static const char	*funcName = "SetDiagnosticMode_Neuron_HHuxley";

	if (hHuxleyNCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
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
SetOperationMode_Neuron_HHuxley(char *theOperationMode)
{
	static const char	*funcName = "SetOperationMode_Neuron_HHuxley";
	int		specifier;

	if (hHuxleyNCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theOperationMode,
	  hHuxleyNCPtr->operationModeList)) == HHUXLEYNC_OPERATION_NULL) {
		NotifyError("%s: Illegal operationMode name (%s).", funcName,
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
	static const char	*funcName = "SetExcitatoryReversalPot_Neuron_HHuxley";

	if (hHuxleyNCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
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
	static const char	*funcName = "SetInhibitoryReversalPot_Neuron_HHuxley";

	if (hHuxleyNCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
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
	static const char	*funcName =
	  "SetShuntInhibitoryReversalPot_Neuron_HHuxley";

	if (hHuxleyNCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
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
	static const char	*funcName = "SetCellCapacitance_Neuron_HHuxley";

	if (hHuxleyNCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
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
	static const char	*funcName = "SetRestingPotential_Neuron_HHuxley";

	if (hHuxleyNCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
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
	static const char	*funcName = "SetRestingSignalDuration_Neuron_HHuxley";

	if (hHuxleyNCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
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
	static const char	*funcName = "SetRestingCriteria_Neuron_HHuxley";

	if (hHuxleyNCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
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
	static const char	*funcName = "CheckPars_Neuron_HHuxley";
	BOOLN	ok;

	ok = TRUE;
	if (hHuxleyNCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!hHuxleyNCPtr->iCList) {
		NotifyError("%s: iCList variable not set.", funcName);
		ok = FALSE;
	}
	if (!hHuxleyNCPtr->diagnosticModeFlag) {
		NotifyError("%s: diagnosticMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!hHuxleyNCPtr->operationModeFlag) {
		NotifyError("%s: operationMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!hHuxleyNCPtr->injectionModeFlag) {
		NotifyError("%s: injectionMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!hHuxleyNCPtr->excitatoryReversalPotFlag) {
		NotifyError("%s: excitatoryReversalPot variable not set.", funcName);
		ok = FALSE;
	}
	if (!hHuxleyNCPtr->inhibitoryReversalPotFlag) {
		NotifyError("%s: inhibitoryReversalPot variable not set.", funcName);
		ok = FALSE;
	}
	if (!hHuxleyNCPtr->shuntInhibitoryReversalPotFlag) {
		NotifyError("%s: shuntInhibitoryReversalPot variable not set.",
		  funcName);
		ok = FALSE;
	}
	if (!hHuxleyNCPtr->cellCapacitanceFlag) {
		NotifyError("%s: cellCapacitance variable not set.", funcName);
		ok = FALSE;
	}
	if (!hHuxleyNCPtr->restingPotentialFlag) {
		NotifyError("%s: restingPotential variable not set.", funcName);
		ok = FALSE;
	}
	if (!hHuxleyNCPtr->restingSignalDurationFlag) {
		NotifyError("%s: restingSignalDuration variable not set.", funcName);
		ok = FALSE;
	}
	if (!hHuxleyNCPtr->restingCriteriaFlag) {
		NotifyError("%s: restingCriteria variable not set.", funcName);
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
	static const char *funcName = "GetICListPtr_Neuron_HHuxley";

	if (hHuxleyNCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (hHuxleyNCPtr->iCList == NULL) {
		NotifyError("%s: Ion Channel list data structure has not been "
		  "correctly set.  NULL returned.", funcName);
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
	static const char	*funcName = "PrintPars_Neuron_HHuxley";

	if (!CheckPars_Neuron_HHuxley()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Hodgkin-Huxley Neural Cell Module Parameters:-\n");
	PrintPars_IonChanList(hHuxleyNCPtr->iCList);
	DPrint("\tDiagnostics mode: %s,", hHuxleyNCPtr->diagnosticModeList[
	  hHuxleyNCPtr->diagnosticMode].name);
	DPrint("\tOperation mode: %s,",
	  hHuxleyNCPtr->operationModeList[hHuxleyNCPtr->operationMode].name);
	DPrint("\tInjection mode = %s,\n",
	  hHuxleyNCPtr->injectionModeList[hHuxleyNCPtr->injectionMode].name);
	DPrint("\tExcitatory reversal potential = %g (mV),\n",
	  MILLI(hHuxleyNCPtr->excitatoryReversalPot));
	DPrint("\tInhibitory reversal potential = %g (mV),\n",
	  MILLI(hHuxleyNCPtr->inhibitoryReversalPot));
	DPrint("\tShunt inhibitory reversal potential = %g (mV),\n",
	  MILLI(hHuxleyNCPtr->shuntInhibitoryReversalPot));
	DPrint("\tCell capacitance = %g (F),\n",
	  hHuxleyNCPtr->cellCapacitance);
	DPrint("\tRestingPotential = %g mV\n",
	  MILLI(hHuxleyNCPtr->restingPotential));
	DPrint("\tResting calibration signal duration = %g ms,\n",
	  MSEC(hHuxleyNCPtr->restingSignalDuration));
	DPrint("\tResting criteria, dV = %g mV.\n",
	  MILLI(hHuxleyNCPtr->restingCriteria));
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_Neuron_HHuxley(char *fileName)
{
	static const char	*funcName = "ReadPars_Neuron_HHuxley";
	BOOLN	ok;
	char	*filePath;
	IonChanListPtr	iCList;
	char	diagnosticMode[MAXLINE], operationMode[MAXLINE];
	char	injectionMode[MAXLINE];
	double	excitatoryReversalPot, inhibitoryReversalPot, cellCapacitance;
	double	restingPotential, restingSignalDuration, restingCriteria;
	double	shuntInhibitoryReversalPot;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(filePath, "r")) == NULL) {
		NotifyError("%s: Cannot open data file '%s'.\n", funcName, filePath);
		return(FALSE);
	}
	DPrint("%s: Reading from '%s':\n", funcName, filePath);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, "%s", diagnosticMode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%s", operationMode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%s", injectionMode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &excitatoryReversalPot))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &inhibitoryReversalPot))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &shuntInhibitoryReversalPot))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &cellCapacitance))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &restingPotential))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &restingSignalDuration))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &restingCriteria))
		ok = FALSE;
	if ((iCList = ReadPars_IonChanList(fp)) == NULL)
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "
		  "parameter file '%s'.", funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_Neuron_HHuxley(iCList, diagnosticMode, operationMode,
	  injectionMode, excitatoryReversalPot, inhibitoryReversalPot,
	  shuntInhibitoryReversalPot, cellCapacitance, restingPotential,
	  restingSignalDuration, restingCriteria)) {
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
SetParsPointer_Neuron_HHuxley(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_Neuron_HHuxley";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
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
	static const char	*funcName = "InitModule_Neuron_HHuxley";

	if (!SetParsPointer_Neuron_HHuxley(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_Neuron_HHuxley(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = hHuxleyNCPtr;
	theModule->CheckPars = CheckPars_Neuron_HHuxley;
	theModule->Free = Free_Neuron_HHuxley;
	theModule->GetPotentialResponse = GetPotentialResponse_Neuron_HHuxley;
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
	static const char	*funcName = "CheckData_Neuron_HHuxley";
	int		i;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	for (i = 0; i < data->numInSignals; i++)
		if (!CheckInit_SignalData(data->inSignal[i], funcName)) {
			NotifyError("%s: input signal %d not initialised.", funcName, i);
			return(FALSE);
		}
	if ((hHuxleyNCPtr->injectionMode == HHUXLEYNC_INJECTION_OFF) &&
	  ((hHuxleyNCPtr->operationMode ==
	  HHUXLEYNC_OPERATION_VOLTAGE_CLAMP_MODE))) {
		NotifyError("%s: Voltage clamp mode must also use injection mode.",
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
 * Note that the "normal" operation mode must be used to calculate the
 * resting potential.
 */

double
FindRestingPotential_Neuron_HHuxley(double restingCriteria, double duration,
  double dt)
{
	static const char	*funcName = "FindRestingPotential_Neuron_HHuxley";
	int		i, oldOperationMode, oldInjectionMode;
	double	deltaV, newPotential;
	EarObjectPtr	stimulus = NULL, findResting = NULL;
	
	hHuxleyNCPtr->restingRun = TRUE;
	if ((stimulus = Init_EarObject("null")) == NULL) {
		NotifyError("%s: Out of memory for stimulus EarObjectPtr.", funcName);
		exit(1);
	}
	if ((findResting = Init_EarObject("null")) == NULL) {
		NotifyError("%s: Out of memory for findResing EarObjectPtr.", funcName);
		exit(1);
	}
	if (!InitOutSignal_EarObject(stimulus, 1, (ChanLen) (duration / dt), dt)) {
		NotifyError("%s: Could not initialise stimulus output signal.",
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
		NotifyWarning("%s: Failed to satisfy resting criteria\nafter %d loops.",
		  funcName, i);
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
	static const char *funcName = "InitProcessVariables_Neuron_HHuxley";
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
		p->numChannels = data->outSignal->numChannels;
		if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
			FreeProcessVariables_Neuron_HHuxley();
			OpenDiagnostics_NSpecLists(&hHuxleyNCPtr->fp,
			  hHuxleyNCPtr->diagnosticModeList, hHuxleyNCPtr->diagnosticMode);
			if ((p->state = (HHuxleyState *) calloc(p->numChannels,
			   sizeof(HHuxleyState))) == NULL) {
			 	NotifyError("%s: Out of memory.", funcName);
			 	return(FALSE);
			}
			for (i = 0; i < p->numChannels; i++) {
				if ((p->state[i].y = (double *) calloc(p->iCList->numChannels,
				  sizeof(double))) == NULL) {
					NotifyError("%s: Out of memory for state->y array",
					  funcName);
					ok = FALSE;
				}
				if ((p->state[i].z = (double *) calloc(p->iCList->numChannels,
				  sizeof(double))) == NULL) {
					NotifyError("%s: Out of memory for state->z array",
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
				  data->outSignal->dt);
			}
			p->updateProcessVariablesFlag = FALSE;
		}
		for (i = 0; i < p->numChannels; i++) {
			p->state[i].potential_V = p->restingPotential;
			yPtr = p->state[i].y;
			zPtr = p->state[i].z;
			for (node = p->iCList->ionChannels; node; node = node->next) {
				iC = (IonChannelPtr) node->data;
				if (!iC->enabled)
					continue;
				if ((e = GetTableEntry_IonChanList(iC, p->restingPotential)) ==
				  NULL) {
					NotifyError("%s: Could not find entry.", funcName);
					return(FALSE);
				}
				*yPtr++ = e->yY;
				*zPtr++ = e->zZ;
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

double
GetPotentialResponse_Neuron_HHuxley(double potential)
{
	static const char *funcName = "GetPotentialResponse_Neuron_HHuxley";

	if (!CheckPars_Neuron_HHuxley()) {
		NotifyError("%s: Parameters have not been correctly set, zero "\
		  "returned.", funcName);
		return(0.0);
	}
	return(potential);

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
	static const char	*funcName = "RunModel_Neuron_HHuxley";
	register	ChanData	*gExPtr, *gInPtr, *gSInPtr, *injPtr, *outPtr;
	register	double		deltaY, deltaZ;
	BOOLN	debug;
	int		i, inSignal;
	double	*yPtr, *zPtr, dt, dtOverC, ionChanCurrentSum, currentSum;
	double	activation, conductance;
	ChanLen	j;
	DynaListPtr		node;
	HHuxleyNCPtr	c;
	HHuxleyStatePtr	s;
	IonChannelPtr	iC;
	ICTableEntryPtr	e;

	if (!CheckPars_Neuron_HHuxley())
		return(FALSE);
	if (!CheckData_Neuron_HHuxley(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels,
	  data->inSignal[0]->length, data->inSignal[0]->dt)) {
		NotifyError("%s: Could not initialise output signal.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "Hodkin-Huxley neural cell");
	if (!InitProcessVariables_Neuron_HHuxley(data)) {
		NotifyError("%s: Could not initialise the process variables.",
		  funcName);
		return(FALSE);
	}
	c = hHuxleyNCPtr;
	dt = data->outSignal->dt;
	dtOverC = dt / c->cellCapacitance;
	debug = !c->restingRun && (c->diagnosticMode !=
	  GENERAL_DIAGNOSTIC_OFF_MODE);

	if (debug) {
		DynaListPtr	node;

		fprintf(c->fp, "Time(s)\tVm (mV)\tIleak (nA)");
		for (node = c->iCList->ionChannels; node; node = node->next) {
			iC = (IonChannelPtr) node->data;
			if (!iC->enabled)
				continue;
			fprintf(c->fp, "\tG(%s)\tI(%s)", iC->description, iC->description);
		}
		fprintf(c->fp, "\n");
	}

	for (i = 0; i < data->outSignal->numChannels; i++) {
		s = &c->state[i];
		inSignal = 0;
		injPtr = (c->injectionMode == HHUXLEYNC_INJECTION_OFF)?
		  (ChanData *) NULL: data->inSignal[inSignal++]->channel[i];
		gExPtr = (inSignal < data->numInSignals)?
		  data->inSignal[inSignal++]->channel[i]: (ChanData *) NULL;
		gInPtr = (inSignal < data->numInSignals)?
		  data->inSignal[inSignal++]->channel[i]: (ChanData *) NULL;
		gSInPtr = (inSignal < data->numInSignals)?
		  data->inSignal[inSignal++]->channel[i]: (ChanData *) NULL;
		outPtr = data->outSignal->channel[i];
		switch (c->operationMode) {
		case HHUXLEYNC_OPERATION_NORMAL_MODE:
			for (j = 0; j < data->outSignal->length; j++, outPtr++) {
				yPtr = s->y;
				zPtr = s->z;
				if (debug)
				  	fprintf(c->fp, "%g\t%g\t%g",  MILLI(j * dt),
					  MILLI(s->potential_V), NANO(c->iCList->leakageCond *
					  (s->potential_V - c->iCList->leakagePot)));
				
				for (node = c->iCList->ionChannels, ionChanCurrentSum = 0.0;
				  node; node = node->next, yPtr++, zPtr++) {
					iC = (IonChannelPtr) node->data;
					if (!iC->enabled)
						continue;
					activation = iC->PowFunc(*yPtr, iC->activationExponent);
					if (debug) {
						conductance = iC->maxConductance * activation * *zPtr;
				  		fprintf(c->fp, "\t%g\t%g", NANO(conductance),
						  NANO(conductance * (s->potential_V -
						  iC->equilibriumPot)));
				  	}
					ionChanCurrentSum += iC->maxConductance * activation *
					  *zPtr * (s->potential_V - iC->equilibriumPot);
				}
				if (debug)
					fprintf(c->fp, "\n");
				currentSum = ionChanCurrentSum + c->iCList->leakageCond *
				  (s->potential_V - c->iCList->leakagePot);
				if (injPtr)
					currentSum -= *injPtr++;
				if (gExPtr)
					currentSum += *gExPtr++ * (s->potential_V -
					  c->excitatoryReversalPot);
				if (gInPtr)
					currentSum += *gInPtr++ * (s->potential_V -
					  c->inhibitoryReversalPot);
				if (gSInPtr)
					currentSum += *gSInPtr++ * (s->potential_V -
					  c->shuntInhibitoryReversalPot);
				*outPtr = s->potential_V - currentSum * dtOverC;
				yPtr = s->y;
				zPtr = s->z;
				for (node = c->iCList->ionChannels; node; node = node->next) {
					iC = (IonChannelPtr) node->data;
					if (!iC->enabled)
						continue;
					if ((e = GetTableEntry_IonChanList(iC, *outPtr)) == NULL) {
						NotifyError("%s: Could not find entry.", funcName);
						return(FALSE);
					}
					deltaY = (e->yY - *yPtr) * dt / e->ty;
					deltaZ = (e->zZ - *zPtr) * dt / e->tz;
					*yPtr++ += deltaY;
					*zPtr++ += deltaZ;
				}
				s->potential_V = *outPtr;
			}
			break;
		case HHUXLEYNC_OPERATION_VOLTAGE_CLAMP_MODE:
			for (j = 0; j < data->outSignal->length; j++, outPtr++, injPtr++) {
				if (debug)
					fprintf(c->fp, "%g\t%g\t%g", MILLI(j * dt), MILLI(*injPtr),
					  NANO(c->iCList->leakageCond * (*injPtr -
					  c->iCList->leakagePot)));
				yPtr = s->y;
				zPtr = s->z;
				for (node = c->iCList->ionChannels, ionChanCurrentSum = 0.0;
				  node; node = node->next, yPtr++, zPtr++) {
					iC = (IonChannelPtr) node->data;
					if (!iC->enabled)
						continue;
					activation = iC->PowFunc(*yPtr, iC->activationExponent);
					if (debug) {
						conductance = iC->maxConductance * activation * *zPtr;
				  		fprintf(c->fp, "\t%g\t%g", NANO(conductance),
						  NANO(conductance * (*injPtr - iC->equilibriumPot)));
				  	}
					ionChanCurrentSum += iC->maxConductance *
					  activation * *zPtr * (*injPtr - iC->equilibriumPot);
				}
				if (debug)
					fprintf(c->fp, "\n");
				*outPtr = -ionChanCurrentSum - c->iCList->leakageCond *
				  (*injPtr - c->iCList->leakagePot);
				if (gExPtr)
					*outPtr -= *gExPtr++ * (*injPtr - c->excitatoryReversalPot);
				if (gInPtr)
					*outPtr -= *gInPtr++ * (*injPtr - c->inhibitoryReversalPot);
				if (gSInPtr)
					*outPtr -= *gSInPtr++ * (*injPtr - c->
					  shuntInhibitoryReversalPot);
				yPtr = s->y;
				zPtr = s->z;
				for (node = c->iCList->ionChannels; node; node = node->next) {
					iC = (IonChannelPtr) node->data;
					if (!iC->enabled)
						continue;
					if ((e = GetTableEntry_IonChanList(iC, *injPtr)) == NULL) {
						NotifyError("%s: Could not find entry.", funcName);
						return(FALSE);
					}
					deltaY = (e->yY - *yPtr) * dt / e->ty;
					deltaZ = (e->zZ - *zPtr) * dt / e->tz;
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

