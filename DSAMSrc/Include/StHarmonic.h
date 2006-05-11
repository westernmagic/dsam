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
 * Copyright:	(c) 1997,  University of Essex
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

typedef enum {

	HARMONIC_RANDOM,
	HARMONIC_SINE,
	HARMONIC_COSINE,
	HARMONIC_ALTERNATING,
	HARMONIC_SCHROEDER,
	HARMONIC_PLACK_AND_WHITE,
	HARMONIC_USER,
	HARMONIC_NULL

} HarmonicPhaseModeSpecifier;

/*
 * Note that the pointers do not require flags, as they are set to NULL if
 * they have not been set.  This also applies to integers which are initialised
 * to zero.
 */

typedef struct {

	ParameterSpecifier parSpec;

	BOOLN	lowestHarmonicFlag, highestHarmonicFlag, mistunedHarmonicFlag;
	BOOLN	phaseVariableFlag, frequencyFlag, intensityFlag, durationFlag;
	BOOLN	mistuningFactorFlag, modulationFrequencyFlag, modulationPhaseFlag;
	BOOLN	modulationDepthFlag, orderFlag, lowerCutOffFreqFlag, phaseModeFlag;
	BOOLN	upperCutOffFreqFlag, dtFlag;
	BOOLN	updateProcessVariablesFlag;
	int		lowestHarmonic;
	int		highestHarmonic;
	int		mistunedHarmonic;
	int		order;
	int		phaseMode;
	double	phaseVariable;
	double	mistuningFactor;
	double	frequency;
	double	intensity;
	double	duration, dt;
	double	modulationFrequency;
	double	modulationPhase;
	double	modulationDepth;
	double	lowerCutOffFreq;
	double	upperCutOffFreq;
	
	/* Private process variables */
	NameSpecifier *phaseModeList;
	UniParListPtr	parList;
	long	ranSeed;
	double	*phase;
	double	*harmonicFrequency;
	double	*modIndex;

} Harmonic, *HarmonicPtr;

/*********************** External Variables ***********************************/

extern	HarmonicPtr	harmonicPtr;

/*********************** Function Prototypes **********************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckPars_Harmonic(void);

BOOLN	Free_Harmonic(void);

BOOLN	FreeProcessVariables_Harmonic(void);

BOOLN	GenerateSignal_Harmonic(EarObjectPtr theObject);

UniParListPtr	GetUniParListPtr_Harmonic(void);

BOOLN	SetHighestHarmonic_Harmonic(int theHighestHarmonic);

BOOLN	Init_Harmonic(ParameterSpecifier parSpec);

BOOLN	InitPhaseModeList_Harmonic(void);

BOOLN	InitProcessVariables_Harmonic(EarObjectPtr data);

BOOLN	SetLowestHarmonic_Harmonic(int theLowestHarmonic);

BOOLN	SetMistunedHarmonic_Harmonic(int theMistunedHarmonic);

BOOLN	SetMistuningFactor_Harmonic(double theMistuningFactor);

BOOLN	PrintPars_Harmonic(void);

BOOLN	ReadPars_Harmonic(WChar *fileName);

BOOLN	SetDuration_Harmonic(double theDuration);

BOOLN	SetFrequency_Harmonic(double theFrequency);

BOOLN	SetIntensity_Harmonic(double theIntensity);

BOOLN	SetLowerCutOffFreq_Harmonic(double theLowerCutOffFreq);

BOOLN	SetModulationDepth_Harmonic(double theModulationDepth);

BOOLN	SetModulationFrequency_Harmonic(double theModulationFrequency);

BOOLN	SetModulationPhase_Harmonic(double theModulationPhase);

BOOLN	InitModule_Harmonic(ModulePtr theModule);

BOOLN	SetOrder_Harmonic(int theOrder);

BOOLN	SetParsPointer_Harmonic(ModulePtr theModule);

BOOLN	SetPars_Harmonic(WChar *thePhaseMode, int theLowestHarmonic,
		  int theHighestHarmonic, int theMistunedHarmonic,
		  double theMistuningFactor, double thePhaseVariable,
		  double theFrequency, double theIntensity, double theDuration,
		  double theSamplingInterval,  double theModulationFrequency,
		  double theModulationPhase, double theModulationDepth, int theOrder,
		  double theLowerCutFreq, double theUpperCutFreq);

BOOLN	SetPhaseMode_Harmonic(WChar *thePhaseMode);

BOOLN	SetPhaseVariable_Harmonic(double thePhaseVariable);

BOOLN	SetSamplingInterval_Harmonic(double theSamplingInterval);

BOOLN	SetUniParList_Harmonic(void);

BOOLN	SetUpperCutOffFreq_Harmonic(double theUpperCutOffFreq);

__END_DECLS

#endif
