/**********************
 *
 * File:		MatInSignal.cpp
 * Purpose:		This routine contains the DSAM matlab/octave input signal class
 *				that initialises and converts the input signal.
 * Comments:	This was create for use by the scripting language interface
 *				RunDSAMSim.
 * Author:		L. P. O'Mard
 * Created:		05 Jul 2010
 * Updated:
 * Copyright:	(c) 2010, L. P. O'Mard
 *
 *********************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <DSAM.h>

#include "MatInSignal.h"

/******************************************************************************/
/*************************** Global Variables *********************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Methods (Subroutines) *************************/
/******************************************************************************/

/****************************** Constructor - Vector mode *********************/

/*
 *  Vector mode constructor.
 */

MatInSignal::MatInSignal(Float *inVector, int numChannels, int interleaveLevel,
		ChanLen length, Float dt, bool staticTimeFlag, Float outputTimeOffset)
{
	static const wxChar *funcName = wxT("MatInSignal::MatMainApp");

	vectorModeFlag = true;
	SetSignalPars(numChannels, interleaveLevel, length, dt, staticTimeFlag,
	  outputTimeOffset);
	this->inVector = inVector;

}

/****************************** Constructor - Pointer mode *********************/

/*
 *  Pointer mode constructor.
 */

MatInSignal::MatInSignal(Float **inPtrs, int numChannels, int interleaveLevel,
		ChanLen length, Float dt, bool staticTimeFlag, Float outputTimeOffset)
{
	static const wxChar *funcName = wxT("MatInSignal::MatMainApp");

	vectorModeFlag = false;
	SetSignalPars(numChannels, interleaveLevel, length, dt, staticTimeFlag,
	  outputTimeOffset);
	this->inPtrs = inPtrs;

}

/****************************** Destructor ************************************/

/*
 * Customer routines must ensure that the inputProcess is properly unconnected.
 */

MatInSignal::~MatInSignal(void)
{
	if (inputProcess)
		Free_EarObject(&inputProcess);

}

/****************************** SetSignalPars *********************************/

void
MatInSignal::SetSignalPars(int numChannels, int interleaveLevel, ChanLen length,
  Float dt, bool staticTimeFlag, Float outputTimeOffset)
{
	inputProcess = NULL;
	this->numChannels = numChannels;
	if (interleaveLevel < 0)
		this->interleaveLevel = (this->numChannels == 2)? 2:
		  SIGNALDATA_DEFAULT_INTERLEAVE_LEVEL;
	else
		this->interleaveLevel = interleaveLevel;
	this->dt = dt;
	this->staticTimeFlag = staticTimeFlag;
	this->outputTimeOffset = outputTimeOffset;
	this->length = length;

}

/****************************** InitInputEarObject ****************************/

/*
 * This function sets the Earobject inputSignal for providing input to a
 * simulation.
 * It uses the low level DSAM functions to create the EarObject, so that
 * unnecessary copies of the data need not be made.
 * This means that it cannot be free'd in the same way as a normal EarObject.
 */

bool
MatInSignal::InitInputEarObject(ChanLen segmentLength)
{
	static const wxChar	*funcName = wxT("MatInSignal::InitInputEarObject");

	if (dt <= 0.0) {
		NotifyError(wxT("%s: dt must be greater than zero."), funcName);
		return(false);
	}
	if (inputProcess)
		Free_EarObject(&inputProcess); 	/* Just in case */
	if ((inputProcess = Init_EarObject(wxT("NULL"))) == NULL) {
		NotifyError(wxT("%s: Could not initialise data results earObject."),
		  funcName);
		return(false);
	}
	if (!InitOutSignal_EarObject(inputProcess, numChannels, segmentLength,
	  dt)) {
		NotifyError(wxT("%s: Cannot initialise input process"), funcName);
		return(false);
	}
	SignalDataPtr	outSignal = _OutSig_EarObject(inputProcess);
	SetStaticTimeFlag_SignalData(outSignal, staticTimeFlag);
	SetLocalInfoFlag_SignalData(outSignal, TRUE);
	SetInterleaveLevel_SignalData(outSignal, interleaveLevel);
	outSignal->rampFlag = TRUE;
	SetInputProcessData(segmentLength);
	return(true);

}

/****************************** SetInputProcessData ***************************/

/*
 * This routine updates the process time and resets the output signal
 * channel pointers accordingly using the input data.
 * The outSignal->length is actually the segment length in segment mode.
 */

void
MatInSignal::SetInputProcessData(ChanLen signalLength)
{
	register double		*inPtr;
	register ChanData	*outPtr;
	int		chan;
	ChanLen	i;
	SignalDataPtr	outSignal = _OutSig_EarObject(inputProcess);

	for (chan = 0; chan < outSignal->numChannels; chan++) {
		outPtr = outSignal->channel[chan];
		if (vectorModeFlag) {
			inPtr = inVector + inputProcess->timeIndex - PROCESS_START_TIME + chan;
			for (i = 0; i < signalLength; i++, inPtr += outSignal->numChannels)
				*outPtr++ = *inPtr;
		} else {
			inPtr = inPtrs[chan] + inputProcess->timeIndex - PROCESS_START_TIME;
			for (i = 0; i < signalLength; i++)
				*outPtr++ = *inPtr++;
		}
	}
	SetTimeContinuity_EarObject(inputProcess);

}
