/******************
 *
 * File:		FiRaw.c
 * Purpose:		This Filing module deals with the writing reading of raw binary
 *				data files.
 * Comments:	14-04-98 LPO: This module has now been separated from the main
 *				FiDataFile module.
 *				17-04-98 LPO: Corrected unix pipe reading behaviour, required
 *				because pipes cannot use the "fseek" function used by
 *				SetPosition_UPortableIO(...).
 *				30-04-98 LPO: Used the _WorldTime_EarObject macro to correct
 *				the writing of the first segment in segmented mode, i.e. the
 *				file is created for writing not appending.
 *				Note that in segmented mode the normalisation calculation
 *				operates on the first segment only.
 * Authors:		L. P. O'Mard
 * Created:		14 Apr 1998
 * Updated:		30 Apr 1998
 * Copyright:	(c) 1998, University of Essex
 * 
 ******************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "UtUIEEEFloat.h"
#include "UtUPortableIO.h"
#include "UtString.h"
#include "FiParFile.h"
#include "FiDataFile.h"
#include "FiRaw.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Subroutines & functions ***********************/
/******************************************************************************/

/**************************** ReadFile ****************************************/

/*
 * This function reads a file in raw byte-stream format.
 * The data is stored in the output signal of an EarObject which will have
 * one channel.
 * This is the default file, and is assumed if no suffix is given
 * If a dash, '-.suffix', is given as the file name, then the data will be 
 * read from the standard input.
 * It returns TRUE if successful.
 * In segmented mode, this routine returns FALSE when it gets to the end of
 * the signal.
 * Using the "SetPosition_UPortableIO(...)" routine it is no longer necessary
 * for the "maxSamples" parameter to be set.
 */

BOOLN
ReadFile_Raw(char *fileName, EarObjectPtr data)
{
	static const char *funcName = "ReadFile_Raw";
	int		chan, endChan;
	ChanLen	i, length, index, requestedLength, numSamples;
	FILE	*fp;

	if (!CheckPars_DataFile()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	if (dataFilePtr->endian == 0)
		SetRWFormat_DataFile(DATA_FILE_BIG_ENDIAN);
	if ((fp = OpenFile_DataFile(fileName, FOR_BINARY_READING)) == NULL) {
		NotifyError("%s: Couldn't open file.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "'%s' byte (raw) file",
	  GetFileNameFPath_Utility_String(fileName));
	if (fp == stdin) {
		requestedLength = (int32) floor(dataFilePtr->duration *
		  dataFilePtr->defaultSampleRate + 1.5);
	} else {
		SetPosition_UPortableIO(fp, 0L, SEEK_END);
		requestedLength = GetPosition_UPortableIO(fp) / dataFilePtr->wordSize /
		  dataFilePtr->numChannels;
	}
	if (!InitProcessVariables_DataFile(data, requestedLength,
	  dataFilePtr->defaultSampleRate)) {
		NotifyError("%s: Could not initialise process variables.", funcName);
		return(FALSE);
	}
	if ((length = SetIOSectionLength_DataFile(data)) <= 0)
		return(FALSE);
	if (!InitOutSignal_EarObject(data, dataFilePtr->numChannels, length, 1.0 /
	  dataFilePtr->defaultSampleRate) ) {
		NotifyError("%s: Cannot initialise output signal", funcName);
		return(FALSE);
	}
	if (fp != stdin)
		SetPosition_UPortableIO(fp, (int32) ((dataFilePtr->timeOffsetIndex +
		  data->timeIndex) * dataFilePtr->numChannels * dataFilePtr->wordSize),
		  SEEK_SET);
	else
		while (dataFilePtr->timeOffsetCount) {
			ReadSample_DataFile(fp);
			dataFilePtr->timeOffsetCount--;
		}
	endChan = dataFilePtr->numChannels - 1;
	numSamples = dataFilePtr->numChannels * length;
	for (i = 0, index = 0; i < numSamples; i++) {
		chan = i % dataFilePtr->numChannels;
		*(data->outSignal->channel[chan] + index) = ReadSample_DataFile(fp);
		if (chan == endChan)
			index++;
	}
	CloseFile(fp);
	return(TRUE);

}

/**************************** WriteFile ***************************************/

/*
 * This function writes a signal to file in the raw word format.
 * It will average the signal across all channels.
 * It returns FALSE if it fails in any way.
 * If the fileName is a dash, '-.suffix', then the file will be written to the
 * standard output.
 * When checking the time the _WorldTime_EarObject macro must be used as this
 * module can be used on another EarObject or on its own, "in line".
 */

BOOLN
WriteFile_Raw(char *fileName, EarObjectPtr data)
{
	static const char *funcName = "WriteFile_Raw";
	int		chan, endChan, outputVal;
	register ChanLen	i, index, numSamples;
	FILE	*fp;
	
	if (dataFilePtr->endian == 0)
		SetRWFormat_DataFile(DATA_FILE_BIG_ENDIAN);
	fp = (GetDSAMPtr_Common()->segmentedMode && (_WorldTime_EarObject(data) !=
	  PROCESS_START_TIME))?
	  OpenFile_DataFile(fileName, FOR_BINARY_APPENDING):
	  OpenFile_DataFile(fileName, FOR_BINARY_WRITING);
	if (fp == NULL) {
		NotifyError("%s: Couldn't open file.", funcName);
		return(FALSE);
	}
	if (_WorldTime_EarObject(data) == PROCESS_START_TIME)
		dataFilePtr->normalise = CalculateNormalisation_DataFile(
		  data->outSignal);
	endChan = data->outSignal->numChannels - 1;
	numSamples = data->outSignal->numChannels * data->outSignal->length;
	for (i = 0, index = 0; i < numSamples; i++) {
		chan = i % data->outSignal->numChannels;
		outputVal = (int) (*(data->outSignal->channel[chan] + index) *
		  dataFilePtr->normalise);
		switch (dataFilePtr->wordSize) {
		case	1:
			Write8Bits(fp, outputVal);
			break;
		case	2:
			dataFilePtr->Write16Bits(fp, (int16) outputVal);
			break;
		case	4:
			dataFilePtr->Write32Bits(fp, (int32) outputVal);
			break;
		default:
			CloseFile(fp);
			NotifyError("%s: Unsupported sample size.", funcName);
			return(FALSE);
		} /* switch */
		if (chan == endChan)
			index++;
	}
	CloseFile(fp);
	return(TRUE);

}

