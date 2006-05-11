/**********************
 *
 * File:		AnSpikeReg.c
 * Purpose:		This module carries out a spike regularity analysis,
 *				calculating the mean, standard deviation and covariance
 *				measures for a spike train.
 *				The results for each channel are stored in the order:
 *				 - mean, standard deviation, covariance'.
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 *				See Hewitt M. J. & Meddis R. (1993) "Regularity of cochlear
 *				nucleus stellate cells: A computational Modeling study",
 *				J. of the Acoust. Soc. Am, 93, pp 3390-3399.
 *				If the standard deviation results are only valid if the 
 *				covariance measure is greater than 0.  This enables the case
 *				when there are less than two counts to be marked.
 *				10-1-97: LPO - added dead-time correction for
 *				covariance CV' = S.D. / (mean - dead time) - see Rothman J. S.
 *				Young E. D. and Manis P. B. "Convergence of Auditory Nerve
 *				Fibers in the Ventral Cochlear Nucleus: Implications of a
 *				Computational Model" J. of NeuroPhysiology, 70:2562-2583.
 *				04-01-05: LPO: The 'countEarObj' EarObject does need to be
 *				registered as a subprocess for the thread processing because
 *				access to the channels is controlled by the main output process
 *				channel access.
 * Author:		L. P. O'Mard
 * Created:		01 Apr 1996
 * Updated:		20 Feb 1997
 * Copyright:	(c) 1998, University of Essex
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
#include "UtString.h"
#include "AnSpikeReg.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

SpikeRegPtr	spikeRegPtr = NULL;

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
Free_Analysis_SpikeRegularity(void)
{
	if (spikeRegPtr == NULL)
		return(FALSE);
	FreeProcessVariables_Analysis_SpikeRegularity();
	if (spikeRegPtr->parList)
		FreeList_UniParMgr(&spikeRegPtr->parList);
	if (spikeRegPtr->parSpec == GLOBAL) {
		free(spikeRegPtr);
		spikeRegPtr = NULL;
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
Init_Analysis_SpikeRegularity(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_Analysis_SpikeRegularity");

	if (parSpec == GLOBAL) {
		if (spikeRegPtr != NULL)
			Free_Analysis_SpikeRegularity();
		if ((spikeRegPtr = (SpikeRegPtr) malloc(sizeof(SpikeReg))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (spikeRegPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	spikeRegPtr->parSpec = parSpec;
	spikeRegPtr->updateProcessVariablesFlag = TRUE;
	spikeRegPtr->eventThresholdFlag = TRUE;
	spikeRegPtr->windowWidthFlag = TRUE;
	spikeRegPtr->timeOffsetFlag = TRUE;
	spikeRegPtr->timeRangeFlag = TRUE;
	spikeRegPtr->deadTimeFlag = TRUE;
	spikeRegPtr->eventThreshold = 0.0;
	spikeRegPtr->windowWidth = -1.0;
	spikeRegPtr->timeOffset = 0.0;
	spikeRegPtr->timeRange = -10.0;
	spikeRegPtr->deadTime = 0.0;

	if (!SetUniParList_Analysis_SpikeRegularity()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Analysis_SpikeRegularity();
		return(FALSE);
	}
	spikeRegPtr->spikeListSpec= NULL;
	spikeRegPtr->countEarObj = NULL;
	spikeRegPtr->runningTimeOffsetIndex = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_Analysis_SpikeRegularity(void)
{
	static const WChar *funcName = wxT(
	  "SetUniParList_Analysis_SpikeRegularity");
	UniParPtr	pars;

	if ((spikeRegPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  ANALYSIS_SPIKEREGULARITY_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = spikeRegPtr->parList->pars;
	SetPar_UniParMgr(&pars[ANALYSIS_SPIKEREGULARITY_EVENTTHRESHOLD],
	  wxT("THRESHOLD"),
	  wxT("Event threshold (arbitrary units)."),
	  UNIPAR_REAL,
	  &spikeRegPtr->eventThreshold, NULL,
	  (void * (*)) SetEventThreshold_Analysis_SpikeRegularity);
	SetPar_UniParMgr(&pars[ANALYSIS_SPIKEREGULARITY_WINDOWWIDTH],
	  wxT("WINDOW_WIDTH"),
	  wxT("Width of analysis window: -ve assumes total analysis range (s)."),
	  UNIPAR_REAL,
	  &spikeRegPtr->windowWidth, NULL,
	  (void * (*)) SetWindowWidth_Analysis_SpikeRegularity);
	SetPar_UniParMgr(&pars[ANALYSIS_SPIKEREGULARITY_TIMEOFFSET], wxT("OFFSET"),
	  wxT("Time offset for start of analysis (s)."),
	  UNIPAR_REAL,
	  &spikeRegPtr->timeOffset, NULL,
	  (void * (*)) SetTimeOffset_Analysis_SpikeRegularity);
	  SetPar_UniParMgr(&pars[ANALYSIS_SPIKEREGULARITY_TIMERANGE], wxT("RANGE"),
	  wxT("Time range for analysis: -ve assumes to end of signal (s)."),
	  UNIPAR_REAL,
	  &spikeRegPtr->timeRange, NULL,
	  (void * (*)) SetTimeRange_Analysis_SpikeRegularity);
	SetPar_UniParMgr(&pars[ANALYSIS_SPIKEREGULARITY_DEADTIME], wxT("DEAD_TIME"),
	  wxT("Spike dead time or absolute refractory period (s)."),
	  UNIPAR_REAL,
	  &spikeRegPtr->deadTime, NULL,
	  (void * (*)) SetDeadTime_Analysis_SpikeRegularity);

	SetAltAbbreviation_UniParMgr(&pars[ANALYSIS_SPIKEREGULARITY_TIMERANGE],
	  wxT("MAX_RANGE"));
	SetAltAbbreviation_UniParMgr(&pars[ANALYSIS_SPIKEREGULARITY_WINDOWWIDTH],
	  wxT("BIN_WIDTH"));
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Analysis_SpikeRegularity(void)
{
	static const WChar	*funcName = wxT(
	  "GetUniParListPtr_Analysis_SpikeRegularity");

	if (spikeRegPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (spikeRegPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been "
		  "initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(spikeRegPtr->parList);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_Analysis_SpikeRegularity(double eventThreshold, double windowWidth,
  double timeOffset, double timeRange)
{
	static const WChar	*funcName = wxT("SetPars_Analysis_SpikeRegularity");
	BOOLN	ok;

	ok = TRUE;
	if (!SetEventThreshold_Analysis_SpikeRegularity(eventThreshold))
		ok = FALSE;
	if (!SetWindowWidth_Analysis_SpikeRegularity(windowWidth))
		ok = FALSE;
	if (!SetTimeOffset_Analysis_SpikeRegularity(timeOffset))
		ok = FALSE;
	if (!SetTimeRange_Analysis_SpikeRegularity(timeRange))
		ok = FALSE;
	if (!ok)
		NotifyError(wxT("%s: Failed to set all module parameters.") ,funcName);
	return(ok);

}

/****************************** SetEventThreshold *****************************/

