/**********************
 *
 * File:		IOAudioIn.c
 * Purpose:		Audio input from sound system.
 * Comments:	Written using ModuleProducer version 1.4.2 (Dec 19 2003).
 * Author:		L. P. O'Mard
 * Created:		08 Nov 2005
 * Updated:	
 * Copyright:	(c) 2005, CNBH, University of Essex.
 *
 *********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG */

#ifdef USE_PORTAUDIO

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "FiParFile.h"
#include "IOAudioIn.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

AudioInPtr	audioInPtr = NULL;

/******************************************************************************/
/****************************** Subroutines and functions *********************/
/******************************************************************************/

/****************************** Free ******************************************/

/*
 * This function releases of the memory allocated for the process
 * variables. It should be called when the module is no longer in use.
 * It is defined as returning a BOOLN value because the generic
 * module interface requires that a non-void value be returned.
 */

BOOLN
Free_IO_AudioIn(void)
{
	/* static const char	*funcName = "Free_IO_AudioIn";  */

	if (audioInPtr == NULL)
		return(FALSE);
	FreeProcessVariables_IO_AudioIn();
	if (audioInPtr->parList)
		FreeList_UniParMgr(&audioInPtr->parList);
	if (audioInPtr->parSpec == GLOBAL) {
		free(audioInPtr);
		audioInPtr = NULL;
	}
	return(TRUE);

}

/****************************** Init ******************************************/

/*
 * This function initialises the module by setting module's parameter
 * pointer structure.
 * The GLOBAL option is for hard programming - it sets the module's
 * pointer to the global parameter structure and initialises the
 * parameters. The LOCAL option is for generic programming - it
 * initialises the parameter structure currently pointed to by the
 * module's parameter pointer.
 */

