/**********************
 *
 * File:		AnSynchIndex.h
 * Purpose:		Calculates the synchronisation index (vector strength) from
 *				the input signal.
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 *				The input is expected to come from a period histogram (PH).
 * Author:		L. P. O'Mard
 * Created:		1 Mar 1996
 * Updated:	
 * Copyright:	(c) Loughborough University
 *
 *********************/

#ifndef _ANSYNCHINDEX_H
#define _ANSYNCHINDEX_H 1

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

BOOLN	Calc_Analysis_SynchIndex(EarObjectPtr data);

BOOLN	CheckData_Analysis_SynchIndex(EarObjectPtr data);

__END_DECLS

#endif
