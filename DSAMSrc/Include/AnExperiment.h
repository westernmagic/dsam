/**********************
 *
 * File:		AnExperiment.c
 * Purpose:		This module contains various analysis routines, which have
 *				been used in real experiments.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		19 Mar 1993
 * Updated:		10 Oct 1996
 * Copyright:	(c) University of Essex
 *
 **********************/

#ifndef	_ANEXPERIMENT_H
#define _ANEXPERIMENT_H	1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define ANALYSIS_FIRST_INTENSITY_GUESS 50.0	/* Initial threshold intensity. */
#define MAXIMUM_THRESHOLD_LOOP		20	/* - FindThresholdIntensity routine. */
    
/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	Calc2CompAdapt_ExpAnalysis(double Tst[], double Tr[], EarObjectPtr data,
		  double shortTermT1, double shortTermPeriod, double rapidAdaptPeriod);

double	CalcQValue_ExpAnalysis( EarObjectPtr (* RunModel)(void),
		  BOOLN (* SetStimulusFreq)(double), double centreFreq,
		  double variationFromCF, double dBDownDiff,
		  double initialFreqIncrement, double accuracy );

double	FindThresholdIntensity_ExpAnalysis( EarObjectPtr (* RunModel)(void),
		  BOOLN (* SetIntensity)(double), double targetThreshold,
		  double targetAccuracy, BOOLN *firstPass );

double	FindThresholdIntensity_ExpAnalysis_Slow(EarObjectPtr (* RunModel)(void),
		  BOOLN (* SetIntensity)(double), double targetThreshold,
		  double targetAccuracy, double initialDeltaIntensity);

__END_DECLS

#endif
