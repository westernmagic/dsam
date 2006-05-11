/**********************
 *
 * File:		StPulseTrain.h
 * Purpose:		The pulse train stimulus generation module.
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 *				21-11-97 LPO: Amended routine so that the first pulse occurrs
 *				in the first sample of the output.
 * Author:		L. P. O'Mard
 * Created:		04 Mar 1996
 * Updated:		21 Nov 1997
 * Copyright:	(c) 1997, University of Essex
 *
 *********************/

#ifndef _STPULSETRAIN_H
#define _STPULSETRAIN_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define PULSETRAIN_NUM_PARS			5

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	PULSETRAIN_PULSERATE,
	PULSETRAIN_PULSEDURATION,
	PULSETRAIN_AMPLITUDE,
	PULSETRAIN_DURATION,
	PULSETRAIN_SAMPLINGINTERVAL

} PulseTrainParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	pulseRateFlag, pulseDurationFlag, amplitudeFlag, durationFlag;
	BOOLN	dtFlag;
	double	pulseRate;
	double	pulseDuration;
	double	amplitude;
	double	duration;
	double	dt;
	
	/* Private members */
	UniParListPtr	parList;
	double	nextPulseTime, remainingPulseTime;

} PulseTrain, *PulseTrainPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	PulseTrainPtr	pulseTrainPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_PulseTrain(EarObjectPtr data);

BOOLN	CheckPars_PulseTrain(void);

BOOLN	Free_PulseTrain(void);

BOOLN	GenerateSignal_PulseTrain(EarObjectPtr data);

UniParListPtr	GetUniParListPtr_PulseTrain(void);

BOOLN	Init_PulseTrain(ParameterSpecifier parSpec);

BOOLN	PrintPars_PulseTrain(void);

BOOLN	ReadPars_PulseTrain(WChar *fileName);

BOOLN	SetAmplitude_PulseTrain(double theAmplitude);

BOOLN	SetDuration_PulseTrain(double theDuration);

BOOLN	InitModule_PulseTrain(ModulePtr theModule);

BOOLN	SetParsPointer_PulseTrain(ModulePtr theModule);

BOOLN	SetPars_PulseTrain(double pulseRate, double pulseDuration,
		  double amplitude, double duration, double samplingInterval);

BOOLN	SetPulseDuration_PulseTrain(double thePulseDuration);

BOOLN	SetPulseRate_PulseTrain(double thePulseRate);

BOOLN	SetSamplingInterval_PulseTrain(double theSamplingInterval);

BOOLN	SetUniParList_PulseTrain(void);

__END_DECLS

#endif
