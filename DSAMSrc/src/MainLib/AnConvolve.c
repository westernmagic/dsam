/**********************
 *
 * File:		AnConvolve.c
 * Purpose:		This routine creates convoluted signal from two EarObject's
 *				output signals.
 * Comments:	Written using ModuleProducer version 1.9 (May 27 1996).
 *				The output signal will be monaural or binural (with interleaved
 *				channels: LRLRLR) depending upon the format of the two input
 *				signals, which will both be the same.
 *				The convolution is defined as (R*S)[j] = sum(j = 0, m - 1)
 *				{ S[j-k]*R[k]}.
 * Author:		Lowel O'Mard and Enrique Lopez-Poveda.
 * Created:		27 May 1996
 * Updated:	
 * Copyright:	(c) University of Essex
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
#include "GeModuleMgr.h"
#include "AnConvolve.h"

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
SetParsPointer_Analysis_Convolution(ModulePtr theModule)
{
	/* static const char	*funcName = "SetParsPointer_Analysis_Convolution";
	*/
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Analysis_Convolution(ModulePtr theModule)
{
	/* static const char	*funcName = "InitModule_Analysis_Convolution"; */

	SetDefault_ModuleMgr(theModule, TrueFunction_ModuleMgr);
	theModule->RunProcess = Calc_Analysis_Convolution;
	theModule->SetParsPointer = SetParsPointer_Analysis_Convolution;
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
CheckData_Analysis_Convolution(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_Analysis_Convolution";

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (data->maxInSignals != 2) {
		NotifyError("%s: EarObject must be initialised using "\
		  "Init_EarObject_Multi(...) to accept 2 inputs.", funcName);
		return(FALSE);
	}
	if (!CheckPars_SignalData(data->inSignal[0]) ||
	  !CheckPars_SignalData(data->inSignal[1])) {
		NotifyError("%s: Input signals not correctly set.", funcName);		
		return(FALSE);
	}
	if ((fabs(data->inSignal[0]->dt - data->inSignal[1]->dt) > DBL_EPSILON) ||
	  (data->inSignal[0]->numChannels != data->inSignal[1]->numChannels)) {
		NotifyError("%s: Input signals are not the same.", funcName);		
		return(FALSE);
	}
	if (data->inSignal[0]->interleaveLevel !=
	  data->inSignal[1]->interleaveLevel) {
		NotifyError("%s: Input signals do not have the same interleave level.",
		  funcName);		
		return(FALSE);
	}
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
Calc_Analysis_Convolution(EarObjectPtr data)
{
	static const char	*funcName = "Calc_Analysis_Convolution";
	int		chan;
	register	ChanData	*inR, *inS, *outPtr, *endInR;
	ChanLen	i;

	if (!CheckData_Analysis_Convolution(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "Auto-convolution Process");
	if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels,
	  data->inSignal[0]->length, data->inSignal[0]->dt)) {
		NotifyError("%s: Cannot initialise output channels.", funcName);
		return(FALSE);
	}
	SetInterleaveLevel_SignalData(data->outSignal,
	  data->inSignal[0]->interleaveLevel);
	for (chan = 0; chan < data->outSignal->numChannels; chan++) {
		outPtr = data->outSignal->channel[chan];
		for (i = 0; i < data->outSignal->length; i++, outPtr++) {
			inR = data->inSignal[1]->channel[chan];
			endInR = inR + data->inSignal[1]->length;
			inS = data->inSignal[0]->channel[chan] + i;
			for (*outPtr = 0.0;(inS >= data->inSignal[0]->channel[chan]) &&
			  (inR <= endInR); inR++, inS--)
				*outPtr += *inR * *inS;
		}
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}