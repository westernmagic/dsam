/**********************
 *
 * File:		StMPPTone.h
 * Purpose:		The module generates a signal which contains multiple puretone
 *				pulses at different frequencies.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		18 Feb 1993
 * Updated:		12 Mar 1997
 * Copyright:	(c) 1997, University of Essex
 *
 *********************/

#ifndef	_STMPPTONE_H
#define _STMPPTONE_H	1
 
/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define PURETONE_MULTIPULSE_NUM_PARS		8
#define	PURE_TONE_3_NUM_CHANNELS	1		/* No. of stimulus channels to
											 * initialise. */

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	PURETONE_MULTIPULSE_NUMPULSES,
	PURETONE_MULTIPULSE_FREQUENCIES,
	PURETONE_MULTIPULSE_INTENSITY,
	PURETONE_MULTIPULSE_BEGINPERIODDURATION,
	PURETONE_MULTIPULSE_PULSEDURATION,
	PURETONE_MULTIPULSE_REPETITIONPERIOD,
	PURETONE_MULTIPULSE_DURATION,
	PURETONE_MULTIPULSE_SAMPLINGINTERVAL

} PureTone4ParSpecifier;

typedef struct {

	ParameterSpecifier parSpec;

	BOOLN	intensityFlag, durationFlag, dtFlag;
	BOOLN	beginPeriodDurationFlag, pulseDurationFlag;
	BOOLN	repetitionPeriodFlag, numPulsesFlag;
	int		numPulses;
	double	intensity, duration, dt;
	double	beginPeriodDuration, pulseDuration, repetitionPeriod;
	double	*frequencies;
	
	/* Private members */
	UniParListPtr	parList;
	BOOLN	pulseOn;
	int		pulseNumber;
	ChanLen	pulseCount, beginIndex;
	
} PureTone4, *PureTone4Ptr;
	
/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern	PureTone4Ptr	pureTone4Ptr;

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	AllocNumPulses_PureTone_MultiPulse(int numPulses);

BOOLN	CheckPars_PureTone_MultiPulse(void);

BOOLN	Free_PureTone_MultiPulse(void);

BOOLN	GenerateSignal_PureTone_MultiPulse(EarObjectPtr theObject);

double	GetIndividualFreq_PureTone_MultiPulse(int index);

UniParListPtr	GetUniParListPtr_PureTone_MultiPulse(void);

BOOLN	Init_PureTone_MultiPulse(ParameterSpecifier parSpec);

BOOLN	PrintPars_PureTone_MultiPulse(void);

BOOLN	ReadPars_PureTone_MultiPulse(char *fileName);

BOOLN	SetBeginPeriodDuration_PureTone_MultiPulse(
		  double theBeginPeriodDuration);

BOOLN	SetDefaultNumPulsesArrays_PureTone_MultiPulse(void);

BOOLN	SetDuration_PureTone_MultiPulse(double theDuration);

BOOLN	SetIntensity_PureTone_MultiPulse(double theIntensity);

BOOLN	SetIndividualFreq_PureTone_MultiPulse(int theIndex, double theFreq);
		  
BOOLN	SetFrequencies_PureTone_MultiPulse(double *theFrequencies);

BOOLN	InitModule_PureTone_MultiPulse(ModulePtr theModule);

BOOLN	SetNumPulses_PureTone_MultiPulse(int theNumPulses);

BOOLN	SetParsPointer_PureTone_MultiPulse(ModulePtr theModule);

BOOLN	SetPars_PureTone_MultiPulse(int numPulses, double *frequencies,
		  double intensity, double beginPeriodDuration, double pulseDuration,
		  double repetitionPeriod, double duration, double samplingInterval);

		  
BOOLN	SetPulseDuration_PureTone_MultiPulse(double thePulseDuration);

BOOLN	SetRepetitionPeriod_PureTone_MultiPulse(double theRepetitionPeriod);

BOOLN	SetSamplingInterval_PureTone_MultiPulse(double theSamplingInterval);

BOOLN	SetUniParList_PureTone_MultiPulse(void);

__END_DECLS

#endif
