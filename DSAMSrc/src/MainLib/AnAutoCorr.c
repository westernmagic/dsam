/**********************
 *
 * File:		AnAutoCorr.c
 * Purpose:		Auto correlation analysis routine.
 * Comments:	Written using ModuleProducer version 1.1.
 *				This routine always sets the data->updateProcessFlag, to reset
 *				the output signal to zero.
 *				The exponetDt look up table needs to be set up as a process
 *				variable.
 *				12-03-97 LPO: Added DBL_EPSILON additions in CheckData for PC
 *				version.
 *				29-06-98 LPO: Introduced summation limit + maxLag < timeoffset
 *				limit check in CheckData.
 *				30-06-98 LPO: Introduced use of the SignalData structure's
 *				'numWindowFrames' field.
 *				10-11-98 LPO: Normalisation mode now uses 'NameSpecifier'.
 *				13 Jan 2003 LPO: Added options for the Weigrebe proportional
 *				time constant (Wiegrebe L. (2001) "Searching for the time
 *				constant of neural pitch extraction" JASA, 109, 1082-1091.
 * Author:		L. P. O'Mard
 * Created:		14 Nov 1995
 * Updated:		10 Nov 1998
 * Copyright:	(c) 1998, University of Essex
 *
 *********************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "FiParFile.h"
#include "AnAutoCorr.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

AutoCorrPtr	autoCorrPtr = NULL;

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
Free_Analysis_ACF(void)
{
	if (autoCorrPtr == NULL)
		return(FALSE);
	FreeProcessVariables_Analysis_ACF();
	if (autoCorrPtr->parList)
		FreeList_UniParMgr(&autoCorrPtr->parList);
	if (autoCorrPtr->parSpec == GLOBAL) {
		free(autoCorrPtr);
		autoCorrPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitNormalisationModeList *********************/

/*
 * This function initialises the 'normalisationMode' list array
 */

BOOLN
InitNormalisationModeList_Analysis_ACF(void)
{
	static NameSpecifier	modeList[] = {

			{ "NONE",		ANALYSIS_NORM_MODE_NONE },
			{ "STANDARD",	ANALYSIS_NORM_MODE_STANDARD },
			{ "UNITY",		ANALYSIS_NORM_MODE_UNITY },
			{ "",			ANALYSIS_NORM_MODE_NULL }
		};
	autoCorrPtr->normalisationModeList = modeList;
	return(TRUE);

}

/****************************** InitTimeConstModeList *************************/

/*
 * This function initialises the 'timeConstMode' list array
 */

