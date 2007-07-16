/**********************
 *
 * File:		PhaseFreqFuncs.c
 * Purpose:		This program calculates the phase functions for a simulation and
 *				different stimulus frequencies.
 * Comments:	Written using ModuleProducer version 1.2.7 (Jun 26 2000).
 * Author:		L. P. O'Mard
 * Created:		10 Jul 2000
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
#include "PhaseFreqFuncs.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

PFFParsPtr	pFFParsPtr = NULL;

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
Free_PhaseFreqFuncs(void)
{
	/* static const char	*funcName = "Free_PhaseFreqFuncs";  */

	if (pFFParsPtr == NULL)
		return(FALSE);
	if (pFFParsPtr->parSpec == GLOBAL) {
		free(pFFParsPtr);
		pFFParsPtr = NULL;
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
Init_PhaseFreqFuncs(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_PhaseFreqFuncs";

	if (parSpec == GLOBAL) {
		if (pFFParsPtr != NULL)
			Free_PhaseFreqFuncs();
		if ((pFFParsPtr = (PFFParsPtr) malloc(sizeof(PFFPars))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (pFFParsPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	pFFParsPtr->parSpec = parSpec;
	pFFParsPtr->enabledFlag = FALSE;
	pFFParsPtr->outputModeFlag = FALSE;
	pFFParsPtr->analysisOutputModeFlag = FALSE;
	pFFParsPtr->numFrequenciesFlag = FALSE;
	pFFParsPtr->stimulusDurationFlag = FALSE;
	pFFParsPtr->dtFlag = FALSE;
	pFFParsPtr->initialFrequencyFlag = FALSE;
	pFFParsPtr->deltaFrequencyFlag = FALSE;
	pFFParsPtr->numIntensitiesFlag = FALSE;
	pFFParsPtr->initialIntensityFlag = FALSE;
	pFFParsPtr->deltaIntensityFlag = FALSE;
	pFFParsPtr->enabled = 0;
	pFFParsPtr->outputMode = 0;
	pFFParsPtr->analysisOutputMode = 0;
	pFFParsPtr->numFrequencies = 0;
	pFFParsPtr->stimulusDuration = 0.0;
	pFFParsPtr->dt = 0.0;
	pFFParsPtr->initialFrequency = 0.0;
	pFFParsPtr->deltaFrequency = 0.0;
	pFFParsPtr->numIntensities = 0;
	pFFParsPtr->initialIntensity = 0.0;
	pFFParsPtr->deltaIntensity = 0.0;

	sprintf(pFFParsPtr->outputFile, NO_FILE);
	if ((pFFParsPtr->outputModeList = InitNameList_NSpecLists(
	  DiagModeList_NSpecLists(0), pFFParsPtr->outputFile)) == NULL)
		return(FALSE);
	pFFParsPtr->analysisOutputModeList = InitPhaseOutputModeList_Utils();
	pFFParsPtr->audModel = NULL;
	pFFParsPtr->stimulus = NULL;
	pFFParsPtr->analysis = NULL;
	pFFParsPtr->stimulusAnalysis = NULL;
	pFFParsPtr->resultEarObj = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_PhaseFreqFuncs(UniParPtr pars)
{
	static const char *funcName = "SetUniParList_PhaseFreqFuncs";

	if (pFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	SetPar_UniParMgr(&pars[PHASEFREQFUNCS_ENABLED], "PFF_ENABLED",
	  "Phase intensity function test state 'on' or 'off'.",
	  UNIPAR_BOOL,
	  &pFFParsPtr->enabled, NULL,
	  (void * (*)) SetEnabled_PhaseFreqFuncs);
	SetPar_UniParMgr(&pars[PHASEFREQFUNCS_OUTPUTMODE], "PFF_OUTPUT_MODE",
	  "Phase/frequency function test output 'off', 'screen' or <file name>.",
	  UNIPAR_NAME_SPEC_WITH_FILE,
	  &pFFParsPtr->outputMode, pFFParsPtr->outputModeList,
	  (void * (*)) SetOutputMode_PhaseFreqFuncs);
	SetPar_UniParMgr(&pars[PHASEFREQFUNCS_ANALYSISOUTPUTMODE],
	  "PFF_ANAL_OUTPUT",
	  "Analysis output mode ('phase' or 'degrees').",
	  UNIPAR_NAME_SPEC,
	  &pFFParsPtr->analysisOutputMode, pFFParsPtr->analysisOutputModeList,
	  (void * (*)) SetAnalysisOutputMode_PhaseFreqFuncs);
	SetPar_UniParMgr(&pars[PHASEFREQFUNCS_NUMFREQUENCIES], "PFF_NUM_FREQ",
	  "No. of test frequencies.",
	  UNIPAR_INT,
	  &pFFParsPtr->numFrequencies, NULL,
	  (void * (*)) SetNumFrequencies_PhaseFreqFuncs);
	SetPar_UniParMgr(&pars[PHASEFREQFUNCS_STIMULUSDURATION], "PFF_DURATION",
	  "Stimulus duration (s).",
	  UNIPAR_REAL,
	  &pFFParsPtr->stimulusDuration, NULL,
	  (void * (*)) SetStimulusDuration_PhaseFreqFuncs);
	SetPar_UniParMgr(&pars[PHASEFREQFUNCS_SAMPLINGINTERVAL], "PFF_DT",
	  "Stimulus Sampling interval, dt (s).",
	  UNIPAR_REAL,
	  &pFFParsPtr->dt, NULL,
	  (void * (*)) SetSamplingInterval_PhaseFreqFuncs);
	SetPar_UniParMgr(&pars[PHASEFREQFUNCS_INITIALFREQUENCY], "PFF_INIT_FREQ",
	  "Initial test frequency (Hz).",
	  UNIPAR_REAL,
	  &pFFParsPtr->initialFrequency, NULL,
	  (void * (*)) SetInitialFrequency_PhaseFreqFuncs);
	SetPar_UniParMgr(&pars[PHASEFREQFUNCS_DELTAFREQUENCY], "PFF_DELTA_FREQ",
	  "Frequency increment (Hz).",
	  UNIPAR_REAL,
	  &pFFParsPtr->deltaFrequency, NULL,
	  (void * (*)) SetDeltaFrequency_PhaseFreqFuncs);
	SetPar_UniParMgr(&pars[PHASEFREQFUNCS_NUMINTENSITIES], "PFF_NUM_INTENSTIES",
	  "No. of test intensities.",
	  UNIPAR_INT,
	  &pFFParsPtr->numIntensities, NULL,
	  (void * (*)) SetNumIntensities_PhaseFreqFuncs);
	SetPar_UniParMgr(&pars[PHASEFREQFUNCS_INITIALINTENSITY],
	  "PFF_INIT_INTENSITY",
	  "Initial test intensity (dB SPL).",
	  UNIPAR_REAL,
	  &pFFParsPtr->initialIntensity, NULL,
	  (void * (*)) SetInitialIntensity_PhaseFreqFuncs);
	SetPar_UniParMgr(&pars[PHASEFREQFUNCS_DELTAINTENSITY],
	  "PFF_DELTA_INTENSITY",
	  "Intensity increment (dB).",
	  UNIPAR_REAL,
	  &pFFParsPtr->deltaIntensity, NULL,
	  (void * (*)) SetDeltaIntensity_PhaseFreqFuncs);
	return(TRUE);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_PhaseFreqFuncs(char * enabled, char * outputMode,
  char * analysisOutputMode, int numFrequencies,
  double stimulusDuration, double samplingInterval, double initialFrequency,
  double deltaFrequency, int numIntensities, double initialIntensity,
  double deltaIntensity)
{
	static const char	*funcName = "SetPars_PhaseFreqFuncs";
	BOOLN	ok;

	ok = TRUE;
	if (!SetEnabled_PhaseFreqFuncs(enabled))
		ok = FALSE;
	if (!SetOutputMode_PhaseFreqFuncs(outputMode))
		ok = FALSE;
	if (!SetAnalysisOutputMode_PhaseFreqFuncs(analysisOutputMode))
		ok = FALSE;
	if (!SetNumFrequencies_PhaseFreqFuncs(numFrequencies))
		ok = FALSE;
	if (!SetStimulusDuration_PhaseFreqFuncs(stimulusDuration))
		ok = FALSE;
	if (!SetSamplingInterval_PhaseFreqFuncs(samplingInterval))
		ok = FALSE;
	if (!SetInitialFrequency_PhaseFreqFuncs(initialFrequency))
		ok = FALSE;
	if (!SetDeltaFrequency_PhaseFreqFuncs(deltaFrequency))
		ok = FALSE;
	if (!SetNumIntensities_PhaseFreqFuncs(numIntensities))
		ok = FALSE;
	if (!SetInitialIntensity_PhaseFreqFuncs(initialIntensity))
		ok = FALSE;
	if (!SetDeltaIntensity_PhaseFreqFuncs(deltaIntensity))
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
SetEnabled_PhaseFreqFuncs(char * theEnabled)
{
	static const char	*funcName = "SetEnabled_PhaseFreqFuncs";
	int		specifier;

	if (pFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theEnabled, BooleanList_NSpecLists(
	  0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError("%s: Illegal name (%s).", funcName, theEnabled);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	pFFParsPtr->enabledFlag = TRUE;
	pFFParsPtr->enabled = specifier;
	return(TRUE);

}

/****************************** SetOutputMode *********************************/

/*
 * This function sets the module's outputMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOutputMode_PhaseFreqFuncs(char * theOutputMode)
{
	static const char	*funcName = "SetOutputMode_PhaseFreqFuncs";

	if (pFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	pFFParsPtr->outputMode = IdentifyDiag_NSpecLists(theOutputMode,
	  pFFParsPtr->outputModeList);
	pFFParsPtr->outputModeFlag = TRUE;
	return(TRUE);

}

/****************************** SetAnalysisOutputMode *************************/

/*
 * This function sets the module's analysisOutputMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetAnalysisOutputMode_PhaseFreqFuncs(char * theAnalysisOutputMode)
{
	static const char	*funcName = "SetAnalysisOutputMode_PhaseFreqFuncs";
	int		specifier;

	if (pFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theAnalysisOutputMode,
	  pFFParsPtr->analysisOutputModeList)) ==
	  UTILS_PHASE_ANALYSISOUTPUTMODE_NULL) {
		NotifyError("%s: Illegal name (%s).", funcName, theAnalysisOutputMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	pFFParsPtr->analysisOutputModeFlag = TRUE;
	pFFParsPtr->analysisOutputMode = specifier;
	return(TRUE);

}

/****************************** SetNumFrequencies *****************************/

/*
 * This function sets the module's numFrequencies parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNumFrequencies_PhaseFreqFuncs(int theNumFrequencies)
{
	static const char	*funcName = "SetNumFrequencies_PhaseFreqFuncs";

	if (pFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	pFFParsPtr->numFrequenciesFlag = TRUE;
	pFFParsPtr->numFrequencies = theNumFrequencies;
	return(TRUE);

}

/****************************** SetStimulusDuration ***************************/

/*
 * This function sets the module's stimulusDuration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetStimulusDuration_PhaseFreqFuncs(double theStimulusDuration)
{
	static const char	*funcName = "SetStimulusDuration_PhaseFreqFuncs";

	if (pFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	pFFParsPtr->stimulusDurationFlag = TRUE;
	pFFParsPtr->stimulusDuration = theStimulusDuration;
	return(TRUE);

}

/****************************** SetSamplingInterval ***************************/

/*
 * This function sets the module's samplingInterval parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSamplingInterval_PhaseFreqFuncs(double theSamplingInterval)
{
	static const char	*funcName = "SetSamplingInterval_PhaseFreqFuncs";

	if (pFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	pFFParsPtr->dtFlag = TRUE;
	pFFParsPtr->dt = theSamplingInterval;
	return(TRUE);

}

/****************************** SetInitialFrequency ***************************/

/*
 * This function sets the module's initialFrequency parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetInitialFrequency_PhaseFreqFuncs(double theInitialFrequency)
{
	static const char	*funcName = "SetInitialFrequency_PhaseFreqFuncs";

	if (pFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	pFFParsPtr->initialFrequencyFlag = TRUE;
	pFFParsPtr->initialFrequency = theInitialFrequency;
	return(TRUE);

}

/****************************** SetDeltaFrequency *****************************/

/*
 * This function sets the module's deltaFrequency parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDeltaFrequency_PhaseFreqFuncs(double theDeltaFrequency)
{
	static const char	*funcName = "SetDeltaFrequency_PhaseFreqFuncs";

	if (pFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	pFFParsPtr->deltaFrequencyFlag = TRUE;
	pFFParsPtr->deltaFrequency = theDeltaFrequency;
	return(TRUE);

}

/****************************** SetNumIntensities *****************************/

/*
 * This function sets the module's numIntensities parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNumIntensities_PhaseFreqFuncs(int theNumIntensities)
{
	static const char	*funcName = "SetNumIntensities_PhaseFreqFuncs";

	if (pFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	pFFParsPtr->numIntensitiesFlag = TRUE;
	pFFParsPtr->numIntensities = theNumIntensities;
	return(TRUE);

}

/****************************** SetInitialIntensity ***************************/

/*
 * This function sets the module's initialIntensity parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetInitialIntensity_PhaseFreqFuncs(double theInitialIntensity)
{
	static const char	*funcName = "SetInitialIntensity_PhaseFreqFuncs";

	if (pFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	pFFParsPtr->initialIntensityFlag = TRUE;
	pFFParsPtr->initialIntensity = theInitialIntensity;
	return(TRUE);

}

/****************************** SetDeltaIntensity *****************************/

/*
 * This function sets the module's deltaIntensity parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDeltaIntensity_PhaseFreqFuncs(double theDeltaIntensity)
{
	static const char	*funcName = "SetDeltaIntensity_PhaseFreqFuncs";

	if (pFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	pFFParsPtr->deltaIntensityFlag = TRUE;
	pFFParsPtr->deltaIntensity = theDeltaIntensity;
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
CheckPars_PhaseFreqFuncs(void)
{
	static const char	*funcName = "CheckPars_PhaseFreqFuncs";
	BOOLN	ok;

	ok = TRUE;
	if (pFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!pFFParsPtr->enabledFlag) {
		NotifyError("%s: enabled variable not set.", funcName);
		ok = FALSE;
	}
	if (!pFFParsPtr->outputModeFlag) {
		NotifyError("%s: outputMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!pFFParsPtr->analysisOutputModeFlag) {
		NotifyError("%s: analysisOutputMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!pFFParsPtr->numFrequenciesFlag) {
		NotifyError("%s: numFrequencies variable not set.", funcName);
		ok = FALSE;
	}
	if (!pFFParsPtr->stimulusDurationFlag) {
		NotifyError("%s: stimulusDuration variable not set.", funcName);
		ok = FALSE;
	}
	if (!pFFParsPtr->dtFlag) {
		NotifyError("%s: dt variable not set.", funcName);
		ok = FALSE;
	}
	if (!pFFParsPtr->initialFrequencyFlag) {
		NotifyError("%s: initialFrequency variable not set.", funcName);
		ok = FALSE;
	}
	if (!pFFParsPtr->deltaFrequencyFlag) {
		NotifyError("%s: deltaFrequency variable not set.", funcName);
		ok = FALSE;
	}
	if (!pFFParsPtr->numIntensitiesFlag) {
		NotifyError("%s: numIntensities variable not set.", funcName);
		ok = FALSE;
	}
	if (!pFFParsPtr->initialIntensityFlag) {
		NotifyError("%s: initialIntensity variable not set.", funcName);
		ok = FALSE;
	}
	if (!pFFParsPtr->deltaIntensityFlag) {
		NotifyError("%s: deltaIntensity variable not set.", funcName);
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
PrintPars_PhaseFreqFuncs(void)
{
	static const char	*funcName = "PrintPars_PhaseFreqFuncs";

	if (!CheckPars_PhaseFreqFuncs()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("?? PhaseFreqFuncs Module Parameters:-\n");
	DPrint("\tenabled = %s \n",
	  BooleanList_NSpecLists(pFFParsPtr->enabled)->name);
	DPrint("\toutputMode = %s \n",
	  pFFParsPtr->outputModeList[pFFParsPtr->outputMode].name);
	DPrint("\tanalysisOutputMode = %s \n",
	  pFFParsPtr->analysisOutputModeList[pFFParsPtr->analysisOutputMode].name);
	DPrint("\tnumFrequencies = %d ??\n", pFFParsPtr->numFrequencies);
	DPrint("\tstimulusDuration = %g ??\n", pFFParsPtr->stimulusDuration);
	DPrint("\tsamplingInterval = %g ??\n", pFFParsPtr->dt);
	DPrint("\tinitialFrequency = %g ??\n", pFFParsPtr->initialFrequency);
	DPrint("\tdeltaFrequency = %g ??\n", pFFParsPtr->deltaFrequency);
	DPrint("\tnumIntensities = %d ??\n", pFFParsPtr->numIntensities);
	DPrint("\tinitialIntensity = %g ??\n", pFFParsPtr->initialIntensity);
	DPrint("\tdeltaIntensity = %g ??\n", pFFParsPtr->deltaIntensity);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_PhaseFreqFuncs(FILE *fp)
{
	static const char	*funcName = "ReadPars_PhaseFreqFuncs";
	BOOLN	ok = TRUE;
	char	enabled[MAXLINE], outputMode[MAX_FILE_PATH];
	char	analysisOutputMode[MAXLINE];
	int		numFrequencies, numIntensities;
	double	stimulusDuration, samplingInterval, initialFrequency;
	double	deltaFrequency, initialIntensity, deltaIntensity;

	if (!fp) {
		NotifyError("%s: File not selected.", funcName);
		return(FALSE);
	}
	if (!GetPars_ParFile(fp, "%s", enabled))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%s", outputMode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%s", analysisOutputMode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &stimulusDuration))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &samplingInterval))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%d", &numFrequencies))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &initialFrequency))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &deltaFrequency))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%d", &numIntensities))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &initialIntensity))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &deltaIntensity))
		ok = FALSE;
	if (!ok) {
		NotifyError("%s: Failed to read parameters.", funcName);
		return(FALSE);
	}
	if (!SetPars_PhaseFreqFuncs(enabled, outputMode, analysisOutputMode,
	  numFrequencies, stimulusDuration, samplingInterval, initialFrequency,
	  deltaFrequency, numIntensities, initialIntensity, deltaIntensity)) {
		NotifyError("%s: Could not set parameters.", funcName);
		return(FALSE);
	}
	return(TRUE);

}

/****************************** InitProcessVariables **************************/

/*
 * This function initialises the process variables, making connections to the
 * simulation process as required.
 * It returns FALSE if it fails in any way.
 */

BOOLN
InitProcessVariables_PhaseFreqFuncs(EarObjectPtr data)
{
	static const char * funcName = "InitProcessVariables_PhaseFreqFuncs";

	pFFParsPtr->audModel = data;
	if ((pFFParsPtr->stimulus = Init_EarObject(STIM_PURETONE_PROCESS_NAME)) ==
	  NULL) {
		NotifyError("%s: Could not initialise the stimulus EarObject.",
		  funcName);
		return(FALSE);
	}
	if ((pFFParsPtr->analysis = Init_EarObject(ANA_FOURIERT_PROCESS_NAME)) ==
	  NULL) {
		NotifyError("%s: Could not initialise the analysis EarObject.",
		  funcName);
		return(FALSE);
	}
	if ((pFFParsPtr->stimulusAnalysis = Init_EarObject(
	  ANA_FOURIERT_PROCESS_NAME)) == NULL) {
		NotifyError("%s: Could not initialise the stimulus analysis EarObject.",
		  funcName);
		return(FALSE);
	}

	ConnectOutSignalToIn_EarObject(pFFParsPtr->stimulus, pFFParsPtr->audModel);
	ConnectOutSignalToIn_EarObject(pFFParsPtr->stimulus,
	  pFFParsPtr->stimulusAnalysis);
	ConnectOutSignalToIn_EarObject(pFFParsPtr->audModel, pFFParsPtr->analysis);

	if (!SetRealPar_ModuleMgr(pFFParsPtr->stimulus, "duration",
	  pFFParsPtr->stimulusDuration)) {
		NotifyError("%s: Cannot set the stimulus duration.", funcName);
		return(FALSE);
	}
	if (!SetRealPar_ModuleMgr(pFFParsPtr->stimulus, "dt", pFFParsPtr->dt)) {
		NotifyError("%s: Cannot set the stimulus sampling interval.", funcName);
		return(FALSE);
	}
	if (!SetPar_ModuleMgr(pFFParsPtr->stimulusAnalysis, "output_mode",
	  "phase")) {
		NotifyError("%s: Cannot set the stimulusanalysis output mode.",
		  funcName);
		return(FALSE);
	}
	if (!SetPar_ModuleMgr(pFFParsPtr->analysis, "output_mode", "phase")) {
		NotifyError("%s: Cannot set the analysis output mode.", funcName);
		return(FALSE);
	}

	if ((pFFParsPtr->resultEarObj = InitResultEarObject_Utils(
	  pFFParsPtr->numIntensities, pFFParsPtr->numFrequencies,
	  pFFParsPtr->initialFrequency, pFFParsPtr->deltaFrequency, funcName)) ==
	  NULL) {
		NotifyError("%s: Could not initialise results EarObject", funcName);
		return(FALSE);
	}
	SetInfoSampleTitle_SignalData(pFFParsPtr->resultEarObj->outSignal,
	  "Frequency (Hz)");
	return(TRUE);

}

/****************************** FreeProcessVariables **************************/

/*
 * This routine unconnects and frees the process variables.
 */

void
FreeProcessVariables_PhaseFreqFuncs(void)
{
	if (pFFParsPtr->stimulus && pFFParsPtr->audModel)
		DisconnectOutSignalFromIn_EarObject(pFFParsPtr->stimulus,
		  pFFParsPtr->audModel);
	if (pFFParsPtr->stimulus && pFFParsPtr->stimulusAnalysis)
		DisconnectOutSignalFromIn_EarObject(pFFParsPtr->stimulus,
		  pFFParsPtr->stimulusAnalysis);
	if (pFFParsPtr->audModel && pFFParsPtr->analysis)
		DisconnectOutSignalFromIn_EarObject(pFFParsPtr->audModel,
		  pFFParsPtr->analysis);
	Free_EarObject(&pFFParsPtr->stimulus);
	Free_EarObject(&pFFParsPtr->analysis);
	Free_EarObject(&pFFParsPtr->stimulusAnalysis);
	Free_EarObject(&pFFParsPtr->resultEarObj);
	
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
RunProcess_PhaseFreqFuncs(EarObjectPtr data)
{
	static const char	*funcName = "RunProcess_PhaseFreqFuncs";
	int		i, j;
	double	intensity, stimulusPhase, outputPhase, relativePhase, frequency;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckPars_PhaseFreqFuncs())
		return(FALSE);
	if (!InitProcessVariables_PhaseFreqFuncs(data)) {
		NotifyError("%s: Could not initialise the process variables.",
		  funcName);
		FreeProcessVariables_PhaseFreqFuncs();
		return(FALSE);
	}
	for (i = 0, intensity = pFFParsPtr->initialIntensity; i <
	  pFFParsPtr->numIntensities; i++, intensity +=
	  pFFParsPtr->deltaIntensity) {
		if (!SetRealPar_ModuleMgr(pFFParsPtr->stimulus, "intensity",
		  intensity)) {
			NotifyError("%s: Cannot set the stimulus intensity.", funcName);
			FreeProcessVariables_PhaseFreqFuncs();
			return(FALSE);
		}
		for (j = 0, frequency = pFFParsPtr->initialFrequency; j <
		  pFFParsPtr->numFrequencies; j++, frequency +=
		  pFFParsPtr->deltaFrequency) {
			if (!SetRealPar_ModuleMgr(pFFParsPtr->stimulus, "frequency",
			  frequency)) {
				NotifyError("%s: Cannot set the stimulus intensity.", funcName);
				FreeProcessVariables_PhaseFreqFuncs();
				return(FALSE);
			}
			if (!RunProcess_ModuleMgr(pFFParsPtr->stimulus)) {
				NotifyError("%s: Failed to run stimulus process.", funcName);
				FreeProcessVariables_PhaseFreqFuncs();
				return(FALSE);
			}
			if (!RunProcess_ModuleMgr(pFFParsPtr->stimulusAnalysis)) {
				NotifyError("%s: Failed to run stimulus analysis process.",
				  funcName);
				FreeProcessVariables_PhaseFreqFuncs();
				return(FALSE);
			}
			stimulusPhase = GetFreqIndexSample_Utils(
			  pFFParsPtr->stimulusAnalysis, frequency);
			if (!RunProcess_ModuleMgr(pFFParsPtr->audModel)) {
				NotifyError("%s: Failed to run auditory model.", funcName);
				FreeProcessVariables_PhaseFreqFuncs();
				return(FALSE);
			}
			if (!RunProcess_ModuleMgr(pFFParsPtr->analysis)) {
				NotifyError("%s: Failed to run analysis process.", funcName);
				FreeProcessVariables_PhaseFreqFuncs();
				return(FALSE);
			}
			outputPhase = GetFreqIndexSample_Utils(pFFParsPtr->analysis,
			  frequency);
			relativePhase = stimulusPhase - outputPhase;
			pFFParsPtr->resultEarObj->outSignal->channel[i][j] =
			  CalcPhase_Utils(relativePhase, pFFParsPtr->deltaFrequency,
			  pFFParsPtr->analysisOutputMode, (j == 0));
		}
		SetInfoChannelLabel_SignalData(pFFParsPtr->resultEarObj->outSignal, i,
		  intensity);
	}
	OutputResults_Utils(pFFParsPtr->resultEarObj, &pFFParsPtr->outputModeList[
	  pFFParsPtr->outputMode], UTILS_NORMAL_SIGNAL);
	FreeProcessVariables_PhaseFreqFuncs();
	return(TRUE);

}

