/**********************
 *
 * File:		UtSwapLR.h
 * Purpose:		This utility module swaps the left and right channels of
 *				binarual signals.
 * Comments:	Written using ModuleProducer version 1.3.3 (Oct  1 2001).
 * Author:		L. P. O'Mard
 * Created:		25 Jan 2002
 * Updated:	
 * Copyright:	(c) 2002, CNBH, University of Essex.
 *
 *********************/

#ifndef _UTSWAPLR_H
#define _UTSWAPLR_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	Process_Utility_SwapLR(EarObjectPtr data);

BOOLN	CheckData_Utility_SwapLR(EarObjectPtr data);

BOOLN	InitModule_Utility_SwapLR(ModulePtr theModule);

BOOLN	SetParsPointer_Utility_SwapLR(ModulePtr theModule);

BOOLN	SetUniParList_Utility_SwapLR(void);

__END_DECLS

#endif
