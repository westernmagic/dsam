/**********************
 *
 * File:		MoBMDGammaC.h
 * Purpose:		An implementation of the dynamic compressive gammachirp: Irino T.
 * 				and Patterson R. D. (2006) "A Dynamic Compressive Gammachirp
 *				Auditory Filterbank", IEEE Transcation on Audio, Speech and
 * 				Language Processing, Vol. 14, No. 6, pp 2222-2232.
 * Comments:	Written using ModuleProducer version 1.6.0 (Jun 20 2007).
 *				This code module was revised from the implementation in the
 *				AIM2007a matlab code.
 * Author:		L. P. O'Mard 
 * Created:		18 Jun 2007
 * Updated:	
 * Copyright:	(c) 2007, L. P. O'Mard
 *
 *********************/

#ifndef _MOBMDGAMMAC_H
#define _MOBMDGAMMAC_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define BM_GC_DYN_NUM_PARS			25
#define BM_GC_DYN_MOD_NAME			wxT("BM_GammaC_Dyn")

#define BM_GC_DYN_NUM_GC_ARRAYS_N		4
#define BM_GC_DYN_NUM_LVLEST_ARRAYS_N	2
#define	BM_GC_DYN_LVLLIN_SIZE			2
#define	BM_GC_DYN_FRATVAL_EXPRS			2
#define	BM_GC_DYN_FRATVAL_ARRAY_N		2

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	BM_GC_DYN_DIAGNOSTICMODE,
	BM_GC_DYN_PGCCARRIERMODE,
	BM_GC_DYN_PEAKSPECTRUMNORMMODE,
	BM_GC_DYN_GC_CTRL,
	BM_GC_DYN_GC_N,
	BM_GC_DYN_GC_B1,
	BM_GC_DYN_GC_C1,
	BM_GC_DYN_GC_ARRAYN,
	BM_GC_DYN_GC_FRAT,
	BM_GC_DYN_GC_B2,
	BM_GC_DYN_GC_C2,
	BM_GC_DYN_GC_PHASE,
	BM_GC_DYN_GC_GAINREFDB,
	BM_GC_DYN_GC_GAINCMPNSTDB,
	BM_GC_DYN_LVEST_LCTERB,
	BM_GC_DYN_LVEST_DECAYHL,
	BM_GC_DYN_LVEST_B2,
	BM_GC_DYN_LVEST_C2,
	BM_GC_DYN_LVEST_FRAT,
	BM_GC_DYN_LVEST_RMSTOSPLDB,
	BM_GC_DYN_LVEST_WEIGHT,
	BM_GC_DYN_LVEST_REFDB,
	BM_GC_DYN_LVEST_ARRAYN,
	BM_GC_DYN_LVEST_PWR,
	BM_GC_DYN_THECFS

} BMDGammaCParSpecifier;

typedef enum {

	BM_GC_DYN_GC_CTRL_FIXED,
	BM_GC_DYN_GC_CTRL_TIME_VARYING,
	BM_GC_DYN_GC_CTRL_NULL

} BasilarMGC_ctrlSpecifier;

typedef struct {

	int		nchLvlEst;
	double	fp1;
	double	eF;
	double	b2Val, c2Val;
	double	gainFactor;
	double	lvldB;
	double	fratVal[BM_GC_DYN_FRATVAL_EXPRS];
	double	savedPGCOut;
	double	lvlLinPrev[BM_GC_DYN_LVLLIN_SIZE];
	double	lvlLinNow[BM_GC_DYN_LVLLIN_SIZE];

} BMGCDGenChanInfo, *BMGCDGenChanInfoPtr;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	updateProcessVariablesFlag;
	int		diagMode;
	int		pGCCarrierMode;
	BOOLN	peakSpectrumNormMode;
	int		gC_ctrl;
	int		gC_n;
	double	gC_b1;
	double	gC_c1;
	int		gC_arrayN;
	double	*gC_frat;
	double	*gC_b2;
	double	*gC_c2;
	double	gC_phase;
	double	gC_gainRefdB;
	double	gC_gainCmpnstdB;
	double	lvEst_LctERB;
	double	lvEst_DecayHL;
	double	lvEst_b2;
	double	lvEst_c2;
	double	lvEst_frat;
	double	lvEst_RMStoSPLdB;
	double	lvEst_Weight;
	double	lvEst_RefdB;
	int		lvEst_arrayN;
	double	*lvEst_Pwr;
	CFListPtr	theCFs;

	/* Private members */
	NameSpecifier	*diagModeList;
	UniParListPtr	parList;
	WChar	diagFileName[MAX_FILE_PATH];
	int		numChannels;
	double	expDecayVal, lvlLinRef, lvlLinMinLim;
	BMGCDGenChanInfo	*genChanInfo;
	GammaChirpCoeffsPtr	*pGCoeffs;
	AsymCmpCoeffs2Ptr	*aCFCoeffLvlEst;
	AsymCmpCoeffs2Ptr	*aCFCoeffSigPath;

} BMDGammaC, *BMDGammaCPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	BMDGammaCPtr	bMDGammaCPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	AllocGC_arrayN_BasilarM_GammaChirp_Dyn(int gC_arrayN);

