/**********************
 *
 * File:		TrGate.c
 * Purpose:		This is the signal gate module transformation module.
 *				At changes the start or end envelope of a signal.
 *				Transformation modules do not create a signal, but change an
 *				existing one.  In order to preserve the processing line, the
 *				output signal is just pointed to the input signal.
 * Comments:	Written using ModuleProducer version 1.11 (Apr  9 1997).
 * 				Valid ramp functions are defined as a function which varies
 *				from 0 to 1 as a function of the ramp interval.
 *				The functions used must be defined in the format given below
 *				Impulse signals, such as clicks, should not be ramped: if a
 *				warning message is given, in this case ignore it.
 *				If a signal is ramped, then the ramp flag will be set.
 *				In segmented mode, this routine will only work for the first
 *				signal section.
 *				i.e. if the ramp interval has expired, then all subsequent
 *				segments will be marked as ramped.
 *				A duration = -1 denotes to the end of the signal.
 *				17-11-97 LPO: Changed name from TrRampOrDamp to TrGate and also
 *				introduced the Raised_Cos ramp mode.  This should be the ramp
 *				that we use by default.
 *				07-05-98 LPO: Corrected time offset usage in relative mode.
 * Author:		L. P. O'Mard
 * Created:		20 Aug 1997
 * Updated:		07 May 1998
 * Copyright:	(c) 1998, University of Essex.
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
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "FiParFile.h"
#include "TrGate.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

GatePtr	gatePtr = NULL;

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
Free_Transform_Gate(void)
{
	/* static const char	*funcName = "Free_Transform_Gate";  */

	if (gatePtr == NULL)
		return(FALSE);
	if (gatePtr->parList)
		FreeList_UniParMgr(&gatePtr->parList);
	if (gatePtr->parSpec == GLOBAL) {
		free(gatePtr);
		gatePtr = NULL;
	}
	return(TRUE);

}

/****************************** InitOperationModeList *************************/

/*
 * This routine intialises the Operation Mode list array.
 */

BOOLN
InitOperationModeList_Transform_Gate(void)
{
	static NameSpecifier	modeList[] = {

					{ "RAMP",	GATE_RAMP_OPERATION_MODE},
					{ "DAMP",	GATE_DAMP_OPERATION_MODE},
					{ "",		GATE_NULL_OPERATION_MODE}

				};
	gatePtr->operationModeList = modeList;
	return(TRUE);

}

/****************************** InitPositionModeList **************************/

/*
 * This routine intialises the Position Mode list array.
 */

BOOLN
InitPositionModeList_Transform_Gate(void)
{
	static NameSpecifier	modeList[] = {

					{ "ABSOLUTE",	GATE_ABSOLUTE_POSITION_MODE},
					{ "RELATIVE",	GATE_RELATIVE_POSITION_MODE},
					{ "",			GATE_NULL_POSITION_MODE}

				};
	gatePtr->positionModeList = modeList;
	return(TRUE);

}

/****************************** InitTypeModeList ******************************/

/*
 * This routine intialises the Type Mode list array.
 */

BOOLN
InitTypeModeList_Transform_Gate(void)
{
	static NameSpecifier	modeList[] = {

					{ "LINEAR",		GATE_LINEAR_TYPE_MODE},
					{ "SINE",		GATE_SINE_TYPE_MODE},
					{ "RAISED_COS",	GATE_RAISED_COS_TYPE_MODE},
					{ "EXP_DECAY",	GATE_EXP_DECAY_TYPE_MODE},
					{ "",			GATE_NULL_TYPE_MODE}

				};
	gatePtr->typeModeList = modeList;
	return(TRUE);

}

/****************************** InitProcessModeList ***************************/

/*
 * This routine intialises the Type Mode list array.
 */

