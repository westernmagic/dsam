/**********************
 *
 * File:		UtConvMonaural.c
 * Purpose:		This module converts a binaural signal to a monaural signal.
 * Comments:	Written using ModuleProducer version 1.10 (Jan  3 1997).
 * Author:		L. P. O'Mard
 * Convd:		5 Mar 1997
 * Updated:	
 * Copyright:	(c) 1998, University of Essex.
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
#include "UtConvMonaural.h"

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
SetParsPointer_Utility_ConvMonaural(ModulePtr theModule)
{
	/* static const char *funcName = "SetParsPointer_Utility_ConvMonaural"; */

	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Utility_ConvMonaural(ModulePtr theModule)
{
	/* static const char	*funcName = "InitModule_Utility_ConvMonaural"; */

	SetDefault_ModuleMgr(theModule, TrueFunction_ModuleMgr);
	theModule->RunProcess = Process_Utility_ConvMonaural;
	theModule->SetParsPointer = SetParsPointer_Utility_ConvMonaural;
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
CheckData_Utility_ConvMonaural(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_Utility_ConvMonaural";

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInit_SignalData(data->inSignal[0],
	  "CheckData_Utility_ConvMonaural"))
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
 * It can be called repeatedly with different parameter values if required.
 * Stimulus generation only sets the output signal, the input signal
 * is not used.
 * With repeated calls the Signal memory is only allocated once, then
 * re-used.
 */

BOOLN
Process_Utility_ConvMonaural(EarObjectPtr data)
{
	static const char	*funcName = "Process_Utility_ConvMonaural";
	register	ChanData	 *inPtr, *outPtr;
	int		i, outChan, inChan;
	ChanLen	j;

	if (!CheckData_Utility_ConvMonaural(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "Convert monaural -> binaural utility");
	ResetProcess_EarObject(data);
	if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels /
	  data->inSignal[0]->interleaveLevel, data->inSignal[0]->length,
	  data->inSignal[0]->dt)) {
		NotifyError("%s: Cannot initialise output channels.", funcName);
		return(FALSE);
	}
	SetInterleaveLevel_SignalData(data->outSignal, 1);
	for (outChan = 0; outChan < data->outSignal->numChannels; outChan++) {
		inChan = outChan * data->inSignal[0]->interleaveLevel;
		for (i = 0; i < data->inSignal[0]->interleaveLevel; i++) {
			inPtr = data->inSignal[0]->channel[inChan + i];
			outPtr = data->outSignal->channel[outChan];
			for (j = 0; j < data->inSignal[0]->length; j++)
				*(outPtr++) += *(inPtr++);
		}
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

