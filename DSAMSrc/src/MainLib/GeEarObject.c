/******************
 *
 * File:		EarObject.c
 * Purpose:		This module contains the routines for the EarObject Module.
 * Comments:	To compile without the ModuleMgr support, uncomment
 *				"#define _NO_MODULMGR" in the "GeEarObject.h" file.
 *				12-03-97 LPO: Added SetMaxInSignals routine.
 *				29-04-98 LPO: The SetProcessName_EarObject routine will now
 *				indescriminately replace any previous names.
 *				01-05-98 LPO: Created the GetInSignalRef_EarObject() routine
 *				so that it can be used by the Simulation specification module
 *				for connection to input signals.
 *				15-10-98 LPO: The 'ResetProcess_...' routine now resets the
 *				process time to the PROCESS_START_TIME.
 *				27-01-99 LPO: The 'FreeAll_' routine now sets the
 *				'earObjectCount' to zero.
 * Authors:		L. P. O'Mard
 * Created:		18 Feb 1993
 * Updated:		27 Jan 1998
 * Copyright:	(c) 1999, University of Essex
 * 
 ******************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
 
#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "UtRandom.h"
#include "UtString.h"

/******************************************************************************/
/*************************** Global Variables *********************************/
/******************************************************************************/

EarObjHandle	earObjectCount = 0;
EarObjRefPtr	mainEarObjectList = NULL;
BOOLN	(* ResetProcess_EarObject)(EarObjectPtr) =
		  ResetProcessStandard_EarObject;

/******************************************************************************/
/********************************* Subroutines and functions ******************/
/******************************************************************************/

/**************************** Init ********************************************/

/*
 * This routine takes care of the correct initialisation of an EarObject: the
 * input and output signals are set to NULL, as required later, and the
 * pointer to the prepared object is returned.
 * The Module connected with the process is supplied as an argument.  This
 * is a change from the previous version.
 * The routine returns NULL if an error occurs.
 * All initialised EarObjects are registered in the main list.
 * This new version initialises the input signals array to NULL, then input
 * signals are dynamically added when processes are connected.
 * 
 */

EarObjectPtr
Init_EarObject(WChar *moduleName)
{
	static const WChar *funcName = wxT("Init_EarObject");
	EarObjectPtr	data;

	if ((data = (EarObjectPtr) malloc(sizeof(EarObject))) == NULL) {
		NotifyError(wxT("%s: Out of memory for EarObject - used with the '%s' "
		  "module."), funcName, moduleName);
		return(NULL);
	}
	data->processName = NULL;
	data->localOutSignalFlag = FALSE;
	data->externalDataFlag = FALSE;
	data->initThreadRunFlag = FALSE;
	data->threadRunFlag = FALSE;
	data->useThreadsFlag = FALSE;
	data->chainInitRunFlag = FALSE;
	data->updateCustomersFlag = TRUE;
	data->updateProcessFlag = TRUE;
	data->firstSectionFlag = TRUE;
	data->numInSignals = 0;
	data->timeIndex = PROCESS_START_TIME;
	data->randPars = NULL;
	data->inSignal = NULL;
	_OutSig_EarObject(data) = NULL;
	data->customerList = NULL;
	data->supplierList = NULL;
	data->handle = earObjectCount++;	/* Unique handle for each EarObject. */
	if (!AddEarObjRef_EarObject(&mainEarObjectList, data)) {
		NotifyError(wxT("%s: Could not register new EarObject."), funcName);
		Free_EarObject(&data);
		return(NULL);
	}
#   ifndef _NO_MODULEMGR
	if ((data->module = Init_ModuleMgr(moduleName)) == NULL) {
		NotifyError(wxT("%s: Could not set module type."), funcName);
		Free_EarObject(&data);
		return(NULL);
	}
#	endif
	data->clientData = NULL;
	data->numThreads = 1;
	data->threadIndex = 0;
	data->numSubProcesses = 0;
	data->threadProcs = NULL;
	data->subProcessList = NULL;
	return(data);

} /* Init_EarObject */

/**************************** AddInSignal *************************************/

/*
 * This routine adds a new input signal to an EarObject.
 */
 
BOOLN
AddInSignal_EarObject(EarObjectPtr data)
{
	static const WChar *funcName = wxT("AddInSignal_EarObject");
	SignalDataPtr	*signals;

	if (!data) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if ((signals = (SignalDataPtr *) realloc(data->inSignal, ((data->
	  numInSignals + 1) * sizeof(SignalDataPtr)))) == NULL) {
		NotifyError(wxT("%s: Out of memory for EarObject input signals "
		  "(EarObject process %lu)."), funcName, data->handle);
		return(FALSE);
	}
	signals[data->numInSignals++] = NULL;
	data->inSignal = signals;
	return(TRUE);

}

/**************************** DelInSignal *************************************/

/*
 * This routine removes an input signal from an EarObject.
 * The signal to be remove must be identified and moved to the end of the
 * list so that it is lost with a call to realloc.
 * This routine also updates all this EarObject's suppliers for which the input
 * reference may have changed, i.e. all those below the deleted signal in the
 * array.
 */
 
BOOLN
DelInSignal_EarObject(EarObjectPtr data, SignalDataPtr signal)
{
	static const WChar *funcName = wxT("DelInSignal_EarObject");
	BOOLN	found = FALSE;
	int		i = 0, j;
	SignalDataPtr	*signals;
	EarObjRefPtr	p;	

	if (!data) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!data->numInSignals)
		return(TRUE);
	while (!found && (i < data->numInSignals))
		found = (data->inSignal[i++] == signal);

	if (!found) {
		NotifyError(wxT("%s: Signal pointer not found for '%s' EarObject "
		  "process."), funcName, data->processName);
		return(FALSE);
	}
	for (j = i; j < data->numInSignals; j++)
		data->inSignal[j - 1] = data->inSignal[j];

	if (data->numInSignals == 1) {
		free(data->inSignal);
		data->inSignal = NULL;
	} else {
		if ((signals = (SignalDataPtr *) realloc(data->inSignal, ((data->
		  numInSignals - 1) * sizeof(SignalDataPtr)))) == NULL) {
			NotifyError(wxT("%s: memory reallocation problem for EarObject "
			  "input signals ('%s' Earobject process)."), funcName, data->
			  processName);
			return(FALSE);
		}
		data->inSignal = signals;
	}
	data->numInSignals--;
	for (p = data->supplierList; p != NULL; p = p->next)
		if (p->inSignalRef > data->numInSignals)
			p->inSignalRef--;
	return(TRUE);

}

