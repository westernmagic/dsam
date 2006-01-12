/**********************
 *
 * File:		StEGatedTone.c
 * Purpose:		This module generates exponential damped or ramped sinusoids.
 * Comments:	Written using ModuleProducer version 1.12 (Oct 12 1997).
 * Author:		L. P. O'Mard
 * Created:		14 Dec 1997
 * Updated:	
 * Copyright:	(c) 1998, University of Essex.
 *
 *********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "GeNSpecLists.h"
#include "FiParFile.h"
#include "StEGatedTone.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

EGatedTonePtr	eGatedTonePtr = NULL;

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
Free_Stimulus_ExpGatedTone(void)
{
	/* static const char	*funcName = "Free_Stimulus_ExpGatedTone";  */

	if (eGatedTonePtr == NULL)
		return(FALSE);
	if (eGatedTonePtr->parList)
		FreeList_UniParMgr(&eGatedTonePtr->parList);
	if (eGatedTonePtr->parSpec == GLOBAL) {
		free(eGatedTonePtr);
		eGatedTonePtr = NULL;
	}
	return(TRUE);

}

/****************************** InitTypeModeList ******************************/

/*
 * This routine intialises the Type Mode list array.
 */

BOOLN
InitTypeModeList_Stimulus_ExpGatedTone(void)
{
	static NameSpecifier	typeModeList[] = {

					{"RAMPED",	EGATED_TONE_RAMPED_MODE },
					{"DAMPED",	EGATED_TONE_DAMPED_MODE },
					{"",		EGATED_TONE_MODE_NULL }

				};
	eGatedTonePtr->typeModeList = typeModeList;
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
Init_Stimulus_ExpGatedTone(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_Stimulus_ExpGatedTone";

	if (parSpec == GLOBAL) {
		if (eGatedTonePtr != NULL)
			Free_Stimulus_ExpGatedTone();
		if ((eGatedTonePtr = (EGatedTonePtr) malloc(sizeof(EGatedTone))) ==
		  NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (eGatedTonePtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	eGatedTonePtr->parSpec = parSpec;
	eGatedTonePtr->typeModeFlag = TRUE;
	eGatedTonePtr->floorModeFlag = TRUE;
	eGatedTonePtr->carrierFrequencyFlag = TRUE;
	eGatedTonePtr->amplitudeFlag = TRUE;
	eGatedTonePtr->phaseFlag = TRUE;
	eGatedTonePtr->beginPeriodDurationFlag = TRUE;
	eGatedTonePtr->repetitionRateFlag = TRUE;
	eGatedTonePtr->halfLifeFlag = TRUE;
	eGatedTonePtr->floorFlag = TRUE;
	eGatedTonePtr->durationFlag = TRUE;
	eGatedTonePtr->dtFlag = TRUE;
	eGatedTonePtr->typeMode = EGATED_TONE_RAMPED_MODE;
	eGatedTonePtr->floorMode = GENERAL_BOOLEAN_OFF;
	eGatedTonePtr->carrierFrequency = 1000.0;
	eGatedTonePtr->amplitude = 10000.0;
	eGatedTonePtr->phase = 0.0;
	eGatedTonePtr->beginPeriodDuration = 0.0;
	eGatedTonePtr->repetitionRate = 40.0;
	eGatedTonePtr->halfLife = 5e-3;
	eGatedTonePtr->floor = 0.0;
	eGatedTonePtr->duration = 0.1;
	eGatedTonePtr->dt = 0.1e-3;

	InitTypeModeList_Stimulus_ExpGatedTone();
	if (!SetUniParList_Stimulus_ExpGatedTone()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_Stimulus_ExpGatedTone();
		return(FALSE);
	}
	eGatedTonePtr->cycleTimer = 0.0;
	return(TRUE);

}


/********************************* SetUniParList ******************************/

/*
 * This routine initialises and sets the module's universal parameter list.
 * This list provides universal access to the module's parameters.
 */
 
BOOLN
SetUniParList_Stimulus_ExpGatedTone(void)
{
	static const char *funcName = "SetUniParList_Stimulus_ExpGatedTone";
	UniParPtr	pars;

	if ((eGatedTonePtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  EGATED_TONE_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = eGatedTonePtr->parList->pars;
	SetPar_UniParMgr(&pars[EGATED_TONE_TYPEMODE], "T_MODE",
	  "Type mode ('ramped' or 'damped').",
	  UNIPAR_NAME_SPEC,
	  &eGatedTonePtr->typeMode, eGatedTonePtr->typeModeList,
	  (void * (*)) SetTypeMode_Stimulus_ExpGatedTone);
	pars = eGatedTonePtr->parList->pars;
	SetPar_UniParMgr(&pars[EGATED_TONE_FLOORMODE], "F_MODE",
	  "Floor mode ('on' or 'off').",
	  UNIPAR_BOOL,
	  &eGatedTonePtr->floorMode, NULL,
	  (void * (*)) SetFloorMode_Stimulus_ExpGatedTone);
	pars = eGatedTonePtr->parList->pars;
	SetPar_UniParMgr(&pars[EGATED_TONE_CARRIERFREQUENCY], "FREQUENCY",
	  "Carrier frequency (Hz).",
	  UNIPAR_REAL,
	  &eGatedTonePtr->carrierFrequency, NULL,
	  (void * (*)) SetCarrierFrequency_Stimulus_ExpGatedTone);
	pars = eGatedTonePtr->parList->pars;
	SetPar_UniParMgr(&pars[EGATED_TONE_AMPLITUDE], "AMPLITUDE",
	  "Amplitude (uPa).",
	  UNIPAR_REAL,
	  &eGatedTonePtr->amplitude, NULL,
	  (void * (*)) SetAmplitude_Stimulus_ExpGatedTone);
	pars = eGatedTonePtr->parList->pars;
	SetPar_UniParMgr(&pars[EGATED_TONE_PHASE], "PHASE",
	  "Phase (degrees).",
	  UNIPAR_REAL,
	  &eGatedTonePtr->phase, NULL,
	  (void * (*)) SetPhase_Stimulus_ExpGatedTone);
	pars = eGatedTonePtr->parList->pars;
	SetPar_UniParMgr(&pars[EGATED_TONE_BEGINPERIODDURATION], "SILENCE",
	  "Begin period duration - silence (s).",
	  UNIPAR_REAL,
	  &eGatedTonePtr->beginPeriodDuration, NULL,
	  (void * (*)) SetBeginPeriodDuration_Stimulus_ExpGatedTone);
	pars = eGatedTonePtr->parList->pars;
	SetPar_UniParMgr(&pars[EGATED_TONE_REPETITIONRATE], "R_RATE",
	  "Repetition rate (Hz).",
	  UNIPAR_REAL,
	  &eGatedTonePtr->repetitionRate, NULL,
	  (void * (*)) SetRepetitionRate_Stimulus_ExpGatedTone);
	pars = eGatedTonePtr->parList->pars;
	SetPar_UniParMgr(&pars[EGATED_TONE_HALFLIFE], "HALF_LIFE",
	  "Half life (s).",
	  UNIPAR_REAL,
	  &eGatedTonePtr->halfLife, NULL,
	  (void * (*)) SetHalfLife_Stimulus_ExpGatedTone);
	pars = eGatedTonePtr->parList->pars;
	SetPar_UniParMgr(&pars[EGATED_TONE_FLOOR], "FLOOR",
	  "Floor value (uPa).",
	  UNIPAR_REAL,
	  &eGatedTonePtr->floor, NULL,
	  (void * (*)) SetFloor_Stimulus_ExpGatedTone);
	pars = eGatedTonePtr->parList->pars;
	SetPar_UniParMgr(&pars[EGATED_TONE_DURATION], "DURATION",
	  "Duration (s).",
	  UNIPAR_REAL,
	  &eGatedTonePtr->duration, NULL,
	  (void * (*)) SetDuration_Stimulus_ExpGatedTone);
	pars = eGatedTonePtr->parList->pars;
	SetPar_UniParMgr(&pars[EGATED_TONE_SAMPLINGINTERVAL], "DT",
	  "Sampling interval, dt (s).",
	  UNIPAR_REAL,
	  &eGatedTonePtr->dt, NULL,
	  (void * (*)) SetSamplingInterval_Stimulus_ExpGatedTone);
	return(TRUE);

}

/********************************* GetUniParListPtr ***************************/

/*
 * This routine returns a pointer to the module's universal parameter list.
 */

UniParListPtr
GetUniParListPtr_Stimulus_ExpGatedTone(void)
{
	static const char *funcName = "GetUniParListPtr_Stimulus_ExpGatedTone";

	if (eGatedTonePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (eGatedTonePtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(eGatedTonePtr->parList);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_Stimulus_ExpGatedTone(char *typeMode, char *floorMode,
  double carrierFrequency, double amplitude, double phase, double
  beginPeriodDuration, double repetitionRate, double halfLife, double floor,
  double duration, double samplingInterval)
{
	static const char	*funcName = "SetPars_Stimulus_ExpGatedTone";
	BOOLN	ok;

	ok = TRUE;
	if (!SetTypeMode_Stimulus_ExpGatedTone(typeMode))
		ok = FALSE;
	if (!SetFloorMode_Stimulus_ExpGatedTone(floorMode))
		ok = FALSE;
	if (!SetCarrierFrequency_Stimulus_ExpGatedTone(carrierFrequency))
		ok = FALSE;
	if (!SetAmplitude_Stimulus_ExpGatedTone(amplitude))
		ok = FALSE;
	if (!SetPhase_Stimulus_ExpGatedTone(phase))
		ok = FALSE;
	if (!SetBeginPeriodDuration_Stimulus_ExpGatedTone(beginPeriodDuration))
		ok = FALSE;
	if (!SetRepetitionRate_Stimulus_ExpGatedTone(repetitionRate))
		ok = FALSE;
	if (!SetHalfLife_Stimulus_ExpGatedTone(halfLife))
		ok = FALSE;
	if (!SetFloor_Stimulus_ExpGatedTone(floor))
		ok = FALSE;
	if (!SetDuration_Stimulus_ExpGatedTone(duration))
		ok = FALSE;
	if (!SetSamplingInterval_Stimulus_ExpGatedTone(samplingInterval))
		ok = FALSE;
	if (!ok)
		NotifyError("%s: Failed to set all module parameters." ,funcName);
	return(ok);

}

/****************************** SetTypeMode ***********************************/

/*
 * This function sets the module's typeMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTypeMode_Stimulus_ExpGatedTone(char *theTypeMode)
{
	static const char	*funcName = "SetTypeMode_Stimulus_ExpGatedTone";
	int		specifier;

	if (eGatedTonePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theTypeMode,
	  eGatedTonePtr->typeModeList)) == EGATED_TONE_MODE_NULL) {
		NotifyError("%s: Illegal type mode (%s).", funcName,
		  theTypeMode);
		return(FALSE);
	}
	eGatedTonePtr->typeModeFlag = TRUE;
	eGatedTonePtr->typeMode = specifier;
	return(TRUE);

}

/****************************** SetFloorMode **********************************/

/*
 * This function sets the module's floorMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetFloorMode_Stimulus_ExpGatedTone(char *theFloorMode)
{
	static const char	*funcName = "SetFloorMode_Stimulus_ExpGatedTone";
	int		specifier;

	if (eGatedTonePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theFloorMode,
	  BooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError("%s: Illegal switch state (%s).", funcName,
		  theFloorMode);
		return(FALSE);
	}
	eGatedTonePtr->floorModeFlag = TRUE;
	eGatedTonePtr->floorMode = specifier;
	return(TRUE);

}

/****************************** SetCarrierFrequency ***************************/

/*
 * This function sets the module's carrierFrequency parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCarrierFrequency_Stimulus_ExpGatedTone(double theCarrierFrequency)
{
	static const char	*funcName = "SetCarrierFrequency_Stimulus_ExpGatedTone";

	if (eGatedTonePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	eGatedTonePtr->carrierFrequencyFlag = TRUE;
	eGatedTonePtr->carrierFrequency = theCarrierFrequency;
	return(TRUE);

}

/****************************** SetAmplitude **********************************/

/*
 * This function sets the module's amplitude parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetAmplitude_Stimulus_ExpGatedTone(double theAmplitude)
{
	static const char	*funcName = "SetAmplitude_Stimulus_ExpGatedTone";

	if (eGatedTonePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	eGatedTonePtr->amplitudeFlag = TRUE;
	eGatedTonePtr->amplitude = theAmplitude;
	return(TRUE);

}

/****************************** SetPhase **************************************/

/*
 * This function sets the module's phase parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPhase_Stimulus_ExpGatedTone(double thePhase)
{
	static const char	*funcName = "SetPhase_Stimulus_ExpGatedTone";

	if (eGatedTonePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	eGatedTonePtr->phaseFlag = TRUE;
	eGatedTonePtr->phase = thePhase;
	return(TRUE);

}

/****************************** SetBeginPeriodDuration ************************/

/*
 * This function sets the module's beginPeriodDuration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetBeginPeriodDuration_Stimulus_ExpGatedTone(double theBeginPeriodDuration)
{
	static const char	*funcName =
	  "SetBeginPeriodDuration_Stimulus_ExpGatedTone";

	if (eGatedTonePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	eGatedTonePtr->beginPeriodDurationFlag = TRUE;
	eGatedTonePtr->beginPeriodDuration = theBeginPeriodDuration;
	return(TRUE);

}

/****************************** SetRepetitionRate *****************************/

/*
 * This function sets the module's repetitionRate parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRepetitionRate_Stimulus_ExpGatedTone(double theRepetitionRate)
{
	static const char	*funcName = "SetRepetitionRate_Stimulus_ExpGatedTone";

	if (eGatedTonePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	eGatedTonePtr->repetitionRateFlag = TRUE;
	eGatedTonePtr->repetitionRate = theRepetitionRate;
	return(TRUE);

}

/****************************** SetHalfLife ***********************************/

/*
 * This function sets the module's halfLife parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetHalfLife_Stimulus_ExpGatedTone(double theHalfLife)
{
	static const char	*funcName = "SetHalfLife_Stimulus_ExpGatedTone";

	if (eGatedTonePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	eGatedTonePtr->halfLifeFlag = TRUE;
	eGatedTonePtr->halfLife = theHalfLife;
	return(TRUE);

}

/****************************** SetFloor **************************************/

/*
 * This function sets the module's floor parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetFloor_Stimulus_ExpGatedTone(double theFloor)
{
	static const char	*funcName = "SetFloor_Stimulus_ExpGatedTone";

	if (eGatedTonePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	eGatedTonePtr->floorFlag = TRUE;
	eGatedTonePtr->floor = theFloor;
	return(TRUE);

}

/****************************** SetDuration ***********************************/

/*
 * This function sets the module's duration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDuration_Stimulus_ExpGatedTone(double theDuration)
{
	static const char	*funcName = "SetDuration_Stimulus_ExpGatedTone";

	if (eGatedTonePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	eGatedTonePtr->durationFlag = TRUE;
	eGatedTonePtr->duration = theDuration;
	return(TRUE);

}

/****************************** SetSamplingInterval ***************************/

/*
 * This function sets the module's samplingInterval parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSamplingInterval_Stimulus_ExpGatedTone(double theSamplingInterval)
{
	static const char	*funcName = "SetSamplingInterval_Stimulus_ExpGatedTone";

	if (eGatedTonePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	eGatedTonePtr->dtFlag = TRUE;
	eGatedTonePtr->dt = theSamplingInterval;
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
CheckPars_Stimulus_ExpGatedTone(void)
{
	static const char	*funcName = "CheckPars_Stimulus_ExpGatedTone";
	BOOLN	ok;

	ok = TRUE;
	if (eGatedTonePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!eGatedTonePtr->typeModeFlag) {
		NotifyError("%s: typeMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!eGatedTonePtr->floorModeFlag) {
		NotifyError("%s: floorMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!eGatedTonePtr->carrierFrequencyFlag) {
		NotifyError("%s: carrierFrequency variable not set.", funcName);
		ok = FALSE;
	}
	if (!eGatedTonePtr->amplitudeFlag) {
		NotifyError("%s: amplitude variable not set.", funcName);
		ok = FALSE;
	}
	if (!eGatedTonePtr->phaseFlag) {
		NotifyError("%s: phase variable not set.", funcName);
		ok = FALSE;
	}
	if (!eGatedTonePtr->beginPeriodDurationFlag) {
		NotifyError("%s: beginPeriodDuration variable not set.", funcName);
		ok = FALSE;
	}
	if (!eGatedTonePtr->repetitionRateFlag) {
		NotifyError("%s: repetitionRate variable not set.", funcName);
		ok = FALSE;
	}
	if (!eGatedTonePtr->halfLifeFlag) {
		NotifyError("%s: halfLife variable not set.", funcName);
		ok = FALSE;
	}
	if (!eGatedTonePtr->floorFlag) {
		NotifyError("%s: floor variable not set.", funcName);
		ok = FALSE;
	}
	if (!eGatedTonePtr->durationFlag) {
		NotifyError("%s: duration variable not set.", funcName);
		ok = FALSE;
	}
	if (!eGatedTonePtr->dtFlag) {
		NotifyError("%s: dt variable not set.", funcName);
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
PrintPars_Stimulus_ExpGatedTone(void)
{
	static const char	*funcName = "PrintPars_Stimulus_ExpGatedTone";

	if (!CheckPars_Stimulus_ExpGatedTone()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Exponentially Gated Pure Tone Module Parameters:-\n");
	DPrint("\tType mode = %s", eGatedTonePtr->typeModeList[
	  eGatedTonePtr->typeMode].name);
	DPrint("\tCarrier frequency = %g Hz,\n", eGatedTonePtr->carrierFrequency);
	DPrint("\tAmplitude (maximum) = %g uPa,", eGatedTonePtr->amplitude);
	DPrint("\tPhase = %g (degrees),\n", eGatedTonePtr->phase);
	DPrint("\tRepetition rate = %g Hz,", eGatedTonePtr->repetitionRate);
	DPrint("\tHalf Life = %g ms,", MSEC(eGatedTonePtr->halfLife));
	DPrint("\tFloor mode: %s,\n", BooleanList_NSpecLists(
	  eGatedTonePtr->floorMode)->name);
	if (eGatedTonePtr->floorMode == GENERAL_BOOLEAN_ON)
		DPrint("\tFloor = %g uPa,", eGatedTonePtr->floor);
	DPrint("\tBegin period duration = %g ms\n",
	  MSEC(eGatedTonePtr->beginPeriodDuration));
	DPrint("\tDuration = %g ms,", MSEC(eGatedTonePtr->duration));
	DPrint("\tSampling interval = %g ms\n", MSEC(eGatedTonePtr->dt));
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_Stimulus_ExpGatedTone(char *fileName)
{
	static const char	*funcName = "ReadPars_Stimulus_ExpGatedTone";
	BOOLN	ok;
	char	*filePath;
	char	typeMode[MAXLINE], floorMode[MAXLINE];
	double	carrierFrequency, amplitude, phase, beginPeriodDuration;
	double	repetitionRate, halfLife, floor, duration, samplingInterval;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(filePath, "r")) == NULL) {
		NotifyError("%s: Cannot open data file '%s'.\n", funcName, filePath);
		return(FALSE);
	}
	DPrint("%s: Reading from '%s':\n", funcName, filePath);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, "%s", typeMode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &carrierFrequency))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &amplitude))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &phase))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &repetitionRate))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &halfLife))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%s", floorMode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &floor))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &beginPeriodDuration))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &duration))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &samplingInterval))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "
		  "parameter file '%s'.", funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_Stimulus_ExpGatedTone(typeMode, floorMode, carrierFrequency,
	  amplitude, phase, beginPeriodDuration, repetitionRate, halfLife, floor,
	  duration, samplingInterval)) {
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
SetParsPointer_Stimulus_ExpGatedTone(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_Stimulus_ExpGatedTone";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	eGatedTonePtr = (EGatedTonePtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Stimulus_ExpGatedTone(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_Stimulus_ExpGatedTone";

	if (!SetParsPointer_Stimulus_ExpGatedTone(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_Stimulus_ExpGatedTone(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = eGatedTonePtr;
	theModule->CheckPars = CheckPars_Stimulus_ExpGatedTone;
	theModule->Free = Free_Stimulus_ExpGatedTone;
	theModule->GetUniParListPtr = GetUniParListPtr_Stimulus_ExpGatedTone;
	theModule->PrintPars = PrintPars_Stimulus_ExpGatedTone;
	theModule->ReadPars = ReadPars_Stimulus_ExpGatedTone;
	theModule->RunProcess = GenerateSignal_Stimulus_ExpGatedTone;
	theModule->SetParsPointer = SetParsPointer_Stimulus_ExpGatedTone;
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
GenerateSignal_Stimulus_ExpGatedTone(EarObjectPtr data)
{
	static const char	*funcName = "GenerateSignal_Stimulus_ExpGatedTone";
	register ChanData	 *outPtr;
	register double		time, cycle, expFactor, phaseRads, cyclePeriod;
	ChanLen		i, t;
	EGatedTonePtr	p = eGatedTonePtr;

	if (!data->threadRunFlag) {
		if (data == NULL) {
			NotifyError("%s: EarObject not initialised.", funcName);
			return(FALSE);
		}
		if (!CheckPars_Stimulus_ExpGatedTone())
			return(FALSE);
		SetProcessName_EarObject(data, "Exponentially Gated Pure Tone Module "
		  "Process");
		if ( !InitOutSignal_EarObject(data, EGATED_TONE_NUM_CHANNELS,
		  (ChanLen) floor(p->duration / p->dt + 0.5), p->dt)) {
			NotifyError("%s: Cannot initialise output signal", funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	cyclePeriod = 1.0 / p->repetitionRate;
	expFactor = -LN_2 / p->halfLife;
	phaseRads = DEGREES_TO_RADS(p->phase);
	if (data->timeIndex == PROCESS_START_TIME) {
		p->cycleTimer = 0.0;
		p->nextCycle = cyclePeriod;
	}
	outPtr = data->outSignal->channel[0];
	for (i = 0, t = data->timeIndex + 1; i < data->outSignal->length; i++, t++,
	  outPtr++) {
		time = t * p->dt;
	  	if (time < p->beginPeriodDuration) {
			*outPtr = 0.0;
	  		continue;
		}
		cycle = (p->typeMode == EGATED_TONE_RAMPED_MODE)? cyclePeriod -
		  p->cycleTimer: p->cycleTimer;
		*outPtr = p->amplitude * sin(p->carrierFrequency * p->cycleTimer *
		  PIx2 + phaseRads) * exp(cycle * expFactor);
		if (p->floorMode && (*outPtr < p->floor))
			*outPtr = p->floor;
		if (time >= p->nextCycle) {
			p->cycleTimer = 0.0;
			p->nextCycle += cyclePeriod;
		} else
			p->cycleTimer += p->dt;
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

