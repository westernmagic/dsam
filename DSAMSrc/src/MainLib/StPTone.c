/**********************
 *
 * File:		StPTone.c
 * Purpose:		This module contains the methods for the simple pure-tone
 *				signal generation paradigm.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		12 Mar 1997
 * Copyright:	(c) 1998,  University of Essex
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
#include "StPTone.h"

/********************************* Global variables ***************************/

PureTonePtr	pureTonePtr = NULL;

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
Init_PureTone(ParameterSpecifier parSpec)
{
	static const WChar *funcName = wxT("Init_PureTone");

	if (parSpec == GLOBAL) {
		if (pureTonePtr != NULL)
			Free_PureTone();
		if ((pureTonePtr = (PureTonePtr) malloc(sizeof(PureTone))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"), funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (pureTonePtr == NULL) { 
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	pureTonePtr->parSpec = parSpec;
	pureTonePtr->frequencyFlag = TRUE;
	pureTonePtr->durationFlag = TRUE;
	pureTonePtr->dtFlag = TRUE;
	pureTonePtr->intensityFlag = TRUE;
	pureTonePtr->frequency = 1000.0;
	pureTonePtr->intensity = DEFAULT_INTENSITY;
	pureTonePtr->duration = 0.1;
	pureTonePtr->dt = DEFAULT_DT;
	
	if (!SetUniParList_PureTone()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_PureTone();
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
Free_PureTone(void)
{
	if (pureTonePtr == NULL)
		return(TRUE);
	if (pureTonePtr->parList)
		FreeList_UniParMgr(&pureTonePtr->parList);
	if (pureTonePtr->parSpec == GLOBAL) {
		free(pureTonePtr);
		pureTonePtr = NULL;
	}
	return(TRUE);

}

/********************************* SetUniParList ******************************/

/*
 * This routine initialises and sets the module's universal parameter list.
 * This list provides universal access to the module's parameters.
 */
 
BOOLN
SetUniParList_PureTone(void)
{
	static const WChar *funcName = wxT("SetUniParList_PureTone");
	UniParPtr	pars;

	if ((pureTonePtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  PURETONE_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = pureTonePtr->parList->pars;
	SetPar_UniParMgr(&pars[PURETONE_FREQUENCY], wxT("FREQUENCY"),
	  wxT("Frequency (Hz)."),
	  UNIPAR_REAL,
	  &pureTonePtr->frequency, NULL,
	  (void * (*)) SetFrequency_PureTone);
	SetPar_UniParMgr(&pars[PURETONE_INTENSITY], wxT("INTENSITY"),
	  wxT("Intensity (dB SPL)."),
	  UNIPAR_REAL,
	  &pureTonePtr->intensity, NULL,
	  (void *(*)) SetIntensity_PureTone);
	SetPar_UniParMgr(&pars[PURETONE_DURATION], wxT("DURATION"),
	  wxT("Duration (s)."),
	  UNIPAR_REAL,
	  &pureTonePtr->duration, NULL,
	  (void *(*)) SetDuration_PureTone);
	SetPar_UniParMgr(&pars[PURETONE_SAMPLINGINTERVAL], wxT("DT"),
	  wxT("Sampling interval, dt (s)."),
	  UNIPAR_REAL,
	  &pureTonePtr->dt, NULL,
	  (void *(*)) SetSamplingInterval_PureTone);
	return(TRUE);

}

/********************************* GetUniParListPtr ***************************/

/*
 * This routine returns a pointer to the module's universal parameter list.
 */

UniParListPtr
GetUniParListPtr_PureTone(void)
{
	static const WChar *funcName = wxT("GetUniParListPtr_PureTone");

	if (pureTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (pureTonePtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been "
		  "initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(pureTonePtr->parList);

}

/********************************* CheckPars **********************************/

/*
 * This routine checks that the necessary parameters for the module have been
 * correctly initialised.
 * It also checks that the Nyquist critical frequency is not exceeded.
 * It returns TRUE if there are no problems.
 */
 
BOOLN
CheckPars_PureTone(void)
{
	static const WChar *funcName = wxT("CheckPars_PureTone");
	BOOLN	ok;
	double	criticalFrequency;
	
	ok = TRUE;
	if (pureTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (!pureTonePtr->frequencyFlag) {
		NotifyError(wxT("%s: Frequency variable not set."), funcName);
		ok = FALSE;
	}
	if (!pureTonePtr->intensityFlag) {
		NotifyError(wxT("%s: intensity variable not set."), funcName);
		ok = FALSE;
	}
	if (!pureTonePtr->durationFlag) {
		NotifyError(wxT("%s: duration variable not set."), funcName);
		ok = FALSE;
	}
	if (!pureTonePtr->dtFlag) {
		NotifyError(wxT("%s: dt variable not set."), funcName);
		ok = FALSE;
	}
	criticalFrequency = 1.0 / (2.0 * pureTonePtr->dt);
	if (ok && (criticalFrequency <= pureTonePtr->frequency)) {
		NotifyError(wxT("%s: Sampling rate (dt = %g ms) is too low for the "
		  "frequency."), funcName, MSEC(pureTonePtr->dt));
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
SetFrequency_PureTone(double theFrequency)
{
	static const WChar *funcName = wxT("SetFrequency_PureTone");

	if (pureTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theFrequency <= 0.0) {
		NotifyError(wxT("%s: Frequency must be greater than zero (%g Hz)."),
		  funcName, theFrequency);
		return(FALSE);
	}
	pureTonePtr->frequencyFlag = TRUE;
	pureTonePtr->frequency = theFrequency;
	return(TRUE);

}

/********************************* SetIntensity *******************************/

/*
 * This function sets the module's intensity parameter.  It first checks that
 * the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetIntensity_PureTone(double theIntensity)
{
	static const WChar *funcName = wxT("SetIntensity_PureTone");

	if (pureTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	pureTonePtr->intensityFlag = TRUE;
	pureTonePtr->intensity = theIntensity;
	return(TRUE);

}

/********************************* SetDuration ********************************/

/*
 * This function sets the module's duration parameter.  It first checks that
 * the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetDuration_PureTone(double theDuration)
{
	static const WChar *funcName = wxT("SetDuration_PureTone");

	if (pureTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	pureTonePtr->durationFlag = TRUE;
	pureTonePtr->duration = theDuration;
	return(TRUE);

}

/********************************* SetSamplingInterval ************************/

/*
 * This function sets the module's samplingInterval parameter.  It first checks
 * that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetSamplingInterval_PureTone(double theSamplingInterval)
{
	static const WChar *funcName = wxT("SetSamplingInterval_PureTone");

	if (pureTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ( theSamplingInterval <= 0.0 ) {
		NotifyError(wxT("%s: Illegal sampling interval value = %g!"), funcName,
		  theSamplingInterval);
		return(FALSE);
	}
	pureTonePtr->dtFlag = TRUE;
	pureTonePtr->dt = theSamplingInterval;
	return(TRUE);

}

/********************************* SetPars ************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */
 
BOOLN
SetPars_PureTone(double theFrequency, double theIntensity, double theDuration,
  double theSamplingInterval)
{
	static const WChar *funcName = wxT("SetPars_PureTone");
	BOOLN	ok;
	
	ok = TRUE;
	if (!SetFrequency_PureTone(theFrequency))
		ok = FALSE;
	if (!SetIntensity_PureTone(theIntensity))
		ok = FALSE;
	if (!SetDuration_PureTone(theDuration))
		ok = FALSE;
	if (!SetSamplingInterval_PureTone(theSamplingInterval))
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
PrintPars_PureTone(void)
{
	static const WChar *funcName = wxT("PrintPars_PureTone");
	if (!CheckPars_PureTone()) {
		NotifyError(wxT("%s: Parameters have not been correctly set."),
		  funcName);
		return(FALSE);
	}
	DPrint(wxT("Pure Tone Module Parameters:-\n"));
	DPrint(wxT("\tFrequency = %g Hz,\tIntensity = %g dB SPL,\n"),
	  pureTonePtr->frequency, pureTonePtr->intensity);
	DPrint(wxT("\tDuration = %g ms,\tSampling interval = %g ms\n"),
	  MSEC(pureTonePtr->duration), MSEC(pureTonePtr->dt));
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.
 */
 
BOOLN
ReadPars_PureTone(WChar *fileName)
{
	static const WChar *funcName = wxT("ReadPars_PureTone");
	BOOLN	ok;
	WChar	*filePath;
	double  frequency, intensity;
	double  duration, samplingInterval;
    FILE    *fp;
    
	filePath = GetParsFileFPath_Common(fileName);
    if ((fp = fopen(ConvUTF8_Utility_String(filePath), "r")) == NULL) {
        NotifyError(wxT("%s: Cannot open data file '%s'.\n"), funcName, filePath);
		return(FALSE);
    }
    DPrint(wxT("%s: Reading from '%s':\n"), funcName, filePath);
    Init_ParFile();
	ok = TRUE;
    if (!GetPars_ParFile(fp, wxT("%lf"), &frequency))
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
		NotifyError(wxT("%s: Not enough lines, or invalid parameters, in "
		  "module parameter file '%s'."), funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_PureTone(frequency, intensity, duration,
	  samplingInterval)) {
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
SetParsPointer_PureTone(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_PureTone");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	pureTonePtr = (PureTonePtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 * It also initialises the process structure.
 */

BOOLN
InitModule_PureTone(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_PureTone");

	if (!SetParsPointer_PureTone(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_PureTone(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = pureTonePtr;
	theModule->CheckPars = CheckPars_PureTone;
	theModule->Free = Free_PureTone;
	theModule->GetUniParListPtr = GetUniParListPtr_PureTone;
	theModule->PrintPars = PrintPars_PureTone;
	theModule->ReadPars = ReadPars_PureTone;
	theModule->RunProcess = GenerateSignal_PureTone;
	theModule->SetParsPointer = SetParsPointer_PureTone;
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
GenerateSignal_PureTone(EarObjectPtr data)
{
	static const WChar *funcName = wxT("GenerateSignal_PureTone");
	ChanLen		i, t;
	register	double		amplitude;
	register	ChanData	*dataPtr;

	if (!data->threadRunFlag) {
		if (data == NULL) {
			NotifyError(wxT("%s: EarObject not initialised."), funcName);
			return(FALSE);
		}	
		if (!CheckPars_PureTone())
			return(FALSE);
		SetProcessName_EarObject(data, wxT("Pure Tone stimulus"));
		if ( !InitOutSignal_EarObject(data, PURETONE_NUM_CHANNELS,
		  (ChanLen) floor(pureTonePtr->duration / pureTonePtr->dt + 0.5),
		  pureTonePtr->dt) ) {
			NotifyError(wxT("%s: Cannot initialise output signal"), funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	amplitude = RMS_AMP(pureTonePtr->intensity) * SQRT_2;
	dataPtr = data->outSignal->channel[0];
	for (i = 0, t = data->timeIndex + 1; i < data->outSignal->length; i++, t++)
		*(dataPtr++) = amplitude * sin(PIx2 * pureTonePtr->frequency *
		  (t * data->outSignal->dt));
	SetProcessContinuity_EarObject(data);
	return(TRUE);

} /* GenerateSignal_PureTone */