/**************************** FreeOutSignal ***********************************/

/*
 * This routine ensures releases the memory for an EarObject's output signal.
 * Only locally create signals will be free'd, i.e. each EarObject must release
 * the signals which it created.
 */
 
void
FreeOutSignal_EarObject(EarObjectPtr data)
{
	if (!data)
		return;
	if (data->localOutSignalFlag)
		Free_SignalData(&_OutSig_EarObject(data));
	else
		_OutSig_EarObject(data) = NULL;

}

/**************************** Free ********************************************/

/*
 * This routine ensures that all pointers of the object have their memory
 * released before memory of the object itself is released.
 * Only locally create signals will be free'd, i.e. each EarObject must release
 * the signals which it created.
 * When releasing the memory for the 'threadProcs' remember that these
 * EarObjects are copies and have no personal allocated space.
 */
 
void
Free_EarObject(EarObjectPtr *theObject)
{
	if (*theObject == NULL)
		return;
	if ((*theObject)->inSignal)
		free((*theObject)->inSignal);
	if ((*theObject)->threadProcs)
		FreeThreadProcs_EarObject(*theObject);
	FreeSubProcessList_EarObject(*theObject);
	FreeOutSignal_EarObject(*theObject);
	/* This next line unregisters the object from the main list. */
	FreeEarObjRef_EarObject(&mainEarObjectList, (*theObject)->handle);
	/* This next line will set the customer input signals to NULL. */
	UpdateCustomers_EarObject(*theObject);
	RemoveEarObjRefs_EarObject(*theObject);
	if ((*theObject)->processName != NULL)
		free((*theObject)->processName);
#	ifndef _NO_MODULEMGR
	Free_ModuleMgr(&(*theObject)->module);
#	endif
	FreeEarObjRefList_EarObject(&(*theObject)->customerList);
	FreeEarObjRefList_EarObject(&(*theObject)->supplierList);
	if ((*theObject)->randPars)
		FreePars_Random(&(*theObject)->randPars);
	free(*theObject);
	*theObject = NULL;		
	
} /* Free_EarObject */

/**************************** FreeAll *****************************************/

/*
 * This routine frees the memory for all of the initialised EarObjects from
 * the main register list.
 * It does not, however, set the pointer variables to NULL - watch this.
 */

void
FreeAll_EarObject(void)
{
	EarObjectPtr	tempPtr;

	while (mainEarObjectList != NULL) {
		tempPtr = mainEarObjectList->earObject;
		Free_EarObject(&tempPtr);
	}
	earObjectCount = 0;

}

/**************************** SetProcessName **********************************/

/*
 * Sets a process name for an EarObject.  This sort of information will be used
 * in process titles and dislpays.
 * Space is allocated for the name, if it has not already been initialised.
 * The name can only be set once: repeated calls will do nothing.
 * This routine uses the variable argument format - like 'printf' does.
 */
 
void
SetProcessName_EarObject(EarObjectPtr theObject, WChar *format, ...)
{
	static const WChar *funcName = wxT("SetProcessName_EarObject");
	WChar	string[MAXLINE];
	va_list	args;
	
	if (theObject->processName != NULL)
		free(theObject->processName);
#	if DSAM_USE_UNICODE
	WChar	newFormat[MAXLINE];
	ConvIOFormat_Utility_String(newFormat, format, MAXLINE);
	format = newFormat;
#	endif
	va_start(args, format);
	DSAM_vsnprintf(string, MAXLINE, format, args);
	va_end(args);
	if ((theObject->processName = (WChar *) calloc(DSAM_strlen(string) + 1,
	  sizeof(WChar))) == NULL) {
		NotifyError(wxT("%s: Out of memory, cannot allocate name."), funcName);
		NotifyError(wxT("%s: string = '%s'."), funcName, string);
		return;
	}
	DSAM_strcpy(theObject->processName, string);

} /* SetProcessName_EarObject */

/**************************** SetNewOutSignal *********************************/

/*
 * This function checks the parameters of an EarObject's output  SignalData
 * data structure and decides whether or not to create a new data signal
 * element, i.e. if the dimensions of the SignalData object have changed.
 * It destroys the old output signal and re-creates it if necessary, in which
 * case the updateCustomersFlag is set.
 * If the sampling interval is changed, then the "updateProcessFlag" is set.
 * I have introduced a check for processes that have been used by the 
 * NULL process, RunModel_ModuleMgr_Null.  In these processes the 'outSignal'
 * will not be NULL, but 'localOutSignalFlag' will be FALSE.
 * It returns FALSE if it fails in any way.
 */

