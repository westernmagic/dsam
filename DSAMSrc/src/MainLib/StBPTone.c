/**********************
 *
 * File:		StBPTone.c
 * Purpose:		This module contains the methods for the multiple pure-tone
 *				signal generation paradigm.
 * Comments:	Written using ModuleProducer version 1.2.0 (Nov  6 1998).
 *				06-11-98 LPO: This version of the module was created to make it
 *				easier to create the universal parameter lists.  This is because
 *				the arrays are assumed to have a length of two, rather than
 *				there being a parameter which defined the lengths.
 * Author:		L. P. O'Mard
 * Created:		06 Nov 1998
 * Updated:	
 * Copyright:	(c) 1998, University of Essex.
 *
 *********************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG */

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "FiParFile.h"
#include "UtString.h"
#include "StBPTone.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

BPureTonePtr	bPureTonePtr = NULL;

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
Free_PureTone_Binaural(void)
{
	if (bPureTonePtr == NULL)
		return(FALSE);
	if (bPureTonePtr->parList)
		FreeList_UniParMgr(&bPureTonePtr->parList);
	if (bPureTonePtr->parSpec == GLOBAL) {
		free(bPureTonePtr);
		bPureTonePtr = NULL;
	}
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
Init_PureTone_Binaural(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_PureTone_Binaural");

	if (parSpec == GLOBAL) {
		if (bPureTonePtr != NULL)
			Free_PureTone_Binaural();
		if ((bPureTonePtr = (BPureTonePtr) malloc(sizeof(BPureTone))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"), funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (bPureTonePtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	bPureTonePtr->parSpec = parSpec;
	bPureTonePtr->leftFrequencyFlag = TRUE;
	bPureTonePtr->rightFrequencyFlag = TRUE;
	bPureTonePtr->leftIntensityFlag = TRUE;
	bPureTonePtr->rightIntensityFlag = TRUE;
	bPureTonePtr->phaseDifferenceFlag = TRUE;
	bPureTonePtr->durationFlag = TRUE;
	bPureTonePtr->dtFlag = TRUE;
	bPureTonePtr->leftFrequency = 1000.0;
	bPureTonePtr->rightFrequency = 1000.0;
	bPureTonePtr->leftIntensity = DEFAULT_INTENSITY;
	bPureTonePtr->rightIntensity = DEFAULT_INTENSITY;
	bPureTonePtr->phaseDifference = 180.0;
	bPureTonePtr->duration = 0.1;
	bPureTonePtr->dt = DEFAULT_DT;

	if (!SetUniParList_PureTone_Binaural()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_PureTone_Binaural();
		return(FALSE);
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
SetUniParList_PureTone_Binaural(void)
{
	static const WChar *funcName = wxT("SetUniParList_PureTone_Binaural");
	UniParPtr	pars;

	if ((bPureTonePtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  PURETONE_Binaural_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = bPureTonePtr->parList->pars;
	SetPar_UniParMgr(&pars[PURETONE_Binaural_LEFTFREQUENCY], wxT(
	  "LEFT_FREQUENCY"),
	  wxT("Left channel frequency (Hz)."),
	  UNIPAR_REAL,
	  &bPureTonePtr->leftFrequency, NULL,
	  (void * (*)) SetLeftFrequency_PureTone_Binaural);
	SetPar_UniParMgr(&pars[PURETONE_Binaural_RIGHTFREQUENCY], wxT(
	  "RIGHT_FREQUENCY"),
	  wxT("Right channel frequency (Hz)."),
	  UNIPAR_REAL,
	  &bPureTonePtr->rightFrequency, NULL,
	  (void * (*)) SetRightFrequency_PureTone_Binaural);
	SetPar_UniParMgr(&pars[PURETONE_Binaural_LEFTINTENSITY], wxT(
	  "LEFT_INTENSITY"),
	  wxT("Left channel intensity (dB SPL)."),
	  UNIPAR_REAL,
	  &bPureTonePtr->leftIntensity, NULL,
	  (void * (*)) SetLeftIntensity_PureTone_Binaural);
	SetPar_UniParMgr(&pars[PURETONE_Binaural_RIGHTINTENSITY], wxT(
	  "RIGHT_INTENSITY"),
	  wxT("Right channel intensity (dB SPL)."),
	  UNIPAR_REAL,
	  &bPureTonePtr->rightIntensity, NULL,
	  (void * (*)) SetRightIntensity_PureTone_Binaural);
	SetPar_UniParMgr(&pars[PURETONE_Binaural_PHASEDIFFERENCE], wxT(
	  "PHASE_DIFF"),
	  wxT("Phase difference between channels (degrees)."),
	  UNIPAR_REAL,
	  &bPureTonePtr->phaseDifference, NULL,
	  (void * (*)) SetPhaseDifference_PureTone_Binaural);
	SetPar_UniParMgr(&pars[PURETONE_Binaural_DURATION], wxT("DURATION"),
	  wxT("Duration (s)."),
	  UNIPAR_REAL,
	  &bPureTonePtr->duration, NULL,
	  (void * (*)) SetDuration_PureTone_Binaural);
	SetPar_UniParMgr(&pars[PURETONE_Binaural_SAMPLINGINTERVAL], wxT("DT"),
	  wxT("Sampling interval, dt (s)."),
	  UNIPAR_REAL,
	  &bPureTonePtr->dt, NULL,
	  (void * (*)) SetSamplingInterval_PureTone_Binaural);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_PureTone_Binaural(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_PureTone_Binaural");

	if (bPureTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (bPureTonePtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been "
		  "initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(bPureTonePtr->parList);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_PureTone_Binaural(double leftFrequency, double rightFrequency,
  double leftIntensity, double rightIntensity, double phaseDifference,
  double duration, double samplingInterval)
{
	static const WChar	*funcName = wxT("SetPars_PureTone_Binaural");
	BOOLN	ok;

	ok = TRUE;
	if (!SetLeftFrequency_PureTone_Binaural(leftFrequency))
		ok = FALSE;
	if (!SetRightFrequency_PureTone_Binaural(rightFrequency))
		ok = FALSE;
	if (!SetLeftIntensity_PureTone_Binaural(leftIntensity))
		ok = FALSE;
	if (!SetRightIntensity_PureTone_Binaural(rightIntensity))
		ok = FALSE;
	if (!SetPhaseDifference_PureTone_Binaural(phaseDifference))
		ok = FALSE;
	if (!SetDuration_PureTone_Binaural(duration))
		ok = FALSE;
	if (!SetSamplingInterval_PureTone_Binaural(samplingInterval))
		ok = FALSE;
	if (!ok)
		NotifyError(wxT("%s: Failed to set all module parameters.") ,funcName);
	return(ok);

}

/****************************** SetLeftFrequency ******************************/

/*
 * This function sets the module's leftFrequency parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLeftFrequency_PureTone_Binaural(double theLeftFrequency)
{
	static const WChar	*funcName = wxT("SetLeftFrequency_PureTone_Binaural");

	if (bPureTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bPureTonePtr->leftFrequencyFlag = TRUE;
	bPureTonePtr->leftFrequency = theLeftFrequency;
	return(TRUE);

}

/****************************** SetRightFrequency *****************************/

/*
 * This function sets the module's rightFrequency parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRightFrequency_PureTone_Binaural(double theRightFrequency)
{
	static const WChar	*funcName = wxT("SetRightFrequency_PureTone_Binaural");

	if (bPureTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bPureTonePtr->rightFrequencyFlag = TRUE;
	bPureTonePtr->rightFrequency = theRightFrequency;
	return(TRUE);

}

/****************************** SetLeftIntensity ******************************/

/*
 * This function sets the module's leftIntensity parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLeftIntensity_PureTone_Binaural(double theLeftIntensity)
{
	static const WChar	*funcName = wxT("SetLeftIntensity_PureTone_Binaural");

	if (bPureTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bPureTonePtr->leftIntensityFlag = TRUE;
	bPureTonePtr->leftIntensity = theLeftIntensity;
	return(TRUE);

}

/****************************** SetRightIntensity *****************************/

/*
 * This function sets the module's rightIntensity parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRightIntensity_PureTone_Binaural(double theRightIntensity)
{
	static const WChar	*funcName = wxT("SetRightIntensity_PureTone_Binaural");

	if (bPureTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bPureTonePtr->rightIntensityFlag = TRUE;
	bPureTonePtr->rightIntensity = theRightIntensity;
	return(TRUE);

}

/****************************** SetPhaseDifference ****************************/

/*
 * This function sets the module's phaseDifference parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPhaseDifference_PureTone_Binaural(double thePhaseDifference)
{
	static const WChar	*funcName = wxT("SetPhaseDifference_PureTone_Binaural");

	if (bPureTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bPureTonePtr->phaseDifferenceFlag = TRUE;
	bPureTonePtr->phaseDifference = thePhaseDifference;
	return(TRUE);

}

/****************************** SetDuration ***********************************/

/*
 * This function sets the module's duration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDuration_PureTone_Binaural(double theDuration)
{
	static const WChar	*funcName = wxT("SetDuration_PureTone_Binaural");

	if (bPureTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bPureTonePtr->durationFlag = TRUE;
	bPureTonePtr->duration = theDuration;
	return(TRUE);

}

/****************************** SetSamplingInterval ***************************/

/*
 * This function sets the module's samplingInterval parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSamplingInterval_PureTone_Binaural(double theSamplingInterval)
{
	static const WChar	*funcName = wxT(
	  "SetSamplingInterval_PureTone_Binaural");

	if (bPureTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ( theSamplingInterval <= 0.0 ) {
		NotifyError(wxT("%s: Illegal sampling interval value = %g!"), funcName,
		  theSamplingInterval);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bPureTonePtr->dtFlag = TRUE;
	bPureTonePtr->dt = theSamplingInterval;
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
CheckPars_PureTone_Binaural(void)
{
	static const WChar	*funcName = wxT("CheckPars_PureTone_Binaural");
	BOOLN	ok;
	double	criticalFrequency;

	ok = TRUE;
	if (bPureTonePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (!bPureTonePtr->leftFrequencyFlag) {
		NotifyError(wxT("%s: leftFrequency variable not set."), funcName);
		ok = FALSE;
	}
	if (!bPureTonePtr->rightFrequencyFlag) {
		NotifyError(wxT("%s: rightFrequency variable not set."), funcName);
		ok = FALSE;
	}
	if (!bPureTonePtr->leftIntensityFlag) {
		NotifyError(wxT("%s: leftIntensity variable not set."), funcName);
		ok = FALSE;
	}
	if (!bPureTonePtr->rightIntensityFlag) {
		NotifyError(wxT("%s: rightIntensity variable not set."), funcName);
		ok = FALSE;
	}
	if (!bPureTonePtr->phaseDifferenceFlag) {
		NotifyError(wxT("%s: phaseDifference variable not set."), funcName);
		ok = FALSE;
	}
	if (!bPureTonePtr->durationFlag) {
		NotifyError(wxT("%s: duration variable not set."), funcName);
		ok = FALSE;
	}
	if (!bPureTonePtr->dtFlag) {
		NotifyError(wxT("%s: dt variable not set."), funcName);
		ok = FALSE;
	}
	criticalFrequency = 1.0 / (2.0 * bPureTonePtr->dt);
	if (ok && ((criticalFrequency <= bPureTonePtr->leftFrequency) ||
	  (criticalFrequency <= bPureTonePtr->rightFrequency))) {
		NotifyError(wxT("%s: Sampling rate (dt = %g ms) is too low for one of "
		  "the stimulus\nfrequencies."), funcName, MSEC(bPureTonePtr->dt));
		ok = FALSE;
	} 
	return(ok);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the lUTEar.parsFile file pointer.
 */

BOOLN
PrintPars_PureTone_Binaural(void)
{
	static const WChar	*funcName = wxT("PrintPars_PureTone_Binaural");

	if (!CheckPars_PureTone_Binaural()) {
		NotifyError(wxT("%s: Parameters have not been correctly set."),
		  funcName);
		return(FALSE);
	}
	DPrint(wxT("Binaural Pure Tone Module Parameters:-\n"));
	DPrint(wxT("\tFrequency: left / right = %g / %g (Hz)\n"),
	  bPureTonePtr->leftFrequency, bPureTonePtr->rightFrequency);
	DPrint(wxT("\tIntensity: left / right = %g / %g (dB SPL)\n"),
	  bPureTonePtr->leftIntensity, bPureTonePtr->rightIntensity);
	DPrint(wxT("\tPhase difference = %g degrees\n"), bPureTonePtr->
	  phaseDifference);
	DPrint(wxT("\tDuration = %g ms, "), MILLI(bPureTonePtr->duration));
	DPrint(wxT("\tSamplingInterval = %g ms\n"), MILLI(bPureTonePtr->dt));
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_PureTone_Binaural(WChar *fileName)
{
	static const WChar	*funcName = wxT("ReadPars_PureTone_Binaural");
	BOOLN	ok;
	WChar	*filePath;
	double	leftFrequency, rightFrequency, leftIntensity, rightIntensity;
	double	phaseDifference, duration, samplingInterval;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(ConvUTF8_Utility_String(filePath), "r")) == NULL) {
		NotifyError(wxT("%s: Cannot open data file '%s'.\n"), funcName,
		  filePath);
		return(FALSE);
	}
	DPrint(wxT("%s: Reading from '%s':\n"), funcName, filePath);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, wxT("%lf %lf"), &leftFrequency, &leftIntensity))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf %lf"), &rightFrequency, &rightIntensity))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &phaseDifference))
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
	if (!SetPars_PureTone_Binaural(leftFrequency, rightFrequency,
	  leftIntensity, rightIntensity, phaseDifference, duration,
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
SetParsPointer_PureTone_Binaural(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_PureTone_Binaural");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	bPureTonePtr = (BPureTonePtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_PureTone_Binaural(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_PureTone_Binaural");

	if (!SetParsPointer_PureTone_Binaural(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_PureTone_Binaural(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = bPureTonePtr;
	theModule->CheckPars = CheckPars_PureTone_Binaural;
	theModule->Free = Free_PureTone_Binaural;
	theModule->GetUniParListPtr = GetUniParListPtr_PureTone_Binaural;
	theModule->PrintPars = PrintPars_PureTone_Binaural;
	theModule->ReadPars = ReadPars_PureTone_Binaural;
	theModule->RunProcess = GenerateSignal_PureTone_Binaural;
	theModule->SetParsPointer = SetParsPointer_PureTone_Binaural;
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
CheckData_PureTone_Binaural(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_PureTone_Binaural");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	/*** Put additional checks here. ***/
	return(TRUE);

}

/****************************** GenerateSignal ********************************/

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
 */

BOOLN
GenerateSignal_PureTone_Binaural(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("GenerateSignal_PureTone_Binaural");
	int			j;
	ChanLen		i, t;
	register	double		amplitude, phase, frequency, intensity;
	register	ChanData	*dataPtr;

	if (!data->threadRunFlag) {
		if (!CheckPars_PureTone_Binaural())
			return(FALSE);
		if (!CheckData_PureTone_Binaural(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Binaural pure tone stimulus"));
		if ( !InitOutSignal_EarObject(data, BINAURAL_PTONE_CHANNELS,
		  (ChanLen) floor(bPureTonePtr->duration / bPureTonePtr->dt + 0.5),
	    	bPureTonePtr->dt) ) {
			NotifyError(wxT("%s: Cannot initialise output signal."), funcName);
			return(FALSE);
		}
		SetInterleaveLevel_SignalData(data->outSignal, 2);
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	phase = bPureTonePtr->phaseDifference * PIx2 / 360.0;
	for (j = 0; j < BINAURAL_PTONE_CHANNELS; j++) {
		if (j == 0) {
			frequency = bPureTonePtr->leftFrequency;
			intensity = bPureTonePtr->leftIntensity;
		} else {
			frequency = bPureTonePtr->rightFrequency;
			intensity = bPureTonePtr->rightIntensity;
		}
		amplitude = RMS_AMP(intensity) * SQRT_2;
		dataPtr = data->outSignal->channel[j];
		for (i = 0, t = data->timeIndex + 1; i < data->outSignal->length; i++,
		  t++)
			*(dataPtr++) = (ChanData) (amplitude * sin(PIx2 * frequency * (t *
			data->outSignal->dt) + phase * j));
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

