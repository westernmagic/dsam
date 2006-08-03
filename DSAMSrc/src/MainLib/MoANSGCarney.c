/**********************
 *
 * File:		MoANSGCarney.c
 * Purpose:		Laurel H. Carney auditory nerve spike generation module:
 *				Carney L. H. (1993) "A model for the responses of low-frequency
 *				auditory-nerve fibers in cat", JASA, 93, pp 401-417.
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 *				24-02-97 LPO: Amended to allow multiple fibres.
 * Authors:		L. P. O'Mard modified from L. H. Carney's code
 * Created:		7 May 1996
 * Updated:		24 Feb 1997
 * Copyright:	(c) 1997,  University of Essex
 *
 *********************/

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
#include "UtRandom.h"
#include "MoANSGCarney.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

CarneySGPtr	carneySGPtr = NULL;

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
Free_ANSpikeGen_Carney(void)
{
	if (carneySGPtr == NULL)
		return(FALSE);
	FreeProcessVariables_ANSpikeGen_Carney();
	if (carneySGPtr->parList)
		FreeList_UniParMgr(&carneySGPtr->parList);
	if (carneySGPtr->parSpec == GLOBAL) {
		free(carneySGPtr);
		carneySGPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitInputModeList *****************************/

/*
 * This function initialises the 'inputMode' list array
 */

BOOLN
InitInputModeList_ANSpikeGen_Carney(void)
{
	static NameSpecifier	modeList[] = {

			{ wxT("CORRECTED"),	ANSPIKEGEN_CARNEY_INPUTMODE_CORRECTED },
			{ wxT("ORIGINAL"),	ANSPIKEGEN_CARNEY_INPUTMODE_ORIGINAL },
			{ wxT(""),			ANSPIKEGEN_CARNEY_INPUTMODE_NULL },
		};
	carneySGPtr->inputModeList = modeList;
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
Init_ANSpikeGen_Carney(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_ANSpikeGen_Carney");

	if (parSpec == GLOBAL) {
		if (carneySGPtr != NULL)
			Free_ANSpikeGen_Carney();
		if ((carneySGPtr = (CarneySGPtr) malloc(sizeof(CarneySG))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (carneySGPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	carneySGPtr->parSpec = parSpec;
	carneySGPtr->updateProcessVariablesFlag = TRUE;
	carneySGPtr->inputModeFlag = TRUE;
	carneySGPtr->ranSeedFlag = TRUE;
	carneySGPtr->numFibresFlag = TRUE;
	carneySGPtr->pulseDurationFlag = TRUE;
	carneySGPtr->pulseMagnitudeFlag = TRUE;
	carneySGPtr->refractoryPeriodFlag = TRUE;
	carneySGPtr->maxThresholdFlag = TRUE;
	carneySGPtr->dischargeCoeffC0Flag = TRUE;
	carneySGPtr->dischargeCoeffC1Flag = TRUE;
	carneySGPtr->dischargeTConstS0Flag = TRUE;
	carneySGPtr->dischargeTConstS1Flag = TRUE;
	carneySGPtr->inputMode = ANSPIKEGEN_CARNEY_INPUTMODE_ORIGINAL;
	carneySGPtr->ranSeed = -1;
	carneySGPtr->numFibres = 1;
	carneySGPtr->pulseDuration = -1.0;
	carneySGPtr->pulseMagnitude = 1.0;
	carneySGPtr->refractoryPeriod = 0.75e-3;
	carneySGPtr->maxThreshold = 1.0;
	carneySGPtr->dischargeCoeffC0 = 0.5;
	carneySGPtr->dischargeCoeffC1 = 0.5;
	carneySGPtr->dischargeTConstS0 = 1e-3;
	carneySGPtr->dischargeTConstS1 = 12.5e-3;

	InitInputModeList_ANSpikeGen_Carney();
	if (!SetUniParList_ANSpikeGen_Carney()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_ANSpikeGen_Carney();
		return(FALSE);
	}
	carneySGPtr->timer = NULL;
	carneySGPtr->remainingPulseTime = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_ANSpikeGen_Carney(void)
{
	static const WChar *funcName = wxT("SetUniParList_ANSpikeGen_Carney");
	UniParPtr	pars;

	if ((carneySGPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  ANSPIKEGEN_CARNEY_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = carneySGPtr->parList->pars;
	SetPar_UniParMgr(&pars[ANSPIKEGEN_CARNEY_INPUTMODE], wxT("INPUT_MODE"),
	  wxT("Input mode, 'corrected' (2001), or 'original' (1993) setting."),
	  UNIPAR_NAME_SPEC,
	  &carneySGPtr->inputMode, carneySGPtr->inputModeList,
	  (void * (*)) SetInputMode_ANSpikeGen_Carney);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_CARNEY_RANSEED], wxT("RAN_SEED"),
	  wxT("Random number seed (0 produces a different seed each run)."),
	  UNIPAR_LONG,
	  &carneySGPtr->ranSeed, NULL,
	  (void * (*)) SetRanSeed_ANSpikeGen_Carney);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_CARNEY_NUMFIBRES], wxT("NUM_FIBRES"),
	  wxT("Number of fibres."),
	  UNIPAR_INT,
	  &carneySGPtr->numFibres, NULL,
	  (void * (*)) SetNumFibres_ANSpikeGen_Carney);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_CARNEY_PULSEDURATION], wxT(
	  "PULSE_DURATION"),
	  wxT("Excitary post-synaptic pulse duration (s)."),
	  UNIPAR_REAL,
	  &carneySGPtr->pulseDuration, NULL,
	  (void * (*)) SetPulseDuration_ANSpikeGen_Carney);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_CARNEY_PULSEMAGNITUDE], wxT("MAGNITUDE"),
	  wxT("Pulse magnitude (arbitrary units)."),
	  UNIPAR_REAL,
	  &carneySGPtr->pulseMagnitude, NULL,
	  (void * (*)) SetPulseMagnitude_ANSpikeGen_Carney);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_CARNEY_REFRACTORYPERIOD], wxT(
	  "REFRAC_PERIOD"),
	  wxT("Refractory period (s)."),
	  UNIPAR_REAL,
	  &carneySGPtr->refractoryPeriod, NULL,
	  (void * (*)) SetRefractoryPeriod_ANSpikeGen_Carney);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_CARNEY_MAXTHRESHOLD], wxT(
	  "THRESHOLD_INC"),
	  wxT("Maximum threshold increase following discharge, Hmax (spikes/s)."),
	  UNIPAR_REAL,
	  &carneySGPtr->maxThreshold, NULL,
	  (void * (*)) SetMaxThreshold_ANSpikeGen_Carney);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_CARNEY_DISCHARGECOEFFC0], wxT("C0"),
	  wxT("Coefficient for discharge history effect, c0 (s)."),
	  UNIPAR_REAL,
	  &carneySGPtr->dischargeCoeffC0, NULL,
	  (void * (*)) SetDischargeCoeffC0_ANSpikeGen_Carney);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_CARNEY_DISCHARGECOEFFC1], wxT("C1"),
	  wxT("Coefficient for discharge history effect, c1 (s)."),
	  UNIPAR_REAL,
	  &carneySGPtr->dischargeCoeffC1, NULL,
	  (void * (*)) SetDischargeCoeffC1_ANSpikeGen_Carney);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_CARNEY_DISCHARGETCONSTS0], wxT("S0"),
	  wxT("Time constant for discharge history effect, s0 (s)."),
	  UNIPAR_REAL,
	  &carneySGPtr->dischargeTConstS0, NULL,
	  (void * (*)) SetDischargeTConstS0_ANSpikeGen_Carney);
	SetPar_UniParMgr(&pars[ANSPIKEGEN_CARNEY_DISCHARGETCONSTS1], wxT("S1"),
	  wxT("Time constant for discharge history effect, s1 (s)."),
	  UNIPAR_REAL,
	  &carneySGPtr->dischargeTConstS1, NULL,
	  (void * (*)) SetDischargeTConstS1_ANSpikeGen_Carney);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_ANSpikeGen_Carney(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_ANSpikeGen_Carney");

	if (carneySGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (carneySGPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been "
		  "initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(carneySGPtr->parList);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_ANSpikeGen_Carney(long ranSeed, int numFibres, double pulseDuration,
  double pulseMagnitude, double refractoryPeriod, double maxThreshold,
  double dischargeCoeffC0, double dischargeCoeffC1, double dischargeTConstS0,
  double dischargeTConstS1)
{
	static const WChar	*funcName = wxT("SetPars_ANSpikeGen_Carney");
	BOOLN	ok;

	ok = TRUE;
	if (!SetRanSeed_ANSpikeGen_Carney(ranSeed))
		ok = FALSE;
	if (!SetNumFibres_ANSpikeGen_Carney(numFibres))
		ok = FALSE;
	if (!SetPulseDuration_ANSpikeGen_Carney(pulseDuration))
		ok = FALSE;
	if (!SetPulseMagnitude_ANSpikeGen_Carney(pulseMagnitude))
		ok = FALSE;
	if (!SetRefractoryPeriod_ANSpikeGen_Carney(refractoryPeriod))
		ok = FALSE;
	if (!SetMaxThreshold_ANSpikeGen_Carney(maxThreshold))
		ok = FALSE;
	if (!SetDischargeCoeffC0_ANSpikeGen_Carney(dischargeCoeffC0))
		ok = FALSE;
	if (!SetDischargeCoeffC1_ANSpikeGen_Carney(dischargeCoeffC1))
		ok = FALSE;
	if (!SetDischargeTConstS0_ANSpikeGen_Carney(dischargeTConstS0))
		ok = FALSE;
	if (!SetDischargeTConstS1_ANSpikeGen_Carney(dischargeTConstS1))
		ok = FALSE;
	if (!ok)
		NotifyError(wxT("%s: Failed to set all module parameters.") ,funcName);
	return(ok);

}

/****************************** SetInputMode **********************************/

/*
 * This function sets the module's inputMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetInputMode_ANSpikeGen_Carney(WChar * theInputMode)
{
	static const WChar	*funcName = wxT("SetInputMode_ANSpikeGen_Carney");
	int		specifier;

	if (carneySGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theInputMode,
		carneySGPtr->inputModeList)) == ANSPIKEGEN_CARNEY_INPUTMODE_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theInputMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneySGPtr->inputModeFlag = TRUE;
	carneySGPtr->inputMode = specifier;
	return(TRUE);

}

/****************************** SetRanSeed ************************************/

/*
 * This function sets the module's ranSeed parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRanSeed_ANSpikeGen_Carney(long theRanSeed)
{
	static const WChar	*funcName = wxT("SetRanSeed_ANSpikeGen_Carney");

	if (carneySGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	carneySGPtr->ranSeedFlag = TRUE;
	carneySGPtr->updateProcessVariablesFlag = TRUE;
	carneySGPtr->ranSeed = theRanSeed;
	return(TRUE);

}

/****************************** SetNumFibres **********************************/

/*
 * This function sets the module's numFibres parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNumFibres_ANSpikeGen_Carney(int theNumFibres)
{
	static const WChar	*funcName = wxT("SetNumFibres_ANSpikeGen_Carney");

	if (carneySGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theNumFibres < 1) {
		NotifyError(wxT("%s: Illegal no. of fibres (%d)."), funcName,
		  theNumFibres);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneySGPtr->numFibresFlag = TRUE;
	carneySGPtr->numFibres = theNumFibres;
	return(TRUE);

}

/****************************** SetPulseDuration ******************************/

/*
 * This function sets the module's pulseDuration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPulseDuration_ANSpikeGen_Carney(double thePulseDuration)
{
	static const WChar	*funcName = wxT("SetPulseDuration_ANSpikeGen_Carney");

	if (carneySGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	carneySGPtr->pulseDurationFlag = TRUE;
	carneySGPtr->pulseDuration = thePulseDuration;
	return(TRUE);

}

/****************************** SetPulseMagnitude *****************************/

/*
 * This function sets the module's pulseMagnitude parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPulseMagnitude_ANSpikeGen_Carney(double thePulseMagnitude)
{
	static const WChar	*funcName = wxT("SetPulseMagnitude_ANSpikeGen_Carney");

	if (carneySGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneySGPtr->pulseMagnitudeFlag = TRUE;
	carneySGPtr->pulseMagnitude = thePulseMagnitude;
	return(TRUE);

}

/****************************** SetRefractoryPeriod ***************************/

/*
 * This function sets the module's refractoryPeriod parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRefractoryPeriod_ANSpikeGen_Carney(double theRefractoryPeriod)
{
	static const WChar	*funcName = wxT(
	  "SetRefractoryPeriod_ANSpikeGen_Carney");

	if (carneySGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theRefractoryPeriod < 0.0) {
		NotifyError(wxT("%s: Refractory period must be greater than zero.\n"),
		  funcName);
		return(FALSE);
	}
	carneySGPtr->refractoryPeriodFlag = TRUE;
	carneySGPtr->refractoryPeriod = theRefractoryPeriod;
	return(TRUE);

}

/****************************** SetMaxThreshold *******************************/

/*
 * This function sets the module's maxThreshold parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMaxThreshold_ANSpikeGen_Carney(double theMaxThreshold)
{
	static const WChar	*funcName = wxT("SetMaxThreshold_ANSpikeGen_Carney");

	if (carneySGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneySGPtr->maxThresholdFlag = TRUE;
	carneySGPtr->maxThreshold = theMaxThreshold;
	return(TRUE);

}

/****************************** SetDischargeCoeffC0 ***************************/

/*
 * This function sets the module's dischargeCoeffC0 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDischargeCoeffC0_ANSpikeGen_Carney(double theDischargeCoeffC0)
{
	static const WChar	*funcName = wxT(
	  "SetDischargeCoeffC0_ANSpikeGen_Carney");

	if (carneySGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneySGPtr->dischargeCoeffC0Flag = TRUE;
	carneySGPtr->dischargeCoeffC0 = theDischargeCoeffC0;
	return(TRUE);

}

/****************************** SetDischargeCoeffC1 ***************************/

/*
 * This function sets the module's dischargeCoeffC1 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDischargeCoeffC1_ANSpikeGen_Carney(double theDischargeCoeffC1)
{
	static const WChar	*funcName = wxT(
	  "SetDischargeCoeffC1_ANSpikeGen_Carney");

	if (carneySGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneySGPtr->dischargeCoeffC1Flag = TRUE;
	carneySGPtr->dischargeCoeffC1 = theDischargeCoeffC1;
	return(TRUE);

}

/****************************** SetDischargeTConstS0 **************************/

/*
 * This function sets the module's dischargeTConstS0 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDischargeTConstS0_ANSpikeGen_Carney(double theDischargeTConstS0)
{
	static const WChar	*funcName = wxT(
	  "SetDischargeTConstS0_ANSpikeGen_Carney");

	if (carneySGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneySGPtr->dischargeTConstS0Flag = TRUE;
	carneySGPtr->dischargeTConstS0 = theDischargeTConstS0;
	return(TRUE);

}

/****************************** SetDischargeTConstS1 **************************/

/*
 * This function sets the module's dischargeTConstS1 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDischargeTConstS1_ANSpikeGen_Carney(double theDischargeTConstS1)
{
	static const WChar	*funcName = wxT(
	  "SetDischargeTConstS1_ANSpikeGen_Carney");

	if (carneySGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneySGPtr->dischargeTConstS1Flag = TRUE;
	carneySGPtr->dischargeTConstS1 = theDischargeTConstS1;
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
CheckPars_ANSpikeGen_Carney(void)
{
	static const WChar	*funcName = wxT("CheckPars_ANSpikeGen_Carney");
	BOOLN	ok;

	ok = TRUE;
	if (carneySGPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (!carneySGPtr->inputModeFlag) {
		NotifyError(wxT("%s: inputMode variable not set."), funcName);
		ok = FALSE;
	}
	if (!carneySGPtr->ranSeedFlag) {
		NotifyError(wxT("%s: ranSeed variable not set."), funcName);
		ok = FALSE;
	}
	if (!carneySGPtr->numFibresFlag) {
		NotifyError(wxT("%s: numFibres variable not set."), funcName);
		ok = FALSE;
	}
	if (!carneySGPtr->pulseDurationFlag) {
		NotifyError(wxT("%s: pulseDuration variable not set."), funcName);
		ok = FALSE;
	}
	if (!carneySGPtr->pulseMagnitudeFlag) {
		NotifyError(wxT("%s: pulseMagnitude variable not set."), funcName);
		ok = FALSE;
	}
	if (!carneySGPtr->refractoryPeriodFlag) {
		NotifyError(wxT("%s: refractoryPeriod variable not set."), funcName);
		ok = FALSE;
	}
	if (!carneySGPtr->maxThresholdFlag) {
		NotifyError(wxT("%s: maxThreshold variable not set."), funcName);
		ok = FALSE;
	}
	if (!carneySGPtr->dischargeCoeffC0Flag) {
		NotifyError(wxT("%s: dischargeCoeffC0 variable not set."), funcName);
		ok = FALSE;
	}
	if (!carneySGPtr->dischargeCoeffC1Flag) {
		NotifyError(wxT("%s: dischargeCoeffC1 variable not set."), funcName);
		ok = FALSE;
	}
	if (!carneySGPtr->dischargeTConstS0Flag) {
		NotifyError(wxT("%s: dischargeTConstS0 variable not set."), funcName);
		ok = FALSE;
	}
	if (!carneySGPtr->dischargeTConstS1Flag) {
		NotifyError(wxT("%s: dischargeTConstS1 variable not set."), funcName);
		ok = FALSE;
	}
	if ((carneySGPtr->pulseDuration > 0.0) && (carneySGPtr->pulseDuration >=
	  carneySGPtr->refractoryPeriod)) {
		NotifyError(wxT("%s: Pulse duration must be smaller than the\n"
		  "refractory period, %g ms (%g ms)."), funcName, MSEC(
		  carneySGPtr->refractoryPeriod), MSEC(carneySGPtr->pulseDuration));
		ok = FALSE;
	}
	return(ok);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the dSAM.parsFile file pointer.
 */

BOOLN
PrintPars_ANSpikeGen_Carney(void)
{
	static const WChar	*funcName = wxT("PrintPars_ANSpikeGen_Carney");

	if (!CheckPars_ANSpikeGen_Carney()) {
		NotifyError(wxT("%s: Parameters have not been correctly set."),
		  funcName);
		return(FALSE);
	}
	DPrint(wxT("Carney Post-Synaptic Firing Module Parameters:-\n"));
	DPrint(wxT("\tInput mode: %s,\n"), carneySGPtr->inputModeList[carneySGPtr->
	  inputMode].name);
	DPrint(wxT("\tRandom number seed = %ld,"),
	  carneySGPtr->ranSeed);
	DPrint(wxT("\tNumber of fibres = %d,\n"), carneySGPtr->numFibres);
	DPrint(wxT("\tPulse duration = "));
	if (carneySGPtr->pulseDuration > 0.0)
		DPrint(wxT("%g ms,"), MSEC(carneySGPtr->pulseDuration));
	else
		DPrint(wxT("< prev. signal dt>,"));
	DPrint(wxT("\tPulse magnitude = %g (nA?)\n"), carneySGPtr->pulseMagnitude);
	DPrint(wxT("\tRefractory period, Ra = %g ms\n"), MSEC(
	  carneySGPtr->refractoryPeriod));
	DPrint(wxT("\tMax. threshold increase, Hmax = %g (spikes/s)\n"),
	  carneySGPtr->maxThreshold);
	DPrint(wxT("\tDischarge coefficent, C0 = %g,"), carneySGPtr->
	  dischargeCoeffC0);
	DPrint(wxT("\tDischarge coefficent, C1 = %g\n"), carneySGPtr->
	  dischargeCoeffC1);
	DPrint(wxT("\tDischarge time constant, S0 = %g ms,\n"), MSEC(
	  carneySGPtr->dischargeTConstS0));
	DPrint(wxT("\tDischarge time constant, S1 = %g ms.\n"), MSEC(
	  carneySGPtr->dischargeTConstS1));
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_ANSpikeGen_Carney(WChar *fileName)
{
	static const WChar	*funcName = wxT("ReadPars_ANSpikeGen_Carney");
	BOOLN	ok;
	WChar	*filePath;
	int		numFibres;
	long	ranSeed;
	double	pulseDuration, pulseMagnitude, refractoryPeriod, maxThreshold;
	double	dischargeCoeffC0, dischargeCoeffC1, dischargeTConstS0;
	double	dischargeTConstS1;
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
	if (!GetPars_ParFile(fp, wxT("%ld"), &ranSeed))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%d"), &numFibres))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &pulseDuration))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &pulseMagnitude))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &refractoryPeriod))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &maxThreshold))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &dischargeCoeffC0))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &dischargeCoeffC1))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &dischargeTConstS0))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &dischargeTConstS1))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError(wxT("%s: Not enough lines, or invalid parameters, in "
		  "module parameter file '%s'."), funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_ANSpikeGen_Carney(ranSeed, numFibres, pulseDuration,
	  pulseMagnitude, refractoryPeriod, maxThreshold, dischargeCoeffC0,
	  dischargeCoeffC1, dischargeTConstS0, dischargeTConstS1)) {
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
SetParsPointer_ANSpikeGen_Carney(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_ANSpikeGen_Carney");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	carneySGPtr = (CarneySGPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_ANSpikeGen_Carney(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_ANSpikeGen_Carney");

	if (!SetParsPointer_ANSpikeGen_Carney(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_ANSpikeGen_Carney(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = carneySGPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->CheckPars = CheckPars_ANSpikeGen_Carney;
	theModule->Free = Free_ANSpikeGen_Carney;
	theModule->GetUniParListPtr = GetUniParListPtr_ANSpikeGen_Carney;
	theModule->PrintPars = PrintPars_ANSpikeGen_Carney;
	theModule->ReadPars = ReadPars_ANSpikeGen_Carney;
	theModule->ResetProcess = ResetProcess_ANSpikeGen_Carney;
	theModule->RunProcess = RunModel_ANSpikeGen_Carney;
	theModule->SetParsPointer = SetParsPointer_ANSpikeGen_Carney;
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
CheckData_ANSpikeGen_Carney(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_ANSpikeGen_Carney");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	if ((carneySGPtr->pulseDuration > 0.0) && (carneySGPtr->pulseDuration <
	  _InSig_EarObject(data, 0)->dt)) {
		NotifyError(wxT("%s: Pulse duration is too small for sampling\n"
		  "interval, %g ms (%g ms)\n"), funcName,
		  MSEC(_InSig_EarObject(data, 0)->dt), MSEC(carneySGPtr->pulseDuration));
		return(FALSE);
	}
	/*** Put additional checks here. ***/
	return(TRUE);

}

/**************************** ResetProcessThread ******************************/

/*
 * This routine resets the process thread-related variables.
 */

void
ResetProcessThread_ANSpikeGen_Carney(EarObjectPtr data,
  double timeGreaterThanRefractoryPeriod, int i)
{
	int		j;
	double	*timerPtr, *remainingPulseTimePtr;
	CarneySGPtr	p = carneySGPtr;

	timerPtr = p->timer[i];
	remainingPulseTimePtr = p->remainingPulseTime[i];
	for (j = 0; j < p->arrayLength; j++) {
		*timerPtr++ = timeGreaterThanRefractoryPeriod;
		*remainingPulseTimePtr++ = 0.0;
	}

}

/**************************** ResetProcess ************************************/

/*
 * This routine resets the process variables.
 */

void
ResetProcess_ANSpikeGen_Carney(EarObjectPtr data)
{
	int		i;
	double	timeGreaterThanRefractoryPeriod;
	CarneySGPtr	p = carneySGPtr;

	ResetOutSignal_EarObject(data);
	timeGreaterThanRefractoryPeriod = p->refractoryPeriod + data->
	  outSignal->dt;
	if (data->threadRunFlag)
		ResetProcessThread_ANSpikeGen_Carney(data,
		  timeGreaterThanRefractoryPeriod, data->threadIndex);
	else  {
		for (i = 0; i < data->numThreads; i++) {
			ResetProcessThread_ANSpikeGen_Carney(data,
			  timeGreaterThanRefractoryPeriod, i);
		}
	}
}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 */

BOOLN
InitProcessVariables_ANSpikeGen_Carney(EarObjectPtr data)
{
	static const WChar *funcName = wxT(
	  "InitProcessVariables_ANSpikeGen_Carney");
	int		i;
	CarneySGPtr	p = carneySGPtr;
	
	if (p->updateProcessVariablesFlag || data->updateProcessFlag || (data->
	  timeIndex == PROCESS_START_TIME)) {
		p->arrayLength = _OutSig_EarObject(data)->numChannels * p->numFibres;
		if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
			FreeProcessVariables_ANSpikeGen_Carney();
			if (!SetRandPars_EarObject(data, p->ranSeed, funcName))
				return(FALSE);
			p->numThreads = data->numThreads;
			if ((p->timer = (double **) calloc(p->numThreads, sizeof(
			  double*))) == NULL) {
			 	NotifyError(wxT("%s: Out of memory for timer pointer array."),
				  funcName);
			 	return(FALSE);
			}
			if ((p->remainingPulseTime = (double **) calloc(p->numThreads,
			  sizeof(double*))) == NULL) {
			 	NotifyError(wxT("%s: Out of memory for remainingPulseTime "
				  "pointer array."), funcName);
			 	return(FALSE);
			}
			for (i = 0; i < p->numThreads; i++) {
				if ((p->timer[i] = (double *) calloc(p->arrayLength, sizeof(
				  double))) == NULL) {
			 		NotifyError(wxT("%s: Out of memory for timer array."),
					  funcName);
			 		return(FALSE);
				}
				if ((p->remainingPulseTime[i] = (double *) calloc(p->
				  arrayLength, sizeof(double))) == NULL) {
			 		NotifyError(wxT("%s: Out of memory for remainingPulseTime "
					  "array."), funcName);
			 		return(FALSE);
				}
			}
			p->updateProcessVariablesFlag = FALSE;
		}
		ResetProcess_ANSpikeGen_Carney(data);
	}
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

void
FreeProcessVariables_ANSpikeGen_Carney(void)
{
	int		i;
	CarneySGPtr	p = carneySGPtr;

	if (p->timer) {
		for (i = 0; i < p->numThreads; i++)
			if (p->timer[i])
				free(p->timer[i]);
		free(p->timer);
		p->timer = NULL;
	}
	if (p->remainingPulseTime) {
		for (i = 0; i < p->numThreads; i++)
			if (p->remainingPulseTime[i])
				free(p->remainingPulseTime[i]);
		free(p->remainingPulseTime);
		p->remainingPulseTime = NULL;
	}
	p->updateProcessVariablesFlag = TRUE;

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
 * This code differs somewhat from Carney's because the 'timer' field records
 * the time from the last spike.
 */

BOOLN
RunModel_ANSpikeGen_Carney(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("RunModel_ANSpikeGen_Carney");
	register	ChanData	*inPtr, *outPtr;
	register	double		*timerPtr, *remainingPulseTimePtr;
	int		i, chan;
	double	threshold, excessTime;
	ChanLen	j;
	SignalDataPtr	outSignal;
	CarneySGPtr	p = carneySGPtr;
	
	if (!data->threadRunFlag) {
		if (!CheckPars_ANSpikeGen_Carney())
			return(FALSE);
		if (!CheckData_ANSpikeGen_Carney(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Carney Post-Synaptic Spike "
		  "Firing"));
		if (!InitOutSignal_EarObject(data, _InSig_EarObject(data, 0)->numChannels,
		  _InSig_EarObject(data, 0)->length, _InSig_EarObject(data, 0)->dt)) {
			NotifyError(wxT("%s: Could not initialise output signal."),
			  funcName);
			return(FALSE);
		}
		if (!InitProcessVariables_ANSpikeGen_Carney(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		p->dt = _InSig_EarObject(data, 0)->dt;
		p->wPulseDuration = (p->pulseDuration > 0.0)? p->pulseDuration: p->dt;
		for (chan = 0; chan < _OutSig_EarObject(data)->numChannels; chan++) {
			outPtr = _OutSig_EarObject(data)->channel[chan];
			for (j = 0; j < _OutSig_EarObject(data)->length; j++)
				*outPtr++ = 0.0;
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	outSignal = _OutSig_EarObject(data);
	for (i = 0, timerPtr = p->timer[data->threadIndex], remainingPulseTimePtr =
	  p->remainingPulseTime[data->threadIndex]; i < p->numFibres; i++)
		for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
			inPtr = _InSig_EarObject(data, 0)->channel[chan];
			outPtr = outSignal->channel[chan];
			for (j = 0; j < outSignal->length; j++) {
				if (*timerPtr > p->refractoryPeriod) {
					excessTime = *timerPtr - p->refractoryPeriod;
					threshold = p->maxThreshold * (p->dischargeCoeffC0 * 
					  exp(-excessTime / p->dischargeTConstS0) +
					  p->dischargeCoeffC1 * exp(-excessTime /
					  p->dischargeTConstS1));
					if (((((p->inputMode ==
					  ANSPIKEGEN_CARNEY_INPUTMODE_ORIGINAL)? *inPtr / p->dt:
					  *inPtr) - threshold) * p->dt) > Ran01_Random(data->
					  randPars)) {
						*remainingPulseTimePtr = p->wPulseDuration;
						*timerPtr = 0.0;
					}
				}
				if (*remainingPulseTimePtr > 0.0) {
					*outPtr += p->pulseMagnitude;
					*remainingPulseTimePtr -= p->dt;
				}
				*timerPtr += p->dt;
				inPtr++;
				outPtr++;
			}
			timerPtr++;
			remainingPulseTimePtr++;
		}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}