/*
 * This function sets the module's eventThreshold parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetEventThreshold_Analysis_SpikeRegularity(double theEventThreshold)
{
	static const WChar	*funcName =
	  wxT("SetEventThreshold_Analysis_SpikeRegularity");

	if (spikeRegPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	spikeRegPtr->eventThresholdFlag = TRUE;
	spikeRegPtr->eventThreshold = theEventThreshold;
	return(TRUE);

}

/****************************** SetWindowWidth ********************************/

/*
 * This function sets the module's windowWidth parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetWindowWidth_Analysis_SpikeRegularity(double theWindowWidth)
{
	static const WChar	*funcName = wxT(
	  "SetWindowWidth_Analysis_SpikeRegularity");

	if (spikeRegPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	spikeRegPtr->windowWidthFlag = TRUE;
	spikeRegPtr->updateProcessVariablesFlag = TRUE;
	spikeRegPtr->windowWidth = theWindowWidth;
	return(TRUE);

}

/****************************** SetTimeOffset *********************************/

/*
 * This function sets the module's timeOffset parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTimeOffset_Analysis_SpikeRegularity(double theTimeOffset)
{
	static const WChar	*funcName = wxT(
	  "SetTimeOffset_Analysis_SpikeRegularity");

	if (spikeRegPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theTimeOffset < 0.0) {
		NotifyError(wxT("%s: Time offset must be greater than zero (%g ms)."),
		  funcName, MSEC(theTimeOffset));
		return(FALSE);
	}
	spikeRegPtr->timeOffsetFlag = TRUE;
	spikeRegPtr->updateProcessVariablesFlag = TRUE;
	spikeRegPtr->timeOffset = theTimeOffset;
	return(TRUE);

}

/****************************** SetTimeRange **********************************/

