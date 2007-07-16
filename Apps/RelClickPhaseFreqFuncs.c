/**********************
 *
 * File:		RelClickPhaseFreqFuncs.c
 * Purpose:		Calculates the click relative phase/frequency response using an
 *				FFT.
 * Comments:	Written using ModuleProducer version 1.2.11 (Jan 30 2001).
 * Author:		L. P. O'Mard
 * Created:		30 Jan 2001
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
#include "RelClickPhaseFreqFuncs.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

RCPFFParsPtr	rCPFFParsPtr = NULL;

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
Free_RelClickPhaseFreqFuncs(void)
{
	/* static const char	*funcName = "Free_RelClickPhaseFreqFuncs";  */

	if (rCPFFParsPtr == NULL)
		return(FALSE);
	if (rCPFFParsPtr->parSpec == GLOBAL) {
		free(rCPFFParsPtr);
		rCPFFParsPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitClickModeList *****************************/

/*
 * This function initialises the 'clickMode' list array
 */

BOOLN
InitClickModeList_RelClickPhaseFreqFuncs(void)
{
	static const char	*funcName = "InitClickModeList_RelClickPhaseFreqFuncs";
	static NameSpecifier	modeList[] = {

			{ "SIMPLE",		RELCLICKPHASEFREQFUNCS_CLICKMODE_SIMPLE },
			{ NO_FILE,		RELCLICKPHASEFREQFUNCS_CLICKMODE_FILE_MODE},
			{ "",			RELCLICKPHASEFREQFUNCS_CLICKMODE_NULL },
		};
	if ((rCPFFParsPtr->clickModeList = InitNameList_NSpecLists(modeList,
	  rCPFFParsPtr->stimFileName)) == NULL) {
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
Init_RelClickPhaseFreqFuncs(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_RelClickPhaseFreqFuncs";

	if (parSpec == GLOBAL) {
		if (rCPFFParsPtr != NULL)
			Free_RelClickPhaseFreqFuncs();
		if ((rCPFFParsPtr = (RCPFFParsPtr) malloc(sizeof(RCPFFPars))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (rCPFFParsPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	rCPFFParsPtr->parSpec = parSpec;
	rCPFFParsPtr->enabledFlag = FALSE;
	rCPFFParsPtr->outputModeFlag = FALSE;
	rCPFFParsPtr->clickModeFlag = FALSE;
	rCPFFParsPtr->analysisOutputModeFlag = FALSE;
	rCPFFParsPtr->clickTimeFlag = FALSE;
	rCPFFParsPtr->stimulusDurationFlag = FALSE;
	rCPFFParsPtr->dtFlag = FALSE;
	rCPFFParsPtr->dBAdjustmentFlag = FALSE;
	rCPFFParsPtr->numIntensitiesFlag = FALSE;
	rCPFFParsPtr->initialIntensityFlag = FALSE;
	rCPFFParsPtr->deltaIntensityFlag = FALSE;
	rCPFFParsPtr->refIntensityFlag = FALSE;
	rCPFFParsPtr->enabled = 0;
	rCPFFParsPtr->outputMode = 0;
	rCPFFParsPtr->clickMode = 0;
	rCPFFParsPtr->analysisOutputMode = 0;
	rCPFFParsPtr->clickTime = 0.0;
	rCPFFParsPtr->stimulusDuration = 0.0;
	rCPFFParsPtr->dt = 0.0;
	rCPFFParsPtr->dBAdjustment = 0.0;
	rCPFFParsPtr->numIntensities = 0;
	rCPFFParsPtr->initialIntensity = 0.0;
	rCPFFParsPtr->deltaIntensity = 0.0;
	rCPFFParsPtr->refIntensity = 0.0;

	InitClickModeList_RelClickPhaseFreqFuncs();
	sprintf(rCPFFParsPtr->stimFileName, NO_FILE);
	sprintf(rCPFFParsPtr->outputFile, NO_FILE);
	if ((rCPFFParsPtr->outputModeList = InitNameList_NSpecLists(
	  DiagModeList_NSpecLists(0), rCPFFParsPtr->outputFile)) == NULL)
		return(FALSE);
	rCPFFParsPtr->analysisOutputModeList = InitPhaseOutputModeList_Utils();
	rCPFFParsPtr->audModel = NULL;
	rCPFFParsPtr->stimulus = NULL;
	rCPFFParsPtr->setLevel = NULL;
	rCPFFParsPtr->analysis = NULL;
	rCPFFParsPtr->stimulusAnalysis = NULL;
	rCPFFParsPtr->gate = NULL;
	rCPFFParsPtr->resultEarObj = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_RelClickPhaseFreqFuncs(UniParPtr pars)
{
	static const char *funcName = "SetUniParList_RelClickPhaseFreqFuncs";

	if (rCPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	SetPar_UniParMgr(&pars[RELCLICKPHASEFREQFUNCS_ENABLED], "RCPFF_ENABLED",
	  "Phase intensity function test state 'on' or 'off'.",
	  UNIPAR_BOOL,
	  &rCPFFParsPtr->enabled, NULL,
	  (void * (*)) SetEnabled_RelClickPhaseFreqFuncs);
	SetPar_UniParMgr(&pars[RELCLICKPHASEFREQFUNCS_OUTPUTMODE],
	  "RCPFF_OUTPUT_MODE",
	  "Click magnitude/frequency function test output 'off', 'screen' or "
	  "<file name>.",
	  UNIPAR_NAME_SPEC_WITH_FILE,
	  &rCPFFParsPtr->outputMode, rCPFFParsPtr->outputModeList,
	  (void * (*)) SetOutputMode_RelClickPhaseFreqFuncs);
	SetPar_UniParMgr(&pars[RELCLICKPHASEFREQFUNCS_CLICKMODE],
	  "RCPFF_CLICK_MODE",
	  "Click mode ('simple' or '<datafile par. file name>').",
	  UNIPAR_NAME_SPEC_WITH_FILE,
	  &rCPFFParsPtr->clickMode, rCPFFParsPtr->clickModeList,
	  (void * (*)) SetClickMode_RelClickPhaseFreqFuncs);
	SetPar_UniParMgr(&pars[RELCLICKPHASEFREQFUNCS_ANALYSISOUTPUTMODE],
	  "RCPFF_ANAL_OUTPUT",
	  "Analysis output mode ('absolute' or 'relative').",
	  UNIPAR_NAME_SPEC,
	  &rCPFFParsPtr->analysisOutputMode, rCPFFParsPtr->analysisOutputModeList,
	  (void * (*)) SetAnalysisOutputMode_RelClickPhaseFreqFuncs);
	SetPar_UniParMgr(&pars[RELCLICKPHASEFREQFUNCS_CLICKTIME], "RCPFF_CLICKTIME",
	  "Stimulus click time (s).",
	  UNIPAR_REAL,
	  &rCPFFParsPtr->clickTime, NULL,
	  (void * (*)) SetClickTime_RelClickPhaseFreqFuncs);
	SetPar_UniParMgr(&pars[RELCLICKPHASEFREQFUNCS_STIMULUSDURATION],
	  "RCPFF_DURATION",
	  "Stimulus duration (s).",
	  UNIPAR_REAL,
	  &rCPFFParsPtr->stimulusDuration, NULL,
	  (void * (*)) SetStimulusDuration_RelClickPhaseFreqFuncs);
	SetPar_UniParMgr(&pars[RELCLICKPHASEFREQFUNCS_SAMPLINGINTERVAL], "RCPFF_DT",
	  "Stimulus Sampling interval, dt (s).",
	  UNIPAR_REAL,
	  &rCPFFParsPtr->dt, NULL,
	  (void * (*)) SetSamplingInterval_RelClickPhaseFreqFuncs);
	SetPar_UniParMgr(&pars[RELCLICKPHASEFREQFUNCS_DBADJUSTMENT],
	  "RCPFF_DB_ADJUST",
	  "Adjustment of dB scale to experiment (dB).",
	  UNIPAR_REAL,
	  &rCPFFParsPtr->dBAdjustment, NULL,
	  (void * (*)) SetDBAdjustment_RelClickPhaseFreqFuncs);
	SetPar_UniParMgr(&pars[RELCLICKPHASEFREQFUNCS_NUMINTENSITIES],
	  "RCPFF_NUM_INTENSTIES",
	  "No. of test intensities.",
	  UNIPAR_INT,
	  &rCPFFParsPtr->numIntensities, NULL,
	  (void * (*)) SetNumIntensities_RelClickPhaseFreqFuncs);
	SetPar_UniParMgr(&pars[RELCLICKPHASEFREQFUNCS_INITIALINTENSITY],
	  "RCPFF_INIT_INTENSITY",
	  "Initial test intensity (dB SPL).",
	  UNIPAR_REAL,
	  &rCPFFParsPtr->initialIntensity, NULL,
	  (void * (*)) SetInitialIntensity_RelClickPhaseFreqFuncs);
	SetPar_UniParMgr(&pars[RELCLICKPHASEFREQFUNCS_DELTAINTENSITY],
	  "RCPFF_DELTA_INTENSITY",
	  "Intensity increment (dB).",
	  UNIPAR_REAL,
	  &rCPFFParsPtr->deltaIntensity, NULL,
	  (void * (*)) SetDeltaIntensity_RelClickPhaseFreqFuncs);
	SetPar_UniParMgr(&pars[RELCLICKPHASEFREQFUNCS_REFINTENSITY],
	  "RCPFF_REL_INTENSITY",
	  "Relative intensity reference (dB).",
	  UNIPAR_REAL,
	  &rCPFFParsPtr->refIntensity, NULL,
	  (void * (*)) SetRefIntensity_RelClickPhaseFreqFuncs);
	return(TRUE);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_RelClickPhaseFreqFuncs(char * enabled, char * outputMode,
  char * clickMode, char * analysisOutputMode, double clickTime,
  double stimulusDuration, double samplingInterval, double dBAdjustment,
  int numIntensities, double initialIntensity, double deltaIntensity,
  double refIntensity)
{
	static const char	*funcName = "SetPars_RelClickPhaseFreqFuncs";
	BOOLN	ok;

	ok = TRUE;
	if (!SetEnabled_RelClickPhaseFreqFuncs(enabled))
		ok = FALSE;
	if (!SetOutputMode_RelClickPhaseFreqFuncs(outputMode))
		ok = FALSE;
	if (!SetClickMode_RelClickPhaseFreqFuncs(clickMode))
		ok = FALSE;
	if (!SetAnalysisOutputMode_RelClickPhaseFreqFuncs(analysisOutputMode))
		ok = FALSE;
	if (!SetClickTime_RelClickPhaseFreqFuncs(clickTime))
		ok = FALSE;
	if (!SetStimulusDuration_RelClickPhaseFreqFuncs(stimulusDuration))
		ok = FALSE;
	if (!SetSamplingInterval_RelClickPhaseFreqFuncs(samplingInterval))
		ok = FALSE;
	if (!SetDBAdjustment_RelClickPhaseFreqFuncs(dBAdjustment))
		ok = FALSE;
	if (!SetNumIntensities_RelClickPhaseFreqFuncs(numIntensities))
		ok = FALSE;
	if (!SetInitialIntensity_RelClickPhaseFreqFuncs(initialIntensity))
		ok = FALSE;
	if (!SetDeltaIntensity_RelClickPhaseFreqFuncs(deltaIntensity))
		ok = FALSE;
	if (!SetRefIntensity_RelClickPhaseFreqFuncs(refIntensity))
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
SetEnabled_RelClickPhaseFreqFuncs(char * theEnabled)
{
	static const char	*funcName = "SetEnabled_RelClickPhaseFreqFuncs";
	int		specifier;

	if (rCPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theEnabled, BooleanList_NSpecLists(
	  0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError("%s: Illegal name (%s).", funcName, theEnabled);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	rCPFFParsPtr->enabledFlag = TRUE;
	rCPFFParsPtr->enabled = specifier;
	return(TRUE);

}

/****************************** SetOutputMode *********************************/

/*
 * This function sets the module's outputMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOutputMode_RelClickPhaseFreqFuncs(char * theOutputMode)
{
	static const char	*funcName = "SetOutputMode_RelClickPhaseFreqFuncs";

	if (rCPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	rCPFFParsPtr->outputMode = IdentifyDiag_NSpecLists(theOutputMode,
	  rCPFFParsPtr->outputModeList);
	rCPFFParsPtr->outputModeFlag = TRUE;
	return(TRUE);

}

/****************************** SetClickMode **********************************/

/*
 * This function sets the module's clickMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetClickMode_RelClickPhaseFreqFuncs(char * theClickMode)
{
	static const char	*funcName = "SetClickMode_RelClickPhaseFreqFuncs";
	int		specifier;

	if (rCPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	specifier = Identify_NameSpecifier(theClickMode,
	  rCPFFParsPtr->clickModeList);
	if ((specifier == RELCLICKPHASEFREQFUNCS_CLICKMODE_FILE_MODE) ||
	  (specifier == RELCLICKPHASEFREQFUNCS_CLICKMODE_NULL)) {
		sprintf(rCPFFParsPtr->stimFileName, theClickMode);
		specifier = RELCLICKPHASEFREQFUNCS_CLICKMODE_FILE_MODE;
	}
	rCPFFParsPtr->clickMode = specifier;
	rCPFFParsPtr->clickModeFlag = TRUE;
	return(TRUE);

}

/****************************** SetAnalysisOutputMode *************************/

/*
 * This function sets the module's analysisOutputMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetAnalysisOutputMode_RelClickPhaseFreqFuncs(char * theAnalysisOutputMode)
{
	static const char	*funcName =
	  "SetAnalysisOutputMode_RelClickPhaseFreqFuncs";
	int		specifier;

	if (rCPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theAnalysisOutputMode,
	  rCPFFParsPtr->analysisOutputModeList)) ==
	  UTILS_PHASE_ANALYSISOUTPUTMODE_NULL) {
		NotifyError("%s: Illegal name (%s).", funcName, theAnalysisOutputMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	rCPFFParsPtr->analysisOutputModeFlag = TRUE;
	rCPFFParsPtr->analysisOutputMode = specifier;
	return(TRUE);

}

/****************************** SetClickTime **********************************/

/*
 * This function sets the module's clickTime parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetClickTime_RelClickPhaseFreqFuncs(double theClickTime)
{
	static const char	*funcName = "SetClickTime_RelClickPhaseFreqFuncs";

	if (rCPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	rCPFFParsPtr->clickTimeFlag = TRUE;
	rCPFFParsPtr->clickTime = theClickTime;
	return(TRUE);

}

/****************************** SetStimulusDuration ***************************/

/*
 * This function sets the module's stimulusDuration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetStimulusDuration_RelClickPhaseFreqFuncs(double theStimulusDuration)
{
	static const char	*funcName =
	  "SetStimulusDuration_RelClickPhaseFreqFuncs";

	if (rCPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	rCPFFParsPtr->stimulusDurationFlag = TRUE;
	rCPFFParsPtr->stimulusDuration = theStimulusDuration;
	return(TRUE);

}

/****************************** SetSamplingInterval ***************************/

/*
 * This function sets the module's samplingInterval parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSamplingInterval_RelClickPhaseFreqFuncs(double theSamplingInterval)
{
	static const char	*funcName =
	  "SetSamplingInterval_RelClickPhaseFreqFuncs";

	if (rCPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	rCPFFParsPtr->dtFlag = TRUE;
	rCPFFParsPtr->dt = theSamplingInterval;
	return(TRUE);

}

/****************************** SetDBAdjustment *******************************/

/*
 * This function sets the module's dBAdjustment parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDBAdjustment_RelClickPhaseFreqFuncs(double theDBAdjustment)
{
	static const char	*funcName = "SetDBAdjustment_RelClickPhaseFreqFuncs";

	if (rCPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	rCPFFParsPtr->dBAdjustmentFlag = TRUE;
	rCPFFParsPtr->dBAdjustment = theDBAdjustment;
	return(TRUE);

}

/****************************** SetNumIntensities *****************************/

/*
 * This function sets the module's numIntensities parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNumIntensities_RelClickPhaseFreqFuncs(int theNumIntensities)
{
	static const char	*funcName = "SetNumIntensities_RelClickPhaseFreqFuncs";

	if (rCPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	rCPFFParsPtr->numIntensitiesFlag = TRUE;
	rCPFFParsPtr->numIntensities = theNumIntensities;
	return(TRUE);

}

/****************************** SetInitialIntensity ***************************/

/*
 * This function sets the module's initialIntensity parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetInitialIntensity_RelClickPhaseFreqFuncs(double theInitialIntensity)
{
	static const char	*funcName =
	  "SetInitialIntensity_RelClickPhaseFreqFuncs";

	if (rCPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	rCPFFParsPtr->initialIntensityFlag = TRUE;
	rCPFFParsPtr->initialIntensity = theInitialIntensity;
	return(TRUE);

}

/****************************** SetDeltaIntensity *****************************/

/*
 * This function sets the module's deltaIntensity parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDeltaIntensity_RelClickPhaseFreqFuncs(double theDeltaIntensity)
{
	static const char	*funcName = "SetDeltaIntensity_RelClickPhaseFreqFuncs";

	if (rCPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	rCPFFParsPtr->deltaIntensityFlag = TRUE;
	rCPFFParsPtr->deltaIntensity = theDeltaIntensity;
	return(TRUE);

}

/****************************** SetRefIntensity *******************************/

/*
 * This function sets the module's refIntensity parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRefIntensity_RelClickPhaseFreqFuncs(double theRefIntensity)
{
	static const char	*funcName = "SetRefIntensity_RelClickPhaseFreqFuncs";

	if (rCPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	rCPFFParsPtr->refIntensityFlag = TRUE;
	rCPFFParsPtr->refIntensity = theRefIntensity;
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
CheckPars_RelClickPhaseFreqFuncs(void)
{
	static const char	*funcName = "CheckPars_RelClickPhaseFreqFuncs";
	BOOLN	ok;

	ok = TRUE;
	if (rCPFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!rCPFFParsPtr->enabledFlag) {
		NotifyError("%s: enabled variable not set.", funcName);
		ok = FALSE;
	}
	if (!rCPFFParsPtr->outputModeFlag) {
		NotifyError("%s: outputMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!rCPFFParsPtr->clickModeFlag) {
		NotifyError("%s: clickMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!rCPFFParsPtr->analysisOutputModeFlag) {
		NotifyError("%s: analysisOutputMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!rCPFFParsPtr->clickTimeFlag) {
		NotifyError("%s: clickTime variable not set.", funcName);
		ok = FALSE;
	}
	if (!rCPFFParsPtr->stimulusDurationFlag) {
		NotifyError("%s: stimulusDuration variable not set.", funcName);
		ok = FALSE;
	}
	if (!rCPFFParsPtr->dtFlag) {
		NotifyError("%s: dt variable not set.", funcName);
		ok = FALSE;
	}
	if (!rCPFFParsPtr->dBAdjustmentFlag) {
		NotifyError("%s: dBAdjustment variable not set.", funcName);
		ok = FALSE;
	}
	if (!rCPFFParsPtr->numIntensitiesFlag) {
		NotifyError("%s: numIntensities variable not set.", funcName);
		ok = FALSE;
	}
	if (!rCPFFParsPtr->initialIntensityFlag) {
		NotifyError("%s: initialIntensity variable not set.", funcName);
		ok = FALSE;
	}
	if (!rCPFFParsPtr->deltaIntensityFlag) {
		NotifyError("%s: deltaIntensity variable not set.", funcName);
		ok = FALSE;
	}
	if (!rCPFFParsPtr->refIntensityFlag) {
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
PrintPars_RelClickPhaseFreqFuncs(void)
{
	static const char	*funcName = "PrintPars_RelClickPhaseFreqFuncs";

	if (!CheckPars_RelClickPhaseFreqFuncs()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("?? RelClickPhaseFreqFuncs Module Parameters:-\n");
	DPrint("\tenabled = %s \n", BooleanList_NSpecLists(
	  rCPFFParsPtr->enabled)->name);
	DPrint("\toutputMode = %s \n", rCPFFParsPtr->outputModeList[
	  rCPFFParsPtr->outputMode].name);
	DPrint("\tclickMode = %s \n", rCPFFParsPtr->clickModeList[
	  rCPFFParsPtr->clickMode].name);
	DPrint("\tanalysisOutputMode = %s \n",
	  rCPFFParsPtr->analysisOutputModeList
	  [rCPFFParsPtr->analysisOutputMode].name);
	DPrint("\tclickTime = %g ??\n", rCPFFParsPtr->clickTime);
	DPrint("\tstimulusDuration = %g ??\n", rCPFFParsPtr->stimulusDuration);
	DPrint("\tsamplingInterval = %g ??\n", rCPFFParsPtr->dt);
	DPrint("\tdBAdjustment = %g ??\n", rCPFFParsPtr->dBAdjustment);
	DPrint("\tnumIntensities = %d ??\n", rCPFFParsPtr->numIntensities);
	DPrint("\tinitialIntensity = %g ??\n", rCPFFParsPtr->initialIntensity);
	DPrint("\tdeltaIntensity = %g ??\n", rCPFFParsPtr->deltaIntensity);
	DPrint("\trefIntensity = %g ??\n", rCPFFParsPtr->refIntensity);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_RelClickPhaseFreqFuncs(FILE *fp)
{
	static const char	*funcName = "ReadPars_RelClickPhaseFreqFuncs";
	BOOLN	ok = TRUE;
	int		numIntensities;
	char	enabled[MAXLINE], outputMode[MAXLINE];
	char	clickMode[MAXLINE], analysisOutputMode[MAXLINE];
	double	clickTime, stimulusDuration, samplingInterval;
	double	dBAdjustment, initialIntensity, deltaIntensity, refIntensity;

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
	if (!GetPars_ParFile(fp, "%lf", &refIntensity))
		ok = FALSE;
	if (!ok) {
		NotifyError("%s: Failed to read parameters.", funcName);
		return(FALSE);
	}
	if (!SetPars_RelClickPhaseFreqFuncs(enabled, outputMode, clickMode,
	  analysisOutputMode, clickTime, stimulusDuration, samplingInterval,
	  dBAdjustment, numIntensities, initialIntensity, deltaIntensity,
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
CheckData_RelClickPhaseFreqFuncs(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_RelClickPhaseFreqFuncs";

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
InitProcessVariables_RelClickPhaseFreqFuncs(EarObjectPtr data)
{
	static const char * funcName = "InitProcessVariables_RelClickPhaseFreqFunc";
	double	realIndex;
	DatumPtr	pc;
	RCPFFParsPtr	p = rCPFFParsPtr;

	p->audModel = data;
	if ((p->stimulus = Init_EarObject((p->clickMode ==
	  RELCLICKPHASEFREQFUNCS_CLICKMODE_SIMPLE)? STIM_CLICK_PROCESS_NAME:
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
	case RELCLICKPHASEFREQFUNCS_CLICKMODE_SIMPLE:
		if (!SetRealPar_ModuleMgr(p->stimulus, "amplitude",
		  RELCLICK_DEFAULT_CLICK_MAGNITUDE)) {
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
	SetInfoSampleTitle_SignalData(p->resultEarObj->outSignal, "Frequency (Hz)");
	realIndex = (p->refIntensity - p->initialIntensity) / p->deltaIntensity;
	p->refIntIndex = (int) floor(realIndex + 0.5);
	if ((fabs(p->refIntIndex - realIndex) > DBL_EPSILON) || (p->refIntIndex <
	  0) || (p->refIntIndex >= p->numIntensities)) {
		NotifyError("%s: Reference intensity (%g dB) is not\nin intensity "
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
FreeProcessVariables_RelClickPhaseFreqFuncs(void)
{
	if (rCPFFParsPtr->stimulus && rCPFFParsPtr->audModel)
		DisconnectOutSignalFromIn_EarObject(rCPFFParsPtr->stimulus,
		  rCPFFParsPtr->audModel);
	if (rCPFFParsPtr->stimulus && rCPFFParsPtr->stimulusAnalysis)
		DisconnectOutSignalFromIn_EarObject(rCPFFParsPtr->stimulus,
		  rCPFFParsPtr->stimulusAnalysis);
	if (rCPFFParsPtr->audModel && rCPFFParsPtr->analysis)
		DisconnectOutSignalFromIn_EarObject(rCPFFParsPtr->audModel,
		  rCPFFParsPtr->analysis);
	Free_EarObject(&rCPFFParsPtr->stimulus);
	Free_EarObject(&rCPFFParsPtr->setLevel);
	Free_EarObject(&rCPFFParsPtr->analysis);
	Free_EarObject(&rCPFFParsPtr->stimulusAnalysis);
	Free_EarObject(&rCPFFParsPtr->resultEarObj);
	if (rCPFFParsPtr->gate) {
		Enable_ModuleMgr(rCPFFParsPtr->gate, TRUE);
		rCPFFParsPtr->gate = NULL;
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
RunProcess_RelClickPhaseFreqFuncs(EarObjectPtr data)
{
	static const char	*funcName = "RunProcess_RelClickPhaseFreqFuncs";
	register ChanData	*stimFTPtr, *modFTPtr, *outPtr;
	int		i;
	ChanLen	j;
	double	intensity, relativePhase;
	RCPFFParsPtr	p = rCPFFParsPtr;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckPars_RelClickPhaseFreqFuncs())
		return(FALSE);
	if (!CheckData_RelClickPhaseFreqFuncs(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	if (!InitProcessVariables_RelClickPhaseFreqFuncs(data)) {
		NotifyError("%s: Could not initialise the process variables.",
		  funcName);
		FreeProcessVariables_RelClickPhaseFreqFuncs();
		return(FALSE);
	}
	for (i = 0, intensity = p->initialIntensity; i < p->numIntensities; i++,
	  intensity += p->deltaIntensity) {
		if (!SetRealPar_ModuleMgr(p->setLevel, "dBSPL", intensity +
		  p->dBAdjustment)) {
			NotifyError("%s: Cannot set the level intensity.", funcName);
			FreeProcessVariables_RelClickPhaseFreqFuncs();
			return(FALSE);
		}
		if (!RunProcess_ModuleMgr(p->stimulus)) {
			NotifyError("%s: Failed to run stimulus process.", funcName);
			FreeProcessVariables_RelClickPhaseFreqFuncs();
			return(FALSE);
		}
		if (!RunProcess_ModuleMgr(p->setLevel)) {
			NotifyError("%s: Failed to run the set-level process.", funcName);
			FreeProcessVariables_RelClickPhaseFreqFuncs();
			return(FALSE);
		}
		if (!RunProcess_ModuleMgr(p->stimulusAnalysis)) {
			NotifyError("%s: Failed to run stimulus analysis process.",
			  funcName);
			FreeProcessVariables_RelClickPhaseFreqFuncs();
			return(FALSE);
		}
		if (!RunProcess_ModuleMgr(p->audModel)) {
			NotifyError("%s: Failed to run auditory model.", funcName);
			FreeProcessVariables_RelClickPhaseFreqFuncs();
			return(FALSE);
		}
		if (!RunProcess_ModuleMgr(p->analysis)) {
			NotifyError("%s: Failed to run analysis process.", funcName);
			FreeProcessVariables_RelClickPhaseFreqFuncs();
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
	for (i = 0; i < p->numIntensities; i++)
		for (j = 0; (i != p->refIntIndex) && (j <
		  p->resultEarObj->outSignal->length); j++)
			p->resultEarObj->outSignal->channel[i][j] =
			  p->resultEarObj->outSignal->channel[p->refIntIndex][j] -
			  p->resultEarObj->outSignal->channel[i][j];
	for (j = 0; j < p->resultEarObj->outSignal->length; j++)
		p->resultEarObj->outSignal->channel[p->refIntIndex][j] = 0.0;
	OutputResults_Utils(p->resultEarObj, &p->outputModeList[p->outputMode],
	  UTILS_NORMAL_SIGNAL);
	FreeProcessVariables_RelClickPhaseFreqFuncs();
	return(TRUE);

}

