/**********************
 *
 * File:		UtStddise.c
 * Purpose:		This routine Standardises a signal: sum(Si) = 0,
 *			 	sum(Si * Si)/N = 1.
 * Comments:	Written using ModuleProducer version 1.9 (May 27 1996).
 *				Reference (LUT:542b)
 *				The method used uses the standard deviation and mean of the
 *				data i.e sd = sum{(x - mean)^2 / n). The standardised values,
 *				Si = (X - mean) / sd.
 * Author:		L. P. O'Mard
 * Created:		27 May 1996
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
#include "UtStddise.h"

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
SetParsPointer_Utility_Standardise(ModulePtr theModule)
{
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Utility_Standardise(ModulePtr theModule)
{
	/* static const char	*funcName = "InitModule_Utility_Standardise"; */

	SetDefault_ModuleMgr(theModule, TrueFunction_ModuleMgr);
	theModule->RunProcess = Process_Utility_Standardise;
	theModule->SetParsPointer = SetParsPointer_Utility_Standardise;
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
CheckData_Utility_Standardise(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_Utility_Standardise";

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
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
Process_Utility_Standardise(EarObjectPtr data)
{
	static const char	*funcName = "Process_Utility_Standardise";
	register	ChanData	 *inPtr, *outPtr;
	int		chan;
	double	mean, sumXX, sumX, standardDev;
	ChanLen	i;

	if (!CheckData_Utility_Standardise(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "Signal Standardisation Process");
	if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels,
	  data->inSignal[0]->length, data->inSignal[0]->dt)) {
		NotifyError("%s: Cannot initialise output channels.", funcName);
		return(FALSE);
	}	

	for (chan = 0; chan < data->inSignal[0]->numChannels; chan++) {
		inPtr = data->inSignal[0]->channel[chan];
		outPtr = data->outSignal->channel[chan];
		for (i = 0, sumX = sumXX = 0.0; i < data->inSignal[0]->length; i++,
		  inPtr++) {
			sumX += *inPtr;
			sumXX += SQR(*inPtr);
		}
		mean = sumX / data->inSignal[0]->length;
		standardDev = sqrt((sumXX - 2.0 * mean * sumX) /
		  data->inSignal[0]->length + mean * mean);
		inPtr = data->inSignal[0]->channel[chan];
		for (i = 0; i < data->inSignal[0]->length; i++)
			*outPtr++ = (*inPtr++ - mean) / standardDev;

	}
	SetUtilityProcessContinuity_EarObject(data);
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

