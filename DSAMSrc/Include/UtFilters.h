/**********************
 *
 * File:		UtFilters.h
 * Purpose:		This module contains the functions that initialise and
 *				generate various filters., e.g. the  2-pole GammaTone filters,
 *				the 2-pole Butterworth low pass filter, the Bessel low pass
 *				filter and the non-linear compresion filter. (They were
 *				revised from M.A. Stone's HUMAN.c 8-10-92 (Exp. Psych.
 *				Cambridge, England).
 * Comments:	Some IIR filters are used, which performs z plane filters for
 *				two-ploe coefficients. A digital band pass filter has been
 *				added.
 *				29-08-95 E.A. Lopez-Poveda: According to the theory, DSP,
 *				William D. Stanley, Reston press, 1st. Ed., p. 172, tan(theta)
 *				must always be positive, ie, the argument of the tangent must
 *				be always smaller than PI / 2. This is being checked here in
 *				all relevant routines.
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		26 Jan 1997
 * Copyright:	(c) 1997,  University of Essex
 *
 **********************/

#ifndef	_UTFILTERS_H
#define _UTFILTERS_H	1
 
/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#ifndef PI
# define PI 3.1415926538
#endif
#define Pi PI
#define Piby2 (PI/2.)
#define TwoPi (2.* Pi)
#define LOWPASS 1
#define HIGHPASS 0
#define DELAY_SF		(double) (NGAMMA - 1)	/* Delay scale factor to
												 * time align FB output */
#define FILTERS_NUM_GAMMAT_STATE_VARS_PER_FILTER	2 /* - per cascaded filter*/
#define FILTERS_NUM_IIR2_STATE_VARS			2
#define FILTERS_NUM_BP_STATE_VARS			4
#define FILTERS_NUM_IIR2CONT_STATE_VARS		4
#define FILTERS_NUM_CONTBUTT1_STATE_VARS	2
#define FILTERS_NUM_CONTBUTT2_STATE_VARS	4

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/
typedef struct {

	int		cascade;				/* The cascade of the filter */
	double	a0, a1;
	double	b1, b2;
	double	*stateVector;

} GammaToneCoeffs, *GammaToneCoeffsPtr;

typedef struct {

	int		cascade;
	double	b1, b2;
	double	a0, a1, a2;
	double	gainLossFactor;
	double	*state;

} TwoPoleCoeffs, *TwoPoleCoeffsPtr;

typedef struct {

	int		cascade;
	double	j;
	double	k;
	double	l;
	double	gainLossFactor;
	double	*state;
	
} BandPassCoeffs, *BandPassCoeffsPtr;

/*
 * Second order segmented Butterworth IIR Filter - see "Digital Methods for
 * Signal Analysis" by K. Beauchamp and C. Yuen, page 261 and previous.
 */

typedef struct {

	double	cC;
	double	twoC;
	double	dD;
	double	eE;
	double	gainLossFactor;
	double	state[FILTERS_NUM_CONTBUTT2_STATE_VARS];

} ContButtCoeffs, *ContButtCoeffsPtr;

/*
 * First order segmented Butterworth IIR Filter - see "Digital Methods for
 * Signal Analysis" by K. Beauchamp and C. Yuen, page 261 and previous.
 */

typedef struct {

	double	gG;
	double	gGG;
	double	hH;
	double	gainLossFactor;
	double	state[FILTERS_NUM_CONTBUTT1_STATE_VARS];

} ContButt1Coeffs, *ContButt1CoeffsPtr;

	
/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern	double	Filters_bess2Poly[];         	/* Bessel coefficents */

extern	double	Filters_butt2Poly[];			/* Butterworth */

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

void	BandPass_Filters(SignalDataPtr theSignal, BandPassCoeffsPtr p[]);

double	BandPassFD_Filters(double freq, double lowerCutOffFreq,
		  double upperCutOffFreq, int order);

void	BrokenStick1Compression_Filters(SignalDataPtr theSignal, double aA,
		  double bB, double cC);

void	BrokenStick1Compression2_Filters(SignalDataPtr theSignal, double *aA,
		  double *bB, double cC);

void	Compression_Filters(SignalDataPtr theSignal, double nrwthr,
		  double nrwcr);

void	FreeBandPassCoeffs_Filters(BandPassCoeffsPtr *p);

void	FreeGammaToneCoeffs_Filters(GammaToneCoeffsPtr *p);

void	FreeTwoPoleCoeffs_Filters(TwoPoleCoeffsPtr *p);

void	GammaTone_Filters(SignalDataPtr theSignal, GammaToneCoeffsPtr p[]);

void	IIR2_Filters(SignalDataPtr theSignal, TwoPoleCoeffsPtr p[]);

void	IIR1Cont_Filters(SignalDataPtr theSignal, ContButt1CoeffsPtr p[]);

void	IIR2ContSingle_Filters(SignalDataPtr theSignal, ContButtCoeffsPtr p);

void	IIR1ContSingle_Filters(SignalDataPtr theSignal, ContButt1CoeffsPtr p);

BandPassCoeffsPtr	InitBandPassCoeffs_Filters(int cascade,
					  double lowerCutOffFreq, double upperCutOffFreq,
					  double dt);

ContButtCoeffsPtr	InitIIR2ContCoeffs_Filters(double cutOffFrequency,
					  double samplingInterval, int highOrLowPass);

ContButt1CoeffsPtr	InitIIR1ContCoeffs_Filters(double cutOffFrequency,
					  double samplingInterval, int highOrLowPass);

GammaToneCoeffsPtr	InitGammaToneCoeffs_Filters(double centreFreq,
					  double bWidth3dB, int cascade, double sampleClk);

TwoPoleCoeffsPtr 	InitIIR2Coeffs_Filters(double *splane, int cascade, 
					  double f3dB, double fs, int low_or_high);
					   
		/* latter generates z plane coefficients from 2 pole blocks, f3dB is
		 * the corner frequency, fs sampling freq, coeffs points to an array
		 * the corner the corner splane contains the coefficients of the
		 * denominator polynomial. */

void	InversePowerCompression_Filters(SignalDataPtr theSignal, double shift,
		  double slope);

void	UptonBStick1Compression_Filters(SignalDataPtr theSignal, double aA,
			double bB, double cC, double dD);

void	ZeroArray_Filters(double *p, int length);

__END_DECLS

#endif
