/**********************
 *
 * File:		IOAudioIn.h
 * Purpose:		Audio input from sound system.
 * Comments:	Written using ModuleProducer version 1.4.2 (Dec 19 2003).
 * Author:		L. P. O'Mard
 * Created:		08 Nov 2005
 * Updated:	
 * Copyright:	(c) 2005, CNBH, University of Essex.
 *
 *********************/

#ifndef _IOAUDIOIN_H
#define _IOAUDIOIN_H 1

#ifdef USE_PORTAUDIO

#include <portaudio.h>

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define	IO_AUDIOIN_MOD_NAME			"IO_AudioIn"
#define IO_AUDIOIN_NUM_PARS			7
#define	IO_AUDIOIN_SAMPLE_FORMAT	paFloat32
#define IO_AUDIOIN_SAMPLE_SILENCE	0.0f

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef	float AudioInSample;

typedef enum {

	IO_AUDIOIN_DEVICEID,
	IO_AUDIOIN_NUMCHANNELS,
	IO_AUDIOIN_SEGMENTSPERBUFFER,
	IO_AUDIOIN_SAMPLERATE,
	IO_AUDIOIN_DURATION,
	IO_AUDIOIN_SLEEP,
	IO_AUDIOIN_GAIN

} AudioInParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	deviceIDFlag, numChannelsFlag, sampleRateFlag, durationFlag;
	BOOLN	segmentsPerBufferFlag, sleepFlag, gainFlag;
	BOOLN	updateProcessVariablesFlag;
	int		deviceID;
	int		numChannels;
	int		segmentsPerBuffer;
	double	sampleRate;
	double	duration;
	double	sleep;
	double	gain;

	/* Private members */
	UniParListPtr	parList;
	BOOLN	portAudioInitialised;
	BOOLN	streamStarted;
	BOOLN	segmentReadyFlag;
	long		frameIndex;
	long		segmentIndex;
	PaError	pAError;
	PaStream	*stream;
	EarObjectPtr	buffer;
	EarObjectPtr	data;

} AudioIn, *AudioInPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	AudioInPtr	audioInPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckData_IO_AudioIn(EarObjectPtr data);

BOOLN	CheckPars_IO_AudioIn(void);

BOOLN	FreeProcessVariables_IO_AudioIn(void);

BOOLN	Free_IO_AudioIn(void);

UniParListPtr	GetUniParListPtr_IO_AudioIn(void);

BOOLN	InitModule_IO_AudioIn(ModulePtr theModule);

BOOLN	InitProcessVariables_IO_AudioIn(EarObjectPtr data);

BOOLN	Init_IO_AudioIn(ParameterSpecifier parSpec);

void	NotifyError_IO_AudioIn(char *format, ...);

BOOLN	PrintPars_IO_AudioIn(void);

BOOLN	ReadSignal_IO_AudioIn(EarObjectPtr data);

static int	RecordCallback_IO_AudioIn(const void *inputBuffer,
			  void *outputBuffer, unsigned long framesPerBuffer,
			  const PaStreamCallbackTimeInfo* timeInfo,
			  PaStreamCallbackFlags statusFlags, void *userData);

void	ResetBuffer_IO_AudioIn(void);

BOOLN	SetDeviceID_IO_AudioIn(int theDeviceID);

BOOLN	SetDuration_IO_AudioIn(double theDuration);

BOOLN	SetGain_IO_AudioIn(double theGain);

BOOLN	SetNumChannels_IO_AudioIn(int theNumChannels);

BOOLN	SetParsPointer_IO_AudioIn(ModulePtr theModule);

BOOLN	SetSampleRate_IO_AudioIn(double theSampleRate);

BOOLN	SetSegmentsPerBuffer_IO_AudioIn(int theSegmentsPerBuffer);

BOOLN	SetSleep_IO_AudioIn(double theSleep);

BOOLN	SetUniParList_IO_AudioIn(void);

__END_DECLS

#endif /* USE_PORTAUDIO */
#endif
