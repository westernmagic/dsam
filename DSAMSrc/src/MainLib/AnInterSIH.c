/**********************
 *
 * File:		AnInterSIH.c
 * Purpose:		This routine generates an inter-spike interval histogram from
 *				the EarObject's input signal.
 *				It counts all events over the specified "eventthreshold".
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 *				It adds to previous data if the output signal has already been
 *				initialised and the data->updateProcessFlag is set to FALSE.
 *				Otherwise it will overwrite the old signal or create a new
 *				signal as required.  The data->updateProcessFlag facility is
 *				useful for repeated runs.
 * Author:		L. P. O'Mard
 * Created:		19 Mar 1996
 * Updated:	
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
#include "AnInterSIH.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

InterSIHPtr	interSIHPtr = NULL;

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
Free_Analysis_ISIH(void)
{
	/* static const char	*funcName = "Free_Analysis_ISIH"; */

	if (interSIHPtr == NULL)
		return(FALSE);
	FreeProcessVariables_Analysis_ISIH();
	if (interSIHPtr->parList)
		FreeList_UniParMgr(&interSIHPtr->parList);
	if (interSIHPtr->parSpec == GLOBAL) {
		free(interSIHPtr);
		interSIHPtr = NULL;
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
Init_Analysis_ISIH(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_Analysis_ISIH";

	if (parSpec == GLOBAL) {
		if (interSIHPtr != NULL)
			Free_Analysis_ISIH();
		if ((interSIHPtr = (InterSIHPtr) malloc(sizeof(InterSIH))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (interSIHPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	interSIHPtr->parSpec = parSpec;
	interSIHPtr->updateProcessVariablesFlag = TRUE;
	interSIHPtr->orderFlag = TRUE;
	interSIHPtr->eventThresholdFlag = TRUE;
	interSIHPtr->maxIntervalFlag = TRUE;
	interSIHPtr->order = -1;
	interSIHPtr->eventThreshold = 0.0;
	interSIHPtr->maxInterval = -1.0;

	if (!SetUniParList_Analysis_ISIH()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_Analysis_ISIH();
		return(FALSE);
	}
	interSIHPtr->spikeListSpec = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_Analysis_ISIH(void)
{
	static const char *funcName = "SetUniParList_Analysis_ISIH";
	UniParPtr	pars;

	if ((interSIHPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  ANALYSIS_ISIH_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = interSIHPtr->parList->pars;
	SetPar_UniParMgr(&pars[ANALYSIS_ISIH_ORDER], "ORDER",
	  "Order of spike interactions (1 = 1st, 2 = 2nd, -1 = all order).",
	  UNIPAR_INT,
	  &interSIHPtr->order, NULL,
	  (void * (*)) SetOrder_Analysis_ISIH);
	SetPar_UniParMgr(&pars[ANALYSIS_ISIH_EVENTTHRESHOLD], "THRESHOLD",
	  "Event threshold (arbitrary units).",
	  UNIPAR_REAL,
	  &interSIHPtr->eventThreshold, NULL,
	  (void * (*)) SetEventThreshold_Analysis_ISIH);
	SetPar_UniParMgr(&pars[ANALYSIS_ISIH_MAXINTERVAL], "MAX_INTERVAL",
	  "Max. interval for histogram: -ve assumes end of signal (s).",
	  UNIPAR_REAL,
	  &interSIHPtr->maxInterval, NULL,
	  (void * (*)) SetMaxInterval_Analysis_ISIH);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Analysis_ISIH(void)
{
	static const char	*funcName = "GetUniParListPtr_Analysis_ISIH";

	if (interSIHPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (interSIHPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(interSIHPtr->parList);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_Analysis_ISIH(int order, double eventThreshold,
  double maxInterval)
{
	static const char	*funcName = "SetPars_Analysis_ISIH";
	BOOLN	ok;

	ok = TRUE;
	if (!SetOrder_Analysis_ISIH(order))
		ok = FALSE;
	if (!SetEventThreshold_Analysis_ISIH(eventThreshold))
		ok = FALSE;
	if (!SetMaxInterval_Analysis_ISIH(maxInterval))
		ok = FALSE;
	if (!ok)
		NotifyError("%s: Failed to set all module parameters." ,funcName);
	return(ok);

}

/****************************** SetOrder **************************************/

/*
 * This function sets the module's order parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOrder_Analysis_ISIH(int theOrder)
{
	static const char	*funcName = "SetOrder_Analysis_ISIH";

	if (interSIHPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	interSIHPtr->orderFlag = TRUE;
	interSIHPtr->order = theOrder;
	return(TRUE);

}

/****************************** SetEventThreshold *****************************/

/*
 * This function sets the module's eventThreshold parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetEventThreshold_Analysis_ISIH(double theEventThreshold)
{
	static const char	*funcName = "SetEventThreshold_Analysis_ISIH";

	if (interSIHPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	interSIHPtr->eventThresholdFlag = TRUE;
	interSIHPtr->eventThreshold = theEventThreshold;
	return(TRUE);

}

/****************************** SetMaxInterval ********************************/

/*
 * This function sets the module's maxInterval parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMaxInterval_Analysis_ISIH(double theMaxInterval)
{
	static const char	*funcName = "SetMaxInterval_Analysis_ISIH";

	if (interSIHPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	interSIHPtr->maxIntervalFlag = TRUE;
	interSIHPtr->maxInterval = theMaxInterval;
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
CheckPars_Analysis_ISIH(void)
{
	static const char	*funcName = "CheckPars_Analysis_ISIH";
	BOOLN	ok;

	ok = TRUE;
	if (interSIHPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!interSIHPtr->orderFlag) {
		NotifyError("%s: order variable not set.", funcName);
		ok = FALSE;
	}
	if (!interSIHPtr->eventThresholdFlag) {
		NotifyError("%s: eventThreshold variable not set.", funcName);
		ok = FALSE;
	}
	if (!interSIHPtr->maxIntervalFlag) {
		NotifyError("%s: maxInterval variable not set.", funcName);
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
PrintPars_Analysis_ISIH(void)
{
	static const char	*funcName = "PrintPars_Analysis_ISIH";

	if (!CheckPars_Analysis_ISIH()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Inter-Spike Interval Histogram (ISIH) Module "
	  "Parameters:-\n");
	DPrint("\tOrder = ");
	if (interSIHPtr->order > 0)
		DPrint("%d,", interSIHPtr->order);
	else
		DPrint("unlimited,");
	DPrint("\tEvent threshold = %g units,\n",
	  interSIHPtr->eventThreshold);
	DPrint("\tMax. interval = ");
	if (interSIHPtr->maxInterval > 0.0)
		DPrint("%g ms.\n", MSEC(interSIHPtr->maxInterval));
	else
		DPrint("end of signal.\n");
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_Analysis_ISIH(char *fileName)
{
	static const char	*funcName = "ReadPars_Analysis_ISIH";
	BOOLN	ok;
	char	*filePath;
	int		order;
	double	eventThreshold, maxInterval;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(filePath, "r")) == NULL) {
		NotifyError("%s: Cannot open data file '%s'.\n", funcName, filePath);
		return(FALSE);
	}
	DPrint("%s: Reading from '%s':\n", funcName, filePath);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, "%d", &order))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &eventThreshold))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &maxInterval))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "
		  "parameter file '%s'.", funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_Analysis_ISIH(order, eventThreshold, maxInterval)) {
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
SetParsPointer_Analysis_ISIH(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_Analysis_ISIH";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	interSIHPtr = (InterSIHPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Analysis_ISIH(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_Analysis_ISIH";

	if (!SetParsPointer_Analysis_ISIH(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_Analysis_ISIH(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = interSIHPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->CheckPars = CheckPars_Analysis_ISIH;
	theModule->Free = Free_Analysis_ISIH;
	theModule->GetUniParListPtr = GetUniParListPtr_Analysis_ISIH;
	theModule->PrintPars = PrintPars_Analysis_ISIH;
	theModule->ReadPars = ReadPars_Analysis_ISIH;
	theModule->RunProcess = Calc_Analysis_ISIH;
	theModule->SetParsPointer = SetParsPointer_Analysis_ISIH;
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
CheckData_Analysis_ISIH(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_Analysis_ISIH";
	double	signalDuration;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	signalDuration = data->inSignal[0]->dt * data->inSignal[0]->length;
	if (interSIHPtr->maxInterval > signalDuration) {
		NotifyError("%s: Maximum interval is longer than signal (value must\n"
		  "be <= %g ms).", funcName, signalDuration);
		return(FALSE);
	}
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
InitProcessVariables_Analysis_ISIH(EarObjectPtr data)
{
	static const char *funcName = "InitProcessVariables_Analysis_ISIH";

	if (interSIHPtr->updateProcessVariablesFlag || data->updateProcessFlag ||
	  (data->timeIndex == PROCESS_START_TIME)) {
		if (interSIHPtr->updateProcessVariablesFlag ||
		  data->updateProcessFlag) {
			FreeProcessVariables_Analysis_ISIH();
			if ((interSIHPtr->spikeListSpec = InitListSpec_SpikeList(
			  data->inSignal[0]->numChannels)) == NULL) {
				NotifyError("%s: Out of memory for spikeListSpec.", funcName);
				return(FALSE);
			}
			interSIHPtr->updateProcessVariablesFlag = FALSE;
		}
		ResetListSpec_SpikeList(interSIHPtr->spikeListSpec);
	}
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

void
FreeProcessVariables_Analysis_ISIH(void)
{
	FreeListSpec_SpikeList(&interSIHPtr->spikeListSpec);

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
Calc_Analysis_ISIH(EarObjectPtr data)
{
	static const char	*funcName = "Calc_Analysis_ISIH";
	register	ChanData	 *outPtr;
	int		chan;
	ChanLen	maxIntervalIndex, spikeIntervalIndex;
	SpikeSpecPtr	p1, p2, headSpikeList, currentSpikeSpec;
	InterSIHPtr	p = interSIHPtr;

	if (!data->threadRunFlag) {
		if (!CheckPars_Analysis_ISIH())
			return(FALSE);
		if (!CheckData_Analysis_ISIH(data)) {
			NotifyError("%s: Process data invalid.", funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, "Inter-Spike Interval Histogram (ISIH) "
		  "analysis");
		p->maxIntervalIndex = (p->maxInterval > 0.0)? (ChanLen) floor(p->
		  maxInterval / data->inSignal[0]->dt + 0.5): data->inSignal[0]->length;
		if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels,
		  p->maxIntervalIndex, data->inSignal[0]->dt)) {
			NotifyError("%s: Cannot initialise output channels.", funcName);
			return(FALSE);
		}
		SetStaticTimeFlag_SignalData(data->outSignal, TRUE);
		if (!InitProcessVariables_Analysis_ISIH(data)) {
			NotifyError("%s: Could not initialise the process variables.",
			  funcName);
			return(FALSE);
		}
		GenerateList_SpikeList(p->spikeListSpec, p->eventThreshold, data->
		  inSignal[0]);
		p->maxSpikes = (p->order > 0)? p->order: abs((int) data->inSignal[0]->
		  length);
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	for (chan = data->outSignal->offset; chan < data->outSignal->numChannels;
	  chan++) {
		outPtr = data->outSignal->channel[chan];
		headSpikeList = p->spikeListSpec->head[chan];
		currentSpikeSpec = p->spikeListSpec->current[chan];
		for (p1 = headSpikeList; p1 != currentSpikeSpec; p1 = p1->next)
			for (p2 = p1->next; (p2 != NULL) && (p2 != currentSpikeSpec) &&
			  (p2->number - p1->number <= p->maxSpikes); p2 = p2->next)
				if ((spikeIntervalIndex = p2->timeIndex - p1->timeIndex) <
				  p->maxIntervalIndex)
					outPtr[spikeIntervalIndex]++;
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

