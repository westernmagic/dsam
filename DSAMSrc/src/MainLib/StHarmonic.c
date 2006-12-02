/**********************
 *
 * File:		StHarmonic.c
 * Purpose:		This module contains the methods for the harmonic series
 *				stimulus.
 * Comments:	This was amended by Almudena to include FM and also a
 *				frequency domain butterworth filter options.
 *				16-07-98 LPO: Addit negative and postive Shroeder phase.
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		16 Jul 1998
 * Copyright:	(c) 1998,  University of Essex
 *
 **********************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <ctype.h>
#include <string.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "UtRandom.h"
#include "FiParFile.h"
#include "StHarmonic.h"
#include "UtString.h"
#include "UtFilters.h"

/********************************* Global variables ***************************/

HarmonicPtr	harmonicPtr = NULL;

/******************************************************************************/
/********************************* Subroutines and functions ******************/
/******************************************************************************/

/****************************** InitPhaseModeList *****************************/

/*
 * This routine intialises the Phase Mode list array.
 */

BOOLN
InitPhaseModeList_Harmonic(void)
{
	static NameSpecifier	modeList[] = {

					{ wxT("RANDOM"),			HARMONIC_RANDOM },
					{ wxT("SINE"), 				HARMONIC_SINE },
					{ wxT("COSINE"), 			HARMONIC_COSINE },
					{ wxT("ALTERNATING"),		HARMONIC_ALTERNATING },
					{ wxT("SCHROEDER"),			HARMONIC_SCHROEDER },
					{ wxT("PLACK_AND_WHITE"),	HARMONIC_PLACK_AND_WHITE },
					{ wxT("USER"),				HARMONIC_USER },
					{ NULL,						HARMONIC_NULL },
				};
	harmonicPtr->phaseModeList = modeList;
	return(TRUE);

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
Init_Harmonic(ParameterSpecifier parSpec)
{
	static const WChar *funcName = wxT("nit_Harmonic");

	if (parSpec == GLOBAL) {
		if (harmonicPtr != NULL)
			Free_Harmonic();
		if ((harmonicPtr = (HarmonicPtr) malloc(sizeof(Harmonic))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"), funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (harmonicPtr == NULL) { 
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	harmonicPtr->parSpec = parSpec;
	harmonicPtr->updateProcessVariablesFlag = TRUE;
	harmonicPtr->lowestHarmonicFlag = TRUE;
	harmonicPtr->highestHarmonicFlag = TRUE;
	harmonicPtr->mistunedHarmonicFlag = TRUE;
	harmonicPtr->phaseModeFlag = TRUE;
	harmonicPtr->phaseVariableFlag = TRUE;
	harmonicPtr->mistuningFactorFlag = TRUE;
	harmonicPtr->frequencyFlag = TRUE;
	harmonicPtr->intensityFlag = TRUE;
	harmonicPtr->durationFlag = TRUE;
	harmonicPtr->dtFlag = TRUE;
	harmonicPtr->modulationFrequencyFlag = TRUE;
	harmonicPtr->modulationPhaseFlag = TRUE;
	harmonicPtr->modulationDepthFlag = TRUE;
	harmonicPtr->orderFlag = TRUE;
	harmonicPtr->lowerCutOffFreqFlag = TRUE;
	harmonicPtr->upperCutOffFreqFlag = TRUE;
	harmonicPtr->intensityFlag = TRUE;
	harmonicPtr->lowestHarmonic = 1;
	harmonicPtr->highestHarmonic = 10;
	harmonicPtr->mistunedHarmonic = -1;
	harmonicPtr->phaseMode = HARMONIC_SINE;
	harmonicPtr->mistuningFactor = 40.0;
	harmonicPtr->phaseVariable = 1.0;
	harmonicPtr->frequency = 100.0;
	harmonicPtr->intensity = DEFAULT_INTENSITY;
	harmonicPtr->duration = 0.1;
	harmonicPtr->dt = DEFAULT_DT;
	harmonicPtr->modulationFrequency = 1.0;
	harmonicPtr->modulationPhase = 0.0;
	harmonicPtr->modulationDepth = 0.0;
	harmonicPtr->order = 0;
	harmonicPtr->lowerCutOffFreq = 200.0;
	harmonicPtr->upperCutOffFreq = 600.0;
	harmonicPtr->phase = NULL;
	harmonicPtr->modIndex = NULL;
	harmonicPtr->harmonicFrequency = NULL;

	InitPhaseModeList_Harmonic();
	if (!SetUniParList_Harmonic()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Harmonic();
		return(FALSE);
	}
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
Free_Harmonic(void)
{
	if (harmonicPtr == NULL)
		return(TRUE);
	FreeProcessVariables_Harmonic();
	if (harmonicPtr->parList)
		FreeList_UniParMgr(&harmonicPtr->parList);
	if (harmonicPtr->parSpec == GLOBAL) {
		free(harmonicPtr);
		harmonicPtr = NULL;
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
SetUniParList_Harmonic(void)
{
	static const WChar *funcName = wxT("SetUniParList_Harmonic");
	UniParPtr	pars;

	if ((harmonicPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  HARMONIC_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = harmonicPtr->parList->pars;
	SetPar_UniParMgr(&pars[HARMONIC_LOWESTHARMONIC], wxT("LOW_HARMONIC"),
	  wxT("Lowest harmonic number."),
	  UNIPAR_INT,
	  &harmonicPtr->lowestHarmonic, NULL,
	  (void * (*)) SetLowestHarmonic_Harmonic);
	SetPar_UniParMgr(&pars[HARMONIC_HIGHESTHARMONIC], wxT("HIGH_HARMONIC"),
	  wxT("Highest harmonic number."),
	  UNIPAR_INT,
	  &harmonicPtr->highestHarmonic, NULL,
	  (void * (*)) SetHighestHarmonic_Harmonic);
	SetPar_UniParMgr(&pars[HARMONIC_PHASEMODE], wxT("PHASE_MODE"),
	  wxT("Phase mode (ALTERNATING, COSINE, RANDOM, SCHROEDER, SINE, USER)."),
	  UNIPAR_NAME_SPEC,
	  &harmonicPtr->phaseMode, harmonicPtr->phaseModeList,
	  (void * (*)) SetPhaseMode_Harmonic);
	SetPar_UniParMgr(&pars[HARMONIC_PHASE_PAR], wxT("PHASE_PAR"),
	  wxT("Phase parameter (Shroeder phase: C value, Random: random number ")
	    wxT("seed)."),
	  UNIPAR_REAL,
	  &harmonicPtr->phaseVariable, NULL,
	  (void * (*)) SetPhaseVariable_Harmonic);
	SetPar_UniParMgr(&pars[HARMONIC_MISTUNEDHARMONIC], wxT("MISTUNED_HARM"),
	  wxT("Mistuned harmonic number (0 = F0, -ve implies none mistuned)."),
	  UNIPAR_INT,
	  &harmonicPtr->mistunedHarmonic, NULL,
	  (void * (*)) SetMistunedHarmonic_Harmonic);
	SetPar_UniParMgr(&pars[HARMONIC_MISTUNINGFACTOR], wxT("MT_FACTOR"),
	  wxT("Mistuning factor (%)."),
	  UNIPAR_REAL,
	  &harmonicPtr->mistuningFactor, NULL,
	  (void * (*)) SetMistuningFactor_Harmonic);
	SetPar_UniParMgr(&pars[HARMONIC_FREQUENCY], wxT("FUND_FREQ"),
	  wxT("Fundamental frequency (Hz)."),
	  UNIPAR_REAL,
	  &harmonicPtr->frequency, NULL,
	  (void * (*)) SetFrequency_Harmonic);
	SetPar_UniParMgr(&pars[HARMONIC_INTENSITY], wxT("INTENSITY"),
	  wxT("Intensity (dB SPL)."),
	  UNIPAR_REAL,
	  &harmonicPtr->intensity, NULL,
	  (void * (*)) SetIntensity_Harmonic);
	SetPar_UniParMgr(&pars[HARMONIC_DURATION], wxT("DURATION"),
	  wxT("Duration (s)."),
	  UNIPAR_REAL,
	  &harmonicPtr->duration, NULL,
	  (void * (*)) SetDuration_Harmonic);
	SetPar_UniParMgr(&pars[HARMONIC_SAMPLINGINTERVAL], wxT("DT"),
	  wxT("Sampling interval, dt (s)."),
	  UNIPAR_REAL,
	  &harmonicPtr->dt, NULL,
	  (void * (*)) SetSamplingInterval_Harmonic);
	SetPar_UniParMgr(&pars[HARMONIC_MODULATIONFREQUENCY], wxT("MOD_FREQ"),
	  wxT("Modulation Frequency (Hz)."),
	  UNIPAR_REAL,
	  &harmonicPtr->modulationFrequency, NULL,
	  (void * (*)) SetModulationFrequency_Harmonic);
	SetPar_UniParMgr(&pars[HARMONIC_MODULATIONPHASE], wxT("MOD_PHASE"),
	  wxT("Modulation Phase (degrees)."),
	  UNIPAR_REAL,
	  &harmonicPtr->modulationPhase, NULL,
	  (void * (*)) SetModulationPhase_Harmonic);
	SetPar_UniParMgr(&pars[HARMONIC_MODULATIONDEPTH], wxT("MOD_DEPTH"),
	  wxT("Modulation depth (%)."),
	  UNIPAR_REAL,
	  &harmonicPtr->modulationDepth, NULL,
	  (void * (*)) SetModulationDepth_Harmonic);
	SetPar_UniParMgr(&pars[HARMONIC_ORDER], wxT("ORDER"),
	  wxT("Filter order."),
	  UNIPAR_INT,
	  &harmonicPtr->order, NULL,
	  (void * (*)) SetOrder_Harmonic);
	SetPar_UniParMgr(&pars[HARMONIC_LOWERCUTOFFFREQ], wxT("LOW_CUTOFF"),
	  wxT("Lower cut off frequency 3 dB down (Hz)."),
	  UNIPAR_REAL,
	  &harmonicPtr->lowerCutOffFreq, NULL,
	  (void * (*)) SetLowerCutOffFreq_Harmonic);
	SetPar_UniParMgr(&pars[HARMONIC_UPPERCUTOFFFREQ], wxT("UPPER_CUTOFF"),
	  wxT("Upper cut off frequency 3 dB down (Hz)."),
	  UNIPAR_REAL,
	  &harmonicPtr->upperCutOffFreq, NULL,
	  (void * (*)) SetUpperCutOffFreq_Harmonic);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Harmonic(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_Harmonic");

	if (harmonicPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (harmonicPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(harmonicPtr->parList);

}

/********************************* CheckPars **********************************/

/*
 * This routine checks that the necessary parameters for the module have been
 * correctly initialised.
 * It also checks that the Nyquist critical frequency is not exceeded.
 * It returns TRUE if there are no problems.
 */
 
BOOLN
CheckPars_Harmonic(void)
{
	static const WChar *funcName = wxT("CheckPars_Harmonic");
	BOOLN	ok;
	double	criticalFrequency;
	
	ok = TRUE;
	if (harmonicPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (!harmonicPtr->lowestHarmonicFlag) {
		NotifyError(wxT("%s: Lowest harmonic variable not set."), funcName);
		ok = FALSE;
	}
	if (!harmonicPtr->highestHarmonicFlag) {
		NotifyError(wxT("%s: Highest harmonic variable not set."), funcName);
		ok = FALSE;
	}
	if (!harmonicPtr->mistunedHarmonicFlag) {
		NotifyError(wxT("%s: Mistuned harmonic variable not set."), funcName);
		ok = FALSE;
	}
	if (!harmonicPtr->phaseModeFlag) {
		NotifyError(wxT("%s: Phase mode not set."), funcName);
		ok = FALSE;
	}
	if (!harmonicPtr->mistuningFactorFlag) {
		NotifyError(wxT("%s: Mistuning factor variable not set."), funcName);
		ok = FALSE;
	}
	if (!harmonicPtr->phaseVariableFlag) {
		NotifyError(wxT("%s: Phase variable variable not set."), funcName);
		ok = FALSE;
	}
	if (!harmonicPtr->frequencyFlag) {
		NotifyError(wxT("%s: Frequency variable not set."), funcName);
		ok = FALSE;
	}
	if (!harmonicPtr->intensityFlag) {
		NotifyError(wxT("%s: Intensity variable not set."), funcName);
		ok = FALSE;
	}
	if (!harmonicPtr->durationFlag) {
		NotifyError(wxT("%s: Duration variable not set."), funcName);
		ok = FALSE;
	}
	if (!harmonicPtr->dtFlag) {
		NotifyError(wxT("%s: Sampling interval (dt) variable not set."),
		  funcName);
		ok = FALSE;
	}
	if (!harmonicPtr->modulationDepthFlag) {
		NotifyError(wxT("%s: Modulation depth variable not set."), funcName);
		ok = FALSE;
	}
	if (harmonicPtr->modulationDepth > DBL_EPSILON) {
		if (!harmonicPtr->modulationFrequencyFlag) {
			NotifyError(wxT("%s: Modulation frequency variable not set."),
			  funcName);
			ok = FALSE;
		}
		if (!harmonicPtr->modulationPhaseFlag) {
			NotifyError(wxT("%s: Modulation phase variable not set."),
			  funcName);
			ok = FALSE;
		}
	}
	if (!harmonicPtr->orderFlag) {
		NotifyError(wxT("%s: Filter order variable not set."), funcName);
		ok = FALSE;
	}
	if (harmonicPtr->order > 0) {
		if (!harmonicPtr->lowerCutOffFreqFlag) {
			NotifyError(wxT("%s: Filter lower cut-off frequency variable not ")
			  wxT("set."), funcName);
			ok = FALSE;
		}
		if (!harmonicPtr->upperCutOffFreqFlag) {
			NotifyError(wxT("%s: Filter upper cut-off frequency variable not set."),
			  funcName);
			ok = FALSE;
		}
		if (harmonicPtr->lowerCutOffFreq > harmonicPtr->upperCutOffFreq) {
			NotifyError(wxT("%s: Illegal cut-off frequency range (%g - %g Hz)."),
			  funcName, harmonicPtr->lowerCutOffFreq,
			  harmonicPtr->upperCutOffFreq);
			ok = FALSE;
		}
	}
	if (harmonicPtr->lowestHarmonic > harmonicPtr->highestHarmonic) {
		NotifyError(wxT("%s: Illegal harmonic range (%d - %d)."), funcName,
		  harmonicPtr->lowestHarmonic, harmonicPtr->highestHarmonic);
		ok = FALSE;
	}
	criticalFrequency = 1.0 / (2.0 * harmonicPtr->dt);
	if (ok && (criticalFrequency <= harmonicPtr->frequency *
	   harmonicPtr->highestHarmonic)) {
		NotifyError(wxT("%s: Sampling rate (dt = %g ms) is too low for the ")
		  wxT("highest frequency."), funcName, MSEC(harmonicPtr->dt));
		ok = FALSE;
	}
	return(ok);
	
}	

/********************************* SetLowestHarmonic **************************/

/*
 * This function sets the module's lowest harmonic parameter.  It first checks
 * that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetLowestHarmonic_Harmonic(int theLowestHarmonic)
{
	static const WChar *funcName = wxT("SetLowestHarmonic_Harmonic");

	harmonicPtr->lowestHarmonicFlag = TRUE;
	if (theLowestHarmonic < 1) {
		NotifyError(wxT("%s: Lowest harmonic must be > 1 (%d)."), funcName,
		  theLowestHarmonic);
		return(FALSE);
	}
	harmonicPtr->lowestHarmonic = theLowestHarmonic;
	harmonicPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetHighestHarmonic *************************/

/*
 * This function sets the module's highest harmonic parameter.  It first checks
 * that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetHighestHarmonic_Harmonic(int theHighestHarmonic)
{
	harmonicPtr->highestHarmonicFlag = TRUE;
	harmonicPtr->highestHarmonic = theHighestHarmonic;
	harmonicPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetMistunedHarmonic ************************/

/*
 * This function sets the module's mistuned harmonic parameter.  It first checks
 * that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetMistunedHarmonic_Harmonic(int theMistunedHarmonic)
{

	harmonicPtr->mistunedHarmonicFlag = TRUE;
	harmonicPtr->mistunedHarmonic = theMistunedHarmonic;
	return(TRUE);

}

/********************************* SetPhaseMode *******************************/

/*
 * This function sets the module's phase mode parameter.  It first checks
 * that the module has been initialised.
 * It returns TRUE if the operation is successful.
C */

BOOLN
SetPhaseMode_Harmonic(WChar *thePhaseMode)
{
	static const WChar *funcName = wxT("SetPhaseMode_Harmonic");
	int		specifier;

	if (harmonicPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(thePhaseMode,
	  harmonicPtr->phaseModeList)) == HARMONIC_NULL) {
		NotifyError(wxT("%s: Illegal mode name (%s)."), funcName, thePhaseMode);
		return(FALSE);
	}
	harmonicPtr->phaseModeFlag = TRUE;
	harmonicPtr->phaseMode = specifier;
	return(TRUE);

}

/********************************* SetMistuningFactor *************************/

/*
 * This function sets the module's mistuningFactor parameter.  It first checks
 * that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetMistuningFactor_Harmonic(double theMistuningFactor)
{

	harmonicPtr->mistuningFactorFlag = TRUE;
	harmonicPtr->mistuningFactor = theMistuningFactor;
	return(TRUE);

}

/****************************** SetPhaseVariable ******************************/

/*
 * This function sets the module's phase mode variable.
 * It first checks that the module has been initialised. 
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPhaseVariable_Harmonic(double thePhaseVariable)
{

	harmonicPtr->phaseVariableFlag = TRUE;
	harmonicPtr->updateProcessVariablesFlag = TRUE;
	harmonicPtr->phaseVariable = thePhaseVariable;
	return(TRUE);

}

/********************************* SetFrequency *******************************/

/*
 * This function sets the module's frequency parameter.  It first checks that
 * the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetFrequency_Harmonic(double theFrequency)
{

	harmonicPtr->frequencyFlag = TRUE;
	harmonicPtr->frequency = theFrequency;
	return(TRUE);

}

/********************************* SetIntensity *******************************/

/*
 * This function sets the module's intensity parameter.  It first checks that
 * the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetIntensity_Harmonic(double theIntensity)
{

	harmonicPtr->intensityFlag = TRUE;
	harmonicPtr->intensity = theIntensity;
	return(TRUE);

}

/********************************* SetDuration ********************************/

/*
 * This function sets the module's duration parameter.  It first checks that
 * the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetDuration_Harmonic(double theDuration)
{

	harmonicPtr->durationFlag = TRUE;
	harmonicPtr->duration = theDuration;
	return(TRUE);

}

/********************************* SetSamplingInterval ************************/

/*
 * This function sets the module's samplingInterval parameter.  It first checks
 * that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetSamplingInterval_Harmonic(double theSamplingInterval)
{
	static const WChar *funcName = wxT("SetSamplingInterval_Harmonic");

	if ( theSamplingInterval <= 0.0 ) {
		NotifyError(wxT("%s: Illegal sampling interval value = %g!"), funcName,
		  theSamplingInterval);
		return(FALSE);
	}
	harmonicPtr->dtFlag = TRUE;
	harmonicPtr->dt = theSamplingInterval;
	return(TRUE);

}

/****************************** SetModulationFrequency ************************/

/*
 * This function sets the module's modulationFrequency parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetModulationFrequency_Harmonic(double theModulationFrequency)
{
	static const WChar	*funcName = wxT("SetModulationFrequency_Harmonic");

	if (harmonicPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	harmonicPtr->modulationFrequencyFlag = TRUE;
	harmonicPtr->modulationFrequency = theModulationFrequency;
	if (harmonicPtr->modulationFrequency == 0.0) {
		NotifyError(wxT("%s: The modulation frequency must be greater than ")
		  wxT("zero."), funcName);
		return(FALSE);
	}	
	return(TRUE);

}

/****************************** SetModulationPhase ****************************/

/*
 * This function sets the module's modulationPhase parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetModulationPhase_Harmonic(double theModulationPhase)
{
	static const WChar	*funcName = wxT("SetModulationPhase_Harmonic");

	if (harmonicPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	harmonicPtr->modulationPhaseFlag = TRUE;
	harmonicPtr->modulationPhase = theModulationPhase;
	return(TRUE);

}

/****************************** SetModulationDepth ****************************/

/*
 * This function sets the module's modulationDepth parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetModulationDepth_Harmonic(double theModulationDepth)
{
	static const WChar	*funcName = wxT("SetModulationDepth_Harmonic");

	if (harmonicPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	harmonicPtr->modulationDepthFlag = TRUE;
	harmonicPtr->modulationDepth = (theModulationDepth < DBL_EPSILON)? 0.0:
	  theModulationDepth;
	return(TRUE);

}

/****************************** SetOrder **************************************/

/*
 * This function sets the module's order parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOrder_Harmonic(int theorder)
{
	static const WChar	*funcName = wxT("SetOrder_Harmonic");

	if (harmonicPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if (harmonicPtr->order < 0)	{
		NotifyError(wxT("%s: Filter order must be > 0."), funcName);
		return(FALSE);
	}
	harmonicPtr->orderFlag = TRUE;
	harmonicPtr->order = theorder;
	return(TRUE);

}

/****************************** SetLowerCutOffFreq ****************************/

/*
 * This function sets the module's lowerCutOffFreq parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLowerCutOffFreq_Harmonic(double theLowerCutOffFreq)
{
	static const WChar	*funcName = wxT("SetLowerCutOffFreq_Harmonic");

	if (harmonicPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	harmonicPtr->lowerCutOffFreqFlag = TRUE;
	harmonicPtr->lowerCutOffFreq = theLowerCutOffFreq;
	return(TRUE);

}

/****************************** SetUpperCutOffFreq ****************************/

/*
 * This function sets the module's upperCutOffFreq parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetUpperCutOffFreq_Harmonic(double theUpperCutOffFreq)
{
	static const WChar	*funcName = wxT("SetUpperCutOffFreq_Harmonic");

	if (harmonicPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	harmonicPtr->upperCutOffFreqFlag = TRUE;
	harmonicPtr->upperCutOffFreq = theUpperCutOffFreq;
	return(TRUE);

}
/********************************* SetPars ************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */
 
BOOLN
SetPars_Harmonic(WChar *thePhaseMode, int theLowestHarmonic,
  int theHighestHarmonic, int theMistunedHarmonic, double theMistuningFactor,
  double thePhaseVariable, double theFrequency, double theIntensity,
  double theDuration, double theSamplingInterval, double theModulationFrequency,
  double theModulationPhase, double theModulationDepth, int theOrder,
  double theLowCutFreq, double theHighCutFreq)
{
	static const WChar *funcName = wxT("SetPars_Harmonic");
	BOOLN	ok;
	
	ok = TRUE;
	if (!SetLowestHarmonic_Harmonic(theLowestHarmonic))
		ok = FALSE;
	if (!SetHighestHarmonic_Harmonic(theHighestHarmonic))
		ok = FALSE;
	if (!SetMistunedHarmonic_Harmonic(theMistunedHarmonic))
		ok = FALSE;
	if (!SetMistuningFactor_Harmonic(theMistuningFactor))
		ok = FALSE;
	if (!SetPhaseMode_Harmonic(thePhaseMode))
		ok = FALSE;
	if (!SetPhaseVariable_Harmonic(thePhaseVariable))
		ok = FALSE;
	if (!SetFrequency_Harmonic(theFrequency))
		ok = FALSE;
	if (!SetIntensity_Harmonic(theIntensity))
		ok = FALSE;
	if (!SetDuration_Harmonic(theDuration))
		ok = FALSE;
	if (!SetSamplingInterval_Harmonic(theSamplingInterval))
		ok = FALSE;
	if (!SetModulationFrequency_Harmonic(theModulationFrequency))
		ok = FALSE;
	if (!SetModulationPhase_Harmonic(theModulationPhase))
		ok = FALSE;
	if (!SetModulationDepth_Harmonic(theModulationDepth))
		ok = FALSE;
	if (!SetOrder_Harmonic(theOrder))
		ok = FALSE;
	if (!SetLowerCutOffFreq_Harmonic(theLowCutFreq))
		ok = FALSE;
	if (!SetUpperCutOffFreq_Harmonic(theHighCutFreq))
		ok = FALSE;
	if (!ok)
		NotifyError(wxT("%s: Failed to set all module parameters."), funcName);
	return(ok);
	
}

/****************************** PrintPars *************************************/

/*
 * This program prints the parameters of the module to the standard output.
 */
 
BOOLN
PrintPars_Harmonic(void)
{
	static const WChar *funcName = wxT("PrintPars_Harmonic");

	if (!CheckPars_Harmonic()) {
		NotifyError(wxT("%s: Parameters have not been correctly set."),
		  funcName);
		return(FALSE);
	}
	DPrint(wxT("Harmonic series Module Parameters:-\n"));
	DPrint(wxT("\tLowest/highest harmonic = %d / %d,\n"),
	  harmonicPtr->lowestHarmonic, harmonicPtr->highestHarmonic);
	DPrint(wxT("\tPhase mode = %s,\t"),
	  harmonicPtr->phaseModeList[harmonicPtr->phaseMode].name);
	DPrint(wxT("Phase variable = %g"), harmonicPtr->phaseVariable);
	switch (harmonicPtr->phaseMode) {
	case HARMONIC_SCHROEDER:
		DPrint(wxT(" (C value)"));
		break;
	case HARMONIC_RANDOM:
		DPrint(wxT(" (random number seed)"));
		break;
	default:
		;
	}
	DPrint(wxT(",\n"));
	DPrint(wxT("\tMistuned harmonic = %d,\tmistuning factor = %g %%,\n"),
	  harmonicPtr->mistunedHarmonic, harmonicPtr->mistuningFactor);
	DPrint(wxT("\tFundamental frequency = %g Hz,\tIntensity = %g dB SPL,\n"),
	  harmonicPtr->frequency, harmonicPtr->intensity);
	DPrint(wxT("\tDuration = %g ms,\tSampling interval = %g ms\n"), MSEC(
	  harmonicPtr->duration), MSEC(harmonicPtr->dt));
	if (harmonicPtr->modulationDepth > DBL_EPSILON) {
		DPrint(wxT("\tModulation frequency = %g Hz\n"), harmonicPtr->
		  modulationFrequency);
		DPrint(wxT("\tModulation phase = %g degrees,"), harmonicPtr->
		  modulationPhase);
		DPrint(wxT("\tmodulationDepth = %g %%\n"), harmonicPtr->
		  modulationDepth);
	}
	if (harmonicPtr->order > 0) {
		DPrint(wxT("\tFilter order = %d \n"), harmonicPtr->order);
		DPrint(wxT("\tFilter low/high cut-off frequencys = %g / %g Hz\n"),
		  harmonicPtr->lowerCutOffFreq, harmonicPtr->upperCutOffFreq);
	}
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.
 */
 
BOOLN
ReadPars_Harmonic(WChar *fileName)
{
	static const WChar *funcName = wxT("ReadPars_Harmonic");
	BOOLN	ok;
	WChar	*filePath;
	WChar	phaseMode[MAXLINE];
	int		lowestHarmonic, highestHarmonic, mistunedHarmonic, order;
	double	phaseVariable, mistuningFactor, frequency, intensity, duration;
	double	samplingInterval, modulationFrequency, modulationPhase;
	double	modulationDepth, lowerCutOffFreq, upperCutOffFreq;
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
    if (!GetPars_ParFile(fp, wxT("%lf"), &frequency))
		ok = FALSE;
    if (!GetPars_ParFile(fp, wxT("%d"), &lowestHarmonic))
		ok = FALSE;
    if (!GetPars_ParFile(fp, wxT("%d"), &highestHarmonic))
		ok = FALSE;
    if (!GetPars_ParFile(fp, wxT("%d"), &mistunedHarmonic))
		ok = FALSE;
    if (!GetPars_ParFile(fp, wxT("%lf"), &mistuningFactor))
		ok = FALSE;
    if (!GetPars_ParFile(fp, wxT("%s"), &phaseMode))
		ok = FALSE;
    if (!GetPars_ParFile(fp, wxT("%lf"), &phaseVariable))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &modulationFrequency))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &modulationPhase))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &modulationDepth))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%d"), &order))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &lowerCutOffFreq))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &upperCutOffFreq))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &intensity))
		ok = FALSE;
    if (!GetPars_ParFile(fp, wxT("%lf"), &duration))
		ok = FALSE;
    if (!GetPars_ParFile(fp, wxT("%lf"), &samplingInterval))
		ok = FALSE;
    fclose(fp);
    Free_ParFile();
	if (!ok) {
		NotifyError(wxT("%s: Not enough lines, or invalid parameters, in ")
		  wxT("module parameter file '%s'."), funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_Harmonic(phaseMode, lowestHarmonic, highestHarmonic,
	  mistunedHarmonic, mistuningFactor, phaseVariable, frequency, intensity,
	  duration, samplingInterval, modulationFrequency, modulationPhase, 
	  modulationDepth, order, lowerCutOffFreq, upperCutOffFreq)) {
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
SetParsPointer_Harmonic(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Harmonic");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	harmonicPtr = (HarmonicPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Harmonic(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Harmonic");

	if (!SetParsPointer_Harmonic(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Harmonic(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = harmonicPtr;
	theModule->CheckPars = CheckPars_Harmonic;
	theModule->Free = Free_Harmonic;
	theModule->GetUniParListPtr = GetUniParListPtr_Harmonic;
	theModule->PrintPars = PrintPars_Harmonic;
	theModule->ReadPars = ReadPars_Harmonic;
	theModule->RunProcess = GenerateSignal_Harmonic;
	theModule->SetParsPointer = SetParsPointer_Harmonic;
	return(TRUE);

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 */

BOOLN
InitProcessVariables_Harmonic(EarObjectPtr data)
{
	static const WChar *funcName = wxT("InitProcessVariables_Harmonic");
	int		totalNumberOfHarmonics;
	HarmonicPtr	p = harmonicPtr;
	
	totalNumberOfHarmonics = p->highestHarmonic - p->lowestHarmonic + 1;
	if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
		FreeProcessVariables_Harmonic();
		if (!SetRandPars_EarObject(data, p->ranSeed, funcName))
			return(FALSE);
		if ((p->phase = (double *) calloc(totalNumberOfHarmonics, sizeof(
		  double))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'phase' array"), funcName);
			return(FALSE);
		}
		if ((p->harmonicFrequency = (double *) calloc(totalNumberOfHarmonics,
		  sizeof(double))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'harmonic frequencies' ")
			  wxT("array"), funcName);
			return(FALSE);
		}
		if ((p->modIndex = (double *) calloc(totalNumberOfHarmonics, sizeof(
		  double))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'modIndex' array"),
			  funcName);
			return(FALSE);
		}
		p->updateProcessVariablesFlag = FALSE;
	}
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

BOOLN
FreeProcessVariables_Harmonic(void)
{
	if (harmonicPtr->phase) {
		free(harmonicPtr->phase);
		harmonicPtr->phase = NULL;
	}
	if (harmonicPtr->harmonicFrequency) {
		free(harmonicPtr->harmonicFrequency);
		harmonicPtr->harmonicFrequency = NULL;
	}
	if (harmonicPtr->modIndex) {
		free(harmonicPtr->modIndex);
		harmonicPtr->modIndex = NULL;
	}
	return(TRUE);

}

/********************************* GenerateSignal *****************************/

/*
 * This routine allocates memory for the output signal, if necessary, and
 * generates the signal into channel[0] of the signal data-set.
 * It checks that all initialisation has been correctly carried out by calling
 * the appropriate checking routines.
 * It can be called repeatedly with different parameter values if required.
 * Stimulus generation only sets the output signal, the input signal is not
 * used.
 * With repeated calls the Signal memory is only allocated once, then re-used.
 */

BOOLN
GenerateSignal_Harmonic(EarObjectPtr data)
{
	static const WChar *funcName = wxT("GenerateSignal_Harmonic");
	int			j, totalNumberOfHarmonics, harmonicNumber;
	ChanLen		i, t;
	double		instantFreq, piOver2;
	double		amplitude, timexPix2, filterAmp, modulation;
	register	ChanData	*dataPtr;
	HarmonicPtr	p = harmonicPtr;

	if (!data->threadRunFlag) {
		if (data == NULL) {
			NotifyError(wxT("%s: EarObject not initialised."), funcName);
			return(FALSE);
		}	
		if (!CheckPars_Harmonic())
			return(FALSE);
		SetProcessName_EarObject(data, wxT("Harmonic series stimulus"));
		if ( !InitOutSignal_EarObject(data, HARMONIC_NUM_CHANNELS,
		  (ChanLen) floor(harmonicPtr->duration / harmonicPtr->dt + 0.5),
	    	harmonicPtr->dt) ) {
			NotifyError(wxT("%s: Cannot initialise output signal"), funcName);
			return(FALSE);
		}
		if (!InitProcessVariables_Harmonic(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	totalNumberOfHarmonics = harmonicPtr->highestHarmonic -
	  harmonicPtr->lowestHarmonic + 1;
	piOver2 = PI / 2.0;
	for (j = 0; j < totalNumberOfHarmonics; j++) {
		harmonicNumber = p->lowestHarmonic + j;
		switch (p->phaseMode) {
			case HARMONIC_RANDOM:
				p->ranSeed = (long) p->phaseVariable;
				p->phase[j] = PIx2 * Ran01_Random(data->randPars);
				break;
			case HARMONIC_SINE:
				p->phase[j] = 0.0;
				break;
			case HARMONIC_COSINE:
				p->phase[j] = PI / 2.0;
				break;
			case HARMONIC_ALTERNATING:
				p->phase[j] = ((j % 2) == 0)? 0.0: piOver2;
				break;
			case HARMONIC_SCHROEDER:
				p->phase[j] = p->phaseVariable * PI *
				  harmonicNumber * (harmonicNumber + 1) /
				  totalNumberOfHarmonics;
				break;
			case HARMONIC_PLACK_AND_WHITE:
				p->phase[j] = harmonicNumber * p->phaseVariable;
				break;
			case HARMONIC_USER:
				p->phase[j] = p->phaseVariable;
				break;
			case HARMONIC_NULL:
				p->phase[j] = 0.0;
				break;
		} /* switch */
		p->harmonicFrequency[j] = p->frequency * harmonicNumber;
		if (harmonicNumber == p->mistunedHarmonic)
			p->harmonicFrequency[j] += p->harmonicFrequency[j] *
			p->mistuningFactor / 100.0;
		p->modIndex[j] = (p->modulationDepth < DBL_EPSILON)? 0.0:
		  (p->modulationDepth / 100.0) * (p->harmonicFrequency[j] / p->
		  modulationFrequency);
	}
	amplitude = RMS_AMP(harmonicPtr->intensity) * SQRT_2;
	dataPtr = _OutSig_EarObject(data)->channel[0];
	for (i = 0, t = data->timeIndex + 1; i < _OutSig_EarObject(data)->length; i++, t++,
	  dataPtr++) {
	  	timexPix2 = PIx2 * t * _OutSig_EarObject(data)->dt;
		for (j = 0, *dataPtr = 0.0; j < totalNumberOfHarmonics; j++) {
	  		modulation = harmonicPtr->modIndex[j] *
	  		  cos(harmonicPtr->modulationFrequency * timexPix2 +
	  		  DEGREES_TO_RADS(harmonicPtr->modulationPhase));
			instantFreq = harmonicPtr->harmonicFrequency[j] +
			  harmonicPtr->modIndex[j] * harmonicPtr->modulationFrequency *
		  	  sin(harmonicPtr->modulationFrequency * timexPix2 +
		  	  DEGREES_TO_RADS(harmonicPtr->modulationPhase));
			filterAmp = BandPassFD_Filters(instantFreq,
			  harmonicPtr->lowerCutOffFreq, harmonicPtr->upperCutOffFreq,
			  harmonicPtr->order);
			*dataPtr += amplitude * filterAmp *
			  sin(harmonicPtr->harmonicFrequency[j] * timexPix2 +
			  harmonicPtr->phase[j] - modulation);
		}
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

} /* GenerateSignal_Harmonic */

