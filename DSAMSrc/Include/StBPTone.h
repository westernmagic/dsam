/**********************
 *
 * File:		StBPTone.h
 * Purpose:		This module contains the methods for the multiple pure-tone
 *				signal generation paradigm.
 * Comments:	Written using ModuleProducer version 1.2.0 (Nov  6 1998).
 *				06-11-98 LPO: This version of the module was created to make it
 *				easier to create the universal parameter lists.  This is because
 *				the arrays are assumed to have a length of two, rather than
 *				there being a parameter which defined the lengths.
 * Author:		L. P. O'Mard
 * Created:		06 Nov 1998
 * Updated:
 * Copyright:	(c) 1998, University of Essex.
 *
 *********************/

#ifndef _STBPTONE_H
#define _STBPTONE_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define PURETONE_Binaural_NUM_PARS			7
#define BINAURAL_PTONE_CHANNELS				2

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	PURETONE_Binaural_LEFTFREQUENCY,
	PURETONE_Binaural_RIGHTFREQUENCY,
	PURETONE_Binaural_LEFTINTENSITY,
	PURETONE_Binaural_RIGHTINTENSITY,
	PURETONE_Binaural_PHASEDIFFERENCE,
	PURETONE_Binaural_DURATION,
	PURETONE_Binaural_SAMPLINGINTERVAL

} BPureToneParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	Float	leftFrequency;
	Float	rightFrequency;
	Float	leftIntensity;
	Float	rightIntensity;
	Float	phaseDifference;
	Float	duration;
	Float	dt;

	/* Private members */
	UniParListPtr	parList;

} BPureTone, *BPureTonePtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	BPureTonePtr	bPureTonePtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_PureTone_Binaural(EarObjectPtr data);

BOOLN	Free_PureTone_Binaural(void);

BOOLN	GenerateSignal_PureTone_Binaural(EarObjectPtr data);

UniParListPtr	GetUniParListPtr_PureTone_Binaural(void);

BOOLN	Init_PureTone_Binaural(ParameterSpecifier parSpec);

BOOLN	PrintPars_PureTone_Binaural(void);

BOOLN	SetDuration_PureTone_Binaural(Float theDuration);

BOOLN	SetLeftFrequency_PureTone_Binaural(Float theLeftFrequency);

BOOLN	SetLeftIntensity_PureTone_Binaural(Float theLeftIntensity);

BOOLN	InitModule_PureTone_Binaural(ModulePtr theModule);

BOOLN	SetParsPointer_PureTone_Binaural(ModulePtr theModule);

BOOLN	SetPhaseDifference_PureTone_Binaural(Float thePhaseDifference);

BOOLN	SetRightFrequency_PureTone_Binaural(Float theRightFrequency);

BOOLN	SetRightIntensity_PureTone_Binaural(Float theRightIntensity);

BOOLN	SetSamplingInterval_PureTone_Binaural(Float theSamplingInterval);

BOOLN	SetUniParList_PureTone_Binaural(void);

__END_DECLS

#endif
