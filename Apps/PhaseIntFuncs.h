/**********************
 *
 * File:		PhaseIntFuncs.h
 * Purpose:		This program calculates the phase functions for a simulation and
 *				different stimulus frequencies.
 * Comments:	Written using ModuleProducer version 1.2.7 (Jun 26 2000).
 * Author:		L. P. O'Mard
 * Created:		10 Jul 2000
 * Updated:	
 * Copyright:	(c) 2000, University of Essex.
 *
 *********************/

#ifndef _PHASEINTFUNCS_H
#define _PHASEINTFUNCS_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define PHASEINTFUNCS_NUM_PARS			11

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	PHASEINTFUNCS_ENABLED,
	PHASEINTFUNCS_OUTPUTMODE,
	PHASEINTFUNCS_ANALYSISOUTPUTMODE,
	PHASEINTFUNCS_NUMFREQUENCIES,
	PHASEINTFUNCS_STIMULUSDURATION,
	PHASEINTFUNCS_SAMPLINGINTERVAL,
	PHASEINTFUNCS_INITIALFREQUENCY,
	PHASEINTFUNCS_DELTAFREQUENCY,
	PHASEINTFUNCS_NUMINTENSITIES,
	PHASEINTFUNCS_INITIALINTENSITY,
	PHASEINTFUNCS_DELTAINTENSITY

} PIFParsParSpecifier;

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

} PIFPars, *PIFParsPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

extern	PIFParsPtr	pIFParsPtr;

__END_DECLS

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckPars_PhaseIntFuncs(void);

void	FreeProcessVariables_PhaseIntFunc(void);

BOOLN	Free_PhaseIntFuncs(void);

BOOLN	InitAnalysisProcessList_PhaseIntFunc(void);

BOOLN	InitAnalysisOutputModeList_PhaseIntFuncs(void);

BOOLN	InitEnabledList_PhaseIntFuncs(void);

BOOLN	Init_PhaseIntFuncs(ParameterSpecifier parSpec);

BOOLN	PrintPars_PhaseIntFuncs(void);

BOOLN	ReadPars_PhaseIntFuncs(FILE *fp);

BOOLN	RunProcess_PhaseIntFuncs(EarObjectPtr data);

BOOLN	SetAnalysisOutputMode_PhaseIntFuncs(char * theAnalysisOutputMode);

BOOLN	SetDeltaFrequency_PhaseIntFuncs(double theDeltaFrequency);

BOOLN	SetDeltaIntensity_PhaseIntFuncs(double theDeltaIntensity);

BOOLN	SetEnabled_PhaseIntFuncs(char * theEnabled);

BOOLN	SetInitialFrequency_PhaseIntFuncs(double theInitialFrequency);

BOOLN	SetInitialIntensity_PhaseIntFuncs(double theInitialIntensity);

BOOLN	SetNumFrequencies_PhaseIntFuncs(int theNumFrequencies);

BOOLN	SetNumIntensities_PhaseIntFuncs(int theNumIntensities);

BOOLN	SetOutputMode_PhaseIntFuncs(char * theOutputMode);

BOOLN	SetPars_PhaseIntFuncs(char * enabled, char * outputMode,
		  char * analysisOutputMode, int numFrequencies,
		  double stimulusDuration, double samplingInterval,
		  double initialFrequency, double deltaFrequency, int numIntensities,
		  double initialIntensity, double deltaIntensity);

BOOLN	SetSamplingInterval_PhaseIntFuncs(double theSamplingInterval);

BOOLN	SetStimulusDuration_PhaseIntFuncs(double theStimulusDuration);

BOOLN	SetUniParList_PhaseIntFuncs(UniParPtr pars);

__END_DECLS

#endif
