/**********************
 *
 * File:		UtGcFilters.h
 * Purpose:		This module contains the functions that initialise, generate,
 *				control various filters.
 * Comments:	This module is based on subroutine of Package of GCFBv1.04a.
 * Author:		Masashi Unoki
 * Created:		21 Nov 2000
 * Updated:		07 Dec 2000
 * Copyright:	(c) 2000, CNBH Univ. of Cambridge
 *
 **********************/

#ifndef	_UTGCFILTERS_H
#define _UTGCFILTERS_H	1
 
#include "UtFFT.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#ifndef PI 
# define PI 3.1415926538
#endif
#ifndef TwoPI
# define TwoPI (2* PI)
#endif
#ifndef winSizeERB
# define winSizeERB	3
#endif

#define GCFILTERS_NUM_CASCADE_ACF_FILTER			4 	/* cascadeAC */ 
#define GCFILTERS_NUM_CASCADE_ERBGT_FILTER			4 	/* cascadeAC */ 
#define GCFILTERS_NUM_ACF_STATE_VARS_PER_FILTER		2 	/* - per cascaded filter*/
#define GCFILTERS_NUM_LI_STATE_VARS_PER_FILTER		1 	/* - per cascaded filter*/
#define GCFILTERS_NUM_CNTL_STATE_VARS_PER_FILTER	1 /* - per cascaded filter*/
#define GCFILTERS_NUM_FRQ_RSL						1024 /* Frequency Resolution */

#define GCFILTERS_REF_FREQ				1000.0	/* Reference frequency */
#define	GCFILTERS_ACF_P0				2.0;
#define	GCFILTERS_ACF_P4				1.0724;
#define	GCFILTERS_ACF_NUM_COEFFS		3
#define	GCFILTERS_ACF_LEN_STATE_VECTOR	3

/******************************************************************************/
/*************************** Macro Definitions ********************************/
/******************************************************************************/

#define MAX(x,y) ((x)<(y)? (y):(x))
#define MIN(x,y) ((x)<(y)? (x):(y))

#define GCFILTERS_P1(B, C)	(1.7818 * (1 - 0.0791 * (B)) * (1 - 0.1655 * fabs(C)))
#define GCFILTERS_P2(B, C)	(0.5689 * (1 - 0.1620 * (B)) * (1 - 0.0857 * fabs(C)))
#define GCFILTERS_P3(B, C)	(0.2523 * (1 - 0.0244 * (B)) * (1 + 0.0574 * fabs(C)))

/* The following macro estimates fr from fpeak: Revised from matlab Fr2Fpeak.m */
#define GCFILTERS_FR2FPEAK(N, B, C, FR, ERBW) ((FR) + (C) * (ERBW) * (B) / (N))

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	GCFILTER_CARRIER_MODE_COMPLEX,
	GCFILTER_CARRIER_MODE_COS,
	GCFILTER_CARRIER_MODE_ENVELOPE,
	GCFILTER_CARRIER_MODE_SIN,
	GCFILTER_CARRIER_MODE_NULL

} GCCarrierModeSpecifier;

typedef struct {

	int	cascade;				/* The cascade of the filter */
	double	*numCoeffs;			/* feed feedforward coeffs */
	double	*denomCoeffs;		/* feed feedback coeffs    */
	double	gainLossFactor;
	double	*stateFVector;
	double	*stateBVector;

} AsymCmpCoeffs, *AsymCmpCoeffsPtr;

typedef struct {

	int		numFilt;			/* The cascade of the filter */
	double	fs;					/* Sampling rate */
	double	p0, p1, p2, p3, p4;
	double	b, c;
	double	*bz;				/* MA coefficients  (NumCh*3*NumFilt) */
	double	*ap;				/* AR coefficients  (NumCh*3*NumFilt) */
	double	*sigInPrev;			/* Input state vector */
	double	*sigOutPrev;		/* Output state vector */
	BandwidthModePtr	bMode;

} AsymCmpCoeffs2, *AsymCmpCoeffs2Ptr;

typedef struct {

	int	cascade;
	double	*a0, *a1, *a2;		/* feed feedforward coeffs */
	double	*b1, *b2;			/* feed feedback coeffs    */
	double	gainLossFactor;
	double	*stateVector;

} ERBGammaToneCoeffs, *ERBGammaToneCoeffsPtr;

typedef struct {

	int	cascade;
	double	a0, a1;			/* feed feedforward coeffs */
	double	b1;				/* feed feedback coeffs    */
	double	gainLossFactor;
	double	*stateVector;

} OnePoleCoeffs, *OnePoleCoeffsPtr;

typedef struct {

	double 	outSignalLI;
	double	aEst;			/* Estimated a */
	double	cEst;			/* Estimated c */
	double	psEst;			/* Estimated Ps */

} CntlGammaC, *CntlGammaCPtr;

