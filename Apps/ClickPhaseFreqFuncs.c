/**********************
 *
 * File:		ClickPhaseFreqFuncs.c
 * Purpose:		Calculates the click phase/frequency response using an FFT.
 * Comments:	Written using ModuleProducer version 1.2.10 (Jan  9 2001).
 * Author:		L. P. O'Mard
 * Created:		19 Jan 2001
 * Updated:	
 * Copyright:	(c) 2001, CNBH, University of Essex.
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
#include "ClickPhaseFreqFuncs.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

CPFFParsPtr	cPFFParsPtr = NULL;

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
Free_ClickPhaseFreqFuncs(void)
{
	/* static const char	*funcName = "Free_ClickPhaseFreqFuncs";  */

	if (cPFFParsPtr == NULL)
		return(FALSE);
	if (cPFFParsPtr->parSpec == GLOBAL) {
		free(cPFFParsPtr);
		cPFFParsPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitClickModeList *****************************/

/*
 * This function initialises the 'clickMode' list array
 */

BOOLN
InitClickModeList_ClickPhaseFreqFuncs(void)
{
	static const char	*funcName = "InitClickModeList_ClickPhaseFreqFuncs";
	static NameSpecifier	modeList[] = {

			{ "SIMPLE",		CLICKPHASEFREQFUNCS_CLICKMODE_SIMPLE },
			{ NO_FILE,		CLICKPHASEFREQFUNCS_CLICKMODE_FILE_MODE},
			{ "",			CLICKPHASEFREQFUNCS_CLICKMODE_NULL },
		};
	if ((cPFFParsPtr->clickModeList = InitNameList_NSpecLists(modeList,
	  cPFFParsPtr->stimFileName)) == NULL) {
		NotifyError("%s: Could not initialise list.", funcName);
		return(FALSE);
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
Init_ClickPhaseFreqFuncs(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_ClickPhaseFreqFuncs";

	if (parSpec == GLOBAL) {
		if (cPFFParsPtr != NULL)
			Free_ClickPhaseFreqFuncs();
		if ((cPFFParsPtr = (CPFFParsPtr) malloc(sizeof(CPFFPars))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (cPFFParsPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	cPFFParsPtr->parSpec = parSpec;
	cPFFParsPtr->enabledFlag = FALSE;
	cPFFParsPtr->outputModeFlag = FALSE;
	cPFFParsPtr->clickModeFlag = FALSE;
	cPFFParsPtr->analysisOutputModeFlag = FALSE;
	cPFFParsPtr->clickTimeFlag = FALSE;
	cPFFParsPtr->stimulusDurationFlag = FALSE;
	cPFFParsPtr->dtFlag = FALSE;
	cPFFParsPtr->dBAdjustmentFlag = FALSE;
	cPFFParsPtr->numIntensitiesFlag = FALSE;
	cPFFParsPtr->initialIntensityFlag = FALSE;
	cPFFParsPtr->deltaIntensityFlag = FALSE;
	cPFFParsPtr->enabled = 0;
	cPFFParsPtr->outputMode = 0;
	cPFFParsPtr->clickMode = 0;
	cPFFParsPtr->analysisOutputMode = 0;
	cPFFParsPtr->clickTime = 0.0;
	cPFFParsPtr->stimulusDuration = 0.0;
	cPFFParsPtr->dt = 0.0;
	cPFFParsPtr->dBAdjustment = 0.0;
	cPFFParsPtr->numIntensities = 0;
	cPFFParsPtr->initialIntensity = 0.0;
	cPFFParsPtr->deltaIntensity = 0.0;

	InitClickModeList_ClickPhaseFreqFuncs();
	sprintf(cPFFParsPtr->stimFileName, NO_FILE);
	sprintf(cPFFParsPtr->outputFile, NO_FILE);
	if ((cPFFParsPtr->outputModeList = InitNameList_NSpecLists(
	  DiagModeList_NSpecLists(0), cPFFParsPtr->outputFile)) == NULL)
		return(FALSE);
	cPFFParsPtr->analysisOutputModeList = InitPhaseOutputModeList_Utils();
	cPFFParsPtr->audModel = NULL;
	cPFFParsPtr->stimulus = NULL;
	cPFFParsPtr->setLevel = NULL;
	cPFFParsPtr->analysis = NULL;
	cPFFParsPtr->stimulusAnalysis = NULL;
	cPFFParsPtr->gate = NULL;
	cPFFParsPtr->resultEarObj = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_ClickPhaseFreqFuncs(UniParPtr pars)
{
	static const char *funcName = "SetUniParList_ClickPhaseFreqFuncs";

	if (cPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	SetPar_UniParMgr(&pars[CLICKPHASEFREQFUNCS_ENABLED], "CPFF_ENABLED",
	  "Phase intensity function test state 'on' or 'off'.",
	  UNIPAR_BOOL,
	  &cPFFParsPtr->enabled, NULL,
	  (void * (*)) SetEnabled_ClickPhaseFreqFuncs);
	SetPar_UniParMgr(&pars[CLICKPHASEFREQFUNCS_OUTPUTMODE], "CPFF_OUTPUT_MODE",
	  "Click magnitude/frequency function test output 'off', 'screen' or "
	  "<file name>.",
	  UNIPAR_NAME_SPEC_WITH_FILE,
	  &cPFFParsPtr->outputMode, cPFFParsPtr->outputModeList,
	  (void * (*)) SetOutputMode_ClickPhaseFreqFuncs);
	SetPar_UniParMgr(&pars[CLICKPHASEFREQFUNCS_CLICKMODE], "CPFF_CLICK_MODE",
	  "Click mode ('simple' or '<datafile par. file name>').",
	  UNIPAR_NAME_SPEC_WITH_FILE,
	  &cPFFParsPtr->clickMode, cPFFParsPtr->clickModeList,
	  (void * (*)) SetClickMode_ClickPhaseFreqFuncs);
	SetPar_UniParMgr(&pars[CLICKPHASEFREQFUNCS_ANALYSISOUTPUTMODE],
	  "CPFF_ANAL_OUTPUT",
	  "Analysis output mode ('absolute' or 'relative').",
	  UNIPAR_NAME_SPEC,
	  &cPFFParsPtr->analysisOutputMode, cPFFParsPtr->analysisOutputModeList,
	  (void * (*)) SetAnalysisOutputMode_ClickPhaseFreqFuncs);
	SetPar_UniParMgr(&pars[CLICKPHASEFREQFUNCS_CLICKTIME], "CPFF_CLICKTIME",
	  "Stimulus click time (s).",
	  UNIPAR_REAL,
	  &cPFFParsPtr->clickTime, NULL,
	  (void * (*)) SetClickTime_ClickPhaseFreqFuncs);
	SetPar_UniParMgr(&pars[CLICKPHASEFREQFUNCS_STIMULUSDURATION],
	  "CPFF_DURATION",
	  "Stimulus duration (s).",
	  UNIPAR_REAL,
	  &cPFFParsPtr->stimulusDuration, NULL,
	  (void * (*)) SetStimulusDuration_ClickPhaseFreqFuncs);
	SetPar_UniParMgr(&pars[CLICKPHASEFREQFUNCS_SAMPLINGINTERVAL], "CPFF_DT",
	  "Stimulus Sampling interval, dt (s).",
	  UNIPAR_REAL,
	  &cPFFParsPtr->dt, NULL,
	  (void * (*)) SetSamplingInterval_ClickPhaseFreqFuncs);
	SetPar_UniParMgr(&pars[CLICKPHASEFREQFUNCS_DBADJUSTMENT], "CPFF_DB_ADJUST",
	  "Adjustment of dB scale to experiment (dB).",
	  UNIPAR_REAL,
	  &cPFFParsPtr->dBAdjustment, NULL,
	  (void * (*)) SetDBAdjustment_ClickPhaseFreqFuncs);
	SetPar_UniParMgr(&pars[CLICKPHASEFREQFUNCS_NUMINTENSITIES],
	  "CPFF_NUM_INTENSTIES",
	  "No. of test intensities.",
	  UNIPAR_INT,
	  &cPFFParsPtr->numIntensities, NULL,
	  (void * (*)) SetNumIntensities_ClickPhaseFreqFuncs);
	SetPar_UniParMgr(&pars[CLICKPHASEFREQFUNCS_INITIALINTENSITY],
	  "CPFF_INIT_INTENSITY",
	  "Initial test intensity (dB SPL).",
	  UNIPAR_REAL,
	  &cPFFParsPtr->initialIntensity, NULL,
	  (void * (*)) SetInitialIntensity_ClickPhaseFreqFuncs);
	SetPar_UniParMgr(&pars[CLICKPHASEFREQFUNCS_DELTAINTENSITY],
	  "CPFF_DELTA_INTENSITY",
	  "Intensity increment (dB).",
	  UNIPAR_REAL,
	  &cPFFParsPtr->deltaIntensity, NULL,
	  (void * (*)) SetDeltaIntensity_ClickPhaseFreqFuncs);
	return(TRUE);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_ClickPhaseFreqFuncs(char * enabled, char * outputMode,
  char * clickMode, char * analysisOutputMode, double clickTime,
  double stimulusDuration, double samplingInterval, double dBAdjustment,
  int numIntensities, double initialIntensity, double deltaIntensity)
{
	static const char	*funcName = "SetPars_ClickPhaseFreqFuncs";
	BOOLN	ok;

	ok = TRUE;
	if (!SetEnabled_ClickPhaseFreqFuncs(enabled))
		ok = FALSE;
	if (!SetOutputMode_ClickPhaseFreqFuncs(outputMode))
		ok = FALSE;
	if (!SetClickMode_ClickPhaseFreqFuncs(clickMode))
		ok = FALSE;
	if (!SetAnalysisOutputMode_ClickPhaseFreqFuncs(analysisOutputMode))
		ok = FALSE;
	if (!SetClickTime_ClickPhaseFreqFuncs(clickTime))
		ok = FALSE;
	if (!SetStimulusDuration_ClickPhaseFreqFuncs(stimulusDuration))
		ok = FALSE;
	if (!SetSamplingInterval_ClickPhaseFreqFuncs(samplingInterval))
		ok = FALSE;
	if (!SetDBAdjustment_ClickPhaseFreqFuncs(dBAdjustment))
		ok = FALSE;
	if (!SetNumIntensities_ClickPhaseFreqFuncs(numIntensities))
		ok = FALSE;
	if (!SetInitialIntensity_ClickPhaseFreqFuncs(initialIntensity))
		ok = FALSE;
	if (!SetDeltaIntensity_ClickPhaseFreqFuncs(deltaIntensity))
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
SetEnabled_ClickPhaseFreqFuncs(char * theEnabled)
{
	static const char	*funcName = "SetEnabled_ClickPhaseFreqFuncs";
	int		specifier;

	if (cPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theEnabled, BooleanList_NSpecLists(
	  0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError("%s: Illegal name (%s).", funcName, theEnabled);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cPFFParsPtr->enabledFlag = TRUE;
	cPFFParsPtr->enabled = specifier;
	return(TRUE);

}

/****************************** SetOutputMode *********************************/

/*
 * This function sets the module's outputMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOutputMode_ClickPhaseFreqFuncs(char * theOutputMode)
{
	static const char	*funcName = "SetOutputMode_ClickPhaseFreqFuncs";

	if (cPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	cPFFParsPtr->outputMode = IdentifyDiag_NSpecLists(theOutputMode,
	  cPFFParsPtr->outputModeList);
	cPFFParsPtr->outputModeFlag = TRUE;
	return(TRUE);

}

/****************************** SetClickMode **********************************/

/*
 * This function sets the module's clickMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetClickMode_ClickPhaseFreqFuncs(char * theClickMode)
{
	static const char	*funcName = "SetClickMode_ClickPhaseFreqFuncs";
	int		specifier;

	if (cPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	specifier = Identify_NameSpecifier(theClickMode,
	  cPFFParsPtr->clickModeList);
	if ((specifier == CLICKPHASEFREQFUNCS_CLICKMODE_FILE_MODE) || (specifier == 
	  CLICKPHASEFREQFUNCS_CLICKMODE_NULL)) {
		sprintf(cPFFParsPtr->stimFileName, theClickMode);
		specifier = CLICKPHASEFREQFUNCS_CLICKMODE_FILE_MODE;
	}
	cPFFParsPtr->clickMode = specifier;
	cPFFParsPtr->clickModeFlag = TRUE;
	return(TRUE);

}

/****************************** SetClickTime **********************************/

/*
 * This function sets the module's stimulus click time parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetClickTime_ClickPhaseFreqFuncs(double theClickTime)
{
	static const char	*funcName = "SetClickTime_ClickPhaseFreqFuncs";

	if (cPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cPFFParsPtr->clickTimeFlag = TRUE;
	cPFFParsPtr->clickTime = theClickTime;
	return(TRUE);

}

/****************************** SetAnalysisOutputMode *************************/

/*
 * This function sets the module's analysisOutputMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetAnalysisOutputMode_ClickPhaseFreqFuncs(char * theAnalysisOutputMode)
{
	static const char	*funcName = "SetAnalysisOutputMode_ClickPhaseFreqFuncs";
	int		specifier;

	if (cPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theAnalysisOutputMode,
	  cPFFParsPtr->analysisOutputModeList)) ==
	  UTILS_PHASE_ANALYSISOUTPUTMODE_NULL) {
		NotifyError("%s: Illegal name (%s).", funcName, theAnalysisOutputMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cPFFParsPtr->analysisOutputModeFlag = TRUE;
	cPFFParsPtr->analysisOutputMode = specifier;
	return(TRUE);

}

/****************************** SetStimulusDuration ***************************/

/*
 * This function sets the module's stimulusDuration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetStimulusDuration_ClickPhaseFreqFuncs(double theStimulusDuration)
{
	static const char	*funcName = "SetStimulusDuration_ClickPhaseFreqFuncs";

	if (cPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cPFFParsPtr->stimulusDurationFlag = TRUE;
	cPFFParsPtr->stimulusDuration = theStimulusDuration;
	return(TRUE);

}

/****************************** SetSamplingInterval ***************************/

/*
 * This function sets the module's samplingInterval parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSamplingInterval_ClickPhaseFreqFuncs(double theSamplingInterval)
{
	static const char	*funcName = "SetSamplingInterval_ClickPhaseFreqFuncs";

	if (cPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cPFFParsPtr->dtFlag = TRUE;
	cPFFParsPtr->dt = theSamplingInterval;
	return(TRUE);

}

/****************************** SetDBAdjustment *******************************/

/*
 * This function sets the module's dBAdjustment parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDBAdjustment_ClickPhaseFreqFuncs(double theDBAdjustment)
{
	static const char	*funcName = "SetDBAdjustment_ClickPhaseFreqFuncs";

	if (cPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cPFFParsPtr->dBAdjustmentFlag = TRUE;
	cPFFParsPtr->dBAdjustment = theDBAdjustment;
	return(TRUE);

}

/****************************** SetNumIntensities *****************************/

/*
 * This function sets the module's numIntensities parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNumIntensities_ClickPhaseFreqFuncs(int theNumIntensities)
{
	static const char	*funcName = "SetNumIntensities_ClickPhaseFreqFuncs";

	if (cPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cPFFParsPtr->numIntensitiesFlag = TRUE;
	cPFFParsPtr->numIntensities = theNumIntensities;
	return(TRUE);

}

/****************************** SetInitialIntensity ***************************/

/*
 * This function sets the module's initialIntensity parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetInitialIntensity_ClickPhaseFreqFuncs(double theInitialIntensity)
{
	static const char	*funcName = "SetInitialIntensity_ClickPhaseFreqFuncs";

	if (cPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cPFFParsPtr->initialIntensityFlag = TRUE;
	cPFFParsPtr->initialIntensity = theInitialIntensity;
	return(TRUE);

}

/****************************** SetDeltaIntensity *****************************/

/*
 * This function sets the module's deltaIntensity parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDeltaIntensity_ClickPhaseFreqFuncs(double theDeltaIntensity)
{
	static const char	*funcName = "SetDeltaIntensity_ClickPhaseFreqFuncs";

	if (cPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cPFFParsPtr->deltaIntensityFlag = TRUE;
	cPFFParsPtr->deltaIntensity = theDeltaIntensity;
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
CheckPars_ClickPhaseFreqFuncs(void)
{
	static const char	*funcName = "CheckPars_ClickPhaseFreqFuncs";
	BOOLN	ok;

	ok = TRUE;
	if (cPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!cPFFParsPtr->enabledFlag) {
		NotifyError("%s: enabled variable not set.", funcName);
		ok = FALSE;
	}
	if (!cPFFParsPtr->outputModeFlag) {
		NotifyError("%s: outputMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!cPFFParsPtr->clickModeFlag) {
		NotifyError("%s: clickMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!cPFFParsPtr->analysisOutputModeFlag) {
		NotifyError("%s: analysisOutputMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!cPFFParsPtr->clickTimeFlag) {
		NotifyError("%s: click time variable not set.", funcName);
		ok = FALSE;
	}
	if (!cPFFParsPtr->stimulusDurationFlag) {
		NotifyError("%s: stimulusDuration variable not set.", funcName);
		ok = FALSE;
	}
	if (!cPFFParsPtr->dtFlag) {
		NotifyError("%s: dt variable not set.", funcName);
		ok = FALSE;
	}
	if (!cPFFParsPtr->dBAdjustmentFlag) {
		NotifyError("%s: dBAdjustment variable not set.", funcName);
		ok = FALSE;
	}
	if (!cPFFParsPtr->numIntensitiesFlag) {
		NotifyError("%s: numIntensities variable not set.", funcName);
		ok = FALSE;
	}
	if (!cPFFParsPtr->initialIntensityFlag) {
		NotifyError("%s: initialIntensity variable not set.", funcName);
		ok = FALSE;
	}
	if (!cPFFParsPtr->deltaIntensityFlag) {
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
PrintPars_ClickPhaseFreqFuncs(void)
{
	static const char	*funcName = "PrintPars_ClickPhaseFreqFuncs";

	if (!CheckPars_ClickPhaseFreqFuncs()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("?? ClickGainFreqFuncs Module Parameters:-\n");
	DPrint("\tenabled = %s \n", BooleanList_NSpecLists(
	  cPFFParsPtr->enabled)->name);
	DPrint("\toutputMode = %s \n", cPFFParsPtr->outputModeList[
	  cPFFParsPtr->outputMode].name);
	DPrint("\tclickMode = %s \n", cPFFParsPtr->clickModeList[
	  cPFFParsPtr->clickMode].name);
	DPrint("\tanalysisOutputMode = %s \n", cPFFParsPtr->analysisOutputModeList[
	  cPFFParsPtr->analysisOutputMode].name);
	DPrint("\tclickTime = %g ??\n", cPFFParsPtr->clickTime);
	DPrint("\tstimulusDuration = %g ??\n", cPFFParsPtr->stimulusDuration);
	DPrint("\tsamplingInterval = %g ??\n", cPFFParsPtr->dt);
	DPrint("\tdBAdjustment = %g ??\n", cPFFParsPtr->dBAdjustment);
	DPrint("\tnumIntensities = %d ??\n", cPFFParsPtr->numIntensities);
	DPrint("\tinitialIntensity = %g ??\n", cPFFParsPtr->initialIntensity);
	DPrint("\tdeltaIntensity = %g ??\n", cPFFParsPtr->deltaIntensity);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_ClickPhaseFreqFuncs(FILE *fp)
{
	static const char	*funcName = "ReadPars_ClickPhaseFreqFuncs";
	BOOLN	ok = TRUE;
	char	enabled[MAXLINE], outputMode[MAXLINE];
	char	clickMode[MAXLINE], analysisOutputMode[MAXLINE];
	int		numIntensities;
	double	clickTime, stimulusDuration, samplingInterval, deltaIntensity;
	double	initialIntensity, dBAdjustment;

	if (!fp) {
		NotifyError("%s: File not selected.", funcName);
		return(FALSE);
	}
	if (!GetPars_ParFile(fp, "%s", enabled))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%s", outputMode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%s", clickMode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%s", analysisOutputMode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &clickTime))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &stimulusDuration))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &samplingInterval))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &dBAdjustment))
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
	if (!SetPars_ClickPhaseFreqFuncs(enabled, outputMode, clickMode,
	  analysisOutputMode, clickTime, stimulusDuration, samplingInterval,
	  dBAdjustment, numIntensities, initialIntensity, deltaIntensity)) {
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
CheckData_ClickPhaseFreqFuncs(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_ClickPhaseFreqFuncs";

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
InitProcessVariables_ClickPhaseFreqFuncs(EarObjectPtr data)
{
	static const char * funcName = "InitProcessVariables_ClickPhaseFreqFuncs";
	DatumPtr	pc;
	CPFFParsPtr	p = cPFFParsPtr;

	p->audModel = data;
	if ((p->stimulus = Init_EarObject((p->clickMode ==
	  CLICKPHASEFREQFUNCS_CLICKMODE_SIMPLE)? STIM_CLICK_PROCESS_NAME:
	  DATAFILE_IN_PROCES_NAME)) == NULL) {
		NotifyError("%s: Could not initialise the stimulus EarObject.",
		  funcName);
		return(FALSE);
	}
	if ((p->setLevel = Init_EarObject(TRANS_SETDBSPL_PROCES_NAME)) == NULL) {
		NotifyError("%s: Could not initialise the Trans_SetDBSPL EarObject.",
		  funcName);
		return(FALSE);
	}
	if ((p->analysis = Init_EarObject(ANA_FOURIERT_PROCESS_NAME)) == NULL) {
		NotifyError("%s: Could not initialise the analysis EarObject.",
		  funcName);
		return(FALSE);
	}
	if ((p->stimulusAnalysis = Init_EarObject(ANA_FOURIERT_PROCESS_NAME)) ==
	  NULL) {
		NotifyError("%s: Could not initialise the stimulus analysis EarObject.",
		  funcName);
		return(FALSE);
	}

	ConnectOutSignalToIn_EarObject(p->stimulus, p->audModel);
	ConnectOutSignalToIn_EarObject(p->stimulus, p->stimulusAnalysis);
	ConnectOutSignalToIn_EarObject(p->stimulus, p->setLevel);
	ConnectOutSignalToIn_EarObject(p->audModel, p->analysis);

	switch (p->clickMode) {
	case CLICKPHASEFREQFUNCS_CLICKMODE_SIMPLE:
		if (!SetRealPar_ModuleMgr(p->stimulus, "amplitude",
		  DEFAULT_CLICK_MAGNITUDE)) {
			NotifyError("%s: Cannot set the stimulus amplitude.", funcName);
			return(FALSE);
		}
		if (!SetRealPar_ModuleMgr(p->stimulus, "time", p->clickTime)) {
			NotifyError("%s: Cannot set the stimulus amplitude.", funcName);
			return(FALSE);
		}
		if (!SetRealPar_ModuleMgr(p->stimulus, "dt", p->dt)) {
			NotifyError("%s: Cannot set the stimulus sampling interval.",
			  funcName);
			return(FALSE);
		}
		break;
	default:
		if (!ReadPars_ModuleMgr(p->stimulus, p->stimFileName)) {
			NotifyError("%s: Could not read datafile parameter file.",
			  funcName);
			return(FALSE);
		}
	}
	if (!SetRealPar_ModuleMgr(p->stimulus, "duration", p->stimulusDuration)) {
		NotifyError("%s: Cannot set the stimulus duration.", funcName);
		return(FALSE);
	}
	if (!SetPar_ModuleMgr(p->stimulusAnalysis, "output_mode", "phase")) {
		NotifyError("%s: Cannot set the stimulusanalysis output mode.",
		  funcName);
		return(FALSE);
	}
	if (!SetRealPar_ModuleMgr(p->setLevel, "Offset", 0.0)) {
		NotifyError("%s: Cannot set the set level time offset.", funcName);
		return(FALSE);
	}
	if (!SetPar_ModuleMgr(p->analysis, "output_mode", "phase")) {
		NotifyError("%s: Cannot set the analysis output mode.", funcName);
		return(FALSE);
	}
	if ((pc = FindModuleProcessInst_Utility_Datum(GetSimulation_ModuleMgr(data),
	  "Trans_gate")) != NULL) {
		p->gate = pc->data;
		Enable_ModuleMgr(p->gate, FALSE);
	}
	if ((p->resultEarObj = InitResultEarObject_Utils(p->numIntensities,
	  (ChanLen) (p->stimulusDuration / p->dt + 1.5), 0.0, p->dt, funcName)) ==
	  NULL) {
		NotifyError("%s: Could not initialise results EarObject", funcName);
		return(FALSE);
	}
	SetInfoSampleTitle_SignalData(p->resultEarObj->outSignal,
	  "Frequency (Hz)");
	return(TRUE);

}

/****************************** FreeProcessVariables **************************/

/*
 * This routine unconnects and frees the process variables.
 */

void
FreeProcessVariables_ClickPhaseFreqFuncs(void)
{
	if (cPFFParsPtr->stimulus && cPFFParsPtr->audModel)
		DisconnectOutSignalFromIn_EarObject(cPFFParsPtr->stimulus,
		  cPFFParsPtr->audModel);
	if (cPFFParsPtr->stimulus && cPFFParsPtr->stimulusAnalysis)
		DisconnectOutSignalFromIn_EarObject(cPFFParsPtr->stimulus,
		  cPFFParsPtr->stimulusAnalysis);
	if (cPFFParsPtr->audModel && cPFFParsPtr->analysis)
		DisconnectOutSignalFromIn_EarObject(cPFFParsPtr->audModel,
		  cPFFParsPtr->analysis);
	Free_EarObject(&cPFFParsPtr->stimulus);
	Free_EarObject(&cPFFParsPtr->setLevel);
	Free_EarObject(&cPFFParsPtr->analysis);
	Free_EarObject(&cPFFParsPtr->stimulusAnalysis);
	Free_EarObject(&cPFFParsPtr->resultEarObj);
	if (cPFFParsPtr->gate) {
		Enable_ModuleMgr(cPFFParsPtr->gate, TRUE);
		cPFFParsPtr->gate = NULL;
	}
	
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
RunProcess_ClickPhaseFreqFuncs(EarObjectPtr data)
{
	static const char	*funcName = "RunProcess_ClickPhaseFreqFuncs";
	register ChanData	*stimFTPtr, *modFTPtr, *outPtr;
	int		i;
	ChanLen	j;
	double	intensity, relativePhase;
	CPFFParsPtr	p = cPFFParsPtr;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckPars_ClickPhaseFreqFuncs())
		return(FALSE);
	if (!CheckData_ClickPhaseFreqFuncs(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	if (!InitProcessVariables_ClickPhaseFreqFuncs(data)) {
		NotifyError("%s: Could not initialise the process variables.",
		  funcName);
		FreeProcessVariables_ClickPhaseFreqFuncs();
		return(FALSE);
	}
	for (i = 0, intensity = p->initialIntensity; i < p->numIntensities; i++,
	  intensity += p->deltaIntensity) {
		if (!SetRealPar_ModuleMgr(p->setLevel, "dBSPL", intensity +
		  p->dBAdjustment)) {
			NotifyError("%s: Cannot set the level intensity.", funcName);
			FreeProcessVariables_ClickPhaseFreqFuncs();
			return(FALSE);
		}
		if (!RunProcess_ModuleMgr(p->stimulus)) {
			NotifyError("%s: Failed to run stimulus process.", funcName);
			FreeProcessVariables_ClickPhaseFreqFuncs();
			return(FALSE);
		}
		if (!RunProcess_ModuleMgr(p->setLevel)) {
			NotifyError("%s: Failed to run the set-level process.", funcName);
			FreeProcessVariables_ClickPhaseFreqFuncs();
			return(FALSE);
		}
		if (!RunProcess_ModuleMgr(p->stimulusAnalysis)) {
			NotifyError("%s: Failed to run stimulus analysis process.",
			  funcName);
			FreeProcessVariables_ClickPhaseFreqFuncs();
			return(FALSE);
		}
		if (!RunProcess_ModuleMgr(p->audModel)) {
			NotifyError("%s: Failed to run auditory model.", funcName);
			FreeProcessVariables_ClickPhaseFreqFuncs();
			return(FALSE);
		}
		if (!RunProcess_ModuleMgr(p->analysis)) {
			NotifyError("%s: Failed to run analysis process.", funcName);
			FreeProcessVariables_ClickPhaseFreqFuncs();
			return(FALSE);
		}
		stimFTPtr = p->stimulusAnalysis->outSignal->channel[0];
		modFTPtr = p->analysis->outSignal->channel[0];
		outPtr = p->resultEarObj->outSignal->channel[i];
		for (j = 0; j < p->resultEarObj->outSignal->length; j++) {
			relativePhase = *stimFTPtr++ - *modFTPtr++;
			*outPtr++ = CalcPhase_Utils(relativePhase,
			  p->analysis->outSignal->dt, p->analysisOutputMode, (j == 0));
		}
		SetInfoChannelLabel_SignalData(p->resultEarObj->outSignal, i,
		  intensity);
	}
	SetSamplingInterval_SignalData(p->resultEarObj->outSignal,
	  p->analysis->outSignal->dt);
	OutputResults_Utils(p->resultEarObj, &p->outputModeList[p->outputMode],
	  UTILS_NORMAL_SIGNAL);
	FreeProcessVariables_ClickPhaseFreqFuncs();
	return(TRUE);

}
