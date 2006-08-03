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
#include "UtString.h"
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
	static const WChar	*funcName = wxT("Init_Analysis_ISIH");

	if (parSpec == GLOBAL) {
		if (interSIHPtr != NULL)
			Free_Analysis_ISIH();
		if ((interSIHPtr = (InterSIHPtr) malloc(sizeof(InterSIH))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (interSIHPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
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
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
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
	static const WChar *funcName = wxT("SetUniParList_Analysis_ISIH");
	UniParPtr	pars;

	if ((interSIHPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  ANALYSIS_ISIH_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = interSIHPtr->parList->pars;
	SetPar_UniParMgr(&pars[ANALYSIS_ISIH_ORDER], wxT("ORDER"),
	  wxT("Order of spike interactions (1 = 1st, 2 = 2nd, -1 = all order)."),
	  UNIPAR_INT,
	  &interSIHPtr->order, NULL,
	  (void * (*)) SetOrder_Analysis_ISIH);
	SetPar_UniParMgr(&pars[ANALYSIS_ISIH_EVENTTHRESHOLD], wxT("THRESHOLD"),
	  wxT("Event threshold (arbitrary units)."),
	  UNIPAR_REAL,
	  &interSIHPtr->eventThreshold, NULL,
	  (void * (*)) SetEventThreshold_Analysis_ISIH);
	SetPar_UniParMgr(&pars[ANALYSIS_ISIH_MAXINTERVAL], wxT("MAX_INTERVAL"),
	  wxT("Max. interval for histogram: -ve assumes end of signal (s)."),
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
	static const WChar	*funcName = wxT("GetUniParListPtr_Analysis_ISIH");

	if (interSIHPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (interSIHPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been "
		  "initialised. NULL returned."), funcName);
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
	static const WChar	*funcName = wxT("SetPars_Analysis_ISIH");
	BOOLN	ok;

	ok = TRUE;
	if (!SetOrder_Analysis_ISIH(order))
		ok = FALSE;
	if (!SetEventThreshold_Analysis_ISIH(eventThreshold))
		ok = FALSE;
	if (!SetMaxInterval_Analysis_ISIH(maxInterval))
		ok = FALSE;
	if (!ok)
		NotifyError(wxT("%s: Failed to set all module parameters.") ,funcName);
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
	static const WChar	*funcName = wxT("SetOrder_Analysis_ISIH");

	if (interSIHPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
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
	static const WChar	*funcName = wxT("SetEventThreshold_Analysis_ISIH");

	if (interSIHPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
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
	static const WChar	*funcName = wxT("SetMaxInterval_Analysis_ISIH");

	if (interSIHPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
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
	static const WChar	*funcName = wxT("CheckPars_Analysis_ISIH");
	BOOLN	ok;

	ok = TRUE;
	if (interSIHPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (!interSIHPtr->orderFlag) {
		NotifyError(wxT("%s: order variable not set."), funcName);
		ok = FALSE;
	}
	if (!interSIHPtr->eventThresholdFlag) {
		NotifyError(wxT("%s: eventThreshold variable not set."), funcName);
		ok = FALSE;
	}
	if (!interSIHPtr->maxIntervalFlag) {
		NotifyError(wxT("%s: maxInterval variable not set."), funcName);
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
	static const WChar	*funcName = wxT("PrintPars_Analysis_ISIH");

	if (!CheckPars_Analysis_ISIH()) {
		NotifyError(wxT("%s: Parameters have not been correctly set."),
		  funcName);
		return(FALSE);
	}
	DPrint(wxT("Inter-Spike Interval Histogram (ISIH) Module Parameters:-\n"));
	DPrint(wxT("\tOrder = "));
	if (interSIHPtr->order > 0)
		DPrint(wxT("%d,"), interSIHPtr->order);
	else
		DPrint(wxT("unlimited,"));
	DPrint(wxT("\tEvent threshold = %g units,\n"),
	  interSIHPtr->eventThreshold);
	DPrint(wxT("\tMax. interval = "));
	if (interSIHPtr->maxInterval > 0.0)
		DPrint(wxT("%g ms.\n"), MSEC(interSIHPtr->maxInterval));
	else
		DPrint(wxT("end of signal.\n"));
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_Analysis_ISIH(WChar *fileName)
{
	static const WChar	*funcName = wxT("ReadPars_Analysis_ISIH");
	BOOLN	ok;
	WChar	*filePath;
	int		order;
	double	eventThreshold, maxInterval;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = DSAM_fopen(filePath, "r")) == NULL) {
		NotifyError(wxT("%s: Cannot open data file '%s'.\n"), funcName,
		  filePath);
		return(FALSE);
	}
	DPrint(wxT("%s: Reading from '%s':\n"), funcName, filePath);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, wxT("%d"), &order))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &eventThreshold))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &maxInterval))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError(wxT("%s: Not enough lines, or invalid parameters, in "
		  "module parameter file '%s'."), funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_Analysis_ISIH(order, eventThreshold, maxInterval)) {
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
SetParsPointer_Analysis_ISIH(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Analysis_ISIH");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
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
	static const WChar	*funcName = wxT("InitModule_Analysis_ISIH");

	if (!SetParsPointer_Analysis_ISIH(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Analysis_ISIH(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = interSIHPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->CheckPars = CheckPars_Analysis_ISIH;
	theModule->Free = Free_Analysis_ISIH;
	theModule->GetUniParListPtr = GetUniParListPtr_Analysis_ISIH;
	theModule->PrintPars = PrintPars_Analysis_ISIH;
	theModule->ReadPars = ReadPars_Analysis_ISIH;
	theModule->ResetProcess = ResetProcess_Analysis_ISIH;
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
	static const WChar	*funcName = wxT("CheckData_Analysis_ISIH");
	double	signalDuration;

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	signalDuration = _InSig_EarObject(data, 0)->dt * _InSig_EarObject(data, 0)->
	  length;
	if (interSIHPtr->maxInterval > signalDuration) {
		NotifyError(wxT("%s: Maximum interval is longer than signal (value "
		  "must\nbe <= %g ms)."), funcName, signalDuration);
		return(FALSE);
	}
	return(TRUE);

}

/**************************** ResetProcess ************************************/

/*
 * This routine resets the process variables.
 */

void
ResetProcess_Analysis_ISIH(EarObjectPtr data)
{
	ResetOutSignal_EarObject(data);
	ResetListSpec_SpikeList(interSIHPtr->spikeListSpec, _OutSig_EarObject(
	  data));

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
	static const WChar *funcName = wxT("InitProcessVariables_Analysis_ISIH");
	InterSIHPtr	p = interSIHPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag ||
	  (data->timeIndex == PROCESS_START_TIME)) {
		if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
			FreeProcessVariables_Analysis_ISIH();
			if ((p->spikeListSpec = InitListSpec_SpikeList(
			  _InSig_EarObject(data, 0)->numChannels)) == NULL) {
				NotifyError(wxT("%s: Out of memory for spikeListSpec."),
				  funcName);
				return(FALSE);
			}
			p->updateProcessVariablesFlag = FALSE;
		}
		ResetProcess_Analysis_ISIH(data);
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
	static const WChar	*funcName = wxT("Calc_Analysis_ISIH");
	register	ChanData	 *outPtr;
	int		chan;
	ChanLen	spikeIntervalIndex;
	SpikeSpecPtr	p1, p2, headSpikeList, currentSpikeSpec;
	SignalDataPtr	inSignal, outSignal;
	InterSIHPtr	p = interSIHPtr;

	if (!data->threadRunFlag) {
		if (!CheckPars_Analysis_ISIH())
			return(FALSE);
		if (!CheckData_Analysis_ISIH(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		inSignal = _InSig_EarObject(data, 0);
		SetProcessName_EarObject(data, wxT("Inter-Spike Interval Histogram "
		  "(ISIH) analysis"));
		p->maxIntervalIndex = (p->maxInterval > 0.0)? (ChanLen) floor(p->
		  maxInterval / inSignal->dt + 0.5): inSignal->length;
		if (!InitOutSignal_EarObject(data, inSignal->numChannels, p->
		  maxIntervalIndex, inSignal->dt)) {
			NotifyError(wxT("%s: Cannot initialise output channels."),
			  funcName);
			return(FALSE);
		}
		SetStaticTimeFlag_SignalData(_OutSig_EarObject(data), TRUE);
		if (!InitProcessVariables_Analysis_ISIH(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		GenerateList_SpikeList(p->spikeListSpec, p->eventThreshold, inSignal);
		p->maxSpikes = (p->order > 0)? p->order: abs((int) inSignal->length);
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	outSignal = _OutSig_EarObject(data);
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		outPtr = outSignal->channel[chan];
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

