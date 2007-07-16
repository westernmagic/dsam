/**********************
 *
 * File:		TwoTSRatio.c
 * Purpose:		Two tone suppression analysis.
 * Comments:	Written using ModuleProducer version 1.2.9 (Aug 10 2000).
 * Author:		L. P. O'Mard
 * Created:		11 Aug 2000
 * Updated:	
 * Copyright:	(c) 2000, CNBH, University of Essex.
 *
 *********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "DSAM.h"
#include "Utils.h"
#include "TwoTSRatio.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

TTSRParsPtr	tTSRParsPtr = NULL;

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
Free_TwoTSuppRatio(void)
{
	/* static const char	*funcName = "Free_TwoTSuppRatio";  */

	if (tTSRParsPtr == NULL)
		return(FALSE);
	if (tTSRParsPtr->parSpec == GLOBAL) {
		free(tTSRParsPtr);
		tTSRParsPtr = NULL;
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
Init_TwoTSuppRatio(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_TwoTSuppRatio";

	if (parSpec == GLOBAL) {
		if (tTSRParsPtr != NULL)
			Free_TwoTSuppRatio();
		if ((tTSRParsPtr = (TTSRParsPtr) malloc(sizeof(TTSRPars))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (tTSRParsPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	tTSRParsPtr->parSpec = parSpec;
	tTSRParsPtr->enabledFlag = FALSE;
	tTSRParsPtr->outputModeFlag = FALSE;
	tTSRParsPtr->t2ScaleFlag = FALSE;
	tTSRParsPtr->numT1IntensitiesFlag = FALSE;
	tTSRParsPtr->initialT1IntensityFlag = FALSE;
	tTSRParsPtr->deltaT1IntensityFlag = FALSE;
	tTSRParsPtr->numT2IntensitiesFlag = FALSE;
	tTSRParsPtr->initialT2IntensityFlag = FALSE;
	tTSRParsPtr->deltaT2IntensityFlag = FALSE;
	tTSRParsPtr->stimulusDurationFlag = FALSE;
	tTSRParsPtr->dtFlag = FALSE;
	tTSRParsPtr->enabled = 0;
	tTSRParsPtr->outputMode = 0;
	tTSRParsPtr->t2Scale = 0.0;
	tTSRParsPtr->numT1Intensities = 0;
	tTSRParsPtr->initialT1Intensity = 0.0;
	tTSRParsPtr->deltaT1Intensity = 0.0;
	tTSRParsPtr->numT2Intensities = 0;
	tTSRParsPtr->initialT2Intensity = 0.0;
	tTSRParsPtr->deltaT2Intensity = 0.0;
	tTSRParsPtr->stimulusDuration = 0.0;
	tTSRParsPtr->dt = 0.0;

	sprintf(tTSRParsPtr->outputFile, NO_FILE);
	if ((tTSRParsPtr->outputModeList = InitNameList_NSpecLists(
	  DiagModeList_NSpecLists(0), tTSRParsPtr->outputFile)) == NULL)
		return(FALSE);
	tTSRParsPtr->audModel = NULL;
	tTSRParsPtr->stimulus[0] = NULL;
	tTSRParsPtr->stimulus[1] = NULL;
	tTSRParsPtr->analysis = NULL;
	tTSRParsPtr->resultEarObj = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_TwoTSuppRatio(UniParPtr pars)
{
	static const char *funcName = "SetUniParList_TwoTSuppRatio";

	if (tTSRParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	SetPar_UniParMgr(&pars[TWOTSUPPRATIO_ENABLED], "TTSR_ENABLED",
	  "Phase intensity function test state 'on' or 'off'.",
	  UNIPAR_BOOL,
	  &tTSRParsPtr->enabled, NULL,
	  (void * (*)) SetEnabled_TwoTSuppRatio);
	SetPar_UniParMgr(&pars[TWOTSUPPRATIO_OUTPUTMODE], "TTSR_OUTPUT_MODE",
	  "I/O function test output 'off', 'screen' or <file name>.",
	  UNIPAR_NAME_SPEC_WITH_FILE,
	  &tTSRParsPtr->outputMode, tTSRParsPtr->outputModeList,
	  (void * (*)) SetOutputMode_TwoTSuppRatio);
	SetPar_UniParMgr(&pars[TWOTSUPPRATIO_T2SCALE], "TTSR_T2_SCALE",
	  "T2 tone frequency scale - multiplied by T1 frequency.",
	  UNIPAR_REAL,
	  &tTSRParsPtr->t2Scale, NULL,
	  (void * (*)) SetT2Scale_TwoTSuppRatio);
	SetPar_UniParMgr(&pars[TWOTSUPPRATIO_NUMT1INTENSITIES], "TTSR_NUM_T1",
	  "Number of T1 intensities - functions plotted (dB SPL).",
	  UNIPAR_INT,
	  &tTSRParsPtr->numT1Intensities, NULL,
	  (void * (*)) SetNumT1Intensities_TwoTSuppRatio);
	SetPar_UniParMgr(&pars[TWOTSUPPRATIO_INITIALT1INTENSITY], "TTSR_INITIAL_T1",
	  "Initial T1 intensity (dB SPL).",
	  UNIPAR_REAL,
	  &tTSRParsPtr->initialT1Intensity, NULL,
	  (void * (*)) SetInitialT1Intensity_TwoTSuppRatio);
	SetPar_UniParMgr(&pars[TWOTSUPPRATIO_DELTAT1INTENSITY], "TTSR_DELTA_T1",
	  "T1 intensity interval (dB).",
	  UNIPAR_REAL,
	  &tTSRParsPtr->deltaT1Intensity, NULL,
	  (void * (*)) SetDeltaT1Intensity_TwoTSuppRatio);
	SetPar_UniParMgr(&pars[TWOTSUPPRATIO_NUMT2INTENSITIES], "TTSR_NUM_T2",
	  "Number of T2 intensities - function points (dB SPL).",
	  UNIPAR_INT,
	  &tTSRParsPtr->numT2Intensities, NULL,
	  (void * (*)) SetNumT2Intensities_TwoTSuppRatio);
	SetPar_UniParMgr(&pars[TWOTSUPPRATIO_INITIALT2INTENSITY], "TTSR_INITIAL_T2",
	  "Initial T2 intensity (dB SPL).",
	  UNIPAR_REAL,
	  &tTSRParsPtr->initialT2Intensity, NULL,
	  (void * (*)) SetInitialT2Intensity_TwoTSuppRatio);
	SetPar_UniParMgr(&pars[TWOTSUPPRATIO_DELTAT2INTENSITY], "TTSR_DELTA_T2",
	  "T2 intensity interval (dB).",
	  UNIPAR_REAL,
	  &tTSRParsPtr->deltaT2Intensity, NULL,
	  (void * (*)) SetDeltaT2Intensity_TwoTSuppRatio);
	SetPar_UniParMgr(&pars[TWOTSUPPRATIO_STIMULUSDURATION], "TTSR_DURATION",
	  "Stimulus duration (s).",
	  UNIPAR_REAL,
	  &tTSRParsPtr->stimulusDuration, NULL,
	  (void * (*)) SetStimulusDuration_TwoTSuppRatio);
	SetPar_UniParMgr(&pars[TWOTSUPPRATIO_SAMPLINGINTERVAL], "TTSR_DT",
	  "Stimulus Sampling interval, dt (s).",
	  UNIPAR_REAL,
	  &tTSRParsPtr->dt, NULL,
	  (void * (*)) SetSamplingInterval_TwoTSuppRatio);
	return(TRUE);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_TwoTSuppRatio(char * enabled, char * outputMode, double t2Scale,
  int numT1Intensities, double initialT1Intensity, double deltaT1Intensity,
  int numT2Intensities, double initialT2Intensity, double deltaT2Intensity,
  double stimulusDuration, double samplingInterval)
{
	static const char	*funcName = "SetPars_TwoTSuppRatio";
	BOOLN	ok;

	ok = TRUE;
	if (!SetEnabled_TwoTSuppRatio(enabled))
		ok = FALSE;
	if (!SetOutputMode_TwoTSuppRatio(outputMode))
		ok = FALSE;
	if (!SetT2Scale_TwoTSuppRatio(t2Scale))
		ok = FALSE;
	if (!SetNumT1Intensities_TwoTSuppRatio(numT1Intensities))
		ok = FALSE;
	if (!SetInitialT1Intensity_TwoTSuppRatio(initialT1Intensity))
		ok = FALSE;
	if (!SetDeltaT1Intensity_TwoTSuppRatio(deltaT1Intensity))
		ok = FALSE;
	if (!SetNumT2Intensities_TwoTSuppRatio(numT2Intensities))
		ok = FALSE;
	if (!SetInitialT2Intensity_TwoTSuppRatio(initialT2Intensity))
		ok = FALSE;
	if (!SetDeltaT2Intensity_TwoTSuppRatio(deltaT2Intensity))
		ok = FALSE;
	if (!SetStimulusDuration_TwoTSuppRatio(stimulusDuration))
		ok = FALSE;
	if (!SetSamplingInterval_TwoTSuppRatio(samplingInterval))
		ok = FALSE;
	if (!ok)
		NotifyError("%s: Failed to set all module parameters." ,funcName);
	return(ok);

}

/****************************** SetEnabled ************************************/

/*
 * This function sets the module's enabled parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetEnabled_TwoTSuppRatio(char * theEnabled)
{
	static const char	*funcName = "SetEnabled_TwoTSuppRatio";
	int		specifier;

	if (tTSRParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theEnabled, BooleanList_NSpecLists(
	  0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError("%s: Illegal name (%s).", funcName, theEnabled);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	tTSRParsPtr->enabledFlag = TRUE;
	tTSRParsPtr->enabled = specifier;
	return(TRUE);

}

/****************************** SetOutputMode *********************************/

/*
 * This function sets the module's outputMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOutputMode_TwoTSuppRatio(char * theOutputMode)
{
	static const char	*funcName = "SetOutputMode_TwoTSuppRatio";

	if (tTSRParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	tTSRParsPtr->outputMode = IdentifyDiag_NSpecLists(theOutputMode,
	  tTSRParsPtr->outputModeList);
	tTSRParsPtr->outputModeFlag = TRUE;
	return(TRUE);

}

/****************************** SetT2Scale ********************************/

/*
 * This function sets the module's t2Scale parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetT2Scale_TwoTSuppRatio(double theT2Scale)
{
	static const char	*funcName = "SetT2Scale_TwoTSuppRatio";

	if (tTSRParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	tTSRParsPtr->t2ScaleFlag = TRUE;
	tTSRParsPtr->t2Scale = theT2Scale;
	return(TRUE);

}

/****************************** SetNumT1Intensities ***************************/

/*
 * This function sets the module's numT1Intensities parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNumT1Intensities_TwoTSuppRatio(int theNumT1Intensities)
{
	static const char	*funcName = "SetNumT1Intensities_TwoTSuppRatio";

	if (tTSRParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	tTSRParsPtr->numT1IntensitiesFlag = TRUE;
	tTSRParsPtr->numT1Intensities = theNumT1Intensities;
	return(TRUE);

}

/****************************** SetInitialT1Intensity *************************/

/*
 * This function sets the module's initialT1Intensity parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetInitialT1Intensity_TwoTSuppRatio(double theInitialT1Intensity)
{
	static const char	*funcName = "SetInitialT1Intensity_TwoTSuppRatio";

	if (tTSRParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	tTSRParsPtr->initialT1IntensityFlag = TRUE;
	tTSRParsPtr->initialT1Intensity = theInitialT1Intensity;
	return(TRUE);

}

/****************************** SetDeltaT1Intensity ***************************/

/*
 * This function sets the module's deltaT1Intensity parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDeltaT1Intensity_TwoTSuppRatio(double theDeltaT1Intensity)
{
	static const char	*funcName = "SetDeltaT1Intensity_TwoTSuppRatio";

	if (tTSRParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	tTSRParsPtr->deltaT1IntensityFlag = TRUE;
	tTSRParsPtr->deltaT1Intensity = theDeltaT1Intensity;
	return(TRUE);

}

/****************************** SetNumT2Intensities ***************************/

/*
 * This function sets the module's numT2Intensities parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNumT2Intensities_TwoTSuppRatio(int theNumT2Intensities)
{
	static const char	*funcName = "SetNumT2Intensities_TwoTSuppRatio";

	if (tTSRParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	tTSRParsPtr->numT2IntensitiesFlag = TRUE;
	tTSRParsPtr->numT2Intensities = theNumT2Intensities;
	return(TRUE);

}

/****************************** SetInitialT2Intensity *************************/

/*
 * This function sets the module's initialT2Intensity parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetInitialT2Intensity_TwoTSuppRatio(double theInitialT2Intensity)
{
	static const char	*funcName = "SetInitialT2Intensity_TwoTSuppRatio";

	if (tTSRParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	tTSRParsPtr->initialT2IntensityFlag = TRUE;
	tTSRParsPtr->initialT2Intensity = theInitialT2Intensity;
	return(TRUE);

}

/****************************** SetDeltaT2Intensity ***************************/

/*
 * This function sets the module's deltaT2Intensity parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDeltaT2Intensity_TwoTSuppRatio(double theDeltaT2Intensity)
{
	static const char	*funcName = "SetDeltaT2Intensity_TwoTSuppRatio";

	if (tTSRParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	tTSRParsPtr->deltaT2IntensityFlag = TRUE;
	tTSRParsPtr->deltaT2Intensity = theDeltaT2Intensity;
	return(TRUE);

}

/****************************** SetStimulusDuration ***************************/

/*
 * This function sets the module's stimulusDuration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetStimulusDuration_TwoTSuppRatio(double theStimulusDuration)
{
	static const char	*funcName = "SetStimulusDuration_TwoTSuppRatio";

	if (tTSRParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	tTSRParsPtr->stimulusDurationFlag = TRUE;
	tTSRParsPtr->stimulusDuration = theStimulusDuration;
	return(TRUE);

}

/****************************** SetSamplingInterval ***************************/

/*
 * This function sets the module's samplingInterval parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSamplingInterval_TwoTSuppRatio(double theSamplingInterval)
{
	static const char	*funcName = "SetSamplingInterval_TwoTSuppRatio";

	if (tTSRParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	tTSRParsPtr->dtFlag = TRUE;
	tTSRParsPtr->dt = theSamplingInterval;
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
CheckPars_TwoTSuppRatio(void)
{
	static const char	*funcName = "CheckPars_TwoTSuppRatio";
	BOOLN	ok;

	ok = TRUE;
	if (tTSRParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!tTSRParsPtr->enabledFlag) {
		NotifyError("%s: enabled variable not set.", funcName);
		ok = FALSE;
	}
	if (!tTSRParsPtr->outputModeFlag) {
		NotifyError("%s: outputMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!tTSRParsPtr->t2ScaleFlag) {
		NotifyError("%s: t2Scale variable not set.", funcName);
		ok = FALSE;
	}
	if (!tTSRParsPtr->numT1IntensitiesFlag) {
		NotifyError("%s: numT1Intensities variable not set.", funcName);
		ok = FALSE;
	}
	if (!tTSRParsPtr->initialT1IntensityFlag) {
		NotifyError("%s: initialT1Intensity variable not set.", funcName);
		ok = FALSE;
	}
	if (!tTSRParsPtr->deltaT1IntensityFlag) {
		NotifyError("%s: deltaT1Intensity variable not set.", funcName);
		ok = FALSE;
	}
	if (!tTSRParsPtr->numT2IntensitiesFlag) {
		NotifyError("%s: numT2Intensities variable not set.", funcName);
		ok = FALSE;
	}
	if (!tTSRParsPtr->initialT2IntensityFlag) {
		NotifyError("%s: initialT2Intensity variable not set.", funcName);
		ok = FALSE;
	}
	if (!tTSRParsPtr->deltaT2IntensityFlag) {
		NotifyError("%s: deltaT2Intensity variable not set.", funcName);
		ok = FALSE;
	}
	if (!tTSRParsPtr->stimulusDurationFlag) {
		NotifyError("%s: stimulusDuration variable not set.", funcName);
		ok = FALSE;
	}
	if (!tTSRParsPtr->dtFlag) {
		NotifyError("%s: dt variable not set.", funcName);
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
PrintPars_TwoTSuppRatio(void)
{
	static const char	*funcName = "PrintPars_TwoTSuppRatio";

	if (!CheckPars_TwoTSuppRatio()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("?? TwoTSuppRatio Module Parameters:-\n");
	DPrint("\tenabled = %s \n", BooleanList_NSpecLists(
	  tTSRParsPtr->enabled)->name);
	DPrint("\toutputMode = %s \n", tTSRParsPtr->outputModeList[
	  tTSRParsPtr->outputMode].name);
	DPrint("\tt2Scale = %g ??\n", tTSRParsPtr->t2Scale);
	DPrint("\tnumT1Intensities = %d ??\n", tTSRParsPtr->numT1Intensities);
	DPrint("\tinitialT1Intensity = %g ??\n", tTSRParsPtr->initialT1Intensity);
	DPrint("\tdeltaT1Intensity = %g ??\n", tTSRParsPtr->deltaT1Intensity);
	DPrint("\tnumT2Intensities = %d ??\n", tTSRParsPtr->numT2Intensities);
	DPrint("\tinitialT2Intensity = %g ??\n", tTSRParsPtr->initialT2Intensity);
	DPrint("\tdeltaT2Intensity = %g ??\n", tTSRParsPtr->deltaT2Intensity);
	DPrint("\tstimulusDuration = %g ??\n", tTSRParsPtr->stimulusDuration);
	DPrint("\tsamplingInterval = %g ??\n", tTSRParsPtr->dt);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_TwoTSuppRatio(FILE *fp)
{
	static const char	*funcName = "ReadPars_TwoTSuppRatio";
	BOOLN	ok = TRUE;
	int		numT1Intensities, numT2Intensities;
	char	enabled[MAXLINE], outputMode[MAXLINE];
	double	t2Scale, initialT1Intensity, deltaT1Intensity, initialT2Intensity;
	double	deltaT2Intensity, stimulusDuration, samplingInterval;

	if (!fp) {
		NotifyError("%s: File not selected.", funcName);
		return(FALSE);
	}
	if (!GetPars_ParFile(fp, "%s", enabled))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%s", outputMode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &t2Scale))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%d", &numT1Intensities))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &initialT1Intensity))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &deltaT1Intensity))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%d", &numT2Intensities))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &initialT2Intensity))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &deltaT2Intensity))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &stimulusDuration))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &samplingInterval))
		ok = FALSE;
	if (!ok) {
		NotifyError("%s: Failed to read parameters.", funcName);
		return(FALSE);
	}
	if (!SetPars_TwoTSuppRatio(enabled, outputMode, t2Scale, numT1Intensities,
	  initialT1Intensity, deltaT1Intensity, numT2Intensities,
	  initialT2Intensity, deltaT2Intensity, stimulusDuration,
	  samplingInterval)) {
		NotifyError("%s: Could not set parameters.", funcName);
		return(FALSE);
	}
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
CheckData_TwoTSuppRatio(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_TwoTSuppRatio";

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	/*** Put additional checks here. ***/
	return(TRUE);

}

/****************************** InitProcessVariables **************************/

/*
 * This function initialises the process variables, making connections to the
 * simulation process as required.
 * The stimulus connection will be done later.
 * It returns FALSE if it fails in any way.
 */

BOOLN
InitProcessVariables_TwoTSuppRatio(EarObjectPtr data)
{
	static const char * funcName = "InitProcessVariables_TwoTSuppRatio";
	int		i;
	UniParPtr	par;

	tTSRParsPtr->audModel = data;
	if ((tTSRParsPtr->stimulus[0] = Init_EarObject(
	  STIM_PURETONE_PROCESS_NAME)) == NULL) {
		NotifyError("%s: Could not initialise the single tone stimulus "
		  "EarObject.", funcName);
		return(FALSE);
	}
	if ((tTSRParsPtr->stimulus[1] = Init_EarObject(
	  STIM_PURETONE_MULTI_PROCESS_NAME)) == NULL) {
		NotifyError("%s: Could not initialise the 2 tone stimulus EarObject.",
		  funcName);
		return(FALSE);
	}
	if ((tTSRParsPtr->analysis = Init_EarObject(ANA_FOURIERT_PROCESS_NAME)) ==
	  NULL) {
		NotifyError("%s: Could not initialise the analysis EarObject.",
		  funcName);
		return(FALSE);
	}

	ConnectOutSignalToIn_EarObject(tTSRParsPtr->audModel,
	  tTSRParsPtr->analysis);

	for (i = 0; i < 2; i++) {
		if (!SetRealPar_ModuleMgr(tTSRParsPtr->stimulus[i], "duration",
		  tTSRParsPtr->stimulusDuration)) {
			NotifyError("%s: Cannot set the stimulus duration.", funcName);
			return(FALSE);
		}
		if (!SetRealPar_ModuleMgr(tTSRParsPtr->stimulus[i], "dt",
		  tTSRParsPtr->dt)) {
			NotifyError("%s: Cannot set the stimulus sampling interval.",
			  funcName);
			return(FALSE);
		}
	}
	if ((par = GetUniParPtr_ModuleMgr(data, "cflist")) == NULL) {
		NotifyError("%s: Cannot find the CF list to set the tone frequencies.",
		  funcName);
		return(FALSE);
	}
	tTSRParsPtr->t1Frequency = (*par->valuePtr.cFPtr)->frequency[CHANNEL];
	if (!SetRealPar_ModuleMgr(tTSRParsPtr->stimulus[0], "frequency",
	   tTSRParsPtr->t1Frequency)) {
		NotifyError("%s: Cannot set the single tone stimulus frequency.",
		  funcName);
		return(FALSE);
	}
	if (!SetRealPar_ModuleMgr(tTSRParsPtr->stimulus[1], "num", 2.0)) {
		NotifyError("%s: Cannot set the number of pure tones.", funcName);
		return(FALSE);
	}
	if (!SetRealArrayPar_ModuleMgr(tTSRParsPtr->stimulus[1], "frequency",
	  0, tTSRParsPtr->t1Frequency)) {
		NotifyError("%s: Cannot set the double tone stimulus, T1 frequency.",
		  funcName);
		return(FALSE);
	}
	if (!SetRealArrayPar_ModuleMgr(tTSRParsPtr->stimulus[1], "frequency",
	  1, tTSRParsPtr->t2Scale * tTSRParsPtr->t1Frequency)) {
		NotifyError("%s: Cannot set the double tone stimulus, T2 frequency.",
		  funcName);
		return(FALSE);
	}
	if (!SetPar_ModuleMgr(tTSRParsPtr->analysis, "output_mode", "modulus")) {
		NotifyError("%s: Cannot set the analysis output mode.", funcName);
		return(FALSE);
	}

	if ((tTSRParsPtr->resultEarObj = InitResultEarObject_Utils(
	  tTSRParsPtr->numT1Intensities, tTSRParsPtr->numT2Intensities,
	  tTSRParsPtr->initialT2Intensity, tTSRParsPtr->deltaT2Intensity,
	  funcName)) == NULL) {
		NotifyError("%s: Could not initialise results EarObject", funcName);
		return(FALSE);
	}
	return(TRUE);

}

/****************************** FreeProcessVariables **************************/

/*
 * This routine unconnects and frees the process variables.
 * The stimulus will already have been disconnected.
 */

void
FreeProcessVariables_TwoTSuppRatio(void)
{
	if (tTSRParsPtr->audModel && tTSRParsPtr->analysis)
		DisconnectOutSignalFromIn_EarObject(tTSRParsPtr->audModel,
		  tTSRParsPtr->analysis);
	Free_EarObject(&tTSRParsPtr->stimulus[0]);
	Free_EarObject(&tTSRParsPtr->stimulus[1]);
	Free_EarObject(&tTSRParsPtr->analysis);
	Free_EarObject(&tTSRParsPtr->resultEarObj);
	
}

/****************************** RunToneAnalysis *******************************/

/*
 * This function runs the analysis, according to which stimulus is to be used.
 * It assumes that the correct connection has already been made.
 * The connection is not done within this routine as it would mean multiple
 * connections and disconnections which are not needed.
 */

BOOLN
RunToneAnalysis_TwoTSuppRatio(int stimulusIndex)
{
	static const char	*funcName = "RunProcess_TwoTSuppRatio";

	if (!RunProcess_ModuleMgr(tTSRParsPtr->stimulus[stimulusIndex])) {
		NotifyError("%s: Failed to run stimulus process.", funcName);
		return(FALSE);
	}
	if (!RunProcess_ModuleMgr(tTSRParsPtr->audModel)) {
		NotifyError("%s: Failed to run auditory model.", funcName);
		return(FALSE);
	}
	if (!RunProcess_ModuleMgr(tTSRParsPtr->analysis)) {
		NotifyError("%s: Failed to run analysis process.", funcName);
		return(FALSE);
	}
	return(TRUE);

}

/****************************** RunProcess ************************************/

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
RunProcess_TwoTSuppRatio(EarObjectPtr data)
{
	static const char	*funcName = "RunProcess_TwoTSuppRatio";
	int		i, j;
	double	t1Intensity, t2Intensity, unSuppressedMod, suppressedMod;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckPars_TwoTSuppRatio())
		return(FALSE);
	if (!CheckData_TwoTSuppRatio(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "TTS Analysis process");
	if (!InitProcessVariables_TwoTSuppRatio(data)) {
		NotifyError("%s: Could not initialise the process variables.",
		  funcName);
		return(FALSE);
	}
	for (i = 0, t1Intensity = tTSRParsPtr->initialT1Intensity; i < 
	  tTSRParsPtr->numT1Intensities; i++, t1Intensity +=
	  tTSRParsPtr->deltaT1Intensity) {
		if (!SetRealPar_ModuleMgr(tTSRParsPtr->stimulus[0], "intensity",
		  t1Intensity)) {
			NotifyError("%s: Cannot set the single stimulus intensity.",
			  funcName);
			return(FALSE);
		}
		if (!SetRealArrayPar_ModuleMgr(tTSRParsPtr->stimulus[1], "intensity",
		  0, t1Intensity)) {
			NotifyError("%s: Cannot set the stimulus Tone 1 intensity.",
			  funcName);
			return(FALSE);
		}

		ConnectOutSignalToIn_EarObject(tTSRParsPtr->stimulus[0],
		  tTSRParsPtr->audModel);
		if (!RunToneAnalysis_TwoTSuppRatio(0)) {
			NotifyError("%s: Could not run tone analysis.", funcName);
			return(FALSE);
		}
		DisconnectOutSignalFromIn_EarObject(tTSRParsPtr->stimulus[0],
		  tTSRParsPtr->audModel);
		unSuppressedMod = GetFreqIndexSample_Utils(tTSRParsPtr->analysis,
		  tTSRParsPtr->t1Frequency);

		ConnectOutSignalToIn_EarObject(tTSRParsPtr->stimulus[1],
		  tTSRParsPtr->audModel);
		for (j = 0, t2Intensity = tTSRParsPtr->initialT2Intensity; j <
		  tTSRParsPtr->numT2Intensities; j++, t2Intensity +=
		  tTSRParsPtr->deltaT2Intensity) {
			if (!SetRealArrayPar_ModuleMgr(tTSRParsPtr->stimulus[1],
			  "intensity", 1, t2Intensity)) {
				NotifyError("%s: Cannot set the stimulus Tone 2 intensity.",
				  funcName);
				return(FALSE);
			}
			if (!RunToneAnalysis_TwoTSuppRatio(1)) {
				NotifyError("%s: Could not run tone analysis.", funcName);
				return(FALSE);
			}
			suppressedMod = GetFreqIndexSample_Utils(tTSRParsPtr->analysis,
			  tTSRParsPtr->t1Frequency);
			tTSRParsPtr->resultEarObj->outSignal->channel[i][j] = 100.0 *
			  suppressedMod / unSuppressedMod;
		}
		DisconnectOutSignalFromIn_EarObject(tTSRParsPtr->stimulus[1],
		  tTSRParsPtr->audModel);
		SetInfoChannelLabel_SignalData(tTSRParsPtr->resultEarObj->outSignal, i,
		  t1Intensity);
	}
	OutputResults_Utils(tTSRParsPtr->resultEarObj, &tTSRParsPtr->outputModeList[
	  tTSRParsPtr->outputMode], UTILS_NORMAL_SIGNAL);
	FreeProcessVariables_TwoTSuppRatio();
	return(TRUE);

}

