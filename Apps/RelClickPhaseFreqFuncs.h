/**********************
 *
 * File:		RelClickPhaseFreqFuncs.h
 * Purpose:		Calculates the click relative phase/frequency response using an
 *				FFT.
 * Comments:	Written using ModuleProducer version 1.2.11 (Jan 30 2001).
 * Author:		L. P. O'Mard
 * Created:		30 Jan 2001
 * Updated:	
 * Copyright:	(c) 2001, CNBH, University of Essex.
 *
 *********************/

#ifndef _RELCLICKPHASEFREQFUNCS_H
#define _RELCLICKPHASEFREQFUNCS_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define RELCLICKPHASEFREQFUNCS_NUM_PARS			12

#define	RELCLICK_DEFAULT_CLICK_MAGNITUDE				964.29

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	RELCLICKPHASEFREQFUNCS_ENABLED,
	RELCLICKPHASEFREQFUNCS_OUTPUTMODE,
	RELCLICKPHASEFREQFUNCS_CLICKMODE,
	RELCLICKPHASEFREQFUNCS_ANALYSISOUTPUTMODE,
	RELCLICKPHASEFREQFUNCS_CLICKTIME,
	RELCLICKPHASEFREQFUNCS_STIMULUSDURATION,
	RELCLICKPHASEFREQFUNCS_SAMPLINGINTERVAL,
	RELCLICKPHASEFREQFUNCS_DBADJUSTMENT,
	RELCLICKPHASEFREQFUNCS_NUMINTENSITIES,
	RELCLICKPHASEFREQFUNCS_INITIALINTENSITY,
	RELCLICKPHASEFREQFUNCS_DELTAINTENSITY,
	RELCLICKPHASEFREQFUNCS_REFINTENSITY

} RCPFFParsParSpecifier;

typedef enum {

	RELCLICKPHASEFREQFUNCS_CLICKMODE_SIMPLE,
	RELCLICKPHASEFREQFUNCS_CLICKMODE_FILE_MODE,
	RELCLICKPHASEFREQFUNCS_CLICKMODE_NULL

} RelClickPhaseFreqFuncsClickModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	enabledFlag, outputModeFlag, clickModeFlag, analysisOutputModeFlag;
	BOOLN	clickTimeFlag, stimulusDurationFlag, dtFlag, dBAdjustmentFlag;
	BOOLN	numIntensitiesFlag, initialIntensityFlag, deltaIntensityFlag;
	BOOLN	refIntensityFlag;
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
	double	refIntensity;

	/* Private members */
	char	outputFile[MAX_FILE_PATH];
	char	stimFileName[MAX_FILE_PATH];
	int		refIntIndex;
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

} RCPFFPars, *RCPFFParsPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

extern	RCPFFParsPtr	rCPFFParsPtr;

__END_DECLS

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckData_RelClickPhaseFreqFuncs(EarObjectPtr data);

BOOLN	CheckPars_RelClickPhaseFreqFuncs(void);

void	FreeProcessVariables_RelClickPhaseFreqFuncs(void);

BOOLN	Free_RelClickPhaseFreqFuncs(void);

BOOLN	InitAnalysisOutputModeList_RelClickPhaseFreqFuncs(void);

BOOLN	InitClickModeList_RelClickPhaseFreqFuncs(void);

BOOLN	InitProcessVariables_ClickPhaseFreqFuncs(EarObjectPtr data);

BOOLN	Init_RelClickPhaseFreqFuncs(ParameterSpecifier parSpec);

BOOLN	PrintPars_RelClickPhaseFreqFuncs(void);

BOOLN	ReadPars_RelClickPhaseFreqFuncs(FILE *fp);

BOOLN	RunProcess_RelClickPhaseFreqFuncs(EarObjectPtr data);

BOOLN	SetAnalysisOutputMode_RelClickPhaseFreqFuncs(
		  char * theAnalysisOutputMode);

BOOLN	SetClickMode_RelClickPhaseFreqFuncs(char * theClickMode);

BOOLN	SetClickTime_RelClickPhaseFreqFuncs(double theClickTime);

BOOLN	SetDBAdjustment_RelClickPhaseFreqFuncs(double theDBAdjustment);

BOOLN	SetDeltaIntensity_RelClickPhaseFreqFuncs(double theDeltaIntensity);

BOOLN	SetEnabled_RelClickPhaseFreqFuncs(char * theEnabled);

BOOLN	SetInitialIntensity_RelClickPhaseFreqFuncs(
		  double theInitialIntensity);

BOOLN	SetNumIntensities_RelClickPhaseFreqFuncs(int theNumIntensities);

BOOLN	SetOutputMode_RelClickPhaseFreqFuncs(char * theOutputMode);

BOOLN	SetPars_RelClickPhaseFreqFuncs(char * enabled, char * outputMode,
		  char * clickMode, char * analysisOutputMode, double clickTime,
		  double stimulusDuration, double samplingInterval,
		  double dBAdjustment, int numIntensities, double initialIntensity,
		  double deltaIntensity, double refIntensity);

BOOLN	SetRefIntensity_RelClickPhaseFreqFuncs(double theRefIntensity);

BOOLN	SetSamplingInterval_RelClickPhaseFreqFuncs(
		  double theSamplingInterval);

BOOLN	SetStimulusDuration_RelClickPhaseFreqFuncs(
		  double theStimulusDuration);

BOOLN	SetUniParList_RelClickPhaseFreqFuncs(UniParPtr pars);

__END_DECLS

#endif
