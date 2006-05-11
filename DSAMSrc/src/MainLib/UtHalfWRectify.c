/**********************
 *
 * File:		UtHalfWRectify.c
 * Purpose:		This proces module half-wave rectifies a signal, setting
 *				all negative values to zero.
 * Comments:	Written using ModuleProducer version 1.2.6 (Jul 22 1999).
 * Author:		L. P. O'Mard
 * Created:		22 Jul 1999
 * Updated:	
 * Copyright:	(c) 1999, University of Essex.
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
#include "UtHalfWRectify.h"

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
SetParsPointer_Utility_HalfWaveRectify(ModulePtr theModule)
{
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Utility_HalfWaveRectify(ModulePtr theModule)
{
	SetDefault_ModuleMgr(theModule, TrueFunction_ModuleMgr);
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->RunProcess = Process_Utility_HalfWaveRectify;
	theModule->SetParsPointer = SetParsPointer_Utility_HalfWaveRectify;
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
CheckData_Utility_HalfWaveRectify(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Utility_HalfWaveRectify");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	/*** Put additional checks here. ***/
	return(TRUE);

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
Process_Utility_HalfWaveRectify(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("Process_Utility_HalfWaveRectify");
	register	ChanData	 *inPtr, *outPtr;
	int			chan;
	ChanLen	i;

	if (!data->threadRunFlag) {
		if (!CheckData_Utility_HalfWaveRectify(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Half-wave rectification process "));
		if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels,
		  data->inSignal[0]->length, data->inSignal[0]->dt)) {
			NotifyError(wxT("%s: Cannot initialise output channels."),
			  funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	for (chan = data->outSignal->offset; chan < data->outSignal->numChannels;
	  chan++) {
		inPtr = data->inSignal[0]->channel[chan];
		outPtr = data->outSignal->channel[chan];
		for (i = 0; i < data->outSignal->length; i++, inPtr++)
			*(outPtr++) = (*inPtr > 0.0)? *inPtr: 0.0;
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

