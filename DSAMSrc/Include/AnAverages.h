/**********************
 *
 * File:		AnAverages.h
 * Purpose:		This routine calculates the average value for each channel.
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 *				This module assumes that a negative timeRange assumes a 
 *				period to the end of the signal.
 * Author:		L. P. O'Mard
 * Created:		21 May 1996
 * Updated:	
 * Copyright:	(c) 1998, University of Essex.
 *
 *********************/

#ifndef _ANAVERAGES_H
#define _ANAVERAGES_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define ANALYSIS_AVERAGES_NUM_PARS			3

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	ANALYSIS_AVERAGES_MODE,
	ANALYSIS_AVERAGES_TIMEOFFSET,
	ANALYSIS_AVERAGES_TIMERANGE

} AveragesParSpecifier;

typedef	enum {

	AVERAGES_FULL,
	AVERAGES_NEGATIVE_WAVE,
	AVERAGES_POSITIVE_WAVE,
	AVERAGES_NULL

} AveragesModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	modeFlag, timeOffsetFlag, timeRangeFlag;
	int		mode;
	double	timeOffset;
	double	timeRange;
	
	/* Private members */
	NameSpecifier *modeList;
	UniParListPtr	parList;

} Averages, *AveragesPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	AveragesPtr	averagesPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	Calc_Analysis_Averages(EarObjectPtr data);

BOOLN	CheckData_Analysis_Averages(EarObjectPtr data);

BOOLN	CheckPars_Analysis_Averages(void);

UniParListPtr	GetUniParListPtr_Analysis_Averages(void);

BOOLN	Free_Analysis_Averages(void);

BOOLN	Init_Analysis_Averages(ParameterSpecifier parSpec);

BOOLN	InitModeList_Analysis_Averages(void);

BOOLN	PrintPars_Analysis_Averages(void);

BOOLN	ReadPars_Analysis_Averages(char *fileName);

BOOLN	SetMode_Analysis_Averages(char *theMode);

BOOLN	SetPars_Analysis_Averages(char *mode, double timeOffset,
		  double timeRange);

BOOLN	SetTimeRange_Analysis_Averages(double theTimeRange);

BOOLN	SetTimeOffset_Analysis_Averages(double theTimeOffset);

BOOLN	SetUniParList_Analysis_Averages(void);

__END_DECLS

#endif