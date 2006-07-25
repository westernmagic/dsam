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

/**************************** InitialFileRead *********************************/

/*
 * This function returns a file pointer from the initial reading of a raw binary
 * file.
 * It is created because this initial reading process is required for reading
 * the entire file, or just extracting the size of the file.
 * It returns a pointer to the file if successful, or NULL if it fails.
 */

FILE *
InitialFileRead_Raw(WChar *fileName)
{
	static const WChar *funcName = wxT("InitialFileRead_Wave");
	FILE	*fp;

	if (dataFilePtr->endian == 0)
		SetRWFormat_DataFile(DATA_FILE_BIG_ENDIAN);
	if ((fp = OpenFile_DataFile(fileName, FOR_BINARY_READING)) == NULL) {
		NotifyError(wxT("%s: Couldn't open file."), funcName);
		return(FALSE);
	}
	return(fp);

}

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
ReadFile_Raw(WChar *fileName, EarObjectPtr data)
{
	static const WChar *funcName = wxT("ReadFile_Raw");
	int		chan, endChan;
	ChanLen	i, length, index, requestedLength, numSamples;
	FILE	*fp;

	if ((fp = InitialFileRead_Raw(fileName)) == NULL) {
		NotifyError(wxT("%s: Could not read initial file structure from '%s'."),
		  funcName, fileName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, wxT("'%s' byte (raw) file"),
	  GetFileNameFPath_Utility_String(fileName));
	if (fp == stdin) {
		requestedLength = (int32) floor(dataFilePtr->duration * dataFilePtr->
		  defaultSampleRate + 0.5);
	} else {
		SetPosition_UPortableIO(fp, 0L, SEEK_END);
		requestedLength = GetPosition_UPortableIO(fp) / dataFilePtr->wordSize /
		  dataFilePtr->numChannels;
	}
	if (!InitProcessVariables_DataFile(data, requestedLength,
	  dataFilePtr->defaultSampleRate)) {
		NotifyError(wxT("%s: Could not initialise process variables."),
		  funcName);
		return(FALSE);
	}
	if ((length = SetIOSectionLength_DataFile(data)) <= 0)
		return(FALSE);
	if (!InitOutSignal_EarObject(data, (uShort) dataFilePtr->numChannels,
	  length, 1.0 / dataFilePtr->defaultSampleRate) ) {
		NotifyError(wxT("%s: Cannot initialise output signal"), funcName);
		return(FALSE);
	}
	if (dataFilePtr->numChannels == 2)
		SetInterleaveLevel_SignalData(_OutSig_EarObject(data), 2);
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
		*(_OutSig_EarObject(data)->channel[chan] + index) = ReadSample_DataFile(fp);
		if (chan == endChan)
			index++;
	}
	CloseFile(fp);
	return(TRUE);

}

/**************************** GetDuration *************************************/

/*
 * This function extracts the duration of the file in a signal by reading the
 * minimum of information from the file.
 * It returns a negative value if it fails to read the file duration.
 */

double
GetDuration_Raw(WChar *fileName)
{
	static const WChar *funcName = wxT("GetDuration_Wave");
	double	duration;
	FILE	*fp;

	if ((fp = InitialFileRead_Raw(fileName)) == NULL) {
		NotifyError(wxT("%s: Could not do initial file operations for '%s'."),
		  funcName, fileName);
		CloseFile(fp);
		return(-1.0);
	}
	if (fp == stdin) {
		NotifyError(wxT("%s: Reading from stdin disables this function for "
		  "this format."), funcName);
		CloseFile(fp);
		return(-1.0);
	}
	duration = FileLength_DataFile(fp) / dataFilePtr->wordSize / dataFilePtr->
	  defaultSampleRate;
	CloseFile(fp);
	return(duration);

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
WriteFile_Raw(WChar *fileName, EarObjectPtr data)
{
	static const WChar *funcName = wxT("WriteFile_Raw");
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
		NotifyError(wxT("%s: Couldn't open file."), funcName);
		return(FALSE);
	}
	if (_WorldTime_EarObject(data) == PROCESS_START_TIME)
		dataFilePtr->normalise = CalculateNormalisation_DataFile(
		  _OutSig_EarObject(data));
	endChan = _OutSig_EarObject(data)->numChannels - 1;
	numSamples = _OutSig_EarObject(data)->numChannels * _OutSig_EarObject(data)->length;
	for (i = 0, index = 0; i < numSamples; i++) {
		chan = i % _OutSig_EarObject(data)->numChannels;
		outputVal = (int) (*(_OutSig_EarObject(data)->channel[chan] + index) *
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
			NotifyError(wxT("%s: Unsupported sample size."), funcName);
			return(FALSE);
		} /* switch */
		if (chan == endChan)
			index++;
	}
	CloseFile(fp);
	return(TRUE);

}

