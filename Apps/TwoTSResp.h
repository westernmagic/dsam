/**********************
 *
 * File:		TwoTSResp.h
 * Purpose:		This routine generates the response of a probe tone subjected to
 *				a suppressor tone.
 * Comments:	Written using ModuleProducer version 1.2.9 (Aug 10 2000).
 * Author:		L. P. O'Mard
 * Created:		21 Aug 2000
 * Updated:	
 * Copyright:	(c) 2000, University of Essex.
 *
 *********************/

#ifndef _TWOTSRESP_H
#define _TWOTSRESP_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define TWOTSUPPRESP_NUM_PARS			10

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	TWOTSUPPRESP_ENABLED,
	TWOTSUPPRESP_OUTPUTMODE,
	TWOTSUPPRESP_PROBEFREQ,
	TWOTSUPPRESP_PROBEINTENSITY,
	TWOTSUPPRESP_SUPPRESSORFREQ,
	TWOTSUPPRESP_NUMSUPPRINTENSITIES,
	TWOTSUPPRESP_INITIALSUPPRINTENSITY,
	TWOTSUPPRESP_DELTASUPPRINTENSITY,
	TWOTSUPPRESP_STIMULUSDURATION,
	TWOTSUPPRESP_SAMPLINGINTERVAL

} TTSReParsParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	enabledFlag, outputModeFlag, probeFreqFlag, probeIntensityFlag;
	BOOLN	suppressorFreqFlag, numSupprIntensitiesFlag;
	BOOLN	initialSupprIntensityFlag, deltaSupprIntensityFlag;
	BOOLN	stimulusDurationFlag, dtFlag;
	int		enabled;
	int		outputMode;
	double	probeFreq;
	double	probeIntensity;
	double	suppressorFreq;
	int		numSupprIntensities;
	double	initialSupprIntensity;
	double	deltaSupprIntensity;
	double	stimulusDuration;
	double	dt;

	/* Private members */
	char	outputFile[MAX_FILE_PATH];
	NameSpecifier	*outputModeList;
	EarObjectPtr	audModel;
	EarObjectPtr	stimulus;
	EarObjectPtr	resultEarObj;

} TTSRePars, *TTSReParsPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

extern	TTSReParsPtr	tTSReParsPtr;

__END_DECLS

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckData_TwoTSuppResp(EarObjectPtr data);

BOOLN	CheckPars_TwoTSuppResp(void);

void	FreeProcessVariables_TwoTSuppResp(void);

BOOLN	Free_TwoTSuppResp(void);

BOOLN	InitProcessVariables_TwoTSuppResp(EarObjectPtr data);

BOOLN	Init_TwoTSuppResp(ParameterSpecifier parSpec);

BOOLN	PrintPars_TwoTSuppResp(void);

BOOLN	ReadPars_TwoTSuppResp(FILE *fp);

BOOLN	RunProcess_TwoTSuppResp(EarObjectPtr data);

BOOLN	SetDeltaSupprIntensity_TwoTSuppResp(double theDeltaSupprIntensity);

BOOLN	SetEnabled_TwoTSuppResp(char * theEnabled);

BOOLN	SetInitialSupprIntensity_TwoTSuppResp(
		  double theInitialSupprIntensity);

BOOLN	SetNumSupprIntensities_TwoTSuppResp(int theNumSupprIntensities);

BOOLN	SetOutputMode_TwoTSuppResp(char * theOutputMode);

BOOLN	SetPars_TwoTSuppResp(char * enabled, char * outputMode,
		  double probeFreq, double probeIntensity, double suppressorFreq,
		  int numSupprIntensities, double initialSupprIntensity,
		  double deltaSupprIntensity, double stimulusDuration,
		  double samplingInterval);

BOOLN	SetProbeFreq_TwoTSuppResp(double theProbeFreq);

BOOLN	SetProbeIntensity_TwoTSuppResp(double theProbeIntensity);

BOOLN	SetSamplingInterval_TwoTSuppResp(double theSamplingInterval);

BOOLN	SetStimulusDuration_TwoTSuppResp(double theStimulusDuration);

BOOLN	SetSuppressorFreq_TwoTSuppResp(double theSuppressorFreq);

BOOLN	SetUniParList_TwoTSuppResp(UniParPtr pars);

__END_DECLS

#endif
