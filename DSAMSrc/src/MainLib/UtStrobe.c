/**********************
 *
 * File:		UtStrobe.c
 * Purpose:		This module implements strobe criteria for the AIM stabilised
 *				auditory image (SAI)
 *				See Patterson R. D. and Allerhand M. H. (1995) "Time-domain
 *				modeling of peripheral auditory processing: A modular
 *				Architecture and a software platform", J. Acoust. Soc. Am. 98,
 *				1890-1894.
 * Comments:	Written using ModuleProducer version 1.12 (Oct  9 1997).
 *				The "Threshold" strobe mode algorithm is the same as the
 *				"user" mode at present.  This is because the "User" mode is
 *				interpreted differently by the calling SAI module program.
 *				This method saves having to have a separate set of mode names
 *				in the SAI module.
 *				20-11-97 LPO: Peak strobe criteria now only strobes when the
 *				peak is above the threshold.
 *				21-11-97 LPO: Changed "lagTime" to "delay".
 *				25-11-97 LPO: Implemented the peak shadow "-" mode.
 *				10-12-97 LPO: After a long battle I have finished implementing
 *				the peak shadow "+" mode.  Note that the definition for this
 *				criterion is different from that described in the AIM code
 *				(though even in the AIM code is was not implemented.)
 *				11-12-97 LPO: Added the delayTimeout constraint.  This causes
 *				the most recent strobe peak to be set, regardless of the
 *				delayCount state.
 *				11-09-98 LPO: Memory allocation under Borland C++ 4.5 returns
 *				a null pointer if zero units are requested.  A minimum of 1 must
 *				always be allocated.
 *				06-07-99 LPO: This module now sets the 'staticTimeFlag', as
 *				it sets the 'outputTimeOffset' field,
 * Author:		L. P. O'Mard
 * Created:		10 Oct 1997
 * Updated:		11 Sep 1997
 * Copyright:	(c) 1997-1999, University of Essex
 *
 *********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "GeCommon.h"
#include "GeNSpecLists.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "FiParFile.h"
#include "UtStrobe.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

StrobePtr	strobePtr = NULL;

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
Free_Utility_Strobe(void)
{
	/* static const char	*funcName = "Free_Utility_Strobe";  */

	if (strobePtr == NULL)
		return(FALSE);
	FreeProcessVariables_Utility_Strobe();
	if (strobePtr->diagnosticModeList)
		free(strobePtr->diagnosticModeList);
	if (strobePtr->parList)
		FreeList_UniParMgr(&strobePtr->parList);
	if (strobePtr->parSpec == GLOBAL) {
		free(strobePtr);
		strobePtr = NULL;
	}
	return(TRUE);

}

/****************************** InitTypeModeList ******************************/

/*
 * This routine intialises the TypeMode list array.
 */

