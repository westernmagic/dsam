/**********************
 *
 * File:		DistProds.c
 * Purpose:		
 * Comments:	Written using ModuleProducer version 1.2.9 (Aug 10 2000).
 * Author:		
 * Created:		22 Aug 2000
 * Updated:	
 * Copyright:	(c) 2000, 
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
#include "DistProds.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

DPParsPtr	dPParsPtr = NULL;

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
Free_DistProds(void)
{
	/* static const char	*funcName = "Free_DistProds";  */

	if (dPParsPtr == NULL)
		return(FALSE);
	if (dPParsPtr->parSpec == GLOBAL) {
		free(dPParsPtr);
		dPParsPtr = NULL;
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
Init_DistProds(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_DistProds";

	if (parSpec == GLOBAL) {
		if (dPParsPtr != NULL)
			Free_DistProds();
		if ((dPParsPtr = (DPParsPtr) malloc(sizeof(DPPars))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (dPParsPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	dPParsPtr->parSpec = parSpec;
	dPParsPtr->enabledFlag = FALSE;
	dPParsPtr->outputModeFlag = FALSE;
	dPParsPtr->f1FrequencyFlag = FALSE;
	dPParsPtr->f2ScaleFlag = FALSE;
	dPParsPtr->intensityFlag = FALSE;
	dPParsPtr->stimulusDurationFlag = FALSE;
	dPParsPtr->dtFlag = FALSE;
	dPParsPtr->enabled = 0;
	dPParsPtr->outputMode = 0;
	dPParsPtr->f1Frequency = 0.0;
	dPParsPtr->f2Scale = 0.0;
	dPParsPtr->intensity = 0.0;
	dPParsPtr->stimulusDuration = 0.0;
	dPParsPtr->dt = 0.0;

	sprintf(dPParsPtr->outputFile, NO_FILE);
	if ((dPParsPtr->outputModeList = InitNameList_NSpecLists(
	  DiagModeList_NSpecLists(0), dPParsPtr->outputFile)) == NULL)
		return(FALSE);
	dPParsPtr->audModel = NULL;
	dPParsPtr->stimulus = NULL;
	dPParsPtr->analysis = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_DistProds(UniParPtr pars)
{
	static const char *funcName = "SetUniParList_DistProds";

	if (dPParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	SetPar_UniParMgr(&pars[DISTPRODS_ENABLED], "DP_ENABLED",
	  "Distortion products test state 'on' or 'off'.",
	  UNIPAR_BOOL,
	  &dPParsPtr->enabled, NULL,
	  (void * (*)) SetEnabled_DistProds);
	SetPar_UniParMgr(&pars[DISTPRODS_OUTPUTMODE], "DP_OUTPUT_MODE",
	  "Distortion products test output 'off', 'screen' or <file name>.",
	  UNIPAR_NAME_SPEC_WITH_FILE,
	  &dPParsPtr->outputMode, dPParsPtr->outputModeList,
	  (void * (*)) SetOutputMode_DistProds);
	SetPar_UniParMgr(&pars[DISTPRODS_F1FREQUENCY], "DP_F1_FREQ",
	  "F1 tone stimulus frequency (Hz).",
	  UNIPAR_REAL,
	  &dPParsPtr->f1Frequency, NULL,
	  (void * (*)) SetF1Frequency_DistProds);
	SetPar_UniParMgr(&pars[DISTPRODS_F2SCALE], "DP_F2_SCALE",
	  "F2 tone frequency scale - multiplied by F1.",
	  UNIPAR_REAL,
	  &dPParsPtr->f2Scale, NULL,
	  (void * (*)) SetF2Scale_DistProds);
	SetPar_UniParMgr(&pars[DISTPRODS_INTENSITY], "DP_INTENSITY",
	  "Intensity for each of the tone components (dB SPL).",
	  UNIPAR_REAL,
	  &dPParsPtr->intensity, NULL,
	  (void * (*)) SetIntensity_DistProds);
	SetPar_UniParMgr(&pars[DISTPRODS_STIMULUSDURATION], "DP_DURATION",
	  "Stimulus duration (s).",
	  UNIPAR_REAL,
	  &dPParsPtr->stimulusDuration, NULL,
	  (void * (*)) SetStimulusDuration_DistProds);
	SetPar_UniParMgr(&pars[DISTPRODS_SAMPLINGINTERVAL], "DP_DT",
	  "Stimulus sampling interval (s).",
	  UNIPAR_REAL,
	  &dPParsPtr->dt, NULL,
	  (void * (*)) SetSamplingInterval_DistProds);
	return(TRUE);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_DistProds(char * enabled, char * outputMode, double f1Frequency,
  double f2Scale, double intensity, double stimulusDuration,
  double samplingInterval)
{
	static const char	*funcName = "SetPars_DistProds";
	BOOLN	ok;

	ok = TRUE;
	if (!SetEnabled_DistProds(enabled))
		ok = FALSE;
	if (!SetOutputMode_DistProds(outputMode))
		ok = FALSE;
	if (!SetF1Frequency_DistProds(f1Frequency))
		ok = FALSE;
	if (!SetF2Scale_DistProds(f2Scale))
		ok = FALSE;
	if (!SetIntensity_DistProds(intensity))
		ok = FALSE;
	if (!SetStimulusDuration_DistProds(stimulusDuration))
		ok = FALSE;
	if (!SetSamplingInterval_DistProds(samplingInterval))
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
SetEnabled_DistProds(char * theEnabled)
{
	static const char	*funcName = "SetEnabled_DistProds";
	int		specifier;

	if (dPParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theEnabled, BooleanList_NSpecLists(
	  0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError("%s: Illegal name (%s).", funcName, theEnabled);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	dPParsPtr->enabledFlag = TRUE;
	dPParsPtr->enabled = specifier;
	return(TRUE);

}

/****************************** SetOutputMode *********************************/

/*
 * This function sets the module's outputMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOutputMode_DistProds(char * theOutputMode)
{
	static const char	*funcName = "SetOutputMode_DistProds";

	if (dPParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	dPParsPtr->outputMode = IdentifyDiag_NSpecLists(theOutputMode,
	  dPParsPtr->outputModeList);
	dPParsPtr->outputModeFlag = TRUE;
	return(TRUE);

}

/****************************** SetF1Frequency ********************************/

/*
 * This function sets the module's f1Frequency parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetF1Frequency_DistProds(double theF1Frequency)
{
	static const char	*funcName = "SetF1Frequency_DistProds";

	if (dPParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	dPParsPtr->f1FrequencyFlag = TRUE;
	dPParsPtr->f1Frequency = theF1Frequency;
	return(TRUE);

}

/****************************** SetF2Scale ********************************/

/*
 * This function sets the module's f2Scale parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetF2Scale_DistProds(double theF2Scale)
{
	static const char	*funcName = "SetF2Scale_DistProds";

	if (dPParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	dPParsPtr->f2ScaleFlag = TRUE;
	dPParsPtr->f2Scale = theF2Scale;
	return(TRUE);

}

/****************************** SetIntensity **********************************/

/*
 * This function sets the module's intensity parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIntensity_DistProds(double theIntensity)
{
	static const char	*funcName = "SetIntensity_DistProds";

	if (dPParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	dPParsPtr->intensityFlag = TRUE;
	dPParsPtr->intensity = theIntensity;
	return(TRUE);

}

/****************************** SetStimulusDuration ***************************/

/*
 * This function sets the module's stimulusDuration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetStimulusDuration_DistProds(double theStimulusDuration)
{
	static const char	*funcName = "SetStimulusDuration_DistProds";

	if (dPParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	dPParsPtr->stimulusDurationFlag = TRUE;
	dPParsPtr->stimulusDuration = theStimulusDuration;
	return(TRUE);

}

/****************************** SetSamplingInterval ***************************/

/*
 * This function sets the module's samplingInterval parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSamplingInterval_DistProds(double theSamplingInterval)
{
	static const char	*funcName = "SetSamplingInterval_DistProds";

	if (dPParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	dPParsPtr->dtFlag = TRUE;
	dPParsPtr->dt = theSamplingInterval;
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
CheckPars_DistProds(void)
{
	static const char	*funcName = "CheckPars_DistProds";
	BOOLN	ok;

	ok = TRUE;
	if (dPParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!dPParsPtr->enabledFlag) {
		NotifyError("%s: enabled variable not set.", funcName);
		ok = FALSE;
	}
	if (!dPParsPtr->outputModeFlag) {
		NotifyError("%s: outputMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!dPParsPtr->f1FrequencyFlag) {
		NotifyError("%s: f1Frequency variable not set.", funcName);
		ok = FALSE;
	}
	if (!dPParsPtr->f2ScaleFlag) {
		NotifyError("%s: f2Scale variable not set.", funcName);
		ok = FALSE;
	}
	if (!dPParsPtr->intensityFlag) {
		NotifyError("%s: intensity variable not set.", funcName);
		ok = FALSE;
	}
	if (!dPParsPtr->stimulusDurationFlag) {
		NotifyError("%s: stimulusDuration variable not set.", funcName);
		ok = FALSE;
	}
	if (!dPParsPtr->dtFlag) {
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
PrintPars_DistProds(void)
{
	static const char	*funcName = "PrintPars_DistProds";

	if (!CheckPars_DistProds()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("?? DistProds Module Parameters:-\n");
	DPrint("\tenabled = %s \n", BooleanList_NSpecLists(
	  dPParsPtr->enabled)->name);
	DPrint("\toutputMode = %s \n", dPParsPtr->outputModeList[
	  dPParsPtr->outputMode].name);
	DPrint("\tf1Frequency = %g ??\n", dPParsPtr->f1Frequency);
	DPrint("\tf2Scale = %g ??\n", dPParsPtr->f2Scale);
	DPrint("\tintensity = %g ??\n", dPParsPtr->intensity);
	DPrint("\tstimulusDuration = %g ??\n", dPParsPtr->stimulusDuration);
	DPrint("\tsamplingInterval = %g ??\n", dPParsPtr->dt);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_DistProds(FILE *fp)
{
	static const char	*funcName = "ReadPars_DistProds";
	BOOLN	ok = TRUE;
	char	enabled[MAXLINE], outputMode[MAXLINE];
	double	f1Frequency, f2Scale, intensity, stimulusDuration;
	double	samplingInterval;

	if (!fp) {
		NotifyError("%s: File not selected.", funcName);
		return(FALSE);
	}
	if (!GetPars_ParFile(fp, "%s", enabled))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%s", outputMode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &f1Frequency))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &f2Scale))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &intensity))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &stimulusDuration))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &samplingInterval))
		ok = FALSE;
	if (!ok) {
		NotifyError("%s: Failed to read parameters.", funcName);
		return(FALSE);
	}
	if (!SetPars_DistProds(enabled, outputMode, f1Frequency, f2Scale,
	  intensity, stimulusDuration, samplingInterval)) {
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
CheckData_DistProds(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_DistProds";

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
InitProcessVariables_DistProds(EarObjectPtr data)
{
	static const char * funcName = "InitProcessVariables_DistProds";
	char	indexedValue[MAXLINE];

	dPParsPtr->audModel = data;
	if ((dPParsPtr->stimulus = Init_EarObject(
	  STIM_PURETONE_MULTI_PROCESS_NAME)) == NULL) {
		NotifyError("%s: Could not initialise the 2 tone stimulus EarObject.",
		  funcName);
		return(FALSE);
	}
	if ((dPParsPtr->analysis = Init_EarObject(ANA_FOURIERT_PROCESS_NAME)) ==
	  NULL) {
		NotifyError("%s: Could not initialise the analysis EarObject.",
		  funcName);
		return(FALSE);
	}

	ConnectOutSignalToIn_EarObject(dPParsPtr->stimulus, dPParsPtr->audModel);
	ConnectOutSignalToIn_EarObject(dPParsPtr->audModel, dPParsPtr->analysis);

	if (!SetRealPar_ModuleMgr(dPParsPtr->stimulus, "num", 2.0)) {
		NotifyError("%s: Cannot set the number of pure tones.", funcName);
		return(FALSE);
	}
	if (!SetRealPar_ModuleMgr(dPParsPtr->stimulus, "duration",
	  dPParsPtr->stimulusDuration)) {
		NotifyError("%s: Cannot set the stimulus duration.", funcName);
		return(FALSE);
	}
	if (!SetRealPar_ModuleMgr(dPParsPtr->stimulus, "dt", dPParsPtr->dt)) {
		NotifyError("%s: Cannot set the stimulus sampling interval.",
		  funcName);
		return(FALSE);
	}
	sprintf(indexedValue, "0:%g", dPParsPtr->f1Frequency);
	if (!SetPar_ModuleMgr(dPParsPtr->stimulus, "frequency", indexedValue)) {
		NotifyError("%s: Cannot set the f1 tone frequency.", funcName);
		return(FALSE);
	}
	sprintf(indexedValue, "1:%g", dPParsPtr->f2Scale * dPParsPtr->f1Frequency);
	if (!SetPar_ModuleMgr(dPParsPtr->stimulus, "frequency", indexedValue)) {
		NotifyError("%s: Cannot set the f2 tone frequency.", funcName);
		return(FALSE);
	}
	sprintf(indexedValue, "0:%g", dPParsPtr->intensity);
	if (!SetPar_ModuleMgr(dPParsPtr->stimulus, "intensity", indexedValue)) {
		NotifyError("%s: Cannot set the f1 tone intensity.", funcName);
		return(FALSE);
	}
	sprintf(indexedValue, "1:%g", dPParsPtr->intensity);
	if (!SetPar_ModuleMgr(dPParsPtr->stimulus, "intensity", indexedValue)) {
		NotifyError("%s: Cannot set the f2 tone intensity.", funcName);
		return(FALSE);
	}

	if (!SetPar_ModuleMgr(dPParsPtr->analysis, "output_mode", "modulus")) {
		NotifyError("%s: Cannot set the analysis output mode.", funcName);
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
FreeProcessVariables_DistProds(void)
{
	if (dPParsPtr->stimulus && dPParsPtr->audModel)
		DisconnectOutSignalFromIn_EarObject(dPParsPtr->stimulus,
		  dPParsPtr->audModel);
	if (dPParsPtr->audModel && dPParsPtr->analysis )
		DisconnectOutSignalFromIn_EarObject(dPParsPtr->audModel,
		  dPParsPtr->analysis);
	Free_EarObject(&dPParsPtr->stimulus);
	Free_EarObject(&dPParsPtr->analysis);
	
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
RunProcess_DistProds(EarObjectPtr data)
{
	static const char	*funcName = "RunProcess_DistProds";

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckPars_DistProds())
		return(FALSE);
	if (!CheckData_DistProds(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "Distortion products process");
	if (!InitProcessVariables_DistProds(data)) {
		NotifyError("%s: Could not initialise the process variables.",
		  funcName);
		FreeProcessVariables_DistProds();
		return(FALSE);
	}
	if (!RunProcess_ModuleMgr(dPParsPtr->stimulus)) {
		NotifyError("%s: Failed to run stimulus process.", funcName);
		FreeProcessVariables_DistProds();
		return(FALSE);
	}
	if (!RunProcess_ModuleMgr(dPParsPtr->audModel)) {
		NotifyError("%s: Failed to run auditory model.", funcName);
		FreeProcessVariables_DistProds();
		return(FALSE);
	}
	if (!RunProcess_ModuleMgr(dPParsPtr->analysis)) {
		NotifyError("%s: Failed to run analysis process.", funcName);
		FreeProcessVariables_DistProds();
		return(FALSE);
	}
	WriteOutSignal_DataFile(dPParsPtr->outputFile, dPParsPtr->analysis);
	FreeProcessVariables_DistProds();
	return(TRUE);

}

