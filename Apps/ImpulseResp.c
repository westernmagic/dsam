/**********************
 *
 * File:		ImpulseResp.c
 * Purpose:		This routine generates the impulse response for different
 *				intensities.
 * Comments:	Written using ModuleProducer version 1.2.9 (Aug 10 2000).
 *				This process disables the first gating process that it finds in
 *				the model (ramp).
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
#include "ImpulseResp.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

IRParsPtr	iRParsPtr = NULL;

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
Free_ImpulseResp(void)
{
	/* static const WChar	*funcName = wxT("Free_ImpulseResp");  */

	if (iRParsPtr == NULL)
		return(FALSE);
	if (iRParsPtr->parList)
		FreeList_UniParMgr(&iRParsPtr->parList);
	if (iRParsPtr->parSpec == GLOBAL) {
		free(iRParsPtr);
		iRParsPtr = NULL;
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
Init_ImpulseResp(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_ImpulseResp");

	if (parSpec == GLOBAL) {
		if (iRParsPtr != NULL)
			Free_ImpulseResp();
		if ((iRParsPtr = (IRParsPtr) malloc(sizeof(IRPars))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"), funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (iRParsPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	iRParsPtr->parSpec = parSpec;
	iRParsPtr->enabledFlag = FALSE;
	iRParsPtr->outputModeFlag = FALSE;
	iRParsPtr->clickTimeFlag = FALSE;
	iRParsPtr->clickWidthFlag = FALSE;
	iRParsPtr->refAmplitudeFlag = FALSE;
	iRParsPtr->numIntensitiesFlag = FALSE;
	iRParsPtr->initialIntensityFlag = FALSE;
	iRParsPtr->deltaIntensityFlag = FALSE;
	iRParsPtr->stimulusDurationFlag = FALSE;
	iRParsPtr->dtFlag = FALSE;
	iRParsPtr->enabled = 0;
	iRParsPtr->outputMode = 0;
	iRParsPtr->clickTime = 0.0;
	iRParsPtr->clickWidth = 0.0;
	iRParsPtr->refAmplitude = 0.0;
	iRParsPtr->numIntensities = 0;
	iRParsPtr->initialIntensity = 0.0;
	iRParsPtr->deltaIntensity = 0.0;
	iRParsPtr->stimulusDuration = 0.0;
	iRParsPtr->dt = 0.0;

	DSAM_strcpy(iRParsPtr->outputFile, NO_FILE);
	if ((iRParsPtr->outputModeList = InitNameList_NSpecLists(
	  DiagModeList_NSpecLists(0), iRParsPtr->outputFile)) == NULL)
		return(FALSE);
	if (!SetUniParList_ImpulseResp()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_ImpulseResp();
		return(FALSE);
	}
	iRParsPtr->audModel = NULL;
	iRParsPtr->stimulus = NULL;
	iRParsPtr->gate = NULL;
	iRParsPtr->resultEarObj = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_ImpulseResp(void)
{
	static const WChar *funcName = wxT("SetUniParList_ImpulseResp");
	UniParPtr	pars;

	if (iRParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((iRParsPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  IMPULSERESP_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = iRParsPtr->parList->pars;
	SetPar_UniParMgr(&pars[IMPULSERESP_ENABLED], wxT("ENABLED"),
	  wxT("Click response test state 'on' or 'off'."),
	  UNIPAR_BOOL,
	  &iRParsPtr->enabled, NULL,
	  (void * (*)) SetEnabled_ImpulseResp);
	SetPar_UniParMgr(&pars[IMPULSERESP_OUTPUTMODE], wxT("OUTPUT_MODE"),
	  wxT("Click response test output 'off', 'screen' or <file name>."),
	  UNIPAR_NAME_SPEC_WITH_FILE,
	  &iRParsPtr->outputMode, iRParsPtr->outputModeList,
	  (void * (*)) SetOutputMode_ImpulseResp);
	SetPar_UniParMgr(&pars[IMPULSERESP_CLICKTIME], wxT("CLICK_TIME"),
	  wxT("Click time (s)."),
	  UNIPAR_REAL,
	  &iRParsPtr->clickTime, NULL,
	  (void * (*)) SetClickTime_ImpulseResp);
	SetPar_UniParMgr(&pars[IMPULSERESP_CLICKWIDTH], wxT("CLICK_WIDTH"),
	  wxT("Click width (s)."),
	  UNIPAR_REAL,
	  &iRParsPtr->clickWidth, NULL,
	  (void * (*)) SetClickWidth_ImpulseResp);
	SetPar_UniParMgr(&pars[IMPULSERESP_REFAMPLITUDE], wxT("REF_AMP"),
	  wxT("Reference click amplitude for the first intensity (m/s)."),
	  UNIPAR_REAL,
	  &iRParsPtr->refAmplitude, NULL,
	  (void * (*)) SetRefAmplitude_ImpulseResp);
	SetPar_UniParMgr(&pars[IMPULSERESP_NUMINTENSITIES], wxT("NUM_INTENSITIES"),
	  wxT("Number of click intensities."),
	  UNIPAR_INT,
	  &iRParsPtr->numIntensities, NULL,
	  (void * (*)) SetNumIntensities_ImpulseResp);
	SetPar_UniParMgr(&pars[IMPULSERESP_INITIALINTENSITY], wxT("INIT_INTENSITY"),
	  wxT("Initial click intensity (dB SPL)."),
	  UNIPAR_REAL,
	  &iRParsPtr->initialIntensity, NULL,
	  (void * (*)) SetInitialIntensity_ImpulseResp);
	SetPar_UniParMgr(&pars[IMPULSERESP_DELTAINTENSITY], wxT("DELTA_INTENSITY"),
	  wxT("Click intensity change (dB)."),
	  UNIPAR_REAL,
	  &iRParsPtr->deltaIntensity, NULL,
	  (void * (*)) SetDeltaIntensity_ImpulseResp);
	SetPar_UniParMgr(&pars[IMPULSERESP_STIMULUSDURATION], wxT("DURATION"),
	  wxT("Stimulus duration (s)."),
	  UNIPAR_REAL,
	  &iRParsPtr->stimulusDuration, NULL,
	  (void * (*)) SetStimulusDuration_ImpulseResp);
	SetPar_UniParMgr(&pars[IMPULSERESP_SAMPLINGINTERVAL], wxT("DT"),
	  wxT("Stimulus sampling interval (s)."),
	  UNIPAR_REAL,
	  &iRParsPtr->dt, NULL,
	  (void * (*)) SetSamplingInterval_ImpulseResp);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_ImpulseResp(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_ImpulseResp");

	if (iRParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (iRParsPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(iRParsPtr->parList);

}

/****************************** SetEnabled ************************************/

/*
 * This function sets the module's enabled parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetEnabled_ImpulseResp(WChar * theEnabled)
{
	static const WChar	*funcName = wxT("SetEnabled_ImpulseResp");
	int		specifier;

	if (iRParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theEnabled, BooleanList_NSpecLists(
	  0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theEnabled);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	iRParsPtr->enabledFlag = TRUE;
	iRParsPtr->enabled = specifier;
	return(TRUE);

}

/****************************** SetOutputMode *********************************/

/*
 * This function sets the module's outputMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOutputMode_ImpulseResp(WChar * theOutputMode)
{
	static const WChar	*funcName = wxT("SetOutputMode_ImpulseResp");

	if (iRParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	iRParsPtr->outputMode = IdentifyDiag_NSpecLists(theOutputMode,
	  iRParsPtr->outputModeList);
	iRParsPtr->outputModeFlag = TRUE;
	return(TRUE);

}

/****************************** SetClickTime **********************************/

/*
 * This function sets the module's clickTime parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetClickTime_ImpulseResp(double theClickTime)
{
	static const WChar	*funcName = wxT("SetClickTime_ImpulseResp");

	if (iRParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	iRParsPtr->clickTimeFlag = TRUE;
	iRParsPtr->clickTime = theClickTime;
	return(TRUE);

}

/****************************** SetClickWidth *********************************/

/*
 * This function sets the module's clickWidth parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetClickWidth_ImpulseResp(double theClickWidth)
{
	static const WChar	*funcName = wxT("SetClickWidth_ImpulseResp");

	if (iRParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	iRParsPtr->clickWidthFlag = TRUE;
	iRParsPtr->clickWidth = theClickWidth;
	return(TRUE);

}

/****************************** SetRefAmplitude *******************************/

/*
 * This function sets the module's refAmplitude parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRefAmplitude_ImpulseResp(double theRefAmplitude)
{
	static const WChar	*funcName = wxT("SetRefAmplitude_ImpulseResp");

	if (iRParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	iRParsPtr->refAmplitudeFlag = TRUE;
	iRParsPtr->refAmplitude = theRefAmplitude;
	return(TRUE);

}

/****************************** SetNumIntensities *****************************/

/*
 * This function sets the module's numIntensities parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNumIntensities_ImpulseResp(int theNumIntensities)
{
	static const WChar	*funcName = wxT("SetNumIntensities_ImpulseResp");

	if (iRParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	iRParsPtr->numIntensitiesFlag = TRUE;
	iRParsPtr->numIntensities = theNumIntensities;
	return(TRUE);

}

/****************************** SetInitialIntensity ***************************/

/*
 * This function sets the module's initialIntensity parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetInitialIntensity_ImpulseResp(double theInitialIntensity)
{
	static const WChar	*funcName = wxT("SetInitialIntensity_ImpulseResp");

	if (iRParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	iRParsPtr->initialIntensityFlag = TRUE;
	iRParsPtr->initialIntensity = theInitialIntensity;
	return(TRUE);

}

/****************************** SetDeltaIntensity *****************************/

/*
 * This function sets the module's deltaIntensity parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDeltaIntensity_ImpulseResp(double theDeltaIntensity)
{
	static const WChar	*funcName = wxT("SetDeltaIntensity_ImpulseResp");

	if (iRParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	iRParsPtr->deltaIntensityFlag = TRUE;
	iRParsPtr->deltaIntensity = theDeltaIntensity;
	return(TRUE);

}

/****************************** SetStimulusDuration ***************************/

/*
 * This function sets the module's stimulusDuration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetStimulusDuration_ImpulseResp(double theStimulusDuration)
{
	static const WChar	*funcName = wxT("SetStimulusDuration_ImpulseResp");

	if (iRParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	iRParsPtr->stimulusDurationFlag = TRUE;
	iRParsPtr->stimulusDuration = theStimulusDuration;
	return(TRUE);

}

/****************************** SetSamplingInterval ***************************/

/*
 * This function sets the module's samplingInterval parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSamplingInterval_ImpulseResp(double theSamplingInterval)
{
	static const WChar	*funcName = wxT("SetSamplingInterval_ImpulseResp");

	if (iRParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	iRParsPtr->dtFlag = TRUE;
	iRParsPtr->dt = theSamplingInterval;
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
CheckPars_ImpulseResp(void)
{
	static const WChar	*funcName = wxT("CheckPars_ImpulseResp");
	BOOLN	ok;

	ok = TRUE;
	if (iRParsPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (!iRParsPtr->enabledFlag) {
		NotifyError(wxT("%s: enabled variable not set."), funcName);
		ok = FALSE;
	}
	if (!iRParsPtr->outputModeFlag) {
		NotifyError(wxT("%s: outputMode variable not set."), funcName);
		ok = FALSE;
	}
	if (!iRParsPtr->clickTimeFlag) {
		NotifyError(wxT("%s: clickTime variable not set."), funcName);
		ok = FALSE;
	}
	if (!iRParsPtr->clickWidthFlag) {
		NotifyError(wxT("%s: clickWidth variable not set."), funcName);
		ok = FALSE;
	}
	if (!iRParsPtr->refAmplitudeFlag) {
		NotifyError(wxT("%s: refAmplitude variable not set."), funcName);
		ok = FALSE;
	}
	if (!iRParsPtr->numIntensitiesFlag) {
		NotifyError(wxT("%s: numIntensities variable not set."), funcName);
		ok = FALSE;
	}
	if (!iRParsPtr->initialIntensityFlag) {
		NotifyError(wxT("%s: initialIntensity variable not set."), funcName);
		ok = FALSE;
	}
	if (!iRParsPtr->deltaIntensityFlag) {
		NotifyError(wxT("%s: deltaIntensity variable not set."), funcName);
		ok = FALSE;
	}
	if (!iRParsPtr->stimulusDurationFlag) {
		NotifyError(wxT("%s: stimulusDuration variable not set."), funcName);
		ok = FALSE;
	}
	if (!iRParsPtr->dtFlag) {
		NotifyError(wxT("%s: dt variable not set."), funcName);
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
PrintPars_ImpulseResp(void)
{
	static const WChar	*funcName = wxT("PrintPars_ImpulseResp");

	if (!CheckPars_ImpulseResp()) {
		NotifyError(wxT("%s: Parameters have not been correctly set."), funcName);
		return(FALSE);
	}
	DPrint(wxT("?? ImpulseResp Module Parameters:-\n"));
	DPrint(wxT("\tenabled = %s \n"), BooleanList_NSpecLists(
	  iRParsPtr->enabled)->name);
	DPrint(wxT("\toutputMode = %s \n"), iRParsPtr->outputModeList[
	  iRParsPtr->outputMode].name);
	DPrint(wxT("\tclickTime = %g ??\n"), iRParsPtr->clickTime);
	DPrint(wxT("\tclickWidth = %g ??\n"), iRParsPtr->clickWidth);
	DPrint(wxT("\trefAmplitude = %g ??\n"), iRParsPtr->refAmplitude);
	DPrint(wxT("\tnumIntensities = %d ??\n"), iRParsPtr->numIntensities);
	DPrint(wxT("\tinitialIntensity = %g ??\n"), iRParsPtr->initialIntensity);
	DPrint(wxT("\tdeltaIntensity = %g ??\n"), iRParsPtr->deltaIntensity);
	DPrint(wxT("\tstimulusDuration = %g ??\n"), iRParsPtr->stimulusDuration);
	DPrint(wxT("\tsamplingInterval = %g ??\n"), iRParsPtr->dt);
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
CheckData_ImpulseResp(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_ImpulseResp");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	/*** Put additional checks here. ***/
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_ImpulseResp(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_ImpulseResp");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	iRParsPtr = (IRParsPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 * It also initialises the process structure.
 */

BOOLN
InitModule_ImpulseResp(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_ImpulseResp");

	if (!SetParsPointer_ImpulseResp(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_ImpulseResp(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = iRParsPtr;
	theModule->CheckPars = CheckPars_ImpulseResp;
	theModule->Free = Free_ImpulseResp;
	/*theModule->GetData = NormalSignalStyle_ImpulseResp;*/
	theModule->GetUniParListPtr = GetUniParListPtr_ImpulseResp;
	theModule->PrintPars = PrintPars_ImpulseResp;
	theModule->RunProcess = RunProcess_ImpulseResp;
	theModule->SetParsPointer = SetParsPointer_ImpulseResp;
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
InitProcessVariables_ImpulseResp(EarObjectPtr data)
{
	static const WChar * funcName = wxT("InitProcessVariables_ImpulseResp");
	double	beginPeriod, endPeriod;
	DatumPtr	pc;
	IRParsPtr	p = iRParsPtr;

	p->audModel = GetSimProcess_AppInterface();
	if ((p->stimulus = Init_EarObject(STIM_STEPFUN_PROCESS_NAME)) == NULL) {
		NotifyError(wxT("%s: Could not initialise the stimulus EarObject."),
		  funcName);
		return(FALSE);
	}

	ConnectOutSignalToIn_EarObject(p->stimulus, p->audModel);

	if (!SetRealPar_ModuleMgr(p->stimulus, wxT("b_e_amp"), 0.0)) {
		NotifyError(wxT("%s: Cannot set the silence period amplitude."), funcName);
		return(FALSE);
	}
	beginPeriod = p->clickTime - p->dt;
	if (!SetRealPar_ModuleMgr(p->stimulus, wxT("begin_period"), beginPeriod)) {
		NotifyError(wxT("%s: Cannot set the stimulus begin period (%g ms)."),
		  funcName, MILLI(beginPeriod));
		return(FALSE);
	}
	if (!SetRealPar_ModuleMgr(p->stimulus, wxT("duration"), p->clickWidth)) {
		NotifyError(wxT("%s: Cannot set the stimulus duration."), funcName);
		return(FALSE);
	}
	endPeriod = p->stimulusDuration - beginPeriod - p->clickWidth;
	if (!SetRealPar_ModuleMgr(p->stimulus, wxT("end_period"), endPeriod)) {
		NotifyError(wxT("%s: Cannot set the stimulus end period (%g ms)."),
		  funcName, MILLI(endPeriod));
		return(FALSE);
	}
	if (!SetRealPar_ModuleMgr(p->stimulus, wxT("dt"), p->dt)) {
		NotifyError(wxT("%s: Cannot set the stimulus sampling interval."),
		  funcName);
		return(FALSE);
	}
	if ((pc = FindModuleProcessInst_Utility_Datum(GetSimulation_ModuleMgr(data),
	  wxT("Trans_ramp"))) != NULL) {
		p->gate = pc->data;
		Enable_ModuleMgr(p->gate, FALSE);
	}
	if ((p->resultEarObj = InitResultEarObject_Utils(p->numIntensities,
	  (ChanLen) (p->stimulusDuration / p->dt + 1.5), 0.0, p->dt, funcName)) ==
	  NULL) {
		NotifyError(wxT("%s: Could not initialise results EarObject"), funcName);
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
FreeProcessVariables_ImpulseResp(void)
{
	if (iRParsPtr->stimulus && iRParsPtr->audModel)
		DisconnectOutSignalFromIn_EarObject(iRParsPtr->stimulus,
		  iRParsPtr->audModel);
	Free_EarObject(&iRParsPtr->stimulus);
	Free_EarObject(&iRParsPtr->resultEarObj);
	if (iRParsPtr->gate) {
		Enable_ModuleMgr(iRParsPtr->gate, TRUE);
		iRParsPtr->gate = NULL;
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
RunProcess_ImpulseResp(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("RunProcess_ImpulseResp");
	register	ChanData	 *inPtr, *outPtr;
	int		i;
	ChanLen	j;
	double	intensity, vRef;

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckPars_ImpulseResp())
		return(FALSE);
	if (!CheckData_ImpulseResp(data)) {
		NotifyError(wxT("%s: Process data invalid."), funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, wxT("Impulse Response process"));
	if (!InitProcessVariables_ImpulseResp(data)) {
		NotifyError(wxT("%s: Could not initialise the process variables."),
		  funcName);
		return(FALSE);
	}
	vRef = iRParsPtr->refAmplitude / pow(10.0, iRParsPtr->initialIntensity /
	  20.0);
	for (i = 0, intensity = iRParsPtr->initialIntensity; i < 
	  iRParsPtr->numIntensities; i++, intensity += iRParsPtr->deltaIntensity) {
		if (!SetRealPar_ModuleMgr(iRParsPtr->stimulus, wxT("amplitude"),
		  vRef * pow(10.0, intensity / 20.0))) {
			NotifyError(wxT("%s: Cannot set the click intensity (%g dB SPL)."),
			  funcName, intensity);
			return(FALSE);
		}
		if (!RunProcess_ModuleMgr(iRParsPtr->stimulus)) {
			NotifyError(wxT("%s: Failed to run stimulus process."), funcName);
			FreeProcessVariables_ImpulseResp();
			return(FALSE);
		}
		if (!RunProcess_ModuleMgr(iRParsPtr->audModel)) {
			NotifyError(wxT("%s: Failed to run auditory model."), funcName);
			FreeProcessVariables_ImpulseResp();
			return(FALSE);
		}
		inPtr =  iRParsPtr->audModel->outSignal->channel[0];
		outPtr = iRParsPtr->resultEarObj->outSignal->channel[i];
		for (j = 0; j < iRParsPtr->audModel->outSignal->length; j++)
			*outPtr++ = *inPtr++;
		SetInfoChannelLabel_SignalData(iRParsPtr->resultEarObj->outSignal, i,
		  intensity);
	}
	OutputResults_Utils(iRParsPtr->resultEarObj, &iRParsPtr->outputModeList[
	  iRParsPtr->outputMode], UTILS_NORMAL_SIGNAL);
	FreeProcessVariables_ImpulseResp();
	return(TRUE);

}

