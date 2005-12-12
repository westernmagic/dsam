/**********************
 *
 * File:		UtAccumulate.c
 * Purpose:		This general utility accumulates its input signal into it
 *				output signal.
 * 				It adds to previous data if the output signal has already been
 * 				initialised and the data->updateProcessFlag is set to FALSE.
 * 				Otherwise it will overwrite the old signal or create a new
 *				signal as required.
 * 				The data->updateProcessFlag facility is useful for repeated
 *				runs.  It is set to FALSE before the routine returns.
 * Comments:	Written using ModuleProducer version 1.8.
 *				It does not have any parameters.
 *				06-09-96 LPO: Now can accumulate output from more than one
 *				EarObject.
 *				27-01-99 LPO: 
 * Author:		L. P. O'Mard
 * Created:		Dec 21 1995
 * Updated:		27 Jan 1998
 * Copyright:	(c) 1999, University of Essex.
 *
 *********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "UtAccumulate.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Subroutines and functions *********************/
/******************************************************************************/

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Utility_Accumulate(ModulePtr theModule)
{
	/* static const char	*funcName = "SetParsPointer_Utility_Accumulate"; */

	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Utility_Accumulate(ModulePtr theModule)
{
	/* static const char	*funcName = "InitModule_Utility_Accumulate"; */

	SetDefault_ModuleMgr(theModule, TrueFunction_ModuleMgr);
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->ResetProcess = ResetProcess_Utility_Accumulate;
	theModule->RunProcess = Process_Utility_Accumulate;
	theModule->SetParsPointer = SetParsPointer_Utility_Accumulate;
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
CheckData_Utility_Accumulate(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_Utility_Accumulate";
	BOOLN	ok = TRUE;
	int		i;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	for (i = 1; ok && (i < data->numInSignals); i++)
		if (!SameType_SignalData_NoDiagnostics(data->inSignal[0],
		  data->inSignal[i])) {
			NotifyError("%s: Input signal [%d] is not the same as the first "
			  "[0].", funcName, i);
			ok = FALSE;
		}
	if (!ok)
		return(FALSE);
	/*** Put additional checks here. ***/
	return(TRUE);

}

/**************************** ResetProcess ************************************/

/*
 * This routine resets the process variables.
 */

void
ResetProcess_Utility_Accumulate(EarObjectPtr data)
{
	ResetOutSignal_EarObject(data);

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
 */

BOOLN
Process_Utility_Accumulate(EarObjectPtr data)
{
	static const char	*funcName = "Process_Utility_Accumulate";
	register	ChanData	 *inPtr, *outPtr;
	int			chan, input;
	ChanLen	i;

	if (!data->threadRunFlag) {
		if (!CheckData_Utility_Accumulate(data)) {
			NotifyError("%s: Process data invalid.", funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, "Signal accumulator.");
		if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels,
		  data->inSignal[0]->length, data->inSignal[0]->dt)) {
			NotifyError("%s: Cannot initialise output channels.", funcName);
			return(FALSE);
		}
		ResetProcess_Utility_Accumulate(data);
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	for (input = 0; input < data->numInSignals; input++)
		for (chan = data->outSignal->offset; chan < data->outSignal->
		  numChannels; chan++) {
			inPtr = data->inSignal[input]->channel[chan];
			outPtr = data->outSignal->channel[chan];
			for (i = 0; i < data->outSignal->length; i++)
				*(outPtr++) += *(inPtr++);
		}
	SetUtilityProcessContinuity_EarObject(data);
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

