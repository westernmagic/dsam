/**********************
 *
 * File:		AnAverages.c
 * Purpose:		This routine calculates the average value for each channel.
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 *				This module assumes that a negative timeRange assumes a 
 *				period to the end of the signal.
 * Author:		L. P. O'Mard
 * Created:		21 May 1996
 * Updated:	
 * Copyright:	(c) 1998, University of Essex.
 *
 *********************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "FiParFile.h"
#include "GeModuleMgr.h"
#include "AnAverages.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

AveragesPtr	averagesPtr = NULL;

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
Free_Analysis_Averages(void)
{
	/* static const char	*funcName = "Free_Analysis_Averages"; */

	if (averagesPtr == NULL)
		return(FALSE);
	if (averagesPtr->parList)
		FreeList_UniParMgr(&averagesPtr->parList);
	if (averagesPtr->parSpec == GLOBAL) {
		free(averagesPtr);
		averagesPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitModeList **********************************/

/*
 * This routine intialises the Mode list array.
 */

BOOLN
InitModeList_Analysis_Averages(void)
{
	static NameSpecifier	modeList[] = {

					{ "FULL", AVERAGES_FULL },
					{ "-WAVE", AVERAGES_NEGATIVE_WAVE },
					{ "+WAVE", AVERAGES_POSITIVE_WAVE },
					{ "", AVERAGES_NULL }
				};
	averagesPtr->modeList = modeList;
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
Init_Analysis_Averages(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_Analysis_Averages";

	if (parSpec == GLOBAL) {
		if (averagesPtr != NULL)
			Free_Analysis_Averages();
		if ((averagesPtr = (AveragesPtr) malloc(sizeof(Averages))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (averagesPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	averagesPtr->parSpec = parSpec;
	averagesPtr->modeFlag = TRUE;
	averagesPtr->timeOffsetFlag = TRUE;
	averagesPtr->timeRangeFlag = TRUE;
	averagesPtr->mode = AVERAGES_FULL;
	averagesPtr->timeOffset = 0.0;
	averagesPtr->timeRange = -1.0;

	InitModeList_Analysis_Averages();
	if (!SetUniParList_Analysis_Averages()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_Analysis_Averages();
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
SetUniParList_Analysis_Averages(void)
{
	static const char *funcName = "SetUniParList_Analysis_Averages";
	UniParPtr	pars;

	if ((averagesPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  ANALYSIS_AVERAGES_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = averagesPtr->parList->pars;
	SetPar_UniParMgr(&pars[ANALYSIS_AVERAGES_MODE], "MODE",
	  "Averaging mode ('full', '-wave' or '+wave').",
	  UNIPAR_NAME_SPEC,
	  &averagesPtr->mode, averagesPtr->modeList,
	  (void * (*)) SetMode_Analysis_Averages);
	SetPar_UniParMgr(&pars[ANALYSIS_AVERAGES_TIMEOFFSET], "OFFSET",
	  "Time offset for start of averaging period (s).",
	  UNIPAR_REAL,
	  &averagesPtr->timeOffset, NULL,
	  (void * (*)) SetTimeOffset_Analysis_Averages);
	SetPar_UniParMgr(&pars[ANALYSIS_AVERAGES_TIMERANGE], "DURATION",
	  "Averaging period duration: -ve assumes to end of signal (s).",
	  UNIPAR_REAL,
	  &averagesPtr->timeRange, NULL,
	  (void * (*)) SetTimeRange_Analysis_Averages);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Analysis_Averages(void)
{
	static const char	*funcName = "GetUniParListPtr_Analysis_Averages";

	if (averagesPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (averagesPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(averagesPtr->parList);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_Analysis_Averages(char *mode, double timeOffset, double timeRange)
{
	static const char	*funcName = "SetPars_Analysis_Averages";
	BOOLN	ok;

	ok = TRUE;
	if (!SetMode_Analysis_Averages(mode))
		ok = FALSE;
	if (!SetTimeOffset_Analysis_Averages(timeOffset))
		ok = FALSE;
	if (!SetTimeRange_Analysis_Averages(timeRange))
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
SetMode_Analysis_Averages(char *theMode)
{
	static const char	*funcName = "SetMode_Analysis_Averages";
	int		specifier;

	if (averagesPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theMode, averagesPtr->modeList)) ==
	  AVERAGES_NULL) {
		NotifyError("%s: Illegal mode name (%s).", funcName, theMode);
		return(FALSE);
	}
	averagesPtr->modeFlag = TRUE;
	averagesPtr->mode = specifier;
	return(TRUE);

}

/****************************** SetTimeOffset *********************************/

/*
 * This function sets the module's timeOffset parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTimeOffset_Analysis_Averages(double theTimeOffset)
{
	static const char	*funcName = "SetTimeOffset_Analysis_Averages";

	if (averagesPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (theTimeOffset < 0.0) {
		NotifyError("%s: Illegal value (%g ms).", funcName,
		  MSEC(theTimeOffset));
		return(FALSE);
	}
	averagesPtr->timeOffsetFlag = TRUE;
	averagesPtr->timeOffset = theTimeOffset;
	return(TRUE);

}

/****************************** SetTimeRange **********************************/

/*
 * This function sets the module's timeRange parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTimeRange_Analysis_Averages(double theTimeRange)
{
	static const char	*funcName = "SetTimeRange_Analysis_Averages";

	if (averagesPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	averagesPtr->timeRangeFlag = TRUE;
	averagesPtr->timeRange = theTimeRange;
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
CheckPars_Analysis_Averages(void)
{
	static const char	*funcName = "CheckPars_Analysis_Averages";
	BOOLN	ok;

	ok = TRUE;
	if (!averagesPtr->modeFlag) {
		NotifyError("%s: mode variable not set.", funcName);
		ok = FALSE;
	}
	if (averagesPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!averagesPtr->timeOffsetFlag) {
		NotifyError("%s: timeOffset variable not set.", funcName);
		ok = FALSE;
	}
	if (!averagesPtr->timeRangeFlag) {
		NotifyError("%s: timeRange variable not set.", funcName);
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
PrintPars_Analysis_Averages(void)
{
	static const char	*funcName = "PrintPars_Analysis_Averages";

	if (!CheckPars_Analysis_Averages()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Averages Analysis Module Parameters:-\n");
	DPrint("\tMode = %s,",
	  averagesPtr->modeList[averagesPtr->mode].name);
	DPrint("\tTime offset = %g ms,\n",
	  MSEC(averagesPtr->timeOffset));
	DPrint("\tTime range = ");
	if (averagesPtr->timeRange < 0.0)
		DPrint("<end of signal>\n");
	else
		DPrint("%g ms\n", MSEC(averagesPtr->timeRange));
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_Analysis_Averages(char *fileName)
{
	static const char	*funcName = "ReadPars_Analysis_Averages";
	BOOLN	ok;
	char	*filePath;
	char	mode[MAXLINE];
	double	timeOffset, timeRange;
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
	if (!GetPars_ParFile(fp, "%lf", &timeOffset))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &timeRange))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "
		  "parameter file '%s'.", funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_Analysis_Averages(mode, timeOffset, timeRange)) {
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
SetParsPointer_Analysis_Averages(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_Analysis_Averages";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	averagesPtr = (AveragesPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Analysis_Averages(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_Analysis_Averages";

	if (!SetParsPointer_Analysis_Averages(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_Analysis_Averages(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = averagesPtr;
	theModule->CheckPars = CheckPars_Analysis_Averages;
	theModule->Free = Free_Analysis_Averages;
	theModule->GetUniParListPtr = GetUniParListPtr_Analysis_Averages;
	theModule->PrintPars = PrintPars_Analysis_Averages;
	theModule->ReadPars = ReadPars_Analysis_Averages;
	theModule->RunProcess = Calc_Analysis_Averages;
	theModule->SetParsPointer = SetParsPointer_Analysis_Averages;
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
CheckData_Analysis_Averages(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_Analysis_Averages";
	double	signalDuration;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInit_SignalData(data->inSignal[0], "CheckData_Analysis_Averages"))
		return(FALSE);
	signalDuration = _GetDuration_SignalData(data->inSignal[0]);
	if (averagesPtr->timeOffset > signalDuration) {
		NotifyError("%s: Time offset (%g ms) must be less than the signal\n"
		  "duration (%g ms).", funcName, MSEC(averagesPtr->timeOffset),
		  MSEC(signalDuration));
		return(FALSE);
	}
	if ((averagesPtr->timeRange >= 0.0) && (averagesPtr->timeOffset +
	  averagesPtr->timeRange > signalDuration)) {
		NotifyError("%s: Time offset (%g ms) + timeRange (%g ms) must be\n"
		  "less than the signal duration (%g ms).", funcName,
		  MSEC(averagesPtr->timeOffset), MSEC(averagesPtr->timeRange),
		  MSEC(signalDuration));
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
Calc_Analysis_Averages(EarObjectPtr data)
{
	static const char	*funcName = "Calc_Analysis_Averages";
	register	ChanData	 *inPtr, sum;
	int		chan;
	double	dt;
	ChanLen	i, timeOffsetIndex, timeRangeIndex;

	if (!CheckPars_Analysis_Averages())
		return(FALSE);
	if (!CheckData_Analysis_Averages(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "Averages Analysis");
	if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels, 1,
	  1.0)) {
		NotifyError("%s: Cannot initialise output channels.", funcName);
		return(FALSE);
	}
	dt = data->inSignal[0]->dt;
	timeOffsetIndex = (ChanLen) floor(averagesPtr->timeOffset / dt + 0.5);
	timeRangeIndex = (averagesPtr->timeRange <= 0.0)?
	  data->inSignal[0]->length - timeOffsetIndex:
	  (ChanLen) floor(averagesPtr->timeRange / dt + 0.5);
	for (chan = 0; chan < data->inSignal[0]->numChannels; chan++) {
		inPtr = data->inSignal[0]->channel[chan] + timeOffsetIndex;
		for (i = 0, sum = 0.0; i < timeRangeIndex; i++, inPtr++)
			switch (averagesPtr->mode) {
			case AVERAGES_FULL:
				sum += *inPtr;
				break;
			case AVERAGES_NEGATIVE_WAVE:
				if (*inPtr < 0.0)
					sum += *inPtr;
				break;
			case AVERAGES_POSITIVE_WAVE:
				if (*inPtr > 0.0)
					sum += *inPtr;
				break;
			} /* switch */
				
		data->outSignal->channel[chan][0] = (ChanData) (sum / timeRangeIndex);
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

