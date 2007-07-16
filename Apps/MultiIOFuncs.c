/**********************
 *
 * File:		MultiIOFuncs.c
 * Purpose:		This program calculates the I/O functions for a simulation and
 *				different stimulus frequencies.
 * Comments:	Written using ModuleProducer version 1.2.7 (Jun 26 2000).
 * Author:		L. P. O'Mard
 * Created:		03 Jul 2000
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

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "GeNSpecLists.h"
#include "FiParFile.h"
#include "UtAppInterface.h"
#include "Utils.h"
#include "MultiIOFuncs.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

MIOParsPtr	mIOParsPtr = NULL;

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
Free_MultiIOFuncs(void)
{
	/* static const WChar	*funcName = wxT("Free_MultiIOFuncs");  */

	if (mIOParsPtr == NULL)
		return(FALSE);
	if (mIOParsPtr->parSpec == GLOBAL) {
		free(mIOParsPtr);
		mIOParsPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitAnalysisProcessList ***********************/

/*
 * This function initialises the 'analysisProcess' list array
 */

BOOLN
InitAnalysisProcessList_MultiIOFuncs(void)
{
	static NameSpecifier	modeList[] = {

			{ wxT("Ana_Intensity"),	MULTIIOFUNCS_ANALYSIS_PROCESS_INTENSITY },
			{ wxT("Ana_FindBin"),	MULTIIOFUNCS_ANALYSIS_PROCESS_FINDBIN },
			{ NULL,					MULTIIOFUNCS_ANALYSIS_PROCESS_NULL },
		};
	mIOParsPtr->analysisProcessList = modeList;
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
Init_MultiIOFuncs(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_MultiIOFuncs");

	if (parSpec == GLOBAL) {
		if (mIOParsPtr != NULL)
			Free_MultiIOFuncs();
		if ((mIOParsPtr = (MIOParsPtr) malloc(sizeof(MIOPars))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"), funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (mIOParsPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	mIOParsPtr->parSpec = parSpec;
	mIOParsPtr->enabledFlag = TRUE;
	mIOParsPtr->outputModeFlag = TRUE;
	mIOParsPtr->analysisProcessFlag = TRUE;
	mIOParsPtr->numFrequenciesFlag = TRUE;
	mIOParsPtr->stimulusDurationFlag = TRUE;
	mIOParsPtr->dtFlag = TRUE;
	mIOParsPtr->initialFrequencyFlag = TRUE;
	mIOParsPtr->deltaFrequencyFlag = TRUE;
	mIOParsPtr->numIntensitiesFlag = TRUE;
	mIOParsPtr->initialIntensityFlag = TRUE;
	mIOParsPtr->deltaIntensityFlag = TRUE;
	mIOParsPtr->enabled = GENERAL_BOOLEAN_OFF;
	mIOParsPtr->outputMode = 0;
	mIOParsPtr->analysisProcess = MULTIIOFUNCS_ANALYSIS_PROCESS_FINDBIN;
	mIOParsPtr->numFrequencies = 1;
	mIOParsPtr->stimulusDuration = 0.1;
	mIOParsPtr->dt = 0.05e-3;
	mIOParsPtr->initialFrequency = 1100.0;
	mIOParsPtr->deltaFrequency = 0.0;
	mIOParsPtr->numIntensities = 10;
	mIOParsPtr->initialIntensity = 10.0;
	mIOParsPtr->deltaIntensity = 10.0;

	DSAM_strcpy(mIOParsPtr->outputFile, NO_FILE);
	InitAnalysisProcessList_MultiIOFuncs();
	if ((mIOParsPtr->outputModeList = InitNameList_NSpecLists(
	  DiagModeList_NSpecLists(0), mIOParsPtr->outputFile)) == NULL)
		return(FALSE);
	if (!SetUniParList_MultiIOFuncs()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_MultiIOFuncs();
		return(FALSE);
	}
	mIOParsPtr->audModel = NULL;
	mIOParsPtr->stimulus = NULL;
	mIOParsPtr->analysis = NULL;
	mIOParsPtr->resultEarObj = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_MultiIOFuncs(void)
{
	static const WChar *funcName = wxT("SetUniParList_MultiIOFuncs");
	UniParPtr	pars;

	if (mIOParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((mIOParsPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  MULTIIOFUNCS_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = mIOParsPtr->parList->pars;
	SetPar_UniParMgr(&pars[MULTIIOFUNCS_ENABLED], wxT("ENABLED"),
	  wxT("Multi I/O function test state 'on' or 'off'."),
	  UNIPAR_BOOL,
	  &mIOParsPtr->enabled, NULL,
	  (void * (*)) SetEnabled_MultiIOFuncs);
	SetPar_UniParMgr(&pars[MULTIIOFUNCS_OUTPUTMODE], wxT("OUTPUT_MODE"),
	  wxT("I/O function test output 'off', 'screen' or <file name>."),
	  UNIPAR_NAME_SPEC_WITH_FILE,
	  &mIOParsPtr->outputMode, mIOParsPtr->outputModeList,
	  (void * (*)) SetOutputMode_MultiIOFuncs);
	SetPar_UniParMgr(&pars[MULTIIOFUNCS_ANALYSISPROCESS], wxT("ANALYSIS"),
	  wxT("Analysis process module."),
	  UNIPAR_NAME_SPEC,
	  &mIOParsPtr->analysisProcess, mIOParsPtr->analysisProcessList,
	  (void * (*)) SetAnalysisProcess_MultiIOFuncs);
	SetPar_UniParMgr(&pars[MULTIIOFUNCS_NUMFREQUENCIES], wxT("NUM_FREQ"),
	  wxT("No. of test frequencies."),
	  UNIPAR_INT,
	  &mIOParsPtr->numFrequencies, NULL,
	  (void * (*)) SetNumFrequencies_MultiIOFuncs);
	SetPar_UniParMgr(&pars[MULTIIOFUNCS_STIMULUSDURATION], wxT("STIM_DURATION"),
	  wxT("Stimulus duration (s)."),
	  UNIPAR_REAL,
	  &mIOParsPtr->stimulusDuration, NULL,
	  (void * (*)) SetStimulusDuration_MultiIOFuncs);
	SetPar_UniParMgr(&pars[MULTIIOFUNCS_SAMPLINGINTERVAL], wxT("DT"),
	  wxT("Stimulus Sampling interval, dt (s)."),
	  UNIPAR_REAL,
	  &mIOParsPtr->dt, NULL,
	  (void * (*)) SetSamplingInterval_MultiIOFuncs);
	SetPar_UniParMgr(&pars[MULTIIOFUNCS_INITIALFREQUENCY], wxT("INIT_FREQ"),
	  wxT("Initial test frequency (Hz)."),
	  UNIPAR_REAL,
	  &mIOParsPtr->initialFrequency, NULL,
	  (void * (*)) SetInitialFrequency_MultiIOFuncs);
	SetPar_UniParMgr(&pars[MULTIIOFUNCS_DELTAFREQUENCY], wxT("DELTA_FREQ"),
	  wxT("Frequency increment (Hz)."),
	  UNIPAR_REAL,
	  &mIOParsPtr->deltaFrequency, NULL,
	  (void * (*)) SetDeltaFrequency_MultiIOFuncs);
	SetPar_UniParMgr(&pars[MULTIIOFUNCS_NUMINTENSITIES], wxT("NUM_INTENSTIES"),
	  wxT("No. of test intensities."),
	  UNIPAR_INT,
	  &mIOParsPtr->numIntensities, NULL,
	  (void * (*)) SetNumIntensities_MultiIOFuncs);
	SetPar_UniParMgr(&pars[MULTIIOFUNCS_INITIALINTENSITY], wxT("INIT_INTENSITY"),
	  wxT("Initial test intensity (dB SPL)."),
	  UNIPAR_REAL,
	  &mIOParsPtr->initialIntensity, NULL,
	  (void * (*)) SetInitialIntensity_MultiIOFuncs);
	SetPar_UniParMgr(&pars[MULTIIOFUNCS_DELTAINTENSITY], wxT("DELTA_INTENSITY"),
	  wxT("Intensity increment (dB)."),
	  UNIPAR_REAL,
	  &mIOParsPtr->deltaIntensity, NULL,
	  (void * (*)) SetDeltaIntensity_MultiIOFuncs);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_MultiIOFuncs(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_MultiIOFuncs");

	if (mIOParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (mIOParsPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(mIOParsPtr->parList);

}

/****************************** SetEnabled ************************************/

/*
 * This function sets the module's enabled parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetEnabled_MultiIOFuncs(WChar * theEnabled)
{
	static const WChar	*funcName = wxT("SetEnabled_MultiIOFuncs");
	int		specifier;

	if (mIOParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theEnabled, BooleanList_NSpecLists(
	  0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theEnabled);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	mIOParsPtr->enabledFlag = TRUE;
	mIOParsPtr->enabled = specifier;
	return(TRUE);

}

/****************************** SetOutputMode *********************************/

/*
 * This function sets the module's outputMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOutputMode_MultiIOFuncs(WChar * theOutputMode)
{
	static const WChar	*funcName = wxT("SetOutputMode_MultiIOFuncs");

	if (mIOParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	mIOParsPtr->outputMode = IdentifyDiag_NSpecLists(theOutputMode,
	  mIOParsPtr->outputModeList);
	mIOParsPtr->outputModeFlag = TRUE;
	return(TRUE);

}

/****************************** SetAnalysisProcess ****************************/

/*
 * This function sets the module's analysisProcess parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetAnalysisProcess_MultiIOFuncs(WChar * theAnalysisProcess)
{
	static const WChar	*funcName = wxT("SetAnalysisProcess_MultiIOFuncs");
	int		specifier;

	if (mIOParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theAnalysisProcess,
		mIOParsPtr->analysisProcessList)) ==
		  MULTIIOFUNCS_ANALYSIS_PROCESS_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theAnalysisProcess);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	mIOParsPtr->analysisProcessFlag = TRUE;
	mIOParsPtr->analysisProcess = specifier;
	return(TRUE);

}

/****************************** SetNumFrequencies *****************************/

/*
 * This function sets the module's numFrequencies parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNumFrequencies_MultiIOFuncs(int theNumFrequencies)
{
	static const WChar	*funcName = wxT("SetNumFrequencies_MultiIOFuncs");

	if (mIOParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	mIOParsPtr->numFrequenciesFlag = TRUE;
	mIOParsPtr->numFrequencies = theNumFrequencies;
	return(TRUE);

}

/****************************** SetStimulusDuration ***************************/

/*
 * This function sets the module's stimulusDuration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetStimulusDuration_MultiIOFuncs(double theStimulusDuration)
{
	static const WChar	*funcName = wxT("SetStimulusDuration_MultiIOFuncs");

	if (mIOParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	mIOParsPtr->stimulusDurationFlag = TRUE;
	mIOParsPtr->stimulusDuration = theStimulusDuration;
	return(TRUE);

}

/****************************** SetSamplingInterval ***************************/

/*
 * This function sets the module's samplingInterval parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSamplingInterval_MultiIOFuncs(double theSamplingInterval)
{
	static const WChar	*funcName = wxT("SetSamplingInterval_MultiIOFuncs");

	if (mIOParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	mIOParsPtr->dtFlag = TRUE;
	mIOParsPtr->dt = theSamplingInterval;
	return(TRUE);

}

/****************************** SetInitialFrequency ***************************/

/*
 * This function sets the module's initialFrequency parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetInitialFrequency_MultiIOFuncs(double theInitialFrequency)
{
	static const WChar	*funcName = wxT("SetInitialFrequency_MultiIOFuncs");

	if (mIOParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	mIOParsPtr->initialFrequencyFlag = TRUE;
	mIOParsPtr->initialFrequency = theInitialFrequency;
	return(TRUE);

}

/****************************** SetDeltaFrequency *****************************/

/*
 * This function sets the module's deltaFrequency parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDeltaFrequency_MultiIOFuncs(double theDeltaFrequency)
{
	static const WChar	*funcName = wxT("SetDeltaFrequency_MultiIOFuncs");

	if (mIOParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	mIOParsPtr->deltaFrequencyFlag = TRUE;
	mIOParsPtr->deltaFrequency = theDeltaFrequency;
	return(TRUE);

}

/****************************** SetNumIntensities *****************************/

/*
 * This function sets the module's numIntensities parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNumIntensities_MultiIOFuncs(int theNumIntensities)
{
	static const WChar	*funcName = wxT("SetNumIntensities_MultiIOFuncs");

	if (mIOParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	mIOParsPtr->numIntensitiesFlag = TRUE;
	mIOParsPtr->numIntensities = theNumIntensities;
	return(TRUE);

}

/****************************** SetInitialIntensity ***************************/

/*
 * This function sets the module's initialIntensity parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetInitialIntensity_MultiIOFuncs(double theInitialIntensity)
{
	static const WChar	*funcName = wxT("SetInitialIntensity_MultiIOFuncs");

	if (mIOParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	mIOParsPtr->initialIntensityFlag = TRUE;
	mIOParsPtr->initialIntensity = theInitialIntensity;
	return(TRUE);

}

/****************************** SetDeltaIntensity *****************************/

/*
 * This function sets the module's deltaIntensity parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDeltaIntensity_MultiIOFuncs(double theDeltaIntensity)
{
	static const WChar	*funcName = wxT("SetDeltaIntensity_MultiIOFuncs");

	if (mIOParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	mIOParsPtr->deltaIntensityFlag = TRUE;
	mIOParsPtr->deltaIntensity = theDeltaIntensity;
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
CheckPars_MultiIOFuncs(void)
{
	static const WChar	*funcName = wxT("CheckPars_MultiIOFuncs");
	BOOLN	ok;

	ok = TRUE;
	if (mIOParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (!mIOParsPtr->enabledFlag) {
		NotifyError(wxT("%s: enabled variable not set."), funcName);
		ok = FALSE;
	}
	if (!mIOParsPtr->outputModeFlag) {
		NotifyError(wxT("%s: outputMode variable not set."), funcName);
		ok = FALSE;
	}
	if (!mIOParsPtr->analysisProcessFlag) {
		NotifyError(wxT("%s: analysisProcess variable not set."), funcName);
		ok = FALSE;
	}
	if (!mIOParsPtr->numFrequenciesFlag) {
		NotifyError(wxT("%s: numFrequencies variable not set."), funcName);
		ok = FALSE;
	}
	if (!mIOParsPtr->stimulusDurationFlag) {
		NotifyError(wxT("%s: stimulusDuration variable not set."), funcName);
		ok = FALSE;
	}
	if (!mIOParsPtr->dtFlag) {
		NotifyError(wxT("%s: dt variable not set."), funcName);
		ok = FALSE;
	}
	if (!mIOParsPtr->initialFrequencyFlag) {
		NotifyError(wxT("%s: initialFrequency variable not set."), funcName);
		ok = FALSE;
	}
	if (!mIOParsPtr->deltaFrequencyFlag) {
		NotifyError(wxT("%s: deltaFrequency variable not set."), funcName);
		ok = FALSE;
	}
	if (!mIOParsPtr->numIntensitiesFlag) {
		NotifyError(wxT("%s: numIntensities variable not set."), funcName);
		ok = FALSE;
	}
	if (!mIOParsPtr->initialIntensityFlag) {
		NotifyError(wxT("%s: initialIntensity variable not set."), funcName);
		ok = FALSE;
	}
	if (!mIOParsPtr->deltaIntensityFlag) {
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
PrintPars_MultiIOFuncs(void)
{
	static const WChar	*funcName = wxT("PrintPars_MultiIOFuncs");

	if (!CheckPars_MultiIOFuncs()) {
		NotifyError(wxT("%s: Parameters have not been correctly set."), funcName);
		return(FALSE);
	}
	DPrint(wxT("?? MultiIOFuncs Module Parameters:-\n"));
	DPrint(wxT("\tenabled = %s \n"),
	  BooleanList_NSpecLists(mIOParsPtr->enabled)->name);
	DPrint(wxT("\toutputMode = %s \n"),
	  mIOParsPtr->outputModeList[mIOParsPtr->outputMode].name);
	DPrint(wxT("\tanalysisProcess = %s \n"),
	  mIOParsPtr->analysisProcessList[mIOParsPtr->analysisProcess].name);
	DPrint(wxT("\tnumFrequencies = %d ??\n"), mIOParsPtr->numFrequencies);
	DPrint(wxT("\tstimulusDuration = %g ??\n"), mIOParsPtr->stimulusDuration);
	DPrint(wxT("\tsamplingInterval = %g ??\n"), mIOParsPtr->dt);
	DPrint(wxT("\tinitialFrequency = %g ??\n"), mIOParsPtr->initialFrequency);
	DPrint(wxT("\tdeltaFrequency = %g ??\n"), mIOParsPtr->deltaFrequency);
	DPrint(wxT("\tnumIntensities = %d ??\n"), mIOParsPtr->numIntensities);
	DPrint(wxT("\tinitialIntensity = %g ??\n"), mIOParsPtr->initialIntensity);
	DPrint(wxT("\tdeltaIntensity = %g ??\n"), mIOParsPtr->deltaIntensity);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_MultiIOFuncs(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_MultiIOFuncs");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	mIOParsPtr = (MIOParsPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 * It also initialises the process structure.
 */

BOOLN
InitModule_MultiIOFuncs(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_MultiIOFuncs");

	if (!SetParsPointer_MultiIOFuncs(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_MultiIOFuncs(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = mIOParsPtr;
	theModule->CheckPars = CheckPars_MultiIOFuncs;
	theModule->Free = Free_MultiIOFuncs;
	/*theModule->GetData = NormalSignalStyle_MultiIOFuncs;*/
	theModule->GetUniParListPtr = GetUniParListPtr_MultiIOFuncs;
	theModule->PrintPars = PrintPars_MultiIOFuncs;
	theModule->RunProcess = RunProcess_MultiIOFuncs;
	theModule->SetParsPointer = SetParsPointer_MultiIOFuncs;
	return(TRUE);

}

/****************************** InitProcessVariables **************************/

/*
 * This function initialises the process variables, making connections to the
 * simulation process as required.
 * It returns FALSE if it fails in any way.
 */

BOOLN
InitProcessVariables_MultiIOFuncs(EarObjectPtr data)
{
	static const WChar * funcName = wxT("InitProcessVariables_MultiIOFuncs");
	double	timeOffset;
	UniParPtr	par;

	mIOParsPtr->audModel = GetSimProcess_AppInterface();
	if ((mIOParsPtr->stimulus = Init_EarObject(STIM_PURETONE_PROCESS_NAME)) ==
	  NULL) {
		NotifyError(wxT("%s: Could not initialise the stimulus EarObject."),
		  funcName);
		return(FALSE);
	}
	if ((mIOParsPtr->analysis = Init_EarObject(mIOParsPtr->analysisProcessList[
	  mIOParsPtr->analysisProcess].name)) == NULL) {
		NotifyError(wxT("%s: Could not initialise the analysis EarObject."),
		  funcName);
		return(FALSE);
	}

	ConnectOutSignalToIn_EarObject(mIOParsPtr->stimulus, mIOParsPtr->audModel);
	ConnectOutSignalToIn_EarObject(mIOParsPtr->audModel, mIOParsPtr->analysis);

	if (!SetRealPar_ModuleMgr(mIOParsPtr->stimulus, wxT("duration"),
	  mIOParsPtr->stimulusDuration)) {
		NotifyError(wxT("%s: Cannot set the stimulus duration."), funcName);
		return(FALSE);
	}
	if (!SetRealPar_ModuleMgr(mIOParsPtr->stimulus, wxT("dt"), mIOParsPtr->dt)) {
		NotifyError(wxT("%s: Cannot set the stimulus sampling interval."), funcName);
		return(FALSE);
	}

	if (mIOParsPtr->analysisProcess == MULTIIOFUNCS_ANALYSIS_PROCESS_FINDBIN) {
		if (!SetPar_ModuleMgr(mIOParsPtr->analysis, wxT("mode"),
		  wxT("max_value"))) {
			NotifyError(wxT("%s: Cannot set the analysis max_value."), funcName);
			return(FALSE);
		}
		if (!SetRealPar_ModuleMgr(mIOParsPtr->analysis, wxT("bin_width"), -1.0)) {
			NotifyError(wxT("%s: Cannot set the analysis bin_width."), funcName);
			return(FALSE);
		}
		if (!SetRealPar_ModuleMgr(mIOParsPtr->analysis, wxT("width"), -1.0)) {
			NotifyError(wxT("%s: Cannot set the search width."), funcName);
			return(FALSE);
		}
		timeOffset = mIOParsPtr->stimulusDuration / 2.0;
	} else
		timeOffset = ((par = GetUniParPtr_ModuleMgr(mIOParsPtr->audModel,
		  wxT("duration.Trans_gate"))) == NULL)? 0.0: *par->valuePtr.r;

	if (!SetRealPar_ModuleMgr(mIOParsPtr->analysis, wxT("Offset"), timeOffset)) {
		NotifyError(wxT("%s: Cannot set the analysis offset."), funcName);
		return(FALSE);
	}

	if ((mIOParsPtr->resultEarObj = InitResultEarObject_Utils(
	  mIOParsPtr->numFrequencies, mIOParsPtr->numIntensities,
	  mIOParsPtr->initialIntensity, mIOParsPtr->deltaIntensity, funcName)) ==
	  NULL) {
		NotifyError(wxT("%s: Could not initialise results EarObject"), funcName);
		return(FALSE);
	}
	return(TRUE);

}

/****************************** FreeProcessVariables **************************/

/*
 * This routine unconnects and frees the process variables.
 */

void
FreeProcessVariables_MultiIOFuncs(void)
{
	if (mIOParsPtr->stimulus && mIOParsPtr->audModel)
		DisconnectOutSignalFromIn_EarObject(mIOParsPtr->stimulus,
		  mIOParsPtr->audModel);
	if (mIOParsPtr->audModel && mIOParsPtr->analysis)
		DisconnectOutSignalFromIn_EarObject(mIOParsPtr->audModel,
		  mIOParsPtr->analysis);
	Free_EarObject(&mIOParsPtr->stimulus);
	Free_EarObject(&mIOParsPtr->analysis);
	Free_EarObject(&mIOParsPtr->resultEarObj);
	
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
RunProcess_MultiIOFuncs(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("RunProcess_MultiIOFuncs");
	int		i, j;
	double	frequency, intensity;

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckPars_MultiIOFuncs())
		return(FALSE);
	if (!InitProcessVariables_MultiIOFuncs(data)) {
		NotifyError(wxT("%s: Could not initialise the process variables."),
		  funcName);
		FreeProcessVariables_MultiIOFuncs();
		return(FALSE);
	}
	for (i = 0, frequency = mIOParsPtr->initialFrequency; i < 
	  mIOParsPtr->numFrequencies; i++, frequency +=
	  mIOParsPtr->deltaFrequency) {
		if (!SetRealPar_ModuleMgr(mIOParsPtr->stimulus, wxT("frequency"),
		  frequency)) {
			NotifyError(wxT("%s: Cannot set the stimulus frequency."), funcName);
			FreeProcessVariables_MultiIOFuncs();
			return(FALSE);
		}
		for (j = 0, intensity = mIOParsPtr->initialIntensity; j <
		  mIOParsPtr->numIntensities; j++, intensity +=
		  mIOParsPtr->deltaIntensity) {
			if (!SetRealPar_ModuleMgr(mIOParsPtr->stimulus, wxT("intensity"),
			  intensity)) {
				NotifyError(wxT("%s: Cannot set the stimulus intensity."), funcName);
				FreeProcessVariables_MultiIOFuncs();
				return(FALSE);
			}
			if (!RunProcess_ModuleMgr(mIOParsPtr->stimulus)) {
				NotifyError(wxT("%s: Failed to run stimulus process."), funcName);
				FreeProcessVariables_MultiIOFuncs();
				return(FALSE);
			}
			if (!RunProcess_ModuleMgr(mIOParsPtr->audModel)) {
				NotifyError(wxT("%s: Failed to run auditory model."), funcName);
				FreeProcessVariables_MultiIOFuncs();
				return(FALSE);
			}
			if (!RunProcess_ModuleMgr(mIOParsPtr->analysis)) {
				NotifyError(wxT("%s: Failed to run analysis process."), funcName);
				FreeProcessVariables_MultiIOFuncs();
				return(FALSE);
			}
			mIOParsPtr->resultEarObj->outSignal->channel[i][j] =
			  GetResult_EarObject(mIOParsPtr->analysis, CHANNEL);

		}
		SetInfoChannelLabel_SignalData(mIOParsPtr->resultEarObj->outSignal, i,
		  frequency);
	}
	OutputResults_Utils(mIOParsPtr->resultEarObj, &mIOParsPtr->outputModeList[
	  mIOParsPtr->outputMode], UTILS_NORMAL_SIGNAL);
	FreeProcessVariables_MultiIOFuncs();
	return(TRUE);

}

