/**********************
 *
 * File:		AnHistogram.h
 * Purpose:		This program calculates period or post-stimulus time histograms.
 * Comments:	Written using ModuleProducer version 1.3.
 *				A negative binwidth will instruct instruct the process to use
 *				the previous signal's dt value.
 *				As for most of the analysis routines, this routine is not
 *				segmented mode friendly.
 *				13-02-98 LPO: Corrected time offset checking.
 *				04-03-99 LPO: Corrected the double precsision error using SGI's
 *				by introducing the 'DBL_GREATER macro'.
 * Author:		L. P. O'Mard
 * Created:		17 Nov 1995
 * Updated:		04 Mar 1999
 * Copyright:	(c) 1999, University of Essex.
 *
 *********************/

#ifndef _ANHISTOGRAM_H
#define _ANHISTOGRAM_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define ANALYSIS_HISTOGRAM_NUM_PARS				7
#define ANALYSIS_HISTOGRAM_NUM_SUB_PROCESSES	1

/******************************************************************************/
/****************************** Macro definitions *****************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	ANALYSIS_HISTOGRAM_DATABUFFER

} AnHistogramSubProcessSpecifier;

typedef enum {

	ANALYSIS_HISTOGRAM_DETECTIONMODE,
	ANALYSIS_HISTOGRAM_OUTPUT_MODE,
	ANALYSIS_HISTOGRAM_TYPEMODE,
	ANALYSIS_HISTOGRAM_EVENTTHRESHOLD,
	ANALYSIS_HISTOGRAM_BINWIDTH,
	ANALYSIS_HISTOGRAM_PERIOD,
	ANALYSIS_HISTOGRAM_TIMEOFFSET

} HistogramParSpecifier;

typedef enum {

	HISTOGRAM_DETECT_SPIKES,
	HISTOGRAM_CONTINUOUS,
	HISTOGRAM_DETECT_NULL

} HistogramDetectionSpecifier;

typedef enum {

	HISTOGRAM_OUTPUT_BIN_COUNTS,
	HISTOGRAM_OUTPUT_SPIKE_RATE,
	HISTOGRAM_OUTPUT_NULL

} HistogramOutputSpecifier;

typedef enum {

	HISTOGRAM_PSTH,		
	HISTOGRAM_PH,
	HISTOGRAM_TYPE_NULL

} HistogramTypeSpecifier;
		
typedef struct {

	ParameterSpecifier parSpec;
	
	BOOLN	detectionModeFlag, typeModeFlag, eventThresholdFlag, binWidthFlag;
	BOOLN	periodFlag, timeOffsetFlag, outputModeFlag;
	BOOLN	updateProcessVariablesFlag;
	int		detectionMode;
	int		outputMode;
	int		typeMode;
	double	eventThreshold;
	double	binWidth;
	double	period;
	double	timeOffset;

	/* Private members */
	NameSpecifier	*detectionModeList;
	NameSpecifier	*outputModeList;
	NameSpecifier	*typeModeList;
	UniParListPtr	parList;
	BOOLN			*riseDetected;
	double			wBinWidth, wPeriod, dt;
	ChanLen			*numPeriods, *offsetIndex, *extraSample, *bufferSamples;
	EarObjectPtr	dataBuffer;

} Histogram, *HistogramPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	HistogramPtr	histogramPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	Calc_Analysis_Histogram(EarObjectPtr data);

BOOLN	CheckData_Analysis_Histogram(EarObjectPtr data);

BOOLN	CheckPars_Analysis_Histogram(void);

BOOLN	Free_Analysis_Histogram(void);

BOOLN	FreeProcessVariables_Analysis_Histogram(void);

UniParListPtr	GetUniParListPtr_Analysis_Histogram(void);

BOOLN	Init_Analysis_Histogram(ParameterSpecifier parSpec);

BOOLN	InitProcessVariables_Analysis_Histogram(EarObjectPtr data);

BOOLN	InitDetectionModeList_Analysis_Histogram(void);

BOOLN	InitOutputModeList_Analysis_Histogram(void);

BOOLN	InitTypeModeList_Analysis_Histogram(void);

BOOLN	PrintPars_Analysis_Histogram(void);

void	PushDataBuffer_Analysis_Histogram(EarObjectPtr data,
		  ChanLen lastSamples);

BOOLN	ReadPars_Analysis_Histogram(char *fileName);

BOOLN	SetBinWidth_Analysis_Histogram(double theBinWidth);

BOOLN	SetDetectionMode_Analysis_Histogram(char *theDetectionMode);

BOOLN	SetEventThreshold_Analysis_Histogram(double theEventThreshold);

BOOLN	SetOutputMode_Analysis_Histogram(char *theOutputMode);

BOOLN	SetTimeOffset_Analysis_Histogram(double theTimeOffset);

BOOLN	InitModule_Analysis_Histogram(ModulePtr theModule);

BOOLN	SetParsPointer_Analysis_Histogram(ModulePtr theModule);

BOOLN	SetPars_Analysis_Histogram(char *detectionMode, char *typeMode,
		  double eventThreshold, double binWidth, double period,
		  double timeOffset);

BOOLN	SetPeriod_Analysis_Histogram(double thePeriod);

BOOLN	SetTypeMode_Analysis_Histogram(char *theTypeMode);

BOOLN	SetUniParList_Analysis_Histogram(void);

__END_DECLS

#endif
