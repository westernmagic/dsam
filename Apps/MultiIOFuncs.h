/**********************
 *
 * File:		MultiIOFuncs.h
 * Purpose:		This program calculates the I/O functions for a simulation and
 *				different stimulus frequencies.
 * Comments:	Written using ModuleProducer version 1.2.7 (Jun 26 2000).
 * Author:		L. P. O'Mard
 * Created:		03 Jul 2000
 * Updated:	
 * Copyright:	(c) 2000, University of Essex.
 *
 *********************/

#ifndef _MULTIIOFUNCS_H
#define _MULTIIOFUNCS_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define MULTIIOFUNCS_NUM_PARS			11
#define MULTIIOFUNCS_MOD_NAME			wxT("MultiIOFuncs")

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	MULTIIOFUNCS_ENABLED,
	MULTIIOFUNCS_OUTPUTMODE,
	MULTIIOFUNCS_ANALYSISPROCESS,
	MULTIIOFUNCS_NUMFREQUENCIES,
	MULTIIOFUNCS_STIMULUSDURATION,
	MULTIIOFUNCS_SAMPLINGINTERVAL,
	MULTIIOFUNCS_INITIALFREQUENCY,
	MULTIIOFUNCS_DELTAFREQUENCY,
	MULTIIOFUNCS_NUMINTENSITIES,
	MULTIIOFUNCS_INITIALINTENSITY,
	MULTIIOFUNCS_DELTAINTENSITY

} MIOParsParSpecifier;

typedef enum {

	MULTIIOFUNCS_ANALYSIS_PROCESS_INTENSITY,
	MULTIIOFUNCS_ANALYSIS_PROCESS_FINDBIN,
	MULTIIOFUNCS_ANALYSIS_PROCESS_NULL

} MultiIOFuncsAnalysisProcessSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	enabledFlag, outputModeFlag, analysisProcessFlag;
	BOOLN	numFrequenciesFlag, stimulusDurationFlag, dtFlag;
	BOOLN	initialFrequencyFlag, deltaFrequencyFlag, numIntensitiesFlag;
	BOOLN	initialIntensityFlag, deltaIntensityFlag;
	int		enabled;
	int		outputMode;
	int		analysisProcess;
	int		numFrequencies;
	double	stimulusDuration;
	double	dt;
	double	initialFrequency;
	double	deltaFrequency;
	int		numIntensities;
	double	initialIntensity;
	double	deltaIntensity;

	/* Private members */
	WChar	outputFile[MAX_FILE_PATH];
	NameSpecifier	*outputModeList;
	NameSpecifier	*analysisProcessList;
	UniParListPtr	parList;
	EarObjectPtr	audModel;
	EarObjectPtr	stimulus;
	EarObjectPtr	analysis;
	EarObjectPtr	resultEarObj;

} MIOPars, *MIOParsPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

extern	MIOParsPtr	mIOParsPtr;

__END_DECLS

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckPars_MultiIOFuncs(void);

BOOLN	Free_MultiIOFuncs(void);

void	FreeProcessVariables_MultiIOFuncs(void);

BOOLN	InitAnalysisProcessList_MultiIOFuncs(void);

BOOLN	InitEnabledList_MultiIOFuncs(void);

BOOLN	InitModule_MultiIOFuncs(ModulePtr theModule);

BOOLN	InitOutputModeList_MultiIOFuncs(void);

BOOLN	InitProcessVariables_MultiIOFuncs(EarObjectPtr simProcess);

BOOLN	Init_MultiIOFuncs(ParameterSpecifier parSpec);

BOOLN	PrintPars_MultiIOFuncs(void);

BOOLN	RunProcess_MultiIOFuncs(EarObjectPtr data);

BOOLN	SetAnalysisProcess_MultiIOFuncs(WChar * theAnalysisProcess);

BOOLN	SetDeltaFrequency_MultiIOFuncs(double theDeltaFrequency);

BOOLN	SetDeltaIntensity_MultiIOFuncs(double theDeltaIntensity);

BOOLN	SetEnabled_MultiIOFuncs(WChar * theEnabled);

BOOLN	SetInitialFrequency_MultiIOFuncs(double theInitialFrequency);

BOOLN	SetInitialIntensity_MultiIOFuncs(double theInitialIntensity);

BOOLN	SetNumFrequencies_MultiIOFuncs(int theNumFrequencies);

BOOLN	SetNumIntensities_MultiIOFuncs(int theNumIntensities);

BOOLN	SetOutputMode_MultiIOFuncs(WChar * theOutputMode);

BOOLN	SetSamplingInterval_MultiIOFuncs(double theSamplingInterval);

BOOLN	SetStimulusDuration_MultiIOFuncs(double theStimulusDuration);

BOOLN	SetUniParList_MultiIOFuncs(void);

__END_DECLS

#endif
