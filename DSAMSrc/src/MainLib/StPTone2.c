/**********************
 *
 * File:		StPTone2.c
 * Purpose:		The module generates a pure-tone signal preceded and ended by
 *				periods of silence.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		12 Mar 1997
 * Copyright:	(c) 1997,  University of Essex
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
#include "StPTone2.h"

/********************************* Global variables ***************************/

PureTone2Ptr	pureTone2Ptr = NULL;

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
Init_PureTone_2(ParameterSpecifier parSpec)
{
	static const char *funcName = "Init_PureTone_2";

	if (parSpec == GLOBAL) {
		if (pureTone2Ptr != NULL)
			Free_PureTone_2();
		if ((pureTone2Ptr = (PureTone2Ptr) malloc(sizeof(PureTone2))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (pureTone2Ptr == NULL) { 
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	pureTone2Ptr->parSpec = parSpec;
	pureTone2Ptr->frequencyFlag = FALSE;
	pureTone2Ptr->durationFlag = FALSE;
	pureTone2Ptr->dtFlag = FALSE;
	pureTone2Ptr->intensityFlag = FALSE;
	pureTone2Ptr->beginPeriodDurationFlag = FALSE;
	pureTone2Ptr->endPeriodDurationFlag = FALSE;
	pureTone2Ptr->frequency = 0.0;
	pureTone2Ptr->intensity = 0.0;
	pureTone2Ptr->duration = 0.0;
	pureTone2Ptr->dt = 0.0;
	pureTone2Ptr->beginPeriodDuration = 0.0;
	pureTone2Ptr->endPeriodDuration = 0.0;

	if (!SetUniParList_PureTone_2()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_PureTone_2();
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
Free_PureTone_2(void)
{
	if (pureTone2Ptr == NULL)
		return(TRUE);
	if (pureTone2Ptr->parList)
		FreeList_UniParMgr(&pureTone2Ptr->parList);
	if (pureTone2Ptr->parSpec == GLOBAL) {
		free(pureTone2Ptr);
		pureTone2Ptr = NULL;
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
SetUniParList_PureTone_2(void)
{
	static const char *funcName = "SetUniParList_PureTone_2";
	UniParPtr	pars;

	if ((pureTone2Ptr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  PURETONE_2_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = pureTone2Ptr->parList->pars;
	SetPar_UniParMgr(&pars[PURETONE_2_FREQUENCY], "FREQUENCY",
	  "Frequency (Hz).",
	  UNIPAR_REAL,
	  &pureTone2Ptr->frequency, NULL,
	  (void * (*)) SetFrequency_PureTone_2);
	SetPar_UniParMgr(&pars[PURETONE_2_INTENSITY], "INTENSITY",
	  "Intensity (dB SPL).",
	  UNIPAR_REAL,
	  &pureTone2Ptr->intensity, NULL,
	  (void * (*)) SetIntensity_PureTone_2);
	SetPar_UniParMgr(&pars[PURETONE_2_DURATION], "DURATION",
	  "Duration (s).",
	  UNIPAR_REAL,
	  &pureTone2Ptr->duration, NULL,
	  (void * (*)) SetDuration_PureTone_2);
	SetPar_UniParMgr(&pars[PURETONE_2_SAMPLINGINTERVAL], "DT",
	  "Sampling interval, dt (s).",
	  UNIPAR_REAL,
	  &pureTone2Ptr->dt, NULL,
	  (void * (*)) SetSamplingInterval_PureTone_2);
	SetPar_UniParMgr(&pars[PURETONE_2_BEGINPERIODDURATION], "BEGIN_SILENCE",
	  "Silence period before the signal begins (s).",
	  UNIPAR_REAL,
	  &pureTone2Ptr->beginPeriodDuration, NULL,
	  (void * (*)) SetBeginPeriodDuration_PureTone_2);
	SetPar_UniParMgr(&pars[PURETONE_2_ENDPERIODDURATION], "END_SILENCE",
	  "Silence period after the signal ends (s).",
	  UNIPAR_REAL,
	  &pureTone2Ptr->endPeriodDuration, NULL,
	  (void * (*)) SetEndPeriodDuration_PureTone_2);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_PureTone_2(void)
{
	static const char	*funcName = "GetUniParListPtr_PureTone_2";

	if (pureTone2Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (pureTone2Ptr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(pureTone2Ptr->parList);

}

/********************************* CheckPars **********************************/

/*
 * This routine checks that the necessary parameters for the module have been
 * correctly initialised.
 * It checks that the Nyquist critical frequency is not exceeded.
 * It returns TRUE if there are no problems.
 */
 
BOOLN
CheckPars_PureTone_2(void)
{
	static const char *funcName = "CheckPars_PureTone_2";
	BOOLN	ok;
	double	criticalFrequency;
	
	ok = TRUE;
	if (pureTone2Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!pureTone2Ptr->frequencyFlag) {
		NotifyError("%s: Frequency variable not set.", funcName);
		ok = FALSE;
	}
	if (!pureTone2Ptr->intensityFlag) {
		NotifyError("%s: intensity variable not set.", funcName);
		ok = FALSE;
	}
	if (!pureTone2Ptr->durationFlag) {
		NotifyError("%s: duration variable not set.", funcName);
		ok = FALSE;
	}
	if (!pureTone2Ptr->beginPeriodDurationFlag) {
		NotifyError("%s: beginPeriodDuration variable not set.", funcName);
		ok = FALSE;
	}
	if (!pureTone2Ptr->endPeriodDurationFlag) {
		NotifyError("%s: endPeriodDuration variable not set.", funcName);
		ok = FALSE;
	}
	if (!pureTone2Ptr->dtFlag) {
		NotifyError("%s: dt variable not set.", funcName);
		ok = FALSE;
	}
	/* if ( (pureTone2Ptr->beginPeriodDuration > pureTone2Ptr->duration) ||
	  (pureTone2Ptr->endPeriodDuration > pureTone2Ptr->duration) ) {
	  	NotifyError("%s: A silence length parameters is longer than the "\
	  	  "signal duration.", funcName);
	  	ok = FALSE;
	} */
	criticalFrequency = 1.0 / (2.0 * pureTone2Ptr->dt);
	if (ok && (criticalFrequency <= pureTone2Ptr->frequency)) {
		NotifyError("%s: Sampling rate (dt = %g ms) is too low for the "\
		  "frequency.", funcName, MSEC(pureTone2Ptr->dt));
		ok = FALSE;
	} 
	return(ok);

}	

/********************************* SetFrequency *******************************/

/*
 * This function sets the module's frequency parameter.  It first checks that
 * the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetFrequency_PureTone_2(double theFrequency)
{
	static const char *funcName = "SetFrequency_PureTone_2";

	if (pureTone2Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	pureTone2Ptr->frequencyFlag = TRUE;
	pureTone2Ptr->frequency = theFrequency;
	return(TRUE);

}

/********************************* SetIntensity *******************************/

/*
 * This function sets the module's intensity parameter.  It first checks that
 * the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetIntensity_PureTone_2(double theIntensity)
{
	static const char *funcName = "SetIntensity_PureTone_2";

	if (pureTone2Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	pureTone2Ptr->intensityFlag = TRUE;
	pureTone2Ptr->intensity = theIntensity;
	return(TRUE);

}

/********************************* SetDuration ********************************/

/*
 * This function sets the module's duration parameter.  It first checks that
 * the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetDuration_PureTone_2(double theDuration)
{
	static const char *funcName = "SetDuration_PureTone_2";

	if (pureTone2Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	pureTone2Ptr->durationFlag = TRUE;
	pureTone2Ptr->duration = theDuration;
	return(TRUE);

}

/********************************* SetBeginPeriodDuration *********************/

/*
 * This function sets the module's beginPeriodDuration parameter.  It first
 * checks that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetBeginPeriodDuration_PureTone_2(double theBeginPeriodDuration)
{
	static const char *funcName = "SetBeginPeriodDuration_PureTone_2";

	if (pureTone2Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	pureTone2Ptr->beginPeriodDurationFlag = TRUE;
	pureTone2Ptr->beginPeriodDuration = theBeginPeriodDuration;
	return(TRUE);

}

/********************************* SetEndPeriodDuration ***********************/

/*
 * This function sets the module's endPeriodDuration parameter.  It first checks
 * that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetEndPeriodDuration_PureTone_2(double theEndPeriodDuration)
{
	static const char *funcName = "SetEndPeriodDuration_PureTone_2";

	if (pureTone2Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	pureTone2Ptr->endPeriodDurationFlag = TRUE;
	pureTone2Ptr->endPeriodDuration = theEndPeriodDuration;
	return(TRUE);

}

/********************************* SetSamplingInterval ************************/

/*
 * This function sets the module's timeStep parameter.  It first checks that
 * the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetSamplingInterval_PureTone_2(double theSamplingInterval)
{
	static const char *funcName = "SetSamplingInterval_PureTone_2";

	if (pureTone2Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ( theSamplingInterval <= 0.0 ) {
		NotifyError("%s: Illegal sampling interval value = %g!", funcName,
		  theSamplingInterval);
		return(FALSE);
	}
	pureTone2Ptr->dtFlag = TRUE;
	pureTone2Ptr->dt = theSamplingInterval;
	return(TRUE);

}

/********************************* SetPars ************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */
 
BOOLN
SetPars_PureTone_2(double theFrequency, double theIntensity, double theDuration,
  double theSamplingInterval, double theBeginPeriodDuration,
  double theEndPeriodDuration)
{
	static const char *funcName = "SetPars_PureTone_2";
	BOOLN	ok;
	
	ok = TRUE;
	if (!SetFrequency_PureTone_2(theFrequency))
		ok = FALSE;
	if (!SetIntensity_PureTone_2(theIntensity))
		ok = FALSE;
	if (!SetDuration_PureTone_2(theDuration))
		ok = FALSE;
	if (!SetSamplingInterval_PureTone_2(theSamplingInterval))
		ok = FALSE;
	if (!SetBeginPeriodDuration_PureTone_2(theBeginPeriodDuration))
		ok = FALSE;
	if (!SetEndPeriodDuration_PureTone_2(theEndPeriodDuration))
		ok = FALSE;
	if (!ok)
		NotifyError("%s: Failed to set all module parameters.", funcName);
	return(ok);
	
}

/****************************** PrintPars *************************************/

/*
 * This program prints the parameters of the module to the standard output.
 */
 
BOOLN
PrintPars_PureTone_2(void)
{
	static const char *funcName = "PrintPars_PureTone_2";

	if (!CheckPars_PureTone_2()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Pure Tone 2 (surrounded by silence) Module "\
	  "Parameters:-\n");
	DPrint("\tFrequency = %g (Hz),\tIntensity = %g dB SPL,\n",
	  pureTone2Ptr->frequency, pureTone2Ptr->intensity);
	DPrint("\tDuration = %g ms,\tSampling interval = %g ms\n",
	  MSEC(pureTone2Ptr->duration), MSEC(pureTone2Ptr->dt));
	DPrint("\tBegin silence = %g ms,\tEnd silence = %g ms\n",
	  MSEC(pureTone2Ptr->beginPeriodDuration),
	  MSEC(pureTone2Ptr->endPeriodDuration));
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.
 */
 
BOOLN
ReadPars_PureTone_2(char *fileName)
{
	static const char *funcName = "ReadPars_PureTone_2";
	BOOLN	ok;
	char	*filePath;
	double  frequency, intensity, beginPeriodDuration, endPeriodDuration;
	double  duration, samplingInterval;
    FILE    *fp;
    
	filePath = GetParsFileFPath_Common(fileName);
    if ((fp = fopen(filePath, "r")) == NULL) {
        NotifyError("%s: Cannot open data file '%s'.\n", funcName, filePath);
		return(FALSE);
    }
    DPrint("%s: Reading from '%s':\n", funcName, filePath);
    Init_ParFile();
	ok = TRUE;
    if (!GetPars_ParFile(fp, "%lf", &frequency))
		ok = FALSE;
    if (!GetPars_ParFile(fp, "%lf", &intensity))
		ok = FALSE;
    if (!GetPars_ParFile(fp, "%lf", &duration))
		ok = FALSE;
    if (!GetPars_ParFile(fp, "%lf", &beginPeriodDuration))
		ok = FALSE;
    if (!GetPars_ParFile(fp, "%lf", &endPeriodDuration))
		ok = FALSE;
    if (!GetPars_ParFile(fp, "%lf", &samplingInterval))
		ok = FALSE;
    fclose(fp);
    Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "\
		  "parameter file '%s'.", funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_PureTone_2(frequency, intensity, duration,
	  samplingInterval, beginPeriodDuration, endPeriodDuration)) {
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
SetParsPointer_PureTone_2(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_PureTone_2";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	pureTone2Ptr = (PureTone2Ptr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_PureTone_2(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_PureTone_2";

	if (!SetParsPointer_PureTone_2(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_PureTone_2(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = pureTone2Ptr;
	theModule->CheckPars = CheckPars_PureTone_2;
	theModule->Free = Free_PureTone_2;
	theModule->GetUniParListPtr = GetUniParListPtr_PureTone_2;
	theModule->PrintPars = PrintPars_PureTone_2;
	theModule->ReadPars = ReadPars_PureTone_2;
	theModule->RunProcess = GenerateSignal_PureTone_2;
	theModule->SetParsPointer = SetParsPointer_PureTone_2;
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
GenerateSignal_PureTone_2(EarObjectPtr data)
{
	static const char *funcName = "GenerateSignal_PureTone_2";
	double		totalDuration, amplitude;
	register	double	dt, time, endSignal, startSignal;
	register	ChanData	*dataPtr;
	ChanLen		i;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}	
	if (!CheckPars_PureTone_2())
		return(FALSE);
	SetProcessName_EarObject(data, "Silence pure-tone stimulus");
	totalDuration = pureTone2Ptr->beginPeriodDuration + pureTone2Ptr->duration +
	  pureTone2Ptr->endPeriodDuration;
	if ( !InitOutSignal_EarObject(data, PURE_TONE_2_NUM_CHANNELS,
	  (ChanLen) floor(totalDuration / pureTone2Ptr->dt + 0.5), pureTone2Ptr->
	   dt) ) {
		NotifyError("%s: Cannot initialise output signal", funcName);
		return(FALSE);
	}
	dt = data->outSignal->dt;
	startSignal = pureTone2Ptr->beginPeriodDuration;
	endSignal = pureTone2Ptr->beginPeriodDuration + pureTone2Ptr->duration;
	amplitude = RMS_AMP(pureTone2Ptr->intensity) * SQRT_2;
	dataPtr = data->outSignal->channel[0];
	for (i = 0, time = dt; i < data->outSignal->length; i++, time += dt,
	  dataPtr++) {
		if ( (time > startSignal) && (time < endSignal) )
			*dataPtr = amplitude * sin(PIx2 * pureTone2Ptr->frequency *
			  (time - pureTone2Ptr->beginPeriodDuration));
		else
			*dataPtr = 0.0;
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

} /* GenerateSignal_PureTone_2 */

