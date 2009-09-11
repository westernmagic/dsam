/**********************
 *
 * File:		StMPTone.h
 * Purpose:		This module contains the methods for the multiple pure-tone
 *				signal generation paradigm.
 * Comments:
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		12 Mar 1997
 * Copyright:	(c) 1998, University of Essex
 *
 **********************/

#ifndef	_STMPTONE_H
#define _STMPTONE_H	1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define PURETONE_MULTI_NUM_PARS		6
#define	PURE_TONE_NUM_CHANNELS		1		/* No. of stimulus channels to
											 * initialise. */

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	PURETONE_MULTI_NUMPTONES,
	PURETONE_MULTI_FREQUENCIES,
	PURETONE_MULTI_INTENSITIES,
	PURETONE_MULTI_PHASES,
	PURETONE_MULTI_DURATION,
	PURETONE_MULTI_SAMPLINGINTERVAL

} MPureToneParSpecifier;

/*
 * Note that the pointers do not require flags, as they are set to NULL if
 * they have not been set.  This also applies to integers which are initialised
 * to zero.
 */

typedef struct {

	ParameterSpecifier parSpec;

	int		numPTones;
	Float	duration, dt;
	Float	*intensities, *frequencies, *phases;

	/* Private members */
	UniParListPtr	parList;

} MPureTone, *MPureTonePtr;

/********************************* External Variables *************************/

extern	MPureTonePtr	mPureTonePtr;

/********************************* Function Prototypes ************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	AllocNumPTones_PureTone_Multi(int numPTones);

BOOLN	CheckData_PureTone_Multi(EarObjectPtr data);

BOOLN	Free_PureTone_Multi(void);

BOOLN	GenerateSignal_PureTone_Multi(EarObjectPtr theObject);

Float	GetIndividualFreq_PureTone_Multi(int index);

Float	GetIndividualIntensity_PureTone_Multi(int index);

UniParListPtr	GetUniParListPtr_PureTone_Multi(void);

BOOLN	Init_PureTone_Multi(ParameterSpecifier parSpec);

BOOLN	PrintPars_PureTone_Multi(void);

BOOLN	SetDefaultNumPTonesArrays_PureTone_Multi(void);

BOOLN	SetDuration_PureTone_Multi(Float theDuration);

BOOLN	SetFrequencies_PureTone_Multi(Float *theFrequencies);

BOOLN	SetIndividualFreq_PureTone_Multi(int theIndex, Float theFrequency);

BOOLN	SetIndividualIntensity_PureTone_Multi(int theIndex,
		  Float theIntensity);

BOOLN	SetIndividualPhase_PureTone_Multi(int theIndex, Float thePhase);

BOOLN	SetIntensities_PureTone_Multi(Float *theIntensities);

BOOLN	InitModule_PureTone_Multi(ModulePtr theModule);

BOOLN	SetNumPTones_PureTone_Multi(int theNumPTones);

BOOLN	SetPhases_PureTone_Multi(Float *thePhases);

BOOLN	SetParsPointer_PureTone_Multi(ModulePtr theModule);

BOOLN	SetSamplingInterval_PureTone_Multi(Float theSamplingInterval);

BOOLN	SetUniParList_PureTone_Multi(void);

__END_DECLS

#endif
