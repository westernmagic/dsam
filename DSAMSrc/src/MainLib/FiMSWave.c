/******************
 *
 * File:		FiMSWave.c
 * Purpose:		This Filing module deals with the writing reading of Microsoft
 *				"Wave" format binary files.
 * Comments:	14-04-98 LPO: This module has now been separated from the main
 *				FiDataFile module.
 *				20-04-98 LPO: Corrected support for 'stdin' reading using pipes.
 * Authors:		L. P. O'Mard
 * Created:		14 Apr 1998
 * Updated:		20 Apr 1998
 * Copyright:	(c) 1998, University of Essex
 * 
 ******************/

#include <stdlib.h>
#include <stdio.h>

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
#include "FiMSWave.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Subroutines & functions ***********************/
/******************************************************************************/

/**************************** ReadHeader **************************************/

/*
 * This function reades a waver header structure from file.
 * It returns FALSE if it fails.
 */

BOOLN
ReadHeader_Wave(FILE *fp, WaveHeaderPtr p)
{
	static const char *funcName = "ReadHeader_Wave";
	int32	subSize, chunkSize, iD;

	if ((p->identifier = ReadFileIdentifier_DataFile(fp, WAVE_RIFF,
	  "MS WAVE")) == 0)
		return(FALSE);
	p->length = chunkSize = dataFilePtr->Read32Bits(fp);
	if ((p->chunkType = dataFilePtr->Read32Bits(fp)) != WAVE_WAVE) {
		NotifyError("%s: Could not find chunk identifier.", funcName);
		return(FALSE);
	}
	p->subChunkLength = p->dataChunkLength = 0;
	while ((chunkSize - 4) > 0) {
		chunkSize -= 4;
		switch (iD = dataFilePtr->Read32Bits(fp)) {
		case WAVE_FMT:
			p->subChunkLength = dataFilePtr->Read32Bits(fp);
			chunkSize -= subSize = p->subChunkLength;
			p->format = dataFilePtr->Read16Bits(fp);
			subSize -= 2;
			p->numChannels = dataFilePtr->Read16Bits(fp);
			subSize -= 2;
			p->sampleRate = dataFilePtr->Read32Bits(fp);
			subSize -= 4;
			p->byteRate = dataFilePtr->Read32Bits(fp);
			subSize -= 4;
			p->blockAlign = dataFilePtr->Read16Bits(fp);
			subSize -= 2;
			p->bitsPerSample = dataFilePtr->Read16Bits(fp);
			subSize -= 2;
			SetPosition_UPortableIO(fp, subSize, SEEK_CUR);
			break;
		case WAVE_DATA:
			p->dataChunkLength = dataFilePtr->Read32Bits(fp);
			chunkSize -= subSize = p->dataChunkLength;
			p->soundPosition = GetPosition_UPortableIO(fp);
			SetPosition_UPortableIO(fp, subSize, SEEK_CUR);
			break;
		default:
			chunkSize -= subSize = dataFilePtr->Read32Bits(fp);
			SetPosition_UPortableIO(fp, subSize, SEEK_CUR);
		}
	}
	if (!p->subChunkLength) {
		NotifyError("%s: Could not find FMT chunk identifier.", funcName);
		return(FALSE);
	}
	if (!p->dataChunkLength) {
		NotifyError("%s: Could not find DATA chunk identifier.", funcName);
		return(FALSE);
	}
	return(TRUE);

}

/**************************** InitialFileRead *********************************/

/*
 * This function returns a file pointer from the initial reading of a MS Wave
 * file.
 * It is created because this initial reading process is required for reading
 * the entire file, or just extracting the size of the file.
 * It returns a pointer to the file if successful, or NULL if it fails.
 */

FILE *
InitialFileRead_Wave(char *fileName)
{
	static const char *funcName = "InitialFileRead_Wave";
	FILE	*fp;

	if (dataFilePtr->endian == DATA_FILE_DEFAULT_ENDIAN)
		SetRWFormat_DataFile(DATA_FILE_LITTLE_ENDIAN);
	if ((fp = OpenFile_DataFile(fileName, FOR_BINARY_READING)) == NULL) {
		NotifyError("%s: Couldn't read file '%s'.", funcName, fileName);
		return(NULL);
	}
	return(fp);

}

