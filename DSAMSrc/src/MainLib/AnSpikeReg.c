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
	/* static const char	*funcName = "Free_Analysis_SpikeRegularity"; */

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
	static const char	*funcName = "Init_Analysis_SpikeRegularity";

	if (parSpec == GLOBAL) {
		if (spikeRegPtr != NULL)
			Free_Analysis_SpikeRegularity();
		if ((spikeRegPtr = (SpikeRegPtr) malloc(sizeof(SpikeReg))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (spikeRegPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	spikeRegPtr->parSpec = parSpec;
	spikeRegPtr->updateProcessVariablesFlag = TRUE;
	spikeRegPtr->eventThresholdFlag = TRUE;
	spikeRegPtr->binWidthFlag = TRUE;
	spikeRegPtr->timeOffsetFlag = TRUE;
	spikeRegPtr->timeRangeFlag = TRUE;
	spikeRegPtr->eventThreshold = 0.0;
	spikeRegPtr->binWidth = -1.0;
	spikeRegPtr->timeOffset = 0.0;
	spikeRegPtr->timeRange = -10.0;

	if (!SetUniParList_Analysis_SpikeRegularity()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_Analysis_SpikeRegularity();
		return(FALSE);
	}
	spikeRegPtr->spikeListSpec= NULL;
	spikeRegPtr->countEarObj = NULL;
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
	static const char *funcName = "SetUniParList_Analysis_SpikeRegularity";
	UniParPtr	pars;

	if ((spikeRegPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  ANALYSIS_SPIKEREGULARITY_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = spikeRegPtr->parList->pars;
	SetPar_UniParMgr(&pars[ANALYSIS_SPIKEREGULARITY_EVENTTHRESHOLD],
	  "THRESHOLD",
	  "Event threshold (arbitrary units).",
	  UNIPAR_REAL,
	  &spikeRegPtr->eventThreshold, NULL,
	  (void * (*)) SetEventThreshold_Analysis_SpikeRegularity);
	SetPar_UniParMgr(&pars[ANALYSIS_SPIKEREGULARITY_BINWIDTH], "BIN_WIDTH",
	  "Bin width for spike events (s).",
	  UNIPAR_REAL,
	  &spikeRegPtr->binWidth, NULL,
	  (void * (*)) SetBinWidth_Analysis_SpikeRegularity);
	SetPar_UniParMgr(&pars[ANALYSIS_SPIKEREGULARITY_TIMEOFFSET], "OFFSET",
	  "Time offset for start of analysis (s).",
	  UNIPAR_REAL,
	  &spikeRegPtr->timeOffset, NULL,
	  (void * (*)) SetTimeOffset_Analysis_SpikeRegularity);
	  SetPar_UniParMgr(&pars[ANALYSIS_SPIKEREGULARITY_TIMERANGE], "MAX_RANGE",
	  "Max. time range: -ve assumes to end of signal (s).",
	  UNIPAR_REAL,
	  &spikeRegPtr->timeRange, NULL,
	  (void * (*)) SetTimeRange_Analysis_SpikeRegularity);
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
	static const char	*funcName = "GetUniParListPtr_Analysis_SpikeRegularity";

	if (spikeRegPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (spikeRegPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
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
SetPars_Analysis_SpikeRegularity(double eventThreshold, double binWidth,
  double timeOffset, double timeRange)
{
	static const char	*funcName = "SetPars_Analysis_SpikeRegularity";
	BOOLN	ok;

	ok = TRUE;
	if (!SetEventThreshold_Analysis_SpikeRegularity(eventThreshold))
		ok = FALSE;
	if (!SetBinWidth_Analysis_SpikeRegularity(binWidth))
		ok = FALSE;
	if (!SetTimeOffset_Analysis_SpikeRegularity(timeOffset))
		ok = FALSE;
	if (!SetTimeRange_Analysis_SpikeRegularity(timeRange))
		ok = FALSE;
	if (!ok)
		NotifyError("%s: Failed to set all module parameters." ,funcName);
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
	static const char	*funcName =
	  "SetEventThreshold_Analysis_SpikeRegularity";

	if (spikeRegPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	spikeRegPtr->eventThresholdFlag = TRUE;
	spikeRegPtr->eventThreshold = theEventThreshold;
	return(TRUE);

}

/****************************** SetBinWidth ***********************************/

/*
 * This function sets the module's binWidth parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetBinWidth_Analysis_SpikeRegularity(double theBinWidth)
{
	static const char	*funcName = "SetBinWidth_Analysis_SpikeRegularity";

	if (spikeRegPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	spikeRegPtr->binWidthFlag = TRUE;
	spikeRegPtr->updateProcessVariablesFlag = TRUE;
	spikeRegPtr->binWidth = theBinWidth;
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
	static const char	*funcName = "SetTimeOffset_Analysis_SpikeRegularity";

	if (spikeRegPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (theTimeOffset < 0.0) {
		NotifyError("%s: Time offset must be greater than zero (%g ms).",
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
	static const char	*funcName = "SetTimeRange_Analysis_SpikeRegularity";

	if (spikeRegPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	spikeRegPtr->timeRangeFlag = TRUE;
	spikeRegPtr->updateProcessVariablesFlag = TRUE;
	spikeRegPtr->timeRange = theTimeRange;
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
	static const char	*funcName = "CheckPars_Analysis_SpikeRegularity";
	BOOLN	ok;

	ok = TRUE;
	if (spikeRegPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!spikeRegPtr->eventThresholdFlag) {
		NotifyError("%s: eventThreshold variable not set.", funcName);
		ok = FALSE;
	}
	if (!spikeRegPtr->binWidthFlag) {
		NotifyError("%s: binWidth variable not set.", funcName);
		ok = FALSE;
	}
	if (!spikeRegPtr->timeOffsetFlag) {
		NotifyError("%s: timeOffset variable not set.", funcName);
		ok = FALSE;
	}
	if (!spikeRegPtr->timeRangeFlag) {
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
PrintPars_Analysis_SpikeRegularity(void)
{
	static const char	*funcName = "PrintPars_Analysis_SpikeRegularity";

	if (!CheckPars_Analysis_SpikeRegularity()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Spike Regularity Analysis Module Parameters:-\n");
	DPrint("\tEvent threshold = %g units,",
	  spikeRegPtr->eventThreshold);
	DPrint("\tBin width = ");
	if (spikeRegPtr->binWidth <= 0.0)
		DPrint("<prev. signal dt>,\n");
	else
		DPrint("%g ms,\n", MSEC(spikeRegPtr->binWidth));
	DPrint("\tTime offset = %g ms,", MSEC(spikeRegPtr->timeOffset));
	DPrint("\tTime range = ");
	if (spikeRegPtr->timeRange > 0.0)
		DPrint("%g ms.\n", MSEC(spikeRegPtr->timeRange));
	else
		DPrint("end of signal.\n");
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_Analysis_SpikeRegularity(char *fileName)
{
	static const char	*funcName = "ReadPars_Analysis_SpikeRegularity";
	BOOLN	ok;
	char	*filePath;
	double	eventThreshold, binWidth, timeOffset, timeRange;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(filePath, "r")) == NULL) {
		NotifyError("%s: Cannot open data file '%s'.\n", funcName, filePath);
		return(FALSE);
	}
	DPrint("%s: Reading from '%s':\n", funcName, filePath);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, "%lf", &eventThreshold))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &binWidth))
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
	if (!SetPars_Analysis_SpikeRegularity(eventThreshold, binWidth, timeOffset,
	  timeRange)) {
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
SetParsPointer_Analysis_SpikeRegularity(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_Analysis_SpikeRegularity";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
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
	static const char	*funcName = "InitModule_Analysis_SpikeRegularity";

	if (!SetParsPointer_Analysis_SpikeRegularity(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_Analysis_SpikeRegularity(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = spikeRegPtr;
	theModule->CheckPars = CheckPars_Analysis_SpikeRegularity;
	theModule->Free = Free_Analysis_SpikeRegularity;
	theModule->GetUniParListPtr = GetUniParListPtr_Analysis_SpikeRegularity;
	theModule->PrintPars = PrintPars_Analysis_SpikeRegularity;
	theModule->ReadPars = ReadPars_Analysis_SpikeRegularity;
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
	static const char	*funcName = "CheckData_Analysis_SpikeRegularity";
	double	signalDuration, maxTime;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	if ((spikeRegPtr->binWidth > 0.0) && (spikeRegPtr->binWidth <
	 data->inSignal[0]->dt)) {
		NotifyError("%s: Bin width (%g ms) is too small for sampling\n"
		  "interval (%g ms).", funcName, MSEC(spikeRegPtr->binWidth),
		  MSEC(data->inSignal[0]->dt));
		return(FALSE);
	}
	signalDuration = _GetDuration_SignalData(data->inSignal[0]);
	maxTime = (spikeRegPtr->timeRange > 0.0)? spikeRegPtr->timeRange:
	  signalDuration;
	if ((maxTime - spikeRegPtr->timeOffset) > signalDuration) {
		NotifyError("%s: Time offset (%g ms) to range (%g ms) is "
		  "longer than signal (%g ms).", funcName,
		  MSEC(spikeRegPtr->timeOffset), MSEC(maxTime), signalDuration);
		return(FALSE);
	}
	/*** Put additional checks here. ***/
	return(TRUE);

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
	static const char *funcName =
	  "InitProcessVariables_Analysis_SpikeRegularity";

	if (spikeRegPtr->updateProcessVariablesFlag || data->updateProcessFlag ||
	  (data->timeIndex == PROCESS_START_TIME)) {
		if (spikeRegPtr->updateProcessVariablesFlag ||
		  data->updateProcessFlag) {
			FreeProcessVariables_Analysis_SpikeRegularity();
			if ((spikeRegPtr->spikeListSpec = InitListSpec_SpikeList(
			  data->inSignal[0]->numChannels)) == NULL) {
				NotifyError("%s: Out of memory for spikeListSpec.", funcName);
				return(FALSE);
			}
			spikeRegPtr->countEarObj = Init_EarObject("NULL");
			spikeRegPtr->updateProcessVariablesFlag = FALSE;
		}
		if (!InitOutSignal_EarObject(spikeRegPtr->countEarObj,
		  data->inSignal[0]->numChannels, data->outSignal->length,
		  data->outSignal->dt)) {
			NotifyError("%s: Cannot initialise countEarObj.", funcName);
			return(FALSE);
		}
		ResetListSpec_SpikeList(spikeRegPtr->spikeListSpec);
	}
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
	FreeListSpec_SpikeList(&spikeRegPtr->spikeListSpec);
	Free_EarObject(&spikeRegPtr->countEarObj);

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
 * A rounding value of 0.4999.... is used because of high-precision errors
 * on my PC: a spike time of 0.013900000000001 was being set - it's not my
 * fault.
 */

BOOLN
Calc_Analysis_SpikeRegularity(EarObjectPtr data)
{
	static const char	*funcName = "Calc_Analysis_SpikeRegularity";
	register ChanData	*sumPtr, *covarPtr, *sumSqrsPtr, *countPtr, diff;
	int		outChan, inChan;
	double	interval, dt, spikeTime, maxSpikeTime, maxTime, binWidth;
	double	deadTime = 0.0;
	ChanLen	i, spikeTimeHistIndex, timeRangeIndex;
	ChanLen	timeOffsetHistIndex;
	ChanData		oldMean;
	SpikeSpecPtr	p, headSpikeList, currentSpikeSpec;

	if (!CheckPars_Analysis_SpikeRegularity())
		return(FALSE);
	if (!CheckData_Analysis_SpikeRegularity(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "Spike Regularity Analysis");
	dt = data->inSignal[0]->dt;
	binWidth = (spikeRegPtr->binWidth <= 0.0)? dt: spikeRegPtr->binWidth;
	maxTime = (spikeRegPtr->timeRange > 0.0)?
	  spikeRegPtr->timeRange: _GetDuration_SignalData(data->inSignal[0]);
	timeRangeIndex = (ChanLen) floor((maxTime - spikeRegPtr->timeOffset) /
	  binWidth + 0.5);
	if (!InitOutSignal_EarObject(data, (uShort) (data->inSignal[0]->
	  numChannels * SPIKE_REG_NUM_RETURNS), timeRangeIndex, binWidth)) {
		NotifyError("%s: Cannot initialise sumEarObj.", funcName);
		return(FALSE);
	}
	SetOutputTimeOffset_SignalData(data->outSignal, spikeRegPtr->timeOffset);
	SetInterleaveLevel_SignalData(data->outSignal, (uShort) (
	  data->inSignal[0]->interleaveLevel * SPIKE_REG_NUM_RETURNS));
	if (!InitProcessVariables_Analysis_SpikeRegularity(data)) {
		NotifyError("%s: Could not initialise the process variables.",
		  funcName);
		return(FALSE);
	}
	/* First recover previous sums from signal. */
	for (outChan = 0; outChan < data->outSignal->numChannels; outChan +=
	  SPIKE_REG_NUM_RETURNS) {
		inChan = outChan / SPIKE_REG_NUM_RETURNS;
		countPtr = spikeRegPtr->countEarObj->outSignal->channel[inChan];
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
	/* Add additional sums. */
	GenerateList_SpikeList(spikeRegPtr->spikeListSpec,
	  spikeRegPtr->eventThreshold, data->inSignal[0]);
	maxSpikeTime = _GetDuration_SignalData(data->outSignal);
	maxSpikeTime -=	spikeRegPtr->timeOffset /* - THINK C complains */;
	timeOffsetHistIndex = (ChanLen) floor(spikeRegPtr->timeOffset /
	  data->outSignal->dt + 0.5);
	for (outChan = 0; outChan < data->outSignal->numChannels; outChan +=
	  SPIKE_REG_NUM_RETURNS) {
		inChan = outChan / SPIKE_REG_NUM_RETURNS;
		countPtr = spikeRegPtr->countEarObj->outSignal->channel[inChan];
		sumPtr = data->outSignal->channel[outChan + SPIKE_REG_MEAN];
		sumSqrsPtr = data->outSignal->channel[outChan + SPIKE_REG_STANDARD_DEV];
		headSpikeList = spikeRegPtr->spikeListSpec->head[inChan];
		deadTime = (headSpikeList->timeIndex - 1) * dt -
		  spikeRegPtr->timeOffset;
		currentSpikeSpec = spikeRegPtr->spikeListSpec->current[inChan];
		for (p = headSpikeList; (p != NULL) && (p->next != NULL) && (p->next !=
		  currentSpikeSpec); p = p->next) {
			spikeTime = p->timeIndex * dt;
			if ((spikeTime >= spikeRegPtr->timeOffset) && (spikeTime <=
			  maxSpikeTime)) {
			  	spikeTimeHistIndex = (ChanLen) floor(spikeTime /
			  	  data->outSignal->dt + 0.49999999) - timeOffsetHistIndex;
				interval = p->next->timeIndex * dt - spikeTime;
				*(sumPtr + spikeTimeHistIndex) += interval;
				*(sumSqrsPtr + spikeTimeHistIndex) += interval * interval;
				*(countPtr + spikeTimeHistIndex) += 1.0;
			}
		}
	}
	/* Re-calculate statics */
	for (outChan = 0; outChan < data->outSignal->numChannels; outChan +=
	  SPIKE_REG_NUM_RETURNS) {
		inChan = outChan / SPIKE_REG_NUM_RETURNS;
		countPtr = spikeRegPtr->countEarObj->outSignal->channel[inChan];
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
					*covarPtr = *sumSqrsPtr / (*sumPtr - deadTime);
				}
			}
	}
	SetProcessContinuity_EarObject(spikeRegPtr->countEarObj);
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

