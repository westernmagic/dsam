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

#define ANSPIKEGEN_CARNEY_NUM_PARS			13

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	ANSPIKEGEN_CARNEY_DIAGNOSTICMODE,
	ANSPIKEGEN_CARNEY_INPUTMODE,
	ANSPIKEGEN_CARNEY_RANSEED,
	ANSPIKEGEN_CARNEY_NUMFIBRES,
	ANSPIKEGEN_CARNEY_PULSEDURATION,
	ANSPIKEGEN_CARNEY_PULSEMAGNITUDE,
	ANSPIKEGEN_CARNEY_REFRACTORYPERIOD,
	ANSPIKEGEN_CARNEY_MAXTHRESHOLD,
	ANSPIKEGEN_CARNEY_DISCHARGECOEFFC0,
	ANSPIKEGEN_CARNEY_DISCHARGECOEFFC1,
	ANSPIKEGEN_CARNEY_DISCHARGETCONSTS0,
	ANSPIKEGEN_CARNEY_DISCHARGETCONSTS1,
	ANSPIKEGEN_CARNEY_DISTRIBUTION

} CarneySGParSpecifier;

typedef enum {

	ANSPIKEGEN_CARNEY_INPUTMODE_CORRECTED,
	ANSPIKEGEN_CARNEY_INPUTMODE_ORIGINAL,
	ANSPIKEGEN_CARNEY_INPUTMODE_NULL

} ANSpikeGenInputModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	updateProcessVariablesFlag;
	int		diagnosticMode;
	int		inputMode;
	long	ranSeed;
	int		numFibres;
	Float	pulseDuration;
	Float	pulseMagnitude;
	Float	refractoryPeriod;
	Float	maxThreshold;
	Float	dischargeCoeffC0;
	Float	dischargeCoeffC1;
	Float	dischargeTConstS0;
	Float	dischargeTConstS1;
	ParArrayPtr	distribution;

	/* Private members */
	NameSpecifier	*diagnosticModeList;
	UniParListPtr	parList;
	WChar			diagFileName[MAX_FILE_PATH];
	FILE	*fp;
	int		numChannels;
	int		*numFibres2;
	Float	dt, wPulseDuration;
	Float	**timer;
	Float	**remainingPulseTime;

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

BOOLN	Free_ANSpikeGen_Carney(void);

void	FreeProcessVariables_ANSpikeGen_Carney(void);

UniParListPtr	GetUniParListPtr_ANSpikeGen_Carney(void);

BOOLN	InitProcessVariables_ANSpikeGen_Carney(EarObjectPtr data);

BOOLN	Init_ANSpikeGen_Carney(ParameterSpecifier parSpec);

BOOLN	PrintPars_ANSpikeGen_Carney(void);

void	ResetProcess_ANSpikeGen_Carney(EarObjectPtr data);

BOOLN	RunModel_ANSpikeGen_Carney(EarObjectPtr data);

BOOLN	SetDiagnosticMode_ANSpikeGen_Carney(WChar * theDiagnosticMode);

BOOLN	SetDischargeCoeffC0_ANSpikeGen_Carney(Float theDischargeCoeffC0);

BOOLN	SetDischargeCoeffC1_ANSpikeGen_Carney(Float theDischargeCoeffC1);

BOOLN	SetDischargeTConstS0_ANSpikeGen_Carney(Float theDischargeTConstS0);

BOOLN	SetDischargeTConstS1_ANSpikeGen_Carney(Float theDischargeTConstS1);

BOOLN	SetDistribution_ANSpikeGen_Carney(ParArrayPtr theDistribution);

BOOLN	SetInputMode_ANSpikeGen_Carney(WChar * theInputMode);

BOOLN	SetMaxThreshold_ANSpikeGen_Carney(Float theMaxThreshold);

BOOLN	InitModule_ANSpikeGen_Carney(ModulePtr theModule);

BOOLN	SetNumFibres_ANSpikeGen_Carney(int theNumFibres);

BOOLN	SetParsPointer_ANSpikeGen_Carney(ModulePtr theModule);

BOOLN	SetPulseDuration_ANSpikeGen_Carney(Float thePulseDuration);

BOOLN	SetPulseMagnitude_ANSpikeGen_Carney(Float thePulseMagnitude);

BOOLN	SetRanSeed_ANSpikeGen_Carney(long theRanSeed);

BOOLN	SetRefractoryPeriod_ANSpikeGen_Carney(Float theRefractoryPeriod);

BOOLN	SetUniParList_ANSpikeGen_Carney(void);

__END_DECLS

#endif
