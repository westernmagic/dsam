/**********************
 *
 * File:		FilterShape.c
 * Purpose:		
 * Comments:	Written using ModuleProducer version 1.2.8 (Jul 13 2000).
 * Author:		
 * Created:		13 Jul 2000
 * Updated:	
 * Copyright:	(c) 2000, 
 *
 *********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG */

#include <stdio.h>
#include <stdlib.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "GeNSpecLists.h"
#include "FiParFile.h"
#include "UtAppInterface.h"
#include "Utils.h"
#include "FilterShape.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

FSParsPtr	fSParsPtr = NULL;

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
Free_FilterShape(void)
{
	/* static const WChar	*funcName = wxT("Free_FilterShape");  */

	if (fSParsPtr == NULL)
		return(FALSE);
	if (fSParsPtr->parList)
		FreeList_UniParMgr(&fSParsPtr->parList);
	if (fSParsPtr->parSpec == GLOBAL) {
		free(fSParsPtr);
		fSParsPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitAnalysisProcessList ***********************/

/*
 * This function initialises the 'analysisProcess' list array
 */

BOOLN
InitAnalysisProcessList_FilterShape(void)
{
	static NameSpecifier	modeList[] = {

			{ wxT("Ana_Intensity"),	FILTERSHAPE_ANALYSIS_PROCESS_INTENSITY },
			{ wxT("Ana_FindBin"),	FILTERSHAPE_ANALYSIS_PROCESS_FINDBIN },
			{ NULL,					FILTERSHAPE_ANALYSIS_PROCESS_NULL },
		};
	fSParsPtr->analysisProcessList = modeList;
	return(TRUE);

}

/****************************** InitAnalysisOutputModeList ********************/

/*
 * This function initialises the 'analysisOutputMode' list array
 */

BOOLN
InitAnalysisOutputModeList_FilterShape(void)
{
	static NameSpecifier	modeList[] = {

			{ wxT("NORMALISED"),	FILTERSHAPE_NORMALISED_ANALYSISOUTPUTMODE },
			{ wxT("STANDARD"),		FILTERSHAPE_STANDARD_ANALYSISOUTPUTMODE },
			{ NULL,					FILTERSHAPE_ANALYSISOUTPUTMODE_NULL },
		};
	fSParsPtr->analysisOutputModeList = modeList;
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
Init_FilterShape(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_FilterShape");

	if (parSpec == GLOBAL) {
		if (fSParsPtr != NULL)
			Free_FilterShape();
		if ((fSParsPtr = (FSParsPtr) malloc(sizeof(FSPars))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"), funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (fSParsPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	fSParsPtr->parSpec = parSpec;
	fSParsPtr->enabledFlag = TRUE;
	fSParsPtr->outputModeFlag = TRUE;
	fSParsPtr->analysisProcessFlag = TRUE;
	fSParsPtr->analysisOutputModeFlag = TRUE;
	fSParsPtr->numberOfFreqsFlag = TRUE;
	fSParsPtr->stimulusDurationFlag = TRUE;
	fSParsPtr->dtFlag = TRUE;
	fSParsPtr->lowestFrequencyFlag = TRUE;
	fSParsPtr->highestFrequencyFlag = TRUE;
	fSParsPtr->numIntensitiesFlag = TRUE;
	fSParsPtr->initialIntensityFlag = TRUE;
	fSParsPtr->deltaIntensityFlag = TRUE;
	fSParsPtr->enabled = GENERAL_BOOLEAN_OFF;
	fSParsPtr->outputMode = 0;
	fSParsPtr->analysisProcess = FILTERSHAPE_ANALYSIS_PROCESS_INTENSITY;
	fSParsPtr->analysisOutputMode = FILTERSHAPE_STANDARD_ANALYSISOUTPUTMODE;
	fSParsPtr->numberOfFreqs = 40;
	fSParsPtr->stimulusDuration = 0.1;
	fSParsPtr->dt = 0.02e-3;
	fSParsPtr->lowestFrequency = 20.0;
	fSParsPtr->highestFrequency = 20.0e-3;
	fSParsPtr->numIntensities = 1;
	fSParsPtr->initialIntensity = 0.0;
	fSParsPtr->deltaIntensity = 20.0;

	DSAM_strcpy(fSParsPtr->outputFile, NO_FILE);
	InitAnalysisProcessList_FilterShape();
	InitAnalysisOutputModeList_FilterShape();
	if ((fSParsPtr->outputModeList = InitNameList_NSpecLists(
	  DiagModeList_NSpecLists(0), fSParsPtr->outputFile)) == NULL)
		return(FALSE);
	if (!SetUniParList_FilterShape()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_FilterShape();
		return(FALSE);
	}
	fSParsPtr->frequencies = NULL;
	fSParsPtr->audModel = NULL;
	fSParsPtr->stimulus = NULL;
	fSParsPtr->analysis = NULL;
	fSParsPtr->resultEarObj = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_FilterShape(void)
{
	static const WChar *funcName = wxT("SetUniParList_FilterShape");
	UniParPtr	pars;

	if (fSParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((fSParsPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  FILTERSHAPE_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = fSParsPtr->parList->pars;
	SetPar_UniParMgr(&pars[FILTERSHAPE_ENABLED], wxT("ENABLED"),
	  wxT("Filter shape test state 'on' or 'off'."),
	  UNIPAR_BOOL,
	  &fSParsPtr->enabled, NULL,
	  (void * (*)) SetEnabled_FilterShape);
	SetPar_UniParMgr(&pars[FILTERSHAPE_OUTPUTMODE], wxT("OUTPUT_MODE"),
	  wxT("Filter shape test output 'off', 'screen' or <file name>."),
	  UNIPAR_NAME_SPEC_WITH_FILE,
	  &fSParsPtr->outputMode, fSParsPtr->outputModeList,
	  (void * (*)) SetOutputMode_FilterShape);
	SetPar_UniParMgr(&pars[FILTERSHAPE_ANALYSISPROCESS], wxT("ANALYSIS"),
	  wxT("Analysis process module."),
	  UNIPAR_NAME_SPEC,
	  &fSParsPtr->analysisProcess, fSParsPtr->analysisProcessList,
	  (void * (*)) SetAnalysisProcess_FilterShape);
	SetPar_UniParMgr(&pars[FILTERSHAPE_ANALYSISOUTPUTMODE], wxT("ANAL_OUTPUT"),
	  wxT("Analysis output mode ('normalised' or 'standard')."),
	  UNIPAR_NAME_SPEC,
	  &fSParsPtr->analysisOutputMode, fSParsPtr->analysisOutputModeList,
	  (void * (*)) SetAnalysisOutputMode_FilterShape);
	SetPar_UniParMgr(&pars[FILTERSHAPE_NUMBEROFFREQS], wxT("NUM_FREQS"),
	  wxT("No. of frequencies within range (logarithmic)."),
	  UNIPAR_INT,
	  &fSParsPtr->numberOfFreqs, NULL,
	  (void * (*)) SetNumberOfFreqs_FilterShape);
	SetPar_UniParMgr(&pars[FILTERSHAPE_STIMULUSDURATION], wxT("STIM_DURATION"),
	  wxT("Stimulus duration (s)."),
	  UNIPAR_REAL,
	  &fSParsPtr->stimulusDuration, NULL,
	  (void * (*)) SetStimulusDuration_FilterShape);
	SetPar_UniParMgr(&pars[FILTERSHAPE_SAMPLINGINTERVAL], wxT("DT"),
	  wxT("Stimulus sampling interval, dt (s)."),
	  UNIPAR_REAL,
	  &fSParsPtr->dt, NULL,
	  (void * (*)) SetSamplingInterval_FilterShape);
	SetPar_UniParMgr(&pars[FILTERSHAPE_LOWESTFREQUENCY], wxT("LOWEST_FREQ"),
	  wxT("Lowest frequency for pure tone probe stimulus (Hz)."),
	  UNIPAR_REAL,
	  &fSParsPtr->lowestFrequency, NULL,
	  (void * (*)) SetLowestFrequency_FilterShape);
	SetPar_UniParMgr(&pars[FILTERSHAPE_HIGHESTFREQUENCY], wxT("HIGHEST_FREQ"),
	  wxT("Highest frequency for pure tone probe stimulus (Hz)."),
	  UNIPAR_REAL,
	  &fSParsPtr->highestFrequency, NULL,
	  (void * (*)) SetHighestFrequency_FilterShape);
	SetPar_UniParMgr(&pars[FILTERSHAPE_NUMINTENSITIES], wxT("NUM_FUNCTIONS"),
	  wxT("Number of filter shape functions (intensities)."),
	  UNIPAR_INT,
	  &fSParsPtr->numIntensities, NULL,
	  (void * (*)) SetNumIntensities_FilterShape);
	SetPar_UniParMgr(&pars[FILTERSHAPE_INITIALINTENSITY],
	  wxT("INITIAL_INTENSITY"),
	  wxT("Initial filter function intensity (dB SPL)."),
	  UNIPAR_REAL,
	  &fSParsPtr->initialIntensity, NULL,
	  (void * (*)) SetInitialIntensity_FilterShape);
	SetPar_UniParMgr(&pars[FILTERSHAPE_DELTAINTENSITY], wxT("DELTA_INTENSITY"),
	  wxT("Intensity change for filter functions (dB)."),
	  UNIPAR_REAL,
	  &fSParsPtr->deltaIntensity, NULL,
	  (void * (*)) SetDeltaIntensity_FilterShape);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_FilterShape(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_FilterShape");

	if (fSParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (fSParsPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(fSParsPtr->parList);

}

/****************************** SetEnabled ************************************/

/*
 * This function sets the module's enabled parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetEnabled_FilterShape(WChar * theEnabled)
{
	static const WChar	*funcName = wxT("SetEnabled_FilterShape");
	int		specifier;

	if (fSParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theEnabled, BooleanList_NSpecLists(
	  0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theEnabled);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	fSParsPtr->enabledFlag = TRUE;
	fSParsPtr->enabled = specifier;
	return(TRUE);

}

/****************************** SetOutputMode *********************************/

/*
 * This function sets the module's outputMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOutputMode_FilterShape(WChar * theOutputMode)
{
	static const WChar	*funcName = wxT("SetOutputMode_FilterShape");

	if (fSParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	fSParsPtr->outputMode = IdentifyDiag_NSpecLists(theOutputMode,
	  fSParsPtr->outputModeList);
	fSParsPtr->outputModeFlag = TRUE;
	return(TRUE);

}

/****************************** SetAnalysisProcess ****************************/

/*
 * This function sets the module's analysisProcess parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetAnalysisProcess_FilterShape(WChar * theAnalysisProcess)
{
	static const WChar	*funcName = wxT("SetAnalysisProcess_FilterShape");
	int		specifier;

	if (fSParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theAnalysisProcess,
		fSParsPtr->analysisProcessList)) == FILTERSHAPE_ANALYSIS_PROCESS_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theAnalysisProcess);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	fSParsPtr->analysisProcessFlag = TRUE;
	fSParsPtr->analysisProcess = specifier;
	return(TRUE);

}

/****************************** SetAnalysisOutputMode *************************/

/*
 * This function sets the module's analysisOutputMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetAnalysisOutputMode_FilterShape(WChar * theAnalysisOutputMode)
{
	static const WChar	*funcName = wxT("SetAnalysisOutputMode_FilterShape");
	int		specifier;

	if (fSParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theAnalysisOutputMode,
		fSParsPtr->analysisOutputModeList)) ==
		  FILTERSHAPE_ANALYSISOUTPUTMODE_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theAnalysisOutputMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	fSParsPtr->analysisOutputModeFlag = TRUE;
	fSParsPtr->analysisOutputMode = specifier;
	return(TRUE);

}

/****************************** SetNumberOfFreqs ******************************/

/*
 * This function sets the module's numberOfFreqs parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNumberOfFreqs_FilterShape(int theNumberOfFreqs)
{
	static const WChar	*funcName = wxT("SetNumberOfFreqs_FilterShape");

	if (fSParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	fSParsPtr->numberOfFreqsFlag = TRUE;
	fSParsPtr->numberOfFreqs = theNumberOfFreqs;
	return(TRUE);

}

/****************************** SetStimulusDuration ***************************/

/*
 * This function sets the module's stimulusDuration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetStimulusDuration_FilterShape(double theStimulusDuration)
{
	static const WChar	*funcName = wxT("SetStimulusDuration_FilterShape");

	if (fSParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	fSParsPtr->stimulusDurationFlag = TRUE;
	fSParsPtr->stimulusDuration = theStimulusDuration;
	return(TRUE);

}

/****************************** SetSamplingInterval ***************************/

/*
 * This function sets the module's samplingInterval parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSamplingInterval_FilterShape(double theSamplingInterval)
{
	static const WChar	*funcName = wxT("SetSamplingInterval_FilterShape");

	if (fSParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	fSParsPtr->dtFlag = TRUE;
	fSParsPtr->dt = theSamplingInterval;
	return(TRUE);

}

/****************************** SetLowestFrequency ****************************/

/*
 * This function sets the module's lowestFrequency parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLowestFrequency_FilterShape(double theLowestFrequency)
{
	static const WChar	*funcName = wxT("SetLowestFrequency_FilterShape");

	if (fSParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	fSParsPtr->lowestFrequencyFlag = TRUE;
	fSParsPtr->lowestFrequency = theLowestFrequency;
	return(TRUE);

}

/****************************** SetHighestFrequency ***************************/

/*
 * This function sets the module's highestFrequency parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetHighestFrequency_FilterShape(double theHighestFrequency)
{
	static const WChar	*funcName = wxT("SetHighestFrequency_FilterShape");

	if (fSParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	fSParsPtr->highestFrequencyFlag = TRUE;
	fSParsPtr->highestFrequency = theHighestFrequency;
	return(TRUE);

}

/****************************** SetNumIntensities *****************************/

/*
 * This function sets the module's numIntensities parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNumIntensities_FilterShape(int theNumIntensities)
{
	static const WChar	*funcName = wxT("SetNumIntensities_FilterShape");

	if (fSParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	fSParsPtr->numIntensitiesFlag = TRUE;
	fSParsPtr->numIntensities = theNumIntensities;
	return(TRUE);

}

/****************************** SetInitialIntensity ***************************/

/*
 * This function sets the module's initialIntensity parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetInitialIntensity_FilterShape(double theInitialIntensity)
{
	static const WChar	*funcName = wxT("SetInitialIntensity_FilterShape");

	if (fSParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	fSParsPtr->initialIntensityFlag = TRUE;
	fSParsPtr->initialIntensity = theInitialIntensity;
	return(TRUE);

}

/****************************** SetDeltaIntensity *****************************/

/*
 * This function sets the module's deltaIntensity parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDeltaIntensity_FilterShape(double theDeltaIntensity)
{
	static const WChar	*funcName = wxT("SetDeltaIntensity_FilterShape");

	if (fSParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	fSParsPtr->deltaIntensityFlag = TRUE;
	fSParsPtr->deltaIntensity = theDeltaIntensity;
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
CheckPars_FilterShape(void)
{
	static const WChar	*funcName = wxT("CheckPars_FilterShape");
	BOOLN	ok;

	ok = TRUE;
	if (fSParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (!fSParsPtr->enabledFlag) {
		NotifyError(wxT("%s: enabled variable not set."), funcName);
		ok = FALSE;
	}
	if (!fSParsPtr->outputModeFlag) {
		NotifyError(wxT("%s: outputMode variable not set."), funcName);
		ok = FALSE;
	}
	if (!fSParsPtr->analysisProcessFlag) {
		NotifyError(wxT("%s: analysisProcess variable not set."), funcName);
		ok = FALSE;
	}
	if (!fSParsPtr->analysisOutputModeFlag) {
		NotifyError(wxT("%s: analysisOutputMode variable not set."), funcName);
		ok = FALSE;
	}
	if (!fSParsPtr->numberOfFreqsFlag) {
		NotifyError(wxT("%s: numberOfFreqs variable not set."), funcName);
		ok = FALSE;
	}
	if (!fSParsPtr->stimulusDurationFlag) {
		NotifyError(wxT("%s: stimulusDuration variable not set."), funcName);
		ok = FALSE;
	}
	if (!fSParsPtr->dtFlag) {
		NotifyError(wxT("%s: dt variable not set."), funcName);
		ok = FALSE;
	}
	if (!fSParsPtr->lowestFrequencyFlag) {
		NotifyError(wxT("%s: lowestFrequency variable not set."), funcName);
		ok = FALSE;
	}
	if (!fSParsPtr->highestFrequencyFlag) {
		NotifyError(wxT("%s: highestFrequency variable not set."), funcName);
		ok = FALSE;
	}
	if (!fSParsPtr->numIntensitiesFlag) {
		NotifyError(wxT("%s: numIntensities variable not set."), funcName);
		ok = FALSE;
	}
	if (!fSParsPtr->initialIntensityFlag) {
		NotifyError(wxT("%s: initialIntensity variable not set."), funcName);
		ok = FALSE;
	}
	if (!fSParsPtr->deltaIntensityFlag) {
		NotifyError(wxT("%s: deltaIntensity variable not set."), funcName);
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
PrintPars_FilterShape(void)
{
	static const WChar	*funcName = wxT("PrintPars_FilterShape");

	if (!CheckPars_FilterShape()) {
		NotifyError(wxT("%s: Parameters have not been correctly set."), funcName);
		return(FALSE);
	}
	DPrint(wxT("?? FilterShape Module Parameters:-\n"));
	DPrint(wxT("\tenabled = %s \n"),
	  BooleanList_NSpecLists(fSParsPtr->enabled)->name);
	DPrint(wxT("\toutputMode = %s \n"),
	  fSParsPtr->outputModeList[fSParsPtr->outputMode].name);
	DPrint(wxT("\tanalysisProcess = %s \n"),
	  fSParsPtr->analysisProcessList[fSParsPtr->analysisProcess].name);
	DPrint(wxT("\tanalysisOutputMode = %s \n"), fSParsPtr->analysisOutputModeList[
	  fSParsPtr->analysisOutputMode].name);
	DPrint(wxT("\tnumberOfFreqs = %d ??\n"), fSParsPtr->numberOfFreqs);
	DPrint(wxT("\tstimulusDuration = %g ??\n"), fSParsPtr->stimulusDuration);
	DPrint(wxT("\tsamplingInterval = %g ??\n"), fSParsPtr->dt);
	DPrint(wxT("\tlowestFrequency = %g ??\n"), fSParsPtr->lowestFrequency);
	DPrint(wxT("\thighestFrequency = %g ??\n"), fSParsPtr->highestFrequency);
	DPrint(wxT("\tnumIntensities = %d ??\n"), fSParsPtr->numIntensities);
	DPrint(wxT("\tinitialIntensity = %g ??\n"), fSParsPtr->initialIntensity);
	DPrint(wxT("\tdeltaIntensity = %g ??\n"), fSParsPtr->deltaIntensity);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_FilterShape(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_FilterShape");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	fSParsPtr = (FSParsPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 * It also initialises the process structure.
 */

BOOLN
InitModule_FilterShape(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_FilterShape");

	if (!SetParsPointer_FilterShape(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_FilterShape(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = fSParsPtr;
	theModule->CheckPars = CheckPars_FilterShape;
	theModule->Free = Free_FilterShape;
	/*theModule->GetData = NormalSignalStyle_FilterShape;*/
	theModule->GetUniParListPtr = GetUniParListPtr_FilterShape;
	theModule->PrintPars = PrintPars_FilterShape;
	theModule->RunProcess = RunProcess_FilterShape;
	theModule->SetParsPointer = SetParsPointer_FilterShape;
	return(TRUE);

}

/****************************** InitProcessVariables **************************/

/*
 * This function initialises the process variables, making connections to the
 * simulation process as required.
 * It returns FALSE if it fails in any way.
 */

BOOLN
InitProcessVariables_FilterShape(EarObjectPtr data)
{
	static const WChar * funcName = wxT("InitProcessVariables_FilterShape");
	double	timeOffset;
	CFListPtr	theBMCFs = NULL;
	UniParPtr	par;
	FSParsPtr	p = fSParsPtr;


	p->audModel = GetSimProcess_AppInterface();
	if ((p->stimulus = Init_EarObject(STIM_PURETONE_PROCESS_NAME)) == NULL) {
		NotifyError(wxT("%s: Could not initialise the stimulus EarObject."),
		  funcName);
		return(FALSE);
	}
	if ((p->analysis = Init_EarObject(p->analysisProcessList[p->
	  analysisProcess].name)) == NULL) {
		NotifyError(wxT("%s: Could not initialise the analysis EarObject."),
		  funcName);
		return(FALSE);
	}

	ConnectOutSignalToIn_EarObject(p->stimulus, p->audModel);
	ConnectOutSignalToIn_EarObject(p->audModel, p->analysis);

	if (!SetRealPar_ModuleMgr(p->stimulus, wxT("duration"), p->
	  stimulusDuration)) {
		NotifyError(wxT("%s: Cannot set the stimulus duration."), funcName);
		return(FALSE);
	}
	if (!SetRealPar_ModuleMgr(p->stimulus, wxT("dt"), p->dt)) {
		NotifyError(wxT("%s: Cannot set the stimulus sampling interval."),
		  funcName);
		return(FALSE);
	}

	timeOffset = ((par = GetUniParPtr_ModuleMgr(p->audModel,
	  wxT("duration.Trans_ramp"))) == NULL)? 0.0: *par->valuePtr.r;
	if (!SetRealPar_ModuleMgr(p->analysis, wxT("Offset"), timeOffset)) {
		NotifyError(wxT("%s: Cannot set the analysis offset."), funcName);
		return(FALSE);
	}
	if (p->analysisProcess == FILTERSHAPE_ANALYSIS_PROCESS_FINDBIN) {
		if (!SetPar_ModuleMgr(p->analysis, wxT("mode"), wxT("max_value"))) {
			NotifyError(wxT("%s: Cannot set the analysis max_value."),
			  funcName);
			return(FALSE);
		}
		if (!SetRealPar_ModuleMgr(p->analysis, wxT("bin_width"), -1.0)) {
			NotifyError(wxT("%s: Cannot set the analysis bin_width."),
			  funcName);
			return(FALSE);
		}
	}

	if ((par = GetUniParPtr_ModuleMgr(p->audModel, wxT("cflist"))) != NULL) {
		theBMCFs = *par->valuePtr.cFPtr;
		if ((p->frequencies = GenerateList_CFList(wxT("focal_log"), wxT(
		  "parameters"), p->numberOfFreqs, p->lowestFrequency, p->
		  highestFrequency, theBMCFs->frequency[CHANNEL], 0.0, NULL)) == NULL)
			return(FALSE);
	} else {
		if ((p->frequencies = GenerateList_CFList(wxT("log"), wxT("parameters"),
		  p->numberOfFreqs, p->lowestFrequency, p->highestFrequency, 0.0, 0.0,
		  NULL)) == NULL)
			return(FALSE);
	}
	if ((p->resultEarObj = InitResultEarObject_Utils(1 + p->numIntensities,
	  p->frequencies->numChannels, 0.0, 1.0, funcName)) == NULL) {
		NotifyError(wxT("%s: Could not initialise results EarObject"),
		  funcName);
		return(FALSE);
	}
	SetInfoSampleTitle_SignalData(p->resultEarObj->outSignal,
	  wxT("Intensity (dB SPL)"));
	if (par)
		p->resultEarObj->outSignal->info.chanLabel[1] = theBMCFs->frequency[
		  CHANNEL];
	return(TRUE);

}

/****************************** FreeProcessVariables **************************/

/*
 * This routine unconnects and frees the process variables.
 */

void
FreeProcessVariables_FilterShape(void)
{
	Free_CFList(&fSParsPtr->frequencies);
	if (fSParsPtr->stimulus && fSParsPtr->audModel)
		DisconnectOutSignalFromIn_EarObject(fSParsPtr->stimulus,
		  fSParsPtr->audModel);
	if (fSParsPtr->audModel && fSParsPtr->analysis)
		DisconnectOutSignalFromIn_EarObject(fSParsPtr->audModel,
		  fSParsPtr->analysis);
	Free_EarObject(&fSParsPtr->stimulus);
	Free_EarObject(&fSParsPtr->analysis);
	Free_EarObject(&fSParsPtr->resultEarObj);
	
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
RunProcess_FilterShape(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("RunProcess_FilterShape");
	int		i, j, plotIndex;
	double	intensity, output, maxOutput;

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!InitProcessVariables_FilterShape(data)) {
		NotifyError(wxT("%s: Could not initialise the process variables."),
		  funcName);
		FreeProcessVariables_FilterShape();
		return(FALSE);
	}
	
	for (i = 0, intensity = fSParsPtr->initialIntensity; i <
	  fSParsPtr->numIntensities; i++, intensity += fSParsPtr->deltaIntensity) {
		plotIndex = i + 1;
		if (!SetRealPar_ModuleMgr(fSParsPtr->stimulus, wxT("intensity"),
		  intensity)) {
			NotifyError(wxT("%s: Cannot set the stimulus intensity."), funcName);
			FreeProcessVariables_FilterShape();
			return(FALSE);
		}
		for (j = 0, maxOutput = -HUGE_VAL; j <
		  fSParsPtr->frequencies->numChannels; j++) {
			if (!SetRealPar_ModuleMgr(fSParsPtr->stimulus, wxT("frequency"),
			  fSParsPtr->frequencies->frequency[j])) {
				NotifyError(wxT("%s: Cannot set the stimulus frequency."), funcName);
				FreeProcessVariables_FilterShape();
				return(FALSE);
			}
			if (!RunProcess_ModuleMgr(fSParsPtr->stimulus)) {
				NotifyError(wxT("%s: Failed to run stimulus process."), funcName);
				FreeProcessVariables_FilterShape();
				return(FALSE);
			}
			if (!RunProcess_ModuleMgr(fSParsPtr->audModel)) {
				NotifyError(wxT("%s: Failed to run auditory model."), funcName);
				FreeProcessVariables_FilterShape();
				return(FALSE);
			}
			if (!RunProcess_ModuleMgr(fSParsPtr->analysis)) {
				NotifyError(wxT("%s: Failed to run analysis process."), funcName);
				FreeProcessVariables_FilterShape();
				return(FALSE);
			}
			output = GetResult_EarObject(fSParsPtr->analysis, CHANNEL);
			fSParsPtr->resultEarObj->outSignal->channel[plotIndex][j] = output;
			if (output > maxOutput)
				maxOutput = output;
			if (i == 0)
				fSParsPtr->resultEarObj->outSignal->channel[0][j] =
				  fSParsPtr->frequencies->frequency[j];
		}
		if (fSParsPtr->analysisOutputMode == 
		  FILTERSHAPE_NORMALISED_ANALYSISOUTPUTMODE)
			for (j = 0; j < fSParsPtr->frequencies->numChannels; j++)
				fSParsPtr->resultEarObj->outSignal->channel[plotIndex][j] /=
				  maxOutput;  
		SetInfoChannelLabel_SignalData(fSParsPtr->resultEarObj->outSignal,
		  plotIndex,intensity);
	}
	OutputResults_Utils(fSParsPtr->resultEarObj, &fSParsPtr->outputModeList[
	  fSParsPtr->outputMode], UTILS_NO_TIME_COLUMN);
	FreeProcessVariables_FilterShape();
	return(TRUE);

}

