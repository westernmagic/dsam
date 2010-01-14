/**********************
 *
 * File:		PrGeneral.h
 * Purpose:		This header file holds the general parallel functions etc.
 * Comments:	
 * Author:		
 * Created:		Nov 28 1995
 * Updated:	
 * Copyright:	(c) 1998, 2010 Lowel P. O'Mard
 *
 *********************
 *
 *  This file is part of DSAM.
 *
 *  DSAM is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  DSAM is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with DSAM.  If not, see <http://www.gnu.org/licenses/>.
 *
 *********************/

#include <stdio.h>
#include <stdlib.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#ifdef MPI_SUPPORT	/* To stop package dependency problems. */
#	include "mpi.h"
#endif
#include "PaGeneral.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Subroutines and functions *********************/
/******************************************************************************/

/****************************** ControlStr ************************************/

/*
 * This routine returns a pointer to a command string.
 */

WChar *
ControlStr_MPI_General(ControlSpecifier control)
{

	switch (control) {
	case INIT_SIMULATION:
		return(wxT("INIT_SIMULATION"));
	case RUN_SIMULATION:
		return(wxT("RUN_SIMULATION"));
	case REQUEST_SIGNAL_PARS:
		return(wxT("REQUEST_SIGNAL_PARS"));
	case REQUEST_CHANNEL_DATA:
		return(wxT("REQUEST_CHANNEL_DATA"));
	case PRINT_SIMULATION_PARS:
		return(wxT("PRINT_SIMULATION_PARS"));
	default:
		return(wxT("<unknown>"));
	} /* switch */

}

/****************************** InitWorkerDiagModeList ************************/

/*
 * This routine intialises the Worker Diagnostics Mode list array.
 */

NameSpecifier *
InitWorkerDiagModeList_MPI_General(void)
{
	static NameSpecifier	modeList[] = {

					{ wxT("OFF"), WORKER_DIAGNOSTICS_OFF },
					{ wxT("FILE"), WORKER_DIAGNOSTICS_FILE },
					{ wxT("SCREEN"), WORKER_DIAGNOSTICS_SCREEN },
					{ wxT(""), WORKER_DIAGNOSTICS_NULL }
				
				};
	return(modeList);

}

/****************************** InitWorkerChanModeList ************************/

/*
 * This routine intialises the Worker channel Mode list array.
 */

NameSpecifier *
InitWorkerChanModeList_MPI_General(void)
{
	static NameSpecifier	modeList[] = {

					{ wxT("NORMAL"),		WORKER_CHANNEL_NORMAL_MODE },
					{ wxT("SET_TO_BM"),	WORKER_CHANNEL_SET_TO_BM_MODE },
					{ wxT(""),			WORKER_CHANNEL_MODE_NULL }
				
				};
	return(modeList);

}

/****************************** SendSignalPars ********************************/

/*
 * This routine sends the signal parameters to the master, upon request.
 */

BOOLN
SendSignalPars_MPI_General(SignalDataPtr signal, int numChannels,
  int receiverRank, int tag)
{
	static const WChar	*funcName = wxT("SendSignalPars_MPI_General");
	int		  rampFlag, interleaveLevel;
	unsigned long	length, timeIndex;

	if (!CheckPars_SignalData(signal)) {
		NotifyError(wxT("%s: Sent to %d: Output signal not initialised."),
		  funcName, receiverRank);
		return(FALSE);
	}
	rampFlag = (int) signal->rampFlag;
	MPI_Send(&rampFlag, 1, MPI_INT, receiverRank, tag, MPI_COMM_WORLD);
	MPI_Send(&numChannels, 1, MPI_INT, receiverRank, tag, MPI_COMM_WORLD);
	interleaveLevel = (int) signal->interleaveLevel;
	MPI_Send(&interleaveLevel, 1, MPI_INT, receiverRank, tag, MPI_COMM_WORLD);
	length = (unsigned long) signal->length;
	MPI_Send(&length, 1, MPI_UNSIGNED_LONG, receiverRank, tag, MPI_COMM_WORLD);
	timeIndex = (unsigned long) signal->timeIndex;
	MPI_Send(&timeIndex, 1, MPI_UNSIGNED_LONG, receiverRank, tag,
	  MPI_COMM_WORLD);
	MPI_Send(&signal->dt, 1, MPI_DOUBLE, receiverRank, tag, MPI_COMM_WORLD);
	MPI_Send(&signal->outputTimeOffset, 1, MPI_DOUBLE, receiverRank, tag,
	  MPI_COMM_WORLD);
	return(TRUE);

}

/****************************** SendChannelData *******************************/

/*
 * This routine sends the channel data parameters to the receiver, upon request.
 * The data is sent in packets.
 * This routine first sends the signal length, in-case a change has accord
 * since the last transmission.  This allows the reciever to take the
 * appropriate action.
 */

