/**********************
 *
 * File:		UtCreateBinaural.h
 * Purpose:		This routine creates an interleaved binaural signal from two
 *				EarObject's output signals.
 * Comments:	Written using ModuleProducer version 1.9.
 *				The output signal channels will be interleaved: LRLRLR... with
 *				inSignal[0] and inSignal[1] as left (L) and right (R)
 *				respectively.  If the two signals are already binaural, then
 *				they will be merged into a single binaural signal.
 * Author:		L. P. O'Mard
 * Created:		Feb 14 1996
 * Updated:	
 * Copyright:	(c) Loughborough University of Technology
 *
 *********************/

#ifndef _UTCREATEBINAURAL_H
#define _UTCREATEBINAURAL_H 1

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

BOOLN	CheckData_Utility_CreateBinaural(EarObjectPtr data);

BOOLN	Process_Utility_CreateBinaural(EarObjectPtr data);

__END_DECLS

#endif
