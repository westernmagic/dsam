/**********************
 *
 * File:		AnHistogram.c
 * Purpose:		This program calculates period or post-stimulus time histograms.
 * Comments:	Written using ModuleProducer version 1.3.
 *				A negative binwidth will instruct instruct the process to use
 *				the previous signal's dt value.
 *				As for most of the analysis routines, this routine is not
 *				segmented mode friendly.
 *				13-02-98 LPO: Corrected time offset checking.
 *				04-03-99 LPO: Corrected the double precsision error using SGI's
 *				by introducing the 'DBL_GREATER macro'.
 * Author:		L. P. O'Mard
 * Created:		17 Nov 1995
 * Updated:		04 Mar 1999
 * Copyright:	(c) 1999, University of Essex.
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
#include "AnHistogram.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

HistogramPtr	histogramPtr = NULL;

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
Free_Analysis_Histogram(void)
{

	if (histogramPtr == NULL)
		return(FALSE);
	FreeProcessVariables_Analysis_Histogram();
	if (histogramPtr->parList)
		FreeList_UniParMgr(&histogramPtr->parList);
	if (histogramPtr->parSpec == GLOBAL) {
		free(histogramPtr);
		histogramPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitDetectionModeList *************************/

/*
 * This routine intialises the Dectection Mode list array.
 */

BOOLN
InitDetectionModeList_Analysis_Histogram(void)
{
	static NameSpecifier	detectionModeList[] = {

					{"SPIKE",		HISTOGRAM_DETECT_SPIKES },
					{"CONTINUOUS",	HISTOGRAM_CONTINUOUS },
					{"",	HISTOGRAM_DETECT_NULL }

				};


	histogramPtr->detectionModeList = detectionModeList;
	return(TRUE);

}

/****************************** InitOutputModeList ****************************/

/*
 * This routine intialises the output Mode list array.
 */

BOOLN
InitOutputModeList_Analysis_Histogram(void)
{
	static NameSpecifier	outputModeList[] = {

					{"BIN_COUNTS",	HISTOGRAM_OUTPUT_BIN_COUNTS },
					{"SPIKE_RATE",	HISTOGRAM_OUTPUT_SPIKE_RATE },
					{"",			HISTOGRAM_OUTPUT_NULL }

				};
	histogramPtr->outputModeList = outputModeList;
	return(TRUE);

}

/****************************** InitTypeModeList ******************************/

/*
 * This routine intialises the Type Mode list array.
 */

