/**********************
 *
 * File:		TwoTSRatio.h
 * Purpose:		Two tone suppression analysis.
 * Comments:	Written using ModuleProducer version 1.2.9 (Aug 10 2000).
 * Author:		L. P. O'Mard
 * Created:		11 Aug 2000
 * Updated:	
 * Copyright:	(c) 2000, CNBH, University of Essex.
 *
 *********************/

#ifndef _TWOTSRATIO_H
#define _TWOTSRATIO_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define TWOTSUPPRATIO_NUM_PARS			11

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	TWOTSUPPRATIO_ENABLED,
	TWOTSUPPRATIO_OUTPUTMODE,
	TWOTSUPPRATIO_T2SCALE,
	TWOTSUPPRATIO_NUMT1INTENSITIES,
	TWOTSUPPRATIO_INITIALT1INTENSITY,
	TWOTSUPPRATIO_DELTAT1INTENSITY,
	TWOTSUPPRATIO_NUMT2INTENSITIES,
	TWOTSUPPRATIO_INITIALT2INTENSITY,
	TWOTSUPPRATIO_DELTAT2INTENSITY,
	TWOTSUPPRATIO_STIMULUSDURATION,
	TWOTSUPPRATIO_SAMPLINGINTERVAL

} TTSRParsParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	enabledFlag, outputModeFlag, t2ScaleFlag;
	BOOLN	numT1IntensitiesFlag, initialT1IntensityFlag, deltaT1IntensityFlag;
	BOOLN	numT2IntensitiesFlag, initialT2IntensityFlag, deltaT2IntensityFlag;
	BOOLN	stimulusDurationFlag, dtFlag;
	int		enabled;
	int		outputMode;
	double	t2Scale;
	int		numT1Intensities;
	double	initialT1Intensity;
	double	deltaT1Intensity;
	int		numT2Intensities;
	double	initialT2Intensity;
	double	deltaT2Intensity;
	double	stimulusDuration;
	double	dt;

	/* Private members */
	char	outputFile[MAX_FILE_PATH];
	double	t1Frequency;
	NameSpecifier	*outputModeList;
	EarObjectPtr	audModel;
	EarObjectPtr	stimulus[2];
	EarObjectPtr	analysis;
	EarObjectPtr	resultEarObj;

} TTSRPars, *TTSRParsPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

extern	TTSRParsPtr	tTSRParsPtr;

__END_DECLS

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckData_TwoTSuppRatio(EarObjectPtr data);

BOOLN	CheckPars_TwoTSuppRatio(void);

BOOLN	Free_TwoTSuppRatio(void);

BOOLN	InitOutputModeList_TwoTSuppRatio(void);

BOOLN	Init_TwoTSuppRatio(ParameterSpecifier parSpec);

BOOLN	PrintPars_TwoTSuppRatio(void);

BOOLN	ReadPars_TwoTSuppRatio(FILE *fp);

BOOLN	RunProcess_TwoTSuppRatio(EarObjectPtr data);

BOOLN	RunToneAnalysis_TwoTSuppRatio(int stimulusIndex);

BOOLN	SetDeltaT1Intensity_TwoTSuppRatio(double theDeltaT1Intensity);

BOOLN	SetDeltaT2Intensity_TwoTSuppRatio(double theDeltaT2Intensity);

BOOLN	SetEnabled_TwoTSuppRatio(char * theEnabled);

BOOLN	SetInitialT1Intensity_TwoTSuppRatio(double theInitialT1Intensity);

BOOLN	SetInitialT2Intensity_TwoTSuppRatio(double theInitialT2Intensity);

BOOLN	SetNumT1Intensities_TwoTSuppRatio(int theNumT1Intensities);

BOOLN	SetNumT2Intensities_TwoTSuppRatio(int theNumT2Intensities);

BOOLN	SetOutputMode_TwoTSuppRatio(char * theOutputMode);

BOOLN	SetPars_TwoTSuppRatio(char * enabled, char * outputMode,
		  double t2Scale, int numT1Intensities, double initialT1Intensity,
		  double deltaT1Intensity, int numT2Intensities,
		  double initialT2Intensity, double deltaT2Intensity,
		  double stimulusDuration, double samplingInterval);

BOOLN	SetSamplingInterval_TwoTSuppRatio(double theSamplingInterval);

BOOLN	SetStimulusDuration_TwoTSuppRatio(double theStimulusDuration);

BOOLN	SetT2Scale_TwoTSuppRatio(double theT2Scale);

BOOLN	SetUniParList_TwoTSuppRatio(UniParPtr pars);

__END_DECLS

#endif
