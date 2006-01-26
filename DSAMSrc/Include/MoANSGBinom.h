/**********************
 *
 * File:		MoANSGBinom.h
 * Purpose:		Uses spike probabilities to produce the output from a number of
 *				fibres, using a binomial approximation.
 * Comments:	Written using ModuleProducer version 1.9 (May 27 1996).
 *				This is an approximation, so it should only be used when theree
 *				are a large number of fibres.  If in doubt, test against the
 *				output from the "Simple" spike generation module.
 *				It uses the refractory adjustment model module.
 * Author:		L. P. O'Mard, revised from code by M. J. Hewitt
 * Created:		25 Jun 1996
 * Updated:		15 Sep 1996
 * Copyright:	(c) University of Essex
 *
 *********************/

#ifndef _MOANSGBINOM_H
#define _MOANSGBINOM_H 1

#include "UtRandom.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define ANSPIKEGEN_BINOM_NUM_PARS			5
#define ANSPIKEGEN_BINOM_NUM_SUB_PROCESSES	1

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	ANSPIKEGEN_REFRACTADJDATA

} AnSpikeGenBinomSubProcessSpecifier;

typedef enum {

	ANSPIKEGEN_BINOM_NUMFIBRES,
	ANSPIKEGEN_BINOM_RANSEED,
	ANSPIKEGEN_BINOM_PULSEDURATION,
	ANSPIKEGEN_BINOM_PULSEMAGNITUDE,
	ANSPIKEGEN_BINOM_REFRACTORYPERIOD

} BinomialSGParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	numFibresFlag, ranSeedFlag, pulseDurationFlag, pulseMagnitudeFlag;
	BOOLN	refractoryPeriodFlag;
	BOOLN	updateProcessVariablesFlag;
	int		numFibres;
	long	ranSeed;
	double	pulseDuration;
	double	pulseMagnitude;
	double	refractoryPeriod;

	/* Private members */
	UniParListPtr	parList;
	int		numChannels;
	double	*lastOutput;
	ChanLen	*remainingPulseIndex, pulseDurationIndex;
	EarObjectPtr	refractAdjData;

} BinomialSG, *BinomialSGPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	BinomialSGPtr	binomialSGPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_ANSpikeGen_Binomial(EarObjectPtr data);

BOOLN	CheckPars_ANSpikeGen_Binomial(void);

BOOLN	Free_ANSpikeGen_Binomial(void);

void	FreeProcessVariables_ANSpikeGen_Binomial(void);

UniParListPtr	GetUniParListPtr_ANSpikeGen_Binomial(void);

BOOLN	Init_ANSpikeGen_Binomial(ParameterSpecifier parSpec);

BOOLN	InitProcessVariables_ANSpikeGen_Binomial(EarObjectPtr data);

BOOLN	PrintPars_ANSpikeGen_Binomial(void);

BOOLN	ReadPars_ANSpikeGen_Binomial(char *fileName);

BOOLN	RunModel_ANSpikeGen_Binomial(EarObjectPtr data);

BOOLN	InitModule_ANSpikeGen_Binomial(ModulePtr theModule);

BOOLN	SetNumFibres_ANSpikeGen_Binomial(int theNumFibres);

BOOLN	SetParsPointer_ANSpikeGen_Binomial(ModulePtr theModule);

BOOLN	SetPars_ANSpikeGen_Binomial(int numFibres, long ranSeed,
		  double pulseDuration, double pulseMagnitude,
		  double refractoryPeriod);

BOOLN	SetPulseDuration_ANSpikeGen_Binomial(double thePulseDuration);

BOOLN	SetPulseMagnitude_ANSpikeGen_Binomial(double thePulseMagnitude);

BOOLN	SetRanSeed_ANSpikeGen_Binomial(long theRanSeed);

BOOLN	SetRefractoryPeriod_ANSpikeGen_Binomial(double theRefractoryPeriod);

BOOLN	SetUniParList_ANSpikeGen_Binomial(void);

__END_DECLS

#endif
