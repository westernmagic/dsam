/**********************
 *
 * File:		AnFindBin.c
 * Purpose:		This module finds the maximum/minimum bin values/indexes for a
 *				signal
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 *				The search is implemented using a bin "window" of "binWidth".
 * Author:		L. P. O'Mard
 * Created:		8 Mar 1996
 * Updated:		9 Jun 1996
 * Copyright:	(c) 1998, University of Essex
 *
 *********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "FiParFile.h"
#include "AnFindBin.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

FindBinPtr	findBinPtr = NULL;

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
Free_Analysis_FindBin(void)
{
	/* static const char	*funcName = "Free_Analysis_FindBin"; */

	if (findBinPtr == NULL)
		return(FALSE);
	if (findBinPtr->parList)
		FreeList_UniParMgr(&findBinPtr->parList);
	if (findBinPtr->parSpec == GLOBAL) {
		free(findBinPtr);
		findBinPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitModeList **********************************/

/*
 * This routine intialises the Mode list array.
 */

BOOLN
InitModeList_Analysis_FindBin(void)
{
	static NameSpecifier	modeList[] = {

					{ "MIN_VALUE", FIND_BIN_MIN_VALUE_MODE},
					{ "MIN_INDEX", FIND_BIN_MIN_INDEX_MODE},
					{ "MAX_VALUE", FIND_BIN_MAX_VALUE_MODE},
					{ "MAX_INDEX", FIND_BIN_MAX_INDEX_MODE},
					{ "", FIND_BIN_NULL }
				
				};
	findBinPtr->modeList = modeList;
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
Init_Analysis_FindBin(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_Analysis_FindBin";

	if (parSpec == GLOBAL) {
		if (findBinPtr != NULL)
			Free_Analysis_FindBin();
		if ((findBinPtr = (FindBinPtr) malloc(sizeof(FindBin))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (findBinPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	findBinPtr->parSpec = parSpec;
	findBinPtr->modeFlag = TRUE;
	findBinPtr->binWidthFlag = TRUE;
	findBinPtr->timeOffsetFlag = TRUE;
	findBinPtr->timeWidthFlag = TRUE;
	findBinPtr->mode = FIND_BIN_MIN_VALUE_MODE;
	findBinPtr->binWidth = -1.0;
	findBinPtr->timeOffset = 0.0;
	findBinPtr->timeWidth = -1.0;

	InitModeList_Analysis_FindBin();
	if (!SetUniParList_Analysis_FindBin()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_Analysis_FindBin();
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
SetUniParList_Analysis_FindBin(void)
{
	static const char *funcName = "SetUniParList_Analysis_FindBin";
	UniParPtr	pars;

	if ((findBinPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  ANALYSIS_FINDBIN_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = findBinPtr->parList->pars;
	SetPar_UniParMgr(&pars[ANALYSIS_FINDBIN_MODE], "MODE",
	  "Search mode ('max_value', 'max_index', 'min_value' or 'min_index').",
	  UNIPAR_NAME_SPEC,
	  &findBinPtr->mode, findBinPtr->modeList,
	  (void * (*)) SetMode_Analysis_FindBin);
	SetPar_UniParMgr(&pars[ANALYSIS_FINDBIN_BINWIDTH], "BIN_WIDTH",
	  "Bin width (time window) for search bin: -ve assumes dt for previous "
	  "signal (s).",
	  UNIPAR_REAL,
	  &findBinPtr->binWidth, NULL,
	  (void * (*)) SetBinWidth_Analysis_FindBin);
	SetPar_UniParMgr(&pars[ANALYSIS_FINDBIN_TIMEOFFSET], "OFFSET",
	  "Offset from which to start search (s?).",
	  UNIPAR_REAL,
	  &findBinPtr->timeOffset, NULL,
	  (void * (*)) SetTimeOffset_Analysis_FindBin);
	SetPar_UniParMgr(&pars[ANALYSIS_FINDBIN_TIMEWIDTH], "WIDTH",
	  "Analysis window width for search: -ve assume to end of signal (s?).",
	  UNIPAR_REAL,
	  &findBinPtr->timeWidth, NULL,
	  (void * (*)) SetTimeWidth_Analysis_FindBin);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Analysis_FindBin(void)
{
	static const char	*funcName = "GetUniParListPtr_Analysis_FindBin";

	if (findBinPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (findBinPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(findBinPtr->parList);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_Analysis_FindBin(char *mode, double binWidth,
  double timeOffset, double timeWidth)
{
	static const char	*funcName = "SetPars_Analysis_FindBin";
	BOOLN	ok;

	ok = TRUE;
	if (!SetMode_Analysis_FindBin(mode))
		ok = FALSE;
	if (!SetBinWidth_Analysis_FindBin(binWidth))
		ok = FALSE;
	if (!SetTimeOffset_Analysis_FindBin(timeOffset))
		ok = FALSE;
	if (!SetTimeWidth_Analysis_FindBin(timeWidth))
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
SetMode_Analysis_FindBin(char *theMode)
{
	static const char	*funcName = "SetMode_Analysis_FindBin";
	int		specifier;

	if (findBinPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theMode, findBinPtr->modeList)) ==
	  FIND_BIN_NULL) {
		NotifyError("%s: Illegal mode name (%s).", funcName, theMode);
		return(FALSE);
	}
	findBinPtr->modeFlag = TRUE;
	findBinPtr->mode = specifier;
	return(TRUE);

}

/****************************** SetBinWidth ***********************************/

/*
 * This function sets the module's binWidth parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetBinWidth_Analysis_FindBin(double theBinWidth)
{
	static const char	*funcName = "SetBinWidth_Analysis_FindBin";

	if (findBinPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	findBinPtr->binWidthFlag = TRUE;
	findBinPtr->binWidth = theBinWidth;
	return(TRUE);

}

/****************************** SetTimeOffset *********************************/

/*
 * This function sets the module's timeOffset parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTimeOffset_Analysis_FindBin(double theTimeOffset)
{
	static const char	*funcName = "SetTimeOffset_Analysis_FindBin";

	if (findBinPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (theTimeOffset < 0.0) {
		NotifyError("%s: Illegal time offset = %g ms.", funcName,
		  theTimeOffset);
		return(FALSE);
	}
	findBinPtr->timeOffsetFlag = TRUE;
	findBinPtr->timeOffset = theTimeOffset;
	return(TRUE);

}

/****************************** SetTimeWidth **********************************/

/*
 * This function sets the module's timeWidth parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTimeWidth_Analysis_FindBin(double theTimeWidth)
{
	static const char	*funcName = "SetTimeWidth_Analysis_FindBin";

	if (findBinPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	findBinPtr->timeWidthFlag = TRUE;
	findBinPtr->timeWidth = theTimeWidth;
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
CheckPars_Analysis_FindBin(void)
{
	static const char	*funcName = "CheckPars_Analysis_FindBin";
	BOOLN	ok;

	ok = TRUE;
	if (findBinPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!findBinPtr->modeFlag) {
		NotifyError("%s: mode variable not set.", funcName);
		ok = FALSE;
	}
	if (!findBinPtr->binWidthFlag) {
		NotifyError("%s: binWidth variable not set.", funcName);
		ok = FALSE;
	}
	if (!findBinPtr->timeOffsetFlag) {
		NotifyError("%s: timeOffset variable not set.", funcName);
		ok = FALSE;
	}
	if (!findBinPtr->timeWidthFlag) {
		NotifyError("%s: timeWidth variable not set.", funcName);
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
PrintPars_Analysis_FindBin(void)
{
	static const char	*funcName = "PrintPars_Analysis_FindBin";

	if (!CheckPars_Analysis_FindBin()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Find Max. bin Values Module Parameters:-\n");
	DPrint("\tSearch mode = %s,",
	  findBinPtr->modeList[findBinPtr->mode].name);
	DPrint("\tBin width = ");
	if (findBinPtr->binWidth <= 0.0)
		DPrint("<prev. signal dt>,\n");
	else
		DPrint("%g ms,\n", MSEC(findBinPtr->binWidth));
	DPrint("\tTime offset = %g ms,",
	  MSEC(findBinPtr->timeOffset));
	DPrint("\tTime range = ");
	if (findBinPtr->timeWidth < 0.0)
		DPrint("<end of signal>\n");
	else
		DPrint("%g ms\n", MSEC(findBinPtr->timeWidth));
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_Analysis_FindBin(char *fileName)
{
	static const char	*funcName = "ReadPars_Analysis_FindBin";
	BOOLN	ok;
	char	*filePath;
	char	mode[MAXLINE];
	double	binWidth, timeOffset, timeWidth;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(filePath, "r")) == NULL) {
		NotifyError("%s: Cannot open data file '%s'.\n", funcName, filePath);
		return(FALSE);
	}
	DPrint("%s: Reading from '%s':\n", funcName, filePath);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, "%s", mode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &binWidth))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &timeOffset))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &timeWidth))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "
		  "parameter file '%s'.", funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_Analysis_FindBin(mode, binWidth, timeOffset,
	  timeWidth)) {
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
SetParsPointer_Analysis_FindBin(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_Analysis_FindBin";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	findBinPtr = (FindBinPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Analysis_FindBin(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_Analysis_FindBin";

	if (!SetParsPointer_Analysis_FindBin(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_Analysis_FindBin(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = findBinPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->CheckPars = CheckPars_Analysis_FindBin;
	theModule->Free = Free_Analysis_FindBin;
	theModule->GetUniParListPtr = GetUniParListPtr_Analysis_FindBin;
	theModule->PrintPars = PrintPars_Analysis_FindBin;
	theModule->ReadPars = ReadPars_Analysis_FindBin;
	theModule->RunProcess = Calc_Analysis_FindBin;
	theModule->SetParsPointer = SetParsPointer_Analysis_FindBin;
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
CheckData_Analysis_FindBin(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_Analysis_FindBin";
	double	signalDuration, dt;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	dt = data->inSignal[0]->dt;
	if ((findBinPtr->binWidth > 0.0) && (findBinPtr->binWidth < dt)) {
		NotifyError("%s: Bin width (%g ms) is less than sampling interval, "
		  "(%g ms).", funcName, MSEC(findBinPtr->binWidth), MSEC(dt));
		return(FALSE);
	}
	signalDuration = _GetDuration_SignalData(data->inSignal[0]);
	if (findBinPtr->timeOffset >= signalDuration) {
		NotifyError("%s: Time offset (%g ms) is too long for the signal "
		  "duration (%g ms).", funcName, MSEC(findBinPtr->timeOffset),
		  MSEC(signalDuration));
		return(FALSE);
	}
	if ((findBinPtr->binWidth > 0.0) && (findBinPtr->timeOffset +
	  findBinPtr->binWidth >= signalDuration)) {
		NotifyError("%s: Time offset + bin width (%g ms) must not "
		  "exceed the signal duration (%g ms).", funcName, MSEC(findBinPtr->
		  timeOffset + findBinPtr->binWidth), MSEC(signalDuration));
		return(FALSE);
	}
	if ((findBinPtr->timeWidth > 0.0) && (findBinPtr->timeOffset +
	  findBinPtr->timeWidth >= signalDuration)) {
		NotifyError("%s: Time offset + range (%g ms) must not exceed the "
		  "signal duration (%g ms).", funcName, MSEC(findBinPtr->timeOffset +
		  findBinPtr->timeWidth), MSEC(signalDuration));
		return(FALSE);
	}
	return(TRUE);

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
Calc_Analysis_FindBin(EarObjectPtr data)
{
	static const char	*funcName = "Calc_Analysis_FindBin";
	register	ChanData	*inPtr, *binPtr;
	register	double		sum, binSum;
	int		chan;
	double	dt;
	ChanLen	i, j, binIndex = 0;
	FindBinPtr	p = findBinPtr;

	if (!data->threadRunFlag) {
		if (!CheckPars_Analysis_FindBin())
			return(FALSE);
		if (!CheckData_Analysis_FindBin(data)) {
			NotifyError("%s: Process data invalid.", funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, "Find Maximum Bin Value Analysis");
		if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels, 1,
		  1.0)) {
			NotifyError("%s: Cannot initialise output channels.", funcName);
			return(FALSE);
		}
		p->findMinimum = (p->mode == FIND_BIN_MIN_VALUE_MODE) || (p->mode ==
		  FIND_BIN_MIN_INDEX_MODE);
		dt = data->inSignal[0]->dt;
		p->binWidthIndex = (p->binWidth <= 0.0)? (ChanLen) 1: (ChanLen) (p->
		  binWidth / dt + 0.5);
		p->timeOffsetIndex = (ChanLen) (p->timeOffset / dt + 0.5);
		p->timeWidthIndex = (p->timeWidth <= 0.0)? data->inSignal[0]->length -
		  p->timeOffsetIndex: (ChanLen) (p->timeWidth / dt + 0.5);
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	for (chan = data->outSignal->offset; chan < data->outSignal->numChannels;
	  chan++) {
		inPtr = data->inSignal[0]->channel[chan] + p->timeOffsetIndex;
		binSum = (p->findMinimum)? DBL_MAX: -DBL_MAX;
		for (i = 0; i < p->timeWidthIndex - p->binWidthIndex; i++, inPtr++) {
			for (j = 0, sum = 0.0, binPtr = inPtr; j < p->binWidthIndex; j++,
			  binPtr++)
				sum += *binPtr;
			if ((p->findMinimum)? (sum < binSum): (sum > binSum)) {
				binSum = sum;
				binIndex = p->timeOffsetIndex + i - p->binWidthIndex / 2;
			}
		}
		switch (p->mode) {
		case FIND_BIN_MIN_VALUE_MODE:
		case FIND_BIN_MAX_VALUE_MODE:
			data->outSignal->channel[chan][0] = binSum / p->binWidthIndex;
			break;
		case FIND_BIN_MIN_INDEX_MODE:
		case FIND_BIN_MAX_INDEX_MODE:
			data->outSignal->channel[chan][0] = binIndex;
			break;
		} /* switch */
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