/*
 * This function sets the module's timeRange parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTimeRange_Analysis_SpikeRegularity(double theTimeRange)
{
	static const WChar	*funcName = wxT(
	  "SetTimeRange_Analysis_SpikeRegularity");

	if (spikeRegPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	spikeRegPtr->timeRangeFlag = TRUE;
	spikeRegPtr->updateProcessVariablesFlag = TRUE;
	spikeRegPtr->timeRange = theTimeRange;
	return(TRUE);

}

/****************************** SetDeadTime ***********************************/

/*
 * This function sets the module's deadTime parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDeadTime_Analysis_SpikeRegularity(double theDeadTime)
{
	static const WChar	*funcName = wxT("SetDeadTime_Analysis_SpikeRegularity");

	if (spikeRegPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theDeadTime < 0.0) {
		NotifyError(wxT("%s: Dead time cannot be negative. (%d ms)."), funcName,
		  MILLI(theDeadTime));
		return(FALSE);
	}
	spikeRegPtr->deadTimeFlag = TRUE;
	spikeRegPtr->deadTime = theDeadTime;
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
CheckPars_Analysis_SpikeRegularity(void)
{
	static const WChar	*funcName = wxT("CheckPars_Analysis_SpikeRegularity");
	BOOLN	ok;

	ok = TRUE;
	if (spikeRegPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (!spikeRegPtr->eventThresholdFlag) {
		NotifyError(wxT("%s: eventThreshold variable not set."), funcName);
		ok = FALSE;
	}
	if (!spikeRegPtr->windowWidthFlag) {
		NotifyError(wxT("%s: windowWidth variable not set."), funcName);
		ok = FALSE;
	}
	if (!spikeRegPtr->timeOffsetFlag) {
		NotifyError(wxT("%s: timeOffset variable not set."), funcName);
		ok = FALSE;
	}
	if (!spikeRegPtr->timeRangeFlag) {
		NotifyError(wxT("%s: timeRange variable not set."), funcName);
		ok = FALSE;
	}
	if (!spikeRegPtr->deadTimeFlag) {
		NotifyError(wxT("%s: deadTime variable not set."), funcName);
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
PrintPars_Analysis_SpikeRegularity(void)
{
	static const WChar	*funcName = wxT("PrintPars_Analysis_SpikeRegularity");

	if (!CheckPars_Analysis_SpikeRegularity()) {
		NotifyError(wxT("%s: Parameters have not been correctly set."),
		  funcName);
		return(FALSE);
	}
	DPrint(wxT("Spike Regularity Analysis Module Parameters:-\n"));
	DPrint(wxT("\tEvent threshold = %g units,"),
	  spikeRegPtr->eventThreshold);
	DPrint(wxT("\tBin width = "));
	if (spikeRegPtr->windowWidth <= 0.0)
		DPrint(wxT("<prev. signal dt>,\n"));
	else
		DPrint(wxT("%g ms,\n"), MSEC(spikeRegPtr->windowWidth));
	DPrint(wxT("\tTime offset = %g ms,"), MSEC(spikeRegPtr->timeOffset));
	DPrint(wxT("\tTime range = "));
	if (spikeRegPtr->timeRange > 0.0)
		DPrint(wxT("%g ms.\n"), MSEC(spikeRegPtr->timeRange));
	else
		DPrint(wxT("end of signal.\n"));
	DPrint(wxT("\tDead time = %g ms.\n"), MILLI(spikeRegPtr->deadTime));
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_Analysis_SpikeRegularity(WChar *fileName)
{
	static const WChar	*funcName = wxT("ReadPars_Analysis_SpikeRegularity");
	BOOLN	ok;
	WChar	*filePath;
	double	eventThreshold, windowWidth, timeOffset, timeRange;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(ConvUTF8_Utility_String(filePath), "r")) == NULL) {
		NotifyError(wxT("%s: Cannot open data file '%s'.\n"), funcName,
		  filePath);
		return(FALSE);
	}
	DPrint(wxT("%s: Reading from '%s':\n"), funcName, filePath);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &eventThreshold))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &windowWidth))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &timeOffset))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &timeRange))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError(wxT("%s: Not enough lines, or invalid parameters, in "
		  "module parameter file '%s'."), funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_Analysis_SpikeRegularity(eventThreshold, windowWidth,
	  timeOffset, timeRange)) {
		NotifyError(wxT("%s: Could not set parameters."), funcName);
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
SetParsPointer_Analysis_SpikeRegularity(ModulePtr theModule)
{
	static const WChar	*funcName = wxT(
	  "SetParsPointer_Analysis_SpikeRegularity");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	spikeRegPtr = (SpikeRegPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Analysis_SpikeRegularity(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Analysis_SpikeRegularity");

	if (!SetParsPointer_Analysis_SpikeRegularity(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Analysis_SpikeRegularity(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."), funcName);
		return(FALSE);
	}
	theModule->parsPtr = spikeRegPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->CheckPars = CheckPars_Analysis_SpikeRegularity;
	theModule->Free = Free_Analysis_SpikeRegularity;
	theModule->GetUniParListPtr = GetUniParListPtr_Analysis_SpikeRegularity;
	theModule->PrintPars = PrintPars_Analysis_SpikeRegularity;
	theModule->ReadPars = ReadPars_Analysis_SpikeRegularity;
	theModule->ResetProcess = ResetProcess_Analysis_SpikeRegularity;
	theModule->RunProcess = Calc_Analysis_SpikeRegularity;
	theModule->SetParsPointer = SetParsPointer_Analysis_SpikeRegularity;
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
CheckData_Analysis_SpikeRegularity(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Analysis_SpikeRegularity");
	double	signalDuration;

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	if ((spikeRegPtr->windowWidth > 0.0) && (spikeRegPtr->windowWidth <
	 data->inSignal[0]->dt)) {
		NotifyError(wxT("%s: Bin width (%g ms) is too small for sampling\n"
		  "interval (%g ms)."), funcName, MSEC(spikeRegPtr->windowWidth),
		  MSEC(data->inSignal[0]->dt));
		return(FALSE);
	}
	signalDuration = _GetDuration_SignalData(data->inSignal[0]);
	if ((spikeRegPtr->timeRange > 0.0) && ((spikeRegPtr->timeRange +
	  spikeRegPtr->timeOffset) > signalDuration)) {
		NotifyError(wxT("%s: Time offset (%g ms) plus range (%g ms) is "
		  "longer than signal (%g ms)."), funcName, MSEC(spikeRegPtr->
		  timeOffset), MSEC(spikeRegPtr->timeRange), signalDuration);
		return(FALSE);
	}
	/*** Put additional checks here. ***/
	return(TRUE);

}

