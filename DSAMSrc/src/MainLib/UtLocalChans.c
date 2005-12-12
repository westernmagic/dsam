/**********************
 *
 * File:		UtLocalChans.c
 * Purpose:		It calculates measures of a multi-channel input, grouping
 *				the channels according to a specified algorithm.
 * Comments:	Written using ModuleProducer version 1.3.3 (Oct  1 2001).
 * Author:		L. P. O'Mard
 * Created:		17 Jan 2002
 * Updated:	
 * Copyright:	(c) 2002, CNBH, University of Essex
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
#include "FiParFile.h"
#include "UtLocalChans.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

LocalChansPtr	localChansPtr = NULL;

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
Free_Utility_LocalChans(void)
{
	/* static const char	*funcName = "Free_Utility_LocalChans";  */

	if (localChansPtr == NULL)
		return(FALSE);
	if (localChansPtr->parList)
		FreeList_UniParMgr(&localChansPtr->parList);
	if (localChansPtr->parSpec == GLOBAL) {
		free(localChansPtr);
		localChansPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitModeList **********************************/

/*
 * This function initialises the 'mode' list array
 */

BOOLN
InitModeList_Utility_LocalChans(void)
{
	static NameSpecifier	modeList[] = {

			{ "SUM",		UTILITY_LOCALCHANS_MODE_SUM },
			{ "AVERAGE",	UTILITY_LOCALCHANS_MODE_AVERAGE },
			{ "",			UTILITY_LOCALCHANS_MODE_NULL },
		};
	localChansPtr->modeList = modeList;
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
Init_Utility_LocalChans(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_Utility_LocalChans";

	if (parSpec == GLOBAL) {
		if (localChansPtr != NULL)
			Free_Utility_LocalChans();
		if ((localChansPtr = (LocalChansPtr) malloc(sizeof(LocalChans))) ==
		  NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (localChansPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	localChansPtr->parSpec = parSpec;
	localChansPtr->modeFlag = TRUE;
	localChansPtr->limitModeFlag = TRUE;
	localChansPtr->lowerLimitFlag = TRUE;
	localChansPtr->upperLimitFlag = TRUE;
	localChansPtr->mode = UTILITY_LOCALCHANS_MODE_SUM;
	localChansPtr->limitMode = SIGNALDATA_LIMIT_MODE_OCTAVE;
	localChansPtr->lowerLimit = -1.0;
	localChansPtr->upperLimit = 1.0;

	InitModeList_Utility_LocalChans();
	if (!SetUniParList_Utility_LocalChans()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_Utility_LocalChans();
		return(FALSE);
	}
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_Utility_LocalChans(void)
{
	static const char *funcName = "SetUniParList_Utility_LocalChans";
	UniParPtr	pars;

	if ((localChansPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  UTILITY_LOCALCHANS_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = localChansPtr->parList->pars;
	SetPar_UniParMgr(&pars[UTILITY_LOCALCHANS_MODE], "MODE",
	  "Operation mode ('sum' or 'average').",
	  UNIPAR_NAME_SPEC,
	  &localChansPtr->mode, localChansPtr->modeList,
	  (void * (*)) SetMode_Utility_LocalChans);
	SetPar_UniParMgr(&pars[UTILITY_LOCALCHANS_LIMITMODE], "LIMIT_MODE",
	  "Limit mode ('octave' or 'channel').",
	  UNIPAR_NAME_SPEC,
	  &localChansPtr->limitMode, LimitModeList_SignalData(0),
	  (void * (*)) SetLimitMode_Utility_LocalChans);
	SetPar_UniParMgr(&pars[UTILITY_LOCALCHANS_LOWERAVELIMIT],
	  "LOWER_AVE_LIMIT",
	  "Local window limit below channel.",
	  UNIPAR_REAL,
	  &localChansPtr->lowerLimit, NULL,
	  (void * (*)) SetLowerLimit_Utility_LocalChans);
	SetPar_UniParMgr(&pars[UTILITY_LOCALCHANS_UPPERAVELIMIT],
	  "UPPER_AVE_LIMIT",
	  "Local window limit above channel.",
	  UNIPAR_REAL,
	  &localChansPtr->upperLimit, NULL,
	  (void * (*)) SetUpperLimit_Utility_LocalChans);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Utility_LocalChans(void)
{
	static const char	*funcName = "GetUniParListPtr_Utility_LocalChans";

	if (localChansPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (localChansPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(localChansPtr->parList);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_Utility_LocalChans(char * mode, char * limitMode, double lowerLimit,
  double upperLimit)
{
	static const char	*funcName = "SetPars_Utility_LocalChans";
	BOOLN	ok;

	ok = TRUE;
	if (!SetMode_Utility_LocalChans(mode))
		ok = FALSE;
	if (!SetLimitMode_Utility_LocalChans(limitMode))
		ok = FALSE;
	if (!SetLowerLimit_Utility_LocalChans(lowerLimit))
		ok = FALSE;
	if (!SetUpperLimit_Utility_LocalChans(upperLimit))
		ok = FALSE;
	if (!ok)
		NotifyError("%s: Failed to set all module parameters." ,funcName);
	return(ok);

}

/****************************** SetMode ***************************************/

/*
 * This function sets the module's mode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMode_Utility_LocalChans(char * theMode)
{
	static const char	*funcName = "SetMode_Utility_LocalChans";
	int		specifier;

	if (localChansPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theMode,
		localChansPtr->modeList)) == UTILITY_LOCALCHANS_MODE_NULL) {
		NotifyError("%s: Illegal name (%s).", funcName, theMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	localChansPtr->modeFlag = TRUE;
	localChansPtr->mode = specifier;
	return(TRUE);

}

/****************************** SetLimitMode **********************************/

/*
 * This function sets the module's limitMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLimitMode_Utility_LocalChans(char * theLimitMode)
{
	static const char	*funcName = "SetLimitMode_Utility_LocalChans";
	int		specifier;

	if (localChansPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theLimitMode,
	  LimitModeList_SignalData(0))) == SIGNALDATA_LIMIT_MODE_NULL) {
		NotifyError("%s: Illegal name (%s).", funcName, theLimitMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	localChansPtr->limitModeFlag = TRUE;
	localChansPtr->limitMode = specifier;
	return(TRUE);

}

/****************************** SetLowerLimit *********************************/

/*
 * This function sets the module's lowerLimit parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLowerLimit_Utility_LocalChans(double theLowerLimit)
{
	static const char	*funcName = "SetLowerLimit_Utility_LocalChans";

	if (localChansPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	localChansPtr->lowerLimitFlag = TRUE;
	localChansPtr->lowerLimit = theLowerLimit;
	return(TRUE);

}

/****************************** SetUpperLimit *********************************/

/*
 * This function sets the module's upperLimit parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetUpperLimit_Utility_LocalChans(double theUpperLimit)
{
	static const char	*funcName = "SetUpperLimit_Utility_LocalChans";

	if (localChansPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	localChansPtr->upperLimitFlag = TRUE;
	localChansPtr->upperLimit = theUpperLimit;
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
CheckPars_Utility_LocalChans(void)
{
	static const char	*funcName = "CheckPars_Utility_LocalChans";
	BOOLN	ok;

	ok = TRUE;
	if (!localChansPtr->modeFlag) {
		NotifyError("%s: mode variable not set.", funcName);
		ok = FALSE;
	}
	if (localChansPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!localChansPtr->limitModeFlag) {
		NotifyError("%s: limitMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!localChansPtr->lowerLimitFlag) {
		NotifyError("%s: lowerLimit variable not set.", funcName);
		ok = FALSE;
	}
	if (!localChansPtr->upperLimitFlag) {
		NotifyError("%s: upperLimit variable not set.", funcName);
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
PrintPars_Utility_LocalChans(void)
{
	static const char	*funcName = "PrintPars_Utility_LocalChans";

	if (!CheckPars_Utility_LocalChans()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Channel localisation Utility Module Parameters:-\n");
	DPrint("\tOperation mode = %s,", localChansPtr->modeList[localChansPtr->
	  mode].name);
	DPrint("\tLimit mode = %s,\n", LimitModeList_SignalData(localChansPtr->
	  limitMode)->name);
	DPrint("\tLower window limit below channel = %g (%s),\n", localChansPtr->
	  lowerLimit, LimitModeList_SignalData(localChansPtr->limitMode)->name);
	DPrint("\tUpper window limit above channel = %g (%s).\n", localChansPtr->
	  upperLimit, LimitModeList_SignalData(localChansPtr->limitMode)->name);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_Utility_LocalChans(char *fileName)
{
	static const char	*funcName = "ReadPars_Utility_LocalChans";
	BOOLN	ok = TRUE;
	char	*filePath, mode[MAX_FILE_PATH], limitMode[MAX_FILE_PATH];
	double	lowerLimit, upperLimit;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(filePath, "r")) == NULL) {
		NotifyError("%s: Cannot open data file '%s'.\n", funcName, fileName);
		return(FALSE);
	}
	DPrint("%s: Reading from '%s':\n", funcName, fileName);
	Init_ParFile();
	if (!GetPars_ParFile(fp, "%s", mode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%s", limitMode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &lowerLimit))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &upperLimit))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "
		  "parameter file '%s'.", funcName, fileName);
		return(FALSE);
	}
	if (!SetPars_Utility_LocalChans(mode, limitMode, lowerLimit, upperLimit)) {
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
SetParsPointer_Utility_LocalChans(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_Utility_LocalChans";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	localChansPtr = (LocalChansPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 * It also initialises the process structure.
 */

BOOLN
InitModule_Utility_LocalChans(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_Utility_LocalChans";

	if (!SetParsPointer_Utility_LocalChans(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_Utility_LocalChans(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = localChansPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->CheckPars = CheckPars_Utility_LocalChans;
	theModule->Free = Free_Utility_LocalChans;
	theModule->GetUniParListPtr = GetUniParListPtr_Utility_LocalChans;
	theModule->PrintPars = PrintPars_Utility_LocalChans;
	theModule->ReadPars = ReadPars_Utility_LocalChans;
	theModule->ResetProcess = ResetProcess_Utility_LocalChans;
	theModule->RunProcess = Calc_Utility_LocalChans;
	theModule->SetParsPointer = SetParsPointer_Utility_LocalChans;
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
CheckData_Utility_LocalChans(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_Utility_LocalChans";

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	if (data->inSignal[0]->numChannels < 2) {
		NotifyError("%s: This module expects multi-channel input.", funcName);
		return(FALSE);
	}
	/*** Put additional checks here. ***/
	return(TRUE);

}

/**************************** ResetProcess ************************************/

/*
 * This routine resets the process variables.
 */

void
ResetProcess_Utility_LocalChans(EarObjectPtr data)
{
	ResetOutSignal_EarObject(data);

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
 */

BOOLN
Calc_Utility_LocalChans(EarObjectPtr data)
{
	static const char	*funcName = "Calc_Utility_LocalChans";
	register ChanData	 *inPtr, *outPtr;
	char	channelTitle[MAXLINE];
	int		outChan, inChan, maxChan, lowerChanLimit, upperChanLimit;
	int		numChannels;
	ChanLen	i;
	LocalChansPtr	p = localChansPtr;

	if (!data->threadRunFlag) {
		if (!CheckPars_Utility_LocalChans())
			return(FALSE);
		if (!CheckData_Utility_LocalChans(data)) {
			NotifyError("%s: Process data invalid.", funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, "Channel localisation Module process");

		if (!GetChannelLimits_SignalData(data->inSignal[0], &p->minChan,
		  &maxChan, p->lowerLimit, p->upperLimit, p->limitMode)) {
			NotifyError("%s: Could not find a channel limits for signal.",
			  funcName);
			return(FALSE);
		}
		if (!InitOutSignal_EarObject(data, (uShort) (maxChan - p->minChan + 1),
		  data->inSignal[0]->length, data->inSignal[0]->dt)) {
			NotifyError("%s: Cannot initialise output channels.", funcName);
			return(FALSE);
		}
		ResetProcess_Utility_LocalChans(data);
		SetLocalInfoFlag_SignalData(data->outSignal, TRUE);
		snprintf(channelTitle, MAXLINE, "Averaged channels ('%s' mode)",
		  LimitModeList_SignalData(p->limitMode)->name);
		SetInfoChannelTitle_SignalData(data->outSignal, channelTitle);
		SetInfoSampleTitle_SignalData(data->outSignal, data->inSignal[0]->info.
		  sampleTitle);
		if (data->initThreadRunFlag)
			return(TRUE);
	}

	for (outChan = data->outSignal->offset; outChan < data->outSignal->
	  numChannels; outChan++) {
		inChan = p->minChan + outChan;
		data->outSignal->info.chanLabel[outChan] = data->inSignal[0]->info.
		  chanLabel[inChan];
		data->outSignal->info.cFArray[outChan] = data->inSignal[0]->info.
		  cFArray[inChan];
		GetWindowLimits_SignalData(data->inSignal[0], &lowerChanLimit,
		  &upperChanLimit, data->inSignal[0]->info.cFArray[inChan],
		  p->lowerLimit, p->upperLimit, p->limitMode);
		for (inChan = lowerChanLimit; inChan <= upperChanLimit; inChan++) {
			inPtr = data->inSignal[0]->channel[inChan];
			outPtr = data->outSignal->channel[outChan];
			for (i = 0; i < data->outSignal->length; i++)
				*outPtr++ += *inPtr++;
		}
		if ((p->mode == UTILITY_LOCALCHANS_MODE_AVERAGE) &&
		  (lowerChanLimit != upperChanLimit)) {
			numChannels = upperChanLimit - lowerChanLimit + 1;
			outPtr = data->outSignal->channel[outChan];
			for (i = 0; i < data->outSignal->length; i++)
				*outPtr++ /= numChannels;
		}
			
	}

	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

