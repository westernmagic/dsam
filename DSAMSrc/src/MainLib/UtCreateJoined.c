/**********************
 *
 * File:		UtCreateJoined.c
 * Purpose:		To create a signal by joining the output signals of multiply
 *				connected EarObjects.
 * Comments:	Written using ModuleProducer version 1.11 (Apr  9 1997).
 * Author:		L. P. O'Mard
 * Created:		26 Aug 1997
 * Updated:	
 * Copyright:	(c) 1998, University of Essex
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
#include "UtCreateJoined.h"

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
SetParsPointer_Utility_CreateJoined(ModulePtr theModule)
{
	/*static const char	*funcName = "SetParsPointer_Utility_CreateJoined"; */

	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Utility_CreateJoined(ModulePtr theModule)
{
	/* static const char	*funcName = "InitModule_Utility_CreateJoined"; */

	SetDefault_ModuleMgr(theModule, TrueFunction_ModuleMgr);
	theModule->RunProcess = Process_Utility_CreateJoined;
	theModule->SetParsPointer = SetParsPointer_Utility_CreateJoined;
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
CheckData_Utility_CreateJoined(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_Utility_CreateJoined";
	int		i;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	for (i = 0; i < data->numInSignals; i++) {
		if (!CheckInit_SignalData(data->inSignal[i],
		  "CheckData_Utility_CreateJoined")) {
			NotifyError("%s: Failed for signal %d.", funcName, i);
			return(FALSE);
		}
		if ((i != 0) && ((data->inSignal[0]->dt != data->inSignal[i]->dt) ||
		  (data->inSignal[0]->numChannels != data->inSignal[i]->numChannels))) {
			NotifyError("%s: All signals must of the same number of channels\n"
			  "and sampling Interval (signal %d is different).\n", funcName, i);
			return(FALSE);
		}
	}
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
Process_Utility_CreateJoined(EarObjectPtr data)
{
	static const char	*funcName = "Process_Utility_CreateJoined";
	register	ChanData	 *inPtr, *outPtr;
	int		i, chan;
	ChanLen	j, joinedLength;

	if (!CheckData_Utility_CreateJoined(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "Create Joined Utility module process");
	for (i = 0, joinedLength = 0; i < data->numInSignals; i++)
		joinedLength += data->inSignal[i]->length;
	if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels,
	  joinedLength, data->inSignal[0]->dt)) {
		NotifyError("%s: Cannot initialise output channels.", funcName);
		return(FALSE);
	}
	for (chan = 0; chan < data->outSignal->numChannels; chan++) {
		outPtr = data->outSignal->channel[chan];
		for (i = 0; i < data->numInSignals; i++) {
			inPtr = data->inSignal[i]->channel[chan];
			for (j = 0; j < data->inSignal[i]->length; j++)
				*outPtr++ = *inPtr++;
		}			
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