/**************************** ResetStatistics *********************************/

/*
 * This routine resets the statistics to zero values.
 * It expects the process EarObject, "countEarObj" and 'data' to be correctly
 * initialised.
 */

void
ResetStatistics_Analysis_SpikeRegularity(EarObjectPtr data)
{
	register ChanData	*sumPtr, *sumSqrsPtr, *countPtr;
	int		outChan, inChan;
	ChanLen	i;

	for (outChan = data->outSignal->offset; outChan <
	  data->outSignal->numChannels; outChan += SPIKE_REG_NUM_RETURNS) {
		inChan = outChan / SPIKE_REG_NUM_RETURNS;
		countPtr = spikeRegPtr->countEarObj->outSignal->channel[inChan];
		sumPtr = data->outSignal->channel[outChan + SPIKE_REG_MEAN];
		sumSqrsPtr = data->outSignal->channel[outChan +
		  SPIKE_REG_STANDARD_DEV];
		for (i = 0; i < data->outSignal->length; i++) {
			*sumPtr++ = 0.0;
			*sumSqrsPtr++ = 0.0;
			*countPtr++ = 0;
		}
	}

}

/**************************** ResetProcess ************************************/

/*
 * This routine resets the process variables.
 */

void
ResetProcess_Analysis_SpikeRegularity(EarObjectPtr data)
{
	SpikeRegPtr	p = spikeRegPtr;

	ResetOutSignal_EarObject(p->countEarObj);
	ResetListSpec_SpikeList(p->spikeListSpec, data->inSignal[0]);

}

/**************************** InitProcessVariables ****************************/

