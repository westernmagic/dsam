/**********************
 *
 * File:		StStepFun.c
 * Purpose:		This module contains the step function stimulus generation
 *				paradigm.
 * Comments:	The module generates a constant signal preceded and ended by
 *				periods of of specified amplitude.
 *				The magnitude of the step function has no units, i.e. they
 *				must be implicitly defined by the user (Arbitrary units)
 *				09-11-98 LPO: Changed parameter names 'stepAmplitude' ->
 *				'amplitude' and 'duration' -> 'duration'.
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		09 Nov 1998
 * Copyright:	(c) 1998, University of Essex
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
#include "StStepFun.h"

/********************************* Global variables ***************************/

StepFunPtr	stepFunPtr = NULL;

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
Init_StepFunction(ParameterSpecifier parSpec)
{
	static const WChar *funcName = wxT("Init_StepFunction");

	if (parSpec == GLOBAL) {
		if (stepFunPtr != NULL)
			free(stepFunPtr);
		if ((stepFunPtr = (StepFunPtr) malloc(sizeof(StepFun))) ==
		  NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"), funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (stepFunPtr == NULL) { 
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	stepFunPtr->parSpec = parSpec;
	stepFunPtr->durationFlag = TRUE;
	stepFunPtr->dtFlag = TRUE;
	stepFunPtr->amplitudeFlag = TRUE;
	stepFunPtr->beginPeriodDurationFlag = TRUE;
	stepFunPtr->endPeriodDurationFlag = TRUE;
	stepFunPtr->beginEndAmplitudeFlag = TRUE;
	stepFunPtr->amplitude = 30.0;
	stepFunPtr->duration = 0.08;
	stepFunPtr->dt = DEFAULT_DT;
	stepFunPtr->beginPeriodDuration  = 0.01;
	stepFunPtr->endPeriodDuration = 0.01;
	stepFunPtr->beginEndAmplitude = 0.0;

	if (!SetUniParList_StepFunction()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_StepFunction();
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
Free_StepFunction(void)
{
	if (stepFunPtr == NULL)
		return(TRUE);
	if (stepFunPtr->parList)
		FreeList_UniParMgr(&stepFunPtr->parList);
	if (stepFunPtr->parSpec == GLOBAL) {
		free(stepFunPtr);
		stepFunPtr = NULL;
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
SetUniParList_StepFunction(void)
{
	static const WChar *funcName = wxT("SetUniParList_StepFunction");
	UniParPtr	pars;

	if ((stepFunPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  STEPFUNCTION_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = stepFunPtr->parList->pars;
	SetPar_UniParMgr(&pars[STEPFUNCTION_AMPLITUDE], wxT("AMPLITUDE"),
	  wxT("Amplitude (arbitrary units)."),
	  UNIPAR_REAL,
	  &stepFunPtr->amplitude, NULL,
	  (void * (*)) SetAmplitude_StepFunction);
	SetPar_UniParMgr(&pars[STEPFUNCTION_BEGINENDAMPLITUDE], wxT("B_E_AMP"),
	  wxT("Begin-end period amplitude (arbitrary units)."),
	  UNIPAR_REAL,
	  &stepFunPtr->beginEndAmplitude, NULL,
	  (void * (*)) SetBeginEndAmplitude_StepFunction);
	SetPar_UniParMgr(&pars[STEPFUNCTION_BEGINPERIODDURATION], wxT("BEGIN_PERIOD"),
	  wxT("Period before the signal begins (s)."),
	  UNIPAR_REAL,
	  &stepFunPtr->beginPeriodDuration, NULL,
	  (void * (*)) SetBeginPeriodDuration_StepFunction);
	SetPar_UniParMgr(&pars[STEPFUNCTION_ENDPERIODDURATION], wxT("END_PERIOD"),
	  wxT("Period after the signal ends (s)."),
	  UNIPAR_REAL,
	  &stepFunPtr->endPeriodDuration, NULL,
	  (void * (*)) SetEndPeriodDuration_StepFunction);
	SetPar_UniParMgr(&pars[STEPFUNCTION_DURATION], wxT("DURATION"),
	  wxT("Duration (s)."),
	  UNIPAR_REAL,
	  &stepFunPtr->duration, NULL,
	  (void * (*)) SetDuration_StepFunction);
	SetPar_UniParMgr(&pars[STEPFUNCTION_SAMPLINGINTERVAL], wxT("DT"),
	  wxT("Sampling interval, dt (s)."),
	  UNIPAR_REAL,
	  &stepFunPtr->dt, NULL,
	  (void * (*)) SetSamplingInterval_StepFunction);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_StepFunction(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_StepFunction");

	if (stepFunPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (stepFunPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been initialised. "
		  "NULL returned."), funcName);
		return(NULL);
	}
	return(stepFunPtr->parList);

}

/********************************* CheckPars **********************************/

/*
 * This routine checks that the necessary parameters for the module have been
 * correctly initialised.
 * It returns TRUE if there are no problems.
 */
 
BOOLN
CheckPars_StepFunction(void)
{
	static const WChar *funcName = wxT("CheckPars_StepFunction");
	BOOLN ok;
	
	ok = TRUE;
	if (stepFunPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (!stepFunPtr->amplitudeFlag) {
		NotifyError(wxT("%s: step amplitude variable not set."), funcName);
		ok = FALSE;
	}
	if (!stepFunPtr->durationFlag) {
		NotifyError(wxT("%s: step duration variable not set."), funcName);
		ok = FALSE;
	}
	if (!stepFunPtr->beginPeriodDurationFlag) {
		NotifyError(wxT("%s: beginPeriodDuration variable not set."), funcName);
		ok = FALSE;
	}
	if (!stepFunPtr->endPeriodDurationFlag) {
		NotifyError(wxT("%s: endPeriodDuration variable not set."), funcName);
		ok = FALSE;
	}
	if (!stepFunPtr->beginEndAmplitudeFlag) {
		NotifyError(wxT("%s: beginEndAmplitude variable not set."), funcName);
		ok = FALSE;
	}
	if (!stepFunPtr->dtFlag) {
		NotifyError(wxT("%s: dtFlag variable not set."), funcName);
		ok = FALSE;
	}
	/* if ( (stepFunPtr->beginPeriodDuration > stepFunPtr->duration) ||
	  (stepFunPtr->endPeriodDuration > stepFunPtr->duration) ) {
	  	NotifyError(wxT("%s:  begin and/or end length parameters are longer than "\
	  	  "the signal duration."), funcName);
		ok = FALSE;
	} */
	return(ok);

}	

/********************************* SetAmplitude *******************************/

/*
 * This function sets the module's amplitude parameter.  It first checks
 * that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetAmplitude_StepFunction(double theAmplitude)
{
	static const WChar *funcName = wxT("SetAmplitude_StepFunction");

	if (stepFunPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	stepFunPtr->amplitudeFlag = TRUE;
	stepFunPtr->amplitude = theAmplitude;
	return(TRUE);

}

/********************************* SetDuration ********************************/

/*
 * This function sets the module's duration parameter.  It first checks
 * that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetDuration_StepFunction(double theDuration)
{
	static const WChar *funcName = wxT("SetDuration_StepFunction");

	if (stepFunPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	stepFunPtr->durationFlag = TRUE;
	stepFunPtr->duration = theDuration;
	return(TRUE);

}

/********************************* SetBeginPeriodDuration *********************/

/*
 * This function sets the module's beginPeriodDuration parameter.  It first
 * checks that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetBeginPeriodDuration_StepFunction(double theBeginPeriodDuration)
{
	static const WChar *funcName = wxT("SetBeginPeriodDuration_StepFunction");

	if (stepFunPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	stepFunPtr->beginPeriodDurationFlag = TRUE;
	stepFunPtr->beginPeriodDuration = theBeginPeriodDuration;
	return(TRUE);

}

/********************************* SetEndPeriodDuration ***********************/

/*
 * This function sets the module's endPeriodDuration parameter.  It first checks
 * that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetEndPeriodDuration_StepFunction(double theEndPeriodDuration)
{
	static const WChar *funcName = wxT("SetEndPeriodDuration_StepFunction");

	if (stepFunPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	stepFunPtr->endPeriodDurationFlag = TRUE;
	stepFunPtr->endPeriodDuration = theEndPeriodDuration;
	return(TRUE);

}

/********************************* SetBeginEndAmplitude ***********************/

/*
 * This function sets the module's beginEndAmplitude parameter.  It first
 * checks that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetBeginEndAmplitude_StepFunction(double theBeginEndAmplitude)
{
	static const WChar *funcName = wxT("SetBeginEndAmplitude_StepFunction");

	if (stepFunPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	stepFunPtr->beginEndAmplitudeFlag = TRUE;
	stepFunPtr->beginEndAmplitude = theBeginEndAmplitude;
	return(TRUE);

}

/********************************* SetSamplingInterval ************************/

/*
 * This function sets the module's timeStep parameter.  It first checks that
 * the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetSamplingInterval_StepFunction(double theSamplingInterval)
{
	static const WChar *funcName = wxT("SetSamplingInterval_StepFunction");

	if (stepFunPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ( theSamplingInterval <= 0.0 ) {
		NotifyError(wxT("%s: Illegal sampling interval value = %g!"), funcName,
		  theSamplingInterval);
		return(FALSE);
	}
	stepFunPtr->dtFlag = TRUE;
	stepFunPtr->dt = theSamplingInterval;
	return(TRUE);

}

/********************************* SetPars ************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */
 
BOOLN
SetPars_StepFunction(double theAmplitude, double theDuration,
  double theSamplingInterval, double theBeginPeriodDuration,
  double theEndPeriodDuration, double theBeginEndAmplitude)
{
	static const WChar *funcName = wxT("SetPars_StepFunction");
	BOOLN	ok;
	
	ok = TRUE;
	if (!SetAmplitude_StepFunction(theAmplitude))
		ok = FALSE;
	if (!SetDuration_StepFunction(theDuration))
		ok = FALSE;
	if (!SetSamplingInterval_StepFunction(theSamplingInterval))
		ok = FALSE;
	if (!SetBeginPeriodDuration_StepFunction(theBeginPeriodDuration))
		ok = FALSE;
	if (!SetEndPeriodDuration_StepFunction(theEndPeriodDuration))
		ok = FALSE;
	if (!SetBeginEndAmplitude_StepFunction(theBeginEndAmplitude))
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
PrintPars_StepFunction(void)
{
	static const WChar *funcName = wxT("PrintPars_StepFunction");

	if (!CheckPars_StepFunction()) {
		NotifyError(wxT("%s: Parameters have not been correctly set."), funcName);
		return(FALSE);
	}
	DPrint(wxT("Step Function Module Parameters:-\n"));
	DPrint(wxT("\tStep amplitude = %g,\t\tStep duration = "
	  "%g ms,\n"), stepFunPtr->amplitude, MSEC(stepFunPtr->duration));
	DPrint(wxT("\tBegin-end Amplitude = %g,\tBeing/end period "
	  "duration = %g/%g ms\n"), stepFunPtr->beginEndAmplitude,
	  MSEC(stepFunPtr->beginPeriodDuration),
	  MSEC(stepFunPtr->endPeriodDuration));
	DPrint(wxT("\tSampling interval = %g ms\n"),
	  MSEC(stepFunPtr->dt));
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.
 */
 
BOOLN
ReadPars_StepFunction(WChar *fileName)
{
	static const WChar *funcName = wxT("ReadPars_StepFunction");
	BOOLN	ok;
	char	*filePath;
	double  amplitude, beginPeriodDuration, endPeriodDuration;
	double  duration, samplingInterval, beginEndAmplitude;
    FILE    *fp;
    
	filePath = ConvUTF8_Utility_String(GetParsFileFPath_Common(fileName));
    if ((fp = fopen(filePath, "r")) == NULL) {
        NotifyError(wxT("%s: Cannot open data file '%s'.\n"), funcName, filePath);
		return(FALSE);
    }
    DPrint(wxT("%s: Reading from '%s':\n"), funcName, filePath);
    Init_ParFile();
	ok = TRUE;
    if (!GetPars_ParFile(fp, wxT("%lf"), &amplitude))
		ok = FALSE;
    if (!GetPars_ParFile(fp, wxT("%lf"), &duration))
		ok = FALSE;
   if (!GetPars_ParFile(fp, wxT("%lf"), &beginEndAmplitude))
		ok = FALSE;
    if (!GetPars_ParFile(fp, wxT("%lf"), &beginPeriodDuration))
		ok = FALSE;
    if (!GetPars_ParFile(fp, wxT("%lf"), &endPeriodDuration))
		ok = FALSE;
    if (!GetPars_ParFile(fp, wxT("%lf"), &samplingInterval))
		ok = FALSE;
    fclose(fp);
    Free_ParFile();
	if (!ok) {
		NotifyError(wxT("%s: Not enough lines, or invalid parameters, in module "
		  "parameter file '%s'."), funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_StepFunction(amplitude, duration, samplingInterval,
	  beginPeriodDuration, endPeriodDuration, beginEndAmplitude)) {
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
SetParsPointer_StepFunction(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_StepFunction");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	stepFunPtr = (StepFunPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_StepFunction(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_StepFunction");

	if (!SetParsPointer_StepFunction(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_StepFunction(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."), funcName);
		return(FALSE);
	}
	theModule->parsPtr = stepFunPtr;
	theModule->CheckPars = CheckPars_StepFunction;
	theModule->Free = Free_StepFunction;
	theModule->GetUniParListPtr = GetUniParListPtr_StepFunction;
	theModule->PrintPars = PrintPars_StepFunction;
	theModule->ReadPars = ReadPars_StepFunction;
	theModule->RunProcess = GenerateSignal_StepFunction;
	theModule->SetParsPointer = SetParsPointer_StepFunction;
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
 */

BOOLN
GenerateSignal_StepFunction(EarObjectPtr data)
{
	static const WChar *funcName = wxT("GenerateSignal_StepFunction");
	double		totalDuration;
	register	ChanData	*dataPtr;
	ChanLen		i, beginPeriodDurationIndex, endSignalIndex;

	if (!data->threadRunFlag) {
		if (data == NULL) {
			NotifyError(wxT("%s: EarObject not initialised."), funcName);
			return(FALSE);
		}	
		if (!CheckPars_StepFunction())
			return(FALSE);
		SetProcessName_EarObject(data, wxT("Step-function stimulus"));
		totalDuration = stepFunPtr->beginPeriodDuration +
		  stepFunPtr->duration + stepFunPtr->endPeriodDuration;
		data->updateProcessFlag = TRUE;
		if ( !InitOutSignal_EarObject(data, 1, (ChanLen) floor(totalDuration /
		  stepFunPtr->dt + 0.5), stepFunPtr->dt) ) {
			NotifyError(wxT("%s: Cannot initialise output signal"), funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	beginPeriodDurationIndex = (ChanLen) floor(stepFunPtr->beginPeriodDuration /
	  _OutSig_EarObject(data)->dt + 0.5);
	endSignalIndex = beginPeriodDurationIndex + (ChanLen) floor(stepFunPtr->
	  duration / stepFunPtr->dt + 0.5);
	dataPtr = _OutSig_EarObject(data)->channel[0];
	for (i = 0; i < _OutSig_EarObject(data)->length; i++)
		if ( (i >= beginPeriodDurationIndex) && (i <= endSignalIndex) )
			*(dataPtr++) = stepFunPtr->amplitude;
		else
			*(dataPtr++) = stepFunPtr->beginEndAmplitude;
	_OutSig_EarObject(data)->rampFlag = TRUE;/* Step functions should not be ramped. */
	SetProcessContinuity_EarObject(data);
	return(TRUE);

} /* GenerateSignal_StepFunction */

