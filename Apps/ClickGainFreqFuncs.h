/**********************
 *
 * File:		ClickGainFreqFuncs.h
 * Purpose:		Calculates the click gain/frequency response using an FFT.
 * Comments:	Written using ModuleProducer version 1.2.10 (Jan  9 2001).
 *				The sampling interval parameter is not used for the simulation
 *				file stimulus option.
 * Author:		L. P. O'Mard
 * Created:		12 Jan 2001
 * Updated:	
 * Copyright:	(c) 2001, CNBH, University of Essex.
 *
 *********************/

#ifndef _CLICKGAINFREQFUNCS_H
#define _CLICKGAINFREQFUNCS_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define CLICKGAINFREQFUNCS_NUM_PARS			11

#define	DEFAULT_CLICK_MAGNITUDE				964.29

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	CLICKGAINFREQFUNCS_ENABLED,
	CLICKGAINFREQFUNCS_OUTPUTMODE,
	CLICKGAINFREQFUNCS_CLICKMODE,
	CLICKGAINFREQFUNCS_ANALYSISOUTPUTMODE,
	CLICKGAINFREQFUNCS_CLICKTIME,
	CLICKGAINFREQFUNCS_STIMULUSDURATION,
	CLICKGAINFREQFUNCS_SAMPLINGINTERVAL,
	CLICKGAINFREQFUNCS_DBAJDUSTMENT,
	CLICKGAINFREQFUNCS_NUMINTENSITIES,
	CLICKGAINFREQFUNCS_INITIALINTENSITY,
	CLICKGAINFREQFUNCS_DELTAINTENSITY

} CGFFParsParSpecifier;

typedef enum {

	CLICKGAINFREQFUNCS_CLICKMODE_SIMPLE,
	CLICKGAINFREQFUNCS_FILE_MODE,
	CLICKGAINFREQFUNCS_CLICKMODE_NULL

} ClickGainFreqFuncsClickModeSpecifier;

typedef enum {

	CLICKGAINFREQFUNCS_ANALYSISOUTPUTMODE_ABSOLUTE,
	CLICKGAINFREQFUNCS_ANALYSISOUTPUTMODE_RELATIVE,
	CLICKGAINFREQFUNCS_ANALYSISOUTPUTMODE_NULL

} ClickGainFreqFuncsAnalysisOutputModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	enabledFlag, outputModeFlag, clickModeFlag, analysisOutputModeFlag;
	BOOLN	clickTimeFlag, dBAdjustmentFlag, stimulusDurationFlag, dtFlag;
	BOOLN	numIntensitiesFlag, initialIntensityFlag, deltaIntensityFlag;
	int		enabled;
	int		outputMode;
	int		clickMode;
	int		analysisOutputMode;
	double	clickTime;
	double	stimulusDuration;
	double	dt;
	double	dBAdjustment;
	int		numIntensities;
	double	initialIntensity;
	double	deltaIntensity;

	/* Private members */
	char	outputFile[MAX_FILE_PATH];
	char	stimFileName[MAX_FILE_PATH];
	NameSpecifier	*outputModeList;
	NameSpecifier	*clickModeList;
	NameSpecifier	*analysisOutputModeList;
	EarObjectPtr	audModel;
	EarObjectPtr	stimulus;
	EarObjectPtr	setLevel;
	EarObjectPtr	analysis;
	EarObjectPtr	stimulusAnalysis;
	EarObjectPtr	gate;
	EarObjectPtr	resultEarObj;

} CGFFPars, *CGFFParsPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

extern	CGFFParsPtr	cGFFParsPtr;

__END_DECLS

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckData_ClickGainFreqFuncs(EarObjectPtr data);

BOOLN	CheckPars_ClickGainFreqFuncs(void);

void	FreeProcessVariables_PhaseFreqFunc(void);

BOOLN	Free_ClickGainFreqFuncs(void);

BOOLN	InitAnalysisOutputModeList_ClickGainFreqFuncs(void);

BOOLN	InitClickModeList_ClickGainFreqFuncs(void);

BOOLN	InitProcessVariables_ClickGainFreqFuncs(EarObjectPtr data);

BOOLN	Init_ClickGainFreqFuncs(ParameterSpecifier parSpec);

BOOLN	PrintPars_ClickGainFreqFuncs(void);

BOOLN	ReadPars_ClickGainFreqFuncs(FILE *fp);

BOOLN	RunProcess_ClickGainFreqFuncs(EarObjectPtr data);

BOOLN	SetAnalysisOutputMode_ClickGainFreqFuncs(char * theAnalysisOutputMode);

BOOLN	SetClickMode_ClickGainFreqFuncs(char * theClickMode);

BOOLN	SetClickTime_ClickGainFreqFuncs(double theClickTime);

BOOLN	SetDeltaIntensity_ClickGainFreqFuncs(double theDeltaIntensity);

BOOLN	SetEnabled_ClickGainFreqFuncs(char * theEnabled);

BOOLN	SetInitialIntensity_ClickGainFreqFuncs(double theInitialIntensity);

BOOLN	SetNumIntensities_ClickGainFreqFuncs(int theNumIntensities);

BOOLN	SetOutputMode_ClickGainFreqFuncs(char * theOutputMode);

BOOLN	SetPars_ClickGainFreqFuncs(char * enabled, char * outputMode,
		  char * clickMode, char * analysisOutputMode, double clickTime,
		  double dBAdjustment, double stimulusDuration,
		  double samplingInterval, int numIntensities, double
		  initialIntensity, double deltaIntensity);

BOOLN	SetDBAdjustment_ClickGainFreqFuncs(double theDBAdjustment);

BOOLN	SetSamplingInterval_ClickGainFreqFuncs(double theSamplingInterval);

BOOLN	SetStimulusDuration_ClickGainFreqFuncs(double theStimulusDuration);

BOOLN	SetUniParList_ClickGainFreqFuncs(UniParPtr pars);

__END_DECLS

#endif
