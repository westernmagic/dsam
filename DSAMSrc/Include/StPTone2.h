/**********************
 *
 * File:		StPTone2.h
 * Purpose:		The module generates a pure-tone signal preceded and ended by
 *				periods of silence.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		12 Mar 1997
 * Copyright:	(c) 1997,  University of Essex
 *
 **********************/

#ifndef	_STPTONE2_H
#define _STPTONE2_H	1
 
/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define PURETONE_2_NUM_PARS			6
#define	PURE_TONE_2_NUM_CHANNELS	1		/* No. of stimulus channels to
											 * initialise. */

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	PURETONE_2_FREQUENCY,
	PURETONE_2_INTENSITY,
	PURETONE_2_DURATION,
	PURETONE_2_SAMPLINGINTERVAL,
	PURETONE_2_BEGINPERIODDURATION,
	PURETONE_2_ENDPERIODDURATION

} PureTone2ParSpecifier;

typedef struct {

	ParameterSpecifier parSpec;

	BOOLN	frequencyFlag, durationFlag, dtFlag;
	BOOLN	intensityFlag, beginPeriodDurationFlag, endPeriodDurationFlag;
	double	frequency, intensity;
	double	duration, dt;
	double	beginPeriodDuration, endPeriodDuration;
	
	/* Private members */
	UniParListPtr	parList;

} PureTone2, *PureTone2Ptr;
	
/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern	PureTone2Ptr	pureTone2Ptr;

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckPars_PureTone_2(void);

BOOLN	Free_PureTone_2(void);

BOOLN	GenerateSignal_PureTone_2(EarObjectPtr theObject);

UniParListPtr	GetUniParListPtr_PureTone_2(void);

BOOLN	Init_PureTone_2(ParameterSpecifier parSpec);

BOOLN	PrintPars_PureTone_2(void);

BOOLN	ReadPars_PureTone_2(char *fileName);

BOOLN	SetBeginPeriodDuration_PureTone_2(double theBeginPeriodDuration);

BOOLN	SetDuration_PureTone_2(double theDuration);

BOOLN	SetEndPeriodDuration_PureTone_2(double theEndPeriodDuration);

BOOLN	SetFrequency_PureTone_2(double theFrequency);

BOOLN	SetIntensity_PureTone_2(double theIntensity);

BOOLN	InitModule_PureTone_2(ModulePtr theModule);

BOOLN	SetParsPointer_PureTone_2(ModulePtr theModule);

BOOLN	SetPars_PureTone_2(double theFrequency, double theIntensity,
		  double theDuration, double theSamplingInterval,
		  double theBeginPeriodDuration, double theEndPeriodDuration);
		  
BOOLN	SetSamplingInterval_PureTone_2(double theSamplingInterval);

BOOLN	SetUniParList_PureTone_2(void);

__END_DECLS

#endif
