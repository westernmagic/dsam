/**********************
 *
 * File:		PhaseFreqFuncs.h
 * Purpose:		This program calculates the phase functions for a simulation and
 *				different stimulus frequencies.
 * Comments:	Written using ModuleProducer version 1.2.7 (Jun 26 2000).
 * Author:		L. P. O'Mard
 * Created:		10 Jul 2000
 * Updated:	
 * Copyright:	(c) 2000, University of Essex.
 *
 *********************/

#ifndef _PHASEFREQFUNCS_H
#define _PHASEFREQFUNCS_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define PHASEFREQFUNCS_NUM_PARS			11

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	PHASEFREQFUNCS_ENABLED,
	PHASEFREQFUNCS_OUTPUTMODE,
	PHASEFREQFUNCS_ANALYSISOUTPUTMODE,
	PHASEFREQFUNCS_NUMFREQUENCIES,
	PHASEFREQFUNCS_STIMULUSDURATION,
	PHASEFREQFUNCS_SAMPLINGINTERVAL,
	PHASEFREQFUNCS_INITIALFREQUENCY,
	PHASEFREQFUNCS_DELTAFREQUENCY,
	PHASEFREQFUNCS_NUMINTENSITIES,
	PHASEFREQFUNCS_INITIALINTENSITY,
	PHASEFREQFUNCS_DELTAINTENSITY

} PFFParsParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	enabledFlag, outputModeFlag, analysisOutputModeFlag;
	BOOLN	numFrequenciesFlag, stimulusDurationFlag, dtFlag;
	BOOLN	initialFrequencyFlag, deltaFrequencyFlag, numIntensitiesFlag;
	BOOLN	initialIntensityFlag, deltaIntensityFlag;
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

	/* Private members */
	char	outputFile[MAX_FILE_PATH];
	NameSpecifier	*outputModeList;
	NameSpecifier	*analysisOutputModeList;
	EarObjectPtr	audModel;
	EarObjectPtr	stimulus;
	EarObjectPtr	analysis;
	EarObjectPtr	stimulusAnalysis;
	EarObjectPtr	resultEarObj;

} PFFPars, *PFFParsPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

extern	PFFParsPtr	pFFParsPtr;

__END_DECLS

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckPars_PhaseFreqFuncs(void);

void	FreeProcessVariables_PhaseFreqFunc(void);

BOOLN	Free_PhaseFreqFuncs(void);

BOOLN	InitProcessVariables_PhaseFreqFuncs(EarObjectPtr data);

BOOLN	Init_PhaseFreqFuncs(ParameterSpecifier parSpec);

BOOLN	PrintPars_PhaseFreqFuncs(void);

BOOLN	ReadPars_PhaseFreqFuncs(FILE *fp);

BOOLN	RunProcess_PhaseFreqFuncs(EarObjectPtr data);

BOOLN	SetAnalysisOutputMode_PhaseFreqFuncs(char * theAnalysisOutputMode);

BOOLN	SetDeltaFrequency_PhaseFreqFuncs(double theDeltaFrequency);

BOOLN	SetDeltaIntensity_PhaseFreqFuncs(double theDeltaIntensity);

BOOLN	SetEnabled_PhaseFreqFuncs(char * theEnabled);

BOOLN	SetInitialFrequency_PhaseFreqFuncs(double theInitialFrequency);

BOOLN	SetInitialIntensity_PhaseFreqFuncs(double theInitialIntensity);

BOOLN	SetNumFrequencies_PhaseFreqFuncs(int theNumFrequencies);

BOOLN	SetNumIntensities_PhaseFreqFuncs(int theNumIntensities);

BOOLN	SetOutputMode_PhaseFreqFuncs(char * theOutputMode);

BOOLN	SetPars_PhaseFreqFuncs(char * enabled, char * outputMode,
		  char * analysisOutputMode, int numFrequencies,
		  double stimulusDuration, double samplingInterval,
		  double initialFrequency, double deltaFrequency, int numIntensities,
		  double initialIntensity, double deltaIntensity);

BOOLN	SetSamplingInterval_PhaseFreqFuncs(double theSamplingInterval);

BOOLN	SetStimulusDuration_PhaseFreqFuncs(double theStimulusDuration);

BOOLN	SetUniParList_PhaseFreqFuncs(UniParPtr pars);

__END_DECLS

#endif