/*
 * This routine initialises the exponential table.
 * It assumes that all of the parameters for the module have been correctly
 * initialised.
 * This routine assumes that calloc sets all of the SpikeSpecPtr
 * pointers to NULL.
 */

BOOLN
InitProcessVariables_Analysis_SpikeRegularity(EarObjectPtr data)
{
	static const WChar *funcName =
	  wxT("InitProcessVariables_Analysis_SpikeRegularity");
	SpikeRegPtr	p = spikeRegPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag || (data->
	  timeIndex == PROCESS_START_TIME)) {
		if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
			FreeProcessVariables_Analysis_SpikeRegularity();
			if ((p->spikeListSpec = InitListSpec_SpikeList(data->inSignal[0]->
			  numChannels)) == NULL) {
				NotifyError(wxT("%s: Out of memory for spikeListSpec."),
				  funcName);
				return(FALSE);
			}
			p->countEarObj = Init_EarObject(wxT("NULL"));
			p->updateProcessVariablesFlag = FALSE;
		}
		if (!InitOutSignal_EarObject(p->countEarObj, data->inSignal[0]->
		  numChannels, data->outSignal->length, data->outSignal->dt)) {
			NotifyError(wxT("%s: Cannot initialise countEarObj."), funcName);
			return(FALSE);
		}
		if ((p->runningTimeOffsetIndex = (ChanLen *) calloc(data->inSignal[0]->
		  numChannels, sizeof(ChanLen))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'runningTimeOffsetIndex"
			  "[%d]' array."), funcName, data->inSignal[0]->numChannels);
			return(FALSE);
		}
		ResetProcess_Analysis_SpikeRegularity(data);
	} else
		ResetStatistics_Analysis_SpikeRegularity(data);
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

