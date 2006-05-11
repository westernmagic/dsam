/**********************
 *
 * File:		StPTone.h
 * Purpose:		This module contains the methods for the simple pure-tone
 *				signal generation paradigm.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		12 Mar 1997
 * Copyright:	(c) 1997,  University of Essex
 *
 **********************/

#ifndef	_STPTONE_H
#define _STPTONE_H	1
 
/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define PURETONE_NUM_PARS			4
#define	PURETONE_NUM_CHANNELS		1	/* No. of channels to initialise. */

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	PURETONE_FREQUENCY,
	PURETONE_INTENSITY,
	PURETONE_DURATION,
	PURETONE_SAMPLINGINTERVAL

} PureToneParSpecifier;

typedef struct {

	ParameterSpecifier parSpec;

	BOOLN	frequencyFlag, durationFlag, dtFlag;
	BOOLN	intensityFlag;
	double	frequency, intensity;
	double	duration, dt;

	/* Private members */
	UniParListPtr	parList;

} PureTone, *PureTonePtr;
	
/*********************** External Variables ***********************************/

extern	PureTonePtr	pureTonePtr;

/*********************** Function Prototypes **********************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckPars_PureTone(void);

BOOLN	Free_PureTone(void);

BOOLN	GenerateSignal_PureTone(EarObjectPtr theObject);

UniParListPtr	GetUniParListPtr_PureTone(void);

BOOLN	Init_PureTone(ParameterSpecifier parSpec);

BOOLN	PrintPars_PureTone(void);

BOOLN	ReadPars_PureTone(WChar *fileName);

BOOLN	SetDuration_PureTone(double theDuration);

BOOLN	SetFrequency_PureTone(double theFrequency);

BOOLN	SetIntensity_PureTone(double theIntensity);

BOOLN	InitModule_PureTone(ModulePtr theModule);

BOOLN	SetParsPointer_PureTone(ModulePtr theModule);

BOOLN	SetPars_PureTone(double theFrequency, double theIntensity,
		  double theDuration, double theSamplingInterval);
		  
BOOLN	SetSamplingInterval_PureTone(double theSamplingInterval);

BOOLN	SetUniParList_PureTone(void);

__END_DECLS

#endif
