/**********************
 *
 * File:		AnConvolve.h
 * Purpose:		This routine creates convoluted signal from two EarObject's
 *				output signals.
 * Comments:	Written using ModuleProducer version 1.9 (May 27 1996).
 *				The output signal will be monaural or binural (with interleaved
 *				channels: LRLRLR) depending upon the format of the two input
 *				signals, which will both be the same.
 *				The convolution is defined as (R*S)[j] = sum(j = 0, m - 1)
 *				{ S[j-k]*R[k]}.
 * Author:		Lowel O'Mard and Enrique Lopez-Poveda.
 * Created:		27 May 1996
 * Updated:	
 * Copyright:	(c) 1998, University of Essex
 *
 *********************/

#ifndef _ANCONVOLVE_H
#define _ANCONVOLVE_H 1

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

BOOLN	Calc_Analysis_Convolution(EarObjectPtr data);

BOOLN	CheckData_Analysis_Convolution(EarObjectPtr data);

__END_DECLS

#endif