BOOLN
InitTypeModeList_Analysis_Histogram(void)
{
	static NameSpecifier	typeModeList[] = {

					{"PSTH",	HISTOGRAM_PSTH },
					{"PH",		HISTOGRAM_PH },
					{"",		HISTOGRAM_TYPE_NULL }

				};
	histogramPtr->typeModeList = typeModeList;
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
Init_Analysis_Histogram(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_Analysis_Histogram";

	if (parSpec == GLOBAL) {
		if (histogramPtr != NULL)
			Free_Analysis_Histogram();
		if ((histogramPtr = (HistogramPtr) malloc(sizeof(Histogram))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (histogramPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	histogramPtr->parSpec = parSpec;
	histogramPtr->updateProcessVariablesFlag = TRUE;
	histogramPtr->detectionModeFlag = FALSE;
	histogramPtr->typeModeFlag = FALSE;
	histogramPtr->eventThresholdFlag = FALSE;
	histogramPtr->binWidthFlag = FALSE;
	histogramPtr->periodFlag = FALSE;
	histogramPtr->outputModeFlag = TRUE;
	histogramPtr->timeOffsetFlag = FALSE;
	histogramPtr->detectionMode = 0;
	histogramPtr->outputMode = HISTOGRAM_OUTPUT_BIN_COUNTS;
	histogramPtr->typeMode = 0;
	histogramPtr->eventThreshold = 0.0;
	histogramPtr->binWidth = 0.0;
	histogramPtr->period = 0.0;
	histogramPtr->timeOffset = 0.0;

	InitDetectionModeList_Analysis_Histogram();
	InitOutputModeList_Analysis_Histogram();
	InitTypeModeList_Analysis_Histogram();
	if (!SetUniParList_Analysis_Histogram()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_Analysis_Histogram();
		return(FALSE);
	}
	histogramPtr->riseDetected = NULL;
	histogramPtr->numPeriods = 0;
	histogramPtr->dataBuffer = NULL;
	histogramPtr->offsetIndex = 0;
	histogramPtr->bufferSamples = 0;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_Analysis_Histogram(void)
{
	static const char *funcName = "SetUniParList_Analysis_Histogram";
	UniParPtr	pars;

	if ((histogramPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  ANALYSIS_HISTOGRAM_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = histogramPtr->parList->pars;
	SetPar_UniParMgr(&pars[ANALYSIS_HISTOGRAM_DETECTIONMODE], "DETECT_MODE",
	  "Detection mode: 'spike' (detection) or 'continuous' (signal).",
	  UNIPAR_NAME_SPEC,
	  &histogramPtr->detectionMode, histogramPtr->detectionModeList,
	  (void * (*)) SetDetectionMode_Analysis_Histogram);
	SetPar_UniParMgr(&pars[ANALYSIS_HISTOGRAM_OUTPUT_MODE], "OUTPUT_MODE",
	  "Output mode: 'bin_counts'  or 'spike_rate'.",
	  UNIPAR_NAME_SPEC,
	  &histogramPtr->outputMode, histogramPtr->outputModeList,
	  (void * (*)) SetOutputMode_Analysis_Histogram);
	SetPar_UniParMgr(&pars[ANALYSIS_HISTOGRAM_TYPEMODE], "TYPE_MODE",
	  "Histogram type: 'PSTH', or 'PH'.",
	  UNIPAR_NAME_SPEC,
	  &histogramPtr->typeMode, histogramPtr->typeModeList,
	  (void * (*)) SetTypeMode_Analysis_Histogram);
	SetPar_UniParMgr(&pars[ANALYSIS_HISTOGRAM_EVENTTHRESHOLD], "THRESHOLD",
	  "Event threshold for bin counts (units as appropriate).",
	  UNIPAR_REAL,
	  &histogramPtr->eventThreshold, NULL,
	  (void * (*)) SetEventThreshold_Analysis_Histogram);
	SetPar_UniParMgr(&pars[ANALYSIS_HISTOGRAM_BINWIDTH], "BIN_WIDTH",
	  "Bin width for the histogram: -ve assumes dt for previous signal (s).",
	  UNIPAR_REAL,
	  &histogramPtr->binWidth, NULL,
	  (void * (*)) SetBinWidth_Analysis_Histogram);
	SetPar_UniParMgr(&pars[ANALYSIS_HISTOGRAM_PERIOD], "PERIOD",
	  "Period for period histogram: used in 'PH' mode only (s).",
	  UNIPAR_REAL,
	  &histogramPtr->period, NULL,
	  (void * (*)) SetPeriod_Analysis_Histogram);
	SetPar_UniParMgr(&pars[ANALYSIS_HISTOGRAM_TIMEOFFSET], "OFFSET",
	  "Time offset for histogram (s).",
	  UNIPAR_REAL,
	  &histogramPtr->timeOffset, NULL,
	  (void * (*)) SetTimeOffset_Analysis_Histogram);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Analysis_Histogram(void)
{
	static const char	*funcName = "GetUniParListPtr_Analysis_Histogram";

	if (histogramPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (histogramPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(histogramPtr->parList);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_Analysis_Histogram(char *detectionMode, char *typeMode,
  double eventThreshold, double binWidth, double period, double timeOffset)
{
	static const char	*funcName = "SetPars_Analysis_Histogram";
	BOOLN	ok;

	ok = TRUE;
	if (!SetDetectionMode_Analysis_Histogram(detectionMode))
		ok = FALSE;
	if (!SetTypeMode_Analysis_Histogram(typeMode))
		ok = FALSE;
	if (!SetEventThreshold_Analysis_Histogram(eventThreshold))
		ok = FALSE;
	if (!SetBinWidth_Analysis_Histogram(binWidth))
		ok = FALSE;
	if (!SetPeriod_Analysis_Histogram(period))
		ok = FALSE;
	if (!SetTimeOffset_Analysis_Histogram(timeOffset))
		ok = FALSE;
	if (!ok)
		NotifyError("%s: Failed to set all module parameters." ,funcName);
	return(ok);

}

/****************************** SetDetectionMode ******************************/

/*
 * This function sets the module's detectionMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDetectionMode_Analysis_Histogram(char *theDetectionMode)
{
	static const char	*funcName = "SetDetectionMode_Analysis_Histogram";
	int specifier;

	if (histogramPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theDetectionMode,
	  histogramPtr->detectionModeList)) == HISTOGRAM_DETECT_NULL) {
		NotifyError("%s: Illegal detection mode  (%s).", funcName,
		  theDetectionMode);
		return(FALSE);
	}
	histogramPtr->detectionModeFlag = TRUE;
	histogramPtr->detectionMode = specifier;
	return(TRUE);

}

/****************************** SetOutputMode *********************************/

/*
 * This function sets the module's output mode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOutputMode_Analysis_Histogram(char *theOutputMode)
{
	static const char	*funcName = "SetOutputMode_Analysis_Histogram";
	int		specifier;

	if (histogramPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theOutputMode,
	  histogramPtr->outputModeList)) == HISTOGRAM_OUTPUT_NULL) {
		NotifyError("%s: Illegal output mode (%s).", funcName,
		  theOutputMode);
		return(FALSE);
	}
	histogramPtr->outputModeFlag = TRUE;
	histogramPtr->outputMode = specifier;
	return(TRUE);

}

/****************************** SetTypeMode ***********************************/

/*
 * This function sets the module's typeMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTypeMode_Analysis_Histogram(char *theTypeMode)
{
	static const char	*funcName = "SetTypeMode_Analysis_Histogram";
	int		specifier;

	if (histogramPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theTypeMode,
	  histogramPtr->typeModeList)) == HISTOGRAM_TYPE_NULL) {
		NotifyError("%s: Illegal type mode (%s).", funcName,
		  theTypeMode);
		return(FALSE);
	}
	histogramPtr->typeModeFlag = TRUE;
	histogramPtr->updateProcessVariablesFlag = TRUE;
	histogramPtr->typeMode = specifier;
	return(TRUE);

}

/****************************** SetEventThreshold *****************************/

/*
 * This function sets the module's eventThreshold parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetEventThreshold_Analysis_Histogram(double theEventThreshold)
{
	static const char	*funcName = "SetEventThreshold_Analysis_Histogram";

	if (histogramPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any required checks here. ***/
	histogramPtr->eventThresholdFlag = TRUE;
	histogramPtr->eventThreshold = theEventThreshold;
	return(TRUE);

}

/****************************** SetBinWidth ***********************************/

/*
 * This function sets the module's binWidth parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetBinWidth_Analysis_Histogram(double theBinWidth)
{
	static const char	*funcName = "SetBinWidth_Analysis_Histogram";

	if (histogramPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any required checks here. ***/
	histogramPtr->binWidthFlag = TRUE;
	histogramPtr->binWidth = theBinWidth;
	return(TRUE);

}

/****************************** SetPeriod *************************************/

/*
 * This function sets the module's period parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPeriod_Analysis_Histogram(double thePeriod)
{
	static const char	*funcName = "SetPeriod_Analysis_Histogram";

	if (histogramPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any required checks here. ***/
	histogramPtr->periodFlag = TRUE;
	histogramPtr->updateProcessVariablesFlag = TRUE;
	histogramPtr->period = thePeriod;
	return(TRUE);

}

/****************************** SetTimeOffset *********************************/

/*
 * This function sets the module's timeOffset parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTimeOffset_Analysis_Histogram(double theTimeOffset)
{
	static const char	*funcName = "SetTimeOffset_Analysis_Histogram";

	if (histogramPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any required checks here. ***/
	histogramPtr->timeOffsetFlag = TRUE;
	histogramPtr->timeOffset = theTimeOffset;
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
CheckPars_Analysis_Histogram(void)
{
	static const char	*funcName = "CheckPars_Analysis_Histogram";
	BOOLN	ok;

	ok = TRUE;
	if (histogramPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!histogramPtr->detectionModeFlag) {
		NotifyError("%s: detectionMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!histogramPtr->typeModeFlag) {
		NotifyError("%s: typeMode variable not set.", funcName);
		ok = FALSE;
	}
	if ((histogramPtr->detectionMode == HISTOGRAM_DETECT_SPIKES) && 
	  !histogramPtr->eventThresholdFlag) {
		NotifyError("%s: Event threshold variable not set.", funcName);
		ok = FALSE;
	}
	if (!histogramPtr->binWidthFlag) {
		NotifyError("%s: binWidth variable not set.", funcName);
		ok = FALSE;
	}
	if (histogramPtr->typeMode == HISTOGRAM_PH) {
	   if (!histogramPtr->periodFlag) {
			NotifyError("%s: period variable not set.", funcName);
			ok = FALSE;
		}
		if (histogramPtr->binWidth > histogramPtr->period) {
			NotifyError("%s: The bin width is too small for the period value "
			  "(%g ms & %g ms respectively).", funcName,
			  MSEC(histogramPtr->binWidth), MSEC(histogramPtr->period));
			ok = FALSE;
		}
	}
	if (!histogramPtr->timeOffsetFlag) {
		NotifyError("%s: timeOffset variable not set.", funcName);
		ok = FALSE;
	}
	if ((histogramPtr->detectionMode == HISTOGRAM_CONTINUOUS) && (histogramPtr->
	  outputMode == HISTOGRAM_OUTPUT_SPIKE_RATE)) {
		NotifyError("%s: The spike rate histogram modes cannot be used in "
		  "continuous detection mode.", funcName);
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
PrintPars_Analysis_Histogram(void)
{
	static const char	*funcName = "PrintPars_Analysis_Histogram";

	if (histogramPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckPars_Analysis_Histogram()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Histogram analysis module parameters:-\n");
	DPrint("\tDetection mode = %s,", histogramPtr->detectionModeList[
	  histogramPtr->detectionMode].name);
	DPrint("\tOutput mode = %s,", histogramPtr->outputModeList[histogramPtr->
	  outputMode].name);
	DPrint("\tType mode = %s,\n", histogramPtr->typeModeList[histogramPtr->
	  typeMode].name);
	if (histogramPtr->detectionMode == HISTOGRAM_DETECT_SPIKES)
		DPrint("\tEvent threshold = %g units,\n", histogramPtr->eventThreshold);
	DPrint("\tBin width = ");
	if (histogramPtr->binWidth <= 0.0)
		DPrint("<prev. signal dt>,");
	else
		DPrint("%g ms,", MSEC(histogramPtr->binWidth));
	switch (histogramPtr->typeMode) {
	case HISTOGRAM_PSTH:
	case HISTOGRAM_PH:
		DPrint("\tPeriod = %g ms\n", MSEC(histogramPtr->period));
		break;
	default:
		DPrint("\tTime offset = %g ms\n", MSEC(histogramPtr->timeOffset));
	}
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_Analysis_Histogram(char *fileName)
{
	static const char	*funcName = "ReadPars_Analysis_Histogram";
	BOOLN	ok;
	char	*filePath;
	char	detectionMode[MAXLINE], typeMode[MAXLINE];
	double	eventThreshold, binWidth, period, timeOffset;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(filePath, "r")) == NULL) {
		NotifyError("%s: Cannot open data file '%s'.\n", funcName, filePath);
		return(FALSE);
	}
	DPrint("%s: Reading from '%s':\n", funcName, filePath);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, "%s", &detectionMode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%s", &typeMode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &eventThreshold))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &binWidth))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &period))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &timeOffset))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "
		  "parameter file '%s'.", funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_Analysis_Histogram(detectionMode, typeMode, eventThreshold,
	  binWidth, period, timeOffset)) {
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
SetParsPointer_Analysis_Histogram(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_Analysis_Histogram";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	histogramPtr = (HistogramPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Analysis_Histogram(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_Analysis_Histogram";

	if (!SetParsPointer_Analysis_Histogram(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_Analysis_Histogram(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = histogramPtr;
	theModule->CheckPars = CheckPars_Analysis_Histogram;
	theModule->Free = Free_Analysis_Histogram;
	theModule->GetUniParListPtr = GetUniParListPtr_Analysis_Histogram;
	theModule->PrintPars = PrintPars_Analysis_Histogram;
	theModule->ReadPars = ReadPars_Analysis_Histogram;
	theModule->RunProcess = Calc_Analysis_Histogram;
	theModule->SetParsPointer = SetParsPointer_Analysis_Histogram;
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
CheckData_Analysis_Histogram(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_Analysis_Histogram";
	double	signalDuration;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInit_SignalData(data->inSignal[0],
	  "CheckData_Analysis_Histogram"))
		return(FALSE);
	if ((histogramPtr->binWidth > 0.0) && (histogramPtr->binWidth <
	  data->inSignal[0]->dt)) {
		NotifyError("%s: Bin width is too small for sampling interval (%g ms).",
		  funcName, MSEC(histogramPtr->binWidth));
		return(FALSE);
	}
	signalDuration = data->inSignal[0]->length * data->inSignal[0]->dt;
	if (histogramPtr->typeMode == HISTOGRAM_PH) {
		if (histogramPtr->period < data->inSignal[0]->dt) {
			NotifyError("%s: The period (%g ms) is less than the input signal "
			  "sampling interval (%g ms)!", funcName, MILLI(histogramPtr->
			  period), MILLI(data->inSignal[0]->dt));
			return(FALSE);
		}
	}
	return(TRUE);

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables if necessary
 * at sets the initial process values as required.
 * It assumes that all of the parameters for the module have been correctly
 * initialised.
 */

BOOLN
InitProcessVariables_Analysis_Histogram(EarObjectPtr data)
{
	static const char *funcName = "InitProcessVariables_Analysis_Histogram";
	int		i;
	ChanLen	bufferLength;
	HistogramPtr	p = histogramPtr;
	
	if (p->updateProcessVariablesFlag || data->updateProcessFlag ||
	  (data->timeIndex == PROCESS_START_TIME)) {
		if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
			FreeProcessVariables_Analysis_Histogram();
			if ((p->riseDetected = (BOOLN *) calloc(
			  data->outSignal->numChannels, sizeof(BOOLN))) == NULL) {
				NotifyError("%s: Out of memory for 'riseDetected' array.",
				  funcName);
				return(FALSE);
			}
			if ((p->dataBuffer = Init_EarObject("NULL")) == NULL) {
				NotifyError("%s: Could not initialise data buffer EarObject",
				  funcName);
				return(FALSE);
			}
			p->updateProcessVariablesFlag = FALSE;
		}
		p->numPeriods = 0;
		ResetProcess_EarObject(p->dataBuffer);
		bufferLength = (p->typeMode == HISTOGRAM_PSTH)? 1: (ChanLen) floor(p->
		  period / data->inSignal[0]->dt + 0.5);
		if (!InitOutSignal_EarObject(p->dataBuffer, data->outSignal->
		  numChannels, bufferLength, data->inSignal[0]->dt)) {
			NotifyError("%s: Cannot initialise channels for PH Buffer.",
			  funcName);
			return(FALSE);
		}
		p->bufferSamples = 0;
		p->offsetIndex = (ChanLen) floor(p->timeOffset / data->inSignal[0]->dt +
		  0.5);
		p->extraSample = (p->offsetIndex)? 0: 1;
		for (i = 0; i < data->outSignal->numChannels; i++)
			p->riseDetected[i] = FALSE;
		if (p->offsetIndex < data->inSignal[0]->length)
			for (i = 0; i < data->outSignal->numChannels; i++)
				p->dataBuffer->outSignal->channel[i][0] = data->inSignal[0]->
				  channel[0][p->offsetIndex + p->extraSample - 1];
	}
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

BOOLN
FreeProcessVariables_Analysis_Histogram(void)
{
	if (histogramPtr->riseDetected) {
		free(histogramPtr->riseDetected);
		histogramPtr->riseDetected = NULL;
	}
	if (histogramPtr->dataBuffer)
		Free_EarObject(&histogramPtr->dataBuffer);
	histogramPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/**************************** PushDataBuffer **********************************/

/*
 * This routine pushes sections from the end of the input signal into the data
 * buffer.
 */

void
PushDataBuffer_Analysis_Histogram(EarObjectPtr data, ChanLen lastSamples)
{
	register	ChanData	*inPtr, *buffPtr;
	int		chan;
	ChanLen	i;

	for (chan = 0; chan < data->inSignal[0]->numChannels; chan++) {
		inPtr = data->inSignal[0]->channel[chan] + data->inSignal[0]->length -
		  lastSamples;
		buffPtr = histogramPtr->dataBuffer->outSignal->channel[chan] +
		  histogramPtr->bufferSamples;
		for (i = 0; i < lastSamples; i++)
			*buffPtr++ = *inPtr++;
	}
	histogramPtr->bufferSamples = lastSamples;

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
 * The 'extraSample' and 'extraTimeInterval' are only required when the time
 * offset is zero. 
 */

BOOLN
Calc_Analysis_Histogram(EarObjectPtr data)
{
	static const char	*funcName = "Calc_Analysis_Histogram";
	register	ChanData	*inPtr, *outPtr, *buffPtr, binSum;
	int		chan;
	double	nextCutOff, nextBinCutOff, dt, binWidth, time, totalBinDuration;
	double	extraTimeInterval;
	ChanLen	i, bufferSamples, processLength, availableLength;
	HistogramPtr	p = histogramPtr;

	if (!CheckPars_Analysis_Histogram())
		return(FALSE);
	if (!CheckData_Analysis_Histogram(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "Histogram Analysis Module");
	dt = data->inSignal[0]->dt;
	if (p->typeMode == HISTOGRAM_PSTH)
		p->period = data->inSignal[0]->length * dt - p->timeOffset;
	binWidth = (p->binWidth <= 0.0)? dt: p->binWidth;
	if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels,
	  (ChanLen) floor(p->period / binWidth + 0.5), binWidth)) {
		NotifyError("%s: Cannot initialise output channels.", funcName);
		return(FALSE);
	}
	SetOutputTimeOffset_SignalData(data->outSignal, binWidth);
	SetStaticTimeFlag_SignalData(data->outSignal, TRUE);
	if (!InitProcessVariables_Analysis_Histogram(data)) {
		NotifyError("%s: Could not initialise the process variables.",
		  funcName);
		return(FALSE);
	}
	if (p->offsetIndex > data->inSignal[0]->length)
		processLength = 0;
	else {
		availableLength = data->inSignal[0]->length - p->offsetIndex - p->
		  extraSample + p->bufferSamples + 1;
		processLength = (p->typeMode == HISTOGRAM_PSTH)? availableLength:
		  (ChanLen) floor(floor(availableLength * dt / p->period) * p->period /
		  dt + DBL_EPSILON);
	}
	if (processLength) {
		if ((p->detectionMode == HISTOGRAM_DETECT_SPIKES) && (p->outputMode ==
		  HISTOGRAM_OUTPUT_SPIKE_RATE) && p->numPeriods) {
			totalBinDuration = binWidth * p->numPeriods;
			for (chan = 0; chan < data->inSignal[0]->numChannels; chan++) {
				outPtr = data->outSignal->channel[chan];
				for (i = 0; i < data->outSignal->length; i++)
					*outPtr++ *= totalBinDuration;
			}
		}
		for (chan = 0; chan < data->inSignal[0]->numChannels; chan++) {
			inPtr = data->inSignal[0]->channel[chan] + p->offsetIndex + p->
			  extraSample;
			outPtr = data->outSignal->channel[chan];
			buffPtr = p->dataBuffer->outSignal->channel[chan];
			bufferSamples = p->bufferSamples;
			extraTimeInterval = dt * p->extraSample;
			nextCutOff = p->period - extraTimeInterval;
			nextBinCutOff = binWidth - extraTimeInterval;
			for (i = 0, binSum = 0; i < processLength; i++) {
				switch (p->detectionMode) {
				case HISTOGRAM_DETECT_SPIKES:
					if (!p->riseDetected[chan])
						p->riseDetected[chan] = (*inPtr > *buffPtr);
					else {
						if (*inPtr < *buffPtr) {
							p->riseDetected[chan] = FALSE;
							if (*buffPtr > p->eventThreshold)
								binSum++;
						}
					}
					break;
				case HISTOGRAM_CONTINUOUS:
					binSum += *buffPtr;
					break;
				} /* switch */
				if (!bufferSamples)
					*buffPtr = *(inPtr++);
				else {
					buffPtr++;
					bufferSamples--;
				}
				time = (i + 1) * dt;
				if (DBL_GREATER(time, nextBinCutOff)) {
					if ((ChanLen) (outPtr - data->outSignal->channel[chan]) <
					  data->outSignal->length) /* - because of rounding errors*/
						*outPtr++ += binSum;
					binSum = 0;
					nextBinCutOff += binWidth;
				}
				if (DBL_GREATER(time, nextCutOff)) {
					outPtr = data->outSignal->channel[chan];
					nextCutOff += p->period;
					p->numPeriods++;
				}
			}
			if ((ChanLen) (outPtr - data->outSignal->channel[chan]) <
			  data->outSignal->length)	/* Last incomplete bin */
				*outPtr +=  binSum;
		}
		if (p->typeMode == HISTOGRAM_PSTH)
			p->numPeriods++;
		if ((p->detectionMode == HISTOGRAM_DETECT_SPIKES) && (p->outputMode ==
		  HISTOGRAM_OUTPUT_SPIKE_RATE)) {
			totalBinDuration = binWidth * p->numPeriods;
			for (chan = 0; chan < data->inSignal[0]->numChannels; chan++) {
				outPtr = data->outSignal->channel[chan];
				for (i = 0; i < data->outSignal->length; i++)
					*outPtr++ /= totalBinDuration;
			}
		}
	}
	if (p->offsetIndex + p->extraSample) {
		if (p->offsetIndex < data->inSignal[0]->length) {
			if (processLength < availableLength)
				PushDataBuffer_Analysis_Histogram(data, availableLength -
				  processLength);
			p->offsetIndex = 0;
		} else {
			p->offsetIndex -= data->inSignal[0]->length;
			PushDataBuffer_Analysis_Histogram(data, 1);
		}
		p->extraSample = 0;
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}
