/**********************
 *
 * File:		AnSpikeReg.h
 * Purpose:		This module carries out a spike regularity analysis,
 *				calculating the mean, standard deviation and covariance
 *				measures for a spike train.
 *				The results for each channel are stored in the order:
 *				 - mean, standard deviation, covariance.
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 *				See Hewitt M. J. & Meddis R. (1993) "Regularity of cochlear
 *				nucleus stellate cells: A computational Modeling study",
 *				J. of the Acoust. Soc. Am, 93, pp 3390-3399.
 * Author:		L. P. O'Mard
 * Created:		01 Apr 1996
 * Updated:		10 Jan 1997
 * Copyright:	(c) 1998, University of Essex
*
 *********************/

#ifndef _ANSPIKEREG_H
#define _ANSPIKEREG_H 1

#include "UtSpikeList.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define ANALYSIS_SPIKEREGULARITY_NUM_PARS		4
#define SPIKE_REG_NUM_RETURNS					3
#define	SPIKE_REG_MEAN							0
#define	SPIKE_REG_STANDARD_DEV					1
#define	SPIKE_REG_CO_VARIANCE					2
#define	SPIKE_REG_MIN_SPIKES_FOR_STATISTICS		2.0

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	ANALYSIS_SPIKEREGULARITY_EVENTTHRESHOLD,
	ANALYSIS_SPIKEREGULARITY_BINWIDTH,
	ANALYSIS_SPIKEREGULARITY_TIMEOFFSET,
	ANALYSIS_SPIKEREGULARITY_TIMERANGE

} SpikeRegParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;
	BOOLN	updateProcessVariablesFlag;

	BOOLN	eventThresholdFlag, binWidthFlag, timeOffsetFlag, timeRangeFlag;
	double	eventThreshold;
	double	binWidth;
	double	timeOffset;
	double	timeRange;

	/* Private members */
	UniParListPtr	parList;
	EarObjectPtr	countEarObj;
	SpikeListSpecPtr	spikeListSpec;

} SpikeReg, *SpikeRegPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	SpikeRegPtr	spikeRegPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	Calc_Analysis_SpikeRegularity(EarObjectPtr data);

BOOLN	CheckData_Analysis_SpikeRegularity(EarObjectPtr data);

BOOLN	CheckPars_Analysis_SpikeRegularity(void);

BOOLN	Free_Analysis_SpikeRegularity(void);

void	FreeProcessVariables_Analysis_SpikeRegularity(void);

UniParListPtr	GetUniParListPtr_Analysis_SpikeRegularity(void);

BOOLN	Init_Analysis_SpikeRegularity(ParameterSpecifier parSpec);

BOOLN	InitProcessVariables_Analysis_SpikeRegularity(EarObjectPtr data);

BOOLN	PrintPars_Analysis_SpikeRegularity(void);

BOOLN	ReadPars_Analysis_SpikeRegularity(char *fileName);

BOOLN	SetBinWidth_Analysis_SpikeRegularity(double theBinWidth);

BOOLN	SetEventThreshold_Analysis_SpikeRegularity(double theEventThreshold);

BOOLN	InitModule_Analysis_SpikeRegularity(ModulePtr theModule);

BOOLN	SetParsPointer_Analysis_SpikeRegularity(ModulePtr theModule);

BOOLN	SetPars_Analysis_SpikeRegularity(double eventThreshold, double binWidth,
		  double timeOffset, double timeRange);

BOOLN	SetTimeOffset_Analysis_SpikeRegularity(double theTimeOffset);

BOOLN	SetTimeRange_Analysis_SpikeRegularity(double theTimeRange);

BOOLN	SetUniParList_Analysis_SpikeRegularity(void);

__END_DECLS

#endif
