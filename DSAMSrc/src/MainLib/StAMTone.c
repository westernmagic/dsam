/**********************
 *
 * File:		StAMTone.c
 * Purpose:		This module contains the methods for the AM-tone (amplitude
 *				modulated) generation paradigm.
 * Comments:	09-07-98 LPO: Carrier frequency is now in sine phase and not cos
 *				phase.
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		09 Jul 1998
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
#include "StAMTone.h"
#include "UtString.h"
#include "FiParFile.h"

/********************************* Global variables ***************************/

AMTonePtr	aMTonePtr = NULL;

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
Init_PureTone_AM(ParameterSpecifier parSpec)
{
	static const WChar *funcName = wxT("Init_PureTone_AM");

	if (parSpec == GLOBAL) {
		if (aMTonePtr != NULL)
			Free_PureTone_AM();
		if ((aMTonePtr = (AMTonePtr) malloc(sizeof(AMTone))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (aMTonePtr == NULL) { 
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	aMTonePtr->parSpec = parSpec;
	aMTonePtr->frequencyFlag = TRUE;
	aMTonePtr->modulationFrequencyFlag = TRUE;
	aMTonePtr->modulationDepthFlag = TRUE;
	aMTonePtr->durationFlag = TRUE;
	aMTonePtr->dtFlag = TRUE;
	aMTonePtr->intensityFlag = TRUE;
	aMTonePtr->frequency = 1000.0;
	aMTonePtr->modulationFrequency = 50.0;
	aMTonePtr->modulationDepth = 100.0;
	aMTonePtr->intensity = DEFAULT_INTENSITY;
	aMTonePtr->duration = 0.1;
	aMTonePtr->dt = 0.1e-5;

	if (!SetUniParList_PureTone_AM()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_PureTone_AM();
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
Free_PureTone_AM(void)
{
	if (aMTonePtr == NULL)
		return(TRUE);
	if (aMTonePtr->parList)
		FreeList_UniParMgr(&aMTonePtr->parList);
	if (aMTonePtr->parSpec == GLOBAL) {
		free(aMTonePtr);
		aMTonePtr = NULL;
	}
	return(TRUE);

}

/********************************* SetUniParList ******************************/

/*
 * This routine initialises and sets the module's universal parameter list.
 * This list provides universal access to the module's parameters.
 */
 
BOOLN
SetUniParList_PureTone_AM(void)
{
	static const WChar *funcName = wxT("SetUniParList_PureTone_AM");
	UniParPtr	pars;

	if ((aMTonePtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  PURETONE_AM_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = aMTonePtr->parList->pars;
	SetPar_UniParMgr(&pars[AM_TONE_FREQUENCY], wxT("CARRIER_FREQ"),
	  wxT("Carrier frequency (Hz)."),
	  UNIPAR_REAL,
	  &aMTonePtr->frequency, NULL,
	  (void * (*)) SetFrequency_PureTone_AM);
	SetPar_UniParMgr(&pars[AM_TONE_MODULATIONFREQUENCY],wxT("MOD_FREQUENCY"),
	  wxT("Modulation frequency (Hz)."),
	  UNIPAR_REAL,
	  &aMTonePtr->modulationFrequency, NULL,
	  (void * (*)) SetModulationFrequency_PureTone_AM);
	SetPar_UniParMgr(&pars[AM_TONE_MODULATIONDEPTH], wxT("DEPTH"),
	  wxT("Amplitude modulation depth (%)."),
	  UNIPAR_REAL,
	  &aMTonePtr->modulationDepth, NULL,
	  (void * (*)) SetModulationDepth_PureTone_AM);
	SetPar_UniParMgr(&pars[AM_TONE_INTENSITY], wxT("INTENSITY"),
	  wxT("Intensity (dB SPL)."),
	  UNIPAR_REAL,
	  &aMTonePtr->intensity, NULL,
	  (void * (*)) SetIntensity_PureTone_AM);
	SetPar_UniParMgr(&pars[AM_TONE_DURATION], wxT("DURATION"),
	  wxT("Duration (s)."),
	  UNIPAR_REAL,
	  &aMTonePtr->duration, NULL,
	  (void * (*)) SetDuration_PureTone_AM);
	SetPar_UniParMgr(&pars[AM_TONE_SAMPLINGINTERVAL], wxT("DT"),
	  wxT("Sampling interval, dt (s)."),
	  UNIPAR_REAL,
	  &aMTonePtr->dt, NULL,
	  (void * (*)) SetSamplingInterval_PureTone_AM);
	return(TRUE);

}

/********************************* GetUniParListPtr ***************************/

/*
 * This routine returns a pointer to the module's universal parameter list.
 */

UniParListPtr
GetUniParListPtr_PureTone_AM(void)
{
	static const WChar *funcName = wxT("GetUniParListPtr_PureTone_AM");

	if (aMTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (aMTonePtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been "
		  "initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(aMTonePtr->parList);

}

/********************************* CheckPars **********************************/

/*
 * This routine checks that the necessary parameters for the module have been
 * correctly initialised.
 * It also checks that the Nyquist critical frequency is not exceeded.
 * It returns TRUE if there are no problems.
 */
 
BOOLN
CheckPars_PureTone_AM(void)
{
	static const WChar *funcName = wxT("CheckPars_PureTone_AM");
	BOOLN	ok;
	double	criticalFrequency;
	
	ok = TRUE;
	if (aMTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (!aMTonePtr->frequencyFlag) {
		NotifyError(wxT("%s: Carrier frequency variable not set."), funcName);
		ok = FALSE;
	}	
	if (!aMTonePtr->modulationFrequencyFlag) {
		NotifyError(wxT("%s: Modulation frequency variable not set."),
		  funcName);
		ok = FALSE;
	}
	if (!aMTonePtr->modulationDepthFlag) {
		NotifyError(wxT("%s: Percent AM variable not set."), funcName);
		ok = FALSE;
	}
		if (!aMTonePtr->intensityFlag) {
		NotifyError(wxT("%s: intensity variable not set."), funcName);
		ok = FALSE;
	}
	if (!aMTonePtr->durationFlag) {
		NotifyError(wxT("%s: duration variable not set."), funcName);
		ok = FALSE;
	}
	if (!aMTonePtr->dtFlag) {
		NotifyError(wxT("%s: dtFlag variable not set."), funcName);
		ok = FALSE;
	}
	criticalFrequency = 1.0 / (2.0 * aMTonePtr->dt);
	if (ok && (criticalFrequency <= aMTonePtr->frequency)) {
		NotifyError(wxT("%s: Sampling rate (dt = %g ms) is to low for the "
		  "carrier frequency."), funcName, MSEC(aMTonePtr->dt));
		ok = FALSE;
	} 
	if (ok && (criticalFrequency <= aMTonePtr->modulationFrequency)) {
		NotifyError(wxT("%s: Sampling rate (dt = %g ms) is too low for the "
		  "modulation frequency."), funcName, MSEC(aMTonePtr->dt));
		ok = FALSE;
	} 
	return(ok);
	
}	

/********************************* SetFrequency *******************************/

/*
 * This function sets the module's frequency parameter.  
 * It returns TRUE if the operation is successful.
 */
 
BOOLN
SetFrequency_PureTone_AM(double theFrequency)
{
	static const WChar *funcName = wxT("SetFrequency_PureTone_AM");

	if (aMTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	aMTonePtr->frequencyFlag = TRUE;
	aMTonePtr->frequency = theFrequency;
	return(TRUE);

}

/********************************* SetModulationFrequency *********************/

/*
 * This function sets the module's modulationFrequency parameter.  
 * It returns TRUE if the operation is successful.
 */
 
BOOLN
SetModulationFrequency_PureTone_AM(double theModulationFrequency)
{
	static const WChar *funcName = wxT("SetModulationFrequency_PureTone_AM");

	if (aMTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	aMTonePtr->modulationFrequencyFlag = TRUE;
	aMTonePtr->modulationFrequency = theModulationFrequency;
	return(TRUE);

}

/********************************* SetModulationDepth *************************/

/*
 * This function sets the module's modulationDepth parameter.  
 * It returns TRUE if the operation is successful.
 */
 
BOOLN
SetModulationDepth_PureTone_AM(double theModulationDepth)
{
	static const WChar *funcName = wxT("SetModulationDepth_PureTone_AM");

	if (aMTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	aMTonePtr->modulationDepthFlag = TRUE;
	aMTonePtr->modulationDepth = theModulationDepth;
	return(TRUE);

}

/********************************* SetIntensity *******************************/

/*
 * This function sets the module's intensity parameter.
 * It returns TRUE if the operation is successful.
 */


BOOLN
SetIntensity_PureTone_AM(double theIntensity)
{
	static const WChar *funcName = wxT("SetIntensity_PureTone_AM");

	if (aMTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	aMTonePtr->intensityFlag = TRUE;
	aMTonePtr->intensity = theIntensity;
	return(TRUE);

}

/********************************* SetDuration ********************************/

/*
 * This function sets the module's duration parameter.
 * It returns TRUE if the operation is successful.
 */
 
BOOLN
SetDuration_PureTone_AM(double theDuration)
{
	static const WChar *funcName = wxT("SetDuration_PureTone_AM");

	if (aMTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	aMTonePtr->durationFlag = TRUE;
	aMTonePtr->duration = theDuration;
	return(TRUE);

}

/********************************* SetSamplingInterval ************************/

/*
 * This function sets the module's samplingInterval parameter.
 * It returns TRUE if the operation is successful.
 */
 
BOOLN
SetSamplingInterval_PureTone_AM(double theSamplingInterval)
{
	static const WChar *funcName = wxT("SetSamplingInterval_PureTone_AM");

	if (aMTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ( theSamplingInterval <= 0.0 ) {
		NotifyError(wxT("%s: Illegal sampling interval value = %g!"), funcName,
		  theSamplingInterval);
		return(FALSE);
	}
	aMTonePtr->dtFlag = TRUE;
	aMTonePtr->dt = theSamplingInterval;
	return(TRUE);

}

/********************************* SetPars ************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */
 
BOOLN
SetPars_PureTone_AM(double theFrequency, double theModulationFrequency,
  double theModulationDepth, double theIntensity, double theDuration,
  double theSamplingInterval)
{
	static const WChar *funcName = wxT("SetPars_PureTone_AM");
	BOOLN	ok;
	
	ok = TRUE;
	if (!SetFrequency_PureTone_AM(theFrequency))
		ok = FALSE;
	if (!SetModulationFrequency_PureTone_AM(theModulationFrequency))
		ok = FALSE;
	if (!SetModulationDepth_PureTone_AM(theModulationDepth))
		ok = FALSE;
	if (!SetIntensity_PureTone_AM(theIntensity))
		ok = FALSE;
	if (!SetDuration_PureTone_AM(theDuration))
		ok = FALSE;
	if (!SetSamplingInterval_PureTone_AM(theSamplingInterval))
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
PrintPars_PureTone_AM(void)
{
	static const WChar *funcName = wxT("PrintPars_PureTone_AM");

	if (!CheckPars_PureTone_AM()) {
		NotifyError(wxT("%s: Parameters have not been correctly set."),
		  funcName);
		return(FALSE);
	}
	DPrint(wxT("Amplitude Modulated Tone Module Parameters:-\n"));
	DPrint(wxT("\tCarrier frequency = %g Hz,\tModulation frequency = %g\n"),
	  aMTonePtr->frequency, aMTonePtr->modulationFrequency);
	DPrint(wxT("\tPecentage Amplitude = %g,\tIntensity = %g dB SPL,\n"),
	  aMTonePtr->modulationDepth, aMTonePtr->intensity);
	DPrint(wxT("\tDuration = %g ms,\tSampling interval = %g ms\n"),
	  MSEC(aMTonePtr->duration), MSEC(aMTonePtr->dt));
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.
 */
 
BOOLN
ReadPars_PureTone_AM(WChar *fileName)
{
	static const WChar *funcName = wxT("ReadPars_PureTone_AM");
	BOOLN	ok;
	WChar	*filePath;
	double  frequency, modulationFrequency, modulationDepth, intensity;
	double  duration, samplingInterval;
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
	if (!GetPars_ParFile(fp, wxT("%lf"), &modulationFrequency))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &modulationDepth))
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
		NotifyError(wxT("%s:  Not enough, lines or invalid parameters, in "
		  "module parameter file '%s'."), funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_PureTone_AM(frequency, modulationFrequency, modulationDepth,
	  intensity, duration, samplingInterval)) {
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
SetParsPointer_PureTone_AM(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_PureTone_AM");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	aMTonePtr = (AMTonePtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_PureTone_AM(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_PureTone_AM");

	if (!SetParsPointer_PureTone_AM(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_PureTone_AM(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = aMTonePtr;
	theModule->CheckPars = CheckPars_PureTone_AM;
	theModule->Free = Free_PureTone_AM;
	theModule->GetUniParListPtr = GetUniParListPtr_PureTone_AM;
	theModule->PrintPars = PrintPars_PureTone_AM;
	theModule->ReadPars = ReadPars_PureTone_AM;
	theModule->RunProcess = GenerateSignal_PureTone_AM;
	theModule->SetParsPointer = SetParsPointer_PureTone_AM;
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
GenerateSignal_PureTone_AM(EarObjectPtr data)
{
	static const WChar *funcName = wxT("GenerateSignal_PureTone_AM");
	ChanLen		i, t;
	double 		modulationIndex;
	register	double		time, amplitude;
	register	ChanData	*dataPtr;

	if (!data->threadRunFlag) {
		if (data == NULL) {
			NotifyError(wxT("%s: EarObject not initialised."), funcName);
			return(FALSE);
		}	
		if (!CheckPars_PureTone_AM())
			return(FALSE);
		SetProcessName_EarObject(data, wxT("AM Tone stimulus"));
		if ( !InitOutSignal_EarObject(data, AM_TONE_NUM_CHANNELS,
		  (ChanLen) floor(aMTonePtr->duration / aMTonePtr->dt + 0.5),
		  aMTonePtr->dt) ) {
			NotifyError(wxT("%s: Cannot initialise output signal"), funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	amplitude = RMS_AMP(aMTonePtr->intensity) * SQRT_2;
	modulationIndex = aMTonePtr->modulationDepth / 100.0;
	dataPtr = _OutSig_EarObject(data)->channel[0];
	for (i = 0, t = data->timeIndex + 1; i < _OutSig_EarObject(data)->length; i++,
	  t++) {
		time = t * _OutSig_EarObject(data)->dt;
		*(dataPtr++) = amplitude * (1.0 + modulationIndex * sin(PIx2 *
		  aMTonePtr->modulationFrequency * time)) *
		  sin(PIx2 * aMTonePtr->frequency * time);
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

} /* GenerateSignal_PureTone_AM */




