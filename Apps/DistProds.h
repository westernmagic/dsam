/**********************
 *
 * File:		DistProds.h
 * Purpose:		
 * Comments:	Written using ModuleProducer version 1.2.9 (Aug 10 2000).
 * Author:		
 * Created:		22 Aug 2000
 * Updated:	
 * Copyright:	(c) 2000, 
 *
 *********************/

#ifndef _DISTPRODS_H
#define _DISTPRODS_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define DISTPRODS_NUM_PARS			7

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	DISTPRODS_ENABLED,
	DISTPRODS_OUTPUTMODE,
	DISTPRODS_F1FREQUENCY,
	DISTPRODS_F2SCALE,
	DISTPRODS_INTENSITY,
	DISTPRODS_STIMULUSDURATION,
	DISTPRODS_SAMPLINGINTERVAL

} DPParsParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	enabledFlag, outputModeFlag, f1FrequencyFlag, f2ScaleFlag;
	BOOLN	intensityFlag, stimulusDurationFlag, dtFlag;
	int		enabled;
	int		outputMode;
	double	f1Frequency;
	double	f2Scale;
	double	intensity;
	double	stimulusDuration;
	double	dt;

	/* Private members */
	char	outputFile[MAX_FILE_PATH];
	NameSpecifier	*outputModeList;
	EarObjectPtr	audModel;
	EarObjectPtr	stimulus;
	EarObjectPtr	analysis;

} DPPars, *DPParsPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

extern	DPParsPtr	dPParsPtr;

__END_DECLS

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckData_DistProds(EarObjectPtr data);

BOOLN	CheckPars_DistProds(void);

void	FreeProcessVariables_DistProds(void);

BOOLN	Free_DistProds(void);

BOOLN	InitAnalysisProcessList_DistProds(void);

BOOLN	Init_DistProds(ParameterSpecifier parSpec);

BOOLN	PrintPars_DistProds(void);

BOOLN	ReadPars_DistProds(FILE *fp);

BOOLN	RunProcess_DistProds(EarObjectPtr data);

BOOLN	SetEnabled_DistProds(char * theEnabled);

BOOLN	SetF1Frequency_DistProds(double theF1Frequency);

BOOLN	SetF2Scale_DistProds(double theF2Scale);

BOOLN	SetIntensity_DistProds(double theIntensity);

BOOLN	SetOutputMode_DistProds(char * theOutputMode);

BOOLN	SetPars_DistProds(char * enabled, char * outputMode,
		  double f1Frequency, double f2Scale, double intensity,
		  double stimulusDuration, double samplingInterval);

BOOLN	SetSamplingInterval_DistProds(double theSamplingInterval);

BOOLN	SetStimulusDuration_DistProds(double theStimulusDuration);

BOOLN	SetUniParList_DistProds(UniParPtr pars);

__END_DECLS

#endif
