/**********************
 *
 * File:		UtSample.h
 * Purpose:		This module samples a signal at selected intervals.
 * Comments:	Written using ModuleProducer version 1.10 (Oct 18 1996).
 * Author:		L. P. O'Mard
 * Created:		22 Oct 1996
 * Updated:	
 * Copyright:	(c) University of Essex
 *
 *********************/

#ifndef _UTSAMPLE_H
#define _UTSAMPLE_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define UTILITY_SAMPLE_NUM_PARS			2

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	UTILITY_SAMPLE_TIMEOFFSET,
	UTILITY_SAMPLE_SAMPLINGINTERVAL

} SampleParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	timeOffsetFlag, dtFlag;
	double	timeOffset;
	double	dt;

	/* Private members */
	UniParListPtr	parList;

} Sample, *SamplePtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	SamplePtr	samplePtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_Utility_Sample(EarObjectPtr data);

BOOLN	CheckPars_Utility_Sample(void);

BOOLN	Free_Utility_Sample(void);

UniParListPtr	GetUniParListPtr_Utility_Sample(void);

BOOLN	Init_Utility_Sample(ParameterSpecifier parSpec);

BOOLN	PrintPars_Utility_Sample(void);

BOOLN	Process_Utility_Sample(EarObjectPtr data);

BOOLN	ReadPars_Utility_Sample(char *fileName);

BOOLN	InitModule_Utility_Sample(ModulePtr theModule);

BOOLN	SetParsPointer_Utility_Sample(ModulePtr theModule);

BOOLN	SetPars_Utility_Sample(double timeOffset,
			  double samplingInterval);

BOOLN	SetSamplingInterval_Utility_Sample(double theSamplingInterval);

BOOLN	SetTimeOffset_Utility_Sample(double theTimeOffset);

BOOLN	SetUniParList_Utility_Sample(void);

__END_DECLS

#endif
