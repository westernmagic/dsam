/**********************
 *
 * File:		MoANSGCarney.h
 * Purpose:		Laurel H. Carney auditory nerve spike generation module:
 *				Carney L. H. (1993) "A model for the responses of low-frequency
 *				auditory-nerve fibers in cat", JASA, 93, pp 401-417.
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 *				24-02-97 LPO: Amended to allow multiple fibres.
 * Authors:		L. P. O'Mard modified from L. H. Carney's code
 * Created:		7 May 1996
 * Updated:		24 Feb 1997
 * Copyright:	(c) 1997,  University of Essex
 *
 *********************/

#ifndef _MOANSGCARNEY_H
#define _MOANSGCARNEY_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define ANSPIKEGEN_CARNEY_NUM_PARS			10

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	ANSPIKEGEN_CARNEY_RANSEED,
	ANSPIKEGEN_CARNEY_NUMFIBRES,
	ANSPIKEGEN_CARNEY_PULSEDURATION,
	ANSPIKEGEN_CARNEY_PULSEMAGNITUDE,
	ANSPIKEGEN_CARNEY_REFRACTORYPERIOD,
	ANSPIKEGEN_CARNEY_MAXTHRESHOLD,
	ANSPIKEGEN_CARNEY_DISCHARGECOEFFC0,
	ANSPIKEGEN_CARNEY_DISCHARGECOEFFC1,
	ANSPIKEGEN_CARNEY_DISCHARGETCONSTS0,
	ANSPIKEGEN_CARNEY_DISCHARGETCONSTS1

} CarneySGParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	ranSeedFlag, numFibresFlag, pulseDurationFlag, pulseMagnitudeFlag;
	BOOLN	refractoryPeriodFlag, maxThresholdFlag, dischargeCoeffC0Flag;
	BOOLN	dischargeCoeffC1Flag, dischargeTConstS0Flag, dischargeTConstS1Flag;
	BOOLN	updateProcessVariablesFlag;
	long	ranSeed;
	int		numFibres;
	double	pulseDuration;
	double	pulseMagnitude;
	double	refractoryPeriod;
	double	maxThreshold;
	double	dischargeCoeffC0;
	double	dischargeCoeffC1;
	double	dischargeTConstS0;
	double	dischargeTConstS1;

	/* Private members */
	UniParListPtr	parList;
	double	*timer;
	double	*remainingPulseTime;

} CarneySG, *CarneySGPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	CarneySGPtr	carneySGPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_ANSpikeGen_Carney(EarObjectPtr data);

BOOLN	CheckPars_ANSpikeGen_Carney(void);

BOOLN	Free_ANSpikeGen_Carney(void);

void	FreeProcessVariables_ANSpikeGen_Carney(void);

UniParListPtr	GetUniParListPtr_ANSpikeGen_Carney(void);

BOOLN	InitProcessVariables_ANSpikeGen_Carney(EarObjectPtr data);

BOOLN	Init_ANSpikeGen_Carney(ParameterSpecifier parSpec);

BOOLN	PrintPars_ANSpikeGen_Carney(void);

BOOLN	ReadPars_ANSpikeGen_Carney(char *fileName);

BOOLN	RunModel_ANSpikeGen_Carney(EarObjectPtr data);

BOOLN	SetDischargeCoeffC0_ANSpikeGen_Carney(double theDischargeCoeffC0);

BOOLN	SetDischargeCoeffC1_ANSpikeGen_Carney(double theDischargeCoeffC1);

BOOLN	SetDischargeTConstS0_ANSpikeGen_Carney(double theDischargeTConstS0);

BOOLN	SetDischargeTConstS1_ANSpikeGen_Carney(double theDischargeTConstS1);

BOOLN	SetMaxThreshold_ANSpikeGen_Carney(double theMaxThreshold);

BOOLN	InitModule_ANSpikeGen_Carney(ModulePtr theModule);

BOOLN	SetNumFibres_ANSpikeGen_Carney(int theNumFibres);

BOOLN	SetParsPointer_ANSpikeGen_Carney(ModulePtr theModule);

BOOLN	SetPars_ANSpikeGen_Carney(long ranSeed, int numFibres,
		  double pulseDuration, double pulseMagnitude, double
		  refractoryPeriod, double maxThreshold, double dischargeCoeffC0,
		  double dischargeCoeffC1, double dischargeTConstS0,
		  double dischargeTConstS1);

BOOLN	SetPulseDuration_ANSpikeGen_Carney(double thePulseDuration);

BOOLN	SetPulseMagnitude_ANSpikeGen_Carney(double thePulseMagnitude);

BOOLN	SetRanSeed_ANSpikeGen_Carney(long theRanSeed);

BOOLN	SetRefractoryPeriod_ANSpikeGen_Carney(double theRefractoryPeriod);

BOOLN	SetUniParList_ANSpikeGen_Carney(void);

__END_DECLS

#endif
