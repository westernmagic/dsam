/**********************
 *
 * File:		UtPadSignal.h
 * Purpose:		This module produces an output signal which has been padded
 *				at the beginning and end of the signal with a specified value.
 * Comments:	Written using ModuleProducer version 1.3.3 (Oct  1 2001).
 * Author:		L. P. O'Mard
 * Created:		14 Mar 2002
 * Updated:
 * Copyright:	(c) 2002, CNBH, University of Essex
 *
 *********************/

#ifndef _UTPADSIGNAL_H
#define _UTPADSIGNAL_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define UTILITY_PADSIGNAL_NUM_PARS			4

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	UTILITY_PADSIGNAL_BEGINDURATION,
	UTILITY_PADSIGNAL_BEGINVALUE,
	UTILITY_PADSIGNAL_ENDDURATION,
	UTILITY_PADSIGNAL_ENDVALUE

} PadSignalParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	Float	beginDuration;
	Float	beginValue;
	Float	endDuration;
	Float	endValue;

	/* Private members */
	UniParListPtr	parList;
	ChanLen	beginDurationIndex, endDurationIndex;

} PadSignal, *PadSignalPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	PadSignalPtr	padSignalPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckData_Utility_PadSignal(EarObjectPtr data);

BOOLN	Free_Utility_PadSignal(void);

UniParListPtr	GetUniParListPtr_Utility_PadSignal(void);

BOOLN	InitModule_Utility_PadSignal(ModulePtr theModule);

BOOLN	Init_Utility_PadSignal(ParameterSpecifier parSpec);

BOOLN	PrintPars_Utility_PadSignal(void);

BOOLN	Process_Utility_PadSignal(EarObjectPtr data);

BOOLN	SetBeginDuration_Utility_PadSignal(Float theBeginDuration);

BOOLN	SetBeginValue_Utility_PadSignal(Float theBeginValue);

BOOLN	SetEndDuration_Utility_PadSignal(Float theEndDuration);

BOOLN	SetEndValue_Utility_PadSignal(Float theEndValue);

BOOLN	SetParsPointer_Utility_PadSignal(ModulePtr theModule);

BOOLN	SetUniParList_Utility_PadSignal(void);

__END_DECLS

#endif
