/**********************
 *
 * File:		PhaseIntFuncs.c
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
#include "PhaseIntFuncs.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

PIFParsPtr	pIFParsPtr = NULL;

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
Free_PhaseIntFuncs(void)
{
	/* static const char	*funcName = "Free_PhaseIntFuncs";  */

	if (pIFParsPtr == NULL)
		return(FALSE);
	if (pIFParsPtr->parSpec == GLOBAL) {
		free(pIFParsPtr);
		pIFParsPtr = NULL;
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
Init_PhaseIntFuncs(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_PhaseIntFuncs";

	if (parSpec == GLOBAL) {
		if (pIFParsPtr != NULL)
			Free_PhaseIntFuncs();
		if ((pIFParsPtr = (PIFParsPtr) malloc(sizeof(PIFPars))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (pIFParsPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	pIFParsPtr->parSpec = parSpec;
	pIFParsPtr->enabledFlag = FALSE;
	pIFParsPtr->outputModeFlag = FALSE;
	pIFParsPtr->analysisOutputModeFlag = FALSE;
	pIFParsPtr->numFrequenciesFlag = FALSE;
	pIFParsPtr->stimulusDurationFlag = FALSE;
	pIFParsPtr->dtFlag = FALSE;
	pIFParsPtr->initialFrequencyFlag = FALSE;
	pIFParsPtr->deltaFrequencyFlag = FALSE;
	pIFParsPtr->numIntensitiesFlag = FALSE;
	pIFParsPtr->initialIntensityFlag = FALSE;
	pIFParsPtr->deltaIntensityFlag = FALSE;
	pIFParsPtr->enabled = 0;
	pIFParsPtr->outputMode = 0;
	pIFParsPtr->analysisOutputMode = 0;
	pIFParsPtr->numFrequencies = 0;
	pIFParsPtr->stimulusDuration = 0.0;
	pIFParsPtr->dt = 0.0;
	pIFParsPtr->initialFrequency = 0.0;
	pIFParsPtr->deltaFrequency = 0.0;
	pIFParsPtr->numIntensities = 0;
	pIFParsPtr->initialIntensity = 0.0;
	pIFParsPtr->deltaIntensity = 0.0;

	sprintf(pIFParsPtr->outputFile, NO_FILE);
	if ((pIFParsPtr->outputModeList = InitNameList_NSpecLists(
	  DiagModeList_NSpecLists(0), pIFParsPtr->outputFile)) == NULL)
		return(FALSE);
	pIFParsPtr->analysisOutputModeList = InitPhaseOutputModeList_Utils();
	pIFParsPtr->audModel = NULL;
	pIFParsPtr->stimulus = NULL;
	pIFParsPtr->analysis = NULL;
	pIFParsPtr->stimulusAnalysis = NULL;
	pIFParsPtr->resultEarObj = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_PhaseIntFuncs(UniParPtr pars)
{
	static const char *funcName = "SetUniParList_PhaseIntFuncs";

	if (pIFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	SetPar_UniParMgr(&pars[PHASEINTFUNCS_ENABLED], "PIF_ENABLED",
	  "Phase intensity function test state 'on' or 'off'.",
	  UNIPAR_BOOL,
	  &pIFParsPtr->enabled, NULL,
	  (void * (*)) SetEnabled_PhaseIntFuncs);
	SetPar_UniParMgr(&pars[PHASEINTFUNCS_OUTPUTMODE], "PIF_OUTPUT_MODE",
	  "I/O function test output 'off', 'screen' or <file name>.",
	  UNIPAR_NAME_SPEC_WITH_FILE,
	  &pIFParsPtr->outputMode, pIFParsPtr->outputModeList,
	  (void * (*)) SetOutputMode_PhaseIntFuncs);
	SetPar_UniParMgr(&pars[PHASEINTFUNCS_ANALYSISOUTPUTMODE], "PIF_ANAL_OUTPUT",
	  "Analysis output mode ('phase' or 'degrees').",
	  UNIPAR_NAME_SPEC,
	  &pIFParsPtr->analysisOutputMode, pIFParsPtr->analysisOutputModeList,
	  (void * (*)) SetAnalysisOutputMode_PhaseIntFuncs);
	SetPar_UniParMgr(&pars[PHASEINTFUNCS_NUMFREQUENCIES], "PIF_NUM_FREQ",
	  "No. of test frequencies.",
	  UNIPAR_INT,
	  &pIFParsPtr->numFrequencies, NULL,
	  (void * (*)) SetNumFrequencies_PhaseIntFuncs);
	SetPar_UniParMgr(&pars[PHASEINTFUNCS_STIMULUSDURATION], "PIF_DURATION",
	  "Stimulus duration (s).",
	  UNIPAR_REAL,
	  &pIFParsPtr->stimulusDuration, NULL,
	  (void * (*)) SetStimulusDuration_PhaseIntFuncs);
	SetPar_UniParMgr(&pars[PHASEINTFUNCS_SAMPLINGINTERVAL], "PIF_DT",
	  "Stimulus Sampling interval, dt (s).",
	  UNIPAR_REAL,
	  &pIFParsPtr->dt, NULL,
	  (void * (*)) SetSamplingInterval_PhaseIntFuncs);
	SetPar_UniParMgr(&pars[PHASEINTFUNCS_INITIALFREQUENCY], "PIF_INIT_FREQ",
	  "Initial test frequency (Hz).",
	  UNIPAR_REAL,
	  &pIFParsPtr->initialFrequency, NULL,
	  (void * (*)) SetInitialFrequency_PhaseIntFuncs);
	SetPar_UniParMgr(&pars[PHASEINTFUNCS_DELTAFREQUENCY], "PIF_DELTA_FREQ",
	  "Frequency increment (Hz).",
	  UNIPAR_REAL,
	  &pIFParsPtr->deltaFrequency, NULL,
	  (void * (*)) SetDeltaFrequency_PhaseIntFuncs);
	SetPar_UniParMgr(&pars[PHASEINTFUNCS_NUMINTENSITIES], "PIF_NUM_INTENSTIES",
	  "No. of test intensities.",
	  UNIPAR_INT,
	  &pIFParsPtr->numIntensities, NULL,
	  (void * (*)) SetNumIntensities_PhaseIntFuncs);
	SetPar_UniParMgr(&pars[PHASEINTFUNCS_INITIALINTENSITY],
	  "PIF_INIT_INTENSITY",
	  "Initial test intensity (dB SPL).",
	  UNIPAR_REAL,
	  &pIFParsPtr->initialIntensity, NULL,
	  (void * (*)) SetInitialIntensity_PhaseIntFuncs);
	SetPar_UniParMgr(&pars[PHASEINTFUNCS_DELTAINTENSITY], "PIF_DELTA_INTENSITY",
	  "Intensity increment (dB).",
	  UNIPAR_REAL,
	  &pIFParsPtr->deltaIntensity, NULL,
	  (void * (*)) SetDeltaIntensity_PhaseIntFuncs);
	return(TRUE);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_PhaseIntFuncs(char * enabled, char * outputMode,
  char * analysisOutputMode, int numFrequencies,
  double stimulusDuration, double samplingInterval, double initialFrequency,
  double deltaFrequency, int numIntensities, double initialIntensity,
  double deltaIntensity)
{
	static const char	*funcName = "SetPars_PhaseIntFuncs";
	BOOLN	ok;

	ok = TRUE;
	if (!SetEnabled_PhaseIntFuncs(enabled))
		ok = FALSE;
	if (!SetOutputMode_PhaseIntFuncs(outputMode))
		ok = FALSE;
	if (!SetAnalysisOutputMode_PhaseIntFuncs(analysisOutputMode))
		ok = FALSE;
	if (!SetNumFrequencies_PhaseIntFuncs(numFrequencies))
		ok = FALSE;
	if (!SetStimulusDuration_PhaseIntFuncs(stimulusDuration))
		ok = FALSE;
	if (!SetSamplingInterval_PhaseIntFuncs(samplingInterval))
		ok = FALSE;
	if (!SetInitialFrequency_PhaseIntFuncs(initialFrequency))
		ok = FALSE;
	if (!SetDeltaFrequency_PhaseIntFuncs(deltaFrequency))
		ok = FALSE;
	if (!SetNumIntensities_PhaseIntFuncs(numIntensities))
		ok = FALSE;
	if (!SetInitialIntensity_PhaseIntFuncs(initialIntensity))
		ok = FALSE;
	if (!SetDeltaIntensity_PhaseIntFuncs(deltaIntensity))
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
SetEnabled_PhaseIntFuncs(char * theEnabled)
{
	static const char	*funcName = "SetEnabled_PhaseIntFuncs";
	int		specifier;

	if (pIFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theEnabled, BooleanList_NSpecLists(
	  0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError("%s: Illegal name (%s).", funcName, theEnabled);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	pIFParsPtr->enabledFlag = TRUE;
	pIFParsPtr->enabled = specifier;
	return(TRUE);

}

/****************************** SetOutputMode *********************************/

/*
 * This function sets the module's outputMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOutputMode_PhaseIntFuncs(char * theOutputMode)
{
	static const char	*funcName = "SetOutputMode_PhaseIntFuncs";

	if (pIFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	pIFParsPtr->outputMode = IdentifyDiag_NSpecLists(theOutputMode,
	  pIFParsPtr->outputModeList);
	pIFParsPtr->outputModeFlag = TRUE;
	return(TRUE);

}

/****************************** SetAnalysisOutputMode *************************/

/*
 * This function sets the module's analysisOutputMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetAnalysisOutputMode_PhaseIntFuncs(char * theAnalysisOutputMode)
{
	static const char	*funcName = "SetAnalysisOutputMode_PhaseIntFuncs";
	int		specifier;

	if (pIFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theAnalysisOutputMode,
	  pIFParsPtr->analysisOutputModeList)) ==
	  UTILS_PHASE_ANALYSISOUTPUTMODE_NULL) {
		NotifyError("%s: Illegal name (%s).", funcName, theAnalysisOutputMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	pIFParsPtr->analysisOutputModeFlag = TRUE;
	pIFParsPtr->analysisOutputMode = specifier;
	return(TRUE);

}

/****************************** SetNumFrequencies *****************************/

/*
 * This function sets the module's numFrequencies parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNumFrequencies_PhaseIntFuncs(int theNumFrequencies)
{
	static const char	*funcName = "SetNumFrequencies_PhaseIntFuncs";

	if (pIFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	pIFParsPtr->numFrequenciesFlag = TRUE;
	pIFParsPtr->numFrequencies = theNumFrequencies;
	return(TRUE);

}

/****************************** SetStimulusDuration ***************************/

/*
 * This function sets the module's stimulusDuration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetStimulusDuration_PhaseIntFuncs(double theStimulusDuration)
{
	static const char	*funcName = "SetStimulusDuration_PhaseIntFuncs";

	if (pIFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	pIFParsPtr->stimulusDurationFlag = TRUE;
	pIFParsPtr->stimulusDuration = theStimulusDuration;
	return(TRUE);

}

/****************************** SetSamplingInterval ***************************/

/*
 * This function sets the module's samplingInterval parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSamplingInterval_PhaseIntFuncs(double theSamplingInterval)
{
	static const char	*funcName = "SetSamplingInterval_PhaseIntFuncs";

	if (pIFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	pIFParsPtr->dtFlag = TRUE;
	pIFParsPtr->dt = theSamplingInterval;
	return(TRUE);

}

/****************************** SetInitialFrequency ***************************/

/*
 * This function sets the module's initialFrequency parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetInitialFrequency_PhaseIntFuncs(double theInitialFrequency)
{
	static const char	*funcName = "SetInitialFrequency_PhaseIntFuncs";

	if (pIFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	pIFParsPtr->initialFrequencyFlag = TRUE;
	pIFParsPtr->initialFrequency = theInitialFrequency;
	return(TRUE);

}

/****************************** SetDeltaFrequency *****************************/

/*
 * This function sets the module's deltaFrequency parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDeltaFrequency_PhaseIntFuncs(double theDeltaFrequency)
{
	static const char	*funcName = "SetDeltaFrequency_PhaseIntFuncs";

	if (pIFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	pIFParsPtr->deltaFrequencyFlag = TRUE;
	pIFParsPtr->deltaFrequency = theDeltaFrequency;
	return(TRUE);

}

/****************************** SetNumIntensities *****************************/

/*
 * This function sets the module's numIntensities parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNumIntensities_PhaseIntFuncs(int theNumIntensities)
{
	static const char	*funcName = "SetNumIntensities_PhaseIntFuncs";

	if (pIFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	pIFParsPtr->numIntensitiesFlag = TRUE;
	pIFParsPtr->numIntensities = theNumIntensities;
	return(TRUE);

}

/****************************** SetInitialIntensity ***************************/

/*
 * This function sets the module's initialIntensity parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetInitialIntensity_PhaseIntFuncs(double theInitialIntensity)
{
	static const char	*funcName = "SetInitialIntensity_PhaseIntFuncs";

	if (pIFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	pIFParsPtr->initialIntensityFlag = TRUE;
	pIFParsPtr->initialIntensity = theInitialIntensity;
	return(TRUE);

}

/****************************** SetDeltaIntensity *****************************/

/*
 * This function sets the module's deltaIntensity parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDeltaIntensity_PhaseIntFuncs(double theDeltaIntensity)
{
	static const char	*funcName = "SetDeltaIntensity_PhaseIntFuncs";

	if (pIFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	pIFParsPtr->deltaIntensityFlag = TRUE;
	pIFParsPtr->deltaIntensity = theDeltaIntensity;
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
CheckPars_PhaseIntFuncs(void)
{
	static const char	*funcName = "CheckPars_PhaseIntFuncs";
	BOOLN	ok;

	ok = TRUE;
	if (pIFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!pIFParsPtr->enabledFlag) {
		NotifyError("%s: enabled variable not set.", funcName);
		ok = FALSE;
	}
	if (!pIFParsPtr->outputModeFlag) {
		NotifyError("%s: outputMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!pIFParsPtr->analysisOutputModeFlag) {
		NotifyError("%s: analysisOutputMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!pIFParsPtr->numFrequenciesFlag) {
		NotifyError("%s: numFrequencies variable not set.", funcName);
		ok = FALSE;
	}
	if (!pIFParsPtr->stimulusDurationFlag) {
		NotifyError("%s: stimulusDuration variable not set.", funcName);
		ok = FALSE;
	}
	if (!pIFParsPtr->dtFlag) {
		NotifyError("%s: dt variable not set.", funcName);
		ok = FALSE;
	}
	if (!pIFParsPtr->initialFrequencyFlag) {
		NotifyError("%s: initialFrequency variable not set.", funcName);
		ok = FALSE;
	}
	if (!pIFParsPtr->deltaFrequencyFlag) {
		NotifyError("%s: deltaFrequency variable not set.", funcName);
		ok = FALSE;
	}
	if (!pIFParsPtr->numIntensitiesFlag) {
		NotifyError("%s: numIntensities variable not set.", funcName);
		ok = FALSE;
	}
	if (!pIFParsPtr->initialIntensityFlag) {
		NotifyError("%s: initialIntensity variable not set.", funcName);
		ok = FALSE;
	}
	if (!pIFParsPtr->deltaIntensityFlag) {
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
PrintPars_PhaseIntFuncs(void)
{
	static const char	*funcName = "PrintPars_PhaseIntFuncs";

	if (!CheckPars_PhaseIntFuncs()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("?? PhaseIntFuncs Module Parameters:-\n");
	DPrint("\tenabled = %s \n",
	  BooleanList_NSpecLists(pIFParsPtr->enabled)->name);
	DPrint("\toutputMode = %s \n",
	  pIFParsPtr->outputModeList[pIFParsPtr->outputMode].name);
	DPrint("\tanalysisOutputMode = %s \n",
	  pIFParsPtr->analysisOutputModeList[pIFParsPtr->analysisOutputMode].name);
	DPrint("\tnumFrequencies = %d ??\n", pIFParsPtr->numFrequencies);
	DPrint("\tstimulusDuration = %g ??\n", pIFParsPtr->stimulusDuration);
	DPrint("\tsamplingInterval = %g ??\n", pIFParsPtr->dt);
	DPrint("\tinitialFrequency = %g ??\n", pIFParsPtr->initialFrequency);
	DPrint("\tdeltaFrequency = %g ??\n", pIFParsPtr->deltaFrequency);
	DPrint("\tnumIntensities = %d ??\n", pIFParsPtr->numIntensities);
	DPrint("\tinitialIntensity = %g ??\n", pIFParsPtr->initialIntensity);
	DPrint("\tdeltaIntensity = %g ??\n", pIFParsPtr->deltaIntensity);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_PhaseIntFuncs(FILE *fp)
{
	static const char	*funcName = "ReadPars_PhaseIntFuncs";
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
	if (!SetPars_PhaseIntFuncs(enabled, outputMode, analysisOutputMode,
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
InitProcessVariables_PhaseIntFuncs(EarObjectPtr data)
{
	static const char * funcName = "InitProcessVariables_PhaseIntFuncs";

	pIFParsPtr->audModel = data;
	if ((pIFParsPtr->stimulus = Init_EarObject(STIM_PURETONE_PROCESS_NAME)) ==
	  NULL) {
		NotifyError("%s: Could not initialise the stimulus EarObject.",
		  funcName);
		return(FALSE);
	}
	if ((pIFParsPtr->analysis = Init_EarObject(ANA_FOURIERT_PROCESS_NAME)) ==
	  NULL) {
		NotifyError("%s: Could not initialise the analysis EarObject.",
		  funcName);
		return(FALSE);
	}
	if ((pIFParsPtr->stimulusAnalysis = Init_EarObject(
	  ANA_FOURIERT_PROCESS_NAME)) == NULL) {
		NotifyError("%s: Could not initialise the stimulus analysis EarObject.",
		  funcName);
		return(FALSE);
	}

	ConnectOutSignalToIn_EarObject(pIFParsPtr->stimulus, pIFParsPtr->audModel);
	ConnectOutSignalToIn_EarObject(pIFParsPtr->stimulus,
	  pIFParsPtr->stimulusAnalysis);
	ConnectOutSignalToIn_EarObject(pIFParsPtr->audModel, pIFParsPtr->analysis);

	if (!SetRealPar_ModuleMgr(pIFParsPtr->stimulus, "duration",
	  pIFParsPtr->stimulusDuration)) {
		NotifyError("%s: Cannot set the stimulus duration.", funcName);
		return(FALSE);
	}
	if (!SetRealPar_ModuleMgr(pIFParsPtr->stimulus, "dt", pIFParsPtr->dt)) {
		NotifyError("%s: Cannot set the stimulus sampling interval.", funcName);
		return(FALSE);
	}
	if (!SetPar_ModuleMgr(pIFParsPtr->stimulusAnalysis, "output_mode",
	  "phase")) {
		NotifyError("%s: Cannot set the stimulusanalysis output mode.",
		  funcName);
		return(FALSE);
	}
	if (!SetPar_ModuleMgr(pIFParsPtr->analysis, "output_mode", "phase")) {
		NotifyError("%s: Cannot set the analysis output mode.", funcName);
		return(FALSE);
	}

	if ((pIFParsPtr->resultEarObj = InitResultEarObject_Utils(
	  pIFParsPtr->numFrequencies, pIFParsPtr->numIntensities,
	  pIFParsPtr->initialIntensity, pIFParsPtr->deltaIntensity, funcName)) ==
	  NULL) {
		NotifyError("%s: Could not initialise results EarObject", funcName);
		return(FALSE);
	}
	SetInfoSampleTitle_SignalData(pIFParsPtr->resultEarObj->outSignal,
	  "Intensity (dB SPL)");
	return(TRUE);

}

/****************************** FreeProcessVariables **************************/

/*
 * This routine unconnects and frees the process variables.
 */

void
FreeProcessVariables_PhaseIntFuncs(void)
{
	if (pIFParsPtr->stimulus && pIFParsPtr->audModel)
		DisconnectOutSignalFromIn_EarObject(pIFParsPtr->stimulus,
		  pIFParsPtr->audModel);
	if (pIFParsPtr->stimulus && pIFParsPtr->stimulusAnalysis)
		DisconnectOutSignalFromIn_EarObject(pIFParsPtr->stimulus,
		  pIFParsPtr->stimulusAnalysis);
	if (pIFParsPtr->audModel && pIFParsPtr->analysis)
		DisconnectOutSignalFromIn_EarObject(pIFParsPtr->audModel,
		  pIFParsPtr->analysis);
	Free_EarObject(&pIFParsPtr->stimulus);
	Free_EarObject(&pIFParsPtr->analysis);
	Free_EarObject(&pIFParsPtr->stimulusAnalysis);
	Free_EarObject(&pIFParsPtr->resultEarObj);
	
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
RunProcess_PhaseIntFuncs(EarObjectPtr data)
{
	static const char	*funcName = "RunProcess_PhaseIntFuncs";
	int		i, j;
	double	intensity, stimulusPhase, outputPhase, relativePhase, frequency;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckPars_PhaseIntFuncs())
		return(FALSE);
	if (!InitProcessVariables_PhaseIntFuncs(data)) {
		NotifyError("%s: Could not initialise the process variables.",
		  funcName);
		FreeProcessVariables_PhaseIntFuncs();
		return(FALSE);
	}
	for (i = 0, frequency = pIFParsPtr->initialFrequency; i <
	  pIFParsPtr->numFrequencies; i++, frequency +=
	  pIFParsPtr->deltaFrequency) {
		if (!SetRealPar_ModuleMgr(pIFParsPtr->stimulus, "frequency",
		  frequency)) {
			NotifyError("%s: Cannot set the stimulus frequency.", funcName);
			return(FALSE);
		}
		for (j = 0, intensity = pIFParsPtr->initialIntensity; j <
		  pIFParsPtr->numIntensities; j++, intensity +=
		  pIFParsPtr->deltaIntensity) {
			if (!SetRealPar_ModuleMgr(pIFParsPtr->stimulus, "intensity",
			  intensity)) {
				NotifyError("%s: Cannot set the stimulus intensity.", funcName);
				FreeProcessVariables_PhaseIntFuncs();
				return(FALSE);
			}
			if (!RunProcess_ModuleMgr(pIFParsPtr->stimulus)) {
				NotifyError("%s: Failed to run stimulus process.", funcName);
				FreeProcessVariables_PhaseIntFuncs();
				return(FALSE);
			}
			if (!RunProcess_ModuleMgr(pIFParsPtr->stimulusAnalysis)) {
				NotifyError("%s: Failed to run stimulus analysis process.",
				  funcName);
				FreeProcessVariables_PhaseIntFuncs();
				return(FALSE);
			}
			stimulusPhase = GetFreqIndexSample_Utils(
			  pIFParsPtr->stimulusAnalysis, frequency);
			if (!RunProcess_ModuleMgr(pIFParsPtr->audModel)) {
				NotifyError("%s: Failed to run auditory model.", funcName);
				FreeProcessVariables_PhaseIntFuncs();
				return(FALSE);
			}
			if (!RunProcess_ModuleMgr(pIFParsPtr->analysis)) {
				NotifyError("%s: Failed to run analysis process.", funcName);
				FreeProcessVariables_PhaseIntFuncs();
				return(FALSE);
			}
			outputPhase = GetFreqIndexSample_Utils(pIFParsPtr->analysis,
			  frequency);
			relativePhase = stimulusPhase - outputPhase;
			pIFParsPtr->resultEarObj->outSignal->channel[i][j] =
			  CalcPhase_Utils(relativePhase,
			  pIFParsPtr->analysis->outSignal->dt,
			  pIFParsPtr->analysisOutputMode, (j == 0));
		}
		SetInfoChannelLabel_SignalData(pIFParsPtr->resultEarObj->outSignal, i,
		  frequency);
	}
	OutputResults_Utils(pIFParsPtr->resultEarObj, &pIFParsPtr->outputModeList[
	  pIFParsPtr->outputMode], UTILS_NORMAL_SIGNAL);
	FreeProcessVariables_PhaseIntFuncs();
	return(TRUE);

}

