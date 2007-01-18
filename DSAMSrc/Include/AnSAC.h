/**********************
 *
 * File:		AnSAC.h
 * Purpose:		This module calculates a shuffled autocorrelogram from the
 * 				input signal channels: Dries H. G. Louage, Marcel van der
 * 				Heijden, and Philip X. Joris, (2004) "Temporal Properties of
 * 				Responses to Broadband Noise in the Auditory Nerve",
 * 				J. Neurophysiol. 91:2051-2065.
 * Comments:	Written using ModuleProducer version 1.5.0 (Jan 17 2007).
 * Author:		L. P. O'Mard
 * Created:		17 Jan 2007
 * Updated:	
 * Copyright:	(c) 2007, Lowel P. O'Mard
 *
 *********************/

#ifndef _ANSAC_H
#define _ANSAC_H 1

#include "UtSpikeList.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define ANALYSIS_SAC_MOD_NAME			wxT("ANA_SAC")
#define ANALYSIS_SAC_NUM_PARS			4

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	ANALYSIS_SAC_ORDER,
	ANALYSIS_SAC_EVENTTHRESHOLD,
	ANALYSIS_SAC_MAXINTERVAL,
	ANALYSIS_SAC_BINWIDTH

} SACParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	orderFlag, eventThresholdFlag, maxIntervalFlag, binWidthFlag;
	BOOLN	updateProcessVariablesFlag;
	int		order;
	double	eventThreshold;
	double	maxInterval;
	double	binWidth;

	/* Private members */
	UniParListPtr	parList;
	SpikeListSpecPtr	spikeListSpec;
	int		maxSpikes;
	ChanLen	maxIntervalIndex;

} SAC, *SACPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	SACPtr	sACPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	Calc_Analysis_SAC(EarObjectPtr data);

BOOLN	CheckData_Analysis_SAC(EarObjectPtr data);

BOOLN	CheckPars_Analysis_SAC(void);

BOOLN	FreeProcessVariables_Analysis_SAC(void);

BOOLN	Free_Analysis_SAC(void);

UniParListPtr	GetUniParListPtr_Analysis_SAC(void);

BOOLN	InitModule_Analysis_SAC(ModulePtr theModule);

BOOLN	InitProcessVariables_Analysis_SAC(EarObjectPtr data);

BOOLN	Init_Analysis_SAC(ParameterSpecifier parSpec);

BOOLN	PrintPars_Analysis_SAC(void);

void	ResetProcess_Analysis_SAC(EarObjectPtr data);

BOOLN	SetBinWidth_Analysis_SAC(double theBinWidth);

BOOLN	SetEventThreshold_Analysis_SAC(double theEventThreshold);

BOOLN	SetMaxInterval_Analysis_SAC(double theMaxInterval);

BOOLN	SetOrder_Analysis_SAC(int theOrder);

BOOLN	SetParsPointer_Analysis_SAC(ModulePtr theModule);

BOOLN	SetUniParList_Analysis_SAC(void);

__END_DECLS

#endif
