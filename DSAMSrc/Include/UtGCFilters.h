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
 
/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#ifndef PI 
# define PI 3.1415926538
#endif
#ifndef TwoPI
# define TwoPI (2.* PI)
#endif
#ifndef winSizeERB
# define winSizeERB	3
#endif

#define MAX(x,y) ((x)<(y)? (y):(x))
#define MIN(x,y) ((x)<(y)? (x):(y))

#define GCFILTERS_NUM_CASCADE_ACF_FILTER		4 	/* cascadeAC */ 
#define GCFILTERS_NUM_CASCADE_ERBGT_FILTER		4 	/* cascadeAC */ 
#define GCFILTERS_NUM_ACF_STATE_VARS_PER_FILTER	2 	/* - per cascaded filter*/
#define GCFILTERS_NUM_LI_STATE_VARS_PER_FILTER	1 	/* - per cascaded filter*/
#define GCFILTERS_NUM_CNTL_STATE_VARS_PER_FILTER	1 /* - per cascaded filter*/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/
typedef struct {

	int	cascade;				/* The cascade of the filter */
	double	*numCoeffs;			/* feed feedforward coeffs */
	double	*denomCoeffs;		/* feed feedback coeffs    */
	double	gainLossFactor;
	double	*stateFVector;
	double	*stateBVector;

} AsymCmpCoeffs, *AsymCmpCoeffsPtr;

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

void	FreeAsymCmpCoeffs_GCFilters(AsymCmpCoeffsPtr *p);

void	FreeERBGammaToneCoeffs_GCFilters(ERBGammaToneCoeffsPtr *p);

void	FreeOwoPoleCoeffs_GCFilters(OnePoleCoeffsPtr *p);

void	FreeCntlGammaChirp_GCFilters(CntlGammaCPtr *p);

void	FreeHammingWindow(double *p);

void	FreeERBWindow_GCFilters(double *p);

void	FreeLeakyIntCoeffs_GCFilters(OnePoleCoeffsPtr *p);

void	AsymCmp_GCFilters(SignalDataPtr theSignal, ChanLen nsmpl, 
						AsymCmpCoeffsPtr p[]);

BOOLN	CheckCntlInit_GCFilters(CntlGammaCPtr *cntlGammaC);

void	ERBGammaTone_GCFilters(SignalDataPtr theSignal, 
						ERBGammaToneCoeffsPtr p[]);

void	LeakyInt_GCFilters(CntlGammaCPtr p[], OnePoleCoeffsPtr q[], 
		  int numChannels);

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
