/******************
 *
 * File:		FiAIFF.c
 * Purpose:		This Filing module deals with the writing reading of AIFF binary
 *				data files.
 * Comments:	14-04-98 LPO: This module has now been separated from the main
 *				FiDataFile module.
 *				20-04-98 LPO: Outputs error message if an attempt is made to
 *				read files from 'stdin'.
 *				20-04-98 LPO: In segmented mode the normalisation calculation
 *				operates on the first segment only.
 *				The DBL_EPSILON was replaced by AIFF_SMALL_VALUE, because of
 *				rounding errors under glibc-2.0.6.
 *				30-06-98 LPO: Introduced use of the SignalData structure's
 *				'numWindowFrames' field.  A corresponding field has also been
 *				added to the DSAM chunk.
 *				The WriteDSAMChunk routine has been amended so that the
 *				numWindowFrames field can be updated in segment processing mode.
 *				03-07-98 LPO: Introduced staticTimeFlag into DSAM Chunk.
 *				21-12-98 LPO: The interleave level was not being set by non-
 *				DSAM AIFF files.  Now 2 channel data will be assumed to be
 *				binarual, with the interleave level set to 2.
 *				27-04-99 LPO: I have fixed moved the "wordSize" filed setting
 *				to the 'ReadFile_' routine from the 'ReadSoundDataChunk_'
 *				routine.  This needed to be done for when the 'normalise' field
 *				was set.
 *				05-05-99 LPO: I corrected the read normalisation setting and
 *				this routine now uses the "ReadSample" routine.
 * Authors:		L. P. O'Mard
 * Created:		14 Apr 1998
 * Updated:		05 May 1999
 * Copyright:	(c) 1999, University of Essex.
 * 
 ******************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>

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
#include "FiAIFF.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Subroutines & functions ***********************/
/******************************************************************************/

/**************************** InitParams **************************************/

/*
 * This routine initialises the AIFFParams data structure.
 * It expects the pointer to have been initialised.
 */

void
InitParams_AIFF(AIFFParamsPtr p)
{
	p->chunkSize = 0;
	p->numChannels = 0;
	p->numSampleFrames = 0;
	p->sampleSize = 0;
	p->sampleRate = 0;
	p->offset = 0;
	p->blockSize = 0;
	p->soundPosition = 0;
	p->dSAMChunk.flag = FALSE;

}

/**************************** ReadParams **************************************/

/*
 * This routine reads the parameters from the chunks of an AIFF file (stream).
 * It does not read the sound data itself, but does return its position
 * in the file.
 * At present only the common chunk and sound data chunk are used, all other
 * chunks are ignored.
 */

void
ReadParams_AIFF(FILE *fp, AIFFParamsPtr p)
{
	int32	subSize;

	while ((p->chunkSize - 4) > 0) {
		p->chunkSize -= 4;
		switch(dataFilePtr->Read32Bits(fp)){
		case	AIFF_COMM:
			p->chunkSize -= subSize = dataFilePtr->Read32Bits(fp);
			p->numChannels = dataFilePtr->Read16Bits(fp);
			subSize -= 2;
			p->numSampleFrames = dataFilePtr->Read32Bits(fp);
			subSize -= 4;
			p->sampleSize = dataFilePtr->Read16Bits(fp);
			subSize -= 2;
			p->sampleRate = dataFilePtr->ReadIEEEExtended(fp);
			subSize -= 10;
			while (subSize > 0){
				Read8Bits(fp);
				subSize--;
			}
			break;
		case	AIFF_SSND:
			p->chunkSize -= subSize = dataFilePtr->Read32Bits(fp);
			p->offset = dataFilePtr->Read32Bits(fp);
			subSize -= 4;
			p->blockSize = dataFilePtr->Read32Bits(fp);
			subSize -= 4;
			p->soundPosition = GetPosition_UPortableIO(fp) + p->offset;
			while (subSize > 0){
				Read8Bits(fp);
				subSize--;
			}
			break;
		case	AIFF_DSAM:
			p->dSAMChunk.flag = TRUE;
			p->chunkSize -= subSize = dataFilePtr->Read32Bits(fp);
			p->dSAMChunk.interleaveLevel = dataFilePtr->Read16Bits(fp);
			subSize -= 2;
			p->dSAMChunk.numWindowFrames = dataFilePtr->Read16Bits(fp);
			subSize -= 2;
			p->dSAMChunk.staticTimeFlag = dataFilePtr->Read16Bits(fp);
			subSize -= 2;
			p->dSAMChunk.outputTimeOffset = dataFilePtr->ReadIEEEExtended(fp);
			subSize -= 10;
			p->dSAMChunk.normalise = dataFilePtr->ReadIEEEExtended(fp);
			subSize -= 10;
			p->dSAMChunk.posOfChannelLabels = GetPosition_UPortableIO(fp);
			while (subSize > 0){
				Read8Bits(fp);
				subSize--;
			}
			break;
		default:
			p->chunkSize -= subSize = dataFilePtr->Read32Bits(fp);
			while (subSize > 0){
				Read8Bits(fp);
				subSize--;
			}
			break;
		} /* switch */
	}

}

