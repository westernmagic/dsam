/**********************
 *
 * File:		RelPhaseFreqFuncs.c
 * Purpose:		
 * Comments:	Written using ModuleProducer version 1.2.10 (Jan  9 2001).
 * Author:		
 * Created:		09 Jan 2001
 * Updated:	
 * Copyright:	(c) 2001, 
 *
 *********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "DSAM.h"
#include "Utils.h"
#include "RelPhaseFreqFuncs.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

RPFFParsPtr	rPFFParsPtr = NULL;

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
Free_RelPhaseFreqFuncs(void)
{
	/* static const char	*funcName = "Free_RelPhaseFreqFuncs";  */

	if (rPFFParsPtr == NULL)
		return(FALSE);
	if (rPFFParsPtr->parSpec == GLOBAL) {
		free(rPFFParsPtr);
		rPFFParsPtr = NULL;
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
Init_RelPhaseFreqFuncs(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_RelPhaseFreqFuncs";

	if (parSpec == GLOBAL) {
		if (rPFFParsPtr != NULL)
			Free_RelPhaseFreqFuncs();
		if ((rPFFParsPtr = (RPFFParsPtr) malloc(sizeof(RPFFPars))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (rPFFParsPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	rPFFParsPtr->parSpec = parSpec;
	rPFFParsPtr->enabledFlag = FALSE;
	rPFFParsPtr->outputModeFlag = FALSE;
	rPFFParsPtr->analysisOutputModeFlag = FALSE;
	rPFFParsPtr->numFrequenciesFlag = FALSE;
	rPFFParsPtr->stimulusDurationFlag = FALSE;
	rPFFParsPtr->dtFlag = FALSE;
	rPFFParsPtr->initialFrequencyFlag = FALSE;
	rPFFParsPtr->deltaFrequencyFlag = FALSE;
	rPFFParsPtr->numIntensitiesFlag = FALSE;
	rPFFParsPtr->initialIntensityFlag = FALSE;
	rPFFParsPtr->deltaIntensityFlag = FALSE;
	rPFFParsPtr->refIntensityFlag = FALSE;
	rPFFParsPtr->enabled = 0;
	rPFFParsPtr->outputMode = 0;
	rPFFParsPtr->analysisOutputMode = 0;
	rPFFParsPtr->numFrequencies = 0;
	rPFFParsPtr->stimulusDuration = 0.0;
	rPFFParsPtr->dt = 0.0;
	rPFFParsPtr->initialFrequency = 0.0;
	rPFFParsPtr->deltaFrequency = 0.0;
	rPFFParsPtr->numIntensities = 0;
	rPFFParsPtr->initialIntensity = 0.0;
	rPFFParsPtr->deltaIntensity = 0.0;
	rPFFParsPtr->refIntensity = 0.0;

	sprintf(rPFFParsPtr->outputFile, NO_FILE);
	if ((rPFFParsPtr->outputModeList = InitNameList_NSpecLists(
	  DiagModeList_NSpecLists(0), rPFFParsPtr->outputFile)) == NULL)
		return(FALSE);
	rPFFParsPtr->analysisOutputModeList = InitPhaseOutputModeList_Utils();
	rPFFParsPtr->audModel = NULL;
	rPFFParsPtr->stimulus = NULL;
	rPFFParsPtr->analysis = NULL;
	rPFFParsPtr->stimulusAnalysis = NULL;
	rPFFParsPtr->resultEarObj = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_RelPhaseFreqFuncs(UniParPtr pars)
{
	static const char *funcName = "SetUniParList_RelPhaseFreqFuncs";

	if (rPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	SetPar_UniParMgr(&pars[RELPHASEFREQFUNCS_ENABLED], "RPFF_ENABLED",
	  "Phase intensity function test state 'on' or 'off'.",
	  UNIPAR_BOOL,
	  &rPFFParsPtr->enabled, NULL,
	  (void * (*)) SetEnabled_RelPhaseFreqFuncs);
	SetPar_UniParMgr(&pars[RELPHASEFREQFUNCS_OUTPUTMODE], "RPFF_OUTPUT_MODE",
	  "I/O function test output 'off', 'screen' or <file name>.",
	  UNIPAR_NAME_SPEC_WITH_FILE,
	  &rPFFParsPtr->outputMode, rPFFParsPtr->outputModeList,
	  (void * (*)) SetOutputMode_RelPhaseFreqFuncs);
	SetPar_UniParMgr(&pars[RELPHASEFREQFUNCS_ANALYSISOUTPUTMODE],
	  "RPFF_ANAL_OUTPUT",
	  "Analysis output mode ('phase' or 'degrees').",
	  UNIPAR_NAME_SPEC,
	  &rPFFParsPtr->analysisOutputMode, rPFFParsPtr->analysisOutputModeList,
	  (void * (*)) SetAnalysisOutputMode_RelPhaseFreqFuncs);
	SetPar_UniParMgr(&pars[RELPHASEFREQFUNCS_STIMULUSDURATION], "RPFF_DURATION",
	  "Stimulus duration (s).",
	  UNIPAR_REAL,
	  &rPFFParsPtr->stimulusDuration, NULL,
	  (void * (*)) SetStimulusDuration_RelPhaseFreqFuncs);
	SetPar_UniParMgr(&pars[RELPHASEFREQFUNCS_SAMPLINGINTERVAL], "RPFF_DT",
	  "Stimulus Sampling interval, dt (s).",
	  UNIPAR_REAL,
	  &rPFFParsPtr->dt, NULL,
	  (void * (*)) SetSamplingInterval_RelPhaseFreqFuncs);
	SetPar_UniParMgr(&pars[RELPHASEFREQFUNCS_NUMFREQUENCIES], "RPFF_NUM_FREQ",
	  "No. of test frequencies.",
	  UNIPAR_INT,
	  &rPFFParsPtr->numFrequencies, NULL,
	  (void * (*)) SetNumFrequencies_RelPhaseFreqFuncs);
	SetPar_UniParMgr(&pars[RELPHASEFREQFUNCS_INITIALFREQUENCY],
	  "RPFF_INIT_FREQ",
	  "Initial test frequency (Hz).",
	  UNIPAR_REAL,
	  &rPFFParsPtr->initialFrequency, NULL,
	  (void * (*)) SetInitialFrequency_RelPhaseFreqFuncs);
	SetPar_UniParMgr(&pars[RELPHASEFREQFUNCS_DELTAFREQUENCY], "RPFF_DELTA_FREQ",
	  "Frequency increment (Hz).",
	  UNIPAR_REAL,
	  &rPFFParsPtr->deltaFrequency, NULL,
	  (void * (*)) SetDeltaFrequency_RelPhaseFreqFuncs);
	SetPar_UniParMgr(&pars[RELPHASEFREQFUNCS_NUMINTENSITIES],
	  "RPFF_NUM_INTENSTIES",
	  "No. of test intensities.",
	  UNIPAR_INT,
	  &rPFFParsPtr->numIntensities, NULL,
	  (void * (*)) SetNumIntensities_RelPhaseFreqFuncs);
	SetPar_UniParMgr(&pars[RELPHASEFREQFUNCS_INITIALINTENSITY],
	  "RPFF_INIT_INTENSITY",
	  "Initial test intensity (dB SPL).",
	  UNIPAR_REAL,
	  &rPFFParsPtr->initialIntensity, NULL,
	  (void * (*)) SetInitialIntensity_RelPhaseFreqFuncs);
	SetPar_UniParMgr(&pars[RELPHASEFREQFUNCS_DELTAINTENSITY],
	  "RPFF_DELTA_INTENSITY",
	  "Intensity increment (dB).",
	  UNIPAR_REAL,
	  &rPFFParsPtr->deltaIntensity, NULL,
	  (void * (*)) SetDeltaIntensity_RelPhaseFreqFuncs);
	SetPar_UniParMgr(&pars[RELPHASEFREQFUNCS_REFINTENSITY],
	  "RPFF_REL_INTENSITY",
	  "Relative intensity reference (dB).",
	  UNIPAR_REAL,
	  &rPFFParsPtr->refIntensity, NULL,
	  (void * (*)) SetRefIntensity_RelPhaseFreqFuncs);
	return(TRUE);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_RelPhaseFreqFuncs(char * enabled, char * outputMode,
  char * analysisOutputMode, int numFrequencies, double stimulusDuration,
  double samplingInterval, double initialFrequency, double deltaFrequency,
  int numIntensities, double initialIntensity, double deltaIntensity,
  double refIntensity)
{
	static const char	*funcName = "SetPars_RelPhaseFreqFuncs";
	BOOLN	ok;

	ok = TRUE;
	if (!SetEnabled_RelPhaseFreqFuncs(enabled))
		ok = FALSE;
	if (!SetOutputMode_RelPhaseFreqFuncs(outputMode))
		ok = FALSE;
	if (!SetAnalysisOutputMode_RelPhaseFreqFuncs(analysisOutputMode))
		ok = FALSE;
	if (!SetStimulusDuration_RelPhaseFreqFuncs(stimulusDuration))
		ok = FALSE;
	if (!SetSamplingInterval_RelPhaseFreqFuncs(samplingInterval))
		ok = FALSE;
	if (!SetNumFrequencies_RelPhaseFreqFuncs(numFrequencies))
		ok = FALSE;
	if (!SetInitialFrequency_RelPhaseFreqFuncs(initialFrequency))
		ok = FALSE;
	if (!SetDeltaFrequency_RelPhaseFreqFuncs(deltaFrequency))
		ok = FALSE;
	if (!SetNumIntensities_RelPhaseFreqFuncs(numIntensities))
		ok = FALSE;
	if (!SetInitialIntensity_RelPhaseFreqFuncs(initialIntensity))
		ok = FALSE;
	if (!SetDeltaIntensity_RelPhaseFreqFuncs(deltaIntensity))
		ok = FALSE;
	if (!SetRefIntensity_RelPhaseFreqFuncs(refIntensity))
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
SetEnabled_RelPhaseFreqFuncs(char * theEnabled)
{
	static const char	*funcName = "SetEnabled_RelPhaseFreqFuncs";
	int		specifier;

	if (rPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theEnabled, BooleanList_NSpecLists(
	  0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError("%s: Illegal name (%s).", funcName, theEnabled);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	rPFFParsPtr->enabledFlag = TRUE;
	rPFFParsPtr->enabled = specifier;
	return(TRUE);

}

/****************************** SetOutputMode *********************************/

/*
 * This function sets the module's outputMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOutputMode_RelPhaseFreqFuncs(char * theOutputMode)
{
	static const char	*funcName = "SetOutputMode_RelPhaseFreqFuncs";

	if (rPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	rPFFParsPtr->outputMode = IdentifyDiag_NSpecLists(theOutputMode,
	  rPFFParsPtr->outputModeList);
	rPFFParsPtr->outputModeFlag = TRUE;
	return(TRUE);

}

/****************************** SetAnalysisOutputMode *************************/

/*
 * This function sets the module's analysisOutputMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetAnalysisOutputMode_RelPhaseFreqFuncs(char * theAnalysisOutputMode)
{
	static const char	*funcName = "SetAnalysisOutputMode_RelPhaseFreqFuncs";
	int		specifier;

	if (rPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theAnalysisOutputMode,
	  rPFFParsPtr->analysisOutputModeList)) ==
	  UTILS_PHASE_ANALYSISOUTPUTMODE_NULL) {
		NotifyError("%s: Illegal name (%s).", funcName, theAnalysisOutputMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	rPFFParsPtr->analysisOutputModeFlag = TRUE;
	rPFFParsPtr->analysisOutputMode = specifier;
	return(TRUE);

}

/****************************** SetNumFrequencies *****************************/

/*
 * This function sets the module's numFrequencies parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNumFrequencies_RelPhaseFreqFuncs(int theNumFrequencies)
{
	static const char	*funcName = "SetNumFrequencies_RelPhaseFreqFuncs";

	if (rPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	rPFFParsPtr->numFrequenciesFlag = TRUE;
	rPFFParsPtr->numFrequencies = theNumFrequencies;
	return(TRUE);

}

/****************************** SetStimulusDuration ***************************/

/*
 * This function sets the module's stimulusDuration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetStimulusDuration_RelPhaseFreqFuncs(double theStimulusDuration)
{
	static const char	*funcName = "SetStimulusDuration_RelPhaseFreqFuncs";

	if (rPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	rPFFParsPtr->stimulusDurationFlag = TRUE;
	rPFFParsPtr->stimulusDuration = theStimulusDuration;
	return(TRUE);

}

/****************************** SetSamplingInterval ***************************/

/*
 * This function sets the module's samplingInterval parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSamplingInterval_RelPhaseFreqFuncs(double theSamplingInterval)
{
	static const char	*funcName = "SetSamplingInterval_RelPhaseFreqFuncs";

	if (rPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	rPFFParsPtr->dtFlag = TRUE;
	rPFFParsPtr->dt = theSamplingInterval;
	return(TRUE);

}

/****************************** SetInitialFrequency ***************************/

/*
 * This function sets the module's initialFrequency parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetInitialFrequency_RelPhaseFreqFuncs(double theInitialFrequency)
{
	static const char	*funcName = "SetInitialFrequency_RelPhaseFreqFuncs";

	if (rPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	rPFFParsPtr->initialFrequencyFlag = TRUE;
	rPFFParsPtr->initialFrequency = theInitialFrequency;
	return(TRUE);

}

/****************************** SetDeltaFrequency *****************************/

/*
 * This function sets the module's deltaFrequency parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDeltaFrequency_RelPhaseFreqFuncs(double theDeltaFrequency)
{
	static const char	*funcName = "SetDeltaFrequency_RelPhaseFreqFuncs";

	if (rPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	rPFFParsPtr->deltaFrequencyFlag = TRUE;
	rPFFParsPtr->deltaFrequency = theDeltaFrequency;
	return(TRUE);

}

/****************************** SetNumIntensities *****************************/

/*
 * This function sets the module's numIntensities parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNumIntensities_RelPhaseFreqFuncs(int theNumIntensities)
{
	static const char	*funcName = "SetNumIntensities_RelPhaseFreqFuncs";

	if (rPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	rPFFParsPtr->numIntensitiesFlag = TRUE;
	rPFFParsPtr->numIntensities = theNumIntensities;
	return(TRUE);

}

/****************************** SetInitialIntensity ***************************/

/*
 * This function sets the module's initialIntensity parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetInitialIntensity_RelPhaseFreqFuncs(double theInitialIntensity)
{
	static const char	*funcName = "SetInitialIntensity_RelPhaseFreqFuncs";

	if (rPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	rPFFParsPtr->initialIntensityFlag = TRUE;
	rPFFParsPtr->initialIntensity = theInitialIntensity;
	return(TRUE);

}

/****************************** SetDeltaIntensity *****************************/

/*
 * This function sets the module's deltaIntensity parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDeltaIntensity_RelPhaseFreqFuncs(double theDeltaIntensity)
{
	static const char	*funcName = "SetDeltaIntensity_RelPhaseFreqFuncs";

	if (rPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	rPFFParsPtr->deltaIntensityFlag = TRUE;
	rPFFParsPtr->deltaIntensity = theDeltaIntensity;
	return(TRUE);

}

/****************************** SetRefIntensity *******************************/

/*
 * This function sets the module's refIntensity parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRefIntensity_RelPhaseFreqFuncs(double theRefIntensity)
{
	static const char	*funcName = "SetRefIntensity_RelPhaseFreqFuncs";

	if (rPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	rPFFParsPtr->refIntensityFlag = TRUE;
	rPFFParsPtr->refIntensity = theRefIntensity;
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
CheckPars_RelPhaseFreqFuncs(void)
{
	static const char	*funcName = "CheckPars_RelPhaseFreqFuncs";
	BOOLN	ok;

	ok = TRUE;
	if (rPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!rPFFParsPtr->enabledFlag) {
		NotifyError("%s: enabled variable not set.", funcName);
		ok = FALSE;
	}
	if (!rPFFParsPtr->outputModeFlag) {
		NotifyError("%s: outputMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!rPFFParsPtr->analysisOutputModeFlag) {
		NotifyError("%s: analysisOutputMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!rPFFParsPtr->numFrequenciesFlag) {
		NotifyError("%s: numFrequencies variable not set.", funcName);
		ok = FALSE;
	}
	if (!rPFFParsPtr->stimulusDurationFlag) {
		NotifyError("%s: stimulusDuration variable not set.", funcName);
		ok = FALSE;
	}
	if (!rPFFParsPtr->dtFlag) {
		NotifyError("%s: dt variable not set.", funcName);
		ok = FALSE;
	}
	if (!rPFFParsPtr->initialFrequencyFlag) {
		NotifyError("%s: initialFrequency variable not set.", funcName);
		ok = FALSE;
	}
	if (!rPFFParsPtr->deltaFrequencyFlag) {
		NotifyError("%s: deltaFrequency variable not set.", funcName);
		ok = FALSE;
	}
	if (!rPFFParsPtr->numIntensitiesFlag) {
		NotifyError("%s: numIntensities variable not set.", funcName);
		ok = FALSE;
	}
	if (!rPFFParsPtr->initialIntensityFlag) {
		NotifyError("%s: initialIntensity variable not set.", funcName);
		ok = FALSE;
	}
	if (!rPFFParsPtr->deltaIntensityFlag) {
		NotifyError("%s: deltaIntensity variable not set.", funcName);
		ok = FALSE;
	}
	if (!rPFFParsPtr->refIntensityFlag) {
		NotifyError("%s: refIntensity variable not set.", funcName);
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
PrintPars_RelPhaseFreqFuncs(void)
{
	static const char	*funcName = "PrintPars_RelPhaseFreqFuncs";

	if (!CheckPars_RelPhaseFreqFuncs()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("?? RelPhaseFreqFuncs Module Parameters:-\n");
	DPrint("\tenabled = %s \n", BooleanList_NSpecLists(
	  rPFFParsPtr->enabled)->name);
	DPrint("\toutputMode = %s \n", rPFFParsPtr->outputModeList[
	  rPFFParsPtr->outputMode].name);
	DPrint("\tanalysisOutputMode = %s \n",
	  rPFFParsPtr->analysisOutputModeList[rPFFParsPtr->analysisOutputMode]
	  .name);
	DPrint("\tstimulusDuration = %g ??\n", rPFFParsPtr->stimulusDuration);
	DPrint("\tsamplingInterval = %g ??\n", rPFFParsPtr->dt);
	DPrint("\tnumFrequencies = %d ??\n", rPFFParsPtr->numFrequencies);
	DPrint("\tinitialFrequency = %g ??\n", rPFFParsPtr->initialFrequency);
	DPrint("\tdeltaFrequency = %g ??\n", rPFFParsPtr->deltaFrequency);
	DPrint("\tnumIntensities = %d ??\n", rPFFParsPtr->numIntensities);
	DPrint("\tinitialIntensity = %g ??\n", rPFFParsPtr->initialIntensity);
	DPrint("\tdeltaIntensity = %g ??\n", rPFFParsPtr->deltaIntensity);
	DPrint("\trefIntensity = %g ??\n", rPFFParsPtr->refIntensity);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_RelPhaseFreqFuncs(FILE *fp)
{
	static const char	*funcName = "ReadPars_RelPhaseFreqFuncs";
	BOOLN	ok = TRUE;
	int		numFrequencies, numIntensities;
	char	enabled[MAXLINE], outputMode[MAXLINE];
	char	analysisOutputMode[MAXLINE];
	double	stimulusDuration, samplingInterval, initialFrequency;
	double	deltaFrequency, initialIntensity, deltaIntensity, refIntensity;

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
	if (!GetPars_ParFile(fp, "%lf", &refIntensity))
		ok = FALSE;
	if (!ok) {
		NotifyError("%s: Failed to read parameters.", funcName);
		return(FALSE);
	}
	if (!SetPars_RelPhaseFreqFuncs(enabled, outputMode, analysisOutputMode,
	  numFrequencies, stimulusDuration, samplingInterval, initialFrequency,
	  deltaFrequency, numIntensities, initialIntensity, deltaIntensity,
	  refIntensity)) {
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
CheckData_RelPhaseFreqFuncs(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_RelPhaseFreqFuncs";

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
 * It returns FALSE if it fails in any way.
 */

BOOLN
InitProcessVariables_RelPhaseFreqFuncs(EarObjectPtr data)
{
	static const char * funcName = "InitProcessVariables_RelPhaseFreqFuncs";
	double	realIndex;
	RPFFParsPtr	p = rPFFParsPtr;

	p->audModel = data;
	if ((p->stimulus = Init_EarObject(STIM_PURETONE_PROCESS_NAME)) == NULL) {
		NotifyError("%s: Could not initialise the stimulus EarObject.",
		  funcName);
		return(FALSE);
	}
	if ((p->analysis = Init_EarObject(ANA_FOURIERT_PROCESS_NAME)) == NULL) {
		NotifyError("%s: Could not initialise the analysis EarObject.",
		  funcName);
		return(FALSE);
	}
	if ((p->stimulusAnalysis = Init_EarObject(
	  ANA_FOURIERT_PROCESS_NAME)) == NULL) {
		NotifyError("%s: Could not initialise the stimulus analysis EarObject.",
		  funcName);
		return(FALSE);
	}

	ConnectOutSignalToIn_EarObject(p->stimulus, p->audModel);
	ConnectOutSignalToIn_EarObject(p->stimulus, p->stimulusAnalysis);
	ConnectOutSignalToIn_EarObject(p->audModel, p->analysis);

	if (!SetRealPar_ModuleMgr(p->stimulus, "duration", p->stimulusDuration)) {
		NotifyError("%s: Cannot set the stimulus duration.", funcName);
		return(FALSE);
	}
	if (!SetRealPar_ModuleMgr(p->stimulus, "dt", p->dt)) {
		NotifyError("%s: Cannot set the stimulus sampling interval.", funcName);
		return(FALSE);
	}
	if (!SetPar_ModuleMgr(p->stimulusAnalysis, "output_mode", "phase")) {
		NotifyError("%s: Cannot set the stimulusanalysis output mode.",
		  funcName);
		return(FALSE);
	}
	if (!SetPar_ModuleMgr(p->analysis, "output_mode", "phase")) {
		NotifyError("%s: Cannot set the analysis output mode.", funcName);
		return(FALSE);
	}

	if ((p->resultEarObj = InitResultEarObject_Utils(p->numIntensities,
	  p->numFrequencies, p->initialFrequency, p->deltaFrequency,
	  funcName)) == NULL) {
		NotifyError("%s: Could not initialise results EarObject", funcName);
		return(FALSE);
	}
	SetInfoSampleTitle_SignalData(p->resultEarObj->outSignal, "Frequency (Hz)");
	realIndex = (p->refIntensity - p->initialIntensity) / p->deltaIntensity;
	p->refIntIndex = (int) floor(realIndex + 0.5);
	if ((fabs(p->refIntIndex - realIndex) > DBL_EPSILON) || (p->refIntIndex <
	  0) || (p->refIntIndex >= p->numIntensities)) {
		NotifyError("%s: Reference intensity (%g dB) is not in\nintensity "
		  "range.", funcName, p->refIntensity);
		return(FALSE);
	}

	return(TRUE);

}

/****************************** FreeProcessVariables **************************/

/*
 * This routine unconnects and frees the process variables.
 */

void
FreeProcessVariables_RelPhaseFreqFuncs(void)
{
	if (rPFFParsPtr->stimulus && rPFFParsPtr->audModel)
		DisconnectOutSignalFromIn_EarObject(rPFFParsPtr->stimulus,
		  rPFFParsPtr->audModel);
	if (rPFFParsPtr->stimulus && rPFFParsPtr->stimulusAnalysis)
		DisconnectOutSignalFromIn_EarObject(rPFFParsPtr->stimulus,
		  rPFFParsPtr->stimulusAnalysis);
	if (rPFFParsPtr->audModel && rPFFParsPtr->analysis)
		DisconnectOutSignalFromIn_EarObject(rPFFParsPtr->audModel,
		  rPFFParsPtr->analysis);
	Free_EarObject(&rPFFParsPtr->stimulus);
	Free_EarObject(&rPFFParsPtr->analysis);
	Free_EarObject(&rPFFParsPtr->stimulusAnalysis);
	Free_EarObject(&rPFFParsPtr->resultEarObj);
	
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
RunProcess_RelPhaseFreqFuncs(EarObjectPtr data)
{
	static const char	*funcName = "RunProcess_RelPhaseFreqFuncs";
	int		i, j;
	double	intensity, stimulusPhase, outputPhase, relativePhase, frequency;
	RPFFParsPtr	p = rPFFParsPtr;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckPars_RelPhaseFreqFuncs())
		return(FALSE);
	SetProcessName_EarObject(data, "Module process ??");
	if (!InitProcessVariables_RelPhaseFreqFuncs(data)) {
		NotifyError("%s: Could not initialise the process variables.",
		  funcName);
		FreeProcessVariables_RelPhaseFreqFuncs();
		return(FALSE);
	}
	for (i = 0, intensity = p->initialIntensity; i < p->numIntensities; i++,
	  intensity += p->deltaIntensity) {
		if (!SetRealPar_ModuleMgr(p->stimulus, "intensity", intensity)) {
			NotifyError("%s: Cannot set the stimulus intensity.", funcName);
			FreeProcessVariables_RelPhaseFreqFuncs();
			return(FALSE);
		}
		for (j = 0, frequency = p->initialFrequency; j < p->numFrequencies; j++,
		  frequency += p->deltaFrequency) {
			if (!SetRealPar_ModuleMgr(p->stimulus, "frequency", frequency)) {
				NotifyError("%s: Cannot set the stimulus intensity.", funcName);
				FreeProcessVariables_RelPhaseFreqFuncs();
				return(FALSE);
			}
			if (!RunProcess_ModuleMgr(p->stimulus)) {
				NotifyError("%s: Failed to run stimulus process.", funcName);
				FreeProcessVariables_RelPhaseFreqFuncs();
				return(FALSE);
			}
			if (!RunProcess_ModuleMgr(p->stimulusAnalysis)) {
				NotifyError("%s: Failed to run stimulus analysis process.",
				  funcName);
				FreeProcessVariables_RelPhaseFreqFuncs();
				return(FALSE);
			}
			stimulusPhase = GetFreqIndexSample_Utils(p->stimulusAnalysis,
			  frequency);
			if (!RunProcess_ModuleMgr(p->audModel)) {
				NotifyError("%s: Failed to run auditory model.", funcName);
				FreeProcessVariables_RelPhaseFreqFuncs();
				return(FALSE);
			}
			if (!RunProcess_ModuleMgr(p->analysis)) {
				NotifyError("%s: Failed to run analysis process.", funcName);
				FreeProcessVariables_RelPhaseFreqFuncs();
				return(FALSE);
			}
			outputPhase = GetFreqIndexSample_Utils(p->analysis, frequency);
			relativePhase = stimulusPhase - outputPhase;
			p->resultEarObj->outSignal->channel[i][j] = CalcPhase_Utils(
			  relativePhase, p->analysis->outSignal->dt, p->analysisOutputMode,
			  (j == 0));
		}
		SetInfoChannelLabel_SignalData(p->resultEarObj->outSignal, i,
		  intensity);
	}
	for (i = 0; i < p->numIntensities; i++)
		for (j = 0; (i != p->refIntIndex) && (j <
		  p->resultEarObj->outSignal->length); j++)
			p->resultEarObj->outSignal->channel[i][j] =
			  p->resultEarObj->outSignal->channel[p->refIntIndex][j] -
			  p->resultEarObj->outSignal->channel[i][j];
	for (j = 0; j < p->resultEarObj->outSignal->length; j++)
		p->resultEarObj->outSignal->channel[p->refIntIndex][j] = 0.0;
	OutputResults_Utils(p->resultEarObj, &p->outputModeList[
	  p->outputMode], UTILS_NORMAL_SIGNAL);
	FreeProcessVariables_RelPhaseFreqFuncs();
	return(TRUE);

}