BOOLN
InitTimeConstModeList_Analysis_ACF(void)
{
	static NameSpecifier	modeList[] = {

			{ "LICKLIDER",	ANALYSIS_ACF_TIMECONSTMODE_LICKLIDER },
			{ "WIEGREBE",	ANALYSIS_ACF_TIMECONSTMODE_WIEGREBE },
			{ "",			ANALYSIS_ACF_TIMECONSTMODE_NULL },
		};
	autoCorrPtr->timeConstModeList = modeList;
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
Init_Analysis_ACF(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_Analysis_ACF";

	if (parSpec == GLOBAL) {
		if (autoCorrPtr != NULL)
			Free_Analysis_ACF();
		if ((autoCorrPtr = (AutoCorrPtr) malloc(sizeof(AutoCorr))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (autoCorrPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	autoCorrPtr->parSpec = parSpec;
	autoCorrPtr->updateProcessVariablesFlag = TRUE;
	autoCorrPtr->normalisationModeFlag = TRUE;
	autoCorrPtr->timeConstModeFlag = TRUE;
	autoCorrPtr->timeOffsetFlag = TRUE;
	autoCorrPtr->timeConstantFlag = TRUE;
	autoCorrPtr->timeConstScaleFlag = TRUE;
	autoCorrPtr->maxLagFlag = TRUE;
	autoCorrPtr->normalisationMode = ANALYSIS_NORM_MODE_STANDARD;
	autoCorrPtr->timeConstMode = ANALYSIS_ACF_TIMECONSTMODE_LICKLIDER;
	autoCorrPtr->timeOffset = -1.0;
	autoCorrPtr->timeConstant = 0.0025;
	autoCorrPtr->timeConstScale = 2.0;
	autoCorrPtr->maxLag = 0.0075;

	InitNormalisationModeList_Analysis_ACF();
	InitTimeConstModeList_Analysis_ACF();
	if (!SetUniParList_Analysis_ACF()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_Analysis_ACF();
		return(FALSE);
	}
	autoCorrPtr->exponentDt = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_Analysis_ACF(void)
{
	static const char *funcName = "SetUniParList_Analysis_ACF";
	UniParPtr	pars;

	if ((autoCorrPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  ANALYSIS_ACF_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = autoCorrPtr->parList->pars;
	SetPar_UniParMgr(&pars[ANALYSIS_ACF_NORMALISATIONMODE], "MODE",
	  "Normalisation mode ('none', 'standard' or 'unity').",
	  UNIPAR_NAME_SPEC,
	  &autoCorrPtr->normalisationMode, autoCorrPtr->normalisationModeList,
	  (void * (*)) SetNormalisationMode_Analysis_ACF);
	SetPar_UniParMgr(&pars[ANALYSIS_ACF_TIMECONSTMODE], "T_CONST_MODE",
	  "Time constant mode ('Licklider' or 'Wiegrebe'.)",
	  UNIPAR_NAME_SPEC,
	  &autoCorrPtr->timeConstMode, autoCorrPtr->timeConstModeList,
	  (void * (*)) SetTimeConstMode_Analysis_ACF);
	SetPar_UniParMgr(&pars[ANALYSIS_ACF_TIMEOFFSET], "OFFSET",
	  "Time offset, t0 (if -ve the end of the signal is assumed) (s).",
	  UNIPAR_REAL,
	  &autoCorrPtr->timeOffset, NULL,
	  (void * (*)) SetTimeOffset_Analysis_ACF);
	SetPar_UniParMgr(&pars[ANALYSIS_ACF_TIMECONSTANT], "TIME_CONST",
	  "Time constant (or minimum time constant in 'Wiegrebe' mode) (s).",
	  UNIPAR_REAL,
	  &autoCorrPtr->timeConstant, NULL,
	  (void * (*)) SetTimeConstant_Analysis_ACF);
	SetPar_UniParMgr(&pars[ANALYSIS_ACF_TIMECONSTSCALE], "T_CONST_SCALE",
	  "Time constant scale (only used in 'Wiegrebe' mode).",
	  UNIPAR_REAL,
	  &autoCorrPtr->timeConstScale, NULL,
	  (void * (*)) SetTimeConstScale_Analysis_ACF);
	SetPar_UniParMgr(&pars[ANALYSIS_ACF_MAXLAG], "MAX_LAG",
	  "Maximum autocorrelation lag (s).",
	  UNIPAR_REAL,
	  &autoCorrPtr->maxLag, NULL,
	  (void * (*)) SetMaxLag_Analysis_ACF);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Analysis_ACF(void)
{
	static const char	*funcName = "GetUniParListPtr_Analysis_ACF";

	if (autoCorrPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (autoCorrPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(autoCorrPtr->parList);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_Analysis_ACF(char * normalisationMode, double timeOffset,
  double timeConstant, double maxLag)
{
	static const char	*funcName = "SetPars_Analysis_ACF";
	BOOLN	ok;

	ok = TRUE;
	if (!SetNormalisationMode_Analysis_ACF(normalisationMode))
		ok = FALSE;
	if (!SetTimeOffset_Analysis_ACF(timeOffset))
		ok = FALSE;
	if (!SetTimeConstant_Analysis_ACF(timeConstant))
		ok = FALSE;
	if (!SetMaxLag_Analysis_ACF(maxLag))
		ok = FALSE;
	if (!ok)
		NotifyError("%s: Failed to set all module parameters." ,funcName);
	return(ok);

}

/****************************** SetNormalisationMode **************************/

/*
 * This function sets the module's normalisationMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNormalisationMode_Analysis_ACF(char * theNormalisationMode)
{
	static const char	*funcName = "SetNormalisationMode_Analysis_ACF";
	int		specifier;

	if (autoCorrPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theNormalisationMode,
		autoCorrPtr->normalisationModeList)) == ANALYSIS_NORM_MODE_NULL) {
		NotifyError("%s: Illegal name (%s).", funcName, theNormalisationMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	autoCorrPtr->normalisationModeFlag = TRUE;
	autoCorrPtr->normalisationMode = specifier;
	return(TRUE);

}

/****************************** SetTimeConstMode ******************************/

/*
 * This function sets the module's timeConstMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTimeConstMode_Analysis_ACF(char * theTimeConstMode)
{
	static const char	*funcName = "SetTimeConstMode_Analysis_ACF";
	int		specifier;

	if (autoCorrPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theTimeConstMode,
		autoCorrPtr->timeConstModeList)) ==
		  ANALYSIS_ACF_TIMECONSTMODE_NULL) {
		NotifyError("%s: Illegal name (%s).", funcName, theTimeConstMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	autoCorrPtr->timeConstModeFlag = TRUE;
	autoCorrPtr->updateProcessVariablesFlag = TRUE;
	autoCorrPtr->timeConstMode = specifier;
	return(TRUE);

}

/****************************** SetTimeOffset *********************************/

/*
 * This function sets the module's timeOffset parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTimeOffset_Analysis_ACF(double theTimeOffset)
{
	static const char	*funcName = "SetTimeOffset_Analysis_ACF";

	if (autoCorrPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	autoCorrPtr->timeOffsetFlag = TRUE;
	autoCorrPtr->timeOffset = theTimeOffset;
	return(TRUE);

}

/****************************** SetTimeConstant *******************************/

/*
 * This function sets the module's timeConstant parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTimeConstant_Analysis_ACF(double theTimeConstant)
{
	static const char	*funcName = "SetTimeConstant_Analysis_ACF";

	if (autoCorrPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (theTimeConstant < 0.0) {
		NotifyError("%s: Value must be greater than zero (%g s)", funcName,
		  theTimeConstant);
		return(FALSE);
	}
	autoCorrPtr->timeConstantFlag = TRUE;
	autoCorrPtr->updateProcessVariablesFlag = TRUE;
	autoCorrPtr->timeConstant = theTimeConstant;
	return(TRUE);

}

/****************************** SetTimeConstScale *****************************/

/*
 * This function sets the module's timeConstScale parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTimeConstScale_Analysis_ACF(double theTimeConstScale)
{
	static const char	*funcName = "SetTimeConstScale_Analysis_ACF";

	if (autoCorrPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (theTimeConstScale < 0.0) {
		NotifyError("%s: The time constant scale must be greater than zero "
		  "(%g).", funcName, theTimeConstScale);
		return(FALSE);
	}
	autoCorrPtr->timeConstScaleFlag = TRUE;
	autoCorrPtr->updateProcessVariablesFlag = TRUE;
	autoCorrPtr->timeConstScale = theTimeConstScale;
	return(TRUE);

}

/****************************** SetMaxLag *************************************/

/*
 * This function sets the module's maxLag parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMaxLag_Analysis_ACF(double theMaxLag)
{
	static const char	*funcName = "SetMaxLag_Analysis_ACF";

	if (autoCorrPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (theMaxLag < 0.0) {
		NotifyError("%s: Value must be greater than zero (%g s)", funcName,
		  theMaxLag);
		return(FALSE);
	}
	autoCorrPtr->maxLagFlag = TRUE;
	autoCorrPtr->maxLag = theMaxLag;
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
CheckPars_Analysis_ACF(void)
{
	static const char	*funcName = "CheckPars_Analysis_ACF";
	BOOLN	ok;

	ok = TRUE;
	if (autoCorrPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!autoCorrPtr->normalisationModeFlag) {
		NotifyError("%s: normalisationMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!autoCorrPtr->timeConstModeFlag) {
		NotifyError("%s: timeConstMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!autoCorrPtr->timeOffsetFlag) {
		NotifyError("%s: timeOffset variable not set.", funcName);
		ok = FALSE;
	}
	if (!autoCorrPtr->timeConstantFlag) {
		NotifyError("%s: timeConstant variable not set.", funcName);
		ok = FALSE;
	}
	if (!autoCorrPtr->timeConstScaleFlag) {
		NotifyError("%s: timeConstScale variable not set.", funcName);
		ok = FALSE;
	}
	if (!autoCorrPtr->maxLagFlag) {
		NotifyError("%s: maxLag variable not set.", funcName);
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
PrintPars_Analysis_ACF(void)
{
	static const char	*funcName = "PrintPars_Analysis_ACF";

	if (!CheckPars_Analysis_ACF()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Auto-correlation analysis module parameters:-\n");
	DPrint("\tNormalisation mode = %s,",
	  autoCorrPtr->normalisationModeList[autoCorrPtr->normalisationMode].name);
	DPrint("\tTime constant mode = %s,\n", autoCorrPtr->timeConstModeList[
	  autoCorrPtr->timeConstMode].name);
	DPrint("\tTime offset = ");
	if (autoCorrPtr->timeOffset < 0.0)
		DPrint("end of signal,\n");
	else
		DPrint("%g ms,\n", MSEC(autoCorrPtr->timeOffset));
	if (autoCorrPtr->timeConstMode == ANALYSIS_ACF_TIMECONSTMODE_WIEGREBE) {
		DPrint("\tMinimum time constant = %g ms,", MSEC(autoCorrPtr->
		  timeConstant));
		DPrint("\tTime constant scale = %g (units),\n", autoCorrPtr->
		  timeConstScale);
	} else
		DPrint("\tTime constant = %g ms,\n", MSEC(autoCorrPtr->timeConstant));
	DPrint("\tMaximum lag = %g ms.\n", MSEC(autoCorrPtr->maxLag));
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_Analysis_ACF(char *fileName)
{
	static const char	*funcName = "ReadPars_Analysis_ACF";
	BOOLN	ok;
	char	*filePath;
	char	normalisationMode[MAXLINE];
	double	timeOffset, timeConstant, maxLag;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if (autoCorrPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((fp = fopen(filePath, "r")) == NULL) {
		NotifyError("%s: Cannot open data file '%s'.\n", funcName, filePath);
		return(FALSE);
	}
	DPrint("%s: Reading from '%s':\n", funcName, filePath);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, "%s", normalisationMode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &timeOffset))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &timeConstant))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &maxLag))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "
		  "parameter file '%s'.", funcName, filePath);
		return(FALSE);
	}
    if (!SetPars_Analysis_ACF(normalisationMode, timeOffset, timeConstant,
      maxLag)) {
		NotifyError("%s: Could not set parameters.", funcName);
		return(FALSE);
	}
	return(TRUE);

}

/************************** SetParsPointer ************************************/

/*
 * This routine sets the global parameter pointer for the module to that
 * associated with the module instance.
 */

BOOLN
SetParsPointer_Analysis_ACF(ModulePtr theModule)
{
	static const char *funcName = "SetParsPointer_Analysis_ACF";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	autoCorrPtr = (AutoCorrPtr) theModule->parsPtr;
	return(TRUE);

}

/************************** InitModule ****************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Analysis_ACF(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_Analysis_ACF";

	if (!SetParsPointer_Analysis_ACF(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_Analysis_ACF(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = autoCorrPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->CheckPars = CheckPars_Analysis_ACF;
	theModule->Free = Free_Analysis_ACF;
	theModule->GetUniParListPtr = GetUniParListPtr_Analysis_ACF;
	theModule->PrintPars = PrintPars_Analysis_ACF;
	theModule->ReadPars = ReadPars_Analysis_ACF;
	theModule->RunProcess = Calc_Analysis_ACF;
	theModule->SetParsPointer = SetParsPointer_Analysis_ACF;
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
 * A specific check for when timeOffset < 0.0 (set to signal duration) need
 * not be made.
 */

BOOLN
CheckData_Analysis_ACF(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_Analysis_ACF";
	double	signalDuration, timeOffset;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	signalDuration = _GetDuration_SignalData(data->inSignal[0]);
	if (autoCorrPtr->timeOffset > signalDuration + DBL_EPSILON) {
		NotifyError("%s: Time offset is longer than signal duration.",
		  funcName);
		return(FALSE);
	}		
	if (autoCorrPtr->maxLag > signalDuration + DBL_EPSILON) {
		NotifyError("%s: maximum auto-correlation lag is longer than signal "
		  "duration.", funcName);
		return(FALSE);
	}
	timeOffset = (autoCorrPtr->timeOffset < 0.0)? signalDuration:
	  autoCorrPtr->timeOffset;
	if (autoCorrPtr->maxLag > (timeOffset + DBL_EPSILON)) {
		NotifyError("%s: Time offset (%g ms) too for short maximum lag (%g "
		  "ms).", funcName, MILLI(autoCorrPtr->timeOffset), MILLI(autoCorrPtr->
		  maxLag));
		return(FALSE);
	}
	if ((autoCorrPtr->normalisationMode == ANALYSIS_NORM_MODE_UNITY) &&
	  (autoCorrPtr->timeConstMode == ANALYSIS_ACF_TIMECONSTMODE_WIEGREBE)) {
		NotifyError("%s: The 'unity' normalisation mode cannot be used with "
		  "the\n'Wiegrebe' tau mode.\n");
		return(FALSE);
	}
	return(TRUE);

}

/**************************** TimeConstant ************************************/

/*
 * This function calculates the Wiegrebe proportional time constant.
 * It assumes that the module has been correctly initialised.
 */

double
TimeConstant_Analysis_ACF(double lag)
{
	double	 timeConstant = lag * autoCorrPtr->timeConstScale;
	
	return((timeConstant > autoCorrPtr->timeConstant)? timeConstant:
	  autoCorrPtr->timeConstant);

}

/**************************** SunLimitIndex ***********************************/

/*
 * This function returns the summation limit index.
 * It assumes that the data object have been correctly initialised.
 */

ChanLen
SunLimitIndex_Analysis_ACF(EarObjectPtr data, double timeConstant)
{
	ChanLen	sumLimitIndex;

	sumLimitIndex = (ChanLen) floor(timeConstant / data->outSignal->dt + 0.5);
	return((sumLimitIndex < data->outSignal->length)? sumLimitIndex:
	  data->outSignal->length);

}

/**************************** InitProcessVariables ****************************/

/*
 * This routine initialises the exponential table.
 * It assumes that all of the parameters for the module have been correctly
 * initialised.
 */

BOOLN
InitProcessVariables_Analysis_ACF(EarObjectPtr data)
{
	static const char *funcName = "InitProcessVariables_Analysis_ACF";
	register double	*expDtPtr, dt;
	int		chan;
	double	minDecay;
	ChanLen	i, minLagIndex;
	AutoCorrPtr	p = autoCorrPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
		FreeProcessVariables_Analysis_ACF();
		if (p->timeConstMode == ANALYSIS_ACF_TIMECONSTMODE_WIEGREBE) {
			dt = data->inSignal[0]->dt;
			if ((p->exponentDt = (double *) calloc(p->maxLagIndex, sizeof(
			  double))) == NULL) {
				NotifyError("%s: Out of memory for exponent lookup table.",
				  funcName);
				return(FALSE);
			}
			minLagIndex = (ChanLen) floor(p->timeConstant / (dt *
			  p->timeConstScale) + 0.5);
			minDecay = exp(-dt / p->timeConstant);
			for (i = 0, expDtPtr = p->exponentDt; i < p->maxLagIndex; i++,
			  expDtPtr++)
				*expDtPtr = (i > minLagIndex)? exp(-1.0 / (i * p->
				  timeConstScale)): minDecay;
			
		}
		SetLocalInfoFlag_SignalData(data->outSignal, TRUE);
		SetStaticTimeFlag_SignalData(data->outSignal, TRUE);
		SetOutputTimeOffset_SignalData(data->outSignal, 0.0);
		for (chan = 0; chan < data->outSignal->numChannels; chan++)
			data->outSignal->info.cFArray[chan] = data->inSignal[0]->info.
			  cFArray[chan];
		SetInfoSampleTitle_SignalData(data->outSignal, "Delay Lag (s)");
		SetNumWindowFrames_SignalData(data->outSignal, 0);
		p->updateProcessVariablesFlag = FALSE;
	}
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

void
FreeProcessVariables_Analysis_ACF(void)
{
	if (autoCorrPtr->exponentDt) {
		free(autoCorrPtr->exponentDt);
		autoCorrPtr->exponentDt = NULL;
	}
	autoCorrPtr->updateProcessVariablesFlag = TRUE;

}

/****************************** Calc ******************************************/

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
 * The setting of the static 'expDtPtr' pointer is done outside of the
 * 'initThreadRun' for the sake of speed, i.e. because it is defined as a
 * register.
 */

BOOLN
Calc_Analysis_ACF(EarObjectPtr data)
{
	static const char	*funcName = "Calc_Analysis_ACF";
	register    double  *expDtPtr = NULL;
	register    ChanData    *inPtr, *outPtr;
	int		chan;
	double	wiegrebeTimeConst;
	ChanLen i, deltaT;
	AutoCorrPtr	p = autoCorrPtr;

	if (!data->threadRunFlag) {
		if (!CheckPars_Analysis_ACF())
			return(FALSE);
		if (!CheckData_Analysis_ACF(data)) {
			NotifyError("%s: Process data invalid.", funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, "Auto Correlation Function (ACF) "
		  "analysis");
		p->dt = data->inSignal[0]->dt;
		p->maxLagIndex = (ChanLen) floor(p->maxLag / p->dt + 0.5);
		if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels, 
		  p->maxLagIndex, p->dt)) {
			NotifyError("%s: Cannot initialise output channels.", funcName);
			return(FALSE);
		}
		if (!InitProcessVariables_Analysis_ACF(data)) {
			NotifyError("%s: Could not initialise the process variables.",
			  funcName);
			return(FALSE);
		}
		p->timeOffsetIndex = (ChanLen) ((p->timeOffset < 0.0)? data->inSignal[
		  0]->length: p->timeOffset / p->dt + 0.5);
		if (p->timeConstMode == ANALYSIS_ACF_TIMECONSTMODE_LICKLIDER) {
			p->sumLimitIndex = SunLimitIndex_Analysis_ACF(data,
			  p->timeConstant * 3.0);
			p->expDecay = exp(-p->dt / p->timeConstant);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	if (p->timeConstMode == ANALYSIS_ACF_TIMECONSTMODE_LICKLIDER)
		expDtPtr = &p->expDecay;
	for (chan = data->outSignal->offset; chan < data->inSignal[0]->numChannels;
	  chan++) {
		outPtr = data->outSignal->channel[chan];
		if (p->timeConstMode == ANALYSIS_ACF_TIMECONSTMODE_WIEGREBE)
			expDtPtr = p->exponentDt;
		for (deltaT = 0; deltaT < p->maxLagIndex; deltaT++, outPtr++) {
			if (p->timeConstMode == ANALYSIS_ACF_TIMECONSTMODE_WIEGREBE) {
				wiegrebeTimeConst = TimeConstant_Analysis_ACF(deltaT * p->dt);
				p->sumLimitIndex = SunLimitIndex_Analysis_ACF(data,
				  wiegrebeTimeConst);
			}
			inPtr = data->inSignal[0]->channel[chan] + (p->timeOffsetIndex -
			  p->sumLimitIndex);
			for (i = 0, *outPtr = 0.0; i < p->sumLimitIndex; i++, inPtr++)
				*outPtr =  (*outPtr * *expDtPtr) + (*inPtr * *(inPtr - deltaT));
			if (p->timeConstMode == ANALYSIS_ACF_TIMECONSTMODE_WIEGREBE) {
				expDtPtr++;
				*outPtr /= wiegrebeTimeConst;
			}
			switch(p->normalisationMode) {
			case ANALYSIS_NORM_MODE_STANDARD:
				*outPtr /= p->maxLagIndex;
				break;
			case ANALYSIS_NORM_MODE_UNITY:
				sqrt(*outPtr /= p->timeConstant);
			default:
				;
			} /* switch */
		}
	}
	data->outSignal->numWindowFrames++;
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

