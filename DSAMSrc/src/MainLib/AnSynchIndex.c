/**********************
 *
 * File:		AnSynchIndex.c
 * Purpose:		Calculates the synchronisation index (vector strength) from
 *				the input signal.
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 *				The input is expected to come from a period histogram (PH).
 * Author:		L. P. O'Mard
 * Created:		1 Mar 1996
 * Updated:	
 * Copyright:	(c) 1997, University of Essex.
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
#include "AnSynchIndex.h"

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
SetParsPointer_Analysis_SynchIndex(ModulePtr theModule)
{
	/* static const WChar *funcName = "SetParsPointer_Analysis_SynchIndex"; */

	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Analysis_SynchIndex(ModulePtr theModule)
{
	SetDefault_ModuleMgr(theModule, TrueFunction_ModuleMgr);
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->RunProcess = Calc_Analysis_SynchIndex;
	theModule->SetParsPointer = SetParsPointer_Analysis_SynchIndex;
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
CheckData_Analysis_SynchIndex(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Analysis_SynchIndex");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
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
Calc_Analysis_SynchIndex(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("Calc_Analysis_SynchIndex");
	register	ChanData	 *r;
	int		chan;
	double	theta, sineSum, cosineSum, rSum;
	ChanLen	i;

	if (!data->threadRunFlag) {
		if (!CheckData_Analysis_SynchIndex(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Synchronisation Index Analysis"));
		if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels, 1,
		  1.0)) {
			NotifyError(wxT("%s: Cannot initialise output channels."),
			  funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	for (chan = _OutSig_EarObject(data)->offset; chan < _OutSig_EarObject(data)->numChannels;
	  chan++) {
		sineSum = cosineSum = rSum = 0.0;
		r = data->inSignal[0]->channel[chan];
		for (i = 0; i < data->inSignal[0]->length; i++) {
			theta = PIx2 * i / data->inSignal[0]->length; 
			sineSum += *r * sin(theta);
			cosineSum += *r * cos(theta);
			rSum += fabs(*r++);
		}
		_OutSig_EarObject(data)->channel[chan][0] = (rSum == 0.0)? 0.0: sqrt(sineSum *
		  sineSum + cosineSum * cosineSum) / rSum;
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