BOOLN
InitTypeModeList_Utility_Strobe(void)
{
	static NameSpecifier	modeList[] = {

					{ "USER", 			STROBE_USER_MODE },
					{ "THRESHOLD", 		STROBE_THRESHOLD_MODE },
					{ "PEAK", 			STROBE_PEAK_MODE },
					{ "PEAK_SHADOW-",	STROBE_PEAK_SHADOW_NEGATIVE_MODE },
					{ "PEAK_SHADOW+",	STROBE_PEAK_SHADOW_POSITIVE_MODE },
					{ "DELTA_GAMMA",	STROBE_DELTA_GAMMA_MODE },
					{ "", STROBE_MODE_NULL }
				};
	strobePtr->typeModeList = modeList;
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
Init_Utility_Strobe(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_Utility_Strobe";

	if (parSpec == GLOBAL) {
		if (strobePtr != NULL)
			Free_Utility_Strobe();
		if ((strobePtr = (StrobePtr) malloc(sizeof(Strobe))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (strobePtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	strobePtr->parSpec = parSpec;
	strobePtr->updateProcessVariablesFlag = TRUE;
	strobePtr->typeModeFlag = TRUE;
	strobePtr->diagnosticModeFlag = TRUE;
	strobePtr->thresholdFlag = TRUE;
	strobePtr->thresholdDecayRateFlag = TRUE;
	strobePtr->delayFlag = TRUE;
	strobePtr->delayTimeoutFlag = TRUE;
	strobePtr->typeMode = STROBE_PEAK_SHADOW_POSITIVE_MODE;
	strobePtr->diagnosticMode = GENERAL_DIAGNOSTIC_OFF_MODE;
	strobePtr->threshold = 0.0;
	strobePtr->thresholdDecayRate = 5000.0;
	strobePtr->delay = 5e-3;
	strobePtr->delayTimeout = 10e-3;

	InitTypeModeList_Utility_Strobe();
	if ((strobePtr->diagnosticModeList = InitNameList_NSpecLists(
	  DiagModeList_NSpecLists(0), strobePtr->diagnosticString)) == NULL)
		return(FALSE);
	if (!SetUniParList_Utility_Strobe()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_Utility_Strobe();
		return(FALSE);
	}
	strobePtr->numChannels = 0;
	strobePtr->thresholdDecay = 0.0;
	strobePtr->numLastSamples = 0;
	strobePtr->delayTimeoutSamples = 0;
	strobePtr->fp = NULL;
	strobePtr->stateVars = NULL;
	return(TRUE);

}

/********************************* SetUniParList ******************************/

/*
 * This routine initialises and sets the module's universal parameter list.
 * This list provides universal access to the module's parameters.
 */
 
BOOLN
SetUniParList_Utility_Strobe(void)
{
	static const char *funcName = "SetUniParList_Utility_Strobe";
	UniParPtr	pars;

	if ((strobePtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  STROBE_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = strobePtr->parList->pars;
	SetPar_UniParMgr(&pars[STROBE_TYPE_MODE], "CRITERION",
	  "Strobe criterion (mode): 'user ', 'threshold' (0), 'peak' (1), "
	  "'peak_shadow-' (3), or 'peak_shadow+' (4/5).",
	  UNIPAR_NAME_SPEC,
	  &strobePtr->typeMode, strobePtr->typeModeList,
	  (void * (*)) SetTypeMode_Utility_Strobe);
	SetPar_UniParMgr(&pars[STROBE_DIAGNOSTIC_MODE], "STROBE_DIAGNOSTICS",
	  "Diagnostic mode ('off', 'screen', 'error' or <file name>).",
	  UNIPAR_NAME_SPEC_WITH_FILE,
	  &strobePtr->diagnosticMode, strobePtr->diagnosticModeList,
	  (void * (*)) SetDiagnosticMode_Utility_Strobe);
	SetPar_UniParMgr(&pars[STROBE_THRESHOLD], "THRESHOLD",
	  "Threshold for strobing: 'user', 'threshold' and 'peak' modes only.",
	  UNIPAR_REAL,
	  &strobePtr->threshold, NULL,
	  (void * (*)) SetThreshold_Utility_Strobe);
	SetPar_UniParMgr(&pars[STROBE_THRESHOLD_DECAY_RATE], "THRESHOLD_DECAY",
	  "Threshold decay rate (%/s).",
	  UNIPAR_REAL,
	  &strobePtr->thresholdDecayRate, NULL,
	  (void * (*)) SetThresholdDecayRate_Utility_Strobe);
	SetPar_UniParMgr(&pars[STROBE_DELAY], "STROBE_LAG",
	  "Strobe lag (delay) time (s).",
	  UNIPAR_REAL,
	  &strobePtr->delay, NULL,
	  (void * (*)) SetDelay_Utility_Strobe);
	SetPar_UniParMgr(&pars[STROBE_DELAY_TIMEOUT], "TIMEOUT",
	  "Strobe lag (delay) timeout (s).",
	  UNIPAR_REAL,
	  &strobePtr->delayTimeout, NULL,
	  (void * (*)) SetDelayTimeout_Utility_Strobe);

	return(TRUE);

}

/********************************* GetUniParListPtr ***************************/

/*
 * This routine returns a pointer to the module's universal parameter list.
 */

UniParListPtr
GetUniParListPtr_Utility_Strobe(void)
{
	static const char *funcName = "GetUniParListPtr_Utility_Strobe";

	if (strobePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (strobePtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(strobePtr->parList);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_Utility_Strobe(char *typeMode, char *diagnosticMode, double threshold,
  double thresholdDecayRate, double delay, double delayTimeout)
{
	static const char	*funcName = "SetPars_Utility_Strobe";
	BOOLN	ok;

	ok = TRUE;
	if (!SetTypeMode_Utility_Strobe(typeMode))
		ok = FALSE;
	if (!SetDiagnosticMode_Utility_Strobe(diagnosticMode))
		ok = FALSE;
	if (!SetThreshold_Utility_Strobe(threshold))
		ok = FALSE;
	if (!SetThresholdDecayRate_Utility_Strobe(thresholdDecayRate))
		ok = FALSE;
	if (!SetDelay_Utility_Strobe(delay))
		ok = FALSE;
	if (!SetDelayTimeout_Utility_Strobe(delayTimeout))
		ok = FALSE;
	if (!ok)
		NotifyError("%s: Failed to set all module parameters." ,funcName);
	return(ok);

}

/****************************** SetTypeMode ***********************************/

/*
 * This function sets the module's typeMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTypeMode_Utility_Strobe(char *theTypeMode)
{
	static const char	*funcName = "SetTypeMode_Utility_Strobe";
	int		specifier;

	if (strobePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theTypeMode,
	  strobePtr->typeModeList)) == STROBE_MODE_NULL) {
		NotifyError("%s: Illegal type mode name (%s).", funcName, theTypeMode);
		return(FALSE);
	}
	strobePtr->typeModeFlag = TRUE;
	strobePtr->typeMode = specifier;
	strobePtr->updateProcessVariablesFlag = TRUE;
	switch (strobePtr->typeMode) {
	case STROBE_USER_MODE:
	case STROBE_THRESHOLD_MODE:
	case STROBE_PEAK_MODE:
		strobePtr->parList->pars[STROBE_THRESHOLD].enabled = TRUE;
		strobePtr->parList->pars[STROBE_THRESHOLD_DECAY_RATE].enabled = FALSE;
		strobePtr->parList->pars[STROBE_DELAY].enabled = FALSE;
		strobePtr->parList->pars[STROBE_DELAY_TIMEOUT].enabled = FALSE;
		break;
	case STROBE_PEAK_SHADOW_NEGATIVE_MODE:
		strobePtr->parList->pars[STROBE_THRESHOLD].enabled = FALSE;
		strobePtr->parList->pars[STROBE_THRESHOLD_DECAY_RATE].enabled = TRUE;
		strobePtr->parList->pars[STROBE_DELAY].enabled = FALSE;
		strobePtr->parList->pars[STROBE_DELAY_TIMEOUT].enabled = FALSE;
		break;
	case STROBE_PEAK_SHADOW_POSITIVE_MODE:
		strobePtr->parList->pars[STROBE_THRESHOLD].enabled = FALSE;
		strobePtr->parList->pars[STROBE_THRESHOLD_DECAY_RATE].enabled = TRUE;
		strobePtr->parList->pars[STROBE_DELAY].enabled = TRUE;
		strobePtr->parList->pars[STROBE_DELAY_TIMEOUT].enabled = TRUE;
		break;
	default:
		strobePtr->parList->pars[STROBE_THRESHOLD].enabled = FALSE;
		strobePtr->parList->pars[STROBE_THRESHOLD_DECAY_RATE].enabled = FALSE;
		strobePtr->parList->pars[STROBE_DELAY].enabled = FALSE;
		strobePtr->parList->pars[STROBE_DELAY_TIMEOUT].enabled = FALSE;
		;
	}
	strobePtr->parList->updateFlag = TRUE;
	return(TRUE);

}

/****************************** SetDiagnosticMode *****************************/

/*
 * This function sets the module's diagnosticMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDiagnosticMode_Utility_Strobe(char *theDiagnosticMode)
{
	static const char	*funcName = "SetDiagnosticMode_Utility_Strobe";

	if (strobePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	strobePtr->diagnosticModeFlag = TRUE;
	strobePtr->diagnosticMode = IdentifyDiag_NSpecLists(theDiagnosticMode,
	  strobePtr->diagnosticModeList);
	return(TRUE);

}

/****************************** SetThreshold **********************************/

/*
 * This function sets the module's threshold parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetThreshold_Utility_Strobe(double theThreshold)
{
	static const char	*funcName = "SetThreshold_Utility_Strobe";

	if (strobePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	strobePtr->thresholdFlag = TRUE;
	strobePtr->threshold = theThreshold;
	return(TRUE);

}

/****************************** SetThresholdDecayRate *************************/

/*
 * This function sets the module's thresholdDecayRate parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetThresholdDecayRate_Utility_Strobe(double theThresholdDecayRate)
{
	static const char	*funcName = "SetThresholdDecayRate_Utility_Strobe";

	if (strobePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	strobePtr->thresholdDecayRateFlag = TRUE;
	strobePtr->thresholdDecayRate = theThresholdDecayRate;
	strobePtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** SetDelay **************************************/

/*
 * This function sets the module's delay parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDelay_Utility_Strobe(double theDelay)
{
	static const char	*funcName = "SetDelay_Utility_Strobe";

	if (strobePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	strobePtr->delayFlag = TRUE;
	strobePtr->delay = theDelay;
	strobePtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** SetDelayTimeout *******************************/

/*
 * This function sets the module's delayTimeout parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDelayTimeout_Utility_Strobe(double theDelayTimeout)
{
	static const char	*funcName = "SetDelayTimeout_Utility_Strobe";

	if (strobePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	strobePtr->delayTimeoutFlag = TRUE;
	strobePtr->delayTimeout = theDelayTimeout;
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
CheckPars_Utility_Strobe(void)
{
	static const char	*funcName = "CheckPars_Utility_Strobe";
	BOOLN	ok;

	ok = TRUE;
	if (strobePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!strobePtr->typeModeFlag) {
		NotifyError("%s: Type mode variable not set.", funcName);
		ok = FALSE;
	}
	if (!strobePtr->diagnosticModeFlag) {
		NotifyError("%s: diagnosticMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!strobePtr->thresholdFlag) {
		NotifyError("%s: Threshold variable not set.", funcName);
		ok = FALSE;
	}
	if (!strobePtr->thresholdDecayRateFlag) {
		NotifyError("%s: ThresholdDecayRate variable not set.", funcName);
		ok = FALSE;
	}
	if (!strobePtr->delayFlag) {
		NotifyError("%s: Delay variable not set.", funcName);
		ok = FALSE;
	}
	if (!strobePtr->delayTimeoutFlag) {
		NotifyError("%s: Delay limit variable not set.", funcName);
		ok = FALSE;
	}
	return(ok);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the dSAM.parsFile file pointer.
 */

BOOLN
PrintPars_Utility_Strobe(void)
{
	static const char	*funcName = "PrintPars_Utility_Strobe";

	if (!CheckPars_Utility_Strobe()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Strobe Utility Module Parameters:-\n");
	DPrint("\tType mode: %s,", strobePtr->typeModeList[
	  strobePtr->typeMode].name);
	DPrint("\tDiagnostic mode: %s,\n", strobePtr->diagnosticModeList[
	  strobePtr->diagnosticMode].name);
	if ((strobePtr->typeMode == STROBE_USER_MODE) ||
	  (strobePtr->typeMode == STROBE_THRESHOLD_MODE))
		DPrint("\tThreshold = %g units,", strobePtr->threshold);
	if ((strobePtr->typeMode == STROBE_PEAK_MODE) ||
	  (strobePtr->typeMode == STROBE_PEAK_SHADOW_NEGATIVE_MODE) ||
	  (strobePtr->typeMode == STROBE_PEAK_SHADOW_POSITIVE_MODE)) 
		DPrint("\tThreshold decay rate = %g %/s,\n", strobePtr->
		  thresholdDecayRate);
	else
		DPrint("\n");
	if (strobePtr->typeMode == STROBE_PEAK_SHADOW_POSITIVE_MODE) {
		DPrint("\tDelay = %g ms,", MSEC(strobePtr->delay));
		DPrint("\tDelay timeout = ");
		if (strobePtr->delayTimeout < 0.0)
			DPrint("unlimited.\n");
		else
			DPrint("%g ms.\n", MSEC(strobePtr->delayTimeout));
	}
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_Utility_Strobe(char *fileName)
{
	static const char	*funcName = "ReadPars_Utility_Strobe";
	BOOLN	ok;
	char	*filePath;
	char	typeMode[MAXLINE], diagnosticMode[MAXLINE];
	double	threshold, thresholdDecayRate, delay, delayTimeout;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(filePath, "r")) == NULL) {
		NotifyError("%s: Cannot open data file '%s'.\n", funcName, filePath);
		return(FALSE);
	}
	DPrint("%s: Reading from '%s':\n", funcName, filePath);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, "%s", typeMode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%s", diagnosticMode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &threshold))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &thresholdDecayRate))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &delay))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &delayTimeout))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "
		  "parameter file '%s'.", funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_Utility_Strobe(typeMode, diagnosticMode, threshold,
	  thresholdDecayRate, delay, delayTimeout)) {
		NotifyError("%s: Could not set parameters.", funcName);
		return(FALSE);
	}
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Utility_Strobe(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_Utility_Strobe";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	strobePtr = (StrobePtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Utility_Strobe(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_Utility_Strobe";

	if (!SetParsPointer_Utility_Strobe(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_Utility_Strobe(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = strobePtr;
	theModule->CheckPars = CheckPars_Utility_Strobe;
	theModule->Free = Free_Utility_Strobe;
	theModule->GetUniParListPtr = GetUniParListPtr_Utility_Strobe;
	theModule->PrintPars = PrintPars_Utility_Strobe;
	theModule->ReadPars = ReadPars_Utility_Strobe;
	theModule->RunProcess = Process_Utility_Strobe;
	theModule->SetParsPointer = SetParsPointer_Utility_Strobe;
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
CheckData_Utility_Strobe(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_Utility_Strobe";

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	if ((strobePtr->typeMode == STROBE_PEAK_SHADOW_POSITIVE_MODE) &&
	  (_GetDuration_SignalData(data->inSignal[0]) <= strobePtr->delay)) {
		NotifyError("%s: Strobe delay (%g ms) is not less than signal\n"
		  "duration (%g ms).", funcName, MSEC(strobePtr->delay),
		  MSEC(_GetDuration_SignalData(data->inSignal[0])));
		return(FALSE);
	}
	return(TRUE);

}

/**************************** FreeStateVariables ******************************/

/*
 * This routine frees the space allocated for the state variables.
 */

void
FreeStateVariables_Utility_Strobe(StrobeStatePtr *p)
{
	if (*p == NULL)
		return;
	if ((*p)->lastInput)
		free((*p)->lastInput);
	free(*p);
	*p = NULL;

}

/**************************** InitStateVariables ******************************/

/*
 * This function allocates the memory for the process state variables.
 * It also carries out the necessary initialisation.
 * These are variables which must be remembered from one run to another in 
 * segment processing mode.
 */

StrobeStatePtr
InitStateVariables_Utility_Strobe(ChanLen numLastSamples)
{
	static const char *funcName = "InitStateVariables_Utility_Strobe";
	StrobeStatePtr	p;
	
	if ((p = (StrobeStatePtr) malloc(sizeof(StrobeState))) == NULL) {
		NotifyError("%s: Out of memory for state variables.", funcName);
		return(NULL);
	}
	if (numLastSamples) {
		if ((p->lastInput = (ChanData *) calloc(numLastSamples, sizeof(
		  ChanData))) == NULL) {
			NotifyError("%s: Out of memory for 'lastInput' array (%u "
			  "elements).", funcName, numLastSamples);
			FreeStateVariables_Utility_Strobe(&p);
			return(NULL);
		}
	} else
		p->lastInput = NULL;
	p->gradient = FALSE;
	p->strobeAlreadyPlaced = FALSE;
	p->widthIndex = 0;
	p->delayCount = 0;
	p->prevPeakIndex = 0;
	p->prevPeakHeight = 0.0;
	p->lastSample = 0.0;
	p->threshold = 0.0;
	p->deltaThreshold = 0.0;
	return(p);

}


/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 * It also carries out any necessary initialisations.
 * This routine adjusts the process timing according to the response delay
 * required.
 */

BOOLN
InitProcessVariables_Utility_Strobe(EarObjectPtr data)
{
	static const char *funcName = "InitProcessVariables_Utility_Strobe";
	register ChanData	*lastInput, *outPtr;
	int			i;
	ChanLen		j;
	StrobePtr	p;
	StrobeStatePtr	statePtr;

	p = strobePtr;
	if (p->updateProcessVariablesFlag || data->updateProcessFlag ||
	  (data->timeIndex == PROCESS_START_TIME)) {
		if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
			FreeProcessVariables_Utility_Strobe();
			OpenDiagnostics_NSpecLists(&strobePtr->fp,
			  strobePtr->diagnosticModeList, strobePtr->diagnosticMode);
			if (strobePtr->diagnosticMode)
				fprintf(strobePtr->fp, "Time (s)\tThreshold\n");
			p->numChannels = data->outSignal->numChannels;
			if ((p->stateVars = (StrobeStatePtr *) calloc(
			  p->numChannels, sizeof(StrobeState))) == NULL) {
		 		NotifyError("%s: Out of memory for state variables array.",
		 		  funcName);
		 		return(FALSE);
			}
			switch (p->typeMode) {
			case STROBE_PEAK_MODE:
			case STROBE_PEAK_SHADOW_NEGATIVE_MODE:
				p->numLastSamples = 1;
				break;
			case STROBE_PEAK_SHADOW_POSITIVE_MODE:
				p->numLastSamples = (ChanLen) floor(p->delay / data->outSignal->
				  dt + 0.5);
				p->delayTimeoutSamples = (ChanLen) floor((p->delayTimeout <
				  0.0)? 0: p->delayTimeout / data->outSignal->dt + 0.5);
				break;
			default:
				p->numLastSamples = 0;
			}
			p->thresholdDecay = p->thresholdDecayRate / 100.0 *
			  data->outSignal->dt;
			for (i = 0; i < p->numChannels; i++) {
				if ((p->stateVars[i] = InitStateVariables_Utility_Strobe(
				  p->numLastSamples)) == NULL) {
					NotifyError("%s: Out of memory for state variable (%d).",
					  funcName, i);
					FreeProcessVariables_Utility_Strobe();
					return(FALSE);
				}
			}
			p->updateProcessVariablesFlag = FALSE;
		}
		for (i = 0; i < data->outSignal->numChannels; i++) {
			statePtr = p->stateVars[i];
			lastInput = statePtr->lastInput;
			outPtr = data->outSignal->channel[i];
			statePtr->lastSample = data->inSignal[0]->channel[i][0];
			for (j = 0; j < p->numLastSamples; j++) {
				*lastInput++ = data->inSignal[0]->channel[i][0];
				*outPtr++ = 0.0;
			}
			statePtr->gradient = FALSE;
			statePtr->widthIndex = 0;
			statePtr->prevPeakIndex = 0;
			statePtr->prevPeakHeight = data->inSignal[0]->channel[i][0];
			statePtr->delayCount = 0;
			statePtr->delayTimeoutCount = 0;
			switch (p->typeMode) {
			case STROBE_PEAK_MODE:
				statePtr->threshold = strobePtr->threshold;
				break;
			case STROBE_PEAK_SHADOW_NEGATIVE_MODE:
			case STROBE_PEAK_SHADOW_POSITIVE_MODE:
				statePtr->threshold = data->inSignal[0]->channel[i][0];
				break;
			default:
				;
			} /* switch */
		}
				
	}
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

void
FreeProcessVariables_Utility_Strobe(void)
{
	int		i;

	if (strobePtr->stateVars) {
		for (i = 0; i < strobePtr->numChannels; i++)
    		FreeStateVariables_Utility_Strobe(&strobePtr->stateVars[i]);
		free(strobePtr->stateVars);
		strobePtr->stateVars = NULL;
	}
	CloseDiagnostics_NSpecLists(&strobePtr->fp);
	strobePtr->updateProcessVariablesFlag = TRUE;

}

/****************************** ProcessThesholdModes **************************/

/*
 * This routine carries out the threshold strobe processing mode.
 * It is used by both the "threshold" and "user" modes.
 * It assumes that all the correct module intialisation has been performed.
 */

void
ProcessThesholdModes_Utility_Strobe(EarObjectPtr data)
{
	/* static const char	*funcName = "ProcessThesholdModes_Utility_Strobe";
	*/
	register	ChanData	 *inPtr, *outPtr;
	int		chan;
	ChanLen	i;

	for (chan = 0; chan < data->inSignal[0]->numChannels; chan++) {
		inPtr = data->inSignal[0]->channel[chan];
		outPtr = data->outSignal->channel[chan];
		for (i = 0; i < data->inSignal[0]->length; i++)
			*outPtr++ = (*inPtr++ > strobePtr->threshold)? 1.0: 0.0;
	}

}

/****************************** ProcessPeakChannel ****************************/

/*
 * This routine carries out the peak strobe processing for a range of data.
 * A special provision had to be made for the buffer reading when tests for the
 * end of a peak are made across the end boundary.
 * For reading past the end of the PROCESS_DATA_CHANNEL an copy of the count is
 * used to preserve the count contents, and if the delay reaches zero, and the
 * strobe point is set, then the previous height is set to less than the current
 * input signal height, to ensure no attempt is made to place the strobe point.
 */

#define	_SetDecayStrobeTrigger()  \
			s->threshold = *inPtr; \
			s->deltaThreshold = *inPtr * strobePtr->thresholdDecay;

void
ProcessPeakChannel_Utility_Strobe(EarObjectPtr data,
  StrobeChanProcessSpecifier chanProcessSpecifier)
{
	/* static const char	*funcName = "ProcessPeakChannel_Utility_Strobe"; */
	register	ChanData	 *inPtr, *outPtr;
	BOOLN		delayTimeout, gradient, foundPeak;
	int			chan;
	ChanLen		i, length, endProcessCount, endProcessLimitCount;
	ChanData	nextSample, lastSample;
	StrobeStatePtr	s;

	for (chan = 0; chan < data->inSignal[0]->numChannels; chan++) {
		s = strobePtr->stateVars[chan];
		if (chanProcessSpecifier == STROBE_PROCESS_DATA_CHANNEL) {
			inPtr = data->inSignal[0]->channel[chan];
			outPtr = data->outSignal->channel[chan] + strobePtr->numLastSamples;
			length = data->inSignal[0]->length - strobePtr->numLastSamples;
		} else {
			inPtr = s->lastInput;
			outPtr = data->outSignal->channel[chan];
			length = strobePtr->numLastSamples;
		}
		for (i = 0; i < length; i++, inPtr++, outPtr++) {
			*outPtr = 0.0;
			if (!s->gradient)
				s->gradient = (*inPtr > s->lastSample);
			if (s->gradient) {
				nextSample = ((chanProcessSpecifier ==
				  STROBE_PROCESS_DATA_CHANNEL) || (i < length - 1))? *(inPtr +
				  1): *(data->inSignal[0]->channel[chan]);
				if (*inPtr > nextSample) {
					if (*inPtr > s->threshold) {
						switch (strobePtr->typeMode) {
						case STROBE_PEAK_MODE:
							*(outPtr - s->widthIndex / 2) = STROBE_SPIKE_UNIT;
							break;
						case STROBE_PEAK_SHADOW_NEGATIVE_MODE:
							*(outPtr - s->widthIndex / 2) = STROBE_SPIKE_UNIT;
							_SetDecayStrobeTrigger();
							break;
						case STROBE_PEAK_SHADOW_POSITIVE_MODE:
							if (!s->delayCount || (s->prevPeakHeight <
							  *inPtr)) {
								s->prevPeakHeight = *inPtr;
								s->prevPeakIndex = (chanProcessSpecifier ==
								  STROBE_PROCESS_BUFFER_CHANNEL)? i: i +
								  strobePtr->numLastSamples;
								s->delayCount = strobePtr->numLastSamples + 1;
								_SetDecayStrobeTrigger();
								if (strobePtr->delayTimeoutSamples &&
								  !s->delayTimeoutCount)
									s->delayTimeoutCount =
									  strobePtr->delayTimeoutSamples;
							}
							break;
						default: 
							;
						}
					}
					s->widthIndex = 0;
					s->gradient = FALSE;
				} else if (*inPtr == nextSample) /* - for flat troughs.*/
					s->widthIndex++;
			}
			if (strobePtr->diagnosticMode)
				fprintf(strobePtr->fp, "%g\t%g\n", (data->timeIndex + i) *
				  data->outSignal->dt, s->threshold);
			s->threshold -= s->deltaThreshold;
			if (strobePtr->typeMode == STROBE_PEAK_SHADOW_POSITIVE_MODE) {
				delayTimeout = (strobePtr->delayTimeoutSamples &&
				  s->delayTimeoutCount && !--s->delayTimeoutCount);
				if (s->delayCount && (!--s->delayCount || delayTimeout)) {
					if (!s->strobeAlreadyPlaced)
						*(data->outSignal->channel[chan] + s->prevPeakIndex) =
						  STROBE_SPIKE_UNIT;
					else
						s->strobeAlreadyPlaced = FALSE;
					s->delayTimeoutCount = 0;
				}
			}
			s->lastSample = *inPtr;
		}
		if ((chanProcessSpecifier == STROBE_PROCESS_DATA_CHANNEL) &&
		  s->delayCount) {		/* Look past end of process for peak */
			endProcessLimitCount = s->delayTimeoutCount;
			lastSample = s->lastSample;
			for (endProcessCount = s->delayCount, delayTimeout = foundPeak =
			  gradient = FALSE; endProcessCount && !foundPeak && !delayTimeout;
			  endProcessCount--) {
				if (!gradient)
					gradient = (*inPtr > lastSample);
				if (gradient) {
					if (*inPtr > *(inPtr + 1)) {
						foundPeak = (s->prevPeakHeight < *inPtr);
						gradient = FALSE;
					}
				}
				delayTimeout = (strobePtr->delayTimeoutSamples &&
				  endProcessLimitCount && !--endProcessLimitCount);
				lastSample = *inPtr++;
			}
			if (!foundPeak || delayTimeout) {
				*(data->outSignal->channel[chan] + s->prevPeakIndex) =
				  STROBE_SPIKE_UNIT;
				s->strobeAlreadyPlaced = TRUE;
			}
		}
	}

}

#undef _SetDecayStrobeTrigger

/****************************** ProcessPeakModes ******************************/

/*
 * This routine carries out the peak strobe processing modes.
 * It assumes that all the correct module intialisation has been performed.
 */

void
ProcessPeakModes_Utility_Strobe(EarObjectPtr data)
{
	/* static const char	*funcName = "ProcessPeakModes_Utility_Strobe"; */
	register	ChanData	 *inPtr, *outPtr;
	int		chan;
	ChanLen	i;

	ProcessPeakChannel_Utility_Strobe(data, STROBE_PROCESS_BUFFER_CHANNEL);
	ProcessPeakChannel_Utility_Strobe(data, STROBE_PROCESS_DATA_CHANNEL);
	if (strobePtr->numLastSamples)
		for (chan = 0; chan < data->inSignal[0]->numChannels; chan++) {
			inPtr = data->inSignal[0]->channel[chan] +
			  data->inSignal[0]->length - strobePtr->numLastSamples;
			outPtr = strobePtr->stateVars[chan]->lastInput;
			for (i = 0; i < strobePtr->numLastSamples; i++)
				*outPtr++ = *inPtr++;
		}

}

/****************************** Process ***************************************/

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
Process_Utility_Strobe(EarObjectPtr data)
{
	static const char	*funcName = "Process_Utility_Strobe";

	if (!CheckPars_Utility_Strobe())
		return(FALSE);
	if (!CheckData_Utility_Strobe(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "Strobe Utility Module process");
	if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels, 
	  data->inSignal[0]->length, data->inSignal[0]->dt)) {
		NotifyError("%s: Cannot initialise output channels.", funcName);
		return(FALSE);
	}
	if (!InitProcessVariables_Utility_Strobe(data)) {
		NotifyError("%s: Could not initialise the process variables.",
		  funcName);
		return(FALSE);
	}
	SetOutputTimeOffset_SignalData(data->outSignal, -data->outSignal->dt *
	 strobePtr->numLastSamples);
	switch (strobePtr->typeMode) {
	case STROBE_USER_MODE:
	case STROBE_THRESHOLD_MODE:
		ProcessThesholdModes_Utility_Strobe(data);
		break;
	case STROBE_PEAK_MODE:
	case STROBE_PEAK_SHADOW_NEGATIVE_MODE:
	case STROBE_PEAK_SHADOW_POSITIVE_MODE:
		ProcessPeakModes_Utility_Strobe(data);
		break;
	default:
		NotifyError("%s: Unimplemented mode (%d).", funcName,
		  strobePtr->typeMode);
		return(FALSE);			  
	} /* switch */
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

