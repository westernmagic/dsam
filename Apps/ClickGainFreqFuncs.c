/**********************
 *
 * File:		ClickGainFreqFuncs.c
 * Purpose:		Calculates the click gain/frequency response using an FFT.
 * Comments:	Written using ModuleProducer version 1.2.10 (Jan  9 2001).
 *				The sampling interval parameter is not used for the simulation
 *				file stimulus option.
 * Author:		L. P. O'Mard
 * Created:		12 Jan 2001
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

#include "DSAM.h"
#include "Utils.h"
#include "ClickGainFreqFuncs.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

CGFFParsPtr	cGFFParsPtr = NULL;

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
Free_ClickGainFreqFuncs(void)
{
	/* static const char	*funcName = "Free_ClickGainFreqFuncs";  */

	if (cGFFParsPtr == NULL)
		return(FALSE);
	if (cGFFParsPtr->parSpec == GLOBAL) {
		free(cGFFParsPtr);
		cGFFParsPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitClickModeList *****************************/

/*
 * This function initialises the 'clickMode' list array
 */

BOOLN
InitClickModeList_ClickGainFreqFuncs(void)
{
	static const char	*funcName = "InitClickModeList_ClickGainFreqFuncs";
	static NameSpecifier	modeList[] = {

			{ "SIMPLE",		CLICKGAINFREQFUNCS_CLICKMODE_SIMPLE },
			{ NO_FILE,		CLICKGAINFREQFUNCS_FILE_MODE},
			{ "",			CLICKGAINFREQFUNCS_CLICKMODE_NULL },
		};
	if ((cGFFParsPtr->clickModeList = InitNameList_NSpecLists(modeList,
	  cGFFParsPtr->stimFileName)) == NULL) {
		NotifyError("%s: Could not initialise list.", funcName);
		return(FALSE);
	}
	return(TRUE);

}

/****************************** InitAnalysisOutputModeList ********************/

/*
 * This function initialises the 'analysisOutputMode' list array
 */

BOOLN
InitAnalysisOutputModeList_ClickGainFreqFuncs(void)
{
	static NameSpecifier	modeList[] = {

			{ "ABSOLUTE",	CLICKGAINFREQFUNCS_ANALYSISOUTPUTMODE_ABSOLUTE },
			{ "RELATIVE",	CLICKGAINFREQFUNCS_ANALYSISOUTPUTMODE_RELATIVE },
			{ "",			CLICKGAINFREQFUNCS_ANALYSISOUTPUTMODE_NULL },
		};
	cGFFParsPtr->analysisOutputModeList = modeList;
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
Init_ClickGainFreqFuncs(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_ClickGainFreqFuncs";

	if (parSpec == GLOBAL) {
		if (cGFFParsPtr != NULL)
			Free_ClickGainFreqFuncs();
		if ((cGFFParsPtr = (CGFFParsPtr) malloc(sizeof(CGFFPars))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (cGFFParsPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	cGFFParsPtr->parSpec = parSpec;
	cGFFParsPtr->enabledFlag = FALSE;
	cGFFParsPtr->outputModeFlag = FALSE;
	cGFFParsPtr->clickModeFlag = FALSE;
	cGFFParsPtr->analysisOutputModeFlag = FALSE;
	cGFFParsPtr->clickTimeFlag = FALSE;
	cGFFParsPtr->stimulusDurationFlag = FALSE;
	cGFFParsPtr->dtFlag = FALSE;
	cGFFParsPtr->dBAdjustmentFlag = FALSE;
	cGFFParsPtr->numIntensitiesFlag = FALSE;
	cGFFParsPtr->initialIntensityFlag = FALSE;
	cGFFParsPtr->deltaIntensityFlag = FALSE;
	cGFFParsPtr->enabled = 0;
	cGFFParsPtr->outputMode = 0;
	cGFFParsPtr->clickMode = 0;
	cGFFParsPtr->analysisOutputMode = 0;
	cGFFParsPtr->clickTime = 0.0;
	cGFFParsPtr->stimulusDuration = 0.0;
	cGFFParsPtr->dt = 0.0;
	cGFFParsPtr->dBAdjustment = 0.0;
	cGFFParsPtr->numIntensities = 0;
	cGFFParsPtr->initialIntensity = 0.0;
	cGFFParsPtr->deltaIntensity = 0.0;

	InitClickModeList_ClickGainFreqFuncs();
	sprintf(cGFFParsPtr->stimFileName, NO_FILE);
	sprintf(cGFFParsPtr->outputFile, NO_FILE);
	if ((cGFFParsPtr->outputModeList = InitNameList_NSpecLists(
	  DiagModeList_NSpecLists(0), cGFFParsPtr->outputFile)) == NULL)
		return(FALSE);
	InitAnalysisOutputModeList_ClickGainFreqFuncs();
	cGFFParsPtr->audModel = NULL;
	cGFFParsPtr->stimulus = NULL;
	cGFFParsPtr->setLevel = NULL;
	cGFFParsPtr->analysis = NULL;
	cGFFParsPtr->stimulusAnalysis = NULL;
	cGFFParsPtr->gate = NULL;
	cGFFParsPtr->resultEarObj = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_ClickGainFreqFuncs(UniParPtr pars)
{
	static const char *funcName = "SetUniParList_ClickGainFreqFuncs";

	if (cGFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	SetPar_UniParMgr(&pars[CLICKGAINFREQFUNCS_ENABLED], "CGFF_ENABLED",
	  "Phase intensity function test state 'on' or 'off'.",
	  UNIPAR_BOOL,
	  &cGFFParsPtr->enabled, NULL,
	  (void * (*)) SetEnabled_ClickGainFreqFuncs);
	SetPar_UniParMgr(&pars[CLICKGAINFREQFUNCS_OUTPUTMODE], "CGFF_OUTPUT_MODE",
	  "Click magnitude/frequency function test output 'off', 'screen' or "
	  "<file name>.",
	  UNIPAR_NAME_SPEC_WITH_FILE,
	  &cGFFParsPtr->outputMode, cGFFParsPtr->outputModeList,
	  (void * (*)) SetOutputMode_ClickGainFreqFuncs);
	SetPar_UniParMgr(&pars[CLICKGAINFREQFUNCS_CLICKMODE], "CGFF_CLICK_MODE",
	  "Click mode ('simple' or '<datafile par. file name>').",
	  UNIPAR_NAME_SPEC_WITH_FILE,
	  &cGFFParsPtr->clickMode, cGFFParsPtr->clickModeList,
	  (void * (*)) SetClickMode_ClickGainFreqFuncs);
	SetPar_UniParMgr(&pars[CLICKGAINFREQFUNCS_ANALYSISOUTPUTMODE],
	  "CGFF_ANAL_OUTPUT",
	  "Analysis output mode ('absolute' or 'relative').",
	  UNIPAR_NAME_SPEC,
	  &cGFFParsPtr->analysisOutputMode, cGFFParsPtr->analysisOutputModeList,
	  (void * (*)) SetAnalysisOutputMode_ClickGainFreqFuncs);
	SetPar_UniParMgr(&pars[CLICKGAINFREQFUNCS_CLICKTIME], "CGFF_CLICKTIME",
	  "Stimulus click time (s).",
	  UNIPAR_REAL,
	  &cGFFParsPtr->clickTime, NULL,
	  (void * (*)) SetClickTime_ClickGainFreqFuncs);
	SetPar_UniParMgr(&pars[CLICKGAINFREQFUNCS_STIMULUSDURATION],
	  "CGFF_DURATION",
	  "Stimulus duration (s).",
	  UNIPAR_REAL,
	  &cGFFParsPtr->stimulusDuration, NULL,
	  (void * (*)) SetStimulusDuration_ClickGainFreqFuncs);
	SetPar_UniParMgr(&pars[CLICKGAINFREQFUNCS_SAMPLINGINTERVAL], "CGFF_DT",
	  "Stimulus Sampling interval, dt (s).",
	  UNIPAR_REAL,
	  &cGFFParsPtr->dt, NULL,
	  (void * (*)) SetSamplingInterval_ClickGainFreqFuncs);
	SetPar_UniParMgr(&pars[CLICKGAINFREQFUNCS_DBAJDUSTMENT], "CGFF_DB_ADJUST",
	  "Adjustment of dB scale to experiment (dB).",
	  UNIPAR_REAL,
	  &cGFFParsPtr->dBAdjustment, NULL,
	  (void * (*)) SetDBAdjustment_ClickGainFreqFuncs);
	SetPar_UniParMgr(&pars[CLICKGAINFREQFUNCS_NUMINTENSITIES],
	  "CGFF_NUM_INTENSTIES",
	  "No. of test intensities.",
	  UNIPAR_INT,
	  &cGFFParsPtr->numIntensities, NULL,
	  (void * (*)) SetNumIntensities_ClickGainFreqFuncs);
	SetPar_UniParMgr(&pars[CLICKGAINFREQFUNCS_INITIALINTENSITY],
	  "CGFF_INIT_INTENSITY",
	  "Initial test intensity (dB SPL).",
	  UNIPAR_REAL,
	  &cGFFParsPtr->initialIntensity, NULL,
	  (void * (*)) SetInitialIntensity_ClickGainFreqFuncs);
	SetPar_UniParMgr(&pars[CLICKGAINFREQFUNCS_DELTAINTENSITY],
	  "CGFF_DELTA_INTENSITY",
	  "Intensity increment (dB).",
	  UNIPAR_REAL,
	  &cGFFParsPtr->deltaIntensity, NULL,
	  (void * (*)) SetDeltaIntensity_ClickGainFreqFuncs);
	return(TRUE);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_ClickGainFreqFuncs(char * enabled, char * outputMode,
  char * clickMode, char * analysisOutputMode, double clickTime,
  double dBAdjustment, double stimulusDuration, double samplingInterval,
  int numIntensities, double initialIntensity, double deltaIntensity)
{
	static const char	*funcName = "SetPars_ClickGainFreqFuncs";
	BOOLN	ok;

	ok = TRUE;
	if (!SetEnabled_ClickGainFreqFuncs(enabled))
		ok = FALSE;
	if (!SetOutputMode_ClickGainFreqFuncs(outputMode))
		ok = FALSE;
	if (!SetClickMode_ClickGainFreqFuncs(clickMode))
		ok = FALSE;
	if (!SetAnalysisOutputMode_ClickGainFreqFuncs(analysisOutputMode))
		ok = FALSE;
	if (!SetClickTime_ClickGainFreqFuncs(clickTime))
		ok = FALSE;
	if (!SetStimulusDuration_ClickGainFreqFuncs(stimulusDuration))
		ok = FALSE;
	if (!SetSamplingInterval_ClickGainFreqFuncs(samplingInterval))
		ok = FALSE;
	if (!SetDBAdjustment_ClickGainFreqFuncs(dBAdjustment))
		ok = FALSE;
	if (!SetNumIntensities_ClickGainFreqFuncs(numIntensities))
		ok = FALSE;
	if (!SetInitialIntensity_ClickGainFreqFuncs(initialIntensity))
		ok = FALSE;
	if (!SetDeltaIntensity_ClickGainFreqFuncs(deltaIntensity))
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
SetEnabled_ClickGainFreqFuncs(char * theEnabled)
{
	static const char	*funcName = "SetEnabled_ClickGainFreqFuncs";
	int		specifier;

	if (cGFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theEnabled, BooleanList_NSpecLists(
	  0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError("%s: Illegal name (%s).", funcName, theEnabled);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cGFFParsPtr->enabledFlag = TRUE;
	cGFFParsPtr->enabled = specifier;
	return(TRUE);

}

/****************************** SetOutputMode *********************************/

/*
 * This function sets the module's outputMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOutputMode_ClickGainFreqFuncs(char * theOutputMode)
{
	static const char	*funcName = "SetOutputMode_ClickGainFreqFuncs";

	if (cGFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	cGFFParsPtr->outputMode = IdentifyDiag_NSpecLists(theOutputMode,
	  cGFFParsPtr->outputModeList);
	cGFFParsPtr->outputModeFlag = TRUE;
	return(TRUE);

}

/****************************** SetClickMode **********************************/

/*
 * This function sets the module's clickMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetClickMode_ClickGainFreqFuncs(char * theClickMode)
{
	static const char	*funcName = "SetClickMode_ClickGainFreqFuncs";
	int		specifier;

	if (cGFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	specifier = Identify_NameSpecifier(theClickMode,
	  cGFFParsPtr->clickModeList);
	if ((specifier == CLICKGAINFREQFUNCS_FILE_MODE) || (specifier == 
	  CLICKGAINFREQFUNCS_CLICKMODE_NULL)) {
		sprintf(cGFFParsPtr->stimFileName, theClickMode);
		specifier = CLICKGAINFREQFUNCS_FILE_MODE;
	}
	cGFFParsPtr->clickMode = specifier;
	cGFFParsPtr->clickModeFlag = TRUE;
	return(TRUE);

}

/****************************** SetClickTime **********************************/

/*
 * This function sets the module's stimulus click time parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetClickTime_ClickGainFreqFuncs(double theClickTime)
{
	static const char	*funcName = "SetClickTime_ClickGainFreqFuncs";

	if (cGFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cGFFParsPtr->clickTimeFlag = TRUE;
	cGFFParsPtr->clickTime = theClickTime;
	return(TRUE);

}

/****************************** SetDBAdjustment *******************************/

/*
 * This function sets the module's dBAdjustment parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDBAdjustment_ClickGainFreqFuncs(double theDBAdjustment)
{
	static const char	*funcName = "SetDBAdjustment_ClickGainFreqFuncs";

	if (cGFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cGFFParsPtr->dBAdjustmentFlag = TRUE;
	cGFFParsPtr->dBAdjustment = theDBAdjustment;
	return(TRUE);

}

/****************************** SetAnalysisOutputMode *************************/

/*
 * This function sets the module's analysisOutputMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetAnalysisOutputMode_ClickGainFreqFuncs(char * theAnalysisOutputMode)
{
	static const char	*funcName = "SetAnalysisOutputMode_ClickGainFreqFuncs";
	int		specifier;

	if (cGFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theAnalysisOutputMode,
		cGFFParsPtr->analysisOutputModeList)) == 
		  CLICKGAINFREQFUNCS_ANALYSISOUTPUTMODE_NULL) {
		NotifyError("%s: Illegal name (%s).", funcName, theAnalysisOutputMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cGFFParsPtr->analysisOutputModeFlag = TRUE;
	cGFFParsPtr->analysisOutputMode = specifier;
	return(TRUE);

}

/****************************** SetStimulusDuration ***************************/

/*
 * This function sets the module's stimulusDuration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetStimulusDuration_ClickGainFreqFuncs(double theStimulusDuration)
{
	static const char	*funcName = "SetStimulusDuration_ClickGainFreqFuncs";

	if (cGFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cGFFParsPtr->stimulusDurationFlag = TRUE;
	cGFFParsPtr->stimulusDuration = theStimulusDuration;
	return(TRUE);

}

/****************************** SetSamplingInterval ***************************/

/*
 * This function sets the module's samplingInterval parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSamplingInterval_ClickGainFreqFuncs(double theSamplingInterval)
{
	static const char	*funcName = "SetSamplingInterval_ClickGainFreqFuncs";

	if (cGFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cGFFParsPtr->dtFlag = TRUE;
	cGFFParsPtr->dt = theSamplingInterval;
	return(TRUE);

}

/****************************** SetNumIntensities *****************************/

/*
 * This function sets the module's numIntensities parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNumIntensities_ClickGainFreqFuncs(int theNumIntensities)
{
	static const char	*funcName = "SetNumIntensities_ClickGainFreqFuncs";

	if (cGFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cGFFParsPtr->numIntensitiesFlag = TRUE;
	cGFFParsPtr->numIntensities = theNumIntensities;
	return(TRUE);

}

/****************************** SetInitialIntensity ***************************/

/*
 * This function sets the module's initialIntensity parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetInitialIntensity_ClickGainFreqFuncs(double theInitialIntensity)
{
	static const char	*funcName = "SetInitialIntensity_ClickGainFreqFuncs";

	if (cGFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cGFFParsPtr->initialIntensityFlag = TRUE;
	cGFFParsPtr->initialIntensity = theInitialIntensity;
	return(TRUE);

}

/****************************** SetDeltaIntensity *****************************/

/*
 * This function sets the module's deltaIntensity parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDeltaIntensity_ClickGainFreqFuncs(double theDeltaIntensity)
{
	static const char	*funcName = "SetDeltaIntensity_ClickGainFreqFuncs";

	if (cGFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cGFFParsPtr->deltaIntensityFlag = TRUE;
	cGFFParsPtr->deltaIntensity = theDeltaIntensity;
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
CheckPars_ClickGainFreqFuncs(void)
{
	static const char	*funcName = "CheckPars_ClickGainFreqFuncs";
	BOOLN	ok;

	ok = TRUE;
	if (cGFFParsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!cGFFParsPtr->enabledFlag) {
		NotifyError("%s: enabled variable not set.", funcName);
		ok = FALSE;
	}
	if (!cGFFParsPtr->outputModeFlag) {
		NotifyError("%s: outputMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!cGFFParsPtr->clickModeFlag) {
		NotifyError("%s: clickMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!cGFFParsPtr->analysisOutputModeFlag) {
		NotifyError("%s: analysisOutputMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!cGFFParsPtr->clickTimeFlag) {
		NotifyError("%s: click time variable not set.", funcName);
		ok = FALSE;
	}
	if (!cGFFParsPtr->stimulusDurationFlag) {
		NotifyError("%s: stimulusDuration variable not set.", funcName);
		ok = FALSE;
	}
	if (!cGFFParsPtr->dtFlag) {
		NotifyError("%s: dt variable not set.", funcName);
		ok = FALSE;
	}
	if (!cGFFParsPtr->dBAdjustmentFlag) {
		NotifyError("%s: dBAdjustment variable not set.", funcName);
		ok = FALSE;
	}
	if (!cGFFParsPtr->numIntensitiesFlag) {
		NotifyError("%s: numIntensities variable not set.", funcName);
		ok = FALSE;
	}
	if (!cGFFParsPtr->initialIntensityFlag) {
		NotifyError("%s: initialIntensity variable not set.", funcName);
		ok = FALSE;
	}
	if (!cGFFParsPtr->deltaIntensityFlag) {
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
PrintPars_ClickGainFreqFuncs(void)
{
	static const char	*funcName = "PrintPars_ClickGainFreqFuncs";

	if (!CheckPars_ClickGainFreqFuncs()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("?? ClickGainFreqFuncs Module Parameters:-\n");
	DPrint("\tenabled = %s \n", BooleanList_NSpecLists(
	  cGFFParsPtr->enabled)->name);
	DPrint("\toutputMode = %s \n", cGFFParsPtr->outputModeList[
	  cGFFParsPtr->outputMode].name);
	DPrint("\tclickMode = %s \n", cGFFParsPtr->clickModeList[
	  cGFFParsPtr->clickMode].name);
	DPrint("\tanalysisOutputMode = %s \n", cGFFParsPtr->analysisOutputModeList[
	  cGFFParsPtr->analysisOutputMode].name);
	DPrint("\tclickTime = %g ??\n", cGFFParsPtr->clickTime);
	DPrint("\tstimulusDuration = %g ??\n", cGFFParsPtr->stimulusDuration);
	DPrint("\tsamplingInterval = %g ??\n", cGFFParsPtr->dt);
	DPrint("\tdBAdjustment = %g ??\n", cGFFParsPtr->dBAdjustment);
	DPrint("\tnumIntensities = %d ??\n", cGFFParsPtr->numIntensities);
	DPrint("\tinitialIntensity = %g ??\n", cGFFParsPtr->initialIntensity);
	DPrint("\tdeltaIntensity = %g ??\n", cGFFParsPtr->deltaIntensity);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_ClickGainFreqFuncs(FILE *fp)
{
	static const char	*funcName = "ReadPars_ClickGainFreqFuncs";
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
	if (!SetPars_ClickGainFreqFuncs(enabled, outputMode, clickMode,
	  analysisOutputMode, clickTime, dBAdjustment, stimulusDuration,
	  samplingInterval, numIntensities, initialIntensity, deltaIntensity)) {
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
CheckData_ClickGainFreqFuncs(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_ClickGainFreqFuncs";

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
InitProcessVariables_ClickGainFreqFuncs(EarObjectPtr data)
{
	static const char * funcName = "InitProcessVariables_ClickGainFreqFuncs";
	DatumPtr	pc;
	CGFFParsPtr	p = cGFFParsPtr;

	p->audModel = data;
	if ((p->stimulus = Init_EarObject((p->clickMode ==
	  CLICKGAINFREQFUNCS_CLICKMODE_SIMPLE)? STIM_CLICK_PROCESS_NAME:
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

	ConnectOutSignalToIn_EarObject(p->stimulus, p->audModel);
	ConnectOutSignalToIn_EarObject(p->stimulus, p->setLevel);
	ConnectOutSignalToIn_EarObject(p->audModel, p->analysis);

	switch (p->clickMode) {
	case CLICKGAINFREQFUNCS_CLICKMODE_SIMPLE:
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
	if (!SetRealPar_ModuleMgr(p->setLevel, "Offset", 0.0)) {
		NotifyError("%s: Cannot set the set level time offset.", funcName);
		return(FALSE);
	}
	if (!SetPar_ModuleMgr(p->analysis, "output_mode", "modulus")) {
		NotifyError("%s: Cannot set the analysis output mode.", funcName);
		return(FALSE);
	}
	if (p->analysisOutputMode ==
	  CLICKGAINFREQFUNCS_ANALYSISOUTPUTMODE_RELATIVE) {
		if ((p->stimulusAnalysis = Init_EarObject(ANA_FOURIERT_PROCESS_NAME)) ==
		  NULL) {
			NotifyError("%s: Could not initialise the stimulus analysis "
			  "EarObject.", funcName);
			return(FALSE);
		}
		ConnectOutSignalToIn_EarObject(p->stimulus, p->stimulusAnalysis);
		if (!SetPar_ModuleMgr(p->stimulusAnalysis, "output_mode", "modulus")) {
			NotifyError("%s: Cannot set the stimulusanalysis output mode.",
			  funcName);
			return(FALSE);
		}
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
	return(TRUE);

}

/****************************** FreeProcessVariables **************************/

/*
 * This routine unconnects and frees the process variables.
 */

void
FreeProcessVariables_ClickGainFreqFuncs(void)
{
	if (cGFFParsPtr->stimulus && cGFFParsPtr->audModel)
		DisconnectOutSignalFromIn_EarObject(cGFFParsPtr->stimulus,
		  cGFFParsPtr->audModel);
	if (cGFFParsPtr->stimulus && cGFFParsPtr->stimulusAnalysis)
		DisconnectOutSignalFromIn_EarObject(cGFFParsPtr->stimulus,
		  cGFFParsPtr->stimulusAnalysis);
	if (cGFFParsPtr->audModel && cGFFParsPtr->analysis)
		DisconnectOutSignalFromIn_EarObject(cGFFParsPtr->audModel,
		  cGFFParsPtr->analysis);
	Free_EarObject(&cGFFParsPtr->stimulus);
	Free_EarObject(&cGFFParsPtr->setLevel);
	Free_EarObject(&cGFFParsPtr->analysis);
	Free_EarObject(&cGFFParsPtr->stimulusAnalysis);
	Free_EarObject(&cGFFParsPtr->resultEarObj);
	if (cGFFParsPtr->gate) {
		Enable_ModuleMgr(cGFFParsPtr->gate, TRUE);
		cGFFParsPtr->gate = NULL;
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
RunProcess_ClickGainFreqFuncs(EarObjectPtr data)
{
	static const char	*funcName = "RunProcess_ClickGainFreqFuncs";
	register ChanData	*stimFTPtr, *modFTPtr, *outPtr;
	int		i;
	ChanLen	j;
	double	intensity;
	CGFFParsPtr	p = cGFFParsPtr;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckPars_ClickGainFreqFuncs())
		return(FALSE);
	if (!CheckData_ClickGainFreqFuncs(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	if (!InitProcessVariables_ClickGainFreqFuncs(data)) {
		NotifyError("%s: Could not initialise the process variables.",
		  funcName);
		FreeProcessVariables_ClickGainFreqFuncs();
		return(FALSE);
	}
	for (i = 0, intensity = p->initialIntensity; i < p->numIntensities; i++,
	  intensity += p->deltaIntensity) {
		if (!SetRealPar_ModuleMgr(p->setLevel, "dBSPL", intensity +
		  p->dBAdjustment)) {
			NotifyError("%s: Cannot set the level intensity.", funcName);
			FreeProcessVariables_ClickGainFreqFuncs();
			return(FALSE);
		}
		if (!RunProcess_ModuleMgr(p->stimulus)) {
			NotifyError("%s: Failed to run stimulus process.", funcName);
			FreeProcessVariables_ClickGainFreqFuncs();
			return(FALSE);
		}
		if (!RunProcess_ModuleMgr(p->setLevel)) {
			NotifyError("%s: Failed to run the set-level process.", funcName);
			FreeProcessVariables_ClickGainFreqFuncs();
			return(FALSE);
		}
		if (p->analysisOutputMode ==
		  CLICKGAINFREQFUNCS_ANALYSISOUTPUTMODE_RELATIVE) {
			if (!RunProcess_ModuleMgr(p->stimulusAnalysis)) {
				NotifyError("%s: Failed to run stimulus analysis process.",
				  funcName);
				FreeProcessVariables_ClickGainFreqFuncs();
				return(FALSE);
			}
		}
		if (!RunProcess_ModuleMgr(p->audModel)) {
			NotifyError("%s: Failed to run auditory model.", funcName);
			FreeProcessVariables_ClickGainFreqFuncs();
			return(FALSE);
		}
		if (!RunProcess_ModuleMgr(p->analysis)) {
			NotifyError("%s: Failed to run analysis process.", funcName);
			FreeProcessVariables_ClickGainFreqFuncs();
			return(FALSE);
		}
		modFTPtr = p->analysis->outSignal->channel[0];
		outPtr = p->resultEarObj->outSignal->channel[i];
		if (p->analysisOutputMode ==
		  CLICKGAINFREQFUNCS_ANALYSISOUTPUTMODE_RELATIVE) {
			stimFTPtr = p->stimulusAnalysis->outSignal->channel[0];
			for (j = 0; j < p->resultEarObj->outSignal->length; j++)
				*outPtr++ = *modFTPtr++ / *stimFTPtr++;
		} else {
			for (j = 0; j < p->resultEarObj->outSignal->length; j++)
				*outPtr++ = *modFTPtr++;
		}
		SetInfoChannelLabel_SignalData(p->resultEarObj->outSignal, i,
		  intensity);
	}
	SetSamplingInterval_SignalData(p->resultEarObj->outSignal,
	  p->analysis->outSignal->dt);
	OutputResults_Utils(p->resultEarObj, &p->outputModeList[p->outputMode],
	  UTILS_NORMAL_SIGNAL);
	FreeProcessVariables_ClickGainFreqFuncs();
	return(TRUE);

}