BOOLN
SendChannelData_MPI_General(SignalDataPtr signal, int chanOffset,
   int numChannels, int receiverRank, int dataTag, int endChannelTag)
{
	static const WChar	*funcName = wxT("SendChannelData_MPI_General");
	int		i, ok;
	ChanData	*dataPtr, *chanEnd;
	MPI_Request	request;

	if (!CheckPars_SignalData(signal)) {
		NotifyError(wxT("%s: Sent to %d: Output signal not initialised."),
		  funcName, receiverRank);
		return(FALSE);
	}
	for (i = 0, ok = TRUE; i < numChannels; i++) {
		dataPtr = signal->channel[(i + chanOffset) % signal->numChannels];
		chanEnd = dataPtr + signal->length - 1;
		while (dataPtr + DATA_BLOCK_SIZE <= chanEnd) {
			MPI_Isend(dataPtr, DATA_BLOCK_SIZE, MPI_DOUBLE, receiverRank,
			  dataTag, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, NULL);
			dataPtr += DATA_BLOCK_SIZE;
		}
		if (dataPtr <= chanEnd) {
			MPI_Isend(dataPtr, chanEnd - dataPtr + 1, MPI_DOUBLE, receiverRank,
			  dataTag, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, NULL);
		}
		MPI_Send(&ok, 1, MPI_INT, receiverRank, endChannelTag,
		  MPI_COMM_WORLD);
	}
	return(TRUE);

}

/****************************** ReceiveSignalPars *****************************/

/*
 * This routine receives  the signal parameters for a signal to be received
 * later.
 * It returns a pointer to a static signal with all the appropriate values set
 * as required for a signal.
 * The "SetLength_SignalData(...)" routine is not used as it expects the signal
 * not to have been set before (i.e. it expects the signal to be created each
 * time).
 */

SignalDataPtr
ReceiveSignalPars_MPI_General(int sourceRank, int tag)
{
	static const WChar	*funcName = wxT("ReceiveSignalPars_MPI_General");
	static SignalData	signal;
	unsigned long	length, timeIndex;
	int		rampFlag, interleaveLevel, response, numChannels;
	double	dt, outputTimeOffset;
	MPI_Status		status;

	MPI_Recv(&rampFlag, 1, MPI_INT, sourceRank, tag, MPI_COMM_WORLD, &status);
	MPI_Recv(&numChannels, 1, MPI_INT, sourceRank, tag, MPI_COMM_WORLD,
	  &status);
	MPI_Recv(&interleaveLevel, 1, MPI_INT, sourceRank, tag, MPI_COMM_WORLD,
	  &status);
	MPI_Recv(&length, 1, MPI_UNSIGNED_LONG, sourceRank, tag, MPI_COMM_WORLD,
	  &status);
	MPI_Recv(&timeIndex, 1, MPI_UNSIGNED_LONG, sourceRank, tag, MPI_COMM_WORLD,
	  &status);
	MPI_Recv(&dt, 1, MPI_DOUBLE, sourceRank, tag, MPI_COMM_WORLD, &status);
	MPI_Recv(&outputTimeOffset, 1, MPI_DOUBLE, sourceRank, tag, MPI_COMM_WORLD,
	  &status);
	MPI_Recv(&response, 1, MPI_INT, sourceRank, tag, MPI_COMM_WORLD, &status);
	if (response == FALSE) {
		NotifyError(wxT("%s: Worker[%d] failed to return the signal ")
		  wxT("parameters."), funcName, sourceRank);
		return(FALSE);
	}
	signal.rampFlag = (BOOLN) rampFlag;
	signal.numChannels = numChannels;
	signal.length = (ChanLen) length;
	SetInterleaveLevel_SignalData(&signal, interleaveLevel);
	SetSamplingInterval_SignalData(&signal, dt);
	SetTimeIndex_SignalData(&signal, (ChanLen) timeIndex);
	SetOutputTimeOffset_SignalData(&signal, outputTimeOffset);
	return(&signal);

}

/****************************** SetOutSignal **********************************/

/*
 * This routine sets the signal parameters for the output signal.
 * It returns FALSE if the signal is not initialised, or if a failure response
 * is returned from the source.
 */

BOOLN
SetOutSignal_MPI_General(EarObjectPtr data, SignalDataPtr templateSignal)
{
	static const WChar	*funcName = wxT("SetOutSignal_MPI_General");

	if (templateSignal == NULL) {
		NotifyError(wxT("%s: null template signal is invalid."), funcName);
		return(FALSE);
	}
	if (!InitOutSignal_EarObject(data, templateSignal->numChannels,
	 templateSignal->length, templateSignal->dt) ) {
		NotifyError(wxT("%s: Cannot initialise output signal"), funcName);
		return(FALSE);
	}
	_OutSig_EarObject(data)->rampFlag = templateSignal->rampFlag;
	SetInterleaveLevel_SignalData(_OutSig_EarObject(data),
	  templateSignal->interleaveLevel);
	SetTimeIndex_SignalData(_OutSig_EarObject(data), templateSignal->timeIndex);
	SetOutputTimeOffset_SignalData(_OutSig_EarObject(data),
	  templateSignal->outputTimeOffset);
	return(TRUE);

}

