/**********************
 *
 * File:		TuningCurve.h
 * Purpose:		This module runs a tuning curve test on a simulation.
 * Comments:	It was created to be used with the feval program.
 * Author:		L. P. O'Mard
 * Created:		28 Jun 2000
 * Updated:		
 * Copyright:	(c) 2000, University of Essex
 *
 *********************/

#ifndef TUNINGCURVE_H
#define TUNINGCURVE_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define TUNINGCURVE_NUM_PARS		11
#define TUNINGCURVE_MOD_NAME		wxT("TuningCurve")
#define SAMPLING_FREQ_MULTIPLIER	20.0	/* To set optimum lowest value. */
#define	TUNINGCURVE_INITIAL_STEP	10.0

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	TUNINGCURVE_ENABLED,
	TUNINGCURVE_OUTPUTMODE,
	TUNINGCURVE_ANALYSIS_PROCESS,
	TUNINGCURVE_THRESHOLDMODE,
	TUNINGCURVE_NUMBEROFFREQS,
	TUNINGCURVE_STIMULUSDURATION,
	TUNINGCURVE_LOWESTFREQUENCY,
	TUNINGCURVE_HIGHESTFREQUENCY,
	TUNINGCURVE_MINIMUMFREQ,
	TUNINGCURVE_TARGETTHRESHOLD,
	TUNINGCURVE_ACCURACY

} TCParsParSpecifier;

typedef enum {

	TUNINGCURVE_ANALYSIS_PROCESS_INTENSITY,
	TUNINGCURVE_ANALYSIS_PROCESS_FINDBIN,
	TUNINGCURVE_ANALYSIS_PROCESS_NULL

} TuningCurveAnalysisProcessSpecifier;

typedef enum {

	TUNINGCURVE_THRESHOLDMODE_GRADIENT,
	TUNINGCURVE_THRESHOLDMODE_STEP_DIVIDE,
	TUNINGCURVE_THRESHOLDMODE_STEP_DIVIDE2,
	TUNINGCURVE_THRESHOLDMODE_NULL

} TuningCurveThresholdModeSpecifier;

typedef struct {

	ParameterSpecifier parSpec;
	
	BOOLN	enabledFlag, outputModeFlag, analysisProcessFlag, thresholdModeFlag;
	BOOLN	numberOfFreqsFlag, stimulusDurationFlag, lowestFrequencyFlag;
	BOOLN	highestFrequencyFlag, minimumFreqFlag, targetThresholdFlag;
	BOOLN	accuracyFlag;
	int		enabled;
	int		outputMode;
	int		analysisProcess;
	int		thresholdMode;
	int		numberOfFreqs;
	double	stimulusDuration;
	double	lowestFrequency;
	double	highestFrequency;
	double	minimumFreq;
	double	targetThreshold;
	double	accuracy;

	/* Private members */
	WChar	outputFile[MAX_FILE_PATH];
	NameSpecifier	*analysisProcessList;
	NameSpecifier	*outputModeList;
	NameSpecifier	*thresholdModeList;
	UniParListPtr	parList;
	CFListPtr	frequencies;
	EarObjectPtr	audModel;
	EarObjectPtr	stimulus;
	EarObjectPtr	analysis;
	EarObjectPtr	resultEarObj;

}  TCPars, *TCParsPtr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

extern	TCParsPtr	tCParsPtr;

__END_DECLS

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckPars_TuningCurve(void);

BOOLN	Free_TuningCurve(void);

void	FreeProcessVariables_TuningCurve(void);

BOOLN	InitAnalysisProcessList_TuningCurve(void);

BOOLN	InitModule_TuningCurve(ModulePtr theModule);

BOOLN	InitOutputModeList_TuningCurve(void);

BOOLN	InitProcessVariables_TuningCurve(EarObjectPtr simProcess);

BOOLN	InitThresholdModeList_TuningCurve(void);

BOOLN	Init_TuningCurve(ParameterSpecifier parSpec);

BOOLN	PrintPars_TuningCurve(void);

BOOLN	RunProcess_TuningCurve(EarObjectPtr simProcess);

BOOLN	SetAnalysisProcess_TuningCurve(WChar * theAnalysisProcess);

BOOLN	SetAccuracy_TuningCurve(double theAccuracy);

BOOLN	SetEnabled_TuningCurve(WChar * theEnabled);

BOOLN	SetHighestFrequency_TuningCurve(double theHighestFrequency);

BOOLN	SetIntensity_TuningCurve(double intensity);

BOOLN	SetLowestFrequency_TuningCurve(double theLowestFrequency);

BOOLN	SetMinimumFreq_TuningCurve(double theMinimumFreq);

BOOLN	SetNumberOfFreqs_TuningCurve(int theNumberOfFreqs);

BOOLN	SetOutputMode_TuningCurve(WChar * theOutputMode);

BOOLN	SetStimulusDuration_TuningCurve(double theStimulusDuration);

BOOLN	SetTargetThreshold_TuningCurve(double theTargetThreshold);

BOOLN	SetThresholdMode_TuningCurve(WChar * theThresholdMode);

BOOLN	SetUniParList_TuningCurve(void);

__END_DECLS

#endif
