/**********************
 *
 * File:		AnAutoCorr.h
 * Purpose:		Auto correlation analysis routine.
 * Comments:	Written using ModuleProducer version 1.1.
 *				FiDataFile module.
 *				20-04-98 LPO: Outputs error message if an attempt is made to
 *				read files from 'stdin'.
 *				20-04-98 LPO: In segmented mode the normalisation calculation
 *				operates on the first segment only.
 *				The DBL_EPSILON was replaced by AIFF_SMALL_VALUE, because of
 *				rounding errors under glibc-2.0.6.
 *				30-06-98 LPO: Introduced use of the SignalData structure's
 *				'numWindowFrames' field.  A corresponding field has also been
 *				added to the DSAM chunk.
 *				The WriteDSAMChunk routine has been amended so that the
 *				numWindowFrames field can be updated in segment processing mode.
 *				10-11-98 LPO: Normalisation mode now uses 'NameSpecifier'.
 * Author:		L. P. O'Mard
 * Created:		14 Nov 1995
 * Updated:		10 Nov 1998
 * Copyright:	(c) 1998, University of Essex
 *
 *********************/

#ifndef _ANAUTOCORR_H
#define _ANAUTOCORR_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define ANALYSIS_ACF_NUM_PARS			6

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	ANALYSIS_ACF_NORMALISATIONMODE,
	ANALYSIS_ACF_TIMECONSTMODE,
	ANALYSIS_ACF_TIMEOFFSET,
	ANALYSIS_ACF_TIMECONSTANT,
	ANALYSIS_ACF_TIMECONSTSCALE,
	ANALYSIS_ACF_MAXLAG

} AutoCorrParSpecifier;

typedef enum {

	ANALYSIS_NORM_MODE_NONE,
	ANALYSIS_NORM_MODE_STANDARD,
	ANALYSIS_NORM_MODE_UNITY,
	ANALYSIS_NORM_MODE_NULL

} AnalysisNormalisationModeSpecifier;

typedef enum {

	ANALYSIS_ACF_TIMECONSTMODE_LICKLIDER,
	ANALYSIS_ACF_TIMECONSTMODE_WIEGREBE,
	ANALYSIS_ACF_TIMECONSTMODE_NULL

} AnalysisTimeConstModeSpecifier;

typedef struct {

	ParameterSpecifier parSpec;
	
	BOOLN	normalisationModeFlag, timeConstModeFlag, timeOffsetFlag;
	BOOLN	timeConstantFlag, timeConstScaleFlag, maxLagFlag;
	BOOLN	updateProcessVariablesFlag;
	int		normalisationMode;
	int		timeConstMode;
	double	timeOffset;
	double	timeConstant;
	double	timeConstScale;
	double	maxLag;

	/* Private members */
	NameSpecifier	*normalisationModeList;
	NameSpecifier	*timeConstModeList;
	UniParListPtr	parList;
	ChanLen	timeOffsetIndex, sumLimitIndex, maxLagIndex;
	double	*exponentDt, expDecay, dt;
	

} AutoCorr, *AutoCorrPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	AutoCorrPtr	autoCorrPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	Calc_Analysis_ACF(EarObjectPtr data);

BOOLN	CheckData_Analysis_ACF(EarObjectPtr data);

BOOLN	CheckPars_Analysis_ACF(void);

BOOLN	Free_Analysis_ACF(void);

void	FreeProcessVariables_Analysis_ACF(void);

BOOLN	Init_Analysis_ACF(ParameterSpecifier parSpec);

UniParListPtr	GetUniParListPtr_Analysis_ACF(void);

BOOLN	InitNormalisationModeList_Analysis_ACF(void);

BOOLN	InitProcessVariables_Analysis_ACF(EarObjectPtr data);

BOOLN	PrintPars_Analysis_ACF(void);

BOOLN	ReadPars_Analysis_ACF(WChar *fileName);

BOOLN	SetMaxLag_Analysis_ACF(double theMaxLag);

BOOLN	InitModule_Analysis_ACF(ModulePtr theModule);

BOOLN	SetNormalisationMode_Analysis_ACF(WChar * theNormalisationMode);

BOOLN	SetParsPointer_Analysis_ACF(ModulePtr theModule);

BOOLN	SetPars_Analysis_ACF(WChar * normalisationMode, double timeOffset,
		  double timeConstant, double maxLag);

BOOLN	SetTimeConstMode_Analysis_ACF(WChar * theTimeConstMode);

BOOLN	SetTimeConstScale_Analysis_ACF(double theTimeConstScale);

BOOLN	SetTimeConstant_Analysis_ACF(double theTimeConstant);

BOOLN	SetTimeOffset_Analysis_ACF(double theTimeOffset);

BOOLN	SetUniParList_Analysis_ACF(void);

ChanLen	SunLimitIndex_Analysis_ACF(EarObjectPtr data, double timeConstant);

double	TimeConstant_Analysis_ACF(double lag);

__END_DECLS

#endif
