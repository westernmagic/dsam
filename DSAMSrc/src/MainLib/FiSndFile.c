/******************
 *
 * File:		FiSndFile.c
 * Purpose:		This Filing reads sound format files using the libsndfile
 * 				library.
 * Comments:	
 * Authors:		L. P. O'Mard
 * Created:		07 Nov 2006
 * Updated:		
 * Copyright:	(c) 2006, Lowel P. O'Mard
 * 
 ******************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sndfile.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "UtString.h"
#include "FiParFile.h"
#include "FiDataFile.h"
#include "FiSndFile.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Subroutines & functions ***********************/
/******************************************************************************/

/**************************** Free ********************************************/

/*
 * Free and close the sound file.
 */

void 
Free_SndFile(void)
{
	static const WChar *funcName = wxT("Free_SndFile");
	int error;
	DataFilePtr p = dataFilePtr;

	if (!p->sndFile)
		return;
	if ((error = sf_close(p->sndFile)) != 0)
		NotifyError(wxT("%s: Could not close file: %s"), funcName,
		  MBSToWCS_Utility_String(sf_error_number(error)));
	p->sndFile = NULL;

}

/**************************** GetWordSize *************************************/

/*
 * Returns the sound format word size.
 */

int
GetWordSize_SndFile(int format)
{
	static const WChar *funcName = wxT("GetWordSize_SndFile");
	int		subType = format & 0xFF;

	if (subType < 5)
		return(subType);
	if (subType & SF_FORMAT_PCM_U8)
		return(1);
	else if (subType & SF_FORMAT_FLOAT)
		return(4);
	else if (subType & SF_FORMAT_DOUBLE)
		return(8);
	
	NotifyError(wxT("Undefined format (%d)."), funcName, subType);
	return(-1);

}

/**************************** OpenFile ****************************************/

/*
 * This function opens the interaction with the file.
 * It assumes if the sndFile field is not NULL then a file has been correctly
 * opened.
 */

BOOLN
OpenFile_SndFile(WChar *fileName, int mode)
{
	static const WChar *funcName = wxT("OpenFile_SndFile");
	WChar	*parFilePath;
	DataFilePtr	p = dataFilePtr;

	if (p->sndFile)
		Free_SndFile();
	parFilePath = GetParsFileFPath_Common(fileName);
	if ((p->type == SF_FORMAT_RAW) || (mode == SFM_WRITE)) {
		p->sFInfo.samplerate = p->defaultSampleRate;
		p->sFInfo.channels = p->numChannels;
		p->sFInfo.format = p->type | p->subFormatType;
		if (!sf_format_check(&p->sFInfo)) {
			NotifyError(wxT("%s: Illegal output format for sound file."),
			  funcName);
			return(FALSE);
		}
		if (mode == SFM_WRITE) {
			switch (p->endian) {
			case DATA_FILE_LITTLE_ENDIAN:
				p->sFInfo.format |= SF_ENDIAN_LITTLE;
				break;
			case DATA_FILE_BIG_ENDIAN:
				p->sFInfo.format |= SF_ENDIAN_BIG;
				break;
			case DATA_FILE_CPU_ENDIAN:
				p->sFInfo.format |= SF_ENDIAN_CPU;
				break;
			default:
				;
			}
		}
	}
	if ((p->sndFile = sf_open(ConvUTF8_Utility_String(parFilePath), mode,
	  &p->sFInfo)) == NULL) {
		NotifyError(wxT("%s: Could not open file '%s'\n"), funcName, fileName);
		return(FALSE);
	}
	return(TRUE);

}

/**************************** ReadFrames **************************************/

/*
 * This routine reads the data frames using a buffer for speed.
 */
 
BOOLN
ReadFrames_SndFile(EarObjectPtr data, sf_count_t length)
{
	static const WChar *funcName = wxT("ReadFrames_SndFile");
	register ChanData	*inPtr, *outPtr;
	int		chan;
	sf_count_t	count = 0, frames, i, bufferFrames;
	DataFilePtr	p = dataFilePtr;
	SignalDataPtr	outSignal = _OutSig_EarObject(data);

	if (!InitBuffer_DataFile(funcName))
		return(FALSE);
	bufferFrames = DATAFILE_BUFFER_FRAMES;
	while (count < length) {
		if ((length - count) < bufferFrames)
			bufferFrames = length - count;
		frames = sf_readf_double(p->sndFile, p->buffer, bufferFrames);
		if (!frames)
			break;
		for (chan = 0; chan < outSignal->numChannels; chan++) {
			outPtr = outSignal->channel[chan] + count;
			inPtr = p->buffer + chan;
			for (i = 0, inPtr = p->buffer + chan; i < frames; i++, inPtr +=
			  outSignal->numChannels)
				*outPtr++ = *inPtr;
		}
		count += frames;
	}
	if (count == length)
		return(TRUE);
	return(FALSE);

}

