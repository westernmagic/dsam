/**********************
 *
 * File:		StWhiteNoise.h
 * Purpose:		This module contains the methods for the simple white-noise
 *				signal generation paradigm.
 * Comments:	
 * Author:		E. A. Lopez-Poveda
 * Created:		17 Jan 1995
 * Updated:		12 Mar 1997
 * Copyright:	(c) 1998, University of Essex
 *
 **********************/

#ifndef	_STWHITENOISE_H
#define _STWHITENOISE_H	1
 
/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define WHITENOISE_NUM_PARS			7

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	WHITENOISE_NUMCHANNELS,
	WHITENOISE_CORRELATIONDEGREE,
	WHITENOISE_RANDOMIZATIONINDEX,
	WHITENOISE_RANSEED,
	WHITENOISE_INTENSITY,
	WHITENOISE_DURATION,
	WHITENOISE_SAMPLINGINTERVAL

} WhiteNoiseParSpecifier;

typedef struct {

	ParameterSpecifier parSpec;

	BOOLN	numChannelsFlag, durationFlag, dtFlag, intensityFlag;
	BOOLN	correlationDegreeFlag, randomizationIndexFlag, ranSeedFlag;
	uShort	numChannels;
	double	intensity;
	double	duration;
	double	dt;
	int		correlationDegree;
	int		randomizationIndex;
	long	ranSeed;
	
	/* Private members */
	UniParListPtr	parList;

} WhiteNoise, *WhiteNoisePtr;
	
/********************************* External Variables *************************/

extern	WhiteNoisePtr	whiteNoisePtr;

/********************************* Function Prototypes ************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckPars_WhiteNoise(void);

BOOLN	Free_WhiteNoise(void);

BOOLN	GenerateSignal_WhiteNoise(EarObjectPtr theObject);

BOOLN	Init_WhiteNoise(ParameterSpecifier parSpec);

UniParListPtr	GetUniParListPtr_WhiteNoise(void);

BOOLN	PrintPars_WhiteNoise(void);

BOOLN	ReadPars_WhiteNoise(char *fileName);

BOOLN	InitModule_WhiteNoise(ModulePtr theModule);

BOOLN	SetNumChannels_WhiteNoise(int theNumChannels);

BOOLN	SetDuration_WhiteNoise(double theDuration);

BOOLN	SetIntensity_WhiteNoise(double theIntensity);

BOOLN	SetParsPointer_WhiteNoise(ModulePtr theModule);

BOOLN	SetPars_WhiteNoise(int numChannels, double theIntensity,
		  double theDuration, double theSamplingInterval, int theCorrelationDegree,
		  int theRandomizationIndex, long theRanSeed);
		  
BOOLN	SetSamplingInterval_WhiteNoise(double theSamplingInterval);

BOOLN	SetCorrelationDegree_WhiteNoise(int theCorrelationDegree);

BOOLN	SetRandomizationIndex_WhiteNoise(int theRandomizationIndex);

BOOLN	SetRanSeed_WhiteNoise(long theRanSeed);

BOOLN	SetUniParList_WhiteNoise(void);

__END_DECLS

#endif