/**************************** GetNumSamples ***********************************/

/*
 * This function calculates the number of samples from the wave header
 * parameters.
 */

ChanLen
GetNumSamples_Wave(WaveHeader *p)
{
	return(p->dataChunkLength / p->numChannels / (p->bitsPerSample / 8));

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
ReadFile_Wave(char *fileName, EarObjectPtr data)
{
	static const char *funcName = "ReadFile_Wave";
	int		j;
	ChanLen	i, length;
	FILE	*fp;
	WaveHeader	pars;

	if ((fp = InitialFileRead_Wave(fileName)) == NULL) {
		NotifyError("%s: Could not read initial file structure from '%s'.",
		  funcName, fileName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "'%s' Microsoft Wave file",
	  GetFileNameFPath_Utility_String(fileName));
	if (!GetDSAMPtr_Common()->segmentedMode || (data->timeIndex ==
	  PROCESS_START_TIME) || (fp != stdin)) {
		if (!ReadHeader_Wave(fp, &pars)) {
			NotifyError("%s: Could not read header.", funcName);
			CloseFile(fp);
			return(FALSE);
		}
		dataFilePtr->numChannels = pars.numChannels;
		dataFilePtr->defaultSampleRate = pars.sampleRate;
		dataFilePtr->wordSize = pars.bitsPerSample / 8;
		dataFilePtr->numSamples = GetNumSamples_Wave(&pars);
		if (!InitProcessVariables_DataFile(data, dataFilePtr->numSamples,
		  pars.sampleRate)) {
			NotifyError("%s: Could not initialise process variables.",
			  funcName);
			return(FALSE);
		}
	}
	if ((length = SetIOSectionLength_DataFile(data)) <= 0)
		return(FALSE);
	if (!InitOutSignal_EarObject(data, (uShort) dataFilePtr->numChannels,
	  length, 1.0 / dataFilePtr->defaultSampleRate)) {
		NotifyError("%s: Cannot initialise output signal", funcName);
		return(FALSE);
	}
	if (pars.numChannels == 2)
		SetInterleaveLevel_SignalData(data->outSignal, 2);
	if (fp != stdin)
		SetPosition_UPortableIO(fp, pars.soundPosition + (int32)
		  (data->timeIndex + dataFilePtr->timeOffsetCount - 1) *
		  dataFilePtr->numChannels * dataFilePtr->wordSize, SEEK_SET);
	else if (data->timeIndex == PROCESS_START_TIME) {
		for (i = 0; i < dataFilePtr->timeOffsetCount * dataFilePtr->numChannels;
		  i++)
			ReadSample_DataFile(fp);
	}
	for (i = 0; i < length; i++)
		for (j = 0; j < dataFilePtr->numChannels; j++)
			data->outSignal->channel[j][i] = ReadSample_DataFile(fp);
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
GetDuration_Wave(char *fileName)
{
	static const char *funcName = "GetDuration_Wave";
	FILE	*fp;
	WaveHeader	pars;

	if ((fp = InitialFileRead_Wave(fileName)) == NULL) {
		NotifyError("%s: Could not read initial file structure from '%s'.",
		  funcName, fileName);
		return(-1.0);
	}
	if (!ReadHeader_Wave(fp, &pars)) {
		NotifyError("%s: Could not read header.", funcName);
		CloseFile(fp);
		return(FALSE);
	}
	CloseFile(fp);
	return((double) GetNumSamples_Wave(&pars) / pars.sampleRate);

}

/**************************** WriteHeader *************************************/

/*
 * This routine writes the header structure for the Wave format to a file
 * (stream).
 * It does not write the sound data itself.
 * The file stream position is always set to the beginning of the file before
 * writing the header.
 */

void
WriteHeader_Wave(FILE *fp, EarObjectPtr data, int32 offset)
{
	int32		totalLength, dataLength;
	
	dataLength = (int32) (offset + data->outSignal->length *
	  data->outSignal->numChannels * dataFilePtr->wordSize);
	totalLength = WAVE_HEADER_SIZE + dataLength;

	SetPosition_UPortableIO(fp, 0L, SEEK_SET);
	dataFilePtr->Write32Bits(fp, WAVE_RIFF);
	dataFilePtr->Write32Bits(fp, totalLength);		/* Total file length */
	dataFilePtr->Write32Bits(fp,  WAVE_WAVE);	/* Type of Chunk */

	dataFilePtr->Write32Bits(fp, WAVE_FMT);	/* Sub chunk type */
	dataFilePtr->Write32Bits(fp, 16);			/* Sub chunk Size */
	dataFilePtr->Write16Bits(fp, WAVE_PCM_CODE); /* No idea what this is for. */
	dataFilePtr->Write16Bits(fp, data->outSignal->numChannels);
	dataFilePtr->Write32Bits(fp,(int32) (1.0 / data->outSignal->dt + 0.5));
											/*Sample rate*/
	dataFilePtr->Write32Bits(fp, (int32) (dataFilePtr->wordSize /
	  data->outSignal->dt + 0.5));
	dataFilePtr->Write16Bits(fp, (int16) (dataFilePtr->wordSize * data->
	  outSignal->numChannels));	/* block align */
	/* Next line: S. size in bits*/
	dataFilePtr->Write16Bits(fp, (int16) (dataFilePtr->wordSize * 8));

	dataFilePtr->Write32Bits(fp, WAVE_DATA);
	dataFilePtr->Write32Bits(fp, dataLength);	/* Sample count */

}

/**************************** WriteFile ***************************************/

/*
 * This function writes the data from the output signal of an EarObject.
 * It returns FALSE if it fails in any way.
 * If the fileName is a dash, '-.suffix', then the file will be written to the
 * standard output.
 */

BOOLN
WriteFile_Wave(char *fileName, EarObjectPtr data)
{
	static const char *funcName = "WriteFile_Wave";
	BOOLN	ok;
	int32	dataOffset;
	FILE	*fp;
	WaveHeader	pars;

	if (dataFilePtr->endian == DATA_FILE_DEFAULT_ENDIAN)
		SetRWFormat_DataFile(DATA_FILE_LITTLE_ENDIAN_UNSIGNED);
	if (!GetDSAMPtr_Common()->segmentedMode || data->firstSectionFlag) {
		if ((fp = OpenFile_DataFile(fileName, FOR_BINARY_WRITING)) == NULL) {
			NotifyError("%s: Couldn't open file '%s'.", funcName, fileName);
			return(FALSE);
		}
		dataOffset = 0L;
		dataFilePtr->normalise = CalculateNormalisation_DataFile(
		  data->outSignal);
	} else {
		if ((fp = OpenFile_DataFile(fileName, FOR_BINARY_UPDATING)) == NULL) {
			NotifyError("%s: Couldn't open file '%s'.", funcName, fileName);
			return(FALSE);
		}
		if (!ReadHeader_Wave(fp, &pars)) {
			NotifyError("%s: Could not read header (using segmented mode).",
			  funcName);
			CloseFile(fp);
			return(FALSE);
		}
		if ((pars.numChannels != data->outSignal->numChannels) ||
		  (pars.sampleRate != (int32) (1.0 / data->outSignal->dt + 0.5)) ||
		  ((pars.bitsPerSample / 8) != dataFilePtr->wordSize)) {
			NotifyError("%s: Cannot append to different format file!",
			  funcName);
			return(FALSE);
		}
		dataOffset = pars.dataChunkLength;
	}
	WriteHeader_Wave(fp, data, dataOffset);
	SetPosition_UPortableIO(fp, 0L, SEEK_END);
	ok = WriteSignal_DataFile(fp, data->outSignal);
	CloseFile(fp);
	return(ok);
	
}