/**************************** ReadFile ****************************************/
/*
 * This function reads a file in Microsoft Wave format.
 * The data is stored in the output signal of an EarObject which will have
 * one channel.
 * This is the default file, and is assumed if no suffix is given
 * If a dash, '-.suffix', is given as the file name, then the data will be 
 * read from the standard input.
 * I am not quite certain about the stereo format.  At present it will assume
 * that if there are two channels, then it is a stereo signal.
 * It returns TRUE if successful.
 */

BOOLN
ReadFile_SndFile(WChar *fileName, EarObjectPtr data)
{
	static const WChar *funcName = wxT("ReadFile_SndFile");
	BOOLN	ok = TRUE;
	const char *titleString;
	ChanLen	length;
	DataFilePtr	p = dataFilePtr;

	if (!OpenFile_SndFile(fileName, SFM_READ)) {
		NotifyError(wxT("%s: Could not open file '%s'\n"), funcName,
		  fileName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, wxT("Read '%s' Sound data file"),
	  GetFileNameFPath_Utility_String(fileName));
	titleString = sf_get_string(p->sndFile, SF_STR_TITLE);
	wprintf(wxT("titleString = '%s'\n"), titleString);
	if (!GetDSAMPtr_Common()->segmentedMode || (data->timeIndex ==
	  PROCESS_START_TIME)) {
	  	p->subFormatType = p->sFInfo.format & SF_FORMAT_SUBMASK;
		p->numChannels = p->sFInfo.channels;
		p->defaultSampleRate = p->sFInfo.samplerate;
		p->wordSize = GetWordSize_SndFile(p->sFInfo.format);
		p->numSamples = p->sFInfo.frames;
		if (!InitProcessVariables_DataFile(data, p->numSamples, p->sFInfo.
		  samplerate)) {
			NotifyError(wxT("%s: Could not initialise process variables."),
			  funcName);
			return(FALSE);
		}
	}
	if ((length = SetIOSectionLength_DataFile(data)) <= 0)
		return(FALSE);
	if (!InitOutSignal_EarObject(data, (uShort) p->numChannels,
	  length, 1.0 / p->defaultSampleRate)) {
		NotifyError(wxT("%s: Cannot initialise output signal"), funcName);
		return(FALSE);
	}
	if (p->numChannels == 2)
		SetInterleaveLevel_SignalData(_OutSig_EarObject(data), 2);
	sf_seek(p->sndFile, (int32) (data->timeIndex + p->timeOffsetCount),
	  SEEK_SET);
	ok = ReadFrames_SndFile(data, length);
	Free_SndFile();
	return(ok);

}

/**************************** GetDuration *************************************/

/*
 * This function extracts the duration of the file in a signal by reading the
 * minimum of information from the file.
 * It returns a negative value if it fails to read the file duration.
 */

double
GetDuration_SndFile(WChar *fileName)
{
	static const WChar *funcName = wxT("GetDuration_SndFile");

	if (OpenFile_SndFile(fileName, SFM_READ)) {
		NotifyError(wxT("%s: Could not read initial file structure from '%s'."),
		  funcName, fileName);
		return(-1.0);
	}
	return((double) dataFilePtr->sFInfo.frames / dataFilePtr->sFInfo.
	  samplerate);
	Free_SndFile();

}

/**************************** WriteFrames **************************************/

/*
 * This routine writes the data frames using a buffer for speed.
 */
 
BOOLN
WriteFrames_SndFile(SignalDataPtr inSignal)
{
	static const WChar *funcName = wxT("WriteFrames_SndFile");
	register ChanData	*inPtr, *outPtr;
	int		chan;
	sf_count_t	count = 0, frames, i, bufferFrames;
	DataFilePtr	p = dataFilePtr;

	if (!InitBuffer_DataFile(funcName))
		return(FALSE);
	bufferFrames = DATAFILE_BUFFER_FRAMES;
	while (count < inSignal->length) {
		if ((inSignal->length - count) < bufferFrames)
			bufferFrames = inSignal->length - count;
		for (chan = 0; chan < inSignal->numChannels; chan++) {
			inPtr = inSignal->channel[chan] + count;
			for (i = bufferFrames, outPtr = p->buffer + chan; i--; outPtr +=
			  inSignal->numChannels)
				*outPtr = *inPtr++ / p->normalise;
		}
		frames = sf_writef_double(p->sndFile, p->buffer, bufferFrames);
		if (frames != bufferFrames) {
			NotifyError(wxT("%s: Failed to write sound data."), funcName);
			return(FALSE);
		}
		count += frames;
	}
	if (count == inSignal->length)
		return(TRUE);
	return(FALSE);

}

/**************************** CalculateNormalisation **************************/

/*
 * This routine calculates the normalisation factor for a signal.
 * It expects the signal to be correctly initialised.
 * It returns zero if the maximum value is 0.0.
 * It returns the normalisation argument value, if it is greater than zero in
 * non-auto mode.
 */

double
CalculateNormalisation_SndFile(SignalDataPtr signal)
{
	int		chan;
	double 	maxValue;
	ChanLen	i;
	ChanData	*dataPtr;

	for (chan = signal->numChannels, maxValue = -DBL_MAX; chan-- ;) {
		dataPtr = signal->channel[chan];
		for (i = signal->length; i-- ; dataPtr++)
			if (fabs(*dataPtr) > maxValue)
				maxValue = fabs(*dataPtr);
	}
	if (maxValue < DBL_EPSILON)
		return(1.0);
	return(maxValue);

}

/**************************** CreateTitleString ********************************/

/*
 * This function creates and returns the title string.
 * The memory for this string must be "freed" by the calling program.
 */
 
char *
CreateTitleString_SndFile(EarObjectPtr data)
{
	static const WChar *funcName = wxT("CreateTitleString_SndFile");
	char *s;
	size_t	length = 20;

	if ((s = (char *) malloc(length)) == NULL) {
		NotifyError(wxT("%s: Out of memory for string (%d)."), funcName,
		  length);
		return(NULL);
	}
	sprintf(s, "DSAM googoo");
	return(s);

}
	
/**************************** WriteFile ***************************************/

/*
 * This function writes the data from the output signal of an EarObject.
 * It returns FALSE if it fails in any way.
 * If the fileName is a dash, '-.suffix', then the file will be written to the
 * standard output.
 */

BOOLN
WriteFile_SndFile(WChar *fileName, EarObjectPtr data)
{
	static const WChar *funcName = wxT("WriteFile_SndFile");
	BOOLN	ok = TRUE;
	char	*titleString;
	DataFilePtr	p = dataFilePtr;
	SignalDataPtr	inSignal = _InSig_EarObject(data, 0);

	SetProcessName_EarObject(data, wxT("Output '%s' Sound data file"),
	  GetFileNameFPath_Utility_String(fileName));
	p->defaultSampleRate = 1.0 / inSignal->dt;
	if (!GetDSAMPtr_Common()->segmentedMode || (data->firstSectionFlag)) {
		if (!OpenFile_SndFile(fileName, SFM_WRITE)) {
			NotifyError(wxT("%s: Could not open file '%s'\n"), funcName,
			  fileName);
			return(FALSE);
		}
		dataFilePtr->normalise = CalculateNormalisation_SndFile(inSignal);
		if ((titleString = CreateTitleString_SndFile(data)) == NULL)
			return(FALSE);
		sf_set_string(p->sndFile, SF_STR_TITLE, titleString);
		free(titleString);
	} else {
		if (!OpenFile_SndFile(fileName, SFM_RDWR)) {
			NotifyError(wxT("%s: Could not open file '%s'\n"), funcName,
			  fileName);
			return(FALSE);
		}
		if (sf_command(p->sndFile, SFC_SET_UPDATE_HEADER_AUTO, NULL, SF_TRUE) ==
		  0) {
 			NotifyError(wxT("%s: Failed to set auto update header: '%s'."),
 			  funcName, MBSToWCS_Utility_String(sf_strerror(p->sndFile)));
 			return(FALSE);
		}
		if ((p->sFInfo.channels != inSignal->numChannels) || (fabs(p->
		  sFInfo.samplerate - (1.0 / inSignal->dt)) > DATAFILE_NEGLIGIBLE_SR_DIFF) ||
		  (p->sFInfo.format | SF_FORMAT_SUBMASK != dataFilePtr->
		  subFormatType)) {
			NotifyError(wxT("%s: Cannot append to different format file!"),
			  funcName);
			return(FALSE);
		}
		sf_seek(p->sndFile, 0, SEEK_END);
	}
	ok = WriteFrames_SndFile(inSignal);
	Free_SndFile();
	return(ok);
	
}

