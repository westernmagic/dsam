/**********************
 *
 * File:		FilterShape.h
 * Purpose:		
 * Comments:	Written using ModuleProducer version 1.2.8 (Jul 13 2000).
 * Author:		
 * Created:		13 Jul 2000
 * Updated:	
 * Copyright:	(c) 2000, 
 *
 *********************/

#ifndef _FILTERSHAPE_H
#define _FILTERSHAPE_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define FILTERSHAPE_NUM_PARS		12
#define FILTERSHAPE_MOD_NAME		wxT("FilterShape")

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	FILTERSHAPE_ENABLED,
	FILTERSHAPE_OUTPUTMODE,
	FILTERSHAPE_ANALYSISPROCESS,
	FILTERSHAPE_ANALYSISOUTPUTMODE,
	FILTERSHAPE_NUMBEROFFREQS,
	FILTERSHAPE_STIMULUSDURATION,
	FILTERSHAPE_SAMPLINGINTERVAL,
	FILTERSHAPE_LOWESTFREQUENCY,
	FILTERSHAPE_HIGHESTFREQUENCY,
	FILTERSHAPE_NUMINTENSITIES,
	FILTERSHAPE_INITIALINTENSITY,
	FILTERSHAPE_DELTAINTENSITY

} FSParsParSpecifier;

typedef enum {

	FILTERSHAPE_ANALYSIS_PROCESS_INTENSITY,
	FILTERSHAPE_ANALYSIS_PROCESS_FINDBIN,
	FILTERSHAPE_ANALYSIS_PROCESS_NULL

} FilterShapeAnalysisProcessSpecifier;

typedef enum {

	FILTERSHAPE_NORMALISED_ANALYSISOUTPUTMODE,
	FILTERSHAPE_STANDARD_ANALYSISOUTPUTMODE,
	FILTERSHAPE_ANALYSISOUTPUTMODE_NULL

} FilterShapeAnalysisOutputModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	enabledFlag, outputModeFlag, analysisProcessFlag;
	BOOLN	analysisOutputModeFlag, numberOfFreqsFlag, stimulusDurationFlag;
	BOOLN	dtFlag, lowestFrequencyFlag, highestFrequencyFlag;
	BOOLN	numIntensitiesFlag, initialIntensityFlag, deltaIntensityFlag;
	int		enabled;
	int		outputMode;
	int		analysisProcess;
	int		analysisOutputMode;
	int		numberOfFreqs;
	double	stimulusDuration;
	double	dt;
	double	lowestFrequency;
	double	highestFrequency;
	int		numIntensities;
	double	initialIntensity;
	double	deltaIntensity;

	/* Private members */
	WChar	outputFile[MAX_FILE_PATH];
	NameSpecifier	*outputModeList;
	NameSpecifier	*analysisProcessList;
	NameSpecifier	*analysisOutputModeList;
	UniParListPtr	parList;
	CFListPtr	frequencies;
	EarObjectPtr	audModel;
	EarObjectPtr	stimulus;
	EarObjectPtr	analysis;
	EarObjectPtr	resultEarObj;

} FSPars, *FSParsPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

extern	FSParsPtr	fSParsPtr;

__END_DECLS

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckPars_FilterShape(void);

BOOLN	InitModule_FilterShape(ModulePtr theModule);

BOOLN	Free_FilterShape(void);

void	FreeProcessVariables_FilterShape(void);

BOOLN	InitAnalysisOutputModeList_FilterShape(void);

BOOLN	InitAnalysisProcessList_FilterShape(void);

BOOLN	InitOutputModeList_FilterShape(void);

BOOLN	InitProcessVariables_FilterShape(EarObjectPtr simProcess);

BOOLN	Init_FilterShape(ParameterSpecifier parSpec);

BOOLN	PrintPars_FilterShape(void);

BOOLN	RunProcess_FilterShape(EarObjectPtr data);

BOOLN	SetAnalysisOutputMode_FilterShape(WChar * theAnalysisOutputMode);

BOOLN	SetAnalysisProcess_FilterShape(WChar * theAnalysisProcess);

BOOLN	SetDeltaIntensity_FilterShape(double theDeltaIntensity);

BOOLN	SetEnabled_FilterShape(WChar * theEnabled);

BOOLN	SetHighestFrequency_FilterShape(double theHighestFrequency);

BOOLN	SetInitialIntensity_FilterShape(double theInitialIntensity);

BOOLN	SetLowestFrequency_FilterShape(double theLowestFrequency);

BOOLN	SetNumIntensities_FilterShape(int theNumIntensities);

BOOLN	SetNumberOfFreqs_FilterShape(int theNumberOfFreqs);

BOOLN	SetOutputMode_FilterShape(WChar * theOutputMode);

BOOLN	SetSamplingInterval_FilterShape(double theSamplingInterval);

BOOLN	SetStimulusDuration_FilterShape(double theStimulusDuration);

BOOLN	SetUniParList_FilterShape(void);

__END_DECLS

#endif
