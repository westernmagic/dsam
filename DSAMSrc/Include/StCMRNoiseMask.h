/**********************
 *
 * File:		StCMRNoiseMask.h
 * Purpose:		
 * Comments:	Written using ModuleProducer version 1.6.1 (Sep 24 2008).
 * Author:		
 * Created:		30 Sep 2008
 * Updated:	
 * Copyright:	(c) 2008, 
 *
 *********************/

#ifndef _STCMRNOISEMASK_H
#define _STCMRNOISEMASK_H 1

#include "UtCmplxM.h"
#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define CMR_NOISEMASKER_MOD_NAME			wxT("STIM_CMR_NOISEMASKER")
#define CMR_NOISEMASKER_NUM_PARS			22
#define CMR_NOISEMASKER_MAXNTOTAL			9	/* aximun number of flanking bands */

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	CMR_NOISEMASKER_BINAURALMODE,
	CMR_NOISEMASKER_SPACINGTYPE,
	CMR_NOISEMASKER_CONDITION,
	CMR_NOISEMASKER_ONFREQEAR,
	CMR_NOISEMASKER_SIGEAR,
	CMR_NOISEMASKER_FLANKEAR,
	CMR_NOISEMASKER_NLOW,
	CMR_NOISEMASKER_NUPP,
	CMR_NOISEMASKER_NGAPLOW,
	CMR_NOISEMASKER_NGAPUPP,
	CMR_NOISEMASKER_RANSEED,
	CMR_NOISEMASKER_LOWFBLEVEL,
	CMR_NOISEMASKER_UPPFBLEVEL,
	CMR_NOISEMASKER_BATTEN,
	CMR_NOISEMASKER_SPACING,
	CMR_NOISEMASKER_MASKERMODFREQ,
	CMR_NOISEMASKER_BANDWIDTH,
	CMR_NOISEMASKER_SIGLEVEL,
	CMR_NOISEMASKER_SIGFREQ,
	CMR_NOISEMASKER_GATETIME,
	CMR_NOISEMASKER_DURATION,
	CMR_NOISEMASKER_SAMPLINGINTERVAL

} CMRNoiseMParSpecifier;

typedef enum {

	CMR_NOISEMASKER_SPACINGTYPE_LINEAR,
	CMR_NOISEMASKER_SPACINGTYPE_OCTAVE,
	CMR_NOISEMASKER_SPACINGTYPE_NULL

} CMRSpacingTypeSpecifier;

typedef enum {

	CMR_NOISEMASKER_CONDITION_CD,
	CMR_NOISEMASKER_CONDITION_CM,
	CMR_NOISEMASKER_CONDITION_DV,
	CMR_NOISEMASKER_CONDITION_RF,
	CMR_NOISEMASKER_CONDITION_SO,
	CMR_NOISEMASKER_CONDITION_NULL

} CMRConditionSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	updateProcessVariablesFlag;
	int		binauralMode;
	int		spacingType;
	int		condition;
	int		onFreqEar;
	int		sigEar;
	WChar	flankEar[MAXLINE];
	int		nlow;
	int		nupp;
	int		nGapLow;
	int		nGapUpp;
	long	ranSeed;
	double	lowFBLevel;
	double	uppFBLevel;
	double	oFMLevel;
	double	spacing;
	double	maskerModFreq;
	double	bandwidth;
	double	sigLevel;
	double	sigFreq;
	double	gateTime;
	double	duration, dt;

	/* Private members */
	NameSpecifier	*spacingTypeList;
	NameSpecifier	*conditionList;
	UniParListPtr	parList;
	FFTArrayPtr		fTInv;

} CMRNoiseM, *CMRNoiseMPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	CMRNoiseMPtr	cMRNoiseMPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckData_CMR_NoiseMasker(EarObjectPtr data);

BOOLN	FreeProcessVariables_CMR_NoiseMasker(void);

BOOLN	Free_CMR_NoiseMasker(void);

BOOLN	GenerateSignal_CMR_NoiseMasker(EarObjectPtr data);

UniParListPtr	GetUniParListPtr_CMR_NoiseMasker(void);

BOOLN	InitConditionList_CMR_NoiseMasker(void);

BOOLN	InitModule_CMR_NoiseMasker(ModulePtr theModule);

BOOLN	InitProcessVariables_CMR_NoiseMasker(EarObjectPtr data);

BOOLN	InitSpacingTypeList_CMR_NoiseMasker(void);

BOOLN	Init_CMR_NoiseMasker(ParameterSpecifier parSpec);

BOOLN	PrintPars_CMR_NoiseMasker(void);

BOOLN	SetOFMLevel_CMR_NoiseMasker(double theOFMLevel);

BOOLN	SetBinauralMode_CMR_NoiseMasker(WChar * theBinauralMode);

BOOLN	SetCondition_CMR_NoiseMasker(WChar * theCondition);

BOOLN	SetDuration_CMR_NoiseMasker(double theDuration);

BOOLN	SetFlankEar_CMR_NoiseMasker(WChar *theFlankEar);

BOOLN	SetGateTime_CMR_NoiseMasker(double theGateTime);

BOOLN	SetBandwidth_CMR_NoiseMasker(double theBandwidth);

BOOLN	SetLowFBLevel_CMR_NoiseMasker(double theLowFBLevel);

BOOLN	SetMaskerModFreq_CMR_NoiseMasker(double theMaskerModFreq);

BOOLN	SetNGapLow_CMR_NoiseMasker(int theNGapLow);

BOOLN	SetNGapUpp_CMR_NoiseMasker(int theNGapUpp);

BOOLN	SetNlow_CMR_NoiseMasker(int theNlow);

BOOLN	SetNupp_CMR_NoiseMasker(int theNupp);

BOOLN	SetOnFreqEar_CMR_NoiseMasker(WChar * theOnFreqEar);

BOOLN	SetParsPointer_CMR_NoiseMasker(ModulePtr theModule);

BOOLN	SetRanSeed_CMR_NoiseMasker(long theRanSeed);

BOOLN	SetSamplingInterval_CMR_NoiseMasker(double theSamplingInterval);

BOOLN	SetSigLevel_CMR_NoiseMasker(double theSigLevel);

BOOLN	SetSigEar_CMR_NoiseMasker(WChar * theSigEar);

BOOLN	SetSigFreq_CMR_NoiseMasker(double theSigFreq);

BOOLN	SetSpacingType_CMR_NoiseMasker(WChar * theSpacingType);

BOOLN	SetSpacing_CMR_NoiseMasker(double theSpacing);

BOOLN	SetUniParList_CMR_NoiseMasker(void);

BOOLN	SetUppFBLevel_CMR_NoiseMasker(double theUppFBLevel);

__END_DECLS

#endif