BOOLN
SetNewOutSignal_EarObject(EarObjectPtr data, uShort numChannels, ChanLen length,
  double samplingInterval)
{
	static const WChar *funcName = wxT("SetNewOutSignal_EarObject");
	BOOLN	createNewSignal = TRUE, deletedOldOutSignal = FALSE;
	SignalData	oldOutSignal;

	if (data->chainInitRunFlag)
		return(TRUE);
	if (data->localOutSignalFlag && (_OutSig_EarObject(data) != NULL)) {
		if (!_OutSig_EarObject(data)->dtFlag || (_OutSig_EarObject(data)->dt !=
		  samplingInterval) || !_OutSig_EarObject(data)->lengthFlag ||
		  (_OutSig_EarObject(data)->length != length) ||
		  !_OutSig_EarObject(data)->numChannels || (_OutSig_EarObject(data)->numChannels !=
		  numChannels)) {
		 	oldOutSignal = *_OutSig_EarObject(data);
			Free_SignalData(&_OutSig_EarObject(data));
			data->updateCustomersFlag = TRUE;
			deletedOldOutSignal = TRUE;
		} else
			createNewSignal = FALSE;
	} else
		data->updateCustomersFlag = TRUE;/* Customers must be updated anyway. */
	
	if (createNewSignal) {
		_OutSig_EarObject(data) = Init_SignalData(funcName);
		SetLength_SignalData(_OutSig_EarObject(data), length);
		SetSamplingInterval_SignalData(_OutSig_EarObject(data), samplingInterval);
		SetOutputTimeOffset_SignalData(_OutSig_EarObject(data), samplingInterval);
		if (!InitChannels_SignalData(_OutSig_EarObject(data), numChannels,
		  data->externalDataFlag)) {
			NotifyError(wxT("%s: Cannot initialise output channels for "
			  "EarObject '%s'."), funcName, POSSIBLY_NULL_STRING_PTR(data->
			  processName));
			Free_SignalData(&_OutSig_EarObject(data));
			data->localOutSignalFlag = FALSE;
			return(FALSE);
		}
	}
	data->localOutSignalFlag = TRUE;
	if (deletedOldOutSignal) {
		data->updateProcessFlag = (oldOutSignal.dt != samplingInterval) ||
		  (oldOutSignal.length != length) || (oldOutSignal.numChannels !=
		   numChannels);
		ResetSignalContinuity_EarObject(data, (data->updateProcessFlag)? NULL:
		  &oldOutSignal);
	} else
		ResetSignalContinuity_EarObject(data, NULL);
	return(TRUE);

}

/**************************** ResetSignalContinuity ***************************/

/*
 * This routine resets continuity fields of an output signal from the input
 * signal.  These are fields such as the ramp flag etc.
 * N.B. pointers from the "oldOutSignal" are not valid, as their memory has
 * been released.
 */
 
void
ResetSignalContinuity_EarObject(EarObjectPtr data, SignalDataPtr oldOutSignal)
{
	static const WChar *funcName = wxT("ResetSignalContinuity_EarObject");

	if (!CheckPars_SignalData(_OutSig_EarObject(data))) {
		NotifyError(wxT("%s: Output signal not correctly set."), funcName);
		exit(1);
	}
	if (!data->inSignal || (data->inSignal[0] == NULL)) {
		if (oldOutSignal != NULL) {
			_OutSig_EarObject(data)->rampFlag = oldOutSignal->rampFlag;
			_OutSig_EarObject(data)->interleaveLevel = oldOutSignal->interleaveLevel;
			_OutSig_EarObject(data)->timeIndex = oldOutSignal->timeIndex;
		} else {
			_OutSig_EarObject(data)->rampFlag = FALSE;
			_OutSig_EarObject(data)->interleaveLevel =
			  SIGNALDATA_DEFAULT_INTERLEAVE_LEVEL;
			_OutSig_EarObject(data)->timeIndex = PROCESS_START_TIME;
		}
		ResetInfo_SignalData(_OutSig_EarObject(data));
	} else {
		_OutSig_EarObject(data)->rampFlag = data->inSignal[0]->rampFlag;
		if (_OutSig_EarObject(data)->numChannels != data->inSignal[0]->numChannels)
			SetInterleaveLevel_SignalData(_OutSig_EarObject(data),
			  SIGNALDATA_DEFAULT_INTERLEAVE_LEVEL);
		else
			SetInterleaveLevel_SignalData(_OutSig_EarObject(data),
			  data->inSignal[0]->interleaveLevel);
		_OutSig_EarObject(data)->timeIndex = data->inSignal[0]->timeIndex;
		_OutSig_EarObject(data)->outputTimeOffset = data->inSignal[0]->outputTimeOffset;
		if (!_OutSig_EarObject(data)->localInfoFlag)
			CopyInfo_SignalData(_OutSig_EarObject(data), data->inSignal[0]);
	}

}

/**************************** InitOutSignal ***********************************/

/*
 * This routine sets the appriopriate fields of a signal and initialises the
 * channels.
 * Signal setting is usually carried out automatically by the module routines
 * using this routine.
 * The signal channels are initialised to zero.
 * It will overwrite any previous signal information, and also allows the
 * signal length to be re-set.
 * It will copy the input signal's the rampFlag setting if the signal is set.
 * 20-1-94 L.P.O'Mard this routine has been changed to accept the signal length
 * rather than the signal duration.
 * 16-8-94 L.P.O'Mard: This version does not reset an existing output signal to
 * zero, unless the updateProcessFlag is set.
 */
 
BOOLN
InitOutSignal_EarObject(EarObjectPtr data, uShort numChannels, ChanLen length,
  double samplingInterval)
{
	static const WChar *funcName = wxT("InitOutSignal_EarObject");
	
	if (samplingInterval == 0.0) {
		NotifyError(wxT("%s: Time step is zero for EarObject '%s'!"), funcName,
		  POSSIBLY_NULL_STRING_PTR(data->processName));
		return(FALSE);
	}
	if (!SetNewOutSignal_EarObject(data, numChannels, length,
	  samplingInterval)) {
		NotifyError(wxT("%s: Could not set output signal."), funcName);
		return(FALSE);
	}
	return(TRUE);
	
}

/**************************** ResetOutSignal **********************************/

/*
 * This routine resets an output signal to zero.if the 'updateProcessFlag' is
 * set and the signal is not external.
 */
 
void
ResetOutSignal_EarObject(EarObjectPtr data)
{
	int		i;
	register	ChanData	*dataPtr;
	ChanLen	j;

	if (!data->updateProcessFlag || data->externalDataFlag)
		return;
	for (i = _OutSig_EarObject(data)->offset; i < _OutSig_EarObject(data)->numChannels; i++)
		for (j = 0, dataPtr = _OutSig_EarObject(data)->channel[i]; j < _OutSig_EarObject(data)->
		  length; j++)
			*(dataPtr++) = 0.0;

}

/**************************** InitOutFromInSignal *****************************/

