/**********************
 *
 * File:		MoANSGMeddis02.h
 * Purpose:		Auditory nerve spike generation program, based on vessicle
 *				release with a refractory time an recovery period.
 * Comments:	Written using ModuleProducer version 1.4.0 (Oct 10 2002).
 * Author:		L. P. O'Mard from a matlab routine from R. Meddis
 * Created:		08 Nov 2002
 * Updated:	
 * Copyright:	(c) 2002, CNBH, University of Essex.
 *
 *********************/

#ifndef _MOANSGMEDDIS02_H
#define _MOANSGMEDDIS02_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define ANSPIKEGEN_MEDDIS02_NUM_PARS			6

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	ANSPIKEGEN_MEDDIS02_RANSEED,
	ANSPIKEGEN_MEDDIS02_NUMFIBRES,
	ANSPIKEGEN_MEDDIS02_PULSEDURATION,
	ANSPIKEGEN_MEDDIS02_PULSEMAGNITUDE,
	ANSPIKEGEN_MEDDIS02_REFRACTORYPERIOD,
	ANSPIKEGEN_MEDDIS02_RECOVERYTAU

} Meddis02SGParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	ranSeedFlag, numFibresFlag, pulseDurationFlag, pulseMagnitudeFlag;
	BOOLN	refractoryPeriodFlag, recoveryTauFlag;
	BOOLN	updateProcessVariablesFlag;
	long	ranSeed;
	int		numFibres;
	double	pulseDuration;
	double	pulseMagnitude;
	double	refractoryPeriod;
	double	recoveryTau;

	/* Private members */
	UniParListPtr	parList;
	int		numThreads;
	double	dt, wPulseDuration;
	double	**timer;
	double	**remainingPulseTime;

} Meddis02SG, *Meddis02SGPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	Meddis02SGPtr	meddis02SGPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckData_ANSpikeGen_Meddis02(EarObjectPtr data);

BOOLN	CheckPars_ANSpikeGen_Meddis02(void);

BOOLN	FreeProcessVariables_ANSpikeGen_Meddis02(void);

BOOLN	Free_ANSpikeGen_Meddis02(void);

UniParListPtr	GetUniParListPtr_ANSpikeGen_Meddis02(void);

BOOLN	InitModule_ANSpikeGen_Meddis02(ModulePtr theModule);

BOOLN	InitProcessVariables_ANSpikeGen_Meddis02(EarObjectPtr data);

BOOLN	Init_ANSpikeGen_Meddis02(ParameterSpecifier parSpec);

BOOLN	PrintPars_ANSpikeGen_Meddis02(void);

BOOLN	RunModel_ANSpikeGen_Meddis02(EarObjectPtr data);

BOOLN	SetNumFibres_ANSpikeGen_Meddis02(int theNumFibres);

BOOLN	SetParsPointer_ANSpikeGen_Meddis02(ModulePtr theModule);

BOOLN	SetPulseDuration_ANSpikeGen_Meddis02(double thePulseDuration);

BOOLN	SetPulseMagnitude_ANSpikeGen_Meddis02(double thePulseMagnitude);

BOOLN	SetRanSeed_ANSpikeGen_Meddis02(long theRanSeed);

BOOLN	SetRecoveryTau_ANSpikeGen_Meddis02(double theRecoveryTau);

BOOLN	SetRefractoryPeriod_ANSpikeGen_Meddis02(double theRefractoryPeriod);

BOOLN	SetUniParList_ANSpikeGen_Meddis02(void);

__END_DECLS

#endif