typedef struct {

	FFTArrayPtr	pGC;
	FFTArrayPtr	pGCOut;

} GammaChirpCoeffs, *GammaChirpCoeffsPtr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern	double	Filters_AsymCmpCoef0[];        	/* ACF coefficents */

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

void	ACFilterBank_GCFilters(AsymCmpCoeffs2Ptr *aCFCoeffs, EarObjectPtr data,
		  ChanLen sample);

void	AsymCmp_GCFilters(SignalDataPtr theSignal, ChanLen nsmpl, 
						AsymCmpCoeffsPtr p[]);

void	ASympCmpFreqResp_GCFilters(double *asymFunc, double frs, double fs, double b,
		  double c, BandwidthModePtr bMode);

NameSpecifier *	PGCCarrierList_GCFilters(int index);

BOOLN	CheckCntlInit_GCFilters(CntlGammaCPtr *cntlGammaC);

void	ERBGammaTone_GCFilters(SignalDataPtr theSignal, 
						ERBGammaToneCoeffsPtr p[]);

void	FreeAsymCmpCoeffs_GCFilters(AsymCmpCoeffsPtr *p);

void	FreeAsymCmpCoeffs2_GCFilters(AsymCmpCoeffs2Ptr *p);

void	FreeERBGammaToneCoeffs_GCFilters(ERBGammaToneCoeffsPtr *p);

void	FreeOwoPoleCoeffs_GCFilters(OnePoleCoeffsPtr *p);

void	FreeCntlGammaChirp_GCFilters(CntlGammaCPtr *p);

void	FreePGammaChirpCoeffs_GCFilters(GammaChirpCoeffsPtr *p);

void	FreeHammingWindow(double *p);

void	FreeERBWindow_GCFilters(double *p);

void	FreeLeakyIntCoeffs_GCFilters(OnePoleCoeffsPtr *p);

void	GammaChirpAmpFreqResp_GCFilters(double *ampFrsp, double frs, double eRBw,
		  double sR, double orderG, double coefERBw, double coefC, double phase);

AsymCmpCoeffs2Ptr	InitAsymCmpCoeffs2_GCFilters(int cascade, double fs,
					  double b, double c, BandwidthModePtr bMode);

GammaChirpCoeffsPtr	InitPGammaChirpCoeffs_GCFilters(double cF, double bw, double sR,
					  double orderG, double coefERBw, double coefC, double phase,
					  int swCarr, int swNorm, SignalDataPtr inSignal);

void	LeakyInt_GCFilters(CntlGammaCPtr p[], OnePoleCoeffsPtr q[], 
		  int numChannels);

void	PassiveGCFilter_GCFilters(EarObjectPtr data, GammaChirpCoeffsPtr *pGCoeffs);

void	SetAsymCmpCoeffs2_GCFilters(AsymCmpCoeffs2Ptr p, double frs);

void	SetpsEst_GCFilters(CntlGammaCPtr cntlGammaC[], int numChannels, 
		  double *winPsEst, double coefPsEst);

void	SetcEst_GCFilters(CntlGammaCPtr cntlGammaC[], int numChannels, 
		  double cCoeff0, double cCoeff1, double cLowerLim, double cUpperLim);

void 	SetaEst_GCFilters(CntlGammaCPtr cntlGammaC[], int numChannels, 
		  double cmprs);

void	CntlGammaChirp_GCFilters(SignalDataPtr theSignal, ChanLen nsmpl, 
		  CntlGammaCPtr cntlGammaC[], double cCoeff0, double cCoeff1,
		  double cLowerLim, double cUpperLim, double *winPsEst,
		  double coefPsEst, double cmprs, OnePoleCoeffsPtr coefficientsLI[]);

void	CalcAsymCmpCoeffs_GCFilters(AsymCmpCoeffsPtr p, double centreFreq, 
		  double bWidth3dB, double bCoeff, double cCoeff, int cascade,
		  double sampleClk);

AsymCmpCoeffsPtr	InitAsymCmpCoeffs_GCFilters(void);

OnePoleCoeffsPtr 	InitLeakyIntCoeffs_GCFilters(double Tcnst,
					  double sampleClk);

ERBGammaToneCoeffsPtr	InitERBGammaToneCoeffs_GCFilters(double centreFreq, 
						  double bWidth3dB, double bCoeff, int cascade,
						  double sampleClk);

CntlGammaCPtr	InitCntlGammaChirp_GCFilters(void);

double*	InitERBWindow_GCFilters(double eRBDensity, int numChannels);

BOOLN	CheckInit_CntlGammaC(CntlGammaCPtr cntlGammaC[]);
					   
__END_DECLS

#endif
