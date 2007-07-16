/**********************
 *
 * File:		RelPhaseFreqFuncs.h
 * Purpose:		
 * Comments:	Written using ModuleProducer version 1.2.10 (Jan  9 2001).
 * Author:		
 * Created:		09 Jan 2001
 * Updated:	
 * Copyright:	(c) 2001, 
 *
 *********************/

#ifndef _RELPHASEFREQFUNCS_H
#define _RELPHASEFREQFUNCS_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define RELPHASEFREQFUNCS_NUM_PARS			12

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	RELPHASEFREQFUNCS_ENABLED,
	RELPHASEFREQFUNCS_OUTPUTMODE,
	RELPHASEFREQFUNCS_ANALYSISOUTPUTMODE,
	RELPHASEFREQFUNCS_STIMULUSDURATION,
	RELPHASEFREQFUNCS_SAMPLINGINTERVAL,
	RELPHASEFREQFUNCS_NUMFREQUENCIES,
	RELPHASEFREQFUNCS_INITIALFREQUENCY,
	RELPHASEFREQFUNCS_DELTAFREQUENCY,
	RELPHASEFREQFUNCS_NUMINTENSITIES,
	RELPHASEFREQFUNCS_INITIALINTENSITY,
	RELPHASEFREQFUNCS_DELTAINTENSITY,
	RELPHASEFREQFUNCS_REFINTENSITY

} RPFFParsParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	enabledFlag, outputModeFlag, analysisOutputModeFlag;
	BOOLN	numFrequenciesFlag, stimulusDurationFlag, dtFlag;
	BOOLN	initialFrequencyFlag, deltaFrequencyFlag, numIntensitiesFlag;
	BOOLN	initialIntensityFlag, deltaIntensityFlag, refIntensityFlag;
	int		enabled;
	int		outputMode;
	int		analysisOutputMode;
	int		numFrequencies;
	double	stimulusDuration;
	double	dt;
	double	initialFrequency;
	double	deltaFrequency;
	int		numIntensities;
	double	initialIntensity;
	double	deltaIntensity;
	double	refIntensity;

	/* Private members */
	char	outputFile[MAX_FILE_PATH];
	int		refIntIndex;
	NameSpecifier	*outputModeList;
	NameSpecifier	*analysisOutputModeList;
	EarObjectPtr	audModel;
	EarObjectPtr	stimulus;
	EarObjectPtr	analysis;
	EarObjectPtr	stimulusAnalysis;
	EarObjectPtr	resultEarObj;

} RPFFPars, *RPFFParsPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

extern	RPFFParsPtr	rPFFParsPtr;

__END_DECLS

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckPars_RelPhaseFreqFuncs(void);

BOOLN	Free_RelPhaseFreqFuncs(void);

void	FreeProcessVariables_RelPhaseFreqFunc(void);

UniParListPtr	GetUniParListPtr_RelPhaseFreqFuncs(void);

BOOLN	InitProcessVariables_RelPhaseFreqFuncs(EarObjectPtr data);

BOOLN	Init_RelPhaseFreqFuncs(ParameterSpecifier parSpec);

BOOLN	PrintPars_RelPhaseFreqFuncs(void);

BOOLN	ReadPars_RelPhaseFreqFuncs(FILE *fp);

BOOLN	RunProcess_RelPhaseFreqFuncs(EarObjectPtr data);

BOOLN	SetAnalysisOutputMode_RelPhaseFreqFuncs(char * theAnalysisOutputMode);

BOOLN	SetDeltaFrequency_RelPhaseFreqFuncs(double theDeltaFrequency);

BOOLN	SetDeltaIntensity_RelPhaseFreqFuncs(double theDeltaIntensity);

BOOLN	SetEnabled_RelPhaseFreqFuncs(char * theEnabled);

BOOLN	SetInitialFrequency_RelPhaseFreqFuncs(double theInitialFrequency);

BOOLN	SetInitialIntensity_RelPhaseFreqFuncs(double theInitialIntensity);

BOOLN	SetNumFrequencies_RelPhaseFreqFuncs(int theNumFrequencies);

BOOLN	SetNumIntensities_RelPhaseFreqFuncs(int theNumIntensities);

BOOLN	SetOutputMode_RelPhaseFreqFuncs(char * theOutputMode);

BOOLN	SetPars_RelPhaseFreqFuncs(char * enabled, char * outputMode,
		  char * analysisOutputMode, int numFrequencies,
		  double stimulusDuration, double samplingInterval,
		  double initialFrequency, double deltaFrequency, int numIntensities,
		  double initialIntensity, double deltaIntensity,
		  double refIntensity);

BOOLN	SetRefIntensity_RelPhaseFreqFuncs(double theRefIntensity);

BOOLN	SetSamplingInterval_RelPhaseFreqFuncs(double theSamplingInterval);

BOOLN	SetStimulusDuration_RelPhaseFreqFuncs(double theStimulusDuration);

BOOLN	SetUniParList_RelPhaseFreqFuncs(UniParPtr pars);

__END_DECLS

#endif
