/******************
 *
 * File:		MoANSGSimple.h
 * Purpose:		This module contains the model for the hair cell post-synaptic
 *				firing
 * Comments:	24-02-97 LPO: Amended to allow multiple fibres.
 * Authors:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		24 Feb 1997
 * Copyright:	(c) 1998, University of Essex.
 * 
 ******************/

#ifndef	_MOANSGSIMPLE_H
#define _MOANSGSIMPLE_H	1
 
/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define ANSPIKEGEN_SIMPLE_NUM_PARS	5
#define PS_REFRACTORY_PERIOD		1.0E-3		/* Default value in seconds. */

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	ANSPIKEGEN_SIMPLE_RANSEED,
	ANSPIKEGEN_SIMPLE_NUMFIBRES,
	ANSPIKEGEN_SIMPLE_PULSEDURATION,
	ANSPIKEGEN_SIMPLE_PULSEMAGNITUDE,
	ANSPIKEGEN_SIMPLE_REFRACTORYPERIOD

} SimpleSGParSpecifier;

typedef struct {

	ParameterSpecifier parSpec;
	
 	BOOLN	refractoryPeriodFlag, pulseDurationFlag, pulseMagnitudeFlag;
 	BOOLN	ranSeedFlag, numFibresFlag;
	BOOLN	updateProcessVariablesFlag;
	long	ranSeed;			/* seed for the random number generator. */
	int		numFibres;
	double	pulseDuration;		/* Duration applied to each pulse (s). */
	double	pulseMagnitude;		/* Magnitude for each pulse (nA?). */
	double	refractoryPeriod;	/* The time during which spikes cannot occur */
	

	/* Private members */
	UniParListPtr	parList;
	int		numThreads;
	double	dt;
	double	**timer;
	double	**remainingPulseTime;

} SimpleSG, *SimpleSGPtr;

typedef struct {

	BOOLN	(* CheckPars)(void);
	BOOLN	(* Free)(void);
	BOOLN	(* Init)(ParameterSpecifier parSpec);
	BOOLN	(* PrintPars)(void);
	BOOLN	(* ReadPars)(char *fileName);
	BOOLN	(* RunProcess)(EarObjectPtr data);
	BOOLN	(* SetRanSeed)(long theRanSeed);
	BOOLN	(* SetPulseDuration)(double thePulseDuration);
	BOOLN	(* SetPulseMagnitude)(double thePulseMagnitude);
	BOOLN	(* SetRefractoryPeriod)(double theRefractoryPeriod);

} SpikeGenFunc, *SpikeGenFuncPtr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern	SimpleSGPtr		simpleSGPtr;

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_ANSpikeGen_Simple(EarObjectPtr data);

BOOLN	CheckPars_ANSpikeGen_Simple(void);

BOOLN	Init_ANSpikeGen_Simple(ParameterSpecifier parSpec);

SpikeGenFuncPtr	InitFuncPtr_ANSpikeGen_Simple(void);

BOOLN	Free_ANSpikeGen_Simple(void);

void	FreeProcessVariables_ANSpikeGen_Simple(void);

UniParListPtr	GetUniParListPtr_ANSpikeGen_Simple(void);

BOOLN	InitProcessVariables_ANSpikeGen_Simple(EarObjectPtr data);

BOOLN	PrintPars_ANSpikeGen_Simple(void);

BOOLN	ReadPars_ANSpikeGen_Simple(char *fileName);

BOOLN	RunModel_ANSpikeGen_Simple(EarObjectPtr data);

BOOLN	InitModule_ANSpikeGen_Simple(ModulePtr theModule);

BOOLN	SetNumFibres_ANSpikeGen_Simple(int theNumFibres);

BOOLN	SetParsPointer_ANSpikeGen_Simple(ModulePtr theModule);

BOOLN	SetPars_ANSpikeGen_Simple(long theRanSeed, int numFibres,
		  double pulseDuration, double pulseMagnitude,
		  double theRefractoryPeriod);

BOOLN	SetPulseDuration_ANSpikeGen_Simple(double thePulseDuration);

BOOLN	SetPulseMagnitude_ANSpikeGen_Simple(double thePulseMagnitude);

BOOLN	SetRanSeed_ANSpikeGen_Simple(long theRanSeed);

BOOLN	SetRefractoryPeriod_ANSpikeGen_Simple(double theRefractoryPeriod);

BOOLN	SetUniParList_ANSpikeGen_Simple(void);

__END_DECLS

#endif
