/**********************
 *
 * File:		AnSpikeReg.h
 * Purpose:		This module carries out a spike regularity analysis,
 *				calculating the mean, standard deviation and covariance
 *				measures for a spike train.
 *				The results for each channel are stored in the order:
 *				 - mean, standard deviation, covariance'.
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 *				See Hewitt M. J. & Meddis R. (1993) "Regularity of cochlear
 *				nucleus stellate cells: A computational Modeling study",
 *				J. of the Acoust. Soc. Am, 93, pp 3390-3399.
 *				If the standard deviation results are only valid if the 
 *				covariance measure is greater than 0.  This enables the case
 *				when there are less than two counts to be marked.
 *				10-1-97: LPO - added dead-time correction for
 *				covariance CV' = S.D. / (mean - dead time) - see Rothman J. S.
 *				Young E. D. and Manis P. B. "Convergence of Auditory Nerve
 *				Fibers in the Ventral Cochlear Nucleus: Implications of a
 *				Computational Model" J. of NeuroPhysiology, 70:2562-2583.
 *				04-01-05: LPO: The 'countEarObj' EarObject does need to be
 *				registered as a subprocess for the thread processing because
 *				access to the channels is controlled by the main output process
 *				channel access.
 * Author:		L. P. O'Mard
 * Created:		01 Apr 1996
 * Updated:		20 Feb 1997
 * Copyright:	(c) 1998, University of Essex
*
 *********************/

#ifndef _ANSPIKEREG_H
#define _ANSPIKEREG_H 1

#include "UtSpikeList.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define ANALYSIS_SPIKEREGULARITY_NUM_PARS		5
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
	ANALYSIS_SPIKEREGULARITY_WINDOWWIDTH,
	ANALYSIS_SPIKEREGULARITY_TIMEOFFSET,
	ANALYSIS_SPIKEREGULARITY_TIMERANGE,
	ANALYSIS_SPIKEREGULARITY_DEADTIME

} SpikeRegParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;
	BOOLN	updateProcessVariablesFlag;

	BOOLN	eventThresholdFlag, windowWidthFlag, timeOffsetFlag, timeRangeFlag;
	BOOLN	deadTimeFlag;
	double	eventThreshold;
	double	windowWidth;
	double	timeOffset;
	double	timeRange;
	double	deadTime;

	/* Private members */
	UniParListPtr	parList;
	double	dt, convertDt;
	ChanLen	runningTimeOffsetIndex;
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

void	ResetStatistics_Analysis_SpikeRegularity(EarObjectPtr data);

BOOLN	SetDeadTime_Analysis_SpikeRegularity(double theDeadTime);

BOOLN	SetWindowWidth_Analysis_SpikeRegularity(double theWindowWidth);

BOOLN	SetEventThreshold_Analysis_SpikeRegularity(double theEventThreshold);

BOOLN	InitModule_Analysis_SpikeRegularity(ModulePtr theModule);

BOOLN	SetParsPointer_Analysis_SpikeRegularity(ModulePtr theModule);

BOOLN	SetPars_Analysis_SpikeRegularity(double eventThreshold,
		  double windowWidth, double timeOffset, double timeRange);

BOOLN	SetTimeOffset_Analysis_SpikeRegularity(double theTimeOffset);

BOOLN	SetTimeRange_Analysis_SpikeRegularity(double theTimeRange);

BOOLN	SetUniParList_Analysis_SpikeRegularity(void);

__END_DECLS

#endif