/**************************** ReadSoundChunkData ******************************/

/*
 * This routine reads the sound data from an AIFF file stream.
 * It expects the soundPosition for the file stream to have been
 * set to the start of the sound data.
 * In segmented mode, this routine returns FALSE when it gets to the end of
 * the signal.
 */

BOOLN
ReadSoundChunkData_AIFF(FILE *fp, EarObjectPtr data, AIFFParamsPtr p)
{
	static const char *funcName = "ReadSoundChunkData_AIFF";
	int		j;
	ChanLen	i, length;

	if ((length = SetIOSectionLength_DataFile(data)) <= 0)
		return(FALSE);
	if (!InitOutSignal_EarObject(data, p->numChannels, length, 1.0 /
	  p->sampleRate) ) {
		NotifyError("%s: Cannot initialise output signal.", funcName);
		return(FALSE);
	}	
	SetPosition_UPortableIO(fp, (int32) (p->soundPosition +
	  (dataFilePtr->timeOffsetIndex + data->timeIndex) * p->numChannels *
	  dataFilePtr->wordSize), SEEK_SET);
	for (i = 0; i < length; i++)
		for (j = 0; j < p->numChannels; j++)
			data->outSignal->channel[j][i] = ReadSample_DataFile(fp);
	return(TRUE);

}

/**************************** ReadDSAMChunkData *******************************/

/*
 * This routine reads the DSAM data from an AIFF file stream.
 * It expects the posOfChannelLabels for the file stream to have been
 * set to the start of the CFList.
 */

BOOLN
ReadDSAMChunkData_AIFF(FILE *fp, EarObjectPtr data, AIFFParamsPtr p)
{
	int		i;

	SetInterleaveLevel_SignalData(data->outSignal,
	  p->dSAMChunk.interleaveLevel);
	SetNumWindowFrames_SignalData(data->outSignal,
	  p->dSAMChunk.numWindowFrames);
	SetOutputTimeOffset_SignalData(data->outSignal,
	  p->dSAMChunk.outputTimeOffset);
	SetStaticTimeFlag_SignalData(data->outSignal, p->dSAMChunk.staticTimeFlag);
	SetPosition_UPortableIO(fp, p->dSAMChunk.posOfChannelLabels, SEEK_SET);
	for (i = 0; i < p->numChannels / p->dSAMChunk.interleaveLevel; i++)
		data->outSignal->info.chanLabel[i] = dataFilePtr->ReadIEEEExtended(fp);
	return(TRUE);

}

/**************************** ReadFile ****************************************/

/*
 * This function reads a file in AIFF format.
 * The data is stored in the output signal of an EarObject which will have the
 * same number of channels as the input signal.
 * If a dash, '-.sufffix', is given as the file name, then the data will be 
 * read from the standard input.
 * It returns TRUE if successful.
 * In segmented mode, this routine returns FALSE when it gets to the end of
 * the signal.
 */

