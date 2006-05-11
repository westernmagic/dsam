/**********************
 *
 * File:		StStepFun.h
 * Purpose:		This module contains the step function stimulus generation
 *				paradigm.
 * Comments:	The module generates a constant signal preceded and ended by
 *				periods of of specified amplitude.
 *				The magnitude of the step function has no units, i.e. they
 *				must be implicitly defined by the user (Arbitrary units)
 *				09-11-98 LPO: Changed parameter names 'amplitude' ->
 *				'amplitude' and 'duration' -> 'duration'.
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		09 Nov 1998
 * Copyright:	(c) 1998, University of Essex
 *
 **********************/

#ifndef	_STSTEPFUN_H
#define _STSTEPFUN_H	1
 
/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define STEPFUNCTION_NUM_PARS			6

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	STEPFUNCTION_AMPLITUDE,
	STEPFUNCTION_BEGINENDAMPLITUDE,
	STEPFUNCTION_BEGINPERIODDURATION,
	STEPFUNCTION_ENDPERIODDURATION,
	STEPFUNCTION_DURATION,
	STEPFUNCTION_SAMPLINGINTERVAL

} StepFunParSpecifier;

typedef struct {

	ParameterSpecifier parSpec;

	BOOLN	durationFlag, dtFlag;
	BOOLN	amplitudeFlag, beginPeriodDurationFlag, endPeriodDurationFlag;
	BOOLN	beginEndAmplitudeFlag;
	double	amplitude;		/* Arbitary units. */
	double	duration, dt;
	double	beginPeriodDuration, endPeriodDuration, beginEndAmplitude;
	
	/* Private members */
	UniParListPtr	parList;

} StepFun, *StepFunPtr;
	
/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern	StepFunPtr	stepFunPtr;

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckPars_StepFunction(void);

BOOLN	Free_StepFunction(void);

BOOLN	GenerateSignal_StepFunction(EarObjectPtr theObject);

UniParListPtr	GetUniParListPtr_StepFunction(void);

BOOLN	Init_StepFunction(ParameterSpecifier parSpec);

BOOLN	PrintPars_StepFunction(void);

BOOLN	ReadPars_StepFunction(WChar *fileName);

BOOLN	SetBeginPeriodDuration_StepFunction(double theBeginPeriodDuration);

BOOLN	SetBeginEndAmplitude_StepFunction(double theBeginEndAmplitude);

BOOLN	SetEndPeriodDuration_StepFunction(double theEndPeriodDuration);

BOOLN	InitModule_StepFunction(ModulePtr theModule);

BOOLN	SetParsPointer_StepFunction(ModulePtr theModule);

BOOLN	SetPars_StepFunction(double theAmplitude, double theDuration,
		  double theSamplingInterval, double theBeginPeriodDuration,
		  double theEndPeriodDuration, double theBeginEndAmplitude);

BOOLN	SetSamplingInterval_StepFunction(double theSamplingInterval);

BOOLN	SetAmplitude_StepFunction(double theAmplitude);

BOOLN	SetDuration_StepFunction(double theDuration);
		  
BOOLN	SetUniParList_StepFunction(void);

__END_DECLS

#endif
