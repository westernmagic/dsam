/**********************
 *
 * File:		UtReduceDt.h
 * Purpose:		This routine produces an output signal whose sampling rate is
 *				increased by reducing the sampling interval of the input signal
 *				by a specified denominator.
 * Comments:	Written using ModuleProducer version 1.8.
 *				The length of the signal will be increased accordingly.
 *				A new output signal is created, if required, or the old output
 *				signal is overwritten.
 * Author:		
 * Created:		Dec 21 1995
 * Updated:	
 * Copyright:	(c) 
 *
**********************/

#ifndef _UTREDUCEDT_H
#define _UTREDUCEDT_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define UTILITY_REDUCEDT_NUM_PARS			1

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	UTILITY_REDUCEDT_DENOMINATOR

} ReduceDtParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	denominatorFlag;
	int		denominator;

	/* Private members */
	UniParListPtr	parList;

} ReduceDt, *ReduceDtPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	ReduceDtPtr	reduceDtPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_Utility_ReduceDt(EarObjectPtr data);

BOOLN	CheckPars_Utility_ReduceDt(void);

BOOLN	Free_Utility_ReduceDt(void);

UniParListPtr	GetUniParListPtr_Utility_ReduceDt(void);

BOOLN	Init_Utility_ReduceDt(ParameterSpecifier parSpec);

BOOLN	PrintPars_Utility_ReduceDt(void);

BOOLN	Process_Utility_ReduceDt(EarObjectPtr data);

BOOLN	ReadPars_Utility_ReduceDt(char *fileName);

BOOLN	SetDenominator_Utility_ReduceDt(int theDenominator);

BOOLN	SetPars_Utility_ReduceDt(int denominator);

BOOLN	SetUniParList_Utility_ReduceDt(void);

__END_DECLS

#endif