BOOLN
Init_IO_AudioIn(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_IO_AudioIn";

	if (parSpec == GLOBAL) {
		if (audioInPtr != NULL)
			Free_IO_AudioIn();
		if ((audioInPtr = (AudioInPtr) malloc(sizeof(AudioIn))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (audioInPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	audioInPtr->parSpec = parSpec;
	audioInPtr->updateProcessVariablesFlag = TRUE;
	audioInPtr->deviceIDFlag = TRUE;
	audioInPtr->numChannelsFlag = TRUE;
	audioInPtr->sampleRateFlag = TRUE;
	audioInPtr->durationFlag = TRUE;
	audioInPtr->segmentsPerBufferFlag = TRUE;
	audioInPtr->sleepFlag = TRUE;
	audioInPtr->gainFlag = TRUE;
	audioInPtr->deviceID = -1;
	audioInPtr->numChannels = 1;
	audioInPtr->sampleRate = 44100.0;
	audioInPtr->duration = 10e-3;
	audioInPtr->segmentsPerBuffer = 10;
	audioInPtr->sleep = 0.0;
	audioInPtr->gain = 0.0;

	if (!SetUniParList_IO_AudioIn()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_IO_AudioIn();
		return(FALSE);
	}
	audioInPtr->portAudioInitialised = FALSE;
	audioInPtr->pAError = paNoError;
	audioInPtr->stream = NULL;
	audioInPtr->buffer = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_IO_AudioIn(void)
{
	static const char *funcName = "SetUniParList_IO_AudioIn";
	UniParPtr	pars;

	if ((audioInPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  IO_AUDIOIN_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = audioInPtr->parList->pars;
	SetPar_UniParMgr(&pars[IO_AUDIOIN_DEVICEID], "DEVICE_ID",
	  "Device ID/number (int).",
	  UNIPAR_INT,
	  &audioInPtr->deviceID, NULL,
	  (void * (*)) SetDeviceID_IO_AudioIn);
	SetPar_UniParMgr(&pars[IO_AUDIOIN_NUMCHANNELS], "NUM_CHANNELS",
	  "Number of input channels, i.e. 1 = mono, 2 = stereo (int.)",
	  UNIPAR_INT,
	  &audioInPtr->numChannels, NULL,
	  (void * (*)) SetNumChannels_IO_AudioIn);
	SetPar_UniParMgr(&pars[IO_AUDIOIN_SEGMENTSPERBUFFER], "SEGMENTS_PER_BUFFER",
	  "Main input buffer length: multiples of the (segment) duration (int).",
	  UNIPAR_INT,
	  &audioInPtr->segmentsPerBuffer, NULL,
	  (void * (*)) SetSegmentsPerBuffer_IO_AudioIn);
	SetPar_UniParMgr(&pars[IO_AUDIOIN_SAMPLERATE], "SAMPLE_RATE",
	  "Input sample rate = 1 / sampling interval (Hz).",
	  UNIPAR_REAL,
	  &audioInPtr->sampleRate, NULL,
	  (void * (*)) SetSampleRate_IO_AudioIn);
	SetPar_UniParMgr(&pars[IO_AUDIOIN_DURATION], "DURATION",
	  "Input duration or segment duration (s).",
	  UNIPAR_REAL,
	  &audioInPtr->duration, NULL,
	  (void * (*)) SetDuration_IO_AudioIn);
	SetPar_UniParMgr(&pars[IO_AUDIOIN_SLEEP], "SLEEP",
	  "Sleep time between process reads (ms).",
	  UNIPAR_REAL,
	  &audioInPtr->sleep, NULL,
	  (void * (*)) SetSleep_IO_AudioIn);
	SetPar_UniParMgr(&pars[IO_AUDIOIN_GAIN], "GAIN",
	  "Signal gain (dB).",
	  UNIPAR_REAL,
	  &audioInPtr->gain, NULL,
	  (void * (*)) SetGain_IO_AudioIn);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_IO_AudioIn(void)
{
	static const char	*funcName = "GetUniParListPtr_IO_AudioIn";

	if (audioInPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (audioInPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(audioInPtr->parList);

}

/****************************** SetDeviceID ***********************************/

/*
 * This function sets the module's deviceID parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDeviceID_IO_AudioIn(int theDeviceID)
{
	static const char	*funcName = "SetDeviceID_IO_AudioIn";

	if (audioInPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	audioInPtr->deviceIDFlag = TRUE;
	audioInPtr->updateProcessVariablesFlag = TRUE;
	audioInPtr->deviceID = theDeviceID;
	return(TRUE);

}

/****************************** SetNumChannels ********************************/

/*
 * This function sets the module's numChannels parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNumChannels_IO_AudioIn(int theNumChannels)
{
	static const char	*funcName = "SetNumChannels_IO_AudioIn";

	if (audioInPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((theNumChannels < 1 ) || (theNumChannels > 2)) {
		NotifyError("%s: The number of channels must be 1 or 2.", funcName);
		return(FALSE);
	}
	audioInPtr->numChannelsFlag = TRUE;
	audioInPtr->updateProcessVariablesFlag = TRUE;
	audioInPtr->numChannels = theNumChannels;
	return(TRUE);

}

/****************************** SetSampleRate *********************************/

/*
 * This function sets the module's sampleRate parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSampleRate_IO_AudioIn(double theSampleRate)
{
	static const char	*funcName = "SetSampleRate_IO_AudioIn";

	if (audioInPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	audioInPtr->sampleRateFlag = TRUE;
	audioInPtr->updateProcessVariablesFlag = TRUE;
	audioInPtr->sampleRate = theSampleRate;
	return(TRUE);

}

/****************************** SetDuration ***********************************/

/*
 * This function sets the module's duration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDuration_IO_AudioIn(double theDuration)
{
	static const char	*funcName = "SetDuration_IO_AudioIn";

	if (audioInPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	audioInPtr->durationFlag = TRUE;
	audioInPtr->updateProcessVariablesFlag = TRUE;
	audioInPtr->duration = theDuration;
	return(TRUE);

}

/****************************** SetSegmentsPerBuffer **************************/

/*
 * This function sets the module's segmentsPerBuffer parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSegmentsPerBuffer_IO_AudioIn(int theSegmentsPerBuffer)
{
	static const char	*funcName = "SetSegmentsPerBuffer_IO_AudioIn";

	if (audioInPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	audioInPtr->segmentsPerBufferFlag = TRUE;
	audioInPtr->updateProcessVariablesFlag = TRUE;
	audioInPtr->segmentsPerBuffer = theSegmentsPerBuffer;
	return(TRUE);

}

/****************************** SetSleep **************************************/

/*
 * This function sets the module's sleep parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSleep_IO_AudioIn(double theSleep)
{
	static const char	*funcName = "SetSleep_IO_AudioIn";

	if (audioInPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	audioInPtr->sleepFlag = TRUE;
	audioInPtr->updateProcessVariablesFlag = TRUE;
	audioInPtr->sleep = theSleep;
	return(TRUE);

}

/****************************** SetGain ***************************************/

/*
 * This function sets the module's gain parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetGain_IO_AudioIn(double theGain)
{
	static const char	*funcName = "SetGain_IO_AudioIn";

	if (audioInPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	audioInPtr->gainFlag = TRUE;
	audioInPtr->updateProcessVariablesFlag = TRUE;
	audioInPtr->gain = theGain;
	return(TRUE);

}

/****************************** CheckPars *************************************/

/*
 * This routine checks that the necessary parameters for the module
 * have been correctly initialised.
 * Other 'operational' tests which can only be done when all
 * parameters are present, should also be carried out here.
 * It returns TRUE if there are no problems.
 */

BOOLN
CheckPars_IO_AudioIn(void)
{
	static const char	*funcName = "CheckPars_IO_AudioIn";
	BOOLN	ok;

	ok = TRUE;
	if (audioInPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!audioInPtr->deviceIDFlag) {
		NotifyError("%s: deviceID variable not set.", funcName);
		ok = FALSE;
	}
	if (!audioInPtr->numChannelsFlag) {
		NotifyError("%s: numChannels variable not set.", funcName);
		ok = FALSE;
	}
	if (!audioInPtr->sampleRateFlag) {
		NotifyError("%s: sampleRate variable not set.", funcName);
		ok = FALSE;
	}
	if (!audioInPtr->durationFlag) {
		NotifyError("%s: duration variable not set.", funcName);
		ok = FALSE;
	}
	if (!audioInPtr->segmentsPerBufferFlag) {
		NotifyError("%s: segmentsPerBuffer variable not set.", funcName);
		ok = FALSE;
	}
	if (!audioInPtr->sleepFlag) {
		NotifyError("%s: sleep variable not set.", funcName);
		ok = FALSE;
	}
	if (!audioInPtr->gainFlag) {
		NotifyError("%s: gain variable not set.", funcName);
		ok = FALSE;
	}
	return(ok);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the lUTEar.parsFile file pointer.
 */

BOOLN
PrintPars_IO_AudioIn(void)
{
	static const char	*funcName = "PrintPars_IO_AudioIn";

	if (!CheckPars_IO_AudioIn()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Audio Module Parameters:-\n");
	DPrint("\tdeviceID = %d ??\n", audioInPtr->deviceID);
	DPrint("\tnumChannels = %d ??\n", audioInPtr->numChannels);
	DPrint("\tsegmentsPerBuffer = %g ??\n", audioInPtr->segmentsPerBuffer);
	DPrint("\tsampleRate = %g ??\n", audioInPtr->sampleRate);
	DPrint("\tduration = %g ??\n", audioInPtr->duration);
	DPrint("\tsleep = %g ??\n", audioInPtr->sleep);
	DPrint("\tgain = %g ??\n", audioInPtr->gain);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_IO_AudioIn(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_IO_AudioIn";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	audioInPtr = (AudioInPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 * It also initialises the process structure.
 */

BOOLN
InitModule_IO_AudioIn(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_IO_AudioIn";

	if (!SetParsPointer_IO_AudioIn(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_IO_AudioIn(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = audioInPtr;
	theModule->CheckPars = CheckPars_IO_AudioIn;
	theModule->Free = Free_IO_AudioIn;
	theModule->GetUniParListPtr = GetUniParListPtr_IO_AudioIn;
	theModule->PrintPars = PrintPars_IO_AudioIn;
	theModule->RunProcess = ReadSignal_IO_AudioIn;
	theModule->SetParsPointer = SetParsPointer_IO_AudioIn;
	return(TRUE);

}

/****************************** CheckData *************************************/

/*
 * This routine checks that the 'data' EarObject and input signal are
 * correctly initialised.
 * It should also include checks that ensure that the module's
 * parameters are compatible with the signal parameters, i.e. dt is
 * not too small, etc...
 * The 'CheckRamp_SignalData()' can be used instead of the
 * 'CheckInit_SignalData()' routine if the signal must be ramped for
 * the process.
 */

BOOLN
CheckData_IO_AudioIn(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_IO_AudioIn";

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	/*** Put additional checks here. ***/
	return(TRUE);

}

/****************************** NotifyError ***********************************/

/*
 * This routine returns PortAudio specific error messages.
 */

void
NotifyError_IO_AudioIn(char *format, ...)
{
	char	msg[LONG_STRING];
	va_list	args;
	
	va_start(args, format);
	vsnprintf(msg, LONG_STRING, format, args);
	va_end(args);
	NotifyError("%s (PortAudioError [%d]: %s)\n", msg, audioInPtr->pAError,
	  Pa_GetErrorText(audioInPtr->pAError));

}

/****************************** ResetBuffer ***********************************/

/*
 * This routine will be called by the PortAudio engine when audio is needed.
 * It may be called at interrupt level on some machines so don't do anything
 * that could mess up the system like calling malloc() or free().
 */

void
ResetBuffer_IO_AudioIn(void)
{
	AudioInPtr p = audioInPtr;

	p->frameIndex = 0;
	p->segmentIndex = 0;

}

/****************************** RecordCallback ********************************/

/*
 * This routine will be called by the PortAudio engine when audio is needed.
 * It may be called at interrupt level on some machines so don't do anything
 * that could mess up the system like calling malloc() or free().
 */

static int
RecordCallback_IO_AudioIn(const void *inputBuffer, void *outputBuffer,
  unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
  PaStreamCallbackFlags statusFlags, void *userData)
{
	static const char *funcName = "RecordCallback_IO_AudioIn";
	long	i, framesToCalc;

	AudioInPtr p = (AudioInPtr) userData;
	const AudioInSample	*inPtr = (const AudioInSample *) inputBuffer;
	ChanData	*outPtr[2];
	SignalDataPtr	bufSignal = p->buffer->outSignal;
    unsigned long framesLeft = bufSignal->length - p->frameIndex;

    framesToCalc = (framesLeft < framesPerBuffer)? framesLeft: framesPerBuffer;
	outPtr[LEFT_CHAN] = bufSignal->channel[LEFT_CHAN] + p->frameIndex;
	if (bufSignal->numChannels == 2)
		outPtr[RIGHT_CHAN] = bufSignal->channel[RIGHT_CHAN] + p->frameIndex;
	if (inputBuffer) {
		for (i = 0; i < framesToCalc; i++) {
			*outPtr[LEFT_CHAN]++ = (ChanData) *inPtr++;
			if (bufSignal->numChannels == 2)
				*outPtr[RIGHT_CHAN]++ = (ChanData) *inPtr++;
		}
	} else {
		for (i = 0; i < framesToCalc; i++) {
			*outPtr[LEFT_CHAN]++ = (ChanData) IO_AUDIOIN_SAMPLE_SILENCE;
			if (bufSignal->numChannels == 2)
				*outPtr[RIGHT_CHAN]++ = (ChanData) IO_AUDIOIN_SAMPLE_SILENCE;
		}
	}
	printf("RecordCallback_IO_AudioIn: Debug: frameIndex = %ld, %ld\n",
	  p->frameIndex, p->segmentIndex);
	p->frameIndex += framesToCalc;
	if ((ChanLen) (p->frameIndex - p->segmentIndex) >= p->data->outSignal->
	  length) {
		/*printf("RecordCallback_IO_AudioIn: Debug: Got segment, length = %d"
		  ".\n", p->frameIndex - p->segmentIndex);*/
		p->segmentReadyFlag = TRUE;
	}
	/*if (p->frameIndex <= p->segmentIndex) {
		NotifyError("RecordCallback_IO_AudioIn: Debug: Buffer pointers "
		  "colliding.\n");
		return(paComplete);
	}*/
	if ((ChanLen) p->frameIndex >= bufSignal->length)
		p->frameIndex = 0;
	return(paContinue);

}


/****************************** InitProcessVariables **************************/

/*
 * This function allocates the memory for the process variables.
 * It assumes that all of the parameters for the module have been
 * correctly initialised.
 */

BOOLN
InitProcessVariables_IO_AudioIn(EarObjectPtr data)
{
	static const char	*funcName = "InitProcessVariables_IO_AudioIn";
	PaStreamParameters  inputParameters;
	AudioInPtr p = audioInPtr;
  
	if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
		FreeProcessVariables_IO_AudioIn();
		if ((p->buffer = Init_EarObject("NULL")) == NULL) {
			NotifyError("%s: Could not initialise previous data EarObject",
			  funcName);
			return(FALSE);
		}
		if (!InitOutSignal_EarObject(p->buffer, data->outSignal->numChannels,
		  data->outSignal->length * p->segmentsPerBuffer, data->outSignal->
		  dt)) {
			NotifyError("%s: Cannot initialise channels for previous data.",
			  funcName);
			return(FALSE);
		}
		p->data = data;
		if ((p->pAError = Pa_Initialize()) != paNoError) {
			NotifyError_IO_AudioIn("%s: Could not initialise PortAudio",
			  funcName);
			return(FALSE);
		}
		p->portAudioInitialised = TRUE;
		inputParameters.device = (p->deviceID < 0)? Pa_GetDefaultInputDevice():
		  p->deviceID;
		inputParameters.channelCount = p->numChannels;
		inputParameters.sampleFormat = IO_AUDIOIN_SAMPLE_FORMAT;
		inputParameters.suggestedLatency = Pa_GetDeviceInfo(
		  inputParameters.device )->defaultLowInputLatency;
		inputParameters.hostApiSpecificStreamInfo = NULL;
		if ((p->pAError = Pa_OpenStream(&p->stream, &inputParameters, NULL,
		  p->sampleRate, paFramesPerBufferUnspecified, paClipOff,
		  RecordCallback_IO_AudioIn, p)) != paNoError) {
			NotifyError_IO_AudioIn("%s: Could not open stream", funcName);
			return(FALSE);
		}
		p->frameIndex = 0;
		p->segmentIndex = 0;
		if ((p->pAError = Pa_StartStream(p->stream)) != paNoError) {
			NotifyError_IO_AudioIn("%s: Could not start PortAudio stream",
			  funcName);
			return(FALSE);
		}
		p->updateProcessVariablesFlag = FALSE;
	}
	if (data->timeIndex == PROCESS_START_TIME) {
		p->segmentReadyFlag = FALSE;
	}
	return(TRUE);

}

/****************************** FreeProcessVariables **************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

BOOLN
FreeProcessVariables_IO_AudioIn(void)
{
	static const char	*funcName = "FreeProcessVariables_IO_AudioIn";
	AudioInPtr p = audioInPtr;

	Free_EarObject(&p->buffer);
	if (p->stream) {
		if ((p->pAError = Pa_CloseStream(p->stream)) != paNoError) {
			NotifyError_IO_AudioIn("%s: Could not close PortAudio stream",
			  funcName);
		}
	}
	if (p->portAudioInitialised)
		Pa_Terminate();
	return(TRUE);

}

/****************************** ReadSignal ************************************/

/*
 * This routine allocates memory for the output signal, if necessary,
 * and generates the signal into channel[0] of the signal data-set.
 * It checks that all initialisation has been correctly carried out by
 * calling the appropriate checking routines.
 * It can be called repeatedly with different parameter values if
 * required.
 * Stimulus generation only sets the output signal, the input signal
 * is not used.
 * With repeated calls the Signal memory is only allocated once, then
 * re-used.
 */

BOOLN
ReadSignal_IO_AudioIn(EarObjectPtr data)
{
	static const char	*funcName = "ReadSignal_IO_AudioIn";
	BOOLN	ok = TRUE;
	AudioInPtr p = audioInPtr;

	if (!data->threadRunFlag) {
		if (!CheckPars_IO_AudioIn())
			return(FALSE);
		if (!CheckData_IO_AudioIn(data)) {
			NotifyError("%s: Process data invalid.", funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, "Audio input module process");
		data->externalDataFlag = TRUE;
		if (!InitOutSignal_EarObject(data, (uShort) p->numChannels, (ChanLen)
		  floor(p->duration * p->sampleRate + 0.5), 1.0 / p->sampleRate)) {
			NotifyError("%s: Cannot initialise output channels.", funcName);
			return(FALSE);
		}
		data->outSignal->rampFlag = TRUE;
		if (!InitProcessVariables_IO_AudioIn(data)) {
			NotifyError("%s: Could not initialise the process variables.",
			  funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	do {
		p->pAError = Pa_IsStreamActive(p->stream);
		if (p->sleep > 0)
			Pa_Sleep((int) p->sleep);
	} while (!p->segmentReadyFlag && (p->pAError == 1));
	data->outSignal->channel[LEFT_CHAN] = p->buffer->outSignal->channel[
	  LEFT_CHAN] + p->segmentIndex;
	if (p->buffer->outSignal->numChannels == 2)
		data->outSignal->channel[RIGHT_CHAN] = p->buffer->outSignal->channel[
		  RIGHT_CHAN] + p->segmentIndex;
	p->segmentIndex += data->outSignal->length;
	if ((ChanLen) p->segmentIndex > p->buffer->outSignal->length)
		p->segmentIndex = 0;
	p->segmentReadyFlag = FALSE;
	if ((p->pAError < 0) && !GetDSAMPtr_Common()->segmentedMode)
		NotifyError_IO_AudioIn("%s: Failed to record sound.", funcName);
	if (p->pAError < 1)
		ok = FALSE;
	if (ok && (fabs(p->gain) > DBL_EPSILON))
		GaindB_SignalData(data->outSignal, p->gain);
	SetProcessContinuity_EarObject(data);
	return(ok);

}

#endif /* USE_PORTAUDIO */
