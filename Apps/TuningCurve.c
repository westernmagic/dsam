/**********************
 *
 * File:		TuningCurve.c
 * Purpose:		This module runs a tuning curve test on a simulation.
 * Comments:	It was created to be used with the feval program.
 * Author:		L. P. O'Mard
 * Created:		28 Jun 2000
 * Updated:		
 * Copyright:	(c) 2000, University of Essex
 *
 *********************/

#include <stdlib.h> 
#include <stdio.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "GeNSpecLists.h"
#include "FiParFile.h"
#include "UtAppInterface.h"
#include "AnExperiment.h"
#include "Utils.h"
#include "TuningCurve.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

TCParsPtr	tCParsPtr = NULL;

/******************************************************************************/
/****************************** Functions and subroutines *********************/
/******************************************************************************/

/****************************** Free ******************************************/

/*
 * This function releases of the memory allocated for the process
 * variables. It should be called when the module is no longer in use.
 * It is defined as returning a BOOLN value because the generic
 * module interface requires that a non-void value be returned.
 */

BOOLN
Free_TuningCurve(void)
{
	/* static const WChar	*funcName = wxT("Free_TuningCurve");  */

	if (tCParsPtr == NULL)
		return(FALSE);
	if (tCParsPtr->parList)
		FreeList_UniParMgr(&tCParsPtr->parList);
	if (tCParsPtr->parSpec == GLOBAL) {
		free(tCParsPtr);
		tCParsPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitAnalysisProcessList ***********************/

/*
 * This function initialises the 'analysisProcess' list array
 */

BOOLN
InitAnalysisProcessList_TuningCurve(void)
{
	static NameSpecifier	modeList[] = {

			{ wxT("Ana_Intensity"),	TUNINGCURVE_ANALYSIS_PROCESS_INTENSITY },
			{ wxT("Ana_FindBin"),	TUNINGCURVE_ANALYSIS_PROCESS_FINDBIN },
			{ NULL,					TUNINGCURVE_ANALYSIS_PROCESS_NULL },
		};
	tCParsPtr->analysisProcessList = modeList;
	return(TRUE);

}

/****************************** InitThresholdModeList *************************/

/*
 * This function initialises the 'thresholdMode' list array
 */

BOOLN
InitThresholdModeList_TuningCurve(void)
{
	static NameSpecifier	modeList[] = {

			{  wxT("GRADIENT"),		TUNINGCURVE_THRESHOLDMODE_GRADIENT },
			{  wxT("STEP_DIVIDE"),	TUNINGCURVE_THRESHOLDMODE_STEP_DIVIDE },
			{  wxT("STEP_DIVIDE2"),	TUNINGCURVE_THRESHOLDMODE_STEP_DIVIDE2 },
			{  NULL,				TUNINGCURVE_THRESHOLDMODE_NULL },
		};
	tCParsPtr->thresholdModeList = modeList;
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
Init_TuningCurve(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_TuningCurve");

	if (parSpec == GLOBAL) {
		if (tCParsPtr != NULL)
			Free_TuningCurve();
		if ((tCParsPtr = (TCParsPtr) malloc(sizeof(TCPars))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"), funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (tCParsPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	tCParsPtr->parSpec = parSpec;
	tCParsPtr->enabledFlag = TRUE;
	tCParsPtr->outputModeFlag = TRUE;
	tCParsPtr->analysisProcessFlag = TRUE;
	tCParsPtr->thresholdModeFlag = TRUE;
	tCParsPtr->numberOfFreqsFlag = TRUE;
	tCParsPtr->stimulusDurationFlag = TRUE;
	tCParsPtr->lowestFrequencyFlag = TRUE;
	tCParsPtr->highestFrequencyFlag = TRUE;
	tCParsPtr->minimumFreqFlag = TRUE;
	tCParsPtr->targetThresholdFlag = TRUE;
	tCParsPtr->accuracyFlag = TRUE;
	tCParsPtr->enabled = GENERAL_BOOLEAN_OFF;
	tCParsPtr->outputMode = 0;
	tCParsPtr->analysisProcess = TUNINGCURVE_ANALYSIS_PROCESS_FINDBIN;
	tCParsPtr->thresholdMode = TUNINGCURVE_THRESHOLDMODE_GRADIENT;
	tCParsPtr->numberOfFreqs = 40;
	tCParsPtr->stimulusDuration = 100.0e-3;
	tCParsPtr->lowestFrequency = 200.0;
	tCParsPtr->highestFrequency = 20000.0;
	tCParsPtr->minimumFreq = 5000.0;
	tCParsPtr->targetThreshold = 5e-5;
	tCParsPtr->accuracy = 2e-5;

	DSAM_strcpy(tCParsPtr->outputFile, NO_FILE);
	InitAnalysisProcessList_TuningCurve();
	InitThresholdModeList_TuningCurve();
	if ((tCParsPtr->outputModeList = InitNameList_NSpecLists(
	  DiagModeList_NSpecLists(0), tCParsPtr->outputFile)) == NULL)
		return(FALSE);
	if (!SetUniParList_TuningCurve()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_TuningCurve();
		return(FALSE);
	}
	tCParsPtr->frequencies = NULL;
	tCParsPtr->audModel = NULL;
	tCParsPtr->stimulus = NULL;
	tCParsPtr->analysis = NULL;
	tCParsPtr->resultEarObj = NULL;
	
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * entries in the main list.
 * It assumes that the array has been passed at the correct offset.
 */

BOOLN
SetUniParList_TuningCurve(void)
{
	static const WChar *funcName = wxT("SetUniParList_TuningCurve");
	UniParPtr	pars;

	if (tCParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((tCParsPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  TUNINGCURVE_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = tCParsPtr->parList->pars;
	SetPar_UniParMgr(&pars[TUNINGCURVE_ENABLED], wxT("ENABLED"),
	  wxT("Tuning curve test state 'on' or 'off'."),
	  UNIPAR_BOOL,
	  &tCParsPtr->enabled, NULL,
	  (void * (*)) SetEnabled_TuningCurve);
	SetPar_UniParMgr(&pars[TUNINGCURVE_OUTPUTMODE], wxT("OUTPUT_MODE"),
	  wxT("Tuning curve test output 'off', 'screen' or <file name>."),
	  UNIPAR_NAME_SPEC_WITH_FILE,
	  &tCParsPtr->outputMode, tCParsPtr->outputModeList,
	  (void * (*)) SetOutputMode_TuningCurve);
	SetPar_UniParMgr(&pars[TUNINGCURVE_ANALYSIS_PROCESS], wxT("ANALYSIS"),
	  wxT("Analysis process module."),
	  UNIPAR_NAME_SPEC,
	  &tCParsPtr->analysisProcess, tCParsPtr->analysisProcessList,
	  (void * (*)) SetAnalysisProcess_TuningCurve);
	SetPar_UniParMgr(&pars[TUNINGCURVE_THRESHOLDMODE], wxT("THRESHOLD_MODE"),
	  wxT("Threshold determination mode."),
	  UNIPAR_NAME_SPEC,
	  &tCParsPtr->thresholdMode, tCParsPtr->thresholdModeList,
	  (void * (*)) SetThresholdMode_TuningCurve);
	SetPar_UniParMgr(&pars[TUNINGCURVE_NUMBEROFFREQS], wxT("NUM_FREQS"),
	  wxT("No. of frequencies within range (logarithmic)."),
	  UNIPAR_INT,
	  &tCParsPtr->numberOfFreqs, NULL,
	  (void * (*)) SetNumberOfFreqs_TuningCurve);
	SetPar_UniParMgr(&pars[TUNINGCURVE_STIMULUSDURATION], wxT("STIM_DURATION"),
	  wxT("Stimulus duration (s)."),
	  UNIPAR_REAL,
	  &tCParsPtr->stimulusDuration, NULL,
	  (void * (*)) SetStimulusDuration_TuningCurve);
	SetPar_UniParMgr(&pars[TUNINGCURVE_LOWESTFREQUENCY], wxT("LOWEST_FREQ"),
	  wxT("Lowest frequency for pure tone probe stimulus (Hz)."),
	  UNIPAR_REAL,
	  &tCParsPtr->lowestFrequency, NULL,
	  (void * (*)) SetLowestFrequency_TuningCurve);
	SetPar_UniParMgr(&pars[TUNINGCURVE_HIGHESTFREQUENCY], wxT("HIGHEST_FREQ"),
	  wxT("Highest frequency for pure tone probe stimulus (Hz)."),
	  UNIPAR_REAL,
	  &tCParsPtr->highestFrequency, NULL,
	  (void * (*)) SetHighestFrequency_TuningCurve);
	SetPar_UniParMgr(&pars[TUNINGCURVE_MINIMUMFREQ], wxT("MINIMUM_FREQ"),
	  wxT("Minimum frequency for sampling interval setting (Hz)."),
	  UNIPAR_REAL,
	  &tCParsPtr->minimumFreq, NULL,
	  (void * (*)) SetMinimumFreq_TuningCurve);
	SetPar_UniParMgr(&pars[TUNINGCURVE_TARGETTHRESHOLD], wxT("TARGETTHRESHOLD"),
	  wxT("Target threshold (?)."),
	  UNIPAR_REAL,
	  &tCParsPtr->targetThreshold, NULL,
	  (void * (*)) SetTargetThreshold_TuningCurve);
	SetPar_UniParMgr(&pars[TUNINGCURVE_ACCURACY], wxT("ACCURACY"),
	  wxT("Target accuracy (?)."),
	  UNIPAR_REAL,
	  &tCParsPtr->accuracy, NULL,
	  (void * (*)) SetAccuracy_TuningCurve);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_TuningCurve(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_TuningCurve");

	if (tCParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (tCParsPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(tCParsPtr->parList);

}

/****************************** SetEnabled ************************************/

/*
 * This function sets the module's enabled parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetEnabled_TuningCurve(WChar * theEnabled)
{
	static const WChar	*funcName = wxT("SetEnabled_TuningCurve");
	int		specifier;

	if (tCParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theEnabled, BooleanList_NSpecLists(
	  0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theEnabled);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	tCParsPtr->enabledFlag = TRUE;
	tCParsPtr->enabled = specifier;
	return(TRUE);

}

/****************************** SetOutputMode *********************************/

/*
 * This function sets the module's outputMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOutputMode_TuningCurve(WChar * theOutputMode)
{
	static const WChar	*funcName = wxT("SetOutputMode_TuningCurve");

	if (tCParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	tCParsPtr->outputMode = IdentifyDiag_NSpecLists(theOutputMode,
	  tCParsPtr->outputModeList);
	tCParsPtr->outputModeFlag = TRUE;
	return(TRUE);

}

/****************************** SetAnalysisProcess ****************************/

/*
 * This function sets the module's analysisProcess parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetAnalysisProcess_TuningCurve(WChar * theAnalysisProcess)
{
	static const WChar	*funcName = wxT("SetAnalysisProcess_TuningCurve");
	int		specifier;

	if (tCParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theAnalysisProcess,
		tCParsPtr->analysisProcessList)) == TUNINGCURVE_ANALYSIS_PROCESS_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theAnalysisProcess);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	tCParsPtr->analysisProcessFlag = TRUE;
	tCParsPtr->analysisProcess = specifier;
	return(TRUE);

}

/****************************** SetThresholdMode ******************************/

/*
 * This function sets the module's thresholdMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetThresholdMode_TuningCurve(WChar * theThresholdMode)
{
	 static const WChar	*funcName = wxT("SetThresholdMode_TuningCurve");
	int		specifier;

	if (tCParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theThresholdMode,
	  tCParsPtr->thresholdModeList)) == TUNINGCURVE_THRESHOLDMODE_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theThresholdMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	tCParsPtr->thresholdModeFlag = TRUE;
	tCParsPtr->thresholdMode = specifier;
	return(TRUE);

}

/****************************** SetNumberOfFreqs ******************************/

/*
 * This function sets the module's numberOfFreqs parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNumberOfFreqs_TuningCurve(int theNumberOfFreqs)
{
	static const WChar	*funcName = wxT("SetNumberOfFreqs_TuningCurve");

	if (tCParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	tCParsPtr->numberOfFreqsFlag = TRUE;
	tCParsPtr->numberOfFreqs = theNumberOfFreqs;
	return(TRUE);

}

/****************************** SetStimulusDuration ***************************/

/*
 * This function sets the module's stimulusDuration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetStimulusDuration_TuningCurve(double theStimulusDuration)
{
	static const WChar	*funcName = wxT("SetStimulusDuration_TuningCurve");

	if (tCParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	tCParsPtr->stimulusDurationFlag = TRUE;
	tCParsPtr->stimulusDuration = theStimulusDuration;
	return(TRUE);

}

/****************************** SetLowestFrequency ****************************/

/*
 * This function sets the module's lowestFrequency parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLowestFrequency_TuningCurve(double theLowestFrequency)
{
	static const WChar	*funcName = wxT("SetLowestFrequency_TuningCurve");

	if (tCParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	tCParsPtr->lowestFrequencyFlag = TRUE;
	tCParsPtr->lowestFrequency = theLowestFrequency;
	return(TRUE);

}

/****************************** SetHighestFrequency ***************************/

/*
 * This function sets the module's highestFrequency parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetHighestFrequency_TuningCurve(double theHighestFrequency)
{
	static const WChar	*funcName = wxT("SetHighestFrequency_TuningCurve");

	if (tCParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	tCParsPtr->highestFrequencyFlag = TRUE;
	tCParsPtr->highestFrequency = theHighestFrequency;
	return(TRUE);

}

/****************************** SetMinimumFreq ********************************/

/*
 * This function sets the module's minimumFreq parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMinimumFreq_TuningCurve(double theMinimumFreq)
{
	static const WChar	*funcName = wxT("SetMinimumFreq_TuningCurve");

	if (tCParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	tCParsPtr->minimumFreqFlag = TRUE;
	tCParsPtr->minimumFreq = theMinimumFreq;
	return(TRUE);

}

/****************************** SetTargetThreshold ****************************/

/*
 * This function sets the module's targetThreshold parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTargetThreshold_TuningCurve(double theTargetThreshold)
{
	static const WChar	*funcName = wxT("SetTargetThreshold_TuningCurve");

	if (tCParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	tCParsPtr->targetThresholdFlag = TRUE;
	tCParsPtr->targetThreshold = theTargetThreshold;
	return(TRUE);

}

/****************************** SetAccuracy ***********************************/

/*
 * This function sets the module's accuracy parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetAccuracy_TuningCurve(double theAccuracy)
{
	static const WChar	*funcName = wxT("SetAccuracy_TuningCurve");

	if (tCParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	tCParsPtr->accuracyFlag = TRUE;
	tCParsPtr->accuracy = theAccuracy;
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
CheckPars_TuningCurve(void)
{
	static const WChar	*funcName = wxT("CheckPars_TuningCurve");
	BOOLN	ok;

	ok = TRUE;
	if (tCParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (!tCParsPtr->enabledFlag) {
		NotifyError(wxT("%s: enabled variable not set."), funcName);
		ok = FALSE;
	}
	if (!tCParsPtr->outputModeFlag) {
		NotifyError(wxT("%s: outputMode variable not set."), funcName);
		ok = FALSE;
	}
	if (!tCParsPtr->analysisProcessFlag) {
		NotifyError(wxT("%s: analysisProcess variable not set."), funcName);
		ok = FALSE;
	}
	if (!tCParsPtr->thresholdModeFlag) {
		NotifyError(wxT("%s: thresholdMode variable not set."), funcName);
		ok = FALSE;
	}
	if (!tCParsPtr->numberOfFreqsFlag) {
		NotifyError(wxT("%s: numberOfFreqs variable not set."), funcName);
		ok = FALSE;
	}
	if (!tCParsPtr->stimulusDurationFlag) {
		NotifyError(wxT("%s: stimulusDuration variable not set."), funcName);
		ok = FALSE;
	}
	if (!tCParsPtr->lowestFrequencyFlag) {
		NotifyError(wxT("%s: lowestFrequency variable not set."), funcName);
		ok = FALSE;
	}
	if (!tCParsPtr->highestFrequencyFlag) {
		NotifyError(wxT("%s: highestFrequency variable not set."), funcName);
		ok = FALSE;
	}
	if (!tCParsPtr->minimumFreqFlag) {
		NotifyError(wxT("%s: minimumFreq variable not set."), funcName);
		ok = FALSE;
	}
	if (!tCParsPtr->targetThresholdFlag) {
		NotifyError(wxT("%s: targetThreshold variable not set."), funcName);
		ok = FALSE;
	}
	if (!tCParsPtr->accuracyFlag) {
		NotifyError(wxT("%s: accuracy variable not set."), funcName);
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
PrintPars_TuningCurve(void)
{
	static const WChar	*funcName = wxT("PrintPars_TuningCurve");

	if (!CheckPars_TuningCurve()) {
		NotifyError(wxT("%s: Parameters have not been correctly set."),
		  funcName);
		return(FALSE);
	}
	DPrint(wxT("?? TuningCurve Module Parameters:-\n"));
	DPrint(wxT("\tenabled = %s \n"), BooleanList_NSpecLists(tCParsPtr->
	  enabled)->name);
	DPrint(wxT("\toutputMode = %s \n"), tCParsPtr->outputModeList[tCParsPtr->
	  outputMode].name);
	DPrint(wxT("\tanalysisProcess = %s \n"), tCParsPtr->analysisProcessList[
	  tCParsPtr->analysisProcess].name);
	DPrint(wxT("\tthresholdMode = %s \n"), tCParsPtr->thresholdModeList[
	  tCParsPtr->thresholdMode].name);
	DPrint(wxT("\tnumberOfFreqs = %d ??\n"), tCParsPtr->numberOfFreqs);
	DPrint(wxT("\tstimulusDuration = %g ??\n"), tCParsPtr->stimulusDuration);
	DPrint(wxT("\tlowestFrequency = %g ??\n"), tCParsPtr->lowestFrequency);
	DPrint(wxT("\thighestFrequency = %g ??\n"), tCParsPtr->highestFrequency);
	DPrint(wxT("\tminimumFreq = %g ??\n"), tCParsPtr->minimumFreq);
	DPrint(wxT("\ttargetThreshold = %g ??\n"), tCParsPtr->targetThreshold);
	DPrint(wxT("\taccuracy = %g ??\n"), tCParsPtr->accuracy);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_TuningCurve(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_TuningCurve");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	tCParsPtr = (TCParsPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 * It also initialises the process structure.
 */

BOOLN
InitModule_TuningCurve(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_TuningCurve");

	if (!SetParsPointer_TuningCurve(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_TuningCurve(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = tCParsPtr;
	theModule->CheckPars = CheckPars_TuningCurve;
	theModule->Free = Free_TuningCurve;
	/*theModule->GetData = NormalSignalStyle_TuningCurve;*/
	theModule->GetUniParListPtr = GetUniParListPtr_TuningCurve;
	theModule->PrintPars = PrintPars_TuningCurve;
	theModule->RunProcess = RunProcess_TuningCurve;
	theModule->SetParsPointer = SetParsPointer_TuningCurve;
	return(TRUE);

}

/****************************** InitProcessVariables **************************/

/*
 * This function initialises the process variables, making connections to the
 * simulation process as required.
 * It returns FALSE if it fails in any way.
 */

BOOLN
InitProcessVariables_TuningCurve(EarObjectPtr data)
{
	static const WChar * funcName = wxT("InitProcessVariables_TuningCurve");
	double	timeOffset;
	UniParPtr	par;
	CFListPtr	theBMCFs = NULL;

	tCParsPtr->audModel = GetSimProcess_AppInterface();
	if ((tCParsPtr->stimulus = Init_EarObject(STIM_PURETONE_PROCESS_NAME)) ==
	  NULL) {
		NotifyError(wxT("%s: Could not initialise the stimulus EarObject."),
		  funcName);
		return(FALSE);
	}
	if ((tCParsPtr->analysis = Init_EarObject(tCParsPtr->analysisProcessList[
	  tCParsPtr->analysisProcess].name)) == NULL) {
		NotifyError(wxT("%s: Could not initialise the analysis EarObject."),
		  funcName);
		return(FALSE);
	}

	ConnectOutSignalToIn_EarObject(tCParsPtr->stimulus, tCParsPtr->audModel);
	ConnectOutSignalToIn_EarObject(tCParsPtr->audModel, tCParsPtr->analysis);

	if (!SetRealPar_ModuleMgr(tCParsPtr->stimulus, wxT("duration"),
	  tCParsPtr->stimulusDuration)) {
		NotifyError(wxT("%s: Cannot set the stimulus duration."), funcName);
		return(FALSE);
	}

	timeOffset = ((par = GetUniParPtr_ModuleMgr(tCParsPtr->audModel,
	  wxT("duration.Trans_Ramp"))) == NULL)? 0.0: *par->valuePtr.r;
	if (!SetRealPar_ModuleMgr(tCParsPtr->analysis, wxT("Offset"), timeOffset)) {
		NotifyError(wxT("%s: Cannot set the analysis offset."), funcName);
		return(FALSE);
	}
	if (tCParsPtr->analysisProcess == TUNINGCURVE_ANALYSIS_PROCESS_FINDBIN) {
		if (!SetPar_ModuleMgr(tCParsPtr->analysis, wxT("mode"), wxT(
		  "max_value"))) {
			NotifyError(wxT("%s: Cannot set the analysis max_value."),
			  funcName);
			return(FALSE);
		}
		if (!SetRealPar_ModuleMgr(tCParsPtr->analysis, wxT("bin_width"),
		  -1.0)) {
			NotifyError(wxT("%s: Cannot set the analysis bin_width."),
			  funcName);
			return(FALSE);
		}
		if (!SetRealPar_ModuleMgr(tCParsPtr->analysis, wxT("width"), -1.0)) {
			NotifyError(wxT("%s: Cannot set the analysis window width."),
			  funcName);
			return(FALSE);
		}
	}

	if ((par = GetUniParPtr_ModuleMgr(tCParsPtr->audModel, wxT("cflist"))) !=
	   NULL) {
		theBMCFs = *par->valuePtr.cFPtr;
		if ((tCParsPtr->frequencies = GenerateList_CFList(wxT("focal_log"),
		  wxT("parameters"), tCParsPtr->numberOfFreqs, tCParsPtr->
		  lowestFrequency, tCParsPtr->highestFrequency, theBMCFs->frequency[
		  CHANNEL], 0.0, NULL)) == NULL)
			return(FALSE);
	} else {
		if ((tCParsPtr->frequencies = GenerateList_CFList(wxT("log"),
		  wxT("parameters"), tCParsPtr->numberOfFreqs, tCParsPtr->
		  lowestFrequency, tCParsPtr->highestFrequency, 0.0, 0.0, NULL)) ==
		  NULL)
			return(FALSE);
	}
	if ((tCParsPtr->resultEarObj = InitResultEarObject_Utils(2,
	  tCParsPtr->frequencies->numChannels, 0.0, 1.0, funcName)) == NULL) {
		NotifyError(wxT("%s: Could not initialise results EarObject"),
		  funcName);
		return(FALSE);
	}
	if (par)
		tCParsPtr->resultEarObj->outSignal->info.chanLabel[1] =
		  theBMCFs->frequency[CHANNEL];
	SetInfoSampleTitle_SignalData(tCParsPtr->resultEarObj->outSignal,
	  wxT("Frequency (Hz)"));

	return(TRUE);

}

/****************************** FreeProcessVariables **************************/

/*
 * This routine unconnects and frees the process variables.
 */

void
FreeProcessVariables_TuningCurve(void)
{
	Free_CFList(&tCParsPtr->frequencies);
	if (tCParsPtr->stimulus && tCParsPtr->audModel)
		DisconnectOutSignalFromIn_EarObject(tCParsPtr->stimulus,
		  tCParsPtr->audModel);
	if (tCParsPtr->audModel && tCParsPtr->analysis)
		DisconnectOutSignalFromIn_EarObject(tCParsPtr->audModel,
		  tCParsPtr->analysis);
	Free_EarObject(&tCParsPtr->stimulus);
	Free_EarObject(&tCParsPtr->analysis);
	Free_EarObject(&tCParsPtr->resultEarObj);
	
}

/****************************** SetIntensity **********************************/

/*
 * This function sets the intensity for a simulation.
 */

BOOLN
SetIntensity_TuningCurve(double intensity)
{
	static WChar * funcName = wxT("SetIntensity_TuningCurve");
	
	if (!SetRealPar_ModuleMgr(tCParsPtr->stimulus, wxT("intensity"),
	  intensity)) {
		NotifyError(wxT("%s: Cannot set the intensity parameter."), funcName);
		return(FALSE);
	}
	return(TRUE);

}

/****************************** RunSimulation *********************************/

/*
 * This routine runs the 'data' simulation.
 * It returns NULL if it fails in any way, otherwise it returns a pointer
 * to the results EarObjectPtr.
 */

EarObjectPtr
RunSimulation_TuningCurve(void)
{
	static const WChar * funcName = wxT("RunSimulation_TuningCurve");

	if (!RunProcess_ModuleMgr(tCParsPtr->stimulus)) {
		NotifyError(wxT("%s: Failed to run stimulus process."), funcName);
		return(NULL);
	}
	if (!RunProcess_ModuleMgr(tCParsPtr->audModel)) {
		NotifyError(wxT("%s: Failed to run auditory model."), funcName);
		return(NULL);
	}
	if (!RunProcess_ModuleMgr(tCParsPtr->analysis)) {
		NotifyError(wxT("%s: Failed to run analysis process."), funcName);
		return(NULL);
	}
	return(tCParsPtr->analysis);

}

/****************************** FindThresholdIntensity ************************/

/*
 * This general routine calculates the threshold intensity of model, starting
 * from an initial 'guess' intensity.
 * It returns the respective intensity that produces the threshold intensity.
 * It works only on a single channel.
 * This routine now expects the intensity calculation to be done at the end of
 * the "RunModel" simulation routine.
 */
 
#define FIRST_INTENSITY_GUESS		100.0
#define	MIN_DELTA_INPUT				10.0
#define ANALYSIS_CHANNEL			0
#define TC_MAXIMUM_THRESHOLD_LOOP	30
 
double
FindThresholdIntensity_TuningCurve(EarObjectPtr (* RunModel)(void),
  BOOLN (* SetIntensity)(double), double targetThreshold,
  double targetAccuracy, BOOLN *firstPass)
{
	static const WChar *funcName = wxT("FindThresholdIntensity_TuningCurve");
	static double	lastInputIntensity, presentOutIntensity;
	int		i;
	double	intensityDiff, presentInputIntensity, lastOutIntensity;
	double	absoluteIntensityDiff, bestIntensityDiff = 0.0, lastIntensityDiff;
	double	bestOutputIntensity = 0.0, bestInputIntensity = 0.0, deltaInput;
	EarObjectPtr	modelResp;
	
	if (targetAccuracy <= 0.0) {
		NotifyWarning(wxT("FindThresholdIntensity_ExpAnalysis: Illegal target ")
		  wxT("threshold accuracy (= %g)."), targetAccuracy);
		return(0.0);
	}
	if (*firstPass) {
		*firstPass = FALSE;
		lastInputIntensity = FIRST_INTENSITY_GUESS;
		presentInputIntensity = lastInputIntensity - MIN_DELTA_INPUT;
		if (!(* SetIntensity)(lastInputIntensity)) {
			NotifyWarning(wxT("FindThresholdIntensity_ExpAnalysis: Intensity ")
			  wxT("not set."));
			return(0.0);
		}
	}
	if ((modelResp = (* RunModel)()) == NULL) {
		NotifyWarning(wxT("FindThresholdIntensity_ExpAnalysis: Could not ")
		  wxT("calculate present intensity (first pass).  Returning ")
		  wxT("zero."));
		return(0.0);
	}
	presentOutIntensity = GetResult_EarObject(modelResp, ANALYSIS_CHANNEL);
	deltaInput = -MIN_DELTA_INPUT;
	for (i = 0, bestIntensityDiff = HUGE_VAL; i < TC_MAXIMUM_THRESHOLD_LOOP;
	  i++) {
		lastIntensityDiff = targetThreshold - presentOutIntensity;
		lastOutIntensity = presentOutIntensity;
		if (!(* SetIntensity)(presentInputIntensity)) {
			NotifyWarning(wxT("FindThresholdIntensity_ExpAnalysis: Could not ")
			  wxT("set intensity."));
			return(0.0);
		}
		if ((modelResp = (* RunModel)()) == NULL) {
			NotifyWarning(wxT("FindThresholdIntensity_ExpAnalysis: Could not ")
			  wxT("calculate present output intensity.  Returning zero."));
			return(0.0);
		}
		presentOutIntensity = GetResult_EarObject(modelResp, ANALYSIS_CHANNEL);
		intensityDiff = targetThreshold - presentOutIntensity;
		absoluteIntensityDiff = fabs(intensityDiff);
		if (absoluteIntensityDiff <= targetAccuracy)
			return(presentInputIntensity);
		if (absoluteIntensityDiff < bestIntensityDiff) {
			bestIntensityDiff = absoluteIntensityDiff;
			bestInputIntensity = presentInputIntensity;
			bestOutputIntensity = presentOutIntensity;
		}
		lastInputIntensity = presentInputIntensity;
		if ((lastIntensityDiff * intensityDiff) < 0.0)
			deltaInput /= -2.0;
		else if (fabs(lastIntensityDiff) < absoluteIntensityDiff)
			deltaInput *= -1.0;
		presentInputIntensity += deltaInput;
		
	}
	NotifyWarning(wxT("%s: Looped %d times and couldn't\n\tcome any closer to ")
	  wxT("the threshold than %g units.\n"), funcName,
	  TC_MAXIMUM_THRESHOLD_LOOP, bestIntensityDiff);
	lastInputIntensity = bestInputIntensity;
	presentOutIntensity = bestOutputIntensity;
	return(bestInputIntensity);

}

/****************************** RunProcess ************************************/

/*
 * This routine runs the filter shape test for a simulation.
 * It operates on the global 'data' EarObject.
 * It returns NULL if it fails in any way, otherwise it returns a pointer
 * to the results EarObjectPtr.
 */

BOOLN
RunProcess_TuningCurve(EarObjectPtr data)
{
	static const WChar * funcName = wxT("RunProcess_TuningCurve");
	BOOLN	firstPass;
	int		i;
	double	samplingFrequency, intensity;

	if (!data) {
		NotifyError(wxT("%s: Simulation EarObject not Initialised."), funcName);
		return(FALSE);
	}
	if (!InitProcessVariables_TuningCurve(data)) {
		NotifyError(wxT("%s: Could not initialise the process variables."),
		  funcName);
		FreeProcessVariables_TuningCurve();
		return(FALSE);
	}
	
	for (i = 0, firstPass = TRUE; i < tCParsPtr->frequencies->numChannels;
	  i++) {
		if (!SetRealPar_ModuleMgr(tCParsPtr->stimulus, wxT("frequency"),
		  tCParsPtr->frequencies->frequency[i])) {
			NotifyError(wxT("%s: Cannot set the frequency parameter."),
			  funcName);
			FreeProcessVariables_TuningCurve();
			return(FALSE);
		}
		/* Set the sampling interval to min for Freq. */
		samplingFrequency = (tCParsPtr->frequencies->frequency[i] >=
		  tCParsPtr->minimumFreq)? tCParsPtr->frequencies->frequency[i]:
		  tCParsPtr->minimumFreq;
		if (!SetRealPar_ModuleMgr(tCParsPtr->stimulus, wxT("dt"), 1.0 /
		  (SAMPLING_FREQ_MULTIPLIER * samplingFrequency))) {
			NotifyError(wxT("%s: Cannot set the frequency parameter."),
			  funcName);
			FreeProcessVariables_TuningCurve();
			return(FALSE);
		}
		switch (tCParsPtr->thresholdMode) {
		case TUNINGCURVE_THRESHOLDMODE_GRADIENT:
			intensity = FindThresholdIntensity_ExpAnalysis(
			  RunSimulation_TuningCurve, SetIntensity_TuningCurve,
			  tCParsPtr->targetThreshold, tCParsPtr->accuracy, &firstPass);
			break;
		case TUNINGCURVE_THRESHOLDMODE_STEP_DIVIDE:
			intensity = FindThresholdIntensity_ExpAnalysis_Slow(
			  RunSimulation_TuningCurve, SetIntensity_TuningCurve,
			  tCParsPtr->targetThreshold, tCParsPtr->accuracy,
			  TUNINGCURVE_INITIAL_STEP);
			break;
		case TUNINGCURVE_THRESHOLDMODE_STEP_DIVIDE2:
			intensity = FindThresholdIntensity_TuningCurve(
			  RunSimulation_TuningCurve, SetIntensity_TuningCurve,
			  tCParsPtr->targetThreshold, tCParsPtr->accuracy, &firstPass);
			break;
		default:
			;
		}
		tCParsPtr->resultEarObj->outSignal->channel[0][i] =
		  tCParsPtr->frequencies->frequency[i];
		tCParsPtr->resultEarObj->outSignal->channel[1][i] = intensity;
		
	}
	OutputResults_Utils(tCParsPtr->resultEarObj, &tCParsPtr->outputModeList[
	  tCParsPtr->outputMode], UTILS_NO_TIME_COLUMN);
	FreeProcessVariables_TuningCurve();
	return(TRUE);

}
