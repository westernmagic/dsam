/******************
 *
 * File:		FiMSWave.h
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

#ifndef	_FIMSWAVE_H
#define _FIMSWAVE_H	1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

/*************************** Microsoft Wave definitions format ****************/

#define WAVE_RIFF		0x46464952		/* "FFIR" (RIFF) */
#define WAVE_WAVE		0x45564157		/* "EVAW" (WAVE) */
#define WAVE_FMT		0x20746D66		/* " tmf" (fmt ) */
#define WAVE_DATA		0x61746164		/* "atad" (data) */
#define WAVE_PCM_CODE	1
#define WAVE_MONO		1
#define WAVE_STEREO		2
#define WAVE_HEADER_SIZE	44

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef struct {

	int32	identifier;		/* 'RIFF' */
	int32	length;			/* total file length. */
	int32	chunkType;		/* 'WAVE' */

	int32	subChunkType;	/* 'fmt ' */
	int32	subChunkLength;	/* length of sub_chunk, =16 */
	int16	format;			/* should be 1 for PCM-code */
	int16	numChannels;	/* 1 Mono, 2 Stereo */
	int32	sampleRate;		/* frequency of sample */
	int32	byteRate;		/* No. of bytes per second. */
	int16	blockAlign;		/* = numChannels * bitsperSample / 8*/
	int16	bitsPerSample;	/* 8 bits = 8, 16 bits  = 16, etc. */
	int16	sampleSize;		/* 8, 12 or 16 bit */ 

	int32	dataChunk;		/* 'data' */
	int32	dataChunkLength;/* = numSamples * numChannels * bitsPerSample / 8 */

	/* My info */
	int32	soundPosition;
  
} WaveHeader, *WaveHeaderPtr;

/******************************************************************************/
/*************************** External variables *******************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

double	GetDuration_Wave(char *fileName);

ChanLen	GetNumSamples_Wave(WaveHeader *p);

BOOLN	ReadFile_Wave(char *fileName, EarObjectPtr data);

BOOLN	ReadHeader_Wave(FILE *fp, WaveHeaderPtr p);

BOOLN	WriteFile_Wave(char *fileName, EarObjectPtr data);

void	WriteHeader_Wave(FILE *fp, EarObjectPtr data, int32 offset);

__END_DECLS

#endif
