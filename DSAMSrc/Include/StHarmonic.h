/**********************
 *
 * File:		StHarmonic.h
 * Purpose:		This module contains the methods for the harmonic series
 *				stimulus.
 * Comments:	This was amended by Almudena to include FM and also a
 *				frequency domain butterworth filter options.
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		12 Mar 1997
 * Copyright:	(c) 1997,  L. P. O'Mard
 *
 **********************/

#ifndef	_STHARMONIC_H
#define _STHARMONIC_H	1

#include "UtNameSpecs.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define HARMONIC_NUM_PARS			16
#define	HARMONIC_NUM_CHANNELS		1		/* No. of stimulus channels to
											 * initialise. */

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	HARMONIC_LOWESTHARMONIC,
	HARMONIC_HIGHESTHARMONIC,
	HARMONIC_PHASEMODE,
	HARMONIC_PHASE_PAR,
	HARMONIC_MISTUNEDHARMONIC,
	HARMONIC_MISTUNINGFACTOR,
	HARMONIC_FREQUENCY,
	HARMONIC_INTENSITY,
	HARMONIC_DURATION,
	HARMONIC_SAMPLINGINTERVAL,
	HARMONIC_MODULATIONFREQUENCY,
	HARMONIC_MODULATIONPHASE,
	HARMONIC_MODULATIONDEPTH,
	HARMONIC_ORDER,
	HARMONIC_LOWERCUTOFFFREQ,
	HARMONIC_UPPERCUTOFFFREQ

} HarmonicParSpecifier;

/*
 * Note that the pointers do not require flags, as they are set to NULL if
 * they have not been set.  This also applies to integers which are initialised
 * to zero.
 */

typedef struct {

	ParameterSpecifier parSpec;

	BOOLN	updateProcessVariablesFlag;
	int		lowestHarmonic;
	int		highestHarmonic;
	int		mistunedHarmonic;
	int		order;
	int		phaseMode;
	Float	phaseVariable;
	Float	mistuningFactor;
	Float	frequency;
	Float	intensity;
	Float	duration, dt;
	Float	modulationFrequency;
	Float	modulationPhase;
	Float	modulationDepth;
	Float	lowerCutOffFreq;
	Float	upperCutOffFreq;

	/* Private process variables */
	UniParListPtr	parList;
	long	ranSeed;
	Float	*phase;
	Float	*harmonicFrequency;
	Float	*modIndex;

} Harmonic, *HarmonicPtr;

/*********************** External Variables ***********************************/

extern	HarmonicPtr	harmonicPtr;

/*********************** Function Prototypes **********************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_Harmonic(EarObjectPtr data);

BOOLN	Free_Harmonic(void);

BOOLN	FreeProcessVariables_Harmonic(void);

BOOLN	GenerateSignal_Harmonic(EarObjectPtr theObject);

UniParListPtr	GetUniParListPtr_Harmonic(void);

BOOLN	SetHighestHarmonic_Harmonic(int theHighestHarmonic);

BOOLN	Init_Harmonic(ParameterSpecifier parSpec);

BOOLN	InitProcessVariables_Harmonic(EarObjectPtr data);

BOOLN	SetLowestHarmonic_Harmonic(int theLowestHarmonic);

BOOLN	SetMistunedHarmonic_Harmonic(int theMistunedHarmonic);

BOOLN	SetMistuningFactor_Harmonic(Float theMistuningFactor);

BOOLN	PrintPars_Harmonic(void);

BOOLN	SetDuration_Harmonic(Float theDuration);

BOOLN	SetFrequency_Harmonic(Float theFrequency);

BOOLN	SetIntensity_Harmonic(Float theIntensity);

BOOLN	SetLowerCutOffFreq_Harmonic(Float theLowerCutOffFreq);

BOOLN	SetModulationDepth_Harmonic(Float theModulationDepth);

BOOLN	SetModulationFrequency_Harmonic(Float theModulationFrequency);

BOOLN	SetModulationPhase_Harmonic(Float theModulationPhase);

BOOLN	InitModule_Harmonic(ModulePtr theModule);

BOOLN	SetOrder_Harmonic(int theOrder);

BOOLN	SetParsPointer_Harmonic(ModulePtr theModule);

BOOLN	SetPhaseMode_Harmonic(WChar *thePhaseMode);

BOOLN	SetPhaseVariable_Harmonic(Float thePhaseVariable);

BOOLN	SetSamplingInterval_Harmonic(Float theSamplingInterval);

BOOLN	SetUniParList_Harmonic(void);

BOOLN	SetUpperCutOffFreq_Harmonic(Float theUpperCutOffFreq);

__END_DECLS

#endif
