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

	if ((p->identifier = ReadFileIdentifier_DataFile(fp, WAVE_RIFF,
	  "MS WAVE")) == 0)
		return(FALSE);
	p->length = dataFilePtr->Read32Bits(fp);
	if ((p->chunkType = dataFilePtr->Read32Bits(fp)) != WAVE_WAVE) {
		NotifyError("%s: Could not find chunk identifier.", funcName);
		return(FALSE);
	}
	if ((p->subChunkType = dataFilePtr->Read32Bits(fp)) != WAVE_FMT) {
		NotifyError("%s: Could not find sub-chunk identifier.", funcName);
		return(FALSE);
	}
	p->subChunkLength = dataFilePtr->Read32Bits(fp);
	p->format = dataFilePtr->Read16Bits(fp);
	p->numChannels = dataFilePtr->Read16Bits(fp);
	p->sampleRate = dataFilePtr->Read32Bits(fp);
	p->byteRate = dataFilePtr->Read32Bits(fp);
	p->blockAlign = dataFilePtr->Read16Bits(fp);
	p->bitsPerSample = dataFilePtr->Read16Bits(fp);
	if ((p->dataChunk = dataFilePtr->Read32Bits(fp)) != WAVE_DATA) {
		NotifyError("%s: Could not find data chunk identifier.", funcName);
		return(FALSE);
	}
	p->dataChunkLength = dataFilePtr->Read32Bits(fp);
	return(TRUE);

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

	if (dataFilePtr->endian == DATA_FILE_DEFAULT_ENDIAN)
		SetRWFormat_DataFile(DATA_FILE_LITTLE_ENDIAN);
	if ((fp = OpenFile_DataFile(fileName, FOR_BINARY_READING)) == NULL) {
		NotifyError("%s: Couldn't read file '%s'.", funcName, fileName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "'%s' Microsoft Wave file", fileName);
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
		dataFilePtr->numSamples = pars.dataChunkLength / pars.numChannels /
		  dataFilePtr->wordSize;
		if (!InitProcessVariables_DataFile(data, dataFilePtr->numSamples,
		  pars.sampleRate)) {
			NotifyError("%s: Could not initialise process variables.",
			  funcName);
			return(FALSE);
		}
	}
	if ((length = SetIOSectionLength_DataFile(data)) <= 0)
		return(FALSE);
	if (!InitOutSignal_EarObject(data, (int) dataFilePtr->numChannels, length,
	  1.0 / dataFilePtr->defaultSampleRate)) {
		NotifyError("%s: Cannot initialise output signal", funcName);
		return(FALSE);
	}
	if (pars.numChannels == 2)
		SetInterleaveLevel_SignalData(data->outSignal, 2);
	if (fp != stdin)
		SetPosition_UPortableIO(fp, GetPosition_UPortableIO(fp) + (int32)
		  (data->timeIndex + dataFilePtr->timeOffsetCount - 1) *
		  dataFilePtr->numChannels * dataFilePtr->wordSize, SEEK_SET);
	else if (data->timeIndex == PROCESS_START_TIME) {
		for (i = 0; i < dataFilePtr->timeOffsetCount * dataFilePtr->numChannels;
		  i++)
			(* dataFilePtr->ReadSample)(fp);
	}
	for (i = 0; i < length; i++)
		for (j = 0; j < dataFilePtr->numChannels; j++) {
			data->outSignal->channel[j][i] =
			  (ChanData) ((* dataFilePtr->ReadSample)(fp) /
			    dataFilePtr->normalise);
		}
	CloseFile(fp);
	return(TRUE);

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
	dataFilePtr->Write16Bits(fp, dataFilePtr->wordSize * data->outSignal->
	  numChannels);	/* block align */
	dataFilePtr->Write16Bits(fp, dataFilePtr->wordSize * 8);/* S. size in bits*/

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