/*
 * This routine initialises or re-initialises the output signal of data set,
 * from the input signal.  Signal setting is usually carried out automatically
 * by the module routines using this routine.
 * It will overwrite any previous signal information, and also allows the
 * signal length to be re-set.
 * If the argument numChannels is zero, then the routine initialises the same
 * number of channels as for the input signal.
 * If the input signal is binaural, then the output signal will contain twice
 * as many output channels, interleaved, e.g. LRLRLR... - 'numChannels' times.
 * It is convenient to use the "ResetSignalContinuity_EarObject()" routine,
 * but we need to save the interleavel level which is set by the
 * "SetChannelsFromSignal_SignalData(...)" routine.
 */
 
BOOLN
InitOutFromInSignal_EarObject(EarObjectPtr data, uShort numChannels)
{
	static const WChar *funcName = wxT("InitOutFromInSignal_EarObject");
	uShort	channelsToSet;

	if (!data->inSignal) {
		NotifyError(wxT("%s: No connected input processes."), funcName);
		return(FALSE);
	}
	if (!CheckPars_SignalData(data->inSignal[0])) {
		NotifyError(wxT("%s: Signal not correctly initialised."), funcName);
		return(FALSE);
	}
	channelsToSet = (numChannels == 0) ? data->inSignal[0]->numChannels:
	  numChannels;

	if (!SetNewOutSignal_EarObject(data, channelsToSet,
		data->inSignal[0]->length, data->inSignal[0]->dt)) {
		NotifyError(wxT("%s: Could not set output signal."), funcName);
		return(FALSE);
	}
	SetChannelsFromSignal_SignalData(_OutSig_EarObject(data), data->inSignal[0]);
	if (_OutSig_EarObject(data)->numChannels != data->inSignal[0]->numChannels)
		SetInterleaveLevel_SignalData(_OutSig_EarObject(data), data->inSignal[0]->
		  numChannels);
	else
		SetInterleaveLevel_SignalData(_OutSig_EarObject(data), data->inSignal[0]->
		  interleaveLevel);
	return(TRUE);
	
}

/**************************** InitOutDataFromInSignal *************************/

/*
 * This routine initialises or re-initialises the output signal data
 * from the input signal.
 * It will overwrite any previous signal information.
 * This routine has been revised from the 'InitOutFromInSignal_EarObject'
 * which caused complications with threaded operation.
 * This function should be used in conjuction with the
 * 'InitOutTypeFromInSignal_EarObject' routine to get the same functionality
 * of the 'InitOutFromInSignal_EarObject' routine.
 */
 
void
InitOutDataFromInSignal_EarObject(EarObjectPtr data)
{
	SetChannelsFromSignal_SignalData(_OutSig_EarObject(data), data->inSignal[0]);

}

/**************************** InitOutTypeFromInSignal *************************/

/*
 * This routine initialises or re-initialises the output signal type, from the
 * input signal type.  Signal setting is usually carried out automatically by
 * the module routines using this routine.
 * It allows the signal length to be re-set.
 * If the argument numChannels is zero, then the routine initialises the same
 * number of channels as for the input signal.
 * If the input signal is binaural, then the output signal will contain twice
 * as many output channels, interleaved, e.g. LRLRLR... - 'numChannels' times.
 * This routine has been revised from the 'InitOutFromInSignal_EarObject'
 * which caused complications with threaded operation.
 * It does not call 'SetChannelsFromSignal_SignalData' routine because it is
 * used within the pre-thread initialisation section of a process.
 */
 
BOOLN
InitOutTypeFromInSignal_EarObject(EarObjectPtr data, uShort numChannels)
{
	static const WChar *funcName = wxT("InitOutTypeFromInSignal_EarObject");
	uShort	channelsToSet;

	if (!data->inSignal) {
		NotifyError(wxT("%s: No connected input processes."), funcName);
		return(FALSE);
	}
	if (!CheckPars_SignalData(data->inSignal[0])) {
		NotifyError(wxT("%s: Signal not correctly initialised."), funcName);
		return(FALSE);
	}
	channelsToSet = (numChannels == 0) ? data->inSignal[0]->numChannels:
	  numChannels;

	if (!SetNewOutSignal_EarObject(data, channelsToSet,
		data->inSignal[0]->length, data->inSignal[0]->dt)) {
		NotifyError(wxT("%s: Could not set output signal."), funcName);
		return(FALSE);
	}
	if (_OutSig_EarObject(data)->numChannels != data->inSignal[0]->numChannels)
		SetInterleaveLevel_SignalData(_OutSig_EarObject(data), data->inSignal[0]->
		  numChannels);
	else
		SetInterleaveLevel_SignalData(_OutSig_EarObject(data), data->inSignal[0]->
		  interleaveLevel);
	return(TRUE);
	
}

/**************************** PrintProcessName ********************************/

/*
 * This routine outputs the process name to the standard output, provided that
 * it has been set, otherwise it will output "<no name set>".
 * The message string should be in the form "text....%s.....more text (if 
 * required).".
 * It assumes that all EarObjects are initialised to NULL when declared.
 */
 
void
PrintProcessName_EarObject(WChar *message, EarObjectPtr data)
{
	if (data->processName != NULL)
		DSAM_printf(message, data->processName);
	else
		DSAM_printf(message, wxT("<no name set>"));
}

/**************************** ConnectOutSignalToIn ****************************/

/*
 * This routine connects the output signal of the supplier to the input signal
 * of the customer.  It does this by merely putting the customer into the
 * supplier's customer list, and adding the supplier to the customer's
 * supplier list.
 * This routine tests for NULL EarObjects.
 */
 
