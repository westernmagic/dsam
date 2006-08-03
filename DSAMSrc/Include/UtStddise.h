/**********************
 *
 * File:		UtStddise.h
 * Purpose:		This routine Standardises a signal: sum(Si) = 0,
 *			 	sum(Si * Si)/N = 1.
 * Comments:	Written using ModuleProducer version 1.9 (May 27 1996).
 *				Reference (LUT:542b)
 *				The method used uses the standard deviation and mean of the
 *				data i.e sd = sum{(x - mean)^2 / n). The standardised values,
 *				Si = (X - mean) / sd.
 * Author:		L. P. O'Mard
 * Created:		27 May 1996
 * Updated:	
 * Copyright:	(c) 2006, L. P. O'Mard
 *
 *********************/

#ifndef _UTSTDDISE_H
#define _UTSTDDISE_H 1

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

BOOLN	CheckData_Utility_Standardise(EarObjectPtr data);

BOOLN	Process_Utility_Standardise(EarObjectPtr data);

BOOLN	InitModule_Utility_Standardise(ModulePtr theModule);

BOOLN	SetParsPointer_Utility_Standardise(ModulePtr theModule);

__END_DECLS

#endif
