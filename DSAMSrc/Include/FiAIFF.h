/******************
 *
 * File:		FiAIFF.h
 * Purpose:		This Filing module deals with the writing reading of AIFF binary
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

#ifndef	_FIAIFF_H
#define _FIAIFF_H	1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

/*************************** AIFF definitions *********************************/

#define	AIFF_FORM			0x464f524d		/* "FORM" */
#define	AIFF_AIFF			0x41494646		/* "AIFF" */
#define	AIFF_COMM			0x434f4d4d		/* "COMM" */
#define	AIFF_SSND			0x53534e44		/* "SSND" */
#define	AIFF_DSAM			0x4c555432		/* "LUT2" */
#define	AIFF_CHUNK_HEADER_SIZE	8			/* 4 + 4 = ID + SIZE */
#define	AIFF_HEADER_SIZE		30			/* 4 + 8 + 18 */
#define	AIFF_SSND_HEADER_SIZE	8			/* 4 + 4 */
#define	AIFF_DSAM_HEADER_SIZE	26			/* 2 + 2 + 2 + 10 + 10 */
#define AIFF_SMALL_VALUE		1.0e-10

/* Macros */

#define AIFF_DSAM_CHUNK_SIZE(SIGNAL) (AIFF_DSAM_HEADER_SIZE + \
  (SIGNAL)->numChannels / (SIGNAL)->interleaveLevel * UIEEE_FLOAT_EXTENDED_LEN)


/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef struct {

	BOOLN	flag;
	BOOLN	staticTimeFlag;
	uShort	numWindowFrames;	/* No. of window frames in sound chunk. */
	uShort	interleaveLevel;
	double	outputTimeOffset;	
	double	normalise;	
	int32	posOfChannelLabels;

} DSAMChunk, *DSAMChunkPtr;

typedef struct {

	/* Form AIFF chunk data (FORM, AIFF). */
	int32	chunkSize;

	/* DSAM Chunk data (COMM) */
	uShort	numChannels;		/* No. of channels - mono, stereo...etc. */
	int32	numSampleFrames;	/* No. of sample frames in sound chunk. */
	int		sampleSize;			/* No. of data bits for each data point. */
	double	sampleRate;			/* Sample rate of data in sound chunk. */

	/* Sound data Chunk data (SSND). */
	
	int32	offset;				/* - where first sample frame starts */
	int		blockSize;			/* used in conjuction with offset. */
	int32	soundPosition;		/* Start of the sound data - used later. */
	DSAMChunk	dSAMChunk;

} AIFFParams, *AIFFParamsPtr;

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

double		GetDuration_AIFF(char *fileName);

int32		GetFileSize_AIFF(SignalDataPtr theSignal, int32 previousSamples);

int32		GetFormChunkSize_AIFF(SignalDataPtr theSignal,
			  int32 previousSamples);

FILE *		InitialFileRead_AIFF(char *fileName, AIFFParams *pars);

void		InitParams_AIFF(AIFFParamsPtr p);

BOOLN		ReadFile_AIFF(char *fileName, EarObjectPtr data);

void		ReadParams_AIFF(FILE *fp, AIFFParamsPtr p);

BOOLN		ReadPartialHeader_AIFF(FILE *fp, AIFFParamsPtr p);

BOOLN		ReadDSAMChunkData_AIFF(FILE *fp, EarObjectPtr data,
			  AIFFParamsPtr p);

BOOLN		ReadSoundChunkData_AIFF(FILE *fp, EarObjectPtr data,
			  AIFFParamsPtr p);
			  
BOOLN		WriteFile_AIFF(char *fileName, EarObjectPtr data);

void		WriteHeader_AIFF(FILE *fp, EarObjectPtr data,
			  int32 previousSamples);

void		WriteDSAMChunk_AIFF(FILE *fp, SignalDataPtr signal, BOOLN updating);

BOOLN		WriteSSNDChunk_AIFF(FILE *fp, SignalDataPtr signal,
			  int32 previousSamples);

__END_DECLS

#endif