BOOLN	AllocLvEst_arrayN_BasilarM_GammaChirp_Dyn(int lvEst_arrayN);

BOOLN	CheckData_BasilarM_GammaChirp_Dyn(EarObjectPtr data);

BOOLN	CheckPars_BasilarM_GammaChirp_Dyn(void);

void	FreeProcessVariables_BasilarM_GammaChirp_Dyn(void);

BOOLN	Free_BasilarM_GammaChirp_Dyn(void);

CFListPtr	GetCFListPtr_BasilarM_GammaChirp_Dyn(void);

UniParListPtr	GetUniParListPtr_BasilarM_GammaChirp_Dyn(void);

BOOLN	InitModule_BasilarM_GammaChirp_Dyn(ModulePtr theModule);

BOOLN	InitProcessVariables_BasilarM_GammaChirp_Dyn(EarObjectPtr data);

BOOLN	Init_BasilarM_GammaChirp_Dyn(ParameterSpecifier parSpec);

BOOLN	PrintPars_BasilarM_GammaChirp_Dyn(void);

BOOLN	RunModel_BasilarM_GammaChirp_Dyn(EarObjectPtr data);

BOOLN	SetBandWidths_BasilarM_GammaChirp_Dyn(WChar *theBandwidthMode,
		  double *theBandwidths);

BOOLN	SetCFList_BasilarM_GammaChirp_Dyn(CFListPtr theCFList);

BOOLN	SetDefaultGC_arrayNArrays_BasilarM_GammaChirp_Dyn(void);

BOOLN	SetDefaultLvEst_arrayNArrays_BasilarM_GammaChirp_Dyn(void);

BOOLN	SetDiagMode_BasilarM_GammaChirp_Dyn(WChar * theDiagMode);

BOOLN	SetGC_arrayN_BasilarM_GammaChirp_Dyn(int theGC_arrayN);

BOOLN	SetGC_b1_BasilarM_GammaChirp_Dyn(double theGC_b1);

BOOLN	SetGC_b2_BasilarM_GammaChirp_Dyn(double *theGC_b2);

BOOLN	SetGC_c1_BasilarM_GammaChirp_Dyn(double theGC_c1);

BOOLN	SetGC_c2_BasilarM_GammaChirp_Dyn(double *theGC_c2);

BOOLN	SetGC_ctrl_BasilarM_GammaChirp_Dyn(WChar * theGC_ctrl);

BOOLN	SetGC_frat_BasilarM_GammaChirp_Dyn(double *theGC_frat);

BOOLN	SetGC_n_BasilarM_GammaChirp_Dyn(int theGC_n);

BOOLN	SetGC_gainCmpnstdB_BasilarM_GammaChirp_Dyn(double theGC_gainCmpnstdB);

BOOLN	SetGC_gainRefdB_BM_GC_DYN(double theGC_gainRefdB);

BOOLN	SetGC_phase_BasilarM_GammaChirp_Dyn(double theGC_phase);

BOOLN	SetIndividualGC_b2_BasilarM_GammaChirp_Dyn(int theIndex,
		  double theGC_b2);

BOOLN	SetIndividualGC_c2_BasilarM_GammaChirp_Dyn(int theIndex,
		  double theGC_c2);

BOOLN	SetIndividualGC_frat_BasilarM_GammaChirp_Dyn(int theIndex,
		  double theGC_frat);

BOOLN	SetIndividualLvEst_Pwr_BasilarM_GammaChirp_Dyn(int theIndex,
		  double theLvEst_Pwr);

BOOLN	SetLvEst_DecayHL_BasilarM_GammaChirp_Dyn(double theLvEst_DecayHL);

BOOLN	SetLvEst_LctERB_BasilarM_GammaChirp_Dyn(double theLvEst_LctERB);

BOOLN	SetLvEst_Pwr_BasilarM_GammaChirp_Dyn(double *theLvEst_Pwr);

BOOLN	SetLvEst_RMStoSPLdB_BasilarM_GammaChirp_Dyn(
		  double theLvEst_RMStoSPLdB);

BOOLN	SetLvEst_RefdB_BasilarM_GammaChirp_Dyn(double theLvEst_RefdB);

BOOLN	SetLvEst_Weight_BasilarM_GammaChirp_Dyn(double theLvEst_Weight);

BOOLN	SetLvEst_arrayN_BasilarM_GammaChirp_Dyn(int theLvEst_arrayN);

BOOLN	SetLvEst_b2_BasilarM_GammaChirp_Dyn(double theLvEst_b2);

BOOLN	SetLvEst_c2_BasilarM_GammaChirp_Dyn(double theLvEst_c2);

BOOLN	SetLvEst_frat_BasilarM_GammaChirp_Dyn(double theLvEst_frat);

BOOLN	SetPGCCarrierMode_BasilarM_GammaChirp_Dyn(WChar * thePGCCarrierMode);

BOOLN	SetParsPointer_BasilarM_GammaChirp_Dyn(ModulePtr theModule);

BOOLN	SetPeakSpectrumNormMode_BasilarM_GammaChirp_Dyn(
		  WChar *thePeakSpectrumNormMode);

BOOLN	SetUniParList_BasilarM_GammaChirp_Dyn(void);

__END_DECLS

#endif
