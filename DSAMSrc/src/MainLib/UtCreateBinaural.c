/**********************
 *
 * File:		UtCreateBinaural.c
 * Purpose:		This routine creates an interleaved binaural signal from two
 *				EarObject's output signals.
 * Comments:	Written using ModuleProducer version 1.9.
 *				The output signal channels will be interleaved: LRLRLR... with
 *				inSignal[0] and inSignal[1] as left (L) and right (R)
 *				respectively.  If the two signals are already binaural, then
 *				they will be merged into a single binaural signal.
 *				It is not worth thread enabling this utility.
 * Author:		L. P. O'Mard
 * Created:		Feb 14 1996
 * Updated:	
 * Copyright:	(c) 2005, CNBH, University of Essex.
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
#include "UtCreateBinaural.h"

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
SetParsPointer_Utility_CreateBinaural(ModulePtr theModule)
{
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Utility_CreateBinaural(ModulePtr theModule)
{
	SetDefault_ModuleMgr(theModule, TrueFunction_ModuleMgr);
	theModule->RunProcess = Process_Utility_CreateBinaural;
	theModule->SetParsPointer = SetParsPointer_Utility_CreateBinaural;
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
CheckData_Utility_CreateBinaural(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Utility_CreateBinaural");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (data->numInSignals != 2) {
		NotifyError(wxT("%s: The process EarObject must have 2 inputs from "
		  "other processes."), funcName);
		return(FALSE);
	}
	if (!CheckPars_SignalData(data->inSignal[0]) ||
	  !CheckPars_SignalData(data->inSignal[1])) {
		NotifyError(wxT("%s: Input signals not correctly set."), funcName);		
		return(FALSE);
	}
	if (!SameType_SignalData(data->inSignal[0], data->inSignal[1])) {
		NotifyError(wxT("%s: Input signals are not the same."), funcName);		
		return(FALSE);
	}
	
	if (data->inSignal[0]->interleaveLevel !=
	  data->inSignal[1]->interleaveLevel) {
		NotifyError(wxT("%s: Input signals do not have the same interleave "
		  "level."), funcName);		
		return(FALSE);
	}
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
Process_Utility_CreateBinaural(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("Process_Utility_CreateBinaural");
	register	ChanData	 *inPtr, *outPtr;
	uShort	numChannelsToSet;
	int		j, chan, outChanOffset, transferLevel;
	ChanLen	i;

	if (!data->threadRunFlag) {
		if (!CheckData_Utility_CreateBinaural(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Create/merge binaural signal "
		  "routine"));
		numChannelsToSet = (data->inSignal[0]->interleaveLevel == 2)?
		  data->inSignal[0]->numChannels: data->inSignal[0]->numChannels * 2;
		data->updateProcessFlag = TRUE;
		if (!InitOutSignal_EarObject(data, numChannelsToSet,
		  data->inSignal[0]->length, data->inSignal[0]->dt)) {
			NotifyError(wxT("%s: Cannot initialise output channels."),
			  funcName);
			return(FALSE);
		}
		SetInterleaveLevel_SignalData(_OutSig_EarObject(data), 2);
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	transferLevel = (data->inSignal[0]->interleaveLevel == 2)? 1: 2;
	for (j = 0; j < 2; j++) {
		outChanOffset = j * (transferLevel - 1);
		for (chan = 0; chan < _OutSig_EarObject(data)->numChannels; chan +=
		  transferLevel) {
			inPtr = data->inSignal[j]->channel[chan / transferLevel];
			outPtr = _OutSig_EarObject(data)->channel[chan + outChanOffset];
			for (i = 0; i < _OutSig_EarObject(data)->length; i++)
				*outPtr++ += *inPtr++;
		}
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