void
FreeProcessVariables_Analysis_SpikeRegularity(void)
{
	SpikeRegPtr	p = spikeRegPtr;

	if (p->runningTimeOffsetIndex) {
		free(p->runningTimeOffsetIndex);
		p->runningTimeOffsetIndex = NULL;
	}
	FreeListSpec_SpikeList(&p->spikeListSpec);
	Free_EarObject(&p->countEarObj);

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
 * The variable "diff" is tested for negative values, due to high-precision
 * rounding errors.
 */

BOOLN
Calc_Analysis_SpikeRegularity(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("Calc_Analysis_SpikeRegularity");
	register ChanData	*sumPtr, *covarPtr, *sumSqrsPtr, *countPtr, diff;
	int		outChan, inChan;
	double	interval, spikeTime, windowWidth, timeRange;
	ChanLen	i, spikeTimeHistIndex, timeRangeIndex;
	ChanLen	timeOffsetIndex, *runningTimeOffsetIndex;
	ChanData	oldMean;
	SpikeSpecPtr	s, headSpikeList, currentSpikeSpec;
	SpikeRegPtr	p = spikeRegPtr;

	if (!data->threadRunFlag) {
		if (!CheckPars_Analysis_SpikeRegularity())
			return(FALSE);
		if (!CheckData_Analysis_SpikeRegularity(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Spike Regularity Analysis"));
		p->dt = data->inSignal[0]->dt;
		timeRange = (p->timeRange > 0.0)? p->timeRange: _GetDuration_SignalData(
		  data->inSignal[0]) - p->timeOffset;
		windowWidth = (p->windowWidth > 0.0)? p->windowWidth: timeRange;
		timeRangeIndex = (ChanLen) floor(timeRange / windowWidth + 0.5);
		if (!InitOutSignal_EarObject(data, (uShort) (data->inSignal[0]->
		  numChannels * SPIKE_REG_NUM_RETURNS), timeRangeIndex, windowWidth)) {
			NotifyError(wxT("%s: Cannot initialise sumEarObj."), funcName);
			return(FALSE);
		}
		SetOutputTimeOffset_SignalData(data->outSignal, p->timeOffset + data->
		  outSignal->dt);
		SetInterleaveLevel_SignalData(data->outSignal, (uShort) (
		  data->inSignal[0]->interleaveLevel * SPIKE_REG_NUM_RETURNS));
		if (!InitProcessVariables_Analysis_SpikeRegularity(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		p->convertDt = p->dt / data->outSignal->dt;
		GenerateList_SpikeList(p->spikeListSpec, p->eventThreshold, data->
		  inSignal[0]);
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	/* Add additional sums. */
	timeOffsetIndex = (ChanLen) floor(p->timeOffset / p->dt + 0.5);
	for (inChan = data->inSignal[0]->offset; inChan < data->inSignal[0]->
	  numChannels; inChan++) {
		runningTimeOffsetIndex = p->spikeListSpec->timeIndex + inChan;
		if (*runningTimeOffsetIndex < timeOffsetIndex)
			*runningTimeOffsetIndex += timeOffsetIndex -
			  *runningTimeOffsetIndex;
	}
	/* First recover previous sums from signal (when not in segment mode).*/
	for (outChan = 0; outChan < data->outSignal->numChannels; outChan +=
	  SPIKE_REG_NUM_RETURNS) {
		inChan = outChan / SPIKE_REG_NUM_RETURNS;
		countPtr = p->countEarObj->outSignal->channel[inChan];
		sumPtr = data->outSignal->channel[outChan + SPIKE_REG_MEAN];
		sumSqrsPtr = data->outSignal->channel[outChan + SPIKE_REG_STANDARD_DEV];
		for (i = 0; i < data->outSignal->length; i++, sumPtr++, sumSqrsPtr++,
		  countPtr++) {
			oldMean = *sumPtr;
			*sumPtr *= *countPtr;
			if (*countPtr > SPIKE_REG_MIN_SPIKES_FOR_STATISTICS) {
				*sumSqrsPtr = SQR(*sumSqrsPtr);
				*sumSqrsPtr = *sumSqrsPtr * (*countPtr - 1.0) + (*sumPtr *
				  oldMean);
			}
		}
	}
	for (outChan = data->outSignal->offset; outChan <
	  data->outSignal->numChannels; outChan += SPIKE_REG_NUM_RETURNS) {
		inChan = outChan / SPIKE_REG_NUM_RETURNS;
		countPtr = p->countEarObj->outSignal->channel[inChan];
		sumPtr = data->outSignal->channel[outChan + SPIKE_REG_MEAN];
		sumSqrsPtr = data->outSignal->channel[outChan + SPIKE_REG_STANDARD_DEV];
		if ((headSpikeList = p->spikeListSpec->head[inChan]) == NULL)
			continue;
		currentSpikeSpec = p->spikeListSpec->current[inChan];
		for (s = headSpikeList; (s != NULL) && (s->next != NULL) && (s->next !=
		  currentSpikeSpec); s = s->next) {
			spikeTime = s->timeIndex * p->dt;
			if (s->next && (spikeTime >= p->timeOffset) &&
			  ((spikeTimeHistIndex = (ChanLen) floor((s->timeIndex -
				  p->runningTimeOffsetIndex[inChan]) * p->convertDt)) <
				  data->outSignal->length)) {
				interval = s->next->timeIndex * p->dt - spikeTime;
				*(sumPtr + spikeTimeHistIndex) += interval;
				*(sumSqrsPtr + spikeTimeHistIndex) += interval * interval;
				*(countPtr + spikeTimeHistIndex) += 1.0;
			}
		}
	}
	/* Re-calculate statics */
	for (outChan = data->outSignal->offset; outChan <
	  data->outSignal->numChannels; outChan += SPIKE_REG_NUM_RETURNS) {
		inChan = outChan / SPIKE_REG_NUM_RETURNS;
		countPtr = p->countEarObj->outSignal->channel[inChan];
		sumPtr = data->outSignal->channel[outChan + SPIKE_REG_MEAN];
		sumSqrsPtr = data->outSignal->channel[outChan + SPIKE_REG_STANDARD_DEV];
		covarPtr = data->outSignal->channel[outChan + SPIKE_REG_CO_VARIANCE];
		for (i = 0; i < data->outSignal->length; i++, sumPtr++, sumSqrsPtr++,
		  covarPtr++, countPtr++)
			if (*countPtr > 0.0) {
				*sumPtr /= *countPtr;
				if (*countPtr > SPIKE_REG_MIN_SPIKES_FOR_STATISTICS) {
					diff = *sumSqrsPtr - *countPtr * *sumPtr * *sumPtr;
					if (diff < 0.0)
						diff = 0.0;
					*sumSqrsPtr = sqrt(diff / (*countPtr - 1.0));
					*covarPtr = *sumSqrsPtr / (*sumPtr - p->deadTime);
				}
			}
	}
	SetProcessContinuity_EarObject(p->countEarObj);
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