BOOLN
InitProcessModeList_Transform_Gate(void)
{
	static NameSpecifier	modeList[] = {

					{ "TRANS_GATE",	GATE_STANDARD_PROCESS_MODE},
					{ "TRANS_RAMP",	GATE_RAMP_PROCESS_MODE},
					{ "TRANS_DAMP",	GATE_DAMP_PROCESS_MODE},
					{ "",			GATE_NULL_TYPE_MODE}

				};
	gatePtr->processModeList = modeList;
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
Init_Transform_Gate(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_Transform_Gate";

	if (parSpec == GLOBAL) {
		if (gatePtr != NULL)
			Free_Transform_Gate();
		if ((gatePtr = (GatePtr) malloc(sizeof(Gate))) ==
		  NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (gatePtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	gatePtr->parSpec = parSpec;
	gatePtr->positionModeFlag = TRUE;
	gatePtr->operationModeFlag = TRUE;
	gatePtr->typeModeFlag = TRUE;
	gatePtr->timeOffsetFlag = TRUE;
	gatePtr->durationFlag = TRUE;
	gatePtr->slopeParameterFlag = TRUE;
	gatePtr->positionMode = GATE_RELATIVE_POSITION_MODE;
	gatePtr->operationMode = GATE_RAMP_OPERATION_MODE;
	gatePtr->typeMode = GATE_RAISED_COS_TYPE_MODE;
	gatePtr->timeOffset = 0.0;
	gatePtr->duration = 2.5e-3;
	gatePtr->slopeParameter = 0.016;

	InitPositionModeList_Transform_Gate();
	InitOperationModeList_Transform_Gate();
	InitTypeModeList_Transform_Gate();
	InitProcessModeList_Transform_Gate();
	if (!SetUniParList_Transform_Gate()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_Transform_Gate();
		return(FALSE);
	}
	return(TRUE);

}

/********************************* SetUniParList ******************************/

/*
 * This routine initialises and sets the module's universal parameter list.
 * This list provides universal access to the module's parameters.
 */
 
BOOLN
SetUniParList_Transform_Gate(void)
{
	static const char *funcName = "SetUniParList_Transform_Gate";
	UniParPtr	pars;

	if ((gatePtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  GATE_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = gatePtr->parList->pars;
	SetPar_UniParMgr(&pars[GATE_POSITION_MODE], "POS_MODE",
	 "Position mode ('absolute' or 'relative')",
	 UNIPAR_NAME_SPEC,
	 &gatePtr->positionMode, gatePtr->positionModeList,
	 (void * (*)) SetPositionMode_Transform_Gate);
	SetPar_UniParMgr(&pars[GATE_OPERATION_MODE], "OP_MODE",
	  "Operation mode ('ramp' or 'damp')",
	  UNIPAR_NAME_SPEC,
	  &gatePtr->operationMode, gatePtr->operationModeList,
	  (void * (*)) SetOperationMode_Transform_Gate);
	SetPar_UniParMgr(&pars[GATE_TYPE_MODE], "TYPE_MODE",
	  "Type mode ('linear', 'sine', 'raised_cos' or 'exp_decay')",
	  UNIPAR_NAME_SPEC,
	  &gatePtr->typeMode, gatePtr->typeModeList,
	  (void * (*)) SetTypeMode_Transform_Gate);
	SetPar_UniParMgr(&pars[GATE_TIME_OFFSET], "OFFSET",
	  "Time offset (s)",
	  UNIPAR_REAL, &gatePtr->timeOffset, NULL,
	  (void * (*)) SetTimeOffset_Transform_Gate);
	SetPar_UniParMgr(&pars[GATE_DURATION], "DURATION",
	  "Ramp duration - negative assumes to the end of the signal (s)", 
	  UNIPAR_REAL,
	  &gatePtr->duration, NULL,
	  (void * (*)) SetDuration_Transform_Gate);
	SetPar_UniParMgr(&pars[GATE_SLOPE_PARAMETER], "SLOPE",
	  "Slope parameter or half-life (in 'decay_par' mode only)",
	  UNIPAR_REAL,
	  &gatePtr->slopeParameter, NULL,
	  (void * (*)) SetSlopeParameter_Transform_Gate);

	SetDefaulEnabledPars_Transform_Gate();
	return(TRUE);

}

/********************************* SetDefaulEnabledPars ***********************/

/*
 * This routine sets the parameter list so that the correct default parameters
 * are enabled/disabled.
 */

BOOLN
SetDefaulEnabledPars_Transform_Gate(void)
{
	static const char *funcName = "SetDefaulEnabledPars_Transform_Gate";

	if (gatePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	switch (gatePtr->typeMode) {
	case GATE_EXP_DECAY_TYPE_MODE:
		gatePtr->parList->pars[GATE_SLOPE_PARAMETER].enabled = TRUE;
		break;
	default:
		gatePtr->parList->pars[GATE_SLOPE_PARAMETER].enabled = FALSE;
	}
	return(TRUE);

}

/********************************* GetUniParListPtr ***************************/

/*
 * This routine returns a pointer to the module's universal parameter list.
 */

UniParListPtr
GetUniParListPtr_Transform_Gate(void)
{
	static const char *funcName = "GetUniParListPtr_Transform_Gate";

	if (gatePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (gatePtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(gatePtr->parList);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_Transform_Gate(char *positionMode, char *operationMode,
  char *typeMode, double timeOffset, double duration, double slopeParameter)
{
	static const char	*funcName = "SetPars_Transform_Gate";
	BOOLN	ok;

	ok = TRUE;
	if (!SetPositionMode_Transform_Gate(positionMode))
		ok = FALSE;
	if (!SetOperationMode_Transform_Gate(operationMode))
		ok = FALSE;
	if (!SetTypeMode_Transform_Gate(typeMode))
		ok = FALSE;
	if (!SetTimeOffset_Transform_Gate(timeOffset))
		ok = FALSE;
	if (!SetDuration_Transform_Gate(duration))
		ok = FALSE;
	if (!SetSlopeParameter_Transform_Gate(slopeParameter))
		ok = FALSE;
	if (!ok)
		NotifyError("%s: Failed to set all module parameters." ,funcName);
	return(ok);

}

/****************************** SetPositionMode *******************************/

/*
 * This function sets the module's positionMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPositionMode_Transform_Gate(char *thePositionMode)
{
	static const char	*funcName = "SetPositionMode_Transform_Gate";
	int		specifier;

	if (gatePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(thePositionMode,
	  gatePtr->positionModeList)) == GATE_NULL_POSITION_MODE) {
		NotifyError("%s: Illegal mode name (%s).", funcName, thePositionMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	gatePtr->positionModeFlag = TRUE;
	gatePtr->positionMode = specifier;
	gatePtr->parList->pars[GATE_TIME_OFFSET].enabled = (gatePtr->positionMode !=
	  GATE_RELATIVE_POSITION_MODE);
	gatePtr->parList->updateFlag = TRUE;
	return(TRUE);

}

/****************************** SetOperationMode ******************************/

/*
 * This function sets the module's operationMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOperationMode_Transform_Gate(char *theOperationMode)

{
	static const char	*funcName = "SetOperationMode_Transform_Gate";
	int		specifier;

	if (gatePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theOperationMode,
	  gatePtr->operationModeList)) == GATE_NULL_OPERATION_MODE) {
		NotifyError("%s: Illegal mode name (%s).", funcName, theOperationMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	gatePtr->operationModeFlag = TRUE;
	gatePtr->operationMode = specifier;
	return(TRUE);

}

/****************************** SetTypeMode ***********************************/

/*
 * This function sets the module's typeMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTypeMode_Transform_Gate(char *theTypeMode)
{
	static const char	*funcName = "SetTypeMode_Transform_Gate";
	int		specifier;

	if (gatePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theTypeMode,
	  gatePtr->typeModeList)) == GATE_NULL_TYPE_MODE) {
		NotifyError("%s: Illegal mode name (%s).", funcName, theTypeMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	gatePtr->typeModeFlag = TRUE;
	gatePtr->typeMode = specifier;
	SetDefaulEnabledPars_Transform_Gate();
	gatePtr->parList->updateFlag = TRUE;
	return(TRUE);

}

/****************************** SetTimeOffset *********************************/

/*
 * This function sets the module's timeOffset parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTimeOffset_Transform_Gate(double theTimeOffset)
{
	static const char	*funcName = "SetTimeOffset_Transform_Gate";

	if (gatePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (theTimeOffset < 0.0) {
		NotifyError("%s: Illegal time offset (%g ms).", funcName,
		  MSEC(theTimeOffset));
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	gatePtr->timeOffsetFlag = TRUE;
	gatePtr->timeOffset = theTimeOffset;
	return(TRUE);

}

/****************************** SetDuration ***********************************/

/*
 * This function sets the module's duration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDuration_Transform_Gate(double theDuration)
{
	static const char	*funcName = "SetDuration_Transform_Gate";

	if (gatePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (fabs(theDuration) < DBL_EPSILON) {
		NotifyError("%s: Time interval must be non-zero.", funcName);
		return(FALSE);
	}
	gatePtr->durationFlag = TRUE;
	gatePtr->duration = theDuration;
	return(TRUE);

}

/****************************** SetSlopeParameter *****************************/

/*
 * This function sets the module's slopeParameter parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSlopeParameter_Transform_Gate(double theSlopeParameter)
{
	static const char	*funcName = "SetSlopeParameter_Transform_Gate";

	if (gatePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	gatePtr->slopeParameterFlag = TRUE;
	gatePtr->slopeParameter = theSlopeParameter;
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
CheckPars_Transform_Gate(void)
{
	static const char	*funcName = "CheckPars_Transform_Gate";
	BOOLN	ok;

	ok = TRUE;
	if (gatePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!gatePtr->positionModeFlag) {
		NotifyError("%s: positionMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!gatePtr->operationModeFlag) {
		NotifyError("%s: operationMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!gatePtr->typeModeFlag) {
		NotifyError("%s: typeMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!gatePtr->timeOffsetFlag) {
		NotifyError("%s: timeOffset variable not set.", funcName);
		ok = FALSE;
	}
	if (!gatePtr->durationFlag) {
		NotifyError("%s: duration variable not set.", funcName);
		ok = FALSE;
	}
	if (!gatePtr->slopeParameterFlag) {
		NotifyError("%s: slopeParameter variable not set.", funcName);
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
PrintPars_Transform_Gate(void)
{
	static const char	*funcName = "PrintPars_Transform_Gate";

	if (!CheckPars_Transform_Gate()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Gate Transformation Module Parameters:-\n");
	DPrint("\tPosition mode: %s,",
	  gatePtr->positionModeList[gatePtr->positionMode].name);
	DPrint("\tOperation mode: %s,\n",
	  gatePtr->operationModeList[gatePtr->operationMode].name);
	DPrint("\tType mode: %s,",
	  gatePtr->typeModeList[gatePtr->typeMode].name);
	DPrint("\tTime offset");
	if (gatePtr->positionMode == GATE_ABSOLUTE_POSITION_MODE)
		DPrint(" = %g ms,\n", MSEC(gatePtr->timeOffset));
	else
		DPrint(": unused.\n");
	DPrint("\tRamp duration = ");
	if (gatePtr->duration < 0.0)
		DPrint("signal extent,");
	else
		DPrint("%g ms,", MSEC(gatePtr->duration));
	DPrint("\tSlope parameter");
	if (gatePtr->typeMode == GATE_EXP_DECAY_TYPE_MODE)
		DPrint(" (half-life) = %g ms.\n", MSEC(gatePtr->slopeParameter));
	else
		DPrint(": unused.\n");
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_Transform_Gate(char *fileName)
{
	static const char	*funcName = "ReadPars_Transform_Gate";
	BOOLN	ok;
	char	*filePath;
	char	positionMode[SMALL_STRING], operationMode[SMALL_STRING];
	char	typeMode[SMALL_STRING];
	double	timeOffset, duration, slopeParameter;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(filePath, "r")) == NULL) {
		NotifyError("%s: Cannot open data file '%s'.\n", funcName, filePath);
		return(FALSE);
	}
	DPrint("%s: Reading from '%s':\n", funcName, filePath);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, "%s", positionMode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%s", operationMode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%s", typeMode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &timeOffset))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &duration))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &slopeParameter))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "
		  "parameter file '%s'.", funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_Transform_Gate(positionMode, operationMode, typeMode,
	  timeOffset, duration, slopeParameter)) {
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
SetParsPointer_Transform_Gate(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_Transform_Gate";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	gatePtr = (GatePtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Transform_Gate(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_Transform_Gate";

	if (!SetParsPointer_Transform_Gate(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_Transform_Gate(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = gatePtr;
	SetDefault_ModuleMgr(theModule, TrueFunction_ModuleMgr);
	theModule->CheckPars = CheckPars_Transform_Gate;
	theModule->Free = Free_Transform_Gate;
	theModule->GetUniParListPtr = GetUniParListPtr_Transform_Gate;
	theModule->PrintPars = PrintPars_Transform_Gate;
	theModule->ReadPars = ReadPars_Transform_Gate;
	theModule->RunProcess = Process_Transform_Gate;
	theModule->SetParsPointer = SetParsPointer_Transform_Gate;
	gatePtr->processMode = Identify_NameSpecifier(theModule->name, gatePtr->
	  processModeList);
	switch (gatePtr->processMode) {
	case GATE_RAMP_PROCESS_MODE:
		SetOperationMode_Transform_Gate("RAMP");
		gatePtr->parList->pars[GATE_OPERATION_MODE].enabled = FALSE;
		break;
	case GATE_DAMP_PROCESS_MODE:
		SetOperationMode_Transform_Gate("DAMP");
		gatePtr->parList->pars[GATE_OPERATION_MODE].enabled = FALSE;
		break;
	default:
		;
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
CheckData_Transform_Gate(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_Transform_Gate";
	double	signalDuration, duration;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	signalDuration = _GetDuration_SignalData(data->inSignal[0]);
	duration = (gatePtr->duration < 0.0)? signalDuration - gatePtr->timeOffset:
	  gatePtr->duration;
 	if (duration > signalDuration) {
		NotifyError("%s: Gate time interval (%g ms) is longer than signal "
		  "(%g ms).", funcName, MSEC(duration), signalDuration);
		return(FALSE);
	}
	if ((gatePtr->positionMode == GATE_ABSOLUTE_POSITION_MODE) &&
	  (gatePtr->timeOffset + duration - signalDuration) > DBL_EPSILON) {
		NotifyError("%s: Time offset (%g ms) plus gate interval (%g ms) is "
		  "longer than signal (%g ms).", funcName, MSEC(gatePtr->timeOffset),
		  MSEC(duration), MSEC(signalDuration));
		return(FALSE);
	}
	if ((gatePtr->typeMode == GATE_EXP_DECAY_TYPE_MODE) &&
	  (gatePtr->slopeParameter <= 0.0)) {
		NotifyError("%s: Illegal slope parameter (half-life) value (%g ms).",
		  funcName, MSEC(gatePtr->slopeParameter));
		return(FALSE);
	}
	return(TRUE);

}

/****************************** RampFunction **********************************/

/*
 * This function returns the result of the ramp function.
 * It assumes that a legal type mode is set.
 * This routine returns zero at the function end because the SGI compiler
 * complains if it is not there.
 */

double
GateFunction_Transform_Gate(ChanLen step, ChanLen intervalIndex,
  double dt)
{
	switch(gatePtr->typeMode) {
	case GATE_LINEAR_TYPE_MODE:
		return((double) step / intervalIndex);
	case GATE_SINE_TYPE_MODE:
		return(sin(step * PI / 2.0 / intervalIndex));
	case GATE_RAISED_COS_TYPE_MODE:
		return((cos(PI * ((double) step / intervalIndex + 1.0)) + 1.0) / 2.0);
	case GATE_EXP_DECAY_TYPE_MODE:
		return(exp(-dt * step * LN_2 / gatePtr->slopeParameter));
	default:
		;
	}/* switch */
	return(0.0);

}

/****************************** Ramp ******************************************/

/*
 * This routine ramps a signal.
 */

void
Ramp_Transform_Gate(EarObjectPtr data, ChanLen offsetIndex,
  ChanLen intervalIndex)
{
	/* static const char *funcName = "Ramp_Transform_Gate"; */
	int		chan;
	ChanLen	i;
	ChanData	*dataPtr, *endPtr;
	
	for (chan = 0; chan < data->outSignal->numChannels; chan++) {
		dataPtr = data->outSignal->channel[chan];
		endPtr = dataPtr + data->outSignal->length - 1;
		if (gatePtr->positionMode == GATE_ABSOLUTE_POSITION_MODE)
			dataPtr += offsetIndex;
		else
			for ( ; ((dataPtr + 1) < endPtr) && (fabs(*dataPtr -
			  *(dataPtr + 1)) < DBL_EPSILON); dataPtr++)
				;
		for (i = 0; (i < intervalIndex) && (dataPtr < endPtr); i++)
			*dataPtr++ *= GateFunction_Transform_Gate(i, intervalIndex,
			  data->outSignal->dt);
	}

}

/****************************** Damp ******************************************/

/*
 * This routine Damps a signal.
 */

void
Damp_Transform_Gate(EarObjectPtr data, ChanLen offsetIndex,
  ChanLen intervalIndex)
{
	register ChanData	*dataPtr, *startPtr;
	int		chan;
	ChanLen	i;

	for (chan = 0; chan < data->outSignal->numChannels; chan++) {
		startPtr = data->outSignal->channel[chan];
		dataPtr = startPtr + data->outSignal->length - 1;
		if (gatePtr->positionMode == GATE_ABSOLUTE_POSITION_MODE)
			dataPtr -= offsetIndex;
		else
			for ( ; ((dataPtr - 1) > startPtr) && (fabs(*dataPtr - *(dataPtr -
			  1)) < DBL_EPSILON); dataPtr--)
				;
		for (i = 0; (i < intervalIndex) && ( dataPtr > startPtr); i++)
			*dataPtr-- *= GateFunction_Transform_Gate(i, intervalIndex,
			  data->outSignal->dt);
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
 * The output signal is manually connected to the input signal.
 * It assumes that a legal operation mode is set.
 * The "exp_decay" type mode requires the "ramp" and "damp" operations to be
 * switched.
 */

BOOLN
Process_Transform_Gate(EarObjectPtr data)
{
	static const char	*funcName = "Process_Transform_Gate";
	int		operationMode;
	ChanLen	intervalIndex, offsetIndex;

	if (!CheckPars_Transform_Gate())
		return(FALSE);
	if (!CheckData_Transform_Gate(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "Gate Module process");
	if (data->outSignal != data->inSignal[0]) {
		data->outSignal = data->inSignal[0];
		data->updateCustomersFlag = TRUE;
	}
	data->outSignal->rampFlag = TRUE;
	offsetIndex = (gatePtr->positionMode == GATE_ABSOLUTE_POSITION_MODE)?
	  (ChanLen) floor(gatePtr->timeOffset / data->outSignal->dt + 0.5): 0;
	intervalIndex = (gatePtr->duration < 0.0)?
	  data->inSignal[0]->length: (ChanLen) floor(gatePtr->duration /
	  data->outSignal->dt + 0.5);
	if (data->timeIndex < (intervalIndex + offsetIndex)) {
		operationMode = gatePtr->operationMode;
		if (gatePtr->typeMode == GATE_EXP_DECAY_TYPE_MODE)
			operationMode = (operationMode == GATE_RAMP_OPERATION_MODE)?
			  GATE_DAMP_OPERATION_MODE: GATE_RAMP_OPERATION_MODE;

		switch (operationMode) {
		case GATE_RAMP_OPERATION_MODE:
			Ramp_Transform_Gate(data, offsetIndex, intervalIndex);
			break;
		case GATE_DAMP_OPERATION_MODE:
			Damp_Transform_Gate(data, offsetIndex, intervalIndex);
			break;
		default:
			;
		}
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

