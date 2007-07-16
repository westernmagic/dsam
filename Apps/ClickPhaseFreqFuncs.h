/**********************
 *
 * File:		ClickPhaseFreqFuncs.h
 * Purpose:		Calculates the click phase/frequency response using an FFT.
 * Comments:	Written using ModuleProducer version 1.2.10 (Jan  9 2001).
 * Author:		L. P. O'Mard
 * Created:		19 Jan 2001
 * Updated:	
 * Copyright:	(c) 2001, CNBH, University of Essex.
 *
 *********************/

#ifndef _CLICKPHASEFREQFUNCS_H
#define _CLICKPHASEFREQFUNCS_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define CLICKPHASEFREQFUNCS_NUM_PARS			11

#define	DEFAULT_CLICK_MAGNITUDE				964.29

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	CLICKPHASEFREQFUNCS_ENABLED,
	CLICKPHASEFREQFUNCS_OUTPUTMODE,
	CLICKPHASEFREQFUNCS_CLICKMODE,
	CLICKPHASEFREQFUNCS_ANALYSISOUTPUTMODE,
	CLICKPHASEFREQFUNCS_CLICKTIME,
	CLICKPHASEFREQFUNCS_STIMULUSDURATION,
	CLICKPHASEFREQFUNCS_SAMPLINGINTERVAL,
	CLICKPHASEFREQFUNCS_DBADJUSTMENT,
	CLICKPHASEFREQFUNCS_NUMINTENSITIES,
	CLICKPHASEFREQFUNCS_INITIALINTENSITY,
	CLICKPHASEFREQFUNCS_DELTAINTENSITY

} CPFFParsParSpecifier;

typedef enum {

	CLICKPHASEFREQFUNCS_CLICKMODE_SIMPLE,
	CLICKPHASEFREQFUNCS_CLICKMODE_FILE_MODE,
	CLICKPHASEFREQFUNCS_CLICKMODE_NULL

} ClickPhaseFreqFuncsClickModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	enabledFlag, outputModeFlag, clickModeFlag, analysisOutputModeFlag;
	BOOLN	clickTimeFlag, stimulusDurationFlag, dtFlag, dBAdjustmentFlag;
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

} CPFFPars, *CPFFParsPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

extern	CPFFParsPtr	cPFFParsPtr;

__END_DECLS

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckData_ClickPhaseFreqFuncs(EarObjectPtr data);

BOOLN	CheckPars_ClickPhaseFreqFuncs(void);

void	FreeProcessVariables_PhaseFreqFunc(void);

BOOLN	Free_ClickPhaseFreqFuncs(void);

BOOLN	InitClickModeList_ClickPhaseFreqFuncs(void);

BOOLN	InitProcessVariables_ClickPhaseFreqFuncs(EarObjectPtr data);

BOOLN	Init_ClickPhaseFreqFuncs(ParameterSpecifier parSpec);

BOOLN	PrintPars_ClickPhaseFreqFuncs(void);

BOOLN	ReadPars_ClickPhaseFreqFuncs(FILE *fp);

BOOLN	RunProcess_ClickPhaseFreqFuncs(EarObjectPtr data);

BOOLN	SetAnalysisOutputMode_ClickPhaseFreqFuncs(
		  char * theAnalysisOutputMode);

BOOLN	SetClickMode_ClickPhaseFreqFuncs(char * theClickMode);

BOOLN	SetClickTime_ClickPhaseFreqFuncs(double theClickTime);

BOOLN	SetDBAdjustment_ClickPhaseFreqFuncs(double theDBAdjustment);

BOOLN	SetDeltaIntensity_ClickPhaseFreqFuncs(double theDeltaIntensity);

BOOLN	SetEnabled_ClickPhaseFreqFuncs(char * theEnabled);

BOOLN	SetInitialIntensity_ClickPhaseFreqFuncs(double theInitialIntensity);

BOOLN	SetNumIntensities_ClickPhaseFreqFuncs(int theNumIntensities);

BOOLN	SetOutputMode_ClickPhaseFreqFuncs(char * theOutputMode);

BOOLN	SetPars_ClickPhaseFreqFuncs(char * enabled, char * outputMode,
		  char * clickMode, char * analysisOutputMode, double clickTime,
		  double stimulusDuration, double samplingInterval,
		  double dBAdjustment, int numIntensities, double initialIntensity,
		  double deltaIntensity);

BOOLN	SetSamplingInterval_ClickPhaseFreqFuncs(double theSamplingInterval);

BOOLN	SetStimulusDuration_ClickPhaseFreqFuncs(double theStimulusDuration);

BOOLN	SetUniParList_ClickPhaseFreqFuncs(UniParPtr pars);

__END_DECLS

#endif
