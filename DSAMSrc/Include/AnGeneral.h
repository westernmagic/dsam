/**********************
 *
 * File:		AnGeneral.h
 * Purpose:		This module contains various general analysis routines.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		
 * Copyright:	(c) 1998, University of Essex
 *
 **********************/

#ifndef	_ANGENERAL_H
#define _ANGENERAL_H	1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define	ANALYSIS_SINGLE_CHANNEL	1		/* For single channel routines. */
#define	ANALYSIS_CHANNEL		0		/* For single channel routines. */
    
/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

double	EuclideanDistance_GenAnalysis(double *arrayA, double *arrayB,
		  ChanLen startIndex, ChanLen endIndex);

BOOLN	LinearRegression_GenAnalysis(double *y0, double *gradient, double *y,
		  double dx, ChanLen startIndex, ChanLen length);

__END_DECLS

#endif
