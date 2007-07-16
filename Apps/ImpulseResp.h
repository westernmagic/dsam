/**********************
 *
 * File:		ImpulseResp.h
 * Purpose:		This routine generates the impulse response for different
 *				intensities.
 * Comments:	Written using ModuleProducer version 1.2.9 (Aug 10 2000).
 *				This process disables the first gating process that it finds in
 *				the model (ramp).
 * Author:		L. P. O'Mard
 * Created:		21 Aug 2000
 * Updated:	
 * Copyright:	(c) 2000, University of Essex.
 *
 *********************/

#ifndef _IMPULSERESP_H
#define _IMPULSERESP_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define IMPULSERESP_NUM_PARS		10
#define IMPULSERESP_MOD_NAME		wxT("ImpulesResp")

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	IMPULSERESP_ENABLED,
	IMPULSERESP_OUTPUTMODE,
	IMPULSERESP_CLICKTIME,
	IMPULSERESP_CLICKWIDTH,
	IMPULSERESP_REFAMPLITUDE,
	IMPULSERESP_NUMINTENSITIES,
	IMPULSERESP_INITIALINTENSITY,
	IMPULSERESP_DELTAINTENSITY,
	IMPULSERESP_STIMULUSDURATION,
	IMPULSERESP_SAMPLINGINTERVAL

} IRParsParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	enabledFlag, outputModeFlag, clickTimeFlag, clickWidthFlag;
	BOOLN	refAmplitudeFlag, numIntensitiesFlag, initialIntensityFlag;
	BOOLN	deltaIntensityFlag, stimulusDurationFlag, dtFlag;
	int		enabled;
	int		outputMode;
	double	clickTime;
	double	clickWidth;
	double	refAmplitude;
	int		numIntensities;
	double	initialIntensity;
	double	deltaIntensity;
	double	stimulusDuration;
	double	dt;

	/* Private members */
	WChar	outputFile[MAX_FILE_PATH];
	NameSpecifier	*outputModeList;
	UniParListPtr	parList;
	EarObjectPtr	audModel;
	EarObjectPtr	stimulus;
	EarObjectPtr	gate;
	EarObjectPtr	resultEarObj;

} IRPars, *IRParsPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

extern	IRParsPtr	iRParsPtr;

__END_DECLS

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckData_ImpulseResp(EarObjectPtr data);

BOOLN	CheckPars_ImpulseResp(void);

void	FreeProcessVariables_ImpulseResp(void);

BOOLN	Free_ImpulseResp(void);

BOOLN	InitModule_ImpulseResp(ModulePtr theModule);

BOOLN	InitProcessVariables_ImpulseResp(EarObjectPtr data);

BOOLN	Init_ImpulseResp(ParameterSpecifier parSpec);

BOOLN	PrintPars_ImpulseResp(void);

BOOLN	RunProcess_ImpulseResp(EarObjectPtr data);

BOOLN	SetClickTime_ImpulseResp(double theClickTime);

BOOLN	SetClickWidth_ImpulseResp(double theClickWidth);

BOOLN	SetDeltaIntensity_ImpulseResp(double theDeltaIntensity);

BOOLN	SetEnabled_ImpulseResp(WChar * theEnabled);

BOOLN	SetInitialIntensity_ImpulseResp(double theInitialIntensity);

BOOLN	SetNumIntensities_ImpulseResp(int theNumIntensities);

BOOLN	SetOutputMode_ImpulseResp(WChar * theOutputMode);

BOOLN	SetRefAmplitude_ImpulseResp(double theRefAmplitude);

BOOLN	SetSamplingInterval_ImpulseResp(double theSamplingInterval);

BOOLN	SetStimulusDuration_ImpulseResp(double theStimulusDuration);

BOOLN	SetUniParList_ImpulseResp(void);

__END_DECLS

#endif