BOOLN
ConnectOutSignalToIn_EarObject(EarObjectPtr supplier, EarObjectPtr customer)
{
	static const WChar *funcName = wxT("ConnectOutSignalToIn_EarObject");

	if (supplier == NULL) {
		NotifyError(wxT("%s: Supplier EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (customer == NULL) {
		NotifyError(wxT("%s: Customer EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (supplier == customer) {
		NotifyError(wxT("%s: Attempted to connect EarObject to itself!"),
		  funcName);
		return(FALSE);
	}
	if (!AddInSignal_EarObject(customer)) {
		NotifyError(wxT("%s: Could not add input signal for customer (%lu)."),
		  funcName, customer->handle);
		return(FALSE);
	}
	if (!AddEarObjRef_EarObject(&supplier->customerList, customer))
		return(FALSE);
	if (!AddEarObjRef_EarObject(&customer->supplierList, supplier))
		return(FALSE);
	if (supplier->outSignal != NULL)
		customer->inSignal[customer->numInSignals - 1] = supplier->outSignal;
	return(TRUE);
	
}

/**************************** DisconnectOutSignalFromIn ***********************/

/*
 * This routine disconnects the output signal of the supplier from the input
 * signal of the customer.  It does this by removing the customer from the
 * supplier's customer list, and the supplier from the customer's supplier list.
 * This routine tests for NULL EarObjects.
 */
 
BOOLN
DisconnectOutSignalFromIn_EarObject(EarObjectPtr supplier,
  EarObjectPtr customer)
{
	static const WChar *funcName = wxT("DisconnectOutSignalFromIn_EarObject");
	
	if (supplier == NULL) {
		NotifyError(wxT("%s: Supplier EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (customer == NULL) {
		NotifyError(wxT("%s: Customer EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (supplier == customer) {
		NotifyError(wxT("%s: Attempted to disconnect EarObject from itself!"),
		  funcName);
		return(FALSE);
	}
	if (!DelInSignal_EarObject(customer, supplier->outSignal)) {
		NotifyError(wxT("%s: Could not delete input signal for customer "
		  "(%lu)."), funcName, customer->handle);
		return(FALSE);
	}
	FreeEarObjRef_EarObject(&supplier->customerList, customer->handle);
	FreeEarObjRef_EarObject(&customer->supplierList, supplier->handle);
	supplier->updateCustomersFlag = FALSE;
	return(TRUE);
	
}

/**************************** CreateEarObjRef *********************************/

/*
 * This function returns a pointer to an earObjRef node.
 * It returns NULL if it fails.
 * The input signal is always the last signal created.
 */
 
EarObjRefPtr
CreateEarObjRef_EarObject(EarObjectPtr theObject)
{
	static const WChar *funcName = wxT("CreateEarObjRef_EarObject");
	EarObjRef	*newNode;

	if ((newNode = (EarObjRef *) malloc(sizeof (EarObjRef))) == NULL) {
		NotifyError(wxT("%s: Out of memory."), funcName);
		return(NULL);
	}
	newNode->earObject = theObject;
	newNode->inSignalRef = theObject->numInSignals - 1;
	newNode->next = NULL;
	return(newNode);

}

/**************************** AddEarObjRef ************************************/

/*
 * This routine adds an EarObject reference to an EarObject's reference list.
 * It places the earObject handles in ascending order.
 * This function assumes that the supplier and EarObjects have been
 * initialised.
 */

BOOLN
AddEarObjRef_EarObject(EarObjRefPtr *theList, EarObjectPtr theObject)
{
	static const WChar *funcName = wxT("AddEarObjRef_EarObject");
	EarObjRefPtr	temp, p, last;

    if (*theList == NULL) {
    	if ((*theList = CreateEarObjRef_EarObject(theObject)) ==
    	  NULL) {
    		NotifyError(wxT("%s: Could not create new node."), funcName);
    		return(FALSE);
    	}
    	
		return(TRUE);
	}
	for (p = *theList, last = NULL; (p->next != NULL) && (p->earObject->handle <
	  theObject->handle); p = p->next)
		last = p;
	if (p->earObject->handle == theObject->handle) {
		/* NotifyWarning(wxT("AddEarObjRef_EarObject: EarObject already in "
		    "list."));
		 */
		return(TRUE);
	}
	if ((temp = CreateEarObjRef_EarObject(theObject)) == NULL) {
		NotifyError(wxT("%s: Could not create temp node."), funcName);
		return(FALSE);
	}
	if (p->earObject->handle > theObject->handle) {
		temp->next = p;
		if (p == *theList)	/* Test for head of list. */
			*theList = temp;	
		else
			last->next = temp;
	} else
		p->next = temp;
	return(TRUE);

}

/**************************** FreeEarObjRef ***********************************/

/*
 * This routine removes an EarObjRef from a list.
 * It returns the inSignalRef for further use.
 */

int
FreeEarObjRef_EarObject(EarObjRefPtr *theList, EarObjHandle theHandle)
{
	int		inSignalRef = -1;
	EarObjRefPtr	p, lastRef;

    if (*theList == NULL)
    	return (-1);
	for (p = lastRef = *theList; (p != NULL) && (p->earObject->handle !=
	  theHandle); p = p->next)
		lastRef = p;
	if (p->earObject->handle == theHandle) {
		if (p == *theList) /* Test for head of list. */
			*theList = p->next;
		else
			lastRef->next = p->next;
		inSignalRef = p->inSignalRef;
		free(p);
	}
	return(inSignalRef);

}

/**************************** FreeEarObjRefList *******************************/

/*
 * This routine frees the space allocated for a EarObjRef linked list.
 */
 
void
FreeEarObjRefList_EarObject(EarObjRefPtr *theList)
{
	EarObjRefPtr	temp;
	
	while (*theList != NULL) {
		temp = *theList;			/* Point to top earObjRef node */
		*theList = (*theList)->next;	/* Point earObjRefList to next node*/
		free(temp);
	}

}

/**************************** UpdateCustomers *********************************/

/*
 * This routine updates all of an EarObject's customers, i.e. all customers
 * pointing to its output signal are reset, but only if they are not already
 * pointing to the correct signal.
 */

void
UpdateCustomers_EarObject(EarObjectPtr theObject)
{
	EarObjRefPtr	p;
	
	for (p = theObject->customerList; p != NULL; p = p->next)
		if (p->earObject->inSignal[p->inSignalRef] != theObject->outSignal) {
			p->earObject->inSignal[p->inSignalRef] = theObject->outSignal;
			p->earObject->updateProcessFlag = TRUE;
		}
	theObject->updateCustomersFlag = FALSE;
	
}

/**************************** RemoveEarObjRefs ********************************/

/*
 * This routine updates all references to an EarObject, i.e. this routine is
 * only used when deleting an EarObject, when all customers must delete it
 * from their supplier lists, and all suppliers must delete it from their
 * customer lists.
 */

void
RemoveEarObjRefs_EarObject(EarObjectPtr theObject)
{
	EarObjRefPtr	p;
	
	for (p = theObject->supplierList; p != NULL; p = p->next) {
		FreeEarObjRef_EarObject(&p->earObject->customerList, theObject->handle);
	}
	for (p = theObject->customerList; p != NULL; p = p->next) {
		FreeEarObjRef_EarObject(&p->earObject->supplierList, theObject->handle);
	}
	theObject->updateCustomersFlag = FALSE;
	
}

/**************************** SetTimeContinuity *******************************/

/*
 * This routine sets the time continuity if the segmented mode is on.
 * The time is only updated after the first signal section has been processed.
 * It expects the EarObject to be correctly initialised. 
 */

void
SetTimeContinuity_EarObject(EarObjectPtr data)
{
	if (!GetDSAMPtr_Common()->segmentedMode)
		return;
	if (data->timeIndex > 1)
		data->firstSectionFlag = FALSE;
	data->timeIndex += _OutSig_EarObject(data)->length;

}

/**************************** SetProcessContinuity ****************************/

/*
 * This routine sets the various process continuity requirements, such as
 * time, customer updating, etc.
 * It assumes that the EarObjectPtr has been initialised.
 * The '_OutSig_EarObject(data)->offset' comparison ensures that only the first thread
 * of a multi-threaded run will set the process continuity information.
 */

void
SetProcessContinuity_EarObject(EarObjectPtr data)
{
	if (_OutSig_EarObject(data)->offset)
		return;
	SetTimeContinuity_EarObject(data);
	if (data->updateCustomersFlag)
		UpdateCustomers_EarObject(data);
	data->updateProcessFlag = FALSE;

}

/**************************** SetUtilityProcessContinuity *********************/

/*
 * This routine sets the various process continuity requirements for utility 
 * modules, such as staticTimeFlag, outputTimeOffset etc.
 * It assumes that the EarObjectPtr has been initialised.
 * The '_OutSig_EarObject(data)->offset' comparison ensures that only the first thread
 * of a multi-threaded run will set the process continuity information.
 */

void
SetUtilityProcessContinuity_EarObject(EarObjectPtr data)
{
	if (_OutSig_EarObject(data)->offset)
		return;
	if (data->inSignal[0]) {
		_OutSig_EarObject(data)->staticTimeFlag = data->inSignal[0]->staticTimeFlag;
		_OutSig_EarObject(data)->numWindowFrames = data->inSignal[0]->numWindowFrames;
		_OutSig_EarObject(data)->outputTimeOffset = data->inSignal[0]->outputTimeOffset;
	}

}

/**************************** SetUpdateProcessFlag ****************************/

/*
 * This routine sets the 'updateProcessFlag' for a process and all its thread
 * processes.
 * It assumes that the process has been correctly initialised.
 */

void
SetUpdateProcessFlag_EarObject(EarObjectPtr theObject, BOOLN setting)
{
	int		i;
	EarObjectPtr	p;

	theObject->updateProcessFlag = setting;
	for (i = 0, p = theObject->threadProcs; p && (i < theObject->numThreads -
	  1); i++, p++)
		p->updateProcessFlag = setting;
	for (i = 0; i < theObject->numSubProcesses; i++)
		theObject->subProcessList[i]->updateProcessFlag = setting;

}

/**************************** SetProcessForReset ******************************/

/*
 * This routine sets an EarObject's process for reset by setting the
 * updateProcessFlag and associated fields to TRUE.  This routine is used, so
 * that if later I introduce a different flag for reseting EarObject signals to
 * zero, to much of a code change will not be nessary.
 */

void
SetProcessForReset_EarObject(EarObjectPtr theObject)
{
	static const WChar *funcName = wxT("SetProcessForReset_EarObject");

	if (!theObject) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		exit(1);
	}
	theObject->updateProcessFlag = TRUE;
	theObject->timeIndex = PROCESS_START_TIME;
	theObject->firstSectionFlag = TRUE;

}

/************************** SetResetProcess ***********************************/

/*
 * This routine sets the global 'RunProcess' function pointer.
 */

void
SetResetProcess_EarObject(BOOLN (* Func)(EarObjectPtr))
{
	ResetProcess_EarObject = Func;

}


/**************************** ResetProcessStandard ****************************/

/*
 * This routine resets an EarObject's process and its thread subprocesses.
 */

BOOLN
ResetProcessStandard_EarObject(EarObjectPtr theObject)
{
	static const WChar *funcName = wxT("ResetProcessStandard_EarObject");
	int		i;
	EarObjectPtr	p;

	if (!theObject) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	SetProcessForReset_EarObject(theObject);
	for (i = 0, p = theObject->threadProcs; i < theObject->numThreads - 1; i++,
	  p++)
		SetProcessForReset_EarObject(p);
	return(TRUE);

}

/**************************** GetSample ***************************************/

/*
 * This routine returns a sample from an EarObject output signal channel.
 * It will produce a fatal error if an error occurs, i.e. the program will exit.
 */
 
double
GetSample_EarObject(EarObjectPtr data, uShort channel, ChanLen sample)
{
	static const WChar	*funcName = wxT("GetSample_EarObject");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		exit(1);
	}
	if (!CheckInit_SignalData(_OutSig_EarObject(data), funcName))
		exit(1);
	if (channel >= _OutSig_EarObject(data)->numChannels) {
		NotifyError(wxT("%s: Illegal channel number (%u) for '%s' EarObject."),
		  funcName, channel, POSSIBLY_NULL_STRING_PTR(data->processName));
		exit(1);
	}
	if (sample >= _OutSig_EarObject(data)->length) {
		NotifyError(wxT("%s: Illegal sample number (%u) for '%s' EarObject."),
		  funcName, sample, POSSIBLY_NULL_STRING_PTR(data->processName));
		exit(1);
	}
	return(_OutSig_EarObject(data)->channel[channel][sample]);

}

/**************************** GetResult ***************************************/

/*
 * This routine returns sample 0 from an EarObject output signal channel.
 * It is expected that it will be mostly used to return results from the
 * analysis modules.
 * It will produce a fatal error if an error occurs, i.e. the program will exit.
 */
 
double
GetResult_EarObject(EarObjectPtr data, uShort channel)
{
	static const WChar	*funcName = wxT("GetResult_EarObject");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		exit(1);
	}
	if (!CheckInit_SignalData(_OutSig_EarObject(data), funcName))
		exit(1);
	if (channel >= _OutSig_EarObject(data)->numChannels) {
		NotifyError(wxT("%s: Illegal channel number (%u) for '%s' EarObject."),
		  funcName, channel, POSSIBLY_NULL_STRING_PTR(data->processName));
		exit(1);
	}
	if (_OutSig_EarObject(data)->length < 1) {
		NotifyError(wxT("%s: Illegal signal length."), funcName);
		exit(1);
	}
	return(_OutSig_EarObject(data)->channel[channel][0]);

}

/**************************** CheckInSignal ***********************************/

/*
 * This routine checks for the presence of input signals, and whether they are
 * correctly initialised.
 * It assumes that the EarObject process is correctly initialised.
 * It returns FALSE if it fails in any way.
 */
 
BOOLN
CheckInSignal_EarObject(EarObjectPtr data, const WChar *callingFuncName)
{
	static const WChar	*funcName = wxT("CheckInSignal_EarObject");
	int		i;

	if (data->numInSignals < 1) {
		NotifyError(wxT("%s: No connected input signals for process (called "
		  "from '%s')."), funcName, callingFuncName);
		return(FALSE);
	}
	for (i = 0; i < data->numInSignals; i++)
		if (!CheckInit_SignalData(data->inSignal[i], funcName)) {
			NotifyError(wxT("%s: Signal %d not initialised."), funcName, i);
			return(FALSE);
		}
	return(TRUE);

}

/**************************** TempInputConnection *****************************/

/*
 * This routine makes manual/temporary input connections between the input
 * signal of a base EarObject process and another supporting EarObject process.
 * It assumes that both EarObject processes are correctly initialised.
 * It returns FALSE if it fails in any way.
 */
 
BOOLN
TempInputConnection_EarObject(EarObjectPtr base, EarObjectPtr supporting,
  int numInSignals)
{
	static const WChar	*funcName = wxT("TempInputConnection_EarObject");
	int		i;

	if (numInSignals > base->numInSignals) {
		NotifyError(wxT("%s: Base process has only %d input signals (%d)."),
		  funcName, base->numInSignals);
		return(FALSE);
	}
	if (supporting->numInSignals != numInSignals) {
		if (supporting->inSignal) {
			free(supporting->inSignal);
			supporting->inSignal = NULL;
			supporting->numInSignals = 0;
		}
		for (i = 0; i < numInSignals; i++)
			AddInSignal_EarObject(supporting);
	}
		
	for (i = 0; i < supporting->numInSignals; i++)
		supporting->inSignal[i] = base->inSignal[i];
	return(TRUE);

}

/**************************** SetRandPars *************************************/

/*
 * This function initialises the random parameters structure.
 * It sets the random seed if the structure already exists, otherwise it
 * initialises it.
 * It returns FALSE if it fails in any way.
 */

BOOLN
SetRandPars_EarObject(EarObjectPtr p, long ranSeed, const WChar *callingFunc)
{
	
	if (p->randPars) {
		SetSeed_Random(p->randPars, ranSeed, (long) p->threadIndex);
		return(TRUE);
	}
	if ((p->randPars = InitPars_Random(ranSeed, (long) p->threadIndex)) ==
	  NULL) {
		NotifyError(wxT("%s: Out of memory for 'random' parameters."),
		  callingFunc);
		return(FALSE);
	}
	return(TRUE);

}

/**************************** FreeThreadSubProcs ******************************/

/*
 * This routine frees the memory for the EarObject Copies for the sub-processes.
 * The out signal created as only a copy for the output signal, so it can
 * be simply free'd.  In addition, the space for the all the out signals was
 * created in one go, so the first out signal is used to delete all of them.
 * The same applies to the process EarObjects.
 */
 
void
FreeThreadSubProcs_EarObject(EarObjectPtr p)
{
	int		i;
	EarObjectPtr	*sP;

	if (!p->subProcessList)
		return;
	for (i = 0, sP = p->subProcessList; i < p->numSubProcesses; i++, sP++)
		if ((*sP)->randPars)
			FreePars_Random(&(*sP)->randPars);
 	if (p->subProcessList[0]->outSignal)
		free(p->subProcessList[0]->outSignal);
	free(p->subProcessList[0]);
	FreeSubProcessList_EarObject(p);

}

/**************************** InitThreadSubProcs ******************************/

/*
 * This function initialises the thread sub-process list.
 * It returns FALSE if it fails in any way.
 */

BOOLN
InitThreadSubProcs_EarObject(EarObjectPtr p, EarObjectPtr baseP)
{
	static const WChar *funcName = wxT("InitThreadSubProcs_EarObject");
	int		i;
	EarObjectPtr	*sP, *oSP, earObjectList;
	SignalDataPtr	signalList;

#	if DEBUG
	wprintf(wxT("%s: Debug: process '%s' * %d\n"), funcName, baseP->processName,
	  baseP->numSubProcesses);
#	endif
	if (!InitSubProcessList_EarObject(p, baseP->numSubProcesses)) {
		NotifyError(wxT("%s: Could not initialise subProcesses."), funcName);
		return(FALSE);
	}
	if (*p->subProcessList)
		return(TRUE);
	if ((earObjectList = (EarObject *) calloc(p->numSubProcesses, sizeof(
	  EarObject))) == NULL) {
		NotifyError(wxT("%s: Could not initialise sub-process EarObject Copy "
		  "list (%d copies)."), funcName, p->numSubProcesses);
		return(FALSE);
	}
	if ((signalList = (SignalData *) calloc(p->numSubProcesses, sizeof(
	  SignalData))) == NULL) {
		NotifyError(wxT("%s: Out of memory for %d signal copies."), funcName,
		  p->numSubProcesses);
		return(FALSE);
	}
	for (i = 0, sP = p->subProcessList, oSP = baseP->subProcessList; i < p->
	  numSubProcesses; i++, sP++, oSP++) {
		*sP = earObjectList++;
		*(*sP) = *(*oSP);
		(*sP)->outSignal = signalList++;
		(*sP)->randPars = NULL;
		(*sP)->threadIndex = baseP->threadIndex;
		*(*sP)->outSignal = *(*oSP)->outSignal;
		InitThreadRandPars_EarObject((*sP), baseP);
	}
	return(TRUE);

}

/**************************** FreeThreadProcs *********************************/

/*
 * This routine frees the memory for the EarObjectCopies.
 * The out signal created as only a copy for the output signal, so it can
 * be simply free'd.  In addition, the space for the all the out signals was
 * created in one go, so the first out signal is used to delete all of them.
 * The same applies to the process EarObjects.
 */
 
void
FreeThreadProcs_EarObject(EarObjectPtr p)
{
	int		i;
	EarObjectPtr	pI;

	if (!p->threadProcs)
		return;
#	if DEBUG
	printf(wxT("FreeThreadProcs_EarObject: Debug: process '%s' * %d\n"),
	  p->processName, p->numThreads - 1);
#	endif
	for (i = 0, pI = p->threadProcs; i < p->numThreads - 1; i++, pI++) {
		if (pI->randPars)
			FreePars_Random(&pI->randPars);
		if (pI->subProcessList)
			FreeThreadSubProcs_EarObject(pI);
	}
 	if (p->threadProcs[0].outSignal)
		free(p->threadProcs[0].outSignal);
	free(p->threadProcs);
	p->threadProcs = NULL;

}

/**************************** InitThreadRandPars ******************************/

/*
 * This routine initialised the random number structure for a sub process.
 */

void
InitThreadRandPars_EarObject(EarObjectPtr p, EarObjectPtr baseP)
{
	static const WChar *funcName = wxT("InitThreadRandPars_EarObject");

	if (!baseP->randPars)
		return;
	SetRandPars_EarObject(p, baseP->randPars->idum, funcName);
	return;
	
}

/**************************** InitThreadProcs *********************************/

/*
 * This function initialises the threadProcs list.
 * It returns FALSE if it fails in any way.
 * The first thread uses the original process data, randPars, outSignal, etc.
 */

BOOLN
InitThreadProcs_EarObject(EarObjectPtr p)
{
	static const WChar *funcName = wxT("InitThreadProcs_EarObject");
	int		i, numCopies;
	EarObjectPtr	tP;
	SignalDataPtr	s;

#	if DEBUG
	printf(wxT("%s: numThreads = %d.\n"), funcName, p->numThreads);
#	endif
	SetThreadRunFlag_EarObject(p, TRUE);
	if (p->threadProcs)
		return(TRUE);
	numCopies = p->numThreads - 1;
	if ((p->threadProcs = (EarObject *) calloc(numCopies, sizeof(
	  EarObject))) == NULL) {
		NotifyError(wxT("%s: Could not initialise EarObject Copy list (%d "
		  "copies)."), funcName, numCopies);
		return(FALSE);
	}
	if ((s = (SignalData *) calloc(numCopies, sizeof(SignalData))) ==
	  NULL) {
		NotifyError(wxT("%s: Out of memory for %d signal copies."), funcName,
		  numCopies);
		return(FALSE);
	}
#	ifdef DEBUG
	printf(wxT("%s: Debug: Main outSignal = %x.\n"), funcName, p->outSignal);
#	endif
	for (i = 0, tP = p->threadProcs; i < numCopies; i++, tP++, s++) {
		*tP = *p;
		tP->threadProcs = NULL;
		tP->outSignal = s;
		tP->threadIndex = i + 1;
#		ifdef DEBUG
		printf(wxT("%s: Debug: Thread outSignal = %x (%x).\n"), funcName, tP->
		  outSignal, p->outSignal);
#		endif
		*tP->outSignal = *p->outSignal;
		tP->randPars = NULL;
		tP->subProcessList = NULL;
		InitThreadRandPars_EarObject(tP, p);
		if (p->subProcessList && !InitThreadSubProcs_EarObject(tP, p)) {
			NotifyError(wxT("%s: Could not initialise sub process copies."),
			  funcName);
			return(FALSE);
		}

	}
	return(TRUE);

}

/**************************** SetThreadRunFlag ********************************/

/*
 * This routine sets the 'threadRunFlag' for a process and all its thread
 * processes.
 * It assumes that the process has been correctly initialised.
 */

void
SetThreadRunFlag_EarObject(EarObjectPtr theObject, BOOLN setting)
{
	int		i;

	theObject->threadRunFlag = setting;
	for (i = 0; i < theObject->numSubProcesses; i++)
		theObject->subProcessList[i]->threadRunFlag = setting;

}

/************************** FreeSubProcessList ********************************/

/*
 * This routine frees the memory allocated for a sub process list.
 * It assumes that the module has been correctly initialised.
 */

void
FreeSubProcessList_EarObject(EarObjectPtr p)
{
	if (!p->subProcessList)
		return;
	free(p->subProcessList);
	p->subProcessList = NULL;
	p->numSubProcesses = 0;

}

/************************** InitSubProcessList ********************************/

/*
 * Initialise a list of references to sub process Lists.
 * This list provides a way of initialising the sub processes for threaded 
 * operation.
 * It assumes that the module has been correctly initialised.
 */

BOOLN
InitSubProcessList_EarObject(EarObjectPtr p, int numSubProcesses)
{
	static const WChar	*funcName = wxT("InitSubProcessList_EarObject");

	if (p->subProcessList) {
		if (numSubProcesses == p->numSubProcesses)
			return(TRUE);
		FreeSubProcessList_EarObject(p);
	}
	if ((p->subProcessList = (EarObjectPtr *) calloc(numSubProcesses, sizeof(
	  EarObjectPtr))) == NULL) {
	  	NotifyError(wxT("%s: Out of memory for %d element sub-process list."),
		  funcName, numSubProcesses);
		return(FALSE);
	}
	p->numSubProcesses = numSubProcesses;
	return(TRUE);

}