BOOLN
ReadFile_AIFF(char *fileName, EarObjectPtr data)
{
	static const char *funcName = "ReadFile_AIFF";
	FILE	*fp;
	AIFFParams	pars;

	if ((fp = OpenFile_DataFile(fileName, FOR_BINARY_READING)) == NULL) {
		NotifyError("%s: Couldn't open file.", funcName);
		return(FALSE);
	}
	if (fp == stdin) {
		NotifyError("%s: Reading from stdin not supported for this format.",
		  funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "'%s' AIFF file",
	  GetFileNameFPath_Utility_String(fileName));
	InitParams_AIFF(&pars);
	if (dataFilePtr->endian == 0)
		SetRWFormat_DataFile(DATA_FILE_BIG_ENDIAN);
	rewind(fp);
	if (!ReadFileIdentifier_DataFile(fp, AIFF_FORM, "AIFF")) {
		NotifyError("%s: Couldn't find initial chunk in file '%s'.", funcName,
		  fileName);
		return(FALSE);
	}
	pars.chunkSize = dataFilePtr->Read32Bits(fp);
	if (dataFilePtr->Read32Bits(fp) != AIFF_AIFF){
		NotifyError("%s: Couldn't find AIFF chunk in file '%s'.", funcName,
		  fileName);
		return(FALSE);
	}

	ReadParams_AIFF(fp, &pars);

	if (!pars.soundPosition) {
		NotifyError("%s: Didn't find a SSND chunk in file '%s'.", funcName,
		  fileName);
		return(FALSE);
	}
	dataFilePtr->wordSize = BITS_TO_BYTES(pars.sampleSize);
	if (!InitProcessVariables_DataFile(data, pars.numSampleFrames,
	  pars.sampleRate)) {
		NotifyError("%s: Could not initialise process variables.", funcName);
		return(FALSE);
	}
	if (pars.dSAMChunk.flag)
		dataFilePtr->normalise = pars.dSAMChunk.normalise;
	if (!ReadSoundChunkData_AIFF(fp, data, &pars)) {
		if (!GetDSAMPtr_Common()->segmentedMode)
			NotifyError("%s: Couldn't read the SSND chunk in file '%s'.",
			  funcName ,fileName);
		return(FALSE);
	}
	if (pars.dSAMChunk.flag) {
		if (!ReadDSAMChunkData_AIFF(fp, data, &pars)) {
			NotifyError("%s: Couldn't read the DSAM chunk in file '%s'.",
			  funcName, fileName);
			return(FALSE);
		}
	} else {
		if (data->outSignal->numChannels == 2)
			SetInterleaveLevel_SignalData(data->outSignal, 2);
	}
	CloseFile(fp);
	return(TRUE);

}

/**************************** GetFormChunkSize ********************************/

/*
 * This function returns the form chunk size required for a signal.
 * The eight bytes (+8) required by the chunk ID and the chunk size parameter
 * must be added to get the total file size in bytes,
 * i.e. "AIFF_CHUNK_HEADER_SIZE".
 */

int32
GetFormChunkSize_AIFF(SignalDataPtr theSignal, int32 previousSamples)
{
	int32	totalSize;

	totalSize = (int32) (AIFF_HEADER_SIZE + AIFF_CHUNK_HEADER_SIZE +
	  AIFF_SSND_HEADER_SIZE + (theSignal->length + previousSamples) *
	  theSignal->numChannels * dataFilePtr->wordSize);
	/* DSAM Chunk */
	totalSize += AIFF_CHUNK_HEADER_SIZE;/* chunk ID and chunk size parameters */
	totalSize += AIFF_DSAM_CHUNK_SIZE(theSignal);
	return(totalSize);

}

/**************************** WriteHeader *************************************/

/*
 * This routine writes the FORM and COMMON header information of the AIFF
 * format to a file (stream).
 * The file stream position is always set to the beginning of the file before
 * writing the header.
 */

void
WriteHeader_AIFF(FILE *fp, EarObjectPtr data, int32 previousSamples)
{
	int32		chunkSize;
	
	chunkSize = GetFormChunkSize_AIFF(data->outSignal, previousSamples);

	SetPosition_UPortableIO(fp, 0L, SEEK_SET);
	dataFilePtr->Write32Bits(fp, AIFF_FORM);	/* Magic Number */
	dataFilePtr->Write32Bits(fp, chunkSize);	/* Chunk Size */
	dataFilePtr->Write32Bits(fp, AIFF_AIFF);	/* Type of Chunk */

						/* AIFF Common Chunk */
	dataFilePtr->Write32Bits(fp, AIFF_COMM);
	dataFilePtr->Write32Bits(fp, 18);		/* Chunk Size */
	dataFilePtr->Write16Bits(fp, data->outSignal->numChannels);
	dataFilePtr->Write32Bits(fp, (int32) data->outSignal->length +
	  previousSamples);
	 /* next line: write Sample Size */
	dataFilePtr->Write16Bits(fp, (int16) (8 * dataFilePtr->wordSize));
	dataFilePtr->WriteIEEEExtended(fp, 1.0 / data->outSignal->dt);

}

/**************************** WriteSSNDChunk **********************************/

/*
 * This routine writes the sound chunk of the AIFF format to a file (stream).
 */

BOOLN
WriteSSNDChunk_AIFF(FILE *fp, SignalDataPtr signal,
  int32 previousSamples)
{
	int32		chunkSize;

	chunkSize = (int32) (AIFF_SSND_HEADER_SIZE + (signal->length +
	  previousSamples) * signal->numChannels * dataFilePtr->wordSize);
	dataFilePtr->Write32Bits(fp, AIFF_SSND);
	dataFilePtr->Write32Bits(fp, chunkSize);	/* Chunk Size */
	dataFilePtr->Write32Bits(fp, 0);		/* Offset */
	dataFilePtr->Write32Bits(fp, 0);		/* Block Size*/
	SetPosition_UPortableIO(fp, 0L, SEEK_END);
	return(WriteSignal_DataFile(fp, signal));

}

/**************************** WriteDSAMChunk **********************************/

/*
 * This routine writes the 'DSAM' chunk of the AIFF format to a file (stream).
 * The AIFF_DSAM_CHUNK_SIZE does not include the 8 bytes for the chunk
 * ID and the chunk size, i.e. "AIFF_CHUNK_HEADER_SIZE".
 */

void
WriteDSAMChunk_AIFF(FILE *fp, SignalDataPtr signal, BOOLN updating)
{
	int		i;
	int32	chunkSize;

	chunkSize = AIFF_DSAM_CHUNK_SIZE(signal);
	dataFilePtr->Write32Bits(fp, AIFF_DSAM);
	dataFilePtr->Write32Bits(fp, chunkSize);	/* Chunk Size */
	dataFilePtr->Write16Bits(fp, signal->interleaveLevel);
	dataFilePtr->Write16Bits(fp, signal->numWindowFrames);
	dataFilePtr->Write16Bits(fp, (int16) signal->staticTimeFlag);
	if (!updating) {
		dataFilePtr->WriteIEEEExtended(fp, signal->outputTimeOffset);
		dataFilePtr->WriteIEEEExtended(fp, dataFilePtr->normalise);
		for (i = 0; i < signal->numChannels / signal->interleaveLevel; i++)
			dataFilePtr->WriteIEEEExtended(fp, signal->info.chanLabel[i]);
	} else
		SetPosition_UPortableIO(fp, AIFF_CHUNK_HEADER_SIZE + AIFF_HEADER_SIZE +
		  AIFF_CHUNK_HEADER_SIZE + chunkSize, SEEK_SET);

}

/**************************** ReadPartialHeader *******************************/

/*
 * This routine reads some of the parameters from the chunks of an AIFF file
 * (stream).
 * It expects the AIFF file to be set out the "DSAM" way as it used only
 * with files created in DSAM in the segmented mode.
 * The dataFilePtr->Read... routine is used without assignment for dummy reads.
 */

BOOLN
ReadPartialHeader_AIFF(FILE *fp, AIFFParamsPtr p)
{
	static const char *funcName = "ReadPartialHeader_AIFF";

	SetPosition_UPortableIO(fp, 0L, SEEK_SET);
	if (dataFilePtr->Read32Bits(fp) != AIFF_FORM) {
		NotifyError("%s: Could not find initial chunk.", funcName);
		return(FALSE);
	}
	p->chunkSize = dataFilePtr->Read32Bits(fp);
	dataFilePtr->Read32Bits(fp);	/* AIFF_AIFF */
	dataFilePtr->Read32Bits(fp);	/* AIFF_COMM */
	dataFilePtr->Read32Bits(fp);	/* Chunk size */
	p->numChannels = dataFilePtr->Read16Bits(fp);
	p->numSampleFrames = dataFilePtr->Read32Bits(fp);
	p->sampleSize = dataFilePtr->Read16Bits(fp);
	p->sampleRate = dataFilePtr->ReadIEEEExtended(fp);
	if (dataFilePtr->Read32Bits(fp) != AIFF_DSAM) {
		NotifyError("%s: Could not find DSAM chunk.", funcName);
		return(FALSE);
	}
	dataFilePtr->Read32Bits(fp);	/* Chunk Size */
	p->dSAMChunk.interleaveLevel = dataFilePtr->Read16Bits(fp);
	return(TRUE);

}

/**************************** WriteFile ***************************************/

/*
 * This function writes the data from the output signal of an EarObject.
 * It returns FALSE if it fails in any way.
 * If the fileName is a dash, '-.suffix', then the file will be written to the
 * standard output.
 * This AIFF format has the sound chunk at the end, so that it can add to the
 * file in segmented mode, without reading the rest of the file.
 * The DBL_EPSILON was replaced by 1e-10, because of rounding errors under
 * glibc-2.0.6.
 */

BOOLN
WriteFile_AIFF(char *fileName, EarObjectPtr data)
{
	static const char *funcName = "WriteFile_AIFF";
	int32	previousSamples;
	FILE	*fp;
	AIFFParams pars;

	if (dataFilePtr->endian == 0)
		SetRWFormat_DataFile(DATA_FILE_BIG_ENDIAN);
	if (!GetDSAMPtr_Common()->segmentedMode || (data->firstSectionFlag)) {
		if ((fp = OpenFile_DataFile(fileName, FOR_BINARY_WRITING)) == NULL) {
			NotifyError("%s: Couldn't open file '%s'.", funcName, fileName);
			return(FALSE);
		}
		previousSamples = 0L;
		dataFilePtr->normalise = CalculateNormalisation_DataFile(
		  data->outSignal);
	} else {
		if ((fp = OpenFile_DataFile(fileName, FOR_BINARY_UPDATING)) == NULL) {
			NotifyError("%s: Couldn't open file '%s'.", funcName, fileName);
			return(FALSE);
		}
		if (!ReadPartialHeader_AIFF(fp, &pars)) {
			NotifyError("%s: Could not read header (using segmented mode).",
			  funcName );
			CloseFile(fp);
			return(FALSE);
		}
		if ((pars.numChannels != data->outSignal->numChannels) ||
		  (fabs(pars.sampleRate - (1.0 / data->outSignal->dt)) >
		  AIFF_SMALL_VALUE) || (BITS_TO_BYTES(pars.sampleSize) !=
		  dataFilePtr->wordSize) || (pars.dSAMChunk.interleaveLevel !=
		  data->outSignal->interleaveLevel)) {
			NotifyError("%s: Cannot append to different format file!",
			  funcName);
			return(FALSE);
		}
		previousSamples = pars.numSampleFrames;
	}
	WriteHeader_AIFF(fp, data, previousSamples);
	WriteDSAMChunk_AIFF(fp, data->outSignal, (previousSamples != 0));
	if (!WriteSSNDChunk_AIFF(fp, data->outSignal, previousSamples)) {
		NotifyError("%s: Couldn't write SSND chunk.", funcName);
		return(FALSE);
	}
	if (fp != stdin)
		CloseFile(fp);
	return(TRUE);
	
}
