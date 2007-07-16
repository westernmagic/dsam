/**********************
 *
 * File:		TwoTSResp.c
 * Purpose:		This routine generates the response of a probe tone subjected to
 *				a suppressor tone.
 * Comments:	Written using ModuleProducer version 1.2.9 (Aug 10 2000).
 * Author:		L. P. O'Mard
 * Created:		21 Aug 2000
 * Updated:	
 * Copyright:	(c) 2000, University of Essex.
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
#include "TwoTSResp.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

TTSReParsPtr	tTSReParsPtr = NULL;

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
Free_TwoTSuppResp(void)
{
	/* static const char	*funcName = "Free_TwoTSuppResp";  */

	if (tTSReParsPtr == NULL)
		return(FALSE);
	if (tTSReParsPtr->parSpec == GLOBAL) {
		free(tTSReParsPtr);
		tTSReParsPtr = NULL;
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
Init_TwoTSuppResp(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_TwoTSuppResp";

	if (parSpec == GLOBAL) {
		if (tTSReParsPtr != NULL)
			Free_TwoTSuppResp();
		if ((tTSReParsPtr = (TTSReParsPtr) malloc(sizeof(TTSRePars))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (tTSReParsPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	tTSReParsPtr->parSpec = parSpec;
	tTSReParsPtr->enabledFlag = FALSE;
	tTSReParsPtr->outputModeFlag = FALSE;
	tTSReParsPtr->probeFreqFlag = FALSE;
	tTSReParsPtr->probeIntensityFlag = FALSE;
	tTSReParsPtr->suppressorFreqFlag = FALSE;
	tTSReParsPtr->numSupprIntensitiesFlag = FALSE;
	tTSReParsPtr->initialSupprIntensityFlag = FALSE;
	tTSReParsPtr->deltaSupprIntensityFlag = FALSE;
	tTSReParsPtr->stimulusDurationFlag = FALSE;
	tTSReParsPtr->dtFlag = FALSE;
	tTSReParsPtr->enabled = 0;
	tTSReParsPtr->outputMode = 0;
	tTSReParsPtr->probeFreq = 0.0;
	tTSReParsPtr->probeIntensity = 0.0;
	tTSReParsPtr->suppressorFreq = 0.0;
	tTSReParsPtr->numSupprIntensities = 0;
	tTSReParsPtr->initialSupprIntensity = 0.0;
	tTSReParsPtr->deltaSupprIntensity = 0.0;
	tTSReParsPtr->stimulusDuration = 0.0;
	tTSReParsPtr->dt = 0.0;

	sprintf(tTSReParsPtr->outputFile, NO_FILE);
	if ((tTSReParsPtr->outputModeList = InitNameList_NSpecLists(
	  DiagModeList_NSpecLists(0), tTSReParsPtr->outputFile)) == NULL)
		return(FALSE);
	tTSReParsPtr->audModel = NULL;
	tTSReParsPtr->stimulus = NULL;
	tTSReParsPtr->resultEarObj = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_TwoTSuppResp(UniParPtr pars)
{
	static const char *funcName = "SetUniParList_TwoTSuppResp";

	if (tTSReParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	SetPar_UniParMgr(&pars[TWOTSUPPRESP_ENABLED], "TTSRE_ENABLED",
	  "2-Tone supppression response test state 'on' or 'off'.",
	  UNIPAR_BOOL,
	  &tTSReParsPtr->enabled, NULL,
	  (void * (*)) SetEnabled_TwoTSuppResp);
	SetPar_UniParMgr(&pars[TWOTSUPPRESP_OUTPUTMODE], "TTSRE_OUTPUT_MODE",
	  "2-Tone suppression response test output 'off', 'screen' or <file name>.",
	  UNIPAR_NAME_SPEC_WITH_FILE,
	  &tTSReParsPtr->outputMode, tTSReParsPtr->outputModeList,
	  (void * (*)) SetOutputMode_TwoTSuppResp);
	SetPar_UniParMgr(&pars[TWOTSUPPRESP_PROBEFREQ], "TTSRE_PROBE_FREQ",
	  "Probe tone frequency (Hz).",
	  UNIPAR_REAL,
	  &tTSReParsPtr->probeFreq, NULL,
	  (void * (*)) SetProbeFreq_TwoTSuppResp);
	SetPar_UniParMgr(&pars[TWOTSUPPRESP_PROBEINTENSITY],
	  "TTSRE_PROBE_INTENSITY",
	  "Probe tone intensity (dB SPL).",
	  UNIPAR_REAL,
	  &tTSReParsPtr->probeIntensity, NULL,
	  (void * (*)) SetProbeIntensity_TwoTSuppResp);
	SetPar_UniParMgr(&pars[TWOTSUPPRESP_SUPPRESSORFREQ], "TTSRE_SUPPR_FREQ",
	  "Supprocessor tone frequency (Hz).",
	  UNIPAR_REAL,
	  &tTSReParsPtr->suppressorFreq, NULL,
	  (void * (*)) SetSuppressorFreq_TwoTSuppResp);
	SetPar_UniParMgr(&pars[TWOTSUPPRESP_NUMSUPPRINTENSITIES], "TTSRE_NUM_INT",
	  "Number of suppressor tone intensities.",
	  UNIPAR_INT,
	  &tTSReParsPtr->numSupprIntensities, NULL,
	  (void * (*)) SetNumSupprIntensities_TwoTSuppResp);
	SetPar_UniParMgr(&pars[TWOTSUPPRESP_INITIALSUPPRINTENSITY],
	  "TTSRE_INIT_INT",
	  "Initial suppressor tone intensity (dB SPL).",
	  UNIPAR_REAL,
	  &tTSReParsPtr->initialSupprIntensity, NULL,
	  (void * (*)) SetInitialSupprIntensity_TwoTSuppResp);
	SetPar_UniParMgr(&pars[TWOTSUPPRESP_DELTASUPPRINTENSITY], "TTSRE_DELTA_INT",
	  "Suppressor intensity change (dB).",
	  UNIPAR_REAL,
	  &tTSReParsPtr->deltaSupprIntensity, NULL,
	  (void * (*)) SetDeltaSupprIntensity_TwoTSuppResp);
	SetPar_UniParMgr(&pars[TWOTSUPPRESP_STIMULUSDURATION], "TTSRE_DURATION",
	  "Stimulus duration (s).",
	  UNIPAR_REAL,
	  &tTSReParsPtr->stimulusDuration, NULL,
	  (void * (*)) SetStimulusDuration_TwoTSuppResp);
	SetPar_UniParMgr(&pars[TWOTSUPPRESP_SAMPLINGINTERVAL], "TTSRE_DT",
	  "Stimulus sampling interval (s).",
	  UNIPAR_REAL,
	  &tTSReParsPtr->dt, NULL,
	  (void * (*)) SetSamplingInterval_TwoTSuppResp);
	return(TRUE);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_TwoTSuppResp(char * enabled, char * outputMode, double
  probeFreq, double probeIntensity, double suppressorFreq,
  int numSupprIntensities, double initialSupprIntensity,
  double deltaSupprIntensity, double stimulusDuration, double samplingInterval)
{
	static const char	*funcName = "SetPars_TwoTSuppResp";
	BOOLN	ok;

	ok = TRUE;
	if (!SetEnabled_TwoTSuppResp(enabled))
		ok = FALSE;
	if (!SetOutputMode_TwoTSuppResp(outputMode))
		ok = FALSE;
	if (!SetProbeFreq_TwoTSuppResp(probeFreq))
		ok = FALSE;
	if (!SetProbeIntensity_TwoTSuppResp(probeIntensity))
		ok = FALSE;
	if (!SetSuppressorFreq_TwoTSuppResp(suppressorFreq))
		ok = FALSE;
	if (!SetNumSupprIntensities_TwoTSuppResp(numSupprIntensities))
		ok = FALSE;
	if (!SetInitialSupprIntensity_TwoTSuppResp(initialSupprIntensity))
		ok = FALSE;
	if (!SetDeltaSupprIntensity_TwoTSuppResp(deltaSupprIntensity))
		ok = FALSE;
	if (!SetStimulusDuration_TwoTSuppResp(stimulusDuration))
		ok = FALSE;
	if (!SetSamplingInterval_TwoTSuppResp(samplingInterval))
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
SetEnabled_TwoTSuppResp(char * theEnabled)
{
	static const char	*funcName = "SetEnabled_TwoTSuppResp";
	int		specifier;

	if (tTSReParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theEnabled, BooleanList_NSpecLists(
	  0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError("%s: Illegal name (%s).", funcName, theEnabled);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	tTSReParsPtr->enabledFlag = TRUE;
	tTSReParsPtr->enabled = specifier;
	return(TRUE);

}

/****************************** SetOutputMode *********************************/

/*
 * This function sets the module's outputMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOutputMode_TwoTSuppResp(char * theOutputMode)
{
	static const char	*funcName = "SetOutputMode_TwoTSuppResp";

	if (tTSReParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	tTSReParsPtr->outputMode = IdentifyDiag_NSpecLists(theOutputMode,
	  tTSReParsPtr->outputModeList);
	tTSReParsPtr->outputModeFlag = TRUE;
	return(TRUE);

}

/****************************** SetProbeFreq **********************************/

/*
 * This function sets the module's probeFreq parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetProbeFreq_TwoTSuppResp(double theProbeFreq)
{
	static const char	*funcName = "SetProbeFreq_TwoTSuppResp";

	if (tTSReParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	tTSReParsPtr->probeFreqFlag = TRUE;
	tTSReParsPtr->probeFreq = theProbeFreq;
	return(TRUE);

}

/****************************** SetProbeIntensity *****************************/

/*
 * This function sets the module's probeIntensity parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetProbeIntensity_TwoTSuppResp(double theProbeIntensity)
{
	static const char	*funcName = "SetProbeIntensity_TwoTSuppResp";

	if (tTSReParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	tTSReParsPtr->probeIntensityFlag = TRUE;
	tTSReParsPtr->probeIntensity = theProbeIntensity;
	return(TRUE);

}

/****************************** SetSuppressorFreq *****************************/

/*
 * This function sets the module's suppressorFreq parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSuppressorFreq_TwoTSuppResp(double theSuppressorFreq)
{
	static const char	*funcName = "SetSuppressorFreq_TwoTSuppResp";

	if (tTSReParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	tTSReParsPtr->suppressorFreqFlag = TRUE;
	tTSReParsPtr->suppressorFreq = theSuppressorFreq;
	return(TRUE);

}

/****************************** SetNumSupprIntensities ************************/

/*
 * This function sets the module's numSupprIntensities parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNumSupprIntensities_TwoTSuppResp(int theNumSupprIntensities)
{
	static const char	*funcName = "SetNumSupprIntensities_TwoTSuppResp";

	if (tTSReParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	tTSReParsPtr->numSupprIntensitiesFlag = TRUE;
	tTSReParsPtr->numSupprIntensities = theNumSupprIntensities;
	return(TRUE);

}

/****************************** SetInitialSupprIntensity **********************/

/*
 * This function sets the module's initialSupprIntensity parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetInitialSupprIntensity_TwoTSuppResp(double theInitialSupprIntensity)
{
	static const char	*funcName = "SetInitialSupprIntensity_TwoTSuppResp";

	if (tTSReParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	tTSReParsPtr->initialSupprIntensityFlag = TRUE;
	tTSReParsPtr->initialSupprIntensity = theInitialSupprIntensity;
	return(TRUE);

}

/****************************** SetDeltaSupprIntensity ************************/

/*
 * This function sets the module's deltaSupprIntensity parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDeltaSupprIntensity_TwoTSuppResp(double theDeltaSupprIntensity)
{
	static const char	*funcName = "SetDeltaSupprIntensity_TwoTSuppResp";

	if (tTSReParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	tTSReParsPtr->deltaSupprIntensityFlag = TRUE;
	tTSReParsPtr->deltaSupprIntensity = theDeltaSupprIntensity;
	return(TRUE);

}

/****************************** SetStimulusDuration ***************************/

/*
 * This function sets the module's stimulusDuration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetStimulusDuration_TwoTSuppResp(double theStimulusDuration)
{
	static const char	*funcName = "SetStimulusDuration_TwoTSuppResp";

	if (tTSReParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	tTSReParsPtr->stimulusDurationFlag = TRUE;
	tTSReParsPtr->stimulusDuration = theStimulusDuration;
	return(TRUE);

}

/****************************** SetSamplingInterval ***************************/

/*
 * This function sets the module's samplingInterval parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSamplingInterval_TwoTSuppResp(double theSamplingInterval)
{
	static const char	*funcName = "SetSamplingInterval_TwoTSuppResp";

	if (tTSReParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	tTSReParsPtr->dtFlag = TRUE;
	tTSReParsPtr->dt = theSamplingInterval;
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
CheckPars_TwoTSuppResp(void)
{
	static const char	*funcName = "CheckPars_TwoTSuppResp";
	BOOLN	ok;

	ok = TRUE;
	if (tTSReParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!tTSReParsPtr->enabledFlag) {
		NotifyError("%s: enabled variable not set.", funcName);
		ok = FALSE;
	}
	if (!tTSReParsPtr->outputModeFlag) {
		NotifyError("%s: outputMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!tTSReParsPtr->probeFreqFlag) {
		NotifyError("%s: probeFreq variable not set.", funcName);
		ok = FALSE;
	}
	if (!tTSReParsPtr->probeIntensityFlag) {
		NotifyError("%s: probeIntensity variable not set.", funcName);
		ok = FALSE;
	}
	if (!tTSReParsPtr->suppressorFreqFlag) {
		NotifyError("%s: suppressorFreq variable not set.", funcName);
		ok = FALSE;
	}
	if (!tTSReParsPtr->numSupprIntensitiesFlag) {
		NotifyError("%s: numSupprIntensities variable not set.", funcName);
		ok = FALSE;
	}
	if (!tTSReParsPtr->initialSupprIntensityFlag) {
		NotifyError("%s: initialSupprIntensity variable not set.", funcName);
		ok = FALSE;
	}
	if (!tTSReParsPtr->deltaSupprIntensityFlag) {
		NotifyError("%s: deltaSupprIntensity variable not set.", funcName);
		ok = FALSE;
	}
	if (!tTSReParsPtr->stimulusDurationFlag) {
		NotifyError("%s: stimulusDuration variable not set.", funcName);
		ok = FALSE;
	}
	if (!tTSReParsPtr->dtFlag) {
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
PrintPars_TwoTSuppResp(void)
{
	static const char	*funcName = "PrintPars_TwoTSuppResp";

	if (!CheckPars_TwoTSuppResp()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("?? TwoTSuppResp Module Parameters:-\n");
	DPrint("\tenabled = %s \n", BooleanList_NSpecLists(
	  tTSReParsPtr->enabled)->name);
	DPrint("\toutputMode = %s \n", tTSReParsPtr->outputModeList[
	  tTSReParsPtr->outputMode].name);
	DPrint("\tprobeFreq = %g ??\n", tTSReParsPtr->probeFreq);
	DPrint("\tprobeIntensity = %g ??\n", tTSReParsPtr->probeIntensity);
	DPrint("\tsuppressorFreq = %g ??\n", tTSReParsPtr->suppressorFreq);
	DPrint("\tnumSupprIntensities = %d ??\n",
	  tTSReParsPtr->numSupprIntensities);
	DPrint("\tinitialSupprIntensity = %g ??\n",
	  tTSReParsPtr->initialSupprIntensity);
	DPrint("\tdeltaSupprIntensity = %g ??\n",
	  tTSReParsPtr->deltaSupprIntensity);
	DPrint("\tstimulusDuration = %g ??\n", tTSReParsPtr->stimulusDuration);
	DPrint("\tsamplingInterval = %g ??\n", tTSReParsPtr->dt);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_TwoTSuppResp(FILE *fp)
{
	static const char	*funcName = "ReadPars_TwoTSuppResp";
	BOOLN	ok = TRUE;
	char	enabled[MAXLINE], outputMode[MAXLINE];
	int		numSupprIntensities;
	double	probeFreq, probeIntensity, suppressorFreq, initialSupprIntensity;
	double	deltaSupprIntensity, stimulusDuration, samplingInterval;

	if (!fp) {
		NotifyError("%s: File not selected.", funcName);
		return(FALSE);
	}
	if (!GetPars_ParFile(fp, "%s", enabled))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%s", outputMode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &probeFreq))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &probeIntensity))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &suppressorFreq))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%d", &numSupprIntensities))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &initialSupprIntensity))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &deltaSupprIntensity))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &stimulusDuration))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &samplingInterval))
		ok = FALSE;
	if (!ok) {
		NotifyError("%s: Failed to read parameters.", funcName);
		return(FALSE);
	}
	if (!SetPars_TwoTSuppResp(enabled, outputMode, probeFreq, probeIntensity,
	  suppressorFreq, numSupprIntensities, initialSupprIntensity,
	  deltaSupprIntensity, stimulusDuration, samplingInterval)) {
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
CheckData_TwoTSuppResp(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_TwoTSuppResp";

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
InitProcessVariables_TwoTSuppResp(EarObjectPtr data)
{
	static const char * funcName = "InitProcessVariables_TwoTSuppResp";
	char	indexedValue[MAXLINE];

	tTSReParsPtr->audModel = data;
	if ((tTSReParsPtr->stimulus = Init_EarObject(
	  STIM_PURETONE_MULTI_PROCESS_NAME)) == NULL) {
		NotifyError("%s: Could not initialise the 2 tone stimulus EarObject.",
		  funcName);
		return(FALSE);
	}

	ConnectOutSignalToIn_EarObject(tTSReParsPtr->stimulus,
	  tTSReParsPtr->audModel);

	if (!SetRealPar_ModuleMgr(tTSReParsPtr->stimulus, "num", 2.0)) {
		NotifyError("%s: Cannot set the number of pure tones.", funcName);
		return(FALSE);
	}
	if (!SetRealPar_ModuleMgr(tTSReParsPtr->stimulus, "duration",
	  tTSReParsPtr->stimulusDuration)) {
		NotifyError("%s: Cannot set the stimulus duration.", funcName);
		return(FALSE);
	}
	if (!SetRealPar_ModuleMgr(tTSReParsPtr->stimulus, "dt", tTSReParsPtr->dt)) {
		NotifyError("%s: Cannot set the stimulus sampling interval.",
		  funcName);
		return(FALSE);
	}
	sprintf(indexedValue, "0:%g", tTSReParsPtr->probeFreq);
	if (!SetPar_ModuleMgr(tTSReParsPtr->stimulus, "frequency", indexedValue)) {
		NotifyError("%s: Cannot set the probe tone frequency.", funcName);
		return(FALSE);
	}
	sprintf(indexedValue, "1:%g", tTSReParsPtr->suppressorFreq);
	if (!SetPar_ModuleMgr(tTSReParsPtr->stimulus, "frequency", indexedValue)) {
		NotifyError("%s: Cannot set the suppressor tone frequency.", funcName);
		return(FALSE);
	}
	sprintf(indexedValue, "0:%g", tTSReParsPtr->probeIntensity);
	if (!SetPar_ModuleMgr(tTSReParsPtr->stimulus, "intensity", indexedValue)) {
		NotifyError("%s: Cannot set the probe tone intensity.", funcName);
		return(FALSE);
	}

	if ((tTSReParsPtr->resultEarObj = InitResultEarObject_Utils(
	  tTSReParsPtr->numSupprIntensities, (ChanLen) (
	  tTSReParsPtr->stimulusDuration / tTSReParsPtr->dt + 1.5), 0.0,
	  tTSReParsPtr->dt, funcName)) == NULL) {
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
FreeProcessVariables_TwoTSuppResp(void)
{
	if (tTSReParsPtr->stimulus && tTSReParsPtr->audModel)
		DisconnectOutSignalFromIn_EarObject(tTSReParsPtr->stimulus,
		  tTSReParsPtr->audModel);
	Free_EarObject(&tTSReParsPtr->stimulus);
	Free_EarObject(&tTSReParsPtr->resultEarObj);
	
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
RunProcess_TwoTSuppResp(EarObjectPtr data)
{
	static const char	*funcName = "RunProcess_TwoTSuppResp";
	register	ChanData	 *inPtr, *outPtr;
	char	indexedIntensity[MAXLINE];
	int		i;
	ChanLen	j;
	double	supprIntensity;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckPars_TwoTSuppResp())
		return(FALSE);
	if (!CheckData_TwoTSuppResp(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "TTS Response process");
	if (!InitProcessVariables_TwoTSuppResp(data)) {
		NotifyError("%s: Could not initialise the process variables.",
		  funcName);
		return(FALSE);
	}
	for (i = 0, supprIntensity = tTSReParsPtr->initialSupprIntensity; i < 
	  tTSReParsPtr->numSupprIntensities; i++, supprIntensity +=
	  tTSReParsPtr->deltaSupprIntensity) {
	  	sprintf(indexedIntensity, "1:%g", supprIntensity);
		if (!SetPar_ModuleMgr(tTSReParsPtr->stimulus, "intensity",
		  indexedIntensity)) {
			NotifyError("%s: Cannot set the stimulus Tone 1 intensity.",
			  funcName);
			return(FALSE);
		}
		if (!RunProcess_ModuleMgr(tTSReParsPtr->stimulus)) {
			NotifyError("%s: Failed to run stimulus process.", funcName);
			FreeProcessVariables_TwoTSuppResp();
			return(FALSE);
		}
		if (!RunProcess_ModuleMgr(tTSReParsPtr->audModel)) {
			NotifyError("%s: Failed to run auditory model.", funcName);
			FreeProcessVariables_TwoTSuppResp();
			return(FALSE);
		}
		inPtr =  tTSReParsPtr->audModel->outSignal->channel[0];
		outPtr = tTSReParsPtr->resultEarObj->outSignal->channel[i];
		for (j = 0; j < tTSReParsPtr->audModel->outSignal->length; j++)
			*outPtr++ = *inPtr++;
		SetInfoChannelLabel_SignalData(tTSReParsPtr->resultEarObj->outSignal, i,
		  supprIntensity);
	}
	OutputResults_Utils(tTSReParsPtr->resultEarObj,
	  &tTSReParsPtr->outputModeList[tTSReParsPtr->outputMode],
	  UTILS_NORMAL_SIGNAL);
	FreeProcessVariables_TwoTSuppResp();
	return(TRUE);

}

