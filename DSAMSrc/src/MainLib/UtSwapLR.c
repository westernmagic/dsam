/**********************
 *
 * File:		UtSwapLR.c
 * Purpose:		This utility module swaps the left and right channels of
 *				binarual signals.
 * Comments:	Written using ModuleProducer version 1.3.3 (Oct  1 2001).
 * Author:		L. P. O'Mard
 * Created:		25 Jan 2002
 * Updated:	
 * Copyright:	(c) 2002, CNBH, University of Essex.
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
#include "UtSwapLR.h"

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
SetParsPointer_Utility_SwapLR(ModulePtr theModule)
{
	/* static const char	*funcName = "SetParsPointer_Utility_SwapLR"; */

	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 * It also initialises the process structure.
 */

BOOLN
InitModule_Utility_SwapLR(ModulePtr theModule)
{
	/* static const char	*funcName = "InitModule_Utility_SwapLR"; */

	SetDefault_ModuleMgr(theModule, TrueFunction_ModuleMgr);
	theModule->RunProcess = Process_Utility_SwapLR;
	theModule->SetParsPointer = SetParsPointer_Utility_SwapLR;
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
CheckData_Utility_SwapLR(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_Utility_SwapLR";

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInit_SignalData(data->inSignal[0], funcName))
		return(FALSE);
	if (data->inSignal[0]->interleaveLevel != 2) {
		NotifyError("%s: This utility only works with binaural signals.",
		  funcName);
		return(FALSE);
	} 
	/*** Put additional checks here. ***/
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
Process_Utility_SwapLR(EarObjectPtr data)
{
	static const char	*funcName = "Process_Utility_SwapLR";
	register ChanData	 *inPtr, *outPtr;
	int		chan;
	ChanLen	i;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckData_Utility_SwapLR(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "Swap L->R Utility Module process");

	/*** Example Initialise output signal - ammend/change if required. ***/
	if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels,
	  data->inSignal[0]->length, data->inSignal[0]->dt)) {
		NotifyError("%s: Cannot initialise output channels.", funcName);
		return(FALSE);
	}
	SetInterleaveLevel_SignalData(data->outSignal, data->inSignal[0]->
	  interleaveLevel);
	for (chan = 0; chan < data->inSignal[0]->numChannels - 1; chan += data->
	  inSignal[0]->interleaveLevel) {
		inPtr = data->inSignal[0]->channel[chan];
		outPtr = data->outSignal->channel[chan + 1];
		for (i = 0; i < data->outSignal->length; i++)
			*outPtr++ = *inPtr++;
		inPtr = data->inSignal[0]->channel[chan + 1];
		outPtr = data->outSignal->channel[chan];
		for (i = 0; i < data->outSignal->length; i++)
			*outPtr++ = *inPtr++;
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

